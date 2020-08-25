/*
 * $Id$
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$ 
 *
 * Cint QOS Setup example code
 *
 * copy to /usr/local/sbin location, run bcm.user
 * Run script:
 *
   cd ../../../src/examples/dpp
   cint cint_qos_l3.c
   cint
 */
 
   /* Map ingress and egress as follows
    * Ingress DSCP     INTERNAL TC/DP     Egress DSCP
    *     IPV4
    *     0-3              0/GREEN         1
    *     4-7              0/YELLOW        2
    *     8-11             1/GREEN         3
    *     12-15            1/YELLOW        4
    *     16-19            2/GREEN         5
    *     20-23            2/YELLOW        6
    *     24-27            3/GREEN         7
    *     28-31            3/RED           8
    *     32-63            unmapped 0/GREEN
    * Ingress DSCP     INTERNAL TC/DP     Egress DSCP
    *     IPV6
    *     32-35           4/GREEN           9
    *     36-39           4/RED            10
    *     40-43           5/GREEN          11
    *     44-47           5/RED            12
    *     48-51           6/GREEN          13
    *     52-55           6/RED            14
    *     56-59           7/GREEN          15
    *     60-63           7/RED            16
    */
int qos_map_l3_internal_pri[16] = {1, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};

bcm_color_t qos_map_l3_internal_color[16] = {bcmColorGreen,
					     bcmColorYellow,
					     bcmColorGreen,
					     bcmColorYellow,
					     bcmColorGreen,
					     bcmColorYellow,
					     bcmColorGreen,
					     bcmColorRed,
					     bcmColorGreen,
					     bcmColorRed,
					     bcmColorGreen,
					     bcmColorRed,
					     bcmColorGreen,
					     bcmColorRed,
					     bcmColorGreen,
					     bcmColorRed};

int qos_map_l3_remark_pri[16] = {12, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64};

int map_ids[20];
static int map_id_cnt = 0;

static int l3_in_map_id = 7;
static int l3_eg_map_id = 0;

static int l3_in_opcode_id = 25;
static int l3_eg_opcode_id = 27;

int qos_l3_map_to_l2 = 0;
int
qos_map_id_l3_egress_get(int unit)
{
    return l3_eg_map_id;
}

int
qos_map_id_l3_ingress_get(int unit)
{
    return l3_in_map_id;
}

int
qos_map_l3_ingress_remark_profile(int unit)
{
    bcm_qos_map_t l3_in_map;
    int flags = 0;
    int dscp;
    int rv = BCM_E_NONE;

    /* Clear structure */

    bcm_qos_map_t_init(&l3_in_map);
    
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID, &l3_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in ingress l3 bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID, &l3_in_opcode_id);
    if (rv != BCM_E_NONE) {
        printf("error in ingress l3 opcode bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    map_ids[map_id_cnt++] = l3_in_map_id;

    l3_in_map.opcode = l3_in_opcode_id;

    /* 
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol 
     */ 
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 |  BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id);
      	
    if (rv != BCM_E_NONE) {
        printf("error in L3 ingress bcm_qos_map_add()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /* 
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol 
     */ 
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 |  BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id);

    if (rv != BCM_E_NONE) {
        printf("error in L3 ingress bcm_qos_map_add()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    
    for (dscp=0; dscp<64; dscp++) {
        bcm_qos_map_t_init(&l3_in_map);
      	if (dscp < 32) {
      	    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 |  BCM_QOS_MAP_REMARK ;
      	} else {
      	    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK;
      	}

        /* Set ingress dscp Priority */
      	l3_in_map.dscp = dscp << 2;
      	
      	/* Set internal priority for this ingress DSCP  */
      	l3_in_map.int_pri = qos_map_l3_internal_pri[dscp>>2];
        l3_in_map.remark_int_pri = qos_map_l3_internal_pri[dscp>>2];
      	
      	/* Set color for this ingress DSCP Priority  */
      	l3_in_map.color = qos_map_l3_internal_color[dscp>>2];
      	
      	rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_opcode_id);
      	
      	if (rv != BCM_E_NONE) {
      	    printf("error in L3 ingress bcm_qos_map_add()\n");
      	    printf("rv is: $rv \n");
      	    printf("(%s)\n", bcm_errmsg(rv));
      	    return rv;
      	}
    }
    return rv;
}

int
qos_map_l3_egress_remark_profile(int unit)
{
    bcm_qos_map_t l3_eg_map;
    int flags = 0;
    int dscp;
    int MAX_PRI = 64;
    int dp;
    int rv;
 
    /* Clear structure */
    bcm_qos_map_t_init(&l3_eg_map);

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID, &l3_eg_map_id);

    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
    } else {
        map_ids[map_id_cnt++] = l3_eg_map_id;
    }

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID, &l3_eg_opcode_id);
    if (rv != BCM_E_NONE) {
        printf("error in ingress l3 opcode bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    l3_eg_map.opcode = l3_eg_opcode_id;
    /* 
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol 
     */ 
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_map_id);
    	
    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_add()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_map_id);

    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_add()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /*add current layer mapping */
    for (dscp=0; dscp<MAX_PRI; dscp++) {
        for (dp=0; dp < 2; dp++) {
            bcm_qos_map_t_init(&l3_eg_map);

            /* Set internal priority  */
            l3_eg_map.int_pri = dscp;
            if (dscp < MAX_PRI/2) {
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
            l3_eg_map.dscp = qos_map_l3_remark_pri[dscp>>2];
    	
            rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_opcode_id);
    	
            if (rv != BCM_E_NONE) {
                 printf("error in L3 egress bcm_qos_map_add()\n");
                 printf("rv is: $rv \n");
                 printf("(%s)\n", bcm_errmsg(rv));
                 return rv;
            }
        }
    }
    /*inheritance to next layer mapping*/
     for (dscp=0; dscp<MAX_PRI; dscp++) {
        for (dp=0; dp < 2; dp++) {
            bcm_qos_map_t_init(&l3_eg_map);

            /*add mapping next layer*/
            /* Set internal priority  */
            l3_eg_map.int_pri = dscp;
            if (dscp < MAX_PRI/2) {
                flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK;
            } else {
                flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK;
            }
            /* Set internal color */
            if (dp == 0){
                l3_eg_map.color = bcmColorGreen;
                l3_eg_map.remark_color = bcmColorGreen;
            } else {
                l3_eg_map.color = bcmColorYellow;
                l3_eg_map.remark_color = bcmColorYellow;
            }
    	
			l3_eg_map.remark_int_pri = dscp;
    	
            rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_map_id);
    	
            if (rv != BCM_E_NONE) {
              printf("error in L3 egress bcm_qos_map_add()\n");
              printf("rv is: $rv \n");
              printf("(%s)\n", bcm_errmsg(rv));
              return rv;
            }
        }
    }
    return rv;
}

/** 
 *  ing_qos_profile - ingress map id value, in case of -1 the
 *                    default value defined in the global
 *                    variable is used
 *  egr_qos_profile - egress map id value, in case of -1 the
 *                    default value defined in the global
 *                    variable is used
 */
int
qos_map_l3_remark(int unit, int ing_qos_profile, int egr_qos_profile)
{
    int rv = BCM_E_NONE;

    if (ing_qos_profile != -1)
    {
        l3_in_map_id = ing_qos_profile;
    }
    if (egr_qos_profile != -1)
    {
        l3_eg_map_id = egr_qos_profile;
    }
    
    rv = qos_map_l3_egress_remark_profile(unit);

    if (rv) {
	printf("error setting up egress qos profile\n");
	return rv;
    }

    rv = qos_map_l3_ingress_remark_profile(unit);
    if (rv) {
	printf("error setting up ingress qos profile\n");
	return rv;
    }

    return rv;
}

int
qos_map_phb_remark_flags_resolve(int unit, uint32 qos_map_flags, uint32 *flags, int *nof_qos)
{
    if (qos_map_flags & BCM_QOS_MAP_IPV6) {
        *flags |= BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6;
        *nof_qos = 256;
    } else if (qos_map_flags & BCM_QOS_MAP_IPV4) {
        *flags |= BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4;
        *nof_qos = 64;
    } else if (qos_map_flags & BCM_QOS_MAP_MPLS) {
        *flags |= BCM_QOS_MAP_MPLS;
        *nof_qos = 8;
    } else if (qos_map_flags & BCM_QOS_MAP_L2_OUTER_TAG) {
        *flags |= BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG;
        *nof_qos = 16;
    } else if (qos_map_flags & BCM_QOS_MAP_L2_INNER_TAG) {
        *flags |= BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_INNER_TAG;
        *nof_qos = 16;
    } else if (qos_map_flags & BCM_QOS_MAP_L2_UNTAGGED) {
        *flags |= BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_UNTAGGED;
        *nof_qos = 16;
    } else if (qos_map_flags & BCM_QOS_MAP_L2_VLAN_PCP) {
        *flags |= BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP;
        *nof_qos = 16;
    } else {
        printf("error config in preparing qos map flags (0x%08X)!\n", qos_map_flags);
        return BCM_E_CONFIG;
    }

    return BCM_E_NONE;
}

int
qos_map_ingress_phb_remark_map_entry_resolve(int unit, uint32 flags, int qos_value, int nof_qos, bcm_qos_map_t *qos_map)
{
    if (flags & BCM_QOS_MAP_IPV6) {
         /** Map-key: ingress dscp Priority, used for L3 remark */
        qos_map->dscp = qos_value;
    } else if (flags & BCM_QOS_MAP_IPV4) {
        /** Map-key: ingress dscp Priority, used for L3 remark */
        qos_map->dscp = qos_value << 2;
    } else if (flags & BCM_QOS_MAP_L2) {
        /** Map-key: ingress pkt Priority and cfi, used for L2 PHP */
        qos_map->pkt_pri = qos_value >> 1;
        qos_map->pkt_cfi = qos_value % 2;
    } else if (flags & BCM_QOS_MAP_MPLS) {
        qos_map->exp = qos_value % 8;
    } else {
        printf("error config found in resolving ingress qos map entry (0x%08X)!\n", flags);
        return BCM_E_CONFIG;
    }

    /** map-result: TC */
    qos_map->int_pri = ((qos_value + 1) & (nof_qos - 1)) % 8;

    /** map-result: DP */
    qos_map->color = ((qos_value + 1) & (nof_qos - 1)) % 2;

    /** map-result: Network_qos*/
    qos_map->remark_int_pri = ((qos_value + 1) & (nof_qos - 1));

    return BCM_E_NONE;
}

int
qos_map_egress_phb_remark_map_entry_resolve(int unit, uint32 flags, int qos_value, int nof_qos, bcm_qos_map_t *qos_map)
{
    /* 
     * For JR2, map-key: int_pri & color, opcode
     *          map-result: dscp(L3) or pkt_pri/cfi(L2) or exp(MPLS)
     *                      and remark_int_pri/remark_color for inhertance
     * For JR+, map-key: int_pri & color, cos-profile
     *          map-result: remark_int_pri
     */

    /** map-key: int_pri & color */
    qos_map->int_pri = qos_value;
    qos_map->color = qos_value % 2;

    /** map-result: current network-qos */
    if (flags & BCM_QOS_MAP_IPV6) {
        qos_map->dscp = ((qos_value - 2) & (nof_qos - 1));
        /* for JR+*/
        qos_map->remark_int_pri = qos_map->dscp;
    } else if (flags & BCM_QOS_MAP_IPV4) {
        qos_map->dscp = ((qos_value - 2) & (nof_qos - 1)) << 2;
        /* for JR+*/
        qos_map->remark_int_pri = qos_map->dscp;
    } else if (flags & BCM_QOS_MAP_L2) {
        qos_map->pkt_pri = ((qos_value - 2) >> 1) & 0x7;
        qos_map->pkt_cfi = (qos_value - 2) & 0x1;
    } else if (flags & BCM_QOS_MAP_MPLS) {
        qos_map->exp = (qos_value - 2) & 0x7;
        /* for JR+*/
        qos_map->remark_int_pri = qos_map->exp;
    } else {
        printf("error config found in resolving egress qos maps (0x%08X)!\n", flags);
        return BCM_E_CONFIG;
    }

    if (is_device_or_above(unit,JERICHO2)) {
        /** map-result: next-layer network-qos */
        qos_map->remark_int_pri = qos_value;
        qos_map->remark_color = qos_value % 2;
    }

    return BCM_E_NONE;
}


/*
 * phb_flags - Qos map flags for PHB
 * remark_flags - Qos map flags for REMARK
 */
int
qos_map_ingress_phb_remark_set(int unit, uint32 phb_flags, uint32 remark_flags)
{
    bcm_qos_map_t l3_in_map;
    int dscp;
    int rv = BCM_E_NONE;
    uint32 flags, remark_map_flags, phb_map_flags;
    int qos, nof_qos_phb , nof_qos_remark;
    int opcode_id = l3_in_opcode_id;

    /** Clear structure */
    bcm_qos_map_t_init(&l3_in_map);

    /** 1. Create qos-profile */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_WITH_ID;
    if (is_device_or_above(unit,JERICHO2)) {
        flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID;
    }

    rv = bcm_qos_map_create(unit, flags, &l3_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_create() for ingress cos-profile\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    printf("bcm_qos_map_create ingress cos-profile: %d\n", l3_in_map_id);

    flags = 0;

    rv = qos_map_phb_remark_flags_resolve(unit, remark_flags, &remark_map_flags, &nof_qos_remark);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in resolving remark map flags (0x%X) for ingress\n", rv, remark_flags);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    if (is_device_or_above(unit,JERICHO2)) {
        /** Create opcode for PHB */
        opcode_id = l3_in_opcode_id;
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID, &opcode_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_create() for ingress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        printf("bcm_qos_map_create ingress PHB opcode: %d\n", l3_in_opcode_id);

        rv = qos_map_phb_remark_flags_resolve(unit, phb_flags, &phb_map_flags, &nof_qos_phb);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in resolving phb map flags (0x%X) for ingress\n", rv, phb_flags);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /** Add the maps for PHB */
        l3_in_map.opcode = opcode_id;
        phb_map_flags |= BCM_QOS_MAP_OPCODE;
        phb_map_flags |= BCM_QOS_MAP_PHB;
        rv = bcm_qos_map_add(unit, phb_map_flags, &l3_in_map, l3_in_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for ingress opcode for PHB\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /** Add maps from {packet.qos & opcode} to {tc,dp} for JR2 only*/
        phb_map_flags &= (~BCM_QOS_MAP_OPCODE);
        for (qos = 0; qos < nof_qos_phb; qos++) {
            bcm_qos_map_t_init(&l3_in_map);
            rv = qos_map_ingress_phb_remark_map_entry_resolve(unit, phb_map_flags, qos, nof_qos_phb, &l3_in_map);
            if (rv != BCM_E_NONE) {
                printf("error (%d) in qos_map_ingress_phb_remark_map_entry_resolve for PHB\n", rv);
                printf("(%s)\n", bcm_errmsg(rv));
                return rv;
            }

            rv = bcm_qos_map_add(unit, phb_map_flags, &l3_in_map, opcode_id);
            if (rv != BCM_E_NONE) {
                printf("error (%d) in bcm_qos_map_add() for ingress qos-map for PHB\n", rv);
                printf("(%s)\n", bcm_errmsg(rv));
                return rv;
            }
        }

        /** Create opcode for remark*/
        opcode_id = l3_in_opcode_id;
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID, &opcode_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_create() for ingress remark opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    
        printf("bcm_qos_map_create ingress REMARK opcode: %d\n", l3_in_opcode_id);
    
        l3_in_map.opcode = opcode_id;
    
        /*
         * 3. Map cos-profile and cos-type to opcode.
         * For dscp remarking for example, the map_flags is something like:
         * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE
         */
        /** Add the maps for remarking */
        remark_map_flags |= BCM_QOS_MAP_OPCODE;
        remark_map_flags |= BCM_QOS_MAP_REMARK;
        rv = bcm_qos_map_add(unit, remark_map_flags, &l3_in_map, l3_in_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for ingress opcode for remarking\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /*
     *  Map packet.qos and opcode to network_qos and tc/dp. 
     * For dscp remarking for example,the map_flags is something like (For JR2):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK
     * the map_flags is something like (For JR):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4
     */
    for (qos = 0; qos < nof_qos_remark; qos++) {
        bcm_qos_map_t_init(&l3_in_map);
        rv = qos_map_ingress_phb_remark_map_entry_resolve(unit, remark_map_flags, qos, nof_qos_remark, &l3_in_map);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in qos_map_ingress_phb_remark_map_entry_resolve for REMARK\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        if (!is_device_or_above(unit,JERICHO2) && (remark_flags & BCM_QOS_MAP_L3_L2)) {
            remark_map_flags |= BCM_QOS_MAP_L3_L2;
            /*
             * In JR+, if map pcpdei to dscp,l3_in_map.dscp carry pcpdei for mapping to in-dscp.
             * Also it is l3_in_map.dscp is used to map to tc/dp (not so reasonable).
             */
            if (remark_flags & (BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_MPLS)) {
                l3_in_map.dscp = qos;
                l3_in_map.color = 0;
            }
        }

        /** Add maps from {packet.qos & opcode} to {network_qos (and tc/dp for JR+)}*/
        remark_map_flags &= (is_device_or_above(unit,JERICHO2) ? (~BCM_QOS_MAP_OPCODE) : 0xFFFFFFFF);
        if (is_device_or_above(unit,JERICHO2)) {
             rv = bcm_qos_map_add(unit, remark_map_flags, &l3_in_map, opcode_id);
        } else {
            rv = bcm_qos_map_add(unit, remark_map_flags, &l3_in_map, l3_in_map_id);
        }
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for ingress qos-map\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}


/*
 * remark_flags - Qos map flags for egress remark qos map add.
 * fwd_hrd_flags - forward head flags for egress QoS opcode.
 *                 Used for inidicating mark L3/MPLS/ETH
 */
int
qos_map_egress_phb_remark_set(int unit, uint32 remark_flags, uint32 fwd_hrd_flags)
{
    bcm_qos_map_t l3_eg_map;
    int flags = 0;
    int qos;
    int nof_qos = 64;
    int dp;
    int rv;
    uint32 remark_map_flags, create_flags, opcode_app_flags;
 
    /** Clear structure */
    bcm_qos_map_t_init(&l3_eg_map);

    /** 1. Create qos-profile */
    create_flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_WITH_ID;
    if (is_device_or_above(unit,JERICHO2)) {
        create_flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID;
    }
    rv = bcm_qos_map_create(unit, create_flags, &l3_eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_create() for egress cos-profile\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
    } else {
        map_ids[map_id_cnt++] = l3_eg_map_id;
    }

    printf("bcm_qos_map_create egress cos-profile: %d\n", l3_eg_map_id);

    /** Prepare map flags*/
    remark_map_flags = 0;
    rv = qos_map_phb_remark_flags_resolve(unit, remark_flags, &remark_map_flags, &nof_qos);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in resolving remark_flags (0x%X) for egress\n", rv, remark_flags);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    if (qos_l3_map_to_l2 && (remark_map_flags & BCM_QOS_MAP_L2)) {
        nof_qos = 64;
    }
    if (is_device_or_above(unit,JERICHO2)) {
        /** 2. Create opcode */
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID, &l3_eg_opcode_id);
        if (rv != BCM_E_NONE) {
            printf("error  (%d) in bcm_qos_map_create() for egress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        printf("bcm_qos_map_create egress opcode: %d\n", l3_eg_opcode_id);

        l3_eg_map.opcode = l3_eg_opcode_id;

        int tmp;
        opcode_app_flags = 0;
        rv = qos_map_phb_remark_flags_resolve(unit, fwd_hrd_flags, &opcode_app_flags, &tmp);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in resolving fwd_hrd_flags (0x%X) for ingress\n", rv, fwd_hrd_flags);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        opcode_app_flags |= BCM_QOS_MAP_REMARK;
        opcode_app_flags |= BCM_QOS_MAP_OPCODE;

        /*
         * 3. Map cos-profile and cos-type to opcode.
         * For dscp remarking for example, the map_flags is something like:
         * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE
         */
        rv = bcm_qos_map_add(unit, opcode_app_flags, &l3_eg_map, l3_eg_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for ingress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /*
     * 4. Map packet.qos and opcode to network_qos.
     * For dscp remarking for example, the map_flags is something like (For JR2):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK
     * the map_flags is something like (For JR):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4
     */
    for (qos = 0; qos < nof_qos; qos++) {

        if (is_device_or_above(unit,JERICHO2)) {
            remark_map_flags |= BCM_QOS_MAP_REMARK;
        }

        /* 
         * For JR2, map-key: int_pri & color, opcode
         *          map-result: dscp(L3) or pkt_pri/cfi(L2) or exp(MPLS)
         *                      and remark_int_pri/remark_color for inhertance
         * For JR+, map-key: int_pri & color, cos-profile
         *          map-result: remark_int_pri
         */
        rv = qos_map_egress_phb_remark_map_entry_resolve(unit, remark_map_flags, qos, nof_qos, &l3_eg_map);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in qos_map_egress_phb_remark_map_entry_resolve\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        if (is_device_or_above(unit,JERICHO2)) {
             rv = bcm_qos_map_add(unit, remark_map_flags, &l3_eg_map, l3_eg_opcode_id);
        } else {
             rv = bcm_qos_map_add(unit, remark_map_flags, &l3_eg_map, l3_eg_map_id);
        }
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for egress qos-map (%d, %d)\n", rv, qos, nof_qos);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}


/**
 *  ing_phb_flags - flags for ingress qos PHB map. The possible flags are:
 *                  BCM_QOS_MAP_IPV4, BCM_QOS_MAP_IPV6, BCM_QOS_MAP_MPLS
 *                  BCM_QOS_MAP_L2_OUTER_TAG, BCM_QOS_MAP_L2_INNER_TAG 
 *                  BCM_QOS_MAP_L2_UNTAGGED, BCM_QOS_MAP_L2_VLAN_PCP
 *  ing_remark_flags - flags for ingress qos REMARK map.
 *                     The possible flags are the same ing_phb_flags.
 *  egr_remark_flags - flags for egress qos REMARK map. 
 *                     The possible flags are the same ing_phb_flags.
 *  ing_gport - gport for ingress LIF for which to set the cos-profile
 *              No action for it if it is invalid.
 *  ing_gport - gport for egress LIF for which to set the cos-profile
 *              No action for it if it is invalid.
 *  ing_qos_profile - ingress map id value, in case of -1 the
 *                    default value defined in the global
 *                    variable is used
 *  egr_qos_profile - egress map id value, in case of -1 the
 *                    default value defined in the global
 *                    variable is used
 *
 *  This procedure create the cos-profile and add qos mapping.
 *  At ingress, it resusts as pph.nwk-qos/in-dscp-exp = packet.qos + 1.
 *  At egress, it resusts as packet.qos = pph.nwk-qos/in-dscp-exp - 2.
 *  So, finally out-packet.qos = in-packet.qos - 1.
 */
int
qos_map_phb_remark_set(int unit,
                       uint32 ing_phb_flags,
                       uint32 ing_remark_flags,
                       uint32 egr_remark_flags,
                       int ing_gport,
                       int egr_gport,
                       int ing_qos_profile,
                       int egr_qos_profile)
{
    int rv = BCM_E_NONE;
    int in_qos_profile, out_qos_profile;

    if (ing_qos_profile != -1)
    {
        l3_in_map_id = ing_qos_profile;
    }
    if (egr_qos_profile != -1)
    {
        l3_eg_map_id = egr_qos_profile;
    }

    rv = qos_map_egress_phb_remark_set(unit, egr_remark_flags, egr_remark_flags);
    if (rv) {
        printf("error (%d) setting up egress qos profile\n", rv);
        return rv;
    }

    if (egr_gport != BCM_GPORT_INVALID)
    {
        out_qos_profile = ((0x2000 | 0x1 | 0x2) << 16) | l3_eg_map_id;
        rv = bcm_qos_port_map_set(unit, egr_gport, -1, out_qos_profile);
        if (rv) {
            printf("error (%d) setting ingress qos profile in OutLif\n", rv);
            return rv;
        }
    }

    rv = qos_map_ingress_phb_remark_set(unit, ing_phb_flags, ing_remark_flags);
    if (rv) {
        printf("error (%d) setting up ingress qos profile\n", rv);
        return rv;
    }

    if (ing_gport != BCM_GPORT_INVALID)
    {
        in_qos_profile = ((0x1000 | 0x1 | 0x2) << 16) | l3_in_map_id;
        rv = bcm_qos_port_map_set(unit, ing_gport, in_qos_profile, -1);
        if (rv) {
            printf("error (%d) setting ingress qos profile in InLif\n", rv);
            return rv;
        }
    }

    return rv;
}

int
qos_map_ingress_phb_remark_clear(int unit, uint32 phb_flags, uint32 remark_flags)
{
    bcm_qos_map_t l3_in_map;
    int rv = BCM_E_NONE;
    uint32 flags, remark_map_flags, phb_map_flags;
    int qos, nof_qos_phb, nof_qos_remark;
    int ing_qos_map_id = ((0x1000 | 0x1 | 0x2) << 16) | l3_in_map_id;
    int ing_qos_phb_opcode = ((0x1000 | 0x8 | 0x1) << 16) | l3_in_opcode_id;
    int ing_qos_remark_opcode = ((0x1000 | 0x8 | 0x2) << 16) | l3_in_opcode_id;

    if (!is_device_or_above(unit, JERICHO2))
    {
        ing_qos_map_id = ((0x1000 | 0x2) << 16) | l3_in_map_id;
    }

    /** Clear structure */
    bcm_qos_map_t_init(&l3_in_map);

    rv = qos_map_phb_remark_flags_resolve(unit, remark_flags, &remark_map_flags, &nof_qos_remark);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in resolving remark map flags (0x%X) for ingress\n", rv, remark_flags);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /*
     * 1. Delete the Maps from cos-profile and cos-type to opcode. 
     * For dscp remarking for example, the map_flags is something like:
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE
     */
    if (is_device_or_above(unit,JERICHO2)) {

        l3_in_map.opcode = ing_qos_remark_opcode;

        /** Delete the map for remark*/
        remark_map_flags |= BCM_QOS_MAP_OPCODE;
        remark_map_flags |= BCM_QOS_MAP_REMARK;
        rv = bcm_qos_map_delete(unit, remark_map_flags, &l3_in_map, ing_qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_delete() for ingress opcode for remark\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /** Delete the map for phb*/
        rv = qos_map_phb_remark_flags_resolve(unit, phb_flags, &phb_map_flags, &nof_qos_phb);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in resolving phb map flags (0x%X) for ingress\n", rv, phb_flags);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        l3_in_map.opcode = ing_qos_phb_opcode;
        phb_map_flags |= BCM_QOS_MAP_OPCODE;
        phb_map_flags |= BCM_QOS_MAP_PHB;
        rv = bcm_qos_map_delete(unit, phb_map_flags, &l3_in_map, ing_qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_delete() for ingress opcode for phb\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /** 2.1 Delete the maps from {packet.qos & opcode} to {tc,dp} for JR2*/
        phb_map_flags &= (~BCM_QOS_MAP_OPCODE);
        for (qos = 0; qos < nof_qos_phb; qos++) {
             bcm_qos_map_t_init(&l3_in_map);
            rv = qos_map_ingress_phb_remark_map_entry_resolve(unit, phb_map_flags, qos, nof_qos_phb, &l3_in_map);
            if (rv != BCM_E_NONE) {
                printf("error (%d) in qos_map_ingress_phb_remark_map_entry_resolve\n", rv);
                printf("(%s)\n", bcm_errmsg(rv));
                return rv;
            }

            rv = bcm_qos_map_delete(unit, phb_map_flags, &l3_in_map, ing_qos_phb_opcode);
            if (rv != BCM_E_NONE) {
                printf("error (%d) in bcm_qos_map_add() for ingress qos-map for PHB\n", rv);
                printf("(%s)\n", bcm_errmsg(rv));
                return rv;
            }
        }
    }

    /*
     * 2.2. Delete the Maps from packet.qos and opcode to network_qos/tc&dp.
     * for dscp remarking for example, the map_flags is something like (For JR2):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK
     * the map_flags is something like (For JR):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4
     */
    for (qos = 0; qos < nof_qos_remark; qos++) {
        rv = qos_map_ingress_phb_remark_map_entry_resolve(unit, remark_map_flags, qos, nof_qos_remark, &l3_in_map);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in qos_map_ingress_phb_remark_map_entry_resolve\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /** Delete the maps from {packet.qos, opcode} to {nwk_qos (and tc/dp for JR+)}*/
        remark_map_flags &= (is_device_or_above(unit,JERICHO2) ? (~BCM_QOS_MAP_OPCODE) : 0xFFFFFFFF);
         if (is_device_or_above(unit,JERICHO2)) {
              rv = bcm_qos_map_delete(unit, remark_map_flags, &l3_in_map, ing_qos_remark_opcode);
         } else {
              rv = bcm_qos_map_delete(unit, remark_map_flags, &l3_in_map, ing_qos_map_id);
         }
        
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_delete() for ingress qos-map\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /** 3. Destroy the opcode */
    if (is_device_or_above(unit,JERICHO2)) {
        rv = bcm_qos_map_destroy(unit, ing_qos_remark_opcode);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_destroy() for ingress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_qos_map_destroy(unit, ing_qos_phb_opcode);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_destroy() for ingress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        printf("bcm_qos_map_destroy ingress opcode: %d\n", l3_in_opcode_id);
    }

    /** 4. Destroy cos-profile */
    rv = bcm_qos_map_destroy(unit, ing_qos_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_create() for ingress cos-profile\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    printf("bcm_qos_map_destroy ingress cos-profile: %d\n", l3_in_map_id);

    return rv;
}


int
qos_map_egress_phb_remark_clear(int unit, uint32 remark_flags, uint32 fwd_hrd_flags)
{
    bcm_qos_map_t l3_eg_map;
    int flags = 0;
    int qos;
    int nof_qos = 64;
    int dp;
    int rv;
    uint32 remark_map_flags, create_flags, opcode_app_flags;

    int egr_qos_map_id = ((0x2000 | 0x1 | 0x2) << 16) | l3_eg_map_id;
    int egr_qos_opcode = ((0x2000 | 0x8 | 0x2) << 16) | l3_eg_opcode_id;

    if (!is_device_or_above(unit, JERICHO2))
    {
        egr_qos_map_id = ((0x2000 | 0x4) << 16) | l3_eg_map_id;
    }
 
    /** Clear structure */
    bcm_qos_map_t_init(&l3_eg_map);

    /** Prepare map flags*/
    remark_map_flags = 0;
    rv = qos_map_phb_remark_flags_resolve(unit, remark_flags, &remark_map_flags, &nof_qos);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in resolving remark_flags (0x%X) for egress\n", rv, remark_flags);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    if (is_device_or_above(unit,JERICHO2)) {

        l3_eg_map.opcode = egr_qos_opcode;

        int tmp;
        opcode_app_flags = 0;
        rv = qos_map_phb_remark_flags_resolve(unit, fwd_hrd_flags, &opcode_app_flags, &tmp);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in resolving fwd_hrd_flags (0x%X) for egress\n", rv, fwd_hrd_flags);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        opcode_app_flags |= BCM_QOS_MAP_REMARK;
        opcode_app_flags |= BCM_QOS_MAP_OPCODE;

        /*
         * 1. Delete the Maps from cos-profile and cos-type to opcode.
         * For dscp remarking for example, the map_flags is something like:
         * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE
         */
        rv = bcm_qos_map_delete(unit, opcode_app_flags, &l3_eg_map, egr_qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_delete() for egress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /*
     * 2. Delete the Maps from packet.qos and opcode to network_qos.
     * For dscp remarking for example, the map_flags is something like (For JR2):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK
     * the map_flags is something like (For JR):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4
     */
    for (qos = 0; qos < nof_qos; qos++) {
        bcm_qos_map_t_init(&l3_eg_map);
        if (is_device_or_above(unit,JERICHO2)) {
            remark_map_flags |= BCM_QOS_MAP_REMARK;
        }

        /* 
         * For JR2, map-key: int_pri & color, opcode
         *          map-result: dscp(L3) or pkt_pri/cfi(L2) or exp(MPLS)
         *                      and remark_int_pri/remark_color for inhertance
         * For JR+, map-key: int_pri & color, cos-profile
         *          map-result: remark_int_pri
         */
        rv = qos_map_egress_phb_remark_map_entry_resolve(unit, remark_map_flags, qos, nof_qos, &l3_eg_map);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in qos_map_egress_phb_remark_map_entry_resolve\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
        if (is_device_or_above(unit,JERICHO2)) {
            rv = bcm_qos_map_delete(unit, remark_map_flags, &l3_eg_map, egr_qos_opcode);
        } else {
            rv = bcm_qos_map_delete(unit, remark_map_flags, &l3_eg_map, egr_qos_map_id);
        }
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_delete() for egress qos-map\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    if (is_device_or_above(unit,JERICHO2)) {
        /** 3. Destroy opcode */
        rv = bcm_qos_map_destroy(unit, egr_qos_opcode);
        if (rv != BCM_E_NONE) {
            printf("error  (%d) in bcm_qos_map_destroy() for egress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        printf("bcm_qos_map_destroy egress opcode: %d\n", l3_eg_opcode_id);
    }

    /** 4. Destroy cos-profile */
    rv = bcm_qos_map_destroy(unit, egr_qos_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_destroy() for egress cos-profile\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
    }

    printf("bcm_qos_map_destroy egress cos-profile: %d\n", l3_eg_map_id);
    return rv;
}


/**
 *
 *  This procedure clear the cos-profile and add qos mapping.
 *  At ingress, it resusts as pph.nwk-qos/in-dscp-exp = packet.qos + 1.
 *  At egress, it resusts as packet.qos = pph.nwk-qos/in-dscp-exp - 2.
 *  So, finally out-packet.qos = in-packet.qos - 1.
 */
int
qos_map_phb_remark_clear(int unit,
                         uint32 ing_phb_flags,
                         uint32 ing_remark_flags,
                         uint32 egr_remark_flags,
                         int ing_gport,
                         int egr_gport,
                         int ing_qos_profile,
                         int egr_qos_profile)
{
    int rv = BCM_E_NONE;
    int in_qos_profile, out_qos_profile;

    if (ing_qos_profile != -1)
    {
        l3_in_map_id = ing_qos_profile;
    }
    if (egr_qos_profile != -1)
    {
        l3_eg_map_id = egr_qos_profile;
    }

    in_qos_profile = ((0x1000 | 0x1 | 0x2) << 16) | 0;
    out_qos_profile = ((0x2000 | 0x1 | 0x2) << 16) | 0;

    if (egr_gport != BCM_GPORT_INVALID)
    {
        rv = bcm_qos_port_map_set(unit, egr_gport, -1, out_qos_profile);
        if (rv) {
            printf("error (%d) clear ingress qos profile in OutLif\n", rv);
            return rv;
        }
    }

    rv = qos_map_egress_phb_remark_clear(unit, egr_remark_flags, egr_remark_flags);
    if (rv) {
        printf("error (%d) clear up egress qos profile\n", rv);
        return rv;
    }

    if (ing_gport != BCM_GPORT_INVALID)
    {
        rv = bcm_qos_port_map_set(unit, ing_gport, in_qos_profile, -1);
        if (rv) {
            printf("error (%d) clear ingress qos profile in InLif\n", rv);
            return rv;
        }
    }

    rv = qos_map_ingress_phb_remark_clear(unit, ing_phb_flags, ing_remark_flags);
    if (rv) {
        printf("error (%d) clear up ingress qos profile\n", rv);
        return rv;
    }

    return rv;
}

/*
 * For routing packets, map L3 IPV4.TOS/IPV6.TC/MPLS.EXP to NWK_QOS/IN_DSCP_EXP
 * For routing packets, map L3 IPV4.TOS/IPV6.TC/MPLS.EXP to PHB
 * Possible qos_map_flags: BCM_QOS_MAP_IPV4/BCM_QOS_MAP_IPV6/BCM_QOS_MAP_MPLS
 */
int
qos_map_l3_dscp_config(int unit,
                       uint32 qos_map_flags,
                       int ing_gport,
                       int egr_gport,
                       int ing_qos_profile,
                       int egr_qos_profile)
{
    return qos_map_phb_remark_set(unit,
                                  qos_map_flags,qos_map_flags,qos_map_flags,
                                  ing_gport,egr_gport,ing_qos_profile,egr_qos_profile);
}

int
qos_map_l3_dscp_clear(int unit,
                      uint32 qos_map_flags,
                      int ing_gport,
                      int egr_gport,
                      int ing_qos_profile,
                      int egr_qos_profile)
{
    return qos_map_phb_remark_clear(unit,
                                  qos_map_flags,qos_map_flags,qos_map_flags,
                                  ing_gport,egr_gport,ing_qos_profile,egr_qos_profile);
}

/*
 * For routing packets, map L2 ETH.PCPDEI to NWK_QOS/IN_DSCP_EXP
 * For routing packets, map L3 IPV4.TOS/IPV6.TC/MPLS.EXP to PHB
 * Possible qos_map_flags: BCM_QOS_MAP_IPV4/BCM_QOS_MAP_IPV6/BCM_QOS_MAP_MPLS
 */
int
qos_map_l3_pcpdei_config(int unit,
                         uint32 qos_map_flags,
                         int ing_gport,
                         int egr_gport,
                         int ing_qos_profile,
                         int egr_qos_profile)
{
    uint32 ing_phb_flags, ing_remark_flags, egr_remark_flags;

    if (is_device_or_above(unit,JERICHO2)) {
        ing_phb_flags = BCM_QOS_MAP_L2_OUTER_TAG;
        ing_remark_flags = BCM_QOS_MAP_L2_OUTER_TAG;
        egr_remark_flags = qos_map_flags;
    } else {
        ing_phb_flags = 0;
        ing_remark_flags = qos_map_flags | BCM_QOS_MAP_L3_L2;
        egr_remark_flags = qos_map_flags;
    }
    return qos_map_phb_remark_set(unit,
                                  ing_phb_flags,ing_remark_flags,egr_remark_flags,
                                  ing_gport,egr_gport,ing_qos_profile,egr_qos_profile);
}

int
qos_map_l3_pcpdei_clear(int unit,
                        int32 qos_map_flags,
                        int ing_gport,
                        int egr_gport,
                        int ing_qos_profile,
                        int egr_qos_profile)
{
    uint32 ing_phb_flags, ing_remark_flags, egr_remark_flags;

    if (is_device_or_above(unit,JERICHO2)) {
        ing_phb_flags = BCM_QOS_MAP_L2_OUTER_TAG;
        ing_remark_flags = BCM_QOS_MAP_L2_OUTER_TAG;
        egr_remark_flags = qos_map_flags;
    } else {
        ing_phb_flags = 0;
        ing_remark_flags = qos_map_flags | BCM_QOS_MAP_L3_L2;
        egr_remark_flags = qos_map_flags;
    }

    return qos_map_phb_remark_clear(unit,
                                  ing_phb_flags,ing_remark_flags,egr_remark_flags,
                                  ing_gport,egr_gport,ing_qos_profile,egr_qos_profile);
}




