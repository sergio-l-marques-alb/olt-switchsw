/*
 * $Id: dnxf_defs.h,v 1.9 Broadcom SDK $
 *
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DNXF DEFS H
 */
#ifndef _SOC_DNXF_FABRIC_DEFS_H_
#define _SOC_DNXF_FABRIC_DEFS_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <soc/dnxf/swstate/auto_generated/types/dnxf_types.h>
#include <soc/error.h>
#include <shared/port.h>
#include <shared/fabric.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>
/**********************************************************/
/*                  Enumerators                           */
/**********************************************************/

typedef enum soc_dnxf_fabric_link_cell_size_e
{
    soc_dnxf_fabric_link_cell_size_VSC256_V1 = _SHR_FABRIC_LINK_CELL_FORMAT_VSC256_V1,
    soc_dnxf_fabric_link_cell_size_VSC256_V2 = _SHR_FABRIC_LINK_CELL_FORMAT_VSC256_V2
} soc_dnxf_fabric_link_cell_size_t;

typedef enum soc_dnxf_fabric_device_mode_e
{
    soc_dnxf_fabric_device_mode_single_stage_fe2 = 0,
    soc_dnxf_fabric_device_mode_multi_stage_fe2,
    soc_dnxf_fabric_device_mode_multi_stage_fe13,
    soc_dnxf_fabric_device_mode_repeater,
    soc_dnxf_fabric_device_mode_multi_stage_fe13_asymmetric
} soc_dnxf_fabric_device_mode_t;

typedef enum soc_dnxf_fabric_priority_e
{
    soc_dnxf_fabric_priority_0 = 0,
    soc_dnxf_fabric_priority_1,
    soc_dnxf_fabric_priority_2,
    soc_dnxf_fabric_priority_3,
    soc_dnxf_fabric_priority_nof
} soc_dnxf_fabric_priority_t;

typedef enum soc_dnxf_multicast_mode_e
{
    soc_dnxf_multicast_mode_direct = 0,
    soc_dnxf_multicast_mode_indirect = 1
} soc_dnxf_multicast_mode_t;

typedef enum soc_dnxf_load_balancing_mode_e
{
    soc_dnxf_load_balancing_mode_normal = 0,
    soc_dnxf_load_balancing_mode_destination_unreachable,
    soc_dnxf_load_balancing_mode_balanced_input
} soc_dnxf_load_balancing_mode_t;

typedef enum soc_dnxf_multicast_table_mode_e
{
    /*
     * 8x64K x (192 bit) mode, 8 parallel accesses to the multicast table
     * can be handled
     */
    soc_dnxf_multicast_table_mode_64k = 0,
    /*
     * 4x128Kx (192 bit) mode, 4 parallel accesses to the multicast table
     * can be handled
     */
    soc_dnxf_multicast_table_mode_128k = 1,
    /*
     * 8x128Kx (96 bit ) mode, 8 parallel accesses to the multicast
     * table can be handled
     */
    soc_dnxf_multicast_table_mode_128k_half = 2,
    /*
     * 2x256Kx (192 bit) mode, 2 parallel accesses to the multicast table
     * can be handled
     */
    soc_dnxf_multicast_table_mode_256k = 3,
    /*
     * 4x256Kx (96 bit) mode, 4 parallel accesses to the multicast table can be handled 
     */
    soc_dnxf_multicast_table_mode_256k_half = 4,
    /*
     * 2x512Kx (96 bit) mode, 2 parallel accesses to the multicast table
     * can be handled
     */
    soc_dnxf_multicast_table_mode_512k_half = 5
} soc_dnxf_multicast_table_mode_t;

typedef enum soc_dnxf_fabric_isolate_type_e
{
    soc_dnxf_fabric_isolate_type_none = 0,
    soc_dnxf_fabric_isolate_type_isolate = 1,
    soc_dnxf_fabric_isolate_type_shutdown = 2
} soc_dnxf_fabric_isolate_type_t;

typedef enum soc_dnxf_cosq_weight_mode_e
{
    soc_dnxf_cosq_weight_mode_regular = 0,
    soc_dnxf_cosq_weight_mode_dynamic_0 = 1,
    soc_dnxf_cosq_weight_mode_dynamic_1 = 2
} soc_dnxf_cosq_weight_mode_t;

typedef enum soc_dnxf_cosq_pipe_type_e
{
    soc_dnxf_cosq_pipe_type_ingress = 0,
    soc_dnxf_cosq_pipe_type_rtp,
    soc_dnxf_cosq_pipe_type_middle,
    soc_dnxf_cosq_pipe_type_egress
} soc_dnxf_cosq_pipe_type_t;

/******************************************************/
/*                  Defines                           */
/******************************************************/

#define SOC_DNXF_PROPERTY_UNAVAIL (-1)

#define SOC_DNXF_MODID_LOCAL_NOF(unit) \
    (SOC_DNXF_IS_FE13(unit) ? dnxf_data_fabric.topology.nof_local_modid_fe13_get(unit) : dnxf_data_fabric.topology.nof_local_modid_get(unit))

#define SOC_DNXF_MODID_GROUP_NOF(unit) \
    (SOC_DNXF_IS_FE13(unit) ? dnxf_data_fabric.topology.nof_local_modid_fe13_get(unit) : dnxf_data_fabric.topology.nof_local_modid_get(unit))

#endif /*_SOC_DNXF_FABRIC_DEFS_H_*/
