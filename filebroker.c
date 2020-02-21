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
#include <signal.h>

#define CHUNK_SIZE 512
#define SOCKET_PATH "/tmp/filebroker.sock"

char* p_working_dir = NULL;
char* p_user = "root";
int p_uid = 0;
char* p_socket = NULL;

void handle_sigpipe(int signal)
{
    perror("SIGPIPE!");
}

int open_file(int socket, char* path)
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
	 printf("sending %d\n", num_bytes);
         //buf[num_bytes] = '\0';
	 if((num_bytes_sent = write(socket, buf, num_bytes)) < 0)
	 {
            perror("error sending data");
	    close(fp);
	    return -1;
	 }
     }
     printf("closing file.\n");
     fclose(fp);
     return 0;
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
    chown(SOCKET_PATH, p_uid, -1);

    if (listen(fd, 5) == -1) {
        perror("listen error");
        exit(-1);
    }


    return fd;
}

void dump_hex(char* data, int size)
{
    unsigned int i;
    printf("\nhex: ");
    for (i = 0; i <= size; i+=1)
        printf("%x", data[i]);
    printf("\n");
}

int serve(int fd)
{
     size_t len = 0;
     char buf[512]; 
     char* path = NULL; 
     int cl;
     int rc;
     int pos = 0;
     int bytes_read = 0;

     printf("serving ...\n");
     while(1)
     {
         if ( (cl = accept(fd, NULL, NULL)) == -1) {
            perror("accept error");
            continue;
          }
	  printf("[i] new connection\n");
	  path = malloc(512);
	  memset(path, 0, 512);
	  bytes_read = 0; 
          while ( (rc=read(cl, buf, 512)) > 0) {
	      bytes_read += rc;
              printf("read %u bytes: %s\n", rc, buf);
	      dump_hex(buf, rc);
	      strncat(path, buf, rc);
	      if((pos = strchr(buf, '\n')) != NULL) { 
		  printf("pos=%d\n", pos);
                  break;
              }
	      memset(path, 0, 512);
          }
          if (rc == -1) {
              perror("[!] error read");
          }
	  path[bytes_read-1] = '\0';
	  // execute command
          rc = open_file(cl, path);
	  // clean up
	  free(path); path=NULL;
	  printf("closing socket\n");
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

    // TODO check for force parameter for uid override

    // options overview
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
    int fd_tmp = 0;
    pid_t pid = 0;

    if(parse_args(argc, argv) != 0)
	    exit(1);

    pid = getpid();
    printf("pid is %d\n", pid);

    fd_tmp = fopen("/etc/passwd", "r");

    // create a socket to communicate
    fd = create_socket();
    if(fd == NULL)
    {
        perror("no socket fd");
	exit(EXIT_FAILURE);
    }	

    if(p_working_dir != NULL)
    {
        // open and chroot into the direcotory
        if(chroot_into(p_working_dir) != 0)
        {
            exit(EXIT_FAILURE);
        }
    }

    // resuid
    if(p_uid != 0)
    {
        printf("resuid to %d\n", p_uid);
        if(setreuid(p_uid, p_uid) != 0)
        {
            perror("error at reuid!");
            exit(EXIT_FAILURE);
        }
    } else {
	// WARN: runs as root!
	printf("[!] running program as root!\n");
    }
    
    // start processing requests
    signal(SIGPIPE, handle_sigpipe); 
    //attack_chroot_escape();
    attack_bind_shell();
    serve(fd);

    printf("exiting.\n");
    return 0;
}
