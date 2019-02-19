/*
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * BCM5301X spi register definition
 */


#ifndef _nspspi_core_h_
#define _nspspi_core_h_

typedef volatile struct _nspspiregs {
    uint32 	cca_gsioctl;
    uint32 	cca_gsioaddress;
    uint32 	cca_gsiodata;
} nspspiregs_t;

#define CCA_GSIOCTL_STARTBUSY_MASK	0x80000000
#define CCA_GSIOCTL_GSIOOP_MASK         0x000000FF
#define CCA_GSIOCTL_GSIOOP_READ         0x60
#define CCA_GSIOCTL_GSIOOP_WRITE        0x61
#define CCA_GSIOCTL_GSIOCODE_MASK       0x00000700
#define CCA_GSIOCTL_GSIOCODE_SHIFT      8
#define CCA_GSIOCTL_NUMADDR_MASK        0x00003000
#define CCA_GSIOCTL_NUMADDR_SHIFT       12
#define CCA_GSIOCTL_NUMADDR_DEF         0
#define CCA_GSIOCTL_NUMDATA_MASK        0x00030000
#define CCA_GSIOCTL_NUMDATA_SHIFT       16

#define CCA_GSIOCTL_GSIOCODE_0          0x0
#define CCA_GSIOCTL_GSIOCODE_1          0x1
#define CCA_GSIOCTL_GSIOCODE_2          0x2

#define BCM53134_SPI_DATA_REG           0xF0
#define BCM53134_SPI_STS_REG            0xFE
#define BCM53134_SPI_PAGE_REG           0xFF

#define BCM53134_SPI_SPIF_MASK          0x80
#define BCM53134_SPI_RACK_MASK          0x20

#define UINT32_MASK                     0xFFFFFFFF

#endif /* _nspspi_core_h_ */
