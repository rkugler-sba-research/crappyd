
#include "request_handler.h"

int process_request(char* buffer, http_request* request) {
	// http_request* request = (void *)malloc(sizeof(http_request));
	strcpy(request->body, buffer);
	request->ipv4_addr = 33752069;
	return 0;
}

char* format_ip(unsigned int ip, char* buf)
{
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;   
    snprintf(buf, "%d.%d.%d.%d\n", bytes[3], bytes[2], bytes[1], bytes[0]);        
    return buf;
}

void print_request(http_request* request)
{
    char ip[20];
    format_ip(request->ipv4_addr, ip);
    printf("request ipv4=%s, body >>>%s<<<\n", ip, request->body);        
}
