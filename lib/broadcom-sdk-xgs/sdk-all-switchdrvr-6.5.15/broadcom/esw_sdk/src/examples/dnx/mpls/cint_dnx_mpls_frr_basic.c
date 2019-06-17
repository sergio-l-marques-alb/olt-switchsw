/*
 * $Id: cint_dnx_mpls_frr_basic.c, Exp $
 $Copyright: (c) 2018 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/dpp/utility/cint_utils_global.c
 * cint ../../../../src/examples/sand/cint_sand_utils_l3.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../../../src/examples/dnx/cint_dnx_mpls_frr_basic.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int port1 = 200;
 * int vlan_id = 100;
 * int frr_label = 0x1111
 * rv = mpls_upstream_assigned(unit,port1,vlan_id,frr_label);
 * print rv;
 *
 * Set the in port to support MPLS FRR
 * Configure the MPLS FRR label in FRR table
 *
 *  ###################################################################################################
 */

int
mpls_frr_basic_main(
    int unit,
    int port,
    int vlan,
    int frr_label)
{
    int rv = BCM_E_NONE;
    uint32 flags = 0;
    bcm_mpls_special_label_t label_info = { 0 };

    rv = bcm_port_control_set(unit, port, bcmPortControlMplsFRREnable, 1);
    if(rv)
    {
        printf("Error in bcm_port_control_set\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vlan, port, 0);
    if(rv)
    {
        printf("Error in bcm_vlan_gport_add\n");
        return rv;
    }

    label_info.label_value = frr_label;
    rv = bcm_mpls_special_label_identifier_add(unit, bcmMplsSpecialLabelTypeFrr, label_info);
    if(rv)
    {
        printf("Error in bcm_mpls_special_label_identifier_add\n");
        return rv;
    }

    return rv;
}
