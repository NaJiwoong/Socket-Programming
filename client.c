// [CS341] Project #0
// Socket progamming
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

#include "valid.h"

#include <errno.h>

#define MAX_PACKET_SIZE (unsigned int)10u*1000u*1000u					// Max packet size
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

	int param_valid;												// flag for check parameter validity
	int opt;																// switch option for getopt()
	char ip_addr[16], op, shift[4];			// char pointer for parameters
	unsigned short port;

	/* Get parameters and store them for later use */
	while((opt = getopt(argc, argv, "h:p:o:s:")) != -1){
		switch(opt){
			case 'h':
				// Check validity of ip address
				if (!is_ipaddr(optarg))
					exit(-1);
				strncpy(ip_addr, optarg, strlen(optarg)+1);
				break;
			case 'p':
				// Check validity of porta
				if (!is_valid_number(optarg, 5, 1))
					exit(-1);
				port = atoi(optarg);
				break;
			case 'o':
				// Check if o is either '0' or '1'
				if (!((*optarg=='0'||*optarg=='1')&&strlen(optarg)==1))
					exit(-1);
				op = optarg[0];
				break;
			case 's':
				// Check if s is valid number
				if (!is_valid_number(optarg, 4, 2))
					exit(-1);
				strncpy(shift, optarg, strlen(optarg)+1);
				break;
			case '?':														// In unknown option, terminate the program
				exit(-1);
		}
	}


	/* Get string from standard input until meet EOF */
	char ch;
	unsigned int idx=0, size=BUF_BLOCK_SIZE;
	char *string = malloc(BUF_BLOCK_SIZE);

	while((ch=getchar()) != EOF){
		string[idx] = (char)ch;
		idx++;
		if (idx >= size){
			string = realloc(string, size+BUF_BLOCK_SIZE);
			size += BUF_BLOCK_SIZE;
		}
	}
	printf("string length: %lu\n", strlen(string));

	/* Create socket and connect to server */
	int socketfd, i;
	socketfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
	memset(addr, 0, sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = inet_addr(ip_addr);

	if (connect(socketfd, (struct sockaddr *)addr, sizeof(struct sockaddr_in)) == -1){
		printf("failed to connect\n");
		exit(-1);
	}
	int socket_connection = 1;

	/* Build packet */
	if (strlen(string) <= 0)
		goto terminate;

	unsigned int length = strlen(string), used = 0u;
	int required_packet = length/(10u*1000u*1000u - 8u) + 1;
	char **packets = malloc(sizeof(char *)*required_packet);

	printf("required_packet: %d\n", required_packet);
	packets[0] = build_packet((unsigned int)(op-'0'), (unsigned int) atoi(shift), string+used);
	used += ntohl(*(unsigned int *)(packets[0]+4)) - 8;
	for (i = 1; i < required_packet; i++){
		packets[i] = malloc(sizeof(char *)*MIN(strlen(string+used), MAX_PACKET_SIZE));
		memcpy(packets[i], string+used, MIN(strlen(string+used), MAX_PACKET_SIZE));
		used += MIN(strlen(string+used), MAX_PACKET_SIZE);
	}

	/* Send packet */
	int send_size;
	unsigned int total_packet_size = strlen(string) + 8;
	for (i = 0; i < required_packet; i++){
		send_size = send(socketfd, packets[i], ntohl(*(unsigned int *)(packets[i]+4)), 0);
	}

	/* Receive packet */
	unsigned int to_recv = total_packet_size > MAX_PACKET_SIZE ?
		MAX_PACKET_SIZE : total_packet_size;
	unsigned int to_recv_string;
	char **replies = malloc(sizeof(char *)*1024);					// List for pointers of replies
	memset(replies, 0, sizeof(char *)*1024);
	replies[0] = malloc(to_recv);
	unsigned int len_req = strlen(string), len_rec = 0;	// Required length, and received length

	// Do until get whole string to get (until received length exeeds required length)
	int packet_number = 0;
	int slot_number = 1024;
	while ((to_recv_string=(len_req-len_rec)) > 0){		
		printf("while...");
		packet_number++;
		if (packet_number > slot_number){
			replies = realloc(replies, sizeof(char *)*(1024 + slot_number));
			slot_number += 1024;
		}
		int iter=0;
		unsigned int recv_size, string_length;
		unsigned int total_recv = strlen(replies[iter]);

		//printf("error----: %d\n", errno);
		// Get the header at least (8B)
		printf("why?");
		while (total_recv < 8){
			recv_size = recv(socketfd, replies[iter]+total_recv, to_recv, 0);
			printf("first recv: %d\n", recv_size);
			if (recv_size == 0)
				exit(-1);
			total_recv += recv_size;
		}
		

		// Get string length keep receiving until string length
		string_length = ntohl(*(unsigned int *)(replies[iter]+4))-8;
		while (string_length+8 > total_recv){
			recv_size = recv(socketfd, replies[iter]+total_recv, 
					string_length+8-total_recv, 0);
			printf("second recv: %d\n", recv_size);
			total_recv += recv_size;
		}
		len_rec += string_length;

		iter++;
		replies[iter] = malloc(to_recv);
		if (total_recv > string_length+8){			// If over received, pass it to next elem
			memcpy(replies[iter], replies[iter-1]+string_length+8,
					total_recv-string_length-8);
		}
		printf("why?");

		// Checksum validity
		struct packet_header *recv_header = malloc(sizeof(struct packet_header ));
		memcpy(recv_header, replies[iter-1], 8);
		memset(replies[iter-1]+2, 0, 2);
		unsigned short server_checksum = get_checksum((struct packet_header *)replies[iter-1],
				(char *)(replies[iter-1]+8), ntohl((recv_header->length)));
		if (!(server_checksum == recv_header->checksum)){
			close(socketfd);
			socket_connection = 0;
			break;
		}
		free(recv_header);
	}
	if (socket_connection == 1)
		close(socketfd);
	
	
	printf("lets merge\n");
	/* Concatenate string from packets */
	char *result = malloc(strlen(string)+1);
	i=0;
	char *reply;
	unsigned int add_string=0;
	reply = replies[0];
	unsigned int total_len = ntohl(*(unsigned int *)(reply+4))-8u;
	memcpy(result, reply+8, total_len);
	i= 1;
	printf("pass\n");
	for(reply = replies[i]; i < packet_number ; i++){
		printf("pass\n");
		if (reply == 0 || reply == NULL)
			break;
		memcpy(result+(int)add_string, reply, strlen(reply));
		add_string += strlen(reply);
	}
	result[strlen(string)] = '\0';

	/* Print the result */
	printf("%s", result);

	/* Termination */
	free(replies);
	free(reply);
	free(result);
	free(string);
	free(packets);
	free(addr);

terminate:

	return 0;
}
