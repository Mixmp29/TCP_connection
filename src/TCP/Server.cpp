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