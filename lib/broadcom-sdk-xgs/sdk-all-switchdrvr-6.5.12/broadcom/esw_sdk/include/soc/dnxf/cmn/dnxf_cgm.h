/*
 * $Id: dnxf_port.h,v 1.5 Broadcom SDK $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DNXF PORT H
 */

#ifndef _SOC_DNXF_CGM_H_
#define _SOC_DNXF_CGM_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <shared/fabric.h>

#include <soc/types.h>
#include <soc/error.h>
#include <soc/portmod/portmod.h>

#define SOC_DNXF_CGM_MAX_NOF_TH_REG_FIELDS           9

#define SOC_DNXF_FULL_FIFO_OFFSET                   (16)

#define SOC_DNXF_CGM_ACTION_NONE                                   0x0
#define SOC_DNXF_CGM_ACTION_APPLY_SOFT_RESET                       0x1
#define SOC_DNXF_CGM_ACTION_APPLY_OFFSET                           0x2
#define SOC_DNXF_CGM_ACTION_APPLY_OFFSET_AND_SOFT_RESET            (DNXF_CGM_APPLY_SOFT_RESET | DNXF_CGM_APPLY_OFFSET)
/**********************************************************/
/*                     Functions                          */
/**********************************************************/
_shr_error_t soc_dnxf_cgm_profile_threshold_set(
    int unit,
    int profile_id,
    _shr_dnxf_threshold_id_t threshold_id,
    _shr_dnxf_threshold_type_t threshold_type,
    uint32 flags,
    int threshold_value);
_shr_error_t soc_dnxf_cgm_profile_threshold_get(
    int unit,
    int profile_id,
    _shr_dnxf_threshold_id_t threshold_id,
    _shr_dnxf_threshold_type_t threshold_type,
    uint32 flags,
    int *threshold_value);

_shr_error_t soc_dnxf_cgm_link_profile_set(
    int unit,
    int profile_id,
    uint32 flags,
    uint32 links_count,
    soc_port_t * links);
_shr_error_t soc_dnxf_cgm_link_profile_get(
    int unit,
    int profile_id,
    uint32 flags,
    uint32 links_count_max,
    uint32 *links_count,
    soc_port_t * links);

_shr_error_t soc_dnxf_cgm_pipe_level_type_decode(
    int unit,
    _shr_dnxf_threshold_id_t threshold_id,
    int *level,
    int *pipe);
_shr_error_t soc_dnxf_cgm_cast_prio_type_decode(
    int unit,
    _shr_dnxf_threshold_id_t threshold_id,
    int *cast,
    int *priority);

typedef enum soc_dnxf_cgm_reg_structure_e
{
    soc_dnxf_cgm_reg_structure_1,       /* Common case where with few registers and pipe index in the filed array */
    soc_dnxf_cgm_reg_structure_2,       /* Special case where the pipe index is the registers array */
    soc_dnxf_cgm_reg_structure_3,       /* Special case where profile id is in the register index */
    soc_dnxf_cgm_reg_structure_4        /* Special case where the pipe is in the register field */
} soc_dnxf_cgm_reg_structure_t;

/*
 * Typedef:    soc_ramon_cgm_registers_table_t
 * Purpose:    Special structure to discribe all threshold type registers and fields
 */
typedef struct soc_dnxf_cgm_registers_table_s
{
    soc_reg_t register_name;
    soc_field_t field_name_array[SOC_DNXF_CGM_MAX_NOF_TH_REG_FIELDS];
} soc_dnxf_cgm_registers_table_t;

/*
 * Typedef:    soc_ramon_cgm_threshold_to_reg_binding_t
 * Purpose:    Table to match each threshold to the registers fileds that it should modify
 */
typedef struct soc_dnxf_cgm_threshold_to_reg_binding_s
{
    _shr_dnxf_threshold_type_t threshold_type;
    soc_dnxf_cgm_reg_structure_t th_handle_case;
    uint32 nof_regs_to_write_to;
    const soc_dnxf_cgm_registers_table_t *reg_description_ptr;
    uint32 action_required;
} soc_dnxf_cgm_threshold_to_reg_binding_t;

#endif /*_SOC_DNXF_CGM_H_*/
