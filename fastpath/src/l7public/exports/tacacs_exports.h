/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename tacacs_exports.h
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

#ifndef __TACACS_EXPORTS_H_
#define __TACACS_EXPORTS_H_


/*----------------------------------*/
/*  TACACS+ Public Constants        */
/*----------------------------------*/

#define L7_TACACS_MAX_SERVERS             5
#define L7_TACACS_MAX_AUTH_SESSIONS       10

#define L7_TACACS_USER_NAME_MAX           L7_LOGIN_SIZE
#define L7_TACACS_PASSWORD_MAX            L7_PASSWORD_SIZE
#define L7_TACACS_PORT_NUM_MIN            0
#define L7_TACACS_PORT_NUM_MAX            65535
#define L7_TACACS_TIMEOUT_MIN             1
#define L7_TACACS_TIMEOUT_MAX             30
#define L7_TACACS_KEY_LEN_MIN             0
#define L7_TACACS_KEY_LEN_MAX             128
#define L7_TACACS_PRIORITY_MIN            0
#define L7_TACACS_PRIORITY_MAX            65535



/******************** conditional Override *****************************/

#ifdef INCLUDE_TACACS_EXPORTS_OVERRIDES
#include "tacacs_exports_overrides.h"
#endif

#endif /* __TACACS_EXPORTS_H_*/
