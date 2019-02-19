/*

 * $Id: dnxf_drv.c,v 1.87 Broadcom SDK $

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


#include <soc/mcm/driver.h>     /* soc_base_driver_table */
#include <soc/error.h>
#include <soc/ipoll.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/linkctrl.h>

#include <soc/dcmn/fabric.h>
#include <soc/dcmn/error.h>

#include <soc/dnxf/cmn/dnxf_drv.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_config_defs.h>
#include <soc/dnxf/cmn/dnxf_config_imp_defs.h>
#include <soc/dcmn/dcmn_cmic.h>
#include <soc/dcmn/dcmn_mem.h>

soc_driver_t*
soc_dnxf_chip_driver_find(int unit , uint16 pci_dev_id, uint8 pci_rev_id)
{
    uint16              driver_dev_id;
    uint8               driver_rev_id;

    if (soc_cm_get_id_driver(pci_dev_id, pci_rev_id, &driver_dev_id, &driver_rev_id) < 0) {
        return NULL;
    }

     switch(driver_dev_id)
     {
     case BCM88790_DEVICE_ID:
        if (pci_rev_id == BCM88790_A0_REV_ID) {
           return &soc_driver_bcm88790_a0;
        } else {
             LOG_ERROR(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_dnxf_chip_driver_find: driver in devid table "
                                   "not in soc_base_driver_table\n")));
        }
         break;
       default:
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "soc_dnxf_chip_driver_find: driver in devid table "
                                  "not in soc_base_driver_table\n")));
           break;         
     }
  
    return NULL;
}

int
soc_dnxf_info_config(int unit, int dev_id)
{
    soc_info_t          *si;
    soc_control_t       *soc;
    int                 mem, blk, blktype;
    char                instance_string[3];
    int port, phy_port, bindex;

    SOCDNX_INIT_FUNC_DEFS;

    soc = SOC_CONTROL(unit);

    /*set chip string*/
    switch (dev_id) {
        case BCM88790_DEVICE_ID:
            SOC_CHIP_STRING(unit) = "ramon";
            break;

        default:
            SOC_CHIP_STRING(unit) = "???";
           LOG_ERROR(BSL_LS_SOC_INIT,
                     (BSL_META_U(unit,
                                 "soc_dnxf_info_config: driver device %04x unexpected\n"),
                                 dev_id));
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("failed to find device id")));
    }


    si  = &SOC_INFO(unit);
    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];

    SOCDNX_IF_ERR_CONT(soc_dnxf_defines_init(unit));

    SOCDNX_IF_ERR_CONT(soc_dnxf_implementation_defines_init(unit));


    si->fe.min          = si->fe.max          = -1;         si->fe.num = 0;
    si->ge.min          = si->ge.max          = -1;         si->ge.num = 0;
    si->xe.min          = si->xe.max          = -1;         si->xe.num = 0;
    si->hg.min          = si->hg.max          = -1;         si->hg.num = 0;
    si->hg_subport.min  = si->hg_subport.max  = -1;         si->hg_subport.num = 0;
    si->hl.min          = si->hl.max          = -1;         si->hl.num = 0;
    si->st.min          = si->st.max          = -1;         si->st.num = 0;
    si->gx.min          = si->gx.max          = -1;         si->gx.num = 0;
    si->xg.min          = si->xg.max          = -1;         si->xg.num = 0;
    si->spi.min         = si->spi.max         = -1;         si->spi.num = 0;
    si->spi_subport.min = si->spi_subport.max = -1;         si->spi_subport.num = 0;
    si->sci.min         = si->sci.max         = -1;         si->sci.num = 0;
    si->sfi.min         = si->sfi.max         = -1;         si->sfi.num = 0;
    si->port.min        = si->port.max        = -1;         si->port.num = 0;
    si->ether.min       = si->ether.max       = -1;         si->ether.num = 0;
    si->all.min         = si->all.max         = -1;         si->all.num = 0;
    
    
    si->port_num = 0;

    sal_memset(si->has_block, 0, sizeof(soc_block_t) * COUNTOF(si->has_block));

    for (blk = 0; blk < SOC_MAX_NUM_BLKS; blk++) {
        si->block_port[blk] = REG_PORT_ANY;
        si->block_valid[blk] = 0;
    }

    SOC_PBMP_CLEAR(si->cmic_bitmap);

    si->cmic_block = -1;

    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++) {
        blktype = SOC_BLOCK_INFO(unit, blk).type;

        if(blk >= SOC_MAX_NUM_BLKS)
        {
              SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dnxf_info_config: too much blocks for device \n")));
        }
        si->has_block[blk] = blktype;
        sal_snprintf(instance_string, sizeof(instance_string), "%d",
                     SOC_BLOCK_INFO(unit, blk).number);

        /* To Do - dispatcher for DNXF must be created */
        /* rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_block_valid_get, (unit, blktype, SOC_BLOCK_INFO(unit, blk).number, &(si->block_valid[blk])));
        SOCDNX_IF_ERR_EXIT(rv); */

        si->block_valid[blk]=1;

        switch(blktype)
        {
            case SOC_BLK_ECI:
                si->eci_block = blk;
                break;
            case SOC_BLK_AVS:
                si->avs_block = blk;
                break;
            case SOC_BLK_MESH_TOPOLOGY:
                si->mesh_topology_block = blk;
                break;
            case SOC_BLK_CMIC:
                si->cmic_block = blk;
                break;
            case SOC_BLK_FMAC:
                if(SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_FMAC_BLKS) {
                    si->fmac_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dnxf_info_config: too much FMAC blocks")));
                }
                break;
            case SOC_BLK_OTPC:
                si->otpc_block = blk;
                break;
            case SOC_BLK_FSRD:
                if(SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_FSRD_BLKS) {
                    si->fsrd_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dnxf_info_config: too much FSRD blocks")));
                }
                break;
            case SOC_BLK_RTP:
                si->rtp_block = blk;
                break;
            case SOC_BLK_OCCG:
                si->occg_block = blk;
                break;
            case SOC_BLK_DCH:
                if(SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_DCH_BLKS) {
                    si->dch_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                     SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dnxf_info_config: too much DCH blocks")));
                }
                break;
            case SOC_BLK_DCML:
                if (SOC_BLOCK_INFO(unit,blk).number < SOC_MAX_NUM_DCML_BLKS) {
                    si->dcml_block[SOC_BLOCK_INFO(unit,blk).number] = blk;
                    si->block_port[blk]= SOC_BLOCK_INFO(unit,blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dnxf_info_config: too much DCML blocks")));
                }
                break;
            case SOC_BLK_MCT:
                si->mct_block=blk;
                break;
            case SOC_BLK_QRH:
                if (SOC_BLOCK_INFO(unit,blk).number < SOC_MAX_NUM_QRH_BLKS) {
                    si->qrh_block[SOC_BLOCK_INFO(unit,blk).number] = blk;
                    si->block_port[blk]= SOC_BLOCK_INFO(unit,blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dnxf_info_config: too much QRH blocks")));
                }
                break;
            case SOC_BLK_CCH:
                if (SOC_BLOCK_INFO(unit,blk).number < SOC_MAX_NUM_CCH_BLKS) {
                    si->cch_block[SOC_BLOCK_INFO(unit,blk).number] = blk;
                    si->block_port[blk]= SOC_BLOCK_INFO(unit,blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dnxf_info_config: too much CCH blocks")));
                }
                break;
            case SOC_BLK_LCM:
                if (SOC_BLOCK_INFO(unit,blk).number < SOC_MAX_NUM_LCM_BLKS) {
                    si->lcm_block[SOC_BLOCK_INFO(unit,blk).number] = blk;
                    si->block_port[blk]= SOC_BLOCK_INFO(unit,blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dnxf_info_config: too much LCM blocks")));
                }
                break;
            case SOC_BLK_BRDC_FSRD:
                si->brdc_fsrd_block = blk;
                break;
            case SOC_BLK_BRDC_DCH:
                si->brdc_dch_block= blk;
                break;
            case SOC_BLK_BRDC_FMAC:
                si->brdc_fmac_block=blk;
                break;
            case SOC_BLK_BRDC_CCH:
                si->brdc_cch_block=blk;
                break;
            case SOC_BLK_BRDC_DCML:
                si->brdc_dcml_block=blk;
                break;
            case SOC_BLK_BRDC_LCM:
                si->brdc_lcm_block=blk;
                break;
            case SOC_BLK_BRDC_QRH:
                si->brdc_qrh_block=blk;
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dnxf_info_config: unknown block type")));
                break;            
        }
        
        sal_snprintf(si->block_name[blk], sizeof(si->block_name[blk]),
                     "%s%s",
                     soc_block_name_lookup_ext(blktype, unit),
                     instance_string);
    }
    si->block_num = blk;

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

    for (phy_port = 0; ; phy_port++) {
        
        blk = SOC_DRIVER(unit)->port_info[phy_port].blk;
        bindex = SOC_DRIVER(unit)->port_info[phy_port].bindex;
        if (blk < 0 && bindex < 0) { /* end of list */
            break;
        }
       
        port = phy_port;
        
        if (blk < 0 ) { /* empty slot*/
            blktype = 0; 
        } else {
            blktype = SOC_BLOCK_INFO(unit, blk).type;
        }
        
        if (blktype == 0) {
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                         "sfi%d", port);
            si->port_offset[port] = port;
            continue;
        }
        
        switch (blktype) {
            case SOC_BLK_CMIC:
                si->cmic_port = port;
                sal_sprintf(SOC_PORT_NAME(unit, port),"CMIC");
                SOC_PBMP_PORT_ADD(si->cmic_bitmap, port);
                break;
        default:
                si->port_num_lanes[port] = 1;
                sal_sprintf(SOC_PORT_NAME(unit, port),"sfi%d",port);
                sal_sprintf(SOC_PORT_NAME_ALTER(unit, port),"fabric%d",port);
                SOC_PORT_NAME_ALTER_VALID(unit, port) = 1;
                DNXF_ADD_PORT(sfi, port);
                DNXF_ADD_PORT(port, port);
                DNXF_ADD_PORT(all, port);
                break;
        }

        si->port_type[phy_port] = blktype;
        
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dnxf_detach(int unit)
{
    soc_control_t       *soc;
    int                  mem;
    int                  cmc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("invalid unit")));
    }

    soc = SOC_CONTROL(unit);
    if (soc == NULL) {
        SOC_EXIT;
    }


    if (soc->miimMutex) {
        sal_mutex_destroy(soc->miimMutex);
        soc->miimMutex = NULL;
    }

    if (soc->counterMutex) {
        sal_mutex_destroy(soc->counterMutex);
        soc->counterMutex = NULL;
    }

    if (soc->schanMutex) {
        sal_mutex_destroy(soc->schanMutex);
        soc->schanMutex = NULL;
    }

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++) {
        if (soc->schanIntr[cmc]) {
            sal_sem_destroy(soc->schanIntr[cmc]);
            soc->schanIntr[cmc] = NULL;
        }
    }

    (void)soc_sbusdma_lock_deinit(unit);

    /*
    * Memory mutex release
    */
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (SOC_MEM_IS_VALID(unit, mem)) {
            /*
             * Mutexes created only for valid memories. 
             */
            if (soc->memState[mem].lock) {
                 sal_mutex_destroy(soc->memState[mem].lock);
                 soc->memState[mem].lock = NULL;
            }
        }
    }

    if (soc->miimIntr) {
        sal_sem_destroy(soc->miimIntr);
        soc->miimIntr = NULL;
    }

    if (SOC_PERSIST(unit)) {
        sal_free(SOC_PERSIST(unit));
        SOC_PERSIST(unit) = NULL;
    }

    if (soc->socControlMutex) {
        sal_mutex_destroy(soc->socControlMutex);
        soc->socControlMutex = NULL;
    }

    if (_bcm_lock[unit] != NULL) {
        sal_mutex_destroy(_bcm_lock[unit]);
        _bcm_lock[unit] = NULL;
    }
    if (soc->schan_wb_mutex != NULL) {
        sal_mutex_destroy(soc->schan_wb_mutex);
        soc->schan_wb_mutex = NULL;
    }
	

    if (SOC_CONTROL(unit)->drv != NULL)
    {
    sal_free((soc_dfe_control_t *)SOC_CONTROL(unit)->drv);
    SOC_CONTROL(unit)->drv = NULL;
    }

    sal_free(soc);
    SOC_CONTROL(unit) = NULL;

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dnxf_chip_type_set(int unit, uint16 dev_id)
{
    soc_info_t           *si; 
    SOCDNX_INIT_FUNC_DEFS;

    si  = &SOC_INFO(unit);

    /*
     * Used to implement the SOC_IS_*(unit) macros
     */
    switch (dev_id) {
        case BCM88790_DEVICE_ID:
            si->chip_type = SOC_INFO_CHIP_TYPE_RAMON;
            break;

        default:
            si->chip_type = 0;
           LOG_ERROR(BSL_LS_SOC_INIT,
                     (BSL_META_U(unit,
                                 "soc_dnxf_chip_type_set: driver device %04x unexpected\n"),
                                 dev_id));
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("failed to find device id")));
    }

exit:  
    SOCDNX_FUNC_RETURN;
}

int
soc_dnxf_control_init(int unit)
{
    int rv;
    uint16 dev_id;
    uint8 rev_id;
    soc_dfe_control_t    *dnxf = NULL;
    SOCDNX_INIT_FUNC_DEFS;

    /*dnxf info config*/
    /*prepare config info for the next init sequnace*/
    dnxf = SOC_DFE_CONTROL(unit);
    if (dnxf == NULL) {
      dnxf = (soc_dfe_control_t *) sal_alloc(sizeof(soc_dfe_control_t),
                                            "soc_dfe_control");
      if (dnxf == NULL) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate soc_dnxf_control")));
      }
      sal_memset(dnxf, 0, sizeof (soc_dfe_control_t));

      SOC_CONTROL(unit)->drv = dnxf;
   }

    soc_cm_get_id(unit, &dev_id, &rev_id);
    rv = soc_dnxf_info_config(unit, dev_id);
    SOCDNX_IF_ERR_CONT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dnxf_attach(int unit)
{
    soc_control_t        *soc;
    soc_persist_t        *sop;
    soc_info_t           *si; 
    uint16               dev_id;
    uint8                rev_id;
    int                  rc = SOC_E_NONE, mem;
    int                  cmc;
    SOCDNX_INIT_FUNC_DEFS;

    /* Allocate soc_control and soc_persist if not already. */
    soc = SOC_CONTROL(unit);
    if (soc == NULL) {
        soc = sal_alloc(sizeof (soc_control_t), "soc_control");
        if (soc == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate soc_control")));
        }
        sal_memset(soc, 0, sizeof (soc_control_t));
        SOC_CONTROL(unit) = soc;
    } else {
        if (soc->soc_flags & SOC_F_ATTACHED) {
            SOC_EXIT;
        }
    }

    soc->soc_link_pause = 0;

    SOC_PCI_CMCS_NUM(unit) = soc_property_uc_get(unit, 0, spn_PCI_CMCS_NUM, 1);

    /* Setup DMA structures when a device is attached */
    SOCDNX_IF_ERR_EXIT(soc_dma_attach(unit, 1 /* Reset */));

    /* Init cmic_pcie_userif_purge_ctrl */
    SOCDNX_IF_ERR_EXIT(soc_dcmn_cmic_pcie_userif_purge_ctrl_init(unit));

    /*
     * Create mutexes and semaphores.
     */

    if ((soc->miimMutex = sal_mutex_create("MIIM")) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate MIIM lock")));
    }
        
    if ((soc->miimIntr = sal_sem_create("MIIM interrupt", sal_sem_BINARY, 0)) == NULL)  {
         SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate MIIM semaphore")));
    }

    if (_bcm_lock[unit] == NULL) {
        if ((_bcm_lock[unit] = sal_mutex_create("bcm_dfe_config_lock")) == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate _bcm_lock")));
        }
    }

    if (_bcm_lock[unit] == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate DFE lock")));
    }

    if ((soc->socControlMutex = sal_mutex_create("SOC_CONTROL")) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate socControlMutex")));
    }

    soc->counterMutex = sal_mutex_create("Counter");
    if (soc->counterMutex == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate counterMutex")));
    }

    soc->schanMutex = sal_mutex_create("SCHAN");
    if (soc->schanMutex == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate schanMutex")));
    }

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++) {
        if ((soc->schanIntr[cmc] =
             sal_sem_create("SCHAN interrupt", sal_sem_BINARY, 0)) == NULL) {
             SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate schanSem")));
        }
    }
    
    SOC_PERSIST(unit) = sal_alloc(sizeof (soc_persist_t), "soc_persist");
    if (SOC_PERSIST(unit) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate soc_persist")));
    }
    sal_memset(SOC_PERSIST(unit), 0, sizeof (soc_persist_t));
    sop = SOC_PERSIST(unit);
    sop->version = 1;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    SOCDNX_IF_ERR_EXIT(soc_dnxf_chip_type_set(unit, dev_id));

    /* Instantiate the driver -- Verify chip revision matches driver
     * compilation revision.
     */
    soc->chip_driver = soc_dnxf_chip_driver_find(unit, dev_id, rev_id);
    if (soc->chip_driver == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("unit has no driver (device 0x%04x rev 0x%02x)"),dev_id, rev_id));
    }

    /*feature init*/
    soc_feature_init(unit);

    si  = &SOC_INFO(unit);
    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];

    /*should be at the attach function to enable register access without chip init*/
    rc = soc_dnxf_control_init(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    /*Required In order to read soc properties*/
    soc->soc_flags |= SOC_F_ATTACHED;

    /*
     * DMA
     */

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
                SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate memState lock")));
            }
    
            /* Set cache copy pointers to NULL */
            sal_memset(soc->memState[mem].cache,
                       0,
                       sizeof (soc->memState[mem].cache));
        } else {
            sop->memState[mem].index_max = -1;
        }
    }

    if ((soc->schan_wb_mutex = sal_mutex_create("SchanWB")) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate SchanWB")));
    }

    /* Initialize SCHAN */
    rc = soc_schan_init(unit);
    if (rc != SOC_E_NONE) {
       SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("failed to Initialize SCHAN")));
    }

exit:  
    if(SOCDNX_FUNC_ERROR) {
       LOG_ERROR(BSL_LS_SOC_INIT,
                 (BSL_META_U(unit,
                             "soc_dnxf_attach: unit %d failed (%s)\n"), 
                             unit, soc_errmsg(rc)));
        soc_dnxf_detach(unit);
    }

    SOCDNX_FUNC_RETURN;
}
