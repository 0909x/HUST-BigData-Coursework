#include "parsing.h"

int error = 0;

/**
 * @description: 判断token是否为类型说明符
 * @param {int} token_type 标识符种类
 * @return {bool} true/false
 * @call_by ex_Declaration_List; Param_List; Statement; Local_Val_Declaration
 */
bool is_Type_Specifier(int token_type) {
    if (token_type >= CHAR && token_type <= UNSIGNED)
        return true;
    else
        return false;
}

/**
 * @description: 判断token是否为运算符
 * @param {int} token_type 标识符种类
 * @return {bool} true/false
 * @called_by Expression
 */
bool is_Operator(int token_type) {
    if (token_type >= ASSIGN && token_type <= MOD)
        return true;
    else
        return false;
}

/**
 * @description: 判断token是否为常量值
 * @param {int} token_type 标识符种类
 * @return {bool} true/false
 * @called_by: Statement; Params_Call
 */
bool is_CONST_value(int token_type) {
    if (token_type >= INT_CONST && token_type <= STRING_CONST)
        return true;
    else
        return false;
}

/**
 * @description: 创建程序结点
 * @param {Queue} &Q 读取的token队列
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call ex_Declaration_List
 */
p_treeNode Program(Queue& Q, FILE* p, int& line, char token_text[]) {
    p_treeNode program = NULL;
    char grammar_type[] = "program";
    if (!iniTreeNode(program, -1, grammar_type)) { //初始化根结点失败
        printf("%s\n", "unknown error in creating program!\n");
        return NULL;
    }

    char son_grammar_type[] = "ex declaration list";
    p_treeNode ex_declaration_list = insertChild(program, -1, son_grammar_type);
    ex_declaration_list =
        ex_Declaration_List(Q, ex_declaration_list, p, line, token_text);

    if (!ex_declaration_list) { //外部定义序列为空
        printf("fail in creating ex declaration list!(in line %d)\n", line);
        error = 1;
        destoryAST(ex_declaration_list);
        return program;
    }
    return program;
} // end of Program

/**
 * @description: 创建外部定义序列
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} ex_declaration_list根结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: is_Type_Specifier; Fun_Declaration; ex_Val_Declaration
 * @called_by: program; ex_Declaration_List
 */
p_treeNode ex_Declaration_List(Queue& Q, p_treeNode ex_declaration_list,
    FILE* p, int& line, char token_text[]) {
    token_info w = get_token(p, line, token_text);

    if (w.token_kind == EOF || feof(p))
        return NULL;
    if ((!is_Type_Specifier(w.token_kind)) && w.token_kind != VOID) {
        printf(
            "fail in creating ex declaration: expect type specifier!(in line %d)\n",
            line);
        error = 1;
        destoryAST(ex_declaration_list);
        return NULL;
    }
    enQueue(Q, w);
    w = get_token(p, line, token_text);
    if (w.token_kind != IDENT) {
        printf("fail in creating ex declaration: expect IDENT!(in line %d)\n",
            line);
        error = 1;
        destoryAST(ex_declaration_list);
        return NULL;
    }
    enQueue(Q, w);

    w = get_token(p, line, token_text);
    switch (w.token_kind) {
    case LPARENTHESE: {
        char grammar_type[] = "fun declaration";
        p_treeNode fun_dec = insertChild(ex_declaration_list, -1, grammar_type);
        fun_dec = Fun_Declaration(Q, fun_dec, p, line, token_text);
        break;
    }
    case COMMA:
    case SEMICOLON:
    case LSUBSCRIPT:
    case ASSIGN: {
        enQueue(Q, w);
        if (error)
            return ex_declaration_list;
        char grammar_type1[] = "ex val declaration";
        p_treeNode ex_val_dec = insertChild(ex_declaration_list, -1, grammar_type1);
        ex_val_dec = ex_Val_Declaration(Q, ex_val_dec, p, line, token_text);
        break;
    }
    default: {
        printf("fail in creating ex declaration: expect ; or param(in line %d)\n",
            line);
        error = 1;
        destoryAST(ex_declaration_list);
        return NULL;
        break;
    }
    } // end of switch
    if (error)
        return ex_declaration_list;
    char grammar_type2[] = "ex declaration list";
    p_treeNode son_ex_declaration_list =
        insertChild(ex_declaration_list, -1, grammar_type2);
    son_ex_declaration_list =
        ex_Declaration_List(Q, son_ex_declaration_list, p, line, token_text);
    return ex_declaration_list;

} // end of ex_Declaration_List

/**
 * @description: 生成外部变量声明
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} dec_list根结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Type_Specifier; Val_List
 * @called_by: ex_Declaration_List
 */
p_treeNode ex_Val_Declaration(Queue& Q, p_treeNode val_dec, FILE* p, int& line,
    char token_text[]) {
    token_info w;

    //将类型识别符放入结点
    deQueue(Q, w);
    char son0_gt[] = "type specifier"; //第一个子结点的语法单元类型
    p_treeNode type_spe = insertChild(val_dec, -1, son0_gt);
    type_spe = Type_Specifier(w, type_spe);
    if (error)
        return val_dec;

    //创建标识符序列结点
    char son1_gt[] = "val list"; //第二个子结点的语法单元类型
    p_treeNode ex_val_list = insertChild(val_dec, -1, son1_gt);
    ex_val_list = Val_List(Q, ex_val_list, p, line, token_text);
    return val_dec;
} // end of ex_Val_Declaration

/**
 * @description: 生成函数定义或函数声明结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} fun_dec根结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Type_Specifier; Params; Compound_Statement
 * @called_by: ex_Declaration_List
 */
p_treeNode Fun_Declaration(Queue& Q, p_treeNode fun_dec, FILE* p, int& line,
    char token_text[]) {
    token_info w;

    //生成函数返回类型结点
    deQueue(Q, w);
    char son0_gt[] = "return type specifier";
    p_treeNode type_specifier = insertChild(fun_dec, -1, son0_gt);
    type_specifier = Type_Specifier(w, type_specifier);
    if (error)
        return fun_dec;

    //生成函数名结点
    deQueue(Q, w);
    char son1_gt[] = "fun name";
    p_treeNode fun_name = insertChild(fun_dec, IDENT, w.token_text);
    if (error)
        return fun_dec;
    //生成参数序列

    char son2_gt[] = "params";
    p_treeNode params = insertChild(fun_dec, -1, son2_gt);
    params = Params(Q, params, p, line, token_text);
    if (error)
        return fun_dec;

    //判断是否为函数声明，若不是则生成函数体
    w = get_token(p, line, token_text);
    if (w.token_kind == SEMICOLON) {
        fun_dec->self_value[0] = '\0';
        strcpy(fun_dec->self_value, "fun announce");
        return fun_dec;
    }
    else if (w.token_kind == LCURLYBRACE) { //生成函数体复合语句结点
        // enQueue(Q, w);
        enQueue(Q, w);
        char son3_gt[] = "fun compound statement";
        p_treeNode fun_body = insertChild(fun_dec, -1, son3_gt);
        fun_body = Compound_Statement(Q, fun_body, p, line, token_text);
        return fun_dec;
    }
    else {
        printf("fail in creating fun declaration: expect \';\' (in line %d)\n",
            line);
        error = 1;
        destoryAST(fun_dec);
        return NULL;
    }
} // end of Fun_Declaration

/**
 * @description: 生成种类识别符结点
 * @param {token_info} w token信息结构
 * @param {p_treeNode} type_specifier 种类识别符结点指针
 * @return {p_treeNode} 种类识别符结点指针
 * @called_by ex_Val_Declaration; Fun_Declaration; Param_List; Param; Statement;
 */
p_treeNode Type_Specifier(token_info w, p_treeNode type_specifier) {
    p_treeNode type = insertChild(type_specifier, w.token_kind, w.token_text);
    return type_specifier;
} // end of Type_Specifier

/**
 * @description: 生成变量序列结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} val_list根结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Val_List;
 * @called_by: ex_val_declaration; Val_List
 */
p_treeNode Val_List(Queue& Q, p_treeNode val_list, FILE* p, int& line,
    char token_text[]) {
    //进入该函数时栈中应有两个元素
    //栈中第一个元素应为变量标识符
    token_info w;
    deQueue(Q, w);
    if (w.token_kind != IDENT) {
        printf("fail in creating valid list: expect ident(in line %d)\n", line);
        error = 1;
        destoryAST(val_list);
        return NULL;
    } // end of if

    if (Q_get_last(Q).token_kind == LSUBSCRIPT) {
        //数组
        string list_subscript;
        list_subscript = list_subscript + w.token_text;
        deQueue(Q, w);
        while (w.token_kind != RSUBSCRIPT) {
            list_subscript = list_subscript + w.token_text;
            w = get_token(p, line, token_text);
        }
        list_subscript = list_subscript + w.token_text;
        w = get_token(p, line, token_text);

        if (w.token_kind == COMMA) {
            p_treeNode val_name =
                insertChild(val_list, IDENT, (char*)list_subscript.data());
            w = get_token(p, line, token_text);
            enQueue(Q, w);
            w = get_token(p, line, token_text);
            enQueue(Q, w);
            char son1_gt[] = "val list";
            p_treeNode son_val_list = insertChild(val_list, -1, son1_gt);
            son_val_list = Val_List(Q, son_val_list, p, line, token_text);
            if (error)
                return val_list;
        } // end of if
        else if (w.token_kind == ASSIGN) {
            list_subscript = list_subscript + w.token_text;
            w = get_token(p, line, token_text);
            list_subscript = list_subscript + w.token_text;
            p_treeNode val_name =
                insertChild(val_list, IDENT, (char*)list_subscript.data());
        }
        else if (w.token_kind == SEMICOLON) {
            p_treeNode val_name =
                insertChild(val_list, IDENT, (char*)list_subscript.data());
            return val_list;
        }
    } // end of if
    else if (Q_get_last(Q).token_kind == ASSIGN) {
        //定义时赋值
        string val_assign;
        val_assign = val_assign + w.token_text;
        deQueue(Q, w);
        val_assign = val_assign + w.token_text;
        w = get_token(p, line, token_text);
        val_assign = val_assign + w.token_text;
        p_treeNode val_name =
            insertChild(val_list, IDENT, (char*)val_assign.data());
        w = get_token(p, line, token_text);

        if (w.token_kind == COMMA) {
            w = get_token(p, line, token_text);
            enQueue(Q, w);
            w = get_token(p, line, token_text);
            enQueue(Q, w);
            char son1_gt[] = "val list";
            p_treeNode son_val_list = insertChild(val_list, -1, son1_gt);
            son_val_list = Val_List(Q, son_val_list, p, line, token_text);
            if (error)
                return val_list;
        } // end of if
        else if (w.token_kind == SEMICOLON) {
            return val_list;
        }
    } // end of else if
    else if (Q_get_last(Q).token_kind == COMMA) {
        //变量序列
        p_treeNode val_name = insertChild(val_list, IDENT, w.token_text);
        deQueue(Q, w);
        w = get_token(p, line, token_text);
        enQueue(Q, w);
        w = get_token(p, line, token_text);
        enQueue(Q, w);
        char son1_gt[] = "val list";
        p_treeNode son_val_list = insertChild(val_list, -1, son1_gt);
        son_val_list = Val_List(Q, son_val_list, p, line, token_text);
        if (error)
            return val_list;
    }                                                 // end of else if
    else if (Q_get_last(Q).token_kind == SEMICOLON) { //变量序列结束
        p_treeNode val_name = insertChild(val_list, IDENT, w.token_text);
        return val_list;
    }
    else {
        printf("fail in creating valid list: expect \';\'(in line %d)\n", line);
        error = 1;
        destoryAST(val_list);
        return NULL;
    }
    return val_list;
} // end of Val_List

/**
 * @description: 生成参数集结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} param_list结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Param_List
 * @called_by: Fun_Declaration; Fun_call
 */
p_treeNode Params(Queue& Q, p_treeNode params, FILE* p, int& line,
    char token_text[]) {
    token_info w = get_token(p, line, token_text);
    if (w.token_kind == RPARENTHESE || w.token_kind == VOID) { //函数无参数
        return params;
    }
    else {
        enQueue(Q, w);
        char son_gt[] = "param list";
        p_treeNode param_list = insertChild(params, -1, son_gt);
        param_list = Param_List(Q, param_list, p, line, token_text);
        return params;
    }
} // end of Params

/**
 * @description: 创建参数列表结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} param_list结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: is_Type_Specifier; Param; Param_List
 * @called_by: Params
 */
p_treeNode Param_List(Queue& Q, p_treeNode param_list, FILE* p, int& line,
    char token_text[]) {
    token_info w;
    deQueue(Q, w);
    if (w.token_kind == RPARENTHESE) { //参数序列结束
        return NULL;
    }
    if (!is_Type_Specifier(w.token_kind)) {
        printf("fail in creating param list: expect type specifier (in line %d)\n",
            line);
        error = 1;
        destoryAST(param_list);
        return NULL;
    }
    enQueue(Q, w);
    w = get_token(p, line, token_text);
    if (w.token_kind != IDENT) {
        printf("fail in creating param list: expect ident (in line %d)\n", line);
        error = 1;
        destoryAST(param_list);
        return NULL;
    }
    enQueue(Q, w);

    //创建参数结点
    char son0_gt[] = "param";
    p_treeNode param = insertChild(param_list, -1, son0_gt);
    param = Param(Q, param, p, line, token_text);
    if (error)
        return param_list;

    //创建参数序列结点
    if (Q_num(Q) == 0) {
        w = get_token(p, line, token_text);
        enQueue(Q, w);
    }
    char son1_gt[] = "param list";
    p_treeNode son_param_list = insertChild(param_list, -1, son1_gt);
    son_param_list = Param_List(Q, son_param_list, p, line, token_text);
    if (error)
        return param_list;
    return param_list;
} // end of Param_List

/**
 * @description: 生成参数结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} param结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Type_Specifier
 * @called_by: Param_List
 */
p_treeNode Param(Queue& Q, p_treeNode param, FILE* p, int& line,
    char token_text[]) {
    token_info w;

    //创建类型识别符结点
    deQueue(Q, w);
    char son0_gt[] = "type specifier";
    p_treeNode type_specifier = insertChild(param, -1, son0_gt);
    type_specifier = Type_Specifier(w, type_specifier);

    //创建参数名结点
    deQueue(Q, w);

    token_info nw = get_token(p, line, token_text);
    if (nw.token_kind == COMMA) {
        ;
    }
    else if (nw.token_kind == RPARENTHESE) {
        enQueue(Q, nw);
    }
    else if (nw.token_kind == LSUBSCRIPT) {
        //数组参数
        string list;
        list = list + w.token_text + '[';
        nw = get_token(p, line, token_text);
        while (nw.token_kind != RSUBSCRIPT) {
            list = list + nw.token_text;
            nw = get_token(p, line, token_text);
        }
        list = list + nw.token_text;
        nw = get_token(p, line, token_text);
        if (nw.token_kind == COMMA) {
            ;
        }
        else if (nw.token_kind == RPARENTHESE) {
            enQueue(Q, nw);
        }
        p_treeNode param_name = insertChild(param, IDENT, (char*)list.data());
        return param;
    } // end of else if(数组参数)

    p_treeNode param_name = insertChild(param, IDENT, w.token_text);
    return param;
} // end of Param

/**
 * @description: 生成语句结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} statement结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: is_Type_Specifier; is_CONST_value; Local_Val_Declaration;
 *        Expression; Selection_Statement; While_Statement; For_Statement;
 *        Compound_Statement; Return_Statement; Break_Statement;
 *        Continue_Statement
 * @called_by: Compound_Statement
 */
p_treeNode Statement(Queue& Q, p_treeNode statement, FILE* p, int& line,
    char token_text[]) {
    //调用此函数时，Q中有语句的第一个单词
    //此函数结束时，Q中有下一语句的第一个单词
    token_info w;
    if (is_Type_Specifier(Q_get_first(Q).token_kind)) {
        char son0_gt[] = "local val declaration";
        p_treeNode local_val_declaration = insertChild(statement, -1, son0_gt);
        local_val_declaration =
            Local_Val_Declaration(Q, local_val_declaration, p, line, token_text);
        w = get_token(p, line, token_text);
        enQueue(Q, w);
        if (error)
            return statement;
    }
    else if (is_CONST_value(Q_get_first(Q).token_kind) ||
        Q_get_first(Q).token_kind == IDENT) {
        char son0_gt[] = "expression";
        p_treeNode expression = insertChild(statement, -1, son0_gt);
        expression = Expression(Q, expression, p, line, token_text, SEMICOLON);
        w = get_token(p, line, token_text);
        enQueue(Q, w);
        if (error)
            return statement;
    }
    else {
        deQueue(Q, w);
        switch (w.token_kind) {
        case IF: {
            char son0_gt[] = "selection statement";
            p_treeNode selection_statement = insertChild(statement, -1, son0_gt);
            selection_statement =
                Selection_Statement(Q, selection_statement, p, line, token_text);
            if (error)
                return statement;
            break;
        }
        case WHILE: {
            char son0_gt[] = "while statement";
            p_treeNode while_statement = insertChild(statement, -1, son0_gt);
            while_statement =
                While_Statement(Q, while_statement, p, line, token_text);
            if (error)
                return statement;
            break;
        }
        case FOR: {
            char son0_gt[] = "for statement";
            p_treeNode for_statement = insertChild(statement, -1, son0_gt);
            for_statement = For_Statement(Q, for_statement, p, line, token_text);
            if (error)
                return statement;
            break;
        }

        case LCURLYBRACE: {
            enQueue(Q, w);
            char son0_gt[] = "compound statement";
            p_treeNode compound_statement = insertChild(statement, -1, son0_gt);
            compound_statement =
                Compound_Statement(Q, compound_statement, p, line, token_text);
            if (error)
                return statement;
            break;
        }
        case RCURLYBRACE: {
            enQueue(Q, w);
            return NULL;
        }
        case RETURN: {
            char son0_gt[] = "return statement";
            p_treeNode return_statement = insertChild(statement, -1, son0_gt);
            return_statement =
                Return_Statement(Q, return_statement, p, line, token_text);
            w = get_token(p, line, token_text);
            enQueue(Q, w);
            if (error)
                return statement;
            break;
        }
        case BREAK: {
            char son0_gt[] = "break statement";
            p_treeNode break_statement = insertChild(statement, -1, son0_gt);
            break_statement =
                Break_Statement(Q, break_statement, p, line, token_text);
            w = get_token(p, line, token_text);
            w = get_token(p, line, token_text);
            enQueue(Q, w);
            if (error)
                return statement;
            break;
        }
        case CONTINUE: {
            char son0_gt[] = "continue statement";
            p_treeNode continue_statement = insertChild(statement, -1, son0_gt);
            continue_statement =
                Continue_Statement(Q, continue_statement, p, line, token_text);
            w = get_token(p, line, token_text);
            w = get_token(p, line, token_text);
            enQueue(Q, w);
            if (error)
                return statement;
            break;
        }
        case COMMA:
        case SEMICOLON: {
            w = get_token(p, line, token_text);
            enQueue(Q, w);
            return statement;
            break;
        }
        default: {
            printf("fail in creating statement: (in line %d)\n", line);
            error = 1;
            destoryAST(statement);
            return NULL;
            break;
        }
        } // end of switch
    }   // end of else
    return statement;
} // end of Statement

/**
 * @description: 创建复合语句结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} compound_statement结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Statement; Compound_Statement
 * @called_by: Statement; Fun_Declaration
 */
p_treeNode Compound_Statement(Queue& Q, p_treeNode compound_statement, FILE* p,
    int& line, char token_text[]) {
    //调用此函数时，Q中应只有大括号

    token_info w;
    deQueue(Q, w);
    if (w.token_kind == RCURLYBRACE) {
        //复合语句结束
        w = get_token(p, line, token_text);
        enQueue(Q, w);
        return NULL;
        //结束时Q中应有一个元素
    }
    else {
        if (w.token_kind == LCURLYBRACE) {
            w = get_token(p, line, token_text);
        }
        enQueue(Q, w);

        //创建语句结点
        char son0_gt[] = "statement";
        p_treeNode statement = insertChild(compound_statement, -1, son0_gt);
        statement = Statement(Q, statement, p, line, token_text);
        if (error)
            return compound_statement;

        //创建复合语句结点
        char son1_gt[] = "compound statement";
        p_treeNode son_compound_statement =
            insertChild(compound_statement, -1, son1_gt);
        son_compound_statement =
            Compound_Statement(Q, son_compound_statement, p, line, token_text);
        return compound_statement;
    } // end of else
    //结束时Q中应有一个元素
} // end of Compound_Statement

/**
 * @description: 生成选择语句结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} selection_statement结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Condition_Expression; IF_Statement; IF_ELSE_Statement
 * @called_by: Statement
 */
p_treeNode Selection_Statement(Queue& Q, p_treeNode selection_statement,
    FILE* p, int& line, char token_text[]) {
    //调用此函数时Q中无元素
    token_info w = get_token(p, line, token_text);
    if (w.token_kind != LPARENTHESE) {
        printf("fail in creating selection statement: expext \'(\' (in line %d)",
            line);
        error = 1;
        destoryAST(selection_statement);
        return NULL;
    }
    char son0_gt[] = "condition expression";
    p_treeNode condition_expression =
        insertChild(selection_statement, -1, son0_gt);
    condition_expression =
        Condition_Expression(Q, condition_expression, p, line, token_text);
    if (error)
        return selection_statement;

    //创建IF_Statement语句
    //此时Q中无元素
    p_treeNode par_if_statm;
    iniTreeNode(par_if_statm, -1,(char *) "par if statm");
    char son1_gt[] = "if statement";
    p_treeNode if_statement = insertChild(par_if_statm, -1, son1_gt);
    if_statement = IF_Statement(Q, if_statement, p, line, token_text);
    if (error)
        return selection_statement;
    //此时Q中有下一语句的第一个单词

    //判断是否有else语句
    deQueue(Q, w);
    if (w.token_kind == ELSE) { //如果为if_else语句
        char son2_gt[] = "if else statement";
        p_treeNode if_else_statement =
            insertChild(selection_statement, -1, son2_gt);
        if_else_statement = IF_ELSE_Statement(Q, if_else_statement, if_statement, p,
            line, token_text);
        if (error)
            return selection_statement;
    }
    else { //如果为if语句
        enQueue(Q, w);
        selection_statement->firstChild->nextBro = if_statement;
    }
    // destoryAST(par_if_statm);
    //结束时Q中有下一语句的第一个单词

} // end of Selection_Statement

/**
 * @description: 创建条件表达式结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} condition_expression结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Expression
 * @called_by: Selection_Statement; While_Statement; For_Statement
 */
p_treeNode Condition_Expression(Queue& Q, p_treeNode condition_expression,
    FILE* p, int& line, char token_text[]) {
    enQueue(Q, get_token(p, line, token_text));
    char son0_gt[] = "expression";
    p_treeNode expression = insertChild(condition_expression, -1, son0_gt);
    expression = Expression(Q, expression, p, line, token_text, RPARENTHESE);
    if (error)
        return condition_expression;
    return condition_expression;
} // end of Condition_Expression

/**
 * @description: 创建IF_Statement语句结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} if_statement结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Compound_Statement; Statement
 * @called_by: Selection_Statement
 */
p_treeNode IF_Statement(Queue& Q, p_treeNode if_statement, FILE* p, int& line,
    char token_text[]) {
    //调用此函数时Q中无元素
    token_info w = get_token(p, line, token_text);
    enQueue(Q, w);
    if (w.token_kind == LCURLYBRACE) {
        char son0_gt[] = "compound statement";
        p_treeNode compound_statement = insertChild(if_statement, -1, son0_gt);
        compound_statement =
            Compound_Statement(Q, compound_statement, p, line, token_text);
    }
    else {
        char son0_gt[] = "statement";
        p_treeNode statement = insertChild(if_statement, -1, son0_gt);
        statement = Statement(Q, statement, p, line, token_text);
    }
    //结束时Q中有下一语句的第一个单词
    return if_statement;
} // end of IF_Statement

/**
 * @description: 创建IF_ELSE_Statement语句结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} if_else_statement结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: ELSE_Statement
 * @called_by: Selection_Statement
 */
p_treeNode IF_ELSE_Statement(Queue& Q, p_treeNode if_else_statement,
    p_treeNode if_statement, FILE* p, int& line,
    char token_text[]) {
    //调用此函数时Q中无元素，应读取else下一个单词
    enQueue(Q, get_token(p, line, token_text));

    if_else_statement->firstChild = if_statement;
    char son1_gt[] = "else statement";
    p_treeNode else_statement = insertChild(if_else_statement, -1, son1_gt);
    else_statement = ELSE_Statement(Q, else_statement, p, line, token_text);
    return if_else_statement;
    //结束时Q中有下一语句的第一个单词
} // end of IF_ELSE_Statement

/**
 * @description: 创建ELSE_Statement语句结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} else_statement
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Compound_Statement; Statement
 * @called_by: Selection_Statement
 */
p_treeNode ELSE_Statement(Queue& Q, p_treeNode else_statement, FILE* p,
    int& line, char token_text[]) {
    //调用此函数时，Q中有else后的第一个单词
    if (Q_get_first(Q).token_kind == LCURLYBRACE) {
        char son0_gt[] = "compound statement";
        p_treeNode statement = insertChild(else_statement, -1, son0_gt);
        statement = Compound_Statement(Q, statement, p, line, token_text);
    }
    else {
        char son0_gt[] = "statement";
        p_treeNode statement = insertChild(else_statement, -1, son0_gt);
        statement = Statement(Q, statement, p, line, token_text);
    }
    return else_statement;
    //结束时Q中有下一语句的第一个单词
} // end of ELSE_Statement

/**
 * @description: 创建While_Statement语句结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} while_statement结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Condition_Expression; Compound_Statement; Statement
 * @called_by: Statement
 */
p_treeNode While_Statement(Queue& Q, p_treeNode while_statement, FILE* p,
    int& line, char token_text[]) {
    //调用此函数时Q中无元素
    token_info w = get_token(p, line, token_text);
    if (w.token_kind != LPARENTHESE) {
        printf("fail in creating while statement: expect \'(\' (in line %d)\n",
            line);
        error = 1;
        destoryAST(while_statement);
        return NULL;
    }

    //创建条件表达式语句
    char son0_gt[] = "condition expression";
    p_treeNode condition_expression = insertChild(while_statement, -1, son0_gt);
    condition_expression =
        Condition_Expression(Q, condition_expression, p, line, token_text);
    if (error)
        return while_statement;

    //创建循环执行语句
    w = get_token(p, line, token_text);
    enQueue(Q, w);
    if (w.token_kind == LCURLYBRACE) {
        char son1_gt[] = "compound statement";
        p_treeNode iteration_statement = insertChild(while_statement, -1, son1_gt);
        iteration_statement =
            Compound_Statement(Q, iteration_statement, p, line, token_text);
    }
    else {
        char son1_gt[] = "statement";
        p_treeNode iteration_statement = insertChild(while_statement, -1, son1_gt);
        iteration_statement =
            Statement(Q, iteration_statement, p, line, token_text);
    }

    return while_statement;
} // end of While_Statement

/**
 * @description: 创建For_Statement语句结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} for_statement结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Statement; Expression; Compound_Statement
 * @called_by: Statement
 */
p_treeNode For_Statement(Queue& Q, p_treeNode for_statement, FILE* p, int& line,
    char token_text[]) {
    //调用此函数时Q中无元素
    token_info w = get_token(p, line, token_text);
    if (w.token_kind != LPARENTHESE) {
        printf("fail in creating for statement: expect \'(\' (in line %d)\n", line);
        error = 1;
        destoryAST(for_statement);
        return NULL;
    }
    w = get_token(p, line, token_text);

    //创建循环条件变量初始化语句结点
    enQueue(Q, w);
    char son0_gt[] = "condition val initial";
    p_treeNode condition_val_init = insertChild(for_statement, -1, son0_gt);
    condition_val_init = Statement(Q, condition_val_init, p, line, token_text);
    if (error)
        return for_statement;

    //创建循环条件表达式结点
    char son1_gt[] = "expression";
    p_treeNode condition_expression = insertChild(for_statement, -1, son1_gt);
    condition_expression =
        Expression(Q, condition_expression, p, line, token_text, SEMICOLON);
    if (error)
        return for_statement;

    //创建循环变量迭代表达式结点
    w = get_token(p, line, token_text);
    enQueue(Q, w);
    char son2_gt[] = "expression";
    p_treeNode condition_iteration = insertChild(for_statement, -1, son2_gt);
    condition_iteration =
        Expression(Q, condition_iteration, p, line, token_text, RPARENTHESE);
    if (error)
        return for_statement;

    //创建循环执行语句部分
    w = get_token(p, line, token_text);
    enQueue(Q, w);
    if (w.token_kind == LCURLYBRACE) {
        char son3_gt[] = "compound statement";
        p_treeNode iteration_statement = insertChild(for_statement, -1, son3_gt);
        iteration_statement =
            Compound_Statement(Q, iteration_statement, p, line, token_text);
    }
    else {
        char son3_gt[] = "statement";
        p_treeNode iteration_statement = insertChild(for_statement, -1, son3_gt);
        iteration_statement =
            Statement(Q, iteration_statement, p, line, token_text);
    }
    return for_statement;
} // end of For_Statement

/**
 * @description: 创建局部变量定义结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} local_val_declaration结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Type_Specifier; Val_List
 * @called_by: Statement
 */
p_treeNode Local_Val_Declaration(Queue& Q, p_treeNode local_val_declaration,
    FILE* p, int& line, char token_text[]) {
    //创建类型识别符结点
    token_info w;
    deQueue(Q, w);
    char son_0gt[] = "type specifier";
    p_treeNode type_specifier = insertChild(local_val_declaration, -1, son_0gt);
    type_specifier = Type_Specifier(w, type_specifier);

    //创建变量序列结点
    w = get_token(p, line, token_text);
    if (w.token_kind != IDENT) {
        printf(
            "fail in creating local val declaration: expect IDENT (in line %d)\n",
            line);
        error = 1;
        destoryAST(local_val_declaration);
        return NULL;
    }
    enQueue(Q, w);
    w = get_token(p, line, token_text);
    if (w.token_kind != COMMA && w.token_kind != SEMICOLON &&
        w.token_kind != LSUBSCRIPT && w.token_kind != ASSIGN) {
        printf(
            "fail in creating local val declaration: expect \';\' (in line %d)\n",
            line);
        error = 1;
        destoryAST(local_val_declaration);
        return NULL;
    }
    enQueue(Q, w);

    char son1_gt[] = "val list";
    p_treeNode val_list = insertChild(local_val_declaration, -1, son1_gt);
    val_list = Val_List(Q, val_list, p, line, token_text);
    return local_val_declaration;
} // end of Local_Val_List

/**
 * @description: 创建函数调用结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} Fun_Call结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Params_Call
 * @called_by: Expression
 */
p_treeNode Fun_Call(Queue& Q, p_treeNode fun_Call, FILE* p, int& line,
    char token_text[]) {
    //调用此函数时Q中应有一个ident元素
    //创建函数名结点
    token_info w;
    deQueue(Q, w);
    p_treeNode fun_name = insertChild(fun_Call, IDENT, w.token_text);

    //创建函数参数集结点
    char son1_gt[] = "params";
    p_treeNode params = insertChild(fun_Call, -1, son1_gt);
    params = Params_Call(Q, params, p, line, token_text);

    return fun_Call;
} // end of Fun_Call

/**
 * @description: 创建函数调用的参数语句结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} params_call结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call:
 * @called_by: Fun_Call
 */
p_treeNode Params_Call(Queue& Q, p_treeNode params_call, FILE* p, int& line,
    char token_text[]) {
    //调用此函数时Q中应无元素
    token_info w = get_token(p, line, token_text);
    while (w.token_kind != RPARENTHESE) {
        if (w.token_kind == IDENT || is_CONST_value(w.token_kind)) {
            p_treeNode param = insertChild(params_call, w.token_kind, w.token_text);
            w = get_token(p, line, token_text);
        }
        else if (w.token_kind == COMMA) {
            w = get_token(p, line, token_text);
        }
        else {
            printf("fail in creating fun call params: expect \')\' (in line %d)\n",
                line);
            error = 1;
            destoryAST(params_call);
            return NULL;
        }
    }
    return params_call;
}

/**
 * @description: 创建返回语句结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} return_statement结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call: Expression
 * @called_by: Statement
 */
p_treeNode Return_Statement(Queue& Q, p_treeNode return_statement, FILE* p,
    int& line, char token_text[]) {
    token_info w = get_token(p, line, token_text);
    enQueue(Q, w);
    char son0_gt[] = "expression";
    p_treeNode return_expression = insertChild(return_statement, -1, son0_gt);
    return_expression =
        Expression(Q, return_expression, p, line, token_text, SEMICOLON);
    return return_statement;
} // end of Return_Statement

/**
 * @description: 创建break语句结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} break结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call:
 * @called_by: Statement
 */
p_treeNode Break_Statement(Queue& Q, p_treeNode break_statement, FILE* p,
    int& line, char token_text[]) {
    return break_statement;
}

/**
 * @description: 创建continue语句结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} continue结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @return {p_treeNode} 结点指针
 * @call:
 * @called_by: Statement
 */
p_treeNode Continue_Statement(Queue& Q, p_treeNode continue_statement, FILE* p,
    int& line, char token_text[]) {
    return continue_statement;
}

/**
 * @description: 创建表达式结点
 * @param {Queue} &Q 读取的token队列
 * @param {p_treeNode} expression结点
 * @param {FILE} *p 读取token的文件指针
 * @param {int} &line token的行号
 * @param {char} token_text token自身值
 * @param {int} endsym 标记结束符号
 * @return {p_treeNode} 结点指针
 * @call:
 * @called_by: Statement; Condition_Expression; For_Statement; is_Operator
 */
p_treeNode Expression(Queue& Q, p_treeNode expression, FILE* p, int& line,
    char token_text[], int endsym) {
    //调用该算法时，Q中有表达式的第一个单词

    Stack op;  //运算符栈
    Stack opn; //操作数栈
    iniStack(op);
    iniStack(opn);
    bool expError = false; //错误标记
    token_info w;
    deQueue(Q, w);
    p_treeNode pw = NULL;
    iniTreeNode(pw, BEGIN_END, (char *)"#");
    S_push(op, pw);
    pw = NULL;

    while (!isEmptyStack(op) && w.token_kind != BEGIN_END && !expError) {
        if (is_CONST_value(w.token_kind)) {
            //常数直接进操作数栈，并读取下一个单词
            iniTreeNode(pw, w.token_kind, w.token_text);
            S_push(opn, pw);
            pw = NULL;
            w = get_token(p, line, token_text);
            continue;
        } // end of if(w is const value)
        else if (w.token_kind == IDENT) {
            //读入下一个单词检查是否为函数调用
            token_info nw = get_token(p, line, token_text);
            if (nw.token_kind == LPARENTHESE) {
                // w为函数调用
                enQueue(Q, w);
                char is_fun_call[] = "fun call";
                iniTreeNode(pw, -1, is_fun_call);
                pw = Fun_Call(Q, pw, p, line, token_text);
                S_push(opn, pw);
                pw = NULL;
                w = get_token(p, line, token_text);
                continue;
            }
            else {
                // w不为函数调用
                iniTreeNode(pw, IDENT, w.token_text);
                S_push(opn, pw);
                pw = NULL;
                w = nw;
                continue;
            }
        } // end of else if(w is ident)
        else if (w.token_kind == LPARENTHESE) {
            //左小括号直接进栈
            S_push(op, pw);
            pw = NULL;
            w = get_token(p, line, token_text);
        }
        else if (is_Operator(w.token_kind) || w.token_kind == RPARENTHESE) {
            iniTreeNode(pw, w.token_kind, w.token_text);
            p_treeNode top = NULL;
            S_pop(op, top);
            if (top->node_type == BEGIN_END) {
                if (w.token_kind == RPARENTHESE) {
                    if (w.token_kind == endsym) {
                        w.token_kind = BEGIN_END;
                        continue;
                    }
                    else {
                        error = 2;
                        continue;
                    }
                }
                S_push(op, top);
                S_push(op, pw);
                w = get_token(p, line, token_text);
                continue;
            }
            else if (top->node_type == LPARENTHESE) {
                if (w.token_kind == RPARENTHESE) {
                    //去括号
                    pw = NULL;
                    w = get_token(p, line, token_text);
                }
                else {
                    S_push(op, top);
                    S_push(op, pw);
                    pw = NULL;
                    w = get_token(p, line, token_text);
                }
            }
            else if (top->node_type < w.token_kind) {
                // w的运算优先级高
                S_push(op, top);
                S_push(op, pw);
                pw = NULL;
                w = get_token(p, line, token_text);
            }
            else if (top->node_type >= w.token_kind) {
                // w的运算优先级低
                //取出栈顶运算符执行操作
                p_treeNode t1, t2; // t1:前一个操作数; t2:后一个操作数
                if (!S_pop(opn, t2)) {
                    expError = 3;
                }
                if (!S_pop(opn, t1)) {
                    expError = 4;
                }
                top->firstChild = t1;
                t1->nextBro = t2;
                S_push(opn, top);
                continue;
            }
            else if (w.token_kind == endsym) {
                w.token_kind = BEGIN_END;
            }
            else {
                expError = 5;
            }
        } // end of if(w is operator)
        else if (w.token_kind == endsym) {
            w.token_kind = BEGIN_END;
        }
        else {
            expError = 6;
        }
    } // end of while

    while (!isEmptyStack(op) && S_get_top(op)->node_type != BEGIN_END) {
        //最后对栈中所有运算符执行操作
        p_treeNode top;
        S_pop(op, top);
        p_treeNode t1, t2; // t1:前一个操作数; t2:后一个操作数
        if (!S_pop(opn, t2))
            expError = 7;
        if (!S_pop(opn, t1))
            expError = 8;
        top->firstChild = t1;
        t1->nextBro = t2;
        S_push(opn, top);
    } // end of while

    if (S_num(opn) == 1 && !expError) {
        S_pop(opn, expression->firstChild);
        return expression;
    }
    else {
        printf("fail in creating expression! %d (in line %d)\n", expError, line);
        error = 1;
        destoryAST(expression);
        return NULL;
    }
}

/**
 * @description: 创建常量语句结点
 * @param {token_info} w token种类信息
 * @param {p_treeNode} const_value结点
 * @return {p_treeNode} 结点指针
 * @call:
 * @called_by: Expression
 */
p_treeNode Const_Value(token_info w, p_treeNode const_value) {
    iniTreeNode(const_value, w.token_kind, w.token_text);
    return const_value;
}