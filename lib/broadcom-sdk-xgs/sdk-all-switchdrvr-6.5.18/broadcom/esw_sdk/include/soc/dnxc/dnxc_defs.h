/*
* $Id: dnxc_defs.h,v 1.8 Broadcom SDK $
* $Copyright: (c) 2019 Broadcom.
* Broadcom Proprietary and Confidential. All rights reserved.$
*
* This file contains structure and routine declarations for the
* Switch-on-a-Chip Driver.
*
* This file also includes the more common include files so the
* individual driver files don't have to include as much.
*/
#include <shared/bitop.h>
#include <shared/fabric.h>
#ifdef BCM_DNX_SUPPORT
/*dnx data*/
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_fabric.h>
#endif

#ifdef BCM_DNXF_SUPPORT
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_port.h>

#endif

#include <soc/sand/sand_defs.h>

#ifndef _SOC_DNXC_DEFS_H
#define _SOC_DNXC_DEFS_H

#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>

#define SOC_DNXC_NOF_LINKS_IN_MAC SOC_SAND_NOF_LINKS_IN_MAC

/*Reset flags*/
#define SOC_DNXC_RESET_ACTION_IN_RESET          SOC_SAND_RESET_ACTION_IN_RESET
#define SOC_DNXC_RESET_ACTION_OUT_RESET         SOC_SAND_RESET_ACTION_OUT_RESET
#define SOC_DNXC_RESET_ACTION_INOUT_RESET       SOC_SAND_RESET_ACTION_INOUT_RESET

#define SOC_DNXC_RESET_MODE_HARD_RESET                              SOC_SAND_RESET_MODE_HARD_RESET
#define SOC_DNXC_RESET_MODE_BLOCKS_RESET                            SOC_SAND_RESET_MODE_BLOCKS_RESET
#define SOC_DNXC_RESET_MODE_BLOCKS_SOFT_RESET                       SOC_SAND_RESET_MODE_BLOCKS_SOFT_RESET
#define SOC_DNXC_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET               SOC_SAND_RESET_MODE_BLOCKS_SOFT_INGRESS_RESET
#define SOC_DNXC_RESET_MODE_BLOCKS_SOFT_EGRESS_RESET                SOC_SAND_RESET_MODE_BLOCKS_SOFT_EGRESS_RESET
#define SOC_DNXC_RESET_MODE_INIT_RESET                              SOC_SAND_RESET_MODE_INIT_RESET
#define SOC_DNXC_RESET_MODE_REG_ACCESS                              SOC_SAND_RESET_MODE_REG_ACCESS
#define SOC_DNXC_RESET_MODE_ENABLE_TRAFFIC                          SOC_SAND_RESET_MODE_ENABLE_TRAFFIC
#define SOC_DNXC_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET            SOC_SAND_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET
#define SOC_DNXC_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_INGRESS_RESET    SOC_SAND_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_INGRESS_RESET
#define SOC_DNXC_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_EGRESS_RESET     SOC_SAND_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_EGRESS_RESET
#define SOC_DNXC_RESET_MODE_BLOCKS_SOFT_RESET_DIRECT                SOC_SAND_RESET_MODE_BLOCKS_SOFT_RESET_DIRECT

/* SET MAX NUMBER PIPES */
#define SOC_DNXC_MAX_NOF_PIPES 1
#ifdef BCM_DNXF_SUPPORT
#if SOC_DNXC_MAX_NOF_PIPES < DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPE
#undef SOC_DNXC_MAX_NOF_PIPES
#define SOC_DNXC_MAX_NOF_PIPES DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPE
#endif
#endif

#ifdef BCM_DNX_SUPPORT
#if SOC_DNXC_MAX_NOF_PIPES < DNX_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPE
#undef SOC_DNXC_MAX_NOF_PIPES
#define SOC_DNXC_MAX_NOF_PIPES DNX_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPE
#endif
#endif

/* SET MAX NUMBER PIPES - end */

/* SET MAX NUMBER OF FE LINKS*/
#define SOC_DNXC_MAX_NOF_FABRIC_LINKS 1
#ifdef BCM_DNXF_SUPPORT
#if SOC_DNXC_MAX_NOF_FABRIC_LINKS < DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS
#undef SOC_DNXC_MAX_NOF_FABRIC_LINKS
#define SOC_DNXC_MAX_NOF_FABRIC_LINKS DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS
#endif
#endif

#ifdef BCM_DNX_SUPPORT
#if SOC_DNXC_MAX_NOF_FABRIC_LINKS < DNX_DATA_MAX_FABRIC_LINKS_NOF_LINKS
#undef SOC_DNXC_MAX_NOF_FABRIC_LINKS
#define SOC_DNXC_MAX_NOF_FABRIC_LINKS DNX_DATA_MAX_FABRIC_LINKS_NOF_LINKS
#endif
#endif
/* SET MAX NUMBER OF FE LINKS - end */

/* SET MAX NUMBER OF CGM THRESHOLD PRIORITIES*/
#define SOC_DNXC_MAX_NOF_THRESHOLD_PRIORITIES 0
#ifdef BCM_DNXF_SUPPORT
#if SOC_DNXC_MAX_NOF_THRESHOLD_PRIORITIES < DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES
#undef SOC_DNXC_MAX_NOF_THRESHOLD_PRIORITIES
#define SOC_DNXC_MAX_NOF_THRESHOLD_PRIORITIES DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES
#endif
#endif
/* SET MAX NUMBER OF CGM THRESHOLD PRIORITIES - end */

/* SET MAX NUMBER OF CGM THRESHOLD LEVELS*/
#define SOC_DNXC_MAX_NOF_THRESHOLD_LEVELS 0
#ifdef BCM_DNXF_SUPPORT
#if SOC_DNXC_MAX_NOF_THRESHOLD_LEVELS < DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_LEVELS
#undef SOC_DNXC_MAX_NOF_THRESHOLD_LEVELS
#define SOC_DNXC_MAX_NOF_THRESHOLD_LEVELS DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_LEVELS
#endif
#endif
/* SET MAX NUMBER OF CGM THRESHOLD LEVELS - end */

/* SET MAX NUMBER OF PVT MONITORS*/
#if defined(BCM_DNX_SUPPORT) && defined(BCM_DNXF_SUPPORT)
#if DNX_DATA_MAX_PVT_GENERAL_NOF_PVT_MONITORS < DNXF_DATA_MAX_DEVICE_GENERAL_NOF_PVT_MONITORS
#define SOC_DNXC_MAX_NOF_PVT_MONITORS DNXF_DATA_MAX_DEVICE_GENERAL_NOF_PVT_MONITORS
#else
#define SOC_DNXC_MAX_NOF_PVT_MONITORS DNX_DATA_MAX_PVT_GENERAL_NOF_PVT_MONITORS
#endif
#else
#ifdef BCM_DNX_SUPPORT
#define SOC_DNXC_MAX_NOF_PVT_MONITORS DNX_DATA_MAX_PVT_GENERAL_NOF_PVT_MONITORS
#endif
#ifdef BCM_DNXF_SUPPORT
#define SOC_DNXC_MAX_NOF_PVT_MONITORS DNXF_DATA_MAX_DEVICE_GENERAL_NOF_PVT_MONITORS
#endif
#endif
/* SET MAX NUMBER OF PVT MONITORS - end */

/**
 * Definies error check
 */

#if (_SHR_FABRIC_TH_INDEX_LEVEL_NOF_BITS) < (SHR_BITS_TO_REPRESENT(SOC_DNXC_NOF_THRESHOLD_LEVELS))
#error _SHR_FABRIC_TH_INDEX_PIPE_NOF_BITS define is too small to hold SOC_DNXC_NOF_THRESHOLD_LEVELS
#endif

/**
* Definies error check - end
*/

typedef enum soc_dnxc_filter_type_e
{
    soc_dnxc_filter_type_source_id = 0, /* Expect all 11 bits */
    soc_dnxc_filter_type_multicast_id = 1,
    soc_dnxc_filter_type_priority = 2,
    soc_dnxc_filter_type_dest_id = 3,
    soc_dnxc_filter_type_traffic_cast = 4
} soc_dnxc_filter_type_t;

typedef enum soc_dnxc_control_cell_filter_type_e
{
    soc_dnxc_filter_control_cell_type_source_device = 0,
    soc_dnxc_filter_control_cell_type_dest_device
} soc_dnxc_control_cell_filter_type_t;

/* match the values of the control cell type of the jr2_arad device */
typedef enum soc_dnxc_control_cell_types_e
{
    soc_dnxc_flow_status_cell = 0,
    soc_dnxc_credit_cell,
    soc_dnxc_reachability_cell,
    soc_dnxc_fe2_filtered_cell = 6
} soc_dnxc_control_cell_types_t;

/* Date cell capture*/
typedef struct soc_dnxc_captured_control_cells_s
{
    int valid;
    soc_dnxc_control_cell_types_t control_type;
    int dest_device;
    int source_device;
    int dest_port;
    int src_queue_num;
    int dest_queue_num;
    int sub_flow_id;
    int flow_id;
    int reachability_bitmap;
    int base_index;
    int source_link_number;
} soc_dnxc_captured_control_cell_t;

typedef enum soc_dnxc_device_type_actual_value_e
{
    soc_dnxc_device_type_actual_value_FAP_1 = 0,
    soc_dnxc_device_type_actual_value_FAP = 5,
    soc_dnxc_device_type_actual_value_FIP = 1,
    soc_dnxc_device_type_actual_value_FOP = 4,
    soc_dnxc_device_type_actual_value_FE1 = 6,
    soc_dnxc_device_type_actual_value_FE2 = 3,
    soc_dnxc_device_type_actual_value_FE2_1 = 7,
    soc_dnxc_device_type_actual_value_FE3 = 2
} soc_dnxc_device_type_actual_value_t;

typedef enum soc_dnxc_init_serdes_ref_clock_s
{
    /*
     * Disable pll
     */
    soc_dnxc_init_serdes_ref_clock_disable = -1,
    /*
     * 125 MHZ
     */
    soc_dnxc_init_serdes_ref_clock_125 = 0,
    /*
     * 156/25 Mhz
     */
    soc_dnxc_init_serdes_ref_clock_156_25 = 1,
    /*
     * 25 MHZ
     */
    soc_dnxc_init_serdes_ref_clock_25 = 2,
    /*
     * 312/5 MHZ
     */
    soc_dnxc_init_serdes_ref_clock_312_5 = 3,
    /*
     * Bypass pll
     */
    soc_dnxc_init_serdes_ref_clock_bypass = 4,
    /*
     *  Number of ref clocks in JER2_ARAD_INIT_SERDES_REF_CLOCK
     */
    soc_dnxc_init_serdes_nof_ref_clocks = 5
} soc_dnxc_init_serdes_ref_clock_t;

typedef enum soc_dnxc_fabric_control_source_e
{
    soc_dnxc_fabric_control_source_none = 0,
    soc_dnxc_fabric_control_source_primary = 1,
    soc_dnxc_fabric_control_source_secondary = 2,
    soc_dnxc_fabric_control_source_both = 3
} soc_dnxc_fabric_control_source_t;
#endif /*_SOC_DNXC_DEFS_H*/
