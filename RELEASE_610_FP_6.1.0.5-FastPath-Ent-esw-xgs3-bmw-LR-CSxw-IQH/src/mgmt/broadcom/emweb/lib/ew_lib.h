/*
 * Product: EmWeb
 * Release: R6_2_0
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
 * THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
 * AND OTHER US AND INTERNATIONAL PATENTS PENDING.
 * 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
 * 
 * Notice to Users of this Software Product:
 * 
 * This software product of Virata Corporation ("Virata"), 5 Clock Tower
 * Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in 
 * source and object code format embodies valuable intellectual property 
 * including trade secrets, copyrights and patents which are the exclusive
 * proprietary property of Virata. Access, use, reproduction, modification
 * disclsoure and distribution are expressly prohibited unless authorized
 * in writing by Virata.  Under no circumstances may you copy this
 * software or distribute it to anyone else by any means whatsoever except in
 * strict accordance with a license agreement between Virata and your
 * company.  This software is also protected under patent, trademark and 
 * copyright laws of the United States and foreign countries, as well as 
 * under the license agreement, against any unauthorized copying, modification,
 * or distribution.
 * Please see your supervisor or manager regarding any questions that you may
 * have about your right to use this software product.  Thank you for your
 * cooperation.
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
 * All Rights Reserved
 * 
 * Virata Corporation
 * 5 Clock Tower Place
 * Suite 400
 * Maynard, MA  01754
 * 
 * Voice: +1 (978) 461-0888
 * Fax:   +1 (978) 461-2080
 * 
 * http://www.emweb.com/
 *   support@agranat.com
 *     sales@agranat.com
 *
 * EmWeb/Server Standard Library
 *
 */

#ifndef _EWS_LIB_H_
#define _EWS_LIB_H_

/******************************************************************************
 * LINK
 *
 * Simple double-linked ring structure and manipulation macros
 *
 * Note that the EwsLink objects in a list point to each other, not the
 * structures containing them.  This makes the link manipulation macros
 * simpler, but requires some care.
 *
 * A list always has a 'head' or 'anchor' EwsLink, which is contained in
 * the structure that 'owns' the list, and does not represent an element
 * of the list.  The list itself is a doubly-linked ring structure.
 *
 *****************************************************************************/
typedef struct EwsLink_s
  {
    struct EwsLink_s    *next;
    struct EwsLink_s    *prev;
  } EwsLink, *EwsLinkP;

#define EWS_LINK_INIT(node)\
  {\
    (node)->next = (node)->prev = node;\
  }
/* Insert 'node' just before the list head (or element) 'list'. */
#define EWS_LINK_INSERT(list, node)\
  {\
    (node)->prev = (list)->prev;\
    (node)->next = (list);\
    (list)->prev->next = (node);\
    (list)->prev = (node);\
  }
/* Insert 'node' just after the list head (or element) 'list'. */
#define EWS_LINK_PUSH(list, node)\
  {\
    (node)->prev = (list);\
    (node)->next = (list)->next;\
    (list)->next->prev = (node);\
    (list)->next = (node);\
  }
#define EWS_LINK_DELETE(node)\
  {\
    (node)->prev->next = (node)->next;\
    (node)->next->prev = (node)->prev;\
    (node)->next = (node)->prev = node;\
  }

#define EWS_LINK_END(list) (&(list))
#define EWS_LINK_HEAD(list) ((list).next)
#define EWS_LINK_TAIL(list) ((list).prev)

/*
 * Go to the next/previous object on a linked list.
 */
#define EWS_LINK_NEXT(node) ((node).next)
#define EWS_LINK_PREV(node) ((node).prev)

#define EWS_LINK_IS_EMPTY(node) ((node)->prev == (node))

/*
 * Go to the next/previous object on a linked list of structures.
 * Works even if the link field is not the first field of the structure.
 *
 * node          a structure (not pointer) of type ElementType
 * ElementType   the type of the structure containing the links
 * link_field    the name of the field of ElementType that is the link that
 *               chains the objects together
 *
 * Returns a pointer (not structure) to the next object, of type
 * (ElementType *.).
 */

#define EWS_LINK_NEXT_FIELD(node, ElementType, link_field)                    \
((ElementType *)                                                              \
 ((char *) (((EwsLink *) &(node).link_field)->next) -                         \
  ((int) &((ElementType *) 0)->link_field)                                    \
  ))

#define EWS_LINK_PREV_FIELD(node, ElementType, link_field)                    \
((ElementType *)                                                              \
 ((char *) (((EwsLink *) &(node).link_field)->prev) -                         \
  ((int) &((ElementType *) 0)->link_field)                                    \
  ))

/*
 * Go to the first/last object on a linked list of structures.
 * Works even if the link field is not the first field of the structure.
 * Differs from EWS_LINK_NEXT/PREV_FIELD in that these macros start with an
 * EwsLink object, the header of the list, rather than an object of the type
 * that is on the list.
 *
 * node          a structure (not pointer) of type EwsLink
 * ElementType   the type of the structure containing the links
 * link_field    the name of the field of ElementType that is the link that
 *               chains the objects together
 *
 * Returns a pointer (not structure) to the next object, of type
 * (ElementType *.).
 */

#define EWS_LINK_HEAD_FIELD(node, ElementType, link_field)                    \
((ElementType *)                                                              \
 ((char *) ((node).next) -                                                    \
  ((int) &((ElementType *) 0)->link_field)                                    \
  ))

#define EWS_LINK_TAIL_FIELD(node, ElementType, link_field)                    \
((ElementType *)                                                              \
 ((char *) ((node).prev) -                                                    \
  ((int) &((ElementType *) 0)->link_field)                                    \
  ))

/*
 * Create a 'for' clause to walk through a list of structures linked with
 * EwsLink's.
 *
 * head          the EwsLink object (not pointer) that is the head of the list
 * variable      an identifier declared of type (ElementType *.)
 *               It will be the iteration variable of the 'for' loop.
 * ElementType   the type of the structure containing the links
 * link_field    the name of the field of ElementType that is the link that
 *               chains the objects together
 */
#define EWS_LINK_FOR(head, variable, ElementType, link_field)                 \
  for (variable = (ElementType *)                                             \
         (((char *) EWS_LINK_HEAD((head))) -                                  \
          ((int) &((ElementType *) 0)->link_field));                          \
       variable != (ElementType *)                                            \
         (((char *) &(head)) - ((int) &((ElementType *) 0)->link_field));     \
       variable = (ElementType *)                                             \
         (((char *) EWS_LINK_NEXT(variable->link_field)) -                    \
          ((int) &((ElementType *) 0)->link_field)))

/******************************************************************************
 *
 * STANDARD LIBRARY
 *
 * This file contains EmWeb/Server definitions for standard ANSI C library
 * functions...
 *
 *      EMWEB_MEMCPY    - memcpy()
 *      EMWEB_MEMSET    - memset()
 *      EMWEB_STRCPY    - strcpy()
 *      EMWEB_STRCMP    - strcmp()
 *      EMWEB_STRNCMP   - strncmp()
 *      EMWEB_STRLEN    - strlen()
 *      EMWEB_STRNLEN   - strnlen()
 *      EMWEB_STRCAT    - strcat()
 *      EMWEB_STRCHR    - strchr()   in ews_str.h,c
 *      EMWEB_STRSTR    - strstr()   in ews_str.h,c
 *      EMWEB_STRDUP    - string duplication
 *                        (EMWEB_STRDUP always returns memory that can be
 *                         freed with ewaFree.)
 * 
 * ... and more
 *
 *      EMWEB_LTOA      - unsigned long to string
 *      EMWEB_LTOAX     - unsigned long to string base hex
 *      EMWEB_SLTOA     - signed long to string
 *
 *****************************************************************************/

#ifdef EMWEB_HAVE_MEMCPY
#define EMWEB_MEMCPY(dst, src, len)     memcpy(dst, src, len)
#else /* EMWEB_HAVE_MEMCPY */
#define EMWEB_MEMCPY(dst, src, len)             \
  {                                             \
    uint8 *_dp = (uint8 *) dst;                 \
    const uint8 *_sp = (const uint8 *) src;     \
    uintf _len = len;                           \
      while (_len-- > 0)                        \
        {                                       \
          *_dp++ = *_sp++;                      \
        }                                       \
  }
#endif /* EMWEB_HAVE_MEMCPY */

#ifdef EMWEB_HAVE_MEMSET
#define EMWEB_MEMSET(dst, val, len)     memset(dst, val, len)
#else /* EMWEB_HAVE_MEMSET */
#define EMWEB_MEMSET(dst, val, len)             \
  {                                             \
    uint8 *_dp = (uint8 *) dst;                 \
    uintf _len = len;                           \
      while(_len-- > 0)                         \
        {                                       \
          *_dp++ = val;                         \
        }                                       \
  }
#endif /* EMWEB_HAVE_MEMSET */

#ifdef EMWEB_HAVE_STRCPY
#define EMWEB_STRCPY(dst, src)          strcpy((char *)dst, (const char *)src)
#else /* EMWEB_HAVE_STRCPY */
#define EMWEB_STRCPY(dst, src)                  \
  {                                             \
    char *_dp = (char *) dst;                   \
    const char *_sp = (const char *) src;       \
      while(*_sp != '\0')                       \
        {                                       \
           *_dp++ = *_sp++;                     \
        }                                       \
      *_dp = *_sp;                              \
  }
#endif /* EMWEB_HAVE_STRCPY */

#ifdef EMWEB_HAVE_STRNCPY
#define EMWEB_STRNCPY(dst, src, len)  strncpy((char *)dst, (const char *)src, len)
#else /* EMWEB_HAVE_STRNCPY */
#define EMWEB_STRNCPY(dst, src, len)            \
  {                                             \
    char *_dp = (char *) dst;                   \
    const char *_sp = (const char *) src;       \
    uintf _len = (uintf) len;                   \
      while(*_sp != '\0' && _len > 1)           \
        {                                       \
           *_dp++ = *_sp++;                     \
           _len--;                              \
        }                                       \
      *_dp = '\0';                              \
  }
#endif /* EMWEB_HAVE_STRNCPY */


#ifdef EMWEB_HAVE_STRCMP
#define EMWEB_STRCMP(ret, s1, s2)       ret = strcmp((const char *)s1, (const char *)s2)
#else /* EMWEB_HAVE_STRCMP */
#define EMWEB_STRCMP(ret, s1, s2)               \
  {                                             \
    const char *_sp1 = (const char *) s1;       \
    const char *_sp2 = (const char *) s2;       \
      while (*_sp1 == *_sp2 && *_sp1 != '\0')   \
        {                                       \
          _sp1++; _sp2++;                       \
        }                                       \
      ret = (*_sp1 - *_sp2);                    \
  }
#endif /* EMWEB_HAVE_STRCMP */

#ifdef EMWEB_HAVE_STRNCMP
#define EMWEB_STRNCMP(ret, s1, s2, len)       ret = strncmp((const char *)s1, (const char *)s2, uintf len)
#else /* EMWEB_HAVE_STRNCMP */
#define EMWEB_STRNCMP(ret, s1, s2, len)         \
{                                                       \
  const char *_tp1 = (const char *)(s1);                \
  const char *_tp2 = (const char *)(s2);                \
  uintf tlen = (len);                                   \
                                                        \
  for (ret=0; tlen && ret==0 && *_tp1 && *_tp2; tlen--,_tp1++,_tp2++)     \
    {                                                   \
      ret = (*_tp1 - *_tp2);                            \
    }                                                   \
}
#endif /* EMWEB_HAVE_STRNCMP */

#ifdef EMWEB_HAVE_MEMCMP
#define EMWEB_MEMCMP(ret, p1, p2, len) { ret = memcmp(p1, p2, len); }
#else /* EMWEB_HAVE_MEMCMP */
#define EMWEB_MEMCMP(ret, p1, p2, len)                  \
{                                                       \
  const char *_tp1 = (const char *)(p1);                \
  const char *_tp2 = (const char *)(p2);                \
  uintf tlen = (len);                                   \
                                                        \
  for (ret=0; tlen && ret==0; tlen--,_tp1++,_tp2++)     \
    {                                                   \
      ret = (*_tp1 - *_tp2);                            \
    }                                                   \
}
#endif /* EMWEB_HAVE_MEMCMP */

#ifdef EMWEB_HAVE_STRLEN
#define EMWEB_STRLEN(len, s)            (len) = strlen(s)
#else /* EMWEB_HAVE_STRLEN */
#define EMWEB_STRLEN(len, s)                        \
  {                                                 \
    const char *_cp = (const char *)(s);            \
      for ((len) = 0; *_cp != '\0'; _cp++, (len)++) \
        ;                                           \
  }
#endif /* EMWEB_HAVE_STRLEN */

#ifdef EMWEB_HAVE_SPRINTF
#define EMWEB_LTOA(cp, l, buf, len)     sprintf(cp=buf, "%lu", (unsigned long)l)
#define EMWEB_LTOAX(cp, l, buf, len)    sprintf(cp=buf, "%lx", (unsigned long)l)
#define EMWEB_SLTOA(cp, l, buf, len)    sprintf(cp=buf, "%ld", (long int)l)
#else /* EMWEB_HAVE_SPRINTF */

/*
 * EMWEB_LTOA
 *
 * Used to convert a uint32 into a char* nul-terminated string of decimal
 * digits.
 *
 * cp  - output: resulting nul-terminated string of digits
 * l   - input:  uint32 value to be converted to string
 * buf - input:  pointer to char* scratch buffer used to build string
 * len - input:  length of scratch buffer (buf).
 *
 * Note that cp might not equal buf on return.  This is because the algorithm
 * below determines digits from right (least significant) to left (most
 * significant) and thus writes from the end of the buffer backwards towards
 * the beginning.
 *
 * This is equivalent to: 
 *
 *   sprintf( cp=buf, "%lu", (unsigned long) l )
 *
 * WARNING: This macro assumes that the buffer is large enough to 
 *          accommodate the value.  There are no bounds checking.
 *          The largest 32-bit number can fit in 10 decimal digits
 *          plus a null-termination byte, so always safe to specify
 *          an 11-byte buffer.
 */
#define EMWEB_LTOA(cp, l, buf, len)             \
  {                                             \
    uint32 _div = (uint32)(l);                  \
    uintf _rem;                                 \
      cp = &(buf)[(len) - 1];                   \
      *cp = '\0';                               \
      if ((_div) == 0)                          \
        {                                       \
          cp--;                                 \
          *cp = '0';                            \
        }                                       \
      else while ((_div) != 0)                  \
        {                                       \
          cp--;                                 \
          _rem = _div % 10;                     \
          *cp = '0' + _rem;                     \
          _div = _div / 10;                     \
        }                                       \
  }

/*
 * EMWEB_LTOAX
 *
 * Used to convert a uint32 into a char* nul-terminated string of hexadecimal
 * digits.
 *
 * cp  - output: resulting nul-terminated string of hex digits
 * l   - input:  uint32 value to be converted to string
 * buf - input:  pointer to char* scratch buffer used to build string
 * len - input:  length of scratch buffer (buf).
 *
 * Note that cp might not equal buf on return.  This is because the algorithm
 * below determines digits from right (least significant) to left (most
 * significant) and thus writes from the end of the buffer backwards towards
 * the beginning.
 *
 * This is equivalent to: 
 *
 *   sprintf( cp=buf, "%lx", (unsigned long) l )
 *
 * WARNING: This macro assumes that the buffer is large enough to 
 *          accommodate the value.  There are no bounds checking.
 *          The largest 32-bit number can fit in 8 hexadecimal digits
 *          plus a null-termination byte, so always safe to specify
 *          a 9-byte buffer.
 */
#define EMWEB_LTOAX(cp, l, buf, len)            \
  {                                             \
    uint32 _div = (uint32)(l);                  \
    uintf _rem;                                 \
      cp = &(buf)[(len) - 1];                   \
      *cp = '\0';                               \
      if ((_div) == 0)                          \
        {                                       \
          cp--;                                 \
          *cp = '0';                            \
        }                                       \
      else while ((_div) != 0)                  \
        {                                       \
          cp--;                                 \
          _rem = _div % 16;                     \
          *cp = _rem + (_rem > 9? 'a'-10 : '0');   \
          _div = _div / 16;                     \
        }                                       \
  }

/*
 * EMWEB_SLTOA
 *
 * Used to convert an int32 into a char* nul-terminated string of decimal
 * digits with a possible leading '-' if <0.
 *
 * cp   - output: resulting nul-terminated string of decimal digits
 * sl   - input:  int32 value to be converted to string
 * sbuf - input:  pointer to char* scratch buffer used to build string
 * slen - input:  length of scratch buffer (buf).
 *
 * Note that cp might not equal buf on return.  This is because the algorithm
 * below determines digits from right (least significant) to left (most
 * significant) and thus writes from the end of the buffer backwards towards
 * the beginning.
 *
 * This is equivalent to: 
 *
 *   sprintf( cp=buf, "%ld", (long) sl )
 *
 * WARNING: This macro assumes that the buffer is large enough to 
 *          accommodate the value.  There are no bounds checking.
 *          The largest 32-bit number can fit in 10 decimal digits
 *          plus a possible minus sign, plus the null-termination byte, 
 *          so always safe to specify a 12-byte buffer.
 */
#define EMWEB_SLTOA(scp, sl, sbuf, slen)         \
   {                                             \
     int32 _sdiv = (int32)sl;                    \
     boolean _neg;                               \
                                                 \
     _neg = FALSE;                               \
     if (_sdiv < 0)                              \
       {                                         \
         _neg = TRUE;                            \
         _sdiv = -_sdiv;                         \
       }                                         \
                                                 \
     EMWEB_LTOA(scp, _sdiv, sbuf, slen );        \
                                                 \
     if (_neg)                                   \
       {                                         \
         --scp;                                  \
         *scp = '-';                             \
       }                                         \
   }

#endif /* EMWEB_HAVE_SPRINTF */

#ifdef EMWEB_HAVE_STRCAT
#define EMWEB_STRCAT(dst, src)          strcat((char *)dst, (char *)src)
#else /* EMWEB_HAVE_STRCAT */
#define EMWEB_STRCAT(dst, src)                  \
  {                                             \
    uintf _len;                                 \
    char *_dstP = dst;                          \
                                                \
    EMWEB_STRLEN(_len, _dstP);                  \
                                                \
    _dstP += _len;                              \
                                                \
    EMWEB_STRCPY(_dstP, src);                   \
                                                \
  }
#endif /* EMWEB_HAVE_STRCAT */

#ifdef EMWEB_HAVE_STRDUP
#define EMWEB_STRDUP(ret, s)         (ret) = strdup((s))
#else /* EMWEB_HAVE_STRDUP */
#define EMWEB_STRDUP(ret, s)                    \
  {                                             \
    uintf _len;                                 \
    char *_retp = (char *)ret;                  \
    const char *_s_dup = (const char *)(s);     \
                                                \
    EMWEB_STRLEN(_len, _s_dup);                 \
                                                \
    _retp = (char *)ewaAlloc(_len + 1);         \
                                                \
    if (_retp)                                  \
      {                                         \
        EMWEB_STRCPY(_retp, _s_dup);            \
      }                                         \
    ret = _retp;                                \
  }
#endif /* EMWEB_HAVE_STRDUP */

/******************************************************************************
 * Reference Counting
 *****************************************************************************/

typedef int16 RefCount;

#define REFCOUNT RefCount refcount

/*
 * INITREFCOUNT(foo) - initialize reference count for foo to 1
 */
#define INITREFCOUNT(object) (object)->refcount = 1;

/*
 * EWINUSE(foo) - increments refcount for foo
 */
#define EWINUSE(object) ((object)->refcount++)

/*
 * EWUNUSE(foo) - decrements refcount, returns TRUE if refcount <= 0
 */
#define EWUNUSE(object) ((object)->refcount--, (object)->refcount <= 0)


/******************************************************************************
 * BIT MASK Operations
 *****************************************************************************/

/* The type of a word in the bit vector system, and of a pointer to
 * a word. */
typedef uint32 EwsBitVector, *EwsBitVector_p;

/* The number of bits in a "word". */
#define EWS_BITS_PER_WORD 32

#define EWS_BITVECTOR_ZERO(v, len)                               \
  {                                                              \
    unsigned _i;                                                 \
    for (_i = 0; _i < len; _i += EWS_BITS_PER_WORD)              \
      {                                                          \
        v[_i/EWS_BITS_PER_WORD] = 0;                             \
      }                                                          \
  }

#define EWS_BITVECTOR_ONES(v, len)                               \
  {                                                              \
    unsigned _i;                                                 \
    for (_i = 0; _i < len; _i += EWS_BITS_PER_WORD)              \
      {                                                          \
        v[_i/EWS_BITS_PER_WORD] = ~0;                            \
      }                                                          \
  }

#define EWS_BITVECTOR_OR(dst, src, len)                          \
  {                                                              \
    unsigned _i;                                                 \
    for (_i = 0; _i < len; _i += EWS_BITS_PER_WORD)              \
      {                                                          \
        dst[_i/EWS_BITS_PER_WORD] |= src[_i/EWS_BITS_PER_WORD];  \
      }                                                          \
  }

#define EWS_BITVECTOR_COPY(dst, src, len)                        \
  {                                                              \
    unsigned _i;                                                 \
    for (_i = 0; _i < len; _i += EWS_BITS_PER_WORD)              \
      {                                                          \
        dst[_i/EWS_BITS_PER_WORD] = src[_i/EWS_BITS_PER_WORD];   \
      }                                                          \
  }

#define EWS_BITVECTOR_ISZERO(result, vec, len)                   \
  {                                                              \
    unsigned _i;                                                 \
    result = 0;                                                  \
    for (_i = 0; _i < len; _i += EWS_BITS_PER_WORD)              \
      {                                                          \
        result += vec[_i/EWS_BITS_PER_WORD];                     \
      }                                                          \
    result = ! result;                                           \
  }


#define EWS_BITVECTOR_SET(v, bit)      \
        ((v)[bit/EWS_BITS_PER_WORD] |= (1U << ((bit) % EWS_BITS_PER_WORD)))
#define EWS_BITVECTOR_CLEAR(v, bit)    \
        ((v)[bit/EWS_BITS_PER_WORD] &= ~(1U << ((bit) % EWS_BITS_PER_WORD)))
#define EWS_BITVECTOR_GET(v, bit)       \
        (((v)[bit/EWS_BITS_PER_WORD] >> ((bit) % EWS_BITS_PER_WORD)) & 1U)

#endif /* _EWS_LIB_H_ */
