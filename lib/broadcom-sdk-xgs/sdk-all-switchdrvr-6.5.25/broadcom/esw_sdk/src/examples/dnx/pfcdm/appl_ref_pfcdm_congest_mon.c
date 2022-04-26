/** \file appl_ref_pfcdm_congest_mon.c
 * $Id$
 *
 * L@ application procedures for DNX. 
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/multicast.h>
#include <bcm/port.h>
#include <bcm/l2.h>
#include <bcm/switch.h>
#include <bcm/mirror.h>
#include <bcm/field.h>
#include <src/appl/reference/dnx/appl_ref_sys_device.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/sand/sand_aux_access.h>
#include <phymod_custom_config.h>
#include <appl/reference/sand/appl_ref_sand.h>

#define PFCDM_HW_DO_NOTHING_TRAP_CODE 9
#define PFCDM_INVALID_ENTRY_ID  (-1)
#define PFCDM_VOQ_NUM (64 * 1024) /* 64K */
#define PFCDM_VOS_CONGEST_MON_TABLE_SIZE (512)



typedef struct pfcdm_voq_congest_mon_entry_s{
    uint8_t valid;
    uint8_t congest_trigger_flag;
    uint16_t voq_out_port;
    uint16_t voq_priority;
    uint16_t voq_id;
    uint16_t congest_trigger_cnt; /* for debug */
    uint16_t age_cnt;
    uint32_t hw_entry_id;
}pfcdm_voq_congest_mon_entry_t;


/* this group id is fixed */
bcm_field_group_t pfcdm_congest_mon_fg_id = 25;

const int pfcdm_cong_mon_event_id = 874;
uint16_t pfcdm_voq_id_map_table[PFCDM_VOQ_NUM];
pfcdm_voq_congest_mon_entry_t pfcdm_congest_mon_table[PFCDM_VOS_CONGEST_MON_TABLE_SIZE];

bcm_gport_t pfcdm_snoop_gport_id = 0;

const int local_host_cpu = 0;

int
pfcdm_ingress_mirror_gport_get(int unit)
{
    bcm_mirror_destination_t dest;
    int int_ingress_mirror_profile = 2;
    int rv;
    int mirror_dest = local_host_cpu;
    bcm_mirror_header_info_t mirror_header_info;

    sal_memset(&dest, 0, sizeof(dest));
    bcm_mirror_destination_t_init(&dest);
	
    dest.flags = BCM_MIRROR_DEST_WITH_ID;
    dest.packet_copy_size = 256; /* Fixed value. */
	
    BCM_GPORT_LOCAL_SET(dest.gport, mirror_dest);
    BCM_GPORT_MIRROR_SET(dest.mirror_dest_id, int_ingress_mirror_profile);

    dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;

    rv = bcm_mirror_destination_create(unit, &dest);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mirror_destination_create\n");
        return rv;
    }

    pfcdm_snoop_gport_id = dest.mirror_dest_id;

    /*deliver original packets to R5 */
    bcm_mirror_header_info_t_init(&mirror_header_info);
    rv = bcm_mirror_header_info_set(unit, BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER, dest.mirror_dest_id, &mirror_header_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mirror_header_info_set\n");
        return rv;
    }
	
    printf("pfcdm_snoop_gport_id 0x%08x\n", pfcdm_snoop_gport_id);
    return 0;
};


int pfcdm_field_ipmf3_congest_mon_entry_add(int unit, int out_port, int priority, bcm_field_entry_t *entry_id_ptr)
{
    bcm_field_entry_info_t ent_info;
    int rv;

    /* Add entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 200;

    ent_info.nof_entry_quals = 3;

    ent_info.entry_qual[0].type = bcmFieldQualifyContainer;
    ent_info.entry_qual[0].value[0] = 0; /* miss in flow table */
    ent_info.entry_qual[0].mask[0] = 0xFFFFFFFF;
  
    ent_info.entry_qual[1].type = bcmFieldQualifyDstPort;
    ent_info.entry_qual[1].value[0] = out_port;
    ent_info.entry_qual[1].mask[0] = 0x1FFFFF;
  
    ent_info.entry_qual[2].type = bcmFieldQualifyIntPriority;
    ent_info.entry_qual[2].value[0] = priority;
    ent_info.entry_qual[2].mask[0] = 0x7;

    /* just for trap code == 9 packets. */
    ent_info.entry_qual[3].type = bcmFieldQualifyRxTrapCode;
    ent_info.entry_qual[3].value[0] = PFCDM_HW_DO_NOTHING_TRAP_CODE; /* ... */
    ent_info.entry_qual[3].mask[0] = 0x1FF;
	
    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = bcmFieldActionMirrorIngress; /* bcmFieldActionSnoop; */
    ent_info.entry_action[0].value[0] = pfcdm_snoop_gport_id;

    rv = bcm_field_entry_add(unit, 0, pfcdm_congest_mon_fg_id, &ent_info, entry_id_ptr);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("iPMF3 TCAM Entry Id (%d) for packet snoop\n", *entry_id_ptr);

    return rv;
}


int pfcdm_field_ipmf3_congest_mon_entry_del(int unit, bcm_field_entry_t entry_id)
{
    int rv;
    rv = bcm_field_entry_delete(unit, pfcdm_congest_mon_fg_id, NULL, entry_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in pfcdm_field_ipmf3_congest_mon_entry_del\n", rv);
        return rv;
    }

    return rv;
}

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
static int pfcdm_voq_id_output_pri_map(int unit, int core_id, uint16_t voq_id, uint32_t *out_port, uint8_t *pri)
{
    /* just for reference code */
    uint32 table_idx = voq_id / 4;
    uint32 value = 0;
    int rv = BCM_E_NONE;

    rv = soc_mem_read(unit, IPS_QSPMm, IPS_BLOCK(unit, core_id), table_idx, &value);
	
    *out_port = 0xC0000 | (value & 0x7FFF);
    *pri = voq_id % 8;

    return rv;
}



int pfcdm_congest_mon_process(int unit, int core_id, uint16_t voq_id)
{
    int rv = BCM_E_NONE;
    uint32_t out_port = 0;
    uint8_t pri = 0;
    uint16_t sw_entry_id = 0;
    bcm_field_entry_t hw_entry_id = 0;
    pfcdm_voq_congest_mon_entry_t *entry_ptr = NULL;
	
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




void appl_pfcdm_cong_mon_cb(
    int unit,
    uint32 core_id,
    uint32 resource_type,
    uint32 queue_idx)
{
    int rv = BCM_E_NONE;

    rv = pfcdm_congest_mon_process(unit, core_id, queue_idx);
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

/*
 * See prototype definition for function description
 */
int
appl_pfcdm_congest_mon_init(
    int unit)
{
    pfcdm_congest_mont_init();
    pfcdm_ingress_mirror_gport_get(unit);
    printf("appl_pfcdm_congest_mon_init\n");

    return BCM_E_NONE;
}
