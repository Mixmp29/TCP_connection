#include <TCP/TCP.hpp>
#include <iostream>

Client::Client(char *ip, int port) {
  struct hostent *hp;

  if ((sockMain = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Не могу получить socket\n");
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
    printf("Успешное подключение к серверу.\n");
  }

  std::thread thrd1(&Client::threadsend, this, sockMain);
  std::thread thrd2(&Client::threadrecv, this, sockMain);

  thrd1.join();
  thrd2.join();
}

int Client::threadsend(int sockClient) {
  int Client = sockClient;

  char name[BUFLEN];
  char message[BUFLEN];
  char buf[BUFLEN];
  printf("What is your name?\n\n");
  scanf("%s", name);
  printf("\nYou can type message now %s\n\n", name);
  strcat(name, ".");
  for (;;) {
    bzero(buf, BUFLEN);
    std::cin.getline(message, BUFLEN);
    strcat(buf, name);
    strcat(buf, message);
    if (strlen(message) > 0) {
      // send
      send_msg(buf, Client);
      //
    }
  }

  close(Client);
  return 0;
}

int Client::threadrecv(int sockClient) {
  int Client = sockClient;

  char buf[BUFLEN];
  int msgLength = 0;
  for (int i = 1;; ++i) {
    // recv
    recv_msg(buf, msgLength, Client);
    //

    if (msgLength == 0) {
      printf("\nBreak in recv\n");
      break;
    }

    printf("\n%s\n\n", buf);
  }

  close(Client);
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
    printf("sockClient in pthread: %d\n", sockClient);
    printf("msgLength in pthread: %d\n", sockClient);
    exit(1);
  }
}