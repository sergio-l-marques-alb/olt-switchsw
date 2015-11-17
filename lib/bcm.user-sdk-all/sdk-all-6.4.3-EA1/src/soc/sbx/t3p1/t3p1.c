/*
 * $Id: t3p1.c,v 1.1.2.2 Broadcom SDK $
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

#include <shared/bsl.h>

#include <soc/sbx/t3p1/t3p1.h>
#include <soc/sbx/t3p1/t3p1_int.h>
#include <soc/sbx/t3p1/t3p1_defs.h>

#ifdef BCM_CALADAN3_T3P1_SUPPORT

#define PPE_CFG_INIT 1
#define PED_CFG_INIT 1
#define COP_CFG_INIT 1
#define CMU_CFG_INIT 1
#define TMU_CFG_INIT 1

/*
 * Function: t3p1_app_init
 *    Initialize the Empty Microcode Application
 * Parameters
 *    unit: Input, unit number
 *    contexts: Output, number of contexts used in application
 *    epoch: Output, epoch len or number of instr in application
 * Returns
 *    error status: one of SOC_E* 
 */
int
t3p1_app_init(int unit, soc_sbx_caladan3_ucode_pkg_t *pkg, uint32 *contexts, uint32 *epoch, int reload)
{
    int rv = SOC_E_NONE;
    soc_sbx_control_t *sbx;

    sbx = SOC_SBX_CONTROL(unit);
    if (!sbx) {
        return SOC_E_INIT;
    }

    if (!reload) {

        if (!sbx->drv) {
            sbx->drv = sal_alloc(sizeof(soc_sbx_t3p1_state_t), "t3p1 ucode state");
            if (!sbx->drv) {
                return SOC_E_MEMORY;
            }
        }
        sal_memset(sbx->drv, 0, sizeof(soc_sbx_t3p1_state_t));

#if COP_CFG_INIT
        rv = soc_sbx_t3p1_cop_init(unit);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Caladan3 unit %d COP init failed %d\n"),
                       unit, rv));
            return (rv);
        }
#endif
#if CMU_CFG_INIT
        rv = soc_sbx_t3p1_cmu_init(unit);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Caladan3 unit %d CMU init failed %d\n"),
                       unit, rv));
            return (rv);
        }
#endif

        rv = soc_sbx_t3p1_init(unit, (void*)pkg);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_sbx_caladan3_ucodemgr_app_init: unit %d microcode application init failed (%d) \n"), unit, rv));
            return rv;
        }

#if PED_CFG_INIT
        rv = soc_sbx_t3p1_ped_init(unit);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_sbx_caladan3_ucodemgr_app_init: unit %d PED header init %d \n"), unit, rv));
            return rv;
        }
#endif
#if PPE_CFG_INIT
        rv = soc_sbx_t3p1_ppe_init(unit);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_sbx_caladan3_ucodemgr_app_init: unit %d PPE configuration failed %d \n"), unit, rv));
            return SOC_E_PARAM;
        }
        rv = soc_sbx_t3p1_ppe_tables_init(unit);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Caladan3 unit %d PPE table manager init failed %d\n"),
                       unit, rv));
            return (rv);
        }
        /*
        rv = soc_sbx_t3p1_ppe_init_ext(unit);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Caladan3 unit %d PPE hand coded init failed %d\n"),
                       unit, rv));
            return (rv);
        }
        */
#endif
#if TMU_CFG_INIT
        rv = soc_sbx_t3p1_tmu_init(unit);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Caladan3 unit %d TMU table manager init failed %d\n"),
                       unit, rv));
            return (rv);
        }
#endif

    }

    /*
     * Microcode application initialized successfully
     * Final steps before letting LR execute
     *   1) Set application params to LRP
     *   2) Set SDK overrides
     */

    /*
     * LR parameters:
     * 1) Epoch length,
     *      can this be directly taken from the ucode->m_inum
     * 2) Num of contexts used
     */
    rv = soc_sbx_t3p1_global_get(unit, "elen", epoch);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_sbx_caladan3_ucodemgr_app_init: unit %d Reading Epoch len failed (%d), \n"), unit, rv));
        return rv;
    }

    rv = soc_sbx_t3p1_global_get(unit, "CONTEXTS", contexts);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_sbx_caladan3_ucodemgr_app_init: unit %d Reading number of contexts used failed (%d) \n"),
                   unit, rv));
        return rv;
    }

    /* Set the microcode type */
    SOC_SBX_CONTROL(unit)->ucodetype = SOC_SBX_UCODE_TYPE_T3P1;

    sal_memcpy(sbx->uver_name,
               pkg->m_uver_name,
               strlen(pkg->m_uver_name));
    sbx->uver_maj   = pkg->m_uver_maj;
    sbx->uver_min   = pkg->m_uver_min;
    sbx->uver_patch = pkg->m_uver_patch;

    if (!soc_sbx_ucode_versions[SOC_SBX_CONTROL(unit)->ucodetype]) {
        soc_sbx_ucode_versions[SOC_SBX_CONTROL(unit)->ucodetype] =
            sal_alloc(64, "ucode ver");
        if (!soc_sbx_ucode_versions[SOC_SBX_CONTROL(unit)->ucodetype]) {
            return SOC_E_MEMORY;
        }
    }
    sal_sprintf(soc_sbx_ucode_versions[SOC_SBX_CONTROL(unit)->ucodetype], "%s %d.%d.%d",
                sbx->uver_name,
                sbx->uver_maj,
                sbx->uver_min,
                sbx->uver_patch);

    
    if ((sal_strlen(sbx->uver_name) == 5) &&
         (sal_strcmp(sbx->uver_name, "t3p1a")==0)) {
        soc_sbx_caladan3_sws_pr_icc_program_arad_header(unit, 0);
    } else {
        soc_sbx_caladan3_sws_pr_icc_program_sirius_header(unit, 0);
    }

    return rv;
}

#endif

