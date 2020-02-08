
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 

// bind socket 
int vulnerability1() 
{
    int s;
    struct sockaddr_in addr;
    printf("!!! triggering vulnerability1 !!!\n");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(443);

    s = socket(PF_INET, SOCK_STREAM, 0); 
    bind(s, (struct sockaddr*)&addr, sizeof(addr));  
    listen(s, 100);
}
