enum unite_state {
  UNITE_WAIT_TO_SEND,
  UNITE_WAIT_TO_RECV,
};

struct unite_struct {
	int fd;
	unsigned char rxbuf[4096];
	unsigned char txbuf[4096];
  enum unite_state state;

	int (* open)(struct unite_struct*, char*);
	int (* close)(struct unite_struct*);
	int (* init)(struct unite_struct*, tcflag_t, tcflag_t, tcflag_t, tcflag_t);
	int (* read)(struct unite_struct*);
	int (* write)(struct unite_struct*, int);
	int (* parse)(struct unite_struct*, int);
};

struct unite_struct new_unite_struct();
