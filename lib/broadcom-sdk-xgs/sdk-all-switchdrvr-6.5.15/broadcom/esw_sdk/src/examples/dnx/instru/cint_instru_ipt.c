/*
 * $Id: cint_instru_ipt.c,v 1.15 2018/04/16 12:47:34 Leon Akura Exp $
 $Copyright: (c) 2018 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$ File: cint_instru_ipt.c Purpose: IPT profiles configurations (INT nodes, Tail Edit)
 */

/*
 * File: cint_vpls_statistics.c
 * Purpose: Example of IPT profiles (INT, Tail Edit) congfiguration
 *
 * Configure full metadata for INT over Vxlan (32 bytes) and Tail-Edit (30 bytes) profiles and first header (iOAM) information for INT.
 *
 * The cint includes:
 * - Main function to configure IPT profiles and
 *
 * Usage:
 * ------
 *
 * Test Scenario:
 *--------------
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint
 * cint_instru_ipt_main(0,0xcd, 0x2);
 * exit;
 *
 * After calling this cint, it can be tested using PMF rules that yield these IPT profiles (INT/Tail-Edit)
 *
 * Note, to clear configurations back to default call:
 * cint
 * cint_instru_ipt_clear(0);
 * exit;
 */


/*
 * main function.
 * Calls:
 *
 */

/**
 * \brief IPT profile IDs
 */
int CINT_INSTRU_IPT_TAIL_EDIT_PROFILE              = 1;
int CINT_INSTRU_IPT_INT_INTERMEDIATE_NODE_PROFILE  = 2;
int CINT_INSTRU_IPT_INT_FIRST_NODE_PROFILE         = 4;
int CINT_INSTRU_IPT_INT_LAST_NODE_PROFILE          = 6;

/*
 * Default values used for setting back defaults
 */
bcm_instru_ipt_t default_tail_edit_config;
bcm_instru_ipt_t default_int_first_config;
bcm_instru_ipt_t default_int_intermediate_config;
int default_switch_id;

int
cint_instru_ipt_main(
   int unit,
   int switch_id,
   int shim_type)
{
    int rv;
    uint32 ipt_profile;
    bcm_instru_ipt_t config;

    /** set tail-edit profile */
    {
        ipt_profile = CINT_INSTRU_IPT_TAIL_EDIT_PROFILE;

        /** get default */
        rv  = bcm_instru_ipt_profile_get(unit, 0, ipt_profile, &default_tail_edit_config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_get failed for Tail Edit\n");
            return rv;
        }

        bcm_instru_ipt_t_init(&config);

        config.node_type = bcmInstruIptNodeTypeTail;
        config.metadata_flags = (BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_TOD |
                BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_SYS_PORT |BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_DEV_ID |
                BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_RESERVED | BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_TOD |
                BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_SYS_PORT | BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_DEV_ID |
                BCM_INSTRU_IPT_METADATA_FLAG_SWITCH_ID | BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO);

        rv  = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, &config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_set failed for Tail Edit\n");
            return rv;
        }
    }

    /** set INT-first node profile */
    {
        ipt_profile = CINT_INSTRU_IPT_INT_FIRST_NODE_PROFILE;

        /** get default */
        rv  = bcm_instru_ipt_profile_get(unit, 0, ipt_profile, &default_int_first_config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_get failed for INT first node\n");
            return rv;
        }

        bcm_instru_ipt_t_init(&config);

        config.node_type = bcmInstruIptNodeTypeIntOverVxlanFirst;
        config.metadata_flags = (BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_TOD | BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_RESERVED |
                BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_SYS_PORT |BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_DEV_ID |
                BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_RESERVED | BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_TOD |
                BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_SYS_PORT | BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_DEV_ID |
                BCM_INSTRU_IPT_METADATA_FLAG_SWITCH_ID | BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO);

        config.first_node_header.length = 0;
        config.first_node_header.options = 0xff;
        config.first_node_header.type = shim_type;

        rv = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, &config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_set failed for INT first node\n");
            return rv;
        }
    }


    /** set INT-Intermediate node profile */
    {
        ipt_profile = CINT_INSTRU_IPT_INT_INTERMEDIATE_NODE_PROFILE;

        /** get default */
        rv  = bcm_instru_ipt_profile_get(unit, 0, ipt_profile, &default_int_intermediate_config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_get failed for INT intermediate node\n");
            return rv;
        }

        bcm_instru_ipt_t_init(&config);

        config.node_type = bcmInstruIptNodeTypeIntOverVxlanIntermediate;
        config.metadata_flags = (BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_TOD | BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_RESERVED |
                BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_SYS_PORT |BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_DEV_ID |
                BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_RESERVED | BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_TOD |
                BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_SYS_PORT | BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_DEV_ID |
                BCM_INSTRU_IPT_METADATA_FLAG_SWITCH_ID | BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO);

        rv = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, &config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_set failed for INT intermediate node\n");
            return rv;
        }
    }

    /** get default switch id */
    rv = bcm_instru_control_get(unit, 0, bcmInstruControlIptSwitchId, &default_switch_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_control_get failed for setting switch id\n");
        return rv;
    }

    /** set switch ID */
    rv = bcm_instru_control_set(unit, 0, bcmInstruControlIptSwitchId, switch_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_control_set failed for setting switch id\n");
        return rv;
    }

    return rv;
}


int
cint_instru_ipt_clear(
   int unit)
{
    int rv;
    int ipt_profile;

    /** set tail-edit profile */
    {
        ipt_profile = CINT_INSTRU_IPT_TAIL_EDIT_PROFILE;

        rv  = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, default_tail_edit_config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_set failed for default Tail Edit\n");
            return rv;
        }
    }

    /** set INT-first node profile */
    {
        ipt_profile = CINT_INSTRU_IPT_INT_FIRST_NODE_PROFILE;

        rv = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, default_int_first_config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_set failed for default INT first node\n");
            return rv;
        }
    }


    /** set INT-Intermediate node profile */
    {
        ipt_profile = CINT_INSTRU_IPT_INT_INTERMEDIATE_NODE_PROFILE;

        rv = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, default_int_intermediate_config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_set failed for default INT intermediate node\n");
            return rv;
        }
    }

    /** set switch ID */
    rv = bcm_instru_control_set(unit, 0, bcmInstruControlIptSwitchId, default_switch_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_control_set failed for setting default switch id\n");
        return rv;
    }

    return rv;
}
