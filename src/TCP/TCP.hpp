#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

class TCP {
  int sockMain;
  int sockClient;
  int length;
  struct sockaddr_in servAddr;
};