/*
 * $Id: $
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
 * File: jer_egr_queuing.h
 */

#ifndef __JER_EGR_QUEUING_INCLUDED__

#define __JER_EGR_QUEUING_INCLUDED__

/* EGQ put cpu/olp/oam/rcy in 28/29/30/31 */
#define SOC_JER_EGR_IF_CPU      28
#define SOC_JER_EGR_IF_OLP      29  
#define SOC_JER_EGR_IF_OAMP     30
#define SOC_JER_EGR_IF_RCY      31

int soc_jer_egr_interface_init(int unit);
int soc_jer_egr_interface_alloc(int unit, soc_port_t port);
int soc_jer_egr_interface_free(int unit, soc_port_t port);

int
  soc_jer_egr_port2egress_offset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT uint32              *egr_if
  );

int
    soc_jer_egr_is_channelized(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  int                   core, 
    SOC_SAND_OUT uint32                *is_channalzied
   );

int soc_jer_egr_tm_init(int unit);

typedef enum soc_jer_egr_cal_type_e {
    socJerCalTypeFQP = 0,
    socJerCalTypePQP,
    socJerCalTypesCount
} soc_jer_egr_cal_type_t;

int
  soc_jer_egr_q_nif_cal_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_to_update,
    SOC_SAND_IN soc_jer_egr_cal_type_t cal_to_update
  );

int
  soc_jer_egr_q_nif_cal_set_all(
    SOC_SAND_IN int unit
  );

#endif /*__JER_EGR_QUEUING_INCLUDED__*/

