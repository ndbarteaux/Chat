CC=gcc
CFLAGS=-I.

hellomake: chat.o
	$(CC) -o chat chat.o -I.