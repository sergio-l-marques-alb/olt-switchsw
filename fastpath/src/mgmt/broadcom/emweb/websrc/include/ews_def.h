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
 * EmWeb/Server public definitions
 *
 */
#ifndef _EWS_DEF_H_
#define _EWS_DEF_H_

typedef const char *EwsConstCharP;

/*
 * --no-strict-const handling affects compiler-generated types to be compatible
 * with pre-6.0.0 which now, by default, add "const" to return values for
 * ewaFormSubmit_XXX, ewaNamespaceGet_XXX, ewaNamespaceSetFinish_XXX and
 * for form field values for text, password, textarea, and select fields.
 * The following macro resolves at run-time to "" if the preprocessor macro
 * EW_CONFIG_OPTION_NO_STRICT_CONST is defined, else "const".
 */
#ifdef EW_CONFIG_OPTION_NO_STRICT_CONST
#  define EW_NSC /* const */
#else
#  define EW_NSC const
#endif

#ifndef EMWEB_RELEASE
#  define EMWEB_RELEASE "X" /* "R6_2_0" */
#endif
#ifndef EMWEB_ID
#  define EMWEB_ID "Web Server" /* "Virata-EmWeb/"EMWEB_RELEASE */
#else /* ! EMWEB_ID */
/*
 * For server regression tests, -DEMWEB_ID=REGRESSION_TEST_ID
 * The indirection in the definition avoids the unpleasantness of having
 * to quote the dollar signs through multiple levels of makefiles
 */
#  define REGRESSION_TEST_ID 0
#  if EMWEB_ID == REGRESSION_TEST_ID
#    undef EMWEB_ID
#    define EMWEB_ID "Agranat-EmWeb/_TEST_ID_"
#  endif
#endif /* EMWEB_ID */

extern const char *EmWebId; /* defined in ews.c */

#ifndef EMWEB_OS
#  define EMWEB_OS "Unknown/0.0"
#endif /* EMWEB_OS */

/*
 * Status codes returned to the application by EmWeb/Server
 */
typedef enum EwsStatus_e
  {
    EWS_STATUS_OK,
    EWS_STATUS_BAD_MAGIC,
    EWS_STATUS_BAD_VERSION,
    EWS_STATUS_ALREADY_EXISTS,
    EWS_STATUS_NO_RESOURCES,
    EWS_STATUS_IN_USE,
    EWS_STATUS_NOT_REGISTERED,
    EWS_STATUS_NOT_CLONED,
    EWS_STATUS_NOT_FOUND,
    EWS_STATUS_AUTH_FAILED,
    EWS_STATUS_BAD_STATE,
    EWS_STATUS_BAD_REALM,
    EWS_STATUS_FATAL_ERROR,
    EWS_STATUS_ABORTED,
    EWS_STATUS_AUTH_CHALLENGE,
    EWS_STATUS_BAD_REQUEST,
    EWS_STATUS_BAD_SYNTAX,
    EWS_STATUS_AUTH_INCOMPLETE
  } EwsStatus;

/*
 * Status codes returned to EmWeb/Server by the application
 */
typedef enum EwaStatus_e
  {
    EWA_STATUS_OK,

#   ifdef EW_CONFIG_OPTION_SCHED
    EWA_STATUS_OK_YIELD,
#   endif

    EWA_STATUS_ERROR

  } EwaStatus;

/*
 * EmWeb/Server request context handle
 */
typedef struct EwsContext_s * EwsContext;
#define EWS_CONTEXT_NULL        ((EwsContext) NULL)

#ifdef EWA_LOG_HOOK
/*
 * Status codes used for logging requests
 */
typedef enum EwsLogStatus_e
  {
    /*
     * 200 Request accepted
     */
    EWS_LOG_STATUS_OK,

    /*
     * Request dispositions (after successful request)
     */
    EWS_LOG_STATUS_NO_CONTENT,                  /* 204 no-op form or imagemap */
    EWS_LOG_STATUS_MOVED_PERMANENTLY,           /* 301 link */
    EWS_LOG_STATUS_MOVED_TEMPORARILY,           /* 302 redirect */
    EWS_LOG_STATUS_SEE_OTHER,                   /* 303 see other */
    EWS_LOG_STATUS_NOT_MODIFIED,                /* 304 not modified since */

    /*
     * 401 Unauthorized
     */
    EWS_LOG_STATUS_AUTH_FAILED,                 /* authorization failed */
    EWS_LOG_STATUS_AUTH_FORGERY,                /* bad message checksum */
    EWS_LOG_STATUS_AUTH_STALE,                  /* authorization nonce stale */
    EWS_LOG_STATUS_AUTH_REQUIRED,               /* authorization not present */
    EWS_LOG_STATUS_AUTH_DIGEST_REQUIRED,        /* message digest not present */

    /*
     * 400 Bad Request
     */
    EWS_LOG_STATUS_BAD_REQUEST,                 /* HTTP parse error */
    EWS_LOG_STATUS_BAD_FORM,                    /* form data parse error */
    EWS_LOG_STATUS_BAD_IMAGEMAP,                /* imagemap query parse error */

    /*
     * Additional errors
     */
    EWS_LOG_STATUS_NOT_FOUND,                   /* 404 not found or hidden */
    EWS_LOG_STATUS_METHOD_NOT_ALLOWED,          /* 405 method not allowed */
    EWS_LOG_STATUS_LENGTH_REQUIRED,             /* 411 length required */
    EWS_LOG_STATUS_PRECONDITION_FAILED,         /* 412 Precondition Failed */
    EWS_LOG_STATUS_UNAVAILABLE,                 /* 503 aborted Document Fault */
    EWS_LOG_STATUS_NOT_IMPLEMENTED,             /* 501 bad method for URL */
    EWS_LOG_STATUS_NO_RESOURCES,                /* 500 insufficient resources */
    EWS_LOG_STATUS_INTERNAL_ERROR,              /* 500 internal error */
    EWS_LOG_STATUS_PASSWORD_CHANGE              /* there was a password change */

  } EwsLogStatus;

/*
 * ewaLogHook
 * Application logging hook
 *
 * context - context of request being logged
 * status  - logging status
 */
#ifndef ewaLogHook
extern void ewaLogHook(EwsContext context, EwsLogStatus status);
#endif /* ewaLogHook */

#else /* EWA_LOG_HOOK */
#define ewaLogHook(context, status)
#endif /* EWA_LOG_HOOK */

/*
 * EmWeb/Server archive descriptor
 */
typedef struct EwsArchive_s * EwsArchive;

/*
 * EmWeb/Server document handle
 */
typedef struct EwsDocument_s * EwsDocument;
#define EWS_DOCUMENT_NULL       ((EwsDocument) NULL)

/*
 * EmWeb/Server opaque read-only data handle
 */
typedef const uint8 EwsArchiveData[];

/*
 * EmWeb/Server authorization handle
 */
typedef struct EwsAuthHandle_s * EwsAuthHandle;
#define EWS_AUTH_HANDLE_NULL    ((EwsAuthHandle) NULL)

#ifdef EW_CONFIG_OPTION_FORM
/*
 * Application form interface constants for status byte
 */
#define EW_FORM_INITIALIZED     0x01
#define EW_FORM_DYNAMIC         0x02
#define EW_FORM_RETURNED        0x10
#define EW_FORM_PARSE_ERROR     0x20
#define EW_FORM_FILE_ERROR      0x20        /* equivalent to parse error */

#define EW_FORM_BOOLEAN_TRUE( form, field ) \
  ( (((form)->status.field & EW_FORM_RETURNED) == EW_FORM_RETURNED) \
    && (form)->value.field )

/*
 * EmWeb/Server form element types
 */
#ifdef EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING
typedef struct EwsFormFieldHexString_s
  {
    uint32 length;
    uint8 *datap;
  } EwsFormFieldHexString, *EwsFormFieldHexStringP;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING */

#if (defined(EW_CONFIG_OPTION_FIELDTYPE_IEEE_MAC)       \
     || defined(EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC)    \
     || defined(EW_CONFIG_OPTION_FIELDTYPE_STD_MAC)     \
     )
typedef uint8 EwsFormFieldMAC[6];
#endif /* MAC */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_OID
typedef struct EwsFormFieldObjectID_s
  {
    uint32 length;      /* number of uint32's in datap */
    uint32 *datap;      /* array of uint32's representing OID */
  } EwsFormFieldObjectID, *EwsFormFieldObjectIDP;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_OID */

typedef uint8 EwsFormFieldUUID[16], (*EwsFormFieldUUIDP)[16];

#if defined(EW_CONFIG_OPTION_FIELDTYPE_RADIO) ||        \
    defined(EW_CONFIG_OPTION_FIELDTYPE_SELECT_SINGLE)
/*
 * ewsFormEnumToString
 * Lookup string corresponding to enum from context
 *
 * context      - context of request
 * enum         - value of enumerator
 *
 * Returns string from corresponding HTML VALUE= field, or NULL if out of
 * bounds.
 */
extern const char * ewsFormEnumToString( EwsContext context, int value );
#endif /* EW_CONFIG_OPTION_FIELDTYPE_RADIO | SELECT_SINGLE */

#endif /* EW_CONFIG_OPTION_FORM */

/*
 * EwsRequestMethod
 *
 * This is an enum for supported request methods.  Bit values are used so
 * groups of valid request methods can be grouped into a single field
 */
typedef enum EwsRequestMethod_e
  {
    ewsRequestMethodUnknown = 0x0000    /* all others */
   ,ewsRequestMethodGet     = 0x0001    /* GET */
   ,ewsRequestMethodPost    = 0x0002    /* POST */
   ,ewsRequestMethodHead    = 0x0004    /* HEAD */

#  ifdef EW_CONFIG_OPTION_METHOD_OPTIONS
   ,ewsRequestMethodOptions = 0x0008    /* OPTIONS */
#  endif
#  ifdef EW_CONFIG_OPTION_METHOD_TRACE
   ,ewsRequestMethodTrace   = 0x0010    /* TRACE */
#  endif
    /* Client can use the PUT method even if no filesystem. */
#  if defined(EW_CONFIG_OPTION_FILE_PUT) || defined(EW_CONFIG_OPTION_CLIENT)
   ,ewsRequestMethodPut     = 0x0020    /* PUT */
#  endif
#  ifdef EW_CONFIG_OPTION_FILE_DELETE
   ,ewsRequestMethodDelete  = 0x0040    /* DELETE */
#  endif
#  ifdef EW_CONFIG_OPTION_UPNP
   ,ewsRequestMethodMSearch  = 0x0080    /* M-SEARCH */
   ,ewsRequestMethodNotify   = 0x0100    /* NOTIFY */
   ,ewsRequestMethodSubscribe = 0x0200   /* SUBSCRIBE */
   ,ewsRequestMethodUnsubscribe = 0x0400 /* UNSUBSCRIBE */
#  ifdef  EW_CONFIG_OPTION_SOAP
   ,ewsRequestMethodMPost =       0x0800    /* M-POST */
#  endif /* EW_CONFIG_OPTION_SOAP */
#  ifdef EW_CONFIG_OPTION_UPNP_WINDOWS_WORKAROUND
   ,ewsRequestMethodNotifyBuffered = 0x1000    /* NOTIFY with buffering of
                                                * request body*/
#  endif /* EW_CONFIG_OPTION_UPNP_WINDOWS_WORKAROUND */
#  endif /* EW_CONFIG_OPTION_UPNP */

  } EwsRequestMethod;


#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
/*
 * DYNAMIC SELECT SUPPORT
 *
 */
typedef struct EwsFormSelectOption_s
  {
    EW_NSC void        *valuep;
    EW_NSC char        *choice;
    boolean     selected;
  } EwsFormSelectOption, *EwsFormSelectOptionP;

#endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC  */

/*
  * Schema metadata access operations, used with
  * ewsContextSchemaInfo interface (ews_ctxt.c)
  */

typedef enum
{
  ewsSchemaGetAppTypeCode,      /* application type (uint32) */
  ewsSchemaGetRenderingCode,    /* Emweb type (EwFieldType) */
  ewsSchemaGetSyntax,           /* SMI Type (const char *) */
  ewsSchemaGetTranslation,      /* object ID (const char *) */
  ewsSchemaGetName              /* object name (const char *) */
} EwsSchemaOperation;

typedef enum    /* order of enums must be preserved */
{
  ewsSchemaRenderHexString = 0,    /* Hex octet string */
  ewsSchemaRenderText,             /* printable text field */
  ewsSchemaRenderDecimalUint,      /* unsigned integer */
  ewsSchemaRenderDecimalInt,       /* signed integer */
  ewsSchemaRenderHexInt,           /* hexadecimal integer */
  ewsSchemaRenderDottedIP,         /* dotted IP address */
  ewsSchemaRenderIEEEMAC,          /* IEEE MAC */
  ewsSchemaRenderDecnetIV,         /* Decnet IV Address */
  ewsSchemaRenderObjectID,         /* object identifier */
  ewsSchemaRenderBoolean,          /* boolean */
  ewsSchemaRenderBase64,           /* Base 64 */
  ewsSchemaRenderUUID,             /* UUID */
  /* ADD NEW ENUMS HERE, DO NOT REORDER EXISTING ENUMS */
  /* SNMP Agent ports depend on order preservation */
  ewsSchemaRenderMAXInternal = 128  /* reserve first 128 for EmWeb */
} EwsSchemaRenderingCode;

typedef struct EwsFormFieldDynamic_s
  {
    EwsSchemaRenderingCode type;
    void                   *datap;
  } EwsFormFieldDynamic, *EwsFormFieldDynamicP;

typedef union
{
  uint32                    appTypeCode;    /* ewsSchemaGetAppTypeCode */
  EwsSchemaRenderingCode    renderingCode;  /* ewsSchemaGetRenderingCode */
  const char                *syntax;        /* ewsSchemaGetSyntax */
  const char                *translation;   /* ewsSchemaGetTranslation */
  const char                *name;          /* ewsSchemaGetName */
} EwsSchemaInfo;


#ifdef EW_CONFIG_OPTION_AUTH
/*
 * states of authentication process
 */

typedef enum
  {
    ewsAuthStateUnauthenticated,  /* document not protected        */
    ewsAuthStateUninitialized,    /* process not started yet,
                                     still gathering data          */
    ewsAuthStateIntegrity,        /* entity body digest in process */
                                  /* request authorized            */
    ewsAuthStateAuthenticateMBasic,/* we have user but authentication not done yet */
    ewsAuthStateAuthenticateDigest,/* we have user but authentication not done yet */
    ewsAuthStateOKDigest,
    ewsAuthStateOKBasic,
    ewsAuthStateOKMBasic,
#ifdef _CONFIG_OPTION_AUTH_DIGEST_INT
    ewsAuthStateOKDigestIntegrity,
#endif
    ewsAuthStateFailed            /* authentication failed         */
  } EwsAuthState;
#endif

#ifndef EW_UNUSED
#   define EW_UNUSED(x) (void)x
#endif

#endif /* _EWS_DEF_H_ */
