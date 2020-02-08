
#include <unistd.h> 
#include <linux/capability.h>
#include <sys/capability.h>
#include <stdio.h> 
#include <stdlib.h> 

int drop_capabilties()
{
    cap_t caps;
    int ncap = 0;
    cap_value_t cap_list[255];

    if (!CAP_IS_SUPPORTED(CAP_SETFCAP))
    {
        perror("no capabiltities are supported");
	return -1;
    }

    caps = cap_get_proc();
    if(caps == NULL)
        return -1;

    cap_list[0] = CAP_NET_BIND_SERVICE;
    ncap = 1; // one element in the array is set

    if(cap_set_flag(caps, CAP_INHERITABLE, ncap, cap_list, CAP_CLEAR) == -1)
    {
        perror("error cap_set_flag");
        cap_free(caps);
	return -1;
    }
    if(cap_set_proc(caps) == -1)
    {
        perror("error cap_set_proc");
	return -1;
    }
    if (cap_free(caps) == -1)
        perror("error cap_free");

    return 0;
}
