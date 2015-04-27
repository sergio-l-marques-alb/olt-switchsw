/*
 * Copyright (c) 1983, 1988, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgment:
 *  This product includes software developed by the University of
 *  California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/sbin/routed/main.c,v 1.11 1999/09/05 17:49:11 peter Exp $
 */

#include "defs.h"
#include "pathnames.h"
#include "l7_common.h"
#include "l7_common_l3.h"
#include "osapi.h"
#include "rip_util.h"
#include "osapi_support.h"
#include <net/if.h>

#if !defined(sgi) && !defined(__NetBSD__)
char copyright[] =
"@(#) Copyright (c) 1983, 1988, 1993\n\
    The Regents of the University of California.  All rights reserved.\n";
static char sccsid[] __attribute__((unused)) = "@(#)main.c  8.1 (Berkeley) 6/5/93";
#elif defined(__NetBSD__)
__RCSID("$NetBSD$");
__COPYRIGHT("@(#) Copyright (c) 1983, 1988, 1993\n\
    The Regents of the University of California.  All rights reserved.\n");
#endif


extern ripInfo_t    *pRipInfo;

pid_t   mypid;

naddr   myaddr;             /* system address */
char    myname[MAXHOSTNAMELEN+1];

int verbose;

int supplier;           /* supply or broadcast updates */
int supplier_set;
int ipforwarding = 1;       /* kernel forwarding on */

int default_gateway;        /* 1=advertise default */
int background = 1;
int ridhosts;           /* 1=reduce host routes */
int mhome;              /* 1=want multi-homed host route */
int advertise_mhome;        /* 1=must continue advertising it */
int auth_ok = 1;            /* 1=ignore auth if we do not care */

struct timeval epoch;           /* when started */
struct timeval clk, prev_clk;
static int usec_fudge;
struct timeval now;         /* current idea of time */
time_t  now_stale;
time_t  now_expire;
time_t  now_garbage;

struct timeval next_bcast;      /* next general broadcast */
struct timeval no_flash = {EPOCH+SUPPLY_INTERVAL};  /* inhibit flash update */

struct timeval flush_kern_timer;

fd_set  fdbits;
int sock_max;
int rip_sock = -1;      /* RIP receive socket */

/* LVL7_MOD start */
struct rip_global_mibstats rip_glob_mibstats;   /* RFC 1724 RIPv2 MIB Stats */

/* array of structures where each entry caches
 * neighbor router's specific data.
 * One entry per neighbor */
struct nbr_specific_data    nbr_data[NUM_RIP_NBRS];
struct nbr_specific_data    tmp_data[NUM_RIP_NBRS];

int tracelevel = 0;         /* changed via ripMapVendTraceModeSet() */
int new_tracelevel = 0;
FILE *ftrace = (FILE *)1;   /* (was stdout in original code, now just a flag) */
#ifdef LVL7_ORIGINAL_VENDOR_CODE
static int on_bio;          /* use for file I/O IOCTL arg */
#else
#endif /* LVL7_ORIGINAL_VENDOR_CODE */

#ifdef LVL7_ORIGINAL_VENDOR_CODE
#else
int need_ifinit_flag=0;
int intfPipe=-1;
int intfPipeMsgLen=-1;
char * intfPipeBuf;
ripIntfCom_t      intfCom;            /* Used for communication from mapping to vendor*/
int ripSplitHorizon;
int ripAutoSummary;
int ripHostRoutesAccept;
#endif /* LVL7_ORIGINAL_VENDOR_CODE */

static void ripUpdateTimebase( struct timeval *wt );
void ripProcessPipeMsg();
void ripDefaultRouteMetricSet(unsigned int mode, unsigned int metric);
void ripRedistributeRouteAdd(uint dst, uint mask,
                                         uint gw, uint metric, rip_redist_proto rip_proto);
void ripRedistributeRouteDelete(uint dst, uint mask,
                                uint gw, rip_redist_proto proto);
/* LVL7_MOD end */

static  int get_rip_sock(naddr, int);
static void timevalsub(struct timeval *, struct timeval *, struct timeval *);

void rip_Task(void)
{
  int n;
  struct timeval wtime, wtime_temp, t2;
  fd_set ibits;
  struct interface *ifp;

  /* establish values for globals */
  supplier = 1;
  supplier_set = 1;
  ridhosts = 0;
  mhome = 1;
  advertise_mhome = 0;  /* initially, but may get updated during operation */
  ipforwarding = 1;
  myaddr = 0;
  /* Ignore authentication if we do not care.
   * Crazy as it is, that is what RFC 1723 requires.
   */
  auth_ok = 0;

    ripgettimeofday(&clk, 0);
    prev_clk = clk;
    epoch = clk;
    epoch.tv_sec -= EPOCH;
    now.tv_sec = EPOCH;
    now_stale = EPOCH - STALE_TIME;
    now_expire = EPOCH - EXPIRE_TIME;
    now_garbage = EPOCH - GARBAGE_TIME;
    wtime.tv_sec = 0;


    fix_select();

    /* Initialize the trace function */
    ripDebugSystemInitalize();
    ripMapVendRipIntfPipeGet(&intfPipe, &intfPipeMsgLen);
    if (intfPipe != -1 && intfPipeMsgLen != -1)
    {
      intfPipeBuf = (char *)osapiMalloc(L7_RIP_MAP_COMPONENT_ID, intfPipeMsgLen+1);
    }
    ripMapVendGlobalsGet(&ripSplitHorizon, &ripAutoSummary, &ripHostRoutesAccept);

    if (ripMapVendRnTreeCritEnter())
      LOG_ERROR(0);

    fix_select();


    /*if (tracename != 0) {
        strncpy(inittracename, tracename, sizeof(inittracename)-1);
        set_tracefile(inittracename, "%s", -1);
    } else {
        tracelevel_msg("%s", -1);*/   /* turn on tracing to stdio */
    /*}*/

    bufinit();

    /* initialize radix tree */
    rtinit();

    /* Pick a random part of the second for our output to minimize
     * collisions.
     *
     * Start broadcasting after hearing from other routers, and
     * at a random time so a bunch of systems do not get synchronized
     * after a power failure.
     */
   intvl_random(&next_bcast, EPOCH+MIN_WAITTIME, EPOCH+SUPPLY_INTERVAL);
   age_timer.tv_usec = orig_age_usec = next_bcast.tv_usec;  /* LVL7_MOD */
   age_timer.tv_sec = EPOCH+MIN_WAITTIME;
   rdisc_timer = next_bcast;
   ifinit_timer.tv_usec = next_bcast.tv_usec;

   /* LVL7_MOD start */
   /* Initialize the RFC 1724 RIPv2 global MIB counters */
   memset(&rip_glob_mibstats, 0, sizeof(rip_glob_mibstats));
   memset(nbr_data, 0, sizeof(nbr_data));
   memset(tmp_data, 0, sizeof(tmp_data));
   /* LVL7_MOD end */

    /* Collect an initial view of the world by checking the interface
     * configuration and the kludge file.
     */
    /* gwkludge(); */                     /* LVL7_MOD */
    ifinit();

    /* Ask for routes */
    rip_query();
    rdisc_sol();

   for (;;)
   {

     /* hook to give RIP mapping layer control over task execution */
     ripMapVendTaskControl();
     /* refresh current time */
     ripUpdateTimebase(&wtime);

     /* deal with signals that should affect tracing */
     set_tracelevel();

    if (stopint != 0) {
          rip_bcast(0);
          rdisc_adv();
          trace_off("exiting with signal %d", stopint);
          exit(stopint | 128);
      }

      /* look for new or dead interfaces */
      timevalsub(&wtime, &ifinit_timer, &now);

      if (wtime.tv_sec <= 0 || need_ifinit_flag != 0) {
          wtime.tv_sec = 0;
          last_ifinit = now;
          ifinit_timer.tv_sec = now.tv_sec + (supplier
                      ? CHECK_ACT_INTERVAL
                      : CHECK_QUIET_INTERVAL);
          RIP_DEBUG_TRACE(RIP_TRACE_TIMER,"Time to check for interface status.");
          ifinit();

          if (need_ifinit_flag != 0)
          {
            ASYNC_RESPONSE_t  asyncResponse;

            need_ifinit_flag = 0;

            /* With RIP, we can only determine that the interface request has
               completed.  We cannot determine any more information than that */

            memset( &asyncResponse, 0, sizeof(ASYNC_RESPONSE_t) );
            asyncResponse.rc       = L7_SUCCESS;
            asyncResponse.reason   = 0;

            asyncEventSignalSend( &(pRipInfo->ripVendIfCompletionSignal), &asyncResponse);
          }

          rip_query();
          continue;
        }

        /* Check the kernel table occassionally for mysteriously
         * evaporated routes
         */
        timevalsub(&t2, &flush_kern_timer, &now);
        if (t2.tv_sec <= 0) {
            flush_kern();
            flush_kern_timer.tv_sec = (now.tv_sec
                           + CHECK_QUIET_INTERVAL);
            continue;
        }
        if (timercmp(&t2, &wtime, <))
            wtime = t2;

        /* If it is time, then broadcast our routes.
         */
        if (supplier || advertise_mhome) {
          timevalsub(&t2, &next_bcast, &now);
          if (t2.tv_sec <= 0) {
            /* Synchronize the aging and broadcast
             * timers to minimize awakenings
             */
            RIP_DEBUG_TRACE(RIP_TRACE_TIMER,"Time to age and send regular updates.");
            age(0);

            rip_bcast(0);

            /* It is desirable to send routing updates
             * regularly.  So schedule the next update
             * 30 seconds after the previous one was
             * scheduled, instead of 30 seconds after
             * the previous update was finished.
             * Even if we just started after discovering
             * a 2nd interface or were otherwise delayed,
             * pick a 30-second aniversary of the
             * original broadcast time.
             */
            n = 1 + (0-t2.tv_sec)/SUPPLY_INTERVAL;
            next_bcast.tv_sec += n*SUPPLY_INTERVAL;

            continue;
          }

          if (timercmp(&t2, &wtime, <))
              wtime = t2;
        }

        /* If we need a flash update, either do it now or
         * set the delay to end when it is time.
         *
         * If we are within MIN_WAITTIME seconds of a full update,
         * do not bother.
         */
        if (need_flash
            && supplier
            && no_flash.tv_sec+MIN_WAITTIME < next_bcast.tv_sec) {
            /* accurate to the millisecond */
            if (!timercmp(&no_flash, &now, >))
            {
              RIP_DEBUG_TRACE(RIP_TRACE_TIMER,"Flash update timer popped.");
              rip_bcast(1);
            }
            timevalsub(&t2, &no_flash, &now);
            if (timercmp(&t2, &wtime, <))
                wtime = t2;
        }

        /* trigger the main aging timer.
         */
        timevalsub(&t2, &age_timer, &now);
        if (t2.tv_sec <= 0) {
          RIP_DEBUG_TRACE(RIP_TRACE_TIMER,"Main aging timer popped.");
          age(0);
          continue;
        }
        if (timercmp(&t2, &wtime, <))
            wtime = t2;

        /* update the kernel routing table
         */
        timevalsub(&t2, &need_kern, &now);
        if (t2.tv_sec <= 0) {
          age(0);
          continue;
        }
        if (timercmp(&t2, &wtime, <))
          wtime = t2;

        /* take care of router discovery,
         * but do it in the correct the millisecond
         */
        if (!timercmp(&rdisc_timer, &now, >)) {
          rdisc_age(0);
          continue;
        }
        timevalsub(&t2, &rdisc_timer, &now);
        if (timercmp(&t2, &wtime, <))
          wtime = t2;


        /* wait for input or a timer to expire.
         */
        trace_flush();
        ibits = fdbits;

        wtime_temp = wtime;

        ripMapVendRnTreeCritExit();
        n = select(sock_max, &ibits, 0, 0, &wtime_temp);
        if (ripMapVendRnTreeCritEnter())
          LOG_ERROR(0);

        if (n <= 0) {
            if (n < 0 && errno != EINTR && errno != EAGAIN)
            {
               L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RIP_MAP_COMPONENT_ID,
                       "Select failed with error %d, reinitializing RIP sockets", errno);

               /* select() on rip socket failed due to an error other
                * than EINTR and EAGAIN. Close the sockets and re-initialize
                * the sockets again. */

               /* Close sockets */
               if(rip_sock >= 0)
               {
                  close(rip_sock);
                  rip_sock = -1;
               }

               /* Re-open sockets. That will inturn fix_select(). */
               for (ifp = ifnet; ifp != 0; ifp = ifp->int_next)
               {
                  if (ifp->int_rip_sock >= 0)
                  {
                     close(ifp->int_rip_sock);
                     ifp->int_rip_sock = -1;
                     rip_on(ifp);
                  }
               }
            }
            continue;
        }

        /* refresh current time before processing received messages */
        ripUpdateTimebase(&wtime);

        if (FD_ISSET(intfPipe, &ibits))
        {
          osapiPipeRead(intfPipe, intfPipeBuf, intfPipeMsgLen);
          ripProcessPipeMsg();
          n--;
        }


        if (rdisc_sock >= 0 && FD_ISSET(rdisc_sock, &ibits))
        {
          read_d();
          n--;
        }

        if((rip_sock >= 0) &&
           (FD_ISSET(rip_sock, &ibits)))
        {
            RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, "%s: got data from rip_sock\n", __FUNCTION__);
            read_rip(rip_sock, 0);
            n--;
        }

        for (ifp = ifnet; n > 0 && 0 != ifp; ifp = ifp->int_next)
        {
          if (ifp->int_rip_sock >= 0
              && FD_ISSET(ifp->int_rip_sock, &ibits))
          {
            RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, "%s: got data from ifaddr %s\n",
                            __FUNCTION__, naddr_ntoa(ifp->int_addr));
            read_rip(ifp->int_rip_sock, ifp);
            n--;
          }
        }
    }
}

void
fix_select(void)
{
  struct interface *ifp;


  FD_ZERO(&fdbits);
  sock_max = 0;

  /* rip receive socket, receives incoming multicast and unicast data */
  if(rip_sock >= 0)
  {
    FD_SET(rip_sock, &fdbits);
    if (sock_max <= rip_sock)
      sock_max = rip_sock+1;
  }

  /* interface sockets. they're setup for read but dont receive anything
   * as the kernel sends incoming packets to rip_sock
   */
  for (ifp = ifnet; 0 != ifp; ifp = ifp->int_next)
  {
    if (ifp->int_rip_sock >= 0)
    {
      FD_SET(ifp->int_rip_sock, &fdbits);
      if (sock_max <= ifp->int_rip_sock)
          sock_max = ifp->int_rip_sock+1;
    }
  }

  /* router discovery socket */
  if (rdisc_sock >= 0)
  {
    FD_SET(rdisc_sock, &fdbits);
    if (sock_max <= rdisc_sock)
        sock_max = rdisc_sock+1;
  }

  /* pipe for communication with the mapping layer */
  if (intfPipe >= 0)
  {
    FD_SET(intfPipe, &fdbits);
    if (sock_max <= intfPipe)
    {
      sock_max = intfPipe + 1;
    }
  }
}


void
fix_sock(int sock,
     const char *name)
{
    int on;
#define MIN_SOCKBUF (4*1024)
    static int rbuf = 0;

    osapiSocketNonBlockingModeSet(sock,L7_TRUE);

    on = 1;
    if (osapiSetsockopt(sock, L7_SOL_SOCKET,L7_SO_BROADCAST, (caddr_t)&on,sizeof(on)) != L7_SUCCESS )
        LOGERR("RIP: Socket Setting SO_BROADCAST Error\n");
    on = 1;
    if (osapiSetsockopt(sock, IPPROTO_IP, L7_IP_PKTINFO, (caddr_t)&on,sizeof(on)) != L7_SUCCESS )
        LOGERR("RIP Socket Setting IP_PKTINFO Error\n");
#ifdef USE_PASSIFNAME
    on = 1;
    if (osapiSetsockopt(sock, L7_SOL_SOCKET, L7_SO_PASSIFNAME, (caddr_t)&on,sizeof(on)) != L7_SUCCESS )
        LOGERR("RIP: Socket Setting SO_PASSIFNAME Error\n");
#endif

    if (rbuf >= MIN_SOCKBUF) {
        if (osapiSetsockopt(sock, L7_SOL_SOCKET, L7_SO_RCVBUF,
                   (caddr_t)&rbuf, sizeof(rbuf)) != L7_SUCCESS )
            LOGERR("RIP: Socket Setting SO_RCVBUF Error\n");
    } else {
        for (rbuf = 60*1024; ; rbuf -= 4096) {
            if (osapiSetsockopt(sock, L7_SOL_SOCKET, L7_SO_RCVBUF,
                       (caddr_t)&rbuf, sizeof(rbuf)) == L7_SUCCESS) {
                RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"SO_RCVBUF set to %d for socket %s.", rbuf, name);
                break;
            }
            if (rbuf < MIN_SOCKBUF) {
                LOGERR("RIP: Socket Setting SO_RCVBUF Error\n");
                break;
            }
        }
    }
}

/* get a rip socket
 */
static int              /* <0 or file descriptor */
get_rip_sock(naddr addr,
         int serious)       /* 1=failure to bind is serious */
{
    L7_sockaddr_in_t baddr;
    int s;
    int prm = 1;
    unsigned char ttl;

    if (osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0,&s) != L7_SUCCESS)
    {
      LOGERR("RIP: Socket Creation Error\n");
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, "%s: Socket Creation Error for ifaddr %s: %s",
            __FUNCTION__, naddr_ntoa(addr), strerror(errno));
    }

    /* allow address reuse */
    if(osapiSetsockopt(s, L7_SOL_SOCKET, L7_SO_REUSEADDR, (unsigned char*)&prm, sizeof(prm)) != L7_SUCCESS)
    {
      LOGERR("RIP: Error setting sock option SO_REUSEADDR\n");
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, "%s: SO_REUSEADDR failed for ifaddr %s: %s",
            __FUNCTION__, naddr_ntoa(addr), strerror(errno));
    }

    memset(&baddr, 0, sizeof(baddr));
#ifdef _HAVE_SIN_LEN
    baddr.sin_len = sizeof(baddr);
#endif
    baddr.sin_family = L7_AF_INET;
    baddr.sin_port = htons(RIP_PORT);
    baddr.sin_addr.s_addr = htonl(addr);
    if (osapiSocketBind(s, (L7_sockaddr_t *)&baddr, sizeof(baddr)) != L7_SUCCESS)
    {
      LOGERR("RIP: Socket Bind Error\n");
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, "%s: Socket Bind Error for ifaddr %s: %s",
            __FUNCTION__, naddr_ntoa(addr), strerror(errno));
      return -1;
    }

    fix_sock(s,"get_rip_sock");

    ttl = 1;
    if (osapiSetsockopt(s, IPPROTO_IP, L7_IP_MULTICAST_TTL,
               (caddr_t)&ttl, sizeof(ttl)) != L7_SUCCESS )
    {
      LOGERR("RIP: Error setting sock option IP_MULTICAST_TTL for rip_sock\n");
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, "%s: IP_MULTICAST_TTL failed for ifaddr %s: %s",
            __FUNCTION__, naddr_ntoa(addr), strerror(errno));
    }

    return s;
}


void
rip_off(void)
{
  struct interface *ifp;

  if ( (rip_sock >= 0) && !mhome)
  {
    RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, "Turn off RIP\n");

    /* close all interface sockets
     */
    for (ifp = ifnet; ifp != 0; ifp = ifp->int_next)
    {
      if (ifp->int_rip_sock >= 0)
      {
        close(ifp->int_rip_sock);
        ifp->int_rip_sock = -1;
      }
    }

    /* close rip_sock */
    if(rip_sock >= 0)
    {
      close(rip_sock);
      rip_sock = -1;
    }

    fix_select();

    age(0);
  }
}


/* Turn on RIP multicast input and output via an interface
 *
 * RX:
 * Since rip_sock is used to receive packets, it joins the multicast group on
 * every interface.
 *
 * TX:
 * interface sockets (ifp->int_rip_sock) are used to send outgoing multicast
 * packets. IP_MULTICAST_IF on this socket directs outgoing multicast output
 * to the interface associated with the socket.
 */
static void
rip_mcast_on(struct interface *ifp)
{
  struct L7_ip_mreq_s m;
  unsigned int tgt_mcast;

  if (!IS_RIP_IN_OFF(ifp->int_state)
      && (ifp->int_if_flags & IFF_MULTICAST)
      && (ifp->int_rip_sock >= 0))
  {

    /* set outgoing interface for mcast */
    tgt_mcast =osapiHtonl(ifp->int_addr);
    if (L7_SUCCESS !=  osapiSetsockopt(ifp->int_rip_sock,
                       IPPROTO_IP, L7_IP_MULTICAST_IF,
                       (caddr_t)&tgt_mcast,
                       sizeof(tgt_mcast)))
    {
      LOGERR("Errror setting sock option IP_MULTICAST_IF for rip_sock.\n");
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, "%s: setsockopt %d IP_MULTICAST_IF Error on if %s: %s",
                      __FUNCTION__, ifp->int_rip_sock, naddr_ntoa(ifp->int_addr), strerror(errno));
    }

    /* rip_sock joins RIP mcast group for primary interfaces */
    if(!(ifp->int_state & IS_ALIAS))
    {
      memset(&m, 0, sizeof(struct L7_ip_mreq_s));
      m.imr_multiaddr.s_addr = osapiHtonl(INADDR_RIP_GROUP);
      m.imr_interface.s_addr = osapiHtonl((ifp->int_if_flags & IFF_POINTOPOINT)
                                          ? ifp->int_dstaddr
                                          : ifp->int_addr);

      if (osapiSetsockopt(rip_sock,IPPROTO_IP, L7_IP_ADD_MEMBERSHIP,
                     (caddr_t)&m, sizeof(m)) != L7_SUCCESS)
      {
        LOGERR("RIP: setsockopt IP_ADD_MEMBERSHIP RIP Error\n");
        RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, "%s: setsockopt IP_ADD_MEMBERSHIP Error on if %s: %s",
                        __FUNCTION__, naddr_ntoa(ifp->int_addr), strerror(errno));
      }
      else
      {
        RIP_DEBUG_TRACE(RIP_TRACE_NORMAL, "%s: setsockopt IP_ADD_MEMBERSHIP done for if %s",
                        __FUNCTION__, naddr_ntoa(ifp->int_addr));
      }
    }
  }
}
/* Prepare socket used for RIP.
 */
void
rip_on(struct interface *ifp)
{
  /* must have an interface */
  if(!ifp)
    return;

  /* is rip being turned on for the first time? */
  if (rip_sock < 0)
  {
    /* create the global receive socket */
    rip_sock = get_rip_sock(INADDR_ANY, 1);

    /* Do not advertise anything until we have heard something
     */
    if (next_bcast.tv_sec < now.tv_sec+MIN_WAITTIME)
        next_bcast.tv_sec = now.tv_sec+MIN_WAITTIME;

    ifinit_timer.tv_sec = now.tv_sec;
  }

  /* turn on RIP on the interface */
  if(ifp->int_rip_sock < 0)
  {
    RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Turning on rip on intf %s %s.",
                    ifp->int_name, naddr_ntoa(ifp->int_addr));

    ifp->int_rip_sock = get_rip_sock(ifp->int_addr, 1);
  }

  ifp->int_query_time = NEVER;

  /* turn on multicast */
  rip_mcast_on(ifp);

  /* fix the FDSET */
  fix_select();
}


void *
rtmalloc(size_t size,
     const char *msg)
{
    void *p = malloc(size);
    if (p == 0)
        logbad(1,"RIP rtmalloc: malloc(%lu) failed in %s\n", (u_long)size, msg);
    return p;
}


/* get a random instant in an interval
 */
void
intvl_random(struct timeval *tp,    /* put value here */
         u_long lo,         /* value is after this second */
         u_long hi)         /* and before this */
{
    tp->tv_sec = (time_t)(hi == lo
                  ? lo
                  : (lo + random() % ((hi - lo))));
    tp->tv_usec = random() % 1000000;
}


void
ripTimevaladd(struct timeval *t1,
       struct timeval *t2)
{

    t1->tv_sec += t2->tv_sec;
    if ((t1->tv_usec += t2->tv_usec) >= 1000000) {
        t1->tv_sec++;
        t1->tv_usec -= 1000000;
    }
}


/* t1 = t2 - t3
 */
static void
timevalsub(struct timeval *t1,
       struct timeval *t2,
       struct timeval *t3)
{
    t1->tv_sec = t2->tv_sec - t3->tv_sec;
    if ((t1->tv_usec = t2->tv_usec - t3->tv_usec) < 0) {
        t1->tv_sec--;
        t1->tv_usec += 1000000;
    }
}


void
ripgettimeofday( struct timeval *tv , struct timezone *not_used )
{
    L7_clocktime ct;
    osapiClockTimeRaw ( &ct );
    tv->tv_sec = ct.seconds;
    tv->tv_usec = ct.nanoseconds / 1000;

}


#ifdef LVL7_ORIGINAL_VENDOR_CODE    /* LVL7_MOD start */
#else                               /* LVL7_MOD */
static void
ripUpdateTimebase( struct timeval *wt )
{
  time_t dt;
  struct timeval t2;
  static u_int fudge_peak = 0;  /* track for performance analysis */
  prev_clk = clk;
  ripgettimeofday(&clk, 0);
  if (prev_clk.tv_sec == clk.tv_sec
      && prev_clk.tv_usec == clk.tv_usec+usec_fudge) {
    /* Much of `routed` depends on time always advancing.
     * On systems that do not guarantee that gettimeofday()
     * produces unique timestamps even if called within
     * a single tick, use trickery like that in classic
     * BSD kernels.
     */
    clk.tv_usec += ++usec_fudge;
  } else {
    if (usec_fudge > fudge_peak)
      fudge_peak = usec_fudge;
    usec_fudge = 0;
    timevalsub(&t2, &clk, &prev_clk);
    if (t2.tv_sec < 0
        || t2.tv_sec > wt->tv_sec + 5) {
      /* Deal with time changes before other
       * housekeeping to keep everything straight.
       */
      dt = t2.tv_sec;
      if (dt > 0)
        dt -= wt->tv_sec;
      trace_act("time changed by %d sec", (int)dt);
      RIP_DEBUG_TRACE(RIP_TRACE_TIMER,"Time changed by %d sec.", (int)dt);
      epoch.tv_sec += dt;
    }
  }
  timevalsub(&now, &clk, &epoch);
  now_stale = now.tv_sec - STALE_TIME;
  now_expire = now.tv_sec - EXPIRE_TIME;
  now_garbage = now.tv_sec - GARBAGE_TIME;
}

void ripProcessPipeMsg()
{
  ripPipeMsg_t *ripVendorPipeMsg;

  ripVendorPipeMsg = (ripPipeMsg_t *)intfPipeBuf;
  switch (ripVendorPipeMsg->msgId)
  {
    case L7_RIP_INTF_STATE_CHANGE:
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Received interface state changed message.");
      need_ifinit_flag = 1;
      break;

    case L7_RIP_SPLITHORIZON_MODE_CHANGE:
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Received split horizon mode change message.");
      ripSplitHorizon = ripVendorPipeMsg->arg1;
      break;

    case L7_RIP_AUTOSUMMARY_MODE_CHANGE:
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Received auto summary mode change message.");
      ripAutoSummary = ripVendorPipeMsg->arg1;
      break;

    case L7_RIP_HOSTROUTESACCEPT_MODE_CHANGE:
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Received host routes accept mode change message.");
      ripHostRoutesAccept = ripVendorPipeMsg->arg1;
      break;

    case L7_RIP_DEFAULT_ROUTE_ADV_CHANGE:
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Received default route adv mode change message.");
      ripDefaultRouteMetricSet(ripVendorPipeMsg->arg1, ripVendorPipeMsg->arg2);
      break;

    case L7_RIP_DEFAULT_METRIC_CHANGE:
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Received default metric change message.");
      ripMapVendRipDefaultMetricSet(ripVendorPipeMsg->arg1);
      break;

    case L7_RIP_ROUTE_REDISTRIBUTE_METRIC_CHANGE:
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Received Route Redistribute Metric change message.");
      ripMapVendRipRouteRedistMetricSet(ripVendorPipeMsg->rip_proto, ripVendorPipeMsg->arg1);
      break;

    case L7_RIP_ROUTE_REDISTRIBUTE_ROUTE_ADD:
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Received Redistribute Route Add message.");
      ripRedistributeRouteAdd(ripVendorPipeMsg->arg1,
                              ripVendorPipeMsg->arg2,
                              ripVendorPipeMsg->arg3,
                              ripVendorPipeMsg->arg4,
                              ripVendorPipeMsg->rip_proto);
      break;

    case L7_RIP_ROUTE_REDISTRIBUTE_ROUTE_DELETE:
      RIP_DEBUG_TRACE(RIP_TRACE_NORMAL,"Received Redistribute Route Delete message.");
      ripRedistributeRouteDelete(ripVendorPipeMsg->arg1,
                                 ripVendorPipeMsg->arg2,
                                 ripVendorPipeMsg->arg3,
                                 ripVendorPipeMsg->rip_proto);
      break;

    default:
      break;
  }

}

void ripDefaultRouteMetricSet(unsigned int mode, unsigned int metric)
{
  struct interface *ifp;

  switch (mode)
  {
    case DEF_ROUTE_ADV_OFF:
      metric = 0;
      break;

    case DEF_ROUTE_ADV_ON:
      break;

    default:
      return;
  }

  /* Iterate thru all ifps and set the default route metric */
  for (ifp = ifnet; ifp != 0; ifp = ifp->int_next)
  {
    ifp->int_d_metric = metric;
  }
  return;
}

/*********************************************************************
* @purpose  Add a redistributed route to the RIP route table.
*
* @param    dst      @b{(input)}  destination address
* @param    mask     @b{(input)}  network mask of destination prefix
* @param    gw       @b{(input)}  next hop IP address
* @param    metric   @b{(input)}  metric to advertise for this prefix
* @param    srcProto @b{(input)}  protocol that added this prefix to RTO
*
* @returns  void
*
* @notes    The RIP route table is a radix tree. Each node in the tree is a set
*           of up to NUM_SPARES routes to that exact prefix. Each individual
*           route is called a "spare." The first route (index 0) is the best
*           route. The other spares are not ordered. RIP uses the
*           route metric to decide whether a redistributed route is better or
*           worse than native RIP routes. Redistributed routes are neither
*           considered better nor worse than native RIP routes simply because
*           they are external.
*
*           When we add a redistributed route, we check if there is already a
*           set of routes to this destination. If not, the redistributed route
*           is added to the radix tree. If a set of routes already exists, then
*           we check if one of the existing routes is a redistributed route. If
*           so, the new route replaces the old. There can only be one
*           redistributed route to a given prefix, since RTO selects a single
*           best route to each destination and RIP ignores all but the first
*           next hop from each best route. If none of the existing routes are
*           redistributed routes, then we try to add the redistributed route
*           as a new spare. If there are already NUM_SPARES spares, then we
*           check if the metric of the redistributed route is better than that
*           of the worst spare. If so, we replace the worst spare and reorder
*           the spares. Otherwise we ignore the route (which probably means it
*           is forgotten forever and won't be used if the better routes go away).
*
*           The route flag RS_REDIST indicates that the set of routes to this
*           destination includes a redistributed route. The spare that is the
*           redistributed route has rts_redist_proto set to a non-zero value
*           from the rip_redist_proto enumeration.
*
* @end
*********************************************************************/
void ripRedistributeRouteAdd(uint dst, uint mask, uint gw, uint metric,
                             rip_redist_proto srcProto)
{
  int i;
  struct rt_entry *rtList;             /* Existing set of routes to this prefix */
  struct rt_spare *rts;                /* one of the routes to this prefix */
  struct rt_spare *worstSpare = NULL;  /* the existing route with highest hop count */
  struct rt_spare newRoute;            /* newly redistributed route */
  L7_char8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_char8 destMaskStr[OSAPI_INET_NTOA_BUF_SIZE];

  /* configure new route */
  newRoute.rts_gate = gw;
  newRoute.rts_router = gw;
  newRoute.rts_metric = metric;
  newRoute.rts_tag = 0;
  newRoute.rts_time = now.tv_sec;
  newRoute.rts_ifp = 0;
  newRoute.rts_de_ag = 0;
  newRoute.rts_redist_proto = srcProto; /* Redistributed route protocol */

  /* Find route to exact prefix in RIP route table */
  rtList = rtget(dst, mask);
  if (rtList == NULL)
  {
    /* The prefix doesn't exist in the rip protocol route table. Add it. */
    rtadd(dst, mask, RS_REDIST, &newRoute);
    return;
  }

  /* We have one or more routes to this prefix */
  if ((rtList->rt_state & RS_REDIST) != 0)
  {
    /* One of the routes to this destination is a redistributed route.
     * Replace it with the new route.  */
    rts = rtList->rt_spares;
    for (i = 0; i < NUM_SPARES; i++)
    {
      if ((rtList->rt_spares[i].rts_redist_proto > RIP_RT_REDIST_FIRST) &&
          (rtList->rt_spares[i].rts_redist_proto < RIP_RT_REDIST_LAST))
      {
        /* This route is a redist route. Replace it. */
        *rts = newRoute;            /* structure copy */
        rtswitch(rtList, 0);        /* check for a new best route */
        set_need_flash();
        return;
      }
      rts++;    /* next spare */
    }
    /* If we get here, rt_state seems to have been out of sync with the actual
     * routes to this destination. Thought we had a redist route, but didn't find
     * one. */
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RIP_MAP_COMPONENT_ID,
           "Failed to find previous redistributed route.");
    /* blunder on */
  }

  /* Route list does not include a redistributed route. Add redist route
   * as a new spare. */

  /* Find index of first empty spare. */
  rts = rtList->rt_spares;
  for (i = 0; i < NUM_SPARES; i++)
  {
    if ((rts->rts_gate == 0))
    {
      /* Add new spare */
      rtList->rt_state |= RS_REDIST;
      *rts = newRoute;
      rtswitch(rtList, rts);
      set_need_flash();
      return;
    }
    /* Track worst spare (spare with largest route metric) */
    if ((worstSpare == NULL) || BETTER_LINK(rtList, worstSpare, rts))
    {
      worstSpare = rts;
    }
    rts++;
  }

  /* No empty spares. See if redist route is better than worst existing spare */
  if (newRoute.rts_metric < worstSpare->rts_metric)
  {
    /* Replace worst spare with redist route */
    rtList->rt_state |= RS_REDIST;
    *worstSpare = newRoute;
    rtswitch(rtList, worstSpare);
    set_need_flash();
  }
  /* simply ignore redist route. Would be nice to clear the routeAdded flag
   * on the mapping side */
  osapiInetNtoa(dst, destAddrStr);
  osapiInetNtoa(mask, destMaskStr);
  L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RIP_MAP_COMPONENT_ID,
            "RIP ignoring redistributed route to %s/%s. All spares taken by better routes.",
            destAddrStr, destMaskStr);
  return;
}

/*********************************************************************
* @purpose  Stop redistributing a given route.
*
* @param    dst      @b{(input)}  destination address
* @param    mask     @b{(input)}  network mask of destination prefix
* @param    gw       @b{(input)}  next hop IP address
* @param    metric   @b{(input)}  metric to advertise for this prefix
* @param    srcProto @b{(input)}  protocol that added this prefix to RTO
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ripRedistributeRouteDelete(uint dst, uint mask, uint gw,
                                rip_redist_proto srcProto)
{
  int i;
  struct rt_entry *rtList;            /* list of routes to this prefix */
  L7_char8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_char8 destMaskStr[OSAPI_INET_NTOA_BUF_SIZE];


  /* Get list of routes to this exact prefix */
  rtList = rtget(dst, mask);
  if (rtList == NULL)
  {
    /* No routes to this destination */
    osapiInetNtoa(dst, destAddrStr);
    osapiInetNtoa(mask, destMaskStr);
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RIP_MAP_COMPONENT_ID,
            "No route to %s/%s.",
            destAddrStr, destMaskStr);
    return;
  }

  if ((rtList->rt_state & RS_REDIST) == 0)
  {
    /* Existing list of routes does not include a redsitributed route. This
     * could be ok, because ripRedistributeRouteAdd() ignores redist route
     * if no empty spares. But could indicate that RIP MAP and protocol
     * have gotten out of sync because of a coding error. */
    osapiInetNtoa(dst, destAddrStr);
    osapiInetNtoa(mask, destMaskStr);
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RIP_MAP_COMPONENT_ID,
            "List of routes to %s/%s does not include a redistributed route.",
            destAddrStr, destMaskStr);
    return;
  }

  for (i = 0; i < NUM_SPARES; i++)
  {
    /* Since only one spare can be a redistributed route, find it and remove */
    if ((rtList->rt_spares[i].rts_redist_proto > RIP_RT_REDIST_FIRST) &&
        (rtList->rt_spares[i].rts_redist_proto < RIP_RT_REDIST_LAST))
    {
      if (i == 0)
      {
        /* deleting best route. Poison it. */
        rtList->rt_spares[i].rts_metric = HOPCNT_INFINITY;
        rtchange(rtList, rtList->rt_state, rtList->rt_spares, 0);
      }
      else
      {
        /* deleting non-best route. Just obliterate it. */
        rts_delete(rtList, &rtList->rt_spares[i]);
        rtList->rt_state &= ~(RS_REDIST);
      }
      return;
    }
  }

  osapiInetNtoa(dst, destAddrStr);
  osapiInetNtoa(mask, destMaskStr);
  L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_RIP_MAP_COMPONENT_ID,
          "Failed to find redistributed route to %s/%s.",
          destAddrStr, destMaskStr);
  return;
}
#endif                              /* LVL7_MOD end */


#ifdef LVL7_ORIGINAL_VENDOR_CODE    /* LVL7_MOD start */
void
setRipVersion( char *version )
{
  const char *cp;
  cp = parse_parms(version, 0);
  if (cp != 0)
    printf("Invalid Parameter for RIP Version\n");
}
#endif                                                      /* LVL7_MOD end */


/* convert IP address to a string, but not into a single buffer
 */
char *
naddr_ntoa(naddr a)
{
#define NUM_BUFS 4
    static int bufno;
    static struct {
        char    str[16];        /* xxx.xxx.xxx.xxx\0 */
    } bufs[NUM_BUFS];
    char *s;
    struct in_addr addr;
  char addrStr[INET_NTOA_BUFSIZE];  /* LVL7_MOD */

/*#ifndef L7_ORIGINAL_VENDOR_CODE
    addr.s_addr = osapiHtonl(a);
    ripMapVendInetNtoa(osapiNtohl(addr.s_addr), addrStr, INET_NTOA_BUFSIZE);     * LVL7_MOD *
#else*//* L7_ORIGINAL_VENDOR_CODE */
    addr.s_addr = (a);
    ripMapVendInetNtoa(addr.s_addr, addrStr, INET_NTOA_BUFSIZE);     /* LVL7_MOD */
/*#endif* * L7_ORIGINAL_VENDOR_CODE */
    s = strcpy(bufs[bufno].str, addrStr);       /* LVL7_MOD */
    bufno = (bufno+1) % NUM_BUFS;
    return s;
#undef NUM_BUFS
}


/* LVL7_MOD start */

#include <stdarg.h>

const char *saddr_ntoa(struct sockaddr *sa)
{
#ifndef L7_ORIGINAL_VENDOR_CODE
  return(naddr_ntoa(osapiNtohl(S_ADDR(sa))));
#else /* L7_ORIGINAL_VENDOR_CODE */
  return(naddr_ntoa(S_ADDR(sa)));
#endif /* L7_ORIGINAL_VENDOR_CODE */
}

/* create formatted IP address string */
char *addrname(naddr ip, naddr mask, int order)
{
  naddr netnum;

  netnum = (ip & mask);
  /* NOTE:  Not sure how third parm is used.  Net byte order flag?? */
  return(naddr_ntoa(order ? osapiHtonl(netnum) : netnum));
}

/* create a string indicating the route and its gateway */
char *rtname(naddr dst, naddr mask, naddr gate)
{
  static char buf[80];

#ifndef L7_ORIGINAL_VENDOR_CODE
  sprintf(buf, "%s via %s", addrname(dst, osapiHtonl(mask), 1), naddr_ntoa(gate));
#else /* L7_ORIGINAL_VENDOR_CODE */
  sprintf(buf, "%s via %s", addrname(dst, htonl(mask), 1), naddr_ntoa(gate));
#endif /* L7_ORIGINAL_VENDOR_CODE */
  return buf;
}

#if 0  /* NOTE: Having problems with variable arg list on certain calls. */
void msglim(struct msg_limit *lim, naddr addr, const char *p, ...)
{
    va_list args;
    va_start(args, p);
  LOG_MSG(p, args);
  va_end(args);
}
#endif

/* provide a debug mechanism to change the value of the auth_ok flag */
void ripSkipAuthCheck(int chk)
{
  /* a value of 0 means RIPv2 packets should be checked for proper authorization
   * per RFC 1723 (default); a non-zero value disables authorization checking
   */
  auth_ok = (chk == 0) ? 0 : 1;
}

/* provide a debug mechanism to suppress host routes from output messages */
void ripNoHostOut(int chk)
{
  /* a nonzero value means host routes are not to be advertised
   */
  ridhosts = (chk == 0) ? 0 : 1;
}

/* LVL7_MOD end */

