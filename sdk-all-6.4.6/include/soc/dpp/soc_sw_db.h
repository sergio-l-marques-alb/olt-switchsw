/*
 * $Id: soc_sw_db.h,v 1.3 Broadcom SDK $
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
 */


#ifndef _SOC_DPP_SW_DB_H_
#define _SOC_DPP_SW_DB_H_


#include <soc/types.h>
#include <soc/scache.h>

/* scache operations */
typedef enum soc_dpp_scache_oper_e {
    socDppScacheRetrieve,
    socDppScacheCreate,
    socDppScacheRealloc,
    socDppScacheFreeCreate
} soc_dpp_scache_oper_t;


/* consistency flags */
#define SOC_DPP_SCACHE_EXISTS_ERROR                0x00000001
#define SOC_DPP_SCACHE_DOWNGRADE_INVALID           0x00000002
#define SOC_DPP_SCACHE_DEFAULT                     (SOC_DPP_SCACHE_DOWNGRADE_INVALID)
                                                    

/* module space:
 * 0-79 - modules.h
 * 80-119 - bcmm_int/dpp/wb_db.h
 * 120-179 - common/wb_engine.h
 * 180-255 -soc_sw_db.h*
 */
#define SOC_MODULE_WB_BASE                (180)
#define SOC_MODULE_SW_DB                  (SOC_MODULE_WB_BASE + 0)
#define SOC_MODULE_SW_DB_PP               (SOC_MODULE_WB_BASE + 1)
#define ARAD_WB_DB_MODULE                 (SOC_MODULE_WB_BASE + 2)
#define SOC_MODULE_SDK_VER                (SOC_MODULE_WB_BASE + 3)


int
soc_sw_db_init(int unit);

int
soc_sw_db_sync(int unit);

extern int
soc_dpp_scache_ptr_get(int unit, soc_scache_handle_t handle, soc_dpp_scache_oper_t oper,
                                 int flags, uint32 *size, uint8 **scache_ptr,
                                 uint16 ver, uint16 *recovered_ver, int *already_exists);

#endif
