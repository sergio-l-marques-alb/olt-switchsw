/*
 * $Id$
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    PTP_feature.h
 *
 * Holds SDK features version info, to be shared with PTP FW
 */

#ifndef __PTP_FEATURE_H__
#define __PTP_FEATURE_H__

/*
#define PTP_SDK_VERSION         0x01000000
#define PTP_UC_MIN_VERSION      0x01000200
*/

#define ENABLE       1
#define DISABLE      0
#define PTP_SDK_BASE_VERSION         0x01000000
#define PTP_SDK_VERSION   PTP_SDK_BASE_VERSION         | \
                            (ENABLE  << PTP_VP_TOD_IN_SERIAL)        | \
                            (ENABLE  << PTP_CMICM_CHANNEL_CONF_L1BKUP)

#define PTP_UC_MIN_VERSION     0x01000200

/*PTP FW version*/
extern uint32 ptp_firmware_version;

#define PTP_UC_FEATURE_CHECK(feature)  (ptp_firmware_version & (1 << feature))

/* Feature support bit should be shared between SDK and UKERNEL version.
 * 24th bit are reserved in SDK as they are set in the legacy version
 * PTP_VERSION : 0x01000200 MIN_SDK_VERSION : 0x01000000*/
#define PTP_FEATURE_RESERVED1  9
#define PTP_FEATURE_RESERVED2  24

/* Features added in uKernel PTP FW */
#define PTP_VP_TOD_IN_SERIAL  1 /*Virtual Port TODIn Source Etherenet/Serial Support*/
#define PTP_CMICM_CHANNEL_CONF_L1BKUP 2 /* CMICM L1 backup conf support in ptp channel config */



#endif /*__PTP_FEATURE_H__*/

