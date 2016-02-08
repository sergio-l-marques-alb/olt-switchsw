#ifdef _L7_OS_LINUX_
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  osapipipes.c
*
* @purpose   OS independent pipes API's
*
* @component osapi
*
* @comments 
*
* @create    01/08/2001
*
* @author    Neil Horman 
*
* @end
*
*********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <l7_common.h>
#include <osapi.h>
#include <errno.h>



/*********************************************************************
* @purpose  Initializes pipes.
*          
*	
* @returns     L7_SUCCESS 
* @returns     L7_ERROR driver returned an error
*
* @comments    none
*           
* @end
*
*********************************************************************/
L7_RC_t osapiPipesInitialize ()
{
  /*No init required*/
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Creates a pipe.
*          
* @param 		name        Name of pipe to be created.
* @param        nMessages   Max. number of messages in pipe.
* @param        nBytes      Size of each message.
*	
* @returns     L7_SUCCESS 
* @returns     L7_ERROR 
*
* @comments    none
*           
* @end
*
*********************************************************************/
L7_RC_t osapiPipeCreate(L7_char8* name, L7_int32 nMessages, L7_int32 nBytes)
{
  dev_t temp = 0;

  /* The man page of mknod section 2 states, 3rd parm is not used for this
     specific parm 2 options. So it is not filled here.
  */
  if (mknod(name, O_RDWR | S_IFIFO, temp) < 0)
  {
	if (errno == EEXIST)
	  return L7_SUCCESS;
	return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Opens a pipe.
*          
* @param 		name        Name of pipe to be opened.
* @param        flag        O_RDONLY, O_WRONLY, O_RDWR, or O_CREAT.
* @param        mode        Mode of file to create (UNIX chmod style) )
* @param    	descriptor  ptr to pipe descriptor
*	
* @returns     L7_SUCCESS	If pipe has been opened succesfully
* @returns     L7_ERROR 	If any error occurs while opening pipe
*
* @comments    
*           
* @end
*
*********************************************************************/
L7_RC_t osapiPipeOpen(L7_char8* name, L7_int32 flags,L7_int32 mode, L7_uint32 *descriptor)
{
  int fd;
  fd = open(name,flags,mode);
  if(fd == -1)
    {
      perror("unable to open pipe file\n");
      return L7_ERROR;
    }
  *descriptor = fd;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Closes a pipe.
*          
* @param 		fd          The file descriptor to close.
*	
* @returns     L7_SUCCESS 
* @returns     L7_ERROR 
*
* @comments    none
*           
* @end
*
*********************************************************************/
L7_RC_t osapiPipeClose(L7_int32 fd)
{

  close(fd);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Reads from a pipe.
*          
* @param 		fd          The file descriptor from which to read.
* @param 		buffer      Pointer to buffer to receive bytes.
* @param        mode        Max no. of bytes to read into buffer.
*	
* @returns      The number of bytes read (between 1 and maxbytes, 0 if end of file). 
* @returns      L7_ERROR  on error
*
* @comments    none
*           
* @end
*
*********************************************************************/
L7_int32 osapiPipeRead(L7_int32 fd, L7_char8* buffer, L7_int32 maxbytes)
{
    L7_int32 ret;
        
    if ((ret = read(fd,buffer,maxbytes)) == -1) {
        return L7_ERROR;
    }
    return ret;
}


/*********************************************************************
* @purpose  Writes to a pipe.
*          
* @param 		fd          The file descriptor on which to write.
* @param 		buffer      Buffer containing bytes to be written.
* @param        mode        Number of bytes to write.
*	
* @returns      The number of bytes written (if not equal to nbytes, an error has occurred). 
* @returns      L7_ERROR  on error
*
* @comments    none
*           
* @end
*
*********************************************************************/
L7_int32 osapiPipeWrite(L7_int32 fd, L7_char8* buffer, L7_int32 nbytes )
{
    int ret;
    if ((ret = write(fd,buffer,nbytes )) == -1) {
        return L7_ERROR;
    }
    return ret;
}
#endif 
