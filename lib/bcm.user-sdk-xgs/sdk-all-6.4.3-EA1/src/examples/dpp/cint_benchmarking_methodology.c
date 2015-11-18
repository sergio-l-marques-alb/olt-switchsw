/*
 * $Id: cint_benchmarking_methodology.c v 1.0 Exp $
 *
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
 * File: cint_benchmarking_methodology.c
 * Purpose: Example Usage of egress programs used for benchmarking methedology (RFC-2544)
 */

/*
  Below is an example of how to use the egress editor programs specifically 
  designed for devices operating as reflectors 
  when set up in the following configuration: 
 
          +----------+            +-------------+          +-----------+
          |sender/ |-------->|  device(s)  |------->|              |
         |receiver|            |    under     |           | reflector |
        |           |<--------|     testing   |<-------|              |
       +--------+            +-------------+           +-----------+
 
Usage: 
1) 
    One of the soc properties
		RFC2544_reflector_mac_swap_port 
		RFC2544_reflector_mac_and_ip_swap_port 
    should be set to the reflector port. The reflector port must be defined as an Ethernet port.
2) 
    Call
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






/**
 * Function sets up an egress PMF rule changing the Out-TM-Port 
 * of all traffic coming from selection_port to reflector_port.
 *  
 * 
 * @author sinai (07/04/2014)
 * 
 * @param unit 
 * @param selection_port 
 * @param reflector_port 
 * 
 * @return int 
 */
int setup_port_for_reflector_program(int unit, int selection_port, int reflector_port) {
	int rv;

	bcm_vlan_port_t vp;
	bcm_vlan_port_t_init(& vp);
	vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
	vp.port = 14;
	vp.match_vlan = 10;
	vp.egress_vlan = 10;
	vp.vsi = 100; 

	rv = bcm_vlan_port_create(0, &vp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }


	bcm_vlan_action_set_t action;
	bcm_vlan_action_set_t_init(&action);
	action.ot_outer = bcmVlanActionReplace;
	action.new_outer_vlan = 500;

	rv = bcm_vlan_translate_egress_action_add(0, vp.vlan_port_id, BCM_VLAN_NONE, BCM_VLAN_NONE, &action);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
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

	int statId;
	int local_gport = 17;
	uint64 value;
	bcm_gport_t destination_gport, selection_gport;

/* qset */
	BCM_FIELD_QSET_INIT(qset);
	BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);

    BCM_FIELD_ASET_INIT(aset);
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


	BCM_GPORT_SYSTEM_PORT_ID_SET(destination_gport, reflector_port);
	rv = bcm_field_action_add(unit, entry, bcmFieldActionRedirect, 0 , destination_gport);
    if (rv != BCM_E_NONE) {
        printf("field action add: \n",bcm_errmsg(rv));
        return rv;
    }


	rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("field group install: \n",bcm_errmsg(rv));
        return rv;
    }
	return 0;
}

