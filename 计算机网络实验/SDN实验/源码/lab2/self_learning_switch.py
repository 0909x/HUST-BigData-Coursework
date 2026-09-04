from os_ken.base import app_manager
from os_ken.controller import ofp_event
from os_ken.controller.handler import MAIN_DISPATCHER, CONFIG_DISPATCHER
from os_ken.controller.handler import set_ev_cls
from os_ken.ofproto import ofproto_v1_3
from os_ken.lib.packet import packet
from os_ken.lib.packet import ethernet

class Switch(app_manager.OSKenApp):
    
    OFP_VERSIONS = [ofproto_v1_3.OFP_VERSION]
    def __init__(self, *args, **kwargs):
        super(Switch, self).__init__(*args, **kwargs)
        # maybe you need a global data structure to save the mapping
        
    def add_flow(self, datapath, priority, match, actions,idle_timeout=0,hard_timeout=0):
        dp = datapath
        ofp = dp.ofproto
        parser = dp.ofproto_parser
        inst = [parser.OFPInstructionActions(ofp.OFPIT_APPLY_ACTIONS, actions)]
        mod = parser.OFPFlowMod(datapath=dp, priority=priority,
                                idle_timeout=idle_timeout,
                                hard_timeout=hard_timeout,
                                match=match,instructions=inst)
        dp.send_msg(mod)
        
    @set_ev_cls(ofp_event.EventOFPSwitchFeatures, CONFIG_DISPATCHER)
    def switch_features_handler(self, ev):
        msg = ev.msg
        dp = msg.datapath
        ofp = dp.ofproto
        parser = dp.ofproto_parser
        match = parser.OFPMatch()
        actions = [parser.OFPActionOutput(ofp.OFPP_CONTROLLER,ofp.OFPCML_NO_BUFFER)]
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
        # get the mac
        dst = eth_pkt.dst
        src = eth_pkt.src
        
        # You need to code here to avoid the direct flooding
        # Have fun!
        # :)
        
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
