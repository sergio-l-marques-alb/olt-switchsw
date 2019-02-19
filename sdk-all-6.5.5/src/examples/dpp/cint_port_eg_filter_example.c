/* $Id: cint_port_eg_filter_example.c,v 1.1.2.1 Broadcom SDK $
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~Traditional Bridge: Port Egress filter~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * File: cint_port_eg_filter_example.c 
 * Purpose: Example of how to set the Port Egress filter for Unknown UC / Unknown MC / BC 
 *  
 * Default settings:
 * It is assumed diag_init is executed:
 *  o    All ports set with TPID 0x8100 and all packet formats (tagged/untagged) are accepted.
 *  o    PVID of all ports set to VID = 1, thus untagged packet is associated to VLAN 1.
 *  o    All VLANs are mapped to Default STG (0).
 *  o    STP state of all ports in default topology is set to forwarding.
 * 
 *  
 * Usage: 
 *  o    port_eg_filter_set(unit,eg_port,disable_types): Set disable filter types on Egress port.
 *  disable_types according to port_control types: BCM_PORT_CONTROL_FILTER_DISABLE_*
 *  o    trap_drop_eg_filter_set(unit,enable): Enable , Disable drop of egress filter type.
 *  
 *  o    trap_drop_eg_filter_destroy(unit): Destroy trap handle
 *  
 * Specific example: 
 * Filter Unknown DA on egress port 14 
 *  port_eg_filter_example(unit,14,0); 
 * Send Packet: 
 * Unknown DA packet : VLAN 1 DA : 1 
 * Expects flooding to ethernet ports except 14 
 *  
 */

int trap_id_handle = -1;

int
port_eg_filter_set(int unit, int egress_port, int disable_types){
    int rv;
    
    rv = bcm_port_control_set(unit,egress_port,bcmPortControlEgressFilterDisable,disable_types);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_control_set, port=%d, \n", egress_port);
        return rv;
    }    
    
    return rv;
}

int
trap_drop_eg_filter_set(int unit, int enable){
    int rv;
    int trap_id;
    bcm_rx_trap_config_t config;

    bcm_rx_trap_t trap_type = bcmRxTrapEgUnknownDa; /* Trap for Unknown DA / Unknown MC / BC */

    bcm_rx_trap_config_t_init(&config);
    config.flags |= (BCM_RX_TRAP_UPDATE_DEST);
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    if (trap_id_handle == -1) {
      rv = bcm_rx_trap_type_create(unit,0x0,trap_type,&trap_id);
      if (rv != BCM_E_NONE) {
          printf("Error, in bcm_rx_trap_type_create\n");
          return rv;
      }    

       trap_id_handle = trap_id; /*Update trap id handle*/
    } else {
       trap_id = trap_id_handle;
    }
   

    rv = bcm_rx_trap_set(unit,trap_id,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    }
    
    return rv;
}

int trap_drop_eg_filter_destroy(int unit)
{
    int rv;
    int trap_id = trap_id_handle;
    bcm_rx_trap_config_t config;

    bcm_rx_trap_t trap_type = bcmRxTrapEgUnknownDa; /* Trap for Unknown DA / Unknown MC / BC */

    if (trap_id != -1) {
        rv = bcm_rx_trap_type_destroy(unit,trap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_rx_trap_type_create\n");
            return rv;
        } 
    }

    return rv;
}

/* An example of filter according to type */
/* type 0 - enable only Unknown DA , 1 - enable only Unknown MC, 2 - enable only BC */
int port_eg_filter_example(int unit, int eg_port, int type)
{
  int rv;
  int value;

  if (type == 0) {
    value = BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC | BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC;
  } else if (type == 1) {
    value = BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC | BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC;
  } else {
    value = BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC | BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC;
  }

  rv = port_eg_filter_set(unit,eg_port,value);
  if (rv != BCM_E_NONE) {
      printf("Error, in port_eg_filter_set\n");
      return rv;
  } 

  rv = trap_drop_eg_filter_set(unit,1);
  if (rv != BCM_E_NONE) {
      printf("Error, in trap_drop_eg_filter_set\n");
      return rv;
  } 

  return rv;
}
