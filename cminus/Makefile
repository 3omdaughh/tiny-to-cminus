# Makefile for the C-Minus compiler front end
#
# Two executables are built from the same parser and main program;
# they differ only in the scanner:
#   cminus        Flex-generated scanner   (src/scan.l)
#   cminus_hand   hand-written DFA scanner (src/scan.c, modified from TINY)
#
#   make                        build both
#   make run  FILE=x.cm         scan + parse x.cm, print syntax tree
#   make scan FILE=x.cm         scanner only, print tokens
#   make ... SCANNER=hand       use cminus_hand instead of cminus
#   make test                   run all tests with both executables
#   make clean                  remove generated files

# Disable make's built-in rules. Otherwise make knows the rule
# "%.c: %.l" and, whenever src/scan.l is newer than src/scan.c,
# silently REGENERATES src/scan.c from scan.l with flex,
# overwriting the hand-written scanner.
MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

CC      = gcc
CFLAGS  = -Wall -Wextra -Iinclude -Ibuild
LEX     = flex
YACC    = bison

BUILD   = build
FILE   ?= tests/parser/valid/gcd.cm
SCANNER ?= flex

ifeq ($(SCANNER),hand)
  EXE = ./cminus_hand
else
  EXE = ./cminus
endif

HEADERS = include/globals.h include/util.h include/scan.h \
          include/parse.h include/tree.h $(BUILD)/parse.tab.h
COMMON  = $(BUILD)/main.o $(BUILD)/util.o $(BUILD)/tree.o $(BUILD)/parse.tab.o

all: cminus cminus_hand

cminus: $(COMMON) $(BUILD)/lex.yy.o
	$(CC) $(CFLAGS) -o $@ $^

cminus_hand: $(COMMON) $(BUILD)/scan.o
	$(CC) $(CFLAGS) -o $@ $^

# bison: parse.y -> parse.tab.c (yyparse) + parse.tab.h (token codes)
#   -d  also write the header     -v  write parse.output (LALR states)
$(BUILD)/parse.tab.c $(BUILD)/parse.tab.h: src/parse.y | $(BUILD)
	$(YACC) -d -v -Wall -o $(BUILD)/parse.tab.c $<

# flex: scan.l -> lex.yy.c (the generated scanner, yylex())
$(BUILD)/lex.yy.c: src/scan.l | $(BUILD)
	$(LEX) -o $@ $<

$(BUILD)/parse.tab.o: $(BUILD)/parse.tab.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/lex.yy.o: $(BUILD)/lex.yy.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

# hand-written C sources (main, util, tree, scan)
$(BUILD)/%.o: src/%.c $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD):
	mkdir -p $(BUILD)

run: all
	$(EXE) $(FILE)

scan: all
	$(EXE) -s $(FILE)

test: all
	./tests/run_tests.sh

clean:
	rm -rf $(BUILD) cminus cminus_hand

.PHONY: all run scan test clean
