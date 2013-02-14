/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename tr069_exports.h
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
* @created 
*
* @author 
* @end
*
**********************************************************************/

#ifndef __TR069_EXPORTS_H_
#define __TR069_EXPORTS_H_



/* TR-069 defines */
typedef enum
{
  L7_TR069C_STATS_INFORM_COUNT = 0,
  L7_TR069C_STATS_ACS_CONNECTION_REQUEST_COUNT = 1,
  L7_TR069C_STATS_ACS_TOTAL_FAULTS = 2,
  L7_TR069C_STATS_METHOD_UNSUPPORTED_FAULTS = 9000,
  L7_TR069C_STATS_REQUEST_DENIED_FAULTS = 9001,
  L7_TR069C_STATS_INTERNAL_ERROR_FAULTS = 9002,
  L7_TR069C_STATS_INVALID_ARG_FAULTS = 9003,
  L7_TR069C_STATS_RESOURCES_EXCEEDED_FAULTS = 9004,
  L7_TR069C_STATS_INVALID_PARAM_NAME_FAULTS = 9005,
  L7_TR069C_STATS_INVALID_PARAM_TYPE_FAULTS = 9006,
  L7_TR069C_STATS_INVALID_PARAM_VALUE_FAULTS = 9007,
  L7_TR069C_STATS_INVALID_WRITE_ATTEMPT_FAULTS = 9008,
  L7_TR069C_STATS_REQ_REJECT_FAULTS = 9009,
  L7_TR069C_STATS_DOWNLOAD_FAIL_FAULTS = 9010,
  L7_TR069C_STATS_UPLOAD_FAIL_FAULTS = 9011,
  L7_TR069C_STATS_FILE_TRANSFER_SERVER_AUTH_FAIL_FAULTS = 9012,
  L7_TR069C_STATS_VENDOR_DEFAULT_FAULTS = 9013,
  L7_TR069C_STATS_ALL

} L7_TR069C_STATS_TYPE_t;

#define L7_TR069_ACS_URL_LEN     256
#define L7_TR069_ACS_USER_LEN    256
#define L7_TR069_ACS_PASSWD_LEN  256
#define L7_TR069_TIME_STRING_LEN 19
#define L7_TR069_ENCRYPTED_PASSWORD_SIZE  (((L7_TR069_ACS_PASSWD_LEN - 1) *2) + 1) /* + 1 for Null char*/


#define L7_TR069_PERIODIC_INFORM_INTERVAL_MIN   0
#define L7_TR069_PERIODIC_INFORM_INTERVAL_MAX   2592000

#define L7_TR069_ACS_SSLT_PEM_DIR               FD_SSLT_PEM_DIR
#define L7_TR069_ACS_SSLT_ROOT_PEM              FD_TR069_ACS_SSLT_ROOT_PEM
#define L7_TR069_CLIENT_SSL_PRIV_KEY            FD_TR069_CLIENT_SSL_PRIV_KEY
#define L7_TR069_CLIENT_SSL_CERT                FD_TR069_CLIENT_SSL_CERT

/* End of TR-069 Defines */

/******************** conditional Override *****************************/

#ifdef INCLUDE_TR069_EXPORTS_OVERRIDES
#include "tr069_exports_overrides.h"
#endif

#endif /* __TR069_EXPORTS_H_*/
