/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename pml_exports.h
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
* @created 07/17/2008
*
* @author colinw
* @end
*
**********************************************************************/

#ifndef __PML_EXPORTS_H_
#define __PML_EXPORTS_H_

#define L7_PML_VIOLATION_TRAPS_SECONDS_MIN 1
#define L7_PML_VIOLATION_TRAPS_SECONDS_MAX 1000000

/******************** conditional Override *****************************/

#ifdef INCLUDE_PML_EXPORTS_OVERRIDES
#include "pml_exports_overrides.h"
#endif


#endif /* __PML_EXPORTS_H_*/


