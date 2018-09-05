#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "optargs.h"

tcflag_t set_baudrate(char *rate)
{
	if(rate == 0)
		return B9600;

	if(strcmp(rate, "2400") == 0)
		return B2400;

	if(strcmp(rate, "4800") == 0)
		return B4800;

	if(strcmp(rate, "9600") == 0)
		return B9600;

	if(strcmp(rate, "19200") == 0)
		return B19200;

	if(strcmp(rate, "38400") == 0)
		return B38400;

	if(strcmp(rate, "57600") == 0)
		return B57600;

	if(strcmp(rate, "115200") == 0)
		return B115200;

	return B9600;
}

tcflag_t set_parity(char *parity) {
	if(parity == 0)
		return 0;

	if(strcmp(parity, "e") == 0)
		return PARENB;

	if(strcmp(parity, "o") == 0)
		return PARENB | PARODD;

	return 0;
}

tcflag_t set_stopbit(char *bit) {
	if(bit == 0)
		return 0;

	if(strcmp(bit, "2") == 0)
		return CSTOPB;

	return 0;
}

tcflag_t set_length(char *len) {
	if(len == 0)
		return CS8;

	if(strcmp(len, "7") == 0)
		return CS7;

	return CS8;
}

int parse_optarg(int argc, char* argv[], struct optarg_struct *opts) {
	int opt;
	opterr = 0;
	char *b_optarg;
	char *p_optarg;
	char *s_optarg;
	char *l_optarg;
	while ((opt = getopt(argc, argv, "b:p:s:l:f:")) != -1) {
		switch(opt) {
			case 'b':
				b_optarg = optarg;
				break;
			case 'p':
				p_optarg = optarg;
				break;
			case 's':
				s_optarg = optarg;
				break;
			case 'l':
				l_optarg = optarg;
				break;
			case 'f':
				opts->filename = optarg;
				break;
			default:
				break;
		}
	}

	opts->baudrate = set_baudrate(b_optarg);
	opts->parity = set_parity(p_optarg);
	opts->stopbit = set_stopbit(s_optarg);
	opts->length = set_length(l_optarg);

	return 0;
}

