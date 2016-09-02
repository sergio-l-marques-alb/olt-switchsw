/*

 * $Id: init.c,v 1.94 Broadcom SDK $

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
 *
 *   This module calls the initialization routine of each BCM-DNX module.
 *
 * Here add DESCRIPTION.
 */
/*
 * $Copyright:.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

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
#include <bcm_int/dnx/rx/rx.h>
/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/control.h>
#include <bcm_int/common/rx.h>
#include <soc/drv.h>
/*
 * }
 */
int
bcm_dnx_init(
  int unit)
{
  SHR_FUNC_INIT_VARS(unit);
  SHR_IF_ERR_EXIT(dbal_init(unit));
  SHR_IF_ERR_EXIT(_bcm_common_rx_init(unit));
  SHR_IF_ERR_EXIT(bcm_dnx_rx_start(unit, NULL));

exit:
  SHR_FUNC_EXIT ;
}

int
dnx_info_get_verify(
  int unit,
  bcm_info_t * info)
{
  SHR_FUNC_INIT_VARS(unit);

  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

exit:
  SHR_FUNC_EXIT ;
}

int
bcm_dnx_info_get(
  int unit,
  bcm_info_t * info)
{
  SHR_FUNC_INIT_VARS(unit);

  SHR_INVOKE_VERIFY_DNX(dnx_info_get_verify(unit, info));

  info->vendor = SOC_PCI_VENDOR(unit);
  info->device = SOC_PCI_DEVICE(unit);
  info->revision = SOC_PCI_REVISION(unit);
  info->capability = 0;

  

exit:
  SHR_FUNC_EXIT ;
}

/*
 * IMPORTANT:
 *   The code here is empty and meaningless.
 *   It will probably need to be rewritten!
 *   See bcm/dpp/init.c (_bcm_petra_attach) as example.
 */
int
_bcm_dnx_attach(
  int unit,
  char *subtype)
{
  int dunit;
  SHR_FUNC_INIT_VARS(unit);

  BCM_CONTROL(unit)->capability |= BCM_CAPA_LOCAL;
  dunit = BCM_CONTROL(unit)->unit;
  if (SOC_UNIT_VALID(dunit))
  {
    BCM_CONTROL(unit)->chip_vendor = SOC_PCI_VENDOR(dunit);
    BCM_CONTROL(unit)->chip_device = SOC_PCI_DEVICE(dunit);
    BCM_CONTROL(unit)->chip_revision = SOC_PCI_REVISION(dunit);
    BCM_CONTROL(unit)->capability |= BCM_CAPA_SWITCH;
  }

  SHR_EXIT();
exit:
  SHR_FUNC_EXIT ;
}

int
_bcm_dnx_detach(
  int unit)
{
  int r_rv;
  r_rv = _SHR_E_UNAVAIL;
  return r_rv;
}

int
_bcm_dnx_match(
  int unit,
  char *subtype_a,
  char *subtype_b)
{
  int r_rv;
  r_rv = _SHR_E_UNAVAIL;
  return r_rv;
}
