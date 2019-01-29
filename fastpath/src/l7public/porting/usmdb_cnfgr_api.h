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

#ifndef __USMDB_CNFGR_API_H__
#define __USMDB_CNFGR_API_H__

#include "l7_common.h"

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
L7_RC_t usmDbComponentIndexGet(L7_uint32 componentIndex, L7_COMPONENT_IDS_t *componentId);

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
L7_RC_t usmDbComponentIndexNextGet(L7_uint32 *componentIndex, L7_COMPONENT_IDS_t *componentId);

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
L7_RC_t usmDbComponentNameGet(L7_COMPONENT_IDS_t componentId, L7_char8 *componentName);

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
L7_RC_t usmDbComponentMnemonicGet(L7_COMPONENT_IDS_t componentId, L7_char8 *componentMnemonic);

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
L7_RC_t usmDbComponentIdFromMnemonicGet(L7_char8 *componentMnemonic, L7_COMPONENT_IDS_t *componentId);

/*********************************************************************
*
* @purpose Performs a reverse lookup of the Index from the Component ID
*          
* @param   componentId     @b{(input)}  The Component ID
* @param   *componentIndex  @b{(output)} The corresponding Component Index
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbComponentIndexFromIdGet(L7_COMPONENT_IDS_t componentId, L7_uint32 *componentIndex);

/*********************************************************************
*
* @purpose to find out if the platform is a chassis or a non-chassis
*
* @param   @b
*
* @returns L7_TRUE
           L7_FALSE
*
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL usmDbIsAChassisPlatform();

#endif /* __USMDB_CNFGR_API_H__ */
