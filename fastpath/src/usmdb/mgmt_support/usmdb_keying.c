/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename usmdb_keying.c
*
* @purpose Provide interface to provide keying finctionality to the customers
*
* @component keying
*
* @comments Provides functions to check and set function licenses.
*
* @create 18-Feb-2004
*
* @author Suhel Goel
* @end
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "l7_common.h"

#include "comm_mask.h"
#include "sysapi.h"

#include "usmdb_keying_api.h"
#include "l7_keying_api.h"

/*********************************************************************
*
* @purpose Get the status whether the functionality is to be enabled.
*          It also return whether the feature is keyable.
*
* @param L7_uint32 unitIndex   @b{(input)} Unit Index
* @param L7_uint32 componentId @b{(input)} Component Id of the protocol
* @param L7_BOOL   mode        @b{(input)} Status
* @param L7_BOOL   keyable     @b{(input)} Keyable
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFeatureKeyGet(L7_uint32 unitIndex,L7_COMPONENT_IDS_t  componentId, L7_BOOL *mode, L7_BOOL *keyable)
{

  L7_RC_t rc;

  rc = sysapiFeatureKeyGet(componentId, mode, keyable);

  return rc;
}


/*********************************************************************
*
* @purpose Gets the next functionality for which key faeture is activated.
*
* @param L7_uint32  unitIndex            @b{(input)} Unit for this operation
* @param L7_uint32  currectComponentId   @b{(input)} ComponentId of the current component
* @param L7_uint32* nextComponentId      @b{(output)} ComponentId of the next component
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFeatureGetNext( L7_uint32 unitIndex , L7_COMPONENT_IDS_t  currentComponentId ,
                             L7_COMPONENT_IDS_t * nextComponentId )
{

  L7_RC_t rc = L7_FAILURE;

#ifdef L7_CLI_PACKAGE
  rc = featureKeyingGetNext(currentComponentId, nextComponentId);
#endif /* L7_CLI_PACKAGE */

  return rc;

}

/*********************************************************************
*
* @purpose Gets the first functionality present in L7_COMPONENT_IDS_t list.
*
* @param L7_uint32  unitIndex         @b{(input)}  Unit for this operation
* @param L7_uint32* firstComponentId  @b{(output)} First component in the list
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFeatureGetFirst(L7_uint32 unitIndex,L7_COMPONENT_IDS_t *firstComponentId)
{

  L7_RC_t rc = L7_FAILURE;

#ifdef L7_CLI_PACKAGE
  rc = featureKeyingGetFirst(firstComponentId);
#endif /* L7_CLI_PACKAGE */

  return rc;

}

/*********************************************************************
*
* @purpose Check for key validation
*
* @param L7_uint32  unitIndex         @b{(input)}  Unit for this operation
* @param L7_char8   *keyString        @b{(input)}  Key entered
* @param L7_uint32  mode              @b{(input)}  Enable/Disable
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFeatureKeyLicenseValidate(L7_uint32 unit, L7_char8 *keyString, L7_uint32 mode)
{
#ifndef L7_CLI_PACKAGE
  return L7_SUCCESS;
#else
  L7_uint32 maskLoop;
  COMPONENT_MASK_t maskForAdvancedKey;

  if (featureKeyingVendorSpecificValidation(keyString, &maskForAdvancedKey) == L7_SUCCESS)
  {
    for (maskLoop=1; maskLoop < L7_LAST_COMPONENT_ID; maskLoop++)
    {
      if (COMPONENT_ISMASKBITSET(maskForAdvancedKey, maskLoop))
      {
        /* component is set in this mask */
        if (sysapiFeatureKeySet(maskLoop, mode) == L7_SUCCESS)
        {

        }
      }
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
#endif /* L7_CLI_PACKAGE */
}

