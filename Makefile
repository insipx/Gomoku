#
# Makefile for the Gomoku project.
# 
#

CC = cc
CDEBUG = -g -Wall -Wextra -Werror
make:
	mkdir -p build
	mkdir -p build/server
	mkdir -p build/client
	$(CC) src/server/server.c -lpthread -o build/server/server
	$(CC) src/client/client.c -o build/client/client

client:
	mkdir -p build
	mkdir -p build/client
	$(CC) src/client/client.c -o build/client/client

server:
	mkdir -p build
	mkdir -p build/server
	$(CC) src/server/server.c -lpthread -o build/server/server


debug:
	mkdir -p debug
	mkdir -p debug/server
	mkdir -p debug/client
	$(CC) src/server/server.c $(CDEBUG) -lpthread  -o debug/server/server
	$(CC) src/client/client.c $(CDEBUG) -o debug/client/client

client-debug:
	mkdir -p debug
	mkdir -p debug/client
	$(CC) src/client/client.c $(CDEBUG) -o debug/client/client

server-debug:
	mkdir -p debug
	mkdir -p debug/server
	$(CC) src/server/server.c $(CDEBUG) -lpthread -o debug/server/server

clean:
	rm -rf build
	rm -rf debug
