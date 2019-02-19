
/* $Id: cint_field_oam_trap_wa.c,v 1.1 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * For OAM applications, work-around to set the egress default trap 
 * on all the OAM packets, unless injected from a specific system-port 
 */

/* 
 *  Define the Egress Field group, with Key: {Src-System-Port, Forwarding-Type, EtherType}
 *  and actions {Redirect, Set internal-data}
 */
int oam_egress_default_trap_setup(/* in */ int unit,
                                  /* in */ int group_priority,
                                  /* in */ bcm_field_group_t group) {
  int result;
  int auxRes;
  bcm_field_qset_t qset;

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOutPort); /* Out-Port */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardingType); /* Forwarding-Type */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpType); /* EtherType */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort); /* Src-System-Port */

  /*
   *  Create the group
   */
  result = bcm_field_group_create_id(unit, qset, group_priority, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
      return result;
  }
  printf("After bcm_field_group_create_id\n");
 
  /*
   *  Commit the entire group to hardware.  
   */
  result = bcm_field_group_install(unit, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_install\n");
    return result;
  }
  printf("After bcm_field_group_install\n");
  return result;
}

/* 
 *  Add a new rule: Redirect to the current OutPort if OAM packet
 *  Internal-Data selects a special egress editor program.
 */
int oam_egress_default_trap_entry_add(/* in */ int unit,
                                      /* in */ bcm_field_group_t group,
                                      /* in */ bcm_port_t out_port) {
  int result;
  int auxRes, ace_var;
  bcm_field_entry_t ent;
  int statId;
  bcm_gport_t local_gport;
  bcm_gport_t mirror_gport;

  result = bcm_field_entry_create(unit, group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  /* Set the priority of these entries below the other entries */
  result = bcm_field_entry_prio_set(unit, ent, 10);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_prio_set\n");
    return result;
  }

  result = bcm_field_qualify_OutPort(unit, ent, out_port, -1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_OutPort\n");
    return result;
  }

  result = bcm_field_qualify_ForwardingType(unit, ent, bcmFieldForwardingTypeL2);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_ForwardingType\n");
      return result;
  }

  result = bcm_field_qualify_IpType(unit, ent, bcmFieldIpTypeCfm /* OAM EtherType */);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_EtherType\n");
    return result;
  }

  /* Redirect to same out-port */
  BCM_GPORT_LOCAL_SET(local_gport, out_port);
  result = bcm_field_action_add(unit, ent, bcmFieldActionRedirect, 0, local_gport);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_action_add\n");
      return result;
  } 

  /* mirror to default trap destination */
  BCM_GPORT_MIRROR_SET(mirror_gport, 5);
  result = bcm_field_action_add(unit, ent, bcmFieldActionMirrorEgress, 0, mirror_gport);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_action_add\n");
      return result;
  }

  /* configure default trap destination port */

  bcm_rx_trap_config_t trap_config;
  BCM_GPORT_LOCAL_SET(trap_config.dest_port, 0);
  trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP | BCM_RX_TRAP_REPLACE;
  result = bcm_rx_trap_set(unit, 264, &trap_config);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_rx_trap_set\n");
      return result;
  }

  /*
   * Due to HW reasons, the Counter-ID can be only at egress between 1024 and 
   * 3839. Besides, the user must re-indicate the redirect gport as 
   * second parameter. 
   *  
   * When redirecting at egress without counting, the user must still 
   * set the stat function with stat-id equal to 0 (do not count) 
   */
  ace_var = (0x2 << 8);
  statId = (ace_var << 19); /* Set internal data */
  
  result = bcm_field_action_add(unit, ent, bcmFieldActionStat, statId, local_gport);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_action_add\n");
    return result;
  }

  /*
   *  Commit the entire group to hardware.  We could do individual entries,
   *  but since we just created the group here and want the entry/entries all
   *  in hardware at this point, it's a bit quicker to commit the whole group.
   */
  result = bcm_field_group_install(unit, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_install\n");
    return result;
  }
  return result;
}

/* 
 *  Add a new rule: Do nothing if injecting from source system port configure in "custom_feature_oam_eg_fake_injection_port"
 */
int oam_egress_skip_oam_port_entry_add(/* in */ int unit,
                                       /* in */ bcm_field_group_t group) {
  int result;
  int auxRes;
  bcm_field_entry_t ent;
  bcm_gport_t injection_sys_port;

  result = bcm_field_entry_create(unit, group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  /* Set the priority of these entries above the other entries */
  result = bcm_field_entry_prio_set(unit, ent, 20);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_prio_set\n");
    return result;
  }

  /* configure a designated "injection" system port. In this example, packets at the egress with source port
   * 254 will not be trapped by the default trap, because they are treated as injected. Replace 254 with the port configured in 
   * "custom_feature_oam_eg_fake_injection_port" 
   */
  BCM_GPORT_SYSTEM_PORT_ID_SET(injection_sys_port,254);
  result = bcm_field_qualify_SrcPort(unit, ent, 0, 0, injection_sys_port, 0xffffffff);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_SrcPort\n");
    return result;
  }

  /* Do nothing - no action */

  /*
   *  Commit the entire group to hardware.  We could do individual entries,
   *  but since we just created the group here and want the entry/entries all
   *  in hardware at this point, it's a bit quicker to commit the whole group.
   */
  result = bcm_field_group_install(unit, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_install\n");
    return result;
  }
  return result;
}

/*
 *  Set the previous configurations for an example configuration
 */
int oam_egress_default_trap(int unit) 
{
  int result;
  int out_port;
  int group_priority[2] = {26, 27}, group[2] = {12, 13};

  result =  oam_egress_default_trap_setup(unit, group_priority[0], group[0]);
  if (BCM_E_NONE != result) {
      printf("Error in oam_egress_default_trap_setup\n");
      return result;
  }

  /* 
   * Add an entry matching OAM packets exiting through port out_port 
   * The user must run over its local ports. 
   * In the example, run over ports 0 and 1 
   */
  for (out_port = 13; out_port < 15; out_port++) {
      result =  oam_egress_default_trap_entry_add(unit, group[0], out_port);
      if (BCM_E_NONE != result) {
          printf("Error in oam_egress_default_trap_entry_add\n");
          return result;
      }
  }

  /* 
   * Skip the egress trap setting for OAM packets if 
   * the packet has a bypass-filter bit set
   */
  result =  oam_egress_skip_oam_port_entry_add(unit, group[0]);
  if (BCM_E_NONE != result) {
      printf("Error in oam_egress_skip_oam_port_entry_add\n");
      return result;
  }

  return result;
}



