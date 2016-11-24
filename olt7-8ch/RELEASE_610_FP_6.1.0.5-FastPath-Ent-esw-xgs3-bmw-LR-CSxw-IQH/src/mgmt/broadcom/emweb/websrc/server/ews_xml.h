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
 *      Interfaces to the XML services of EmWeb
 */

#ifndef _EWS_XML_H_
#define _EWS_XML_H_  1  /* version # */


#ifndef _EWS_XML_C_
#   define _EXTERN_EWS_XML extern
#else
#   define _EXTERN_EWS_XML
#endif

/* ================================================================
 * Types & Constants
 * ================================================================ */

#ifdef EW_CONFIG_OPTION_NS_LOOKUP

/*
 * Schema hash table entry
 */
typedef struct EwsSchemaNode_s
  {
    EwsLink          link;         /* hash bucket linkage */
    EwsArchive      descriptor;    /* code archive descriptor */
    const uint8     *datap;        /* data archive */
    const EwsDocumentNode *schema_nodes; /* schema node list */
    const EwsDocumentNode *mynode;       /* this node */
  } EwsSchemaNode, *EwsSchemaNodeP;

/*
 * Schema information structure
 */
typedef struct EwsSchemaNodeInfo_s
  {
    uint8      type;
    uint8      _res;
    uint16     ns_index;
    const char *nameSpace;
    const char *name;
    const char *object_id;
    uint32     data_offset;
  } EwsSchemaNodeInfo, *EwsSchemaNodeInfoP;

#define EWS_NAMESPACE_INDEX_EMWEB 0
#define EWS_NAMESPACE_HASH_INDEX  0

#endif /* EW_CONFIG_OPTION_NS_LOOKUP */

/* ================================================================
 * External Data
 * ================================================================ */

/* ================================================================
 * Interfaces
 * ================================================================ */

_EXTERN_EWS_XML
void ewsServeXMLEmWebStringBegin( EwsContext context
                                  ,EwsDocumentContextP doc_context
                                  );

_EXTERN_EWS_XML
void ewsServeXMLEmWebStringEnd( EwsContext context
                                ,EwsDocumentContextP doc_context
                                );

_EXTERN_EWS_XML
void ewsServeXMLNamespaceBegin( EwsContext context
                                ,EwsDocumentContextP doc_context
                                );

_EXTERN_EWS_XML
void ewsServeXMLNamespaceEnd( EwsContext context
                              ,EwsDocumentContextP doc_context
                              );

/* ewsContextServeXML and ewsContextServingXML are declared in
 * include/ews_ctxt.h. */

_EXTERN_EWS_XML
void ewsServeXMLBeginConvert( EwsContext context );

_EXTERN_EWS_XML
void ewsServeXMLEndConvert( EwsContext context );

#ifdef EW_CONFIG_OPTION_NS_LOOKUP

_EXTERN_EWS_XML
void ewsSchemaInfoRead(const EwsSchemaNodeP nodep, EwsSchemaNodeInfoP infop);

_EXTERN_EWS_XML
void ewsSchemaHashInit ( void );

_EXTERN_EWS_XML
uintf ewsSchemaHash ( const char *name );

_EXTERN_EWS_XML
EwsSchemaNodeP
ewsSchemaHashInsert ( uintf            indx
                     ,EwsArchive       descriptor
                     ,EwsArchiveData   datap
                     ,const EwsDocumentNode *schema_nodes
                     ,const EwsDocumentNode *mynode );

_EXTERN_EWS_XML
EwsSchemaNodeP ewsSchemaHashLookup ( uintf indx
                                    ,const char *nameSpace
                                    ,const char *name );
_EXTERN_EWS_XML
EwsStatus ewsSchemaInit( EwsArchive        descriptor
                        ,EwsArchiveData    datap
                        ,const EwsDocumentHeader *schema_doc
                        ,const EwsDocumentNode   *schema_nodes
                       );

_EXTERN_EWS_XML
void ewsSchemaShutdown ( EwsArchive descriptor );

#endif /* EW_CONFIG_OPTION_NS_LOOKUP */

/*
 *** Local Variables: ***
 *** mode: c ***
 *** tab-width: 4 ***
 *** End: ***
 */
#endif /* _EWS_XML_H_ */
