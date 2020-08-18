/*
 * $Id: drv.h,v 1.187 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */
#ifndef _SOC_DNX_LEGACY_DRV_H
#define _SOC_DNX_LEGACY_DRV_H

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

#include <sal/types.h>

#include <shared/cyclic_buffer.h>

#include <soc/drv.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <soc/scache.h>
#include <soc/mem.h>

#include <soc/dnxc/legacy/dnxc_defs.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>

#include <soc/dnx/legacy/fabric.h>

#include <soc/dnx/legacy/TMC/tmc_api_multicast_egress.h>
#include <soc/dnx/legacy/TMC/tmc_api_multicast_fabric.h>
#include <soc/dnxc/legacy/error.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>

#define SOC_DNX_MAX_NOF_CHANNELS    (DNX_DATA_MAX_PORT_GENERAL_MAX_NOF_CHANNELS)
#define SOC_DNX_DRV_TDM_OPT_SIZE    (78)

/* Reset flags */
#define SOC_DNX_RESET_ACTION_IN_RESET                               SOC_DNXC_RESET_ACTION_IN_RESET
#define SOC_DNX_RESET_ACTION_OUT_RESET                              SOC_DNXC_RESET_ACTION_OUT_RESET
#define SOC_DNX_RESET_ACTION_INOUT_RESET                            SOC_DNXC_RESET_ACTION_INOUT_RESET

#define SOC_DNX_RESET_MODE_HARD_RESET                               SOC_DNXC_RESET_MODE_HARD_RESET
#define SOC_DNX_RESET_MODE_BLOCKS_RESET                             SOC_DNXC_RESET_MODE_BLOCKS_RESET
#define SOC_DNX_RESET_MODE_BLOCKS_SOFT_RESET                        SOC_DNXC_RESET_MODE_BLOCKS_SOFT_RESET
#define SOC_DNX_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET                SOC_DNXC_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET
#define SOC_DNX_RESET_MODE_BLOCKS_SOFT_EGRESS_RESET                 SOC_DNXC_RESET_MODE_BLOCKS_SOFT_EGRESS_RESET
#define SOC_DNX_RESET_MODE_INIT_RESET                               SOC_DNXC_RESET_MODE_INIT_RESET
#define SOC_DNX_RESET_MODE_REG_ACCESS                               SOC_DNXC_RESET_MODE_REG_ACCESS
#define SOC_DNX_RESET_MODE_ENABLE_TRAFFIC                           SOC_DNXC_RESET_MODE_ENABLE_TRAFFIC
#define SOC_DNX_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET             SOC_DNXC_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET
#define SOC_DNX_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_INGRESS_RESET     SOC_DNXC_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_INGRESS_RESET
#define SOC_DNX_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_EGRESS_RESET      SOC_DNXC_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_EGRESS_RESET
#define SOC_DNX_RESET_MODE_BLOCKS_SOFT_RESET_DIRECT                 SOC_DNXC_RESET_MODE_BLOCKS_SOFT_RESET_DIRECT


#define SOC_DNX_MAX_PADDING_SIZE 0x7f

/* 0-511 pbmp 512-1023 internal 1024-1535 interface */ 

/* Different port types have different ranges                   */
/* each port range is sized at SOC_DNX_PORT_RANGE_NUM_ENTRIES   */
/* pbmp ports start at 0.                                       */
#define SOC_DNX_PORT_RANGE_NUM_ENTRIES 512 /* 288 required for JER2_ARAD */


#define SOC_DNX_PORT_INTERNAL_START                 (DNX_ALGO_PORT_INVALID)
#define SOC_DNX_PORT_INTERNAL_OLP(core)             (DNX_ALGO_PORT_INVALID)
#define SOC_DNX_PORT_INTERNAL_OAMP(core)            (DNX_ALGO_PORT_INVALID)
#define SOC_DNX_PORT_INTERNAL_ERP(core)             (DNX_ALGO_PORT_INVALID)


#define SOC_DNX_PORT_INTERNAL_END                   (SOC_DNX_PORT_INTERNAL_START + SOC_DNX_PORT_RANGE_NUM_ENTRIES - 1)


/* Number of MACT limit table ranges to be mapped */
#define SOC_DNX_MAX_NOF_MACT_LIMIT_MAPPED_LIF_RANGES    (2)

#define SOC_DNX_FABRIC_LOGICAL_PORT_BASE(unit)            (dnx_data_port.general.fabric_port_base_get(unit))

#define SOC_DNX_NUM_OF_ROUTES (2048)
#define SOC_DNX_MAX_NUM_OF_ROUTE_GROUPS           (32)

#define SOC_DNX_INVALID_NOF_REMOTE_CORES            (0xffff)
#define SOC_DNX_INVALID_MAPPING_MODE                (0xffff)
#ifdef FIXME_DNX_LEGACY /** SOC_DNX_DEFS not supported */
#define SOC_DNX_MAX_RIF_ID                          (SOC_DNX_DEFS_GET(unit, max_nof_rifs) - 1)
#endif

#define SOC_DNX_FABRIC_PORT_TO_LINK(unit, port) ((port) - SOC_DNX_FABRIC_LOGICAL_PORT_BASE(unit))
#define SOC_DNX_FABRIC_LINK_TO_PORT(unit, port) ((port) + SOC_DNX_FABRIC_LOGICAL_PORT_BASE(unit))

/*
 * Definitions of various SOC properties
 */
#define SOC_DNX_FC_CAL_MODE_DISABLE                             0
#define SOC_DNX_FC_CAL_MODE_RX_ENABLE                           0x1
#define SOC_DNX_FC_CAL_MODE_TX_ENABLE                           0x2

#define SOC_DNX_FC_INBAND_INTLKN_CAL_LLFC_MODE_DISABLE          0
#define SOC_DNX_FC_INBAND_INTLKN_CAL_LLFC_MODE1                 1 /* calender 0 */
#define SOC_DNX_FC_INBAND_INTLKN_CAL_LLFC_MODE2                 2 /* calender 0, 16, ... */

#define SOC_DNX_FC_INBAND_INTLKN_LLFC_MUB_DISABLE               0
#define SOC_DNX_FC_INBAND_INTLKN_LLFC_MUB_MASK                  0xFF

#define SOC_DNX_FC_INBAND_INTLKN_CHANNEL_MUB_DISABLE            0
#define SOC_DNX_FC_INBAND_INTLKN_CHANNEL_MUB_MASK               0xFF

#define SOC_DNX_FABRIC_TDM_PRIORITY_NONE                        7 /* system does not support TDM */

/*
 * dnx tm config declaration
 */
#ifdef FIXME_DNX_LEGACY /** SOC_DNX_DEFS not supported */
#define SOC_DNX_MAX_INTERLAKEN_PORTS                            SOC_DNX_DEFS_MAX(NOF_INTERLAKEN_PORTS)
#define SOC_DNX_MAX_CAUI_PORTS                                  SOC_DNX_DEFS_MAX(NOF_CAUI_PORTS)
#endif
#define SOC_DNX_MAX_OOB_PORTS                                   2
#define SOC_DNX_NOF_LINKS                                      36 /* for JER2_ARAD only */
#define SOC_DNX_NOF_INSTANCES_MAC                               9 /* for JER2_ARAD only */

#define JER2_ARAD_MULTICAST_TABLE_MODE         JER2_ARAD_MULT_NOF_MULTICAST_GROUPS
#define JER2_QAX_NOF_MCDB_ENTRIES              (SOC_IS_QUX(unit) ? (48*1024) : (96*1024))
#define JER2_JER_NOF_MCDB_ENTRIES              (256*1024)

/* DNX MC Modes */
#define DNX_MC_ALLOW_DUPLICATES_MODE            1  /* allow duplicate replications in multicast groups */
#define DNX_MC_EGR_17B_CUDS_127_PORTS_MODE      4  /* Arad+ encoding mode */
#define DNX_MC_CUD_EXTENSION_MODE               8  /* CUD extension node */
#define DNX_MC_EGR_CORE_FDA_MODE                16 /* FDA egress hardware controls whether each egress group core is replicated to */
#define DNX_MC_EGR_CORE_MESH_MODE               64 /* Mesh ingress MC hardware controls whether each egress group core is replicated to */

#define JER2_ARAD_Q_PAIRS_ILKN                 8


/* Macros for multicast CUD (outlif) and destination encoding */

#define JER2_ARAD_MC_DEST_ENCODING_0 0 /* supported on all Arads, 16bit CUDs, 96K flows, 32K sysports, 32K MCIDs */
#define JER2_ARAD_MC_DEST_ENCODING_1 1 /* supported on Arad+, 17bit CUDs, 64K-1 flows, 32K sysports */
#define JER2_ARAD_MC_DEST_ENCODING_2 2 /* supported on Arad+, 18bit CUDs, 32K-1 flows, 16K sysports */
#define JER2_ARAD_MC_DEST_ENCODING_3 3 /* supported on Arad+, 17bit CUDs, 96K flows */

/* maximum number of queue that the encoding supports (also limited by no more than 96K queues) */
#define JER2_ARAD_MC_DEST_ENCODING_0_MAX_QUEUE 0x17fff
#define JER2_ARAD_MC_DEST_ENCODING_1_MAX_QUEUE 0xfffe /* 0xffff may cause no replication */
#define JER2_ARAD_MC_DEST_ENCODING_2_MAX_QUEUE 0x7ffe /* 0x7fff may cause no replication */
#define JER2_ARAD_MC_DEST_ENCODING_3_MAX_QUEUE 0x17fff

/* maximum CUD value that the ingress encoding supports (also limited by no more than 96K queues) */
#define JER2_ARAD_MC_DEST_ENCODING_0_MAX_ING_CUD  0xffff
#define JER2_ARAD_MC_DEST_ENCODING_1_MAX_ING_CUD 0x1ffff
#define JER2_ARAD_MC_DEST_ENCODING_2_MAX_ING_CUD 0x3ffff
#define JER2_ARAD_MC_DEST_ENCODING_3_MAX_ING_CUD 0x1ffff
/* maximum CUD value that the egress encoding supports (also limited by no more than 96K queues) */
#define JER2_ARAD_MC_16B_MAX_EGR_CUD  0xffff
#define JER2_ARAD_MC_17B_MAX_EGR_CUD 0x1ffff

#define JER2_JER_MC_MAX_EGR_CUD  0x3ffff
#define JER2_JER_MC_MAX_ING_CUD  0x7ffff

/* number of system ports that the encoding supports */
#define JER2_ARAD_MC_DEST_ENCODING_0_NOF_SYSPORT 0x8000
#define JER2_ARAD_MC_DEST_ENCODING_1_NOF_SYSPORT 0x8000
#define JER2_ARAD_MC_DEST_ENCODING_2_NOF_SYSPORT 0x4000
#define JER2_ARAD_MC_DEST_ENCODING_3_NOF_SYSPORT 0

#define JER2_ARAD_MC_MAX_BITMAPS 8191

/* base queue to module x port mapping modes: */
#define VOQ_MAPPING_DIRECT   0
#define VOQ_MAPPING_INDIRECT 1

/* Reserve Unique Device ID for TDM settings. All FAP-IDs must be different than this ID */
#define DNX_TDM_FAP_DEVICE_ID_UNIQUE      (0x80)

/* supported values for the action_type_source_mode field */
#define DNX_ACTION_TYPE_FROM_QUEUE_SIGNATURE 0   /* get action type from queue signature (default) */
#define DNX_ACTION_TYPE_FROM_FORWARDING_ACTION 1 /* get action type from packet header */

#define DNX_IS_CREDIT_WATCHDOG_FAST_STATUS_MESSAGE_MODE(unit, mode) (mode == CREDIT_WATCHDOG_FAST_STATUS_MESSAGE_MODE)
#define DNX_IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit, mode) (mode >= CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE)
#define DNX_IS_CREDIT_WATCHDOG_UNINITIALIZED(unit, mode) (mode == CREDIT_WATCHDOG_UNINITIALIZED)
#define DNX_GET_CREDIT_WATCHDOG_MODE_BASE(mode) ((mode) < CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE ? (mode) : CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE)
#define DNX_GET_CREDIT_WATCHDOG_MODE(unit, mode) (sw_state_legacy_access[unit].soc.jer2_arad.tm.tm_info.credit_watchdog.credit_watchdog_mode.get(unit, &(mode)))
#define DNX_IS_CREDIT_WATCHDOG_MODE(unit, mode, credit_watchdog_mode) (DNX_GET_CREDIT_WATCHDOG_MODE_BASE(mode) == DNX_GET_CREDIT_WATCHDOG_MODE_BASE(credit_watchdog_mode))
#define DNX_SET_CREDIT_WATCHDOG_MODE(unit, mode) (sw_state_legacy_access[unit].soc.jer2_arad.tm.tm_info.credit_watchdog.credit_watchdog_mode.set(unit, (mode)))
#define DNX_GET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, scan_time_nano) \
    (sw_state_legacy_access[unit].soc.jer2_arad.tm.tm_info.credit_watchdog.exact_credit_watchdog_scan_time_nano.get(unit, &(scan_time_nano)))
#define DNX_SET_EXACT_CREDIT_WATCHDOG_SCAN_TIME_NANO(unit, scan_time_nano) \
    (sw_state_legacy_access[unit].soc.jer2_arad.tm.tm_info.credit_watchdog.exact_credit_watchdog_scan_time_nano.set(unit, (scan_time_nano)))


#define JER2_ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_MS 2
#define JER2_ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS (1000000 * JER2_ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_MS)
#define JER2_ARAD_CREDIT_WATCHDOG_COMMON_MAX_DELETE_EXP 12
#define JER2_ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES 4
#define JER2_ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS (JER2_ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS >> JER2_ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES)

/* Using this number for the start and end of the queue scan range in jer2_arad_itm_cr_wd_set/get,
 * marks to update the delete time exponents for all profiles, in the common FSM mode */
#define JER2_ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP ((uint32)(-2))

#define DNX_VARIOUS_BM_FORCE_MBIST_TEST 1

#define JER2_ARAD_PLUS_MIN_ALPHA -7
#define JER2_ARAD_PLUS_MAX_ALPHA 7
#define JER2_ARAD_PLUS_ALPHA_NOT_SUPPORTED 255


/* XGS Diffserv, HQOS system port encoding mode */
#define SOC_DNX_XGS_TM_7_MODID_8_PORT        (0)
#define SOC_DNX_XGS_TM_8_MODID_7_PORT        (1)
#define SOC_DNX_WARMBOOT_TIME_OUT            (5000000) /*5 sec*/

/* put these defenitions under #if 0 and defined those above because of mutex deadlock they cause */    \

#define SOC_DNX_ALLOW_WARMBOOT_WRITE(operation, _rv) \
            SOC_ALLOW_WB_WRITE(unit, operation, _rv)

#define SOC_DNX_ALLOW_WARMBOOT_WRITE_NO_ERR(operation, _rv) \
        do { \
            SOC_ALLOW_WB_WRITE(unit, operation, _rv); \
            if (_rv != _SHR_E_UNIT) { \
                _rv = _SHR_E_NONE; \
            } \
        } while(0)

#define SOC_DNX_WARMBOOT_RELEASE_HW_MUTEX(_rv)\
        do {\
            _rv = soc_schan_override_disable(unit); \
        } while(0)

/* iterate over all cores */
#define SOC_DNX_CORES_ITER(core_id, index) DNXCMN_CORES_ITER(unit, core_id, index)

/* validate core in range */
#define SOC_DNX_CORE_VALIDATE(unit, core, allow_all) DNXCMN_CORE_VALIDATE(unit, core, allow_all)

/* converts modid to base modid */
#define SOC_DNX_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, modid, base_modid) \
           ((base_modid <= modid) && (base_modid + dnx_data_device.general.nof_cores_get(unit) > modid))

#define SOC_DNX_CORE_TO_MODID(base_modid, core) (base_modid + core)

#define SOC_DNX_MODID_TO_CORE(unit, base_modid, modid) \
           modid - base_modid; \
           SOC_DNX_CORE_VALIDATE(unit, modid - base_modid, FALSE) 

#define _SOC_DNX_HTONS_CVT_SET(pkt, val, posn)  \
    do { \
         uint16 _tmp; \
         _tmp = soc_htons(val); \
         sal_memcpy((pkt)->_pkt_data.data + (posn), &_tmp, 2); \
    } while (0) 

#define SOC_DNX_GET_BLOCK_TYPE_SCHAN_ID(unit,block_type,other_value)   soc_dnx_get_block_type_schan_id(unit,block_type)>=0 ? soc_dnx_get_block_type_schan_id(unit,block_type): other_value

#define SOC_DNX_CORE_MODE_IS_SINGLE_CORE(unit) (dnx_data_device.general.nof_cores_get(unit) == 1)
#define SOC_DNX_CORE_ALL(unit) (SOC_DNX_CORE_MODE_IS_SINGLE_CORE(unit) ? 0 : SOC_CORE_ALL)

#define SOC_DNX_IS_MESH(unit) (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_MESH)

#define SOC_DNX_SINGLE_FAP(unit)            (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_SINGLE_FAP)

int soc_dnx_info_config_common_tm(int unit);
int soc_dnx_common_init(int unit);
int soc_dnx_info_config(int unit);
int soc_sand_info_config_blocks(int unit, uint8 nof_cores_to_broadcast);
int soc_dnx_chip_type_set(int unit, uint16 dev_id);
int soc_dnx_prop_parse_admission_precedence_preference(int unit, uint32* preference);

extern int soc_dnx_attach(int unit);
extern int soc_dnx_detach(int unit);
extern int soc_dnx_init(int unit, int reset);
extern int soc_dnx_deinit(int unit);
extern int soc_dnx_reinit(int unit, int reset);
extern int soc_dnx_dump(int unit, char *pfx);
extern void soc_dnx_chip_dump(int unit, soc_driver_t *d);
extern int soc_dnx_device_reset(int unit, int mdoe, int action);

/* Read/write internal registers */
extern int soc_dnx_reg_read(int unit, soc_reg_t reg, uint32 addr, 
                            uint64 *data);
extern int soc_dnx_reg_write(int unit, soc_reg_t reg, uint32 addr, 
                             uint64 data);

extern int soc_dnx_reg32_read(int unit, uint32 addr, uint32 *data);
extern int soc_dnx_reg32_write(int unit, uint32 addr, uint32 data);

extern int soc_dnx_reg64_read(int unit, uint32 addr, uint64 *data);
extern int soc_dnx_reg64_write(int unit, uint32 addr, uint64 data);

/* Read/Write internal memories */
extern int soc_dnx_mem_read(int unit, soc_mem_t mem, int copyno, int index, 
                            void *entry_data);
/*
extern int soc_dnx_mem_array_read(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index,
                            void *entry_data);
*/
extern int soc_dnx_mem_write(int unit, soc_mem_t mem, int copyno, int index, 
                             void *entry_data);
/*
extern int soc_dnx_mem_array_write(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index,
                             void *entry_data);
*/
/*
 * Map block type to number of block instance. 
 */
int
soc_dnx_nof_block_instances(int unit,  soc_block_types_t block_types, int *nof_block_instances) ;

/*
 * Get unit AVS value 
 */
int
soc_dnx_avs_value_get(int unit, uint32* avs_value);

uint32
_soc_dnx_property_port_get(int unit, soc_port_t port,
                      const char *name, uint32 defl);

#ifdef BCM_WARM_BOOT_SUPPORT
uint8 
_bcm_dnx_switch_is_immediate_sync(int unit);
#endif /*BCM_WARM_BOOT_SUPPORT*/

/*init ofp rates configuration*/
int
_soc_dnx_jer2_arad_ofp_rates_set(int unit);

int
soc_dnx_cache_enable_init(int unit);
int soc_dnx_info_config_ports(int unit);

int soc_dnx_get_block_type_schan_id(int unit, soc_block_t block_type);


/* check whether next hop mac extension feature work in jer2_arad mode:
   for compatibility with JER2_ARAD, Disable hardware computation of Host-Index for DA.
   Instead, do it the Arad way: PMF will add pph learn extension (system header ), egress program editor will stamp the DA  
   if this soc property is disabled, then use hardware computation using the chicken bit */ 
#define SOC_IS_NEXT_HOP_MAC_EXT_JER2_ARAD_COMPATIBLE(unit) (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "next_hop_mac_ext_jer2_arad_compatible", 0))


#endif  /* _SOC_DNX_DRV_H */
