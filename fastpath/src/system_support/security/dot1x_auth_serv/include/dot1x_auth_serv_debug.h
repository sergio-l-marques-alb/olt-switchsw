/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename dot1x_auth_serv_debug.c
*
* @purpose Dot1x Auth Server debug utilities
*
* @component Dot1x Authentication Server
*
* @comments none
*
* @create 11/17/2009
*
* @author pradeepk
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1X_AUTH_SERV_DEBUG_H
#define INCLUDE_DOT1X_AUTH_SERV_DEBUG_H

/*********************************************************************
 * @purpose  Print the current configured users to
 *           serial port
 *
 * @param    void
 *
 * @returns  L7_SUCCESS or L7_FALIURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dot1xAuthServUserDBShow(void);

#endif
