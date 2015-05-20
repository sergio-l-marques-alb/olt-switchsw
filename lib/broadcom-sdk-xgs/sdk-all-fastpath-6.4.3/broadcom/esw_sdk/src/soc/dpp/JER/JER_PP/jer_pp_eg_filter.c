
 
/* $Id: arad_pp_api_eg_encap.c,v 1.22 Broadcom SDK $
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
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/JER/JER_PP/jer_pp_eg_filter.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

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

int 
soc_jer_pp_network_group_config_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32 source_network_group_id,
    SOC_SAND_IN uint32 dest_network_group_id, 
    SOC_SAND_IN uint32 is_filter)
{
    int res;
    uint32
        relevant_bit = 1 << (source_network_group_id << 2 | dest_network_group_id), /* bit to turn on or off */
        val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* 
     * the register maps {inlif.orientation(2b), outlif orientation(2b)} to filter (1) or unfilter (0) 
     */
    res = READ_EPNI_CFG_SPLIT_HORIZON_FILTERr(unit, REG_PORT_ANY, &val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (is_filter){
        val |= relevant_bit;
    } else {
        val &= ~relevant_bit;
    }
    res = WRITE_EPNI_CFG_SPLIT_HORIZON_FILTERr(unit, SOC_CORE_ALL, val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_pp_network_group_config_set()",0,0);
}

int 
soc_jer_pp_network_group_config_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32 source_network_group_id,
    SOC_SAND_IN uint32 dest_network_group_id,
    SOC_SAND_OUT uint32 *is_filter)
{
    int res;
    uint32
        relevant_bit = 1 << (source_network_group_id << 2 | dest_network_group_id), /* bit to check if on or off */
        val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = READ_EPNI_CFG_SPLIT_HORIZON_FILTERr(unit, REG_PORT_ANY, &val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (val & relevant_bit) {
        *is_filter = 1;
    } else {
        *is_filter = 0;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_pp_network_group_config_get()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
