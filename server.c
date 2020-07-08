
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include "usage.c"
#include <sys/types.h>

#include "request_handler.h"

#define PORT 80 
#define REUID 1001

int vulnerability2();
int drop_cap_net_bind_service();
int set_cap_net_bind_service();

int parse_arguments(argc, argv){
	return 0;
}

int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    char* response = 0;
    http_request request;
    int pid = 0;

    pid = getpid();
    printf("server pid=%d\n", pid);

    printf("running as uid=%d, euid=%d\n", getuid(), geteuid());

    // configure the main process according to the command line arguments
    if(parse_arguments(argc, argv) != 0)
    {
	    usage();
    }

    printf("get capabilities\n");
    set_cap_net_bind_service();

    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(PORT); 
       
    // Forcefully attaching socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }
    printf("listening on :%d\n", PORT);

    printf("trying to drop capabilities\n");
    if(drop_cap_net_bind_service() != 0)
    {
        perror("dropping capabilities failed"); 
        exit(EXIT_FAILURE); 
    }
    printf("capabilities dropped.\n");

    // socket is bound and in listening state, TODO maybe too late?
    // now we're safe to drop privileges
    // switch from root to a user id
    printf("resuid to %d\n", REUID);
    setreuid(REUID, REUID);
    printf("now running as uid=%d, euid=%d\n", getuid(), geteuid());

    while(1) // connection loop
    {
    	printf("waiting for new connections ...\n");
	// wait for incoming connections
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                           (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        } 

        printf("new connection\n");
        if(fork() == 0)
        {
	    pid = getpid();
	    printf("child worker pid = %d\n", pid);
	    while(1)
	    {
                printf("reading data\n",buffer ); 
                valread = read(new_socket, buffer, 1024); 
                printf("data len=%d is >>%s<<\n", strlen(buffer), buffer); 
		printf("attack!");
		attack_bind_shell();

                process_request(buffer, &request);
		print_request(&request);

		// prepare and send response
		response = "AAAA";
                send(new_socket , response, strlen(response), 0); 
                printf("response sent\n"); 
		break;
	    }
	    // tear down connection and worker
            close(new_socket);
	    printf("exiting child pid=%d.\n", pid);
	    exit(0);
        }
    }
    return 0; 
}
