/*! \file stg_stp.c
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
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STG
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
#include <bcm/port.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

/*
 * }
 */

static shr_error_e
dnx_stg_stp_set_verify(
  int unit,
  bcm_stg_t stg,
  bcm_port_t port,
  int stp_state)
{
  SHR_FUNC_INIT_VARS(unit);
  SHR_EXIT();

  /*
   * STG Validation 
   */

  /*
   * Port validations 
   */

  /*
   * STP State validations 
   */

exit:
  SHR_FUNC_EXIT;
}

int
bcm_dnx_stg_stp_set(
  int unit,
  bcm_stg_t stg,
  bcm_port_t port,
  int stp_state)
{
  uint32 entry_handle_id;
  dnx_algo_gpm_gport_info_t gport_info;

  SHR_FUNC_INIT_VARS(unit);

  SHR_INVOKE_VERIFY_DNX(dnx_stg_stp_set_verify(unit, stg, port, stp_state));

  /*
   * Get Port + Core 
   */
  SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_phy_port(unit, port, 0 /*operations */ , &gport_info));

  

  /*
   * Write the STP state to the Ingress HW 
   */
  SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_INGRESS_PORT_STP, &entry_handle_id));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_VLAN_ID, gport_info.internal_port)); /* Why is it VLAN and not port */
  SHR_IF_ERR_EXIT(dbal_entry_field32_set
                  (unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_CORE_ID, gport_info.core_id));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_STG_ID, stg));       /* Need to add XML subtraction by 1 */
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_STP_STATE, stp_state));    /* Need to add XML conversion */
  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

  /*
   * Write the STP state to the Egress HW 
   */
  SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_EGRESS_PORT_STP, &entry_handle_id));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_VLAN_ID, gport_info.internal_port)); /* Why is it VLAN and not port */
  SHR_IF_ERR_EXIT(dbal_entry_field32_set
                  (unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_CORE_ID, gport_info.core_id));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_STG_ID, stg));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_STP_STATE, stp_state));    /* Need to add XML conversion */
  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

exit:
  SHR_FUNC_EXIT;
}
