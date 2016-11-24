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
 * EmWeb/Server internal definitions
 *
 */

#ifndef _EWS_H_
#define _EWS_H_

#include "ew_types.h"
#include "ews_common.h"
#include "ew_config.h"

#include "ews_optck.h"

#include "ew_lib.h"
#include "ews_str.h"
#include "ew_common.h"
#include "ews_file.h"
#include "ews_pars.h"
#include "ews_secu.h"
#include "ew_db.h"
#include "ews_def.h"
#include "ews_send.h"
#include "ews_dist.h"
#include "session.h"

#if 0
extern int printf (const char *, ...);                      /*lvl7_P0006*/
extern char *strstr (const char *__s1, const char *__s2);   /*lvl7_P0006*/
extern char *strcat (char *__s1, const char *__s2);         /*lvl7_P0006*/
#endif

#ifdef EW_CONFIG_OPTION_COMPRESS
#include "ew_cmp.h"
#endif

#ifdef EW_CONFIG_OPTION_SMTP
#define _EWS_H_MAIL 1 /* version */
#include "../mail/ews_mail.h"
#endif /* EW_CONFIG_OPTION_SMTP */

#ifdef EW_CONFIG_OPTION_TELNET
#define _EWS_H_TELNET 1 /* version */
#include "../telnet/ews_tel.h"
#endif /* EW_CONFIG_OPTION_TELNET */

#ifdef EW_CONFIG_OPTION_CLIENT
#define _EWS_H_CLIENT 1 /* version */
#include "../client/ews_client_i.h"
#endif /* EW_CONFIG_OPTION_CLIENT */

#ifdef EMWEB_XMLP
#define _EWS_H_XMLP 1 /* version */
#include "ews_xmlp.h"
#include "../xmlp/xmllocal.h"
#endif /* EMWEB_XMLP */

#ifdef EW_CONFIG_OPTION_UPNP
#include "ews_upnp.h"
#include "../upnp/ews_upnp_i.h"
#endif /* EW_CONFIG_OPTION_UPNP */

#if 0
#include "config_script_api.h"
#endif

#define EWSASNUM(context)  (context)->promptcontext.router_context.asnumber
#if 0
#define EWSACLID(context)   (context)->promptcontext.acl_context.aclid
#define EWSACLRULNUM(context)   (context)->promptcontext.acl_context.rulenum
#endif
#define EWSPOLICYNAME(context)   (context)->promptcontext.policymapContext.strPolicyName
#define EWSSERVICENAME(context)   (context)->promptcontext.policymapContext.strClassName
#define EWSCLASSMAP(context)   (context)->promptcontext.classmapContext.strClassName
#define EWSCLASSMAPACLTYPE(context)   (context)->promptcontext.classmapContext.aclType
#define EWSCLASSMAPACLID(context)   (context)->promptcontext.classmapContext.aclId

#if 0
/* To refer to the BW provisioning parameters*/
#define EWSBWCLASSNAME(context)   (context)->promptcontext.bwp_context.strClassName
#define EWSBWCLASSTYPE(context)   (context)->promptcontext.bwp_context.strType
#define EWSBWALLOCNAME(context)   (context)->promptcontext.bwp_context.strBWAllocationName
#endif

#define EWSLINECONFIGTYPE(context) (context)->promptcontext.lineConfig_context.lineConfigType
#define EWSACCESSLISTNAME(context)   (context)->promptcontext.accessList_context.strAccessListName
#define EWSDOT1ADSERVICENAME(context) (context)->promptcontext.dot1adService_context.strDot1adServiceName
#define EWSDOT1AGDOMAINNAME(context) (context)->promptcontext.dot1agDomain_context.domainName
#define EWSDOT1AGDOMAINLEVEL(context) (context)->promptcontext.dot1agDomain_context.mdLevel
#define EWSARPACLNAME(context)       (context)->promptcontext.arpAcl_context.strArpAclName
#define EWSUNIT(context)   (context)->promptcontext.if_context.unit
#define EWSSLOT(context)   (context)->promptcontext.if_context.slot
#define EWSPORT(context)   (context)->promptcontext.if_context.port
#define EWSTUNNELID(context)    (context)->promptcontext.if_context.ifId
#define EWSLOOPBACKID(context)    (context)->promptcontext.if_context.ifId
#define EWSTACACSSRVHOST(context) (context)->promptcontext.tacacs_context.strHost
#define EWSPOOLNAME(context)  (context)->promptcontext.poolConfig_context.strPoolName
#define EWSPOOL6NAME(context)  (context)->promptcontext.pool6Config_context.strPoolName
#define EWSMACADDR(context)   (context)->promptcontext.wirelessAPConfig_context.macAddr
#define EWSNETWORKID(context)   (context)->promptcontext.wirelessNetworkConfig_context.networkId
#define EWSPROFILEID(context)   (context)->promptcontext.wirelessAPProfileConfig_context.profileId
#define EWSPROFILEPROFILEID(context)   (context)->promptcontext.wirelessAPProfileRadioConfig_context.profileId
#define EWSPROFILERADIOID(context)   (context)->promptcontext.wirelessAPProfileRadioConfig_context.radioIndex
#define EWSVAPAPPROFILEID(context)   (context)->promptcontext.wirelessAPProfileVAPConfig_context.profileId
#define EWSVAPAPPROFILERADIO(context)   (context)->promptcontext.wirelessAPProfileVAPConfig_context.radioIndex
#define EWSVAPAPPROFILEVAPID(context)   (context)->promptcontext.wirelessAPProfileVAPConfig_context.vapId

#define EWSCPID(context)       (context)->promptcontext.captivePortalIdConfig_context.cpId
#define EWSWEBID(context)       (context)->promptcontext.captivePortalIdConfig_context.webId

#define EWSSHOWALL(context)   (context)->showRunningDefaultFlag
#define EWSWRITESCR(context)   (context)->showRunningOutputFlag

typedef enum
{
  CLI_EWS_WRITE_FLAGS_NORMAL = 10,
  CLI_EWS_WRITE_FLAGS_MAIN,
  CLI_EWS_WRITE_FLAGS_MODE_START,
  CLI_EWS_WRITE_FLAGS_MODE_END,
  CLI_EWS_WRITE_FLAGS_MODE_HIDE,
  CLI_EWS_WRITE_FLAGS_MODE_NOHIDE,
  CLI_EWS_WRITE_FLAGS_MODE_SHOW,
}CLI_EWS_WRITE_FLAGS_t;

L7_BOOL cliEwsWriteBuffer (L7_uint32 lcr, L7_uint32 tcr, L7_uint32 lsp, L7_uint32 tsp, 
    L7_char8 *prefix, EwsContext context, L7_char8 *buf, CLI_EWS_WRITE_FLAGS_t flags);

#define EWSWRITEBUFFER_MAIN_FLAG(lcr,tcr,lsp,tsp,prefix,context,buf,flag) \
{\
  if(L7_TRUE == cliEwsWriteBuffer(lcr,tcr,lsp,tsp,prefix,context,buf,flag))\
  {\
    return L7_SUCCESS;\
  }\
}

#define EWSWRITEBUFFER_MAIN_ADD_BLANKS(lcr,tcr,lsp,tsp,prefix,context,buf) \
  EWSWRITEBUFFER_MAIN_FLAG(lcr,tcr,tsp,lsp,prefix,context,buf,CLI_EWS_WRITE_FLAGS_MAIN)
#define EWSWRITEBUFFER_MAIN(context,buf) \
  EWSWRITEBUFFER_MAIN_FLAG(0,0,0,0,L7_NULLPTR,context,buf,CLI_EWS_WRITE_FLAGS_MAIN)
#define EWS_WRITE_MODE_START(context,buf) \
  EWSWRITEBUFFER_MAIN_FLAG(0,0,0,0,L7_NULLPTR,context,buf,CLI_EWS_WRITE_FLAGS_MODE_START)
#define EWS_WRITE_MODE_EXIT(context,buf) \
  EWSWRITEBUFFER_MAIN_FLAG(0,0,0,0,L7_NULLPTR,context,buf,CLI_EWS_WRITE_FLAGS_MODE_END)
#define EWSWRITEBUFFER(context,buf) \
  cliEwsWriteBuffer(0,0,0,0,L7_NULLPTR,context,buf,CLI_EWS_WRITE_FLAGS_NORMAL)
#define EWSWRITEBUFFER_ADD_BLANKS(lcr,tcr,tsp,lsp,prefix,context,buf) \
  cliEwsWriteBuffer(lcr,tcr,tsp,lsp,prefix,context,buf,CLI_EWS_WRITE_FLAGS_NORMAL)

/* DOCUMENT CONTEXT
 *
 * The EmWeb/Server maintains a per-document context structure containing
 * all the necessary state information to serve a document from the archive.
 * These are chained together into a LIFO stack to handle EMWEB_INCLUDE
 * nesting.
 */
typedef struct EwsDocumentContext_s
{
  struct EwsDocumentContext_s *next;  /* next document in LIFO stack */
  struct EwsDocumentContext_s *prev;  /* previous document in LIFO stack */

  EwsContext          context;        /* pointer to request context */
  EwsDocument         document;       /* pointer to filesystem document */
  const EwsDocumentHeader *doc_headerp; /* pointer to document header */
  const EwsDocumentNode *nodep;         /* pointer to current node */
  uintf               node_count;     /* nodes remaining to process */
  const uint8         *datap;         /* pointer to compressed data */
  uint32              remaining;      /* bytes remaining in data */
  uint32              offset;         /* offset to uncompressed data */
  uint32              iterations;     /* iterations of string/include
                                         /dynamic select */
  void                *compress_context;      /* compression context */
#   ifdef EW_CONFIG_FILE_METHODS
  EwaFileHandle       fileHandle;
#   ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
  EwsFileSystemHandle fileSystem;
  boolean             fs_inuse;
#   endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
#   endif   /* EW_CONFIG_FILE_METHODS */
#   ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
  void                *iterationHandle;  /* value returned by iterat_f */
#   endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */
#   ifdef EW_CONFIG_OPTION_STRING_VALUE
  char                   *string_value;  /* pointer to parameter value */
  char                   *string_buffer; /* dynamic buffer for param */
#   endif /* EW_CONFIG_OPTION_STRING_VALUE */
#   ifdef EW_CONFIG_OPTION_FORM_REPEAT
  /*
   * To control repeated bodies in the form we need some
   * extra info. We have to preserve starting point of the body
   * (repeat node and node count) and we have to keep track of
   * repetition process by counting which repeat body in the
   * form we are processing and at which iteration we are at.
   * Repetition process info is necessary for accessing
   * proper form substructures as well as for generating
   * name and value prefixes.
   */
  const EwsDocumentNode *rpt_body_start_nodep; /* ptr to first node of the body */
  uintf       rpt_body_start_node_count; /* nodes remaining to process */
  uintf       rpt_body_total_repeat; /* total number of repeats for this body*/
  uint32      rpt_body_start_offset;   /* first data offset */
  uint32      rpt_body_start_remaining;/* first data remaining */
  const uint8 *rpt_body_start_datap; /* first data pointer */
  uint32      rpt_body_number;       /* which repeat body in the form */
  uint32      rpt_body_iteration;    /* which iteration of repeat */
  EwFormRepeatField *rpt_fieldp;     /* ptr to repeat form field */
#   ifdef EW_CONFIG_OPTION_COMPRESS
  EwDecompressSaveState_t rpt_compress;   /* store decompress state here */
#   endif /* EW_CONFIG_OPTION_COMPRESS */
  boolean     rpt_body_skip;         /* if TRUE, repeat body not included
                                        in the outgoing document */
#   endif /* EW_CONFIG_OPTION_FORM_REPEAT */

#   ifdef EW_CONFIG_OPTION_ITERATE
  struct EwDocIterateContext_s  *iterate;  /* stack of iterate contexts */
#   endif /* EW_CONFIG_OPTION_ITERATE */

#   ifdef EW_CONFIG_OPTION_IF
  uintf       if_depth;
  boolean     if_true;
#   endif /* EW_CONFIG_OPTION_IF */

} EwsDocumentContext, * EwsDocumentContextP;

#ifdef EW_CONFIG_OPTION_ITERATE
/*
 * Repeated document output using <EMWEB_ITERATE> TAG
 *
 * To control repeated bodies in the form we need some
 * extra info. We have to preserve starting point of the body
 * (repeat node, node count, and current data info).
 *
 * These structures are stacked (LIFO) off the document
 * structure to correspond to nested <EMWEB_ITERATE> tags.
 *
 * Probably could meld this with FORM_REPEAT, but the schedule's
 * too tight.
 */
typedef struct EwDocIterateContext_s
{
  struct EwDocIterateContext_s *stack;     /* LIFO */

  const void    *iterator;              /* returned by application code */

  /* node count at the START_ITERATE node is used to
   * identify the context with the particular START_ITERATE
   * doc node that generated it
   */
  uintf iterateNodeId;

  /*
   * following are the values that must be restored when the
   * iteration begins again.  Note that nodep and nodeCount
   * may not correspond to the START_ITERATE node, as there may be
   * set_value nodes _before_ the START_ITERATE to (re)setup the
   * value="" attribute prior to invoking the associated C="" code
   */
  uintf nodeCount;                      /* node count at repeat point */
  const EwsDocumentNode *nodep;         /* ptr to starting node */
  uint32 offset;                        /* saved data offset at start node */
  uint32 remaining;                     /* saved data count at start node */
  const uint8 *datap;                   /* saved datap at start node */
# ifdef EW_CONFIG_OPTION_COMPRESS
  EwDecompressSaveState_t save_cmp;     /* saved compression state */
# endif /* EW_CONFIG_OPTION_COMPRESS */

} EwDocIterateContext;
#endif /* EW_CONFIG_OPTION_ITERATE */

typedef enum
{
  CLI_NORMAL_CMD = 1,  /* Normal command */
  CLI_NO_CMD,                /* No command type*/
  CLI_ERR_CMD                /* End of command type*/
} COMMAND_TYPE_t;


/*
 *
 * Enums For Edit Config File
 *
 */

typedef enum
{
  L7_EXECUTE_SCRIPT= 0,
  L7_VALIDATE_SCRIPT = 1
} L7_ConfigScriptActionFlag_t;


typedef enum
{
  L7_HIDE_DEFAULT = 0,
  L7_SHOW_DEFAULT = 1
} L7_ShowRunningDefaultFlag_t;


typedef enum
{
  L7_WRITE_TERMINAL = 0,
  L7_WRITE_CONFIG_SCRIPT = 1
} L7_ShowRunningOutputFlag_t;


typedef enum
{
  L7_ACTION_FUNC_SUCCESS = 0,
  L7_ACTION_FUNC_FAILED = 1
} L7_ConfigScriptActionFunctionStatusFlag_t;



typedef enum
{
  L7_CONFIG_SCRIPT_NOT_RUNNING = 0,
  L7_CONFIG_SCRIPT_RUNNING = 1
} L7_ConfigScriptStateFlag_t;

/*
 * HTTP Context States
 */
typedef enum
{
  ewsContextStateParseRequest,            /* parse HTTP request line */
  ewsContextStateParseHeaders,            /* parse HTTP/1.0 headers */
  ewsContextStateFindDocument,            /* get doc info for serving */
  ewsContextStateServeSetup,              /* setup doc for serving */

#   ifdef EW_CONFIG_OPTION_FORM
  ewsContextStateParsingForm,             /* parsing x-www-urlencoded */
  ewsContextStateSubmittingForm,          /* submitting form */
  ewsContextStateServingForm,             /* serving form */
#   ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
  ewsContextStateParsingMPForm,
#   endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */
#   endif

#   ifdef EW_CONFIG_OPTION_CGI
  ewsContextStateStartingCGI,
  ewsContextStateServingCGI,              /* handle CGI data body */
#   endif

#   ifdef EW_CONFIG_OPTION_DEMAND_LOADING
  ewsContextStateDocumentFault,           /* URL registered & unloaded */
#   endif

#   ifdef EW_CONFIG_OPTION_STRING
  ewsContextStateString,                  /* handle <EMWEB_STRING> */
#   endif

#   ifdef EW_CONFIG_OPTION_INCLUDE
  ewsContextStateInclude,                 /* handle <EMWEB_INCLUDE> */
#   endif

  ewsContextStateServingDocument,         /* serving document */

#   ifdef EW_CONFIG_OPTION_PERSISTENT
  ewsContextStateSkipContent,             /* skip unparsed entity body */
#   endif /* EW_CONFIG_OPTION_PERSISTENT */

#   ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
  ewsContextStateDynamicSelect,             /* generate dynamic OPTIONS */
#   endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */

#   ifdef EW_CONFIG_OPTION_FILE_PUT
  ewsContextStateParsingPut,              /* writing PUT data to file */
#   endif /* EW_CONFIG_OPTION_FILE_PUT */

#   ifdef EW_CONFIG_OPTION_PRIMARY
  ewsContextStateForwarding,              /* forwarding to secondary */
  ewsContextStateForwardingQueued,        /* queued to secondary */
#   endif /* EW_CONFIG_OPTION_PRIMARY */

#   ifdef EW_CONFIG_OPTION_SMTP
  ewsContextStateSmtp,
#   endif /* EW_CONFIG_OPTION_SMTP */

#   ifdef EW_CONFIG_OPTION_CLIENT
  ewsContextStateClient,
  ewsContextStateClientIdle,
#   endif /* EW_CONFIG_OPTION_CLIENT */

#   ifdef EW_CONFIG_OPTION_TELNET
  ewsContextStateTelnet,
#   endif /* EW_CONFIG_OPTION_TELNET */

#   ifdef EW_CONFIG_OPTION_UPNP
  ewsContextStateDoMSearch,
  ewsContextStateNotify,
  ewsContextStateReNotify,
#   ifdef EW_CONFIG_OPTION_SOAP
  ewsContextStateSOAPAction,
  ewsContextStateServingSOAPResponse,
  ewsContextStateSOAPQuery,
  ewsContextStateSSDPMsearchResponse,
#   endif /*EW_CONFIG_OPTION_SOAP */
#   endif /* EW_CONFIG_OPTION_UPNP */

#   ifdef EW_CONFIG_OPTION_UPNP_SSDP_CP
  ewsContextStateSsdpCPSearchResourceDB,
#   endif

#   ifdef EMWEB_XMLP
  ewsContextStateXMLP,                  /* parsing XML */
#   endif /* EMWEB_XMLP */

  ewsContextStateLastState              /* aborting document */

}                 EwsContextState;        /* high-level request state */


/*
 * define substates for each state that needs them
 */


/*
 * substates for ewsContextStateParseRequest state
 */
typedef enum
{
  ewsContextSubstateParseMethod       /* GET/HEAD/POST */
    ,ewsContextSubstatePreParseURI       /* SP */
    ,ewsContextSubstateParseURI          /* absolute_path */
    ,ewsContextSubstateParseQuery        /* query string */
    ,ewsContextSubstatePreParseProtocol  /* SP or EOL for HTTP/0.9 */
    ,ewsContextSubstateParseProtocol     /* HTTP/1.0 */
#   if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
    ,ewsContextSubstateParseScheme       /* http: for http://host */
    ,ewsContextSubstatePreParseHost1     /* first / for http://host */
    ,ewsContextSubstatePreParseHost2     /* second / for http://host */
    ,ewsContextSubstateParseHost         /* host for http://host */
#   endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */
} EwsRequestSubstate;

#if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
typedef enum
{
  ewsContextChunkInStateInit             /* initial chunk state */
    ,ewsContextChunkInStatePreParseLength  /* skip LWS before chunk length */
    ,ewsContextChunkInStateParseLength     /* parse chunk length (hex) */
    ,ewsContextChunkInStatePostParseLength /* skip between chunk length and EOL */
    ,ewsContextChunkInStateHaveNL          /* got '\n' */
    ,ewsContextChunkInStateParseFooter     /* skip footer to end of data */
    ,ewsContextChunkInStateParseData       /* parse chunk data */
} EwsContextChunkInState;
#endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */

/*
 * substates for ewsContextStateParseHeaders
 */
typedef enum
{
  ewsContextSubstateParseHeader,      /* Header */
  ewsContextSubstatePostParseHeader,  /* : */
  ewsContextSubstatePreParseValue,    /* LWS */
  ewsContextSubstateParseValue,       /* Value to EOL */
  ewsContextSubstateCRNLPending,      /* CR or NL is next character */
  ewsContextSubstateHaveCR,           /* got '\r' */
  ewsContextSubstateHaveNL            /* got '\n' */
} EwsHeaderSubstate;

#ifdef EW_CONFIG_OPTION_FORM
/*
 * substates for ewsContextStateParsingForm state
 */
typedef enum
{
  ewsContextSubstateParseName        /* Field name up to = */
    ,ewsContextSubstateParseFormValue    /* Value up to &, LWS, EOF */
#  ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
    ,ewsContextSubstateMPPreamble
    ,ewsContextSubstateMPFindBoundary
    ,ewsContextSubstateMPSearchDone
    ,ewsContextSubstateMPBoundaryFound
    ,ewsContextSubstateMPHeaderHaveCR
    ,ewsContextSubstateMPHeaderHaveNL
    ,ewsContextSubstateMPParseHeader
    ,ewsContextSubstateMPPostParseHeader
    ,ewsContextSubstateMPPreParseValue
    ,ewsContextSubstateMPParseValue
    ,ewsContextSubstateMPOpenFile
    ,ewsContextSubstateMPParseBoundary
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */
} EwsFormSubstate;
#endif /* EW_CONFIG_OPTION_FORM */

/*
 * substates for ewsContextStateServeSetup state
 */
typedef enum
{
  ewsContextServeSetupBegin               /* starting substate */

#  ifdef EW_CONFIG_OPTION_AUTH
    ,ewsContextServeSetupAuth01BasicCheck    /* first basic auth state */
    ,ewsContextServeSetupAuth02BasicCheck    /* second basic auth state */
    ,ewsContextServeSetupAuthDigestNonceCheck/* digest auth states */
    ,ewsContextServeSetupAuthDigestHashA1    /* digest auth states */
    ,ewsContextServeSetupAuthDigestHashA2    /* digest auth states */
    ,ewsContextServeSetupAuthDigestVerify    /* digest auth states */
    ,ewsContextServeSetupAuthDigestIntegrity /* digest auth states */
    ,ewsContextServeSetupAuthDone            /* continue setup after auth */
#  endif /* EW_CONFIG_OPTION_AUTH */

#  ifdef EW_CONFIG_OPTION_DYNAMIC_ETAG
    ,ewsContextServeSetupDynEtag
#  endif /* EW_CONFIG_OPTION_DYNAMIC_ETAG */
} EwsServeSetupSubstate;

#ifdef EW_CONFIG_OPTION_PRIMARY
/*
 * substates for ewsContextStateForwarding state
 */
typedef enum
{
  ewsContextSubstateForwardMethod
    ,ewsContextSubstateForwardProtocol
    ,ewsContextSubstateForwardHeaders
    ,ewsContextSubstateForwardGenerate
    ,ewsContextSubstateForwardVia0
    ,ewsContextSubstateForwardVia1
    ,ewsContextSubstateForwardVia2
    ,ewsContextSubstateForwardVia3
    ,ewsContextSubstateForwardVia4
    ,ewsContextSubstateForwardVia5
    ,ewsContextSubstateForwardVia6
    ,ewsContextSubstateForwardVia7
    ,ewsContextSubstateForwardHost0
    ,ewsContextSubstateForwardHost1
    ,ewsContextSubstateForwardCRNL
    ,ewsContextSubstateForwardBody
    ,ewsContextSubstateForwardResponse  /* pcontext is forwarding response to client */
    /* following states apply ONLY to scontext   */
    ,ewsContextSubstateResponse
    ,ewsContextSubstateResponse1
    ,ewsContextSubstateResponse2
} EwsForwardingSubstate;
#endif /* EW_CONFIG_OPTION_PRIMARY */

#ifdef EW_CONFIG_OPTION_SCHED_FC
/*
 * Flow control state - independent of any state/substate above.  This
 * flag tracks the network-layer's flow state.
 */
typedef enum
{
  ewsContextFlowControlOff,         /* not flow controlled */
  ewsContextFlowControlPending,     /* flow control requested (serving) */
  ewsContextFlowControlOn           /* flow controlled (suspended) */
} EwsFlowControlState;              /* flow control state */
#endif /* EW_CONFIG_OPTION_SCHED_FC */



/*
 * Context Scheduling state - independent from any state/substate above.
 * This state flag indicates the "run state" of the current context.
 * This state is used whether or not EW_CONFIG_OPTION_SCHED is defined!!
 *
 * When EW_CONFIG_OPTION_SCHED is NOT defined, then the state defaults
 * to DataWait until ewsNetHTTPReceive() runs the context with incoming
 * data - then it is set to Scheduled.  The state can be set to DataWait
 * at any time by the code if it runs out of data.
 *
 * When scheduling is enabled, then the context is on the serve_list if
 * the state is Scheduled, otherwise its on the wait_list (either due to
 * suspension or DataWait.
 */
typedef enum
{
  ewsContextScheduled,            /* context ready to run/running */
#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
  ewsContextSuspended,            /* context suspended */
  ewsContextResuming,             /* rerunning app callout that suspended */
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
  ewsContextDataWait              /* run again when more data available */
} EwsContextSchedulingState;

#ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION
typedef enum
{
  ewsDocSelectStateFinish,
  ewsDocSelectStateStart,
  ewsDocSelectStateFindDocument
} EwsDocSelectState;

#endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION */

#ifdef EW_CONFIG_OPTION_CHUNKED_OUT
/*
 * EwsContextChunkOutState is maintained by
 *    the ewsSendBody and ewsSendNBody routines
 */
typedef enum
{
  ewsContextChunkOutNotChunked,
  ewsContextChunkOutNewChunk,
  ewsContextChunkOutPartial
} EwsContextChunkOutState;
#endif /* EW_CONFIG_OPTION_CHUNKED_OUT */

/*
 * REQUEST CONTEXT
 *
 * The EmWeb/Server maintains a per-request context structure containing all
 * the necessary state information to schedule and process HTTP requests
 * from the network.
 */
typedef struct EwsContext_s
{
  /*
   * CONTEXT STATE
   *
   * These fields manage the current state of the context.  Both the state
   * of the protocol, and the state of the scheduler, are managed with these
   * fields.
   */

  EwsLink             link;                   /* list linkage */

  EwsContextState     state;                  /* high-level request state */

  union
  {
    EwsRequestSubstate request;
    EwsHeaderSubstate  header;
    EwsServeSetupSubstate  setup;

#       if defined(EW_CONFIG_OPTION_FORM)
    EwsFormSubstate    form;
#       endif /* EW_CONFIG_OPTION_FORM */

#       ifdef EW_CONFIG_OPTION_PRIMARY
    EwsForwardingSubstate forwarding;
#       endif /* EW_CONFIG_OPTION_PRIMARY */
  }                 substate;       /* state-specific substate */

#   ifdef EW_CONFIG_OPTION_URL_HOOK_LIST
  /* If state == ewsContextStateFindDocument, this is a pointer to the
   * link block for the next URL hook to be executed.
   * NULL means that all URL must be executed for this request.
   * A pointer to the header &(ews_state->url_hook_list) means that all
   * URL hooks have been executed for this request.
   * Thus, its initial value is NULL, and the final value is
   * &(ews_state->url_hook_list).
   * This could be simplified a bit, but it makes setting the state
   * simpler:
   *    context->state = ewsContextStateFindDocument;
   *    #ifdef EW_CONFIG_OPTION_URL_HOOK_LIST
   *    context->substate.next_url_hook = NULL;
   *    #endif
   * It might be more accurate to have the type be (EwsUrlHookEntry *),
   * but that would introduce a dependency on EwsUrlHookEntry.
   */
  EwsLink *next_url_hook;
  boolean url_continue;   /* process more hooks even if one returns url */
#   endif /* EW_CONFIG_OPTION_URL_HOOK_LIST */

  /*
   * execution state of context
   */
  EwsContextSchedulingState   schedulingState;
#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
  /* race condition bugfix: if a suspended context is resumed before
   * returning to ewsRun, the suspension is LOST and never resumed.
   * fix: when ewsResume is called, set resumePending True and leave
   * scheduling state alone.  When code returns to ewsRun, ewsRun
   * will set the schedulingState to resumed. Hack. Real fix would
   * introduce a new scheduling state (resumePending), but that
   * breaks alot of code.  I will fix this later when I get some
   * free time.  Yeah, right.
   */
  boolean     resumePending;
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */


  /*
   * CONTEXT FLAGS
   *
   * General purpose flags to maintain context state.  These flags are
   * cleared _only_ when the context is first allocated.  These flags are
   * NOT automatically cleared between requests on the same context. It is up
   * to the server to set/clear this bits when appropriate.
   *
   * EWS_CONTEXT_FLAGS_CLEANUP - when set, call ewaNetHTTPCleanup at end of
   *      request processing
   *
   * EWS_CONTEXT_FLAGS_ACTION_URL  - set on submission to form with ACTION
   *      specifier.  If ewsContextSendReply() is called when this flag is
   *      set with a relative URL, EmWeb/Server must strip off the action
   *      from the base URL.
   *
   * EWS_CONTEXT_FLAGS_INDEX_URL - set if expanding from directory URL
   *      and the URL does not end in a '/'.  If set, this will cause a
   *      Content-Base: header to be generated so the browser will be able
   *      to resolve relative links.
   *
   * EWS_CONTEXT_FLAGS_FLUSH_DATA - force a flush of all data pending to go
   *      out the network.  This will send all data queued, regardless of
   *      whether or not there is more free space in the last buffer,
   *      whenever ewsServeDocument() is called by the state machine.
   *
   * EWS_CONTEXT_FLAGS_CONVERT_XML - serve the XML version of the requested
   *      URL.
   * EWS_CONTEXT_FLAGS_OUTPUT_OFF - if set, disables writing output data
   *      to the net buffers - effectively sending output to /dev/null.
   *      See ewsBody[N]Send().
   *
   * EWS_CONTEXT_FLAGS_SENT_HEADER - if set, headers have been sent.
   *      Used in ewsContextSendError to see if it's too late to send
   *      an error header.
   *
   * EWS_CONTEXT_FLAGS_URL_NO_SLASH - if set, URL (after URL Hook) did not
   *      have a trailing slash.  If this turns out to be for an index
   *      URL and EW_CONFIG_OPTION_DIRECTORY_REDIRECT is enabled, force
   *      a redirect to the index document.
   *
   * EWS_CONTEXT_FLAGS_CONVERT_TEXTAREA - if set, the document is to be
   *      served inside of a "TEXTAREA hack" wrapper document.  The
   *      boilerplate for the wrapper document is hard-coded in ews_serv.c.
   *
   * EWS_CONTEXT_FLAGS_BODY_QUOTE - if set, '&' and '<' characters that
   *      are being sent to the output will be escaped.
   *      This flag is primarily used while serving the real
   *      document content when EWS_CONTEXT_FLAGS_CONVERT_TEXTAREA has been
   *      set for the whole document.  It stays on during the document
   *      content, and is used by ewsBody[N]Send to determine when to set
   *      EWS_CONTEXT_FLAGS_OUTPUT_QUOTE on.
   *
   * EWS_CONTEXT_FLAGS_OUTPUT_QUOTE - if set, '&' and '<' characters that
   *      are being sent to the output will be escaped.
   *      Set on by ewsBody[N]Send to tell ewsString[N]CopyIn to do quoting.
   *
   * EWS_CONTEXT_FLAGS_CONVERT_SKELETON - if set, the document is to be
   *      served as a skeleton XML document, giving the constant strings
   *      inside of CDATA sections, and the dynamic content in XML tags.
   *
   * EWS_CONTEXT_FLAGS_INPUT_EOF - if set, the input stream has presented
   *      and end-of-file indication.  This does *not* mean that the request
   *      has been aborted by the client, but it means that if processing
   *      demands further input, the context should either be ended or
   *      an 'invalid request' indication should be returned to the client.
   *      Not used for TCP connections (though in theory it could be, since
   *      TCP allows one direction of a connection to present EOF without
   *      closing the connection (see the shutdown() function).
   *      Used to mark the end of data in an HTTP/UDP request, so that the
   *      parser does not hang indefinitely at the end of the packet.
  *
    * EWS_CONTEXT_FLAGS_BUFFER_BODY - if set, the outgoing body is to be
    *      buffered and sent as if it was a fixed-length, static document.
    *      This is used to implement ewsRequestMethodNotifyBuffered, which
    *      is used only to send event notices to early version Windows Me.
    *      It might not work in the general case.
    */
#   define EWS_CONTEXT_FLAGS_CLEANUP          ((uint32) 0x00000001)
#   define EWS_CONTEXT_FLAGS_ACTION_URL       ((uint32) 0x00000002)
#   define EWS_CONTEXT_FLAGS_INDEX_URL        ((uint32) 0x00000004)
#   define EWS_CONTEXT_FLAGS_FLUSH_DATA       ((uint32) 0x00000008)
#   define EWS_CONTEXT_FLAGS_CONVERT_XML      ((uint32) 0x00000010)
#   define EWS_CONTEXT_FLAGS_OUTPUT_OFF       ((uint32) 0x00000020)
#   define EWS_CONTEXT_FLAGS_SENT_HEADER      ((uint32) 0x00000040)
#   define EWS_CONTEXT_FLAGS_URL_NO_SLASH     ((uint32) 0x00000080)
#   define EWS_CONTEXT_FLAGS_CONVERT_TEXTAREA ((uint32) 0x00000100)
#   define EWS_CONTEXT_FLAGS_BODY_QUOTE       ((uint32) 0x00000200)
#   define EWS_CONTEXT_FLAGS_OUTPUT_QUOTE     ((uint32) 0x00000400)
#   define EWS_CONTEXT_FLAGS_CONVERT_SKELETON ((uint32) 0x00000800)
#   define EWS_CONTEXT_FLAGS_INPUT_EOF        ((uint32) 0x00001000)
#   define EWS_CONTEXT_FLAGS_BUFFER_BODY      ((uint32) 0x00002000)

#   define EWS_CONTEXT_FLAGS_RESET_BETWEEN_REQUESTS \
    (  EWS_CONTEXT_FLAGS_ACTION_URL        \
       | EWS_CONTEXT_FLAGS_INDEX_URL         \
       | EWS_CONTEXT_FLAGS_CONVERT_XML       \
       | EWS_CONTEXT_FLAGS_OUTPUT_OFF        \
       | EWS_CONTEXT_FLAGS_SENT_HEADER       \
       | EWS_CONTEXT_FLAGS_URL_NO_SLASH      \
       | EWS_CONTEXT_FLAGS_CONVERT_TEXTAREA  \
       | EWS_CONTEXT_FLAGS_BODY_QUOTE        \
       | EWS_CONTEXT_FLAGS_OUTPUT_QUOTE      \
       | EWS_CONTEXT_FLAGS_CONVERT_SKELETON  \
    )

    uint32  flags;

  /*
   * Abort flags, stored separately to be thread safe.  Access to these
   * flags must be locked.
   */
#   define EWS_CONTEXT_FLAGS_ABORT            ((uint32) 0x00001000)
#   define EWS_CONTEXT_FLAGS_INTERNAL_ABORT   ((uint32) 0x00002000)
  uint32  abort_flags;

  /*
   * if not using scheduling, then all processing is done under
   * the call to ewsNetHTTPReceive().  In ewsNetHTTPAbort(), it is
   * necessary to know whether or not the context passed is currently
   * running.  We cannot safely abort a currently running context -
   * we must mark it so the calling task (ewsRun or ewsNetHTTPReceive)
   * can kill it when its safe.  We determine if the context is currently
   * running base on whether or not scheduling is enabled.  If scheduling,
   * we consult the ews_state structure.  If scheduling not enabled,
   * we use the boolean below.
   */
#   ifdef EW_CONFIG_OPTION_SCHED
#   define IS_CONTEXT_CURRENT( context ) ((context) == ews_state->context_current)
#   else  /* no EW_CONFIG_OPTION_SCHED */
  boolean current;
#   define IS_CONTEXT_CURRENT( context ) ((context)->current)
#   endif

  /*
   * HANDLES
   *
   * Application-specific handles are saved in the context and initialized
   * to their null values (except the net_handle which is passed to
   * EmWeb/Server when the context is created).
   */
  EwaNetHandle        net_handle;     /* application network handle */

#   if defined (EW_CONFIG_OPTION_DEMAND_LOADING) \
  || defined (EW_CONFIG_OPTION_CLONING)
  EwaDocumentHandle   doc_handle;     /* application document handle */
#   endif /* EW_CONFIG_OPTION_DEMAND_LOADING || EW_CONFIG_OPTION_CLONING */

#   ifdef EW_CONFIG_OPTION_AUTH
  EwaAuthHandle       auth_handle;    /* application authorization handle */
#   endif /* EW_CONFIG_OPTION_AUTH */


  /*
   * SERVE SETUP
   *
   * Various parameters used by the serve setup state.  Authentication
   * is done during this state. If scheduling is enabled, then some
   * authentication state is stored here.
   */
#   if defined( EW_CONFIG_OPTION_SCHED )
  EwsDocument         document;       /* parameter to ewsServeSetup */
#   endif /* EW_CONFIG_OPTION_SCHED */

#   if defined( EW_CONFIG_OPTION_AUTH )
  EwsAuthState        auth_state;
#   endif
#   if defined(EW_CONFIG_OPTION_AUTH_DIGEST)
  EwsAuthStateParams  authStateParams;    /* parameters for auth */
  EwsAuthP            authp;
  MD5_CTX             md5_ctx;        /* client message digest */
  MD5_CTX             md5_entity_ctx; /* digest of entity-body */
  EwsString           md5_entity_str; /* entity-body to be digested */
  uintf               md5_remaining;  /* bytes of body remaining */
  EwsRealmP           realmp;
  char                *rcved_url;     /* original url of the request */
  boolean             req_entity_body;
#   if defined(EW_CONFIG_OPTION_AUTH_MDIGEST)
  boolean             auth_mdigest;
#   endif
  boolean             nonce_stale;    /* nonce was stale */
  EwsNonceP           noncep;
#   else
#   ifdef EW_CONFIG_AUTH_SCHED
#   if defined(EW_CONFIG_OPTION_AUTH_BASIC) \
  || defined(EW_CONFIG_OPTION_AUTH_MBASIC)
  EwsAuthStateParams  authStateParams;    /* parameters for auth */
#   endif /* EW_CONFIG_OPTION_AUTH_(M)BASIC */
#   endif /* EW_CONFIG_AUTH_SCHED */
#   endif /* EW_CONFIG_OPTION_AUTH_(M)DIGEST */


  /*
   * HTTP REQUEST STATE
   *
   * As buffers are received from the network containing request data,
   * they are queued in a chain.  Elements within the buffer (e.g. URL,
   * various headers, etc.) are referenced by EwsString structures.
   * Note that the buffers containing request headers are saved until the
   * request is terminated.
   */
  EwaNetBuffer        req_buf_first;  /* first buffer in request chain */
  EwaNetBuffer        req_buf_last;   /* last buffer in request chain */
  EwaNetBuffer        req_buf_used;   /* last buffer used for EwsString */

  EwsString           req_str_begin;  /* start of current estring */
  EwsString           req_str_end;    /* end of current estring +1 */

  EwsString           req_method_str; /* "GET", "HEAD", or "POST" */
  EwsString           req_protocol;   /* "HTTP/1.0" */
  uint32              req_protocol_maj; /* major version # */
#                       define          HTTP_NOT 0
  uint32              req_protocol_min; /* minor version # */
  EwsString           req_url;        /* Full URL before query or params*/
  EwsString           req_query;      /* Query string, if present */

  EwsRequestHeaderIndex req_header;   /* state: current header in parser */
  EwsRequestMethod    req_method;     /* request method */

#   if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 ||\
  defined (EW_CONFIG_OPTION_CONTEXT_HOST) ||\
  defined (EW_CONFIG_OPTION_PRIMARY)
  EwsString           req_host;       /* request host */
#   endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */

#   ifdef EW_CONFIG_OPTION_FORM
  uint32              *form_expectp;  /* Form bytes expected */
  uint32              form_expectv;   /* possible counter (get method) */
  EwsString           *form_parsep;   /* pointer to estring used for parse */
  EwsString           form_parsev;    /* possible estring (get method) */
#   if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
  boolean             *form_chunkedp; /* pointer to chunk boolean */
  boolean             form_chunkedv;  /* FALSE (get method ) */
#   endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */
  EwsString           form_name;      /* Current form field name */
  uint32              form_index;     /* index into form */
#   ifdef EW_CONFIG_OPTION_NS_SET
  const EmWebNamespaceTable *form_ns; /* index into namespace for set form */
  void                *form_handle;   /* namespace handle */
#   endif /* EW_CONFIG_OPTION_NS_SET */
  const EwFormEntry   *form_entry;    /* form entry table (from compiler) */
  const char          **form_enum_table;
  uint32              form_enum_table_size;
#   endif /* EW_CONFIG_OPTION_FORM */

#   if defined(EW_CONFIG_OPTION_FORM) ||\
  defined(EW_CONFIG_OPTION_STRING_TYPED)
  void                *form;          /* Current form template or NULL */
#   ifdef EW_CONFIG_OPTION_FORM_REPEAT
  /*
   * For repeat bodies we will use form pointer (above) to point
   * to the value substructure to which value_offset in the
   * field entry relates to.
   * In base_form we keep pointer to the base form structure.
   * form_status is used to point to the status substructure
   * for repeat body or is same as base_form for base structure.
   * We rely on the fact that, if base_form is not null, then
   * it points a the form "template" (true form structure).
   * Do not use base_form to point to anything else!
   */
  void                *form_status;   /* current status template    */
  void                *base_form;     /* current base form template    */
#   endif /* EW_CONFIG_OPTION_FORM_REPEAT */
#   endif /* EW_CONFIG_OPTION_FORM | EW_CONFIG_OPTION_STRING_TYPED */

#   if defined( EW_CONFIG_OPTION_CGI_PATH_INFO ) \
  || defined( EW_CONFIG_OPTION_URL_PATH_INFO )
  char                *path_info;     /* pointer to path info if present */
#   endif

#   ifdef EW_CONFIG_OPTION_CGI
  uint32              cgi_index;      /* index for CGI request */
  EwaCGIData_f        *cgi_data_f;    /* archive CGI data function */
  EwaCGIHandle        cgi_handle;     /* CGI handle from start function */
#   endif /* EW_CONFIG_OPTION_CGI */

#   ifdef EW_CONFIG_OPTION_PRIMARY
  EwsSecondaryHandle  secondary;      /* Acting as gateway to secondary */
  uint32              max_forwards;   /* value from Max-Forwards: or 0 */
  EwsString           hdr_str_clobber;/* used for header clobbering */
  EwsString           hdr_str_begin;  /* beginning of headers */
  EwsString           hdr_str_end;    /* end of headers */
  boolean             is_secondary_context; /* TRUE if special sec. context */
  boolean             downgrade;      /* TRUE if downgrade to HTTP/1.0 */
  char               *cptr;           /* char pointer for writing strings */
  uintf               clen;           /* character counter */
  char                cbuf[12];       /* character buffer */
  uint32              res_status;     /* response status code */
#   endif /* EW_CONFIG_OPTION_PRIMARY */

  /*
   * HTTP HEADERS
   *
   * Extra memory is allocated immediately following this structure for the
   * ewsRequestHeaderCount+1 per-header EwsString structures.  This structure
   * element points to the start of this EwsString table.
   */
  EwsString           *req_headerp;
  char                *url;
  boolean             url_dynamic;
  uint32              content_length;
  uint32              content_remaining;

#   if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
  /*
   * Chunk In state
   */
  boolean             chunked_encoding;       /* TRUE if request chunked */
  EwsContextChunkInState chunk_in_state;           /* Chunk parse state */
  EwsString           chunk_str_begin;        /* saved req_str_begin */
  EwsString           chunk_str_end;          /* saved req_str_end */

#  ifdef EW_CONFIG_OPTION_EXPECT
  /*
   * Expectations
   */
  boolean             expect_100_continue;   /* TRUE if Expect: 100-continue */
  boolean             expect_failed;         /* TRUE if unknown Expect: */
#   endif /* EW_CONFIG_OPTION_EXPECT */

#   endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */


  /*
   * HTTP REPLY STATE
   */
  EwsDocumentContextP doc_stack_head;         /* head of document stack */
  EwsDocumentContextP doc_stack_tail;         /* tail of document stack */
  uintf               doc_stack_depth;        /* depth of document stack */
  EwaNetBuffer        res_buf_next;           /* next buffer to send */
  EwsString           res_str_end;            /* end of outbound estring */

#   ifdef EW_CONFIG_OPTION_PERSISTENT
  boolean             have_connection_header; /* Connection: header present */
  boolean             keep_alive;             /* persistent connection flag */
#   ifdef EW_CONFIG_OPTION_SCHED
  boolean             finish_yield;           /* yield between responses */
#   endif /* EW_CONFIG_OPTION_SCHED */
#   endif /* EW_CONFIG_OPTION_PERSISTENT */

#   ifdef EW_CONFIG_OPTION_CHUNKED_OUT
  EwsContextChunkOutState    chunk_out_state;
  EwsString           res_chunk;  /* latest response chunk header
                                   * defined iff ewsContextChunkOutPartial
                                   */
#   endif /* EW_CONFIG_OPTION_CHUNKED_OUT */
#   ifdef EW_CONFIG_OPTION_AUTH_DIGEST
  MD5_CTX             res_md5_ctxt; /* response entity body hash */
  boolean             res_entity_body;
  /*
   * send_auth_info_hdr
   *
   * This flag is set to FALSE when context is created.
   * It is set to TRUE if ewsSendHeader() has to add Authentication-info
   * header to the reply and set back to FALSE right after header is
   * added
   */
  boolean             send_auth_info_hdr;
#   endif

#   ifdef EW_CONFIG_OPTION_SCHED_FC
  EwsFlowControlState flow_control;           /* flow control state */
#   endif /* EW_CONFIG_OPTION_SCHED_FC */

#   ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
  /*
   * FORM <INPUT TYPE=FILE> Support
   */
  struct EwsFormInputFileState_s   *inputFile;  /* alloc'ed on demand */
#   ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
  EwsLink     inputFile_system_list;
#   endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
#   endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */


  /*
   * Local file system support.  Only use this field
   * if local file system supports GET or PUT or DELETE
   */
#   ifdef EW_CONFIG_FILE_METHODS
  union EwsFileParams_s   *fileParams;
#   ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
  EwsFileSystemHandle     fileSystem;
  boolean                 fs_inuse;
#   endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
  EwsDocument             dummy;      /* temp holder for dummy doc */
  EwaFileHandle           fileHandle; /* handle for put file */
#   endif   /* EW_CONFIG_FILE_METHODS */

#   ifdef EW_CONFIG_OPTION_COOKIES
  EwsCookieP  cookie_list_first;      /* head of the list of cookie values
                                         to be 'freed' when server is done
                                         with request */
  EwsCookieP  cookie_list_last;       /* tail of the list of cookie values
                                         needed, because cookies linked
                                         in order thay are parsed in */
#   endif /* EW_CONFIG_OPTION_COOKIES */

#   ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION
  EwsDocSelectState      select_state;
  EwsNegotiateHdrDataP   negotiate_hdr_first;
  EwsNegotiateHdrDataP   negotiate_hdr_last;
  EwsAcceptHdrDataP      accept_hdr_first;
  EwsAcceptHdrDataP      accept_hdr_last;
  EwsAcceptHdrDataP      accept_charset_hdr_first;
  EwsAcceptHdrDataP      accept_charset_hdr_last;
  EwsAcceptHdrDataP      accept_language_hdr_first;
  EwsAcceptHdrDataP      accept_language_hdr_last;
  EwsAcceptHdrDataP      accept_encoding_hdr_first;
  EwsAcceptHdrDataP      accept_encoding_hdr_last;
  EwsTcnHdrType          tcn_hdr;
  char                   *alt_hdr; /* points to Alternates hdr in archive */
  char                   *vary_hdr;
  char                   *content_location;
  boolean                ck_accept_encoding;
#   endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION */
  char                   *content_encoding;

#   ifdef EW_CONFIG_OPTION_SMTP
  struct EwsSmtp_s       *smtp;
#   endif /* EW_CONFIG_OPTION_SMTP */

#   ifdef EW_CONFIG_OPTION_GROUP_DYNAMIC
  uintf                   group_dynamic_count;
  boolean                 group_dynamic_flag[1];
  void                   *group_dynamic_ptr[1];
#   endif /* EW_CONFIG_OPTION_GROUP_DYNAMIC */

#   ifdef EW_CONFIG_OPTION_SNMP_AGENT
  /*
   * for use with SNMP Interface functions
   */
  EwaSNMPAgentHandle        snmpHandle;
#   ifdef EW_CONFIG_OPTION_AUTH
  EwaSNMPAgentAuthHandle    snmpAuthHandle;
#   endif /* EW_CONFIG_OPTION_AUTH */
#   endif   /* EW_CONFIG_OPTION_SNMP_AGENT */

#   if defined( EW_CONFIG_SCHEMA_ARCHIVE )
  /*
   * for use with Schema data access routines - we need to know
   * which archive we are using, where the schema nodes are, and
   * which schema node is active for the current operation.
   */
  const uint8               *archiveData;    /* archive data */
  const EwsDocumentNode     *schemaNodes;    /* schema node table */
  const EwsDocumentNode     *currentElement; /* current schema node */
#   endif /* EW_CONFIG_SCHEMA_ARCHIVE */

#   ifdef EW_CONFIG_OPTION_CONVERT_XML
  /*
   * for use when serving an HTML page's dynamic content as XML...
   */
  const char *xmlCurrentNamespace;    /* address of current namespace name */
#   endif /* !EW_CONFIG_OPTION_CONVERT_XML */

#   ifdef EW_CONFIG_OPTION_NS_LOOKUP
  uint32 node_index;        /* temporary node_index storage */
  const char * ns_set_name; /* name of namespace for indirect group set */
#   endif /* EW_CONFIG_OPTION_NS_LOOKUP */

#   ifdef EW_CONFIG_OPTION_TELNET
  struct EwsTelnet_s *telnet;
#   endif /* EW_CONFIG_OPTION_TELNET */

#   ifdef EW_CONFIG_OPTION_SSL_RSA
  boolean ssl_connection;
#   endif /* EW_CONFIG_OPTION_SSL_RSA */

#   ifdef EW_CONFIG_OPTION_CLIENT
  struct EwsClient_s       *client;          /* current client request */
  struct EwsClientHost_s   *cmp;             /* current client connection */
  const char               *override_status; /* reset status for proxy */

#   ifdef EW_CONFIG_OPTION_CLIENT_CACHE
  boolean                   parse_from_file; /* TRUE if parsing from file */
  EwaFileHandle             source_file;     /* source file handle */
#   ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
  EwsFileSystemHandle       source_fs;       /* source filesystem */
#   endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
#   endif /* EW_CONFIG_OPTION_CLIENT_CACHE */

#   endif /* EW_CONFIG_OPTION_CLIENT */

#   ifdef EW_CONFIG_OPTION_UPNP
  /* GENA subscription Headers */
  char* sid;
  uint32 subscriptionDuration;
  /*
   * UPNP variables
   */
  boolean                  ssdp_connection;
#   define                   UPNP_NO_HEADERS            (uint8)0
#   define                   UPNP_MAN_HEADER            (uint8)0x01
#   define                   UPNP_MX_HEADER             (uint8)0x02
#   define                   UPNP_ST_HEADER             (uint8)0x04
#   define                   UPNP_HAVE_ALL_HEADERS ((uint8)(UPNP_MAN_HEADER | UPNP_MX_HEADER | UPNP_ST_HEADER))
#   ifdef EW_CONFIG_OPTION_UPNP_SSDP_CP
#   define                   UPNP_NT_HEADER             (uint8)0x08
#   define                   UPNP_NTS_HEADER            (uint8)0x10
#   define                   UPNP_USN_HEADER            (uint8)0x20
#   define                   UPNP_LOCATION_HEADER       (uint8)0x40
#   define                   UPNP_CACHE_CONTROL_HEADER  (uint8)0x80
#   define                   UPNP_HAVE_NOTIFY_HEADERS ((uint8)(UPNP_CACHE_CONTROL_HEADER | UPNP_LOCATION_HEADER | UPNP_NT_HEADER | UPNP_NTS_HEADER | UPNP_USN_HEADER))
#   define IS_NOTIFY_REQUEST(context) (  (context->ssdp_connection) \
    &&(context->upnp_have_headers & UPNP_NT_HEADER))
#   endif /* EW_CONFIG_OPTION_UPNP_SSDP_CP */
  uint8                    upnp_have_headers;
  uint32                   ssdp_mx;        /* device/service expiration
                                            * time */
  struct EwuSsdpDevice_s   *upnp_rootP;    /* UPnP device description */
  boolean                  upnp_parse_ddd; /* TRUE if this context is
                                            * parsing a Device Description
                                            * Document. */
  struct EwuSsdpDevice_s   *upnp_parse_root_device;
  /* ptr to the root device instance we are
   * parsing. */
  struct EwuSsdpDevice_s   *upnp_parse_containing_device;
  /* ptr to the device in which new device
   * instances will be created. */
  struct EwuSsdpDevice_s   *upnp_parse_device;
  /* ptr to the device instance we are
   * parsing. */
  struct EwuSsdpService_s  *upnp_parse_service;
  /* ptr to the service instance we are
   * parsing. */
  boolean                   upnp_parse_error_found;
  /* TRUE if an error has been found while
   * parsing this UPnP Device Description
   * Document. */
  /*
   * Variables to control scratch space.
   *
   * The scratch space is a dynamically allocated string that is lengthened
   * as necessary.  scratch_length_allocated is the number of bytes
   * allocated.  scratch_length_used is the number of bytes of data.
   * These bytes are followed by a NUL (so that scratch can be used as a
   * C string).  The NUL is not counted in scratch_length_used.
   * The scratch can be used as a binary string if scratch_length_used is
   * consulted.
   */
  int                      scratch_length_allocated;
  int                      scratch_length_used;
  char                     *scratch;

#   ifdef EW_CONFIG_OPTION_SOAP
#   define                   EW_SOAP_REQUEST            (uint8)0x10
#   define                   EW_SOAP_ACTION_REQ         (uint8)0x01
#   define                   EW_SOAP_QUERY_REQ          (uint8)0x02
#   define                   EW_SOAP_ERROR_RSP          (uint8)0x04
#   define                   EW_SOAP_ABORT              (uint8)0x08
  uint8                    soap_flags;
#   define                   EW_SOAP_XMLP_ENVELOPE_END  (uint16)0x0001
#   define                   EW_SOAP_XMLP_BODY_END      (uint16)0x0002
#   define                   EW_SOAP_XMLP_ACTION_END    (uint16)0x0004
#   define                   EW_SOAP_XMLP_ARGUMENT_END  (uint16)0x0008
#   define                   EW_SOAP_XMLP_QUERY_END     (uint16)0x0010
#   define                   EW_SOAP_XMLP_VALUE         (uint16)0x0020
  uint8                    xmlp_flags;
  EwuXMLParseState         xmlp_state;
  XmlpConsumer             xmlp_consumer;     /* from XML register consumer*/
  EwsString                soap_action_name;
  char                     *soap_action_doc_url;
  EwsString                soap_arg_name;
  boolean                  soap_arg_empty;
  EwsString                soap_var_name;
  int16                    soap_var_code;
  EwsFormFieldDynamic      soap_dynamic_field;
  EwuSsdpServiceP          soap_serviceP;
  EmWebUPnPTable           *soap_service_dataP;
  uint16                   soap_error_code;
  char                     *soap_error_description;
#   endif /* EW_CONFIG_OPTION_SOAP */

#   ifdef EW_CONFIG_OPTION_UPNP_SSDP_CP
  EwuSsdpInterestP         interestP;
#   endif /* EW_CONFIG_OPTION_UPNP_SSDP_CP */

  /*
   * State for parsing SSDP M-SEARCH responses.  Uses client's parser for
   * parsing status line, then returns to server.
   */
  boolean                  isMsearchResponse; /* M-SEARCH response context */
  EwsClientState           clientState;  /* for parsing status header */
  uint32                   clientStatus; /* status line */
  EwsString                clientString; /* status string */

#   endif /* EW_CONFIG_OPTION_UPNP */

#   ifdef EMWEB_XMLP
  XmlpParser                xmlp_handle;     /* from XML start function */
#   endif /* EMWEB_XMLP */

#   ifdef EW_CONFIG_OPTION_BYTE_RANGES
  boolean                   isRange;         /* TRUE if range header */
  int32                     firstBytePos;    /* >=0 first, <0 suffix pos */
  int32                     lastBytePos;     /* pos or unknown */
  uint32                    instanceLength;  /* used in error case */
#   endif /* EW_CONFIG_OPTION_BYTE_RANGES */

#   ifdef EW_CONFIG_OPTION_DYNAMIC_ETAG
  char                     *etag;            /* see ewsRetrieveEtag */
  uintf                     max_age;         /* see ewsContextSetMaxAge */
#   endif /* EW_CONFIG_OPTION_DYNAMIC_ETAG */
  COMMAND_TYPE_t commType; /* To store the command type*/

  /***Values for Configuration script*******/
  L7_ConfigScriptActionFlag_t  scriptActionFlag;

  L7_ShowRunningDefaultFlag_t  showRunningDefaultFlag;

  L7_ShowRunningOutputFlag_t   showRunningOutputFlag;

  L7_ConfigScriptActionFunctionStatusFlag_t  actionFunctionStatusFlag;

  L7_ConfigScriptStateFlag_t  configScriptStateFlag;

  /*L7_ConfigScript_t *ConfigScriptData;*/
  void *configScriptData;

  boolean unbufferedWrite;

  boolean allow_http;  /* Web Mode Enabled/Disabled TRUE/FALSE */

  union
  {
    ews_aclContext_t                acl_context;
#if 0
    ews_bwpContext_t                bwp_context;
#endif
    ews_ifContext_t                 if_context;
    ews_routerContext_t             router_context;
    ews_policymapContext_t  policymapContext;
    ews_classmapContext_t   classmapContext;
    ews_dhcpsPoolConfigContext_t    poolConfig_context;
    ews_dhcp6sPoolConfigContext_t   pool6Config_context;
    ews_lineConfigContext_t         lineConfig_context;
    ews_accessListContext_t         accessList_context;
    ews_dot1adServiceContext_t         dot1adService_context;
    ews_dot1agDomainContext_t          dot1agDomain_context;
    ews_tacacsContext_t             tacacs_context;  
    ews_wirelessAPConfigContext_t   wirelessAPConfig_context;
    ews_wirelessNetworkConfigContext_t     wirelessNetworkConfig_context;
    ews_wirelessAPProfileConfigContext_t   wirelessAPProfileConfig_context;
    ews_wirelessAPProfileRadioConfigContext_t wirelessAPProfileRadioConfig_context;
    ews_wirelessAPProfileVAPConfigContext_t wirelessAPProfileVAPConfig_context;
    ews_captivePortalIdConfigContext_t captivePortalIdConfig_context;    
    ews_rangeContext_t              rangeContext;
    ews_arpAclContext_t             arpAcl_context;
  }promptcontext;    

  int                 obtSocket;         /* OBT socket number */
  boolean             obtActive;         /* OBT status flag */
  EwaSession*         session;

  struct ews_runningConfigContext_t runningConfig; /* show running config purpose */     

  /* XUI */
  /* XUI */
  /* XUI */
  /* XUI */
   void *fileHandler;
  char prefixBuf[32];
  char indexModBuf[32];
#ifdef EW_CONFIG_OPTION_PER_SESSION_ETAG
    char eTagBuf[128];
#endif
  int http_download_started;
  /* XUI */
  /* XUI */
  /* XUI */
  /* XUI */

} EwsContext_t;

/*
 * GLOBAL STATE
 *
 * All EmWeb/Server global state information is contained in the variable
 * ews_state with the following type.
 */
typedef struct EwsState_s
{
  EwsDocument file_hash_table[ EWS_FILE_HASH_SIZE ];

#   ifdef EW_CONFIG_OPTION_SCHED
  EwsLink     context_wait_list;      /* parsing & suspended requests */
  EwsLink     context_serve_list;     /* serving requests */
  EwsLinkP    context_serve_next;     /* next request to serve */
  EwsContext  context_current;        /* current context */
#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
  boolean     contextResumed;         /* true if a context is ewaResumed */
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
#   endif /* EW_CONFIG_OPTION_SCHED */

#ifdef EW_CONFIG_OPTION_AUTH
  EwsRealmP   realm_hash_table [ EWS_REALM_HASH_SIZE ];

#if defined(EW_CONFIG_OPTION_AUTH_DIGEST)
  EwsLink     nonce_list;             /* outstanding nonce */
  uint32      *nonce_index_bmap;      /* nonce table bmap */
  const char  *secret;                /* string used for nonce generation */
#endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

#endif /* EW_CONFIG_OPTION_AUTH */

  const EwsConstCharP *req_header_table;    /* table of request headers */
#   if defined(EW_CONFIG_OPTION_FORM) || defined(EW_CONFIG_OPTION_STRING_TYPED)
  const EwFormFieldTable *form_field_table; /* table of form field conversions */
#   endif /* EW_CONFIG_OPTION_FORM || STRING_TYPED */
#   ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
  const EwsConstCharP *form_data_header_table;   /* table of form-data headers */
#   endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */

#   ifdef EW_CONFIG_OPTION_PRIMARY
  EwsLink secondary_list; /* list of registered secondaries */
#   endif /* EW_CONFIG_OPTION_PRIMARY */

#   ifdef EW_CONFIG_OPTION_URL_HOOK_LIST
  EwsLink url_hook_list;  /* List of nodes pointing to the URL hooks. */
#   endif /* EW_CONFIG_OPTION_URL_HOOK_LIST */

#   ifdef EW_CONFIG_OPTION_SNMP_AGENT
  EwaSNMPAgentState   snmpAgent;
#   endif /* EW_CONFIG_OPTION_SNMP_AGENT */

#   ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
  EwsLink     file_system_list;
#   endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */

#   ifdef EW_CONFIG_OPTION_NS_LOOKUP
#   define EWS_SCHEMA_HASH_SIZE 256
  EwsLink     schema_hash_table[ EWS_SCHEMA_HASH_SIZE ];
#   endif /* EW_CONFIG_OPTION_NS_LOOKUP */

#   ifdef EW_CONFIG_OPTION_CLIENT
  struct EwsClientGlobal_s * client;
#   endif /* EW_CONFIG_OPTION_CLIENT */

#   if defined(EW_CONFIG_OPTION_CLI) 
  struct EwsCliCommand_s * super_root;
#   endif /* EW_CONFIG_OPTION_CLI */

#   ifdef EW_CONFIG_OPTION_UPNP
  struct upnp_archive_s * upnp_archive;
#   endif /* EW_CONFIG_OPTION_UPNP */
} EwsState;

#ifndef ews_state
extern EwsStateP ews_state;
#endif

extern const EwsConstCharP ews_request_header_table[];

extern EwaStatus ewsInternalAbort ( EwsContext context );
extern void ewsInternalSetup ( EwsContext context );
extern void ewsInternalCleanup ( EwsContext context );
extern void ewsInternalFinish ( EwsContext context );

#ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
extern const EwsConstCharP ews_form_data_header_table[];
#endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */


#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
/*
 * dynamic select feature:
 *
 *      maximum number of options (on submitt side)
 *      to be used if user did not provide EMWEB_MAX
 */

#define EWS_DYNAMIC_SELECT_MAX_CHOICES  16

#endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
/*
 * dynamic select feature:
 *
 *      maximum number of options (on submitt side)
 *      to be used if user did not provide EMWEB_MAX
 */

#define EWS_FORM_REPEAT_MAX_REPEATS  16

#endif /* EW_CONFIG_OPTION_FORM_REPEAT */

#endif /* _EWS_H_ */
