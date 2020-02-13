
#include <unistd.h> 
#include <linux/capability.h>
#include <sys/capability.h>
#include <stdio.h> 
#include <stdlib.h> 

int drop_capabilties(int ncap, cap_value_t cap_list[], cap_flag_value_t flag)
{
    cap_t caps;

    if (!CAP_IS_SUPPORTED(CAP_SETFCAP))
    {
        perror("no capabiltities are supported");
	return -1;
    }

    caps = cap_get_proc();
    if(caps == NULL)
        return -1;

    if(cap_set_flag(caps, CAP_EFFECTIVE, ncap, cap_list, flag) == -1)
    {
        perror("error cap_set_flag effective");
        cap_free(caps);
	return -1;
    }

    if(cap_set_flag(caps, CAP_PERMITTED, ncap, cap_list, flag) == -1)
    {
        perror("error cap_set_flag permitted");
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

int drop_cap_net_bind_service()
{
    cap_value_t cap_list[] = { CAP_NET_BIND_SERVICE };
    return drop_capabilties(1, cap_list, CAP_CLEAR);
}

int set_cap_net_bind_service()
{
    cap_value_t cap_list[] = { CAP_NET_BIND_SERVICE };
    return drop_capabilties(1, cap_list, CAP_SET);
}

