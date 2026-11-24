/****************************************************/
/* File: util.c                                     */
/* Utility function implementation for the          */
/* C-Minus compiler.                                */
/* Adapted from Louden's TINY compiler.             */
/****************************************************/

#include "globals.h"
#include "util.h"

/* printToken prints a token and its lexeme
 * to the listing file
 */
void printToken(TokenType token, const char *lexeme)
{
    switch (token)
    {
        /* reserved words */
        case ELSE:
        case IF:
        case INT:
        case RETURN:
        case VOID:
        case WHILE:
            fprintf(listing, "reserved word: %s\n", lexeme);
            break;

        /* special symbols */
        case PLUS:     fprintf(listing, "+\n");  break;
        case MINUS:    fprintf(listing, "-\n");  break;
        case TIMES:    fprintf(listing, "*\n");  break;
        case OVER:     fprintf(listing, "/\n");  break;
        case LT:       fprintf(listing, "<\n");  break;
        case LTE:      fprintf(listing, "<=\n"); break;
        case GT:       fprintf(listing, ">\n");  break;
        case GTE:      fprintf(listing, ">=\n"); break;
        case EQ:       fprintf(listing, "==\n"); break;
        case NEQ:      fprintf(listing, "!=\n"); break;
        case ASSIGN:   fprintf(listing, "=\n");  break;
        case SEMI:     fprintf(listing, ";\n");  break;
        case COMMA:    fprintf(listing, ",\n");  break;
        case LPAREN:   fprintf(listing, "(\n");  break;
        case RPAREN:   fprintf(listing, ")\n");  break;
        case LBRACKET: fprintf(listing, "[\n");  break;
        case RBRACKET: fprintf(listing, "]\n");  break;
        case LBRACE:   fprintf(listing, "{\n");  break;
        case RBRACE:   fprintf(listing, "}\n");  break;

        /* multi-character tokens */
        case NUM:
            fprintf(listing, "NUM, val= %s\n", lexeme);
            break;
        case ID:
            fprintf(listing, "ID, name= %s\n", lexeme);
            break;

        /* book-keeping tokens */
        case ENDFILE:
            fprintf(listing, "EOF\n");
            break;
        case ERROR:
            fprintf(listing, "ERROR: %s\n", lexeme);
            break;

        default: /* should never happen */
            fprintf(listing, "Unknown token: %d\n", token);
    }
}

/* copyString allocates and returns a new copy of s */
char *copyString(const char *s)
{
    char *copy;

    if (s == NULL)
        return NULL;
    copy = malloc(strlen(s) + 1);
    if (copy == NULL)
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    else
        strcpy(copy, s);
    return copy;
}
