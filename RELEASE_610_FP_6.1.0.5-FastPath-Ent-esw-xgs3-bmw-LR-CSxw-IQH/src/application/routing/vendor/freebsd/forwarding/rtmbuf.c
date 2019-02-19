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
 /* lvl7_@ip_forward start*/
#include "rtmbuf.h"
void
rtm_adj(mp, req_len)
	struct rtmbuf *mp;
	int req_len;
{
	register int len = req_len;
	register struct rtmbuf *m;
	register int count;

	if ((m = mp) == NULL)
		return;
	if (len >= 0) {
		/*
		 * Trim from head.
		 */
		while (m != NULL && len > 0) {
			if (m->rtm_len <= len) {
				len -= m->rtm_len;
				m->rtm_len = 0;
				m = m->rtm_next;
			} else {
				m->rtm_len -= len;
				m->rtm_data += len;
				len = 0;
			}
		}
		m = mp;
		if (mp->rtm_flags & M_PKTHDR)
			m->rtm_pkthdr.len -= (req_len - len);
	} else {
		/*
		 * Trim from tail.  Scan the mbuf chain,
		 * calculating its length and finding the last mbuf.
		 * If the adjustment only affects this mbuf, then just
		 * adjust and return.  Otherwise, rescan and truncate
		 * after the remaining size.
		 */
		len = -len;
		count = 0;
		for (;;) {
			count += m->rtm_len;
			if (m->rtm_next == (struct rtmbuf *)0)
				break;
			m = m->rtm_next;
		}
		if (m->rtm_len >= len) {
			m->rtm_len -= len;
			if (mp->rtm_flags & M_PKTHDR)
				mp->rtm_pkthdr.len -= len;
			return;
		}
		count -= len;
		if (count < 0)
			count = 0;
		/*
		 * Correct length for chain is "count".
		 * Find the mbuf with last data, adjust its length,
		 * and toss data from remaining mbufs on chain.
		 */
		m = mp;
		if (m->rtm_flags & M_PKTHDR)
			m->rtm_pkthdr.len = count;
		for (; m; m = m->rtm_next) {
			if (m->rtm_len >= count) {
				m->rtm_len = count;
				break;
			}
			count -= m->rtm_len;
		}
    if (m == NULL)    /* shut up coverity */
      return;
		while (m->rtm_next)
			(m = m->rtm_next) ->rtm_len = 0;
	}
}

void
rtm_copydata(m, off, len, cp)
	register struct rtmbuf *m;
	register int off;
	register int len;
	caddr_t cp;
{
	register unsigned count;

    while (off > 0) {
        if (off < m->rtm_len)
			break;
		off -= m->rtm_len;
		m = m->rtm_next;
	}
	while (len > 0) {
        count = min(m->rtm_len - off, len);
		bcopy(rtmtod(m, caddr_t) + off, cp, count);
		len -= count;
		cp += count;
		off = 0;
		m = m->rtm_next;
	}
}
/* lvl7_@ip_forward end*/
