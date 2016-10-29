#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#define HOST "127.0.0.1"
#define HTTPPORT "32200"
#define BACKLOG 10
#define NUM_THREADS 2

void *get_in_addr(struct sockaddr * sa); //get info of incoming addr in struct
void print_ip( struct addrinfo *ai); //prints IP
void *subserver(void * reply_sock_fd_as_ptr); //starts subserver, sends HTML page
int get_server_socket(char *hostname, char *port); //get a socket and bind to it
int start_server(int serv_socket, int backlog);  //starts listening on port for inc connections
int accept_client(int serv_sock); //accepts incoming connection
void start_subserver(int reply_sock_fd); //starts subserver

int main(void) {

	int http_sock_fd;
	int reply_sock_fd;
	
	/*
	 * int yes; This patches a compiler error that prevented compiling
	 * with the current compiler settings that complained about it being
	 * unused.
	 */

	http_sock_fd = get_server_socket(HOST, HTTPPORT);

	if (start_server(http_sock_fd, BACKLOG) == -1) {
		printf("start server error\n");
		exit(1);
	}

	while(1) {
		if ((reply_sock_fd = accept_client(http_sock_fd)) == -1) {
			continue;
		}

		start_subserver(reply_sock_fd);
	}
}


// 
int get_server_socket(char *hostname, char *port) {
	struct addrinfo hints, *servinfo, *p;
	int status;
	int server_socket;
	int yes = 1;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(status));
		exit(1);
	}

	for (p = servinfo; p != NULL; p = p ->ai_next) {
		if ((server_socket = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) == -1) {
			printf("socket socket \n");
			continue;
		}
		if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			printf("socket option\n");
			continue;
		}

		if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1) {
			printf("socket bind \n");
			continue;
		}
		break;
	}
	print_ip(servinfo);
	freeaddrinfo(servinfo);
	return server_socket;
}

int start_server(int serv_socket, int backlog) {
	int status = 0;
	if ((status = listen(serv_socket, backlog)) == -1) {
		printf("socket listen error\n");
	}
	return status;
}

int accept_client(int serv_sock) {
	int reply_sock_fd = -1;
	socklen_t sin_size = sizeof(struct sockaddr_storage);
	struct sockaddr_storage client_addr;
	char client_printable_addr[INET6_ADDRSTRLEN];

	if ((reply_sock_fd = accept(serv_sock, 
					(struct sockaddr *)&client_addr, &sin_size)) == -1) {
		printf("socket accept error\n");
	}
	else {
		inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), 
				client_printable_addr, sizeof client_printable_addr);
		printf("server: connection from %s at port %d\n", client_printable_addr,
				((struct sockaddr_in*)&client_addr)->sin_port);
	}
	return reply_sock_fd;
}

void start_subserver(int reply_sock_fd) {
	pthread_t pthread;
	long reply_sock_fd_long = reply_sock_fd;
	if (pthread_create(&pthread, NULL, subserver, (void *)reply_sock_fd_long) != 0) {
		printf("failed to start subserver\n");
	}
	else {
		printf("subserver %ld started\n", (unsigned long)pthread);
	}
}

void *subserver(void * reply_sock_fd_as_ptr) {
	char *html_file;
	int html_file_fd;
	int read_count = -1;
	int BUFFERSIZE = 256;
	char buffer[BUFFERSIZE+1];

	long reply_sock_fd_long = (long) reply_sock_fd_as_ptr;
	int reply_sock_fd = (int) reply_sock_fd_long;
	printf("subserver ID = %ld\n", (unsigned long) pthread_self());
	read_count = recv(reply_sock_fd, buffer, BUFFERSIZE, 0);
	buffer[read_count] = '\0';
	printf("%s\n", buffer);
	html_file = "../pages/";
	html_file = strtok(&buffer[5], " \t\n");
	printf("FILENAME: %s\n", html_file);
	html_file_fd = open(html_file, O_RDONLY);
	while ((read_count = read(html_file_fd, buffer, BUFFERSIZE))>0) {
		send(reply_sock_fd, buffer, read_count, 0);
	}
	close(reply_sock_fd);

	return NULL;
}

void print_ip( struct addrinfo *ai) {
	struct addrinfo *p;
	void *addr;
	char *ipver;
	char ipstr[INET6_ADDRSTRLEN];
	struct sockaddr_in *ipv4;
	struct sockaddr_in6 *ipv6;
	short port = 0;

	for (p = ai; p !=  NULL; p = p->ai_next) {
		if (p->ai_family == AF_INET) {
			ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			port = ipv4->sin_port;
			ipver = "IPV4";
		}
		else {
			ipv6= (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			port = ipv4->sin_port;
			ipver = "IPV6";
		}
		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		printf("serv ip info: %s - %s @%d\n", ipstr, ipver, ntohs(port));
	}
}

//get the structure of incoming addr
void *get_in_addr(struct sockaddr * sa) {
	if (sa->sa_family == AF_INET) {
		printf("ipv4\n");
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}
	else {
		printf("ipv6\n");
		return &(((struct sockaddr_in6 *)sa)->sin6_addr);
	}
}

