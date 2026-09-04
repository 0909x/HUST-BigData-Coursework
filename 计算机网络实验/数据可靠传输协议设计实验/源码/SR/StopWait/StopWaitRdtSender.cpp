#include "stdafx.h"
#include "StopWaitRdtSender.h"
#include "Tool.h"
#include "Global.h"

void SRRdtSender::Init()//初始化
{
	base = nextSeqnum = 0;
	for (int i = 0; i < seqsize; i++)
	{
		bufStatus[i] = false;
	}
}
//打印窗口
void SRRdtSender::printSlideWindow()
{
	printf("【滑动窗口状态】");
	printf("base=%d, nextSeqnum=%d, 窗口大小=%d/%d, 等待状态=%s\n",
		base, nextSeqnum, wndsize, seqsize, getWaitingState() ? "是" : "否");

	printf("窗口内容: [");
	bool first = true;
	for (int i = 0; i < seqsize; i++)
	{
		if (isInWindow(i)) {
			if (!first) printf(" | ");
			if (i >= nextSeqnum)//可用，未发送
				printf("分组%d(可用)", i);
			else if (bufStatus[i] == true)//发送，已确认
				printf("分组%d(已确认)", i);
			else//已发送，未确认
				printf("分组%d(未确认)", i);
			first = false;
		}
	}
	printf("]\n");
}

//判断序号是否在窗口内
bool SRRdtSender::isInWindow(int seqnum)
{
	if (base < (base + wndsize) % seqsize)//窗口没有跨越序号边界
	{
		return seqnum >= base && seqnum < (base + wndsize) % seqsize;
	}
	else//循环
	{
		return seqnum >= base || seqnum < (base + wndsize) % seqsize;
	}
}

SRRdtSender::SRRdtSender(int sSize, int wsize) :
	seqsize(sSize), wndsize(wsize), sendBuf(new Packet[sSize]), bufStatus(new bool[sSize])
{
	Init();
}

SRRdtSender::SRRdtSender() :
	seqsize(8), wndsize(4), sendBuf(new Packet[8]), bufStatus(new bool[8])
{
	Init();
}

bool SRRdtSender::send(const Message& message)
{
	if (getWaitingState())//检查窗口是否已满
	{//缓冲区满，等待ack
		printf("ERROR:窗口已满，稍等\n\n");
		return false;
	}

	printf(">>发送分组%d<<\n", nextSeqnum);
	printf("发送前:\n");
	printSlideWindow();
	//数据包
	sendBuf[nextSeqnum].acknum = -1;
	sendBuf[nextSeqnum].seqnum = nextSeqnum;
	memcpy(sendBuf[nextSeqnum].payload, message.data, sizeof(message.data));//拷贝应用层数据
	sendBuf[nextSeqnum].checksum = pUtils->calculateCheckSum(sendBuf[nextSeqnum]);//计算校验和
	pUtils->printPacket("发送方发送报文", sendBuf[nextSeqnum]);

	//发送报文
	pns->sendToNetworkLayer(RECEIVER, sendBuf[nextSeqnum]);
	//启动定时器，每个分组独立定时器
	pns->startTimer(SENDER, Configuration::TIME_OUT, nextSeqnum);
	//发送完毕，更新序号
	nextSeqnum = (nextSeqnum + 1) % seqsize;

	printf("发送后:\n");
	printSlideWindow();
	printf("\n");
	return true;
}


bool SRRdtSender::getWaitingState()
{
	WaitingState = (base + wndsize) % seqsize == (nextSeqnum) % seqsize;
	return WaitingState;
}

void SRRdtSender::receive(const Packet& ackPkt)
{
	int checksum = pUtils->calculateCheckSum(ackPkt);
	if (checksum != ackPkt.checksum)//校验和出错
	{
		pUtils->printPacket("ERROR:接收的ack损坏，校验和出错", ackPkt);
		return;
	}
	else
	{
		printf(">>收到ACK%d<<\n", ackPkt.acknum);
		printf("处理前:\n");
		printSlideWindow();

		pns->stopTimer(SENDER, ackPkt.acknum);//停止计时器
		if (isInWindow(ackPkt.acknum))//ACK在窗口内
		{//更新窗口
			bufStatus[ackPkt.acknum] = true;//已确认
			int oldBase = base;
			while (bufStatus[base] == true)
			{//移动base到第一个未确认
				bufStatus[base] = false;
				base = (base + 1) % seqsize;
			}

			printf("处理后: base从%d移动到%d\n", oldBase, base);
			printSlideWindow();
			printf(">>>>>> 窗口移动完成 <<<<<<\n\n");
		}
		else
		{
			printf("ACK%d不在窗口内，忽略\n", ackPkt.acknum);
			printSlideWindow();
			printf("\n");
		}
	}
}

void SRRdtSender::timeoutHandler(int seqnum)
{
	printf("!!超时重传!!\n");
	printf("超时的分组: %d\n", seqnum);
	printSlideWindow();

	pUtils->printPacket("ERROR:发送方定时器时间到，重发上一次发送的报文", sendBuf[seqnum]);
	//只传超时的单个分组
	pns->sendToNetworkLayer(RECEIVER, sendBuf[seqnum]);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqnum);//重启计时器
	pUtils->printPacket("重发数据完毕", sendBuf[seqnum]);
	printf("重传完成\n\n");
}

SRRdtSender::~SRRdtSender()
{
	delete[] bufStatus;
	delete[] sendBuf;
}