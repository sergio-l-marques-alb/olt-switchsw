/*****************************************************************************************
 *****************************************************************************************
 *                                                                                       *
 *  Revision      :   *
 *                                                                                       *
 *  Description   :  Functions used internally and available in debug shell only         *
 *                                                                                       *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$                                                                 *
 *  No portions of this material may be reproduced in any form without                   *
 *  the written permission of:                                                           *
 *      Broadcom Corporation                                                             *
 *      5300 California Avenue                                                           *
 *      Irvine, CA  92617                                                                *
 *                                                                                       *
 *  All information contained in this document is Broadcom Corporation                   *
 *  company private proprietary, and trade secret.                                       *
 *                                                                                       *
 *****************************************************************************************
 *****************************************************************************************/

/** @file srds_api_types.h
 * Common types used by Serdes API functions
 * This file contains types which are common to all serdes cores
 */

#ifndef SRDS_API_TYPES_H
#define SRDS_API_TYPES_H

typedef struct srds_access_s {
    void* user_acc; /**< Optional application data - not used by PHY driver */
    int unit; /* Switch unit */
    uint8_t core; /* Serdes core number, 0 - 3, each core has 4 lanes */
    uint8_t lane_mask; /* specific lanes bitmap */
    uint16_t devid; /* device id, should be 1 for this device */
} srds_access_t;

#endif
