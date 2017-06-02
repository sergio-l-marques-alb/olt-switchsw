/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  md5map.c
*
* @purpose   Maps Vendor MD5 support to LVL7 utility functions
*
* @component MD5 Mapping Layer
*
* @comments  none
*
* @create    02/13/2003
*
* @author    gpaussa
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "md5_api.h"
#include "md5map_api.h"


/*********************************************************************
* @purpose  Produces an MD5 message digest for a message of arbitrary length
*
* @param    *msgBuf     @b{(input)}  Ptr to message character buffer       
* @param    msgLen      @b{(input)}  Message length in bytes
* @param    *digest     @b{(output)} Ptr to message digest output location
*
* @returns  void
*
* @comments Uses native data types for parameters to reduce dependency
*           on vendor-specific types.
*
* @comments The message is assumed to be an integral number of bytes.
*           Although the MD5 algorithm can handle a message of arbitrary
*           bit length, some optimization is realized when working with
*           a stream of characters rather than bits.
*
* @comments The message is assumed to be a complete message and not part of
*           a larger message.  Stated another way, each call to this function
*           generates a new digest based on the entire contents of the 
*           message buffer in accordance with the MD5 algorithm.
*       
* @end
*********************************************************************/
void md5MapDigestGet(unsigned char *msgBuf, unsigned int msgLen, 
                     unsigned char *digest)
{
  L7_MD5_CTX_t  context;

  l7_md5_init(&context);
  l7_md5_update(&context, (L7_uchar8 *)msgBuf, (L7_uint32)msgLen);
  l7_md5_final((L7_uchar8 *)digest, &context);
}

/*********************************************************************
* @purpose  Begin a new message digest operation by initializing the context.
*
* @param    *context    @{(input)} Pointer to standard MD5 operating context
*
* @returns  void
*
* @comments Uses native data types for parameters to reduce dependency
*           on vendor-specific types.
*
* @comments Must be called once at the beginning of each new digest 
*           computation.

*
* @end
*********************************************************************/
void md5MapInit(void *context)
{
  l7_md5_init((L7_MD5_CTX_t *)context);
}

/*********************************************************************
* @purpose  Process a message block and update context with latest digest.
*
* @param    *context    @{(input)} Pointer to standard MD5 operating context
* @param    *input      @{(input)} Pointer to message block character array
* @param    inputLen    @{(input)} Message block length, in bytes
*
* @returns  void
*
* @comments Uses native data types for parameters to reduce dependency
*           on vendor-specific types.
*
* @comments May be called repeatedly to process additional parts of the
*           overall message.  The computed message digest is updated 
*           with each invocation of this function.
*
* @end
*********************************************************************/
void md5MapUpdate(void *context, unsigned char *input, unsigned int inputLen)
{
  l7_md5_update((L7_MD5_CTX_t *)context, (L7_uchar8 *)input, (L7_uint32)inputLen);
}

/*********************************************************************
* @purpose  Finish the MD5 message-digest calculation and output the result.
*
* @param    *digest     @{(output)} Pointer to message digest output location
* @param    *context    @{(input)}  Pointer to standard MD5 operating context
*
* @returns  void
*
* @comments Uses native data types for parameters to reduce dependency
*           on vendor-specific types.
*
* @comments Must be called after the last MD5 Update operation.
*
* @comments The digest output parameter is intentionally listed first to 
*           match the order specified by RFC 1321.
*
* @end
*********************************************************************/
void md5MapFinal(unsigned char *digest, void *context)
{
  l7_md5_final((L7_uchar8 *)digest, (L7_MD5_CTX_t *)context);
}

