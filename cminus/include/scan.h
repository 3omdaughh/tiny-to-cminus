/****************************************************/
/* File: scan.h                                     */
/* Scanner interface for the C-Minus compiler.      */
/* Implemented by the Flex specification scan.l.    */
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

#ifndef SCAN_H
#define SCAN_H

/* MAXTOKENLEN is the maximum size of a token */
#define MAXTOKENLEN 40

/* tokenString holds the lexeme of the most recent token */
extern char tokenString[MAXTOKENLEN + 1];

/* getToken returns the next token in the source file */
TokenType getToken(void);

#endif
