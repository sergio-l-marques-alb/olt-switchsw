/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename macal_exports.h
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

#ifndef __MACAL_EXPORTS_H_
#define __MACAL_EXPORTS_H_

#define MACAL_MAX_NUM_RULES 64

/* Management Acess Control and Administration List (MACAL) rule fields */
typedef enum
{
  MACAL_SERVICE = (1 << 0),
  MACAL_SRCIP = (1 << 1),
  MACAL_SRCIP_MASK = (1 << 2),
  MACAL_VLAN = (1 << 3),
  MACAL_IF = (1 << 4)
} macalRuleFields_t;

/******************** conditional Override *****************************/

#ifdef INCLUDE_MACAL_EXPORTS_OVERRIDES
#include "macal_exports_overrides.h"
#endif

#endif /* __MACAL_EXPORTS_H_*/
