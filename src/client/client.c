/*
 * Client utilizing the Gomoku Inter Process Shuttle (GIPS) protocol designed by Sean Batzel and Andrew Plaza.
 *
 * Client designed by Sean Batzel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netdb.h>
#include <pthread.h>
#include "../lib/network.h"
#include "../lib/gips.h"

#define HOST "server1.cs.scranton.edu"
#define HTTPPORT "32200"
#define BACKLOG 10

char *send_move(int a, int b, char *board) {
	board[a][b] = 'A';
	// Send the move to the other guy.
	return board;
}

char *get_move(char *board) {
	// Get the move from the other guy.
	// Get an x and y coordinate from the gips packet.
	// Check if somebody won.
	// If they did, jump to that subroutine and tell us,
	// then terminate.
	board[x][y] = 'B';
	return board;
}

void display_board(char *board) {
	int i;
	int j;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			printf("%c", board[i][j]);
		}
		printf("\n");
	}
}

int main() {
	char *name;
	int move_x;
	int move_y;
	char board[8][8];
	int sock = connect_to_server();
	printf("Gomoku Client for Linux\n");
	if (sock != -1) {
		printf("Enter your name: ");
		scanf("%s", name);
		send(sock, name, sizeof(name), 0);
	} else {
		printf("Couldn't connect to the server.\n");
		exit(0);
	}
	while () {
		printf("%d> ", name);
		
	}
	close(sock);
}
