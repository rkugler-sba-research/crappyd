.PHONY: all cap

all: server

server: server.c request_handler.c capabilities.c attack.c
	gcc -Wall -Wl,--no-as-needed -lcap -o server server.c request_handler.c attack.c capabilities.c -lcap

cap: server
	sudo setcap 'cap_net_bind_service=+ep' ./server


