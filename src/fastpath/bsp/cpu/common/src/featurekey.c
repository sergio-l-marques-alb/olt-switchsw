/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename  featurekey.c
*
* @purpose   Feature Key routines.
*
*
* @component hw
*
* @create    03/05/2004
*
* @author    paulq
*
* @end
*
*********************************************************************/

#include "string.h"
#include "l7_common.h"
#include "log.h"
#include "sysapi.h"
#include "l7_cnfgr_api.h"

/*
* This array is to be set by, or for, the customer, to select which features are keyable for
* this platform.
*/
static L7_char8 KeyableFeaturesArray[] =
{
  L7_LAST_COMPONENT_ID
};


/*
* This define is to be set by, or for, the customer, to define the length, in BITS, of the algorithm key.
*/
#define FEATURE_KEY_ALGORITHM_LENGTH 32

static ComponentMask FeatureKeyCurrentValue;
static ComponentMask KeyableFeatures;

/*********************************************************************
* @purpose  Load the stored value of FeatureKeyCurrentValue.
*
* @param    none.
*
* @returns  none.
*
* @comments This is where the system integrator should make
*           modifications for loading their feature keys after a
*           reboot.
*
* @end
*********************************************************************/
static void sysapiFeatureKeyRead()
{
  return;
}

/*********************************************************************
* @purpose  Store the value of of the FeatureKeyCurrentValue.
*
* @param    none.
*
* @returns  none.
*
* @comments This is where the system integrator should make
*           modifications for storing their feature keys accross
*           reboots.
*
* @end
*********************************************************************/
static void sysapiFeatureKeyWrite()
{
  return;
}

/*********************************************************************
* @purpose  Initialize the keyable features mask and read out the saved enabled mask.
*
* @param    none.
*
* @returns  none.
*
* @end
*********************************************************************/
void FeatureKeyInit()
{

  L7_int32 idx = 0;

  memset ((void *)&KeyableFeatures, 0, COMPONENT_INDICES);
  memset ((void *)&FeatureKeyCurrentValue, 0, COMPONENT_INDICES);
  while (KeyableFeaturesArray[idx] != L7_LAST_COMPONENT_ID)
  {
    COMPONENT_SETMASKBIT(KeyableFeatures, KeyableFeaturesArray[idx]);
    idx++;
  }

  sysapiFeatureKeyRead();
}


/*********************************************************************
* @purpose  Set a Feature key's value.
*
* @param    CompID        Component ID to set.
* @param    enabled       L7_TRUE or L7_FALSE.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t sysapiFeatureKeySet(L7_COMPONENT_IDS_t CompID, L7_BOOL enabled)
{
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  if (cnfgrApiComponentNameGet(CompID, name) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  if ((enabled != L7_TRUE) && (enabled != L7_FALSE))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "Invalid value passed for parm 'enabled'\n");
    return L7_FAILURE;
  }
  if (CompID >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "Invalid Component ID\n");
    return L7_FAILURE;
  }

  if (COMPONENT_ISMASKBITSET(KeyableFeatures, CompID) == 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "Component %s is not keyable\n", name);
    return L7_FAILURE;
  }

  if (enabled == L7_TRUE)
  {
    COMPONENT_SETMASKBIT(FeatureKeyCurrentValue, CompID);
  }
  else
  {
    COMPONENT_CLRMASKBIT(FeatureKeyCurrentValue, CompID);
  }

  /* Commit feature key to storage */
  sysapiFeatureKeyWrite();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the value of a feature key.
*
* @param    CompID   Component ID to get.
* @param    enabled  ptr to place value of key (L7_TRUE or L7_FALSE).
* @param    keyable  ptr to return status if component ID requested is keyable (L7_TRUE or L7_FALSE).
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if CompID is out of range.
*
* @end
*********************************************************************/
L7_RC_t sysapiFeatureKeyGet(L7_COMPONENT_IDS_t CompID, L7_BOOL *enabled, L7_BOOL *keyable)
{
  if (CompID >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_BSP_COMPONENT_ID,
            "Invalid Component ID\n");
    return L7_FAILURE;
  }

  if (COMPONENT_ISMASKBITSET(KeyableFeatures, CompID) == 0)
    *keyable = L7_FALSE;
  else
    *keyable = L7_TRUE;
  if (COMPONENT_ISMASKBITSET(FeatureKeyCurrentValue, CompID) == 0)
    *enabled = L7_FALSE;
  else
    *enabled = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the address of the static feature array mask.
*
* @param    FeatureKeyMask Ptr to place address of Feature Key Mask.
*
* @returns  none.
*
* @end
*********************************************************************/
void sysapiFeatureKeyMaskGet(ComponentMask **FeatureKeyMask)
{
  *FeatureKeyMask = &KeyableFeatures;
}

/*********************************************************************
* @purpose  Get the length, in bits, of the feature key algorithm
*
* @param    FeatureKeyAlgorithmLen Ptr to place length
*
* @returns  none.
*
* @end
*********************************************************************/
void sysapiFeatureKeyAlgorithmLength(L7_uint32 *FeatureKeyAlgorithmLen)
{
  *FeatureKeyAlgorithmLen = FEATURE_KEY_ALGORITHM_LENGTH;
}

/*********************************************************************
* @purpose  Returns the presence of feature keys
*
* @param    none
*
* @returns  L7_TRUE   if the first key is not L7_LAST_COMPONENT_ID
*           L7_FALSE  if it is
*
* @end
*********************************************************************/
L7_BOOL sysapiFeatureKeyPresent()
{
  if (KeyableFeaturesArray[0] != L7_LAST_COMPONENT_ID)
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}
