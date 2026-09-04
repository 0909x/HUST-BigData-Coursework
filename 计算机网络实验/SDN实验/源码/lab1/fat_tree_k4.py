#!/usr/bin/env python
# fat_tree_final.py

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import OVSSwitch
from mininet.cli import CLI
from mininet.log import setLogLevel, info
import time

class FatTree(Topo):
    def build(self):
        k=4
        # 核心交换机 (4个)
        c1, c2, c3, c4 = [self.addSwitch('c%d' % i) for i in range(1,5)]
        
        # 4个pods
        for pod in range(1,k+1):
            # 每个pod有2个聚合交换机和2个边缘交换机
            a1 = self.addSwitch('a%d1' % pod)
            a2 = self.addSwitch('a%d2' % pod)
            e1 = self.addSwitch('e%d1' % pod)
            e2 = self.addSwitch('e%d2' % pod)
            
            # 连接聚合和边缘
            self.addLink(a1, e1)
            self.addLink(a1, e2)
            self.addLink(a2, e1)
            self.addLink(a2, e2)
            
            # 为边缘交换机添加主机（使用/8子网）
            host_base = (pod-1)*4# 计算主机起始编号
            # 边缘交换机e1连接2台主机
            self.addLink(e1, self.addHost('h%d' % (host_base+1), ip='10.%d.1.1/8' % pod))
            self.addLink(e1, self.addHost('h%d' % (host_base+2), ip='10.%d.1.2/8' % pod))
            # 边缘交换机e2连接2台主机
            self.addLink(e2, self.addHost('h%d' % (host_base+3), ip='10.%d.2.1/8' % pod))
            self.addLink(e2, self.addHost('h%d' % (host_base+4), ip='10.%d.2.2/8' % pod))
            
            # 连接聚合和核心
            self.addLink(a1, c1)
            self.addLink(a1, c2)
            self.addLink(a2, c3) 
            self.addLink(a2, c4)

def setup_network(net):
    """配置网络"""
    info("*** Configuring switches...\n")
    
    # 配置交换机
    for switch in net.switches:
        # 设置为standalone模式
        switch.cmd('ovs-vsctl set-fail-mode', switch, 'standalone')
        # 启用STP防止环路
        switch.cmd('ovs-vsctl set Bridge', switch, 'stp_enable=true')
        # 设置协议
        switch.cmd('ovs-vsctl set Bridge', switch, 'protocols=OpenFlow13')
        info(f"*** Configured {switch.name}\n")
    
    info("*** Waiting for STP convergence (30 seconds)...\n")
    time.sleep(30)
    
    # 显示STP状态
    info("*** STP Status:\n")
    for switch in ['c1', 'a11', 'e11']:
        sw = net.get(switch)
        status = sw.cmd('ovs-vsctl get Bridge', sw, 'stp_status')
        info(f"{switch}: {status}")



if __name__ == '__main__':
    setLogLevel('info')
    
    # 创建拓扑
    topo = FatTree()
    
    # 创建网络
    net = Mininet(topo=topo, switch=OVSSwitch, controller=None)
    
    # 启动网络
    net.start()
    
    print("=== Fat Tree k=4 Topology ===")
    print("Hosts:", [h.name for h in net.hosts])
    print("Switches:", [s.name for s in net.switches])
    
    # 配置网络
    setup_network(net)
    
    print("\n" + "="*50)
    print("ANALYSIS: Without a controller, switches need time to learn MAC addresses")
    print("via flooding. The connectivity should improve over time as MAC tables")
    print("are populated through the learning process.")
    print("="*50)
    
    print("\n*** Entering CLI for manual testing")
    print("*** Useful commands:")
    print("  h1 arp -a                          # View ARP table")
    print("  h1 ping -c 3 h2                    # Test connectivity") 
    print("  sh ovs-appctl fdb/show e11         # View MAC table")
    print("  pingall                            # Test all hosts")
    print("  nodes                              # List all nodes")
    print("  net                                # Show network")
    
    CLI(net)
    net.stop()
