#include"stu.h"
int main(void) {
	SqList L;  int op = 1;
	L.elem = NULL;
	LISTS Lists;  Lists.length = 0;
	while (op) {
		system("cls");	printf("\n\n");
		printf("      Menu for Linear Table On Sequence Structure \n");
		printf("-------------------------------------------------\n");
		printf("  1.InitList          2.DestroyList\n");
		printf("  3.ClearList         4.ListEmpty\n");
		printf("  5.ListLength        6.GetElem\n");
		printf("  7.LocateElem        8.PriorElem\n");
		printf("  9.NextElem          10.ListInsert\n");
		printf("  11.ListDelete       12.ListTraverse\n");
		printf("  13.Save/LoadList    14.AddList\n");
		printf("  15.RemoveList       16.LocateList\n");
		printf("    	  0. Exit\n");
		printf("-------------------------------------------------\n");
		printf("    请选择你的操作[0~16]:");
		scanf("%d", &op);
		switch (op) {
		case 1:
		{
			if (InitList(L) == OK) printf("线性表创建成功！\n");
			else printf("线性表创建失败！\n");
			getchar(); getchar();
			break;
		}
		case 2:
		{
			if (DestroyList(L) == OK) printf("线性表销毁成功！\n");
			else printf("线性表销毁失败！\n");
			getchar(); getchar();
			break;
		}
		case 3:
		{
			if (ClearList(L) == OK) printf("线性表清空成功！\n");
			else printf("线性表清空失败！\n");
			getchar(); getchar();
			break;
		}
		case 4:
		{
			if (ListEmpty(L) == TRUE) printf("线性表为空！\n");
			if (ListEmpty(L) == INFEASIBLE) printf("线性表不存在，判空失败！\n");
			if (ListEmpty(L) == FALSE) printf("线性表不为空！\n");
			getchar(); getchar();
			break;
		}
		case 5:
		{
			if (ListLength(L) != INFEASIBLE) {
				int Length = 0;
				Length = ListLength(L);
				printf("计算线性表长度成功！长度为%d\n", Length);
			}
			else printf("线性表不存在，计算线性表长度失败！\n");
			getchar(); getchar();
			break;
		}
		case 6:
		{
			int i = 0; ElemType e = 0;
			printf("请输入你想获取第几个元素！\n");
			scanf("%d", &i);
			if (GetElem(L, i, e) == ERROR)
			{
				printf("i取值不合法！\n");
			}
			else if (GetElem(L, i, e) == INFEASIBLE)
			{
				printf("线性表不存在，获取线性表元素失败！\n");
			}
			else if (GetElem(L, i, e) == OK)
			{
				printf("获取线性表元素成功！\n");
				printf("该元素为%d", e);
			}
			getchar(); getchar();
			break;
		}
		case 7:
		{
			int i = 0;
			ElemType e = 0;
			printf("请输入想要查找的元素！\n");
			scanf("%d", &e);
			i = LocateElem(L, e);
			if (i == INFEASIBLE)
			{
				printf("线性表不存在！\n");
			}
			if (i == 0)
			{
				printf("该元素不存在！\n");
			}
			if (i != INFEASIBLE && i != 0)
			{
				printf("该元素位置为%d\n", i);
			}
			getchar(); getchar();
			break;
		}
		case 8:
		{
			ElemType e = 0;
			printf("请输入你想获得前驱元素的元素!\n");
			scanf("%d", &e);
			ElemType pre_e = 0;
			if (PriorElem(L, e, pre_e) == ERROR)
			{
				printf("该元素没有前驱！\n");
			}
			if (PriorElem(L, e, pre_e) == INFEASIBLE)
			{
				printf("该线性表不存在！\n");
			}
			if (PriorElem(L, e, pre_e) == OK)
			{
				printf("前驱获得成功！\n");
				printf("该前驱为%d", pre_e);
			}
			getchar(); getchar();
			break;
		}
		case 9:
		{
			ElemType e = 0;
			printf("请输入你想获得后继元素的元素!\n");
			scanf("%d", &e);
			ElemType next_e = 0;
			if (NextElem(L, e, next_e) == ERROR)
			{
				printf("该元素没有后继！\n");
			}
			if (NextElem(L, e, next_e) == INFEASIBLE)
			{
				printf("该线性表不存在！\n");
			}
			if (NextElem(L, e, next_e) == OK)
			{
				printf("后继获得成功！\n");
				printf("该后继为%d", next_e);
			}
			getchar(); getchar();
			break;
		}
		case 10:
		{
			int i;
			ElemType e;
			printf("请输入你想插入的元素位置和元素\n");
			scanf("%d%d", &i, &e);
			if (ListInsert(L, i, e) == OK) printf("插入成功！");
			else if (ListInsert(L, i, e) == INFEASIBLE) printf("线性表L不存在！");
			else printf("插入失败！");
			getchar(); getchar();
			break;
		}
		case 11:
		{
			int i = 0, j = 0;
			ElemType e = 0;
			printf("请输入你想要删除第几个元素！\n");
			scanf("%d", &i);
			j = ListDelete(L, i, e);
			if (j == ERROR)
			{
				printf("删除位置不合法！\n");
			}
			if (j == INFEASIBLE)
			{
				printf("线性表不存在！\n");
			}
			if (j == OK)
			{
				printf("删除成功！\n");
				printf("删除元素为%d", e);
			}
			getchar(); getchar();
			break;
		}
		case 12:  
		{
			if (ListTraverse(L) == INFEASIBLE) printf("线性表是空表！\n");
			getchar(); getchar();
			break;
		}
		case 13:
		{
			char FileName[30] = "file";
			printf("写文件请输入0；读文件请输入1\n");
			int f;
			scanf("%d", &f);
			if (!f) {
				if (SaveList(L, FileName) == OK) printf("写入文件成功!\n");
				else printf("线性表不存在，写入失败！\n");
			}
			else {
				if (LoadList(L, FileName) == OK)
				{
					printf("读文件成功！\n所读文件为：\n");
					for (int i = 0; i < L.length; i++)
						printf("%d ", L.elem[i]);
				}
				else printf("线性表L存在,读文件失败！\n");
			}
			getchar(); getchar();
			break;
		}
		case 14:
		{
			printf("请输入增加线性表的个数：\n");
			int n, e;
			char name[30];
			scanf("%d", &n);
			while (n--)
			{
				printf("请输入线性表的名称：\n");
				scanf("%s", name);
				if (AddList(Lists, name) == OK)
				{
					printf("请输入线性表的元素,以0结束：\n");
					scanf("%d", &e);
					while (e)
					{
						ListInsert(Lists.elem[Lists.length - 1].L, Lists.elem[Lists.length - 1].L.length + 1, e);
						scanf("%d", &e);
					}
					printf("增加成功！\n");
				}
				else printf("增加失败！\n");
			}
			getchar(); getchar();
			break;
		}
		case 15:
		{
			char name[30];
			printf("请输入想要删除的线性表的名称：\n");
			scanf("%s", name);
			if (RemoveList(Lists, name) == OK) printf("删除成功！\n");
			else printf("删除失败！\n");
			getchar(); getchar();
			break;
		}
		case 16:
		{
			char name[30];
			int i;
			printf("请输入想要查找的线性表的名称：\n");
			scanf("%s", name);
			if (i = LocateList(Lists, name)) printf("该线性表的逻辑序号为:%d\n", i);
			else printf("没有找到该线性表\n");
			getchar(); getchar();
			break;
		}
		case 0:
			break;
		}//end of switch
	}//end of while
	printf("欢迎下次再使用本系统！\n");
	return 0;
}//end of main()