/****************************************************/
/* File: scan.h                                     */
/* Scanner interface for the C-Minus compiler.      */
/* Implemented by the Flex specification scan.l.    */
/* Adapted from Louden's TINY compiler.             */
/****************************************************/

#ifndef SCAN_H
#define SCAN_H

/* MAXTOKENLEN is the maximum size of a token */
#define MAXTOKENLEN 40

/* tokenString holds the lexeme of the most recent token */
extern char tokenString[MAXTOKENLEN + 1];

/* getToken returns the next token in the source file */
TokenType getToken(void);

#endif
