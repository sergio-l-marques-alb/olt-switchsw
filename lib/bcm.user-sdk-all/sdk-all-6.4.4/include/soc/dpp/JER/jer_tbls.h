/*
 * $Id: drv.h,v 1.0 Broadcom SDK $
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
 */

#ifndef _SOC_DPP_JER_TBLS_H
#define _SOC_DPP_JER_TBLS_H

#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <shared/bsl.h>

#define SOC_JER_TBLS_MAX_WORDS_FOR_REGULAR_STATIC_TABLE_ENTRY 20

uint32 soc_jer_sch_tbls_init (int unit);
uint32 soc_jer_irr_tbls_init (int unit);
uint32 soc_jer_ire_tbls_init (int unit);
uint32 soc_jer_ihb_tbls_init (int unit);
uint32 soc_jer_iqm_tbls_init (int unit);
uint32 soc_jer_ips_tbls_init (int unit);
uint32 soc_jer_ipt_tbls_init (int unit);
uint32 soc_jer_fdt_tbls_init (int unit);
uint32 soc_jer_egq_tbls_init (int unit);
uint32 soc_jer_epni_tbls_init (int unit);
uint32 soc_jer_oamp_tbls_init (int unit);

/*
 * Function:
 *      soc_jer_excluded_tbls_list_set
 * Purpose:
 *      sets the excluded memory list with the relevant memories
 * Parameters:
 *      unit    - Device Number 
 * Returns:
 *      SOC_E_XXX 
 * Note:
 *      to insert a memory to excluded list write the memory's name in the relevant exclude list above
 */
int soc_jer_excluded_tbls_list_set (int unit);

/*
 * Function:
 *      soc_jer_static_tbls_reset
 * Purpose:
 *      iterates over all memories and resets the static ones
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_static_tbls_reset (int unit);
/*
 * Read table SCH_DEVICE_RATE_MEMORY_DRM from block SCH,
 * doesn't take semaphore.
 */
uint32
  jer_sch_drm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_DRM_TBL_DATA* SCH_drm_tbl_data
  ) ;
/*
 * Write table SCH_DEVICE_RATE_MEMORY_DRM from block SCH,
 * doesn't take semaphore.
 */
uint32
  jer_sch_drm_tbl_set_unsafe(
    SOC_SAND_IN   int                   unit,
    SOC_SAND_IN   int                   core,
    SOC_SAND_IN   uint32                entry_offset,
    SOC_SAND_IN   ARAD_SCH_DRM_TBL_DATA *SCH_drm_tbl_data
  ) ;
/*
 * Read indirect table shds_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  jer_sch_shds_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  );
/*
 * Write indirect table shds_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  jer_sch_shds_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  );

#define JER_TBL_SOC_REF(unit, tbl_ptr, field, field_dst)       \
{                                                     \
    soc_mem_info_t *meminfo;                            \
    soc_field_info_t    *fieldinfo;                     \
                                                      \
        meminfo = &SOC_MEM_INFO(unit, tbl_ptr);           \
        SOC_FIND_FIELD(field,                                   \
                       meminfo->fields,                         \
                       meminfo->nFields,                        \
                       fieldinfo);                              \
        if (NULL != fieldinfo) {                          \
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_os_memcpy(field_dst, fieldinfo, sizeof(soc_field_info_t)));  \
                    }\
        else {\
            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);\
        }\
}

#endif /* _SOC_DPP_JER_TBLS_H */
