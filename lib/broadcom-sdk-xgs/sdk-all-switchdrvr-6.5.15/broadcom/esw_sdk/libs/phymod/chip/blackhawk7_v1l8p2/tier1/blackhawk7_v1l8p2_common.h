/********************************************************************************
 ********************************************************************************
 *                                                                              *
 *  Revision      :   *
 *                                                                              *
 *  Description   :  Defines and Enumerations required by Serdes APIs           *
 *                                                                              *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$                                                        *
 *  No portions of this material may be reproduced in any form without          *
 *  the written permission of:                                                  *
 *      Broadcom Corporation                                                    *
 *      5300 California Avenue                                                  *
 *      Irvine, CA  92617                                                       *
 *                                                                              *
 *  All information contained in this document is Broadcom Corporation          *
 *  company private proprietary, and trade secret.                              *
 *                                                                              *
 ********************************************************************************
 ********************************************************************************/

/** @file blackhawk7_v1l8p2_common.h
 * Defines and Enumerations shared across Merlin16/Falcon16 APIs BUT NOT MICROCODE
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef BLACKHAWK7_V1L8P2_API_COMMON_H
#define BLACKHAWK7_V1L8P2_API_COMMON_H


/** Macro to determine sign of a value */
#define sign(x) ((x>=0) ? 1 : -1)

#define UCODE_MAX_SIZE  (120*1024)    /* 120K CODE RAM */

/*
 * IP-Specific Iteration Bounds
 */
#   define DUAL_PLL_NUM_PLLS  2

#endif
#ifdef __cplusplus
}
#endif
