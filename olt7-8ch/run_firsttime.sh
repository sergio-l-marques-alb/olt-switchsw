#!/bin/sh

# Compile kernel
cd kernel/linux-2.6.17.14_olt7-8ch
sh pq2_build.sh
cd -

# Correct SNMP bug
cd RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/output/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH_OLT7-8CH_A
sh correct_snmp_bug.sh
cd -

echo
echo "Switchdrvr is ready to be compiled... goto RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/output/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH_OLT7-8CH_A folder and type 'sh build.sh'"

