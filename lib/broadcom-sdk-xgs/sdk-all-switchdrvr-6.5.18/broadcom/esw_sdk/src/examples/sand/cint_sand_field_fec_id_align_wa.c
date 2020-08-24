/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_sand_field_fec_id_align_wa.c,v 1.0 2018/05/11 08:05:38 Jingli Guo $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Purpose: 
 *     FEC ID in learning payload is 15 bits,whereas the FEC ID in JR2 is 18 bits wide.
 *     this WA will tranlsate the global FEC ID from learning 
 *     payload to local FEC ID used for local FEC access.
 *     this is just WA and not the final solution.
 */


/**
 * FEC ID in learning payload is 15 bits,whereas the
 * FEC ID in JR2 is 18 bits wide.
 * this WA will tranlsate the global FEC ID from learning 
 * payload to local FEC ID used for local FEC access.
 * this is just WA and not the final solution.
 */
 int
fec_id_alignment_field_wa_db_init(
    int unit,
    bcm_field_group_t *wa_fg_id,
    int port)
{
   /*
    *  * Context "FEC ID Alignment"
    *   Context_Selection:
    *      Fwd layerType = Eth
    *      out_lif0_range (=0x0)
    *      out_lif1_range (=0x0)
    *   context Key:
    *      fwd_action_dst
    *   Actions: 
    *      fwd_action_dst = local_fec_id
    *     
    */
   int rv;
   bcm_field_presel_entry_data_t p_data;
   bcm_field_presel_entry_id_t p_id;
   bcm_field_context_t context_id;
   bcm_field_context_info_t context_info;
   void *dest_char;
   bcm_field_group_t fg_id = 0;

   bcm_field_group_info_t fg_info;
   bcm_field_group_attach_info_t attach_info;
   
   bcm_field_qualifier_info_create_t qual_info;
   bcm_field_qualify_t qual_id;
   bcm_field_qualifier_info_get_t qual_info_get;
   bcm_field_range_info_t range_info;
   char *proc_name;
   uint32 olp_port_class_id = 0;

   proc_name = "fec_id_alignment_field_wa_db_init";

   bcm_field_group_info_t_init(&fg_info);
   fg_info.fg_type = bcmFieldGroupTypeTcam;
   fg_info.stage = bcmFieldStageIngressPMF1;

   /*
    * Set quals
    */
   fg_info.nof_quals = 1;
   fg_info.qual_types[0] = bcmFieldQualifyDstPort;

   /*
    * Set actions
    */
   fg_info.nof_actions = 1;
   fg_info.action_types[0] = bcmFieldActionRedirect;

   dest_char = &(fg_info.name[0]);
   sal_strncpy_s(dest_char, "J1_FEC_ID_ALIGN_FG", sizeof(fg_info.name));
   rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id);
   if (rv != BCM_E_NONE)
   {
       printf("Error, bcm_field_group_add\n");
       return rv;
   }
   *wa_fg_id = fg_id;

   bcm_field_context_info_t_init(&context_info);
   dest_char = &(context_info.name[0]);
   sal_strncpy_s(dest_char, "J1_FEC_ID_ALIGN_CTX", sizeof(context_info.name));

   rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
   if (rv != BCM_E_NONE)
   {
       printf("Error, bcm_field_context_create\n");
       return rv;
   }


   bcm_field_group_attach_info_t_init(&attach_info);

   attach_info.key_info.nof_quals = fg_info.nof_quals;
   attach_info.payload_info.nof_actions = fg_info.nof_actions;
   attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
   attach_info.payload_info.action_types[0] = fg_info.action_types[0];
   attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;


   rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);
   if (rv != BCM_E_NONE)
   {
       printf("Error, bcm_field_group_context_attach\n");
       return rv;
   }


  
   p_id.presel_id = 100;
   p_id.stage = bcmFieldStageIngressPMF1;
   p_data.entry_valid = TRUE;
   p_data.context_id = context_id;
   p_data.nof_qualifiers = 2;

   p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
   p_data.qual_data[0].qual_arg = 0;
   p_data.qual_data[0].qual_value = bcmFieldLayerTypeEth;
   p_data.qual_data[0].qual_mask = 0x1F;

   rv = bcm_port_class_get(unit, port, 
                   bcmPortClassFieldIngressPMF1PacketProcessingPortCs, &olp_port_class_id);
   if (rv != BCM_E_NONE)
   {               
       printf("Error, bcm_port_class_get\n");
       return rv;
   }   

   p_data.qual_data[1].qual_type = bcmFieldQualifyPortClassPacketProcessing;
   p_data.qual_data[1].qual_arg = 0;
   p_data.qual_data[1].qual_value = olp_port_class_id;
   p_data.qual_data[1].qual_mask = 0x7;


   rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
   if (rv != BCM_E_NONE)
   {
       printf("Error, bcm_field_presel_set\n");
       return rv;
   }

   bcm_field_range_info_t_init(&range_info);
   range_info.range_type = bcmFieldRangeTypeOutVport;
   range_info.min_val = 0;
   range_info.max_val = 0;
   rv = bcm_field_range_set(unit, 0, bcmFieldStageIngressPMF1, 0, &range_info);
   if (rv != BCM_E_NONE)
   {
       printf("%s Error (%d), in bcm_field_range_set\n", proc_name, rv);
       return rv;
   }

   return rv;
}

/**
 * FEC ID alignment WA entry adding routing 
*/
int fec_id_alignment_field_wa_entry_add(int unit,  bcm_field_group_t wa_fg_id, int fec_id)
{
    bcm_field_entry_info_t entry_info;
    bcm_gport_t in_port_gport;
    int rv = BCM_E_NONE;
    uint32 global_fec_id = 0;
    uint32 global_fec_destination = 0, local_fec_destination = 0;
    bcm_field_entry_t entry_id;
    bcm_gport_t local_fec_gport;

    global_fec_id = fec_id & 0x7FFF;
    BCM_GPORT_FORWARD_PORT_SET(local_fec_gport, fec_id);

    
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = 1;
    entry_info.entry_qual[0].type = bcmFieldQualifyDstPort;
    entry_info.entry_qual[0].value[0] = global_fec_id;
    entry_info.entry_qual[0].mask[0] = 0x7FFF ;
    entry_info.entry_action[0].type = bcmFieldActionRedirect;
    entry_info.entry_action[0].value[0] = local_fec_gport;

    rv = bcm_field_entry_add(unit, 0, wa_fg_id , &entry_info, &entry_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("entry %d is installed, convert the global FEC 0x%x to local FEC 0x%x",entry_id,global_fec_id,fec_id);
    
    return rv;

}

int fec_id_alignment_field_wa_add(int unit,  int access_port, int fec_id)
{
    int rv = BCM_E_NONE;
    
    if (is_device_or_above(unit, JERICHO2) && 
        (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0)) {
        bcm_gport_t gport; 
        bcm_field_group_t wa_fg_id;

        BCM_GPORT_LOCAL_SET(gport,access_port);
        rv = fec_id_alignment_field_wa_db_init(unit, &wa_fg_id, gport);
        if (rv != BCM_E_NONE)
        {
            printf("Error, fec_id_alignment_field_wa_db_init\n");
            return rv;
        }

        rv = fec_id_alignment_field_wa_entry_add(unit, wa_fg_id, fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, fec_id_alignment_field_wa_entry_add\n");
            return rv;
        }      
    }

    return rv;
}

