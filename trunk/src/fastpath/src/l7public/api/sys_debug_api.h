#ifndef SYS_DEBUG_H_
#define SYS_DEBUG_H_
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename    sys_debug.c
*
* @purpose     system debug utilities
*
* @component 
*
* @comments 
*
* @create      10/27/2003
*
* @author      wjacobs
* @end
*
**********************************************************************/


#include "l7_common.h"
#include "datatypes.h"
#include "osapi.h"

/*********************************************************************
* @purpose  This function creates and initializes a trace block
*          
* @param 	 logEntry   @b{(input)}   L7_TRUE, if output is to be logged
*                                     Otherwise, output is printed
*	
* @returns  void
*
* @notes  

*
* @end
*
*********************************************************************/
void timeMillisecondsTrace(L7_BOOL logEntry);

#endif /*SYS_DEBUG_H_*/
