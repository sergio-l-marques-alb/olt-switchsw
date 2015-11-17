/* 
 * $Id: cosq.h Exp $
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
 * File:        cosq.h
 * Purpose:     COSQ internal definitions to the BCM library.
 */

#ifndef _BCM_INT_SBX_CALADAN3_COSQ_H_
#define _BCM_INT_SBX_CALADAN3_COSQ_H_

#include <bcm/types.h>
#include <bcm/cosq.h>

typedef struct {
    pbmp_t sq_bmp; /* 128 */
    pbmp_t dq_bmp; /* 128 */
} bcm_c3_cosq_port_queues_t;

int bcm_caladan3_cosq_init(int unit);

int bcm_caladan3_cosq_gport_add(int unit, bcm_gport_t physical_port,
                                int num_cos_levels, uint32 flags, bcm_gport_t *req_gport);

int bcm_caladan3_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *physical_port,
                                int *num_cos_levels, uint32 *flags);

int bcm_caladan3_cosq_gport_delete(int unit, bcm_gport_t gport);

int bcm_caladan3_cosq_gport_queue_attach(int unit, uint32 flags, bcm_gport_t ingress_queue, 
                                         bcm_cos_t ingress_int_pri, bcm_gport_t egress_queue, 
                                         bcm_cos_t egress_int_pri, int *attach_id);

int bcm_caladan3_cosq_gport_queue_attach_get(int unit, bcm_gport_t ingress_queue, bcm_cos_t ingress_int_pri, 
                                             bcm_gport_t *egress_queue, bcm_cos_t *egress_int_pri, 
                                             int attach_id);

int bcm_caladan3_cosq_gport_queue_detach(int unit, bcm_gport_t ingress_queue, bcm_cos_t ingress_int_pri, 
                                         int attach_id);

int bcm_c3_cosq_queue_delete(int unit, int queue);

int bcm_c3_cosq_info_dump(int unit);

int bcm_c3_cosq_queues_from_port_get(int unit, bcm_port_t port, bcm_c3_cosq_port_queues_t *port_queues);

int bcm_c3_cosq_dest_port_from_sq_get(int unit, int sq_id, bcm_port_t *dest_port, int *dest_dq_id);

int bcm_c3_cosq_src_port_from_dq_get(int unit, int dq_id, bcm_port_t *src_port, int *src_dq_id);

int bcm_caladan3_cosq_src_queue_set(int unit, int queue, uint32 bytes_min, uint32 bytes_max);

int bcm_caladan3_cosq_src_queue_get(int unit, int queue, uint32* bytes_min, uint32* bytes_max);

#endif /* _BCM_INT_SBX_CALADAN3_COSQ_H_ */
