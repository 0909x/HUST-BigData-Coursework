#ifndef COURSE_DESIGN_STACK_H
#define COURSE_DESIGN_STACK_H
#define _CRT_SECURE_NO_WARNINGS 1
//#include <mm_malloc.h>
#include "AST.h"
#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1

typedef p_treeNode S_ElemType;
typedef int status;

//栈定义（有头结点）
typedef struct StackNode
{
    S_ElemType data;
    StackNode* next;
} StackNode; //栈链表结点
typedef struct Stack
{
    StackNode* begin;
    int num; //栈元素个数
} Stack;     //栈

void iniStack(Stack& S);
status S_push(Stack& S, S_ElemType e);
status S_pop(Stack& S, S_ElemType& e);
int S_num(Stack S);
int isEmptyStack(Stack S);
S_ElemType S_get_top(Stack S);
void destroyStack(Stack S);

#endif //COURSE_DESIGN_STACK_H