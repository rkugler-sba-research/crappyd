
struct http_request {
    char method; // Get, Post, Head, ... (first char)
    int ipv4_addr;
    char body[100];
} typedef http_request;

int process_request(char* buffer, http_request* request);
void print_request(http_request* request);
