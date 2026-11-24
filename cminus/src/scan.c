/****************************************************/
/* File: scan.c                                     */
/* Hand-written scanner for the C-Minus compiler.   */
/* Modified from the TINY scanner (Louden): the     */
/* DFA in getToken() is extended to recognize the   */
/* C-Minus tokens of Appendix A.1.                  */
/* Provides the same interface (scan.h) as the      */
/* Flex scanner scan.l, so either can be linked.    */
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

#include <errno.h>
#include <limits.h>
#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in the scanner DFA
 *   TINY had: START, INASSIGN, INCOMMENT, INNUM, INID, DONE
 *   C-Minus : INASSIGN removed (":=" no longer exists);
 *             INLT/INGT/INEQ/INNOT decide between a one-
 *             and a two-character operator (< <=, > >=,
 *             = ==, ! !=); INSLASH decides between "/" and
 *             the start of a comment ('/' then '*'); INCOMMENT_STAR
 *             has seen '*' inside a comment and checks
 *             whether the next character is '/'.
 */
typedef enum
{
    START, INNUM, INID,
    INLT, INGT, INEQ, INNOT, INSLASH,
    INCOMMENT, INCOMMENT_STAR,
    DONE
} StateType;

/* lexeme of the most recent token */
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = length of the input buffer for source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN];   /* holds the current line           */
static int linepos = 0;        /* current position in lineBuf      */
static int bufsize = 0;        /* current size of buffer string    */
static int EOF_flag = FALSE;   /* corrects ungetNextChar at EOF    */
static int atLineStart = TRUE; /* previous chunk ended with '\n'?  */

/* line on which the current comment started */
static int commentStartLine = 0;

/* getNextChar fetches the next character from lineBuf,
 * reading in a new line if lineBuf is exhausted.
 * Changes from TINY:
 *  - a line longer than the buffer is read in pieces; the
 *    line number only advances at a real new line
 *  - lineno advances at EOF only if the last line ended with
 *    '\n' (same line numbers as the Flex scanner)
 *  - characters are returned as unsigned values, so bytes
 *    >= 128 are never confused with EOF (-1)
 */
static int getNextChar(void)
{
    if (linepos < bufsize)
        return (unsigned char) lineBuf[linepos++];

    if (EOF_flag)
        return EOF;

    if (fgets(lineBuf, BUFLEN, source) == NULL)
    {
        EOF_flag = TRUE;
        if (atLineStart)
            lineno++;
        return EOF;
    }

    bufsize = (int) strlen(lineBuf);
    linepos = 0;
    if (atLineStart)
    {
        lineno++;
        if (EchoSource)
            fprintf(listing, "%4d: ", lineno);
    }
    if (EchoSource)
    {
        fputs(lineBuf, listing);
        /* last line of the file may lack '\n'; keep listing tidy */
        if (lineBuf[bufsize - 1] != '\n' && feof(source))
            fputc('\n', listing);
    }
    atLineStart = (lineBuf[bufsize - 1] == '\n');
    return (unsigned char) lineBuf[linepos++];
}

/* ungetNextChar backtracks one character in lineBuf */
static void ungetNextChar(void)
{
    if (!EOF_flag)
        linepos--;
}

/* lookup table of reserved words (Appendix A.1, rule 1) */
#define MAXRESERVED 6

static struct
{
    const char *str;
    TokenType tok;
} reservedWords[MAXRESERVED] =
{
    {"else", ELSE}, {"if", IF}, {"int", INT},
    {"return", RETURN}, {"void", VOID}, {"while", WHILE}
};

/* lookup an identifier to see if it is a reserved word;
 * uses linear search, as in TINY */
static TokenType reservedLookup(const char *s)
{
    int i;
    for (i = 0; i < MAXRESERVED; i++)
        if (strcmp(s, reservedWords[i].str) == 0)
            return reservedWords[i].tok;
    return ID;
}

/* A.1 rule 3: letter = a..z | A..Z, digit = 0..9.
 * Explicit ASCII tests instead of isalpha()/isdigit(),
 * whose result can depend on the locale. */
static int isLetter(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int isDigit(int c)
{
    return c >= '0' && c <= '9';
}

/* reportLexError prints a lexical error with its line
 * number and marks the compilation as failed.
 * (Messages are identical to those of scan.l.) */
static void reportLexError(const char *message)
{
    fprintf(listing, "Lexical error at line %d: %s\n", lineno, message);
    Error = TRUE;
}

static void reportIllegalChar(int c)
{
    char message[64];
    if (isprint(c))
        snprintf(message, sizeof message, "illegal character '%c'", c);
    else
        snprintf(message, sizeof message, "illegal character (code 0x%02X)", c);
    reportLexError(message);
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* getToken returns the next token in the source file */
TokenType getToken(void)
{
    int tokenStringIndex = 0;  /* index for storing into tokenString */
    int tokenLength = 0;       /* full lexeme length (may exceed MAXTOKENLEN) */
    TokenType currentToken = ERROR;
    StateType state = START;   /* current state - always begins at START */
    const char *specialLexeme = NULL;
    int save;                  /* TRUE: store c into tokenString */

    while (state != DONE)
    {
        int c = getNextChar();
        save = TRUE;
        switch (state)
        {
            case START:
                if (isDigit(c))
                    state = INNUM;
                else if (isLetter(c))
                    state = INID;
                else if (c == '<')
                    state = INLT;
                else if (c == '>')
                    state = INGT;
                else if (c == '=')
                    state = INEQ;
                else if (c == '!')
                    state = INNOT;
                else if (c == '/')
                    state = INSLASH;
                else if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
                    save = FALSE;               /* skip white space */
                else
                {
                    state = DONE;
                    switch (c)
                    {
                        case EOF:
                            save = FALSE;
                            currentToken = ENDFILE;
                            break;
                        case '+': currentToken = PLUS;     break;
                        case '-': currentToken = MINUS;    break;
                        case '*': currentToken = TIMES;    break;
                        case ';': currentToken = SEMI;     break;
                        case ',': currentToken = COMMA;    break;
                        case '(': currentToken = LPAREN;   break;
                        case ')': currentToken = RPAREN;   break;
                        case '[': currentToken = LBRACKET; break;
                        case ']': currentToken = RBRACKET; break;
                        case '{': currentToken = LBRACE;   break;
                        case '}': currentToken = RBRACE;   break;
                        default:
                            currentToken = ERROR;
                            reportIllegalChar(c);
                            break;
                    }
                }
                break;

            case INNUM:
                if (!isDigit(c))
                {   /* backup in the input */
                    ungetNextChar();
                    save = FALSE;
                    state = DONE;
                    currentToken = NUM;
                }
                break;

            case INID:
                if (!isLetter(c))
                {   /* backup in the input */
                    ungetNextChar();
                    save = FALSE;
                    state = DONE;
                    currentToken = ID;
                }
                break;

            case INLT:                          /* seen '<' */
                state = DONE;
                if (c == '=')
                    currentToken = LTE;
                else
                {
                    ungetNextChar();
                    save = FALSE;
                    currentToken = LT;
                }
                break;

            case INGT:                          /* seen '>' */
                state = DONE;
                if (c == '=')
                    currentToken = GTE;
                else
                {
                    ungetNextChar();
                    save = FALSE;
                    currentToken = GT;
                }
                break;

            case INEQ:                          /* seen '=' */
                state = DONE;
                if (c == '=')
                    currentToken = EQ;
                else
                {
                    ungetNextChar();
                    save = FALSE;
                    currentToken = ASSIGN;
                }
                break;

            case INNOT:                         /* seen '!' */
                state = DONE;
                if (c == '=')
                    currentToken = NEQ;
                else
                {
                    ungetNextChar();
                    save = FALSE;
                    currentToken = ERROR;
                    reportLexError("'!' must be followed by '=' (C-Minus has no '!' operator)");
                }
                break;

            case INSLASH:                       /* seen '/' */
                if (c == '*')
                {   /* start of a comment: discard the saved "/" */
                    save = FALSE;
                    tokenStringIndex = 0;
                    tokenLength = 0;
                    commentStartLine = lineno;
                    state = INCOMMENT;
                }
                else
                {
                    ungetNextChar();
                    save = FALSE;
                    state = DONE;
                    currentToken = OVER;
                }
                break;

            case INCOMMENT:                     /* inside a comment */
            case INCOMMENT_STAR:                /* inside, just seen '*' */
                save = FALSE;
                if (c == EOF)
                {
                    char message[80];
                    snprintf(message, sizeof message,
                             "unterminated comment (opened at line %d)",
                             commentStartLine);
                    reportLexError(message);
                    specialLexeme = "unterminated comment";
                    state = DONE;
                    currentToken = ERROR;
                }
                else if (c == '*')
                    state = INCOMMENT_STAR;
                else if (state == INCOMMENT_STAR && c == '/')
                    state = START;              /* end of comment */
                else
                    state = INCOMMENT;
                break;

            case DONE:
            default: /* should never happen */
                fprintf(listing, "Scanner Bug: state= %d\n", state);
                state = DONE;
                currentToken = ERROR;
                break;
        }

        if (save)
        {
            if (tokenStringIndex < MAXTOKENLEN)
                tokenString[tokenStringIndex++] = (char) c;
            tokenLength++;
        }
    }
    tokenString[tokenStringIndex] = '\0';
    if (specialLexeme != NULL)
        snprintf(tokenString, sizeof tokenString, "%s", specialLexeme);

    /* semantic values for the parser (see parse.y %union) */
    if (currentToken == ID)
    {
        currentToken = reservedLookup(tokenString);
        if (currentToken == ID)
            yylval.name = copyString(tokenString);
    }
    else if (currentToken == NUM)
    {
        long value;
        errno = 0;
        value = strtol(tokenString, NULL, 10);
        if (errno == ERANGE || value > INT_MAX)
        {
            reportLexError("integer constant too large");
            currentToken = ERROR;
        }
        else
            yylval.value = (int) value;
    }

    /* an ID or NUM longer than MAXTOKENLEN is reported
     * instead of being silently truncated (as TINY did) */
    if ((currentToken == ID || currentToken == NUM) && tokenLength > MAXTOKENLEN)
    {
        char message[96];
        snprintf(message, sizeof message,
                 "token too long (%d characters, maximum is %d)",
                 tokenLength, MAXTOKENLEN);
        reportLexError(message);
        currentToken = ERROR;
    }

    /* the token's line, read by the parser as @n */
    yylloc.first_line = yylloc.last_line = lineno;

    if (TraceScan)
    {
        fprintf(listing, "\t%d: ", lineno);
        printToken(currentToken, tokenString);
    }
    return currentToken;
} /* end getToken */
