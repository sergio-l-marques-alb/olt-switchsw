/*
 * $Id$
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$ 
 *
 *File: cint_dnx_qos_ecn.c
 * Purpose: An example of QOS ECN mapping
 *
 * 1. This examples create ingress ECN QOS profile and add mapping, 
 *       It enables ECN and sets extraction of ECN (Eligible and congestion bits). 
 * 2. This examples create egress remark profile and add mapping with ecn.
 *       It remark output packet's QOS field and update ECN bits with FTMH.CNI and FTMH.ECN-Eligible. 
 * 3. This examples also configure IPV4 forwarding and
 *       apply ingress ECN mapping and egress remark mapping with ECN for output packet QOS.
 *
 *
 * Set up sequence:
 *    1. create ingress ECN profile and add mapping
 *    2. create egress remark profile and add mapping with ecn
 *    3. create IPv4 forward service
 *    4. update qos profile of IPv4 forward service 
 *
 * run traffic: 
 *     IPV4 header with random TOS
 * expected: 
 *     IPV4 header TOS is same as input without application ECN profile at ingress
 *     IPV4 header TOS ECN bits are 3 with application ECN profile at ingress
 * 
 *
 * copy to /usr/local/sbin location, run bcm.user
 * Run script:
 *    cd ../../../..
 *    cint src/examples/sand/cint_ip_route_basic.c
 *    cint src/examples/dnx/qos/cint_dnx_qos_ecn.c
 *    cint
 *    dnx_ecn_basic_example(unit, <in_port>, <out_port>);
 */

static int in_ecn_map_id=-1;
static int eg_map_id=-1;

static int in_ecn_opcode;
static int eg_ecn_opcode;
static int eg_remark_opcode;

int
qos_map_id_egress_get(int unit)
{
    return eg_map_id;
}

int
qos_ecn_map_id_ingress_get(int unit)
{
    return in_ecn_map_id;
}

/**create ingress ecn profile and add ecn mapping, CNI = 1,  Eligible = 1*/
int
qos_map_ingress_ecn_remark(int unit)
{
    bcm_qos_map_t l3_in_map;
    int flags = 0;
    int dscp;
    int rv = BCM_E_NONE;

    /* Clear structure */
    bcm_qos_map_t_init(&l3_in_map);

    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_ECN;
    if(in_ecn_map_id != -1)
    {
        flags |= BCM_QOS_MAP_WITH_ID;
    }
    if(in_ecn_map_id != 0)
    {
        rv = bcm_qos_map_create(unit, flags, &in_ecn_map_id);
    }
    else
    {
        rv = bcm_qos_map_id_get_by_profile(unit, flags, in_ecn_map_id, &in_ecn_map_id);
    }
    if (rv != BCM_E_NONE) {
        printf("error in ingress qos profile create\n");
        return rv;
    }

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_ECN | BCM_QOS_MAP_OPCODE, &in_ecn_opcode);
    if (rv != BCM_E_NONE) {
        printf("error in ingress l3 opcode bcm_qos_map_create()\n");
        return rv;
    }

    l3_in_map.opcode = in_ecn_opcode;
    /* 
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol 
     */ 
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 |  BCM_QOS_MAP_ECN | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_in_map, in_ecn_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in L3 ingress bcm_qos_map_add()\n");
        return rv;
    }

    /* 
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol 
     */ 
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 |  BCM_QOS_MAP_ECN | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_in_map, in_ecn_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in L3 ingress bcm_qos_map_add()\n");
        return rv;
    }

    for (dscp=0; dscp<256; dscp++) {
        bcm_qos_map_t_init(&l3_in_map);
        if (dscp < 128) {
              flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 |  BCM_QOS_MAP_ECN ;
        } else {
              flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_ECN;
        }
        /* Set ingress dscp Priority */
        l3_in_map.dscp = dscp;
        /* Set CNI=0, Eligible=1 for this ingress DSCP  */
        l3_in_map.int_pri = 0x1;
        rv = bcm_qos_map_add(unit, flags, &l3_in_map, in_ecn_opcode);
        if (rv != BCM_E_NONE) {
            printf("error in L3 ingress bcm_qos_map_add()\n");
            return rv;
      	}
    }
    return rv;
}

/**create egress qos remark profile, add remark mapping without/with ecn,
   TOS 1:1 mapping, output ECN bits should be updated when ingress ECN eligible enabled*/
int
qos_map_egress_remark_without_and_with_ecn(int unit)
{
    bcm_qos_map_t l3_eg_map;
    int flags = 0;
    int dscp;
    int dp;
    int rv;
 
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    if(eg_map_id !=-1)
    {
        flags |= BCM_QOS_MAP_WITH_ID;
    }
    if(eg_map_id != 0)
    {
       rv = bcm_qos_map_create(unit, flags, &eg_map_id);
    }
    {
        rv = bcm_qos_map_id_get_by_profile(unit, flags, eg_map_id, &eg_map_id);
    }
    if (rv != BCM_E_NONE) {
        printf("error in L3 egress qos profile create\n");
        return rv;
    }

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_ECN;
    rv = bcm_qos_map_create(unit, flags, &eg_ecn_opcode);
    if (rv != BCM_E_NONE) {
        printf("error in ingress l3 opcode bcm_qos_map_create()\n");
        return rv;
    }

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_create(unit, flags, &eg_remark_opcode);
    if (rv != BCM_E_NONE) {
        printf("error in ingress l3 opcode bcm_qos_map_create()\n");
        return rv;
    }

    /*
     *start add mapping without ecn
     */
    bcm_qos_map_t_init(&l3_eg_map);
    l3_eg_map.opcode = eg_remark_opcode;
    /** Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID */ 
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_eg_map, eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_add()\n");
        return rv;
    }

    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_eg_map, eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_add()\n");
        return rv;
    }

    /*add mapping */
    for (dscp=0; dscp<256; dscp++) {
        for (dp=0; dp < 4; dp++) {
            bcm_qos_map_t_init(&l3_eg_map);
            l3_eg_map.int_pri = dscp;
            if (dscp < 128) {
                flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK;
            } else {
                flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK;
            }

            /* Set internal color */
            if (dp == 0){
                l3_eg_map.color = bcmColorGreen;
            } else {
                l3_eg_map.color = bcmColorYellow;
            }
            /* Set egress DSCP */
            l3_eg_map.dscp = dscp;
            rv = bcm_qos_map_add(unit, flags, &l3_eg_map, eg_remark_opcode);
            if (rv != BCM_E_NONE) {
                 printf("error in L3 egress bcm_qos_map_add()\n");
                 return rv;
            }
        }
    }

    /*
     *end mapping without ecn, start mapping with ecn
     */
    bcm_qos_map_t_init(&l3_eg_map);
    l3_eg_map.opcode = eg_ecn_opcode;
    /** Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID */ 
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_ECN | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_eg_map, eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_add()\n");
        return rv;
    }

    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_ECN | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_eg_map, eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_add()\n");
        return rv;
    }

    /*add qos 1:1 mapping */
    for (dscp=0; dscp<256; dscp++) {
        for (dp=0; dp < 4; dp++) {
            bcm_qos_map_t_init(&l3_eg_map);

            l3_eg_map.int_pri = dscp;
            if (dscp < 128) {
                flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_ECN;
            } else {
                flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_ECN;
            }

            /* Set internal color */
            if (dp == 0){
                l3_eg_map.color = bcmColorGreen;
            } else {
                l3_eg_map.color = bcmColorYellow;
            }
            /* Set egress DSCP */
            l3_eg_map.dscp = dscp;
            rv = bcm_qos_map_add(unit, flags, &l3_eg_map, eg_ecn_opcode);
            if (rv != BCM_E_NONE) {
                 printf("error in L3 egress bcm_qos_map_add()\n");
                 return rv;
            }
        }
    }

    /*
     *end mapping with ecn
     */
    return rv;
}

/*ecn example main*/
int dnx_ecn_basic_example(int unit, int in_port, int out_port, int egress_qos_profile)
{
    int rv = BCM_E_NONE;

    if (egress_qos_profile != -1)
    {
        eg_map_id = egress_qos_profile;
    }

    rv = qos_map_egress_remark_without_and_with_ecn(unit);
    if (rv) {
        printf("error setting up egress qos profile\n");
        return rv;
    }
  
    rv = basic_example_qos(unit, in_port, out_port, 0, egress_qos_profile);
    if (rv) {
        printf("error setting up egress qos profile\n");
        return rv;
    }

    return rv;
}

/*update ecn profile in IN-RIF, so that ingress ecn mapping enabled*/
int dnx_ecn_profile_port_set(int unit, int ingress_ecn_profile)
{
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int vrf = 1;
    int rv;
    bcm_l3_ingress_t l3_ing_if;

    if(ingress_ecn_profile != -1)
    {
        in_ecn_map_id = ingress_ecn_profile;
    }

    rv = qos_map_ingress_ecn_remark(unit);
    if (rv) {
        printf("error setting up ingress ecn profile\n");
        return rv;
    }

    rv = bcm_l3_ingress_get(unit, intf_in, &l3_ing_if);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_get\n");
        return rv;
    }
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;   /* must, as we update exist RIF */
    l3_ing_if.vrf = vrf;
    
    /* test case use ingress_ecn_profile 0 as workarround. 
       Usually if it is not 0, here updates ecn mapping profile on in-RIF*/
    /*l3_ing_if.ecn_map_id = in_ecn_map_id;*/

    rv = bcm_l3_ingress_create(unit, l3_ing_if, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }
    return rv;
}

