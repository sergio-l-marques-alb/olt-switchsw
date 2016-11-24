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
#define L7_EMWEB_FILE_DOWNLOAD_PATH "/tmp/"

typedef struct
{
  int fd;
  char *localPath;              /* local file pathname */
  long magic;
  int remove;
} osapiWebFileHandle_t;

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

  if (op == 'n')                /* create new file */
  {
    /* create work area */
    fp = osapiWebFileHandleCreate (L7_EMWEB_FILE_DOWNLOAD_PATH, filename, 1);

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
    fp->fd = osapiFsOpen (fp->localPath);
    if (fp->fd == L7_ERROR)
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

