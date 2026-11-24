/****************************************************/
/* File: main.c                                     */
/* Main program for the C-Minus compiler front end  */
/* (lexical analysis + syntax analysis only).       */
/* Adapted from Louden's TINY compiler.             */
/****************************************************/

#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE TRUE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#endif

/* allocate global variables */
int lineno = 0;
FILE *source;
FILE *listing;

/* allocate and set tracing flags */
int EchoSource = TRUE;
int TraceScan  = TRUE;
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

int main(int argc, char *argv[])
{
    char pgm[MAXFILENAME]; /* source code file name */

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <filename>[.cm]\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (!buildFileName(pgm, sizeof pgm, argv[1]))
        return EXIT_FAILURE;

    source = fopen(pgm, "r");
    if (source == NULL)
    {
        fprintf(stderr, "Error: file %s not found\n", pgm);
        return EXIT_FAILURE;
    }

    listing = stdout; /* send listing to screen */
    fprintf(listing, "\nC-MINUS COMPILATION: %s\n", pgm);

#if NO_PARSE
    while (getToken() != ENDFILE)
        ; /* scan the whole file; getToken prints each token */
#else
    {
        TreeNode *syntaxTree = parse();
        if (TraceParse && !Error)
        {
            fprintf(listing, "\nSyntax tree:\n");
            printTree(syntaxTree);
        }
    }
#endif

    fclose(source);

    /* non-zero exit status if any error was reported,
     * so the test suite can detect failures */
    return Error ? EXIT_FAILURE : EXIT_SUCCESS;
}
