/*
 * Product:     EmWeb/Server
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
 *      Serve skeleton of EmWeb document
 */


#include "ews.h"
#include "ews_sys.h"
#include "ews_serv.h"
#include "ews_ctxt.h"
#include "ews_send.h"

#define _EWS_SKEL_C_
#include "ews_skel.h"

/* ================================================================
 * Types & Constants
 * ================================================================ */

/* ================================================================
 * Static Data
 * ================================================================ */

/* ================================================================
 * Static Function Declarations
 * ================================================================ */

/* ================================================================
 * External Interfaces
 * ================================================================ */
/* START-STRIP-EMWEB-LIGHT */
#if defined(EW_CONFIG_OPTION_CONVERT_SKELETON)
void
ewsServeSkeletonBeginConvert( EwsContext context )
{
  /*
   * No processing if not serving in skeleton mode.
   */
  if (!(context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON))
    {
      return;
    }

  /*
   * send <?xml ?> tag
   * Note: Even if this is a secure connection, the URL for the DTD uses
   * schema "http:", since this is not a link to it, but rather its name.
   */
  ewsBodySend(context,
                          "<?xml version=\"1.0\"?>\n"
                          "<skeleton xmlns=\"http://www.emweb.com/xml/skeleton.dtd\">\n");
}

void
ewsServeSkeletonEndConvert( EwsContext context )
{
  /*
   * No processing if not serving in skeleton mode.
   */
  if (!(context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON))
    {
      return;
    }

  ewsBodySend(context, "</skeleton>\n");
}
#endif /* defined(EW_CONFIG_OPTION_CONVERT_SKELETON) */


int32
ewsServeSkeletonCDATABegin(EwsContext context,
                                                   EwsDocumentContextP doc_context,
                                                   int32 available)
{
  EW_UNUSED(doc_context);
# define CDATA_LENGTH 12 /* == strlen("<![CDATA[") + strlen("]]>") */

  /*
   * No processing if not serving in skeleton mode.
   */
  if (!(context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON))
    {
      return available;
    }

  /* If the buffer has too few bytes to write a complete CDATA, return -1. */
  if (available < CDATA_LENGTH + 1)
    {
      return -1;
    }

  /* Start the CDATA block. */
  ewsBodySend(context, "<![CDATA[");

  return available - CDATA_LENGTH;
}

void
ewsServeSkeletonCDATAEnd(EwsContext context,
                                                 EwsDocumentContextP doc_context)
{
  EW_UNUSED(doc_context);
  /*
   * No processing if not serving in skeleton mode.
   */
  if (!(context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON))
    {
      return;
    }

  /* End the CDATA block. */
  ewsBodySend(context, "]]>");

  return;
}

#if defined(EW_CONFIG_OPTION_CONVERT_SKELETON) && defined(EW_CONFIG_OPTION_STRING)
boolean
ewsServeSkeletonEmWebString(EwsContext context,
                            EwsDocumentContextP doc_context)
{
  const char *name;
  uint32    index;
  EwsDocumentNode *schema;

  /*
   * No processing if not serving in skeleton mode.
   */
  if (!(context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON))
    {
      return FALSE;
    }

  if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_STR_NAME_INFO)
    {
      /*
       * get the name from the schema node
       */
      index = 0x00FFFFFF & EW_BYTES_TO_UINT32(doc_context->nodep->index);
      schema = (EwsDocumentNode *)
        (  ((const char *)doc_context->document->schema_nodes)
         + (SIZEOF_EWS_DOCUMENT_NODE * (index - 1))
        );

      name = (const char *)
        &doc_context->document->archive_data[EW_BYTES_TO_UINT32(schema->index)];

      /* Send the REFRESH tag. */
      ewsBodySend(context, "<dynamic ns=\"emweb\" id=\"");
      ewsBodySend(context, name);

      /* send VALUE=currentvalue if value present */
      if ((name = ewsContextGetValue(context)) != NULL)
        {
          ewsBodySend(context, "\" param=\"");
          ewsBodySend(context, name);
        }

      ewsBodySend(context, "\"/>");        /* close the tag */
    }

  return TRUE;
}
#endif /* defined( EW_CONFIG_OPTION_CONVERT_SKELETON ) && defined( EW_CONFIG_OPTION_STRING ) */



#if defined(EW_CONFIG_OPTION_CONVERT_SKELETON) && defined(EW_CONFIG_OPTION_NS_GET)
boolean
ewsServeSkeletonEmWebNamespace( EwsContext context
                                ,EwsDocumentContextP doc_context
                                )
{
  uint32 node_index;
  uintf  ns_index;
  uint32 offset;
  const char *name;
  const char *id;
  const EwsDocumentNode *schemaNode;

  /*
   * no processing if not outputting skeleton
   */
  if (!(context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON))
    {
      return FALSE;
    }

  /*
   * Find namespace name and schema node
   */
  node_index = EW_BYTES_TO_UINT32(doc_context->nodep->index);
  ns_index = (node_index >> 24);
  offset = (node_index & 0x00ffffff);

  name = doc_context->document->archive->emweb_namespace_table[ns_index].name;
  schemaNode =  (const EwsDocumentNode *)
      (  ((const char *)doc_context->document->schema_nodes)
       + (SIZEOF_EWS_DOCUMENT_NODE * (offset - 1))
      );

  /* get offset to name string */

  offset = EW_DOCNODE_GET_3BYTE_ATTR( schemaNode );

  if (!offset) /* no name available, use translated name */
    {
      offset = EW_BYTES_TO_UINT32(schemaNode->index);
    }
  id = (const char *) &doc_context->document->archive_data[offset];

  /* Send the REFRESH tag. */
  ewsBodySend(context, "<refresh ns=\"");
  ewsBodySend(context, name);
  ewsBodySend(context, "\" id=\"");
  ewsBodySend(context, id);

  /* send VALUE=currentvalue if value present */
  if ((id = ewsContextGetValue(context)) != NULL)
    {
      ewsBodySend(context, "\" param=\"");
      ewsBodySend(context, id);
    }

  ewsBodySend(context, "\">");        /* close the tag */

  return TRUE;
}
#endif /* defined(EW_CONFIG_OPTION_CONVERT_SKELETON) && defined(EW_CONFIG_OPTION_NS_GET)*/


#if defined( EW_CONFIG_OPTION_CONVERT_SKELETON )
/**********************************************************************
 *
 *  ewsContextServeSkeleton
 *
 *  Purpose: causes the current document to be served as skeleton
 *
 *  Inputs:  current context
 *
 *  Outputs: modifies current context
 *
 *  Returns: TRUE on success (always successful at this point).
 *
 */
boolean
  ewsContextServeSkeleton( EwsContext context )
{
  context->flags |= EWS_CONTEXT_FLAGS_CONVERT_SKELETON;
  return TRUE;
}

/**********************************************************************
 *
 *  ewsContextServingSkeleton
 *
 *  Purpose: Boolean check to determine if the current document is
 *          being served in skeleton mode.
 *
 *  Inputs:  current context
 *
 *  Outputs: none
 *
 *  Returns: true if skeleton mode is in effect, else false
 */
boolean
  ewsContextServingSkeleton( EwsContext context )
{
  return (context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON) != 0;
}
#endif /* EW_CONFIG_OPTION_CONVERT_SKELETON */

/* END-STRIP-EMWEB-LIGHT */


/* ================================================================
 * Static Functions
 * ================================================================ */

#ifndef EW_CONFIG_OPTION_CONVERT_SKELETON
const char ews_skeleton_dummy = 0;      /* keep compiler from issuing an error for empty file */
#endif


/*
*** Local Variables: ***
*** mode: c ***
*** tab-width: 4 ***
*** End: ***
 */
