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
	int i;
	for (i=0; i < strlen(string); i++){
		string[i] = encrypt(string[i], shift);
	}
	return string;
}

char *
caesar_cipher_d(char* string, int shift){
	return caesar_cipher_d(string, -shift);
}

char *
caesar_cipher(char *string,unsigned int op, unsigned int shift){
	return (op == 0) ? caesar_cipher_e(string, shift) : caesar_cipher_d(string, shift);
}
