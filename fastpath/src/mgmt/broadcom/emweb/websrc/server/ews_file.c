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
 * EmWeb/Server file system internals
 *
 */

#include "ews.h"
#include "ews_sys.h"

/*
 * ewsFileHashInit
 * Initialize hash table - fill all entries with NULL pointers
 */
void
ewsFileHashInit ( void )
{
  EwsDocument *p;

    for (p = &ews_state->file_hash_table[0];
         p < &ews_state->file_hash_table [ EWS_FILE_HASH_SIZE ];
         *p++ = (EwsDocument) NULL)
      ;
}

#ifdef EW_CONFIG_OPTION_CLEANUP
/*
 * ewsFileHashShutdown
 * Release hash table resources
 */
void
ewsFileHashShutdown ( void )
{
  uintf i;
  EwsDocument p, tmp;

    for (i = 0; i < EWS_FILE_HASH_SIZE; i++)
      {
        p = ews_state->file_hash_table[i];
        while (p != EWS_DOCUMENT_NULL)
          {
            tmp = p;
            p = p->next_hash;

#           ifdef EW_CONFIG_OPTION_DEMAND_LOADING
            while (&tmp->fault_list != tmp->fault_list.next)
              {
                ewsInternalAbort((EwsContext)tmp->fault_list.next);
              }
#           endif /* EW_CONFIG_OPTION_DEMAND_LOADING */

            if ((tmp->flags & (EWS_FILE_FLAG_CLONE | EWS_FILE_FLAG_REGISTERED))
                != 0)
              {
                ewaFree( (void *)tmp->url);
              }
            ewaFree(tmp);
          }
      }
}
#endif /* EW_CONFIG_OPTION_CLEANUP */

/*
 * ewsFileHash
 * Return the hash table index corresponding to the provided URL string.
 * For now, the hash algorithm simply adds the individual characters in the
 * string modulo the hash table size.
 *
 * url  - string containing URL "/..."
 *
 * Returns the hash table index
 */
uintf
ewsFileHash ( const char * url )
{
  uint32 hash = 0;

    while (*url != '\0')
      {
        hash += *url++;
      }

    return hash % EWS_FILE_HASH_SIZE;
}

/*
 * EwsFileHashInsert
 * Create and insert a new entry into the hash table bucket
 *
 * index - index of hash table in which to insert the entry
 *
 * Returns pointer to new document element, or NULL if resources were not
 * available.
 */
EwsDocument
ewsFileHashInsert ( uintf indx )
{
  EwsDocument   document;
  EwsDocument   *hashp;

    EMWEB_TRACE(("ewsFileHashInsert(%u)\n", indx));

    document = (EwsDocument) ewaAlloc(sizeof(EwsDocument_t));
    if (document != (EwsDocument) NULL)
      {
        hashp = &ews_state->file_hash_table[indx];
        if (*hashp != NULL)
          {
            (*hashp)->prev_hash = document;
          }
        document->next_hash = *hashp;
        document->prev_hash = (EwsDocument) NULL;
        document->hash = indx;
        document->url = (char *) NULL;
        *hashp = document;
      }
    return document;
}

/*
 * ewsFileHashDelete
 * Remove a document entry from the hash table.
 *
 * index        - hash table index corresponding to document entry
 * document     - pointer to document entry to be deleted
 *
 * No return value
 * NOTE: caller MUST hold the EWA_TAKS_LOCK across this call
 */
void
ewsFileHashDelete ( EwsDocument document )
{
  uintf indx = document->hash;

    EMWEB_TRACE(("ewsFileHashDelete(%u, %p)\n", indx, document));


    if (document->next_hash != (EwsDocument) NULL)
      {
        document->next_hash->prev_hash = document->prev_hash;
      }
    if (document->prev_hash != (EwsDocument) NULL)
      {
        document->prev_hash->next_hash = document->next_hash;
      }
    else
      {
        ews_state->file_hash_table[indx] = document->next_hash;
      }

    ewaFree(document);
}

/*
 * ewsFileHashLookup
 * Scan entries in a hash table bucket looking for a matching URL
 *
 * index        - index in hash table corresponding to hashing bucket
 * url          - string containing URL without leading /
 *
 * Returns pointer to document entry, or NULL if no matching URL was found.
 */
EwsDocument
ewsFileHashLookup ( uintf indx, const char * url )
{
  EwsDocument document;
  boolean bvalue;

    EMWEB_TRACE(("ewsFileHashLookup(%u, %s)\n", indx, url));

    for (document = ews_state->file_hash_table[indx];
         document != NULL;
         document = document->next_hash)
      {
        if (document->url != NULL)
          {
            EMWEB_STRCMP(bvalue, url, document->url);
            if (!bvalue)
              {
                break;
              }
          }
      }

    return document;
}

#ifdef EW_CONFIG_FILE_METHODS
/*
 * ewsFileSetupFilesystemDoc
 * This function sets up a dummy document and doc_header structure
 * to use with a document that is part of an external filesystem (rather than
 * part of the emweb archive).  The document structures are allocated here
 * and initialized.  It is up to the abort or ewsDocumentServe to deallocate
 * this memory when finished with the document.
 *
 * context - request context that will access the local filesystem
 * url     - url of local filesystem document
 *
 * Returns a dummy document structure & doc_header structure.
 */
EwsDocument
ewsFileSetupFilesystemDoc( EwsContext context, const char *url )
{
  EwsDocument document;
  uintf len;
  static const EwsDocumentHeader doc_header =   /* shared by all files */
    {
      {0,0,0,0},                        /* next_offset */
      {0,0,0,0},                        /* url_offset */
      EW_ARCHIVE_DOC_TYPE_FILE,         /* document_type */
      0,                                /* document_flags */
      { 0,0 },
      {0,0,0,0 },
      { 0,0,0,0 },
      { 0,0,0,0 },
      { 0,0,0,0 },
      { 0,0,0,0 },
      { 0,0,0,0 },
      0,
      0,
      { 0,0 }
    };

  /*
   * Mark context for application cleanup since we are about to invoke
   * the external filesystem APIs.  Note that we do this here because
   * this function is called from various EmWeb modules (e.g. Client)
   * which may not go through normal URL hook processing.
   */
  context->flags |= EWS_CONTEXT_FLAGS_CLEANUP;

  EMWEB_STRLEN( len, url );
  if ((document = (EwsDocument) ewaAlloc( sizeof( EwsDocument_t )
                           + len + 1
                          )
      ) == NULL)
    {
      return NULL;
    }

  document->flags = EWS_FILE_FLAG_FILESYSTEM | EWS_FILE_FLAG_LOADED;
  document->archive = NULL;
  document->archive_data = NULL;
  document->doc_header = &doc_header;
  document->url = (char *) (document + 1);
  EMWEB_STRCPY( document->url, url );

# ifdef EW_CONFIG_OPTION_CLONING
  document->clone_count = 0;
# endif /* EW_CONFIG_OPTION_CLONING */

# ifdef EW_CONFIG_OPTION_AUTH
  document->realmp = NULL;
  if (context->fileParams->fileInfo.realm != NULL
      && *context->fileParams->fileInfo.realm != '\0')
    {
      document->realmp = ewsAuthRealmGet(context->fileParams->fileInfo.realm);
      if (document->realmp == NULL)
        {
          ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
          EMWEB_WARN(("ewsServeFindDocument: no resources for realm\n"));
          ewsNetHTTPAbort(context);
          return NULL;
        }
    }
# endif /* EW_CONFIG_OPTION_AUTH */

# ifdef EMWEB_SANITY
  if (context->dummy != NULL)
    {
      EMWEB_WARN(("ewsFileSetupFilesystemDoc: unclaimed dummy document context\n"));
      return NULL;
    }
# endif /* EMWEB_SANITY */

  document->fileParams = context->fileParams;   /* move fileparams to document */
  context->fileParams = NULL;

  /* store document here temporarily until doc context is created,
   * then move it there.  We do this so memory won't be lost if
   * context is aborted request completes
   */

  context->dummy = document;

  return document;
}

/*
 * The following are the support functions for multiple
 * file system functions.
 *
 */
#ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST

/*
 * ewsFileSystemRegister
 *   Adds a set of file system functions to the file_system_list.
 *
 * fileSystemP - pointer to the set to be added to the list
 *
 * return  - EwsFileSystemHandle
 *
 */
EwsFileSystemHandle  ewsFileSystemRegister( EwsFileSystemP fileSystemP )
{
  EwsFileSystemHandle newHandle;

  newHandle = (EwsFileSystemHandle)ewaAlloc(sizeof(EwsFileSystemEntry));
  if ( NULL != newHandle )
    {
      newHandle->fs = *fileSystemP;
      newHandle->use_cnt = 0;
      EWS_LINK_INSERT(&(ews_state->file_system_list), &(newHandle->link));
    }
  else
    {
      newHandle = NULL;
    }

  return newHandle;
}

/*
 * ewsFileSystemUnregister
 *   Removes a set of file system functions from the File System List.
 *
 * handle - entry to be removed from the list
 *
 * return  - EWS_STATUS_OK
 *           EWS_STATUS_NOT_FOUND
 *
 */
EwsStatus ewsFileSystemUnregister( EwsFileSystemHandle handle )
{
  EwsStatus result;
  EwsFileSystemHandle fshandle;

  if ((handle == NULL) ||
      (EWS_LINK_IS_EMPTY(&(ews_state->file_system_list))))
    {
      /* just to cover ourselves */
      EMWEB_ERROR(("ewsFileSystemUnregister: file system not registered\n"));
      result =  EWS_STATUS_NOT_FOUND;
    }
  else
    {
      result = EWS_STATUS_NOT_FOUND;
      fshandle = (EwsFileSystemHandle)EWS_LINK_HEAD(ews_state->file_system_list);
      do
        {
          if (fshandle == handle)
            {
              result = EWS_STATUS_OK;
              break;
            }
          fshandle = (EwsFileSystemHandle)(EWS_LINK_NEXT(fshandle->link));
        } while (fshandle != (EwsFileSystemHandle)EWS_LINK_HEAD(ews_state->file_system_list));
      if (result == EWS_STATUS_NOT_FOUND)
        {
              EMWEB_ERROR(("ewsFileSystemUnregister: file system not registered\n"));
        }
      else
        {
          if (handle->use_cnt != 0)
            {
              EMWEB_ERROR(("ewsFileSystemUnregister: attempt to unregister file system in use.\n"));
              result = EWS_STATUS_IN_USE;
            }
          else
            {
              EWS_LINK_DELETE(&(handle->link));  /* delete set from the list */
              ewaFree(handle);
              result = EWS_STATUS_OK;
            }
        }
    }
  return(result);
}
#endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
#endif /* EW_CONFIG_FILE_METHODS */
#ifdef EW_CONFIG_OPTION_URL_GETNEXT
/*
 * ewsGetNextURL
 * Given a URL in the hash table, get the next URL.  Note that these are
 * in no particular order, but provides a means of scanning the list of
 * URLs by the application.
 *
 *   url - NULL to read first URL, else URL from which to find next URL
 *
 * Returns NULL if end of list, or next URL
 */
const char *
ewsGetNextURL ( const char * url )
{
  uintf hash = 0;
  EwsDocument doc = NULL;

    /*
     * If URL specified, find next one in hash table
     */
    if (url != NULL)
      {
        hash = ewsFileHash(url);
        doc = ewsFileHashLookup(hash, url);
        if (doc != NULL)
          {
            doc = doc->next_hash;
          }
      }

    /*
     * If end of hash bucket, find next non-empty hash bucket.
     */
    while (doc == NULL || doc->url == NULL)
      {
        hash++;
          if (hash == EWS_FILE_HASH_SIZE)
          {
            break;
          }
        doc = ews_state->file_hash_table[hash];
      }
    if (doc != NULL)
      {
        return doc->url;
      }
    return NULL;
}
#endif /* EW_CONFIG_OPTION_URL_GETNEXT */
