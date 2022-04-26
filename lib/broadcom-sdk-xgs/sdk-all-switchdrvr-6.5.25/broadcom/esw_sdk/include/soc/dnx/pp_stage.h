/** \file soc/dnx/pp_stage.h
 *
 *  Created on: Oct 30, 2017
 *      Author: mf954075
 */

#ifndef INCLUDE_SOC_DNX_PP_STAGE_H_
#define INCLUDE_SOC_DNX_PP_STAGE_H_

#include <shared/shrextend/shrextend_error.h>
#include <sal/types.h>

#define MAX_DEBUG_BUS_SIZE_BITS      2560
#define MAX_DEBUG_BUS_SIZE_WORDS     BITS2WORDS(MAX_DEBUG_BUS_SIZE_BITS)

typedef struct
{
    char *name;
    char *direction;
} dnx_pp_block_info_t;
/**
 * Describe available stages in PP
 */
typedef enum
{
    /**
     * Invalid PP Stage
     */
    DNX_PP_STAGE_INVALID = 0,
    DNX_PP_STAGE_FIRST = 1,
    DNX_PP_STAGE_PORT_IN = DNX_PP_STAGE_INVALID,
    DNX_PP_STAGE_PRD_PARSER = DNX_PP_STAGE_INVALID,
    /**
     * Network Interface
     */
    DNX_PP_STAGE_NIF = DNX_PP_STAGE_PRD_PARSER + 1,
    /**
     * Port Termination Stage - resolves L2 channelization and set initial parser program
     */
    DNX_PP_STAGE_PRT,
    /**
     * Ingress Stages
     */
    DNX_PP_STAGE_IRPP_FIRST,
    /**
     * Ingress Parser
     */
    DNX_PP_STAGE_IPARSER = DNX_PP_STAGE_IRPP_FIRST,
    /**
     * Ingress Link Layer
     */
    DNX_PP_STAGE_LLR,
    /**
     * Ethernet Termination
     */
    DNX_PP_STAGE_VTT1,
    /**
     * Ethernet Termination
     */
    DNX_PP_STAGE_VTT2,
    /**
     * Tunnel Termination
     */
    DNX_PP_STAGE_VTT3,
    /**
     * Tunnel Termination
     */
    DNX_PP_STAGE_VTT4,
    /**
     * Tunnel Termination
     */
    DNX_PP_STAGE_VTT5,
    /**
     * Ingress Forwarding 1
     */
    DNX_PP_STAGE_IFWD1,
    /**
     * Ingress Forwarding 2
     */
    DNX_PP_STAGE_IFWD2,
    /**
     * Ingress PMF 1 (Pass 0 of PMF-A)
     */
    DNX_PP_STAGE_IPMF1,
    /**
     * Ingress PMF 2 (Pass 1 of PMF-A)
     */
    DNX_PP_STAGE_IPMF2,
    /**
     * FEC resolution
     */
    DNX_PP_STAGE_FER,
    /**
     * Ingress PMF 3 (Rename of PMF-B)
     */
    DNX_PP_STAGE_IPMF3,
    /**
     * System Header Editor Block
     */
    DNX_PP_STAGE_LBP,
    /**
     * Resolution before Ingress Traffic Manager
     */
    DNX_PP_STAGE_ITMR,
    /**
     * Ingress Traffic Manager
     */
    DNX_PP_STAGE_ITM,
    /**
     * Actually it is not stage but block with only one stage
     */
    DNX_PP_STAGE_ITPP,
    /**
     * ERPP Stages
     */
    DNX_PP_STAGE_ERPP_FIRST,
    /**
     * Represents signals from fabric
     */
    DNX_PP_STAGE_FABRIC = DNX_PP_STAGE_ERPP_FIRST,
    /**
     * ERPP Parser
     */
    DNX_PP_STAGE_ERPARSER,
    /**
     * ERPP Preprocessor
     */
    DNX_PP_STAGE_ERPRP,
    /**
     * ERPP Protocol Filter
     */
    DNX_PP_STAGE_FILTER,
    /**
     * Egress PMF - single one at egress.
     */
    DNX_PP_STAGE_EPMF,
    /**
     * Egress Traffic Manager Resolution
     */
    DNX_PP_STAGE_ETMR,
    /**
     * Egress Traffic Manager - between ERPP and ETPP
     */
    DNX_PP_STAGE_ETM,
    /**
     * ETPP Stages
     */
    DNX_PP_STAGE_ETPP_FIRST,
    /**
     * ETPP Applet
     */
    DNX_PP_STAGE_APPLET = DNX_PP_STAGE_ETPP_FIRST,
    /**
     * ETPP Parser
     */
    DNX_PP_STAGE_ETPARSER,
    /**
     *
     */
    DNX_PP_STAGE_ETPRP1,
    /**
     *
     */
    DNX_PP_STAGE_ETPRP2,
    /**
     *
     */
    DNX_PP_STAGE_TERM,
    /**
     *
     */
    DNX_PP_STAGE_EFWD,
    /**
     *
     */
    DNX_PP_STAGE_ENC1,
    /**
     *
     */
    DNX_PP_STAGE_ENC2,
    /**
     *
     */
    DNX_PP_STAGE_ENC3,
    /**
     *
     */
    DNX_PP_STAGE_ENC4,
    /**
     *
     */
    DNX_PP_STAGE_ENC5,
    /**
     *
     */
    DNX_PP_STAGE_TRAP,
    /**
     *
     */
    DNX_PP_STAGE_BTC,
    /**
     *
     */
    DNX_PP_STAGE_ALIGNER,
    /**
     *
     */
    DNX_PP_STAGE_EPNI,
    /**
     * Virtual stage - PPMC table and ACE ACR.
     */
    DNX_PP_STAGE_ACE,
    /**
     * Number of elements in dnx_pp_stage_e
     */
    DNX_PP_STAGE_NOF
} dnx_pp_stage_e;

/**
 * present all available physical DBs in PP
 */
typedef enum
{
    /**
     * Invalid Physical DB
     */
    DNX_PP_DB_INVALID = 0,
    DNX_PP_DB_FIRST = 1,
    /**
     *
     */
    DNX_PP_DB_INLIF1 = DNX_PP_DB_FIRST,
    /**
     *
     */
    DNX_PP_DB_INLIF2,
    /**
     *
     */
    DNX_PP_DB_VSI_DB,
    /**
     *
     */
    DNX_PP_DB_TCAM,
    /**
     *
     */
    DNX_PP_DB_LEXEM,
    /**
     *
     */
    DNX_PP_DB_GLEM,
    /**
     * Number of elements in dnx_pp_db_e
     */
    DNX_PP_DB_NOF
} dnx_pp_db_e;

/**
 * present all available physical DBs in PP
 */
typedef enum
{
    /**
     * Invalid Physical DB
     */
    DNX_PP_BLOCK_INVALID = 0,
    DNX_PP_BLOCK_FIRST = 1,
    /**
     *
     */
    DNX_PP_BLOCK_IRPP = DNX_PP_BLOCK_FIRST,
    /**
     *
     */
    DNX_PP_BLOCK_ITPP,
    /**
     *
     */
    DNX_PP_BLOCK_ERPP,
    /**
     *
     */
    DNX_PP_BLOCK_ETPP,
    /**
     * Number of elements in dnx_pp_db_e
     */
    DNX_PP_BLOCK_NOF
} dnx_pp_block_e;

/*
 * PP Stages API
 * { 
 */

/**
 * \brief  Get block id of a given stage
 * \param [in] unit  - Identifier of HW platform.
 * \param [in] stage - stage ID
 * \param [out] block - block ID
 * \return
 *   \retval shr_error_e
 * \remark
 */
shr_error_e dnx_pp_get_block_id(
    int unit,
    int stage,
    int *block);

/**
 * \brief  Return name of pipeline stage
 * \param [in] unit  - Identifier of HW platform.
 * \param [in] stage - stage ID
 * \return
 *   \retval 1       - If stage is valid and recognized as kleap one
 *   \retval 0       - Any other result, including out of range id and non-kleap
 * \remark
 */
int dnx_pp_stage_is_kleap(
    int unit,
    dnx_pp_stage_e stage);
/**
 * \brief  Return name of pipeline stage
 * \param [in] unit  - Identifier of HW platform.
 * \param [in] pp_stage - stage ID
 * \return
 *   \retval stage name - On success
 *   \retval NULL       - On Failure
 * \remark
 */
char *dnx_pp_stage_name(
    int unit,
    dnx_pp_stage_e pp_stage);

/**
 * \brief  Return ID of pipeline stage
 * \param [in] unit  - Identifier of HW platform. 
 * \param [in] pp_stage_name  - stage Name. 
 * \param [out] pp_stage_id - pointer to stage ID
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 */
shr_error_e dnx_pp_stage_string_to_id(
    int unit,
    char *pp_stage_name,
    dnx_pp_stage_e * pp_stage_id);

/**
 * \brief  Return ID of pipeline stage without printing an error
 * \param [in] unit  - Identifier of HW platform.
 * \param [in] pp_stage_name  - stage Name.
 * \param [out] pp_stage_id - pointer to stage ID
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 */
shr_error_e dnx_pp_stage_string_to_id_no_error(
    int unit,
    char *pp_stage_name,
    dnx_pp_stage_e * pp_stage_id);

/**
 * \brief  Return Pema ID of pipeline stage
 * \param [in] unit  - Identifier of HW platform.
 * \param [in] pp_stage_name  - stage Name.
 * \param [out] pema_id_p - pointer to stage pema ID
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 */
shr_error_e dnx_pp_stage_pema_id(
    int unit,
    char *pp_stage_name,
    int *pema_id_p);

/**
 * \brief  Return name of pipeline stage
 * \param [in] unit  - Identifier of HW platform.
 * \param [in] pp_block - block ID
 * \return
 *   \retval block name - On success
 *   \retval NULL       - On Failure
 * \remark
 */
char *dnx_pp_block_name(
    int unit,
    dnx_pp_block_e pp_block);

/**
 * \brief  Return ID of pipeline block
 * \param [in] unit             - Identifier of HW platform.
 * \param [in] pp_block_name    - Block Name.
 * \param [out] pp_block_id_p   - pointer to block ID
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 */
shr_error_e dnx_pp_block_string_to_id_no_error(
    int unit,
    char *pp_block_name,
    dnx_pp_block_e * pp_block_id_p);

/**
 * \brief  Return ID of pipeline block
 * \param [in] unit             - Identifier of HW platform.
 * \param [in] pp_block_name    - Block Name.
 * \param [out] pp_block_id_p   - pointer to block ID
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 */
shr_error_e dnx_pp_block_string_to_id(
    int unit,
    char *pp_block_name,
    dnx_pp_block_e * pp_block_id_p);

/**
 * \brief  Return name of pipeline stage
 * \param [in] unit  - Identifier of HW platform.
 * \param [in] pp_block - block ID
 * \return
 *   \retval block direction - On success
 *   \retval NULL       - On Failure
 * \remark
 */
char *dnx_pp_block_direction(
    int unit,
    dnx_pp_block_e pp_block);

/**
 * \brief  Return name of block name per stage
 * \param [in] unit  - Identifier of HW platform.
 * \param [in] stage - stage ID
 * \return
 *   \retval block name - On success
 *   \retval NULL       - On Failure
 * \remark
 */
char *dnx_pp_stage_block_name(
    int unit,
    dnx_pp_stage_e stage);
/*
 * }
 * End of PP Stages API
 */

/**
 * \brief  Check if the name of the stage is KBR encoded and return the true stage
 * \param [in] unit  - Identifier of HW platform.
 * \param [in] stage_n_in - input stage name
 * \param [out] stage_n_out - output stage name
 * \return
 *      _SHR_E_NONE  - stage found as KBR encoded and true stage name returned
 *      _SHR_E_NOT_FOUND - stage is not KBR encoded
 *      other standard shr_error_e
 * \remark
 */
shr_error_e dnx_pp_stage_kbr_resolve(
    int unit,
    char *stage_n_in,
    char *stage_n_out);

#endif /* INCLUDE_SOC_DNX_PP_STAGE_H_ */
