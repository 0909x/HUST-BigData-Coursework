#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
// 返回 x 的绝对值
int absVal(int x)
{
    int mask = x >> 31;//右移取符号位——正数得0，负数得-1（全1）
    return (x ^ mask) + (~mask + 1); // 将正数保持不变，负数变为其绝对值的补码，加上负数的补码来得到绝对值
}

// 不使用负号，实现 -x
int negate(int x)
{
    return ~x + 1;//取反加1得其补码-利用正负数补码表示的不同，仅改变符号位
}

// 仅使用 ~ 和 |，实现 &
int bitAnd(int x, int y)
{
    return ~(~x | ~y);//非（非A 或 非B）
}

// 仅使用 ~ 和 &，实现 |
int bitOr(int x, int y)
{
    return ~(~x & ~y); // 非（非A 且 非B）
}

// 仅使用 ~ 和 &，实现 ^
int bitXor(int x, int y)
{
    return ~(x & y) & ~(~x & ~y);
    //异或即保留两者对 彼此不同时为1时的每一位，那么删掉相同的位数即可
}

// 判断x是否为最大的正整数（0x7FFFFFFF）
int isTmax(int x)
{ //最大 2147483647
    return !(x + x + 1) & !!x;
    // 通过判断x加上自身再加1的结果是否为0，同时又要保证x本身不为0即可（利用溢出）
}

// 统计x的二进制表示中 1 的个数
int bitCount(int x)
{
    int cnt = 0;
    for (; x; x &= x - 1)cnt++;
    // 不断清除x的最右边的1（通过x &= x - 1），直到x变为0，期间循环的次数即为1的个数。
    //PS：在二进制表示中，对一个数减1相当于将其最右边的1以及右边的所有0取反。
    return cnt;
}

// 产生从lowbit 到 highbit 全为1，其他位为0的数
int bitMask(int highbit, int lowbit)
{
    return (~(~0 << (highbit + 1))) & (~0 << lowbit);
    // 先将0左移highbit+1位，然后再取反，再将结果左移lowbit位，并再次取反即可得
    //前半部分是高于highbit的全为0（低于highbit的全为1，通过&读取后半部分），后半部分就是low以上为1，low以下为0
}

// 当x+y 会产生溢出时返回1，否则返回 0
int addOK(int x, int y)
{
    int sum = x + y;
    return (((sum ^ x) & (sum ^ y)) >> 31);
    // 通过判断加法的结果sum与x、y异或的结果的符号位是否一致即可（一致则溢出
}

// 将x的第n个字节与第m个字节交换，返回交换后的结果
int byteSwap(int x, int n, int m)
{ // 提取第n个字节和第m个字节的值
    int nthByte = (x >> (n << 3)) & 0xFF; // 将x右移n * 8位（一个字节占8位），再析取出最右边的字节（n）
    int mthByte = (x >> (m << 3)) & 0xFF;
    // 构建掩码
    int mask = (0xFF << (n << 3)) | (0xFF << (m << 3));
    // 执行字节交换
    return (x & ~mask) | (nthByte << (m << 3)) | (mthByte << (n << 3));
}
