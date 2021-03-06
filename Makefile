.PHONY: all cap docker

all: server filebroker seccomptest

server: server.c request_handler.c capabilities.c attack.c request_handler.h seccomp.c
	gcc -fno-stack-protector -z execstack -Wall -Wl,--no-as-needed -lcap -o server server.c request_handler.c attack.c capabilities.c seccomp.c -lcap

filebroker: filebroker.c attack.c
	gcc -Wall -o filebroker filebroker.c attack.c

cap: server
	sudo setcap 'cap_net_bind_service=+ep' ./server

docker: server
	docker build .

seccomptest: seccomp.c seccomptest.c
	gcc -Wall -o seccomptest seccomp.c seccomptest.c
