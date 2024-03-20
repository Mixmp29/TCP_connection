#include <TCP/TCP.hpp>

Server::Server() {
  if ((sockMain = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Сервер не может открыть главный socket");
    exit(1);
  }

  bzero((char *)&servAddr, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = 0;

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

    chat(recvbuf, sendbuf, name, message);

    send_msg(sendbuf, sockClient);

    printf("SERVER: Socket для клиента - %d\n", sockClient);
    printf("SERVER: Длина сообщения - %d\n", msgLength);
    printf("SERVER: Данные от клиента - %s\n", recvbuf);
    printf("SERVER: Имя клиента - %s\n", name);
    printf("SERVER: Сообщение - %s\n\n", message);
  }

  close(sockClient);

  for (auto i = sockets.begin(); i < sockets.end(); ++i) {
    if (*i == sockClient)
      sockets.erase(i);
  }
  return 0;
}

void Server::send_msg(char *buf, int sockClient) {
  for (auto i = sockets.begin(); i < sockets.end(); ++i) {
    printf("Отправка клиенту %d\n", *i);
    if (*i != sockClient)
      send(*i, buf, BUFLEN, 0);
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

void Server::chat(char *recvbuf, char *sendbuf, char *name, char *message) {
  size_t index = 0;
  for (int i = 0; recvbuf[i] != '.'; ++i) {
    name[i] = recvbuf[i];
    ++index;
  }

  index++;

  for (int i = 0; index < strlen(recvbuf); ++i) {
    message[i] = recvbuf[index];
    index++;
  }

  strcat(sendbuf, name);
  strcat(sendbuf, ": ");
  strcat(sendbuf, message);
}