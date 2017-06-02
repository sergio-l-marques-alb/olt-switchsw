/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename usmdb_sshd.c
*
* @purpose SSHD USMDB API functions
*
* @component sshd
*
* @comments none
*
* @create 07/15/2003
*
* @author dcbii
*
* @end
*
**********************************************************************/
#include "l7_common.h"
#include "usmdb_sshd_api.h"
#include "sshd_api.h"

/*********************************************************************
*
* @purpose Set the Admin Mode of the SSHD server.
*
* @param unitIndex @b((input)) the unit for this operation
* @param mode @b{(input)} value of new Admin mode setting,L7_ENABLE or
*                         L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbsshdAdminModeSet(L7_uint32 unitIndex,
                              L7_uint32 mode)
{
  return sshdAdminModeSet(mode);
}

/*********************************************************************
*
* @purpose Get the Admin Mode of the SSL Tunnel server.
*
* @param unitIndex @b((input)) the unit for this operation
* @param mode @b{(input)} location to store the Admin mode setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbsshdAdminModeGet(L7_uint32 unitIndex,
                              L7_uint32 *mode)
{
  return sshdAdminModeGet(mode);
}

/*********************************************************************
*
* @purpose Get the number of active SSHD sessions.
*
* @param unitIndex @b((input)) the unit for this operation
* @param number @b{(input)} location to store the value of the port number
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbsshdNumSessionsGet(L7_uint32 unitIndex,
                               L7_uint32 *number)
{
  return sshdNumSessionsGet(number);
}

/*********************************************************************
*
* @purpose Set the maximum number of allowed SSHD sessions.
*
* @param unitIndex @b((input)) the unit for this operation
* @param number @b{(input)} location to store the value of max SSHD sessions
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSshdMaxNumSessionsSet(L7_uint32 unitIndex,
                               L7_uint32 *number)
{
    return cliSshdMaxNumSessionsSet(number);
}

/*********************************************************************
*
* @purpose Get the maximum number of allowed SSHD sessions.
*
* @param unitIndex @b((input)) the unit for this operation
* @param number @b{(input)} location to store the value of max SSHD sessions
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/

L7_RC_t usmDbSshdMaxNumSessionsGet(L7_uint32 unitIndex,
                                   L7_uint32 *number)
{
    *number = cliSshdMaxNumSessionsGet();
    return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose Set the Protocol Level of the SSHD server.
*
* @param unitIndex @b((input)) the unit for this operation
* @param protoLevel @b{(input)} value of new mode setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbsshdProtoLevelSet(L7_uint32 unitIndex,
                             L7_uint32 protoLevel)
{
  return sshdProtoLevelSet(protoLevel);
}

/*********************************************************************
*
* @purpose Get the Protocol Level of the SSHD server.
*
* @param unitIndex @b((input)) the unit for this operation
* @param protoLevel @b{(input)} location to store the mode setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbsshdProtoLevelGet(L7_uint32 unitIndex,
                             L7_uint32 *protoLevel)
{
  return sshdProtoLevelGet(protoLevel);
}

/*********************************************************************
*
* @purpose set the idle timeout value for ssh sessions.
*
* @param unitIndex @b((input)) the unit for this operation
* @param protoLevel @b{(input)} location to store the mode setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbsshdTimeoutSet(L7_uint32 unitIndex, L7_uint32 *idleTimeoutValue)
{
    return cliSshdIdleTimeoutSet(idleTimeoutValue);
}
/*********************************************************************
*
* @purpose Get the idle timeout value for ssh sessions.
*
* @param unitIndex @b((input)) the unit for this operation
* @param protoLevel @b{(input)} location to store the mode setting
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbsshdTimeoutGet(L7_uint32 unitIndex, L7_uint32 *idleTimeoutValue)
{
    *idleTimeoutValue = cliSshdIdleTimeoutGet();
    return L7_SUCCESS;
}

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
L7_RC_t usmDbsshdPortNumSet(L7_uint32 portNum)
{
  return sshdPortNumSet(portNum);
}

/*********************************************************************
*
* @purpose Get the Port Number for the ssh server
*
* @param L7_uint32 *portNum @b{(output)} configured port number
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbsshdPortNumGet(L7_uint32 *portNum)
{
  return sshdPortNumGet(portNum);
}

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
L7_RC_t usmDbsshdPubKeyAuthModeSet(L7_uint32 mode)
{
  return sshdPubKeyAuthModeSet(mode);
}

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
L7_RC_t usmDbsshdPubKeyAuthModeGet(L7_uint32 *mode)
{
  return sshdPubKeyAuthModeGet(mode);
}

/*********************************************************************
*
* @purpose generate ssh server public/private key pair
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
L7_RC_t usmDbsshdKeyGenerate(sshdKeyType_t type)
{
  return sshdKeyGenerate(type);
}

/*********************************************************************
*
* @purpose To verify whether a key pair of specified type exists
*
* @param sshdKeyType_t type @b{(input)} RSA or DSA.
*
* @returns L7_SUCCESS if key type exists
* @returns L7_FAILURE invalid argument or key generation in progress
* @returns L7_NOT_EXIST if key type does not exist
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t usmDbsshdKeyExists(sshdKeyType_t type)
{
  return sshdKeyExists(type);
}

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
L7_RC_t usmDbsshdKeyDataGet(sshdKeyType_t type,
                            L7_char8 *keyData)
{
  return sshdKeyDataGet(type, keyData);
}

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
L7_RC_t usmDbsshdKeyFingerprintHexGet(sshdKeyType_t type,
                                      L7_char8 *fingerprint)
{
  return sshdKeyFingerprintHexGet(type, fingerprint);
}

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
L7_RC_t usmDbsshdKeyFingerprintBBGet(sshdKeyType_t type,
                                     L7_char8 *fingerprint)
{
  return sshdKeyFingerprintBBGet(type, fingerprint);
}

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
L7_RC_t usmDbsshdPubKeyChainUserKeyAdd(L7_char8 *username,
                                       sshdKeyType_t type)
{
  return sshdPubKeyChainUserKeyAdd(username, type);
}

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
L7_RC_t usmDbsshdPubKeyChainUserKeyRemove(L7_char8 *username,
                                          sshdKeyType_t type)
{
  return sshdPubKeyChainUserKeyRemove(username, type);
}

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
L7_RC_t usmDbsshdPubKeyChainUserKeyStringSet(L7_char8 *username,
                                             sshdKeyType_t type,
                                             L7_char8 *key)
{
  return sshdPubKeyChainUserKeyStringSet(username, type, key);
}

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
L7_RC_t usmDbsshdPubKeyChainUserKeyNextGet(L7_char8 *username,
                                           sshdKeyType_t *type)
{
  return sshdPubKeyChainUserKeyNextGet(username, type);
}

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
L7_RC_t usmDbsshdPubKeyChainUserKeyDataGet(L7_char8 *username,
                                           sshdKeyType_t type,
                                           L7_char8 *keyData)
{
  return sshdPubKeyChainUserKeyDataGet(username, type, keyData);
}

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
L7_RC_t usmDbsshdPubKeyChainUserKeyFingerprintHexGet(L7_char8 *username,
                                                     sshdKeyType_t type,
                                                     L7_char8 *fingerprint)
{
  return sshdPubKeyChainUserKeyFingerprintHexGet(username, type, fingerprint);
}

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
L7_RC_t usmDbsshdPubKeyChainUserKeyFingerprintBBGet(L7_char8 *username,
                                                    sshdKeyType_t type,
                                                    L7_char8 *fingerprint)
{
  return sshdPubKeyChainUserKeyFingerprintBBGet(username, type, fingerprint);
}
