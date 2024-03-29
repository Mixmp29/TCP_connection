#include <TCP/TCP.hpp>
#include <iostream>
#include <sstream>
#include <string>

Client::Client(char *ip, int port) {
  struct hostent *hp;

  if ((sockMain = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Не удалось получить socket\n");
    exit(1);
  }

  bzero((char *)&servAddr, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  hp = gethostbyname(ip);
  bcopy(hp->h_addr_list[0], &servAddr.sin_addr, hp->h_length);
  servAddr.sin_port = htons(port);
}

void Client::run() {
  if (connect(sockMain, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
    perror("Клиент не может соединиться.\n");
    exit(1);
  } else {
    printf("Успешное подключение к серверу.\n\n");
  }

  std::thread thrd1(&Client::threadsend, this, sockMain);
  std::thread thrd2(&Client::threadrecv, this, sockMain);

  thrd1.join();
  thrd2.join();
}

int Client::threadsend(int sockClient) {
  char name[BUFLEN];
  char message[BUFLEN];
  char buf[BUFLEN];
  printf("Введите своё имя: ");
  scanf("%s", name);
  printf("\nТеперь вы можете писать сообщения, %s.\n\n", name);
  strcat(name, ".");
  for (;;) {
    bzero(buf, BUFLEN);
    std::cin.getline(message, BUFLEN);
    printf("\n");
    strcat(buf, name);
    strcat(buf, message);
    if (strlen(message) > 0) {
      send_msg(buf, sockClient);
    }
  }

  close(sockClient);
  return 0;
}

int Client::threadrecv(int sockClient) {
  char buf[BUFLEN];
  int msgLength = 0;
  for (;;) {
    recv_msg(buf, msgLength, sockClient);

    if (msgLength == 0) {
      printf("\nРазрыв с сервером.\n");
      if (reconnect(sockClient) == 1)
        exit(1);
      // break;
    }

    if (check(buf) == "-parse")
      print_parse(buf);
    else if (check(buf) == "-get_users_count")
      print_usr_cnt(buf);
    else
      printf("%s\n\n", buf);
  }

  close(sockClient);
  return 0;
}

void Client::send_msg(char *buf, int sockClient) {
  if (send(sockClient, buf, BUFLEN, MSG_NOSIGNAL) < 0) {
    perror("Проблемы с пересылкой.\n");
    exit(1);
  }
}

void Client::recv_msg(char *buf, int &msgLength, int sockClient) {
  bzero(buf, BUFLEN);
  if ((msgLength = recv(sockClient, buf, BUFLEN, 0)) < 0) {
    perror("Плохое получение потоком\n");
    printf("Socket клиента в потоке: %d\n", sockClient);
    printf("Длина сообщения в потоке: %d\n", sockClient);
    exit(1);
  }
}

std::string Client::check(char *recvbuf) {
  std::string command;
  std::istringstream buf(recvbuf);

  std::getline(buf, command, '.');

  return command;
}

void Client::print_parse(char *recvbuf) {
  std::string command, string, message, count;
  std::istringstream buf(recvbuf);

  std::getline(buf, command, '.');
  std::getline(buf, string, '.');
  std::getline(buf, message, '.');
  std::getline(buf, count);

  std::istringstream val(count);
  printf("Message   %s", string.c_str());

  for (auto &symbol : message) {
    std::string cnt;
    val >> cnt;

    printf("\n%c \t  %s", symbol, cnt.c_str());
  }

  printf("\n\n");
}

void Client::print_usr_cnt(char *recvbuf) {
  std::string command, count;
  std::istringstream buf(recvbuf);

  std::getline(buf, command, '.');
  std::getline(buf, count);

  printf("\nКоличество клиентов на сервере: %s", count.c_str());

  printf("\n\n");
}

int Client::reconnect(int &sockClient) {
  for (int i = 0; i < 5; ++i) {
    printf("Попытка переподключения...\n");
    sleep(5);

    close(sockClient);
    sockClient = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sockMain, (struct sockaddr *)&servAddr, sizeof(servAddr)) >=
        0) {
      printf("\nУспешное переподключение.\n");
      return 0;
    }
  }
  printf("\nНе удалось переподключиться. Завершение сеанса.\n");
  return 1;
}