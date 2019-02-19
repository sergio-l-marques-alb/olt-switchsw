/*
 * $Id: fec_allocation.h,v 1.312 Broadcom SDK $
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
 * File:        fec_allocation.h
 * Purpose:     fec allocation managament
 *
 */


#ifndef __USERINTERFACE_FEC_ALLOCATION_H__
#define __USERINTERFACE_FEC_ALLOCATION_H__


#include <soc/dpp/SAND/Utils/sand_sorted_list.h>
#include <bcm/l3.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/l3.h>
#include <shared/bsl.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm/debug.h>
#include <bcm/failover.h>
#include <sal/appl/io.h> 
#include <sal/core/libc.h> 
#include <sal/core/spl.h> 
#include <sal/core/alloc.h> 
#include <sal/core/dpc.h> 


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_L3
#define DOES_NOT_BELONG_TO_ECMP -1
#define DATABASE_FILE_PATH_ALLOC_MANAGER "database_file_alloc_manager.txt"
#define DATABASE_FILE_PATH_FECS_LIST "database_file_fecs_list.txt"

int fec_allocation_bcm_l3_egress_ecmp_destroy(int unit,bcm_l3_egress_ecmp_t *ecmp);
int fec_allocation_bcm_l3_egress_destroy(int unit, bcm_if_t intf);
int fec_allocation_bcm_l3_egress_ecmp_delete(int unit, bcm_l3_egress_ecmp_t *ecmp, bcm_if_t intf);
int fec_allocation_bcm_l3_egress_ecmp_add(int unit, bcm_l3_egress_ecmp_t *ecmp, bcm_if_t intf);
int fec_allocation_bcm_l3_egress_ecmp_create(int unit, bcm_l3_egress_ecmp_t *ecmp, int intf_count, bcm_if_t *intf_array);
int fec_allocation_bcm_l3_egress_create(int unit, uint32 flags, bcm_l3_egress_t *egr, bcm_if_t *intf);
int fec_allocation_create_database_file(int unit);
int fec_allocation_initialize(int unit);
void fec_allocation_deinitialize_last(void);
void fec_allocation_deinitialize_unit(int unit);
int fec_allocation_associate_fec(int unit, bcm_if_t fec_id);
int soc_dpp_fec_allocation_main(int unit);
int fec_allocation_main(int unit, int write_logs);

#endif

