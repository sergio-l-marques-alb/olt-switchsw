/*~~~~~~~~~~~~~~~~~~~~~~~Mulitpoint VPLS Service~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_mpls_ecmp.c,v 1.00 Broadcom SDK $
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
 * File: cint_mpls_ecmp.c
 * Purpose: Example of two levels MPLS-ECMP configuration.
 *
 *   Incoming Ethernet Packet
 *      * * *   with MPLS label of 0x5000
 *    * 5000  *                                          Outer outgoing
 *    *       *                            * * * * * * * * * * *   MPLS label (0x1000)
 *    *   |   *                          *                       *
 *      * | *                          *  0        * * *           *
 *        |                           *  0       *  101  *          *
 *        |                          *  0        *      ------------------------->
 *        |                         *  1         /       *            *
 *        |   +----+               *            /  * * *               *
 *        |   |ECMP|               *  +----+   /            * * *      *
 *        \-->| 1  |----------------->|ECMP|----------------      *    *       Inner outgoing
 *            +----+               *  |1_2 |   \          *  102  *    *          MPLS label (0x101 / 0x102 / 0x103)
 *               | Hashing         *  +----+    \         *     ----------------->
 *               | configured      *             \ * *      * * *      *
 *               | to round robin   *          *  \    *              *
 *               |                   *         *      --------------------------->
 *               |                    *        *  103  *            *
 *               |                     *         * * *             *
 *               |                       *                       *
 *               |                         * * * * * * * * * * *
 *               |
 *               |                         Outer outgoing
 *               |           * * * * * * * * * * *   MPLS label (0x2000)
 *               |         *                       *
 *               |       *  0        * * *           *
 *               |      *  0       *  201  *          *
 *               |     *  0        *      ------------------------->
 *               |    *  2         /       *            *
 *               |   *            /  * * *               *
 *               |   *  +----+   /            * * *      *
 *               \----->|ECMP|----------------      *    *       Inner outgoing
 *                   *  |2_2 |   \          *  202  *    *          MPLS label (0x201 / 0x202 / 0x203)
 *                   *  +----+    \         *     ----------------->
 *                   *             \ * *      * * *      *
 *                    *          *  \    *              *
 *                     *         *      --------------------------->
 *                      *        *  203  *            *
 *                       *         * * *             *
 *                         *                       *
 *                           * * * * * * * * * * *
 *
 * Explanation:
 *  Incoming MPLS over Ethernet packets coming from in_port, vlan 100 with a label of 0x5000
 *  will be directed to ECMP 1 which will select next FEC by hash results.
 *  next FEC will point to an MPLS tunnel and will forward to another ECMP which will select next FEC.
 *  This FEC will point to another MPLS tunnel and will forward to physical out_port.
 *
 * Packets:
 *  Incoming
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | Ethernet                                          | MPLS                    | Payload |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | DA                | SA | TPID   | PCP | DEI | VID | Label  | TC | BOS | TTL |         |
 *  | 00:00:00:01:01:01 |    | 0x8100 |     |     | 100 | 0x5000 |    | 1   |     |         |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *  Outgoing:
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | Ethernet                                          | MPLS                    | MPLS                    | MPLS                    | Payload |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | DA                | SA | TPID   | PCP | DEI | VID | Label  | TC | BOS | TTL | Label  | TC | BOS | TTL | Label  | TC | BOS | TTL |         |
 *  | 00:00:00:08:08:08 |    | 0x8100 |     |     | 200 | 0x1000 |    | 0   |     | 0x101  |    | 0   |     | 0x5000 |    | 1   |     |         |
 *  |                   |    |        |     |     |     |        |    |     |     | 0x102  |    |     |     |        |    |     |     |         |
 *  |                   |    |        |     |     |     |        |    |     |     | 0x103  |    |     |     |        |    |     |     |         |
 *  |                   |    |        |     |     |     | 0x2000 |    |     |     | 0x201  |    |     |     |        |    |     |     |         |
 *  |                   |    |        |     |     |     |        |    |     |     | 0x202  |    |     |     |        |    |     |     |         |
 *  |                   |    |        |     |     |     |        |    |     |     | 0x203  |    |     |     |        |    |     |     |         |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * To Activate Above Settings Run:
 *      BCM> cint examples/dpp/utility/cint_utils_global.c
 *      BCM> cint examples/dpp/utility/cint_utils_vlan.c
 *      BCM> cint examples/dpp/utility/cint_utils_l3.c
 *      BCM> cint examples/dpp/cint_mpls_ecmp.c
 *      BCM> cint
 *      cint> mpls_dual_ecmp_main(0,200,201);
 *
 */

struct mpls_ecmp_info_s {
    int in_vlan;
    int in_label;
    int out_label1;
    int out_label1_1;
    int out_label1_2;
    int out_label1_3;
    int out_label2;
    int out_label2_1;
    int out_label2_2;
    int out_label2_3;
    int out_vlan;
    uint8 my_mac1[6];
    uint8 my_mac2[6];
    uint8 next_hop_mac[6];
};

mpls_ecmp_info_s mpls_ecmp_info_1;

int mpls_ecmp_init() {
    uint8 my_mac1[6] = {0, 0, 0, 1, 1, 1};
    uint8 my_mac2[6] = {0, 0, 0, 1, 1, 2};
    uint8 next_hop_mac[6] = {0, 0, 0, 8, 8, 8};

    mpls_ecmp_info_1.in_vlan = 100;
    mpls_ecmp_info_1.in_label = 0x5000;
    mpls_ecmp_info_1.out_label1 = 0x1000;
    mpls_ecmp_info_1.out_label1_1 = 0x101;
    mpls_ecmp_info_1.out_label1_2 = 0x102;
    mpls_ecmp_info_1.out_label1_3 = 0x103;
    mpls_ecmp_info_1.out_label2 = 0x2000;
    mpls_ecmp_info_1.out_label2_1 = 0x201;
    mpls_ecmp_info_1.out_label2_2 = 0x202;
    mpls_ecmp_info_1.out_label2_3 = 0x203;
    mpls_ecmp_info_1.out_vlan = 200;
    sal_memcpy(mpls_ecmp_info_1.my_mac1, my_mac1, 6);
    sal_memcpy(mpls_ecmp_info_1.my_mac2, my_mac2, 6);
    sal_memcpy(mpls_ecmp_info_1.next_hop_mac, next_hop_mac, 6);

    return 0;
}

/* Create MPLS tunnel initiator */
int mpls_ecmp_tunnel_initiator_create(int unit, int level, int label, int out_interface, int* tunnel_id) {
    int rc;

    bcm_mpls_egress_label_t label_array[1];
    label_array[0].exp = 0;
    label_array[0].label = label;
    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_ACTION_VALID|BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT|BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    label_array[0].action = BCM_MPLS_EGRESS_ACTION_PUSH;
    label_array[0].l3_intf_id = out_interface;
    if (level == 2) {
        label_array[0].ttl = 20;
        BCM_L3_ITF_SET(label_array[0].l3_intf_id, BCM_L3_ITF_TYPE_LIF, 0); /* Crucial part - Otherwise MPLS tunnels will compete in the same phase */
    } else {
        label_array[0].ttl = 30;
        label_array[0].l3_intf_id = out_interface;
    }

    rc = bcm_mpls_tunnel_initiator_create(unit,0,1,label_array);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_mpls_tunnel_initiator_create Label = 0x%x\n", label_array[0].label);
        return rc;
    }

    *tunnel_id = label_array[0].tunnel_id;

    printf("MPLS tunnel created. Label = 0x%x, Tunnel id = 0x%x\n", label_array[0].label, *tunnel_id);

    return rc;
}

/* Create cascaded FEC (ingress only) */
int mpls_ecmp_cascaded_fec_create(int unit, int out_interface, int out_port, int out_vlan, bcm_if_t* fec_id) {
    int rc;

    bcm_l3_egress_t l3eg_fec;
    bcm_l3_egress_t_init(&l3eg_fec);
    l3eg_fec.flags = BCM_L3_CASCADED; /* Declare it's a second layer */
    l3eg_fec.intf = out_interface;
    l3eg_fec.port = out_port;
    l3eg_fec.vlan = out_vlan;

    rc = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3eg_fec, fec_id);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_create\n");
        return rc;
    }

    printf("Cascaded FEC created. ID = 0x%x, for tunnel id = 0x%x\n", *fec_id, out_interface);

    return rc;
}

/* Dual level ECMP with MPLS */
int mpls_dual_ecmp_main(int unit, int in_port, int out_port) {
    int rc;

    /* Init default settings */
    mpls_ecmp_init();

    /* Create VLAN for incoming traffic */
    rc = vlan__open_vlan_per_mc(unit, mpls_ecmp_info_1.in_vlan, 0x1);
    if (rc != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", mpls_ecmp_info_1.in_vlan, unit);
    }
    /* Assign in_port to vlan */
    rc = bcm_vlan_gport_add(unit, mpls_ecmp_info_1.in_vlan, in_port, 0);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, mpls_ecmp_info_1.in_vlan);
        return rc;
    }

    /* Create VLAN for outgoing traffic */
    rc = vlan__open_vlan_per_mc(unit, mpls_ecmp_info_1.out_vlan, 0x1);
    if (rc != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", mpls_ecmp_info_1.out_vlan, unit);
    }
    /* Assign out_port to vlan */
    rc = bcm_vlan_gport_add(unit, mpls_ecmp_info_1.out_vlan, out_port, 0);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, mpls_ecmp_info_1.out_vlan);
        return rc;
    }

    /* Creating in-rif to terminate ETH */
    create_l3_intf_s in_intf;
    in_intf.vsi = mpls_ecmp_info_1.in_vlan;
    in_intf.my_global_mac = mpls_ecmp_info_1.my_mac1;
    in_intf.my_lsb_mac = mpls_ecmp_info_1.my_mac1;

    rc = l3__intf_rif__create(unit, &in_intf);
    if (rc != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
        return rc;
    }

    /* Creating out-lif */
    create_l3_intf_s out_intf;
    out_intf.vsi = mpls_ecmp_info_1.out_vlan;
    out_intf.my_global_mac = mpls_ecmp_info_1.my_mac2;
    out_intf.my_lsb_mac = mpls_ecmp_info_1.my_mac2;

    rc = l3__intf_rif__create(unit, &out_intf);
    if (rc != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
        return rc;
    }

    /* Creating two level-1 MPLS tunnels for outgoing packets */
    int tunnel_id_1, tunnel_id_2;
    mpls_ecmp_tunnel_initiator_create(unit, 1, mpls_ecmp_info_1.out_label1, out_intf.rif, &tunnel_id_1);
    mpls_ecmp_tunnel_initiator_create(unit, 1, mpls_ecmp_info_1.out_label2, out_intf.rif, &tunnel_id_2);

    /* Creating six level-2 MPLS tunnels for outgoing packets */
    int tunnel_id_1_1, tunnel_id_1_2, tunnel_id_1_3, tunnel_id_2_1, tunnel_id_2_2, tunnel_id_2_3;

    mpls_ecmp_tunnel_initiator_create(unit, 2, mpls_ecmp_info_1.out_label1_1, 0, &tunnel_id_1_1);
    mpls_ecmp_tunnel_initiator_create(unit, 2, mpls_ecmp_info_1.out_label1_2, 0, &tunnel_id_1_2);
    mpls_ecmp_tunnel_initiator_create(unit, 2, mpls_ecmp_info_1.out_label1_3, 0, &tunnel_id_1_3);

    mpls_ecmp_tunnel_initiator_create(unit, 2, mpls_ecmp_info_1.out_label2_1, 0, &tunnel_id_2_1);
    mpls_ecmp_tunnel_initiator_create(unit, 2, mpls_ecmp_info_1.out_label2_2, 0, &tunnel_id_2_2);
    mpls_ecmp_tunnel_initiator_create(unit, 2, mpls_ecmp_info_1.out_label2_3, 0, &tunnel_id_2_3);

    /* Creating level-2 FEC for each level-2 MPLS tunnel */
    bcm_if_t fec_id1_1, fec_id1_2, fec_id1_3, fec_id2_1, fec_id2_2, fec_id2_3;

    mpls_ecmp_cascaded_fec_create(unit, tunnel_id_1_1, out_port, mpls_ecmp_info_1.out_vlan, &fec_id1_1);
    mpls_ecmp_cascaded_fec_create(unit, tunnel_id_1_2, out_port, mpls_ecmp_info_1.out_vlan, &fec_id1_2);
    mpls_ecmp_cascaded_fec_create(unit, tunnel_id_1_3, out_port, mpls_ecmp_info_1.out_vlan, &fec_id1_3);

    mpls_ecmp_cascaded_fec_create(unit, tunnel_id_2_1, out_port, mpls_ecmp_info_1.out_vlan, &fec_id2_1);
    mpls_ecmp_cascaded_fec_create(unit, tunnel_id_2_2, out_port, mpls_ecmp_info_1.out_vlan, &fec_id2_2);
    mpls_ecmp_cascaded_fec_create(unit, tunnel_id_2_3, out_port, mpls_ecmp_info_1.out_vlan, &fec_id2_3);

    /* Creating two level-2 ECMPs each to balance between two level-2 MPLS tunnels */
    int interface_count = 3;
    bcm_if_t interfaces1_1[interface_count];
    bcm_if_t interfaces1_2[interface_count];

    interfaces1_1[0] = fec_id1_1;
    interfaces1_1[1] = fec_id1_2;
    interfaces1_1[2] = fec_id1_3;

    interfaces1_2[0] = fec_id2_1;
    interfaces1_2[1] = fec_id2_2;
    interfaces1_2[2] = fec_id2_3;

    bcm_l3_egress_ecmp_t ecmp1_1;
    bcm_l3_egress_ecmp_t_init(&ecmp1_1);
    ecmp1_1.max_paths = interface_count;
    rc = bcm_l3_egress_ecmp_create(unit, &ecmp1_1, interface_count, interfaces1_1);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_egress_ecmp_create failed: %d \n", rc);
        return rc;
    }
    printf("level-2 ECMP created. interface = 0x%x, for FECs 0x%x, 0x%x, 0x%x\n", ecmp1_1.ecmp_intf, interfaces1_1[0], interfaces1_1[1], interfaces1_1[2]);

    bcm_l3_egress_ecmp_t ecmp1_2;
    bcm_l3_egress_ecmp_t_init(&ecmp1_2);
    ecmp1_2.max_paths = interface_count;
    rc = bcm_l3_egress_ecmp_create(unit, &ecmp1_2, interface_count, interfaces1_2);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_egress_ecmp_create failed: %d \n", rc);
        return rc;
    }
    printf("level-2 ECMP created. interface = 0x%x, for FECs 0x%x, 0x%x, 0x%x\n", ecmp1_2.ecmp_intf, interfaces1_2[0], interfaces1_2[1], interfaces1_2[2]);

    /* Creating two level-1 FECs for each level-1 MPLS tunnel and level-1 ECMPs */
    /* Hierarchical FEC does not work when the 1st FEC hierarchy is unprotected.
       * In case we are using hierarchical FEC, the first FEC must be protected.
       */
    bcm_failover_t failover_id_fec;
    rc = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id_fec);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_failover_create\n");
        return rc;
    }

    bcm_if_t fec_id1;
    bcm_if_t fec_id2;

    bcm_l3_egress_t l3eg_fec_1;
    bcm_l3_egress_t_init(&l3eg_fec_1);
    l3eg_fec_1.intf = tunnel_id_1;                                             /* Choose level-1 MPLS tunnel */
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(l3eg_fec_1.port, ecmp1_1.ecmp_intf); /* level-2 MPLS tunnel will be choosed by ECMP */
    l3eg_fec_1.failover_id = failover_id_fec;                                  /* Protect FEC to enable hierarchical FEC */
    l3eg_fec_1.failover_if_id = 0;
    sal_memcpy(l3eg_fec_1.mac_addr, mpls_ecmp_info_1.next_hop_mac, 6);
    l3eg_fec_1.flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    rc = bcm_l3_egress_create(unit,0,&l3eg_fec_1,&fec_id1);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_create\n");
        return rc;
    }
    l3eg_fec_1.failover_if_id = fec_id1;
    l3eg_fec_1.encap_id = 0;
    rc = bcm_l3_egress_create(unit,0,&l3eg_fec_1,&fec_id1);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_create\n");
        return rc;
    }
    printf("FEC created. ID = 0x%x, for tunnel id = 0x%x\n", fec_id1, tunnel_id_1);

    bcm_l3_egress_t l3eg_fec_2;
    bcm_l3_egress_t_init(&l3eg_fec_2);
    l3eg_fec_2.intf = tunnel_id_2;
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(l3eg_fec_2.port, ecmp1_2.ecmp_intf);
    l3eg_fec_2.failover_id = failover_id_fec;
    l3eg_fec_2.failover_if_id = 0;
    sal_memcpy(l3eg_fec_2.mac_addr, mpls_ecmp_info_1.next_hop_mac, 6);
    l3eg_fec_2.flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    rc = bcm_l3_egress_create(unit,0,&l3eg_fec_2,&fec_id2);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_create\n");
        return rc;
    }
    l3eg_fec_2.failover_if_id = fec_id2;
    l3eg_fec_2.encap_id = 0;
    rc = bcm_l3_egress_create(unit,0,&l3eg_fec_2,&fec_id2);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_create\n");
        return rc;
    }
    printf("FEC created. ID = 0x%x, for tunnel id = 0x%x\n", fec_id2, tunnel_id_2);

    /* Creating level-1 ECMP to balance between the two level-2 ECMPs */
    interface_count = 2;
    bcm_if_t interfaces1[interface_count];
    interfaces1[0] = fec_id1;
    interfaces1[1] = fec_id2;

    bcm_l3_egress_ecmp_t ecmp1;
    bcm_l3_egress_ecmp_t_init(&ecmp1);
    ecmp1.max_paths = interface_count;

    rc = bcm_l3_egress_ecmp_create(unit, &ecmp1, interface_count, interfaces1);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_egress_ecmp_create failed: %d \n", rc);
        return rc;
    }
    printf("level-1 ECMP created. interface = 0x%x, for FECs 0x%x, 0x%x\n", ecmp1.ecmp_intf, interfaces1[0], interfaces1[1]);

    /* Set ECMP hashing */
    rc = bcm_switch_control_set(unit, bcmSwitchECMPHashConfig, BCM_HASH_CONFIG_ROUND_ROBIN);
    if (rc != BCM_E_NONE) {
        printf ("bcm_switch_control_set with type bcmSwitchECMPHashConfig failed: %d \n", rc);
        return rc;
    }

    /* Create MPLS tunnel switch and point to the level-1 ECMP */
    bcm_mpls_tunnel_switch_t mpls_tunnel_info;
    mpls_tunnel_info.action = BCM_MPLS_SWITCH_ACTION_NOP;
    mpls_tunnel_info.flags = BCM_MPLS_SWITCH_TTL_DECREMENT | BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;
    mpls_tunnel_info.label = mpls_ecmp_info_1.in_label;
    mpls_tunnel_info.egress_if = ecmp1.ecmp_intf;

    rc = bcm_mpls_tunnel_switch_create(unit, &mpls_tunnel_info);
    if (rc != BCM_E_NONE) {
        printf ("bcm_mpls_tunnel_switch_create failed: %x \n", rc);
        return rc;
    }

    return rc;
}
