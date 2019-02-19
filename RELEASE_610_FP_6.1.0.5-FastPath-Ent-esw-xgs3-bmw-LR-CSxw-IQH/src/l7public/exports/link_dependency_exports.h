/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename link_dependency_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 03/26/2008
*
* @author akulkarn    
* @end
*
**********************************************************************/

#ifndef __LINK_DEPENDENCY_EXPORTS_H_
#define __LINK_DEPENDENCY_EXPORTS_H_


/* Link Dependency Limits */
#define L7_LINK_DEPENDENCY_MAX_GROUPS   16




/******************** conditional Override *****************************/

#ifdef INCLUDE_LINK_DEPENDENCY_EXPORTS_OVERRIDES
#include "link_dependency_exports_overrides.h"
#endif

#endif /* __LINK_DEPENDENCY_EXPORTS_H_*/
