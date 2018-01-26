/*
 * $Id: dnxc_fabric_source_routed_cell.c,v 1.3 Broadcom SDK $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/drv.h>
#include <soc/dnxc/legacy/dnxc_fabric_source_routed_cell.h>
#include <soc/dnxc/legacy/dnxc_error.h>
#include <shared/shrextend/shrextend_debug.h>


/*
* Function:
*      soc_dnxc_actual_entity_value
* Purpose:
*      Convert type dnxc_fabric_device_type_t to type soc_dnxc_device_type_actual_value_t
* Parameters:
*      device_entity  - (IN)  Value to translate
*      actual_entity  - (IN)  Translated value
* Returns:
*      _SHR_E_XXX
*/
shr_error_e
soc_dnxc_actual_entity_value(
                            int unit,
                            dnxc_fabric_device_type_t            device_entity,
                            soc_dnxc_device_type_actual_value_t* actual_entity
                            )
{
    SHR_FUNC_INIT_VARS(unit);

    switch(device_entity)
    {
    case dnxcFabricDeviceTypeFE1:
        {
            *actual_entity = soc_dnxc_device_type_actual_value_FE1;
            break;
        }
    case dnxcFabricDeviceTypeFE2:
        {
            *actual_entity = soc_dnxc_device_type_actual_value_FE2;
            break;
        }
    case dnxcFabricDeviceTypeFE3:
        {
            *actual_entity = soc_dnxc_device_type_actual_value_FE3;
            break;
        }
    case dnxcFabricDeviceTypeFAP:
        {
            *actual_entity = soc_dnxc_device_type_actual_value_FAP;
            break;
        }
    case dnxcFabricDeviceTypeFOP:
        {
            *actual_entity = soc_dnxc_device_type_actual_value_FOP;
            break;
        }
    case dnxcFabricDeviceTypeFIP:
        {
            *actual_entity = soc_dnxc_device_type_actual_value_FIP;
            break;
        }
    case dnxcFabricDeviceTypeUnknown:
    case dnxcFabricDeviceTypeFE13:
        {
            /*
            * In the context of cells, there is no.
            * FE13 or Unknown entity.
            */
            SHR_ERR_EXIT(_SHR_E_PARAM, "device_entity %d can't be FE13 or unknown",device_entity);
            break;
        }
    default:
        {
            /*
            * This is the case of bad use of the method.
            * (Input value is out of range)
            */
            SHR_ERR_EXIT(_SHR_E_PARAM, "device_entity %d is out-of-range",device_entity);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
* Function:
*      soc_dnxc_actual_entity_value
* Purpose:
*      Convert type dnxc_fabric_device_type_t to type soc_dnxc_device_type_actual_value_t
* Parameters:
*      device_entity  - (IN)  Value to translate
*      actual_entity  - (IN)  Translated value
* Returns:
*      _SHR_E_XXX
*/
shr_error_e
soc_dnxc_device_entity_value(
                            int unit,
                            soc_dnxc_device_type_actual_value_t actual_entity,
                            dnxc_fabric_device_type_t*          device_entity

                            )
{
    SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;

    switch(actual_entity)
    {
    case soc_dnxc_device_type_actual_value_FE1:
        {
            *device_entity = dnxcFabricDeviceTypeFE1;
            break;
        }
    case soc_dnxc_device_type_actual_value_FE2:
    case soc_dnxc_device_type_actual_value_FE2_1:
        {
            *device_entity = dnxcFabricDeviceTypeFE2;
            break;
        }
    case soc_dnxc_device_type_actual_value_FE3:
        {
            *device_entity = dnxcFabricDeviceTypeFE3;
            break;
        }
    case soc_dnxc_device_type_actual_value_FAP:
    case soc_dnxc_device_type_actual_value_FAP_1:
        {
            *device_entity = dnxcFabricDeviceTypeFAP;
            break;
        }
    case soc_dnxc_device_type_actual_value_FOP:
        {
            *device_entity = dnxcFabricDeviceTypeFOP;
            break;
        }
    case soc_dnxc_device_type_actual_value_FIP:
        {
            *device_entity = dnxcFabricDeviceTypeFIP;
            break;
        }
    default:
        {
            /*
            * This is the case of bad use of the method.
            * (Input value is out of range)
            */
            SHR_ERR_EXIT(_SHR_E_PARAM, "actual_entity %d is out-of-range",actual_entity);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
