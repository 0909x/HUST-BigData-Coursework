#include "AST.h"
#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS 1
/**
 * @description: 释放抽象语法树全部空间
 * @param {p_treeNode} root 抽象语法树根结点
 * @return {p_treeNode} 空指针
 * @call: destoryAST
 */
p_treeNode destoryAST(p_treeNode root) {
    if (!root)
        return NULL;
    if (root->firstChild)
        destoryAST(root->firstChild);
    if (root->nextBro)
        destoryAST(root->nextBro);
    free(root);
    return NULL;
}

/**
 * @description: 初始化树结点
 * @param {p_treeNode} newTree 树结点指针
 * @param {int} token -1:该节点为语法单元 其他:该节点token_kind值
 * @param {char} *grammar_type 语法单元类型(结点为token时可传入空字符串)
 * @return {status} OK：初始化成功 ERROR：初始化失败
 * @call: (none)
 * @called_by: insertChild
 */
status iniTreeNode(p_treeNode& newTree, int token, char* grammar_type) {
    if (!newTree) {
        newTree = destoryAST(newTree);
    }
    newTree = (p_treeNode)malloc(sizeof(treeNode));
    if (!newTree)
        return ERROR;
    newTree->firstChild = NULL;
    newTree->nextBro = NULL;

    newTree->node_type = token;
    strcpy(newTree->self_value, grammar_type);

    return OK;
}

/**
 * @description: 插入子结点
 * @param {p_treeNode} parent 父结点指针
 * @param {int} token: -1:该节点为语法单元 其他:该节点token_kind值
 * @param {char} *childGrammarType
 * 子结点语法单元类型(结点为token时可传入空字符串)
 * @return {status} OK：插入成功 ERROR：插入失败
 * @call: iniTreeNode
 */
p_treeNode insertChild(p_treeNode& parent, int token, char* childGrammarType) {
    p_treeNode newChild = (p_treeNode)malloc(sizeof(treeNode));
    // int childTabs = parent->tabs;
    if (!iniTreeNode(newChild, token, childGrammarType))
        return NULL;

    if (!parent->firstChild) { //当前结点无子结点
        parent->firstChild = newChild;
    }
    else { //当前结点有子结点
        p_treeNode pointer = parent->firstChild;
        for (; pointer->nextBro; pointer = pointer->nextBro)
            ; //前往最后一个子结点
        pointer->nextBro = newChild;
    }
    return newChild;
}