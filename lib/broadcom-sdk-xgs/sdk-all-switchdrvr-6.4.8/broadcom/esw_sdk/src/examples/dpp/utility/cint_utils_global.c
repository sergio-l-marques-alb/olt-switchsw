/* $Id: cint_utils_global.c,v 1.10 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
*/


/* ********* 
  Globals/Aux Variables
 ********** */

/* debug prints */
int verbose = 1;

/* These defines used for in/out port definitions*/
int IN_PORT=0;
int OUT_PORT=1;
int NUMBER_OF_PORTS=2;

/* General port defines*/
int PORT_1 = 0;
int PORT_2 = 1;
int PORT_3 = 2;
int PORT_4 = 3;

/* General RIF defines */
int RIF_1 = 0;
int RIF_2 = 1;
int RIF_3 = 2;
int RIF_4 = 3;

/* General host defines*/
int HOST_1 = 0;
int HOST_2 = 1;
int HOST_3 = 2;
int HOST_4 = 3;
int HOST_5 = 4;

int PETRA     = 0x88640;
int ARAD      = 0x88650;
int ARAD_PLUS = 0x88660;
int ARDON     = 0x88202;
int JERICHO   = 0x88675;
int QUMRAN_MX = 0x88375;

/* General device list - devices must be chronologically ordered
   New devices should be added at the end */
int DEVICE_LIST_SIZE = 10;
int DEVICE_LIST [DEVICE_LIST_SIZE] = {0};
DEVICE_LIST[0] = PETRA;
DEVICE_LIST[1] = ARAD;
DEVICE_LIST[2] = ARAD_PLUS;
DEVICE_LIST[3] = ARDON;
DEVICE_LIST[4] = JERICHO;
DEVICE_LIST[5] = QUMRAN_MX;

/* port_tpid */
struct port_tpid_info_s {
    int port;
    int outer_tpids[2];
    int nof_outers; /*0, 1, 2*/
    int inner_tpids[2];
    int nof_inners; /*0, 1 */
    int discard_frames; /* BCM_PORT_DISCARD_NONE/ALL/UNTAG/TAG */
    uint32 tpid_class_flags; /* flags for bcm_port_tpid_class_set */
    int vlan_transation_profile_id; /* Default Ingress action command in case of no match in VTT L2 LIF lookup */
};

port_tpid_info_s port_tpid_info1;

/* Global declaration, since many cints use it and some redeclare it, or don't declare it at all, which causes errors. */
int advanced_vlan_translation_mode = 0;

/* indicate if the current device is equal to or newer than the one provided
   Get unit - unit id
   Get device - first supported device
   returns 1 if current device is equal or newer than the one provided */
int is_device_or_above(int unit, int supported_device_id) {
    bcm_info_t info;
	int i;
	int current_device_id;
	int current_device_index;
	int supported_device_index = DEVICE_LIST_SIZE;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
  	  printf("Error in bcm_info_get\n");
  	  print rv;
  	  return rv;
    }

	current_device_id = info.device;
	for (i=0;i<DEVICE_LIST_SIZE;i++) {
		if ((DEVICE_LIST[i] & 0xffff) == current_device_id) {
			current_device_index = i;
		}
		if (DEVICE_LIST[i] == supported_device_id) {
			supported_device_index = i;
		}
	}

	return current_device_index >= supported_device_index;
}

/* indicate if the device is arad
   Get unit - unit id
   yesno  -  indicate if the device is arad */ 
int is_arad_only(int unit, int *yesno) {
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
  	  printf("Error in bcm_info_get\n");
  	  print rv;
  	  return rv;
    }

    *yesno = info.device == 0x8650 ? 1 : 0;

    return rv;
}

/* indicate if the device is arad a0
   Get unit - unit id
   yesno  -  indicate if the device is arad a0 */ 
int is_arad_a0_only(int unit, int *yesno) {
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
  	  printf("Error in bcm_info_get\n");
  	  print rv;
  	  return rv;
    }

    *yesno = info.device == 0x8650 &
			 info.revision == 0x0 ? 1 : 0;

    return rv;
}

/* indicate if the device is arad b0
   Get unit - unit id
   yesno  -  indicate if the device is arad b0 */ 
int is_arad_b0_only(int unit, int *yesno) {
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
  	  printf("Error in bcm_info_get\n");
  	  print rv;
  	  return rv;
    }

    *yesno = info.device == 0x8650 &
			 info.revision == 0x11 ? 1 : 0;

    return rv;
}

/* indicate if the device is arad b1
   Get unit - unit id
   yesno  -  indicate if the device is arad b1 */ 
int is_arad_b1_only(int unit, int *yesno) {
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
  	  printf("Error in bcm_info_get\n");
  	  print rv;
  	  return rv;
    }

    *yesno = info.device == 0x8650 &
			 info.revision == 0x12 ? 1 : 0;

    return rv;
}

/* indicate if the device is arad plus
   Get unit - unit id
   yesno  -  indicate if the device is arad plus */ 
int is_arad_plus_only(int unit, int *yesno) {
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
  	  printf("Error in bcm_info_get\n");
  	  print rv;
  	  return rv;
    }

    *yesno = info.device == 0x8660 ? 1 : 0;

    return rv;
}

/* indicate if the device is jericho
   Get unit - unit id
   yesno  -  indicate if the device is jericho */ 
int is_jericho_only(int unit, int *yesno) {
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
  	  printf("Error in bcm_info_get\n");
  	  print rv;
  	  return rv;
    }

    *yesno = info.device == 0x8675 ? 1 : 0;

    return rv;
}

/* indicate if the device is qumran_mx
   Get unit - unit id
   yesno  -  indicate if the device is qumran_mx */ 
int is_qumran_mx_only(int unit, int *yesno) {
    bcm_info_t info;

 
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
  	  printf("Error in bcm_info_get\n");
  	  print rv;
  	  return rv;
    }

    *yesno = info.device == 0x8375 ? 1 : 0;

    return rv;
}

/* port_tpid */

/* 
 *  nof_outers = 1,2
 *  nof_inners = 0,1
 *  nof_outers >= nof_inners
 *  nof_outers + nof_inners = 2
 */ 
void
port_tpid_init(int port, int nof_outers, int nof_inners) {
    port_tpid_info1.port = port;
    port_tpid_info1.outer_tpids[0] = 0x8100;
    port_tpid_info1.outer_tpids[1] = 0x88a0;
    port_tpid_info1.inner_tpids[0] = 0x9100;
    port_tpid_info1.nof_outers = nof_outers;
    port_tpid_info1.nof_inners = nof_inners;
    port_tpid_info1.discard_frames = BCM_PORT_DISCARD_NONE;
    port_tpid_info1.tpid_class_flags = 0;
    port_tpid_info1.vlan_transation_profile_id = 0;
}

int
port_tpid_set(int unit) {
    return _port_tpid_set(unit, 0);
}

int
port_lif_tpid_set(int unit) {
    return _port_tpid_set(unit, 1);
}

int
_port_tpid_set(int unit, int is_lif) {
    int rv;
    int indx;
    int color_set=0;/* has to be zero */

    advanced_vlan_translation_mode = soc_property_get(unit, "bcm886xx_vlan_translate_mode",0);

    if (advanced_vlan_translation_mode) {
        if (port_tpid_info1.nof_outers == 1 && port_tpid_info1.nof_inners == 1) {
            return port_dt_tpid_set(unit, port_tpid_info1.port, port_tpid_info1.outer_tpids[0], port_tpid_info1.inner_tpids[0], is_lif, port_tpid_info1.vlan_transation_profile_id);
        } else if (port_tpid_info1.nof_outers == 1 && port_tpid_info1.nof_inners == 0) {
            return port_outer_tpid_set(unit, port_tpid_info1.port, port_tpid_info1.outer_tpids[0], is_lif, port_tpid_info1.vlan_transation_profile_id);
        }
    }
    
    /* assume port_tpid_init was called with legal params*/
    
    /* remove old tpids on port */
    rv = bcm_port_tpid_delete_all(unit,port_tpid_info1.port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_delete_all, port=%d, \n", port_tpid_info1.port);
        return rv;
    }
    
    /* set outer tpids of the port */
    for(indx = 0; indx < port_tpid_info1.nof_outers; ++indx){
        rv = bcm_port_tpid_add(unit, port_tpid_info1.port, port_tpid_info1.outer_tpids[indx], color_set);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_add, tpid=%d, \n", port_tpid_info1.outer_tpids[indx]);
            return rv;
        }
    }
    
    /* set inner tpids of the port */
    for(indx = 0; indx < port_tpid_info1.nof_inners; ++indx){
        rv = bcm_port_inner_tpid_set(unit, port_tpid_info1.port, port_tpid_info1.inner_tpids[indx]);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_inner_tpid_set, tpid=%d, \n", port_tpid_info1.inner_tpids[indx]);
            return rv;
        }
    }
    
    /* Only relevant for physical port */    
    if(!is_lif) {    
        /* set discard frames on port */
        rv = bcm_port_discard_set(unit, port_tpid_info1.port, port_tpid_info1.discard_frames);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_discard_set, discard-type=%d, \n", port_tpid_info1.discard_frames);
            return rv;
        }
    }
    
    return rv;
}
