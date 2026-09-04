#include "print.h"
#define _CRT_SECURE_NO_WARNINGS 1
void f_printTabs(FILE* fp, int tabs) {
    for (int i = 0; i < tabs; i++)
        fprintf(fp, "    ");
}

void f_printFunCall(FILE* fp, p_treeNode fun_call) {
    fprintf(fp, "%s", fun_call->firstChild->self_value);
    fprintf(fp, "(");
    if (fun_call->firstChild->nextBro) {
        for (p_treeNode p = fun_call->firstChild->nextBro->firstChild; p;) {
            fprintf(fp, "%s", p->self_value);
            p = p->nextBro;
            if (p) {
                fprintf(fp, ",");
            }
        } // end of for
    }   // end of if
    fprintf(fp, ")");
} // end of f_printFunCall

void f_printExp(FILE* fp, p_treeNode p) {
    if (!p)
        return;
    if (strcmp(p->self_value, "fun call") == 0) {
        f_printFunCall(fp, p);
    }
    else {
        fprintf(fp, "(");
        f_printExp(fp, p->firstChild);
        fprintf(fp, " %s ", p->self_value);
        if (p->firstChild) {
            f_printExp(fp, p->firstChild->nextBro);
        }
        fprintf(fp, ")");
        return;
    } // end of else
} // end of f_printExp

void f_traverseExp(FILE* fp, p_treeNode p, bool newRoll) {
    f_printExp(fp, p);
    if (newRoll)
        fprintf(fp, ";");
} // end of f_traverseExp

void f_traverse(FILE* fp, p_treeNode p, int tabs, bool newRoll) {
    if (!p)
        return;
    if (strcmp(p->self_value, "fun declaration") == 0 ||
        strcmp(p->self_value, "fun announce") == 0) {
        fprintf(fp, "%s ", p->firstChild->firstChild->self_value);
        fprintf(fp, "%s", p->firstChild->nextBro->self_value);
        if (!p->firstChild->nextBro->nextBro->firstChild) {
            // 无参数
            fprintf(fp, "(void)");
        }
        else {
            // 有参数
            // param list
            fprintf(fp, "(");
            f_traverse(fp, p->firstChild->nextBro->nextBro->firstChild, tabs + 1,
                true);
            fprintf(fp, ")");
        }
        if (strcmp(p->self_value, "fun declaration") == 0) {
            // fun compound statement
            fprintf(fp, "\n{\n");
            tabs++;
            f_traverse(fp, p->firstChild->nextBro->nextBro->nextBro, tabs, true);
            tabs--;
            fprintf(fp, "\n}");
            return;
        }
        else {
            fprintf(fp, ";\n");
            return;
        }
    } // end of if(fun_dec or fun_ann)
    else if (strcmp(p->self_value, "param list") == 0) {
        if (!p->firstChild)
            return;
        //函数声明有参数的param list结点
        fprintf(fp, "%s %s", p->firstChild->firstChild->firstChild->self_value,
            p->firstChild->firstChild->nextBro->self_value);
        if (p->firstChild->nextBro->firstChild) {
            fprintf(fp, ", ");
        }
        f_traverse(fp, p->firstChild->nextBro, tabs, 1);
        return;
    } // end of else if(param_list)
    else if (strcmp(p->self_value, "ex val declaration") == 0 ||
        strcmp(p->self_value, "local val declaration") == 0) {
        //变量声明
        fprintf(fp, "%s ", p->firstChild->firstChild->self_value);
        f_traverse(fp, p->firstChild->nextBro, tabs, 0); // val list结点
        fprintf(fp, ";");
        if (strcmp(p->self_value, "ex val declaration") == 0)
            fprintf(fp, "\n");
        return;
    } // end of else if(val_dec)
    else if (strcmp(p->self_value, "val list") == 0) {
        if (!p->firstChild) {
            return;
        }
        fprintf(fp, "%s", p->firstChild->self_value);
        if (p->firstChild->nextBro)
            if (p->firstChild->nextBro->firstChild) {
                fprintf(fp, ", ");
            }
        f_traverse(fp, p->firstChild->nextBro, tabs, 1);
        return;
    } // end of else if(val_list)
    else if (strcmp(p->self_value, "fun compound statement") == 0) {
        //函数语句结点fun compound statement
        f_traverse(fp, p->firstChild, tabs, 0);
        f_traverse(fp, p->firstChild->nextBro, tabs, 0);
        return;
    }
    else if (strcmp(p->self_value, "statement") == 0) {
        // statement结点
        if (!p->firstChild) {
            return;
        }
        f_printTabs(fp, tabs);
        f_traverse(fp, p->firstChild, tabs, 1);
        fprintf(fp, "\n");
        return;
    }
    else if (strcmp(p->self_value, "compound statement") == 0) {
        // compound statement结点
        if (!p->firstChild) {
            return;
        }
        f_traverse(fp, p->firstChild, tabs, 0);
        f_traverse(fp, p->firstChild->nextBro, tabs, 0);
        return;
    }
    else if (strcmp(p->self_value, "selection statement") == 0) {
        fprintf(fp, "if");
        f_traverse(fp, p->firstChild, tabs, 1);
        f_traverse(fp, p->firstChild->nextBro, tabs, 0);
        return;
    }
    else if (strcmp(p->self_value, "condition expression") == 0) {
        f_traverse(fp, p->firstChild, tabs, 0);
        fprintf(fp, "\n");
        return;
    }
    else if (strcmp(p->self_value, "if else statement") == 0) {
        f_traverse(fp, p->firstChild, tabs, 1);
        f_traverse(fp, p->firstChild->nextBro, tabs, 1);
        return;
    }
    else if (strcmp(p->self_value, "if statement") == 0) {
        f_printTabs(fp, tabs);
        fprintf(fp, "{\n");
        tabs++;
        f_traverse(fp, p->firstChild, tabs, 1);
        tabs--;
        f_printTabs(fp, tabs);
        fprintf(fp, "}\n");
        return;
    }
    else if (strcmp(p->self_value, "else statement") == 0) {
        f_printTabs(fp, tabs);
        fprintf(fp, "else\n");
        f_printTabs(fp, tabs);
        fprintf(fp, "{\n");
        tabs++;
        f_traverse(fp, p->firstChild, tabs, 1);
        tabs--;
        f_printTabs(fp, tabs);
        fprintf(fp, "}\n");
        return;
    }
    else if (strcmp(p->self_value, "while statement") == 0) {
        fprintf(fp, "while");
        f_traverse(fp, p->firstChild, tabs, 1);
        f_printTabs(fp, tabs);
        fprintf(fp, "{\n");
        tabs++;
        f_traverse(fp, p->firstChild->nextBro, tabs, 0);
        tabs--;
        f_printTabs(fp, tabs);
        fprintf(fp, "}\n");
        return;
    }
    else if (strcmp(p->self_value, "for statement") == 0) {
        fprintf(fp, "for");
        fprintf(fp, "( ");
        f_traverse(fp, p->firstChild->firstChild, tabs, 0);
        if (strcmp(p->firstChild->firstChild->self_value, "expression") == 0)
            fprintf(fp, ";");
        fprintf(fp, " ");
        f_traverse(fp, p->firstChild->nextBro, tabs, 0);
        fprintf(fp, "; ");
        f_traverse(fp, p->firstChild->nextBro->nextBro, tabs, 0);
        fprintf(fp, ")\n");
        f_printTabs(fp, tabs);
        fprintf(fp, "{\n");
        tabs++;
        f_traverse(fp, p->firstChild->nextBro->nextBro->nextBro, tabs, 0);
        tabs--;
        f_printTabs(fp, tabs);
        fprintf(fp, "}\n");
        return;
    }
    else if (strcmp(p->self_value, "break statement") == 0) {
        fprintf(fp, "break;");
        return;
    }
    else if (strcmp(p->self_value, "continue statement") == 0) {
        fprintf(fp, "continue;");
        return;
    }
    else if (strcmp(p->self_value, "return statement") == 0) {
        f_printTabs(fp, tabs);
        fprintf(fp, "return ");
        f_traverse(fp, p->firstChild, tabs, 1);
        return;
    }
    else if (strcmp(p->self_value, "expression") == 0) {
        f_traverseExp(fp, p->firstChild, newRoll);
        return;
    }
    else {
        f_traverse(fp, p->firstChild, tabs, newRoll);
        if (p->firstChild)
            f_traverse(fp, p->firstChild->nextBro, tabs, newRoll);
        return;
    }
} // end of f_traverse