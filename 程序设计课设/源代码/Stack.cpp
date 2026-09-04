#include "Stack.h"
#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS 1
//栈函数

/**
 * @description: 初始化栈
 * @param {Stack} &S
 * @return
 */
void iniStack(Stack& S) {
    S.begin = (StackNode*)malloc(sizeof(StackNode));
    S.begin->next = NULL;
    S.num = 0;
    return;
}

/**
 * @description: 将元素e入栈
 * @param {Stack} &S
 * @param {S_ElemType} e
 * @return 成功返回OK，失败返回ERROR
 */
status S_push(Stack& S, S_ElemType e) {
    StackNode* new_node = (StackNode*)malloc(sizeof(StackNode));
    if (!new_node)
        return ERROR;
    new_node->next = S.begin;
    new_node->data = e;
    S.begin = new_node;
    S.num += 1;
    return OK;
}

/**
 * @description: 将栈顶元素出栈并赋值给e
 * @param {Stack} &S
 * @param {S_ElemType} &e
 * @return 成功返回OK，失败返回ERROR
 */
status S_pop(Stack& S, S_ElemType& e) {
    if (S.num == 0)
        return ERROR;
    e = S.begin->data;
    StackNode* new_begin = S.begin->next;
    free(S.begin);
    S.begin = new_begin;
    S.num -= 1;
    return OK;
}

/**
 * @description: 查询栈中元素个数
 * @param {Stack} S
 * @return {int} 元素个数
 */
int S_num(Stack S) { return S.num; }

/**
 * @description: 判断栈是否为空
 * @param {Stack} S
 * @return 空则返回1，非空返回0
 */
int isEmptyStack(Stack S) {
    if (S.num == 0)
        return 1;
    else
        return 0;
}

/**
 * @description:获取栈顶元素
 * @param {Stack} S
 * @return {*}
 */
S_ElemType S_get_top(Stack S) {
    if (S.num != 0)
        return S.begin->data;
}

/**
 * @description: 清空栈
 * @param {Stack} S
 * @return
 */
void destroyStack(Stack S) {
    while (S_num(S)) {
        S_ElemType e;
        S_pop(S, e);
    }
    return;
}