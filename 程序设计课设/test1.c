#include <stdlib.h>
#include "test.h"
#define DEF def

//行注释
//line comment
void funcDeclare(int i, float f);
void funcVoid();
double globVar = 1, globarr[9];
/*
 *block comment
 *
 */
int main()
{
int i_1 = 123ul, i_2 = 0x9b7c, i_3 = 0125;/*此处缩进有问题*/
float f_1 = .1l, f_2 = .3e4;/*缩进*/

    double d = .314e3;//改为doublle报错
    char c_1 = 'o';//改为chat报错

    char s[0x19] = "helloworld";
    if (c_1 == 's' || i_1 != i_2)
    {
funcDeclare(i, 0.1);/*缩进*/
        if (c_1 == 9)
        {
            globVar *= 2;
        }
    }
    else if (d == .27 || c_1 > '9')//删去右括号报错、删去左括号报错
    {
        funcDeclare(i, 0.1);
    }
    else
    {
        i_1 += 0;
    }
    while (i_1 >= 0 && f_1 < 1 + 3)//删去一个&报错
    {
        for (i_1 = 0; i_1 <= c_1; i_1 += 1)//删去第一个分号报错
        {
            i_1 += 0;//删去分号报错
            continue;
        }
        break;
    }

    return i_1;
}