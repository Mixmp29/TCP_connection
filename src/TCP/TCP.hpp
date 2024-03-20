#pragma once

#include <cstdio>
#include <cstdlib>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

#define BUFLEN 81
#define NTHRDS 5

class TCP {
public:
  virtual void send_msg(char *, int) = 0;
  virtual void recv_msg(char *, int &, int) = 0;

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
  int threadclient(int sockClient);

  void send_msg(char *buf, int sockClient) override;
  void recv_msg(char *buf, int &msgLength, int sockClient) override;

private:
  std::vector<int> sockets;
  std::vector<std::jthread> thrds;
};

class Client : public TCP {
public:
  Client(char *ip, int port);

  void run();
  int threadsend(int sockClient);
  int threadrecv(int sockClient);

  void send_msg(char *buf, int sockClient) override;
  void recv_msg(char *buf, int &msgLength, int sockClient) override;
};