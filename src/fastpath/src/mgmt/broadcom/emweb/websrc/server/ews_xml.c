/*
 *
 * Product: EmWeb/Server
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
 *      XML Services of EmWeb
 */


#include "ews.h"
#include "ews_sys.h"
#include "ews_serv.h"
#include "ews_ctxt.h"
#include "ews_send.h"

#define _EWS_XML_C_
#include "ews_xml.h"

/* START-STRIP-EMWEB-LIGHT */

/* ================================================================
 * Types & Constants
 * ================================================================ */

/* ================================================================
 * Static Data
 * ================================================================ */

/* ================================================================
 * Static Function Declarations
 * ================================================================ */

#ifdef EW_CONFIG_OPTION_CONVERT_XML
static void setCurrentNamespace( EwsContext, const char *);
#endif


/* ================================================================
 * External Interfaces
 * ================================================================ */

#if defined( EW_CONFIG_OPTION_CONVERT_XML ) && defined( EW_CONFIG_OPTION_STRING )
void
ewsServeXMLEmWebStringBegin( EwsContext context
                            ,EwsDocumentContextP doc_context
                           )
{
  const char *name;
  uint32    indx;
  EwsDocumentNode *schema;

  /*
   * no processing if
   *        1) not converting to XML
   *        2) converting, but in XML Content block
   */
  if (!(context->flags & EWS_CONTEXT_FLAGS_CONVERT_XML))
    {
      return;
    }

  if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_STR_NAME_INFO)
    {
      /*
       * get the name from the schema node
       */
      indx = 0x00FFFFFF & EW_BYTES_TO_UINT32(doc_context->nodep->index);
      schema = (EwsDocumentNode *)
        (  ((const char *)doc_context->document->schema_nodes)
         + (SIZEOF_EWS_DOCUMENT_NODE * (indx - 1))
        );

      name = (const char *)
        &doc_context->document->archive_data[EW_BYTES_TO_UINT32(schema->index)];

      /*
       * enable the "body send" interfaces
       */
      context->flags &= ~EWS_CONTEXT_FLAGS_OUTPUT_OFF;

      /*
       * Set the namespace.  This may create a new  <data ns="emweb">
       * block in the output (if we are not in one already).
       */
      setCurrentNamespace( context, "emweb" );

      /*
       * send the start tag
       */
      ewsBodySend( context, "<object name=\"" );
      ewsBodySend( context, name );

      /* send VALUE=currentvalue if value present */

#     ifdef EW_CONFIG_OPTION_STRING_VALUE
      if ((name = ewsContextGetValue( context )) != NULL)
        {
          ewsBodySend( context, "\" param=\"" );
          ewsBodySend( context, name );
        }
#     endif /* EW_CONFIG_OPTION_STRING_VALUE */

      ewsBodySend( context, "\">" );        /* close the tag */
    }
}





void
ewsServeXMLEmWebStringEnd( EwsContext context
                            ,EwsDocumentContextP doc_context
                           )
{
  /*
   * no processing if
   *        1) not converting to XML
   *        2) converting, but in XML Content block
   */
  if (!(context->flags & EWS_CONTEXT_FLAGS_CONVERT_XML))
    {
      return;
    }

  if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_STR_NAME_INFO)
    {

      ewsBodySend( context, "</object>\n" );

      /*
       * disable the "body send" interfaces
       */
      context->flags |= EWS_CONTEXT_FLAGS_OUTPUT_OFF;
    }
}
#endif /* defined( EW_CONFIG_OPTION_CONVERT_XML ) && defined( EW_CONFIG_OPTION_STRING ) */



#if defined( EW_CONFIG_OPTION_CONVERT_XML ) && defined( EW_CONFIG_OPTION_NS_GET )
void
ewsServeXMLNamespaceBegin( EwsContext context
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
   * no processing if
   *        1) not converting to XML
   *        2) converting, but in XML Content block
   */
  if (!(context->flags & EWS_CONTEXT_FLAGS_CONVERT_XML))
    {
      return;
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

  /*
   * enable the "body send" interfaces
   */
  context->flags &= ~EWS_CONTEXT_FLAGS_OUTPUT_OFF;

  /*
   * Set the new namespace (terminate current namespace if different,
   * so we don't appear in the current namespace block (if there is one)
   */
  setCurrentNamespace( context, name );

  /*
   * send the start tag
   */
  ewsBodySend( context, "<object name=\"" );
  ewsBodySend( context, id );

  /* send VALUE=currentvalue if value present */

  if ((id = ewsContextGetValue( context )) != NULL)
    {
      ewsBodySend( context, "\" param=\"" );
      ewsBodySend( context, id );
    }

  ewsBodySend( context, "\">" );        /* close the tag */
}


void
ewsServeXMLNamespaceEnd( EwsContext context
                         ,EwsDocumentContextP doc_context
                         )
{
  EW_UNUSED( doc_context );

  /*
   * no processing if
   *        1) not converting to XML
   *        2) converting, but in XML Content block
   */
  if (!(context->flags & EWS_CONTEXT_FLAGS_CONVERT_XML))
    {
      return;
    }

  /* close the object */

  ewsBodySend( context, "</object>\n" );

  /*
   * disable the "body send" interfaces
   */
  context->flags |= EWS_CONTEXT_FLAGS_OUTPUT_OFF;
}
#endif /* defined( EW_CONFIG_OPTION_CONVERT_XML ) && defined( EW_CONFIG_OPTION_NS_GET )*/


#if defined( EW_CONFIG_OPTION_CONVERT_XML )
/**********************************************************************
 *
 *  ewsContextServeXML
 *
 *  Purpose: causes the current document to be served as XML
 *
 *  Inputs:  current context
 *
 *  Outputs: modifies current context
 *
 *  Returns: TRUE on success (always successful at this point).
 *
 */
boolean
  ewsContextServeXML( EwsContext context )
{
  context->flags |= EWS_CONTEXT_FLAGS_CONVERT_XML;
  return TRUE;
}

/**********************************************************************
 *
 *  ewsContextServingXML
 *
 *  Purpose: Boolean check to determine if the current document is
 *          being served in XML out mode.
 *
 *  Inputs:  current context
 *
 *  Outputs: none
 *
 *  Returns: true if xmlout is in effect, else false
 */
boolean
  ewsContextServingXML( EwsContext context )
{
  return (context->flags & EWS_CONTEXT_FLAGS_CONVERT_XML) != 0;
}


void
ewsServeXMLBeginConvert( EwsContext context )
{
  /*
   * no processing if
   *        1) not converting to XML
   *        2) converting, but in XML Content block
   */
  if (!(context->flags & EWS_CONTEXT_FLAGS_CONVERT_XML))
    {
      return;
    }

  /*
   * send <?xml ?> tag
   * Note: Even if this is a secure connection, the URL for the DTD uses
   * schema "http:", since this is not a link to it, but rather its name.
   */
  ewsBodySend( context
               ,"<?xml version=\"1.0\"?>\n<xmlout xmlns=\"http://www.emweb.com/xml/XMLout.dtd\">\n" );

  /* set "body send" routines to skip all outgoing data */
  context->flags |= EWS_CONTEXT_FLAGS_OUTPUT_OFF;
}

void
ewsServeXMLEndConvert( EwsContext context )
{

  if ((context->flags & EWS_CONTEXT_FLAGS_CONVERT_XML))
    {
      /* restore send routines */

      context->flags &= ~EWS_CONTEXT_FLAGS_OUTPUT_OFF;

      /* terminate namespace block (if there is one) */

      setCurrentNamespace( context, NULL );

      /* terminate xml block */

      ewsBodySend( context, "</xmlout>\n" );

      /* end xml translation */

      context->flags &= ~EWS_CONTEXT_FLAGS_CONVERT_XML;
    }
}
#endif /* EW_CONFIG_OPTION_CONVERT_XML */


/* ================================================================
 * Static Functions
 * ================================================================ */

#ifdef EW_CONFIG_OPTION_CONVERT_XML
/*
 * set the current namespace.  This may involve terminating
 * the "old" current namespace.  This can be used to just terminate
 * the current namespace by sending a NULL newNSName.
 * newNSName = new namespace to use, or NULL if no more namespaces
 */
static void
setCurrentNamespace( EwsContext context, const char *newNsName )
{
  boolean   differ = TRUE;

# ifdef EMWEB_SANITY
  /*
   * this code assumes that the caller has enabled output...
   */
  if (context->flags & EWS_CONTEXT_FLAGS_OUTPUT_OFF)
    {
      EMWEB_ERROR((__FILE__ ":setCurrentNamespace: called with output disabled!!\n"));
    }
# endif /* EMWEB_SANITY */

  /* see if same namespace */

  if (context->xmlCurrentNamespace && newNsName)
    {
      EMWEB_STRCMP( differ
                    ,context->xmlCurrentNamespace
                    ,newNsName
                  );
    }

  /* if different namespace */

  if (differ)
    {
      if (context->xmlCurrentNamespace)
        {
          /* terminate old namespace */
          ewsBodySend( context, "</data>\n" );
        }

      if (newNsName)
        {
          ewsBodySend( context, "<data ns=\"" );
          ewsBodySend( context, newNsName );
          ewsBodySend( context, "\">\n" );
        }
    }

  context->xmlCurrentNamespace = newNsName;
}

#endif /* EW_CONFIG_OPTION_CONVERT_XML*/


#ifndef EW_CONFIG_OPTION_CONVERT_XML
const char ews_xml_dummy = 0;      /* keep compiler from issuing an error for empty file */
#endif

#ifdef EW_CONFIG_OPTION_NS_LOOKUP
/*
 * ewsSchemaInfoRead
 * Read fields from EwsSchemaNode into EwsSchemaNodeInfo
 *
 *   nodep - pointer to schema node
 *   infop - (output) information from schema
 */
void
ewsSchemaInfoRead ( EwsSchemaNodeP nodep, EwsSchemaNodeInfoP infop )
{
  const EwsDocumentNode *dnp;
  uint32                offset;

    dnp = nodep->mynode;
    infop->type = dnp->type;
    switch(dnp->type)
      {
        case EW_DOCUMENT_NODE_TYPE_START_ELEMENT:
          {
            offset = EW_BYTES_TO_UINT32(dnp->data_offset);
            infop->ns_index    = offset >> 24;
            infop->nameSpace   = nodep->descriptor
                                   ->emweb_namespace_table[infop->ns_index].name;
            infop->object_id   = (const char *)
                                  &nodep->datap[EW_BYTES_TO_UINT32(dnp->index)];
            infop->name        = (const char *)
                                 &nodep->datap[EW_DOCNODE_GET_3BYTE_ATTR(dnp)];
            infop->data_offset = offset & 0x00FFFFFF;
            if (infop->name == (const char *) nodep->datap)
              {
                infop->name = infop->object_id;
              }
            break;
          }

        case EW_DOCUMENT_NODE_TYPE_EWSTR_SCHEMA:
          {
            infop->ns_index    = EWS_NAMESPACE_INDEX_EMWEB;
            infop->nameSpace   = EWS_NAMESPACE_EMWEB;
            infop->name        = (const char *)
                                 &nodep->datap[EW_BYTES_TO_UINT32(dnp->index)];
            infop->object_id   = infop->name;
            infop->data_offset = EW_BYTES_TO_UINT32(dnp->data_offset);
            break;
          }
        default:
          {
            infop->name = NULL; /* skip other schema nodes */
            break;
          }
      }
}

/*
 * ewsSchemaHashInit
 * Invoked by ewsInit() to initialize schema hash table
 */
void
ewsSchemaHashInit( void )
{
  uintf i;

    for (i = 0; i < EWS_SCHEMA_HASH_SIZE; i++)
      {
        EWS_LINK_INIT(&ews_state->schema_hash_table[i]);
      }
}


/*
 * ewsSchemaHash
 * Return hash function on symbol, simple modulo.  We reserve zero for
 * lookup by namespace.
 *
 * name       - schema object to hash
 */
uintf
ewsSchemaHash ( const char * name )
{
  uintf hash = 0;
  while (*name != '\0')
    {
      hash += *name++;
    }
  return 1 + (hash % (EWS_SCHEMA_HASH_SIZE - 1));
}

/*
 * ewsSchemaHashInsert
 * Create and insert a new entry into the hash table bucket
 *
 * index        - index of hash table in which to insert the entry
 * descriptor   - archive descriptor
 * datap        - archive data
 * schema_ndoes - schema nodes
 * mynode       - my node
 *
 * Returns pointer to new hash entry, or NULL if no resourses
 */
EwsSchemaNodeP
ewsSchemaHashInsert ( uintf            indx
                     ,EwsArchive       descriptor
                     ,EwsArchiveData   datap
                     ,const EwsDocumentNode *schema_nodes
                     ,const EwsDocumentNode *mynode )
{
  EwsSchemaNodeP  schema;
  EwsLinkP        hashp;

    schema = (EwsSchemaNodeP) ewaAlloc(sizeof(EwsSchemaNode));
    if (schema != (EwsSchemaNodeP) NULL)
      {
        hashp = &ews_state->schema_hash_table[indx];
        EWS_LINK_INSERT(&ews_state->schema_hash_table[indx], &schema->link);
        schema->descriptor   = descriptor;
        schema->datap        = datap;
        schema->schema_nodes = schema_nodes;
        schema->mynode       = mynode;
      }
    else
      {
        EMWEB_WARN(("ewsSchemaHashInsert: no resources\n"));
      }
    return schema;
}

/*
 * ewsSchemaHashLookup
 * Scan entries in hash table bucket looking for matching name.  Returns
 * hash entry or NULL
 *
 * index      - index into hash table
 * namespace  - name of namespace or NULL for EmWeb namespace
 * name       - name of schema object
 */
EwsSchemaNodeP
ewsSchemaHashLookup ( uintf      indx
                     ,const char *nameSpace
                     ,const char *name )
{
  EwsLinkP hashp = &ews_state->schema_hash_table[indx];
  EwsSchemaNodeP  schema;
  EwsSchemaNodeInfo   info;
  boolean         bvalue;

    /*
     * Scan hash table for matching entry
     */
    for ( schema = (EwsSchemaNodeP) EWS_LINK_HEAD(*hashp);
          schema != (EwsSchemaNodeP)EWS_LINK_END(*hashp);
          schema = (EwsSchemaNodeP) EWS_LINK_NEXT(schema->link) )
      {
        ewsSchemaInfoRead(schema, &info);

        /*
         * If this node is an EmWeb namespace, but we are searching for
         * a non-EmWeb namespace, then continue
         */
        if (info.ns_index == EWS_NAMESPACE_INDEX_EMWEB &&
            nameSpace != EWS_NAMESPACE_EMWEB)
          {
            continue;
          }

        /*
         * If object name doesn't match, then continue.  Special case -
         * skip name compare for hash bucket zero, we are only interested
         * in the namespace.
         */
        if (indx != EWS_NAMESPACE_HASH_INDEX)
          {
            EMWEB_STRCMP(bvalue, name, info.name);
            if (bvalue)
              {
                continue;
              }
          }

        /*
         * If the namespace name doesn't match, then continue
         */
        if (nameSpace != EWS_NAMESPACE_EMWEB)
          {
            EMWEB_STRCMP(bvalue
                        ,nameSpace
                        ,info.nameSpace);
            if (bvalue)
              {
                continue;
              }
          }

        /*
         * Match found
         */
        return schema;

      }
    return NULL;
}

/*
 * ewsSchemaInit
 * Build hash table for schema nodes
 *
 * descriptor   - archive descriptor containing schema
 * datap        - archive data containing schema
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewsSchemaInit ( EwsArchive     descriptor
               ,EwsArchiveData datap
               ,const EwsDocumentHeader *dhp
               ,const EwsDocumentNode   *node )
{
  uintf                   i;
  uintf                   hash;
  EwsSchemaNodeP          nodep;
  EwsSchemaNodeInfo       info;
  const EwsDocumentNode   *schema_nodes = node;

    /*
     * For each schema node
     */
    for (i = EW_BYTES_TO_UINT16(dhp->node_count); i > 0; i--)
      {
        EwsSchemaNode tmp;

        /*
         * Fake entry to read information directly out of node
         */
        tmp.descriptor = descriptor;
        tmp.datap      = datap;
        tmp.mynode      = node;

        ewsSchemaInfoRead(&tmp, &info);

        /*
         * If NULL name, then this schema node needs to be skipped
         */
        if (info.name == NULL)
          {
            node = NEXT_DOCNODE( node );
            continue;
          }

        /*
         * First, check to see if namespace is already present in hash[0].
         * Add if not.
         */
        if (info.nameSpace != NULL)
          {
            nodep = ewsSchemaHashLookup( EWS_NAMESPACE_HASH_INDEX
                                       , info.nameSpace
                                       , info.nameSpace );
            if (nodep == NULL)
              {
                nodep = ewsSchemaHashInsert( EWS_NAMESPACE_HASH_INDEX
                                            ,descriptor
                                            ,datap
                                            ,schema_nodes
                                            ,node );
                if (nodep == NULL)
                  {
                    return EWS_STATUS_NO_RESOURCES;
                  }
              }
          }

        /*
         * Check to see if schema is already present.
         */
        hash = ewsSchemaHash( info.name );
        nodep = ewsSchemaHashLookup( hash
                                    ,info.nameSpace
                                    ,info.name );
        /*
         * If not already in table, add new node
         */
        if (nodep == NULL)
          {
            nodep = ewsSchemaHashInsert ( hash
                                         ,descriptor
                                         ,datap
                                         ,schema_nodes
                                         ,node
                                        );
            if (nodep == NULL)
              {
                return EWS_STATUS_NO_RESOURCES;
              }
          }
#       ifdef EMWEB_SANITY
        /*
         * If colision, make sure that the archive matches.  This can
         * happen if a master and derived archive contain redundant
         * schema information.  If the archive is different, then we
         * have two master archives with overlapping schema information.
         * We should warn of a possible conflict.  Also, if we remove
         * the archive that was hashed, then the other archive will no
         * longer have schema information available.
         */
        else if (nodep->descriptor != descriptor)
          {
            EMWEB_WARN(("ewsSchemaInit: duplicate schema %s:%s\n"
                        , info.nameSpace, info.name));
          }
#       endif /* EMWEB_SANITY */

        node = NEXT_DOCNODE( node );
      }

    return EWS_STATUS_OK;
}

#ifdef EW_CONFIG_OPTION_CLEANUP
/*
 * ewsSchemaShutdown
 * Cleanup entire symbol table
 *
 *   descriptor - code archive descriptor, or NULL for all archives
 */
void
ewsSchemaShutdown ( EwsArchive descriptor )
{
  uintf i;
  EwsLinkP       hashp;
  EwsSchemaNodeP nodep;
  EwsSchemaNodeP nextNodep;

    for (i = 0; i < EWS_SCHEMA_HASH_SIZE; i++)
      {
        hashp = &ews_state->schema_hash_table[i];
        for (nodep  = (EwsSchemaNodeP)EWS_LINK_HEAD(*hashp);
             nodep != (EwsSchemaNodeP)EWS_LINK_END(*hashp);
             nodep  = nextNodep
             )
          {
            nextNodep = (EwsSchemaNodeP)EWS_LINK_NEXT(nodep->link);

            /* if this node points to the archive we're removing, zap it */
            if (descriptor == NULL || descriptor == nodep->descriptor )
              {
                EWS_LINK_DELETE(&nodep->link);
                ewaFree(nodep);
              }
          }
      }
}
#endif /* EW_CONFIG_OPTION_CLEANUP */

#if defined (EW_CONFIG_OPTION_NS_GET) && defined (EW_CONFIG_OPTION_STRING)
/*
 * ewsNamespaceGet
 * Called from EMWEB_STRING code fragment to execute an EmWeb macro or
 * namespace get function by reference.
 *
 *   context    - context pointer
 *   namespace  - name of namespace, or NULL for EmWeb namespace
 *   name       - name of object
 *   parameters - parameters to pass to macro
 *
 * Returns result to be returned from EMWEB_STRING code fragment, or
 * EWS_NAMESPACE_GET_ERROR if the lookup failed.
 */
#ifndef _cplusplus
const
#endif
char * EWS_NAMESPACE_GET_ERROR = "EWS_NAMESPACE_GET_ERROR";

char *
ewsNamespaceGet ( EwsContext   context
                 ,const char * nameSpace
                 ,const char * name
                 ,const char * parameters )
{
  uintf          hash;
  EwsSchemaNodeP nodep;
  EwsDocumentContextP doc_context;
# ifdef EW_CONFIG_OPTION_NS_GET
  char *         cp;
# endif /* EW_CONFIG_OPTION_NS_GET */

  EwsSchemaNodeInfo  info;
#if 0     /* LVL7_P0006 */
#   ifdef EMWEB_SANITY
    /*
     * Must only be called from EMWEB_STRING code fragment
     */
    if (context->state != ewsContextStateString)
      {
        EMWEB_WARN(("ewsNamespaceGet: bad state\n"));
        return (char*)EWS_NAMESPACE_GET_ERROR;
      }
#   endif /* EMWEB_SANITY */
#endif    /* LVL7_P0006 */
    /*
     * Lookup name in schema table
     */
    hash  = ewsSchemaHash(name);
    nodep = ewsSchemaHashLookup( hash
                                ,nameSpace
                                ,name );
    if (nodep == NULL)
      {
        return (char*)EWS_NAMESPACE_GET_ERROR;
      }
    ewsSchemaInfoRead(nodep, &info);

    /*
     * If EmWeb namespace, invoke string
     */
    if (nameSpace == EWS_NAMESPACE_EMWEB)
      {
        doc_context = context->doc_stack_tail;

#       ifdef EW_CONFIG_OPTION_STRING_VALUE
        /* Set parameters */
        doc_context->string_value = (char *) parameters;
#       endif /* EW_CONFIG_OPTION_STRING_VALUE */

        /* Update node index to inherit type information */
        context->node_index = info.data_offset;

        /* Invoke macro and return */
        return (char *)(*nodep->descriptor->emweb_string)
                         (context ,info.data_offset & 0x00ffffff);
      }

#   ifdef EW_CONFIG_OPTION_NS_GET
    /*
     * Otherwise, user namespace.  Invoke get function
     */
    else
      {
        context->node_index = 0; /* reset type information for namespace */
        context->currentElement = nodep->mynode;
        context->archiveData = nodep->datap;
        context->schemaNodes = nodep->schema_nodes;
        cp = (char *) (*nodep->descriptor->emweb_namespace_table
               [info.ns_index].get_f) (context
                                      ,info.object_id
                                      ,parameters);
        context->currentElement = NULL;
        return cp;
      }
#   endif /* EW_CONFIG_OPTION_NS_GET */
    return (char*)EWS_NAMESPACE_GET_ERROR;
}
#endif /* EW_CONFIG_OPTION_NS_GET || EW_CONFIG_OPTION_STRING */

#ifdef EW_CONFIG_OPTION_NS_SET
/*
 * The following functions give the user a mechanism to set the value of
 * namespace objects indirectly by name.  These functions mirror the three
 * per-namespace API functions used to implement sets.  These functions
 * can only be called from a valid context.  Note that if the per-namespace
 * set_finish function invokes ewsContextSendReply, then it should only be
 * invoked from a state in which this would be allowed.
 */

 /*
  * ewsNamespaceSetStart
  * Returns handle for atomic set operation, or NULL on error
  *
  *   context    - context of request
  *   namespace  - name of namespace
  *
  * Returns EWS_STATUS_OK or EWS_STATUS_NOT_FOUND;
  */
EwsStatus ewsNamespaceSetStart ( EwsContext context
                                 ,const char *nameSpace )
{
  EwsSchemaNodeP nodep;
  EwsSchemaNodeInfo  info;

# ifdef EMWEB_SANITY
  if (context->form_handle != NULL)
    {
      EMWEB_WARN(("ewsNamespaceSetStart: bad state\n"));
      return EWS_STATUS_BAD_STATE;
    }
# endif /* EMWEB_SANITY */

  /*
   * Lookup namespace to find master archive
   */
  nodep = ewsSchemaHashLookup(EWS_NAMESPACE_HASH_INDEX, nameSpace, nameSpace);
  if (nodep == NULL)
    {
      return EWS_STATUS_NOT_FOUND;
    }
  ewsSchemaInfoRead(nodep, &info);

  /*
   * Do namespace start
   */
  context->ns_set_name = nameSpace;
  context->form_ns = &nodep->descriptor->emweb_namespace_table
                        [ info.ns_index ];
  context->form_handle = (*context->form_ns->set_start_f)(context);
  if (context->form_handle == NULL)
    {
      return EWS_STATUS_NO_RESOURCES;
    }

  return EWS_STATUS_OK;
}

/*
 * ewsNamespaceSet
 * Set object value
 *
 *   context    - context of request
 *   name       - name of object to set
 *   parameters - qualifying parameters
 *   value      - new value
 *
 * Returns EWS_STATUS_OK or EWS_STATUS_NOT_FOUND.
 */
EwsStatus ewsNamespaceSet ( EwsContext context
                           ,const char *name
                           ,const char *parameters
                           ,const char *value )
{
  uintf hash;
  EwsSchemaNodeP nodep;
  EwsSchemaNodeInfo  info;

#   ifdef EMWEB_SANITY
    if (context->form_handle == NULL)
      {
        EMWEB_WARN(("ewsNamespaceSet: bad state\n"));
        return EWS_STATUS_BAD_STATE;
      }
#   endif /* EMWEB_SANITY */

    /*
     * Lookup name in schema table, return error if EmWeb namespace or
     * schema node not found.
     */
    hash  = ewsSchemaHash(name);
    nodep = ewsSchemaHashLookup( hash
                                ,context->ns_set_name
                                ,name );
    if (nodep != NULL)
      {
        ewsSchemaInfoRead(nodep, &info);
      }
    if (nodep == NULL || info.ns_index == EWS_NAMESPACE_INDEX_EMWEB)
      {
        return EWS_STATUS_NOT_FOUND;
      }

#   ifdef EMWEB_SANITY
    /*
     * Check for illegal nesting
     */
    else if (   context->form_ns
             != &nodep->descriptor->emweb_namespace_table [info.ns_index ] )
      {
        EMWEB_ERROR(("ewsNamespaceSet: illegal nesting detected\n"));
        return EWS_STATUS_BAD_STATE;
      }
#   endif /* EMWEB_SANITY */

    /*
     * Perform set
     */
    context->currentElement = nodep->mynode;
    context->archiveData = nodep->datap;
    context->schemaNodes = nodep->schema_nodes;
    (*context->form_ns->set_f)(context
                              ,context->form_handle
                              ,info.object_id
                              ,parameters
                              ,value);
    context->currentElement = NULL;
    return EWS_STATUS_OK;
}

/*
 * ewsNamespaceSetFinish
 * Complete atomic set operation.  This should be called from a state in
 * which ewsContextSendReply() is permitted, and should not be called from
 * a namespace form (since nesting is not allowed).
 *
 *   context - context of request
 *   status  - completion status
 *
 * Note that abort case is handled by server.
 */
EW_NSC char * ewsNamespaceSetFinish ( EwsContext context
                                     ,EwsStatus  status  )
{
  EW_NSC char * cp;

#   ifdef EMWEB_SANITY
    if (context->form_handle == NULL)
      {
        EMWEB_WARN(("ewsNamespaceSetFinish: bad state\n"));
        return NULL;
      }
#   endif /* EMWEB_SANITY */

    cp = (*context->form_ns->set_finish_f)(context
                                          ,context->form_handle
                                          ,status);
#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    if (context->schedulingState != ewsContextSuspended)
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
      {
        context->form_handle = NULL;
      }

    return cp;
}
#endif /* EW_CONFIG_OPTION_NS_SET */

#ifdef EW_CONFIG_OPTION_NS_GETNEXT
/*
 * ewsGetNextObject
 * Read object names from namespace
 *
 *   ns     - name of namespace
 *   object - name of object
 *
 * Returns next name of object or NULL
 */
const char *
ewsGetNextObject ( const char *ns, const char *obj )
{
  uintf hash;
  EwsSchemaNodeP nodep;
  EwsSchemaNodeInfo info;
  EwsLinkP hashp;
  boolean  bvalue;

    /*
     * If current object specified, look it up and get the next one
     */
    if (obj != NULL)
      {
        hash  = ewsSchemaHash(obj);
        nodep = ewsSchemaHashLookup( hash
                                    ,ns
                                    ,obj );
        if (nodep != NULL)
          {
            nodep = (EwsSchemaNodeP) EWS_LINK_NEXT(nodep->link);
          }
      }

    /*
     * Otherwise, start at the beginning
     */
    else
      {
        hash  = 1; /* 0 reserved for ns lookup */
        nodep = (EwsSchemaNodeP)
                EWS_LINK_HEAD(ews_state->schema_hash_table[hash]);
      }

    /*
     * Forever scan the hash table until the end
     */
    for (;;)
      {
        /*
         * If end of hash bucket, advance to next hash bucket
         */
        hashp = &ews_state->schema_hash_table[hash];
        if (nodep == NULL ||
            nodep == (EwsSchemaNodeP) EWS_LINK_END(*hashp))
          {
            hash++;
            if (hash == EWS_SCHEMA_HASH_SIZE)
              {
                return NULL;
              }
            nodep = (EwsSchemaNodeP)
                    EWS_LINK_HEAD(ews_state->schema_hash_table[hash]);
            continue;
          }

        /*
         * Read schema information and compare namespace
         */
        ewsSchemaInfoRead(nodep, &info);
        if (ns == EWS_NAMESPACE_EMWEB ||
            info.nameSpace == EWS_NAMESPACE_EMWEB)
          {
            bvalue = (info.nameSpace != ns);
          }
        else
          {
            EMWEB_STRCMP(bvalue, info.nameSpace, ns);
          }

        /*
         * If no name or mismatch namespace, keep scanning
         */
        if (info.name == NULL || bvalue)
          {
            nodep = (EwsSchemaNodeP) EWS_LINK_NEXT(nodep->link);
            continue;
          }

        /*
         * Return next object
         */
        return info.name;
      }
}
#endif /* EW_CONFIG_OPTION_NS_GETNEXT */

#endif /* EW_CONFIG_OPTION_NS_LOOKUP */

/* END-STRIP-EMWEB-LIGHT */

#ifndef EW_CONFIG_OPTION_NS_LOOKUP
int ews_no_ns_lookup;
#endif /* EW_CONFIG_OPTION_NS_LOOKUP */
#ifndef EW_CONFIG_OPTION_CONVERT_XML
int ews_no_convert_xml;
#endif /* EW_CONFIG_OPTION_CONVERT_XML */


/*
*** Local Variables: ***
*** mode: c ***
*** tab-width: 4 ***
*** End: ***
 */
