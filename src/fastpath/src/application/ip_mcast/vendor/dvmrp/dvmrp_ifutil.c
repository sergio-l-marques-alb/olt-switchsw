/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   ifutil.c
*
* @purpose    if routines 
*
*
* @comments   
*
* @create     03/27/2006
*
* @author     Dasoju Shashidhar
* @end
*
**********************************************************************/
#include "osapi.h"
#include "osapi_support.h"

/*********************************************************************
* @purpose  Returns whether the content of pointer is Zero or not.
*
* @param    *ptr   -    @b{(input)}  Pointer to Data element.
* @param    size   -    @b{(input)}  Size of the data element. 
*
* @returns  0 : if content of data element is non zero
* @returns  1 : if content of data element is zero
*
* @notes   None
*
* @end
*********************************************************************/
L7_int32 ifzero (void *ptr, L7_int32 size)
{
  L7_uchar8 *p = ptr;
  L7_int32 zero = 0;
  while (size--)
  {
    zero |= *p++;
  }
  return(zero == 0);
}

/*********************************************************************
* @purpose 
*
* @param    *a   -     @b{(input)}  
* @param    *b   -     @b{(input)}  
* @param    *c   -     @b{(input)}  
* @param    size   -   @b{(input)}    
*
* @returns 
*
* @notes   None
*
* @end
*********************************************************************/
L7_int32 ifor (void *a, void *b, void *c, L7_int32 size)
{
  L7_uchar8 *p = a;
  L7_uchar8 *q = b;
  L7_uchar8 *r = c;
  L7_int32 zero = 0;
  while (size--)
  {
    zero |= (*p | *q);
    if (r)
    {
      *r++ = (*p | *q);
    }
    p++;
    q++;
  }
  return(zero);
}



