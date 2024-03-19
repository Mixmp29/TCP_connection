#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

class TCP {
  int sockMain;
  int sockConnect;
  int length;
  struct sockaddr_in servAddr;
};