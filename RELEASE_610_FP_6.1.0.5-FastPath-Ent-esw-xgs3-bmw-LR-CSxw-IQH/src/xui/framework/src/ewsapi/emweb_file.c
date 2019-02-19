
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename emweb_file.c
*
* @purpose
*
* @component EMWEB
*
* @comments
*
* @create 04/17/2007
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include <stdarg.h>

#include "ews_api.h"
#include "ews.h"
#include "emweb_if.h"
#include "ews_sys.h"
#ifdef L7_WIRELESS_PACKAGE
#include "wnv_api.h"
#endif
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
#include "usmdb_cpdm_api.h"
#endif
#ifdef _L7_OS_VXWORKS_
#include "usmdb_sim_api.h"
#define BUFFER_FOR_UNKNOWN_SIZE_FILES 16*1024*1024

/* Global variables for HTTP download */
extern L7_char8 *L7_http_dl_ram_buffer;
extern L7_uint32 L7_http_dl_data_last_seen;
extern L7_RC_t usmDbHttpTransferFileCleanup(L7_uint32 unit);
#endif
#ifdef _L7_OS_ECOS_
#include "usmdb_sim_api.h"
#include <unistd.h>
#define BUFFER_FOR_UNKNOWN_SIZE_FILES 16*1024*1024
/* Global variables for HTTP download */
extern L7_char8 *L7_http_dl_ram_buffer;
extern L7_uint32 L7_http_dl_data_last_seen;
extern L7_RC_t usmDbHttpTransferFileCleanup(L7_uint32 unit);
#define ERROR -1
#endif
#define IMAGE_FILE_MAX_LENGTH 512*1024

extern void emweb_trace (const char *fmt, ...);
#if 1
#define EMWEB_FILE_TRACE(fmt,arg...) printf(fmt,##arg)
#else
#define EMWEB_FILE_TRACE(fmt,arg...)
#endif

#define EMWEB_FILE_WARN EMWEB_FILE_TRACE

#define EMWEB_FILE_LOC() printf("%s-%d\r\n", __FUNCTION__, __LINE__)

#ifdef _L7_OS_LINUX_
extern int osapiWebFileHandler (int op, char *filename, void **handle, char *data, int length);
typedef int (*emwebFileHandler_t) (int op, char *filename, void **handle, char *data, int length);

static emwebFileHandler_t ewaFileHandlerGet (EwsContext context)
{
  return (emwebFileHandler_t) context->fileHandler;
}
#endif
void emwebFileHandlerSet (EwsContext context, void *handler)
{
  context->fileHandler = (void *) handler;
}

char *emwebFileNameGet (void *arg)
{
  EwaFileHandle fp = (EwaFileHandle) arg;
  return fp->localPath;
}

void ewaFileClean (EwsContext context)
{
  EMWEB_FILE_LOC ();
}

static void ewaFileDownloadSetProgress(EwsContext context);
EwaFileHandle ewaFilePost (EwsContext context, const EwsFileParams * params)
{
#ifdef _L7_OS_LINUX_
  int len;
  char *filename;
  EwaFileHandle fp;
  emwebFileHandler_t appFunc = ewaFileHandlerGet (context);
  
  /* check if application support is present */
  if (appFunc == NULL)
  {
    /*once the transfer is complete, emweb is making 
      context fileHandler null.so if for the same context if we do one more 
      transfer,it will fail.
    */
    emwebFileHandlerSet(context,osapiWebFileHandler);   
    appFunc = ewaFileHandlerGet (context);
    if(appFunc == NULL)
    {
     EMWEB_FILE_WARN (("ewaFilePost: unable to register file handler\r\n"));
      return (EWA_FILE_HANDLE_NULL);
    } 
  }

  /* check for file name validity */
  if (params->fileField.fileName == NULL)
  {
    /* EMWEB_FILE_WARN (("ewaFilePost: Invalid file name\r\n")); */
    return (EWA_FILE_HANDLE_NULL);
  }

  /* Strip out filename if in WinOs name format */
  if (strrchr (params->fileField.fileName, '\\') != NULL)
  {
    filename = strrchr (params->fileField.fileName, '\\');
    filename++;                 /* skip slash */
  }
  else
  {
    filename = (char *) params->fileField.fileName;
  }

  /* allocate emweb file handle */
  len = strlen (filename) + 1;
  fp = (EwaFileHandle_t *) ewaAlloc (sizeof (EwaFileHandle_t) + len);
  if (fp == EWA_FILE_HANDLE_NULL)
  {
    EMWEB_FILE_WARN (("ewaFilePost: ewaAlloc failed\r\n"));
    return EWA_FILE_HANDLE_NULL;
  }

  /* store the file name */
  fp->localPath = (char *) (fp + 1);
  fp->appFunc = appFunc;
  strncpy (fp->localPath, filename, len);

  /* issue application request to create new file */
  if (0 > appFunc ('n', filename, &fp->appWap, NULL, 0))
  {
    EMWEB_FILE_WARN (("ewaFilePost: unable to create file\n"));
    ewaFree (fp);
    return EWA_FILE_HANDLE_NULL;
  }
  ewaFileDownloadSetProgress(context);
  return fp;
#elif (_L7_OS_VXWORKS_ || _L7_OS_ECOS_)
 L7_BOOL fIsNVImageFile = L7_FALSE;
  L7_BOOL fIsCPImageFile = L7_FALSE;
  EwaFileHandle fp;
  int PMAX = 80;
  char path[PMAX];
  char *filename;
  L7_uint32 unit = 0;
 
#if defined (L7_WIRELESS_PACKAGE) || defined (L7_CAPTIVE_PORTAL_PACKAGE)
  char *GIF = ".GIF";
  char *JPG = ".JPG";
  char *gif = ".gif";
  char *jpg = ".jpg";
#endif

  EMWEB_TRACE(("Post url '%s'\n", context->url));
 
  if (strstr(context->url, "http_file_download.html") == NULL)
  {
    /* Perform some simple validation, if not a file download - client 
       filename is not used for HTTP file download */
    if ((params->fileField.fileName == NULL) ||
        (strstr(params->fileField.fileName,".") == NULL) ||
        (strlen(params->fileField.fileName)<4))
    {
      /* EMWEB_WARN( ("ewaFilePost: Invalid file name\n") );*/
      return (EWA_FILE_HANDLE_NULL);
    }
  }
  filename = ewaAlloc(sizeof(params->fileField.fileName)+1);
 
  /* Strip out filename if in WinOs name format */
  if (strrchr(params->fileField.fileName,'\\') != NULL)
  {
    filename = strrchr(params->fileField.fileName,'\\');
    filename++; /* skip slash */
  }
  else
  {
    filename = (char *)params->fileField.fileName;
  }
  EMWEB_TRACE(("Post file '%s'\n", filename));
 
/* if file length is known, and is too big, then fail */
# ifdef FILE_MAX_LENGTH
  if (   params->fileField.contentLength != EWS_CONTENT_LENGTH_UNKNOWN
      && params->fileField.contentLength > FILE_MAX_LENGTH
     )
  {
    EMWEB_WARN(("ewaFilePost: file too long\n"));
    return (EWA_FILE_HANDLE_NULL);
}
#endif /* FILE_MAX_LENGTH */
 
  #ifdef L7_WIRELESS_PACKAGE
  /* Define the target path by parsing on the URL */
  if (strstr(context->url, "nv_image_mgmt.html") != NULL)
  {
    strncpy(path,RAM_NV_PATH,PMAX);
    if (strlen(filename)>L7_NV_MAX_FILE_NAME)
    {
      /* EMWEB_WARN(("ewaFilePost: invalid file name, too big.\n")); */
      return (EWA_FILE_HANDLE_NULL);
    }
 
    if ((strstr(filename,GIF)==NULL) &&
        (strstr(filename,JPG)==NULL) &&
        (strstr(filename,gif)==NULL) &&
        (strstr(filename,jpg)==NULL))
    {
      /* EMWEB_WARN(("ewaFilePost: invalid file type.\n")); */
      return (EWA_FILE_HANDLE_NULL);
    }
 
    fIsNVImageFile = L7_TRUE;
    strcat(path,RAM_NV_NAME);
  }
  else
  {
    /* could assign a different path based on url */
  }
#endif /* L7_WIRELESS_PACKAGE */
 
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  /* Define the target path by parsing on the URL */
  if (strstr(context->url, "cp_web_custom.html") != NULL)
  {
    strncpy(path,RAM_CP_PATH,PMAX);
    if (strlen(filename)>CP_FILE_NAME_MAX)
    {
      /* EMWEB_WARN(("ewaFilePost: invalid file name, too big.\n")); */
      return (EWA_FILE_HANDLE_NULL);
    }
  if (   params->fileField.contentLength != EWS_CONTENT_LENGTH_UNKNOWN
      && params->fileField.contentLength > IMAGE_FILE_MAX_LENGTH
     )
  {
    /* EMWEB_WARN(("ewaFilePost: file too long\n")); */
    return (EWA_FILE_HANDLE_NULL);
  }
 
    if ((strstr(filename,GIF)==NULL) &&
        (strstr(filename,JPG)==NULL) &&
        (strstr(filename,gif)==NULL) &&
        (strstr(filename,jpg)==NULL))
    {
      /* EMWEB_WARN(("ewaFilePost: invalid file type.\n")); */
      return (EWA_FILE_HANDLE_NULL);
    }
 
    fIsCPImageFile = L7_TRUE;
strcat(path,RAM_CP_NAME);
  }
  else
  {
    /* could assign a different path based on url */
  }
#endif /* L7_WIRELESS_PACKAGE */
 
  if (strstr(context->url, "http_file_download.html") != NULL)
    {
      L7_uint32 buf_len;
 
      L7_http_dl_data_last_seen = osapiUpTimeRaw();
      if ( usmDbHttpTransferFileCleanup( unit) != L7_SUCCESS)
      {
        return(EWA_FILE_HANDLE_NULL);
      }
      if (params->fileField.contentLength == EWS_CONTENT_LENGTH_UNKNOWN)
      {
        if (context->content_length > 15)
        {
          buf_len = context->content_length;
        }
        else
        {
          buf_len = BUFFER_FOR_UNKNOWN_SIZE_FILES;
        }
      }
      else
      {
        buf_len = params->fileField.contentLength;
      }
      /* Add 4 for actual length of file at beginning */
      L7_http_dl_ram_buffer = osapiMalloc(L7_CLI_WEB_COMPONENT_ID, buf_len+4);
      if (L7_http_dl_ram_buffer == NULL)
      {
        return(EWA_FILE_HANDLE_NULL);
      }
#ifdef __64_BIT__
      sprintf(filename, "0x%016llX", (L7_uint64)L7_http_dl_ram_buffer);
#else
      sprintf(filename, "0x%016X", (L7_uint32)L7_http_dl_ram_buffer);
#endif
      if (usmDbTransferFileNameLocalSet(unit, filename) != L7_SUCCESS)
      {
        osapiFree(L7_CLI_WEB_COMPONENT_ID, L7_http_dl_ram_buffer);
        L7_http_dl_ram_buffer = NULL;
        return(EWA_FILE_HANDLE_NULL);
      }
      if ((fp = (EwaFileHandle)ewaAlloc(sizeof(EwaFileHandle_t))) !=
          EWA_FILE_HANDLE_NULL)
      {
        fp->fd = -1;
        fp->buf_size = buf_len;
        fp->buf_index = 4;
      }
      else
      {
        usmDbTransferFileNameLocalSet(unit, "");
        osapiFree(L7_CLI_WEB_COMPONENT_ID, L7_http_dl_ram_buffer);
        L7_http_dl_ram_buffer = NULL;
      }

     /* set global http transfer status and in context */
	 ewaFileDownloadSetProgress(context);

      return fp;
    }
 
  if ((fp = (EwaFileHandle)ewaAlloc(  sizeof( EwaFileHandle_t )
                                      + strlen(path)
                                      + 1 /* path delimiter */
                                      + strlen(filename)
                                      + 1 /* EOL */))
       != EWA_FILE_HANDLE_NULL)
  {
    /* construct the path to the local filesystem and open it. */
    fp->localPath = (char *)(fp + 1);
    strcpy(fp->localPath,path);
    strcat(fp->localPath,filename);
    fp->buf_size = 0;
  }
 
#ifdef FILE_USE_OPEN_TO_CREATE
  fp->fd = open( fp->localPath, (O_CREAT | O_WRONLY), 0x644 );
#else
  fp->fd = creat( fp->localPath, O_WRONLY );
#endif /* FILE_USE_OPEN_TO_CREATE */
 
  if (fp->fd == ERROR)          /* if error on open, fail */
  {
    /* EMWEB_WARN( ("ewaFilePost: unable to create file\n") ); */
    ewaFree( fp );
    fp = EWA_FILE_HANDLE_NULL;
  }
  else
  {
    if (fIsNVImageFile == TRUE)
    {
#ifdef L7_WIRELESS_PACKAGE
      wnvSetImageCfgChanged();
#endif /* L7_WIRELESS_PACKAGE */
    }
    if (fIsCPImageFile == TRUE)
    {
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
      usmDbCpdmSetImageFileChanged();
#endif /* L7_CAPTIVE_PORTAL_PACKAGE */
    }
  }
  return fp;
#endif
}

sintf ewaFileWrite (EwsContext context, EwaFileHandle handle, const uint8 * datap, uintf length)
{
#ifdef  _L7_OS_LINUX_
  emwebFileHandler_t appFunc = ewaFileHandlerGet (context);

  if (handle == EWA_FILE_HANDLE_NULL)
  {
    EMWEB_FILE_WARN (("ewaFileWrite: null handle.\r\n"));
    return -1;
  }

  /* issue application request to write to file */
  if (0 > appFunc ('w', handle->localPath, &handle->appWap, (char *) datap, length))
  {
    EMWEB_FILE_WARN (("ewaFileWrite: Error writing to file.\r\n"));
    return -1;
  }

  return length;
#elif (_L7_OS_VXWORKS_ || _L7_OS_ECOS_)
int   written;
 
  if (handle->buf_size)
  { /* Not using a file, but a RAM buffer */
    L7_uint32 *file_length;
 
    L7_http_dl_data_last_seen = osapiUpTimeRaw();
    if ((length + handle->buf_index) > (handle->buf_size + 4))
    {
      return -1;
    }
    memcpy(&(L7_http_dl_ram_buffer[handle->buf_index]), datap, length);
    handle->buf_index += length;
    file_length = (L7_uint32 *)(L7_http_dl_ram_buffer);
    *file_length = handle->buf_index - 4;
    return length;
  }
 
  if (   ((written = write( handle->fd
                          ,(char *)datap
                          ,length))
          != length)
      || written == ERROR
     )
    {
      /* EMWEB_WARN(("ewaFileWrite: Error writing to file.\n")); */
      return -1;
    }
  return written;
#endif
}

sintf ewaFileRead (EwsContext context, EwaFileHandle handle, uint8 * datap, uintf length)
{
#ifdef _L7_OS_LINUX_ 
  int retval;
  emwebFileHandler_t appFunc = ewaFileHandlerGet (context);

  if (handle == EWA_FILE_HANDLE_NULL || appFunc == NULL)
  {
    EMWEB_FILE_WARN (("ewaFileRead: null handle.\r\n"));
    return -1;
  }

  /* issue application request read file */
  retval = appFunc ('r', handle->localPath, &handle->appWap, datap, length);
  if (0 > retval)
  {
    EMWEB_FILE_WARN (("ewaFileRead: Error reading file.\r\n"));
    return -1;
  }
  return retval;
#elif _L7_OS_VXWORKS_
 int bytes;
 
  if ((bytes = read( handle->fd, (char *)datap, length ))
      == ERROR)
    {
      /* EMWEB_WARN(("ewaFileRead: cannot read file.\n")); */
      return -1;
    }
  return bytes;
#elif _L7_OS_ECOS_
 int len; /* available buffer length */
  int buf_size;
  int buf_index;

   buf_size = handle->buf_size;
   buf_index = handle->buf_index;
   if(handle->buf_size)
   {
      /* Not using file but using a RAM buffer */
      if((handle->buf_index) >= (handle->buf_size))
      {
        EMWEB_FILE_TRACE ("ewaFileRead end\n");
        osapiFree(L7_OSAPI_COMPONENT_ID,L7_http_dl_ram_buffer);
        return 0;
      }
      len = buf_size - buf_index;
      if(len > length)
      {
         len = length;
         memcpy(datap, &(L7_http_dl_ram_buffer[handle->buf_index]),len);
         handle->buf_index += len;
      }
      else
      {
         memcpy(datap, &(L7_http_dl_ram_buffer[handle->buf_index]),len);
         handle->buf_index += len; 
      }
      return len;
   }
   return -1;
#endif
}

EwaFileStatus ewaFileClose (EwaFileHandle handle, EwsStatus status)
{
#ifdef _L7_OS_LINUX_ 
  emwebFileHandler_t appFunc = NULL;

  if (handle == EWA_FILE_HANDLE_NULL || handle->appFunc == NULL)
  {
    EMWEB_FILE_WARN (("ewaFileClose: null handle.\r\n"));
    return EWA_FILE_STATUS_ERROR;
  }

  /* issue application request close file */
  appFunc = (emwebFileHandler_t) handle->appFunc;
  appFunc ('c', handle->localPath, &handle->appWap, NULL, 0);
  
  return EWA_FILE_STATUS_OK;
#elif (_L7_OS_VXWORKS_ || _L7_OS_ECOS_)
if (handle->buf_size)
  { /* Not using a file, but a RAM buffer */
    /* Set final length of data in first 4 bytes */
    L7_uint32 *datalen = (L7_uint32 *)(L7_http_dl_ram_buffer);
      *datalen = handle->buf_index - 4;
    return EWA_FILE_STATUS_OK;
  }
  close( handle->fd );
 
  /*
   * remove the file if a failure occurred while creating/writing
   * a new file
   */
  if (handle->op == ewaFileDeleteOnError && status != EWS_STATUS_OK)
  {
#ifdef _L7_OS_ECOS_
    unlink(handle->localPath);
#else
    remove( handle->localPath );
#endif
  }
 
  ewaFree( handle );
  return EWA_FILE_STATUS_OK;
#endif
}

EwaFileHandle ewaFileGet (EwsContext context, const char *url, const EwsFileParams * params)
{
#ifdef _L7_OS_LINUX_ 
  EwaFileHandle efa = &ewsContextNetHandle (context)->file_handle;
  emwebFileHandler_t appFunc = ewaFileHandlerGet (context);
  EW_UNUSED (url);

  efa->context = context;
  efa->type = fileHandleStream;
  efa->appFunc = appFunc;
  efa->localPath = (char *) params->fileInfo.fileName;

  if (appFunc == NULL)
  {
    return EWA_FILE_HANDLE_NULL;
  }

  /* issue application request read file */
  if (0 > appFunc ('o', (char *) params->fileInfo.fileName, &efa->appWap, NULL, 0))
  {
    EMWEB_FILE_WARN (("ewaFileGet: unable to open file\n"));
    return EWA_FILE_HANDLE_NULL;
  }

  return efa;
#elif  _L7_OS_ECOS_
EwaFileHandle fp;
 
  /*
   * We know that the filename corresponds to a regular file (not a directory
   * or device) see ewaURLHook.  Make sure the filename doesn't try to change
   * directories
   */
  if (   strstr( params->fileInfo.fileName, "/../" ) != NULL
      || strstr( params->fileInfo.fileName, "/~" ) != NULL
     )
    {
      /* EMWEB_WARN(("ewaFileGet: invalid path.\n")); */
      return EWA_FILE_HANDLE_NULL;
    }
 
  /*
   * Create a file handle
   */
  if ((fp = (EwaFileHandle)ewaAlloc(  sizeof( EwaFileHandle_t )))
      != EWA_FILE_HANDLE_NULL)
    {
 
       if(L7_http_dl_ram_buffer == NULL)
       {
          return(EWA_FILE_HANDLE_NULL);
       }
       L7_uint32 *buf_len = (L7_uint32 *)(L7_http_dl_ram_buffer); 
      /*
       * set the file handle's path pointer into the params structure
       */
      fp->fd = -1;
      /* 
       * L7_http_dl_ram_buffer has additional first four bytes which actually specifies length 
       */
      fp->buf_size = *buf_len+4;
      fp->buf_index = 4;
      fp->localPath = (char *)params->fileInfo.fileName;
      fp->op = ewaFileNeverDelete;  /* in case write fails */
 
    }
  return fp;
#elif _L7_OS_VXWORKS_
EwaFileHandle fp;

  /*
   * We know that the filename corresponds to a regular file (not a directory
   * or device) see ewaURLHook.  Make sure the filename doesn't try to change
   * directories
   */
  if (   strstr( params->fileInfo.fileName, "/../" ) != NULL
      || strstr( params->fileInfo.fileName, "/~" ) != NULL
     )
    {
      /* EMWEB_WARN(("ewaFileGet: invalid path.\n")); */
      return EWA_FILE_HANDLE_NULL;
    }

  /*
   * Create a file handle
   */
  if ((fp = (EwaFileHandle)ewaAlloc(  sizeof( EwaFileHandle_t )))
      != EWA_FILE_HANDLE_NULL)
    {
        char lbuf[256];
        memset(lbuf,0x0,sizeof(lbuf));
        strcpy(lbuf, (char *)params->fileInfo.fileName);
        if(osapiStrncmp(lbuf,"/filesystem/image2.stk",22) == 0 )
        {
           memset(lbuf,0x0,sizeof(lbuf));
           osapiStrncpy(lbuf,"/filesystem/image2",18);
        }

        if(osapiStrncmp(lbuf,"/filesystem/image1.stk",22) == 0 )
        {
           memset(lbuf,0x0,sizeof(lbuf));
           osapiStrncpy(lbuf,"/filesystem/image1",18);
        }
        
        if(osapiStrncmp(lbuf,"startup-config",14) == 0 )
        {
           memset(lbuf,0x0,sizeof(lbuf));
           osapiStrncpy(lbuf,"TempConfigScript.scr",21);
        }
        if(osapiStrncmp(lbuf,"backup-config",13) == 0 )
        {
           memset(lbuf,0x0,sizeof(lbuf));
           osapiStrncpy(lbuf,"TempConfigScript.scr",21);
        }

        if(osapiStrncmp(lbuf,"image2.stk",10) == 0 )
        {
           memset(lbuf,0x0,sizeof(lbuf));
           osapiStrncpy(lbuf,"image2\0",7);
        }

        if(osapiStrncmp(lbuf,"image1.stk",10) == 0 )
        {
           memset(lbuf,0x0,sizeof(lbuf));
           osapiStrncpy(lbuf,"image1\0",7);
        }
        
        EMWEB_TRACE(("Calling osapiFsOpen '%s'\n", lbuf));

      /*
       * set the file handle's path pointer into the params structure
       */
      fp->localPath = (char *)params->fileInfo.fileName;
      fp->op = ewaFileNeverDelete;  /* in case write fails */

      if ((fp->fd = osapiFsOpen(lbuf)) == L7_ERROR)
        {
          EMWEB_WARN( ("ewaFileGet: unable to open file\n") );
          ewaFree( fp );
          fp = EWA_FILE_HANDLE_NULL;
        }
    }
  return fp;

#endif
}

EwaFileHandle ewaFilePut (EwsContext context, const EwsFileParams * params)
{
  EMWEB_FILE_LOC ();
  return ewaFileGet (context, NULL, params);
}

EwaStatus ewaFileDelete (EwsContext context, const EwsFileParams * params)
{
  
  EMWEB_FILE_LOC ();
/*
     emwebFileHandler_t appFunc = ewaFileHandlerGet (context);
     appFunc ('c', params->fileInfo.fileName, &handle->appWap, NULL, 0);
     osapiFsDeleteFile ((L7_char8 *) params->fileInfo.fileName); 
*/
  return EWA_STATUS_OK;
}

static int http_download_started = FALSE;
void ewaFileAbortConnection(EwsContext context)
{
  if(context->http_download_started)
  {
  	http_download_started = 0;
  	context->http_download_started = 0;
  }
}

int ewaFileDownloadGetProgressGlobal(void)
{
  return http_download_started;
}

int ewaFileDownloadGetProgress(EwsContext context)
{
  return context->http_download_started;
}

static void ewaFileDownloadSetProgress(EwsContext context)
{
  /* mark the global http download started flag and note down that this context has set the flag */
  context->http_download_started = 1;
  http_download_started = 1;
  printf("\nHTTP File Transfer is in progress. Management access will be blocked for the duration of the transfer. please wait...\n");  
}
 
 
