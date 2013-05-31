CC=gcc
EXTRAFLAGS=-Wall
CFLAGS=-mpopcnt -mtune='corei7' -O3

xkcd: xkcd.o skein.o
	$(CC) $(CFLAGS) $(EXTRAfLAGS) *.o -o xkcd -lpthread

xkcd.o: xkcd.c
	$(CC) $(CFLAGS) $(EXTRAFLAGS) -c xkcd.c

skein.o:
	$(CC) $(CFLAGS) -c skein/*.c

clean:
	rm -f xkcd *.o
