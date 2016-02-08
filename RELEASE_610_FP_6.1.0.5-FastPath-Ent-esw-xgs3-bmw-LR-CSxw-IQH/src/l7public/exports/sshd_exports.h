/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename sshd_exports.h
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

#ifndef __SSHD_EXPORTS_H_
#define __SSHD_EXPORTS_H_

/* SSHD Component Feature List */
typedef enum
{
  L7_SSHD_FEATURE_ID = 0,                   /* general support statement */
  L7_SSHD_SECURE_TRANSFER_FEATURE_ID,       /* SFTP/SCP support statement */
  L7_SSHD_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_SSHD_FEATURE_IDS_t;


/*--------------------------------------*/
/*  Start SSHD Public Constants         */
/*--------------------------------------*/

#define L7_SSHD_ADMIN_MODE              FD_SSHD_ADMIN_MODE
#define L7_SSHD_PROTO_LEVEL             FD_SSHD_PROTO_LEVEL
#define L7_SSHD_PROTO_LEVEL_V2_ONLY     FD_SSHD_PROTO_LEVEL_V2_ONLY
#define L7_SSHD_PROTO_LEVEL_V1_ONLY     FD_SSHD_PROTO_LEVEL_V1_ONLY
#define L7_SSHD_DEFAULT_IDLE_TIMEOUT    FD_SSHD_TIMEOUT
#define L7_SSHD_DEFAULT_MAX_CONNECTIONS FD_SSHD_MAX_SESSIONS
#define L7_SSHD_EMWEB_PORT              FD_SSHD_EMWEB_PORT
#define L7_SSHC_EMWEB_PORT              FD_SSHC_EMWEB_PORT
#define L7_SSHD_UNSECURE_SERVER_ADDR    FD_SSHD_UNSECURE_SERVER_ADDR
#define L7_SSHD_PORT                    FD_SSHD_PORT
#define L7_SSHD_PUBKEY_AUTH_MODE        FD_SSHD_PUBKEY_AUTH_MODE     

#define L7_SSHD_PATH_SSH_USER_DIR    FD_SSHD_PATH_SSH_USER_DIR

#define L7_SSHD_NAME_SERVER_PRIVKEY        FD_SSHD_NAME_SERVER_PRIVKEY
#define L7_SSHD_NAME_SERVER_PRIVKEY_FRESSH FD_SSHD_NAME_SERVER_PRIVKEY_FRESSH
#define L7_SSHD_NAME_SERVER_PRIVKEY_DSA    FD_SSHD_NAME_SERVER_PRIVKEY_DSA
#define L7_SSHD_NAME_SERVER_PRIVKEY_RSA    FD_SSHD_NAME_SERVER_PRIVKEY_RSA

#define L7_SSHD_PATH_SERVER_PRIVKEY        FD_SSHD_PATH_SERVER_PRIVKEY
#define L7_SSHD_PATH_SERVER_PRIVKEY_FRESSH FD_SSHD_PATH_SERVER_PRIVKEY_FRESSH
#define L7_SSHD_PATH_SERVER_PRIVKEY_DSA    FD_SSHD_PATH_SERVER_PRIVKEY_DSA
#define L7_SSHD_PATH_SERVER_PRIVKEY_RSA    FD_SSHD_PATH_SERVER_PRIVKEY_RSA

#define L7_SSHD_KEY_LENGTH                          1024
#define L7_SSHD_KEY_DATA_SIZE_MAX                   8192
#define L7_SSHD_KEY_FINGERPRINT_SIZE_MAX            128
#define L7_SSHD_PORT_NUMBER_MIN                     1
#define L7_SSHD_PORT_NUMBER_MAX                     65535
#define L7_SSHD_PUBKEY_USERS_MAX                    5
#define L7_SSHD_USERNAME_SIZE_MIN                   1
#define L7_SSHD_USERNAME_SIZE_MAX                   48


/*--------------------------------------*/
/*  Start SSHD Public Constants         */
/*--------------------------------------*/

#define L7_SSHC_USERNAME_SIZE_MAX            L7_LOGIN_SIZE
#define L7_SSHC_PASSWORD_SIZE_MAX            L7_PASSWORD_SIZE

/*------------------------------------*/
/*  End SSHD Public Constants         */
/*------------------------------------*/



/******************** conditional Override *****************************/

#ifdef INCLUDE_SSHD_EXPORTS_OVERRIDES
#include "sshd_exports_overrides.h"
#endif


#endif /* __SSHD_EXPORTS_H_*/

