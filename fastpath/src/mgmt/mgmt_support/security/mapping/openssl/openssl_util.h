/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename openssl_util.h
*
* @purpose Open SSL Tunnel Utilities header
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
#ifndef INCLUDE_OPENSSL_UTIL_H
#define INCLUDE_OPENSSL_UTIL_H

#include <openssl/ssl.h>

/*
** Internal function prototypes
*/

L7_RC_t opensslSemaSetup(void);

void opensslSemaCleanup(void);

L7_RC_t opensslDebugLevelSet(L7_uint32 level);

/*********************************************************************
*
* @purpose  Read DH params from PEM file
*
* @param    L7_char8  *file  @b{(input)}
* @param    DH       **dh    @b((output))  diffie hellman parameters
*
*
* @returns  L7_SUCCESS, file written
* @returns  L7_FAILURE, failed to read pem file
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t opensslDiffieHellmanParamsRead(L7_char8 *file, DH **dh);

#endif /* INCLUDE_OPENSSL_UTIL_H */

