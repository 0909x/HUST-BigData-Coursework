#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <iostream>
#include <cstring>
using namespace std;
#define N 5
#define N1 2
#define N2 3
struct student {
    char name[8];
    short age;
    float score;
    char remark[200];
};
//pack bytebybyte
int pack_student_bytebybyte(student* s, int sno, char* buf)
{
    // s为压缩数组的始地址,sno为压缩人数,buf为压缩存储区的首地址,返回值为压缩后的字节数
    int cnt = 0; // 计数器
    int cntName, cntAge, cntScore, cntRemark, cntBuf = 0;
    char* p = (char*)s; // 指向待操作数组
    char* pd = buf;      // 指向压缩存储地址
    while (cnt < sno)
    {
        // 读取name
        cntName = 0;
        while (cntName < 8)
        {
            if (*p)
            {
                // p不为空时
                *pd = *p;
                cntName++, cntBuf++;
                p++, pd++;
            }
            else
            {
                //*p为\0时
                *pd = 0;
                cntBuf++;
                p += (8 - cntName); // name可能存在空余
                pd++;
                break;
            }
        }

        // 读取age
        cntAge = 0;
        while (cntAge < 2)
        {
            *pd = *p;
            cntBuf++, cntAge++;
            p++, pd++;
        }

        p += 2; // 内存空间中short与float之间有2个字节的未利用空间
        // 读取score
        cntScore = 0;
        while (cntScore < 4)
        {
            *pd = *p;
            cntBuf++, cntScore++;
            pd++, p++;
        }

        // 读取remark
        cntRemark = 0;
        while (cntRemark < 200)
        {
            if (*p)
            {
                *pd = *p;
                cntBuf++, cntRemark++;
                pd++, p++;
            }
            else
            {
                *pd = 0;
                cntBuf++, pd++;
                p += (200 - cntRemark); // 空余处理
                break;
            }
        }
        cnt++;
    }
    return cntBuf;
}

// 整体压缩
int pack_student_whole(student* s, int sno, char* buf)
{
    int cnt = 0;
    char* p = (char*)s;
    char* pd = buf;
    student* pp = s;
    while (cnt < sno)
    {
        // copy name
        strcpy(pd, pp[cnt].name);
        pd += strlen(pp[cnt].name) + 1;
        // copy age
        *((short*)pd) = pp[cnt].age;
        pd += 2;
        // copy score
        *((float*)pd) = pp[cnt].score;
        pd += 4;
        // copy remark
        strcpy(pd, pp[cnt].remark);
        pd += strlen(pp[cnt].remark) + 1;
        cnt++;
    }
    return pd - buf; // 末地址减始地址
}

void printMessage(char* message, int len)
{
    char* p = message;
    while (p - message < len)
    {
        cout << p << " "; // name
        p += strlen(p) + 1;
        cout << *((short*)p) << " "; // age
        p += 2;
        cout << *((float*)p) << " "; // score
        p += 4;
        cout << p << " "; // remark
        p += strlen(p) + 1;
    }
    cout << endl;
}

// 解压函数
int restore_student(char* buf, int len, student* s)
{
    // buf为压缩存储区的首地址，len为压缩数据段长度，s为解压结构数组首地址，返回解压人数
    int cnt = 0; // 计数器->人数
    char* p = buf;
    student* pd = s;
    while ((p - buf) < len)
    {
        // restore name
        strcpy(pd[cnt].name, p);
        p += strlen(pd[cnt].name) + 1;//'\0'
        // restore age
        pd[cnt].age = *((short*)p);
        p += 2;
        // restore score
        pd[cnt].score = *((float*)p);
        p += 2;
        // restore remark
        strcpy(pd[cnt].remark, p);
        p += strlen(pd[cnt].remark) + 1;
        cnt++; // 处理完人数++
    }
    return cnt;
}