#include "stdafx.h"
#include "Global.h"
#include "StopWaitRdtSender.h"
#include <iostream>

GBNSender::GBNSender() :base(1), nextseqnum(1), waitingState(false), num_packet_win(0)
{
}
GBNSender::~GBNSender()
{
}

void GBNSender::printSlideWindow() {
    printf("【滑动窗口状态】");
    printf("base=%d, nextseqnum=%d, 窗口内分组数=%d/%d, 等待状态=%s\n",
        base, nextseqnum, num_packet_win, len, waitingState ? "是" : "否");

    if (num_packet_win == 0) {
        printf("窗口内容: [空]\n");
    }
    else {
        printf("窗口内容: [");
        for (int i = 0; i < num_packet_win; i++) {
            printf("分组%d", win[i].seqnum);
            if (i < num_packet_win - 1) printf(" | ");
        }
        printf("]\n");
    }
}

bool GBNSender::getWaitingState()
{
    return waitingState;
}

bool GBNSender::send(const struct Message& message)
{
    if (nextseqnum < base + len)//检查发送窗口是否已满
    {
        this->waitingState = false;//标记窗口未满
        this->win[num_packet_win].acknum = -1;//ack置-1
        this->win[num_packet_win].seqnum = this->nextseqnum;//指向下一个报文
        this->win[num_packet_win].checksum = 0;//检查和置0
        memcpy(this->win[num_packet_win].payload, message.data, sizeof(message.data));//拷贝数据

        this->win[num_packet_win].checksum = pUtils->calculateCheckSum(this->win[num_packet_win]);//计算检查和
        pUtils->printPacket("发送方发送报文", this->win[num_packet_win]);
        if (base == nextseqnum)//为base启动计时器
            pns->startTimer(SENDER, Configuration::TIME_OUT, this->win[num_packet_win].seqnum);

        pns->sendToNetworkLayer(RECEIVER, this->win[num_packet_win]);//发送报文至网络层
        this->num_packet_win++;//窗口内packet数目+1

        printf(">>>>>> 发送分组%d <<<<<<\n", this->win[num_packet_win - 1].seqnum);
        printSlideWindow();
        printf("\n");

        if (num_packet_win >= len)//检查窗口是否已满
            this->waitingState = true;

        this->nextseqnum++;//发送下一个组
        return true;
    }
    else//满了直接下一个
    {
        this->waitingState = true;
        return false;
    }
}

void GBNSender::receive(const struct Packet& ack)//接受确认ack
{
    if (this->num_packet_win > 0)//窗口报文数大于0
    {
        int checkSum = pUtils->calculateCheckSum(ack);//计算检查和

        if (checkSum == ack.checksum && ack.acknum >= this->base)//累计确认
        {
            int num = ack.acknum - this->base + 1;

            printf(">>收到ACK%d，开始移动窗口<<\n", ack.acknum);
            printf("移动前: base=%d, 窗口大小=%d\n", base, num_packet_win);
            printSlideWindow();  // 移动前打印

            base = ack.acknum + 1;//重设base
            pUtils->printPacket("发送方收到正确确认", ack);

            if (this->base == this->nextseqnum)//没有已传输未确认的分组
                pns->stopTimer(SENDER, this->win[0].seqnum);
            else//否则重启计时器
            {
                pns->stopTimer(SENDER, this->win[0].seqnum);
                pns->startTimer(SENDER, Configuration::TIME_OUT, this->win[num].seqnum);
            }

            for (int i = 0; i < num_packet_win - num; i++)//移动窗口
            {
                win[i] = win[i + num];
            }
            this->num_packet_win = this->num_packet_win - num;//packet数目减去num

            printf("移动后: base=%d, 窗口大小=%d\n", base, num_packet_win);
            printSlideWindow();  // 移动后打印
            printf(">>窗口移动完成<<\n\n");
        }
    }
}

void GBNSender::timeoutHandler(int seqNum)
{
    printf("!!超时重传!!\n");
    printf("超时的分组: %d\n", seqNum);
    printSlideWindow();

    pUtils->printPacket("超时重发", this->win[0]);
    pns->stopTimer(SENDER, this->win[0].seqnum);
    pns->startTimer(SENDER, Configuration::TIME_OUT, this->win[0].seqnum);
    for (int i = 0; i < num_packet_win; i++)//挨个重发所有已发送但未确认分组
    {
        pns->sendToNetworkLayer(RECEIVER, this->win[i]);
    }

    printf("重传完成\n\n");
}

void GBNSender::Init()
{
    // 初始化代码
}

