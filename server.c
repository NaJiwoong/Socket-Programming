// [CS341] Project #0
// Socket progamming: Client
// 20170201
// Na Jiwoong

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <poll.h>

#include "valid.h"
#include "caesar.h"

#include <errno.h>

#define MAX_PACKET_SIZE (unsigned int)10u*1024u*1024u					// Max packet size
#define STRING_SIZE_LIMIT MAX_PACKET_SIZE - 8u	// String bytes limit
#define BUF_BLOCK_SIZE 65536u										// String buffer block size

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

struct packet_header{
	unsigned char opt;
	unsigned char shift;
	unsigned short checksum;
	unsigned int length;
};

int main(int argc, char **argv){

	int opt;
	unsigned short port;

	/* Get a parameter an store it for later use */
	while ((opt = getopt(argc, argv, "p:")) != -1){
		switch (opt){
			case 'p':
				if (!is_valid_number(optarg, 5, 1))
					exit(-1);
				port = atoi(optarg);
				break;
			case '?':
				exit(-1);
		}
		
	}

	/* Create socket */
	int listen_socket, connect_socket;
	struct sockaddr_in listen_addr, connect_addr;
	memset(&listen_addr, 0, sizeof(listen_addr));
	memset(&connect_addr, 0, sizeof(connect_addr));

	listen_socket = socket(PF_INET, SOCK_STREAM, 0);	// Socket for listening
	
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listen_addr.sin_port = htons(PORT);

	int bind_status;
	if ((bind_status = bind(listen_socket, (struct sockaddr *) &listen_addr, sizeof(listen_addr))) != 0){
		exit(-1);
	}

	int listen_status;
	if ((listen_status = listen(listen_socket, 33)) == -1){
		exit(-1);
	}

	socklen_t addrlen;
	addrlen = sizeof(struct sockaddr *);

	struct pollfd pollings[100];

	pollings[0].fd = listen_socket;
	pollings[0].events = POLLIN;
	pollings[0].revents = 0;

	for (i = 1; i < 100; i++)
		pollings[i].fd = -1;

	while (1){
		int i, wait = poll(pollings, 100, -1);						// Wait until the events occur
	
		if (wait > 1){
			if (pollings[0].revents == POLLIN){
				// If the event is requesting connection
				for (i = 1; i < 100; i++){
					if (pollings[i].fd == -1){
						pollings[i].fd = accept(listen_socket, (struct sockaddr *)&connect_addr, &addrlen);
						pollings[i].events = POLLIN;
						pollings[i].revents = 0;
						break;
					}
				}
			}

			// IF the event is sending packets
			for (i = 1; i < 100; i++){
				if (pollings[i].revents == POLLIN){
					// Get packets as client code, and handle it
					


					// Caesar cipher



					// Build packets and send all of them.




				}
				else{
					close(pollings[i].fd);
					pollings[i].fd = -1;
					pollings[i].revents = 0;
				}
			}
		}
	
	}

	close(pollings[0].fd);

	return 0;
}






