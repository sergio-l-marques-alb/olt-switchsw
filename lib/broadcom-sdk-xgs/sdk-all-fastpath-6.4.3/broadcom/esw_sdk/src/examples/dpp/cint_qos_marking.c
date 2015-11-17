/*
 * $Id: cint_qos_marking.c,v 1.4 Broadcom SDK $
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
 * Cint cos marking example.
 *
 * Cross connect two ports (denoted @in_port and @out_port), and set @out_port to perform
 * DSCP/EXP marking when the expected traffic (vlan 10) is received from in_port.
 * Set @port to do DSCP marking for ip packets with Ethernet TPID (VLAN) = 10.
 * The packets are expected to be received on in_port.
 * The resulting packets will be sent from out_port.
 *
 * There are two types of calling sequences - basic and advanced.
 * One of the parameters for determining the DSCP/EXP of the new packets
 * is inlif profile.
 * The inlif profile is a property of the lif.
 * In basic mode, the inlif profile is avoided altogether by managing it internally.
 * In basic mode the user sets the port for DSCP/EXP marking and does not
 * need to do anything about the inlif profile.
 * In advanced mode the user manages inlif profiles and sets the required inlif
 * profile for each port.
 * Therefore when setting the DSCP/EXP mapping the advanced user has more control
 * since he/she can control the DSCP/EXP mapping by inlif profile as well as other
 * parameters.
 *
 * Calling sequence:
 *  1. Map drop precedence values to resolved drop precedence values.
 *     The DSCP/EXP marking is done according to resolved drop precedence values which are 1 bit.
 *     However the drop precedence of packets in the device is 2 bits.
 *     This map is used to resolve which 1 bit resolved drop precedence should be
 *     used for each 2 bit drop precedence value.
 *  2. [Basic Mode]
 *       Set the in port to do DSCP/EXP marking.
 *     [Advanced Mode]
 *       Set some in inlif profile to do DSCP/EXP marking and set the inlif profile of the
 *       in port to it.
 *  3. [Basic Mode]
 *       Create a QoS profile and map (tc,resolved-dp,qos_profile) to the desired
 *       EXP and DSCP.
 *     [Advanced Mode]
 *       Create a QoS profile and map (tc,resolved-dp,qos_profile,inlif_profile) to the desired
 *       EXP and DSCP.
 *  5. Set the QoS profile to the created QoS profile.
 *
 * Traffic:
 *   Send IP packets with VLAN=10 and any MAC Destination Address in the Ethernet header.
 *
 * When the expected packets are sent on in_port, the DSCP/EXP value is changed according to the setup.
 *
 * Run script:
 *
 * For simple mode, the following SOC property should be set:
 * bcm886xx_qos_l3_l2_marking=1
 * 
   cint cint_vswitch_cross_connect_p2p.c
   cint cint_qos_marking.c
   cint
   // print qos_map_dscp_marking(@unit,@in_port,@out_port);
   print qos_map_dscp_marking(0,13,200);
 
 * 
 */

int verbose = 3;

struct qos_marking_info_s {
    /* In advanced mode, this inlif_profile will be used for the gport, and will be set for DSCP/EXP marking. */
    int inlif_profile;
    /* The cross connection will be between port pin with vlan vid and port pout and vlan vid+1. */
    int vid; 
    /* If non-zero then the cint uses the simple mode calling sequence. */
    /* Otherwise the advanced mode will be used. */
    int simple_mode;
    /* The IP DSCP will be set to this value for the expected traffic. */
    int new_dscp;
    /* The MPLS EXP will be set to this value for the expected traffic. */
    int new_exp;
    /* The id to use for bcm_qos_map_create. If -1, no id is used (it will be automatically allocated). */
    int qos_profile_id;
};

qos_marking_info_s default_qos_marking_info = {
    1,  /*inlif_profile*/
    10, /*vid*/
    1,  /*simple_mode*/
    5,  /*new_dscp*/
    2,  /*new_exp*/
    1   /*qos_profile_id*/
};

/** 
 * Synopsis:
 *  Configure DSCP/EXP marking on @in_port and cross connect it to @out_port
 *  (for IP/MPLS packets). 
 * 
 * Calling Sequence: 
 *  1. Cross connect (port=in_port, tpid=info.vid) to (port=out_port, tpid=info.vid+1).
 *  2. Set the resolved drop precedence value to 0 for all drop precedence values (0-3).
 *  3. Set the in_port for basic mode (or the inlif for advanced mode) to do DSCP/EXP marking:
 *     [Basic Mode]
 *       Set @in_port to perform DSCP/EXP marking.
 *     [Advanced Mode]
 *       Set the inlif profile of in_port to info.inlif_profile and enable DSCP/EXP marking for this
 *       profile. An inlif profile is a property of the inlif that can be used to set various parameters
 *       for the port such as the one described here (enable DSCP/EXP marking).
 *  4. Create a DSCP/EXP marking profile (denoted by qos_profile), and add the following mappings:
 *     [Basic Mode]
 *       (tc=[0..7], resolved-dp=0, qos_profile) -> (info.new_dscp, info.new_exp)
 *     [Advanced Mode]
 *       (tc=[0..7], resolved-dp=0, qos_profile, inlif_profile) -> (info.new_dscp, info.new_exp)     
 *  5. Set the quality of service profile of out_port to qos_profile (allocated with map_create).
 *  
 * Remarks: 
 *  - The global variable default_qos_marking_info is used for default test info.
 */
int qos_map_dscp_marking(int unit, int in_port, int out_port)
{

    int rv = 0;
    int map_id;
    int in_vlan;
    int out_vlan;
    bcm_port_t gport_in;
    bcm_port_t gport_out;
    int simple;
    int inlif_profile;
    int new_dscp;
    int new_exp;
    int tc_idx;
    int qos_profile_id;
    int qos_create_flags;
    bcm_qos_map_t map;
    qos_marking_info_s info = default_qos_marking_info;

    in_vlan = info.vid;
    out_vlan = in_vlan+1; /* The value is based on the cross_connect function. */
    inlif_profile = info.inlif_profile;
    simple = info.simple_mode;
    new_dscp = info.new_dscp;
    new_exp = info.new_exp;
    qos_profile_id = info.qos_profile_id;

    /* Cross connect in_port,vlan 10 to out_port,vlan 11 */
    if (verbose >= 2) {
        printf("Doing port setup (cross connect port %d vlan %d to port %d vlan %d)...\n", in_port, in_vlan, out_port, out_vlan);
    }

    rv = cross_connect(unit, in_port, &gport_in, out_port, &gport_out, in_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error doing port setup.\n");
        print rv;
        return rv;
    }

    /* Set all DPs (0-3) to Resolved-DP 0. */
    if (verbose >= 2) {
        printf("Setting all DPs to resolved-DP 0.\n");
    }
    rv = bcm_switch_control_set(unit, bcmSwitchColorL3L2Marking, 0);
    if (rv) {
        printf("Error doing bcm_switch_control_set with bcmSwitchColorL3L2Marking.\n");
        print rv;
        return rv;
    }

    if (simple) {
        /* Set DSCP/EXP marking for gport */
        if (verbose >= 2) {
            printf("Setting the gport 0x%x to do dscp/exp marking.\n", gport_in, inlif_profile);
        }

        rv = bcm_port_control_set(unit, gport_in, bcmPortControlEgressModifyDscp, 1);
        if (rv != BCM_E_NONE) {
            printf("Error doing bcm_port_control_set(0,0x%x,bcmPortControlEgressModifyDscp,1).\n", gport_in);
            print rv;
            return rv;
        }
    } else {
        /* Set DSCP/EXP marking for inlif profile, and set the inlif profile to the gport */
        if (verbose >= 2) {
            printf("Setting the inlif profile of gport 0x%x to inlif profile %d.\n", gport_in, inlif_profile);
        }
        
        rv = bcm_port_class_set(unit, gport_in, bcmPortClassFieldIngress, inlif_profile);
        if (rv != BCM_E_NONE) {
            printf("Error doing bcm_port_class_set(0,0x%x,bcmPortClassFieldIngress,%d).\n", gport_in, inlif_profile);
            print rv;
            return rv;
        }

        if (verbose >= 2) {
            printf("Setting inlif profile %d to do DSCP/EXP marking.\n", inlif_profile);
        }

        rv = bcm_port_control_set(unit, inlif_profile, bcmPortControlEgressModifyDscp, 1);
        if (rv != BCM_E_NONE) {
            printf("Error doing bcm_port_control_set(0,%d,bcmPortControlEgressModifyDscp,1).\n", inlif_profile);
            print rv;
            return rv;
        }
    }

    if (verbose >= 2) {
        printf("Creating qos profile...\n");
        if (qos_profile_id != -1) {
            printf("Using id %d for qos profile.\n", qos_profile_id);
        }
    }

    qos_create_flags = BCM_QOS_MAP_L3_L2 | BCM_QOS_MAP_EGRESS;
    if (qos_profile_id != -1) {
        qos_create_flags |= BCM_QOS_MAP_WITH_ID;
        map_id = qos_profile_id;
    }
    rv = bcm_qos_map_create(unit, qos_create_flags, &map_id);
    if (rv) {
        printf("Error doing bcm_qos_map_create for flags BCM_QOS_MAP_L3_L2 | BCM_QOS_MAP_EGRESS.\n");
        print rv;
        return rv;
    }

    if (verbose >= 2) {
        char *inlif_profile_str = "";
        printf("qos profile %d created.\n", map_id);
        if (!simple) {
            inlif_profile_str = "inlif_profile=1, ";
        }
        printf("Adding all mappings of (resolved_dp=0, %sqos_profile=%d, TC=0..7) to set DSCP to %d and EXP to %d.\n", inlif_profile_str, map_id, new_dscp, new_exp);
    }

    bcm_qos_map_t_init(&map);
    for (tc_idx = 0; tc_idx < 8; tc_idx++) {
        map.int_pri = tc_idx; /* TC */
        map.remark_color = 0; /* Resolved-DP */
        if (!simple) {
            map.port_offset = inlif_profile; /* FHEI InLIF profile (2 bits, not 4) */
        }
        map.dscp = new_dscp; /* DSCP */
        map.exp = new_exp; /* EXP */
        if (verbose >= 2) {
            printf("\tAdding TC %d...\n", tc_idx);
        }
        rv = bcm_qos_map_add(unit, 0, &map, map_id);
        if (rv != BCM_E_NONE) {
            printf("Error doing bcm_qos_map_add(0,0,map=<tc=%d,resolved-dp=%d,", tc_idx, 0);
            if (!simple) {
                printf("inlif_profile=0x%x,", inlif_profile);
            }
            printf("dscp=%d,exp=%d>,map_id=%d).\n", new_dscp, new_exp, map_id);
            print rv;
            return rv;
        }
    }

    if (verbose >= 2) {
        printf("Setting the qos profile of port 0x%x to qos profile %d.\n", out_port, map_id);
    }
    rv = bcm_qos_port_map_set(unit, out_port, -1, map_id);
    if (rv) {
        printf("Error doing bcm_qos_port_map_set(0,port_out=%d,0,map_id=%d(qos_profile)).\n", out_port, map_id);
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

