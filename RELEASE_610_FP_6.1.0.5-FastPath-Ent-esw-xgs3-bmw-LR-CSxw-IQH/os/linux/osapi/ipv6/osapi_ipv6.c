/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  osapi_ipv6.c
*
* @purpose   Gets the ip tables from vxworks
*
* @component ip
*
* @comments  none
*
* @create    05/07/2001
*
* @author    anayar, cpverne
*
* @end
*
**********************************************************************/

/*************************************************************

************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/in.h>

#include "l7_common.h"
#include "bspapi.h"
#include "dtlapi.h"
#include "l3end_api.h" /* still need a better name for this header file... */
#include "ipstk_api.h"
#include "ipstk_mib_api.h"
#include "osapi_support.h"
#include "osapi_priv.h"
#include "ipv6_commdefs.h"

void osapiIpv6NdpCacheInit(void);

/*
********************************************************************
*                     LINUX GETTERS
********************************************************************
*/
#define IPSTK_NLMIB_BUFMAX  4096
L7_uint32 osapiIpeakNetlinkMibSeq = 0;

static L7_uchar8 osapiIpeakNetlinkMibBuf[IPSTK_NLMIB_BUFMAX];


/********************************************
lex compare of nbr  entry
*********************************************/
static int 
osapiV6NbrCmp(L7_uint32 idx1, L7_in6_addr_t *addr1,
            L7_uint32 idx2, L7_in6_addr_t *addr2)
{
    if(idx1 < idx2) return -1;
    if(idx1 > idx2) return  1;

    return memcmp(addr1,addr2,sizeof(L7_in6_addr_t));
}

typedef struct xxEntry_s{
  struct xxEntry_s    *fwd;
  struct xxEntry_s    *bwd;
  ipstkIpv6NetToMediaEntry_t  ndp;
}ndpDumpCacheEntry_t;
#define NDP_DUMPCACHE_SIZE  64 
ndpDumpCacheEntry_t  dumpNdpCache[NDP_DUMPCACHE_SIZE];


ndpDumpCacheEntry_t *dumpNdpCacheFreeListHead = 0;
ndpDumpCacheEntry_t *dumpNdpCacheFreeListTail = 0;
ndpDumpCacheEntry_t *dumpNdpCacheListHead = 0;
ndpDumpCacheEntry_t *dumpNdpCacheListTail = 0;

#define NDP_CACHE_APPEND_FREE(xxent) { \
    (xxent)->bwd = dumpNdpCacheFreeListTail; \
    if(dumpNdpCacheFreeListTail){ \
        dumpNdpCacheFreeListTail->fwd = xxent; \
    } \
    else{ \
        dumpNdpCacheFreeListHead = xxent; \
    } \
    dumpNdpCacheFreeListTail = xxent; \
    (xxent)->fwd = 0; \
}

#define NDP_CACHE_UNLINK_FREE(xxent) { \
    if((xxent)->bwd) \
        (xxent)->bwd->fwd = (xxent)->fwd; \
    else \
        dumpNdpCacheFreeListHead = (xxent)->fwd; \
    if((xxent)->fwd) \
        (xxent)->fwd->bwd = (xxent)->bwd; \
    else \
        dumpNdpCacheFreeListTail = (xxent)->bwd; \
}

#define NDP_CACHE_UNLINK(xxent) { \
    if((xxent)->bwd) \
        (xxent)->bwd->fwd = (xxent)->fwd; \
    else \
        dumpNdpCacheListHead = (xxent)->fwd; \
    if((xxent)->fwd) \
        (xxent)->fwd->bwd = (xxent)->bwd; \
    else \
        dumpNdpCacheListTail = (xxent)->bwd; \
}

void cacheCheck(void)
{
    ndpDumpCacheEntry_t *cache;
    int cmp;

    for(cache = dumpNdpCacheListHead; cache; cache = cache->fwd){
        if(cache == dumpNdpCacheListHead){
           if(cache->bwd)
             printf("yuk0\n");
        }
        else if(cache->bwd->fwd != cache)
             printf("yuk1\n");
        if(cache == dumpNdpCacheListTail){
           if(cache->fwd)
             printf("yuk2\n");
        }
        else if(cache->fwd->bwd != cache)
             printf("yuk3\n");
        if(cache->fwd){
           cmp = osapiV6NbrCmp(cache->ndp.ipv6IfIndex, &cache->ndp.ipv6NetToMediaNetAddress,
                        cache->fwd->ndp.ipv6IfIndex,&cache->fwd->ndp.ipv6NetToMediaNetAddress);
           if(cmp >= 0)
           {
             L7_uchar8 addr1Str[40];
             L7_uchar8 addr2Str[40];
             osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&cache->ndp.ipv6NetToMediaNetAddress, 
                           addr1Str, 40);
             osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&cache->fwd->ndp.ipv6NetToMediaNetAddress, 
                           addr2Str, 40);
             printf("\nCache entry for ifIndex %d address %s before entry for ifIndex %d address %s.",
                    cache->ndp.ipv6IfIndex, addr1Str, cache->fwd->ndp.ipv6IfIndex, addr2Str);
           }
        }
    }
}
       

/*********************************************************************
* @purpose  initialize ndp dump free list
*
* @param    
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void osapiIpv6NdpCacheInit(void)
{
    L7_uint32 i;
    ndpDumpCacheEntry_t *cache;

    /* a null call from monitor task */
    /* cache already initialized */
    if(dumpNdpCacheFreeListHead && !dumpNdpCacheListHead)
        return;

    /* the common case */
    if(dumpNdpCacheListHead && (dumpNdpCacheListHead == dumpNdpCacheListTail)){
        cache = dumpNdpCacheListHead;
        NDP_CACHE_UNLINK(cache);
        NDP_CACHE_APPEND_FREE(cache);
        return;
    }

    dumpNdpCacheListHead = L7_NULL;
    dumpNdpCacheListTail = L7_NULL;
    dumpNdpCacheFreeListHead = L7_NULL;
    dumpNdpCacheFreeListTail = L7_NULL;
    for(i = 0; i < NDP_DUMPCACHE_SIZE; i++)
    {
        NDP_CACHE_APPEND_FREE(&dumpNdpCache[i]);
    }
cacheCheck();
}
/*********************************************************************
* @purpose  add cache entry in proper order, replacing worst if necessary
*
* @param    
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void osapiNdpDumpCacheAdd(L7_uint32 intIfNum, L7_in6_addr_t *v6addr, L7_uint32 state,
                      L7_uint32 flags, L7_uchar8 *lladdr, L7_uint32 lastupdated)
{
    ndpDumpCacheEntry_t *cache, *tcache;
    int cmp;

    /* find free entry or replace worst */
    if(dumpNdpCacheFreeListHead){
         cache = dumpNdpCacheFreeListHead;
         NDP_CACHE_UNLINK_FREE(cache);
    }
    else{
         cache = dumpNdpCacheListTail;
         NDP_CACHE_UNLINK(cache);
    }
    /* build the entry */
    cache->ndp.ipv6IfIndex = intIfNum;
    cache->ndp.ipv6NetToMediaNetAddress = *v6addr;
    cache->ndp.ipv6NetToMediaPhysAddress.len = 6;
    memcpy(cache->ndp.ipv6NetToMediaPhysAddress.addr,lladdr,6);
    cache->ndp.ipv6IfNetToMediaType = 2;
    cache->ndp.ipv6IfNetToMediaLastUpdated = lastupdated;     /* not tracked */
    cache->ndp.ipv6IfNetToMediaIsRouter = (flags & NTF_ROUTER)?L7_TRUE:L7_FALSE;
    cache->ndp.ipv6IfNetToMediaValid = 1;
    switch(state)
    {
            case NUD_REACHABLE :
                   cache->ndp.ipv6IfNetToMediaState = L7_IP6_NBR_STATE_REACHABLE;  /* reachable */
                   break;
            case NUD_STALE :
                   cache->ndp.ipv6IfNetToMediaState = L7_IP6_NBR_STATE_STALE;  /* stale */
                   break;
            case NUD_DELAY :
                   cache->ndp.ipv6IfNetToMediaState = L7_IP6_NBR_STATE_DELAY;  /* delay */
                   break;
            case NUD_PROBE :
                   cache->ndp.ipv6IfNetToMediaState = L7_IP6_NBR_STATE_PROBE;  /* probe */
                   break;
            default:
                   cache->ndp.ipv6IfNetToMediaState = L7_IP6_NBR_STATE_UNKNOWN;  /* unknown */
                   break;
    }

    /* link in order */
    if(!dumpNdpCacheListHead){
        dumpNdpCacheListHead = cache;
        dumpNdpCacheListTail = cache;
        cache->fwd = cache->bwd = 0;
        return;
    }
    else for(tcache = dumpNdpCacheListHead; tcache; tcache = tcache->fwd){

        cmp = osapiV6NbrCmp(cache->ndp.ipv6IfIndex, &cache->ndp.ipv6NetToMediaNetAddress,
                        tcache->ndp.ipv6IfIndex,&tcache->ndp.ipv6NetToMediaNetAddress);
        if(cmp < 0){
            /* put in front of tcache */
            cache->fwd = tcache;
            cache->bwd = tcache->bwd;
            if(tcache->bwd){
                tcache->bwd->fwd = cache;
            }
            else{
                dumpNdpCacheListHead = cache;
            }
            tcache->bwd = cache;
            return;
        }
    }
    /* if we get here, its because we replaced the tail */
    tcache = dumpNdpCacheListTail;
    tcache->fwd = cache;
    cache->fwd = L7_NULL;
    cache->bwd = tcache;
    dumpNdpCacheListTail = cache;

}

/*********************************************************************
* @purpose  append new "worst" entry if cache not full
*
* @param    
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void osapiNdpDumpCacheTryAppend(L7_uint32 intIfNum, L7_in6_addr_t *v6addr, L7_uint32 state,
                      L7_uint32 flags, L7_uchar8 *lladdr, L7_uint32 lastupdated)
{
    ndpDumpCacheEntry_t *cache, *tcache;

    /* find free entry or replace worst */
    if(dumpNdpCacheFreeListHead){
         cache = dumpNdpCacheFreeListHead;
         NDP_CACHE_UNLINK_FREE(cache);
    }
    else
         return;

    /* build the entry */
    cache->ndp.ipv6IfIndex = intIfNum;
    cache->ndp.ipv6NetToMediaNetAddress = *v6addr;
    cache->ndp.ipv6NetToMediaPhysAddress.len = 6;
    memcpy(cache->ndp.ipv6NetToMediaPhysAddress.addr,lladdr,6);
    cache->ndp.ipv6IfNetToMediaType = 2;
    cache->ndp.ipv6IfNetToMediaLastUpdated = lastupdated;     
    cache->ndp.ipv6IfNetToMediaIsRouter = (flags & NTF_ROUTER)?L7_TRUE:L7_FALSE;
    cache->ndp.ipv6IfNetToMediaValid = 1;
    switch(state)
    {
            case NUD_REACHABLE :
                   cache->ndp.ipv6IfNetToMediaState = L7_IP6_NBR_STATE_REACHABLE;  /* reachable */
                   break;
            case NUD_STALE :
                   cache->ndp.ipv6IfNetToMediaState = L7_IP6_NBR_STATE_STALE;  /* stale */
                   break;
            case NUD_DELAY :
                   cache->ndp.ipv6IfNetToMediaState = L7_IP6_NBR_STATE_DELAY;  /* delay */
                   break;
            case NUD_PROBE :
                   cache->ndp.ipv6IfNetToMediaState = L7_IP6_NBR_STATE_PROBE;  /* probe */
                   break;
            default:
                   cache->ndp.ipv6IfNetToMediaState = L7_IP6_NBR_STATE_UNKNOWN;  /* unknown */
                   break;
    }

    /* link at end */
    tcache = dumpNdpCacheListTail;

    tcache->fwd = cache;
    cache->fwd = L7_NULL;
    cache->bwd = tcache;
    dumpNdpCacheListTail = cache;

}

/*********************************************************************
* @purpose  get NDisc info from stack
*
* @param    get_all     L7_TRUE, all interfaces, L7_FALSE router only
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    caches ordered subset of entries for next call in order to mitigate
*           n**2 issue.
*
* @end
*********************************************************************/
L7_RC_t osapiIpv6NetToMediaEntryGet( L7_uint32 searchType, L7_BOOL get_all, ipstkIpv6NetToMediaEntry_t *vars)
{
    int rc, cmp,sd, ndmlen,totlen;
    struct nlmsghdr   *nlmsg;
    L7_uint32 seq = ++osapiIpeakNetlinkMibSeq;
    L7_uint32 tid;
    fd_set rfd;
    struct ndmsg *ndm;
    struct rtattr *dst;
    L7_uint32 dummy = 0;
    L7_uchar8  *lladdr;
    L7_in6_addr_t *dst_v6_addr;
    L7_uint32 intIfNum;
    L7_uint32 pending = 1;
    struct msghdr rmsg;
    struct iovec siov;
    ndpDumpCacheEntry_t *cache;
    L7_BOOL exact_match = L7_FALSE;
    struct nda_cacheinfo *ci_info;
    L7_uint32 neigh_lastupdated = 0;
    L7_uint32 numDups = 0;

cacheCheck();
    /* find best entry in cache */
    /* under normal cases, first cache entry is last entry returned in walk */
    /* so each time we return an entry, we unlink the previous */
    if(searchType == L7_MATCH_GETNEXT){
      for(cache = dumpNdpCacheListHead; cache; cache = cache->fwd){
        cmp = osapiV6NbrCmp(cache->ndp.ipv6IfIndex, &cache->ndp.ipv6NetToMediaNetAddress,
                        vars->ipv6IfIndex,&vars->ipv6NetToMediaNetAddress);
        if(cmp < 0)
            continue;
        else if(cmp == 0){
cacheCheck();

            *vars = cache->ndp;
            cache = dumpNdpCacheListHead;

            NDP_CACHE_UNLINK(cache);
            NDP_CACHE_APPEND_FREE(cache);
cacheCheck();
            return L7_SUCCESS;

        }
        else if(cache == dumpNdpCacheListHead){
            /* probably start of a new dump */
            break;
        }
        else if(searchType == L7_MATCH_GETNEXT){
cacheCheck();
            /* got it */
            *vars = cache->ndp;
            cache = dumpNdpCacheListHead;
            NDP_CACHE_UNLINK(cache);
            NDP_CACHE_APPEND_FREE(cache);
cacheCheck();
            return L7_SUCCESS;
        }
      }
        
    }
cacheCheck();
    osapiIpv6NdpCacheInit();



    tid = osapiGetpid();

    nlmsg = (struct nlmsghdr *)osapiIpeakNetlinkMibBuf;
    ndm = NLMSG_DATA(nlmsg);
    ndm->ndm_family = AF_INET6;
    ndmlen = sizeof(struct ndmsg);

    nlmsg->nlmsg_len = NLMSG_LENGTH(ndmlen);
    nlmsg->nlmsg_type = (L7_ushort16)RTM_GETNEIGH;
    nlmsg->nlmsg_flags = NLM_F_REQUEST|NLM_F_DUMP;
    nlmsg->nlmsg_pid = tid;
    nlmsg->nlmsg_seq = seq;


    /* do we have space */
    if(nlmsg->nlmsg_len > IPSTK_NLMIB_BUFMAX)
       return L7_FAILURE;


    /*
     *get an open netlink socket
     */
    sd = socket(AF_NETLINK,SOCK_RAW,0);
    if (sd < 0)
    {
      L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
            "osapiNetLinkNeighDump: could not open socket!"
            " Couldn’t open a “netlink” socket. Make sure “ARP Daemon "
            "support” (CONFIG_ARPD) is enabled in the Linux kernel, if "
            "the LVL7 reference kernel binary is not being used.\n");
      close(sd);
      return L7_FAILURE;
    }

    totlen = nlmsg->nlmsg_len;
    rc = send(sd,(L7_uchar8 *)nlmsg, totlen,0);
    /* NB: netlink dump always returns -1 */

    osapiSocketNonBlockingModeSet(sd,L7_TRUE);

    /* handle multi-part messages */
    while(pending)
    {
      nlmsg = (struct nlmsghdr *)osapiIpeakNetlinkMibBuf;

      /* wait for response */
      while(1)
      {
         struct timeval tmout;

         /* netlink is unreliable (eg malloc fail): dont hang */
         FD_ZERO(&rfd);
         FD_SET(sd, &rfd);
         tmout.tv_sec = 2;
         tmout.tv_usec = 0;

         rc = select(sd+1,&rfd,0,0,&tmout);
         if(rc <= 0){
            close(sd);
            return L7_FAILURE;
         }
         if (!FD_ISSET(sd, &rfd))
         {
           continue;
         }
         
         rmsg.msg_name = 0;
         rmsg.msg_namelen = 0;
         rmsg.msg_iov = &siov;
         rmsg.msg_iovlen = 1;
         rmsg.msg_control = 0;
         rmsg.msg_controllen = 0;
         rmsg.msg_flags = 0;

         memset(osapiIpeakNetlinkMibBuf, 0, IPSTK_NLMIB_BUFMAX);
         siov.iov_base = &osapiIpeakNetlinkMibBuf[0];
         siov.iov_len = IPSTK_NLMIB_BUFMAX;
         rc = recvmsg(sd,&rmsg, 0);

         if(rc <= 0){
            close(sd);
            return L7_FAILURE;
         }
         if(rmsg.msg_flags & MSG_TRUNC){
           LOG_MSG("osapiIpv6NetToMediaEntryGet: truncated data");
         }

         if((nlmsg->nlmsg_type == (L7_ushort16)RTM_NEWNEIGH) &&
            /* dont know why this is not us.
            (nlmsg->nlmsg_pid == tid) &&
            */
            (nlmsg->nlmsg_seq == seq)){
            break;
         }
         if((nlmsg->nlmsg_type == (L7_ushort16)NLMSG_DONE) &&
            /* dont know why this is not us.
            (nlmsg->nlmsg_pid == tid) &&
            */
            (nlmsg->nlmsg_seq == seq)){
            break;
         }
      }

    while (NLMSG_OK(nlmsg,rc))
    {
      if (nlmsg->nlmsg_type == (L7_ushort16)NLMSG_DONE)
      {
        pending = 0;
        break;
      }
      if (nlmsg->nlmsg_type == RTM_NEWNEIGH)
      {
        ndm = NLMSG_DATA(nlmsg);
        if (ndm->ndm_family != AF_INET6)
        {
          goto ndp_next;
        }
        /* for get_all, ifIndex is used instead of intIfNum to all get of non-router
           ports.
        */
        if (get_all == L7_TRUE)
        {
          intIfNum = ndm->ndm_ifindex;
        }
        else
        {
          if (ipstkStackIfIndexToIntIfNum(ndm->ndm_ifindex,&intIfNum) != L7_SUCCESS)
          {
            goto ndp_next;
          }
        }

        if (intIfNum == 0)
        {
          goto ndp_next;
        }

	/* Get attributes of this entry */
	dst_v6_addr = NULL;
	lladdr = NULL;
	ci_info = NULL;	
        dst = ((struct rtattr*)(((char*)(ndm)) + NLMSG_ALIGN(sizeof(struct ndmsg))));;
	while(RTA_OK(dst, rc-NLMSG_ALIGN(sizeof(struct ndmsg)))) {
	  if (dst->rta_type == NDA_DST) {
	    dst_v6_addr = (L7_in6_addr_t *)RTA_DATA(dst);
	    if (L7_IP6_IS_ADDR_UNSPECIFIED(dst_v6_addr)) {
	      goto ndp_next;
	    }
	    if (L7_IP6_IS_ADDR_MULTICAST(dst_v6_addr)) {
	      goto ndp_next;
	    }
	  }
	  if (dst->rta_type == NDA_LLADDR) {
	    lladdr = RTA_DATA(dst);
	  }
	  if (dst->rta_type == NDA_CACHEINFO) {
	    ci_info = RTA_DATA(dst);
	  }
	  dst = RTA_NEXT(dst,dummy);	  
	}
	if ((ci_info == NULL) || (lladdr == NULL) || (dst_v6_addr == NULL)) {
	  goto ndp_next;
	} 

        /* We changed the linux 2.6 kernel to report in msec rather 
         * than jiffies, since the length of a jiffy can vary from 
         * platform to platform. */
        neigh_lastupdated = ci_info->ndm_updated/1000;      

        cmp = osapiV6NbrCmp(intIfNum, dst_v6_addr, 
                            vars->ipv6IfIndex,&vars->ipv6NetToMediaNetAddress);

        if ( cmp > 0 )
        {
          /* entry returned from stack is lexically beyond the input search value */
          /* compare new nbr with tail of cache */
          cache = dumpNdpCacheListTail;
          int cmpTail = 0;
          if (cache)
          {
            cmpTail = osapiV6NbrCmp(intIfNum, dst_v6_addr, 
                                    cache->ndp.ipv6IfIndex,
                                    &cache->ndp.ipv6NetToMediaNetAddress);
          }

          if ((cache == 0) || (cmpTail < 0))
          {
            /* Cache is empty or new nbr is lexically before cache tail.
             * Add even if that means replacing another cache entry */
            osapiNdpDumpCacheAdd(intIfNum, dst_v6_addr, ndm->ndm_state, 
                                 ndm->ndm_flags, lladdr, neigh_lastupdated );
          }
          else if (cmpTail > 0)
          {
            /* greater than tail cache value; add only if cache not full */
            osapiNdpDumpCacheTryAppend(intIfNum, dst_v6_addr, ndm->ndm_state, 
                                       ndm->ndm_flags, lladdr, neigh_lastupdated );
          }
          /* entry already in cache. Maybe should bail out here to avoid 
           * apparent hang if stack keeps returning same set of neighbors. */
          else if (numDups++ > 5)
          {
            pending = 0;
          }
        }
        else if (cmp == 0)
        {
          /* agent autoincrement's so return exact match even for getnext */
          exact_match = L7_TRUE;
          osapiNdpDumpCacheAdd(intIfNum, dst_v6_addr, ndm->ndm_state, 
                               ndm->ndm_flags, lladdr, neigh_lastupdated);
        }
      }

ndp_next:
        /* next msg */
        nlmsg = NLMSG_NEXT(nlmsg,rc);
      } /* end while nlmsg */

    } /* end while pending */

    close(sd);

    if((exact_match == L7_TRUE) || (dumpNdpCacheListHead && (searchType == L7_MATCH_GETNEXT)))
    {
        *vars = dumpNdpCacheListHead->ndp;
        return L7_SUCCESS;
    }
    else{
        return L7_FAILURE;
    }
}





int osapiIpeakNetLinkRtaPut(void *buf, int attrtype, int attrlen, void *data)
{
    struct rtattr *rta = (struct rtattr *)buf;
    
    rta->rta_type = (L7_ushort16) attrtype;
    rta->rta_len  = (L7_ushort16) RTA_LENGTH (attrlen);

    if (data != L7_NULLPTR)
        memcpy (RTA_DATA(rta), data, attrlen);

    return rta->rta_len;
}


#ifdef L7_ROUTING_PACKAGE
/**************************************************************************
*
* @purpose  Delete all NDP entries from the operating system NDP table.
*           Or single entry if ip6Addr not NULL
*
* @param       intIfNum   internal interface number
* @param       ip6Addr    pointer to NDP address
*
* @returns     none
*
* @comments    
*
* @end
*
*************************************************************************/
void osapiIpeakIpv6NdpAdd( L7_uint32 intIfNum, L7_in6_addr_t *ip6Addr)
{
  int sd; /*socket descriptior*/
  int rc; /*return code*/
  L7_uint32 sequence = 0;
  L7_uint32 ifIndex;
  int messages_pending;
  L7_uint32 tid,tabno,msglen,dummylen = 0;
  struct rtattr *rta;
  struct ndmsg *ndm;
  L7_uchar8  mac[6];

  struct del_msg
  {
    struct nlmsghdr n;
    L7_uchar8 buf[256];
  }dmsg;


  messages_pending = 1;

  if((intIfNum != 0) && (ipstkIntIfNumToStackIfIndex(intIfNum,&ifIndex) != L7_SUCCESS))
    return;

  /*
   *get an open netlink socket
   */
  sd = socket(AF_NETLINK,SOCK_RAW,0);
  if (sd < 0)
  {
    L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
            "ipstkNdpFlush: could not open socket!"
            " Couldn’t open a “netlink” socket. Make sure “ARP Daemon "
            "support” (CONFIG_ARPD) is enabled in the Linux kernel, if "
            "the LVL7 reference kernel binary is not being used.\n");
    close(sd);
    return;
  }

  tid = osapiGetpid();


      /*
       *this is our interface name.  WE can now send the delete message
       */
      dmsg.n.nlmsg_type = RTM_NEWNEIGH;
      dmsg.n.nlmsg_flags = NLM_F_REQUEST|NLM_F_CREATE;
      dmsg.n.nlmsg_seq = ++sequence;
      dmsg.n.nlmsg_pid = tid;

      ndm = NLMSG_DATA(&dmsg);
      ndm->ndm_family = AF_INET6;
      ndm->ndm_flags  = 0;
      ndm->ndm_type   = RTM_NEWNEIGH;
      ndm->ndm_state  = NUD_REACHABLE;
      ndm->ndm_ifindex = ifIndex;
      
      rta    = ((struct rtattr*)(((char*)(ndm)) + NLMSG_ALIGN(sizeof(struct ndmsg))));;
      msglen = NLMSG_ALIGN(sizeof(struct ndmsg));
    
      /* Put destination address */
      (void)osapiIpeakNetLinkRtaPut(rta, 
                                NDA_DST, 
                                sizeof(L7_in6_addr_t),ip6Addr);

      rta = RTA_NEXT(rta, dummylen);
      msglen += RTA_LENGTH(RTA_ALIGN(sizeof(L7_in6_addr_t)));
    
      /* Put link layer address */
      (void)osapiIpeakNetLinkRtaPut(rta,
                                NDA_LLADDR, 
                                6,mac);

      rta = RTA_NEXT(rta, dummylen);
      msglen += RTA_SPACE(6);

      /* Put route table idx */
      tabno = 0;
    
      dmsg.n.nlmsg_len = NLMSG_LENGTH(msglen);
    

      rc = send(sd,&dmsg.n,dmsg.n.nlmsg_len,0);
      if (rc < 0)
      {
perror("fred\n");
          LOG_MSG("ipstkNdpFlush: sending delete failed\n");
          close(sd);
          return;
      }

  close(sd);
  return;
}

#if 0
/* tbd: rm this debug code */
void debugAdd( L7_uint32 nbr_cnt, L7_uchar8 *astr)
{
   L7_in6_addr_t addr;
   L7_uint32 i;
   
   inet_pton(AF_INET6, astr, &addr);
   for(i = 0; i < nbr_cnt; i++){
      addr.in6.addr16[6]++;
      osapiIpeakIpv6NdpAdd(1, &addr);
   }
}
#endif




/*********************************************************************
* @purpose  get v6 general group info from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    only read hop_limit once since we cant change it.
*
* @end
*********************************************************************/
static L7_BOOL have_cache_hop_limit = L7_FALSE;
static L7_uint32 cache_hop_limit;
L7_RC_t osapiIpv6GeneralGroupGet( ipstkIpv6GeneralGroup_t *vars)
{
  FILE *in;
  char line [64];

  memset(vars,0,sizeof(*vars));

  if(have_cache_hop_limit == L7_FALSE)
  {
     in = fopen ("/proc/sys/net/ipv6/conf/default/hop_limit", "r");
     if(!in) {
       L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID,
              "unable to open /proc/net/ipv6/conf/default/hop_limit!"
              " IPv6 MIB objects read, but /proc filesystem is "
              "not mounted, or running kernel does not have IPV6 support");
       return L7_FAILURE;
     }
     fgets (line, sizeof(line), in);
     cache_hop_limit = atoi(line);
     have_cache_hop_limit= L7_TRUE;
     fclose(in);
  }
  vars->ipv6DefaultHopLimit = cache_hop_limit;

  return L7_SUCCESS;

}

typedef struct ipv6ProcSnmp_s{
   L7_uchar8   *sname;
   L7_uint32   offset;
}ipv6ProcSnmp_t;

ipstkIpv6IfStats_t dumIfVars;
#define PROCSNMP_IP6IF_OFF(x) (&dumIfVars.x - (L7_uint32 *)&dumIfVars)

ipv6ProcSnmp_t ipv6ProcSnmpIfStrs[] ={
{"Ip6InReceives", PROCSNMP_IP6IF_OFF(ipv6IfStatsInReceives)},
{"Ip6InHdrErrors", PROCSNMP_IP6IF_OFF(ipv6IfStatsInHdrErrors)},
{"Ip6InTooBigErrors", PROCSNMP_IP6IF_OFF(ipv6IfStatsInTooBigErrors)},
{"Ip6InNoRoutes", PROCSNMP_IP6IF_OFF(ipv6IfStatsInNoRoutes)},
{"Ip6InAddrErrors", PROCSNMP_IP6IF_OFF(ipv6IfStatsInAddrErrors)},
{"Ip6InUnknownProtos", PROCSNMP_IP6IF_OFF(ipv6IfStatsInUnknownProtos)},
{"Ip6InTruncatedPkts", PROCSNMP_IP6IF_OFF(ipv6IfStatsInTruncatedPkts)},
{"Ip6InDiscards", PROCSNMP_IP6IF_OFF(ipv6IfStatsInDiscards)},
{"Ip6InDelivers", PROCSNMP_IP6IF_OFF(ipv6IfStatsInDelivers)},
{"Ip6OutForwDatagrams", PROCSNMP_IP6IF_OFF(ipv6IfStatsOutForwDatagrams)},
{"Ip6OutRequests", PROCSNMP_IP6IF_OFF(ipv6IfStatsOutRequests)},
{"Ip6OutDiscards", PROCSNMP_IP6IF_OFF(ipv6IfStatsOutDiscards)},
{"Ip6ReasmReqds", PROCSNMP_IP6IF_OFF(ipv6IfStatsReasmReqds)},
{"Ip6ReasmOKs", PROCSNMP_IP6IF_OFF(ipv6IfStatsReasmOKs)},
{"Ip6ReasmFails", PROCSNMP_IP6IF_OFF(ipv6IfStatsReasmFails)},
{"Ip6FragOKs", PROCSNMP_IP6IF_OFF(ipv6IfStatsOutFragOKs)},
{"Ip6FragFails", PROCSNMP_IP6IF_OFF(ipv6IfStatsOutFragFails)},
{"Ip6FragCreates", PROCSNMP_IP6IF_OFF(ipv6IfStatsOutFragCreates)},
{"Ip6InMcastPkts", PROCSNMP_IP6IF_OFF(ipv6IfStatsInMcastPkts)},
{"Ip6OutMcastPkts", PROCSNMP_IP6IF_OFF(ipv6IfStatsOutMcastPkts)},
{0,0}
};


/* tbd: make these use per interface stats */

/*********************************************************************
* @purpose  get interface stats from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIpv6IfStatsGet( L7_uint32 searchType, ipstkIpv6IfStats_t *vars)
{

  L7_uint32 ifi_sav = vars->ipv6IfIndex;
  FILE *in;
  L7_uchar8 line [1024];
  L7_uchar8 *sp;
  L7_uchar8 scan_buf[80];
  L7_uint32 i,lineno,val;
  L7_uchar8 ifName[IFNAMSIZ], fname[80];

  if (osapiIfNameStringGet(vars->ipv6IfIndex,ifName,IFNAMSIZ) != L7_SUCCESS)
  {
    LOG_MSG("intIfNum %d: unable to get interface name\n", ifi_sav);
    return L7_FAILURE; 
  }

  sprintf(fname, "/proc/net/dev_snmp6/%s", ifName);

  memset(vars,0,sizeof(*vars));
  vars->ipv6IfIndex = ifi_sav;

  in = fopen (fname, "r");
  if(!in) {
    printf("unable to open /proc/net/dev_snmp6/%s!\n", ifName);
    return L7_FAILURE;
  }


  lineno =0;
  while (line == (L7_uchar8 *)fgets (line, sizeof(line), in)) {

    if(strncmp(line,"Ip6", 3))
      continue;

    /* in order case */
    if(((sp =ipv6ProcSnmpIfStrs[lineno].sname) != 0) &&
      !strncmp( line, sp, strlen(sp))){

        strcpy(scan_buf, sp);
        strcat(scan_buf," %d");
        sscanf  ( line, scan_buf,&val);
        *((L7_uint32 *)vars + ipv6ProcSnmpIfStrs[lineno].offset) = val;
    }
    else{
        /* out of order case */
        i = 0;
        sp = ipv6ProcSnmpIfStrs[i].sname;
        while(sp){
          if (!strncmp( line, sp, strlen(sp))){
            strcpy(scan_buf, sp);
            strcat(scan_buf," %d");
            sscanf  ( line, scan_buf,&val);
            *((L7_uint32 *)vars + ipv6ProcSnmpIfStrs[i].offset) = val;
            break;
          }
          i++;
          sp = ipv6ProcSnmpIfStrs[i].sname;
        }
    }
    if(sp)lineno++;
  }
  fclose (in);

  return L7_SUCCESS;

}

ipstkIpv6IfIcmpStats_t dumVars;
#define PROCSNMP_ICMP6_OFF(x) (&dumVars.x - (L7_uint32 *)&dumVars)
ipv6ProcSnmp_t ipv6ProcSnmpIcmpStrs[] ={
{"Icmp6InMsgs", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInMsgs)},
{"Icmp6InErrors", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInErrors)},
{"Icmp6InDestUnreachs", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInDestUnreachs)},
{"Icmp6InPktTooBigs", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInPktTooBigs)},
{"Icmp6InTimeExcds", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInTimeExcds)},
{"Icmp6InParmProblems", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInParmProblems)},
{"Icmp6InEchos", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInEchos)},
{"Icmp6InEchoReplies", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInEchoReplies)},
{"Icmp6InGroupMembQueries", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInGroupMembQueries)},
{"Icmp6InGroupMembResponses", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInGroupMembResponses)},
{"Icmp6InGroupMembReductions", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInGroupMembReductions)},
{"Icmp6InRouterSolicits", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInRouterSolicits)},
{"Icmp6InRouterAdvertisements", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInRouterAdvertisements)},
{"Icmp6InNeighborSolicits", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInNeighborSolicits)},
{"Icmp6InNeighborAdvertisements", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInNeighborAdvertisements)},
{"Icmp6InRedirects", PROCSNMP_ICMP6_OFF(ipv6IfIcmpInRedirects)},
{"Icmp6OutMsgs", PROCSNMP_ICMP6_OFF(ipv6IfIcmpOutMsgs)},
{"Icmp6OutDestUnreachs", PROCSNMP_ICMP6_OFF(ipv6IfIcmpOutDestUnreachs)},
{"Icmp6OutPktTooBigs", PROCSNMP_ICMP6_OFF(ipv6IfIcmpOutPktTooBigs)},
{"Icmp6OutTimeExcds", PROCSNMP_ICMP6_OFF(ipv6IfIcmpOutTimeExcds)},
{"Icmp6OutParmProblems", PROCSNMP_ICMP6_OFF(ipv6IfIcmpOutParmProblems)},
{"Icmp6OutEchos", PROCSNMP_ICMP6_OFF(ipv6IfIcmpOutEchos)},
{"Icmp6OutEchoReplies", PROCSNMP_ICMP6_OFF(ipv6IfIcmpOutEchoReplies)},
{"Icmp6OutRouterSolicits", PROCSNMP_ICMP6_OFF(ipv6IfIcmpOutRouterSolicits)},
{"Icmp6OutRouterAdvertisements", PROCSNMP_ICMP6_OFF(ipv6IfIcmpOutRouterAdvertisements)},
{"Icmp6OutNeighborSolicits", PROCSNMP_ICMP6_OFF(ipv6IfIcmpOutNeighborSolicits)},
{"Icmp6OutNeighborAdvertisements", PROCSNMP_ICMP6_OFF(ipv6IfIcmpOutNeighborAdvertisements)},
{"Icmp6OutRedirects", PROCSNMP_ICMP6_OFF(ipv6IfIcmpOutRedirects)},
{"Icmp6OutGroupMembResponses", PROCSNMP_ICMP6_OFF(ipv6IfIcmpOutGroupMembResponses)},
{"Icmp6OutGroupMembReductions", PROCSNMP_ICMP6_OFF(ipv6IfIcmpOutGroupMembReductions)},
{"Icmp6DadFails", PROCSNMP_ICMP6_OFF(ipv6IfIcmpDupAddrDetects)},
{0, 0 }
};

/*********************************************************************
* @purpose  get interface icmp stats from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIpv6IfIcmpStatsGet( L7_uint32 searchType, ipstkIpv6IfIcmpStats_t *vars)
{
  L7_uint32 ifi_sav = vars->ipv6IfIndex;
  FILE *in;
  L7_uchar8 line [1024];
  L7_uchar8 *sp;
  L7_uchar8 scan_buf[80];
  L7_uint32 i,lineno,val;
  L7_uchar8 ifName[IFNAMSIZ], fname[80];

  if (osapiIfNameStringGet(vars->ipv6IfIndex,ifName,IFNAMSIZ) != L7_SUCCESS)
  {
    LOG_MSG("intIfNum %d: unable to get interface name\n", ifi_sav);
    return L7_FAILURE; 
  }

  sprintf(fname, "/proc/net/dev_snmp6/%s", ifName);

  memset(vars,0,sizeof(*vars));
  vars->ipv6IfIndex = ifi_sav;

  in = fopen (fname, "r");
  if(!in) {
    printf("unable to open /proc/net/dev_snmp6!\n");
    return L7_FAILURE;
  }



  lineno =0;
  while (line == (L7_uchar8 *)fgets (line, sizeof(line), in)) {

    if(strncmp(line,"Icmp6", 5))
      continue;

    /* in order case */
    if(((sp =ipv6ProcSnmpIcmpStrs[lineno].sname) != 0) &&
      !strncmp( line, sp, strlen(sp))){

        strcpy(scan_buf, sp);
        strcat(scan_buf," %d");
        sscanf  ( line, scan_buf,&val);
        *((L7_uint32 *)vars + ipv6ProcSnmpIcmpStrs[lineno].offset) = val;
    }
    else{
        /* out of order case */
        i = 0;
        sp = ipv6ProcSnmpIcmpStrs[i].sname;
        while(sp){
          if (!strncmp( line, sp, strlen(sp))){
            strcpy(scan_buf, sp);
            strcat(scan_buf," %d");
            sscanf  ( line, scan_buf,&val);
            *((L7_uint32 *)vars + ipv6ProcSnmpIcmpStrs[i].offset) = val;
            break;
          }
          i++;
          sp = ipv6ProcSnmpIcmpStrs[i].sname;
        }
    }
    if(sp)lineno++;
  }
  fclose (in);

  return L7_SUCCESS;

}

#endif /* ROUTING PACKAGE */
