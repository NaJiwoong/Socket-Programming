typedef int bool;
#define true 1
#define false 0

struct packet_header;

bool is_valid_number(char* number, int maxlen, int opt);
bool is_ipaddr(char* addr);

unsigned short
get_checksum(struct packet_header* header, char* string, unsigned int length);
char* build_packet(unsigned char op, unsigned char shift, char* string);

void print_byte(void *ptr, int size);
