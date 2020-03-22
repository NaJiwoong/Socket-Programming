#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include <arpa/inet.h>
#include <assert.h>

typedef int bool;
#define true 1
#define false 0

#define MAX_PACKET_SIZE 10u*1000u*1000u					// Max packet size
#define STRING_SIZE_LIMIT MAX_PACKET_SIZE - 8u	// String bytes limit

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

struct packet_header{
	unsigned char opt;
	unsigned char shift;
	unsigned short checksum;
	unsigned int length;
};

/* Check if the string is number and it is valid 
 	 opt parameter is for choosing whether it is number
 	 for ipaddress, or port number */ 
bool is_valid_number(char* number, int maxlen, int opt){
	if (strlen(number) > maxlen)
		return false;
	
//	int idx;
//	for (idx=0; idx < strlen(number); idx++){ // Check if every char is digit, otherwise, false
//		if (!isdigit(number[idx]))
//			return false;
//	}
	int num = atoi(number);
	if (opt == 2)															// For checking validity of shift number
		return true;
	if (opt == 1){
		return (0 <= num && num <= 65535);
	}
	else if (!(0 <= num && num <= 255))
		return false;

	return true;
}

/* Check if the addr is valid ip address */
bool is_ipaddr(char* addr){
	char *token, addr_tok[16], *context;
	strncpy(addr_tok, addr, strlen(addr)+1);
	
	token = strtok_r(addr_tok, ".", &context);
	if (!is_valid_number(token, 3, 0))
		return false;
	int repeat=1;
	while((token = strtok_r(NULL, ".", &context))){
		if (repeat>3)
			return false;
		if (is_valid_number(token, 3, 0))
			repeat++;
		else{
			return false;
		}
	}
	return true;
}

unsigned short
get_checksum(struct packet_header* header, char* string, unsigned int length){
	unsigned long long sum = 0, result = 0;
	int i;

	/* Accumulate checksum */
	for (i=0; i<4; i++){
		sum += ((unsigned short *)header)[i];
	}
	for (i=0; i < length-1; i += 2){
		//printf("il?");
		unsigned short word16 = *(unsigned short *) &string[i];
		sum += word16;
	}
	
	/* Handle odd-sized case */
	if (length & 1){
		unsigned short word16 = (unsigned char) string[i];
		sum += word16;
	}

	/* Fold to get the ones-complement result */
	result += (sum >> 48) & 0xFFFF;
	result += (sum >> 32) & 0xFFFF;
	result += (sum >> 16) & 0xFFFF;
	result += sum & 0xFFFF;
	
	while (result >> 16){
		result = (result & 0xFFFF) + (result >> 16);
	}


	return ~result;
}


/* Build packet from op, shift , and given input string 
 	 This packet does not have checksum */
char* build_packet(unsigned int op, unsigned int shift, char* string){
	unsigned int length = MIN(strlen(string), STRING_SIZE_LIMIT);
	char *packet = malloc(sizeof(char)*length+8);
	
	struct packet_header *header = malloc(sizeof(struct packet_header));
	header->opt = op;
	header->shift = shift;
	header->checksum = (unsigned short) 0;
	header->length = htonl(length+8);

	header->checksum = get_checksum(header, string, length+8);
	
	/* Make entire packet in char ptr type */

	memcpy(packet, header, sizeof(header));
	memcpy((packet+8), string, length);

	free(header);

	return packet;

}

/* For debugging */
void print_byte(void *ptr, int size){
	unsigned char *p = ptr;
	int i;
	for (i=0; i<size; i++) {
		printf("%02hhX ", p[i]);
	}
	printf("\n");
}


