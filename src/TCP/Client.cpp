#include <TCP/TCP.hpp>

Client::Client(char *ip, int port) {
  int sock;
  struct sockaddr_in servAddr;
  struct hostent *hp;
  pthread_t tid[2];

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Не могу получить socket\n");
    exit(1);
  }

  bzero((char *)&servAddr, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  hp = gethostbyname(ip);
  bcopy(hp->h_addr_list[0], &servAddr.sin_addr, hp->h_length);
  servAddr.sin_port = htons(port);
}