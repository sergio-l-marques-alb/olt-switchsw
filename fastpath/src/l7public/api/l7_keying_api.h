/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   l7_keying_api.h
*
* @purpose    Keying Mapping Layer APIs
*
* @component  Keying Mapping Layer
*
* @comments   none
*
* @create     02/13/2004
*
* @author     Suhel Goel
* @end
*
**********************************************************************/

#ifndef _KEYING_MAP_API_H_
#define _KEYING_MAP_API_H_


/*********************************************************************
*
* @purpose Get the next valid functionality in the list after CompId
*
* @param L7_uint32  currentComponentId   @b{(input)}  Current Component Id
* @param L7_uint32* nextComponentId      @b{(output)} Next Component Id
*
* @returns L7_SUCCESS 
*					 L7_FAILURE
*           
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t featureKeyingGetNext( L7_COMPONENT_IDS_t  currentComponentId, L7_COMPONENT_IDS_t  *nextComponentId);



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
L7_RC_t featureKeyingGetFirst(L7_COMPONENT_IDS_t  *nextComponentId);



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
L7_RC_t featureKeyingVendorSpecificValidation(L7_char8 *keyString, COMPONENT_MASK_t *maskForAdvancedKey);


#endif /* _KEYING_MAP_API_H_*/

