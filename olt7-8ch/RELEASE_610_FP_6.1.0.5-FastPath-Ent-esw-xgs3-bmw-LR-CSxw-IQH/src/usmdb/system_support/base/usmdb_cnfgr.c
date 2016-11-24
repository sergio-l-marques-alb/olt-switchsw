/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  usmdb_cnfgr_api.c
*
* @purpose   USMDB API's for the Configurator component
*
* @component cnfgr
*
* @create    02/17/2004
*
* @author    cpverne
*
* @end
**********************************************************************/   

#include <string.h>
#include "l7_common.h"
#include "usmdb_util_api.h"

#include "l7_cnfgr_api.h"
#include "usmdb_cnfgr_api.h"
#include "cfg_change_api.h"
/*********************************************************************
*
* @purpose Verifies that the component id with that index exists
*          
* @param   componentIndex  @b{(input)}  The index of the Component ID
* @param   *componentId    @b{(output)} The corresponding Component ID
*
* @returns L7_RC_t  L7_SUCCESS  if this index exists
* @returns L7_RC_t  L7_FAILURE  if the index does not exist
*
* @notes Component Index is defined as 1-(number of components present)
*
* @end
*
*********************************************************************/
L7_RC_t usmDbComponentIndexGet(L7_uint32 componentIndex, L7_COMPONENT_IDS_t *componentId)
{
  L7_CNFGR_QUERY_DATA_t queryData;
  L7_uint32 cid_list[L7_LAST_COMPONENT_ID-1];

  if (componentIndex >= L7_LAST_COMPONENT_ID-1)
    return L7_FAILURE;

  queryData.type = L7_CNFGR_QRY_TYPE_LIST;
  queryData.args.cid_list = cid_list;
  queryData.request = L7_CNFGR_QRY_RQST_ALL_PRESENT;

  if (cnfgrApiQuery(&queryData) == L7_SUCCESS)
  {
    if (cid_list[componentIndex] != L7_NULL)
    {
      *componentId = cid_list[componentIndex];
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Retrieves the next valid Component index
*          
* @param   componentIndex  @b{(input/output)} The next valid index of the Component ID
* @param   *componentId    @b{(output)}       The corresponding Component ID
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes Component Index is defined as 1-(number of components present)
*
* @end
*
*********************************************************************/
L7_RC_t usmDbComponentIndexNextGet(L7_uint32 *componentIndex, L7_COMPONENT_IDS_t *componentId)
{
  /* check to see if the next index exists */
  *componentIndex = *componentIndex + 1;
  return usmDbComponentIndexGet(*componentIndex, componentId);
}

/*********************************************************************
*
* @purpose Retrieves the name of the Component
*          
* @param   componentId     @b{(input)}  The index of the Component ID
* @param   *componentName  @b{(output)} Storage for the name of the component
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes componentName must be able to hold atleast L7_COMPONENT_NAME_MAX_LEN characters. 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbComponentNameGet(L7_COMPONENT_IDS_t componentId, L7_char8 *componentName)
{
  return cnfgrApiComponentNameGet(componentId, componentName);
}

/*********************************************************************
*
* @purpose Retrieves the abreviation/mnemonic of the Component
*          
* @param   componentId         @b{(input)}  The index of the Component ID
* @param   *componentMnemonic  @b{(output)} Storage for the mnemonic of the component
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes componentName must be able to hold atleast L7_COMPONENT_MNEMONIC_MAX_LEN characters. 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbComponentMnemonicGet(L7_COMPONENT_IDS_t componentId, L7_char8 *componentMnemonic)
{
  L7_CNFGR_QUERY_DATA_t queryData;

  queryData.type = L7_CNFGR_QRY_TYPE_SINGLE;
  queryData.args.cid = componentId;
  queryData.request = L7_CNFGR_QRY_RQST_MNEMONIC;
  queryData.data.name = componentMnemonic;

  return cnfgrApiQuery(&queryData);
}

/*********************************************************************
*
* @purpose Performs a reverse lookup of the Component ID from it's mnemonic
*          
* @param   *componentMnemonic  @b{(input)}  The mnemonic associated with a Component
* @param   *componentId        @b{(output)} The corresponding Component ID
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes Mnemonic comparison is case insensitive.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbComponentIdFromMnemonicGet(L7_char8 *componentMnemonic, L7_COMPONENT_IDS_t *componentId)
{
  L7_CNFGR_QUERY_DATA_t queryData;
  L7_char8 mnemonic[L7_COMPONENT_MNEMONIC_MAX_LEN];
  L7_uint32 cid=0;

  bzero(mnemonic, L7_COMPONENT_MNEMONIC_MAX_LEN);

  queryData.type = L7_CNFGR_QRY_TYPE_SINGLE;
  queryData.request = L7_CNFGR_QRY_RQST_MNEMONIC;
  queryData.data.name = mnemonic;

  for (cid=L7_FIRST_COMPONENT_ID+1; cid<L7_LAST_COMPONENT_ID; cid++)
  {
    queryData.args.cid = cid;
    if (cnfgrApiQuery(&queryData) == L7_SUCCESS)
    {
      if (usmDbStringCaseInsensitiveCompare(componentMnemonic, mnemonic) == L7_SUCCESS)
      {
        *componentId = cid;
        return L7_SUCCESS;
      }
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Performs a reverse lookup of the Index from the Component ID
*          
* @param   componentId      @b{(input)}  The Component ID
* @param   *componentIndex  @b{(output)} The corresponding Component Index
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbComponentIndexFromIdGet(L7_COMPONENT_IDS_t componentId, L7_uint32 *componentIndex)
{
  L7_CNFGR_QUERY_DATA_t queryData;
  L7_uint32 cid_list[L7_LAST_COMPONENT_ID-1];
  L7_uint32 index=0;

  queryData.type = L7_CNFGR_QRY_TYPE_LIST;
  queryData.args.cid_list = cid_list;
  queryData.request = L7_CNFGR_QRY_RQST_ALL_PRESENT;

  if (cnfgrApiQuery(&queryData) == L7_SUCCESS)
  {
    for (index=0; index < (L7_LAST_COMPONENT_ID-1); index++)
    {
      /* return if we reach the end of the list */
      if (cid_list[index] == 0)
        return L7_FAILURE;

      if (cid_list[index] == componentId)
      {
        *componentIndex = index;
        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Retrieves the enable routine of the Component
*          
* @param   componentId      @b{(input)}  The index of the Component ID
* @param   **enable_routine @b{(output)} Storage for the function pointer
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbComponentEnableRoutineGet(L7_COMPONENT_IDS_t componentId, L7_RC_t (**enable_routine)(L7_uint32 mode))
{
  L7_CNFGR_QUERY_DATA_t queryData;

  queryData.type = L7_CNFGR_QRY_TYPE_SINGLE;
  queryData.args.cid = componentId;
  queryData.request = L7_CNFGR_QRY_RQST_ENABLE_ROUTINE;

  if (cnfgrApiQuery(&queryData) == L7_SUCCESS)
  {
    *enable_routine = queryData.data.enable_routine;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose To find out if working on a chassis or a non-chassis platform
*
* @param    @b  
* @param    @b 
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL usmDbIsAChassisPlatform(void)
{
  if(cnfgrIsChassis()==L7_TRUE)
    return L7_TRUE;
  else
    return L7_FALSE;
}
