
/* $Id: stat.h,v 1.1 Broadcom SDK $
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
 * File:        stat.h
 * Purpose:     stat internal definitions specific to Caladan3 BCM library
 */

#ifndef _BCM_INT_SBX_CALADAN3_STAT_H_
#define _BCM_INT_SBX_CALADAN3_STAT_H_

#ifdef BCM_CALADAN3_G3P1_SUPPORT

typedef enum caladan3_g3p1_counter_id_s {
    CALADAN3_G3P1_COUNTER_INGRESS,
    CALADAN3_G3P1_COUNTER_EGRESS,
    CALADAN3_G3P1_COUNTER_MAX
} caladan3_g3p1_counter_id_t;


#define BCM_CALADAN3_STAT_WITH_ID 1

/*
 * Function:
 *      _bcm_caladan3_stat_block_alloc
 * Description:
 *      Allocate counters from a statistics block
 * Parameters:
 *      unit      - device unit number.
 *      type      - one of the defined segment types
 *      count     - number of counters required
 *      start     - (OUT) where to put first of allocated counter block
 *      flags     - BCM_CALADAN3_STAT_WITH_ID indicates start value passed in.
 * Returns:
 *      BCM_E_NONE      - Success
 *      BCM_E_XXXX      - Failure
 */
int _bcm_caladan3_stat_block_alloc(int unit,
                                   int type,
                                   shr_aidxres_element_t *start,
                                   shr_aidxres_element_t count,
                                   uint32 flags);

/*
 * Function:
 *      _bcm_caladan3_stat_block_free
 * Description:
 *      Free counters from a statistics block
 * Parameters:
 *      unit      - device unit number.
 *      type      - one of the defined segment types
 *      start     - (OUT) where to put first of allocated counter block
 * Returns:
 *      BCM_E_NONE      - Success
 *      BCM_E_XXXX      - Failure
 */
int _bcm_caladan3_stat_block_free(int unit,
                                int type,
                                shr_aidxres_element_t start);


#endif /* BCM_CALADAN3_G3P1_SUPPORT */

#endif /* _BCM_INT_SBX_CALADAN3_STAT_H_ */

