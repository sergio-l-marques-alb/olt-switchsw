/*
 * $Id: macutil.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
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
 * MAC driver support utilities. 
 */

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/ll.h>
#include <soc/error.h>
#include <soc/portmode.h>
#include <soc/macutil.h>
#include <soc/phyctrl.h>

#include <soc/debug.h>

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SIRIUS_SUPPORT)

/*
 * Function:
 *      soc_egress_cell_count
 * Purpose:
 *      Return the approximate number of cells of packets pending
 *      in the MMU destined for a specified egress.
 */
int
soc_egress_cell_count(int unit, soc_port_t port, uint32 *count)
{
    uint32              val;
    int                 cos;

    *count = 0;

    switch (SOC_CHIP_GROUP(unit)) {
    case SOC_CHIP_BCM5690:
    case SOC_CHIP_BCM5695:
    case SOC_CHIP_BCM56504:
    case SOC_CHIP_BCM56102:
    case SOC_CHIP_BCM56304:
    case SOC_CHIP_BCM56218:
    case SOC_CHIP_BCM56112:
    case SOC_CHIP_BCM56314:
    case SOC_CHIP_BCM56514:
    case SOC_CHIP_BCM56224:
    case SOC_CHIP_BCM53314:
       for (cos = 0; cos < NUM_COS(unit); cos++) {
            SOC_IF_ERROR_RETURN(soc_reg_egress_cell_count_get(unit, port, cos, &val));
            *count += val;
        }
        break;
#if defined(BCM_HERCULES_SUPPORT)
    case SOC_CHIP_BCM5670:
    case SOC_CHIP_BCM5675:
        
        break;
#endif /* BCM_HERCULES_SUPPORT */
#if defined(BCM_TUCANA_SUPPORT)
    case SOC_CHIP_BCM5665:
        for (cos = 0; cos < 8; cos++) {
            SOC_IF_ERROR_RETURN(READ_MTC0COSr(unit, port, cos, &val));
            *count += soc_reg_field_get(unit, MTC0COSr, val, CELLCOUNTf);
            SOC_IF_ERROR_RETURN(READ_MTC1COSr(unit, port, cos, &val));
            *count += soc_reg_field_get(unit, MTC1COSr, val, CELLCOUNTf);
        }
        break;
    case SOC_CHIP_BCM5650:
        for (cos = 0; cos < 8; cos++) {
            SOC_IF_ERROR_RETURN(READ_MTC0COSr(unit, port, cos, &val));
            *count += soc_reg_field_get(unit, MTC0COSr, val, CELLCOUNTf);
        }
        break;
#endif /* BCM_TUCANA_SUPPORT */
#if defined(BCM_EASYRIDER_SUPPORT)
    case SOC_CHIP_BCM56601:
        for (cos = 0; cos < 8; cos++) {
            SOC_IF_ERROR_RETURN(READ_MTCCOSr(unit, port, cos, &val));
            *count += soc_reg_field_get(unit, MTCCOSr, val, CELLCOUNTf);
        }
        break;
#endif /* BCM_EASYRIDER_SUPPORT */
#if defined(BCM_BRADLEY_SUPPORT)
    case SOC_CHIP_BCM56800:
    case SOC_CHIP_BCM56820:
    case SOC_CHIP_BCM56725:
        SOC_IF_ERROR_RETURN(READ_OP_PORT_TOTAL_COUNTr(unit, port, &val));
        *count += soc_reg_field_get(unit, OP_PORT_TOTAL_COUNTr, val,
                                    OP_PORT_TOTAL_COUNTf);
        if (IS_GX_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(READ_XP_XBODE_CELL_CNTr(unit, port, &val));
            *count += soc_reg_field_get(unit, XP_XBODE_CELL_CNTr, val,
                                        CELL_CNTf);
            SOC_IF_ERROR_RETURN(READ_GE_GBODE_CELL_CNTr(unit, port, &val));
            *count += soc_reg_field_get(unit, GE_GBODE_CELL_CNTr, val,
                                        CELL_CNTf);
        }
        break;
#endif /* BCM_BRADLEY_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
    case SOC_CHIP_BCM56624:
    case SOC_CHIP_BCM56680:
    case SOC_CHIP_BCM56634:
    case SOC_CHIP_BCM56524:
    case SOC_CHIP_BCM56685:
    case SOC_CHIP_BCM56334:
        SOC_IF_ERROR_RETURN(READ_OP_PORT_TOTAL_COUNT_CELLr(unit, port, &val));
        *count += soc_reg_field_get(unit, OP_PORT_TOTAL_COUNT_CELLr, val,
                                   OP_PORT_TOTAL_COUNT_CELLf);
        break;
#endif /* BCM_TRIUMPH_SUPPORT */
    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

int
soc_mmu_flush_enable(int unit, soc_port_t port, int enable) 
{
    uint32 flush_ctrl;

    COMPILER_REFERENCE(flush_ctrl);

    switch (SOC_CHIP_GROUP(unit)) {
#if defined(BCM_RAVEN_SUPPORT)
    case SOC_CHIP_BCM56224:
    case SOC_CHIP_BCM53314:
        SOC_IF_ERROR_RETURN
            (READ_MMUFLUSHCONTROLr(unit, &flush_ctrl));
        flush_ctrl &= ~(0x1 << port);
        flush_ctrl |= enable ? (0x1 << port) : 0;
        SOC_IF_ERROR_RETURN
            (WRITE_MMUFLUSHCONTROLr(unit, flush_ctrl));
        break;
#endif /* BCM_RAVEN_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT)
    case SOC_CHIP_BCM56634:
    case SOC_CHIP_BCM56524:
    case SOC_CHIP_BCM56685:
    case SOC_CHIP_BCM56334:
        SOC_IF_ERROR_RETURN
            (READ_FAST_TX_FLUSHr(unit, port, &flush_ctrl));
        soc_reg_field_set(unit, FAST_TX_FLUSHr, &flush_ctrl, IDf,
                          (enable) ? 1 : 0);
        SOC_IF_ERROR_RETURN
            (WRITE_FAST_TX_FLUSHr(unit, port, flush_ctrl)); 
    /* Fall through */
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    case SOC_CHIP_BCM56624:
    case SOC_CHIP_BCM56680:
        if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
                (READ_XP_EGR_PKT_DROP_CTLr(unit, port, &flush_ctrl));
            soc_reg_field_set(unit, XP_EGR_PKT_DROP_CTLr, &flush_ctrl, FLUSHf,
                              (enable) ? 1 : 0);
            SOC_IF_ERROR_RETURN
                (WRITE_XP_EGR_PKT_DROP_CTLr(unit, port, flush_ctrl)); 
        }
        break;
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_SCORPION_SUPPORT)
    case SOC_CHIP_BCM56820:
    case SOC_CHIP_BCM56725:
        if (IS_GX_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
                (READ_XP_EGR_PKT_DROP_CTLr(unit, port, &flush_ctrl));
            soc_reg_field_set(unit, XP_EGR_PKT_DROP_CTLr, &flush_ctrl, FLUSHf,
                              (enable) ? 1 : 0);
            SOC_IF_ERROR_RETURN
                (WRITE_XP_EGR_PKT_DROP_CTLr(unit, port, flush_ctrl)); 
        }
        break;
#endif /* BCM_TRIUMPH_SUPPORT || BCM_SCORPION_SUPPORT */

    default:
        return SOC_E_NONE;
    }
    return SOC_E_NONE; 
}

int
soc_port_blk_init(int unit, soc_port_t port)
{
    switch (SOC_CHIP_GROUP(unit)) {
#if defined(BCM_RAVEN_SUPPORT)
    case SOC_CHIP_BCM56224:
        if (IS_S_PORT(unit, port)) {
            soc_pbmp_t pbmp_s0, pbmp_s1, pbmp_s3, pbmp_s4;
            uint32     val32;
            int        higig_mode;

            SOC_IF_ERROR_RETURN(READ_GPORT_CONFIGr(unit, port, &val32));
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32, CLR_CNTf, 1);
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32, GPORT_ENf, 1);
            SOC_PBMP_WORD_SET(pbmp_s0, 0, 0x00000002);
            SOC_PBMP_WORD_SET(pbmp_s1, 0, 0x00000004);
            SOC_PBMP_WORD_SET(pbmp_s3, 0, 0x00000010);
            SOC_PBMP_WORD_SET(pbmp_s4, 0, 0x00000020);
 
            higig_mode = IS_ST_PORT(unit, port) ? 1 : 0;
            if (SOC_PBMP_MEMBER(pbmp_s0, port)) {
                /* The "SOP check enables" are not gated with "HiGig2 enables"
                 * and "bond_disable_stacking".
                 * Thus software need to enable/disable the SOP drop check
                 * in HiGig2 mode as desired.
                 */
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  DROP_ON_WRONG_SOP_EN_S0f, 0);
                /* Enable HiGig 2 */
                /* Assuming that always use stacking port in HiGig mode */
                /* Actually, stacking port can also be used in ethernet mode */
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  HGIG2_EN_S0f, higig_mode); 
            } else if (SOC_PBMP_MEMBER(pbmp_s1, port)) {
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  DROP_ON_WRONG_SOP_EN_S1f, 0);
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  HGIG2_EN_S1f, higig_mode);
            } else if (SOC_PBMP_MEMBER(pbmp_s3, port)) {
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  DROP_ON_WRONG_SOP_EN_S3f, 0);
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  HGIG2_EN_S3f, higig_mode); 
            } else if (SOC_PBMP_MEMBER(pbmp_s4, port)) {
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  DROP_ON_WRONG_SOP_EN_S4f, 0);
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  HGIG2_EN_S4f, higig_mode); 
            }

            SOC_IF_ERROR_RETURN
                (WRITE_GPORT_CONFIGr(unit, port, val32));

            /* Reset the clear-count bit after 64 clocks */
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32, CLR_CNTf, 0);
            SOC_IF_ERROR_RETURN
                (WRITE_GPORT_CONFIGr(unit, port, val32));
        }
        break;
#endif /* BCM_RAVEN_SUPPORT */
    default:
        return SOC_E_NONE;
    }
    return SOC_E_NONE;
}

int
soc_packet_purge_control_init(int unit, soc_port_t port)
{
    uint32 mask;
    mask = soc_property_port_get(unit, port, spn_GPORT_RSV_MASK, 0x070);
    switch (SOC_CHIP_GROUP(unit)) {
#if defined(BCM_RAVEN_SUPPORT)
    case SOC_CHIP_BCM56224:
    case SOC_CHIP_BCM53314:
        /* GPORT_RSV_MASK fields
         * bit
         * 0     frm_align_err_latch
         * 1     rx_frm_stack_vlan_latch
         * 2     rx_carrier_event_latch
         * 3     rx_frm_gmii_err_latch
         * 4     (frm_crc_err_latch | frm_crc_err)
         * 5     frm_length_err_latch & ~frm_truncate_latch
         * 6     frm_truncate_latch
         * 7     ~rx_frm_err_latch & ~frm_truncate_latch
         * 8     rx_frm_mltcast_latch
         * 9     rx_frm_broadcast_latch
         * 10    drbl_nbl_latch
         * 11    cmd_rcv_latch
         * 12    pause_rcv_latch
         * 13    rx_cmd_op_err_latch
         * 14    rx_frm_vlan_latch
         * 15    rx_frm_unicast_latch
         * 16    frm_truncate_latch
         */
        SOC_IF_ERROR_RETURN
            (WRITE_GPORT_RSV_MASKr(unit, port, mask));
        SOC_IF_ERROR_RETURN
            (WRITE_GPORT_STAT_UPDATE_MASKr(unit, port, mask));
        break;
#endif /* BCM_RAVEN_SUPPORT */
#ifdef BCM_TRX_SUPPORT
    case SOC_CHIP_BCM56624:
    case SOC_CHIP_BCM56680:
    case SOC_CHIP_BCM56634:
    case SOC_CHIP_BCM56524:
    case SOC_CHIP_BCM56685:
    case SOC_CHIP_BCM56820:
    case SOC_CHIP_BCM56725:
    case SOC_CHIP_BCM56334:
        SOC_IF_ERROR_RETURN
            (WRITE_GPORT_RSV_MASKr(unit, port, mask));
        SOC_IF_ERROR_RETURN
            (WRITE_GPORT_STAT_UPDATE_MASKr(unit, port, mask));
        break;
#endif
    default:
        return SOC_E_NONE;
    }

    return SOC_E_NONE;
}

int
soc_egress_enable(int unit, soc_port_t port, int enable)
{
    uint64 val64;

    COMPILER_REFERENCE(val64);

    switch (SOC_CHIP_GROUP(unit)) {
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
    case SOC_CHIP_BCM56820:
    case SOC_CHIP_BCM56224:
    case SOC_CHIP_BCM53314:
        SOC_IF_ERROR_RETURN(READ_EGR_ENABLEr(unit, port, &val64));
        soc_reg64_field32_set(unit, EGR_ENABLEr, &val64, PRT_ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, val64));
        break;
#endif /* BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT */
    default:
        return SOC_E_NONE;
    }
    return SOC_E_NONE;
}
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SIRIUS_SUPPORT) */

