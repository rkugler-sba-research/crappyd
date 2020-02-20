

void usage()
{
    printf("usage:\n");
    printf("server (runs with default parameters)");
    printf("server -p 80 -d /var/www -l /var/log/server.log -c /etc/server.conf -k /etc/server.d/key");
    printf("downserver -p 80 -d /var/www -l /var/log/server.log");
}
