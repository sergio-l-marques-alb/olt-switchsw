/*
* $Id: cint_field_policer_redirect_per_l2_l3_addresses.c,v 1.5 Broadcom SDK $
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
*
* 
*/
/*
 *  Create a bcm_field_group_t, then invoke field_api_setup(unit, prio, &group).
 *  Perform any frame testing desired.  Invoke field_api_teardown(unit, group).
 *
 *  Modify field_api_setup to fit your needs, or perhaps clone it several times
 *  for a number of different groups.  You can put more than one entry in each
 *  group, so each clone could set up several entries.
 */
 
 /* utilitry to define trap gport (toward CPU)
  *  - *trap_gport is out param, includes gport point to allocated trap-port
  */
 
int set_cpu_trap(int unit, int *trap_gport){

    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    int flags = 0;
    int trap_id;
    int cpu_port = 1;

  bcm_rx_trap_config_t_init(&config);

  /*for port dest change*/
  config.flags |= (BCM_RX_TRAP_UPDATE_DEST|BCM_RX_TRAP_REPLACE); 
  config.trap_strength = 0; /*FOR USER DEFINE set as part of destination */
  config.dest_port=cpu_port; /* cpu port */

  rv = bcm_rx_trap_type_create(unit,flags,bcmRxTrapUserDefine,&trap_id);
  if (rv != BCM_E_NONE) {
        printf("Error, in trap create, trap id $trap_id \n");
        return rv;
  }


  rv = bcm_rx_trap_set(unit,trap_id,&config);
  if (rv != BCM_E_NONE) {
      printf("Error, in trap set \n");
      return rv;
  }

  /* encode trap as gport */   
  BCM_GPORT_TRAP_SET(*trap_gport, trap_id, 7, 0);

  return rv;
}

  /* 
   * setup FP group 
   * key: SA,DA, SIP
   * action: meter, redirect
   * entry-id selects which entry to add to Group
   *  entry 1: 
   *   - SA = EE:EE:EE:AA:AA:AA 
   *   - DA = CB:A9:87:65:43:21
   *   - SIP: 204.*.*.* / 8
   *   action -> redirect to port 13
   *  entry 2: 
   *   - SA = **:**:EE:AA:AA:**
   *   - DA = CB:A9:87:65:43:21
   *   - SIP: 204.204.204.204
   *   action -> redirect to flow 154 (port 15, + 2)
   *  entry 3: 
   *   - SA = EE:EE:EE:AA:AA:AA 
   *   - DA = CB:A9:87:65:43:21
   *   - SIP: 204.204.204.204
   *   action -> redirect trap to CPU + meter = 100
   */

int field_api_setup(/* in */ int unit,
                    /* in */ int entry_id,
                    /* in */ int group_priority,
                    /* out */ bcm_field_group_t *group) {
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_entry_t ent[3];
  bcm_field_group_t grp;
  bcm_mac_t macData;
  
  /* for redirect to sys-port */
  int redirect_sys_port = 1;
  int sys_gport;

 /* for redirect to flow */
  int redirect_flow = 154;
  int flow_gport;
  
 /* 
  * for redirect to FEC - requires to run: 
  * - utility/cint_utils_l3.c    
  * - cint_mpls_lsr.c 
  * - and cint_vswitch_cross_connect_p2p.c 
  */
  int redirect_fec_id = 1026;
  int fec_gport;
  
  /* for meter + trap */
  int meter_id = 100;
  int trap_gport = 0;

  bcm_mac_t macMask;
  bcm_field_stat_t stats[2];
  int statId;


  result = set_cpu_trap(0,&trap_gport);
  if (BCM_E_NONE != result) {
    return result;
  }

  if (NULL == group) {
    printf("Pointer to group ID must not be NULL\n");
    return BCM_E_PARAM;
  }

  /*
   *  Build qualifier set for the group we want to create.  Entries must exist
   *  within groups, and groups define what kind of things the entries can
   *  match, by their inclusion in the group's qualitier set.
   *
   *  This example demonstrate L2/L3 packets attributes.
   *  fields of interest for L2 work.  Right now, the field APIs do not separate
   *  frames according to their type.
   *  this example assume injected traffic is IPv4-uc.
   *  in future implementation user will be able point to this database only for IPv4-UC traffic
   *
   *  Also, we must specify stage ingress or egress.
   * 
   *  Note that only qualifiers specified in the QSET can be used by any of the
   *  entries in the group.  See the output from bcm_field_show(unit,"") for a
   *  list of the supported qualifiers per stage.
   */
  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcMac);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcIp);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);

  /*
   *  Build action set for the group we want to create.  Entries can only do
   *  something that is included on this list.
   *
   *  See the output from bcm_field_show(unit,"") for a list of the supported
   *  actions per stage.
   */
  BCM_FIELD_ASET_INIT(aset);
  
  /* redirect can update destination to new port, flow, or trap-destination */
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
  /* policer level 0, can attach two policers per packet, 2nd is not implemented! */
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel0);

  /*
   *  Create the group and set is actions.  This is two distinct API calls due
   *  to historical reasons.  On soc_petra, the action set call is obligatory, and
   *  a group must have at least one action in its action set before any entry
   *  can be created in the group.
   */
  result = bcm_field_group_create(unit, qset, group_priority, &grp);
  if (BCM_E_NONE != result) {
    return result;
  }
  result = bcm_field_group_action_set(unit, grp, aset);
  if (BCM_E_NONE != result) {
    auxRes = bcm_field_group_destroy(unit, grp);
    return result;
  }



  if((entry_id & 1) == 1){
      /*
       *  entry 1: 
       *   - SA = EE:EE:EE:AA:AA:AA 
       *   - DA = CB:A9:87:65:43:21
       *   - SIP: 204.*.*.* / 8
       *   action -> redirect to port 13
       */
       
       
      result = bcm_field_entry_create(unit, grp, &ent[0]);
      if (BCM_E_NONE != result) {
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }
         /* SA */
      macData[0] = 0xaa;
      macData[1] = 0xaa;
      macData[2] = 0xaa;
      macData[3] = 0xee;
      macData[4] = 0xee;
      macData[5] = 0xee;
      macMask[0] = 0xff;
      macMask[1] = 0xff;
      macMask[2] = 0xff;
      macMask[3] = 0xff;
      macMask[4] = 0xff;
      macMask[5] = 0xff;
      print macMask;
      result = bcm_field_qualify_SrcMac(unit, ent[0], macData, macMask);
      if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent[0]);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }
      /* DA */
      macData[0] = 0xcb;
      macData[1] = 0xa9;
      macData[2] = 0x87;
      macData[3] = 0x65;
      macData[4] = 0x43;
      macData[5] = 0x21;
      macMask[0] = 0xff;
      macMask[1] = 0xff;
      macMask[2] = 0xff;
      macMask[3] = 0xff;
      macMask[4] = 0xff;
      macMask[5] = 0xff;
      print macMask;
      result = bcm_field_qualify_DstMac(unit, ent[0], macData, macMask);
      if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent[0]);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }

      /* SIP */
      result = bcm_field_qualify_SrcIp(
          unit, 
          ent[0], 
          0xcc000000, 
          0xff000000);

      if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent[0]);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }
      
      
      /* add entry to redircet to port */
      
      BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, redirect_sys_port);
      result = bcm_field_action_add(unit, ent[0], bcmFieldActionRedirect, 0 , sys_gport);
      if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent[0]);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }
  
 /*
  * entry 1. }
  */
  }


  /*
   *  entry 2: 
   *   - SA = **:AA:AA:EE:**:**
   *   - DA = CB:A9:87:65:43:21
   *   - SIP: 204.204.204.204
   *   action -> redirect to flow 154 (port 15, + 2)
   *   + meter = 100
   */

  if((entry_id & 2) == 2){

      result = bcm_field_entry_create(unit, grp, &ent[1]);
      if (BCM_E_NONE != result) {
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }

      /* SA */
      macData[0] = 0x00;
      macData[1] = 0xaa;
      macData[2] = 0xaa;
      macData[3] = 0xee;
      macData[4] = 0x00;
      macData[5] = 0x00;
      macMask[0] = 0x00;
      macMask[1] = 0xff;
      macMask[2] = 0xff;
      macMask[3] = 0xff;
      macMask[4] = 0x00;
      macMask[5] = 0x00;
      print macMask;
      result = bcm_field_qualify_SrcMac(unit, ent[1], macData, macMask);
      if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent[1]);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }
      /* DA */
      macData[0] = 0xcb;
      macData[1] = 0xa9;
      macData[2] = 0x87;
      macData[3] = 0x65;
      macData[4] = 0x43;
      macData[5] = 0x21;
      macMask[0] = 0xff;
      macMask[1] = 0xff;
      macMask[2] = 0xff;
      macMask[3] = 0xff;
      macMask[4] = 0xff;
      macMask[5] = 0xff;
      print macMask;
      result = bcm_field_qualify_DstMac(unit, ent[1], macData, macMask);
      if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent[1]);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }

      /* SIP */
      result = bcm_field_qualify_SrcIp(
          unit, 
          ent[1], 
          0xcccccccc, 
          0xffffffff);

      if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent[1]);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }


      /* add entry to redircet to flow */
       BCM_GPORT_UNICAST_QUEUE_GROUP_SET(flow_gport,redirect_flow);
       result = bcm_field_action_add(unit, ent[1], bcmFieldActionRedirect, 0 , flow_gport);
       if (BCM_E_NONE != result) {
         auxRes = bcm_field_entry_destroy(unit, ent[1]);
         auxRes = bcm_field_group_destroy(unit, grp);
         return result;
       }

      result = bcm_field_action_add(unit, ent[2], bcmFieldActionPolicerLevel0, meter_id, 0);
      if (BCM_E_NONE != result) {
          auxRes = bcm_field_entry_destroy(unit, ent[1]);
          auxRes = bcm_field_group_destroy(unit, grp);
          return result;
      }

 /*
  * entry 2. }
  */
  
  } 
  

  /*
   *  entry 4: 
   *   - SIP: 204.204.204.204
   *   action -> redirect to FEC 1026 
   */

  if((entry_id & 4) == 4){

      result = bcm_field_entry_create(unit, grp, &ent[1]);
      if (BCM_E_NONE != result) {
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }

      /* SIP */
      result = bcm_field_qualify_SrcIp(
          unit, 
          ent[1], 
          0xcccccccc, 
          0xffffffff);

      if (BCM_E_NONE != result) {
        auxRes = bcm_field_entry_destroy(unit, ent[1]);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
      }


      /* add entry to redircet to flow */
      BCM_GPORT_VLAN_PORT_ID_SET(fec_gport, redirect_fec_id);
      result = bcm_field_action_add(unit, ent[0], bcmFieldActionRedirect, 0 , fec_gport);
       if (BCM_E_NONE != result) {
         auxRes = bcm_field_entry_destroy(unit, ent[1]);
         auxRes = bcm_field_group_destroy(unit, grp);
         return result;
       }

 /*
  * entry 4. }
  */

  } 
   /* add entry to redircet to fec_id */


  /*
   *  Commit the entire group to hardware.  We could do individual entries,
   *  but since we just created the group here and want the entry/entries all
   *  in hardware at this point, it's a bit quicker to commit the whole group.
   */
  result = bcm_field_group_install(unit, grp);
  if (BCM_E_NONE != result) {
    auxRes = bcm_field_entry_destroy(unit, ent);
    /* auxRes = bcm_field_stat_destroy(unit, statId);*/
    auxRes = bcm_field_group_destroy(unit, grp);
    return result;
  }
  
  /*
   *  Everything went well; return the group ID that we allocated earlier.
   */
  *group = grp; 

  return result;
}

int field_api_entry_add(/* in */ int unit,
                    /* in */ bcm_field_group_t grp, uint8 mac_lsb)
{
  int result;
  int auxRes;
  bcm_field_entry_t ent;
  bcm_mac_t macData;
  bcm_mac_t macMask;
  bcm_field_stat_t stats[2];
  int statId;

  /*
   *  Add an entry to the group (could add many entries).
   *
   *  Relatively basic example here, only drop and count.  We could also take
   *  other actions (list them in the aset above).  Some actions only take the
   *  BCM canonical form of param0,param1; other actions (such as redirect and
   *  mirror) can take a GPORT as the port argument; such GPORT can be any
   *  valid GPORT in the given context (redirect family of actions the GPORT
   *  can be anything that works out to be a destination type fwd action, same
   *  for mirror).
   *
   *  This example will match Novell NetWare IPX frames sent using the
   *  Ethernet II frame format by certain Broadcom NICs, and count + drop them.
   */
  result = bcm_field_entry_create(unit, grp, &ent);
  if (BCM_E_NONE != result) {
    auxRes = bcm_field_group_destroy(unit, grp);
    return result;
  }
  /* certain Broadcom NICs */
  macData[0] = mac_lsb;
  macData[1] = 0x1A;
  macData[2] = 0xA0;
  macData[3] = 0x00;
  macData[4] = 0x00;
  macData[5] = 0x00;
  macMask[0] = 0xFF;
  macMask[1] = 0xFF;
  macMask[2] = 0xFF;
  macMask[3] = 0x00;
  macMask[4] = 0x00;
  macMask[5] = 0x00;
  result = bcm_field_qualify_SrcMac(unit, ent, macData, macMask);
  if (BCM_E_NONE != result) {
    auxRes = bcm_field_entry_destroy(unit, ent);
    auxRes = bcm_field_group_destroy(unit, grp);
    return result;
  }
  /* NetWare IPX over Ethernet_II 
  result = bcm_field_qualify_EtherType(unit, ent, 0x8137, 0xFFFF);
  if (BCM_E_NONE != result) {
    auxRes = bcm_field_entry_destroy(unit, ent);
    auxRes = bcm_field_group_destroy(unit, grp);
    return result;
  }
*/
  /*
   *  Once we match the frames, let's drop them.
   */
  /* note that param0 and param1 don't care for bcmFieldActionDrop) */
  result = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
  if (BCM_E_NONE != result) {
    auxRes = bcm_field_entry_destroy(unit, ent);
    auxRes = bcm_field_group_destroy(unit, grp);
    return result;
  }

  /*
   *  Let's count the frames that hit the entry
   *
   *  We could be extravagant here and ask for all of the stats supported,
   *  but that would only work if the counter processor is configured to
   *  the full mode (g,y,r drop; g,y forward).  Instead, we only ask for
   *  bytes and packets, which should be available in any counter proc mode
   *  that supports the field processor using counters.
   *
   *  Unlike some devices, the statistics ID is an opaque type that includes
   *  information about the counter processor and set; we can't treat it as
   *  a simple number, and the APIs don't want such mistreatment.
   
  stats[0] = bcmFieldStatPackets;
  stats[1] = bcmFieldStatBytes;
  result = bcm_field_stat_create(unit, grp, 2 num stats , &(stats[0]), &statId);
  if (BCM_E_NONE != result) {
    auxRes = bcm_field_entry_destroy(unit, ent);
    auxRes = bcm_field_group_destroy(unit, grp);
    return result;
  }*/
  

  /*
   *  Commit the entire group to hardware.  We could do individual entries,
   *  but since we just created the group here and want the entry/entries all
   *  in hardware at this point, it's a bit quicker to commit the whole group.
   */
  result = bcm_field_group_install(unit, grp);
  if (BCM_E_NONE != result) {
    auxRes = bcm_field_entry_destroy(unit, ent);
    /*auxRes = bcm_field_stat_destroy(unit, statId);*/
    auxRes = bcm_field_group_destroy(unit, grp);
    return result;
  }
  
  /*
   *  Everything went well; 
   */
  return result;

}

int field_api_teardown(/* in */ int unit,
                       /* in */ bcm_field_group_t group) {
  int result;
  int auxRes;
  bcm_field_entry_t ent[128];
  int entCount;
  int entIndex;
  int statId;

  do {
    /* get a bunch of entries in this group */
    result = bcm_field_entry_multi_get(unit, group, 128, &(ent[0]), entCount);
    if (BCM_E_NONE != result) {
      return result;
    }
    for (entIndex = 0; entIndex < entCount; entIndex++) {
      /* get statistics for this entry if any */
      result = bcm_field_entry_stat_get(unit, ent[entIndex], &statId);
      if (BCM_E_NONE == result) {
        /* found stat; get rid of it */
        result = bcm_field_entry_stat_detach(unit, ent[entIndex], statId);
        /*
         *  In this loop, we are assuming one stat for each entry.  The API
         *  supports sharing stats between entries (one stat can be used by
         *  mulitple entries) but not the other way (each entry can only
         *  use one stat).  It is possible that the API will return an error
         *  if there is an entry using a stat at the time we ask a stat to
         *  be destroyed.  This can be controlled at compile time on Soc_petra,
         *  but maybe not all devices.
         */
        if (BCM_E_NONE == result) {
          result = bcm_field_stat_destroy(unit, statId);
          if (BCM_E_FULL == result) {
            /* ignore failed attempt to destroy still-in-use stat */
            result = BCM_E_NONE;
          } else if (BCM_E_NONE != result) {
            return result;
          }
        }
      } else if (BCM_E_NOT_FOUND == result) {
        /* no stat, keep going */
        result = BCM_E_NONE;
      } else {
        return result;
      }
      /* remove each entry from hardware and destroy it */
      result = bcm_field_entry_remove(unit, ent[entIndex]);
      if (BCM_E_NONE != result) {
        return result;
      }
      result = bcm_field_entry_destroy(unit, ent[entIndex]);
      if (BCM_E_NONE != result) {
        return result;
      }
    }
    /* all as long as there were entries to remove & destroy */
  } while (entCount > 0);

  /* destroy the group */  
  result = bcm_field_group_destroy(unit, group);

  return result;
}
