/*
 *
 * Product: EmWeb
 * Release: R6_2_0
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
 * EmWeb/Server internal authorization interfaces
 *
 */

#ifndef _EWS_SECURITY_H_
#define _EWS_SECURITY_H_

#ifdef EW_CONFIG_OPTION_AUTH

#include "ews_auth.h"
#include "ews_str.h"

/*
 * define configuration symbol that is true if scheduling will be used
 * by the authorization routines - based on the user's ew_config.h
 */
#if (   defined( EW_CONFIG_OPTION_SCHED_SUSP_RES )   \
     && (   defined( EW_CONFIG_OPTION_AUTH_MBASIC )  \
         || defined( EW_CONFIG_OPTION_AUTH_MDIGEST ) \
         || defined( EW_CONFIG_OPTION_AUTH_VERIFY )  \
        )                                           \
    )
#define EW_CONFIG_AUTH_SCHED
#else
#undef EW_CONFIG_AUTH_SCHED
#endif

#include "ew_md5.h"
#define EW_MD5_SZ       (16)                    /* MD5 digest bytes */
#define EW_MD5_HEX_SZ   (2 * EW_MD5_SZ)         /* ASCII hexadecimal chars */
#define EW_NONCE_SZ     (2 + EW_MD5_HEX_SZ)     /* plus 2 hex chars for index */
#if defined(EW_CONFIG_OPTION_AUTH_DIGEST)
#endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

/******************************************************************************
 * AUTHORIZATION
 *
 * The authorization database is hashed by realm.  For each unique realm,
 * there exists an EwsRealm structure.  Each of these contains a doubly linked
 * list of EwsAuth structures for each authorization scheme supported.
 *
 *****************************************************************************/

typedef struct EwsRealm_s
  {
    struct EwsRealm_s   *next;                  /* hash linkage */
    char                *realm;                 /* realm name - below struct */
    boolean             enabled;                /* TRUE if enabled */

#   ifdef EW_CONFIG_OPTION_SNMP_AGENT
    EwaSNMPAgentAuthHandle  snmp;               /* for SNMP interfaces */
#   endif /* EW_CONFIG_OPTION_SNMP_AGENT */

#   ifdef EW_CONFIG_OPTION_AUTH_MBASIC
    EwsLink             mBasic;                 /* only one manual basic */
#   endif /* EW_CONFIG_OPTION_AUTH_MBASIC */

#   ifdef EW_CONFIG_OPTION_AUTH_BASIC
    EwsLink             basic;                  /* Basic schemes */
#   endif /* EW_CONFIG_OPTION_AUTH_BASIC */

#   ifdef EW_CONFIG_OPTION_AUTH_DIGEST
    EwsLink             digest;                 /* Digest schemes */
#   endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

  } EwsRealm, * EwsRealmP;

typedef struct EwsAuthHandle_s
  {
    EwsLink             link;                   /* linked list header */
    EwaAuthHandle       handle;                 /* application handle */
    EwsRealmP           realmp;                 /* pointer to realm entry */
    EwsAuthScheme       scheme;                 /* scheme (client requests) */

    union
      {
#       ifdef EW_CONFIG_OPTION_AUTH_BASIC
        char            *basic;                 /* Basic scheme */
#       endif /* EW_CONFIG_OPTION_AUTH_BASIC */

#       if defined(EW_CONFIG_OPTION_AUTH_DIGEST)
        struct
          {
            char        *user;                  /* user name */
            char        *md5;                   /* MD5(user:realm:password) */
          } digest;                             /* Digest scheme */
#       endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

        char    reserved;                       /* prevent null union error */

      }                 param;

  } EwsAuth_t, * EwsAuthP;

#ifndef EWS_REALM_HASH_SIZE
#define EWS_REALM_HASH_SIZE     4
#endif /* EWS_REALM_HASH_SIZE */

#if defined(EW_CONFIG_OPTION_AUTH_DIGEST)
#ifndef EWS_NONCE_LIST_LEN
#define EWS_NONCE_LIST_LEN     4      /* max outstanding nonce could be 256 */
#endif

/*
 * Outstanding nonce information
 *
 * The nonce value is a 34 character string of hexadecimal digits.
 * The last 32 characters represent MD5 hash of application-specific nonce
 * parameters. Two digits before that represent the index into the nonce queue,
 * and the remainig characters represent an MD5 hash of secret, index and
 * hash of app params.
 */
#define EWS_NONCE_VERIFIER_SIZE 32
#define EWS_NONCE_APP_HASH_SIZE 32
#define EWS_NONCE_VALUE_SIZE    (  EWS_NONCE_VERIFIER_SIZE \
                                 + 2                       \
                                 + EWS_NONCE_APP_HASH_SIZE)
#define EWS_NONCE_INDEX    32
#define EWS_NONCE_APP_HASH 34

typedef struct EwsNonce_s
  {
    EwsLink       link;
    EwsRealmP     realmp;
    EwaAuthNonce  nonce_params;   /* application-specific parameters */
    /*
     * maximum number of uses per nonce is EWS_NONCE_MAX_CNT
     * but, after EWS_NONCE_OLD_THERSHOLD uses of the nonce,
     * server is going to start sending new next nonce value
     * so the client has a chance to start using new nonce
     * and avoid being challenged.
     */
#   define        EWS_NONCE_MAX_CNT 32
#   define        EWS_NONCE_OLD_THRESHOLD 24 /* 3/4 of EWS_NONCE_MAX_CNT */
    uint32        nc_bmap[1];  /* use count */
    uintf         index;
    uintf         count;
    char          nonce_value[EWS_NONCE_VALUE_SIZE + 1];
                  /* MD5(MD5(user:realm:pass):nonce:cnonce for the session */
    char          md5A1Sess[EW_MD5_HEX_SZ+1];
    EwaAuthNonceStatus status;
    char          *next_nonce_value; /* if this nonce is last OK, use next_nonce_value */
#if defined(EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE)
    boolean       pswd_change_flag;
#endif
  } EwsNonce, *EwsNonceP;

#endif /* EW_CONFIG_OPTION_AUTH_(M)DIGEST */

/*
 * Authentication State machine parameters
 *
 * The following structure holds all the various data that exists
 * across the sub-state-machine's states.  If ewsSuspend/ewsResume
 * functionality is not configured, then this structure is declared as a local
 * variable to the ewsAuthAuthenticate function.  If suspension is used, then
 * this structure is declared as a part of the EwsContext structure (where
 * it's preserved across suspension).
 */


typedef struct
  {

    /* define parameters used by manual Basic authentication */

#   if defined(EW_CONFIG_OPTION_AUTH_MBASIC)
    struct
      {
        char    *cookie;        /* address of cookie string in req */
        boolean freeCookie;     /* true if cookie needs ewaFree when done */
#       if defined(EW_CONFIG_OPTION_AUTH_MBASIC_DECODE)
        char    *user;          /* decoded user and password from cookie */
        char    *password;
#       endif /* EW_CONFIG_OPTION_AUTH_MBASIC_DECODE */
      } basic;
#   endif  /* EW_CONFIG_OPTION_AUTH_MBASIC */

    /* define parameters used by Digest authentication */

#   if defined(EW_CONFIG_OPTION_AUTH_DIGEST)
    struct
      {
        EwsString   username;
        EwsString   nonce;
        EwsString   nc;
        EwsString   digest_uri;
        EwsString   response;
        EwsString   cnonce;
        char  md5A1Ptr[EW_MD5_HEX_SZ+1];     /* address of MD5(user:realm:pass) */
      } digest;
#   endif  /* EW_CONFIG_OPTION_AUTH_DIGEST */

#   if !defined(EW_CONFIG_OPTION_AUTH_MBASIC) \
    && !defined(EW_CONFIG_OPTION_AUTH_DIGEST)
    char    reserved;
#   endif   /* prevent null structure */

} EwsAuthStateParams, *EwsAuthStateParamsP;



/* ================================================================
 * Interfaces
 * ================================================================ */

/*
 * ewsAuthChallenge
 * Builds "WWW-Authenticate:" header(s) for the relm given in
 * "realm" into an outgoing net buffer, if authentication is needed.
 *
 * realmp - Input. pointer to EwsRealm structure
 * context - Input. Context of request
 *
 * Returns EWS_STATUS_OK if success (whether or not an
 * authorization is required), else a failure code.
 */
extern EwsStatus ewsAuthChallenge ( EwsRealmP realmp, EwsContext context );


/*
 * ewsAuthAuthenticate
 * Parses a single "Authorization:" string sent by the client
 * for authorization with realm.
 *
 * context - Input/output.  The current context for the
 *      request.  If the client's authentication validates
 *      against an authorization record for the realm in
 *      the authentication database, then auth_handlep
 *      is set to the void * handle of the qualifying
 *      authorization entry, and auth_user_handlep is
 *      set to the EwaAuthHandle of the qualifying
 *      authorization entry.  If there is no
 *      authentication necessary for that realm, or the
 *      authentication fails, then both handles are reset
 *      to null values (NULL for the internal handle,
 *      EWA_AUTH_HANDLE_NULL for the user handle).
 * realmp - input. pointer to corresponding EwsRealmP
 *
 * Returns EWS_STATUS_OK if Authorization string is valid for
 * that realm, else EWS_STATUS_AUTH_FAILED.
 */
extern EwsStatus ewsAuthAuthenticate ( EwsContext context, EwsRealmP realmp );

/*
 * ewsAuthRealmHash
 * Hash function - simply add up the octets in the realm name modulo the
 * hash table size
 *
 * realm - Input, case sensitive asciiz realm name
 *
 * Returns index into hash table corresponding to realm
 */
extern uintf ewsAuthRealmHash ( const char * realm );

/*
 * ewsAuthRealmLookup
 * Lookup a realm in the hash table bucket
 *
 * hash         - hash bucket index
 * realm        - case sensitive asciiz realm name
 *
 * Returns pointer to realm structure or NULL if not found
 */
extern EwsRealmP ewsAuthRealmLookup ( uintf hash, const char * realm );

/*
 * ewsAuthRealmGet
 * This function returns an EwsRealmP to the specified realm if it exists,
 * or creates a new one if it doesn't.
 *
 * realm        - asciiz realm name
 *
 * Returns realm descriptor or NULL if resources unavailable to build one.
 */
extern EwsRealmP ewsAuthRealmGet ( const char *realm );

/*
 * ewsAuthInit
 * Initialize database
 */
extern boolean ewsAuthInit ( void );

/*
 * Function ewsConvertToHexChar(dst, value);
 * convert the integer, (value), into the correct hex char (dst).
 */
extern void ewsConvertToHexChar( char *dst, int value);

#ifdef EW_CONFIG_OPTION_CLEANUP
/*
 * ewsAuthShutdown
 * Graceful shutdown of database
 */
extern void ewsAuthShutdown ( void );
#endif /* EW_CONFIG_OPTION_CLEANUP */

extern void ewsAuthMD5String( char * dst, MD5_CTX *md5_ctx );
#if defined(EW_CONFIG_OPTION_AUTH_DIGEST)

extern void ewsAuthMD5Update(MD5_CTX *md5_ctx, EwsStringP estring);
extern EwsNonceP ewsAuthNonceCreate (EwsContext context,
                                     EwsLinkP nonce_list,
                                     uint32 * nonce_index_bmapp,
                                     EwsRealmP realmp);

/*
 * ewsAuthDigestUpdate
 * Update entity-body digest from context->md5_entity_ctx to req_str_end
 */
extern void ewsAuthDigestUpdate ( EwsContext context );

/*
 * ewsAuthDigestFinish
 * Finish message digest computation and return TRUE if forged
 */
extern EwsStatus ewsAuthDigestFinish ( EwsContext context );

/*
 * Generate AuthenticationInfo: trailer
 *
 */
extern void ewsAuthenticationInfoHeadOrTrail(EwsContext context);

#endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

#endif /* EW_CONFIG_OPTION_AUTH */

#endif /* _EWS_SECURITY_H_ */

/*
 *** Local Variables: ***
 *** mode: c ***
 *** tab-width: 4 ***
 *** End: ***
 */
