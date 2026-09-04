#ifndef COURSE_DESIGN_TRAVERSE_H
#define COURSE_DESIGN_TRAVERSE_H
#define _CRT_SECURE_NO_WARNINGS 1
#include "parsing.h"

void printTabs(int tabs);
void printFunCall(p_treeNode fun_call);
void traverseExp(p_treeNode p);
void traverse(p_treeNode p, int tabs);

#endif //COURSE_DESIGN_TRAVERSE_H