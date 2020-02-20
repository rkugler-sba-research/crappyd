#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define socket_path "/tmp/filebroker.sock"

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
  char buf[100];
  int fd,rc;
  char* request_file;
  int bytes_sent = 0;

  if (argc > 1) request_file=argv[1];

  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("connect error");
    exit(-1);
  }
  
  bytes_sent = 0;
  strncat(request_file, "\n", strlen(request_file)+1);
  while(bytes_sent < strlen(request_file))
  {
      rc = write(fd, request_file, strlen(request_file));
      bytes_sent += rc;
      printf("bytes_sent=%d\n", rc);

  }

  while( (rc=read(fd, buf, 1024)) > 0) {
    if (write(2, buf, rc) != rc) {
      if (rc > 0) fprintf(stderr,"partial read");
      else {
        perror("read error");
        exit(-1);
      }
    }
  }

  return 0;
}
