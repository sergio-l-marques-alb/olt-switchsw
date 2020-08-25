vlan=100

/usr/local/ptin/sbin/fp.cli m 1602 101                                                       
/usr/local/ptin/sbin/fp.cli m 1602 102

/usr/local/ptin/sbin/fp.cli m 1601 101 0 0 0 0h 2 0/16/0/$vlan 0/17/1/$vlan
/usr/local/ptin/sbin/fp.cli m 1601 102 0 0 0 0h 2 0/18/0/$vlan 0/19/1/$vlan

echo "Linecard configured!"

