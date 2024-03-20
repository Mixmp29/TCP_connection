#include <TCP/TCP.hpp>

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

  std::thread thrd1(Client::send_msg, this, sockMain);
  std::thread thrd2(Client::send_msg, this, sockMain);

  thrd1.join();
  thrd2.join();
}