 $Id: README.txt,v 1.55 2017/10/12 13:29:08 dbelchev $
 $Copyright: (c) 2021 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$

 
 ~ Name:
+-------------------------------------+
|    PHY LoopBack on BlackHawk PHY    |
+-------------------------------------+
 ~ Description:
    An example of closing a PHY loopback over a link from a BH PHY core.
    The link might be from a PHY with swapped lanes.

 ~ File Reference:
    cint_phy_loopback_on_lane_swapped_port.c 

 ~ Name:
+---------------------+
|    Port resource    |
+---------------------+
 ~ Description:
    Examples of setting port resource variables - speed, link training, FEC, etc.

 ~ File Reference:
     cint_port_resource.c
     cint_port_resource_multi.c

 ~ Name:
+------------------------+
|    Fabric Multicast    |
+------------------------+
 ~ Description:
    Examples of setting fabric + egress multicast.

 ~ File Reference:
     cint_fabric_egress_mc.c
     
 ~ Name:
+------------------------+
|    Interrupt call back |
+------------------------+
 ~ Description:
    Examples of setting customer call back function at interrupt handle.

 ~ File Reference:
     cint_dnxc_interrupt.c
