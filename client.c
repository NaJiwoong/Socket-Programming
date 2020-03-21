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

#include "valid.h"

#define STRING_SIZE_LIMIT 9999992u				// String bytes limit
#define BUF_BLOCK_SIZE 65536u							// String buffer block size

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

	/* Create socket and connect to server */
	int socketfd;
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

	/* Build packet */
	if (strlen(string) <= 0)
		goto terminate; 
	char *packet = build_packet((unsigned int)(op-'0'), (unsigned int) atoi(shift), string);

	/* Send packet */
	int send_size;
	send_size = send(socketfd, packet, strlen(string)+8, 0);

	/* Receive packet */
	int recv_size;
	char* reply = malloc(strlen(string)+8);
	recv_size = recv(socketfd, reply, strlen(string)+8, 0);

	/* Checksum validity */
	struct packet_header *recv_header = malloc(sizeof(struct packet_header));
	memcpy(recv_header, reply, 8);
	memset(reply+2, 0, 2);
	unsigned short server_checksum = get_checksum((struct packet_header *)reply, 
																															(char *)(reply+8));
	if (!(server_checksum == recv_header->checksum)){
		close(socketfd);
		printf("wrong checksum!\n");
	}


	/* Print the result */
	char* result = malloc(strlen(string));
	memcpy(result, reply+8, strlen(string));
	printf("<Received string>\n%s\n", result);


	/* Termination */
	free(reply);
	free(result);
	free(string);
	free(packet);
	
	terminate:
	
		return 0;
}




