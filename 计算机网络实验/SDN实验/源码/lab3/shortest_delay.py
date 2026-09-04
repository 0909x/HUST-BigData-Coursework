from os_ken.base import app_manager
from os_ken.controller import ofp_event
from os_ken.controller.handler import CONFIG_DISPATCHER, MAIN_DISPATCHER, DEAD_DISPATCHER, HANDSHAKE_DISPATCHER
from os_ken.controller.handler import set_ev_cls
from os_ken.ofproto import ofproto_v1_3
from os_ken.lib.packet import packet
from os_ken.lib.packet import ethernet, arp, ipv4, ether_types
from os_ken.topology import event
import sys
from network_awareness import NetworkAwareness
from os_ken.topology.api import get_all_switch
import networkx as nx

ETHERNET = ethernet.ethernet.__name__
ETHERNET_MULTICAST = "ff:ff:ff:ff:ff:ff"
ARP = arp.arp.__name__

class ShortestDelay(app_manager.OSKenApp):
    OFP_VERSIONS = [ofproto_v1_3.OFP_VERSION]
    _CONTEXTS = {
        'network_awareness': NetworkAwareness
    }

    def __init__(self, *args, **kwargs):
        super(ShortestDelay, self).__init__(*args, **kwargs)
        self.network_awareness = kwargs['network_awareness']
        self.weight = 'delay'  # 修改为 'delay' 以使用时延作为权重
        self.mac_to_port = {}
        self.sw = {}
        self.path = None

    def add_flow(self, datapath, priority, match, actions, idle_timeout=0, hard_timeout=0):
        dp = datapath
        ofp = dp.ofproto
        parser = dp.ofproto_parser

        inst = [parser.OFPInstructionActions(ofp.OFPIT_APPLY_ACTIONS, actions)]
        mod = parser.OFPFlowMod(
            datapath=dp, priority=priority,
            idle_timeout=idle_timeout,
            hard_timeout=hard_timeout,
            match=match, instructions=inst)
        dp.send_msg(mod)

    @set_ev_cls(ofp_event.EventOFPPacketIn, MAIN_DISPATCHER)
    def packet_in_handler(self, ev):
        msg = ev.msg
        dp = msg.datapath
        ofp = dp.ofproto
        parser = dp.ofproto_parser

        dpid = dp.id
        in_port = msg.match['in_port']

        pkt = packet.Packet(msg.data)
        eth_pkt = pkt.get_protocol(ethernet.ethernet)
        arp_pkt = pkt.get_protocol(arp.arp)
        ipv4_pkt = pkt.get_protocol(ipv4.ipv4)

        pkt_type = eth_pkt.ethertype

        # layer 2 self-learning
        dst_mac = eth_pkt.dst
        src_mac = eth_pkt.src

        if isinstance(arp_pkt, arp.arp):
            self.handle_arp(msg, in_port, dst_mac, src_mac, pkt, pkt_type)

        if isinstance(ipv4_pkt, ipv4.ipv4):
            self.handle_ipv4(msg, ipv4_pkt.src, ipv4_pkt.dst, pkt_type)

    def handle_arp(self, msg, in_port, dst, src, pkt, pkt_type):
        """
            handle arp loop
        """
        dp = msg.datapath
        dpid = dp.id
        parser = dp.ofproto_parser

        # 记录 ARP 请求的转发历史
        if (dpid, src, dst) not in self.sw:
            self.sw[(dpid, src, dst)] = in_port
        else:
            # 如果已经记录过，说明出现环路，丢弃该包
            return

        # 洪泛 ARP 请求
        actions = [parser.OFPActionOutput(dp.ofproto.OFPP_FLOOD)]
        out = parser.OFPPacketOut(
            datapath=dp, buffer_id=msg.buffer_id, in_port=in_port,
            actions=actions, data=msg.data
        )
        dp.send_msg(out)

    def handle_ipv4(self, msg, src_ip, dst_ip, pkt_type):
        parser = msg.datapath.ofproto_parser

        dpid_path = self.network_awareness.shortest_path(src_ip, dst_ip, weight=self.weight)
        if not dpid_path:
            return

        self.path = dpid_path
        port_path = []
        for i in range(1, len(dpid_path) - 1):
            in_port = self.network_awareness.link_info[(dpid_path[i], dpid_path[i - 1])]
            out_port = self.network_awareness.link_info[(dpid_path[i], dpid_path[i + 1])]
            port_path.append((in_port, dpid_path[i], out_port))
        self.show_path(src_ip, dst_ip, port_path)

        total_delay = 0.0
        link_delay_dict = {}
        
        # 遍历路径上的每条链路
        for i in range(len(dpid_path) - 1):
            src_dpid = dpid_path[i]
            dst_dpid = dpid_path[i + 1]
            link_key = (src_dpid, dst_dpid)
            
            # 查找链路时延
            if link_key in self.network_awareness.link_delay_table:
                delay = self.network_awareness.link_delay_table[link_key]
            elif (dst_dpid, src_dpid) in self.network_awareness.link_delay_table:
                delay = self.network_awareness.link_delay_table[(dst_dpid, src_dpid)]
            else:
                # 如果找不到时延信息，使用默认值或跳过
                self.logger.warning("No delay info for link %s->%s, using 0", src_dpid, dst_dpid)
                delay = 0
                
            # 累加到总延迟（注意单位：秒）
            total_delay += delay
            link_delay_dict[link_key] = delay

        # 输出结果（确保单位转换正确）
        self.logger.info('link delay dict: %s', link_delay_dict)
        self.logger.info("path delay = %.5fms", total_delay * 1000)  # 秒转毫秒
        self.logger.info("path RTT = %.5fms", total_delay * 2 * 1000)  # 往返时间

        # 发送流表
        for node in port_path:
            in_port, dpid, out_port = node
            self.send_flow_mod(parser, dpid, pkt_type, src_ip, dst_ip, in_port, out_port)
            self.send_flow_mod(parser, dpid, pkt_type, dst_ip, src_ip, out_port, in_port)

        # 发送 packet_out
        _, dpid, out_port = port_path[-1]
        dp = self.network_awareness.switch_info[dpid]
        actions = [parser.OFPActionOutput(out_port)]
        out = parser.OFPPacketOut(
            datapath=dp, buffer_id=msg.buffer_id, in_port=in_port, actions=actions, data=msg.data)
        dp.send_msg(out)
        
    def send_flow_mod(self, parser, dpid, pkt_type, src_ip, dst_ip, in_port, out_port):
        dp = self.network_awareness.switch_info[dpid]
        match = parser.OFPMatch(
            in_port=in_port, eth_type=pkt_type, ipv4_src=src_ip, ipv4_dst=dst_ip)
        actions = [parser.OFPActionOutput(out_port)]
        self.add_flow(dp, 1, match, actions, 10, 30)

    def show_path(self, src, dst, port_path):
        self.logger.info('path: {} -> {}'.format(src, dst))
        path = src + ' -> '
        for node in port_path:
            path += '{}:s{}:{}'.format(*node) + ' -> '
        path += dst
        self.logger.info(path)
        
    # ========== 任务三：链路故障容忍实现 ==========

    @set_ev_cls(ofp_event.EventOFPPortStatus, MAIN_DISPATCHER)
    def port_status_handler(self, ev):
        """
        处理端口状态变化事件（链路故障或恢复）
        """
        msg = ev.msg
        datapath = msg.datapath
        ofproto = datapath.ofproto

        self.logger.info("=== Port status changed ===")
        self.logger.info("Switch: %s, Port: %s, Reason: %s", 
                        datapath.id, msg.desc.port_no, msg.reason)

        # 检查是否是链路down事件
        is_link_down = (msg.desc.config & ofproto.OFPPC_PORT_DOWN) or \
                       (msg.desc.state & ofproto.OFPPS_LINK_DOWN)
        
        # 只在真正需要的时候清理（链路down或端口删除）
        if is_link_down or msg.reason == ofproto.OFPPR_DELETE:
            self.logger.info("Significant link change detected, performing selective cleanup")
            self.cleanup_after_link_change()
        else:
            self.logger.info("Minor port change, no cleanup needed")

    def cleanup_after_link_change(self):
        """
        链路变化后的清理工作
        - 只清空交换机之间的链路信息，保留主机信息
        """
        try:
            self.logger.info("=== Starting selective cleanup after link change ===")
            
            # 1. 只清空交换机间的链路信息，保留主机连接信息
            self.logger.info("Step 1: Clearing switch-to-switch links only")
            if hasattr(self.network_awareness, 'topo_map'):
                # 备份主机节点
                host_nodes = [node for node in self.network_awareness.topo_map.nodes() 
                             if isinstance(node, str) and node.startswith('10.0.0.')]
                self.logger.info("Preserving %d host nodes: %s", len(host_nodes), host_nodes)
                
                # 清空拓扑图但重新添加主机
                self.network_awareness.topo_map.clear()
                
                # 重新添加主机到其连接交换机的边
                for host_ip in host_nodes:
                    # 查找主机连接的交换机
                    for key in list(self.network_awareness.link_info.keys()):
                        if key[1] == host_ip:  # (dpid, host_ip)
                            dpid = key[0]
                            self.network_awareness.topo_map.add_edge(host_ip, dpid, hop=1, delay=0, is_host=True)
                            self.logger.debug("Readded host %s connected to switch %s", host_ip, dpid)
                            break
                
                self.logger.info("Switch links cleared, host connections preserved")
            
            # 2. 清空链路信息（但保留主机相关的链路信息）
            if hasattr(self.network_awareness, 'link_info'):
                # 备份主机相关的链路信息
                host_links = {k: v for k, v in self.network_awareness.link_info.items() 
                             if isinstance(k[1], str) and k[1].startswith('10.0.0.')}
                
                self.network_awareness.link_info.clear()
                # 恢复主机链路信息
                self.network_awareness.link_info.update(host_links)
                self.logger.info("Switch links cleared, %d host links preserved", len(host_links))
            
            # 3. 清空端口链路映射
            if hasattr(self.network_awareness, 'port_link'):
                self.network_awareness.port_link.clear()
                self.logger.info("Port link mapping cleared")
            
            # 4. 清空学习表
            self.logger.info("Step 2: Clearing learning tables")
            self.sw.clear()  # ARP转发历史
            self.logger.info("ARP forwarding history (sw) cleared")
            
            self.mac_to_port.clear()  # MAC到端口映射
            self.logger.info("MAC to port mapping cleared")
            
            # 5. 选择性删除流表
            self.logger.info("Step 3: Deleting only IPv4 flow tables")
            self.delete_ipv4_flows_only()
            
            # 6. 保留时延测量数据
            self.logger.info("Step 4: Preserving delay measurement data")
            self.logger.info("lldp_delay_table preserved: %d entries", len(self.network_awareness.lldp_delay_table))
            self.logger.info("echo_RTT_table preserved: %d entries", len(self.network_awareness.echo_RTT_table))
            self.logger.info("link_delay_table preserved: %d entries", len(self.network_awareness.link_delay_table))
            
            self.logger.info("=== Selective cleanup completed ===")
            self.logger.info("Host information preserved for path calculation")
            
        except Exception as e:
            self.logger.error("Cleanup error: %s", str(e))

    def delete_all_flow(self):
        """
        删除所有交换机的所有流表项
        """
        self.logger.info("Starting to delete all flow tables")
        try:
            # 获取所有交换机
            switches = get_all_switch(self)
            self.logger.info("Found %d switches", len(switches))
            
            deleted_count = 0
            for switch in switches:
                datapath = switch.dp
                if datapath and datapath.id:
                    self.logger.info("Deleting flows from switch %s", datapath.id)
                    
                    # 删除所有流表项
                    parser = datapath.ofproto_parser
                    ofproto = datapath.ofproto
                    
                    match = parser.OFPMatch()
                    mod = parser.OFPFlowMod(
                        datapath=datapath,
                        command=ofproto.OFPFC_DELETE,
                        out_port=ofproto.OFPP_ANY,
                        out_group=ofproto.OFPG_ANY,
                        match=match,
                        table_id=ofproto.OFPTT_ALL
                    )
                    datapath.send_msg(mod)
                    deleted_count += 1
                    
                    self.logger.info("Sent delete command to switch %s", datapath.id)
            
            self.logger.info("Flow table deletion completed for %d switches", deleted_count)
            
        except Exception as e:
            self.logger.error("Failed to delete all flow tables: %s", str(e))
            
    def delete_flow(self, datapath, port_no):
        """
        删除与指定端口相关的流表项
        """
        ofproto = datapath.ofproto
        parser = datapath.ofproto_parser

        try:
            self.logger.info("Deleting flow entries for switch %s port %s", datapath.id, port_no)
            
            # 删除所有流表
            match = parser.OFPMatch()
            mod = parser.OFPFlowMod(
                datapath=datapath,
                command=ofproto.OFPFC_DELETE,
                out_port=ofproto.OFPP_ANY,
                out_group=ofproto.OFPG_ANY,
                match=match,
                table_id=ofproto.OFPTT_ALL
            )
            datapath.send_msg(mod)
            
            self.logger.info("All flow entries deleted for switch %s port %s", datapath.id, port_no)
            
        except Exception as e:
            self.logger.error("Failed to delete flow entries for switch %s port %s: %s", 
                            datapath.id, port_no, str(e))
                            
    def delete_ipv4_flows_only(self):
        """
        只删除IPv4数据流表，保留ARP和基础流表
        """
        self.logger.info("Deleting only IPv4 flow tables (preserving ARP flows)")
        try:
            switches = get_all_switch(self)
            self.logger.info("Found %d switches", len(switches))
            
            deleted_count = 0
            for switch in switches:
                datapath = switch.dp
                if datapath and datapath.id:
                    self.logger.info("Deleting IPv4 flows from switch %s", datapath.id)
                    
                    parser = datapath.ofproto_parser
                    ofproto = datapath.ofproto
                    
                    # 只删除IPv4流表，不删除ARP流表
                    match_ipv4 = parser.OFPMatch(eth_type=ether_types.ETH_TYPE_IP)
                    mod = parser.OFPFlowMod(
                        datapath=datapath,
                        command=ofproto.OFPFC_DELETE,
                        out_port=ofproto.OFPP_ANY,
                        out_group=ofproto.OFPG_ANY,
                        match=match_ipv4,
                        table_id=ofproto.OFPTT_ALL
                    )
                    datapath.send_msg(mod)
                    deleted_count += 1
                    
                    self.logger.info("Sent IPv4 delete command to switch %s", datapath.id)
            
            self.logger.info("IPv4 flow table deletion completed for %d switches", deleted_count)
            
        except Exception as e:
            self.logger.error("Failed to delete IPv4 flow tables: %s", str(e))
