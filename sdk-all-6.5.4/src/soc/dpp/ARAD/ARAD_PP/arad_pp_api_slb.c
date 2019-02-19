/* $Id: $
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
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_SLB

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_slb.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_slb.h>

#ifdef BCM_88660_A0
#define PPD_API_SLB_INTERNAL_STD_IMPL(suffix, args) \
{ \
  uint32 res = SOC_SAND_OK; \
  SOC_SAND_INIT_ERROR_DEFINITIONS(0); \
  SOC_SAND_CHECK_DRIVER_AND_DEVICE; \
  res = arad_pp_api_impl_ ## suffix ## _verify args; \
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); \
  SOC_SAND_TAKE_DEVICE_SEMAPHORE; \
  res = arad_pp_api_impl_ ## suffix ## _unsafe args; \
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore); \
exit_semaphore: \
  SOC_SAND_GIVE_DEVICE_SEMAPHORE; \
  ARAD_PP_DO_NOTHING_AND_EXIT; \
exit: \
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0); \
}
#else 
#define PPD_API_SLB_INTERNAL_STD_IMPL(suffix, args) \
{ \
  uint32 res = SOC_SAND_OK; \
  SOC_SAND_INIT_ERROR_DEFINITIONS(0); \
  SOC_SAND_CHECK_DRIVER_AND_DEVICE; \
  SOC_SAND_SET_ERROR_CODE(0, 10, exit); \
  ARAD_PP_DO_NOTHING_AND_EXIT; \
exit: \
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0); \
}
#endif /* BCM_88660_A0 */

#define PPD_API_SLB_INTERNAL_PREFIX(suffix) arad_pp_api_impl_ ## suffix
#define PPD_API_SLB_INTERNAL_FUNCTIONS_ONLY

/* Add arad_pp_api implementation. */
#include <soc/dpp/PPD/ppd_api_slb.h>

#undef PPD_API_SLB_INTERNAL_FUNCTIONS_ONLY
#undef PPD_API_SLB_INTERNAL_PREFIX
#undef PPD_API_SLB_INTERNAL_STD_IMPL


#include <soc/dpp/SAND/Utils/sand_footer.h>

