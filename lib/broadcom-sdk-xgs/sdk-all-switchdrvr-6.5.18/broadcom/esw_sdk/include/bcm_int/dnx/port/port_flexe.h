/** \file include/bcm_int/dnx/port/port_flexe.h
 * $Id$
 * 
 * Internal DNX Port APIs 
 * 
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _PORT_FLEXE_H_INCLUDED_
/** { */
#define _PORT_FLEXE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>

/**
 * FlexE mode
 */
typedef enum
{
    /*
     * FlexE is disable in the device
     */
    DNX_FLEXE_MODE_DISABLED = 0,
    /*
     * FlexE centralized mode
     */
    DNX_FLEXE_MODE_CENTRALIZED,
    /*
     * FlexE distributed mode
     */
    DNX_FLEXE_MODE_DISTRIBUTED,
} dnx_flexe_mode_e;

/**
 * FlexE NB TDM slots allocation mode for
 * 50G NRZ speed
 */
typedef enum
{
    /*
     * Always allocate single slot for 50G NRZ
     */
    DNX_FLEXE_ALLOC_MODE_SINGLE = 0,
    /*
     * Always allocate double slot for 50G NRZ
     */
    DNX_FLEXE_ALLOC_MODE_DOUBLE,
    /*
     * The number of slots can be modified
     * dynamically
     */
    DNX_FLEXE_ALLOC_MODE_DYNAMIC,
} dnx_flexe_nb_tdm_slot_alloc_mode_e;

typedef struct dnx_flexe_cal_info_s
{
    /*
     * Indicated all calendars in FEU has been updated
     */
    int feu_cal_is_updated;
    /*
     * Indicated all calendars in FLEXEWP has been updated
     */
    int flexewp_cal_is_updated;
} dnx_flexe_cal_info_t;

/**
 * \brief - Configure the FlexE configurations
 *
 */
shr_error_e dnx_port_flexe_config_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    dnx_flexe_cal_info_t * cal_info);

/**
 * \brief - Configure FlexE calendar dynamically
 *
 */
shr_error_e dnx_port_flexe_calendar_set(
    int unit,
    bcm_port_t port,
    uint32 flags);

/**
 * \brief - Init FlexE HW
 *
 */
shr_error_e dnx_flexe_init(
    int unit);

/** } */
#endif /*_PORT_FLEXE_H_INCLUDED_*/
