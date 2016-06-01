/* $Id: jerp_pp_kaps_xpt.c, hagayco Exp $
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
 * $
*/

#include <soc/mem.h>

#if defined(BCM_88680_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD
#include <soc/mem.h>

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_dev_feature_manager.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>

#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_xpt.h>

#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_kaps_xpt.h>

#include <soc/dpp/JER/jer_sbusdma_desc.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define JERP_RPB_FIRST_BLK_ID  1
#define JERP_RPB_LAST_BLK_ID   4
#define JERP_BB_FIRST_BLK_ID   5
#define JERP_BB_LAST_BLK_ID    12
#define JERP_BBS_FIRST_BLK_ID  13
#define JERP_BBS_LAST_BLK_ID   36

#define JERP_NUMELS_BBS        8

#define JERP_JER_MODE_MAX_IDX  1024

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

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

kbp_status jerp_kaps_translate_blk_func_offset_to_mem_reg(int unit,
                                                          uint8     blk_id,
                                                          uint32     func,
                                                          uint32     offset,
                                                          soc_mem_t *mem,
                                                          soc_reg_t *reg,
                                                          uint32    *array_index,
                                                          int       *blk)
{
    uint32 rv = KBP_OK;

    *mem = INVALIDm;
    *reg = INVALIDr;
    *array_index = 0;

    if (blk_id >= JERP_BBS_FIRST_BLK_ID && blk_id <= JERP_BBS_LAST_BLK_ID) {
        *array_index = (blk_id - JERP_BBS_FIRST_BLK_ID) % JERP_NUMELS_BBS;
        *blk = KAPS_BBS_BLOCK(unit, ((blk_id - JERP_BBS_FIRST_BLK_ID) / JERP_NUMELS_BBS));

        switch (func)
        {
           case KAPS_FUNC0:
               if (offset == KAPS_BB_GLOBAL_CONFIG_OFFSET) {
                   *reg = KAPS_BBS_BB_GLOBAL_CONFIGr;
               } else{
                   LOG_CLI((BSL_META_U(0, "%s():  unsupported BB register offset: %d\n"),
                                    FUNCTION_NAME(), offset));
                   rv = KBP_FATAL_TRANSPORT_ERROR;
               }
               break;

            case KAPS_FUNC1:
               *mem = KAPS_BUCKET_MAP_MEMORYm;
               if (!(dcmn_device_block_for_feature(unit, DCMN_LPM_EXTENDED_FEATURE))) {
                   LOG_CLI((BSL_META_U(0, "%s():  BB, unsupported func: %d. KAPS_BUCKET_MAP_MEMORYm is not  supported in Jericho Plus!\n"),
                                        FUNCTION_NAME(), func));
                   rv = KBP_FATAL_TRANSPORT_ERROR;
               }
               break;
            case KAPS_FUNC2:
            case KAPS_FUNC5:
               *mem = KAPS_BBS_BUCKET_MEMORYm;
               if (dcmn_device_block_for_feature(unit, DCMN_LPM_EXTENDED_FEATURE) && (*array_index >= JERP_JER_MODE_MAX_IDX)) {
                   rv = KBP_FATAL_TRANSPORT_ERROR;
               }
               break;

            default:
               LOG_CLI((BSL_META_U(0, "%s():  BB, unsupported func: %d\n"),
                                    FUNCTION_NAME(), func));
               rv = KBP_FATAL_TRANSPORT_ERROR;
               break;
        }
    } else if (blk_id >= JERP_BB_FIRST_BLK_ID && blk_id <= JERP_BB_LAST_BLK_ID) {
        *array_index = blk_id - JERP_BB_FIRST_BLK_ID;
        *blk = KAPS_BLOCK(unit);
        switch (func)
        {
           case KAPS_FUNC0:
               if (offset == KAPS_BB_GLOBAL_CONFIG_OFFSET) {
                   *reg = KAPS_BB_GLOBAL_CONFIGr;
               } else{
                   LOG_CLI((BSL_META_U(0, "%s():  unsupported BB register offset: %d\n"),
                                    FUNCTION_NAME(), offset));
                   rv = KBP_FATAL_TRANSPORT_ERROR;
               }
               break;

            case KAPS_FUNC1:
               *mem = KAPS_BUCKET_MAP_MEMORYm;
               if (!(dcmn_device_block_for_feature(unit, DCMN_LPM_EXTENDED_FEATURE))) {
                   LOG_CLI((BSL_META_U(0, "%s():  BB, unsupported func: %d. KAPS_BUCKET_MAP_MEMORYm is not  supported in Jericho Plus!\n"),
                                        FUNCTION_NAME(), func));
                   rv = KBP_FATAL_TRANSPORT_ERROR;
               }
               break;
            case KAPS_FUNC2:
            case KAPS_FUNC5:
               *mem = KAPS_BUCKET_MEMORYm;
               if (dcmn_device_block_for_feature(unit, DCMN_LPM_EXTENDED_FEATURE) && (*array_index >= JERP_JER_MODE_MAX_IDX)) {
                   rv = KBP_FATAL_TRANSPORT_ERROR;
               }
               break;

            default:
               LOG_CLI((BSL_META_U(0, "%s():  BB, unsupported func: %d\n"),
                                    FUNCTION_NAME(), func));
               rv = KBP_FATAL_TRANSPORT_ERROR;
               break;
        }
    } else if (blk_id >= JERP_RPB_FIRST_BLK_ID && blk_id <= JERP_RPB_LAST_BLK_ID) {
        *array_index = blk_id - JERP_RPB_FIRST_BLK_ID;
        *blk = KAPS_BLOCK(unit);
        switch (func)
        {
           case KAPS_FUNC0:
               if (offset == KAPS_RPB_CAM_BIST_CONTROL_OFFSET) {
                   *reg = KAPS_RPB_CAM_BIST_CONTROLr;
               } else if (offset == KAPS_RPB_CAM_BIST_STATUS_OFFSET) {
                   *reg = KAPS_RPB_CAM_BIST_STATUSr;
               } else if (offset == KAPS_RPB_GLOBAL_CONFIG_OFFSET) {
                   *reg = KAPS_RPB_GLOBAL_CONFIGr;
               } else{
                   LOG_CLI((BSL_META_U(0, "%s():  unsupported RPB register offset: %d\n"),
                                    FUNCTION_NAME(), offset));
                   rv = KBP_FATAL_TRANSPORT_ERROR;
               }
               break;

            case KAPS_FUNC1:
               *mem = KAPS_RPB_TCAM_CPU_COMMANDm;
               break;

            case KAPS_FUNC4:
                *mem = KAPS_RPB_ADSm;
                break;

            default:
               LOG_CLI((BSL_META_U(0, "%s():  RPB, unsupported func: %d\n"),
                                    FUNCTION_NAME(), func));
               rv = KBP_FATAL_TRANSPORT_ERROR;
               break;
        }


    } else {
        LOG_CLI((BSL_META_U(0,"%s(), unrecognized blk_id = %d.\n"),FUNCTION_NAME(), blk_id));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    return rv;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* defined(BCM_88680_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030) */
