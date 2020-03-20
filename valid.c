#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

/* Check if the string is number and it is 0<=x<=255 */ 
bool is_valid_number(char* number){					// Numbers in ip address is not longer than 3
	if (strlen(number) > 3)
		return false;
	
	int idx;
	for (idx=0; idx < strlen(number); idx++){ // Check if every char is digit, otherwise, false
		if (!isdigit(number[idx]))
			return false;
	}

	int num = atoi(number);
	if (!(0 <= num <= 255))
		return false;

	return true;
}
