/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_parse_util.c
*
* @purpose VOIP parsing utility functions
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
#include "voip_parse_util.h"
#include "osapi.h"
#include "log.h"
#include "stdio.h"
/*********************************************************************
* @purpose Intialize the VOIP list
*
* @param   voipList_t * list  pointer to voip List
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipListInit (voipList_t *list)
{
  if (list == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  memset (list, 0x00, sizeof (voipList_t));
  list->count = 0;
  return L7_SUCCESS;
}
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
L7_uint32 voipListSize (const voipList_t * list)
{
  if (list != L7_NULLPTR)
  {
    return list->count;
  }
  else
  {
    return 0;
  }
}
/*********************************************************************
* @purpose Add an element to VOIP list
*
* @param   voipList_t * list       pointer to voip List
* @param   void *element           pointer to element
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes   Inserts at beginning of list
*
* @end
*********************************************************************/
L7_RC_t voipListAdd(voipList_t *list, void *element)
{
  voipListNode_t *ntmp;

  if (list ==L7_NULLPTR)
    return L7_FAILURE;

  if (list->count == 0)
  {
    list->node = (voipListNode_t *) osapiMalloc (L7_FLEX_QOS_VOIP_COMPONENT_ID,sizeof (voipListNode_t));
    if (list->node == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
              "VOIP:voipListAdd Could not allocate memory\n");
      return L7_FAILURE;
    }
    list->node->element = element;
    list->node->next = L7_NULLPTR;
  }
  else
  {
    ntmp = (voipListNode_t *) osapiMalloc (L7_FLEX_QOS_VOIP_COMPONENT_ID,sizeof (voipListNode_t));
    if (ntmp == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
              "VOIP:voipListAdd Could not allocate memory\n");
      return L7_FAILURE;
    }
    ntmp->element = element;
    ntmp->next = list->node;
    list->node = ntmp;
  }
  list->count++;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Returns element at position specified by pos
*
* @param   const voipList_t *list   pointer to voip List
* @param   L7_uint32 pos            pos of element to be retrieved

* @returns  pointer to element
*
* @notes    none
*
* @end
*********************************************************************/
void *voipListGet(const voipList_t *list, L7_uint32 pos)
{
  voipListNode_t *ntmp;
  L7_uint32 i = 0;

  if (list == L7_NULLPTR)
    return L7_NULLPTR;

  if (list->count == 0 || pos > list->count)
    return L7_NULLPTR;

  ntmp = list->node;
  for(i = 1; i <= pos; i++)
    ntmp = ntmp->next;
  return ntmp->element;

}
/*********************************************************************
* @purpose Removes voipListNode element at position specified by pos
*
* @param   voipList_t *list       pointer to voip List
* @param   L7_uint32 pos            pos of element to be removed
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void voipListRemoveNode(voipList_t *list, L7_uint32 pos)
{
  voipListNode_t *ntmp;
  voipListNode_t *prev;
  L7_uint32 i = 0;

  if (list == L7_NULLPTR)
    return;

  if (list->count == 0 || pos > list->count)
    return;

  prev = L7_NULLPTR;
  ntmp = list->node;
  for(i = 1; i <= pos; i++)
  {
    prev = ntmp;
    ntmp = ntmp->next;
  }
  if(prev!=L7_NULLPTR)
  {
    prev->next = ntmp->next;
  }
  else
  {
    list->node = ntmp->next;
  }
  osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,ntmp);
  list->count--;
}


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
L7_uchar8 *voipStrncpy (L7_uchar8 *dest, const L7_uchar8 *src,
                        L7_uint32 length)
{
  strncpy (dest, src, length);
  dest[length] = '\0';
  return dest;
}
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
                          L7_uchar8 **next)
{
  L7_uchar8 *sep = L7_NULLPTR;                    /* separator */
  *next = L7_NULLPTR;

  sep = buf;
  while ((*sep != endSeparator) && (*sep != '\0') && (*sep != '\r')
         && (*sep != '\n'))
    sep++;
  if ((*sep == '\r') || (*sep == '\n'))
  {                           /* we should continue normally only if this is the separator asked! */
    if (*sep != endSeparator)
      return L7_FAILURE;
  }
  if (*sep == '\0')
    return L7_FAILURE;                  /* value must not end with this separator! */

  if (sep == buf)
    return L7_FAILURE;                  /* empty value (or several space!) */

  *dest = (L7_uchar8 *)osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,sep - (buf) + 1);
  if(*dest == L7_NULLPTR)
  {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
             "VOIP: voipSetNextToken could not allocate memory\n");
     return L7_FAILURE;
  }
  voipStrncpy (*dest, buf, sep - buf);
  *next = sep + 1;              /* return the position right after the separator */

  return L7_SUCCESS;
}

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
L7_RC_t voipFindNextCrlf (const L7_uchar8 *startHeader, const L7_uchar8 **endHeader)
{
  const L7_uchar8 *soh = startHeader;

  *endHeader = L7_NULLPTR;

  while (('\r' != *soh) && ('\n' != *soh))
  {
    if (*soh)
      soh++;
    else
    {
      return L7_FAILURE;
    }
  }
  if (('\r' == soh[0]) && ('\n' == soh[1]))
  {
    /* case 1: CRLF is the separator
       case 2 or 3: CR or LF is the separator */
    soh = soh + 1;
  }

  *endHeader = soh + 1;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Copies the data of length len from destination to source
*           after clearing the destination
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
L7_uchar8 *voipClrncpy (L7_uchar8 *dst, const L7_uchar8 *src, L7_uint32 len)
{
  const L7_uchar8 *pbeg;
  const L7_uchar8 *pend;
  L7_uchar8 *p;
  L7_uint32 spaceless_length;

  if (src == L7_NULLPTR)
    return L7_NULLPTR;

  /* find the start of relevant text */
  pbeg = src;
  while ((' ' == *pbeg) || ('\r' == *pbeg) || ('\n' == *pbeg) || ('\t' == *pbeg))
    pbeg++;


  /* find the end of relevant text */
  pend = src + len - 1;
  while ((' ' == *pend) || ('\r' == *pend) || ('\n' == *pend) || ('\t' == *pend))
  {
    pend--;
    if (pend < pbeg)
    {
      *dst = '\0';
       return dst;
    }
  }

  /* if pend == pbeg there is only one char to copy */
  spaceless_length = pend - pbeg + 1;   /* excluding any '\0' */
  memmove (dst, pbeg, spaceless_length);
  p = dst + spaceless_length;

  /* terminate the string and pad dest with zeros until len */
  do
  {
    *p = '\0';
    p++;
    spaceless_length++;
  }
  while (spaceless_length < len);

  return dst;
}


