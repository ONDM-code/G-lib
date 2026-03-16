CC     = gcc
CFLAGS = -Wall -std=c99

all: bibliotheque

bibliotheque: main.c
	$(CC) $(CFLAGS) -o bibliotheque main.c

clean:
	rm -f bibliotheque *.csv
