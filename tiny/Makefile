CC = gcc
CFLAGS = -Wall
FILE ?= sample.tny

tiny_scan: main.c util.c scan.c globals.h util.h scan.h
	$(CC) $(CFLAGS) -o tiny_scan main.c util.c scan.c

run: tiny_scan
	./tiny_scan $(FILE)

clean:
	rm -f tiny_scan

.PHONY: run clean
