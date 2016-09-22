vlan=100
evc_base=100

if [ $# -lt 3 ]; then
  echo "Syntax: $0 <uplink_port1> <uplink_port2> <lc_base_port> [lc_slots]"
  exit 0;
fi

uplink_port1=$1
uplink_port2=$2
lc_base_port=$3
lc_slots=1
if [ $# -ge 4 ]; then
  lc_slots=$4
fi

i=$lc_base_port
let "last_port=lc_base_port+(4*lc_slots)-1"

while [ $i -le $last_port ]; do
  echo "Configuring port $i as promiscuous..."
  /usr/local/ptin/sbin/fp.cli m 1016 intf=0/$i etype=0
  let "i=i+1"
done

evc=$evc_base
i=$lc_base_port
echo "Creating EVC $evc..."
/usr/local/ptin/sbin/fp.cli m 1602 $evc
/usr/local/ptin/sbin/fp.cli m 1601 $evc 0 0 0 0h 2 0/$uplink_port1/0/$vlan 0/$i/1/$vlan

let "evc=evc+1"
let "i=i+1"
while [ $i -lt $last_port ]; do
  let "i_next=i+1"
  echo "Creating EVC $evc..."
  /usr/local/ptin/sbin/fp.cli m 1602 $evc
  /usr/local/ptin/sbin/fp.cli m 1601 $evc 0 0 0 0h 2 0/$i/0/$vlan 0/$i_next/1/$vlan
  let "i=i+2"
  let "evc=evc+1"
done

echo "Creating EVC $evc..."
/usr/local/ptin/sbin/fp.cli m 1601 $evc 0 0 0 0h 2 0/$last_port/0/$vlan 0/$uplink_port2/0/$vlan

echo "Matrix configured!"

