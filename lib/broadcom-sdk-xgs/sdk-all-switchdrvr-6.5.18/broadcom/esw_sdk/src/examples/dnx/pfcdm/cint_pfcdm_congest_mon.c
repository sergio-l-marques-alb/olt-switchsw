
const int pfcdm_cong_mon_event_id = 874;
uint16_t pfcdm_voq_id_map_table[PFCDM_VOQ_NUM];
pfcdm_voq_congest_mon_entry_t pfcdm_congest_mon_table[PFCDM_VOS_CONGEST_MON_TABLE_SIZE];


static int pfcdm_congest_mon_sw_entry_alloc(uint16_t *entry_id_ptr)
{
    uint16_t i = 0;
	
    for (i = 0; i < PFCDM_VOS_CONGEST_MON_TABLE_SIZE; i ++)
    {
        if (0 == pfcdm_congest_mon_table[i].valid)
        {
            break;
		}
	}
	
    if (i >= PFCDM_VOS_CONGEST_MON_TABLE_SIZE)
    {
        printf("PFCDM congestion mon table is full.\n");
        return BCM_E_FULL;
	}
	
    *entry_id_ptr = i;
    return BCM_E_NONE;
}



/* could save in application layer as well */
static int pfcdm_voq_id_output_pri_map(int unit, int core_id, uint16_t *out_port, uint8_t *pri)
{
    /* just for reference code */
    *out_port = 0xc000 | out_port_ingress;
    *pri = 0; 
    return BCM_E_NONE;
}



int pfcdm_congest_mon_process(int unit, int core_id, uint16_t voq_id)
{
    int rv = BCM_E_NONE;
    uint16_t out_port = 0;
    uint8_t pri = 0;
    uint16_t sw_entry_id = 0;
    bcm_field_entry_t hw_entry_id = 0;
    pfcdm_voq_congest_mon_entry_t *entry_ptr = NULL;

    printf("VOQ_ID (%d) \n", voq_id);

    if (voq_id >= PFCDM_VOQ_NUM)
    {
        printf("VOQ_ID (%d) is invalid\n", voq_id);
        return BCM_E_PARAM;
    }

    /* firstly, check the voq_id is processed or not */
    sw_entry_id = pfcdm_voq_id_map_table[voq_id];
    if (sw_entry_id < PFCDM_VOS_CONGEST_MON_TABLE_SIZE)
    {
		entry_ptr = &pfcdm_congest_mon_table[sw_entry_id];
        entry_ptr->congest_trigger_flag = 1; /* the congest is triggered */
        entry_ptr->congest_trigger_cnt ++;

        return BCM_E_NONE;
    }

    /* allocate a free entry */
    rv = pfcdm_congest_mon_sw_entry_alloc(&sw_entry_id);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }
	
    /* get out_port and priority of this voq */
    rv = pfcdm_voq_id_output_pri_map(unit, core_id, voq_id, &out_port, &pri);
    if (rv != BCM_E_NONE) 
    {
        printf("Congestion Mon voq_id %d mapping to port failure.\n", voq_id);
        return rv;
    }

    /* install the TCAM */
    rv = pfcdm_field_ipmf3_congest_mon_entry_add(unit, out_port, pri, &hw_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("Congest Mon TCAM install failure (voq_id %d, out_port %d, pri %d).\n", voq_id, out_port, pri);
        return rv;
    }
	
    /* update the software table */
    entry_ptr = &pfcdm_congest_mon_table[sw_entry_id];
    entry_ptr->congest_trigger_flag = 1;
    entry_ptr->congest_trigger_cnt ++;
    entry_ptr->voq_out_port = out_port;
    entry_ptr->voq_priority = pri;
    entry_ptr->hw_entry_id = hw_entry_id;
    entry_ptr->voq_id = voq_id;
    entry_ptr->valid = 1;
	
    pfcdm_voq_id_map_table[voq_id] = sw_entry_id;
    return BCM_E_NONE;
}



int pfcdm_congest_mon_aging_process(int unit, uint16_t sw_entry_id)
{
    int rv = 0;
    pfcdm_voq_congest_mon_entry_t *entry_ptr = NULL;
	
    entry_ptr = &pfcdm_congest_mon_table[sw_entry_id];
    if (0 == entry_ptr->valid)
    {
        return BCM_E_NONE;
	}
	
    /* delete TCAM entry */
    rv = pfcdm_field_ipmf3_congest_mon_entry_del(unit, entry_ptr->hw_entry_id);
    if (rv != BCM_E_NONE)
    {
        printf("failed to delete HW entry\n");
        return rv;
    }

    /* set the voq mapping invalid */
    pfcdm_voq_id_map_table[entry_ptr->voq_id] = PFCDM_INVALID_ENTRY_ID;
    sal_memset(entry_ptr, 0, sizeof(*entry_ptr));
	
    return BCM_E_NONE;
}



void pfcdm_cong_mon_cb(
    int unit,
    bcm_switch_event_t event,
    uint32 arg1,
    uint32 arg2,
    uint32 arg3,
    void *userdata)
{
    printf("event %d\n", event);

    return;
}



int pfcdm_congest_mon_interrupt_log_defaults_set(int unit)
{
    int rv = BCM_E_NONE;
    bcm_switch_event_control_t event;

    event.action = bcmSwitchEventLog;
    event.index = BCM_CORE_ALL;
    event.event_id = pfcdm_cong_mon_event_id;

    rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0x1);

    return rv;
}



int
pfcdm_congest_mon_interrupt_init(
    int unit)
{
    int rc;
    bcm_switch_event_control_t bcm_switch_event_control;

    sal_memset((void *) &bcm_switch_event_control, 0, sizeof(bcm_switch_event_control));

/*
    rc = bcm_switch_event_register(unit, pfcdm_cong_mon_cb, NULL);
    if (rc != BCM_E_NONE)
    {
        printf("bcm_switch_event_register failure.\n");
        return rc;
	}
*/

    rc = pfcdm_congest_mon_interrupt_log_defaults_set(unit);
    if (rc != BCM_E_NONE)
    {
        printf("bcm_switch_event_register failure.\n");
        return rc;
	}

    /*
     * Enable interrupts
     */
    bcm_switch_event_control.action = bcmSwitchEventMask;
    bcm_switch_event_control.index = BCM_CORE_ALL;
    bcm_switch_event_control.event_id = pfcdm_cong_mon_event_id;
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, bcm_switch_event_control, 0x0);

    return rc;
}


int pfcdm_congest_mont_init(void)
{
    int index = 0;
	
    for (index = 0; index < PFCDM_VOQ_NUM; index ++) 
    {
        pfcdm_voq_id_map_table[index] = PFCDM_INVALID_ENTRY_ID;
    }

    sal_memset(pfcdm_congest_mon_table, 0, sizeof(pfcdm_congest_mon_table));
	
    return BCM_E_NONE;
}
