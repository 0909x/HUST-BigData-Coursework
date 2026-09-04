#include "Traverse.h"
#define _CRT_SECURE_NO_WARNINGS 1

void printTabs(int tabs) {
    for (int i = 0; i < tabs; i++)
        printf("  ");
}

void printFunCall(p_treeNode fun_call) {
    printf("%s", fun_call->firstChild->self_value);
    printf("(");
    if (fun_call->firstChild->nextBro) {
        for (p_treeNode p = fun_call->firstChild->nextBro->firstChild; p;) {
            printf("%s", p->self_value);
            p = p->nextBro;
            if (p) {
                printf(",");
            }
        } // end of for
    }   // end of if
    printf(")");
} // end of printFunCall

void traverseExp(p_treeNode p) {
    if (!p)
        return;
    else {
        if (strcmp(p->self_value, "fun call") == 0) {
            printFunCall(p);
        }
        else {
            printf("(");
            traverseExp(p->firstChild);
            printf(" %s ", p->self_value);
            if (p->firstChild)
                traverseExp(p->firstChild->nextBro);
            printf(")");
        } // end of else
        return;
    } // end of else
} // end of traverseExp

void traverse(p_treeNode p, int tabs) {
    if (!p)
        return;
    if (strcmp(p->self_value, "fun declaration") == 0 ||
        strcmp(p->self_value, "fun announce") == 0) {
        //函数定义与声明
        printTabs(tabs);
        tabs++;
        printf("%s\n", p->self_value);
        printTabs(tabs);
        printf("return type: %s\n", p->firstChild->firstChild->self_value);
        printTabs(tabs);
        printf("fun name: %s\n", p->firstChild->nextBro->self_value);
        printTabs(tabs);
        printf("params: \n");
        //        printTabs(++tabs);
        tabs++;
        if (!p->firstChild->nextBro->nextBro) {
            //无参数
            printTabs(++tabs);
            printf("void\n");
        }
        else {
            //有参数
            // param list
            traverse(p->firstChild->nextBro->nextBro->firstChild, tabs);
        }
        tabs--;
        if (strcmp(p->self_value, "fun announce") == 0)
            return;
        // fun compound statement
        printTabs(tabs);
        printf("fun compound statement:\n");
        tabs++;
        traverse(p->firstChild->nextBro->nextBro->nextBro, tabs);
        return;
    } // end of if(fun_dec or fun_ann)
    else if (strcmp(p->self_value, "param list") == 0) {
        //函数声明有参数的param list结点
        if (!p->firstChild)
            return;
        printTabs(tabs);
        printf("type specifier: %s ; param name:%s \n",
            p->firstChild->firstChild->firstChild->self_value,
            p->firstChild->firstChild->nextBro->self_value);
        if (p->firstChild)
            traverse(p->firstChild->nextBro, tabs);
        return;
    } // end of else if(param_list)
    else if (strcmp(p->self_value, "ex val declaration") == 0 ||
        strcmp(p->self_value, "local val declaration") == 0) {
        //变量声明
        printTabs(tabs);
        printf("%s :\n", p->self_value);
        tabs++;
        printTabs(tabs);
        printf("type specifier: %s\n", p->firstChild->firstChild->self_value);
        traverse(p->firstChild->nextBro, tabs); // val list结点
        tabs--;
        return;
    } // end of else if(val_dec)
    else if (strcmp(p->self_value, "val list") == 0) {
        printTabs(tabs);
        printf("valid name: %s\n", p->firstChild->self_value);
        traverse(p->firstChild->nextBro, tabs);
        return;
    } // end of else if(val_list)
    else if (strcmp(p->self_value, "fun compound statement") == 0) {
        //函数语句结点fun compound statement
        traverse(p->firstChild, tabs);
        if (p->firstChild)
            traverse(p->firstChild->nextBro, tabs);
        return;
    } // end of else if(fun_cmpd_statmt)
    else if (strcmp(p->self_value, "statement") == 0) {
        // statement结点
        traverse(p->firstChild, tabs);
        return;
    }
    else if (strcmp(p->self_value, "compound statement") == 0) {
        // compound statement结点
        traverse(p->firstChild, tabs);
        if (p->firstChild)
            traverse(p->firstChild->nextBro, tabs);
        return;
    }
    else if (strcmp(p->self_value, "selection statement") == 0) {
        printTabs(tabs);
        printf("selection statement: \n");
        tabs++;
        traverse(p->firstChild, tabs);
        traverse(p->firstChild->nextBro, tabs);
        tabs--;
        return;
    }
    else if (strcmp(p->self_value, "condition expression") == 0) {
        printTabs(tabs);
        printf("condition expression: \n");
        traverse(p->firstChild, tabs);
        return;
    }
    else if (strcmp(p->self_value, "if else statement") == 0) {
        // tabs++;
        traverse(p->firstChild, tabs);
        traverse(p->firstChild->nextBro, tabs);
        // tabs--;
        return;
    }
    else if (strcmp(p->self_value, "if statement") == 0) {
        printTabs(tabs);
        printf("if statement:\n");
        tabs++;
        traverse(p->firstChild, tabs);
        tabs--;
        return;
    }
    else if (strcmp(p->self_value, "else statement") == 0) {
        printTabs(tabs);
        printf("else statement:\n");
        tabs++;
        traverse(p->firstChild, tabs);
        tabs--;
        return;
    }
    else if (strcmp(p->self_value, "while statement") == 0) {
        printTabs(tabs);
        printf("while statement:\n");
        tabs++;
        printTabs(tabs);
        printf("condition expression:\n");
        traverse(p->firstChild, tabs);
        printTabs(tabs);
        printf("iteration statement:\n");
        traverse(p->firstChild->nextBro, tabs);
        tabs--;
        return;
    } // end of else if(while_statmt)
    else if (strcmp(p->self_value, "for statement") == 0) {
        printTabs(tabs);
        printf("for statement:\n");
        printf("condition val initial:\n");
        traverse(p->firstChild->firstChild, tabs + 1);
        printTabs(tabs);
        printf("condition expression:\n");
        traverse(p->firstChild->nextBro, tabs + 1);
        printTabs(tabs);
        printf("condition iteration:\n");
        traverse(p->firstChild->nextBro->nextBro, tabs + 1);
        printTabs(tabs);
        printf("iteration statement:\n");
        traverse(p->firstChild->nextBro->nextBro->nextBro, tabs + 1);
        return;
    } // end of else if(for_statmt)
    else if (strcmp(p->self_value, "break statement") == 0) {
        printTabs(tabs);
        printf("break statement\n");
        return;
    }
    else if (strcmp(p->self_value, "continue statement") == 0) {
        printTabs(tabs);
        printf("continue statement\n");
        return;
    }
    else if (strcmp(p->self_value, "return statement") == 0) {
        printTabs(tabs);
        printf("return statement\n");
        printTabs(tabs + 1);
        printf("return expression:\n");
        traverse(p->firstChild, tabs + 1);
        return;
    }
    else if (strcmp(p->self_value, "expression") == 0) {
        printTabs(tabs);
        printf("expression:\n");
        printTabs(tabs + 1);
        traverseExp(p->firstChild);
        printf("\n");
        return;
    }
    else {
        traverse(p->firstChild, tabs);
        if (p->firstChild)
            traverse(p->firstChild->nextBro, tabs);
        return;
    }
} // end of traverse