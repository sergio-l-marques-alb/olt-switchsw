/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename emweb_if.c
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

#include "posix_config.h"
#include "ews_api.h"
#include "ewnet.h"

#define _EMWEB_IF_C
#include "emweb_if.h"
#include "ew_config.h"
#include "osapi_support.h"
#include "defaultconfig.h"

#ifdef L7_WIRELESS_PACKAGE
#include "wnv_api.h"
#endif


#ifdef L7_CAPTIVE_PORTAL_PACKAGE
#include "captive_portal_commdefs.h"
#endif


#ifdef L7_WIRELESS_PACKAGE
#include "wnv_api.h"
#endif



static int emwebTraceEnable = 0;
static int emwebTraceBufEnable = 0;

void *emwebWorkAreaGetByConnection (void *context);

void emwebFormSubmitSet (void *context, unsigned int type)
{
  ewsContextNetHandle (context)->submitType = type;
}

unsigned int emwebFormSubmitGet (void *context)
{
  emwebWa_t *ewap = emwebWorkAreaGetByConnection (context);
  return ewap->submitType;
}

void *emwebWorkAreaGetByConnection (void *context)
{
  return emwebWorkAreaGetByHandle (ewsContextNetHandle (context));
}

void *emwebWorkAreaGetByHandle (void *handle)
{
  return handle;
}

char *emwebRepeatInitDetails (void *context, short index, int count, int split, int max,
                              boolean more)
{
  emwebWa_t *ewap = emwebWorkAreaGetByConnection (context);

  ewap->repeatCount[index] = count;
  ewap->repeatCounter[index] = 0;

  if (split != 0)
  {
    if (split > count)
    {
      split = count;
    }
    ewap->repeatSplitCount[index] = split;
  }
  strcpy (ewap->repeatRetVal[index], "TRUE");
  if (max != 0)
  {
    ewap->repeatMax[index] = max;
    ewap->repeatMore[index] = more;
  }
  return (char *) &ewap->repeatCount[index];
}

static int getRowsPerRepeat (int curRepeat, int count, int split)
{
  int *a;
  int i, j, temp;
  int retVal = 0;
  int stopFlag = 0;

  a = (int *) osapiMalloc(L7_CLI_WEB_COMPONENT_ID, (split * sizeof (int)));

  for (i = 0; i < split; i++)
  {
    a[i] = 0;
  }

  /* distribute the rows across repeats */
  while (!stopFlag)
  {
    for (i = 0; i < split; i++)
    {
      temp = 0;
      for (j = 0; j < split; j++)
      {
        temp = temp + a[j];
      }
      if (temp + 1 > count)
      {
        stopFlag = 1;
        break;
      }
      else
      {
        ++a[i];
      }
    }                           /* end for */
  }                             /* end while */

  /* return the number of rows in the current repeat */
  retVal = a[curRepeat - 1];
  osapiFree (L7_CLI_WEB_COMPONENT_ID, a);
  return retVal;
}

static int getRepeatForRow (int row, int count, int split)
{

  int i;
  int temp = 0;

  for (i = 1; i <= split; i++)
  {
    temp = temp + getRowsPerRepeat (i, count, split);
    if (row <= temp)
    {
      return i;
    }
  }

  return 0;
}

char *emwebRepeatIsFirst (void *context, short index)
{
  emwebWa_t *ewap = emwebWorkAreaGetByConnection (context);

  int cur_repeat;
  int num_rows = 0;
  int i;
  int split, count;

  split = ewap->repeatSplitCount[index];
  count = ewap->repeatCount[index];

  cur_repeat = getRepeatForRow (++(ewap->repeatCounter[index]), count, split);

  /* count number of rows till last split */

  for (i = 1; i < cur_repeat; i++)
  {
    num_rows = num_rows + getRowsPerRepeat (i, count, split);
  }

  if (ewap->repeatCounter[index] == num_rows + 1)
  {
    return ewap->repeatRetVal[index];
  }

  return NULL;
}

int emwebGetRepeatCount (void *context, short index)
{
  int split, count;
  emwebWa_t *ewap = emwebWorkAreaGetByConnection (context);
  
  split = ewap->repeatSplitCount[index];

  count = ewap->repeatCount[index];
 
  return count;  
}

char *emwebRepeatIsLast (void *context, short index)
{
  emwebWa_t *ewap = emwebWorkAreaGetByConnection (context);

  int cur_repeat;
  int num_rows = 0;
  int i;
  int split, count;

  split = ewap->repeatSplitCount[index];
  count = ewap->repeatCount[index];

  cur_repeat = getRepeatForRow (ewap->repeatCounter[index], count, split);

  /* count number of rows till this split */
  for (i = 1; i <= cur_repeat; i++)
  {
    num_rows = num_rows + getRowsPerRepeat (i, count, split);
  }

  if (ewap->repeatCounter[index] == num_rows)
  {
    return ewap->repeatRetVal[index];
  }

  return NULL;
}

char *emwebRepeatIsMore (void *context, short index)
{
  emwebWa_t *ewap = emwebWorkAreaGetByConnection (context);
  if (ewap->repeatMore[index] == FALSE)
  {
    return NULL;
  }
  return ewap->repeatRetVal[index];
}

int emwebGetReqIntValue (void *context, char **buf16)
{
  emwebWa_t *ewap = emwebWorkAreaGetByConnection (context);
  *buf16 = ewap->numberBuf;

  return atoi ((char *) ewsContextGetValue (context));
}

void emwebFormValueSet (char **var, unsigned char *status, char *value, int isDyn)
{
  if (status == NULL || var == NULL)
  {
    printf ("emwebFormValueSet: null input vars 0x%lx 0x%lx\r\n", (long) status, (long) var);
    return;
  }
 
  if((*status & EW_FORM_DYNAMIC) != 0)
  {
    emwebFree(*var); 
  }  
  *status |= EW_FORM_INITIALIZED;
  if (isDyn)
  {
    char *ptr = emwebStrdup (value ? value : "");
    *status |= EW_FORM_DYNAMIC;
    if (ptr != NULL)
    {
      *var = ptr;
    }
    else
    {
      printf ("emwebFormValueSet: emwebStrdup failed\r\n");
    }
  }
  else if (value)
  {
    memcpy (var, value, 4);
  }
}

char *emwebContextBufferGet (void *context, unsigned short *bufSize)
{
  *bufSize = APP_BUFFER_SIZE;
  return ewsContextNetHandle (context)->buffer;
}

void *emwebAppWaGet (void *context, int size)
{
  return ewsContextNetHandle (context)->workarea;
}

void emwebAppWaSet (void *context, void *wap)
{
  ewsContextNetHandle (context)->workarea = wap;
}

void *emwebGetFilters (void *context)
{
  return &ewsContextNetHandle (context)->filters;
}

void emwebFilterDumpAppend (void *context, char *datap, boolean isWrite)
{
  char *current, *newVal;

  datap = datap ? datap : "";

  if (isWrite == TRUE)
  {
    current = ewsContextNetHandle (context)->filterDumpWrite;
  }
  else
  {
    current = ewsContextNetHandle (context)->filterDumpRead;
  }

  if (current)
  {
    newVal = ewaAlloc (strlen (current) + strlen (datap) + 1);
    sprintf (newVal, "%s%s", current, datap);
    emwebFree (current);
  }
  else
  {
    newVal = emwebStrdup (datap);
  }

  if (isWrite == TRUE)
  {
    ewsContextNetHandle (context)->filterDumpWrite = newVal;
  }
  else
  {
    ewsContextNetHandle (context)->filterDumpRead = newVal;
  }
}

char *emwebFilterDumpGet (void *context, boolean isWrite)
{
  if (isWrite == TRUE)
  {
    return ewsContextNetHandle (context)->filterDumpWrite;
  }
  return ewsContextNetHandle (context)->filterDumpRead;
}

void emwebFilterDumpSet (void *context, char *datap, boolean isWrite)
{
  if (isWrite == TRUE)
  {
    if (ewsContextNetHandle (context)->filterDumpWrite != NULL)
    {
      emwebFree (ewsContextNetHandle (context)->filterDumpWrite);
    }
    ewsContextNetHandle (context)->filterDumpWrite = datap;
  }
  else
  {
    if (ewsContextNetHandle (context)->filterDumpRead != NULL)
    {
      emwebFree (ewsContextNetHandle (context)->filterDumpRead);
    }
    ewsContextNetHandle (context)->filterDumpRead = datap;
  }
}

boolean emwebServeAfterSubmitGet (void *context)
{
  return ewsContextNetHandle (context)->serveAfterSubmit;
}

extern void ewaFileAbortConnection(EwsContext context); 
void emwebServeAfterSubmitSet (void *context, boolean val)
{
  ewsContextNetHandle (context)->serveAfterSubmit = val;
  /* cleanup if there are any file operations in this request */
  ewaFileAbortConnection(context);
}

char *emwebContextSendReply (void *context, char *file)
{
  EwaNetHandle handle = ewsContextNetHandle (context);

  if(file == NULL)
  {
    ewsContextSendReply (context, "/");
    return NULL;
  }

  if ( strcmp(file,"File_Upload_to_Server.html") == 0)
  {
    if (handle->downloadUrl[0] != 0)
    {
      return handle->downloadUrl;
    }
  }
  ewsContextSendReply (context, file);
  return NULL;
}

void emwebOptionSet (void *optp, char *bufChoice, void *valp, int sel)
{
  EwsFormSelectOptionP optionp = (EwsFormSelectOptionP) optp;

  optionp->choice = bufChoice;
  optionp->valuep = valp;
  optionp->selected = sel ? TRUE : FALSE;
}

static void emwebPrintString (char *str)
{
  int i, len;

  for (i = 0, len = strlen (str); i < len; i++)
  {
    if (str[i] == '\n')
    {
      printf ("\r");
    }
    printf ("%c", str[i]);
  }
}

void emweb_trace (const char *fmt, ...)
{
  va_list ap;
  char str[1024];

  if (emwebTraceEnable != 0)
  {
    memset (&ap, 0, sizeof (ap));
    va_start (ap, fmt);
    osapiVsnprintf (str, sizeof (str), fmt, ap);
    va_end (ap);
    emwebPrintString (str);
  }
}

void emweb_trace_buf (const char *fmt, ...)
{
  va_list ap;
  char str[1024];

  if (emwebTraceBufEnable != 0)
  {
    memset (&ap, 0, sizeof (ap));
    va_start (ap, fmt);
    osapiVsnprintf (str, sizeof (str), fmt, ap);
    va_end (ap);
    emwebPrintString (str);
  }
}

int emwebTraceSet (int val, int bufdbg)
{
  emwebTraceEnable = val;
  emwebTraceBufEnable = bufdbg;
  return 0;
}

char *emwebErrorBufGet (void *context, unsigned short *len)
{
  *len = EW_ERROR_BUF_SIZE;
  return ewsContextNetHandle (context)->errMsg;
}

char *emwebLevelCheckBuffer (void *context)
{
  return ewsContextNetHandle (context)->lvlCheckBuf;
}

char *emwebStrdup (char *srcString)
{
  int len;
  char *copy;

  len = strlen (srcString) + 1;
  copy = ewaAlloc (len);
  if (copy)
  {
    strcpy (copy, srcString);
  }
  else
  {
    EMWEB_ERROR (("ewaAlloc failure: %s\n", strerror (errno)));
  }

  return copy;
}

void emwebFree (void *str)
{
  ewaFree (str);
}

void ewaCopyInetAddrFromContext (EwsContext context, L7_inet_addr_t * ipAddrPtr)
{
  EwaNetHandle net = ewsContextNetHandle (context);
  memset (ipAddrPtr, 0, sizeof (L7_inet_addr_t));
  if (net->peer.u.sa.sa_family == AF_INET6)
  {
    if(L7_IP6_IS_ADDR_V4MAPPED(&(net->peer.u.sa6.sin6_addr)))
    {
      ipAddrPtr->family = L7_AF_INET;
#if defined(_L7_OS_VXWORKS_)
      ipAddrPtr->addr.ipv4.s_addr = net->peer.u.sa6.sin6_addr.in6.addr32[3];
#elif defined(_L7_OS_ECOS_)
      ipAddrPtr->addr.ipv4.s_addr = net->peer.u.sa6.sin6_addr.__u6_addr.__u6_addr32[3];
#elif !defined(PC_LINUX_HOST) /* hack */
      ipAddrPtr->addr.ipv4.s_addr = net->peer.u.sa6.sin6_addr.in6_u.u6_addr32[3];
#endif	  
    }
    else
    {
      ipAddrPtr->family = L7_AF_INET6;
      ipAddrPtr->addr.ipv6 = *(L7_in6_addr_t *)&net->peer.u.sa6.sin6_addr;
    }
  }
  else{
    ipAddrPtr->family = L7_AF_INET;
#if defined(_L7_OS_VXWORKS_)
    ipAddrPtr->addr.ipv4.s_addr = net->peer.u.sa4.sin_addr.s_addr;
#elif defined(_L7_OS_ECOS_)
    ipAddrPtr->addr.ipv4.s_addr = net->peer.u.sa4.sin_addr.s_addr;  
#elif !defined(PC_LINUX_HOST) /* hack */
    ipAddrPtr->addr.ipv4.s_addr = net->peer.u.sa6.sin6_addr.in6_u.u6_addr32[3];
#endif	  
  }
}
int ewaDumpSockDetails (int fd)
{
  int sndBufLen = 0;
  socklen_t optlen = sizeof (sndBufLen);
  getsockopt (fd, SOL_SOCKET, SO_SNDBUF, (char *) &sndBufLen, &optlen);
  printf ("getsockopt: SO_SNDBUF = %d\r\n", sndBufLen);
  return 0;
}

char *emwebContextGetValue (void *cntx)
{
  return (char *) ewsContextGetValue (cntx);
}

/* the code below is used only for old cli and new web */
extern void *ew_archive_data;
extern void *ew_archive;
void usmWebGetArchive (void **data, void **code)
{
  *data = (void *) &ew_archive_data;
  *code = (void *) ew_archive;
}
#ifdef _L7_OS_VXWORKS_
int emwebContextRemoteIpAddressGet(void* cntx,L7_inet_addr_t* remote)
{
  EwsContext context = (EwsContext)cntx;
  EwaNetHandle handle = ewsContextNetHandle(context);
  if (handle->peer.u.sa.sa_family == AF_INET6)  
  {
    if(L7_IP6_IS_ADDR_V4MAPPED(&handle->peer.u.sa6.sin6_addr))
    {
      remote->family = L7_AF_INET;
      remote->addr.ipv4.s_addr = handle->peer.u.sa6.sin6_addr.in6.addr32[3];

    }
    else
    {
      remote->family = L7_AF_INET6;  
      remote->addr.ipv6 = *(L7_in6_addr_t *)&handle->peer.u.sa6.sin6_addr;
    }    
  }
  else 
  { 
    remote->family = L7_AF_INET;
    remote->addr.ipv4.s_addr = osapiNtohl(handle->peer.u.sa4.sin_addr.s_addr);  

  }
  return 0;
}

char* emwebFilePathHandler(EwsContext context,char* url)
{
  L7_BOOL fNullRealm = L7_FALSE; /* Allows local file gets */

#ifdef EW_CONFIG_OPTION_FILE_GET
  {
    char *p;
    EwsFileParams   *params = NULL;
    struct stat fs;

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
    if (((p = strstr( url, CP_URL_FILE_SYSTEM_PATH)) != NULL) ||
        ((p = strstr( url, FILE_SYSTEM_URL_NV_PATH)) != NULL) ||
        ((p = strstr( url, FILE_SYSTEM_URL_PATH)) != NULL))
#else
    if (((p = strstr( url, FILE_SYSTEM_URL_NV_PATH)) != NULL) ||
        ((p = strstr( url, FILE_SYSTEM_URL_PATH)) != NULL))
#endif
    {
      fNullRealm = L7_TRUE;
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
      if ((strstr(url,CP_URL_FILE_SYSTEM_PATH)) != NULL)
      {
        p += strlen( CP_URL_FILE_SYSTEM_PATH );
      }
      else 
#endif
      if ((strstr(url,FILE_SYSTEM_URL_NV_PATH)) != NULL)
      {
        p += strlen( FILE_SYSTEM_URL_NV_PATH );
      }
      else
      {
        p += strlen( FILE_SYSTEM_URL_PATH );
      }
      bzero( (char *)&ewsContextNetHandle( context )->fileParams
          ,sizeof( EwsFileParams ) );

      /* build the pathname */
      strcpy( ewsContextNetHandle( context )->pathName,FILE_SYSTEM_PATH );

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
      if ((strstr(url,CP_URL_FILE_SYSTEM_PATH)) != NULL)
      {
        strcpy( ewsContextNetHandle( context )->pathName,RAM_CP_NAME );
      }
#endif /* L7_CAPTIVE_PORTAL_PACKAGE */

#ifdef L7_WIRELESS_PACKAGE
      if ((strstr(url,FILE_SYSTEM_URL_NV_PATH)) != NULL)
      {
        strcpy( ewsContextNetHandle( context )->pathName,RAM_NV_NAME );
      }
#endif /* L7_WIRELESS_PACKAGE */

      strncat( ewsContextNetHandle( context )->pathName,p,strlen(p) );
      do
      {
        char lbuf[256];
        memset(lbuf,0x0,sizeof(lbuf));
        EMWEB_TRACE (( "FileName in the Context %s",ewsContextNetHandle( context )->pathName));
        if ( strncmp(ewsContextNetHandle( context )->pathName ,"image1.stk",10 ) == 0 )
        {
           osapiStrncpy(lbuf,"image1",7);
        }
        else if ( strncmp(ewsContextNetHandle( context )->pathName ,"image2.stk",10 ) == 0 )
        {
           osapiStrncpy(lbuf,"image2",7);
        }
        else if ( strncmp(ewsContextNetHandle( context )->pathName ,"startup-config",14 ) == 0 )
        {
           osapiStrncpy(lbuf,"TempConfigScript.scr",21);
        }
        else if ( strncmp(ewsContextNetHandle( context )->pathName ,"backup-config",13 ) == 0 )
        {
           osapiStrncpy(lbuf,"TempConfigScript.scr",21);
        }
        else
        {
           osapiStrncpy(lbuf,ewsContextNetHandle( context )->pathName,strlen(ewsContextNetHandle( context )->pathName));
        }

        EMWEB_TRACE (( "lbuf '%s'\n", lbuf));
      if ( stat( lbuf,&fs ) == OK
          && S_ISREG( fs.st_mode )
         )
      {
        struct tm *tptr;

        params = &ewsContextNetHandle( context )->fileParams;
        params->fileInfo.fileName = ewsContextNetHandle( context )->pathName;

        /*
         * get date of last modification, if possible
         */

        if ((tptr = gmtime( &fs.st_mtime )) != NULL)
        {
          if  (strftime( ewsContextNetHandle(context)->lastModified
                ,FILE_DATE_LEN
                /* day, xx mth year hh:mm:ss zone */
                ,"%a, %d %b %Y %H:%M:%S %Z"
                ,tptr )
              != 0)
          {
            params->fileInfo.lastModified =
              ewsContextNetHandle(context)->lastModified;
          }

          if (strftime( ewsContextNetHandle(context)->lastModified1036
                ,FILE_DATE_LEN
                /* fullday, xx-mth-yy hh:mm:ss zone */
                ,"%A, %d-%b-%d %H:%M:%S %Z"
                ,tptr )
              != 0)
          {
            params->fileInfo.lastModified1036 =
              ewsContextNetHandle(context)->lastModified1036;
          }
        }

        /*
         * get file size, and allow get method
         */

        params->fileInfo.contentLength = fs.st_size;
        params->fileInfo.allow = ewsRequestMethodGet;

        if (fNullRealm==L7_TRUE)
        {
          /* Open up local file access for specific locations */
          params->fileInfo.realm = NULL;
        }

        /*
         * fudge the content type.  For this
         * exercise, I'm assuming that the file
         * extension determines content type.
         */
        {
          const struct
          {
            char *extension;
            char *type;
          } typeTable[] =
          {
            {"htm", "text/html"},
            {"html", "text/html"},
            {"txt",  "text/plain"},
            {"gif",  "image/gif"},
            {"jpg",  "image/jpeg"},
            {NULL, NULL}      /* terminator */
          };

          p = (char *)params->fileInfo.fileName + strlen( params->fileInfo.fileName );
          while (   p != params->fileInfo.fileName
              && *p != '.')
            p--;
          if (*p++ == '.')
          {
            int index;
            for (index = 0;
                typeTable[index].extension != NULL;
                index++)
            {
              if (strcmp( p, typeTable[index].extension) == 0)
              {
                params->fileInfo.contentType = typeTable[index].type;
                break;
              }
            }
          }

       if ((!strcmp(ewsContextNetHandle( context )->pathName,"startup-config")) ||
            (!strcmp(ewsContextNetHandle( context )->pathName,"running-config")) || 
             (!strcmp(ewsContextNetHandle( context )->pathName,"slog0.txt")) || 
              (!strcmp(ewsContextNetHandle( context )->pathName,"olog0.txt")) ||
               (!strcmp(ewsContextNetHandle( context )->pathName,"backup-config"))) 
       {
#ifdef EW_CONFIG_OPTION_DISPOSITION
           params->fileInfo.contentDisposition = ewsContextNetHandle( context )->pathName;
#endif
           params->fileInfo.contentType = "text/plain";
       }
          if (!params->fileInfo.contentType)
          {
            params->fileInfo.contentType = "application/octet-stream";
          }
        }

      }
      } while(0);

      /*
       * at this point, we're looking at a valid local file if
       * params is not NULL
       */
      if (params != NULL)
      {
        EMWEB_TRACE(("ewsContextSetFile call\n"));
        ewsContextSetFile( context, params );
      }
    }     /* end if URL points to local filesystem */
  }
# endif /* EW_CONFIG_OPTION_FILE_GET */

  return url;
}
#elif _L7_OS_LINUX_
int emwebContextRemoteIpAddressGet(void* cntx,L7_inet_addr_t* remote)
{
  EwsContext context = (EwsContext)cntx;
  EwaNetHandle handle = ewsContextNetHandle(context);
  if (handle->peer.u.sa.sa_family == AF_INET6)
  {
    if(L7_IP6_IS_ADDR_V4MAPPED(&handle->peer.u.sa6.sin6_addr))
    {
      remote->family = L7_AF_INET;
      remote->addr.ipv4.s_addr = L7_IP6_ADDR_V4MAPPED(&handle->peer.u.sa6.sin6_addr);
    }
    else
    {
      remote->family = L7_AF_INET6;
      remote->addr.ipv6 = *(L7_in6_addr_t *)&handle->peer.u.sa6.sin6_addr;
      L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
          "%s IPV6 is not currently supported",__FUNCTION__);
    }
  }
  else
  {
    remote->family = L7_AF_INET;
    remote->addr.ipv4.s_addr = osapiNtohl(handle->peer.u.sa4.sin_addr.s_addr);
  }
  return 0; 
}

char* emwebFilePathHandler(EwsContext context,char* url)
{
  EwaNetHandle handle = ewsContextNetHandle(context);
  /*
   * If URL begins with URL prefix, then map URL to local filesystem
   * instead of installed EmWeb archives.
   *
   * For example, map '/filesystem/x/y/z.html' => '/mnt/filesystem/x/y/z.html'
   *
   * We don't allow ".." to appear in the path for security.
   */
# if defined( EW_CONFIG_OPTION_FILE_GET ) \
  || defined( EW_CONFIG_OPTION_FILE_PUT ) \
  || defined( EW_CONFIG_OPTION_FILE_DELETE )

  char *ptr;

  if ((ptr = strstr( url, EMWEB_FILE_LOCAL_PREFIX )) == url)
  {
    static char prefix[] = EMWEB_FILE_LOCAL_ROOT;
    static char toc[]    = EMWEB_FILE_LOCAL_INDEX;
    STATBUF statbuf;
    struct tm  *tmp;
    char *path;
    char *date;
    char *date1036;
    char *cp;

    /*
     * Disallow ".." in path for security
     */
    if (strstr(url, "..") != NULL)
    {
      /* EMWEB_WARN(("ewaURLHook: Attempted .. in local file path\n")); */
      return NULL;
    }

    /*
     * setup the file information structure
     */
    ptr += strlen( EMWEB_FILE_LOCAL_PREFIX );  /* skip prefix */
    if (handle->params != NULL)
    {
      ewaFree(handle->params); /* may be same connection, so free prev */
    }

    if ((handle->params = (EwsFileParamsP)
          ewaAlloc(  sizeof( EwsFileParams )
            + strlen( ptr )
            + strlen( prefix )
            + strlen( toc )
            + 30 /* RFC1123 date */
            + 34 /* RFC1036 date */
            + 1)
        ) == NULL)
    {
      EMWEB_ERROR(("ewaURLHook: unable to allocate file parameters\n"));
      return NULL;
    }
    memset( &handle->params->fileInfo
        ,0
        ,sizeof( handle->params->fileInfo )
        );

    /* build pathname under local root */
    date = (char *)(handle->params + 1);
    date1036 = date + 30;
    path = date1036 + 34;
    strcpy( path, prefix );
    strcat( path, ptr );

    /*
     * Read file information.
     */
    if (STAT(path, &statbuf) < 0)
    {

#         ifdef EW_CONFIG_OPTION_FILE_PUT
      /*
       * If file is not found, and method is not PUT, then return URL
       * (without file information) to cause a "not found" error.
       */
      if (ewsContextRequestMethod(context) != ewsRequestMethodPut)
      {
        ewaFree(handle->params);
        handle->params = NULL;
        return url;
      }

      /*
       * If method is PUT and file is not found, then set file with
       * PUT method allowed.
       */
      handle->params->fileInfo.allow = ewsRequestMethodPut;
      handle->params->fileInfo.realm = "File Write";
      handle->params->fileInfo.fileName = path;
      if (ewsContextSetFile( context, handle->params ) != EWS_STATUS_OK)
      {
        EMWEB_ERROR(( "ewaURLHook: ewsContextSetFile failed\n" ));
        ewaFree( handle->params );
        handle->params = NULL;
        return NULL;
      }
      return url;

#         else /* !EW_CONFIG_OPTION_FILE_PUT */

      ewaFree(handle->params);
      handle->params = NULL;
      return url;

#         endif /* EW_CONFIG_OPTION_FILE_PUT */

    }
    /*
     * If accessing a directory, append the index file to URL and
     * send redirect
     */
    if (S_ISDIR(statbuf.st_mode))
    {
      strcpy(path, url);
      strcat(path, toc);
      ewsContextSendRedirect(context, path);
      return NULL;
    }

    /*
     * Set mime type according to path suffix.  For now, we assume
     * text/html unless .gif
     */
    cp = strrchr(path, '.');
    if (!strcmp(".txt", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "text/plain";
    }
    else if (!strcmp(".jpg", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "image/jpeg";
    }
    else if (!strcmp(".png", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "image/png";
    }
    else if (!strcmp(".gif", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "image/gif";
    }
    else if (!strcmp(".html", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "text/html";
    }
    else if (!strcmp(".htm", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "text/html";
    }
    else if (!strcmp(".xml", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "text/xml";
    }
    else if (!strcmp(".js", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "text/javascript";
    }
    else if (!strcmp(".css", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "text/css";
    }
    else if (!strcmp(".pdf", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "applications/pdf";
    }
    else if (!strcmp(".ps", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "applications/postscript";
    }
    else
    {
       if ((!strcmp(path,"/filesystem/running-config")) ||
            (!strcmp(path,"/filesystem/startup-config")) || 
              (!strcmp(path,"/filesystem/backup-config"))) 
       {
#ifdef EW_CONFIG_OPTION_DISPOSITION
           handle->params->fileInfo.contentDisposition = path+12;
#endif
           handle->params->fileInfo.contentType = "text/plain";
       }
       else
       {
          handle->params->fileInfo.contentType = "application/octet-stream";
       }
    }
    /*
     * If '/filesystem/public/...' or '/filesystem/ramnv' or '/filesystem/ramcp'
     * then disable authentication
     */
    if ((strstr( url, EMWEB_FILE_LOCAL_PUBLIC_PREFIX ) == url) ||
        (strstr( url, EMWEB_FILE_LOCAL_RAM_NV_PREFIX ) == url) ||
        (strstr( url, EMWEB_FILE_LOCAL_RAM_CP_PREFIX ) == url))
    {
      handle->params->fileInfo.realm = NULL;
    }

    /*
     * Otherwise, assign realm for access control
     */
    else
    {
      handle->params->fileInfo.realm = "File Read";
    }

#      ifdef EW_CONFIG_OPTION_DATE
    static const char *month[] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun",
      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    static const char *day[] = {
      "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    /* !TBD! pSOS gmtime doesn't work */
    tmp = gmtime(&statbuf.st_mtime);
    sprintf( date
        , "%s, %02d %s %04d %02d:%02d:%02d GMT"
        ,day[tmp->tm_wday]
        ,tmp->tm_mday
        ,month[tmp->tm_mon]
        ,tmp->tm_year + 1900
        ,tmp->tm_hour
        ,tmp->tm_min
        ,tmp->tm_sec
        );
    {
      /* Put this array here because the only reference to date1036 is
       * in the following sprintf(), and this code is heavily
       * conditionalized. */

      static const char *day1036[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
        "Saturday"
      };

      sprintf( date1036
          , "%s, %02d-%s-%02d %02d:%02d:%02d GMT"
          , day1036[tmp->tm_wday]
          , tmp->tm_mday
          , month[tmp->tm_mon]
          ,tmp->tm_year % 100
          ,tmp->tm_hour
          ,tmp->tm_min
          ,tmp->tm_sec
          );
    }
#      endif
    /*
     * Set up remaining file information
     */
    handle->params->fileInfo.fileName = path;
    handle->params->fileInfo.contentLength = statbuf.st_size;
    handle->params->fileInfo.allow = ( ewsRequestMethodGet
#                                       ifdef EW_CONFIG_OPTION_FILE_PUT
        | ewsRequestMethodPut
#                                       endif /* EW_CONFIG_OPTION_FILE_PUT */
#                                       ifdef EW_CONFIG_OPTION_FILE_DELETE
        | ewsRequestMethodDelete
#                                       endif /* EW_CONFIG_OPTION_FILE_DELETE */
        );
    handle->params->fileInfo.lastModified = date;
    handle->params->fileInfo.lastModified1036 = date1036;
    handle->params->fileInfo.eTag = "";

#     ifdef EW_CONFIG_OPTION_FILE_DELETE
    /*
     * If DELETE method, set access controls
     */
    if (ewsContextRequestMethod(context) == ewsRequestMethodDelete)
    {
      handle->params->fileInfo.realm = "File Write";
    }
#     endif /* EW_CONFIG_OPTION_FILE_DELETE */

#     ifdef EW_CONFIG_OPTION_BYTE_RANGES
    /*
     * If byte range, adjust params
     */
    {
      int32 firstBytePos;
      int32 lastBytePos;
      handle->params->fileInfo.acceptRanges = TRUE;
      handle->params->fileInfo.instanceLength = statbuf.st_size;
      if (ewsContextRange(context, &firstBytePos, &lastBytePos)
          == EWS_STATUS_OK)
      {
        if (firstBytePos < 0)
        {
          firstBytePos += statbuf.st_size;
          lastBytePos = statbuf.st_size - 1;
        }
        if (lastBytePos == EWS_CONTENT_LENGTH_UNKNOWN)
        {
          lastBytePos = statbuf.st_size - 1;
        }
        if (firstBytePos >= statbuf.st_size ||
            lastBytePos >= statbuf.st_size
           )
        {
          ewsContextSendRangeError(context, statbuf.st_size);
          return NULL;
        }

        handle->params->fileInfo.isRange = TRUE;
        handle->params->fileInfo.firstBytePos = firstBytePos;
        handle->params->fileInfo.lastBytePos = lastBytePos;
        handle->params->fileInfo.contentLength
          = 1 + lastBytePos - firstBytePos;
      }
    }
#     endif /* EW_CONFIG_OPTION_BYTE_RANGES */

    /*
     * Notify EmWeb/Server to serve file from local filesystem instead
     * of installed archives.
     */
    if (ewsContextSetFile( context, handle->params ) != EWS_STATUS_OK)
    {
      EMWEB_ERROR(( "ewaURLHook: ewsContextSetFile failed\n" ));
      ewaFree( handle->params );
      handle->params = NULL;
      return NULL;
    }
  }
# endif /* EW_CONFIG_OPTION_FILE_* */


  return url;
}
#elif _L7_OS_ECOS_
int emwebContextRemoteIpAddressGet(void* cntx,L7_inet_addr_t* remote)
{
  EwsContext context = (EwsContext)cntx;
  EwaNetHandle handle = ewsContextNetHandle(context);
#if 0
  if (handle->peer.u.sa.sa_family == AF_INET6)
  {
    if(L7_IP6_IS_ADDR_V4MAPPED(&handle->peer.u.sa6.sin6_addr))
    {
      remote->family = L7_AF_INET;
      remote->addr.ipv4.s_addr = L7_IP6_ADDR_V4MAPPED(&handle->peer.u.sa6.sin6_addr);
    }
    else
    {
      remote->family = L7_AF_INET6;
      remote->addr.ipv6 = *(L7_in6_addr_t *)&handle->peer.u.sa6.sin6_addr;
      L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
          "%s IPV6 is not currently supported",__FUNCTION__);
    }
  }
  else
#endif
  {
    remote->family = L7_AF_INET;
    remote->addr.ipv4.s_addr = osapiNtohl(handle->peer.u.sa4.sin_addr.s_addr);
  }
  return 0; 
}

char* emwebFilePathHandler(EwsContext context,char* url)
{
  EwaNetHandle handle = ewsContextNetHandle(context);
  /*
   * If URL begins with URL prefix, then map URL to local filesystem
   * instead of installed EmWeb archives.
   *
   * For example, map '/filesystem/x/y/z.html' => '/mnt/filesystem/x/y/z.html'
   *
   * We don't allow ".." to appear in the path for security.
   */
# if defined( EW_CONFIG_OPTION_FILE_GET ) \
  || defined( EW_CONFIG_OPTION_FILE_PUT ) \
  || defined( EW_CONFIG_OPTION_FILE_DELETE )

  char *ptr;

  if ((ptr = strstr( url, EMWEB_FILE_LOCAL_PREFIX )) == url)
  {
    static char prefix[] = EMWEB_FILE_LOCAL_ROOT;
    static char toc[]    = EMWEB_FILE_LOCAL_INDEX;
    STATBUF statbuf;
    struct tm  *tmp;
    char *path;
    char *date;
    char *date1036;
    char *cp;

    /*
     * Disallow ".." in path for security
     */
    if (strstr(url, "..") != NULL)
    {
      /* EMWEB_WARN(("ewaURLHook: Attempted .. in local file path\n")); */
      return NULL;
    }

    /*
     * setup the file information structure
     */
    ptr += strlen( EMWEB_FILE_LOCAL_PREFIX );  /* skip prefix */
    if (handle->params != NULL)
    {
      ewaFree(handle->params); /* may be same connection, so free prev */
    }

    if ((handle->params = (EwsFileParamsP)
          ewaAlloc(  sizeof( EwsFileParams )
            + strlen( ptr )
            + strlen( prefix )
            + strlen( toc )
            + 30 /* RFC1123 date */
            + 34 /* RFC1036 date */
            + 1)
        ) == NULL)
    {
      EMWEB_ERROR(("ewaURLHook: unable to allocate file parameters\n"));
      return NULL;
    }
    memset( &handle->params->fileInfo
        ,0
        ,sizeof( handle->params->fileInfo )
        );

    /* build pathname under local root */
    date = (char *)(handle->params + 1);
    date1036 = date + 30;
    path = date1036 + 34;
    strcpy( path, prefix );
    strcat( path, ptr );

    /*
     * Read file information.
     */
    if (STAT(path, &statbuf) < 0)
    {

#         ifdef EW_CONFIG_OPTION_FILE_PUT
      /*
       * If file is not found, and method is not PUT, then return URL
       * (without file information) to cause a "not found" error.
       */
      if (ewsContextRequestMethod(context) != ewsRequestMethodPut)
      {
        ewaFree(handle->params);
        handle->params = NULL;
        return url;
      }

      /*
       * If method is PUT and file is not found, then set file with
       * PUT method allowed.
       */
      handle->params->fileInfo.allow = ewsRequestMethodPut;
      handle->params->fileInfo.realm = "File Write";
      handle->params->fileInfo.fileName = path;
      if (ewsContextSetFile( context, handle->params ) != EWS_STATUS_OK)
      {
        EMWEB_ERROR(( "ewaURLHook: ewsContextSetFile failed\n" ));
        ewaFree( handle->params );
        handle->params = NULL;
        return NULL;
      }
      return url;

#         else /* !EW_CONFIG_OPTION_FILE_PUT */

      ewaFree(handle->params);
      handle->params = NULL;
      return url;

#         endif /* EW_CONFIG_OPTION_FILE_PUT */

    }
    /*
     * If accessing a directory, append the index file to URL and
     * send redirect
     */
    if (S_ISDIR(statbuf.st_mode))
    {
      strcpy(path, url);
      strcat(path, toc);
      ewsContextSendRedirect(context, path);
      return NULL;
    }

    /*
     * Set mime type according to path suffix.  For now, we assume
     * text/html unless .gif
     */
    cp = strrchr(path, '.');
    if (!strcmp(".txt", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "text/plain";
    }
    else if (!strcmp(".jpg", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "image/jpeg";
    }
    else if (!strcmp(".png", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "image/png";
    }
    else if (!strcmp(".gif", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "image/gif";
    }
    else if (!strcmp(".html", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "text/html";
    }
    else if (!strcmp(".htm", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "text/html";
    }
    else if (!strcmp(".xml", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "text/xml";
    }
    else if (!strcmp(".js", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "text/javascript";
    }
    else if (!strcmp(".css", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "text/css";
    }
    else if (!strcmp(".pdf", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "applications/pdf";
    }
    else if (!strcmp(".ps", cp == NULL ? "" : cp))
    {
      handle->params->fileInfo.contentType = "applications/postscript";
    }
    else
    {
       if ((!strcmp(path,"/filesystem/running-config")) ||
            (!strcmp(path,"/filesystem/startup-config")) || 
              (!strcmp(path,"/filesystem/backup-config"))) 
       {
#ifdef EW_CONFIG_OPTION_DISPOSITION
           handle->params->fileInfo.contentDisposition = path+12;
#endif
           handle->params->fileInfo.contentType = "text/plain";
       }
       else
       {
          handle->params->fileInfo.contentType = "application/octet-stream";
       }
    }
    /*
     * If '/filesystem/public/...' or '/filesystem/ramnv' or '/filesystem/ramcp'
     * then disable authentication
     */
    if ((strstr( url, EMWEB_FILE_LOCAL_PUBLIC_PREFIX ) == url) ||
        (strstr( url, EMWEB_FILE_LOCAL_RAM_NV_PREFIX ) == url) ||
        (strstr( url, EMWEB_FILE_LOCAL_RAM_CP_PREFIX ) == url))
    {
      handle->params->fileInfo.realm = NULL;
    }

    /*
     * Otherwise, assign realm for access control
     */
    else
    {
      handle->params->fileInfo.realm = "File Read";
    }

#      ifdef EW_CONFIG_OPTION_DATE
    static const char *month[] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun",
      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    static const char *day[] = {
      "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    /* !TBD! pSOS gmtime doesn't work */
    tmp = gmtime(&statbuf.st_mtime);
    sprintf( date
        , "%s, %02d %s %04d %02d:%02d:%02d GMT"
        ,day[tmp->tm_wday]
        ,tmp->tm_mday
        ,month[tmp->tm_mon]
        ,tmp->tm_year + 1900
        ,tmp->tm_hour
        ,tmp->tm_min
        ,tmp->tm_sec
        );
    {
      /* Put this array here because the only reference to date1036 is
       * in the following sprintf(), and this code is heavily
       * conditionalized. */

      static const char *day1036[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
        "Saturday"
      };

      sprintf( date1036
          , "%s, %02d-%s-%02d %02d:%02d:%02d GMT"
          , day1036[tmp->tm_wday]
          , tmp->tm_mday
          , month[tmp->tm_mon]
          ,tmp->tm_year % 100
          ,tmp->tm_hour
          ,tmp->tm_min
          ,tmp->tm_sec
          );
    }
#      endif
    /*
     * Set up remaining file information
     */
    handle->params->fileInfo.fileName = path;
    handle->params->fileInfo.contentLength = statbuf.st_size;
    handle->params->fileInfo.allow = ( ewsRequestMethodGet
#                                       ifdef EW_CONFIG_OPTION_FILE_PUT
        | ewsRequestMethodPut
#                                       endif /* EW_CONFIG_OPTION_FILE_PUT */
#                                       ifdef EW_CONFIG_OPTION_FILE_DELETE
        | ewsRequestMethodDelete
#                                       endif /* EW_CONFIG_OPTION_FILE_DELETE */
        );
    handle->params->fileInfo.lastModified = date;
    handle->params->fileInfo.lastModified1036 = date1036;
    handle->params->fileInfo.eTag = "";

#     ifdef EW_CONFIG_OPTION_FILE_DELETE
    /*
     * If DELETE method, set access controls
     */
    if (ewsContextRequestMethod(context) == ewsRequestMethodDelete)
    {
      handle->params->fileInfo.realm = "File Write";
    }
#     endif /* EW_CONFIG_OPTION_FILE_DELETE */

#     ifdef EW_CONFIG_OPTION_BYTE_RANGES
    /*
     * If byte range, adjust params
     */
    {
      int32 firstBytePos;
      int32 lastBytePos;
      handle->params->fileInfo.acceptRanges = TRUE;
      handle->params->fileInfo.instanceLength = statbuf.st_size;
      if (ewsContextRange(context, &firstBytePos, &lastBytePos)
          == EWS_STATUS_OK)
      {
        if (firstBytePos < 0)
        {
          firstBytePos += statbuf.st_size;
          lastBytePos = statbuf.st_size - 1;
        }
        if (lastBytePos == EWS_CONTENT_LENGTH_UNKNOWN)
        {
          lastBytePos = statbuf.st_size - 1;
        }
        if (firstBytePos >= statbuf.st_size ||
            lastBytePos >= statbuf.st_size
           )
        {
          ewsContextSendRangeError(context, statbuf.st_size);
          return NULL;
        }

        handle->params->fileInfo.isRange = TRUE;
        handle->params->fileInfo.firstBytePos = firstBytePos;
        handle->params->fileInfo.lastBytePos = lastBytePos;
        handle->params->fileInfo.contentLength
          = 1 + lastBytePos - firstBytePos;
      }
    }
#     endif /* EW_CONFIG_OPTION_BYTE_RANGES */

    /*
     * Notify EmWeb/Server to serve file from local filesystem instead
     * of installed archives.
     */
    if (ewsContextSetFile( context, handle->params ) != EWS_STATUS_OK)
    {
      EMWEB_ERROR(( "ewaURLHook: ewsContextSetFile failed\n" ));
      ewaFree( handle->params );
      handle->params = NULL;
      return NULL;
    }
  }
# endif /* EW_CONFIG_OPTION_FILE_* */


  return url;
}


#endif

int emwebContextListenPortGet(void* cntx,int* listenPort)
{
  EwsContext context = (EwsContext)cntx;
  EwaNetHandle handle = ewsContextNetHandle(context);
  if (handle->peer.u.sa.sa_family == AF_INET6)
  {
    if(L7_IP6_IS_ADDR_V4MAPPED(&handle->peer.u.sa6.sin6_addr))
    {
      *listenPort = handle->listeningPort;
    }

  }
  else
  {

    *listenPort = handle->listeningPort;
  }
  return 0;
}

char *emwebIsRepeatNotEmpty(void *context, short index)
{
  int count = 0;

  count = emwebGetRepeatCount (context, index); 

  if (count != 0)
  {
    return "TRUE";
  }

  return NULL;
}


