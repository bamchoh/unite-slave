#include <time.h>
#include <stdio.h>
#include "optargs.h"
#include "unite.h"

#define SERIAL_PORT "/dev/ttyAMA0"

typedef struct log {
	int size;
	int num;
	struct timespec st;
	struct timespec et;
} log; 

void print_time(char *name, struct timespec t) {
	printf("%s:%10ld.%09ld ", name, t.tv_sec, t.tv_nsec);
}

struct timespec calc_duration(FILE *fp, struct timespec s, struct timespec e) {
	struct timespec tmp;
	if(e.tv_nsec < s.tv_nsec) {
		tmp.tv_sec = e.tv_sec - s.tv_sec - 1;
		tmp.tv_nsec = e.tv_nsec + 1000000000 - s.tv_nsec;
	} else {
		tmp.tv_sec = e.tv_sec - s.tv_sec;
		tmp.tv_nsec = e.tv_nsec - s.tv_nsec;
	}
	return tmp;
}

int main(int argc, char *argv[])
{
	struct optarg_struct opts;
	parse_optarg(argc, argv, &opts);

	struct unite_struct driver = new_unite_struct();

	if(driver.open(&driver, SERIAL_PORT)) {
		fprintf(stderr, "open error\n");
		return -1;
	}

	driver.init(&driver, opts.baudrate, opts.length, opts.parity, opts.stopbit);

	// waiting for receive any data
	int n;
  int type;
	while(1) {
		n = driver.read(&driver);

		// print received data
		for(int i = 0; i < n; i++) {
			if(i != 0) {
				printf(":");
			}
			printf("%02X", driver.rxbuf[i]);
		}
		printf("\n");

		type = driver.parse(&driver, n);

    driver.write(&driver, type);
  }

  driver.close(&driver);
  // fclose(fp);
  return 0;
}
