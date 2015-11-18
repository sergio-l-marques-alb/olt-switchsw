
/* $Id: cint_field_egress_modify_tc_per_port.c,v 1.6 Broadcom SDK $
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
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$3
*/

/*
 *  Create a bcm_field_group_t, then invoke field_api_setup(unit, prio, &group).
 *  Perform any frame testing desired.  Invoke field_api_teardown(unit, group).
 *
 *  Modify field_api_setup to fit your needs, or perhaps clone it several times
 *  for a number of different groups.  You can put more than one entry in each
 *  group, so each clone could set up several entries.
 */
int egress_acl_add(int unit, bcm_field_group_t group, int group_priority, bcm_port_t out_port, uint32 counter_id, uint32 counter_proc){
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  bcm_field_entry_t ent;
  bcm_field_aset_t aset;
  bcm_gport_t local_gport;
  bcm_field_stat_t stats[2];
  int statId;

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOutPort);

  /*
   *  Create the group and set is actions.  This is two distinct API calls due
   *  to historical reasons.  On soc_petra, the action set call is obligatory, and
   *  a group must have at least one action in its action set before any entry
   *  can be created in the group.
   */
  result = bcm_field_group_create_id(unit, qset, group_priority, group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
    return result;
  }

  result = bcm_field_entry_create(unit, group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
    return result;
  }

  result = bcm_field_qualify_OutPort(unit, ent, out_port, -1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_OutPort\n");
    return result;
  }

  result = bcm_field_action_add(unit, ent, bcmFieldActionPrioIntNew, 5, 0);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_action_add\n");
    return result;
  }

  /* Redirect to port 1 */
  BCM_GPORT_LOCAL_SET(local_gport, out_port);
  result = bcm_field_action_add(unit, ent, bcmFieldActionRedirect, 0, local_gport);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_action_add\n");
    return result;
  }

  /*
   * The stat-id is an encoding of the HW Counter-ID and the HW 
   * Counter-Processor (aka Counter-Engine). 
   * Due to HW reasons, the Counter-ID can be only at egress between 1024 and 
   * 3839. Besides, the user must re-indicate the redirect gport as 
   * second parameter. 
   *  
   * When redirecting at egress without counting, the user must still 
   * set the stat function with stat-id equal to 0 (do not count) 
   */
  statId = (counter_id) + (counter_proc << 29 /* _BCM_PETRA_CTR_PROC_SHIFT */);
  
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
  
  return BCM_E_NONE;
}