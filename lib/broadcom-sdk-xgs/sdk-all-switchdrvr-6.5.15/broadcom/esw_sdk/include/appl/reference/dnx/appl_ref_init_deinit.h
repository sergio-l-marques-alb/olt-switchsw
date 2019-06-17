/** \file appl_ref_init_deinit.h
 *
 * init and deinit functions to be used by the INIT_DNX command.
 *
 * need to make minimal amount of steps before we are able to use call bcm_init
 *
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef FILE_APPL_REF_DNX_INIT_DEINIT_H_INCLUDED
/* { */
#define FILE_APPL_REF_DNX_INIT_DEINIT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <appl/diag/diag.h>
#include <appl/reference/dnxc/appl_ref_init_utils.h>
#include <shared/utilex/utilex_seq.h>

/*
* MACROs:
* {
*/

/*
 * }
 */

/*
 * Structs and Enums:
 * {
 */

/*
 * }
 */

/*
 * Globals:
 * {
 */
extern char appl_dnx_init_usage[];
/*
 * }
 */
/*
* Function Declarations:
* {
*/


/**
 * \brief - Get the list of DNX application steps
 * 
 * \param [in] unit - Unit ID
 * \param [Out] appl_steps - pointer to the list of application steps
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnx_step_list_get(
    int unit, 
    const appl_dnxc_init_step_t **appl_steps);

/**
 * \brief Describes the IDs for the different steps and 
 *        sub-steps of the DNX APPL.
 */
typedef enum
{
    /**
     * Invalid Value, used to indicate to APIs that this input is
     * not relevant
     */
    APPL_DNX_STEP_INVALID_STEP = UTILEX_SEQ_STEP_INVALID,
    /**
     * SDK init (attach and bcm_init())
     */
    APPL_DNX_STEP_SDK,
    /**
     * Reference applications
     */
    APPL_DNX_STEP_APPS,
    /**
     * parse system params application
     */
    APPL_DNX_STEP_SYS_PARAMS,
    /**
     * General device confiugurations application
     */
    APPL_DNX_STEP_SYS_DEVICE,
    /**
     * System ports application
     */
    APPL_DNX_STEP_SYS_PORTS,
    /**
     * Sniff recycle ports application
     */
    APPL_DNX_STEP_SNIFF_PORTS,
    /**
     * End to end scheduling scheme application
     */
    APPL_DNX_STEP_E2E_SCHEME,
    /**
     * OAM application
     */
    APPL_DNX_STEP_OAM,
    /**
     * PP ETH Processing application
     */
    APPL_DNX_STEP_PP_ETH_PROCESSING,
    /**
     * L2 application
     */
    APPL_DNX_STEP_L2,
    /**
     * VLAN application
     */
    APPL_DNX_STEP_VLAN,
    /**
     * VLAN application
     */
    APPL_DNX_STEP_COMPRESSED_LAYER_TYPE,
    /**
     * Traffic enable application
     */
    APPL_DNX_STEP_TRAFFIC_EN,
    /**
     * Reference FIELD WA applications
     */
    APPL_DNX_STEP_FP_WA_APPS,
    /**
     * Fec Dest enable application
     */
    APPL_DNX_STEP_FP_WA_FEC_DEST,
    /**
     * Oam Index enable application
     */
    APPL_DNX_STEP_FP_WA_OAM_INDEX,
    /**
     * Trap L4 enable application
     */
    APPL_DNX_STEP_FP_WA_TRAP_L4,
    /**
     * Flow ID WA enable application
     */
    APPL_DNX_STEP_FP_WA_FLOW_ID,
    /**
     * ROO WA enable application
     */
    APPL_DNX_STEP_FP_WA_ROO,
    /**
     * J1 Same port drop WA enable application
     */
    APPL_DNX_STEP_FP_WA_J1_SAME_PORT,
    /**
     * J1 learning WA enable application
     */
    APPL_DNX_STEP_FP_WA_J1_LEARNING,
    /**
     * Oam stat enable application
     */
    APPL_DNX_STEP_FP_WA_OAM_STAT,
#if defined(INCLUDE_KBP)
    /**
     * KBP application
     */
    APPL_DNX_STEP_KBP,
#endif
    /**
     * Linkscan application
     */
    APPL_DNX_STEP_LINKSCAN,
    /**
     * ITMH application
     */
    APPL_DNX_STEP_ITMH,
    /**
     * ITMH-PPH application
     */
    APPL_DNX_STEP_ITMH_PPH,
    /**
     * J1 ITMH application
     */
    APPL_DNX_STEP_J1_ITMH,
    /**
     * J1 ITMH-PPH application
     */
    APPL_DNX_STEP_J1_ITMH_PPH,
    /**
     * Stacking application
     */
    APPL_DNX_STEP_STACKING,
    /**
     * SRV6 application
     */
    APPL_DNX_STEP_SRV6,
    /*
     * Interrupt application
     */
    APPL_DNX_STEP_INTERRUPT,
    /**
     * QOS application
     */
    APPL_DNX_STEP_QOS,
    /**
     * DRAM application
     */
    APPL_DNX_STEP_DRAM,
    /**
     * VISIBILITY application
     */
    APPL_DNX_STEP_VISIBILITY,
    /**
     * COMPENSATION application
     */
    APPL_DNX_STEP_COMPENSATION,
    /**
     * RX Trap application
     */
    APPL_DNX_STEP_RX_TRAP,    
    /**
     * init done step, used to update modules that the init was done
     */
    APPL_DNX_STEP_INIT_DONE,
    /**
     * Must be last
     */
    APPL_DNX_STEP_LAST_STEP = UTILEX_SEQ_STEP_LAST
} appl_dnx_step_id_e;


/*
 * }
 */

/* } */
#endif
