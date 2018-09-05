#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <termios.h>
#include <stdio.h>
#include "unite.h"

#define UIOGRXIS 0x80000001

#define DLE 0x10
#define EOT 0x04
#define ENQ 0x05
#define SLAV_ADDR 0x01

int check_serial_error(int fd, struct serial_icounter_struct prev, struct serial_icounter_struct now) {
	if(prev.parity != now.parity || prev.frame != now.frame || prev.overrun != now.overrun)
		return -1;

	return 0;
}

int unite_open(struct unite_struct *driver, char *device) {
	driver->fd = open(device, O_RDWR);
	if (driver->fd < 0) {
		return driver->fd;
	}
	return 0;
}

int unite_close(struct unite_struct *driver) {
	return close(driver->fd);
}

int unite_init(struct unite_struct *driver, tcflag_t baud, tcflag_t length, tcflag_t parity, tcflag_t stopbit) {
	struct termios tio = {0};
	tio.c_cflag |= CREAD;
	tio.c_cflag |= CLOCAL;
	tio.c_cflag |= CRTSCTS;
	tio.c_cflag |= baud;
	tio.c_cflag |= parity;
	tio.c_cflag |= stopbit;
	tio.c_cflag |= length;

	// non canonical mode setting
	tio.c_lflag = 0;
	tio.c_cc[VTIME] = 0;
	tio.c_cc[VMIN] = 1;

	tcflush(driver->fd, TCIFLUSH);
	tcsetattr(driver->fd, TCSANOW, &tio);

	return 0;
}

int unite_read(struct unite_struct *driver) {
	int len, total = 0;
	unsigned char buf[255];
	unsigned long rxtocnt, prev_rxtocnt;
	struct serial_icounter_struct prev_icount, now_icount;
	__u32 rxtotal = 0;

	// init rxbuf
	memset(driver->rxbuf, 0, sizeof(driver->rxbuf));

	// init rxtocnt
	ioctl(driver->fd, UIOGRXIS, &rxtocnt);
	prev_rxtocnt = rxtocnt;

	// init rxtotal
	ioctl(driver->fd, TIOCGICOUNT, &prev_icount);
	rxtotal = prev_icount.rx;

	// write(fd, sdata, size);
	while(1) {
		len = read(driver->fd, buf, sizeof(buf));
		if (len <= 0) {
			fprintf(stderr, "read error!!\n");
			break;
		}

		ioctl(driver->fd, UIOGRXIS, &rxtocnt);

		for(int i = 0; i < len;i++) {
			driver->rxbuf[total+i] = buf[i];
		}

		// check break detection count
		// ioctl(fd, TIOCGICOUNT, &now_icount);
		// if(check_serial_error(fd, prev_icount, now_icount))
		// 	printf("serial error happens!!\n");

		ioctl(driver->fd, TIOCGICOUNT, &now_icount);
		prev_icount = now_icount;
		total += len;

		if (prev_rxtocnt != rxtocnt && total == (prev_icount.rx - rxtotal)) {
			return total;
		}
	}
	return total;
}

int unite_parse(struct unite_struct *driver, int dlen) {
	unsigned char addr;
	if (dlen >= 3) {
		printf("dlen is 3\n");
		if (driver->rxbuf[0] == DLE && driver->rxbuf[1] == ENQ) {
			printf("invitation\n");
			if (driver->rxbuf[2] == SLAV_ADDR) {
				printf("write\n");
				driver->txbuf[0] = EOT;
				write(driver->fd, driver->txbuf, 1);
			}
		}
	}
}

struct unite_struct new_unite_struct() {
	struct unite_struct driver = {
		.open = unite_open,
		.close = unite_close,
		.init = unite_init,
		.read = unite_read,
		.parse = unite_parse,
	};
	return driver;
}

