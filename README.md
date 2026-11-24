# TINY to C-Minus: Compiler Front End

CS.411 Compiler Design project. The TINY scanner from Louden's
*Compiler Construction: Principles and Practice* is converted into a scanner
and parser for C-Minus (Appendix A of the book).

Team: Mohamed Emad Hamdi (200044096), AlOwida Elsayed (200042786),
Seif Adel Elbasha (200046355), Amr Khaled Eid Diab (200045561),
Ali Muhammad Hassan (200039403), Yousif Soliman Mohamed (200043875).

## Contents

```
tiny/                  original TINY scanner (reference, Step 1)
cminus/
├── include/           globals.h  util.h  scan.h  parse.h  tree.h
├── src/
│   ├── main.c         driver: accepts .cm files, -s = scanner only
│   ├── util.c         printToken, copyString
│   ├── scan.c         hand-written scanner (TINY DFA extended, Step 3)
│   ├── scan.l         Flex scanner (same interface and output as scan.c)
│   ├── parse.y        Bison parser, grammar of Appendix A.2
│   └── tree.c         syntax tree construction and printing
├── tests/             scanner/ and parser/ test programs + expected output
└── Makefile
```

## Requirements

gcc, flex, bison, make (tested with gcc 16.2.1, flex 2.6.4, bison 3.8.2, make 4.4.1).

## Build and run

```sh
cd cminus
make                                   # builds ./cminus (Flex) and ./cminus_hand (scan.c)

./cminus -s tests/parser/valid/gcd.cm  # scanner only: token listing
./cminus    tests/parser/valid/gcd.cm  # scanner + parser: syntax tree
./cminus_hand -s tests/parser/valid/gcd.cm

make scan FILE=x.cm                    # same, through make
make run  FILE=x.cm  SCANNER=hand      # SCANNER=hand selects cminus_hand
make test                              # all tests, both executables
make clean
```

A file name without an extension gets `.cm` appended; other extensions are rejected.
The exit status is 0 if no errors were found, 1 otherwise.

To build and run the original TINY scanner: `cd tiny && make run FILE=sample.tny`.

## Tests

`make test` runs every test program with **both** executables and compares the
output with the stored `.expected` file (listing + exit status).

| # | Category | Test files |
|---|----------|-----------|
| 1 | Every C-Minus token | `scanner/valid/all_tokens.cm` |
| 2 | Keywords vs identifiers | `scanner/valid/keywords_vs_ids.cm` |
| 3 | Integer constants | `scanner/valid/numbers.cm`, `scanner/invalid/number_too_large.cm` |
| 4 | Operators and delimiters | `scanner/valid/all_tokens.cm`, `scanner/valid/numbers.cm` |
| 5 | Comments and white space | `scanner/valid/comments.cm`, `scanner/valid/whitespace.cm` |
| 6 | Variable declarations | `parser/valid/declarations.cm` |
| 7 | Function declarations | `parser/valid/functions.cm`, `parser/valid/gcd.cm` |
| 8 | Parameters and arguments | `parser/valid/functions.cm`, `parser/valid/arrays_loops.cm` |
| 9 | Arithmetic and relational expressions | `parser/valid/precedence.cm`, `parser/valid/conditionals.cm` |
| 10 | Conditional statements | `parser/valid/conditionals.cm`, `parser/valid/dangling_else.cm` |
| 11 | Iteration statements | `parser/valid/iteration.cm`, `parser/valid/arrays_loops.cm` |
| 12 | Return statements | `parser/valid/functions.cm`, `parser/valid/gcd.cm` |
| 13 | Nested compound statements | `parser/valid/nested_compound.cm` |
| 14 | Invalid characters | `scanner/invalid/illegal_chars.cm`, `scanner/invalid/lone_bang.cm` |
| 15 | Invalid token sequences | `parser/invalid/double_operator.cm`, `digit_in_id.cm`, `keyword_as_identifier.cm`, `two_relops.cm`, `trailing_comma.cm` |
| 16 | Syntax errors / malformed programs | `parser/invalid/*` (missing `;`, `}`, `)`, type; `else` without `if`; empty program; ...), `scanner/invalid/unterminated_comment.cm`, `scanner/invalid/too_long.cm` |

## Known limitations

- The parser stops at the first syntax error (no error recovery).
- A missing `;` is reported on the line of the next token, where Bison detects it.
- No semantic analysis (by design): e.g. `void v;` or use of an undeclared
  variable is accepted, because both are syntactically legal.
- The grammar in `parse.y` follows Appendix A.2 (29 rules) but has not yet
  been checked line by line against the printed appendix.
