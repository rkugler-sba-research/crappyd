
#include "seccomp.c"

int
main(int argc, char* argv[])
{
    printf("before seccomp\n");
    if(prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0))
	exit(1);
    seccomp_filter();
    printf("filter installed. opening file\n");
    //execve("/bin/sh", NULL, NULL);
    open("/etc/passwd", "r");
    printf("after seccomp\n");
    return 0;
}
