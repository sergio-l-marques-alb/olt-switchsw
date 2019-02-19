/* $Id: jer_pp_frwrd_ipv4.c $
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_L3

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dcmn/error.h>
#include <soc/dcmn/utils.h>
#include <shared/bsl.h>
#include <soc/dpp/JER/JER_PP/jer_pp_frwrd_ipv4.h>
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>
#endif
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

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC
  uint32
    jer_pp_frwrd_ipv4_mc_bridge_kaps_dbal_add(
      SOC_SAND_IN  int                               unit,
      SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_MC_ROUTE_KEY      *route_key,
      SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_MC_ROUTE_INFO     *route_info,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE             *success
    )
{
    uint32 payload, dest_val = 0;
    soc_error_t rv;
    ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_VALS_CLEAR(qual_vals);
    DBAL_QUAL_VAL_ENCODE_FID(&qual_vals[0], route_key->fid);
    DBAL_QUAL_VAL_ENCODE_IPV4_DIP(&qual_vals[1], route_key->group, route_key->group_prefix_len);
    DBAL_QUAL_VAL_ENCODE_IPV4_SIP(&qual_vals[2], route_key->source.ip_address, route_key->source.prefix_len);

    dest_val += route_info->dest_id.dest_val;
    payload = JER_PP_KAPS_MC_ENCODE(dest_val);

    rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FID_KAPS, qual_vals, 0,  &payload, success);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

soc_error_t
  jer_pp_frwrd_ip_ipmc_ssm_add(
     SOC_SAND_IN  int unit,
     SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
     SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
     SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE* success
   )
{
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(route_key);
    SOCDNX_NULL_CHECK(route_info);
    SOCDNX_NULL_CHECK(success);

    rv = arad_pp_frwrd_ipv4_mc_route_add_verify(
              unit,
              route_key,
              route_info
            );

    SOCDNX_SAND_IF_ERR_EXIT(rv);

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    if(JER_KAPS_ENABLE(unit))
    {
        rv = jer_pp_frwrd_ipv4_mc_bridge_kaps_dbal_add(
                  unit,
                  route_key,
                  route_info,
                  success);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
    } else {
#else
    /*
    * At the moment the only SSM implementation using the DBAL is by KAPS, in case the TCAM
    * SSM or any other SSM implementation will be added, remove the following lines and add here
    */
    {
#endif
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("KAPS isn't available for SSM add entry ")));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_pp_frwrd_ip_ipmc_ssm_delete(
     SOC_SAND_IN int unit,
     SOC_SAND_IN ARAD_PP_FRWRD_IPV4_MC_ROUTE_KEY* route_key)
{
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    soc_error_t rv;
    ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_SAND_SUCCESS_FAILURE             success;

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_VALS_CLEAR(qual_vals);
    DBAL_QUAL_VAL_ENCODE_FID(&qual_vals[0], route_key->fid);
    DBAL_QUAL_VAL_ENCODE_IPV4_DIP(&qual_vals[1], route_key->group, route_key->group_prefix_len);
    DBAL_QUAL_VAL_ENCODE_IPV4_SIP(&qual_vals[2], route_key->source.ip_address, route_key->source.prefix_len);

    if(JER_KAPS_ENABLE(unit))
    {
        rv = jer_pp_kaps_db_enabled(unit, 0 /*private*/);
        SOCDNX_SAND_IF_ERR_EXIT(rv);

        rv = arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FID_KAPS, qual_vals, &success);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
    } else {
#else
    /*
     * At the moment the only SSM implementation using the DBAL is by KAPS, in case the TCAM
     * SSM or any other SSM implementation will be added, remove the following lines and add here
     */
    SOCDNX_INIT_FUNC_DEFS;
    {
#endif
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("KAPS isn't available for SSM delete entry ")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  jer_pp_frwrd_ip_ipmc_ssm_get(
     SOC_SAND_IN  int unit,
     SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
     SOC_SAND_OUT ARAD_PP_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
     SOC_SAND_OUT ARAD_PP_FRWRD_IP_ROUTE_STATUS*    route_status,
     SOC_SAND_OUT uint8* found)
{

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    uint32 payload;
    soc_error_t rv;
    ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_VALS_CLEAR(qual_vals);
    DBAL_QUAL_VAL_ENCODE_FID(&qual_vals[0], route_key->fid);
    DBAL_QUAL_VAL_ENCODE_IPV4_DIP(&qual_vals[1], route_key->group, route_key->group_prefix_len);
    DBAL_QUAL_VAL_ENCODE_IPV4_SIP(&qual_vals[2], route_key->source.ip_address, route_key->source.prefix_len);

    if(JER_KAPS_ENABLE(unit))
    {
        rv = jer_pp_kaps_db_enabled(unit, 0/*private*/);
        SOCDNX_SAND_IF_ERR_EXIT(rv);

        rv = arad_pp_dbal_entry_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FID_KAPS, qual_vals, &payload, 0/*priority*/, NULL/*hit_bit*/, found);
        SOCDNX_SAND_IF_ERR_EXIT(rv);


        if (*found) {
           route_info->dest_id.dest_val = JER_PP_KAPS_MC_DECODE(payload);
           route_info->dest_id.dest_type = SOC_SAND_PP_DEST_MULTICAST;
        }
    } else {
#else
    /*
     * At the moment the only SSM implementation using the DBAL is by KAPS, in case the TCAM
     * SSM or any other SSM implementation will be added, remove the following lines and add here
     */
    SOCDNX_INIT_FUNC_DEFS;
    {
#endif
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("KAPS isn't available for SSM get entry ")));
    }
exit:
    SOCDNX_FUNC_RETURN;
}


/* } */

