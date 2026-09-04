#define _CRT_SECURE_NO_WARNINGS 1
#pragma once
#include "Traverse.h"
#include "lexer.h"
#include "parsing.h"
#include "pre_process.h"
#include "print.h"
//全局变量
int choose = -1;
int line = 1;
char token_text[50];
Queue Q;

int main() {
    char source_file_name[100], mid_file_name[110];
    printf("请输入源程序文件名称：\n");
    scanf("%s", source_file_name);
    sprintf(mid_file_name, "mid_%s.c", source_file_name);
    sprintf(source_file_name, "%s.c", source_file_name);
    if (!pre_process(source_file_name, mid_file_name))
        printf("文件打开失败！\n");
    FILE* mid_file = fopen(mid_file_name, "r");
    printf("请选择要实现的功能：\n");
    printf("1.进行词法分析和语法分析\t2.格式化输出语法分析树\n3.将代码格式化输出至文件%s\t0.退出系统\n", mid_file_name);
    iniQueue(Q);
    scanf("%d", &choose);
    if (choose == 0)return 0;
    else if (choose == 1)
    {
        p_treeNode program = Program(Q, mid_file, line, token_text);
    }
    else if (choose == 2)
    {
        p_treeNode program = Program(Q, mid_file, line, token_text);
        system("cls");
        traverse(program, 0);
    }
    else if (choose == 3)
    {
        p_treeNode program = Program(Q, mid_file, line, token_text);
        system("cls");
        traverse(program, 0);
        system("cls");
        fclose(mid_file);
        mid_file = fopen(mid_file_name, "w");
        if (!mid_file) {
            printf("文件打开失败\n");

        }
        f_traverse(mid_file, program, 0, true);
    }
    return 0;
}