/* $Id: arad_sw_db.h,v 1.102 Broadcom SDK $
* $Copyright: Copyright 2012 Broadcom Corporation.
* This program is the proprietary software of Broadcom Corporation
* and/or its licensors, and may only be used, duplicated, modified
* or distributed pursuant to the terms and conditions of a separate,
* written license agreement executed between you and Broadcom
* (an "Authorized License").  Except as set forth in an Authorized
* License, Broadcom grants no license (express or implied), right
* to use, or waiver of any kind with respect to the Software, and
* Broadcom expressly reserves all rights in and to the Software
* and all intellectual property rights therein.  IF YOU HAVE
* NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
* IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
* ALL USE OF THE SOFTWARE.  
*  
* Except as expressly set forth in the Authorized License,
*  
* 1.     This program, including its structure, sequence and organization,
* constitutes the valuable trade secrets of Broadcom, and you shall use
* all reasonable efforts to protect the confidentiality thereof,
* and to use this information only in connection with your use of
* Broadcom integrated circuit products.
*  
* 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
* PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
* REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
* OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
* DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
* NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
* ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
* CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
* OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
* 
* 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
* BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
* INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
* ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
* TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
* THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
* WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
* ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
* $Copyright: Copyright 2012 Broadcom Corporation.
* This program is the proprietary software of Broadcom Corporation
* and/or its licensors, and may only be used, duplicated, modified
* or distributed pursuant to the terms and conditions of a separate,
* written license agreement executed between you and Broadcom
* (an "Authorized License").  Except as set forth in an Authorized
* License, Broadcom grants no license (express or implied), right
* to use, or waiver of any kind with respect to the Software, and
* Broadcom expressly reserves all rights in and to the Software
* and all intellectual property rights therein.  IF YOU HAVE
* NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
* IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
* ALL USE OF THE SOFTWARE.  
*  
* Except as expressly set forth in the Authorized License,
*  
* 1.     This program, including its structure, sequence and organization,
* constitutes the valuable trade secrets of Broadcom, and you shall use
* all reasonable efforts to protect the confidentiality thereof,
* and to use this information only in connection with your use of
* Broadcom integrated circuit products.
*  
* 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
* PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
* REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
* OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
* DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
* NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
* ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
* CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
* OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
* 
* 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
* BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
* INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
* ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
* TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
* THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
* WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
* ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/


#ifndef __ARAD_SW_DB_INCLUDED__
/* { */
#define __ARAD_SW_DB_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/multicast.h>
#include <soc/dpp/multicast_imp.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_api_end2end_scheduler.h>
#include <soc/dpp/ARAD/arad_ofp_rates.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>
#include <soc/dpp/ARAD/arad_pmf_prog_select.h>
#include <soc/dpp/ARAD/arad_tcam.h>
#include <soc/dpp/ARAD/arad_tdm.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/SAND/Utils/sand_hashtable.h>
#include <soc/dpp/SAND/Utils/sand_hashtable.h>
#include <soc/dpp/SAND/Utils/sand_sorted_list.h>
#include <soc/dpp/ARAD/arad_tcam_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_fem.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>

#include <soc/dpp/SAND/Utils/sand_multi_set.h>

#include <soc/dpp/ARAD/arad_interrupts.h>
#include <soc/dpp/drv.h>


#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/kbp/alg_kbp/include/db.h>
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

/* } */

/*************
 * DEFINES   *
 *************/
/* { */


typedef uint16 ARAD_SYSPORT; /* type for system physical port */
#define ARAD_NOF_MODPORT (ARAD_NOF_FAPS_IN_SYSTEM*ARAD_NOF_FAP_PORTS) /* number of devices x number of device ports 2048*256=512K */
#define ARAD_MOD_PORT2SYSPORT_ALLOC_SIZE 2048 /* allocate memory in 2KB units to the modport 2 sysport mapping table */
#define ARAD_MOD_PORT2SYSPORT_ALLOC_NUM (sizeof(ARAD_SYSPORT[ARAD_NOF_MODPORT]) / 2048)
#define ARAD_MOD_PORT2SYSPORT_SYSPORTS_PER_ALLOC (ARAD_MOD_PORT2SYSPORT_ALLOC_SIZE / sizeof(ARAD_SYSPORT))
#define ARAD_MOD_PORT2SYSPORT_SYSPORTS_PER_ALLOC_BITS 10
#define ARAD_SW_DB_MODPORT2SYSPORT_REVERSE_GET_NOT_FOUND ((uint32)(-1)) /* return value of arad_sw_db_modport2sysport_reverse_get when it fails the search */

/* number of programs */
#define ARAD_SW_DB_PMF_NOF_PROGS                (ARAD_PMF_LOW_LEVEL_NOF_PROGS_ALL_STAGES)
/* number of cycle */
#define ARAD_SW_DB_PMF_NOF_CYCLES               (ARAD_PMF_LOW_LEVEL_NOF_CYCLES_MAX_ALL_LEVELS)
/* number of keys in program in cycle */
#define ARAD_SW_DB_PMF_NOF_KEYS                 (ARAD_PMF_LOW_LEVEL_PMF_KEY_MAX_ALL_STAGES + 1)
/* number of data bases */
#define ARAD_SW_DB_PMF_NOF_DBS                  (ARAD_PMF_LOW_LEVEL_NOF_DBS)
/* number of FES per program */
#define ARAD_SW_DB_PMF_NOF_FES                  (ARAD_PMF_LOW_LEVEL_NOF_FESS)
#define SOC_DPP_DEFS_MAX_TCAM_NOF_LINES_IN_BYTES           ((SOC_DPP_DEFS_MAX(NOF_TCAM_BIG_BANK_LINES)+7)/8)
#define SOC_DPP_DEFS_GET_TCAM_NOF_LINES_IN_BYTES(unit)     ((SOC_DPP_DEFS_GET(unit, nof_tcam_big_bank_lines)+7)/8)

/* Hash table properties */
#define ARAD_TCAM_DB_HASH_TBL_KEY_SIZE                           (sizeof(uint32) /* for entry */+ sizeof(uint16) /* for DB */)
#define ARAD_TCAM_DB_HASH_TBL_DATA_SIZE                          (sizeof(uint32) /* for hash-id */)



#define ARAD_SW_DB_NOF_DYNAMIC_QUEUE_TYPES 13
#define ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE 255

/* } */

/*************
 * GLOBALS   *
 *************/


/*************
 *  MACROS   *
 *************/
/* { */

#define ARAD_SW_DB_INIT_DEFS                    \
    uint32                                    \
    soc_sand_ret = SOC_SAND_OK

#define ARAD_SW_DB_FIELD_SET(res, unit, field_name, val)           \
    if (Arad_sw_db.arad_device_sw_db[unit] == NULL)                \
    {                                                                   \
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 1, exit);                         \
    }                                                                   \
    res = soc_sand_os_memcpy(&(Arad_sw_db.arad_device_sw_db[unit]->field_name), \
                         val,                                           \
                         sizeof(*val)                                   \
                         );                                             \
    SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);


#define ARAD_SW_DB_FIELD_GET(unit, field_name, val)         \
  if (Arad_sw_db.arad_device_sw_db[unit] == NULL)          \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      val,                                                        \
      &(Arad_sw_db.arad_device_sw_db[unit]->field_name),   \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;

#define ARAD_SW_DB_GLOBAL_FIELD_SET(field_name, val)             \
  uint32                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      &(Arad_sw_db.field_name),                                  \
      val,                                                        \
      sizeof(*val)                                                \
    );                                                            \
                                                                  \
  return soc_sand_ret;

#define ARAD_SW_DB_GLOBAL_FIELD_GET(field_name, val)             \
  uint32                                                        \
    soc_sand_ret = SOC_SAND_OK;                                           \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      val,                                                        \
      &(Arad_sw_db.field_name),                                  \
      sizeof(*val)                                                \
    );                                                            \
                                                                  \
  return soc_sand_ret;

/*************
 * TYPE DEFS *
 *************/
/* { */
typedef struct
{
  uint8
    valid;
  /* EGQ rate shaper */
  uint32
    egq_rates;
  /* EGQ burst max */
  uint32
    egq_bursts;
} ARAD_SW_DB_DEV_RATE;

typedef struct
{
  /* 
   * EGQ rate for each TCG entity. Summerize by NOF_PS * NOF_TCGS_IN_PS
   */
  ARAD_SW_DB_DEV_RATE
    tcg_rate[ARAD_EGR_NOF_PS][ARAD_NOF_TCGS];

} ARAD_SW_DB_DEV_EGR_TCG;

typedef struct
{
  /* 
   * EGQ rate for each Q-Pair
   */
  ARAD_SW_DB_DEV_RATE
    queue_rate[ARAD_EGR_NOF_Q_PAIRS];

} ARAD_SW_DB_DEV_EGR_PORT_PRIORITY;

typedef struct
{
  uint8
    valid;
  uint32
    sch_rates;
  uint32
    egq_rates;
  uint32
    egq_bursts;
}  ARAD_SW_DB_DEV_EGR_RATE;

typedef struct 
{
  uint32
    nof_calcal_instances;

} ARAD_SW_DB_DEV_EGR_CHAN_ARB;

typedef struct
{
  /* 
   * Bimtap occupation to allocate channelize arbiter for requested interface
   */
  SOC_SAND_OCC_BM_PTR
    chanif2chan_arb_occ;
  /* 
   * Bimtap occupation to allocate non channelize arbiter for requested scheduler interfaces
   */
  SOC_SAND_OCC_BM_PTR
    nonchanif2sch_offset_occ;

  /*  
   * Calendar information of Port-Priority
   */
  ARAD_SW_DB_DEV_EGR_PORT_PRIORITY
    port_priority_cal;

  /* 
   * Calendar information of TCG
   */
  ARAD_SW_DB_DEV_EGR_TCG
    tcg_cal;

  ARAD_SW_DB_DEV_EGR_CHAN_ARB
    chan_arb[ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB];

  uint32
    calcal_length;

  ARAD_SW_DB_DEV_EGR_RATE
    rates[SOC_DPP_DEFS_MAX(NOF_CORES)][ARAD_EGR_NOF_BASE_Q_PAIRS];

 /*
  *  In init all the calendar rates calculation must be done at one call
  *  in order to optimize the bring up time
  */
  uint8
    update_dev_changed;
 /* 
  * Indicates if each (egress) local port has a reassembly context reserved for it
  * for a non mirroring application. 1 means reserved.
  */
  uint32
    port_reserved_reassembly_context[ARAD_NOF_LOCAL_PORTS / SOC_SAND_NOF_BITS_IN_UINT32 ];
  /* 
  * Temp. field: 
  * Save the information of programming profile per port.
  */
  ARAD_EGR_PROG_TM_PORT_PROFILE
    ports_prog_editor_profile[ARAD_NOF_FAP_PORTS];

  /* 
   * ERP interface ID (taken from NIF avaiable interface)
   */
  ARAD_INTERFACE_ID
    erp_interface_id;

  /* 
   * Disable / Enable EGQ TCG and QPair shapers
   */
  uint8
    egq_tcg_qpair_shaper_enable;

}  ARAD_SW_DB_DEV_EGR_PORTS;

typedef struct 
{
  /* 
   * Bimtap occupation to allocate spoof-id for requested binding information
   */
  SOC_SAND_OCC_BM_PTR
    srcbind_arb_occ;

} ARAD_SW_DB_SRC_BINDS;

/* } */

typedef struct
{
  uint32 *egress_groups_open_data; /* replicate the information of is each egress group, in warm boot it is stored to the warm boot file */
} ARAD_SWDB_MULTICAST;
  


typedef struct
{
  /*
   *  If TRUE, soc_petrab devices exist in the system. This imposes
   *  certain limitations on Arad behavior (e.g. TDM optimize
   *  cells must be fixed size etc.).
   */
  uint8 is_petrab_in_system;
  /*
   * TDM traffic mode and FTMH format configuration
   */
  ARAD_MGMT_TDM_MODE tdm_mode;
  /*
   * ILKN TDM SP mode - does tdm get presedence over regular packets 
   */
  ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;

  /* 
   * Multicast table mode
   */

}  ARAD_SW_DB_OP_MODE;

typedef struct
{
 /*
  *  to identify whether the size of the LAG is zero or one.
  */
  uint8
    in_use[ARAD_NOF_LAG_GROUPS_MAX];

  uint32
    local_to_sys[ARAD_NOF_LOCAL_PORTS];

  /* 
   * Specify port mapping between ingress port index (0-252) to reassembly context (0-191)
   */
  uint32
    local_to_reassembly_context[ARAD_NOF_LOCAL_PORTS];

}  ARAD_SW_DB_LAGS_INFO;

typedef struct
{
 /*
  *  to identify whether the size of the LAG is zero or one.
  */
  ARAD_INTERFACE_ID
    context_map[ARAD_NOF_TDM_CONTEXT_MAP];  

}  ARAD_SW_DB_TDM;

typedef struct
{
  /*
   *  The cell ident of the source routed cell
   */
  uint16  current_cell_ident;
}  ARAD_SW_DB_CELL;


typedef struct
{
  /*  Buffers and index for FIFO read results. */
  uint32 *host_buff[SOC_TMC_CNT_NOF_PROCESSOR_IDS_ARAD]; 
  uint32 buff_line_ndx[SOC_TMC_CNT_NOF_PROCESSOR_IDS_ARAD]; 
}  ARAD_SW_DB_CNT;

typedef struct
{
  /*
   * The list of deleted buffers.
   */
  uint32  dram_deleted_buff_list[ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR];

}  ARAD_SW_DB_DRAM;

/*
 *  SW-DB
 */

typedef enum
{
  ARAD_NOF_SW_DB_MULTI_SETS
} ARAD_SW_DB_MULTI_SET;

#ifdef LINK_ARAD_LIBRARIES

typedef struct
{
  uint8                         valid;
  uint8                         has_direct_table;
  ARAD_TCAM_BANK_ENTRY_SIZE     entry_size;
  SOC_SAND_OCC_BM_PTR           entries_used[2]; /* 0 - regular, 1-its inverse */
  uint32                        nof_entries_free;
} ARAD_SW_DB_TCAM_BANK;

typedef struct
{
  uint8                     valid;
  ARAD_TCAM_ACTION_SIZE     action_bitmap_ndx;
  ARAD_TCAM_BANK_ENTRY_SIZE entry_size;
  uint32                    prefix_size;
  uint8                     bank_used[SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS];
  uint8                     is_direct;
  ARAD_TCAM_SMALL_BANKS     use_small_banks;  
  uint8                     no_insertion_priority_order;  
  uint8                     sparse_priorities;  
  ARAD_TCAM_PREFIX          prefix;  
  SOC_SAND_SORTED_LIST_INFO priorities;
  uint32                    access_profile_id[ARAD_PP_FP_KEY_NOF_KEYS_PER_DB_MAX];
  /* List of Databases forbidden to share a TCAM Bank with this one */
  uint32                    forbidden_dbs[ARAD_BIT_TO_U32(ARAD_TCAM_MAX_NOF_LISTS)];
  /* number of entries per bank */
  uint32                    bank_nof_entries[SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS];  
  SOC_SAND_OCC_BM_PTR       entries_used[SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS]; 
} ARAD_SW_DB_TCAM_DB;

typedef struct
{
  uint8 is_exist;
  SOC_SAND_SORTED_LIST_INIT_INFO init_info;
} ARAD_TCAM_SORTED_LIST_RESTORE;

typedef struct
{
  uint8 is_exist;
  SOC_SAND_OCC_BM_INIT_INFO init_info;
} ARAD_TCAM_OCC_BM_RESTORE;

typedef struct
{
  ARAD_TCAM_SORTED_LIST_RESTORE  priorities[ARAD_TCAM_MAX_NOF_LISTS]; /* Given DB and priority, retrieve the first and last acceptable entry-id */
  ARAD_TCAM_OCC_BM_RESTORE       entries_used[SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS][2];
  ARAD_TCAM_OCC_BM_RESTORE       db_entries_used[ARAD_TCAM_MAX_NOF_LISTS][SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS];
} ARAD_SW_DB_TCAM_DATA_FOR_RESTORATION;

typedef struct
{
  ARAD_SW_DB_TCAM_DATA_FOR_RESTORATION restoration_data;
  ARAD_SW_DB_TCAM_BANK bank[SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS];
   
  ARAD_SW_DB_TCAM_DB   tcam_db[ARAD_TCAM_MAX_NOF_LISTS];  
  SOC_SAND_HASH_TABLE_INFO  entry_id_to_location; /* Given DB and entry id, get an hash id for the next location table */
  ARAD_TCAM_LOCATION        *db_location_tbl; /* Given DB and entry, retrieve the location */
  ARAD_TCAM_GLOBAL_LOCATION *global_location_tbl; /* Given location, retrieve DB, entry-id and priority */
} ARAD_SW_DB_TCAM;

typedef struct
{
  uint8             valid;
  uint32             min_banks;
  ARAD_TCAM_BANK_OWNER bank_owner;
  ARAD_TCAM_MGMT_SIGNAL   callback;
  uint32              user_data;
} ARAD_SW_DB_TCAM_ACCESS_PROFILE;

typedef struct
{
  uint32 prefix_db[ARAD_TCAM_NOF_PREFIXES];  
  uint32 nof_dbs;
} ARAD_SW_DB_TCAM_MANAGED_BANK;

typedef struct
{ 
  ARAD_SW_DB_TCAM_ACCESS_PROFILE profiles[ARAD_TCAM_NOF_ACCESS_PROFILE_IDS]; 
  ARAD_SW_DB_TCAM_MANAGED_BANK   banks[SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS];
} ARAD_SW_DB_TCAM_MGMT;

typedef struct
{
  SOC_SAND_HASH_TABLE_INFO isem_key_to_entry_id;
} ARAD_SW_DB_VTT;

typedef struct
{
  uint32 vsi_to_isid[32*1024];
} ARAD_SW_DB_VSI;

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
typedef struct
{
  struct kbp_entry  *db_entry;
} ARAD_SW_KBP_HANDLE;
#endif

typedef struct
{
  SOC_SAND_HASH_TABLE_INFO route_key_to_entry_id;

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  /* 
   * In ACLs, a location table is needed 
   * to translate the SOC entry ID to the KBP entry handle. 
   */
  ARAD_SW_KBP_HANDLE *location_tbl;  /* for ACL location table */
  uint8 kbp_cache_mode;
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */

} ARAD_SW_DB_FRWRD_IP;


typedef struct{
  
    uint32 flags;
    uint32 storm_timed_count;
    uint32 storm_timed_period;
  
  }ARAD_SW_DB_INTERRUPT_DATA;

typedef struct
{  
/* The following 3 variables are not used and remain for future use */
  uint32      cmc_irq2_mask[SOC_CMCS_NUM_MAX]; /* Cached copy of CMIC_CMCx_PCIE_IRQ_MASK2 */
  uint32      cmc_irq3_mask[SOC_CMCS_NUM_MAX]; /* Cached copy of CMIC_CMCx_PCIE_IRQ_MASK3 */
  uint32      cmc_irq4_mask[SOC_CMCS_NUM_MAX]; /* Cached copy of CMIC_CMCx_PCIE_IRQ_MASK4 */
    
  ARAD_SW_DB_INTERRUPT_DATA* interrupt_data;
  
} ARAD_SW_DB_INTERRUPTS;

typedef struct
{
    uint8  is_simple_mode;
    uint32 ref_count[SOC_TMC_ITM_NOF_RATE_CLASSES];

} ARAD_SW_DB_QUEUE_TO_RATE_CLASS_MAPPING;

typedef struct
{
  uint8  is_power_saving_called;
  ARAD_SW_DB_QUEUE_TO_RATE_CLASS_MAPPING queue_to_rate_class_mapping;

}ARAD_SW_DB_TM;

typedef struct
{
  /*
   *  Used by instruction allocation code
   */
  uint8 is_used;

 /*
  * in which DB this CE is ued
  */
  uint8 db_id;

 /*
  * Number of lost bits in this CE
  */
  uint8 lsb;
 /*
  * MSB indicates the number of copied bits in this CE minus one
  */
  uint8 msb;

  /* Indicates the location inside the key - LSB or MSB */
  uint8 is_msb;

  /* In case of 320b, differentiate between the 2 keys */
  uint8 is_second_key;

  /* For this qualifier type, first bit (i.e., LSB) taken inside the qualifier (offst inside the qualifier)*/
  uint8 qual_lsb;
  /*
   * qual-type set by this CE
   */
  ARAD_PP_FP_QUAL_TYPE                qual_type;

} ARAD_SW_DB_PMF_CE;

typedef struct
{
  /*
   *  Used by instruction allocation code
   */
  uint8 is_used;

} ARAD_SW_DB_PMF_KEY;


typedef struct
{
  /*
   *  Used by instruction allocation code
   */
  uint32 ce[ARAD_SW_DB_PMF_NOF_PROGS][ARAD_SW_DB_PMF_NOF_CYCLES];

  uint32 key[ARAD_SW_DB_PMF_NOF_PROGS][ARAD_SW_DB_PMF_NOF_CYCLES];

  uint32 progs[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_PROGS)];

} ARAD_SW_DB_PMF_RESOURCE;

typedef struct
{
  /*
   *  relevant programs to update for the DB, bitmap
   */
  uint32 progs[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_PROGS)]; /* ok */

  /* which cycle is used for this DB - same cycle for any program due to the deterministic allocation */
  uint32 prog_used_cycle_bmp;

  /* which key is used in each program for this DB */
  uint8 used_key[ARAD_SW_DB_PMF_NOF_PROGS][ARAD_PP_FP_KEY_NOF_KEYS_PER_DB_MAX];

  /* priority of this Database */
  uint32 prio;

  /* is-320b  */
  uint32 is_320b;

  /* Which half-keys are taken  */
  uint32 alloc_place;

 /* valid Database */
  uint8 valid; 

    /* which key is used in each program for this DB */
  uint32 nof_bits_zone[ARAD_PP_FP_KEY_NOF_KEYS_PER_DB_MAX][2];

  /* 
   * Cascaded key: assign it before committing to HW and SW 
   * Not assigned if equal to 0 
   * Otherwise: Key+1 (1..4 in Arad) 
   * Relevant only for 2nd cycle cascaded DBs 
   */
  uint8 cascaded_key; 

} ARAD_SW_DB_PMF_DB_INFO;

/*
 * PSL info
 */
typedef struct ARAD_SW_DB_PMF_PSL_LINE_INFO_s
{
  /*
   *  group represented by this line:
   *  buffer itself (value and mask)
   */
  ARAD_PMF_SEL_GROUP group; 
  /*
   *  Bitmap of groups that intersected in this line
   */
  uint32 groups[ARAD_PMF_GROUP_LEN];
  /*
   *  Program ID this line associated to.
   *  If ARAD_PMF_PROG_INVALID, then program invalid
   */
  uint32 prog_id;
  /* Define line index in case of presel mgmt advanced mode*/
  uint32 line_id;
  /* Define the action according to ARAD_PMF_SEL_LINE_ACTION */
  uint32 flags;

} ARAD_SW_DB_PMF_PSL_LINE_INFO;


typedef struct ARAD_SW_DB_PMF_PSL_LEVEL_INFO_s
{
  /*
   *  index of first line belongs to this level
   *  In practice, the lowest (high value) line (closest to static non-Eth lines)
   */
  uint32 first_index;
  /*
   *  index of last line belongs to this level
   *  In practice, the highest (lowest value) line (closest to 0)
   */
  uint32 last_index;

  ARAD_SW_DB_PMF_PSL_LINE_INFO lines[ARAD_PMF_NOF_LINES_IN_LEVEL_MAX_ALL_STAGES];
  /*
   *  number of lines in this level not include new lines
   */
  uint32 nof_lines;
  /*
   *  number of new lines in this level from last calculation
   */
  uint32 nof_new_lines;
  /*
   *  number of new lines in this level from last calculation
   */
  uint32 nof_removed_lines;
  /*
   *  level index
   */
  uint32 level_index;

} ARAD_SW_DB_PMF_PSL_LEVEL_INFO;




typedef struct
{
  /*
   *  for each PFG what is the Db in use.
   */
  uint32 pfgs_db_pmb[ARAD_PMF_NOF_GROUPS][ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)];
  /*
   *  the Db always in use because they have no preselectors.
   */
  uint32 default_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)];
  /* 
   * TM PFGs for the special usage of PMF by TM applications
   */
  uint32 pfgs_tm_bmp[ARAD_BIT_TO_U32(ARAD_PMF_NOF_GROUPS)];

  ARAD_SW_DB_PMF_PSL_LEVEL_INFO levels_info[2/*is-TM*/][ARAD_PMF_NOF_LEVELS_MAX_ALL_STAGES];

  ARAD_PMF_SEL_INIT_INFO init_info;

} ARAD_SW_DB_PMF_PSL_INFO;


typedef struct
{
  /*
   *	Indicates if the number of entries already created
   */
  uint32 nof_db_entries;

} ARAD_SW_DB_FP_ENTRY;

typedef struct
{
  /*
   *  Indicate the Database type (TCAM, Direct Table, Direct Extraction, or not exist)
   */
  ARAD_PP_FP_DATABASE_INFO db_info[ARAD_PP_FP_NOF_DBS];

  /*
   *  Indicates if the number of entries already created
   */
  ARAD_SW_DB_FP_ENTRY db_entries[ARAD_SW_DB_PMF_NOF_DBS];

  /*
   * Packet Format Groups
   */
  SOC_TMC_PMF_PFG_INFO pfg_info[ARAD_PMF_NOF_GROUPS];

  /*
   * User-Defined Fields
   */
  ARAD_PMF_CE_QUAL_INFO  udf[ARAD_PP_FP_NOF_HDR_USER_DEFS];

  /*
   * Number of VLAN Tags in the inner Ethernet header per PFG
   */
  uint32 inner_eth_nof_tags[ARAD_PMF_NOF_GROUPS];
  /*
   * Key change size in bits. Default: 12 bits
   */
  uint8 key_change_size;

  uint8 entry_bitmap[SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS][SOC_DPP_DEFS_MAX_TCAM_NOF_LINES_IN_BYTES];

} ARAD_SW_DB_FP_INFO;


typedef struct
{
    /*
     * PMF-Program Copy Engine instructions 
     * for each program x cycle: 
     *   16 CE 
     *   - 8 * 16
     *   - 8 * 32
     *   where
     *   - 8 for LSB (72)
     *   - 8 for MSB (144)
     */
    ARAD_SW_DB_PMF_CE pgm_ce[ARAD_SW_DB_PMF_NOF_PROGS][ARAD_SW_DB_PMF_NOF_CYCLES][ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_MAX_ALL_LEVELS];

    ARAD_SW_DB_PMF_FES pgm_fes[ARAD_SW_DB_PMF_NOF_PROGS][ARAD_SW_DB_PMF_NOF_FES]; 

    ARAD_PP_FP_FEM_ENTRY fem_entry[ARAD_PMF_LOW_LEVEL_NOF_FEMS];

    ARAD_SW_DB_PMF_RESOURCE rsources;

    /* List of Databases per Program */
    uint32 pgm_db_pmb[ARAD_SW_DB_PMF_NOF_PROGS][ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)]; 

    ARAD_SW_DB_PMF_DB_INFO db_info[ARAD_SW_DB_PMF_NOF_DBS]; 

    ARAD_SW_DB_PMF_PSL_INFO psl_info;

    /* FP info */
    ARAD_SW_DB_FP_INFO fp_info;

} ARAD_SW_DB_PMF;


#endif /* LINK_ARAD_LIBRARIES */


typedef struct
{
  ARAD_SW_DB_OP_MODE
    op_mode;

  ARAD_SW_DB_LAGS_INFO
    lag;

  ARAD_SW_DB_TDM
    tdm;

  ARAD_SW_DB_DEV_EGR_PORTS
    arad_sw_db_egr_ports;

  ARAD_SW_DB_SRC_BINDS
    arad_sw_db_src_binds;

  ARAD_SWDB_MULTICAST
    arad_sw_db_multicast;

  uint8 /* maps user defined queue types to (the smaller amount of) hardware types (credit request profiled) */
    q_type_map[ARAD_SW_DB_NOF_DYNAMIC_QUEUE_TYPES];

  ARAD_SW_DB_CELL
    cell;

  ARAD_SW_DB_DRAM
    dram;

#ifdef LINK_ARAD_LIBRARIES

  ARAD_SW_DB_TCAM
    tcam;

  ARAD_SW_DB_TCAM_MGMT
    tcam_mgmt;

  ARAD_SW_DB_VTT
    vtt;

  ARAD_SW_DB_VSI
    vsi;

  ARAD_SW_DB_FRWRD_IP
    frwrd_ip;


  ARAD_SW_DB_PMF
    pmf[ARAD_NOF_FP_DATABASE_STAGES];
  ARAD_SW_DB_CNT
    cnt;

#endif /* LINK_ARAD_LIBRARIES */

  ARAD_SW_DB_INTERRUPTS interrupts;

  /* 2D array representing the mapping. 1D allocated if the mapping is supported. 2D allocated on demand */
  ARAD_SYSPORT** modport2sysport;

  ARAD_SW_DB_TM tm;
}  ARAD_SW_DB_DEVICE;


typedef struct
{
  ARAD_SW_DB_DEVICE*
    arad_device_sw_db[SOC_SAND_MAX_DEVICE];
}  ARAD_SW_DB;

extern uint8 Arad_sw_db_initialized;
extern ARAD_SW_DB_DEVICE **_arad_device_sw_db;

/*************
 * FUNCTIONS *
 *************/
/* { */

/********************************************************************************************
 * Initialization
 * {
 ********************************************************************************************/
uint32
  arad_sw_db_init(void);

void
  arad_sw_db_close(void);

/*
 *  Per-device software database initializations
 */
uint32
  arad_sw_db_device_init(
    SOC_SAND_IN int     unit
  );

/*
 *  Per-device software database closing
 */
uint32
  arad_sw_db_device_close(
    SOC_SAND_IN int unit
  );

/*********************************************************************************************
 * }
 * arad_egr_ports
 * {
 *********************************************************************************************/

uint32
  arad_sw_db_egr_ports_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                  base_q_pair,
                 int                     core,
    SOC_SAND_OUT ARAD_SW_DB_DEV_EGR_RATE *val
  );

uint32
  arad_sw_db_egr_ports_port_priority_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT ARAD_SW_DB_DEV_EGR_PORT_PRIORITY  *val
  );

uint32
  arad_sw_db_egr_ports_port_priority_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                q_pair,
    SOC_SAND_IN ARAD_SW_DB_DEV_RATE      *val
  );

uint32
  arad_sw_db_egr_ports_tcg_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT ARAD_SW_DB_DEV_EGR_TCG   *val
  );

uint32
  arad_sw_db_egr_ports_tcg_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                ps,
    SOC_SAND_IN uint32                tcg_ndx,
    SOC_SAND_IN ARAD_SW_DB_DEV_RATE      *val
  );




/*********************************************************************************************
 * }
 * arad_scheduler
 * {
 *********************************************************************************************/

uint32
  arad_sw_db_sch_max_expected_port_rate_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 port_ndx,
    SOC_SAND_IN uint32  rate
  );

uint32
  arad_sw_db_sch_max_expected_port_rate_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 port_ndx
  );

uint32
  arad_sw_db_sch_accumulated_grp_port_rate_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 grp_ndx,
    SOC_SAND_IN uint32  rate
  );

uint32
  arad_sw_db_sch_accumulated_grp_port_rate_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 grp_ndx
  );

/*********************************************************************************************
 * }
 * arad_multicast
 * {
 *********************************************************************************************/

/* Mark the given egress multicast group as open or not in SWDB */
uint32 arad_sw_db_egress_group_open_set(
    SOC_SAND_IN  int     unit, /* device */
    SOC_SAND_IN  uint32  group_id,  /* multicast ID */
    SOC_SAND_IN  uint8   is_open    /* non zero value will mark the group as open */
);

/* Check if the given egress multicast group is created=opened, will return 1 if the group is marked as open, or 0 */
uint8 arad_sw_db_egress_group_open_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 group_id /* multicast ID */
);

/* return the address of the egress_groups_open_data pointer in SWDB */
uint32** arad_sw_db_get_egress_group_open_data_address(
    SOC_SAND_IN  int    unit
);


uint32
  arad_sw_db_op_mode_init(
    SOC_SAND_IN int unit
  );

void
  arad_sw_db_is_petrab_in_system_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_petrab_in_system
  );

uint8
  arad_sw_db_is_petrab_in_system_get(
    SOC_SAND_IN int unit
  );


void
  arad_sw_db_tdm_mode_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_MGMT_TDM_MODE tdm_mode
  );

ARAD_MGMT_TDM_MODE
  arad_sw_db_tdm_mode_get(
    SOC_SAND_IN int unit
  );


void
  arad_sw_db_ilkn_tdm_dedicated_queuing_set(
     SOC_SAND_IN int unit,
     SOC_SAND_IN ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing
  );

ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE
  arad_sw_db_ilkn_tdm_dedicated_queuing_get(
     SOC_SAND_IN int unit
  );


/*
 *  Cell
 */

uint32
  arad_sw_db_cell_cell_ident_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint16                      cell_ident
  );

uint32
  arad_sw_db_cell_cell_ident_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint16                      *cell_ident
  );

/*
 * Cnt
 */
uint32
  arad_sw_db_cnt_buff_and_index_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint16                     proc_id,
    SOC_SAND_IN uint32                     *buff,
    SOC_SAND_IN uint32                     index
  );
uint32
  arad_sw_db_cnt_buff_and_index_get(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint16                     proc_id,
    SOC_SAND_OUT uint32                     **buff,
    SOC_SAND_OUT uint32                    *index
   );
  
uint32
  arad_sw_db_dram_deleted_buff_list_add(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     buff
  );

uint32
  arad_sw_db_dram_deleted_buff_list_remove(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     buff
  );
 
uint32
  arad_sw_db_dram_deleted_buff_list_get(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     buff,
    SOC_SAND_OUT uint32*     is_deleted
  ); 
 
uint32 
  arad_sw_db_dram_deleted_buff_list_get_all(
    SOC_SAND_IN int    unit,
    SOC_SAND_OUT uint32*    buff_list_arr,
    SOC_SAND_IN uint32      arr_size,
    SOC_SAND_OUT uint32*    buff_list_num);
  
/*
 * Interrupts
*/

/* The following 6 functions are not used and remain for future use */
uint32 arad_sw_db_interrupts_cmc_irq2_mask_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       cmc,
    SOC_SAND_IN uint32                       val
  );  

uint32 arad_sw_db_interrupts_cmc_irq2_mask_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       cmc,
    SOC_SAND_OUT uint32*                     val
  ); 
  
uint32 arad_sw_db_interrupts_cmc_irq3_mask_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       cmc,
    SOC_SAND_IN uint32                       val
  );  

uint32 arad_sw_db_interrupts_cmc_irq3_mask_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       cmc,
    SOC_SAND_OUT uint32*                     val
  );
  
  
uint32 arad_sw_db_interrupts_cmc_irq4_mask_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       cmc,
    SOC_SAND_IN uint32                       val
  );  

uint32 arad_sw_db_interrupts_cmc_irq4_mask_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       cmc,
    SOC_SAND_OUT uint32*                     val
  );  
  
uint32
  arad_sw_db_interrupts_flags_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       interrupt_id,
    SOC_SAND_IN uint32                      val 
  );

uint32
  arad_sw_db_interrupts_flags_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       interrupt_id,
    SOC_SAND_OUT uint32*                      val 
  );

uint32
  arad_sw_db_interrupts_storm_timed_count_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       interrupt_id,
    SOC_SAND_IN uint32                      val 
  );

uint32
  arad_sw_db_interrupts_storm_timed_count_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       interrupt_id,
    SOC_SAND_OUT uint32*                      val 
  );

uint32
  arad_sw_db_interrupts_storm_timed_period_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       interrupt_id,
    SOC_SAND_IN uint32                       val 
  );

uint32
  arad_sw_db_interrupts_storm_timed_period_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       interrupt_id,
    SOC_SAND_OUT uint32*                      val 
  );
/*
 * LAGs
 */
uint32
  arad_sw_db_lag_in_use_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                      lag_ndx,
    SOC_SAND_IN uint8                      use
  );

uint32
  arad_sw_db_lag_in_use_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                       lag_ndx,
    SOC_SAND_OUT uint8                      *use
  );

uint32
  arad_sw_db_local_to_sys_port_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                     tm_port,
    SOC_SAND_IN uint32                     sys_port
  );

uint32
  arad_sw_db_local_to_sys_port_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                      tm_port,
    SOC_SAND_OUT uint32                     *sys_port
  );

uint32
  arad_sw_db_local_to_reassembly_context_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                     tm_port,
    SOC_SAND_IN uint32                     reassembly_context
  );

/*
 * TDM
 */
uint32  
  arad_sw_db_tdm_context_map_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                     tdm_context_map_id,
    SOC_SAND_IN ARAD_INTERFACE_ID          if_id
  );

uint32
  arad_sw_db_tdm_context_map_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                      tdm_context_map_id,
    SOC_SAND_OUT ARAD_INTERFACE_ID          *if_id
  );

uint32
  arad_sw_db_local_to_reassembly_context_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                      tm_port
  );

/*
 * check/set if a (egress) local port has a reassembly context reserved for it
 * for a non mirroring application. 
 */
uint32
  arad_sw_db_is_port_reserved_for_reassembly_context(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_OUT uint8  *is_reserved /* returns one of: 0 for not reserved, 1 for reserved */
  );

uint32
  arad_sw_db_set_port_reserved_for_reassembly_context(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint8   reserve /* 0 will cancel reservation, other values will reserve */
  );

SOC_SAND_OCC_BM_PTR
  arad_sw_db_egr_ports_chanif2chan_arb_occ_get(
    SOC_SAND_IN int unit
  );

SOC_SAND_OCC_BM_PTR
  arad_sw_db_egr_ports_nonchanif2sch_offset_occ_get(
    SOC_SAND_IN int unit
  );

SOC_SAND_OCC_BM_PTR
  arad_sw_db_src_bind_arb_occ_get(
    SOC_SAND_IN int unit
  );

uint32
  arad_sw_db_egr_ports_prog_editor_profile_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 ofp_idx,
    SOC_SAND_IN ARAD_EGR_PROG_TM_PORT_PROFILE port_profile
  );

ARAD_EGR_PROG_TM_PORT_PROFILE
  arad_sw_db_egr_ports_prog_editor_profile_get(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32  tm_port
  );


uint32
  arad_sw_db_egr_ports_erp_interface_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_INTERFACE_ID erp_interface_id
  );

ARAD_INTERFACE_ID
  arad_sw_db_egr_ports_erp_interface_get(
    SOC_SAND_IN int unit
  );

uint32
  arad_sw_db_egr_ports_egq_tcg_qpair_shaper_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8  is_enable
  );

uint8
  arad_sw_db_egr_ports_egq_tcg_qpair_shaper_enable_get(
    SOC_SAND_IN int unit
  );

uint32
  arad_sw_db_ofp_rates_calcal_length_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 calcal_length
  );

uint32
  arad_sw_db_ofp_rates_calcal_length_get(
    SOC_SAND_IN int unit
  );

uint32
  arad_sw_db_ofp_rates_update_dev_changed_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 update_dev_changed
  );

uint8
  arad_sw_db_ofp_rates_update_dev_changed_get(
    SOC_SAND_IN int unit
  );

uint32
  arad_sw_db_ofp_rates_nof_instances_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_OFP_RATES_EGQ_CHAN_ARB_ID chan_arb_id,
    SOC_SAND_IN uint32 nof_instances
  );

uint32
  arad_sw_db_ofp_rates_nof_instances_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_OFP_RATES_EGQ_CHAN_ARB_ID chan_arb_id
  );

/*
 * TCAM
 */

 /* restoration data*/
uint32
  arad_sw_db_tcam_occ_bm_restore_set(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                   bank_id,
    SOC_SAND_IN uint32                   is_inverse,
    SOC_SAND_IN ARAD_TCAM_OCC_BM_RESTORE occ_bm_restore
  );

uint32
  arad_sw_db_tcam_occ_bm_restore_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   bank_id,
    SOC_SAND_IN  uint32                   is_inverse,
    SOC_SAND_OUT ARAD_TCAM_OCC_BM_RESTORE *occ_bm_restore
  );

uint32
  arad_sw_db_tcam_db_bank_occ_bm_restore_set(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                   tcam_db_id,
    SOC_SAND_IN uint32                   bank_id,
    SOC_SAND_IN ARAD_TCAM_OCC_BM_RESTORE occ_bm_restore
  );

uint32
  arad_sw_db_tcam_db_bank_occ_bm_restore_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   tcam_db_id,
    SOC_SAND_IN  uint32                   bank_id,
    SOC_SAND_OUT ARAD_TCAM_OCC_BM_RESTORE *occ_bm_restore
  );


uint32
  arad_sw_db_tcam_sorted_list_restore_set(
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN uint32                        tcam_db_id,
    SOC_SAND_IN ARAD_TCAM_SORTED_LIST_RESTORE sorted_list_restore
  );

uint32
  arad_sw_db_tcam_sorted_list_restore_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                        tcam_db_id,
    SOC_SAND_OUT ARAD_TCAM_SORTED_LIST_RESTORE *sorted_list_restore
  );

    /* end of restoration data */

uint32
  arad_sw_db_tcam_bank_valid_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint8  valid
  );

uint32
  arad_sw_db_tcam_bank_valid_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_OUT uint8  *valid
  );

uint32
  arad_sw_db_tcam_bank_has_direct_table_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint8  has_direct_table
  );

uint32
  arad_sw_db_tcam_bank_has_direct_table_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_OUT uint8  *has_direct_table
  );

uint32
  arad_sw_db_tcam_bank_entry_size_set(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                    bank_id,
    SOC_SAND_IN ARAD_TCAM_BANK_ENTRY_SIZE entry_size
  );

uint32
  arad_sw_db_tcam_bank_entry_size_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                    bank_id,
    SOC_SAND_OUT ARAD_TCAM_BANK_ENTRY_SIZE *entry_size
  );

void
  arad_sw_db_tcam_bank_entries_used_set(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32       bank_id,
    SOC_SAND_IN uint8        is_inverse,
    SOC_SAND_IN SOC_SAND_OCC_BM_PTR entries_used
  );

SOC_SAND_OCC_BM_PTR
  arad_sw_db_tcam_bank_entries_used_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint8  is_inverse
  );
  
void
  arad_sw_db_tcam_db_entries_used_set(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32       tcam_db_id,
    SOC_SAND_IN uint32       bank_id,
    SOC_SAND_IN SOC_SAND_OCC_BM_PTR entries_used
  );

SOC_SAND_OCC_BM_PTR
  arad_sw_db_tcam_db_entries_used_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 bank_id
  );
  
uint32
  arad_sw_db_tcam_bank_nof_entries_free_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 nof_entries_free
  );
uint32
  arad_sw_db_tcam_bank_nof_entries_free_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_OUT uint32 *nof_entries_free
  );
uint32
  arad_sw_db_tcam_db_valid_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint8  valid
  );

uint32
  arad_sw_db_tcam_db_valid_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 tcam_db_id,
    SOC_SAND_OUT uint8  *valid
  );

uint32
  arad_sw_db_tcam_db_is_direct_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint8  is_direct
  );

uint32
  arad_sw_db_tcam_db_is_direct_get(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_OUT uint8 *is_direct
  );

uint32
  arad_sw_db_tcam_db_entry_size_set(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                    tcam_db_id,
    SOC_SAND_IN ARAD_TCAM_BANK_ENTRY_SIZE entry_size
  );

uint32
  arad_sw_db_tcam_db_entry_size_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                    tcam_db_id,
    SOC_SAND_OUT ARAD_TCAM_BANK_ENTRY_SIZE *entry_size
  );

uint32
  arad_sw_db_tcam_db_action_bitmap_ndx_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                tcam_db_id,
    SOC_SAND_IN ARAD_TCAM_ACTION_SIZE action_bitmap_ndx
  );
uint32
  arad_sw_db_tcam_db_action_bitmap_ndx_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                tcam_db_id,
    SOC_SAND_OUT ARAD_TCAM_ACTION_SIZE *action_bitmap_ndx
  );


uint32
  arad_sw_db_tcam_db_prefix_size_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 prefix_size
  );

uint32
  arad_sw_db_tcam_db_prefix_size_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 tcam_db_id,
    SOC_SAND_OUT uint32 *prefix_size
  );

uint32
  arad_sw_db_tcam_db_bank_used_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint8  is_used
  );
uint32
  arad_sw_db_tcam_db_bank_used_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 tcam_db_id,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_OUT uint8  *is_used
  );

uint32
  arad_sw_db_tcam_use_small_banks_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                tcam_db_id,
    SOC_SAND_IN ARAD_TCAM_SMALL_BANKS use_small_banks
  );
uint32
  arad_sw_db_tcam_use_small_banks_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                tcam_db_id,
    SOC_SAND_OUT ARAD_TCAM_SMALL_BANKS *use_small_banks
  );

uint32
  arad_sw_db_tcam_no_insertion_priority_order_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint8  no_insertion_priority_order
  );
uint32
  arad_sw_db_tcam_no_insertion_priority_order_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 tcam_db_id,
    SOC_SAND_OUT uint8  *no_insertion_priority_order
  );

uint32
  arad_sw_db_tcam_sparse_priorities_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint8  sparse_priorities
  );
uint32
  arad_sw_db_tcam_sparse_priorities_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 tcam_db_id,
    SOC_SAND_OUT uint8  *sparse_priorities
  );

uint32
  arad_sw_db_tcam_db_bank_nof_entries_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 bank_nof_entries
  );
uint32
  arad_sw_db_tcam_db_bank_nof_entries_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 tcam_db_id,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_OUT uint32 *bank_nof_entries
  );

uint32
  arad_sw_db_tcam_db_access_profile_id_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 access_profile_array_id,
    SOC_SAND_IN uint32 access_profile_id    
  );

uint32
  arad_sw_db_tcam_db_access_profile_id_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 tcam_db_id,
    SOC_SAND_IN  uint32 access_profile_array_id,
    SOC_SAND_OUT uint32 *access_profile_id
  );

uint32
  arad_sw_db_tcam_db_forbidden_dbs_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 tcam_db_other,
    SOC_SAND_IN uint8  is_forbidden
  );

uint32
  arad_sw_db_tcam_db_forbidden_dbs_get(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 tcam_db_other,
    SOC_SAND_OUT uint8 *is_forbidden
  );

uint32
  arad_sw_db_tcam_db_prefix_set(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN uint32           tcam_db_id,
    SOC_SAND_IN ARAD_TCAM_PREFIX *prefix
  );


uint32
  arad_sw_db_tcam_db_prefix_get(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  uint32      tcam_db_id,
    SOC_SAND_OUT ARAD_TCAM_PREFIX *prefix
  );

SOC_SAND_SORTED_LIST_INFO *
  arad_sw_db_tcam_db_priorities_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  );

SOC_SAND_HASH_TABLE_INFO *
  arad_sw_db_tcam_db_entry_id_to_location_get(
    SOC_SAND_IN int unit
  );

uint32
  arad_sw_db_tcam_db_location_tbl_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32             location_tbl_ndx,
    SOC_SAND_IN ARAD_TCAM_LOCATION *location
  );

uint32 
  arad_sw_db_tcam_db_location_tbl_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             location_tbl_ndx,
    SOC_SAND_OUT ARAD_TCAM_LOCATION *location
  );

uint32
  arad_sw_db_tcam_global_location_tbl_set(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                    location_tbl_ndx,
    SOC_SAND_IN ARAD_TCAM_GLOBAL_LOCATION *location
  );

uint32 
  arad_sw_db_tcam_global_location_tbl_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                    location_tbl_ndx,
    SOC_SAND_OUT ARAD_TCAM_GLOBAL_LOCATION *location
  );

uint32
  arad_sw_db_tcam_access_profile_valid_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint8  valid
  );

uint32
  arad_sw_db_tcam_access_profile_valid_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 profile,
    SOC_SAND_OUT uint8  *valid
  );


uint32
  arad_sw_db_tcam_access_profile_min_banks_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint32 min_banks
  );

uint32
  arad_sw_db_tcam_access_profile_min_banks_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 profile,
    SOC_SAND_OUT uint32 *min_banks
  );

uint32
  arad_sw_db_tcam_access_profile_bank_owner_set(
    SOC_SAND_IN int                  unit,
    SOC_SAND_IN uint32               profile,
    SOC_SAND_IN ARAD_TCAM_BANK_OWNER bank_owner
  );

uint32
  arad_sw_db_tcam_access_profile_bank_owner_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32               profile,
    SOC_SAND_OUT ARAD_TCAM_BANK_OWNER *bank_owner
  );

void
  arad_sw_db_tcam_access_profile_callback_set(
    SOC_SAND_IN int           unit,
    SOC_SAND_IN uint32           profile,
    SOC_SAND_IN ARAD_TCAM_MGMT_SIGNAL callback
  );

ARAD_TCAM_MGMT_SIGNAL
  arad_sw_db_tcam_access_profile_callback_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  );

uint32
  arad_sw_db_tcam_access_profile_user_data_set(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32  profile,
    SOC_SAND_IN uint32  user_data
  );

uint32
  arad_sw_db_tcam_access_profile_user_data_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 profile,
    SOC_SAND_OUT uint32 *user_data
  );

uint32
  arad_sw_db_tcam_managed_bank_prefix_db_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 prefix,
    SOC_SAND_IN uint32 tcam_db_id
  );

uint32
  arad_sw_db_tcam_managed_bank_prefix_db_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_IN  uint32 prefix,
    SOC_SAND_OUT uint32 *tcam_db_id
  );

uint32
  arad_sw_db_tcam_managed_bank_nof_db_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 nof_dbs
  );

uint32
  arad_sw_db_tcam_managed_bank_nof_db_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 bank_id,
    SOC_SAND_OUT uint32 *nof_dbs
  );
SOC_SAND_HASH_TABLE_INFO *
  arad_sw_db_vtt_isem_key_to_entry_id_get(
    SOC_SAND_IN int unit
  );

uint32
  arad_sw_db_vsi_isid_set(
    SOC_SAND_IN int unit,
	SOC_SAND_IN uint32 vsi,
	SOC_SAND_IN uint32 isid
  );

uint32
  arad_sw_db_vsi_isid_get(
    SOC_SAND_IN int unit,
	SOC_SAND_IN uint32 vsi
  );

SOC_SAND_HASH_TABLE_INFO *
  arad_sw_db_frwrd_ip_route_key_to_entry_id_get(
    SOC_SAND_IN int unit
  );

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
uint32
  arad_sw_db_frwrd_ip_kbp_location_tbl_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN  uint32                  data_index,
    SOC_SAND_OUT ARAD_SW_KBP_HANDLE      *location
  );

uint32
  arad_sw_db_frwrd_ip_kbp_location_tbl_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN  uint32                  data_index,
    SOC_SAND_IN ARAD_SW_KBP_HANDLE      *location
  );

uint32
  arad_sw_db_frwrd_ip_kbp_cache_mode_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint32 *val
  );

uint32
  arad_sw_db_frwrd_ip_kbp_cache_mode_set(
    SOC_SAND_IN int  unit,
    SOC_SAND_IN uint32  val
  );
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */


uint32
arad_sw_db_vtt_terminate(
    SOC_SAND_IN  int unit
  );

uint32
arad_sw_db_frwrd_ip_terminate(
    SOC_SAND_IN  int unit
  );
uint32
  arad_sw_db_tcam_terminate(
    SOC_SAND_IN  int unit
  );
uint32
  arad_sw_db_pgm_ce_instr_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32             prog_ndx,
    SOC_SAND_IN  uint32             cycle_ndx,
    SOC_SAND_IN  uint32             instr_ndx,
    SOC_SAND_IN  ARAD_SW_DB_PMF_CE  *ce_instr
  );

/* */
uint32
  arad_sw_db_pgm_ce_instr_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                 prog_ndx,
    SOC_SAND_IN  uint32                 cycle_ndx,
    SOC_SAND_IN  uint32                 ce_ndx,
    SOC_SAND_OUT ARAD_SW_DB_PMF_CE      *ce_instr
  );

uint32
  arad_sw_db_pgm_fes_set(
    SOC_SAND_IN  int             devifes_id,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32             prog_ndx,
    SOC_SAND_IN  uint32             fes_ndx,
    SOC_SAND_IN  ARAD_SW_DB_PMF_FES  *fes
  );

/* */
uint32
  arad_sw_db_pgm_fes_get(
    SOC_SAND_IN  int                 devifes_id,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                 prog_ndx,
    SOC_SAND_IN  uint32                 fes_ndx,
    SOC_SAND_OUT ARAD_SW_DB_PMF_FES      *fes
  );
  
uint32
  arad_sw_db_pgm_all_fes_get(
    SOC_SAND_IN  int                 devifes_id,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                 prog_ndx,
    SOC_SAND_OUT ARAD_SW_DB_PMF_FES      fes_arr[ARAD_PMF_LOW_LEVEL_NOF_FESS]
  );
  

uint32
  arad_sw_db_fp_fem_entry_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             fem_ndx,
    SOC_SAND_IN  ARAD_PP_FP_FEM_ENTRY   *fem_entry
  );

uint32
  arad_sw_db_fp_fem_entry_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             fem_ndx,
    SOC_SAND_OUT ARAD_PP_FP_FEM_ENTRY   *fem_entry
  );

/*
 * PMF resource sattus API
 */
uint32
  arad_sw_db_pgm_ce_rsrc_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32             prog_ndx,
    SOC_SAND_IN  uint32             cycle_ndx,
    SOC_SAND_IN  uint32             *ce_rsrc
  );

/* */
uint32
  arad_sw_db_pgm_ce_rsrc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                 prog_ndx,
    SOC_SAND_IN  uint32                 cycle_ndx,
    SOC_SAND_OUT uint32                 *ce_rsrc
  );



uint32
  arad_sw_db_pgm_key_rsrc_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32             prog_ndx,
    SOC_SAND_IN  uint32             cycle_ndx,
    SOC_SAND_IN  uint32             *key_rsrc
  );

/* */
uint32
  arad_sw_db_pgm_key_rsrc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                 prog_ndx,
    SOC_SAND_IN  uint32                 cycle_ndx,
    SOC_SAND_OUT uint32                 *key_rsrc
  );

uint32
  arad_sw_db_pgm_program_rsrc_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32             *program_rsrc
  );

uint32
  arad_sw_db_pgm_program_rsrc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_OUT uint32                 *program_rsrc
  );

uint32
  arad_sw_db_pmf_db_info_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                             db_ndx,
    SOC_SAND_IN  ARAD_SW_DB_PMF_DB_INFO             *db_info
  );

/* */
uint32
  arad_sw_db_pmf_db_info_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                             db_ndx,
    SOC_SAND_OUT ARAD_SW_DB_PMF_DB_INFO             *db_info
  );

uint32
  arad_sw_db_pmf_db_prio_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32             db_ndx,
    SOC_SAND_OUT uint32             *prio
  );


uint32
  arad_sw_db_pmf_db_prog_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                     db_ndx,
    SOC_SAND_OUT uint32                    *exist_progs
  );


uint32
  arad_sw_db_pmf_db_prog_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                     db_ndx,
    SOC_SAND_IN uint32                    exist_progs
  );

uint32
  arad_sw_db_pmf_db_prog_add(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                     db_ndx,
    SOC_SAND_IN uint32                    prog_id
  );


/*
 * PMF PSL info {
 */

uint32 
  arad_sw_db_level_info_get(
      SOC_SAND_IN  int             unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32             level_indx,
      SOC_SAND_IN  uint32             is_tm,
      SOC_SAND_OUT  ARAD_SW_DB_PMF_PSL_LEVEL_INFO  *level_info
  );

uint32 
  arad_sw_db_level_info_set(
      SOC_SAND_IN  int             unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32             level_indx,
      SOC_SAND_IN  uint32             is_tm,
      SOC_SAND_IN  ARAD_SW_DB_PMF_PSL_LEVEL_INFO  *level_info
  );


uint32 
  arad_sw_db_pmf_psl_init_info_set(
      SOC_SAND_IN  int             unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_OUT  ARAD_PMF_SEL_INIT_INFO  *init_info
  );

uint32 
  arad_sw_db_pmf_psl_init_info_get(
      SOC_SAND_IN  int             unit,
      SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
      SOC_SAND_OUT  ARAD_PMF_SEL_INIT_INFO  *init_info
  );

uint32
  arad_sw_db_pmf_psl_pgm_dbs_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                   pgm_ndx,
    SOC_SAND_IN uint32                    pgms_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)]
  );


uint32
  arad_sw_db_pmf_psl_pgm_dbs_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                   pgm_ndx,
    SOC_SAND_OUT uint32                   pgms_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)]
  );


uint32
  arad_sw_db_pmf_psl_pfg_dbs_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                   pfg_ndx,
    SOC_SAND_IN uint32                    pfgs_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)]
  );


uint32
  arad_sw_db_pmf_psl_pfg_dbs_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                   pfg_ndx,
    SOC_SAND_OUT uint32                   pfgs_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)]
  );

uint32
  arad_sw_db_pmf_psl_default_dbs_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN uint32                    default_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)]
  );


uint32
  arad_sw_db_pmf_psl_default_dbs_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_OUT uint32                   default_db_pmb[ARAD_BIT_TO_U32(ARAD_SW_DB_PMF_NOF_DBS)]
  );


uint32
  arad_sw_db_pmf_psl_new_db_id_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                   new_db_id
  );

uint32
  arad_sw_db_pmf_psl_new_db_id_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_OUT  uint32                   *new_db_id
  );


/*
 * PMF PSL info }
 */


/*
 * FP info }
 */
uint32
  arad_sw_db_fp_db_info_set(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32              db_ndx,
    SOC_SAND_IN  ARAD_PP_FP_DATABASE_INFO *db_info
  );

uint32
  arad_sw_db_pmf_psl_pfg_tms_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN uint32                    pfg_tm_bmp[ARAD_BIT_TO_U32(ARAD_PMF_NOF_GROUPS)]
  );


uint32
  arad_sw_db_pmf_psl_pfg_tms_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_OUT uint32                   pfg_tm_bmp[ARAD_BIT_TO_U32(ARAD_PMF_NOF_GROUPS)]
  );

uint32
  arad_sw_db_fp_db_info_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32               db_ndx,
    SOC_SAND_OUT ARAD_PP_FP_DATABASE_INFO *db_info
 );

uint32
  arad_sw_db_fp_db_entries_set(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32              db_ndx,
    SOC_SAND_IN  ARAD_SW_DB_FP_ENTRY  *db_entries
  );

uint32
  arad_sw_db_fp_db_entries_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32               db_ndx,
    SOC_SAND_OUT ARAD_SW_DB_FP_ENTRY   *db_entries
 );

uint32
  arad_sw_db_fp_db_entry_bitmap_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE     stage,
    SOC_SAND_IN  uint32                     bank_index,
    SOC_SAND_IN  uint32                     entry_index,
    SOC_SAND_IN  uint8                      is_used
  );

uint32
  arad_sw_db_fp_db_entry_bitmap_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE     stage,
    SOC_SAND_IN  uint32                     bank_index,
    SOC_SAND_IN  uint32                     entry_index,
    SOC_SAND_OUT uint8                      *is_used
  );
 
uint32
  arad_sw_db_fp_db_entry_bitmap_clear(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE     stage
  );

uint32
  arad_sw_db_fp_db_udf_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                     udf_ndx,
    SOC_SAND_IN  ARAD_PMF_CE_QUAL_INFO        *qual_info
  );

uint32
  arad_sw_db_fp_db_udf_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                      udf_ndx,
    SOC_SAND_OUT ARAD_PMF_CE_QUAL_INFO         *qual_info
 );

uint32
  arad_sw_db_fp_inner_eth_nof_tags_set(
    SOC_SAND_IN int                                unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN uint32                             pfg_ndx,
    SOC_SAND_IN uint32                             inner_eth_nof_tags
  );

uint32
  arad_sw_db_fp_inner_eth_nof_tags_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pfg_ndx,
    SOC_SAND_OUT uint32                            *inner_eth_nof_tags
  );

uint32
  arad_sw_db_fp_key_change_size_set(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN ARAD_FP_DATABASE_STAGE stage,
    SOC_SAND_IN uint8                  key_change_size
  );

uint32
  arad_sw_db_fp_key_change_size_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_OUT uint8                             *key_change_size
  );




uint32
  arad_sw_db_pfg_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE    stage,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_IN  SOC_TMC_PMF_PFG_INFO      *pfg_info
  );

uint32
  arad_sw_db_pfg_info_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_OUT SOC_TMC_PMF_PFG_INFO      *pfg_info
  );

uint32
  arad_sw_db_eg_encap_prge_tbl_nof_dynamic_entries_get(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint32 *nof_dynamic_members
  );

uint32
  arad_sw_db_eg_encap_prge_tbl_overlay_arp_entries_base_index_get(
     SOC_SAND_IN  int  unit, 
     SOC_SAND_OUT uint32 *overlay_arp_entry_start_index
  );


/*
 * SW DB multi-sets {
 */
uint32
  arad_sw_db_buffer_set_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_INOUT  uint8                           *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_IN  uint8                              *data
  );

uint32
  arad_sw_db_buffer_get_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_IN  uint8                              *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_OUT uint8                              *data
  );

uint32
  arad_sw_db_multiset_add(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type, /* ARAD_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32                 *val,
    SOC_SAND_OUT  uint32                *data_indx,
    SOC_SAND_OUT  uint8               *first_appear,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE    *success
  );

uint32
  arad_sw_db_multiset_remove(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type, /* ARAD_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32                 *val,
    SOC_SAND_OUT  uint32                *data_indx,
    SOC_SAND_OUT  uint8               *last_appear
  );

uint32
  arad_sw_db_multiset_lookup(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       multiset_type, /* ARAD_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32        *val,
    SOC_SAND_OUT  uint32       *data_indx,
    SOC_SAND_OUT  uint32       *ref_count
  );

uint32
  arad_sw_db_multiset_add_by_index(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type, /* ARAD_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32                 *val,
    SOC_SAND_OUT  uint32                data_indx,
    SOC_SAND_OUT  uint8               *first_appear,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE    *success
  );

uint32
  arad_sw_db_multiset_remove_by_index(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type, /* ARAD_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32                 data_indx,
    SOC_SAND_OUT  uint8               *last_appear
  );

uint32
  arad_sw_db_multiset_clear(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type /* ARAD_SW_DB_MULTI_SET */
  );

/*
 * } Configuration
 */

/*********************************************************************************************
* }
* arad interrupt
* {
*********************************************************************************************/

uint8
  arad_sw_db_interrupt_mask_on_get(
    SOC_SAND_IN  int                        unit
  );

void
  arad_sw_db_interrupt_mask_on_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint8                        val
  );

/* } */


void arad_sw_db_sw_dump(int unit);

/*
Create and allocate device x port to system physical port mapping.
The input should be NULL and will contain a pointer to the mapping data
structure on successful return.
*/
uint32 arad_sw_db_modport2sysport_create(
    SOC_SAND_IN int unit
  );

/*
Destroy and deallocate device x port to system physical port mapping data structure.
The input will be assigned NULL on successful return.
*/
uint32 arad_sw_db_modport2sysport_destroy(
    SOC_SAND_IN int unit
  );

/*
Set a device x port to system physical port mapping.
Performs allocation inside the data structure if needed.
*/
uint32 arad_sw_db_modport2sysport_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 fap_id,
    SOC_SAND_IN uint32 fap_port_id,
    SOC_SAND_IN ARAD_SYSPORT sysport
  );

/*
Get a device x port to system physical port mapping.
If the mapping does not exist, the value of ARAD_NOF_SYS_PHYS_PORTS is returned
*/
uint32 arad_sw_db_modport2sysport_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 fap_id,
    SOC_SAND_IN uint32 fap_port_id,
    SOC_SAND_OUT ARAD_SYSPORT *sysport
  );

/*
Get a reverse system physical port to device x port mapping.
Works by searching the mapping till finding the system physical port.
If the mapping does not exist, the value of ARAD_SW_DB_MODPORT2SYSPORT_REVERSE_GET_NOT_FOUND is returned
*/
uint32 arad_sw_db_modport2sysport_reverse_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_SYSPORT sysport,
    SOC_SAND_OUT uint32 *fap_id,
    SOC_SAND_OUT uint32 *fap_port_id
  );

/*
Remove a device x port to system physical port mapping.
*/
uint32 arad_sw_db_modport2sysport_remove(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 fap_id,
    SOC_SAND_IN uint32 fap_port_id
  );


/* Get the hardware queue type mapped to from the user queue type. Returns ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE in mapped_q_type if not found */
uint32
  arad_sw_db_queue_type_map_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint8          user_q_type,  /* input user queue type (predefined type or user defined allocated type) */
    SOC_SAND_OUT uint8*         mapped_q_type /* output hardware queue type, 0 if not mapped */
  );

/*
 * Get the hardware queue type mapped to from the user queue type, allocating it if it was not allocated before.
 * Returns ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE in mapped_q_type if mapping is not possible since all hardware types (credit request profiles) are used.
 * If given a predefined queue type, will just return it as output as it does not use dynamic allocation.
 */
uint32
  arad_sw_db_queue_type_map_get_alloc(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint8          user_q_type,  /* input user queue type (predefined type or user defined allocated type) */
    SOC_SAND_OUT uint8*         mapped_q_type /* output hardware queue type, 0 if not mapped */
  );

/* Get the user queue type mapped from the given hardware queue type. */
uint32
  arad_sw_db_queue_type_map_reverse_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint8          mapped_q_type,  /* input hardware queue type, 0 if not mapped */
    SOC_SAND_OUT uint8*         user_q_type     /* output user queue type (predefined type or user defined allocated type) */
  );

uint32
  arad_sw_db_is_port_valid_get(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  uint32            base_q_pair,
                int               core,
   SOC_SAND_OUT uint8             *is_valid
   );
uint32
  arad_sw_db_is_port_valid_set(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  uint32            base_q_pair,
                int               core,
   SOC_SAND_IN  uint8             is_valid
   );
uint32
  arad_sw_db_egq_port_rate_set(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  uint32            base_q_pair,
                int               core,
   SOC_SAND_IN  uint32            rate
   );
uint32
  arad_sw_db_egq_port_rate_get(
   SOC_SAND_IN   int               unit,
   SOC_SAND_IN   uint32            base_q_pair,
                 int               core,
   SOC_SAND_OUT  uint32           *rate
   );
uint32
  arad_sw_db_sch_port_rate_set(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  uint32            base_q_pair,
                int               core,
   SOC_SAND_IN  uint32            rate
   );
uint32
  arad_sw_db_sch_port_rate_get(
   SOC_SAND_IN  int               unit,
   SOC_SAND_IN  uint32            base_q_pair,
                int               core,
   SOC_SAND_OUT uint32            *rate
   );
uint32 
    arad_sw_db_tm_queue_to_rate_class_mapping_is_simple_set(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN uint8                        queue_to_rate_class_mapping_is_simple
  );
uint32
    arad_sw_db_tm_queue_to_rate_class_mapping_is_simple_get(
       SOC_SAND_IN  int                      unit,
       SOC_SAND_OUT uint8*                      queue_to_rate_class_mapping_is_simple
  );
uint32 
    arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_set(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      rate_class,
       SOC_SAND_IN uint32                       ref_count
  );
uint32 
    arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_get(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      rate_class,
       SOC_SAND_OUT uint32*                     ref_count
  );
uint32 
    arad_sw_db_tm_queue_to_rate_class_mapping_ref_count_exchange(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      old_rate_class,
       SOC_SAND_IN  uint32                      new_rate_class
       );

#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __ARAD_SW_DB_INCLUDED__*/

#endif
