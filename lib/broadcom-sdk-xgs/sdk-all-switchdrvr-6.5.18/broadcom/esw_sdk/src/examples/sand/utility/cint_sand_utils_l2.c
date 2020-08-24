/* $Id: cint_utils_l2.c,v 1.10 Broadcom SDK $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * This file provides L2 basic functionality and defines L2 global variables
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 *************************************************************************************************** */


static int OLP_DEFAULT_CORE = 0;

/* *************************************************************************************************** */


/* Return the default OLP port */
int l2__olp_port__get(int unit, bcm_gport_t *port){

    bcm_gport_t olp[BCM_PIPES_MAX];
    int count, rv = BCM_E_NONE;

    rv = bcm_port_internal_get(unit,BCM_PORT_INTERNAL_OLP,BCM_PIPES_MAX,olp,&count);
    if (rv != BCM_E_NONE) {
        printf("Error, l2__olp_port__get\n");
        return rv;
    }
    if (count < 1) {
        printf("Error, No olp port configured\n");
        return BCM_E_NOF_FOUND;
    }

    *port = olp[OLP_DEFAULT_CORE];

    return BCM_E_NONE;
}

/* Create L2 Logical port according <port>*/
int l2__port__create(int unit, int flags,  bcm_gport_t port, int vsi, bcm_gport_t *gport ){

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);

    if(is_device_or_above(unit, JERICHO2))
    {
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    }
    else
    {
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT;
    }
    vp.port = port;
    vp.vsi = vsi;
    vp.flags = flags;
    vp.vlan_port_id = *gport;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        print rv;
        return rv;
    }

    *gport = vp.vlan_port_id;

    return BCM_E_NONE;

}

/* Create L2 Logical port according <port> <vlan>*/
int l2__port_vlan__create(int unit, int flags,  bcm_gport_t port, int vsi, bcm_gport_t *gport, int vlan){

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);

    if(is_device_or_above(unit, JERICHO2))
    {
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    }
    else
    {
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT;
    }
    vp.port = port;
    vp.vsi = vsi;
    vp.flags = flags;
    vp.vlan_port_id = *gport;
    vp.match_vlan = vlan;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        print rv;
        return rv;
    }

    *gport = vp.vlan_port_id;

    return BCM_E_NONE;

}


