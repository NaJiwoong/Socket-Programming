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

#include "valid.h"

#define STRING_SIZE_LIMIT 9999992					// String bytes limit
#define BUF_BLOCK_SIZE 65536							// String buffer block size


int main(int argc, char **argv){

	int param_valid;												// flag for check parameter validity
	int opt;																// switch option for getopt()
	char ip_addr[16], port[6], op, shift[32];			// char pointer for parameters

	/* Get parameters and store them for later use */
	while((opt = getopt(argc, argv, "h:p:o:s:")) != -1){
		switch(opt){
			case 'h':
				// Check validity of ip address
				strncpy(ip_addr, optarg, strlen(optarg)+1);
				break;
			case 'p':
				// Check validity of port
				strncpy(port, optarg, strlen(optarg)+1);
				break;
			case 'o':
				// Check if o is either '0' or '1'
				if (!((*optarg=='0'||*optarg=='1')&&strlen(optarg)==1))
					exit(-1);
				op = optarg[0];
				break;
			case 's':
				// Check if s is valid number
				strncpy(shift, optarg, strlen(optarg)+1);
				break;
			case '?':														// In unknown option, terminate the program
				exit(-1);
		}
	}
	
	/* Get string from standard input until meet EOF */
	char* test = "123";
	if (is_valid_number(test))
		printf("succeed";


	return 0;
}
