/*
 * $Id: cint_vpls_mpls_qos.c, Exp $
 $Copyright: (c) 2019 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$ File: cint_vpls_mpls_qos.c Purpose: utility for MPLS TANDEM over VPLS with QoS.
 */

/*
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/dpp/utility/cint_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../../../src/examples/dnx/vpls/cint_vpls_mpls_qos.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int in_port = 200;
 * int out_port = 201;
 * int mode = 0;
 * rv = vpls_mpls_qos_main(unit,in_port,out_port,mode);
 * print rv;
 *
 *

 *  Scenarios configured in this cint:
 *  In each cint itteration only 1 scenario is configured, based on defined 'mode'
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   mode = 0 - basic with 1 MPLS label
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core.
 *  Exit with a packet including an 1 MPLS and PWE labels.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan    | tpid
 *   |    |0c:00:02:00:00       |11:00:00:01:12    || 5         | 0x8100
 *   |    |                         |                       ||          ||               ||      ||
 *   |    |                         |                       ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||   MPLS   ||  PWE         ||      DA                  |     SA                || vlan | tpid
 *   |    |0c:00:02:00:01       |00:00:00:cd:1d   ||Label:0xD05||Label:0xD80  || 0c:00:02:00:00   |11:00:00:01:12  || 1111 | 0x8100
 *   |    |                         |                       ||Exp:5     ||Exp:6            ||                          ||                      ||
 *   |    |                         |                       ||TTL:20    ||                  ||                          ||                      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   mode = 1 - basic with 2 MPLS labels
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core.
 *  Exit with a packet including an 2 MPLS and PWE labels.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan    | tpid
 *   |    |0c:00:02:00:00       |11:00:00:01:12    || 5         | 0x8100
 *   |    |                         |                       ||          ||               ||      ||
 *   |    |                         |                       ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||  MPLS        ||   MPLS   ||  PWE         ||      DA                  |     SA                || vlan | tpid
 *   |    |0c:00:02:00:01       |00:00:00:cd:1d   ||Label:0x4444||Label:0xD05||Label:0xD80  || 0c:00:02:00:00   |11:00:00:01:12  || 1111 | 0x8100
 *   |    |                         |                       ||Exp:5         ||Exp:5     ||Exp:6             ||                          ||                      ||
 *   |    |                         |                      ||TTL:20          ||TTL:20    ||                  ||                          ||                      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   mode = 2 - basic with 1 MPLS with 1 TANDEM labels
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core.
 *  Exit with a packet including an 1 MPLS, 1 TANDEM and PWE labels.
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan    | tpid
 *   |    |0c:00:02:00:00       |11:00:00:01:12    || 5         | 0x8100
 *   |    |                         |                       ||          ||               ||      ||
 *   |    |                         |                       ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||  MPLS        ||   MPLS       ||  PWE         ||      DA                  |     SA                || vlan | tpid
 *   |    |0c:00:02:00:01       |00:00:00:cd:1d   ||Label:0x5656||Label:0xD05||Label:0xD80  || 0c:00:02:00:00   |11:00:00:01:12  || 1111 | 0x8100
 *   |    |                         |                       ||Exp:5         ||Exp:5         ||Exp:6           ||                          ||                      ||
 *   |    |                         |                      ||TTL:20          ||TTL:20    ||                  ||                          ||                      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 */

int MPLS_1_TANDEM_0_MODE = 0;
int MPLS_2_TANDEM_0_MODE = 1;
int MPLS_1_TANDEM_1_MODE = 2;

int
vpls_mpls_qos_main(
    int unit,
    int port1,
    int port2,
    int mode)
{
    int rv = BCM_E_NONE;

    init_default_vpls_params(unit);

    /** set EXP to UNIFORM mode in egress */
    mpls_encap_tunnel[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    pwe_encap.egress_qos_model.egress_qos = bcmQosEgressModelUniform;

    /** set EXP UNIFORM mode in ingress */
    pwe_term.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    pwe_term.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    pwe_term.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    pwe_term.qos_map_id = 3;

    switch (mode)
    {
        case MPLS_1_TANDEM_0_MODE:
            outer_mpls_tunnel_index = 0;
            break;
        case MPLS_2_TANDEM_0_MODE:
            mpls_encap_tunnel[0].num_labels = 2;
            mpls_encap_tunnel[0].label[1] = 0x4444;

            outer_mpls_tunnel_index = 0;
            break;
        case MPLS_1_TANDEM_1_MODE:
            mpls_encap_tunnel[1].label[0] = 0x5656;
            mpls_encap_tunnel[1].num_labels = 1;
            mpls_encap_tunnel[1].encap_access = bcmEncapAccessTunnel3;;
            mpls_encap_tunnel[1].tunnel_id = 8196;
            mpls_encap_tunnel[1].flags = mpls_encap_tunnel[0].flags | BCM_MPLS_EGRESS_LABEL_TANDEM;

            outer_mpls_tunnel_index = 1;
            break;
        default:
            rv = BCM_E_PARAM;
            printf("Error(%d), invalid test mode(%d) - valid range is %d-%d \n", rv, mode, MPLS_1_TANDEM_0_MODE,
                   MPLS_1_TANDEM_1_MODE);
            return rv;
            break;
    }

    rv = vpls_main(unit, port1, port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_check_support\n", rv);
        return rv;
    }

    return rv;
}

/*
 * Connect GPORT (VLAN-Port) to its QOS-profile
 */
int
vpls_mpls_qos_map_gport(
    int unit,
    int ingress_qos_profile,
    int egress_qos_profile)
{
    int rv = BCM_E_NONE;
    int i;
    int flags;
    bcm_gport_t gport;
    bcm_l3_egress_t l3eg;
    int ingress_qos_map_id;
    int egress_qos_map_id;

    rv = bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK,
                                       ingress_qos_profile, &ingress_qos_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_qos_map_id_get_by_profile\n");
        return rv;
    }

    rv = bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK,
                                       egress_qos_profile, &egress_qos_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_qos_map_id_get_by_profile\n");
        return rv;
    }

    if (ingress_qos_profile != -1)
    {
        rv = bcm_qos_port_map_set(unit, ac_port.vlan_port_id, ingress_qos_map_id, -1);
        if (rv != BCM_E_NONE)
        {
            printf("error bcm_qos_port_map_set setting up ingress gport(0x%08x) to map\n", ac_port.vlan_port_id);
            return rv;
        }
    }

    if (egress_qos_profile != -1)
    {
        /** set remark profile to pwe*/
        rv = bcm_qos_port_map_set(unit, pwe_encap.mpls_port_id, -1, egress_qos_map_id);
        if (rv != BCM_E_NONE)
        {
            printf("error bcm_qos_port_map_set setting up egress gport(0x%08x) to map\n", pwe_encap.mpls_port_id);
            return rv;
        }

        /** update tunnel with egress qos map id*/
        for (i = MAX_NOF_TUNNELS - 1; i >= 0; i--)
        {
            mpls_encap_tunnel[i].qos_map_id = egress_qos_map_id;
            mpls_encap_tunnel[i].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_REPLACE;
        }

        rv = vpls_create_mpls_tunnel(unit, mpls_encap_tunnel);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in vpls_create_mpls_tunnel\n");
            return rv;
        }

        /** update link layer with egress qos map id*/
        for (i = MAX_NOF_TUNNELS - 1; i >= 0; i--)
        {
            gport = mpls_encap_tunnel[i].l3_intf_id;
            print gport;
            if(BCM_L3_ITF_TYPE_IS_LIF(gport))
            {
                int flags;
                bcm_l3_egress_t l3eg;
                bcm_l3_egress_t_init(l3eg);
                rv = bcm_l3_egress_get(unit,gport,&l3eg);
                if (rv != BCM_E_NONE) {
                    printf ("bcm_l3_egress_get failed: %d \n", rv);
                }
                l3eg.qos_map_id = egress_qos_map_id;
                flags=BCM_L3_REPLACE | BCM_L3_WITH_ID | BCM_L3_EGRESS_ONLY;
                rv = bcm_l3_egress_create(unit,flags,&l3eg,gport);
                if (rv != BCM_E_NONE) {
                    printf ("bcm_l3_egress_create failed: %d \n", rv);
                }
                break;
            }
        }
    }

    return rv;
}

/**
 * \brief
 *
 * This function performs the following steps:
 * 1. Sets port tpid class:
 * 2. Set a vlan translation command (replace outer) with the
 * following parameters: edit_profile=5, tag_format=1,
 * new_vlan=1111
 */
int
vpls_mpls_qos_ve_swap(
    int unit,
    int port,
    int is_ive)
{
    int rv = BCM_E_NONE;

    bcm_port_tpid_class_t port_tpid_class;
    /*
     * set tag format
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 4;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    if (is_ive == 0)
    {
        port_tpid_class.flags |= BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    }
    else if (is_ive == 1)
    {
        port_tpid_class.flags |= BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    }
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }

    /*
     * configure ive swap
     */
    rv = ive_eve_translation_set(unit, ac_port.vlan_port_id, 0x8100, 0, bcmVlanActionReplace, bcmVlanActionNone, 1111, 0, 5     /* edit_profile
                                  */ , 4 /* tag_format */ , is_ive);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in ive_eve_translation_set\n", rv);
        return rv;
    }
    return rv;
}

/**
 * \brief
 *
 * This function performs the following steps:
 * 1. Sets port tpid class
 * 2. Set a vlan translation command (replace outer, and add
 * priority parameters) with the following parameters:
 * edit_profile=5, tag_format=4, new_vlan=1111
 */
int
vpls_mpls_qos_ve_pri_action_swap(
    int unit,
    int port,
    int is_ive)
{
    int rv = BCM_E_NONE;

    bcm_port_tpid_class_t port_tpid_class;
    /*
     * set tag format
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 4;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    if (is_ive == 0)
    {
        port_tpid_class.flags |= BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    }
    else if (is_ive == 1)
    {
        port_tpid_class.flags |= BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    }
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }

    /*
     * configure ive swap
     */
    rv = vlan_translate_ive_eve_translation_set_with_pri_action(unit, ac_port.vlan_port_id, 0x8100, 0x09100, bcmVlanTpidActionNone, bcmVlanTpidActionNone, bcmVlanActionReplace, bcmVlanActionNone, bcmVlanActionAdd, bcmVlanActionNone, 1111, 0, 5     /* edit_profile
                                                                 */ , 4 /* tag_format */ ,
                                                                is_ive);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vlan_translate_ive_eve_translation_set_with_pri_action\n", rv);
        return rv;
    }
    return rv;
}

/*all egress remark profile and add mapping, used in case pipeNextNameSpace qos model*/
int vpls_mpls_qos_egress_profile(int unit, int profile_id)
{
    int egress_qos_profile;
    bcm_qos_map_t mpls_eg_map;
    int map_opcode;
    int flags = 0;
    int qos_var;
    int color;
    int exp = 0;
    int rv;

    /** Create QOS profile */
    if (profile_id !=0)
    {
        egress_qos_profile = profile_id;
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID, &egress_qos_profile);
    }
    else
    {
        rv = bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK, 0, &egress_qos_profile);
    }
    if (rv != BCM_E_NONE)
    {
        printf("error in mpls egress bcm_qos_map_create()\n");
        return rv;
    }
    /*save egress qos map id*/
    mpls_encap_tunnel[0].qos_map_id = egress_qos_profile;

    /** Create QOS opcode */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE, &map_opcode);
    if (rv != BCM_E_NONE)
    {
        printf("error in mpls egress opcode bcm_qos_map_create()\n");
        return rv;
    }

    /** Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID */
    bcm_qos_map_t_init(&mpls_eg_map);
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    mpls_eg_map.opcode = map_opcode;
    rv = bcm_qos_map_add(unit, flags, &mpls_eg_map, egress_qos_profile);
    if (rv != BCM_E_NONE)
    {
        printf("error in mpls egress bcm_qos_map_add()\n");
        return rv;
    }

    /*
     * COLOR, INT_PRI(NEWK_WOS) -> remark_int_pri (Out-EXP) 
     * 6, 7 are exp which set in pw and tunnel separately 
     * remark out exp to 5
     */
    bcm_qos_map_t_init(&mpls_eg_map);
    for (qos_var = 6; qos_var < 8; qos_var++)
    {
        for (color = 0; color < 4; color++)
        {
            /** Clear structure */
            bcm_qos_map_t_init(&mpls_eg_map);
            flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK;
            mpls_eg_map.color = color;
            mpls_eg_map.int_pri = qos_var;
            mpls_eg_map.exp = 5;
            rv = bcm_qos_map_add(unit, flags, &mpls_eg_map, map_opcode);
            if (rv != BCM_E_NONE)
            {
                printf("error in mpls egress bcm_qos_map_add()\n");
                printf("rv is: $rv \n");
                printf("(%s)\n", bcm_errmsg(rv));
                return rv;
            }
        }
    }
    return rv;
}

/**qos pipe model test main function*/
int vpls_mpls_qos_model_main(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;

    init_default_vpls_params(unit);

    /** set EXP to UNIFORM mode in egress */
    mpls_encap_tunnel[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    mpls_encap_tunnel[0].exp = 6;
    pwe_encap.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    pwe_encap.exp = 7;

    outer_mpls_tunnel_index = 0;

    rv = vpls_main(unit, port1, port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_check_support\n", rv);
        return rv;
    }

    return rv;
}

/*update pw and tunnel qos model from pipeNextNameSpace to pipeMyNameSpace*/
int vpls_mpls_qos_model_update(int unit)
{
    int rv = BCM_E_NONE;
    int i;

    pwe_encap.egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
    pwe_encap.flags |= (BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_REPLACE);
    rv = vpls_mpls_port_add_encapsulation(unit, &pwe_encap);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mpls_port_add_encapsulation\n", rv);
        return rv;
    }

    for (i = MAX_NOF_TUNNELS - 1; i >= 0; i--)
    {
        mpls_encap_tunnel[i].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;;
        mpls_encap_tunnel[i].flags |= (BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_REPLACE);
    }

    rv = vpls_create_mpls_tunnel(unit, mpls_encap_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vpls_create_mpls_tunnel\n");
        return rv;
    }
    return rv;
}
