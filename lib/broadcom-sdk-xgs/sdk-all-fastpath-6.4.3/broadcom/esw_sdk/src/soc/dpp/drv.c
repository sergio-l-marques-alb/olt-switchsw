/*
 * $Id: drv.c,v 1.594 Broadcom SDK $
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
 */
#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

#include <shared/bsl.h>
#include <shared/swstate/sw_state.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>
#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/ARAD/arad_drv.h>
#include <soc/dpp/ARAD/arad_init.h>
#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/JER/jer_drv.h>
#endif /* BCM_JERICHO_SUPPORT */
#endif /* BCM_ARAD_SUPPORT */
#ifdef BCM_PETRAB_SUPPORT
#include <soc/dpp/Petra/petra_drv.h>
#include <soc/dpp/Petra/petra_api_ofp_rates.h>
#endif /* BCM_PETRAB_SUPPORT */
#include <soc/dpp/SAND/Management/sand_device_management.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_module_management.h>
#include <soc/mcm/driver.h>
#include <bcm_int/dpp/error.h>

soc_driver_t *
soc_dpp_chip_driver_find(uint16 pci_dev_id, uint8 pci_rev_id)
{
   /* int                 i;*/
   /* soc_driver_t        *d;*/
    uint16              driver_dev_id;
    uint8               driver_rev_id;

    if (soc_cm_get_id_driver(pci_dev_id, pci_rev_id,
                             &driver_dev_id, &driver_rev_id) < 0) {
        return NULL;
    }

     switch(driver_dev_id)
     {
#if defined(BCM_88640_A0)
     case PETRAB_DEVICE_ID:
           return &soc_driver_bcm88640_a0;
#endif
#if defined(BCM_88650_A0) || defined(BCM_88650_B0) || defined(BCM_88660_A0) || defined(BCM_88202_A0) 
     case BCM88650_DEVICE_ID:
     case BCM88350_DEVICE_ID:
     case BCM88351_DEVICE_ID:
     case BCM88450_DEVICE_ID:
     case BCM88451_DEVICE_ID:
     case BCM88550_DEVICE_ID:
     case BCM88551_DEVICE_ID:
     case BCM88552_DEVICE_ID:
     case BCM88651_DEVICE_ID:
     case BCM88654_DEVICE_ID:
     
#if defined(BCM_88650_A0)
           if (pci_rev_id == BCM88650_A0_REV_ID)            
           {
               return &soc_driver_bcm88650_a0;               
           } 
#endif
#if defined(BCM_88650_B0)
           if ((pci_rev_id == BCM88650_B0_REV_ID) || (pci_rev_id == BCM88650_B1_REV_ID))
           {
               return &soc_driver_bcm88650_b0;
           }
#endif
           break;

#if defined(BCM_88650_A0)
#endif

/* JERICHO-2-P3 */
#if defined(BCM_88850_P3)
     case BCM88850_DEVICE_ID:
     if (pci_rev_id == BCM88850_P3_REV_ID)
     {
           return &soc_driver_bcm88850_p3;
     }
     break;
#endif



#if defined(BCM_88660_A0)
     case BCM88660_DEVICE_ID:
     case BCM88360_DEVICE_ID:
     case BCM88361_DEVICE_ID:
     case BCM88363_DEVICE_ID:
     case BCM88460_DEVICE_ID:
     case BCM88461_DEVICE_ID:
     case BCM88560_DEVICE_ID:
     case BCM88561_DEVICE_ID:
     case BCM88562_DEVICE_ID:
     case BCM88661_DEVICE_ID:
     case BCM88664_DEVICE_ID:
           /*if (pci_rev_id == BCM88660_A0_REV_ID)*/ {
               return &soc_driver_bcm88660_a0;
           }
           break;
#endif


#if defined(BCM_88202_A0)
     case BCM88202_DEVICE_ID:
           return &soc_driver_bcm88202_a0;
           break;
#endif

     case ACP_PCI_DEVICE_ID:
             return &soc_driver_acp;
#endif
#if defined(BCM_88675)
    case BCM88675_DEVICE_ID:
        return &soc_driver_bcm88675_a0;
#endif
#if defined(BCM_88375)
    case BCM88375_DEVICE_ID:
        return &soc_driver_bcm88375_a0;
#endif
       default:
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META("soc_chip_driver_find: driver in devid table "
                                "not in soc_base_driver_table\n")));
           break;         
     }



    return NULL;
}

STATIC int
soc_dpp_info_config_ports(int unit)
{
    soc_info_t          *si;
    soc_error_t         rv;
    int                 i;

    SOCDNX_INIT_FUNC_DEFS;

    si  = &SOC_INFO(unit);

    si->fe.min          = si->fe.max          = -1;
    si->il.min          = si->il.max          = -1;
    si->ge.min          = si->ge.max          = -1;
    si->xe.min          = si->xe.max          = -1;
    si->ce.min          = si->ce.max          = -1;
    si->xl.min          = si->xl.max          = -1;
    si->hg.min          = si->hg.max          = -1;
    si->hg_subport.min  = si->hg_subport.max  = -1;
    si->hl.min          = si->hl.max          = -1;
    si->st.min          = si->st.max          = -1;
    si->tdm.min         = si->tdm.max         = -1;
    si->gx.min          = si->gx.max          = -1;
    si->xg.min          = si->xg.max          = -1;
    si->spi.min         = si->spi.max         = -1;
    si->spi_subport.min = si->spi_subport.max = -1;
    si->sci.min         = si->sci.max         = -1;
    si->sfi.min         = si->sfi.max         = -1;
    si->port.min        = si->port.max        = -1;
    si->ether.min       = si->ether.max       = -1;
    si->rcy.min         = si->rcy.max         = -1;
    si->all.min         = si->all.max         = -1;
   
    si->fe.num          = 0;
    si->il.num          = 0;
    si->ge.num          = 0;
    si->xe.num          = 0;
    si->ce.num          = 0;
    si->xl.num          = 0;
    si->hg.num          = 0;
    si->hg_subport.num  = 0;
    si->hl.num          = 0;
    si->st.num          = 0;
    si->tdm.num         = 0;
    si->gx.num          = 0;
    si->xg.num          = 0;
    si->spi.num         = 0;
    si->spi_subport.num = 0;
    si->sci.num         = 0;
    si->sfi.num         = 0;
    si->port.num        = 0;
    si->ether.num       = 0;
    si->rcy.num         = 0;
    si->all.num         = 0;

    SOC_PBMP_CLEAR(si->fe.bitmap);
    SOC_PBMP_CLEAR(si->il.bitmap);
    SOC_PBMP_CLEAR(si->ge.bitmap);
    SOC_PBMP_CLEAR(si->xe.bitmap);
    SOC_PBMP_CLEAR(si->ce.bitmap);
    SOC_PBMP_CLEAR(si->xl.bitmap);
    SOC_PBMP_CLEAR(si->hg.bitmap);
    SOC_PBMP_CLEAR(si->hg_subport.bitmap);
    SOC_PBMP_CLEAR(si->hl.bitmap);
    SOC_PBMP_CLEAR(si->st.bitmap);
    SOC_PBMP_CLEAR(si->tdm.bitmap);
    SOC_PBMP_CLEAR(si->gx.bitmap);
    SOC_PBMP_CLEAR(si->spi.bitmap);
    SOC_PBMP_CLEAR(si->spi_subport.bitmap);
    SOC_PBMP_CLEAR(si->sci.bitmap);
    SOC_PBMP_CLEAR(si->sfi.bitmap);
    SOC_PBMP_CLEAR(si->port.bitmap);
    SOC_PBMP_CLEAR(si->ether.bitmap);
    SOC_PBMP_CLEAR(si->rcy.bitmap);
    SOC_PBMP_CLEAR(si->all.bitmap);

    SOC_PBMP_CLEAR(si->fe.disabled_bitmap);
    SOC_PBMP_CLEAR(si->il.disabled_bitmap);
    SOC_PBMP_CLEAR(si->ge.disabled_bitmap);
    SOC_PBMP_CLEAR(si->xe.disabled_bitmap);
    SOC_PBMP_CLEAR(si->ce.disabled_bitmap);
    SOC_PBMP_CLEAR(si->xl.disabled_bitmap);
    SOC_PBMP_CLEAR(si->hg.disabled_bitmap);
    SOC_PBMP_CLEAR(si->hg_subport.disabled_bitmap);
    SOC_PBMP_CLEAR(si->hl.disabled_bitmap);
    SOC_PBMP_CLEAR(si->st.disabled_bitmap);
    SOC_PBMP_CLEAR(si->tdm.disabled_bitmap);
    SOC_PBMP_CLEAR(si->gx.disabled_bitmap);
    SOC_PBMP_CLEAR(si->spi.disabled_bitmap);
    SOC_PBMP_CLEAR(si->spi_subport.disabled_bitmap);
    SOC_PBMP_CLEAR(si->sci.disabled_bitmap);
    SOC_PBMP_CLEAR(si->sfi.disabled_bitmap);
    SOC_PBMP_CLEAR(si->port.disabled_bitmap);
    SOC_PBMP_CLEAR(si->ether.disabled_bitmap);
    SOC_PBMP_CLEAR(si->rcy.disabled_bitmap);
    SOC_PBMP_CLEAR(si->all.disabled_bitmap);

    for( i = 0 ; i < SOC_MAX_NUM_PORTS ; i++){
        si->fe.port[i] = 0;
        si->il.port[i] = 0;
        si->ge.port[i] = 0;
        si->xe.port[i] = 0;
        si->ce.port[i] = 0;
        si->xl.port[i] = 0;
        si->hg.port[i] = 0;
        si->hg_subport.port[i] = 0;
        si->hl.port[i] = 0;
        si->st.port[i] = 0;
        si->tdm.port[i] = 0;
        si->gx.port[i] = 0;
        si->spi.port[i] = 0;
        si->spi_subport.port[i] = 0;
        si->sci.port[i] = 0;
        si->sfi.port[i] = 0;
        si->port.port[i] = 0;
        si->ether.port[i] = 0;
        si->rcy.port[i] = 0;
        si->all.port[i] = 0;
    }

    si->port_num = 0;
    si->nof_pmqs = 6;

    SOC_PBMP_CLEAR(si->hg2_pbm);
    SOC_PBMP_CLEAR(si->cmic_bitmap);

    rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_drv_info_config_device_ports,(unit));
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;

}

int
soc_dpp_info_config_blocks(int unit)
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
            case SOC_BLK_CFC:
                si->cfc_block = blk;
                break;
            case SOC_BLK_OCB:
                si->ocb_block = blk;
                break;
            case SOC_BLK_CRPS:
                si->crps_block = blk;
                break;
            case SOC_BLK_ECI:
                si->eci_block = blk;
                break;
            case SOC_BLK_EGQ:
                if (blknum >= SOC_MAX_NUM_EGQ_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many EGQ blocks")));
                }
                si->egq_blocks[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_CMIC:
                si->cmic_block = blk;
                break;
            case SOC_BLK_IPROC:
                si->iproc_block = blk;
                break;
            case SOC_BLK_FCR:
                si->fcr_block = blk;
                break;
            case SOC_BLK_FCT:
                si->fct_block = blk;
                break;
            case SOC_BLK_MMU:
                si->mmu_block = blk;
                break;
            case SOC_BLK_FDR:
                si->fdr_block = blk;
                break;
            case SOC_BLK_FDA:
                si->fda_block = blk;
                break;
            case SOC_BLK_FDT:
                si->fdt_block = blk;
                break;
            case SOC_BLK_MESH_TOPOLOGY:
                si->mesh_topology_block = blk;
                break;
            case SOC_BLK_IDR:
                si->idr_block = blk;
                break;
            case SOC_BLK_IHB:
                if (blknum >= SOC_MAX_NUM_IHB_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many IHB blocks")));
                }
                si->ihb_blocks[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_IHP:
                if (blknum >= SOC_MAX_NUM_IHP_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many IHP blocks")));
                }
                si->ihp_blocks[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_IPS:
                if (blknum >= SOC_MAX_NUM_IPS_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many IPS blocks")));
                }
                si->ips_blocks[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_IPT:
                si->ipt_block = blk;
                break;
            case SOC_BLK_IQM:
                if (blknum >= SOC_MAX_NUM_IQM_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many IQM blocks")));
                }
                si->iqm_blocks[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_IRE:
                si->ire_block = blk;
                break;
            case SOC_BLK_IRR:
                si->irr_block = blk;
                break;
            case SOC_BLK_FMAC:
                if (blknum >= SOC_MAX_NUM_FMAC_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many FMAC blocks")));
                }
                si->fmac_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_OTPC:
                si->otpc_block = blk;
                break;
            case SOC_BLK_XLP:
                if (blknum >= SOC_MAX_NUM_XLP_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many XLP blocks")));
                }
                si->xlp_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK; /*will be override if port is defined*/
                break;
            case SOC_BLK_CLP:
                if (blknum >= SOC_MAX_NUM_CLP_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many CLP blocks")));
                }
                si->clp_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK; /*will be override if port is defined*/
                break;
            case SOC_BLK_GPORT:
                if (blknum >= SOC_MAX_NUM_GPORT_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many GPORT blocks")));
                }
                si->gport_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK; /*will be override if port is defined*/
                break;
            case SOC_BLK_PMQ:
                if (blknum >= SOC_MAX_NUM_PMQ_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many PMQ blocks")));
                }
                si->pmq_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK; /*will be override if port is defined*/
                break;
            case SOC_BLK_NBI:
                si->nbi_block = blk;
                break;
            case SOC_BLK_CGM:
                if (blknum >= SOC_MAX_NUM_CGM_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many CGM blocks")));
                }
                si->cgm_blocks[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_OAMP:
                si->oamp_block = blk;
                break;
            case SOC_BLK_OLP:
                si->olp_block = blk;
                break;
            case SOC_BLK_FSRD:
                if (blknum >= SOC_MAX_NUM_FSRD_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many FSRD blocks")));
                }
                si->fsrd_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_RTP:
                si->rtp_block = blk;
                break;
            case SOC_BLK_SCH:
                if (blknum >= SOC_MAX_NUM_SCH_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many SCH blocks")));
                }
                si->sch_blocks[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_EPNI:
                if (blknum >= SOC_MAX_NUM_EPNI_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many EPNI blocks")));
                }
                si->epni_blocks[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_MRPS:
                if (blknum >= SOC_MAX_NUM_MRPS_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many MRPS blocks")));
                }
                si->mrps_blocks[blknum] = blk;
                break;
            case SOC_BLK_EDB:
                si->edb_block = blk;
                break;
            case SOC_BLK_ILKN_PMH:
                si->ilkn_pmh_block = blk;
                break;
            case SOC_BLK_IPST:
                si->ipst_block = blk;
                break;
            case SOC_BLK_ILKN_PML:
                si->ilkn_pml_block = blk;
                break;
            case SOC_BLK_IQMT:
                si->iqmt_block = blk;
                break;
            case SOC_BLK_PPDB_A:
                si->ppdb_a_block = blk;
                break;
            case SOC_BLK_PPDB_B:
                si->ppdb_b_block = blk;
                break;
            case SOC_BLK_NBIL:
                if (blknum >= SOC_MAX_NUM_NBIL_BLKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_info_config_blocks: soc_dpp_info_config: too many NBIL blocks")));
                }
                si->nbil_block[blknum] = blk;
                si->block_port[blk] = blknum | SOC_REG_ADDR_INSTANCE_MASK;
                break;
            case SOC_BLK_NBIH:
                si->nbih_block = blk;
                break;
            case SOC_BLK_DRCA:
                si->drca_block = blk;
                break;
            case SOC_BLK_DRCB:
                si->drcb_block = blk;
                break;
            case SOC_BLK_DRCC:
                si->drcc_block = blk;
                break;
            case SOC_BLK_DRCD:
                si->drcd_block = blk;
                break;
            case SOC_BLK_DRCE:
                si->drce_block = blk;
                break;
            case SOC_BLK_DRCF:
                si->drcf_block = blk;
                break;
            case SOC_BLK_DRCG:
                si->drcg_block = blk;
                break;
            case SOC_BLK_DRCH:
                si->drch_block = blk;
                break;
            case SOC_BLK_DRCBROADCAST:
                si->drcbroadcast_block = blk;
                break;
            case SOC_BLK_BRDC_FSRD:
                si->brdc_fsrd_block = blk;
                break;
            case SOC_BLK_BRDC_FMAC:
                si->brdc_fmac_block = blk;
                break;
            case SOC_BLK_BRDC_CGM:
                si->brdc_cgm_block = blk;
                si->broadcast_blocks[blk] = si->cgm_blocks;
                si->broadcast_blocks_size[blk] = SOC_DPP_DEFS_GET(unit, nof_cores);
                break;
            case SOC_BLK_BRDC_EGQ:
                si->brdc_egq_block = blk;
                si->broadcast_blocks[blk] = si->egq_blocks;
                si->broadcast_blocks_size[blk] = SOC_DPP_DEFS_GET(unit, nof_cores);
                break;
            case SOC_BLK_BRDC_EPNI:
                si->brdc_epni_block = blk;
                si->broadcast_blocks[blk] = si->epni_blocks;
                si->broadcast_blocks_size[blk] = SOC_DPP_DEFS_GET(unit, nof_cores);
                break;
            case SOC_BLK_BRDC_IHB:
                si->brdc_ihb_block = blk;
                si->broadcast_blocks[blk] = si->ihb_blocks;
                si->broadcast_blocks_size[blk] = SOC_DPP_DEFS_GET(unit, nof_cores);
                break;
            case SOC_BLK_BRDC_IHP:
                si->brdc_ihp_block = blk;
                si->broadcast_blocks[blk] = si->ihp_blocks;
                si->broadcast_blocks_size[blk] = SOC_DPP_DEFS_GET(unit, nof_cores);
                break;
            case SOC_BLK_BRDC_IPS:
                si->brdc_ips_block = blk;
                si->broadcast_blocks[blk] = si->ips_blocks;
                si->broadcast_blocks_size[blk] = SOC_DPP_DEFS_GET(unit, nof_cores);
                break;
            case SOC_BLK_BRDC_IQM:
                si->brdc_iqm_block = blk;
                si->broadcast_blocks[blk] = si->iqm_blocks;
                si->broadcast_blocks_size[blk] = SOC_DPP_DEFS_GET(unit, nof_cores);
                break;
            case SOC_BLK_BRDC_SCH:
                si->brdc_sch_block = blk;
                si->broadcast_blocks[blk] = si->sch_blocks;
                si->broadcast_blocks_size[blk] = SOC_DPP_DEFS_GET(unit, nof_cores);
                break;
            case SOC_BLK_CFG:
                si->p3_cfg_block = blk;
                break;
            case SOC_BLK_EM:
                si->p3_em_block = blk;
                break;
            case SOC_BLK_TABLE:
                si->p3_table_block = blk;
                break;
            case SOC_BLK_TCAM:
                si->p3_tcam_block = blk;
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


/* 
 * Function: 
 *      soc_dpp_device_specific_info_config_direct
 * Description: 
 *      primitive dispatcher for soc properties which are found only in Arad or only in Jericho
 *      and are not dependent on common soc properties
 * Parameters: 
 *      int unit - unit number
 *  
 * Return Value: SOC_E_XXX
 */
int soc_dpp_device_specific_info_config_direct(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_arad_specific_info_config_direct(unit)); 
    } 
#ifdef BCM_JERICHO_SUPPORT
    else if(SOC_IS_JERICHO(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_specific_info_config_direct(unit));
    }
#endif /* BCM_JERICHO_SUPPORT */

exit: 
    SOCDNX_FUNC_RETURN;
}

/* 
 * Function: 
 *      soc_dpp_device_specific_info_config_derived
 * Description: 
 *      primitive dispatcher for soc properties which are found only in Arad or only in Jericho
 *      and are dependent on common soc properties
 * Parameters: 
 *      int unit - unit number
 *  
 * Return Value: SOC_E_XXX
 */
int soc_dpp_device_specific_info_config_derived(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_arad_specific_info_config_derived(unit)); 
    } 
#ifdef BCM_JERICHO_SUPPORT
    else if(SOC_IS_JERICHO(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_specific_info_config_derived(unit));
    }
#endif /* BCM_JERICHO_SUPPORT */

exit: 
    SOCDNX_FUNC_RETURN;
}


int
soc_dpp_info_config(int unit)
{
    soc_info_t          *si;
    soc_control_t       *soc;
    int                 mem, blk;

    SOCDNX_INIT_FUNC_DEFS;

    soc = SOC_CONTROL(unit);

    si  = &SOC_INFO(unit);
    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];

    SOCDNX_IF_ERR_EXIT(soc_dpp_info_config_ports(unit));

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


int
soc_dpp_str_prop_parse_flow_control_type(int unit, soc_port_t port, SOC_TMC_PORTS_FC_TYPE *p_val)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_FLOW_CONTROL_TYPE;
    propval = soc_property_port_get_str(unit, port, propkey);    
    
    if ((!propval) || (sal_strcmp(propval, "NONE") == 0)) {
        *p_val = SOC_TMC_PORTS_FC_TYPE_NONE;
    } else if (sal_strcmp(propval, "LL") == 0) {
        *p_val = SOC_TMC_PORTS_FC_TYPE_LL;
    } else if (sal_strcmp(propval, "CB2") == 0) {
        *p_val = SOC_TMC_PORTS_FC_TYPE_CB2;
    } else if (sal_strcmp(propval, "CB8") == 0) {
        *p_val = SOC_TMC_PORTS_FC_TYPE_CB8;
    } else  {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value")));
    }

exit:
    SOCDNX_FUNC_RETURN
}

int
soc_dpp_str_prop_ext_ram_type(int unit, SOC_TMC_DRAM_TYPE *soc_petra_dram_type)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_RAM_TYPE;
    propval = soc_property_get_str(unit, propkey);    

    if ((!propval) || (sal_strcmp(propval, "NONE") == 0)) {
        *soc_petra_dram_type = SOC_TMC_DRAM_NOF_TYPES;
    } else if (sal_strcmp(propval, "GDDR3") == 0) {
        *soc_petra_dram_type = SOC_TMC_DRAM_TYPE_GDDR3;
    } else if (sal_strcmp(propval, "DDR2") == 0) {
        *soc_petra_dram_type = SOC_TMC_DRAM_TYPE_DDR2;
    } else if (sal_strcmp(propval, "DDR3") == 0) {
        *soc_petra_dram_type = SOC_TMC_DRAM_TYPE_DDR3;        
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value")));
    }

exit:
    SOCDNX_FUNC_RETURN
}

int
soc_dpp_str_prop_fabric_connect_mode_get(int unit, SOC_TMC_FABRIC_CONNECT_MODE *fabric_connect_mode)
{
    int rv = SOC_E_NONE;
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_FABRIC_CONNECT_MODE;
    propval = soc_property_get_str(unit, propkey);   
    
    if (propval) {
        if (sal_strcmp(propval, "FE") == 0) {
            *fabric_connect_mode = SOC_TMC_FABRIC_CONNECT_MODE_FE;
        } else if (sal_strcmp(propval, "BACK2BACK") == 0) {
            *fabric_connect_mode = SOC_TMC_FABRIC_CONNECT_MODE_BACK2BACK;
        } else if (sal_strcmp(propval, "MESH") == 0) {
            *fabric_connect_mode = SOC_TMC_FABRIC_CONNECT_MODE_MESH;
        } else if (sal_strcmp(propval, "MULT_STAGE_FE") == 0) {
            *fabric_connect_mode = SOC_TMC_FABRIC_CONNECT_MODE_MULT_STAGE_FE;
        } else if (sal_strcmp(propval, "SINGLE_FAP") == 0) {
            *fabric_connect_mode = SOC_TMC_FABRIC_CONNECT_MODE_SINGLE_FAP;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s must be defined"), propkey));
    }

    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_drv_validate_fabric_mode, (unit, fabric_connect_mode));
    SOCDNX_IF_ERR_EXIT(rv);
        
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_str_prop_fabric_ftmh_outlif_extension_get(int unit, SOC_TMC_PORTS_FTMH_EXT_OUTLIF *fabric_ftmh_outlif_extension)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_FABRIC_FTMH_OUTLIF_EXTENSION;
    propval = soc_property_get_str(unit, propkey);    

    if (propval) {
        if (sal_strcmp(propval, "ALWAYS") == 0) {
            *fabric_ftmh_outlif_extension = SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ALWAYS;
        } else if (sal_strcmp(propval, "IF_MC") == 0) {
            *fabric_ftmh_outlif_extension = SOC_TMC_PORTS_FTMH_EXT_OUTLIF_IF_MC;
        } else if (sal_strcmp(propval, "NEVER") == 0) {
            *fabric_ftmh_outlif_extension = SOC_TMC_PORTS_FTMH_EXT_OUTLIF_NEVER;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s must be defined"), propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_dpp_str_prop_parse_tm_port_header_type_one_dir(int unit, soc_port_t port, const char *propval, SOC_TMC_PORT_HEADER_TYPE *p_val, uint32* is_hg_header)
{
    int rv = SOC_E_NONE;
    uint8 pp_enable;
    uint32 is_olp=0, is_oamp=0;
    SOC_TMC_MGMT_TDM_MODE tdm_mode;

    SOCDNX_INIT_FUNC_DEFS;

    if (propval) {
        if (sal_strcmp(propval, "ETH") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_ETH;
        } else if (sal_strcmp(propval, "RAW") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_RAW;
        } else if (sal_strcmp(propval, "TM") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_TM;
        } else if (sal_strcmp(propval, "PROG") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_PROG;
        } else if (sal_strcmp(propval, "CPU") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_CPU;
        } else if (sal_strcmp(propval, "STACKING") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_STACKING;
        } else if (sal_strcmp(propval, "TDM") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_TDM;
        } else if (sal_strcmp(propval, "TDM_RAW") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_TDM_RAW;
        } else if (sal_strcmp(propval, "INJECTED_2_PP") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP;
        } else if (sal_strcmp(propval, "INJECTED_2") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_INJECTED_2;
        } else if (sal_strcmp(propval, "INJECTED_PP") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_INJECTED_PP;
        } else if (sal_strcmp(propval, "INJECTED") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_INJECTED;
        } else if (sal_strcmp(propval, "XGS_HQoS") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_XGS_HQoS;
            if(NULL != is_hg_header) {
                (*is_hg_header) = 1;
            }
        } else if (sal_strcmp(propval, "XGS_DiffServ") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_XGS_DiffServ;
            if(NULL != is_hg_header) {
                (*is_hg_header) = 1;
            }
        } else if (sal_strcmp(propval, "XGS_MAC_EXT") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT;
            if(NULL != is_hg_header) {
                (*is_hg_header) = 1;
            }
        } else if (sal_strcmp(propval, "UDH_ETH") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_UDH_ETH;
        } else if (sal_strcmp(propval, "MPLS_RAW") == 0) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW;
            
        } else  {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, spn_TM_PORT_HEADER_TYPE));
        }
    } else {
    /* If no property was found:
       - Set OLP to TM
       - set other ports according to pp enable
    */
        rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_drv_is_olp,(unit, port, &is_olp));
        SOCDNX_IF_ERR_EXIT(rv);
        rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_drv_is_oamp,(unit, port, &is_oamp));
        SOCDNX_IF_ERR_EXIT(rv);

        if (is_olp) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_TM;
        } else if (is_oamp) {
            *p_val = SOC_TMC_PORT_HEADER_TYPE_INJECTED_2;
        } else {
            rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_drv_prop_fap_device_mode_get, (unit, &pp_enable, &tdm_mode));
            SOCDNX_IF_ERR_EXIT(rv);
            
            if (pp_enable) {
                *p_val = SOC_TMC_PORT_HEADER_TYPE_ETH;
            } else {
                *p_val = SOC_TMC_PORT_HEADER_TYPE_TM;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_str_prop_parse_tm_port_header_type(int unit, soc_port_t port, SOC_TMC_PORT_HEADER_TYPE *p_val_incoming, SOC_TMC_PORT_HEADER_TYPE *p_val_outgoing, uint32* is_hg_header)
{
    int rv = SOC_E_NONE;
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    if(NULL != is_hg_header) {
        (*is_hg_header) = 0;
    }

    propkey = spn_TM_PORT_HEADER_TYPE;

    propval = soc_property_port_suffix_num_get_str(unit, port, 0, propkey, "in");
    rv = soc_dpp_str_prop_parse_tm_port_header_type_one_dir(unit,port,propval,p_val_incoming,is_hg_header);
    SOCDNX_IF_ERR_EXIT(rv);

    propval = soc_property_port_suffix_num_get_str(unit, port, 0, propkey, "out");
    rv = soc_dpp_str_prop_parse_tm_port_header_type_one_dir(unit,port,propval,p_val_outgoing,is_hg_header);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_str_prop_parse_logical_port_mim(int unit, SOC_PPD_LIF_ID *p_val_incoming, SOC_PPD_AC_ID *p_val_outgoing)
{
    char *propkey;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_LOGICAL_PORT_MIM;

    *p_val_incoming = soc_property_suffix_num_get(unit, 0, propkey, "in", 2);
    if((*p_val_incoming) > 0xffff) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for %s"), *p_val_incoming, spn_LOGICAL_PORT_MIM));
    }

    *p_val_outgoing = soc_property_suffix_num_get(unit, 0, propkey, "out", (SOC_IS_PETRAB(unit) ? 3 : 4096));
    if((*p_val_outgoing) > 0xffff) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%d\") for %s"), *p_val_outgoing, spn_LOGICAL_PORT_MIM));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_str_prop_parse_tm_port_otmh_extensions_en(int unit, soc_port_t port, SOC_TMC_PORTS_OTMH_EXTENSIONS_EN *p_val)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_TM_PORT_OTMH_DEST_EXT_ENABLE;
    propval = soc_property_port_get_str(unit, port, propkey);
    if (propval) {
        if ((sal_strcmp(propval, "DISABLE") == 0) ||
            (sal_strcmp(propval, "DIS")     == 0) ||
            (sal_strcmp(propval, "FALSE")   == 0) ||
            (sal_strcmp(propval, "0")       == 0)) {
                p_val->dest_ext_en = 0;
        } else if ((sal_strcmp(propval, "ENABLE") == 0) ||
                   (sal_strcmp(propval, "EN")     == 0) ||
                   (sal_strcmp(propval, "TRUE")   == 0) ||
                   (sal_strcmp(propval, "1")      == 0)) {
                p_val->dest_ext_en = 1;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    }
    else {
          p_val->dest_ext_en = 0;
    }

    propkey = spn_TM_PORT_OTMH_SRC_EXT_ENABLE;
    propval = soc_property_port_get_str(unit, port, propkey);
    if (propval) {
        if ((sal_strcmp(propval, "DISABLE") == 0) ||
            (sal_strcmp(propval, "DIS")     == 0) ||
            (sal_strcmp(propval, "FALSE")   == 0) ||
            (sal_strcmp(propval, "0")       == 0)) {
            p_val->src_ext_en = 0;
        } else if ((sal_strcmp(propval, "ENABLE") == 0) ||
                   (sal_strcmp(propval, "EN")     == 0) ||
                   (sal_strcmp(propval, "TRUE")   == 0) ||
                   (sal_strcmp(propval, "1")      == 0)) {
            p_val->src_ext_en = 1;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
          p_val->src_ext_en = 0;
    }

    propkey = spn_TM_PORT_OTMH_OUTLIF_EXT_MODE;
    propval = soc_property_port_get_str(unit, port, propkey);
    if (propval) {
        if ((sal_strcmp(propval, "NEVER") == 0)) {
            p_val->outlif_ext_en = SOC_TMC_PORTS_FTMH_EXT_OUTLIF_NEVER;
        } else if ((sal_strcmp(propval, "IF_MC") == 0)) {
            p_val->outlif_ext_en = SOC_TMC_PORTS_FTMH_EXT_OUTLIF_IF_MC;
        } else if ((sal_strcmp(propval, "ALWAYS") == 0)) {
            p_val->outlif_ext_en = SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ALWAYS;
        } else if ((sal_strcmp(propval, "DOUBLE_TAG") == 0)) {
            p_val->outlif_ext_en = SOC_TMC_PORTS_FTMH_EXT_OUTLIF_DOUBLE_TAG;
        } else if ((sal_strcmp(propval, "ENLARGE") == 0)) {
            p_val->outlif_ext_en = SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ENLARGE;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } else {
            p_val->outlif_ext_en = SOC_TMC_PORTS_FTMH_EXT_OUTLIF_NEVER;
    }    
        
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_str_prop_parse_mpls_context(int unit, uint8 *include_inrif, uint8 *include_port)
{
    char *propkey, *propval;

    SOCDNX_INIT_FUNC_DEFS;
        
    propkey = spn_MPLS_CONTEXT;
    propval = soc_property_get_str(unit, propkey);    
    
    *include_inrif = FALSE;
    *include_port = FALSE;

    if (propval) {
        if (sal_strcmp(propval, "global") == 0) {
            *include_inrif = FALSE;
            *include_port = FALSE;
        } else if (sal_strcmp(propval, "port") == 0) {
            *include_inrif = FALSE;
            *include_port = TRUE;
        } else if (sal_strcmp(propval, "interface") == 0) {
            *include_inrif = TRUE;
            *include_port = FALSE;
        } else if (sal_strcmp(propval, "port_and_interface") == 0) {
            *include_inrif = TRUE;
            *include_port = TRUE;
        } else  {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), propval, propkey));
        }
    } 

    /* In case of not found, return the clear value */
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_prop_parse_dram_number_of_columns(int unit, SOC_TMC_DRAM_NUM_COLUMNS *p_val) 
{
    char *propkey;
    uint32 propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_RAM_COLUMNS;
    propval = soc_property_get(unit, propkey, SOC_TMC_NOF_DRAM_NUMS_COLUMNS);    

    switch (propval) {   
        case 256:
            *p_val = SOC_TMC_DRAM_NUM_COLUMNS_256;
            break;
        case 512:
            *p_val = SOC_TMC_DRAM_NUM_COLUMNS_512;
            break;
        case 1024:
            *p_val = SOC_TMC_DRAM_NUM_COLUMNS_1024;
            break;
        case 2048:
            *p_val = SOC_TMC_DRAM_NUM_COLUMNS_2048;
            break;
        case 4096:
            *p_val = SOC_TMC_DRAM_NUM_COLUMNS_4096;
            break;
        case 8192:
            *p_val = SOC_TMC_DRAM_NUM_COLUMNS_8192;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s must be defined"), propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_prop_parse_dram_number_of_banks(int unit, SOC_TMC_DRAM_NUM_BANKS *p_val) 
{
    char *propkey;
    uint32 propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_RAM_BANKS;
    propval = soc_property_get(unit, propkey, SOC_TMC_NOF_DRAM_NUM_BANKS);    
    switch (propval) {   
        case 4:
            *p_val = SOC_TMC_DRAM_NUM_BANKS_4;
            break;
        case 8:
            *p_val = SOC_TMC_DRAM_NUM_BANKS_8;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s must be defined"), propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_prop_parse_dram_burst_size(int unit, SOC_TMC_DRAM_BURST_SIZE *p_val) 
{
    char *propkey;
    uint32 propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_RAM_BURST_SIZE;
    propval = soc_property_get(unit, propkey, SOC_TMC_DRAM_NOF_BURST_SIZES);    
    switch (propval) {   
        case 16:
            *p_val = SOC_TMC_DRAM_BURST_SIZE_16;
            break;
        case 32:
            *p_val = SOC_TMC_DRAM_BURST_SIZE_32;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s must be defined"), propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_prop_parse_dram_ap_bit_pos(int unit, SOC_TMC_DRAM_AP_POSITION *p_val) 
{
    char *propkey;
    uint32 propval;

    SOCDNX_INIT_FUNC_DEFS;

    propkey = spn_EXT_RAM_AP_BIT_POS;
    propval = soc_property_get(unit, propkey, SOC_TMC_NOF_DRAM_AP_POSITIONS);    
    switch (propval) {   
        case 8:
            *p_val = SOC_TMC_DRAM_AP_POSITION_08;
            break;
        case 9:
            *p_val = SOC_TMC_DRAM_AP_POSITION_09;
            break;
        case 10:
            *p_val = SOC_TMC_DRAM_AP_POSITION_10;
            break;
        case 11:
            *p_val = SOC_TMC_DRAM_AP_POSITION_11;
            break;
        case 12:
            *p_val = SOC_TMC_DRAM_AP_POSITION_12;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s must be defined"), propkey));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_dpp_common_deinit(int unit)
{    
    int soc_sand_rv = 0, rv;
    

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_counter_detach(unit);
    SOCDNX_IF_ERR_CONT(rv);

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit,mbcm_dpp_unregister_device,( unit)));
    SOCDNX_SAND_IF_ERR_CONT(soc_sand_rv);

    SOC_DPP_CONTROL(unit)->is_modid_set_called = 0;

    SOCDNX_FUNC_RETURN;
}   

int
soc_dpp_deinit(int unit)
{
    int rv;
    soc_control_t* soc;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("soc_dpp_deinit: invalid unit.\n")));
    }

    soc = SOC_CONTROL(unit);

    soc->soc_flags &= ~SOC_F_INITED;

    rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_drv_device_deinit,(unit));
    SOCDNX_IF_ERR_CONT(rv);

    rv = soc_dpp_common_deinit(unit);
    SOCDNX_IF_ERR_CONT(rv);

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        soc_dpp_wb_engine_deinit(unit);
        rv = soc_sw_state_deinit(unit);
        SOCDNX_IF_ERR_CONT(rv);
    }
#endif /* BCM_ARAD_SUPPORT */

    rv = soc_sw_state_deinit(unit);
    SOCDNX_IF_ERR_CONT(rv);

    rv = soc_dpp_implementation_defines_deinit(unit);
    SOCDNX_IF_ERR_RETURN(rv);
    
exit:
    /*inform that detaching device is done*/
    if (SOC_UNIT_VALID(unit)) {
        SOC_DETACH(unit,0);
    }
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_detach(int unit)
{
    soc_control_t       *soc;
    soc_dpp_config_t *dpp; 
    int mem;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNIT);
    }

    soc = SOC_CONTROL(unit);
    if (soc == NULL) {
        SOC_EXIT;
    }
    
    /* Destroy memory mutex */
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (soc->memState[mem].lock != NULL) {
            sal_mutex_destroy(soc->memState[mem].lock);
            soc->memState[mem].lock = NULL;                    
        }
    } 
     
    SOCDNX_IF_ERR_EXIT(soc_dpp_defines_deinit(unit));
    
    if (SOC_DPP_CONTROL(unit) != NULL) {
        /* Free Arad SOC config */
        dpp = SOC_DPP_CONFIG(unit);
        if (dpp != NULL) {
#ifdef BCM_ARAD_SUPPORT
            if (dpp->arad != NULL) { /* arad_config */
                 sal_free(dpp->arad);
                 dpp->arad = NULL;
            }
            if (dpp->arad_plus != NULL) {
                 sal_free(dpp->arad_plus); /* arad_plus_config */
                 dpp->arad_plus = NULL;
            }
#endif /* BCM_ARAD_SUPPORT */
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

    if(soc->schanIntr != NULL) {    
        sal_sem_destroy(soc->schanIntr);
        soc->schanIntr = NULL;
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
soc_dpp_chip_type_set(int unit, uint16 dev_id)
{
    soc_info_t           *si; 

    SOCDNX_INIT_FUNC_DEFS;

    si  = &SOC_INFO(unit);

    /*
     * Used to implement the SOC_IS_*(unit) macros
     */
    switch (dev_id) {
#ifdef BCM_PETRAB_SUPPORT
    case PETRAB_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_PETRAB;
        SOC_CHIP_STRING(unit) = "petrab";
        break;
#endif /* BCM_PETRAB_SUPPORT */

#ifdef BCM_ARAD_SUPPORT
    case BCM88650_DEVICE_ID:
    case BCM88350_DEVICE_ID:
    case BCM88351_DEVICE_ID:
    case BCM88450_DEVICE_ID:
    case BCM88451_DEVICE_ID:
    case BCM88550_DEVICE_ID:
    case BCM88551_DEVICE_ID:
    case BCM88552_DEVICE_ID:
    case BCM88651_DEVICE_ID:
    case BCM88654_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_ARAD;
        SOC_CHIP_STRING(unit) = "arad";
        break;

    case BCM88660_DEVICE_ID:
    case BCM88360_DEVICE_ID:
    case BCM88361_DEVICE_ID:
    case BCM88363_DEVICE_ID:
    case BCM88460_DEVICE_ID:
    case BCM88461_DEVICE_ID:
    case BCM88560_DEVICE_ID:
    case BCM88561_DEVICE_ID:
    case BCM88562_DEVICE_ID:
    case BCM88661_DEVICE_ID:
    case BCM88664_DEVICE_ID:    
        si->chip_type = SOC_INFO_CHIP_TYPE_ARADPLUS;
        SOC_CHIP_STRING(unit) = "arad_plus";
        break;

    case BCM88202_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_ARDON;
        SOC_CHIP_STRING(unit) = "ardon";
        break;

    case ACP_PCI_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_ACP;
        SOC_CHIP_STRING(unit) = "acp";
        break;
#endif
#ifdef BCM_JERICHO_SUPPORT
    case JERICHO_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_JERICHO;
        SOC_CHIP_STRING(unit) = "jericho";
        break;
    case QMX_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_QMX;
        SOC_CHIP_STRING(unit) = "qmx";
        break;
#endif /* BCM_JERICHO_SUPPORT */
/* JERICHO-2-P3 */
#ifdef BCM_JERICHO_P3_SUPPORT
    case BCM88850_DEVICE_ID:
        si->chip_type = SOC_INFO_CHIP_TYPE_JERICHO_P3;
        SOC_CHIP_STRING(unit) = "jericho_p3";
        break;
#endif
    default:
        si->chip_type = 0;
        SOC_CHIP_STRING(unit) = "???";
        LOG_VERBOSE(BSL_LS_SOC_INIT,
                    (BSL_META_U(unit,
                                "soc_dpp_info_config: driver device %04x unexpected\n"), dev_id));
        break;
    }

    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_dpp_attach
 * Purpose:
 *      Initialize the soc_control_t structure for a device,
 *      allocating all memory and semaphores required.
 * Parameters:
 *      unit - device unit 
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      SOC property can be retrive only after this function.
 */
int
soc_dpp_attach(int unit)
{
    soc_control_t        *soc;
    soc_persist_t        *sop;
    soc_info_t           *si; 
    soc_dpp_control_t    *dpp = NULL;
    uint16               dev_id;
    uint8                rev_id;
    int                  mem;
    int                  rv=0;
    
    SOCDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_SOC_INIT,
                (BSL_META_U(unit,
                            "%s: unit %d\n"), FUNCTION_NAME(), unit));

    /* Allocate soc_control and soc_persist if not already. */
    soc = SOC_CONTROL(unit);
    if (soc != NULL) {  
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("unit %d has alreaddy attached"), unit));
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

    if (SOC_IS_DPP_TYPE(dev_id) == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("unit %d is not DPP type"), unit));
    }

    /* Instantiate the driver -- Verify chip revision matches driver
     * compilation revision.
     */
    soc->chip_driver = soc_dpp_chip_driver_find(dev_id, rev_id);
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

    rv = soc_dpp_chip_type_set(unit, dev_id);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "%s: unit %d soc_dpp_chip_type_set"
                              " failed (%s)\n"), FUNCTION_NAME(), unit, soc_errmsg(rv)));
    }

    /* Finish ACP attach */
    if(SOC_IS_DPP_CP_TYPE(dev_id)) {
        soc->soc_flags |= SOC_F_ATTACHED;
        SOC_EXIT;
    }

    SOCDNX_IF_ERR_EXIT(soc_dpp_defines_init(unit));

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
    
    if ((soc->schanIntr = sal_sem_create("SCHAN interrupt", sal_sem_BINARY, 0)) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate Schan interrupt Sem")));
    }
    
#ifdef BCM_CMICM_SUPPORT  
    soc->fschanMutex = NULL;   
    if ((soc->fschanMutex = sal_mutex_create("FSCHAN")) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate fSchan Lock")));
    }
#endif /* BCM_CMICM_SUPPORT */

#ifdef INCLUDE_MEM_SCAN
    /* do not enable memscan task, otherwise tr 50 & 51 will segmantation fail*/
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

    /* Must call mbcm and mbcm_pp init first to ensure driver properly installed */
    rv = mbcm_dpp_init(unit);
    if (rv != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "soc_dpp_init error in mbcm_dpp_init\n")));
    }
    SOCDNX_IF_ERR_EXIT(rv); 

    rv = mbcm_dpp_pp_init(unit);
    if (rv != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "soc_dpp_init error in mbcm_pp_init\n")));
    }
    SOCDNX_IF_ERR_EXIT(rv); 

    rv = soc_dpp_info_config(unit);    
    SOCDNX_IF_ERR_EXIT(rv);
    
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


    soc->soc_flags |= SOC_F_ATTACHED;

    /* Init Feature list */
    soc_feature_init(unit);

    /* Set schan_op function pointer so soc_schan_op() is usable */
	SOC_CONTROL(unit)->schan_op = soc_find_schan_op(unit);

exit: 
    if(SOCDNX_FUNC_ERROR) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "soc_attach: unit %d failed (%s)"), unit, soc_errmsg(rv)));
        soc_dpp_detach(unit);
    }

    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_dram_prop_get(int unit, char *prop, uint32 *p_val) 
{
  char *propval, *s;
  int val;

  SOCDNX_INIT_FUNC_DEFS;

  propval = soc_property_get_str(unit, prop);

  if (propval) {
    val = sal_ctoi(propval, &s);
    if ((*s == 'c') && (*(s+1) == '\0')) {
        /* Property is XXXXc, so convert it to clocks */
#ifdef BCM_PETRAB_SUPPORT
        if(SOC_IS_PETRAB(unit)) {
            *p_val = SOC_PETRA_DRAM_VAL_IN_CLOCKS(val);
        }
#endif
#ifdef BCM_ARAD_SUPPORT
        if(SOC_IS_ARAD(unit)) {
            *p_val = ARAD_DRAM_VAL_IN_CLOCKS(val);
        }
#endif
    } else if (*s == '\0') {
        /* Property is a simple number */
        *p_val = val;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Property %s is in a unknown format. Should be a number, or a number followed by c (e.g. 1000c)"), prop));
    }
  }

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_dpp_info_config_common_tm(int unit) 
{
    soc_error_t rv;
    int port;
    int fc_direction;

    SOCDNX_INIT_FUNC_DEFS;

    /*
     * Using unit as base address. Access hooks call the CM read/write with this unit id.
     */
    SOC_DPP_CONFIG(unit)->tm.base_address = INT_TO_PTR(unit);

    /* flow control properties - Out-Of-Band */
    for (port = 0; port < SOC_DPP_CONFIG(unit)->tm.max_oob_ports; port++) {
        SOC_DPP_CONFIG(unit)->tm.fc_oob_type[port] = soc_property_port_get(unit, port, spn_FC_OOB_TYPE, SOC_DPP_CONFIG(unit)->tm.fc_oob_type[port]);

        SOC_DPP_CONFIG(unit)->tm.fc_oob_mode[port] = soc_property_port_get(unit, port, spn_FC_OOB_MODE, SOC_DPP_CONFIG(unit)->tm.fc_oob_mode[port]);
        SOC_DPP_CONFIG(unit)->tm.fc_oob_mode[port] &= (SOC_DPP_FC_CAL_MODE_RX_ENABLE | SOC_DPP_FC_CAL_MODE_TX_ENABLE);

        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_RX] = soc_property_port_get(unit, port, spn_FC_OOB_CALENDER_LENGTH, SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_RX]);
        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_TX] = soc_property_port_get(unit, port, spn_FC_OOB_CALENDER_LENGTH, SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_TX]);

        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_RX] = soc_property_suffix_num_get(unit, port, spn_FC_OOB_CALENDER_LENGTH, "rx", SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_RX]);
        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_TX] = soc_property_suffix_num_get(unit, port, spn_FC_OOB_CALENDER_LENGTH, "tx", SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_TX]);

        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_RX] = soc_property_port_get(unit, port, spn_FC_OOB_CALENDER_REP_COUNT, SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_RX]);
        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_TX] = soc_property_port_get(unit, port, spn_FC_OOB_CALENDER_REP_COUNT, SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_TX]);

        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_RX] = soc_property_suffix_num_get(unit, port, spn_FC_OOB_CALENDER_REP_COUNT, "rx", SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_RX]);
        SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_TX] = soc_property_suffix_num_get(unit, port, spn_FC_OOB_CALENDER_REP_COUNT, "tx", SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_TX]);
    }

    /* Flow Control - InBand Mode */
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++)
    {
      SOC_DPP_CONFIG(unit)->tm.fc_inband_mode[port][SOC_TMC_CONNECTION_DIRECTION_RX] = soc_property_suffix_num_get(unit, port, spn_FC_INBAND_MODE, "rx", 1);
      SOC_DPP_CONFIG(unit)->tm.fc_inband_mode[port][SOC_TMC_CONNECTION_DIRECTION_TX] = soc_property_suffix_num_get(unit, port, spn_FC_INBAND_MODE, "tx", 0);
    }

    /* flow control properties - InBand ILKN*/
    for (port = 0; port < SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports; port++) {
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_mode[port] = soc_property_port_get(unit, port, spn_FC_INBAND_INTLKN_MODE, SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_mode[port]);
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_mode[port] &= (SOC_DPP_FC_CAL_MODE_RX_ENABLE | SOC_DPP_FC_CAL_MODE_TX_ENABLE);

        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_RX] = soc_property_port_get(unit, port, spn_FC_INBAND_INTLKN_CALENDER_LENGTH, SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_RX]);
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_TX] = soc_property_port_get(unit, port, spn_FC_INBAND_INTLKN_CALENDER_LENGTH, SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_TX]);

        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_RX] = soc_property_suffix_num_get(unit, port, spn_FC_INBAND_INTLKN_CALENDER_LENGTH, "rx", SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_RX]);
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_TX] = soc_property_suffix_num_get(unit, port, spn_FC_INBAND_INTLKN_CALENDER_LENGTH, "tx", SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[port][SOC_TMC_CONNECTION_DIRECTION_TX]);

        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_RX] = soc_property_port_get(unit, port, spn_FC_INBAND_INTLKN_CALENDER_REP_COUNT, SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_RX]);
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_TX] = soc_property_port_get(unit, port, spn_FC_INBAND_INTLKN_CALENDER_REP_COUNT, SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_TX]);

        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_RX] = soc_property_suffix_num_get(unit, port, spn_FC_INBAND_INTLKN_CALENDER_REP_COUNT, "rx", SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_RX]);
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_TX] = soc_property_suffix_num_get(unit, port, spn_FC_INBAND_INTLKN_CALENDER_REP_COUNT, "tx", SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[port][SOC_TMC_CONNECTION_DIRECTION_TX]);

        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_llfc_mode[port] = soc_property_port_get(unit, port, spn_FC_INBAND_INTLKN_CALENDER_LLFC_MODE, SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_llfc_mode[port]);

        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_llfc_mub_enable_mask[port] = soc_property_port_get(unit, port, spn_FC_INBAND_INTLKN_LLFC_MUB_ENABLE_MASK, SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_llfc_mub_enable_mask[port]);
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_llfc_mub_enable_mask[port] &= SOC_DPP_FC_INBAND_INTLKN_LLFC_MUB_MASK; 
    }

    /* consistency checks - Out-Of-Band */
    for (port = 0; port < SOC_DPP_CONFIG(unit)->tm.max_oob_ports; port++) {
        if (SOC_DPP_CONFIG(unit)->tm.fc_oob_mode[port] == SOC_DPP_FC_CAL_MODE_DISABLE) {
            continue;
        }
        
        rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_drv_fc_oob_mode_validate,(unit, port));
        SOCDNX_IF_ERR_EXIT(rv);

        switch (SOC_DPP_CONFIG(unit)->tm.fc_oob_type[port]) {
            case SOC_TMC_FC_CAL_TYPE_SPI:
            case SOC_TMC_FC_CAL_TYPE_ILKN:
            case SOC_TMC_FC_CAL_TYPE_HCFC:
                 break;

            default:
                 LOG_ERROR(BSL_LS_SOC_INIT,
                           (BSL_META_U(unit,
                                       "Setting OOB port(%d) type to Interlaken\n"), port));
                 SOC_DPP_CONFIG(unit)->tm.fc_oob_type[port] = SOC_TMC_FC_CAL_TYPE_ILKN;
                 break;
        }

        for(fc_direction = SOC_TMC_CONNECTION_DIRECTION_RX; fc_direction <= SOC_TMC_CONNECTION_DIRECTION_TX; fc_direction++)
        {
          if (SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[port][fc_direction] > SOC_TMC_FC_OOB_CAL_LEN_MAX) {
              LOG_ERROR(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "Setting OOB port(%d) Cal Length to (%d)\n"), port, SOC_TMC_FC_OOB_CAL_LEN_MAX));
              SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[port][fc_direction] = SOC_TMC_FC_OOB_CAL_LEN_MAX;
          }
          if ((SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[port][fc_direction] < SOC_TMC_FC_OOB_CAL_REP_MIN) ||
            (SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[port][fc_direction] > SOC_TMC_FC_OOB_CAL_REP_MAX)) {
              LOG_ERROR(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "Setting OOB port(%d) Cal Rep Count to (%d)\n"), port, SOC_TMC_FC_OOB_CAL_REP_MIN));
              SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[port][fc_direction] = SOC_TMC_FC_OOB_CAL_REP_MIN;
          }
          if (SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_length[port][fc_direction] * SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[port][fc_direction] > SOC_TMC_FC_OOB_CAL_LEN_MAX) {
              LOG_ERROR(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "Setting OOB port(%d) Cal Rep Count to (%d)\n"), port, SOC_TMC_FC_OOB_CAL_REP_MIN));
              SOC_DPP_CONFIG(unit)->tm.fc_oob_calender_rep_count[port][fc_direction] = SOC_TMC_FC_OOB_CAL_REP_MIN;
          }
        }
    }

    /* consistency checks - InBand */
    for (port = 0; port < SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports; port++) {
        if (SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_mode[port] == SOC_DPP_FC_CAL_MODE_DISABLE) {
            continue;
        }

        for(fc_direction = SOC_TMC_CONNECTION_DIRECTION_RX; fc_direction <= SOC_TMC_CONNECTION_DIRECTION_TX; fc_direction++)
        {
          if (SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[port][fc_direction] > (SOC_TMC_FC_OOB_CAL_LEN_MAX/2)) {
              LOG_ERROR(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "Setting interlaken port(%d) Cal Length to (%d)\n"), port, (SOC_TMC_FC_OOB_CAL_LEN_MAX/2)));
              SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[port][fc_direction] = (SOC_TMC_FC_OOB_CAL_LEN_MAX/2);
          }
          if ((SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[port][fc_direction] < SOC_TMC_FC_OOB_CAL_REP_MIN) ||
            (SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[port][fc_direction] > SOC_TMC_FC_OOB_CAL_REP_MAX)) {
              LOG_ERROR(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "Setting interlaken port(%d) Cal Rep Count to (%d)\n"), port, SOC_TMC_FC_OOB_CAL_REP_MIN));
              SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[port][fc_direction] = SOC_TMC_FC_OOB_CAL_REP_MIN;
          }
          if (SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_length[port][fc_direction] * SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[port][fc_direction] > (SOC_TMC_FC_OOB_CAL_LEN_MAX/2)) {
              LOG_ERROR(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "Setting interlaken port(%d) Cal Rep Count to (%d)\n"), port, SOC_TMC_FC_OOB_CAL_REP_MIN));
              SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_rep_count[port][fc_direction] = SOC_TMC_FC_OOB_CAL_REP_MIN;
          }
        }

        switch(SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_llfc_mode[port]) {
           case SOC_DPP_FC_INBAND_INTLKN_CAL_LLFC_MODE1:
               break;
        
           case SOC_DPP_FC_INBAND_INTLKN_CAL_LLFC_MODE2:
               break;

            default:
                 LOG_ERROR(BSL_LS_SOC_INIT,
                           (BSL_META_U(unit,
                                       "disabling interlaken port(%d) LLFC via calender\n"), port));
                 SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_llfc_mode[port] = SOC_DPP_FC_INBAND_INTLKN_CAL_LLFC_MODE_DISABLE;
                 break;
        }

        if ( (SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_calender_llfc_mode[port] != SOC_DPP_FC_INBAND_INTLKN_CAL_LLFC_MODE_DISABLE) &&
             (SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_llfc_mub_enable_mask[port] != SOC_DPP_FC_INBAND_INTLKN_LLFC_MUB_DISABLE) ) {
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "disabling interlaken port(%d) LLFC via multi use bits\n"), port));
            SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_llfc_mub_enable_mask[port] = SOC_DPP_FC_INBAND_INTLKN_LLFC_MUB_DISABLE;
        }
    }

    /* consistency checks - Out-Of-Band/InBand */
    if ( (SOC_DPP_CONFIG(unit)->tm.fc_oob_mode[0] != SOC_DPP_FC_CAL_MODE_DISABLE) &&
          (SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_mode[0] != SOC_DPP_FC_CAL_MODE_DISABLE) ) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "disabling interlaken port(%d)\n"), port));
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_mode[0] = SOC_DPP_FC_CAL_MODE_DISABLE;
    }
    if ( (SOC_DPP_CONFIG(unit)->tm.fc_oob_mode[1] != SOC_DPP_FC_CAL_MODE_DISABLE) &&
          (SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_mode[1] != SOC_DPP_FC_CAL_MODE_DISABLE) ) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "disabling interlaken port(%d)\n"), port));
        SOC_DPP_CONFIG(unit)->tm.fc_inband_intlkn_mode[1] = SOC_DPP_FC_CAL_MODE_DISABLE;
    }

    /* Set WRED packet size */
    SOC_DPP_CONFIG(unit)->tm.wred_packet_size = soc_property_get(unit, spn_DISCARD_MTU_SIZE, 0x0);

    /* Set multicast egress bitmap range */
    SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(&SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range);
    SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_low = soc_property_get(unit,spn_EGRESS_MULTICAST_DIRECT_BITMAP_MIN,0x0);
    SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high = soc_property_get(unit,spn_EGRESS_MULTICAST_DIRECT_BITMAP_MAX,4095);
    
    /* Set Flow Base Queue */
    SOC_DPP_CONFIG(unit)->tm.flow_base_q = soc_property_get(unit, spn_FLOW_MAPPING_QUEUE_BASE, 0);

    /* CNM Mode */
    SOC_DPP_CONFIG(unit)->tm.cnm_mode = soc_property_get(unit,spn_CONGESTION_POINT_MODE,0x0);
    SOC_DPP_CONFIG(unit)->tm.cnm_voq_mapping_mode = soc_property_get(unit,spn_VOQ_TO_CMQ_MAPPING_MODE,0x0);

    for (port = 0; port < SOC_DPP_CONFIG(unit)->tm.max_interlaken_ports; port++) {
        SOC_DPP_CONFIG(unit)->tm.fc_ilkn_rt_calendar_mode[port][SOC_TMC_CONNECTION_DIRECTION_RX] = soc_property_port_get(unit, port, spn_ILKN_RETRANSMIT_CALENDAR_MODE_RX, 0);
        SOC_DPP_CONFIG(unit)->tm.fc_ilkn_rt_calendar_mode[port][SOC_TMC_CONNECTION_DIRECTION_TX] = soc_property_port_get(unit, port, spn_ILKN_RETRANSMIT_CALENDAR_MODE_TX, 0);
    }

#ifdef BCM_ARAD_SUPPORT
    if (SOC_DPP_CONFIG(unit)->arad_plus) {
        SOCDNX_IF_ERR_EXIT(soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_PLUS_CONFIG)); /* initialize WB buffer (reminder: on warm boot restore Arad+ config) */
    }
#endif /* BCM_ARAD_SUPPORT */
    SOC_DPP_CONFIG(unit)->tm.guaranteed_q_mode = SOC_DPP_GUARANTEED_Q_RESOURCE_BDS;

#ifdef BCM_ARAD_SUPPORT
    SOCDNX_IF_ERR_EXIT(soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_TM_GUARANTEED_Q_RESOURCE));
    if (!SOC_WARM_BOOT(unit)) {
        soc_dpp_guaranteed_q_resource_t guaranteed_q_resource;
        guaranteed_q_resource.total = 0;
        guaranteed_q_resource.used = 0;
        rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_TM_GUARANTEED_Q_RESOURCE, &guaranteed_q_resource);
        SOCDNX_IF_ERR_EXIT(rv);
    }
#endif /* BCM_ARAD_SUPPORT */

exit:
    SOCDNX_FUNC_RETURN;
}


SOC_SAND_RET
  soc_dpp_sand_write_cb(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 )
{
   SOC_SAND_RET
     ret = SOC_SAND_OK;
   int
    i,
    local_size,
    unit;
    
   unit = PTR_TO_INT(base_address);

   local_size = size >> 2;
  
   for (i=0;i<local_size;i++)
   {
     CMVEC(unit).write(&CMDEV(unit).dev, (offset + (i * 4)), (uint32)*(array + i));
   }
   return ret;  
}

SOC_SAND_RET
  soc_dpp_sand_read_cb(
    SOC_SAND_INOUT  uint32 *array,
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
 )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  int
    i,
    local_size,
    unit;
    
  unit = PTR_TO_INT(base_address);

  local_size = size >> 2 ;
  
  for (i=0;i<local_size;i++)
  {
    *(array + i) = CMVEC(unit).read(&CMDEV(unit).dev, (offset + (i * 4)));
  }  
  return ret;
}

STATIC int
soc_dpp_fabric_mc_init(int unit) {
    int soc_sand_rv = 0;
    SOC_TMC_MULT_FABRIC_INFO info, exact_info;

    SOCDNX_INIT_FUNC_DEFS;
    
    SOC_TMC_MULT_FABRIC_INFO_clear(&info);
    
    info.max_rate = 250 * 1024 * 1024;
    info.guaranteed.gr_shaper.max_burst = 0;
    info.best_effort.be_shaper.max_burst = 0;
    
    soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_credit_source_set,(unit, &info, &exact_info));
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
 
exit:
    SOCDNX_FUNC_RETURN; 
}

int
soc_dpp_common_init(int unit)
{    
    int soc_sand_rv = 0;
    uint32 is_already_opened;
    SOC_SAND_PHYSICAL_ACCESS soc_sand_physical_access;
    soc_dpp_config_t *dpp = NULL;

    SOCDNX_INIT_FUNC_DEFS;

    dpp = SOC_DPP_CONFIG(unit);

    
    /* allocate counter module resources */
#ifdef BCM_ARAD_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        SOC_CONTROL(unit)->controlled_counters = soc_arad_controlled_counter;
    }
#endif
    
    /* Init SOC_SAND module */
    soc_sand_rv = soc_sand_module_open(SOC_SAND_MAX_DEVICE, 16, SOC_SAND_TCM_DEFAULT_TASK_PRIORITY,
                               1, FALSE, NULL, NULL, &is_already_opened);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    soc_sand_tcm_set_enable_flag(0);

    /* Register device */
    
#if defined(BCM_PETRAB_SUPPORT) && defined(PLISIM)
    if (SOC_IS_PETRAB(unit) && SAL_BOOT_PLISIM) {
        soc_sand_physical_access.physical_write = soc_dpp_sand_write_cb_chipsim_pb;
        soc_sand_physical_access.physical_read  = soc_dpp_sand_read_cb_chipsim_pb;
    } else
#endif
    {
        soc_sand_physical_access.physical_write = soc_dpp_sand_write_cb;
        soc_sand_physical_access.physical_read  = soc_dpp_sand_read_cb;
    }

    soc_sand_set_physical_access_hook(&soc_sand_physical_access);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    soc_sand_rv = soc_sand_device_register_with_id(TRUE);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Set 1*1 mapping. Assumed in some SOC places. */

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit,mbcm_dpp_register_device,( (uint32 *) dpp->tm.base_address, NULL, &unit)));
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (!SOC_IS_ARDON(unit)) {
          soc_sand_rv = soc_counter_attach(unit);   
          SOCDNX_IF_ERR_EXIT(soc_sand_rv); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}   

int
soc_dpp_common_tm_init(int unit, SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *multicast_egress_bitmap_group_range)
{    
    int soc_sand_rv = 0;
    soc_dpp_config_t *dpp = NULL;
    SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO ipq_explicit_mapping_mode;

    SOCDNX_INIT_FUNC_DEFS;

    dpp = SOC_DPP_CONFIG(unit);

    /* Fabric multicsat credit source and rate */
    soc_sand_rv = soc_dpp_fabric_mc_init(unit);
    SOCDNX_IF_ERR_EXIT(soc_sand_rv);
    
#if defined(BCM_ARAD_SUPPORT) && !defined(JERICHO_MULTICAST_IMPLEMENTED)
    if (!SOC_IS_JERICHO(unit)) {
#endif
    /* Set multicast egress vlan membership range */
    soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_vlan_membership_group_range_set,(unit,multicast_egress_bitmap_group_range));
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
#if defined(BCM_ARAD_SUPPORT) && !defined(JERICHO_MULTICAST_IMPLEMENTED)
    }
#endif
    
    /* Set the Base Flow ID */    
    SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(&ipq_explicit_mapping_mode);
    ipq_explicit_mapping_mode.queue_id_add_not_decrement = TRUE; /* Decrement is not supported */
    ipq_explicit_mapping_mode.base_queue_id = dpp->tm.flow_base_q; 

    soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_explicit_mapping_mode_info_set,(unit, &ipq_explicit_mapping_mode));
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}   

int
soc_dpp_petra_stk_modid_config_set(int unit, int fap_id) {
  int soc_sand_rv = 0;

  SOCDNX_INIT_FUNC_DEFS;

  /* Special check for TDM special source ID. Only valid if Packets are coming from IRE */
  if (fap_id == DPP_TDM_FAP_DEVICE_ID_UNIQUE && /* Special FAP Device ID */
      (SOC_DPP_CONFIG(unit)->tdm.is_bypass || /* TDM bypass mode on */
       (SOC_IS_PETRAB(unit) && (SOC_IS_DPP_TDM_STANDARD(unit) || SOC_IS_DPP_TDM_OPTIMIZE(unit))))) { /* Or Petra-B is TDM optimize or standarad */
      SOCDNX_EXIT_WITH_ERR(SOC_E_BADID, (_BSL_SOCDNX_MSG("invalid FAP ID %d . In case TDM in device FAP device ID must not special device unique id"),fap_id));
  }

  soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_system_fap_id_set,(unit,fap_id));
  SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}
    

int
soc_dpp_petra_stk_modid_set(int unit, int fap_id) {
  int soc_sand_rv = 0;
#ifdef BCM_PETRAB_SUPPORT
  soc_dpp_config_t *dpp = NULL;
#endif
  int silent = 0;

  SOCDNX_INIT_FUNC_DEFS;

  if(unit < 0 || unit >= SOC_MAX_NUM_DEVICES) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("ERROR: invalid unit")));
  }

  /* Special check for TDM special source ID. Only valid if Packets are coming from IRE */
  if (fap_id == DPP_TDM_FAP_DEVICE_ID_UNIQUE && /* Special FAP Device ID */
      (SOC_DPP_CONFIG(unit)->tdm.is_bypass || /* TDM bypass mode on */
       (SOC_IS_PETRAB(unit) && (SOC_IS_DPP_TDM_STANDARD(unit) || SOC_IS_DPP_TDM_OPTIMIZE(unit))))) { /* Or Petra-B is TDM optimize or standarad */
      SOCDNX_EXIT_WITH_ERR(SOC_E_BADID, (_BSL_SOCDNX_MSG("invalid FAP ID %d . In case TDM in device FAP device ID must not special device unique id"),fap_id));
  }

  soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_system_fap_id_set,(unit,fap_id));
  SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

#ifdef BCM_PETRAB_SUPPORT
  dpp = SOC_DPP_CONFIG(unit);

  if (SOC_IS_PETRAB(unit))
  {
      SOC_PB_MGMT_B0_INFO b0_info;

      if (!SOC_DPP_CONTROL(unit)->is_modid_set_called) {
          soc_sand_rv = soc_pb_mgmt_init_sequence_phase2(unit, &(dpp->pb->oor), silent);
          SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
          
          soc_sand_rv = soc_petra_default_ofp_rates_set(unit);
          SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

          /* Enable bug fixes */
          if (!SOC_PB_REV_A1_OR_BELOW) {
            SOC_PB_MGMT_B0_INFO_clear(&b0_info);
            b0_info.bugfixes_enabled = 1;

            soc_sand_rv = soc_pb_mgmt_rev_b0_set(unit,&b0_info);
            SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
          }
    
          SOC_DPP_CONTROL(unit)->is_modid_set_called=1;
      }
  }
#endif /* BCM_PETRAB_SUPPORT */

#ifdef BCM_ARAD_SUPPORT
  
  if (SOC_IS_ARAD(unit) && !SOC_IS_JERICHO(unit)) 
  {
      if (!SOC_DPP_CONTROL(unit)->is_modid_set_called) {
          
          soc_sand_rv = arad_mgmt_init_sequence_phase2(unit, silent);
          SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

          SOC_DPP_CONTROL(unit)->is_modid_set_called=1;
      }
  }
#endif /* BCM_ARAD_SUPPORT */

exit:
    SOCDNX_FUNC_RETURN;
}
    
int
soc_dpp_petra_stk_module_enable(int unit, int enable) {
    int soc_sand_rv;
    
#ifdef BCM_ARAD_SUPPORT
    if ((SOC_IS_ARAD(unit)) && (enable))  {
        soc_error_t rv;
        uint8 is_power_save_called = 0;

        soc_sand_rv = arad_mgmt_init_finalize(unit);
        SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
        rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TM_POWER_SAVE_INIT_CALLED, &is_power_save_called);
        if(SOC_FAILURE(rv)){
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "SOC_DPP_WB_ENGINE_GET_VAR failed\n")));
            return rv;
        }
        if(!is_power_save_called){ 
            soc_sand_rv = arad_activate_power_savings(unit);
            SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
            is_power_save_called= 1;
            rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_TM_POWER_SAVE_INIT_CALLED, &is_power_save_called);
            if(SOC_FAILURE(rv)){
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "SOC_DPP_WB_ENGINE_SET_VAR failed\n")));
                return rv;
            }
        }
    }
#endif /* BCM_ARAD_SUPPORT */
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit))
    { 
        soc_sand_rv = soc_petra_ofp_rates_update_device_set(unit, enable? TRUE : FALSE);
        SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
    }
#endif /* BCM_PETRAB_SUPPORT */

    soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_enable_traffic_set,(unit, enable? TRUE : FALSE));
    SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

    return SOC_E_NONE;
}

STATIC int
soc_dpp_parse_device_core_mode(int unit) {
    char *core_mode_propval = NULL,
         *core_mode_propkey = spn_DEVICE_CORE_MODE;
    SOCDNX_INIT_FUNC_DEFS;
    if (SOC_DPP_DEFS_GET(unit, nof_cores) > 1) {
        core_mode_propval = soc_property_get_str(unit, core_mode_propkey);
        if (core_mode_propval) {
            if (sal_strcmp(core_mode_propval, "SYMMETRIC") == 0) {
                SOC_DPP_CONFIG(unit)->core_mode.type = soc_dpp_core_symmetric;
                SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
            } else if (sal_strcmp(core_mode_propval, "ASYMMETRIC") == 0) {
                SOC_DPP_CONFIG(unit)->core_mode.type = soc_dpp_core_asymmetric;
                SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
            } else if (sal_strcmp(core_mode_propval, "SINGLE_CORE") == 0) {
                SOC_DPP_CONFIG(unit)->core_mode.type = soc_dpp_core_symmetric;
                SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores = 1;
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Unexpected property value (\"%s\") for %s"), core_mode_propval, core_mode_propkey));
            }
        } else {
            /*canot be undifined, return failure*/
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Core device mode must be defined.")));
        }
    } else {
        SOC_DPP_CONFIG(unit)->core_mode.type = soc_dpp_core_symmetric;
        SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores = 1;
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_init(int unit, int reset)
{
    int rv;
    soc_control_t* soc;

    soc = SOC_CONTROL(unit);
    rv = soc_dpp_implementation_defines_init(unit);
    SOCDNX_IF_ERR_RETURN(rv);

#ifdef BCM_ARAD_SUPPORT      
    if (SOC_IS_ARAD(unit)) {

        rv = soc_arad_default_config_get(unit, SOC_DPP_CONFIG(unit)->arad);
        SOCDNX_IF_ERR_RETURN(rv);

        /* Retrive SOC properties which are relevant to WB (user dram buffer) */
        rv = soc_arad_dram_param_set(unit, SOC_DPP_CONFIG(unit)->arad);
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_SOC_INIT,
                     (BSL_META_U(unit,
                                 "%s: unit %d soc_arad_dram_param_set failed (%s)\n"), FUNCTION_NAME(), unit, soc_errmsg(rv)));
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    /* Recover stored Level 2 Warm Boot cache */
    /* The stable and stable size must be selected first */
    if (SOC_WARM_BOOT(unit)) {
        rv = soc_scache_recover(unit);
        if (SOC_FAILURE(rv)) {
            /* Fall back to Level 1 Warm Boot recovery */
            SOCDNX_IF_ERR_RETURN(soc_stable_size_set(unit, 0));
            SOCDNX_IF_ERR_RETURN
                (soc_stable_set(unit, _SHR_SWITCH_STABLE_NONE, 0));
            /* Error report */
            SOCDNX_IF_ERR_RETURN
                (soc_event_generate(unit, SOC_SWITCH_EVENT_STABLE_ERROR, 
                                    SOC_STABLE_CORRUPT,
                                    SOC_STABLE_FLAGS(unit), 0));
            LOG_VERBOSE(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "Unit %d: Corrupt stable cache.\n"),
                                    unit));
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
    rv = soc_dpp_wb_engine_init(unit);

    if (SOC_WARM_BOOT(unit)) {
        rv = soc_sw_state_init(unit, 0x0, socSwStateDataBlockRestoreAndOveride, -1);
        SOCDNX_IF_ERR_RETURN(rv);
    }
    else{
        rv = soc_sw_state_init(unit, 0x0, socSwStateDataBlockRegularInit, SOC_SW_STATE_MAX_DATA_SIZE_IN_BYTES);
        SOCDNX_IF_ERR_RETURN(rv);
    }
    SOCDNX_IF_ERR_RETURN(rv);
#endif /* BCM_ARAD_SUPPORT */

    rv = soc_dpp_parse_device_core_mode(unit);
    SOCDNX_IF_ERR_RETURN(rv);

    switch (SOC_INFO(unit).chip_type) {
#if defined(BCM_PETRA_SUPPORT) 

#ifdef BCM_PETRAB_SUPPORT
    case SOC_INFO_CHIP_TYPE_PETRAB:
        rv = soc_petra_init(unit, reset);
        break;
#endif /* BCM_PETRAB_SUPPORT */

#ifdef BCM_ARAD_SUPPORT
    case SOC_INFO_CHIP_TYPE_ARAD:
    case SOC_INFO_CHIP_TYPE_ARADPLUS:
    case SOC_INFO_CHIP_TYPE_ARDON:
        rv = soc_arad_init(unit, reset);        
        break;
#endif /* BCM_ARAD_SUPPORT */


#ifdef BCM_JERICHO_P3_SUPPORT
    case SOC_INFO_CHIP_TYPE_JERICHO_P3:
        rv = SOC_E_NONE;/* nothing to do */      
        break;
#endif

#ifdef BCM_JERICHO_SUPPORT
    case SOC_INFO_CHIP_TYPE_JERICHO:
    case SOC_INFO_CHIP_TYPE_QMX:
        rv = soc_dpp_jericho_init(unit, reset);
        break;
#endif /* BCM_JERICHO_SUPPORT */

#endif
    default:
        rv = SOC_E_UNIT;
        break;
    }

    soc->soc_flags |= SOC_F_INITED;
    return rv;
}

int
soc_dpp_device_reset(int unit, int mode, int action)
{
#if defined(BCM_ARAD_SUPPORT)
#if defined(BCM_JERICHO_SUPPORT)
    if (SOC_IS_JERICHO(unit)) {
        return (soc_jer_device_reset(unit, mode, action));
    }
#endif /* BCM_JERICHO_SUPPORT */
    if (SOC_IS_ARAD(unit)) {
        return (soc_arad_device_reset(unit, mode, action));
    }
#endif /* defined(BCM_ARAD_SUPPORT) */

#ifdef BCM_JERICHO_P3_SUPPORT
    if (SOC_IS_JERICHO_P3(unit)) {
        return SOC_E_NONE;
    }
#endif
    return SOC_E_UNAVAIL;
}

int
soc_dpp_reinit(int unit, int reset)
{
    int rv;

    soc_feature_init(unit);


    /* Must call mbcm init first to ensure driver properly installed */


    switch (SOC_INFO(unit).chip_type) {
#if defined(BCM_PETRA_SUPPORT) 
#ifdef BCM_PETRAB_SUPPORT
    case SOC_INFO_CHIP_TYPE_PETRAB:
        rv = soc_petra_init(unit, reset);
        break;
#endif /* BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT        
    case SOC_INFO_CHIP_TYPE_ARAD:
    case SOC_INFO_CHIP_TYPE_ARADPLUS:
        rv = soc_arad_reinit(unit, reset);
       
        break;
#endif

#ifdef BCM_JERICHO_P3_SUPPORT
    case SOC_INFO_CHIP_TYPE_JERICHO_P3:
        rv = SOC_E_NONE;/* nothing to do */      
        break;
#endif
                
#endif
    default:
        rv = SOC_E_UNIT;
        break;
    }
return rv;
    
}



#define P bsl_printf

int
soc_dpp_dump(int unit, char *pfx)
{
    soc_control_t       *soc;
    soc_persist_t       *sop;
    soc_stat_t          *stat;
    int                 i;
    uint16              dev_id;
    uint8               rev_id;
    int                 dcb_type;

    if (!SOC_UNIT_VALID(unit)) {
        return(SOC_E_UNIT);
    }

    soc = SOC_CONTROL(unit);
    sop = SOC_PERSIST(unit);

    stat = &soc->stat;

    P("%sUnit %d Driver Control Structure:\n", pfx, unit);

    soc_cm_get_id(unit, &dev_id, &rev_id);

    P("%sChip=%s Rev=0x%02x Driver=%s\n",
      pfx,
      soc_dev_name(unit),
      rev_id,
      SOC_CHIP_NAME(soc->chip_driver->type));
    P("%sFlags=0x%x:",
      pfx, soc->soc_flags);
    if (soc->soc_flags & SOC_F_ATTACHED)        P(" attached");
    if (soc->soc_flags & SOC_F_INITED)          P(" initialized");
    if (soc->soc_flags & SOC_F_LSE)             P(" link-scan");
    if (soc->soc_flags & SOC_F_SL_MODE)         P(" sl-mode");
    if (soc->soc_flags & SOC_F_POLLED)          P(" polled");
    if (soc->soc_flags & SOC_F_URPF_ENABLED)    P(" urpf");
    if (soc->soc_flags & SOC_F_MEM_CLEAR_USE_DMA) P(" mem-clear-use-dma");
    if (soc->soc_flags & SOC_F_IPMCREPLSHR)     P(" ipmc-repl-shared");
    if (soc->remote_cpu)                P(" rcpu");
    P("; board type 0x%x", soc->board_type);
    P("\n");
    P("%s", pfx);
    soc_cm_dump(unit);

    P("%sDisabled: reg_flags=0x%x mem_flags=0x%x\n",
      pfx,
      soc->disabled_reg_flags, soc->disabled_mem_flags);
    P("%sSchanOps=%d MMUdbg=%d LinkPause=%d\n",
      pfx,
      stat->schan_op,
      sop->debugMode, soc->soc_link_pause);
    P("%sCounter: int=%dus per=%dus dmaBuf=%p\n",
      pfx,
      soc->counter_interval,
      SAL_USECS_SUB(soc->counter_coll_cur, soc->counter_coll_prev),
      (void *)soc->counter_buf32);
    P("%sTimeout: Schan=%d(%dus) MIIM=%d(%dus)\n",
      pfx,
      stat->err_sc_tmo, soc->schanTimeout,
      stat->err_mii_tmo, soc->miimTimeout);
    P("%sIntr: Total=%d Sc=%d ScErr=%d MMU/ARLErr=%d\n"
      "%s      LinkStat=%d PCIfatal=%d PCIparity=%d\n"
      "%s      ARLdrop=%d ARLmbuf=%d ARLxfer=%d ARLcnt0=%d\n"
      "%s      TableDMA=%d TSLAM-DMA=%d\n"
      "%s      MemCmd[BSE]=%d MemCmd[CSE]=%d MemCmd[HSE]=%d\n"
      "%s      ChipFunc[0]=%d ChipFunc[1]=%d ChipFunc[2]=%d\n"
      "%s      ChipFunc[3]=%d ChipFunc[4]=%d\n"
      "%s      FifoDma[0]=%d FifoDma[1]=%d FifoDma[2]=%d FifoDma[3]=%d\n"
      "%s      I2C=%d MII=%d StatsDMA=%d Desc=%d Chain=%d\n",
      pfx, stat->intr, stat->intr_sc, stat->intr_sce, stat->intr_mmu,
      pfx, stat->intr_ls,
      stat->intr_pci_fe, stat->intr_pci_pe,
      pfx, stat->intr_arl_d, stat->intr_arl_m,
      stat->intr_arl_x, stat->intr_arl_0,
      pfx, stat->intr_tdma, stat->intr_tslam,
      pfx, stat->intr_mem_cmd[0],
      stat->intr_mem_cmd[1], stat->intr_mem_cmd[2],
      pfx, stat->intr_chip_func[0], stat->intr_chip_func[1],
      stat->intr_chip_func[2],
      pfx, stat->intr_chip_func[3], stat->intr_chip_func[4],
      pfx, stat->intr_fifo_dma[0], stat->intr_fifo_dma[1],
      stat->intr_fifo_dma[2], stat->intr_fifo_dma[3],
      pfx, stat->intr_i2c, stat->intr_mii, stat->intr_stats,
      stat->intr_desc, stat->intr_chain);
    P("%sError: SDRAM=%d CFAP=%d Fcell=%d MmuSR=%d\n",
      pfx,
      stat->err_sdram, stat->err_cfap,
      stat->err_fcell, stat->err_sr);

    if (SOC_DCB(unit) != NULL) {
        dcb_type = SOC_DCB_TYPE(unit);
    }
    else {
        dcb_type = -1;
    }
    P("%sPKT DMA: dcb=t%d tpkt=%u tbyt=%u rpkt=%u rbyt=%u\n", pfx,
      dcb_type,
      stat->dma_tpkt, stat->dma_tbyt,
      stat->dma_rpkt, stat->dma_rbyt);
    P("%sDV: List: max-q=%d cur-tq=%d cur-rq=%d dv-size=%d\n", pfx,
      soc->soc_dv_cnt, soc->soc_dv_tx_free_cnt, soc->soc_dv_rx_free_cnt,
      soc->soc_dv_size);
    P("%sDV: Statistics: allocs=%d frees=%d alloc-q=%d\n",
      pfx, stat->dv_alloc, stat->dv_free, stat->dv_alloc_q);
   
    for (i = 0; i < soc->soc_max_channels; i++) { 
        sdc_t *sdc = &soc->soc_channels[i];     
        char *type;
        switch(sdc->sc_type) {
        case DV_NONE:           type = "--";            break;
        case DV_TX:             type = "TX";            break;
        case DV_RX:             type = "RX";            break;
        default:                type = "*INVALID*";     break;
        }
        P("%sdma-ch-%d %s %s Queue=%d (%p)%s%s%s%s\n",
          pfx, i, type,
          sdc->sc_dv_active ? "Active" : "Idle  ",
          sdc->sc_q_cnt, (void *)sdc->sc_q,
          (sdc->sc_flags & SOC_DMA_F_DEFAULT) ? " default" : "",
          (sdc->sc_flags & SOC_DMA_F_POLL) ? " polled" : " intr",
          (sdc->sc_flags & SOC_DMA_F_MBM) ? " mbm" : " no-mbm",
          (sdc->sc_flags & SOC_DMA_F_TX_DROP) ? " tx-drop" : "");
    }

    return(0);
}   

void
soc_dpp_chip_dump(int unit, soc_driver_t *d)
{
#define P bsl_printf
    soc_info_t          *si;
    int                 i, count = 0;
    soc_port_t          port;
    char                pfmt[SOC_PBMP_FMT_LEN];
    uint16              dev_id;
    uint8               rev_id;
    int                 blk, bindex;
    char                instance_string[3], block_name[14];

    if (d == NULL) {
        P("unit %d: no driver attached\n", unit);
        return;
    }

    P("driver %s (%s)\n", SOC_CHIP_NAME(d->type), d->chip_string);
    P("\tregsfile\t\t%s\n", d->origin);
    P("\tpci identifier\t\tvendor 0x%04x device 0x%04x rev 0x%02x\n",
      d->pci_vendor, d->pci_device, d->pci_revision);
    P("\tclasses of service\t%d\n", d->num_cos);
    P("\tmaximums\t\tblock %d ports %d mem_bytes %d\n",
      SOC_MAX_NUM_BLKS, SOC_MAX_NUM_PORTS, SOC_MAX_MEM_BYTES);

    if (unit < 0) {
        return;
    }
    si = &SOC_INFO(unit);
    for (blk = 0; d->block_info[blk].type >= 0; blk++) {
        sal_snprintf(instance_string, sizeof(instance_string), "%d",
                     d->block_info[blk].number);
        if (d->block_info[blk].type == SOC_BLK_PORT_GROUP4 ||
            d->block_info[blk].type == SOC_BLK_PORT_GROUP5) {
            sal_strncpy(instance_string,
                       d->block_info[blk].number ? "_y" : "_x", 2);
            instance_string[2] = '\0';
        }
        sal_snprintf(block_name, sizeof(block_name), "%s%s",
                     soc_block_name_lookup_ext(d->block_info[blk].type, unit),
                     instance_string);
        P("\tblk %d\t\t%-14s schan %d cmic %d\n",
          blk,
          block_name,
          d->block_info[blk].schan,
          d->block_info[blk].cmic);
    }
    for (port = 0; ; port++) {
        blk = d->port_info[port].blk;
        bindex = d->port_info[port].bindex;
        if (blk < 0 && bindex < 0) {            /* end of list */
            break;
        }
        if (blk < 0) {                          /* empty slot */
            continue;
        }
        P("\tport %d\t\t%s\tblk %d %s%d.%d\n",
          soc_feature(unit, soc_feature_logical_port_num) ?
          si->port_p2l_mapping[port] : port,
          soc_block_port_name_lookup_ext(d->block_info[blk].type, unit),
          blk,
          soc_block_name_lookup_ext(d->block_info[blk].type, unit),
          d->block_info[blk].number,
          bindex);
    }

    soc_cm_get_id(unit, &dev_id, &rev_id);
    P("unit %d:\n", unit);
    P("\tpci\t\t\tdevice %04x rev %02x\n", dev_id, rev_id);
    P("\tdriver\t\t\ttype %d (%s) group %d (%s)\n",
      si->driver_type, SOC_CHIP_NAME(si->driver_type),
      si->driver_group, soc_chip_group_names[si->driver_group]);
    P("\tchip\t\t\t%s\n",
      SOC_IS_PETRAB(unit) ? "petrab" : "");
    P("\tGE ports\t%d\t%s (%d:%d)\n",
      si->ge.num, SOC_PBMP_FMT(si->ge.bitmap, pfmt),
      si->ge.min, si->ge.max);
    P("\tXE ports\t%d\t%s (%d:%d)\n",
      si->xe.num, SOC_PBMP_FMT(si->xe.bitmap, pfmt),
      si->xe.min, si->xe.max);
    P("\tCE ports\t%d\t%s (%d:%d)\n",
      si->ce.num, SOC_PBMP_FMT(si->ce.bitmap, pfmt),
      si->ce.min, si->ce.max);
    P("\tXL ports\t%d\t%s (%d:%d)\n",
      si->xl.num, SOC_PBMP_FMT(si->xl.bitmap, pfmt),
      si->xl.min, si->xl.max);
    P("\tHG ports\t%d\t%s (%d:%d)\n",
      si->hg.num, SOC_PBMP_FMT(si->hg.bitmap, pfmt),
      si->hg.min, si->hg.max);
    P("\tST ports\t%d\t%s (%d:%d)\n",
      si->st.num, SOC_PBMP_FMT(si->st.bitmap, pfmt),
      si->st.min, si->st.max);
    P("\tETHER ports\t%d\t%s (%d:%d)\n",
      si->ether.num, SOC_PBMP_FMT(si->ether.bitmap, pfmt),
      si->ether.min, si->ether.max);
    P("\tPON ports\t%d\t%s (%d:%d)\n",   
        si->pon.num, SOC_PBMP_FMT(si->pon.bitmap, pfmt),   
        si->pon.min, si->pon.max); 
    P("\tPORT ports\t%d\t%s (%d:%d)\n",
      si->port.num, SOC_PBMP_FMT(si->port.bitmap, pfmt),
      si->port.min, si->port.max);
    P("\tALL ports\t%d\t%s (%d:%d)\n",
      si->all.num, SOC_PBMP_FMT(si->all.bitmap, pfmt),
      si->all.min, si->all.max);
    P("\tIPIC port\t%d\tblock %d\n", si->ipic_port, si->ipic_block);
    P("\tCMIC port\t%d\t%s block %d\n", si->cmic_port,
      SOC_PBMP_FMT(si->cmic_bitmap, pfmt), si->cmic_block);
    P("\tother blocks\t\tARL %d MMU %d MCU %d\n",
      si->arl_block, si->mmu_block, si->mcu_block);
    P("\t            \t\tIPIPE %d IPIPE_HI %d EPIPE %d EPIPE_HI %d BSAFE %d ESM %d OTPC %d\n",
      si->ipipe_block, si->ipipe_hi_block,
      si->epipe_block, si->epipe_hi_block, si->bsafe_block, si->esm_block, si->otpc_block);

    for (i = 0; i < COUNTOF(si->has_block); i++) {
        if (si->has_block[i]) {
            count++;
        }
    }
    P("\thas blocks\t%d\t", count);
    for (i = 0; i < COUNTOF(si->has_block); i++) {
        if (si->has_block[i]) {
            P("%s ", soc_block_name_lookup_ext(si->has_block[i], unit));
            if ((i) && !(i%6)) {
                P("\n\t\t\t\t");
            }
        }
    }

    P("\n");
    P("\tport names\t\t");
    for (port = 0; port < si->port_num; port++) {
        if (port > 0 && (port % 5) == 0) {
            P("\n\t\t\t\t");
        }
        P("%d=%s\t",
          port, si->port_name[port]);
    }
    P("\n");
    i = 0;
    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++) {
        if (SOC_PBMP_IS_NULL(si->block_bitmap[blk])) {
            continue;
        }
        if (++i == 1) {
            P("\tblock bitmap\t");
        } else {
            P("\n\t\t\t");
        }
        P("%-2d  %-14s %s (%d ports)",
          blk,
          si->block_name[blk],
          SOC_PBMP_FMT(si->block_bitmap[blk], pfmt),
          si->block_valid[blk]);
    }
    if (i > 0) {
        P("\n");
    }

    {
        soc_feature_t f;

        P("\tfeatures\t");
        i = 0;
        for (f = 0; f < soc_feature_count; f++) {
            if (soc_feature(unit, f)) {
                if (++i > 3) {
                    P("\n\t\t\t");
                    i = 1;
                }
                P("%s ", soc_feature_name[f]);
            }
        }
        P("\n");
    }
#undef P
}

int
soc_dpp_nof_interrupts(int unit, int* nof_interrupts) 
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("invalid unit")));
    }

    SOCDNX_NULL_CHECK(nof_interrupts);

    rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_nof_interrupts,(unit, nof_interrupts));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dpp_nof_block_instances(int unit,  soc_block_types_t block_types, int *nof_block_instances) 
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("invalid unit")));
    }

    SOCDNX_NULL_CHECK(nof_block_instances);

    rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_nof_block_instance,(unit, block_types, nof_block_instances));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_dpp_avs_value_get(int unit, uint32* avs_value)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;
    
    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("invalid unit")));
    }
    
    SOCDNX_NULL_CHECK(avs_value);
    
    rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_avs_value_get,(unit, avs_value));
    SOCDNX_IF_ERR_EXIT(rc);
    
exit:
    SOCDNX_FUNC_RETURN;    
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * return 1 if warmboot sync should be done immediately after sw state change.
 * the autosync flag in SOC_CONTROL is used to indicate auto sync at the end of APIs
 * immediate sync override regular BCM autosync functionality.
 */
uint8 _bcm_dpp_switch_is_immediate_sync( 
    int unit) 
{
    return SOC_CONTROL(unit)->autosync;
}
#endif /*BCM_WARM_BOOT_SUPPORT*/

int
soc_dpp_cache_enable_init(int unit)
{
	uint32 						cache_enable = 1;
	int 						enable_all, enable_parity, enable_ecc, enable_predefined, enable_specific, disable_specific;
    char        				prop_enable_specific[SOC_PROPERTY_NAME_MAX];
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("invalid unit")));
    }

    sal_memset(prop_enable_specific, 0 , SOC_PROPERTY_NAME_MAX);
    sal_snprintf(prop_enable_specific, SOC_PROPERTY_NAME_MAX, "%s_%s", spn_MEM_CACHE_ENABLE, "specific");

    /* GET SOC PROPERTY*/
    enable_all 			= soc_property_suffix_num_get(unit, 0, spn_MEM_CACHE_ENABLE, "all", 0);
    enable_parity 		= soc_property_suffix_num_get(unit, 0, spn_MEM_CACHE_ENABLE, "parity", 0);
    enable_ecc	 		= soc_property_suffix_num_get(unit, 0, spn_MEM_CACHE_ENABLE, "ecc", 0);
    enable_predefined	= soc_property_suffix_num_get(unit, 0, spn_MEM_CACHE_ENABLE, "predefined", 0);
    enable_specific 	= soc_property_suffix_num_get(unit, 0, spn_MEM_CACHE_ENABLE, "specific", 0);
    disable_specific 	= soc_property_get(unit, spn_MEM_NOCACHE, 0);

	LOG_DEBUG(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "soc_dpp_cache_enable_init: enable_all %d enable_parity %d enable_static %d enable_specific %d disable_specific %d\n"),
			enable_all, enable_parity, enable_predefined, enable_specific, disable_specific));

    if (enable_all)
    {
		if (soc_mem_iterate(unit, arad_tbl_mem_cache_mem_set, &cache_enable) < 0)
			LOG_ERROR(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "soc_dpp_cache_enable_init: unit %d all_cache enable failed\n"), unit));
    }
	if (enable_parity)
	{
		if (soc_mem_iterate(unit, arad_tbl_mem_cache_enable_parity_tbl, &cache_enable) < 0)
			LOG_ERROR(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "soc_dpp_cache_enable_init: unit %d parity cache enable failed\n"), unit));
	}
	if (enable_ecc)
	{
		if (soc_mem_iterate(unit, arad_tbl_mem_cache_enable_ecc_tbl, &cache_enable) < 0)
			LOG_ERROR(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "soc_dpp_cache_enable_init: unit %d ecc cache enable failed\n"), unit));
	}
	if (enable_predefined)
	{
		if (arad_tbl_cache_enable_predefined_tbl(unit) < 0)
			LOG_ERROR(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "soc_dpp_cache_enable_init: unit %d predefined cache enable failed\n"), unit));
	}
	if (enable_specific)
	{
		if (soc_mem_iterate(unit, arad_tbl_mem_cache_enable_specific_tbl, &cache_enable) < 0)
			LOG_ERROR(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "soc_dpp_cache_enable_init: unit %d specific cache enable failed\n"), unit));
	}
	if (disable_specific)
	{
		cache_enable = 0;
		if (soc_mem_iterate(unit, arad_tbl_mem_cache_enable_specific_tbl, &cache_enable) < 0)
			LOG_ERROR(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "soc_dpp_cache_enable_init: unit %d specific cache disable failed\n"), unit));
	}
exit:
    SOCDNX_FUNC_RETURN;
}


