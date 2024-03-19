#include "TCP.hpp"

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

  printf("SERVER: Номер sid: %d\n", sockMain);
  printf("SERVER: IP-адресс: %d\n", ntohs(servAddr.sin_addr.s_addr));
  printf("SERVER: номер порта - % d\n\n", ntohs(servAddr.sin_port));

  listen(sockMain, NTHRDS);
}

void Server::in_work() {
  if ((sockConnect = accept(sockMain, 0, 0)) < 0) {
    perror("Неверный socket для клиента.");
    exit(1);
  }

  sockets.push_back(sockConnect);

  printf("sockClient: %d\n", sockConnect);

  if (pthread_create(&thrds[sockConnect], NULL, (void *(*)(void *))threadclient,
                     (void *)&sockConnect) < 0) {
    perror("Ошибка создания потока.");
    exit(1);
  }
}

int Server::threadclient(void *sockClient) {
  int Client = *((int *)sockClient);

  char recvbuf[BUFLEN];
  char sendbuf[BUFLEN];
  char name[BUFLEN];
  char message[BUFLEN];
  int msgLength;
  size_t index;
  for (int i = 1;; ++i) {
    index = 0;
    bzero(recvbuf, BUFLEN);
    bzero(sendbuf, BUFLEN);
    bzero(name, BUFLEN);
    bzero(message, BUFLEN);

    // recv
    if ((msgLength = recv(Client, recvbuf, BUFLEN, 0)) < 0) {
      perror("Плохое получение потоком\n");
      printf("Итерация %d\n", i);
      printf("sockClient in pthread: %d\n", Client);
      printf("msgLength in pthread: %d\n", Client);
      exit(1);
    }

    if (msgLength == 0) {
      printf("Break in serv\n");
      break;
    }
    //

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

    // send
    send_msg(sendbuf, Client);
    //

    printf("SERVER: Socket для клиента - %d\n", Client);
    printf("SERVER: Длина сообщения - %d\n", msgLength);
    printf("SERVER: Данные от клиента - %s\n", recvbuf);
    printf("SERVER: Имя клиента - %s\n", name);
    printf("SERVER: Сообщение - %s\n\n", message);
  }

  close(Client);
  for (auto i = sockets.begin(); i < sockets.end(); ++i) {
    if (*i == Client)
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