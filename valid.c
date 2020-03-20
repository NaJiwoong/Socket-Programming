#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

typedef int bool;
#define true 1
#define false 0

/* Check if the string is number and it is valid 
 	 opt parameter is for choosing whether it is number
 	 for ipaddress, or port number */ 
bool is_valid_number(char* number, int maxlen, int opt){
	if (strlen(number) > maxlen)
		return false;
	
	int idx;
	for (idx=0; idx < strlen(number); idx++){ // Check if every char is digit, otherwise, false
		if (!isdigit(number[idx]))
			return false;
	}
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

