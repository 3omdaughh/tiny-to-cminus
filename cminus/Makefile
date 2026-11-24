# Makefile for the C-Minus compiler front end
#
#   make                  build ./cminus
#   make run FILE=x.cm    build, then scan x.cm
#   make clean            remove generated files

CC      = gcc
CFLAGS  = -Wall -Wextra -Iinclude
LEX     = flex

BUILD   = build
TARGET  = cminus
FILE   ?= tests/valid/gcd.cm

HEADERS = include/globals.h include/util.h include/scan.h
OBJS    = $(BUILD)/main.o $(BUILD)/util.o $(BUILD)/lex.yy.o

all: $(TARGET)

# link all object files into the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# flex: scan.l -> lex.yy.c (the generated scanner, yylex())
$(BUILD)/lex.yy.c: src/scan.l | $(BUILD)
	$(LEX) -o $@ $<

$(BUILD)/lex.yy.o: $(BUILD)/lex.yy.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

# hand-written C sources
$(BUILD)/%.o: src/%.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD):
	mkdir -p $(BUILD)

run: $(TARGET)
	./$(TARGET) $(FILE)

clean:
	rm -rf $(BUILD) $(TARGET)

.PHONY: all run clean
