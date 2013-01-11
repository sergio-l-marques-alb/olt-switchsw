/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename  usmdb_keying_api.h
*
* @purpose   Provide interface to API's for keying for Advanced Features
*
* @component unitmgr
*
* @comments
*
* @create    02/16/2004
*
* @author    Suhel Goel
*
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#ifndef _USMDB_KEYING_API_H_
#define _USMDB_KEYING_API_H_

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
*		   L7_FAILURE
*           
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFeatureKeyGet(L7_uint32 unitIndex,L7_COMPONENT_IDS_t componentId, L7_BOOL *mode, L7_BOOL *keyable);

/*********************************************************************
*
* @purpose Gets the next functionality for which key faeture is activated.
*
* @param L7_uint32  unitIndex   				 @b{(input)} Unit for this operation
* @param L7_uint32  currectComponentId   @b{(input)} ComponentId of the current component
* @param L7_uint32* nextComponentId      @b{(output)} ComponentId of the next component
*
* @returns L7_SUCCESS 
*	   L7_FAILURE
*           
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFeatureGetNext(L7_uint32 unitIndex,L7_COMPONENT_IDS_t currentComponentId,L7_COMPONENT_IDS_t *nextComponentId);

/*********************************************************************
*
* @purpose Gets the first functionality present in L7_COMPONENT_IDS_t list.
*
* @param L7_uint32  unitIndex         @b{(input)}  Unit for this operation
* @param L7_uint32* firstComponentId  @b{(output)} First component in the list
*
* @returns L7_SUCCESS 
*	   L7_FAILURE
*           
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFeatureGetFirst(L7_uint32 unitIndex, L7_COMPONENT_IDS_t *firstComponentId);

/*********************************************************************
*
* @purpose Check for key validation
*
* @param L7_uint32  unitIndex         @b{(input)}  Unit for this operation
* @param L7_char8   *keyString        @b{(input)}  Key entered
* @param L7_uint32  mode              @b{(input)}  Enable/Disable
*
* @returns L7_SUCCESS 
*					 L7_FAILURE
*           
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFeatureKeyLicenseValidate(L7_uint32 unit, L7_char8 *keyString, L7_uint32 mode);

#endif /* _USMDB_KEYING_API_H_ */


