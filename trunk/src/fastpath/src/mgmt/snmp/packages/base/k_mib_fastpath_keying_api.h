/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename  k_mib_fastpath_keying_api.h
*
*
* @purpose  Provide interface to keying Private MIB
** @component SNMP
*
* @comments
*
* @create 03/15/2004
*
* @author Suhel Goel
* @end
*
**********************************************************************/

#include <k_private_base.h>
#include <usmdb_keying_api.h>
#include "usmdb_keying_api.h"


/*********************************************************************
* @purpose  Validate the Enable Key
*
* @param    unitIndex            @b{(input)} Unit for this operation
* @param    snmp_buffer          @b{(input)} Key
*
*
* @returns  L7_SUCCESS    Enable Key is valid
* @returns  L7_FAILURE    Invalid Key
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t 
snmpFeatureKeyingEnableValidate(L7_uint32 unitIndex, L7_char8* snmp_buffer)  
{
  if (usmDbFeatureKeyLicenseValidate(unitIndex, snmp_buffer, L7_ENABLE) != L7_FAILURE)
    return L7_SUCCESS;

  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Validate the Disable Key
*
* @param     unitIndex            @b{(input)} Unit for this operation
* @param     snmp_buffer          @b{(input)} Key
*
*
* @returns  L7_SUCCESS  Disable Key is Valid
* @returns  L7_FAILURE  Invalid Key
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t 
snmpFeatureKeyingDisableValidate(L7_uint32 unitIndex, L7_char8* snmp_buffer) 
{
  if (usmDbFeatureKeyLicenseValidate(unitIndex, snmp_buffer, L7_DISABLE) != L7_FAILURE)
    return L7_SUCCESS;

  return L7_FAILURE;

}

/*********************************************************************
 * @purpose  Get the Status of keyable protocol
 *
 * @param    unitIndex                @b{(input)}   Unit for this operation
 * @param    componentId              @b{(input)}   Component Id
 * @param    val                      @b{(output)}  Status
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t
snmpAgentFeatureKeyingStatusGet(L7_uint32 unitIndex, L7_uint32 componentId, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 mode = 0,keyable = 0;

  rc = usmDbFeatureKeyGet(unitIndex, componentId, &mode, &keyable);

  if (rc == L7_SUCCESS)
  {
     if( keyable == L7_TRUE)
     {
        switch (mode)
        {
  	case L7_ENABLE:
	  *val = D_agentFeatureKeyingStatus_enable;
	  break;

	case L7_DISABLE:
	  *val = D_agentFeatureKeyingStatus_disable;
	  break;
	default:
	/* unknown value */
    	rc = L7_FAILURE;
  	break;
        }
     }
   }
   return rc;
	  
}

