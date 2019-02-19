/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_h323.c
*
* @purpose H323 protocol definations
*
* @component VOIP | H323
*
* @comments
*
* @create 05/12/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#include "voip_exports.h"
#include "voip_h323.h"
#include "voip_h323_message_types.h"
#include "osapi_support.h"
#include "voip_debug.h"
#include "dtl_voip.h"
#include "voip_control.h"
#include "usmdb_util_api.h"
#include "sysapi.h"

/* Decoder Functions */
/* The packets of H245 and Q931 are ASN.1 encoded To parse the packets
we first need to decode them . Following are the functions used to decode the 
packet recieved */

static L7_RC_t decode_nul(asnBitStr_t *bs, asnField_t *f, L7_uchar8 *base, L7_uint32 level);
static L7_RC_t decode_bool(asnBitStr_t *bs, asnField_t *f, L7_uchar8 *base, L7_uint32 level);
static L7_RC_t decode_oid(asnBitStr_t *bs, asnField_t *f, L7_uchar8 *base, L7_uint32 level);
static L7_RC_t decode_int(asnBitStr_t *bs, asnField_t *f, L7_uchar8 *base, L7_uint32 level);
static L7_RC_t decode_enum(asnBitStr_t *bs, asnField_t *f, L7_uchar8 *base, L7_uint32 level);
static L7_RC_t decode_bitstr(asnBitStr_t *bs, asnField_t *f, L7_uchar8 *base, L7_uint32 level);
static L7_RC_t decode_numstr(asnBitStr_t *bs, asnField_t *f, L7_uchar8 *base, L7_uint32 level);
static L7_RC_t decode_octstr(asnBitStr_t *bs, asnField_t *f, L7_uchar8 *base, L7_uint32 level);
static L7_RC_t decode_bmpstr(asnBitStr_t *bs, asnField_t *f, L7_uchar8 *base, L7_uint32 level);
static L7_RC_t decode_seq(asnBitStr_t *bs, asnField_t *f, L7_uchar8 *base, L7_uint32 level);
static L7_RC_t decode_seqof(asnBitStr_t *bs, asnField_t *f, L7_uchar8 *base, L7_uint32 level);
static L7_RC_t decode_choice(asnBitStr_t *bs, asnField_t *f, L7_uchar8 *base, L7_uint32 level);

/* Decoder Functions Vector */
typedef L7_RC_t (*decoder_t) (asnBitStr_t *, asnField_t *, L7_uchar8 *, L7_uint32);
static decoder_t decoders[] = {
  decode_nul,
  decode_bool,
  decode_oid,
  decode_int,
  decode_enum,
  decode_bitstr,
  decode_numstr,
  decode_octstr,
  decode_bmpstr,
  decode_seq,
  decode_seqof,
  decode_choice,
};

static voipH323Call_t *voipH323CallList=L7_NULLPTR;

/*********************************************************************
* @purpose To check if two q931 connections are equal or not
*
* @param   connInfo_t       *ct_info   Pointer to connInfo_t
* @param   const connPair_t *info      Pointer to connection to be 
*          compared
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
q931ConnInfoEqual(connInfo_t *ct_info, const connPair_t *info)
{
  if ( (ct_info->pair.src_ip == info->src_ip) &&
     (ct_info->pair.dst_ip == info->dst_ip) &&
     (ct_info->pair.src_port == info->src_port) &&
     (ct_info->pair.dst_port == info->dst_port) ) 
  {
     /* originating side */
     ct_info->dir = 0;
     return L7_SUCCESS;
  }
  if ( (ct_info->pair.src_ip == info->dst_ip) &&
     (ct_info->pair.dst_ip == info->src_ip) &&
     (ct_info->pair.src_port == info->dst_port) &&
     (ct_info->pair.dst_port == info->src_port) ) 
  {
     /* terminating side */
     ct_info->dir = 1;
     return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose To check if two h245 connections are equal or not
*
* @param   connInfo_t       *ct_info   Pointer to connInfo_t
* @param   const connPair_t *info      Pointer to connection to be
*          compared
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
h245ConnInfoEqual(connInfo_t *ct_info, const connPair_t *info)
{
  if ( (ct_info->h245_ip == info->src_ip) &&
      (ct_info->h245_port == info->src_port) ) 
  {
    /* terminating side */
    ct_info->dir = 1;
    return L7_SUCCESS;
  }
  if ( (ct_info->h245_ip == info->dst_ip) &&
    (ct_info->h245_port == info->dst_port) ) 
  {
    /* originating side */
    ct_info->dir = 0;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose Look up for a H323 Call
*
* @param   voipH323Call_t   **list    List of H323 calls
* @param   connPair_t       *info     Pointer to connInfo_t 
* @param   voipH323Call_t   **entry   H323 call entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipH323CallLookup(voipH323Call_t **list, const connPair_t *info, voipH323Call_t **entry)
{
  while (*list != L7_NULLPTR) 
  {
    if (q931ConnInfoEqual(&((*list)->ct_info), info)==L7_SUCCESS) 
    {
      *entry = *list;
      return L7_SUCCESS;
    }
    list = &(*list)->next;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose Look up for a H245 Call
*
* @param   voipH323Call_t   **list    List of H323 calls
* @param   connPair_t       *info     Pointer to connInfo_t
* @param   voipH323Call_t   **entry   H323 call entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipH245CallLookup(voipH323Call_t **list, const connPair_t *info,voipH323Call_t **entry)
{
  while (*list != L7_NULLPTR) 
  {
    if (h245ConnInfoEqual(&((*list)->ct_info), info) == L7_SUCCESS) 
    {
      *entry = *list;
       return L7_SUCCESS;
    }
    list = &(*list)->next;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose Insert H323 call entry
*
* @param   voipH323Call_t   **list    List of H323 calls
* @param   connPair_t       *info     Pointer to connInfo_t
* @param   voipH323Call_t   **ins     Inserted entry 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipH323CallInsert(voipH323Call_t **list, const connPair_t *info, voipH323Call_t **ins)
{
  voipH323Call_t *entry;
  L7_char8  strSrcIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  strDstIPaddr[L7_CLI_MAX_STRING_LENGTH];

  if ((entry = osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID,sizeof(voipH323Call_t))) == L7_NULLPTR) 
  {
    LOG_MSG("voipH323CallInsert memory allocation failed\n");
    return L7_FAILURE;
  }
  memset(entry, 0x00,sizeof(voipH323Call_t));
  entry->ct_info.pair.src_ip = info->src_ip;
  entry->ct_info.pair.dst_ip = info->dst_ip;
  entry->ct_info.pair.src_port = info->src_port;
  entry->ct_info.pair.dst_port = info->dst_port;
  entry->next = *list;
  *list = entry;
  *ins = entry;

  /* Log the event */
  usmDbInetNtoa(entry->ct_info.pair.src_ip, strSrcIPaddr);
  usmDbInetNtoa(entry->ct_info.pair.dst_ip, strDstIPaddr);

  LOG_MSG("Inserted new H323 call\r\n Source IP = %s, Dest IP = %s\r\n"
           "Source Port = %d, Dest Port = %d\r\n", 
           strSrcIPaddr, strDstIPaddr,
           entry->ct_info.pair.src_port, 
           entry->ct_info.pair.dst_port);
  
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Remove H323 call entry
*
* @param   voipH323Call_t   **list    List of H323 calls
* @param   voipH323Call_t   *del      Entry to be deleted
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipH323CallRemove(voipH323Call_t **list, connInfo_t *del)
{
  voipH323Call_t *entry;
  L7_char8  strSrcIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  strDstIPaddr[L7_CLI_MAX_STRING_LENGTH];
  
  /* Log the event */
  usmDbInetNtoa(del->pair.src_ip, strSrcIPaddr);
  usmDbInetNtoa(del->pair.dst_ip, strDstIPaddr);

  LOG_MSG("Removed H323 call\r\n Source IP = %s, Dest IP = %s\r\n"
               "Source Port = %d, Dest Port = %d\r\n", 
               strSrcIPaddr, strDstIPaddr,
               del->pair.src_port, 
               del->pair.dst_port);
  
  while (*list != L7_NULLPTR) 
  {
    if (memcmp(&(*list)->ct_info,del,sizeof(connInfo_t))==0) 
    {
      entry = *list;
      *list = entry->next;

        osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID,entry);
      return L7_SUCCESS;
    }
    list = &(*list)->next;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  To get length of ASN bit STRING
*
* @param    asnBitStr_t *bs     Pointer to bitStr_t
*
* @return   Length to bit string 
*
* @notes    none Assume bs is aligned && v < 16384 
*
* @end
*********************************************************************/
L7_uint32 asnGetLen(asnBitStr_t * bs)
{
  L7_uint32 v;

  v = *bs->cur++;

  if (v & 0x80) 
  {
    v &= 0x3f;
    v <<= 8;
    v += *bs->cur++;
  }
  return v;
}
/*********************************************************************
* @purpose  Returns the value of current bit an increment the pointers
*           to point to next
*
* @param    asnBitStr_t *bs     Pointer to bitStr_t
*
* @return   bit value
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 asnGetBit(asnBitStr_t * bs)
{
  L7_uint32 b = (*bs->cur) & (0x80 >> bs->bit);

  INC_BIT(bs);
  return b;
}
/*********************************************************************
* @purpose  returns the value represented by a group of bits specified 
*           by b.Starting point for gtting value is bs->cur
*
* @param    asnBitStr_t *bs     Pointer to bitStr_t
* @param    L7_uint32    b      number of bits
*
* @return   bit value
*
* @notes    Assume b <= 8 
*
* @end
*********************************************************************/
L7_uint32 asnGetBits(asnBitStr_t * bs, L7_uint32 b)
{
  L7_uint32 v, l;

  v = (*bs->cur) & (0xffU >> bs->bit);
  l = b + bs->bit;

  if (l < 8) 
  {
    v >>= 8 - l;
    bs->bit = l;
  }
  else if (l == 8) 
  {
    bs->cur++;
    bs->bit = 0;
  } 
  else 
  {    /* l > 8 */
    v <<= 8;
    v += *(++bs->cur);
    v >>= 16 - l;
    bs->bit = l - 8;
  }
  return v;
}
/*********************************************************************
* @purpose  returns th Bit map
*
* @param    asnBitStr_t *bs     Pointer to bitStr_t
* @param    L7_uint32 b         number of bits
*
* @return   bitMap
*
* @notes    Assume b <= 8
*
* @end
*********************************************************************/
L7_uint32 asnGetBitmap(asnBitStr_t * bs, L7_uint32 b)
{
  L7_uint32 v, l, shift, bytes;

  if (!b)
    return 0;

  l = bs->bit + b;

  if (l < 8) 
  {
    v = (L7_uint32) (*bs->cur) << (bs->bit + 24);
    bs->bit = l;
  }
  else if (l == 8) 
  {
    v = (L7_uint32) (*bs->cur++) << (bs->bit + 24);
    bs->bit = 0;
  } 
  else 
  {
    for (bytes = l >> 3, shift = 24, v = 0; bytes; bytes--, shift -= 8)
      v |= (L7_uint32) (*bs->cur++) << shift;

    if (l < 32) 
    {
      v |= (L7_uint32) (*bs->cur) << shift;
      v <<= bs->bit;
    }
    else if (l > 32) 
    {
      v <<= bs->bit;
      v |= (*bs->cur) >> (8 - bs->bit);
    }

    bs->bit = l & 0x7;
  }

  v &= 0xffffffff << (32 - b);

  return v;
}

/****************************************************************************
 * Assume bs is aligned and sizeof(L7_uint32 int) == 4
 ****************************************************************************/
/*
L7_uint32 asnGet_uint(asnBitStr_t * bs, int b)
{
        L7_uint32 v = 0;

        switch (b) {
        case 4:
                v |= *bs->cur++;
                v <<= 8;
        case 3:
                v |= *bs->cur++;
                v <<= 8;
        case 2:
                v |= *bs->cur++;
                v <<= 8;
        case 1:
                v |= *bs->cur++;
                break;
        }
        return v;
}
*/

/* Follwing functions are used to decode the received packets as per their
respective type */ 
/*********************************************************************
* @purpose deocode the ASN NUL type 
*
* @param  asnBitStr_t *bs    Pointer to asnBitStr_t    
* @param  asnField_t  *f     Pointer to field  
* @param  L7_uchar8   *base  Pointer to address where the decoded
*                            value will be stored  
* @param  L7_uint32   level  level   
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t decode_nul(asnBitStr_t * bs, asnField_t * f, L7_uchar8 *base, L7_uint32 level)
{
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Decode the ASN Boolean type
*
* @param  asnBitStr_t *bs           Pointer to asnBitStr_t    
* @param  asnField_t  *f            Pointer to field  
* @param  L7_uchar8   *base         Pointer to address where the decoded
*                                   value will be stored
* @param  L7_uint32   level         level   
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t decode_bool(asnBitStr_t * bs, asnField_t * f, L7_uchar8 *base, L7_uint32 level)
{
  INC_BIT(bs);
  CHECK_BOUND(bs, 0);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Decode the ASN ID
* 
* @param  asnBitStr_t *bs           Pointer to asnBitStr_t    
* @param  asnField_t  *f            Pointer to field  
* @param  L7_uchar8   *base         Pointer to address where the decoded
*                                   value will be stored
* @param  L7_uint32   level         level   
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t decode_oid(asnBitStr_t * bs, asnField_t * f, L7_uchar8 *base, L7_uint32 level)
{
  L7_int32 len;

  BYTE_ALIGN(bs);
  CHECK_BOUND(bs, 1);
  len = *bs->cur++;
  bs->cur += len;

  CHECK_BOUND(bs, 0);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Decode ASN Integer
*
* @param  asnBitStr_t *bs           Pointer to asnBitStr_t    
* @param  asnField_t  *f            Pointer to field  
* @param  L7_uchar8   *bas          Pointer to address where the decoded
*                                   value will be stored 
* @param  L7_int32    level         level   
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t decode_int(asnBitStr_t * bs, asnField_t * f, L7_uchar8 *base, L7_uint32 level)
{
  L7_uint32 len;

  switch (f->sz) {
  case ASN_BYTE:        /* Range == 256 */
    BYTE_ALIGN(bs);
    bs->cur++;
    break;
  case ASN_WORD:        /* 257 <= Range <= 64K */
   BYTE_ALIGN(bs);
   bs->cur += 2;
  /*
    if (base && (f->attr & ASN_DECODE)) {
      L7_uint32 v = asnGet_uint(bs, len) + f->lb;
      *((L7_uint32 *) (base + f->offset)) = v;
    } else {
     bs->cur += 2;
  }
  */
    break;
  case ASN_CONS:        /* 64K < Range < 4G */
    len = asnGetBits(bs, 2) + 1;
    BYTE_ALIGN(bs);
    bs->cur += len;
    break;
  case ASN_UNCO:
    BYTE_ALIGN(bs);
    CHECK_BOUND(bs, 2);
    len = asnGetLen(bs);
    bs->cur += len;
    break;
  default:    /* 2 <= Range <= 255 */
    INC_BITS(bs, f->sz);
    break;
  }
  CHECK_BOUND(bs, 0);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Decode an ASN Enum
*
* @param  asnBitStr_t *bs           Pointer to asnBitStr_t    
* @param  asnField_t  *f            Pointer to field  
* @param  L7_uchar8   *base         Pointer to address where the decoded
*                                   value will be stored 
* @param  L7_uint32   level         level   
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t decode_enum(asnBitStr_t * bs, asnField_t * f, L7_uchar8 *base, L7_uint32 level)
{
  if ((f->attr & ASN_EXT) && asnGetBit(bs)) 
  {
    INC_BITS(bs, 7);
  } 
  else 
  {
    INC_BITS(bs, f->sz);
  }

  CHECK_BOUND(bs, 0);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Decode ANS BITSTring
*
* @param  asnBitStr_t *bs           Pointer to asnBitStr_t    
* @param  asnField_t  *f            Pointer to field  
* @param  L7_uchar8   *base         Pointer to address where the decoded
*                                   value will be stored 
* @param  L7_uint32   level         level   
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t decode_bitstr(asnBitStr_t * bs, asnField_t * f, L7_uchar8 *base, L7_uint32 level)
{
  L7_uint32 len;

  BYTE_ALIGN(bs);
  switch (f->sz) {
  case ASN_FIXD:        /* fixed length > 16 */
    len = f->lb;
    break;
  case ASN_WORD:        /* 2-byte length */
    CHECK_BOUND(bs, 2);
    len = (*bs->cur++) << 8;
    len += (*bs->cur++) + f->lb;
    break;
  case ASN_SEMI:
    CHECK_BOUND(bs, 2);
    len = asnGetLen(bs);
    break;
  default:
    len = 0;
    break;
  }

  bs->cur += len >> 3;
  bs->bit = len & 7;

  CHECK_BOUND(bs, 0);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Decode ASN NumStr
*
* @param  asnBitStr_t *bs           Pointer to asnBitStr_t    
* @param  asnField_t  *f            Pointer to field  
* @param  L7_uchar8   *base         Pointer to address where the decoded
*                                   value will be stored  
* @param  L7_uint32   level         level   
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t decode_numstr(asnBitStr_t * bs, asnField_t * f, L7_uchar8 *base, L7_uint32 level)
{
  L7_uint32 len;

  /* 2 <= Range <= 255 */
  len = asnGetBits(bs, f->sz) + f->lb;

  BYTE_ALIGN(bs);
  INC_BITS(bs, (len << 2));

  CHECK_BOUND(bs, 0);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Decode ASN OCTSTR
*
* @param  asnBitStr_t *bs           Pointer to asnBitStr_t    
* @param  asnField_t  *f            Pointer to field  
* @param  L7_uchar8   *base         Pointer to address where the decoded
*                                   value will be stored  
* @param  L7_uint32   level         level   
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t decode_octstr(asnBitStr_t * bs, asnField_t * f, L7_uchar8 *base, L7_uint32 level)
{
  L7_uint32 len;

  switch (f->sz) 
  {
    case ASN_FIXD:        /* Range == 1 */
    if (f->lb > 2) 
    {
      BYTE_ALIGN(bs);
      if (base && (f->attr & ASN_DECODE)) 
      {
        /* The IP Address, store the offset only */
        *((L7_uint32 *) (base + f->offset)) =
            bs->cur - bs->buf;
      }
    }
    len = f->lb;
    break;
  case ASN_BYTE:        /* Range == 256 */
    BYTE_ALIGN(bs);
    CHECK_BOUND(bs, 1);
    len = (*bs->cur++) + f->lb;
    break;
  case ASN_SEMI:
    BYTE_ALIGN(bs);
    CHECK_BOUND(bs, 2);
    len = asnGetLen(bs) + f->lb;
    break;
  default:    /* 2 <= Range <= 255 */
    len = asnGetBits(bs, f->sz) + f->lb;
    BYTE_ALIGN(bs);
    break;
  }

  bs->cur += len;

  CHECK_BOUND(bs, 0);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Decode ASN BMP STRING
*
* @param  asnBitStr_t *bs           Pointer to asnBitStr_t    
* @param  asnField_t  *f            Pointer to field  
* @param  L7_uchar8   *base         Pointer to address where the decoded
*                                   value will be stored  
* @param  L7_uint32   level         level   
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t decode_bmpstr(asnBitStr_t * bs, asnField_t * f, L7_uchar8 *base, L7_uint32 level)
{
  L7_uint32 len;

  switch (f->sz) 
  {
    case ASN_BYTE:        /* Range == 256 */
      BYTE_ALIGN(bs);
      CHECK_BOUND(bs, 1);
      len = (*bs->cur++) + f->lb;
      break;
  default:    /* 2 <= Range <= 255 */
      len = asnGetBits(bs, f->sz) + f->lb;
      BYTE_ALIGN(bs);
      break;
  }

  bs->cur += len << 1;
  CHECK_BOUND(bs, 0);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Decode ASN Sequence ( Sequence is similar to array )
*          that is it represents an ordered set of simliar types
*
* @param  asnBitStr_t *bs           Pointer to asnBitStr_t    
* @param  asnField_t  *f            Pointer to field  
* @param  L7_uchar8   *base         Pointer to address where the decoded
*                                   value will be stored
* @param  L7_uint32   level         level   
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t decode_seq(asnBitStr_t * bs, asnField_t * f, L7_uchar8 *base, L7_uint32 level)
{
  L7_uint32 ext, bmp, i, opt, len = 0, bmp2, bmp2_len;
  L7_RC_t err = L7_FAILURE;
  asnField_t *son;
  L7_uchar8 *beg = L7_NULLPTR;

  /* Decode? */
  base = (base && (f->attr & ASN_DECODE)) ? base + f->offset : L7_NULLPTR;

  /* Extensible? */
  ext = (f->attr & ASN_EXT) ? asnGetBit(bs) : 0;

  /* Get fields bitmap */
  bmp = asnGetBitmap(bs, f->sz);
  if (base)
    *(L7_uint32 *) base = bmp;

  /* Decode the root components */
  for (i = opt = 0, son = f->fields; i < f->lb; i++, son++) 
  {
    if (son->attr & ASN_STOP) 
    {
      /* NOT AN ERROR CONDITION */
      return L7_SUCCESS;
    }

    if (son->attr & ASN_OPT) 
    {  /* Optional component */
      if (!((0x80000000U >> (opt++)) & bmp))  /* Not exist */
        continue;
    }

    /* Decode */
    if (son->attr & ASN_OPEN) 
    { /* Open field */
      CHECK_BOUND(bs, 2);
      len = asnGetLen(bs);
      CHECK_BOUND(bs, len);
      if (!base) 
      {
        bs->cur += len;
        continue;
      }
      beg = bs->cur;

      /* Decode */
      if ((err = (decoders[son->type]) (bs, son, base,level + 1)) != L7_SUCCESS)
        return err;

      bs->cur = beg + len;
      bs->bit = 0;
    } 
    else if ((err = (decoders[son->type]) (bs, son, base,level + 1)) != L7_SUCCESS)
      return err;
  }

  /* No extension? */
  if (!ext)
    return L7_SUCCESS;

  /* Get the extension bitmap */
  bmp2_len = asnGetBits(bs, 7) + 1;
  CHECK_BOUND(bs, (bmp2_len + 7) >> 3);
  bmp2 = asnGetBitmap(bs, bmp2_len);
  bmp |= bmp2 >> f->sz;
  if (base)
    *(L7_uint32 *) base = bmp;
  BYTE_ALIGN(bs);

  /* Decode the extension components */
  for (opt = 0; opt < bmp2_len; opt++, i++, son++) 
  {
    if (son->attr & ASN_STOP) 
    {
      /* NOT AN ERROR CONDITION */
      return L7_SUCCESS;
    }

    if (!((0x80000000 >> opt) & bmp2))      /* Not present */
      continue;

    /* Check Range */
    if (i >= f->ub) 
    {       /* Newer Version? */
      CHECK_BOUND(bs, 2);
      len = asnGetLen(bs);
      CHECK_BOUND(bs, len);
      bs->cur += len;
      continue;
    }

    CHECK_BOUND(bs, 2);
    len = asnGetLen(bs);
    CHECK_BOUND(bs, len);
    if (!base || !(son->attr & ASN_DECODE)) 
    {
      bs->cur += len;
      continue;
    }
    beg = bs->cur;

    if ((err = (decoders[son->type]) (bs, son, base,level + 1)) != L7_SUCCESS)
      return err;

    bs->cur = beg + len;
    bs->bit = 0;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose To decode ASN SEQ OF
*
* @param  asnBitStr_t *bs           Pointer to asnBitStr_t    
* @param  asnField_t  *f            Pointer to field  
* @param  L7_uchar8   *base         Pointer to address where the decoded
*                                   value will be stored  
* @param  L7_uint32   level         level   
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t decode_seqof(asnBitStr_t * bs, asnField_t * f, L7_uchar8 *base, L7_uint32 level)
{
  L7_uint32 count, effective_count = 0, i, len = 0;
  L7_RC_t err = L7_FAILURE;
  asnField_t *son;
  L7_uchar8 *beg = L7_NULLPTR;

  /* Decode? */
  base = (base && (f->attr & ASN_DECODE)) ? base + f->offset : L7_NULLPTR;

  /* Decode item count */
  switch (f->sz) {
  case ASN_BYTE:
    BYTE_ALIGN(bs);
    CHECK_BOUND(bs, 1);
    count = *bs->cur++;
    break;
  case ASN_WORD:
    BYTE_ALIGN(bs);
    CHECK_BOUND(bs, 2);
    count = *bs->cur++;
    count <<= 8;
    count = *bs->cur++;
    break;
  case ASN_SEMI:
    BYTE_ALIGN(bs);
    CHECK_BOUND(bs, 2);
    count = asnGetLen(bs);
    break;
  default:
    count = asnGetBits(bs, f->sz);
    break;
  }
  count += f->lb;

  /* Write Count */
  if (base) 
  {
    effective_count = count > f->ub ? f->ub : count;
    *(L7_uint32 *) base = effective_count;
    base += sizeof(L7_uint32);
  }
  /* Decode nested field */
  son = f->fields;
  if (base)
    base -= son->offset;
  for (i = 0; i < count; i++) 
  {
    if (son->attr & ASN_OPEN) 
    {
      BYTE_ALIGN(bs);
      len = asnGetLen(bs);
      CHECK_BOUND(bs, len);
      if (!base || !(son->attr & ASN_DECODE)) 
      {
        bs->cur += len;
        continue;
      }
      beg = bs->cur;

      if ((err = (decoders[son->type]) (bs, son,i < effective_count ? base : L7_NULLPTR,level + 1)) != L7_SUCCESS)
        return err;

      bs->cur = beg + len;
      bs->bit = 0;
    } 
    else  if ((err = (decoders[son->type]) (bs, son,
                  i < effective_count ? base : L7_NULLPTR, level + 1))!= L7_SUCCESS)
      return err;

    if (base)
      base += son->offset;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To decode ASN CHOICE ( choice is simlar to union)
*
* @param  asnBitStr_t *bs           Pointer to asnBitStr_t    
* @param  asnField_t  *f            Pointer to field  
* @param  L7_uchar8   *base         Pointer to address where the decoded
*                                   value will be stored  
* @param  L7_uint32   level         level   
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t decode_choice(asnBitStr_t * bs, asnField_t * f, L7_uchar8 *base, L7_uint32 level)
{
  L7_uint32 type, ext, len = 0;
  L7_RC_t err  = L7_FAILURE;
  asnField_t *son;
  L7_uchar8 *beg = L7_NULLPTR;

  /* Decode? */
  base = (base && (f->attr & ASN_DECODE)) ? base + f->offset : L7_NULLPTR;

  /* Decode the choice index number */
  if ((f->attr & ASN_EXT) && asnGetBit(bs)) 
  {
    ext = 1;
    type = asnGetBits(bs, 7) + f->lb;
  } 
  else 
  {
    ext = 0;
    type = asnGetBits(bs, f->sz);
  }

  /* Check Range */
  if (type >= f->ub) 
  {    /* Newer version? */
    BYTE_ALIGN(bs);
    len = asnGetLen(bs);
    CHECK_BOUND(bs, len);
    bs->cur += len;
    return L7_SUCCESS;
  }

  /* Write Type */
  if (base)
    *(L7_uint32 *) base = type;

  /* Transfer to son level */
  son = &f->fields[type];
  if (son->attr & ASN_STOP) 
  {
     /* NOT AN ERROR CONDITION */
     return L7_SUCCESS;
  }
  if (ext || (son->attr & ASN_OPEN)) 
  {
    BYTE_ALIGN(bs);
    len = asnGetLen(bs);
    CHECK_BOUND(bs, len);
    if (!base || !(son->attr & ASN_DECODE)) 
    {
      bs->cur += len;
      return L7_SUCCESS;
    }
    beg = bs->cur;

    if ((err = (decoders[son->type]) (bs, son, base, level + 1)) != L7_SUCCESS)
      return err;

    bs->cur = beg + len;
    bs->bit = 0;
  }
  else if ((err = (decoders[son->type]) (bs, son, base, level + 1)) != L7_SUCCESS)
    return err;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To decode H323 user Information
*
* @param  L7_uchar8   *buf   Pointer to packet data
* @param  L7_uchar8   *beg   Pointer to UUIE data
* @param  L7_uint32   sz     Length of UUIE
* @param  h323UserInfo_t     *uuie Pointer to addr where
*         decoded information will be stored
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t decodeH323UserInformation(L7_uchar8 *buf, L7_uchar8 *beg,
                                      L7_uint32 sz, h323UserInfo_t *uuie)
{
  static asnField_t h323_userinformation = {
    FNAME("H323-UserInformation") ASN_SEQ, 1, 2, 2, ASN_DECODE | ASN_EXT,
    0, h323UserInformation
  };
  asnBitStr_t bs;

  bs.buf = buf;
  bs.beg = bs.cur = beg;
  bs.end = beg + sz;
  bs.bit = 0;

  return decode_seq(&bs, &h323_userinformation, (L7_uchar8 *) uuie, 0);
}
/*********************************************************************
* @purpose  To decode MultiMedia system control message 
*
* @param  L7_uchar8   *buf   Pointer to packet data
* @param  L7_uint32   sz     Size
* @param  mediaMessage_t     *mscm Pointer to addr where
*         decoded information will be stored
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t decodeMediaControlMessage(L7_uchar8 *buf, L7_uint32 sz,
                                         mediaMessage_t *mscm)
{
  static asnField_t multimediaMessage = {
    FNAME("MultimediaSystemControlMessage") ASN_CHOICE, 2, 4, 4,
    ASN_DECODE | ASN_EXT, 0, mediaMessage
  };
  asnBitStr_t bs;

  bs.buf = bs.beg = bs.cur = buf;
  bs.end = buf + sz;
  bs.bit = 0;

  return decode_choice(&bs, &multimediaMessage,
                       (L7_uchar8 *) mscm, 0);
}
/*********************************************************************
* @purpose  To decode Q931 Message
*
* @param  L7_uchar8   *buf   Pointer to Q931 Message
* @param  L7_uint32   sz     Length of data
* @param  Q931_t      *q931  Pointer to addr where
*         decoded information will be stored
*
* @return   L7_SUCCESS | L7_FAILURE
*
* @notes  Q931 Message consists of 
*   	a) protocol discriminator;
*	b) call reference;
*	c) message type;
*	d) other information elements, as required.
*
* @end
*********************************************************************/
L7_int32 decodeQ931(L7_uchar8 *buf, L7_uint32 sz, Q931_t *q931)
{
  L7_uchar8 *p = buf;
  L7_int32 len;

  if (p == L7_NULLPTR || sz < 1)
    return H323_ERROR_BOUND;

  /* Protocol Discriminator */
  /* 0 0 0 0 1 0 0 0 Recommendation Q.931/I.451 user-network call control messages */
  if (*p != 0x08) 
  {
    VOIP_TRACE(VOIP_DEBUG_ERROR,VOIP_PROTO_H323,"Unknown Protocol Discriminator");
    return H323_ERROR_RANGE;
  }
  p++;
  sz--;

  /* CallReferenceValue */
  if (sz < 1)
  {
    VOIP_TRACE(VOIP_DEBUG_ERROR,VOIP_PROTO_H323,"CallReferenceValue not present");
    return H323_ERROR_BOUND;
  }
  len = *p++;
  sz--;
  if (sz < len)
  {
    VOIP_TRACE(VOIP_DEBUG_ERROR,VOIP_PROTO_H323,"Packet Parsing error");
    return H323_ERROR_BOUND;
  }
  p += len;
  sz -= len;

  /* Message Type */
  if (sz < 1)
  {
    VOIP_TRACE(VOIP_DEBUG_ERROR,VOIP_PROTO_H323,"Message Type not present"); 
    return H323_ERROR_BOUND;
  }
  /* store the message Type */
  q931->messageType = *p++;
  
  /*A protocol discriminator field is also included in the User-user information 
    element to indicate the user protocol within the user information;*/

  if (*p & 0x80) 
  {
    p++;
    sz--;
  }

  /* Decode Information Elements */
  while (sz > 0) 
  {
    /*  Variable length information element format 
        1)Information element identifier 
        2)Length of contents of information element (octets) 2
        3)Contents of information element 3 */

    /* 0 1 1 1 1 1 1 0 User-user Q.931 */
    if (*p == 0x7e) 
    {       
      /* UserUserIE */
      if (sz < 3)
        break;
      p++;
      len = *p++ << 8;
      len |= *p++;
      sz -= 3;
      if (sz < len)
        break;
      p++;
      len--;
      return decodeH323UserInformation(buf, p, len,&q931->UUIE);
    }
    p++;
    sz--;
    if (sz < 1)
      break;
    len = *p++;
    if (sz < len)
      break;
    p += len;
    sz -= len;
  }
  VOIP_TRACE(VOIP_DEBUG_ERROR,VOIP_PROTO_H323,"Q.931 UUIE not found");
  return H323_ERROR_BOUND;
}

/*********************************************************************
* @purpose TPKTs contained in message
*
* @param   L7_uchar8   *buf      Pointer to packet data
* @param   connInfo_t  *info     Pointer to connInfo_t
* @param   L7_uint32   tcpdatalen  Length of TCP data
* @param   L7_uint32   tcpdataoff  offset at which TCP data starts
* @param   L7_uchar8   **data      Pointer to TPKT data
* @param   L7_uint32   *datalen    Pointer to Length of TPKT data
* @param   L7_uint32   *dataoff    offset at which TPKT data starts
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
getTpktData(L7_uchar8 *buf, connInfo_t *info, L7_uint32 tcpdatalen, L7_uint32 tcpdataoff,
            L7_uchar8 **data, L7_uint32 *datalen, L7_uint32 *dataoff)
{
  L7_uchar8 *tpkt;
  L7_uint32 tpktlen;
  L7_uint32 tpktoff;

  if (*data == L7_NULLPTR) 
  { 
    /* first TPKT */
    /* Get first TPKT pointer */
    tpkt = (L7_uchar8 *)(buf + tcpdataoff);

    if (tpkt == L7_NULLPTR) 
     {
        VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_H323, "No TPKT Information in the packet");
        return L7_FAILURE;
     }
    /* Validate TPKT identifier */
    if (tcpdatalen < 4 || tpkt[0] != 0x03 || tpkt[1] != 0) 
    {
      /* Netmeeting sends TPKT header and data separately */
      if (info->tpktlen[info->dir] > 0) 
      {
         if (info->tpktlen[info->dir] <= tcpdatalen) 
         {
           *data = tpkt;
           *datalen = info->tpktlen[info->dir];
           *dataoff = 0;
            goto out;
         }

         /* Fragmented TPKT */
         goto clear_out;
      }

      /* It is not even a TPKT */
      return L7_FAILURE;
    }
    tpktoff = 0;
  } 
  else 
  {    /* Next TPKT */
    tpktoff = *dataoff + *datalen;
    tcpdatalen -= tpktoff;
    if (tcpdatalen <= 4)    /* No more TPKT */
      goto clear_out;
    tpkt = *data + *datalen;


    /* Validate TPKT identifier */
    if (tpkt[0] != 0x03 || tpkt[1] != 0)
      goto clear_out;
  }

  /* Validate TPKT length */
  tpktlen = tpkt[2] * 256 + tpkt[3];
  if (tpktlen > tcpdatalen) 
  {
    if (tcpdatalen == 4) 
    {  
      /* Separate TPKT header */
      /* Netmeeting sends TPKT header and data separately */
      info->tpktlen[info->dir] = tpktlen - 4;
      return L7_FAILURE;
    }

    goto clear_out;
  }

  /* This is the encapsulated data */
  *data = tpkt + 4;
  *datalen = tpktlen - 4;
  *dataoff = tpktoff + 4;

out:
  /* Clear TPKT length */
  info->tpktlen[info->dir] = 0;
  return L7_SUCCESS;

clear_out:
  info->tpktlen[info->dir] = 0;
  return L7_FAILURE;
}

/*********************************************************************
* @purpose Enable/disable a H245 session
*
* @param   connInfo_t   *info   Pointer to connInfo_t
* @param   L7_uint32 enable         L7_ENABLE | L7_DISABLE
* @param   L7_uint32 interface      interface number
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipSetupH245Session(connInfo_t *info,L7_uint32 enable)
{
   L7_RC_t rc =L7_FAILURE;
   L7_uint32   srcIp;
   L7_uint32   dstIp; 
   L7_ushort16 srcL4port;
   L7_ushort16 dstL4port;
   L7_char8  strSrcIPaddr[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  strDstIPaddr[L7_CLI_MAX_STRING_LENGTH];
 
   /* For H245 session source and destination are same */
   srcIp = info->h245_ip;
   dstIp = info->h245_ip;
   srcL4port = info->h245_port;
   dstL4port = info->h245_port;
   rc = dtlVoipProfileParamSet(enable,
                               L7_QOS_VOIP_PROTOCOL_H245,
                               srcIp,
                               dstIp,
                               srcL4port,
                               dstL4port); 
   if (rc == L7_SUCCESS)
   {
    /* Log the event */
    usmDbInetNtoa(srcIp, strSrcIPaddr);
    usmDbInetNtoa(dstIp, strDstIPaddr);

    LOG_MSG("Set up new H245 Session\r\n Source IP = %s, Dest IP = %s\r\n"
             "Source Port = %d, Dest Port = %d\r\n", 
             strSrcIPaddr, strDstIPaddr,
             srcL4port,dstL4port);
   }
   return rc;
}

/*********************************************************************
* @purpose Enable/disable RTP Session  
*
* @param   connInfo_t   *info   Pointer to connInfo_t
* @param   L7_uint32 enable         L7_ENABLE | L7_DISABLE
* @param   L7_uint32 interface      interface number
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
voipSetupRTPSession(connInfo_t *info,L7_uint32 enable)
{
   L7_RC_t     rc =L7_FAILURE; 
   L7_uint32   srcIp;
   L7_uint32   dstIp;
   L7_ushort16 srcL4port;
   L7_ushort16 dstL4port;
   L7_char8  strSrcIPaddr[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  strDstIPaddr[L7_CLI_MAX_STRING_LENGTH];

   /* For RTP session  srcip contains entries at [0] in media_ip and destn [1]*/
  
   srcIp = info->media_ip[0];
   dstIp = info->media_ip[1];
   
   srcL4port = info->media_port[0];
   dstL4port = info->media_port[1];    

   rc = dtlVoipProfileParamSet(enable,L7_QOS_VOIP_PROTOCOL_H323,
                          srcIp,dstIp,srcL4port,dstL4port); 
   if (rc == L7_SUCCESS)
   {
     if (enable == L7_ENABLE)
       voipTimerAdd();
     else if (enable == L7_DISABLE)
      voipTimerDelete();

    /* Log the event */
    usmDbInetNtoa(srcIp, strSrcIPaddr);
    usmDbInetNtoa(dstIp, strDstIPaddr);

    LOG_MSG("Set up new RTP Session\r\n Source IP = %s, Dest IP = %s\r\n"
            "Source Port = %d, Dest Port = %d\r\n", 
            strSrcIPaddr, strDstIPaddr,
            srcL4port,dstL4port);

   }

   return rc;
}

/*********************************************************************
* @purpose Extracts Ip address and port from data 
*
* @param   L7_uchar8 *data   Pointer to data 
* @param   H245TransportAddr_t *addr Pointer to H245TransportAddr_t
* @param   L7_uint32 *ip     Pointer to Ip Address
* @param   L7_ushort16 *port Pointer to port
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
getH245Addr(L7_uchar8 *data,H245TransportAddr_t *addr, L7_uint32 *ip, L7_ushort16 *port)
{
  L7_uchar8 *p = L7_NULLPTR;

  if (addr->choice != transportAddr_unicastAddr ||
      addr->unicastAddr.choice != unicastAddr_ipAddr)
    return L7_FAILURE;

  p = data + addr->unicastAddr.ipAddr.network;
  *ip = ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]));
  *port = (p[4] << 8) | (p[5]);

  return L7_SUCCESS; 

}
/*********************************************************************
* @purpose  Expect RTP RTCP Parameters  
*
* @param   connInfo_t       *ctinfo Pointer to connInfo_t
* @param   L7_uchar8        **data Pointer to packet data
* @param   H2250LogParams_t *channel Pointer to H2250LogParams_t
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t expectRtpRtcp(connInfo_t *ctinfo, L7_uchar8 **data, H245TransportAddr_t *addr)
{
  L7_RC_t rv = L7_SUCCESS;

  
  
  if (getH245Addr(*data, addr,&(ctinfo->media_ip[ctinfo->dir]),
       &(ctinfo->media_port[ctinfo->dir])) == L7_FAILURE) 
  {
    VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_H323, "Failed to get H245 IP Address");
    return L7_FAILURE;
  }
  

  if (ctinfo->media_port[0] != 0 || ctinfo->media_port[1] != 0) 
  {
    if (ctinfo->h245_SessionStatus == L7_TRUE)
    {
      rv = voipSetupH245Session(ctinfo, L7_DISABLE);
      if (rv != L7_SUCCESS) 
      {
        VOIP_TRACE(VOIP_DEBUG_ERROR,VOIP_PROTO_H323,"voipSetupH245Session(disable) failed\n");
      }
    }
    if (ctinfo->rtp_SessionStatus == L7_FALSE)
    {
      rv = voipSetupRTPSession(ctinfo, L7_ENABLE);
      if (rv != L7_SUCCESS) 
      {
        VOIP_TRACE(VOIP_DEBUG_ERROR,VOIP_PROTO_H323,"voipSetupRTPSession(enable) failed\n");
      }
    }
    ctinfo->rtp_SessionStatus = L7_TRUE; 
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Process H2250 openLogicalChannel Parameters 
*
* @param   connInfo_t       *ctinfo Pointer to connInfo_t
* @param   L7_uchar8        **data Pointer to packet data
* @param   L7_uint32        dataoff
* @param   H2250LogParams_t *channel Pointer to H2250LogParams_t
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t processH245Channel(connInfo_t *ctinfo,
                                L7_uchar8 **data, L7_uint32 dataoff,
                                H2250LogParams_t *channel)
{
 L7_RC_t ret;

 if (channel->options & H2250LogParams_mediaChannel) 
 {
    /* RTP */
    ret = expectRtpRtcp(ctinfo, data, &channel->mediaChannel);
    if (ret != L7_SUCCESS)
      return L7_FAILURE;
 }

/* RTCP */
/*
        if (channel->
            options & eH2250LogicalChannelParameters_mediaControlChannel) {

                ret = expect_rtp_rtcp(ctinfo, data, &channel->mediaControlChannel);
                if (ret < 0)
                        return -1;
        }
*/

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Process openLogicalChannel info contained in H245 Request
*          multimedia Message
* 
* @param   connInfo_t      *ctinfo Pointer to connInfo_t
* @param   L7_uchar8       **data Pointer to packet data
* @param   L7_uint32       dataoff
* @param   openLogicalChannel_t *olc  Pointer to openLogicalChannel 
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t processOlc(connInfo_t *ctinfo,L7_uchar8 **data,L7_uint32 dataoff,
                          openLogicalChannel_t *olc)
{
  L7_RC_t ret;

  /* Dealing with forwarding Multiplex Parameters */
  if (olc->fwdLogParams.multiplexParams.choice == 
      openLogFwdMpexParams_h2250Params)
  {
    ret = processH245Channel(ctinfo, data, dataoff,
             &olc->fwdLogParams.multiplexParams.h2250LogParams);
    if (ret != L7_SUCCESS)
      return L7_FAILURE;
  }
  /* Dealing with Reverse Multiplex Parameters */
  if ((olc->options & openLogParam_revParams) &&
      (olc->revLogParams.options & openLogRevParams_multiplexParams)&&
      (olc->revLogParams.multiplexParams.choice == openLogRevMpexParams_h2250Params))
  {
       ret = processH245Channel(ctinfo, data, dataoff,
              &olc->
              revLogParams.multiplexParams.h2250LogParams);
    if (ret != L7_SUCCESS)
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Process openLogicalChannel ACK info contained in H245 
*          Response multimedia Message
*
* @param   connInfo_t      *ctinfo Pointer to connInfo_t
* @param   L7_uchar8       **data Pointer to packet data
* @param   L7_uint32       dataoff
* @param   openLogAckParams_t *olc openLogAckParams_t
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t processOlca(connInfo_t *ctinfo,L7_uchar8 **data, L7_uint32 dataoff,
                           openLogAckParams_t *olca)
{
  H2250LogAckParams_t *ack;
  L7_RC_t ret;

  if ((olca->options & openLogAckParams_reverseParams) &&
      (olca->reverseParams.options & openLogAckRevParams_multiplexParams)&&
      (olca->reverseParams.multiplexParams.choice == openLogAckRevMpexParams_h2250Params))
  {
    ret = processH245Channel(ctinfo, data, dataoff,
             &olca->reverseParams.multiplexParams.h2250LogParams);
    if (ret != L7_SUCCESS)
      return L7_FAILURE;
  }

  if ((olca->options & 	openLogAckParams_forwardMpexAckParams ) &&
      (olca->fwdMpexParams.choice == openLogAckFwdMpexParams_h2250Params))
  {
    ack = &olca->fwdMpexParams.H2250LogAckParams;

    if (ack->options & H2250LogAckParams_mediaChannel) 
    {
      /* RTP */
       ret = expectRtpRtcp(ctinfo, data, &ack->mediaChannel);
       if (ret != L7_SUCCESS)
         return L7_FAILURE;
    }
/* Don't have to deal with RTCP information */
/*
    if (ack->options &
        eH2250LogicalChannelAckParameters_mediaControlChannel) {
      ret = expect_rtp_rtcp(ctinfo, data, &ack->mediaControlChannel);
      if (ret < 0)
        return -1;
    }
*/
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Process H245 Message We are interested in Request/Response
*          message only
* @param   connInfo_t      *ctinfo Pointer to connInfo_t
* @param   L7_uchar8       **data Pointer to packet data
* @param   L7_uint32       dataoff
* @param   mediaMessage_t *mscm Pointer to Media message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t processH245(connInfo_t *ctinfo, L7_uchar8 **data, L7_uint32 dataoff,
                        mediaMessage_t *mscm)
{
  switch (mscm->choice) 
  {
    case mediaMessage_request:
      if (mscm->u.request.choice == req_openLogicalChannel) 
      {
        return processOlc(ctinfo, data, dataoff,
              &mscm->u.request.openLogicalChannel);
      }
      break;
    case mediaMessage_response:
      if (mscm->u.response.choice == resp_openLogicalChannelAck) 
       {
        return processOlca(ctinfo, data, dataoff,
        &mscm->u.response.openLogAckParams);
      }
      break;
   default:
     break;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Parse H245 message
*
* @param   const L7_uchar8 *buf      Pointer to Packet data
* @param   L7_uint32       length    length of message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes   H245 message is ASN.1 encoded and needs to be docoded first
*          before parsing
*
* @end
*********************************************************************/
L7_RC_t voipH245MessageParse(const L7_uchar8 *packetBuf,L7_uint32 length)
{
  static mediaMessage_t mscm;
  L7_uchar8 *data = L7_NULLPTR;
  L7_uchar8 *tmp;
  L7_uint32 datalen;
  L7_uint32 dataoff,offset=0;
  L7_uint32 ret;
  connPair_t pair_info;
  L7_ipHdr_t *iph;
  L7_tcpHdr_t *tcph;
  voipH323Call_t *entry;
  L7_uint32 iphdrlen, tcpdataoff, tcpdatalen;


  offset = sysNetDataOffsetGet((L7_uchar8 *)packetBuf);
  tmp = (L7_uchar8 *)(packetBuf + 18);
  /* Get TCP data offset = IP header len + TCP header len */
  iph = (L7_ipHdr_t *)tmp;
  
  iphdrlen = (osapiNtohs(iph->_v_hl_tos) >> 8) & 0x0f; 
  tcph = (L7_tcpHdr_t *)(tmp+iphdrlen*4);
  tcpdataoff = iphdrlen + (osapiNtohs(tcph->_len) >> 12);
  tcpdataoff <<= 2;

  /* Get TCP data length */
  tcpdatalen = osapiNtohs(iph->_len) - tcpdataoff; 

  if (tcpdatalen <= 0)    /* No TCP data */
  {
    return L7_FAILURE;
  }

  pair_info.src_ip = osapiNtohl(iph->_src_ip);
  pair_info.dst_ip = osapiNtohl(iph->_dst_ip);
  pair_info.src_port = osapiNtohs(tcph->_src_port);
  pair_info.dst_port = osapiNtohs(tcph->_dst_port);

  ret = voipH245CallLookup(&voipH323CallList, &pair_info, &entry);
  if (ret == L7_FAILURE) 
  {
    VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_H323,"H245 call lookupfailed. should not happen");
    return L7_FAILURE;
  }
  /* Process each TPKT */
  while (getTpktData(tmp, &(entry->ct_info), tcpdatalen, tcpdataoff, &data, 
                     &datalen, &dataoff) == L7_SUCCESS ) 
  {
    /* Decode H.245 signal */
    ret = decodeMediaControlMessage(data, datalen,&mscm);
    if (ret != L7_SUCCESS) 
    {
       VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_H323,"h245: decoding error: %d\n",
                       ret == L7_ERROR ? "L7_ERROR" : "L7_FAILURE");
       /* We don't drop when decoding error */
       break;
    }
    /* Process H.245 signal */
    if (processH245(&(entry->ct_info), &data, dataoff, &mscm) != L7_SUCCESS)
    {
      VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_H323,"Failed to process H245 packet");
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get H245 Address from data 
*
* @param    L7_uchar8    *data  Pointer to packet data
* @param    transportAddr_t  *addr Pointer to address
* @param    L7_uint32  *ip   Pointer to IP Address
* @param    L7_ushort16 *port Pointer to port
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/ 
L7_RC_t getH225Addr(L7_uchar8 *data, transportAddr_t  *addr,
                     L7_uint32 *ip, L7_ushort16 *port)
{
  L7_uchar8 *p;

  if (addr->choice != transportAddr_ipAddr)
     return L7_FAILURE;

  p = data + addr->ipAddr.ip;
  *ip = ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]));
  *port = (p[4] << 8) | (p[5]);

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Expect H245 Transport Address
*
* @param    connInfo_t   *ctinfo Pointer to connInfo_t
* @param    L7_uchar8    *data  Pointer to  packet data
* @param    L7_uint32    dataoff
* @param    transportAddr_t  *addr Pointer to address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t expectH245(connInfo_t *ctinfo, L7_uchar8 **data, L7_uint32 dataoff,
                transportAddr_t * addr)
{
  L7_RC_t rv=L7_SUCCESS;
  /* Read h245Address */
  if (getH225Addr(*data, addr, &(ctinfo->h245_ip), &(ctinfo->h245_port)) == L7_FAILURE)
  {
     VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_H323,"Failed to get H245 Address");
     return L7_FAILURE;
  } 

  if ((ctinfo->tunneling == L7_FALSE)&& (ctinfo->h245_SessionStatus == L7_FALSE))
  {
    rv = voipSetupH245Session(ctinfo, L7_ENABLE);
    if (rv != L7_SUCCESS) 
    {
      VOIP_TRACE(VOIP_DEBUG_ERROR,VOIP_PROTO_H323,"Setup_H245_Session(enable) failed\n");
      return rv;
    }
    ctinfo->h245_SessionStatus = L7_TRUE;
  }
 return rv;
}
/*********************************************************************
* @purpose Process Q931 Setup UUIE
*
* @param   connInfo_t      *ctinfo Pointer to connInfo_t
* @param   L7_uchar8       **data Pointer to packet data
* @param   L7_uint32       dataoff
* @param   setupUUIE_t     *setup Pointer to setup User-User Infor-
*          mation element
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t processSetup(connInfo_t *ctinfo, L7_uchar8 **data, L7_uint32 dataoff,
                            setupUUIE_t *setup)
{
  L7_RC_t ret;
  L7_uint32 i;


  if (setup->options & setup_h245Addr)
  {
     ret = expectH245(ctinfo, data, dataoff, 	&setup->h245Addr);
     if (ret != L7_SUCCESS)
        return L7_FAILURE;
  }
  if (setup->options & setup_fastStart) 
  {
    for (i = 0; i < setup->faststart.count; i++) 
    {
      ret = processOlc(ctinfo, data, dataoff, &setup->faststart.item[i]);
     if (ret != L7_SUCCESS)
        return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Process Q931 call Proceeding UUIE
*
* @param   connInfo_t      *ctinfo Pointer to connInfo_t
* @param   L7_uchar8       **data Pointer to packet data
* @param   L7_uint32       dataoff
* @param   callUUIE_t      *callproc Pointer to call User-User Infor-
*          mation element
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t processCall(connInfo_t *ctinfo,L7_uchar8 **data, L7_uint32 dataoff,
                                     callUUIE_t *callproc)
{
  L7_RC_t ret;
  L7_uint32 i;
        

  if (callproc->options & call_h245Addr) 
  {
     ret = expectH245(ctinfo, data, dataoff, &callproc->h245Addr);
     if (ret != L7_SUCCESS)
        return L7_FAILURE;

  }
  if (callproc->options & call_fastStart) 
  {
    for (i = 0; i < callproc->fastStart.count; i++) 
    {
      ret = processOlc(ctinfo, data, dataoff, &callproc->fastStart.item[i]);
      if (ret != L7_SUCCESS)
         return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Process Q931 connect UUIE
*
* @param   connInfo_t      *ctinfo Pointer to connInfo_t
* @param   L7_uchar8       **data Pointer to packet data
* @param   L7_uint32       dataoff
* @param   connectUUIE_t   *connect Pointer to connect User-User Infor-
*          mation element
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t processConnect(connInfo_t *ctinfo,L7_uchar8 **data, L7_uint32 dataoff,
                               connectUUIE_t * connect)
{
  L7_RC_t ret;
  L7_uint32 i;


  if (connect->options & connect_h245Addr) 
  {
    ret = expectH245(ctinfo, data, dataoff, &connect->h245Addr);
    if (ret != L7_SUCCESS)
    {
      VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_H323,"expectH245 failed");
      return L7_FAILURE;
    }
  }
  if (connect->options & connect_fastStart) 
  {
    for (i = 0; i < connect->fastStart.count; i++) 
    {
       ret = processOlc(ctinfo, data, dataoff,&connect->fastStart.item[i]);
       if (ret != L7_SUCCESS)
          return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Process Q931 alerting UUIE
*
* @param   connInfo_t      *ctinfo Pointer to connInfo_t
* @param   L7_uchar8       **data Pointer to packet data
* @param   L7_uint32       dataoff
* @param   alertingUUIE_t  *alert  Pointer to alerting User-User Infor-
*          mation element
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t processAlerting(connInfo_t *ctinfo,L7_uchar8 **data, L7_uint32 dataoff,
                            alertingUUIE_t * alert)
{
  L7_RC_t ret;
  L7_uint32 i;
        

  if (alert->options & alerting_h245Addr) 
  {
     ret = expectH245(ctinfo, data, dataoff, &alert->h245Addr);
     if (ret != L7_SUCCESS)
       return L7_FAILURE;
  }
  if (alert->options & alerting_fastStart) 
  {
     for (i = 0; i < alert->fastStart.count; i++) 
     {
        ret = processOlc(ctinfo, data, dataoff, &alert->fastStart.item[i]);
        if (ret != L7_SUCCESS)
          return L7_FAILURE;
     }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Process Q931 information UUIE
*
* @param   connInfo_t      *ctinfo Pointer to connInfo_t
* @param   L7_uchar8       **data Pointer to packet data
* @param   L7_uint32       dataoff
* @param   informationUUIE_t *info Pointer to informaiton User-User 
*                             Information element
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t processInformation(connInfo_t *ctinfo, L7_uchar8 **data, L7_uint32 dataoff,
                               informationUUIE_t * info)
{
  L7_RC_t ret;
  L7_uint32 i;
  
  if (info->options & information_fastStart) 
  {
     
     for (i = 0; i < info->fastStart.count; i++) 
     {
       ret = processOlc(ctinfo, data, dataoff, &info->fastStart.item[i]);
       if (ret != L7_SUCCESS)
          return ret;
     }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Process Q931 Facility UUIE
*
* @param   connInfo_t      *ctinfo Pointer to connInfo_t
* @param   L7_uchar8       **data Pointer to packet data
* @param   L7_uint32       dataoff
* @param   facilityUUIE_t  *facility Pointer to facility User-User
*                           information element
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t processFacility(connInfo_t *ctinfo, L7_uchar8 **data, L7_uint32 dataoff,
                              facilityUUIE_t * facility)
{
  L7_RC_t ret;
  L7_uint32 i;

 
  if (facility->options & facility_h245Addr) 
  {
     ret = expectH245(ctinfo, data, dataoff, &facility->h245Addr);
     if (ret != L7_SUCCESS)
       return L7_FAILURE;
   }
  if (facility->options & facility_fastStart) 
  {
    for (i = 0; i < facility->fastStart.count; i++) 
    {
      ret = processOlc(ctinfo, data, dataoff, &facility->fastStart.item[i]);
      if (ret != L7_SUCCESS)
        return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Process Q931 Progress UUIE
*
* @param   connInfo_t      *ctinfo Pointer to connInfo_t
* @param   L7_uchar8       **data Pointer to packet data
* @param   L7_uint32       dataoff
* @param   progressUUIE_t *progress Pointer to progress User-User
*                          information element
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t processProgress(connInfo_t *ctinfo, L7_uchar8 **data, L7_uint32 dataoff,
                               progressUUIE_t *progress)
{
  L7_RC_t ret;
  L7_uint32 i;
 
  if (progress->options & progress_h245Addr) 
  {
     ret = expectH245(ctinfo, data, dataoff, &progress->h245Addr);
     if (ret != L7_SUCCESS)
       return L7_FAILURE;

  }
  if (progress->options & progress_fastStart) 
  {
     for (i = 0; i < progress->fastStart.count; i++) 
     {
       ret = processOlc(ctinfo, data, dataoff, &progress->fastStart.item[i]);
       if (ret != L7_SUCCESS)
         return L7_FAILURE;
     }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Process Q931 Release Complete
*
* @param   connInfo_t      *ctinfo Pointer to connInfo_t
* @param   L7_uchar8       **data Pointer to packet data
* @param   L7_uint32       dataoff
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t processReleasecomplete(connInfo_t *ctinfo,
                           L7_uchar8 **data, L7_uint32 dataoff)
{
  L7_RC_t rv;

  /* TODO: remove Logical Channel entries */
  if (ctinfo->rtp_SessionStatus == L7_TRUE)
  {
    rv = voipSetupRTPSession(ctinfo, L7_DISABLE);
    if (rv != L7_SUCCESS) 
    {
      VOIP_TRACE(VOIP_DEBUG_ERROR,VOIP_PROTO_H323,"voipSetupRTPSession(disable) failed\n");
      return rv;
    }
  }
  rv = voipH323CallRemove(&voipH323CallList, ctinfo);
  return rv;
}
/*********************************************************************
* @purpose Process Q931 message 
* 
* @param   connInfo_t      *ctinfo Pointer to connInfo_t
* @param   L7_uchar8       **data Pointer to packet data
* @param   L7_uint32       dataoff data offset
* @param   Q931_t          *q931 Pointer to Q931 message to be processed
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t processQ931(connInfo_t *ctinfo,L7_uchar8 **data, L7_uint32 dataoff, Q931_t *q931)
{
  h323Pdu_t *pdu = &q931->UUIE.h323Pdu;
  L7_RC_t ret = L7_SUCCESS;
  L7_uint32 i;

  if (pdu->options & h323Pdu_h245Tunneling)
     ctinfo->tunneling  = L7_TRUE;

  switch (pdu->h323MessageBody.choice) 
  {
     case h323MessageBody_setup:
           ret = processSetup(ctinfo, data, dataoff,
                &pdu->h323MessageBody.u.setup);
           break;
     case h323MessageBody_callProceeding:
           ret = processCall(ctinfo, data, dataoff,
                 &pdu->h323MessageBody.u.call);
           break;
     case h323MessageBody_connect:
           ret = processConnect(ctinfo, data, dataoff,
                 &pdu->h323MessageBody.u.connect);
           break;
     case  h323MessageBody_alerting:
           ret = processAlerting(ctinfo, data, dataoff,
                 &pdu->h323MessageBody.u.alerting);
           break;
     case  h323MessageBody_information:
           ret = processInformation(ctinfo, data, dataoff,
                 &pdu->h323MessageBody.u.information);

          break;
    case  h323MessageBody_facility:
          ret = processFacility(ctinfo, data, dataoff,
                &pdu->h323MessageBody.u.facility);
          break;
    case  h323MessageBody_progress:
          ret = processProgress(ctinfo, data, dataoff,
                &pdu->h323MessageBody.u.progress);
          break;
    case  h323MessageBody_releaseComplete:
          ret = processReleasecomplete(ctinfo, data, dataoff);
          break;
    default:
          VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_H323,"Q.931 packet %d received\n",
                    pdu->h323MessageBody.choice);
          break;
  }
  if (ret != L7_SUCCESS)
     return L7_FAILURE;

  /* handle H.245 tunneling here */
  if (pdu->options & h323Pdu_h245Control) 
  {
     for (i = 0; i < pdu->h245Control.count; i++) 
     {
       ret = processH245(ctinfo, data, dataoff, &pdu->h245Control.item[i]);
       if (ret != L7_SUCCESS)
          return L7_FAILURE;
     }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Parse Q931 Message
*
* @param   const L7_uchar8 *buf      Pointer to packet data
* @param   L7_uint32       length    length of message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes  Q931 Message is ASN.1 Encoded one needs to decode and then
*         Parse the packet
*
* @end
*********************************************************************/
L7_RC_t voipQ931MessageParse(const L7_uchar8 *packetBuf, L7_uint32 length)
{
  static Q931_t q931;
  L7_uchar8 *data = L7_NULLPTR;
  L7_uchar8 *tmp;
  L7_uint32 datalen;
  L7_uint32 dataoff;
  L7_uint32 ret;
  connPair_t pair_info;
  L7_ipHdr_t *iph;
  L7_tcpHdr_t *tcph;
  voipH323Call_t *entry;
  L7_uint32 iphdrlen, tcpdataoff, tcpdatalen;

  
  tmp = (L7_uchar8 *)(packetBuf + 18);
  /* Get TCP data offset = IP header len + TCP header len */
  iph = (L7_ipHdr_t *)tmp;
  iphdrlen = (osapiNtohs(iph->_v_hl_tos) >> 8) & 0x0f; 
  tcph = (L7_tcpHdr_t *)(tmp+iphdrlen*4);
  tcpdataoff = iphdrlen   + (osapiNtohs(tcph->_len) >> 12); 
  tcpdataoff <<= 2;

  /* Get TCP data length */
  tcpdatalen = osapiNtohs(iph->_len) - tcpdataoff;;

  if (tcpdatalen <= 0)    /* No TCP data */
  {
    VOIP_TRACE(VOIP_DEBUG_ERROR, VOIP_PROTO_H323, "NO TCP data in the packet");
                                          
    return L7_FAILURE;
  } 

  VOIP_GET_LONG(pair_info.src_ip,tmp+12);
  VOIP_GET_LONG(pair_info.dst_ip,tmp+16);
  pair_info.src_port = osapiNtohs(tcph->_src_port);
  pair_info.dst_port = osapiNtohs(tcph->_dst_port);

  ret = voipH323CallLookup(&voipH323CallList, &pair_info, &entry);
  if (ret == L7_FAILURE) 
  {
     ret = voipH323CallInsert(&voipH323CallList, &pair_info, &entry);
     if(ret != L7_SUCCESS)
     {
        VOIP_TRACE(VOIP_DEBUG_INFO,VOIP_PROTO_H323,"voipH323CallInsert Failed");
        return L7_FAILURE;;
     }
  }
 
  /* Process each TPKT */
  while (getTpktData(tmp, &(entry->ct_info), tcpdatalen, tcpdataoff, 
         &data, &datalen, &dataoff)  == L7_SUCCESS) 
  {
    /* Decode Q.931 signal */
    ret = decodeQ931(data, datalen, &q931);
    if (ret < 0) 
    {
       VOIP_TRACE(VOIP_DEBUG_ERROR,VOIP_PROTO_H323,"q931: decoding error: %s\n",
              ret == L7_ERROR ? "L7_ERROR" : "L7_FAILURE");
     /* We don't drop when decoding error */
     break;
    }

    if (processQ931(&(entry->ct_info), &data, dataoff, &q931) != L7_SUCCESS)
    {
       return L7_FAILURE;
    }
  }
  return  L7_SUCCESS;
}
/*********************************************************************
* @purpose Look up for a H245 call
*
* @param   L7_ushort16   srcport  SourcePort
* @param   L7_ushort16   dstport  Destination Port
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vopiH245CallLookup(L7_ushort16 srcport,L7_ushort16 dstport)
{
  voipH323Call_t *list;
  connInfo_t *info;

  list = voipH323CallList;
  while (list != L7_NULLPTR) 
  {
    info = &(list->ct_info);
    if (info->h245_port == srcport || info->h245_port == dstport) 
    {
      return L7_SUCCESS;
    }
    list = list->next;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Clear all call entries corrosponding to an interface
*
* @param    L7_uint32 interface       Interface number for which entries
*           need to be cleared
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipH323ClearInterfaceEntries()
{
  voipH323Call_t *entry = voipH323CallList;

  while(entry!=L7_NULLPTR)
  {
    if (entry->ct_info.h245_SessionStatus == L7_TRUE)
      voipSetupH245Session(&entry->ct_info,L7_DISABLE);
 
    if (entry->ct_info.rtp_SessionStatus == L7_TRUE)
    {
        if (voipSetupRTPSession(&entry->ct_info, L7_DISABLE) == L7_SUCCESS)
           voipH323CallRemove(&voipH323CallList, &entry->ct_info);          
    }
    entry = entry->next;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose voipH323TimerAction
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*          Get the total number of packets belongs to the each call and
*          compare with previous count.
*          End the call if the previoues and current count is equal
*
* @end
*********************************************************************/

L7_RC_t voipH323TimerAction()
{
  voipH323Call_t *entry = voipH323CallList;
  L7_ulong64     counter;

  while(entry!=L7_NULLPTR)
  {
    if (entry->ct_info.rtp_SessionStatus == L7_TRUE)
    {
      if (dtlVoipSessionStatsGet(entry->ct_info.media_ip[0],
                                 entry->ct_info.media_ip[1],
                                 entry->ct_info.media_ip[0],
                                 entry->ct_info.media_ip[1],
                                    &counter) == L7_SUCCESS)
      {
        if((entry->prev_counter.high == counter.high) &&
           (entry->prev_counter.low == counter.low))
        {
          if (entry->ct_info.h245_SessionStatus == L7_TRUE)
           voipSetupH245Session(&entry->ct_info, L7_DISABLE);
         
          if (voipSetupRTPSession(&entry->ct_info, L7_DISABLE) == L7_SUCCESS)
            voipH323CallRemove(&voipH323CallList, &entry->ct_info);
        }
      }
    }
    entry = entry->next;
  }
  return L7_SUCCESS;
}
/************************ DEBUG FUNCTION ****************************/
void voipH323CallDump()
{
  voipH323Call_t *entry = voipH323CallList;
  L7_uint32 i;

  printf("\r\n H323 call database info");
  printf("\r\n -----------------------");

  while(entry!=L7_NULLPTR)
  {
    printf("\r\n source ipaddress =0x%x",entry->ct_info.pair.src_ip);
    printf("\r\n source port =0x%x",entry->ct_info.pair.src_port);
    printf("\r\n destination ipaddress =0x%x",entry->ct_info.pair.dst_ip);
    printf("\r\n destination port =0x%x",entry->ct_info.pair.dst_port);

    printf("\r\n H.245 channel info");
    printf("\r\n ------------------");
    printf("\r\n H.245 channel status =%d",entry->ct_info.h245_SessionStatus);
    printf("\r\n H.245 ipaddress =0x%x",entry->ct_info.h245_ip);
    printf("\r\n H.245 port =0x%x",entry->ct_info.h245_port);
   
    printf("\r\n RTP channel info"); 
    printf("\r\n ------------------");
  
    for (i=0 ;i<2;i++) 
    {
      printf("\r\n media ip address[i] =0x%x",entry->ct_info.media_ip[i]);
      printf("\r\n media port[i]=0x%x",entry->ct_info.media_port[i]);
    }
   entry = entry->next;
  }
  printf("\r\n ---------End------------");
}
    
