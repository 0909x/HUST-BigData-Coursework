from os_ken.base import app_manager
from os_ken.base.app_manager import lookup_service_brick
from os_ken.ofproto import ofproto_v1_3
from os_ken.controller.handler import set_ev_cls
from os_ken.controller.handler import MAIN_DISPATCHER, CONFIG_DISPATCHER, DEAD_DISPATCHER
from os_ken.controller import ofp_event
from os_ken.lib.packet import packet
from os_ken.lib.packet import ethernet, arp
from os_ken.lib import hub
from os_ken.topology import event
from os_ken.topology.api import get_all_host, get_all_link, get_all_switch
from os_ken.topology.switches import LLDPPacket
from os_ken.base.app_manager import lookup_service_brick
import networkx as nx
import copy
import time

GET_TOPOLOGY_INTERVAL = 2
SEND_ECHO_REQUEST_INTERVAL = .05
GET_DELAY_INTERVAL = 2


class NetworkAwareness(app_manager.OSKenApp):
    OFP_VERSIONS = [ofproto_v1_3.OFP_VERSION]

    def __init__(self, *args, **kwargs):
        super(NetworkAwareness, self).__init__(*args, **kwargs)
        self.switch_info = {}  # dpid: datapath
        self.link_info = {}  # (s1, s2): s1.port
        self.port_link = {}  # s1,port:s1,s2
        self.port_info = {}  # dpid: (ports linked hosts)
        self.topo_map = nx.Graph()
        self.topo_thread = hub.spawn(self._get_topology)

        self.weight = 'delay'  # don't forget change it to 'delay'
        # add your variables here
        self.lldp_delay_table = {}  # key: (src_dpid, dst_dpid) -> T_lldp
        self.switches = {}  # switches app instance
        self.echo_RTT_table = {}  # key: dpid -> T_echo
        self.echo_send_timestamp = {}  # key: dpid -> send_time
        self.link_delay_table = {}  # (dpid1, dpid2) -> delay

        # 启动 Echo 测量线程
        self.echo_thread = hub.spawn(self.examine_echo_RTT)

    def add_flow(self, datapath, priority, match, actions):
        dp = datapath
        ofp = dp.ofproto
        parser = dp.ofproto_parser

        inst = [parser.OFPInstructionActions(ofp.OFPIT_APPLY_ACTIONS, actions)]
        mod = parser.OFPFlowMod(datapath=dp, priority=priority, match=match, instructions=inst)
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

    @set_ev_cls(ofp_event.EventOFPStateChange, [MAIN_DISPATCHER, DEAD_DISPATCHER])
    def state_change_handler(self, ev):
        dp = ev.datapath
        dpid = dp.id

        if ev.state == MAIN_DISPATCHER:
            self.switch_info[dpid] = dp

        if ev.state == DEAD_DISPATCHER:
            del self.switch_info[dpid]

    def _get_topology(self):
        _hosts, _switches, _links = None, None, None
        while True:
            hosts = get_all_host(self)
            switches = get_all_switch(self)
            links = get_all_link(self)

            # update topo_map when topology change
            if [str(x) for x in hosts] == _hosts and [str(x) for x in switches] == _switches and [str(x) for x in
                                                                                                  links] == _links:
                continue
            _hosts, _switches, _links = [str(x) for x in hosts], [str(x) for x in switches], [str(x) for x in links]

            for switch in switches:
                self.port_info.setdefault(switch.dp.id, set())
                # record all ports
                for port in switch.ports:
                    self.port_info[switch.dp.id].add(port.port_no)

            for host in hosts:
                # take one ipv4 address as host id
                if host.ipv4:
                    self.link_info[(host.port.dpid, host.ipv4[0])] = host.port.port_no
                    self.topo_map.add_edge(host.ipv4[0], host.port.dpid, hop=1, delay=0, is_host=True)

            for link in links:
                # delete ports linked switches or hosts
                self.port_info[link.src.dpid].discard(link.src.port_no)
                self.port_info[link.dst.dpid].discard(link.dst.port_no)

                # s1 -> s2: s1.port, s2 -> s1: s2.port
                self.port_link[(link.src.dpid, link.src.port_no)] = (link.src.dpid, link.dst.dpid)
                self.port_link[(link.dst.dpid, link.dst.port_no)] = (link.dst.dpid, link.src.dpid)

                self.link_info[(link.src.dpid, link.dst.dpid)] = link.src.port_no
                self.link_info[(link.dst.dpid, link.src.dpid)] = link.dst.port_no

                # Calculate link delay
                '''
                TODO：
                	计算链路delay
                	将delay存入link_delay_table
                	使用self.logger.info打印delay消息

                '''
                # ========== 补充开始：计算链路 delay ==========
                # 计算链路时延
                delay = self.calculate_link_delay(link.src.dpid, link.dst.dpid)

                # 将 delay 存入 link_delay_table
                self.link_delay_table[(link.src.dpid, link.dst.dpid)] = delay
                self.link_delay_table[(link.dst.dpid, link.src.dpid)] = delay

                # 使用 self.logger.info 打印 delay 消息
                self.logger.info("Link: %s -> %s, delay: %.5fms",
                                 link.src.dpid, link.dst.dpid, delay * 1000)
                # ========== 补充结束 ==========
                self.topo_map.add_edge(link.src.dpid, link.dst.dpid,
                                       hop=1, delay=delay, is_host=False)  # 添加 delay 属性

            if self.weight == 'hop' or self.weight == 'delay':
                self.show_topo_map()
            hub.sleep(GET_TOPOLOGY_INTERVAL)

    def shortest_path(self, src, dst, weight='hop'):
        try:
            paths = list(nx.shortest_simple_paths(self.topo_map, src, dst, weight=weight))
            return paths[0]
        except:
            self.logger.info('host not find/no path')

    def show_topo_map(self):
        self.logger.info('topo map:')
        self.logger.info('{:^10s}  ->  {:^10s}'.format('node', 'node'))
        for src, dst in self.topo_map.edges:
            self.logger.info('{:^10s}      {:^10s}'.format(str(src), str(dst)))
        self.logger.info('\n')

    '''
        what you should do
        - add variables
        - lab3.1
            1. get lldp delay
            2. get echo delay
            3. calculate link delay
            4. get shortest path with networkx

        - lab3.2
            1. handle `EventOFPPortStatus`
            2. delete flow when port down
    '''

    @set_ev_cls(ofp_event.EventOFPPacketIn, MAIN_DISPATCHER)
    def packet_in_handler(self, ev):
        msg = ev.msg
        dpid = msg.datapath.id
        try:
            src_dpid, src_port_no = LLDPPacket.lldp_parse(msg.data)

            if not self.switches:
                # get switches
                self.switches = lookup_service_brick('switches')

            # get lldp_delay
            for port in self.switches.ports.keys():
                if src_dpid == port.dpid and src_port_no == port.port_no:
                    self.lldp_delay_table[(src_dpid, dpid)] = self.switches.ports[port].delay
        except:
            return


    def send_echo_request(self, switch):
        datapath = switch.dp
        parser = datapath.ofproto_parser

        # 构造 Echo 请求数据（包含时间戳）
        timestamp = str(time.time()).encode('utf-8')

        # 构造 OFPEchoRequest 消息
        req = parser.OFPEchoRequest(datapath, data=timestamp)
        datapath.send_msg(req)

        # 记录发送时间
        self.echo_send_timestamp[switch.dp.id] = time.time()

    # 将 echo_reply_handler 与事件 EventOFPEchoReply 进行绑定
    @set_ev_cls(ofp_event.EventOFPEchoReply, MAIN_DISPATCHER)
    def handle_echo_reply(self, ev):
        try:
            msg = ev.msg# 获取 Echo Reply 消息
            # 解析所属的交换机的 dpid
            datapath = msg.datapath
            dpid = datapath.id

            # 记录接收时间
            recv_time = time.time()

            # 获取发送时间
            if dpid in self.echo_send_timestamp:
                send_time = self.echo_send_timestamp[dpid]
                # 计算 Echo RTT
                echo_delay = recv_time - send_time
                self.echo_RTT_table[dpid] = echo_delay# 将结果存入 echo_RTT_table[dpid]

                self.logger.debug("Echo RTT for switch %s: %.5fms", dpid, echo_delay * 1000)

            # 取出 data, 并 decode data 获取原始数据 (可选)
            if msg.data:
                try:
                    original_data = msg.data.decode('utf-8')
                    self.logger.debug("Echo reply data: %s", original_data)
                except:
                    pass

        except Exception as e:
            self.logger.warning("Failed to handle echo reply: %s", str(e))


    def examine_echo_RTT(self):
        while True:
            try:
                # 获取所有的 switch
                switches = get_all_switch(self)

                # 对每个 switch 的 echo RTT 进行测量
                for switch in switches:
                    self.send_echo_request(switch)

                # 协程睡眠使用 hub.sleep，以减小对测量的影响
                hub.sleep(SEND_ECHO_REQUEST_INTERVAL)

            except Exception as e:
                self.logger.warning("Error in examine_echo_RTT: %s", str(e))
                hub.sleep(SEND_ECHO_REQUEST_INTERVAL)



    def calculate_link_delay(self, src_dpid, dst_dpid):
        """
        计算链路单向时延
        公式: delay = max((T_lldp12 + T_lldp21 - T_echo1 - T_echo2) / 2, 0)
        """
        try:
            # 获取 LLDP 延迟（单位：秒）
            # 使用 .get(key, default) 方法，键不存在时返回默认值 0
            T_lldp12 = self.lldp_delay_table.get((src_dpid, dst_dpid), 0)
            T_lldp21 = self.lldp_delay_table.get((dst_dpid, src_dpid), 0)

            # 获取 Echo RTT（单位：秒）
            T_echo1 = self.echo_RTT_table.get(src_dpid, 0)
            T_echo2 = self.echo_RTT_table.get(dst_dpid, 0)

            # 计算链路时延
            delay = max((T_lldp12 + T_lldp21 - T_echo1 - T_echo2) / 2, 0)

            self.logger.debug("Link %s->%s: T_lldp12=%.5f, T_lldp21=%.5f, T_echo1=%.5f, T_echo2=%.5f, delay=%.5f",src_dpid, dst_dpid, T_lldp12, T_lldp21, T_echo1, T_echo2, delay)

            return delay

        except Exception as e:
            self.logger.warning("Error calculating link delay between %s and %s: %s",src_dpid, dst_dpid, str(e))
            return 0
