/****************************************************/
/* File: main.c                                     */
/* Main program for the C-Minus compiler front end  */
/* (lexical analysis + syntax analysis only).       */
/* Adapted from Louden's TINY compiler.             */
/*                                                  */
/* Usage:  cminus [-s] <filename>[.cm]              */
/*   -s   scanner only: print the token stream      */
/*   (default) scan + parse, print the syntax tree  */
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

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

/* allocate global variables */
int lineno = 0;
FILE *source;
FILE *listing;

/* allocate tracing flags (set in main from the options) */
int EchoSource = TRUE;
int TraceScan  = FALSE;
int TraceParse = FALSE;

int Error = FALSE;

/* maximum length of the source file name, including ".cm" */
#define MAXFILENAME 256

/* buildFileName copies the command-line argument into pgm.
 * - No extension   -> ".cm" is appended   (gcd   -> gcd.cm)
 * - ".cm"          -> accepted unchanged  (gcd.cm)
 * - Any other one  -> rejected            (gcd.c)
 * Only the last path component is examined, so a dot in a
 * directory name (e.g. "../tests/gcd") is not mistaken for
 * an extension.
 * Returns TRUE on success, FALSE on error.
 */
static int buildFileName(char *pgm, size_t size, const char *arg)
{
    const char *base = strrchr(arg, '/');   /* last path separator */
    const char *dot;
    int written;

    base = (base == NULL) ? arg : base + 1;
    dot = strrchr(base, '.');

    if (dot == NULL)
        written = snprintf(pgm, size, "%s.cm", arg);
    else if (strcmp(dot, ".cm") == 0)
        written = snprintf(pgm, size, "%s", arg);
    else
    {
        fprintf(stderr, "Error: '%s' is not a C-Minus source file "
                        "(expected extension .cm)\n", arg);
        return FALSE;
    }

    if (written < 0 || (size_t) written >= size)
    {
        fprintf(stderr, "Error: file name too long: %s\n", arg);
        return FALSE;
    }
    return TRUE;
}

static void usage(const char *program)
{
    fprintf(stderr, "usage: %s [-s] <filename>[.cm]\n"
                    "  -s   scanner only (print tokens)\n",
            program);
}

int main(int argc, char *argv[])
{
    char pgm[MAXFILENAME]; /* source code file name */
    int scanOnly = FALSE;
    const char *fileArg;

    if (argc == 3 && strcmp(argv[1], "-s") == 0)
    {
        scanOnly = TRUE;
        fileArg = argv[2];
    }
    else if (argc == 2 && argv[1][0] != '-')
        fileArg = argv[1];
    else
    {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (!buildFileName(pgm, sizeof pgm, fileArg))
        return EXIT_FAILURE;

    source = fopen(pgm, "r");
    if (source == NULL)
    {
        fprintf(stderr, "Error: file %s not found\n", pgm);
        return EXIT_FAILURE;
    }

    listing = stdout; /* send listing to screen */
    fprintf(listing, "\nC-MINUS COMPILATION: %s\n", pgm);

    if (scanOnly)
    {
        /* lexical analysis only: getToken prints each token */
        TraceScan = TRUE;
        while (getToken() != ENDFILE)
            ;
    }
    else
    {
        /* syntax analysis: yyparse pulls tokens via getToken */
        TreeNode *syntaxTree;

        TraceParse = TRUE;
        syntaxTree = parse();
        if (Error)
            fprintf(listing, "\nParsing stopped: errors were found.\n");
        else if (TraceParse)
        {
            fprintf(listing, "\nSyntax tree:\n");
            printTree(syntaxTree);
        }
    }

    fclose(source);

    /* non-zero exit status if any error was reported,
     * so the test suite can detect failures */
    return Error ? EXIT_FAILURE : EXIT_SUCCESS;
}
