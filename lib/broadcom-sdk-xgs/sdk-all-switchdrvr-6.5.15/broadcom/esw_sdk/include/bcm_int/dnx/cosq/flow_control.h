/** \file include/bcm_int/dnx/cosq/flow_control.h
 * $Id$
 *
 * Flow control API functions for DNX. \n
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _SRC_DNX_FC_H_INCLUDED_
/** { */
#define _SRC_DNX_FC_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/cosq.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>

/*
 * Defines
 * {
 */

 /**
 * \brief - Handle for template manager
 */
#define RESOURCE_FC_GENERIC_BMP "RESOURCE_FC_GENERIC_BMP"

 /**
 * \brief - Default profile for template manager
 */
#define GENERIC_BMP_DEFAULT_PROFILE_ID (32)

 /*
  * }
  */

 /*
  * Typedefs
  * {
  */

/**
 * \brief - Data type for the PFC Generic Bitmap template manager
 */
typedef struct pfc_generic_bmp
{
    uint32 qpair_bits[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS /
                                                                 (sizeof(uint32) * 8)];
} dnx_pfc_generic_bmp_t;

/**
 * \brief - Type definition for the functions in the generation/reception matrix
 */
typedef shr_error_e(
    *fc_cb_t) (
    int,
    int,
    bcm_cosq_fc_endpoint_t *,
    bcm_cosq_fc_endpoint_t *);

/**
 * \brief - Type definition for the generation/reception matrix
 */
typedef struct fc_matrix
{
    fc_cb_t map;
    fc_cb_t verify;
} fc_matrix_t;

/**
 * \brief - Enumerator for the OOB port modes
 */
typedef enum
{
        /** Disabled */
    DNX_FC_CAL_MODE_DISABLE = 0,
        /** RX enabled */
    DNX_FC_CAL_MODE_RX_ENABLE = 1,
        /** TX enabled */
    DNX_FC_CAL_MODE_TX_ENABLE = 2,
        /** Both RX and TX enabled */
    DNX_FC_CAL_MODE_TX_RX_ENABLE = 3,
        /** Number of OOB port modes */
    DNX_FC_CAL_MODES
} dnx_fc_cal_medes_t;

/**
 * \brief - Enumerator for Flow control generation sources
 */
typedef enum
{
        /** Global resources High priority */
    DNX_FC_GEN_SRC_GLB_RES_HIGH = 0,
        /** Global resources Low priority */
    DNX_FC_GEN_SRC_GLB_RES_LOW = 1,
        /** LLFC VSQs */
    DNX_FC_GEN_SRC_LLFC_VSQ = 2,
        /** PFC VSQ */
    DNX_FC_GEN_SRC_PFC_VSQ = 3,
        /** Category 2 and Traffic Class */
    DNX_FC_GEN_SRC_CAT2_TC_VSQ = 4,
        /** VSQs from A to D */
    DNX_FC_GEN_SRC_VSQ_A_D = 5,
        /** Almost full NIF */
    DNX_FC_GEN_SRC_ALMOST_FULL_NIF = 6,
        /** Number of generation sources */
    DNX_FC_GENERATION_SOURCES
} dnx_fc_gen_src_t;

/**
 * \brief - Enumerator for Flow control generation destinations
 */
typedef enum
{
        /** LLFC */
    DNX_FC_GEN_DEST_LLFC = 0,
        /** PFC */
    DNX_FC_GEN_DEST_PFC = 1,
        /** OOB calendar */
    DNX_FC_GEN_DEST_OOB_CAL = 2,
        /** Number of generation destinations */
    DNX_FC_GENERATION_DESTINATIONS
} dnx_fc_gen_dest_t;

/**
 * \brief - Enumerator for Flow control reception sources
 */
typedef enum
{
        /** LLFC */
    DNX_FC_REC_SRC_LLFC = 0,
        /** PFC */
    DNX_FC_REC_SRC_PFC = 1,
        /** Calendar(OOB, COE) */
    DNX_FC_REC_SRC_CAL = 2,
        /** Number of reception sources */
    DNX_FC_RECEPTION_SOURCES
} dnx_fc_rec_src_t;

/**
 * \brief - Enumerator for Flow control reception reaction points
 */
typedef enum
{
        /** Interface */
    DNX_FC_REC_REACTION_POINT_INTERFACE = 0,
        /** Port */
    DNX_FC_REC_REACTION_POINT_PORT = 1,
        /** Port + COSQ */
    DNX_FC_REC_REACTION_POINT_PORT_COSQ = 2,
        /** Number of reception reaction points */
    DNX_FC_RECEPTION_REACTION_POINTS
} dnx_fc_rec_reaction_point_t;

 /*
  * }
  */

/*
 * Functions
 * {
 */

/**
 * \brief -
 *   Print the PFC Generic Bitmap template content.
 * \param [in] unit - relevant unit
 * \param [in] data - template data
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 */
void dnx_fc_generic_bmp_profile_template_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

shr_error_e bcm_dnx_fc_init(
    int unit);
shr_error_e bcm_dnx_fc_deinit(
    int unit);

 /*
  * }
  */

#endif /** _SRC_DNX_FC_H_INCLUDED_ */
