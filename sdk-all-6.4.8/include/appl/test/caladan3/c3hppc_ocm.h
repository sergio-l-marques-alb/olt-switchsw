/*
 * $Id: c3hppc_ocm.h,v 1.6 Broadcom SDK $
 *
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
 * lrp.h : LRP defines
 *
 *-----------------------------------------------------------------------------*/
#ifndef _C3HPPC_OCM_H_
#define _C3HPPC_OCM_H_

#include <sal/appl/config.h>
#include <sal/appl/io.h>
#include <sal/types.h>
#include <soc/sbx/caladan3/ocm.h>
#include <appl/test/caladan3/c3hppc_utils.h>
#include <soc/types.h>
#include <soc/drv.h>

#define C3HPPC_NO_SEGMENT_TABLE           1

#define C3HPPC_LOGICAL_TO_PHYSICAL_SHIFT      14
#define C3HPPC_OCM_MAX_PHY_BLK                (128)
#define C3HPPC_OCM_PHY_BLK_PER_OCM            (64)
#define C3HPPC_OCM_MAX_PHY_BLK_SIZE_IN_ROWS   (0x4000)
#define C3HPPC_OCM_NUM_OF_TRANSFER_SIZES      (7)
#define C3HPPC_OCM_ALL_TRANSFER_SIZES         (C3HPPC_OCM_NUM_OF_TRANSFER_SIZES + 1)
#define C3HPPC_DATUM_SIZE_BIT                 0
#define C3HPPC_DATUM_SIZE_DBIT                1
#define C3HPPC_DATUM_SIZE_NIBBLE              2
#define C3HPPC_DATUM_SIZE_BYTE                3
#define C3HPPC_DATUM_SIZE_WORD                4
#define C3HPPC_DATUM_SIZE_LONGWORD            5
#define C3HPPC_DATUM_SIZE_QUADWORD            6

#define C3HPPC_ERROR_PROTECTION__NONE         0
#define C3HPPC_ERROR_PROTECTION__PARITY       1
#define C3HPPC_ERROR_PROTECTION__ECC          2

#define C3HPPC_NUM_OF_OCM_PORTS               (15)
#define C3HPPC_NUM_OF_OCM_LRP_PORTS           (10)
#define C3HPPC_NUM_OF_OCM_CMU_PORTS           (2)
#define C3HPPC_NUM_OF_OCM_COP_PORTS           (2)
#define C3HPPC_NUM_OF_OCM_BUBBLE_PORTS        (1)
#define C3HPPC_PHYSICAL_PROC_ACCESS           (C3HPPC_NUM_OF_OCM_PORTS + 1)
#define C3HPPC_RAW_PROC_ACCESS                (C3HPPC_PHYSICAL_PROC_ACCESS + 1)

typedef struct c3hppc_ocm_port_info_s {
  int      nStartingSegment;
  uint32   uSegmentTransferSize;
  uint32   uSegmentBase;
  uint32   uSegmentLimit;
  int      nStartingPhysicalBlock;
  char     bSegmentProtected;
  char     bValid;
} c3hppc_ocm_port_info_t;

typedef struct c3hppc_ocm_control_info_s {
  c3hppc_ocm_port_info_t    *pOcmPortInfo;
} c3hppc_ocm_control_info_t;


typedef struct c3hppc_64b_ocm_entry_template_s {
  uint32    uData[3];
} c3hppc_64b_ocm_entry_template_t;



int c3hppc_ocm_port_program_segment_table(int nUnit, int nOcmPort, int nSegment, 
                                          uint32 uSegmentTransferSize, uint32 uSegmentBase,
                                          uint32 uSegmentLimit, char bSegmentProtected);
int c3hppc_ocm_port_program_port_table(int nUnit, int nOcmPort, uint32 uSegmentBase, 
                                       uint32 uSegmentLimit, uint32 uSegmentTransferSize,
                                       int nStartingPhysicalBlock);
int c3hppc_ocm_port_modify_segment_error_protection(int nUnit, int nOcmPort, int nSegment, 
                                                    uint32 uProtectionScheme);




int c3hppc_ocm_hw_init( int nUnit, c3hppc_ocm_control_info_t *pC3OcmControlInfo );
int c3hppc_ocm_hw_cleanup( int nUnit );
int c3hppc_ocm_display_error_state( int nUnit );
int c3hppc_ocm_port_config( int nUnit, c3hppc_ocm_port_info_t *pOcmPortInfo);
int c3hppc_ocm_mem_read_write( int nUnit, int nOcmPort, int nSegment, uint32 uOffset,
                               uint8 bWrite, uint32 *puEntryData );
int c3hppc_ocm_dma_read_write( int nUnit, int nOcmPort, int nSegment, uint32 uStartOffset,
                               uint32 uEndOffset, uint8 bWrite, uint32 *puDmaData );
int c3hppc_ocm_map_lrp2ocm_port(int nLrpPort);
int c3hppc_ocm_map_cop2ocm_port(int nCopPort);
int c3hppc_ocm_map_cmu2ocm_port(int nCmuPort);
int c3hppc_ocm_map_bubble2ocm_port(int nBubblePort);
#endif /* _C3HPPC_OCM_H_ */
