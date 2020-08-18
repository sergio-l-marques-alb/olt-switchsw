/** \file src/bcm/dnx/cosq/flow_control.c
 * $Id$
 *
 * Flow control API functions for DNX. \n
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

/*
 * $Copyright:.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dbal/dbal.h>
#include <bcm/cosq.h>
#include <bcm/types.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/imb/imb_internal.h>
#include <soc/portmod/portmod.h>
#include <bcm_int/dnx/cosq/flow_control.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <soc/dnx/legacy/TMC/tmc_api_flow_control.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <bcm_int/dnx/cosq/egress/ecgm.h>
#include <soc/dnxc/swstate/types/sw_state_template_mngr.h>
#include <soc/dnx/swstate/auto_generated/access/flow_control_access.h>

#include "flow_control_imp.h"

/*
 * Defines
 * {
 */

#define DNX_COE_VID_OFFSET  6

/*
 * }
 */

/*
 * Functions
 * {
 */

/**
 * \brief - This is a function to indicate that the combination of user specified source and target for the FC is illegal
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] is_delete - indicates weather the function is called from the add or delete API
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] target - holds information for the target of the Flow control
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_fc_illegal_src_tar_combination(
    int unit,
    int is_delete,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal combination of source and target!");
exit:
    SHR_FUNC_EXIT;

}

/* *INDENT-OFF* */
/*
 * Reception Source Reaction-Points Matrix
 */
static fc_matrix_t reception_source_reaction_points_matrix[DNX_FC_RECEPTION_SOURCES][DNX_FC_RECEPTION_REACTION_POINTS] =
{
                           /** Interface                                                         Port                                                              Port+COSQ */
/** LLFC */                {{dnx_fc_rec_llfc_interface_map, dnx_fc_rec_llfc_interface_verify},  {NULL, dnx_fc_illegal_src_tar_combination},                       {NULL, dnx_fc_illegal_src_tar_combination}},
/** PFC */                 {{NULL, dnx_fc_illegal_src_tar_combination},                         {dnx_fc_rec_pfc_port_cosq_map, dnx_fc_rec_pfc_port_cosq_verify},  {dnx_fc_rec_pfc_port_cosq_map, dnx_fc_rec_pfc_port_cosq_verify}},
/** Calendar(OOB, COE) */  {{dnx_fc_rec_cal_interface_map, dnx_fc_rec_cal_interface_verify},    {dnx_fc_rec_cal_port_cosq_map, dnx_fc_rec_cal_port_cosq_verify},  {dnx_fc_rec_cal_port_cosq_map, dnx_fc_rec_cal_port_cosq_verify}}
};

/*
 * Generation Source Destination Matrix
 */
static fc_matrix_t generation_source_destination_matrix[DNX_FC_GENERATION_SOURCES][DNX_FC_GENERATION_DESTINATIONS] =
{
                        /** LLFC                                                                  PFC                                                                  OOB Calendar */
/** GLB Res H */        {{dnx_fc_gen_glb_res_h_llfc_map, dnx_fc_gen_glb_res_h_llfc_verify},   {dnx_fc_gen_glb_res_h_l_pfc_map, dnx_fc_gen_glb_res_h_l_pfc_verify}, {dnx_fc_gen_glb_res_h_l_oob_map, dnx_fc_gen_glb_res_h_l_oob_verify}},
/** GLB Res L */        {{NULL, dnx_fc_illegal_src_tar_combination},                          {dnx_fc_gen_glb_res_h_l_pfc_map, dnx_fc_gen_glb_res_h_l_pfc_verify}, {dnx_fc_gen_glb_res_h_l_oob_map, dnx_fc_gen_glb_res_h_l_oob_verify}},
/** LLFC VSQs */        {{dnx_fc_gen_llfc_vsq_llfc_map, dnx_fc_gen_llfc_vsq_llfc_verify},     {NULL, dnx_fc_illegal_src_tar_combination},                          {dnx_fc_gen_llfc_vsq_oob_map, dnx_fc_gen_to_oob_verify}},
/** PFC VSQs */         {{NULL, dnx_fc_illegal_src_tar_combination},                          {dnx_fc_gen_pfc_vsq_pfc_map, dnx_fc_gen_pfc_vsq_pfc_verify},         {dnx_fc_gen_pfc_vsq_oob_map, dnx_fc_gen_pfc_vsq_oob_verify}},
/** CAT2 TC VSQs */     {{NULL, dnx_fc_illegal_src_tar_combination},                          {dnx_fc_gen_c2_tc_vsq_pfc_map, dnx_fc_gen_c2_tc_vsq_pfc_verify},     {dnx_fc_gen_vsq_a_d_oob_map, dnx_fc_gen_to_oob_verify}},
/** VSQ groups A-D */   {{NULL, dnx_fc_illegal_src_tar_combination},                          {NULL, dnx_fc_illegal_src_tar_combination},                          {dnx_fc_gen_vsq_a_d_oob_map, dnx_fc_gen_to_oob_verify}},
/** Almost full NIF*/   {{dnx_fc_gen_nif_llfc_map, dnx_fc_gen_nif_llfc_verify},               {dnx_fc_gen_nif_pfc_map, dnx_fc_gen_nif_pfc_verify},                 {dnx_fc_gen_nif_oob_map, dnx_fc_gen_to_oob_verify}}
};
/* *INDENT-ON* */

/**
 * \brief -
 * To return types of source and target when generation
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] source - source object
 *   \param [in] target - target object
 *   \param [out] source_type - type of source object
 *   \param [out] target_type - type of target object
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_fc_gen_src_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target,
    dnx_fc_gen_src_t * source_type,
    dnx_fc_gen_dest_t * target_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Determine the generation source
     */
    if (BCM_COSQ_GPORT_IS_VSQ(source->port))
    {
        if (BCM_COSQ_GPORT_IS_VSQ_SRC_PORT(source->port))
        {
            *source_type = DNX_FC_GEN_SRC_LLFC_VSQ;
        }
        else if (BCM_COSQ_GPORT_IS_VSQ_PG(source->port))
        {
            *source_type = DNX_FC_GEN_SRC_PFC_VSQ;
        }
        else if (BCM_COSQ_GPORT_IS_VSQ_CTTC(source->port) && (BCM_COSQ_GPORT_VSQ_CT_GET(source->port) == 2))
        {
            *source_type = DNX_FC_GEN_SRC_CAT2_TC_VSQ;
        }
        else if (BCM_COSQ_GPORT_IS_VSQ_GL(source->port))
        {
            if ((source->priority == 0) || (source->priority == BCM_COSQ_HIGH_PRIORITY))
            {
                /** To keep backward compatibility, because in legacy devices 0 represented high priority */
                *source_type = DNX_FC_GEN_SRC_GLB_RES_HIGH;
            }
            else if ((source->priority == 1) || (source->priority == BCM_COSQ_LOW_PRIORITY))
            {
                /** To keep backward compatibility, because in legacy devices 1 represented low priority */
                *source_type = DNX_FC_GEN_SRC_GLB_RES_LOW;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported priority %d", source->priority);
            }
        }
        else
        {
            *source_type = DNX_FC_GEN_SRC_VSQ_A_D;
        }
    }
    else if (BCM_GPORT_IS_LOCAL(source->port) || BCM_GPORT_IS_LOCAL_INTERFACE(source->port))
    {
        *source_type = DNX_FC_GEN_SRC_ALMOST_FULL_NIF;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unsupported Gport (0x%x)", source->port);
    }

    if (target_type != NULL)
    {
        /*
         * Determine the generation target
         */
        if (BCM_GPORT_IS_CONGESTION(target->port))
        {
            *target_type = DNX_FC_GEN_DEST_OOB_CAL;
        }
        else if (BCM_GPORT_IS_LOCAL(target->port) || BCM_GPORT_IS_LOCAL_INTERFACE(target->port)
                 || (target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT))
        {
            if (target->cosq == BCM_COS_INVALID)
            {
                *target_type = DNX_FC_GEN_DEST_LLFC;
            }
            else if ((source->cosq >= BCM_COS_MIN) || (source->cosq <= BCM_COS_MAX))
            {
                *target_type = DNX_FC_GEN_DEST_PFC;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported COSQ %d", target->cosq);
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "Unsupported Gport (0x%x)", target->port);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * To return types of source and target when reception
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] source - source object
 *   \param [in] target - target object
 *   \param [out] source_type - type of source object
 *   \param [out] target_type - type of target object
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_cosq_fc_rec_src_target_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target,
    dnx_fc_rec_src_t * source_type,
    dnx_fc_rec_reaction_point_t * target_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Determine the reception source
     */
    if (BCM_GPORT_IS_CONGESTION(source->port))
    {
        *source_type = DNX_FC_REC_SRC_CAL;
    }
    else if (BCM_GPORT_IS_LOCAL(source->port) || BCM_GPORT_IS_LOCAL_INTERFACE(source->port))
    {
        if (source->cosq == BCM_COS_INVALID)
        {
            *source_type = DNX_FC_REC_SRC_LLFC;
        }
        else if ((source->cosq >= BCM_COS_MIN) && (source->cosq <= BCM_COS_MAX))
        {
            *source_type = DNX_FC_REC_SRC_PFC;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported COSQ %d", source->cosq);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unsupported Gport (0x%x)", source->port);
    }

    if (target_type != NULL)
    {
        /*
         * Determine the reception target
         */
        if (BCM_GPORT_IS_LOCAL_INTERFACE(target->port) ||
            ((target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT) && BCM_GPORT_IS_LOCAL_INTERFACE(source->port)))
        {
            if (target->cosq == BCM_COS_INVALID)
            {
                *target_type = DNX_FC_REC_REACTION_POINT_INTERFACE;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported COSQ %d", target->cosq);
            }
        }
        else if (BCM_GPORT_IS_LOCAL(target->port) ||
                 ((target->flags & BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT) && BCM_GPORT_IS_LOCAL(source->port)))
        {
            if (target->cosq == BCM_COS_INVALID)
            {
                *target_type = DNX_FC_REC_REACTION_POINT_PORT;
            }
            else if ((source->cosq >= BCM_COS_MIN) || (source->cosq <= BCM_COS_MAX))
            {
                *target_type = DNX_FC_REC_REACTION_POINT_PORT_COSQ;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported COSQ %d", target->cosq);
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "Unsupported Gport (0x%x)", target->port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * To associate ingress objects to a reaction FC in the generation path, or associating the received FC with a reaction egress object in the reception path.
 *
 * Use cases:
 *  ** Use case: "Generation from GLB RES to LLFC"
 *  Set the path for generation from Global recources to LLFC.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - VSQ Global Gport
 *        source->flags - BCM_COSQ_FC_BDB, BCM_COSQ_FC_HEADROOM or BCM_COSQ_FC_IS_OCB_ONLY
 *        source->cosq - 0 for Pool 0, 1 for Pool 1 and -1 for Total
 *        source->calendar - not relevant
 *        source->priority - 0 (HDRAM does not have low and high priority)
 *  - target
 *        target->port - Interface Gport or Local Gport
 *        target->flags - 0
 *        target->cosq - -1, not relevant
 *        target->calendar - not relevant
 *        target->priority - not relevant
 *
 * Use cases:
 *  ** Use case: "Generation from GLB RES to PFC"
 *  Set the path for generation from Global recources to PFC.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - VSQ Global Gport
 *        source->flags - BCM_COSQ_FC_BDB, BCM_COSQ_FC_HEADROOM or BCM_COSQ_FC_IS_OCB_ONLY
 *        source->cosq - 0 for Pool 0, 1 for Pool 1 and -1 for Total
 *        source->calendar - not relevant
 *        source->priority - 0 for high, 1 for low priority
 *  - target
 *        target->port - Interface Gport or Local Gport
 *        target->flags - 0
 *        target->cosq - [0,7]
 *        target->calendar - not relevant
 *        target->priority - not relevant
 *
 * Use cases:
 *  ** Use case: "Generation from GLB RES to OOB calendar"
 *  Set the path for generation from Global recources to OOB calendar.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - VSQ Global Gport
 *        source->flags - BCM_COSQ_FC_BDB, BCM_COSQ_FC_HEADROOM or BCM_COSQ_FC_IS_OCB_ONLY
 *        source->cosq - 0 for Pool 0, 1 for Pool 1 and -1 for Total
 *        source->calendar - not relevant
 *        source->priority - 0 for high, 1 for low priority
 *  - target
 *        target->port - Congestion Gport
 *        target->flags - 0
 *        target->cosq - -1, not relevant
 *        target->calendar - calender entry index
 *        target->priority - not relevant
 *
 *  ** Use case: "Generation from LLFC VSQ to LLFC"
 *  Set the path for generation from LLFC VSQ to LLFC.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - VSQ LLFC Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - not relevant
 *        source->priority - not relevant
 *  - target
 *        target->port - Interface Gport or Local Gport
 *        target->flags - 0 or BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT
 *        target->cosq - -1, not relevant
 *        target->calendar - not relevant
 *        target->priority - not relevant
 *
 *  ** Use case: "Generation from LLFC VSQ to OOB calendar"
 *  Set the path for generation from LLFC VSQ to OOB calendar.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - VSQ LLFC Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - not relevant
 *        source->priority - not relevant
 *  - target
 *        target->port - Congestion Gport
 *        target->flags - 0
 *        target->cosq - -1, not relevant
 *        target->calendar - calender entry index
 *        target->priority - not relevant
 *
 *  ** Use case: "Generation from PFC VSQ to OOB calendar"
 *  Set the path for generation from PFC VSQ to OOB calendar.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - VSQ PFC Gport
 *        source->flags - 0
 *        source->cosq - [0,7]
 *        source->calendar - not relevant
 *        source->priority - not relevant
 *  - target
 *        target->port - Congestion Gport
 *        target->flags - 0
 *        target->cosq - -1, not relevant
 *        target->calendar - calender entry index
 *        target->priority - not relevant
 *
 *  ** Use case: "Generation from Cat2 + TC VSQ to PFC"
 *  Set the path for generation from Cat2 + TC VSQ to PFC.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - VSQ Cat2 + TC Gport
 *        source->flags - 0 or BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT
 *        source->cosq - -1, not relevant
 *        source->calendar - not relevant
 *        source->priority - not relevant
 *  - target
 *        target->port - Interface Gport or Local Gport
 *        target->flags - 0
 *        target->cosq - [0,7]
 *        target->calendar - not relevant
 *        target->priority - not relevant
 *
 *  ** Use case: "Generation from Cat2 + TC VSQ to OOB calendar"
 *  Set the path for generation from Cat2 + TC VSQ to OOB calendar.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - VSQ Cat2 + TC Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - not relevant
 *        source->priority - not relevant
 *  - target
 *        target->port - Congestion Gport
 *        target->flags - 0
 *        target->cosq - -1, not relevant
 *        target->calendar - calender entry index
 *        target->priority - not relevant
 *
 *  ** Use case: "Generation from VSQ Group A-D to OOB calendar"
 *  Set the path for generation from VSQ Group A-D to OOB calendar.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - VSQ Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - not relevant
 *        source->priority - not relevant
 *  - target
 *        target->port - Congestion Gport
 *        target->flags - 0
 *        target->cosq - -1, not relevant
 *        target->calendar - calender entry index
 *        target->priority - not relevant
 *
 *  ** Use case: "Generation from Almost Full NIF to PFC"
 *  Set the path for generation from Almost Full NIF to PFC.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - Interface Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - not relevant
 *        source->priority - not relevant
 *  - target
 *        target->port - Interface Gport or Local Gport
 *        target->flags - 0 or BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT
 *        target->cosq - -1, not relevant
 *        target->calendar - not relevant
 *        target->priority - not relevant
 *
 *  ** Use case: "Generation from Almost Full NIF to LLFC"
 *  Set the path for generation from Almost Full NIF to LLFC.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - Interface Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - not relevant
 *        source->priority - not relevant
 *  - target
 *        target->port - Interface Gport or Local Gport
 *        target->flags - 0 or BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT
 *        target->cosq - [0,7]
 *        target->calendar - not relevant
 *        target->priority - not relevant
 *
 *  ** Use case: "Generation from Almost Full NIF to OOB calendar"
 *  Set the path for generation from Almost Full NIF to OOB calendar.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - Interface Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - not relevant
 *        source->priority - not relevant
 *  - target
 *        target->port - Congestion Gport
 *        target->flags - 0
 *        target->cosq - -1, not relevant
 *        target->calendar - calender entry index
 *        target->priority - not relevant
 *
 *  ** Use case: "Reception from LLFC to Interface reaction point"
 *  Set the path for reception from LLFC to Interface reaction point.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlReception
 *  - source
 *        source->port - Interface Gport or Local Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - -1, not relevant
 *        source->priority - not relevant
 *  - target
 *        target->port - Interface Gport
 *        target->flags - 0 or BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT
 *        target->cosq - -1, not relevant
 *        target->calendar - not relevant
 *        target->priority - not relevant
 *
 *  ** Use case: "Reception from PFC to port reaction point"
 *  Set the path for reception from PFC to port reaction point.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlReception
 *  - source
 *        source->port - Local Gport
 *        source->flags - 0
 *        source->cosq - [0,7]
 *        source->calendar - -1, not relevant
 *        source->priority - not relevant
 *  - target
 *        target->port - Local Gport
 *        target->flags - 0 or BCM_COSQ_FC_PORT_OVER_PFC
 *        target->cosq - -1, not relevant
 *        target->calendar - not relevant
 *        target->priority - not relevant
 *
 *  ** Use case: "Reception from PFC to port+COSQ reaction point"
 *  Set the path for reception from PFC to port+COSQ reaction point.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlReception
 *  - source
 *        source->port - Local Gport
 *        source->flags - 0
 *        source->cosq - [0,7]
 *        source->calendar - -1, not relevant
 *        source->priority - not relevant
 *  - target
 *        target->port - Local Gport
 *        target->flags - 0 or BCM_COSQ_FC_INHERIT_UP, BCM_COSQ_FC_INHERIT_DOWN,
                  BCM_COSQ_FC_PORT_OVER_PFC, BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT
 *        target->cosq - [0,7]
 *        target->calendar - not relevant
 *        target->priority - not relevant
 *
 *  ** Use case: "Reception from OOB calendar to Interface reaction point"
 *  Set the path for reception from OOB calendar to Interface reaction point.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlReception
 *  - source
 *        source->port - Congestion Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - calender entry index
 *        source->priority - not relevant
 *  - target
 *        target->port - Interface Gport
 *        target->flags - 0
 *        target->cosq - -1, not relevant
 *        target->calendar - not relevant
 *        target->priority - not relevant
 *
 *  ** Use case: "Reception from OOB calendar to Port reaction point"
 *  Set the path for reception from OOB calendar to Port reaction point.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlReception
 *  - source
 *        source->port - Congestion Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - calender entry index
 *        source->priority - not relevant
 *  - target
 *        target->port - Local Gport
 *        target->flags - 0
 *        target->cosq - -1, not relevant
 *        target->calendar - not relevant
 *        target->priority - not relevant
 *
 *  ** Use case: "Reception from OOB calendar to Port+COSQ reaction point"
 *  Set the path for reception from OOB calendar to Port+COSQ reaction point.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlReception
 *  - source
 *        source->port - Congestion Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - calender entry index
 *        source->priority - not relevant
 *  - target
 *        target->port - Local Gport
 *        target->flags - 0
 *        target->cosq - [0,7]
 *        target->calendar - not relevant
 *        target->priority - not relevant
 *
 *   \param [in] unit - unit
 *   \param [in] fc_direction - indicates if the path is reception or generation
 *   \param [in] source - source object
 *   \param [in] target - target object
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_fc_path_add(
    int unit,
    bcm_cosq_fc_direction_type_t fc_direction,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    dnx_fc_gen_src_t source_type_gen = DNX_FC_GENERATION_SOURCES;
    dnx_fc_gen_dest_t target_type_gen = DNX_FC_GENERATION_DESTINATIONS;
    dnx_fc_rec_src_t source_type_rec = DNX_FC_RECEPTION_SOURCES;
    dnx_fc_rec_reaction_point_t target_type_rec = DNX_FC_RECEPTION_REACTION_POINTS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(target, _SHR_E_PARAM, "target");
    SHR_NULL_CHECK(source, _SHR_E_PARAM, "source");

    if (fc_direction == bcmCosqFlowControlGeneration)
    {
        /** Verify input gport and get source and target type */
        SHR_IF_ERR_EXIT(dnx_cosq_fc_gen_src_target_get(unit, source, target, &source_type_gen, &target_type_gen));

        /*
         * Call the appropriate function to verify the case specific parameters
         */
        SHR_INVOKE_VERIFY_DNX(generation_source_destination_matrix[source_type_gen][target_type_gen].verify
                              (unit, FALSE, source, target));

        /*
         * Call the appropriate function to write to HW
         */
        SHR_IF_ERR_EXIT(generation_source_destination_matrix[source_type_gen][target_type_gen].map
                        (unit, FALSE, source, target));
    }
    else if (fc_direction == bcmCosqFlowControlReception)
    {
        /** Verify input gport and get source and target type */
        SHR_IF_ERR_EXIT(dnx_cosq_fc_rec_src_target_get(unit, source, target, &source_type_rec, &target_type_rec));

        /*
         * Call the appropriate function to verify the case specific parameters
         */
        SHR_INVOKE_VERIFY_DNX(reception_source_reaction_points_matrix[source_type_rec][target_type_rec].verify
                              (unit, FALSE, source, target));

        /*
         * Call the appropriate function to write to HW
         */
        SHR_IF_ERR_EXIT(reception_source_reaction_points_matrix[source_type_rec][target_type_rec].map
                        (unit, FALSE, source, target));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unit %d, invalid flow control direction %d", unit, fc_direction);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * To delete reception and generation paths.
 *
 *   \param [in] unit - unit
 *   \param [in] fc_direction - indicates if the path is reception or generation
 *   \param [in] source - source object
 *   \param [in] target - target object
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_fc_path_delete(
    int unit,
    bcm_cosq_fc_direction_type_t fc_direction,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    dnx_fc_gen_src_t source_type_gen = DNX_FC_GENERATION_SOURCES;
    dnx_fc_gen_dest_t target_type_gen = DNX_FC_GENERATION_DESTINATIONS;
    dnx_fc_rec_src_t source_type_rec = DNX_FC_RECEPTION_SOURCES;
    dnx_fc_rec_reaction_point_t target_type_rec = DNX_FC_RECEPTION_REACTION_POINTS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(target, _SHR_E_PARAM, "target");
    SHR_NULL_CHECK(source, _SHR_E_PARAM, "source");

    if (fc_direction == bcmCosqFlowControlGeneration)
    {
        /** Verify input gport and get source and target type */
        SHR_IF_ERR_EXIT(dnx_cosq_fc_gen_src_target_get(unit, source, target, &source_type_gen, &target_type_gen));

        /*
         * Call the appropriate function to verify the case specific parameters
         */
        SHR_INVOKE_VERIFY_DNX(generation_source_destination_matrix[source_type_gen][target_type_gen].verify
                              (unit, TRUE, source, target));

        /*
         * Call the appropriate function to write to HW
         */
        SHR_IF_ERR_EXIT(generation_source_destination_matrix[source_type_gen][target_type_gen].map
                        (unit, TRUE, source, target));
    }
    else if (fc_direction == bcmCosqFlowControlReception)
    {
        /** Verify input gport and get source and target type */
        SHR_IF_ERR_EXIT(dnx_cosq_fc_rec_src_target_get(unit, source, target, &source_type_rec, &target_type_rec));

        /*
         * Call the appropriate function to verify the case specific parameters
         */
        SHR_INVOKE_VERIFY_DNX(reception_source_reaction_points_matrix[source_type_rec][target_type_rec].verify
                              (unit, TRUE, source, target));

        /*
         * Call the appropriate function to write to HW
         */
        SHR_IF_ERR_EXIT(reception_source_reaction_points_matrix[source_type_rec][target_type_rec].map
                        (unit, TRUE, source, target));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unit %d, unsupported flow control direction %d", unit, fc_direction);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function to verify the parameters for flow control sources for generation
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] source_type_gen - holds information for the type of the source object
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_fc_gen_src_verify(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    dnx_fc_gen_src_t source_type_gen)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (source_type_gen)
    {
        case DNX_FC_GEN_SRC_GLB_RES_HIGH:
        case DNX_FC_GEN_SRC_GLB_RES_LOW:
        {
            /** Verify source flags */
            if (source->flags & ~(BCM_COSQ_FC_BDB | BCM_COSQ_FC_HEADROOM | BCM_COSQ_FC_IS_OCB_ONLY))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
            }

            /** Verify source COSQ */
            if ((source->cosq < -1) || (source->cosq > 1))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ");
            }
            break;
        }
        case DNX_FC_GEN_SRC_PFC_VSQ:
        {
            /** Verify source flags */
            if (source->flags != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
            }

            /** Verify source COSQ */
            if ((source->cosq < 0) || (source->cosq > 7))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ");
            }
            break;
        }
        case DNX_FC_GEN_SRC_CAT2_TC_VSQ:
        case DNX_FC_GEN_SRC_VSQ_A_D:
        case DNX_FC_GEN_SRC_ALMOST_FULL_NIF:
        case DNX_FC_GEN_SRC_LLFC_VSQ:
        {
            /** Verify source flags */
            if (source->flags != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
            }

            /** Verify source COSQ */
            if (source->cosq != BCM_COS_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The input source_type_gen(%d) supplied is not valid. \n", source_type_gen);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function to verify the parameters for flow control sources for reception
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] source - holds information for the source of the Flow control
 *   \param [in] source_type_rec - holds information for the type of the source object
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_cosq_fc_rec_src_verify(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    dnx_fc_rec_src_t source_type_rec)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (source_type_rec)
    {
        case DNX_FC_REC_SRC_LLFC:
        {
            /** Verify source flags */
            if (source->flags != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
            }

            /** Verify source calender_index */
            if (source->calender_index != -1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source calender_index, must be -1");
            }
            break;
        }
        case DNX_FC_REC_SRC_PFC:
        {
            /** Verify source flags */
            if (source->flags != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
            }

            /** Verify source calender_index */
            if (source->calender_index != -1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source calender_index, must be -1");
            }
            break;
        }
        case DNX_FC_REC_SRC_CAL:
        {
            /** Verify source flags */
            if (source->flags != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source flags.");
            }

            /** Verify source COSQ */
            if (source->cosq != BCM_COS_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source COSQ, must be -1");
            }

             /** Verify source calender_index */
            if ((source->calender_index < 0) || (source->calender_index > 511))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source calender_index");
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The input source_type_rec(%d) supplied is not valid. \n", source_type_rec);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * To get reception reaction points and generation targets.
 * Use cases:
 *  ** Use case: "Generation from GLB RES"
 *  Return the targets of generation from Global recources.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - VSQ Global Gport
 *        source->flags - BCM_COSQ_FC_BDB, BCM_COSQ_FC_HEADROOM or BCM_COSQ_FC_IS_OCB_ONLY
 *        source->cosq - 0 for Pool 0, 1 for Pool 1 and -1 for Total
 *        source->calendar - not relevant
 *        source->priority - 0 for high, 1 for low priority
 *  - target_max - maximum size of target array
 *
 *  ** Use case: "Generation from LLFC VSQ"
 *  Return the targets of generation from LLFC VSQ.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - VSQ LLFC Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - not relevant
 *        source->priority - not relevant
 *  - target_max - maximum size of target array
 *
 *  ** Use case: "Generation from PFC VSQ"
 *  Return the targets of generation from PFC VSQ.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - VSQ PFC Gport
 *        source->flags - 0
 *        source->cosq - [0,7]
 *        source->calendar - not relevant
 *        source->priority - not relevant
 *  - target_max - maximum size of target array
 *
 *  ** Use case: "Generation from Cat2 + TC VSQ"
 *  Return the targets of generation from Cat2 + TC VSQ.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - VSQ Cat2 + TC Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - not relevant
 *        source->priority - not relevant
 *  - target_max - maximum size of target array
 *
 *  ** Use case: "Generation from VSQ Group A-D"
 *  Return the targets of generation from VSQ Group A-D.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - VSQ Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - not relevant
 *        source->priority - not relevant
 *  - target_max - maximum size of target array
 *
 *  ** Use case: "Generation from Almost Full NIF"
 *  Return the targets of generation from Almost Full NIF.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlGeneration
 *  - source
 *        source->port - Interface Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - not relevant
 *        source->priority - not relevant
 *  - target_max - maximum size of target array
 *
 *  ** Use case: "Reception from LLFC"
 *  Return the targets of reception from LLFC.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlReception
 *  - source
 *        source->port - Interface Gport or Local Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - -1, not relevant
 *        source->priority - not relevant
 *  - target_max - maximum size of target array
 *
 *  ** Use case: "Reception from PFC"
 *  Return the targets of reception from PFC.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlReception
 *  - source
 *        source->port - Interface Gport or Local Gport
 *        source->flags - 0
 *        source->cosq - [0,7]
 *        source->calendar - -1, not relevant
 *        source->priority - not relevant
 *  - target_max - maximum size of target array
 *
 *  ** Use case: "Reception from OOB calendar"
 *  Return the targets of reception from OOB calendar.
 *  Parameters:
 *  - fc_direction - bcmCosqFlowControlReception
 *  - source
 *        source->port - Congestion Gport
 *        source->flags - 0
 *        source->cosq - -1, not relevant
 *        source->calendar - calender entry index
 *        source->priority - not relevant
 *  - target_max - maximum size of target array
 *
 *   \param [in] unit - unit
 *   \param [in] fc_direction - indicates if the path is reception or generation
 *   \param [in] source - source object
 *   \param [in] target_max - maximum size of target array
 *   \param [out] target - target object array
 *   \param [out] target_count - updated size(valid entries) of target array
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_cosq_fc_path_get(
    int unit,
    bcm_cosq_fc_direction_type_t fc_direction,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    dnx_fc_gen_src_t source_type_gen = DNX_FC_GENERATION_SOURCES;
    dnx_fc_rec_src_t source_type_rec = DNX_FC_RECEPTION_SOURCES;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(target, _SHR_E_PARAM, "target");
    SHR_NULL_CHECK(target_count, _SHR_E_PARAM, "target_count");
    sal_memset(target, 0, sizeof(bcm_cosq_fc_endpoint_t) * target_max);
    sal_memset(target_count, 0, sizeof(uint32));

    if (fc_direction == bcmCosqFlowControlGeneration)
    {
        /** Verify input gport and get source for generation */
        SHR_IF_ERR_EXIT(dnx_cosq_fc_gen_src_target_get(unit, source, target, &source_type_gen, NULL));

        /*
         * Call the appropriate function to verify the case specific parameters
         */
        SHR_INVOKE_VERIFY_DNX(bcm_dnx_cosq_fc_gen_src_verify(unit, source, source_type_gen));

        /*
         * Call the appropriate function to read from HW
         */
        switch (source_type_gen)
        {
            case DNX_FC_GEN_SRC_GLB_RES_HIGH:
            case DNX_FC_GEN_SRC_GLB_RES_LOW:
            {
                /** Use case: Generation from GLB RES */
                SHR_IF_ERR_EXIT(dnx_fc_gen_glb_res_target_get(unit, source, target_max, target, target_count));
                break;
            }
            case DNX_FC_GEN_SRC_LLFC_VSQ:
            {
                /** Use case: Generation from LLFC VSQ */
                SHR_IF_ERR_EXIT(dnx_fc_gen_llfc_vsq_target_get(unit, source, target_max, target, target_count));
                break;
            }
            case DNX_FC_GEN_SRC_PFC_VSQ:
            {
                /** Use case: Generation from PFC VSQ */
                SHR_IF_ERR_EXIT(dnx_fc_gen_pfc_vsq_target_get(unit, source, target_max, target, target_count));
                break;
            }
            case DNX_FC_GEN_SRC_CAT2_TC_VSQ:
            {
                /** Use case: Generation from Cat2 + TC VSQ */
                SHR_IF_ERR_EXIT(dnx_fc_gen_c2_tc_vsq_target_get(unit, source, target_max, target, target_count));
                break;
            }
            case DNX_FC_GEN_SRC_VSQ_A_D:
            {
                /** Use case: Generation from VSQ Group A-D */
                SHR_IF_ERR_EXIT(dnx_fc_gen_vsq_a_d_target_get(unit, source, target_max, target, target_count));
                break;
            }
            case DNX_FC_GEN_SRC_ALMOST_FULL_NIF:
            {
                /** Use case: Generation from Almost Full NIF */
                SHR_IF_ERR_EXIT(dnx_fc_gen_nif_target_get(unit, source, target_max, target, target_count));
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "The reception source type is not valid. \n");
                break;
            }
        }
    }
    else if (fc_direction == bcmCosqFlowControlReception)
    {
        /** Verify input gport and get source for reception */
        SHR_IF_ERR_EXIT(dnx_cosq_fc_rec_src_target_get(unit, source, target, &source_type_rec, NULL));

        /*
         * Call the appropriate function to verify the case specific parameters
         */
        SHR_INVOKE_VERIFY_DNX(bcm_dnx_cosq_fc_rec_src_verify(unit, source, source_type_rec));

        /*
         * Call the appropriate function to read from HW
         */
        switch (source_type_rec)
        {
            case DNX_FC_REC_SRC_LLFC:
            case DNX_FC_REC_SRC_PFC:
            {
                /** Use case: Reception from LLFC */
                /** Use case: Reception from PFC */
                SHR_IF_ERR_EXIT(dnx_fc_rec_llfc_pfc_react_point_get(unit, source, target_max, target, target_count));
                break;
            }
            case DNX_FC_REC_SRC_CAL:
            {
                /** Use case: Reception from Calendar(OOB, COE) */
                SHR_IF_ERR_EXIT(dnx_fc_rec_cal_react_point_get(unit, source, target_max, target, target_count));
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "The reception source type is not valid. \n");
                break;
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unit %d, unsupported flow control direction %d", unit, fc_direction);
    }

exit:
    SHR_FUNC_EXIT;
}

 /*
  * See header file for description.
  */

void
dnx_fc_generic_bmp_profile_template_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    bcm_core_t core = 0;
    dnx_pfc_generic_bmp_t *pfc_generic_bmp = (dnx_pfc_generic_bmp_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core)
    {
        DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_INT, "Core", core, "PFC Generic Bitmaps for Core",
                                            NULL);
        {
            DNX_ALGO_TEMPLATE_PRINT_CB_SUB_STRUCT_START();
            DNX_ALGO_TEMPLATE_PRINT_CB_ADD_ARR(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "Bitmap",
                                               pfc_generic_bmp->qpair_bits[core], NULL, "0x%x",
                                               (DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS / (sizeof(uint32) * 8)));
            DNX_ALGO_TEMPLATE_PRINT_CB_SUB_STRUCT_END();
        }
    }
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
}

/**
 * \brief -
 * To init Flow Control related memories and registers.
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_fc_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 fld_val;
    uint32 interface_number, coe_chan_id;
    int imb_id;
    uint32 nof_imbs;
    imb_dispatch_type_t type;
    dnx_pfc_generic_bmp_t default_data;
    sw_state_algo_template_create_data_t fc_generic_bmp;
    uint32 max_bmp_idx, max_ref, prio;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flow_control_db.init(unit));

    /** Get the number of Generic PFC Bitmaps in order to create a resource in the resource manager for allocating indexes */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_FC_GENERIC_BITMAP,
                                                          DBAL_FIELD_BMP_IDX,
                                                          TRUE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, &max_bmp_idx));
    /*
     * Calulate the maf references for the template - theoretically it is possible that the entire OOB calendar
     * (both interfaces) and all PFCs will be mapped to only one bitmap, so max_ref must be the size of the calendars + all PFCs.
     */
    for (interface_number = 0; interface_number < SOC_DNX_FC_NOF_OOB_IDS; interface_number++)
    {
        max_ref = +dnx_data_fc.oob.calender_length_get(unit, interface_number)->rx;
    }
    max_ref = +dnx_data_egr_queuing.params.nof_q_pairs_get(unit);

    /** Create template manager insance - handle: RESOURCE_FC_GENERIC_BMP for the index of generic PFC bitmaps */
    sal_memset(&default_data, 0, sizeof(dnx_pfc_generic_bmp_t));
    sal_memset(&fc_generic_bmp, 0, sizeof(sw_state_algo_template_create_data_t));
    fc_generic_bmp.flags = SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    fc_generic_bmp.first_profile = 0;
    /*
     * The max Bitmap index is 31, which means we have 32 in total Generic Bitmaps in HW (0-31).
     * Number of profiles is 33 and not 32 because the profile with index 32 is used as a default profile and does not correspond
     * to a HW Generic Bitmap. This is done so that we don't take one of the indexes existing in HW for deault profile and with 
     * that lower the number of usable bitmaps.
     */
    fc_generic_bmp.nof_profiles = max_bmp_idx + 2;
    fc_generic_bmp.default_profile = max_bmp_idx + 1;
    fc_generic_bmp.data_size = sizeof(dnx_pfc_generic_bmp_t);
    fc_generic_bmp.default_data = &default_data;
    fc_generic_bmp.max_references = max_ref;
    sal_strncpy(fc_generic_bmp.name, RESOURCE_FC_GENERIC_BMP, SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(flow_control_db.fc_generic_bmp.create(unit, &fc_generic_bmp, NULL));

    /*
     * Configure the NIF Flow Control Interface to process indications from CFC.
     * If set, the Flow Control Interface will be held in reset, so set to 0 to get it out of reset.
     */
    SHR_IF_ERR_EXIT(imb_fc_reset_all(unit, FALSE));

    /** Set the CDUs to enable the generation of LLFC based on Global Resources */
    SHR_IF_ERR_EXIT(imb_llfc_from_glb_rsc_enable_all(unit, TRUE));

    /*
     * Get number of IMB instances in use
     */
    SHR_IF_ERR_EXIT(imb_nof_get(unit, &nof_imbs));

    /** Per CDU map the received PFC - each priority received by NIF should be mapped to a priority that will be sent to EGQ in order to stop a Q-pair */
    for (imb_id = 0; imb_id < nof_imbs; imb_id++)
    {
        /** Chec type, mapping can be set only for CDU type IMBs */
        SHR_IF_ERR_EXIT(imb_id_type_get(unit, imb_id, &type));
        if (type != imbDispatchTypeImb_cdu)
        {
            continue;
        }

        /** Unmap the priorities of the PFC signal received from RX Port Macro to clear any pre-existing mapping */
        SHR_IF_ERR_EXIT(imb_pfc_rec_priority_unmap(unit, imb_id));
        for (prio = 0; prio < BCM_COS_COUNT; prio++)
        {
            /** Map the priorities of the PFC signal received from RX Port Macro, default is 1 to 1 mapping */
            SHR_IF_ERR_EXIT(imb_pfc_rec_priority_map(unit, imb_id, prio, prio));
        }
    }

    /** Global resources masking, bit 0 - LP mask, bit 1 - HP mask */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_GLB_RSC_MASKS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_GLB_PRIO, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_POOL_0_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_POOL_0_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_POOL_1_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_POOL_1_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /** HDRAM does not have low and high priority */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_PRIO, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HDRM_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Enable Category 2 & Traffic class mapping to PFC */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_CAT2_TC_MAP_PFC_ENA, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_TC, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** CFC Enablers for flow-control */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_CFC_ENABLERS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RSC_TO_RCL_PFC_HP_CFG, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RSC_TO_RCL_PFC_LP_CFG, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RSC_TO_EGQ_RCL_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_RX_TO_EGQ_PORT_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_RX_TO_EGQ_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_RX_TO_GEN_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_RX_TO_EGQ_PORT_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_RX_TO_EGQ_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_RX_TO_GEN_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_RX_TO_NIF_FAST_LLFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_ILKN_RX_TO_EGQ_PORT_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_ILKN_RX_TO_EGQ_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_ILKN_RX_TO_GEN_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_ILKN_RX_TO_NIF_FAST_LLFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RSC_TO_NIF_LLFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLB_RSC_TO_NIF_PFC_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LLFC_VSQ_TO_NIF_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PFC_VSQ_TO_NIF_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_RX_TO_NIF_FAST_LLFC_EN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** OOB configurations - take into reset */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_OOB_PARAMETERS, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_RX_RSTN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_TX_RSTN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Configure OOB Type: ILKN/SPI
     */
    for (interface_number = 0; interface_number < SOC_DNX_FC_NOF_OOB_IDS; interface_number++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, interface_number);
        fld_val = (dnx_data_fc.oob.port_get(unit, interface_number)->type == SOC_DNX_FC_CAL_TYPE_ILKN ? 1 : 0);

        if ((dnx_data_fc.oob.port_get(unit, interface_number)->mode == DNX_FC_CAL_MODE_RX_ENABLE) ||
            (dnx_data_fc.oob.port_get(unit, interface_number)->mode == DNX_FC_CAL_MODE_TX_RX_ENABLE))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_RX_MODE, INST_SINGLE, fld_val);
            /** If type is ILKN */
            if (dnx_data_fc.oob.port_get(unit, interface_number)->type == SOC_DNX_FC_CAL_TYPE_ILKN)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_RX_FC_POLARITY, INST_SINGLE,
                                             dnx_data_fc.oob.indication_invert_get(unit, interface_number)->ilkn_rx);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_RX_CAL_LEN, INST_SINGLE,
                                             (dnx_data_fc.oob.calender_length_get(unit, interface_number)->rx - 1));
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_RX_EN, INST_SINGLE, 1);
            }

            /** If type is SPI */
            if (dnx_data_fc.oob.port_get(unit, interface_number)->type == SOC_DNX_FC_CAL_TYPE_SPI)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_POLARITY, INST_SINGLE,
                                             dnx_data_fc.oob.indication_invert_get(unit, interface_number)->spi);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_RX_CAL_LEN, INST_SINGLE,
                                             (dnx_data_fc.oob.calender_length_get(unit, interface_number)->rx - 1));
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_RX_CAL_M, INST_SINGLE,
                                             dnx_data_fc.oob.calender_rep_count_get(unit, interface_number)->rx);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_RX_EN, INST_SINGLE, 1);
            }
        }

        if ((dnx_data_fc.oob.port_get(unit, interface_number)->mode == DNX_FC_CAL_MODE_TX_ENABLE) ||
            (dnx_data_fc.oob.port_get(unit, interface_number)->mode == DNX_FC_CAL_MODE_TX_RX_ENABLE))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_TX_MODE, INST_SINGLE, fld_val);
            /** If type is ILKN */
            if (dnx_data_fc.oob.port_get(unit, interface_number)->type == SOC_DNX_FC_CAL_TYPE_ILKN)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_TX_FC_POLARITY, INST_SINGLE,
                                             dnx_data_fc.oob.indication_invert_get(unit, interface_number)->ilkn_tx);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_TX_CAL_LEN, INST_SINGLE,
                                             (dnx_data_fc.oob.calender_length_get(unit, interface_number)->tx - 1));
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_OOB_TX_EN, INST_SINGLE, 1);
            }

            /** If type is SPI */
            if (dnx_data_fc.oob.port_get(unit, interface_number)->type == SOC_DNX_FC_CAL_TYPE_SPI)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_POLARITY, INST_SINGLE,
                                             dnx_data_fc.oob.indication_invert_get(unit, interface_number)->spi);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_TX_CAL_LEN, INST_SINGLE,
                                             (dnx_data_fc.oob.calender_length_get(unit, interface_number)->tx - 1));
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_TX_CAL_M, INST_SINGLE,
                                             dnx_data_fc.oob.calender_rep_count_get(unit, interface_number)->tx);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SPI_OOB_TX_EN, INST_SINGLE, 1);
            }

            /** OOB TX CLK Speed ratio, phase and rough phase*/
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_TX_OUT_SPEED, INST_SINGLE,
                                         dnx_data_fc.oob.tx_freq_get(unit, interface_number)->ratio);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_TX_OUT_PHASE, INST_SINGLE, 1);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_TX_OUT_ROUGH_PHASE, INST_SINGLE, 1);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** OOB configurations - take out of reset */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_RX_RSTN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_TX_RSTN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** COE settings - enable, set mode, set tick, Control opcode, MAC address, data and VID offsets, Ether type */
    for (interface_number = 0; interface_number < dnx_data_fc.coe.nof_coe_cal_instances_get(unit); interface_number++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_COE_PARAMETERS, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_IF, interface_number);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_INB_ENB, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_COE_MODE, INST_SINGLE,
                                     dnx_data_fc.coe.calendar_mode_get(unit)->mode);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_COE_TICK_MHZ, INST_SINGLE,
                                     dnx_data_fc.coe.calendar_pause_resolution_get(unit) *
                                     (dnx_data_device.general.core_clock_khz_get(unit) / 1000));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_CONTROL_OPCODE, INST_SINGLE,
                                     dnx_data_fc.coe.control_opcode_get(unit));
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_COE_MAC_ADDRESS, INST_SINGLE,
                                        (uint8 *) dnx_data_fc.coe.mac_address_get(unit)->mac);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_ETHER_TYPE, INST_SINGLE,
                                     dnx_data_fc.coe.ethertype_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_DATA_OFFSET, INST_SINGLE,
                                     dnx_data_fc.coe.data_offset_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_VID_OFFSET, INST_SINGLE, DNX_COE_VID_OFFSET);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_RX_EN, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_RX_CAL_LEN, INST_SINGLE,
                                     (dnx_data_fc.coe.calender_length_get(unit, interface_number)->len - 1));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Map VID to Channel ID 1x1  - mapping VLAN IDs from 0 to COE calendar length to the corresponding COE channels */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_COE_VID_CHANNEL_ID_MAP, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_IF, interface_number);
        for (coe_chan_id = 0; coe_chan_id < dnx_data_fc.coe.calender_length_get(unit, interface_number)->len;
             coe_chan_id++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID, coe_chan_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COE_CHANNEL_ID, INST_SINGLE, coe_chan_id);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

    /** Set the initial values for the Map Select in OOB TX calendar, for the value of FC_INDEX we are relying on the reset value which is 0 */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_GEN_OOB_MAP, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OOB_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FC_SRC_SEL, INST_SINGLE, DNX_OOB_TX_CAL_SRC_CONST);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Set the initial values for the Map Select in OOB RX calendar, for the value of MAP_VAL we are relying on the reset value which is 0 */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_REC_OOB_CAL_MAP, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEST_SEL, INST_SINGLE, DNX_RX_CAL_DEST_CONST);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Set the initial values for the Map Select in COE RX calendar, for the value of MAP_VAL we are relying on the reset value which is 0 */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_REC_COE_CAL_MAP, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IF, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CAL_IDX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEST_SEL, INST_SINGLE, DNX_RX_CAL_DEST_CONST);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * To de-init Flow Control
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_fc_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
