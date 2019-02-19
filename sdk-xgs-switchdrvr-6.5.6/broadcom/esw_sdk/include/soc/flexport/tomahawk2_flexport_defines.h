/*
* $Id: $
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
* $File:  tomahawk2_flexport_defines.h
*/


#ifndef TOMAHAWK2_FLEXPORT_DEFINES_H
#define TOMAHAWK2_FLEXPORT_DEFINES_H


#define _TH2_PHY_PORT_CPU               0
#define _TH2_PHY_PORT_MNG0              257
#define _TH2_PHY_PORT_MNG1              259
#define _TH2_LOG_PORT_MNG0              66
#define _TH2_LOG_PORT_MNG1              100
#define _TH2_PHY_PORT_LPBK0             260
#define _TH2_PHY_PORT_LPBK1             261
#define _TH2_PHY_PORT_LPBK2             262
#define _TH2_PHY_PORT_LPBK3             263
#define _TH2_FORCE_SAF_TIMER_ENTRY_CNT  5
#define _TH2_OBM_PRIORITY_LOSSY_LO      0
#define _TH2_OBM_PRIORITY_LOSSY_HI      1
#define _TH2_OBM_PRIORITY_LOSSLESS0     2
#define _TH2_OBM_PRIORITY_LOSSLESS1     3
#define COUNTOF(ary)        ((int) (sizeof (ary) / sizeof ((ary)[0])))
#define _TH2_PHY_IS_FRONT_PANEL_PORT(p)        ((p>=1)&& (p<=256))

/* General Physical port */
#define _TH2_PORTS_PER_PBLK             4
#define _TH2_PBLKS_PER_PIPE             16
#define _TH2_PBLKS_PER_HPIPE            8
#define _TH2_PIPES_PER_DEV              4
#define _TH2_XPES_PER_DEV               4
#define _TH2_GPHY_PORTS_PER_PIPE        \
    (_TH2_PORTS_PER_PBLK * _TH2_PBLKS_PER_PIPE)
#define _TH2_PHY_PORTS_PER_PIPE         (_TH2_GPHY_PORTS_PER_PIPE + 2)
#define _TH2_PBLKS_PER_DEV              \
    (_TH2_PBLKS_PER_PIPE * _TH2_PIPES_PER_DEV)
#define _TH2_PHY_PORTS_PER_DEV          \
    (_TH2_PHY_PORTS_PER_PIPE * _TH2_PIPES_PER_DEV)


/* Device port
 * 32 General device port + 1 CPU/Mng + 1 Loopback*/
#define _TH2_GDEV_PORTS_PER_PIPE        32
#define _TH2_DEV_PORTS_PER_PIPE         (_TH2_GDEV_PORTS_PER_PIPE + 2)
#define _TH2_DEV_PORTS_PER_DEV          \
    (_TH2_DEV_PORTS_PER_PIPE * _TH2_PIPES_PER_DEV)

/* MMU port */
#define _TH2_MMU_PORTS_OFFSET_PER_PIPE  64
#define _TH2_MMU_PORTS_PER_DEV          (64 * 4)

/* TDM */
#define _TH2_TDM_LENGTH                 512
#define _TH2_OVS_GROUP_COUNT_PER_HPIPE  6
#define _TH2_OVS_GROUP_COUNT_PER_PIPE   12
#define _TH2_OVS_GROUP_TDM_LENGTH       12
#define _TH2_OVS_HPIPE_COUNT_PER_PIPE   2

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#endif /* TOMAHAWK2_FLEXPORT_DEFINES_H */
