/*
 * $Id: drv.c,v 1.7 Broadcom SDK $
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
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/dnx/drv.h>
#include <soc/mcm/driver.h>
#include "include/soc/dnx/cmodel/cmodel_reg_access.h"

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
                    (BSL_META_U(unit, "soc_dnx_info_config: driver device %04x unexpected\n"), dev_id));
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
    soc_dnx_control_t    *dnx = NULL;
    uint16               dev_id;
    uint8                rev_id;
    int                  mem;
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
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("unit %d is not DNX type"), unit));
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
                              "%s: unit %d soc_dnx_chip_type_set"
                              " failed (%s)\n"), FUNCTION_NAME(), unit, soc_errmsg(rv)));
    }



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

    dnx = SOC_DNX_CONTROL(unit);
    if (dnx != NULL) {
        SOCDNX_IF_ERR_EXIT(SOC_E_INIT);
    }
    
    dnx = (soc_dnx_control_t *) sal_alloc(sizeof(soc_dnx_control_t), "soc_dnx_control");
    if (dnx == NULL) {
       SOCDNX_IF_ERR_EXIT(SOC_E_MEMORY);
    }
    sal_memset(dnx, 0, sizeof (soc_dnx_control_t));

    SOC_CONTROL(unit)->drv = dnx;
        
    dnx->cfg = (soc_dnx_config_t *) sal_alloc(sizeof(soc_dnx_config_t), "soc_dnx_cofig");
    if (dnx->cfg == NULL) {
        SOCDNX_IF_ERR_EXIT(SOC_E_MEMORY);
    } 

    sal_memset(dnx->cfg, 0, sizeof (soc_dnx_config_t));


	rv = soc_dnx_info_config(unit);    
    SOCDNX_IF_ERR_EXIT(rv);
      
#if 0
    rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_drv_device_attach,(unit));
    SOCDNX_IF_ERR_EXIT(rv);
#endif


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

    soc->soc_flags |= SOC_F_ATTACHED;

    /* Init Feature list */
    soc_feature_init(unit);

#ifdef CMODEL_SERVER_MODE
    cmodel_reg_access_init(unit);
#endif

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
    soc_dnx_config_t *dnx; 
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
    
    if (SOC_DNX_CONTROL(unit) != NULL) {
        /* Free Arad SOC config */
        dnx = SOC_DNX_CONFIG(unit);
        if (dnx != NULL) {
#ifdef BCM_DNX_SUPPORT
            if (dnx->jer2 != NULL) { /* jer2_config */
                 sal_free(dnx->jer2);
                 dnx->jer2 = NULL;
            }
#endif /* BCM_DNX_SUPPORT */
            /* free drv cnf */    
            sal_free(SOC_DNX_CONFIG(unit));
        }
        sal_free(SOC_DNX_CONTROL(unit));
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

    SOCDNX_IF_ERR_EXIT(soc_dnx_info_config_blocks(unit));

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

int
soc_dnx_info_config_blocks(int unit)
{
    soc_info_t  *si;
    int         blk, blktype, blknum;
    char        instance_string[3];

    SOCDNX_INIT_FUNC_DEFS;

    si  = &SOC_INFO(unit);

    sal_memset(si->has_block, 0, sizeof(soc_block_t) * COUNTOF(si->has_block));
    sal_memset(si->broadcast_blocks, 0, sizeof(si->broadcast_blocks));
    sal_memset(si->broadcast_blocks_size, 0, sizeof(si->broadcast_blocks_size));

    for (blk = 0; blk < SOC_MAX_NUM_BLKS; blk++) {
        si->block_port[blk] = REG_PORT_ANY;
        si->block_valid[blk] = 0;
    }

    si->cmic_block = -1;
    si->iproc_block = -1;

    /* setting broadcast blocks as "regular blocks" by default */
    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++) {
        blktype = SOC_BLOCK_INFO(unit, blk).type;
        switch(blktype)
        {
            case SOC_BLK_CGM:
                si->brdc_cgm_block = blk;
                break;
            case SOC_BLK_EGQ:
                si->brdc_egq_block = blk;
                break;
            case SOC_BLK_EPNI:
                si->brdc_epni_block = blk;
                break;
            case SOC_BLK_IHB:
                si->brdc_ihb_block = blk;
                break;
            case SOC_BLK_IHP:
                si->brdc_ihp_block = blk;
                break;
            case SOC_BLK_IPS:
                si->brdc_ips_block = blk;
                break;
            case SOC_BLK_IQM:
                si->brdc_iqm_block = blk;
                break;
            case SOC_BLK_SCH:
                si->brdc_sch_block = blk;
                break;
        }
    }

    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++) {
        blktype = SOC_BLOCK_INFO(unit, blk).type;
        blknum = SOC_BLOCK_INFO(unit, blk).number;
        if (blknum < 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: illegal block instance number %d"), blknum));
        }

        if(blk >= SOC_MAX_NUM_BLKS) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: too much blocks for device")));
        }

        si->has_block[blk] = blktype;
        sal_snprintf(instance_string, sizeof(instance_string), "%d", blknum);

        si->block_valid[blk] = 1;
        
        switch(blktype)
        {
            case SOC_BLK_IPS:
                if (blknum >= SOC_MAX_NUM_IPS_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many IPS blocks")));
                }
                si->ips_blocks[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_PQP:
                if (blknum >= SOC_MAX_NUM_PQP_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many PQP blocks")));
                }
                si->pqp_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;				
            case SOC_BLK_CRPS:
                si->crps_block = blk;
                break;
            case SOC_BLK_MRPS:
                if (blknum >= SOC_MAX_NUM_MRPS_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many MRPS blocks")));
                }
                si->mrps_blocks[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;   			
            case SOC_BLK_IRE:
                si->ire_block = blk;
                break;
            case SOC_BLK_IPT:
                si->ipt_block = blk;
                break;				
            case SOC_BLK_OCB:
                if (blknum >= SOC_MAX_NUM_OCB_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many OCB blocks")));
                }
                si->ocb_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;				
            case SOC_BLK_FMAC:
                if (blknum >= SOC_MAX_NUM_FMAC_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many FMAC blocks")));
                }
                si->fmac_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;				
            case SOC_BLK_FDA:
                si->fda_block = blk;
                break;				
            case SOC_BLK_RTP:
                si->rtp_block = blk;
                break;				
            case SOC_BLK_FCR:
                si->fcr_block = blk;
                break;				
            case SOC_BLK_FCT:
                si->fct_block = blk;
                break;				
            case SOC_BLK_FDT:
                si->fdt_block = blk;
                break;
            case SOC_BLK_FDR:
                si->fdr_block = blk;
                break;
            case SOC_BLK_EDB:
                si->edb_block = blk;
                break;
            case SOC_BLK_EPNI:
                if (blknum >= SOC_MAX_NUM_EPNI_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many EPNI blocks")));
                }
                si->epni_blocks[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_SCH:
                if (blknum >= SOC_MAX_NUM_SCH_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many SCH blocks")));
                }
                si->sch_blocks[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;			
            case SOC_BLK_ECI:
                si->eci_block = blk;
                break;
            case SOC_BLK_CFC:
                if (blknum >= SOC_MAX_NUM_CFC_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many CFC blocks")));
                }
                si->cfc_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_OAMP:
                si->oamp_block = blk;
                break;
            case SOC_BLK_OLP:
                si->olp_block = blk;
                break;
            case SOC_BLK_SIF:
                if (blknum >= SOC_MAX_NUM_SIF_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many SIF blocks")));
                }
                si->sif_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_NIF_ILU:
                if (blknum >= SOC_MAX_NUM_NIF_ILU_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many ILU blocks")));
                }
                si->nif_ilu_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_NIF_CDU:
                if (blknum >= SOC_MAX_NUM_NIF_CDU_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many CDU blocks")));
                }
                si->nif_cdu_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_NIF_NMG:
                if (blknum >= SOC_MAX_NUM_NIF_NMG_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many NMG blocks")));
                }
                si->nif_nmg_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_MCP:
                if (blknum >= SOC_MAX_NUM_MCP_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many MCP blocks")));
                }
                si->mcp_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_ITPP:
                if (blknum >= SOC_MAX_NUM_ITPP_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many ITPP blocks")));
                }
                si->itpp_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_PDM:
                if (blknum >= SOC_MAX_NUM_PDM_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many PDM blocks")));
                }
                si->pdm_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_BDM:
                if (blknum >= SOC_MAX_NUM_BDM_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many BDM blocks")));
                }
                si->bdm_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_MTM:
                si->mtm_block = blk;
                break;
            case SOC_BLK_HBM_TDU:
                si->hbm_tdu_block = blk;
                break;
            case SOC_BLK_HBM:
                if (blknum >= SOC_MAX_NUM_HBM_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many HBM blocks")));
                }
                si->hbm_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_RQP:
                if (blknum >= SOC_MAX_NUM_RQP_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many RQP blocks")));
                }
                si->rqp_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_EQM:
                if (blknum >= SOC_MAX_NUM_EQM_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many EQM blocks")));
                }
                si->eqm_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_EPS_SPR:
                if (blknum >= SOC_MAX_NUM_EPS_SPR_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many EPS_SPR blocks")));
                }
                si->eps_spr_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_FQP:
                if (blknum >= SOC_MAX_NUM_FQP_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many FQP blocks")));
                }
                si->fqp_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_DHA:
                if (blknum >= SOC_MAX_NUM_DHA_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many DHA blocks")));
                }
                si->dha_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_DHB:
                if (blknum >= SOC_MAX_NUM_DHB_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many DHB blocks")));
                }
                si->dhb_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_MDB:
                si->mdb_block = blk;
                break;
            case SOC_BLK_ERPP:
                if (blknum >= SOC_MAX_NUM_ERPP_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many ERPP blocks")));
                }
                si->erpp_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_ETPPA:
                if (blknum >= SOC_MAX_NUM_ETPPA_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many ETPPA blocks")));
                }
                si->etppa_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_ETPPB:
                if (blknum >= SOC_MAX_NUM_ETPPB_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many ETPPB blocks")));
                }
                si->etppb_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_EVENTOR:
                si->eventor_block = blk;
                break;
            case SOC_BLK_MACT:
                si->mact_block = blk;
                break;
            case SOC_BLK_IPPA:
                if (blknum >= SOC_MAX_NUM_IPPA_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many IPPA blocks")));
                }
                si->ippa_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_IPPB:
                if (blknum >= SOC_MAX_NUM_IPPB_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many IPPB blocks")));
                }
                si->ippb_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_IPPC:
                if (blknum >= SOC_MAX_NUM_IPPC_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many IPPC blocks")));
                }
                si->ippc_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_IPPD:
                if (blknum >= SOC_MAX_NUM_IPPD_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many IPPD blocks")));
                }
                si->ippd_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            default:
                break;            
        }
              
        si->block_valid[blk] += 1;

        sal_snprintf(si->block_name[blk], sizeof(si->block_name[blk]),
                     "%s%s",
                     soc_block_name_lookup_ext(blktype, unit),
                     instance_string);
    }
    si->block_num = blk;

exit:
    SOCDNX_FUNC_RETURN;

}
