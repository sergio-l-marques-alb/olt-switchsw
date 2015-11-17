/*
 * $Id: debug.c,v 1.9 Broadcom SDK $
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
 *
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_DIAG
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/debug.h>
#include <soc/dpp/drv.h>

#ifdef BCM_PETRAB_SUPPORT
#include <soc/dpp/Petra/petra_debug.h>
#endif /*BCM_PETRAB_SUPPORT*/

#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/arad_debug.h>
#endif /* BCM_ARAD_SUPPORT */

soc_error_t soc_dpp_dbg_egress_shaping_enable_get(const unsigned int unit, uint8 *enable)
{
#if defined(BCM_PETRAB_SUPPORT) || defined(BCM_ARAD_SUPPORT)
  int rv = SOC_E_NONE;
#endif
#ifdef BCM_PETRAB_SUPPORT
  
  if(SOC_IS_PETRAB(unit))
  {
    rv = soc_petra_dbg_egress_shaping_enable_get_unsafe(unit, enable);
    SOCDNX_SAND_IF_ERR_RETURN(rv);
    return SOC_E_NONE;
  }
#endif /* BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT

  if(SOC_IS_ARAD(unit))
  {
    rv = arad_dbg_egress_shaping_enable_get_unsafe(unit, enable);
    SOCDNX_SAND_IF_ERR_RETURN(rv);
    return SOC_E_NONE;
  }
#endif /* BCM_ARAD_SUPPORT */
  return SOC_E_UNAVAIL;
}

soc_error_t soc_dpp_dbg_egress_shaping_enable_set(const unsigned int unit, const uint8 enable)
{
#if defined(BCM_PETRAB_SUPPORT) || defined(BCM_ARAD_SUPPORT)
  int rv = SOC_E_NONE;
#endif
#ifdef BCM_PETRAB_SUPPORT
  
  if(SOC_IS_PETRAB(unit))
  {
    rv = soc_petra_dbg_egress_shaping_enable_set_unsafe(unit, enable);
    SOCDNX_SAND_IF_ERR_RETURN(rv);
    return SOC_E_NONE;
  }
#endif /* BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
  if(SOC_IS_ARAD(unit))
  {
    rv = arad_dbg_egress_shaping_enable_set_unsafe(unit, enable);
    SOCDNX_SAND_IF_ERR_RETURN(rv);
    return SOC_E_NONE;
  }
#endif /* BCM_ARAD_SUPPORT */
  return SOC_E_UNAVAIL;
}

soc_error_t soc_dpp_dbg_flow_control_enable_get(const unsigned int unit, uint8 *enable)
{
#if defined(BCM_PETRAB_SUPPORT) || defined(BCM_ARAD_SUPPORT)
  int rv = SOC_E_NONE;
#endif
#ifdef BCM_PETRAB_SUPPORT
  
  if(SOC_IS_PETRAB(unit))
  {
    rv = soc_petra_dbg_flow_control_enable_get_unsafe(unit, enable);
    SOCDNX_SAND_IF_ERR_RETURN(rv);
    return SOC_E_NONE;
  }
#endif /* BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
  if(SOC_IS_ARAD(unit))
  {
    rv = arad_dbg_flow_control_enable_get_unsafe(unit, enable);
    SOCDNX_SAND_IF_ERR_RETURN(rv);
    return SOC_E_NONE;
  }
#endif /* BCM_ARAD_SUPPORT */
  return SOC_E_UNAVAIL;
}

soc_error_t soc_dpp_dbg_flow_control_enable_set(const unsigned int unit, const uint8 enable)
{
#if defined(BCM_PETRAB_SUPPORT) || defined(BCM_ARAD_SUPPORT)
  int rv = SOC_E_NONE;
#endif
#ifdef BCM_PETRAB_SUPPORT
  
  if(SOC_IS_PETRAB(unit))
  {
    rv = soc_petra_dbg_flow_control_enable_set_unsafe(unit, enable);
    SOCDNX_SAND_IF_ERR_RETURN(rv);
    return SOC_E_NONE;
  }
#endif /* BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
  if(SOC_IS_ARAD(unit))
  {
    rv = arad_dbg_flow_control_enable_set_unsafe(unit, enable);
    SOCDNX_SAND_IF_ERR_RETURN(rv);
    return SOC_E_NONE;
  }
#endif /* BCM_ARAD_SUPPORT */
  return SOC_E_UNAVAIL;
}

soc_error_t soc_dpp_compilation_vendor_valid(const unsigned int unit, const unsigned int val)
{

    return SOC_E_UNAVAIL;
}

#undef _ERR_MSG_MODULE_NAME

