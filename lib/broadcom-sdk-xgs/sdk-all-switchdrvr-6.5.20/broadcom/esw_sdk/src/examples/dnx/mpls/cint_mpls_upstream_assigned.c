/*
 * $Id: cint_mpls_upstream_assigned.c, Exp $
 $Copyright: (c) 2020 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 * 
 * Configuration:
 * 
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../../../src/examples/dnx/mpls/cint_mpls_upstream_assigned.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int port1 = 200; 
 * int port2 = 201;
 * rv = mpls_upstream_assigned_main(unit,port1,port2);
 * print rv; 
 * 
 * 
 * This cint uses cint_dnx_utility_mpls.c to configure MPLS setup with tunnel lookups based on 2 labels.
 *
 *
 *  Scenarios configured in this cint:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  1) Basic upstream assigned MPLS forwarding
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Two MPLS labels are swapped. Lookup in ILM results in an EEDB entry holding swap information.
 *  The ILM points to the EEDB (outlif) and to a FEC entry (holding the next hop information).
 *
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ###################################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA         | SA              ||   MPLS   ||   MPLS   ||  SIP          ||  DIP          ||
 *   |    |00:0c:00:02:00:01|00:0c:00:02:00:00||Label:6666||Label:7777||160.161.161.162||160.161.161.163||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA         | SA              ||   MPLS   ||  SIP          ||  DIP          ||
 *   |    |00:11:00:00:01:12|00:00:00:00:cd:1d||Label:5555||160.161.161.162||160.161.161.163||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  ###################################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   2) Basic MPLS upstream assigned termination
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Two MPLS label are terminated. Lookup in routing table results in  IP forwarding.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ###################################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA         | SA              ||   MPLS   ||   MPLS   ||  SIP          ||  DIP          ||
 *   |    |00:00:00:00:cd:1d|00:11:00:00:01:12||Label:6666||Label:7777||160.161.161.163||160.161.161.162||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA         | SA              ||  SIP          ||  DIP          ||
 *   |    |00:0c:00:02:00:00|00:0c:00:02:00:01||160.161.161.163||160.161.161.162||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  ###################################################################################################
 */

int
mpls_upstream_assigned_main(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;

    init_default_mpls_params(unit, port1, port2);

    /** configure termination entry */
    mpls_util_entity[1].mpls_switch_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_util_entity[1].mpls_switch_tunnel[0].flags = BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL;
    mpls_util_entity[1].mpls_switch_tunnel[0].label = 8888;
    mpls_util_entity[1].mpls_switch_tunnel[0].second_label= 9999;

    /** configure forwarding entry */
    mpls_util_entity[0].mpls_switch_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_NOP;
    mpls_util_entity[0].mpls_switch_tunnel[0].flags = BCM_MPLS_SWITCH_TTL_DECREMENT | BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL;
    mpls_util_entity[0].mpls_switch_tunnel[0].label = 6666;
    mpls_util_entity[0].mpls_switch_tunnel[0].second_label = 7777;
    mpls_util_entity[0].mpls_switch_tunnel[0].port = &mpls_util_entity[0].fecs[0].fec_id;
    mpls_util_entity[0].mpls_switch_tunnel[0].egress_if = NULL;

    rv = mpls_util_main(unit, port1, port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in mpls_util_main\n", rv);
        return rv;
    }

    return rv;
}
