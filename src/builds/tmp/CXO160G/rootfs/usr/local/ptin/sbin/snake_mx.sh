if [ $# -lt 3 ]; then
  echo "Syntax: $0 <linecard_base_port> <uplink_port1> <uplink_port2>"
  exit 0;
fi

lc_base_port=$1
uplink_port1=$2
uplink_port2=$3

vlan=100

let "lc_port1=lc_base_port+0"
let "lc_port2=lc_base_port+1"
let "lc_port3=lc_base_port+2"
let "lc_port4=lc_base_port+3"

/usr/local/ptin/sbin/fp.cli m 1016 intf=0/$lc_port1 etype=0
/usr/local/ptin/sbin/fp.cli m 1016 intf=0/$lc_port2 etype=0
/usr/local/ptin/sbin/fp.cli m 1016 intf=0/$lc_port3 etype=0
/usr/local/ptin/sbin/fp.cli m 1016 intf=0/$lc_port4 etype=0

/usr/local/ptin/sbin/fp.cli m 1602 101
/usr/local/ptin/sbin/fp.cli m 1602 102
/usr/local/ptin/sbin/fp.cli m 1602 103

/usr/local/ptin/sbin/fp.cli m 1601 101 0 0 0 0h 2 0/$uplink_port1/0/$vlan 0/$lc_port1/1/$vlan
/usr/local/ptin/sbin/fp.cli m 1601 102 0 0 0 0h 2 0/$lc_port2/0/$vlan 0/$lc_port3/1/$vlan
/usr/local/ptin/sbin/fp.cli m 1601 103 0 0 0 0h 2 0/$lc_port4/0/$vlan 0/$uplink_port2/1/$vlan

echo "Matrix configured!"

