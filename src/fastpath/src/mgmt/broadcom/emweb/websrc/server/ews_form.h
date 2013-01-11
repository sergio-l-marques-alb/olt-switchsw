/*
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
 * EmWeb/Server form processing
 *
 */

#ifndef EWS_FORM_H
#define EWS_FORM_H

#include "ew_form.h"
#include "ews.h"

#ifdef EW_CONFIG_OPTION_FORM

#ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
/*
 * the following structure is necessary to support RFC1867's method for
 * uploading files to the server.  By defining the INPUT TYPE=FILE field,
 * a browser can send the contents of a file to the server.  This
 * structure manages that process, and is allocated and hung off the
 * context block whenever a submission including a TYPE=FILE field is
 * encountered.
 */
typedef struct EwsFormInputFileState_s
{
  EwsString     *formDataHeaders;   /* headers from form-data chunk */
  char          *formDataBoundary;  /* the multipart boundary string */
  EwaFileHandle formFile;           /* file descriptor for post file */
# ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
  EwsFileSystemHandle  fileSystem;
# endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
  boolean       formDataEnd;        /* true if last segment in post */
  boolean       fieldIsFile;        /* TRUE if current field is file */
  uint8         *fileStatusp;       /* ptr to file form field's status */
  boolean       boundaryMatch;      /* TRUE = search matched boundary */
# ifdef EW_CONFIG_OPTION_CHUNKED_IN
  EwsString     leftOverChunk;      /* end of parsed data in last chunk */
  uint32        leftOverData;       /* #bytes unparsed in last chunk */
# endif /* EW_CONFIG_OPTION_CHUNKED_IN */
} EwsFormInputFileState;

/*
 * the following is a structure to associate all fileHandles contained in the
 * form structure with fileSystemHandles (for ...FileClose() via free_f()
 * via ...FormFree() purpose).
 */
# ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
typedef struct EwsInputFileSystemEntry_s
{
  EwsLink link;
  EwaFileHandle fileHandle;
  EwsFileSystemHandle fileSystem;
} EwsInputFileSystemEntry_t, *EwsInputFileSystemEntry;
# endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
#endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */


/*
 * ewsFormServe
 * Serve document form node.  This function determines from the node index the
 * appropriate form serving action and writes values to the data stream.
 *
 * context      - context of request
 * doc_context  - context of current document
 * node_index   - document node index
 *
 * No return value
 */
extern void ewsFormServe
  (EwsContext context, EwsDocumentContextP doc_context, uint32 node_index );

/*
 * ewsFormSubmitStart
 * Start form submission.  This function allocates and initializes a form
 * structure and attaches it to the context.
 *
 * context      - context of request
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
extern EwsStatus ewsFormSubmitStart ( EwsContext context );

/*
 * ewsFormSubmitSet
 * Given a name/value pair from x-www-urlencoded data, update the form
 * structure appropriately.
 *
 * context      - context of request
 * name         - name in name/value pair
 * value        - value in name/value pair
 *
 * No return value
 */
extern EwsStatus ewsFormSubmitSet ( EwsContext context, EwsStringP name, EwsStringP value );

/*
 * ewsFormSubmitAction
 * Pass the form to the application and process
 *
 * context      - context of request
 *
 * Returns NULL for default action, or URL string for redirect.
 */
extern EW_NSC char * ewsFormSubmitAction ( EwsContext context );

/*
 * ewsFormFree
 * Release form resources
 *
 * context      - context of request
 *
 * No return value
 */
extern void ewsFormFree ( EwsContext context );

/*
 * ewsFormWriteHex
 * Write hex into network buffers with seperator character between bytes,
 * used by hex string and MAC address "To" functions.
 *
 * context      - context for request
 * src          - source of raw data to be converted to hexadecimal
 * length       - number of bytes of data
 * sep          - seperator character, or '\0' for none.
 *
 * No return value
 */
extern void ewsFormWriteHex
  ( EwsContext context, const uint8 *src, uint32 length, char sep );

/*
 * ewsFormReadHex
 * Read hexadecimal characters from network buffers into raw data buffer.
 *
 * dst          - raw data buffer to receive translated data
 * src          - estring representing hexadecimal digits
 * length       - size of raw data buffer
 * sepp         - output: character seperating bytes in input stream
 * sep_pad      - TRUE if separator forces byte alignment
 *
 * Returns number of raw data bytes processed, or -1 on parse error
 */
extern int32 ewsFormReadHex
  ( uint8 *dst, EwsStringP src, uint32 length, char *sepp, boolean sep_pad );

#endif /* EW_CONFIG_OPTION_FORM */

#if defined (EW_CONFIG_SCHEMA_ARCHIVE) || defined (EW_CONFIG_OPTION_FORM)
/*
 * Convert between EmWeb internal field types and external API rendering codes
 */
extern int ewsField2Rendering ( EwFieldType field_type );
extern int ewsRendering2Field ( EwsSchemaRenderingCode rendering_code );
#endif

#endif /* _EWS_FORM_H */










