/****************************************************/
/* File: globals.h                                  */
/* Global types and variables for the C-Minus       */
/* compiler front end (scanner + parser).           */
/* Must be included before the other headers.       */
/* Adapted from Louden's TINY compiler.             */
/****************************************************/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words (Appendix A.1, rule 1) */
#define MAXRESERVED 6

/* Every kind of token the scanner can return.
 * Source: Appendix A.1 lexical conventions.
 * NOTE: in Phase 4 this enum is replaced by the token
 * codes that Bison generates in parse.tab.h.
 */
typedef enum
{
    /* book-keeping tokens */
    ENDFILE, ERROR,

    /* reserved words: else if int return void while */
    ELSE, IF, INT, RETURN, VOID, WHILE,

    /* multi-character tokens */
    ID, NUM,

    /* special symbols */
    PLUS, MINUS, TIMES, OVER,              /* +  -  *  /          */
    LT, LTE, GT, GTE, EQ, NEQ,             /* <  <= >  >= == !=   */
    ASSIGN, SEMI, COMMA,                   /* =  ;  ,             */
    LPAREN, RPAREN,                        /* (  )                */
    LBRACKET, RBRACKET,                    /* [  ]                */
    LBRACE, RBRACE                         /* {  }                */
} TokenType;

extern FILE *source;   /* source code text file (.cm)      */
extern FILE *listing;  /* listing output text file (stdout) */

extern int lineno;     /* current source line number        */

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE: echo each source line, with its
 * line number, to the listing file as it is read.
 */
extern int EchoSource;

/* TraceScan = TRUE: print every token to the listing
 * file as the scanner recognizes it.
 */
extern int TraceScan;

/* TraceParse = TRUE: print the syntax tree after parsing. */
extern int TraceParse;

/* Error = TRUE once any lexical or syntax error is reported. */
extern int Error;

#endif
