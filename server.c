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
	listen_addr.sin_port = htons(port);

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
	
	int k;
	for (k = 1; k < 100; k++)
		pollings[k].fd = -1;

	while (1){
		int i, wait = poll(pollings, 100, -1);						// Wait until the events occur
	
		if (wait > 0){
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
					char **requests = malloc(sizeof(char *)*16);
					memset(requests, 0, sizeof(char *)*16);
					requests[i] = malloc(MAX_PACKET_SIZE);
					
					
					int iter=0, packet_number = 0;
					unsigned int recv_size, string_length;
					unsigned int total_recv = 0;
					while (total_recv < 8){
						recv_size = recv(pollings[i].fd, requests[i]+total_recv, MAX_PACKET_SIZE, 0);
						packet_number++;
						if (recv_size == 0){
							break;
						}
						total_recv += recv_size;
					}
					
					string_length = ntohl(*(unsigned int *)(requests[i]+4)) - 8;
					while (string_length+8 > total_recv){
						recv_size = recv(pollings[i].fd, requests[i]+total_recv,
																string_length+8-total_recv, 0);
						packet_number++;
						total_recv += recv_size;
					}
					
					// Checksum validity
					struct packet_header *recv_header = malloc(sizeof(struct packet_header));
					memcpy(recv_header, requests[i], 8);
					memset(requests[i]+2, 0, 2);
					unsigned short client_checksum = get_checksum((struct packet_header *)requests[i], 
											(char *)(requests[i]+8), ntohl((recv_header->length)));
					if (!(client_checksum == recv_header->checksum)){
						close(pollings[i].fd);
					}
					unsigned int op = recv_header->opt, shift = recv_header->shift;
					free(recv_header);

					// Caesar cipher
					char *result = malloc(ntohl(*(unsigned int *)(requests[i]+4))-8);
					int j = 0;
					unsigned int add_string = 0;
					memcpy(result+(int)add_string, requests[i]+8, ntohl(*(unsigned int *)(requests[i]+4))-8u);
					add_string += ntohl(*(unsigned int*)(requests[i]+4)-8u);
					caesar_cipher(result, op, shift);
					// Build packets and send all of them.
					unsigned int length = strlen(result), used = 0u;
					int required_packet = length/(10u*1000u*1000u - 8u) + 1;
					char **packets = malloc(sizeof(char *)*required_packet);
				
					for (j = 0; j < required_packet; j++){
						packets[j] = build_packet((unsigned int)(op-'0'), (unsigned int)shift, result+used);
						used += ntohl(*(unsigned int *)(packets[j]+4)) - 8;
					}
					
					int send_size;
					for (j = 0; j < required_packet; j++){
						send_size = send(pollings[i].fd, packets[j], ntohl(*(unsigned int *)(packets[j]+4)), 0);
						free(packets[j]);
					}

					pollings[i].revents = 0;
					close(pollings[i].fd);
					pollings[i].fd = -1;
				}
				else{
				}
			}
		}
	
	}

	close(pollings[0].fd);

	return 0;
}






