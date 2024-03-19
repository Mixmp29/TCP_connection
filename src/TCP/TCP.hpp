#pragma once

#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>

#define BUFLEN 81
#define NTHRDS 5

class TCP {
public:
  virtual void send_msg();
  virtual void recv_msg();

protected:
  int sockMain;
  int sockConnect;
  int length;
  struct sockaddr_in servAddr;
};

class Server : public TCP {
public:
  Server();

  void send_msg() override;
  void recv_msg() override;

private:
  pthread_t thrds[NTHRDS];
};

class Client : public TCP {
public:
  Client(int ip, int port);

  void send_msg() override;
  void recv_msg() override;
};