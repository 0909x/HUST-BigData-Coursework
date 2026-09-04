#pragma once
#include"def.h"
status InitList(SqList& L)
{
    //线性表L不存在，构造空的线性表
    if (L.elem == NULL) {
        L.elem = (ElemType*)malloc(sizeof(ElemType) * LIST_INIT_SIZE);
        L.length = 0;
        L.listsize = 100;
        return OK;
    }
    //线性表存在，返回INFEASIBLE
    else return INFEASIBLE;
}
status DestroyList(SqList& L)
{
    //线性表存在，销毁线性表
    if (L.elem) {
        free(L.elem); //释放数据元素的空间
        L.elem = NULL;
        L.length = 0;
        L.listsize = 0; //销毁线性表
        return OK;
    }
    //线性表不存在，不销毁
    else return INFEASIBLE;
}
status ClearList(SqList& L)
{
    //线性表存在，进行清空
    if (L.elem) {
        L.length = 0; //将长度设置为0
        return OK;
    }
    //线性表不存在，不清空
    else return INFEASIBLE;
}
status ListEmpty(SqList L)
{
    //线性表存在，判断是否为空
    if (L.elem) {
        //线性表为空
        if (L.length == 0) {
            return TRUE;
        }
        //线性表非空
        else {
            return FALSE;
        }
    }
    //线性表不存在
    else return INFEASIBLE;
}
status ListLength(SqList L)
{
    //线性表存在
    if (L.elem) {
        return L.length; //返回表长
    }
    //线性表不存在
    else return INFEASIBLE;
}
status GetElem(SqList L, int i, ElemType& e)
{
    //线性表存在
    if (L.elem) {
        //i不合法
        if (i<1 || i>L.length) {
            return ERROR;
        }
        //获取第i个元素，保存在e中
        else {
            e = L.elem[i - 1];
            return OK;
        }
    }
    //线性表不存在
    else return INFEASIBLE;
}
int LocateElem(SqList L, ElemType e)
{
    //线性表存在
    if (L.elem) {
        //遍历线性表中元素，寻找e的位置
        for (int i = 0; i < L.length; i++) {
            if (L.elem[i] == e) {
                return i + 1;
            }
        }
        return 0;
    }
    //线性表不存在
    else return INFEASIBLE;
}
status PriorElem(SqList L, ElemType e, ElemType& pre)
{
    //线性表存在
    if (L.elem) {
        //若为第1个元素，则没有前驱
        if (L.elem[0] == e) return ERROR;
        //遍历线性表，获取e的前驱
        for (int i = 1; i < L.length; i++) {
            if (L.elem[i] == e) {
                pre = L.elem[i - 1];
                return OK;
            }
        }
        return ERROR;
    }
    //线性表不存在
    else return INFEASIBLE;
}
status NextElem(SqList L, ElemType e, ElemType& next)
{
    //线性表存在
    if (L.elem) {
        //若e为最后一个元素，则没有后继
        if (L.elem[L.length - 1] == e) {
            return ERROR;
        }
        //遍历获取e的后继
        for (int i = 0; i < L.length - 1; i++) {
            if (L.elem[i] == e) {
                next = L.elem[i + 1];
                return OK;
            }
        }
        //e不存在，无后继
        return ERROR;
    }
    //线性表不存在
    else return INFEASIBLE;
}
status ListInsert(SqList& L, int i, ElemType e)
{
    //线性表存在
    if (L.elem) {
        //插入位置不正确
        if (i<1 || i>L.length + 1) return ERROR;
        else {
            if (L.length >= L.listsize) {
                L.elem = (ElemType*)realloc(L.elem, sizeof(ElemType) * LIST_INIT_SIZE);
                L.listsize += LIST_INIT_SIZE;
            }

            for (int j = L.length; j > i - 1; j--) {
                L.elem[j] = L.elem[j - 1];
            }
            L.elem[i - 1] = e;
            L.length++;
            return OK;
        }
    }
    else return INFEASIBLE;
}
status ListDelete(SqList& L, int i, ElemType& e)
{
    //线性表存在
    if (L.elem) {
        //删除位置不正确
        if (i<1 || i>L.length) return ERROR;
        else {
            e = L.elem[i - 1];
            for (int j = i; j < L.length; j++) {
                L.elem[j - 1] = L.elem[j];
            }
            L.length--;
            return OK;
        }
    }
    //线性表不存在
    else return INFEASIBLE;
}
status ListTraverse(SqList L)
{
    //线性表存在
    if (L.elem) {
        for (int i = 0; i < L.length; i++) {
            if (i != L.length - 1)
                printf("%i ", L.elem[i]);
            //最后一个元素输出后无需空格
            else
                printf("%i", L.elem[i]);
        }

        return OK;
    }
    //线性表不存在
    else return INFEASIBLE;
}
status  SaveList(SqList L, char FileName[])
{
    //线性表存在，将线性表L的的元素写到FileName文件中
    if (L.elem) {
        FILE* pf = fopen(FileName, "w");
        for (int i = 0; i < L.length; i++) {
            fputc(L.elem[i], pf);
        }
        fclose(pf);
        return OK;
    }
    //线性表不存在
    else return INFEASIBLE;
}
status  LoadList(SqList& L, char FileName[])
{
    //线性表L不存在，将FileName文件中的数据读入到线性表L中
    if (L.elem == NULL) {
        //初始化L
        L.elem = (ElemType*)malloc(sizeof(ElemType) * LIST_INIT_SIZE);
        L.length = 0;
        L.listsize = LIST_INIT_SIZE;
        FILE* pf = fopen(FileName, "r");
        char a;
        while ((a = fgetc(pf)) != EOF) {
            if (L.length == L.listsize) {
                L.elem = (ElemType*)realloc(L.elem, sizeof(ElemType) * LIST_INIT_SIZE);
                L.listsize += LIST_INIT_SIZE;
            }
            L.elem[L.length] = a;
            L.length++;
        }
        fclose(pf);
        return OK;
    }
    //线性表存在
    else return INFEASIBLE;
}
status AddList(LISTS& Lists, char ListName[])
{
    //Lists已满，无法增加
    if (Lists.length >= 10) {
        return ERROR;
    }
    //Lists未满，可以增加空线性表
    int i;
    for (i = 0; ListName[i] != 0; i++) {
        Lists.elem[Lists.length].name[i] = ListName[i];
    }
    Lists.elem[Lists.length].name[i] = 0;
    Lists.elem[Lists.length].L.elem = (ElemType*)malloc(sizeof(ElemType) * LIST_INIT_SIZE);
    Lists.elem[Lists.length].L.length = 0;
    Lists.elem[Lists.length].L.listsize = 100;
    Lists.length++;
    return OK;
}
status RemoveList(LISTS& Lists, char ListName[])
{
    //遍历Lists，寻找名称为ListName的线性表
    for (int i = 0; i < Lists.length; i++) {
        int flag = 1;  //作为找到目标线性表的标记
        for (int j = 0; ListName[j] != 0; j++) {
            if (Lists.elem[i].name[j] != ListName[j]) {
                flag = 0;
                break;  //该线性表不是目标线性表，退出该循坏
            }
        }
        //找到目标线性表，删除
        if (flag) {
            for (int k = i + 1; k < Lists.length; k++) {
                Lists.elem[k - 1] = Lists.elem[k];
            }
            Lists.length--;
            return OK;
        }
    }
    //没找到目标线性表
    return ERROR;
}
int LocateList(LISTS Lists, char ListName[])
{
    //遍历Lists，寻找名称为ListName的线性表
    for (int i = 0; i < Lists.length; i++) {
        int flag = 1;  //作为找到目标线性表的标记
        for (int j = 0; ListName[j] != 0; j++) {
            if (Lists.elem[i].name[j] != ListName[j]) {
                flag = 0;
                break;  //该线性表不是目标线性表，退出该循坏
            }
        }
        //找到目标线性表，返回逻辑序号
        if (flag) {
            return i + 1;
        }
    }
    //没找到目标线性表
    return 0;
}

