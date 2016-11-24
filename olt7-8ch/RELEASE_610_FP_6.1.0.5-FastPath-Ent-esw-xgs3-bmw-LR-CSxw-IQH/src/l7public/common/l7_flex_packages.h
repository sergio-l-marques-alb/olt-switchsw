/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  l7_flex_packages.h
*
* @purpose   Common defines, enums and data structures for flex packages

* @component System Common
*
* @comments  none
*
* @create    07/24/2001
*
* @author    bmutz
*
* @end
*
**********************************************************************/



#ifndef _INCLUDE_L7_FLEX_PACKAGES_
#define _INCLUDE_L7_FLEX_PACKAGES_

#include "l7_common.h"



/*********************************************************************
* @purpose  Check to see if a Flex package component is present
*
* @param    L7_uint32   componentId    
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*       
* @end
*********************************************************************/
extern L7_BOOL cnfgrFlexIsComponentPresent( L7_COMPONENT_IDS_t componentId);

/*********************************************************************
* @purpose  Check to see if a Flex package component feature is present
*
* @param    L7_uint32   componentId    
* @param    L7_uint32   featureId    
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    A feature is a specific part of a component, whose availability
*           is sometimes dictated by the capabilities of the underlying 
*           platform.
*       
* @end
*********************************************************************/
extern L7_BOOL cnfgrFlexIsFeaturePresent( L7_COMPONENT_IDS_t componentId, L7_uint32 featureId);

#endif
