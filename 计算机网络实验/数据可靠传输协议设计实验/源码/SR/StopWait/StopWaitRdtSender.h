#pragma once
#include "RdtSender.h"

class SRRdtSender :
	public RdtSender
{
private:
	const int seqsize;
	const int wndsize;
	Packet* const sendBuf;//发送缓冲区指针
	bool* const bufStatus;//记录每个分组的确认状态
	int base, nextSeqnum;
	bool WaitingState;
private:
	void Init();
	void printSlideWindow();
	bool isInWindow(int seqnum);

public:
	SRRdtSender(int sSize, int wsize);
	SRRdtSender();
	bool send(const Message& message);
	bool getWaitingState();
	void timeoutHandler(int seqnum);
	void receive(const Packet& ackPkt);
	virtual ~SRRdtSender();
};