/* $Id: jer_pp_kaps_xpt.c, hagayco Exp $
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
 * $
*/

#include <soc/mem.h>

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD
#include <soc/mem.h>

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>

#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_xpt.h>

#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* #define JER_KAPS_XPT_PRINTS_ENABLED */

#define JER_KAPS_XPT_EXTENDED_FUNC_ENUM     KAPS_FUNC14
#define JER_KAPS_XPT_RPB_FUNC_KAPS_SEARCH   KAPS_FUNC3

/* Block ID info */
#define JER_KAPS_XPT_BLOCK_OFFSET_RPB   1
#define JER_KAPS_XPT_BLOCK_OFFSET_BB    5
#define JER_KAPS_XPT_BLOCK_OFFSET_BRR   37
#define JER_KAPS_XPT_NOF_BLOCKS         40

/* Response values */
#define JER_KAPS_XPT_RESPONSE_VALID             3
#define JER_KAPS_XPT_RESPONSE_EXTENDED_VALID    2
#define JER_KAPS_RESPONSE_STATUS_AOK            0

/* } */

/*************
 *  MACROS   *
 *************/
/* { */


/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */
typedef enum
{
    DATA_0,
    DATA_1,
    DATA_2,
    DATA_3,
    DATA_4,
    DATA_5,
    DATA_6,
    DATA_7,
    DATA_8,
    DATA_9,
    DATA_10,
    DATA_11,
    DATA_12,
    DATA_13,
    DATA_14,

    DATA_LAST


}JER_KAPS_DATA_REGS;

typedef struct
{
    soc_reg_t reg;
    uint8 index;
}KAPS_REG_ACCESS_INFO;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

KAPS_REG_ACCESS_INFO offset_to_register_access_info[] = 
{
    /* register                               index       offset */   
    /*-----------------------------------------------------------*/      
    {KAPS_REVISIONr,                            0   },  /* 0x0 */
    {KAPS_CONFIGr,                              0   },  /* 0x1 */
    {KAPS_IBC_CONFIGr,                          0   },  /* 0x2 */
    {KAPS_GLOBAL_EVENTr,                        0   },  /* 0x3 */
    {KAPS_GLOBAL_EVENT_MASKr,                   0   },  /* 0x4 */
    {KAPS_R_5_FIFO_MON_EVENTr,                  0   },  /* 0x5 */
    {KAPS_R_5_FIFO_MON_EVENT_MASKr,             0   },  /* 0x6 */
    {KAPS_R_5_ATTNr,                            0   },  /* 0x7 */
    {KAPS_R_5_ATTNr,                            1   },  /* 0x8 */
    {KAPS_R_5_ATTNr,                            2   },  /* 0x9 */
    {KAPS_R_5_ATTNr,                            3   },  /* 0xa */
    {KAPS_R_5_COUNTERSr,                        0   },  /* 0xb */
    {KAPS_R_5_COUNTERSr,                        1   },  /* 0xc */
    {KAPS_R_5_COUNTERSr,                        2   },  /* 0xd */
    {KAPS_R_5_COUNTERSr,                        3   },  /* 0xe */
    {KAPS_R_5_COUNTERSr,                        4   },  /* 0xf */
    {KAPS_R_5_COUNTERSr,                        5   },  /* 0x10 */
    {KAPS_R_5_COUNTERSr,                        6   },  /* 0x11 */
    {KAPS_R_5_COUNTERSr,                        7   },  /* 0x12 */
    {KAPS_R_5_COUNTERSr,                        8   },  /* 0x13 */
    {KAPS_R_5_COUNTERSr,                        9   },  /* 0x14 */
    {KAPS_R_5_COUNTERSr,                        10  },  /* 0x15 */
    {KAPS_R_5_COUNTERSr,                        11  },  /* 0x16 */
    {KAPS_R_5_COUNTERSr,                        12  },  /* 0x17 */
    {KAPS_R_5_COUNTERSr,                        13  },  /* 0x18 */
    {KAPS_R_5_COUNTERSr,                        14  },  /* 0x19 */
    {KAPS_R_5_COUNTERSr,                        15  },  /* 0x1a */
    {KAPS_MEMORY_A_ERRORr,                      0   },  /* 0x1b */
    {KAPS_MEMORY_B_ERRORr,                      0   },  /* 0x1c */
    {KAPS_SEARCH_0A_ERRORr,                     0   },  /* 0x1d */
    {KAPS_MEMORY_A_ERROR_MASKr,                 0   },  /* 0x1e */
    {KAPS_MEMORY_B_ERROR_MASKr,                 0   },  /* 0x1f */
    {KAPS_SEARCH_0A_ERROR_MASKr,                0   },  /* 0x20 */
    {KAPS_SEARCH_1A_ERRORr,                     0   },  /* 0x21 */
    {KAPS_SEARCH_1A_ERROR_MASKr,                0   },  /* 0x22 */
    {KAPS_SEARCH_0B_ERRORr,                     0   },  /* 0x23 */
    {KAPS_SEARCH_0B_ERROR_MASKr,                0   },  /* 0x24 */
    {KAPS_SEARCH_1B_ERRORr,                     0   },  /* 0x25 */
    {KAPS_SEARCH_1B_ERROR_MASKr,                0   },  /* 0x26 */
    {KAPS_CONFIG_CFIFOr,                        0   },  /* 0x27 */
    {KAPS_CONFIG_CFIFOr,                        1   },  /* 0x28 */
    {KAPS_CONFIG_CFIFOr,                        2   },  /* 0x29 */
    {KAPS_CONFIG_CFIFOr,                        3   },  /* 0x2a */
    {KAPS_CONFIG_CFIFO_THRESHr,                 0   },  /* 0x2b */
    {KAPS_CONFIG_CFIFO_THRESHr,                 1   },  /* 0x2c */
    {KAPS_CONFIG_CFIFO_THRESHr,                 2   },  /* 0x2d */
    {KAPS_CONFIG_CFIFO_THRESHr,                 3   },  /* 0x2e */
    {KAPS_STATUS_CFIFOr,                        0   },  /* 0x2f */
    {KAPS_STATUS_CFIFOr,                        1   },  /* 0x30 */
    {KAPS_STATUS_CFIFOr,                        2   },  /* 0x31 */
    {KAPS_STATUS_CFIFOr,                        3   },  /* 0x32 */
    {KAPS_CONFIG_RFIFOr,                        0   },  /* 0x33 */
    {KAPS_CONFIG_RFIFOr,                        1   },  /* 0x34 */
    {KAPS_CONFIG_RFIFOr,                        2   },  /* 0x35 */
    {KAPS_CONFIG_RFIFOr,                        3   },  /* 0x36 */
    {KAPS_STATUS_RFIFOr,                        0   },  /* 0x37 */
    {KAPS_STATUS_RFIFOr,                        1   },  /* 0x38 */
    {KAPS_STATUS_RFIFOr,                        2   },  /* 0x39 */
    {KAPS_STATUS_RFIFOr,                        3   },  /* 0x3a */
    {KAPS_CONFIG_FIFO_MONr,                     0   },  /* 0x3b */
    {0,                                         0   },  /* 0x3c */ 
    {0,                                         0   },  /* 0x3d */ 
    {0,                                         0   },  /* 0x3e */ 
    {0,                                         0   },  /* 0x3f */                                                 
    {KAPS_IBC_COMMAND_DATAr,                    0   },  /* 0x40 */
    {KAPS_IBC_COMMAND_DATAr,                    1   },  /* 0x41 */
    {KAPS_IBC_COMMAND_DATAr,                    2   },  /* 0x42 */
    {KAPS_IBC_COMMAND_DATAr,                    3   },  /* 0x43 */
    {KAPS_IBC_COMMAND_DATAr,                    4   },  /* 0x44 */
    {KAPS_IBC_COMMAND_DATAr,                    5   },  /* 0x45 */
    {KAPS_IBC_COMMAND_DATAr,                    6   },  /* 0x46 */
    {KAPS_IBC_COMMAND_DATAr,                    7   },  /* 0x47 */
    {KAPS_IBC_COMMAND_DATAr,                    8   },  /* 0x48 */
    {KAPS_IBC_COMMAND_DATAr,                    9   },  /* 0x49 */
    {KAPS_IBC_COMMAND_DATAr,                    10  },  /* 0x4a */
    {KAPS_IBC_COMMAND_DATAr,                    11  },  /* 0x4b */
    {KAPS_IBC_COMMAND_DATAr,                    12  },  /* 0x4c */
    {KAPS_IBC_COMMAND_DATAr,                    13  },  /* 0x4d */
    {KAPS_IBC_COMMAND_DATAr,                    14  },  /* 0x4e */
    {KAPS_IBC_COMMAND_WORDr,                    0   },  /* 0x4f */
    {KAPS_IBC_RESPONSE_DATAr,                   0   },  /* 0x50 */
    {KAPS_IBC_RESPONSE_DATAr,                   1   },  /* 0x51 */
    {KAPS_IBC_RESPONSE_DATAr,                   2   },  /* 0x52 */
    {KAPS_IBC_RESPONSE_DATAr,                   3   },  /* 0x53 */
    {KAPS_IBC_RESPONSE_DATAr,                   4   },  /* 0x54 */
    {KAPS_IBC_RESPONSE_DATAr,                   5   },  /* 0x55 */
    {KAPS_IBC_RESPONSE_DATAr,                   6   },  /* 0x56 */
    {KAPS_IBC_RESPONSE_DATAr,                   7   },  /* 0x57 */
    {KAPS_IBC_RESPONSE_DATAr,                   8   },  /* 0x58 */
    {KAPS_IBC_RESPONSE_DATAr,                   9   },  /* 0x59 */
    {KAPS_IBC_RESPONSE_DATAr,                   10  },  /* 0x5a */
    {KAPS_IBC_RESPONSE_DATAr,                   11  },  /* 0x5b */
    {KAPS_IBC_RESPONSE_DATAr,                   12  },  /* 0x5c */
    {KAPS_IBC_RESPONSE_DATAr,                   13  },  /* 0x5d */
    {KAPS_IBC_RESPONSE_DATAr,                   14  },  /* 0x5e */
    {KAPS_IBC_RESPONSE_WORDr,                   0   },  /* 0x5f */
    {KAPS_IBC_QUEUE_STATUSr,                    0   },  /* 0x60 */
    {KAPS_IBC_FIFO_MEM_CONTROLr,                0   },  /* 0x61 */
    {KAPS_IBC_FIFO_ECC_DEBUGr,                  0   },  /* 0x62 */
    {KAPS_IBC_FIFO_CORRECTABLE_STATUSr,         0   },  /* 0x63 */
    {KAPS_IBC_FIFO_UNCORRECTABLE_STATUSr,       0   },  /* 0x64 */
    {KAPS_R_5_CORE_CONTROLr,                    0   },  /* 0x65 */
    {KAPS_R_5_CORE_MEM_CONTROLr,                0   },  /* 0x66 */
    {KAPS_R_5_ECC_ERR_WR_TCM_Ar,                0   },  /* 0x67 */
    {KAPS_R_5_ECC_ERR_WR_TCM_B_0r,              0   },  /* 0x68 */
    {KAPS_R_5_ECC_ERR_WR_TCM_B_1r,              0   },  /* 0x69 */
    {KAPS_R_5_ECC_ERR_RD_TCM_Ar,                0   },  /* 0x6a */
    {KAPS_R_5_ECC_ERR_RD_TCM_B_0r,              0   },  /* 0x6b */
    {KAPS_R_5_ECC_ERR_RD_TCM_B_1r,              0   },  /* 0x6c */
    {KAPS_R_5_CORE_CONFIGAr,                    0   },  /* 0x6d */
    {KAPS_R_5_CORE_CONFIGBr,                    0   },  /* 0x6e */
    {KAPS_R_5_CORE_CONFIGCr,                    0   },  /* 0x6f */
    {KAPS_R_5_CORE_CONFIGDr,                    0   },  /* 0x70 */
    {KAPS_R_5_CORE_CONFIGEr,                    0   },  /* 0x71 */
    {KAPS_R_5_AXI_CONFIGr,                      0   },  /* 0x72 */
    {KAPS_R_5_AXI_ERROR_STATUS_0r,              0   },  /* 0x73 */
    {KAPS_R_5_AXI_ERROR_STATUS_1r,              0   },  /* 0x74 */
    {KAPS_R_5_DEBUG_CONFIGAr,                   0   },  /* 0x75 */
    {KAPS_R_5_DEBUG_CONFIGBr,                   0   },  /* 0x76 */
    {KAPS_R_5_CONFIGAr,                         0   },  /* 0x77 */
    {KAPS_R_5_DEBUG_STATUSr,                    0   },  /* 0x78 */
    {KAPS_R_5_DEBUG_STATUS_EVENT_BUS_0r,        0   },  /* 0x79 */
    {KAPS_R_5_DEBUG_STATUS_EVENT_BUS_0_MASKr,   0   },  /* 0x7a */
    {KAPS_R_5_DEBUG_STATUS_EVENT_BUS_1r,        0   },  /* 0x7b */
    {KAPS_R_5_DEBUG_STATUS_EVENT_BUS_1_MASKr,   0   },  /* 0x7c */
    {KAPS_R_5_DEBUG_EBUS_PATTERN_0r,            0   },  /* 0x7d */
    {KAPS_R_5_DEBUG_EBUS_PATTERN_1r,            0   },  /* 0x7e */
    {KAPS_R_5_DEBUG_EBUS_PATTERN_0_MASKr,       0   },  /* 0x7f */
    {KAPS_R_5_DEBUG_EBUS_PATTERN_1_MASKr,       0   },  /* 0x80 */
    {KAPS_R_5_DAP_APB_CTRLr,                    0   },  /* 0x81 */
    {KAPS_R_5_DAP_APB_ADDRr,                    0   },  /* 0x82 */
    {KAPS_R_5_DAP_APB_WDATAr,                   0   },  /* 0x83 */
    {KAPS_R_5_DAP_APB_RDATAr,                   0   },  /* 0x84 */
    {KAPS_GLOBAL_DEBUGr,                        0   },  /* 0x85 */
    {KAPS_R_5_TM_TEST_CTRLr,                    0   },  /* 0x86 */
    {KAPS_IBC_FIFO_TM_TEST_CTRLr,               0   },  /* 0x87 */
    {KAPS_PD_ASSIST_DEBUGr,                     0   }   /* 0x88 */  
};
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC uint32 jer_pp_kaps_command_word_poll( int unit,
                                             uint32 expected_reg_val)
{
    int rv;
    uint32 reg_val;
    soc_timeout_t to;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SAL_BOOT_PLISIM) {

        /* polling command word to validate it's ready */
        soc_timeout_init(&to, ARAD_TIMEOUT, ARAD_MIN_POLLS);
        for(;;){
            /* get command word register */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
            {
                soc_reg_above_64_val_t above_64_val;
                above_64_val[0] = 0;
                above_64_val[1] = 0;
                rv = ARAD_FAST_REGISER_GET(ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_WORD, above_64_val);
                reg_val = above_64_val[0];
            }
#else
            rv = READ_KAPS_IBC_COMMAND_WORDr(unit, &reg_val);
#endif
            SOCDNX_IF_ERR_EXIT(rv);
            /* validate against expected value */        
            if(reg_val == expected_reg_val) {
                break;
            }

            if (soc_timeout_check(&to)) {
                SOCDNX_IF_ERR_EXIT_MSG(SOC_SAND_GEN_ERR, (_BSL_SOCDNX_MSG("Command word register not ready.")));
                break;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32 jer_pp_kaps_response_word_poll(int unit,
                                             uint32 blk_id,
                                             uint32 rsp,
                                             uint32 func,
                                             uint32 status)
{
    int rv;
    uint32 reg_val, field_val, expected_reg_val = 0;
    soc_timeout_t to;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SAL_BOOT_PLISIM) {
        /* 
         * build expected register value
         */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
        field_val = blk_id;
        ARAD_FAST_FIELD_SET(ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_BLKID, &expected_reg_val, &field_val);

        field_val = rsp;
        ARAD_FAST_FIELD_SET(ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_RSP, &expected_reg_val, &field_val);

        field_val = func;
        ARAD_FAST_FIELD_SET(ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_FUNC, &expected_reg_val, &field_val);

        field_val = status;
        ARAD_FAST_FIELD_SET(ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_STATUS, &expected_reg_val, &field_val);
#else
        field_val = blk_id;
        soc_reg_field_set(unit, KAPS_IBC_RESPONSE_WORDr, &expected_reg_val, BLKIDf, field_val);

        field_val = rsp;
        soc_reg_field_set(unit, KAPS_IBC_RESPONSE_WORDr, &expected_reg_val, RSPf, field_val);

        field_val = func;
        soc_reg_field_set(unit, KAPS_IBC_RESPONSE_WORDr, &expected_reg_val, FUNCf, field_val);

        field_val = status;
        soc_reg_field_set(unit, KAPS_IBC_RESPONSE_WORDr, &expected_reg_val, STATUSf, field_val);
#endif /*ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS*/
        /* polling response word  */
        soc_timeout_init(&to, ARAD_TIMEOUT, ARAD_MIN_POLLS);
        for(;;){
            /* get response word register */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
            {
                soc_reg_above_64_val_t above_64_val;
                above_64_val[0] = 0;
                above_64_val[1] = 0;
                rv = ARAD_FAST_REGISER_GET(ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_WORD, above_64_val);
                reg_val = above_64_val[0];
            }
#else
            rv = READ_KAPS_IBC_RESPONSE_WORDr(unit, &reg_val);
#endif
            SOCDNX_IF_ERR_EXIT(rv);
      
            /* validate blk_id, rsp, func, status */      
            if(reg_val == expected_reg_val) {
                break;
            }

            if (soc_timeout_check(&to)) {
                SOCDNX_IF_ERR_EXIT_MSG(SOC_SAND_GEN_ERR, (_BSL_SOCDNX_MSG("Response word register not ready.")));
                break;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32 jer_pp_kaps_command_word_write(int unit,
                                             uint32 blk_id,
                                             uint32 cmd,
                                             uint32 func,
                                             uint32 offset)
{
    int rv, res = KBP_OK;
    uint32 field_val, reg_val;

    /* 
     * build register value
     */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
    field_val = blk_id;
    ARAD_FAST_FIELD_SET(ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_BLKID, &reg_val, &field_val);

    field_val = cmd;
    ARAD_FAST_FIELD_SET(ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_CMD, &reg_val, &field_val);

    field_val = func;
    ARAD_FAST_FIELD_SET(ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_FUNC, &reg_val, &field_val);

    field_val = offset;
    ARAD_FAST_FIELD_SET(ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_OFFSET, &reg_val, &field_val);

    /* set command word register */
    {
        soc_reg_above_64_val_t above_64_val;
        above_64_val[0] = reg_val;
        above_64_val[1] = 0;
        rv = ARAD_FAST_REGISER_SET(ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_WORD, above_64_val);
    }
#else
    field_val = blk_id;
    soc_reg_field_set(unit, KAPS_IBC_COMMAND_WORDr, &reg_val, BLKIDf, field_val);

    field_val = cmd;
    soc_reg_field_set(unit, KAPS_IBC_COMMAND_WORDr, &reg_val, CMDf, field_val);

    field_val = func;
    soc_reg_field_set(unit, KAPS_IBC_COMMAND_WORDr, &reg_val, FUNCf, field_val);

    field_val = offset;
    soc_reg_field_set(unit, KAPS_IBC_COMMAND_WORDr, &reg_val, OFFSETf, field_val);

    /* set command word register */
    rv = WRITE_KAPS_IBC_COMMAND_WORDr(unit, reg_val);
#endif /*ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS*/
    if (rv != SOC_E_NONE) {
        res = KBP_INTERNAL_ERROR;
    }

#ifdef JER_KAPS_XPT_PRINTS_ENABLED
    LOG_CLI((BSL_META_U(0, "%s():  blk_id: %d, cmd: %d, func: %d, offset: %d, reg_val:0x%08X\n"), 
                         FUNCTION_NAME(), blk_id, cmd, func, offset, reg_val));
#endif
    return res;
}


STATIC kbp_status jer_pp_kaps_search(void *xpt, 
                                     uint8_t *key, 
                                     enum kaps_search_interface search_interface, 
                                     struct kaps_search_result *kaps_result)
{
    int rv, unit;
    uint32  blk_id,
            func,
        tmp, val,i =0,j,k;
    JER_KAPS_DATA_REGS data_reg; 

    unit = ((JER_KAPS_XPT*)xpt)->jer_data.unit;
    blk_id = search_interface + JER_KAPS_XPT_BLOCK_OFFSET_RPB;
    func = JER_KAPS_XPT_RPB_FUNC_KAPS_SEARCH;

    /* polling command word to validate it's ready */
    rv = jer_pp_kaps_command_word_poll(unit, 0);
    if (rv != SOC_SAND_OK) {
        return KBP_INTERNAL_ERROR;
    }

    /* write the key to the data registers */
    data_reg = DATA_10;

    for (k = 0; k < JER_KAPS_KEY_BUFFER_NOF_BYTES/4; k++) {
        tmp = 0;
        for(j=0; j<4; j++) {
            val = key[i];
            tmp |= val << (3-j)*8;
            i++;
        }
        /* write tmp to data register */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
        {
            soc_reg_above_64_val_t above_64_val;
            above_64_val[0] = tmp;
            above_64_val[1] = 0;
            ARAD_FAST_REGISER_SET(ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_0 + data_reg, above_64_val);
        }
#else
        WRITE_KAPS_IBC_COMMAND_DATAr(unit, data_reg, tmp);
#endif
        data_reg++;
    }

    /* trigger search action by writing to command word register */
    rv = jer_pp_kaps_command_word_write(unit, blk_id, KAPS_CMD_READ, func, 0);
    if (rv != SOC_SAND_OK) {
        return KBP_INTERNAL_ERROR;
    }

    /* poll response word until response is ready */
    rv = jer_pp_kaps_response_word_poll(unit, blk_id, JER_KAPS_XPT_RESPONSE_VALID, func, JER_KAPS_RESPONSE_STATUS_AOK);
    if (rv != SOC_SAND_OK) {
        return KBP_INTERNAL_ERROR;
    }

    /* read response_data_14, bits 24-31 */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
    {
        soc_reg_above_64_val_t above_64_val;
        above_64_val[0] = val;
        above_64_val[1] = 0;
        ARAD_FAST_REGISER_GET(ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_0 + DATA_14, above_64_val);
    }
#else
    READ_KAPS_IBC_RESPONSE_DATAr(unit, DATA_14, &val);
#endif
    kaps_result->match_len = val >> 24;

    /* read response_data_13, bits 0-19 */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
    {
        soc_reg_above_64_val_t above_64_val;
        above_64_val[0] = val;
        above_64_val[1] = 0;
        ARAD_FAST_REGISER_GET(ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_0 + DATA_13, above_64_val);
    }
#else
    READ_KAPS_IBC_RESPONSE_DATAr(unit, DATA_13, &val);
#endif
    kaps_result->ad_value[2]= (val << 4);
    kaps_result->ad_value[1]= (val >> 4);
    kaps_result->ad_value[0]= (val >> 12);

    /* zero response word to dequeue response fifo */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
    {
        soc_reg_above_64_val_t above_64_val;
        above_64_val[0] = 0;
        above_64_val[1] = 0;
        rv = ARAD_FAST_REGISER_SET(ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_WORD, above_64_val);
    }
#else
    rv = WRITE_KAPS_IBC_RESPONSE_WORDr(unit, 0);
#endif
    if (rv != SOC_SAND_OK) {
        return KBP_INTERNAL_ERROR;
    }

#ifdef JER_KAPS_XPT_PRINTS_ENABLED
    LOG_CLI((BSL_META_U(0,"%s()\n"), FUNCTION_NAME()));
#endif
    return rv;
}

kbp_status jer_pp_kaps_write_command(void *xpt,  
                                            uint8 blk_id, 
                                            uint32 cmd,
                                            uint32 func,
                                            uint32 offset,
                                            uint32 nbytes, 
                                            uint8 *bytes)
{
    int rv, unit;
    uint32 tmp, val,i =0,j,k;
    JER_KAPS_DATA_REGS data_reg; 

    unit = ((JER_KAPS_XPT*)xpt)->jer_data.unit;

#ifdef JER_KAPS_XPT_PRINTS_ENABLED
    LOG_CLI((BSL_META_U(0,"%s() start\n nbytes: %d, bytes: 0x"),FUNCTION_NAME(), nbytes));
    for (j = 0; j < nbytes; j++) {
        LOG_CLI((BSL_META_U(0,"%02X "),bytes[j]));
    }
    LOG_CLI((BSL_META_U(0,"\n")));
#endif

    /* polling command word to validate it's ready */
    rv = jer_pp_kaps_command_word_poll(unit, 0);
    if (rv != SOC_SAND_OK) {
        return KBP_INTERNAL_ERROR;
    }

    /* write to command data registers */
    memset(&tmp, 0 , sizeof(uint32));
    data_reg = DATA_LAST - SOC_SAND_DIV_ROUND_UP(nbytes,4);

    if (nbytes%4 > 0) {
        tmp = 0;
        for (i = 0; i < nbytes%4; i++) {
            val = bytes[i];
            tmp |= val << (nbytes%4-1-i)*8;
        }
        /* write to data register */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
        {
            soc_reg_above_64_val_t above_64_val;
            above_64_val[0] = tmp;
            above_64_val[1] = 0;
            ARAD_FAST_REGISER_SET(ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_0 + data_reg, above_64_val);
        }
#else
        WRITE_KAPS_IBC_COMMAND_DATAr(unit, data_reg, tmp);
#endif
#ifdef JER_KAPS_XPT_PRINTS_ENABLED
        LOG_CLI((BSL_META_U(0,"data_reg%d: 0x%08X \n"), data_reg, tmp));
#endif
        data_reg++;
    }

    for (k = 0; k < nbytes/4; k++) {
        tmp = 0;
        for(j=0; j<4; j++) {
            val = bytes[i];
            tmp |= val << (3-j)*8;
            i++;
        }
        /* write to data register */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
        {
            soc_reg_above_64_val_t above_64_val;
            above_64_val[0] = tmp;
            above_64_val[1] = 0;
            ARAD_FAST_REGISER_SET(ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_0 + data_reg, above_64_val);
        }
#else
        WRITE_KAPS_IBC_COMMAND_DATAr(unit, data_reg, tmp);
#endif
#ifdef JER_KAPS_XPT_PRINTS_ENABLED
        LOG_CLI((BSL_META_U(0,"data_reg%d: 0x%08X \n"), data_reg, tmp));
#endif
        data_reg++;
    }

    /* trigger write action by writing to command word register */
    rv = jer_pp_kaps_command_word_write(unit, blk_id, cmd, func, offset);
    if (rv != SOC_SAND_OK) {
        return KBP_INTERNAL_ERROR;
    }

    /* poll response word until response is ready */
    rv = jer_pp_kaps_response_word_poll(unit, blk_id, JER_KAPS_XPT_RESPONSE_VALID, func, JER_KAPS_RESPONSE_STATUS_AOK);
    if (rv != SOC_SAND_OK) {
        return KBP_INTERNAL_ERROR;
    }

    /* zero response word to dequeue response fifo */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
    {
        soc_reg_above_64_val_t above_64_val;
        above_64_val[0] = 0;
        above_64_val[1] = 0;
        ARAD_FAST_REGISER_SET(ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_WORD, above_64_val);
    }
#else
    rv = WRITE_KAPS_IBC_RESPONSE_WORDr(unit, 0);
#endif
    if (rv != SOC_SAND_OK) {
        return KBP_INTERNAL_ERROR;
    }

#ifdef JER_KAPS_XPT_PRINTS_ENABLED
    LOG_CLI((BSL_META_U(0,"%s() end\n"),FUNCTION_NAME()));
#endif

    return rv;

}

kbp_status jer_pp_kaps_read_command(void *xpt,  
                                           uint32 blk_id, 
                                           uint32 cmd,
                                           uint32 func,
                                           uint32 offset,
                                           uint32 n_result_bytes, 
                                           uint8 *result_bytes)
{
    int rv, unit;
    uint32 tmp, val,i =0,j,k;
    JER_KAPS_DATA_REGS data_reg; 

    unit = ((JER_KAPS_XPT*)xpt)->jer_data.unit;

    /* polling command word to validate it's ready */
    rv = jer_pp_kaps_command_word_poll(unit, 0);
    if (rv != SOC_SAND_OK) {
        return KBP_INTERNAL_ERROR;
    }

    /* trigger read action by writing to command word register */
    rv = jer_pp_kaps_command_word_write(unit, blk_id, cmd, func, offset);
    if (rv != SOC_SAND_OK) {
        return KBP_INTERNAL_ERROR;
    }

    /* poll response word until response is ready */
    rv = jer_pp_kaps_response_word_poll(unit, blk_id, JER_KAPS_XPT_RESPONSE_VALID, func, JER_KAPS_RESPONSE_STATUS_AOK);
    if (rv != SOC_SAND_OK) {
        return KBP_INTERNAL_ERROR;
    }

    /* copy response data to buffer */
    memset(&tmp, 0 , sizeof(uint32));
    data_reg = DATA_LAST - SOC_SAND_DIV_ROUND_UP(n_result_bytes,4);

    if (n_result_bytes%4 > 0) {
        /* read from response data register */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
        {
            soc_reg_above_64_val_t above_64_val;
            above_64_val[0] = 0;
            above_64_val[1] = 0;
            ARAD_FAST_REGISER_GET(ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_0 + data_reg, above_64_val);
            val = above_64_val[0];
        }
#else
        READ_KAPS_IBC_RESPONSE_DATAr(unit, data_reg, &val);
#endif
        for (i = 0; i < n_result_bytes%4; i++) {
            result_bytes[i] = val >> ((n_result_bytes%4-1-i)*8);
        }
        /* decrement response data register */
        data_reg++;
    }

    for (k = 0; k < n_result_bytes/4; k++) {
        /* read from response data register */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
        {
            soc_reg_above_64_val_t above_64_val;
            above_64_val[0] = 0;
            above_64_val[1] = 0;
            ARAD_FAST_REGISER_GET(ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_0 + data_reg, above_64_val);
            val = above_64_val[0];
        }
#else
        READ_KAPS_IBC_RESPONSE_DATAr(unit, data_reg, &val);
#endif
        for(j=0; j<4; j++) {
            result_bytes[i] = val >> (3-j)*8;
            i++;
        }
        /* write to data register */
        data_reg++;
    }

    /* zero response word to dequeue response fifo */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
    {
        soc_reg_above_64_val_t above_64_val;
        above_64_val[0] = tmp;
        above_64_val[1] = 0;
        ARAD_FAST_REGISER_SET(ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_WORD, above_64_val);
    }
#else
    rv = WRITE_KAPS_IBC_RESPONSE_WORDr(unit, 0);
#endif
    if (rv != SOC_SAND_OK) {
        return KBP_INTERNAL_ERROR;
    }

#ifdef JER_KAPS_XPT_PRINTS_ENABLED
    LOG_CLI((BSL_META_U(0, "%s()\n"), FUNCTION_NAME()));
#endif

    return rv;
}

uint32 jer_pp_xpt_extended_command(void *xpt,  
                                   uint32 blk_id, 
                                   uint32 cmd,
                                   uint32 func,
                                   uint32 offset)
{
    int rv, unit = ((JER_KAPS_XPT*)xpt)->jer_data.unit;


    SOCDNX_INIT_FUNC_DEFS;

    /* polling command word to validate it's ready */
    rv = jer_pp_kaps_command_word_poll(unit, 0);
    SOCDNX_IF_ERR_EXIT(rv);

    /* trigger enumeration command by writing to command word register */
    rv = jer_pp_kaps_command_word_write(unit, blk_id, cmd, func, offset);
    SOCDNX_IF_ERR_EXIT(rv);

    /* poll response word until response is ready. */
    rv = jer_pp_kaps_response_word_poll(unit, JER_KAPS_XPT_NOF_BLOCKS, JER_KAPS_XPT_RESPONSE_EXTENDED_VALID, func, JER_KAPS_RESPONSE_STATUS_AOK);
    SOCDNX_IF_ERR_EXIT(rv);

    /* zero response word to dequeue response fifo */
#ifdef ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS
    {
        soc_reg_above_64_val_t above_64_val;
        above_64_val[0] = 0;
        above_64_val[1] = 0;
        rv = ARAD_FAST_REGISER_SET(ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_WORD, above_64_val);
    }
#else
    rv = WRITE_KAPS_IBC_RESPONSE_WORDr(unit, 0);
#endif
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC kbp_status jer_pp_kaps_command(void *xpt,
                                      enum kaps_cmd cmd,
                                      enum kaps_func func,
                                      uint32_t blk_nr,
                                      uint32_t row_nr,
                                      uint32_t nbytes,
                                      uint8_t *bytes)
{
    int rv = KBP_OK;

    switch (cmd)
    {
       case KAPS_CMD_READ:
           rv = jer_pp_kaps_read_command(xpt, blk_nr, cmd, func, row_nr, nbytes, bytes);
           break;

       case KAPS_CMD_WRITE:
           rv = jer_pp_kaps_write_command(xpt, blk_nr, cmd, func, row_nr, nbytes, bytes);
           break;

       case KAPS_CMD_EXTENDED:
           if (!SOC_WARM_BOOT(((JER_KAPS_XPT*)xpt)->jer_data.unit)) {
               rv = jer_pp_xpt_extended_command(xpt, blk_nr, cmd, func, row_nr);
           }
           break;

       default:
           LOG_CLI((BSL_META_U(0, "%s():  unsupported cmd: %d\n"), 
                                FUNCTION_NAME(), cmd));
           rv = KBP_INTERNAL_ERROR;
           break;
    }

    return rv;
}

kbp_status kaps_register_read(void *xpt, uint32_t offset, uint32_t nbytes, uint8_t *bytes)
{
    int rv, unit;
    uint32 i, val;
    uint8 array_index;
    soc_reg_t reg;

    unit = ((JER_KAPS_XPT*)xpt)->jer_data.unit;
    reg  = offset_to_register_access_info[offset].reg;
    array_index = offset_to_register_access_info[offset].index;

    rv = soc_reg32_get(unit, reg, REG_PORT_ANY, array_index, &val);
    if (rv != SOC_SAND_OK) {
        return KBP_INTERNAL_ERROR;
    }

    for (i = 0; i < nbytes; i++) {
        bytes[i] = (val >> ((nbytes-1-i)*8)) & 0xff;
    }

#ifdef JER_KAPS_XPT_PRINTS_ENABLED
    LOG_CLI((BSL_META_U(0, "%s()\n"), FUNCTION_NAME()));
#endif

    return rv;
}

kbp_status kaps_register_write(void *xpt, uint32_t offset, uint32_t nbytes, uint8_t *bytes)
{
    int rv, unit;
    uint32 reg_val, val, i;
    uint8 array_index;
    soc_reg_t reg;

    unit = ((JER_KAPS_XPT*)xpt)->jer_data.unit;
    reg  = offset_to_register_access_info[offset].reg;
    array_index = offset_to_register_access_info[offset].index;

    /* set data */
    reg_val = 0;
    for (i = 0; i < nbytes; i++) {
        val = bytes[i];
        reg_val |= val << (nbytes-1-i)*8;
    }

    rv = soc_reg32_set(unit, reg, REG_PORT_ANY, array_index, reg_val);
    if (rv != SOC_SAND_OK) {
        return KBP_INTERNAL_ERROR;
    }

#ifdef JER_KAPS_XPT_PRINTS_ENABLED
    LOG_CLI((BSL_META_U(0,"%s()\n"),FUNCTION_NAME()));
#endif

    return rv;

}

uint32 jer_pp_xpt_init(int unit, void **xpt)
{
    JER_KAPS_XPT *xpt_p;

    SOCDNX_INIT_FUNC_DEFS;

    *xpt = soc_sand_os_malloc(sizeof(JER_KAPS_XPT), "kaps_xpt");
    if (*xpt == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Error, failed to allocate memory for KAPS XPT")));
    }

    xpt_p = (JER_KAPS_XPT*)*xpt;

    xpt_p->jer_kaps_xpt.device_type = KBP_DEVICE_KAPS;
    xpt_p->jer_kaps_xpt.kaps_search = jer_pp_kaps_search;
    xpt_p->jer_kaps_xpt.kaps_register_read = kaps_register_read;
    xpt_p->jer_kaps_xpt.kaps_command = jer_pp_kaps_command;
    xpt_p->jer_kaps_xpt.kaps_register_write = kaps_register_write;
    xpt_p->jer_data.unit = unit;

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 jer_pp_xpt_deinit(int unit, void *xpt)
{
    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_os_free(xpt);

    SOCDNX_FUNC_RETURN;
}

/*
 * KAPS ARM Functions
 */

/* Unhalt KAPS ARM */
uint32 jer_pp_xpt_arm_start(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, KAPS_R_5_CORE_CONTROLr, KAPS_BLOCK(unit), CPU_HALT_Nf, 0x1));

exit:
    SOCDNX_FUNC_RETURN;
}

/* Write to KAPS TCM table according to entry */
uint32 jer_pp_xpt_arm_load_file_entry(int unit, uint32 *input_32, int entry_num)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(WRITE_KAPS_TCMm(unit, KAPS_BLOCK(unit), (entry_num) * 8, input_32));

exit:
    SOCDNX_FUNC_RETURN;
}




/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030) */
