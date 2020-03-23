
char encrypt(char ch, int shift);
char decrypt(char ch, int shift);

char *
caesar_cipher_e(char *string, int shift);
char *
caesar_cipher_d(char *string, int shift);
char *
caesar_cipher(char *string, unsigned int op, unsigned int shift);
