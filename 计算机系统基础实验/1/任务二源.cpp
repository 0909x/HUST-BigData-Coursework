#define _CRT_SECURE_NO_WARNINGS 1
#include <iostream>
#include <windows.h>
#include "fuc.h"
int absVal_standard(int x) { return (x < 0) ? -x : x; }
int netgate_standard(int x) { return -x; }
int bitAnd_standard(int x, int y) { return x & y; }

int main()
{
    int choice;
    char temp;
    do
    {
        std::cout << " menu \n";
        std::cout << "1. 返回 x 的绝对值\n";
        std::cout << "2. 不使用负号，实现 -x\n";
        std::cout << "3. 仅使用 ~ 和 |，实现 &\n";
        std::cout << "4. 仅使用 ~ 和 &，实现 |\n";
        std::cout << "5. 仅使用 ~ 和 &，实现 ^\n";
        std::cout << "6. 判断x是否为最大的正整数（0x7FFFFFFF）\n";
        std::cout << "7. 统计x的二进制表示中 1 的个数\n";
        std::cout << "8. 产生从lowbit 到 highbit 全为1，其他位为0的数\n";
        std::cout << "9. 当x+y 会产生溢出时返回1，否则返回 0\n";
        std::cout << "10. 将x的第n个字节与第m个字节交换，返回交换后的结果\n";
        std::cout << "0. 退出\n";
        std::cout << "请输入你的选择: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
        {
            int x;
            std::cout << "请输入一个整数: ";
            std::cin >> x;
            std::cout << "绝对值为: " << absVal(x) << std::endl;
            std::cout << "自动检测绝对值为: " << absVal_standard(x) << std::endl;
            if (absVal(x) == absVal_standard(x))printf("结果正确\n");
            else printf("结果错误\n");
            system("pause");
            system("cls");
            break;
        }
        case 2:
        {
            int x;
            std::cout << "请输入一个整数: ";
            std::cin >> x;
            std::cout << "负数为: " << negate(x) << std::endl;
            std::cout << "自动检测负数为: " << netgate_standard(x) << std::endl;
            if (negate(x) == netgate_standard(x))
                printf("结果正确\n");
            else
                printf("结果错误\n");
            system("pause");
            system("cls");
            break;
        }
        case 3:
        {
            int x, y;
            std::cout << "请输入两个整数: ";
            std::cin >> x >> y;
            std::cout << "按位与结果为: " << bitAnd(x, y) << std::endl;
            std::cout << "自动检测按位与结果为: " << bitAnd_standard(x, y) << std::endl;
            if (bitAnd(x, y) == bitAnd_standard(x, y))
                printf("结果正确\n");
            else
                printf("结果错误\n");
            system("pause");
            system("cls");
            break;
        }
        case 4:
        {
            int x, y;
            std::cout << "请输入两个整数: ";
            std::cin >> x >> y;
            std::cout << "按位或结果为: " << bitOr(x, y) << std::endl;
            std::cout << "自动检测按位或结果为: " << (x | y) << std::endl;
            if (bitOr(x, y) == (x | y))
                printf("结果正确\n");
            else
                printf("结果错误\n");
            system("pause");
            system("cls");
            break;
        }
        case 5:
        {
            int x, y;
            std::cout << "请输入两个整数: ";
            std::cin >> x >> y;
            std::cout << "按位异或结果为: " << bitXor(x, y) << std::endl;
            std::cout << "自动检测按位异或结果为: " << (x ^ y) << std::endl;
            if (bitXor(x, y) == (x ^ y))
                printf("结果正确\n");
            else
                printf("结果错误\n");
            system("pause");
            system("cls");
            break;
        }
        case 6:
        {
            int x;
            std::cout << "请输入一个整数: ";
            std::cin >> x;
            std::cout << (isTmax(x) ? "是" : "不是") << "最大的正整数（0x7FFFFFFF）" << std::endl;

            system("pause");
            system("cls");
            break;
        }
        case 7:
        {
            int x;
            std::cout << "请输入一个整数: ";
            std::cin >> x;
            std::cout << "二进制表示中1的个数为: " << bitCount(x) << std::endl;

            system("pause");
            system("cls");
            break;
        }
        case 8:
        {
            int lowbit, highbit;
            std::cout << "请输入lowbit和highbit: ";
            std::cin >> lowbit >> highbit;
            std::cout << "生成的bit mask为: " << std::hex << bitMask(highbit, lowbit) << std::endl;

            system("pause");
            system("cls");
            break;
        }
        case 9:
        {
            int x, y;
            std::cout << "请输入两个整数: ";
            std::cin >> x >> y;
            std::cout << (addOK(x, y) ? "会" : "不会") << "产生溢出" << std::endl;

            system("pause");
            system("cls");
            break;
        }
        case 10:
        {
            int x, n, m;
            std::cout << "请输入一个整数和两个字节位置（0~3）: ";
            std::cin >> x >> n >> m;
            std::cout << "交换后的结果为: " << std::hex << byteSwap(x, n, m) << std::endl;

            system("pause");
            system("cls");
            break;
        }
        case 0:
            std::cout << "退出程序。\n";

            system("pause");
            system("cls");
            break;
        default:
            std::cout << "无效的选择。\n";

            system("pause");
            system("cls");
            break;
        }
    } while (choice != 0);
    Sleep(1000);
    return 0;
}
