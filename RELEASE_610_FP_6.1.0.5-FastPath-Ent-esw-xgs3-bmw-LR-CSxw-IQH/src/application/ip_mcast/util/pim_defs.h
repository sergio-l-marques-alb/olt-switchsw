/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  pim_defs.h
*
* @purpose   Common Macros for both pimdm and pimsm.
*
* @component PIM
*
* @comments  none
*
* @create   02/21/2006
*
* @author   dsatyanarayana
*
* @end
*
**********************************************************************/
#ifndef PIM_DEFS_H
#define PIM_DEFS_H
#include "mcast_defs.h"
#include "l3_addrdefs.h"
#include "l7apptimer_api.h"
#include "log.h"
/* Addresses Encoding Type (specific for each Address Family) */
#define L7_AF_INET_ENC_TYPE       0
#define L7_AF_INET6_ENC_TYPE      0

/* PIM related common structs 
   useful for both pim-dm and pim-sm */
#define PIM_PROTOCOL_VERSION    2
#define PIM_HEARDER_SIZE        4 
#define PIM_IP_PROTO_NUM        IP_PROT_PIM

/*
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |PIM Ver| Type  |   Reserved    |           Checksum            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |        Upstream Neighbor Address (Encoded-Unicast format)     |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Reserved     | Num groups    |          Holdtime             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         Multicast Group Address 1 (Encoded-Group format)      |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |   Number of Joined Sources    |   Number of Pruned Sources    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


 #define PIM_MAX_JP_MSG_SIZE   = (L7_MULTICAST_MAX_IP_MTU - 
            IPv4/v6-Header-Size - PIM-Header-Size )     
               = 1500 - 40 - 4 = 1456 
   (L7_MULTICAST_MAX_IP_MTU - 4 - 18 - 4 - 
         PIM_MAX_JP_NUM_GROUPS * (20 - 4 - PIM_MAX_ADDRS_IN_LIST * 20))
    20 = sizeof(pim_encod_src_addr_t) , 20 = sizeof(pim_encod_grp_addr_t) 
      18 = sizeof(pim_encod_uni_addr_t)
     4 = two shorts fro numJoins and numPrunes
     4 = pim header
 */
#define PIM_MAX_JP_MSG_SIZE                       1450
#define PIM_IPV4_MAX_JOIN_OR_PRUNE_ADDRS_IN_LIST  64
#define PIM_IPV6_MAX_JOIN_OR_PRUNE_ADDRS_IN_LIST  24

/*********************************************************************
*          PIM Protocol Control Message Type enumeration
*********************************************************************/
typedef enum
{
  PIM_PDU_HELLO = 0,
  PIM_PDU_REGISTER = 1,
  PIM_PDU_REGISTER_STOP = 2,
  PIM_PDU_JOIN_PRUNE = 3,
  PIM_PDU_BOOTSTRAP = 4,
  PIM_PDU_ASSERT = 5,
  PIM_PDU_GRAFT = 6,
  PIM_PDU_GRAFT_ACK = 7,
  PIM_PDU_CANDIDATE_RP_ADVERTISEMENT = 8,
  PIM_PDU_STATE_REFRESH = 9,
  PIM_PDU_MAX
}PIM_PDU_TYPE_t;


typedef struct pim_header_s {
    L7_uchar8    pim_type_version; /* type and version */
    L7_uchar8    pim_reserved;   /* Reserved         */
    L7_short16   pim_cksum;  /* IP-style (CRC) checksum  */
}pim_header_t;

/* Encoded-Unicast: 6 bytes long */
typedef struct pim_encod_uni_addr_s {
    L7_uchar8      addr_family;
    L7_uchar8      encod_type;
    union
     {
       struct L7_in_addr_s    ipv4;
       struct L7_in6_addr_s   ipv6;
     } addr;  /*unicast_addr*/
                     /* XXX: Note the 32-bit boundary
                      * misalignment for  the unicast
                      * address when placed in the
                      * memory. Must read it byte-by-byte!
                      */
} pim_encod_uni_addr_t;

/* Encoded-Group */
typedef struct pim_encod_grp_addr_s {
    L7_uchar8      addr_family;
    L7_uchar8      encod_type;
    L7_uchar8      reserved;
    L7_uchar8      masklen;
    union
     {
       struct L7_in_addr_s    ipv4;
       struct L7_in6_addr_s   ipv6;
     } addr;  /*mcast_addr*/   
} pim_encod_grp_addr_t;

/* Encoded-Source */
typedef struct pim_encod_src_addr_s {
    L7_uchar8      addr_family;
    L7_uchar8      encod_type;
    L7_uchar8      flags;
    L7_uchar8      masklen;
    union
     {
       struct L7_in_addr_s    ipv4;
       struct L7_in6_addr_s   ipv6;
     } addr;  /*src_addr*/  
} pim_encod_src_addr_t;

typedef struct pimRtoInfo_s
{
  L7_APP_TMR_HNDL_t          rtoTimer;
  L7_VOIDFUNCPTR_t           rtoCallBack;
} pimRtoInfo_t;


/* The number of best route changes that PIM requests from RTO. This controls
 * the size of a buffer allocated at init time and is not configurable at
 * run time. */
#define L7_PIM_MAX_BEST_ROUTE_CHANGES L7_IPMAP_MAX_BEST_ROUTE_CHANGES

/* How long PIMSM waits from the time RTO notifies it of best route changes
 * until PIMSM asks RTO for those changes. The delay allows RTO to process
 * several changes and send them all to PIM at one time and also allows
 * flaps to damp out. */
#define L7_PIM_BEST_ROUTE_DELAY    2   /*secs*/ 




/* fix below :
 * change in macros from {GET,PUT}_NETLONG to {GET,PUT}_HOSTLONG
 * assuming BYTE_ORDER to be BIG_ENDIAN since the #if defined
 * directives above do not work
 */
/* for ipv4 :  addr.ipv4.s_addr */ 

#define PIM_GET_ESADDR_IPV4(esa, cp)                     \
        do {                                    \
            (esa)->addr_family = *(cp)++;       \
            (esa)->encod_type  = *(cp)++;       \
            (esa)->flags       = *(cp)++;       \
            (esa)->masklen     = *(cp)++;       \
            MCAST_GET_LONG((esa)->addr.ipv4.s_addr, (cp)); \
        } while(0)


#define PIM_PUT_ESADDR_IPV4(addr, masklen, flags, cp)    \
        do {                                    \
            *(cp)++ = L7_AF_INET; /* family */  \
            *(cp)++ = L7_AF_INET_ENC_TYPE; /* type   */  \
            *(cp)++ = (flags);    /* flags  */  \
            *(cp)++ = (masklen);                \
            MCAST_PUT_LONG((addr) , (cp));   \
        } while(0)

#define PIM_GET_EGADDR_IPV4(ega, cp)                     \
        do {                                    \
            (ega)->addr_family = *(cp)++;       \
            (ega)->encod_type  = *(cp)++;       \
            (ega)->reserved    = *(cp)++;       \
            (ega)->masklen     = *(cp)++;       \
            MCAST_GET_LONG((ega)->addr.ipv4.s_addr, (cp)); \
        } while(0)

#define PIM_PUT_EGADDR_IPV4(addr, masklen, reserved, cp) \
        do {                                    \
            *(cp)++ = L7_AF_INET; /* family */  \
            *(cp)++ = L7_AF_INET_ENC_TYPE; /* type   */  \
            *(cp)++ = (reserved); /* reserved; should be 0 */  \
            *(cp)++ = (masklen);                \
            MCAST_PUT_LONG((addr) , (cp)); \
        } while(0)
#define PIM_GET_EUADDR_IPV4(eua, cp)                     \
        do {                                    \
            (eua)->addr_family = *(cp)++;       \
            (eua)->encod_type  = *(cp)++;       \
            MCAST_GET_LONG((eua)->addr.ipv4.s_addr, (cp)); \
        } while(0)

#define PIM_PUT_EUADDR_IPV4(addr, cp)                    \
        do {                                    \
            *(cp)++ = L7_AF_INET; /* family */  \
            *(cp)++ = L7_AF_INET_ENC_TYPE; /* type   */  \
            MCAST_PUT_LONG((addr), (cp));          \
        } while(0)

#define PIM_GET_ESADDR_IPV6(esa, cp)                     \
        do {                                    \
            (esa)->addr_family = *(cp)++;       \
            (esa)->encod_type  = *(cp)++;       \
            (esa)->flags       = *(cp)++;       \
            (esa)->masklen     = *(cp)++;       \
            MCAST_GET_ADDR6(((esa)->addr.ipv6.in6.addr8), (cp));  \
        } while(0)

#define PIM_PUT_ESADDR_IPV6(esa, masklen, flags, cp)    \
        do {                                    \
            L7_inet_addr_t mask;                       \
            L7_inet_addr_t Addr;                  \
            memset(&Addr,0,sizeof(Addr));         \
            memset(&mask,0,sizeof(mask));         \
            inetMaskLenToMask(L7_AF_INET6, (masklen), &mask);   \
            *(cp)++ = L7_AF_INET6; /* family */  \
            *(cp)++ = L7_AF_INET6_ENC_TYPE; /* type   */  \
            *(cp)++ = (flags);    /* flags  */  \
            *(cp)++ = (masklen);                \
            inetAddressAnd((esa),&mask,&Addr);     \
            MCAST_PUT_ADDR6((Addr.addr.ipv6.in6.addr8), (cp)); \
        } while(0)

#define PIM_GET_EGADDR_IPV6(ega, cp)                     \
        do {                                    \
            (ega)->addr_family = *(cp)++;       \
            (ega)->encod_type  = *(cp)++;       \
            (ega)->reserved    = *(cp)++;       \
            (ega)->masklen     = *(cp)++;       \
            MCAST_GET_ADDR6(((ega)->addr.ipv6.in6.addr8), (cp));  \
        } while(0)

#define PIM_PUT_EGADDR_IPV6(ega, masklen, reserved, cp) \
        do {                                    \
            L7_inet_addr_t mask;                       \
            L7_inet_addr_t Addr;                  \
            inetAddressReset(&Addr);         \
            inetAddressReset(&mask);         \
            inetMaskLenToMask(L7_AF_INET6, (masklen), &mask);   \
            *(cp)++ = L7_AF_INET6; /* family */  \
            *(cp)++ = L7_AF_INET6_ENC_TYPE; /* type   */  \
            *(cp)++ = (reserved); /* reserved; should be 0 */  \
            *(cp)++ = (masklen);                \
            inetAddressAnd((ega),&mask,&Addr);     \
            MCAST_PUT_ADDR6((Addr.addr.ipv6.in6.addr8), (cp)); \
        } while(0)

#define PIM_GET_EUADDR_IPV6(eua, cp)                     \
        do {                                    \
            (eua)->addr_family = *(cp)++;       \
            (eua)->encod_type  = *(cp)++;       \
            MCAST_GET_ADDR6(((eua)->addr.ipv6.in6.addr8), (cp));  \
        } while(0)

#define PIM_PUT_EUADDR_IPV6(euaddr, cp)                    \
        do {                                    \
            *(cp)++ = L7_AF_INET6; /* family */  \
            *(cp)++ = L7_AF_INET6_ENC_TYPE; /* type   */  \
            MCAST_PUT_ADDR6((euaddr)->addr.ipv6.in6.addr8, (cp)); \
        } while(0)        

/*Wrapper Macros */
/* NOTE: FOR ipv6, 
  if you are using this macros, please put
  #include "l3_addrdefs.h" in your .c file. */
  
#define PIM_GET_ESADDR_INET(esa, cp)        \
         do{ \
           if(*(cp) == L7_AF_INET)    \
           {                                \
             PIM_GET_ESADDR_IPV4((esa),(cp));    \
           }                                \
           else if(*(cp) == L7_AF_INET6)                            \
           {                                \
             PIM_GET_ESADDR_IPV6((esa),(cp));    \
           }                                    \
           else                                \
           {                                    \
             LOG_MSG("Invalid Address Family");   \
           }                                    \
         }while(0)

#define PIM_PUT_ESADDR_INET(esa, masklen, flags, cp)    \
         do{\
             if((esa)->family == L7_AF_INET) \
             {                              \
               PIM_PUT_ESADDR_IPV4((esa)->addr.ipv4.s_addr,(masklen),(flags),(cp)); \
             }                              \
             else if((esa)->family == L7_AF_INET6)        \
             {                                           \
               PIM_PUT_ESADDR_IPV6((esa),(masklen),(flags),(cp)); \
             }                                         \
             else                                \
             {                                    \
               LOG_MSG("Invalid Address Family");   \
             }                                       \
           }while(0)


#define PIM_GET_EGADDR_INET(ega, cp)                     \
        do{\
            if(*(cp) == L7_AF_INET)            \
            {                                \
              PIM_GET_EGADDR_IPV4((ega),(cp));   \
            }                               \
            else if(*(cp) == L7_AF_INET6) \
            {                               \
              PIM_GET_EGADDR_IPV6((ega),(cp));  \
            }                               \
            else                                \
            {                                    \
              LOG_MSG("Invalid Address Family");   \
             }                                       \
           }while(0)



#define PIM_PUT_EGADDR_INET(ega, masklen, reserved, cp) \
         do {                                \
             if((ega)->family == L7_AF_INET)  \
             {                               \
               PIM_PUT_EGADDR_IPV4((ega)->addr.ipv4.s_addr,(masklen),(reserved),(cp));  \
             }                               \
             else if((ega)->family == L7_AF_INET6) \
             {                                    \
               PIM_PUT_EGADDR_IPV6((ega),(masklen),(reserved),(cp)); \
             }                                    \
             else                                \
             {                                    \
               LOG_MSG("Invalid Address Family");   \
             }                                       \
           }while(0)

#define PIM_GET_EUADDR_INET(eua, cp)                     \
         do{ \
             if(*(cp) == L7_AF_INET) \
             {                     \
               PIM_GET_EUADDR_IPV4(eua,(cp));  \
             }                     \
             else if(*(cp) == L7_AF_INET6)      \
             {                               \
               PIM_GET_EUADDR_IPV6(eua,(cp));  \
             }                               \
             else                                \
             {                                    \
               LOG_MSG("Invalid Address Family");   \
             }                                       \
           }while(0)

#define PIM_PUT_EUADDR_INET(euaddr, cp) \
         do{                            \
             if((euaddr)->family == L7_AF_INET)  \
             {                               \
               PIM_PUT_EUADDR_IPV4((euaddr)->addr.ipv4.s_addr,(cp)); \
             }                               \
             else if((euaddr)->family == L7_AF_INET6) \
             {                                    \
               PIM_PUT_EUADDR_IPV6((euaddr),(cp)); \
             }                                    \
             else                                \
             {                                    \
               LOG_MSG("Invalid Address Family");   \
             }                                       \
           }while(0)

/*********************************************************************
* @purpose  Enables/Disables the PIM IPv6 Group address and interface ID with the
*           IP stack.
* 
* @param    rtrIfNum   @b{(input)} interface number
* @param    addFlag    @b{(input)} Indicates whether (group, Ifidx) needs to enable or 
*                      disable.
* @param    sockFd     @b{(input)} Socket Desc.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
* @end
*********************************************************************/
L7_RC_t pimV6SockChangeMembership(L7_uint32 rtrIfNum, L7_uint32 intfIfNum,
                              L7_uint32 addFlag, L7_uint32 sockfd);
L7_RC_t pimJoinPruneMsgLenGet(L7_uchar8 family,L7_uint32 *pJoinPruneMsgSize);             

L7_RC_t pimSrcMaskLenGet(L7_uchar8 family,L7_uchar8 *pMaskLen);
L7_RC_t pimGrpMaskLenGet(L7_uchar8 family,L7_uchar8 *pMaskLen);
L7_uint32 pimJoinPruneMaxAddrsInListGet(L7_uchar8 family);
#endif /* PIM_DEFS_H */

