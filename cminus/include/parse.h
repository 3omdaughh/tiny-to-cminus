/****************************************************/
/* File: parse.h                                    */
/* Parser interface for the C-Minus compiler.       */
/* Implemented by the Bison grammar parse.y.        */
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

#ifndef PARSE_H
#define PARSE_H

/* parse runs the parser and returns the syntax tree,
 * or NULL if a syntax error was found */
TreeNode *parse(void);

#endif
