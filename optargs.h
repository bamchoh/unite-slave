#include <termios.h>

struct optarg_struct {
	tcflag_t baudrate;
	tcflag_t parity;
	tcflag_t stopbit;
	tcflag_t length;
	char *filename;
};

int parse_optarg(int argc, char* argv[], struct optarg_struct *optargs);
