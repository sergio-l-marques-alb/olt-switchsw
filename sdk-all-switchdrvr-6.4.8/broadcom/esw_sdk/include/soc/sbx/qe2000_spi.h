
/*
 * $Id: qe2000_spi.h,v 1.5 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * == qe2000_spi.h - QE SPI Initialization      ==
 */

#ifndef _QE2000_SPI_INIT_H
#define _QE2000_SPI_INIT_H

/* #include "sbTypes.h" */
#include "glue.h"
#include "qe2000_init.h"

void      hwQe2000Spi4TxForceTrainingOn(sbhandle userDeviceHandle, uint32 spi_ul);
void      hwQe2000Spi4TxForceTrainingOff(sbhandle userDeviceHandle, uint32 spi_ul);
uint32  hwQe2000Spi4RxStatus(sbhandle userDeviceHandle, uint32 spi_ul);
uint32  hwQe2000Spi4TxStatus(sbhandle userDeviceHandle, uint32 spi_ul);
void      hwQe2000Spi4RxEnable(sbhandle userDeviceHandle, uint32 enable_ul, uint32 spi_ul);
void      hwQe2000Spi4TxEnable(sbhandle userDeviceHandle, uint32 enable_ul, uint32 spi_ul);
void      hwQe2000Spi4RxEnableGet(sbhandle userDeviceHandle, uint32 *enable_ul, uint32 spi_ul);
void      hwQe2000Spi4TxEnableGet(sbhandle userDeviceHandle, uint32 *enable_ul, uint32 spi_ul);
void      hwQe2000Spi4RxRequestTraining(sbhandle userDeviceHandle, uint32 enable_ul, uint32 spi_ul);

/*========
**= QE   =
**========
*/

/* bitfield access */
#define HW_QE2000_BIT_N(x)                (0x1 << (x))
#define HW_QE2000_MSK_NBITS(n)            (0xFFFFFFFF >> (32 - (n)))
#define HW_QE2000_ZIN_MASK64(i, mask64)   (!!(COMPILER_64_BITTEST(mask64,i)))

#define HW_QE2000_MAX_SPI4_PORTS_K   (49)

/* pkt sizes */
#define HW_QE2000_SHIMHDR_SZ_K            (12)
#define HW_QE2000_MIN_FRM_SZ_K            (64)
#define HW_QE2000_MAX_FRM_SZ_NRML_K     (1518)
#define HW_QE2000_MAX_FRM_SZ_JMBO_K     (9216)
#define HW_QE2000_CRC_SZ_K                 (4)

#endif /* _INIT_SPI_QE2000_H */
