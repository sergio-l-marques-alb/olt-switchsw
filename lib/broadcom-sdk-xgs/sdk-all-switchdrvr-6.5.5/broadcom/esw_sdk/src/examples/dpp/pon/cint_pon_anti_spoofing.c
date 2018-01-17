/* $Id: cint_pon_anti_spoofing.c,v 1.3 Broadcom SDK $
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
 * File: cint_pon_anti_spoofing.c
 *
 * Purpose: An example of how to use BCM APIs to implement anti-spoofing(L3 Source Binding) function in pon appplication service.
 *          After enable anti-spoofing function, system will permit/deny traffic pass base source bind information 
 *          of pon gport. SAs of denied packets are not leant.
 *
 *          Here is the binding information, and the mode is decided by IN-lIF bind configuration at  
 *          |----------------|---------------------------------------------------------------|
 *          |     Mode       |     binding configuration                                     |
 *          |----------------|---------------------------------------------------------------|
 *          | IPV4 DHCP      | SIP, SA and flag isn't BCM_L3_SOURCE_BIND_IP6                 |
 *          |----------------|---------------------------------------------------------------|
 *          | IPV4 STATIC    | SIP, SA is zero and flag isn't BCM_L3_SOURCE_BIND_IP6         |
 *          |----------------|---------------------------------------------------------------|
 *          | IPV6 DHCP      | SIPV6, SA and flag is BCM_L3_SOURCE_BIND_IP6                  |
 *          |----------------|---------------------------------------------------------------|
 *          | IPV6 STATIC    | SIPV6, SA is zero and BCM_L3_SOURCE_BIND_IP6                  |
 *          |----------------|---------------------------------------------------------------|
 * Calling sequence:
 *
 * Initialization:
 *  1. Set the following port configureations to config-sand.bcm
 *        ucode_port_128.BCM88650=10GBase-R15
 *        ucode_port_4.BCM88650=10GBase-R14
 *  2. Add the following PON application and anti-spoofing enabling configureations to config-sand.bcm
 *        pon_application_support_enabled_4.BCM88650=TRUE
 *        l3_source_bind_mode=IP
 *  3. Enable pon service:PON port will recognize tunnel Id(100) CTAG(10),NNI port will recognize STAG(100), CTAG(10).
 *        - call pon_anti_spoofing_app()
 *  4. Set IPV4, IPV6 source bind configuration, details:
 *        IPV4 DHCP:   SIP-10.10.10.10, SA-00:00:01:00:00:01
 *        IPV4 STATIC: SIP-10.10.10.11
 *        IPV6 DHCP:   SIP-2000::2, SA-00:00:03:00:00:01
 *        IPV6 STATIC: SIP-2000::3
 *        - call pon_anti_spoofing_enable()
 *  5. Enable FP to drop un-matched traffic.
 *        - call pon_anti_spoofing_drop()
 *  6. Clean all above configuration.
 *        - call pon_anti_spoofing_app_clean()
 *
 * To Activate Above Settings:
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/pon/cint_pon_utils.c
 *      BCM> cint examples/dpp/pon/cint_pon_anti_spoofing.c
 *      BCM> cint
 *      cint> pon_anti_spoofing_app(unit, pon_port, nni_port);
 *      cint> pon_anti_spoofing_enable(unit);
 *      cint> pon_anti_spoofing_drop(unit);
 *
 * To clean all above configuration:
 *      cint> pon_anti_spoofing_app_clean(unit);
 */
 
struct pon_anti_spoof_info_s {
    bcm_vlan_t  vsi;
    bcm_gport_t pon_gport;
    bcm_gport_t nni_gport;
    bcm_port_t pon_port;
    bcm_port_t nni_port;
    int tunnel_id;
    int nni_svlan;
    int pon_cvlan;
    int lif_offset;
    bcm_field_group_config_t grp;
    bcm_field_entry_t ent[4];
};

pon_anti_spoof_info_s pon_anti_spoofing_info;
bcm_l3_source_bind_t src_bind[4];

/* Initialize PON application configurations.
 * Example: pon_anti_spoofing_init(0, 4, 128);
 */
int pon_anti_spoofing_init(int unit, bcm_port_t pon_port, bcm_port_t nni_port)
{
    bcm_ip6_t sip1;

    sal_memset(&pon_anti_spoofing_info, 0, sizeof(pon_anti_spoofing_info));
    pon_anti_spoofing_info.vsi = 0;
    pon_anti_spoofing_info.pon_port = pon_port;
    pon_anti_spoofing_info.nni_port = nni_port;
    pon_anti_spoofing_info.tunnel_id = 100;
    pon_anti_spoofing_info.nni_svlan = 100;
    pon_anti_spoofing_info.pon_cvlan = 10;

    /* Disable trunk in NNI port */
    pon_app_init(unit, pon_port, nni_port, 0, 0);
    /* lif_offset is updated in pon_app_init() based on device type */
    pon_anti_spoofing_info.lif_offset = lif_offset;
    /* src_bind[0] -- ipv4 dhcp */
    /* 10.10.10.10 */
    src_bind[0].ip = 168430090;
    src_bind[0].mac[0] = 0x0;
    src_bind[0].mac[1] = 0x0;
    src_bind[0].mac[2] = 0x1;
    src_bind[0].mac[3] = 0x0;
    src_bind[0].mac[4] = 0x0;
    src_bind[0].mac[5] = 0x1;

    /* src_bind[1] -- ipv4 static */
    /* 10.10.10.11 */
    src_bind[1].ip = 168430091;

    /* src_bind[2] -- ipv6 dhcp */
    src_bind[2].ip6[15]= 0x02;   
    src_bind[2].ip6[0] = 0x20;
    src_bind[2].mac[0] = 0x0;
    src_bind[2].mac[1] = 0x0;
    src_bind[2].mac[2] = 0x3;
    src_bind[2].mac[3] = 0x0;
    src_bind[2].mac[4] = 0x0;
    src_bind[2].mac[5] = 0x1;
    src_bind[2].flags |= BCM_L3_SOURCE_BIND_IP6;

    /* src_bind[3] -- ipv6 static */
    src_bind[3].flags |= BCM_L3_SOURCE_BIND_IP6;
    src_bind[3].ip6[15]= 0x03;
    src_bind[3].ip6[0] = 0x20;
}

/*********************************************************************************
* Setup pon anti-spoofing test model(1:1)
*     PON Port 1 or 7  <------------------------------->  VSI  <---> NNI Port 128
*     Pon 1 Tunnel-ID 100 clvan 10 --------------------|---4096  |----------- SVLAN 100 cvlan 10
*                                                 
* steps
*
*/
int pon_anti_spoofing_app(int unit, bcm_port_t pon, bcm_port_t nni)
{
    int i = 0;
    int rv = 0;
    bcm_if_t encap_id = 0;
    int action_id = 0;

    pon_anti_spoofing_init(unit, pon, nni);

    rv = vswitch_create(unit, &(pon_anti_spoofing_info.vsi));
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create PON LIF */
    rv = pon_lif_create(unit, 
                        pon_anti_spoofing_info.pon_port, 
                        match_otag,
                        0, 0,
                        pon_anti_spoofing_info.tunnel_id,
                        pon_anti_spoofing_info.tunnel_id,
                        pon_anti_spoofing_info.pon_cvlan,
                        0, 0, 0,
                        &(pon_anti_spoofing_info.pon_gport));
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_lif_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add PON LIF to vswitch */
    rv = bcm_vswitch_port_add(unit, 
                              pon_anti_spoofing_info.vsi, 
                              pon_anti_spoofing_info.pon_gport
                             );
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add returned %s\n", bcm_errmsg(rv));
        return rv;
    }
    /*For downstream*/
    rv = multicast_vlan_port_add(unit, 
                                 pon_anti_spoofing_info.vsi+pon_anti_spoofing_info.lif_offset, 
                                 pon_anti_spoofing_info.pon_port, 
                                 pon_anti_spoofing_info.pon_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multicast_vlan_port_add returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set PON LIF ingress VLAN editor */
    rv = pon_port_ingress_vt_set(unit,
                                 otag_to_o_i_tag,
                                 pon_anti_spoofing_info.nni_svlan,
                                 pon_anti_spoofing_info.pon_cvlan,
                                 pon_anti_spoofing_info.pon_gport,
                                 0, &action_id
                                 );
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_port_ingress_vt_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }                                             
    /* Set PON LIF egress VLAN editor */
    rv = pon_port_egress_vt_set(unit,
                                otag_to_o_i_tag,
                                pon_anti_spoofing_info.tunnel_id,
                                pon_anti_spoofing_info.pon_cvlan,
                                0, 
                                pon_anti_spoofing_info.pon_gport
                               );
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_port_egress_vt_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create NNI LIF */
    rv = nni_lif_create(unit,
                       pon_anti_spoofing_info.nni_port,
                       match_o_i_tag,
                       0,
                       pon_anti_spoofing_info.nni_svlan,
                       pon_anti_spoofing_info.pon_cvlan,
                       &(pon_anti_spoofing_info.nni_gport),
                       &encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, nni_lif_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*For upstream*/
    rv = multicast_vlan_port_add(unit, 
                              pon_anti_spoofing_info.vsi,
                              pon_anti_spoofing_info.nni_port,
                              pon_anti_spoofing_info.nni_gport);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, multicast_vlan_port_add returned %s nni_gport\n", bcm_errmsg(rv));
        return rv;
    }

    /* add to vswitch */
    rv = bcm_vswitch_port_add(unit,
                           pon_anti_spoofing_info.vsi,
                           pon_anti_spoofing_info.nni_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add returned %s nni_gport\n", bcm_errmsg(rv));
        return rv;
    }
    
    /* Set the multicast group offset to flood downstream packets in multicast group(vsi+lif_offset) */
    bcm_port_control_set(unit, pon_anti_spoofing_info.nni_gport, bcmPortControlFloodUnknownUcastGroup, pon_anti_spoofing_info.lif_offset);
    bcm_port_control_set(unit, pon_anti_spoofing_info.nni_gport, bcmPortControlFloodUnknownMcastGroup, pon_anti_spoofing_info.lif_offset);
    bcm_port_control_set(unit, pon_anti_spoofing_info.nni_gport, bcmPortControlFloodBroadcastGroup, pon_anti_spoofing_info.lif_offset);

    /* Will set 4 bind configuration in this PON-LIF gport */
    for (i = 0; i < 4; i++)
    {
        src_bind[i].port = pon_anti_spoofing_info.pon_gport;
    }

    return rv;
}

/*********************************************************************************
* Enable anti-spoofing function and set IPV4, IPV6 source bind configuration.
*
* Steps
*     1. Call bcm_l3_source_bind_enable_set() to enable anti-spoofing function
*        in PON-LIF gport.
*     2. Call bcm_l3_source_bind_add() to set bind configuration in this PON-LIF gport.
*
*/
int pon_anti_spoofing_enable(int unit)
{
    int rv = 0;
    int i = 0;

    /* Enable anti-spoffing in this PON-LIF gport */
    rv = bcm_l3_source_bind_enable_set(unit,
                                       pon_anti_spoofing_info.pon_gport,
                                       1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_source_bind_enable_set returned %s pon_gport:%d\n", 
                bcm_errmsg(rv), pon_anti_spoofing_info.pon_gport);
        return rv;
    }

    /* Set 4 bind configuration(IPV4 DHCP, IPV4 STATIC, IPV6 DHCP and IPV6 STATIC)
    * in this PON-LIF gport
    */
    for (i = 0; i < 4; i++) 
    {
        rv = bcm_l3_source_bind_add(unit, 
                                    &(src_bind[i]));
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_source_bind_add returned %s src_bind[%d]\n", bcm_errmsg(rv), i);
            return rv;
        }
    }

    return rv;
}

/*********************************************************************************
* Delete IPV4, IPV6 source bind configuration and disable anti-spoofing function.
*
* Steps
*     1. Call bcm_l3_source_bind_delete() to delete bind configuration 
*        in this PON-LIF gport.
*     2. Call bcm_l3_source_bind_enable_set() to disable anti-spoofing function
*        in PON-LIF gport.
*
*/
int pon_anti_spoofing_disable(int unit)
{
    int rv = 0;
    int i = 0;


    /* Delete 4 bind configuration(IPV4 DHCP, IPV4 STATIC, IPV6 DHCP and IPV6 STATIC)
    * in this PON-LIF gport
    */
    for (i = 0; i < 4; i++)
    {
        rv = bcm_l3_source_bind_delete(unit, 
                                       &(src_bind[i]));
        if (rv != BCM_E_NONE) 
        {
            printf("Error, bcm_l3_source_bind_add returned %s src_bind[%d]\n", bcm_errmsg(rv), i);
            return rv;
        }
    }
    /* Disable anti-spoffing in this gport */
    rv = bcm_l3_source_bind_enable_set(unit,
                                       pon_anti_spoofing_info.pon_gport,
                                       0);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_l3_source_bind_enable_set returned %s pon_gport:%d\n", 
                bcm_errmsg(rv), pon_anti_spoofing_info.pon_gport);
        return rv;
    }

    return rv;
}

/*********************************************************************************
* Create a field group to drop un-matched traffic.
*
* Steps
*     1. Create a field group
*     2. Attach a action set to field group
*     3. Add entries to drop/pass traffic
*
*/
int pon_anti_spoofing_drop(int unit)
{
    bcm_field_aset_t aset;
    bcm_field_entry_t *ent[4];
    bcm_field_group_config_t *grp;
    bcm_field_AppType_t app_type[4] = {bcmFieldAppTypeIp4MacSrcBind, bcmFieldAppTypeIp6MacSrcBind,
        bcmFieldAppTypeIp4SrcBind, bcmFieldAppTypeIp6SrcBind};
    int group_priority = 10;
    int result;
    int auxRes;
    int i = 0;

    grp = &(pon_anti_spoofing_info.grp);
    for (i = 0; i < 4; i++)
    {
        ent[i] = &(pon_anti_spoofing_info.ent[i]);
    }

    bcm_field_group_config_t_init(grp);
    grp->group = -1;

    /* Define the QSET
     *  bcmFieldQualifyInPort: Single Input Port
     *  bcmFieldQualifyAppType: Application type, to see if L3 source bind lookup is done
     *  bcmFieldQualifyL2DestHit: L2 Destination lookup success(LEM look up result)
     *  bcmFieldQualifyIpmcHit:   IP Multicast lookup hit(TCAM look up result)
     *  bcmFieldQualifyStageIngress: Field Processor pipeline ingress stage
     */
    BCM_FIELD_QSET_INIT(grp->qset);
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyAppType);
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyL2DestHit);
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyIpmcHit);
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyStageIngress);

    /*  Create the Field group */
    grp->priority = group_priority;
    result = bcm_field_group_config_create(unit, grp);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_create returned %s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    /*
     *  Define the ASET Field Group
     */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDoNotLearn);

    /*  Attach the action set */
    result = bcm_field_group_action_set(unit, grp->group, aset);
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_group_action_set returned %s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    /*
     *  Add two entries to the Field group.
     *  Match the packets on the l3 source bind is done and look up result.
     *    1. Lookup is done and LEM look up result is found: 
     *       do nothing(IPV4 DHCP, IPV4 STATIC and IPV6 DHCP)
     *    2. Lookup is done and TCAM look up result is found: 
     *       do nothing(IPV6 STATIC)
     *    3. lookup is done and look up result isn't found: drop
     *    4. lookup isn't done: do nothing
     */
    for (i = 0 ; i < 4; i++)
    {
        result = bcm_field_entry_create(unit, grp->group, ent[i]);
        if (BCM_E_NONE != result)
        {
            printf("Error %d in bcm_field_entry_create returned %s\n", i, bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
    }

    /* For IPV4 DHCP, IPV4 STATIC and IPV6 DHCP, 
     * lookup is done and LEM result is found: do nothing 
     */
    for (i = 0; i < 4; i++)
    {
        result = bcm_field_qualify_AppType(unit, *(ent[0]), app_type[i]);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[0] in bcm_field_qualify_AppType returned %s\n", bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
    }

    result = bcm_field_qualify_L2DestHit(unit, *(ent[0]), 0x1, 0x01);
    if (BCM_E_NONE != result)
    {
        printf("Error ent[0] in bcm_field_qualify_L2DestHit returned %s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    /* For IPV6 STATIC, lookup is done and TCAM result is found: do nothing */
    for (i = 0; i < 4; i++)
    {
        result = bcm_field_qualify_AppType(unit, *(ent[1]), app_type[i]);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[1] in bcm_field_qualify_AppType returned %s\n", bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
    }

    result = bcm_field_qualify_IpmcHit(unit, *(ent[1]), 0x1, 0x01);
    if (BCM_E_NONE != result)
    {
        printf("Error ent[1] in bcm_petra_field_qualify_IpmcHit returned %s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }
    
    /* lookup is done and look up result is not-found:drop */
    for (i = 0; i < 4; i++)
    {
        result = bcm_field_qualify_AppType(unit, *(ent[2]), app_type[i]);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[2] in bcm_field_qualify_AppType returned %s\n", bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
    }
 
    result = bcm_field_action_add(unit, *(ent[2]), bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result)
    {
        printf("Error ent[2] in bcm_field_action_add returned %s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    result = bcm_field_action_add(unit, *(ent[2]), bcmFieldActionDoNotLearn, 0, 0);
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_action_add ent[2] - bcmFieldActionDoNotLearn result:%s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    /* others: do nothing */

    /* install */
    result = bcm_field_group_install(unit, grp->group);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_install returned %s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    /*
     *  Everything went well.
     */
    return result;
}

/*********************************************************************************
* Delete the field group which is create by pon_anti_spoofing_drop().
*
* Steps
*     1. Delete  and destory entries
*     2. Destory field group
*
*/
int pon_anti_spoofing_drop_destory(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int i = 0;

    for (i = 0; i < 4; i++)
    {
        rv = bcm_field_entry_remove(unit, pon_anti_spoofing_info.ent[i]);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_field_entry_remove %d returned %s\n", i, bcm_errmsg(rv));
            return rv;
        }

        rv = bcm_field_entry_destroy(unit, pon_anti_spoofing_info.ent[i]);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_field_entry_destroy %d returned %s\n", i, bcm_errmsg(rv));
            return rv;
        }
    }
    
    rv = bcm_field_group_destroy(unit, pon_anti_spoofing_info.grp.group);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_group_destroy returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*********************************************************************************
* clean all configuration. include FG, anti-spoofing and pon service
*
* Steps
*     1. clean FG configuration
*     2. clean anti-spoofing configuration
*     3. clean pon service configuration
*
*/
int pon_anti_spoofing_app_clean(int unit)
{
    int rv = 0;

    rv = pon_anti_spoofing_drop_destory(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_anti_spoofing_drop_destory returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = pon_anti_spoofing_disable(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_anti_spoofing_disable returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Delete pon gport */
    rv = vswitch_delete_port(unit, pon_anti_spoofing_info.vsi, pon_anti_spoofing_info.pon_gport);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, vswitch_delete_port returned %s remove pon gport failed\n", bcm_errmsg(rv));
        return rv;
    }

    /* Delete nni gport */
    rv = vswitch_delete_port(unit, pon_anti_spoofing_info.vsi, pon_anti_spoofing_info.nni_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_delete_port returned %s remove nni gport failed\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vswitch_delete(unit, pon_anti_spoofing_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_delete returned %s remove vsi failed\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}