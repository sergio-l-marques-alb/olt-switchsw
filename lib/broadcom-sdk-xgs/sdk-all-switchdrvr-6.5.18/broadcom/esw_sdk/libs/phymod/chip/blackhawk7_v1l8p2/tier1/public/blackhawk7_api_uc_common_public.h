/*************************************************************************************
 *                                                                                   *
 * Copyright: (c) 2018 Broadcom.                                                     *
 * Broadcom Confidential. All rights reserved.                                       *
 *                                                                                   *
 *************************************************************************************/

/*************************************************************************************
 *************************************************************************************
 *                                                                                   *
 *  Revision      :   *
 *                                                                                   *
 *  Description   :  Defines and Enumerations required by Blackhawk ucode            *
 *                                                                                   *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$                                                             *
 *  No portions of this material may be reproduced in any form without               *
 *  the written permission of:                                                       *
 *      Broadcom Corporation                                                         *
 *      5300 California Avenue                                                       *
 *      Irvine, CA  92617                                                            *
 *                                                                                   *
 *  All information contained in this document is Broadcom Corporation               *
 *  company private proprietary, and trade secret.                                   *
 *                                                                                   *
 *************************************************************************************
 *************************************************************************************/

/** @file blackhawk7_api_uc_common.h
 * Defines and Enumerations shared by Blackhawk IP Specific API and Microcode
 */

#ifndef BLACKHAWK_API_UC_COMMON_H
#define BLACKHAWK_API_UC_COMMON_H

/* Add Blackhawk specific items below this */

/* Please note that when adding entries here you should update the #defines in the blackhawk7_v1l8p2_common.h */

/** OSR_MODES Enum */
enum blackhawk7_v1l8p2_osr_mode_enum {
  /* If the enumerations change, then consider updating OSR_MODE_SUPPORTS_EYE_TESTS(). */
  BLACKHAWK7_V1L8P2_OSX1      = 0,
  BLACKHAWK7_V1L8P2_OSX2      = 1,
  BLACKHAWK7_V1L8P2_OSX4      = 2,
  BLACKHAWK7_V1L8P2_OSX2P5    = 3,
  BLACKHAWK7_V1L8P2_OSX21P25  = 4,
  BLACKHAWK7_V1L8P2_OSX8      = 5,
  BLACKHAWK7_V1L8P2_OSX16P5   = 8,
  BLACKHAWK7_V1L8P2_OSX16     = 9,
  BLACKHAWK7_V1L8P2_OSX20P625 = 12,
  BLACKHAWK7_V1L8P2_OSX32     = 13
};

#endif