from os_ken.base import app_manager
from os_ken.controller import ofp_event
from os_ken.controller.handler import MAIN_DISPATCHER, CONFIG_DISPATCHER
from os_ken.controller.handler import set_ev_cls
from os_ken.ofproto import ofproto_v1_3
from os_ken.lib.packet import packet
from os_ken.lib.packet import ethernet
from os_ken.lib.packet import arp
from os_ken.lib.packet import ether_types

ETHERNET = ethernet.ethernet.__name__
ETHERNET_MULTICAST = "ff:ff:ff:ff:ff:ff"
ARP = arp.arp.__name__


class Switch_Dict(app_manager.OSKenApp):
    OFP_VERSIONS = [ofproto_v1_3.OFP_VERSION]

    def __init__(self, *args, **kwargs):
        super(Switch_Dict, self).__init__(*args, **kwargs)
        self.flag = 0 # only modify once
        # maybe you need a global data structure to save the mapping
        # just data structure in mission 1
            

    def add_flow(self, datapath, priority, match, actions, idle_timeout=0, hard_timeout=0):
        dp = datapath
        ofp = dp.ofproto
        parser = dp.ofproto_parser
        inst = [parser.OFPInstructionActions(ofp.OFPIT_APPLY_ACTIONS, actions)]
        mod = parser.OFPFlowMod(datapath=dp, priority=priority,
                                idle_timeout=idle_timeout,
                                hard_timeout=hard_timeout,
                                match=match, instructions=inst)
        dp.send_msg(mod)

    @set_ev_cls(ofp_event.EventOFPSwitchFeatures, CONFIG_DISPATCHER)
    def switch_features_handler(self, ev):
        msg = ev.msg
        dp = msg.datapath
        ofp = dp.ofproto
        parser = dp.ofproto_parser
        match = parser.OFPMatch()
        actions = [parser.OFPActionOutput(ofp.OFPP_CONTROLLER, ofp.OFPCML_NO_BUFFER)]
        self.add_flow(dp, 0, match, actions)
                


    @set_ev_cls(ofp_event.EventOFPPacketIn, MAIN_DISPATCHER)
    def packet_in_handler(self, ev):
        msg = ev.msg
        dp = msg.datapath
        ofp = dp.ofproto
        parser = dp.ofproto_parser

        # the identity of switch
        dpid = dp.id
        # the port that receive the packet
        in_port = msg.match['in_port']
        pkt = packet.Packet(msg.data)
        eth_pkt = pkt.get_protocol(ethernet.ethernet)
        if eth_pkt.ethertype == ether_types.ETH_TYPE_LLDP:
            return
        if eth_pkt.ethertype == ether_types.ETH_TYPE_IPV6:
            return
        # get the mac
        dst = eth_pkt.dst
        src = eth_pkt.src
        
        # you need to code here to avoid broadcast loop to finish mission 2
        if dpid == 1 and self.flag == 0:
            # 禁用 s1 的一个端口来打破环路
            # 可以选择与 s3 或 s4 连接的端口，3与S3连接，4与S4连接
            target_port = 3
            
            # 构造 OFPPortMod 消息禁用端口
            req = parser.OFPPortMod(
                datapath=dp,
                port_no=target_port,
                hw_addr=dp.ports[target_port].hw_addr,  # 获取端口的MAC地址
                config=ofp.OFPPC_PORT_DOWN,  # 禁用端口
                mask=ofp.OFPPC_PORT_DOWN,    # 只修改端口禁用状态
                advertise=0                  # 不修改通告能力
            )
            dp.send_msg(req)
            
            self.logger.info("Disabled port %s on switch s1 to break loop", target_port)
            self.flag = 1  # 只执行一次

        # self-learning
        # you need to code here to avoid the direct flooding
        # having fun
        # :)
        # just code in mission 1
        
        # 初始化mac_to_port字典
        if not hasattr(self, 'mac_to_port'):
            self.mac_to_port = {}
        
        # 初始化当前交换机的mac_to_port表
        if dpid not in self.mac_to_port:
            self.mac_to_port[dpid] = {}
        
        # 学习源MAC地址和端口映射
        self.mac_to_port[dpid][src] = in_port
        
        # 如果目的MAC地址在mac_to_port表中，则向指定端口转发
        if dst in self.mac_to_port[dpid]:
            out_port = self.mac_to_port[dpid][dst]
            
            # 打印映射表命中时的转发信息
            self.logger.info("(%s, %s, %s, %s, %s)", 
                            dpid, src, in_port, dst, out_port)
            
            # 创建匹配条件和动作
            match = parser.OFPMatch(eth_dst=dst)
            actions = [parser.OFPActionOutput(out_port)]
            
            # 下发流表，设置hard_timeout=5/0
            self.add_flow(dp, 1, match, actions, hard_timeout=0)
            
            # 发送数据包到指定端口
            out = parser.OFPPacketOut(
                datapath=dp, buffer_id=msg.buffer_id, 
                in_port=in_port, actions=actions, data=msg.data)
            dp.send_msg(out)
        else:
            # 如果目的MAC地址未知，则洪泛数据包
            actions = [parser.OFPActionOutput(ofp.OFPP_FLOOD)]
            out = parser.OFPPacketOut(
                datapath=dp, buffer_id=msg.buffer_id, 
                in_port=in_port, actions=actions, data=msg.data)
            dp.send_msg(out)
