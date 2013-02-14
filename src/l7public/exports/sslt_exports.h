/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename sslt_exports.h
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

#ifndef __SSLT_EXPORTS_H_
#define __SSLT_EXPORTS_H_


/* SSLT Component Feature List */
typedef enum
{
  L7_SSLT_FEATURE_ID = 0,                   /* general support statement */
  L7_SSLT_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_SSLT_FEATURE_IDS_t;



/*--------------------------------------*/
/*  Start SSLT Public Constants         */
/*--------------------------------------*/

#define L7_SSLT_CERT_NUMBER_ACTIVE        FD_SSLT_CERT_NUMBER_ACTIVE
#define L7_SSLT_CERT_NUMBER_MIN           1
#define L7_SSLT_CERT_NUMBER_MAX           2

#define L7_SSLT_KEY_LENGTH_MIN            512
#define L7_SSLT_KEY_LENGTH_MAX            2048

#define L7_SSLT_SUBJECT_DN_FIELD_MIN      1
#define L7_SSLT_SUBJECT_DN_FIELD_MAX      64
#define L7_SSLT_SUBJECT_DN_COUNTRY_SIZE   2
#define L7_SSLT_SUBJECT_LINE_MAX          256
#define L7_SSLT_PEM_BUFFER_SIZE_MAX       4096  /* max for one cert or one cert request */
#define L7_SSLT_PEM_FILE_SIZE_MAX         8192  /* max for a PEM file, could include cert chain */
#define L7_SSLT_FINGERPRINT_MD5_SIZE_MAX  36    /* MD5 + spaces + NULL byte */
#define L7_SSLT_CERT_DATE_SIZE_MAX        32

#define L7_SSLT_CERT_VALID_DAYS           FD_SSLT_CERT_VALID_DAYS
#define L7_SSLT_CERT_VALID_DAYS_MIN       30
#define L7_SSLT_CERT_VALID_DAYS_MAX       3650
#define L7_SSLT_CERT_X509_VERSION         2     /* X509v3 */
#define L7_SSLT_ISSUER_COMMON_NAME        "self-signed"

#define L7_SSLT_SERVER_CERT_PEM_ONE       FD_SSLT_ROOT_PEM
#define L7_SSLT_SERVER_KEY_PEM_ONE        FD_SSLT_SERVER_PEM
#define L7_SSLT_SERVER_CERT_PEM_TWO       "sslt_cert2.pem" /* not yet used */
#define L7_SSLT_SERVER_KEY_PEM_TWO        "sslt_key2.pem"  /* not yet used */


#define L7_SSLT_ADMIN_MODE           FD_SSLT_ADMIN_MODE

#define L7_SSLT_PEM_PASS_PHRASE      FD_SSLT_PEM_PASS_PHRASE
#define L7_SSLT_PEM_PASS_PHRASE_MAX  64 + 1

#define L7_SSLT_SECURE_PORT          FD_SSLT_SECURE_PORT
#define L7_SSLT_SECURE_PORT_MIN      1
#define L7_SSLT_SECURE_PORT_MAX      65535

#define L7_SSLT_UNSECURE_PORT        FD_SSLT_UNSECURE_PORT
#define L7_SSLT_UNSECURE_PORT_MIN    1
#define L7_SSLT_UNSECURE_PORT_MAX    65535

#define L7_SSLT_UNSECURE_SERVER_ADDR FD_SSLT_UNSECURE_SERVER_ADDR

/* PEM-encoded certificate files */
#define L7_SSLT_PEM_DIR              FD_SSLT_PEM_DIR
#define L7_SSLT_ROOT_PEM             FD_SSLT_ROOT_PEM
#define L7_SSLT_SERVER_PEM           FD_SSLT_SERVER_PEM
#define L7_SSLT_DHWEAK_PEM           FD_SSLT_DHWEAK_PEM
#define L7_SSLT_DHSTRONG_PEM         FD_SSLT_DHSTRONG_PEM

/* Configurable SSL protocol Mode's */
#define L7_SSLT_SSL30_MODE           FD_SSLT_SSL30_MODE
#define L7_SSLT_TLS10_MODE           FD_SSLT_TLS10_MODE

/* Configurable SSL protocol ID's */
typedef enum
{
  L7_SSLT_PROTOCOL_SSL30 = 0,
  L7_SSLT_PROTOCOL_TLS10,

} L7_SSLT_PROTOCOL_t;

/*------------------------------------*/
/*  End SSLT Public Constants         */
/*------------------------------------*/



/******************** conditional Override *****************************/

#ifdef INCLUDE_SSLT_EXPORTS_OVERRIDES
#include "sslt_exports_overrides.h"
#endif


#endif /* __SSLT_EXPORTS_H_*/
