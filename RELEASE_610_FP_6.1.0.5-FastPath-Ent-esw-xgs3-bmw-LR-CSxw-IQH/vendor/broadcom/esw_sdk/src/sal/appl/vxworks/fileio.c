/*
 * $Id: fileio.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:        fileio.c
 * Purpose:     File I/O
 */

#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>

#define __PROTOTYPE_5_0         /* Get stdarg prototypes for logMsg */
#include <vxWorks.h>
#include <cacheLib.h>
#include <taskLib.h>
#include <bootLib.h>
#include <intLib.h>
#include <netLib.h>
#include <logLib.h>
#include <sysLib.h>
#include <usrLib.h>
#include <vmLib.h>
#include <vxLib.h>
#include <tyLib.h>
#include <shellLib.h>
#include <selectLib.h>
#include <dirent.h>
#include "config.h"                     /* For BOOT_LINE_ADRS */

#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/appl/vxworks/hal.h>

#ifdef SAL_NO_FLASH_FS
#else
#include "flashFsLib.h"
#endif

#include "netio.h"

extern int osapiFsOpen (char * filename);
extern int osapiFsDeleteFile(char * filename);
extern int osapiFsDir (char * pCB, unsigned int max_bytes);

char    defaultdir_path[256];           /* Default (home) directory */
char    cwd_path[256];                  /* Current Working Directory */

/* This variable is managed from the diag shell */
int     _diag_nfs_mounts;

/*
 * sal_flash_sync
 *
 * Sync routine to call after each file operation to prevent loss of
 * data.  This is extremely important on the primitive Mousse flashFs.
 */

int
sal_flash_sync(void)
{
    return 0;
}

int
sal_expand_file(char *file, char *fname)
{
    printk("sal_expand_file called byt not implemented\n");

    return(0);
}

/*
 * Set default directory (for cd with no argument)
 */
int
sal_homedir_set(char *dir)
{
    strncpy(defaultdir_path, ".", sizeof(defaultdir_path));

    return 0;
}

char *
sal_homedir_get(char *buf, size_t size)
{
    strncpy(buf, defaultdir_path, size);
    buf[size - 2] = 0;

    if (buf[strlen(buf) - 1] != '/') {
        strcat(buf, "/");
    }

    return buf;
}

/*
 * Get current working directory.
 */
char *sal_getcwd(char *buf, size_t size)
{
    return(strncpy(buf, cwd_path, size));
}

/*
 * Set current working directory, it is stupid in that is just sets
 * a prefix for open/close/ and friends.
 *
 * If dir is NULL, changes to default (home) directory.
 */
int sal_cd(char *dir)
{
    strncpy(cwd_path,".",sizeof(cwd_path));

    return(0);
}

/*
 * Do an "ls"
 */
int sal_ls(char *f, char *flags)
{
  char buf[256 * 32];
  char * pbuf, * ptok;

  pbuf = buf;

  /* Get the directory listing */
  if ( osapiFsDir(pbuf, (unsigned int)sizeof(buf)) != 0 )
  {
    return -1;
  }

  ptok = strtok(pbuf, "\n");     /* Toss the '.' and '..' entries */
  ptok = strtok((char *)'\0', "\n");

  do
  {
    ptok = strtok((char *)'\0', "\n");    /* Pull the filenames, one at a time */
    if ( ptok != NULL )
    {
      /* check file type*/
      if ((strstr(ptok, ".soc")) != NULL )
      {
        printk("%s\n",ptok);
      }
    }
  } while ( ptok != NULL);

  return 0;
}

int
sal_open(char *file, int oflag, ...)
/*
 * Function:    sal_open
 * Purpose:     Open a file.
 * Parameters:  name - name of file to open, look specifically for
 *                      a colon in the name for netio format
 *              oflag - open mode (O_RDONLY, etc).
 * Returns:     File descriptor, or -1 on error
 * Notes:       Not all file modes are supported for netio.
 */
{
  int mode;
  va_list ap;

  va_start(ap, oflag);
  mode = va_arg(ap, int);
  va_end(ap);

  return(open(file, oflag, mode));
}

int
sal_close(int fd)
/*
 * Function:    sal_close
 * Purpose:     Close a file opened with sal_open
 * Parameters:  fd - File descriptor
 * Returns:     non-zero on error
 */
{
    int         rv;

        rv = close(fd);
        sal_flash_sync();

    return rv;
}


FILE *
sal_fopen(char *file, char *mode)
/*
 * Function:    sal_fopen
 * Purpose:     "fopen" a file.
 * Parameters:  name - name of file to open, look specifically for
 *                      "flash:" for netio format.
 *              mode - file mode.
 * Returns:     NULL or FILE * pointer.
 */
{
    char *fname = file;
    char *s;
    
    if ((s = strchr(file,':')) != NULL)
      return NULL;

    if ((s = strchr(file,'/')) != NULL)
        fname = s + 1;

    printk("Opening %s\n",fname);

    if (fname != NULL)
        return(fopen(fname, mode));
    else
      return NULL;
}

extern int osapiFsClose (int filedesc);

int
sal_fclose(FILE *fp)
/*
 * Function:    sal_fclose
 * Purpose:     Close a file opened with sal_fopen
 * Parameters:  fp - FILE pointer.
 * Returns:     non-zero on error
 */
{
  /*  (void)osapiFsClose((int)fp); */
    (void)fclose(fp);

        sal_flash_sync();

    return 0;
}

int
sal_remove(char *file)
/*
 * Function:    sal_remove
 * Purpose:     Remove a file from a file system.
 * Parameters:  file - name of file to remove.
 * Returns:     0 - OK
 *              -1 - failed.
 */
{
    char *fname = file;
    char *s;
    int rc = 0;
    
    if ((s = strchr(file,':')) != NULL)
      return -1;

    if ((s = strchr(file,'/')) != NULL)
        fname = s + 1;

    printk("Deleteing %s\n",fname);
    if (fname != NULL)
      rc = osapiFsDeleteFile (fname);
    
    if (rc > 0) rc = -1;

    return rc;
}

int
sal_rename(char *file_old, char *file_new)
/*
 * Function:    sal_rename
 * Purpose:     Rename a file on a file system.
 * Parameters:  file_old - name of existing file to rename.
 *              file_new - new name of file.
 * Returns:     0 - OK
 *              -1 - failed.
 */
{
                return -1;
}

int
sal_mkdir(char *path)
{
  return -1;
}

int
sal_rmdir(char *path)
{
    return -1;
}

SAL_DIR *
/*
 * Function:    sal_opendir
 * Purpose:     Open a directory
 * Parameters:  name - name of directory to open
 * Returns:     NULL or SAL_DIR pointer.
 * Notes:       SAL_DIR pointer can be passed to sal_readdir, etc.
 *              sal_closedir should be used to free resources.
 */
sal_opendir(char *dirName)
{
    return (SAL_DIR *) NULL;
}

int
/*
 * Function:    sal_closedir
 * Purpose:     Close a directory
 * Parameters:  dirp - A valid SAL_DIR pointer returned from sal_opendir
 * Returns:     Non-zero on failure
 */
sal_closedir(SAL_DIR *dirp)
{
    return -1;
}

struct sal_dirent *
/*
 * Function:    sal_readdir
 * Purpose:     Return next directory entry
 * Parameters:  dirp - A valid SAL_DIR pointer returned from sal_opendir
 * Returns:     NULL or SAL_DIR pointer.
 */
sal_readdir(SAL_DIR *dirp)
{
            return NULL;
        }

void
/*
 * Function:    sal_rewinddir
 * Purpose:     Start reading directory from beginning again
 * Parameters:  dirp - A valid SAL_DIR pointer returned from sal_opendir
 * Returns:     Nothing
 */
sal_rewinddir(SAL_DIR *dirp)
{
  return;
}
