 $Id: README.txt,v 1.55 Broadcom SDK $
 $Copyright: Copyright 2016 Broadcom Corporation.
 This program is the proprietary software of Broadcom Corporation
 and/or its licensors, and may only be used, duplicated, modified
 or distributed pursuant to the terms and conditions of a separate,
 written license agreement executed between you and Broadcom
 (an "Authorized License").  Except as set forth in an Authorized
 License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and
 Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE
 NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 ALL USE OF THE SOFTWARE.  
  
 Except as expressly set forth in the Authorized License,
  
 1.     This program, including its structure, sequence and organization,
 constitutes the valuable trade secrets of Broadcom, and you shall use
 all reasonable efforts to protect the confidentiality thereof,
 and to use this information only in connection with your use of
 Broadcom integrated circuit products.
  
 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 
 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
   
   
 ~ Name:   
+-----------------------+                                                     
|    Link Monitoring    |                                                          
+-----------------------+                                                        
 ~ Description:                                                                
    An example application to monitor the state of the links and to react to port/link failures.                                            
	1. Setting topology for local module.
	2. Setting topology for FAP groups.  
	
 ~ File Reference:                                                              
    cint_linkscan.c                                                           
     
	 
 ~ Name:	 
+-------------------------+                                                        
|    Snake Application    |                                                         
+-------------------------+                                                       
 ~ Description:
	The test is a stress test on the BCM88750 (FE1600) links. The test validates the links performance under a burst of cells.                                                                                                   
                                                                              
 ~ File Reference:                                                              
    cint_snake_test.c                                                              

	
 ~ Name:   
+----------------+
|    Topology    |
+----------------+
 ~ Description:  
	Examples of topology configuration.
	 - Topology for local module
	 - Topology for FAP groups
	 
 ~ File Reference:
	cint_topology.c	

	 
 ~ Name:   
+-----------------+                                                        
|    Multicast    |                                                       
+-----------------+                                                         
 ~ Description:  
	The cint demonstrate two examples of multicast applications, direct and indirect.
	 - Direct: Set multicast table
	 - Indirect: Set multicast table and create a static topology
                  
 ~ File Reference:
	cint_multicast_test.c                  

	
 ~ Name:   
+-----------------+
|    Warm Boot    |
+-----------------+
 ~ Description:
	Test for BCM88750 (FE1600) warm reboot.

 ~ File Reference: 
	cint_warmboot.c
 
 
 ~ Name:   
+--------------------+
|    Flow Control    |
+--------------------+
 ~ Description:
	This example demonstrates and tests the FIFO threshold sequence.

 ~ File Reference:
	cint_fifos.c

	  
 ~ Name:   
+---------------------------+
|    Flow Control FE3200    |
+---------------------------+
 ~ Description:
	This example demonstrates and tests the FIFO threshold pipe_set/get API for BCM88770 (FE3600, previously known as FE3200).

 ~ File Reference:
	cint_flow_control_fe3200.c
    
	
 ~ Name:   	
+-------------------------+
|    Thresholds FE3200    |
+-------------------------+
 ~ Description:
	This example demonstrate the configuration of the default BCM88750 (FE1600) thresholds to BCM88770 (FE3600, previously known as FE3200).

 ~ File Reference:
	cint_thresholds_fe3200.c

	  
 ~ Name:   	  
+----------------------+
|    FE3200 Interop    |
+----------------------+
 ~ Description:
	This example demonstrate the configuration required when connecting BCM88770 (FE3600, previously known as FE3200) to legacy devices.

 ~ File Reference:
	cint_fe3200_interop.c


 ~ Name:   	  
+----------+
|    RX    |
+----------+
 ~ Description:
	This example demonstrates a configuration when using the CPU2CPU packets mechanism.

 ~ File Reference:
	cint_dfe_cpu_packets.c