/*
 * $Id: fileio.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * File:        fileio.c
 * Purpose:     File I/O
 */

#include <stdio.h> /* for printf */
#include <stdlib.h> /* for malloc */
#include <sal/appl/io.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h> /* opendir */
#include <pkgconf/io_fileio.h>
#include <cyg/infra/diag.h>


#include "netio.h"

char    defaultdir_path[256];           /* Default (home) directory */
char    cwd_path[256];                  /* Current Working Directory */

static void listdir( char *name, int statp )
{
    int err;
    DIR *dirp;
    
    dirp = opendir( name );
    if( dirp == NULL ) {
        diag_printf("opendir failed\n");
        return;
    }

    for(;;)
    {
        struct dirent *entry = readdir( dirp );
        
        if( entry == NULL )
            break;
        
        printf("%14s",entry->d_name);
        if( statp )
        {
            char fullname[PATH_MAX];
            struct stat sbuf;

            if( name[0] )
            {
                strcpy(fullname, name );
                if( !(name[0] == '/' && name[1] == 0 ) )
                    strcat(fullname, "/" );
            }
            else fullname[0] = 0;
            
            strcat(fullname, entry->d_name );
            
            err = stat( fullname, &sbuf );
            if( err < 0 )
            {
                if( errno == ENOSYS )
                    printf(" <no status available>");
            }
            else
            {
                printf(" [mode %08x ino %08x nlink %d size %d]",
                            sbuf.st_mode,sbuf.st_ino,sbuf.st_nlink,(int)sbuf.st_size);
            }
        }

        printf("\n");
    }
    err = closedir( dirp );
}


/*
 * sal_flash_sync
 *
 * Sync routine to call after each file operation to prevent loss of
 * data.  This is extremely important on the primitive Mousse flashFs.
 */

int
sal_flash_sync(void)
{
#if !defined(LVL7_FIXUP)
    if (!SAL_IS_PLATFORM_INFO_VALID) {
        printk("Platform not attached\n");
        return -1;
    }
    if (platform_info->f_fs_sync) {
        return platform_info->f_fs_sync();
    }
    return (0);
#else
    return 0;
#endif

}

/*
 * sal_normalize_file
 *
 * Canonicalize a filename, simplifying occurrences of / and . and ..
 *
 * This procedure is a little complicated and has been regressed through
 * a large set of test cases.
 */

static void
sal_normalize_file(char *path)
{
    char        *s, *d;
    int         dot, dotdot;

    /*
     * Ignore portion of path before ':', if any.
     * Handles 'flash:' as well as netio files.
     */

    if ((s = strchr(path, ':')) != NULL)
        path = s + 1;

    /* Suck path elements, performing simulated chdirs on path forming at d */

    s = d = path;

    /* If absolute path, copy leading / */

    if (*s == '/')
        *d++ = *s++;

    while (*s != 0) {
        dot    = (s[0] == '.' && (s[1] == 0 || s[1] == '/'));
        dotdot = (s[0] == '.' && s[1] == '.' && (s[2] == 0 || s[2] == '/'));

        if (*s == '/' || dot) {
            s++;                /* Null path element, or chdir to . */
        } else if (dotdot && d == path) {
            *d++ = '.';         /* Retain .. at beginning of path */
            *d++ = '.';
            s += 2;
        } else if (dotdot && d == path + 1 && path[0] == '/') {
            /* Ignore .. if at top of absolute path */
            s += 2;
        } else if (dotdot && (d <= path + 1 || d[-1] != '.' || d[-2] != '.')) {
            /* Discard previous path element, only if it is not .. */
            while (--d > path && *d != '/')
                ;
            if (d == path && path[0] == '/')
                d++;    /* Beginning of absolute path keeps / */
            *d = 0;     /* Truncate rest of path element */
            s += 2;
        } else {
            /* Copy plain path element, adding / if not the first */
            if (d > path && ! (d == path + 1 && path[0] == '/'))
                *d++ = '/';
            while (s[0] != '/' && s[0] != 0)
                *d++ = *s++;
        }
    }

    *d = 0;
}

#define SAL_NETIO_FILE(_s)      (0 != strchr((_s), ':'))

int
sal_expand_file(char *file, char *fname)
{
    char        *c;

    if (*file == '/') {         /* ABS in file system */
        strcpy(fname, file);    /* (allows /null, etc. to work) */
    } else if (NULL != (c = strchr(file, ':'))) {
        strcpy(fname, "/");
        strcat(fname, ++c);       /* file name */
    } else {                    /* Relative in file system */
        strcpy(fname, cwd_path);
        if (cwd_path[1] != 0) /* not root directory */
            strcat(fname, "/");
        strcat(fname, file);
    }
    return(0);
}

/*
 * Set default directory (for cd with no argument)
 */
int
sal_homedir_set(char *dir)
{

    char                *buf;

    buf = defaultdir_path;

    if (dir != NULL) {
        strcpy(buf, dir);
    } else {
        /* default romfs mount point */
        strcpy(buf, "/");
    }

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
    char    buf[256];
    char    *c;
    int     err;

    if (dir == NULL) {
        dir = sal_homedir_get(buf, sizeof (buf));
    }
    
    /* Be sure string ends in "/" */
    
    if (cwd_path[0] == 0) {
        strcpy(cwd_path, "/");
    } else {
        c = cwd_path + strlen(cwd_path) - 1; /* Move to end */
        if (*c != '/') {
            strcat(cwd_path, "/");
        }
    }  
    err = chdir(dir);
    if (err < 0) {
    	   diag_printf("chdir(%s) = %d\n", dir, err);
        return (err);
    }
    else {
    	   if (dir[0] == '/') { /* absolute path */
	           strcpy(cwd_path, dir);
    	   } else {
    	      strcat(cwd_path, dir);
           strcat(cwd_path, "/");
           sal_normalize_file(cwd_path);
        }
        return 0;
    }
}

/*
 * Do an "ls"
 */
int sal_ls(char *f, char *flags)
{
    char fullname[256];
    struct stat sbuf;
    int long_fmt = 0;
    int rv = 0;
    
    if (sal_expand_file(f, fullname)) {
        printf("Failed to expand file name: \"%s\"\n", f);
        return -1;
    }
    
    long_fmt = (flags != NULL && strchr(flags, 'l') != NULL);
    
    /* list directory */
    if (f[0] == '.' && f[1] == 0) {
        listdir(cwd_path, long_fmt);
        return 0;
    }
    
    rv = stat( fullname, &sbuf );
    if( rv < 0 )
    {
        printf("ls: Listing failed.\n");
    }
    else
    {
        printf("%14s", f);
        if (long_fmt) 
            printf(" [mode %08x ino %08x nlink %d size %d]",
                     sbuf.st_mode,sbuf.st_ino,sbuf.st_nlink,(int)sbuf.st_size);
        printf("\n");
        
    }
    return rv;
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
    char fname[256];
    int mode;
    va_list ap;

    va_start(ap, oflag);
    mode = va_arg(ap, int);
    va_end(ap);

    if (sal_expand_file(file, fname)) {
        printf("Error: Cannot expand file name: %s\n", file);
        return(-1);
    } else if (SAL_NETIO_FILE(file)) {
        return(netio_open(file, oflag, mode));
    } else if (strcmp(file, "flash:") == 0) {
    	   printf("Don't support this format:%s\n", file);
        return(-1); /* Disaster if no filename --uses raw partition! */
    } else {
        return(open(fname, oflag, mode));
    }
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
    int rv;
    if (netio_valid_fd(fd)) {
        rv = netio_close(fd);
    } 
    else
    {
        rv = close(fd);
        sal_flash_sync();
    }
    return rv;
}

static int config_at_startup = 0;

void sal_config_at_startup(int mode)
{
    config_at_startup = mode;
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

    char fname[256];

    if (sal_expand_file(file, fname)) {
        printf("Error: Cannot expand file name: %s\n", file);
        return(NULL);
    } else if (SAL_NETIO_FILE(file)) {
        return(netio_fopen(file, mode));
    } else if (strcmp(file, "flash:") == 0) {
        return NULL;    /* Disaster if no filename --uses raw partition! */
    } else {
        return(fopen(fname, mode));
    }
    return NULL;
}

int
sal_fclose(FILE *fp)
/*
 * Function:    sal_fclose
 * Purpose:     Close a file opened with sal_fopen
 * Parameters:  fp - FILE pointer.
 * Returns:     non-zero on error
 */
{
    int         rv;

    if (netio_valid_fp(fp)) {
        rv = netio_fclose(fp);
    } 
    else
    {
        rv = fclose(fp);
        if (!config_at_startup)
            sal_flash_sync();
    }

    return rv;
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
    char        fname[256];
    int         rv;

    if (sal_expand_file(file, fname)) {
        printf("Error: Cannot expand file name\n");
        return(-1);
    } else if (SAL_NETIO_FILE(fname)) {
        return(netio_remove(fname));
    } else {
        rv = unlink(fname);
        sal_flash_sync();
        return rv;
    }
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
    char        fname_old[256];
    char        fname_new[256];
    int         rv;

    if (sal_expand_file(file_old, fname_old)) {
        printf("Error: Cannot expand file name\n");
        return(-1);
    } else if (SAL_NETIO_FILE(fname_old)) {
        if (SAL_NETIO_FILE(file_new)) {
            printf("Error: Destination must be plain pathname\n");
            return -1;  /* File name only (no path) allowed for netio */
        }
        return(netio_rename(fname_old, file_new));
    } else {
        if (SAL_NETIO_FILE(file_new)) 
            return -1;
        if (strrchr(file_new, ':'))
            strcpy(fname_new, file_new);
        else {
            if (!sal_getcwd(fname_new, sizeof(fname_new)))
                return -1;
            if (fname_new[1] != 0) /* not root directory */
                strcat(fname_new, "/");
            strcpy(&fname_new[strlen(fname_new)], file_new);
        }
        rv = rename(fname_old, fname_new);
        sal_flash_sync();
        return rv;
    }
}

int
sal_mkdir(char *path)
{
    char        path_full[256];
    int         rv;

    if (sal_expand_file(path, path_full)) {
        printf("Error: Cannot expand file name\n");
        return(-1);
    } else if (SAL_NETIO_FILE(path_full)) {
        return netio_mkdir(path_full);
    } else {
    	   rv = mkdir(path_full, 0);
        sal_flash_sync();
        return rv;
    }
}

int
sal_rmdir(char *path)
{
    char        path_full[256];
    int         rv;

    if (sal_expand_file(path, path_full)) {
        printf("Error: Cannot expand file name\n");
        return(-1);
    } else if (SAL_NETIO_FILE(path_full)) {
        return netio_rmdir(path_full);
    } else {
        rv = rmdir(path_full);
        sal_flash_sync();
        return rv;
    }
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
    char fname[SAL_NAME_MAX];
    SAL_DIR *dirp;

    if (dirName == NULL || dirName[0] == 0) {
        dirName = ".";
    }

    if (sal_expand_file(dirName, fname)) {
        printf("Error: Cannot expand directory name: %s\n", dirName);
        return(NULL);
    }

    if ((dirp = malloc(sizeof (*dirp))) == 0) {
        return NULL;
    }

    if (SAL_NETIO_FILE(fname)) {
        dirp->is_netio = 1;
        dirp->dirp = netio_opendir(fname);
    } else {
        dirp->is_netio = 0;
        dirp->dirp = (void *)opendir(fname);
    }

    if (dirp->dirp == NULL) {
        free(dirp);
        dirp = NULL;
    }

    return (SAL_DIR *) dirp;
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
    int    rv = 0;

    if (dirp->is_netio) {
        rv = netio_closedir(dirp->dirp);
    } else {
        rv = closedir(dirp->dirp);
    }

    return rv;
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
    struct dirent *unix_de;
    struct netio_dirent *netio_de;
    char *s;

    if (dirp->is_netio) {
        if ((netio_de = netio_readdir(dirp->dirp)) == NULL) {
            return NULL;
        }
        s = netio_de->d_name;
    } else {
        if ((unix_de = readdir((DIR *)(dirp->dirp))) == NULL) {
            return NULL;
        }
        s = unix_de->d_name;
    }

    strncpy(dirp->de.d_name, s, sizeof (dirp->de.d_name));

    dirp->de.d_name[sizeof (dirp->de.d_name) - 1] = 0;

    return &dirp->de;
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

    if (dirp->is_netio) {
        netio_rewinddir(dirp->dirp);
    } else {
        rewinddir(dirp->dirp);
    }

}

