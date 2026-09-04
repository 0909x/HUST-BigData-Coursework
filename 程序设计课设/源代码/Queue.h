#ifndef COURSE_DESIGN_QUEUE_H
#define COURSE_DESIGN_QUEUE_H
#define _CRT_SECURE_NO_WARNINGS 1
//#include <mm_malloc.h>
#include "lexer.h"
#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1

typedef token_info Q_ElemType;
typedef int status;

//队列定义（有头结点）
typedef struct QueueNode
{
    Q_ElemType data;
    QueueNode* next;
} QueueNode; //队列链表结点
typedef struct Queue
{
    QueueNode* begin;
    QueueNode* end;
    int num; //队列元素个数
} Queue;     //队列

void iniQueue(Queue& Q);
status enQueue(Queue& Q, Q_ElemType e);
status deQueue(Queue& Q, Q_ElemType& e);
int Q_num(Queue Q);
Q_ElemType Q_get_first(Queue Q);
Q_ElemType Q_get_last(Queue Q);

#endif //COURSE_DESIGN_QUEUE_H