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

#include "valid.h"

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
	struct sockaddr_in 



	return 0;
}






