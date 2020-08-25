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
 * EmWeb/Server: Serve HTTP response
 *
 */

#ifndef EWS_SERV_H_
#define EWS_SERV_H_

#include "ew_types.h"
#include "ew_config.h"
#include "ews_def.h"
#include "ew_conneg.h"

/* constants used by static ews(Search/Handle)MatchHdr fcns and calling fcns */
#ifdef EW_CONFIG_OPTION_CONDITIONAL_MATCH
typedef enum EwsMatchType_e
{
  IfMatch_Conditional,
  IfNoneMatch_Conditional
} EwsMatchType;

typedef enum EwsMatchStatus_e  /* used by MatchHdr fcns & calling fcns ! */
{
  EtagNoMatch,
  EtagWildCard,
  EtagMatched
} EwsMatchStatus;
#endif /* EW_CONFIG_OPTION_CONDITIONAL_MATCH */

typedef struct EwsVariantData_s {
  struct EwsVariantData_s *next;
  char                    *doc;
  char                    *mime_type;
  char                    *charset;
  char                    *language;
  uint32                  quality;
  boolean                 loaded;
} EwsVariantData;

extern uint32
ewsGetAttr(EwsDocument document, uint32 attr_block_offset, EwsAttr attr_type, uint8 *vary_hdr_flags);

extern uint32
ewsGetAttrBlkOffset( EwsDocument document);


/*
 * ewsServeStart
 * This function takes a context that has successfully completed header
 * parsing and starts processing of the request.
 *
 * context      - Context of request
 *
 * Returns TRUE if ready to crank the state machine again, FALSE if more
 * data is needed (e.g. the body of a request for form submission or raw
 * CGI interfaces).
 */
extern boolean ewsServeStart ( EwsContext context );

/*
 * ewsServeFindDocument
 * This function is the continuation of the ewsServeStart function.  It
 * is used to allow the the ewaURLHook call to suspend processing of
 * the context.  This function looks up the document associated with
 * the given URL
 *
 * context      - Context of request
 *
 * Returns TRUE if ready to crank the state machine again, FALSE if more
 * data is needed (e.g. the body of a request for form submission or raw
 * CGI interfaces).
 */
extern boolean ewsServeFindDocument( EwsContext context );

/*
 * ewsServeSetup
 * Setup state for processing request after document identified and loaded
 *
 * context      - context of request
 * document     - requested document
 * Returns TRUE if ready to crank the state machine again, FALSE if more
 * data is needed (e.g. the body of a request for form submission or raw
 * CGI interfaces).
 */
extern boolean ewsServeSetup ( EwsContext context, EwsDocument document );

/*
 * ewsServe
 * This function takes a context for which request processing has started and
 * cranks the state machine.
 *
 * context      - Context of request
 *
 * Returns EWA_STATUS_OK or EWA_STATUS_OK_YIELD.
 */
extern EwaStatus ewsServe ( EwsContext context );

/*
 * ewsServeInclude
 * This function pushes a new document onto the context processing stack.
 * The document must have already been determined to be loaded, of type
 * MIME or TEXT, etc.
 *
 * context      - request context
 * document     - document to serve
 */
extern EwsStatus ewsServeInclude ( EwsContext context, EwsDocument document );

/*
 * ewsServeDocument
 * This function takes a context for which request processing has started and
 * cranks the state machine.
 *
 * context      - Context of request
 *
 * Returns application status from ewaNetHTTPSend, or EWA_STATUS_OK on
 * success, EWA_STATUS_ERROR to abort.
 */
extern EwaStatus ewsServeDocument ( EwsContext context );

#ifdef EW_CONFIG_OPTION_FORM

/*
 * ewsServeSetupForm
 * Set up state to process FORM submission
 *
 * context      - context of request
 * document     - document containing FORM node for submission
 *
 * Returns TRUE if parser needs to be cranked again, FALSE otherwise.
 */
extern boolean ewsServeSetupForm ( EwsContext context, EwsDocument document );

/*
 * ewsServeSetupXMLP
 * Set up state to process XML parser submission
 *
 * context      - context of request
 * document     - document containing FORM node for submission
 *
 * No return value
 */
extern void ewsServeSetupXMLP ( EwsContext context, EwsDocument document );


/*
 * ewsServeSubmitForm
 * This function is called by the parser after parsing of the received
 * form data is complete.  At this point, the form is submitted to the
 * application and a response is generated.  The application returns a
 * URL to serve, or NULL for a default response.
 *
 * context      - context of request
 *
 * No return value
 */
extern void ewsServeSubmitForm( EwsContext context ) ;

#endif /* EW_CONFIG_OPTION_FORM */

#ifdef EW_CONFIG_OPTION_CGI

/*
 * ewsServeSetupCGI
 * Set up state to process CGI request
 *
 * context      - context of request
 * document     - CGI document
 *
 * No return value
 */
extern void ewsServeSetupCGI ( EwsContext context, EwsDocument document );

/*
 * ewsServeCGIData
 * Continue a CGI request by passing data to it.
 *
 * context      - context of request
 *
 * No return value
 */
extern void ewsServeCGIData( EwsContext context );
#endif /* EW_CONFIG_OPTION_CGI */

#ifdef EW_CONFIG_OPTION_IMAGEMAP
/*
 * ewsServeImageMap
 * Process imagemap
 *
 * context      - context of request
 * document     - document containing FORM node for submission
 *
 * No return value
 */
void ewsServeImageMap ( EwsContext context, EwsDocument document );
#endif /* EW_CONFIG_OPTION_IMAGEMAP */

#ifdef EW_CONFIG_OPTION_STRING

/*
 * ewsServeEmWebString
 * Handle <EMWEB_STRING> node
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Returns EwaStatus from application send function, or appropriate
 * status generated internally.
 */
extern EwaStatus ewsServeEmWebString
  ( EwsContext context, EwsDocumentContextP doc_context );

#endif /* EW_CONFIG_OPTION_STRING */

#ifdef EW_CONFIG_OPTION_STRING_VALUE

/*
 * ewsServeEmWebStringValue
 * Handle <EMWEB_STRING_VALUE> node
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Returns EwaStatus from application send function, or appropriate
 * status generated internally.
 */
extern EwaStatus ewsServeEmWebStringValue
  ( EwsContext context, EwsDocumentContextP doc_context );

#endif /* EW_CONFIG_OPTION_STRING_VALUE */

#ifdef EW_CONFIG_OPTION_INCLUDE

/*
 * ewsServeEmWebInclude
 * Handle <EMWEB_INCLUDE> node
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Returns EwaStatus from application send function, or appropriate
 * status generated internally.
 */
extern EwaStatus ewsServeEmWebInclude
  ( EwsContext context , EwsDocumentContextP );

#endif /* EW_CONFIG_OPTION_INCLUDE */

#ifdef EW_CONFIG_OPTION_FORM

/*
 * ewsServeEmWebForm
 * Handle FORM serve node
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Returns EwaStatus from application send function, or appropriate
 * status generated internally.
 */
extern EwaStatus ewsServeEmWebForm
  ( EwsContext context, EwsDocumentContextP doc_context ) ;

#endif /* EW_CONFIG_OPTION_FORM */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
/*
 * ewsServeEmWebDynamicSelect
 * Handle <SELECT ... EMWEB_ITERATE...> node
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Returns EwaStatus from application send function, or appropriate
 * status generated internally.
 */
EwaStatus
ewsServeEmWebDynamicSelect ( EwsContext context, EwsDocumentContextP doc_context ) ;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
/*
 * ewsServeEmWebPrefix
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Returns EwaStatus from application send function, or appropriate
 * status generated internally.
 */
EwaStatus
ewsServeEmWebPrefix ( EwsContext context, EwsDocumentContextP doc_context ) ;

/*
 * ewsServeEmWebRptBodyStart
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Returns EwaStatus from application send function, or appropriate
 * status generated internally.
 */
EwaStatus
ewsServeEmWebRptBodyStart ( EwsContext context, EwsDocumentContextP doc_context ) ;

/*
 * ewsServeEmWebRptBodyEnd
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Returns EwaStatus from application send function, or appropriate
 * status generated internally.
 */
EwaStatus
ewsServeEmWebRptBodyEnd ( EwsContext context, EwsDocumentContextP doc_context ) ;
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */

#ifdef EW_CONFIG_OPTION_NS_GET
/*
 * ewsServeEmWebNamespaceGet
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Returns EwaStatus from application send function, or appropriate
 * status generated internally.
 */
EwaStatus
ewsServeEmWebNamespaceGet ( EwsContext context, EwsDocumentContextP doc_context ) ;
#endif /* EW_CONFIG_OPTION_NS_GET */

#if defined (EW_CONFIG_OPTION_FORM) || defined (EW_CONFIG_OPTION_CGI)
/*
 * ewsServeRedirect
 * This function serves a local URL or redirects to a non-local URL from
 * an unscheduled request context
 *
 * context      - Request context
 * url          - URL redirect
 *
 * Returns EWS_STATUS_OK on success, else error code
 */
extern EwsStatus ewsServeRedirect (EwsContext context, char * url );
#endif /* EW_CONFIG_OPTION_FORM | EW_CONFIG_OPTION_CGI */


/*
 * ewsFreeDocContext
 * Free the memory allocated for the document context.  Be sure to
 * correctly free up substructures and other fields
 *
 *  doc_context - document context structure.
 */
extern void ewsFreeDocContext( EwsDocumentContextP doc_context );

#if defined(EW_CONFIG_OPTION_CONVERT_XML) || defined(EW_CONFIG_OPTION_ITERATE) || defined( EW_CONFIG_OPTION_IF)
extern void ewsSkipBlockContent( EwsContext context
                                ,EwsDocumentContextP doc_context
                                ,uintf endTag
                               );

/*
 * ewsSkipBlockCheck
 * Used by ewsSkipBlockContent to determine if it should keep skipping nodes or not.
 *  doc_context - document context structure
 *  startTag - node type of starting node
 *  endTag - node type of ending node
 */
boolean ewsSkipBlockCheck(EwsDocumentContextP doc_context,
                           uintf startTag,
                           uintf endTag);
#endif /* EW_CONFIG_OPTION_CONVERT_XML || EW_CONFIG_OPTION_ITERATE || EW_CONFIG_OPTION_IF */

#ifdef EW_OPTION_URL_HOOK
/*
 * ewsRelocateURL
 * Given a returned URL pointer (new_url) from a URL hook function,
 * and the URL pointer passed to the function (old_url), check to see if
 * we need to allocate memory and copy the URL into it (and then do so).
 */
extern char * ewsRelocateURL( EwsContext context
                             ,char *old_url
                             ,char *new_url);
#endif /* EW_OPTION_URL_HOOK */

extern void
ewsServeForceCloseLength(EwsContext context);

extern boolean
ewsDocumentIsDynamic( EwsDocument document );

#endif /* EWS_SERV_H_ */
