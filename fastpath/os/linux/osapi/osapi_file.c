/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_file.c
*
* @purpose   OS independent API's
*
* @component osapi
*
* @comments
*
* @create    11/20/2001
*
* @author    John Linville
*
* @end
*
*********************************************************************/

  #include <stdio.h>
  #include <stdlib.h>
  #include <sys/types.h>
  #include <sys/stat.h>
#include <sys/wait.h>
  #include <string.h>
  #include <unistd.h>
  #include <dirent.h>
  #include <fcntl.h>
  #include <pthread.h>
  #include <errno.h>
  #include <sys/statfs.h>

#include "osapi_file.h"

static pthread_mutex_t osapi_readdir_lock = PTHREAD_MUTEX_INITIALIZER;

static struct dirent *osapi_readdir(DIR *dir, char *buf)
{
  struct dirent *entry;

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&osapi_readdir_lock);
  pthread_mutex_lock(&osapi_readdir_lock);

  if ((entry = readdir(dir)) != NULL)
  {
    strncpy(buf, entry->d_name, NAME_MAX);

    /* buf must be allocated as buf[NAME_MAX+1] */
    buf[NAME_MAX] = '\0';
  }

  pthread_cleanup_pop(1);

  return(entry);
}

/**************************************************************************
*
* @purpose  Init and create a RAM file system.
*
* @param    path @b{(input)} ptr to a null terminated string containing
*                            path to ramdisk
* @param    name@b{(input)} ptr to a null terminated string containing
*                            name of ramdisk
* @param    rblks@b{(input)} requested number of blocks
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments Use this function with CAUTION as it is destructive!
* @comments It first unmounts and deletes the previous ramdisk (if any).
* @comments The function proceeds as;
* @comments - Attempt to un-mount previous/unwanted mount
* @comments - Attempt to delete previous/unwanted directory
* @comments - Create and format file storage.
* @comments - Create a ext2/3 filesystem
* @comments - Make new directory
* @comments - Mount the new RAM file system
*
* @notes - mount uses "-o loop" for loop back device
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiFsRamCreate(L7_char8 *path, L7_char8 *name, L7_uint32 rblks)
{
  L7_int32 std_block_size = 1024;
  L7_int32 blocks = 0;
  L7_char8 cmd[128];

  if (rblks > std_block_size)
  {
    blocks = rblks / std_block_size;

    /* Cleanup */
    snprintf(cmd,sizeof(cmd),"umount %s%s >/dev/null 2>&1",path,name);
    if(0 > system(cmd)){}
    snprintf(cmd,sizeof(cmd),"mkdir %s%s >/dev/null 2>&1",path,name);
    if(0 > system(cmd)){}

    snprintf(cmd,sizeof(cmd),
      "mount -t tmpfs -o,rw,size=%d tmpfs %s%s >/dev/null 2>&1",blocks*std_block_size,path,name);
    if (WEXITSTATUS(system(cmd)) != 0)
    {
      return L7_FAILURE;
    }
  }
  return(L7_SUCCESS);
}

/***************************************************************************
*
* @purpose Obtain the directory information from a volume's directory
*
* @param  pCB  where to put listing
* @param  max_bytes  pCB size
*
* @notes  If successful, the output of @b{osapiFsDir()} will be a NULL terminated string
* @notes  Subdirectories always contain two special entries. The “.” entry refers to
*         the subdirectory itself, while the “..” entry refers to the subdirectory’s parent
*         directory. The root directory does not contain these special entries.
*         An example output will look like this...
*
* @table{
* @row{@cell{@c{ ./ }}        @cell{@c{ subdir }}}
* @row{@cell{@c{ ../ }}       @cell{@c{ subdir }}}
* @row{@cell{@c{ subdir1/ }}  @cell{@c{ subdir }}}
* @row{@cell{@c{ subdir2/ }}  @cell{@c{ subdir }}}
* @row{@cell{@c{ testfile }}  @cell{@c{ file   }}}}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @end
*
***************************************************************************/
EXT_API L7_RC_t osapiFsDir ( L7_char8 *pCB, L7_uint32 max_bytes )
{
  DIR *dir;
  struct stat sbuf;
  char buf[NAME_MAX+1];
  L7_uint32 i, directory_size = 0, count=0;
  L7_char8 s[512];
  L7_char8 *p_s;

  if ((dir = opendir(".")) == NULL)
  {
    return(L7_FAILURE);
  }

  while (osapi_readdir(dir, buf) != NULL)
  {
    p_s = &s[0];
    memset((void *)p_s, 0x00, sizeof(s));
    p_s += (snprintf(p_s, L7_MAX_FILENAME, "%s", buf));

    stat(buf, &sbuf);
    if (S_ISDIR(sbuf.st_mode))
    {
      p_s += sprintf (p_s, "\n");
    }
    else
    {
      /* For files only, display the size */
      i = 0;
      if (strlen (buf) < L7_MAX_FILENAME)
      {
        i = (L7_MAX_FILENAME+2)-strlen(buf);
        memset(p_s,' ',i);
      }

      p_s += i;
      p_s += sprintf (p_s, "%lu\n",sbuf.st_size);
      directory_size += sbuf.st_size;
    }
    if ((count + strlen(s)) >= max_bytes)
    {
      /* terminate the directory string and return */
      *pCB = '\0';
      closedir(dir);
      return(L7_FAILURE);
    }
    pCB += sprintf(pCB,"%s",s);
    count += strlen(s);
  }

  p_s = &s[0];
  memset(s, 0x00, sizeof(s));
  p_s += sprintf (p_s, "\nTotal size %d\n",directory_size);

  if ((count + strlen(s)) >= max_bytes)
  {
    /* terminate the directory string and return */
    *pCB = '\0';
    closedir(dir);
    return(L7_FAILURE);
  }
  pCB += sprintf(pCB,"%s",s);
  *pCB = '\0';

  if (closedir(dir) != 0)
  {
    return(L7_FAILURE);
  }

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Create a file in the current subdirectory of the filesystem
*
* @param    filename @b{(input)} ptr to a null terminated string
* @param    filedesc @b{(output)} location to store the file descriptor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if the file is not created
*
* @comments This function creates the file, but does not close the file.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiFsFileCreate(L7_char8 *filename, L7_int32 *filedesc)
{
  if ((*filedesc = creat(filename, 0644)) == -1)
  {
    return(L7_FAILURE);
  }

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Create a file in the current subdirectory of the filesystem
*
* @param    filename @b{(input)} ptr to a null terminated string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if the file is not created
*
* @comments This function creates the file, but does not close the file.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiFsCreateFile(L7_char8 *filename)
{
  L7_int32 fd;

  fd = creat(filename, 0644);
  if (fd == -1)
  {
    return(L7_FAILURE);
  }
  close (fd);

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Delete a file in the current subdirectory of the filesystem
*
* @param    filename @b{(input)} ptr to a null terminated string
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments    none.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiFsDeleteFile(L7_char8 *filename)
{
  if (remove(filename) == -1)
  {
    return(L7_ERROR);
  }

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Rename a file in the current subdirectory of the filesystem
*
* @param    oldfilename @b{(input)} old filename - ptr to a null terminated string
* @param    newfilename @b{(input)} new filename - ptr to a null terminated string
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments    none.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiFsRenameFile(L7_char8 *oldfilename, L7_char8 *newfilename)
{
  remove(newfilename);
  if (rename(oldfilename, newfilename) == -1)
  {
    return(L7_ERROR);
  }

  return(L7_SUCCESS);
}

/**************************************************************************
* @purpose  Sets the read/write pointer of an already open file
*
* @param    fd        Open file descriptor
* @param    nbytes    Offset
* @param    mode      SEEK_SET, pointer set to to offset bytes
*                     SEEK_CUR, pointer set to current value plus offset bytes
*                     SEEK_END, pointer set to size of the file plus offset bytes
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiFileSeek(L7_int32 fd, L7_int32 nbytes, L7_uint32 mode)
{
  if (lseek(fd, nbytes, mode) < 0)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Read specified number of bytes from the file descriptor.
*       If file is shorter then specified number of bytes then
*       return code is still success.
*
* @param    fd     Open file descriptor.
* @param    buffer   where to put data
* @param    nbytes    amount of data to read
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments
*
* @end
**************************************************************************/
EXT_API L7_RC_t osapiFileReadWithLen (L7_int32 fd, L7_char8 * buffer, L7_int32 * nbytesp)
{
  L7_uint32 nbytes = *nbytesp;
  L7_int32 byte_count, total_bytes;
  L7_RC_t  rc = L7_SUCCESS;

  *nbytesp = total_bytes = 0;
  do
  {
    byte_count = read (fd, &buffer[total_bytes], nbytes - total_bytes);
    if (byte_count == -1)
    {
      rc = L7_ERROR;
      break;
    }
    total_bytes += byte_count;
  }
  while ((byte_count > 0) && (total_bytes != nbytes));

  *nbytesp = total_bytes;
  return rc;
}

/**************************************************************************
* @purpose  Read specified number of bytes from the file descriptor.
*       If file is shorter then specified number of bytes then
*       return code is still success.
*
* @param    fd     Open file descriptor.
* @param    buffer   where to put data
* @param    nbytes    amount of data to read
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments
*
* @end
**************************************************************************/
EXT_API L7_RC_t osapiFileRead (L7_int32 fd, L7_char8 * buffer, L7_int32 nbytes)
{
  L7_int32 byte_count = nbytes;
  return osapiFileReadWithLen(fd, buffer, &byte_count);
}

/**************************************************************************
*
* @purpose  Read the contents of a file
*
* @param    filename @b{(input)} File from which to read data
* @param    buffer @b{(output)}  where to put data
* @param    nbytes @b{(input)}   amount of data to read
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments    This function opens the file, reads data, and closes the file
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiFsRead(L7_char8 *filename, L7_char8 *buffer, L7_int32 nbytes)
{
  int fd;
  ssize_t len;
  ssize_t tot = (ssize_t)nbytes;
  L7_RC_t rc = L7_SUCCESS;

  if ((fd = open(filename, O_RDWR, 0644)) == -1)
  {
    return(L7_ERROR);
  }

  while (tot > 0)
  {
    if ((len = read(fd, buffer, tot)) == -1)
    {
      if ((errno == EINTR) || (errno == EAGAIN))
      {
        continue;
      }
      else
      {
        rc = L7_ERROR;
        break;
      }
    }
    else
    {
      if (len == 0) /* EOF */
      {
        break;
      }
      else
      {
        tot -= len;
        buffer += len;
      }
    }
  }

  if (close(fd) == -1)
  {
    rc = L7_ERROR;
  }

  return(rc);
}

/**************************************************************************
*
* @purpose  Opens a file
*
* @param    filename @b{(input)} File to Open
* @param    fd       @b{(input)}  Pointer to file descriptor
*
* @returns  L7_ERROR if file does not exist
*
* @comments    none.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiFsOpen(L7_char8 *filename, L7_int32 *fd)
{
  if ((*fd = open(filename, O_RDWR, 0644)) == -1)
  {
    return(L7_ERROR);
  }

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  closes a file opened by osapiFsOpen
*
* @param    filedesc @b{(input)} descriptor of file to close
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR if file already closed
*
* @comments    none.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiFsClose(L7_int32 filedesc)
{
  if (close(filedesc) == -1)
  {
    return(L7_ERROR);
  }

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Retrieve file size
*
* @param    filename  @b{(input)}  string of file to retrieve size
* @param    filesize  @b{(output)} integer of file size
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiFsFileSizeGet(L7_char8 *filename, L7_uint32 *filesize)
{
  struct stat sbuf;

  /* file size is zero if L7_ERROR or the file does not exist */
  *filesize = L7_NULL;

  if (stat(filename, &sbuf) == -1)
  {
    return(L7_ERROR);
  }

  *filesize = (L7_uint32)sbuf.st_size;

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Write the contents of a file in blocks
*
* @param    filedesc @b{(input)} File descriptor from previous open
* @param    buffer @b{(output)}  where to put data 
* @param    nbytes @b{(input)}   amount of data to write (total)
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments    This function writes data to the file in blocks. The block
* @comments    size was determined roughly as how much data can be written
* @comments    in 500 ms. This is so other threads have time to run.
*
* @end
*
*************************************************************************/
static L7_RC_t osapiFsWriteInBlocks(L7_int32 filedesc, L7_char8 *buffer,
                                    L7_int32 nbytes)
{
  L7_int32 n;
  L7_int32 blocksize = 10 * 1024;
  L7_RC_t  rc = L7_SUCCESS;

  while (nbytes > 0)
  {
    n = write(filedesc, buffer, min(blocksize,nbytes));
    if (n > 0)
    {
      buffer += n;
      nbytes -= n;
    }
    else
    {
      rc = L7_ERROR;
      break;
    }

    sched_yield();   /* allow other threads to run */
  }

  return rc;
}

/**************************************************************************
*
* @purpose  Write data to a file. Create/Open file if does not exist.
*
* @param    filename @b{(input)} File to write data to
* @param    buffer @b{(input)}   actual data to write to file
* @param    nbytes @b{(input)}   number of bytes to write to file
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments If the WRITE_TO_FLASH flag shows ENABLED, also write contents of
*           RAM DISK to Flash.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiFsWrite(L7_char8 * filename, L7_char8 *buffer, L7_int32 nbytes)
{
  L7_int32 fd, rc = L7_SUCCESS;

  if ((fd = open(filename, O_RDWR | O_CREAT | O_TRUNC , 0644)) == -1)
  {
    return(L7_ERROR);
  }

  if (osapiFsWriteInBlocks(fd, buffer, nbytes) != L7_SUCCESS)
  {
    rc = L7_ERROR;
  }

  if (close(fd) == -1)
  {
    rc = L7_ERROR;
  }

  return(rc);
}

/**************************************************************************
*
* @purpose  Write the contents of a file
*
* @param    filedesc @b{(input)} File descriptor from previous open
* @param    buffer @b{(output)}  where to put data
* @param    nbytes @b{(input)}   amount of data to read
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments    This function writes data to the file, but does not close the file.
* @comments    osapiFsOpen must first be called.
* @comments    This function is used when the file pointer must be moved and not reset
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiFsWriteNoClose(L7_int32 filedesc, L7_char8 *buffer,
                            L7_int32 nbytes)
{
  L7_int32 rc = L7_SUCCESS;

  if (osapiFsWriteInBlocks(filedesc, buffer, nbytes) != L7_SUCCESS)
  {
    rc = L7_ERROR;
  }

  return(rc);
}

/***************************************************************************
*
* @purpose Obtain the directory information from a volume's directory
*
* @param  pFile      @b{(input)}  where to put listing
* @param  pDir       @b{(input)}  where to put the directory summary
* @param  max_files  @b{(input, output)}  Max number of files/ available 
*
* @comments  If successful, the @b{osapiFsList()} will return the details of 
*            various files, subdirectorys etc in a given directory.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @end
*
***************************************************************************/
EXT_API L7_RC_t osapiFsList ( L7_FILE_t *pFile, L7_DIR_t *pDir, L7_uint32 *max_files )
{
  L7_int32      fd;
  DIR           *pdir;
  struct dirent *pdirentry;
  struct stat   fileStat;
  L7_long32     directory_size = 0;
  struct statfs StatFS;
  L7_uint32 num_files = 0;

  if ((pdir = opendir(".")) == NULL)
  {
    return(L7_FAILURE);
  }

  /* Until readdir returns NULL, process each entry */

  while ( (( pdirentry = readdir(pdir) ) != NULL ) &&
          (num_files < (*max_files)))
  {
    strcpy(pFile->name, pdirentry->d_name);
    
    /* obtain info...use stat rather than fstat by specifying */
    /* a filename, not a file descriptor.                     */
    
    stat (pdirentry->d_name, &fileStat);

    if ( S_ISDIR (fileStat.st_mode) )
    {
      if((strcmp(pdirentry->d_name, ".") == 0) ||
         (strcmp(pdirentry->d_name, "..") == 0))
      {
        strcpy(pFile->type, OSAPI_SYSTEM_FILE);
      }
      else
      {
        strcpy(pFile->type, OSAPI_DIRECTORY);
      }

      strcpy(pFile->mode, "-");
      pFile->size = 0;
    }
    else
    {
      /* For files only, get the size */
      
      strcpy(pFile->type, OSAPI_REGULAR_FILE);

      pFile->size = fileStat.st_size;

      directory_size += fileStat.st_size;

      strcpy(pFile->mode, OSAPI_RW);
    }

    pFile->index = num_files;

    pFile++;
    num_files++;
 
  }

  *max_files = num_files;

  (void)closedir(pdir);

  /*
  * Get file system statistics. A file in the file system must
  * already have been opened using open( ) or creat( ). The file descriptor
  * returned by open( ) or creat( ) must be passed in. Create a dummy file
  * that we know, get the stats, then delete it.
  */
  
  fd = open ("dummy1", O_RDWR | O_CREAT, 0644);
  (void)fstatfs (fd, &StatFS);
  close(fd);
  (void)remove ( "dummy1" );
  
  pDir->totalSize = (StatFS.f_bsize * StatFS.f_blocks);
  pDir->size = directory_size;  
  pDir->freeSize = (pDir->totalSize - pDir->size);

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Copies a file
*
* @param    infilename   @b{(input)}  in filename - name of file to read
* @param    outfilename  @b{(input)}  out filename - name of file to write
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiFsCopyFile(L7_char8 *infilename, L7_char8 *outfilename)
{
   L7_char8 buf[0x80];

  snprintf(buf, sizeof(buf), "cp %s %s >/dev/null 2>&1",
      infilename, outfilename);

  if (WEXITSTATUS(system(buf)) != 0) 
  {
    return (L7_FAILURE);
  }

  return L7_SUCCESS;
}
 
/**************************************************************************
*
* @purpose     Dump all buffered files to the file system.
*
* @param       none
* @returns     none
*
* @comments    none
*
* @end
*
*************************************************************************/
void osapiFsSync (void)
{
  sync ();
}

/***************************************************************************
*
* @purpose Obtain the directory information from a volume's directory
*
* @param  pFile      @b{(input)}  where to put listing
* @param  max_files  @b{(input, output)}  Max number of files/ available
*
* @comments  If successful, the @b{osapiFsList()} will return the details of
*            various files, subdirectorys etc in a given directory.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @end
*
***************************************************************************/
L7_RC_t osapiFsListCompact( L7_FILE_COMPACT_t *pFile, L7_uint32 *max_files )
{
  DIR           *pdir;
  struct dirent *pdirentry;
  struct stat   fileStat;
  L7_uint32     num_files = 0;

  /* Open the directory in question.*/
  if ((pdir = opendir(".")) == NULL)
  {
    return(L7_FAILURE);
  }

  /* Until readdir returns NULL, process each entry */

  while ( (( pdirentry = readdir(pdir) ) != NULL ) &&
          (num_files < (*max_files)))
  {
    strncpy(pFile->name, pdirentry->d_name, sizeof(pFile->name));
    pFile->name[sizeof(pFile->name)-1] = '\0';

    /* obtain info...use stat rather than fstat by specifying */
    /* a filename, not a file descriptor.                     */

    stat(pdirentry->d_name, &fileStat);

    if ( S_ISDIR(fileStat.st_mode) )
    {
      if ((strcmp(pdirentry->d_name, ".") == 0) ||
          (strcmp(pdirentry->d_name, "..") == 0))
      {
        pFile->type = OSAPI_TYPE_SYSTEM_FILE;
      }
      else
      {
        pFile->type = OSAPI_TYPE_DIRECTORY;
      }
    }
    else if ( S_ISREG(fileStat.st_mode) )
    {
      pFile->type = OSAPI_TYPE_REGULAR_FILE;
    }
    else
    {
      continue;                 /* Don't care about special file types */
    }

    pFile->size = fileStat.st_size;

    if ( (S_IRUSR | S_IWUSR) == (fileStat.st_mode & (S_IRUSR | S_IWUSR)) )
    {
      pFile->mode = OSAPI_PERM_RW;
    }
    else if ( S_IRUSR == (fileStat.st_mode & S_IRUSR))
    {
      pFile->mode = OSAPI_PERM_RO;
    }
    else if (S_IWUSR == (fileStat.st_mode & S_IWUSR))
    {
      pFile->mode = OSAPI_PERM_WO;
    }
    else
    {
      pFile->mode = OSAPI_PERM_MAX;
    }

    pFile++;
    num_files++;
  }

  *max_files = num_files;

  (void)closedir(pdir);

  return(L7_SUCCESS);

}   /* end: osapiFsListCompact */

/**************************************************************************
 *
 * @purpose  Check if the file already exists
 *
 * @param    filename  @b{(input)}  file to check
 *
 * @returns  L7_TRUE    if specified file exist
 * @returns  L7_FALSE   if not exist
 *
 * @comments
 *
 * @end
 *
 *************************************************************************/
L7_BOOL osapiFsFileExist(L7_char8 * filename)
{
  L7_int32 filedesc;

  filedesc = open (filename, O_RDONLY, 0);
  if (filedesc == -1) 
  {
    return L7_FALSE;
  }
  close(filedesc);
  return L7_TRUE;
}

