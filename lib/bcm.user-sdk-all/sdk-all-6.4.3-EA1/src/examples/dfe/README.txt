 $Id: README.txt,v 1.55 Broadcom SDK $
 $Copyright: Copyright 2012 Broadcom Corporation.
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
                      
+------------------------+                                                     
|     Link Monitoring    |                                                          
+------------------------+                                                        
 ~ Description:                                                                
    Example application to monitor the state of the links and to             
    react to ports/links failures.                                            
                                *  1. set topology for local module
 *  2. set topology for faps group                                         
 ~ File Reference:                                                              
    cint_linkscan.c                                                           
                                                                              
+------------------------+                                                        
|    Snake Application   |                                                         
+------------------------+                                                       
 ~ Description:
     The test is a stress test on the BCM88750 links. The test validates the links performance under a burst
     of cells.                                                                                                   
                                                                              
 ~ File Reference:                                                              
    cint_snake_test.c                                                              

+------------------------+
|    Topology            |
+------------------------+
 ~ Description:  
     Examples of topology configuration:
     Topology for local module.
     Topology for faps group.
 ~ File Reference:
     cint_topology.c	

+------------------------+                                                        
|    Multicast           |                                                         
+------------------------+                                                         
 ~ Description:  
     The cint demonstrate two examples of multicast applications: direct and indirect
     Direct: set multicast table.
     Indirect: set multicast table and create a static topology. 
                  
 ~ File Reference:
    cint_multicast_test.c                  

+------------------------+
|    Warm boot		 |
+------------------------+
 ~ Description:
     Test of BCM88750 warm reboot.

 ~ File Reference: 
     cint_warmboot.c


+------------------------+
|    Flow Control        |
+------------------------+
 ~ Description:
      This example demonstrate the fifo threshold sequence and test it.

 ~ File Reference:
      cint_fifos.c


+-----------------------------+
|    Flow Control FE3200      |
+-----------------------------+
 ~ Description:
      This example demonstrate the fifo threshold pipe_set/get api and test it for fe3200.

 ~ File Reference:
      cint_flow_control_fe3200.c
      
+-----------------------------+
|    Thresholds FE3200      |
+-----------------------------+
 ~ Description:
      This example demonstrate the configuration of the default fe1600 thresholds to fe3200.

 ~ File Reference:
      cint_thresholds_fe3200.c

+-----------------------------+
|    FE3200 Interop           |
+-----------------------------+
 ~ Description:
      This example demonstrate the configuration required when connecting FE3200 to legacy device.

 ~ File Reference:
      cint_fe3200_interop.c