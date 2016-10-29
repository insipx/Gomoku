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
#include "../lib/glogic.h"

#define HOST "server1.cs.scranton.edu"
#define HTTPPORT "32200"
#define BACKLOG 10

char *send_move(int a, int b, char *board, int sock) {
	board[a][b] = 'A';
	// Send the move to the other guy.
	gips *z = encode(a, b);
	send_to(z, sock);
	return board;
}

char *get_move(char *board, int sock, short which_player) {
	// TODO This needs to take an argument determining what player we are.
	int *move;
	// Get the move from the other guy.
	gips *z = get_server(sock);
	// Get an x and y coordinate from the gips packet.
	if (z->is_win != 0) {
		// This needs to be changed to the current player's number.
		if (z->is_win == 1) {
			printf("You won!");
		} else {
			printf("You lost.");
		}
	}
	// Check if the game is over.
	// Otherwise we just decode
	move = decode(z);
	board[move[0]][move[1]] = 'B';
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
	// TODO We need a variable we can pass around to keep track of
	// TODO what player number we current are.
	char *name;
	short which_player;
	int move_x;
	int move_y;
	char board[8][8];
	int sock = connect_to_server();
	printf("Gomoku Client for Linux\n");
	if (sock != -1) {
		printf("Enter your name: ");
		scanf("%s", name);
		send(sock, name, sizeof(name), 0);
		recv(sock, which_player, sizeof(which_player), 0);
	} else {
		printf("Couldn't connect to the server.\n");
		exit(0);
	}
	while (1) {
		//TODO check this loop
		printf("%s> ", name);
		scanf("%d%d", &move_x, &move_y);
		board = send_move(move_x, move_y, board, sock);
		board = get_move(board, sock);
	}
	close(sock);
}
