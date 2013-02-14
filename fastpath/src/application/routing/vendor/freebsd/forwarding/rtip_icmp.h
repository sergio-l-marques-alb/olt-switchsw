/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  rtip_icmp.h
*
* @purpose   Header File for ICMP Header definitions
*
* @component 
*
* @comments  the mbuf structure originally used in FreeBSD has been 
*            replaced by a similar kind of structure called rtmbuf
*            so that new data members can be added to it.Also a prefix 
*            of 'rt' has been added to all the mbuf related functions, 
*            structures and data members, so that they won't clash with
*            the VxWorks names.
*
* @create    03/26/2001
*                       
* @author    dipanjan
*
* @end
*             
**********************************************************************/
/*
 * Copyright (c) 1982, 1986, 1993
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
 *	@(#)ip_icmp.h	8.1 (Berkeley) 6/10/93
 * $FreeBSD: src/sys/netinet/ip_icmp.h,v 1.16 1999/12/29 04:41:01 peter Exp $
 */
/*********************************************************************
 *                   
 ********************************************************************/
#ifndef _NETINET_IP_ICMP_H_
#define _NETINET_IP_ICMP_H_

/* lvl7_@ip_forward start*/
#include <string.h>
#include "rtmbuf.h"
#include "rtip.h"
#include "rtip_var.h"
/* lvl7_@ip_forward end*/

#include "l7_icmp.h"

/* lvl7_@ip_forward start*/
extern void	icmp_error (struct rtmbuf *, int, int, u_long, struct rtm_ifnet *);
void	icmp_input (struct rtmbuf *, int, int);
int	 rtin_cksum (struct rtmbuf *, int);
n_time rtiptime();
/* lvl7_@ip_forward end*/

#endif
