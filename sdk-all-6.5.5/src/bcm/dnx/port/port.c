/*! \file port.c
 *
 * PORT procedures for DNX.
 *
 */
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
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
#include <soc/dnx/dbal_api.h>
/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/port/port_pp.h>
/*
 * }
 */

static int
dnx_port_class_set_verify(
  int unit,
  bcm_port_t port,
  bcm_port_class_t pclass,
  uint32 class_id)
{
  SHR_FUNC_INIT_VARS(unit);

  SHR_EXIT();

exit:
  SHR_FUNC_EXIT;
}

static int
dnx_port_untagged_vlan_set_verify(
  int unit,
  bcm_port_t port,
  bcm_vlan_t vid)
{
  SHR_FUNC_INIT_VARS(unit);

  /*
   * Check vid < 4K 
   */

  SHR_EXIT();

exit:
  SHR_FUNC_EXIT;
}

int
bcm_dnx_port_class_set(
  int unit,
  bcm_port_t port,
  bcm_port_class_t pclass,
  uint32 class_id)
{
  SHR_FUNC_INIT_VARS(unit);

  SHR_INVOKE_VERIFY_DNX(dnx_port_class_set_verify(unit, port, pclass, class_id));

  /*
   * Check pclass, for each class call appropriate function 
   */
  /*
   * Note: Do not add code inside the case!!! Only through separate function! 
   */
  switch (pclass)
  {
    case bcmPortClassId:
    {
      /*
       * Set Vlan Domain 
       */
      SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_set(unit, port, class_id));
      break;
    }

    default:
    {
      SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
      SHR_EXIT() ;	    
      break;
    }
  }

exit:
  SHR_FUNC_EXIT;
}

int
bcm_dnx_port_untagged_vlan_set(
  int unit,
  bcm_port_t port,
  bcm_vlan_t vid)
{
  uint32 entry_handle_id;
  dnx_algo_gpm_gport_info_t gport_info;

  SHR_FUNC_INIT_VARS(unit);

  SHR_INVOKE_VERIFY_DNX(dnx_port_untagged_vlan_set_verify(unit, port, vid));

  /*
   * Get Port + Core 
   */
  SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_phy_port(unit, port, 0, &gport_info));

  /*
   * Write to INGRESS_PORT table 
   */
  SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_INGRESS_PORT, &entry_handle_id));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set
                  (unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_PP_PORT, gport_info.internal_port));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set
                  (unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_CORE_ID, gport_info.core_id));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_PORT_VID, vid));
  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

exit:
  SHR_FUNC_EXIT;
}
