/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename      common.h
 *
 * @purpose       Common use macros and types
 *
 * @component     Routing Utils Component
 *
 * @comments
 *
 * @create        07/09/1997
 *
 * @author        Igor Bryskin
 *
 * @end
 *
 * ********************************************************************/
#ifndef _COMMON_H
#define _COMMON_H

#define IN     /* input parameter          */
#define OUT    /* output parameter         */
#define INOUT  /* input & output parameter */

#define ZERO  0
#define DUMMY 0

#define AND  &&
#define OR   ||
#define NOT  !
#define XOR  ^

#define PUBLIC
#define PRIVATE static

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define ENTAN 100 /* Entity for connection manager */

/***************************
 * GENERIC MACRO'S         *
 ***************************/

#define SWAP16(val) \
   (((val)<<8) | ((val)>>8))

#define SWAP32(val) \
   (((ulng)(val) >> 24) | (((ulng)(val) >> 8) & 0x0000ff00) | \
    ((ulng)(val) << 24) | (((ulng)(val) << 8) & 0x00ff0000))

#define MAKE_U16(low, high) ((word)(((byte)(low)) | (((word)((byte)(high))) << 8)))
#define MAKE_U32(low, high) ((ulng)(((word)(low)) | (((ulng)((word)(high))) << 16)))

/* X-ENDIAN to Little-Endian conversion */
#if BIG_ENDIAN
#define WORD_2LE(val)  SWAP16(val)
#define ULNG_2LE(val)  SWAP32(val)
#else
#define WORD_2LE(val)  (val)
#define ULNG_2LE(val)  (val)
#endif

#define LO_U8(l)            ((byte)(l))
#define HI_U8(l)            ((byte)((word)(l) >> 8))
#define LO_U16(l)           ((word)(l))
#define HI_U16(l)           ((word)((ulng)(l) >> 16))

#define HI_NIBBLE(c)        ((byte)(c) >> 4)
#define LO_NIBBLE(c)        ((byte)(c) & 0x0F)

#if BIG_ENDIAN /* Big endian mode machine, preserve the byte order */

/* Convert word value from network to host order */
#define L7_NTOHS(frameBuf, data)\
   (*(byte*)(data) = *(byte*)(frameBuf),\
    *((byte*)(data) + 1) = *((byte*)(frameBuf) + 1))


/* Convert long value from network to host order */
#define L7_NTOHL(frameBuf, data)\
   (*(byte*)(data) = *(byte*)(frameBuf),\
    *((byte*)(data) + 1) = *((byte*)(frameBuf) + 1),\
    *((byte*)(data) + 2) = *((byte*)(frameBuf) + 2),\
    *((byte*)(data) + 3) = *((byte*)(frameBuf) + 3))

/* Store word value in network order */
#define L7_HTONS(data, frameBuf)\
   (*(byte*)(frameBuf) = *(byte*)(data),\
    *((byte*)(frameBuf) + 1) = *((byte*)(data) + 1))

/* Store ulng value in network order */
#define L7_HTONL(data, frameBuf)\
   (*(byte*)(frameBuf) = *(byte*)(data),\
    *((byte*)(frameBuf) + 1) = *((byte*)(data) + 1),\
    *((byte*)(frameBuf) + 2) = *((byte*)(data) + 2),\
    *((byte*)(frameBuf) + 3) = *((byte*)(data) + 3))

#else /* Little endian mode machine, reverse the byte order */

/* Convert word value from network to host order */
#define L7_NTOHS(frameBuf, data)\
   (*(byte*)(data) = *((byte*)(frameBuf) + 1),\
    *((byte*)(data) + 1) = *(byte*)(frameBuf))

/* Convert long value from network to host order */
#define L7_NTOHL(frameBuf, data)\
   (*(byte*)(data) = *((byte*)(frameBuf) + 3),\
    *((byte*)(data) + 1) = *((byte*)(frameBuf) + 2),\
    *((byte*)(data) + 2) = *((byte*)(frameBuf) + 1),\
    *((byte*)(data) + 3) = *(byte*)(frameBuf))

/* Store word value in network order */
#define L7_HTONS(data, frameBuf)\
   (*(byte*)(frameBuf) = *((byte*)(data) + 1),\
    *((byte*)(frameBuf) + 1) = *(byte*)(data))

/* Store ulng value in network order */
#define L7_HTONL(data, frameBuf)\
   (*(byte*)(frameBuf) = *((byte*)(data) + 3),\
    *((byte*)(frameBuf) + 1) = *((byte*)(data) + 2),\
    *((byte*)(frameBuf) + 2) = *((byte*)(data) + 1),\
    *((byte*)(frameBuf) + 3) = *(byte*)(data))

#endif /* BIG_ENDIAN */

#define   BCD_TO_ASCII(c,b)\
 *b++ = c<10 ? c+'0' : c-10+'A'

/* Increment high capacity counters by "nmb"*/
#define INCR_HCCOUNTERS(hclow, hchigh, nmb)\
{\
   Bool fHigh = ((hclow) & 0x80000000) != 0;\
   (hclow) += (nmb);\
   if( fHigh && (((hclow) & 0x80000000) == 0) )\
     (hchigh)++;\
}
/* Protocol events */
typedef enum tag_ProtEvntType
{
   PET_CONCFM,
   PET_DISCIND,
   PET_DISCCFM,
   PET_DATIND,
   PET_DATCFM
} ProtEvntType;

/* Trace levels */
typedef enum TRACE_LEVEL
{
  TRACE_LEVEL_0,
  TRACE_LEVEL_1,
  TRACE_LEVEL_2,
  TRACE_LEVEL_3
} TRACE_LEVEL;

/* Bit set/get macros */
#define L7_BIT_SET(xflag, xbit)      (void) ((xflag) |=  (xbit))
#define L7_BIT_CLR(xflag, xbit)      (void) ((xflag) &= ~(xbit))
#define L7_BIT_ISSET(xflag, xbit)    ((xflag) & (xbit))

#define IS_CUR         0x1
#define SEQNO_OVER     0x2
#define FLUSHING_HELD  0x4

/**********************************************************************/

#ifdef    __cplusplus
extern "C" {
#endif   /* __cplusplus */


int __assertfail(const char *msg, const char *cond, const char *file, int line);

#ifndef ASSERT
#ifdef ERRCHK
#define ASSERT(p) ((p) ? (void)0 : (void) __assertfail(\
   "Assertion failed: %s, file %s, line %d", #p, __FILE__, __LINE__))
#else
#define ASSERT(p)
#endif
#endif

/* ID of the message queue used by L7 task */
extern  ulng XX_GetL7_tid(void);
#define L7_DEFAULT_THREAD ((t_Handle)XX_GetL7_tid())

#define Q93B_MAX_MESSAGE_LENGTH 300

#ifdef __cplusplus
}
#endif

#endif /* _COMMON_H */
