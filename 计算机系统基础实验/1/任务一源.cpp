#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <windows.h>
#include "fuc.h"
using namespace std;

int  pack_student_bytebybyte(student* s, int sno, char* buf);
int  pack_student_whole(student* s, int sno, char* buf);
int restore_student(char* buf, int len, student* s);
void printMessage(char* message, int len);
student old_s[N];
student new_s[N];
int main()
{
	//uesrs operation
	for (int i = 0; i < N; i++)//input message
	{
		cin >> old_s[i].name >> old_s[i].age >> old_s[i].score >> old_s[i].remark;
	}
	for (int i = 0; i < N; i++)//ouput message
	{
		cout << old_s[i].name << " " << old_s[i].age << " " << old_s[i].score << " " << old_s[i].remark << endl;
	}
	//initialization
	char message[300];
	memset(message, 0, sizeof(message));
	//compression
	 //N1
	int len = pack_student_bytebybyte(old_s, N1, message);
	//N2
	len += pack_student_whole(&old_s[2], N2, message + len);
	//print
	printMessage(message, len);
	int num = restore_student(message, len, new_s);
	for (int i = 0; i < num; i++)
	{
		cout << new_s[i].name << " " << new_s[i].age << " " << new_s[i].score << " " << new_s[i].remark << endl;
	}
	Sleep(5000);//  ֹһ      
	return 0;
}
/*
//name age score remark
kky 19 56.00 myself
ysy 19 19.00 stude2
hyn 19 19.00 stude3
lxy 20 20.00 stude4
lxm 19 19.00 stude5
*/
