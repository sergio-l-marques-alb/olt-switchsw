BIN_PATH=/usr/local/ptin/sbin        

#mDNS IPv4
eid=0x400
$BIN_PATH/fp.shell fp entry create 1 $eid
$BIN_PATH/fp.shell fp qual $eid InPorts 0x0112fa01fffe 0xffffffffffff
$BIN_PATH/fp.shell fp qual $eid DstMac 0x01005e0000fb 0xffffffffffff
$BIN_PATH/fp.shell fp qual $eid DstIp 0xe00000fb 0xffffffff
$BIN_PATH/fp.shell fp qual $eid L4DstPort 5353 0xffff
$BIN_PATH/fp.shell fp qual $eid EtherType 0x0800 0xffff
$BIN_PATH/fp.shell fp action add $eid GpDropCancel
$BIN_PATH/fp.shell fp action add $eid YpDropCancel
$BIN_PATH/fp.shell fp entry prio $eid 0x10000001
$BIN_PATH/fp.shell fp entry install $eid

#SSDP IPv4
eid=0x402
$BIN_PATH/fp.shell fp entry create 1 $eid
$BIN_PATH/fp.shell fp qual $eid InPorts 0x0112fa01fffe 0xffffffffffff
$BIN_PATH/fp.shell fp qual $eid DstMac 0x01005e7ffffa 0xffffffffffff
$BIN_PATH/fp.shell fp qual $eid DstIp 0xeffffffa 0xffffffff
$BIN_PATH/fp.shell fp qual $eid L4DstPort 1900 0xffff
$BIN_PATH/fp.shell fp qual $eid EtherType 0x0800 0xffff
$BIN_PATH/fp.shell fp action add $eid GpDropCancel
$BIN_PATH/fp.shell fp action add $eid YpDropCancel
$BIN_PATH/fp.shell fp entry prio $eid 0x10000001
$BIN_PATH/fp.shell fp entry install $eid

#mDNS IPv6
eid=0x401
$BIN_PATH/fp.shell fp entry create 1 $eid
$BIN_PATH/fp.shell fp qual $eid InPorts 0x0112fa01fffe 0xffffffffffff
$BIN_PATH/fp.shell fp qual $eid DstMac 0x3333000000fb 0xffffffffffff
#$BIN_PATH/fp.shell fp qual $eid DstIp6 0xff0200000000000000000000000000fb 0xffffffffffffffffffffffffffffffff
$BIN_PATH/fp.shell fp qual $eid L4DstPort 5353 0xffff
$BIN_PATH/fp.shell fp qual $eid EtherType 0x86dd 0xffff
$BIN_PATH/fp.shell fp action add $eid GpDropCancel
$BIN_PATH/fp.shell fp action add $eid YpDropCancel
$BIN_PATH/fp.shell fp entry prio $eid 0x10000001
$BIN_PATH/fp.shell fp entry install $eid

#SSDP IPv6
eid=0x403
$BIN_PATH/fp.shell fp entry create 1 $eid
$BIN_PATH/fp.shell fp qual $eid InPorts 0x0112fa01fffe 0xffffffffffff
$BIN_PATH/fp.shell fp qual $eid DstMac 0x33330000000c 0xffffffffffff
#$BIN_PATH/fp.shell fp qual $eid DstIp6 0xff00000000000000000000000000000c 0xfff0ffffffffffffffffffffffffffff
$BIN_PATH/fp.shell fp qual $eid L4DstPort 1900 0xffff
$BIN_PATH/fp.shell fp qual $eid EtherType 0x86dd 0xffff
$BIN_PATH/fp.shell fp action add $eid GpDropCancel
$BIN_PATH/fp.shell fp action add $eid YpDropCancel
$BIN_PATH/fp.shell fp entry prio $eid 0x10000001
$BIN_PATH/fp.shell fp entry install $eid

