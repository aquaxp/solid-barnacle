#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

static struct termios original_attr;

void die(const char *s) {
	perror(s);
	exit(1);
}

void disable_raw_mode() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_attr) == -1)
		die("tcsetattr");
}

void enable_raw_mode() {
	struct termios attr;

	if (tcgetattr(STDIN_FILENO, &original_attr) == -1)
		die("tcgetattr");
	atexit(disable_raw_mode);

	attr = original_attr;
	/*
	 * IXON - Intercepting software flows flags
	 * ICRNL - Do not convert carriage return to newline
	 * BRKINT - ignore break conditions (SIGINT sending)
	 * INPCK - parity checking (obsolete)
	 * ISTRIP - stripping 8th bit of byte to be set to 0
	 */
	attr.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
	/*
	 * Turn of output processing.
	 */
	attr.c_oflag &= ~(OPOST);
	/*
	 * set character size as 8 bit
	 */
	attr.c_cflag |= (CS8);
	/* ISIG to intercept Ctrl-Z */
	attr.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	/* Set timeout for read() */
	attr.c_cc[VMIN] = 0;
	attr.c_cc[VTIME] = 1;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &attr);
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &attr) == -1)
		die("tcsetattr");
}

int main() {
	enable_raw_mode();

	while (1) {
		char c = '\0';

		if (read(STDIN_FILENO, &c, 1) == -1 && EAGAIN != errno)
			die("read");
		if (iscntrl(c))
			printf("%d\r\n", c);
		else
			printf("%d ('%c')\r\n", c, c);

		if ('q' == c)
			break;
	}

	return 0;
}
