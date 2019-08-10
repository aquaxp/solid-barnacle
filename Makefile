barnacle: barnacle.c
	$(CC) barnacle.c -o barnacle -Wall -Wextra -pedantic -std=c99

clean:
	$(RM) barnacle
