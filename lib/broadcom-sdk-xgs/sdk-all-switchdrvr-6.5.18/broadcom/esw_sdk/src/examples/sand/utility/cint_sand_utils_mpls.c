/*
 * $Id: cint_sand_utils_mpls.c, Exp $
 $Copyright: (c) 2019 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$ File: cint_sand_utils_mpls.c Purpose: utility for VPLS.
 */


/*******************************************************************************
 *
 * Example translations for the usages of MPLS APIs from DPP(JR+) to DNX(JR2) begin
 * Add your example below between the frame if you have.
 * {
 *
 ******************************************************************************/
/*
 * { mpls_port_add, backward compatible example -- begin
 *
 * This is an Example for porting JR+ bcm_mpls_port_add to JR2.
 * It can be used to replace the right BCM API in JR+ cints/applications
 * and works for both DPP(JR+) and DNX(JR2) on ingress and egress MPLS port instance.
 *
 * Remark:
 * In DPP, bcm_mpls_port_add can create mpls ingress, egress and FEC objects.
 * While in DNX, FEC object should be created with bcm_l3_egress_create sperately.
 * So, here FEC objects won't be returned by mpls_port_id.
 *
 * If both ingress and egress are created in the same call(No _INGRESS_ONLY and
 * _EGRESS_ONLY flags were set), then _WITH_ID should be set for ingress objects.
 *
 */
 

int
sand_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_port_t * mpls_port)
{
    int rv;
    /*
     * In DPP devices, bcm_mpls_port_add can add termination, encapsulation and FEC HW in a single call.
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        rv = bcm_mpls_port_add(unit, vpn, mpls_port);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_mpls_port_add failed: %d \n", rv);
            return rv;
        }

        return rv;
    }

    /*
     * In DNX devices, the same configurations must be achived by:
     * 1) Create egress encap object with BCM_MPLS_PORT_EGRESS_ONLY
     * 2) Create FEC object
     * 3) Create Ingress termination object with BCM_MPLS_PORT_INGRESS_ONLY
     */

    int is_ingress, is_egress;
    bcm_mpls_port_t mpls_port_ing, mpls_port_egr;
    int is_forward_group, is_1to1_protect;
    bcm_if_t if_fec_id;
    uint32 flags, flags2;
    uint8 is_jr2_mode = soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1);

    flags = 0;
    /** Available flags in JR2:*/
    flags |= (mpls_port->flags & BCM_MPLS_PORT_WITH_ID) ? BCM_MPLS_PORT_WITH_ID : 0;
    flags |= (mpls_port->flags & BCM_MPLS_PORT_CONTROL_WORD) ? BCM_MPLS_PORT_CONTROL_WORD : 0;
    flags |= (mpls_port->flags & BCM_MPLS_PORT_NETWORK) ? BCM_MPLS_PORT_NETWORK : 0;
    flags |= (mpls_port->flags & BCM_MPLS_PORT_REPLACE) ? BCM_MPLS_PORT_REPLACE : 0;
    flags |= (mpls_port->flags & BCM_MPLS_PORT_ENTROPY_ENABLE) ? BCM_MPLS_PORT_ENTROPY_ENABLE : 0;
    flags |= (mpls_port->flags & BCM_MPLS_PORT_EGRESS_TUNNEL) ? BCM_MPLS_PORT_EGRESS_TUNNEL : 0;
    flags |= (mpls_port->flags & BCM_MPLS_PORT_ENCAP_WITH_ID) ? BCM_MPLS_PORT_ENCAP_WITH_ID : 0;

    is_forward_group = (mpls_port->flags & BCM_MPLS_PORT_FORWARD_GROUP) ? 1 : 0;
    is_1to1_protect = (mpls_port->failover_id > 0) && (mpls_port->failover_id >> 29 == 2) && !(mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY);

    is_ingress = (mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = (mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY) ? FALSE : TRUE;

    if (is_forward_group)
    {
        is_ingress = is_egress = FALSE;
    }

    /** 1) Encapsulation using bcm_mpls_port_add with BCM_MPLS_PORT_EGRESS_ONLY flag.*/
    if (is_egress)
    {
        bcm_mpls_port_t_init(&mpls_port_egr);

        /** Available flags in JR2:*/
        mpls_port_egr.flags |= flags;

        mpls_port_egr.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;

        mpls_port_egr.criteria = BCM_MPLS_PORT_MATCH_INVALID;
        mpls_port_egr.port = BCM_GPORT_INVALID;
        mpls_port_egr.match_label = 0;

        /** Outlif encap params. */
        sal_memcpy(&(mpls_port_egr.egress_label), &(mpls_port->egress_label), sizeof(mpls_port->egress_label));
        sal_memcpy(&(mpls_port_egr.egress_tunnel_label), &(mpls_port->egress_tunnel_label), sizeof(mpls_port->egress_tunnel_label));

        mpls_port_egr.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        mpls_port_egr.encap_id = mpls_port->encap_id;
        if (BCM_L3_ITF_TYPE_IS_LIF(mpls_port->egress_tunnel_if))
        {
            mpls_port_egr.egress_tunnel_if = mpls_port->egress_tunnel_if;
        }
        else if (BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if))
        {
            bcm_l3_egress_t egr;
            bcm_l3_egress_t_init(&egr);

            /** get the tunnel encap from FEC.*/
            rv = bcm_l3_egress_get(unit, mpls_port->egress_tunnel_if, &egr);
            if (rv != BCM_E_NONE)
            {
                printf("bcm_l3_egress_get failed for getting tunnel encap id from FEC: %d \n", rv);
                return rv;
            }

            mpls_port_egr.egress_tunnel_if = egr.intf;
        }
        mpls_port_egr.mpls_port_id = mpls_port->mpls_port_id;
        mpls_port_egr.network_group_id = mpls_port->network_group_id;

        /** Outlif 1:1 protection params. */
        mpls_port_egr.egress_failover_id = mpls_port->egress_failover_id;
        mpls_port_egr.egress_failover_port_id = mpls_port->egress_failover_port_id;

        rv = bcm_mpls_port_add(unit, 0, &mpls_port_egr);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_mpls_port_add failed for egress: %d \n", rv);
            return rv;
        }

        /** Save the result in mpls-port*/
        mpls_port->encap_id = mpls_port_egr.encap_id;
        mpls_port->mpls_port_id = mpls_port_egr.mpls_port_id;

        if(verbose >= 2) {
            printf("mpls port add egress objects complete, mpls_port_id = 0x%08X, encap_id = 0x%08X\n", mpls_port->mpls_port_id, mpls_port->encap_id);
        }
    }

    /** 2) FEC using bcm_l3_egress_create.*/
    if (is_forward_group || is_1to1_protect)
    {
        uint32 l3eg_flags;
        bcm_l3_egress_t l3eg;
        bcm_if_t l3egid;

        bcm_l3_egress_t_init(l3eg);

        l3eg.port = mpls_port->port;
        l3eg.encap_id = mpls_port->encap_id;

        /** FEC 1:1 params */
        l3eg.failover_id = mpls_port->failover_id;
        l3eg.failover_if_id = mpls_port->egress_failover_port_id;
        l3eg.failover_mc_group = mpls_port->failover_mc_group;

        if(mpls_port.flags & BCM_MPLS_PORT_EGRESS_TUNNEL)
        {
            l3eg.intf = mpls_port->egress_tunnel_if;
        }

        /** For mpls_port -> flags */
        if (mpls_port->flags & BCM_MPLS_PORT_WITH_ID)
        {
            if (BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(mpls_port->mpls_port_id))
            {
                l3eg_flags |= BCM_L3_WITH_ID;
                BCM_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(if_fec_id, mpls_port->mpls_port_id);
            }
            else if(is_1to1_protect)
            {
                l3eg_flags |= BCM_L3_WITH_ID;
                if_fec_id = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id);
                BCM_L3_ITF_SET(*if_fec_id, BCM_L3_ITF_TYPE_FEC, if_fec_id);
            }

        }
        if (mpls_port->flags & BCM_MPLS_PORT_REPLACE)
        {
            l3eg_flags |= BCM_L3_REPLACE;
        }

        /** For mpls_port -> flags2*/
        if(mpls_port->flags2 & BCM_MPLS_PORT2_CASCADED)
        {
            l3eg.flags |= BCM_L3_2ND_HIERARCHY;
        }

        l3eg_flags = BCM_L3_INGRESS_ONLY;
        rv = bcm_l3_egress_create(unit, l3eg_flags, &l3eg, &if_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_l3_egress_create FEC failed: %d \n", rv);
            return rv;
        }

        printf("Example FEC objects created by bcm_l3_egress_create for mpls port add, fec-id = 0x%08X \n", if_fec_id);
        printf("Attention!! FEC objects should be created by bcm_l3_egress_create separately. It can't be resturned by mpls_port_id in JR2.\n");
    }

    /** 3) Termination using bcm_mpls_port_add with BCM_MPLS_PORT_INGRESS_ONLY flag. */
    if (is_ingress)
    {
        bcm_mpls_port_t_init(&mpls_port_ing);

        /** Available flags in JR2:*/
        mpls_port_ing.flags |= flags;

        /** Available flags2 in JR2:*/
        mpls_port_ing.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY | (vpn ? 0 : BCM_MPLS_PORT2_CROSS_CONNECT);

        if (BCM_GPORT_IS_MPLS_PORT(mpls_port->mpls_port_id))
        {
            mpls_port_ing.flags |= BCM_MPLS_PORT_WITH_ID;
        }

        mpls_port_ing.port = mpls_port->port;
        mpls_port_ing.criteria = mpls_port->criteria;
        mpls_port_ing.match_label = mpls_port->match_label;
        mpls_port_ing.mpls_port_id = mpls_port->mpls_port_id;
        
        mpls_port_ing.failover_id = mpls_port->failover_id;
        mpls_port_ing.failover_port_id = mpls_port->failover_port_id;
        mpls_port_ing.failover_mc_group = mpls_port->failover_mc_group;
        mpls_port_ing.ingress_failover_id = mpls_port->ingress_failover_id;
        mpls_port_ing.ingress_failover_port_id = mpls_port->ingress_failover_port_id;
      
        mpls_port_ing.network_group_id = mpls_port->network_group_id;
        mpls_port_ing.vccv_type = mpls_port->vccv_type;
        sal_memcpy(&(mpls_port_ing.ingress_qos_model), &(mpls_port->ingress_qos_model), sizeof(mpls_port->ingress_qos_model));

        /*
         * If egress_tunnel_if is valid for ingress, it must be FEC for learning.
         * But if 1+1 protection, egress_tunnel_if can't be FEC since learning info will be bicast mc-group.
         */
        mpls_port_ing.egress_tunnel_if = BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if) && (mpls_port->failover_mc_group == 0) ? mpls_port->egress_tunnel_if : 0;
        mpls_port_ing.encap_id = mpls_port->failover_mc_group? 0 : (BCM_L3_ITF_TYPE_IS_FEC(mpls_port_ing.egress_tunnel_if) ? 0 : mpls_port->encap_id);
        if (!is_jr2_mode && BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if) && (mpls_port->failover_mc_group == 0)) {
            sal_memcpy(&(mpls_port_ing.egress_label), &(mpls_port->egress_label), sizeof(mpls_port->egress_label));
        }

        rv = bcm_mpls_port_add(unit, vpn, &mpls_port_ing);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_mpls_port_add failed for ingress: %d \n", rv);
            return rv;
        }

        mpls_port->mpls_port_id = mpls_port_ing.mpls_port_id;
        mpls_port->encap_id = mpls_port_ing.encap_id;

        if(verbose >= 2) {
            printf("mpls port add ingress objects complete, mpls_port_id = 0x%08X, encap_id = 0x%08X, egress_tunnel_if = 0x%08X\n", mpls_port_ing.mpls_port_id, mpls_port_ing.encap_id, mpls_port_ing.egress_tunnel_if);
        }
    }

    return rv;
}
/*
 * } mpls port add -- end
 */

/*
 * { mpls tunnel initiator create -- begin
 *
 * This is an Example for porting JR+ bcm_mpls_tunnel_initiator_create to JR2.
 *
 * Remark:
 * Refer to the backward compatibility document for more informations about
 * the API changes.
 */
int
sand_mpls_tunnel_initiator_create(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    int rv;
    /*
     * In DPP devices, bcm_mpls_tunnel_initiator_create can add 4 label entries
     * with different qos inheritance mode and label actions.
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        rv = bcm_mpls_tunnel_initiator_create(unit, intf, num_labels, label_array);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_mpls_tunnel_initiator_create failed: %d \n", rv);
            return rv;
        }

        return rv;
    }

    /*
     * In DNX devices, bcm_mpls_tunnel_initiator_create can add 2 label entries
     * with the same qos inheritance mode.
     * PUSH and SWAP action is per forward-code, rather than tunnel attributes.
     * No changes to PHP action.
     */
    int label_index, skip_count, nof_labels;
    int tunnel_id, access_id;
    bcm_mpls_egress_label_t outer_label_array[2];
    uint32 couple_flags, qos_model_diff =0, ttl_model_diff=0;
    uint32 exp_diff = 0,ttl_diff = 0,qos_id_diff = 0, failover_id_diff = 0,failover_lif_diff = 0;
    bcm_if_t l3_if_id;
    uint8 is_tandem = 1;
    bcm_encap_access_t access_phase[7] = {bcmEncapAccessNativeArp,
                                          bcmEncapAccessTunnel1, bcmEncapAccessTunnel2,
                                          bcmEncapAccessTunnel3, bcmEncapAccessTunnel4,
                                          bcmEncapAccessArp, bcmEncapAccessInvalid};

    for (label_index = 0; label_index < num_labels; label_index++)
    {
        /** Only PHP is supported, other action is resolved by hard-logic.*/
        if(label_array[label_index].action != BCM_MPLS_EGRESS_ACTION_PHP)
        {
            label_array[label_index].action = BCM_MPLS_EGRESS_ACTION_SWAP;
            label_array[label_index].flags &= ~BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
        }

        /** This flag must be set in JR2.*/
        label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

        if (label_array[label_index].flags & BCM_MPLS_EGRESS_LABEL_PRESERVE)
        {
            printf("In JR2, up to 12 labels can be encapsolated in PP, LABEL_PRESERVE solution is not needed!\n");
            return BCM_E_PARAM;
        }

        if (label_array[label_index].flags & (BCM_MPLS_EGRESS_LABEL_IML | BCM_MPLS_EGRESS_LABEL_EVPN))
        {
            printf("Refer to the EVPN UM for solution to EVPN in JR2!\n");
            return BCM_E_PARAM;
        }

        /** Remove the unused flags*/
        label_array[label_index].flags &= ~BCM_MPLS_EGRESS_LABEL_ECN_TO_EXP_MAP;
        label_array[label_index].flags &= ~BCM_MPLS_EGRESS_LABEL_INT_CN_TO_EXP_MAP;
        label_array[label_index].flags &= ~BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST;
        label_array[label_index].flags &= ~BCM_MPLS_EGRESS_LABEL_EXP_REMARK;
        label_array[label_index].flags &= ~BCM_MPLS_EGRESS_LABEL_WIDE;

        /** In JR2, next protocol depends on the next nibble in PHP. */
        label_array[label_index].flags &= ~(BCM_MPLS_EGRESS_LABEL_PHP_IPV4 |
                                            BCM_MPLS_EGRESS_LABEL_PHP_IPV6 |
                                            BCM_MPLS_EGRESS_LABEL_PHP_L2);
    }

    /** Suppose we needed four tunnel encap */
    access_id = 4;

    skip_count = 0;
    tunnel_id = 0;
    bcm_mpls_egress_label_t_init(&outer_label_array[0]);
    bcm_mpls_egress_label_t_init(&outer_label_array[1]);

    l3_if_id = label_array[num_labels-1].l3_intf_id;

    for (label_index = num_labels - 1; label_index >= 0; label_index = label_index - 1 - skip_count)
    {
        if (label_index >= 1)
        {
            nof_labels = 2;
            sal_memcpy(outer_label_array, &label_array[label_index- 1], sizeof(label_array[0]) * nof_labels);
            couple_flags = outer_label_array[0].flags ^ outer_label_array[1].flags;
            if (is_device_or_above(unit, JERICHO2))
            {
                qos_model_diff = (outer_label_array[0].egress_qos_model.egress_qos != outer_label_array[1].egress_qos_model.egress_qos) ? TRUE : FALSE;
                ttl_model_diff = (outer_label_array[0].egress_qos_model.egress_ttl != outer_label_array[1].egress_qos_model.egress_ttl) ? TRUE : FALSE;
            }
            else
            {
                qos_model_diff = ((couple_flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) || (couple_flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY)) ? TRUE :FALSE;
                ttl_model_diff = ((couple_flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) || (couple_flags & BCM_MPLS_EGRESS_LABEL_TTL_COPY)) ? TRUE :FALSE;
            }
            exp_diff = outer_label_array[0].exp ^ outer_label_array[1].exp;
            ttl_diff = outer_label_array[0].ttl ^ outer_label_array[1].ttl;
            qos_id_diff = outer_label_array[0].qos_map_id ^ outer_label_array[1].qos_map_id;
            failover_id_diff = outer_label_array[0].egress_failover_id ^ outer_label_array[1].egress_failover_id;
            failover_lif_diff = outer_label_array[0].egress_failover_if_id ^ outer_label_array[1].egress_failover_if_id;
            if ( qos_model_diff || ttl_model_diff ||
                (couple_flags & BCM_MPLS_EGRESS_LABEL_PROTECTION) ||
                exp_diff || ttl_diff || qos_id_diff ||
                failover_id_diff || failover_lif_diff)
            {
                /** Label 0 and lable 1 can't be created by the same call.*/
                nof_labels = 1;
                skip_count = 0;
                sal_memcpy(outer_label_array, &label_array[label_index], sizeof(label_array[0]) * nof_labels);
            }
            else
            {
                skip_count = 1;
            }
        }
        else
        {
            /** Only 1 label in the array.*/
            nof_labels = 1;
            skip_count = 0;
            sal_memcpy(outer_label_array, &label_array[label_index], sizeof(label_array[0]) * nof_labels);
        }

        /** Apply the input next-eep (poosibly arp encap-id) for the out-most entry*/
        if (l3_if_id != 0)
        {
            tunnel_id = l3_if_id;
            l3_if_id = 0;
        }

        /** Initialize the next-eep*/
        outer_label_array[0].l3_intf_id = tunnel_id;
        if (nof_labels == 2)
        {
            outer_label_array[1].l3_intf_id = tunnel_id;
        }

        /** Consider tandem encap to reduce the encap stages used by mpls.*/
        if (label_index - 1 - skip_count > 0)
        {
            uint8 tandem_feasible;

            /** Tandem label should has the same EXP and TTL with prior label*/
            exp_diff = outer_label_array[0].exp ^ label_array[label_index - 1 - skip_count - 1].exp;
            ttl_diff = outer_label_array[0].ttl ^ label_array[label_index - 1 - skip_count - 1].ttl;
            qos_id_diff = outer_label_array[0].qos_map_id ^ label_array[label_index - 1 - skip_count - 1].qos_map_id;

            tandem_feasible = !(exp_diff | ttl_diff | qos_id_diff);
            if (tandem_feasible)
            {
                /** It's not the last mpls encap entry, type it with tandem.*/
                if (is_tandem)
                {
                    outer_label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TANDEM;
                    if (nof_labels == 2)
                    {
                        outer_label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_TANDEM;
                    }
                }

                /** Revert the tandem indicator to make sure the prior mpls entry is not tandem.*/
                is_tandem = !is_tandem;
            }
        }

        /** Create the initiator. */
        outer_label_array[0].encap_access = access_phase[access_id];
        if (nof_labels == 2)
        {
            outer_label_array[1].encap_access = access_phase[access_id];
        }
        rv = bcm_mpls_tunnel_initiator_create(unit, intf, nof_labels, outer_label_array);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_mpls_port_add failed: %d \n", rv);
            return rv;
        }

        tunnel_id = outer_label_array[0].tunnel_id;
        label_array[label_index].tunnel_id = tunnel_id;
        if (nof_labels == 2)
        {
            label_array[label_index - 1].tunnel_id = tunnel_id;
        }

        /** Update encap access*/
        access_id --;
    }

    return rv;
}
/*
 * } mpls tunnel initiator create -- end
 */


/*******************************************************************************
 *
 * Add your example above between the frame if you have.
 * Examples translation for the usages of L3 APIs from DPP(JR+) to DNX(JR2) end
 * }
 *
 ******************************************************************************/




