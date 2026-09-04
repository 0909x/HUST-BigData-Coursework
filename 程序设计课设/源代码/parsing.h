#ifndef COURSE_DESIGN_PARSER_H
#define COURSE_DESIGN_PARSER_H
#define _CRT_SECURE_NO_WARNINGS 1
#include "lexer.h"
#include "AST.h"
#include "Queue.h"
#include "Stack.h"
#include <iostream>
#include <string>
using namespace std;

bool is_Type_Specifier(int token_type);
bool is_Operator(int token_type);
bool is_CONST_value(int token_type);
p_treeNode Program(Queue& Q, FILE* p, int& line, char token_text[]);
p_treeNode ex_Declaration_List(Queue& Q, p_treeNode ex_declaration_list, FILE* p, int& line, char token_text[]);
p_treeNode ex_Val_Declaration(Queue& Q, p_treeNode val_dec, FILE* p, int& line, char token_text[]);
p_treeNode Fun_Declaration(Queue& Q, p_treeNode fun_dec, FILE* p, int& line, char token_text[]);
p_treeNode Type_Specifier(token_info w, p_treeNode type_specifier);
p_treeNode Val_List(Queue& Q, p_treeNode val_list, FILE* p, int& line, char token_text[]);
p_treeNode Params(Queue& Q, p_treeNode params, FILE* p, int& line, char token_text[]);
p_treeNode Param_List(Queue& Q, p_treeNode param_list, FILE* p, int& line, char token_text[]);
p_treeNode Param(Queue& Q, p_treeNode param, FILE* p, int& line, char token_text[]);
p_treeNode Statement(Queue& Q, p_treeNode statement, FILE* p, int& line, char token_text[]);
p_treeNode Compound_Statement(Queue& Q, p_treeNode compound_statement, FILE* p, int& line, char token_text[]);
p_treeNode Selection_Statement(Queue& Q, p_treeNode selection_statement, FILE* p, int& line, char token_text[]);
p_treeNode Condition_Expression(Queue& Q, p_treeNode condition_expression, FILE* p, int& line, char token_text[]);
p_treeNode IF_Statement(Queue& Q, p_treeNode if_statement, FILE* p, int& line, char token_text[]);
p_treeNode IF_ELSE_Statement(Queue& Q, p_treeNode if_else_statement, p_treeNode if_statement, FILE* p, int& line, char token_text[]);
p_treeNode ELSE_Statement(Queue& Q, p_treeNode else_statement, FILE* p, int& line, char token_text[]);
p_treeNode While_Statement(Queue& Q, p_treeNode while_statement, FILE* p, int& line, char token_text[]);
p_treeNode For_Statement(Queue& Q, p_treeNode for_statement, FILE* p, int& line, char token_text[]);
p_treeNode Local_Val_Declaration(Queue& Q, p_treeNode local_val_declaration, FILE* p, int& line, char token_text[]);
p_treeNode Fun_Call(Queue& Q, p_treeNode fun_Call, FILE* p, int& line, char token_text[]);
p_treeNode Params_Call(Queue& Q, p_treeNode params_call, FILE* p, int& line, char token_text[]);
p_treeNode Return_Statement(Queue& Q, p_treeNode return_statement, FILE* p, int& line, char token_text[]);
p_treeNode Break_Statement(Queue& Q, p_treeNode break_statement, FILE* p, int& line, char token_text[]);
p_treeNode Continue_Statement(Queue& Q, p_treeNode continue_statement, FILE* p, int& line, char token_text[]);
p_treeNode Expression(Queue& Q, p_treeNode expression, FILE* p, int& line, char token_text[], int endsym);
p_treeNode Const_Value(token_info w, p_treeNode const_value);

#endif //COURSE_DESIGN_PARSER_H