#include "Queue.h"
#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS 1
//队列函数

/**
 * @description: 初始化队列
 * @param {Queue} Q
 * @return
 */
void iniQueue(Queue& Q)
{
    Q.begin = (QueueNode*)malloc(sizeof(QueueNode));
    Q.end = Q.begin;
    Q.begin->next = NULL;
    Q.num = 0;
    return;
}

/**
 * @description: 将元素e入队列
 * @param {Queue} &Q
 * @param {Q_ElemType} e
 * @return 成功返回OK，失败返回ERROR
 */
status enQueue(Queue& Q, Q_ElemType e)
{
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!new_node)
        return ERROR;
    new_node->next = NULL;
    new_node->data = e;
    Q.end->next = new_node;
    Q.end = Q.end->next;
    Q.num += 1;
    return OK;
}

/**
 * @description: 将队列首元素出队，赋值给e
 * @param {Queue} &Q
 * @param {Q_ElemType} &e
 * @return 成功返回OK，失败返回ERROR
 */
status deQueue(Queue& Q, Q_ElemType& e)
{
    if (Q.num == 0)
        return ERROR;
    e = Q.begin->next->data;
    if (Q.num == 1)
    {
        free(Q.begin->next);
        Q.end = Q.begin;
        Q.num = 0;
        return OK;
    }
    else
    {
        QueueNode* to_delete = Q.begin->next;
        Q.begin->next = Q.begin->next->next;
        free(to_delete);
        Q.num -= 1;
        return OK;
    }
}

/**
 * @description: 查找队列中元素个数
 * @param {Queue} Q
 * @return {int} 元素个数
 */
int Q_num(Queue Q)
{
    return Q.num;
}

/**
 * @description: 获得队列第一个元素的种类
 * @param {Queue} Q
 * @return {Q_ElemType}
 */
Q_ElemType Q_get_first(Queue Q)
{
    if (Q.num != 0)
        return Q.begin->next->data;
}

/**
 * @description: 获得队列最后一个元素的种类
 * @param {Queue} Q
 * @return {Q_ElemType}
 */
Q_ElemType Q_get_last(Queue Q)
{
    return Q.end->data;
}