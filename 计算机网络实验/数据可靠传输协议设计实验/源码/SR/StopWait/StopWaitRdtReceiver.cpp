#include "stdafx.h"
#include "StopWaitRdtReceiver.h"
#include "Global.h"

void SRRdtReceiver::Init()
{
	base = 0;
	for (int i = 0; i < seqsize; i++)
	{
		bufStatus[i] = false;
	}
	lastAckPkt.acknum = -1; //第一个出错下一次依旧0
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;
	memset(lastAckPkt.payload, '.', Configuration::PAYLOAD_SIZE);
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

void SRRdtReceiver::printSlideWindow()
{
	int i;
	for (i = 0; i < seqsize; i++)
	{
		if (i == base)
			std::cout << "[";
		if (isInWindow(i) && bufStatus[i] == true)
			std::cout << "已缓存";
		else if (isInWindow(i))
			std::cout << "期待";
		if (i == (base + wndsize) % seqsize)
			std::cout << "]";
		if (isInWindow(i) == false)
			std::cout << "不可用";
		std::cout << " ";
	}
	std::cout << std::endl;
}

bool SRRdtReceiver::isInWindow(int seqnum)
{
	if (base < (base + wndsize) % seqsize)
	{
		return seqnum >= base && seqnum < (base + wndsize) % seqsize;
	}
	else
	{
		return seqnum >= base || seqnum < (base + wndsize) % seqsize;
	}
}

SRRdtReceiver::SRRdtReceiver() ://窗口4，序号0-7
	seqsize(8), wndsize(4), recvBuf(new Packet[seqsize]), bufStatus(new bool[seqsize])
{
	Init();
}

SRRdtReceiver::SRRdtReceiver(int sSize, int wsize) :
	seqsize(sSize), wndsize(wsize), recvBuf(new Packet[seqsize]), bufStatus(new bool[seqsize])
{
	Init();
}

void SRRdtReceiver::receive(const Packet& packet)
{
	int checksum = pUtils->calculateCheckSum(packet);
	if (checksum != packet.checksum)
	{//检验和出错
		pUtils->printPacket("ERROR:接收方没有正确收到发送方的报文,数据校验错误", packet);
		//std::cout << "\n\n接收的数据分组校验和错误\n\n";
		return;
	}
	else
	{
		if (isInWindow(packet.seqnum) == false)
		{//序号不在窗口内
			pUtils->printPacket("ERROR:不是期望的数据分组", packet);
			lastAckPkt.acknum = packet.seqnum;//即使序号不在窗口内，也发送该序号的ACK，帮助发送方快速重传
			lastAckPkt.seqnum = -1;
			memset(lastAckPkt.payload, '.', Configuration::PAYLOAD_SIZE);
			lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);
			return;
		}
		else
		{//序号在窗口内
			recvBuf[packet.seqnum] = packet;//缓存
			bufStatus[packet.seqnum] = true;//已确认
			lastAckPkt.acknum = packet.seqnum;//发送ACK
			lastAckPkt.seqnum = 0;
			memset(lastAckPkt.payload, '.', sizeof(lastAckPkt.payload));
			pUtils->printPacket("接收方发送ack", lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);
			while (bufStatus[base] == true)//递交数据
			{
				Message msg;
				memcpy(msg.data, recvBuf[base].payload, sizeof(recvBuf[base].payload));
				pns->delivertoAppLayer(RECEIVER, msg);//递交应用层
				pUtils->printPacket("递交到应用层的数据:", recvBuf[base]);
				bufStatus[base] = false;//释放缓存
				base = (base + 1) % seqsize;//移动窗口基序号
			}
			std::cout << "\n收到数据包，窗口移动：";
			printSlideWindow();
			std::cout << std::endl;
		}
	}

}


SRRdtReceiver::~SRRdtReceiver()
{
	delete[] recvBuf;
	delete[] bufStatus;
}