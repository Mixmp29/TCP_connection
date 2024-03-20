#include <TCP/TCP.hpp>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Введите ./client имя_хоста порт\n");
    exit(1);
  }

  Client client(argv[1], atoi(argv[2]));
  client.run();
}