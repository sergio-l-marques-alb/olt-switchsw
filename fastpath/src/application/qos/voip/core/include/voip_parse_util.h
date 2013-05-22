/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_parse_util.h
*
* @purpose VOIP Signalling protocol parsing utility functions
*
* @component VOIP
*
* @comments none
*
* @create 05/12/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#ifndef VOIP_PARSE_UTIL_H
#define VOIP_PARSE_UTIL_H

#include <string.h>
#include "l7_common.h"

typedef struct voipListNode_s
{
  void *next;                 /* next voipListNode_t containing element */
  void *element;              /* element in Current node */
}voipListNode_t;

typedef struct voip_list
{
  L7_uint32 count;          /* Number of element in the list */
  voipListNode_t *node;     /* Next node containing element  */
}voipList_t;

/*********************************************************************
* @purpose Intialize the VOIP list
*
* @param   voipList_t * list    pointer to voip List
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipListInit (voipList_t * list);

/*********************************************************************
* @purpose returns the number of elements in list
*
* @param   voipList_t * list    pointer to voip List
*
* @returns  Number of element if none then zero
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 voipListSize (const voipList_t * list);

/*********************************************************************
* @purpose Add an element to VOIP list
*
* @param   voipList_t * list         pointer to voip List
* @param   void *element           pointer to element
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes   Inserts at beginning of list
*
* @end
*********************************************************************/
L7_RC_t voipListAdd(voipList_t * list, void *element);

/*********************************************************************
* @purpose Returns element at position specified by pos 
*
* @param   const voipList_t * list  pointer to voip List
* @param   L7_uint32 pos          pos of element to be retrieved

* @returns  pointer to element
*
* @notes    none
*
* @end
*********************************************************************/
void *voipListGet(const voipList_t * list, L7_uint32 pos);

/*********************************************************************
* @purpose copy Source string to destination string
*
* @param   L7_uchar8 *dest         pointer to destination string
* @param   const L7_uchar8 *src    pointer to Source string
* @param   L7_uint32 length        length of the string to be copied
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uchar8 *voipStrncpy (L7_uchar8 *dest, const L7_uchar8 *src, L7_uint32 length);

/*********************************************************************
* @purpose copy the token to dest by extracting it from buf on basis
*          of end_seperator
*
* @param   L7_uchar8 **dest        pointer to destination string
* @param   const L7_uchar8 *buf    pointer to buf
* @param   L7_uint32 endSeparator  seperator value
* @param   L7_uchar8 **next        pointer to next
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t voipSetNextToken (L7_uchar8 **dest, L7_uchar8 *buf, L7_uint32 endSeparator,
                          L7_uchar8 **next);

/*********************************************************************
* @purpose Look for next CRLF in header field and initialize endHeader
*          with starting of next header field
*
* @param   const L7_uchar8 *startHeader   pointer to startHeader
* @param   const L7_uchar8 **endHeader    pointer to endHeader
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t voipFindNextCrlf (const L7_uchar8 *startHeader, const L7_uchar8 **endHeader);

/*********************************************************************
* @purpose  Clear the destination buffer and copy the data from src
*           of length len
*
* @param   L7_uchar8 *dst                 pointer to dst
* @param   const L7_uchar8 *src           pointer to src
* @param   L7_uint32                      len
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_uchar8 *voipClrncpy (L7_uchar8 *dst, const L7_uchar8 *src, L7_uint32 len);

/*********************************************************************
* @purpose Removes voipListNode element at position specified by pos
*
* @param   voipList_t *list       pointer to voip List
* @param   L7_uint32 pos            pos of element to be removed

* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void voipListRemoveNode(voipList_t *list, L7_uint32 pos);

#endif
