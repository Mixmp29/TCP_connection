#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

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
};

class Client : public TCP {
public:
  Client(int ip, int port);

  void send_msg() override;
  void recv_msg() override;
};