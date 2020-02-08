#include <string.h>
#include <stdio.h>

char response[] = "HTTP/1.1 200 OK\nServer: Apache-Coyote/1.1\nPragma: no-cache";

int main() { 
    char *token = NULL;
    token = strtok(response, "\n");
    while (token) {
        printf("Current token: %s.\n", token);
        token = strtok(NULL, "\n");
    }
    return 0;
}
