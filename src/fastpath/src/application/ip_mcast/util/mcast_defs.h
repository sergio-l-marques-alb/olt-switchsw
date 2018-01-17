/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  mcast_defs.h
*
* @purpose   Common Macros
*
* @component
*
* @comments  none
*
* @create   02/21/2006
*
* @author   dshashidhar/dsatyanarayana
*
* @end
*
**********************************************************************/
#ifndef MCAST_DEFS_H
#define MCAST_DEFS_H

#include "ipv6_commdefs.h"
#include "osapi_support.h"
/*
 * Macros to get various length values from the stream.  cp must be a
 * (L7_uchar8 *)
 */

/* PUT_NETLONG puts "network ordered" data to the datastream.
 * PUT_HOSTLONG puts "host ordered" data to the datastream.
 * GET_NETLONG gets the data and keeps it in "network order" in the memory
 * GET_HOSTLONG gets the data, but in the memory it is in "host order"
 * The same for all {PUT,GET}_{NET,HOST}{SHORT,LONG}
 */
#define MCAST_GET_BYTE(val, cp) ((val) = *(L7_uchar8 *)(cp)++)
#define MCAST_PUT_BYTE(val, cp) (*(cp)++ = (L7_uchar8)(val))

#define MCAST_GET_SHORT(val, cp) \
    do { \
        register L7_ushort16 Xv; \
        Xv = (*(L7_uchar8 *)(cp)++) << 8; \
        Xv |= *(L7_uchar8 *)(cp)++; \
        (val) = Xv; \
    } while (0)
#define MCAST_PUT_SHORT(val, cp) \
    do { \
        register L7_ushort16 Xv; \
        Xv = (L7_ushort16)(val); \
        *(cp)++ = (L7_uchar8)(Xv >> 8); \
        *(cp)++ = (L7_uchar8)Xv; \
    } while (0)


#define MCAST_GET_LONG(val, cp) \
    do { \
        register L7_ulong32 Xv; \
        Xv = (*(L7_uchar8 *)(cp)++) << 24; \
        Xv |= (*(L7_uchar8 *)(cp)++) << 16; \
        Xv |= (*(L7_uchar8 *)(cp)++) << 8; \
        Xv |= *(L7_uchar8 *)(cp)++; \
        (val) = Xv; \
    } while (0)
#define MCAST_PUT_LONG(val, cp) \
    do { \
        register L7_uint32 Xv; \
        Xv = (L7_uint32)(val); \
        *(cp)++ = (L7_uchar8)(Xv >> 24); \
        *(cp)++ = (L7_uchar8)(Xv >> 16); \
        *(cp)++ = (L7_uchar8)(Xv >>  8); \
        *(cp)++ = (L7_uchar8)Xv; \
    } while (0)

#if defined(BYTE_ORDER) && (BYTE_ORDER == LITTLE_ENDIAN)
#define MCAST_GET_NETSHORT(val, cp) \
    do { \
        register L7_uchar8 *Xvp; \
        L7_ushort16 Xv; \
        Xvp = (L7_uchar8 *) &Xv; \
        *Xvp++ = *(cp)++; \
        *Xvp++ = *(cp)++; \
        (val) = Xv; \
    } while (0)
#define MCAST_PUT_NETSHORT(val, cp) \
    do { \
        register L7_uchar8 *Xvp; \
        L7_ushort16 Xv = (L7_ushort16)(val); \
        Xvp = (L7_uchar8 *)&Xv; \
        *(cp)++ = *Xvp++; \
        *(cp)++ = *Xvp++; \
    } while (0)


#define MCAST_GET_NETLONG(val, cp) \
    do { \
        register L7_uchar8 *Xvp; \
        L7_ulong32 Xv; \
        Xvp = (L7_uchar8 *) &Xv; \
        *Xvp++ = *(cp)++; \
        *Xvp++ = *(cp)++; \
        *Xvp++ = *(cp)++; \
        *Xvp++ = *(cp)++; \
        (val) = Xv; \
    } while (0)
#define MCAST_PUT_NETLONG(val, cp) \
    do { \
        register L7_uchar8 *Xvp; \
        L7_ulong32 Xv = (L7_ulong32)(val); \
        Xvp = (L7_uchar8 *)&Xv; \
        *(cp)++ = *Xvp++; \
        *(cp)++ = *Xvp++; \
        *(cp)++ = *Xvp++; \
        *(cp)++ = *Xvp++; \
    } while (0)
#else
#define MCAST_GET_NETSHORT(val, cp) MCAST_GET_SHORT(val, cp)
#define MCAST_PUT_NETSHORT(val, cp) MCAST_PUT_SHORT(val, cp)
#define MCAST_GET_NETLONG(val, cp) MCAST_GET_LONG(val, cp)
#define MCAST_PUT_NETLONG(val, cp) MCAST_PUT_LONG(val, cp)
#endif /* defined(BYTE_ORDER)*/

/*
 * The following macro extracts network addresses from the stream.  It
 * is used to decode the end of update messages, and understands that
 * network numbers are stored internally in network byte order.
 */
#define MCAST_GET_ADDR(addr, cp) \
    do { \
        register L7_uchar8 *Xap; \
        Xap = (L7_uchar8 *)(addr); \
        *Xap++ = *(cp)++; \
        *Xap++ = *(cp)++; \
        *Xap++ = *(cp)++; \
        *Xap++ = *(cp)++; \
    } while (0)
/*
 * The following puts a network address into the buffer in the
 * form a BGP update message would like.  We know the address
 * is in network byte order already.
 */
#define MCAST_PUT_ADDR(addr, cp) \
    do { \
        register L7_uchar8 *Xap; \
        Xap = (L7_uchar8 *)(addr); \
        *(cp)++ = *Xap++; \
        *(cp)++ = *Xap++; \
        *(cp)++ = *Xap++; \
        *(cp)++ = *Xap++; \
    } while (0)


#define MCAST_GET_ADDR6(addr, cp) \
    do { \
        register L7_uchar8 *Xap; \
        register int i; \
        Xap = (L7_uchar8 *)(addr); \
        for (i = 0; i < 16; i++) \
            *Xap++ = *(cp)++; \
    } while (0)
#define MCAST_PUT_ADDR6(addr, cp) \
    do { \
        register L7_uchar8 *Xap; \
        register int i; \
        Xap = (L7_uchar8 *)(addr); \
        for (i = 0; i < 16; i++) \
            *(cp)++ = *Xap++; \
    } while (0)


#define MCAST_GET_ADDR_INET(family, inetAddr, cp) \
    do { \
         ((L7_inet_addr_t *)inetAddr)->family = family;  \
         if (family == L7_AF_INET)  \
         {  \
           MCAST_GET_ADDR((&((inetAddr)->addr.ipv4.s_addr)), (cp)); \
         } \
         else if (family == L7_AF_INET6)  \
         {  \
           MCAST_GET_ADDR6((&((inetAddr)->addr.ipv6.in6.addr8)), (cp));  \
         }  \
         else  \
         {  \
           L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_MCAST_MAP_COMPONENT_ID, "Invalid Address Family");   \
         } \
       } while (0)

#define MCAST_PUT_ADDR_INET(family, inetAddr, cp) \
    do { \
         if (family == L7_AF_INET)  \
         {  \
           MCAST_PUT_ADDR((&((inetAddr)->addr.ipv4.s_addr)), (cp)); \
         }  \
         else if (family == L7_AF_INET6)  \
         {  \
           MCAST_PUT_ADDR6((&((inetAddr)->addr.ipv6.in6.addr8)), (cp));  \
         }  \
         else  \
         {  \
           L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_MCAST_MAP_COMPONENT_ID, "Invalid Address Family");   \
         } \
       } while (0)










#define MCAST_GET_DATA(data, len, cp) (memcpy (data, cp, len), (cp) += (len))
#define MCAST_PUT_DATA(data, len, cp) (memcpy (cp, data, len), (cp) += (len))

/* use following for ipv4 only */
#define MCAST_MASK_TO_MASKLEN(mask, masklen)                           \
    do {                                                         \
        register L7_uint32 tmp_mask = osapiNtohl((mask));               \
        register L7_uchar8  tmp_masklen = sizeof((mask)) << 3;      \
        for ( ; tmp_masklen > 0; tmp_masklen--, tmp_mask >>= 1)  \
            if (tmp_mask & 0x1)                                  \
                break;                                           \
        (masklen) = tmp_masklen;                                 \
    } while (0)

#define MCAST_MASKLEN_TO_MASK(masklen, mask)                                       \
        do {                                                                         \
            (mask) = (masklen)? osapiHtonl(~0 << ((sizeof((mask)) << 3) - (masklen))) : 0;\
        } while (0)


#endif /* MCAST_DEFS_H */

