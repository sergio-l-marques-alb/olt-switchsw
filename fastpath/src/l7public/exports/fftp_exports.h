/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename fftp_exports.h
*
* @purpose  Defines constants and feature definition 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 16/05/2008
*
* @author aloks    
* @end
*
**********************************************************************/

#ifndef __FFTP_EXPORTS_H_
#define __FFTP_EXPORTS_H_

/*--------------------------------------*/
/*  FFTP constants                      */
/*--------------------------------------*/

#ifndef FFTP_MAX_SEGMENT_SIZE
#define FFTP_MAX_SEGMENT_SIZE   4000
#endif

#define FFTP_MAX_FILENAME 24
#define FFTP_MAX_FILEPATH 30
#define FFTP_ENTIRE_FILE 999999999

/******************** conditional Override *****************************/

#ifdef INCLUDE_FFTP_EXPORTS_OVERRIDES
#include "fftp_exports_overrides.h"
#endif

#endif /* __FFTP_EXPORTS_H_ */
