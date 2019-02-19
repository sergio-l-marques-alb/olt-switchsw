/*! \file vlan.c
 *
 * VLAN procedures for DNX.
 *
 * Here add DESCRIPTION.
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
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN
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
#include <bcm/vlan.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm/vlan.h>
/*
 * }
 */

static int
dnx_vlan_create_verify(
  int unit,
  bcm_vlan_t vid)
{
  SHR_FUNC_INIT_VARS(unit);

  SHR_EXIT();

exit:
  SHR_FUNC_EXIT;
}

static int
dnx_vlan_port_add_verify(
  int unit,
  bcm_vlan_t vid,
  bcm_pbmp_t pbmp,
  bcm_pbmp_t ubmp)
{
  SHR_FUNC_INIT_VARS(unit);

  /*
   * Check vid < 4K
   * Check ubmp invalid 
   */

  SHR_EXIT();

exit:
  SHR_FUNC_EXIT;
}

int
bcm_dnx_vlan_create(
  int unit,
  bcm_vlan_t vid)
{
  uint32 entry_handle_id;

  SHR_FUNC_INIT_VARS(unit);

  SHR_INVOKE_VERIFY_DNX(dnx_vlan_create_verify(unit, vid));
  /*
   * DNX SW Algorithm, skip allocation of VSI 
   */
  /*
   * Write to VSI table 
   */
  SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_ING_VSI_INFO, &entry_handle_id));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_VSI, vid));
  /*
   * DBAL internally move the BCM value to DNX-1 
   */
  SHR_IF_ERR_EXIT(dbal_entry_field32_set
                  (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_STP_TOPOLOGY_ID, BCM_STG_DEFAULT));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_MY_MAC_PREFIX, 0));

  /*
   * setting the entry with the default values 
   */
  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

  /*
   * For now skip STG addition and multicast removal 
   */

exit:
  SHR_FUNC_EXIT;
}

int
bcm_dnx_vlan_port_add(
  int unit,
  bcm_vlan_t vid,
  bcm_pbmp_t pbmp,
  bcm_pbmp_t ubmp)
{
  uint32 entry_handle_id;
  int core_id, port_i;
  bcm_pbmp_t current_pbmp, pp_pbmp;
  dnx_algo_gpm_gport_info_t gport_info;

  SHR_FUNC_INIT_VARS(unit);

  SHR_INVOKE_VERIFY_DNX(dnx_vlan_port_add_verify(unit, vid, pbmp, ubmp));
  
  for (core_id = 0; core_id < 2; core_id++)
  {
    BCM_PBMP_CLEAR(current_pbmp);
    BCM_PBMP_CLEAR(pp_pbmp);
    /*
     * convert local port bmp to PP port bmp 
     */
    BCM_PBMP_ITER(pbmp, port_i)
    {
      if (BCM_PBMP_MEMBER(pbmp, port_i))
      {
        /*
         * Get Port + Core 
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_phy_port(unit, port_i, 0, &gport_info));

      }
      if ((gport_info.core_id == core_id))
      {
        BCM_PBMP_PORT_ADD(pp_pbmp, gport_info.internal_port);
      }
    }
    /*
     * Get current core port bmp 
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_INGRESS_VLAN_PBMP, &entry_handle_id));

    /*
     * adding Key 
     */
    SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_VLAN_ID, vid));
    SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_CORE_ID, core_id));
    
    /*
     * SHR_IF_ERR_EXIT(dbal_entry_array32_get(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_PP_PORT_MEMBER, &pp_pbmp.pbits));
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

    /*
     * create updated PP port bmp to set 
     */
    BCM_PBMP_OR(pp_pbmp, current_pbmp);
    /*
     * set PP port bmp 
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_INGRESS_VLAN_PBMP, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_VLAN_ID, vid));
    SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_CORE_ID, core_id));
    SHR_IF_ERR_EXIT(dbal_entry_array32_set
                    (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_PP_PORT_MEMBER, pp_pbmp.pbits));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));
  }

exit:
  SHR_FUNC_EXIT;
}

static int
dnx_vlan_port_create_verify(
  int unit,
  bcm_vlan_port_t * vlan_port)
{
  SHR_FUNC_INIT_VARS(unit);

  SHR_EXIT();

exit:
  SHR_FUNC_EXIT;
}

int
bcm_dnx_vlan_port_create(
  int unit,
  bcm_vlan_port_t * vlan_port)
{
  uint32 entry_handle_id;
  dnx_algo_gpm_gport_info_t gport_info;
  uint32 vlan_domain = 0;
  uint32 ac_profile = 0;
  bcm_gport_t local_in_lif;

  SHR_FUNC_INIT_VARS(unit);

  SHR_INVOKE_VERIFY_DNX(dnx_vlan_port_create_verify(unit, vlan_port));
  /*
   * SW Allocations
   */
  /*
   * DNX SW Algorithm, skip allocation of LIF, requires gport decoding in the future
   */
  local_in_lif = vlan_port->vlan_port_id;

  /*
   * Map Local In-LIF to Global In-LIF  
   */
  SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_IN_AC_INFO_DB, &entry_handle_id));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_IN_LIF,
                                         local_in_lif));
  SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_GLOB_IN_LIF,
                                         vlan_port->vlan_port_id));

  
  SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

  if (vlan_port->criteria == BCM_VLAN_PORT_MATCH_PORT)
  {
    /*
     * Get Port + Core 
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_phy_port(unit, vlan_port->port, 0 /*operations */ , &gport_info));

    /*
     * Ingress - Set the 
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_INGRESS_PORT, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_PP_PORT,
                                           gport_info.internal_port));
    SHR_IF_ERR_EXIT(dbal_entry_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_CORE_ID,
                                           gport_info.core_id));
    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

    /*
     * Egress
     */
    /*
     * Match critieria is port 
     */
    /*
     * Add entry to tables: PORT -> AC_Profile and AC_Profile -> AC info 
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_phy_port(unit, vlan_port->port, 0 /*operations */ , &gport_info));

    /*
     * DNX SW Algorithm, skip allocation of AC profile 
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_EGRESS_DEFAULT_AC_PROF, &entry_handle_id));

    /*
     * Write to EGRESS DEFAULT AC PROFILE table 
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_EGRESS_DEFAULT_AC_PROF, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_DEFAULT_AC_PROF, ac_profile));
    SHR_IF_ERR_EXIT(dbal_entry_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_PORT_VID, vlan_port->egress_vlan));

    /*
     * setting the entry with the default values 
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

    /*
     * Write to EGRESS OUT AC table 
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_EGRESS_PORT, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_PP_PORT, gport_info.internal_port));
    SHR_IF_ERR_EXIT(dbal_entry_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_CORE_ID, gport_info.core_id));
    SHR_IF_ERR_EXIT(dbal_entry_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_DEFAULT_AC_PROF, ac_profile));

    /*
     * setting the entry with the default values 
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

  }
  else
  {
    /*
     * Match critieria is not port 
     */

    /*
     * Get Vlan Domain from Port 
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));

    /*
     * Ingress - Add AC lookup entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_IN_AC_S_VLAN_DB, &entry_handle_id));
    
    SHR_IF_ERR_EXIT(dbal_entry_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_IN_PORT, vlan_domain));
    SHR_IF_ERR_EXIT(dbal_entry_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_S_VLAN, vlan_port->match_vlan));
    SHR_IF_ERR_EXIT(dbal_entry_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_IN_LIF, local_in_lif));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));

    /*
     * Egress
     */

    /*
     * Add AC entry to ESEM 
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, vlan_port->port, &vlan_domain));

    /*
     * Write to EGRESS OUT AC table 
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_take(unit, DBAL_TABLE_ESEM_OUT_AC, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_VLAN_DOMAIN, vlan_domain));
    SHR_IF_ERR_EXIT(dbal_entry_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_MODE_IS_KEY, DBAL_FIELD_VSI, vlan_port->vsi));
    SHR_IF_ERR_EXIT(dbal_entry_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_MODE_IS_VALUE, DBAL_FIELD_ENCAP_DEST /*??? */ ,
                     vlan_port->egress_vlan));
    /*
     * setting the entry with the default values 
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_NORMAL));
  }

exit:
  SHR_FUNC_EXIT;
}
