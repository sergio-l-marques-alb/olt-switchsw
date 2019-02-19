#include <soc/mcm/memregs.h>
#if defined(BCM_88675_A0)
/* $Id: arad_multicast_fabric.c,v 1.20 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MULTICAST
/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_multicast_fabric.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_chip_tbls.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/port_sw_db.h>


/* } */

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

/*********************************************************************
*     Configure the Enhanced Fabric Multicast Queue
*     configuration: the fabric multicast queues are defined
*     in a configured range, and the credits are coming to
*     these queues according to a scheduler scheme.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  jer_mult_fabric_enhanced_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                  *queue_range
  )
{
  int
    res;
  uint64
    reg_value; 
  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(queue_range);
  COMPILER_64_SET(reg_value, 0, 0);
  if (core_id != SOC_CORE_ALL && (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
      LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Core %d out of range\n"), core_id));
      SOCDNX_SAND_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (queue_range->start > (SOC_DPP_DEFS_GET(unit, nof_queues) - 1)) {
    LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Queue start %d out of range\n"), queue_range->start));
    SOCDNX_SAND_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (queue_range->end > (SOC_DPP_DEFS_GET(unit, nof_queues) - 1)) {
    LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Queue end %d out of range\n"), queue_range->end));
    SOCDNX_SAND_IF_ERR_EXIT(SOC_E_PARAM);
  }

  if (queue_range->start > queue_range->end) {
      LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Queue start %d is higher than Queue end %d\n"), queue_range->start, queue_range->end));
      SOCDNX_SAND_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (core_id == 0 || core_id == SOC_CORE_ALL) {
      res = READ_ECI_GLOBAL_FMC_0r(unit, &reg_value);
      SOCDNX_IF_ERR_EXIT(res);

      soc_reg64_field32_set(unit, ECI_GLOBAL_FMC_0r, &reg_value, FMC_QNUM_LOW_0f, queue_range->start);
      soc_reg64_field32_set(unit, ECI_GLOBAL_FMC_0r, &reg_value, FMC_QNUM_HIGH_0f, queue_range->end);

      res = WRITE_ECI_GLOBAL_FMC_0r(unit, reg_value);
      SOCDNX_IF_ERR_EXIT(res);
  }
  if (core_id == 1 || core_id == SOC_CORE_ALL) {
      res = READ_ECI_GLOBAL_FMC_1r(unit, &reg_value);
      SOCDNX_IF_ERR_EXIT(res);

      soc_reg64_field32_set(unit, ECI_GLOBAL_FMC_1r, &reg_value, FMC_QNUM_LOW_1f, queue_range->start);
      soc_reg64_field32_set(unit, ECI_GLOBAL_FMC_1r, &reg_value, FMC_QNUM_HIGH_1f, queue_range->end);

      res = WRITE_ECI_GLOBAL_FMC_1r(unit, reg_value);
      SOCDNX_IF_ERR_EXIT(res);
  }

exit:
  SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Configure the Enhanced Fabric Multicast Queue
*     configuration: the fabric multicast queues are defined
*     in a configured range, and the credits are coming to
*     these queues according to a scheduler scheme.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  jer_mult_fabric_enhanced_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_INOUT SOC_SAND_U32_RANGE                *queue_range
  )
{
  int
    res;
  uint64
    reg_value;
  
  SOCDNX_INIT_FUNC_DEFS;
  COMPILER_64_SET(reg_value, 0, 0);
  SOCDNX_NULL_CHECK(queue_range);
  if (core_id != SOC_CORE_ALL && (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
      LOG_ERROR(BSL_LS_SOC_STAT,(BSL_META_U(unit, "Core %d out of range\n"), core_id));
      SOCDNX_SAND_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (core_id == 0 || core_id == SOC_CORE_ALL) {
      res = READ_ECI_GLOBAL_FMC_0r(unit, &reg_value);
      SOCDNX_IF_ERR_EXIT(res);

      queue_range->start = soc_reg64_field32_get(unit, ECI_GLOBAL_FMC_0r, reg_value, FMC_QNUM_LOW_0f);
      queue_range->end = soc_reg64_field32_get(unit, ECI_GLOBAL_FMC_0r, reg_value, FMC_QNUM_HIGH_0f);
  } else if (core_id == 1) {
      res = READ_ECI_GLOBAL_FMC_1r(unit, &reg_value);
      SOCDNX_IF_ERR_EXIT(res);

      queue_range->start = soc_reg64_field32_get(unit, ECI_GLOBAL_FMC_1r, reg_value, FMC_QNUM_LOW_1f);
      queue_range->end = soc_reg64_field32_get(unit, ECI_GLOBAL_FMC_1r, reg_value, FMC_QNUM_HIGH_1f);
  } 
exit:
  SOCDNX_FUNC_RETURN;
}
#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88675_A0) */
