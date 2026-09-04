#ifndef COURSE_DESIGN_LEXER_H
#define COURSE_DESIGN_LEXER_H
#define _CRT_SECURE_NO_WARNINGS 1
#include <cstdio>
#include <cstring>

/*
 * 定义单词种类
 * 关键字:  AUTO-UNSIGNED
 * 类型说明符:  CHAR-UNSIGNED
 * 运算符：ASSIGN-MOD
 * 单目算术运算符:  PLUS-MOD
 * 关系运算符:  EQUAL-MORE_OR_EQUAL
 * 逻辑运算符（双目）:  AND、OR
 * 赋值运算符:  ASSIGN-MODE_EQ
 * 常量:  INT_CONST-STRING_CONST
 * 运算符顺序中越靠后优先级越高
 */
enum token_kind
{
    //标识符
    IDENT,

    //关键字
    AUTO,
    BREAK,
    CASE,
    CONTINUE,
    DEFAULT,
    DO,
    RETURN,
    SIZEOF,
    SWITCH,
    TYPEDEF,
    VOID,
    VOLATILE,
    WHILE,
    ELSE,
    EXTERN,
    FOR,
    GOTO,
    IF,
    CONST,
    STATIC,
    REGISTER,
    CHAR,
    DOUBLE,
    ENUM,
    FLOAT,
    INT,
    LONG,
    SIGNED,
    SHORT,
    STRUCT,
    UNION,
    UNSIGNED,

    //定界符
    //起止符号
    BEGIN_END, //#

    COMMA,           // ,
    SEMICOLON,       // ;
    LPARENTHESE,     // (
    RPARENTHESE,     // )
    LSUBSCRIPT,      // [
    RSUBSCRIPT,      // ]
    LCURLYBRACE,     // {
    RCURLYBRACE,     // }
    SINGLE_QUO_MARK, // '
    DOUBLE_QUO_MARK, // "

    //运算符

    //赋值运算符
    ASSIGN,      // =
    PLUS_EQ,     // +=
    MINUS_EQ,    // -=
    MULTIPLY_EQ, // *=
    DIVIDE_EQ,   // /=
    MOD_EQ,      // %=
    //逻辑运算符（双目）
    AND, // &&
    OR,  // ||
    //关系运算符
    EQUAL,         // ==
    UNEQUAL,       // !=
    LESS,          // <
    MORE,          // >
    LESS_OR_EQUAL, // <=
    MORE_OR_EQUAL, // >=
    //单目算术运算符
    PLUS,     // +
    MINUS,    // -
    MULTIPLY, // *
    DIVIDE,   // /
    MOD,      // %



    //常量
    INT_CONST,
    UNSIGNED_CONST,
    LONG_CONST,
    UNSIGNED_LONG_CONST,
    FLOAT_CONST,
    DOUBLE_CONST,
    LONG_DOUBLE_CONST,
    CHAR_CONST,
    STRING_CONST,

    //预处理标识符
    DEFINE,
    INCLUDE,

    //报错
    ERROR_TOKEN,
}; //end of enum token_kind

//关键字字符串值与枚举常量值对应结构（用于构建哈希表）
typedef struct KeyWord_name_value
{
    char name[10];
    int value;
} KeyWord_name_value;

struct token_info
{
    int token_kind;      //种类码
    int line;            //行数
    char token_text[50]; //自身值
};

//词法分析函数
int isKeyWord(char str[]);
bool isNum(char c);
bool isLetter(char c);
token_info get_token(FILE* p, int& line_num, char token_text[]);

#endif //COURSE_DESIGN_LEXER_H