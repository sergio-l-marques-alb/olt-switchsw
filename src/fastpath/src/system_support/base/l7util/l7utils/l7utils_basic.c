/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   l7utils_basic.c
*
* @purpose    Basic utilities
*
* @component  l7utils
*
* @comments   Anything put in this file must be compatible with building
* @comments   the VxWorks startup image, meaning it cannot have too 
* @comments   many dependencies on other system or FASTPATH functionality
* @comments   (e.g. osapiSnprintf or extensive C library calls).
*
* @create     05/30/2007
*
* @author     gpaussa
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "l7utils_api.h"


/*********************************************************************
* @purpose  Given a pointer to full path of a file, change the pointer
*           to point to the beginning of the file name
*
* @param    **fullPath    @{(input)}  Address of pointer to full path to file
*
* @returns  none
*
* @end
*********************************************************************/
void l7utilsFilenameStrip(L7_char8 **fullPath)
{
  L7_uint32 i;
  L7_char8 *fileP;

  if ((fullPath == L7_NULLPTR) || (*fullPath == L7_NULLPTR))
    return;

  i = strlen(*fullPath);
  fileP = &((*fullPath)[i]);

  /* look for the first slash from the end of string */
  for (; i > 0; i--)
  {
    fileP--;

    /* check for forward or backward slash (need two '\' since first one's an escape char) */
    if (*fileP == '/' || *fileP == '\\')
    {
      fileP++;    /* went too far, move ahead one char */
      break;
    }
  }

  *fullPath = fileP;
}
