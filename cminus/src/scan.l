/****************************************************/
/* File: scan.l                                     */
/* Flex specification for the C-Minus scanner.      */
/* Token definitions follow Appendix A.1 (Louden).  */
/* Replaces the hand-written DFA in TINY's scan.c.  */
/****************************************************/

/*======================================================
 * SECTION 1: DEFINITIONS
 *   - C code between %{ and %} is copied verbatim to
 *     the top of the generated lex.yy.c
 *   - %option lines configure Flex
 *   - name  pattern  lines define reusable patterns
 *====================================================*/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"

/* lexeme of the most recent token (declared in scan.h) */
char tokenString[MAXTOKENLEN + 1];

/* line on which the current comment started, so an
 * unterminated comment can be reported precisely */
static int commentStartLine = 0;

/* When an error token has no meaningful yytext (e.g. at
 * end of file) the rule stores a description here and
 * getToken() uses it instead of yytext. */
static const char *specialLexeme = NULL;

/* reportLexError prints a lexical error with its line
 * number and marks the compilation as failed. */
static void reportLexError(const char *message)
{
    fprintf(listing, "Lexical error at line %d: %s\n", lineno, message);
    Error = TRUE;
}

/*------------------------------------------------------
 * Source echo (EchoSource flag).
 * YY_INPUT is the hook Flex calls whenever it needs more
 * characters. We redefine it to read ONE line at a time
 * with fgets() and print that line with its line number,
 * reproducing TINY's listing format.
 *----------------------------------------------------*/
static int echoLineNo = 0;     /* number of the line being echoed */
static int atLineStart = TRUE; /* last chunk ended with '\n'?     */

static int readSourceLine(char *buf, int maxSize)
{
    size_t len;

    if (fgets(buf, maxSize, source) == NULL)
        return 0;                          /* 0 tells Flex: end of file */
    len = strlen(buf);

    if (EchoSource)
    {
        if (atLineStart)
            fprintf(listing, "%4d: ", ++echoLineNo);
        fputs(buf, listing);
        /* last line of the file may lack '\n'; keep listing tidy */
        if (buf[len - 1] != '\n' && feof(source))
            fputc('\n', listing);
    }
    atLineStart = (buf[len - 1] == '\n');
    return (int) len;
}

#define YY_INPUT(buf, result, max_size) \
    { result = readSourceLine((buf), (int) (max_size)); }
%}

/* noyywrap : only one input file, no yywrap() needed
 * nounput  : unput() is not used (avoids a warning)
 * noinput  : input() is not used (avoids a warning) */
%option noyywrap nounput noinput

/* exclusive start condition: inside a comment, ONLY the
 * rules prefixed with <COMMENT> are active */
%x COMMENT

digit       [0-9]
letter      [a-zA-Z]
number      {digit}+
identifier  {letter}+
whitespace  [ \t\r]+

%%
 /*======================================================
  * SECTION 2: RULES   (pattern   { C action })
  * Flex picks the LONGEST match; on a tie, the rule
  * written FIRST wins. Hence keywords before {identifier}.
  *====================================================*/

 /* ---- reserved words (A.1 rule 1) ---- */
"else"          { return ELSE;   }
"if"            { return IF;     }
"int"           { return INT;    }
"return"        { return RETURN; }
"void"          { return VOID;   }
"while"         { return WHILE;  }

 /* ---- special symbols (A.1 rule 2) ----
  * two-character operators are matched in preference to
  * their one-character prefixes by the longest-match rule */
"<="            { return LTE;      }
">="            { return GTE;      }
"=="            { return EQ;       }
"!="            { return NEQ;      }
"+"             { return PLUS;     }
"-"             { return MINUS;    }
"*"             { return TIMES;    }
"/"             { return OVER;     }
"<"             { return LT;       }
">"             { return GT;       }
"="             { return ASSIGN;   }
";"             { return SEMI;     }
","             { return COMMA;    }
"("             { return LPAREN;   }
")"             { return RPAREN;   }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }
"{"             { return LBRACE;   }
"}"             { return RBRACE;   }

 /* ---- ID and NUM (A.1 rule 3) ---- */
{number}        { return NUM; }
{identifier}    { return ID;  }

 /* ---- white space (A.1 rule 4) ---- */
\n              { lineno++; }
{whitespace}    { /* skip blanks, tabs, carriage returns */ }

 /* ---- comments (A.1 rule 5): enter COMMENT mode ---- */
"/*"            { commentStartLine = lineno; BEGIN(COMMENT); }

<COMMENT>"*/"       { BEGIN(INITIAL);   /* back to normal scanning */ }
<COMMENT>\n         { lineno++;         /* comments may span lines */ }
<COMMENT>[^*\n]+    { /* skip comment text */ }
<COMMENT>"*"        { /* a '*' not followed by '/' */ }
<COMMENT><<EOF>>    {
                      char message[80];
                      snprintf(message, sizeof message,
                               "unterminated comment (opened at line %d)",
                               commentStartLine);
                      reportLexError(message);
                      specialLexeme = "unterminated comment";
                      BEGIN(INITIAL);   /* next call returns ENDFILE */
                      return ERROR;
                    }

 /* ---- errors ---- */
"!"             {
                  reportLexError("'!' must be followed by '=' (C-Minus has no '!' operator)");
                  return ERROR;
                }
.               {
                  char message[64];
                  unsigned char c = (unsigned char) yytext[0];
                  if (isprint(c))
                      snprintf(message, sizeof message,
                               "illegal character '%c'", c);
                  else
                      snprintf(message, sizeof message,
                               "illegal character (code 0x%02X)", c);
                  reportLexError(message);
                  return ERROR;
                }

%%
/*======================================================
 * SECTION 3: USER CODE  (copied verbatim to lex.yy.c)
 *====================================================*/

/* getToken returns the next token in the source file.
 * It wraps the Flex-generated yylex() so that the rest of
 * the compiler keeps TINY's interface (see scan.h).
 */
TokenType getToken(void)
{
    static int firstTime = TRUE;
    TokenType currentToken;

    if (firstTime)
    {
        firstTime = FALSE;
        lineno = 1;       /* scanning starts on line 1 */
        yyin = source;    /* Flex reads from the .cm file */
        yyout = listing;
    }

    specialLexeme = NULL;
    /* yylex() returns 0 at end of file; ENDFILE is 0 */
    currentToken = (TokenType) yylex();

    if (specialLexeme != NULL)
        snprintf(tokenString, sizeof tokenString, "%s", specialLexeme);
    else
        snprintf(tokenString, sizeof tokenString, "%s", yytext);

    /* tokenString holds at most MAXTOKENLEN characters;
     * a longer ID or NUM is reported instead of being
     * silently truncated (TINY truncated silently). */
    if ((currentToken == ID || currentToken == NUM) && yyleng > MAXTOKENLEN)
    {
        char message[96];
        snprintf(message, sizeof message,
                 "token too long (%d characters, maximum is %d)",
                 (int) yyleng, MAXTOKENLEN);
        reportLexError(message);
        currentToken = ERROR;
    }

    if (TraceScan)
    {
        fprintf(listing, "\t%d: ", lineno);
        printToken(currentToken, tokenString);
    }
    return currentToken;
}
