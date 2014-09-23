/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename pw_scramble_api.h
*
* @purpose Password Scramble Uitlity
*
* @component utility
*
* @comments The Password Scramble utility is used to re-arrange the
*           bits in a password.
*
* @create 11/26/2001
*
* @author kmcdowell
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#ifndef _PW_SCRAMBLE_API_H
#define _PW_SCRAMBLE_API_H

#include <l7_common.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "aes_api.h"
#include "user_manager_exports.h"
#include "user_manager_exports.h"
/* #include <osapi.h> */


/* Begin Function Prototypes */

L7_uint32 cliWebGetLoginUserStatus(L7_uint32 index, L7_uint32 *status);
L7_uint32 cliWebGetLoginUserName(L7_uint32 index, L7_char8 *name);
L7_uint32 cliWebSetLoginUserPassword(L7_uint32 index, L7_char8 *password);

/*********************************************************************
*
* @purpose  Scramble the bits of the entered password
*
* @param    *oldPw    pointer to an unscrambled password
* @param    *pw       pointer to the returned, scrambled password
*
* @returns  L7_SUCCESS  on success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
pwScramble(L7_char8 *oldPw, L7_char8 *pw);


/*********************************************************************
*
* @purpose  Scramble the bits of the entered password
*
* @param    *oldPw    pointer to an unscrambled password
* @param    *pw       pointer to the returned, scrambled password
* @param    length    (input) number of bytes to scramble
*
* @returns  L7_SUCCESS  on success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
pwScrambleLength(L7_char8 *oldPw, L7_char8 *pw, L7_uint32 length);

/*********************************************************************
*
* @purpose  Unscramble the bits of the entered password
*
* @param    *oldPw    pointer to a scrambled password
* @param    *pw       pointer to the returned, unscrambled password
*
* @returns  L7_SUCCESS  on success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
pwUnscramble(L7_char8 *oldPw, L7_char8 *pw);


/*********************************************************************
*
* @purpose  Unscramble the bits of the entered password
*
* @param    *oldPw    pointer to a scrambled password
* @param    *pw       pointer to the returned, unscrambled password
* @param    length    (input) number of bytes to unscramble
*
* @returns  L7_SUCCESS  on success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
pwUnscrambleLength(L7_char8 *oldPw, L7_char8 *pw, L7_uint32 length);

/*********************************************************************
*
* @purpose  Encrypt entered password
*
* @param    *password       (input) pointer to an encrypted password
* @param    *encryptedPw    (output) pointer to the returned, encrypted password
* @param    length          (input) length of the source password
* @param    alg             (input)  encryption algorithm
*
* @returns  L7_SUCCESS  on success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pwEncrypt(L7_char8 *password, L7_char8 *encryptedPw, L7_uint32 length, 
                  L7_PASSWORD_ENCRYPT_ALG_t alg);

/*********************************************************************
*
* @purpose  Decrypt encrypted password
*
* @param    *password       (output) pointer to an buffer for decrypted password
* @param    *encryptedPw    (input) pointer to the returned, encrypted password
* @param    length          (input) length of the destination password
*
* @returns  L7_SUCCESS  on success
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pwDecrypt(L7_char8 *password, L7_char8 *encryptedPw, L7_uint32 length);
/* End Function Prototypes */
#endif /* _DLL_API_H */
