#ifndef COURSE_DESIGN_AST_H
#define COURSE_DESIGN_AST_H
#define _CRT_SECURE_NO_WARNINGS 1
#include "lexer.h"
#include <cstdio>
#include <cstring>
//#include <mm_malloc.h>

#define OK 1
#define ERROR 0
typedef int status;

typedef struct treeNode {
	int node_type; //-1:该节点为语法单元 其他:该节点token_kind值
	char self_value[50];  //语句类型或token自身值
	treeNode* nextBro;    //下一个兄弟结点
	treeNode* firstChild; //第一个子结点
} *p_treeNode;

//抽象语法树函数
p_treeNode destoryAST(p_treeNode root);
status iniTreeNode(p_treeNode& newTree, int token, char* grammar_type);
p_treeNode insertChild(p_treeNode& parent, int token, char* childGrammarType);

#endif // COURSE_DESIGN_AST_H