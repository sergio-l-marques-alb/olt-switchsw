/*
 * $Id: link.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * Hardware Linkscan module
 *
 * Hardware linkscan is available, but its use is not recommended
 * because a software linkscan task is very low overhead and much more
 * flexible.
 *
 * If hardware linkscan is used, each MII operation must temporarily
 * disable it and wait for the current scan to complete, increasing the
 * latency.  PHY status register 1 may contain clear-on-read bits that
 * will be cleared by hardware linkscan and not seen later.  Special
 * support is provided for the Serdes MAC.
 */

#include <sal/core/libc.h>
#include <sal/core/boot.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cmic.h>

/*
 * Function:    
 *      _soc_link_update
 * Purpose:
 *      Update the forwarding state in the chip (EPC_LINK).
 * Parameters:  
 *      unit - StrataSwitch unit #.
 * Returns:
 *      SOC_E_XXX
 * NOTE:
 * soc_link_fwd_set and soc_link_mask2_set call
 * this function to update EPC_LINK_BMAP. soc_link_fwd_set is called 
 * with LINK_LOCK and soc_link_mask2_set is called with PORT_LOCK.
 * No synchronization mechanism is implemented in this function. Therefore,
 * the user must make sure that the call to this function is synchronized 
 * between linkscan thread and calling thread. 
 */

STATIC int
_soc_link_update(int unit)
{
    pbmp_t      	pbm;
    soc_control_t	*soc = SOC_CONTROL(unit);
    soc_persist_t	*sop = SOC_PERSIST(unit);

#ifdef  BROADCOM_DEBUG
    char        pfmtl[SOC_PBMP_FMT_LEN],
        pfmtm2[SOC_PBMP_FMT_LEN],
        pfmtp[SOC_PBMP_FMT_LEN];
#endif  /* BROADCOM_DEBUG */

    if (SOC_IS_ROBO(unit)) {
        SOC_PBMP_ASSIGN(pbm, soc->link_fwd);        
    } else {
        SOC_PBMP_ASSIGN(pbm, sop->link_fwd);
    }
    SOC_PBMP_AND(pbm, soc->link_mask2);

    if (SOC_IS_ROBO(unit)){
        SOC_DEBUG_PRINT((DK_LINK | DK_VERBOSE,
		     "_soc_link_update: link=%s pbm=%s\n",
		     SOC_PBMP_FMT(soc->link_fwd, pfmtl),
		     SOC_PBMP_FMT(pbm, pfmtp)));

        return SOC_E_NONE;
     }
    SOC_DEBUG_PRINT((DK_LINK | DK_VERBOSE,
		     "_soc_link_update: link=%s m2=%s pbm=%s\n",
		     SOC_PBMP_FMT(sop->link_fwd, pfmtl),
		     SOC_PBMP_FMT(soc->link_mask2, pfmtm2),
		     SOC_PBMP_FMT(pbm, pfmtp)));

#ifdef BCM_HERCULES_SUPPORT
    if (SOC_IS_HERCULES(unit)) {
        int port;
        uint32  nlink, olink;

        nlink = SOC_PBMP_WORD_GET(pbm, 0);
        olink = -1;
        PBMP_PORT_ITER(unit, port) {
            SOC_IF_ERROR_RETURN
                (READ_ING_EPC_LNKBMAPr(unit, port, &olink));
            break;
        }

        if (nlink != olink) {
            PBMP_PORT_ITER(unit, port) {
                SOC_IF_ERROR_RETURN
                    (WRITE_ING_EPC_LNKBMAPr(unit, port, nlink));
            }
        }
        return SOC_E_NONE;
    }
#endif /* BCM_HERCULES_SUPPORT */

#ifdef BCM_TUCANA_SUPPORT
    if (SOC_IS_TUCANA(unit)) {
        uint64      nlink64, olink64;

        COMPILER_64_SET(nlink64, SOC_PBMP_WORD_GET(pbm, 1),
                        SOC_PBMP_WORD_GET(pbm, 0));

        SOC_IF_ERROR_RETURN
            (soc_reg64_read_any_block(unit, EPC_LINKr, &olink64));
        if (COMPILER_64_NE(nlink64, olink64)) {
            SOC_IF_ERROR_RETURN
                (soc_reg64_write_all_blocks(unit, EPC_LINKr, nlink64));
            SOC_IF_ERROR_RETURN
                (soc_reg64_write_all_blocks(unit, ILINKr, nlink64));
        }
        return SOC_E_NONE;
    }
#endif

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && !SOC_IS_ENDURO(unit)) {
        uint64      nlink64, olink64;

        COMPILER_64_SET(nlink64, SOC_PBMP_WORD_GET(pbm, 1),
                        SOC_PBMP_WORD_GET(pbm, 0));

        SOC_IF_ERROR_RETURN
            (soc_reg64_read_any_block(unit, EPC_LINK_BMAP_64r, &olink64));
        if (COMPILER_64_NE(nlink64, olink64)) {
            SOC_IF_ERROR_RETURN
                (soc_reg64_write_all_blocks(unit, EPC_LINK_BMAP_64r, nlink64));
        }
        return SOC_E_NONE;
    } else if(SOC_IS_ENDURO(unit)){
        uint32          olink, nlink;
        nlink = SOC_PBMP_WORD_GET(pbm, 0);

        SOC_IF_ERROR_RETURN
            (soc_reg_read_any_block(unit, EPC_LINK_BMAP_64r, &olink));
        if (nlink != olink) {
            SOC_IF_ERROR_RETURN
                (soc_reg_write_all_blocks(unit, EPC_LINK_BMAP_64r, nlink));
        }
        return SOC_E_NONE;    
    }   
#endif

    if (SOC_IS_XGS_SWITCH(unit)) {
        uint32          olink, nlink;
        soc_reg_t       epc_lnk_reg = EPC_LINKr;

#ifdef  BCM_XGS3_SWITCH_SUPPORT
        if (SOC_IS_XGS3_SWITCH(unit)) {
            epc_lnk_reg = EPC_LINK_BMAPr;
#if defined(BCM_RAPTOR_SUPPORT)
            if (soc_feature(unit, soc_feature_register_hi)) {
                SOC_IF_ERROR_RETURN
                    (WRITE_EPC_LINK_BMAP_HIr(unit, SOC_PBMP_WORD_GET(pbm, 1)));
            }
#endif /* BCM_RAPTOR_SUPPORT */
        }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

        SOC_IF_ERROR_RETURN
            (soc_reg_read_any_block(unit, epc_lnk_reg, &olink));
        nlink = SOC_PBMP_WORD_GET(pbm, 0);
        if (nlink != olink) {
            SOC_IF_ERROR_RETURN
                (soc_reg_write_all_blocks(unit, epc_lnk_reg, nlink));
#ifdef BCM_XGS12_SWITCH_SUPPORT
            if (SOC_IS_XGS12_SWITCH(unit)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_write_all_blocks(unit, ILINKr, nlink));
            }
#endif
        }
        return SOC_E_NONE; 
    }

    return SOC_E_NONE;  /* SOC_E_UNAVAIL? */
}

/*
 * Function:
 *      soc_link_fwd_set
 * Purpose:
 *      Sets EPC_LINK independent of chip type.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pbmp - Value.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      EPC_LINK should be manipulated only through this routine and
 *      soc_link_maskX_set.
 */

int
soc_link_fwd_set(int unit, pbmp_t fwd)
{
    if (SOC_IS_ROBO(unit)) {
        SOC_CONTROL(unit)->link_fwd = fwd;
    }else{
        SOC_PERSIST(unit)->link_fwd = fwd;
    }

    return _soc_link_update(unit);
}

/*
 * Function:
 *      soc_link_fwd_get
 * Purpose:
 *      Gets EPC_LINK independent of chip type.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pbmp - (OUT) Value.
 */

void
soc_link_fwd_get(int unit, pbmp_t *fwd)
{
    if (SOC_IS_ROBO(unit)) {
        *fwd = SOC_CONTROL(unit)->link_fwd;
    } else {
        *fwd = SOC_PERSIST(unit)->link_fwd;
    }
}

/*
 * Function:
 *      soc_link_mask2_set
 * Purpose:
 *      Mask bits in EPC_LINK independent of soc_link_fwd value.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      mask - Value.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      This routine is used to clear bits in the EPC_LINK to support
 *      the mac_fe/ge_enable_set() calls.
 */

int
soc_link_mask2_set(int unit, pbmp_t mask)
{
    SOC_CONTROL(unit)->link_mask2 = mask;

    return _soc_link_update(unit);
}

/*
 * Function:
 *      soc_link_mask2_get
 * Purpose:
 *      Counterpart to soc_link_mask2_set
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      mask - (OUT) Value.
 */

void
soc_link_mask2_get(int unit, pbmp_t *mask)
{
    *mask = SOC_CONTROL(unit)->link_mask2;
}

/*
 * Function:
 *      soc_linkscan_pause
 * Purpose:
 *      Pauses link scanning, without disabling it.
 *      This call is used to pause scanning temporarily.
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      Nothing
 * Notes:
 *      Nesting pauses is provided for.
 *      Software must ensure every pause is accompanied by a continue
 *      or linkscan will never resume.
 */

void
soc_linkscan_pause(int unit)
{
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SIRIUS_SUPPORT)
    soc_control_t	*soc = SOC_CONTROL(unit);
    int			s;
    if ( SOC_IS_ESW(unit) || SOC_IS_SIRIUS(unit) ) {

        s = sal_splhi();    /* Manipulate flags & regs atomically */

        if (soc->soc_link_pause++ == 0 &&
            (soc->soc_flags & SOC_F_LSE)) {
            /* Stop link scan and wait for current pass to finish */

            soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MIIM_LINK_SCAN_EN_CLR);

            while (soc_pci_read(unit, CMIC_SCHAN_CTRL) & SC_MIIM_SCAN_BUSY_TST) {
            /* Nothing */
            }
        }

        sal_spl(s);
    }
#endif /* BCM_ESW_SUPPORT | BCM_SIRIUS_SUPPORT */   
}

/*
 * Function:
 *      soc_linkscan_continue
 * Purpose:
 *      Continue link scanning after it has been paused.
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      Nothing
 * Notes:
 *      This routine is designed so if soc_linkscan_config is called,
 *      it won't be confused whether or not a pause is in effect.
 */

void
soc_linkscan_continue(int unit)
{
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SIRIUS_SUPPORT)
    soc_control_t	*soc = SOC_CONTROL(unit);
    int			s;

    if ( SOC_IS_ESW(unit) || SOC_IS_SIRIUS(unit) ) {   
        s = sal_splhi();    /* Manipulate flags & regs atomically */

        if (soc->soc_link_pause <= 0) {
            sal_spl(s);
            assert(0);      /* Continue not preceded by a pause */
        }

        if (--soc->soc_link_pause == 0 &&
            (soc->soc_flags & SOC_F_LSE)) {
            /*
             * NOTE: whenever hardware linkscan is running, the PHY_REG_ADDR
             * field of the MIIM_PARAM register must be set to 1 (PHY Link
             * Status register address).
             */

            if (soc_feature(unit, soc_feature_phy_cl45))  {
                /*
                ** Clause 22 Register 0x01 (MII_STAT) for FE/GE.
                ** Clause 45 Register 0x01 (MII_STAT) Devad = 0x1 (PMA_PMD) 
                ** for XE.
                */
                uint32 phy_miim_addr = 0;
                soc_reg_field_set(unit, CMIC_MIIM_ADDRESSr, &phy_miim_addr,
                              CLAUSE_45_DTYPEf, 0x01);
                soc_reg_field_set(unit, CMIC_MIIM_ADDRESSr, &phy_miim_addr,
                              CLAUSE_45_REGADRf, 0x01);
                soc_reg_field_set(unit, CMIC_MIIM_ADDRESSr, &phy_miim_addr,
                              CLAUSE_22_REGADRf, 0x01);
                WRITE_CMIC_MIIM_ADDRESSr(unit, phy_miim_addr);
            } else {
                soc_pci_write(unit, CMIC_MIIM_PARAM, (uint32) 0x01 << 24);
            }
            soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MIIM_LINK_SCAN_EN_SET);
        }

        sal_spl(s);
    }
#endif /* BCM_ESW_SUPPORT | BCM_SIRIUS_SUPPORT */
}

/*
 * Function:
 *      soc_linkscan_register
 * Purpose:
 *      Provide a callout made when CMIC link scanning detects a link change.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      f    - Function called when link status change is detected.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Handler called in interrupt context.
 */

int
soc_linkscan_register(int unit, void (*f)(int))
{
    soc_control_t	*soc = SOC_CONTROL(unit);

    if (f != NULL && soc->soc_link_callout != NULL) {
        return SOC_E_EXISTS;
    }

    soc->soc_link_callout = f;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_linkscan_config
 * Purpose:
 *      Set ports to scan in CMIC.
 * Parameters:
 *      unit - StrataSwich Unit #
 *      mii_pbm - Port bit map of ports to scan with MIIM registers
 *      direct_pbm - Port bit map of ports to scan using NON MII.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_linkscan_config(int unit, pbmp_t hw_mii_pbm, pbmp_t hw_direct_pbm)
{
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SIRIUS_SUPPORT)
    soc_control_t	*soc = SOC_CONTROL(unit);
    uint32		cmic_config;
    int			s, has_mge, has_dge;
    pbmp_t		pbm;
    uint32      link_pbmp; 

    if (SOC_IS_ROBO(unit)) {
        return SOC_E_NONE;
    }

    SOC_PBMP_ASSIGN(pbm, hw_mii_pbm);
    SOC_PBMP_AND(pbm, hw_direct_pbm);
    assert(SOC_PBMP_IS_NULL(pbm));      /* !(hw_mii_pbm & hw_direct_pbm) */

    /*
     * Hardware (direct) scanning is NOT supported on 10/100 ports.
     */
    SOC_PBMP_ASSIGN(pbm, hw_direct_pbm);
    SOC_PBMP_AND(pbm, PBMP_FE_ALL(unit));
    if (SOC_PBMP_NOT_NULL(pbm)) {
        return SOC_E_UNAVAIL;
    }

    /*
     * The LINK_SCAN_GIG control affects ALL ports. Thus, all ports
     * being scanned by H/W must be either MIIM scanned or scanned
     * using the direct connection.
     */
    SOC_PBMP_ASSIGN(pbm, PBMP_GE_ALL(unit));
    SOC_PBMP_AND(pbm, hw_mii_pbm);
    has_mge = SOC_PBMP_NOT_NULL(pbm);
    SOC_PBMP_ASSIGN(pbm, PBMP_GE_ALL(unit));
    SOC_PBMP_AND(pbm, hw_direct_pbm);
    has_dge = SOC_PBMP_NOT_NULL(pbm);
    if (has_mge && has_dge) {
        return SOC_E_UNAVAIL;
    }

    /*
     * soc_linkscan_pause/continue combination will result in the
     * registers being setup and started properly if we are enabling for
     * the first time.
     */

    s = sal_splhi();

    soc_linkscan_pause(unit);

    /* Check if disabling port scanning */

    cmic_config = soc_pci_read(unit, CMIC_CONFIG);

    SOC_PBMP_ASSIGN(pbm, hw_mii_pbm);
    SOC_PBMP_OR(pbm, hw_direct_pbm);
    if (SOC_PBMP_NOT_NULL(pbm)) {
        /*
         * NOTE: we are no longer using CC_LINK_STAT_EN since it is
         * unavailable on 5695 and 5665.  EPC_LINK will be updated by
         * software anyway, it will just take a few extra milliseconds.
         */
        soc->soc_flags |= SOC_F_LSE;
    } else {
        soc->soc_flags &= ~SOC_F_LSE;
    }


    if (soc_reg_field_valid(unit, CMIC_CONFIGr, LINK_STAT_ENf)) {
        soc_reg_field_set(unit, CMIC_CONFIGr, &cmic_config, LINK_STAT_ENf,
                          SOC_PBMP_NOT_NULL(pbm) ? 1 : 0);
    }

    /*
     * CC_LINK_SCAN_GIG must be on even if MII link scan is being used
     * for gig ports, or the gig ports will not be scanned.
     */
    SOC_PBMP_AND(pbm, PBMP_GE_ALL(unit));
    if (soc_reg_field_valid(unit, CMIC_CONFIGr, LINK_SCAN_GIGf)) {
        soc_reg_field_set(unit, CMIC_CONFIGr, &cmic_config, LINK_SCAN_GIGf,
                          SOC_PBMP_NOT_NULL(pbm) ? 1 : 0);
    }
    soc_pci_write(unit, CMIC_CONFIG, cmic_config);

    if (SOC_IS_TUCANA(unit)) {
        WRITE_CMIC_SCAN_PORTS_MOD0r(unit, SOC_PBMP_WORD_GET(hw_mii_pbm, 0));
        WRITE_CMIC_SCAN_PORTS_MOD1r(unit, SOC_PBMP_WORD_GET(hw_mii_pbm, 1));
    } else {
        link_pbmp = SOC_PBMP_WORD_GET(hw_mii_pbm, 0); 
#if defined(BCM_GOLDWING_SUPPORT) 
        if (SOC_IS_GOLDWING(unit)) {
            /* (MSB) 15-14-19-18-17-16-13-12-11-10-9-8-7-6-5-4-3-2-1-0 (LSB) */
            link_pbmp =  (link_pbmp & 0x00003FFF) |
                        ((link_pbmp & 0x000F0000) >> 2) |
                        ((link_pbmp & 0x0000C000) << 4);
        }
#endif /* BCM_GOLDWING_SUPPORT */
#if defined (BCM_SCORPION_SUPPORT)
        if (SOC_IS_SC_CQ(unit)) {
            /* CMIC port not included in link status */
            link_pbmp >>=  1;
        }
#endif /* BCM_SCORPION_SUPPORT */

        soc_pci_write(unit, CMIC_SCAN_PORTS, link_pbmp);

        if (((SOC_IS_TR_VL(unit) || SOC_IS_SIRIUS(unit)) 
                                 && !SOC_IS_ENDURO(unit)) ||
             soc_feature(unit, soc_feature_register_hi)) {
            WRITE_CMIC_SCAN_PORTS_HIr(unit, SOC_PBMP_WORD_GET(hw_mii_pbm, 1));
        }
    }    
    soc_linkscan_continue(unit);

    sal_spl(s);

#endif /* BCM_ESW_SUPPORT | BCM_SIRIUS_SUPPORT */
    return SOC_E_NONE;
}

#if defined(BCM_XGS3_SWITCH_SUPPORT) || defined(BCM_SIRIUS_SUPPORT)
STATIC int
_soc_linkscan_hw_map_init(int unit, soc_port_t port)
{
    uint32      oregv = 0;
    uint32      regv = 0;
    soc_reg_t   reg = CMIC_MIIM_EXT_PHY_ADDR_MAP_3_0r;
    soc_reg_t   map_r[16] = {   CMIC_MIIM_EXT_PHY_ADDR_MAP_3_0r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_7_4r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_11_8r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_15_12r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_19_16r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_23_20r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_27_24r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_31_28r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_35_32r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_39_36r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_43_40r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_47_44r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_51_48r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_55_52r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_59_56r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_63_60r
                                };
    soc_field_t   map_f[4] = {  PHY_ID_0f, PHY_ID_1f, PHY_ID_2f, PHY_ID_3f};
    int map_r_size = sizeof(map_r)/sizeof(map_r[0]);
    soc_port_t  port_bit = port;

#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        /*
         * SC/CQ devices omit the CMIC port from the PHY counting.  All
         * "port bitmaps" are shifted by 1.
         */
        port_bit -= 1;
    }
#endif

    /* Re-initialize the phy port map for the unit */
    /* Use MDIO address re-mapping for hardware linkscan */
    assert((port_bit >=0) && ((port_bit / 4) < map_r_size));
    reg = map_r[port_bit / 4];
    regv = soc_pci_read(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0));
    oregv = regv;
    soc_reg_field_set(unit,
                      CMIC_MIIM_EXT_PHY_ADDR_MAP_3_0r,
                      &regv, map_f[port_bit % 4],
                      PHY_ADDR(unit, port) & 0x1f);
    if (oregv != regv) {
        soc_pci_write(unit,
                      soc_reg_addr(unit, reg, REG_PORT_ANY, 0),
                      regv);
    }

    SOC_IF_ERROR_RETURN(READ_CMIC_CONFIGr(unit, &regv));
    oregv = regv;
    soc_reg_field_set(unit, CMIC_CONFIGr, &regv, MIIM_ADDR_MAP_ENABLEf, 1);
    if (oregv != regv) {
        WRITE_CMIC_CONFIGr(unit, regv);
    }

    return SOC_E_NONE;
}

STATIC int
_soc_linkscan_hw_port_init(int unit, soc_port_t port)
{
    uint32  regval;
    int bus_sel;
    soc_port_t  port_bit = port;

#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        /*
         * SC/CQ devices omit the CMIC port from the PHY counting.  All
         * "port bitmaps" are shifted by 1.
         */
        if (port == CMIC_PORT(unit)) {
            /* This should not be triggered */
            return SOC_E_PORT;
        }
        port_bit -= 1;
    }
#endif

    /*
     * Check If Hardware Linkscan should use Clause 45 mode
     */
    if ((IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)) &&
        PHY_CLAUSE45_MODE(unit, port)) {
        if (port_bit < 32) {
            SOC_IF_ERROR_RETURN
                (READ_CMIC_MIIM_PROTOCOL_MAPr(unit, &regval));
            regval |= (1 << port_bit);
            SOC_IF_ERROR_RETURN
                (WRITE_CMIC_MIIM_PROTOCOL_MAPr(unit, regval));
        } else if ((SOC_IS_TR_VL(unit) && !SOC_IS_ENDURO(unit)) ||
                   soc_feature(unit, soc_feature_register_hi)) {
             SOC_IF_ERROR_RETURN
                (READ_CMIC_MIIM_PROTOCOL_MAP_HIr(unit, &regval));
            regval |= (1 << (port_bit - 32));
            SOC_IF_ERROR_RETURN
                (WRITE_CMIC_MIIM_PROTOCOL_MAP_HIr(unit, regval));
        }
    }

    /*
     *  Select the appropriate MDIO bus
     */
    if (SOC_IS_TRX(unit)) {
        bus_sel = (PHY_ADDR(unit, port) & 0x60) >> 5; /* bus 0, 1, and 2 */
    } else {
        bus_sel = (PHY_ADDR(unit, port) & 0x40) >> 6; /* bus 0 and 1 */
    }

    if (bus_sel == 1) {
        if (port_bit < 32) {
            SOC_IF_ERROR_RETURN
                (READ_CMIC_MIIM_PORT_TYPE_MAPr(unit, &regval));
            regval |= (1 << (port_bit));
            SOC_IF_ERROR_RETURN
                (WRITE_CMIC_MIIM_PORT_TYPE_MAPr(unit, regval));
        } else if ((SOC_IS_TR_VL(unit) && !SOC_IS_ENDURO(unit)) ||
                   soc_feature(unit, soc_feature_register_hi)) {
            SOC_IF_ERROR_RETURN
                (READ_CMIC_MIIM_PORT_TYPE_MAP_HIr(unit, &regval));
            regval |= (1 << (port_bit - 32));
            SOC_IF_ERROR_RETURN
                (WRITE_CMIC_MIIM_PORT_TYPE_MAP_HIr(unit, regval));
        }
    }

#if defined(BCM_TRX_SUPPORT) || defined(BCM_SIRIUS_SUPPORT)
    if (bus_sel == 2) {
        if (port_bit < 32) {
            SOC_IF_ERROR_RETURN
                (READ_CMIC_MIIM_PORT_TYPE_MAP_BUS2r(unit, &regval));
            regval |= (1 << (port_bit));
            SOC_IF_ERROR_RETURN
                (WRITE_CMIC_MIIM_PORT_TYPE_MAP_BUS2r(unit, regval));
        } else if ((SOC_IS_TR_VL(unit) && !SOC_IS_ENDURO(unit)) ||
                   soc_feature(unit, soc_feature_register_hi)) {
             SOC_IF_ERROR_RETURN
                (READ_CMIC_MIIM_PORT_TYPE_MAP_BUS2_HIr(unit, &regval));
            regval |= (1 << (port_bit - 32));
            SOC_IF_ERROR_RETURN
                (WRITE_CMIC_MIIM_PORT_TYPE_MAP_BUS2_HIr(unit, regval));
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT | BCM_SIRIUS_SUPPORT */

    /*
     * Check If Hardware Linkscan should use internal phy
     */
    if (!PHY_EXTERNAL_MODE(unit, port)) {
        uint32  regval;
        if (port_bit < 32) {
            SOC_IF_ERROR_RETURN
                (READ_CMIC_MIIM_INT_SEL_MAPr(unit, &regval));
            regval |= (1 << port_bit);
            SOC_IF_ERROR_RETURN
                (WRITE_CMIC_MIIM_INT_SEL_MAPr(unit, regval));
        } else if ((SOC_IS_TR_VL(unit) && !SOC_IS_ENDURO(unit)) ||
                   soc_feature(unit, soc_feature_register_hi)) {
            SOC_IF_ERROR_RETURN
                (READ_CMIC_MIIM_INT_SEL_MAP_HIr(unit, &regval));
            regval |= (1 << (port_bit - 32));
            SOC_IF_ERROR_RETURN
                (WRITE_CMIC_MIIM_INT_SEL_MAP_HIr(unit, regval));
        }
    }

    SOC_IF_ERROR_RETURN
        (_soc_linkscan_hw_map_init(unit, port));

    return SOC_E_NONE;
}
#endif /* BCM_XGS3_SWITCH_SUPPORT || BCM_SIRIUS_SUPPORT */

#if defined(BCM_ESW_SUPPORT)
STATIC int
_soc_esw_linkscan_hw_link_get(int unit, soc_pbmp_t *hw_link)
{
    uint32              link_stat;
    uint32              link_pbmp;
    soc_pbmp_t          tmp_pbmp;

    if (NULL == hw_link) {
        return SOC_E_PARAM;
    }

    SOC_PBMP_CLEAR(tmp_pbmp);
     /*
      * Read CMIC link status to determine which ports that
      * actually need to be scanned.
      */
    if (SOC_IS_TUCANA(unit)) { 
        /* Tucana ports are split between two module IDs */
        SOC_IF_ERROR_RETURN
            (READ_CMIC_LINK_STAT_MOD0r(unit, &link_stat));
        SOC_PBMP_WORD_SET(tmp_pbmp, 0,
                          soc_reg_field_get(unit, CMIC_LINK_STAT_MOD0r,
                                            link_stat, PORT_BITMAPf));

        SOC_IF_ERROR_RETURN
            (READ_CMIC_LINK_STAT_MOD1r(unit, &link_stat));
        SOC_PBMP_WORD_SET(tmp_pbmp, 1,
                          soc_reg_field_get(unit, CMIC_LINK_STAT_MOD1r,
                                            link_stat, PORT_BITMAPf));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_CMIC_LINK_STATr(unit, &link_stat));

#if defined(BCM_FIREBOLT_SUPPORT)
        if (soc_feature(unit, soc_feature_status_link_fail)) {
            uint32  intsel_reg;

            SOC_IF_ERROR_RETURN
                (READ_CMIC_MIIM_INT_SEL_MAPr(unit, &intsel_reg));
            link_stat ^= intsel_reg;
        }
#endif /* BCM_FIREBOLT_SUPPORT */

        link_pbmp = soc_reg_field_get(unit, CMIC_LINK_STATr,
                                            link_stat, PORT_BITMAPf);

#if defined (BCM_GOLDWING_SUPPORT)
        if (SOC_IS_GOLDWING(unit)) {
            /* (MSB) 15-14-19-18-17-16-13-12-11-10-9-8-7-6-5-4-3-2-1-0 (LSB) */
            link_pbmp =  (link_pbmp & 0x00003FFF) |
                        ((link_pbmp & 0x000C0000) >> 4) |
                        ((link_pbmp & 0x0003C000) << 2);
        }
#endif /* BCM_GOLDWING_SUPPORT */

#if defined (BCM_SCORPION_SUPPORT)
        if (SOC_IS_SC_CQ(unit)) {
            /* CMIC port not included in link status */
            link_pbmp <<=  1;
        }
#endif /* BCM_SCORPION_SUPPORT */

        SOC_PBMP_WORD_SET(tmp_pbmp, 0, link_pbmp);

#if defined (BCM_RAPTOR_SUPPORT) || defined (BCM_TRIUMPH_SUPPORT)
        /* Check for more than 32 ports per unit */
        if ((SOC_IS_TR_VL(unit) && !SOC_IS_ENDURO(unit)) ||
            soc_feature(unit, soc_feature_register_hi)) {
            SOC_IF_ERROR_RETURN
                (READ_CMIC_LINK_STAT_HIr(unit, &link_stat));
            SOC_PBMP_WORD_SET(tmp_pbmp, 1,
                              soc_reg_field_get(unit, CMIC_LINK_STATr,
                                                link_stat, PORT_BITMAPf));
        }
#endif
    }

    SOC_PBMP_ASSIGN(*hw_link, tmp_pbmp);

    return SOC_E_NONE;
}
#endif /* BCM_ESW_SUPPORT */

int
soc_linkscan_hw_init(int unit)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT) || defined(BCM_SIRIUS_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit) || SOC_IS_SIRIUS(unit)) {
        soc_port_t port;
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            SOC_IF_ERROR_RETURN
                (_soc_linkscan_hw_port_init(unit, port));
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT | BCM_SIRIUS_SUPPORT */

    return SOC_E_NONE;
}

int
soc_linkscan_hw_link_get(int unit, soc_pbmp_t *hw_link)
{
    int rv;

    rv = SOC_E_UNAVAIL;

#if defined(BCM_ESW_SUPPORT)
    rv = _soc_esw_linkscan_hw_link_get(unit, hw_link);
#endif /* BCM_ESW_SUPPORT */ 

    return rv;
}
