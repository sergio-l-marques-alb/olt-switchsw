/** \file dnx_hbmc.h
 *
 * This module contains HBMC definitions that should be called from both 
 * soc and bcm layers.
 */
/*
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNX_HBMC_H
/* { */
#define _DNX_HBMC_H

/*
 * Include files
 * {
 */

#include <sal/types.h>

/*
 * }
 */

#define SOC_JR2_HBM_DEVICE_ID_WORDS 3
typedef struct
{
    uint32 device_id_raw[SOC_JR2_HBM_DEVICE_ID_WORDS];  /* Device ID raw data returned from the HBM. See JEDEC for the
                                                         * interpretation of the data. */
} soc_dnx_hbm_device_id_t;

/* } */
#endif /* _DNX_HBMC_H */
