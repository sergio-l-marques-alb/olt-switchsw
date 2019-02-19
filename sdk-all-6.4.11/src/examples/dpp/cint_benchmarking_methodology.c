/*
 * $Id: cint_benchmarking_methodology.c v 1.0 Exp $
 *
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
 * File: cint_benchmarking_methodology.c
 * Purpose: Example Usage of egress programs used for benchmarking methedology (RFC-2544)
 */

/*
  Below is an example of how to use the egress editor programs specifically 
  designed for devices operating as reflectors 
  when set up in the following configuration: 
 
       +----------+         +-------------+        +-----------+
       |sender/   |-------->|  device(s)  |------->|           |
       |receiver  |         |    under    |        | reflector |
       |          |<--------|  testing    |<-------|           |
       +----------+         +-------------+        +-----------+

 
Usage: 
1) 
    One of the soc properties
		RFC2544_reflector_mac_swap_port 
		RFC2544_reflector_mac_and_ip_swap_port 
    should be set to the reflector port. The reflector port must be defined as an Ethernet port.
2) 
    Load the cints
    utility/cint_utils_vlan.c
    cint_benchmarking_methodology.c
    cint_advanced_vlan_translation_mode.c
    and call
    setup_port_for_reflector_program(unit, selection_port,reflector_port).
    At this stage all trafic coming from port "selection_port" will be have the out-TM port changed to the reflector_port.
	The out-PP port will remain as it was originally.
3) 
    The reflector port should be defined as recycle port with the soc property
    ucode_port_[reflector port]=RCY
    The packet will enter the recycle interface with a PTCH with the injected port indication as the original Out-PP-Port
3) 
    Set up L2/L3 configurations on the out-PP port.
4) 
	Send traffic from port "selection_port" 
 *  
 */





/* Egress PMF redirection isn't supported in Jericho due to the double
   core. need to check the device type in order to use StatVport instead. */
int device_is_jer(int unit, uint8* is_jer)
{
  bcm_info_t info;

  int rv = bcm_info_get(unit, &info);
  if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
  }

  *is_jer = ((info.device == 0x8375) || (info.device == 0x8675));
  return rv;
}




/**
 * Function sets up an egress PMF rule changing the Out-TM-Port 
 * of all traffic coming from selection_port to reflector_port.
 *  Runs in advanced vlan editing mode.
 * 
 * @author sinai (07/04/2014)
 * 
 * @param unit 
 * @param selection_port 
 * @param reflector_port 
 * 
 * @return int 
 */
int setup_port_for_reflector_program(int unit, int selection_port, int reflector_port, int vlan_port) {
    int rv;
    char *proc_name ;
    bcm_vlan_port_t vp;

    proc_name = "setup_port_for_reflector_program" ;
    printf("%s(): Enter\r\n",proc_name) ;

    bcm_vlan_port_t_init(& vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = vlan_port;
    vp.match_vlan = 10;
    vp.egress_vlan = 10;
    vp.vsi = 100; 

    uint8 is_jer;

    rv = device_is_jer(unit, &is_jer);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    g_eve_edit_utils.edit_profile =0;



    rv = vlan__eve_default__set(unit, vp.vlan_port_id, 10,BCM_VLAN_NONE,bcmVlanActionReplace,bcmVlanActionNone);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }


    int counter_proc;
    int mod_id = 0;
    int port;
    int pri = 3;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_field_stat_t stats[2];

    unsigned long destination_gport_as_long, statId_as_long ;

    int statId = 1000 ; /* Set internal data */

    /*int local_gport = 317;*/
    uint64 value;
    bcm_gport_t destination_gport, selection_gport;
    int outlif ;
    int cores_num, ii ;
    bcm_field_action_core_config_t core_config_arr[2] ;
    int core_config_arr_len ;

/* qset */
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);

    BCM_FIELD_ASET_INIT(aset);
    if (!is_jer) {
        /*
         * Another Traffic-Management OutPort is set to so that the packet will be
         * selected by the reflector port and sent to the recycling interface.
         * However, the same Packet processing is  performed on the packet,
         * according to the original Out-PP-Port.
         * The  bcmFieldActionRedirect Field action modifies only the
         * Traffic-Management OutPort. It is in general coupled at egress with
         * bcmFieldActionStat to modify also the Out-PP-Port.
         */
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
    }
    else {
        /*
         * Jericho's double core makes it impossible to use redirect.
         * Using StatVportNew to redirect packets symmetrically on both
         * cores, assuming there isn't a need to jump core.
        */
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionStatVportNew);

    }

/* create database */
    rv =  bcm_field_group_create(unit, qset, pri, &group);
    if (rv != BCM_E_NONE) {
        printf("field group create: (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_group_action_set(unit, group, aset);  
    if (rv != BCM_E_NONE) {
        printf("field group action set: (%s) \n",bcm_errmsg(rv));
        return rv;
    }

/* adding entry in database */
    rv =  bcm_field_entry_create(unit, group, &entry);
    if (rv != BCM_E_NONE) {
        printf("field entry create \n",bcm_errmsg(rv));
        return rv;
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(selection_gport,selection_port);
    rv = bcm_field_qualify_SrcPort(unit, entry, 0, 0, selection_gport, 0xffffffff);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_SrcPort\n");
      return result;
    }

    printf("7777777777777777777777777\n");

    BCM_GPORT_SYSTEM_PORT_ID_SET(destination_gport, reflector_port);
    destination_gport_as_long = destination_gport ;
    statId_as_long = statId ;
    if (!is_jer) {
        /*
         * Enter for ARAD PLUS and below
         */
        printf("%s(): ARAD: Call bcm_field_action_add(): bcmFieldActionRedirect, module %d destination_gport %d\n",proc_name,0,destination_gport) ;

        rv = bcm_field_action_add(unit, entry, bcmFieldActionRedirect, 0, destination_gport);
        if (rv != BCM_E_NONE) {
            printf("%s(): field action add: %s\n",proc_name,bcm_errmsg(rv));
            return rv;
        }
    }
    else {
        /*
         * Enter for JERICHO and up
         */
        cores_num = sizeof(core_config_arr) / sizeof(core_config_arr[0]);
        outlif = 100 ;
        printf("%s(): JERICHO: Call bcm_field_action_config_add(): bcmFieldActionStat\n",proc_name) ;
        printf("==> entry %d statId %d (0x%08lX) destination_gport %d (0x%08lX) outlif %d\n",entry,statId,statId_as_long,destination_gport,destination_gport_as_long,outlif) ;

        for (ii = 0 ; ii < cores_num ; ii++) {
          core_config_arr[ii].param0 = statId ;
          core_config_arr[ii].param1 = destination_gport ;
          core_config_arr[ii].param2 = outlif ;
        }

        core_config_arr_len = cores_num ;
        rv = bcm_field_action_config_add(unit, entry, bcmFieldActionStat, core_config_arr_len, &core_config_arr[0]) ;
        if (rv != BCM_E_NONE) {
            printf("%s(): bcm_field_action_config_add() bcmFieldActionStat: %s\n",proc_name,bcm_errmsg(rv));
            return rv;
        }
        /*
         * For JERICHO, this interface action (bcmFieldActionStatVportNew) is not valid any more:
         * It is replaced by the third parameter on action bcmFieldActionStat and by calling
         * bcm_field_action_config_add()
         */
/*
        rv = bcm_field_action_add(unit, entry, bcmFieldActionStatVportNew, statId, 100);
        if (rv != BCM_E_NONE) {
            printf("field action add bcmFieldActionStatVportNew: %s\n",bcm_errmsg(rv));
            return rv;
        }
*/
    }


    rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("field group install: \n",bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Exit OK\r\n",proc_name) ;
    return 0;
}

