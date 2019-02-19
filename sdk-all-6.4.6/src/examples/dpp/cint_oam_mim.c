/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~OAM test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 *
 * $Id: cint_oam.c,v 1.12 Broadcom SDK $
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
 * 
 * File: cint_oam.c
 * Purpose: Example of using OAM APIs.
 *
 * Usage:
 * 
 * cint cint_mim_mp.c 
 * cint cint_mim_p2p.c 
 * cint cint_oam_mim.c
 * cint 
 * print oam_run_with_defaults (0, 13, 14, 15);
 * 
 * 
 * The following MAs:
 * 		2) MA with id 1 & short name format:    1-const; 3-short format; 2-length; all the rest - MA name
 *        short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xcd, 0xab}
 * 
 * The following MEPs:
 *  	2) MEP with id 4096: accellerated,     mac address {0x00, 0x00, 0x00, 0xff, 0xff, 0xff}, in group 2, mdlevel 5, upmep
 * 
 * comments:
 * 1) In order to prevent from OAMP send packets do: BCM.0> m OAMP_MODE_REGISTER TRANSMIT_ENABLE=0
 * 2) MIM OAM enabeld by uses soc property custom_feature_oam_mim=1
 * 
 */

/* Globals - MAC addresses , ports & gports*/
  bcm_mac_t mac_mep_2 = {0x00, 0x00, 0x00, 0xff, 0xff, 0xff};
  bcm_mac_t src_mac_mep_2 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
  bcm_gport_t gport1, gport2, gport3; /* these are the ports created by the vswitch*/
  uint32 port_1 = 13; /* physical port (signal generator)*/
  uint32 port_2 = 14;
  uint32 port_3 = 15;
  bcm_oam_group_info_t group_info_short_ma;
  bcm_oam_endpoint_info_t mep_non_acc_info;

int oam_example(int unit) {
  bcm_error_t rv;
  bcm_oam_endpoint_info_t mep_not_acc_info;

  int md_level_2 = 5;
  int lm_counter_base_id_2  = 6;
  /*1-const; 3-short format; 2-length; all the rest - MA name*/
  uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};

  rv = p2p_main(unit,port_1,port_2); 
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  gport2 = mim_vswitch_cross_connect_gports.port2;

  printf("bcm_oam_init\n");
  rv = bcm_oam_init(unit);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  bcm_oam_group_info_t_init(&group_info_short_ma);
  sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
  rv = bcm_oam_group_create(unit, &group_info_short_ma);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  /*
  * Adding non acc MEP - upmep
  */

  bcm_oam_endpoint_info_t_init(&mep_non_acc_info);

  mep_non_acc_info.type = bcmOAMEndpointTypeEthernet;
  mep_non_acc_info.group = group_info_short_ma.id;
  mep_non_acc_info.level = md_level_2;
 
  mep_non_acc_info.flags |= BCM_OAM_ENDPOINT_UP_FACING;

  /*RX*/
  mep_non_acc_info.gport = gport2;
  sal_memcpy(mep_non_acc_info.dst_mac_address, mac_mep_2, 6);
  mep_non_acc_info.lm_counter_base_id = lm_counter_base_id_2;

  printf("bcm_oam_endpoint_create mep_non_acc_info\n"); 
  rv = bcm_oam_endpoint_create(unit, &mep_non_acc_info);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  printf("created MEP with id %d\n", mep_non_acc_info.id);


  return rv;
}

int oam_run_with_defaults (int unit, int port1, int port2, int port3) {
  bcm_error_t rv;

  port_1 = port1;
  port_2 = port2;
  port_3 = port3;
  
  rv = oam_example(unit);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }
  return rv;
}
