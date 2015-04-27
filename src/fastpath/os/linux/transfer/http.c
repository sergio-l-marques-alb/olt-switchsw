/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2008
 *
 **********************************************************************
 *
 * @filename http.c
 *
 * @purpose
 *
 * @component Transfer
 *
 * @comments
 *
 * @create 22-Jun-2008
 *
 * @author Rama Sasthri, Kristipati
 * @end
 *
 **********************************************************************/

#include <stdio.h>
#include <string.h>
#include "osapi.h"

#define OSAPI_WEB_FILE_HANDLE_COMP 100
#define L7_EMWEB_FILE_DOWNLOAD_PATH "/tmp/"                                     /* PTin modified: paths */

#define L7_EMWEB_NV_FILE_DOWNLOAD_PATH "/usr/local/ptin/log/fastpath/ramnv/"    /* PTin modified: paths */
#define L7_EMWEB_CP_FILE_DOWNLOAD_PATH "/usr/local/ptin/log/fastpath/ramcp/"    /* PTin modified: paths */

typedef struct
{
  int fd;
  char *localPath;              /* local file pathname */
  long magic;
  int remove;
} osapiWebFileHandle_t;

int osapiWebFileDescriptorGet (void *appWap)
{
  int fd = 0;
  if (appWap != NULL)
  {
    fd = ((osapiWebFileHandle_t *)appWap)->fd;
  }
  return fd;
}

static osapiWebFileHandle_t *osapiWebFileHandleCreate (char *path, char *filename, int remove)
{
  osapiWebFileHandle_t *fp;

  int len = strlen (path) + 1 + strlen (filename) + 1;
  fp =
    (osapiWebFileHandle_t *) osapiMalloc (OSAPI_WEB_FILE_HANDLE_COMP,
        sizeof (osapiWebFileHandle_t) + len);
  if (fp == NULL)
  {
    return fp;
  }
  fp->localPath = (char *) (fp + 1);
  fp->remove = remove;

  osapiSnprintf (fp->localPath, len, "%s%s", path, filename);
  return fp;
}

int osapiWebFileHandler (int op, char *filename, void **handle, char *data, int length)
{
  int retval = 0;
  osapiWebFileHandle_t *fp = *(osapiWebFileHandle_t **) handle;
  L7_char8 *pathname = L7_EMWEB_FILE_DOWNLOAD_PATH;

  
  if ((op == 'n') || (op == 'x') || (op == 'y'))               /* create new file */
  {
    #if defined (L7_WIRELESS_PACKAGE) || defined (L7_CAPTIVE_PORTAL_PACKAGE)
    {
      char *GIF = ".GIF";
      char *JPG = ".JPG";
      char *gif = ".gif";
      char *jpg = ".jpg";
 
      /* Check the file type being downloaded*/ 
      if ((strstr(filename,GIF)==NULL) ||
          (strstr(filename,JPG)==NULL) ||
          (strstr(filename,gif)==NULL) ||
          (strstr(filename,jpg)==NULL))
      {
        /* NV file downlaod needs to be done in the respective location created by NV component*/
        if (op == 'x') /* Create new file for Captive Portal Download */
        {
          pathname = L7_EMWEB_CP_FILE_DOWNLOAD_PATH;
        }
        else if (op == 'y') /* Create new file for Wireless download */
        {
          pathname = L7_EMWEB_NV_FILE_DOWNLOAD_PATH;
        }
      }
    }
    #endif

    /* create work area */
    fp = osapiWebFileHandleCreate (pathname, filename, 1);

    /* create file */
    if (L7_SUCCESS != osapiFsFileCreate ((L7_char8 *) fp->localPath, &fp->fd))
    {
      osapiFree (OSAPI_WEB_FILE_HANDLE_COMP, fp);
      retval = -1;
      *handle = NULL;
    }
    else
    {
      *handle = fp;
    }
  }
  else if (op == 'o')           /* open */
  {
    /* create work area */
    fp = osapiWebFileHandleCreate ("", filename, 0);

    /* open the path */
    if (osapiFsOpen (fp->localPath, &fp->fd) == L7_ERROR)
    {
      osapiFree (OSAPI_WEB_FILE_HANDLE_COMP, fp);
      *handle = NULL;
      retval = -1;
    }
    else
    {
      *handle = fp;
    }
  }
  else if (op == 'w')           /* write */
  {
    if (L7_SUCCESS != osapiFsWriteNoClose (fp->fd, (char *) data, length))
    {
      retval = -1;
    }
    else
    {
      retval = length;
    }
  }
  else if (op == 'r')           /* read */
  {
    retval = length;
    if (L7_SUCCESS != osapiFileReadWithLen (fp->fd, (char *) data, &retval))
    {
      retval = -1;
    }
  }
  else if (op == 'c')           /* close */
  {
    (void) osapiFsClose (fp->fd);
  }
  else if (op == 'f')           /* flush */
  {
    if (fp->remove)
    {
      (void) osapiFsDeleteFile (fp->localPath);
    }
    osapiFree (OSAPI_WEB_FILE_HANDLE_COMP, fp);
    *handle = NULL;
  }
  else
  {
    return -1;
  }
  return retval;
}

L7_RC_t osapiWebFileCommit (L7_uint32 fileType, char *fileBaseName)
{
  return L7_SUCCESS;
}

L7_RC_t httpProcess_config_finish( L7_char8 *lfname )
{
  return L7_SUCCESS;
}

