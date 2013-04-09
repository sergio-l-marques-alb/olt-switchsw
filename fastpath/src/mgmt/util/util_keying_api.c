/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2004-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/util/util_keying_api.c
 * @purpose   Utility APIs to implement Keying for Advanced Functions
 * @component keying
 * @comments  none
 * @create    02/17/2004
 * @author    Suhel Goel
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_base_common.h"
#include "string.h"
#include "stdio.h"
#include "comm_mask.h"
#include "l7_keying_api.h"
#include "util_keying.h"
#include "sysapi.h"

/*********************************************************************
*
* @purpose Get the next valid functionality in the list after CompId
*
* @param L7_uint32  currentComponentId   @b{(input)}  Current Component Id
* @param L7_uint32* nextComponentId      @b{(output)} Next Component Id
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t featureKeyingGetNext(L7_COMPONENT_IDS_t currentComponentId, L7_COMPONENT_IDS_t * nextComponentId)
{
  ComponentMask * FeatureKeyMask;
  ComponentMask FeatureMaskNext;
  *nextComponentId = 0;

  /* Get the feature key mask for the protocols */
  sysapiFeatureKeyMaskGet(&FeatureKeyMask);

  memcpy(&FeatureMaskNext, FeatureKeyMask, COMPONENT_INDICES);

  /* Shift left all bits in the mask by the value of component id.
     The zeroeth bit in the mask corresponds to component id 1 */
  featureKeyingMaskFillLeftMostBits(&FeatureMaskNext, currentComponentId);
  /*  Find the next component id */
  featureKeyingMaskFLMaskBit(&FeatureMaskNext, nextComponentId);

  if (*nextComponentId == L7_LAST_COMPONENT_ID)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the first valid functionality in the list
*
* @param L7_uint32* nextComponentId      @b{(output)} first Component Id
*
* @returns L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t featureKeyingGetFirst(L7_COMPONENT_IDS_t * nextComponentId)
{
  L7_uint32 initComponentId = 0;

  if (featureKeyingGetNext(initComponentId, nextComponentId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Validate the key entered
*
* @param L7_char8         *keyString               @b{(input)}  Entered key
* @param COMPONENT_MASK_t *maskForAdvancedKey      @b{(output)} Mask for keyable protocols
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t featureKeyingVendorSpecificValidation(L7_char8 * keyString, COMPONENT_MASK_t * maskForAdvancedKey)
{
  COMPONENT_MASK_t vendorMaskForAdvancedKey;
  L7_RC_t rc = L7_SUCCESS;

  memset((void *)&vendorMaskForAdvancedKey,0,COMPONENT_INDICES);
  if (strcmp(keyString, pStrInfo_base_000000000001) == 0)
  {
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_VRRP_MAP_COMPONENT_ID);
  }
  else if (strcmp(keyString, pStrInfo_base_000000000002) == 0)
  {
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_OSPF_MAP_COMPONENT_ID);
  }
  else if (strcmp(keyString, pStrInfo_base_000000000003) == 0)
  {
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_FLEX_BGP_MAP_COMPONENT_ID);
  }
  else if (strcmp(keyString, pStrInfo_base_000000000004) == 0)
  {
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_FLEX_PIMDM_MAP_COMPONENT_ID);
  }
  else if (strcmp(keyString, pStrInfo_base_000000000005) == 0)
  {
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  }
  else if (strcmp(keyString, pStrInfo_base_000000000006) == 0)
  {
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_FLEX_DVMRP_MAP_COMPONENT_ID);
  }
  else if (strcmp(keyString, pStrInfo_base_000000000007) == 0)
  {
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_OSPF_MAP_COMPONENT_ID);
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_VRRP_MAP_COMPONENT_ID);
  }
  else if (strcmp(keyString, pStrInfo_base_000000000008) == 0)
  {
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_FLEX_PIMDM_MAP_COMPONENT_ID);
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_FLEX_BGP_MAP_COMPONENT_ID);
  }
  else if (strcmp(keyString, pStrInfo_base_111111111111) == 0)
  {
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_VRRP_MAP_COMPONENT_ID);
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_OSPF_MAP_COMPONENT_ID);
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_FLEX_BGP_MAP_COMPONENT_ID);
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_FLEX_PIMDM_MAP_COMPONENT_ID);
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_FLEX_PIMSM_MAP_COMPONENT_ID);
    COMPONENT_SETMASKBIT(vendorMaskForAdvancedKey, L7_FLEX_DVMRP_MAP_COMPONENT_ID);
  }
  else
  {
    rc = L7_FAILURE;
  }

  memcpy(maskForAdvancedKey, &vendorMaskForAdvancedKey, sizeof(vendorMaskForAdvancedKey));

  return rc;
}
