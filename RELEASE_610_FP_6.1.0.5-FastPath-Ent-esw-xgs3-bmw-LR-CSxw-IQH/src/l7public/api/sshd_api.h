/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     sshd_api.h
*
* @purpose      SSHD API header
*
* @component    sshd
*
* @comments     none
*
* @create       09/15/2003
*
* @author       dcbii
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_SSHD_API_H
#define INCLUDE_SSHD_API_H

#include "commdefs.h"
#include "datatypes.h"

typedef enum {
  SSHD_KEY_TYPE_RSA,
  SSHD_KEY_TYPE_DSA
} sshdKeyType_t;

/*********************************************************************
*
* @purpose  Set the Admin Mode of the SSHD server.
*
* @param    mode @b{(input)} value of new Admin mode setting
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t sshdAdminModeSet(L7_uint32 mode);


/*********************************************************************
*
* @purpose  Get the Admin Mode of the SSHD server.
*
* @param    mode @b{(input)} location to store the Admin mode setting
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t sshdAdminModeGet(L7_uint32 *mode);


/*********************************************************************
*
* @purpose  Find out how many SSH sessions are active
*
* @param    NumSessions @b{(input)} location to set with result
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t sshdNumSessionsGet(L7_uint32 *NumSessions);


/*********************************************************************
*
* @purpose  Set the protocol level of the SSH server to 1, 2, or both.
*
* @param    protoLevel @b{(input)} value of new mode setting
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t sshdProtoLevelSet(L7_uint32 protoLevel);


/*********************************************************************
*
* @purpose  Get the protocol level of the SSH server.
*
* @param    protoLevel @b{(input)} location to store the mode setting
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t sshdProtoLevelGet(L7_uint32 *protoLevel);


/*********************************************************************
*
* @purpose  Set the max number of SSH Sessions
*
* @param    NumSessions @b{(input)} Max number of SSH sessions
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t cliSshdMaxNumSessionsSet(L7_uint32 *NumSessions);


/*********************************************************************
*
* @purpose  Find out the max number of CLI SSH sessions
*
* @param    void
*
* @returns  Max number of sessions allowed for SSH
*
* @comments none
*
* @end
*
*********************************************************************/

L7_uint32 cliSshdMaxNumSessionsGet(void);


/*********************************************************************
*
* @purpose  Set the idle timeout for ssh sessions
*
* @param    timeOut @b{(input)} idle timeout in minutes or 0 for none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t cliSshdIdleTimeoutSet(L7_uint32 *timeOut);


/*********************************************************************
*
* @purpose  Find out the CLI SSH idle timeout
*
* @param    void
*
* @returns  idle timeout (in minutes or 0 for none)
*
* @comments none
*
* @end
*
*********************************************************************/

L7_uint32 cliSshdIdleTimeoutGet(void);

/*********************************************************************
*
* @purpose To Generate ssh server public/private Keys
*
* @param sshdKeyType_t type @b{(input)} RSA or DSA.
*
* @returns L7_SUCCESS if the Key is generated
* @retruns L7_FAILURE if the key is not generated
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdKeyGenerate(sshdKeyType_t type);

/*********************************************************************
*
* @purpose To verify whether a key type exists
*
* @param sshdKeyType_t type @b{(input)} RSA or DSA.
*
* @returns L7_SUCCESS if key type exists
* @retruns L7_FAILURE if key type does not exist
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdKeyExists(sshdKeyType_t type);

/*********************************************************************
*
* @purpose To get key data for a specific key type.
*
* @param sshdKeyType_t type    @b{(input)}   RSA or DSA.
* @param L7_char8     *keyData @b((output))  hex format
*
* @returns L7_SUCCESS keyData returned
* @retruns L7_FAILURE key type does not exist
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdKeyDataGet(sshdKeyType_t type,
                       L7_char8 *keyData);

/*********************************************************************
*
* @purpose To get key fingerprint in hex format for a specific key type.
*
* @param sshdKeyType_t type        @b{(input)}   RSA or DSA.
* @param L7_char8     *fingerprint @b((output))  hex format
*
* @returns L7_SUCCESS fingerprint returned
* @retruns L7_FAILURE key type does not exist
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdKeyFingerprintHexGet(sshdKeyType_t type,
                                 L7_char8 *fingerprint);

/*********************************************************************
*
* @purpose To get key fingerprint in bubble babble format for a specific key type.
*
* @param sshdKeyType_t type        @b{(input)}   RSA or DSA.
* @param L7_char8     *fingerprint @b((output))  bubble babble format.
*
* @returns L7_SUCCESS fingerprint returned
* @retruns L7_FAILURE key type does not exist
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdKeyFingerprintBBGet(sshdKeyType_t type,
                                L7_char8 *fingerprint);

/*********************************************************************
*
* @purpose Set the Port Number to be used for SSH Server.
*
* @param L7_uint32 portNum @b{(input)} port number set by user
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshdPortNumSet(L7_uint32 portNum);

/*********************************************************************
*
* @purpose Get the Port Number configured for the ssh server
*
* @param L7_uint32 portNum @b{(output)} configured port number
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshdPortNumGet(L7_uint32 *portNum);
/*********************************************************************
*
* @purpose Set Public-Key Authentication Mode of the SSHD server.
*
* @param L7_uint32 mode @b{(input)} enable or disable
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t sshdPubKeyAuthModeSet(L7_uint32 mode);

/*********************************************************************
*
* @purpose Add public key configuration for remote user/device.
*
* @param L7_char8     *username @b{(input)} user string name (up to 48 characters).
* @param sshdKeyType_t type     @b{(input)} RSA or DSA.
*
* @returns L7_SUCCESS user added
* @retruns L7_FAILURE pubkey chain table full
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdPubKeyChainUserKeyAdd(L7_char8 *username,
                                  sshdKeyType_t type);

/*********************************************************************
*
* @purpose Remove public key configuration for remote user/device.
*
* @param L7_char8     *username @b{(input)} user string name (up to 48 characters).
* @param sshdKeyType_t type     @b{(input)} RSA or DSA.
*
* @returns L7_SUCCESS user added
* @retruns L7_FAILURE pubkey chain table full
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdPubKeyChainUserKeyRemove(L7_char8 *username,
                                     sshdKeyType_t type);

/*********************************************************************
*
* @purpose Configure public key for remote user/device.
*
* @param L7_char8     *username @b{(input)} user string name (up to 48 characters).
* @param sshdKeyType_t type     @b{(input)} RSA or DSA.
* @param L7_char8     *key      @b{(input)} key data in UU-encoded DER format.
*
* @returns L7_SUCCESS key set
* @retruns L7_FAILURE entry not found, invalid key
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdPubKeyChainUserKeyStringSet(L7_char8 *username,
                                        sshdKeyType_t type,
                                        L7_char8 *key);

/*********************************************************************
*
* @purpose Get next public key configured for remote user/device.
*
* @param L7_char8      *username @b{(input)} user string name (up to 48 characters).
* @param sshdKeyType_t *type     @b{(output)} RSA or DSA.
*
* @returns L7_SUCCESS next entry returned
* @retruns L7_FAILURE last entry
*
* @comments username of "" will return first public key chain table entry.
*
* @end
*
**********************************************************************/
L7_RC_t sshdPubKeyChainUserKeyNextGet(L7_char8 *username,
                                      sshdKeyType_t *type);

/*********************************************************************
*
* @purpose Get public key string for remote user/device.
*
* @param L7_char8     *username @b{(input)} user string name (up to 48 characters).
* @param sshdKeyType_t type     @b{(input)} RSA or DSA.
* @param L7_char8     *keyData  @b{(output)} key data.
*
* @returns L7_SUCCESS key data returned
* @retruns L7_FAILURE entry not found
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdPubKeyChainUserKeyDataGet(L7_char8 *username,
                                      sshdKeyType_t type,
                                      L7_char8 *keyData);

/*********************************************************************
*
* @purpose Get public key string for remote user/device.
*
* @param L7_char8     *username    @b{(input)} user string name (up to 48 characters).
* @param sshdKeyType_t type        @b{(input)} RSA or DSA.
* @param L7_char8     *fingerprint @b{(output)} fingerprint in hex format.
*
* @returns L7_SUCCESS fingerprint returned
* @retruns L7_FAILURE entry not found
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdPubKeyChainUserKeyFingerprintHexGet(L7_char8 *username,
                                                sshdKeyType_t type,
                                                L7_char8 *fingerprint);

/*********************************************************************
*
* @purpose Get public key string for remote user/device.
*
* @param L7_char8     *username    @b{(input)} user string name (up to 48 characters).
* @param sshdKeyType_t type        @b{(input)} RSA or DSA.
* @param L7_char8     *fingerprint @b{(output)} fingerprint in bubble babble format.
*
* @returns L7_SUCCESS fingerprint returned
* @retruns L7_FAILURE entry not found
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdPubKeyChainUserKeyFingerprintBBGet(L7_char8 *username,
                                               sshdKeyType_t type,
                                               L7_char8 *fingerprint);

/*********************************************************************
*
* @purpose Get Public-Key Authentication Mode of the SSHD server.
*
* @param L7_uint32 mode @b{(output)} enable or disable
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshdPubKeyAuthModeGet(L7_uint32 *mode);

#endif /* INCLUDE_SSHD_API_H */
