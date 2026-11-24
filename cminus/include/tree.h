/****************************************************/
/* File: tree.h                                     */
/* Syntax tree for the C-Minus compiler.            */
/* Adapted from the TreeNode design of Louden's     */
/* TINY compiler (globals.h), extended for C-Minus  */
/* declarations, functions, arrays and calls.       */
/*                                                  */
/* This header is self-contained on purpose: Bison  */
/* includes it from parse.tab.h (via %code requires)*/
/* so it must not include globals.h.                */
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

#ifndef TREE_H
#define TREE_H

/* Each node is a declaration, a statement or an expression */
typedef enum { DeclK, StmtK, ExpK } NodeKind;

/* declarations */
typedef enum
{
    VarK,      /* int x;  or  int a[10];                 */
    FunK,      /* int f(params) { ... }                  */
    ParamK     /* one parameter: int x  or  int a[]      */
} DeclKind;

/* statements */
typedef enum
{
    CompoundK, /* { local-declarations statement-list }  */
    IfK,       /* if (test) stmt [else stmt]             */
    WhileK,    /* while (test) stmt                      */
    ReturnK    /* return [expression];                   */
} StmtKind;

/* expressions */
typedef enum
{
    AssignK,   /* var = expression                       */
    OpK,       /* binary operator: + - * / < <= ...      */
    ConstK,    /* integer constant (NUM)                 */
    IdK,       /* simple variable: x                     */
    ArrIdK,    /* array element: a[expression]           */
    CallK      /* function call: f(args)                 */
} ExpKind;

/* the two C-Minus types (A.2 rule 5: type-specifier) */
typedef enum { Void, Integer } ExpType;

#define MAXCHILDREN 3

/* Children by node kind:
 *   FunK      child[0] = parameters (sibling list), child[1] = body
 *   CompoundK child[0] = local declarations,        child[1] = statements
 *   IfK       child[0] = test, child[1] = then-part, child[2] = else-part
 *   WhileK    child[0] = test, child[1] = body
 *   ReturnK   child[0] = expression (NULL for "return;")
 *   AssignK   child[0] = variable, child[1] = value
 *   OpK       child[0] = left operand, child[1] = right operand
 *   ArrIdK    child[0] = index expression
 *   CallK     child[0] = arguments (sibling list)
 * Lists (declarations, statements, params, args) are
 * chained through the sibling pointer, as in TINY.
 */
typedef struct treeNode
{
    struct treeNode *child[MAXCHILDREN];
    struct treeNode *sibling;
    int lineno;                 /* source line of this construct */
    NodeKind nodekind;
    union { DeclKind decl; StmtKind stmt; ExpKind exp; } kind;
    struct
    {
        int   op;               /* OpK: operator token (TokenType)  */
        int   val;              /* ConstK: value; VarK: array size  */
        char *name;             /* VarK, FunK, ParamK, IdK, ArrIdK, CallK */
    } attr;
    ExpType type;               /* declared type (VarK, FunK, ParamK) */
    int isArray;                /* VarK / ParamK declared as array    */
} TreeNode;

/* node constructors: line = source line of the construct */
TreeNode *newDeclNode(DeclKind kind, int line);
TreeNode *newStmtNode(StmtKind kind, int line);
TreeNode *newExpNode(ExpKind kind, int line);

/* appendSibling adds node to the end of the sibling list
 * starting at list and returns the (possibly new) head.
 * Either argument may be NULL (e.g. an empty statement). */
TreeNode *appendSibling(TreeNode *list, TreeNode *node);

/* printTree prints the syntax tree to the listing file,
 * indenting children under their parent */
void printTree(TreeNode *tree);

#endif
