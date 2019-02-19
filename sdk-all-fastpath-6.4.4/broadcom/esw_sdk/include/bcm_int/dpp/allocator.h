/* 
 * $Id: allocator.h,v 1.2 Broadcom SDK $
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
 * File:        allocator.h
 */

#ifndef _BCM_INT_DPP_ALLOCATOR_H_
#define _BCM_INT_DPP_ALLOCATOR_H_

#include <bcm/types.h>


/*
 * Resource types visible to the user
 */
typedef enum {
    DPP_USR_RES_FEC_L3,
    DPP_USR_RES_FEC_L3_MPATH,
    DPP_USR_RES_FEC_IPMC,
    DPP_USR_RES_FEC_GLOBAL_GPORT,
    DPP_USR_RES_FEC_LOCAL_GPORT,
    DPP_USR_RES_FEC_L2MC,
    DPP_USR_RES_FEC_MPLS,
    DPP_USR_RES_FEC_UNICAST,
    DPP_USR_RES_LIF,
    DPP_USR_RES_EEP,
    DPP_USR_RES_IFID,
    DPP_USR_RES_VRF,
    DPP_USR_RES_VSI,
    DPP_USR_RES_FLOW_SE,
    DPP_USR_RES_FLOW_CONNECTORS,
    DPP_USR_RES_QUEUES,
    DPP_USR_RES_MAX
} _dpp_usr_res_types_t;

typedef enum {
    DPP_HW_RES_FEC = 0,
    DPP_HW_RES_GLOBAL_GPORT_FEC,
    DPP_HW_RES_LOCAL_GPORT_FEC,
    DPP_HW_RES_LIF,
    DPP_HW_RES_EEP,
    DPP_HW_RES_IFID,
    DPP_HW_RES_VRF,
    DPP_HW_RES_VSI,
    DPP_HW_RES_PROTECTION,
    DPP_HW_RES_FLOWS,
    DPP_HW_RES_QUEUES,
    DPP_HW_RES_MAX
} _dpp_hw_res_t;

typedef enum {
    DPP_TABLE_NONE=0,
    DPP_TABLE_FEC,
    DPP_TABLE_VSI,
    DPP_TABLE_LIF,
    DPP_TABLE_EEP,
    DPP_TABLE_VRF,
    DPP_TABLE_PROT,
    DPP_TABLE_IFID,
    DPP_TABLE_FLOW,
    DPP_TABLE_QUEUE,
    DPP_TABLE_MAX
} _dpp_hw_table_t;


/*
 *   Function
 *      _dpp_resource_init
 *   Purpose
 *      Initialize the shr resource manager for all HW resources
 *      for the unit
 *   Parameters
 *      (IN) unit   : unit number of the device
 *   Returns
 *       BCM_E_NONE all resources are successfully initialized
 *       BCM_E_* as appropriate otherwise
 *   Notes
 *       Returns error is any of the resources cannot be initialized
 */
extern int
_dpp_resource_init(int unit);

/*
 *   Function
 *      _dpp_alloc_wellknown_resources
 *   Purpose
 *      Allocate well-known resources per unit
 *      and save the same in the soc structure
 *      for sharing between modules.
 *
 *   Parameters
 *      (IN) unit   : unit number of the device
 *   Returns
 *       BCM_E_NONE - All required resources are allocated
 *       BCM_E_*    - failure
 *   Notes
 */
extern int
_dpp_alloc_wellknown_resources(int unit);

#endif /* _BCM_INT_DPP_ALLOCATOR_H_ */
