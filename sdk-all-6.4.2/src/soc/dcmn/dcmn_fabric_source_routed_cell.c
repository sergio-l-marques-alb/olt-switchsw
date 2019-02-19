/*
 * $Id: dcmn_fabric_source_routed_cell.c,v 1.3 Broadcom SDK $
 *
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dcmn/dcmn_fabric_source_routed_cell.h>
#include <soc/dcmn/dcmn_error.h>


/*
* Function:
*      soc_dcmn_actual_entity_value
* Purpose:
*      Convert type dcmn_fabric_device_type_t to type soc_dcmn_device_type_actual_value_t
* Parameters:
*      device_entity  - (IN)  Value to translate
*      actual_entity  - (IN)  Translated value
* Returns:
*      SOC_E_XXX
*/
soc_error_t
soc_dcmn_actual_entity_value(
                            int unit,
                            dcmn_fabric_device_type_t            device_entity,
                            soc_dcmn_device_type_actual_value_t* actual_entity
                            )
{
    SOCDNX_INIT_FUNC_DEFS;

    switch(device_entity)
    {
    case dcmnFabricDeviceTypeFE1:
        {
            *actual_entity = soc_dcmn_device_type_actual_value_FE1;
            break;
        }
    case dcmnFabricDeviceTypeFE2:
        {
            *actual_entity = soc_dcmn_device_type_actual_value_FE2;
            break;
        }
    case dcmnFabricDeviceTypeFE3:
        {
            *actual_entity = soc_dcmn_device_type_actual_value_FE3;
            break;
        }
    case dcmnFabricDeviceTypeFAP:
        {
            *actual_entity = soc_dcmn_device_type_actual_value_FAP;
            break;
        }
    case dcmnFabricDeviceTypeFOP:
        {
            *actual_entity = soc_dcmn_device_type_actual_value_FOP;
            break;
        }
    case dcmnFabricDeviceTypeFIP:
        {
            *actual_entity = soc_dcmn_device_type_actual_value_FIP;
            break;
        }
    case dcmnFabricDeviceTypeUnknown:
    case dcmnFabricDeviceTypeFE13:
        {
            /*
            * In the context of cells, there is no.
            * FE13 or Unknown entity.
            */
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("device_entity %d can't be FE13 or unknown"),device_entity));
            break;
        }
    default:
        {
            /*
            * This is the case of bad use of the method.
            * (Input value is out of range)
            */
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("device_entity %d is out-of-range"),device_entity));
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
* Function:
*      soc_dcmn_actual_entity_value
* Purpose:
*      Convert type dcmn_fabric_device_type_t to type soc_dcmn_device_type_actual_value_t
* Parameters:
*      device_entity  - (IN)  Value to translate
*      actual_entity  - (IN)  Translated value
* Returns:
*      SOC_E_XXX
*/
soc_error_t
soc_dcmn_device_entity_value(
                            int unit,
                            soc_dcmn_device_type_actual_value_t actual_entity,
                            dcmn_fabric_device_type_t*          device_entity

                            )
{
    SOCDNX_INIT_FUNC_DEFS;

    switch(actual_entity)
    {
    case soc_dcmn_device_type_actual_value_FE1:
        {
            *device_entity = dcmnFabricDeviceTypeFE1;
            break;
        }
    case soc_dcmn_device_type_actual_value_FE2:
    case soc_dcmn_device_type_actual_value_FE2_1:
        {
            *device_entity = dcmnFabricDeviceTypeFE2;
            break;
        }
    case soc_dcmn_device_type_actual_value_FE3:
        {
            *device_entity = dcmnFabricDeviceTypeFE3;
            break;
        }
    case soc_dcmn_device_type_actual_value_FAP:
    case soc_dcmn_device_type_actual_value_FAP_1:
        {
            *device_entity = dcmnFabricDeviceTypeFAP;
            break;
        }
    case soc_dcmn_device_type_actual_value_FOP:
        {
            *device_entity = dcmnFabricDeviceTypeFOP;
            break;
        }
    case soc_dcmn_device_type_actual_value_FIP:
        {
            *device_entity = dcmnFabricDeviceTypeFIP;
            break;
        }
    default:
        {
            /*
            * This is the case of bad use of the method.
            * (Input value is out of range)
            */
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("actual_entity %d is out-of-range"),actual_entity));
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
