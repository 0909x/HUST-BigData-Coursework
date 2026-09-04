#include "stdafx.h"
#include "Global.h"
#include "StopWaitRdtReceiver.h"

GBNReceiver::GBNReceiver() :seq(1)//初始化
{
    lastAckPkt.acknum = 0;
    lastAckPkt.checksum = 0;
    lastAckPkt.seqnum = -1;//期望下一个
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
        lastAckPkt.payload[i] = '.';//先用.填充
    lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}
GBNReceiver::~GBNReceiver()
{
}
void GBNReceiver::receive(const struct Packet& packet)
{
    int checkSum = pUtils->calculateCheckSum(packet);//计算检查和

    if (checkSum == packet.checksum && this->seq == packet.seqnum)//正确收到按序分组
    {
        pUtils->printPacket("接收方正确收到按序分组", packet);
        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload));//上交文件至应用层
        pns->delivertoAppLayer(RECEIVER, msg);

        lastAckPkt.acknum = packet.seqnum;//准备发回ack
        lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);//计算ackpackage检查和

        pUtils->printPacket("接收方发送确认报文", lastAckPkt);
        pns->sendToNetworkLayer(SENDER, lastAckPkt);//发送ack给发送方

        this->seq++;
    }

    else//失序或损坏，丢弃，重发上次ack
    {
        if (packet.acknum != seq)//失序
            pUtils->printPacket("ERROR：接收方未收到正确报文：失序", packet);
        else
            pUtils->printPacket("ERROR：接收方未收到正确报文：损坏", packet);

        pUtils->printPacket("接收方重发上次ack", lastAckPkt);
        pns->sendToNetworkLayer(SENDER, lastAckPkt);

    }
}