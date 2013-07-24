/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename log_exports.h
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

#ifndef __LOG_EXPORTS_H_
#define __LOG_EXPORTS_H_


#define L7_LOG_EMAILALERT_DURATION_IN_MINUTES_MIN  30

#define L7_LOG_EMAILALERT_DURATION_IN_MINUTES_MAX 1440

#define L7_LOG_EMAILALERT_SEVERITY_MIN  0

#define L7_LOG_EMAILALERT_SEVERITY_MAX 7

#define L7_LOG_EMAILALERT_MAIL_SERVER_LEN 255

#define L7_EMAIL_ALERT_EMAIL_MAX_SIZE   255
#define L7_EMAIL_ALERT_SUBJECT_STRING_SIZE 255
#define L7_EMAIL_ALERT_USERNAME_STRING_SIZE 50
#define L7_EMAIL_ALERT_PASSWD_STRING_SIZE 50
#define L7_EMAIL_ALERT_SERVER_ADDRESS_SIZE 64
#define L7_EMAIL_ALERT_SMTP_MAX_ADDRESS_LEN   64       

#define FD_EMAIL_ALERT_DEFAULT_NON_URGENT_SEVERITY  L7_LOG_SEVERITY_WARNING
#define FD_EMAIL_ALERT_DEFAULT_URGENT_SEVERITY  L7_LOG_SEVERITY_ALERT
#define FD_EMAIL_ALERT_DEFAULT_TRAP_SEVERITY  L7_LOG_SEVERITY_INFO
#define FD_EMAIL_ALERT_DEFAULT_STATUS  L7_ADMIN_MODE_DISABLE
#define FD_EMAIL_ALERT_DEFAULT_LOG_DURATION  30*60*1000 
#define FD_EMAIL_ALERT_DEFAULT_FROM_ADDRESS  "switch@broadcom.com"
#define FD_EMAIL_ALERT_SMTP_DEFAULT_PORT  25
#define FD_EMAIL_ALERT_SMTP_DEFAULT_SECURITY_MODE  L7_LOG_EMAIL_ALERT_NONE
#define FD_EMAIL_ALERT_SMTP_DEFAULT_ENTRY_STATUS  L7_EMAIL_ALERT_ENTRY_NOT_IN_USE
#define FD_EMAIL_ALERT_SMTP_DEFAULT_USERID  "admin"
#define FD_EMAIL_ALERT_SMTP_DEFAULT_PASSWD  "admin"

#define L7_EMAIL_ALERT_MAX_PASSWD_SIZE_ALLOWED 16
#define L7_EMAIL_ALERT_MAX_USERID_SIZE_ALLOWED 16
#define L7_EMAIL_ALERT_MIN_USERID_SIZE_ALLOWED 1
#define L7_EMAIL_ALERT_MIN_PASSWD_SIZE_ALLOWED 1


/* LOG Component Feature List */
typedef enum
{
  L7_LOG_FEATURE_ID = 0,            /* general support statement */
  L7_LOG_PERSISTENT_FEATURE_ID,
  L7_LOG_BUFFERED_FEATURE_ID,
  L7_LOG_CONSOLE_FEATURE_ID,
  L7_LOG_SYSLOG_FEATURE_ID,
  L7_LOG_FEATURE_ID_TOTAL           /* total number of enum values */
} L7_LOG_FEATURE_IDS_t;

typedef struct emailAlertingStatsData_s
{
  L7_uint32           noEmailsSent;
  L7_uint32           noEmailFailures;
  L7_uint32           timeSinceLastMail;	
} emailAlertingStatsData_t;

extern emailAlertingStatsData_t emailAlertStats;

/******************** conditional Override *****************************/

#ifdef INCLUDE_LOG_EXPORTS_OVERRIDES
#include "log_exports_overrides.h"
#endif

#endif /* __LOG_EXPORTS_H_*/
