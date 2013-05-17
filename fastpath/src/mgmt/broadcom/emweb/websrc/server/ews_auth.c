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
 *      EmWeb/Server application interface to authorization database
 *
 */

#include "ews.h"

#ifdef EW_CONFIG_OPTION_AUTH

#include "ews_secu.h"
#include "ews_sys.h"
#include "ews_ctxt.h"

/*
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
 * Static Function Declarations
 * ================================================================ */

/* The following functions are sub-states for the ewsAuthAuthenticate()
 * state machine
 */
static EwsStatus authBegin( EwsContext context
                           ,EwsRealmP realmp
                           ,EwsAuthStateParams *params );

#if defined (EW_CONFIG_OPTION_AUTH_MBASIC)
static EwsStatus authBasicState01( EwsContext context
                                  ,EwsRealmP realmp
                                  ,EwsAuthStateParams *params );
#endif /* EW_CONFIG_OPTION_AUTH_MBASIC */


#if defined(EW_CONFIG_OPTION_AUTH_VERIFY)       \
    && (defined(EW_CONFIG_OPTION_AUTH_MBASIC)   \
        || defined(EW_CONFIG_OPTION_AUTH_BASIC) \
        )
static EwsStatus authBasicState02( EwsContext context
                                  ,EwsRealmP realmp
                                  ,EwsAuthStateParams *params );
#endif /* EW_CONFIG_OPTION_AUTH_VERIFY &&
          (EW_CONFIG_OPTION_AUTH_MBASIC || EW_CONFIG_OPTION_AUTH_BASIC) */



#if defined (EW_CONFIG_OPTION_AUTH_DIGEST)
static EwsStatus authDigestStateNonceCheck( EwsContext context
                                   ,EwsRealmP realmp
                                   ,EwsAuthStateParams *params );
static EwsStatus authDigestStateHashA1( EwsContext context
                                   ,EwsRealmP realmp
                                   ,EwsAuthStateParams *params );
#if defined (EW_CONFIG_OPTION_AUTH_VERIFY)
static EwsStatus authDigestStateVerify( EwsContext context
                                   ,EwsRealmP realmp
                                   ,EwsAuthStateParams *params );
#endif
static EwsStatus authDigestStateHashA2( EwsContext context
                                   ,EwsRealmP realmp
                                   ,EwsAuthStateParams *params );

static EwsNonceP authNonceFindRecord(uintf indx);


#if defined (EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE)
static void getNewSecret(const char *oldSecret, const char *maskedSecret, char *newSecretp);
#endif

/*
 * bit map manipulations, bits are numbered from 1, it is assumed
 * that the bit map is in a form of array of 32bit entries
 */

#define IS_BIT_SET(bit, map) ((map[(bit-1)/32] & (0x80000000 >> ((bit-1)%32))))
#define SET_BIT(bit, map)    ((map[(bit-1)/32] |= (0x80000000 >> ((bit-1)%32))))
#define CLEAR_BIT(bit, map)  ((map[(bit-1)/32] &= (~(0x80000000 >> ((bit-1)%32)))))
#define IS_BMAP_FULL(map)    ((map[0]) & (0xFFFFFFFF))
#endif

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

EwsAuthHandle
ewsAuthRegister ( const char *realm,const EwsAuthorization *authorization )
{
  EwsRealmP realmp;
  EwsAuthP authp = EWS_AUTH_HANDLE_NULL;

    EMWEB_TRACE(("ewsAuthRegister(%s, %p)\n", realm, authorization));

    /*
     * Lookup realm to get per-realm data structure.
     */
    EWA_TASK_LOCK();
    realmp = ewsAuthRealmGet(realm);
    EWA_TASK_UNLOCK();

    /*
     * If not found, error
     */
    if (realmp == (EwsRealmP) NULL)
      {
        EMWEB_WARN(("ewsAuthRegister: bad realm\n"));
        return EWS_AUTH_HANDLE_NULL;
      }

    /*
     * Handle registration by scheme
     */
    switch (authorization->scheme)
      {

#       ifdef EW_CONFIG_OPTION_AUTH_BASIC

        /**********************************************************************
         * BASIC
         * The basic scheme uses a simple Base64 encoding of the string
         * "user:password".  The Base64 encoded string is the sole parameter
         * stored in the authorization entry, and is compared to the encoding
         * returned in Authorization: headers
         */
        case ewsAuthSchemeBasic:
          {
            static const char base64[] = /* Base64 translation table */
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            uintf lenu;           /* Length of user name */
            uintf lenp;           /* Length of password */
            const char *srcp;     /* pointer to source data "user:password" */
            char *dstp;           /* pointer to generated Base64 string */
            char src;             /* value of current source byte */
            char dst = 0;         /* value of current destination byte */
            uintf srcbytes = 0;   /* number of source bytes processed */
            uintf dstbytes = 0;   /* number of destination bytes processed */
            uintf dstbits = 0;    /* number of source bits in destination byte */
            uintf bit;            /* bit index */

              /*
               * Get length of application-provided user name and password
               * strings.
               */
              EMWEB_STRLEN(lenu, authorization->params.basic.userName);
              EMWEB_STRLEN(lenp, authorization->params.basic.passWord);

              /*
               * Allocate an authorization entry.  Get enough extra space for
               * the resulting Base64 string.  There is a 3:4 ratio between
               * the "user:password" string and the resulting Base64 string.
               */
              authp = (EwsAuthP) ewaAlloc
                (sizeof(*authp) + ((lenu + lenp + 4) * 4) / 3 + 1);
              if (authp == (EwsAuthP) NULL)
                {
                  EMWEB_WARN(("ewsAuthRegister: no resources for auth\n"));
                  return EWS_AUTH_HANDLE_NULL;
                }

              /*
               * Insert the authorization entry on the BASIC per-method queue
               * of the realm entry and save application handle.
               */
              authp->scheme = authorization->scheme;
              authp->handle = authorization->handle;
              authp->realmp = realmp;

              /*
               * Initialize pointer to Base64 string in authorization entry,
               * source starts with user string
               */
              dstp = authp->param.basic = (char *) & authp[1];
              srcp = authorization->params.basic.userName;

              /*
               * For all bytes in the source "user:password" string
               */
              for (srcbytes = 0; srcbytes < lenu + lenp + 1; srcbytes++)
                {
                  /*
                   * If we finished processing the "user" string, it's
                   * time to handle the ':' seperator
                   */
                  if (srcbytes == lenu)
                    {
                      srcp = (char *)":";
                    }

                  /*
                   * If we finished processing the ':', it's time to handle
                   * the password string
                   */
                  else if (srcbytes == lenu + 1)
                    {
                      srcp = authorization->params.basic.passWord;
                    }

                  /*
                   * Get the next source byte of the "user:password" string
                   */
                  src = *srcp++;

                  /*
                   * For each of the 8 bits in the source byte
                   */
                  for (bit = 0; bit < 8; bit++)
                    {
                      /*
                       * Shift the MSB of the source into the LSB of the
                       * destination
                       */
                      dst = dst << 1;
                      dst |= (src >> 7) & 1;
                      src = src << 1;

                      /*
                       * For every 6 bits shifted, write a Base64 byte
                       */
                      dstbits++;
                      if (dstbits == 6)
                        {
                          dstbytes++;
                          *dstp++ = base64[dst & 0x3f];
                          dstbits = 0;
                        }
                    }
                }

              /*
               * If there are bits left over in the shift register, shift up
               * to the MSB and write the last Base64 byte
               */
              if (dstbits != 0)
                {
                  dstbytes++;
                  dst = dst << (6 - dstbits);
                  *dstp++ = base64[dst & 0x3f];
                }

              /*
               * Write padding bytes so that resulting Base64 string is an
               * even number of four bytes long
               */
              while ((dstbytes & 3) != 0)
                {
                  dstbytes++;
                  *dstp++ = '=';
                }

              /*
               * Terminate the Base64 string
               */
              *dstp++ = '\0';

              /*
               * Insert authorization record on realm queue
               */
              EWA_TASK_LOCK();
              EWS_LINK_INSERT(&realmp->basic, &authp->link);
              EWA_TASK_UNLOCK();
          }
          break;

#       endif /* EW_CONFIG_OPTION_AUTH_BASIC */

/* START-STRIP-EMWEB-LIGHT */

#       ifdef EW_CONFIG_OPTION_AUTH_DIGEST
        case ewsAuthSchemeDigest:
          {
            uintf lenu;
            uintf lenp;
            uintf lenr;

#if defined(EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE)
              /*
               * Get length of application-provided user name and md5
               * strings and realm name.
               */
              EMWEB_STRLEN(lenu, authorization->params.digest_change.userName);
              EMWEB_STRLEN(lenp, authorization->params.digest_change.md5_secret);
              EMWEB_STRLEN(lenr, realm);

              /*
               * Allocate an authorization entry.  Get enough extra space for
               * the resulting MD5 digest.
               */
              authp = (EwsAuthP) ewaAlloc (sizeof(*authp) + lenu +
                2 + EW_MD5_HEX_SZ);
              if (authp == (EwsAuthP) NULL)
                {
                  EMWEB_WARN(("ewsAuthRegister: no resources for auth\n"));
                  return EWS_AUTH_HANDLE_NULL;
                }

              /*
               * Save application handle, user name
               * and generate MD5(user:realm:password).
               */
              authp->scheme = authorization->scheme;
              authp->handle = authorization->handle;
              authp->realmp = realmp;
              authp->param.digest.md5 = (char *) &authp[1];
              authp->param.digest.user =
                (char *) &authp->param.digest.md5[EW_MD5_HEX_SZ + 1];
              EMWEB_STRCPY(authp->param.digest.user
                          ,authorization->params.digest_change.userName
                          );
              EMWEB_STRCPY(authp->param.digest.md5
                          ,authorization->params.digest_change.md5_secret
                          );
#else
            MD5_CTX md5_ctxt;
#           ifndef EW_CONFIG_OPTION_AUTH_NO_REALM_QUALIFIER
            uintf lenq;
            const char *qualifier = ewaAuthRealmQualifier();
#           endif
              /*
               * Get length of application-provided user name and password
               * strings and realm name.
               */
              EMWEB_STRLEN(lenu, authorization->params.digest.userName);
              EMWEB_STRLEN(lenp, authorization->params.digest.passWord);
              EMWEB_STRLEN(lenr, realm);
#             ifndef EW_CONFIG_OPTION_AUTH_NO_REALM_QUALIFIER
              if (qualifier != NULL)
                {
                  EMWEB_STRLEN(lenq, qualifier);
                }
              else
                {
                  lenq=0;
                }
#             endif

              /*
               * Allocate an authorization entry.  Get enough extra space for
               * the resulting MD5 digest.
               */
              authp = (EwsAuthP) ewaAlloc (sizeof(*authp) + lenu +
                2 + EW_MD5_HEX_SZ);
              if (authp == (EwsAuthP) NULL)
                {
                  EMWEB_WARN(("ewsAuthRegister: no resources for auth\n"));
                  return EWS_AUTH_HANDLE_NULL;
                }

              /*
               * Save application handle, user name
               * and generate MD5(user:realm:password).
               */
              authp->scheme = authorization->scheme;
              authp->handle = authorization->handle;
              authp->realmp = realmp;
              authp->param.digest.md5 = (char *) &authp[1];
              authp->param.digest.user =
                (char *) &authp->param.digest.md5[EW_MD5_HEX_SZ + 1];
              EMWEB_STRCPY(authp->param.digest.user
                          ,authorization->params.digest.userName
                          );

              MD5Init(&md5_ctxt);
              MD5Update(&md5_ctxt
                       ,(uint8 *) authorization->params.digest.userName
                       ,lenu);
              MD5Update(&md5_ctxt, (uint8 *) ":", 1);
              MD5Update(&md5_ctxt, (uint8 *) realm, lenr);
#             ifndef EW_CONFIG_OPTION_AUTH_NO_REALM_QUALIFIER
              if (qualifier != NULL)
                {
                  MD5Update(&md5_ctxt, (uint8 *) qualifier, lenq);
                }
#             endif
              MD5Update(&md5_ctxt, (uint8 *) ":", 1);
              MD5Update(&md5_ctxt
                       ,(uint8 *) authorization->params.digest.passWord
                       ,lenp);
              ewsAuthMD5String(authp->param.digest.md5, &md5_ctxt);
#endif /* EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE */

              /*
               * Link to per-realm queue
               */
              EWA_TASK_LOCK();
              EWS_LINK_INSERT(&realmp->digest, &authp->link);
              EWA_TASK_UNLOCK();
          }
          break;
#       endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

#       ifdef EW_CONFIG_OPTION_AUTH_MBASIC
      case ewsAuthSchemeManualBasic:
        {
#         ifdef EMWEB_SANITY
          if (!EWS_LINK_IS_EMPTY(&realmp->mBasic))
            {
              EMWEB_ERROR(("ewsAuthRegister: Cannot register multiple"
                           "ewsAuthSchemeManualBasic to the same realm\n"));
            }
#         endif /* EMWEB_SANITY */

          if ((authp = (EwsAuthP)ewaAlloc( sizeof(*authp) )) == (EwsAuthP)NULL)
            {
              EMWEB_WARN(("ewsAuthRegister: no resources for auth\n"));
              return EWS_AUTH_HANDLE_NULL;
            }
          authp->handle = authorization->handle;
          authp->realmp = realmp;

          EWA_TASK_LOCK();
          EWS_LINK_INSERT(&realmp->mBasic, &authp->link);
          EWA_TASK_UNLOCK();
        }
        break;
#       endif /* EW_CONFIG_OPTION_AUTH_MBASIC */

/* END-STRIP-EMWEB-LIGHT */

      default:
          EMWEB_WARN(("ewsAuthRegister: bad scheme\n"));
      }

    return authp;
}

/*
 * ewsAuthRemove
 * Deletes a particular authentication entry identified by handle.
 *
 * handle - Input, The handle that was returned by ewsAuthRegister() for
 *          this entry.
 *
 * Returns EWS_STATUS_OK on success, else failure code.
 */
EwsStatus
ewsAuthRemove ( EwsAuthHandle handle )
{
    EMWEB_TRACE(("ewsAuthRemove(%p)\n", handle));

    EWA_TASK_LOCK();

    /*
     * Remove authorization entry from per-method linked list on realm
     */
    EWS_LINK_DELETE(&handle->link);
    EWA_TASK_UNLOCK();

    ewaFree(handle);

    return EWS_STATUS_OK;
}

/*
 * ewsAuthRealmEnable
 * Enables a realm to restrict access to documents (restricted by default)
 *
 * realm - Input, the realm to restrict
 *
 * Returns EWS_STATUS_OK on success, else failure code
 */
EwsStatus
ewsAuthRealmEnable ( const char * realm )
{
  uintf hash;
  EwsRealmP realmp;

    hash = ewsAuthRealmHash(realm);
    EWA_TASK_LOCK();
    realmp = ewsAuthRealmLookup(hash, realm);
    if (realmp == (EwsRealmP) NULL)
      {
        EWA_TASK_UNLOCK();
        return EWS_STATUS_BAD_REALM;
      }
    realmp->enabled = TRUE;
    EWA_TASK_UNLOCK();
    return EWS_STATUS_OK;
}

/*
 * ewsAuthRealmDisable
 * Disables a realm causing documents in that realm to be generally accessible.
 *
 * realm - Input, the realm to restrict
 *
 * Returns EWS_STATUS_OK on success, else failure code
 */
EwsStatus
ewsAuthRealmDisable ( const char * realm )
{
  uintf hash;
  EwsRealmP realmp;

    hash = ewsAuthRealmHash(realm);
    EWA_TASK_LOCK();
    realmp = ewsAuthRealmLookup(hash, realm);
    if (realmp == (EwsRealmP) NULL)
      {
        EWA_TASK_UNLOCK();
        return EWS_STATUS_BAD_REALM;
      }
    realmp->enabled = FALSE;
    EWA_TASK_UNLOCK();
    return EWS_STATUS_OK;
}

/*
 * ewsAuthChallenge
 * Builds "WWW-authenticate:" header(s) for the realm given in
 * "realm" into an outgoing net buffer, if authentication is needed.
 *
 * realmp - Input. Pointer to EwsRealm structure
 * context - Input. Context of request
 *
 * Returns EWS_STATUS_OK if success (whether or not an
 * authorization is required), else a failure code.
 */
EwsStatus ewsAuthChallenge ( EwsRealmP realmp, EwsContext context )
{
# ifdef EW_CONFIG_OPTION_AUTH_DIGEST
  boolean basic_flg = FALSE, digest_flg = FALSE;
# endif

# if (defined(EW_CONFIG_OPTION_AUTH_DIGEST)    \
      || defined(EW_CONFIG_OPTION_AUTH_BASIC)  \
      || defined(EW_CONFIG_OPTION_AUTH_MBASIC))
# ifndef EW_CONFIG_OPTION_AUTH_NO_REALM_QUALIFIER
  const char *qualifier = ewaAuthRealmQualifier ();
# endif
# endif

  EW_UNUSED(realmp);  /* might be unused in some configurations */
  EW_UNUSED(context); /* might be unused in some configurations */

  EMWEB_TRACE(("ewsAuthChallenge(%p, %p)\n", realmp, context));

  /*
   * Generate WWW-Authenticate headers for each method with authentication
   * entries in this realm.
   */

  /* START-STRIP-EMWEB-LIGHT */

# if defined(EW_CONFIG_OPTION_AUTH_DIGEST)
  /*
   * DIGEST
   *
   * authentication digest is only implemented for HTTP 1.1 version
   */
  if (FALSE
#     ifdef EW_CONFIG_OPTION_AUTH_MDIGEST
      || TRUE
#     endif
#     ifdef EW_CONFIG_OPTION_AUTH_DIGEST
      || !EWS_LINK_IS_EMPTY( &realmp->digest )
#     endif
      )

    {
      EwsNonceP noncep;
#     ifdef EW_CONFIG_OPTION_AUTH_DOMAIN
      boolean  flag;
      const char *domainp;
#     endif

      /*
       * Generate new nonce
       */
      noncep = ewsAuthNonceCreate(context,
                                  &ews_state->nonce_list,
                                  ews_state->nonce_index_bmap,
                                  realmp);
      if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
        {
          return EWS_STATUS_ABORTED;
        }
#     ifdef EW_CONFIG_AUTH_SCHED
      if (context->schedulingState == ewsContextSuspended)
        {
          return EWS_STATUS_OK;
        }
      context->schedulingState = ewsContextScheduled;  /* ensure not resuming */
#     endif

      if (noncep == (EwsNonceP) NULL)
        {
          EMWEB_WARN(("ewsAuthChallenge: no resources for nonce\n"));
          return EWS_STATUS_NO_RESOURCES;
        }

      /*
       * Generate WWW-Authenticate header
       */
      (void) ewsStringCopyIn(&context->res_str_end,
        "WWW-Authenticate: Digest realm=\"");

      /*
       * realm="realm[qualifier]_value",
       */
      (void) ewsStringCopyIn(&context->res_str_end, realmp->realm);
#     ifndef EW_CONFIG_OPTION_AUTH_NO_REALM_QUALIFIER
      if (qualifier != NULL)
        {
          (void) ewsStringCopyIn(&context->res_str_end, qualifier);
        }
#     endif

      /*
       * nonce="nonce_value",
       */
      (void) ewsStringCopyIn(&context->res_str_end, "\", nonce=\"");
      (void) ewsStringCopyIn(&context->res_str_end, noncep->nonce_value);
      (void) ewsStringCopyIn(&context->res_str_end, "\"");

#     ifdef EW_CONFIG_OPTION_AUTH_DOMAIN
      /*
       * domain="domain_value"
       */
      flag = ewaAuthDigestDomain(context, realmp->realm, &domainp);
      EWA_TASK_LOCK();
      if ((!flag) || (domainp == NULL))
        {
          ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
          EMWEB_WARN(("ewsAuthChallenge: no free memory\n"));
          EWA_TASK_UNLOCK();
          return EWS_STATUS_NO_RESOURCES;
        }
      (void) ewsStringCopyIn(&context->res_str_end, "\", domain=\"");
      (void) ewsStringCopyIn(&context->res_str_end, domainp);
      (void) ewsStringCopyIn(&context->res_str_end, "\"");
      EWA_TASK_UNLOCK();
#     endif

#     ifdef EW_CONFIG_OPTION_BROKEN_NEED_OPAQUE
      /*
       * opaque="opaque_value",
       */
      (void) ewsStringCopyIn(&context->res_str_end,
                            ", opaque=\"EmWeb\"");
#     endif /* EW_CONFIG_OPTION_BROKEN_NEED_OPAQUE */

      /*
       * stale=true|false,
       */
      if (context->nonce_stale == TRUE)
        {
          (void) ewsStringCopyIn(&context->res_str_end, ", stale=true");
        }
      else
        {
          (void) ewsStringCopyIn(&context->res_str_end, ", stale=false");
        }

      (void) ewsStringCopyIn(&context->res_str_end, ", qop=\"");
#     ifdef _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT
      (void) ewsStringCopyIn(&context->res_str_end,
                             "auth,auth-int");  /* qop="auth,auth-int", */
#     else
      (void) ewsStringCopyIn(&context->res_str_end,
                             "auth");           /* qop="auth", */
#     endif /* _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT */
      (void) ewsStringCopyIn(&context->res_str_end, "\"");

      /*
       * algorithm=MD5-sess\r\n
       */
      (void) ewsStringCopyIn(&context->res_str_end, ", algorithm=MD5-sess\r\n");
      digest_flg = TRUE;
    }
#   endif /* EW_CONFIG_OPTION_AUTH_(M)DIGEST */

  /* END-STRIP-EMWEB-LIGHT */

# if defined(EW_CONFIG_OPTION_AUTH_BASIC) \
  || defined(EW_CONFIG_OPTION_AUTH_MBASIC)
  /*
   * BASIC
   */
  if (FALSE
#     ifdef EW_CONFIG_OPTION_AUTH_BASIC
      || !EWS_LINK_IS_EMPTY( &realmp->basic )
#     endif
#     ifdef EW_CONFIG_OPTION_AUTH_MBASIC
      || !EWS_LINK_IS_EMPTY( &realmp->mBasic )
#     endif
      )
    {
      (void) ewsStringCopyIn(&context->res_str_end,
        "WWW-Authenticate: Basic realm=\"");
      (void) ewsStringCopyIn(&context->res_str_end, realmp->realm);
#     ifndef EW_CONFIG_OPTION_AUTH_NO_REALM_QUALIFIER
      if (qualifier != NULL)
        {
          (void) ewsStringCopyIn(&context->res_str_end, qualifier);
        }
#     endif
      (void) ewsStringCopyIn(&context->res_str_end, "\"\r\n");
#     ifdef EW_CONFIG_OPTION_AUTH_DIGEST
      basic_flg = TRUE;
#     endif
    }
#   endif /* EW_CONFIG_OPTION_AUTH_(M)BASIC */

# ifdef EW_CONFIG_OPTION_AUTH_DIGEST
  if ((digest_flg == TRUE) && (basic_flg == FALSE))
    {
      if (!((context->req_protocol_maj >= 1) && (context->req_protocol_min > 0)))
        {
          /*
           * authentication digest is only implemented for HTTP 1.1 version
           */
          {
            ewaLogHook(context, EWS_LOG_STATUS_BAD_REQUEST);
            return EWS_STATUS_BAD_REQUEST;
          }
        }
    }
# endif


    return EWS_STATUS_OK;
}


/*
 * ewsAuthAuthenticate
 * Parses a single "Authorization:" string sent by the client for
 * authorization with realm.
 *
 * This routine has been implement as a state machine.  Each substate
 * is represented by a static function (located at the end of this file).
 * The state machine implementation allows the application to suspend
 * the context (ewsSuspend) at any substate.  On resumption of the context
 * (ewsResume), the authorization state machine will restart at the
 * substate where the suspend occurred.
 *
 * The following is a flowchart of the state machine (assuming all
 * authorization options are configured):
 *
 *                         +-----------+
 *               >>start>> | authBegin |
 *                         +-----+-----+
 *       +------------------+    |
 *>>end>>| authBasicState01 |<---+
 *       +--------+---------+    |
 *                |              |
 *                |              |
 *                V              |
 *       +------------------+    |
 *>>end>>| authBasicState02 |    |
 *       +--------+---------+    |
 *                               |
 *                               V
 *                           +-----------------------------------+
 *                           | ...ServeSetupAuthDigestNonceCheck |
 *                           +-------------+---------------------+
 *                                         |
 *                                         |
 *                                         V
 *                           +-------------------------------+
 *                           | ...ServeSetupAuthDigestHashA1 |
 *                           +-------------+-----------------+
 *                                         |
 *                        if (entity_body) |
 *                    +--------------------+-------------+
 *                    |                                  |
 *                    |                                  |
 *                    V                                  V
 * +----------------------------------+    +-------------------------------+
 * | ...ServeSetupAuthDigestIntegrity |--->| ...ServeSetupAuthDigestHashA2 |
 * +----------------------------------+    +-------------------------------+
 *                                                       |
 *                                                       |
 *                                                       V
 *                                         +-------------------------------+
 *                                         | ...ServeSetupAuthDigestVerify |
 *                                         +-------------+-----------------+
 *                                                       |
 *                                                       |
 *                                                       |
 *                                                       V
 *                                         +-------------------------------+
 *                                         | ...ServeSetupAuthDone         |
 *                                         +-------------------------------+
 *
 *
 * context - Input/output.  The current context for the request.  If the
 *      client's authentication validates against an authorization record for
 *      the realm in the authentication database, then auth_handle is set to
 *      the EwaAuthHandle of the qualifying authorization entry.  If there is
 *      no authentication necessary for that realm, or the authentication
 *      fails, then both the handle is set to EWA_AUTH_HANDLE_NULL.
 * realmp - pointer to EwsRealm structure
 *
 * Returns EWS_STATUS_AUTH_FAILED if Authorization string is not valid for
 * that realm.   Returns EWS_STATUS_OK if either the Authorization string
 * is valid for the realm, or the application has ewsSuspended the context
 * during the authentication process.  The context's state should be
 * checked on return to determine whether or not the context was suspended.
 */

EwsStatus
ewsAuthAuthenticate( EwsContext context, EwsRealmP realmp )
{
  EwsStatus status = EWS_STATUS_OK;

  /*
   * if scheduling can be used to suspend authentication, then
   * get the authentication state parameters from the context
   * block.  Otherwise, declare them locally to this function
   */
#ifdef EW_CONFIG_OPTION_AUTH_DIGEST
# define PARAMS (context->authStateParams)
#else
# ifdef EW_CONFIG_AUTH_SCHED
# define PARAMS (context->authStateParams)
# else
  EwsAuthStateParams    PARAMS;
# endif  /* EW_CONFIG_AUTH_SCHED */
# endif  /* EW_CONFIG_AUTH_SCHED */

  EMWEB_TRACE(("ewsAuthAuthenticate(0x%p, 0x%p)\n", context, realmp));
  /*
   * If the realm is disabled, access is allowed
   */
  if (realmp->enabled == FALSE)
    {
      context->auth_state= ewsAuthStateUnauthenticated;
#     ifdef EW_CONFIG_AUTH_SCHED
      context->schedulingState = ewsContextScheduled;
#     endif
      context->substate.setup = ewsContextServeSetupAuthDone;
      return EWS_STATUS_OK;
    }

  /*
   * main loop, run state machine until auth pass, fail,
   * or suspended
   */

  do {
    switch (context->substate.setup)
      {
      case ewsContextServeSetupBegin:
        status = authBegin( context, realmp, &PARAMS );
        break;

#     if defined (EW_CONFIG_OPTION_AUTH_MBASIC)

      case ewsContextServeSetupAuth01BasicCheck:
        status = authBasicState01( context, realmp, &PARAMS );
        break;

#     endif    /* EW_CONFIG_OPTION_AUTH_MBASIC */

#if defined(EW_CONFIG_OPTION_AUTH_VERIFY)       \
    && (defined(EW_CONFIG_OPTION_AUTH_MBASIC)   \
        || defined(EW_CONFIG_OPTION_AUTH_BASIC) \
        )

      case ewsContextServeSetupAuth02BasicCheck:
        status = authBasicState02( context, realmp, &PARAMS );
        break;

#endif /* EW_CONFIG_OPTION_AUTH_VERIFY &&
          (EW_CONFIG_OPTION_AUTH_MBASIC || EW_CONFIG_OPTION_AUTH_BASIC) */

#     if defined (EW_CONFIG_OPTION_AUTH_DIGEST)

      case ewsContextServeSetupAuthDigestNonceCheck:
        status = authDigestStateNonceCheck( context, realmp, &PARAMS );
        break;

      case ewsContextServeSetupAuthDigestHashA1:
        status = authDigestStateHashA1( context, realmp, &PARAMS );
        break;

#     if defined EW_CONFIG_OPTION_AUTH_VERIFY
      case ewsContextServeSetupAuthDigestVerify:
        status = authDigestStateVerify( context, realmp, &PARAMS );
        break;
#     endif

      case ewsContextServeSetupAuthDigestHashA2:
        status = authDigestStateHashA2( context, realmp, &PARAMS );
        break;

      case ewsContextServeSetupAuthDigestIntegrity:
        status = EWS_STATUS_OK;
        ewsAuthDigestUpdate( context );
        break;
#     endif

      default:              /* should not happen */
#     ifdef EMWEB_SANITY
        EMWEB_WARN(("ewsAuthAuthenticate: bad setup substate\n"));
#     endif
        context->auth_state= ewsAuthStateFailed;
        return EWS_STATUS_AUTH_FAILED;
      }
  } while (status == EWS_STATUS_OK
           && context->substate.setup != ewsContextServeSetupAuthDone
           && context->substate.setup != ewsContextServeSetupAuthDigestIntegrity
#          ifdef EW_CONFIG_AUTH_SCHED
           && context->schedulingState != ewsContextSuspended
#          endif /* EW_CONFIG_AUTH_SCHED */
           );

  return status;
}



/*
 * ewsAuthRealmHash
 * Hash function - simply add up the octets in the realm name modulo the
 * hash table size
 *
 * realm - Input, case sensitive asciiz realm name
 *
 * Returns index into hash table corresponding to realm
 */
uintf
ewsAuthRealmHash ( const char * realm )
{
  uintf hash;

    for (hash = 0; *realm != '\0'; hash += *realm++)
      ;
    return hash % EWS_REALM_HASH_SIZE;
}

/*
 * ewsAuthRealmLookup
 * Lookup a realm in the hash table bucket
 *
 * hash         - hash bucket index
 * realm        - case sensitive asciiz realm name
 *
 * Returns pointer to realm structure or NULL if not found
 */
EwsRealmP
ewsAuthRealmLookup ( uintf hash, const char * realm )
{
  EwsRealmP realmp;
  boolean bvalue;

    realmp = ews_state->realm_hash_table[hash];
    while (realmp != (EwsRealmP) NULL)
      {
        EMWEB_STRCMP(bvalue, realmp->realm, realm);
        if (!bvalue)
          {
            break;
          }
        realmp = realmp->next;
      }
    return realmp;
}

/*
 * ewsAuthRealmGet
 * This function returns an EwsRealmP to the specified realm if it exists,
 * or creates a new one if it doesn't.
 *
 * realm        - asciiz realm name
 *
 * Returns realm descriptor or NULL if resources unavailable to build one.
 */
EwsRealmP
ewsAuthRealmGet ( const char *realm )
{
  uintf hash;
  EwsRealmP realmp;
  uintf len;

    hash = ewsAuthRealmHash(realm);
    realmp = ewsAuthRealmLookup(hash, realm);
    if (realmp == (EwsRealmP) NULL)
      {
        /*
         * The realm structure is followed immediately by the asciiz realm
         * name, so allocate enough space for it.
         */
        EMWEB_STRLEN(len, realm);
        realmp = (EwsRealmP) ewaAlloc(sizeof(*realmp) + len + 1);
        if (realmp == (EwsRealmP) NULL)
          {
            EMWEB_ERROR(("ewsAuthRealmGet: no resources\n"));
            return (EwsRealmP) NULL;
          }

        /*
         * Copy the realm name
         */
        realmp->realm = (char *) &realmp[1];
        EMWEB_STRCPY(realmp->realm, realm);

        /*
         * By default, disable access to the realm
         */
        realmp->enabled = TRUE;

#       ifdef EW_CONFIG_OPTION_SNMP_AGENT
        /*
         * each realm can have a set of SNMP auth parameters
         * associated with it (these are user defined).
         */
        realmp->snmp = EWA_SNMP_AGENT_AUTH_HANDLE_NULL;
#       endif /* EW_CONFIG_OPTION_SNMP_AGENT */

        /*
         * Initialize the scheme-specific queues
         */
#       ifdef EW_CONFIG_OPTION_AUTH_MBASIC
        EWS_LINK_INIT(&realmp->mBasic);
#       endif /* EW_CONFIG_OPTION_AUTH_MBASIC */
#       ifdef EW_CONFIG_OPTION_AUTH_BASIC
        EWS_LINK_INIT(&realmp->basic);
#       endif /* EW_CONFIG_OPTION_AUTH_BASIC */
#       ifdef EW_CONFIG_OPTION_AUTH_DIGEST
        EWS_LINK_INIT(&realmp->digest);
#       endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

        /*
         * Insert in hash table
         */
        realmp->next = ews_state->realm_hash_table[hash];
        ews_state->realm_hash_table[hash] = realmp;
      }

    return realmp;
}

/*
 * ewsAuthInit
 * Initialize database
 */
boolean
ewsAuthInit ( void )
{
  uintf i;
# if defined(EW_CONFIG_OPTION_AUTH_DIGEST)
  EwsNonceP noncep;
# endif

    for (i = 0; i < EWS_REALM_HASH_SIZE; i++)
      {
        ews_state->realm_hash_table[i] = (EwsRealmP) NULL;
      }

#   if defined(EW_CONFIG_OPTION_AUTH_DIGEST)
    /*
     * server maintaines a list of nonce records.
     * The length of the list is predefined and the whole
     * list is created right now.
     * Records on the list are in Last Used First order (when
     * record is used, we push it to the top of the list),
     * so when the record is needed for new nonce (record re-used,
     * new info), we pick the last record on the list.
     * Also, when the record is declared un-used it is re-linked
     * (inserted) to the tail of the list.
     */
    EWS_LINK_INIT(&ews_state->nonce_list);
    for (i = 1; i <= EWS_NONCE_LIST_LEN; i++)
      {
        noncep = (EwsNonceP)ewaAlloc(sizeof(EwsNonce));
        if (noncep == NULL)
          {
            EMWEB_ERROR(("ewsAuthInit: no free memory\n"));
            return(FALSE);
          }
        noncep->index = i;
        noncep->nc_bmap[0] = 0;
        noncep->count = 0;
        noncep->realmp = NULL;
        noncep->md5A1Sess[0] = '\0';
#if defined(EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE)
        noncep->pswd_change_flag = FALSE;
#endif
        noncep->status = ewaAuthNonceOK;
        noncep->next_nonce_value = NULL;
        EWS_LINK_PUSH(&ews_state->nonce_list, &noncep->link);
      }
    ews_state->nonce_index_bmap =
      (uint32 *)ewaAlloc(sizeof(uint32) * (EWS_NONCE_LIST_LEN/32 + 1));
    if (ews_state->nonce_index_bmap == (uint32 *)NULL)
      {
        EMWEB_ERROR(("ewsAuthInit: no free memory\n"));
        return FALSE;
      }
    EMWEB_MEMSET(ews_state->nonce_index_bmap
                , 0
                , sizeof(uint32) * (EWS_NONCE_LIST_LEN/32 + 1));
    /*
     * get secret
     */

    ews_state->secret = NULL;
    ewaAuthDigestSecret((char **)&ews_state->secret);
#   endif /* EW_CONFIG_OPTION_AUTH_DIGEST */
    return TRUE;
}

#ifdef EW_CONFIG_OPTION_CLEANUP
/*
 * ewsAuthShutdown
 * Graceful shutdown of database
 */
void
ewsAuthShutdown ( void )
{
  uintf i;
  EwsRealmP realmp;
  EwsRealmP tmp;
# if defined(EW_CONFIG_OPTION_AUTH_DIGEST)
  EwsNonceP noncep, next;
# endif

    /*
     * For each entry in the hash table
     */
    for (i = 0; i < EWS_REALM_HASH_SIZE; i++)
      {
        /*
         * For each realm entry in the hash bucket
         */
        realmp = ews_state->realm_hash_table[i];
        while (realmp != (EwsRealmP) NULL)
          {

#          ifdef EW_CONFIG_OPTION_AUTH_MBASIC
            /*
             * release the Manual Basic authentication entry if
             * present
             */
            if (!EWS_LINK_IS_EMPTY(&realmp->mBasic))
              {
                EwsAuthP authp;
                authp = (EwsAuthP) realmp->mBasic.next;
                EWS_LINK_DELETE(&authp->link);
                ewaFree( authp );
              }
#           endif /* EW_CONFIG_OPTION_AUTH_MBASIC */

#           ifdef EW_CONFIG_OPTION_AUTH_BASIC
            /*
             * Release all the basic authentication entries attached to the
             * realm.
             */
            while (realmp->basic.next != &realmp->basic)
              {
                EwsAuthP authp;
                authp = (EwsAuthP) realmp->basic.next;
                EWS_LINK_DELETE(&authp->link);
                ewaFree(authp);
              }
#           endif /* EW_CONFIG_OPTION_AUTH_BASIC */

#           ifdef EW_CONFIG_OPTION_AUTH_DIGEST
            /*
             * Release all the digest authentication entries attached to the
             * realm.
             */
            while (realmp->digest.next != &realmp->digest)
              {
                EwsAuthP authp;
                authp = (EwsAuthP) realmp->digest.next;
                EWS_LINK_DELETE(&authp->link);
                ewaFree(authp);
              }
#           endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

#           ifdef EW_CONFIG_OPTION_SNMP_AGENT
            /*
             * Free any associated SNMP auth stuff
             */
            if (realmp->snmp != EWA_SNMP_AGENT_AUTH_HANDLE_NULL)
              {
                ewaSNMPAgentAuthHandleFree( realmp->snmp );
              }
#           endif /* EW_CONFIG_OPTION_SNMP_AGENT */


            /*
             * Release the realm entry itself
             */
            tmp = realmp->next;
            ewaFree(realmp);
            realmp = tmp;
          }
      }

#   if defined(EW_CONFIG_OPTION_AUTH_DIGEST)
    /*
     * Release state for any outstanding nonce challenges
     */
    noncep = (EwsNonceP)EWS_LINK_HEAD(ews_state->nonce_list);
    next = (EwsNonceP)(EWS_LINK_NEXT(noncep->link));
    for (i = 1; i <= EWS_NONCE_LIST_LEN; i++)
      {
        if (noncep->next_nonce_value != NULL)
          {
            ewaFree(noncep->next_nonce_value);
          }
        ewaFree(noncep);
        noncep = next;
        next = (EwsNonceP)(EWS_LINK_NEXT(next->link));
      }
    if (ews_state->nonce_index_bmap != NULL)
      {
        ewaFree(ews_state->nonce_index_bmap);
        ews_state->nonce_index_bmap = NULL;
      }
#   endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

}
#endif /* EW_CONFIG_OPTION_CLEANUP */

void ewsAuthHashMD5Password(const char *password, char **digest )
{
  MD5_CTX md5_ctxt;
  char    md5[EW_MD5_HEX_SZ + 1];
  uintf   len;

  /*
   * and generate MD5(password)
   */
  MD5Init(&md5_ctxt);
  EMWEB_STRLEN(len, password);
  MD5Update(&md5_ctxt,(uint8 *)password, len);
  ewsAuthMD5String(md5, &md5_ctxt);

  EMWEB_STRCPY(*digest, md5);
  return;
}

/* START-STRIP-EMWEB-LIGHT */

/*
 * Finish MD5 digest and convert to a null-terminated 32-character
 * hexadecimal ascii string
 */
void
ewsAuthMD5String( char * dst, MD5_CTX *md5_ctx )
{
  uint8 md5[EW_MD5_SZ];
  uintf i;
  uintf nybble;

  MD5Final(md5, md5_ctx);
  for (i = 0; i < EW_MD5_SZ; i++)
    {
      nybble = (md5[i] >> 4) & 0x0f;
      *dst++ = nybble > 9 ? nybble - 10 + 'a' : nybble + '0';
      nybble = md5[i] & 0x0f;
      *dst++ = nybble > 9 ? nybble - 10 + 'a' : nybble + '0';
    }
  *dst = '\0';
}

#if defined(EW_CONFIG_OPTION_AUTH_DIGEST)
/*
 * Finish MD5 digest and convert to 32-character
 * hexadecimal ascii string
 */
static void
ewsAuthMD5StringNoNull( char * dst, MD5_CTX *md5_ctx )
{
  uint8 md5[EW_MD5_SZ];
  uintf i;
  uintf nybble;

    MD5Final(md5, md5_ctx);
    for (i = 0; i < EW_MD5_SZ; i++)
      {
        nybble = (md5[i] >> 4) & 0x0f;
        *dst++ = nybble > 9? nybble - 10 + 'a' : nybble + '0';
        nybble = md5[i] & 0x0f;
        *dst++ = nybble > 9? nybble - 10 + 'a' : nybble + '0';
      }
}

/*
 * Update MD5 digest from EwsString
 */
void
ewsAuthMD5Update(MD5_CTX *md5_ctx, EwsStringP estring)
{
  char *buffer;
  boolean buffer_flag = FALSE;
  uintf len;

  if (ewsStringIsContiguous(estring))
    {
      buffer = (char *)ewsStringData( estring );
      buffer[ewsStringLength( estring )] = '\0';
    }
  else
    {
      if ((buffer =
           (char *)ewaAlloc(ewsStringLength(estring) + 1)
           ) == NULL
          )
        {
          EMWEB_ERROR(("ewsAuthMD5Update: no free memory\n"));
          return;
        }
      ewsStringCopyOut( buffer
                        ,estring
                        ,ewsStringLength( estring ) );
      buffer_flag = TRUE;
    }

  EMWEB_STRLEN(len, buffer);
  MD5Update(md5_ctx, (uint8 *)buffer, len);
  if (buffer_flag == TRUE)
    {
      ewaFree(buffer);
    }

/*
  uintf bytes;
  uintf remaining;
  uintf offset;
  EwaNetBuffer buffer;

    buffer = estring->first;
    offset = estring->offset;
    remaining = estring->length;
    while (remaining > 0)
      {
        bytes = ewaNetBufferLengthGet(buffer) - offset;
        if (bytes > remaining) bytes = remaining;
        if (bytes > 0)
          {
            MD5Update(md5_ctx, ewaNetBufferDataGet(buffer) + offset, bytes);
          }
        remaining -= bytes;
        buffer = ewaNetBufferNextGet(buffer);
        offset = 0;
      }
*/
}

/*************************************************************************
 *
 * Nonce management functions
 *
 * New nonce is created:
 * (A) at the time of initial authentication challenge or
 * (B) application may specify, that "this is the last time nonce is good".
 *     Server creates a new nonce to include in Authentication-info
 *     as next nonce or
 * (C) after a nonce hase been used 32 times, server issues a new challenge.
 *
 * A nonce is constructed as follows:
 *
 *          +---------+-----+-----------------+
 *  nonce = |validator|index|MD5(nonce params)|
 *          +---------+-----+-----------------+
 *    where:
 *            index = server created index <1, EWS_NONCE_LIST_SIZE>
 *            nonce params = application provided raw data
 *            validator = MD5(indexMD5(nonce params)secret)
 *            secret = application generated NULL terminated string
 *
 * Nonce becomes stale when:
 * (A) nonce_list overflows and server removes oldest nonce from it,
 * (B) application declares nonce as such.
 *
 *
 *
 *************************************************************************/

/*
 * authNonceFindRecord() - find nonce record on the list
 */

static EwsNonceP authNonceFindRecord(uintf indx)
{
   EwsNonceP  next;

   next = (EwsNonceP)EWS_LINK_HEAD(ews_state->nonce_list);
   do
     {
        if (indx == next->index)
          {
            return(next);
          }
        next = (EwsNonceP)(EWS_LINK_NEXT(next->link));
     } while (next != (EwsNonceP)EWS_LINK_HEAD(ews_state->nonce_list));
   return((EwsNonceP)NULL);
}

/*
 * ewsAuthNonceCreate() - create new nonce and nonce record.
 */

EwsNonceP
ewsAuthNonceCreate (EwsContext context,
                    EwsLinkP nonce_list,
                    uint32 * nonce_index_bmap,
                    EwsRealmP realmp)
{
  MD5_CTX md5_ctx;
  uint8 nybble;
  uintf len;
  EwsNonceP noncep;

  /*
   * Create new nonce
   */
  EWA_TASK_LOCK();      /* protect nonce structures */
  noncep = (EwsNonceP) nonce_list->prev;
  SET_BIT(noncep->index, nonce_index_bmap);

  /* push the record to the top of the list */
  EWS_LINK_DELETE(&noncep->link);
  EWS_LINK_PUSH(nonce_list, &noncep->link);
  EWA_TASK_UNLOCK();

  ewaAuthNonceCreate(context, realmp->realm, &noncep->nonce_params);
  /*
   * first check if the application suspened or aborted
   */
  if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
    {
      return((EwsNonceP)NULL);
    }

# ifdef EW_CONFIG_AUTH_SCHED
  if (context->schedulingState == ewsContextSuspended)
    {
      return((EwsNonceP)NULL);
    }
  context->schedulingState = ewsContextScheduled;  /* ensure not resuming */
# endif

  noncep->nc_bmap[0] = 0;
  noncep->count = 0;
  noncep->realmp = realmp;
  noncep->md5A1Sess[0] = '\0';
#if defined(EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE)
  noncep->pswd_change_flag = FALSE;
#endif
  noncep->status = ewaAuthNonceOK;
  if (noncep->next_nonce_value != NULL)
    {
      ewaFree(noncep->next_nonce_value);
    }
  noncep->next_nonce_value = NULL;

  /*
   * Build nonce
   *
   * get md5 hash of application nonce params
   */
  MD5Init(&md5_ctx);
  MD5Update(&md5_ctx
           ,(uint8 *) &noncep->nonce_params
           ,sizeof(EwaAuthNonce)
           );
  ewsAuthMD5String(&noncep->nonce_value[EWS_NONCE_APP_HASH], &md5_ctx);
  /*
   * attach index
   */
  nybble = (noncep->index >> 4) & 0xf;
  noncep->nonce_value[EWS_NONCE_INDEX] =
    nybble > 9? nybble + 'a' - 10 : nybble + '0';
  nybble = noncep->index & 0xf;
  noncep->nonce_value[EWS_NONCE_INDEX+1] =
    nybble > 9? nybble + 'a' - 10 : nybble + '0';
  /*
   * calculate verifier and attach
   */
  MD5Init(&md5_ctx);
  MD5Update(&md5_ctx, (uint8 *) &noncep->nonce_value[EWS_NONCE_INDEX],
            EWS_NONCE_VALUE_SIZE - EWS_NONCE_VERIFIER_SIZE);
  if (ews_state->secret != NULL)
    {
      EMWEB_STRLEN(len, ews_state->secret);
      MD5Update(&md5_ctx, (uint8 *) ews_state->secret, len);
    }
  ewsAuthMD5StringNoNull(noncep->nonce_value, &md5_ctx);
  noncep->nonce_value[EWS_NONCE_VALUE_SIZE] = '\0';

  return noncep;
}

/*
 * ewsAuthDigestUpdate
 * Update entity digest from md5_entity_str to end-of-data
 */
void
ewsAuthDigestUpdate ( EwsContext context )
{
  EwaNetBuffer buf = context->md5_entity_str.first;
  uintf offset = context->md5_entity_str.offset;
  uintf bytes;

    /*
     * Consume entity-body parsed so far
     */
     while (context->md5_remaining)
       {
         bytes = ewaNetBufferLengthGet(buf) - offset;
         if (bytes > context->md5_remaining)
           {
             bytes = context->md5_remaining;
           }
         if (bytes > 0)
           {
             MD5Update(&context->md5_entity_ctx
                      ,ewaNetBufferDataGet(buf) + offset
                      ,bytes);
          }
        offset += bytes;
        context->md5_remaining -= bytes;
        buf = ewaNetBufferNextGet(buf);
        if (buf == EWA_NET_BUFFER_NULL)
          {
            break;
          }
        context->md5_entity_str.first = buf;
        offset = 0;
      }
    context->md5_entity_str.offset = offset;
}

/*
 * ewsAuthDigestFinish
 * Complete calculation of entity body digest.
 */
EwsStatus
ewsAuthDigestFinish ( EwsContext context )
{
  context->substate.setup = ewsContextServeSetupAuthDigestHashA2;
  return (ewsAuthAuthenticate(context, context->realmp));
}

#endif /* EW_CONFIG_OPTION_AUTH_(M)DIGEST */

/* END-STRIP-EMWEB-LIGHT */


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
EwaSNMPAgentAuthHandle
ewsSNMPAgentAuthHandleSet( const char *realm
                          ,EwaSNMPAgentAuthHandle handle
                         )
{
  uintf hash;
  EwsRealmP realmp;
  EwaSNMPAgentAuthHandle old;

  EMWEB_TRACE(("ewsSNMPAgentAuthHandleSet( realm=%s )\n", realm ));

  /*
   * Lookup realm to get per-realm data structure.
   */
  hash = ewsAuthRealmHash(realm);
  EWA_TASK_LOCK();
  realmp = ewsAuthRealmLookup(hash, realm);

# ifdef EMWEB_SANITY
  /*
   * By definition, the realm needs to exist first.  EMWEB_ERROR
   * in the case that it does not
   */
  if (realmp == (EwsRealmP) NULL)
    {
      EWA_TASK_UNLOCK();
      EMWEB_ERROR(("ewsSNMPAgentAuthHandleSet: ERROR - realm does not exist!\n"
                   " You MUST create the realm by ewsAuthRegister() FIRST!\n"
                 ));
      return EWA_SNMP_AGENT_AUTH_HANDLE_NULL;
    }
# endif

  old = realmp->snmp;
  realmp->snmp = handle;

  EWA_TASK_UNLOCK();

  return old;
}
#endif /* EW_CONFIG_OPTION_SNMP_AGENT */


/* ================================================================
 * Static Functions
 * ================================================================ */

# if defined EW_CONFIG_OPTION_AUTH_DIGEST
/*
 * authFindUser(EwsLink list, char *user)
 *
 * Find user's authentication record on the list
 */
static EwsAuthP authFindUser(EwsLinkP listp, const char *user)
{
  EwsAuthP authp, found_authp = NULL;
  boolean cmp_result;

  if (EWS_LINK_IS_EMPTY(listp))
    {
      return(NULL);
    }
  else
    {
      EWA_TASK_LOCK();          /* protect authorization list */
      authp = (EwsAuthP)EWS_LINK_HEAD((*listp));
      do {
           EMWEB_STRCMP(cmp_result, user, authp->param.digest.user);
           if (!cmp_result)
             {
               found_authp = authp;
               break;
             }
          authp = (EwsAuthP)(EWS_LINK_NEXT(authp->link));
         } while (EWS_LINK_NEXT(authp->link) != EWS_LINK_HEAD((*listp)));
      EWA_TASK_UNLOCK();        /* end protect authorization entry */
    }
  return(found_authp);
}
#endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

/*
 * authBegin
 *
 * Start the authorization process by determining which Scheme is
 * being used.  Setup any pertinent data for that scheme, and move
 * to the next state for the scheme.
 */
static EwsStatus
authBegin ( EwsContext context, EwsRealmP realmp, EwsAuthStateParams *params )
{
  EwsString begin;      /* begin estring for parsing entries */
# if defined EW_CONFIG_OPTION_AUTH_BASIC || defined EW_CONFIG_OPTION_AUTH_MBASIC
  EwsString end;        /* end estring for parsing entries */
  uintf match_c;        /* matching character for parsing entries */
# ifdef EW_CONFIG_OPTION_AUTH_BASIC
  EwsAuthP authp;       /* authorization entry */
# endif /* EW_CONFIG_OPTION_AUTH_BASIC */
# endif /*EW_CONFIG_OPTION_AUTH_BASIC*/

  EW_UNUSED(params);
  EW_UNUSED(realmp);

  /*
   * Start parsing from the value of the Authorization: header, if present
   * (the main parser already saved this in the context for us).
   */
  begin = context->req_headerp[ewsRequestHeaderAuthorization];
  if (ewsStringLength(&begin) == 0)
    {
      ewaLogHook(context, EWS_LOG_STATUS_AUTH_REQUIRED);
      return EWS_STATUS_AUTH_CHALLENGE;
    }


  /* ========================================================
   * If the Authorization: header is for BASIC authentication
   * ========================================================
   */

# if defined EW_CONFIG_OPTION_AUTH_BASIC || defined EW_CONFIG_OPTION_AUTH_MBASIC
  if (ewsStringCompare(&begin, "basic", ewsStringCompareCaseInsensitive, NULL))
    {
      /*
       * Reset end string to beginning of white space after "basic" token.
       * Scan to first non-white-space character which should begin the
       * Base64 basic cookie.
       */

      /*
       * reset parser
       */
      ewsStringLengthSet(&begin, 0);
      end = begin;

      match_c = ewsStringSearch(&begin
                               ,&end
                               ,(char *)" \t"
                               ,ewsStringSearchExclude
                               ,NULL);
      if (match_c == '\r' || match_c == '\n')          /* no cookie? */
        {
          ewaLogHook(context, EWS_LOG_STATUS_AUTH_FAILED);
          return EWS_STATUS_AUTH_FAILED;
        }

      /*
       * Reset begin string as this is the start of the basic cookie.  Now
       * scan until next white-space or new line to find end of cookie.
       */
      begin = end;
      ewsStringLengthSet(&begin, 0);
      match_c = ewsStringSearch(&begin
                               ,&end
                               ,(char *)" \t\r\n"
                               ,ewsStringSearchInclude
                               ,NULL);

#     if defined EW_CONFIG_OPTION_AUTH_BASIC
        /*
         * For each BASIC scheme authorization entry registered for this
         * realm, search for a matching cookie.
         */
      EWA_TASK_LOCK();          /* protect authorization list */

      for (authp = (EwsAuthP) realmp->basic.next;
           authp != (EwsAuthP) &realmp->basic;
           authp = (EwsAuthP) authp->link.next)
        {
          /*
           * If matching cookie was found, user is authenticated.
           */
          if (   ewsStringCompare( &begin
                                  ,authp->param.basic
                                  ,ewsStringCompareCaseSensitive
                                  ,NULL
                                  )
              && ( ewsStringLength( &begin ) == 0 )
              )
            {
              context->auth_handle = authp->handle;

#             ifdef EW_CONFIG_OPTION_SNMP_AGENT
              /*
               * point to snmp auth info for user access
               */
              context->snmpAuthHandle = realmp->snmp;
#             endif   /* EW_CONFIG_OPTION_SNMP_AGENT */

              /*
               * If not at front of list, move to front for efficient
               * caching (list becomes sorted in order of most recently
               * used).
               */
              if (authp != (EwsAuthP)realmp->basic.next)
                {
                  EWS_LINK_DELETE(&authp->link);
                  EWS_LINK_INSERT(&realmp->basic, &authp->link);
                }

              /*
               * Authorization has passed.  If ewaAuthVerifySecurity
               * is being used, skip to that state, else we're done.
               */
              EWA_TASK_UNLOCK();        /* end protect authorization entry */

              context->auth_state = ewsAuthStateOKBasic;

#             if defined EW_CONFIG_OPTION_AUTH_VERIFY
              context->substate.setup = ewsContextServeSetupAuth02BasicCheck;
#             else
              context->substate.setup = ewsContextServeSetupAuthDone;
#             endif
              return EWS_STATUS_OK;
            }
        }

      EWA_TASK_UNLOCK();        /* end protect authorization entry */

#     endif /* EW_CONFIG_OPTION_AUTH_BASIC */

/* START-STRIP-EMWEB-LIGHT */
        /*
         * if manual basic is enabled for this realm,
         * and the above automatic test failed,
         * then test against manual basic...
         */

#     if defined EW_CONFIG_OPTION_AUTH_MBASIC
      if (!EWS_LINK_IS_EMPTY(&realmp->mBasic))
        {
          /*
           * get a null terminated string with the base64 cookie
           * in it.  Will have to copy to allocated storage if
           * the cookie is non-contiguous
           */

          if (ewsStringIsContiguous(&begin))
            {
              params->basic.cookie = (char *) ewsStringData( &begin );
              params->basic.cookie[ewsStringLength( &begin )] = '\0';
              params->basic.freeCookie = FALSE;
            }
          else
            {
              params->basic.cookie =
                (char *) ewaAlloc( ewsStringLength( &begin ) + 1);
              if (params->basic.cookie == (char *) NULL)
                {
                  ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
                  EMWEB_WARN(("ewsAuthAuthenticate: no free memory\n"));
                  return EWS_STATUS_AUTH_FAILED;
                }
              params->basic.freeCookie = TRUE;
              ewsStringCopyOut( params->basic.cookie
                               ,&begin, ewsStringLength( &begin ) );
            }

#         if defined EW_CONFIG_OPTION_AUTH_MBASIC_DECODE
          /*
           * decode the cookie to get the string values for username
           * and password.  Store these in allocated storage: the base64
           * algorithm ensures that the encoded string is larger than
           * its original, so a temporary buffer of length > the
           * encoded string will be more than long enough (add 1 for
           * string terminating null)
           */

          if ((params->basic.user =
               (char *) ewaAlloc( ewsStringLength( &begin ) + 1 ))
              == (char *)NULL)
            {
              ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
              EMWEB_WARN(("ewsAuthAuthenticate: no free memory\n"));
              return EWS_STATUS_AUTH_FAILED;
            }
          else
            {
              char *data = params->basic.cookie;  /* input data - encoded */
              unsigned char code;           /* base64 code from databyte */
              unsigned char result = 0;     /* decoded output char */
              uintf freebits = 8;           /* # free bits in result */
              uintf shiftbits, savebits;    /* for bit shifting */
              char *output = params->basic.user;  /* write output to username */

              params->basic.password = NULL;    /* indicates not set */

              while ((code = (unsigned char) *data++) != '=' && code)
                {
                  /*
                   * get base64 code from ecoded character
                   */
                  code = ((code >= 'A' && code <= 'Z')
                          ? code - 'A'
                          : ((code >= 'a' && code <= 'z')
                             ? code - 'a' + 26
                             : ((code >= '0' && code <= '9')
                                ? code - '0' + 52
                                : ((code == '+')
                                   ? 62
                                   : 63)
                                )
                             )
                          );

                  /*
                   * determine # bits that can be shifted from code
                   * into the result, and do so.
                   */

                  shiftbits = (freebits > 6) ? 6 : freebits;
                  freebits -= shiftbits;

                  result = (  (result << shiftbits)
                              | (code >> (savebits = (6 - shiftbits)))
                              );

                  /*
                   * If a complete char is built up (no more freebits)
                   * write it out to the password/source data buffer.
                   * Then move any remaining code bits into the now-cleared
                   * result register.
                   */

                  if (freebits == 0)
                    {
                      /*
                       * If the decoded char is ":", then we have just finished
                       * decoding the full username.  Null terminate it, then
                       * set the password pointer to the next character - this
                       * is the point in the buffer that the password will be
                       * decoded into.
                       */

                      if (   (':' == (char) result)
                          && (params->basic.password == NULL)
                             )
                        {
                          *output++ = '\0';
                          params->basic.password = output;
                        }
                      else
                        {
                          *output++ = (char) result;
                        }
                      /*
                       * now, if there are any leftover code bits, prime the
                       * empty result register with them.
                       */
                      freebits = 8 - (savebits);
                      result = code & ((1<<savebits) - 1);
                    }
                }
              *output = '\0';
              if (params->basic.password == NULL)
              {
                params->basic.password = output;
              }
              context->auth_state = ewsAuthStateAuthenticateMBasic;
            }
#         endif /*  EW_CONFIG_OPTION_AUTH_MBASIC_DECODE */

          /*
           * At this point, I'm all ready to run the manual
           * basic authentication state.  Move to this state.
           */
          context->substate.setup = ewsContextServeSetupAuth01BasicCheck;
          return EWS_STATUS_OK;
        }
#     endif /* EW_CONFIG_OPTION_AUTH_MBASIC */

/* END-STRIP-EMWEB-LIGHT */

      /*
       * if no matching basic authorization is registered, and manual basic
       * not used, then fail
       */
      ewaLogHook(context, EWS_LOG_STATUS_AUTH_FAILED);
      return EWS_STATUS_AUTH_FAILED;
    }
# endif /* EW_CONFIG_OPTION_AUTH_BASIC || EW_CONFIG_OPTION_AUTH_MBASIC */


  /* =========================================================
   * If the Authorization: header is for Digest authentication
   * =========================================================
   */

# if defined EW_CONFIG_OPTION_AUTH_DIGEST
  if (ewsStringCompare(&begin,"digest",ewsStringCompareCaseInsensitive,NULL))
    {
#     ifndef EW_CONFIG_OPTION_AUTH_NO_REALM_QUALIFIER
      const char *qualifier = ewaAuthRealmQualifier ();
#endif
      EwsString value, name, ewstr_url;
      EwsAuthP  user_authp;
#     ifdef _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT
      boolean qop = FALSE;
#     endif /* _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT */
      boolean algorithm = FALSE, freeUser = FALSE;
      char *user;

      /*
       * Initialize Authorization: parse state
       */
      ewsStringInit(&params->digest.username, EWA_NET_BUFFER_NULL);
      ewsStringInit(&params->digest.nonce, EWA_NET_BUFFER_NULL);
      ewsStringInit(&params->digest.digest_uri, EWA_NET_BUFFER_NULL);
      ewsStringInit(&params->digest.response, EWA_NET_BUFFER_NULL);
      ewsStringInit(&params->digest.cnonce, EWA_NET_BUFFER_NULL);
      ewsStringInit(&params->digest.nc, EWA_NET_BUFFER_NULL);

      /*
       * Reset parser
       */

      while (ewsStringLength(&begin))
        {
      /*
       * Parse name=value pairs until no more data
       */
          if (!ewsParseAttributeFromHeader( &begin
                                            ,&name
                                            ,&value
                                            ,ewsParsePair
                                            ,','
                                            ,ewsHdrUpdateYes))
            break;              /* no more pairs        */

          /*
           * Handle parameter
           */
          if (ewsStringCompare(&name
                               ,"username"
                               ,ewsStringCompareCaseInsensitive
                               ,NULL
                               ))
            {                                   /* save username for now */
                params->digest.username = value;
            }
          else if (ewsStringCompare(&name
                                    ,"realm"
                                    ,ewsStringCompareCaseInsensitive
                                    ,NULL
                                    ))
            {
                  /*
                  * Verify realm matches
                  */
                  if (!ewsStringCompare(&value
                                        ,realmp->realm
                                        ,ewsStringCompareCaseSensitive
                                        ,NULL))
                    {
                      ewaLogHook(context, EWS_LOG_STATUS_AUTH_REQUIRED);
                      context->auth_state = ewsAuthStateUninitialized;
                      return EWS_STATUS_AUTH_CHALLENGE;
                    }
#                 ifndef EW_CONFIG_OPTION_AUTH_NO_REALM_QUALIFIER
                  if (qualifier != NULL &&
                      !ewsStringCompare(&value
                                        ,qualifier
                                        ,ewsStringCompareCaseSensitive
                                        ,NULL))
                    {
                      ewaLogHook(context, EWS_LOG_STATUS_AUTH_FAILED);
                      context->auth_state= ewsAuthStateFailed;
                      return EWS_STATUS_AUTH_FAILED;
                    }
#                 endif
            }
          else if (ewsStringCompare(&name
                                    ,"nonce"
                                    ,ewsStringCompareCaseInsensitive
                                    ,NULL))
            {
              params->digest.nonce = value;        /* save nonce for now */
            }
          else if (ewsStringCompare(&name
                                    ,"uri"
                                    ,ewsStringCompareCaseInsensitive
                                    ,NULL))
            {
              params->digest.digest_uri = value;   /* save digest-uri for now */
            }
          else if (ewsStringCompare(&name
                                    ,"response"
                                    ,ewsStringCompareCaseInsensitive
                                    ,NULL))
            {
              params->digest.response = value; /* save challenge response for now */
            }
          else if (ewsStringCompare(&name
                                    ,"qop"
                                    ,ewsStringCompareCaseInsensitive
                                    ,NULL))
            {
#             ifdef _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT
              if (ewsStringCompare(&value
                                    ,"auth-int"
                                    ,ewsStringCompareCaseInsensitive
                                    ,NULL))
                {
                  qop = TRUE;
                }
#             endif /* _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT */
              if (ewsStringCompare(&value
                                    ,"auth-int"
                                    ,ewsStringCompareCaseInsensitive
                                    ,NULL))
                {
                  ewaLogHook(context, EWS_LOG_STATUS_BAD_REQUEST);
                  return EWS_STATUS_BAD_REQUEST;
                }
            }
          else if (ewsStringCompare(&name
                                    ,"cnonce"
                                    ,ewsStringCompareCaseInsensitive
                                    ,NULL))
            {
              params->digest.cnonce = value;      /* save cnonce for now */
            }
          else if (ewsStringCompare(&name
                                    ,"nc"
                                    ,ewsStringCompareCaseInsensitive
                                    ,NULL))
            {
              params->digest.nc = value;          /* save nonce count for now */
            }
          else if (ewsStringCompare(&name
                                    ,"algorithm"
                                    ,ewsStringCompareCaseInsensitive
                                    ,NULL))
            {
              if (ewsStringCompare(&value
                                    ,"MD5-sess"
                                    ,ewsStringCompareCaseInsensitive
                                    ,NULL))
                {
                  algorithm = TRUE;
                }
            }

        } /* end while */

      /*
       * Verify required fields are present.
       */
      if (ewsStringLength(&params->digest.username) == 0 ||
          ewsStringLength(&params->digest.digest_uri) == 0 ||
          ewsStringLength(&params->digest.nonce) == 0 ||
          ewsStringLength(&params->digest.nc) == 0 ||
          ewsStringLength(&params->digest.cnonce) == 0 ||
          ewsStringLength(&params->digest.response) == 0 ||
#         ifdef _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT
          (qop == FALSE) ||
#         endif /* _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT */
          (algorithm == FALSE))
        {
          ewaLogHook(context, EWS_LOG_STATUS_BAD_REQUEST);
          return EWS_STATUS_BAD_REQUEST;
        }

      context->auth_state = ewsAuthStateAuthenticateDigest;

      /*
       * ??? We must validate digest_uri.  This may be non-trivial as
       * we do not have enough data as to how proxies and browsers behave
       * here.  For now, we'll assume that it's okay.  This needs to be
       * fixed!  (Are fields URL encoded?  Is the querey string included?
       * Can there be a http://hostname prefix?  The original URI header
       * was already parsed into pieces and isn't easy to put back together
       * again for verbatim comparison.
       */
      ewstr_url = params->digest.digest_uri;
      if (!ewsStringCompare(&ewstr_url
                            ,context->rcved_url
                            ,ewsStringCompareCaseSensitive
                            ,NULL))
        {
          ewaLogHook(context, EWS_LOG_STATUS_BAD_REQUEST);
          return EWS_STATUS_BAD_REQUEST;
        }


      /* at this point, we've gathered all we need from the network
       * buffer
       */

      /*
       * Lets make sure this user is authorized for this realm
       *
       */
      /*
       * find authorization record for this user
       */
#if   defined EW_CONFIG_OPTION_AUTH_MDIGEST
      context->auth_mdigest = FALSE;
#     endif
      context->authp = NULL;
      if (ewsStringIsContiguous(&params->digest.username))
        {
          user = (char *)ewsStringData( &params->digest.username );
          user[ewsStringLength( &params->digest.username )] = '\0';
          freeUser = FALSE;
        }
      else
        {
          if ((user =
               (char *)ewaAlloc(ewsStringLength(&params->digest.username) + 1)
               ) == NULL
              )
            {
              ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
              EMWEB_WARN(("authBegin: no free memory\n"));
              context->auth_state= ewsAuthStateFailed;
              return EWS_STATUS_AUTH_FAILED;
            }
          freeUser = TRUE;
          ewsStringCopyOut( user
                            ,&params->digest.username
                            ,ewsStringLength( &params->digest.username ) );
        }
      user_authp = authFindUser(&(realmp->digest), user);
      if (user_authp == NULL)
        {
#         if defined EW_CONFIG_OPTION_AUTH_MDIGEST
            {
               context->auth_mdigest = TRUE;
            }
#         else
            {
               ewaLogHook(context, EWS_LOG_STATUS_AUTH_FAILED);
               context->auth_state= ewsAuthStateFailed;
               return EWS_STATUS_AUTH_FAILED;
             }
#         endif
        }
      else
        {
          EWA_TASK_LOCK();          /* protect authorization list */
          if (user_authp != (EwsAuthP)(EWS_LINK_HEAD(realmp->digest)))
            {
               EWS_LINK_DELETE(&user_authp->link);
               EWS_LINK_INSERT(&realmp->digest, &user_authp->link);
            }
          EWA_TASK_UNLOCK();        /* end protect authorization entry */
          context->auth_handle = user_authp->handle;

 #        ifdef EW_CONFIG_OPTION_SNMP_AGENT
          /*
           * point to snmp auth info for user access
           */
          context->snmpAuthHandle = realmp->snmp;
 #        endif   /* EW_CONFIG_OPTION_SNMP_AGENT */

          context->authp = user_authp;
        }
      if (freeUser == TRUE)
        {
          ewaFree(user);
        }

      /*
       * save realmp for use at the end of integrity digest
       */
      context->realmp = realmp;
      /*
       * Time to jump to the next state
       */
      context->substate.setup = ewsContextServeSetupAuthDigestNonceCheck;
      context->res_entity_body = FALSE;
      return EWS_STATUS_OK;
    }
  context->auth_state= ewsAuthStateFailed;
# endif /* EW_CONFIG_OPTION_AUTH_DIGEST */


  /* ====================================
   * if no authorization known, then fail
   * ====================================
   */

  ewaLogHook(context, EWS_LOG_STATUS_AUTH_FAILED);
  return EWS_STATUS_AUTH_FAILED;
}


#if defined (EW_CONFIG_OPTION_AUTH_MBASIC)
/*
 * authBasicState01
 *
 * The first substate for the Basic authentication scheme.  We enter this
 * routine only if we are doing the  Manual Basic scheme.  Call out to the
 * application to do the verification.
 */
static EwsStatus
authBasicState01( EwsContext context
                 ,EwsRealmP realmp
                 ,EwsAuthStateParams *params )
{
  boolean valid;

# if !defined( EW_CONFIG_OPTION_AUTH_MBASIC_DECODE )
  valid = ewaAuthCheckBasic( context,
                             realmp->realm,
                             params->basic.cookie,
                             NULL,
                             NULL );
# else      /* EW_CONFIG_OPTION_AUTH_MBASIC_DECODE */
  valid = ewaAuthCheckBasic( context,
                             realmp->realm,
                             params->basic.cookie,
                             params->basic.user,
                             params->basic.password );
# endif     /* EW_CONFIG_OPTION_AUTH_MBASIC_DECODE */


  /* if pending, then return status OK */

  if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
    {
      return EWS_STATUS_FATAL_ERROR;
    }

# ifdef EW_CONFIG_AUTH_SCHED
  if (context->schedulingState == ewsContextSuspended)
    {
      return EWS_STATUS_OK;
    }
  context->schedulingState = ewsContextScheduled;    /* ensure not resuming */
# endif


  /*
   * at this point, we're done with all allocated memory, free
   * it here if needed
   */

  if (params->basic.freeCookie) {
    ewaFree( params->basic.cookie );
    params->basic.freeCookie = FALSE; /* so we don't free again on context abort */
  }

  /*
   * now check return code
   */

  if (valid)
    {
      context->auth_handle = ((EwsAuthP)realmp->mBasic.next)->handle;

#     ifdef EW_CONFIG_OPTION_SNMP_AGENT
      /*
       * point to snmp auth info for user access
       */
      context->snmpAuthHandle = realmp->snmp;
#     endif   /* EW_CONFIG_OPTION_SNMP_AGENT */

      context->auth_state = ewsAuthStateOKMBasic;
#     if defined EW_CONFIG_OPTION_AUTH_VERIFY
      context->substate.setup = ewsContextServeSetupAuth02BasicCheck;
#     else
      context->substate.setup = ewsContextServeSetupAuthDone;
#     endif
      return EWS_STATUS_OK;
    }

  context->auth_state = ewsAuthStateFailed;
  ewaLogHook(context, EWS_LOG_STATUS_AUTH_FAILED);
  return EWS_STATUS_AUTH_FAILED;
}
#endif /* EW_CONFIG_OPTION_AUTH_MBASIC */


#if defined(EW_CONFIG_OPTION_AUTH_VERIFY)       \
    && (defined(EW_CONFIG_OPTION_AUTH_MBASIC)   \
        || defined(EW_CONFIG_OPTION_AUTH_BASIC) \
        )
/*
 * authBasicState02
 *
 * The second substate of the Basic authentication procedure. Only called if
 * ewaAuthVerifySecurity is used with Basic or MBasic.   If this test passes,
 * then the authentication state machine completes.
 */
static EwsStatus
authBasicState02( EwsContext context
                 ,EwsRealmP realmp
                 ,EwsAuthStateParams *params )
{
  boolean valid;
  EW_UNUSED( params );

  /*
   * Allow application to impose any proprietary
   * authorization procedures
   */
  valid = ewaAuthVerifySecurity( context, realmp->realm );

  /* if aborting, then fail */

  if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
    {
      return EWS_STATUS_AUTH_FAILED;
    }

# ifdef EW_CONFIG_AUTH_SCHED

  /* if pending, then return status OK */

  if (context->schedulingState == ewsContextSuspended)
    {
      return EWS_STATUS_OK;
    }
  context->schedulingState = ewsContextScheduled;   /* ensure not resuming */
# endif

  if (valid)
    {
      context->substate.setup = ewsContextServeSetupAuthDone;
      return EWS_STATUS_OK;
    }

  context->auth_state = ewsAuthStateFailed;
  ewaLogHook(context, EWS_LOG_STATUS_AUTH_FAILED);
  return EWS_STATUS_AUTH_FAILED;
}
#endif /* EW_CONFIG_OPTION_AUTH_VERIFY &&
          (EW_CONFIG_OPTION_AUTH_MBASIC || EW_CONFIG_OPTION_AUTH_BASIC) */



#if defined (EW_CONFIG_OPTION_AUTH_DIGEST)
/*
 * authDigestStateNonceCheck
 *
 * The first substate of the Digest authentication scheme.  Here
 * we validate the nonce.
 * Age out the nonce and setup the next nonce if needed.
 */

static EwsStatus
authIsNonceStale(EwsContext context, char *nonce)
{
  /*
   * is nonce stale?
   */
  MD5_CTX md5_ctx;
  uint8   md5[EWS_NONCE_VALUE_SIZE + 1];
  uintf   len;
  uintf   i;

  MD5Init(&md5_ctx);
  MD5Update(&md5_ctx, (uint8 *) &nonce[EWS_NONCE_INDEX],
          EWS_NONCE_VALUE_SIZE - EWS_NONCE_VERIFIER_SIZE);
  if (ews_state->secret != NULL)
    {
      EMWEB_STRLEN(len, ews_state->secret);
      MD5Update(&md5_ctx, (uint8 *) ews_state->secret, len);
    }
  ewsAuthMD5String((char *)md5, &md5_ctx);

  for (i = 0; i < EWS_NONCE_VERIFIER_SIZE; i++)
    {
      if (nonce[i] != md5[i])
        {
          ewaLogHook(context, EWS_LOG_STATUS_AUTH_FAILED);
          context->auth_state= ewsAuthStateFailed;
          return EWS_STATUS_AUTH_FAILED;
        }
    }
  ewaLogHook(context, EWS_LOG_STATUS_AUTH_STALE);
  context->nonce_stale = TRUE;
  return EWS_STATUS_AUTH_CHALLENGE;
}

static EwsStatus
authDigestStateNonceCheck( EwsContext context
                  ,EwsRealmP realmp
                  ,EwsAuthStateParams *params)
{
  uint8 nybble;
  uintf i, nonce_len;
  EwsNonceP noncep;
  EwsNonceP next_noncep;
  char *nonce;
  boolean flag, nonce_flag = FALSE;
  EwsString ewstr_save;
  uint32 nc;
  EwsStatus status = EWS_STATUS_OK, nonce_status = EWS_STATUS_AUTH_FAILED;
  EwaAuthNonceStatus app_nonce_status;



  if (ewsStringIsContiguous(&params->digest.nonce))
    {
      nonce = (char *)ewsStringData( &params->digest.nonce );
      nonce[ewsStringLength( &params->digest.nonce )] = '\0';
    }
  else
    {
      if ((nonce =
           (char *)ewaAlloc(ewsStringLength(&params->digest.nonce) + 1)
           ) == NULL
          )
        {
          ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
          EMWEB_WARN(("ewsAuthAuthenticate: no free memory\n"));
          context->auth_state= ewsAuthStateFailed;
          return EWS_STATUS_AUTH_FAILED;
        }
      ewsStringCopyOut( nonce
                        ,&params->digest.nonce
                        ,ewsStringLength( &params->digest.nonce ) );
      nonce_flag = TRUE;
    }
  /*
   * Lookup nonce.  We conveniently set the first two nybbles of the
   * nonce to be an index.
   * Perform range checking and compare values. We revalidate the nonce
   * even if we are resuming an authorization request, since the nonce may no
   * longer be valid on resumption.
   *
   * The nonce may have aged since last used so now we have to declare
   * it stale and re-challenge.
   *
   */
  nybble = (uint8) nonce[EWS_NONCE_INDEX];
  i = ((nybble >= '0' && nybble <= '9')?
       nybble - '0' :
       nybble + 10 - 'a'
       ) << 4;
  nybble = (uint8) nonce[EWS_NONCE_INDEX+1];
  i += (nybble >= '0' && nybble <= '9')? nybble - '0' : nybble + 10 - 'a';
  if (i > EWS_NONCE_LIST_LEN)
    {
      ewaLogHook(context, EWS_LOG_STATUS_AUTH_FAILED);
      if (nonce_flag == TRUE)
        {
          ewaFree(nonce);
        }
      context->auth_state= ewsAuthStateFailed;
      return EWS_STATUS_AUTH_FAILED;
    }

  context->noncep = (EwsNonceP)NULL;
  noncep = authNonceFindRecord(i);
  /*
   * NOTE: noncep will never be NULL if everything works as should.
   *
   * at init time we created record for each potential index
   */

#if defined(EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE)
  /*
   * if there was a password change, the request needs to be challenged
   */
  if (noncep->pswd_change_flag)
    {
      if (nonce_flag == TRUE)
        {
          ewaFree(nonce);
        }
      noncep->pswd_change_flag = FALSE;
      ewaLogHook(context, EWS_LOG_STATUS_PASSWORD_CHANGE);
      EMWEB_WARN(("\newsAuthAuthenticate: password change\n"));
      context->auth_state= ewsAuthStateFailed;
      return EWS_STATUS_AUTH_FAILED;
    }
#endif

  if (!(IS_BIT_SET(i, ews_state->nonce_index_bmap)))
    {
      /*
       * is nonce stale?
       */

      nonce_status = authIsNonceStale(context, nonce);
      if (nonce_flag == TRUE)
        {
          ewaFree(nonce);
        }
      return(nonce_status);
    }

  /*
   * verify nonce value
   */
  EMWEB_STRCMP(flag, nonce, noncep->nonce_value);
  if (flag)
    {
      /*
       * is nonce stale?
       */

      nonce_status = authIsNonceStale(context, nonce);
      if (nonce_flag == TRUE)
        {
          ewaFree(nonce);
        }
      return(nonce_status);
    }

  /*
   * Matching nonce found in cache.  Verify realm
   */
  if (noncep->realmp != realmp)
    {
      ewaLogHook(context, EWS_LOG_STATUS_AUTH_FAILED);
      if (nonce_flag == TRUE)
        {
          ewaFree(nonce);
        }
      context->auth_state= ewsAuthStateFailed;
      return EWS_STATUS_AUTH_FAILED;
    }
  /*
   * save MD(A1) in context
   */

  EMWEB_STRCPY(params->digest.md5A1Ptr, noncep->md5A1Sess);

  /*
   * Give application an opportunity to verify nonce is valid for context
   */
  app_nonce_status = ewaAuthNonceCheck(context
                                   ,realmp->realm
                                   ,&noncep->nonce_params
                                   ,noncep->count);
  /*
   * first check if the application suspened or aborted
   */
  if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
    {
      context->auth_state= ewsAuthStateFailed;
      if (nonce_flag == TRUE)
        {
          ewaFree(nonce);
        }
      return EWS_STATUS_AUTH_FAILED;
    }

# ifdef EW_CONFIG_AUTH_SCHED
  if (context->schedulingState == ewsContextSuspended)
    {
      if (nonce_flag == TRUE)
        {
          ewaFree(nonce);
        }
      return EWS_STATUS_OK;
    }
  context->schedulingState = ewsContextScheduled;  /* ensure not resuming */
# endif

  /* otherwise, check status of nonce */
  switch (app_nonce_status) {
  case ewaAuthNonceDenied:
    context->auth_state= ewsAuthStateFailed;
    EWS_LINK_DELETE(&noncep->link);
    EWS_LINK_INSERT(&ews_state->nonce_list, &noncep->link);
    CLEAR_BIT(noncep->index, ews_state->nonce_index_bmap);
    /*
     * next time there is a request with this nonce,
     * caller will be challenged with stale=true
     */
    noncep->status = app_nonce_status;
    status = EWS_STATUS_AUTH_FAILED;
    break;

  case ewaAuthNonceStale:
    context->nonce_stale = TRUE;
    EWS_LINK_DELETE(&noncep->link);
    EWS_LINK_INSERT(&ews_state->nonce_list, &noncep->link);
    CLEAR_BIT(noncep->index, ews_state->nonce_index_bmap);
    noncep->status = app_nonce_status;
    status = EWS_STATUS_AUTH_CHALLENGE;
    break;

  case ewaAuthNonceLastOK:
    /*
     * Generate new nonce
     */

    next_noncep = ewsAuthNonceCreate(context,
                                     &ews_state->nonce_list,
                                     ews_state->nonce_index_bmap,
                                     realmp);

    if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
      {
        if (nonce_flag == TRUE)
          {
             ewaFree(nonce);
           }
        noncep->status = app_nonce_status;
        return EWS_STATUS_ABORTED;
      }
#   ifdef EW_CONFIG_AUTH_SCHED
    if (context->schedulingState == ewsContextSuspended)
      {
        if (nonce_flag == TRUE)
          {
             ewaFree(nonce);
           }
        noncep->status = app_nonce_status;
        return EWS_STATUS_OK;
      }
    context->schedulingState = ewsContextScheduled;  /* ensure not resuming */
#   endif

    if (next_noncep == (EwsNonceP) NULL)
      {
        EMWEB_WARN(("authDigestStateNonceCheck: no resources for nonce\n"));
        if (nonce_flag == TRUE)
          {
            ewaFree(nonce);
          }
        noncep->status = app_nonce_status;
        return EWS_STATUS_NO_RESOURCES;
      }
    EMWEB_STRLEN(nonce_len, next_noncep->nonce_value);
    noncep->next_nonce_value = (char *)ewaAlloc(nonce_len + 1);
    if (noncep->next_nonce_value == NULL)
      {
        EMWEB_WARN(("authDigestStateNonceCheck: no resources for nonce\n"));
        if (nonce_flag == TRUE)
          {
             ewaFree(nonce);
           }
        noncep->status = app_nonce_status;
        return EWS_STATUS_NO_RESOURCES;
      }
    EMWEB_STRCPY(noncep->next_nonce_value, next_noncep->nonce_value);

  case ewaAuthNonceOK:
    /*
     * Verify nonce count
     */

    ewstr_save = params->digest.nc;
    if (!ewsStringGetInteger(&ewstr_save, 16, &nc) || nc == 0)
      {
        ewaLogHook(context, EWS_LOG_STATUS_BAD_REQUEST);
        status = EWS_STATUS_BAD_REQUEST;
        noncep->status = app_nonce_status;
        break;
      }

    if (nc > EWS_NONCE_MAX_CNT)
      {
        /*
         * stale nonce, challenge again
         */
        context->nonce_stale = TRUE;
        status = EWS_STATUS_AUTH_CHALLENGE;
        noncep->status = app_nonce_status;
        break;
      }
    else
      {
        if (IS_BIT_SET(nc, noncep->nc_bmap))
          {
            /*
             * stale nonce, challenge again
             */
            context->nonce_stale = TRUE;
            status = EWS_STATUS_AUTH_CHALLENGE;
            noncep->status = app_nonce_status;
            break;
          }
        else
          {
            SET_BIT(nc, noncep->nc_bmap);
            /*
             * push used nonce to the head of the list
             */
            EWA_TASK_LOCK();      /* protect nonce structures */
            EWS_LINK_DELETE(&noncep->link);
            EWS_LINK_PUSH(&ews_state->nonce_list, &noncep->link);
            EWA_TASK_UNLOCK();
            context->noncep = noncep;
            noncep->count++;          /* age nonce */
            context->substate.setup = ewsContextServeSetupAuthDigestHashA1;
            if (nc >= EWS_NONCE_OLD_THRESHOLD)
              {
                if (noncep->status != ewaAuthNonceLastOK)
                  {
                    /*
                     * nonce is just about stale (maybe, not if the sequence
                     * of requests with this nonce had nonce counts like:
                     * nc=1, nc=2, nc=31)
                     * so get next nonce for next_nonce attribute just like
                     * ewaAuthNonceLastOK case but only if nonce is not
                     already marked as ewaAuthNonceLastOK.
                     */

                   next_noncep =
                     ewsAuthNonceCreate(context,
                                        &ews_state->nonce_list,
                                        ews_state->nonce_index_bmap,
                                        realmp);

                   if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
                     {
                       if (nonce_flag == TRUE)
                         {
                           ewaFree(nonce);
                         }
                       return EWS_STATUS_ABORTED;
                     }
#                  ifdef EW_CONFIG_AUTH_SCHED
                   if (context->schedulingState == ewsContextSuspended)
                     {
                       if (nonce_flag == TRUE)
                         {
                           ewaFree(nonce);
                         }
                       return EWS_STATUS_OK;
                     }
                   context->schedulingState = ewsContextScheduled;  /* ensure not resuming */
#                  endif

                   if (next_noncep == (EwsNonceP) NULL)
                     {
                       EMWEB_WARN(("authDigestStateNonceCheck: no resources for nonce\n"));
                       if (nonce_flag == TRUE)
                         {
                           ewaFree(nonce);
                         }
                       return EWS_STATUS_NO_RESOURCES;
                     }
                   EMWEB_STRLEN(nonce_len, next_noncep->nonce_value);
                   noncep->next_nonce_value = (char *)ewaAlloc(nonce_len + 1);
                   if (noncep->next_nonce_value == NULL)
                     {
                       EMWEB_WARN(("authDigestStateNonceCheck: no resources for nonce\n"));
                       if (nonce_flag == TRUE)
                         {
                           ewaFree(nonce);
                         }
                       return EWS_STATUS_NO_RESOURCES;
                     }
                   EMWEB_STRCPY(noncep->next_nonce_value, next_noncep->nonce_value);
                   noncep->status = ewaAuthNonceLastOK;
                  }
              }
            else
              {
                noncep->status = app_nonce_status;
              }
          }
      }
    break;

  default:
    ewaLogHook(context, context->nonce_stale?
               EWS_LOG_STATUS_AUTH_STALE :
               EWS_LOG_STATUS_AUTH_FAILED );
    context->auth_state= ewsAuthStateFailed;
    status = EWS_STATUS_AUTH_FAILED;
    break;
  }

  if (nonce_flag == TRUE)
    {
      ewaFree(nonce);
    }
  return(status);

}

/*
 * authDigestStateHashA1
 *
 * The second substate of the Digest athentication scheme.
 * In this state we calculate A1 value  so at the end
 * we have params.digest.md5A1Ptr pointing to a value all ready
 * to use in request digest calculation.
 *
 * If manual digest, we call application to do the job.
 * Otherwise, we do it.
 *
 * Than we move to start MD5(A2) calculations (we need entire entity body
 * to finish) and move on to the integrity state.
 *
 */
static EwsStatus
authDigestStateHashA1( EwsContext context
                  ,EwsRealmP realmp
                  ,EwsAuthStateParams *params )
{
  /*
   * calculate hash of A1 ONLY if this is first request of the session,
   *
   * This is the way IE5 works:
   *
   *  cnonce of the first request after challenge is used to calculate H(A1)
   *  for the whole session.
   *
   *  cnonces in the following requests are used to calculate digest only.
   *
   */
  int len;
  EW_UNUSED(realmp);

  EMWEB_STRLEN(len, context->noncep->md5A1Sess);
  if (len == 0)
    {
#     ifdef EW_CONFIG_OPTION_AUTH_MDIGEST
      if (context->auth_mdigest)
        {
         /*
         * our first goal, get md5A1
         */

          boolean valid;
          boolean freeUser, freeNonce, freeCnonce;
          char *user, *nonce, *cnonce;
          char *A1Ptr = params->digest.md5A1Ptr;
#         ifdef EMWEB_SANITY
          uintf A1len;
#         endif /* EMWEB_SANITY */

          /*
           * involve application
           */
          if (ewsStringIsContiguous(&params->digest.username))
            {
              user = (char *)ewsStringData( &params->digest.username );
              user[ewsStringLength( &params->digest.username )] = '\0';
              freeUser = FALSE;
            }
          else
            {
              if ((user =
                   (char *)ewaAlloc(ewsStringLength(&params->digest.username) + 1)
                   ) == NULL
                  )
                {
                  ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
                  EMWEB_WARN(("authDigestStateHashA1: no free memory\n"));
                  context->auth_state= ewsAuthStateFailed;
                  return EWS_STATUS_AUTH_FAILED;
                }
              freeUser = TRUE;
              ewsStringCopyOut( user
                                ,&params->digest.username
                                ,ewsStringLength( &params->digest.username ) );
            }
          if (ewsStringIsContiguous(&params->digest.nonce))
            {
              nonce = (char *)ewsStringData( &params->digest.nonce );
              nonce[ewsStringLength( &params->digest.nonce )] = '\0';
              freeNonce = FALSE;
            }
          else
            {
              if ((nonce =
                   (char *)ewaAlloc(ewsStringLength(&params->digest.nonce) + 1)
                   ) == NULL
                  )
                {
                  ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
                  EMWEB_WARN(("authDigestStateHashA1: no free memory\n"));
                  context->auth_state= ewsAuthStateFailed;
                  if (freeUser)
                    {
                      ewaFree(user);
                    }
                  return EWS_STATUS_AUTH_FAILED;
                }
              freeNonce = TRUE;
              ewsStringCopyOut( nonce
                                ,&params->digest.nonce
                                ,ewsStringLength( &params->digest.nonce ) );
            }
          if (ewsStringIsContiguous(&params->digest.cnonce))
            {
              cnonce = (char *)ewsStringData( &params->digest.cnonce );
              cnonce[ewsStringLength( &params->digest.cnonce )] = '\0';
              freeCnonce = FALSE;
            }
          else
            {
              if ((cnonce =
                   (char *)ewaAlloc(ewsStringLength(&params->digest.cnonce) + 1)
                   ) == NULL
                  )
                {
                  ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
                  EMWEB_WARN(("authDigestStateHashA1: no free memory\n"));
                  context->auth_state= ewsAuthStateFailed;
                  if (freeUser)
                    {
                      ewaFree(user);
                    }
                  if (freeNonce)
                    {
                      ewaFree(nonce);
                    }
                  return EWS_STATUS_AUTH_FAILED;
                }
              freeCnonce = TRUE;
              ewsStringCopyOut( cnonce
                                ,&params->digest.cnonce
                                ,ewsStringLength( &params->digest.cnonce ) );
            }

          /*
           * have the application manually get the digest
           * A1 parameter MD5(MD5(user:realm:password):nonce:cnonce)
           */
          valid = ewaAuthDigestHash(context
                                    ,realmp->realm
                                    ,user
                                    ,nonce
                                    ,cnonce
                                    ,&A1Ptr);
          if (freeUser)
            {
              ewaFree(user);
            }
          if (freeNonce)
            {
              ewaFree(nonce);
            }
          if (freeCnonce)
            {
              ewaFree(cnonce);
            }
          /*
           * first check if the application suspened or aborted
           */
          if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
            {
              context->auth_state= ewsAuthStateFailed;
              return EWS_STATUS_AUTH_FAILED;
            }

#         ifdef EW_CONFIG_AUTH_SCHED
          if (context->schedulingState == ewsContextSuspended)
            {
              return EWS_STATUS_OK;
            }
          context->schedulingState = ewsContextScheduled; /* ensure not resuming */
#         endif

          /*
           * otherwise, check the return status
           */

          if (!valid)
            {
              ewaLogHook(context, EWS_LOG_STATUS_AUTH_FAILED);
              context->auth_state= ewsAuthStateFailed;
              return EWS_STATUS_AUTH_FAILED;
            }

#         ifdef EMWEB_SANITY
          EMWEB_STRLEN(A1len, A1Ptr);
          if (A1len !=  EW_MD5_HEX_SZ)
            {
              EMWEB_ERROR(("authBegin: bad return from ewaAuthDigestHash()\n"));
              return EWS_STATUS_AUTH_FAILED;
            }
#         endif /* EMWEB_SANITY */


#         ifdef EW_CONFIG_OPTION_SNMP_AGENT
          /*
           * point to snmp auth info for user access
           */
          context->snmpAuthHandle = realmp->snmp;
#         endif   /* EW_CONFIG_OPTION_SNMP_AGENT */
        }
      else
#     endif /* EW_CONFIG_OPTION_AUTH_MDIGEST */

        {
          /*
           * finish MD5(A1), add hash of nonce and cnonce
           */

          MD5_CTX md5_ctx;

          MD5Init(&md5_ctx);
          MD5Update(&md5_ctx
                    ,(uint8 *)context->authp->param.digest.md5
                    ,EW_MD5_HEX_SZ);
          MD5Update(&md5_ctx, (uint8 *) ":", 1);
          ewsAuthMD5Update(&md5_ctx, &params->digest.nonce);
          MD5Update(&md5_ctx, (uint8 *) ":", 1);
          ewsAuthMD5Update(&md5_ctx, &params->digest.cnonce);
          ewsAuthMD5String((char *)params->digest.md5A1Ptr, &md5_ctx);
        }

      EMWEB_STRCPY(context->noncep->md5A1Sess, params->digest.md5A1Ptr);
    }
  else
    {
      EMWEB_STRCPY(params->digest.md5A1Ptr, context->noncep->md5A1Sess);
    }

# ifdef _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT
  /*
   * If we have all the message data expected,  go to calculate MD5(A2)
   * otherwise start integrity digest.
   */
  if ((context->content_length == 0
#  if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
       && !context->chunked_encoding
#      endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */
      ) ||
      context->req_method == ewsRequestMethodGet ||
      context->req_method == ewsRequestMethodHead)
     {
       context->req_entity_body = FALSE;
       context->substate.setup = ewsContextServeSetupAuthDigestHashA2;
     }
   else
     {
       /*
        * ewsNetHTTPReceive() will continue updating
        * as new data arrives, and the digest will be verified before
        * posting a form or sending last data to CGI script.
        */
       context->req_entity_body = TRUE;
       context->md5_remaining = context->content_length;
       context->md5_entity_str = context->req_str_end;
       MD5Init(&context->md5_entity_ctx);
       context->auth_state= ewsAuthStateIntegrity;
       context->substate.setup = ewsContextServeSetupAuthDigestIntegrity;
     }
# endif /* _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT */

  /*
   * If we have all the message data expected,  go to calculate MD5(A2)
   */
  context->req_entity_body = FALSE;
  context->substate.setup = ewsContextServeSetupAuthDigestHashA2;
  return EWS_STATUS_OK;
}



/*
 * authDigestStateHashA2
 *
 * Finish MD(A2) process:
 *
 *   calculate MD5(method:digest-uri) or
 *             MD5(method:digest-uri: MD5(entity-body))
 *
 *   calculate request digest and check against one received from client.
 *
 */
static EwsStatus
authDigestStateHashA2( EwsContext context
                  ,EwsRealmP realmp
                  ,EwsAuthStateParams *params )
{
  /*
   * finish request digest calculations
   */
  char md5_tmp[EW_MD5_HEX_SZ + 1];
  char md5[EW_MD5_HEX_SZ + 1];
  MD5_CTX md5_ctx;
  uintf qop_len;
  boolean flag;
  EW_UNUSED(realmp);

  EWA_TASK_LOCK();      /* avoid conflict with ewsNetHTTPReceive() */

  /*
   * calculate MD5(A2) for request
   */
  MD5Init(&md5_ctx);
  ewsAuthMD5Update(&md5_ctx, &context->req_method_str);
  MD5Update(&md5_ctx, (uint8 *) ":", 1);
  ewsAuthMD5Update(&md5_ctx, &params->digest.digest_uri);
  if (context->req_entity_body)
    {
      MD5Update(&md5_ctx, (uint8 *) ":", 1);
      ewsAuthMD5String(md5_tmp, &context->md5_entity_ctx);
      MD5Update(&md5_ctx, (uint8 *)md5_tmp, EW_MD5_HEX_SZ);
    }
  ewsAuthMD5String(md5_tmp, &md5_ctx); /* md5 contains H(A2) */

  /*
   * calculate request digest =
   *
   * ifdef _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT
   *   MD5(MD5(A1):nonce:nonce_count:cnonce:auth-int:MD5(A2))
   * endif * _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT *
   *   MD5(MD5(A1):nonce:nonce_count:cnonce:auth:MD5(A2))
   */

  MD5Init(&md5_ctx);
  MD5Update(&md5_ctx, (uint8 *)params->digest.md5A1Ptr, EW_MD5_HEX_SZ);
  MD5Update(&md5_ctx, (uint8 *) ":", 1);
  ewsAuthMD5Update(&md5_ctx, &params->digest.nonce);
  MD5Update(&md5_ctx, (uint8 *) ":", 1);
  ewsAuthMD5Update(&md5_ctx, &params->digest.nc);
  MD5Update(&md5_ctx, (uint8 *) ":", 1);
  ewsAuthMD5Update(&md5_ctx, &params->digest.cnonce);
  MD5Update(&md5_ctx, (uint8 *) ":", 1);
# ifdef _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT
  EMWEB_STRLEN(qop_len, "auth-int");
  MD5Update(&md5_ctx, (uint8 *) "auth-int", qop_len);
# endif /* _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT */
  EMWEB_STRLEN(qop_len, "auth");
  MD5Update(&md5_ctx, (uint8 *) "auth", qop_len);
  MD5Update(&md5_ctx, (uint8 *) ":", 1);
  MD5Update(&md5_ctx, (uint8 *)md5_tmp, EW_MD5_HEX_SZ);
  ewsAuthMD5String(md5, &md5_ctx);     /* this is our request digest */


  EWA_TASK_UNLOCK();

  ewsStringCopyOut(md5_tmp, &params->digest.response, EW_MD5_HEX_SZ);
  EMWEB_STRCMP(flag, md5, md5_tmp);
  if (flag)
    {
      ewaLogHook(context, EWS_LOG_STATUS_AUTH_FORGERY);
      context->auth_state= ewsAuthStateFailed;
      return EWS_STATUS_AUTH_FAILED;
    }
  else
    {
#     ifdef _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT
      /*
       * request digest checked OK
       *
       * start calculating response digest. Will update as we go.
       */
      MD5Init(&context->res_md5_ctxt); /* this will be H(entity body)*/
      context->auth_state= ewsAuthStateOKDigestIntegrity;
#     if defined EW_CONFIG_OPTION_AUTH_VERIFY
      context->substate.setup = ewsContextServeSetupAuthDigestVerify;
#     else
      context->substate.setup = ewsContextServeSetupAuthDone;
#     endif
      return EWS_STATUS_OK;
#     endif /* _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT */
      /*
       * reuest digest checked OK
       */
      context->auth_state= ewsAuthStateOKDigest;
#     if defined EW_CONFIG_OPTION_AUTH_VERIFY
      context->substate.setup = ewsContextServeSetupAuthDigestVerify;
#     else
      context->substate.setup = ewsContextServeSetupAuthDone;
#     endif
      return EWS_STATUS_OK;
    }
}

/*
 * authDigestStateVerify
 *
 * The last state of the Digest authentication scheme.
 * Call out to the application verification routine if configured.
 * After that we are done.
 *
 */
# if defined EW_CONFIG_OPTION_AUTH_VERIFY
static EwsStatus
authDigestStateVerify( EwsContext context
                  ,EwsRealmP realmp
                  ,EwsAuthStateParams *params )
{
  /*
   * at this point, we allow the application to provide any sort of
   * "home grown" validation that it wants to.
   */
  boolean valid;
  EW_UNUSED(params);

  /*
   * Allow application to impose any proprietary
   * authorization procedures
   */
  valid = ewaAuthVerifySecurity( context, realmp->realm );

  /* if pending, then return status OK */

  if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
    {
      context->auth_state= ewsAuthStateFailed;
      return EWS_STATUS_AUTH_FAILED;
    }

# ifdef EW_CONFIG_AUTH_SCHED
  if (context->schedulingState == ewsContextSuspended)
    {
      return EWS_STATUS_OK;
    }
  context->schedulingState = ewsContextScheduled;    /* ensure not resuming */
# endif  /* EW_CONFIG_AUTH_SCHED */

  if (!valid)
    {
      ewaLogHook(context, EWS_LOG_STATUS_AUTH_FAILED);
      context->auth_state= ewsAuthStateFailed;
      return EWS_STATUS_AUTH_FAILED;
    }

  context->substate.setup = ewsContextServeSetupAuthDone;
  return EWS_STATUS_OK;
}
# endif  /* EW_CONFIG_OPTION_AUTH_VERIFY */

/*
 * Generate AuthenticationInfo: trailer
 *
 */
void ewsAuthenticationInfoHeadOrTrail(EwsContext context)
{
  char md5[EW_MD5_HEX_SZ + 1];
  char md5_tmp[EW_MD5_HEX_SZ + 1];
  char md5A2[EW_MD5_HEX_SZ + 1];
  MD5_CTX md5_ctx;
  MD5_CTX md5A2_ctx;
  char *cnonce, *nc;
  uintf nonce_len, cnonce_len, nc_len;
  boolean nc_flag = FALSE, cnonce_flag = FALSE;


  if (ewsStringIsContiguous(&context->authStateParams.digest.cnonce))
    {
      cnonce = (char *)ewsStringData( &context->authStateParams.digest.cnonce );
      cnonce[ewsStringLength( &context->authStateParams.digest.cnonce )] = '\0';
    }
  else
    {
      if ((cnonce =
           (char *)ewaAlloc(ewsStringLength(&context->authStateParams.digest.cnonce) + 1)
           ) == NULL
          )
        {
          ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
          EMWEB_WARN(("ewsAuthenticationInfoHeadOrTrail: no free memory\n"));
          return;
        }
      cnonce_flag = TRUE;
      ewsStringCopyOut( cnonce
                        ,&context->authStateParams.digest.cnonce
                        ,ewsStringLength( &context->authStateParams.digest.cnonce ) );
    }

  if (ewsStringIsContiguous(&context->authStateParams.digest.nc))
    {
      nc = (char *)ewsStringData( &context->authStateParams.digest.nc );
      nc[ewsStringLength( &context->authStateParams.digest.nc )] = '\0';
    }
  else
    {
      if ((nc =
           (char *)ewaAlloc(ewsStringLength(&context->authStateParams.digest.nc) + 1)
           ) == NULL
          )
        {
          ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
          EMWEB_WARN(("ewsAuthenticationInfoHeadOrTrail: no free memory\n"));
          if (cnonce_flag == TRUE)
            {
              ewaFree(cnonce);
            }
          return;
        }
      nc_flag = TRUE;
      ewsStringCopyOut( nc
                        ,&context->authStateParams.digest.nc
                        ,ewsStringLength( &context->authStateParams.digest.nc ) );
    }

  (void) ewsStringCopyIn(&context->res_str_end,
                         "Authentication-info: nextnonce=\"");
  if (context->noncep->status == ewaAuthNonceLastOK)
    {
      (void) ewsStringCopyIn(&context->res_str_end,
                             context->noncep->next_nonce_value);
    }
  else
    {
      (void) ewsStringCopyIn(&context->res_str_end,
                             context->noncep->nonce_value);
    }
  (void) ewsStringCopyIn(&context->res_str_end, "\"");

/*****************************************************************************/
# ifdef _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT
  /*
   * qop="auth-int",
   */
  (void) ewsStringCopyIn(&context->res_str_end, ", qop=\"");
  (void) ewsStringCopyIn(&context->res_str_end, "auth-int");
  (void) ewsStringCopyIn(&context->res_str_end, "\"");
# endif /* _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT */
/*****************************************************************************/
  /*
   * qop="auth",
   */
  (void) ewsStringCopyIn(&context->res_str_end, ", qop=\"");
  (void) ewsStringCopyIn(&context->res_str_end, "auth");
  (void) ewsStringCopyIn(&context->res_str_end, "\"");
  /*
   * nonce-count="nonce_count_value",
   */
  (void) ewsStringCopyIn(&context->res_str_end, ", nonce-count=");
  (void) ewsStringCopyIn(&context->res_str_end, nc);
  /*
   * cnonce="cnonce_value",
   */
  (void) ewsStringCopyIn(&context->res_str_end, ", cnonce=\"");
  (void) ewsStringCopyIn(&context->res_str_end, cnonce);
  (void) ewsStringCopyIn(&context->res_str_end, "\"");
  /*
   * response digest
   *
   * calculate MD5(A2) for response
   *
   * ifdef _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT
   *   H(A2) =  MD5(:digest_uri:H(entity_body))
   * endif * _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT *
   *
   *   H(A2) = MD5(:digest_uri)
   */
  MD5Init(&md5A2_ctx);
  MD5Update(&md5A2_ctx, (uint8 *) ":", 1);
  ewsAuthMD5Update(&md5A2_ctx, &context->authStateParams.digest.digest_uri);
  if (context->res_entity_body)
    {
      ewsAuthMD5String(md5_tmp, &context->res_md5_ctxt);
      MD5Update(&md5A2_ctx, (uint8 *)md5_tmp, EW_MD5_HEX_SZ);
    }
  ewsAuthMD5String(md5A2, &md5A2_ctx); /* md5 contains H(A2) */

  /*
   * calculate response digest
   */
  MD5Init(&md5_ctx);
  MD5Update(&md5_ctx, (uint8 *)(context->authStateParams.digest.md5A1Ptr),
            EW_MD5_HEX_SZ);
  MD5Update(&md5_ctx, (uint8 *) ":", 1);
  EMWEB_STRLEN(nonce_len, context->noncep->nonce_value);
  MD5Update(&md5_ctx, (uint8 *)(context->noncep->nonce_value), nonce_len);
  MD5Update(&md5_ctx, (uint8 *) ":", 1);
  EMWEB_STRLEN(nc_len, nc);
  MD5Update(&md5_ctx, (uint8 *)nc, nc_len);
  MD5Update(&md5_ctx, (uint8 *) ":", 1);
  EMWEB_STRLEN(cnonce_len, cnonce);
  MD5Update(&md5_ctx, (uint8 *)cnonce, cnonce_len);
  MD5Update(&md5_ctx, (uint8 *) ":", 1);
  MD5Update(&md5_ctx, (uint8 *)"auth", 4);
  MD5Update(&md5_ctx, (uint8 *) ":", 1);
  MD5Update(&md5_ctx, (uint8 *)md5A2, EW_MD5_HEX_SZ);
  ewsAuthMD5String(md5, &md5_ctx);          /* md5 is our response digest */

  (void) ewsStringCopyIn(&context->res_str_end, "\", rspauth=\"");
  (void) ewsStringCopyIn(&context->res_str_end, md5);
  (void) ewsStringCopyIn(&context->res_str_end, "\"");
  if (!context->send_auth_info_hdr)
    {
      (void) ewsStringCopyIn(&context->res_str_end, "\r\n");
    }

  if (cnonce_flag == TRUE)
    {
      ewaFree(cnonce);  /*LB bug was nc, we never tested with  */
    }
  if (nc_flag == TRUE)
    {
      ewaFree(nc);
    }
  return;
}

/*
 * Function ewsConvertToHexChar(dst, value);
 * convert the integer, (value), into the correct hex char (dst).
 */

void ewsConvertToHexChar( char *dst, int value)
{
  if (value >9) {
    *dst='a'+value-10;
  }
  else {
    *dst = value +'0';
  }
}

#if defined(EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE)


/**********************************************************************
 *
 * Password Change Support Functions
 *
 * Password change happens in the ewaFormSubmit_passwordChange()
 * function. Prototype of this function is created by EmWeb compiler
 * based on the password change form in the archive.
 *
 * !!!!! NOTE, that if the compiler is executed with -o prefix option,
 * the above inclusion * of ew_proto.h will have to be changed
 * to prefix_proto.h
 *
 **********************************************************************/
void ewsAuthHashMD5( const char *user
                    ,const char *realm
                    ,const char *password
                    ,char **digest )
{
  MD5_CTX md5_ctxt;
  char    md5[EW_MD5_HEX_SZ + 1];
  uintf   len;

  /*
   * and generate MD5(user:realm:password)
   */
  MD5Init(&md5_ctxt);
  EMWEB_STRLEN(len, user);
  MD5Update(&md5_ctxt, (uint8 *)user, len);
  MD5Update(&md5_ctxt, (uint8 *) ":", 1);
  EMWEB_STRLEN(len, realm);
  MD5Update(&md5_ctxt, (uint8 *)realm, len);
  MD5Update(&md5_ctxt, (uint8 *) ":", 1);
  EMWEB_STRLEN(len, password);
  MD5Update(&md5_ctxt,(uint8 *)password, len);
  ewsAuthMD5String(md5, &md5_ctxt);

  EMWEB_STRCPY(*digest, md5);
  return;
}

/*
 * Serve password change form
 */
void ewsFormServe_passwordChange
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
  )
{
  uint16 len;
  EwaAuthNonce nonce_params;
  MD5_CTX md5_ctx;
  char psnonce[EW_MD5_HEX_SZ + 1];

  EW_UNUSED(value_hashchkp);
  EW_UNUSED(value_passwdchkp);
  EW_UNUSED(value_maskedsecretp);
  EW_UNUSED(status_hashchkp);
  EW_UNUSED(status_passwdchkp);
  EW_UNUSED(status_maskedsecretp);

  EMWEB_STRLEN(len, context->authp->param.digest.user);

  /* set new user to the current user */
  *value_userp = ewaAlloc(len+1);
  *status_userp = EW_FORM_INITIALIZED | EW_FORM_DYNAMIC;
  EMWEB_STRCPY(*value_userp, context->authp->param.digest.user);

  /* set setting user to current user */
  *value_setting_userp = ewaAlloc(len+1);
  *status_setting_userp = EW_FORM_INITIALIZED | EW_FORM_DYNAMIC;
  EMWEB_STRCPY(*value_setting_userp, context->authp->param.digest.user);

  /* set psnonce  */
  ewaAuthNonceCreate(context, context->authp->realmp->realm, &nonce_params);
  MD5Init(&md5_ctx);
  MD5Update(&md5_ctx ,(uint8 *) &nonce_params ,sizeof(EwaAuthNonce));
  ewsAuthMD5String(psnonce, &md5_ctx);
  *value_psnoncep = ewaAlloc(EW_MD5_HEX_SZ + 1);
  *status_psnoncep = EW_FORM_INITIALIZED | EW_FORM_DYNAMIC;
  EMWEB_STRCPY(*value_psnoncep, psnonce);

  /* set realm */
  EMWEB_STRLEN(len, context->authp->realmp->realm);
  *value_realmp = ewaAlloc(len+1);
  *status_realmp = EW_FORM_INITIALIZED | EW_FORM_DYNAMIC;
  EMWEB_STRCPY(*value_realmp, context->authp->realmp->realm);

  return;
}
/*
 * Function getNewSecret(oldSecret, maskedSecet, newSecretp);
 * calculate the newsecret - (xor of oldSecret and maskedSecret)
 * Get the index value for each character in each input.
 * (Ex for char 'c', index=12, for char '9', index = 9)
 * Then calculate the xor value of the two inputs.
 * sprintf the output to an outputstring as ascii hex value.
 * return it in newSecretp
 * NOTE: the values are expected to be between ascii values '0'-'f'
 */

static void getNewSecret(const char *oldSecret, const char *maskedSecret,char *newSecretp)
{
  /*  char finalString[EW_MD5_HEX_SZ+1]; */
  char outputstring[EW_MD5_HEX_SZ+1];
  char x;
  char y;
  int i;
  int x_indx;
  int y_indx;
  int output;
  int len;

  EMWEB_STRLEN(len, oldSecret);
  EMWEB_MEMSET(outputstring
               ,0
               , sizeof(outputstring));
  /* the lengths of both strings should be the same !    If they are not, return
     0 as newsecret.
  */
  EMWEB_STRLEN(i, maskedSecret);
  if (i !=len)
    {
      EMWEB_STRCPY(newSecretp, outputstring);
    }
  else
    {
      for (i=0; i <len; i++)
        {
          x=*oldSecret++;
          y=*maskedSecret++;
          /*now, find the index for chars x, y */
          x_indx =  x >'9'? x-'a' +10: x-'0';
          y_indx =  y >'9'? y-'a' +10: y-'0';
          output = x_indx^y_indx;
          /* print it as the hex value in the string*/
          /*          sprintf(&outputstring[i],"%x",output); */
          /* bugfix 3134 - do not use sprintf() */
          ewsConvertToHexChar(&outputstring[i],output);
        }
      outputstring[EW_MD5_HEX_SZ]='\0'; /* terminate the string */
      EMWEB_STRCPY(newSecretp, outputstring);
    }
} /* end FUNCTION  getNewSecret */

/*
 * Submit password change form
 */

const char *ewsFormSubmit_passwordChange
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
  )
{
  boolean bval;
  /*  boolean newsecret_flg = FALSE; */
  char *newsecret = NULL;
  const char *oldsecret, *maskedsecret;
  char passwdchk[EW_MD5_HEX_SZ+1];
  char *passwdchkP = passwdchk;
  EwaAuthHandle auth_handle;
  EwsAuthP  user_authp;

  EW_UNUSED(value_hashchkp);
  EW_UNUSED(status_hashchkp);

  if (   ewsContextAuthState(context) == ewsAuthStateOKDigest
      && (*status_setting_userp & EW_FORM_RETURNED)
      && (*status_userp & EW_FORM_RETURNED)
      && (*status_maskedsecretp & EW_FORM_RETURNED)
      && (*status_psnoncep & EW_FORM_RETURNED)
      && (*status_passwdchkp & EW_FORM_RETURNED)
      && (*status_realmp & EW_FORM_RETURNED)
     )
    {
      /*
       * this is the only acceptable authentication state for
       * password change submittion
       */

      /*
       * !!!!!!!!!!! hash check
       */

      /*
       * verify setting_user
       */
      EMWEB_STRCMP(bval,
                   *value_setting_userp,
                   context->authp->param.digest.user);
      if (!bval)
        {
          /*
           * setting user is OK,
           * retrive new hash value = olsecret XOR maskedvalue
           * NOTE: oldsecret is for *value_userp, which DOES NOT
           * have to be the same as setting_user whose auth record is
           * in context.
           * So, get 'destination' user's auth record first.
           */

          user_authp = authFindUser(&(context->authp->realmp->digest),
                                    *value_userp);

          if (user_authp != NULL)
            {
              /*
               * user to be updated is registered within this realm
               */

              maskedsecret = *value_maskedsecretp;
              /* old secret should be the settinguser:realm:oldpasswd */
              oldsecret =context->authp->param.digest.md5;

              newsecret = ewaAlloc(EW_MD5_HEX_SZ+1);
              if (newsecret != NULL)
                {
                  /*                  newsecret_flg = TRUE; */
                  /* get the new secret value based on the oldsecret and masked secret*/
                  getNewSecret(oldsecret,maskedsecret,newsecret);
                  /*
                   * verify passwdchk
                   */
                  ewsAuthHashMD5(*value_psnoncep,
                                 oldsecret,
                                 newsecret,
                                 (char **)&passwdchkP);
                  EMWEB_STRCMP(bval, *value_passwdchkp, passwdchk);
                  if (!bval)
                    {
                      /*
                       * let's tell application we have password change situation
                       */
                      auth_handle = ewaAuthDigestChange(context,
                                                        ewsContextAuthHandle(context),
                                                        *value_realmp,
                                                        *value_userp,
                                                        newsecret);
                      /* If we were suspended during the ewaAuthDigestChange()
                         call, it may be that the handle was not set correctly.
                         A NULL handle indicates that we should *NOT*
                         update the database.
                         In that case, we should just default to NOT
                         updating the database.  LLB.
                      */
#   ifdef  EW_CONFIG_OPTION_SCHED_SUSP_RES
                      if (context->schedulingState == ewsContextSuspended)
                        {
                          if (newsecret != NULL)
                            {
                              ewaFree(newsecret);
                            }
                          return(NULL);
                        }
                      /* reset state to scheduled (may be resuming) */
                      context->schedulingState=ewsContextScheduled;
#endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
                      if (auth_handle != EWA_AUTH_HANDLE_NULL)
                        {
                          EwsNonceP  next;
                          int len,i;

                          /*
                           * update authentication data base,
                           * and set password change flag for each nonce
                           */
                          EWA_TASK_LOCK();
                          user_authp->handle = auth_handle;
                          EMWEB_STRCPY(user_authp->param.digest.md5, newsecret);
                          next = (EwsNonceP)EWS_LINK_HEAD(ews_state->nonce_list);
                          for (i = 1; i <= EWS_NONCE_LIST_LEN; i++)
                            {
                              EMWEB_STRLEN(len, next->md5A1Sess);
                              if (len != 0)
                                {
                                  next->pswd_change_flag = TRUE;
                                }
                              next = (EwsNonceP)(EWS_LINK_NEXT(next->link));
                            }
                          EWA_TASK_UNLOCK();
                        }
                    }
                }
            }
        }
    }
  if (newsecret != NULL)
    {
      ewaFree(newsecret);
    }
  return(NULL);
}
#endif /* EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE */

#endif  /* EW_CONFIG_OPTION_AUTH_DIGEST */

#else /* EW_CONFIG_OPTION_AUTH  - DO NOT ADD CODE BELOW THIS LINE!!! */

/* ANSI C forbids empty source file, so just stub this */
int ews_no_authorization;

#endif /* EW_CONFIG_OPTION_AUTH */
