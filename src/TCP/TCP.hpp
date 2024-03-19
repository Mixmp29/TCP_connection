#pragma once

#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#define BUFLEN 81
#define NTHRDS 5

class TCP {
public:
  virtual void send_msg(char *, int);
  virtual void recv_msg(char *, int &, int);

protected:
  int sockMain;
  int sockConnect;
  int length;
  struct sockaddr_in servAddr;
};

class Server : public TCP {
public:
  Server();
  void in_work();
  int threadclient(void *sockClient);

  void send_msg(char *buf, int sockClient) override;
  void recv_msg(char *buf, int &msgLength, int sockClient) override;

private:
  std::vector<int> sockets;
  pthread_t thrds[NTHRDS];
};

class Client : public TCP {
public:
  Client(int ip, int port);

  void send_msg() override;
  void recv_msg() override;
};