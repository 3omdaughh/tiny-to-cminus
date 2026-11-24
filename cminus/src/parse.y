/****************************************************/
/* File: parse.y                                    */
/* Bison grammar for the C-Minus parser.            */
/* Grammar: Appendix A.2 (Louden), 29 BNF rules.    */
/* Rule numbers in comments refer to Appendix A.2.  */
/* Builds a syntax tree (see tree.h).               */
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

/*======================================================
 * SECTION 1: DECLARATIONS
 *====================================================*/

/* Code that must appear in parse.tab.h itself, BEFORE the
 * semantic-value union, because the union mentions
 * TreeNode and ExpType. */
%code requires {
#include "tree.h"
}

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

/* root of the syntax tree, set by the "program" rule */
static TreeNode *savedTree = NULL;

/* Bison calls yylex() to get each token. We route it
 * through getToken() (scan.l) so the token trace and the
 * line/value bookkeeping happen in one place. This is the
 * same arrangement Louden uses in his Yacc version of TINY. */
static int yylex(void);

/* Bison calls yyerror() when it detects a syntax error */
static void yyerror(const char *message);
%}

/* the generated parse.tab.c includes parse.tab.h instead of
 * duplicating its contents */
%define api.header.include {"parse.tab.h"}

/* detailed messages: "unexpected X, expecting Y" */
%define parse.error verbose

/* track the source line of every token and rule (@n) */
%locations

/* Semantic values: the data attached to a token or a
 * grammar symbol. Each %token / %type below says which
 * member of this union it uses. */
%union {
    TreeNode *node;   /* most grammar symbols: a subtree     */
    char     *name;   /* ID: the identifier's spelling       */
    int       value;  /* NUM: the integer value              */
    ExpType   type;   /* type-specifier: int or void         */
    int       op;     /* relop/addop/mulop: operator token   */
}

/* ---- tokens (Appendix A.1). The quoted strings are
 *      the names used in error messages. ---- */
%token ENDFILE 0 "end of file"
%token ERROR     "illegal token"

%token ELSE "else" IF "if" INT "int" RETURN "return" VOID "void" WHILE "while"

%token <name>  ID  "identifier"
%token <value> NUM "number"

%token PLUS "+" MINUS "-" TIMES "*" OVER "/"
%token LT "<" LTE "<=" GT ">" GTE ">=" EQ "==" NEQ "!="
%token ASSIGN "=" SEMI ";" COMMA ","
%token LPAREN "(" RPAREN ")" LBRACKET "[" RBRACKET "]" LBRACE "{" RBRACE "}"

/* ---- nonterminals and their semantic-value types ---- */
%type <node> program declaration_list declaration var_declaration
%type <node> fun_declaration params param_list param compound_stmt
%type <node> local_declarations statement_list statement
%type <node> expression_stmt selection_stmt iteration_stmt return_stmt
%type <node> expression var simple_expression additive_expression
%type <node> term factor call args arg_list
%type <type> type_specifier
%type <op>   relop addop mulop

/* Dangling else (rule 15). Without these two lines Bison
 * reports 1 shift/reduce conflict. They give the rule
 * "if (...) statement" a LOWER precedence than the token
 * ELSE, so on seeing "else" Bison SHIFTS: the else is
 * attached to the nearest unmatched if.
 * %precedence (Bison) sets precedence only; classic Yacc
 * would write %nonassoc, but associativity is never used
 * here because the two are on different levels. */
%precedence LOWER_THAN_ELSE
%precedence ELSE

%%
 /*======================================================
  * SECTION 2: GRAMMAR RULES  (A.2 rules 1-29)
  *   $$      = semantic value of the left-hand side
  *   $1..$n  = semantic values of the right-hand symbols
  *   @n      = location (line) of the n-th symbol
  *====================================================*/

/* 1 */
program
    : declaration_list                  { savedTree = $1; }
    ;

/* 2: left recursion -> declarations kept in source order */
declaration_list
    : declaration_list declaration      { $$ = appendSibling($1, $2); }
    | declaration                       { $$ = $1; }
    ;

/* 3 */
declaration
    : var_declaration                   { $$ = $1; }
    | fun_declaration                   { $$ = $1; }
    ;

/* 4 */
var_declaration
    : type_specifier ID SEMI
        { $$ = newDeclNode(VarK, @2.first_line);
          $$->type = $1;
          $$->attr.name = $2; }
    | type_specifier ID LBRACKET NUM RBRACKET SEMI
        { $$ = newDeclNode(VarK, @2.first_line);
          $$->type = $1;
          $$->attr.name = $2;
          $$->isArray = TRUE;
          $$->attr.val = $4; }
    ;

/* 5 */
type_specifier
    : INT                               { $$ = Integer; }
    | VOID                              { $$ = Void; }
    ;

/* 6 */
fun_declaration
    : type_specifier ID LPAREN params RPAREN compound_stmt
        { $$ = newDeclNode(FunK, @2.first_line);
          $$->type = $1;
          $$->attr.name = $2;
          $$->child[0] = $4;
          $$->child[1] = $6; }
    ;

/* 7: "(void)" means no parameters -> empty list */
params
    : param_list                        { $$ = $1; }
    | VOID                              { $$ = NULL; }
    ;

/* 8 */
param_list
    : param_list COMMA param            { $$ = appendSibling($1, $3); }
    | param                             { $$ = $1; }
    ;

/* 9 */
param
    : type_specifier ID
        { $$ = newDeclNode(ParamK, @2.first_line);
          $$->type = $1;
          $$->attr.name = $2; }
    | type_specifier ID LBRACKET RBRACKET
        { $$ = newDeclNode(ParamK, @2.first_line);
          $$->type = $1;
          $$->attr.name = $2;
          $$->isArray = TRUE; }
    ;

/* 10 */
compound_stmt
    : LBRACE local_declarations statement_list RBRACE
        { $$ = newStmtNode(CompoundK, @1.first_line);
          $$->child[0] = $2;
          $$->child[1] = $3; }
    ;

/* 11 */
local_declarations
    : local_declarations var_declaration { $$ = appendSibling($1, $2); }
    | %empty                             { $$ = NULL; }
    ;

/* 12 */
statement_list
    : statement_list statement          { $$ = appendSibling($1, $2); }
    | %empty                            { $$ = NULL; }
    ;

/* 13 */
statement
    : expression_stmt                   { $$ = $1; }
    | compound_stmt                     { $$ = $1; }
    | selection_stmt                    { $$ = $1; }
    | iteration_stmt                    { $$ = $1; }
    | return_stmt                       { $$ = $1; }
    ;

/* 14: an empty statement ";" produces no node */
expression_stmt
    : expression SEMI                   { $$ = $1; }
    | SEMI                              { $$ = NULL; }
    ;

/* 15: dangling else resolved by %prec (see section 1) */
selection_stmt
    : IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
        { $$ = newStmtNode(IfK, @1.first_line);
          $$->child[0] = $3;
          $$->child[1] = $5; }
    | IF LPAREN expression RPAREN statement ELSE statement
        { $$ = newStmtNode(IfK, @1.first_line);
          $$->child[0] = $3;
          $$->child[1] = $5;
          $$->child[2] = $7; }
    ;

/* 16 */
iteration_stmt
    : WHILE LPAREN expression RPAREN statement
        { $$ = newStmtNode(WhileK, @1.first_line);
          $$->child[0] = $3;
          $$->child[1] = $5; }
    ;

/* 17 */
return_stmt
    : RETURN SEMI
        { $$ = newStmtNode(ReturnK, @1.first_line); }
    | RETURN expression SEMI
        { $$ = newStmtNode(ReturnK, @1.first_line);
          $$->child[0] = $2; }
    ;

/* 18: right recursion -> a = b = c means a = (b = c) */
expression
    : var ASSIGN expression
        { $$ = newExpNode(AssignK, @2.first_line);
          $$->child[0] = $1;
          $$->child[1] = $3; }
    | simple_expression                 { $$ = $1; }
    ;

/* 19 */
var
    : ID
        { $$ = newExpNode(IdK, @1.first_line);
          $$->attr.name = $1; }
    | ID LBRACKET expression RBRACKET
        { $$ = newExpNode(ArrIdK, @1.first_line);
          $$->attr.name = $1;
          $$->child[0] = $3; }
    ;

/* 20: at most ONE relational operator (non-associative) */
simple_expression
    : additive_expression relop additive_expression
        { $$ = newExpNode(OpK, @2.first_line);
          $$->attr.op = $2;
          $$->child[0] = $1;
          $$->child[1] = $3; }
    | additive_expression               { $$ = $1; }
    ;

/* 21 */
relop
    : LTE   { $$ = LTE; }
    | LT    { $$ = LT;  }
    | GT    { $$ = GT;  }
    | GTE   { $$ = GTE; }
    | EQ    { $$ = EQ;  }
    | NEQ   { $$ = NEQ; }
    ;

/* 22: left recursion -> a - b - c means (a - b) - c */
additive_expression
    : additive_expression addop term
        { $$ = newExpNode(OpK, @2.first_line);
          $$->attr.op = $2;
          $$->child[0] = $1;
          $$->child[1] = $3; }
    | term                              { $$ = $1; }
    ;

/* 23 */
addop
    : PLUS  { $$ = PLUS;  }
    | MINUS { $$ = MINUS; }
    ;

/* 24: term is one level below additive-expression,
 *     so * and / bind tighter than + and - */
term
    : term mulop factor
        { $$ = newExpNode(OpK, @2.first_line);
          $$->attr.op = $2;
          $$->child[0] = $1;
          $$->child[1] = $3; }
    | factor                            { $$ = $1; }
    ;

/* 25 */
mulop
    : TIMES { $$ = TIMES; }
    | OVER  { $$ = OVER;  }
    ;

/* 26: parentheses need no node of their own; the tree
 *     shape already records the grouping */
factor
    : LPAREN expression RPAREN          { $$ = $2; }
    | var                               { $$ = $1; }
    | call                              { $$ = $1; }
    | NUM
        { $$ = newExpNode(ConstK, @1.first_line);
          $$->attr.val = $1; }
    ;

/* 27 */
call
    : ID LPAREN args RPAREN
        { $$ = newExpNode(CallK, @1.first_line);
          $$->attr.name = $1;
          $$->child[0] = $3; }
    ;

/* 28 */
args
    : arg_list                          { $$ = $1; }
    | %empty                            { $$ = NULL; }
    ;

/* 29 */
arg_list
    : arg_list COMMA expression         { $$ = appendSibling($1, $3); }
    | expression                        { $$ = $1; }
    ;

%%
/*======================================================
 * SECTION 3: SUPPORTING C CODE
 *====================================================*/

static int yylex(void)
{
    return getToken();
}

/* Bison's verbose message starts with "syntax error, ";
 * we drop that prefix and add the line number instead. */
static void yyerror(const char *message)
{
    const char *prefix = "syntax error, ";
    size_t prefixLength = strlen(prefix);

    if (strncmp(message, prefix, prefixLength) == 0)
        message += prefixLength;
    fprintf(listing, "Syntax error at line %d: %s\n",
            yylloc.first_line, message);
    Error = TRUE;
}

/* parse returns the syntax tree, or NULL on a syntax error */
TreeNode *parse(void)
{
    if (yyparse() != 0)
        return NULL;
    return savedTree;
}
