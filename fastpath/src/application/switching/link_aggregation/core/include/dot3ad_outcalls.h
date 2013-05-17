/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dot3ad_outcalls.c
*
* @purpose    The functions that handle dot3ad outcall requests
*
* @component  dot3ad
*
* @comments   none
*
* @create     05/07/2003
*
* @author     
* @end
*
**********************************************************************/
#ifndef DOT3AD_OUTCALLS_H
#define DOT3AD_OUTCALLS_H

#include "l7_common.h"

/*********************************************************************
* @purpose  Validate whether the interface type can participate in dot3ad
*
* @param    sysIntfType  @b({input}) interface type being checked
*
* @returns  L7_BOOL
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dot3adIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Validate whether the interface can participate in dot3ad
*
* @param    intIfNum  @b({input}) The internal interface number being checked
*
* @returns  L7_BOOL
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL dot3adIsValidIntf(L7_uint32 intIfNum);

#endif
