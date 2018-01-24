#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <signal.h>

#define N 4

struct termios tty_attr_old;

void tty_raw() {
  struct termios tty_attr;

  tcgetattr(0, &tty_attr);

  tty_attr.c_lflag &= (~(ICANON | ECHO));
  tty_attr.c_cc[CTIME] = 0;
  tty_attr.c_cc[VMIN] = 1;

  tcsetattr(0, TCSANOW, &tty_attr);
}

void ff() {
  tcsetattr(0, TCSANOW, &tty_attr_old);
  putchar('\n');
  exit(0);
}

void print_next_bit(int fd) {
  char buf[N + 1];
  int n;
  
  memset(buf, 0, N);
  
  if ((n = read(fd, buf, N)) > 0) {
    write(1, buf, n);
    memset(buf, 0, N);
  }
}

int main(int argc, char **argv) {
  char c, *file = "/usr/include/stdio.h";

  if (argc > 1)
    file = argv[1];

  tcgetattr(0, &tty_attr_old);
  tty_raw();

  signal(SIGINT, &ff);
  int fd = open(file, O_RDONLY);
  if (fd <= 0) {
    perror("open()");
    ff();
  }

  while (read(0, &c, 1)) {
    if (c == 27)
      ff();
    else
      print_next_bit(fd);
  }
  tcsetattr(0, TCSANOW, &tty_attr_old);
  
  close(fd);
  return 0;
}
