
#include<stdio.h>
#include<stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

#define CHUNK_SIZE 512
#define SOCKET_PATH "/tmp/filebroker.sock"

/*
 * this server gets a request to read a static file and serves it
 * it sends the response through the socket
 * TODO fast IPC
 */

FILE* open_file(char* path)
{
     char* buf[CHUNK_SIZE];
     int num_bytes = 0;
     FILE* fp = NULL;
     // check for traversal
     // check if file exists
     // open the file, open the FP or null 
     fp = fopen(path, "r");
     if(fp == NULL) {
	 perror("can't open file");
         return NULL;
     }
     while( (num_bytes = fread(buf, sizeof(char), CHUNK_SIZE, fp)) > 0)
     {
	 printf("num_bytes %d\n", num_bytes);
	 printf(buf); //TODO format string
     }
     fclose(fp);
     return NULL;
}

int create_socket()
{
    int fd = 0;
    struct sockaddr_un sock_addr;
    size_t size;

    fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(fd < 0)
    {
        perror("socket");
	exit(EXIT_FAILURE);
    }

    sock_addr.sun_family = AF_LOCAL;
    strncpy(sock_addr.sun_path, SOCKET_PATH, strlen(SOCKET_PATH));
    size = SUN_LEN(&sock_addr);

    if(bind(fd, (struct sockaddr *) &sock_addr, size) < 0)
    {
        perror("bind socket");
	exit(EXIT_FAILURE);
    }

    return fd;
}

int process_request()
{
     printf("process_request\n");
     open_file("/tmp/x");
     // something is wrong
     return -1;
}

int parse_args(int argc, char* argv[])
{
	// get a user to resuid
	// service mode: static directory
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

    // resuid
    // open the direcotory
    //
    //
    // test
    process_request();

    
    return 0;
}
