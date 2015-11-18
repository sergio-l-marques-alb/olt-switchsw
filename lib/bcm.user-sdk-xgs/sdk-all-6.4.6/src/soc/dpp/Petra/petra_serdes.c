/* $Id: soc_petra_serdes.c,v 1.8 Broadcom SDK $
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
#include<soc/dpp/SAND/Utils/sand_header.h>
#include<soc/dpp/Petra/petra_serdes.h>
#include<soc/dpp/Petra/petra_serdes_regs.h>
#include<soc/dpp/Petra/petra_serdes_utils.h>
#include<soc/dpp/Petra/petra_api_serdes_utils.h>
#include<soc/dpp/Petra/petra_reg_access.h>
#include<soc/dpp/Petra/petra_api_mgmt.h>
#include<soc/dpp/Petra/petra_serdes_low_level_access.h>
#include<soc/dpp/Petra/petra_sw_db.h>
#include<soc/dpp/Petra/petra_general.h>
#include<soc/dpp/Petra/petra_fabric.h>
#include<soc/dpp/SAND/Utils/sand_bitstream.h>
#include<soc/dpp/SAND/Utils/sand_u64.h>
#include<soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include<soc/dpp/SAND/Utils/sand_workload_status.h>
#include<soc/dpp/SAND/Management/sand_low_level.h>
#define lc rate
#define l136 SOC_PETRA_SRD_IS_RATE_ENUM
#define l134 SOC_PETRA_SRD_DATA_RATE_3125_00_FDR
#define l321 SOC_PETRA_SRD_RATE_RNG_BELOW_3200
#define l181 SOC_PETRA_SRD_RATE_RNG_ABOVE_3200
#define l59 enable
#define lg if
#define l201 soc_petra_srd_brdcst_all_lanes_set
#define ld unit
#define l226 var
#define l493 type
#define l9 TRUE
#define ln info
#define l24 lane_conf
#define l22 for
#define l74 SOC_PETRA_SRD_NOF_LANES_PER_QRTT
#define l747 sal_memcmp
#define l718 sizeof
#define lt FALSE
#define l311 continue
#define l602 typedef
#define l761 enum
#define l356 SOC_PETRA_SRD_SLEW_MODE_NORMAL
#define l499 SOC_PETRA_SRD_SLEW_MODE_MODERATED
#define l689 SOC_PETRA_SRD_NOF_SLEW_MODES
#define l423 SOC_PETRA_SRD_SLEW_MODE
#define l45 static
#define l349 const
#define l141 SOC_PETRA_SRD_TX_PHYS_INTERNAL
#define l765 SOC_PETRA_SRD_NOF_RATE_RNGS
#define l338 SOC_PETRA_SRD_NOF_MEDIA_TYPES
#define le uint32
#define l412 soc_petra_serdes_regs_init
#define lm SOC_SAND_IN
#define l12 SOC_SAND_INIT_ERROR_DEFINITIONS
#define l752 SOC_PETRA_SERDES_REGS_INIT
#define l127 SOC_PETRA_DO_NOTHING_AND_EXIT
#define lb exit
#define l15 SOC_SAND_EXIT_AND_SEND_ERROR
#define l726 soc_petra_serdes_init
#define la res
#define l114 SOC_SAND_OK
#define l596 SOC_PETRA_SERDES_INIT
#define lh SOC_SAND_CHECK_FUNC_RESULT
#define l240 void
#define l11 soc_petra_srd_rate_internal_clear
#define l40 SOC_SAND_OUT
#define l85 SOC_PETRA_SRD_RATE_INTERNAL
#define l29 NULL
#define l2 pll_m
#define l3 pll_n
#define ls dvsr
#define l239 SOC_PETRA_SRD_NOF_RATE_DIVISORS
#define lv uint8
#define l216 soc_petra_srd_rate_internal_is_valid
#define l297 is_valid
#define lu else
#define l28 return
#define l282 soc_petra_srd_divisors2rate
#define l138 SOC_PETRA_SRD_RATE_DIVISOR
#define l1 switch
#define lk case
#define l36 SOC_PETRA_SRD_RATE_DIVISOR_QUARTER
#define lj break
#define l26 SOC_PETRA_SRD_RATE_DIVISOR_HALF
#define l27 SOC_PETRA_SRD_RATE_DIVISOR_FULL
#define l4 default
#define l143 SOC_SAND_DIV_ROUND
#define l528 soc_petra_srd_find_divisor
#define l176 soc_petra_srd_internal_rate_calc
#define l638 SOC_PETRA_SRD_INTERNAL_RATE_CALC
#define l49 SOC_SAND_CHECK_NULL_INPUT
#define l330 SOC_PETRA_MGMT_IS_SRD_REF_CLK_ENUM
#define l126 int
#define l19 SOC_SAND_SET_ERROR_CODE
#define l639 SOC_PETRA_RATE_CONF_MODE_INCONSISTENT_ERR
#define l525 SOC_PETRA_MGMT_SRD_REF_CLK_125_00
#define l533 SOC_PETRA_SRD_DATA_RATE_1250_00
#define l408 SOC_PETRA_SRD_DATA_RATE_2500_00
#define l119 SOC_PETRA_SRD_DATA_RATE_3125_00
#define l166 SOC_PETRA_SRD_DATA_RATE_1562_50
#define l511 SOC_PETRA_SRD_DATA_RATE_3750_00
#define l498 SOC_PETRA_SRD_DATA_RATE_5000_00
#define l448 SOC_PETRA_SRD_DATA_RATE_5833_33
#define l171 SOC_PETRA_SRD_DATA_RATE_6250_00
#define l501 SOC_PETRA_MGMT_SRD_REF_CLK_156_25
#define l145 SOC_PETRA_SRD_DATA_RATE_1041_67
#define l139 SOC_PETRA_SRD_DATA_RATE_1171_88
#define l182 SOC_PETRA_SRD_DATA_RATE_2083_33
#define l233 SOC_PETRA_SRD_DATA_RATE_4166_67
#define l172 SOC_PETRA_SRD_DATA_RATE_2343_75
#define l278 SOC_PETRA_SRD_DATA_RATE_4687_50
#define l427 SOC_PETRA_SRD_DATA_RATE_5208_33
#define l733 SOC_PETRA_SRD_DATA_RATE_1302_03
#define l665 SOC_PETRA_SRD_DATA_RATE_2604_16
#define l512 SOC_PETRA_SRD_DATA_RATE_5468_75
#define l456 SOC_PETRA_MGMT_SRD_REF_CLK_200_00
#define l379 SOC_PETRA_SRD_DATA_RATE_1000_00
#define l333 SOC_PETRA_SRD_DATA_RATE_2666_67
#define l400 SOC_PETRA_SRD_DATA_RATE_3000_00
#define l417 SOC_PETRA_SRD_DATA_RATE_4000_00
#define l548 SOC_PETRA_SRD_DATA_RATE_5333_33
#define l522 SOC_PETRA_SRD_DATA_RATE_6000_00
#define l564 SOC_PETRA_MGMT_SRD_REF_CLK_312_50
#define l482 SOC_PETRA_MGMT_SRD_REF_CLK_218_75
#define l446 SOC_PETRA_SRD_DATA_RATE_4375_00
#define l588 SOC_PETRA_MGMT_SRD_REF_CLK_212_50
#define l495 SOC_PETRA_SRD_DATA_RATE_4250_00
#define l683 SOC_PETRA_SRD_INVALID_REF_CLOCK_ERR
#define l210 soc_petra_srd_internal_rate_get
#define li lane_ndx
#define lp qrtt_id
#define l66 SOC_PETRA_REGS
#define lq regs
#define l732 SOC_PETRA_SRD_INTERNAL_RATE_GET
#define l68 soc_petra_regs
#define l173 SOC_PETRA_SRD_LANE2QRTT_INNER
#define l208 SOC_PETRA_SRD_LANE2INNER_ID
#define l301 SOC_PETRA_SRD_LANE2INSTANCE
#define l76 SOC_PETRA_FLD_IGET
#define l6 serdes
#define l52 srd_cmu_cfga_reg
#define l391 cmu_pllm
#define l370 cmu_plln
#define l81 srd_ln_cfga_reg
#define l465 ln_tx_rate
#define l366 soc_petra_srd_rate_get_and_validate
#define l648 SOC_PETRA_ERR
#define l716 SOC_PETRA_SRD_RATE_GET_AND_VALIDATE
#define l224 soc_petra_sw_db_srd_qrtt_max_rate_get
#define l159 SOC_PETRA_SRD_LANE2QRTT_GLBL
#define l324 soc_petra_srd_rate_get_unsafe
#define l187 SOC_PETRA_SRD_DATA_RATE_NONE
#define l180 SOC_PETRA_MGMT_SRD_REF_CLK
#define l170 soc_petra_srd_ref_clk_get
#define l649 SOC_PETRA_SRD_IS_NIF_LANE
#define l663 soc_petra_sw_db_ref_clocks_nif_get
#define l568 SOC_PETRA_SRD_IS_FABRIC_LANE
#define l589 soc_petra_sw_db_ref_clocks_fabric_get
#define l481 SOC_PETRA_SRD_IS_COMBO_LANE
#define l624 soc_petra_sw_db_ref_clocks_combo_get
#define l662 SOC_PETRA_MGMT_NOF_SRD_REF_CLKS
#define l546 soc_petra_srd_cmu_trim_start
#define l58 SOC_PETRA_SRD_REGS
#define l229 SOC_PETRA_SRD_CMU_TRIM
#define l57 soc_petra_srd_regs
#define l163 SOC_PETRA_SRD_QRTT2INNER_ID
#define l295 SOC_PETRA_SRD_QRTT2INSTANCE
#define l88 SOC_PETRA_SRD_LANE2GLOBAL_ID
#define l279 SOC_PETRA_SRD_QRTT2STAR_ID
#define l5 soc_petra_srd_fld_write_unsafe
#define lr SOC_PETRA_SRD_ENTITY_TYPE_LANE
#define ll lane
#define l10 sm_control
#define l373 ctrl2
#define l359 trimdone
#define l79 rx
#define l334 rxlsppm
#define l362 rclkrls
#define l32 SOC_PETRA_FLD_ISET
#define l576 cmu_start_trim
#define l365 soc_petra_srd_lane_trim_validate_done_internal
#define l291 soc_sand_low_is_sim_active_get
#define l31 soc_petra_srd_fld_read_unsafe
#define l736 SOC_PETRA_SRD_LN_TRIM_DONE_IS_DOWN_ERR
#define l769 SOC_PETRA_SRD_LN_CLCK_RLS_IS_DOWN_ERR
#define l157 rx_hs
#define l630 rxhsstatus
#define l591 rxtdacdone
#define l618 SOC_PETRA_SRD_LN_CLCK_RXTDACDONE_IS_DOWN_ERR
#define l168 ex
#define l518 soc_petra_srd_lane_trim_validate_done
#define l262 SOC_PETRA_SRD_REGS_ADDR
#define l34 addr
#define l209 do
#define l520 soc_sand_update_error_code
#define l551 no_err
#define l355 SOC_PETRA_SRD_TRIM_FAILED_ERR
#define l64 sal_msleep
#define l175 while
#define l572 soc_petra_PETRA_SRD_REGS_ADDR_clear
#define l502 element
#define l430 offset
#define l131 soc_petra_srd_reg_write_unsafe
#define l480 soc_petra_srd_cmu_trim_validate_internal
#define l384 srd_cmu_stat_reg
#define l728 cmu_tx_trimming
#define l677 SOC_PETRA_SRD_TX_TRIMMING_DID_NOT_END_ERR
#define l620 cmu_pll_rst
#define l701 SOC_PETRA_SRD_PLL_RESET_DID_NOT_END_ERR
#define l710 ln_rx_rst
#define l760 SOC_PETRA_SRD_LN_RX_RESET_DID_NOT_END_ERR
#define l555 soc_petra_srd_cmu_trim_validate_done
#define l537 SOC_PETRA_SRD_QRTT_FIRST_LANE
#define l713 SOC_PETRA_SRD_QRTT_LAST_LANE
#define l585 soc_petra_srd_is_qrtt_active
#define l89 is_active
#define l294 cmu_macro_rst
#define l300 SOC_SAND_NUM2BOOL_INVERSE
#define l559 soc_petra_srd_is_any_single_in_mac
#define l153 SOC_PETRA_SRD_NOF_QUARTETS
#define l561 SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac)
#define l693 soc_petra_fbr2srd_lane_id
#define l623 soc_petra_sw_db_is_double_rate_get
#define l477 soc_petra_srd_mac_rate_set
#define l622 SOC_PETRA_SRD_MAC_RATE_SET
#define l734 soc_petra_srd2fbr_link_id
#define l628 SOC_PETRA_FBC_LINK_IN_MAC
#define l755 SOC_PETRA_FBC_MAC_OF_LINK
#define l135 fabric_mac
#define l189 receive_reset_reg
#define l319 fmacrx_rst_n
#define l371 fmactx_rst_n
#define l720 loopback_and_link_level_flow_control_enable_reg
#define l637 double_rate_en
#define l667 soc_petra_sw_db_is_double_rate_set
#define l103 SOC_SAND_NUM2BOOL
#define l698 link_level_flow_control_and_comma_configuration_reg
#define l704 cm_brst_size
#define l343 soc_petra_srd_rate_set_unsafe
#define l685 SOC_PETRA_SRD_RATE_SET_UNSAFE
#define l117 SOC_PETRA_SRD_INVALID_RATE_ERR
#define l396 ln_rx_rate
#define l527 soc_petra_srd_is_fabric_lane
#define l547 soc_petra_srd_rate_verify
#define l709 SOC_PETRA_SRD_RATE_VERIFY
#define l41 SOC_SAND_ERR_IF_ABOVE_MAX
#define l125 SOC_PETRA_SRD_NOF_LANES
#define l237 SOC_PETRA_SRD_LANE_ID_OUT_OF_RANGE_ERR
#define l260 SOC_PETRA_SRD_NOF_DATA_RATES
#define l276 SOC_SAND_ERR_IF_OUT_OF_RANGE
#define l309 SOC_PETRA_SRD_NUM_RATE_KBPS_MIN
#define l351 SOC_PETRA_SRD_NUM_RATE_KBPS_MAX
#define l724 SOC_PETRA_SRD_RATE_GET_UNSAFE
#define l434 SOC_PETRA_SRD_DATA_RATE_FIRST
#define l293 soc_petra_srd_tx_phys_explicit_from_intern
#define l93 SOC_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS
#define l284 SOC_PETRA_SRD_RATE_RNG
#define l200 range
#define l386 SOC_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS
#define l328 int32
#define l20 swing
#define l75 ipre
#define l92 ipost
#define l43 amp
#define l63 main
#define l14 pre
#define l13 post
#define l292 SOC_SAND_IS_VAL_OUT_OF_RANGE
#define l369 SOC_PETRA_SRD_TX_SWING_MV_MIN
#define l364 SOC_PETRA_SRD_TX_SWING_MV_MAX
#define l56 SOC_PETRA_SRD_TX_POST_MAX
#define l268 SOC_PETRA_SRD_PRE_POST_PERCENT_MAX
#define l303 soc_petra_srd_tx_explicit_is_better
#define l194 SOC_PETRA_SRD_TX_EXPLCT_DEVIATION
#define l399 soc_petra_PETRA_SRD_TX_EXPLCT_DEVIATION_clear
#define l483 soc_petra_srd_tx_phys_explicit_to_intern
#define l712 int8
#define l110 imain
#define l556 soc_petra_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS_clear
#define l494 SOC_PETRA_SRD_TX_AMP_MAX
#define l575 SOC_PETRA_SRD_TX_MAIN_MAX
#define l314 SOC_PETRA_SRD_TX_PRE_MAX
#define l188 SOC_SAND_DELTA
#define l479 soc_petra_srd_tx_phys_atten_to_intern
#define l105 atten
#define l299 SOC_PETRA_SRD_TX_ATTEN_X10_MAX
#define l658 SOC_SAND_LIMIT_FROM_ABOVE
#define l444 soc_petra_srd_tx_phys_atten_from_intern
#define l287 soc_petra_srd_tx_params_to_internal
#define l78 params
#define l47 intern
#define l577 soc_petra_srd_tx_params_from_internal
#define l541 soc_petra_srd_tx_phys_params_set_unsafe
#define l352 SOC_PETRA_SRD_TX_PHYS_CONF_MODE
#define l315 SOC_PETRA_SRD_TX_PHYS_INFO
#define l253 SOC_PETRA_SRD_REGS_FIELD
#define l431 SOC_PETRA_SRD_MEDIA_TYPE
#define l130 media_type
#define l678 SOC_PETRA_SRD_TX_PHYS_PARAMS_SET_UNSAFE
#define l656 soc_petra_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS_clear
#define l524 SOC_PETRA_SRD_MISCONFIGURED_RATE_ERR
#define l387 SOC_PETRA_SRD_TX_PHYS_CONF_MODE_MEDIA_TYPE
#define l38 conf
#define l565 SOC_PETRA_SRD_MEDIA_TYPE_CHIP2CHIP
#define l560 SOC_PETRA_COPY
#define l329 SOC_PETRA_SRD_TX_PHYS_CONF_MODE_EXPLICIT
#define l185 explct
#define l627 SOC_PETRA_SRD_TX_EXPLCT_FAIL_ERR
#define l312 SOC_PETRA_SRD_TX_PHYS_CONF_MODE_ATTEN
#define l449 SOC_PETRA_SRD_TX_ATTEN_ABOVE_3_125_FAIL_ERR
#define l612 SOC_PETRA_SRD_TX_ATTEN_FAIL_ERR
#define l357 SOC_PETRA_SRD_TX_PHYS_CONF_MODE_INTERNAL
#define lw tx_hs
#define l587 txqdrctrl1
#define l458 slewqdr
#define l249 txqdrctrl0
#define l439 ampqdr
#define l696 ampqdren
#define l164 txqdrctrl2
#define l269 ipreqdr
#define l184 txqdrctrl3
#define l735 ipreqdren
#define l471 ipstqdr
#define l597 ipstqdren
#define l468 imainqdr
#define l604 imainqdren
#define l443 txhdrctrl1
#define l437 slewhdr
#define l257 txhdrctrl0
#define l584 amphdr
#define l741 amphdren
#define l147 txhdrctrl2
#define l271 iprehdr
#define l169 txhdrctrl3
#define l601 iprehdren
#define l532 ipsthdr
#define l595 ipsthdren
#define l543 imainhdr
#define l730 imainhdren
#define l583 txfdrctrl1
#define l580 slewfdr
#define l267 txfdrctrl0
#define l540 ampfdr
#define l652 ampfdren
#define l165 txfdrctrl2
#define l264 iprefdr
#define l202 txfdrctrl3
#define l729 iprefdren
#define l567 ipstfdr
#define l613 ipstfdren
#define l519 imainfdr
#define l680 imainfdren
#define l467 SOC_PETRA_SRD_RATE_DIVISOR_OUT_OF_RANGE_ERR
#define l86 soc_petra_srd_fld_from_reg_set
#define l671 SOC_SAND_GET_BITS_RANGE
#define l675 SOC_SAND_GET_BIT
#define l462 firlut0
#define l380 tap_low
#define l459 firlut1
#define l377 tap_hi
#define l569 soc_petra_srd_tx_phys_params_verify
#define l617 SOC_PETRA_SRD_TX_PHYS_PARAMS_VERIFY
#define l283 SOC_SAND_MAGIC_NUM_VERIFY
#define l766 SOC_PETRA_SRD_NOF_TX_PHYS_CONF_MODES
#define l590 SOC_PETRA_SRD_TX_CONF_MODE_OUT_OF_RANGE_ERR
#define l664 SOC_PETRA_SRD_MEDIA_TYPE_OUT_OF_RANGE_ERR
#define l388 SOC_PETRA_SRD_EXPLCT_PRE_OUT_OF_RANGE_ERR
#define l634 SOC_PETRA_SRD_ATTEN_OUT_OF_RANGE_ERR
#define l651 SOC_PETRA_SRD_AMP_VAL_OUT_OF_RANGE_ERR
#define l599 SOC_PETRA_SRD_MAIN_VAL_OUT_OF_RANGE_ERR
#define l592 SOC_PETRA_SRD_PRE_EMPHASIS_VAL_OUT_OF_RANGE_ERR
#define l650 SOC_PETRA_SRD_POST_EMPHASIS_VAL_OUT_OF_RANGE_ERR
#define l557 soc_petra_srd_tx_phys_params_get_unsafe
#define l750 SOC_PETRA_SRD_TX_PHYS_PARAMS_GET_UNSAFE
#define l674 SOC_SAND_SET_BITS_RANGE
#define l307 SOC_SAND_SET_BIT
#define l553 tapset
#define l670 SOC_PETRA_COMP
#define l606 SOC_PETRA_SRD_MEDIA_TYPE_NONE
#define l621 SOC_PETRA_SRD_TX_ATTEN_INVALID
#define l133 soc_petra_srd_lane_polarity_set_unsafe
#define l288 SOC_PETRA_CONNECTION_DIRECTION
#define l763 SOC_PETRA_SRD_LANE_POLARITY_SET_UNSAFE
#define l442 SOC_PETRA_IS_DIRECTION_RX
#define l425 SOC_PETRA_IS_DIRECTION_TX
#define l375 SOC_SAND_BOOL2NUM
#define l162 rxlsctrl0
#define l488 rxinvo
#define l492 rxinvf
#define l428 soc_petra_sw_db_srd_orig_swap_polarity_set
#define l258 SOC_PETRA_CONNECTION_DIRECTION_RX
#define l53 tx
#define l120 txlsctrl0
#define l544 txinvo
#define l549 txinvf
#define l323 SOC_PETRA_CONNECTION_DIRECTION_TX
#define l404 soc_petra_srd_lane_polarity_verify
#define l611 SOC_PETRA_SRD_LANE_POLARITY_VERIFY
#define l536 SOC_PETRA_NOF_CONNECTION_DIRECTIONS
#define l539 SOC_PETRA_CONNECTION_DIRECTION_OUT_OF_RANGE_ERR
#define l317 soc_petra_srd_lane_polarity_get_unsafe
#define l155 is_swap_polarity_rx
#define l118 is_swap_polarity_tx
#define l155_orig is_swap_polarity_rx_orig
#define l118_orig is_swap_polarity_tx_orig
#define l751 SOC_PETRA_SRD_LANE_POLARITY_GET_UNSAFE
#define l325 soc_petra_srd_validate_and_relock
#define l762 SOC_PETRA_SRD_PRBS_RX_STATUS
#define l697 SOC_PETRA_SRD_LANE_LOOPBACK_MODE
#define l768 SOC_PETRA_SRD_VALIDATE_AND_RELOCK
#define l717 soc_petra_PETRA_SRD_PRBS_RX_STATUS_clear
#define l731 soc_petra_srd_lane_loopback_mode_get_unsafe
#define l455 soc_petra_srd_lane_loopback_mode_set_unsafe
#define l608 SOC_PETRA_SRD_LANE_LOOPBACK_NSILB
#define l98 SOC_PETRA_CONNECTION_DIRECTION_BOTH
#define l748 soc_petra_srd_prbs_mode_set_unsafe
#define l679 SOC_PETRA_SRD_PRBS_MODE_POLY_7
#define l485 soc_petra_sw_db_srd_is_swap_unhandled_set
#define l686 soc_petra_srd_prbs_start_unsafe
#define l629 soc_petra_srd_prbs_get_and_clear_stat
#define l681 error_cnt
#define l757 prbs_signal_stat
#define l767 SOC_PETRA_SRD_PRBS_SIGNAL_STAT_NOT_LOCKED
#define l756 soc_petra_srd_relock_unsafe
#define l646 soc_petra_srd_prbs_stop_unsafe
#define l152 soc_sand_os_printf
#define l504 soc_petra_srd_lane_power_state_set_unsafe
#define l140 SOC_PETRA_SRD_POWER_STATE
#define l653 SOC_PETRA_SRD_LANE_POWER_STATE_SET_UNSAFE
#define l115 SOC_PETRA_SRD_POWER_STATE_DOWN
#define l252 SOC_PETRA_SRD_POWER_STATE_UP
#define l545 SOC_PETRA_SRD_POWER_STATE_UP_AND_RELOCK
#define l420 SOC_PETRA_SRD_LANE_STATE_OUT_OF_RANGE_ERR
#define l331 rxpwrdn
#define l245 txpwrdn
#define l579 soc_petra_srd_lane_power_state_verify
#define l661 SOC_PETRA_SRD_LANE_POWER_STATE_VERIFY
#define l706 SOC_PETRA_SRD_NOF_POWER_STATES
#define l376 soc_petra_srd_lane_power_state_get_unsafe
#define l633 SOC_PETRA_SRD_LANE_POWER_STATE_GET_UNSAFE
#define l451 soc_petra_srd_star_reset
#define l243 SOC_PETRA_SRD_STAR_ID
#define l353 SOC_PETRA_DEFAULT_INSTANCE
#define l516 SOC_PETRA_SRD_STAR_SET_UNSAFE
#define l256 SOC_PETRA_SRD_STAR2INSTANCE
#define l381 soc_petra_srd_star_nof_qrtts_get
#define l406 SOC_PETRA_REG_IGET
#define l48 srd_ipu_cfg_reg
#define l71 SOC_PETRA_FLD_TO_REG
#define l450 iram_rst
#define l515 rst8051
#define l526 epb_rst
#define l508 ext_scif_rst
#define l600 clk_sel8051
#define l676 ipu_parity_en
#define l672 SOC_PETRA_SRD_CHANNEL_IPU
#define l753 chan_id
#define l754 epb_int_timeout_en
#define l744 clk_pd8051
#define l673 pd8051
#define l405 SOC_PETRA_REG_ISET
#define l688 cmu_trim_self
#define l714 ln_txob_pd
#define l643 cmu_macro_pd
#define l395 soc_petra_srd_ref_clk_to_khz
#define l393 SOC_SAND_INTERN_VAL_INVALID_32
#define l335 soc_petra_srd_ivcdl_set
#define l342 islvgain
#define l619 SOC_PETRA_SRD_IVCDL_SET
#define l603 SOC_SAND_IS_INTERN_VAL_INVALID
#define l684 SOC_PETRA_SRD_CMU_REF_CLK_OUT_OF_RANGE_ERR
#define l759 SOC_PETRA_SRD_RATE_VCO_BELOW_MIN_ERR
#define l626 SOC_PETRA_SRD_RATE_VCO_ABOVE_MAX_ERR
#define l507 vcdldac2
#define l636 ivcdl
#define l566 soc_petra_srd_star_set_unsafe
#define l360 SOC_PETRA_SRD_STAR_INFO
#define l700 soc_petra_srd_ipu_mem_load
#define l121 qrtt
#define l174 max_expected_lane_rate
#define l509 soc_petra_sw_db_srd_qrtt_max_rate_set
#define l266 SOC_PETRA_SRD_QRTT2GLOBAL_ID
#define l206 SOC_PETRA_SRD_ENTITY_TYPE_CMU
#define l215 cmu
#define l73 control
#define l615 cmuversion
#define l694 cmutag
#define l739 SOC_PETRA_SRD_CMU_VER_MISMATCH_ERR
#define l463 soc_petra_srd_reg_read_unsafe
#define l273 SOC_PETRA_SRD_ENTITY_TYPE_IPU
#define l144 ipu
#define l190 mpreg0
#define l255 soc_petra_srd_fld_from_reg_get
#define l616 cefg
#define l764 SOC_PETRA_SRD_8051_CHECKSUM_STILL_RUNNING_ERR
#define l645 ckexed
#define l605 SOC_PETRA_SRD_8051_CHECKSUM_DID_NOT_COMPLETE_ERR
#define l687 ckpassbr
#define l454 SOC_PETRA_SRD_8051_CHECKSUM_FAIL_ERR
#define l707 cksumerr
#define l745 alltrim
#define l424 SOC_PETRA_REG_DB_ACC
#define l476 ln_rx_idle
#define l487 lsb
#define l503 ln_tx_idle
#define l452 beacon
#define l447 txbbctrl
#define l415 rxrateclken
#define l177 prbs
#define l513 genctrl
#define l558 pgudpsel
#define l280 chkctrl
#define l478 togsel
#define l474 pccapture
#define l550 vcdlctrl7
#define l542 disckpat
#define l421 SOC_PETRA_SRD_DISCKPAT_DFLT_VAL
#define l466 rxhsctrl0
#define l414 rxthreshold
#define l419 SOC_PETRA_SRD_RXTHRESHOLD_DFLT_VAL
#define l418 soc_petra_srd_star_verify
#define l644 SOC_PETRA_SRD_STAR_VERIFY
#define l708 SOC_PETRA_SRD_NOF_STAR_IDS
#define l272 SOC_PETRA_SRD_STAR_ID_OUT_OF_RANGE_ERR
#define l719 SOC_PETRA_SRD_NOF_QRTTS_PER_STAR_MAX
#define l570 soc_petra_srd_star_get_unsafe
#define l610 SOC_PETRA_SRD_STAR_GET_UNSAFE
#define l722 SOC_PETRA_SRD_STAR_FIRST_QRTT
#define l461 soc_petra_srd_qrtt_get_unsafe
#define l743 soc_petra_srd_qrtt_set_unsafe
#define l378 SOC_PETRA_SRD_QRTT_INFO
#define l161 is_success
#define l749 SOC_PETRA_POLL_INFO
#define l148 twcsel
#define l149 fifosel
#define l746 SOC_PETRA_SRD_QRTT_SET_UNSAFE
#define l285 cmuctrl0
#define l429 pllrst
#define l472 pllpd
#define l137 txlsctrl1
#define l313 int8051
#define l308 soc_petra_sw_db_srd_is_irq_disable_set
#define l625 soc_petra_PETRA_POLL_INFO_clear
#define l432 expected_value
#define l669 busy_wait_nof_iters
#define l725 timer_nof_iters
#define l703 timer_delay_msec
#define l441 soc_petra_srd_status_fld_poll_unsafe
#define l445 stat1
#define l521 ackint8051
#define l529 SOC_PETRA_SRD_IPU_IRQ_ACK_DOWN_ERR
#define l657 soc_petra_srd_qrtt_verify
#define l738 SOC_PETRA_SRD_QRTT_VERIFY
#define l641 SOC_PETRA_SRD_QRTT_GET_UNSAFE
#define l666 soc_petra_srd_all_set_unsafe
#define l304 SOC_PETRA_SRD_ALL_INFO
#define l635 is_swap_polarity_rx_orig
#define l695 is_swap_polarity_tx_orig
#define l642 SOC_PETRA_SRD_ALL_SET_UNSAFE
#define l336 SOC_PETRA_SRD_STAR_ID_0
#define l318 SOC_PETRA_SRD_NOF_STARS
#define l154 star_conf
#define l217 rate_conf
#define l460 tx_phys_conf_mode
#define l339 tx_phys_conf
#define l435 soc_petra_srd_rx_phys_params_set_unsafe
#define l397 rx_phys_conf
#define l392 power_state_conf
#define l632 soc_petra_srd_all_verify
#define l631 SOC_PETRA_SRD_ALL_VERIFY
#define l538 SOC_PETRA_SRD_LANE2STAR
#define l640 SOC_PETRA_SRD_LANE_AND_QRTT_ENABLE_MISMATCH_ERR
#define l422 soc_petra_srd_rx_phys_params_verify
#define l699 soc_petra_srd_all_get_unsafe
#define l740 SOC_PETRA_SRD_NOF_AEQ_MODES
#define l614 SOC_PETRA_SRD_ALL_GET_UNSAFE
#define l490 soc_petra_srd_rx_phys_params_get_unsafe
#define l721 soc_petra_srd_auto_equalize_unsafe
#define l711 SOC_PETRA_SRD_AEQ_MODE
#define l530 mode
#define l660 SOC_PETRA_SRD_AEQ_STATUS
#define l401 timeout
#define l702 SOC_PETRA_SRD_AUTO_EQUALIZE_UNSAFE
#define l438 rxlsstatus
#define l491 rxlos
#define l453 eye_height_taps
#define l690 SOC_PETRA_SRD_AEQ_MODE_BLIND
#define l609 SOC_PETRA_SRD_AEQ_MODE_STEADY_STATE
#define l655 SOC_PETRA_SRD_AEQ_MODE_INVALID_ERR
#define l97 startupeq1
#define l534 blind
#define l563 steadystate
#define l573 preset
#define l440 eqhltack
#define l473 eqhlt
#define l593 txhsbnctrl0
#define l345 SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS
#define l594 SOC_PETRA_SRD_RX_PHYS_PARAMS_SET_UNSAFE
#define l510 zfr
#define l426 zcntfdr
#define l222 gfr
#define l505 z1cntfdr
#define l270 dfelth
#define l574 dfelthfdr
#define l265 tlth
#define l586 tlthfdr
#define l413 g1cntfdr
#define l500 zhr
#define l484 zcnthdr
#define l246 ghr
#define l517 z1cnthdr
#define l411 dfelthhr
#define l469 dfelthhdr
#define l489 tlthhr
#define l433 tlthhdr
#define l457 g1cnthdr
#define l578 zqr
#define l531 zcntqdr
#define l207 gqr
#define l497 z1cntqdr
#define l486 dfelthqr
#define l582 dfelthqdr
#define l464 tlthqr
#define l535 tlthqdr
#define l475 g1cntqdr
#define l416 SOC_PETRA_SRD_TX_LANE_DATA_RATE_DIVISOR_INVALID_ERR
#define l514 zcnt
#define l552 z1cnt
#define l436 g1cnt
#define l715 SOC_PETRA_SRD_RX_PHYS_PARAMS_VERIFY
#define l654 SOC_PETRA_SRD_RX_PHYS_PARAMS_GET_UNSAFE
#define l668 soc_petra_is_fabric_quartet
#define l523 char
#define l737 soc_petra_PETRA_SRD_RATE_DIVISOR_to_string
#define l156 str
#define l723 soc_petra_srd_qrtt_sync_fifo_en
#define l727 SOC_PETRA_SRD_QRTT_SYNC_FIFO_EN
#define l647 ln_tx_sync_fifo_en
#define l758 ln_tx_sync_fifo_rstn
#define l581 SOC_SAND_BOOL2NUM_INVERSE
#define l691 soc_petra_srd_init
#define l742 SOC_PETRA_SRD_INIT
#define l682 soc_petra_srd_regs_init
#define l705 soc_petra_srd_rate_calc_kbps
#define l659 SOC_PETRA_DEBUG
#define l607 soc_petra_PETRA_SRD_TX_PHYS_INTERNAL_print
#define l598 SOC_SAND_VOID_EXIT_AND_SEND_ERROR
#if ! defined __COBF__
#define SOC_PETRA_SRD_IPU_LOAD_ENABLED 1
#define SOC_PETRA_SRD_CMU_VER 25
#define SOC_PETRA_SRD_DELAY_MSEC 50
#define SOC_PETRA_SRD_DELAY_SMALL_MSEC 5
#define SOC_PETRA_SRD_IPU_RESET_DELAY_MSEC 16
#define SOC_PETRA_SRD_NOF_CHKSUM_BUSY_WAIT_ITERATIONS 100
#define SOC_PETRA_SRD_NOF_CHKSUM_TIMER_ITERATIONS 20
#define SOC_PETRA_SRD_CHKSUM_TIMER_DELAY_MSEC 16
#define SOC_PETRA_SRD_NOF_CHKSUM_ITERS (  \
SOC_PETRA_SRD_NOF_CHKSUM_BUSY_WAIT_ITERATIONS +  \
SOC_PETRA_SRD_NOF_CHKSUM_TIMER_ITERATIONS)
#define SOC_PETRA_SRD_NOF_TRIM_BUSY_WAIT_ITERATIONS 100
#define SOC_PETRA_SRD_NOF_TRIM_TIMER_ITERATIONS 20
#define SOC_PETRA_SRD_TRIM_TIMER_DELAY_MSEC 16
#define SOC_PETRA_SRD_NOF_TRIM_ITERS (  \
SOC_PETRA_SRD_NOF_TRIM_BUSY_WAIT_ITERATIONS +  \
SOC_PETRA_SRD_NOF_TRIM_TIMER_ITERATIONS)
#define SOC_PETRA_SRD_NOF_TRIM_DONE_BUSY_WAIT_ITERATIONS 5
#define SOC_PETRA_SRD_NOF_TRIM_DONE_TIMER_ITERATIONS 40
#define SOC_PETRA_SRD_TRIM_DONE_TIMER_DELAY_MSEC 16
#define SOC_PETRA_SRD_NOF_TRIM_DONE_ITERS (  \
SOC_PETRA_SRD_NOF_TRIM_DONE_BUSY_WAIT_ITERATIONS +  \
SOC_PETRA_SRD_NOF_TRIM_DONE_TIMER_ITERATIONS)
#define SOC_PETRA_SRD_VCO_RATE_INVALID 0xFFFFFFFF
#define SOC_PETRA_SRD_AEQ_WAIT_PERIOD_MSEC 1000
#define SOC_PETRA_SRD_AEQ_NOF_ITERATIONS 5
#define SOC_PETRA_SRD_AEQ_STEADY_DELAY_MSEC 2000
#define SOC_PETRA_SRD_RXLOS_NOT_PRESENT_EYE_INDICATION 0xfff
#define SOC_PETRA_SRD_RELOCK_RETRIES_MAX 6
#define SOC_PETRA_SRD_ACK_BUSY_WAIT_ITERATIONS 50
#define SOC_PETRA_SRD_ACK_TIMER_ITERATIONS 3
#define SOC_PETRA_SRD_ACK_TIMER_DELAY_MSEC 16
#define SOC_PETRA_SRD_DEV_PRE_POST_SWNG_MAX 5
#define SOC_PETRA_SRD_DEV_SWING_MAX 100
#define SOC_PETRA_SRD_TX_ATTEN_AMP 28
#define SOC_PETRA_SRD_TX_ATTEN_MAIN 23
#define SOC_PETRA_SRD_EXTRA_DELAY_ENABLE 0
#define SOC_PETRA_SRD_FVCO_DOUBLE_MIN 3125
#define SOC_PETRA_SRD_FVCO_DOUBLE_MAX 6400
#define SOC_PETRA_SRD_RATE2RATE_RNG( lc) ( l136( lc)?( (( lc) <= l134 \
)? l321 : l181 ):(( lc) <= 3125000)? l321 : l181)
#define SOC_PETRA_SRD_BRDCST_IF_SAME( l59, l220) { lg ( l220) { l201( \
 ld, l59); } }
#define SOC_PETRA_SRD_SAME_IN_QRTT_TST( l226, l493, l220) { l220 = l9 \
; l226##_orig = ln-> l24[ lo]. l226; l22 ( l198 = lo+1; l198 < ( lo + \
 l74); l198++) { lg( !( ln-> l24[ l198]. l59) || l747(&( ln-> l24[  \
l198]. l226),&( l226##_orig), l718( l493)) ) { l220 = lt; l311; } } }
#endif
l602 l761{l356=0,l499=1,l689=2}l423;l45 l349 l141 l350[l765][l338]={{
{0,24,0,15,0x33,0x33},{0,24,5,20,0x33,0x33},{0,24,12,24,0x33,0x33}},{
{0,31,1,20,0x11,0xbb},{3,18,13,30,0x11,0xbb},{6,18,14,31,0x11,0xbb}}}
;l45 le l412(lm le ld){l12(l752);l127;lb:l15("\x65\x72\x72\x6f\x72"
"\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x65\x72"
"\x64\x65\x73\x5f\x72\x65\x67\x73\x5f\x69\x6e\x69\x74\x28\x29",0,0);}
le l726(lm le ld){le la=l114;l12(l596);la=l412(ld);lh(la,10,lb);lb:
l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74"
"\x72\x61\x5f\x73\x65\x72\x64\x65\x73\x5f\x69\x6e\x69\x74\x28\x29",0,
0);}l45 l240 l11(l40 l85*lc){lg(lc!=l29){lc->l2=
SOC_PETRA_SRD_VCO_RATE_INVALID;lc->l3=SOC_PETRA_SRD_VCO_RATE_INVALID;
lc->ls=l239;}}l45 lv l216(lm l85*lc){lv l297=l9;lg(lc==l29){l297=lt;}
lu{lg((lc->l2==SOC_PETRA_SRD_VCO_RATE_INVALID)||(lc->l3==
SOC_PETRA_SRD_VCO_RATE_INVALID)||(lc->ls==l239)){l297=lt;}}l28 l297;}
le l282(lm le l254,lm le l2,lm le l3,lm l138 l128){le l142,l238;l238=
10*10*l254* (l2+1);l1(l128){lk l36:l142=20;lj;lk l26:l142=10;lj;lk l27
:l142=5;lj;l4:l142=10;lj;}l142=l142* (l3+1);l238=l143(l238,l142);l28
l238;}l138 l528(lm le l254,lm le l554){l138 l409=l239,l179;le lc;l22(
l179=l27;l179<=l36;l179++){lc=l282(l254,0,0,l179);lg(lc==l554){l409=
l179;lj;}}l28 l409;}l45 le l176(lm le l21,lm le l70,lm le lx,l40 l85*
lc){lv l80;l12(l638);l49(lc);l80=l330((l126)l21);lg(l80){lg(!l136((
l126)l70)||!l136((l126)lx)){l19(l639,5,lb);}l1(l21){lk l525:l1(l70){
lk l533:lc->l2=1;lc->l3=0;l1(lx){lk l533:lc->ls=l36;lj;l4:l11(lc);}lj
;lk l408:lc->l2=1;lc->l3=0;l1(lx){lk l408:lc->ls=l26;lj;l4:l11(lc);}
lj;lk l119:lc->l2=4;lc->l3=1;l1(lx){lk l119:lc->ls=l26;lj;lk l166:lc
->ls=l36;lj;l4:l11(lc);}lj;lk l134:lc->l2=4;lc->l3=3;l1(lx){lk l134:
lc->ls=l27;lj;l4:l11(lc);}lj;lk l511:lc->l2=2;lc->l3=1;l1(lx){lk l511
:lc->ls=l27;lj;l4:l11(lc);}lj;lk l498:lc->l2=1;lc->l3=0;l1(lx){lk l408
:lc->ls=l26;lj;lk l498:lc->ls=l27;lj;l4:l11(lc);}lj;lk l448:lc->l2=6;
lc->l3=2;l1(lx){lk l448:lc->ls=l27;lj;l4:l11(lc);}lj;lk l171:lc->l2=4
;lc->l3=1;l1(lx){lk l119:lc->ls=l26;lj;lk l171:lc->ls=l27;lj;l4:l11(
lc);}lj;l4:l11(lc);}lj;lk l501:l1(l70){lk l145:lc->l2=3;lc->l3=2;l1(
lx){lk l145:lc->ls=l36;lj;l4:l11(lc);}lj;lk l139:lc->l2=2;lc->l3=1;l1
(lx){lk l139:lc->ls=l36;lj;l4:l11(lc);}lj;lk l166:lc->l2=0;lc->l3=0;
l1(lx){lk l166:lc->ls=l26;lj;l4:l11(lc);}lj;lk l182:lc->l2=3;lc->l3=2
;l1(lx){lk l182:lc->ls=l26;lj;lk l233:lc->ls=l27;lj;l4:l11(lc);}lj;lk
l172:lc->l2=2;lc->l3=1;l1(lx){lk l139:lc->ls=l36;lj;lk l172:lc->ls=
l26;lj;l4:l11(lc);}lj;lk l119:lc->l2=1;lc->l3=0;l1(lx){lk l119:lc->ls
=l26;lj;l4:l11(lc);}lj;lk l134:lc->l2=0;lc->l3=0;l1(lx){lk l134:lc->
ls=l27;lj;lk l166:lc->ls=l26;lj;l4:l11(lc);}lj;lk l233:lc->l2=3;lc->
l3=2;l1(lx){lk l145:lc->ls=l36;lj;lk l182:lc->ls=l26;lj;lk l233:lc->
ls=l27;lj;l4:l11(lc);}lj;lk l278:lc->l2=2;lc->l3=1;l1(lx){lk l139:lc
->ls=l36;lj;lk l172:lc->ls=l26;lj;lk l278:lc->ls=l27;lj;l4:l11(lc);}
lj;lk l427:lc->l2=4;lc->l3=2;l1(lx){lk l733:lc->ls=l36;lj;lk l665:lc
->ls=l26;lj;lk l427:lc->ls=l27;lj;l4:l11(lc);}lj;lk l171:lc->l2=1;lc
->l3=0;l1(lx){lk l119:lc->ls=l26;lj;lk l171:lc->ls=l27;lj;l4:l11(lc);
}lj;lk l512:lc->l2=6;lc->l3=3;l1(lx){lk l512:lc->ls=l27;lj;l4:l11(lc);
}lj;l4:l11(lc);}lj;lk l456:l1(l70){lk l379:lc->l2=0;lc->l3=0;l1(lx){
lk l379:lc->ls=l36;lj;l4:l11(lc);}lj;lk l333:lc->l2=3;lc->l3=2;l1(lx){
lk l333:lc->ls=l26;lj;l4:l11(lc);}lj;lk l400:lc->l2=2;lc->l3=1;l1(lx){
lk l400:lc->ls=l26;lj;l4:l11(lc);}lj;lk l417:lc->l2=0;lc->l3=0;l1(lx){
lk l379:lc->ls=l36;lj;lk l417:lc->ls=l27;lj;l4:l11(lc);}lj;lk l548:lc
->l2=3;lc->l3=2;l1(lx){lk l333:lc->ls=l26;lj;lk l548:lc->ls=l27;lj;l4
:l11(lc);}lj;lk l522:lc->l2=2;lc->l3=1;l1(lx){lk l400:lc->ls=l26;lj;
lk l522:lc->ls=l27;lj;l4:l11(lc);}lj;l4:l11(lc);}lj;lk l564:l1(l70){
lk l145:lc->l2=1;lc->l3=2;l1(lx){lk l145:lc->ls=l36;lj;l4:l11(lc);}lj
;lk l139:lc->l2=2;lc->l3=3;l1(lx){lk l139:lc->ls=l36;lj;l4:l11(lc);}
lj;lk l182:lc->l2=1;lc->l3=2;l1(lx){lk l145:lc->ls=l36;lj;lk l182:lc
->ls=l26;lj;l4:l11(lc);}lj;lk l172:lc->l2=2;lc->l3=3;l1(lx){lk l139:
lc->ls=l36;lj;lk l172:lc->ls=l26;lj;l4:l11(lc);}lj;lk l119:lc->l2=0;
lc->l3=0;l1(lx){lk l166:lc->ls=l36;lj;lk l119:lc->ls=l26;lj;l4:l11(lc
);}lj;lk l134:lc->l2=0;lc->l3=1;l1(lx){lk l134:lc->ls=l27;lj;l4:l11(
lc);}lj;lk l233:lc->l2=1;lc->l3=2;l1(lx){lk l145:lc->ls=l36;lj;lk l182
:lc->ls=l26;lj;lk l233:lc->ls=l27;lj;l4:l11(lc);}lj;lk l278:lc->l2=2;
lc->l3=3;l1(lx){lk l139:lc->ls=l36;lj;lk l172:lc->ls=l26;lj;lk l278:
lc->ls=l27;lj;l4:l11(lc);}lj;lk l171:lc->l2=0;lc->l3=0;l1(lx){lk l166
:lc->ls=l36;lj;lk l119:lc->ls=l26;lj;lk l171:lc->ls=l27;lj;l4:l11(lc);
}lj;l4:l11(lc);}lj;lk l482:l1(l70){lk l446:lc->l2=0;lc->l3=0;l1(lx){
lk l446:lc->ls=l27;lj;l4:l11(lc);}lj;l4:l11(lc);}lj;lk l588:l1(l70){
lk l495:lc->l2=0;lc->l3=0;l1(lx){lk l495:lc->ls=l27;lj;l4:l11(lc);}lj
;l4:l11(lc);}lj;l4:l19(l683,10,lb);}}lu{lc->ls=l528(l21,l70);lg(lc->
ls==l239){l11(lc);}lu{lc->l2=lc->l3=0;}}lb:l15("\x65\x72\x72\x6f\x72"
"\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64"
"\x5f\x69\x6e\x74\x65\x72\x6e\x61\x6c\x5f\x72\x61\x74\x65\x5f\x63\x61"
"\x6c\x63\x28\x29",0,0);}le l210(lm le ld,lm le li,l40 l85*lc){le ly,
la;le lp,lo,lz;l66*lq;l12(l732);l49(lc);lq=l68();lp=l173(li);lo=l208(
li);lz=l301(li);l76(lq->l6.l52[lp].l391,lc->l2,lz,10,lb);l76(lq->l6.
l52[lp].l370,lc->l3,lz,20,lb);l76(lq->l6.l81[lp][lo].l465,ly,lz,40,lb
);lc->ls=ly;lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63"
"\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x69\x6e\x74\x65\x72\x6e"
"\x61\x6c\x5f\x72\x61\x74\x65\x5f\x67\x65\x74\x28\x29",0,0);}l45 le
l366(lm le ld,lm le li,lm l648 l470,l40 le*l302,l40 le*lx){le la;le
l290,l261;l12(l716);l49(l302);l49(lx);l290=l224(ld,l159(li));la=l324(
ld,li,&l261);lh(la,5,lb);lg((l290==l187)||(l261==l187)){l19(l470,7,lb
);} *l302=l290; *lx=l261;lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20"
"\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x72\x61\x74"
"\x65\x5f\x67\x65\x74\x5f\x61\x6e\x64\x5f\x76\x61\x6c\x69\x64\x61\x74"
"\x65\x28\x29",0,0);}l180 l170(lm le ld,lm le li){l180 l21;lg(l649((
l126)li)){l21=l663(ld);}lu lg(l568(li)){l21=l589(ld);}lu lg(l481(li)){
l21=l624(ld);}lu{l21=l662;}l28 l21;}l45 le l546(lm le ld,lm le l25){
le la;le lp,l8,l18,lz;l66*lq;l58*lf;l12(l229);lq=l68();lf=l57();lp=
l163(l25);lz=l295(l25);l22(l8=0;l8<l74;l8++){l18=l88(l279(l25),lp,l8);
la=l5(ld,lr,l18,&(lf->ll.l10.l373.l359),0x0);lh(la,10+l8,lb);la=l5(ld
,lr,l18,&(lf->ll.l79.l334.l362),0x0);lh(la,20+l8,lb);}l32(lq->l6.l52[
lp].l576,0x0,lz,30,lb);l32(lq->l6.l52[lp].l576,0x1,lz,32,lb);lb:l15(""
"\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72"
"\x61\x5f\x73\x72\x64\x5f\x63\x6d\x75\x5f\x74\x72\x69\x6d\x5f\x73\x74"
"\x61\x72\x74\x28\x29",l25,0);}l45 le l365(lm le ld,lm le li){le la;
lv l16;lv l50;l58*lf;l12(l229);lf=l57();
#if ! defined __COBF__
#ifdef l296
l50=l291();
#else
l50=lt;
#endif
#endif
lg(!l50){la=l31(ld,lr,li,&(lf->ll.l10.l373.l359),&l16);lh(la,10+li,lb
);lg(l16==0x0){l19(l736,20+li,lb);}la=l31(ld,lr,li,&(lf->ll.l79.l334.
l362),&l16);lh(la,30+li,lb);lg(l16!=0x1){l19(l769,40+li,lb);}la=l31(
ld,lr,li,&(lf->ll.l157.l630.l591),&l16);lh(la,50+li,lb);lg(l16!=0x1){
l19(l618,60+li,lb);}}lb:l28 l168;}l45 le l518(lm le ld,lm le li){le la
;le l61=0;lv l42;l262 l34;l12(l229);l209{l42=l9;l61++;la=l365(ld,li);
lg(l520(la,&l168)!=l551){l42=lt;lg(l61>=
SOC_PETRA_SRD_NOF_TRIM_DONE_ITERS){lh(la,10,lb);l19(l355,20,lb);}lg(
l61>SOC_PETRA_SRD_NOF_TRIM_DONE_BUSY_WAIT_ITERATIONS){l64(
SOC_PETRA_SRD_TRIM_DONE_TIMER_DELAY_MSEC);}}}l175((l61<=
SOC_PETRA_SRD_NOF_TRIM_DONE_ITERS)&&(l42==lt));l572(&l34);l34.l502=
0x8;l34.l430=0x5;la=l131(ld,lr,li,&l34,0x0);lh(la,40+li,lb);lb:l15(""
"\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72"
"\x61\x5f\x73\x72\x64\x5f\x6c\x61\x6e\x65\x5f\x74\x72\x69\x6d\x5f\x76"
"\x61\x6c\x69\x64\x61\x74\x65\x5f\x64\x6f\x6e\x65\x28\x29",li,0);}l45
le l480(lm le ld,lm le l25){le ly,la;le l8,l18;le lp,lz;lv l50;l66*lq
;l12(l229);lq=l68();
#if ! defined __COBF__
#ifndef l296
l50=lt;
#else
l50=l291();
#endif
#endif
lp=l163(l25);lz=l295(l25);lg(!l50){l76(lq->l6.l384[lp].l728,ly,lz,10,
lb);lg(ly!=0x0){l19(l677,20,lb);}l76(lq->l6.l384[lp].l620,ly,lz,30,lb
);lg(ly!=0x0){l19(l701,35,lb);}l76(lq->l6.l384[lp].l710,ly,lz,40,lb);
lg(ly!=0x0){l19(l760,45,lb);}}lg(!l50){l22(l8=0;l8<l74;l8++){l18=l88(
l279(l25),lp,l8);la=l365(ld,l18);lh(la,50,lb);}}lb:l28 l168;}l45 le
l555(lm le ld,lm le l25){le la;le l18,l61=0;lv l42;l262 l34;l12(l229);
l209{l42=l9;l61++;la=l480(ld,l25);lg(l520(la,&l168)!=l551){l42=lt;lg(
l61>=SOC_PETRA_SRD_NOF_TRIM_DONE_ITERS){lh(la,10,lb);l19(l355,20,lb);
}lg(l61>SOC_PETRA_SRD_NOF_TRIM_DONE_BUSY_WAIT_ITERATIONS){l64(
SOC_PETRA_SRD_TRIM_DONE_TIMER_DELAY_MSEC);}}}l175((l61<=
SOC_PETRA_SRD_NOF_TRIM_DONE_ITERS)&&(l42==lt));l572(&l34);l34.l502=
0x8;l34.l430=0x5;l22(l18=l537(l25);l18<l713(l25);l18++){la=l131(ld,lr
,l18,&l34,0x0);lh(la,50+l18,lb);}lb:l15("\x65\x72\x72\x6f\x72\x20\x69"
"\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x63"
"\x6d\x75\x5f\x74\x72\x69\x6d\x5f\x76\x61\x6c\x69\x64\x61\x74\x65\x5f"
"\x64\x6f\x6e\x65\x28\x29",l25,0);}lv l585(lm le ld,lm le lp){le la,
ly;lv l89=lt;le lz,l39;l66*lq;l12(0);lq=l68();lz=l295(lp);l39=l163(lp
);l76(lq->l6.l52[l39].l294,ly,lz,5,lb);l89=l300(ly);lb:l28 l89;}l45 lv
l559(lm le ld,lm le l341){le lp,lo,l192;lv l277=lt;lv l410[l153];l22(
lp=0;lp<l153;lp++){l410[lp]=l585(ld,lp);}l22(l192=(l341*l561);(l192<(
l341+1) *l561-1)&&(!l277);l192++){lo=l693(l192);l277|=(l300(l623(ld,
l192))&&l410[l159(lo)]);}l28 l277;}l45 le l477(lm le ld,lm le lc,lm le
li){le ly,l394,l382,la;le l221,l123,l90;le l244,l347;lv l372=lt,l80;
l66*lq;l12(l622);lq=l68();l221=l734(li);l123=l628(l221);l90=l755(l221
);l80=l136((l126)lc);l76(lq->l135.l189.l319[l123],l394,l90,20,lb);l76
(lq->l135.l189.l371[l123],l382,l90,22,lb);ly=0x1;l32(lq->l135.l189.
l319[l123],ly,l90,30,lb);l32(lq->l135.l189.l371[l123],ly,l90,32,lb);
lg(l80){l244=(lc<=l134)?0x0:0x1;}lu{l244=(lc<=3125000)?0x0:0x1;}l32(
lq->l135.l720[l123].l637,l244,l90,40,lb);l667(ld,l221,l103(l244));
l372=l559(ld,l90);l347=(l372)?0x3:0x1;l32(lq->l135.l698.l704,l347,l90
,45,lb);l32(lq->l135.l189.l319[l123],l394,l90,50,lb);l32(lq->l135.
l189.l371[l123],l382,l90,52,lb);lb:l15("\x65\x72\x72\x6f\x72\x20\x69"
"\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x6d"
"\x61\x63\x5f\x72\x61\x74\x65\x5f\x73\x65\x74\x28\x29",li,0);}le l343
(lm le ld,lm le li,lm le lc){le ly,la;le lo,lp,l122,lz;l180 l21;l85
l35;le l178;lv l77,l361,l80;l66*lq;l12(l685);lq=l68();l122=l159(li);
lp=l173(li);lo=l208(li);lz=l301(li);l21=l170(ld,li);l80=l136((l126)lc
);lg(l80){l178=l224(ld,l122);la=l176(l21,l178,lc,&l35);lh(la,20,lb);}
lu{la=l176(l21,lc,lc,&l35);lh(la,25,lb);}l77=l216(&l35);lg(!l77){l19(
l117,30,lb);}ly=(le)l35.ls;l32(lq->l6.l81[lp][lo].l396,ly,lz,35,lb);
l32(lq->l6.l81[lp][lo].l465,ly,lz,40,lb);l361=l527(ld,li);lg(l361){la
=l477(ld,lc,li);lh(la,70,lb);}lb:l15("\x65\x72\x72\x6f\x72\x20\x69"
"\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x72"
"\x61\x74\x65\x5f\x73\x65\x74\x5f\x75\x6e\x73\x61\x66\x65\x28\x29",li
,lc);}le l547(lm le ld,lm le li,lm le lc){l12(l709);l41(li,l125-1,
l237,10,lb);lg(l136((l126)lc)){l41(lc,l260-1,l117,20,lb);}lu{l276(lc,
l309,l351,l117,30,lb);}lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20"
"\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x72\x61\x74"
"\x65\x5f\x76\x65\x72\x69\x66\x79\x28\x29",li,0);}le l324(lm le ld,lm
le li,l40 le*lc){le la;le l122;l180 l21;l85 l67,l167;le l158,l197;lv
l77=lt,l69=lt;l12(l724);l49(lc);l122=l159(li);l158=l224(ld,l122);lg(
l158==l187){ *lc=l187;}lu{la=l210(ld,li,&l67);lh(la,40,lb);l21=l170(
ld,li);lg(l330(l21)){l22(l197=l434;(l197<l260)&&(!l69);l197++){la=
l176(l21,l158,l197,&l167);lh(la,20,lb);l77=l216(&l167);lg(l77){lg(((
l167.l2)==(l67.l2))&&((l167.l3)==(l67.l3))&&((l167.ls)==(l67.ls))){
l69=l9; *lc=l197;}}}}lu{lg((l67.l2==0)&&(l67.l3==0)){l69=l9; *lc=l282
(l21,0,0,l67.ls);}}lg(!l69){ *lc=l187;}}lb:l15("\x65\x72\x72\x6f\x72"
"\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64"
"\x5f\x72\x61\x74\x65\x5f\x67\x65\x74\x5f\x75\x6e\x73\x61\x66\x65\x28"
"\x29",0,0);}lv l293(lm l93*l0,lm l284 l200,l40 l386*l82){l328 l219,
l348,l275,l306,l251,l363,l374,l20,l213=0,l242=0,l75,l92,l259,l327,
l344;lv l51=lt;lg(l200==l181){l219=(220+41*l0->l43) *l0->l63+(3500+
680*l0->l43);l348=500*l0->l14;l275=(220+41*l0->l43) *l0->l14;l306=400
 *l0->l13;l251=(220+41*l0->l43) *l0->l13;l363=2* (l219-l251+l348);
l374=2* (l219+l306-l275);l20=2* (l219-l251-l275);lg(l292(l20/100,l369
,l364)){l51=l9;}lu{l213=l143(((l363-l20) *100),l20);l242=l143(((l374-
l20) *100),l20);}}lu{l213=0;lg(l0->l14+l0->l13<=l56){l92=l0->l14+l0->
l13;l75=0;}lu{l92=l56;l75=l0->l14+l0->l13-l56;}l259=(1400+75*l0->l63) *
l0->l43+(4500+400*l0->l63);l327=l259+l143(((1000+(145-3*l0->l63) *l0
->l43) * (l75+l92)),2);l344=l259-2650* (l75+l92);l20=l344;lg(l292(l20
/100,l369,l364)){l51=l9;}lu{l242=l143(((l327-l20) *100),l20);}}lg(
l292(l213,0,l268)||l292(l242,0,l268)){l51=l9;}l82->l14=l213;l82->l13=
l242;l82->l20=l143(l20,100);l28 l51;}l45 lv l303(lm l194*l83,lm l194*
l109){lg(l109->l20>SOC_PETRA_SRD_DEV_SWING_MAX){lg((l83->l14>
SOC_PETRA_SRD_DEV_PRE_POST_SWNG_MAX)||(l83->l13>
SOC_PETRA_SRD_DEV_PRE_POST_SWNG_MAX)){l28 lt;}lg(l83->l20<l109->l20){
l28 l9;}lu lg(l83->l20>l109->l20){l28 lt;}}lg((l109->l20<=
SOC_PETRA_SRD_DEV_SWING_MAX)&&(l83->l20>SOC_PETRA_SRD_DEV_SWING_MAX)){
l28 lt;}lg(l83->l14>l109->l14){l28 lt;}lg(l83->l14<l109->l14){l28 l9;
}lg(l83->l13>l109->l13){l28 lt;}lg(l83->l13<l109->l13){l28 l9;}lg(l83
->l20<l109->l20){l28 l9;}l28 lt;}l45 l240 l399(l40 l194*l129){l129->
l14=SOC_PETRA_SRD_DEV_PRE_POST_SWNG_MAX+1;l129->l13=
SOC_PETRA_SRD_DEV_PRE_POST_SWNG_MAX+1;l129->l20=
SOC_PETRA_SRD_DEV_SWING_MAX+1;}lv l483(lm l386*l82,lm l284 l200,l40
l93*l0,l40 l194*l129){lv l51;l712 l110;lv l151;l194 l54,l60;l93 l37;
l386 l94;lv l225,l69=lt;l556(&l94);l399(&l54);l399(&l60);l22(l37.l43=
l494;l37.l43>=16;l37.l43--){l22(l110=l575;l110>=0;l110--){l37.l63=
l110;lg(l200==l181){l22(l37.l14=0;l37.l14<=l314;l37.l14++){l22(l37.
l13=0;l37.l13<=l56;l37.l13++){l51=l293(&l37,l200,&l94);lg(l51){l311;}
l54.l14=l188(l82->l14,l94.l14);l54.l13=l188(l82->l13,l94.l13);l54.l20
=l188(l82->l20,l94.l20);l225=l303(&l54,&l60);lg(l225){l0->l13=l37.l13
;l0->l14=l37.l14;l0->l43=l37.l43;l0->l63=l37.l63;l60.l14=l54.l14;l60.
l13=l54.l13;l60.l20=l54.l20;}}}}lu{l22(l151=0;l151<=(l314+l56+1);l151
++){lg(l151<=l56){l37.l13=l151;l37.l14=0;}lu{l37.l13=l56;l37.l14=l151
-l56;}l51=l293(&l37,l200,&l94);lg(l51){l311;}l54.l14=l188(l82->l14,
l94.l14);l54.l13=l188(l82->l13,l94.l13);l54.l20=l188(l82->l20,l94.l20
);l225=l303(&l54,&l60);lg(l225){l0->l13=l37.l13;l0->l14=l37.l14;l0->
l43=l37.l43;l0->l63=l37.l63;l60.l14=l54.l14;l60.l13=l54.l13;l60.l20=
l54.l20;}}}}}lg((l60.l14>SOC_PETRA_SRD_DEV_PRE_POST_SWNG_MAX)||(l60.
l13>SOC_PETRA_SRD_DEV_PRE_POST_SWNG_MAX)){l69=lt;}lu{l69=l9;}l129->
l14=l60.l14;l129->l13=l60.l13;l129->l20=l60.l20;l28 l300(l69);}l45 lv
l479(lm le l214,l40 l93*l0){lv l51=lt;le l105,l7;lv l23;l105=(l214>
l299)?l299:l214;l7=l143((l105*72),100);l658(l7,100);l0->l43=
SOC_PETRA_SRD_TX_ATTEN_AMP;l0->l63=SOC_PETRA_SRD_TX_ATTEN_MAIN;lg(l7
<=3)l23=0;lu lg(l7<=7)l23=1;lu lg(l7<=10)l23=2;lu lg(l7<=14)l23=3;lu
lg(l7<=18)l23=4;lu lg(l7<=22)l23=5;lu lg(l7<=26)l23=6;lu lg(l7<=30)l23
=7;lu lg(l7<=34)l23=8;lu lg(l7<=39)l23=9;lu lg(l7<=43)l23=10;lu lg(l7
<=48)l23=11;lu lg(l7<=52)l23=12;lu lg(l7<=57)l23=13;lu lg(l7<=62)l23=
14;lu lg(l7<=67)l23=15;lu lg(l7<=72)l23=16;lu lg(l7<=77)l23=17;lu lg(
/* petra code. Almost not in use. Ignore coverity errors */
/* coverity[dead_error_line] */
l7<=83)l23=18;lu lg(l7<=88)l23=19;lu lg(l7<=94)l23=20;lu lg(l7<=101)l23
=21;lu l23=22;lg(l23<=l56){l0->l14=0;l0->l13=l23;}lu{l0->l14=l23-l56;
l0->l13=l56;}l28 l51;}l45 lv l444(lm l93*l0,l40 le*l214){lv l51=lt;lv
l23;le l7,l105=l299;lg((l0->l43!=SOC_PETRA_SRD_TX_ATTEN_AMP)||(l0->
l63!=SOC_PETRA_SRD_TX_ATTEN_MAIN)){l51=l9;l127;}l23=l0->l14+l0->l13;
lg(l23<=l56){lg(!((l0->l14==0)&&(l0->l13==l23))){l51=l9;l127;}}lu{lg(
!((l0->l14==(l23-l56))&&(l0->l13==l56))){l51=l9;l127;}}l1(l23){lk 0:
l7=3;lj;lk 1:l7=7;lj;lk 2:l7=10;lj;lk 3:l7=14;lj;lk 4:l7=18;lj;lk 5:
l7=22;lj;lk 6:l7=26;lj;lk 7:l7=30;lj;lk 8:l7=34;lj;lk 9:l7=39;lj;lk 10
:l7=43;lj;lk 11:l7=48;lj;lk 12:l7=52;lj;lk 13:l7=57;lj;lk 14:l7=62;lj
;lk 15:l7=67;lj;lk 16:l7=72;lj;lk 17:l7=77;lj;lk 18:l7=83;lj;lk 19:l7
=88;lj;lk 20:l7=94;lj;lk 21:l7=101;lj;lk 22:l7=101;l4:l51=l9;l127;}
l105=l143((l7*100),72);lb: *l214=l105;l28 l51;}l45 l240 l287(lm l93*
l78,l40 l141*l47){lg((l78==l29)||(l47==l29)){l127;}l47->l43=l78->l43;
l47->l110=l78->l63;l47->l92=l78->l13;l47->l75=l78->l14;lb:l28;}l45
l240 l577(lm l141*l47,l40 l93*l78){lg((l78==l29)||(l47==l29)){l127;}
l78->l43=l47->l43;l78->l63=l47->l110;l78->l13=l47->l92;l78->l14=l47->
l75;lb:l28;}le l541(lm le ld,lm le li,lm l352 l55,lm l315*ln){le la;
lv l193,l191,l16,l30;l141 l46;l93 l0;le l70,lx;l284 l84;l423 l310;l85
l35;l253*l101=l29, *l106=l29, *l203=l29, *l95=l29, *l96=l29, *l235=
l29, *l107=l29, *l211=l29, *l91=l29, *l212=l29;l262*l205=l29, *l227=
l29, *l218=l29;l58*lf;l431 l130;l194 l562;l12(l678);l49(ln);l656(&l0);
lf=l57();la=l366(ld,li,l524,&l70,&lx);lh(la,10,lb);l84=
SOC_PETRA_SRD_RATE2RATE_RNG((l126)lx);lg(l55==l387){l130=ln->l38.l130
;}lu{l130=l565;}l560(&l46,&(l350[l84][l130]),l141,1);lg(l55==l329){la
=l483(&(ln->l38.l185),l84,&l0,&l562);lg(la!=l114){l19(l627,20,lb);}
l287(&l0,&l46);}lu lg(l55==l312){lg(l84==l181){l19(l449,25,lb);}la=
l479(ln->l38.l105,&l0);lg(la!=l114){l19(l612,30,lb);}l287(&l0,&l46);}
lu lg(l55==l357){l287(&ln->l38.l47,&l46);}la=l210(ld,li,&l35);lh(la,
50,lb);l1(l35.ls){lk l36:l101=&(lf->ll.lw.l587.l458);l106=&(lf->ll.lw
.l249.l439);l203=&(lf->ll.lw.l249.l696);l95=&(lf->ll.lw.l164.l269);
l96=&(lf->ll.lw.l184.l269);l235=&(lf->ll.lw.l164.l735);l107=&(lf->ll.
lw.l164.l471);l211=&(lf->ll.lw.l164.l597);l91=&(lf->ll.lw.l184.l468);
l212=&(lf->ll.lw.l184.l604);l205=&(lf->ll.lw.l249.l34);l227=&(lf->ll.
lw.l164.l34);l218=&(lf->ll.lw.l184.l34);lj;lk l26:l101=&(lf->ll.lw.
l443.l437);l106=&(lf->ll.lw.l257.l584);l203=&(lf->ll.lw.l257.l741);
l95=&(lf->ll.lw.l147.l271);l96=&(lf->ll.lw.l169.l271);l235=&(lf->ll.
lw.l147.l601);l107=&(lf->ll.lw.l147.l532);l211=&(lf->ll.lw.l147.l595);
l91=&(lf->ll.lw.l169.l543);l212=&(lf->ll.lw.l169.l730);l205=&(lf->ll.
lw.l257.l34);l227=&(lf->ll.lw.l147.l34);l218=&(lf->ll.lw.l169.l34);lj
;lk l27:l101=&(lf->ll.lw.l583.l580);l106=&(lf->ll.lw.l267.l540);l203=
&(lf->ll.lw.l267.l652);l95=&(lf->ll.lw.l165.l264);l96=&(lf->ll.lw.
l202.l264);l235=&(lf->ll.lw.l165.l729);l107=&(lf->ll.lw.l165.l567);
l211=&(lf->ll.lw.l165.l613);l91=&(lf->ll.lw.l202.l519);l212=&(lf->ll.
lw.l202.l680);l205=&(lf->ll.lw.l267.l34);l227=&(lf->ll.lw.l165.l34);
l218=&(lf->ll.lw.l202.l34);lj;l4:l19(l467,60,lb);lj;}l310=(l84==l321)?
l499:l356;l16=(l310==l356)?0x0:0x3;la=l5(ld,lr,li,l101,l16);lh(la,70,
lb);l30=0x0;la=l86(&l46.l43,l106,&l30);lh(la,72,lb);l16=0x1;la=l86(&
l16,l203,&l30);lh(la,74,lb);la=l131(ld,lr,li,l205,l30);lh(la,74,lb);
l193=(lv)l671(l46.l75,1,0);l191=(lv)l675(l46.l75,2);l30=0x0;la=l86(&
l193,l95,&l30);lh(la,80,lb);l16=0x1;la=l86(&l16,l235,&l30);lh(la,82,
lb);la=l86(&l46.l92,l107,&l30);lh(la,84,lb);l16=0x1;la=l86(&l16,l211,
&l30);lh(la,86,lb);la=l131(ld,lr,li,l227,l30);lh(la,88,lb);l30=0x0;la
=l86(&l191,l96,&l30);lh(la,90,lb);la=l86(&l46.l110,l91,&l30);lh(la,92
,lb);l16=0x1;la=l86(&l16,l212,&l30);lh(la,94,lb);la=l131(ld,lr,li,
l218,l30);lh(la,96,lb);la=l131(ld,lr,li,&(lf->ll.lw.l462.l34),l46.
l380);lh(la,100,lb);la=l131(ld,lr,li,&(lf->ll.lw.l459.l34),l46.l377);
lh(la,110,lb);lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f"
"\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x74\x78\x5f\x70\x68"
"\x79\x73\x5f\x70\x61\x72\x61\x6d\x73\x5f\x73\x65\x74\x5f\x75\x6e\x73"
"\x61\x66\x65\x28\x29",li,0);}le l569(lm le ld,lm le li,lm l352 l55,
lm l315*ln){l12(l617);l49(ln);l283(ln);l41(li,l125-1,l237,10,lb);l41(
l55,l766-1,l590,20,lb);lg(l55==l387){l41(ln->l38.l130,l338-1,l664,60,
lb);}lu lg(l55==l329){l41(ln->l38.l185.l14,l268,l388,30,lb);l41(ln->
l38.l185.l13,l268,l388,35,lb);l276(ln->l38.l185.l20,l369,l364,l388,40
,lb);}lu lg(l55==l312){l41(ln->l38.l105,l299,l634,80,lb);}lu lg(l55==
l357){l41(ln->l38.l47.l43,l494,l651,70,lb);l41(ln->l38.l47.l63,l575,
l599,72,lb);l41(ln->l38.l47.l14,l314,l592,74,lb);l41(ln->l38.l47.l13,
l56,l650,76,lb);}lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f"
"\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x74\x78\x5f\x70\x68"
"\x79\x73\x5f\x70\x61\x72\x61\x6d\x73\x5f\x76\x65\x72\x69\x66\x79\x28"
"\x29",li,l55);}le l557(lm le ld,lm le li,lm l352 l55,l40 l315*ln){le
la;lv l193,l191,l16;l141 l46;l93 l0;le l70,lx;l284 l84;l85 l35;l253*
l101=l29, *l106=l29, *l95=l29, *l96=l29, *l107=l29, *l91=l29;l58*lf;
l431 l186;lv l367,l69=lt;l12(l750);l49(ln);lf=l57();la=l366(ld,li,
l524,&l70,&lx);lh(la,10,lb);l84=SOC_PETRA_SRD_RATE2RATE_RNG((l126)lx);la=
l210(ld,li,&l35);lh(la,20,lb);l1(l35.ls){lk l36:l101=&(lf->ll.lw.l587
.l458);l106=&(lf->ll.lw.l249.l439);l95=&(lf->ll.lw.l164.l269);l96=&(
lf->ll.lw.l184.l269);l107=&(lf->ll.lw.l164.l471);l91=&(lf->ll.lw.l184
.l468);lj;lk l26:l101=&(lf->ll.lw.l443.l437);l106=&(lf->ll.lw.l257.
l584);l95=&(lf->ll.lw.l147.l271);l96=&(lf->ll.lw.l169.l271);l107=&(lf
->ll.lw.l147.l532);l91=&(lf->ll.lw.l169.l543);lj;lk l27:l101=&(lf->ll
.lw.l583.l580);l106=&(lf->ll.lw.l267.l540);l95=&(lf->ll.lw.l165.l264);
l96=&(lf->ll.lw.l202.l264);l107=&(lf->ll.lw.l165.l567);l91=&(lf->ll.
lw.l202.l519);lj;l4:l19(l467,30,lb);lj;}la=l31(ld,lr,li,l101,&l16);lh
(la,40,lb);la=l31(ld,lr,li,l106,&(l46.l43));lh(la,50,lb);la=l31(ld,lr
,li,l95,&l193);lh(la,60,lb);la=l31(ld,lr,li,l96,&l191);lh(la,65,lb);
l46.l75=(lv)l674(l193,1,0);l307(l46.l75,l191,2);la=l31(ld,lr,li,l107,
&(l46.l92));lh(la,70,lb);la=l31(ld,lr,li,l91,&(l46.l110));lh(la,80,lb
);la=l31(ld,lr,li,&(lf->ll.lw.l462.l553),&(l46.l380));lh(la,90,lb);la
=l31(ld,lr,li,&(lf->ll.lw.l459.l553),&(l46.l377));lh(la,100,lb);l577(
&l46,&l0);lg(l55==l387){l22(l186=l565;(l186<l338)&&(!l69);l186++){
l670(&l46,&(l350[l84][l186]),l141,1,l367);lg(l367==l9){l69=l9;ln->l38
.l130=l186;}}lg(l69==lt){ln->l38.l130=l606;}}lu lg(l55==l329){la=l293
(&l0,l84,&(ln->l38.l185));lg(la!=l114){l556(&(ln->l38.l185));}}lu lg(
l55==l312){lg(l84==l181){l19(l449,110,lb);}la=l444(&l0,&(ln->l38.l105
));lg(la!=l114){ln->l38.l105=l621;}}lu{l560(&(ln->l38.l47),&l0,l93,1);
}lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65"
"\x74\x72\x61\x5f\x73\x72\x64\x5f\x74\x78\x5f\x70\x68\x79\x73\x5f\x70"
"\x61\x72\x61\x6d\x73\x5f\x67\x65\x74\x5f\x75\x6e\x73\x61\x66\x65\x28"
"\x29",li,0);}le l133(lm le ld,lm le li,lm l288 l62,lm lv l228,lm lv
l326){le la;lv l113;l58*lf;lv l160,l196;l12(l763);lf=l57();l160=l442(
l62);l196=l425(l62);l113=l375(l228);lg(l160){la=l5(ld,lr,li,&(lf->ll.
l79.l162.l488),0x1);lh(la,10,lb);la=l5(ld,lr,li,&(lf->ll.l79.l162.
l492),l113);lh(la,15,lb);lg(l326==l9){l428(ld,l258,li,l228);}}lg(l196
){la=l5(ld,lr,li,&(lf->ll.l53.l120.l544),0x1);lh(la,20,lb);la=l5(ld,
lr,li,&(lf->ll.l53.l120.l549),l113);lh(la,25,lb);lg(l326==l9){l428(ld
,l323,li,l228);}}lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f"
"\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x6c\x61\x6e\x65\x5f"
"\x70\x6f\x6c\x61\x72\x69\x74\x79\x5f\x73\x65\x74\x5f\x75\x6e\x73\x61"
"\x66\x65\x28\x29",li,l62);}le l404(lm le ld,lm le li,lm l288 l62,lm
lv l228){l12(l611);l41(li,l125,l237,10,lb);l41(l62,l536,l539,20,lb);
lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65"
"\x74\x72\x61\x5f\x73\x72\x64\x5f\x6c\x61\x6e\x65\x5f\x70\x6f\x6c\x61"
"\x72\x69\x74\x79\x5f\x76\x65\x72\x69\x66\x79\x28\x29",0,0);}le l317(
lm le ld,lm le li,l40 lv*l155,l40 lv*l118){le la;lv l113;l58*lf;lv
l402=lt,l390=lt;l12(l751);lf=l57();la=l5(ld,lr,li,&(lf->ll.l79.l162.
l488),0x1);lh(la,10,lb);la=l31(ld,lr,li,&(lf->ll.l79.l162.l492),&l113
);lh(la,15,lb);l402=l103(l113);la=l5(ld,lr,li,&(lf->ll.l53.l120.l544),
0x1);lh(la,20,lb);la=l31(ld,lr,li,&(lf->ll.l53.l120.l549),&l113);lh(
la,25,lb);l390=l103(l113); *l155=l402; *l118=l390;lb:l15("\x65\x72"
"\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f"
"\x73\x72\x64\x5f\x6c\x61\x6e\x65\x5f\x70\x6f\x6c\x61\x72\x69\x74\x79"
"\x5f\x67\x65\x74\x5f\x75\x6e\x73\x61\x66\x65\x28\x29",li,0);}l45 le
l325(lm le ld,lm le li){le la;l762 l87;lv l286,l298,l232=lt;l697 l337
;le l231=0;lv l50,l263;l12(l768);
#if ! defined __COBF__
#ifdef l296
l50=l291();
#else
l50=lt;
#endif
#endif
l717(&l87);la=l731(ld,li,&l337);lh(la,5,lb);la=l455(ld,li,l608);lh(la
,10,lb);la=l317(ld,li,&l286,&l298);lh(la,20,lb);l263=l103((l286==lt)&&
(l298==lt));lg(l263==lt){la=l133(ld,li,l98,lt,l9);lh(la,30,lb);}la=
l748(ld,li,l98,l679);lh(la,40,lb);l485(ld,l9);la=l686(ld,li,l98);lh(
la,45,lb);l485(ld,lt);l175((l232==lt)&&(l231<
SOC_PETRA_SRD_RELOCK_RETRIES_MAX)){la=l629(ld,li,&l87);lh(la,50,lb);
l232=l103((l87.l681==0)&&(l87.l757!=l767));lg(l232==lt){la=l756(ld,li
);lh(la,60,lb);lg((l231+1)>=(SOC_PETRA_SRD_RELOCK_RETRIES_MAX-2)){l64
(1);}}l231++;}la=l646(ld,li,l98);lh(la,70,lb);lg(l263==lt){la=l133(ld
,li,l258,l286,l9);lh(la,80,lb);la=l133(ld,li,l323,l298,l9);lh(la,90,
lb);}la=l455(ld,li,l337);lh(la,100,lb);lg(!l50){lg((l232==lt)&&(l231
>=SOC_PETRA_SRD_RELOCK_RETRIES_MAX)){
#if ! defined __COBF__
#if SOC_PETRA_DEBUG_IS_LVL2
l152("\x52\x65\x6c\x6f\x63\x6b\x20\x63\x68\x65\x63\x6b\x20\x72\x65"
"\x70\x6f\x72\x74\x65\x64\x20\x6e\x6f\x2d\x72\x65\x6c\x6f\x63\x6b\x20"
"\x6f\x6e\x20\x6c\x61\x6e\x65\x20\x25\x75\x2e\x20\x54\x68\x69\x73\x20"
"\x6d\x61\x79\x20\x62\x65\x20\x61\x20\x66\x61\x6c\x73\x65\x20\x69\x6e"
"\x64\x69\x63\x61\x74\x69\x6f\x6e\x2c\x20\x68\x65\x6e\x63\x65\x20\x6e"
"\x6f\x74\x20\x74\x72\x65\x61\x74\x65\x64\x20\x61\x73\x20\x65\x72\x72"
"\x6f\x72\x2e\x20" "\n\r",li);
#endif
#endif
}}lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70"
"\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x76\x61\x6c\x69\x64\x61\x74\x65"
"\x5f\x61\x6e\x64\x5f\x72\x65\x6c\x6f\x63\x6b\x28\x29",li,0);}le l504
(lm le ld,lm le li,lm l288 l62,lm l140 l146){le la;lv l72;l58*lf;lv
l160,l196,l320=lt;l140 l389;l12(l653);lf=l57();l160=l442(l62);l196=
l425(l62);l1(l146){lk l115:l72=0x1;lj;lk l252:lk l545:l72=0x0;lj;l4:
l19(l420,5,lb);}lg(l160&&(l146==l115)){la=l5(ld,lr,li,&(lf->ll.l79.
l162.l331),l72);lh(la,10,lb);}lg(l196){la=l5(ld,lr,li,&(lf->ll.l53.
l120.l245),l72);lh(la,20,lb);}lg(l160&&(l146!=l115)){la=l5(ld,lr,li,&
(lf->ll.l79.l162.l331),l72);lh(la,30,lb);}lg(l146==l545){lg(l62==l258
){la=l31(ld,lr,li,&(lf->ll.l53.l120.l245),&l72);lh(la,40,lb);l389=(
l72)?l115:l252;lg(l389==l115){la=l5(ld,lr,li,&(lf->ll.l53.l120.l245),
0x0);lh(la,45,lb);l320=l9;}}la=l325(ld,li);lh(la,50,lb);lg(l320==l9){
la=l5(ld,lr,li,&(lf->ll.l53.l120.l245),0x1);lh(la,60,lb);}}lb:l15(""
"\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72"
"\x61\x5f\x73\x72\x64\x5f\x6c\x61\x6e\x65\x5f\x70\x6f\x77\x65\x72\x5f"
"\x73\x74\x61\x74\x65\x5f\x73\x65\x74\x5f\x75\x6e\x73\x61\x66\x65\x28"
"\x29",li,l62);}le l579(lm le ld,lm le li,lm l288 l62,lm l140 l146){
l12(l661);l41(li,l125-1,l237,10,lb);l41(l62,l536,l539,20,lb);l41(l146
,l706,l420,30,lb);lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73"
"\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x6c\x61\x6e\x65"
"\x5f\x70\x6f\x77\x65\x72\x5f\x73\x74\x61\x74\x65\x5f\x76\x65\x72\x69"
"\x66\x79\x28\x29",li,l62);}le l376(lm le ld,lm le li,l40 l140*l385,
l40 l140*l236){le la;lv l72=0;lv l230=lt;l58*lf;l140 l346=0,l403=0;
l12(l633);l49(l385);l49(l236);lf=l57();la=l31(ld,lr,li,&(lf->ll.l79.
l162.l331),&l72);lh(la,10,lb);l230=l103(l72);l346=(l230)?l115:l252;la
=l31(ld,lr,li,&(lf->ll.l53.l120.l245),&l72);lh(la,20,lb);l230=l103(
l72);l403=(l230)?l115:l252; *l385=l346; *l236=l403;lb:l15("\x65\x72"
"\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f"
"\x73\x72\x64\x5f\x6c\x61\x6e\x65\x5f\x70\x6f\x77\x65\x72\x5f\x73\x74"
"\x61\x74\x65\x5f\x67\x65\x74\x5f\x75\x6e\x73\x61\x66\x65\x28\x29",li
,0);}l45 le l451(lm le ld,lm l243 l33){le l44,ly,la;le l111,lp=0,lz=
l353;l66*lq;l12(l516);lq=l68();lz=l256(l33);l111=l381(l33);l406(lq->
l6.l48,l44,lz,5,lb);ly=0x1;l71(lq->l6.l48.l450,ly,l44,10,lb);ly=0x1;
l71(lq->l6.l48.l515,ly,l44,20,lb);ly=0x1;l71(lq->l6.l48.l526,ly,l44,
30,lb);ly=0x1;l71(lq->l6.l48.l508,ly,l44,40,lb);ly=0x2;l71(lq->l6.l48
.l600,ly,l44,50,lb);ly=0x1;l71(lq->l6.l48.l676,ly,l44,60,lb);ly=l672;
l71(lq->l6.l48.l753,ly,l44,70,lb);ly=0x1;l71(lq->l6.l48.l754,ly,l44,
80,lb);ly=0x0;l71(lq->l6.l48.l744,ly,l44,82,lb);ly=0x0;l71(lq->l6.l48
.l673,ly,l44,84,lb);l405(lq->l6.l48,l44,lz,85,lb);l22(lp=0;lp<l111;lp
++){
#if ! defined __COBF__
#if SOC_PETRA_SRD_IPU_LOAD_ENABLED
ly=0x0;
#else
ly=0x1;
#endif
#endif
l32(lq->l6.l52[lp].l688,ly,lz,90,lb);l32(lq->l6.l52[lp].l714,0x0,lz,
92,lb);l32(lq->l6.l52[lp].l643,0x0,lz,94,lb);l32(lq->l6.l52[lp].l294,
0x1,lz,96,lb);}l406(lq->l6.l48,l44,lz,110,lb);ly=0x0;l71(lq->l6.l48.
l450,ly,l44,115,lb);ly=0x0;l71(lq->l6.l48.l526,ly,l44,120,lb);ly=0x0;
l71(lq->l6.l48.l508,ly,l44,125,lb);l405(lq->l6.l48,l44,lz,130,lb);lb:
l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74"
"\x72\x61\x5f\x73\x72\x64\x5f\x73\x74\x61\x72\x5f\x72\x65\x73\x65\x74"
"\x28\x29",l33,lp);}l45 le l395(lm le ld,lm le l21){le l65;lg(l330((
l126)l21)){l1(l21){lk l525:l65=125000;lj;lk l501:l65=156250;lj;lk l456
:l65=200000;lj;lk l564:l65=312500;lj;lk l482:l65=218750;lj;lk l588:
l65=212500;lj;l4:l65=l393;}}lu{l65=l21;}l28 l65;}l45 le l335(lm le ld
,lm le li,lm le l2,lm le l3,lm le l21){le la;le l234,l150;lv l199,
l342=0x2;l58*lf;l12(l619);lf=l57();l234=l395(ld,l21);lg(l603(l234)){
l19(l684,10,lb);}lu{l234/=10;}l150=(2*l234* (l2+1))/(10* (l3+1));lg(
l150<SOC_PETRA_SRD_FVCO_DOUBLE_MIN){l19(l759,12,lb);}lg(l150>
SOC_PETRA_SRD_FVCO_DOUBLE_MAX){l19(l626,14,lb);}lg(l150<3200){l199=
0x3;}lu lg(l150<4800){l199=0x4;}lu lg(l150<5500){l199=0x5;}lu{l199=
0x6;}la=l5(ld,lr,li,&(lf->ll.l157.l507.l636),l199);lh(la,20,lb);la=l5
(ld,lr,li,&(lf->ll.l157.l507.l342),l342);lh(la,22,lb);lb:l15("\x65"
"\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61"
"\x5f\x73\x72\x64\x5f\x69\x76\x63\x64\x6c\x5f\x73\x65\x74\x28\x29",l2
,l3);}le l566(lm le ld,lm l243 l33,lm l360*ln){le l44,l407,la;lv l77,
l42=lt,l50;lv
#if ! defined __COBF__
#if SOC_PETRA_SRD_IPU_LOAD_ENABLED
l30,l16,
#endif
#endif
l383;le l132=0,l61=0,l111,lp=0,lo,l108,lz=l353;l58*lf;l85 l35;le lx;
l180 l21;l66*lq;l12(l516);l49(ln);lf=l57();lq=l68();
#if ! defined __COBF__
#ifdef l296
l50=l291();
#else
l50=lt;
#endif
#endif
lz=l256(l33);l111=l381(l33);la=l451(ld,l33);lh(la,10,lb);
#if ! defined __COBF__
#if SOC_PETRA_SRD_IPU_LOAD_ENABLED
la=l700(ld,l33);lh(la,15,lb);
#endif
#endif
l22(lp=0;lp<l111;lp++){lg(ln->l121[lp].l89){lx=ln->l121[lp].l174;l509
(ld,l266(l33,lp),lx);l21=l170(ld,l88(l33,lp,0));la=l176(l21,lx,lx,&
l35);lh(la,20,lb);l77=l216(&l35);lg(!l77){l19(l117,25,lb);}l32(lq->l6
.l52[lp].l391,l35.l2,lz,35,lb);l32(lq->l6.l52[lp].l370,l35.l3,lz,40,
lb);l32(lq->l6.l52[lp].l294,0x0,lz,45,lb);la=l31(ld,l206,l266(l33,lp),
&(lf->l215.l73.l615.l694),&l383);lh(la,50,lb);lg(!l50){lg(l383!=
SOC_PETRA_SRD_CMU_VER){l19(l739,55,lb);}}l108=l88(l33,lp,0);l201(ld,
l9);la=l335(ld,l108,l35.l2,l35.l3,l21);lh(la,60,lb);l201(ld,lt);la=
l546(ld,l266(l33,lp));lh(la,63,lb);}}
#if ! defined __COBF__
#if SOC_PETRA_SRD_EXTRA_DELAY_ENABLE
l64(SOC_PETRA_SRD_DELAY_MSEC);
#endif
#endif
#if ! defined __COBF__
#if SOC_PETRA_SRD_IPU_LOAD_ENABLED
l32(lq->l6.l48.l515,0x0,lz,65,lb);
#if SOC_PETRA_SRD_EXTRA_DELAY_ENABLE
l64(SOC_PETRA_SRD_IPU_RESET_DELAY_MSEC);
#endif
l209{l42=l9;l132++;la=l463(ld,l273,l33,&(lf->l144.l73.l190.l34),&l30);
lh(la,70,lb);la=l255(&l30,&(lf->l144.l73.l190.l616),&l16);lh(la,75,lb
);lg(!l50){lg(l16!=0x0){l42=lt;lg(l132>=
SOC_PETRA_SRD_NOF_CHKSUM_ITERS){l19(l764,180,lb);}}la=l255(&l30,&(lf
->l144.l73.l190.l645),&l16);lh(la,80,lb);lg(l16!=0x1){l42=lt;lg(l132
>=SOC_PETRA_SRD_NOF_CHKSUM_ITERS){l19(l605,85,lb);}}la=l255(&l30,&(lf
->l144.l73.l190.l687),&l16);lh(la,90,lb);lg(l16!=0x1){l42=lt;lg(l132
>=SOC_PETRA_SRD_NOF_CHKSUM_ITERS){l19(l454,220,lb);}}la=l255(&l30,&(
lf->l144.l73.l190.l707),&l16);lh(la,95,lb);lg(l16!=0x0){l42=lt;lg(
l132>=SOC_PETRA_SRD_NOF_CHKSUM_ITERS){l19(l454,225,lb);}}lg(l42==lt){
lg(l132>SOC_PETRA_SRD_NOF_CHKSUM_BUSY_WAIT_ITERATIONS){l64(
SOC_PETRA_SRD_CHKSUM_TIMER_DELAY_MSEC);}}}}l175((l132<=
SOC_PETRA_SRD_NOF_CHKSUM_ITERS)&&(l42==lt));
#endif
#if SOC_PETRA_SRD_EXTRA_DELAY_ENABLE
l64(SOC_PETRA_SRD_IPU_RESET_DELAY_MSEC);
#endif
#if SOC_PETRA_SRD_IPU_LOAD_ENABLED
l209{l42=l9;l61++;lg(!l50){la=l31(ld,l273,l33,&(lf->l144.l73.l190.
l745),&l16);lh(la,110,lb);lg(l16!=0x1){l42=lt;lg(l61>=
SOC_PETRA_SRD_NOF_TRIM_ITERS){l19(l355,115,lb);}lg(l61>
SOC_PETRA_SRD_NOF_TRIM_BUSY_WAIT_ITERATIONS){l64(
SOC_PETRA_SRD_TRIM_TIMER_DELAY_MSEC);}}}}l175((l61<=
SOC_PETRA_SRD_NOF_TRIM_ITERS)&&(l42==lt));
#endif
#endif
l22(lp=0;lp<l111;lp++){lg(ln->l121[lp].l89){la=l555(ld,l266(l33,lp));
lh(la,117,lb);lx=ln->l121[lp].l174;l406(lq->l6.l81[lp][0],l407,lz,125
,lb);l22(lo=0;lo<l74;lo++){l44=l407;l307(l44,0x0,l424(lq->l6.l81[lp][
lo].l476.l487));l307(l44,0x0,l424(lq->l6.l81[lp][lo].l503.l487));l405
(lq->l6.l81[lp][lo],l44,lz,129,lb);}
#if ! defined __COBF__
#if SOC_PETRA_SRD_EXTRA_DELAY_ENABLE
l64(SOC_PETRA_SRD_IPU_RESET_DELAY_MSEC);
#endif
#endif
l108=l88(l33,lp,0);l201(ld,l9);la=l5(ld,lr,l108,&(lf->ll.l452.l447.
l415),0x1);lh(la,135,lb);la=l5(ld,lr,l108,&(lf->ll.l177.l513.l558),0);
lh(la,145,lb);la=l5(ld,lr,l108,&(lf->ll.l177.l280.l478),0);lh(la,150,
lb);la=l5(ld,lr,l108,&(lf->ll.l177.l280.l474),0x0);lh(la,155,lb);la=
l5(ld,lr,l108,&(lf->ll.l157.l550.l542),l421);lh(la,160,lb);la=l5(ld,
lr,l108,&(lf->ll.l157.l466.l414),l419);lh(la,165,lb);l201(ld,lt);}}lb
:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65"
"\x74\x72\x61\x5f\x73\x72\x64\x5f\x73\x74\x61\x72\x5f\x73\x65\x74\x5f"
"\x75\x6e\x73\x61\x66\x65\x28\x29",l33,lp);}le l418(lm le ld,lm l243
l33,lm l360*ln){le l195;l328 lc;lv l80;l12(l644);l49(ln);l283(ln);l41
(l33,l708-1,l272,10,lb);l22(l195=0;l195<l719;l195++){lg(ln->l121[l195
].l89==l9){lc=ln->l121[l195].l174;l80=l136(lc);lg(l80){l41(lc,l260-1,
l117,20,lb);}lu{l276(lc,l309,l351,l117,30,lb);}}}lb:l15("\x65\x72\x72"
"\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73"
"\x72\x64\x5f\x73\x74\x61\x72\x5f\x76\x65\x72\x69\x66\x79\x28\x29",
l33,0);}le l570(lm le ld,lm l243 l33,l40 l360*ln){le la;le l111,l183=
0,l39,l289;l12(l610);l49(ln);l111=l381(l33);l289=l722(l33);l22(l183=
l289;l183<(l289+l111);l183++){l39=l163(l183);la=l461(ld,l183,&(ln->
l121[l39]));lh(la,10,lb);}lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e"
"\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x73\x74"
"\x61\x72\x5f\x67\x65\x74\x5f\x75\x6e\x73\x61\x66\x65\x28\x29",l33,0);
}le l743(lm le ld,lm le l25,lm l378*ln){le la,l316;le l8,l18,l39,lz=0
,l17;l85 l35;le lx;l180 l21;lv l77,l241,l161=l9;l749 l124;l140 l358,
l354;lv l148[l74],l149[l74];l58*lf;l66*lq=l29;l12(l746);l41(l25,l153-
1,l272,10,lb);lf=l57();lq=l68();l17=l279(l25);lz=l256(l17);l39=l163(
l25);l161=lt;la=l5(ld,l206,l25,&(lf->l215.l73.l285.l429),0x1);lh(la,
20,lb);lg(ln->l89==lt){la=l5(ld,l206,l25,&(lf->l215.l73.l285.l472),
0x1);lh(la,30,lb);}lu{l22(l8=0;l8<l74;l8++){l18=l88(l17,l39,l8);la=
l31(ld,lr,l18,&(lf->ll.l53.l137.l148),&(l148[l8]));lh(la,31,lb);la=
l31(ld,lr,l18,&(lf->ll.l53.l137.l149),&(l149[l8]));lh(la,32,lb);la=l5
(ld,lr,l18,&(lf->ll.l53.l137.l148),0x0);lh(la,31,lb);la=l5(ld,lr,l18,
&(lf->ll.l53.l137.l149),0x0);lh(la,32,lb);}la=l5(ld,l206,l25,&(lf->
l215.l73.l285.l472),0x0);lh(la,33,lb);l64(
SOC_PETRA_SRD_DELAY_SMALL_MSEC);l32(lq->l6.l48.l313,0x1,lz,36,lb);
l308(ld,l9);l625(&l124);l124.l432=0x1;l124.l669=
SOC_PETRA_SRD_ACK_BUSY_WAIT_ITERATIONS;l124.l725=
SOC_PETRA_SRD_ACK_TIMER_ITERATIONS;l124.l703=
SOC_PETRA_SRD_ACK_TIMER_DELAY_MSEC;la=l441(ld,l273,l17,&(lf->l144.l73
.l445.l521),&l124,&l241);lh(la,38,lb);lg(l241==lt){l19(l529,40,lb);}
lx=ln->l174;l509(ld,l25,lx);l21=l170(ld,l88(l17,l39,0));la=l176(l21,
lx,lx,&l35);lh(la,50,lb);l77=l216(&l35);lg(!l77){l19(l117,60,lb);}l32
(lq->l6.l52[l39].l391,l35.l2,lz,70,lb);l32(lq->l6.l52[l39].l370,l35.
l3,lz,72,lb);l22(l8=0;l8<l74;l8++){l18=l88(l17,l39,l8);la=l5(ld,lr,
l18,&(lf->ll.l10.l373.l359),0x0);lh(la,80+l8,lb);la=l335(ld,l18,l35.
l2,l35.l3,l21);lh(la,82+l8,lb);la=l5(ld,lr,l18,&(lf->ll.l79.l334.l362
),0x0);lh(la,50+l8,lb);}la=l5(ld,l206,l25,&(lf->l215.l73.l285.l429),
0x0);lh(la,84,lb);l64(SOC_PETRA_SRD_DELAY_MSEC);l32(lq->l6.l48.l313,
0x0,lz,90,lb);l124.l432=0x0;la=l441(ld,l273,l17,&(lf->l144.l73.l445.
l521),&l124,&l241);lh(la,100,lb);lg(l241==lt){l19(l529,102,lb);}l308(
ld,lt);l22(l8=0;l8<l74;l8++){l32(lq->l6.l81[l39][l8].l503,0x0,lz,127,
lb);l32(lq->l6.l81[l39][l8].l476,0x0,lz,131,lb);}l64(
SOC_PETRA_SRD_IPU_RESET_DELAY_MSEC);l22(l8=0;l8<l74;l8++){l18=l88(l17
,l39,l8);la=l5(ld,lr,l18,&(lf->ll.l452.l447.l415),0x1);lh(la,135,lb);
la=l5(ld,lr,l18,&(lf->ll.l177.l513.l558),0);lh(la,145,lb);la=l5(ld,lr
,l18,&(lf->ll.l177.l280.l478),0);lh(la,150,lb);la=l5(ld,lr,l18,&(lf->
ll.l177.l280.l474),0x0);lh(la,155,lb);la=l5(ld,lr,l18,&(lf->ll.l157.
l550.l542),l421);lh(la,160,lb);la=l5(ld,lr,l18,&(lf->ll.l157.l466.
l414),l419);lh(la,165,lb);la=l343(ld,l18,lx);lh(la,170,lb);}l22(l8=0;
l8<l74;l8++){l18=l88(l17,l39,l8);la=l376(ld,l18,&l358,&l354);lh(la,
110+l8,lb);lg((l358!=l115)&&(l354!=l115)){la=l518(ld,l18);lh(la,120,
lb);la=l325(ld,l18);lh(la,125,lb);}}}l22(l8=0;l8<l74;l8++){l18=l88(
l17,l39,l8);la=l5(ld,lr,l18,&(lf->ll.l53.l137.l148),l148[l8]);lh(la,
131,lb);la=l5(ld,lr,l18,&(lf->ll.l53.l137.l149),l149[l8]);lh(la,132,
lb);}l161=l9;lb:lg(l161==lt){l161=l9;l316=l168;l32(lq->l6.l48.l313,
0x0,lz,130,lb);l308(ld,lt);l168=l316;}l15("\x65\x72\x72\x6f\x72\x20"
"\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f"
"\x71\x72\x74\x74\x5f\x73\x65\x74\x5f\x75\x6e\x73\x61\x66\x65\x28\x29"
,l25,0);}le l657(lm le ld,lm le l25,lm l378*ln){l328 lc;l12(l738);l49
(ln);l283(ln);l41(l25,l153-1,l272,10,lb);lg(ln->l89==l9){lc=ln->l174;
lg(l136(lc)){l41(lc,l260-1,l117,20,lb);}lu{l276(lc,l309,l351,l117,30,
lb);}}lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f"
"\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x71\x72\x74\x74\x5f\x76\x65"
"\x72\x69\x66\x79\x28\x29",l25,0);}le l461(lm le ld,lm le l25,l40 l378
 *ln){le ly,la;le lz=l353;le l178;l243 l17,l39;l66*lq;l12(l641);l49(
ln);lq=l68();l41(l25,l153-1,l272,10,lb);l17=l279(l25);l39=l163(l25);
lz=l256(l17);l76(lq->l6.l52[l39].l294,ly,lz,145,lb);ln->l89=l300(ly);
l178=l224(ld,l25);ln->l174=l178;lb:l15("\x65\x72\x72\x6f\x72\x20\x69"
"\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x71"
"\x72\x74\x74\x5f\x67\x65\x74\x5f\x75\x6e\x73\x61\x66\x65\x28\x29",
l25,0);}le l666(lm le ld,lm l304*ln){le la=l114;le l17,lo,l8,l198;lv
l116=lt,l635,l695;l12(l642);l49(ln);l201(ld,lt);l22(l17=l336;l17<l318
;l17++){lg(ln->l154[l17].l59==l9){la=l566(ld,l17,&(ln->l154[l17].l38));
lh(la,10,lb);}}l22(lo=0;lo<l125;lo++){l8=l208(lo);lg(ln->l24[lo].l59){
la=l343(ld,lo,ln->l24[lo].l217);lh(la,20,lb);lg(ln->l24[lo].l118==ln
/* Petra codce. Almost not in use. Ignore coverity defects */
/* coverity[overrun-local] */
->l24[lo].l155){lg(l8==0){SOC_PETRA_SRD_SAME_IN_QRTT_TST(l118,lv,l116
);lg(l116==l9){SOC_PETRA_SRD_SAME_IN_QRTT_TST(l155,lv,l116);}
SOC_PETRA_SRD_BRDCST_IF_SAME(l9,l116);la=l133(ld,lo,l98,ln->l24[lo].
l118,l9);lh(la,30,lb);SOC_PETRA_SRD_BRDCST_IF_SAME(lt,l116);}lu{lg(
l116==lt){la=l133(ld,lo,l98,ln->l24[lo].l118,l9);lh(la,31,lb);}}}lu{
l116=lt;la=l133(ld,lo,l323,ln->l24[lo].l118,l9);lh(la,32,lb);la=l133(
ld,lo,l258,ln->l24[lo].l155,l9);lh(la,35,lb);}la=l541(ld,lo,ln->l24[
lo].l460,&(ln->l24[lo].l339));lh(la,40,lb);la=l435(ld,lo,&(ln->l24[lo
].l397.l47));lh(la,50,lb);la=l504(ld,lo,l98,ln->l24[lo].l392);lh(la,
60,lb);}lu{l116=lt;}}lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73"
"\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x61\x6c\x6c\x5f"
"\x73\x65\x74\x5f\x75\x6e\x73\x61\x66\x65\x28\x29",0,0);}le l632(lm le
ld,lm l304*ln){le la;le l17,lp,l39,lo;le l250[l153];l12(l631);l49(ln);
l22(l17=l336;l17<l318;l17++){lg(ln->l154[l17].l59){la=l418(ld,l17,&(
ln->l154[l17].l38));lh(la,10,lb);}}l22(lp=0;lp<l153;lp++){l250[lp]=
l434;}l22(lo=0;lo<l125;lo++){lg(ln->l24[lo].l59){lp=l159(lo);lg(l250[
lp]<ln->l24[lo].l217){l250[lp]=ln->l24[lo].l217;}}}l22(lo=0;lo<l125;
lo++){lg(ln->l24[lo].l59){l39=l173(lo);l17=l538(lo);lg(ln->l154[l17].
l38.l121[l39].l89==lt){l19(l640,15,lb);}la=l547(ld,lo,ln->l24[lo].
l217);lh(la,20,lb);la=l404(ld,lo,l98,ln->l24[lo].l118);lh(la,30,lb);
la=l404(ld,lo,l98,ln->l24[lo].l155);lh(la,35,lb);la=l569(ld,lo,ln->
l24[lo].l460,&(ln->l24[lo].l339));lh(la,40,lb);la=l422(ld,lo,&(ln->
l24[lo].l397.l47));lh(la,50,lb);la=l579(ld,lo,l98,ln->l24[lo].l392);
lh(la,60,lb);}}lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f"
"\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x61\x6c\x6c\x5f\x76"
"\x65\x72\x69\x66\x79\x28\x29",0,0);}le l699(lm le ld,l40 l304*ln){le
la;le l17,lp,lo;l140 l236=l740;l12(l614);l49(ln);l22(l17=l336;l17<
l318;l17++){la=l570(ld,l17,&(ln->l154[l17].l38));lh(la,10,lb);}l22(lo
=0;lo<l125;lo++){l17=l538(lo);lp=l173(lo);ln->l24[lo].l59=l9;lg(ln->
l154[l17].l38.l121[lp].l89){la=l324(ld,lo,&(ln->l24[lo].l217));lh(la,
20,lb);la=l317(ld,lo,&(ln->l24[lo].l155),&(ln->l24[lo].l118));lh(la,
30,lb);la=l557(ld,lo,l357,&(ln->l24[lo].l339));lh(la,40,lb);la=l490(
ld,lo,&(ln->l24[lo].l397.l47));lh(la,50,lb);la=l376(ld,lo,&(ln->l24[
lo].l392),&l236);lh(la,60,lb);}}lb:l15("\x65\x72\x72\x6f\x72\x20\x69"
"\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x61"
"\x6c\x6c\x5f\x67\x65\x74\x5f\x75\x6e\x73\x61\x66\x65\x28\x29",0,0);}
le l721(lm le ld,lm le li,lm l711 l530,l40 l660*l87){le la;lv l368=0,
l506=0,l398=0,l16;le l340=0;lv l305,l42=lt,l401=lt;l58*lf;l12(l702);
l49(l87);lf=l57();la=l5(ld,lr,li,&(lf->ll.l79.l438.l491),0x1);lh(la,5
,lb);la=l31(ld,lr,li,&(lf->ll.l79.l438.l491),&l16);lh(la,10,lb);l305=
!l103(l16);lg(!l305){l87->l161=lt;l87->l453=
SOC_PETRA_SRD_RXLOS_NOT_PRESENT_EYE_INDICATION;}lu{l1(l530){lk l690:
l368=0x1;lj;lk l609:l398=0x1;lj;l4:l19(l655,10,lb);}la=l5(ld,lr,li,&(
lf->ll.l10.l97.l534),l368);lh(la,20,lb);la=l5(ld,lr,li,&(lf->ll.l10.
l97.l563),l398);lh(la,22,lb);la=l5(ld,lr,li,&(lf->ll.l10.l97.l573),
l506);lh(la,24,lb);la=l5(ld,lr,li,&(lf->ll.l10.l97.l440),0x0);lh(la,
32,lb);la=l5(ld,lr,li,&(lf->ll.l10.l97.l473),0x1);lh(la,30,lb);l209{
l64(SOC_PETRA_SRD_AEQ_WAIT_PERIOD_MSEC);la=l31(ld,lr,li,&(lf->ll.l10.
l97.l440),&l16);lh(la,40,lb);l42=l103(l16);l340++;l401=l103((l340>=
SOC_PETRA_SRD_AEQ_NOF_ITERATIONS));}l175(!(l42)&&(!l401));la=l5(ld,lr
,li,&(lf->ll.l10.l97.l473),0x0);lh(la,30,lb);l64(
SOC_PETRA_SRD_AEQ_STEADY_DELAY_MSEC);la=l5(ld,lr,li,&(lf->ll.l10.l97.
l534),0x0);lh(la,20,lb);la=l5(ld,lr,li,&(lf->ll.l10.l97.l563),0x0);lh
(la,22,lb);la=l5(ld,lr,li,&(lf->ll.l10.l97.l573),0x0);lh(la,24,lb);
l87->l161=l42;lg(l42){la=l463(ld,lr,li,&(lf->ll.lw.l593.l34),&l16);lh
(la,50,lb);l87->l453=l16;}}lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e"
"\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x61\x75"
"\x74\x6f\x5f\x65\x71\x75\x61\x6c\x69\x7a\x65\x5f\x75\x6e\x73\x61\x66"
"\x65\x28\x29",0,0);}le l435(lm le ld,lm le li,lm l345*ln){lv l30;le
ly,la;le lp,lo,lz;l138 l128;l58*lf;l253*l104, *l112, *l102, *l99, *
l100;l262*l223=l29;l66*lq;l12(l594);lf=l57();lq=l68();lp=l173(li);lo=
l208(li);lz=l301(li);l76(lq->l6.l81[lp][lo].l396,ly,lz,2,lb);l128=(
l138)ly;l1(l128){lk l27:l104=&(lf->ll.l10.l510.l426);l112=&(lf->ll.
l10.l222.l505);l102=&(lf->ll.l10.l270.l574);l99=&(lf->ll.l10.l265.
l586);l100=&(lf->ll.l10.l222.l413);l223=&(lf->ll.l10.l222.l34);lj;lk
l26:l104=&(lf->ll.l10.l500.l484);l112=&(lf->ll.l10.l246.l517);l102=&(
lf->ll.l10.l411.l469);l99=&(lf->ll.l10.l489.l433);l100=&(lf->ll.l10.
l246.l457);l223=&(lf->ll.l10.l246.l34);lj;lk l36:l104=&(lf->ll.l10.
l578.l531);l112=&(lf->ll.l10.l207.l497);l102=&(lf->ll.l10.l486.l582);
l99=&(lf->ll.l10.l464.l535);l100=&(lf->ll.l10.l207.l475);l223=&(lf->
ll.l10.l207.l34);lj;l4:l19(l416,10,lb);}la=l5(ld,lr,li,l104,ln->l514);
lh(la,20,lb);la=l5(ld,lr,li,l102,ln->l270);lh(la,30,lb);la=l5(ld,lr,
li,l99,ln->l265);lh(la,35,lb);l30=0;la=l86(&(ln->l552),l112,&l30);lh(
la,40,lb);la=l86(&(ln->l436),l100,&l30);lh(la,45,lb);la=l131(ld,lr,li
,l223,l30);lh(la,50,lb);lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20"
"\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x72\x78\x5f"
"\x70\x68\x79\x73\x5f\x70\x61\x72\x61\x6d\x73\x5f\x73\x65\x74\x5f\x75"
"\x6e\x73\x61\x66\x65\x28\x29",li,0);}le l422(lm le ld,lm le li,lm
l345*ln){l12(l715);l49(ln);l283(ln);l41(li,l125,l237,10,lb);lb:l15(""
"\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72"
"\x61\x5f\x73\x72\x64\x5f\x72\x78\x5f\x70\x68\x79\x73\x5f\x70\x61\x72"
"\x61\x6d\x73\x5f\x76\x65\x72\x69\x66\x79\x28\x29",0,0);}le l490(lm le
ld,lm le li,l40 l345*ln){le ly,la;le lp,lo,lz;l138 l128;l58*lf;l66*lq
;l253*l104, *l112, *l102, *l99, *l100;l12(l654);lf=l57();lq=l68();lp=
l173(li);lo=l208(li);lz=l301(li);l76(lq->l6.l81[lp][lo].l396,ly,lz,2,
lb);l128=(l138)ly;l1(l128){lk l27:l104=&(lf->ll.l10.l510.l426);l112=&
(lf->ll.l10.l222.l505);l102=&(lf->ll.l10.l270.l574);l99=&(lf->ll.l10.
l265.l586);l100=&(lf->ll.l10.l222.l413);lj;lk l26:l104=&(lf->ll.l10.
l500.l484);l112=&(lf->ll.l10.l246.l517);l102=&(lf->ll.l10.l411.l469);
l99=&(lf->ll.l10.l489.l433);l100=&(lf->ll.l10.l246.l457);lj;lk l36:
l104=&(lf->ll.l10.l578.l531);l112=&(lf->ll.l10.l207.l497);l102=&(lf->
ll.l10.l486.l582);l99=&(lf->ll.l10.l464.l535);l100=&(lf->ll.l10.l207.
l475);lj;l4:l19(l416,10,lb);}la=l31(ld,lr,li,l104,&(ln->l514));lh(la,
20,lb);la=l31(ld,lr,li,l112,&(ln->l552));lh(la,25,lb);la=l31(ld,lr,li
,l102,&(ln->l270));lh(la,30,lb);la=l31(ld,lr,li,l99,&(ln->l265));lh(
la,35,lb);la=l31(ld,lr,li,l100,&(ln->l436));lh(la,40,lb);;lb:l15(""
"\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72"
"\x61\x5f\x73\x72\x64\x5f\x72\x78\x5f\x70\x68\x79\x73\x5f\x70\x61\x72"
"\x61\x6d\x73\x5f\x67\x65\x74\x5f\x75\x6e\x73\x61\x66\x65\x28\x29",0,
0);}lv l527(lm le ld,lm le li){lv l204;lg(l568(li)){l204=l9;}lu lg(
l481(li)){l204=l668(ld,l159(li));}lu{l204=lt;}l28 l204;}l349 l523*
l737(lm l138 l571){l349 l523*l156=l29;l1(l571){lk l27:l156="\x46\x55"
"\x4c\x4c";lj;lk l26:l156="\x48\x41\x4c\x46";lj;lk l36:l156="\x51\x55"
"\x41\x52\x54\x45\x52";lj;lk l239:l156="\x4e\x4f\x46\x5f\x52\x41\x54"
"\x45\x5f\x44\x49\x56\x49\x53\x4f\x52\x53";lj;l4:l156="\x20\x55\x6e"
"\x6b\x6e\x6f\x77\x6e";}l28 l156;}le l723(lm le ld,lm le lp,lm lv l59
){le la=l114;le l281,l332,l274,l247,l248;l66*lq;l58*lf;l12(l727);lq=
l68();lf=l57();l281=l295(lp);l248=l163(lp);l332=l537(lp);l32(lq->l6.
l52[l248].l647,l375(l59),l281,10,lb);l32(lq->l6.l52[l248].l758,l375(
l59),l281,12,lb);l22(l247=0;l247<l74;l247++){l274=l332+l247;la=l5(ld,
lr,l274,&(lf->ll.l53.l137.l148),l581(l59));lh(la,20,lb);la=l5(ld,lr,
l274,&(lf->ll.l53.l137.l149),l581(l59));lh(la,30,lb);}lb:l15("\x65"
"\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61"
"\x5f\x73\x72\x64\x5f\x71\x72\x74\x74\x5f\x73\x79\x6e\x63\x5f\x66\x69"
"\x66\x6f\x5f\x65\x6e\x28\x29",lp,l59);}le l691(){le la=l114;l12(l742
);la=l682();lh(la,10,lb);lb:l15("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20"
"\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x69\x6e\x69"
"\x74\x28\x29",0,0);}le l705(lm le ld,lm le li){le l322=l393,l158,la=
l114;le l122;le l21,l65;l85 l67;l12(0);l122=l159(li);l158=l224(ld,
l122);lg(l158==l187){l127;}la=l210(ld,li,&l67);lh(la,10,lb);l21=l170(
ld,li);l65=l395(ld,l21);lg(l65==l393){l127;}l322=l282(l65,l67.l2,l67.
l3,l67.ls);lb:l28 l322;}
#if ! defined __COBF__
#if l659
l240 l607(lm l141*ln){l12(0);l49(ln);l152("\x69\x70\x72\x65\x3a\x20"
"\x25\x75\n\r",ln->l75);l152("\x69\x70\x6f\x73\x74\x3a\x20\x25\x75"
"\n\r",ln->l92);l152("\x69\x6d\x61\x69\x6e\x3a\x20\x25\x75\n\r",ln->
l110);l152("\x61\x6d\x70\x3a\x20\x25\x75\n\r",ln->l43);l152("\x74\x61"
"\x70\x5f\x68\x69\x3a\x20\x25\x75\n\r",ln->l377);l152("\x74\x61\x70"
"\x5f\x6c\x6f\x77\x3a\x20\x25\x75\n\r",ln->l380);lb:l598(0,0,0);}
#endif
#endif
#undef lc
#undef l136
#undef l134
#undef l321
#undef l181
#undef l59
#undef lg
#undef l201
#undef ld
#undef l226
#undef l493
#undef l9
#undef ln
#undef l24
#undef l22
#undef l74
#undef l747
#undef l718
#undef lt
#undef l311
#undef l602
#undef l761
#undef l356
#undef l499
#undef l689
#undef l423
#undef l45
#undef l349
#undef l141
#undef l765
#undef l338
#undef le
#undef l412
#undef lm
#undef l12
#undef l752
#undef l127
#undef lb
#undef l15
#undef l726
#undef la
#undef l114
#undef l596
#undef lh
#undef l240
#undef l11
#undef l40
#undef l85
#undef l29
#undef l2
#undef l3
#undef ls
#undef l239
#undef lv
#undef l216
#undef l297
#undef lu
#undef l28
#undef l282
#undef l138
#undef l1
#undef lk
#undef l36
#undef lj
#undef l26
#undef l27
#undef l4
#undef l143
#undef l528
#undef l176
#undef l638
#undef l49
#undef l330
#undef l126
#undef l19
#undef l639
#undef l525
#undef l533
#undef l408
#undef l119
#undef l166
#undef l511
#undef l498
#undef l448
#undef l171
#undef l501
#undef l145
#undef l139
#undef l182
#undef l233
#undef l172
#undef l278
#undef l427
#undef l733
#undef l665
#undef l512
#undef l456
#undef l379
#undef l333
#undef l400
#undef l417
#undef l548
#undef l522
#undef l564
#undef l482
#undef l446
#undef l588
#undef l495
#undef l683
#undef l210
#undef li
#undef lp
#undef l66
#undef lq
#undef l732
#undef l68
#undef l173
#undef l208
#undef l301
#undef l76
#undef l6
#undef l52
#undef l391
#undef l370
#undef l81
#undef l465
#undef l366
#undef l648
#undef l716
#undef l224
#undef l159
#undef l324
#undef l187
#undef l180
#undef l170
#undef l649
#undef l663
#undef l568
#undef l589
#undef l481
#undef l624
#undef l662
#undef l546
#undef l58
#undef l229
#undef l57
#undef l163
#undef l295
#undef l88
#undef l279
#undef l5
#undef lr
#undef ll
#undef l10
#undef l373
#undef l359
#undef l79
#undef l334
#undef l362
#undef l32
#undef l576
#undef l365
#undef l291
#undef l31
#undef l736
#undef l769
#undef l157
#undef l630
#undef l591
#undef l618
#undef l168
#undef l518
#undef l262
#undef l34
#undef l209
#undef l520
#undef l551
#undef l355
#undef l64
#undef l175
#undef l572
#undef l502
#undef l430
#undef l131
#undef l480
#undef l384
#undef l728
#undef l677
#undef l620
#undef l701
#undef l710
#undef l760
#undef l555
#undef l537
#undef l713
#undef l585
#undef l89
#undef l294
#undef l300
#undef l559
#undef l153
#undef l561
#undef l693
#undef l623
#undef l477
#undef l622
#undef l734
#undef l628
#undef l755
#undef l135
#undef l189
#undef l319
#undef l371
#undef l720
#undef l637
#undef l667
#undef l103
#undef l698
#undef l704
#undef l343
#undef l685
#undef l117
#undef l396
#undef l527
#undef l547
#undef l709
#undef l41
#undef l125
#undef l237
#undef l260
#undef l276
#undef l309
#undef l351
#undef l724
#undef l434
#undef l293
#undef l93
#undef l284
#undef l200
#undef l386
#undef l328
#undef l20
#undef l75
#undef l92
#undef l43
#undef l63
#undef l14
#undef l13
#undef l292
#undef l369
#undef l364
#undef l56
#undef l268
#undef l303
#undef l194
#undef l399
#undef l483
#undef l712
#undef l110
#undef l556
#undef l494
#undef l575
#undef l314
#undef l188
#undef l479
#undef l105
#undef l299
#undef l658
#undef l444
#undef l287
#undef l78
#undef l47
#undef l577
#undef l541
#undef l352
#undef l315
#undef l253
#undef l431
#undef l130
#undef l678
#undef l656
#undef l524
#undef l387
#undef l38
#undef l565
#undef l560
#undef l329
#undef l185
#undef l627
#undef l312
#undef l449
#undef l612
#undef l357
#undef lw
#undef l587
#undef l458
#undef l249
#undef l439
#undef l696
#undef l164
#undef l269
#undef l184
#undef l735
#undef l471
#undef l597
#undef l468
#undef l604
#undef l443
#undef l437
#undef l257
#undef l584
#undef l741
#undef l147
#undef l271
#undef l169
#undef l601
#undef l532
#undef l595
#undef l543
#undef l730
#undef l583
#undef l580
#undef l267
#undef l540
#undef l652
#undef l165
#undef l264
#undef l202
#undef l729
#undef l567
#undef l613
#undef l519
#undef l680
#undef l467
#undef l86
#undef l671
#undef l675
#undef l462
#undef l380
#undef l459
#undef l377
#undef l569
#undef l617
#undef l283
#undef l766
#undef l590
#undef l664
#undef l388
#undef l634
#undef l651
#undef l599
#undef l592
#undef l650
#undef l557
#undef l750
#undef l674
#undef l307
#undef l553
#undef l670
#undef l606
#undef l621
#undef l133
#undef l288
#undef l763
#undef l442
#undef l425
#undef l375
#undef l162
#undef l488
#undef l492
#undef l428
#undef l258
#undef l53
#undef l120
#undef l544
#undef l549
#undef l323
#undef l404
#undef l611
#undef l536
#undef l539
#undef l317
#undef l155
#undef l118
#undef l751
#undef l325
#undef l762
#undef l697
#undef l768
#undef l717
#undef l731
#undef l455
#undef l608
#undef l98
#undef l748
#undef l679
#undef l485
#undef l686
#undef l629
#undef l681
#undef l757
#undef l767
#undef l756
#undef l646
#undef l152
#undef l504
#undef l140
#undef l653
#undef l115
#undef l252
#undef l545
#undef l420
#undef l331
#undef l245
#undef l579
#undef l661
#undef l706
#undef l376
#undef l633
#undef l451
#undef l243
#undef l353
#undef l516
#undef l256
#undef l381
#undef l406
#undef l48
#undef l71
#undef l450
#undef l515
#undef l526
#undef l508
#undef l600
#undef l676
#undef l672
#undef l753
#undef l754
#undef l744
#undef l673
#undef l405
#undef l688
#undef l714
#undef l643
#undef l395
#undef l393
#undef l335
#undef l342
#undef l619
#undef l603
#undef l684
#undef l759
#undef l626
#undef l507
#undef l636
#undef l566
#undef l360
#undef l700
#undef l121
#undef l174
#undef l509
#undef l266
#undef l206
#undef l215
#undef l73
#undef l615
#undef l694
#undef l739
#undef l463
#undef l273
#undef l144
#undef l190
#undef l255
#undef l616
#undef l764
#undef l645
#undef l605
#undef l687
#undef l454
#undef l707
#undef l745
#undef l424
#undef l476
#undef l487
#undef l503
#undef l452
#undef l447
#undef l415
#undef l177
#undef l513
#undef l558
#undef l280
#undef l478
#undef l474
#undef l550
#undef l542
#undef l421
#undef l466
#undef l414
#undef l419
#undef l418
#undef l644
#undef l708
#undef l272
#undef l719
#undef l570
#undef l610
#undef l722
#undef l461
#undef l743
#undef l378
#undef l161
#undef l749
#undef l148
#undef l149
#undef l746
#undef l285
#undef l429
#undef l472
#undef l137
#undef l313
#undef l308
#undef l625
#undef l432
#undef l669
#undef l725
#undef l703
#undef l441
#undef l445
#undef l521
#undef l529
#undef l657
#undef l738
#undef l641
#undef l666
#undef l304
#undef l635
#undef l695
#undef l642
#undef l336
#undef l318
#undef l154
#undef l217
#undef l460
#undef l339
#undef l435
#undef l397
#undef l392
#undef l632
#undef l631
#undef l538
#undef l640
#undef l422
#undef l699
#undef l740
#undef l614
#undef l490
#undef l721
#undef l711
#undef l530
#undef l660
#undef l401
#undef l702
#undef l438
#undef l491
#undef l453
#undef l690
#undef l609
#undef l655
#undef l97
#undef l534
#undef l563
#undef l573
#undef l440
#undef l473
#undef l593
#undef l345
#undef l594
#undef l510
#undef l426
#undef l222
#undef l505
#undef l270
#undef l574
#undef l265
#undef l586
#undef l413
#undef l500
#undef l484
#undef l246
#undef l517
#undef l411
#undef l469
#undef l489
#undef l433
#undef l457
#undef l578
#undef l531
#undef l207
#undef l497
#undef l486
#undef l582
#undef l464
#undef l535
#undef l475
#undef l416
#undef l514
#undef l552
#undef l436
#undef l715
#undef l654
#undef l668
#undef l523
#undef l737
#undef l156
#undef l723
#undef l727
#undef l647
#undef l758
#undef l581
#undef l691
#undef l742
#undef l682
#undef l705
#undef l659
#undef l607
#undef l598
#include<soc/dpp/SAND/Utils/sand_footer.h>
