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
 * EmWeb database definitions
 *
 */
#ifndef _EW_DB_H
#define _EW_DB_H

#include "ew_common.h"
#include "ew_types.h"
#include "ew_form.h"
#include "ew_imap.h"
#include "ews_def.h"
#include "ews_cgi.h"

/* START-SOURCE-ONLY */

/******************************************************************************
 *
 * DOCUMENT ARCHIVE
 *
 * An EmWeb document archive contains two parts.  These are the archive data
 * component and the archive object component.
 *
 * The archive object component contains all the run-time object code
 * associated with an archive.  A public symbol (determined by EmWeb/Compiler
 * on a per-archive basis) points to an archive descriptor (EwsArchive).  This
 * contains all of the linkage required by the EmWeb/Server to access the
 * archive-specific run-time object code.
 *
 * The archive data component contains all the static data describing
 * individual documents in the archive.  The data component is an endian-
 * independent fully relocatable contiguous block of constant data
 * analogous to a file.  In fact, it could be a file.  The data component
 * begins with an archive header and contains one or more documents.
 *****************************************************************************/

/*
 * Archive Descriptor
 *
 * Each archive has a unique archive descriptor in the object component of the
 * archive.  The archive descriptor is a list of functions called by the
 * EmWeb/Server to execute archive-specific object code while processing
 * requests for documents in the archive.  Unsupported functions (i.e.
 * features not selected when the EmWeb/Compiler was built) are represented
 * by a NULL pointer.
 */

/* END-SOURCE-ONLY */

typedef const void * EmWebString_f    ( EwsContext context, uint32 indx );
typedef const char * EmWebInclude_f   ( EwsContext context, uint32 indx );
typedef void EmWebLoad_f              ( boolean *done );
typedef void EmWebUnload_f            ( boolean *done );

#ifdef EW_CONFIG_OPTION_UPNP

typedef EwaStatus EmWebUPnPGetVar_f   ( EwsContext                   context
                                       ,EwaUPnPServiceHandle         handle
                                       ,int                          var
                                       ,void                       *valuepp
                                      );
#ifdef EW_CONFIG_OPTION_SOAP
/*
 * Application Action function
 *
 * context - request context
 * handle  - application service handle
 * form    - pointer to form-specific structure
 *
 */
typedef EwaStatus EmWebUPnPAction_f ( EwsContext context
                                     ,EwaUPnPServiceHandle handle
                                     ,void * form
                                    );
#endif /* EW_CONFIG_OPTION_SOAP */

#endif /* EW_CONFIG_OPTION_UPNP */

/* Cookie Attribute structure, element of emweb_cookie_tbl[]            */
typedef struct EwCookieAttr_s {
    const char *name;
    const char *path;
} EwCookieAttributes;


/* NOTE: CGI depricated.  Use filesystem API for raw CGI applications */
typedef struct EmWebCGITable_s
  {
    EwaCGIStart_f     *start_f;
    EwaCGIData_f      *data_f;
  } EmWebCGITable, *EmWebCGITableP;

typedef struct EmWebNamespaceTable_s
  {
    EwaNamespaceGet_f       *get_f;
    EwaNamespaceSetStart_f  *set_start_f;
    EwaNamespaceSet_f       *set_f;
    EwaNamespaceSetFinish_f *set_finish_f;
    const char              *name;
  } EmWebNamespaceTable, *EmWebNamespaceTableP;

#ifdef EW_CONFIG_OPTION_XMLP
#include "ews_xmlp.h"
/*
 * XML parser table definitions.  Note, if XML parser documents are specified
 * in archive at EmWeb/Compiler time, but EW_CONFIG_OPTION_XMLP not defined
 * in EmWeb/Server, then a compile-time error will occur when building the
 * archive ew_code.c.  We detect this here.  The EmWeb/Compiler generates
 * EMWEB_COMPILER_XMLP in ew_code.c if XML parser documents are specified.
 */
typedef struct EmWebXmlpTable_s
  {
    uint32               *events;
    XmlpPolicyWhitespace *defaultWhitespaceHandlingPolicy;
    XmlpPolicyWhitespace *attributeWhitespaceHandlingPolicy;
    XmlpPolicyNamespace  *namespaceHandlingPolicy;
    const char           **namespaces;
    int                  *implicitNamespace;
    XmlcStart_f          *pfXmlpConsumerStart;
    XmlcEvent_f          *pfXmlpConsumerEvent;
    XmlcEnd_f            *pfXmlpConsumerEnd;
    XmlcAbort_f          *pfXmlpConsumerAbort;
    void                 *consumer; /* filled in by EmWeb/Server on install */
  } EmWebXmlpTable, *EmWebXmlpTableP;

#else /* EW_CONFIG_OPTION_XMLP */
#  ifdef EMWEB_COMPILER_XMLP
#error Archive specifies XML parser documents, must set EW_CONFIG_OPTION_XMLP
#  else /* EMWEB_COMPILER_XMLP */
     typedef void * EmWebXmlpTable;
#  endif /* EMWEB_COMPILER_XMLP */
#endif /* EW_CONFIG_OPTION_XMLP */

#define EW_XMLP_INDEX_INTERNAL_SOAP     0xffff
#define EW_XMLP_INDEX_INTERNAL_SSDP_RSP 0xfffe
#define EW_XMLP_INDEX_INTERNAL_EVENT    0xfffd

#ifdef EW_CONFIG_OPTION_UPNP
typedef struct EmWebUPnPVarTable_s
  {
    const char * varName;              /* name of variable */
    EwsSchemaRenderingCode renderCode; /* rendering code of variable */
    boolean      sendEvents;           /* TRUE if variable generates events */
  } EmWebUPnPVarTable, *EmWebUPnPVarTableP;

typedef struct EmWebUPnPTable_s
  {
    EmWebUPnPGetVar_f *getVar;      /* application get variable function */
    const char        *serviceName; /* name of UPnP service */
    EmWebUPnPVarTable *varTable;    /* pointer to table of variables */
  } EmWebUPnPTable, *EmWebUPnPTableP;
#else /* EW_CONFIG_OPTION_UPNP */
#  ifdef EMWEB_COMPILER_UPNP
#    error Archive specifies UPnP documents, must set EW_CONFIG_OPTION_UPNP
#  else /* EMWEB_COMPILER_UPNP */
     typedef void * EmWebUPnPTable;
#  endif /* EMWEB_COMPILER_UPNP */
#endif /* EW_CONFIG_OPTION_UPNP */

#define EW_UPNP_FIELD_OUTPUT ((void *) &ew_upnp_field_output)
#define EW_UPNP_FIELD_INPUT  ((void *) &ew_upnp_field_input)
extern uint32 ew_upnp_field_output;
extern uint32 ew_upnp_field_input;


/* START-EMWEB-MAGIC */
#define EW_STANDALONE_ARCHIVE_MAGIC_0   'E'
#define EW_STANDALONE_ARCHIVE_MAGIC_1   'W'

#define EW_ARCHIVE_MAGIC_0   EW_STANDALONE_ARCHIVE_MAGIC_0
#define EW_ARCHIVE_MAGIC_1   EW_STANDALONE_ARCHIVE_MAGIC_1
/* END-EMWEB-MAGIC */

typedef struct EwsArchive_s
  {
    uint32      magic;          /* Magic number for archive validation */

#   define EW_ARCHIVE_CODE_MAGIC_2   'o'
#   define EW_ARCHIVE_CODE_MAGIC_3   'b'

#   define EW_ARCHIVE_OBJECT_MAGIC      (((uint32)EW_ARCHIVE_MAGIC_0<<24)|((uint32)EW_ARCHIVE_MAGIC_1<<16)|((uint16)EW_ARCHIVE_CODE_MAGIC_2<<8)|EW_ARCHIVE_CODE_MAGIC_3)

    uint8       version_maj;    /* Archive version (major/minor) */
    uint8       version_min;

#   define EW_ARCHIVE_VERSION_MAJ       2
#   define EW_ARCHIVE_VERSION_MIN       2

    uint8       compiler_maj;   /* EmWeb/Compiler version (major/minor) */
    uint8       compiler_min;

#   define EW_ARCHIVE_DATE_1123_SIZE    32

    char        date_1123[EW_ARCHIVE_DATE_1123_SIZE];   /* creation date */

    /*
     * Archive-specific <EMWEB_STRING> dispatch function.  Takes context
     * and index arguments.  Index selects the C-code fragment corresponding
     * to an instance of <EMWEB_STRING>.
     */
    EmWebString_f       *emweb_string;

    /*
     * Archive-specific <EMWEB_INCLUDE> dispatch function.  Takes context
     * and index arguments.  Index selects the C-code fragment corresponding
     * to an instance of <EMWEB_INCLUDE>.
     */
    EmWebInclude_f      *emweb_include;

    /*
     * Archive-specific form table. The Form node index is hierarchical and
     * contains a form number and an element number.  The form table is
     * indexed by form number (0..n-1).  Each form table entry contains a
     * pointer to an array of field elements indexed by one less than the
     * element number.  (An element number of zero starts the form, and
     * an element number greater than the number of elements in the form
     * ends the form).  The table contains all the necessary information to
     * serve and submit forms.  The emweb_form_enum_table is a list of strings
     * corresponding to RADIO and single SELECT values.
     */
    const EwFormEntry *emweb_form_table;
    uint32       emweb_form_table_size;
    const char   **emweb_form_enum_table;
    uint32       emweb_form_enum_table_size;

    /*
     * Archive-specific imagemap table.  The table represents imagemaps
     * compiled by the EmWeb/Compiler into the archive.  This table is
     * indexed by a document node corresponding to the mapfile.  The
     * table entry contains a table of rectangular regions which are scanned
     * for matching x,y coordinates to find a match.
     */
    const EwImageMapTable *emweb_image_table;
    uint32                emweb_image_table_size;

    /*
     * Archive-specific CGI action dispatch table.
     * NOTE: CGI depreicated.  Use raw file system API instead.
     */
    const EmWebCGITable *emweb_cgi_table;
    uint32              emweb_cgi_table_size;

    /*
     * Reference count - this is initialized to zero by the EmWeb/Compiler,
     * but is writable (.data section).  This value is incremented for each
     * cloned page It is also incremented for each page currently in use by an
     * uncompleted request.  This prevents the application from attempting to
     * remove an archive that is in use.
     */
    uint32              reference_count;

    /*
     * Document list - this is the anchor of a list of loaded documents and
     * is used exclusively by EmWeb/Server to keep track of per-document nodes
     * created when loading the archive into the open hashed file system.
     * This should be set to EWS_DOCUMENT_NULL by the EmWeb/Compiler.
     */
    EwsDocument         document_list;

    /*
     * Archive-specific cookie table. This table is indexed by data_offset
     * value find in the cookie header node.
     */
    const EwCookieAttributes *emweb_cookie_tbl;

    /*
     * Archive-specific namespace action dispatch table.
     */
    EmWebNamespaceTable *emweb_namespace_table;
    uint32               emweb_namespace_table_size;


    /*
     * Archive-specific <EMWEB_LOAD> function.
     */
    EmWebLoad_f         *emweb_load;

    /*
     * Archive-specific <EMWEB_UNLOAD> function.
     */
    EmWebUnload_f       *emweb_unload;

    /*
     * Archive-specific XML parser dispatch table
     */
    EmWebXmlpTable      *emweb_xmlp_table;
    uint32               emweb_xmlp_table_size;

    /*
     * Archive-specific UPnP Service Variable table
     */
    EmWebUPnPTable      *emweb_upnp_table;
    uint32               emweb_upnp_table_size;

    /*
     * NOTE!!! For backward compatibility, new fields must be added at the
     * end of this structure.
     */

  } EwsArchive_t;

/* START-SOURCE-ONLY */
/*
 * DOCUMENT ARCHIVE DATA COMPONENT
 *
 * The document archive data component is a relocatable, endian-independent,
 * static data structure containing compressed documents and information about
 * how to access them.
 *
 * All 16-bit and 32-bit quantities are represented as arrays of octets.  The
 * first octet is the most significant (i.e. Big Endian).
 *
 * All pointers are 32-bit (array of four octets) offsets from the beginning of
 * the archive data.
 *
 * All strings are .asciiz ('\0'-terminated ASCII) and are represented by a
 * pointer to the string.
 *
 * Data storage for strings and various other variable-length components can
 * be placed between headers at the discretion of the EmWeb/Compiler.
 *
 * The document archive data component takes the following form:
 *
 *   EwsArchiveHeader
 *   (optional padding)
 *     { EwsDocumentHeader { EwsDocumentNode, ... } (optional padding) }, ...
 *
 * The optional padding can be used to store variable length fields such
 * as strings, compressed documents, etc.
 */

/*
 * EwsArchiveHeader
 *
 * The archive data component begins with an archive header at offset zero
 * NOTE WELL: KEEP SIZEOF_EWS_ARCHIVE_HEADER UP TO DATE!!!
 */
typedef struct EwsArchiveHeader_s
  {

    uint8       magic[4];       /* Magic number for archive validation */

#   define EW_ARCHIVE_DATA_MAGIC_2   'd'
#   define EW_ARCHIVE_DATA_MAGIC_3   'a'
#   define EW_ARCHIVE_DATA_MAGIC (((uint32)EW_ARCHIVE_MAGIC_0<<24)|((uint32)EW_ARCHIVE_MAGIC_1<<16)|((uint16)EW_ARCHIVE_DATA_MAGIC_2<<8)|EW_ARCHIVE_DATA_MAGIC_3)

    uint8       version_maj;    /* Archive version (major/minor) */
    uint8       version_min;

    uint8       compiler_maj;   /* EmWeb/Compiler version (major/minor) */
    uint8       compiler_min;

    uint8       length[4];      /* length of archive, octets */
    uint8       name_offset[4]; /* Offset to archive name string */
    uint8       date_1123_offset[4]; /* Offset to HTTP date (RFC1123 format) */
    uint8       date_1036_offset[4]; /* Offset to HTTP date (RFC1036 format) */
    uint8       doc_offset[4];  /* Offset to first document header */
    uint8       dict_offset[4]; /* Offset to compression dictionary */
    uint8       code_date_1123_offset[4]; /* Offset to subsidiary archive date */

  } EwsArchiveHeader, * EwsArchiveHeaderP;

#define SIZEOF_EWS_ARCHIVE_HEADER 36  /* NEVER use sizeof() */

/*
 * EwsDocumentHeader
 *
 * For each document in the archive, there is a corresponding document
 * header.  The first document header is referenced by the doc_offset
 * field of the archive header.  Subsequent documents are chained together
 * using the next_offset field in each document header.  The last document
 * header in the archive contains a zero in the next_offset field.
 *
 * The following types of documents are supported:
 *
 *   Link       - Generates a redirection URL.  The redirection URL is an
 *                uncompressed string stored in the data area.  node_count
 *                should be zero.
 *
 *   Mime       - Generic (non-text) mime type.  The data area contains the mime
 *                document.  The mime content is indicated by the string at
 *                mime_offset.  Compression is optional.  node_count should be
 *                zero.
 *
 *   Text       - Generic text type.  The text may contain EmWeb/Compiler
 *                HTML extensions for <EMWEB_STRING>, <EMWEB_INCLUDE>,
 *                form get processing, etc.  Each such extension results in
 *                one or more document nodes being present describing the
 *                actions to take at certain points during the processing of
 *                the data.  The data contains the static parts of the
 *                document and may optionally be compressed.  The mime content
 *                is indicated by the string at mime_offset.
 *
 *   Index      - An index URL.  An absolute path URL is stored as an
 *                uncompressed string in the data area.  node_count should be
 *                zero.
 *
 *   Schema     - Archives with Namespaces or archive symbols contain a
 *                schema document.  The schema document exists as a
 *                placeholder for schema nodes.
 *
 *   Form       - A form action URL.  There should not be a data section.
 *                There will be a FORM node that indicates the index to
 *                use for the start/set/submit operations in handling a
 *                posted form.
 *
 *   CGI        - NOTE: This API has been depricated.  Use raw filesystem
 *                run-time APIs instead.  Historically:
 *                A CGI URL.  There will be a CGI node that indicates the
 *                index to use for the start/data operations in handling a
 *                raw CGI request.  An optional data section containing
 *                the uncompressed content of the corresponding URL file
 *                (if present) is made available in the EwsContext.  This
 *                could be used for CGI imagemap data, for example.
 *
 *   Imagemap   - A mapfile URL.  There will be an imagemap node that
 *                indicates the index into a per-archive imagemap table
 *                for processing an imagemap.
 *
 *   XMLP       - An XML parser URL.  The extend bits indicate the index
 *                index into a per-archive XML parser table for selecting
 *                an XML parser consumer.  Some values are reserved for
 *                internal consumers.
 *
 * Note that any document might be marked hidden.  If so, the document is
 * not accessible by a client browser directly.  Instead, the document is
 * used as a building block.  It can be cloned, or EMWEB_INCLUDEd.
 *
 * NOTE WELL: KEEP SIZEOF_EWS_DOCUMENT_HEADER UP TO DATE!!
 */
typedef struct EwsDocumentHeader_s
  {
    uint8       next_offset[4]; /* offset to next document or 0 if last */
    uint8       url_offset[4];  /* offset to URL string */

    uint8       document_type;  /* type of document */

    /* If you add a document type, update ../compiler/readarchive.c */
#   define EW_ARCHIVE_DOC_TYPE_MASK     0x0F    /* type field */
#   define EW_ARCHIVE_DOC_TYPE_LINK     0x00    /* redirection URL */
#   define EW_ARCHIVE_DOC_TYPE_MIME     0x01    /* generic mime data type */
    /*     EW_ARCHIVE_DOC_TYPE_TEXT     0x02     * [obsolete] */
#   define EW_ARCHIVE_DOC_TYPE_FORM     0x04    /* Form action URL */
#   define EW_ARCHIVE_DOC_TYPE_INDEX    0x05        /* index URL */
#   define EW_ARCHIVE_DOC_TYPE_SCHEMA   0x06    /* schema document */
#   define EW_ARCHIVE_DOC_TYPE_CGI      0x08    /* CGI-BIN (depricated) */
#   define EW_ARCHIVE_DOC_TYPE_IMAGEMAP 0x09    /* Imagemap interface */
#   define EW_ARCHIVE_DOC_TYPE_FILE     0x0A    /* local filesystem */
#   define EW_ARCHIVE_DOC_TYPE_XMLP     0x0B    /* XML parser */
#   define EW_ARCHIVE_DOC_FLAG_NR       0x10    /* Negotiable Resource */
#   define EW_ARCHIVE_DOC_FLAG_STATIC   0x20    /* treat as static document */
#   define EW_ARCHIVE_DOC_FLAG_FORM     0x40    /* contains form node */
#   define EW_ARCHIVE_DOC_FLAG_HIDDEN   0x80    /* hidden (internal use) */

    uint8       document_flags;

#   define EW_ARCHIVE_DOC_FLAG_EW_COMPRESS  ((uint8) 0x01)  /* doc compressed */
#   define EW_ARCHIVE_DOC_FLAG_SDD          ((uint8) 0x02)  /* SDD document */
#   define EW_ARCHIVE_DOC_FLAG_SOAP_ACTION  ((uint8) 0x04)  /* SOAP ACTION doc*/
#   define EW_ARCHIVE_DOC_FLAG_PATH_INFO    ((uint8) 0x80)  /* extra URL OK */
#   define EW_ARCHIVE_COMPRESSION_DEFAULT   EW_ARCHIVE_DOC_FLAG_EW_COMPRESS

    uint8       node_count[2];  /* number of document nodes */
    uint8       node_offset[4]; /* offset to array of document nodes */

    uint8       mime_offset[4]; /* offset to mime content type string */
    uint8       realm_offset[4];/* offset to authentication realm string */

    uint8       orig_length[4]; /* length of original (uncompressed) data */
    uint8       data_length[4]; /* length of compressed data */
    uint8       data_offset[4]; /* offset to compressed data */

    uint8       hdr_node_count; /* number of document header nodes */
    uint8       hdr_flags;      /* header specific flags (below)     */
    /*   Header Flag Name                 Value     Header Node Type */
# define EW_ARCHIVE_DOC_HDR_COOKIE_FLG     0x01  /* COOKIE           */
# define EW_ARCHIVE_DOC_HDR_SCACHE_FLG     0x02  /* STATIC_ETAG      */
# define EW_ARCHIVE_DOC_HDR_VARIANTS_FLG   0x04  /* VARIANTS         */
# define EW_ARCHIVE_DOC_HDR_ATTRIBUTES_FLG 0x08  /* ATTRIBUTES       */
# define EW_ARCHIVE_DOC_HDR_DYN_ETAG_FLG   0x10  /* DYN_ETAG         */

    /*
     * Extension (was reserved before archive version 2.2, now used to add
     * 16 bites of additional information to a document as a more efficient
     * method than inserting a new node for certain uses.
     *
     * Currently defined uses for extend are as follows:
     *
     *   1. If (document_flags & EW_ARCHIVE_DOC_FLAG_SOAP_ACTION), then
     *      the extend[0-1] represents the 16-bit index into the emweb_form
     *      table.
     *
     *   2. If document_type == EW_ARCHIVE_DOC_TYPE_XMLP, then the extend[0-1]
     *      represents the 16-bit index into the emweb_xmlp_table (or a
     *      reserved index used for internal purposes, such as the SOAP
     *      parser).
     */
    uint8       extend[2];
  } EwsDocumentHeader, * EwsDocumentHeaderP;

#define SIZEOF_EWS_DOCUMENT_HEADER 40   /* never use sizeof */

/*
 * ewsDocumentNode
 *
 * The document header points to an array of zero or more document nodes
 * followed by header nodes.
 * The node_count field of the document header indicates the number of nodes
 * present.  Each node represents a special run-time action to be taken by the
 * EmWeb/Server when processing requests for the document.  When processing
 * reaches the specified point in the uncompressed data, the specified action
 * is taken.
 *
 * Header node contains information needed to create HTTP reply header(s).
 * The hdr_count field of the document header indicates the number of hdr_nodes
 * present.  Each node represents a special run-time action to be taken by the
 * EmWeb/Server when processing requests for the document. Header nodes are
 * processed first as the headers are built into the reply befor data part
 * of the document is processed.
 * Header nodes are sorted in the archive by type. Compiler sorts and includes
 * in the archive only headers of the types for which the flag is set in
 * hdr_flags field in the Document Header.
 *
 * The document header node(s) immediately follow the ewsDocumentNodes for a
 * given document.
 *
 * NOTE WELL:  KEEP SIZEOF_EWS_DOCUMENT_NODE UP TO DATE!!!
 */
typedef struct EwsDocumentNode_s
  {
    uint8       data_offset[4]; /* Location of node in uncompressed data
                                   stream (relative to start of uncompressed
                                   document data), or for header nodes...

                                   COOKIE header: index to CookieTable.

                                   STATIC_ETAG header: offset to ETag value.

                                   DYN_ETAG header: reserved for future use,
                                   e.g. cache control header.

                                   VARIANT header: offset to variant block.
                                   vb[0-3] = pointer to alternates header,
                                   vb[4-7] = number of variants,
                                   vb[8-11] = offset to first variant's
                                   attribute block, etc.  Where
                                   ab[0-3] = offset to name (URL),
                                   ab[4-7] = offset to content-type,
                                   ab[8-11] = offset to charset,
                                   ab[12-15] = offset to encoding,
                                   ab[16-19] = offset to language,
                                   ab[20-23] = quality value.

                                   or for schema nodes...

                                   START_ELEMENT (namespace schema node):
                                   Composite of namespace index (MSB-8) and
                                   index in schema nodes of format (LSB-24).

                                   FORMAT (namespace format node):
                                   Application format.

                                   EWSTR_SCHEMA: (EmWeb String schema node):
                                   32-bit composite index of emweb_string.
                                */

    uint8       type;           /* type of node */

    /* If you add a node type, update:
     *  ../websrc/compiler/readarchive.c
     *  ../websrc/compiler/ew_archive.c (esp. ResolveMacros())
     */
#   define EW_DOCUMENT_NODE_TYPE_UNKNOWN        0       /* do not use */
#   define EW_DOCUMENT_NODE_TYPE_STRING         1       /* <EMWEB_STRING> */
#   define EW_DOCUMENT_NODE_TYPE_INCLUDE        2       /* <EMWEB_INCLUDE> */
#   define EW_DOCUMENT_NODE_TYPE_FORM           3       /* <FORM> operation */
#   define EW_DOCUMENT_NODE_TYPE_CGI            4       /* CGI (depricated) */
#   define EW_DOCUMENT_NODE_TYPE_IMAGEMAP       5       /* Image Map */
#   define EW_DOCUMENT_NODE_TYPE_COOKIE         6       /* <EMWEB_COOKIE> */
#   define EW_DOCUMENT_NODE_TYPE_STATIC_ETAG    7       /* cachable doc */
#   define EW_DOCUMENT_NODE_TYPE_VARY           8       /* content negotiation*/
#   define EW_DOCUMENT_NODE_TYPE_DYNAMIC_SELECT 9       /* dynamic select */
#   define EW_DOCUMENT_NODE_TYPE_START_RPT_BODY 10      /* <EMWEB_REPEAT> */
#   define EW_DOCUMENT_NODE_TYPE_END_RPT_BODY   11      /* </EMWEB_REPEAT> */
#   define EW_DOCUMENT_NODE_TYPE_NAME_PREFIX    12      /* insert prefix to the name */
#   define EW_DOCUMENT_NODE_TYPE_VALUE_PREFIX   13      /* insert prefix to the value */
#   define EW_DOCUMENT_NODE_TYPE_VARIANT        14      /* Variant hdr node */
#   define EW_DOCUMENT_NODE_TYPE_ATTRIBUTES     15      /* Attributes hdr node */
#   define EW_DOCUMENT_NODE_TYPE_STRING_VALUE   16      /* Macro value param */
#   define EW_DOCUMENT_NODE_TYPE_NS_GET         17      /* Namespace Get */
#   define EW_DOCUMENT_NODE_TYPE_START_ITERATE  18      /* <EMWEB_ITERATE> */
#   define EW_DOCUMENT_NODE_TYPE_END_ITERATE    19      /* </EMWEB_ITERATE> */
#   define EW_DOCUMENT_NODE_TYPE_START_ELEMENT  20      /* Schema element */
#   define EW_DOCUMENT_NODE_TYPE_END_ELEMENT    21      /* Schema element end */
#   define EW_DOCUMENT_NODE_TYPE_FORMAT         22      /* Schema format */
#   define EW_DOCUMENT_NODE_TYPE_SCHEMA_INDEX   23      /* render schema index */
#   define EW_DOCUMENT_NODE_TYPE_CONST_STRING   24      /* output const string data */
#   define EW_DOCUMENT_NODE_TYPE_EWSTR_SCHEMA   25      /* emweb_string schema */
#   define EW_DOCUMENT_NODE_TYPE_IF             26      /* <EMWEB_IF>,<EMWEB_ELSE_IF>,
                                                           <EMWEB_ELSE> */
#   define EW_DOCUMENT_NODE_TYPE_ENDIF          27      /* </EMWEB_IF> */
#   define EW_DOCUMENT_NODE_TYPE_DYN_ETAG       28      /* <EMWEB_ETAG /> */

    uint8       attributes;     /* node attributes */

    /*
     * If you add a node attribute, update the attribute map structures in
     *   ../websrc/compiler/readarchive.c
     *   ../websrc/compiler/ewc_archive.c
     * Note that attribute bits can be reused among different doc node
     * types.  See the above maps to determine which doc nodes use which
     * bits.
     *
     * Note in FORMAT node, attributes is set to the rendering code.
     */
#   define EW_DOCUMENT_NODE_REPEAT          0x01    /* repeat until NULL for
                                                       EMWEB_STRING,
                                                       EMWEB_INCLUDE and
                                                       DYNAMIC_SELECT nodes */
#   define EW_DOCUMENT_NODE_REPEAT_BODY     0x02    /* form contains repeat body*/
#   define EW_DOCUMENT_HDR_SECURE           0x04    /* SECURE flag for cookie */
#   define EW_DOCUMENT_NODE_MACRO           0x08    /* internal to compiler */
#   define EW_DOCUMENT_NODE_SET_VALUE       0x10    /* only save value of
                                                       EMWEB_STRING or NS_GET
                                                       nodes, or URL for
                                                       EMWEB_INCLUDE nodes.
                                                       EMWEB_ITERATE attribute
                                                       (not tag) is not allowed */
#   define EW_DOCUMENT_NODE_CLEAR_VALUE     0x20    /* clear context ->string_value */
#   define EW_DOCUMENT_NODE_FORM_ACTION     0x40    /* EW_STRING NODE || CONST_STRING: */
                                                    /* result/data is Action URL */
#   define EW_DOCUMENT_NODE_STR_NAME_INFO   0x02    /* emweb_string is not a valid XML object
                                                     * NOTE this is reusing the form
                                                     * _repeat_body bit
                                                     */
#   define EW_DOCUMENT_NODE_IF_START        0x02    /* used to differeciate between
                                                     * <EMWEB_IF> & <EMWEB_ELSE_IF>
                                                     */

    uint8       extend[2];      /* use based on node type, else 0 */
                                /* START_ELEMENT: Together w/attributes,
                                   24-bit offset to object name (if
                                   --archive-symbols) */

    uint8       index[4];       /* index assigned to node:

                                   FORM, START_RPT_BODY, DYNAMIC_SELECT:
                                   Form number (MSB-16), element number
                                   (LSB-16).

                                   STRING: Type (MSB-8), switch (LSB-24),
                                   unless STR_NAME_INFO, in which case
                                   index to EWSTR_SCHEMA in schema nodes.

                                   INCLUDE: switch value.

                                   STRING_VALUE: Offset to parameter.

                                   CONST_STRING: Offset to string.

                                   START_ITERATE: emweb_string index
                                   for iterator code.

                                   NS_GET: Namespace index (MSB-8),
                                   schema node index to START_ELEMENT
                                   (LSB-24)

                                   START_ELEMENT: offset to translated
                                   object name (e.g. OID).

                                   FORMAT: offset to format name

                                   EWSTR_SCHEMA: offset to macro name
                                   (if --archive-symbols)

                                   COOKIE: emweb_string index for value

                                   DYN_ETAG: emweb_string index for value

                                   CGI, IMAGEMAP: index to table
                                 */

  } EwsDocumentNode, * EwsDocumentNodeP;

#define SIZEOF_EWS_DOCUMENT_NODE  12    /* never use sizeof */
#define NEXT_DOCNODE( x ) ((EwsDocumentNodeP)(((uint8 *)(x)) \
                                              + SIZEOF_EWS_DOCUMENT_NODE))

#define PREV_DOCNODE( x ) ((EwsDocumentNodeP)(((uint8 *)(x)) \
                                              - SIZEOF_EWS_DOCUMENT_NODE))

     /* macro that uses the attribute/extend fields to store
      * a 24 bit quantity - START_ELEMENT is an example of this.
      */
#define EW_DOCNODE_GET_3BYTE_ATTR( n )  ((((uint32)(n)->attributes)<<16)  \
                                         |(((uint32)(n)->extend[0])<<8)    \
                                         |(n)->extend[1])
#define EW_DOCNODE_PUT_3BYTE_ATTR( n, v )  {  \
      (n)->attributes =  (uint8)(((v)>>16) & 0x00FF);   \
      (n)->extend[0] = (uint8)(((v)>>8) & 0x00FF);      \
      (n)->extend[1] = (uint8)((v) & 0x00FF);}


/*****************************************************************************
 *
 * The EmWeb Compiler creates a document starting with:
 *
 *   "/EmWeb/UPnP/Service/"
 *
 * for each SOAP action defined in an SDD.  This #define sets this prefix,
 * and is included in ew_db.h because both the Compiler and the Server UPnP
 * code use this as the prefix for SOAP action documents.
 *
 *****************************************************************************/

/* Prefix for all SOAP action pseudo-form documents. */
#ifndef EWU_SOAP_ACTION_URL_PREFIX
#define EWU_SOAP_ACTION_URL_PREFIX "/EmWeb/UPnP/Service/"
#endif


/* END-SOURCE-ONLY */

#endif /* _EW_DB_H */
