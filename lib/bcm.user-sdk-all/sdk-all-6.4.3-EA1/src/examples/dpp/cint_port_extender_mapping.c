/* $Id: cint_port_extender_mapping.c,v 1.6 Broadcom SDK $
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
* File: cint_port_extender_mapping.c
* Purpose: An example of the port extender mapping. 
*          The following CINT provides a calling sequence example to set ingress PP port mapping via user define value.
*
* Calling sequence:
*
*  1. Add the following port configureations to config-sand.bcm
*        ucode_port_1.BCM88650=10GBase-R13.0
*        ucode_port_2.BCM88650=10GBase-R13.1
*        ucode_port_3.BCM88650=10GBase-R14.0
*        ucode_port_4.BCM88650=10GBase-R14.1
*        ucode_port_5.BCM88650=10GBase-R15
*        custom_feature_vendor_custom_np_port_1=1
*        custom_feature_vendor_custom_np_port_2=1
*        custom_feature_vendor_custom_np_port_3=1
*        custom_feature_vendor_custom_np_port_4=1
*  2. Map the source board and souce port to in pp port.
*        - call bcm_port_extender_mapping_info_set()
*  3. Set the source board and source port which is stamped into eNPHeader.
*        - call bcm_port_class_set() with bcmPortClassProgramEditorEgressPacketProcessing
*  4. Set the system port of the in pp port
*        - call bcm_stk_sysport_gport_set()
*  5. Create service VLAN.
*        - call bcm_vlan_create()
*  6. Add pp ports and physical port into VLAN.
*        - call bcm_vlan_port_add()
*
* Service Model:
*     Source board + souce port <----> NP PP port   <---VLAN 100---> PHY port
*            1          +             1  <----->     1         <-------------->  5
*            1          +             2  <----->     2         <-------------->  5
*
* Traffic:
*  1. PHY port 5 <-> VLAN 100 <-> NP port 1 (source board 1, source port 1)
*        - From PHY port:
*              -   ethernet header with DA 00:00:00:00:00:01, SA 00:00:00:00:00:02
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*        - From NP port:
*              -   eNPHeader 80008830302880000000000
*              -   ethernet header with any DA 00:00:00:00:00:02, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*
*  2. NP port 1 (source board 1, source port 1) <-> VLAN 100 <-> PHY port 5
*        - From NP port:
*              -   iNPHeader 000000000000010100000000
*              -   ethernet header with DA 00:00:00:00:00:02, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*        - From NNI port:
*              -   ethernet header with any DA 00:00:00:00:00:02, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*
*  3. NP port 2 (source board 1, source port 2) <-> VLAN 100 <-> PHY port 5
*        - From NP port:
*              -   iNPHeader 000000000000010100000000
*              -   ethernet header with DA 00:00:00:00:00:03, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*        - From NNI port:
*              -   ethernet header with any DA 00:00:00:00:00:03, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*
* Service Model:
*     Source board + souce port <----> NP PP port   <---VLAN 200---> Source board + souce port <----> NP PP port 
*            1          +             1  <----->     1          <-------------->        2          +             1  <----->     3
*
* Traffic:
*  1. NP port 3 (source board 2, source port 1) <-> VLAN 200 <-> NP port 1 (source board 1, source port 1)
*        - From NP port:
*              -   iNPHeader 000000000000010200000000
*              -   ethernet header with DA 00:00:00:00:00:01, SA 00:00:00:00:00:04
*              -   VLAN tag: VLAN tag type 0x8100, VID = 200
*        - From NNI port:
*              -   eNPHeader 80008830302880000000000
*              -   ethernet header with any DA 00:00:00:00:00:01, SA 00:00:00:00:00:04
*              -   VLAN tag: VLAN tag type 0x8100, VID = 200
*
* To Activate Above Settings Run:
*      BCM> cint examples/dpp/cint_port_extender_mapping.c
*      BCM> cint
*      cint> port_extender_udf_mapping_np_info_init(0, 1, 1, 1);
*      cint> port_extender_udf_mapping_np_info_init(1, 1, 2, 2);
*      cint> port_extender_udf_mapping_np_info_init(1, 2, 1, 3);
*      cint> port_extender_udf_mapping_np_info_init(1, 2, 2, 4);
*      cint> port_extender_udf_phy_port_vlan_init(100, 200, 5);
*      cint> port_extender_udf_service(0);
*      cint> port_extender_udf_service_clean_up(0);
*/

bcm_vlan_t phy_vlan = 100;
bcm_vlan_t np_vlan = 200;

struct np_port_info_s {
    uint32 source_board;
    uint32 source_port;
    bcm_port_t np_port;
};

np_port_info_s np_port_info[4];
bcm_port_t phy_port = 15;
int nof_np_ports = 4;

/* Init np port info */
int port_extender_udf_mapping_np_info_init(uint32 index, uint32 sb, uint32 sp, bcm_port_t np_port)
{
    if (index > nof_np_ports) {
        return BCM_E_PARAM;
    }

    np_port_info[index].source_board = sb;
    np_port_info[index].source_port = sp;
    np_port_info[index].np_port = np_port;

    return 0;
}

/* Init phy port and vlan */
int port_extender_udf_phy_port_vlan_init(bcm_vlan_t phy_vid, bcm_vlan_t np_vid, bcm_port_t port)
{
    phy_vlan = phy_vid;
    np_vlan = np_vid;
    phy_port = port;

    return 0;
}

/* Set np port mapping. np_port should be equal to np_pp_port */
int port_extender_udf_mapping_set(int unit, int sb, int sp, bcm_port_t np_port, bcm_port_t np_pp_port)
{
    int rv = 0;
    int32 user_define_value;
    bcm_port_extender_mapping_info_t mapping_info;

    user_define_value = (sb<<6)|sp;

    mapping_info.user_define_value=user_define_value;
    mapping_info.pp_port=np_pp_port;
    mapping_info.phy_port=np_port;

    /* Map the source board and souce port to in pp port */
    rv = bcm_port_extender_mapping_info_set(unit, BCM_PORT_EXTENDER_MAPPING_INGRESS, bcmPortExtenderMappingTypeUserDefineValue, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_extender_mapping_info_set failed $rv\n");
        return rv;
    }

    /* Set the destination board and destination port which are stamped into eNPHeader */
    rv = bcm_port_class_set(unit, np_pp_port, bcmPortClassProgramEditorEgressPacketProcessing, user_define_value);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_class_set failed $rv\n");
        return rv;
    }

    /* Set the system port of the in pp port */
    rv = bcm_stk_sysport_gport_set(unit, np_port, np_pp_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_stk_sysport_gport_set failed $rv\n");
        return rv;
    }

    return rv;
}

/* Get np port mapping */
int port_extender_udf_mapping_get(int unit, int sb, int sp, bcm_port_t np_port, bcm_port_t *np_pp_port)
{
    int rv = 0;
    int32 user_define_value;
    bcm_port_extender_mapping_info_t mapping_info;

    user_define_value = (sb<<6)|sp;

    sal_memset(&mapping_info, 0, sizeof(bcm_port_extender_mapping_info_t));
    mapping_info.user_define_value=user_define_value;
    mapping_info.phy_port=np_port;

    /* Get the mapped in pp port based on source board and souce port */
    rv = bcm_port_extender_mapping_info_get(unit, BCM_PORT_EXTENDER_MAPPING_INGRESS, bcmPortExtenderMappingTypeUserDefineValue, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_extender_mapping_info_set failed $rv\n");
        return rv;
    }

    *np_pp_port = mapping_info.pp_port;

    return rv;
}

/* Build the service mode. VLAN 100 includes both np and phy ports */
int port_extender_udf_service(int unit)
{
    int rv = 0;
    bcm_pbmp_t pbmp, ubmp;
    int index;

    /* create phy_vlan for NP port<>PHY port service */
    rv = bcm_vlan_create(unit, phy_vlan);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_create failed $rv\n");
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, np_port_info[0].np_port);
    BCM_PBMP_PORT_ADD(pbmp, np_port_info[1].np_port);
    BCM_PBMP_PORT_ADD(pbmp, phy_port);
    BCM_PBMP_CLEAR(ubmp);

    rv = bcm_vlan_port_add(unit, phy_vlan, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_add failed $rv\n");
        return rv;
    }

    /* create np_vlan for NP port<>NP port service */
    rv = bcm_vlan_create(unit, np_vlan);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_create failed $rv\n");
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    for (index = 0; index < nof_np_ports; index++) {
        BCM_PBMP_PORT_ADD(pbmp, np_port_info[index].np_port);
    }
    BCM_PBMP_CLEAR(ubmp);

    rv = bcm_vlan_port_add(unit, np_vlan, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_add failed $rv\n");
        return rv;
    }

    for (index = 0; index < nof_np_ports; index++) {
        rv = port_extender_udf_mapping_set(unit, np_port_info[index].source_board, np_port_info[index].source_port, 
            np_port_info[index].np_port, np_port_info[index].np_port);
        if (rv != BCM_E_NONE) {
            printf("port_extender_udf_mapping_set failed $rv\n");
            return rv;
        }
    }

    return rv;
}

/* Clean up the service. */
int port_extender_udf_service_clean_up(int unit)
{
    int rv = 0;
    int index;

    rv = bcm_vlan_destroy(unit, phy_vlan);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_destroy failed $rv\n");
        return rv;
    }
    
    rv = bcm_vlan_destroy(unit, np_vlan);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_destroy failed $rv\n");
        return rv;
    }

    for (index = 0; index < nof_np_ports; index++) {
        rv = port_extender_udf_mapping_set(unit, np_port_info[index].source_board, np_port_info[index].source_port, 
            np_port_info[index].np_port, 0);
        if (rv != BCM_E_NONE) {
            printf("port_extender_udf_mapping_set failed $rv\n");
            return rv;
        }
    }

    return rv;
}

