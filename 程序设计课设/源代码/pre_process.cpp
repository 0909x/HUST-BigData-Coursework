#include "pre_process.h"
#include "pre_process.h"
#include <iostream>
#include <string>
using namespace std;

/**
 * @description: 删除注释、执行包含文件与宏定义，然后将剩余部分保存入新文件
 * @param {char} originFileName源文件名
 * @param {char} processedFileName新文件名
 * @return {int} 0：文件打开失败；1：处理成功
 * @call: (none)
 * @called_by: main(in file "main.cpp")
 */
int pre_process(char originFileName[], char processedFileName[]) {
    FILE* origin = fopen(originFileName, "r");
    FILE* processed = fopen(processedFileName, "w");
    if (!origin || !processed)
        return 0; //文件打开失败
    else
        printf("打开文件成功\n");
    printf("\n**********进行编译预处理**********\n");
    char ch;
    string command, file_name, word_defined, define_as;
    while (!feof(origin)) {
        if ((ch = fgetc(origin)) == '#') {
            while ((ch = fgetc(origin)) == ' ') { //清除空白符
                ;
            }
            ungetc(ch, origin);
            while ((ch = fgetc(origin)) != ' ' && ch != '<' && ch != '\"' &&
                ch != '\n' && ch != '\t') { //获得预处理命令
                if (ch == '\n')
                    putc('\n', processed);
                command = command + ch;
            }
            ungetc(ch, origin);
            if (command == "include") { //处理文件包含
                while ((ch = fgetc(origin)) == ' ' || ch == '<' ||
                    ch == '\"') { //清除空白符
                    ;
                }
                ungetc(ch, origin);
                while ((ch = fgetc(origin)) != '>' && ch != '\"') {
                    file_name = file_name + ch;
                }
                cout << "#包含文件：" << file_name << "\n";
            }
            else if (command == "define") {                     //处理宏定义
                while ((ch = fgetc(origin)) == ' ' || ch == '\t') { //清除空白符
                    ;
                }
                ungetc(ch, origin);
                while ((ch = fgetc(origin)) != ' ' && ch != '\t' && ch != '\n') {
                    word_defined = word_defined + ch;
                }
                ungetc(ch, origin);
                while ((ch = fgetc(origin)) == ' ' || ch == '\t') {
                    ;
                }
                if (ch == '\n') {
                    cout << "#定义：" << word_defined << "\n";
                }
                else {
                    do {
                        define_as = define_as + ch;
                    } while ((ch = fgetc(origin)) != '\n');
                    cout << "#定义：" << word_defined << " 为：" << define_as << "\n";
                    fgetc(origin);
                }
                ungetc(ch, origin);
            } // end of else if
            else {
                printf("未知的预处理指令\n");
                while ((ch = fgetc(origin)) != '\n') {
                    ;
                }
            }
            command.clear();
            file_name.clear();
            word_defined.clear();
            define_as.clear(); //重置字符串
            continue;
        } // end of if ((ch = fgetc(origin)) == '#')
        else if (ch == '/') {
            switch ((ch = fgetc(origin))) {
            case '/':
                //行注释
                do {
                    ch = fgetc(origin);
                } while (ch != '\n');
                putc('\n', processed);
                break;
            case '*':
                //块注释
                ch = getc(origin);
                do {
                    if (ch != '*') {
                        if (ch == '\n')
                            putc('\n', processed);
                        ch = fgetc(origin);
                    }
                } while (!(ch == '*' && (ch = fgetc(origin)) == '/'));
                break;
            default:
                //非注释
                fputc('/', processed);
                fputc(ch, processed);
                break;
            } // end of switch
            if ((ch = fgetc(origin)) == EOF)
                return 1;
            ungetc(ch, origin);
        } // end of else if ((ch = fgetc(origin)) == '/')
        else if (ch != EOF) {
            fputc(ch, processed);
        }
    } // end of while
    fclose(origin);
    fclose(processed);
    return 1;
} // end of pre_process