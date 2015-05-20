/*
 * $Id: c3hppc_cmu.h,v 1.1 Broadcom SDK $
 *
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * c3hppc_cmu.h : CMU defines
 *
 *-----------------------------------------------------------------------------*/
#ifndef _C3HPPC_CMU_H_
#define _C3HPPC_CMU_H_

#include <sal/appl/config.h>
#include <sal/appl/io.h>
#include <sal/types.h>
#include <appl/test/caladan3/c3hppc_utils.h>
#include <soc/types.h>
#include <soc/drv.h>


#define C3HPPC_CMU_SEGMENT_NUM                   (32) 
#define C3HPPC_CMU_OCM_PORT_NUM                  (2) 
#define C3HPPC_CMU_SEGMENT_TYPE__TURBO_64b       (0) 
#define C3HPPC_CMU_SEGMENT_TYPE__TURBO_32b       (1) 
#define C3HPPC_CMU_SEGMENT_TYPE__SIMPLE_64b      (2) 
#define C3HPPC_CMU_SEGMENT_TYPE__SIMPLE_32b      (3)

typedef struct c3hppc_cmu_segment_info_s {
  uint32   uSegment;
  uint32   uSegmentOcmBase;
  uint32   uSegmentLimit;
  uint32   uSegmentType;
  uint64   *pSegmentPciBase;
  uint32   uSegmentPort;
  int      nStartingPhysicalBlock;
  char     bValid;
} c3hppc_cmu_segment_info_t;

typedef struct c3hppc_cmu_control_info_s {
  c3hppc_cmu_segment_info_t    *pCmuSegmentInfo;
  uint32                       uLFSRseed;
} c3hppc_cmu_control_info_t;



int c3hppc_cmu_program_segment_table( int nUnit, int nSegment,
                                      c3hppc_cmu_segment_info_t *pCmuSegmentInfo );
int c3hppc_cmu_program_segment_enable( int nUnit, int nSegment );
int c3hppc_cmu_program_segment_ejection_enable( int nUnit, int nSegment );
int c3hppc_cmu_segment_flush( int nUnit, c3hppc_cmu_segment_info_t *pCmuSegmentInfo );




int c3hppc_cmu_hw_init( int nUnit, c3hppc_cmu_control_info_t *pC3CmuControlInfo );
int c3hppc_cmu_hw_cleanup( int nUnit );
int c3hppc_cmu_segments_config( int nUnit, c3hppc_cmu_segment_info_t *pCmuSegmentInfo );
int c3hppc_cmu_segments_enable( int nUnit, c3hppc_cmu_segment_info_t *pCmuSegmentInfo );
int c3hppc_cmu_segments_ejection_enable( int nUnit, c3hppc_cmu_segment_info_t *pCmuSegmentInfo );
int c3hppc_cmu_segments_flush( int nUnit, c3hppc_cmu_segment_info_t *pCmuSegmentInfo );
int c3hppc_cmu_display_error_state( int nUnit );


#endif /* _C3HPPC_CMU_H_ */
