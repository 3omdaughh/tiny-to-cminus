/****************************************************/
/* File: util.h                                     */
/* Utility functions for the C-Minus compiler.      */
/* Adapted from Louden's TINY compiler.             */
/****************************************************/

#ifndef UTIL_H
#define UTIL_H

/* printToken prints a token and its lexeme
 * to the listing file
 */
void printToken(TokenType token, const char *lexeme);

/* copyString allocates and returns a new copy of s */
char *copyString(const char *s);

#endif
