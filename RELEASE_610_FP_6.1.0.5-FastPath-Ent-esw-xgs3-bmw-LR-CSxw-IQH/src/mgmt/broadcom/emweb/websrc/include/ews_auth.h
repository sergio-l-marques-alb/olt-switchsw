/*
 *
 * Product:     EmWeb
 * Release:     R6_2_0
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
 * THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
 * AND OTHER US AND INTERNATIONAL PATENTS PENDING.
 * 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
 *
 * Notice to Users of this Software Product:
 *
 * This software product of Virata Corporation ("Virata"), 5 Clock Tower
 * Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in
 * source and object code format embodies valuable intellectual property
 * including trade secrets, copyrights and patents which are the exclusive
 * proprietary property of Virata. Access, use, reproduction, modification
 * disclsoure and distribution are expressly prohibited unless authorized
 * in writing by Virata.  Under no circumstances may you copy this
 * software or distribute it to anyone else by any means whatsoever except in
 * strict accordance with a license agreement between Virata and your
 * company.  This software is also protected under patent, trademark and
 * copyright laws of the United States and foreign countries, as well as
 * under the license agreement, against any unauthorized copying, modification,
 * or distribution.
 * Please see your supervisor or manager regarding any questions that you may
 * have about your right to use this software product.  Thank you for your
 * cooperation.
 *
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
 * All Rights Reserved
 *
 * Virata Corporation
 * 5 Clock Tower Place
 * Suite 400
 * Maynard, MA  01754
 *
 * Voice: +1 (978) 461-0888
 * Fax:   +1 (978) 461-2080
 *
 * http://www.emweb.com/
 *   support@agranat.com
 *     sales@agranat.com
 *
 * EmWeb/Server application interface to authorization database
 *
 */

#ifndef _EWS_AUTH_H_
#define _EWS_AUTH_H_

#ifdef EW_CONFIG_OPTION_AUTH

#include "ew_types.h"
#include "ew_config.h"

/******************************************************************************
 *
 * HTTP Authorization techniques are discussed in the HTTP specifications.
 * Knowledge of these principles are essential for correct use of these
 * authorization interfaces.
 *
 * One or more HTML documents can be assocated with a single "realm".  A realm
 * is a case-sensitive ascii string that defines a "protection space" for the
 * associated document(s).
 *
 * Each realm can have zero or more authentication entries associated with it.
 * If a realm has no authentication entries, then it is considered
 * "unprotected" by the server, and its acess will always be granted to any
 * requesting client.
 *
 * For those realms with authentication entries, any client that attempts to
 * access any document associated with that realm will be required to
 * authenticate itself.  To authenticate, the client must send authentication
 * parameters that validate against at least one authentication entry for that
 * document's realm.  Clients that do not authenticate are not served the
 * requested document.
 *
 * For example, assume a realm exists called "foo".  It has three
 * authentication entries associated with it (2 using the "basic" scheme
 * defined in the HTTP specification, and one other ficticious type):
 *
 * REALM: "foo"
 *         Authentication Entry 1:
 *              Type: "basic"
 *              parameters: Username="user1"
 *                          Password="guest"
 *              EwaAuthHandle:  <application's entry1 identifier>
 *
 *         Authentication Entry 2:
 *              Type: "future"
 *              parameters: index=37
 *                          cookie="837I8U9"
 *                          token="S37"
 *              EwaAuthHandle:  <application's entry2 identifier>
 *
 *         Authentication Entry 3:
 *              Type: "basic"
 *              parameters: Username="sinclair"
 *                          Password="babylon"
 *              EwaAuthHandle:  <application's entry3 identifier>
 *
 * When a client attempts to access a document associated with realm "foo", it
 * will need to authenticate against one of the above entries.  Which one the
 * client authenticates against is at its disgression.
 *
 * When a client does authenticate against one of the above entries, the
 * EwaAuthHandle for that entry is stored in the current context for the
 * document (EwsContext).  The datatype of this EwaAuthHandle is
 * implementation-defined.
 *****************************************************************************/

/* ================================================================
 * Types & Constants
 * ================================================================ */

/*
 * Defines the authorization schemes supported by the EmWeb server.
 * New schemes will be added as they are supported.
 */
typedef enum
{

#   ifdef EW_CONFIG_OPTION_AUTH_BASIC
    ewsAuthSchemeBasic,
#   endif /* EW_CONFIG_OPTION_AUTH_BASIC */

#   ifdef EW_CONFIG_OPTION_AUTH_DIGEST
    ewsAuthSchemeDigest,
#   endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

#   ifdef EW_CONFIG_OPTION_AUTH_MBASIC
    ewsAuthSchemeManualBasic,
#   endif /* EW_CONFIG_OPTION_AUTH_MBASIC */

    ewsAuthMaxScheme            /* count of supported schemes */

} EwsAuthScheme;


/*
 * this structure defines parameters for all the supported
 * authorization types.  New parameters will be added in
 * to this structure in the future
 */
typedef union
{
#ifdef EW_CONFIG_OPTION_AUTH_BASIC
  struct
  {
        const char  *userName;
        const char  *passWord;
  } basic;
#endif /* EW_CONFIG_OPTION_AUTH_BASIC */
#if defined(EW_CONFIG_OPTION_AUTH_DIGEST)
#if defined(EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE)
  struct
  {
        const char    *userName;
        const char    *md5_secret;
  } digest_change;
#else
  struct
  {
        const char    *userName;
        const char    *passWord;
  } digest;
#endif /* EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE */
#endif /* EW_CONFIG_OPTION_AUTH_DIGEST */
  char reserved1;
} EwsAuthParameters;


/*
 * This structure represents a single authorization entry.
 */
typedef struct
{
  EwsAuthScheme     scheme;
  EwsAuthParameters params;
  EwaAuthHandle     handle;     /* user defined */
} EwsAuthorization;


/* ================================================================
 * Interfaces
 * ================================================================ */


/*
 * ewsAuthRegister
 * And a new authorization entry to the realm.
 *
 * realm - Input, case sensitive asciiz realm name.
 * authorization - Input, buffer containing authorization entry information.
 *                 This buffer is free for use by the caller on return.
 *
 * Returns a non-NULL handle to the authorization entry, or NULL on failure.
 */

extern EwsAuthHandle ewsAuthRegister
  ( const char *realm,const EwsAuthorization *authorization );


/*
 * ewsAuthRemove
 * Deletes a particular authentication entry identified by handle.
 *
 * handle - Input, The handle that was returned by ewsAuthRegister() for
 *          this entry.
 *
 * Returns EWS_STATUS_OK on success, else failure code.
 */
extern EwsStatus ewsAuthRemove ( EwsAuthHandle handle );

/*
 * ewsAuthRealmEnable
 * Enables a realm to restrict access to documents (restricted by default)
 *
 * realm - Input, the realm to restrict
 *
 * Returns EWS_STATUS_OK on success, else failure code
 */
extern EwsStatus ewsAuthRealmEnable ( const char * realm );

/*
 * ewsAuthRealmDisable
 * Disables a realm causing documents in that realm to be generally accessible.
 *
 * realm - Input, the realm to restrict
 *
 * Returns EWS_STATUS_OK on success, else failure code
 */
extern EwsStatus ewsAuthRealmDisable ( const char * realm );

#ifndef EWS_CONFIG_OPTION_AUTH_NO_REALM_QUALIFIER
/*
 * ewaAuthRealmQualifier
 * Returns realm qualifier string to append to realm names, or NULL
 */
#ifndef ewaAuthRealmQualifier
#  ifdef EW_CONFIG_OPTION_REALM_CONTEXT
#    define REALM_QUAL_PARAM EwsContext
#  else
#    define REALM_QUAL_PARAM void
#  endif
extern const char *ewaAuthRealmQualifier ( REALM_QUAL_PARAM );
#endif /* ewaAuthRealmQualifier */
#endif

#ifdef EW_CONFIG_OPTION_AUTH_DOMAIN
/*
 * ewaAuthDigestDomain
 *
 *     input:
 *           context - request context
 *           realm   - null-terminated string containing name of realm,
 *     output:
 *           domainp  - on TRUE return, *domainp will point to a null-terminated
 *                    ascii text string, list of uris.
 */
extern boolean ewaAuthDigestDomain( EwsContext context
                             ,const char *realm
                             ,const char **domainp );
#endif

#if defined(EW_CONFIG_OPTION_AUTH_DIGEST)

/*
 * ewaAuthNonceCreate
 * Create raw input used to generate one-time authentication challenges
 * given a request context and realm.
 *
 * context - request context
 * realm   - null-terminated string containing name of realm
 * noncep  - pointer to application-specific nonce parameters
 *
 * No return value
 */
extern void ewaAuthNonceCreate
  ( EwsContext context, const char * realm, EwaAuthNonce *noncep );

/*
 * ewaAuthNonceCheck
 * Validate that the previously created nonce is valid given the current
 * request.
 *
 * context - request context
 * realm   - null-terminated string containing name of realm
 * noncep  - pointer to requested nonce parameters
 * count   - previous use count (0, 1, ...) of nonce value
 *
 * Returns ewaAuthNonceOK on success, ewaAuthNonceStale if valid but expired,
 * or ewaAuthNonceDenied if nonce is not permitted given the request context.
 */
typedef enum EwaAuthNonceStatus_e
  {
    ewaAuthNonceOK,     /* nonce value valid for request */
    ewaAuthNonceLastOK, /* nonce value valid, but won't be again */
    ewaAuthNonceStale,  /* nonce value is stale, generate new nonce */
    ewaAuthNonceDenied  /* nonce value is invalid */
  } EwaAuthNonceStatus;

extern EwaAuthNonceStatus ewaAuthNonceCheck
  ( EwsContext context, const char * realm, EwaAuthNonce *noncep, uintf count );

extern void ewsAuthHashMD5Password(const char *password, char **digest );

#if defined(EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE)
/*
 * ewsAuthHashMD5
 * Calculate the MD5 secret for input args user, realm and password.
 * Function can be used before calling ewsAuthRegister().
 * user -  (in)username of person registering
 * realm -  (in) realm for registration
 * password - (in)the password being registered.
 * newsecret - (out) - the new secret value calculated
 *
 */
extern void ewsAuthHashMD5 (const char *user,
                            const char *realm,
                            const char *password,
                            char **newsecret);
/*
 * ewaAuthDigestChange
 * Let application know password change has been requested
 *
 * context - request context
 * settin_handle - application authentication handle of password setting user
 * realm - for which password changed
 * user - for whom password changed
 * newsecret - new secret value
 *
 */

extern EwaAuthHandle ewaAuthDigestChange(EwsContext context,
                                         const EwaAuthHandle setting_handle,
                                         const char *realm,
                                         const char *user,
                                         const char *newsecret) ;

/*
 * ewaAuthDigestChange_serve
 * give application a chancw to perform serve form function.
 * Called from EmWeb implemented serve password change form function.
 *
 * context - request context
 * form - pointer to form structure
 *
 */

extern void ewaAuthDigestChange_serve(EwsContext context,
                                      void *form );
/*
 * Called from ewaFormServe_passwordChange with pointers to form's
 * value and status structures
*/
extern void ewsFormServe_passwordChange
  ( EwsContext context
   ,const char **value_userp
   ,const char **value_realmp
   ,const char **value_setting_userp
   ,const char **value_psnoncep
   ,const char **value_hashchkp
   ,const char **value_passwdchkp
   ,const char **value_maskedsecretp
   ,uint8 *status_userp
   ,uint8 *status_realmp
   ,uint8 *status_setting_userp
   ,uint8 *status_psnoncep
   ,uint8 *status_hashchkp
   ,uint8 *status_passwdchkp
   ,uint8 *status_maskedsecretp
  );

/*
 * Called from ewaFormSubmit_passwordChange with pointers to form's
 * value and status structures
 */
extern const char *ewsFormSubmit_passwordChange
  ( EwsContext context
   ,const char **value_userp
   ,const char **value_realmp
   ,const char **value_setting_userp
   ,const char **value_psnoncep
   ,const char **value_hashchkp
   ,const char **value_passwdchkp
   ,const char **value_maskedsecretp
   ,uint8 *status_userp
   ,uint8 *status_realmp
   ,uint8 *status_setting_userp
   ,uint8 *status_psnoncep
   ,uint8 *status_hashchkp
   ,uint8 *status_passwdchkp
   ,uint8 *status_maskedsecretp
 );
#endif
#endif /* EW_CONFIG_OPTION_AUTH_(M)DIGEST */


#ifdef EW_CONFIG_OPTION_AUTH_MBASIC
/*
 * ewaAuthCheckBasic
 * Given the current context, realm, base64 cookie, and optionally
 * decoded username/password text, determine if the authorization should
 * be granted or denied.
 *
 * context - (input) request context
 * realm   - (input) null-terminated string containing name of realm
 * basicCookie - (input) the base64 encoding of the text user:password,
 *           as described in the HTTP RFC.
 * userName - (input) if EW_CONFIG_OPTION_AUTH_MBASIC_DECODE is defined,
 *          then this parameter points to a null terminated string
 *          containing the user name decoded from the basicCookie.  If
 *          EW_CONFIG_OPTION_AUTH_MBASIC_DECODE is not defined, this
 *          is a NULL pointer.
 * password - (input) if EW_CONFIG_OPTION_AUTH_MBASIC_DECODE is defined,
 *          then this parameter points to a null terminated string
 *          containing the password decoded from the basicCookie.  If
 *          EW_CONFIG_OPTION_AUTH_MBASIC_DECODE is not defined, this
 *          is a NULL pointer.
 *
 * Return value: boolean, if TRUE, then the authorization is granted and
 * the document is served.  Otherwise, authorization is denied.
 *
 * This function can be supended by ewsSuspend().  It's return values
 * are ignored, and it will be recalled with the same parameters when
 * the context is resumed using ewsResume().
 */

#ifndef ewaAuthCheckBasic
extern boolean ewaAuthCheckBasic( EwsContext context
                                  ,const char *realm
                                  ,const char *basicCookie
                                  ,const char *userName
                                  ,const char *password );
#endif /* ewaAuthCheckBasic */
#endif /* EW_CONFIG_OPTION_AUTH_MBASIC */


#ifdef EW_CONFIG_OPTION_AUTH_MDIGEST
/*
 * ewaAuthDigestHash
 * Given the current context, realm, username, nonce and cnonce,
 * return the corresponding MD5 hash of the username:realm:password
 * and nonce and cnonce text.  This text will be used by the server
 * to verify the authorization attempt by the client.
 *
 * context - (input) request context
 * realm   - (input) null-terminated string containing name of realm
 *          taken from the request header.
 * userName - (input) this parameter points to a null terminated string
 *          containing the user name as supplied in the authorization
 *          request header.
 * nonce   - (input) this parameter points to a null terminated string
 *          containing the nonce as supplied in the authorization
 *          request header.
 * cnonce   - (input) this parameter points to a null terminated string
 *          containing the client nonce as supplied in the authorization
 *          request header.
 * digest  - (output) on return, *digest will point to a null-terminated
 *          ascii text string representing the MD5 checksum of the
 *          username, realm and password, nonce and cnonce
 *          MD5(MD5(username:realm:password):nonce:cnonce),
 *          as described in the Digest Access Authentication RFC2069
 *
 * Return value: boolean, if TRUE, then *digest points to a valid  hash:
 * MD5(MD5(username:realm:password):nonce:cnonce), which will be used
 * by the server to complete * the authorization process (which may or
 * may not be granted).  If FALSE, * the authorization is immediately denied.
 *
 * This function can be supended by ewsSuspend().  It's return values
 * are ignored, and it will be recalled with the same parameters when
 * the context is resumed using ewsResume().
 */

#ifndef ewaAuthDigestHash
extern boolean ewaAuthDigestHash( EwsContext context
                                  ,const char *realm
                                  ,const char *userName
                                  ,const char *nonce
                                  ,const char *cnonce
                                  ,char **digest );
#endif /* ewaAuthDigestHash */
#endif /* EW_CONFIG_OPTION_AUTH_MDIGEST */

/*
 * ewaAuthDigestSecret()
 *
 * This function, called by server only once at initialization time, provides
 * a 'secret', null terminated string to be used by server to calculate
 * validator part of the challenge nonce.
 *
 *     output:
 *           secretp - pointer to null-terminated 'secret' string.
 */
extern void ewaAuthDigestSecret( char **secret );

#ifdef EW_CONFIG_OPTION_AUTH_VERIFY
/*
 * ewaAuthVerifySecurity
 * Called _after_ the server has determined that the authorization
 * will be granted for the request, this function allows the application
 * to "short circuit" the authorization based on information about
 * the request (eg, IP address of client, etc.).  The method used
 * to determine whether or not the request is authorized is proprietary
 * to the application.
 *
 * context - (input) request context
 * realm   - (input) null-terminated string containing name of realm
 *          taken from the request header.
 *
 * Return value: boolean, if TRUE, then the authorization is granted, and
 * the document will be served to the client.  If FALSE,
 * the authorization is immediately denied.
 *
 * This function can be supended by ewsSuspend().  It's return values
 * are ignored, and it will be recalled with the same parameters when
 * the context is resumed using ewsResume().
 */

#ifndef ewaAuthVerifySecurity
extern boolean ewaAuthVerifySecurity( EwsContext context
                                      ,const char *realm );
#endif /* ewaAuthVerifySecurity */
#endif /* EW_CONFIG_OPTION_AUTH_VERIFY */

#ifdef EW_CONFIG_OPTION_SNMP_AGENT
/**********************************************************************
 *
 *  ewsSNMPAgentAuthHandleSet
 *
 *  Purpose: Application can associate SNMP authentication parameters
 *      with a particular realm.  Can be used to remove this parameters
 *      also.
 *
 *  Inputs:
 *      realm - realm name (string)
 *      handle - application supplied SNMP authentication parameters
 *
 *  Outputs:  none
 *
 *  Returns:
 *      Last value of handle associated with realm, or a null handle
 *      if no handle currently set.
 *
 *  Notes:
 *      1) the realm must exist prior to this call.  The user must
 *      have made at least one call to ewsAuthRegister() prior
 *      to calling this routine.
 *      2) to remove the SNMP auth handle, pass a null handle
 *      as the new handle parameter.
 *      3) ewsAuthShutdown will free any non null handles that
 *      are still attached to realms.
 */
extern EwaSNMPAgentAuthHandle
ewsSNMPAgentAuthHandleSet( const char *realm
                          ,EwaSNMPAgentAuthHandle handle
                         );
#endif /* EW_CONFIG_OPTION_SNMP_AGENT */



/*
 *** Local Variables: ***
 *** mode: c ***
 *** tab-width: 4 ***
 *** End: ***
 */

#endif /* EW_CONFIG_OPTION_AUTH */

#endif /* _EWS_AUTH_H_ */
