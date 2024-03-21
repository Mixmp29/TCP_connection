#include <TCP/TCP.hpp>

int main(int argc, char *argv[]) {
  if (argc == 2) {
    Server server(atoi(argv[1]));
    server.in_work();
  } else {
    Server server;
    server.in_work();
  }

  return 0;
}