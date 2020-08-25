/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename openssl_include.h
*
* @purpose SSL Tunnel code, main header file
*
* @component openssl
*
* @comments none
*
* @create 07/15/2003
*
* @author spetriccione
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_OPENSSL_INCLUDE_H
#define INCLUDE_OPENSSL_INCLUDE_H

/*
System Includes
*/
#include <string.h>
#include <unistd.h>
#include <sys/times.h>

#ifdef _L7_OS_VXWORKS_
#include <selectLib.h>
#endif /* _L7_OS_VXWORKS_ */

/*
LVL7 Base Includes
*/
#include "l7_common.h"
#include "defaultconfig.h"
#include "osapi.h"
#include "osapi_support.h"
#include "log.h"
#include "nvstoreapi.h"
#include "sysnet_api.h"
#include "l7_socket.h"

/*
OpenSSL Includes
*/
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>

/*
OPENSSL Local Includes
*/
#include "openssl_api.h"
#include "openssl_util.h"
#include "openssl_cfg.h"
/*#include "openssl_cnfgr.h"*/

#endif /* INCLUDE_OPENSSL_INCLUDE_H */

