/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename dot1x_auth_serv_util.h
*
* @purpose Dot1x Auth Server util header file
*
* @component Dot1x Authentication Server
*
* @comments none
*
* @create 11/17/2009
*
* @author Pradeep Kumar
*
* @end
*             
**********************************************************************/

#ifndef DOT1X_AUTH_SERV_UTIL_H
#define DOT1X_AUTH_SERV_UTIL_H

/*********************************************************************
*
* @purpose compare 2 buffers.
*
* @param    *buf1    @b{(input)}  input string1. 
* @param    *buf2    @b{(input)}  input string2. 
*
* @returns  L7_TRUE if buffers are the same.  L7_FALSE otherwise.
*
* @notes If the buffers can not be compared, L7_FALSE is returned.
* @notes The buffers must be null terminated.
*
* @end
*
*********************************************************************/
L7_BOOL dot1xAuthServNoCaseCompare(L7_char8 *buf1, L7_char8 *buf2);

/*********************************************************************
* @purpose  Private utility function to initialize all data structure fields
*
* @param    index    @b{(input)}  table index. 
*
* @returns  void
*
* @end
*********************************************************************/
void dot1xAuthServUserDBInitialize(L7_uint32 index );

#endif
