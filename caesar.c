#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

char encrypt(char ch, int shift){
	if ('A' <= ch && ch <= 'Z'){
		ch += 32;
	}
	if ('a' <= ch && ch <= 'z'){
		ch += shift;
		if ('z' < ch){
			ch += ('a' - 1 - 'z');
		}
		else if ('a' > ch){
			ch -= ('a' - 1 - 'z');
		}
	}
	return ch;
}

char decrypt(char ch, int shift){
	return encrypt(ch, -shift);
}

char *
caesar_cipher_e(char* string, int shift){
	char *cipher = malloc(strlen(string)*sizeof(char));
	strncpy(cipher, string, strlen(string));

	int i;
	for (i=0; i < strlen(cipher); i++){
		cipher[i] = encrypt(cipher[i], shift);
	}
	return cipher;
}

char *
caesar_cipher_d(char* string, int shift){
	return caesar_cipher_d(string, -shift);
}
