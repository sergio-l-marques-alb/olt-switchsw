/*----------------------------------------------------------------------
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : dino_cfg_seq.h
 * Description: c functions implementing Tier1
 *---------------------------------------------------------------------*/
/*
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
 *  $Id$
*/


#ifndef DINO_CFG_SEQ_H 
#define DINO_CFG_SEQ_H
#include <phymod/phymod_acc.h>
#include "dino_serdes/common/srds_api_err_code.h"

#define DEV1_SLICE_SLICE_ADR              0x8000
#define DINO_DEV_PMA_PMD                  1
#define DINO_DEV_AN                       7
#define DINO_FW_DLOAD_RETRY_CNT           50000
#define DINO_FW_ENABLE_RETRY_CNT          100
#define DINO_ENABLE                       1
#define DINO_DISABLE                      0
#define DINO_SPD_100G                     100000
#define DINO_SPD_106G                     106000
#define DINO_SPD_1G                       1000
#define DINO_SPD_10G                      10000
#define DINO_SPD_11G                      11000
#define DINO_SPD_20G                      20000
#define DINO_SPD_21G                      21000
#define DINO_SPD_40G                      40000
#define DINO_SPD_42G                      42000
#define DINO_MAX_FALCON_LANE              4
#define DINO_MAX_MERLIN_LANE              4
#define DINO_MAX_CORE_LANE                4
#define DINO_MAX_LANE                     12 
#define DINO_FW_ALREADY_DOWNLOADED        0xFAD
#define DINO_PMD_ID0                      0xAE02
#define DINO_PMD_ID1                      0x5390
#define DINO_CHIP_ID_82332                0x82332
#define DINO_CHIP_ID_82793                0x82793
#define DINO_CHIP_ID_82795                0x82795
#define DINO_FW_HEADER_SIZE               32
#define DINO_40G_PORT0_LANE_MASK          0xf
#define DINO_40G_PORT1_LANE_MASK          0xf0
#define DINO_40G_PORT2_LANE_MASK          0xf00
#define DINO_100G_TYPE1_LANE_MASK         0x3ff
#define DINO_100G_TYPE2_LANE_MASK         0x7fe
#define DINO_100G_TYPE3_LANE_MASK         0xffc
#define DINO_LANE_MASK_ALL_LANES          0xfff
#define DINO_GPREG_0_ADR                  0x8250
#define DINO_SW_GPREG_0_ADR               0x8b90
#define DINO_MER_PLL_MAP_VAL_REG_ADR      0x8b9f
#define DINO_LINE_INTF_MASK               0xf
#define DINO_LINE_INTF_SHIFT              0
#define DINO_SYS_INTF_MASK                0xf0
#define DINO_SYS_INTF_SHIFT               4
#define DINO_DEV_OP_MODE_MASK             0xf00
#define DINO_DEV_OP_MODE_SHIFT            8
#define DINO_DEV_OP_MODE_PT               0
#define DINO_DEV_OP_MODE_GBOX             1
#define DINO_DEV_OP_MODE_INVALID          0xf
#define DINO_PHY_TYPE_MASK                0xf000
#define DINO_PHY_TYPE_SHIFT               12
#define DINO_PHY_TYPE_ETH                 0
#define DINO_PHY_TYPE_HIGIG               1
#define DINO_PHY_TYPE_INVALID             0xf
#define DINO_INTF_TYPE_SR                 0
#define DINO_INTF_TYPE_LR                 1
#define DINO_INTF_TYPE_ER                 2
#define DINO_INTF_TYPE_CAUI4              3
#define DINO_INTF_TYPE_CAUI               4
#define DINO_INTF_TYPE_CAUI4_C2C          5
#define DINO_INTF_TYPE_CAUI4_C2M          6
#define DINO_INTF_TYPE_XLAUI              7
#define DINO_INTF_TYPE_XFI                8
#define DINO_INTF_TYPE_KR                 9
#define DINO_INTF_TYPE_SFI                10
#define DINO_INTF_TYPE_XLPPI              11
#define DINO_INTF_TYPE_CR                 12
#define DINO_INTF_TYPE_VSR                13
#define DINO_INTF_TYPE_INVALID            0xf
#define DINO_LINE_DFE_OPTION_MASK         0xc00
#define DINO_LINE_DFE_OPTION_SHIFT        10
#define DINO_LINE_MEDIA_TYPE_MASK         0x300
#define DINO_LINE_MEDIA_TYPE_SHIFT        8
#define DINO_SYS_DFE_OPTION_MASK         0xc
#define DINO_SYS_DFE_OPTION_SHIFT        2
#define DINO_SYS_MEDIA_TYPE_MASK         0x3
#define DINO_SYS_MEDIA_TYPE_SHIFT        0 
#define DINO_FW_SM_ENABLE_MASK           (1 << 7)
typedef enum {
	MSGOUT_DONTCARE      = 0x0000,
	MSGOUT_GET_CNT	     = 0x8888,
	MSGOUT_GET_LSB	     = 0xABCD,
	MSGOUT_GET_MSB	     = 0x4321,
	MSGOUT_GET_2B	     = 0xEEEE,
	MSGOUT_GET_B	     = 0xF00D,
	MSGOUT_ERR		     = 0x0BAD,
	MSGOUT_NEXT		     = 0x2222, 
	MSGOUT_NOT_DWNLD     = 0x0101, 
	MSGOUT_DWNLD_ALREADY = 0x0202,
	MSGOUT_DWNLD_DONE    = 0x0303,
	MSGOUT_PRGRM_DONE    = 0x0404,
    MSGOUT_HDR_ERR       = 0x0E0E,
    MSGOUT_FLASH         = 0xF1AC
} DINO_MSGOUT_E;

typedef enum {
    DINO_SLICE_UNICAST    = 0,
    DINO_SLICE_MULTICAST  = 1,
    DINO_SLICE_BROADCAST  = 2,
    DINO_SLICE_RESET      = 3,
    DINO_CAST_INVALID     = 4
}DINO_SLICE_OP_TYPE;

typedef enum {
    DINO_IF_LINE = 0,
    DINO_IF_SYS  = 1
}DINO_IF_SIDE;

typedef enum {
    DINO_AN_NONE = 0,
    DINO_AN_CL73 = 1,
    DINO_AN_PROP = 2
}DINO_AN_MODE;

typedef enum 
{
    SERDES_MEDIA_TYPE_BACK_PLANE             = 0,  /* KR link: back plane media */
    SERDES_MEDIA_TYPE_COPPER_CABLE           = 1,  /* CR link: direct-attached copper cable */
    SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS   = 2,  /* SR/LR link: optical fiber with reliable LOS detection */
    SERDES_MEDIA_TYPE_OPTICAL_UNRELIABLE_LOS = 3   /* SR/LR link: optical fiber with unreliable LOS detection */
} DINO_MEDIA_TYPE;

typedef enum 
{
    SERDES_DFE_OPTION_NO_DFE      = 0,  /* no DFE is used */
    SERDES_DFE_OPTION_DFE         = 1,  /* DFE is enabled */
    SERDES_DFE_OPTION_BRDEF       = 2,  /* Force BRDFE mode, DFE will be on, too */
                                        /* only available in Falcon core */
    SERDES_DFE_OPTION_DFE_LP_MODE = 3   /* DFE is enabled in low-power mode */
                                        /* only available in Falcon core */
    /* DFE_LP_MODE and BRDEF are NOT supported in Merlin core,
     * so all non-zero values has the same effect as 1 for Merlin core.
     */
} DINO_DFE_OPTION;


typedef enum {
  DINO_PLL_MODE_64      = 0x0, /*4'b0000*/ 
  DINO_PLL_MODE_66      = 0x1, /*4'b0001*/ 
  DINO_PLL_MODE_80      = 0x2, /*4'b0010*/ 
  DINO_PLL_MODE_128     = 0x3, /*4'b0011*/ 
  DINO_PLL_MODE_132     = 0x4, /*4'b0100*/ 
  DINO_PLL_MODE_140     = 0x5, /*4'b0101*/ 
  DINO_PLL_MODE_160     = 0x6, /*4'b0110*/ 
  DINO_PLL_MODE_165     = 0x7, /*4'b0111*/ 
  DINO_PLL_MODE_168     = 0x8, /*4'b1000*/ 
  DINO_PLL_MODE_170     = 0x9, /*4'b1001*/ 
  DINO_PLL_MODE_175     = 0xa, /*4'b1010*/ 
  DINO_PLL_MODE_180     = 0xb, /*4'b1011*/ 
  DINO_PLL_MODE_184     = 0xc, /*4'b1100*/ 
  DINO_PLL_MODE_200     = 0xd, /*4'b1101*/ 
  DINO_PLL_MODE_224     = 0xe, /*4'b1110*/ 
  DINO_PLL_MODE_264     = 0xf, /*4'b1111*/ 
  DINO_PLL_MODE_33      = 0x10,
  DINO_PLL_MODE_32      = 0x11,
  DINO_PLL_MODE_16      = 0x12,
  DINO_PLL_MODE_82P5    = 0x13,
  DINO_PLL_MODE_40      = 0x14,
  DINO_PLL_MODE_70      = 0x15,
  DINO_PLL_MODE_INVALID = -1
} DINO_PLL_MODE_E;

typedef enum {
    DINO_PORT0    = 0,
    DINO_PORT1    = 1,
    DINO_PORT2    = 2,
    DINO_PORT3    = 3,
    DINO_PORT4    = 4,
    DINO_PORT5    = 5,
    DINO_PORT6    = 6,
    DINO_PORT7    = 7,
    DINO_PORT8    = 8,
    DINO_PORT9    = 9,
    DINO_PORT10   = 10,
    DINO_PORT11   = 11
} DINO_PORT_E;

#define DINO_IF_ERR_RETURN_FREE(B, A)                                           \
    do {                                                                        \
        int loc_err ;                                                           \
        if ((loc_err = (A)) != PHYMOD_E_NONE)                                   \
        {  if (B) { PHYMOD_FREE(B) ; } ;  return loc_err ; }                    \
    } while (0)

#define DINO_IF_ERR_RETURN(A)                                                   \
    do {                                                                        \
        if ((rv = (A)) != PHYMOD_E_NONE)                                        \
        {  goto ERR; }                                                          \
    } while (0)

#define DINO_RETURN_WITH_ERR(A, B)                                              \
    do {                                                                        \
        PHYMOD_DEBUG_ERROR(B);                                                  \
        rv = (A);                                                               \
        { goto ERR; }                                                           \
    } while (0)


#define DINO_GET_INTF_SIDE(phy, if_side)   (if_side = (phy->port_loc == phymodPortLocSys) ? DINO_IF_SYS : DINO_IF_LINE)

#define DINO_GET_PORT_FROM_MODE(_C, _PRT)                                                                    \
    do {                                                                                                               \
        if (_C->data_rate == DINO_SPD_100G || _C->data_rate == DINO_SPD_106G) {                                        \
            _PRT = 1;                                                                                                  \
        } else if ((_C->data_rate == DINO_SPD_40G || _C->data_rate == DINO_SPD_42G)) {          \
            _PRT = 3;                                                                                                  \
        } else if (_C->data_rate == DINO_SPD_10G || _C->data_rate == DINO_SPD_11G || _C->data_rate == DINO_SPD_1G) {   \
            _PRT = 12;                                                                                                 \
        }                                                                                                              \
    }while(0);


#define DINO_IS_FALCON_SIDE(_IF, _LANE)  ((_IF == DINO_IF_LINE) && (_LANE < DINO_MAX_FALCON_LANE))

#define DINO_IS_BCAST_SET(_IF, _LM)                                                                                                 \
        (((_IF == DINO_IF_SYS) &&                                                                                                   \
         ((_LM == 0xf) || (_LM == 0xf0) || (_LM == 0xf00) || (_LM == 0x3ff) || (_LM == 0x7fe) || (_LM == 0xffc))) ||                \
        ((_IF == DINO_IF_LINE) && ((_LM == 0xf) || (_LM == 0xf0) || (_LM == 0xf00))))                                

#define READ_DINO_PMA_PMD_REG(acc, addr, value)                                  \
    do {                                                                         \
        uint32_t read_tmp;                                                       \
        PHYMOD_IF_ERR_RETURN                                                     \
          (PHYMOD_BUS_READ(acc, ((DINO_DEV_PMA_PMD << 16) | addr), &read_tmp));  \
        value = (read_tmp & 0xffff);                                             \
      } while (0);

#define WRITE_DINO_PMA_PMD_REG(acc, addr, value)                                 \
        PHYMOD_IF_ERR_RETURN                                                     \
	        (PHYMOD_BUS_WRITE(acc, (DINO_DEV_PMA_PMD << 16) | addr, value));

#define READ_DINO_AN_REG(acc, addr, value)                                       \
    do {                                                                         \
        uint32_t an_read_tmp;                                                    \
        PHYMOD_IF_ERR_RETURN                                                     \
          (PHYMOD_BUS_READ(acc, ((DINO_DEV_AN << 16) | addr), &an_read_tmp));    \
        value = (an_read_tmp & 0xffff);                                          \
      } while (0);

#define WRITE_DINO_AN_REG(acc, addr, value)                                      \
        PHYMOD_IF_ERR_RETURN                                                     \
	        (PHYMOD_BUS_WRITE(acc, (DINO_DEV_AN << 16) | addr, value));

int dino_get_chipid(const phymod_access_t *pa, uint32_t *chipid, uint32_t *rev);

int _dino_wait_mst_msgout(const phymod_access_t *pa, DINO_MSGOUT_E exp_message, int poll_time);

int _dino_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config);

int _dino_set_slice_reg(const phymod_access_t* pa, DINO_SLICE_OP_TYPE slice_op_type, uint16_t if_side, uint16_t lane); 

int _dino_rx_pmd_lock_get(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, uint32_t* rx_pmd_locked);

int _dino_get_pcs_link_status(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, uint32_t *link_sts);

int _dino_config_dut_mode_reg(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config);

int _dino_udms_config(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config, DINO_AN_MODE mode);

int _dino_phy_interface_config_set(const phymod_phy_access_t *phy, uint32_t flags, const phymod_phy_inf_config_t* config);

int dino_is_lane_mask_set(const phymod_phy_access_t *phy, uint16_t prt, const phymod_phy_inf_config_t *config);

int _dino_config_dut_mode_reg(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config);

int _dino_udms_config(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config, DINO_AN_MODE mode);

int _dino_interface_set(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config);

int _dino_phy_interface_config_set(const phymod_phy_access_t* pa, uint32_t flags, const phymod_phy_inf_config_t* config);

int _dino_phy_interface_config_get(phymod_phy_access_t *phy, uint32_t flags, phymod_phy_inf_config_t *config);

int _dino_configure_ref_clock(const phymod_access_t *pa, phymod_ref_clk_t ref_clk);

int _dino_get_pll_modes(const phymod_phy_access_t *phy, phymod_ref_clk_t ref_clk, const phymod_phy_inf_config_t *config, uint16_t *fal_pll_mode, uint16_t *mer_pll_mode);

int _dino_map_mer_pll_div(DINO_PLL_MODE_E pll_mode);

int _dino_config_pll_div(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t *config);

int _dino_core_reset_set(const phymod_access_t *pa, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction);

int _dino_fw_enable(const phymod_access_t *pa);

int _dino_save_interface(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t *config, phymod_interface_t intf);

int _dino_restore_interface(phymod_phy_access_t *phy, const phymod_phy_inf_config_t *config, phymod_interface_t *intf);

int _dino_loopback_set(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, phymod_loopback_mode_t loopback, uint32_t enable);

int _dino_falcon_lpbk_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_loopback_mode_t loopback, uint32_t enable);

int _dino_merlin_lpbk_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_loopback_mode_t loopback, uint32_t enable);

int _dino_falcon_lpbk_get(const phymod_access_t *pa, uint16_t if_side, phymod_loopback_mode_t loopback, uint32_t* enable);

int _dino_merlin_lpbk_get(const phymod_access_t *pa, uint16_t if_side, phymod_loopback_mode_t loopback, uint32_t* enable);

int _dino_loopback_get(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, phymod_loopback_mode_t loopback, uint32_t *enable);

int _dino_phy_status_dump(const phymod_access_t *pa, uint16_t if_side, uint16_t lane);

int _dino_port_from_lane_map_get(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* cfg, uint16_t *port);

err_code_t _dino_merlin_read_pll_range(const phymod_access_t *pa, uint32_t *pll_range);

err_code_t _dino_falcon_read_pll_range(const phymod_access_t *pa, uint32_t *pll_range);

int _dino_restore_phy_type(const phymod_access_t *pa, phymod_phy_inf_config_t* config, uint16_t *phy_type); 

int _dino_save_phy_type(const phymod_access_t *pa, const phymod_phy_inf_config_t *config); 

int _dino_fifo_reset(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t *config);

int _dino_phy_polarity_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, uint16_t tx_polarity, uint16_t rx_polarity);

int _dino_phy_polarity_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, uint16_t *tx_polarity, uint16_t *rx_polarity);

int _dino_phy_fec_set(const phymod_phy_access_t* phy, uint16_t enable);

int _dino_phy_fec_get(const phymod_phy_access_t* phy, uint32_t *enable);

int _dino_phy_power_set(const phymod_phy_access_t* phy, uint16_t if_side, uint16_t lane, const phymod_phy_power_t* power);

int _dino_phy_power_get(const phymod_phy_access_t* phy, uint16_t if_side, uint16_t lane, phymod_phy_power_t* power);

int _dino_phy_reset_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, const phymod_phy_reset_t* reset);

int _dino_phy_reset_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_phy_reset_t* reset);

int _dino_core_pll_sequencer_restart(const phymod_access_t* pa, uint16_t if_side, phymod_sequencer_operation_t operation);


/*TX transmission control*/
int _dino_phy_tx_lane_control_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_phy_tx_lane_control_t tx_control);
int _dino_phy_tx_lane_control_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_phy_tx_lane_control_t* tx_control);

/*Rx control*/
int _dino_phy_rx_lane_control_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_phy_rx_lane_control_t rx_control);

int _dino_phy_rx_lane_control_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_phy_rx_lane_control_t* rx_control);
#endif /* DINO_CFG_SEQ_H */
