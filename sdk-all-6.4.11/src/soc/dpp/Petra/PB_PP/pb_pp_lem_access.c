/* $Id: pb_pp_lem_access.c,v 1.15 Broadcom SDK $
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
*/

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/petra_general.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_lem_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_mact.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>

#ifdef USING_CHIP_SIM
  #include <sim/dpp/ChipSim/chip_sim_em.h>
#endif

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LEM_ACCESS_CMD_MAX                              (SOC_PB_PP_LEM_ACCESS_NOF_CMDS-1)
#define SOC_PB_PP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_MAC_BMAC_IPV4    (2)
#define SOC_PB_PP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_ILM              (4)

#define SOC_PB_PP_LEM_STAMP_MIN                                   (10)
#define SOC_PB_PP_LEM_STAMP_MAX                                   (250)
#define SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IN_BITS_4                   (4)


/* Payload destination parameter sizes in bits */

/* Polling before writing a CPU request */
#define SOC_PB_PP_LEM_ACCESS_BUSY_WAIT_ITERATIONS                 (50)
#define SOC_PB_PP_LEM_ACCESS_TIMER_ITERATIONS                     (10)
#define SOC_PB_PP_LEM_ACCESS_TIMER_DELAY_MSEC                     (50)

/* Field values */
#define SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_CPU_FLD_VAL              (0)
#define SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_OLP_FLD_VAL              (1)
#define SOC_PB_PP_LEM_ACCESS_ASD_FIRST_PART_NOF_BITS              (3)

#define SOC_PB_PP_LEM_ACCESS_FAIL_REASON_UNKNOWN_VSI_FLD_VAL      (0)
#define SOC_PB_PP_LEM_ACCESS_FAIL_REASON_FID_LIMIT_FLD_VAL        (1)
#define SOC_PB_PP_LEM_ACCESS_FAIL_REASON_EMC_PROBLEM_FLD_VAL      (3)

/* extend P2P */
#define SOC_PB_PP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_EXTEND_P2P       (1)

#define SOC_PB_PP_LEM_PROGRAM_P2P														      (0x0)
#define SOC_PB_PP_LEM_PROGRAM_ETH														      (0x1)
#define SOC_PB_PP_LEM_PROGRAM_TRILL_MC											      (0x2)
#define SOC_PB_PP_LEM_PROGRAM_TRILL_UC                            (0x3)
#define SOC_PB_PP_LEM_PROGRAM_IPV4UC_L3VPN									      (0x4)
#define SOC_PB_PP_LEM_PROGRAM_IPV4UC_RPF                          (0x5)
#define SOC_PB_PP_LEM_PROGRAM_IPV4UC												      (0x6)
#define SOC_PB_PP_LEM_PROGRAM_IPV4MC_L3VPN                        (0x7)
#define SOC_PB_PP_LEM_PROGRAM_IPV4MC_RPF                          (0x8)
#define SOC_PB_PP_LEM_PROGRAM_IPV6UC                              (0xa)
#define SOC_PB_PP_LEM_PROGRAM_IPV6MC                              (0xb)
#define SOC_PB_PP_LEM_PROGRAM_LSR                                 (0xc)
#define SOC_PB_PP_LEM_PROGRAM_DONT_CHANGE                         (0xd)

#define SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA                        {  0,   0,   0,   0,   0}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_LEM0                        {  1,  58,   0,   1,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_LEM1                        { 31,  66,   0,   1,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_LEM1_LSB(n)                 { n-1,  66,   0,   1,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_FID                         { 13,  85,   0,   1,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_FID12                       { 11,  85,   0,   1,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_VRF                         {  7,  56,   0,   1,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_IN_RIF                      { 11,  53,   0,   1,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_IN_PORT                     {  5,   1,   0,   1,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP                         { 31,  32,   6,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP28                       { 27,  32,   6,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP_LSB(n)                  { n-1, 32,   6,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_SIP                         { 31,  24,   6,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_SIP_LSB(n)                  { n-1, 24,   6,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_DA0                         { 31,   0,   6,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_DA1                         { 15,   4,   6,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_ESADI                       {  0,  86,   0,   1,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_6DIP0                       { 31,  72,   6,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_6DIP1                       { 31,  64,   6,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_6DIP2                       { 31,  56,   6,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_6DIP3                       { 31,  48,   6,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_6DIP0_24                    { 23,  48,   6,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_INICK_ENICK                 { 31,   4,   2,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_ENICK                       { 15,   0,   2,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_ENICK_LSB(n)                { n-1,   0,   2,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_DTNICK                      { 15,   0,   2,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_0(n)                        {  n, 107,   0,   1,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_LABEL                       { 19, 253,   6,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_LABEL_LSB(n)                { n-1, 253,   6,   0,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_CEK_ADJ_EEP13                   { 12,  91,   0,   1,   1}
#define SOC_PB_PP_LEM_INSTRUCTION_LBL_EXP                         {  2,  77,   0,   1,   1}


    /*
     * Extended
     */
#define SOC_PB_PP_L2_LIF_KEY_TYPE_EXTENDED_PARAM0_NOF_BITS                  (34)

/* nubmer of bits used for key content, ignore prefix */
#define  SOC_PB_PP_LEM_ACCESS_KEY_LEN  (SOC_DPP_LEM_WIDTH_PETRAB - 4)


/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */
/* entry index */
#define SOC_PB_PP_LEM_PROG_P2P_ID                             (0x0)
#define SOC_PB_PP_LEM_PROG_ETH_COMPATIBLE_MC_ID               (0x1)
#define SOC_PB_PP_LEM_PROG_ETH_NOT_COMPATIBLE_MC_ID           (0x2)
#define SOC_PB_PP_LEM_PROG_TRILL_MC_ID                        (0x3)
#define SOC_PB_PP_LEM_PROG_TRILL_UC_ID                        (0x4)
#define SOC_PB_PP_LEM_PROG_IPV4UC_L3VPN_EN_RPF_EN_ID          (0x5)
#define SOC_PB_PP_LEM_PROG_IPV4UC_L3VPN_EN_RPF_DIS_ID         (0x6)
#define SOC_PB_PP_LEM_PROG_IPV4UC_L3VPN_DIS_RPF_EN_ID         (0x7)
#define SOC_PB_PP_LEM_PROG_IPV4UC_L3VPN_DIS_RPF_DIS_ID        (0x8)
#define SOC_PB_PP_LEM_PROG_IPV4MC_L3VPN_EN_ID                 (0x9)
#define SOC_PB_PP_LEM_PROG_IPV4MC_L3VPN_DIS_ID                (0xa)
#define SOC_PB_PP_LEM_PROG_IPV6UC_ID                          (0xb)
#define SOC_PB_PP_LEM_PROG_IPV6MC_ID                          (0xc)
#define SOC_PB_PP_LEM_PROG_LSR_ID                             (0xd)
#define SOC_PB_PP_LEM_PROG_UD_ID1                             (0xe)
#define SOC_PB_PP_LEM_PROG_UD_ID2                             (0xf)

static SOC_PB_PP_LEM_ACCESS_INFO
  Soc_pb_pp_lem_instruction[] =
{
/*                                             prcess                         ,    or  ,and ,tcams, size, or, and ,lpm2, or  and , lpm1,    or, and ,key , LEM2, or , and ,type,key ,LEM1,elk , instB3      ,instB2      ,instB1       ,instB0       ,instA1       ,instA0 */
  {1, SOC_PB_PP_LEM_PROG_P2P_ID, SOC_PB_PP_LEM_PROGRAM_P2P,                               {0x0, 0xF, 0x0}, 0x0, {0x0, 0x0, 0x0}, {0x0, 0x0, 0x0}, {0x3, 0xF, 0x1}, 0x1, {0x5, 0xF, 0x0}, 0x0, 0x0, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_LEM1,     SOC_PB_PP_LEM_INSTRUCTION_CEK_LEM0,   SOC_PB_PP_LEM_INSTRUCTION_CEK_LEM1_LSB(25), SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,   SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA  },
  {1, SOC_PB_PP_LEM_PROG_ETH_COMPATIBLE_MC_ID, SOC_PB_PP_LEM_PROGRAM_ETH,                 {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x0}, {0x0, 0x3, 0x0}, {0x8, 0xF, 0x1}, 0x1, {0x0, 0xF, 0x1}, 0x1, 0x0, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_DA1,   SOC_PB_PP_LEM_INSTRUCTION_CEK_DA0,      SOC_PB_PP_LEM_INSTRUCTION_CEK_FID,    SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,  SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA },
  {1, SOC_PB_PP_LEM_PROG_ETH_NOT_COMPATIBLE_MC_ID, SOC_PB_PP_LEM_PROGRAM_ETH,             {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x0}, {0x0, 0x3, 0x0}, {0x8, 0xF, 0x1}, 0x1, {0x0, 0xF, 0x1}, 0x1, 0x0, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_DA1,   SOC_PB_PP_LEM_INSTRUCTION_CEK_DA0,      SOC_PB_PP_LEM_INSTRUCTION_CEK_FID,    SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,  SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA },
  {1, SOC_PB_PP_LEM_PROG_TRILL_MC_ID, SOC_PB_PP_LEM_PROGRAM_TRILL_MC,                     {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x0}, {0x0, 0x3, 0x0}, {0x0, 0xF, 0x1}, 0x1, {0x0, 0xF, 0x1}, 0x0, 0x0, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_ESADI, SOC_PB_PP_LEM_INSTRUCTION_CEK_ADJ_EEP13,  SOC_PB_PP_LEM_INSTRUCTION_CEK_FID12,  SOC_PB_PP_LEM_INSTRUCTION_CEK_INICK_ENICK, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,  SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA},
  {1, SOC_PB_PP_LEM_PROG_TRILL_UC_ID, SOC_PB_PP_LEM_PROGRAM_TRILL_UC,                     {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x0}, {0x0, 0x3, 0x0}, {0x1 , 0xF, 0x1}, 0x1, {0x0, 0xF, 0x1}, 0x0, 0x0, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_ENICK,     SOC_PB_PP_LEM_INSTRUCTION_CEK_ENICK,     SOC_PB_PP_LEM_INSTRUCTION_CEK_ENICK,   SOC_PB_PP_LEM_INSTRUCTION_CEK_ENICK_LSB(11), SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,  SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA},
  {1, SOC_PB_PP_LEM_PROG_IPV4UC_L3VPN_EN_RPF_EN_ID, SOC_PB_PP_LEM_PROGRAM_IPV4UC_RPF,   {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x1}, {0x0, 0x3, 0x1}, {0x5, 0x7, 0x1}, 0x1, {0x5, 0x7, 0x1}, 0x0, 0x1, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,     SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP,      SOC_PB_PP_LEM_INSTRUCTION_CEK_VRF,    SOC_PB_PP_LEM_INSTRUCTION_CEK_SIP,  SOC_PB_PP_LEM_INSTRUCTION_CEK_VRF  },
  {1, SOC_PB_PP_LEM_PROG_IPV4UC_L3VPN_EN_RPF_DIS_ID, SOC_PB_PP_LEM_PROGRAM_IPV4UC_L3VPN,  {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x1}, {0x0, 0x3, 0x1}, {0x5, 0x7, 0x1}, 0x1, {0x5, 0x7, 0x1}, 0x0, 0x1, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,     SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP,      SOC_PB_PP_LEM_INSTRUCTION_CEK_VRF,    SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,  SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP  },
  /*                                             prcess                         ,    or  ,and ,tcams, size, or, and ,lpm2, or  and , lpm1,    or, and ,key , LEM2, or , and ,type,key ,LEM1,elk , instB3      ,instB2      ,instB1       ,instB0       ,instA1       ,instA0 */
  {1, SOC_PB_PP_LEM_PROG_IPV4UC_L3VPN_DIS_RPF_EN_ID, SOC_PB_PP_LEM_PROGRAM_IPV4UC_RPF,    {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x1}, {0x0, 0x3, 0x1}, {0x5, 0x7, 0x1}, 0x1, {0x5, 0x7, 0x1}, 0x0, 0x1, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,     SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP,      SOC_PB_PP_LEM_INSTRUCTION_CEK_VRF,    SOC_PB_PP_LEM_INSTRUCTION_CEK_SIP,   SOC_PB_PP_LEM_INSTRUCTION_CEK_VRF  },
  {1, SOC_PB_PP_LEM_PROG_IPV4UC_L3VPN_DIS_RPF_DIS_ID, SOC_PB_PP_LEM_PROGRAM_IPV4UC,       {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x1}, {0x0, 0x3, 0x0}, {0x5, 0x7, 0x1}, 0x1, {0x0, 0xF, 0x1}, 0x1, 0x0, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,     SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP,      SOC_PB_PP_LEM_INSTRUCTION_CEK_VRF,    SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,  SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA },
  {1, SOC_PB_PP_LEM_PROG_IPV4MC_L3VPN_EN_ID, SOC_PB_PP_LEM_PROGRAM_IPV4MC_L3VPN,          {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x1}, {0x0, 0x3, 0x0}, {0x8, 0x7, 0x1}, 0x0, {0x0, 0xF, 0x0}, 0x0, 0x0, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,     SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP,      SOC_PB_PP_LEM_INSTRUCTION_CEK_VRF,    SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,  SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP  },
  /*                                             prcess                         ,    or  ,and ,tcams, size, or, and ,lpm2, or  and , lpm1,    or, and ,key , LEM2, or , and ,type,key ,LEM1,elk , instB3      ,instB2      ,instB1       ,instB0       ,instA1       ,instA0 */
  {1, SOC_PB_PP_LEM_PROG_IPV4MC_L3VPN_DIS_ID, SOC_PB_PP_LEM_PROGRAM_IPV4MC_RPF,           {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x0}, {0x0, 0x3, 0x1}, {0x5, 0xF, 0x1}, 0x1, {0x5, 0x7, 0x1}, 0x0, 0x1, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP28,    SOC_PB_PP_LEM_INSTRUCTION_CEK_SIP,      SOC_PB_PP_LEM_INSTRUCTION_CEK_IN_RIF, SOC_PB_PP_LEM_INSTRUCTION_CEK_SIP,   SOC_PB_PP_LEM_INSTRUCTION_CEK_VRF  },
  {1, SOC_PB_PP_LEM_PROG_IPV6UC_ID, SOC_PB_PP_LEM_PROGRAM_IPV6UC,                         {0x0, 0xF, 0x0}, 0x1, {0x0, 0x3, 0x0}, {0x0, 0x3, 0x0}, {0x0, 0xF, 0x0}, 0x0, {0x0, 0xF, 0x0}, 0x0, 0x0, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_6DIP2,    SOC_PB_PP_LEM_INSTRUCTION_CEK_6DIP3,    SOC_PB_PP_LEM_INSTRUCTION_CEK_VRF,    SOC_PB_PP_LEM_INSTRUCTION_CEK_6DIP0, SOC_PB_PP_LEM_INSTRUCTION_CEK_6DIP1},
  {1, SOC_PB_PP_LEM_PROG_IPV6MC_ID, SOC_PB_PP_LEM_PROGRAM_IPV6MC,                         {0x0, 0xF, 0x0}, 0x1, {0x0, 0x3, 0x0}, {0x0, 0x3, 0x0}, {0x0, 0xF, 0x0}, 0x0, {0x0, 0xF, 0x0}, 0x0, 0x0, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_6DIP2,    SOC_PB_PP_LEM_INSTRUCTION_CEK_6DIP0_24, SOC_PB_PP_LEM_INSTRUCTION_CEK_IN_RIF, SOC_PB_PP_LEM_INSTRUCTION_CEK_6DIP0, SOC_PB_PP_LEM_INSTRUCTION_CEK_6DIP1},
  {1, SOC_PB_PP_LEM_PROG_LSR_ID, SOC_PB_PP_LEM_PROGRAM_LSR,                               {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x0}, {0x0, 0x3, 0x0}, {0x6, 0x3, 0x1}, 0x1, {0x0, 0xF, 0x1}, 0x1, 0x1, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_LABEL, SOC_PB_PP_LEM_INSTRUCTION_LBL_EXP,     SOC_PB_PP_LEM_INSTRUCTION_CEK_LABEL,SOC_PB_PP_LEM_INSTRUCTION_CEK_LABEL_LSB(12),  SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,  SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA },
  /*{1, SOC_PB_PP_LEM_PROG_UD_ID1, SOC_PB_PP_LEM_PROGRAM_ETH,                               {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x0}, {0x0, 0x3, 0x0}, {0x4, 0xF, 0x1}, 0x1, {0x0, 0xF, 0x1}, 0x1, 0x0, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_DA1,   SOC_PB_PP_LEM_INSTRUCTION_CEK_DA0,      SOC_PB_PP_LEM_INSTRUCTION_CEK_FID,    SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,  SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA },*/
  {1, SOC_PB_PP_LEM_PROG_UD_ID1, SOC_PB_PP_LEM_PROGRAM_DONT_CHANGE,               {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x0}, {0x0, 0x3, 0x0}, {0x4, 0xF, 0x1}, 0x1, {0x0, 0xF, 0x0}, 0x1, 0x0, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_DA1,   SOC_PB_PP_LEM_INSTRUCTION_CEK_DA0,      SOC_PB_PP_LEM_INSTRUCTION_CEK_FID,    SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,  SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA },
  {1, SOC_PB_PP_LEM_PROG_UD_ID2, SOC_PB_PP_LEM_PROGRAM_ETH,                       {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x0}, {0x0, 0x3, 0x0}, {0x4, 0xF, 0x1}, 0x1, {0x0, 0xF, 0x0}, 0x1, 0x0, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_DA1,   SOC_PB_PP_LEM_INSTRUCTION_CEK_DA0,      SOC_PB_PP_LEM_INSTRUCTION_CEK_FID,    SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,  SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA },
};

/* programs in case no vrfs, only one routing table */
static SOC_PB_PP_LEM_ACCESS_INFO
  Soc_pb_pp_lem_instruction_no_vrfs[] =
{
  {1, SOC_PB_PP_LEM_PROG_IPV4UC_L3VPN_DIS_RPF_EN_ID, SOC_PB_PP_LEM_PROGRAM_IPV4UC_RPF,    {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x1}, {0x0, 0x3, 0x1}, {0x5, 0x7, 0x1}, 0x1, {0x5, 0x7, 0x1}, 0x0, 0x1, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,     SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP,      SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP_LSB(27),    SOC_PB_PP_LEM_INSTRUCTION_CEK_SIP,   SOC_PB_PP_LEM_INSTRUCTION_CEK_SIP_LSB(27)  },
  {1, SOC_PB_PP_LEM_PROG_IPV4UC_L3VPN_DIS_RPF_DIS_ID, SOC_PB_PP_LEM_PROGRAM_IPV4UC,       {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x1}, {0x0, 0x3, 0x0}, {0x5, 0x7, 0x1}, 0x1, {0x0, 0xF, 0x1}, 0x1, 0x0, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,     SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP,      SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP_LSB(27),    SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA,  SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA },
  {1, SOC_PB_PP_LEM_PROG_IPV4MC_L3VPN_DIS_ID, SOC_PB_PP_LEM_PROGRAM_IPV4MC_RPF,           {0x0, 0xF, 0x0}, 0x0, {0x0, 0x3, 0x0}, {0x0, 0x3, 0x1}, {0x5, 0xF, 0x1}, 0x1, {0x5, 0x7, 0x1}, 0x0, 0x1, 0x0, SOC_PB_PP_LEM_INSTRUCTION_CEK_INVA, SOC_PB_PP_LEM_INSTRUCTION_CEK_DIP28,    SOC_PB_PP_LEM_INSTRUCTION_CEK_SIP,      SOC_PB_PP_LEM_INSTRUCTION_CEK_IN_RIF, SOC_PB_PP_LEM_INSTRUCTION_CEK_SIP,   SOC_PB_PP_LEM_INSTRUCTION_CEK_SIP_LSB(27)},
};

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_lem_access[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_ACCESS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_ACCESS_GET_ERRS_PTR),

  /*
   * } Auto generated. Do not edit previous section.
   */

   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_KEY_ENCODED_BUILD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_ACCESS_PAYLOAD_BUILD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_KEY_ENCODED_PARSE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_REQUEST_SEND),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_REQUEST_ANSWER_RECEIVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_ACCESS_ENTRY_ADD_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_ACCESS_ENTRY_REMOVE_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_ACCESS_ENTRY_BY_KEY_GET_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_ACCESS_ENTRY_BY_INDEX_GET_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_ACCESS_AGE_FLD_SET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_ACCESS_AGE_FLD_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_ACCESS_SA_BASED_ASD_BUILD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_ACCESS_SA_BASED_ASD_PARSE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LEM_ILM_KEY_BUILD_SET),

   /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_lem_access[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_LEM_ACCESS_KEY_PARAM_SIZE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LEM_ACCESS_KEY_PARAM_SIZE_OUT_OF_RANGE_ERR",
    "In the key construction, a parameter has a size out of range. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_OUT_OF_RANGE_ERR",
    "In the key construction, the sum of the parameter size \n\r "
    "with the prefix size is greater than the maximal key size. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR,
    "SOC_PB_PP_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR",
    "The retrieved key has an unknown key prefix. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
    {
      SOC_PB_PP_LEM_ENTRY_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LEM_ENTRY_INDEX_OUT_OF_RANGE_ERR",
    "'entry_ndx' is out of range. \n\r "
    "The range is: 0 to 64K+32-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR",
    "'command' is out of range. \n\r "
    "The range is: 0 to SOC_PB_PP_LEM_ACCESS_NOF_CMDS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_STAMP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LEM_STAMP_OUT_OF_RANGE_ERR",
    "'stamp' is out of range. \n\r "
    "The stamp must be not set (set to zero).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR",
    "'type' is out of range. \n\r "
    "The range is: 0 to SOC_PB_PP_LEM_ACCESS_NOF_KEY_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR",
    "'nof_params' is out of range. \n\r "
    "The range is set according to the type.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR",
    "'nof_bits' is out of range. \n\r "
    "The range is set according to the type.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR",
    "'prefix' is out of range. \n\r "
    "The range is set according to the type.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR",
    "'nof_bits' is out of range. \n\r "
    "The range is set according to the type.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR",
    "'value' is out of range. \n\r "
    "The range is set according to the type.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_ASD_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_LEM_ASD_OUT_OF_RANGE_ERR",
    "'asd' is out of range. \n\r "
    "The range is: 0 to 1<<24-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_AGE_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_LEM_AGE_OUT_OF_RANGE_ERR",
    "'age' is out of range. \n\r "
    "The range is: 0 to 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_DEST_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_LEM_DEST_OUT_OF_RANGE_ERR",
    "'dest' is out of range. \n\r "
    "The range is: 0 to 1<<16-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
    {
      SOC_PB_PP_LEM_ACCESS_LOOKUP_POLL_TIMEOUT_ERR,
    "SOC_PB_PP_LEM_ACCESS_LOOKUP_POLL_TIMEOUT_ERR",
    "The polling on the bit indicating \n\r "
    "the end of the lookup has failed.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_OUT_OF_RANGE_ERR",
    "'req_origin' is out of range. \n\r "
    "The range is: 0 to SOC_PB_PP_LEM_ACCCESS_NOF_REQ_ORIGINS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR,
      "SOC_PB_PP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR",
    "'reason' is out of range. \n\r "
    "The range is: 0,1 and 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_ACCESS_SA_BASED_ILLEGAL_VID_ERR,
    "SOC_PB_PP_LEM_ACCESS_SA_BASED_ILLEGAL_VID_ERR",
    " try to build ASD according to both SA-auth \n\r "
    " and VID-assign while VID value is not equal in both.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LEM_ACCESS_PROGRAM_NOT_FOUND,
    "SOC_PB_PP_LEM_ACCESS_PROGRAM_NOT_FOUND",
    " the requested FLP program was not found\n\r "
    " in the FLP key program map.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },



  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

static uint32
  Soc_pb_pp_lem_actual_stamp[SOC_SAND_MAX_DEVICE] = {SOC_PB_PP_LEM_STAMP_MIN};

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_pp_lem_access_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    reg_val,
    idx;
  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA
    ihb_flp_key_program_map_tbl_data;
  SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_DATA
    program_translation_map_tbl_data;
  uint32 nof_vrfs;
  SOC_PB_PP_REGS
    *pp_regs;
  uint32
    res = SOC_SAND_OK;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* enable forwarding lookup for AC-MP PWE-MP, and ISID-VRL MP */
  pp_regs = soc_pb_pp_regs();

  reg_val = 0x68;
  SOC_PB_PP_REG_SET(pp_regs->ihp.asd_ac_prefix_reg, reg_val, 5, exit);


  for (idx = 0; idx < sizeof(Soc_pb_pp_lem_instruction) / sizeof(SOC_PB_PP_LEM_ACCESS_INFO); ++idx)
  {
    if (Soc_pb_pp_lem_instruction[idx].valid)
    {
      ihb_flp_key_program_map_tbl_data.key_a_instruction0 = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_lem_instruction[idx].key_a_instruction0);
      ihb_flp_key_program_map_tbl_data.key_a_instruction1 = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_lem_instruction[idx].key_a_instruction1);
      ihb_flp_key_program_map_tbl_data.key_b_instruction0 = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_lem_instruction[idx].key_b_instruction0);
      ihb_flp_key_program_map_tbl_data.key_b_instruction1 = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_lem_instruction[idx].key_b_instruction1);
      ihb_flp_key_program_map_tbl_data.key_b_instruction2 = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_lem_instruction[idx].key_b_instruction2);
      ihb_flp_key_program_map_tbl_data.key_b_instruction3 = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_lem_instruction[idx].key_b_instruction3);
      ihb_flp_key_program_map_tbl_data.data_processing_profile = Soc_pb_pp_lem_instruction[idx].data_processing_profile;
      ihb_flp_key_program_map_tbl_data.tcam_or_value = Soc_pb_pp_lem_instruction[idx].tcam.or_value;
      ihb_flp_key_program_map_tbl_data.tcam_and_value = Soc_pb_pp_lem_instruction[idx].tcam.and_value;
      ihb_flp_key_program_map_tbl_data.tcam_bank_valid = Soc_pb_pp_lem_instruction[idx].tcam.valid;
      ihb_flp_key_program_map_tbl_data.tcam_key_size = Soc_pb_pp_lem_instruction[idx].tcam_key_size;
      ihb_flp_key_program_map_tbl_data.lpm_2nd_lkp_or_value = Soc_pb_pp_lem_instruction[idx].lpm_2nd_lkp.or_value;
      ihb_flp_key_program_map_tbl_data.lpm_2nd_lkp_and_value = Soc_pb_pp_lem_instruction[idx].lpm_2nd_lkp.and_value;
      ihb_flp_key_program_map_tbl_data.lpm_2nd_lkp_valid = Soc_pb_pp_lem_instruction[idx].lpm_2nd_lkp.valid;
      ihb_flp_key_program_map_tbl_data.lpm_1st_lkp_or_value = Soc_pb_pp_lem_instruction[idx].lpm_1st_lkp.or_value;
      ihb_flp_key_program_map_tbl_data.lpm_1st_lkp_and_value = Soc_pb_pp_lem_instruction[idx].lpm_1st_lkp.and_value;
      ihb_flp_key_program_map_tbl_data.lpm_1st_lkp_valid = Soc_pb_pp_lem_instruction[idx].lpm_1st_lkp.valid;
      ihb_flp_key_program_map_tbl_data.lem_2nd_lkp_or_value = Soc_pb_pp_lem_instruction[idx].lem_2nd_lkp.or_value;
      ihb_flp_key_program_map_tbl_data.lem_2nd_lkp_and_value = Soc_pb_pp_lem_instruction[idx].lem_2nd_lkp.and_value;
      ihb_flp_key_program_map_tbl_data.lem_2nd_lkp_key_select = Soc_pb_pp_lem_instruction[idx].lem_2nd_lkp_key_select;
      ihb_flp_key_program_map_tbl_data.lem_2nd_lkp_valid = Soc_pb_pp_lem_instruction[idx].lem_2nd_lkp.valid;
      ihb_flp_key_program_map_tbl_data.lem_1st_lkp_or_value = Soc_pb_pp_lem_instruction[idx].lem_1st_lkp.or_value;
      ihb_flp_key_program_map_tbl_data.lem_1st_lkp_and_value = Soc_pb_pp_lem_instruction[idx].lem_1st_lkp.and_value;
      ihb_flp_key_program_map_tbl_data.lem_1st_lkp_key_type = Soc_pb_pp_lem_instruction[idx].lem_1st_lkp_key_type;
      ihb_flp_key_program_map_tbl_data.lem_1st_lkp_key_select = Soc_pb_pp_lem_instruction[idx].lem_1st_lkp_key_select;
      ihb_flp_key_program_map_tbl_data.lem_1st_lkp_valid = Soc_pb_pp_lem_instruction[idx].lem_1st_lkp.valid;
      ihb_flp_key_program_map_tbl_data.elk_lkp_valid = Soc_pb_pp_lem_instruction[idx].elk_lkp_valid;
      res = soc_pb_pp_ihb_flp_key_program_map_tbl_set_unsafe(
              unit,
              Soc_pb_pp_lem_instruction[idx].prog_idx,
              &ihb_flp_key_program_map_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
  }

  res = soc_pb_pp_sw_db_ipv4_nof_vrfs_get(unit, &nof_vrfs);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  if (nof_vrfs <= 1)
  {
    /* in case no VRF in system (except VRF zero), then load programs with wider keys */
    for (idx = 0; idx < sizeof(Soc_pb_pp_lem_instruction_no_vrfs) / sizeof(SOC_PB_PP_LEM_ACCESS_INFO); ++idx)
    {
      if (Soc_pb_pp_lem_instruction_no_vrfs[idx].valid)
      {
        ihb_flp_key_program_map_tbl_data.key_a_instruction0 = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_lem_instruction_no_vrfs[idx].key_a_instruction0);
        ihb_flp_key_program_map_tbl_data.key_a_instruction1 = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_lem_instruction_no_vrfs[idx].key_a_instruction1);
        ihb_flp_key_program_map_tbl_data.key_b_instruction0 = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_lem_instruction_no_vrfs[idx].key_b_instruction0);
        ihb_flp_key_program_map_tbl_data.key_b_instruction1 = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_lem_instruction_no_vrfs[idx].key_b_instruction1);
        ihb_flp_key_program_map_tbl_data.key_b_instruction2 = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_lem_instruction_no_vrfs[idx].key_b_instruction2);
        ihb_flp_key_program_map_tbl_data.key_b_instruction3 = soc_pb_pp_ce_instruction_build(&Soc_pb_pp_lem_instruction_no_vrfs[idx].key_b_instruction3);
        ihb_flp_key_program_map_tbl_data.data_processing_profile = Soc_pb_pp_lem_instruction_no_vrfs[idx].data_processing_profile;
        ihb_flp_key_program_map_tbl_data.tcam_or_value = Soc_pb_pp_lem_instruction_no_vrfs[idx].tcam.or_value;
        ihb_flp_key_program_map_tbl_data.tcam_and_value = Soc_pb_pp_lem_instruction_no_vrfs[idx].tcam.and_value;
        ihb_flp_key_program_map_tbl_data.tcam_bank_valid = Soc_pb_pp_lem_instruction_no_vrfs[idx].tcam.valid;
        ihb_flp_key_program_map_tbl_data.tcam_key_size = Soc_pb_pp_lem_instruction_no_vrfs[idx].tcam_key_size;
        ihb_flp_key_program_map_tbl_data.lpm_2nd_lkp_or_value = Soc_pb_pp_lem_instruction_no_vrfs[idx].lpm_2nd_lkp.or_value;
        ihb_flp_key_program_map_tbl_data.lpm_2nd_lkp_and_value = Soc_pb_pp_lem_instruction_no_vrfs[idx].lpm_2nd_lkp.and_value;
        ihb_flp_key_program_map_tbl_data.lpm_2nd_lkp_valid = Soc_pb_pp_lem_instruction_no_vrfs[idx].lpm_2nd_lkp.valid;
        ihb_flp_key_program_map_tbl_data.lpm_1st_lkp_or_value = Soc_pb_pp_lem_instruction_no_vrfs[idx].lpm_1st_lkp.or_value;
        ihb_flp_key_program_map_tbl_data.lpm_1st_lkp_and_value = Soc_pb_pp_lem_instruction_no_vrfs[idx].lpm_1st_lkp.and_value;
        ihb_flp_key_program_map_tbl_data.lpm_1st_lkp_valid = Soc_pb_pp_lem_instruction_no_vrfs[idx].lpm_1st_lkp.valid;
        ihb_flp_key_program_map_tbl_data.lem_2nd_lkp_or_value = Soc_pb_pp_lem_instruction_no_vrfs[idx].lem_2nd_lkp.or_value;
        ihb_flp_key_program_map_tbl_data.lem_2nd_lkp_and_value = Soc_pb_pp_lem_instruction_no_vrfs[idx].lem_2nd_lkp.and_value;
        ihb_flp_key_program_map_tbl_data.lem_2nd_lkp_key_select = Soc_pb_pp_lem_instruction_no_vrfs[idx].lem_2nd_lkp_key_select;
        ihb_flp_key_program_map_tbl_data.lem_2nd_lkp_valid = Soc_pb_pp_lem_instruction_no_vrfs[idx].lem_2nd_lkp.valid;
        ihb_flp_key_program_map_tbl_data.lem_1st_lkp_or_value = Soc_pb_pp_lem_instruction_no_vrfs[idx].lem_1st_lkp.or_value;
        ihb_flp_key_program_map_tbl_data.lem_1st_lkp_and_value = Soc_pb_pp_lem_instruction_no_vrfs[idx].lem_1st_lkp.and_value;
        ihb_flp_key_program_map_tbl_data.lem_1st_lkp_key_type = Soc_pb_pp_lem_instruction_no_vrfs[idx].lem_1st_lkp_key_type;
        ihb_flp_key_program_map_tbl_data.lem_1st_lkp_key_select = Soc_pb_pp_lem_instruction_no_vrfs[idx].lem_1st_lkp_key_select;
        ihb_flp_key_program_map_tbl_data.lem_1st_lkp_valid = Soc_pb_pp_lem_instruction_no_vrfs[idx].lem_1st_lkp.valid;
        ihb_flp_key_program_map_tbl_data.elk_lkp_valid = Soc_pb_pp_lem_instruction_no_vrfs[idx].elk_lkp_valid;
        res = soc_pb_pp_ihb_flp_key_program_map_tbl_set_unsafe(
          unit,
          Soc_pb_pp_lem_instruction_no_vrfs[idx].prog_idx,
          &ihb_flp_key_program_map_tbl_data
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      }
    }
  }


  for (idx = 0; idx < 15; ++idx)
  {
    program_translation_map_tbl_data.program00 = idx;
    program_translation_map_tbl_data.program01 = idx;
    program_translation_map_tbl_data.program10 = idx;
    program_translation_map_tbl_data.program11 = idx;
    /* write to mim program */
    if(idx == 1 || idx == 2)
    {
      program_translation_map_tbl_data.program11 = SOC_PB_PP_LEM_PROG_UD_ID2;
    }
    res = soc_pb_pp_ihb_program_translation_map_tbl_set_unsafe(
            unit,
            idx,
            &program_translation_map_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_init_unsafe()", 0, 0);
}

STATIC
  uint32
    soc_pb_pp_lem_access_flp_program_map_entry_get(
      SOC_SAND_IN  int                              unit,
      SOC_SAND_IN  uint32                               data_processing_profile,
      SOC_SAND_OUT uint32                              *offset,
      SOC_SAND_OUT SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA *entry)
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_FLP_PROGRAM_MAP_ENTRY_GET);

  for (*offset = 0; *offset < sizeof(Soc_pb_pp_lem_instruction) / sizeof(SOC_PB_PP_LEM_ACCESS_INFO); ++*offset)
  {
    res = soc_pb_pp_ihb_flp_key_program_map_tbl_get_unsafe(
            unit,
            *offset,
            entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (entry->data_processing_profile == data_processing_profile)
    {
      SOC_PB_PP_DO_NOTHING_AND_EXIT;
    }
  }

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_PROGRAM_NOT_FOUND, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_flp_program_map_entry_get()", 0, 0);
}

STATIC
  uint32
    soc_pb_pp_lem_access_tcam_bank_bitmap_set(
      SOC_SAND_IN  int unit,
      SOC_SAND_IN  uint32  data_processing_profile,
      SOC_SAND_IN  uint32  bitmap
    )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset;
  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA
    ihb_flp_key_program_map_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_TCAM_BANK_BITMAP_SET);

  res = soc_pb_pp_lem_access_flp_program_map_entry_get(
          unit,
          data_processing_profile,
          &offset,
          &ihb_flp_key_program_map_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ihb_flp_key_program_map_tbl_data.tcam_bank_valid = bitmap;
  res = soc_pb_pp_ihb_flp_key_program_map_tbl_set_unsafe(
          unit,
          offset,
          &ihb_flp_key_program_map_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_tcam_bank_bitmap_set()", 0, 0);
}

STATIC
  uint32
    soc_pb_pp_lem_access_tcam_bank_bitmap_get(
      SOC_SAND_IN  int unit,
      SOC_SAND_IN  uint32  data_processing_profile,
      SOC_SAND_OUT uint32  *bitmap
    )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset;
  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA
    ihb_flp_key_program_map_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_TCAM_BANK_BITMAP_GET);

  res = soc_pb_pp_lem_access_flp_program_map_entry_get(
          unit,
          data_processing_profile,
          &offset,
          &ihb_flp_key_program_map_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *bitmap = ihb_flp_key_program_map_tbl_data.tcam_bank_valid;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_tcam_bank_bitmap_get()", 0, 0);
}

uint32
  soc_pb_pp_lem_access_ipv6_bank_bitmap_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN SOC_PB_TCAM_USER type,
    SOC_SAND_IN uint32     bitmap
  )
{
  uint32
    profile,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_IPV6_BANK_BITMAP_SET);

  switch (type)
  {
  case SOC_PB_TCAM_USER_FWDING_IPV6_UC:
    profile = SOC_PB_PP_LEM_PROGRAM_IPV6UC;
    break;

  case SOC_PB_TCAM_USER_FWDING_IPV6_MC:
    profile = SOC_PB_PP_LEM_PROGRAM_IPV6MC;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = soc_pb_pp_lem_access_tcam_bank_bitmap_set(
          unit,
          profile,
          bitmap
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_ipv6_bank_bitmap_set()", 0, 0);
}

uint32
  soc_pb_pp_lem_access_ipv4_mc_bank_bitmap_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  bitmap
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_pp_lem_access_tcam_bank_bitmap_set(
          unit,
          SOC_PB_PP_LEM_PROGRAM_IPV4MC_L3VPN,
          bitmap
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_lem_access_tcam_bank_bitmap_set(
          unit,
          SOC_PB_PP_LEM_PROGRAM_IPV4MC_RPF,
          bitmap
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_ipv4_mc_bank_bitmap_set()", 0, 0);
}

uint32
  soc_pb_pp_lem_access_ipv6_bank_bitmap_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  SOC_PB_TCAM_USER type,
    SOC_SAND_OUT uint32     *bitmap
  )
{
  uint32
    profile,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_IPV6_BANK_BITMAP_GET);

  switch (type)
  {
  case SOC_PB_TCAM_USER_FWDING_IPV6_UC:
    profile = SOC_PB_PP_LEM_PROGRAM_IPV6UC;
    break;

  case SOC_PB_TCAM_USER_FWDING_IPV6_MC:
    profile = SOC_PB_PP_LEM_PROGRAM_IPV6MC;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = soc_pb_pp_lem_access_tcam_bank_bitmap_get(
          unit,
          profile,
          bitmap
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_ipv6_bank_bitmap_get()", 0, 0);
}

STATIC
  uint32
    soc_pb_pp_lem_access_tcam_prefix_set(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32  data_processing_profile,
      SOC_SAND_IN uint32  prefix,
      SOC_SAND_IN uint32 nof_bits_in_prefix
    )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    prefix_offset,
    offset;
  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA
    ihb_flp_key_program_map_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_TCAM_PREFIX_SET);

  if ((nof_bits_in_prefix > 4) || ((prefix & 0xf) != prefix))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_PREFIX_TOO_LONG, 10, exit);
  }

  res = soc_pb_pp_lem_access_flp_program_map_entry_get(
          unit,
          data_processing_profile,
          &offset,
          &ihb_flp_key_program_map_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  prefix_offset = 4 - nof_bits_in_prefix;
  ihb_flp_key_program_map_tbl_data.tcam_and_value = (nof_bits_in_prefix < 4) ? SOC_SAND_BITS_MASK(prefix_offset - 1, 0) : 0;
  ihb_flp_key_program_map_tbl_data.tcam_or_value = prefix << prefix_offset;
  res = soc_pb_pp_ihb_flp_key_program_map_tbl_set_unsafe(
          unit,
          offset,
          &ihb_flp_key_program_map_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_tcam_prefix_set()", 0, 0);
}

STATIC
  uint32
    soc_pb_pp_lem_access_tcam_prefix_get(
      SOC_SAND_IN  int unit,
      SOC_SAND_IN  uint32  data_processing_profile,
      SOC_SAND_OUT uint32  *prefix,
      SOC_SAND_OUT uint32 *nof_bits_in_prefix
    )
{
  uint32
    and_val,
    or_val,
    res = SOC_SAND_OK;
  uint32
    prefix_offset,
    offset;
  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA
    ihb_flp_key_program_map_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_TCAM_PREFIX_GET);

  res = soc_pb_pp_lem_access_flp_program_map_entry_get(
          unit,
          data_processing_profile,
          &offset,
          &ihb_flp_key_program_map_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  and_val = ihb_flp_key_program_map_tbl_data.tcam_and_value;
  or_val = ihb_flp_key_program_map_tbl_data.tcam_or_value;

  switch (and_val)
  {
  case 0x0:
    *nof_bits_in_prefix = 4;
    break;
  case 0x1:
    *nof_bits_in_prefix = 3;
    break;
  case 0x3:
    *nof_bits_in_prefix = 2;
    break;
  case 0x7:
    *nof_bits_in_prefix = 1;
    break;
  case 0xf:
    *nof_bits_in_prefix = 0;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_MALFORMED_PREFIX, 20, exit);
  }

  if ((or_val & and_val) != 0) {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_MALFORMED_PREFIX, 30, exit);
  }

  prefix_offset = 4 - *nof_bits_in_prefix;
  *prefix = or_val >> prefix_offset;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_tcam_prefix_get()", 0, 0);
}

uint32
  soc_pb_pp_lem_access_ipv4_mc_tcam_prefix_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  prefix,
    SOC_SAND_IN uint32 nof_bits_in_prefix
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_pp_lem_access_tcam_prefix_set(
          unit,
          SOC_PB_PP_LEM_PROGRAM_IPV4MC_L3VPN,
          prefix,
          nof_bits_in_prefix
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_lem_access_tcam_prefix_set(
          unit,
          SOC_PB_PP_LEM_PROGRAM_IPV4MC_RPF,
          prefix,
          nof_bits_in_prefix
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_ipv6_tcam_prefix_set()", 0, 0);
}

uint32
  soc_pb_pp_lem_access_ipv6_tcam_prefix_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN SOC_PB_TCAM_USER type,
    SOC_SAND_IN uint32     prefix,
    SOC_SAND_IN uint32    nof_bits_in_prefix
  )
{
  uint32
    profile,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_IPV6_TCAM_PREFIX_SET);

  switch (type)
  {
  case SOC_PB_TCAM_USER_FWDING_IPV6_UC:
    profile = SOC_PB_PP_LEM_PROGRAM_IPV6UC;
    break;
  case SOC_PB_TCAM_USER_FWDING_IPV6_MC:
    profile = SOC_PB_PP_LEM_PROGRAM_IPV6MC;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = soc_pb_pp_lem_access_tcam_prefix_set(
          unit,
          profile,
          prefix,
          nof_bits_in_prefix
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_ipv6_tcam_prefix_set()", 0, 0);
}

uint32
  soc_pb_pp_lem_access_ipv6_tcam_prefix_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  SOC_PB_TCAM_USER type,
    SOC_SAND_OUT uint32     *prefix,
    SOC_SAND_OUT uint32    *nof_bits_in_prefix
  )
{
  uint32
    profile,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_IPV6_TCAM_PREFIX_GET);

  switch (type)
  {
  case SOC_PB_TCAM_USER_FWDING_IPV6_UC:
    profile = SOC_PB_PP_LEM_PROGRAM_IPV6UC;
    break;
  case SOC_PB_TCAM_USER_FWDING_IPV6_MC:
    profile = SOC_PB_PP_LEM_PROGRAM_IPV6MC;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = soc_pb_pp_lem_access_tcam_prefix_get(
          unit,
          profile,
          prefix,
          nof_bits_in_prefix
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_ipv6_tcam_prefix_get()", 0, 0);
}

STATIC uint32
  soc_pb_pp_lem_access_age_fld_get(
      SOC_SAND_IN  int  unit,
      SOC_SAND_IN  uint32   age_fld,
      SOC_SAND_OUT uint32   *age
    )
{
  uint32
    age_lcl,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *soc_pb_regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_AGE_FLD_SET);

  SOC_SAND_CHECK_NULL_INPUT(age);

  soc_pb_regs = soc_pb_pp_regs();

  /*
   *	Get the resolution (high / low)
   */
  SOC_PB_PP_FLD_GET(soc_pb_regs->ihp.large_em_age_aging_resolution_reg.large_em_age_aging_resolution, fld_val, 10, exit);

  if (fld_val == 0)
  {
    /*
     *	Low resolution: Age mapped to (2,1,0,0)
     */
    if (age_fld != 0)
    {
      age_lcl = (age_fld % 4) - 1;
    }
    else
    {
      age_lcl = 0;
    }
  }
  else
  {
    /*
     *	High resolution: Age mapped to (6,5,4,2,6,1,0,0)
     */
    switch(age_fld)
    {
    case 0:
    case 1:
      age_lcl = 0;
      break;

    case 2:
      age_lcl = 1;
      break;

    case 3:
    case 7:
      age_lcl = 6;
      break;

    case 4:
      age_lcl = 2;
      break;

    case 5:
      age_lcl = 4;
      break;

    case 6:
      age_lcl = 5;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_AGE_OUT_OF_RANGE_ERR, 20, exit);
    }
  }

  *age = age_lcl;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_age_fld_set()", age_fld, 0);
}

STATIC uint32
  soc_pb_pp_lem_access_actual_stamp_get(
      SOC_SAND_IN int  unit
    )
{
  return Soc_pb_pp_lem_actual_stamp[unit];
}

STATIC uint32
  soc_pb_pp_lem_access_next_stamp_get(
      SOC_SAND_IN int  unit
    )
{
  Soc_pb_pp_lem_actual_stamp[unit] ++;
  if (Soc_pb_pp_lem_actual_stamp[unit] == SOC_PB_PP_LEM_STAMP_MAX)
  {
    Soc_pb_pp_lem_actual_stamp[unit] = SOC_PB_PP_LEM_STAMP_MIN;
  }
  return Soc_pb_pp_lem_actual_stamp[unit];
}


uint32
  soc_pb_pp_lem_access_payload_parse(
      SOC_SAND_IN   int                   unit,
      SOC_SAND_IN   uint32                    payload_data[SOC_PB_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
      SOC_SAND_OUT  SOC_PB_PP_LEM_ACCESS_PAYLOAD   *payload
    )
{
  uint32
    tmp;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_PAYLOAD_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(payload);

  res = soc_sand_bitstream_get_any_field(
          payload_data,
          0,
          SOC_PB_PP_LEM_ACCESS_DEST_NOF_BITS,
          &(payload->dest)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_get_any_field(
          payload_data,
          SOC_PB_PP_LEM_ACCESS_DEST_NOF_BITS,
          SOC_PB_PP_LEM_ACCESS_ASD_NOF_BITS,
          &(payload->asd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tmp = 0;
  res = soc_sand_bitstream_get_any_field(
          payload_data,
          SOC_PB_PP_LEM_ACCESS_DEST_NOF_BITS + SOC_PB_PP_LEM_ACCESS_ASD_NOF_BITS,
          1,
          &tmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  payload->is_dynamic = (uint8)tmp;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_payload_build()", 0, 0);
}


uint32
  soc_pb_pp_lem_access_payload_build(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_PAYLOAD   *payload,
    SOC_SAND_OUT uint32                    payload_data[SOC_PB_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S]
  )
{
  uint32
    payload_ndx;
  uint32
    tmp;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_PAYLOAD_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(payload_data);

  for (payload_ndx = 0; payload_ndx < SOC_PB_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S; ++payload_ndx)
  {
    payload_data[payload_ndx] = 0;
  }

  res = soc_sand_bitstream_set_any_field(
          &(payload->dest),
          0,
          SOC_PB_PP_LEM_ACCESS_DEST_NOF_BITS,
          payload_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_set_any_field(
          &(payload->asd),
          SOC_PB_PP_LEM_ACCESS_DEST_NOF_BITS,
          SOC_PB_PP_LEM_ACCESS_ASD_NOF_BITS,
          payload_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tmp = payload->is_dynamic;
  res = soc_sand_bitstream_set_any_field(
          &tmp,
          SOC_PB_PP_LEM_ACCESS_ASD_NOF_BITS + SOC_PB_PP_LEM_ACCESS_DEST_NOF_BITS,
          1,
          payload_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_payload_build()", 0, 0);
}


uint32
  soc_pb_pp_lem_key_extend(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_KEY             *key,
     SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY             *extnd_key
  )
{
  uint32 
    bits_sum=0, param_ndx,
    new_param_indx=0;
  uint8 mask_inrif=0,mask_port=0;
  uint32 nof_vrfs;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* calc number bits already in use */
  for (param_ndx = 0; param_ndx < key->nof_params; ++param_ndx)
  {
    bits_sum += key->param[param_ndx].nof_bits;
  }
  /* next param to use */
  new_param_indx = key->nof_params;

  /* copy key */
  sal_memcpy(extnd_key,key, sizeof(SOC_PB_PP_LEM_ACCESS_KEY));

  /* fix according to type */
  switch (key->type)
  {
    /* for TRILL UC duplicate ENICK x 2 times + 11 lsb bits */
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_TRILL_UC:
    extnd_key->nof_params += 3;
    /* fill keys till last */
    for (; new_param_indx < (uint8)(extnd_key->nof_params-1); ++new_param_indx)
    {
      extnd_key->param[new_param_indx].nof_bits = key->param[0].nof_bits;
      extnd_key->param[new_param_indx].value[0] = key->param[0].value[0];
      bits_sum += extnd_key->param[new_param_indx].nof_bits;
    }
    /* last param set with rest of bits*/
    extnd_key->param[new_param_indx].nof_bits = (uint8)(SOC_PB_PP_LEM_ACCESS_KEY_LEN - bits_sum);
    extnd_key->param[new_param_indx].value[0] = SOC_SAND_GET_BITS_RANGE(key->param[0].value[0],extnd_key->param[new_param_indx].nof_bits-1,0);
  break;
  /* for IP host if no vrf in system duplicate 27 lsb of DIP */
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IP_HOST:
    /* depends on nof-vrfs, if nof-vrfs is zero, then key is DIP,DIP */
    res = soc_pb_pp_sw_db_ipv4_nof_vrfs_get(unit, &nof_vrfs);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (nof_vrfs > 1)
    {
      break;
    }
    /* not new param but overwrite last parameter (VRF with DIP) */
    --new_param_indx;
    bits_sum -= key->param[new_param_indx].nof_bits;

    extnd_key->param[new_param_indx].nof_bits = (uint8) (SOC_PB_PP_LEM_ACCESS_KEY_LEN - bits_sum);
    /* duplicate lsb of param 0 */
    extnd_key->param[new_param_indx].value[0] = SOC_SAND_GET_BITS_RANGE(key->param[0].value[0],extnd_key->param[new_param_indx].nof_bits-1,0);
  break;
  /* for extended lookup add new param with first param 25 lsb*/
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_EXTENDED:
    extnd_key->nof_params += 1;
    extnd_key->param[new_param_indx].nof_bits = (uint8) (SOC_PB_PP_LEM_ACCESS_KEY_LEN - bits_sum);
    extnd_key->param[new_param_indx].value[0] = SOC_SAND_GET_BITS_RANGE(key->param[0].value[0],extnd_key->param[new_param_indx].nof_bits-1,0);
    break;

  /* for ILM, depend on masking if port/rif masked then use bits from label instead */
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_ILM:
    /* label and exp */
    res = soc_pb_pp_sw_db_ilm_key_mask_inrif_get(unit, &mask_inrif);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    /* instead RIF set part of label */
    if (mask_inrif)
    {
      extnd_key->param[3].value[0] = SOC_SAND_GET_BITS_RANGE(key->param[0].value[0],extnd_key->param[3].nof_bits-1,0);
    }
    res = soc_pb_pp_sw_db_ilm_key_mask_port_get(unit, &mask_port);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    /* if port is masked then take label instead*/
    if (mask_port)
    {
      extnd_key->param[2].value[0] = key->param[0].value[0];
      extnd_key->param[2].nof_bits = key->param[0].nof_bits;
    }
  break;
  default:
  break;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_key_extend()", 0, 0);
}



uint32
  soc_pb_pp_lem_key_encoded_build(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_KEY             *key_in,
     SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY_ENCODED     *key_in_buffer
  )
{
  uint32
    current_param_lsb,
    res = SOC_SAND_OK,
    buffer_lcl[SOC_PB_PP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S];
  uint32
    param_ndx,
    buffer_ndx;
  SOC_PB_PP_LEM_ACCESS_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_KEY_ENCODED_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(key_in);
  SOC_SAND_CHECK_NULL_INPUT(key_in_buffer);

  res = soc_pb_pp_lem_key_extend(unit,key_in,&key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Initialization
   */
  for (buffer_ndx = 0; buffer_ndx < SOC_PB_PP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S; ++buffer_ndx)
  {
    buffer_lcl[buffer_ndx] = 0;
  }

  /*
   *	Copy of the parameters
   */
  current_param_lsb = 0;
  for (param_ndx = 0; param_ndx < key.nof_params; ++param_ndx)
  {
    if (((key.param[param_ndx].nof_bits == 0) && (key.prefix.value != SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_MC))  || (key.param[param_ndx].nof_bits > SOC_DPP_LEM_WIDTH_PETRAB))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEY_PARAM_SIZE_OUT_OF_RANGE_ERR, 10, exit);
    }
    res = soc_sand_bitstream_set_any_field(
            key.param[param_ndx].value,
            current_param_lsb,
            key.param[param_ndx].nof_bits,
            buffer_lcl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    current_param_lsb += key.param[param_ndx].nof_bits;
  }

  /*
   *	Copy of the prefix and verification of its length
   */
  if (current_param_lsb + key.prefix.nof_bits > SOC_DPP_LEM_WIDTH_PETRAB)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = soc_sand_bitstream_set_any_field(
          &(key.prefix.value),
          SOC_DPP_LEM_WIDTH_PETRAB - key.prefix.nof_bits,
          key.prefix.nof_bits,
          buffer_lcl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (buffer_ndx = 0; buffer_ndx < SOC_PB_PP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S; ++buffer_ndx)
  {
    key_in_buffer->buffer[buffer_ndx] = buffer_lcl[buffer_ndx];
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_key_encoded_build()", 0, 0);
}

uint32
  soc_pb_pp_lem_key_encoded_parse(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED     *key_in_buffer,
     SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY             *key
  )
{
  uint32
    curr_bit_lsb,
    key_type_4_msb = 0,
    res = SOC_SAND_OK;
  uint32
    param_ndx;
  uint8
    garbage_bool,
    trill_mc_param1_decider;
  uint8
    param1_in_bits_for_trill_mc;
  uint32 nof_vrfs;
  uint8 mask_inrif=0,mask_port=0;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_KEY_ENCODED_PARSE);

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(key_in_buffer);

  SOC_PB_PP_LEM_ACCESS_KEY_clear(key);

  /*
   *	Find the right type (encoded in the four msb of the buffer)
   */
  res = soc_sand_bitstream_get_any_field(
          key_in_buffer->buffer,
          SOC_DPP_LEM_WIDTH_PETRAB - SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IN_BITS_4,
          SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IN_BITS_4,
          &(key_type_4_msb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  if (SOC_SAND_GET_BIT(key_type_4_msb, 3) == 0x1)
  {
    key_type_4_msb = SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_1;
  }

  switch(key_type_4_msb)
  {
    /*
     *	Backbone MAC
     */
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0001:
    key->nof_params = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_BACKBONE_MAC;
    key->type = SOC_PB_PP_LEM_ACCESS_KEY_TYPE_BACKBONE_MAC;
    key->prefix.value = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_BACKBONE_MAC;
    key->prefix.nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_BACKBONE_MAC;
    key->param[0].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_BACKBONE_MAC;
    key->param[1].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_BACKBONE_MAC;
    key->param[2].nof_bits = 0;
    key->param[3].nof_bits = 0;
    key->param[4].nof_bits = 0;
    break;

    /*
     *	IPv4 Compatible Multicast
     */
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0010:
    key->nof_params = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IPV4_MC;
    key->type = SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IPV4_MC;
    key->prefix.value = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_IPV4_MC;
    key->prefix.nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV4_MC;
    key->param[0].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC;
    key->param[1].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC;
    key->param[2].nof_bits = 0;
    key->param[3].nof_bits = 0;
    key->param[4].nof_bits = 0;
    break;

    /*
     *	IPv4 Host
     */
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0101:

    res = soc_pb_pp_sw_db_ipv4_nof_vrfs_get(unit, &nof_vrfs);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    key->nof_params = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IP_HOST;
    key->type = SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IP_HOST;
    key->prefix.value = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_IP_HOST;
    key->prefix.nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IP_HOST;
    key->param[0].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IP_HOST;
    if (nof_vrfs > 1)
    {
      key->param[1].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IP_HOST;/* VRF */
    }
    else
    {
      key->param[1].nof_bits = 0; /* no second param, used to duplicate DIP */
      key->nof_params = 1;
    }
    key->param[2].nof_bits = 0;
    key->param[3].nof_bits = 0;
    key->param[4].nof_bits = 0;
    break;
    /*
     *	trill UC
     */
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0100:
    key->nof_params = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_UC;
    key->type = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_UC;
    key->prefix.value = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_UC;
    key->prefix.nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_UC;
    key->param[0].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_UC;
    key->param[1].nof_bits = 0;
    key->param[2].nof_bits = 0;
    key->param[3].nof_bits = 0;
    key->param[4].nof_bits = 0;
    break;

    /*
     *	trill MC
     */
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0000:
    soc_pb_pp_lem_trill_mc_key_build_get(unit,&garbage_bool,&garbage_bool,&trill_mc_param1_decider);
    if (trill_mc_param1_decider == TRUE)
    {
      param1_in_bits_for_trill_mc = 0;
    }
    else
    {
      param1_in_bits_for_trill_mc = SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_TRILL_MC;
    }
    key->nof_params = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_MC;
    key->type = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_MC;
    key->prefix.value = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_MC;
    key->prefix.nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_MC;
    key->param[0].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_MC;
    key->param[1].nof_bits = param1_in_bits_for_trill_mc;
    key->param[2].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_TRILL_MC;
    key->param[3].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_TRILL_MC;
    key->param[4].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM4_IN_BITS_FOR_TRILL_MC;
    break;
    
    /*
     *	SA AUTH
     */
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0111:
    key->nof_params = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_SA_AUTH;
    key->type = SOC_PB_PP_LEM_ACCESS_KEY_TYPE_SA_AUTH;
    key->prefix.value = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_SA_AUTH;
    key->prefix.nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_SA_AUTH;
    key->param[0].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_SA_AUTH;
    key->param[1].nof_bits = 0;
    key->param[2].nof_bits = 0;
    key->param[3].nof_bits = 0;
    key->param[4].nof_bits = 0;
    break;

   /*
    *	ILM
    */
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0110:

    res = soc_pb_pp_sw_db_ilm_key_mask_inrif_get(unit, &mask_inrif);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    res = soc_pb_pp_sw_db_ilm_key_mask_port_get(unit, &mask_port);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    
    key->nof_params = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_ILM;
    key->type = SOC_PB_PP_LEM_ACCESS_KEY_TYPE_ILM;
    key->prefix.value = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_ILM;
    key->prefix.nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_ILM;
    key->param[0].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM;
    key->param[1].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM;
    /* if port is masked then there is 20 bits to skip */
    if (mask_port)
    {
      key->param[2].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM;
    }
    else
    {
      key->param[2].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM;
    }
    /* this can be 12 bits of RIF or of label */
    key->param[3].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM;
    key->param[4].nof_bits = 0;
    break;
    /*
     *	MAC
     */
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_1:
    key->nof_params = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_MAC;
    key->type = SOC_PB_PP_LEM_ACCESS_KEY_TYPE_MAC;
    key->prefix.value = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_MAC;
    key->prefix.nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_MAC;
    key->param[0].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC;
    key->param[1].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_MAC;
    key->param[2].nof_bits = 0;
    key->param[3].nof_bits = 0;
    key->param[4].nof_bits = 0;
    break;

  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0011:
    key->nof_params = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_EXTENDED;
    key->type = SOC_PB_PP_LEM_ACCESS_KEY_TYPE_EXTENDED;
    key->prefix.value = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_EXTENDED;
    key->prefix.nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_EXTENDED;
    key->param[0].nof_bits = SOC_PB_PP_L2_LIF_KEY_TYPE_EXTENDED_PARAM0_NOF_BITS;
    break;
  

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR, 20, exit);
  }

  /*
   *	Extract the fields from the buffer
   */
  curr_bit_lsb = 0;
  for (param_ndx = 0; param_ndx < key->nof_params; ++param_ndx)
  {
    if (
        ((key->param[param_ndx].nof_bits == 0) && key->prefix.value != SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_MC)
        || (key->param[param_ndx].nof_bits > SOC_DPP_LEM_WIDTH_PETRAB)
        || (curr_bit_lsb > SOC_DPP_LEM_WIDTH_PETRAB)
       )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEY_PARAM_SIZE_OUT_OF_RANGE_ERR, 30, exit);
    }
    res = soc_sand_bitstream_get_any_field(
            key_in_buffer->buffer,
            curr_bit_lsb,
            key->param[param_ndx].nof_bits,
            key->param[param_ndx].value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    curr_bit_lsb += key->param[param_ndx].nof_bits;
  }

  /*
   *	Last verification of the key format
   */
  for (param_ndx = key->nof_params; param_ndx < SOC_PB_PP_LEM_KEY_MAX_NOF_PARAMS; ++param_ndx)
  {
    if (key->param[param_ndx].nof_bits != 0)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEY_PARAM_SIZE_OUT_OF_RANGE_ERR, 50, exit);
    }
  }

  /* fix content */
  if (key_type_4_msb == SOC_PB_PP_LEM_ACCESS_KEY_TYPE_PREFIX_0110)
  {
    if (mask_port)
    {
      key->param[2].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM;
      key->param[2].value[0] = 0;
    }
    if (mask_inrif)
    {
      key->param[3].nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM;
      key->param[3].value[0] = 0;
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_key_encoded_parse()", 0, 0);
}

/* $Id: pb_pp_lem_access.c,v 1.15 Broadcom SDK $
 *	Write a request into the Request FIFO
 */
STATIC uint32
  soc_pb_pp_lem_request_send(
      SOC_SAND_IN  int                        unit,
      SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_OUTPUT         *request_all,
      SOC_SAND_OUT uint8                       *is_poll_success
    )
{
  uint32
    fld_val,
    reg_val,
    reg_val_key = 0,
    reg_val_key2 = 0,
    tmp,
    reg_vals[3],
    res = SOC_SAND_OK;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    is_key,
    db_profile,
    poll_success;
  SOC_PB_PP_REGS
    *soc_pb_regs;
  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer;
#ifdef USING_CHIP_SIM
  uint32
    key[SOC_PB_PP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S],
    data[SOC_PB_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S];
  uint8
    success;
#endif
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_REQUEST_SEND);

  SOC_SAND_CHECK_NULL_INPUT(request_all);

  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);

  soc_pb_regs = soc_pb_pp_regs();

  /*
   *	Polling to verify no CPU request is waiting
   */
  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x0;
  poll_info.busy_wait_nof_iters = SOC_PB_PP_LEM_ACCESS_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PB_PP_LEM_ACCESS_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PB_PP_LEM_ACCESS_TIMER_DELAY_MSEC;

  res = soc_petra_status_fld_poll_unsafe(
          unit,
          &(soc_pb_regs->ihp.large_em_cpu_request_trigger_reg.large_em_cpu_request_trigger),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (poll_success == FALSE)
  {
    *is_poll_success = poll_success;
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }
  else
  {
    *is_poll_success = poll_success;
  }

  /*
   *	Insert the request details
   */
  /*SOC_PB_PP_REG_GET(soc_pb_regs->ihp.large_em_cpu_request_request_reg_3, reg_val, 20, exit);*/
  reg_val = 0;

  /*
   *	Request origin: CPU or OLP
   */
  switch(request_all->req_origin)
  {
  case SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_CPU:
    fld_val = SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_CPU_FLD_VAL;
    break;

  case SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_OLP:
    fld_val = SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_OLP_FLD_VAL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_OUT_OF_RANGE_ERR, 25, exit);
  }
  SOC_PB_PP_FLD_TO_REG(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_3.large_em_req_qualifier, fld_val, reg_val,  30, exit);

  /*
   *	Entry self-learnt
   */
  fld_val = request_all->is_learned_first_by_me;
  SOC_PB_PP_FLD_TO_REG(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_3.large_em_req_self, fld_val, reg_val, 35, exit);

  /*
   *	No need to set the age (overridden)
   */
  fld_val = 7;
  SOC_PB_PP_FLD_TO_REG(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_3.large_em_req_age_payload, fld_val, reg_val, 40, exit);
  fld_val = request_all->payload.is_dynamic;
  SOC_PB_PP_FLD_TO_REG(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_3.large_em_req_payload_is_dynamic, fld_val, reg_val, 45, exit);

  /*
   *	ASD
   */
  fld_val = request_all->payload.asd >> SOC_PB_PP_LEM_ACCESS_ASD_FIRST_PART_NOF_BITS;
  SOC_PB_PP_FLD_TO_REG(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_3.large_em_req_payload_asd, fld_val, reg_val, 50, exit);
  SOC_PB_PP_REG_SET(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_3, reg_val, 55, exit);

  reg_val = 0;
  fld_val = request_all->payload.asd % (1 << SOC_PB_PP_LEM_ACCESS_ASD_FIRST_PART_NOF_BITS);
  SOC_PB_PP_FLD_TO_REG(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_2.large_em_req_payload_asd, fld_val, reg_val, 65, exit);

  /*
   *	Destination
   */
  SOC_PB_PP_FLD_TO_REG(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_2.large_em_req_payload_destination, request_all->payload.dest, reg_val, 70, exit);

  /*
   *	Stamp
   */
  SOC_PB_PP_FLD_TO_REG(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_2.large_em_req_stamp, request_all->request.stamp, reg_val, 75, exit);

  /*
   *	Command type
   */
  switch(request_all->request.command)
  {
  case SOC_PB_PP_LEM_ACCESS_CMD_DELETE:
    fld_val = SOC_PB_PP_LEM_ACCESS_CMD_DELETE_FLD_VAL;
    break;
  case SOC_PB_PP_LEM_ACCESS_CMD_INSERT:
    fld_val = SOC_PB_PP_LEM_ACCESS_CMD_INSERT_FLD_VAL;
    break;
  case SOC_PB_PP_LEM_ACCESS_CMD_REFRESH:
    fld_val = SOC_PB_PP_LEM_ACCESS_CMD_REFRESH_FLD_VAL;
    break;
  case SOC_PB_PP_LEM_ACCESS_CMD_LEARN:
    fld_val = SOC_PB_PP_LEM_ACCESS_CMD_LEARN_FLD_VAL;
    break;
  case SOC_PB_PP_LEM_ACCESS_CMD_DEFRAG:
    fld_val = SOC_PB_PP_LEM_ACCESS_CMD_DEFRAG_FLD_VAL;
    break;
  case SOC_PB_PP_LEM_ACCESS_CMD_ACK:
    fld_val = SOC_PB_PP_LEM_ACCESS_CMD_ACK_FLD_VAL;
    break;
  case SOC_PB_PP_LEM_ACCESS_CMD_TRANSPLANT:
    fld_val = SOC_PB_PP_LEM_ACCESS_CMD_TRANSPLANT_FLD_VAL;
    break;
  case SOC_PB_PP_LEM_ACCESS_CMD_ERROR:
    fld_val = SOC_PB_PP_LEM_ACCESS_CMD_ERROR_FLD_VAL;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR, 80, exit);
  }
  SOC_PB_PP_FLD_TO_REG(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_2.large_em_req_command, fld_val, reg_val, 85, exit);

  fld_val = 0x0;
  SOC_PB_PP_FLD_TO_REG(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_2.large_em_req_part_of_lag, fld_val, reg_val, 90, exit);

  /*
   *	Key insertion (or (System-VSI, MAC))
   */

  res = soc_pb_pp_lem_key_encoded_build(
          unit,
          &(request_all->request.key),
          &key_in_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

  /*
   *	No insertion of System-VSI mode
   */

  SOC_PB_PP_CLEAR(reg_vals,uint32,2);

  /*if (request_all->request.key.type == SOC_PB_PP_LEM_ACCESS_KEY_TYPE_MAC)*/
  {
    is_key = TRUE;
    tmp = is_key;
    soc_sand_bitstream_set_any_field(&tmp,0,1,reg_vals);
  }

  soc_sand_bitstream_set_any_field(key_in_buffer.buffer,2,62,reg_vals);
  /* msb has to be 1, to match DB-ID */
  tmp = 1;
  /*soc_sand_bitstream_set_any_field(&tmp,63,1,reg_vals);*/

  reg_val_key = reg_vals[0];
  reg_val_key2 = reg_vals[1];
/*
  fld_val_param0 = SOC_SAND_GET_BITS_RANGE(key_in_buffer.buffer[0], 30, 0);
  fld_val_param1 = SOC_SAND_GET_BITS_RANGE(key_in_buffer.buffer[0], 31, 31) +
    (SOC_SAND_GET_BITS_RANGE(key_in_buffer.buffer[1], 30, 0) << 1);*/
  db_profile = 1;/*SOC_SAND_GET_BIT(key_in_buffer.buffer[1], 31);*/
/*
  reg_val_key = is_key + (fld_val_param0 << 1);
  reg_val_key2 = fld_val_param1;*/

  if (request_all->request.key.type == SOC_PB_PP_LEM_ACCESS_KEY_TYPE_MAC)
  {
    fld_val = db_profile;
    SOC_PB_PP_FLD_TO_REG(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_2.large_em_req_mff_key_db_profile, fld_val, reg_val, 100, exit);
  }
  SOC_PB_PP_REG_SET(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_2, reg_val, 105, exit);

  SOC_PB_PP_REG_SET(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_1, reg_val_key2, 110, exit);

  /*
   *	Writing in this register is a trigger to send the request
   */
  SOC_PB_PP_REG_SET(soc_pb_regs->ihp.large_em_cpu_request_request_reg.reg_0, reg_val_key, 115, exit);

#ifdef USING_CHIP_SIM
  sal_memset(key, 0x0, sizeof(key)) ;
  sal_memset(data, 0x0, sizeof(data)) ;

  key[0] = key_in_buffer.buffer[0];
  key[1] = key_in_buffer.buffer[1];
  if (request_all->request.command == SOC_PB_PP_LEM_ACCESS_CMD_DELETE)
  {
    /*
     *	Deletion command
     */
    res = chip_sim_exact_match_entry_remove_unsafe(
            unit,
            CHIP_SIM_LEM_ADDR_BASE,
            key,
            CHIP_SIM_LEM_KEY_SIZE(unit)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

    *is_poll_success = TRUE;
  }
  else
  {
    /*
     *	Insertion command
     */
    res = soc_pb_pp_lem_access_payload_build(
            unit,
            &(request_all->payload),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

    res = chip_sim_exact_match_entry_add_unsafe(
            unit,
            CHIP_SIM_LEM_ADDR_BASE,
            key,
            CHIP_SIM_LEM_KEY_SIZE(unit),
            data,
            CHIP_SIM_LEM_DATA_SIZE(unit),
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

    *is_poll_success = success;
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_request_send()", 0, 0);
}

/*
 *  Parse access output
 */
uint32
  soc_pb_pp_lem_access_parse(
      SOC_SAND_IN  int                        unit,
      SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_BUFFER          *buffer,
      SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_OUTPUT          *request_all,
      SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_ACK_STATUS      *ack_status
    )
{
  uint32
    fld_val,
    reg_val2,
    reg_val,
    reg_val_key[2],
    res = SOC_SAND_OK;
  uint8
    is_key,
    db_profile;
  SOC_PB_PP_REGS
    *soc_pb_regs;
  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_PARSE);

  SOC_SAND_CHECK_NULL_INPUT(buffer);
  SOC_SAND_CHECK_NULL_INPUT(request_all);
  SOC_SAND_CHECK_NULL_INPUT(ack_status);

  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);

  soc_pb_regs = soc_pb_pp_regs();

  reg_val = buffer->data[3];
  reg_val2 = buffer->data[2];
  reg_val_key[1] = buffer->data[1];
  reg_val_key[0] = buffer->data[0];

  /*
   *	Stamp
   */
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_2.large_em_req_stamp, fld_val, reg_val2, 30, exit);
  request_all->stamp = fld_val;

  /*
   *	Request origin: CPU or OLP
   */
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_3.large_em_req_qualifier, fld_val, reg_val,  30, exit);
  switch(fld_val)
  {
  case SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_CPU_FLD_VAL:
    request_all->req_origin = SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_CPU;
    break;

  case SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_OLP_FLD_VAL:
    request_all->req_origin = SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_OLP;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_OUT_OF_RANGE_ERR, 25, exit);
  }

  /*
   *	Entry self-learnt
   */
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_3.large_em_req_self, fld_val, reg_val, 35, exit);
  request_all->is_learned_first_by_me = SOC_SAND_NUM2BOOL(fld_val);

  /*
   *	Get the ACK status
   */
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_3.large_em_req_success, fld_val, reg_val, 37, exit);
  if (fld_val == 0x1)
  {
    ack_status->is_success = TRUE;
    ack_status->reason = SOC_PB_PP_LEM_ACCESS_NOF_FAIL_REASONS;
  }
  else
  {
    ack_status->is_success = FALSE;
    SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_3.large_em_req_reason, fld_val, reg_val, 38, exit);
    switch(fld_val)
    {
    case SOC_PB_PP_LEM_ACCESS_FAIL_REASON_UNKNOWN_VSI_FLD_VAL:
      ack_status->reason = SOC_PB_PP_LEM_ACCESS_FAIL_REASON_FID_UNKNOWN;
      break;

    case SOC_PB_PP_LEM_ACCESS_FAIL_REASON_FID_LIMIT_FLD_VAL:
      ack_status->reason = SOC_PB_PP_LEM_ACCESS_FAIL_REASON_FID_LIMIT;
      break;

    case SOC_PB_PP_LEM_ACCESS_FAIL_REASON_EMC_PROBLEM_FLD_VAL:
      ack_status->reason = SOC_PB_PP_LEM_ACCESS_FAIL_REASON_EMC_PROBLEM;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR, 43, exit);
    }
  }

  /*
   *	Computation of the good aging payload according to the system configuration
   */

  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_3.large_em_req_age_payload, fld_val, reg_val, 45, exit);
  res = soc_pb_pp_lem_access_age_fld_get(
          unit,
          fld_val,
          &(request_all->payload.age)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_3.large_em_req_payload_is_dynamic, fld_val, reg_val, 46, exit);
  request_all->payload.is_dynamic = SOC_SAND_NUM2BOOL(fld_val);

  /*
   *	ASD
   */
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_3.large_em_req_payload_asd, fld_val, reg_val, 50, exit);
  request_all->payload.asd = fld_val << SOC_PB_PP_LEM_ACCESS_ASD_FIRST_PART_NOF_BITS;

  reg_val = reg_val2;
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_2.large_em_req_payload_asd, fld_val, reg_val, 65, exit);
  request_all->payload.asd +=  fld_val;

  /*
   *	Destination
   */
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_2.large_em_req_payload_destination, request_all->payload.dest, reg_val, 70, exit);

  /*
   * Part of LAG
   */
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_2.large_em_req_part_of_lag, fld_val, reg_val, 80, exit);
  request_all->is_part_of_lag = SOC_SAND_NUM2BOOL(fld_val);

  /*
   *	Command type
   */
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_2.large_em_req_command, fld_val, reg_val, 85, exit);
  switch(fld_val)
  {
  case SOC_PB_PP_LEM_ACCESS_CMD_DELETE_FLD_VAL:
    request_all->request.command = SOC_PB_PP_LEM_ACCESS_CMD_DELETE;
    break;
  case SOC_PB_PP_LEM_ACCESS_CMD_INSERT_FLD_VAL:
    request_all->request.command = SOC_PB_PP_LEM_ACCESS_CMD_INSERT;
    break;
  case SOC_PB_PP_LEM_ACCESS_CMD_REFRESH_FLD_VAL:
    request_all->request.command = SOC_PB_PP_LEM_ACCESS_CMD_REFRESH;
    break;
  case SOC_PB_PP_LEM_ACCESS_CMD_LEARN_FLD_VAL:
    request_all->request.command = SOC_PB_PP_LEM_ACCESS_CMD_LEARN;
    break;
  case SOC_PB_PP_LEM_ACCESS_CMD_DEFRAG_FLD_VAL:
    request_all->request.command = SOC_PB_PP_LEM_ACCESS_CMD_DEFRAG;
    break;
  case SOC_PB_PP_LEM_ACCESS_CMD_ACK_FLD_VAL:
    request_all->request.command = SOC_PB_PP_LEM_ACCESS_CMD_ACK;
    break;
  case SOC_PB_PP_LEM_ACCESS_CMD_TRANSPLANT_FLD_VAL:
    request_all->request.command = SOC_PB_PP_LEM_ACCESS_CMD_TRANSPLANT;
    break;
  case SOC_PB_PP_LEM_ACCESS_CMD_ERROR_FLD_VAL:
    request_all->request.command = SOC_PB_PP_LEM_ACCESS_CMD_ERROR;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR, 80, exit);
  }

  /*
   *	Key insertion (or (System-VSI, MAC))
   */
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_2.large_em_req_mff_key_db_profile, fld_val, reg_val, 100, exit);
  db_profile = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_0.large_em_req_mff_is_key, fld_val, reg_val_key[0], 100, exit);
  is_key = SOC_SAND_NUM2BOOL(fld_val);


  if (is_key == FALSE)
  {
    /*
     *  Copy bits 64:1 from the register to key_in_buffer (System-VSI & MAC)
     */
    key_in_buffer.buffer[0] = SOC_SAND_GET_BITS_RANGE(reg_val_key[0], 31, 1) + (SOC_SAND_GET_BIT(reg_val_key[1], 0) << 31);
    key_in_buffer.buffer[1] = SOC_SAND_GET_BITS_RANGE(reg_val_key[1], 31, 1) /*+ (db_profile << 31)*/;
  }
  else
  {
    /*
     *  Copy bits 64:2 from the register to key_in_buffer (LEM key)
     */
    res = soc_sand_bitstream_get_any_field(reg_val_key,2,62,key_in_buffer.buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 88, exit);
    key_in_buffer.buffer[1] |= (db_profile << 30);
  }
  
  res = soc_pb_pp_lem_key_encoded_parse(
          unit,
          &key_in_buffer,
          &(request_all->request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

#ifdef USING_CHIP_SIM
  request_all->request.command = SOC_PB_PP_LEM_ACCESS_CMD_ACK;
  ack_status->is_success = TRUE;
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_parse()", 0, 0);
}

/*
 *	Read a request answer
 */
STATIC uint32
  soc_pb_pp_lem_request_answer_receive(
      SOC_SAND_IN  int                        unit,
      SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_OUTPUT          *request_all,
      SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_ACK_STATUS      *ack_status
    )
{
  uint32
    fld_val,
    reg_interrupt,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_PP_REGS
    *soc_pb_regs;
  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer;
  SOC_PB_PP_LEM_ACCESS_BUFFER
    buffer;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_REQUEST_ANSWER_RECEIVE);

  SOC_SAND_CHECK_NULL_INPUT(request_all);
  SOC_SAND_CHECK_NULL_INPUT(ack_status);

  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);
  SOC_PB_PP_LEM_ACCESS_BUFFER_clear(&buffer);

  regs = soc_petra_regs();
  soc_pb_regs = soc_pb_pp_regs();

  /*
   *  Read registers into access buffer
   */
  SOC_PB_PP_REG_GET(soc_pb_regs->ihp.large_em_reply_reg.reg_3, buffer.data[3], 20, exit);
  SOC_PB_PP_REG_GET(soc_pb_regs->ihp.large_em_reply_reg.reg_2, buffer.data[2], 20, exit);
  SOC_PB_PP_REG_GET(soc_pb_regs->ihp.large_em_reply_reg.reg_1, buffer.data[1], 20, exit);
  SOC_PB_PP_REG_GET(soc_pb_regs->ihp.large_em_reply_reg.reg_0, buffer.data[0], 20, exit);

  /*
   *  Parse access buffer contents
   */
  res = soc_pb_pp_lem_access_parse(
          unit,
          &buffer,
          request_all,
          ack_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Read the stamp to check it is the right request
   */
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_reply_reg.reg_2.large_em_req_stamp, fld_val, buffer.data[2], 20, exit);

  if (fld_val != soc_pb_pp_lem_access_actual_stamp_get(unit))
  {
    ack_status->is_success = FALSE;
    ack_status->reason = SOC_PB_PP_LEM_ACCESS_FAIL_REASON_WRONG_STAMP;
  }

  /*
   *	Update the ACK status
   */
  if (ack_status->reason == SOC_PB_PP_LEM_ACCESS_FAIL_REASON_EMC_PROBLEM)
  {
    /*
     *	Read the interrupts to know the error reason - an OLP interference is possible
     */
    SOC_PB_REG_GET(regs->ihp.mact_interrupt_reg, reg_interrupt, 30, exit);

    SOC_PB_FLD_FROM_REG(regs->ihp.mact_interrupt_reg.mact_error_cam_table_full, fld_val, reg_interrupt, 40, exit);
    if (fld_val != 0x0)
    {
      ack_status->reason = SOC_PB_PP_LEM_ACCESS_FAIL_REASON_CAM_FULL;
    }

    SOC_PB_FLD_FROM_REG(regs->ihp.mact_interrupt_reg.mact_error_delete_unknown_key, fld_val, reg_interrupt, 41, exit);
    if (fld_val != 0x0)
    {
      ack_status->reason = SOC_PB_PP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN;
    }

    SOC_PB_FLD_FROM_REG(regs->ihp.mact_interrupt_reg.mact_error_reached_max_entry_limit, fld_val, reg_interrupt, 42, exit);
    if (fld_val != 0x0)
    {
      ack_status->reason = SOC_PB_PP_LEM_ACCESS_FAIL_REASON_MACT_FULL;
    }

    SOC_PB_FLD_FROM_REG(regs->ihp.mact_interrupt_reg.mact_error_learn_request_over_static, fld_val, reg_interrupt, 43, exit);
    if (fld_val != 0x0)
    {
      ack_status->reason = SOC_PB_PP_LEM_ACCESS_FAIL_REASON_LEARN_STATIC;
    }

    SOC_PB_FLD_FROM_REG(regs->ihp.mact_interrupt_reg.mact_error_change_request_over_static, fld_val, reg_interrupt, 44, exit);
    if (fld_val != 0x0)
    {
      ack_status->reason = SOC_PB_PP_LEM_ACCESS_FAIL_REASON_CHANGE_STATIC;
    }
  }

exit:
#ifdef USING_CHIP_SIM
  ack_status->is_success = TRUE;
#endif
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_request_answer_receive()", 0, 0);
}

/*********************************************************************
 *     Add an entry to the Exact match table.
 *********************************************************************/
uint32
  soc_pb_pp_lem_access_entry_add_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_REQUEST         *request,
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_PAYLOAD         *payload,
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_ACK_STATUS      *ack_status
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_LEM_ACCESS_OUTPUT
    request_hw,
    request_hw_rcv;
  uint8
    poll_success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(request);
  SOC_SAND_CHECK_NULL_INPUT(payload);
  SOC_SAND_CHECK_NULL_INPUT(ack_status);

  SOC_PB_PP_LEM_ACCESS_OUTPUT_clear(&request_hw);
  SOC_PB_PP_LEM_ACCESS_OUTPUT_clear(&request_hw_rcv);

  /*
   *	Verify the inputs
   */
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LEM_ACCESS_REQUEST, request, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LEM_ACCESS_PAYLOAD, payload, 20, exit);

  /*
   *	Build the HW request
   */
  SOC_PETRA_COPY(&(request_hw.request), request, SOC_PB_PP_LEM_ACCESS_REQUEST, 1);
  SOC_PETRA_COPY(&(request_hw.payload), payload, SOC_PB_PP_LEM_ACCESS_PAYLOAD, 1);
  request_hw.is_learned_first_by_me = TRUE;
  request_hw.req_origin = SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_CPU;
  request_hw.request.stamp = soc_pb_pp_lem_access_next_stamp_get(unit);

  if ((request_hw.request.command != SOC_PB_PP_LEM_ACCESS_CMD_INSERT)
    &&(request_hw.request.command != SOC_PB_PP_LEM_ACCESS_CMD_LEARN)
    &&(request_hw.request.command != SOC_PB_PP_LEM_ACCESS_CMD_REFRESH)
    &&(request_hw.request.command != SOC_PB_PP_LEM_ACCESS_CMD_TRANSPLANT))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR, 25, exit);
  }

  /*
   *	Send the request
   */
  res = soc_pb_pp_lem_request_send(
          unit,
          &request_hw,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   *	Case of an unsent request
   */
  if (poll_success == FALSE)
  {
    ack_status->is_success = FALSE;
    ack_status->reason = SOC_PB_PP_LEM_ACCESS_FAIL_REASON_REQUEST_NOT_SENT;
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }

  /*
   *	Receive the answer
   */
  res = soc_pb_pp_lem_request_answer_receive(
          unit,
          &request_hw_rcv,
          ack_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (request_hw_rcv.request.command != SOC_PB_PP_LEM_ACCESS_CMD_ACK)
  {
    ack_status->is_success = FALSE;
    ack_status->reason = SOC_PB_PP_LEM_ACCESS_FAIL_REASON_UNKNOWN;
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_entry_add_unsafe()", 0, 0);
}

/*********************************************************************
 *     Remove an entry to the Exact match table.
 *********************************************************************/
uint32
  soc_pb_pp_lem_access_entry_remove_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_REQUEST         *request,
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_ACK_STATUS      *ack_status
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_LEM_ACCESS_OUTPUT
    request_hw,
    request_hw_rcv;
  uint8
    poll_success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_ENTRY_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(request);
  SOC_SAND_CHECK_NULL_INPUT(ack_status);

  SOC_PB_PP_LEM_ACCESS_OUTPUT_clear(&request_hw);
  SOC_PB_PP_LEM_ACCESS_OUTPUT_clear(&request_hw_rcv);

  /*
   *	Verify the inputs
   */
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LEM_ACCESS_REQUEST, request, 10, exit);

  /*
   *	Build the Hardware request
   */
  SOC_PETRA_COPY(&(request_hw.request), request, SOC_PB_PP_LEM_ACCESS_REQUEST, 1);
  request_hw.req_origin = SOC_PB_PP_LEM_ACCCESS_REQ_ORIGIN_CPU;
  request_hw.request.stamp = soc_pb_pp_lem_access_next_stamp_get(unit);

  if (request_hw.request.command != SOC_PB_PP_LEM_ACCESS_CMD_DELETE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR, 25, exit);
  }

  /*
   *	Send the request
   */
  res = soc_pb_pp_lem_request_send(
          unit,
          &request_hw,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   *	Case of an unsent request
   */
  if (poll_success == FALSE)
  {
    ack_status->is_success = FALSE;
    ack_status->reason = SOC_PB_PP_LEM_ACCESS_FAIL_REASON_REQUEST_NOT_SENT;
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }

  /*
   *	Receive the answer
   */
  res = soc_pb_pp_lem_request_answer_receive(
          unit,
          &request_hw_rcv,
          ack_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (request_hw_rcv.request.command != SOC_PB_PP_LEM_ACCESS_CMD_ACK)
  {
    ack_status->is_success = FALSE;
    ack_status->reason = SOC_PB_PP_LEM_ACCESS_FAIL_REASON_UNKNOWN;
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_mact_entry_remove_unsafe()", 0, 0);
}

/*********************************************************************
 *     Get an entry in the Exact match table according to its key
 *********************************************************************/
uint32
  soc_pb_pp_lem_access_entry_by_key_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_KEY             *key,
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_PAYLOAD         *payload,
    SOC_SAND_OUT uint8                        *is_found
  )
{
  uint32
    fld_val,
    fld_val2,
    reg_val1,
    reg_val2,
    res = SOC_SAND_OK;
  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer;
  SOC_PB_PP_REGS
    *soc_pb_regs;
  uint32
    fld_ndx;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success;
#ifdef USING_CHIP_SIM
  uint32
    key_in[SOC_PB_PP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S],
    data_out[SOC_PB_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S];
  uint8
    success;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_ENTRY_BY_KEY_GET_UNSAFE);

  soc_pb_regs = soc_pb_pp_regs();

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(payload);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);

  /*
   *	Verify the inputs
   */
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LEM_ACCESS_KEY, key, 10, exit);

  /*
   *	Write the key
   */
  res = soc_pb_pp_lem_key_encoded_build(
          unit,
          key,
          &key_in_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (fld_ndx = 0; fld_ndx < SOC_PB_PP_IHP_LARGE_EM_DIAGNOSTICS_KEY_REG_MULT_NOF_REGS; ++fld_ndx)
  {
    SOC_PB_PP_FLD_SET(soc_pb_regs->ihp.large_em_diagnostics_key_reg[fld_ndx].large_em_diagnostics_key, key_in_buffer.buffer[fld_ndx], 20 + fld_ndx, exit);
  }

  /*
   *	Set the trigger
   */
  fld_val = 0x1;
  SOC_PB_PP_FLD_SET(soc_pb_regs->ihp.large_em_diagnostics_reg.large_em_diagnostics_lookup, fld_val, 30, exit);

  /*
   *	Poll on the trigger bit before getting the result
   */
  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x0;
  poll_info.busy_wait_nof_iters = SOC_PB_PP_LEM_ACCESS_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PB_PP_LEM_ACCESS_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PB_PP_LEM_ACCESS_TIMER_DELAY_MSEC;

  res = soc_petra_status_fld_poll_unsafe(
          unit,
          &(soc_pb_regs->ihp.large_em_diagnostics_reg.large_em_diagnostics_lookup),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_LOOKUP_POLL_TIMEOUT_ERR, 50, exit);
  }

  /*
   *	Get the lookup result
   */
  SOC_PB_PP_REG_GET(soc_pb_regs->ihp.large_em_diagnostics_lookup_result_reg_0, reg_val1, 60, exit);
  SOC_PB_PP_REG_GET(soc_pb_regs->ihp.large_em_diagnostics_lookup_result_reg_1, reg_val2, 70, exit);

  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_diagnostics_lookup_result_reg_0.large_em_entry_found, fld_val, reg_val1, 80, exit);
  *is_found = SOC_SAND_NUM2BOOL(fld_val);
#ifndef USING_CHIP_SIM
  if (!*is_found)
  {
    goto exit;
  }
#endif

  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_diagnostics_lookup_result_reg_0.large_em_entry_payload_dest, fld_val, reg_val1, 90, exit);
  payload->dest = fld_val;

  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_diagnostics_lookup_result_reg_0.large_em_entry_payload_asd, fld_val, reg_val1, 100, exit);
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_diagnostics_lookup_result_reg_1.large_em_entry_payload_asd, fld_val2, reg_val2, 110, exit);
  payload->asd = fld_val + (fld_val2 << (soc_pb_regs->ihp.large_em_diagnostics_lookup_result_reg_0.large_em_entry_payload_asd.msb - soc_pb_regs->ihp.large_em_diagnostics_lookup_result_reg_0.large_em_entry_payload_asd.lsb + 1));

  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_diagnostics_lookup_result_reg_1.large_em_entry_payload_is_dyn, fld_val, reg_val2, 115, exit);
  payload->is_dynamic = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_diagnostics_lookup_result_reg_1.large_em_entry_age_stat, fld_val, reg_val2, 120, exit);
  res = soc_pb_pp_lem_access_age_fld_get(
          unit,
          fld_val,
          &(payload->age)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

#ifdef USING_CHIP_SIM
  sal_memset(key_in, 0x0, sizeof(key_in)) ;
  sal_memset(data_out, 0x0, sizeof(data_out)) ;

  key_in[0] = key_in_buffer.buffer[0];
  key_in[1] = key_in_buffer.buffer[1];

  res = chip_sim_exact_match_entry_get_unsafe(
          unit,
          CHIP_SIM_LEM_ADDR_BASE,
          key_in,
          CHIP_SIM_LEM_KEY_SIZE(unit),
          data_out,
          CHIP_SIM_LEM_DATA_SIZE(unit),
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

  *is_found = success;

  res = soc_pb_pp_lem_access_payload_parse(
          unit,
          data_out,
          payload
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_entry_by_key_get_unsafe()", 0, 0);
}

/*********************************************************************
 *     Get an entry in the Exact match table according to its index
 *********************************************************************/
uint32
  soc_pb_pp_lem_access_entry_by_index_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        entry_ndx,
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY             *key,
    SOC_SAND_OUT uint8                        *is_valid
  )
{
  uint32
    fld_val,
    reg_val1,
    reg_val2,
    res = SOC_SAND_OK;
  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer;
  SOC_PB_PP_REGS
    *soc_pb_regs;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success;
#ifdef USING_CHIP_SIM
  uint32
    key_out[SOC_PB_PP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S],
    data_out[SOC_PB_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S];
#else
  uint32
    fld_val2;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_ENTRY_BY_INDEX_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(is_valid);

  soc_pb_regs = soc_pb_pp_regs();

  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);

  /*
   *	Verify the inputs
   */
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_PP_FRWRD_MACT_TABLE_ENTRY_MAX, SOC_PB_PP_LEM_ENTRY_INDEX_OUT_OF_RANGE_ERR, 10, exit);

  /*
   *	Write the index
   */
  SOC_PB_PP_FLD_SET(soc_pb_regs->ihp.large_em_diagnostics_index_reg.large_em_diagnostics_index, entry_ndx, 20, exit);

  /*
   *	Set the trigger
   */
  fld_val = 0x1;
  SOC_PB_PP_FLD_SET(soc_pb_regs->ihp.large_em_diagnostics_reg.large_em_diagnostics_read, fld_val, 30, exit);

  /*
   *	Poll on the trigger bit before getting the result
   */
  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x0;
  poll_info.busy_wait_nof_iters = SOC_PB_PP_LEM_ACCESS_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PB_PP_LEM_ACCESS_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PB_PP_LEM_ACCESS_TIMER_DELAY_MSEC;

  res = soc_petra_status_fld_poll_unsafe(
          unit,
          &(soc_pb_regs->ihp.large_em_diagnostics_reg.large_em_diagnostics_read),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_LOOKUP_POLL_TIMEOUT_ERR, 50, exit);
  }

  /*
   *	Get the lookup result
   */
  SOC_PB_PP_REG_GET(soc_pb_regs->ihp.large_em_diagnostics_read_result_reg_0, reg_val1, 60, exit);
  SOC_PB_PP_REG_GET(soc_pb_regs->ihp.large_em_diagnostics_read_result_reg_1, reg_val2, 70, exit);

  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_diagnostics_read_result_reg_0.large_em_entry_valid, fld_val, reg_val1, 80, exit);
  *is_valid = SOC_SAND_NUM2BOOL(fld_val);

#ifdef USING_CHIP_SIM
  sal_memset(key_out, 0x0, sizeof(key_out)) ;
  sal_memset(data_out, 0x0, sizeof(data_out)) ;
 /* * COVERITY * 
   * overun was not found. */
 /* coverity[overrun-buffer-arg] */
  res = soc_sand_exact_match_entry_get_by_index_unsafe(
          unit,
          CHIP_SIM_LEM_ADDR_BASE,
          entry_ndx,
          key_out,
          CHIP_SIM_LEM_KEY_SIZE(unit),
          data_out,
          CHIP_SIM_LEM_DATA_SIZE(unit),
          is_valid
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

  key_in_buffer.buffer[0] = key_out[0];
  key_in_buffer.buffer[1] = key_out[1];
#else
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_diagnostics_read_result_reg_0.large_em_entry_key, fld_val, reg_val1, 90, exit);
  SOC_PB_PP_FLD_FROM_REG(soc_pb_regs->ihp.large_em_diagnostics_read_result_reg_1.large_em_entry_key, fld_val2, reg_val2, 100, exit);
  key_in_buffer.buffer[0] = fld_val + (SOC_SAND_GET_BIT(fld_val2, 0) << 31);
  key_in_buffer.buffer[1] = SOC_SAND_GET_BITS_RANGE(fld_val2, 31, 1);
#endif
  if (!*is_valid)
  {
    goto exit;
  }

  res = soc_pb_pp_lem_key_encoded_parse(
          unit,
          &key_in_buffer,
          key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_entry_by_index_get_unsafe()", 0, 0);
}

/*********************************************************************
 *     parse/build ASD field for SA-Atuh usages
 *********************************************************************/
uint32
  soc_pb_pp_lem_access_sa_based_asd_build(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_SA_AUTH_MAC_INFO      *auth_info,
    SOC_SAND_IN  SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO   *vid_assign_info,
    SOC_SAND_OUT uint32                        *asd
  )
{
  uint32
    tmp;
  uint32
    asd_lcl[1],
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_SA_BASED_ASD_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(asd);

  if(auth_info && vid_assign_info)
  {
    if (auth_info->expect_tag_vid != SOC_PB_PP_LLP_SA_AUTH_ACCEPT_ALL_VIDS &&
        auth_info->expect_tag_vid != vid_assign_info->vid
       )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_SA_BASED_ILLEGAL_VID_ERR, 5, exit);
    }
  }

  *asd_lcl = 0;
  /* if accept all vid then nothing to set, vid-check is disabled by default (0)*/
  /* if accept specific VID then 1. set this vid 2. enable-vid-check*/
  if (auth_info)
  {
    if (auth_info->expect_tag_vid != SOC_PB_PP_LLP_SA_AUTH_ACCEPT_ALL_VIDS)
    {
      /* check VID */
      tmp = 1;
      res = soc_sand_bitstream_set_any_field(
              &(tmp),
              SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_DROP_DIF_VID_LSB,
              SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_DROP_DIF_VID_LEN,
              asd_lcl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      /* set the VID value */
      res = soc_sand_bitstream_set_any_field(
              &(auth_info->expect_tag_vid),
              SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LSB,
              SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LEN,
              asd_lcl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

    /* if accept all port then set to perform no check, port-check is enabled by default (0)*/
    /* if accept  traffic from specific PORT then nothing to do. port will be set in destination field*/
    if (auth_info->expect_system_port.sys_id == SOC_PB_PP_LLP_SA_AUTH_ACCEPT_ALL_PORTS)
    {
      /* permit all PORTs */
      tmp = 1;
      res = soc_sand_bitstream_set_any_field(
              &(tmp),
              SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_PRMT_ALL_PORTS_LSB,
              SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_PRMT_ALL_PORTS_LEN,
              asd_lcl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }

    /* if accept all (including untagged) then set bit in order to accept untagged*/
    if (!auth_info->tagged_only)
    {
      /* accept untagged */
      tmp = 1;
      res = soc_sand_bitstream_set_any_field(
              &(tmp),
              SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_ACCPT_UNTGD_LSB,
              SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_ACCPT_UNTGD_LEN,
              asd_lcl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }
  if (vid_assign_info)
  {
    tmp = SOC_SAND_BOOL2NUM(vid_assign_info->override_tagged);
    res = soc_sand_bitstream_set_any_field(
            &(tmp),
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_TAGGED_LSB,
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_TAGGED_LEN,
            asd_lcl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    tmp = SOC_SAND_BOOL2NUM(vid_assign_info->use_for_untagged);
    res = soc_sand_bitstream_set_any_field(
            &(tmp),
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_UNTAGGED_LSB,
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_UNTAGGED_LEN,
            asd_lcl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    /* possible when copied from auth-info, to keep no change */
    if (vid_assign_info->vid != SOC_PB_PP_LLP_SA_AUTH_ACCEPT_ALL_VIDS)
    {
      tmp = vid_assign_info->vid;
      res = soc_sand_bitstream_set_any_field(
              &(tmp),
              SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LSB,
              SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LEN,
              asd_lcl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }
  }

  *asd = *asd_lcl;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_sa_based_asd_build()", 0, 0);
}

/*********************************************************************
 *     parse/build ASD field for trill-SA-Atuh usages
 *********************************************************************/
uint32
  soc_pb_pp_lem_access_trill_sa_based_asd_build(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_ADJ_INFO              *auth_info,
    SOC_SAND_OUT uint32                          *asd
  )
{
  uint32
    tmp,
    asd_lcl[1],
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(asd);
  
  *asd_lcl = 0;
  
  /* set the EEP value */
  res = soc_sand_bitstream_set_any_field(
          &(auth_info->expect_adjacent_eep),
          SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_EEP_LSB,
          SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_EEP_LEN,
          asd_lcl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* set the EEP 3 bit prefix to 000 */
  tmp = 0;
  res = soc_sand_bitstream_set_any_field(
          &(auth_info->expect_adjacent_eep),
          SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_EEP_LSB,
          SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_EEP_LEN,
          asd_lcl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  /* if accept all port then set to perform no check, port-check is enabled by default (0)*/
  /* if accept  traffic from specific PORT then nothing to do. port will be set in destination field*/
  if (auth_info->expect_system_port.sys_id == SOC_PB_PP_LLP_SA_AUTH_ACCEPT_ALL_PORTS)
  {
    /* permit all PORTs */
    tmp = 1;
    res = soc_sand_bitstream_set_any_field(
            &(tmp),
            SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_PRMT_ALL_PORTS_LSB,
            SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_PRMT_ALL_PORTS_LEN,
            asd_lcl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
  *asd = *asd_lcl;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_trill_sa_based_asd_build()", 0, 0);
}

uint32
  soc_pb_pp_lem_access_sa_based_asd_parse(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        asd,
    SOC_SAND_OUT SOC_PB_PP_LLP_SA_AUTH_MAC_INFO      *auth_info,
    SOC_SAND_OUT SOC_PB_PP_LLP_VID_ASSIGN_MAC_INFO   *vid_assign_info
  )
{
  uint32
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_SA_BASED_ASD_PARSE);

  /* if auth_info null then don't fill it*/
  if(auth_info)
  {
    /* is check VID? */
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(
            &asd,
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_DROP_DIF_VID_LSB,
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_DROP_DIF_VID_LEN,
            &tmp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    /* if set then specific VID, otherwise ALL VIDs are permitted */
    if (!tmp)
    {
      auth_info->expect_tag_vid = SOC_PB_PP_LLP_SA_AUTH_ACCEPT_ALL_VIDS;;
    }
    else /* specific VID value, get it! */
    {
      auth_info->expect_tag_vid = 0;
      /* set the VID value */
      res = soc_sand_bitstream_get_any_field(
              &asd,
              SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LSB,
              SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LEN,
              &(auth_info->expect_tag_vid)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

    /* port */
    /* is permit all PORTs */
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(
            &asd,
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_PRMT_ALL_PORTS_LSB,
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_PRMT_ALL_PORTS_LEN,
            &(tmp)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if (tmp)/* if set then permit all ports, other wise only the given port, get it from dest */
    {
      auth_info->expect_system_port.sys_id = SOC_PB_PP_LLP_SA_AUTH_ACCEPT_ALL_PORTS;
    }
    else
    {
      auth_info->expect_system_port.sys_id = 0; /* mark to get specific port*/
    }

    /* accept tagged*/
    /* is accept untagged */
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(
            &(asd),
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_ACCPT_UNTGD_LSB,
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_ACCPT_UNTGD_LEN,
            &(tmp)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if (tmp) /* accept untagged ALSO */
    {
      auth_info->tagged_only = FALSE;
    }
    else
    {
      auth_info->tagged_only = TRUE;
    }
  }
  /* if vid_assign_info null then don't fill it*/
  if(vid_assign_info)
  {
    /* use VID in tagged packets */
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(
            &asd,
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_TAGGED_LSB,
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_TAGGED_LEN,
            &tmp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    vid_assign_info->override_tagged = SOC_SAND_NUM2BOOL(tmp);

    /* use VID in untagged packets */
    tmp = 0;
    res = soc_sand_bitstream_get_any_field(
            &asd,
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_UNTAGGED_LSB,
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_OVR_VID_IN_UNTAGGED_LEN,
            &tmp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    vid_assign_info->use_for_untagged = SOC_SAND_NUM2BOOL(tmp);

    /* set the VID value */
    res = soc_sand_bitstream_get_any_field(
            &asd,
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LSB,
            SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_VID_LEN,
            &(vid_assign_info->vid)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_sa_based_asd_parse()", 0, 0);
}


uint32
  soc_pb_pp_lem_access_trill_sa_based_asd_parse(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        asd,
    SOC_SAND_OUT SOC_PB_PP_TRILL_ADJ_INFO            *auth_info
  )
{
  uint32
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(auth_info);

  /* port */
  /* is permit all PORTs */
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(
          &asd,
          SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_PRMT_ALL_PORTS_LSB,
          SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_PRMT_ALL_PORTS_LEN,
          &(tmp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if (tmp)/* if set then permit all ports, other wise only the given port, get it from dest */
  {
    auth_info->expect_system_port.sys_id = SOC_PB_PP_LLP_SA_AUTH_ACCEPT_ALL_PORTS;
  }
  else
  {
    auth_info->expect_system_port.sys_id = 0; /* mark to get specific port*/
  }

  /* eep*/
  tmp = 0;
  res = soc_sand_bitstream_get_any_field(
          &(asd),
          SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_EEP_LSB,
          SOC_PB_PP_LEM_ACCESS_ASD_TRILL_SA_AUTH_EEP_LEN,
          &(tmp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  auth_info->expect_adjacent_eep = tmp;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_access_sa_based_asd_parse()", 0, 0);
}


uint32
  soc_pb_pp_lem_ilm_key_build_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_GLBL_INFO       *glbl_info
  )
{
  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA
    ihb_flp_key_program_map_tbl_data;
  SOC_PB_PP_CE_INSTRUCTION
    inva_inst_6=SOC_PB_PP_LEM_INSTRUCTION_CEK_LABEL,
    inva_inst_12=SOC_PB_PP_LEM_INSTRUCTION_CEK_LABEL_LSB(12),
    port_inst=SOC_PB_PP_LEM_INSTRUCTION_CEK_IN_PORT,
    inrif_inst = SOC_PB_PP_LEM_INSTRUCTION_CEK_IN_RIF,
    *inst;

  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LEM_ACCESS_SA_BASED_ASD_PARSE);

  res = soc_pb_pp_ihb_flp_key_program_map_tbl_get_unsafe(
          unit,
          SOC_PB_PP_LEM_PROG_LSR_ID,
          &ihb_flp_key_program_map_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (glbl_info->key_info.mask_inrif)
  {
    inst = &inva_inst_12;
  }
  else
  {
    inst = &inrif_inst;
  }
  ihb_flp_key_program_map_tbl_data.key_b_instruction0 = soc_pb_pp_ce_instruction_build(inst);

  if (glbl_info->key_info.mask_port)
  {
    inst = &inva_inst_6;
  }
  else
  {
    inst = &port_inst;
  }
  ihb_flp_key_program_map_tbl_data.key_b_instruction1 = soc_pb_pp_ce_instruction_build(inst);

  res = soc_pb_pp_ihb_flp_key_program_map_tbl_set_unsafe(
          unit,
          SOC_PB_PP_LEM_PROG_LSR_ID,
          &ihb_flp_key_program_map_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_ilm_key_build_set()", 0, 0);
}


uint32
  soc_pb_pp_lem_trill_mc_key_build_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  uint8           mask_adjacent_nickname,
    SOC_SAND_IN  uint8           mask_fid,
    SOC_SAND_IN  uint8           mask_ing_nickname
  )
{
  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA
    ihb_flp_key_program_map_tbl_data;
  SOC_PB_PP_CE_INSTRUCTION
    inva_inst_12=SOC_PB_PP_LEM_INSTRUCTION_CEK_0(11), /* length of fid */
    inva_inst_13=SOC_PB_PP_LEM_INSTRUCTION_CEK_0(12), /* length of eep */
    adj_eep_instruction = SOC_PB_PP_LEM_INSTRUCTION_CEK_ADJ_EEP13,
    fid_instruction = SOC_PB_PP_LEM_INSTRUCTION_CEK_FID12,
    ingr_and_dist_nick_inst = SOC_PB_PP_LEM_INSTRUCTION_CEK_INICK_ENICK,
    only_dist_nick_inst = SOC_PB_PP_LEM_INSTRUCTION_CEK_DTNICK,
    *inst;

  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_pp_ihb_flp_key_program_map_tbl_get_unsafe(
          unit,
          SOC_PB_PP_LEM_PROG_TRILL_MC_ID,
          &ihb_flp_key_program_map_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /* instruction 0 is dist-tree and ingress nickname or only dist-tree-nick */
  if (mask_ing_nickname == TRUE)
  {
    inst = &only_dist_nick_inst;
  }
  else
  {
    inst = &ingr_and_dist_nick_inst;
  }
  ihb_flp_key_program_map_tbl_data.key_b_instruction0 = soc_pb_pp_ce_instruction_build(inst);

  /* instruction 1 - FID */
  if (mask_fid == TRUE)
  {
    inst = &inva_inst_12;
  }
  else
  {
    inst = &fid_instruction;
  }
  ihb_flp_key_program_map_tbl_data.key_b_instruction1 = soc_pb_pp_ce_instruction_build(inst);

  /* instruction 2 - adj EEP */
  if (mask_adjacent_nickname == TRUE)
  {
    inst = &inva_inst_13;
  }
  else
  {
    inst = &adj_eep_instruction;
  }
  ihb_flp_key_program_map_tbl_data.key_b_instruction2 = soc_pb_pp_ce_instruction_build(inst);

  /* instruction 3 is ESADI (not masked) */

  res = soc_pb_pp_ihb_flp_key_program_map_tbl_set_unsafe(
          unit,
          SOC_PB_PP_LEM_PROG_TRILL_MC_ID,
          &ihb_flp_key_program_map_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_trill_mc_key_build_set()", 0, 0);
}


uint32
  soc_pb_pp_lem_trill_mc_key_build_get(
    SOC_SAND_IN   int           unit,
    SOC_SAND_OUT  uint8           *mask_adjacent_nickname,
    SOC_SAND_OUT  uint8           *mask_fid,
    SOC_SAND_OUT  uint8           *mask_ing_nickname
  )
{
  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA
    ihb_flp_key_program_map_tbl_data;
  SOC_PB_PP_CE_INSTRUCTION
    inva_inst_12=SOC_PB_PP_LEM_INSTRUCTION_CEK_0(11), /* length of fid */
    inva_inst_13=SOC_PB_PP_LEM_INSTRUCTION_CEK_0(12), /* length of eep */
    adj_eep_instruction = SOC_PB_PP_LEM_INSTRUCTION_CEK_ADJ_EEP13,
    fid_instruction = SOC_PB_PP_LEM_INSTRUCTION_CEK_FID12,
    ingr_and_dist_nick_inst = SOC_PB_PP_LEM_INSTRUCTION_CEK_INICK_ENICK,
    only_dist_nick_inst = SOC_PB_PP_LEM_INSTRUCTION_CEK_DTNICK;

  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_pp_ihb_flp_key_program_map_tbl_get_unsafe(
          unit,
          SOC_PB_PP_LEM_PROG_TRILL_MC_ID,
          &ihb_flp_key_program_map_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /* instruction 0 is dist-tree and ingress nickname (not masked) */
  if (ihb_flp_key_program_map_tbl_data.key_b_instruction0 == soc_pb_pp_ce_instruction_build(&only_dist_nick_inst))
  {
    *mask_ing_nickname = TRUE;
  }
  else if (ihb_flp_key_program_map_tbl_data.key_b_instruction0 == soc_pb_pp_ce_instruction_build(&ingr_and_dist_nick_inst))
  {
    *mask_ing_nickname = FALSE;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEYS_DONT_MATCH, 60, exit);
  }

  /* instruction 1 - FID */
  if (ihb_flp_key_program_map_tbl_data.key_b_instruction1 == soc_pb_pp_ce_instruction_build(&inva_inst_12))
  {
    *mask_fid = TRUE;
  }
  else if (ihb_flp_key_program_map_tbl_data.key_b_instruction1 == soc_pb_pp_ce_instruction_build(&fid_instruction))
  {
    *mask_fid = FALSE;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEYS_DONT_MATCH, 60, exit);
  }

  /* instruction 2 - adj EEP */
  if (ihb_flp_key_program_map_tbl_data.key_b_instruction2 == soc_pb_pp_ce_instruction_build(&inva_inst_13))
  {
    *mask_adjacent_nickname = TRUE;
  }
  else if (ihb_flp_key_program_map_tbl_data.key_b_instruction2 == soc_pb_pp_ce_instruction_build(&adj_eep_instruction))
  {
    *mask_adjacent_nickname = FALSE;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEYS_DONT_MATCH, 60, exit);
  }

  /* instruction 3 is ESADI (not masked) */
  
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lem_trill_mc_key_build_get()", 0, 0);
}

/*
 *	Clear functions
 */
void
  SOC_PB_PP_LEM_ACCESS_KEY_ENCODED_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY_ENCODED *info
  )
{
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_LEM_ACCESS_KEY_ENCODED));
  for (indx = 0; indx < SOC_PB_PP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S; ++indx)
  {
    info->buffer[indx] = 0;
  }

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_LEM_ACCESS_KEY_PARAM_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY_PARAM *info
  )
{
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_LEM_ACCESS_KEY_PARAM));
  for (indx = 0; indx < SOC_PB_PP_LEM_KEY_PARAM_MAX_IN_UINT32S; ++indx)
  {
    info->value[indx] = 0;
  }
  info->nof_bits = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY_PREFIX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_LEM_ACCESS_KEY_PREFIX));

  info->value = 0;
  info->nof_bits = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_LEM_ACCESS_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY *info
  )
{
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_LEM_ACCESS_KEY));
  info->type = SOC_PB_PP_LEM_ACCESS_NOF_KEY_TYPES;
  for (indx = 0; indx < SOC_PB_PP_LEM_KEY_MAX_NOF_PARAMS; ++indx)
  {
    SOC_PB_PP_LEM_ACCESS_KEY_PARAM_clear(&(info->param[indx]));
  }
  info->nof_params = 0;
  SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_clear(&(info->prefix));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_REQUEST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_LEM_ACCESS_REQUEST));
  SOC_PB_PP_LEM_ACCESS_KEY_clear(&(info->key));
  info->command = SOC_PB_PP_LEM_ACCESS_NOF_CMDS;
  info->stamp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_PAYLOAD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_LEM_ACCESS_PAYLOAD));
  info->dest = 0;
  info->age = 0;
  info->asd = 0;
  info->is_dynamic = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_LEM_ACCESS_OUTPUT_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_OUTPUT  *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_LEM_ACCESS_OUTPUT));
  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&(info->request));
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&(info->payload));
  info->is_learned_first_by_me = 0;
  info->req_origin = SOC_PB_PP_LEM_ACCCESS_NOF_REQ_ORIGINS;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_ACK_STATUS  *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_LEM_ACCESS_ACK_STATUS));
  info->is_success = 0;
  info->reason = SOC_PB_PP_LEM_ACCESS_NOF_FAIL_REASONS;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_LEM_ACCESS_BUFFER_clear(
    SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_BUFFER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PP_LEM_ACCESS_BUFFER));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32
  SOC_PB_PP_LEM_ACCESS_KEY_PARAM_verify(
    SOC_SAND_IN SOC_PB_PP_LEM_ACCESS_KEY_TYPE   type,
    SOC_SAND_IN SOC_PB_PP_LEM_ACCESS_KEY_PARAM *info,
    SOC_SAND_IN uint32                   params_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(type, SOC_PB_PP_LEM_ACCESS_NOF_KEY_TYPES-1, SOC_PB_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  switch(type)
  {
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_MAC:
    if (params_ndx == 0)
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 11, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], (1 << (SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC - 32))-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 12, exit);
    }
    else  /* params_ndx == 1 */
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_MAC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_MAC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 13, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_MAC)-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 14, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 15, exit);
    }
    break;

  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_BACKBONE_MAC:
    if (params_ndx == 0)
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_BACKBONE_MAC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_BACKBONE_MAC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 16, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], (1 << (SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_BACKBONE_MAC - 32))-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 17, exit);
    }
    else  /* params_ndx == 1 */
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_BACKBONE_MAC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_BACKBONE_MAC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 18, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_BACKBONE_MAC)-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 19, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 20, exit);
    }
    break;

  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IPV4_MC:
    if (params_ndx == 0)
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 21, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC)-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 22, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 23, exit);
    }
    else  /* params_ndx == 1 */
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 24, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC)-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 25, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 26, exit);
    }
    break;

   case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IP_HOST:
    if (params_ndx == 0)
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IP_HOST, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IP_HOST, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 27, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[2], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 29, exit);
    }
    else  /* params_ndx == 1 */
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IP_HOST, SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IP_HOST, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 30, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[2], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 33, exit);
    }
    break;

   case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_SA_AUTH:
     if (params_ndx == 0)
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 11, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], (1 << (SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC - 32))-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 12, exit);
     }
   break;


   case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_ILM:
     if (params_ndx == 0)
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 37, exit);
       
     }
     else  if (params_ndx == 1 )
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM, SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 41, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM)-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 42, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 47, exit);
     }
     else if ( params_ndx == 2 )
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM, SOC_PB_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 41, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << SOC_PB_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM)-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 42, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 47, exit);
     }
     else  if ( params_ndx == 3 )
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM, SOC_PB_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 41, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << SOC_PB_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM)-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 42, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 47, exit);
     }
     break;

   case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_TRILL_UC:
     if (params_ndx == 0)
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_UC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_UC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 43, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_UC)-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 44, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 47, exit);
     }
     break;

   case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_TRILL_MC:
     if (params_ndx == 0)
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_MC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_MC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 43, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_MC)-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 44, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 45, exit);
     }
     else if (params_ndx == 1)
     {       
       SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_TRILL_MC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 46, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_TRILL_MC)-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 47, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 48, exit);
     }
     else if (params_ndx == 2)
     {       
       SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_TRILL_MC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 48, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << SOC_PB_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_TRILL_MC)-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 50, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 51, exit);
     }
     else if (params_ndx == 3)
     {       
       SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_TRILL_MC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 52, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << SOC_PB_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_TRILL_MC)-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 53, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 54, exit);
     }
     else if (params_ndx == 4)
     {       
       SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PARAM4_IN_BITS_FOR_TRILL_MC, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 55, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << SOC_PB_PP_LEM_ACCESS_KEY_PARAM4_IN_BITS_FOR_TRILL_MC)-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 56, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 57, exit);
     }
     break;

   case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_EXTENDED:
     if (params_ndx == 0)
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_L2_LIF_KEY_TYPE_EXTENDED_PARAM0_NOF_BITS, SOC_PB_PP_L2_LIF_KEY_TYPE_EXTENDED_PARAM0_NOF_BITS, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 45, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], (1 << (SOC_PB_PP_L2_LIF_KEY_TYPE_EXTENDED_PARAM0_NOF_BITS - 32))-1, SOC_PB_PP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 65, exit);
     }
     break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 60, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LEM_ACCESS_KEY_PARAM_verify()",type,params_ndx);
}

uint32
  SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_verify(
    SOC_SAND_IN SOC_PB_PP_LEM_ACCESS_KEY_TYPE type,
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_KEY_PREFIX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(type, SOC_PB_PP_LEM_ACCESS_NOF_KEY_TYPES-1, SOC_PB_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  switch(type)
  {
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_MAC:
    if (info->nof_bits != SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_MAC)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 11, exit);
    }
    if (info->value != SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_MAC)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 12, exit);
    }
    break;

  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_BACKBONE_MAC:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_BACKBONE_MAC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_BACKBONE_MAC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 13, exit);
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->value, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_BACKBONE_MAC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_BACKBONE_MAC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 14, exit);
    break;

  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IPV4_MC:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV4_MC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV4_MC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 15, exit);
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->value, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_IPV4_MC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_IPV4_MC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 16, exit);
    break;

  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IP_HOST:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IP_HOST, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IP_HOST, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 17, exit);
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->value, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_IP_HOST, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_IP_HOST, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 18, exit);
    break;

  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_SA_AUTH:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_SA_AUTH, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_SA_AUTH, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 19, exit);
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->value, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_SA_AUTH, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_SA_AUTH, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 20, exit);
  break;

  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_ILM:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_ILM, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_ILM, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 17, exit);
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->value, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_ILM, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_ILM, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 18, exit);
    break;
  
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_EXTENDED:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_EXTENDED, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_EXTENDED, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 17, exit);
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->value, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_EXTENDED, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_EXTENDED, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 18, exit);
    break;

  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_TRILL_UC:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_UC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_UC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 19, exit);
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->value, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_UC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_UC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 20, exit);
    break;

  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_TRILL_MC:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_MC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_MC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 19, exit);
    SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->value, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_MC, SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 20, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 21, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_verify()",0,0);
}

uint32
  SOC_PB_PP_LEM_ACCESS_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_KEY *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    params_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_LEM_ACCESS_NOF_KEY_TYPES-1, SOC_PB_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  switch(info->type)
  {
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_MAC:
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_BACKBONE_MAC:
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IPV4_MC:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_params, SOC_PB_PP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_MAC_BMAC_IPV4, SOC_PB_PP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_MAC_BMAC_IPV4, SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR, 11, exit);
    break;
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_IP_HOST:/* can be 1 or 2 depends if vrf exist */
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_params, 1, SOC_PB_PP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_MAC_BMAC_IPV4, SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR, 11, exit);
    break;
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_ILM:

    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_params, SOC_PB_PP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_ILM, SOC_PB_PP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_ILM, SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR, 11, exit);
    break;
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_EXTENDED:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_params, SOC_PB_PP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_EXTEND_P2P, SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR, 11, exit);
    break;
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_SA_AUTH:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_params, SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_SA_AUTH, SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_SA_AUTH, SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR, 12, exit);
  break;
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_TRILL_UC:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_params, SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_UC, SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_UC, SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR, 13, exit);
  break;
  case SOC_PB_PP_LEM_ACCESS_KEY_TYPE_TRILL_MC:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_params, SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_MC, SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_MC, SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR, 14, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }
  for (params_ndx = 0; params_ndx < info->nof_params; ++params_ndx)
  {
    res = SOC_PB_PP_LEM_ACCESS_KEY_PARAM_verify(info->type, &(info->param[params_ndx]), params_ndx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20 + params_ndx, exit);
  }

  res = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_verify(info->type, &(info->prefix));
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LEM_ACCESS_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_LEM_ACCESS_REQUEST_verify(
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_REQUEST *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LEM_ACCESS_KEY, &(info->key), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->command, SOC_PB_PP_LEM_ACCESS_CMD_MAX, SOC_PB_PP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->stamp, 0, SOC_PB_PP_LEM_STAMP_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LEM_ACCESS_REQUEST_verify()",0,0);
}

uint32
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_verify(
    SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_PAYLOAD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->dest, (1<<SOC_PB_PP_LEM_ACCESS_DEST_NOF_BITS)-1, SOC_PB_PP_LEM_DEST_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->asd, (1<<SOC_PB_PP_LEM_ACCESS_ASD_NOF_BITS)-1, SOC_PB_PP_LEM_ASD_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->age, SOC_PB_PP_FRWRD_MACT_ENTRY_KEY_AGE_STATUS_MAX+1, SOC_PB_PP_LEM_AGE_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LEM_ACCESS_PAYLOAD_verify()",0,0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_lem_access module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_lem_access_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_lem_access;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_lem_access module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_lem_access_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_lem_access;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

