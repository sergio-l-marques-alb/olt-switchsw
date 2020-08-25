/* $Id: cint_ingress_latency.c, v 1.34 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

 /* Explanation:
 *  This cint includes functions that are required to activate ingress latency measurements between the IPP and ITP.
 *
 *
 * Calling Sequence - ingress latency per queue:
 *  1. create 4 consecutive ingress latency profiles.
 *  2. set the properties of the profiles.
 *  3. map base 4 queues to the latency profile
 *
 * Calling Sequence - ingress latency per pp flow:
 *  1. create ingress latency profile.
 *  2. set the properties of the profile.
 *  3. map pp flow by PMF to the latency profile.
 *
 */

/** holds if system header mode is legacy (jericho-1) */
int jericho1_system_header_mode;
/** holds the latency profile gport (for latency per queue, it holds the base profile)*/
bcm_gport_t latency_gport;

/**
* \brief - function allocate 4 consecutive ingress latency profile and map a base queue quartet to the base profile
*/
int ingress_latency_per_queue_example(
    int unit,
    bcm_gport_t gport_q)
{
    int rv = BCM_E_NONE;
    int bin_idx, quartet = 4, profile_idx;
    bcm_gport_t gport, phy_port;
    int base_profile = 16;
    bcm_switch_profile_mapping_t profile_mapping;
    int num_cos_levels;
    uint32 flags;
    /** check if the qid is base of 4 of base of 8 */
    rv = bcm_cosq_gport_get(unit, gport_q, &phy_port, &num_cos_levels, &flags);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_gport_get failed \n");
        return rv;    
    }

    /** create 4 consecutive profiles */
    /** profile id % 4 must be zero. therefore, we are using WITH_ID flag. As example we use base_profile=16 */
    /** For queue mapping, we are using the extension range, because only queues can use the extansion range. */
    /** This give better usage of the profiles range (but it is not madatory) */
    for(profile_idx = 0; profile_idx < quartet; profile_idx++)
    {
        BCM_GPORT_INGRESS_LATENCY_PROFILE_SET(gport, (base_profile + profile_idx));
        rv = bcm_cosq_latency_profile_create(unit, (BCM_COSQ_LATENCY_PROFILE_ID_EXTENSION | BCM_COSQ_LATENCY_PROFILE_WITH_ID), 
            &gport);
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        }

        /** set the properties of the profile. by default all actions related to latency are disabled. */
        /** Example activate some of them */
        rv = bcm_cosq_control_set(unit, gport, 0, bcmCosqControlLatencyTrack, TRUE);
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        }    
        rv = bcm_cosq_control_set(unit, gport, 0, bcmCosqControlLatencyCounterEnable, TRUE);
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        }    
        rv = bcm_cosq_control_set(unit,gport,0,bcmCosqControlLatencyDropEnable, TRUE);
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        }  
        rv = bcm_cosq_control_set(unit,gport,0,bcmCosqControlLatencyEcnEnable, FALSE);
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        } 
        /** set 10000 nano-second drop threshold */
        rv = bcm_cosq_control_set(unit,gport, 0, bcmCosqControlLatencyDropThreshold, 10000);
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        } 
        rv = bcm_cosq_control_set(unit,gport,0,bcmCosqControlLatencyTrackDropPacket, TRUE);
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        }     
        /** set bin threshold in jump of 1000 nano-second each, just for the example */
        for(bin_idx = 0; bin_idx < 7; bin_idx++)
        {
            rv = bcm_cosq_control_set(unit, gport, bin_idx, bcmCosqControlLatencyBinThreshold, (1000 + bin_idx*1000));
            if (rv != BCM_E_NONE)
            {   
                printf("bcm_cosq_latency_profile_create failed \n");
                return rv;    
            }            
        }
        
        printf (" ingress latency profile %d was created \n", gport);
        if (profile_idx == 0)
        {
            latency_gport = gport;
        }
        
    }

    /** map only the base quartet queue to base quartet profile */
    profile_mapping.profile_type = bcmCosqIngressQueuesToLatencyProfile;
    profile_mapping.mapped_profile = latency_gport;
    rv = bcm_cosq_profile_mapping_set(unit, gport_q, 0, 0, &profile_mapping);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_profile_mapping_set failed \n");
        return rv;    
    }  
    /** if 8 queues bundle, map also the second quartet of the bundle */
    if(num_cos_levels == 8)
    {
        rv = bcm_cosq_profile_mapping_set(unit, gport_q, 4, 0, &profile_mapping);
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_profile_mapping_set failed \n");
            return rv;    
        }          
    }

    return rv;
}

/** globals for acl rule, which hlod the field group id and entry id. */
/** use it when you want to destroy the group and/or delete the entry */
bcm_field_group_t ing_lat_fg_id;
bcm_field_entry_t ing_lat_ent_id;


int
ingress_latency_flow_acl_rule_set(
    int unit,
    bcm_field_context_t context,
    int flow_id,
    int flow_profile,
    bcm_field_qualify_t qual_type,
    int qual_mask,
    int qual_data)
{
    int rv;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_gport_t gport;
    void *dest_char;
            
    /** Create and attach first group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /** Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = qual_type;
    /** Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionLatencyFlowId;
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &ing_lat_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }        

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, ing_lat_fg_id, context, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    
    /* Add entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = qual_data;
    ent_info.entry_qual[0].mask[0] = qual_mask;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    /** for jericho-1 system header mode, flow-id holds the id and the profile as follow: */
    /** Flow-ID [18:3] represents the flow id, Flow-ID [2:0] represents Latency-Flow-Profile */
    if(jericho1_system_header_mode == TRUE)
    {
        flow_id = flow_id << 3 | flow_profile;
    }
    /**  { latency_flow_profile(4), latency_flow_id(19), latency_flow_id_valid(1) } */
    ent_info.entry_action[0].value[0] = 1;    
    ent_info.entry_action[0].value[1] = flow_id;
    ent_info.entry_action[0].value[2] = flow_profile;

    rv = bcm_field_entry_add(unit, 0, ing_lat_fg_id, &ent_info, &ing_lat_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    printf("Entry add: id:(0x%x) ingress latency: flow decode (=%d)\n", ing_lat_fg_id ,ent_info.entry_action[0].value[0]);

    return rv;
}

/**
* \brief - function allocate 1 ingress latency profile and map a flow (by PMF) to the profile
*/
int ingress_latency_per_flow_example(
    int unit,
    int flow_id,
    int in_port,
    bcm_field_context_t pmf_context)
{
    int rv = BCM_E_NONE;
    int bin_idx, profile_idx;
    bcm_gport_t gport_qual;

    /** create a latency profile */
    rv = bcm_cosq_latency_profile_create(unit, 0, &latency_gport);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_latency_profile_create failed \n");
        return rv;    
    }

    /** set the properties of the profile. by default all actions related to latency are disabled. */
    /** Example activate some of them */
    rv = bcm_cosq_control_set(unit, latency_gport, 0, bcmCosqControlLatencyTrack, TRUE);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_latency_profile_create failed \n");
        return rv;    
    }   
    rv = bcm_cosq_control_set(unit , latency_gport, 0, bcmCosqControlLatencyCounterEnable, TRUE);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_latency_profile_create failed \n");
        return rv;    
    }    
    rv = bcm_cosq_control_set(unit,latency_gport,0,bcmCosqControlLatencyDropEnable, TRUE);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_latency_profile_create failed \n");
        return rv;    
    }  
    rv = bcm_cosq_control_set(unit,latency_gport,0,bcmCosqControlLatencyEcnEnable, FALSE);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_latency_profile_create failed \n");
        return rv;    
    } 
    /** set 10000 nano-second drop threshold */
    rv = bcm_cosq_control_set(unit,latency_gport, 0, bcmCosqControlLatencyDropThreshold, 20000);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_latency_profile_create failed \n");
        return rv;    
    } 
    rv = bcm_cosq_control_set(unit,latency_gport,0,bcmCosqControlLatencyTrackDropPacket, TRUE);
    if (rv != BCM_E_NONE)
    {   
        printf("bcm_cosq_latency_profile_create failed \n");
        return rv;    
    }     
    /** set bin threshold in jump of 1000 nano-second each, just for the example */
    for(bin_idx = 0; bin_idx < 7; bin_idx++)
    {
        rv = bcm_cosq_control_set(unit,latency_gport, bin_idx, bcmCosqControlLatencyBinThreshold, (1000 + bin_idx*1000));
        if (rv != BCM_E_NONE)
        {   
            printf("bcm_cosq_latency_profile_create failed \n");
            return rv;    
        }            
    }
    printf (" ingress latency profile %d was created \n", latency_gport);

    /** map a flow to the profile */
    BCM_GPORT_LOCAL_SET(gport_qual, in_port);
    rv = ingress_latency_flow_acl_rule_set(unit, pmf_context, flow_id, BCM_GPORT_INGRESS_LATENCY_PROFILE_GET(latency_gport),
        bcmFieldQualifyInPort, 0x1FF, gport_qual);
    if (rv != BCM_E_NONE)
    {   
        printf("ingress_latency_flow_acl_rule_set failed \n");
        return rv;    
    }      
    return rv;
}

/** hold the max latency value */
uint32 max_latency;
/** hold the dest_gport that belong to the max latency value */
bcm_gport_t dest_gport;


/**
* \brief - function get from HW the max 8 latencies per core
*/
int ingress_latency_max_table_get(
    int unit)
{
    int rv = BCM_E_NONE;
    int actual_count, i;
    int core_idx;
    bcm_gport_t gport;
    uint32 nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
    bcm_cosq_max_latency_pkts_t max_latency_pkts[8];
    int memory_actual_count;
    for (core_idx; core_idx < nof_cores; core_idx++)
    {
        BCM_COSQ_GPORT_CORE_SET(gport, core_idx);
        rv = bcm_cosq_max_latency_pkts_get(unit, gport, 0, 8, max_latency_pkts, &memory_actual_count) ;
        if (BCM_E_NONE != rv) {
              printf("bcm_cosq_icgm_max_latency_pkts_get failed\n");
              return rv;
        }
        printf("------- MAX LATENCY TABLE PRINT: (size=%d, core=%d) --------\n", memory_actual_count, core_idx);  
        for(i=0;i<memory_actual_count; i++)
        {
            printf("max_latency_pkt[%d]:\n",i);
            print("\tlatency=\n");
            print("\t%x\n",max_latency_pkts[i].latency);
            printf("\tlatency_flow=0x%x\n",max_latency_pkts[i].latency_flow);
            printf("\tdest_gport=0x%x\n",max_latency_pkts[i].dest_gport);
            printf("\tcosq (tc)=0x%x\n",max_latency_pkts[i].cosq);
            /** save the max latency in global variable */
            if(COMPILER_64_HI(max_latency_pkts[i].latency) !=  0)
            {
                printf("invalid latency value (>32 bits)\n");
                return BCM_E_FAIL; 
            }
            if(COMPILER_64_LO(max_latency_pkts[i].latency) >  max_latency)
            {
                max_latency = COMPILER_64_LO(max_latency_pkts[i].latency);
                dest_gport = max_latency_pkts[i].dest_gport;
            }               
        }               
    }
    
    return rv;
}

/**
* \brief - create counter processor database in order to hold the max latency value per flow/queue.
*/
int ingress_latency_counter_database_set(
    int unit, 
    int engineId,
    int max_flow_id,
    bcm_stat_expansion_types_t expansion_type)
{
    int rv = BCM_E_NONE;
    int database_id;
    bcm_stat_counter_database_t database;
    bcm_stat_engine_t engine;
    bcm_stat_counter_enable_t enable_config;
    bcm_stat_counter_interface_t cnt_interface;
    bcm_stat_counter_set_map_t ctr_set_map;
    bcm_stat_eviction_t eviction;
    bcm_stat_counter_interface_key_t interface_key;
    bcm_stat_expansion_select_t expansion_select;
    
    bcm_stat_counter_database_t_init(&database);
    bcm_stat_engine_t_init(&engine);
    bcm_stat_eviction_t_init(&eviction);
    bcm_stat_counter_set_map_t_init(&ctr_set_map);
    bcm_stat_counter_interface_key_t_init(&interface_key);
    bcm_stat_expansion_select_t_init(&expansion_select);
    bcm_stat_counter_interface_t_init(&cnt_interface);
    bcm_stat_counter_enable_t_init(&enable_config);

    /** expansion per bins */
    interface_key.command_id = 0;
    interface_key.core_id = BCM_CORE_ALL;
    interface_key.interface_source = bcmStatCounterInterfaceIngressTransmitPp;
    interface_key.type_id = 0;
    expansion_select.nof_elements = 1;
    /** possible value: bcmStatExpansionTypeLatencyFlowProfile, bcmStatExpansionTypeTrafficClass, bcmStatExpansionTypeLatencyBin */
    expansion_select.expansion_elements[0].type = expansion_type; 
    rv = bcm_stat_counter_expansion_select_set(unit, 0, &interface_key, &expansion_select);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_expansion_select_set\n");
        return rv;
    }
    /** configure and create database */
    database.database_id = engineId;
    database.core_id = BCM_CORE_ALL;
    rv = bcm_stat_counter_database_create(unit, BCM_STAT_DATABASE_CREATE_WITH_ID, BCM_CORE_ALL, &database.database_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_create\n");
        return rv;
    }
    
    /** attach the engines to the database */
    rv = bcm_stat_counter_engine_attach(unit, 0, &database, engineId);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_engine_attach\n");
        return rv;
    }
   /** configure the counter eviction per engine */
    engine.engine_id = engineId;
    engine.core_id = BCM_CORE_ALL;
    eviction.dma_fifo_select = 0;
    eviction.record_format = bcmStatEvictionRecordFormatPhysical;
    eviction.eviction_event_id = database_id;
    eviction.type = bcmStatEvictionDestinationLocalHost;
    rv = bcm_stat_counter_eviction_set(unit, 0, &engine, &eviction);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_eviction_set\n");
        return rv;
    }    

     /** configure the counter interface per database */
    cnt_interface.source = bcmStatCounterInterfaceIngressTransmitPp;
    cnt_interface.command_id = 0;
    /** if the expansion is per bin, it means that we want to count packets per bins and not save the max value. */
    if(expansion_type == bcmStatExpansionTypeLatencyBin)
    {
        cnt_interface.format_type = bcmStatCounterFormatDoublePackets;
    }
    else
    {
        cnt_interface.format_type = bcmStatCounterFormatMaxSize;
    }
    
    if(expansion_type == bcmStatExpansionTypeLatencyFlowProfile)
    {
        cnt_interface.counter_set_size = 32;
    }
    else
    {
        cnt_interface.counter_set_size = 8;
    }
    cnt_interface.type_config[0].valid = 1;
    cnt_interface.type_config[0].object_type_offset = 0;
    cnt_interface.type_config[0].start = 0;
    cnt_interface.type_config[0].end = max_flow_id;
    rv = bcm_stat_counter_interface_set(unit, 0, &database, &cnt_interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_set\n");
        return rv;
    }

    /**  configure the counter set mapping per database */
    rv = bcm_stat_counter_set_mapping_set(unit, BCM_STAT_COUNTER_MAPPING_FULL_SPREAD, &database, &ctr_set_map);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_set_mapping_set\n");
        return rv;
    }

    /**  enable the engine */
    enable_config.enable_counting = TRUE;
    enable_config.enable_eviction = TRUE;
    rv = bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_enable_set\n");
        return rv;
    }
    return rv;
}
