.PHONY: all cap docker

all: server filebroker

server: server.c request_handler.c capabilities.c attack.c request_handler.h
	gcc -fno-stack-protector -z execstack -Wall -Wl,--no-as-needed -lcap -o server server.c request_handler.c attack.c capabilities.c -lcap

filebroker: filebroker.c attack.c
	gcc -Wall -o filebroker filebroker.c attack.c

cap: server
	sudo setcap 'cap_net_bind_service=+ep' ./server

docker: server
	docker build .

