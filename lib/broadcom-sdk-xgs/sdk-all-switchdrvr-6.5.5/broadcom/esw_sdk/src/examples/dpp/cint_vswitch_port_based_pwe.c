/* $Id: cint_vswitch_port_based_pwe.c,v 1.00 2016/05/22 12:19:51 Jingli Guo $
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File: cint_vswitch_port_based_pwe.c
 * Purpose: Example of creating a PWE which is identified by port+mpls_label.
 *
 * Attachment circuit (AC): Ethernet port attached to the service based on port-vlan-vlan, 
 *                          connect from access side. 
 * PWE: Virtual circuit attached to the service based on VC-label. Connect to the MPLS 
 *      network side.
 * 
 *   configuration tips:
 *   1. add soc property:
 *       pwe_termination_port_mode_enable =1
 *   2. enable the feature for relevant pp_port
 *   bcm_port_control_set(unit,pp_port,bcmPortControlPWETerminationPortModeEnable,1);
 *   3. call bcm_mpls_port_add() with BCM_MPLS_PORT_MATCH_LABEL_PORT flag and valid mpls_port.port parameter
 *
 */ 
int port_based_pwe_run_with_defaults(int unit, int acP, int pweP)
{
   
    port_based_pwe_enable = 1;

   bcm_port_control_set(unit,pweP,bcmPortControlPWETerminationPortModeEnable ,1); 

    vswitch_vpls_run_with_defaults_dvapi(unit,acP,pweP);
   
    return BCM_E_NONE;
}

int port_based_pwe_update_match_label(int unit,int label)
{
    bcm_mpls_port_t  gport;
    int rv = 0;
   
   bcm_mpls_port_t_init(&gport);
   gport.mpls_port_id = network_port_id;

   rv = bcm_mpls_port_get(unit,vswitch_vpls_shared_info_1.vpn,&gport);
   if (rv != BCM_E_NONE) {
        printf("port_based_pwe_update_match_label(), Error, bcm_mpls_port_get\n");
        return rv;
   }

   if (mpls_termination_label_index_enable) {
       BCM_MPLS_INDEXED_LABEL_SET(gport.match_label, label, vswitch_vpls_info_2.access_index_3);
   } else {
       gport.match_label = label;
   }
   gport.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_REPLACE;

   rv = bcm_mpls_port_add(unit,vswitch_vpls_shared_info_1.vpn,&gport);
   if (rv != BCM_E_NONE) {
        printf("port_based_pwe_update_match_label(), Error, bcm_mpls_port_add\n");
        return rv;
   }
   return rv;
}


