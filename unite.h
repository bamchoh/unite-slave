
struct unite_struct {
	int fd;
	unsigned char rxbuf[4096];
	unsigned char txbuf[4096];

	int (* open)(struct unite_struct*, char*);
	int (* close)(struct unite_struct*);
	int (* init)(struct unite_struct*, tcflag_t, tcflag_t, tcflag_t, tcflag_t);
	int (* read)(struct unite_struct*);
	int (* parse)(struct unite_struct*, int);
};

struct unite_struct new_unite_struct();
