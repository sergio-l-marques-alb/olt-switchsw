
/* $Id: cint_field_elk_acl.c $
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
*/

/* 
 * Using the KBP device as external TCAM lookups (ELK), this CINT describes 
 * a configuration examples of an ELK ACL. 
 *  
 * larg Qset 
 * DIP SIP sharing 
 * configure pmf to dropp packet that was hit in the kbp 
 *  
 */


bcm_field_group_t group_id_elk = 100;
bcm_field_group_t group_id_elk_sharing = 101;

int ACL_BASE_GROUP_INDEX = 0;
int PMF_BASE_GROUP_INDEX = 6;
int ACL_BASE_GROUP_PRIORITY = 40;
int PMF_BASE_GROUP_PRIORITY = 40;

int HIT_BIT_0_QUAL = 0;
int HIT_BIT_1_QUAL = 1;
int HIT_BIT_2_QUAL = 2;
int HIT_BIT_3_QUAL = 3;
int HIT_BIT_4_QUAL = 4;
int HIT_BIT_5_QUAL = 5;


/* this function configures in the KBP ACL with key size 160bit.
   this test validates that we are using the HW capabilities*/
int larg_qset(int unit) {
  int rv=0;
  int group_priority_elk = 41;
  bcm_field_group_config_t grp_elk;
  bcm_field_entry_t ent_elk;
  int presel_id_elk;
  
  rv = bcm_field_presel_create(unit, &presel_id_elk);
  if (rv) {
      printf("bcm_field_presel_create error\n");
      return rv;
  }

  rv = bcm_field_qualify_Stage(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageExternal);
  if (rv) {
      printf("bcm_field_qualify_Stage error\n");
      return rv;
  }

  rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeL2);
  if (rv) {
      printf("bcm_field_qualify_AppType error\n");
      return rv;
  }

  rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp4Ucast);
  if (rv) {
      printf("bcm_field_qualify_AppType error\n");
      return rv;
  }

  rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp4UcastRpf);
  if (rv) {
      printf("bcm_field_presel_create error\n");
      return rv;
  }
  
  bcm_field_group_config_t_init(&grp_elk); 
  BCM_FIELD_PRESEL_ADD(grp_elk.preselset, presel_id_elk);
  grp_elk.group = group_id_elk;
  grp_elk.priority = group_priority_elk ;
  grp_elk.mode =  bcmFieldGroupModeAuto;
  grp_elk.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;
  
  BCM_FIELD_QSET_INIT(grp_elk.qset);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyStageExternal);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyVpn);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyIp4);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcIp);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyDstIp);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyL4SrcPort);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyL4DstPort);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyIpProtocol);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcPort);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyIpFrag);
  
  BCM_FIELD_ASET_INIT(grp_elk.aset);
  BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue1Set);


  rv = bcm_field_group_config_create(unit, &grp_elk);
  if (rv) {
      printf("bcm_field_group_config_create error\n");
      return rv;
  }

  /* Initialize the KBP */
  bshell(unit, "kbp init_appl");

  return rv;
}


/* this function configures the KBP with ACL key bigger than 160bit.
   DIP SIP sharing will not work in this case because the AppTypes are not at the same cluster IPv4 and IPv6*/
int dip_sip_sharing_elk_group_create_fail1(int unit) {
  int rv=0;
  int group_priority_elk = 42;
  bcm_field_group_config_t grp_elk;
  bcm_field_entry_t ent_elk;
  int presel_id_elk;
  
  rv = bcm_field_presel_create(unit, &presel_id_elk);
  if (rv) {
      printf("bcm_field_presel_create error\n");
      return rv;
  }

  rv = bcm_field_qualify_Stage(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageExternal);
  if (rv) {
      printf("bcm_field_qualify_Stage error\n");
      return rv;
  }

  rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp6Ucast);
  if (rv) {
      printf("bcm_field_qualify_AppType error\n");
      return rv;
  }

  rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp4Ucast);
  if (rv) {
      printf("bcm_field_qualify_AppType error\n");
      return rv;
  }
  
  bcm_field_group_config_t_init(&grp_elk); 
  BCM_FIELD_PRESEL_ADD(grp_elk.preselset, presel_id_elk);
  grp_elk.group = group_id_elk_sharing;
  grp_elk.priority = group_priority_elk ;
  grp_elk.mode =  bcmFieldGroupModeAuto;
  grp_elk.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;
  
  BCM_FIELD_QSET_INIT(grp_elk.qset);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyStageExternal);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyDstIp6);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcIp6);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcPort);
  
  
  BCM_FIELD_ASET_INIT(grp_elk.aset);
  BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue1Set);


  rv = bcm_field_group_config_create(unit, &grp_elk);
  if (rv == 0) {
      printf("bcm_field_group_config_create error should fail, but passes\n");
      return rv;
  }

  return 0;
}

/* this function configures new ACL in the KBP with key size bigger than 160bit,
   in this case SW will use the DIP SIP sharing so program will be configured */
int ipv6_dip_sip_sharing_elk_group_create(int unit, int hit_bit) {
  int rv=0;
  int group_priority_elk = 42;
  bcm_field_group_config_t grp_elk;
  bcm_field_entry_t ent_elk;
  int presel_id_elk;
  
  rv = bcm_field_presel_create(unit, &presel_id_elk);
  if (rv) {
      printf("bcm_field_presel_create error\n");
      return rv;
  }

  rv = bcm_field_qualify_Stage(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageExternal);
  if (rv) {
      printf("bcm_field_qualify_Stage error\n");
      return rv;
  }

  rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp6Ucast);
  if (rv) {
      printf("bcm_field_qualify_AppType error\n");
      return rv;
  }

  rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp6UcastRpf);
  if (rv) {
      printf("bcm_field_qualify_AppType error\n");
      return rv;
  }

  rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp6Mcast);
  if (rv) {
      printf("bcm_field_qualify_AppType error\n");
      return rv;
  }
  
  bcm_field_group_config_t_init(&grp_elk); 
  BCM_FIELD_PRESEL_ADD(grp_elk.preselset, presel_id_elk);
  grp_elk.group = group_id_elk_sharing;
  grp_elk.priority = group_priority_elk ;
  grp_elk.mode =  bcmFieldGroupModeAuto;
  grp_elk.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;
  
  BCM_FIELD_QSET_INIT(grp_elk.qset);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyStageExternal);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyDstIp6);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcIp6);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcPort);
  
  
  BCM_FIELD_ASET_INIT(grp_elk.aset);
  BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue0Set+hit_bit);


  rv = bcm_field_group_config_create(unit, &grp_elk);
  if (rv) {
      printf("bcm_field_group_config_create error\n");
      return rv;
  }

  return rv;
}


/* add entry to corresponding IPv6 table */
int ipv6_dip_sip_sharing_elk_entry_add(int unit, int hit_bit) {

  int rv=0;
  bcm_ip6_t ip6_mask =  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  bcm_ip6_t ip6_value = {0x1,0x00,0x16,0x00,0x35,0x00,0x70,0x00,0x00,0x00,0xdb,0x7,0x00,0x00,0x00,0x00};
  bcm_field_entry_t ent_elk;


  bcm_field_entry_create(unit, group_id_elk_sharing, &ent_elk);
  bcm_field_qualify_DstIp6(unit, ent_elk, ip6_value, ip6_mask);

  rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue0Set+hit_bit, 0, 0);
  if (rv) {
      printf("bcm_field_action_add error\n");
      return rv;
  }

  rv = bcm_field_entry_prio_set(unit, ent_elk, 10);
  if (rv) {
      printf("bcm_field_entry_prio_set error\n");
      return rv;
  }

  rv = bcm_field_entry_install(unit, ent_elk);
  if (rv) {
      printf("bcm_field_entry_install error\n");
      return rv;
  }

  return rv;
}




int ipv6_dip_sip_sharing_example(int unit, int hit_bit) {
  int rv=0;
  
  rv = ipv6_dip_sip_sharing_elk_group_create(unit,hit_bit);
  if (rv) {
      printf("dip_sip_sharing_elk_group_create error\n");
      return rv;
  }

  /* Initialize the KBP */
  bshell(unit, "kbp init_appl see=1");

  rv = ipv6_dip_sip_sharing_elk_entry_add(unit,hit_bit);
  if (rv) {
      printf("dip_sip_sharing_elk_group_create error\n");
      return rv;
  }

  return rv;
}


/* this function configures new ACL in the KBP with key size bigger than 160bit,
   in this case SW will use the DIP SIP sharing so program will be configured */
int ipv4_dip_sip_sharing_elk_group_create(int unit, int hit_bit) {
  int rv=0;
  int group_priority_elk = 42;
  bcm_field_group_config_t grp_elk;
  bcm_field_entry_t ent_elk;
  int presel_id_elk;
  
  rv = bcm_field_presel_create(unit, &presel_id_elk);
  if (rv) {
      printf("bcm_field_presel_create error\n");
      return rv;
  }

  rv = bcm_field_qualify_Stage(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageExternal);
  if (rv) {
      printf("bcm_field_qualify_Stage error\n");
      return rv;
  }

  rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp4Ucast);
  if (rv) {
      printf("bcm_field_qualify_AppType 4Ucast error\n");
      return rv;
  }

  rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp4UcastRpf);
  if (rv) {
      printf("bcm_field_qualify_AppType 4UcastRpf error\n");
      return rv;
  }

  rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeCompIp4McastRpf);
  if (rv) {
      printf("bcm_field_qualify_AppType 4UcastRpf error\n");
      return rv;
  }
  
  bcm_field_group_config_t_init(&grp_elk); 
  BCM_FIELD_PRESEL_ADD(grp_elk.preselset, presel_id_elk);
  grp_elk.group = group_id_elk_sharing;
  grp_elk.priority = group_priority_elk ;
  grp_elk.mode =  bcmFieldGroupModeAuto;
  grp_elk.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;
  
  BCM_FIELD_QSET_INIT(grp_elk.qset);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyStageExternal);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcIp);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyDstIp);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcMac);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyDstMac);
  BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcPort);
  
  
  BCM_FIELD_ASET_INIT(grp_elk.aset);
  BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue0Set+hit_bit);


  rv = bcm_field_group_config_create(unit, &grp_elk);
  if (rv) {
      printf("bcm_field_group_config_create error\n");
      return rv;
  }

  return rv;
}

/* add entry to corresponding IPv4 table */
int ipv4_dip_sip_sharing_elk_entry_add(int unit, int hit_bit) {

  int rv=0;
  uint32 dst_ip = 0x7fffff03;
  uint32 dst_ip_mask = 0xffffffff;
  bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */    
  bcm_field_entry_t ent_elk;


  bcm_field_entry_create(unit, group_id_elk_sharing, &ent_elk);
  rv = bcm_field_qualify_DstIp(unit, ent_elk, dst_ip, dst_ip_mask);
  if (rv) {
      printf("bcm_field_qualify_DstIp error\n");
      return rv;
  }
  rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue0Set+hit_bit, 0, 0);
  if (rv) {
      printf("bcm_field_action_add error\n");
      return rv;
  }

  rv = bcm_field_entry_prio_set(unit, ent_elk, 10);
  if (rv) {
      printf("bcm_field_entry_prio_set error\n");
      return rv;
  }

  rv = bcm_field_entry_install(unit, ent_elk);
  if (rv) {
      printf("bcm_field_entry_install error\n");
      return rv;
  }


  /* adding entry for multicast */

  bcm_field_entry_create(unit, group_id_elk_sharing, &ent_elk);
  rv = bcm_field_qualify_DstIp(unit, ent_elk, mc_ip, dst_ip_mask);
  if (rv) {
      printf("bcm_field_qualify_DstIp (MC) error\n");
      return rv;
  }

  rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue0Set+hit_bit, 0, 0);
  if (rv) {
      printf("bcm_field_action_add (MC) error\n");
      return rv;
  }

  rv = bcm_field_entry_prio_set(unit, ent_elk, 11);
  if (rv) {
      printf("bcm_field_entry_prio_set (MC) error\n");
      return rv;
  }

  rv = bcm_field_entry_install(unit, ent_elk);
  if (rv) {
      printf("bcm_field_entry_install (MC) error\n");
      return rv;
  }

  return rv;
}




int ipv4_dip_sip_sharing_example(int unit, int hit_bit) {
  int rv=0;
  
  rv = ipv4_dip_sip_sharing_elk_group_create(unit,hit_bit);
  if (rv) {
      printf("dip_sip_sharing_elk_group_create error\n");
      return rv;
  }

  /* Initialize the KBP */
  bshell(unit, "kbp init_appl see=1");

  rv = ipv4_dip_sip_sharing_elk_entry_add(unit,hit_bit);
  if (rv) {
      printf("dip_sip_sharing_elk_group_create error\n");
      return rv;
  }

  return rv;
}


/* this function configures the PMF to drop a packet if there was an hit on ACL-3*/
int dip_sip_sharing_pmf_configure(int unit, int is_ipv4, int hit_bit) {
  int rv=0;
  int group_id=3;
  int group_priority = 40;
  bcm_field_group_config_t grp;
  bcm_field_entry_t ent;
  int presel_id;
  
  rv = bcm_field_presel_create(unit, &presel_id);
  if (rv) {
      printf("bcm_field_presel_create error\n");
      return rv;
  }

  rv = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
  if (rv) {
      printf("bcm_field_qualify_Stage error\n");
      return rv;
  }

  if (is_ipv4) {
      rv = bcm_field_qualify_AppType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp4UcastRpf);
      if (rv) {
          printf("bcm_field_qualify_AppType error\n");
          return rv;
      }

      rv = bcm_field_qualify_AppType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp4Ucast);
      if (rv) {
          printf("bcm_field_qualify_AppType error\n");
          return rv;
      }

      rv = bcm_field_qualify_AppType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeCompIp4McastRpf);
      if (rv) {
          printf("bcm_field_qualify_AppType error\n");
          return rv;
      }
  }else{

      rv = bcm_field_qualify_AppType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp6UcastRpf);
      if (rv) {
          printf("bcm_field_qualify_AppType error\n");
          return rv;
      }

      rv = bcm_field_qualify_AppType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp6Ucast);
      if (rv) {
          printf("bcm_field_qualify_AppType error\n");
          return rv;
      }

      rv = bcm_field_qualify_AppType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp6Mcast);
      if (rv) {
          printf("bcm_field_qualify_AppType error\n");
          return rv;
      }
  }
  switch (hit_bit) {
  case 0:
	  rv = bcm_field_qualify_ExternalHit0(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 0x1, 0x1);
	  if (rv) {
		  printf("bcm_field_qualify_ExternalHit0 error\n");
		  return rv;
	  }
	  break; 
  case 1:
	  rv = bcm_field_qualify_ExternalHit1(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 0x1, 0x1);
	  if (rv) {
		  printf("bcm_field_qualify_ExternalHit1 error\n");
		  return rv;
	  }
	  break; 
  case 2:
	   rv = bcm_field_qualify_ExternalHit2(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 0x1, 0x1);
	   if (rv) {
		   printf("bcm_field_qualify_ExternalHit2 error\n");
		   return rv;
	   }
	   break; 
  case 3:
	  rv = bcm_field_qualify_ExternalHit3(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 0x1, 0x1);
	  if (rv) {
		  printf("bcm_field_qualify_ExternalHit3 error\n");
		  return rv;
	  }
	  break; 
  }
  bcm_field_group_config_t_init(&grp); 
  BCM_FIELD_PRESEL_ADD(grp.preselset, presel_id);
  grp.group = group_id;
  grp.priority = group_priority ;
  grp.mode =  bcmFieldGroupModeAuto;
  grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;
  
  BCM_FIELD_QSET_INIT(grp.qset);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifySrcPort);
  
  BCM_FIELD_ASET_INIT(grp.aset);
  BCM_FIELD_ASET_ADD(grp.aset, bcmFieldActionDrop);


  rv = bcm_field_group_config_create(unit, &grp);
  if (rv) {
      printf("bcm_field_group_config_create error\n");
      return rv;
  }


  bcm_field_entry_create(unit, group_id, &ent);

  rv = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
  if (rv) {
      printf("Error in bcm_field_action_add\n");
      return rv;
  }

  rv = bcm_field_entry_install(unit, ent);
  if (rv) {
      printf("bcm_field_entry_install error\n");
      return rv;
  }

  return rv;
}


int ipv4_dip_sip_sharing_semantic(int unit) {
  int rv=0;
  
  rv = dip_sip_sharing_elk_group_create_fail1(unit);
  if (rv) {
      printf("dip_sip_sharing_elk_group_create error in semantic\n");
      return rv;
  }

  return rv;
}

/*
*   set the pmf to drop packets for a spesific hit bit options (0-3) and a specific application
*/ 
int configure_pmf_action_by_hit_bit_example(int unit, int hit_bit, bcm_field_AppType_t app_type, bcm_field_action_t action_type, int param0, int param1) {

	int rv=0;
	bcm_field_group_config_t grp;
	bcm_field_entry_t ent;
	int presel_id;
	int ent_grp;

	rv = bcm_field_presel_create(unit, &presel_id);
	if (rv) {
	  printf("bcm_field_presel_create error\n");
	  return rv;
	}

	rv = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
	if (rv) {
	  printf("bcm_field_qualify_Stage error\n");
	  return rv;
	}

	rv = bcm_field_qualify_AppType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, app_type);
	if (rv) {
	  printf("bcm_field_qualify_AppType error\n");
	  return rv;
	}
	switch (hit_bit) {
	case HIT_BIT_0_QUAL:
		rv = bcm_field_qualify_ExternalHit0(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 0x1, 0x1);
		if (rv) {
		  printf("bcm_field_qualify_ExternalHit0 error\n");
		  return rv;
		}
		break;
	case HIT_BIT_1_QUAL:
		rv = bcm_field_qualify_ExternalHit1(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 0x1, 0x1);
		if (rv) {
		  printf("bcm_field_qualify_ExternalHit1 error\n");
		  return rv;
		}
		break;
	case HIT_BIT_2_QUAL:
		rv = bcm_field_qualify_ExternalHit2(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 0x1, 0x1);
		if (rv) {
		  printf("bcm_field_qualify_ExternalHit2 error\n");
		  return rv;
		}
		break;
	case HIT_BIT_3_QUAL:
		rv = bcm_field_qualify_ExternalHit3(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 0x1, 0x1);
		if (rv) {
		  printf("bcm_field_qualify_ExternalHit3 error\n");
		  return rv;
		}
		break;
	case HIT_BIT_4_QUAL:
		rv = bcm_field_qualify_ExternalHit4(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 0x1, 0x1);
		if (rv) {
		  printf("bcm_field_qualify_ExternalHit4 error\n");
		  return rv;
		}
		break;
	case HIT_BIT_5_QUAL:
		rv = bcm_field_qualify_ExternalHit5(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 0x1, 0x1);
		if (rv) {
		  printf("bcm_field_qualify_ExternalHit5 error\n");
		  return rv;
		}
		break;
	}

	bcm_field_group_config_t_init(&grp); 
	BCM_FIELD_PRESEL_ADD(grp.preselset, presel_id);
	grp.group    = PMF_BASE_GROUP_INDEX 	+ hit_bit;
	grp.priority = PMF_BASE_GROUP_PRIORITY  + hit_bit;
	grp.mode 	 = bcmFieldGroupModeAuto;
	grp.flags 	 = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;

	BCM_FIELD_QSET_INIT(grp.qset);
	BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
	BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifySrcPort);

	BCM_FIELD_ASET_INIT(grp.aset);
	BCM_FIELD_ASET_ADD(grp.aset, action_type);

	ent_grp = grp.group;
	rv = bcm_field_group_config_create(unit, &grp);
	if (rv) {
	  printf("bcm_field_group_config_create error\n");
	  return rv;
	}

	bcm_field_entry_create(unit, ent_grp, &ent);

	rv = bcm_field_action_add(unit, ent, action_type, param0, param1);
	if (rv) {
	  printf("Error in bcm_field_action_add\n");
	  return rv;
	}

	rv = bcm_field_entry_install(unit, ent);
	if (rv) {
	  printf("bcm_field_entry_install error\n");
	  return rv;
	}

	return rv;
}

/*
*   set the pmf to drop packets for a spesific hit bit options (0-3) and a specific application
*/ 
 int configure_pmf_action_by_hit_value_example(int unit, int hit_bit, uint64 hit_value, uint64 hit_mask, bcm_field_AppType_t app_type1, bcm_field_AppType_t app_type2, bcm_field_action_t action_type, int param0, int param1, int priority) {

	int rv=0;
	bcm_field_group_config_t grp;
	bcm_field_entry_t ent;
	int presel_id;
	int ent_grp;

	rv = bcm_field_presel_create(unit, &presel_id);
	if (rv) {
	  printf("bcm_field_presel_create error\n");
	  return rv;
	}

	rv = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
	if (rv) {
	  printf("bcm_field_qualify_Stage error\n");
	  return rv;
	}

	rv = bcm_field_qualify_AppType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, app_type1);
	if (rv) {
	  printf("bcm_field_qualify_AppType error\n");
	  return rv;
	}
	
	if (app_type2 != 0) {
		rv = bcm_field_qualify_AppType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, app_type2);
		if (rv) {
		  printf("bcm_field_qualify_AppType error\n");
		  return rv;
		}
	}

	bcm_field_group_config_t_init(&grp); 
	BCM_FIELD_PRESEL_ADD(grp.preselset, presel_id);
	grp.group    = PMF_BASE_GROUP_INDEX 	+ hit_bit + priority;
	grp.priority = PMF_BASE_GROUP_PRIORITY  + hit_bit + priority;
	grp.mode 	 = bcmFieldGroupModeAuto;
	grp.flags 	 = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;

	BCM_FIELD_QSET_INIT(grp.qset);
	BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyExternalValue0);
	BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyExternalValue1);
	BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyExternalValue2);
	BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyExternalValue3);
	BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyExternalValue4);
	BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyExternalValue5);


	BCM_FIELD_ASET_INIT(grp.aset);
	BCM_FIELD_ASET_ADD(grp.aset, action_type);

	ent_grp = grp.group;
	rv = bcm_field_group_config_create(unit, &grp);
	if (rv) {
	  printf("bcm_field_group_config_create error\n");
	  return rv;
	}

	rv = bcm_field_entry_create(unit, ent_grp, &ent);
	if (rv) {
	  printf("Error in bcm_field_entry_create\n");
	  return rv;
	}

	switch (hit_bit) {
	case HIT_BIT_0_QUAL:
		rv = bcm_field_qualify_ExternalValue0(unit, ent, hit_value, hit_mask);
		if (rv) {
		  printf("bcm_field_qualify_ExternalValue0 error\n");
		  return rv;
		}
		break;
	case HIT_BIT_1_QUAL:
		rv = bcm_field_qualify_ExternalValue1(unit, ent, hit_value, hit_mask);
		if (rv) {
		  printf("bcm_field_qualify_ExternalValue1 error\n");
		  return rv;
		}
		break;
	case HIT_BIT_2_QUAL:
		rv = bcm_field_qualify_ExternalValue2(unit, ent, hit_value, hit_mask);
		if (rv) {
		  printf("bcm_field_qualify_ExternalValue2 error\n");
		  return rv;
		}
		break;
	case HIT_BIT_3_QUAL:
		rv = bcm_field_qualify_ExternalValue3(unit, ent, hit_value, hit_mask);
		if (rv) {
		  printf("bcm_field_qualify_ExternalValue3 error\n");
		  return rv;
		}
		break;
	case HIT_BIT_4_QUAL:
		rv = bcm_field_qualify_ExternalValue4(unit, ent, hit_value, hit_mask);
		if (rv) {
		  printf("bcm_field_qualify_ExternalValue4 error\n");
		  return rv;
		}
		break;
	case HIT_BIT_5_QUAL:
		rv = bcm_field_qualify_ExternalValue5(unit, ent, hit_value, hit_mask);
		if (rv) {
		  printf("bcm_field_qualify_ExternalValue5 error\n");
		  return rv;
		}
		break;
	}

	rv = bcm_field_action_add(unit, ent, action_type, param0, param1);
	if (rv) {
	  printf("Error in bcm_field_action_add\n");
	  return rv;
	}

	rv = bcm_field_entry_install(unit, ent);
	if (rv) {
	  printf("bcm_field_entry_install error\n");
	  return rv;
	}

	return rv;
}

/*
 * configure ivp4 ACL with 2 lookups: 
 * 1 - vlan  
 * 2 - dmac + vlan 
*/
int frwd_kbp_app_2_acls_kbp_config_example(int unit) {

	int rv=0;

	int presel_id_elk;
	bcm_field_group_config_t grp_elk;

	rv = bcm_field_presel_create(unit, &presel_id_elk);
	if (rv) {
	  printf("bcm_field_presel_create error\n");
	  return rv;
	}

	rv = bcm_field_qualify_Stage(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageExternal);
	if (rv) {
	  printf("bcm_field_qualify_Stage error\n");
	  return rv;
	}

	rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp4Ucast);
	if (rv) {
	  printf("bcm_field_qualify_AppType Ip4Ucast error\n");
	  return rv;
	}

	bcm_field_group_config_t_init(&grp_elk); 
	BCM_FIELD_PRESEL_ADD(grp_elk.preselset, presel_id_elk);
	grp_elk.mode =  bcmFieldGroupModeAuto;
	grp_elk.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;

	BCM_FIELD_QSET_INIT(grp_elk.qset);
	BCM_FIELD_ASET_INIT(grp_elk.aset);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyStageExternal);
	
	/* 1st: vlan only*/
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyOuterVlanId);
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue1Set);
	grp_elk.group 	 = ACL_BASE_GROUP_INDEX+1;
	grp_elk.priority = ACL_BASE_GROUP_PRIORITY;

	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 1 error \n");
	  return rv;
	}

	/* 2nd: vlan + d mac*/
	BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue1Set);
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue3Set);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcMac);
	grp_elk.group 	 = ACL_BASE_GROUP_INDEX+3;
	grp_elk.priority++;

	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 3 error \n");
	  return rv;
	}

	/* Initialize the KBP */
	bshell(unit, "kbp init_appl");

	return rv;
}

/*
*	set configuration of KBP tables and PMF dropping action 
*/
int frwd_kbp_app_2_acls_kbp_set_configurations(int unit) {
	int rv;

	rv = frwd_kbp_app_2_acls_kbp_config_example(unit);
	if (rv) {
		printf("frwd_kbp_app_2_acls_kbp_config_example error\n");
		return  rv;
	}

	rv = configure_pmf_action_by_hit_bit_example(unit,HIT_BIT_1_QUAL,bcmFieldAppTypeIp4Ucast, bcmFieldActionDrop, 0, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_bit_example bit 1 error\n");
		return  rv;
	}

	rv = configure_pmf_action_by_hit_bit_example(unit,HIT_BIT_3_QUAL,bcmFieldAppTypeIp4Ucast, bcmFieldActionDrop, 0, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_bit_example bit 3 error\n");
		return  rv;
	}

	return rv;
}

/*
*   add an ACL entry according to the test: AT_Cint_external_frwd_kbp_app_2_acl_traffic
*/ 
int frwd_kbp_app_2_acls_kbp_entry_add_example(int unit, int index) {

	int i,rv;
	bcm_field_entry_t ent_elk;

	uint16 vlanId = 100;

	bcm_mac_t s_mac = {0x00, 0x00, 0x07, 0x00, 0x01, 0x00};	
	bcm_mac_t s_mac_mask = {0xff,0xff,0xff,0xff,0xff,0xff};
	
	bcm_field_entry_create(unit, ACL_BASE_GROUP_INDEX + index, &ent_elk);

	switch (index) {
	case 1:/* 1st: vlanId only*/
		rv = bcm_field_qualify_OuterVlanId(unit, ent_elk, vlanId, 0xFFF);
		if (rv) {
		  printf("bcm_field_qualify_OuterVlanId error, index = %d\n",index);
		  return rv;
		}
		break;
	case 3:/* 2nd: VlanId + s mac*/
		rv = bcm_field_qualify_OuterVlanId(unit, ent_elk, vlanId, 0xFFF);
		if (rv) {
		  printf("bcm_field_qualify_OuterVlanId error, index = %d\n",index);
		  return rv;
		}
		rv = bcm_field_qualify_SrcMac(unit, ent_elk, s_mac, s_mac_mask);
		if (rv) {
		  printf("bcm_field_qualify_SrcMac error, index = %d\n",index);
		  return rv;
		}
		break;
	default:
		printf("Unknown index value = %d",index);
		return -1;
	}

	rv = bcm_field_entry_prio_set(unit, ent_elk, 10);
	if (rv) {
	  printf("bcm_field_entry_prio_set error\n");
	  return rv;
	}

	rv = bcm_field_entry_install(unit, ent_elk);
	if (rv) {
	  printf("bcm_field_entry_install error\n");
	  return rv;
	}

	return rv;
}

/*
 * test for 4 mode of ACLs Entries: 
 * 		|****| = first group, |####| = second group 
 *  
 * scenario		 	LSB				MSB	
 * 	  0  		|**********|    |***       | 
 *    1  		|**********|    |***####   | 
 *    2  		|****###   |    |          | 
 *	  3  		|****######|    |###       | 
 *    4          freestyle
 */

int acl_kbp_multiple_acls_config_example(int unit, int scenario) {

	int rv=0;

	int presel_id_elk;
	bcm_field_group_config_t grp_elk;

	rv = bcm_field_presel_create(unit, &presel_id_elk);
	if (rv) {
	  printf("bcm_field_presel_create error\n");
	  return rv;
	}

	rv = bcm_field_qualify_Stage(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageExternal);
	if (rv) {
	  printf("bcm_field_qualify_Stage error\n");
	  return rv;
	}

	rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeL2);
	if (rv) {
	  printf("bcm_field_qualify_AppType AppTypeL2 error\n");
	  return rv;
	}

	bcm_field_group_config_t_init(&grp_elk); 
	BCM_FIELD_PRESEL_ADD(grp_elk.preselset, presel_id_elk);
	grp_elk.mode =  bcmFieldGroupModeAuto;
	grp_elk.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;

	BCM_FIELD_QSET_INIT(grp_elk.qset);
	BCM_FIELD_ASET_INIT(grp_elk.aset);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyStageExternal);
	grp_elk.group 	 = ACL_BASE_GROUP_INDEX;
	grp_elk.priority = ACL_BASE_GROUP_PRIORITY;
	

	switch (scenario) {
		case 0:
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyDstMac);
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcMac);
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyEtherType);
			BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue0Set);

			rv = bcm_field_group_config_create(unit, &grp_elk);
			if (rv) {
			  printf("bcm_field_group_config_create (scenario,group)(0,0) error \n");
			  return rv;
			}
			break;
		case 1:
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyDstMac);
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcMac);
			BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue0Set);

			rv = bcm_field_group_config_create(unit, &grp_elk);
			if (rv) {
				printf("bcm_field_group_config_create (scenario,group)(1,0) error \n");
			  return rv;
			}

			grp_elk.group++;
			grp_elk.priority++;
			BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifySrcMac);
			BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifyDstMac);
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyEtherType);
			BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue0Set);
			BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue1Set);
			
			rv = bcm_field_group_config_create(unit, &grp_elk);
			if (rv) {
				printf("bcm_field_group_config_create (scenario,group)(1,1) error \n");
			  return rv;
			}
			break;
		case 2:
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyDstMac);
			BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue0Set);

			rv = bcm_field_group_config_create(unit, &grp_elk);
			if (rv) {
				printf("bcm_field_group_config_create (scenario,group)(2,0) error \n");
			  return rv;
			}

			grp_elk.group++;
			grp_elk.priority++;
			BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifyDstMac);
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyOuterVlanId);
			BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue0Set);
			BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue1Set);
			
			rv = bcm_field_group_config_create(unit, &grp_elk);
			if (rv) {
				printf("bcm_field_group_config_create (scenario,group)(2,1) error \n");
			  return rv;
			}
			break;
		case 3:
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyDstMac);
			BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue0Set);

			rv = bcm_field_group_config_create(unit, &grp_elk);
			if (rv) {
				printf("bcm_field_group_config_create (scenario,group)(1,0) error \n");
			  return rv;
			}

			grp_elk.group++;
			grp_elk.priority++;
			BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifyDstMac);
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcMac);
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyEtherType);
			BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue0Set);
			BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue1Set);
			
			rv = bcm_field_group_config_create(unit, &grp_elk);
			if (rv) {
				printf("bcm_field_group_config_create (scenario,group)(1,1) error \n");
			  return rv;
			}
			break;
		case 4:
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyDstMac);
			BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue0Set);

			rv = bcm_field_group_config_create(unit, &grp_elk);
			if (rv) {
				printf("bcm_field_group_config_create (scenario,group)(4,0) error \n");
			  return rv;
			}

			grp_elk.group++;
			grp_elk.priority++;
			BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifyDstMac);
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyEtherType);
			BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue0Set);
			BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue1Set);

			rv = bcm_field_group_config_create(unit, &grp_elk);
			if (rv) {
				printf("bcm_field_group_config_create (scenario,group)(4,1) error \n");
			  return rv;
			}
			
			grp_elk.group++;
			grp_elk.priority++;
			BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifyEtherType);
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcPort);
			BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue1Set);
			BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue2Set);

			rv = bcm_field_group_config_create(unit, &grp_elk);
			if (rv) {
				printf("bcm_field_group_config_create (scenario,group)(4,2) error \n");
			  return rv;
			}
			
			grp_elk.group++;
			grp_elk.priority++;
			BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifySrcPort);
			BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcMac);
			BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue2Set);
			BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue3Set);

			rv = bcm_field_group_config_create(unit, &grp_elk);
			if (rv) {
				printf("bcm_field_group_config_create (scenario,group)(4,3) error \n");
			  return rv;
			}

			break;
		default:
			printf("Unknown Scenarion: %d \n",scenario);
			return -1;
	}

	/* Initialize the KBP */
	bshell(unit, "kbp init_appl");

	return rv;
}

int acl_kbp_multiple_acls_add_entry_example(int unit, int scenario, int index) {

	int rv;
	int entry_index;
	bcm_field_entry_t ent_elk;

	uint16 EthType = 0x0800;
	uint16 vlan_id = 100;

	bcm_mac_t dst  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};
	bcm_mac_t src  = {0x00, 0x00, 0x07, 0x00, 0x01, 0x00};

	bcm_mac_t mask_0 = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	bcm_mac_t mask_1 = {0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00};
	bcm_mac_t mask_2 = {0xF0, 0xF0, 0x0F, 0xF0, 0x0F, 0x0F};
	bcm_mac_t mask_3 = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};

	bcm_mac_t dst_freestyle  = {0x11, 0x02, 0x03, 0x04, 0x05, 0x66};
	bcm_mac_t src_freestyle  = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

	bcm_field_entry_create(unit, index, &ent_elk);

	entry_index = scenario*4+index;
	switch (entry_index) {
		case 0:
			rv = bcm_field_qualify_EtherType(unit, ent_elk, EthType, 0xFFFF);
			if (rv) {
			  printf("bcm_field_qualify_OuterVlanId error, entry_index = %d\n",entry_index);
			  return rv;
			}
			rv = bcm_field_qualify_DstMac(unit, ent_elk, dst, mask_1);
			if (rv) {
			  printf("bcm_field_qualify_DstMac error, entry_index = %d\n",entry_index);
			  return rv;
			}
			rv = bcm_field_qualify_SrcMac(unit, ent_elk, src, mask_1);
			if (rv) {
			  printf("bcm_field_qualify_SrcMac error, entry_index = %d\n",entry_index);
			  return rv;
			}
			break;
		case 4:
			rv = bcm_field_qualify_DstMac(unit, ent_elk, dst, mask_3);
			if (rv) {
			  printf("bcm_field_qualify_DstMac error, entry_index = %d\n",entry_index);
			  return rv;
			}
			rv = bcm_field_qualify_SrcMac(unit, ent_elk, src, mask_2);
			if (rv) {
			  printf("bcm_field_qualify_SrcMac error, entry_index = %d\n",entry_index);
			  return rv;
			}
			break;
		case 5:
			rv = bcm_field_qualify_EtherType(unit, ent_elk, EthType, 0xFFFF);
			if (rv) {
			  printf("bcm_field_qualify_EtherType error, entry_index = %d\n",entry_index);
			  return rv;
			}
			break;
		case 8:
			rv = bcm_field_qualify_DstMac(unit, ent_elk, dst, mask_0);
			if (rv) {
			  printf("bcm_field_qualify_DstMac error, entry_index = %d\n",entry_index);
			  return rv;
			}
			break;
		case 9:
			rv = bcm_field_qualify_OuterVlanId(unit, ent_elk, vlan_id, 0xFFF);
			if (rv) {
			  printf("bcm_field_qualify_OuterVlan error, entry_index = %d\n",entry_index);
			  return rv;
			}
			break;
		case 12:
			rv = bcm_field_qualify_DstMac(unit, ent_elk, dst, mask_0);
			if (rv) {
			  printf("bcm_field_qualify_DstMac error, entry_index = %d\n",entry_index);
			  return rv;
			}
			break;
		case 13:
			rv = bcm_field_qualify_SrcMac(unit, ent_elk, src, mask_0);
			if (rv) {
			  printf("bcm_field_qualify_SrcMac error, entry_index = %d\n",entry_index);
			  return rv;
			}
			rv = bcm_field_qualify_EtherType(unit, ent_elk, EthType, 0xFFFF);
			if (rv) {
			  printf("bcm_field_qualify_EtherType error, entry_index = %d\n",entry_index);
			  return rv;
			}
			break;
		case 16:
			rv = bcm_field_qualify_DstMac(unit, ent_elk, dst_freestyle, mask);
			if (rv) {
			  printf("bcm_field_qualify_DstMac error, entry_index = %d\n",entry_index);
			  return rv;
			}
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue0Set,0x12345678,0xabcd0000);
			if (rv) {
			  printf("bcm_field_action_add error\n");
			  return rv;
			}
			break;
		case 17:
		     rv = bcm_field_qualify_EtherType(unit, ent_elk, EthType, 0xFFFF);
			if (rv) {
			  printf("bcm_field_qualify_EtherType error, entry_index = %d\n",entry_index);
			  return rv;
			}
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue1Set,0x4321,0);
			if (rv) {
			  printf("bcm_field_action_add error\n");
			  return rv;
			}
			break;
		case 18:
			break;
		case 19:
			rv = bcm_field_qualify_SrcMac(unit, ent_elk, src_freestyle, mask_0);
			if (rv) {
			  printf("bcm_field_qualify_SrcMac error, entry_index = %d\n",entry_index);
			  return rv;
			}
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue3Set,0x87654321,0);
			if (rv) {
			  printf("bcm_field_action_add error\n");
			  return rv;
			}
			break;
		default:
			printf("Unknown entry_index value = %d, scenario = %d, index = %d\n",entry_index,scenario,index);
			return -1;
	}

	rv = bcm_field_entry_prio_set(unit, ent_elk, 10);
	if (rv) {
	  printf("bcm_field_entry_prio_set error\n");
	  return rv;
	}

	rv = bcm_field_entry_install(unit, ent_elk);
	if (rv) {
	  printf("bcm_field_entry_install error\n");
	  return rv;
	}

	return rv;
}

int acl_kbp_multiple_acls_set_configuration(int unit, int scenario) {
	int rv;

	rv = acl_kbp_multiple_acls_config_example(unit,scenario);
	if (rv) {
		printf("acl_kbp_multiple_acls_config_example error\n");
		return  rv;
	}

	rv = configure_pmf_action_by_hit_bit_example(unit,HIT_BIT_0_QUAL,bcmFieldAppTypeL2, bcmFieldActionDrop, 0, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_bit_example bit 0 error\n");
		return  rv;
	}
	if (scenario != 0) {
		/* scenario 0 has only one group on search 0
		   scenarios 1-3 have 2 groups, on search 0 and search 1*/
		rv = configure_pmf_action_by_hit_bit_example(unit, HIT_BIT_1_QUAL, bcmFieldAppTypeL2, bcmFieldActionDrop, 0, 0);
		if (rv) {
			printf("configure_pmf_action_by_hit_bit_example bit 2 error\n");
			return  rv;
		}
	}
	return rv;
}

int acl_kbp_check_entry_priority_config_example(int unit, int changed_result_size) {

	int rv=0;

	int presel_id_elk;
	bcm_field_group_config_t grp_elk;

	rv = bcm_field_presel_create(unit, &presel_id_elk);
	if (rv) {
	  printf("bcm_field_presel_create error\n");
	  return rv;
	}

	rv = bcm_field_qualify_Stage(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageExternal);
	if (rv) {
	  printf("bcm_field_qualify_Stage error\n");
	  return rv;
	}

	rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeL2);
	if (rv) {
	  printf("bcm_field_qualify_AppType AppTypeL2 error\n");
	  return rv;
	}

	bcm_field_group_config_t_init(&grp_elk); 
	BCM_FIELD_PRESEL_ADD(grp_elk.preselset, presel_id_elk);
	grp_elk.mode =  bcmFieldGroupModeAuto;
	grp_elk.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;

	BCM_FIELD_QSET_INIT(grp_elk.qset);
	BCM_FIELD_ASET_INIT(grp_elk.aset);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyStageExternal);
	grp_elk.group 	 = ACL_BASE_GROUP_INDEX;
	grp_elk.priority = ACL_BASE_GROUP_PRIORITY;

	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyDstMac);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcMac);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyEtherType);

	if (changed_result_size == 0) {
		BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue1Set);
	}else{
		BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue3Set);
	}

	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create error \n");
	  return rv;
	}

	/* Initialize the KBP */
	bshell(unit, "kbp init_appl");

	return rv;
}

int acl_kbp_check_entry_priority_add_entry_example(int unit, int index) {

	int rv;
	bcm_field_entry_t ent_elk;

	uint16 EthType = 0x0800;
	uint16 vlan_id = 100;

	bcm_mac_t dst  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01};
	bcm_mac_t src  = {0x00, 0x00, 0x07, 0x00, 0x01, 0x00};

	bcm_mac_t mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	bcm_field_entry_create(unit, ACL_BASE_GROUP_INDEX, &ent_elk);

	switch (index) {
		case 0:
			rv = bcm_field_qualify_EtherType(unit, ent_elk, EthType, 0xFFFF);
			if (rv) {
			  printf("bcm_field_qualify_EtherType error, index = %d\n",index);
			  return rv;
			}
			rv = bcm_field_qualify_DstMac(unit, ent_elk, dst, mask);
			if (rv) {
			  printf("bcm_field_qualify_DstMac error, index = %d\n",index);
			  return rv;
			}
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue1Set,0x87654321, 0);
			if (rv) {
			  printf("bcm_field_action_add error\n");
			  return rv;
			}
			break;
		case 1:
			rv = bcm_field_qualify_SrcMac(unit, ent_elk, src, mask);
			if (rv) {
			  printf("bcm_field_qualify_SrcMac error, index = %d\n",index);
			  return rv;
			}
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue1Set,0x12345678, 0);
			if (rv) {
			  printf("bcm_field_action_add error\n");
			  return rv;
			}
			break;
	case 2: /* support result change size*/
			rv = bcm_field_qualify_EtherType(unit, ent_elk, EthType, 0xFFFF);
			if (rv) {
			  printf("bcm_field_qualify_EtherType error, index = %d\n",index);
			  return rv;
			}
			rv = bcm_field_qualify_DstMac(unit, ent_elk, dst, mask);
			if (rv) {
			  printf("bcm_field_qualify_DstMac error, index = %d\n",index);
			  return rv;
			}
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue3Set,0x87654321, 0);
			if (rv) {
			  printf("bcm_field_action_add error\n");
			  return rv;
			}
			break;
		default:
			printf("Unknown index = %d\n",index);
			return -1;
	}

	rv = bcm_field_entry_prio_set(unit, ent_elk, 10+index);
	if (rv) {
	  printf("bcm_field_entry_prio_set error\n");
	  return rv;
	}

	rv = bcm_field_entry_install(unit, ent_elk);
	if (rv) {
	  printf("bcm_field_entry_install error\n");
	  return rv;
	}

	return rv;
}

int acl_kbp_check_entry_priority_set_configuration(int unit, int size_change) {

	int rv;

	uint64 val,mask;

	COMPILER_64_ZERO(val);
	COMPILER_64_ZERO(mask);

	COMPILER_64_SET(val ,0x00000000,0x87654321);
	COMPILER_64_SET(mask,0x00000000,0xffffffff);

	rv = acl_kbp_check_entry_priority_config_example(unit, size_change);
	if (rv) {
		printf("acl_kbp_check_entry_priority_config_example error\n");
		return  rv;
	}

	if (size_change == 0) {
		rv = configure_pmf_action_by_hit_value_example(unit, HIT_BIT_1_QUAL, val, mask, bcmFieldAppTypeL2, 0, bcmFieldActionDrop, 0, 0, 0);
		if (rv) {
			printf("configure_pmf_action_by_hit_value_example bit 1 error\n");
			return  rv;
		}
	}else{
		rv = configure_pmf_action_by_hit_value_example(unit, HIT_BIT_3_QUAL, val, mask, bcmFieldAppTypeL2, 0, bcmFieldActionDrop, 0, 0, 0);
		if (rv) {
			printf("configure_pmf_action_by_hit_value_example bit 3 error\n");
			return  rv;
		}
	}
	return rv;
}

int acl_kbp_check_256_bit_result_fwd_rpf_config_example(int unit) {

	int rv=0;

	int presel_id_elk;
	bcm_field_group_config_t grp_elk;

	rv = bcm_field_presel_create(unit, &presel_id_elk);
	if (rv) {
	  printf("bcm_field_presel_create error\n");
	  return rv;
	}

	rv = bcm_field_qualify_Stage(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageExternal);
	if (rv) {
	  printf("bcm_field_qualify_Stage error\n");
	  return rv;
	}

	rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeL2);
	if (rv) {
	  printf("bcm_field_qualify_AppType AppTypeL2 error\n");
	  return rv;
	}

	bcm_field_group_config_t_init(&grp_elk); 
	BCM_FIELD_PRESEL_ADD(grp_elk.preselset, presel_id_elk);
	grp_elk.mode =  bcmFieldGroupModeAuto;
	grp_elk.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;

	BCM_FIELD_QSET_INIT(grp_elk.qset);
	BCM_FIELD_ASET_INIT(grp_elk.aset);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyStageExternal);
	grp_elk.group 	 = ACL_BASE_GROUP_INDEX;
	grp_elk.priority = ACL_BASE_GROUP_PRIORITY;


	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyOuterVlanId);
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue0Set);
	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 0 error \n");
	  return rv;
	}

	BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifyOuterVlanId);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyDstMac);
	BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue0Set);
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue1Set);
	grp_elk.group++;
	grp_elk.priority++;
	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 1 error \n");
	  return rv;
	}
	
	BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifyDstMac);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcMac);
	BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue1Set);
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue2Set);
	grp_elk.group++;
	grp_elk.priority++;
	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 2 error \n");
	  return rv;
	}

	BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifySrcMac);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyEtherType);
	BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue2Set);
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue3Set);
	grp_elk.group++;
	grp_elk.priority++;
	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 3 error \n");
	  return rv;
	}

	/* Initialize the KBP */
	bshell(unit, "kbp init_appl");

	return rv;
}

int acl_kbp_check_256_bit_result_fwd_rpf_add_entry_example(int unit, int hitbit) {

	int rv;
	bcm_field_entry_t ent_elk;

	uint16 vlan_id = 100;
	uint16 EthType = 0x0800;
	bcm_mac_t dst  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01};
	bcm_mac_t src  = {0x00, 0x00, 0x07, 0x00, 0x01, 0x00};

	bcm_mac_t mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	bcm_field_entry_create(unit, ACL_BASE_GROUP_INDEX+hitbit, &ent_elk);

	switch (hitbit) {
		case 0:
			rv = bcm_field_qualify_OuterVlanId(unit, ent_elk, vlan_id, 0xFFF);
			if (rv) {
			  printf("bcm_field_qualify_OuterVlan error\n");
			  return rv;
			}
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue0Set,0x87654321,0xabcdefff);
			if (rv) {
			  printf("bcm_field_action_add error\n");
			  return rv;
			}
			break;
		case 1:
			rv = bcm_field_qualify_DstMac(unit, ent_elk, dst, mask);
			if (rv) {
			  printf("bcm_field_qualify_DstMac error\n");
			  return rv;
			}
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue1Set,0x87654321,0x12340000);
			if (rv) {
			  printf("bcm_field_action_add error\n");
			  return rv;
			}
			break;
		case 2:
			rv = bcm_field_qualify_SrcMac(unit, ent_elk, src, mask);
			if (rv) {
			  printf("bcm_field_qualify_SrcMac error\n");
			  return rv;
			}
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue2Set,0x12346578,0xabcdefff);
			if (rv) {
			  printf("bcm_field_action_add error\n");
			  return rv;
			}
			break;
		case 3:
			rv = bcm_field_qualify_EtherType(unit, ent_elk, EthType, 0xFFFF);
			if (rv) {
			  printf("bcm_field_qualify_EtherType error\n");
			  return rv;
			}
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue3Set,0x12348765,0x00000000);
			if (rv) {
			  printf("bcm_field_action_add error\n");
			  return rv;
			}
			break;
		default:
			printf("Unknown hitbit = %d\n",hitbit);
			return -1;
	}

	rv = bcm_field_entry_prio_set(unit, ent_elk, 10+hitbit);
	if (rv) {
	  printf("bcm_field_entry_prio_set error\n");
	  return rv;
	}

	rv = bcm_field_entry_install(unit, ent_elk);
	if (rv) {
	  printf("bcm_field_entry_install error\n");
	  return rv;
	}

	return rv;

}

int acl_kbp_check_256_bit_result_fwd_rpf_set_pmf_configuration(int unit,int hitbit) {

	int rv;
	uint64 val,mask;

	switch (hitbit) {
	case 0:	/* 48 bit */
		COMPILER_64_ZERO(val);
		COMPILER_64_ZERO(mask);
		COMPILER_64_SET(val ,0x0000abcd,0x87654321);
		COMPILER_64_SET(mask,0x0000ffff,0xffffffff);
		rv = configure_pmf_action_by_hit_value_example(unit, HIT_BIT_0_QUAL, val, mask, bcmFieldAppTypeL2, 0, bcmFieldActionDrop, 0, 0, 0);
		if (rv) {
			printf("configure_pmf_action_by_hit_value_example bit 0 error\n");
			return  rv;
		}
		break;
	case 1: /* 48 bit */
		COMPILER_64_ZERO(val);
		COMPILER_64_ZERO(mask);
		COMPILER_64_SET(val ,0x00001234,0x87654321);
		COMPILER_64_SET(mask,0x0000ffff,0xffffffff);
		rv = configure_pmf_action_by_hit_value_example(unit, HIT_BIT_1_QUAL, val, mask, bcmFieldAppTypeL2, 0, bcmFieldActionDrop, 0, 0, 0);
		if (rv) {
			printf("configure_pmf_action_by_hit_value_example bit 1 error\n");
			return  rv;
		}
		break;
	case 2: /*24 bit*/
		COMPILER_64_ZERO(val);
		COMPILER_64_ZERO(mask);
		COMPILER_64_SET(val ,0x00000000,0x00cdefff);
		COMPILER_64_SET(mask,0x00000000,0x00ffffff);
		rv = configure_pmf_action_by_hit_value_example(unit, HIT_BIT_2_QUAL, val, mask, bcmFieldAppTypeL2, 0, bcmFieldActionDrop, 0, 0, 0);
		if (rv) {
			printf("configure_pmf_action_by_hit_value_example bit 2 error\n");
			return  rv;
		}
		break;
	case 3: /*32 bit*/
		COMPILER_64_ZERO(val);
		COMPILER_64_ZERO(mask);
		COMPILER_64_SET(val ,0x00000000,0x12348765);
		COMPILER_64_SET(mask,0x00000000,0xffffffff);
		rv = configure_pmf_action_by_hit_value_example(unit, HIT_BIT_3_QUAL, val, mask, bcmFieldAppTypeL2, 0, bcmFieldActionDrop, 0, 0, 0);
		if (rv) {
			printf("configure_pmf_action_by_hit_value_example bit 3 error\n");
			return  rv;
		}
		break;
	}
	return rv;
}

int acl_kbp_ipv6mc_config_example(int unit) {

	int rv=0;

	int presel_id_elk;
	bcm_field_group_config_t grp_elk;

	rv = bcm_field_presel_create(unit, &presel_id_elk);
	if (rv) {
	  printf("bcm_field_presel_create error\n");
	  return rv;
	}

	rv = bcm_field_qualify_Stage(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageExternal);
	if (rv) {
	  printf("bcm_field_qualify_Stage error\n");
	  return rv;
	}

	rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp6Mcast);
	if (rv) {
	  printf("bcm_field_qualify_AppType bcmFieldAppTypeIp6Mcast error\n");
	  return rv;
	}

	bcm_field_group_config_t_init(&grp_elk); 
	BCM_FIELD_PRESEL_ADD(grp_elk.preselset, presel_id_elk);
	grp_elk.mode =  bcmFieldGroupModeAuto;
	grp_elk.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;


	BCM_FIELD_QSET_INIT(grp_elk.qset);
	BCM_FIELD_ASET_INIT(grp_elk.aset);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyStageExternal);
	grp_elk.group 	 = ACL_BASE_GROUP_INDEX+1;
	grp_elk.priority = ACL_BASE_GROUP_PRIORITY+1;

	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyDstMac);
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue1Set);
	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 1 error \n");
	  return rv;
	}

	BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifyDstMac);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyEtherType);
	BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue1Set);
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue3Set);
	grp_elk.group 	 = ACL_BASE_GROUP_INDEX+3;
	grp_elk.priority = ACL_BASE_GROUP_PRIORITY+3;

	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 3 error \n");
	  return rv;
	}

	/* Initialize the KBP */
	bshell(unit, "kbp init_appl");

	return rv;
}

int acl_kbp_ipv6mc_add_entry_example(int unit, int hitbit) {

	int rv;
	bcm_field_entry_t ent_elk;

	bcm_mac_t dst  = {0x33, 0x33, 0x03, 0x02, 0x01, 0x00};
	bcm_mac_t mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint16 EthType = 0x86dd;

	bcm_field_entry_create(unit, ACL_BASE_GROUP_INDEX+hitbit, &ent_elk);

	switch (hitbit) {
		case 3:
			rv = bcm_field_qualify_EtherType(unit, ent_elk, EthType, 0x0000);
			if (rv) {
			 printf("bcm_field_qualify_EtherType error\n");
			  return rv;
			}
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue3Set,0x87654321,0xabcde0000);
			if (rv) {
			  printf("bcm_field_action_add error\n");
			  return rv;
			}
			break;
		case 1:
			rv = bcm_field_qualify_DstMac(unit, ent_elk, dst, mask);
			if (rv) {
			  printf("bcm_field_qualify_DstMac error\n");
			  return rv;
			}
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue1Set,0x87654321,0x12340000);
			if (rv) {
			  printf("bcm_field_action_add error\n");
			  return rv;
			}
			break;
		default:
			printf("Unknown hitbit = %d\n",hitbit);
			return -1;
	}

	rv = bcm_field_entry_prio_set(unit, ent_elk, 10+hitbit);
	if (rv) {
	  printf("bcm_field_entry_prio_set error\n");
	  return rv;
	}

	rv = bcm_field_entry_install(unit, ent_elk);
	if (rv) {
	  printf("bcm_field_entry_install error\n");
	  return rv;
	}

	return rv;

}

int acl_kbp_ipv6mc_set_configuration(int unit) {
	int rv;

	rv = acl_kbp_ipv6mc_config_example(unit);
	if (rv) {
		printf("acl_kbp_ipv6mc_config_example error\n");
		return  rv;
	}

	rv = configure_pmf_action_by_hit_bit_example(unit,HIT_BIT_3_QUAL,bcmFieldAppTypeIp6Mcast, bcmFieldActionDrop, 0, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_bit_example bit 3 error\n");
		return  rv;
	}

	rv = configure_pmf_action_by_hit_bit_example(unit,HIT_BIT_1_QUAL,bcmFieldAppTypeIp6Mcast, bcmFieldActionDrop, 0, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_bit_example bit 1 error\n");
		return  rv;
	}

	return rv;
}


int acl_kbp_acls_4_5_basic_config_example(int unit, bcm_field_AppType_t app_type) {

	int rv=0;

	int presel_id_elk;
	bcm_field_group_config_t grp_elk;

	rv = bcm_field_presel_create(unit, &presel_id_elk);
	if (rv) {
	  printf("bcm_field_presel_create error\n");
	  return rv;
	}

	rv = bcm_field_qualify_Stage(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageExternal);
	if (rv) {
	  printf("bcm_field_qualify_Stage error\n");
	  return rv;
	}

	rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, app_type);
	if (rv) {
	  printf("bcm_field_qualify_AppType bcmFieldAppTypeIp6Mcast error\n");
	  return rv;
	}

	bcm_field_group_config_t_init(&grp_elk); 
	BCM_FIELD_PRESEL_ADD(grp_elk.preselset, presel_id_elk);
	grp_elk.mode =  bcmFieldGroupModeAuto;
	grp_elk.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;


	BCM_FIELD_QSET_INIT(grp_elk.qset);
	BCM_FIELD_ASET_INIT(grp_elk.aset);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyStageExternal);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyOuterVlanId);

	grp_elk.group 	 = ACL_BASE_GROUP_INDEX + 1;
	grp_elk.priority = ACL_BASE_GROUP_PRIORITY + 1;
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue1Set);
	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 1 error \n");
	  return rv;
	}

	grp_elk.group 	 = ACL_BASE_GROUP_INDEX + 3;
	grp_elk.priority = ACL_BASE_GROUP_PRIORITY + 3;
	BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue1Set);
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue3Set);
	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 3 error \n");
	  return rv;
	}

	grp_elk.group 	 = ACL_BASE_GROUP_INDEX + 4;
	grp_elk.priority = ACL_BASE_GROUP_PRIORITY + 4;
	BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue3Set);
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue4Set);
	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 4 error \n");
	  return rv;
    }

	grp_elk.group 	 = ACL_BASE_GROUP_INDEX + 5;
	grp_elk.priority = ACL_BASE_GROUP_PRIORITY + 5;
	BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue4Set);
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue5Set);
	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 5 error \n");
	  return rv;
    }

	/* Initialize the KBP */
	bshell(unit, "kbp init_appl");

	return rv;
}

int acl_kbp_acls_4_5_basic_add_entry_example(int unit, int hitbit) {

	int rv;
	bcm_field_entry_t ent_elk;

	uint16 vlan = 100+hitbit;

	bcm_field_entry_create(unit, ACL_BASE_GROUP_INDEX+hitbit, &ent_elk);
	rv = bcm_field_qualify_OuterVlanId(unit, ent_elk, vlan, 0xFFF);
	if (rv) {
	  printf("bcm_field_qualify_OuterVlanId error\n");
	  return rv;
	}

	switch (hitbit) {
		case 0:
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue0Set,0x12121212,0x12121212);
			break;
		case 1:
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue1Set,0x34343434,0x34343434);
			break;
		case 2:
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue2Set,0x56565656,0x56565656);
			break;
		case 3:
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue3Set,0x78787878,0x78787878);
			break;
		case 4:
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue4Set,0xabababab,0xabababab);
			break;
		case 5:
			rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue5Set,0xcdcdcdcd,0xcdcdcdcd);
			break;
		default:
			printf("Unknown hitbit = %d\n",hitbit);
			return -1;
	}
	if (rv) {
	  printf("bcm_field_action_add hit bit =%d error\n",hitbit);
	  return rv;
	}

	rv = bcm_field_entry_prio_set(unit, ent_elk, 10+hitbit);
	if (rv) {
	  printf("bcm_field_entry_prio_set error\n");
	  return rv;
	}

	rv = bcm_field_entry_install(unit, ent_elk);
	if (rv) {
	  printf("bcm_field_entry_install error\n");
	  return rv;
	}

	return rv;

}

int acl_kbp_acls_4_5_basic_set_configuration_by_hit_bit(int unit, int is_frwrd_prgm) {
	int rv;

	bcm_field_AppType_t app_type;
	if (is_frwrd_prgm == 0) {
		app_type = bcmFieldAppTypeL2;
	}else{
		app_type = bcmFieldAppTypeIp4Ucast;
	}
	rv = acl_kbp_acls_4_5_basic_config_example(unit,app_type);
	if (rv) {
		printf("acl_kbp_acls_4_5_basic_config_example error\n");
		return  rv;
	}

	rv = configure_pmf_action_by_hit_bit_example(unit,HIT_BIT_1_QUAL,app_type, bcmFieldActionDrop, 0, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_bit_example bit 1 error\n");
		return  rv;
	}

	rv = configure_pmf_action_by_hit_bit_example(unit,HIT_BIT_3_QUAL,app_type, bcmFieldActionDrop, 0, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_bit_example bit 3 error\n");
		return  rv;
	}

	rv = configure_pmf_action_by_hit_bit_example(unit,HIT_BIT_4_QUAL,app_type, bcmFieldActionDrop, 0, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_bit_example bit 4 error\n");
		return  rv;
	}

	rv = configure_pmf_action_by_hit_bit_example(unit,HIT_BIT_5_QUAL,app_type, bcmFieldActionDrop, 0, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_bit_example bit 5 error\n");
		return  rv;
	}
	return rv;
}

int acl_kbp_acls_4_5_basic_set_configuration_by_results_value(int unit, int is_frwrd_prgm) {
	int rv;
	uint64 val,mask;
	bcm_field_AppType_t app_type;

	if (is_frwrd_prgm == 0) {
		app_type = bcmFieldAppTypeL2;
	}else{
		app_type = bcmFieldAppTypeIp4Ucast;
	}

	rv = acl_kbp_acls_4_5_basic_config_example(unit,app_type);
	if (rv) {
		printf("acl_kbp_acls_4_5_basic_config_example error\n");
		return  rv;
	}

	/*search#1 - 32 bit */
	COMPILER_64_ZERO(val);
	COMPILER_64_ZERO(mask);
	COMPILER_64_SET(val ,0x00000000,0x34343434);
	COMPILER_64_SET(mask,0x00000000,0xffffffff);
	rv = configure_pmf_action_by_hit_value_example(unit, HIT_BIT_1_QUAL, val, mask, app_type, 0, bcmFieldActionDrop, 0, 0, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_value_example bit 1 error\n");
		return  rv;
	}

	/*search#3 - 16 bit */
	COMPILER_64_ZERO(val);
	COMPILER_64_ZERO(mask);
	COMPILER_64_SET(val ,0x00000000,0x00007878);
	COMPILER_64_SET(mask,0x00000000,0x0000ffff);
	rv = configure_pmf_action_by_hit_value_example(unit, HIT_BIT_3_QUAL, val, mask, app_type, 0, bcmFieldActionDrop, 0, 0, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_value_example bit 3 error\n");
		return  rv;
	}

	/*search#4 - 32 bit */
	COMPILER_64_ZERO(val);
	COMPILER_64_ZERO(mask);
	COMPILER_64_SET(val ,0x00000000,0xabababab);
	COMPILER_64_SET(mask,0x00000000,0xffffffff);
	rv = configure_pmf_action_by_hit_value_example(unit, HIT_BIT_4_QUAL, val, mask, app_type, 0, bcmFieldActionDrop, 0, 0, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_value_example bit 4 error\n");
		return  rv;
	}

	/*search#5 - 32 bit */
	COMPILER_64_ZERO(val);
	COMPILER_64_ZERO(mask);
	COMPILER_64_SET(val ,0x00000000,0xcdcdcdcd);
	COMPILER_64_SET(mask,0x00000000,0xffffffff);
	rv = configure_pmf_action_by_hit_value_example(unit, HIT_BIT_5_QUAL, val, mask, app_type, 0, bcmFieldActionDrop, 0, 0, 0);
	if (rv) {
		printf("configure_pmf_action_by_hit_value_example bit 5 error\n");
		return  rv;
	}
	return rv;
}

int acl_kbp_ipv4uc_ipv4dc_shared_acls_config_example(int unit) {

	int rv=0;

	int presel_id_elk;
	bcm_field_group_config_t grp_elk;

	rv = bcm_field_presel_create(unit, &presel_id_elk);
	if (rv) {
	  printf("bcm_field_presel_create error\n");
	  return rv;
	}

	rv = bcm_field_qualify_Stage(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageExternal);
	if (rv) {
	  printf("bcm_field_qualify_Stage error\n");
	  return rv;
	}

	rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp4DoubleCapacity);
	if (rv) {
	  printf("bcm_field_qualify_AppType bcmFieldAppTypeIp4DoubleCapacity error\n");
	  return rv;
	}

	rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp4Ucast);
	if (rv) {
	  printf("bcm_field_qualify_AppType bcmFieldAppTypeIp4Ucast error\n");
	  return rv;
	}

	bcm_field_group_config_t_init(&grp_elk); 
	BCM_FIELD_PRESEL_ADD(grp_elk.preselset, presel_id_elk);
	grp_elk.mode =  bcmFieldGroupModeAuto;
	grp_elk.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;

	BCM_FIELD_QSET_INIT(grp_elk.qset);
	BCM_FIELD_ASET_INIT(grp_elk.aset);
	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyStageExternal);

	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcMac);
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue1Set);
	grp_elk.group 	 = ACL_BASE_GROUP_INDEX + 1;
	grp_elk.priority = ACL_BASE_GROUP_PRIORITY + 1;
	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 1 error \n");
	  return rv;
	}
	BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifySrcMac);
	BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue1Set);

	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyEtherType);
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue3Set);
	grp_elk.group 	 = ACL_BASE_GROUP_INDEX + 3;
	grp_elk.priority = ACL_BASE_GROUP_PRIORITY + 3;
	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 3 error \n");
	  return rv;
    }
	BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifyEtherType);
	BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue3Set);

	BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifySrcIp);
	BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue4Set);
	grp_elk.group 	 = ACL_BASE_GROUP_INDEX + 4;
	grp_elk.priority = ACL_BASE_GROUP_PRIORITY + 4;
	rv = bcm_field_group_config_create(unit, &grp_elk);
	if (rv) {
	  printf("bcm_field_group_config_create 4 error \n");
	  return rv;
    }
	BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifySrcIp);
	BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue4Set);

	/* Initialize the KBP */
	bshell(unit, "kbp init_appl");

	return rv;
}

int acl_kbp_ipv4uc_ipv4dc_shared_acls_add_entry_example(int unit, int hitbit) {

	int rv;
	bcm_field_entry_t ent_elk;

	uint16 EthType = 0x0800;
	
	bcm_mac_t src  = {0x00, 0x00, 0x07, 0x00, 0x01, 0x00};
	bcm_mac_t mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	
	uint32 srcIp  = 0xc0800101;
	uint32 maskIp = 0xffffffff;
	bcm_field_entry_create(unit, ACL_BASE_GROUP_INDEX+hitbit, &ent_elk);

	switch (hitbit) {
	case HIT_BIT_1_QUAL:
		rv = bcm_field_qualify_SrcMac(unit, ent_elk, src, mask);
		if (rv) {
		  printf("bcm_field_qualify_SrcMac error\n");
		  return rv;
		}
		rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue1Set,0x12121212,0x12121212);
		break;
	case HIT_BIT_3_QUAL:
		rv = bcm_field_qualify_EtherType(unit, ent_elk, EthType, 0xFFFF);
		if (rv) {
		  printf("bcm_field_qualify_EtherType error\n");
		  return rv;
		}
		rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue3Set,0x34343434,0x34343434);
		break;
	case HIT_BIT_4_QUAL:
		rv = bcm_field_qualify_SrcIp(unit, ent_elk, srcIp, maskIp);
		if (rv) {
		  printf("bcm_field_qualify_SrcIp error\n");
		  return rv;
		}
		rv = bcm_field_action_add(unit, ent_elk, bcmFieldActionExternalValue4Set,0x56565656,0x56565656);
		break;
	default:
		printf("Unknown hitbit = %d\n",hitbit);
		return -1;
	}
	if (rv) {
	  printf("bcm_field_action_add hit bit =%d error\n",hitbit);
	  return rv;
	}

	rv = bcm_field_entry_prio_set(unit, ent_elk, 10+hitbit);
	if (rv) {
	  printf("bcm_field_entry_prio_set error\n");
	  return rv;
	}

	rv = bcm_field_entry_install(unit, ent_elk);
	if (rv) {
	  printf("bcm_field_entry_install error\n");
	  return rv;
	}

	return rv;
}

int acl_kbp_ipv4uc_ipv4dc_shared_acls_set_configuration_by_results_value(int unit, int hitbit) {
	int rv;
	uint64 val,mask;

	switch (hitbit) {
	case HIT_BIT_1_QUAL:
		/*search#1 - 24 bit */
		COMPILER_64_ZERO(val);
		COMPILER_64_ZERO(mask);
		COMPILER_64_SET(val ,0x00000000,0x00121212);
		COMPILER_64_SET(mask,0x00000000,0x00ffffff);
		rv = configure_pmf_action_by_hit_value_example(unit, HIT_BIT_1_QUAL, val, mask, bcmFieldAppTypeIp4Ucast, bcmFieldAppTypeIp4DoubleCapacity, bcmFieldActionDrop, 0, 0, 0);
		if (rv) {
			printf("configure_pmf_action_by_hit_value_example bit 1 bcmFieldAppTypeIp4Ucast error\n");
			return  rv;
		}
		break;
	case HIT_BIT_3_QUAL:
		/*search#3 - 16 bit */
		COMPILER_64_ZERO(val);
		COMPILER_64_ZERO(mask);
		COMPILER_64_SET(val ,0x00000000,0x00003434);
		COMPILER_64_SET(mask,0x00000000,0x0000ffff);
		rv = configure_pmf_action_by_hit_value_example(unit, HIT_BIT_3_QUAL, val, mask, bcmFieldAppTypeIp4Ucast, bcmFieldAppTypeIp4DoubleCapacity, bcmFieldActionDrop, 0, 0, 0);
		if (rv) {
			printf("configure_pmf_action_by_hit_value_example bit 3 bcmFieldAppTypeIp4Ucast error\n");
			return  rv;
		}
		break;
	case HIT_BIT_4_QUAL:
		/*search#4 - 32 bit */
		COMPILER_64_ZERO(val);
		COMPILER_64_ZERO(mask);
		COMPILER_64_SET(val ,0x00000000,0x00000000);
		COMPILER_64_SET(mask,0x00000000,0x0000ffff);
		rv = configure_pmf_action_by_hit_value_example(unit, HIT_BIT_3_QUAL, val, mask, bcmFieldAppTypeIp4Ucast, bcmFieldAppTypeIp4DoubleCapacity, bcmFieldActionDrop, 0, 0, 20);
		if (rv) {
			printf("configure_pmf_action_by_hit_value_example bit 3 bcmFieldAppTypeIp4Ucast error\n");
			return  rv;
		}
		COMPILER_64_ZERO(val);
		COMPILER_64_ZERO(mask);
		COMPILER_64_SET(val ,0x00000000,0x56565656);
		COMPILER_64_SET(mask,0x00000000,0xffffffff);
		rv = configure_pmf_action_by_hit_value_example(unit, HIT_BIT_4_QUAL, val, mask, bcmFieldAppTypeIp4Ucast, bcmFieldAppTypeIp4DoubleCapacity, bcmFieldActionDrop, 0, 0, 0);
		if (rv) {
			printf("configure_pmf_action_by_hit_value_example bit 4 bcmFieldAppTypeIp4Ucast error\n");
			return  rv;
		}
		break;
	default:
		printf("Unknown hit bit %d\n",hitbit);
		return -1;
	}
	return rv;
}


/*
*   add an ACL entry according to the test: AT_Cint_external_frwd_kbp_mpls_acl_traffic
*/ 
int frwd_kbp_mpls_acls_kbp_entry_add_example(int unit, int index) {

   int i, rv;
   bcm_field_entry_t ent_elk;

   bcm_mac_t d_mac = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x11 };
   bcm_mac_t d_mac_mask = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

   bcm_vlan_t vlan = 102;
   bcm_vlan_t vlan_mask = 0xFFF;

   bcm_field_entry_create(unit, ACL_BASE_GROUP_INDEX + index, &ent_elk);

   switch (index) {
      case 1:/* 1st: dmac */
         rv = bcm_field_qualify_DstMac(unit, ent_elk, d_mac, d_mac_mask);
         if (rv) {
            printf("bcm_field_qualify_DstMac error, index = %d\n", index);
            return rv;
         }
         break;
      case 3:/* 2nd: vlan */
         rv = bcm_field_qualify_OuterVlanId(unit, ent_elk, vlan, vlan_mask);
         if (rv) {
            printf("bcm_field_qualify_OuterVlanId error, index = %d\n", index);
            return rv;
         }
         break;
      default:
         printf("Unknown index value = %d",index);
         return -1;
   }

   rv = bcm_field_entry_prio_set(unit, ent_elk, 10);
   if (rv) {
      printf("bcm_field_entry_prio_set error\n");
      return rv;
   }

   rv = bcm_field_entry_install(unit, ent_elk);
   if (rv) {
      printf("bcm_field_entry_install error\n");
      return rv;
   }

   return rv;
}

/*
 * configure MPLS ACL with 2 lookups: 
 * 1 - dmac
 * 2 - vlan
*/
int frwd_kbp_mpls_acls_kbp_config_example(int unit) {

   int rv=0;

   int presel_id_elk;
   bcm_field_group_config_t grp_elk;

   rv = bcm_field_presel_create(unit, &presel_id_elk);
   if (rv) {
      printf("bcm_field_presel_create error\n");
      return rv;
   }

   rv = bcm_field_qualify_Stage(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageExternal);
   if (rv) {
      printf("bcm_field_qualify_Stage error\n");
      return rv;
   }

   rv = bcm_field_qualify_AppType(unit, presel_id_elk | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeMpls);
   if (rv) {
      printf("bcm_field_qualify_AppType Mpls error\n");
      return rv;
   }

   bcm_field_group_config_t_init(&grp_elk); 
   BCM_FIELD_PRESEL_ADD(grp_elk.preselset, presel_id_elk);
   grp_elk.mode = bcmFieldGroupModeAuto;
   grp_elk.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | 
BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;

   BCM_FIELD_QSET_INIT(grp_elk.qset);
   BCM_FIELD_ASET_INIT(grp_elk.aset);
   BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyStageExternal);

   /* 1st: dmac */
   BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyDstMac);
   BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue1Set);
   grp_elk.group = ACL_BASE_GROUP_INDEX + 1;
   grp_elk.priority = ACL_BASE_GROUP_PRIORITY;

   rv = bcm_field_group_config_create(unit, &grp_elk);
   if (rv) {
      printf("bcm_field_group_config_create 1 error\n");
      return rv;
   }

   /* 2nd: vlan */
   BCM_FIELD_ASET_REMOVE(grp_elk.aset, bcmFieldActionExternalValue1Set);
   BCM_FIELD_ASET_ADD(grp_elk.aset, bcmFieldActionExternalValue3Set);
   BCM_FIELD_QSET_REMOVE(grp_elk.qset, bcmFieldQualifyDstMac);
   BCM_FIELD_QSET_ADD(grp_elk.qset, bcmFieldQualifyOuterVlanId);
   grp_elk.group = ACL_BASE_GROUP_INDEX + 3;
   grp_elk.priority++;

   rv = bcm_field_group_config_create(unit, &grp_elk);
   if (rv) {
      printf("bcm_field_group_config_create 3 error\n");
      return rv;
   }

   /* Initialize the KBP */
   bshell(unit, "kbp init_appl");

   return rv;
}

/*
 * set configuration of KBP tables and PMF dropping action 
*/
int frwd_kbp_mpls_acls_kbp_set_configurations(int unit) {
   int rv;

   rv = frwd_kbp_mpls_acls_kbp_config_example(unit);
   if(rv) {
      printf("acl_kbp_mpls_acls_kbp_config_example error\n");
      return  rv;
   }

   rv = configure_pmf_action_by_hit_bit_example(unit, HIT_BIT_1_QUAL, bcmFieldAppTypeMpls, bcmFieldActionDrop, 0, 
0);
   if(rv) {
      printf("configure_pmf_action_by_hit_bit_example bit 1 error\n");
      return  rv;
   }

   rv = configure_pmf_action_by_hit_bit_example(unit, HIT_BIT_3_QUAL, bcmFieldAppTypeMpls, bcmFieldActionDrop, 0, 
0);
   if(rv) {
      printf("configure_pmf_action_by_hit_bit_example bit 3 error\n");
      return  rv;
   }

   return rv;
}

