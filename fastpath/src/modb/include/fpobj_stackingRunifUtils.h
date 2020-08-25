/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_stackingRunifUtils.h
*
* @purpose
*
* @component RUN IF MACROS AND DEFINITIONS
*
*
* @create  16 Mar 2009, Monday
*
* @notes   This file is manually written to support run if's for stacking objects and
*          object handlers. This can me relocated to some other place if the appropriate
*          place holder is defined down the line.
*
* @author  Vijayanand,Kamasamudram
* @end
*
********************************************************************************/

/********** include files needed to support run if definitions ***********/
L7_BOOL usmUtilStackMemberGet(void);

/********************************************************************
*
* @purpose  runif function for display of stack port diags
*
* @returns  runif for stack port diags
* @end
*********************************************************************/
#define STACK_MEMBER_GET_FIRST  usmUtilStackMemberGet


