/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     sshd_util.h
*
* @purpose      SSH configuration header
*
* @component    sshd
*
* @comments     none
*
* @create       05/01/2005
*
* @author       dfowler
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_SSHD_UTIL_H
#define INCLUDE_SSHD_UTIL_H

#include <openssl/evp.h>
#include "l7_common.h"
#include "sshd_api.h"
#include "key.h"

/* Utility function prototypes */

L7_RC_t sshdKeyFileGet(sshdKeyType_t  type,
                       L7_char8      *filename);

L7_RC_t sshdKeyFileCreate(sshdKeyType_t type,
                          L7_uint32     length);

L7_RC_t sshdPubKeyGet(sshdKeyType_t type,
                      Key         **pkey);

L7_RC_t sshdKeyRSA1Get(Key **key);

L7_RC_t sshdPubKeyMD5HexPrint(Key        *pkey,
                              L7_char8   *bp);

L7_RC_t sshdPubKeySHA1BBPrint(Key        *pkey,
                              L7_char8   *bp);

L7_RC_t sshdUserPubKeyGet(L7_char8     *username,
                          sshdKeyType_t type,
                          Key         *pkey);

L7_RC_t sshdUserPubKeyStringFormat(L7_char8 *key,
                                   sshdKeyType_t type,
                                   L7_char8 *keyData);

L7_RC_t sshdUserPubKeyStringVerify(L7_char8     *key,
                                   sshdKeyType_t type);

L7_RC_t sshdUserPubKeyIndexGet(L7_char8        *username,
                               sshdKeyType_t    type,
                               L7_uint32       *index);
#endif /* INCLUDE_SSHD_UTIL_H */

