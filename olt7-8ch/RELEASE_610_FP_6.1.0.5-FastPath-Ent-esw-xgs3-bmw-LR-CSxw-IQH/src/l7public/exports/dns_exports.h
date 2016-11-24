/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename dns_exports.h
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

#ifndef __DNS_EXPORTS_H_
#define __DNS_EXPORTS_H_


/*----------------------------------*/
/*  DNS Client parameter Limits     */
/*----------------------------------*/
#define L7_DNS_DOMAIN_RETRY_NUMBER_MAX     100
#define L7_DNS_DOMAIN_RETRY_NUMBER_MIN     0
#define L7_DNS_DOMAIN_TIMEOUT_MAX          3600
#define L7_DNS_DOMAIN_TIMEOUT_MIN          0
#define L7_DNS_DOMAIN_LIST_NAME_SIZE_MAX   255
#ifndef L7_DNS_DOMAIN_NAME_SIZE_MAX
#define L7_DNS_DOMAIN_NAME_SIZE_MAX       255
#endif
#define L7_DNS_DOMIAN_NAME_LEN_MIN        1
#define L7_DNS_DOMAIN_NAME_LEN_MAX        255
#define L7_DNS_HOST_NAME_LEN_MIN          1
#define L7_DNS_HOST_NAME_LEN_MAX          255

typedef enum
{
  L7_HOST_IP_ADDR = 0,
  L7_HOST_NAME
} L7_HOST_ADDR_TYPE_t;




/*--------------------------------------*/
/*  END DNS Client parameter Limits     */
/*--------------------------------------*/


/******************** conditional Override *****************************/

#ifdef INCLUDE_DNS_EXPORTS_OVERRIDES
#include "dns_exports_overrides.h"
#endif

#endif /* __DNS_EXPORTS_H_*/
