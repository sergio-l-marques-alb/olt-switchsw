/*
 * Copyright (c) 1988, 1989, 1993
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
 *	@(#)radix.h	8.2 (Berkeley) 10/31/94
 * $FreeBSD: /repoman/r/ncvs/src/sys/net/radix.h,v 1.23 2003/08/19 17:23:07 sam Exp $
 */

#ifndef _RADIX_H_
#define	_RADIX_H_

/*
 * Radix search tree node layout.
 */

/* radix node have been moved to radix_api.h */

#define	rn_dupedkey	rn_u.rn_leaf.rn_Dupedkey
#define	rn_key		rn_u.rn_leaf.rn_Key
#define	rn_mask		rn_u.rn_leaf.rn_Mask
#define	rn_offset	rn_u.rn_node.rn_Off
#define	rn_left		rn_u.rn_node.rn_L
#define	rn_right	rn_u.rn_node.rn_R

/*
 * Annotations to tree concerning potential routes applying to subtrees.
 */
/* radix_mask has been moved to radix_api.h */

#define	rm_mask rm_rmu.rmu_mask
#define	rm_leaf rm_rmu.rmu_leaf		/* extra field would make 32 bytes */

typedef int walktree_f_t(struct radix_node *, void *);

/* radix_node_head has been moved to radix_api.h */

#define Bcmp(a, b, n) bcmp(((char *)(a)), ((char *)(b)), (n))
#define Bcopy(a, b, n) bcopy(((char *)(a)), ((char *)(b)), (unsigned)(n))
#define Bzero(p, n) bzero((char *)(p), (int)(n));

/* tree implementation from radix.c */
int rn_walktree(struct radix_node_head *, walktree_f_t *, void *);
void rn_init(struct radix_node_head *);
int	 rn_inithead(struct radix_node_head *);
int	 rn_refines(void *, void *);
void * rn_getnext(struct radix_node *);
struct radix_node
	 *rn_addmask (struct radix_node_head  *, void *, int, int),
	 *rn_addroute (void *, void *, struct radix_node_head *, struct radix_node [2]),
	 *rn_delete (void *, void *, struct radix_node_head *),
	 *rn_lookup (void *v_arg, void *m_arg, struct radix_node_head *head, int),
   *rn_lookup_ascending (void *v_arg, void *m_arg, struct radix_node_head *head, int),
	 *rn_match (void *, struct radix_node_head *, int),
   *rn_match_count(void *, struct radix_node_head *, int);


/* utility functions from l7_radix_util.c */
void radixTreeFreeDataNode(radixTree_t * tree, void * ptr);
void* radixTreeNewDataNode(radixTree_t * tree);
void radixTreeFreeMaskNode(struct radix_node_head * head, void * ptr);
void * radixTreeNewMaskNode(struct radix_node_head * head);



#endif /* _RADIX_H_ */

