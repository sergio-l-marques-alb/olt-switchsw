/*
 * $Id: netio.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
 * $Copyright: Copyright 2008 Broadcom Corporation.
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
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * TFTP Module
 *
 * Abstracts eCos TFTP modules to look like ordinary file I/O.
 *
 * Permissible filename syntaxes are:
 *
 *	tftp@host:file
 *
 */

#include <pkgconf/kernel.h>
#include <pkgconf/io_fileio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#include <sal/appl/io.h>
#include <sal/core/alloc.h>

#include <network.h>
#include <arpa/inet.h>
#include <tftp_support.h>
#include <pkgconf/fs_ram.h>

#include "netio.h"

#include <cyg/infra/diag.h> 

/*
 * Structure to keep track of transfers in progress
 */

#define MAX_NETIO_OPEN	5

typedef struct NETIO_FILE {
    int		entry_used;
    int		method;
#define	METHOD_TFTP	1
    FILE	*data_fp;
    int		data_fd;
    char filename[32]; /* temp file name after TFTP get */
} NETIO_FILE;

static NETIO_FILE netio_iob[MAX_NETIO_OPEN];

/*
 * netio_defaults
 *
 *   Routine to set default user, password, and host.  This routine is
 *   optional.  If used, allows any of these three fields to be omitted
 *   from file specifications.
 */

char *netio_dfl_user = NULL;
char *netio_dfl_pass = "";
char *netio_dfl_host = NULL;

void netio_defaults(char *user, char *pass, char *host)
{
    if (user)
	       netio_dfl_user = strcpy(malloc(strlen(user) + 1), user);
    if (pass)
        netio_dfl_pass = strcpy(malloc(strlen(pass) + 1), pass);
    if (host)
        netio_dfl_host = strcpy(malloc(strlen(host) + 1), host);
}

/*
 * netio_split
 *
 *   Utility routine to break a file name into directory and file
 *   portions.  Works well on weird paths.  Modifies input string.
 */

void netio_split(char *path, char **dir, char **file)
{
    char		*last_slash;

    if ((last_slash = strrchr(path, '/')) == 0) {
        *dir = ".";
        *file = path;
    } else if (last_slash == path) {
        *dir = "/";
        *file = (path[1] ? &path[1] : ".");
    } else {
        *last_slash = 0;
        *dir = path;
        *file = (last_slash[1] ? &last_slash[1] : ".");
    }
}

/*
 * netio_breakline
 *
 *   Utility routine to parse filename syntax and supply defaults,
 *   if any.
 */

int netio_breakline(char *line,
		    char **host,
		    char **user,
		    char **file,
		    char **pass,
		    int *methodp)
{
    char		*colon;
    
    if (line == NULL) {
        printf("netio: Null filename\n");
        errno = ENOENT;
        return -1;
    }
    /*
     * Syntaxes supported (repeated from comment at top of file):
     *
     *	host:file
     */

    *user = netio_dfl_user;
    *pass = netio_dfl_pass;
    *host = netio_dfl_host;

    if ((colon = strchr(line, ':')) != 0)
        *colon++ = 0;

    if ( colon) {
        *host = line;
        *file = colon;
    } else {
        printf("netio: Filename syntax error (host:file)\n");
        errno = ENOENT;
        return -1;
    }

    if (!*host || !*file) {
        printf("netio: Insufficient filename spec "
	       "(user[%%pass]@host:file)\n");
        errno = ENOENT;
        return -1;
    }

    if (**file == 0)
        *file = ".";

#ifdef BROADCOM_DEBUG
    diag_printf("netio_breakline:\n");
    diag_printf("  File:   %s\n", *file);
    diag_printf("  Host:   %s\n", *host);
#endif
    return 0;
}
#define TFTP_BUFFER_SIZE 128*1024
/*
 * netio_open
 *
 *   This routine is functionally similar to open.  The resulting file
 *   pointer can be used with the read and write routines.  It must be
 *   closed with netio_close, not close.  It is not legal to seek.
 */

int netio_open(char *filespec, int oflag, ...)
{
    char		*pass, *host, *file, *user;
    NETIO_FILE		*ff;
    int			i;
    char tempname[32];

#ifdef BROADCOM_DEBUG
    diag_printf("netio_open: spec=%s oflag=%d\n", filespec, oflag);
#endif

    /* if ((oflag & 3) != O_RDONLY && (oflag & 3) != O_WRONLY) { */
    if ((oflag & 3) != O_RDONLY) {
        errno = EINVAL;
        return -1;		/* Not supported */
    }

    /*
     * Find a free slot in the table of outstanding transfers
     */

    for (i = 0; i < MAX_NETIO_OPEN; i++)
	       if (! netio_iob[i].entry_used)
	          break;

    if (i == MAX_NETIO_OPEN) {		/* Too many outstanding */
	       errno = EMFILE;
	       return -1;
    }

    ff = &netio_iob[i];
    ff->method = METHOD_TFTP; /* to make compiler happy */

    if (netio_breakline(filespec, &host, &user, &file, &pass, &ff->method) < 0)
        return -1;

    /*
     * If password is non-empty, use ftpLib to open a connection for
     * reading, writing, or appending to the file.  Otherwise, use
     * remLib.
     */

    switch (ff->method) {
        case METHOD_TFTP:
            {
	           	   int fd;
                ssize_t size, wrote;
                int res, err;
                struct sockaddr_in shost;
                char *c, *gbuf;

	               memset((char *)&shost, 0, sizeof(shost));
                shost.sin_len = sizeof(shost);
                shost.sin_family = AF_INET;
                if (inet_aton(host,&shost.sin_addr) == 0) {
                    diag_printf("invliad IP : %s\n", host);
                    return -1;
                }
                shost.sin_port = 0;
                gbuf = (char *)sal_alloc(TFTP_BUFFER_SIZE, "SAL_NETIO");
                if (gbuf == NULL) 
                    return -1;
                #ifdef BROADCOM_DEBUG
                diag_printf("Trying tftp_get %s %16s...\n", 
                             file, inet_ntoa(shost.sin_addr));
                #endif
                res = tftp_get( file, &shost, 
                                gbuf, TFTP_BUFFER_SIZE, TFTP_OCTET, &err);
                #ifdef BROADCOM_DEBUG
                diag_printf("res = %d, err = %d\n", res, err);
                #endif
                if (err != 0) {
                    sal_free(gbuf);
                    return -1;
                }
                if (NULL != (c = strrchr(file, '/'))) {
                    strcpy(tempname, ++c);
                }
	               else {
	                   strcpy(tempname, file);
	               }
	               strcat(tempname, ".temp");
	               fd = open( tempname, O_WRONLY|O_CREAT );
                if( fd < 0 ) {
                	    diag_printf("can't create file %s\n", tempname);
                	    sal_free(gbuf);
                	    return -1;
                }
    
                size = res;
                while( size > 0 )
                {
                    ssize_t len = size;
                    if ( len > TFTP_BUFFER_SIZE ) len = TFTP_BUFFER_SIZE;
        
                    wrote = write( fd, gbuf, len );
                    if( wrote != len ) { diag_printf("wrote != len\n"); }
                    size -= wrote;
                }
                err = close( fd );
                sal_free(gbuf);
                if( err < 0 )  { diag_printf("close(fd) failed\n"); }
	           }
	           break;
	       default:
	           return -1;
	   }

    ff->data_fd = open( tempname, O_RDONLY );
    if (ff->data_fd < 0) { diag_printf("can't create file\n"); return -1; }
    ff->entry_used = 1;
    strcpy(ff->filename, tempname);
#ifdef BROADCOM_DEBUG
    diag_printf("netio_open: fd=%d, filename = %s\n", ff->data_fd, tempname);
#endif   
    return ff->data_fd;
}

/*
 * netio_close
 *
 *   Closes a file handle returned from netio_open.  Do not use close
 *   directly.
 */

int netio_close(int fd)
{
    NETIO_FILE		*ff;
    int			i, rc = 0;

    /* Search for entry in table of outstanding transfers */

    for (i = 0; i < MAX_NETIO_OPEN; i++)
	       if (netio_iob[i].data_fd == fd)
	           break;

    if (i == MAX_NETIO_OPEN) {
        errno = EBADF;
	       return -1;
    }

    ff = &netio_iob[i];

    /* Data connection must be closed in order to receive FTP reply */

    if (close(ff->data_fd) < 0)
	       rc = -1;
    unlink(ff->filename);
    
    ff->entry_used = 0;

    errno = EIO;	/* In case rc < 0 */

    return rc;
}

/*
 * netio_valid_fd
 *
 *  Convenient utility routine allowing an application to determine
 *  whether or not a file descriptor was opened using netio_open.
 */

int netio_valid_fd(int fd)
{
    int			i;

    for (i = 0; i < MAX_NETIO_OPEN; i++)
	     if (netio_iob[i].entry_used && netio_iob[i].data_fd == fd)
	    return 1;

    return 0;
}

/*
 * netio_fopen
 *
 *   This routine is functionally similar to fopen.  The resulting file
 *   pointer can be used with stdio read and write routines.  It must be
 *   closed with netio_fclose, not fclose.  It is not legal to fseek.
 */

FILE *netio_fopen(char *filespec, char *fmode)
{
    NETIO_FILE		*ff;
    int			i, fd;
    int			oflag = 0;

    switch (fmode[0]) {
       case 'r':
	         oflag = O_RDONLY;
	         break;
      default:
	         return NULL;
    }

    if ((fd = netio_open(filespec, oflag)) < 0)
	         return NULL;

    /* Search for entry in table of outstanding transfers */

    for (i = 0; i < MAX_NETIO_OPEN; i++)
	       if (netio_iob[i].data_fd == fd)
	           break;

    assert (i < MAX_NETIO_OPEN);	/* Must be found */

    ff = &netio_iob[i];

    if ((ff->data_fp = fdopen(ff->data_fd, fmode)) == NULL) {
	       perror("fdopen");
	       netio_close(ff->data_fd);
	       return NULL;
    }

    return ff->data_fp;
}

/*
 * netio_fclose
 *
 *   Closes a file pointer returned from netio_fopen.  Do not use fclose
 *   directly.
 */

int netio_fclose(FILE *fp)
{
    NETIO_FILE		*ff;
    int			i, rc = 0;

    /* Search for entry in table of outstanding transfers */

    for (i = 0; i < MAX_NETIO_OPEN; i++)
         if (netio_iob[i].data_fp == fp)
	           break;

    if (i == MAX_NETIO_OPEN) {
	      errno = EBADF;
	      return -1;
    }

    ff = &netio_iob[i];

    /*
     * Flush before closing file descriptor
     */

    fflush(ff->data_fp);

    /*
     * fclose will try to close ff->data_fd.
     */

    if (fclose(ff->data_fp) < 0) {
       diag_printf("fclose(ff->data_fp) failed\n");
	      rc = -1;
    }
    ff->data_fp = NULL;
    ff->entry_used = 0;
    unlink(ff->filename);
    
    return rc;
}

/*
 * netio_valid_fp
 *
 *  Convenient utility routine allowing an application to determine
 *  whether or not a FILE * was opened using netio_fopen.
 */

int netio_valid_fp(FILE *fp)
{
    int			i;

    for (i = 0; i < MAX_NETIO_OPEN; i++)
	      if (netio_iob[i].data_fp == fp)
	           return 1;

    return 0;
}

/*
 * netio_ls
 *
 *  Get a file listing and display output on stdout.
 */

int netio_ls(char *path, char *flags)
{
    printf("netio_ls: cannot list files on TFTP connections\n");
	   errno = EIO;
	   return(-1);
}

/*
 * netio_remove
 *
 *   Delete a file.
 */

int
netio_remove(char *path)
{
    printf("netio_remove: cannot remove files on TFTP connections\n");
	   errno = EIO;
	   return(-1);
}

/*
 * netio_rename
 *
 *   Rename a file.  The old_path is a usual netio filename spec, but
 *   the destination name must be a plain path without %, @, or :.
 */

int netio_rename(char *old_path, char *new_path)
{
    printf("netio_rename: cannot rename files on TFTP connections\n");
	   errno = EIO;
	   return(-1);
}

/*
 * netio_mkdir
 *
 *   Make a directory.
 */

int netio_mkdir(char *path)
{
    printf("netio_mkdir: cannot make directories on TFTP connections\n");
	   errno = EIO;
	   return(-1);
}

/*
 * netio_rmdir
 *
 *   Remove a directory.
 */

int netio_rmdir(char *path)
{
    printf("netio_rmdir: cannot remove directories on TFTP connections\n");
	   errno = EIO;
	   return(-1);
}

/*
 * netio_opendir
 *
 *   This routine is functionally similar to opendir.  The resulting DIR
 *   pointer can be used with netio_readdir and netio_closedir.
 *
 *   This routine transfers the entire directory and buffers it for call
 *   to readdir.
 */
NETIO_DIR *
netio_opendir(char *name)
{
    printf("netio_opendir: cannot open directories on TFTP connections\n");
	   errno = EIO;
	   return NULL;
}

/*
 * netio_readdir
 *
 *   This routine is functionally similar to readdir.  It returns an
 *   equivalent type "netio_dirent" that has a d_name field.
 */
struct netio_dirent *
netio_readdir(NETIO_DIR *dirp)
{
    static struct netio_dirent dir;

    if (dirp->names_ptr < dirp->names_used) {
        strcpy(dir.d_name, dirp->names[dirp->names_ptr++]);
        return &dir;
    }

    return NULL;
}

/*
 * netio_closedir
 *
 *   This routine is functionally similar to closedir.
 */
int
netio_closedir(NETIO_DIR *dirp)
{
    if (dirp != NULL) {
       if (dirp->names != NULL) {
	         while (dirp->names_used > 0) {
		            dirp->names_used--;
		            free(dirp->names[dirp->names_used]);
	         }
	         free(dirp->names);
	      }
       free(dirp);
    }

    return 0;
}

/*
 * netio_rewinddir
 *
 *   This routine is functionally similar to rewinddir.
 */
void
netio_rewinddir(NETIO_DIR *dirp)
{
    dirp->names_ptr = 0;
}
