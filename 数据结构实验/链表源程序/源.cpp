#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
//define
#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
#define OVERFLOW -2
#define MAX_NUM 10

typedef int status;
typedef int ElemType;

#define LIST_INIT_SIZE 100
#define LISTINCREMENT  10
typedef struct LNode {
    ElemType data;
    struct LNode* next;
}LNode, * LinkList;

status InitList(LinkList& L);
status DestroyList(LinkList& L);
status ClearList(LinkList& L);
status ListEmpty(LinkList L);
int ListLength(LinkList L);
status GetElem(LinkList L, int i, ElemType& e);
status LocateElem(LinkList L, ElemType e);
status PriorElem(LinkList L, ElemType e, ElemType& pre);
status NextElem(LinkList L, ElemType e, ElemType& next);
status ListInsert(LinkList& L, int i, ElemType e);
status ListDelete(LinkList& L, int i, ElemType& e);
status ListTraverse(LinkList L);
status reverseList(LinkList L);
status RemoveNthFromEnd(LinkList L, int n);
status sortlist(LinkList L, int order);
status SaveList(LinkList L, char FileName[]);
status LoadList(LinkList& L, char FileName[]);

int main() {
    char filename[40];
    int op = 1;
    int i, i_num = 1, len, order = 0;
    LinkList L[MAX_NUM];
    for (i = 0; i < MAX_NUM; i++)
    {
        L[i] = NULL;
    }
    ElemType e, cur_e, pre_e, next_e;
    while (op) {
        system("cls");
        printf("\n\n");
        printf("      \t\t\t菜单 \n");
        printf("  可在%d个顺序表进行多表操作，初始化请先操作功能18,默认在第一个表上操作\n", MAX_NUM);
        printf("  ------------------------------------------------------------------------------\n");
        printf("\t\t\t1. InitList创建        9.  NextElem后继\t\t\t         \n");
        printf("\t\t\t2. DestroyList销毁     10. ListInsert插入\t\t\t \n");
        printf("\t\t\t3. ClearList清空       11. ListDelete删除 \t\t\t \n");
        printf("\t\t\t4. ListEmpty判空       12. ListTraverse遍历\t\t\t \n");
        printf("\t\t\t5. ListLength求长      13. reverseList反转\t\t\t \n");
        printf("\t\t\t6. GetElem获取         14. RemoveNthFromEnd删除结点\t\t\t \n");
        printf("\t\t\t7. LocateElem定位      15. sortlist排序\t\t\t         \n");
        printf("\t\t\t8. PriorElem前驱       16. SaveList存\t\t\t         \n");
        printf("\t\t\t                       17.LoadList取\t\t\t \n");
        printf("\t\t\t                       18.Choose选择表\n");
        printf("\t\t\t0. Exit退出           \t\t\t \n");
        printf("  ------------------------------------------------------------------------------\n");
        printf("请选择你的操作[0-18]:\n");
        scanf("%d", &op);
        switch (op)
        {
        case 1:
            //初始化线性表
            if (InitList(L[i_num]) == OK)
            {

                printf("请输入要保存的线性表名称\n");
                scanf("%s", filename);
                printf("线性表创建成功\n");
            }
            else printf("线性表创建失败！\n");
            getchar(); getchar();
            break;

        case 2:
            //销毁线性表
            if (L[i_num] == NULL)
            {
                printf("线性表不存在!\n");
                getchar(); getchar();
                break;
            }
            if (DestroyList(L[i_num]) == OK)
            {
                printf("销毁线性表成功!\n");
            }
            else printf("销毁线性表失败！\n");
            getchar(); getchar();
            break;

        case 3:
            //重置线性表
            if (L[i_num] == NULL)
            {
                printf("线性表不存在!\n");
                getchar(); getchar();
                break;
            }
            if (ClearList(L[i_num]) == OK)
            {
                printf("线性表重置成功！\n");
            }
            else printf("线性表重置失败！\n");
            getchar(); getchar();
            break;

        case 4:
            //判断是否为空
            if (L[i_num] == NULL)
            {
                printf("线性表不存在!\n");
                getchar(); getchar();
                break;
            }
            if (ListEmpty(L[i_num]))
            {
                printf("线性表为空！\n");
            }
            else printf("线性表不是空表！\n");
            getchar(); getchar();
            break;

        case 5:
            //得到线性表长度
            if (L[i_num] == NULL)
            {
                printf("线性表不存在!\n");
                getchar(); getchar();
                break;
            }
            printf("线性表表长为%d\n", ListLength(L[i_num]));
            getchar(); getchar();
            break;

        case 6:
            //得到某个元素
            if (L[i_num] == NULL)
            {
                printf("线性表不存在!\n");
                getchar(); getchar();
                break;
            }
            printf("请输入要取结点的位置：\n");
            scanf("%d", &i);
            if (GetElem(L[i_num], i, e) == OK)
                printf("第%d个结点的元素是：%d\n", i, e);
            else  printf("输入位置错误！\n");
            getchar(); getchar();
            break;

        case 7:
            //查找e元素在线性表中的位置
            if (L[i_num] == NULL)
            {
                printf("线性表不存在!\n");
                getchar(); getchar();
                break;
            }
            printf("请输入数据元素值：\n");
            scanf("%d", &e);
            i = LocateElem(L[i_num], e);
            if (i != ERROR) {
                printf("%d元素第一次出现位于第%d个位置！\n", e, i);
            }
            else  printf("该元素不存在!\n");
            getchar(); getchar();
            break;

        case 8:
            //前驱
            if (L[i_num] == NULL)
            {
                printf("线性表不存在!\n");
                getchar(); getchar();
                break;
            }
            printf("请输入数据元素：\n");
            scanf("%d", &cur_e);
            PriorElem(L[i_num], cur_e, pre_e);
            if (PriorElem(L[i_num], cur_e, pre_e) == OK)
                printf("其前驱元素为：%d\n", pre_e);
            else if (PriorElem(L[i_num], cur_e, pre_e) == OVERFLOW)
                printf("顺序表中没有该元素！\n");
            else  printf("其不存在前驱元素！\n");
            getchar(); getchar();
            break;


        case 9:
            //后继
            if (L[i_num] == NULL)
            {
                printf("线性表不存在!\n");
                getchar(); getchar();
                break;
            }
            printf("请输入数据元素：\n");
            scanf("%d", &cur_e);
            if (NextElem(L[i_num], cur_e, next_e) == OK) {
                printf("其后继元素为：%d\n", next_e);
            }
            else if (NextElem(L[i_num], cur_e, pre_e) == OVERFLOW) {
                printf("顺序表中没有该元素！\n");
            }
            else {
                printf("其不存在后继元素！\n");
            }
            getchar(); getchar();
            break;

        case 10:
            //插入元素
            if (L[i_num] == NULL)
            {
                printf("线性表不存在!\n");
                getchar(); getchar();
                break;
            }
            printf("请输入您要插入的数据元素：\n");
            scanf("%d", &e);
            printf("请输入您要插入的数据元素的位置：\n");
            scanf("%d", &i);
            if (ListInsert(L[i_num], i, e) == OK)
                printf("插入数据元素成功！\n");
            else
                printf("插入数据元素失败！\n");
            getchar(); getchar();
            break;

        case 11:
            //删除元素
            if (L[i_num] == NULL)
            {
                printf("线性表不存在!\n");
                getchar(); getchar();
                break;
            }
            printf("请输入您要删除的数据元素的位置：\n");
            scanf("%d", &i);
            if (ListDelete(L[i_num], i, e) == OK)
                printf("删除数据元素成功！\n");
            else
                printf("删除数据元素失败！\n");
            getchar(); getchar();
            break;

        case 12:
            //遍历线性表中的元素
            if (L[i_num] == NULL)
            {
                printf("线性表不存在!\n");
                getchar(); getchar();
                break;
            }
            if (ListEmpty(L[i_num])) {
                printf("线性表是空表！\n");
                getchar(); getchar();
                break;
            }
            ListTraverse(L[i_num]);
            getchar(); getchar();
            break;

        case 13:
            //反转线性表
            if (L[i_num] == NULL) {
                printf("线性表不存在！");
                getchar(); getchar();
                break;
            }
            if (reverseList(L[i_num]) == OK) {
                printf("反转完成");
            }
            getchar(); getchar();
            break;

        case 14:
            //删除倒数第n个元素
            if (L[i_num] == NULL) {
                printf("线性表是空表");
                getchar(); getchar();
                break;
            }
            len = ListLength(L[i_num]);
            printf("请输入要删除的倒数第几个位置\n");
            scanf("%d", &i);
            if (i < 1 || i > len) {
                printf("要删除的位置不合法!");
                getchar(); getchar();
                break;
            }
            RemoveNthFromEnd(L[i_num], i);
            printf("删除成功！");
            getchar(); getchar();
            break;

        case 15:
            //线性表排序
            if (L[i_num] == NULL) {
                printf("线性表是空表！");
                getchar(); getchar();
                break;
            }
            printf("请输入排序方式，0：从小到大，1：从大到小\n");
            scanf("%d", &order);
            sortlist(L[i_num], order);
            printf("排序成功");
            getchar(); getchar();
            break;

        case 16:
            //保存文件
            if (L[i_num] == NULL)
            {
                printf("线性表不存在!\n");
                getchar(); getchar();
                break;
            }
            if (SaveList(L[i_num], filename) == OK) {
                printf("文件保存成功!文件名为%s\n", filename);
            }
            getchar(); getchar();
            break;

        case 17:
            //加载文件，需要输入需要加载的名称
            if ((L[i_num] != NULL)) {
                printf("顺序表已存在，无法加载！");
            }
            else {
                printf("请输入要加载的文件名:\n ");
                scanf("%s", filename);
                if (LoadList(L[i_num], filename) == OK) {
                    printf("文件加载成功\n");
                }
            }
            getchar(); getchar();
            break;

        case 18:
            //选择在哪个表进行操作
            printf("请输入要在第几个表操作:\n ");
            printf("*只支持在%d个顺序表上操作*\n", MAX_NUM);
            scanf("%d", &i_num);
            printf("正在对第%d个表进行操作\n", i_num);
            if ((i_num < 1) || (i_num > 10))
            {
                printf("请选择正确范围！\n");
                i_num = 1;
            }
            getchar(); getchar();
            break;

        case 0:
            break;
        }//end of switch
    }//end of while
    printf("\t\t欢迎再次使用本系统！\n");
}//end of main()

//函数名称：IntiaList
//函数功能：构造一个空的线性表
status InitList(LinkList& L)
{
    if (L != NULL) {
        return INFEASIBLE;
    }
    LNode* p;
    p = (LNode*)malloc(sizeof(LNode));
    p->next = NULL;
    L = p;
    return OK;
}
//函数名称：DestoryList
//函数功能：销毁线性表
status DestroyList(LinkList& L)
{
    if (L == NULL) {
        return INFEASIBLE;
    }
    LNode* p;
    for (p = L->next; p; p = L->next) {
        L->next = p->next;
        free(p);
    }
    free(L);
    L = NULL;
    return OK;
}
//函数名称：ClearList
//函数功能：重置顺序表
status ClearList(LinkList& L)
{
    if (L == NULL) {
        return INFEASIBLE;
    }
    LNode* p;
    for (p = L->next; p; p = L->next) {
        L->next = p->next;
        free(p);
    }
    return OK;
}
//函数名称：ListEmpty
//函数功能：判断线性表是否为空
status ListEmpty(LinkList L)
{
    if (L == NULL) {
        return INFEASIBLE;
    }
    if (L->next == NULL) {
        return TRUE;
    }
    return FALSE;
}
//函数名称：ListLength
//函数功能：求线性表的表长
int ListLength(LinkList L)
{
    if (L == NULL) {
        return INFEASIBLE;
    }
    LNode* p;
    int cnt = 0;
    for (p = L->next; p; p = p->next) {
        cnt++;
    }
    return cnt;
}
//函数名称：GetElem
//函数功能：得到某一个元素的值
status GetElem(LinkList L, int i, ElemType& e)
{
    if (L == NULL) {
        return INFEASIBLE;
    }
    int index = 1;
    LNode* p;
    if (i < 1) {
        return ERROR;
    }
    for (p = L->next; p; p = p->next) {
        if (index == i) {
            e = p->data;
            return OK;
        }
        index++;
    }
    return ERROR;
}
//函数名称：LocateElem
//函数功能：查找元素
status LocateElem(LinkList L, ElemType e)
{
    if (L == NULL) {
        return INFEASIBLE;
    }
    LNode* p;
    int index = 1;
    for (p = L->next; p; p = p->next) {
        if (p->data == e) {
            return index;
        }
        index++;
    }
    return ERROR;
}
//函数名称：PriorElem
//函数功能：求元素的前驱
status PriorElem(LinkList L, ElemType e, ElemType& pre)
{
    if (L == NULL) {
        return INFEASIBLE;
    }
    LNode* p, * prior = L->next;
    for (p = L->next; p; prior = p, p = p->next) {
        if (p->data == e) {
            if (p == L->next) {
                return ERROR;
            }
            else {
                pre = prior->data;
                return OK;
            }
        }
    }
    return OVERFLOW;
}
//函数名称：NextElem
//函数功能：求后继节点
status NextElem(LinkList L, ElemType e, ElemType& next)
{
    if (L == NULL) {
        return INFEASIBLE;
    }
    LNode* p;
    for (p = L->next; p; p = p->next) {
        if (p->data == e) {
            if (p->next == NULL) {
                return ERROR;
            }
            else {
                next = p->next->data;
                return OK;
            }
        }
    }
    return OVERFLOW;
}
//函数名称：ListInsert
//函数功能：插入元素
status ListInsert(LinkList& L, int i, ElemType e)
{
    if (L == NULL) {
        return INFEASIBLE;
    }
    if (i < 1) {
        return ERROR;
    }
    LNode* p, * newnode;
    newnode = (LNode*)malloc(sizeof(LNode));
    newnode->data = e;
    if (i == 1) {
        newnode->next = L->next;
        L->next = newnode;
        return OK;
    }
    int cnt = 0;
    for (p = L->next; p; p = p->next) {
        cnt++;
        if (cnt == i - 1) {
            newnode->next = p->next;
            p->next = newnode;
            return OK;
        }
    }
    free(newnode);
    return ERROR;
}

//函数名称：ListDelete
//函数功能：删除元素
status ListDelete(LinkList& L, int i, ElemType& e)
{
    if (L == NULL) {
        return INFEASIBLE;
    }
    if (i < 1) {
        return ERROR;
    }
    LNode* p, * del;
    int cnt = 0;
    for (p = L; p->next && cnt < i - 1;) {
        p = p->next;
        cnt++;
    }
    if (p->next == NULL) {
        return ERROR;
    }
    del = p->next;
    e = del->data;
    p->next = del->next;
    free(del);
    return OK;
}

//函数名称：ListTraverse
//函数功能：遍历顺序表
status ListTraverse(LinkList L)
{
    if (L == NULL) {
        return INFEASIBLE;
    }
    if (L->next == NULL) {
        return ERROR;
    }
    LNode* p;
    printf("\n---------------------------- all elements -------------------------------------\n");
    for (p = L->next; p; p = p->next) {
        printf("%d", p->data);
        if (p->next) {
            printf(" ");
        }
    }
    printf("\n--------------------------------- end -----------------------------------------\n");
    return OK;
}

//函数名称：SaveList
//函数功能：保存线性表
status SaveList(LinkList L, char FileName[])
{
    if (L == NULL) {
        return INFEASIBLE;
    }
    FILE* fp;
    LNode* p;
    fp = fopen(FileName, "wb");
    if (fp == NULL) {
        return ERROR;
    }
    for (p = L->next; p; p = p->next) {
        fprintf(fp, "%d ", p->data);
    }
    fclose(fp);
    return OK;
}

//函数名称：LoadList
//函数功能：加载文件
status LoadList(LinkList& L, char FileName[]) {
    if (L != NULL) {
        return INFEASIBLE;
    }
    LNode* p, * now;
    p = (LNode*)malloc(sizeof(LNode));
    p->next = NULL;
    L = p;
    now = L;
    FILE* fp;
    int c;
    fp = fopen(FileName, "rb");
    if (fp == NULL) {
        return ERROR;
    }
    while (fscanf(fp, "%d ", &c) == 1) {
        LNode* node = (LNode*)malloc(sizeof(LNode));
        node->data = c;
        node->next = NULL;
        now->next = node;
        now = node;
    }
    fclose(fp);
    return OK;
}
//函数名称：reverseList
//函数功能：翻转线性表
status reverseList(LinkList L)
{
    LinkList p, prior, post;
    if (L == NULL || ListEmpty(L)) {
        return INFEASIBLE;
    }
    if (ListLength(L) == 1) {
        return OK;
    }
    prior = L->next;
    p = prior->next;
    post = p->next;
    while (1) {
        p->next = prior;
        if (post == NULL) {
            break;
        }
        prior = p;
        p = post;
        post = post->next;
    }
    L->next->next = NULL;
    L->next = p;
    return OK;
}
//函数名称：RemoveNthFromEnd
//函数功能：删除倒数第n个元素
status RemoveNthFromEnd(LinkList L, int n)
{
    int temp;
    int length = ListLength(L);
    if (L == NULL || L->next == NULL) {
        return INFEASIBLE;
    }
    if (n < 1 || n > length) {
        return ERROR;
    }
    ListDelete(L, length + 1 - n, temp);
    return OK;
}

//函数名称：sortlist
//函数功能：线性表从小到大排序
status sortlist(LinkList L, int order)
{
    int t;
    if (L == NULL) {
        return INFEASIBLE;
    }
    if (L->next == NULL) {
        return OK;
    }
    LinkList ptr, done;
    if (order) {
        //order=1,从大到小排序
        for (done = NULL; L->next->next != done; done = ptr) {
            for (ptr = L->next; ptr->next != done;) {
                if (ptr->data < ptr->next->data) {
                    t = ptr->data;
                    ptr->data = ptr->next->data;
                    ptr->next->data = t;
                }
                ptr = ptr->next;
            }
        }
    }
    else {
        //order=0,从小到大排序
        for (done = NULL; L->next->next != done; done = ptr) {
            for (ptr = L->next; ptr->next != done;) {
                if (ptr->data > ptr->next->data) {
                    t = ptr->data;
                    ptr->data = ptr->next->data;
                    ptr->next->data = t;
                }
                ptr = ptr->next;
            }
        }
    }
    return OK;
}