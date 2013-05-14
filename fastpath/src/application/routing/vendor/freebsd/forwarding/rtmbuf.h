
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  rtmbuf.h
*
* @purpose   Header File for MBUF related functions
*
* @component 
*
* @comments  none
*
* @create    
*                       
* @author    
*
* @end
*             
**********************************************************************/
/*
 * Copyright (c) 1982, 1986, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
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
 *	@(#)mbuf.h	8.5 (Berkeley) 2/19/95
 * $FreeBSD: src/sys/sys/mbuf.h,v 1.44.2.8 2001/02/04 14:49:59 dwmalone Exp $
 */

#ifndef _SYS_MBUF_H_
#define	_SYS_MBUF_H_
/* lvl7_@ip_forward start*/
#include <string.h>
#include <stdio.h>  /* for u_xx types */
#include <l7_socket.h>
#include <sys/types.h>
#include <commdefs.h>

#define ETHER_HEADERLEN   14
#define EARPINPROGRESS 200

#if defined(_L7_OS_LINUX_)
#define M_BCAST 0x10
#define M_MCAST 0x20

#define M_EXT 0x01
#define M_PKTHDR 0x02
#define M_EOR 0x04

#define NTOHS(x) (x) = osapiNtohs(x)
#define HTONS(x) (x) = osapiHtons(x)
#endif

/* lvl7_@ip_forward end*/
/*
 * Mbufs are of a single size, MSIZE (machine/param.h), which
 * includes overhead.  An mbuf may add a single "mbuf cluster" of size
 * MCLBYTES (also in machine/param.h), which has no additional overhead
 * and is used instead of the internal data area; this is done when
 * at least MINCLSIZE of data must be stored.
 */
 /* lvl7_@ip_forward start*/

#define RTMSIZE 256

#if defined(_L7_OS_LINUX_)
#define	MLEN		(RTMSIZE - sizeof(struct rtm_hdr))	/* normal data len */
/* lvl7_@ip_forward end*/
#define	MHLEN		(MLEN - sizeof(struct rtpkthdr))	/* data len w/pkthdr */
#endif

#define	MINCLSIZE	(MHLEN + 1)	/* smallest amount to put in cluster */

 /* lvl7_@ip_forward start*/
#define	rtmtod(m, t)	((t)((m)->rtm_data))
 /* lvl7_@ip_forward end*/

#define	dtom(x)		((struct mbuf *)((intptr_t)(x) & ~(MSIZE-1)))
#define	mtocl(x)	(((uintptr_t)(x) - (uintptr_t)mbutl) >> MCLSHIFT)
#define	cltom(x)	((caddr_t)((uintptr_t)mbutl + \
			    ((uintptr_t)(x) << MCLSHIFT)))


/*
 * Structure defining statistics and other data kept regarding a network
 * interface.
 */
struct	rtm_if_data {
	/* generic interface information */
	u_char	ifi_type;		/* ethernet, tokenring, etc. */
	u_char	ifi_addrlen;		/* media address length */
	u_char	ifi_hdrlen;		/* media header length */
	u_long	ifi_mtu;		/* maximum transmission unit */
	u_long	ifi_metric;		/* routing metric (external only) */
	u_long	ifi_baudrate;		/* linespeed */
	/* volatile statistics */
	u_long	ifi_ipackets;		/* packets received on interface */
	u_long	ifi_ierrors;		/* input errors on interface */
	u_long	ifi_opackets;		/* packets sent on interface */
	u_long	ifi_oerrors;		/* output errors on interface */
	u_long	ifi_collisions;		/* collisions on csma interfaces */
	u_long	ifi_ibytes;		/* total number of octets received */
	u_long	ifi_obytes;		/* total number of octets sent */
	u_long	ifi_imcasts;		/* packets received via multicast */
	u_long	ifi_omcasts;		/* packets sent via multicast */
	u_long	ifi_iqdrops;		/* dropped on input, this interface */
	u_long	ifi_noproto;		/* destined for unsupported protocol */
	u_long  ifi_lastchange;		/* last updated */
};

/*
 * Structure defining a queue for a network interface.
 *
 * (Would like to call this struct ``if'', but C isn't PL/1.)
 */

struct rtm_ifnet {
	u_short	if_index;		/* numeric abbreviation for this if */ /* LVL7: This is internal interface number */
	short	if_flags;		/* up/down, broadcast, etc. */
	struct	rtm_if_data rtm_if_data;	/* statistics and other data about if */
};

#define	rtm_if_mtu		rtm_if_data.ifi_mtu

/* header at beginning of each mbuf: */
/* lvl7_@ip_forward start*/
struct rtm_hdr {
	struct	rtmbuf *mh_next;		/* next buffer in chain */
	struct	rtmbuf *mh_nextpkt;	/* next chain in queue/record */
	caddr_t	mh_data;		/* location of data */
	int	mh_len;			/* amount of data in this mbuf */
    short	mh_type;		/* type of data in this mbuf */
	short	mh_flags;		/* flags; see below */
    void *mh_bufhandle;
    
};
/* lvl7_@ip_forward end*/

/* record/packet header in first mbuf of chain; valid if M_PKTHDR set */
/* lvl7_@ip_forward start*/

struct rtpkthdr {
	struct rtm_ifnet *rcvif;		/* rcv interface */
 L7_uint32 rxPort; /* Physical port where packet was received */
	int	len;			/* total packet length */
	/* variables for ip and tcp reassembly */
	void	*header;		/* pointer to packet header */
	/* variables for hardware checksum */
	int	csum_flags;		/* flags regarding checksum */
	int	csum_data;  	/* data field used by csum routines */
	struct	rtmbuf *aux;		/* extra data buffer; ipsec/others */
};
/* lvl7_@ip_forward end*/

/* description of external storage mapped into mbuf, valid if M_EXT set */
/* lvl7_@ip_forward start*/
struct rtm_ext {
	caddr_t	ext_buf;		/* start of buffer */
	void	(*ext_free)		/* free routine if not the usual */
		(caddr_t, unsigned int);
	unsigned int	ext_size;		/* size of buffer, for ext_free */
	void	(*ext_ref)		/* add a reference to the ext object */
		(caddr_t, unsigned int);
};
/* lvl7_@ip_forward end*/

/* lvl7_@ip_forward start*/
struct rtmbuf {
	struct	rtm_hdr m_hdr;
	union {
		struct {
			struct	rtpkthdr MH_pkthdr;	/* M_PKTHDR set */
			union {
				struct	rtm_ext MH_ext;	/* M_EXT set */
				char	MH_databuf[MHLEN];
			} MH_dat;
		} MH;
		char	M_databuf[MLEN];    	/* !M_PKTHDR, !M_EXT */
    } M_dat;
    char m_dmac[L7_MAC_ADDR_LEN];  /* DMAC for ICMP to search for a VRRP IP addr */
} __attribute__ ((__packed__)); 
/* lvl7_@ip_forward end*/

 /* lvl7_@ip_forward start*/
#define	rtm_next		m_hdr.mh_next
#define	rtm_len		m_hdr.mh_len
#define	rtm_data		m_hdr.mh_data
#define	rtm_type		m_hdr.mh_type
#define	rtm_flags		m_hdr.mh_flags
#define	rtm_nextpkt	m_hdr.mh_nextpkt
#define	rtm_act		rtm_nextpkt
#define	rtm_pkthdr	M_dat.MH.MH_pkthdr
#define	rtm_ext		M_dat.MH.MH_dat.MH_ext
#define	rtm_pktdat	M_dat.MH.MH_dat.MH_databuf
#define	rtm_dat		M_dat.M_databuf
#define rtm_bufhandle m_hdr.mh_bufhandle
 /* lvl7_@ip_forward end*/
/* mbuf flags */

#define	M_PROTO1	0x0008	/* protocol-specific */
#define	M_PROTO2	0x0010	/* protocol-specific */
#define	M_PROTO3	0x0020	/* protocol-specific */
#define	M_PROTO4	0x0040	/* protocol-specific */
#define	M_PROTO5	0x0080	/* protocol-specific */

/* mbuf pkthdr flags, also in m_flags */
#define	M_FRAG		0x0400	/* packet is a fragment of a larger packet */
#define	M_FIRSTFRAG	0x0800	/* packet is first fragment */
#define	M_LASTFRAG	0x1000	/* packet is last fragment */

/* flags copied when copying m_pkthdr */

#if defined(_L7_OS_LINUX_)
/* flags indicating hw checksum support and sw checksum requirements */
#define CSUM_IP			0x0001		/* will csum IP */
#define CSUM_TCP		0x0002		/* will csum TCP */
#define CSUM_UDP		0x0004		/* will csum UDP */
#define CSUM_IP_FRAGS		0x0008		/* will csum IP fragments */
#define CSUM_FRAGMENT		0x0010		/* will do IP fragmentation */

#define CSUM_IP_CHECKED		0x0100		/* did csum IP */
#define CSUM_IP_VALID		0x0200		/*   ... the csum is valid */
#define CSUM_DATA_VALID		0x0400		/* csum_data field is valid */
#define CSUM_PSEUDO_HDR		0x0800		/* csum_data has pseudo hdr */

#define CSUM_DELAY_DATA		(CSUM_TCP | CSUM_UDP)
#define CSUM_DELAY_IP		(CSUM_IP)	/* XXX add ipv6 here too? */
#endif 

/* mbuf types */
#define	MT_FREE		0	/* should be on free list */
#define	MT_DATA		1	/* dynamic (data) allocation */
#define	MT_HEADER	2	/* packet header */
#if 0
#define	MT_SOCKET	3	/* socket structure */
#define	MT_PCB		4	/* protocol control block */
#define	MT_RTABLE	5	/* routing tables */
#define	MT_HTABLE	6	/* IMP host tables */
#define	MT_ATABLE	7	/* address resolution tables */
#endif
#define	MT_SONAME	8	/* socket name */
#if 0
#define	MT_SOOPTS	10	/* socket options */
#endif
#define	MT_FTABLE	11	/* fragment reassembly header */
#if 0
#define	MT_RIGHTS	12	/* access rights */
#define	MT_IFADDR	13	/* interface address */
#endif
#define	MT_CONTROL	14	/* extra-data protocol message */
#define	MT_OOBDATA	15	/* expedited data  */

#define	MT_NTYPES	16	/* number of mbuf types for mbtypes[] */

/*
 * mbuf statistics
 */

/* flags to m_get/MGET */
#define	M_DONTWAIT	1
#define	M_WAIT		0

/* Freelists:
 *
 * Normal mbuf clusters are normally treated as character arrays
 * after allocation, but use the first word of the buffer as a free list
 * pointer while on the free list.
 */

/*
 * These are identifying numbers passed to the m_mballoc_wait function,
 * allowing us to determine whether the call came from an MGETHDR or
 * an MGET.
 */
#define	MGETHDR_C      1
#define	MGET_C         2

/* length to m_copy to copy all */
#define	M_COPYALL	1000000000

/* compatibility with 4.3 */
#define	m_copy(m, o, l)	m_copym((m), (o), (l), M_DONTWAIT)

 /* lvl7_@ip_forward start*/
#define	RTMH_ALIGN(m, len) do {						\
	(m)->rtm_data += (MHLEN - (len)) & ~(sizeof(long) - 1);		\
} while (0)
 /* lvl7_@ip_forward end*/

#if defined(_L7_OS_LINUX_)
/*
 * pkthdr.aux type tags.
 */
struct mauxtag {
	int	af;
	int	type;
};
#endif

extern	u_int		 m_clalloc_wid;	/* mbuf cluster wait count */
extern	u_int		 m_mballoc_wid;	/* mbuf wait count */
extern	int		 max_linkhdr;	/* largest link-level header */
extern	int		 max_protohdr;	/* largest protocol header */
extern	int		 max_hdr;	/* largest link+protocol header */
extern	int		 max_datalen;	/* MHLEN - max_hdr */
extern	struct mbstat	 mbstat;
extern	u_long		 mbtypes[MT_NTYPES]; /* per-type mbuf allocations */
extern	int		 mbuf_wait;	/* mbuf sleep time */
extern	struct mbuf	*mbutl;		/* virtual address of mclusters */
extern	char		*mclrefcnt;	/* cluster reference counts */
extern	union mcluster	*mclfree;
extern	struct mbuf	*mmbfree;
extern	int		 nmbclusters;
extern	int		 nmbufs;
extern	int		 nsfbufs;

 /* lvl7_@ip_forward start*/
extern void rtm_adj(struct rtmbuf *mp,int req_len);
extern void rtm_copydata(register struct rtmbuf *m, register int off, register int len, caddr_t cp);
/* lvl7_@ip_forward end*/	

#endif /* !_SYS_MBUF_H_ */
