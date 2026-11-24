/****************************************************/
/* File: tree.c                                     */
/* Syntax tree construction and printing for the    */
/* C-Minus compiler.                                */
/* Adapted from newStmtNode/newExpNode/printTree in */
/* Louden's TINY util.c.                            */
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
#include "tree.h"

/* newNode allocates a node with all fields cleared */
static TreeNode *newNode(NodeKind nodekind, int line)
{
    TreeNode *t = malloc(sizeof(TreeNode));
    int i;

    if (t == NULL)
    {
        fprintf(stderr, "Out of memory error at line %d\n", line);
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < MAXCHILDREN; i++)
        t->child[i] = NULL;
    t->sibling   = NULL;
    t->lineno    = line;
    t->nodekind  = nodekind;
    t->attr.op   = 0;
    t->attr.val  = 0;
    t->attr.name = NULL;
    t->type      = Void;
    t->isArray   = FALSE;
    return t;
}

TreeNode *newDeclNode(DeclKind kind, int line)
{
    TreeNode *t = newNode(DeclK, line);
    t->kind.decl = kind;
    return t;
}

TreeNode *newStmtNode(StmtKind kind, int line)
{
    TreeNode *t = newNode(StmtK, line);
    t->kind.stmt = kind;
    return t;
}

TreeNode *newExpNode(ExpKind kind, int line)
{
    TreeNode *t = newNode(ExpK, line);
    t->kind.exp = kind;
    t->type = Integer; /* every C-Minus expression value is int */
    return t;
}

TreeNode *appendSibling(TreeNode *list, TreeNode *node)
{
    TreeNode *last;

    if (list == NULL)
        return node;
    if (node == NULL)
        return list;
    last = list;
    while (last->sibling != NULL)
        last = last->sibling;
    last->sibling = node;
    return list;
}

/*------------------------------------------------------
 * Printing
 *----------------------------------------------------*/

/* current indentation (number of spaces) */
static int indentno = 0;

#define INDENT   (indentno += 2)
#define UNINDENT (indentno -= 2)

static void printSpaces(void)
{
    int i;
    for (i = 0; i < indentno; i++)
        fputc(' ', listing);
}

static const char *typeName(ExpType type)
{
    return (type == Integer) ? "int" : "void";
}

static const char *operatorName(int op)
{
    switch (op)
    {
        case PLUS:  return "+";
        case MINUS: return "-";
        case TIMES: return "*";
        case OVER:  return "/";
        case LT:    return "<";
        case LTE:   return "<=";
        case GT:    return ">";
        case GTE:   return ">=";
        case EQ:    return "==";
        case NEQ:   return "!=";
        default:    return "?";
    }
}

static void printDecl(TreeNode *t)
{
    switch (t->kind.decl)
    {
        case VarK:
            if (t->isArray)
                fprintf(listing, "Variable declaration: %s[%d] (type %s) [line %d]\n",
                        t->attr.name, t->attr.val, typeName(t->type), t->lineno);
            else
                fprintf(listing, "Variable declaration: %s (type %s) [line %d]\n",
                        t->attr.name, typeName(t->type), t->lineno);
            break;
        case FunK:
            fprintf(listing, "Function declaration: %s (returns %s) [line %d]\n",
                    t->attr.name, typeName(t->type), t->lineno);
            break;
        case ParamK:
            fprintf(listing, "Parameter: %s%s (type %s)\n",
                    t->attr.name, t->isArray ? "[]" : "", typeName(t->type));
            break;
        default:
            fprintf(listing, "Unknown declaration kind\n");
    }
}

static void printStmt(TreeNode *t)
{
    switch (t->kind.stmt)
    {
        case CompoundK: fprintf(listing, "Compound statement [line %d]\n", t->lineno); break;
        case IfK:
            fprintf(listing, "If%s [line %d]\n",
                    t->child[2] != NULL ? " (with else)" : "", t->lineno);
            break;
        case WhileK:    fprintf(listing, "While [line %d]\n", t->lineno);  break;
        case ReturnK:   fprintf(listing, "Return [line %d]\n", t->lineno); break;
        default:        fprintf(listing, "Unknown statement kind\n");
    }
}

static void printExp(TreeNode *t)
{
    switch (t->kind.exp)
    {
        case AssignK: fprintf(listing, "Assign\n");                          break;
        case OpK:     fprintf(listing, "Op: %s\n", operatorName(t->attr.op)); break;
        case ConstK:  fprintf(listing, "Const: %d\n", t->attr.val);          break;
        case IdK:     fprintf(listing, "Id: %s\n", t->attr.name);            break;
        case ArrIdK:  fprintf(listing, "Array element: %s\n", t->attr.name); break;
        case CallK:   fprintf(listing, "Call: %s\n", t->attr.name);          break;
        default:      fprintf(listing, "Unknown expression kind\n");
    }
}

void printTree(TreeNode *tree)
{
    int i;

    INDENT;
    while (tree != NULL)
    {
        printSpaces();
        switch (tree->nodekind)
        {
            case DeclK: printDecl(tree); break;
            case StmtK: printStmt(tree); break;
            case ExpK:  printExp(tree);  break;
            default:    fprintf(listing, "Unknown node kind\n");
        }
        for (i = 0; i < MAXCHILDREN; i++)
            printTree(tree->child[i]);
        tree = tree->sibling;
    }
    UNINDENT;
}
