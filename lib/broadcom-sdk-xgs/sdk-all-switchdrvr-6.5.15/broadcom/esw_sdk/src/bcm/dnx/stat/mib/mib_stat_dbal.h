/** \file mib_stat_dbal.h
 * General MIB - contains internal functions and definitions for
 * MIB feature support 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef MIB_STAT_DBAL_H_INCLUDED
/*
 * {
 */
#define MIB_STAT_DBAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

shr_error_e mib_stat_init(
    int unit);

shr_error_e mib_stat_fabric_init(
    int unit);

shr_error_e dnx_fmac_stat_dbal_get(
    int unit,
    uint32 fmac_idx,
    uint32 lane_idx,
    int counter_id,
    uint64 *val);

shr_error_e dnx_cdmib_stat_dbal_get(
    int unit,
    bcm_port_t port,
    dbal_fields_e field_id,
    uint64 *val);

shr_error_e dnx_cdmib_stat_dbal_set(
    int unit,
    bcm_port_t port,
    dbal_fields_e field_id,
    uint64 val);

shr_error_e dnx_cdmib_counter_id_to_dbal_field_id_get(
    int unit,
    int port,
    int counter_id,
    dbal_fields_e * field_id);

#endif /* INTERNAL_OAM_INCLUDED */
