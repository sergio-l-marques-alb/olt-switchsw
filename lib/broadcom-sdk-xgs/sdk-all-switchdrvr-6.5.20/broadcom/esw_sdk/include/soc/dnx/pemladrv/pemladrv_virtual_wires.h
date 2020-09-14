/** \file include/soc/dnx/pemladrv/pemladrv_virtual_wires.h
 *
 * virtual wires functionality
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef _PEMLADRV_VIRTUAL_WIRES_H_
#define _PEMLADRV_VIRTUAL_WIRES_H_

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_str.h>
#include <soc/dnx/pp_stage.h>
#include <soc/dnx/pemladrv/auto_generated/pemladrv.h>

/**
 * Iterator for iteraing over the virtual wires within VirtualWiresContainer.
 * INIT is the value to begin iterating with, END indicates that there are no more VW in the stage.
 * Both must be negative.
 */
#define PEMLADRV_ITERATOR_BY_STAGE_INIT (-1)
#define PEMLADRV_ITERATOR_BY_STAGE_END  (-2)

/* copied from adapter hw_id.hpp definition */
typedef enum
{
    PEMLA_PIPE_STAGE_INGRESS_HARD_LLR_SC = 0,
    PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTA,
    PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTB,
    PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTC,
    PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTD,
    PEMLA_PIPE_STAGE_INGRESS_KLEAP_VTE,
    PEMLA_PIPE_STAGE_INGRESS_KLEAP_FLPA,
    PEMLA_PIPE_STAGE_INGRESS_KLEAP_FLPB,
    PEMLA_PIPE_STAGE_INGRESS_HARD_PMFA_SC,
    PEMLA_PIPE_STAGE_INGRESS_SOFT_PMFA_SOFT,
    PEMLA_PIPE_STAGE_INGRESS_HARD_FER_SC,
    PEMLA_PIPE_STAGE_INGRESS_HARD_PMFB_SC,
    PEMLA_PIPE_STAGE_INGRESS_SOFT_PMFB_SOFT,
    PEMLA_PIPE_STAGE_INGRESS_HARD_LBP_SC,
    PEMLA_PIPE_STAGE_INGRESS_HARD_TM,
    PEMLA_PIPE_STAGE_EGRESS_ERPP_PARSER,
    PEMLA_PIPE_STAGE_EGRESS_ETPPA_HARD_PARSER_AND_PRP_SC,
    PEMLA_PIPE_STAGE_EGRESS_SOFT_PRP_SOFT,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_TERMINATION,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_FORWARDING,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION1,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION2,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION3,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION4,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_ENCAPSULATION5,
    PEMLA_PIPE_STAGE_EGRESS_EDITOR_TRAP,
    PEMLA_PIPE_STAGE_EGRESS_HARD_BTC_POST_PROCESSING_SC,

    PEMLA_BTC_REF_MODEL,

    PEMLA_UNDEFINED
} PEMLA_STAGES_ENUM_e;

/**
 * \brief  Get the mapping of a given virtual wire 'vw_name' to the physical wires
 * \param [in] unit  - Identifier of HW platform.
 * \param [in] vw_name - virtual wire name to be looked up
 * \param [in] pp_stage - requested stage ID
 * \param [out] vw_info - virtual wire returned structure including all the relevant information
 * \return
 *   \retval shr_error_e
 * \remark
 */
shr_error_e pemladrv_vw_wire_get(
    int unit,
    char vw_name[MAX_VW_NAME_LENGTH],
    dnx_pp_stage_e pp_stage,
    VirtualWireInfo ** vw_info);

/**
 * \brief  Get the number of VW mapped to a certain stage
 * \param [in] unit  - Identifier of HW platform.
 * \param [in] sdk_stage - requested stage ID
 * \param [out] nof_vw - Number of virtual wires available to the stage
 * \return
 *   \retval shr_error_e
 * \remark
 */
shr_error_e pemladrv_vw_wire_nof_by_stage(
    int unit,
    dnx_pp_stage_e sdk_stage,
    int *nof_vw);

/**
 * \brief  Get the next iterator
 * \param [in] unit  - Identifier of HW platform.
 * \param [in] nof_stages  - Number of stages given as input, number of elements in the arrays
 *                           sdk_stages and stages_not_started.
 * \param [in] sdk_stages - Array of requested stage IDs. enough that one stage will match to return an iterator.
 * \param [in,out] vw_iterator - The iterator of the VW. for the first time use PEMLADRV_ITERATOR_BY_STAGE_INIT,
 *                               when the iteration ends it will be PEMLADRV_ITERATOR_BY_STAGE_END.
 * \param [out] vw_info - The information about the the virtual wire, including name.
 * \param [out] stages_not_started - Boolean array, indicating for each stage if the VW does no start at the stage.
 * \return
 *   \retval shr_error_e
 * \remark
 */
shr_error_e pemladrv_vw_wire_iterator_by_stage_get_next(
    int unit,
    int nof_stages,
    dnx_pp_stage_e * sdk_stages,
    int *vw_iterator,
    VirtualWireInfo ** vw_info,
    uint8 *stages_not_started);

/**
 * \brief  Convert PEMLA stage value to SIGNAL PP stage
 * \param [in] unit  - Identifier of HW platform.
 * \param [in] pemla_stage - pemla stage to be converted
 * \param [out] pp_stage - matching PP stage.
 * \return
 *   \retval shr_error_e
 * \remark
 */
shr_error_e pemladrv_vw_stages_pemla_to_sdk(
    int unit,
    int pemla_stage,
    int *pp_stage);

/**
 * \brief  Convert SIGNAL PP stage to PEMLA stage
 * \param [in] unit  - Identifier of HW platform.
 * \param [in] pp_stage - PP stage to be converted
 * \param [out] pemla_stage - matching PEMLA stage.
 * \return
 *   \retval shr_error_e
 * \remark
 */
shr_error_e pemladrv_vw_stages_sdk_to_pemla(
    int unit,
    int pemla_stage,
    int *pp_stage);

#endif /* _PEMLADRV_VIRTUAL_WIRES_H_ */
