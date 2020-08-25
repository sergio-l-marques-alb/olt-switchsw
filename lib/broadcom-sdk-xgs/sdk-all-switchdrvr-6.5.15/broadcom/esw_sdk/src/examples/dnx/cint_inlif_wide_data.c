/*
 * $Id: cint_inlif_wide_data.c, Exp $
 $Copyright: (c) 2018 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$ File: cint_inlif_wide_data.c Purpose: example for IN-LIF wide data usage
 */

/*
 * 
 * Configuration:
 * 
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/dnx/cint_inlif_wide_data.c
 * cint
 * print run(); 
 * 
 * 
 *  Scenarios configured in this cint:
 *  1. create AC-MP INGRESS and add WIDE_DATA flag: using API bcm_vlan_port_create 
 *      (contained flags:BCM_VLAN_PORT_CREATE_INGRESS_ONLY & BCM_VLAN_PORT_INGRESS_WIDE)     
 *  2. set the wide data for each IN-LIF gport_id, using API bcm_port_wide_data_set.
 *  3. Use the wide data as qualifier in the ingress PMF. 
 *      Set condition for VTT stage 1 or/and VTT stage 5. (Depend on the IN-LIF created: Native goes to VTT5, others to VTT1)
 *      Set an action.  
 *
 *  Note: the wide data scenario is relevant for one of the following IN-LIF types:
 *      a. AC-MP INGRESS, using API bcm_vlan_port_create (flags contain: BCM_VLAN_PORT_CREATE_INGRESS_ONLY & BCM_VLAN_PORT_INGRESS_WIDE)
 *      b. AC-P2P INGRESS, using API bcm_vlan_port_create (flags contain: BCM_VLAN_PORT_CROSS_CONNECT & BCM_VLAN_PORT_INGRESS_WIDE)
 *      c. AC-NATIVE, using API bcm_vlan_port_create (flags contain BCM_VLAN_PORT_NATIVE & BCM_VLAN_PORT_INGRESS_WIDE)
 *      d. EXTENDER, using API bcm_extender_port_add (flags contain BCM_VLAN_PORT_INGRESS_WIDE)

 */

/*  Main Struct  */
struct inlif_wide_data_param_s{
    uint64 wide_data;
    bcm_gport_t vlan_port_id;
    bcm_field_group_t acl_ipmf1_fg_id;
    bcm_field_entry_t ipmf1_entry_handle_1;
    bcm_field_entry_t ipmf1_entry_handle_2;    
};

inlif_wide_data_param_s g_inlif_wide_data;


/**
 * \brief - Initialization of main struct
 *  Function allow to re-write default values
 */
void
inlif_wide_data_init(
    int unit,
    uint32 wide_data)
{
    /** even though the sw api contain 64bits data, the max allowed is much smaller (defined by architecture) */
    COMPILER_64_ZERO(g_inlif_wide_data.wide_data);
    COMPILER_64_SET(g_inlif_wide_data.wide_data, 0x0, wide_data);
    g_inlif_wide_data.vlan_port_id = 0;
}

/**
* \brief - create AC-MP IN-LIF with wide data flag
*  \param [in] unit
*  \param [in] in_port
*  \param [in] vsi
*  \param [in] unit
*/
int
inlif_wide_data_ac_mp_create(
    int unit,
    int in_port,
    int vsi)
{
    bcm_vlan_port_t vlan_port;
    int rc = BCM_E_NONE;
    
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.vsi = vsi;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;

    rc = bcm_vlan_port_create(unit, vlan_port);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rc;
    }   
    g_inlif_wide_data.vlan_port_id = vlan_port.vlan_port_id;
    return rc;
}

/**
* \brief - function set an ingress acl rule.
*          qualifier type is bcmFieldQualifyAcInLifWideData or bcmFieldQualifyNativeAcInLifWideData
*          action is modify the DP value to BLACK.
*  \param [in] unit
*  \param [in] first_run - If TRUE configures the ACL, otherwise just adds a rule to an existing ACL.
*  \param [in] type - qualifier enum to determine the position of the qualifier wide data.
*                 possible values: bcmFieldQualifyAcInLifWideData/bcmFieldQualifyNativeAcInLifWideData
*  \param [in] mask - specific bits which consider in the qualifier type.
*  \param [in] data - generic data used by the IN-LIF
*/
int inlif_wide_data_acl_run(
    int unit,
    int first_run,
    int type,
    int mask,
    uint32 data)
{
    int rc = BCM_E_NONE;    
    bcm_field_context_t ipmf1_context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_in_info;
    bcm_field_entry_t * entry_handle;

    entry_handle = (first_run == TRUE) ? &g_inlif_wide_data.ipmf1_entry_handle_1 : &g_inlif_wide_data.ipmf1_entry_handle_2;

    bcm_field_group_info_t_init(&fg_info);  
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = type;
    
    /**set the actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionDropPrecedence;
    
    if (first_run)
    {
        rc = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, g_inlif_wide_data.acl_ipmf1_fg_id);
        if (rc != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_group_add\n", rc);
            return rc;
        }

        /** prepare attach_info structure */
        bcm_field_group_attach_info_t_init(&attach_info);
        attach_info.key_info.nof_quals = fg_info.nof_quals;
        attach_info.payload_info.nof_actions = fg_info.nof_actions;
        attach_info.payload_info.action_types[0] = fg_info.action_types[0];

        attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[0].input_arg = 0;
    
        rc = bcm_field_group_context_attach(unit, 0, g_inlif_wide_data.acl_ipmf1_fg_id, BCM_FIELD_CONTEXT_ID_DEFAULT, &attach_info);
        if (rc != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_group_context_attach\n", rc);
            return rc;
        }
    }
    
    /** prepare entry_in_info structure */  
    bcm_field_entry_info_t_init(&entry_in_info);
    entry_in_info.priority = 2;
    entry_in_info.nof_entry_quals = 1;
    entry_in_info.nof_entry_actions = 1;    
    entry_in_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_in_info.entry_qual[0].mask[0] = mask;
    entry_in_info.entry_qual[0].value[0] = data; 
    entry_in_info.entry_action[0].type = fg_info.action_types[0];
	/** set Dp=BLACK */
    entry_in_info.entry_action[0].value[0] = BCM_FIELD_COLOR_BLACK;
    rc = bcm_field_entry_add(unit, 0, g_inlif_wide_data.acl_ipmf1_fg_id, &entry_in_info, entry_handle);
    if (rc != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add for context %d, fg_id %d\n", rc, ipmf1_context_id, g_inlif_wide_data.acl_ipmf1_fg_id);
        return rc;
    }
    
    return rc;
}
/**
* \brief - function do the following:
*           1. create AC-MP IN-LIF with wide data flag.
*           2. set wide data for the gport
*           3. create PMF rule based on the wide data. the rule is to change the DP to BLACK.
*           4. drop packets which have DP>= BLACK using ingress cosq API.
*  \param [in] unit
*  \param [in] in_port
*  \param [in] vsi
*  \param [in] unit
*  \param [in] wide_data - generic data used by the AC IN-LIF
*/
int
inlif_wide_data_ac_mp__start_run(
    int unit,
    int in_port,
    int vsi,
    uint32 wide_data)
{       
    int rc = BCM_E_NONE;
    uint64 data;

    printf("inlif_wide_data_ac_mp__start_run: unit=%d, in_port=%d, vsi=%d, wide_data=0x%X \n", 
            unit, in_port, vsi,wide_data);

    inlif_wide_data_init(unit, wide_data);

    /** create AC MP basic bridge, using format that contain generic (wide) data */
    rc = inlif_wide_data_ac_mp_create(unit, in_port, vsi);
    if (rc != BCM_E_NONE)
    {
        printf("Error, inlif_wide_data_ac_mp_create\n");
        return rc;
    }       
    
    /** set the wide data for the created gport */
    rc = bcm_port_wide_data_set(unit, g_inlif_wide_data.vlan_port_id, BCM_PORT_WIDE_DATA_INGRESS, 
                                g_inlif_wide_data.wide_data);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_port_wide_data_set\n");
        return rc;
    }   
       
    /** configure acl rule based on wide data */
    rc = inlif_wide_data_acl_run(unit, 1, bcmFieldQualifyAcInLifWideData, 
                                 0xFF, COMPILER_64_LO(g_inlif_wide_data.wide_data));
    if (rc != BCM_E_NONE)
    {
        printf("Error, inlif_wide_data_acl_set\n");
        return rc;
    }   
    
    /** drop packet which have DP >= 3 (BLACK) */
    rc = bcm_cosq_discard_set(unit, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_cosq_discard_set\n");
        return rc;
    }       
    
    return rc;
} 

/** Destroy ACL rule related  */
int inlif_wide_data_acl_destroy(int unit, int nof_entries)
{
    int rv = BCM_E_NONE;
    bcm_field_context_t ctx;
    bcm_field_stage_t stage;

    if(nof_entries > 2)
    {
        printf("support up to two entries \n");
        return BCM_E_PARAM;        
    }
    if(nof_entries > 1)
    {
        rv = bcm_field_entry_delete(unit, g_inlif_wide_data.acl_ipmf1_fg_id, NULL, g_inlif_wide_data.ipmf1_entry_handle_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_delete  fg %d \n", rv, g_inlif_wide_data.acl_ipmf1_fg_id);
            return rv;
        }
    }
    rv = bcm_field_entry_delete(unit, g_inlif_wide_data.acl_ipmf1_fg_id, NULL, g_inlif_wide_data.ipmf1_entry_handle_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_delete  fg %d \n", rv, g_inlif_wide_data.acl_ipmf1_fg_id);
        return rv;
    }
    /** Sequence to destroy the context and delete the groups -
     *  for now is used the default context -  Cannot destroy
     *  context ID 0, since it is the default context */     
    rv = bcm_field_group_context_detach(unit, g_inlif_wide_data.acl_ipmf1_fg_id, BCM_FIELD_CONTEXT_ID_DEFAULT);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach fg %d from context %d stage %s\n", rv, g_inlif_wide_data.acl_ipmf1_fg_id, 0);
        return rv;
    } 
    rv = bcm_field_group_delete(unit, g_inlif_wide_data.acl_ipmf1_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete  fg %d \n", rv, g_inlif_wide_data.acl_ipmf1_fg_id);
        return rv;
    } 

    return rv;
}

