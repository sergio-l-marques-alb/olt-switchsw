
const int DATA_MAPPING_ARRAY_MAX = 10;
int pfcdm_counter_num = 0;

/* simple example */
uint32_t pfcdm_counter_usage[PFCDM_COUNTER_NUM_MAX];


int pfcdm_stat_id_alloc(uint32_t sw_flow_id, uint32_t *stat_id)
{
    int index = 0;
    for (index = 0; index < pfcdm_counter_num; index ++)
    {
        if (pfcdm_counter_usage[index] == PFCDM_INVALID_ENTRY_ID)
        {
            break;
        }
    }
    if (index >= pfcdm_counter_num)
    {
        printf("stat id is full\n");
        return BCM_E_FULL;
    }
	
    *stat_id = index;
    pfcdm_counter_usage[index] = sw_flow_id;
    return BCM_E_NONE;
}

int pfcdm_stat_id_free(int stat_id)
{
    if (stat_id >= pfcdm_counter_num)
    {
        return -1;
    } 
	
    pfcdm_counter_usage[stat_id] = PFCDM_INVALID_ENTRY_ID;
    return BCM_E_NONE;
}


void pfcdm_single_stat_multiple_show(int unit, int core_id, int stat_id, int number)
{
    int i = 0;
	
    for (i = 0; i < number; i ++)
    {
        pfcdm_single_stat_show(unit, core_id, stat_id + i);
    }
}

int pfcdm_single_stat_show(int unit, int core_id, int stat_id)
{
    int rv = BCM_E_NONE;
    int stat_array[100];
    bcm_stat_counter_input_data_t input_data;
    bcm_stat_counter_output_data_t output_data;
    bcm_stat_counter_input_data_t_init(&input_data);
    bcm_stat_counter_output_data_t_init(&output_data);
    bcm_stat_counter_value_t counter_value[2];

    /* offset 0 - tm drop */
    /* offset 1 - pp drop */
    stat_array[0] = BCM_STAT_COUNTER_STAT_SET(0, 0);
    stat_array[1] = BCM_STAT_COUNTER_STAT_SET(0, 1);

    /** fill the input structure */
    input_data.core_id = core_id;
    input_data.database_id = 0;
    input_data.type_id = PFCDM_COUNTER_TYPE_ID;
    input_data.counter_source_id = stat_id;
    input_data.nstat = 2;
    input_data.stat_arr = stat_array;
    output_data.value_arr = counter_value;

    /** read the counters */
    rv = bcm_stat_counter_get(unit, 0, &input_data, &output_data);

    print counter_value;

    return rv;
}



int pfcdm_counter_mirror_single_init(int unit, int stat_id)
{
    bcm_mirror_profile_cmd_count_t mirror_count_info;
    sal_memset(&mirror_count_info, 0, sizeof(mirror_count_info));
    mirror_count_info.is_counted = 0; /* NOT BCM_MIRROR_COUNT_AS_ORIG */
    mirror_count_info.object_stat_id = stat_id;
    mirror_count_info.type_id = 0;
    bcm_mirror_destination_count_command_set(unit, BCM_CORE_ALL, sampling_mirror_destination, 
        PFCDM_COUNTER_COMMAND, PFCDM_COUNTER_TYPE_ID, &mirror_count_info);
    bcm_mirror_destination_count_command_set(unit, BCM_CORE_ALL, pfcdm_snoop_gport_id, 
        PFCDM_COUNTER_COMMAND, PFCDM_COUNTER_TYPE_ID, &mirror_count_info);
    bcm_mirror_destination_count_command_set(unit, BCM_CORE_ALL, sampling_mirror_destination_2, 
        PFCDM_COUNTER_COMMAND, PFCDM_COUNTER_TYPE_ID, &mirror_count_info);
    return BCM_E_NONE;
}


int
pfcdm_crps_irpp_expansion_builder(
    int unit,
    bcm_stat_expansion_select_t * expansion_select,
    bcm_stat_counter_set_map_t * counter_set_map,
    int *counter_set_size)
{
    int rv, i;
    int meter_0_Valid, meter_0_val, final_meter_val;
    int reject, color;

    /** expansion selection structure. */
    expansion_select->nof_elements = 2;
    expansion_select->expansion_elements[0].type = bcmStatExpansionTypeDispositionIsDrop;
    expansion_select->expansion_elements[1].type = bcmStatExpansionTypeMetaData;
    expansion_select->expansion_elements[1].bitmap = 0x1;

    /** counter set mapping structure. */
    /** Offset 0 - TM drop */
    /** Offset 1 - PP drop  */
    *counter_set_size = 2;
    /** Entry 0 - TM drop; Entry 1 - clear PP drop; Entry 2 - PP
      *  drop with priority
      *  
      *  The case of TM FWD and PP FWD will not be counted at all */
    counter_set_map->nof_entries = 3;
    /** Configure entry 0 - TM drop */
    /** For TM drop - DispositionIsDrop=1, MetaData(PP drop) = 0 */
    counter_set_map->expansion_data_mapping[0].nof_key_conditions = 2;
    counter_set_map->expansion_data_mapping[0].key[0].type = bcmStatExpansionTypeDispositionIsDrop;
    counter_set_map->expansion_data_mapping[0].key[0].value = 1;
    counter_set_map->expansion_data_mapping[0].key[1].type = bcmStatExpansionTypeMetaData;
    counter_set_map->expansion_data_mapping[0].key[1].value = 0;
    counter_set_map->expansion_data_mapping[0].value.counter_set_offset = 0;
    counter_set_map->expansion_data_mapping[0].value.valid = TRUE;

    /** Configure entry 1 - PP drop */
    /** PP drop consists from two entries */
    /** Entry 1: in case of clear pp drop */
    /** For PP drop - DispositionIsDrop=0, MetaData(PP drop)=1 */
    counter_set_map->expansion_data_mapping[1].nof_key_conditions = 2;
    counter_set_map->expansion_data_mapping[1].key[0].type = bcmStatExpansionTypeDispositionIsDrop;
    counter_set_map->expansion_data_mapping[1].key[0].value = 0;
    counter_set_map->expansion_data_mapping[1].key[1].type = bcmStatExpansionTypeMetaData;
    counter_set_map->expansion_data_mapping[1].key[1].value = 1;
    counter_set_map->expansion_data_mapping[1].value.counter_set_offset = 1;
    counter_set_map->expansion_data_mapping[1].value.valid = TRUE;

    /** Entry 2: in case pp drop priority  */
    /** In case of both PP and TM drops - PP drop takes priority */
    /** For PP drop priority - DispositionIsDrop=1, MetaData(PP
      *  drop)=1 */
    counter_set_map->expansion_data_mapping[2].nof_key_conditions = 2;
    counter_set_map->expansion_data_mapping[2].key[0].type = bcmStatExpansionTypeDispositionIsDrop;
    counter_set_map->expansion_data_mapping[2].key[0].value = 1;
    counter_set_map->expansion_data_mapping[2].key[1].type = bcmStatExpansionTypeMetaData;
    counter_set_map->expansion_data_mapping[2].key[1].value = 1;
    counter_set_map->expansion_data_mapping[2].value.counter_set_offset = 1;
    counter_set_map->expansion_data_mapping[2].value.valid = TRUE;

    return BCM_E_NONE;
}




int
pfcdm_crps_irpp_expansion_tm_builder(
    int unit,
    bcm_stat_expansion_select_t * expansion_select,
    bcm_stat_counter_set_map_t * counter_set_map,
    int *counter_set_size)
{
    int rv, i;
    int meter_0_Valid, meter_0_val, final_meter_val;
    int reject, color;

    /** expansion selection strucutre. */
    expansion_select->nof_elements = 1;
    expansion_select->expansion_elements[0].type = bcmStatExpansionTypeDispositionIsDrop;

    /** counter set mappping strucutre. */
    /** Offset 0 - No TM drop */
    /** Offset 1 - TM drop  */
    *counter_set_size = 2;

    /** Entry 0 - No TM drop; Entry 1 - TM Drop */
    counter_set_map->nof_entries = 2;

    /** Configure entry 0 - TM drop */
    /** For No TM drop - DispositionIsDrop=0 */
    counter_set_map->expansion_data_mapping[0].nof_key_conditions = 1;
    counter_set_map->expansion_data_mapping[0].key[0].type = bcmStatExpansionTypeDispositionIsDrop;
    counter_set_map->expansion_data_mapping[0].key[0].value = 0;
    counter_set_map->expansion_data_mapping[0].value.counter_set_offset = 0;
    counter_set_map->expansion_data_mapping[0].value.valid = TRUE;

    /** Configure entry 1 - TM drop */
    /** For TM drop - DispositionIsDrop=1 */
    counter_set_map->expansion_data_mapping[1].nof_key_conditions = 1;
    counter_set_map->expansion_data_mapping[1].key[0].type = bcmStatExpansionTypeDispositionIsDrop;
    counter_set_map->expansion_data_mapping[1].key[0].value = 1;
    counter_set_map->expansion_data_mapping[1].value.counter_set_offset = 1;
    counter_set_map->expansion_data_mapping[1].value.valid = TRUE;

    return BCM_E_NONE;
}


int
pfcdm_crps_irpp_data_base_create(
    int unit,
    int core,
    int database_id,
    int command_id,
    int nof_types,
    int *start_object_stat_id,
    int *end_object_stat_id,
    int *type_valid,
    uint32 *type_offset,
    int nof_engines,
    int *engine_id_arr)
{
    int rv, i, type_id;
    int interface_flags = 0;
    int mapping_flags = 0;
    int eviction_flags = 0;
    int enable_flags = 0;
    bcm_stat_counter_database_t database;
    bcm_stat_engine_t engine;
    bcm_stat_counter_enable_t enable_config;
    bcm_stat_counter_interface_t cnt_interface;
    bcm_stat_counter_set_map_t ctr_set_map;
    bcm_stat_eviction_t eviction;
    bcm_stat_counter_interface_key_t interface_key;
    bcm_stat_expansion_select_t expansion_select;
    bcm_stat_counter_engine_control_t control = bcmStatCounterClearAll;
    bcm_stat_expansion_data_mapping_t data_mapping_array[DATA_MAPPING_ARRAY_MAX];
    int counter_set_size;

    bcm_stat_counter_database_t_init(&database);
    bcm_stat_engine_t_init(&engine);
    bcm_stat_eviction_t_init(&eviction);
    bcm_stat_counter_set_map_t_init(&ctr_set_map);
    bcm_stat_counter_interface_key_t_init(&interface_key);
    bcm_stat_expansion_select_t_init(&expansion_select);
    bcm_stat_counter_interface_t_init(&cnt_interface);
    bcm_stat_counter_enable_t_init(&enable_config);

    /** configure the expansion selection and counter set mapping */
    ctr_set_map.expansion_data_mapping = data_mapping_array;
    rv = pfcdm_crps_irpp_expansion_builder(unit, &expansion_select, &ctr_set_map, &counter_set_size);
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_irpp_expansion_builder\n");
        return rv;
    }

    /*
     * step 1: set the expansion bits format for each interface, using API bcm_stat_counter_expansion_select_set
     * set the interface key. (the selection structure was already feel in function "crps_irpp_expansion_builder" )
     */
    interface_key.core_id = core;
    interface_key.command_id = command_id;
    interface_key.interface_source = bcmStatCounterInterfaceIngressReceivePp;

    /** each type requires expansion selection. */
    for (i = 0; i < nof_types; i++)
    {
        interface_key.type_id = i;
        rv = bcm_stat_counter_expansion_select_set(unit, 0, &interface_key, &expansion_select);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_expansion_select_set\n");
            return rv;
        }
    }

    /** step 2: configure and create database - use user selected
     *  database_id */
    database.database_id = database_id;
    database.core_id = core;
    rv = bcm_stat_counter_database_create(unit, BCM_STAT_DATABASE_CREATE_WITH_ID, core, database_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_create\n");
        return rv;
    }

    /** configure the structure for API bcm_stat_counter_eviction_set */
    eviction.dma_fifo_select = 0;
    eviction.record_format = bcmStatEvictionRecordFormatPhysical;
    eviction.eviction_event_id = database_id;
    eviction.type = bcmStatEvictionDestinationLocalHost;
    /** calling the APIs per engine, in the right order - attach
     *  engines to the database and select eviction */
    for (i = 0; i < nof_engines; i++)
    {
        engine.core_id = core;
        engine.engine_id = engine_id_arr[i];

        printf("*********** set API: engine_id[%d], core=%d **************\n\n", engine.engine_id, engine.core_id);

        /** step 3: attach the engines to the database */
        rv = bcm_stat_counter_engine_attach(unit, 0, &database, engine_id_arr[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_engine_attach\n");
            return rv;
        }
       /** step 4: configure the counter eviction per engine */
        rv = bcm_stat_counter_eviction_set(unit, eviction_flags, &engine, &eviction);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_stat_counter_eviction_set\n");
            return rv;
        }
    }
     /** step 5: configure the counter interface per database */
    cnt_interface.source = bcmStatCounterInterfaceIngressReceivePp;
    cnt_interface.command_id = command_id;
    cnt_interface.format_type = bcmStatCounterFormatPacketsAndBytes;
    cnt_interface.counter_set_size = counter_set_size;

    for (type_id = 0; type_id < nof_types; type_id++)
    {
        /** get type validity and offset from prior configuration */
        cnt_interface.type_config[type_id].valid = type_valid[type_id];
        cnt_interface.type_config[type_id].object_type_offset = type_offset[type_id];
        cnt_interface.type_config[type_id].start = start_object_stat_id[type_id];
        cnt_interface.type_config[type_id].end = end_object_stat_id[type_id];
    }
    rv = bcm_stat_counter_interface_set(unit, interface_flags, &database, &cnt_interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_interface_set\n");
        return rv;
    }

    /** step 6: configure the counter set mapping per engine */
    rv = bcm_stat_counter_set_mapping_set(unit, mapping_flags, &database, &ctr_set_map);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_set_mapping_set\n");
        return rv;
    }

    /** step 7: enable the engine */
    enable_config.enable_counting = TRUE;
    enable_config.enable_eviction = TRUE;
    rv = bcm_stat_counter_database_enable_set(unit, 0, &database, &enable_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_database_enable_set\n");
        return rv;
    }

    /** print the data base */
    /*
     * rv = crps_data_base_print(unit, &interface_key, &database, engine_id_arr[0]); if (rv != BCM_E_NONE) {
     * printf("Error in crps_data_base_print\n"); return rv; }
     */
    return BCM_E_NONE;
}



int
pfcdm_irpp_database_example(
    int unit,
    int database_id)
{
    int base_engine_id = 0;

    int command_id = PFCDM_COUNTER_COMMAND;
	
    /* For core all */
    int core_id = BCM_CORE_ALL;

    int rv;
    int type_id, index, engine_idx;
    int nof_engines = PFCDM_COUNTER_ENGINE_NUM;
    int engine_arr[PFCDM_COUNTER_ENGINE_NUM];
    int nof_types = 1;
    int type_valid[1] = { 1};
    uint32 *engine_size;
    uint32 total_engine_size = 0;
    uint32 type_offset[1] = { 0};
    int start_object_stat_id_arr[1] = { 0};
    int end_object_stat_id_arr[1];

    int counter_set_size[1] = {2 };    /* match to the expansion flag array */

    for (index = 0; index < nof_engines; index ++)
    {
        engine_arr[index] = base_engine_id + index;

        engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_arr[0]);
        total_engine_size += *engine_size;
        printf("engine %d, size %d\n", index, *engine_size);
    }
	
    /** all engines are used for one type  */
    end_object_stat_id_arr[0] = (*engine_size) / counter_set_size[0] - 1;
    type_offset[0] = 0;

    rv = pfcdm_crps_irpp_data_base_create(unit, core_id, database_id, command_id, nof_types,
                                      start_object_stat_id_arr, end_object_stat_id_arr, type_valid, type_offset,
                                      nof_engines, engine_arr);
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_irpp_data_base_create \n");
        return rv;
    }

    pfcdm_counter_num = end_object_stat_id_arr[0];

    printf("Stats counter pairs %d.\n", pfcdm_counter_num);
	
    if (pfcdm_counter_num > PFCDM_COUNTER_NUM_MAX)
    {
        pfcdm_counter_num = PFCDM_COUNTER_NUM_MAX;
    }

    printf("PFCDM counter pairs %d.\n", pfcdm_counter_num);
	
    /* configure to not count mirror/sampling/snoop */
    for (index = 0; index < pfcdm_counter_num; index ++)
    {
        pfcdm_counter_usage[index] = PFCDM_INVALID_ENTRY_ID;
        pfcdm_counter_mirror_single_init(unit, index);
    }

    return BCM_E_NONE;
}
