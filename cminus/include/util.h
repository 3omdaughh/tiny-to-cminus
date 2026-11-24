/****************************************************/
/* File: util.h                                     */
/* Utility functions for the C-Minus compiler.      */
/* Adapted from Louden's TINY compiler.             */
/*                                                  */
/* Course  : CS.411 Compiler Design                 */
/* Team    :                                        */
/*   Mohamed Emad Hamdi      200044096              */
/*   AlOwida Elsayed         200042786              */
/*   Seif Adel Elbasha       200046355              */
/*   Amr Khaled Eid Diab     200045561              */
/*   Ali Muhammad Hassan     200039403              */
/*   Yousif Soliman Mohamed  200043875              */
/* Date    : 25 November 2026                       */
/* Modified: converted from TINY to C-Minus         */
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
