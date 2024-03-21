#include <TCP/TCP.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Server::Server() { settings(0); }

Server::Server(int port) { settings(port); }

void Server::settings(int port) {
  if ((sockMain = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Сервер не может открыть главный socket");
    exit(1);
  }

  int optval = 1;
  setsockopt(sockMain, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

  bzero((char *)&servAddr, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(port);

  if (bind(sockMain, (struct sockaddr *)&servAddr, sizeof(servAddr))) {
    perror("Связывание сервера неудачно.");
    exit(1);
  }

  length = sizeof(servAddr);
  if (getsockname(sockMain, (struct sockaddr *)&servAddr,
                  (socklen_t *)&length)) {
    perror("Вызов getsockname неудачен.");
    exit(1);
  }

  printf("SERVER: Номер sid - %d\n", sockMain);
  printf("SERVER: IP-адресс - %d\n", ntohs(servAddr.sin_addr.s_addr));
  printf("SERVER: Номер порта - %d\n\n", ntohs(servAddr.sin_port));

  listen(sockMain, NTHRDS);
}

void Server::in_work() {
  for (;;) {
    if ((sockConnect = accept(sockMain, 0, 0)) < 0) {
      perror("Неверный socket для клиента.");
      exit(1);
    }

    sockets.push_back(sockConnect);

    printf("Socket клиента: %d\n", sockConnect);

    thrds.push_back(std::jthread(&Server::threadclient, this, sockConnect));
  }
}

int Server::threadclient(int sockClient) {
  char recvbuf[BUFLEN];
  char sendbuf[BUFLEN];
  char name[BUFLEN];
  char message[BUFLEN];
  int msgLength;
  for (;;) {
    bzero(recvbuf, BUFLEN);
    bzero(sendbuf, BUFLEN);
    bzero(name, BUFLEN);
    bzero(message, BUFLEN);

    recv_msg(recvbuf, msgLength, sockClient);

    if (msgLength == 0) {
      printf("Разрыв с клиентом %d\n", sockClient);
      break;
    }

    if (check(recvbuf) == "-parse") {
      cmd = "-parse";
      parse(recvbuf, sendbuf);

      printf("SERVER: Socket для клиента - %d\n", sockClient);
      printf("SERVER: Длина сообщения - %d\n", msgLength);
      printf("SERVER: Данные для клиента - %s\n\n", sendbuf);

    } else if (check(recvbuf) == "-get_users_count") {
      cmd = "-get_users_count";
      get_usr_cnt(recvbuf, sendbuf);

      printf("SERVER: Socket для клиента - %d\n", sockClient);
      printf("SERVER: Длина сообщения - %d\n", msgLength);
      printf("SERVER: Данные для клиента - %s\n\n", sendbuf);

    } else {
      chat(recvbuf, sendbuf, name, message);

      printf("SERVER: Socket для клиента - %d\n", sockClient);
      printf("SERVER: Длина сообщения - %d\n", msgLength);
      printf("SERVER: Данные от клиента - %s\n", recvbuf);
      printf("SERVER: Имя клиента - %s\n", name);
      printf("SERVER: Сообщение - %s\n\n", message);
    }

    send_msg(sendbuf, sockClient);
  }

  close(sockClient);

  for (auto i = sockets.begin(); i < sockets.end(); ++i) {
    if (*i == sockClient)
      sockets.erase(i);
  }
  return 0;
}

void Server::send_msg(char *buf, int sockClient) {
  if (cmd == "-parse" || cmd == "-get_users_count") {
    send(sockClient, buf, BUFLEN, 0);
    cmd = "";
  } else {
    for (auto i = sockets.begin(); i < sockets.end(); ++i) {
      printf("Отправка клиенту %d\n", *i);
      if (*i != sockClient)
        send(*i, buf, BUFLEN, 0);
    }
    printf("\n\n");
  }
}

void Server::recv_msg(char *buf, int &msgLength, int sockClient) {
  if ((msgLength = recv(sockClient, buf, BUFLEN, 0)) < 0) {
    perror("Плохое получение потоком\n");
    printf("Socket клиента в потоке: %d\n", sockClient);
    printf("Длина сообщения в потоке: %d\n", sockClient);
    exit(1);
  }
}

std::string Server::check(char *recvbuf) {
  std::string name, command;
  std::istringstream buf(recvbuf);

  std::getline(buf, name, '.');
  std::getline(buf, command, ' ');

  return command;
}

void Server::parse(char *recvbuf, char *sendbuf) {
  std::string name, command, string;
  std::string message, count;
  std::string result;
  std::vector<std::pair<char, int>> map;

  std::istringstream buf(recvbuf);

  std::getline(buf, name, '.');
  std::getline(buf, command, ' ');
  std::getline(buf, string);

  for (auto &symbol : string) {
    bool repeat = false;

    for (auto &pair : map)
      if (pair.first == symbol)
        repeat = true;

    if (repeat)
      continue;
    else
      map.push_back(std::pair(symbol, 0));
  }

  for (auto &pair : map)
    for (auto &symbol : string)
      if (pair.first == symbol)
        ++pair.second;

  for (auto &[key, val] : map) {
    message += key;
    count += std::to_string(val) + " ";
  }

  result += command + '.' + string + '.' + message + '.' + count;

  strcat(sendbuf, result.c_str());
}

void Server::get_usr_cnt(char *recvbuf, char *sendbuf) {
  std::string name, command;
  std::string result;

  std::istringstream buf(recvbuf);

  std::getline(buf, name, '.');
  std::getline(buf, command, ' ');

  result += command + '.' + std::to_string(sockets.size());

  strcat(sendbuf, result.c_str());
}

void Server::chat(char *recvbuf, char *sendbuf, char *name, char *message) {
  std::string s_name, s_message;

  std::istringstream buf(recvbuf);

  std::getline(buf, s_name, '.');
  std::getline(buf, s_message);

  strcat(name, s_name.c_str());
  strcat(message, s_message.c_str());

  strcat(sendbuf, name);
  strcat(sendbuf, ": ");
  strcat(sendbuf, message);
}