/* $Id: petra_sw_db.h,v 1.10 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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


#ifndef __SOC_PETRA_SW_DB_INCLUDED__
/* { */
#define __SOC_PETRA_SW_DB_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_api_end2end_scheduler.h>
#include <soc/dpp/Petra/petra_ofp_rates.h>
#include <soc/dpp/Petra/petra_api_mgmt.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt.h>
#include <soc/dpp/Petra/petra_api_auto_queue_flow_mgmt.h>
#include <soc/dpp/Petra/petra_api_serdes.h>
#include <soc/dpp/Petra/petra_api_serdes_utils.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_api_nif.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_api_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_pgm_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_ce.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_db.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_fem_tag.h>
#include <soc/dpp/Petra/PB_TM/pb_egr_prog_editor.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_tbls.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/SAND/Utils/sand_hashtable.h>
#include <soc/dpp/SAND/Utils/sand_hashtable.h>
#include <soc/dpp/SAND/Utils/sand_sorted_list.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#endif

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* Multicast-ID sum for ingress and egress 2*SOC_PETRA_MULT_ID_MAX.*/
#define SOC_PETRA_MULT_TABLE_MAX      (SOC_PETRA_MULT_TABLE_SIZE-1)
#define SOC_PETRA_MULT_TABLE_SIZE     (32*1024)

#define SOC_PETRA_FABRIC_LINKS_UINT32S  SOC_SAND_DIV_ROUND_UP(SOC_PETRA_FBR_NOF_LINKS, SOC_SAND_NOF_BITS_IN_UINT32)

#ifdef LINK_PB_LIBRARIES
#define SOC_PB_NOF_IP_TCAM_ENTRY_TYPES       (5)
#define SOC_PB_EGR_NOF_TM_PORT_PROFILES      (12)
#define SOC_PB_PMF_NOF_HEADER_PROFILES       (8)
#define SOC_PB_PMF_NOF_PP_PORT_PROFILES      (8)
#define SOC_PB_PMF_NOF_PGM_PROFILES          (32)
#define SOC_PB_EGR_ACL_NOF_DBS               (8)
#define SOC_PB_PMF_NOF_LKP_PROFILE_IDS       (8)
#define SOC_PB_PMF_NOF_TM_PPPS               (4)
#endif

/* } */

/*************
 *  MACROS   *
 *************/
/* { */
#define SOC_PETRA_SW_DB_INIT_DEFS                                     \
  SOC_SAND_RET                                                        \
    soc_sand_ret = SOC_SAND_OK

#define SOC_PETRA_SW_DB_FIELD_SET(unit, field_name, val)             \
  if (Soc_petra_sw_db.soc_petra_device_sw_db[unit] == NULL)          \
  {                                                                       \
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 1, exit);                       \
  }                                                                       \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      &(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->field_name),   \
      val,                                                                \
      sizeof(*val)                                                        \
    );                                                                    \
  SOC_SAND_CHECK_FUNC_RESULT(soc_sand_ret, 2, exit);

#define SOC_PETRA_SW_DB_FIELD_GET(unit, field_name, val)         \
  if (Soc_petra_sw_db.soc_petra_device_sw_db[unit] == NULL)          \
  {                                                               \
    return SOC_SAND_ERR;                                              \
  }                                                               \
  soc_sand_ret = soc_sand_os_memcpy(                                      \
      val,                                                        \
      &(Soc_petra_sw_db.soc_petra_device_sw_db[unit]->field_name),   \
      sizeof(*val)                                                \
    );                                                            \
  return soc_sand_ret;




/* $Id: petra_sw_db.h,v 1.10 Broadcom SDK $
 * The following defines refer to certain indicators in the multicast sw-db.
 * These values are inputs of the 'next-unoccupied-list' data-base and help
 * determine the status of the table entries. Inputing them there is possible
 * due to the construction of the db-table where an entry that isn't occupied
 * holds the next unoccupied entry, on the other hand, an entry that IS occupied
 * has different statuses, as following:
 * 1 - pointing to NULL, meaning this is the last unoccupied entry in the
 *     'next-unoccupied-list' data-base.
 * 2 - first entry indicator - this is the beginning of a mc-group (mc-id).
 * 3 - ingress occupied entry - this entry is part of an ingress mc-group.
 * 4 - egress occupied entry - this entry is part of an egress mc-group.
 */
/* 1 */
#define SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER 0xFFFF
/* 2 */
#define SOC_PETRA_SW_DB_MULTICAST_FIRST_ENTRY_INDICATOR 0x8000
/* 3 */
#define SOC_PETRA_SW_DB_MULTICAST_ING_OCCUPIED_ENTRY_INDICATOR 0xA000
/* 4 */
#define SOC_PETRA_SW_DB_MULTICAST_EG_OCCUPIED_ENTRY_INDICATOR 0xB000
/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

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
}  SOC_PETRA_SW_DB_DEV_EGR_RATE;

typedef struct
{
  SOC_PETRA_SW_DB_DEV_EGR_RATE
    rates[SOC_PETRA_NOF_FAP_PORTS];
  uint32
    sch_mal_rate;
  uint32
    nof_calcal_instances;

}  SOC_PETRA_SW_DB_DEV_EGR_MAL;

typedef struct
{

  SOC_PETRA_SW_DB_DEV_EGR_MAL
    olp;
  SOC_PETRA_SW_DB_DEV_EGR_MAL
    cpu;
  SOC_PETRA_SW_DB_DEV_EGR_MAL
    rcy;
#ifdef LINK_PB_LIBRARIES
  SOC_PETRA_SW_DB_DEV_EGR_MAL
    mal[SOC_SAND_MAX(SOC_PETRA_NOF_MAC_LANES, SOC_PB_NOF_MAC_LANES)];
#else
  SOC_PETRA_SW_DB_DEV_EGR_MAL
    mal[SOC_PETRA_NOF_MAC_LANES];
#endif
  uint32
    calcal_length;
 /*
  *  Indicates if the calendars are written after their computation
  *  If True, then the calendars are written into the device
  */
  uint8
    update_device;
 /*
  *  Indicates if the update device indication just changed to force the
  *  recalculation
  */
  uint8
    update_dev_changed;
}  SOC_PETRA_SW_DB_DEV_EGR_PORTS;

typedef struct
{
  uint32 max_expected_port_rate[SOC_PETRA_SCH_MAX_PORT_ID+1];
  uint32 accumulated_grp_port_rate[SOC_PETRA_SCH_NOF_GROUPS];
}  SOC_PETRA_SW_DB_DEVICE_SCH;

typedef struct
{
  uint16
    next_unoccupied_ptr;

  uint16
    backwards_ptr;
}  SOC_PETRA_SW_DB_MULTICAST_ENTRY;

typedef struct
{
  uint32
    nof_unoccupied;

  uint16
    unoccupied_list_head;

  SOC_PETRA_SW_DB_MULTICAST_ENTRY
    soc_petra_mult_array[SOC_PETRA_MULT_TABLE_SIZE];
}  SOC_PETRA_SW_DB_MULTICAST;

typedef struct
{
  uint8
    auto_valid;

  SOC_PETRA_AQFM_SYSTEM_INFO
    system_info;

  SOC_PETRA_AQFM_AGG_TABLE_INFO
    agg_table_info[SOC_PETRA_NOF_FAP_PORTS];

}  SOC_PETRA_SW_DB_AUTO_SCHEME;

typedef struct
{
  /*
   *  Soc_petra device sub-type
   */
  SOC_PETRA_REVISION_SUB_TYPE_ID device_sub_type;
  /*
   *  Enable Packet Processing features. Valid only for Soc_petra
   *  revisions that support Packet Processing.
   */
  uint8 pp_enable;
  /*
   *  If TRUE, fap20 devices exist in the system. This imposes
   *  certain limitations on Soc_petra behavior (e.g. fabric
   *  cells must be fixed size, fap20-compatible fabric header must be used etc.).
   */
  uint8 is_fap20_in_system;
  /*
   *  If TRUE, fap21 devices exist in the system. This imposes
   *  certain limitations on Soc_petra behavior (e.g. fabric
   *  cells must be fixed size etc.).
   */
  uint8 is_fap21_in_system;
  /*
   *  If TRUE, soc_petra revision A1 or A0 devices exist in the
   *  system.
   */
  uint8 is_a1_or_below_in_system;

  /*
   *  If TRUE, the system uses SOC_SAND_FE200 fabric.
   */
  uint8 is_fe200_fabric;
  /*
   *  If TRUE,variable cell size will be used. This
   *  configuration is invalid for SOC_SAND_FE200 systems.
   */
  uint8 is_fabric_variable_cell_size;
  /*
   *  If TRUE, fabric segmentation will be performed to
   *  improve the fabric performance in fixed size cell
   *  systems.
   */
  uint8 is_fabric_packet_segmentation;
  /*
   *  Out of total of 15 SerDes quartets, two (one per
   *  internal NIF Group consisting of 4 MAL-s) may be
   *  assigned to either Network or Fabric interfaces.
   */
  uint8 is_combo_nif[SOC_PETRA_COMBO_NOF_QRTTS];
  /*
   *  SerDes Reference Clocks. Three ref-clocks are
   *  configured: 1. Towards NIF 2. Towards Fabric MAC3.
   *  Shared (Combo, either to NIF or MAC) - either to NIF or
   *  MAC value, according to the shared_quartet
   *  configuration.
   */
  SOC_PETRA_MGMT_SRD_REF_CLKS ref_clocks_conf;
  /*
   *  Enable/disable statistics tag.
   */
  uint8 stag_enable;
  /*
   *  This configuration affects the maximal number of Egress
   *  MC groups that can be opened. Can only be enabled (TRUE)
   *  when working with SOC_SAND_FE600 (not SOC_SAND_FE200 or mesh). Note! If
   *  enabled, the FAP-IDs range in the system is limited to 0
   *  - 511.
   */
  uint8 egr_mc_16k_groups_enable;
  /*
   *	TDM traffic mode and FTMH format configuration
   */
  SOC_PETRA_MGMT_TDM_MODE tdm_mode;

#ifdef LINK_PB_LIBRARIES
  /*
   *  If TRUE, Soc_petra Rev-A devices exist in the system. This imposes
   *  certain limitations on the device behavior (e.g. PPH is in
   *  Soc_petra-A compatible mode).
   */
  uint8 is_petra_rev_a_in_system;

  /*
   *  FTMH load balancing mode.
   */
  SOC_PB_MGMT_FTMH_LB_EXT_MODE ftmh_lb_ext_mode;

  /*
   *  If TRUE, the system uses SOC_SAND_FE200 fabric.
   */
  uint8 is_fe1600_in_system;

  uint8 add_pph_eep_ext;
#endif

}  SOC_PETRA_SW_DB_OP_MODE;

typedef struct
{
  /*
   *  Soc_petra device type (Soc_petra-A or Soc_petra-B).
   */
  uint32
    ver;
}  SOC_PETRA_SW_DB_VER;


typedef struct
{
  /*
   *  Number of FAP ports that are members of the Fat Pipe.
   *  0 means Fat pipe is disabled.
   */
  uint32
    nof_fap_ports;
}  SOC_PETRA_SW_DB_FAT_PIPE;


typedef struct
{
 /*
  *  to identify whether the size of the LAG is zero or one.
  */
  uint8
    in_use[SOC_PETRA_NOF_LAG_GROUPS];

#ifdef LINK_PB_LIBRARIES
  uint32
    local_to_sys[SOC_PETRA_NOF_LOCAL_PORTS];
#endif
}  SOC_PETRA_SW_DB_LAGS_INFO;

typedef struct
{
  /*
   *  This configuration is per SerDes connection direction:
   *    0 - Connection direction  receive (RX)
   *    1 - Connection direction  transmit (TX)
   *
   *  Per SerDes lane, indicates (enabled == TRUE) if configured
   *  to SOC_PETRA_SRD_PRBS_MODE_POLY_23_SWAP_POLARITY pattern.
   *  In this configuration, the SerDes polarity is inverted
   *  upon PRBS start, and reverted (second invert) upon PRBS stop.
   */
  uint8 enabled;

  /*
   *  per SerDes connection direction:
   *  Per SerDes lane, indicates (orig_is_swap == TRUE) if
   *  prior to PRBS configuration,
   *  the SerDes swap polarity was enabled.
   */
  uint8 orig_is_swap;

}  SOC_PETRA_SW_DB_SRD_PRBS_23_SWAP;

typedef struct
{
  /*
   *  Maximal data rate (Mbps) per SerDes quartet.
   */
  uint32
    qrtt_max_rate[SOC_PETRA_SRD_NOF_QUARTETS];

  /*
   *  PRBS 23-polynom-mode with swap polarity
   */
  SOC_PETRA_SW_DB_SRD_PRBS_23_SWAP prbs_23_swap[2][SOC_PETRA_SRD_NOF_LANES];

  /*
   *  Handling swap polarity (set according to 23_swap or revert to original)
   *  may sometimes be omitted, e.g. during initialization.
   *  This is used to decrease execution time
   */
  uint8 is_swap_unhandled;

  uint8 is_irq_disable;
}  SOC_PETRA_SW_DB_SERDES;



typedef struct
{
 /*
  *  Port header processing type
  *   - 0: SOC_PETRA_PORT_DIRECTION_INCOMING
  *   - 1: SOC_PETRA_PORT_DIRECTION_OUTGOING
  */
  SOC_PETRA_PORT_HEADER_TYPE
    headers[2];
 /*
  * ITMH header of the port.
  */
  SOC_PETRA_PORTS_ITMH
    itmh;

  uint8
    is_nif_lpbck_on;
 /*
  *  whether this port was previously saved.
  */
  uint8
    saved_valid;

}  SOC_PETRA_SW_DB_LBG_PORT;

typedef struct
{
 /*
  *  The saved ports status
  */
  SOC_PETRA_SW_DB_LBG_PORT
    ports_state[SOC_PETRA_NOF_FAP_PORTS];

  uint32  packet_size;
}  SOC_PETRA_SW_DB_LBG;

typedef struct
{
  /*
   *  The cell ident of the source routed cell
   */
  uint16  current_cell_ident;
}  SOC_PETRA_SW_DB_CELL;

typedef struct
{
  /*
   *  The dram size per interface (in MBytes)
   */
  uint32  dram_size_per_interface_mbyte;

  /*
   *  The DLL periodic training sequence register
   */
  uint32  dll_periodic_trng_reg;

  /*
   *	The DRAM configuration
   */
  SOC_PETRA_HW_DRAM_CONF_PARAMS dram_conf;

  /*
   *	The DRAM type
   */
  SOC_PETRA_DRAM_TYPE dram_type;

}  SOC_PETRA_SW_DB_DRAM;

typedef struct
{
  /*
   *  The DLL periodic training sequence register
   */
  uint32                  dll_periodic_trng_reg;
  SOC_PETRA_HW_QDR_SIZE_MBIT    size;
  SOC_PETRA_HW_QDR_PROTECT_TYPE protection_type;
  uint8                 enable;
}  SOC_PETRA_SW_DB_QDR;

typedef struct
{
  /*
   *  A bit is set if the appropriate fabric
   *  link is double-rate (Above 3.125Gbps)
   */
  uint32  is_link_double_rate[SOC_PETRA_FABRIC_LINKS_UINT32S];

}  SOC_PETRA_SW_DB_MAC;

typedef enum
{
  /*
   *  Set the Egress Port Profile per TM-Port, i.e. which
   *  Egress Editor processing the packet must perform.
   *  Max: 12 profiles.
   */
  SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR = 0,
  /*
   *  Set the PMF-Program system header profile
   *  Max: 8 profiles.
   */
  SOC_PB_SW_DB_MULTI_SET_PMF_HDR_PROFILE = 1,
  /*
   *  Set the PMF Port Profile
   *  Max: 8 profiles.
   */
  SOC_PB_SW_DB_MULTI_SET_PMF_PORT_PROFILE = 2,
  /*
   *  Set the PMF Program Profile
   *  Max: 32 profiles.
   */
  SOC_PB_SW_DB_MULTI_SET_PMF_PGM_PROFILE = 3,
  /*
   *  Set the stacking port pruning bitmap
   *  Max: 2 bitmaps.
   */
  SOC_PB_SW_DB_MULTI_SET_STK_PRUNE_BMP = 4,

  SOC_PB_NOF_SW_DB_MULTI_SETS
} SOC_PB_SW_DB_MULTI_SET;


#ifdef LINK_PB_LIBRARIES
/*
 *	Soc_petra-B only
 *  SW-DB
 */


typedef struct
{
  SOC_PB_NIF_SYNCE_MODE mode;
}  SOC_PB_SW_DB_SYNCE;

typedef struct
{
  /*
   *	Trap code
   */
  uint32 trap_code;

  /*
   *	Strength
   */
  uint32 strength[2];

} SOC_PB_SW_DB_LBG_PP_PORT_STRENGTH;

typedef struct
{
  uint32
    pmf_pgm[SOC_PB_PMF_NOF_LKP_PROFILE_IDS][SOC_PB_PMF_NOF_TM_PPPS];

  /*
   *	Strength configuration
   */
  SOC_PB_SW_DB_LBG_PP_PORT_STRENGTH
    strength;

  /*
   * Header type
   */
  SOC_PETRA_PORT_HEADER_TYPE
    header_type;

}  SOC_PB_SW_DB_PORT;

typedef struct
{
 /*
  *  Port header processing type
  *   - 0: SOC_PETRA_PORT_DIRECTION_INCOMING
  *   - 1: SOC_PETRA_PORT_DIRECTION_OUTGOING
  */
  SOC_PETRA_PORT_HEADER_TYPE
    headers[2];

  uint8
    is_nif_lpbck_on;
 /*
  *  whether this port was previously saved.
  */
  uint8
    saved_valid;

  /*
   *	Its PP-Port
   */
  uint32 pp_port;

}  SOC_PB_SW_DB_LBG_PORT;

typedef struct
{
  /*
   *	Trap data entry
   */
  SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA data;
  SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA snoop;

  /*
   *	Profile previously saved
   */
  uint8 saved_valid;

  /*
  *	Previous Trap code (per PP-Port)
   */
  uint32 trap_code;

} SOC_PB_SW_DB_LBG_PP_PORT_TRAP;

typedef struct
{
  /*
   *	PP-Port configuration
   */
  SOC_PB_PORT_PP_PORT_INFO info;

  /*
   *	Trap configuration
   */
  SOC_PB_SW_DB_LBG_PP_PORT_TRAP trap;

}  SOC_PB_SW_DB_LBG_PP_PORT;

typedef struct
{
 /*
  *  The saved ports status
  */
  SOC_PB_SW_DB_LBG_PORT
    ports_state[SOC_PETRA_NOF_FAP_PORTS];

  /*
   *	The saved PP-Port status
   */
  SOC_PB_SW_DB_LBG_PP_PORT
    profile_state[SOC_PB_PORT_NOF_PP_PORTS];

  uint32  packet_size;
}  SOC_PB_SW_DB_LBG;

typedef struct
{
  uint8               valid;
  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size;
  SOC_SAND_OCC_BM_PTR         entries_used;
  uint32               nof_entries_free;
} SOC_PB_SW_DB_TCAM_BANK;

typedef struct
{
  uint8               valid;
  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size;
  uint32               prefix_size;
  SOC_PB_TCAM_DB_PRIO_MODE    prio_mode;
  uint8               bank_used[SOC_PB_TCAM_NOF_BANKS];
  SOC_PB_TCAM_PREFIX          prefix[SOC_PB_TCAM_NOF_BANKS];
  SOC_SAND_SORTED_LIST_INFO   priorities;
  SOC_SAND_HASH_TABLE_INFO    entry_id_to_location;
  SOC_PB_TCAM_LOCATION        *location_tbl;
} SOC_PB_SW_DB_TCAM_DB;

typedef struct
{
  SOC_PB_SW_DB_TCAM_BANK bank[SOC_PB_TCAM_NOF_BANKS];
  SOC_PB_SW_DB_TCAM_DB   tcam_db[SOC_PB_TCAM_MAX_NOF_LISTS];
} SOC_PB_SW_DB_TCAM;

typedef struct
{
  uint8             valid;
  uint32             tcam_db;
  uint32             cycle;
  uint8             uniform_prefix;
  uint32             min_banks;
  SOC_PB_TCAM_MGMT_SIGNAL   callback;
  uint32              user_data;
  SOC_PB_TCAM_ACCESS_DEVICE access_device;
} SOC_PB_SW_DB_TCAM_ACCESS_PROFILE;

typedef struct
{
  uint32 access_profile[SOC_PB_TCAM_NOF_CYCLES];
  uint32 prefix_db[SOC_PB_TCAM_NOF_PREFIXES];
  uint32 nof_dbs;
} SOC_PB_SW_DB_TCAM_MANAGED_BANK;

typedef struct
{
  SOC_PB_SW_DB_TCAM_ACCESS_PROFILE profiles[SOC_PB_TCAM_MAX_NOF_LISTS];
  SOC_PB_SW_DB_TCAM_MANAGED_BANK   banks[SOC_PB_TCAM_NOF_BANKS];
  uint32                    access_profile_in_access_device[SOC_PB_TCAM_NOF_CYCLES][SOC_PB_TCAM_NOF_ACCESS_DEVICES];
} SOC_PB_SW_DB_TCAM_MGMT;

typedef struct
{
  uint8 explicit_address_set;
  uint8 explicit_bank_sel_set;
  uint32  db_id[SOC_PB_PMF_NOF_LKP_PROFILE_IDS][SOC_PB_TCAM_NOF_CYCLES][SOC_PB_PMF_LOW_LEVEL_NOF_DBS_PER_LOOKUP_PROFILE_MAX];
}  SOC_PB_SW_DB_TCAM_DB_OLD;

typedef struct
{
 /*
  * banks x cycle in use for this TCAM use.
  * For each DB, indicates which bank is used
  */
  uint8 resources[SOC_PB_TCAM_NOF_BANKS];

  /*
   *	Prefix per bank. Range: 0 - 15.
   */
  uint8 prefix[SOC_PB_TCAM_NOF_BANKS];

  /*
   *	Number of significant bits in each bank.
   *  Range: 0 - 4.
   */
  uint8 nof_bits[SOC_PB_TCAM_NOF_BANKS];

  /*
   *	Number of entries for this database in each bank.
   *  Range: 0 - 512.
   */
  uint16 nof_entries[SOC_PB_TCAM_NOF_BANKS];

  /*
   * TCAM Key source - for PMF
   */
  SOC_PB_PMF_TCAM_KEY_SRC key_src;

  /*
   * FEMs with this Database input - for PMF
   */
  uint8 is_in_fem[SOC_PB_PMF_LOW_LEVEL_NOF_FEMS][SOC_PB_TCAM_NOF_CYCLES];

}  SOC_PB_SW_DB_TCAM_USE_INFO;


typedef struct
{
 /*
  * Users of this bank
  */
  SOC_PB_TCAM_USER users[SOC_PB_TCAM_MAX_USERS_PER_BANK];
  /*
   *	Entry size of the bank
   */
  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size;
 /*
  * Entries of the bank used/unused (Bitmap).
  */
  SOC_SAND_OCC_BM_PTR entries_use;
  /*
   * Used prefixes
   */
  uint8 is_prefix_used[SOC_PB_TCAM_NOF_PREFIXES];
  /*
   * FP Database in this bank*cycle
   */
  uint32 fp_db[SOC_PB_TCAM_NOF_CYCLES];
  
}  SOC_PB_SW_DB_TCAM_BANK_INFO;

typedef struct
{
  SOC_PB_SW_DB_TCAM_BANK_INFO
    banks[SOC_PB_TCAM_NOF_BANKS];
  SOC_PB_SW_DB_TCAM_USE_INFO
    uses_info[SOC_PB_TCAM_MAX_NOF_LISTS];
}  SOC_PB_SW_DB_TCAM_MGMT_INFO;

typedef struct
{
  SOC_SAND_HASH_TABLE_INFO route_key_to_entry_id;
} SOC_PB_SW_DB_FRWRD_IP;

typedef struct
{
  /* Indicates nof vlan bitmaps in egress multicast group X */
  uint32
    eg_mult_nof_vlan_bitmaps[SOC_PETRA_MULT_NOF_MULTICAST_GROUPS];

}  SOC_PB_SW_DB_MULTICAST;

typedef struct
{
  SOC_SAND_MULTI_SET_INFO egr_editor_profiles_multi_set;
  SOC_SAND_MULTI_SET_INFO egr_editor_stk_prune_bmps_set;

  /*
   * Profile of this HW index
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE index_profile[SOC_PB_EGR_NOF_TM_PORT_PROFILES];

} SOC_PB_SW_DB_EGR_EDITOR;

typedef struct
{
  /*
   * Type of each Egress ACL Database
   */
  SOC_PB_EGR_ACL_DB_TYPE db_type[SOC_PB_EGR_ACL_NOF_DBS];

  /*
  *  SW Database-ID of the Egress Databases (for FP)
   */
  uint32 sw_db_id[SOC_PB_EGR_ACL_NOF_DBS];


} SOC_PB_SW_DB_EGR_ACL;

typedef struct
{
  /*
   *  Used by instruction allocation code
   */
  uint8 is_used;
  /*
   *  The qualifier copied by this instruction
   */
  uint32 db_id_ndx;

  uint8 is_used_in_cycle_1;
  /*
   * CE validity
   */
  uint8 is_valid;

  /*
   * CE IRPP or header extraction
   */
  uint8 is_irpp;

  uint32 header_offset_select;

  uint32 nibble_field_offset;

  uint32 bit_count;

} SOC_PB_SW_DB_PMF_CE;

typedef struct
{
  /*
   * PP-Port info encoded
   */
  uint32 pp_port_info_encoded;

  /*
   * PFG index
   */
  uint32 pfg_ndx;

} SOC_PB_SW_DB_PMF_PGM_PROFILE;

typedef struct
{
  /*
   * System header Profile multiset of the PMF Programs
   */
  SOC_SAND_MULTI_SET_INFO header_profiles_multi_set;
  /*
   * PMF PP Port Profile multiset of the PP Ports
   */
  SOC_SAND_MULTI_SET_INFO pp_port_profiles_multi_set;
  /*
   * Profile of this HW index
   */
  uint32 pp_port_index_profile[SOC_PB_PMF_NOF_PP_PORT_PROFILES];
  /*
   * PMF Program Profile multiset
   */
  SOC_SAND_MULTI_SET_INFO pgm_profiles_multi_set;
  /*
   * Attributes of this HW index
   */
  SOC_PB_SW_DB_PMF_PGM_PROFILE pgm_profile[SOC_PB_PMF_NOF_PGM_PROFILES];
  /*
   * PMF-Program Copy Engine instructions
   */
  SOC_PB_SW_DB_PMF_CE pgm_ce[SOC_PB_PMF_NOF_LKP_PROFILE_IDS][SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX+1][SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX+1];

  /*
   * Ethernet PMF-Programs
   */
  uint8 is_pmf_pgm_eth[SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX+2];

  /*
   * Packet Format Groups
   */
  SOC_TMC_PMF_PFG_INFO pfg_info[SOC_PB_PMF_NOF_LKP_PROFILE_IDS];

  /*
   * Statistic-Tag offset
   */
  uint32 stag_offset_nibble;

  /*
   * VSQ-Pointer = NIF-Port enable
   */
  uint8 is_vsq_nif_enabled;

} SOC_PB_SW_DB_PMF;

typedef struct
{
  /*
   * Egress mirror
   */
  uint8 eg_mirror_port_vlan[SOC_PB_PORT_NOF_PP_PORTS][SOC_PB_PORT_EG_MIRROR_NOF_VID_MIRROR_INDICES];
} SOC_PB_SW_DB_EGR_MIRROR;

typedef struct
{
  /*
   * Core frequency of the current device. Units: KHz. Range: 100,000 - 500,000
   */
  uint32 self_freq;
  /*
   * The minimal core frequency of any FAP device in system. Units: KHz. Range: 100,000 - 500,000
   */
  uint32 min_fap_freq_in_system;
} SOC_PB_SW_DB_CORE_FREQ_HI_RESOLUTION;
#endif

typedef struct
{
  SOC_PETRA_SW_DB_OP_MODE
    op_mode;

  SOC_PETRA_SW_DB_VER
    dev_version;

  SOC_PETRA_SW_DB_FAT_PIPE
    fat_pipe;

  SOC_PETRA_SW_DB_LAGS_INFO
    lag;

  SOC_PETRA_SW_DB_DEV_EGR_PORTS
    soc_petra_sw_db_egr_ports;

  SOC_PETRA_SW_DB_DEVICE_SCH
    soc_petra_sw_db_scheduler;

  SOC_PETRA_SW_DB_MULTICAST
    soc_petra_sw_db_multicast;

  SOC_PETRA_SW_DB_AUTO_SCHEME
    soc_petra_api_auto_queue_flow_mngmnt;

  SOC_PETRA_SW_DB_SERDES
    serdes;

  SOC_PETRA_SW_DB_LBG
    lbg;

  SOC_PETRA_SW_DB_CELL
    cell;

  SOC_PETRA_SW_DB_DRAM
    dram;

  SOC_PETRA_SW_DB_QDR
    qdr;

  uint8
    is_interrupt_masked_on;

  SOC_PETRA_SW_DB_MAC
    mac;

#ifdef LINK_PB_LIBRARIES
  SOC_PB_SW_DB_SYNCE
    synce;

  SOC_PB_SW_DB_PORT
    port[SOC_PB_PORT_NOF_PP_PORTS];

  SOC_PB_SW_DB_TCAM_MGMT_INFO
    *tcam_info;

  SOC_PB_SW_DB_TCAM_DB_OLD
    tcam_db;

  SOC_PB_SW_DB_TCAM
    tcam;

  SOC_PB_SW_DB_TCAM_MGMT
    tcam_mgmt;

  SOC_PB_SW_DB_FRWRD_IP
    frwrd_ip;

  SOC_PB_SW_DB_LBG
    soc_pb_lbg;

  SOC_PB_SW_DB_MULTICAST
    soc_pb_multicast;

  SOC_PB_SW_DB_EGR_EDITOR
    egr_editor;

  SOC_PB_SW_DB_EGR_ACL
    egr_acl;

  SOC_PB_SW_DB_PMF
    pmf;

  SOC_PB_SW_DB_EGR_MIRROR
    soc_pb_egr_mirror;

  SOC_PB_SW_DB_CORE_FREQ_HI_RESOLUTION
    core_freq_hi_res;
#endif

}  SOC_PETRA_SW_DB_DEVICE_SSR_V01;

typedef struct
{
  SOC_PETRA_SW_DB_OP_MODE
    op_mode;

  SOC_PETRA_SW_DB_VER
    dev_version;

  SOC_PETRA_SW_DB_FAT_PIPE
    fat_pipe;

  SOC_PETRA_SW_DB_LAGS_INFO
    lag;

  SOC_PETRA_SW_DB_DEV_EGR_PORTS
    soc_petra_sw_db_egr_ports;

  SOC_PETRA_SW_DB_DEVICE_SCH
    soc_petra_sw_db_scheduler;

  SOC_PETRA_SW_DB_MULTICAST
    soc_petra_sw_db_multicast;

  SOC_PETRA_SW_DB_AUTO_SCHEME
    soc_petra_api_auto_queue_flow_mngmnt;

  SOC_PETRA_SW_DB_SERDES
    serdes;

  SOC_PETRA_SW_DB_LBG
    lbg;

  SOC_PETRA_SW_DB_CELL
    cell;

  SOC_PETRA_SW_DB_DRAM
    dram;

  SOC_PETRA_SW_DB_QDR
    qdr;

  uint8
    is_interrupt_masked_on;

  SOC_PETRA_SW_DB_MAC
    mac;

#ifdef LINK_PB_LIBRARIES
  SOC_PB_SW_DB_SYNCE
    synce;

  SOC_PB_SW_DB_PORT
    port[SOC_PB_PORT_NOF_PP_PORTS];

  SOC_PB_SW_DB_TCAM_MGMT_INFO
    *tcam_info;

  SOC_PB_SW_DB_TCAM_DB_OLD
    tcam_db;

  SOC_PB_SW_DB_TCAM
    tcam;

  SOC_PB_SW_DB_TCAM_MGMT
    tcam_mgmt;

  SOC_PB_SW_DB_FRWRD_IP
    frwrd_ip;

  SOC_PB_SW_DB_LBG
    soc_pb_lbg;

  SOC_PB_SW_DB_MULTICAST
    soc_pb_multicast;

  SOC_PB_SW_DB_EGR_EDITOR
    egr_editor;

  SOC_PB_SW_DB_EGR_ACL
    egr_acl;

  SOC_PB_SW_DB_PMF
    pmf;

  SOC_PB_SW_DB_EGR_MIRROR
    soc_pb_egr_mirror;

  SOC_PB_SW_DB_CORE_FREQ_HI_RESOLUTION
    core_freq_hi_res;

  uint32
    new_field_for_test_ssr_version_update;

#endif

}  SOC_PETRA_SW_DB_DEVICE_SSR_V02;

typedef SOC_PETRA_SW_DB_DEVICE_SSR_V01 SOC_PETRA_SW_DB_DEVICE;

typedef struct
{
  SOC_PETRA_SW_DB_DEVICE*
    soc_petra_device_sw_db[SOC_SAND_MAX_DEVICE];
}  SOC_PETRA_SW_DB;


#ifdef LINK_PB_LIBRARIES
typedef struct
{
  SOC_PETRA_SW_DB_DEVICE_SSR_V01
    soc_petra_ssr;

}  SOC_PB_SW_DB_DEVICE_SSR_V01;

typedef struct
{
  SOC_PETRA_SW_DB_DEVICE_SSR_V02
    soc_petra_ssr;

}  SOC_PB_SW_DB_DEVICE_SSR_V02;

typedef SOC_PB_SW_DB_DEVICE_SSR_V01 SOC_PB_SW_DB_DEVICE;
#endif

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/********************************************************************************************
 * Initialization
 * {
 ********************************************************************************************/
uint32
  soc_petra_sw_db_init(void);

void
  soc_petra_sw_db_close(void);

/*
 *  Per-device software database initializations
 */
uint32
  soc_petra_sw_db_device_init(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN SOC_PETRA_DEV_VER soc_petra_ver
  );

/*
 *  Per-device software database closing
 */
uint32
  soc_petra_sw_db_device_close(
    SOC_SAND_IN int unit
  );

#ifdef LINK_PB_LIBRARIES
/*
 *	Soc_petra-B TCAM
 */

uint32
  soc_pb_sw_db_tcam_terminate(
    SOC_SAND_IN  int unit
  );


#endif

/********************************************************************************************
 * Configuration
 * {
 ********************************************************************************************/

SOC_PETRA_DEV_VER
  soc_petra_sw_db_ver_get(SOC_SAND_IN int unit);


/*********************************************************************************************
 * }
 * soc_petra_egr_ports
 * {
 *********************************************************************************************/


SOC_SAND_RET
  soc_petra_sw_db_egr_ports_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            mal_ndx,
    SOC_SAND_IN  uint32            port_ndx,
    SOC_SAND_IN  SOC_PETRA_SW_DB_DEV_EGR_RATE  *val
  );

SOC_SAND_RET
  soc_petra_sw_db_egr_ports_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            mal_ndx,
    SOC_SAND_IN  uint32            port_ndx,
    SOC_SAND_OUT SOC_PETRA_SW_DB_DEV_EGR_RATE  *val
  );

SOC_SAND_RET
  soc_petra_sw_db_egr_ports_mal_sch_rate_set(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN  uint32   mal_ndx,
    SOC_SAND_IN  uint32   *val
  );

SOC_SAND_RET
  soc_petra_sw_db_egr_ports_mal_sch_rate_get(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN  uint32   mal_ndx,
    SOC_SAND_OUT  uint32   *val
  );

SOC_SAND_RET
  soc_petra_sw_db_egr_ports_mal_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                mal_ndx,
    SOC_SAND_OUT SOC_PETRA_SW_DB_DEV_EGR_MAL  *val
  );

uint32
  soc_petra_sw_db_egr_ports_active_mals_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 nof_mac_lanes,
    SOC_SAND_OUT uint8 *mal_is_active
  );

/*********************************************************************************************
 * }
 * soc_petra_scheduler
 * {
 *********************************************************************************************/

void
  soc_petra_sw_db_sch_max_expected_port_rate_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 port_ndx,
    SOC_SAND_IN uint32  rate
  );

uint32
  soc_petra_sw_db_sch_max_expected_port_rate_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 port_ndx
  );

void
  soc_petra_sw_db_sch_accumulated_grp_port_rate_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 grp_ndx,
    SOC_SAND_IN uint32  rate
  );

uint32
  soc_petra_sw_db_sch_accumulated_grp_port_rate_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 grp_ndx
  );

/*********************************************************************************************
 * }
 * soc_petra_multicast
 * {
 *********************************************************************************************/
uint32
  soc_petra_sw_db_multicast_init(
    SOC_SAND_IN int      unit
  );

void
  soc_petra_sw_db_multicast_nof_unoccupied_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 nof_unoccupied
  );

uint32
  soc_petra_sw_db_multicast_nof_unoccupied_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_multicast_next_unoccupied_ptr_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  tbl_entry_id,
    SOC_SAND_IN uint16  next_unoccupied
  );

uint16
  soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  tbl_entry_id
  );

void
  soc_petra_sw_db_multicast_backwards_ptr_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  tbl_entry_id,
    SOC_SAND_IN uint16  backwards_ptr
  );

uint16
  soc_petra_sw_db_multicast_backwards_ptr_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  tbl_entry_id
  );

void
  soc_petra_sw_db_multicast_unoccupied_list_head_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint16  new_list_head
  );

uint16
  soc_petra_sw_db_multicast_unoccupied_list_head_get(
    SOC_SAND_IN int unit
  );

/*********************************************************************************************
 * }
 * soc_petra auto scheme
 * {
 *********************************************************************************************/
uint32
  soc_petra_sw_db_auto_scheme_init(
    SOC_SAND_IN int unit
  );

uint32
  soc_petra_sw_db_fat_pipe_init(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_fat_pipe_nof_ports_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 fat_pipe_nof_ports
  );

uint32
  soc_petra_sw_db_fat_pipe_nof_ports_get(
    SOC_SAND_IN int unit
  );

uint8
  soc_petra_sw_db_auto_scheme_is_initialized(
    SOC_SAND_IN int unit
  );

SOC_SAND_RET
  soc_petra_sw_db_auto_scheme_info_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_SYSTEM_INFO          *auto_system_info
  );

SOC_SAND_RET
  soc_petra_sw_db_auto_scheme_info_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT SOC_PETRA_AQFM_SYSTEM_INFO          *auto_system_info
  );

SOC_SAND_RET
  soc_petra_sw_db_auto_agg_info_item_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       port_id,
    SOC_SAND_IN  uint32                       agg_level,
    SOC_SAND_IN  uint32                       agg_index,
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_TABLE_ITEM_INFO  *agg_table_item_info
  );

SOC_SAND_RET
  soc_petra_sw_db_auto_agg_info_item_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       port_id,
    SOC_SAND_IN  uint32                       agg_level,
    SOC_SAND_IN  uint32                       agg_index,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_TABLE_ITEM_INFO  *agg_table_item_info
  );

uint32
  soc_petra_sw_db_op_mode_init(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_revision_subtype_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PETRA_REVISION_SUB_TYPE_ID  subtype_id
  );

SOC_PETRA_REVISION_SUB_TYPE_ID
  soc_petra_sw_db_revision_subtype_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_ver_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PETRA_DEV_VER  ver
  );

void
  soc_petra_sw_db_pp_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 pp_enable
  );

uint8
  soc_petra_sw_db_pp_enable_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_is_fap20_in_system_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_fap20_in_system
  );

void
  soc_petra_sw_db_is_fap21_in_system_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_fap21_in_system
  );

uint8
  soc_petra_sw_db_is_fap20_in_system_get(
    SOC_SAND_IN int unit
  );

uint8
  soc_petra_sw_db_is_fap21_in_system_get(
    SOC_SAND_IN int unit
  );

void
soc_petra_sw_db_is_fe1600_in_system_set(
  SOC_SAND_IN int unit,
  SOC_SAND_IN uint8 is_fe1600_in_system
  );

uint8
soc_petra_sw_db_is_fe1600_in_system_get(
  SOC_SAND_IN int unit
  );


/*
 *	TRUE if fap20 or fap21 in system
 */
uint8
  soc_petra_sw_db_is_fap2x_in_system_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_is_a1_or_below_in_system_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_a1_or_below_in_system
  );

uint8
  soc_petra_sw_db_is_a1_or_below_in_system_get(
    SOC_SAND_IN int unit
  );

uint8
  soc_petra_sw_db_is_fap20_and_a1_in_system_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_is_fe200_fabric_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_fe200_fabric
  );

uint8
  soc_petra_sw_db_is_fe200_fabric_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_egr_mc_16k_groups_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 egr_mc_16k_groups_enable
  );

uint8
  soc_petra_sw_db_egr_mc_16k_groups_enable_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_tdm_mode_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PETRA_MGMT_TDM_MODE tdm_mode
  );

SOC_PETRA_MGMT_TDM_MODE
  soc_petra_sw_db_tdm_mode_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_is_fabric_variable_cell_size_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_fabric_variable_cell_size
  );

uint8
  soc_petra_sw_db_is_fabric_variable_cell_size_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_is_fabric_packet_segmentation_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_fabric_packet_segmentation
  );

uint8
  soc_petra_sw_db_is_fabric_packet_segmentation_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_combo_nif_not_fabric_set(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN SOC_PETRA_COMBO_QRTT  combo_idx,
    SOC_SAND_IN uint8         is_combo_nif
  );

SOC_SAND_RET
  soc_petra_sw_db_ref_clocks_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PETRA_MGMT_SRD_REF_CLKS *ref_clocks_conf
  );

void
  soc_petra_sw_db_stag_enable_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 stag_enable
  );

uint8
  soc_petra_sw_db_stag_enable_get(
    SOC_SAND_IN int unit
  );

uint8
  soc_petra_sw_db_combo_nif_not_fabric_get(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN SOC_PETRA_COMBO_QRTT  combo_idx
  );

SOC_PETRA_MGMT_SRD_REF_CLK
  soc_petra_sw_db_ref_clocks_nif_get(
    SOC_SAND_IN  int unit
  );

SOC_PETRA_MGMT_SRD_REF_CLK
  soc_petra_sw_db_ref_clocks_fabric_get(
    SOC_SAND_IN  int unit
  );

SOC_PETRA_MGMT_SRD_REF_CLK
  soc_petra_sw_db_ref_clocks_combo_get(
    SOC_SAND_IN  int unit
  );

/*
 *  Cell
 */

uint32
  soc_petra_sw_db_cell_cell_ident_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint16                      cell_ident
  );

uint32
  soc_petra_sw_db_cell_cell_ident_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint16                      *cell_ident
  );

/*
 *  DRAM size
 */

uint32
  soc_petra_sw_db_dram_dram_size_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     dram_size_per_interface_mbyte
  );

uint32
  soc_petra_sw_db_dram_dram_size_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint32                      *dram_size_per_interface_mbyte
  );

/*
 * DLL training sequence register
 */

uint32
  soc_petra_sw_db_dram_dll_periodic_trng_reg_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     dll_periodic_trng_reg
  );

uint32
  soc_petra_sw_db_dram_dll_periodic_trng_reg_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint32                      *dll_periodic_trng_reg
  );

/*
 * DLL training sequence register - QDR
 */

uint32
  soc_petra_sw_db_qdr_dll_periodic_trng_reg_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32     dll_periodic_trng_reg
  );

uint32
  soc_petra_sw_db_qdr_dll_periodic_trng_reg_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint32                      *dll_periodic_trng_reg
  );

void
  soc_petra_sw_db_qdr_size_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN SOC_PETRA_HW_QDR_SIZE_MBIT     size
  );

SOC_PETRA_HW_QDR_SIZE_MBIT
  soc_petra_sw_db_qdr_size_get(
    SOC_SAND_IN  int                      unit
  );

void
  soc_petra_sw_db_qdr_protection_type_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN SOC_PETRA_HW_QDR_PROTECT_TYPE     protection_type
  );

SOC_PETRA_HW_QDR_PROTECT_TYPE
  soc_petra_sw_db_qdr_protection_type_get(
    SOC_SAND_IN  int                      unit
  );

void
  soc_petra_sw_db_qdr_enable_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint8    enable
  );

uint8
  soc_petra_sw_db_qdr_enable_get(
    SOC_SAND_IN  int                      unit
  );

/*
 * DRAM type
 */

uint32
  soc_petra_sw_db_dram_type_set(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN SOC_PETRA_DRAM_TYPE dram_type
  );

uint32
  soc_petra_sw_db_dram_type_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_OUT SOC_PETRA_DRAM_TYPE *dram_type
  );

/*
 * DRAM configuration
 */

uint32
  soc_petra_sw_db_dram_conf_set(
    SOC_SAND_IN int                  unit,
    SOC_SAND_IN SOC_PETRA_HW_DRAM_CONF_PARAMS *dram_conf
  );

uint32
  soc_petra_sw_db_dram_conf_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PETRA_HW_DRAM_CONF_PARAMS     *dram_conf
  );

/*********************************************************************************************
 * }
 * soc_petra_serdes
 * {
 *********************************************************************************************/
uint32
  soc_petra_sw_db_device_srd_init(
    SOC_SAND_IN int unit
  );

/*
 *  Polarity swap handling enable/disable
 */
void
  soc_petra_sw_db_srd_is_swap_unhandled_set(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint8   is_swap_unhandled
  );

uint8
  soc_petra_sw_db_srd_is_swap_unhandled_get(
    SOC_SAND_IN int   unit
  );

/*
 *  IRQ enable/disable
 */
void
  soc_petra_sw_db_srd_is_irq_disable_set(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint8   is_irq_disable
  );

uint8
  soc_petra_sw_db_srd_is_irq_disable_get(
    SOC_SAND_IN int   unit
  );

/*
 *  Quartet Max Rate
 */
void
  soc_petra_sw_db_srd_qrtt_max_rate_set(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN uint32            qrtt_ndx,
    SOC_SAND_IN uint32             rate
  );

uint32
  soc_petra_sw_db_srd_qrtt_max_rate_get(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN uint32            qrtt_ndx
  );

/*
 *  IS_PRBS_23_SWAP
 */
void
  soc_petra_sw_db_srd_prbs_23_swap_is_enabled_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN SOC_PETRA_CONNECTION_DIRECTION  direction,
    SOC_SAND_IN uint32                   lane_ndx,
    SOC_SAND_IN uint8                   is_set
  );

uint8
  soc_petra_sw_db_srd_prbs_23_swap_is_enabled_get(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN SOC_PETRA_CONNECTION_DIRECTION  direction,
    SOC_SAND_IN uint32                   lane_ndx
  );


void
  soc_petra_sw_db_srd_orig_swap_polarity_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN SOC_PETRA_CONNECTION_DIRECTION  direction,
    SOC_SAND_IN uint32                   lane_ndx,
    SOC_SAND_IN uint8                   is_swap_polarity
  );

uint8
  soc_petra_sw_db_srd_orig_swap_polarity_get(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN SOC_PETRA_CONNECTION_DIRECTION  direction,
    SOC_SAND_IN uint32                   lane_ndx
  );

/*
 * LBG
 */
void
  soc_petra_sw_db_lbg_port_save_valid_set(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32        port_indx,
    SOC_SAND_IN uint8       valid
  );

uint8
  soc_petra_sw_db_lbg_port_save_valid_get(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32        port_indx
  );

uint32
  soc_petra_sw_db_lbg_port_stat_save(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN uint32              port_indx,
    SOC_SAND_IN SOC_PETRA_SW_DB_LBG_PORT  *port_stat
  );

uint32
  soc_petra_sw_db_lbg_port_stat_load(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN uint32              port_indx,
    SOC_SAND_OUT SOC_PETRA_SW_DB_LBG_PORT  *port_stat
  );

uint32
  soc_petra_sw_db_lbg_packet_size_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                      size
  );

uint32
  soc_petra_sw_db_lbg_packet_size_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_OUT uint32                      *size
  );

/*
 * LAGs
 */
uint32
  soc_petra_sw_db_lag_in_use_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                      lag_ndx,
    SOC_SAND_IN uint8                      use
  );

uint32
  soc_petra_sw_db_lag_in_use_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                       lag_ndx,
    SOC_SAND_OUT uint8                      *use
  );

uint32
  soc_pb_sw_db_local_to_sys_port_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                     tm_port,
    SOC_SAND_IN uint32                     sys_port
  );

uint32
  soc_pb_sw_db_local_to_sys_port_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                      tm_port,
    SOC_SAND_OUT uint32                     *sys_port
  );

void
  soc_petra_sw_db_ofp_rates_calcal_length_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 calcal_length
  );

uint32
  soc_petra_sw_db_ofp_rates_calcal_length_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_ofp_rates_update_device_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 update_device
  );

uint8
  soc_petra_sw_db_ofp_rates_update_device_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_ofp_rates_update_dev_changed_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 update_dev_changed
  );

uint8
  soc_petra_sw_db_ofp_rates_update_dev_changed_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_ofp_rates_nof_instances_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 mal_ndx,
    SOC_SAND_IN uint32 nof_instances
  );

uint32
  soc_petra_sw_db_ofp_rates_nof_instances_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 mal_ndx
  );

/*
 * Fabric MACs
 */
void
  soc_petra_sw_db_is_double_rate_set(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint32   link_ndx,
    SOC_SAND_IN uint8   is_double_rate
  );

uint8
  soc_petra_sw_db_is_double_rate_get(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint32   link_ndx
  );

#ifdef LINK_PB_LIBRARIES

void
  soc_pb_sw_db_synce_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_MODE                val
  );

SOC_PB_NIF_SYNCE_MODE
  soc_pb_sw_db_synce_get(
    SOC_SAND_IN  int                        unit
  );

void
  soc_pb_sw_db_profile_ndx_pmf_pgm_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pp_port_ndx,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_IN  uint32             tm_ppp_ndx,
    SOC_SAND_IN  uint32             pmf_pgm
  );

uint32
  soc_pb_sw_db_profile_ndx_pmf_pgm_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pp_port_ndx,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_IN  uint32             tm_ppp_ndx
  );

void
  soc_pb_sw_db_pp_port_header_type_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pp_port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE header_type
  );

SOC_PETRA_PORT_HEADER_TYPE
  soc_pb_sw_db_pp_port_header_type_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pp_port_ndx
  );

/*
 *	LBG
 */
void
  soc_pb_sw_db_lbg_port_save_valid_set(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32        port_indx,
    SOC_SAND_IN uint8       valid
  );

uint8
  soc_pb_sw_db_lbg_port_save_valid_get(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32        port_indx
  );

uint32
  soc_pb_sw_db_lbg_port_stat_save(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN uint32              port_indx,
    SOC_SAND_IN SOC_PB_SW_DB_LBG_PORT    *port_stat
  );

uint32
  soc_pb_sw_db_lbg_port_stat_load(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN uint32              port_indx,
    SOC_SAND_OUT SOC_PB_SW_DB_LBG_PORT   *port_stat
  );

uint32
  soc_pb_sw_db_lbg_profile_info_stat_load(
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN uint32                            profile_indx,
    SOC_SAND_OUT SOC_PB_PORT_PP_PORT_INFO          *profile_info
  );

uint32
  soc_pb_sw_db_lbg_profile_info_stat_save(
    SOC_SAND_IN int                             unit,
    SOC_SAND_IN uint32                              profile_indx,
    SOC_SAND_IN SOC_PB_PORT_PP_PORT_INFO             *profile_info
  );

uint32
  soc_pb_sw_db_lbg_profile_trap_stat_load(
    SOC_SAND_IN int                           unit,
    SOC_SAND_IN uint32                            profile_indx,
    SOC_SAND_OUT SOC_PB_SW_DB_LBG_PP_PORT_TRAP *profile_trap
  );

uint32
  soc_pb_sw_db_lbg_profile_trap_stat_save(
    SOC_SAND_IN int                             unit,
    SOC_SAND_IN uint32                              profile_indx,
    SOC_SAND_IN SOC_PB_SW_DB_LBG_PP_PORT_TRAP    *profile_trap
  );

uint32
  soc_pb_sw_db_lbg_profile_strength_stat_load(
    SOC_SAND_IN int                               unit,
    SOC_SAND_IN uint32                                pp_port_ndx,
    SOC_SAND_OUT SOC_PB_SW_DB_LBG_PP_PORT_STRENGTH         *profile_strength
  );

uint32
  soc_pb_sw_db_lbg_profile_strength_stat_save(
    SOC_SAND_IN int                                 unit,
    SOC_SAND_IN uint32                                  pp_port_ndx,
    SOC_SAND_IN SOC_PB_SW_DB_LBG_PP_PORT_STRENGTH            *profile_strength
  );

void
  soc_pb_sw_db_lbg_profile_save_valid_set(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32        profile_indx,
    SOC_SAND_IN uint8       valid
  );

uint8
  soc_pb_sw_db_lbg_profile_save_valid_get(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32        profile_indx
  );

uint32
  soc_pb_sw_db_lbg_packet_size_get(
    SOC_SAND_IN int                     unit,
    SOC_SAND_OUT uint32                      *size
  );

uint32
  soc_pb_sw_db_lbg_packet_size_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                      size
  );

/*
 * TCAM
 */

void
  soc_pb_sw_db_tcam_bank_valid_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint8 valid
  );

uint8
  soc_pb_sw_db_tcam_bank_valid_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id
  );

void
  soc_pb_sw_db_tcam_bank_entry_size_set(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint32               bank_id,
    SOC_SAND_IN SOC_PB_TCAM_BANK_ENTRY_SIZE width
  );

SOC_PB_TCAM_BANK_ENTRY_SIZE
  soc_pb_sw_db_tcam_bank_entry_size_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id
  );

void
  soc_pb_sw_db_tcam_bank_entries_used_set(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint32       bank_id,
    SOC_SAND_IN SOC_SAND_OCC_BM_PTR entries_used
  );

SOC_SAND_OCC_BM_PTR
  soc_pb_sw_db_tcam_bank_entries_used_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id
  );

void
  soc_pb_sw_db_tcam_bank_nof_entries_free_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 nof_entries_free
  );

uint32
  soc_pb_sw_db_tcam_bank_nof_entries_free_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id
  );

void
  soc_pb_sw_db_tcam_db_valid_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint8 valid
  );

uint8
  soc_pb_sw_db_tcam_db_valid_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  );

void
  soc_pb_sw_db_tcam_db_entry_size_set(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint32               tcam_db_id,
    SOC_SAND_IN SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size
  );

SOC_PB_TCAM_BANK_ENTRY_SIZE
  soc_pb_sw_db_tcam_db_entry_size_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  );

void
  soc_pb_sw_db_tcam_db_prefix_size_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 prefix_size
  );

uint32
  soc_pb_sw_db_tcam_db_prefix_size_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  );

void
  soc_pb_sw_db_tcam_db_priority_mode_set(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN uint32            tcam_db_id,
    SOC_SAND_IN SOC_PB_TCAM_DB_PRIO_MODE prio_mode
  );

SOC_PB_TCAM_DB_PRIO_MODE
  soc_pb_sw_db_tcam_db_priority_mode_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  );

void
  soc_pb_sw_db_tcam_db_bank_used_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint8 is_used
  );

uint8
  soc_pb_sw_db_tcam_db_bank_used_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id,
    SOC_SAND_IN uint32 bank_id
  );

void
  soc_pb_sw_db_tcam_db_prefix_set(
    SOC_SAND_IN int      unit,
    SOC_SAND_IN uint32      tcam_db_id,
    SOC_SAND_IN uint32      bank_id,
    SOC_SAND_IN SOC_PB_TCAM_PREFIX *prefix
  );

void
  soc_pb_sw_db_tcam_db_prefix_get(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  uint32      tcam_db_id,
    SOC_SAND_IN  uint32      bank_id,
    SOC_SAND_OUT SOC_PB_TCAM_PREFIX *prefix
  );

SOC_SAND_SORTED_LIST_INFO *
  soc_pb_sw_db_tcam_db_priorities_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  );

SOC_SAND_HASH_TABLE_INFO *
  soc_pb_sw_db_tcam_db_entry_id_to_location_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  );

void
  soc_pb_sw_db_tcam_db_location_tbl_set(
    SOC_SAND_IN int        unit,
    SOC_SAND_IN uint32        tcam_db_id,
            SOC_PB_TCAM_LOCATION *location_tbl
  );

SOC_PB_TCAM_LOCATION *
  soc_pb_sw_db_tcam_db_location_tbl_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 tcam_db_id
  );

void
  soc_pb_sw_db_tcam_access_profile_valid_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint8 valid
  );

uint8
  soc_pb_sw_db_tcam_access_profile_valid_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  );

void
  soc_pb_sw_db_tcam_access_profile_db_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint32 tcam_db
  );

uint32
  soc_pb_sw_db_tcam_access_profile_db_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  );

void
  soc_pb_sw_db_tcam_access_profile_cycle_set(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN uint32            profile,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_CYCLE cycle
  );

SOC_PB_TCAM_ACCESS_CYCLE
  soc_pb_sw_db_tcam_access_profile_cycle_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  );

void
  soc_pb_sw_db_tcam_access_profile_uniform_prefix_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint8 uniform_prefix
  );

uint8
  soc_pb_sw_db_tcam_access_profile_uniform_prefix_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  );

void
  soc_pb_sw_db_tcam_access_profile_min_banks_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint32 min_banks
  );

uint32
  soc_pb_sw_db_tcam_access_profile_min_banks_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  );

void
  soc_pb_sw_db_tcam_access_profile_callback_set(
    SOC_SAND_IN int           unit,
    SOC_SAND_IN uint32           profile,
    SOC_SAND_IN SOC_PB_TCAM_MGMT_SIGNAL callback
  );

SOC_PB_TCAM_MGMT_SIGNAL
  soc_pb_sw_db_tcam_access_profile_callback_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  );

void
  soc_pb_sw_db_tcam_access_profile_user_data_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint32  user_data
  );

uint32
  soc_pb_sw_db_tcam_access_profile_user_data_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile
  );

void
  soc_pb_sw_db_tcam_access_profile_access_device_set(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN uint32             profile_id,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_DEVICE device
  );

SOC_PB_TCAM_ACCESS_DEVICE
  soc_pb_sw_db_tcam_access_profile_access_device_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile_id
  );

void
  soc_pb_sw_db_tcam_managed_bank_access_profile_set(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN uint32            bank_id,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_CYCLE cycle,
    SOC_SAND_IN uint32            profile
  );

uint32
  soc_pb_sw_db_tcam_managed_bank_access_profile_get(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN uint32            bank_id,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_CYCLE cycle
  );

void
  soc_pb_sw_db_tcam_managed_bank_prefix_db_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 prefix,
    SOC_SAND_IN uint32 tcam_db_id
  );

uint32
  soc_pb_sw_db_tcam_managed_bank_prefix_db_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 prefix
  );

void
  soc_pb_sw_db_tcam_managed_bank_nof_db_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 nof_dbs
  );

uint32
  soc_pb_sw_db_tcam_managed_bank_nof_db_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id
  );

void
  soc_pb_sw_db_tcam_mgmt_access_profile_in_access_device_set(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_CYCLE  cycle,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_DEVICE access_device,
    SOC_SAND_IN uint32             profile_id
  );

uint32
  soc_pb_sw_db_tcam_mgmt_access_profile_in_access_device_get(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_CYCLE  cycle,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_DEVICE access_device
  );

SOC_SAND_HASH_TABLE_INFO *
  soc_pb_sw_db_frwrd_ip_route_key_to_entry_id_get(
    SOC_SAND_IN int unit
  );

void
  soc_pb_sw_db_tcam_explicit_address_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint8                  explicit_address_set
  );

uint8
  soc_pb_sw_db_tcam_explicit_address_get(
    SOC_SAND_IN  int                        unit
  );

void
  soc_pb_sw_db_tcam_explicit_bank_sel_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint8                  explicit_bank_sel_set
  );

uint8
  soc_pb_sw_db_tcam_explicit_bank_sel_get(
    SOC_SAND_IN  int                        unit
  );

void
  soc_pb_sw_db_tcam_lkp_profile_db_ids_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE         *lkp_profile,
    SOC_SAND_IN  uint32                  db_id_in_profile_ndx,
    SOC_SAND_IN  uint32                  db_id
  );

uint32
  soc_pb_sw_db_tcam_lkp_profile_db_ids_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE         *lkp_profile,
    SOC_SAND_IN  uint32                  db_id_in_profile_ndx
  );

uint32
soc_pb_sw_db_tcam_info_get(
  SOC_SAND_IN int                              unit,
  SOC_SAND_OUT SOC_PB_SW_DB_TCAM_MGMT_INFO        **tcam_mgmt
);

uint32
  soc_pb_sw_db_device_tcam_user_clear(
    SOC_SAND_IN  int                 unit
  );

uint32
  soc_pb_sw_db_tcam_mgmt_initialize(
    SOC_SAND_IN int           unit,
    SOC_SAND_IN uint8   create
  );

void
  soc_petra_sw_db_ftmh_lb_ext_mode_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PB_MGMT_FTMH_LB_EXT_MODE ftmh_lb_ext_mode
  );

SOC_PB_MGMT_FTMH_LB_EXT_MODE
  soc_petra_sw_db_ftmh_lb_ext_mode_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_add_pph_eep_ext_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 add_pph_eep_ext
  );

uint8
  soc_petra_sw_db_add_pph_eep_ext_get(
    SOC_SAND_IN int unit
  );

void
  soc_petra_sw_db_is_petra_rev_a_in_system_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_petra_rev_a_in_system
  );

uint8
  soc_petra_sw_db_is_petra_rev_a_in_system_get(
    SOC_SAND_IN int unit
  );

uint32
  soc_pb_sw_db_multicast_eg_mult_nof_vlan_bitmaps_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 multicast_id
  );

void
  soc_pb_sw_db_multicast_eg_mult_nof_vlan_bitmaps_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 multicast_id,
    SOC_SAND_IN uint32 nof_vlan_bitmaps
  );

uint32
  soc_pb_sw_db_eg_editor_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_sw_db_eg_editor_terminate(
    SOC_SAND_IN  int unit
  );

uint32
soc_pb_sw_db_frwrd_ip_terminate(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_sw_db_eg_acl_initialize(
    SOC_SAND_IN  int unit
   );

SOC_PB_EGR_PROG_TM_PORT_PROFILE
  soc_pb_sw_db_eg_editor_index_profile_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 hw_index
  );

void
  soc_pb_sw_db_eg_editor_index_profile_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                   hw_index,
    SOC_SAND_IN SOC_PB_EGR_PROG_TM_PORT_PROFILE index_profile
  );

SOC_PB_EGR_ACL_DB_TYPE
  soc_pb_sw_db_eg_acl_type_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 db_id
  );

void
  soc_pb_sw_db_eg_acl_type_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                   db_id_ndx,
    SOC_SAND_IN SOC_PB_EGR_ACL_DB_TYPE          type
  );

uint32
  soc_pb_sw_db_pmf_initialize(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_sw_db_pmf_terminate(
    SOC_SAND_IN  int unit
  );

uint32
  soc_pb_sw_db_pmf_port_index_profile_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 hw_index
  );

void
  soc_pb_sw_db_pmf_port_index_profile_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                   hw_index,
    SOC_SAND_IN uint32                    index_profile
  );

void
  soc_pb_sw_db_pgm_profile_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 hw_index,
    SOC_SAND_OUT SOC_PB_SW_DB_PMF_PGM_PROFILE *pgm_profile
  );

void
  soc_pb_sw_db_pgm_profile_set(
    SOC_SAND_IN int                   unit,
    SOC_SAND_IN uint32                   hw_index,
    SOC_SAND_IN SOC_PB_SW_DB_PMF_PGM_PROFILE    *pgm_profile
  );

void
  soc_pb_sw_db_pgm_ce_instr_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_IN  uint32             key_ndx,
    SOC_SAND_IN  uint32             instr_ndx,
    SOC_SAND_IN  SOC_PB_SW_DB_PMF_CE      *ce_instr
  );

void
  soc_pb_sw_db_pgm_ce_instr_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_IN  uint32             key_ndx,
    SOC_SAND_IN  uint32             instr_ndx,
    SOC_SAND_OUT SOC_PB_SW_DB_PMF_CE      *ce_instr
  );

void
  soc_pb_sw_db_is_pgm_eth_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pmf_pgm_ndx,
    SOC_SAND_IN  uint8             is_eth
  );

uint8
  soc_pb_sw_db_is_pgm_eth_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pmf_pgm_ndx
  );

void
  soc_pb_sw_db_pfg_info_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_IN  SOC_TMC_PMF_PFG_INFO      *pfg_info
  );

void
  soc_pb_sw_db_pfg_info_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pfg_ndx,
    SOC_SAND_OUT SOC_TMC_PMF_PFG_INFO      *pfg_info
  );

void
  soc_pb_sw_db_stag_offset_nibble_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             stag_offset_nibble
  );

uint32
  soc_pb_sw_db_stag_offset_nibble_get(
    SOC_SAND_IN  int             unit
  );

void
  soc_pb_sw_db_is_vsq_nif_enabled_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint8             is_vsq_nif_enabled
  );

uint8
  soc_pb_sw_db_is_vsq_nif_enabled_get(
    SOC_SAND_IN  int             unit
  );

void
  soc_pb_sw_db_egr_acl_sw_db_id_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     egr_acl_ndx,
    SOC_SAND_IN  uint32                      sw_db_id
  );

uint32
  soc_pb_sw_db_egr_acl_sw_db_id_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      egr_acl_ndx
 );


uint32
  soc_pb_sw_db_outbound_mirror_port_vlan_is_exist_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pp_port_ndx,
    SOC_SAND_IN  uint32              internal_vid_ndx,
    SOC_SAND_OUT uint8             *is_exist
    );

uint32
  soc_pb_sw_db_outbound_mirror_port_vlan_is_exist_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             pp_port_ndx,
    SOC_SAND_IN  uint32              internal_vid_ndx,
    SOC_SAND_IN  uint8             is_exist
  );

/*
 * SW DB multi-sets {
 */
uint32
  soc_pb_sw_db_buffer_set_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_INOUT  uint8                           *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_IN  uint8                              *data
  );

uint32
  soc_pb_sw_db_buffer_get_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_IN  uint8                              *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_OUT uint8                              *data
  );

uint32
  soc_pb_sw_db_multiset_add(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type, /* SOC_PB_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32                 val,
    SOC_SAND_OUT  uint32                *data_indx,
    SOC_SAND_OUT  uint8               *first_appear,
    SOC_SAND_OUT  SOC_SAND_SUCCESS_FAILURE    *success
  );

uint32
  soc_pb_sw_db_multiset_remove(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type, /* SOC_PB_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32                 val,
    SOC_SAND_OUT  uint32                *data_indx,
    SOC_SAND_OUT  uint8               *last_appear
  );

uint32
  soc_pb_sw_db_multiset_lookup(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       multiset_type, /* SOC_PB_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32        val,
    SOC_SAND_OUT  uint32       *data_indx,
    SOC_SAND_OUT  uint32       *ref_count
  );

uint32
  soc_pb_sw_db_multiset_remove_by_index(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type, /* SOC_PB_SW_DB_MULTI_SET */
    SOC_SAND_IN  uint32                 data_indx,
    SOC_SAND_OUT  uint8               *last_appear
  );

uint32
  soc_pb_sw_db_multiset_clear(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multiset_type /* SOC_PB_SW_DB_MULTI_SET */
  );

void
  soc_pb_sw_db_core_freq_self_freq_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              self_freq
  );

uint32
  soc_pb_sw_db_core_freq_self_freq_get(
    SOC_SAND_IN  int             unit
  );

void
  soc_pb_sw_db_core_freq_min_fap_freq_in_system_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              min_fap_freq_in_system
  );

uint32
  soc_pb_sw_db_core_freq_min_fap_freq_in_system_get(
    SOC_SAND_IN  int             unit
  );

/*
 * SW DB multi-sets }
 */
#endif


/*
 * } Configuration
 */

/*********************************************************************************************
* }
* soc_petra interrupt
* {
*********************************************************************************************/

uint8
  soc_petra_sw_db_interrupt_mask_on_get(
    SOC_SAND_IN  int                        unit
  );

void
  soc_petra_sw_db_interrupt_mask_on_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint8                        val
  );

/* } */

uint32
  soc_petra_ssr_PETRA_SW_DB_SSR_V01_data_save(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT SOC_PETRA_SW_DB_DEVICE_SSR_V01  *ssr_data
  );

uint32
  soc_petra_ssr_PETRA_SW_DB_SSR_V01_data_load(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint8                      *ssr_stream
  );

#ifdef LINK_PB_LIBRARIES
uint32
  soc_pb_ssr_PB_SW_DB_SSR_additional_buff_size_get(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint32    *buff_byte_size
    );

uint32
  soc_pb_ssr_PB_SW_DB_SSR_data_save(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_OUT SOC_PB_SW_DB_DEVICE            *ssr_data,
      SOC_SAND_OUT uint8                      *additional_data,
      SOC_SAND_IN  uint32                      additional_data_size
    );

uint32
  soc_pb_ssr_PB_SW_DB_SSR_data_load(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  uint8                      *ssr_stream,
      SOC_SAND_IN  uint32                     size_of_pb_ssr,
      SOC_SAND_IN  uint8                      *additional_data
    );
#endif

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_SW_DB_INCLUDED__*/
#endif
