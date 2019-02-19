/*
 * $Id: drv.c,v 1.594 Broadcom SDK $
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
 */
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#include <shared/bsl.h>
#include <shared/swstate/sw_state.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/dnx/drv.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>
#include <soc/mcm/driver.h>

soc_driver_t *
soc_dnx_chip_driver_find(uint16 pci_dev_id, uint8 pci_rev_id)
{
    uint16              driver_dev_id;
    uint8               driver_rev_id;

    if (soc_cm_get_id_driver(pci_dev_id, pci_rev_id, &driver_dev_id, &driver_rev_id) < 0) {
        return NULL;
    }
    switch(driver_dev_id)
    {
#if defined(BCM_DNX_SUPPORT)
        case BCM88690_DEVICE_ID:
            return &soc_driver_bcm88690_a0;
#endif
        default:
            LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META("soc_chip_driver_find: driver in devid table not in soc_base_driver_table\n")));
            break;         
    }
    return NULL;
}

int
soc_dnx_chip_type_set(int unit, uint16 dev_id)
{
    soc_info_t           *si; 

    SOCDNX_INIT_FUNC_DEFS;

    si  = &SOC_INFO(unit);

    /*
     * Used to implement the SOC_IS_*(unit) macros
     */
    switch (dev_id) {
#ifdef BCM_DNX_SUPPORT
    case JERICHO_2_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_JERICHO_2;
        SOC_CHIP_STRING(unit) = "jericho_2";
        break;
#endif
    default:
        si->chip_type = 0;
        SOC_CHIP_STRING(unit) = "???";
        LOG_VERBOSE(BSL_LS_SOC_INIT,
                    (BSL_META_U(unit, "soc_dpp_info_config: driver device %04x unexpected\n"), dev_id));
        break;
    }
    SOCDNX_FUNC_RETURN;
}

int
soc_dnx_attach(int unit)
{
    soc_control_t        *soc;
    soc_persist_t        *sop;
    soc_info_t           *si; 
    soc_dpp_control_t    *dpp = NULL;
    uint16               dev_id;
    uint8                rev_id;
    int                  rv=0;
    int                  cmc;
    
    SOCDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_SOC_INIT,
                (BSL_META_U(unit,
                            "%s: unit %d\n"), FUNCTION_NAME(), unit));

    /* Allocate soc_control and soc_persist if not already. */
    soc = SOC_CONTROL(unit);
    if (soc != NULL) {  
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("unit %d has already attached"), unit));
    }
    soc = sal_alloc(sizeof (soc_control_t), "soc_control");
    if (soc == NULL) {
        SOCDNX_IF_ERR_EXIT(SOC_E_MEMORY);
    }
    sal_memset(soc, 0, sizeof (soc_control_t));
    SOC_CONTROL(unit) = soc;

    SOC_PERSIST(unit) = sal_alloc(sizeof (soc_persist_t), "soc_persist");
    if (SOC_PERSIST(unit) == NULL) {
        SOCDNX_IF_ERR_EXIT(SOC_E_MEMORY);
    }
    sal_memset(SOC_PERSIST(unit), 0, sizeof (soc_persist_t));
    sop = SOC_PERSIST(unit);
    sop->version = 1;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (SOC_IS_DNX_TYPE(dev_id) == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("unit %d is not DPP type"), unit));
    }

    /* Instantiate the driver -- Verify chip revision matches driver
     * compilation revision.
     */
    soc->chip_driver = soc_dnx_chip_driver_find(dev_id, rev_id);
    if (soc->chip_driver == NULL) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "%s: unit %d has no driver "
                              "(device 0x%04x rev 0x%02x)\n"),
                   FUNCTION_NAME(), unit, dev_id, rev_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }

    si  = &SOC_INFO(unit);
    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];

    rv = soc_dnx_chip_type_set(unit, dev_id);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "%s: unit %d soc_dpp_chip_type_set"
                              " failed (%s)\n"), FUNCTION_NAME(), unit, soc_errmsg(rv)));
    }


    SOCDNX_IF_ERR_EXIT(soc_dpp_defines_init(unit));

    SOC_PCI_CMCS_NUM(unit) = soc_property_uc_get(unit, 0, spn_PCI_CMCS_NUM, 1);

    /*
     * Create mutexes.
     */
    if ((soc->schan_wb_mutex = sal_mutex_create("SchanWB")) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate SchanWB")));
    }
    
    if ((soc->socControlMutex = sal_mutex_create("SOC_CONTROL")) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate soc_control lock")));
    }

    if( (soc->counterMutex = sal_mutex_create("Counter")) == NULL) {      
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate counter Lock")));
    }

    if ((soc->miimMutex = sal_mutex_create("MIIM")) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate MIIM lock")));
    }

    if((soc->miimIntr = sal_sem_create("MIIM interrupt", sal_sem_BINARY, 0)) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate MIIM interrup Sem")));
    }

    if( (soc->schanMutex = sal_mutex_create("SCHAN")) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate Schan Lock")));
    }
    
    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++) {
        if ((soc->schanIntr[cmc] =
             sal_sem_create("SCHAN interrupt", sal_sem_BINARY, 0)) == NULL) {
             SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate Schan interrupt Sem")));
        }
    }
    
#ifdef BCM_CMICM_SUPPORT  
    soc->fschanMutex = NULL;   
    if ((soc->fschanMutex = sal_mutex_create("FSCHAN")) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate fSchan Lock")));
    }
#endif /* BCM_CMICM_SUPPORT */

#ifdef INCLUDE_MEM_SCAN
    /* do not enable memscan task, otherwise tr 50 & 51 will segmentation fail*/
    soc->mem_scan_pid = SAL_THREAD_ERROR;
    soc->mem_scan_interval = 0;
#endif

    dpp = SOC_DPP_CONTROL(unit);
    if (dpp != NULL) {
        SOCDNX_IF_ERR_EXIT(SOC_E_INIT);
    }
    
    dpp = (soc_dpp_control_t *) sal_alloc(sizeof(soc_dpp_control_t), "soc_dpp_control");
    if (dpp == NULL) {
       SOCDNX_IF_ERR_EXIT(SOC_E_MEMORY);
    }
    sal_memset(dpp, 0, sizeof (soc_dpp_control_t));

    SOC_CONTROL(unit)->drv = dpp;
        
    dpp->cfg = (soc_dpp_config_t *) sal_alloc(sizeof(soc_dpp_config_t), "soc_dpp_cofig");
    if (dpp->cfg == NULL) {
        SOCDNX_IF_ERR_EXIT(SOC_E_MEMORY);
    } 

    sal_memset(dpp->cfg, 0, sizeof (soc_dpp_config_t));


	rv = soc_dnx_info_config(unit);    
    SOCDNX_IF_ERR_EXIT(rv);
      
#if 0
    rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_drv_device_attach,(unit));
    SOCDNX_IF_ERR_EXIT(rv);

    /*
     * Initialize memory index_maxes. Chip specific overrides follow.
     */
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (SOC_MEM_IS_VALID(unit, mem)) {
            sop->memState[mem].index_max = SOC_MEM_INFO(unit, mem).index_max;
            /*
             * should only create mutexes for valid memories. 
             */
            if ((soc->memState[mem].lock =
                 sal_mutex_create(SOC_MEM_NAME(unit, mem))) == NULL) {
                return SOC_E_MEMORY;
            }
    
            /* Set cache copy pointers to NULL */
            sal_memset(soc->memState[mem].cache,
                       0,
                       sizeof (soc->memState[mem].cache));
        } else {
            sop->memState[mem].index_max = -1;
        }
    }
#endif


    soc->soc_flags |= SOC_F_ATTACHED;

    /* Init Feature list */
    soc_feature_init(unit);

    /* Initialize SCHAN */
    rv = soc_schan_init(unit);
    if (rv != SOC_E_NONE) {
       SOCDNX_EXIT_WITH_ERR(rv, (_BSL_SOCDNX_MSG("failed to Initialize SCHAN")));
    }

exit: 
    if(SOCDNX_FUNC_ERROR) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "soc_attach: unit %d failed (%s)"), unit, soc_errmsg(rv)));
        soc_dnx_detach(unit); 
    }
    DISPLAY_MEM ;

    SOCDNX_FUNC_RETURN;
}

int
soc_dnx_detach(int unit)
{
    soc_control_t       *soc;
    soc_dpp_config_t *dpp; 
    int cmc;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNIT);
    }

    soc = SOC_CONTROL(unit);
    if (soc == NULL) {
        SOC_EXIT;
    }
    
 #if 0   /* Destroy memory mutex */ 
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (soc->memState[mem].lock != NULL) {
            sal_mutex_destroy(soc->memState[mem].lock);
            soc->memState[mem].lock = NULL;                    
        }
    } 
#endif     
    
    if (SOC_DPP_CONTROL(unit) != NULL) {
        /* Free Arad SOC config */
        dpp = SOC_DPP_CONFIG(unit);
        if (dpp != NULL) {
#ifdef BCM_ARAD_SUPPORT
            if (dpp->arad != NULL) { /* arad_config */
                 sal_free(dpp->arad);
                 dpp->arad = NULL;
            }
#endif /* BCM_ARAD_SUPPORT */
#ifdef BCM_JERICHO_SUPPORT
            if (dpp->jer != NULL) { /* Jericho config */
                 sal_free(dpp->jer);
                 dpp->jer = NULL;
            }
            if (dpp->qax != NULL) { /* QAX config */
                 sal_free(dpp->qax);
                 dpp->qax = NULL;
            }
#endif /* BCM_JERICHO_SUPPORT */
            /* free drv cnf */    
            sal_free(SOC_DPP_CONFIG(unit));
        }
        sal_free(SOC_DPP_CONTROL(unit));
    }
      
    /* Destroy Sem/Mutex */  
#ifdef BCM_CMICM_SUPPORT  
    if(soc->fschanMutex != NULL) {
        sal_mutex_destroy(soc->fschanMutex);
        soc->fschanMutex = NULL;
    }
#endif /* BCM_CMICM_SUPPORT */    

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++) {
        if (soc->schanIntr[cmc]) {
            sal_sem_destroy(soc->schanIntr[cmc]);
            soc->schanIntr[cmc] = NULL;
        }
    }
    if (soc->schanMutex != NULL) {
        sal_mutex_destroy(soc->schanMutex);
        soc->schanMutex = NULL;
    }    
    if(soc->miimIntr != NULL ) {
        sal_sem_destroy(soc->miimIntr);
        soc->miimIntr = NULL;
    }

    if (soc->miimMutex != NULL) {
        sal_mutex_destroy(soc->miimMutex);
        soc->miimMutex = NULL;
    }

    if (soc->counterMutex != NULL) {
        sal_mutex_destroy(soc->counterMutex);
        soc->counterMutex = NULL;
    }

    if(soc->socControlMutex != NULL ) {
        sal_mutex_destroy(soc->socControlMutex);
        soc->socControlMutex = NULL;
    }

    if (soc->schan_wb_mutex != NULL) {
        sal_mutex_destroy(soc->schan_wb_mutex);
        soc->schan_wb_mutex = NULL;
    }
    
    if (SOC_PERSIST(unit) != NULL) {
        sal_free(SOC_PERSIST(unit));
        SOC_PERSIST(unit) = NULL;
    }

    sal_free(soc);
    SOC_CONTROL(unit) = NULL;

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dnx_info_config(int unit)
{
    soc_info_t          *si;
    soc_control_t       *soc;
    int                 mem, blk;

    SOCDNX_INIT_FUNC_DEFS;

    soc = SOC_CONTROL(unit);

    si  = &SOC_INFO(unit);
    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];

    SOCDNX_IF_ERR_EXIT(soc_dpp_info_config_blocks(unit));

    /*
     * Calculate the mem_block_any array for this configuration
     * The "any" block is just the first one enabled
     */
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        si->mem_block_any[mem] = -1;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            SOC_MEM_BLOCK_ITER(unit, mem, blk) {
                si->mem_block_any[mem] = blk;
                break;
            }
        }
    }

    /* use of gports is true for all SAND devices */
    SOC_USE_GPORT_SET(unit, TRUE);

exit:
    SOCDNX_FUNC_RETURN
}
