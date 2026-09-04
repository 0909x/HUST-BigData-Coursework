#ifndef COURSE_DESIGN_PRINT_FILE_H
#define COURSE_DESIGN_PRINT_FILE_H
#define _CRT_SECURE_NO_WARNINGS 1
#include "parsing.h"

void f_printTabs(FILE* fp, int tabs);
void f_printFunCall(FILE* fp, p_treeNode fun_call);
void f_printExp(FILE* fp, p_treeNode p);
void f_traverseExp(FILE* fp, p_treeNode p, bool newRoll);
void f_traverse(FILE* fp, p_treeNode p, int tabs, bool newRoll);

#endif // COURSE_DESIGN_PRINT_FILE_H