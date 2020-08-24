
/*
## VoQ Threshold ##
mod CGM_VOQ_CONGESTION_PRMS 0 1 VOQ_WORDS_FADT_MIN_TH=100 VOQ_WORDS_FADT_MAX_TH=100 VOQ_WORDS_FADT_ADJUST_FACTOR=0
*/
struct pfcdm_packet_parse_offset_t{
    uint8_t udf_drop_reason_offset;
    uint8_t udf_hash_result_offset;
    uint8_t payload_start_offset;
};

pfcdm_packet_parse_offset_t pfcdm_packet_parse_offset[3] = {
    /* first type: snoop packet */
	{68, 72, 76},
    /* second type: TTL == 0 */
    {38, 42, 46},
    /* third type: unkown dest */
    {35, 39, 43}
};

pfcdm_flow_root_entry_t pfcdm_flow_root_table[PFCDM_FLOW_ROOT_TABLE_SIZE];
pfcdm_flow_list_entry_t pfcdm_flow_list_table[PFCDM_GLOBAL_FLOW_LIST_TABLE_SIZE];

const uint16_t VLAN_TPID  = 0x8100;
const uint16_t IPV4_ETHTYPE    = 0x0800;
const uint16_t VLAN_TPID_OFFSET_BYTE = 12;

/* key information, index table in this case */
pfcdm_key_info_t pfcdm_key_infos[5] = {
    {bcmFieldQualifySrcIp, 0xFFFFFFFF, bcmFieldDataOffsetBaseOuterL3Header, 12, pfcdmFieldTypeWord}, /* pfcdmKeyIPv4Sip */
    {bcmFieldQualifyDstIp, 0xFFFFFFFF, bcmFieldDataOffsetBaseOuterL3Header, 16, pfcdmFieldTypeWord}, /* pfcdmKeyIPv4Dip */	
	{bcmFieldQualifyIpProtocol, 0xFF, bcmFieldDataOffsetBaseOuterL3Header, 9, pfcdmFieldTypeByte}, /* pfcdmKeyIPv4Protocol */
    {bcmFieldQualifyL4SrcPort, 0xFFFF, bcmFieldDataOffsetBaseOuterL3Header, 20, pfcdmFieldTypeHalfword}, /* pfcdmKeyIPv4SPort */
    {bcmFieldQualifyL4DstPort, 0xFFFF, bcmFieldDataOffsetBaseOuterL3Header, 22, pfcdmFieldTypeHalfword} /* pfcdmKeyIPv4DPort */
};

/* key template */
/* the key order should be same to iPMF configuration */
pfcdm_key_template_t pfcdm_key_templates[1] = { 
    {
        5, /* key amount*/
        {pfcdmKeyIPv4Sip, pfcdmKeyIPv4Dip, pfcdmKeyIPv4Protocol, pfcdmKeyIPv4SPort, pfcdmKeyIPv4DPort}
	}
};

/* global group table */
pfcdm_group_table_entry_t pfcdm_group_table[PFCDM_MAX_GROUP_NUM] = {
    { /* group 0 */
        /* key template id */
        0, 
        /* flow age counter configured */		
        3000,
        /* flow count limit */
        200,
        /* flow count limit learn */
        150,
        /* flow count learn */
        0,
		/* flow count static configure */
		0
    } /* end group 0 */
};


uint32_t pfcdm_field_byte_parse(uint8_t *base, uint16_t offset)
{
    uint32_t result = base[offset];
	
    return result;
}

uint32_t pfcdm_field_halfword_parse(uint8_t *base, uint16_t offset)
{
    uint32_t result = (base[offset] << 8) | (base[offset + 1]);
		
    return result;
}

uint32_t pfcdm_field_word_parse(uint8_t *base, uint16_t offset)
{
    uint32_t result = (base[offset] << 24) | (base[offset + 1] << 16) | (base[offset + 2] << 8) | (base[offset + 3]);

    return result;
}



/* could define as function array */
uint32_t pfcdm_field_parse(uint8_t field_type, uint8_t *base, uint16_t offset)
{
    uint32_t result = 0;
    if (pfcdmFieldTypeByte == field_type)
    {
        result = pfcdm_field_byte_parse(base, offset);
    }
    else if (pfcdmFieldTypeHalfword == field_type)
    {
        result = pfcdm_field_halfword_parse(base, offset);
    }
    else if (pfcdmFieldTypeWord == field_type)
    {
        result = pfcdm_field_word_parse(base, offset);
    }

    return result;
}

void pfcdf_key_template_parse(uint8_t *data, uint8_t key_template_id, uint32_t *key_container)
{
    uint8_t i = 0;
    pfcdm_key_template_t * key_templates = &pfcdm_key_templates[key_template_id];
    pfcdm_key_info_t *key_info = NULL; 
    uint8_t key_type = 0;
	
    for (i = 0; i < key_templates->key_amount; i ++)
    {
        key_type = key_templates->key_array[i];
        key_info = &pfcdm_key_infos[key_type];
        key_container[i] = pfcdm_field_parse(key_info->field_type, data, key_info->offset);
    }
}



int pfcdf_ipv4_5tuples_parse(uint8_t *data, uint8_t key_template_id, pfcdm_ipv4_5tuples_t  *ipv4_5tuples)
{
    uint32_t key_container[PFCDM_KEY_TEMPLATE_KEY_NUM_MAX];

    pfcdf_key_template_parse(data, key_template_id, key_container);
    ipv4_5tuples->protocol = key_container[pfcdmKeyIPv4Protocol];
    ipv4_5tuples->dip = key_container[pfcdmKeyIPv4Dip];
    ipv4_5tuples->sip = key_container[pfcdmKeyIPv4Sip];
    ipv4_5tuples->dport = key_container[pfcdmKeyIPv4DPort];
    ipv4_5tuples->sport = key_container[pfcdmKeyIPv4SPort];

    return BCM_E_NONE;
}



int pfcdm_flow_list_table_alloc(uint32_t *entry_id)
{
    int rv = BCM_E_FULL;
    int i = 0;

    pfcdm_flow_entry_t *flow_entry = NULL;
    pfcdm_flow_list_entry_t *flow_list_entry = NULL; 
	
    for (i = 0; i < PFCDM_GLOBAL_FLOW_LIST_TABLE_SIZE; i ++)
    {
        flow_list_entry = &pfcdm_flow_list_table[i];
        flow_entry = &flow_list_entry->flow_entry;
        if (0 == flow_entry->flag)
        {
            break;
		}
    }
    if (i < PFCDM_GLOBAL_FLOW_LIST_TABLE_SIZE)
    {
        *entry_id = i;
        rv = BCM_E_NONE;
    }

    return rv;
}



int pfcdm_flow_root_table_lookup(uint32_t key, uint32_t *entry_id)
{
    int rv = BCM_E_FULL;
    int i = 0;
    uint32_t free_entry_id = PFCDM_INVALID_ENTRY_ID;

    pfcdm_flow_entry_t *flow_entry = NULL;
    pfcdm_flow_root_entry_t *flow_root_entry = NULL; 
	
    for (i = 0; i < PFCDM_FLOW_ROOT_TABLE_SIZE; i ++)
    {
        flow_root_entry = &pfcdm_flow_root_table[i];
        flow_entry = &flow_root_entry->flow_entry;
        if ((PFCDM_INVALID_ENTRY_ID == free_entry_id) && (0 == flow_entry->flag))
        {
            free_entry_id = i;
        }
		
        /* find out it */
        if (flow_root_entry->em_key == key)
        {
            break;	
        }
    }
    if (i < PFCDM_FLOW_ROOT_TABLE_SIZE)
    {
        *entry_id = i;
        rv = BCM_E_EXISTS;
    }
    else if (free_entry_id < PFCDM_FLOW_ROOT_TABLE_SIZE)
    {
        *entry_id = free_entry_id;
        rv = BCM_E_NOT_FOUND;
	}
    
    return rv;
}


int pfcdm_flow_list_lookup(uint32_t head_entry_id,
    pfcdm_ipv4_5tuples_t  *ipv4_5tuples, uint32_t *cur_entry_id)
{
    int rv = BCM_E_NOT_FOUND;
    uint32_t entry_id = PFCDM_INVALID_ENTRY_ID;

    pfcdm_flow_list_entry_t *flow_list_entry = NULL;
    pfcdm_flow_entry_t *flow_entry = NULL;

    if (PFCDM_INVALID_ENTRY_ID == head_entry_id)
    {
        return BCM_E_NOT_FOUND;
	}

    entry_id = head_entry_id;
    while (entry_id != PFCDM_INVALID_ENTRY_ID)
    {
        flow_list_entry = &pfcdm_flow_list_table[head_entry_id];
        flow_entry = &flow_list_entry->flow_entry;
        if ((flow_entry->key.protocol == ipv4_5tuples->protocol) 
            && (flow_entry->key.dip == ipv4_5tuples->dip)
            && (flow_entry->key.sip == ipv4_5tuples->sip)
            && (flow_entry->key.dport == ipv4_5tuples->dport)
            && (flow_entry->key.sport == ipv4_5tuples->sport))
        {
            break;
		}
		
		entry_id = flow_list_entry->next_entry_id;
	}
    if (entry_id != PFCDM_INVALID_ENTRY_ID)
    {
        *cur_entry_id = entry_id;
        rv = BCM_E_EXISTS;
	}

    return rv;
}



static uint32_t pfcdm_local_flow_idx_gen(int flag, uint32_t entry_id)
{
    return (((flag & 0xf) << 28) | (entry_id & 0xfffffff));
}


static void pfcdm_drop_bitmap_set(uint32_t *sw_drop_reason_bitmap, uint32_t drop_reason)
{
    uint16_t base = drop_reason / 32;
    uint32_t offset = drop_reason & 0x1f;
	
    sw_drop_reason_bitmap[base] |=  (1 << offset);
}


static void pfcdm_flow_new_entry_fill(pfcdm_flow_entry_t *flow_entry, 
    int flag, uint32_t stat_id, pfcdm_ipv4_5tuples_t *ipv4_5tuple, 
    uint32_t group_id, uint32_t drop_reason)
{
    flow_entry->flag = flag;
    flow_entry->stat_id = stat_id;
    flow_entry->flow_state = PFCDM_FLOW_STATE_DROP_START;
    flow_entry->key.protocol = ipv4_5tuple->protocol;
    flow_entry->key.dip = ipv4_5tuple->dip;
    flow_entry->key.sip = ipv4_5tuple->sip;
    flow_entry->key.dport = ipv4_5tuple->dport;
    flow_entry->key.sport = ipv4_5tuple->sport;
    flow_entry->group_id = group_id;
    pfcdm_drop_bitmap_set(flow_entry->sw_drop_reason_bitmap, drop_reason);
    flow_entry->sw_last_drop_reason = drop_reason;

    printf("Flow dip 0x%08x, sip 0x%08x, dport %d, sport %d, protocol %d first installed. drop_reason = %d\n", 
       ipv4_5tuple->dip, ipv4_5tuple->sip, ipv4_5tuple->dport, ipv4_5tuple->sport, ipv4_5tuple->protocol, drop_reason);
}


static int pfcdm_flow_entry_update(uint32_t local_flow_idx, uint32_t drop_reason)
{
    uint8_t flag = (local_flow_idx >> 28) & 0xf;
    uint32_t entry_id = local_flow_idx & 0xfffffff; 
    pfcdm_flow_entry_t *flow_entry = NULL;

    int rv = BCM_E_NONE;
    if (PFCDM_FLOW_IN_EXEM3 == flag)
    {
        flow_entry = &pfcdm_flow_root_table[entry_id].flow_entry;
	}
    else if (PFCDM_FLOW_IN_TCAM == flag)
	{
        flow_entry = &pfcdm_flow_list_table[entry_id].flow_entry;
    }
    else
    {
        rv = BCM_E_PARA;
    }
	
    pfcdm_drop_bitmap_set(flow_entry->sw_drop_reason_bitmap, drop_reason);

    printf("Flow entry 0x%08x drop reason update. old = %d, new = %d\n", local_flow_idx, flow_entry->sw_last_drop_reason, drop_reason);
    flow_entry->sw_last_drop_reason = drop_reason;

    return rv;
}


int pfcdm_field_flow_entry_install(int unit, 
    int is_update,
    int pmf_flag, 
    uint32_t em_key, 
    pfcdm_ipv4_5tuples_t  *ipv4_5tuples, 
    uint8_t sw_drop_reaon, 
    uint32_t stat_id, 
    bcm_field_entry_t *entry_id_ptr)
{
    int rv = BCM_E_NONE;
    uint16_t hash_result;
    uint16_t other_key;
    
    if (PFCDM_FLOW_IN_EXEM3 == pmf_flag)
    {
        hash_result = em_key & 0xffff;
        other_key = (em_key >> 16) & 0xffff;

        rv = pfcdm_field_ipv4_flow_entry_em_add(unit, is_update, hash_result, other_key, sw_drop_reaon, stat_id);
	}
    else
    {
        rv = pfcdm_field_ipv4_flow_entry_tcam_add(unit, is_update,  
            ipv4_5tuples->sip, ipv4_5tuples->dip, ipv4_5tuples->protocol, ipv4_5tuples->sport, ipv4_5tuples->dport, 
            sw_drop_reaon, stat_id, entry_id_ptr);
	}

    return rv;
}



/* EM key includes the hash result of 5-tuples */
static int pfcdm_flow_install_common_process(int unit, uint32_t flag, uint32_t group_id, pfcdm_ipv4_5tuples_t  *ipv4_5tuples, 
    uint32_t em_key, uint32_t drop_reason,
    uint32_t *local_flow_idx)
{
    int rv = BCM_E_NONE;
    uint32_t flow_root_entry_id = 0;
    uint32_t flow_list_entry_id = 0;
    uint32_t stat_id = 0;
    uint32_t tmp_flow_idx = 0;
    bcm_field_entry_t pmf_entry_id = 0;

    pfcdm_flow_root_entry_t *flow_root_entry = NULL; 
    pfcdm_flow_list_entry_t *flow_list_entry = NULL; 
    pfcdm_flow_list_entry_t *flow_list_entry_next = NULL; 
    pfcdm_flow_entry_t *flow_em_entry = NULL;
    pfcdm_flow_entry_t *flow_tcam_entry = NULL;
	
    /* lookup flow root table to see whether EM key exists */
	rv = pfcdm_flow_root_table_lookup(em_key, &flow_root_entry_id);
    if (BCM_E_FULL == rv)
    {
        printf("No Free entry could be allocated.\n");
        return rv;
    }
    
    if (BCM_E_EXISTS == rv)
    {
        flow_root_entry = &pfcdm_flow_root_table[flow_root_entry_id];
        flow_em_entry = &flow_root_entry->flow_entry;
        if ((flow_em_entry->key.protocol == ipv4_5tuples->protocol) 
            && (flow_em_entry->key.dip == ipv4_5tuples->dip)
            && (flow_em_entry->key.sip == ipv4_5tuples->sip)
            && (flow_em_entry->key.dport == ipv4_5tuples->dport)
            && (flow_em_entry->key.sport == ipv4_5tuples->sport))
        { /* saved in root table */
            *local_flow_idx = pfcdm_local_flow_idx_gen(PFCDM_FLOW_IN_EXEM3, flow_root_entry_id);

            if (flow_em_entry->sw_last_drop_reason != drop_reason)
	        {
                /* update this flow to S-EXEM3 */
                rv = pfcdm_field_flow_entry_install(unit, 1, 
                    PFCDM_FLOW_IN_EXEM3, em_key, ipv4_5tuples, drop_reason, flow_em_entry->stat_id, &pmf_entry_id);
                if (rv != BCM_E_NONE)
                {
                    printf("Install EXEM flow %d failure\n", flow_root_entry_id);
                    return rv;
                }
                
                pfcdm_flow_entry_update(*local_flow_idx, drop_reason);
            }

            return rv;
        }
        else /* find list table */
        {
            rv = pfcdm_flow_list_lookup(flow_root_entry->flow_list_head, 
                ipv4_5tuples, 
                &flow_list_entry_id);
            if (BCM_E_EXISTS == rv)
            {
                *local_flow_idx = pfcdm_local_flow_idx_gen(PFCDM_FLOW_IN_TCAM, flow_list_entry_id);

                flow_list_entry = &pfcdm_flow_list_table[flow_list_entry_id];
                flow_tcam_entry = &flow_list_entry->flow_entry;
                if (flow_tcam_entry->sw_last_drop_reason != drop_reason)
	            {					
                    /* update this flow to S-TCAM */
                    rv = pfcdm_field_flow_entry_install(unit, 1, 
                        PFCDM_FLOW_IN_TCAM, em_key, ipv4_5tuples, drop_reason, flow_tcam_entry->stat_id, &flow_list_entry->tcam_entry_id);
                    if (rv != BCM_E_NONE)
                    {
                        printf("Install EXEM flow %d failure\n", flow_root_entry_id);
                        return rv;
                    }
                
                    pfcdm_flow_entry_update(*local_flow_idx, drop_reason);
                }

                return rv;
            } 
            else /* not exist in list table as well */
            {
                /* allocate an new entry in list table */
                rv = pfcdm_flow_list_table_alloc(&flow_list_entry_id);
                if (rv != BCM_E_NONE)
                {
                    printf("pfcdm_flow_list_table_alloc failure.\n");
                    return rv;
				}
                
                /* allocate stat.id for this entry */
                tmp_flow_idx = pfcdm_local_flow_idx_gen(PFCDM_FLOW_IN_TCAM, flow_list_entry_id);
                rv = pfcdm_stat_id_alloc(tmp_flow_idx, &stat_id);
                if (rv != BCM_E_NONE)
                {
                    printf("PFCDM_FLOW_IN_TCAM alloc counter failure tmp_flow_idx %d stat_id %d flow_list_entry_id %d\n", tmp_flow_idx, stat_id, flow_list_entry_id);
                    return rv;
		        }

                /* install this flow to TCAM */
                rv = pfcdm_field_flow_entry_install(unit, 0, PFCDM_FLOW_IN_TCAM, em_key, ipv4_5tuples, drop_reason, stat_id, &pmf_entry_id);
                if (rv != BCM_E_NONE)
                {
                    printf("Install TCAM flow %d failure\n", flow_list_entry_id);
                    return rv;
		        }	
			
                /* save the software table */
                flow_list_entry = &pfcdm_flow_list_table[flow_list_entry_id];
                flow_tcam_entry = &flow_list_entry->flow_entry;
				
                /* the list is not empty, insert this entry into the head */
                if (PFCDM_INVALID_ENTRY_ID != flow_root_entry->flow_list_head)
                {
                    flow_list_entry_next = &pfcdm_flow_list_table[flow_root_entry->flow_list_head];
                    flow_list_entry->next_entry_id = flow_root_entry->flow_list_head;
                    flow_list_entry_next->prev_entry_id = flow_list_entry_id;
				}
                else
                {
                    flow_list_entry->next_entry_id = PFCDM_INVALID_ENTRY_ID;					
				}

                flow_list_entry->prev_entry_id = PFCDM_INVALID_ENTRY_ID;
				
                flow_root_entry->flow_list_head = flow_list_entry_id;
                flow_root_entry->flow_count ++;

                /* fill this entry */
                flow_list_entry->tcam_entry_id = pmf_entry_id;
                pfcdm_flow_new_entry_fill(flow_tcam_entry, flag, stat_id, ipv4_5tuples, group_id, drop_reason);
				tmp_flow_idx = pfcdm_local_flow_idx_gen(PFCDM_FLOW_IN_TCAM, flow_list_entry_id);
                *local_flow_idx = tmp_flow_idx;	
            }
        }
    }
    else /* new entry */
    {
        /* allocate stat. id for this entry */
        tmp_flow_idx = pfcdm_local_flow_idx_gen(PFCDM_FLOW_IN_EXEM3, flow_root_entry_id);
        rv = pfcdm_stat_id_alloc(tmp_flow_idx, &stat_id);
        if (rv != BCM_E_NONE)
        {
            printf("PFCDM_FLOW_IN_EXEM3 alloc counter failure tmp_flow_idx %d stat_id %d flow_root_entry_id %d\n", tmp_flow_idx, stat_id, flow_root_entry_id);
            return rv;
		}
		
        /* install this flow to S-EXEM3 */
        rv = pfcdm_field_flow_entry_install(unit, 0, PFCDM_FLOW_IN_EXEM3, em_key, ipv4_5tuples, drop_reason, stat_id, &pmf_entry_id);
        if (rv != BCM_E_NONE)
        {
            printf("Install EXEM flow %d failure\n", flow_root_entry_id);
            return rv;
        }

        /* save the software table */
        flow_root_entry = &pfcdm_flow_root_table[flow_root_entry_id];
        flow_em_entry = &flow_root_entry->flow_entry;

        flow_root_entry->em_key = em_key;
        flow_root_entry->flow_list_head = PFCDM_INVALID_ENTRY_ID;
        flow_root_entry->flow_count = 1;
		
        pfcdm_flow_new_entry_fill(flow_em_entry, flag, stat_id, ipv4_5tuples, group_id, drop_reason);
        *local_flow_idx = tmp_flow_idx;
	}

    return rv;
}



int pfcdm_flow_install_dynm_process(int unit, uint32_t group_id, pfcdm_ipv4_5tuples_t  *ipv4_5tuples, 
    uint32_t em_key, uint32_t drop_reason)
{
    int rv = BCM_E_NONE;
    uint32_t local_flow_idx = 0;
    rv = pfcdm_flow_install_common_process(unit, PFCDM_FLOW_ENTRY_DYNMICAL, group_id, ipv4_5tuples, em_key, drop_reason, &local_flow_idx);
	
    return rv;
}



int pfcdm_flow_static_install(int unit, uint32_t group_id, pfcdm_ipv4_5tuples_t  *ipv4_5tuples, 
    uint32_t em_key)
{
    int rv = BCM_E_NONE;
    uint32_t local_flow_idx = 0;	
    rv = pfcdm_flow_install_common_process(unit, PFCDM_FLOW_ENTRY_STATIC, group_id, ipv4_5tuples, em_key, 0, &local_flow_idx);
	
    return rv;
}



int pfcdm_flow_static_debug(int unit)
{
	uint32_t group_id = 0;
    pfcdm_ipv4_5tuples_t  ipv4_5tuples; 
    uint32_t em_key = 0;
	
    /* dummy, to test TCAM */
    ipv4_5tuples.protocol = 0x11;
    ipv4_5tuples.dip = 0x7fffff02;
    ipv4_5tuples.sip = 0xc0a80102;
    ipv4_5tuples.dport = 200;
    ipv4_5tuples.sport = 32;
	
    em_key = 0xff029493;
    pfcdm_flow_static_install(unit, group_id, &ipv4_5tuples, em_key);

    ipv4_5tuples.protocol = 0x11;
    ipv4_5tuples.dip = 0x7fffff02;
    ipv4_5tuples.sip = 0xc0a80102;
    ipv4_5tuples.dport = 100;
    ipv4_5tuples.sport = 33;
	
    em_key = 0xff029713;
    pfcdm_flow_static_install(unit, group_id, &ipv4_5tuples, em_key);

    ipv4_5tuples.protocol = 0x11;
    ipv4_5tuples.dip = 0x7fffff02;
    ipv4_5tuples.sip = 0xc0a80102;
    ipv4_5tuples.dport = 100;
    ipv4_5tuples.sport = 34;
	
    em_key = 0xff029553;
    pfcdm_flow_static_install(unit, group_id, &ipv4_5tuples, em_key);

    ipv4_5tuples.protocol = 0x11;
    ipv4_5tuples.dip = 0x7fffff02;
    ipv4_5tuples.sip = 0xc0a80102;
    ipv4_5tuples.dport = 100;
    ipv4_5tuples.sport = 35;
	
    em_key = 0xff0296d3;
    pfcdm_flow_static_install(unit, group_id, &ipv4_5tuples, em_key);	

    ipv4_5tuples.protocol = 0x11;
    ipv4_5tuples.dip = 0x7fffff02;
    ipv4_5tuples.sip = 0xc0a80102;
    ipv4_5tuples.dport = 100;
    ipv4_5tuples.sport = 36;
	
    em_key = 0xff029473;
    pfcdm_flow_static_install(unit, group_id, &ipv4_5tuples, em_key);	
}



int pfcdm_flow_install_dynm_process_debug(int unit, uint32_t drop_reason)
{
    int rv = BCM_E_NONE;
    uint32_t group_id = 0;
    pfcdm_ipv4_5tuples_t  ipv4_5tuples;

    ipv4_5tuples.protocol = 0x11;
    ipv4_5tuples.dip = 0x7fffff02;
    ipv4_5tuples.sip = 0xc0a80102;
    ipv4_5tuples.dport = 63;
    ipv4_5tuples.sport = 100;
	
    uint32_t em_key = 0xFF029fce;
	
    rv = pfcdm_flow_install_dynm_process(unit, group_id, &ipv4_5tuples, em_key, drop_reason);
    print rv;
    return rv;
}

int pfcdm_flow_aging_process(int unit, uint16_t stat_id)
{
	

}

/*
 * PFCDM Rx function
 *
 * This function will be executed within the context of the RX Thread.
 *
 */
bcm_rx_t pfcdm_rx_callback(int unit, bcm_pkt_t* pkt, void* cookie)
{
    int idx = 0;
    bcm_pkt_dnx_internal_t *internal_hdr = NULL;
    uint8_t *data = pkt->_pkt_data.data;
    uint16_t tpid = 0;
    uint16_t eth_type = 0;
    uint16_t ip_base_offset = VLAN_TPID_OFFSET_BYTE;
    uint32_t sip = 0;
    uint32_t dip = 0;
    uint8_t protocal = 0;
    uint16_t sport = 0;
    uint16_t dport = 0;
    uint8_t group_id = 0;
    uint8_t sw_drop_reason = 0;
    pfcdm_ipv4_5tuples_t ipv4_5tuples;

    uint16_t udf_data2_offset = 0; /* UDH offset */	
    uint32_t udf_data2 = 0;
    uint16_t udf_data3_offset = 0; /* UDH offset */	
    uint32_t udf_data3 = 0;	
    uint16_t hdr_len = 0 /* pkt->tot_len - pkt->pkt_len */;

    pfcdm_packet_parse_offset_t *pfcdm_packet_parse_offset_ptr = NULL;

    int valid_packet = 0;
	
	
    /* dnx packet 
    typedef struct bcm_pkt_dnx_s {
        bcm_pkt_dnx_type_t type;            # DNX Header type 
        bcm_pkt_dnx_itmh_t itmh;            # ITMH Header 
        bcm_pkt_dnx_ftmh_t ftmh;            # FTMH Header
        bcm_pkt_dnx_otsh_t otsh;            # OAM-TS Header (OTSH) 
        bcm_pkt_dnx_otmh_t otmh;            # OTMH Header 
        bcm_pkt_dnx_internal_t internal;    # Internal Header
    } bcm_pkt_dnx_t;
    */
    for (idx = 0; idx < pkt->dnx_header_count; idx ++)
    {
        /* print pkt->dnx_header_stack[idx].type; */
        /* print pkt->dnx_header_stack[idx].ftmh; */
        /* print pkt->dnx_header_stack[idx].itmh; */
        /* print pkt->dnx_header_stack[idx].internal; */
		
        if (bcmPktDnxTypeInternals == pkt->dnx_header_stack[idx].type)
        {
            internal_hdr = &pkt->dnx_header_stack[idx].internal;
            printf("%d, trap id %d, trap qualifer %d\n", idx, internal_hdr->trap_id, internal_hdr->trap_qualifier);
            valid_packet = 1;
            break;
        }
    }
    if (!valid_packet)
    {
        printf("NOT OWN packet\n");
        return BCM_RX_NOT_HANDLED;
    }

    /* congestion monitor packets */
    if ((482 == internal_hdr->trap_id) || (51 == internal_hdr->trap_qualifier))
    {
        pfcdm_packet_parse_offset_ptr = &pfcdm_packet_parse_offset[0];
	}
    else if (0x02 == data[28])
    {
        pfcdm_packet_parse_offset_ptr = &pfcdm_packet_parse_offset[1];
    }
    else if (0x03 == data[28])
    {
        pfcdm_packet_parse_offset_ptr = &pfcdm_packet_parse_offset[2];
    }
    else
    {
        printf("UNKNOWN packet.\n");
        return BCM_RX_NOT_HANDLED;
    }

    udf_data2_offset = pfcdm_packet_parse_offset_ptr->udf_drop_reason_offset;
    udf_data3_offset = pfcdm_packet_parse_offset_ptr->udf_hash_result_offset;
    hdr_len = pfcdm_packet_parse_offset_ptr->payload_start_offset;

    /* the payload start address */
    data += hdr_len;

    tpid = pfcdm_field_halfword_parse(data, ip_base_offset);
    /* printf("tpid %04x %04x\n", tpid, VLAN_TPID); */
    if (VLAN_TPID == tpid)
    {
        ip_base_offset = ip_base_offset + 4;
        /* printf("ip_base_offset %d\n", ip_base_offset); */
        eth_type = pfcdm_field_halfword_parse(data, ip_base_offset);
    }
    else
    { 
        eth_type = tpid;
    }

    if (eth_type != IPV4_ETHTYPE)
    {
        printf("NOT IPv4, ether_type 0x%04x\n", eth_type);
        return BCM_RX_NOT_HANDLED;
    }
    else
    {
        group_id = 0; /* IPv4 5-tuples */
    }

    /* IP header base */
    data = data + ip_base_offset + 2;

    /* set the initialization value */
    sal_memset(&ipv4_5tuples, 0, sizeof(ipv4_5tuples));
    pfcdf_ipv4_5tuples_parse(data, pfcdm_group_table[group_id].key_template_id, &ipv4_5tuples);
    /* printf("Pro %d, dip 0x%08x, sip %0x, dport %d, sport %d\n", 
        ipv4_5tuples.protocol, ipv4_5tuples.dip, ipv4_5tuples.sip, ipv4_5tuples.dport, ipv4_5tuples.sport) ; */
	
    /* UDH data */
    udf_data2 = pfcdm_field_word_parse(pkt->_pkt_data.data, udf_data2_offset);
    udf_data3 = pfcdm_field_word_parse(pkt->_pkt_data.data, udf_data3_offset);

    printf(" EM Key 0x%08x, HW drop code %d\n", udf_data3, udf_data2); 

    /* map Hardware drop reason to software drop reason, just for the reference codes used */
    if (PFCDM_HW_DO_NOTHING_TRAP_CODE == udf_data2)
	{
        sw_drop_reason = pfcdm_congest_sw_drop_reason;        
    }
    else if (bcmRxTrapForwardingIpv4Ttl0_hw_code == udf_data2)
    {
        sw_drop_reason = bcmRxTrapForwardingIpv4Ttl0_sw_drop_reason;
    }
    else if (bcmRxTrapUnknowDest_hw_code == udf_data2)
    {
        sw_drop_reason = bcmRxTrapUnknowDest_sw_drop_reason;
    }
    else
    {
        printf("Unknown HW drop reason %d\n", udf_data2);
        return BCM_RX_HANDLED_OWNED;
    }

    pfcdm_flow_install_dynm_process(unit, group_id, &ipv4_5tuples, udf_data3, sw_drop_reason);
    return BCM_RX_HANDLED_OWNED;
}



int
pfcdm_rx_start(uint32 unit)
{
    int rv;
    /* RX Configuration Structure */
    bcm_rx_cfg_t cfg;

    /* Initialize the BCM RX API */
    if (bcm_rx_active(unit)) {
        print("Stopping active RX.\n");
        rv = bcm_rx_stop(unit, NULL);
        if (rv != BCM_E_NONE) {
            printf("Unable to stop RX: %d\n", rv);
            return rv;
        }
    }

    /*
     * Setup our RX Configuration.
     * The following values are global settings for all of the RX API.
     */
    bcm_rx_cfg_t_init(&cfg);

    /*
     * Maximum packet buffer size. Packets beyond this size will require
     * scatter gather, and be delivered in multiple data buffers which
     * the application must the reconstruct.
     */
    cfg.pkt_size = 16*1024;

    /*
     * Number of packet buffers per DMA descriptor chain. This determines
     * how many packets the hardware can DMA into before the buffers have
     * to be refilled by software.
     */
    cfg.pkts_per_chain = 15 ;

    /*
     * This sets the global rate of packets sent to the CPU by all RX channels.
     * A value of 0 disables global rate limiting.
     */
    cfg.global_pps = 100; /* 100 pps for reference */

    /*
     * Maximum burst size to the CPU, in packets. Must be less than or equal
     * to cfg.pkts_per_chain.
     */
    cfg.max_burst = 15;

    /*
     * Packets are received from the hardware in interrupt context.
     * The BCM RX API uses a separate thread to handle operations which
     * originate in interrupt context, but cannot be executed there.
     * These operations are scheduled with the RX thread as a deferred
     * procedure call from interrupt context.
     *
     * In addition to its own internal administration, the RX Thread is
     * used by the RX API to deliver packets to the application in
     * non-interrupt context for processing, if the application so
     * designates in its handler registration. (See RX Registration below).
     *
     * Packet buffers allocated for reception by the RX API are done through
     * user registered allocation routines.  The RX API provides default
     * allocation routines which use the BCM Configuration Manager's
     * DMA shared memory vectors. Setting these function vectors to NULL
     * indicate usage of the default allocation routines.
     */
    cfg.rx_alloc = NULL;
    cfg.rx_free = NULL;

    /*
     * These flags modify the behavior of packet reception. There are
     * currently two flags:
     *
     *  BCM_RX_F_IGNORE_HGHDR
     *      The RX API normally strips the HiGig header from
     *          packets received on XGS fabric chips, and sets the
     *          corresponding abstractions in the packet structure
     *          based on the higig information. If you specify this flags,
     *          the higig header will not be stripped from the packet,
     *          and instead will precede the packet data proper (DA.SA...)
     *
     *  BCM_RX_F_IGNORE_SL_TAG
     *      The RX API normally strips the SL Stack tag on Strata,
     *          Strata II, and XGS modules configured for SL stacking.
     *          This flag causes the SL Stack tag to remain in the packet.
     *
     *
     * This example does not use either of these flags.
     */
    cfg.flags = 0;

    /*
     * RX Channel Configuration.
     *
     * The Strata and StrataII family of chips support one RX Channel per chip.
     * The XGS Family of chips supports 4 RX Channels per chip, which may
     * be prioritized by COS.
     *
     * You may only specify one channel in this structure if operating
     * on Strata/Strata II devices.
     *
     * This example assumes an XGS device, and will configure 2 RX chains,
     * one for COS 0-3, one for COS 4-7.
     *
     *
     * Configure RX Channel 1   (Assumes channel 0 has been reserved for TX)
     */

    /*
     * This specifies the number of DV packet chains which will
     * be preallocated for use with this channel.
     *
     * Each chain is basically a queue of packets of length cfg.pkts_per_chain
     * which is setup with the DMA engine.
     * When one chain is full, the driver will queue up
     * another chain (subject to rate limiting).
     */
    cfg.chan_cfg[1].chains = 4; /* 4; */  /*1;      */

    /*
     * Flags:
     *
     * The following per-channel flags are supported:
     *
     *  BCM_RX_F_CRC_STRIP
     *      If specified, packets from this channel will have their
     *          CRC stripped before they are received.
     *  BCM_RX_F_VTAG_STRIP
     *      If specified, packets from this channel will have
     *          their VLAN tag stripped before they are received.
     *  BCM_RX_F_RATE_STALL
     *      Currently unimplemented.
     *
     *
     * This example does no use any of these per-channel flags.
     */
    cfg.chan_cfg[1].flags = 0;

    /*
     * COS Bitmap
     *
     * This value specifies which packet COS will be assigned to this
     * RX Channel.
     * COS mapping to RX channels must be unique.
     *
     * In this example, we want all packets to be assigned to this RX channel.
     * Thus, our COS bitmap will be 0xFF, for all 8 COS values.
     */
    cfg.chan_cfg[1].cos_bmp = 0xFffffffF;



    /*
     * Our RX Configuration is complete.
     * Our handlers are registered.
     * Lets start up packet reception.
     */

    if ((rv = bcm_rx_queue_register(unit, "RX NONINTR CALLBACK", BCM_RX_COS_ALL, pfcdm_rx_callback, 100, NULL,  BCM_RCO_F_ALL_COS)) < 0) {
        printf("bcm_rx_register pfcdm_rx_callback fail: %s \n", rv);
        return rv;
    }

    if ((rv = (bcm_rx_start(unit, &cfg))) < 0) {
        printf("bcm_rx_start fail: %s \n", rv);
        return rv;
    }

    /*
     * Things are running at this point. Packets should be flowing, and
     * handlers should be handling.
     */
    return 0;
}



/*
port loopback 13 mode=phy
port loopback 14 mode=phy

TTL = 0:
tx 1 pbm=eth13 data=0x000C000200000000000011118100000F08004500006600000000001179DBC0A801027FFFFF02003F00640052849C000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F40414243444546474849C7E8F955

TTL = 9:
tx 1 pbm=eth13 data=000C000200000000000011118100000F08004500006600000000091170DBC0A801027FFFFF02003F00640052849C000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F40414243444546474849F68E393

tx 5000 pbm=eth13 data=000C000200000000000011118100000F08004500006600000000091170DBC0A801027FFFFF02003F00640052849C000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F40414243444546474849F68E393


(1) unknown Dest first
02940006 80000000 00e0fb9e 08000000 0004040e 00020000 78000200 03000000 
00a50500 00000aff 02967b00 0c000200 00000000 00111181 00000f08 00450000 
66000000 00001179 dbc0a801 027fffff 02003f00 64005284 9c000102 03040506 
0708090a 0b0c0d0e 0f101112 13141516 1718191a 1b1c1d1e 1f202122 23242526 
2728292a 2b2c2d2e 2f303132 33343536 3738393a 3b3c3d3e 3f404142 43444546 
474849

trap id 10, trap qualifer 0


(2) unknown Dest sequence
02940006 80380000 00e0fb9e 08000000 0004040e 00020000 78000200 03000004 
5d750500 00000aff 02967b00 0c000200 00000000 00111181 00000f08 00450000 
66000000 00001179 dbc0a801 027fffff 02003f00 64005284 9c000102 03040506 
0708090a 0b0c0d0e 0f101112 13141516 1718191a 1b1c1d1e 1f202122 23242526 
2728292a 2b2c2d2e 2f303132 33343536 3738393a 3b3c3d3e 3f404142 43444546 
474849

trap id 471, trap qualifer 34

(3) TTL = 0 first

02a00006 80000006 43e0fb9e 08000000 0004040e 00020000 78000220 02000000 
06c50013 84050000 006cff02 967b000c 00020000 00000000 11118100 000f0800 
45000066 00000000 001179db c0a80102 7fffff02 003f0064 0052849c 00010203 
04050607 08090a0b 0c0d0e0f 10111213 14151617 18191a1b 1c1d1e1f 20212223 
24252627 28292a2b 2c2d2e2f 30313233 34353637 38393a3b 3c3d3e3f 40414243 
44454647 4849

trap id 108, trap qualifer 0

(4) TTL = 0 sequence

02a00006 80380006 43e0fb9e 08000000 0004040e 00020000 78000220 02000004 
5d750013 84050000 006cff02 967b000c 00020000 00000000 11118100 000f0800 
45000066 00000000 001179db c0a80102 7fffff02 003f0064 0052849c 00010203 
04050607 08090a0b 0c0d0e0f 10111213 14151617 18191a1b 1c1d1e1f 20212223 
24252627 28292a2b 2c2d2e2f 30313233 34353637 38393a3b 3c3d3e3f 40414243 
44454647 4849

trap id 471, trap qualifer 34


(5) TTL = 9, snoop, first
03180000 00100000 02e0fb9e 08000000 00000000 00000000 0007fa00 02000000 
1e250030 00000687 00000640 e0fb9e08 00000000 04040e00 02000078 00482002 
00138405 00000009 ff02967b 000c0002 00000000 00001111 8100000f 08004500 
00660000 00000911 70dbc0a8 01027fff ff02003f 00640052 849c0001 02030405 
06070809 0a0b0c0d 0e0f1011 12131415 16171819 1a1b1c1d 1e1f2021 22232425 
26272829 2a2b2c2d 2e2f3031 32333435 36373839 3a3b3c3d 3e3f4041 42434445 
46474849
trap id 482, trap qualifer 0


*/
