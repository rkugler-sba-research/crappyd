/*
 * this server gets a request to read a static file and serves it
 * it sends the response through the socket
 * TODO fast IPC
 */

#include<stdio.h>
#include<stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>

#define CHUNK_SIZE 512
#define SOCKET_PATH "/tmp/filebroker.sock"

char* p_working_dir = NULL;
char* p_user = "root";
int p_uid = 0;
char* p_socket = NULL;

FILE* open_file(int socket, char* path)
{
     char* buf[CHUNK_SIZE];
     int num_bytes = 0, num_bytes_sent = 0;
     FILE* fp = NULL;
     // check for traversal
     // check if file exists
     // open the file, open the FP or null 
     printf("trying to open file %s\n", path);
     fp = fopen(path, "r");
     if(fp == NULL) {
	 perror("can't open file");
         return NULL;
     }
     while( (num_bytes = fread(buf, sizeof(char), CHUNK_SIZE, fp)) > 0)
     {
	 printf("num_bytes %d\n", num_bytes);
         //buf[num_bytes] = '\0';
	 if((num_bytes_sent = write(socket, buf, num_bytes)) < 0)
	 {
            perror("error sending data");
	 }
     }
     printf("closing file.");
     fclose(fp);
     return NULL;
}

int create_socket()
{
    int fd = 0;
    struct sockaddr_un sock_addr;
    size_t size;

    // init sock_addr
    memset(&sock_addr, 0, sizeof(sock_addr));

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd < 0)
    {
        perror("socket");
	exit(EXIT_FAILURE);
    }

    sock_addr.sun_family = AF_LOCAL;
    strncpy(sock_addr.sun_path, SOCKET_PATH, strlen(SOCKET_PATH));
    size = SUN_LEN(&sock_addr);

    // delete old socket
    unlink(SOCKET_PATH);

    if(bind(fd, (struct sockaddr *) &sock_addr, size) < 0)
    {
        perror("bind socket");
	exit(EXIT_FAILURE);
    }

    if (listen(fd, 5) == -1) {
        perror("listen error");
        exit(-1);
    }


    return fd;
}

int serve(int fd)
{
     size_t len = 0;
     char* buf = NULL; 
     int cl;
     int rc;
     int pos = 0;
     int bytes_read = 0;

     printf("process_request\n");
     while(1)
     {
         if ( (cl = accept(fd, NULL, NULL)) == -1) {
            perror("accept error");
            continue;
          }
	  buf = malloc(512);
	  bytes_read = 0; 
          while ( (rc=read(cl, buf, sizeof(buf))) > 0) {
              printf("read %u bytes: %.*s\n", rc, rc, buf);
	      if((pos = strchr(buf, '\n')) != NULL) { 
		  printf("pos=%d\n", pos);
                  break;
              }
          }
          if (rc == -1) {
              perror("read");
              exit(-1);
          }
	  buf[strlen(buf)-1] = '\0';
	  printf("opening %s\n", buf);
	  // execute command
          open_file(cl, buf);
	  
	  // clean up
	  free(buf); buf=NULL;
          close(cl);
     }

     // something is wrong
     return -1;
}

int parse_args(int argc, char* argv[])
{
    int c;
    struct passwd* passwd;

    opterr = 0;

    while ((c = getopt (argc, argv, "d:u:")) != -1)
    switch (c)
      {
      case 'd':
        p_working_dir = optarg;
        break;
      case 'u':
        p_user = optarg;
        passwd = getpwnam(p_user);
        if(passwd == NULL) {
    	    perror("user not found");
    	    return -1;
        } else {
    	    p_uid = passwd->pw_uid;
        }

        break;
      case '?':
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        return 1;
      default:
        abort();
    }

    printf("working_dir=%s\n", p_working_dir);
    printf("user=%s (%d)\n", p_user, p_uid);

    return 0;
}

int chroot_into(char* chroot_target_dir)
{
    printf("chrooting into %s\n", chroot_target_dir);
    chdir(chroot_target_dir);
    if (chroot(chroot_target_dir) != 0) {
        perror("error chroot");
        return 1;
    }
    return 0;
}
 

int main(int argc, char* argv[])
{
    int fd = 0;

    if(parse_args(argc, argv) != 0)
	    exit(1);

    // create a socket to communicate
    fd = create_socket();
    if(fd == NULL)
    {
        perror("no socket fd");
	exit(EXIT_FAILURE);
    }	

    // open and chroot into the direcotory
    if(chroot_into(p_working_dir) != 0)
    {
	exit(EXIT_FAILURE);
    }

    // resuid
    printf("resuid to %d\n", p_uid);
    setreuid(p_uid, p_uid);
    
    // start processing requests
    serve(fd);

    printf("exiting.\n");
    return 0;
}
