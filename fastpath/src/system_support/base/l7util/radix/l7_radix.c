/*
 * Copyright (c) 1988, 1989, 1993
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
 *    must display the following acknowledgement:
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
 *  @(#)radix.c 8.5 (Berkeley) 5/19/95
 * $FreeBSD: /repoman/r/ncvs/src/sys/net/radix.c,v 1.32 2003/09/22 23:24:18 peter Exp $
 */

/*
 * Routines to build and maintain radix trees for routing lookups.
 *
 * ---> See radix_api.h for more on how all this works, and radix.ppt on lvl7ser3
 *      for some sample trees. Also use rtoDebugTree to dump the tree to
 *      the console for analysis.
 *
 */

#include "l7_radix_defs.h"
#include "radix_api.h"
#include "radix.h"

#define log(x, msg...) L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT, msg)

#if 0  /* LVL7 NOT USED */
static int rn_walktree_from(struct radix_node_head *h, void *a, void *m,
            walktree_f_t *f, void *w);
#endif /* LVL7 NOT USED */
static struct radix_node
     *rn_insert(void *, struct radix_node_head *, int *,
         struct radix_node [2]),
     *rn_newpair(void *, int, struct radix_node[2]),
     *rn_search(void *, struct radix_node *),
     *rn_search_m(void *, struct radix_node *, void *);

static char normal_chars[] = {0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, -1};

#define rn_masktop (head->rnh_treetop)

static int  rn_lexobetter(void *m_arg, void *n_arg);
static struct radix_mask * rn_new_radix_mask(struct radix_node_head *head, struct radix_node *tt, struct radix_mask *next);
static int rn_satisfies_leaf(struct radix_node_head * head, char *trial, struct radix_node *leaf, int skip);

/*
 * The data structure for the keys is a radix tree with one way
 * branching removed.  The index rn_bit at an internal node n represents a bit
 * position to be tested.  The tree is arranged so that all descendants
 * of a node n have keys whose bits all agree up to position rn_bit - 1.
 * (We say the index of n is rn_bit.)
 *
 * There is at least one descendant which has a one bit at position rn_bit,
 * and at least one with a zero there.
 *
 * A route is determined by a pair of key and mask.  We require that the
 * bit-wise logical and of the key and mask to be the key.
 * We define the index of a route to associated with the mask to be
 * the first bit number in the mask where 0 occurs (with bit number 0
 * representing the highest order bit).
 *
 * We say a mask is normal if every bit is 0, past the index of the mask.
 * If a node n has a descendant (k, m) with index(m) == index(n) == rn_bit,
 * and m is a normal mask, then the route applies to every descendant of n.
 * If the index(m) < rn_bit, this implies the trailing last few bits of k
 * before bit b are all 0, (and hence consequently true of every descendant
 * of n), so the route applies to all descendants of the node as well.
 *
 * Similar logic shows that a non-normal mask m such that
 * index(m) <= index(n) could potentially apply to many children of n.
 * Thus, for each non-host route, we attach its mask to a list at an internal
 * node as high in the tree as we can go.
 *
 * The present version of the code makes use of normal routes in short-
 * circuiting an explict mask and compare operation when testing whether
 * a key satisfies a normal route, and also in remembering the unique leaf
 * that governs a subtree.
 */

/*********************************************************************
* @purpose  patricia search for a candidate leaf using key
*
* @param    head              pointer to the entry from which to start
* @param    v_arg             pointer to a key (ipaddr) with 1st byte=len
*
* @returns  radix_node *      pointer to the first leaf node found
*
* @notes    This functions walks down the radix tree, testing the bits
*           specified in the tree node, and branching accordingly.
*           The fn stops as soon as it hits a leaf node (rn_bit -ve).
*           Since only the offset bit is tested, there is a possiblity
*           that bits before and/or after than bit are also non-matching.
*
* @end
*********************************************************************/
static struct radix_node *
rn_search(v_arg, head)
    void *v_arg;
    struct radix_node *head;
{
    register struct radix_node *x;
    register caddr_t v;

    /* loop till a leaf (-ve rn_bit) */
    for (x = head, v = v_arg; x->rn_bit >= 0;) {
        if (x->rn_bmask & v[x->rn_offset])
            x = x->rn_right;
        else
            x = x->rn_left;
    }
    return (x);
}

/*********************************************************************
* @purpose  mask restricted patricia search for a candidate leaf
*
* @param    head              pointer to the entry from which to start
* @param    v_arg             pointer to a key (ipaddr) with 1st byte=len
* @param    m_arg             pointer to a mask with 1st byte=len
*
* @returns  radix_node *      pointer to the first leaf node found
*
* @notes    Similar to rn_search but more restrictive. Goes down the
*           tree only if both key and mask have the bit at rn_offset set.
*           This is useful when one wants to restrict the search to a
*           shorter mask while backtracking up the tree.
*           e.g. rn_search whould find 1.1.1.0/28 for 1.1.1.129 bypassing
*           the shorter 1.1.1.0/24. But rn_search_m would stop at ../24
*           when called with mask 255.255.255.0 (i.e. /24)
*
* @end
*********************************************************************/
static struct radix_node *
rn_search_m(v_arg, head, m_arg)
    struct radix_node *head;
    void *v_arg, *m_arg;
{
    register struct radix_node *x;
    register caddr_t v = v_arg, m = m_arg;

    for (x = head; x->rn_bit >= 0;) {
        /* check bits in m(ask) andv(key) */
        if ((x->rn_bmask & m[x->rn_offset]) &&
            (x->rn_bmask & v[x->rn_offset]))
            x = x->rn_right;
        else
            x = x->rn_left;
    }
    return x;
}

/*********************************************************************
* @purpose  checks whether one mask refines another
*
* @param    m_arg             pointer to a mask with 1st byte=len
* @param    n_arg             pointer to a mask with 1st byte=len
*
* @returns  1      if m_arg refines n_arg
* @returns  0      if m_arg does not refine n_arg
*
* @notes    e.g. ff.ff.ff.f0 refines ff.ff.ff.00
*
* @end
*********************************************************************/
int
rn_refines(m_arg, n_arg)
    void *m_arg, *n_arg;
{
    register caddr_t m = m_arg, n = n_arg;
    register caddr_t lim, lim2 = lim = n + *(u_char *)n;
    int longer = (*(u_char *)n++) - (int)(*(u_char *)m++);
    int masks_are_equal = 1;

        /* run for the shorter length */
    if (longer > 0)
        lim -= longer;
    while (n < lim) {
        if (*n & ~(*m))
            return 0;
        if (*n++ != *m++)
            masks_are_equal = 0;
    }
        /* n_arg is longer */
    while (n < lim2)
        if (*n++)
            return 0; /* n has bits set to 1 beyond m's length */
    /* m_arg is longer */
    if (masks_are_equal && (longer < 0))
        for (lim2 = m - longer; m < lim2; )
            if (*m++) /* m refines n! */
                return 1;
    return (!masks_are_equal);
}

/*********************************************************************
* @purpose  Lookup an entry that matches a key and mask exactly
*
* @param    head              pointer to the tree head
* @param    v_arg             pointer to a key with 1st byte=len
* @param    n_arg             pointer to a mask with 1st byte=len
* @param    type              lookup type, for EXACT or GETNEXT
*
* @returns  radix_node *      pointer to the node if found
* @returns  null              if not found
*
* @notes    This function uses rn_addmask in search only mode to
*           find an entry for the netmask. rn_match is used to find
*           an entry matching the key. Duplicates of this entry are
*           then searched to find the entry exactly matching the key
*           and the mask.
*
* @end
*********************************************************************/
struct radix_node *
rn_lookup(v_arg, m_arg, head, type)
    void *v_arg, *m_arg;
    struct radix_node_head *head;
    int type;
{
    register struct radix_node *x;
    register struct radix_node *saved_x;
    caddr_t netmask = 0;
  int isDefault = 0;
  int keyMatch = 0;

  if (m_arg) {
        /* Use rn_addmask in search only mode to find an entry for the netmask */
        x = rn_addmask(head, m_arg, 1, head->rnh_treetop->rn_offset);
        if (x == 0 && type == L7_RN_EXACT)
            return (0); /* mask not found */

        /* it is possible that the mask is not in the tree
         * for getnext operation */
        if (x)
    {
      netmask = x->rn_key;
      isDefault = (((char *)x->rn_key)[0] == 0) ? 1 : 0;
    }
    }

    /* find an entry matching the key in the tree (best match!) */
    x = rn_match(v_arg, head, 0);
  saved_x = x;

  /* check to see if the key is an "exact" match.
   * it wont be an exact match if v_arg has host bits set,
   * (e.g. when SNMP iterates :-/ ) rn_match is happy with
   * "net" matches, but lookup requires a "host" match.
   *
   * another ugly special case for the default route requires
   * us to ignore the length byte here as the default route
   * has its length set to 0 :-/
   */
  if(x && memcmp( ((unsigned char*)x->rn_key)+1,
                  ((unsigned char*)v_arg)+1,
                  head->max_keylen-1) == 0)
    keyMatch = 1;

  /* if its not an exact match reset x.
   * note that we still have the match in saved_x. This
   * serves as a starting point for getting the next entry
   */
  if(!keyMatch)
    x = 0;

    if (x && netmask) {
        /* walk duplicates e.g. for 1.1.1.0/24 and 1.1.1.0/28 */
        while (x && x->rn_mask != netmask)
            x = x->rn_dupedkey;
    }

  /* exact match? */
  if(type == L7_RN_EXACT)
          return x;

  /* found an exact match? */
  if(x && netmask)
  {
    /* return next node */
    while((x = (struct radix_node *)rn_getnext(x)))
    {
      /* make sure its not a mask node
       * (both key and mask must be valid ptrs
       */
      if(x->rn_mask && x->rn_key)
        break;
    }

    return(x);
  }

  /* When a all zeros netmask is passed in, it always matches rn_zeros.
   * When this is the case (isDefault != 0) and
   * we got an entry with a matching key (keyMatch == 1)
   * but could not find an entry with a matching mask (x == 0),
   *
   * Then return what we got, becuase the caller must have passed in a
   * non zero key with a zero mask => he doesn't care about the mask.
   */
  if(isDefault && keyMatch && !x)
    return(saved_x);

  /* if no match start at first node, and find the first valid leaf */
  if(!saved_x)
  {
    saved_x = head->rnh_treetop;
    while((saved_x = (struct radix_node *)rn_getnext(saved_x)))
    {
      if(saved_x->rn_mask && saved_x->rn_key)
        break;
    }
  }

  /* a null netmask means rn_addmask failed. so copy the netmask
   * from the buffer in which the rn_addmask calculated it!
   * This is to get the "trimmed" length for the netmask the user
   * passed in.
   */
  if(!netmask)
          netmask = head->addmask_key;

  /* matched but not found, try to get next */
  x = saved_x;

  /* Open code rn_getnext, to deal with a mask that isn't in the list! */

  /* node addr matches? */
  if(x && keyMatch)
  {
    /* find item with a smaller mask (entries are longest mask first) */
    while(x && memcmp(x->rn_mask, netmask, head->max_keylen) > 0)
    {
      x = x->rn_dupedkey;
    }

    if(x)
      return(x);
  }

  /* iterate to find item with key > v_arg */
  x = saved_x;

  while(x)
  {
    /* make sure its not a mask node (both key and mask must be valid ptrs */
    if(x->rn_mask && x->rn_key)
    {
      /* break on larger key value */
      if(memcmp(x->rn_key, v_arg, head->max_keylen) > 0)
        break;
    }

    x = (struct radix_node *)rn_getnext(x);
  }


  return x;
}

/* rn_getascending: get the next ascending node (smallest mask)
 *
 * This function is no use if you want to an ascending
 * dupkey. e.g. going from 1.0.0.0/8 to 1.0.0.0/16 because
 * the dup list is a singly linked list in descending order
 * so there is no way to walk back!
 *
 * That is best done outside this function based off a call to
 * rn_match. e.g. in rn_lookup see the use of variable "ascending_x"
 */
void *
rn_getascending(rn)
        struct radix_node *rn;
{
  struct radix_node *saved_rn;

  /* First time through node, go left */
  while (rn->rn_bit >= 0)
    rn = rn->rn_left;

  /* If at right child go back up, otherwise, go right */
  while (rn &&
         (rn->rn_parent->rn_right == rn) &&
         ((rn->rn_flags & RNF_ROOT) == 0))
    rn = rn->rn_parent;

  /* Find the next *leaf* since next node might vanish, too */
  if(rn)
  {
    for (rn = rn->rn_parent->rn_right; rn && rn->rn_bit >= 0;)
      rn = rn->rn_left;
  }

  if (!rn || rn->rn_flags & RNF_ROOT)
    return (0);

  if (rn->rn_dupedkey)
  {
    /* go to last item in the chain
     * because masks are stored in descending order
     * and we want the smallest one first
     */
    while(rn)
    {
      saved_rn = rn;
      rn = rn->rn_dupedkey;
    }

    if (!(saved_rn->rn_flags & RNF_ROOT))
      return(saved_rn);
    else
      return ((struct radix_node *)0);
  }
  else
  {
    return rn;
  }

  return(0);
}



/*********************************************************************
* @purpose  Lookup an the next entry for a key and mask in ascending
*           order of key and mask (shortest mask first)
*
* @param    head              pointer to the tree head
* @param    v_arg             pointer to a key with 1st byte=len
* @param    n_arg             pointer to a mask with 1st byte=len
* @param    type              lookup type, for EXACT or L7_RN_GETNEXT_ASCEND
*
* @returns  radix_node *      pointer to the node if found
* @returns  null              if not found
*
* @notes    This function is differs from rn_lookup in that it returns
*           items with the same key in ascending mask size (shortest first).
*           This is the opposite of how the radix tree generally works
*           viz. longest prefix match first.
*
*           !IMPORTANT!
*           This function can NOT be used in conjunction with rn_getnext
*           to iterate the tree as the two take opposing views of mask order.
*
* @end
*********************************************************************/
struct radix_node *
rn_lookup_ascending(v_arg, m_arg, head, type)
    void *v_arg, *m_arg;
    struct radix_node_head *head;
    int type;
{
    register struct radix_node *x;
  register struct radix_node *saved_x;
  register struct radix_node *ascending_x = 0;
    caddr_t netmask = 0;
  int isDefault = 0;
  int keyMatch = 0;

  if (m_arg) {
        /* Use rn_addmask in search only mode to find an entry for the netmask */
        x = rn_addmask(head, m_arg, 1, head->rnh_treetop->rn_offset);
        if (x == 0 && type == L7_RN_EXACT)
            return (0); /* mask not found */

        /* it is possible that the mask is not in the tree
         * for getnext operation */
        if (x)
    {
      netmask = x->rn_key;
      isDefault = (((char *)x->rn_key)[0] == 0) ? 1 : 0;
    }
    }

    /* find an entry matching the key in the tree (best match!) */
    x = rn_match(v_arg, head, 0);
    saved_x = x;

  /* check to see if the key is an "exact" match.
   * it wont be an exact match if v_arg has host bits set,
   * (e.g. when SNMP iterates :-/ ) rn_match is happy with
   * "net" matches, but lookup requires a "host" match.
   *
   * another ugly special case for the default route requires
   * us to ignore the length byte here as the default route
   * has its length set to 0 :-/
   */
  if(x && memcmp( ((unsigned char*)x->rn_key)+1,
                  ((unsigned char*)v_arg)+1,
                  head->max_keylen-1) == 0)
    keyMatch = 1;

  /* if its not an exact match reset x.
   * note that we still have the match in saved_x. This
   * serves as a starting point for getting the next entry
   */
  if(!keyMatch)
    x = 0;

  if (x && netmask) {
        /* walk duplicates e.g. for 1.1.1.0/24 and 1.1.1.0/28 */
        while (x && x->rn_mask != netmask)
    {
      /* masks are stored in descending order, so cache the
       * the previous (bigger) mask for the ascending get_next
       * We have to do this because the dup list is a singly linked
       * list so there is no way to walk back!
       */
      ascending_x = x;
      x = x->rn_dupedkey;
    }
    }

  /* return result if an exact match was requested */
  if(type == L7_RN_EXACT)
    return x;

  /* found an exact match, now get next */
  if(x && netmask)
  {
    /* Case 1: have a cached result?
     * We will have a cached result when the key has a route with a
     * more specific (longer) mask than the one passed in.
     *
    */
    if(ascending_x)
      return ascending_x;

    /* Case 2: The key matches but we dont have a more specific mask
     * so, get the next node.
     * we use rn_getascending: this will search for the next node
     * a'la rn_getnext, then return the dup'd entry with the
     * shortest mask.
     */
    while((x = (struct radix_node *)rn_getascending(x)))
    {
      /* make sure its not a mask node
       * (both key and mask must be valid ptrs)
       */
      if(x->rn_mask && x->rn_key)
        break;
    }

    return(x);
  }

  /* Case 3: First route for a subnet (mask is all zeros)
   * When a all zeros netmask is passed in, it always matches rn_zeros.
   * When this is the case (isDefault != 0) and
   * we got a matching key (keyMatch == 1),
   * but not an exact match as none of the items in the dupd list
   * had a mask that matched (x == 0),
   *
   * Then return the last item in the duped chain of what we got,
   * becuase the caller must have passed in a
   * non zero key with a zero mask => he doesn't care about the mask.
   *
   * The last item in the duped chain will have the smallest mask.
   */
  if(isDefault && keyMatch && !x)
  {
    /* loop till there are no more duplicates */
    while(saved_x)
    {
      ascending_x = saved_x;  /* cache last value */
      saved_x = saved_x->rn_dupedkey;
    }

    return(ascending_x);
  }

  /* if no match start at first node, and find the first valid leaf
   * (this prepares for the fuzzy comparison cases below)
   */
  if(!saved_x)
  {
    saved_x = head->rnh_treetop;
    while((saved_x = (struct radix_node *)rn_getascending(saved_x)))
    {
      if(saved_x->rn_mask && saved_x->rn_key)
        break;
    }
  }

  /* a null netmask means rn_addmask failed. so copy the netmask
   * from the buffer in which the rn_addmask calculated it!
   * This is to get the "trimmed" length for the netmask the user
   * passed in.
   */
  if(!netmask)
          netmask = head->addmask_key;

  /* matched but not found, try to get next */
  x = saved_x;

  /* Open code rn_getascending, to deal with a key and mask that are
   * (potentially) not in the tree!
   */

  /* Case 4: node key found in tree, but mask not found
   * find the first item in the dup'd list with a larger mask
   * (entries are longest mask first)
   */
  if(x && keyMatch)
  {
    int result = 0;

    while(x)
    {
      result = memcmp(x->rn_mask, netmask, head->max_keylen);

      /* cache items with a longer mask */
      if(result > 0)
        ascending_x = x;

      /* stop iterating once we're at an equal/smaller mask */
      if(result <= 0)
        break;

      x = x->rn_dupedkey;
    }

    if(ascending_x)
      return(ascending_x);
  }

  /* iterate to find item with key > v_arg */
  x = saved_x;

  /* Case 5:
   * - mask not found and key not found in tree, or
   * - key found, but no dup'd nodes with a mask that is
   *   more specific than the one passed in.
   *
   * Move to a node with a larger key.
   */
  while(x)
  {
    /* make sure its not a mask node (both key and mask must be valid ptrs */
    if(x->rn_mask && x->rn_key)
    {
      /* break on larger key value */
      if(memcmp(x->rn_key, v_arg, head->max_keylen) > 0)
        break;
    }

    x = (struct radix_node *)rn_getascending(x);
  }


  return x;
}
/*********************************************************************
* @purpose  masked bytewise comparison of keys
*
* @param    head              pointer to the tree head
* @param    trial             pointer to the key being tried
* @param    leaf              pointer to the tree leaf to compare with
* @param    skip              number of bytes to skip (offset)
*
* @returns
*
* @notes    verifies that the bytes in trial key match the non-masked bytes
*           in the leaf key. Used to check if a leaf represents a net
*           route to the dest in trial.
*
* @end
*********************************************************************/
static int
rn_satisfies_leaf(head, trial, leaf, skip)
  struct radix_node_head * head;
  char *trial;
  register struct radix_node *leaf;
  int skip;
{
    char * rn_ones = head->rn_ones;

    register char *cp = trial, *cp2 = leaf->rn_key, *cp3 = leaf->rn_mask;
    char *cplim;
    int length = min(*(u_char *)cp, *(u_char *)cp2);

    if (cp3 == 0)
        cp3 = rn_ones;
    else
        length = min(length, *(u_char *)cp3);
    cplim = cp + length; cp3 += skip; cp2 += skip;
    for (cp += skip; cp < cplim; cp++, cp2++, cp3++)
        if ((*cp ^ *cp2) & *cp3) /* if mask has a 1, key and leaf must match (xor) */
            return 0; /* since its a xor any mismatch will be non-zero */
    return 1;
}

/*********************************************************************
* @purpose  Find a leaf entry with the longest match of the key
*
* @param    head              pointer to the tree head
* @param    v_arg             pointer to a key with 1st byte=len
* @param    skipentries       number of matching entries to skip.
*                             0    => returns best match (longest)
*                             1..n => finds best match and backtracks
*                                     and returns a shorter match.
*
* @returns  radix_node *      pointer to the node if found
* @returns  null              if not found
*
* @notes    The entry returned is the one with the longest mask out
*           of the set of entries that match. The match is therefore
*           done based on (key & node_mask) == (node_key & node_mask)
*
*           Setting skipentries to 0 returns the best match.
*           To back track up the tree set skipentries to 1,2..n.
*           e.g. for a tree with 1.1.1.0/24 and 1.1.0.0/16, the address
*           1.1.1.10 will match A for skipentries=0, B for skipentries=1.
*           Setting skipentries to 3..n will return null as there are no
*           more matching entries.
*
* @end
*********************************************************************/
struct radix_node *
rn_match(v_arg, head, skipentries)
    void *v_arg;
    struct radix_node_head *head;
    int skipentries;
{
    int saved_skipentries;
    caddr_t v = v_arg;
    register struct radix_node *t = head->rnh_treetop, *x;
    register caddr_t cp = v, cp2;
    caddr_t cplim;
    struct radix_node *saved_t, *top = t;
    int off = t->rn_offset, vlen = *(u_char *)cp, matched_off;
    register int test, b, rn_bit;

    /*
     * Open code rn_search(v, top) to avoid overhead of extra
     * subroutine call.
     */
    for (; t->rn_bit >= 0; ) {
        if (t->rn_bmask & cp[t->rn_offset])
            t = t->rn_right;
        else
            t = t->rn_left;
    }
    /*
     * See if we match exactly as a host destination
     * or at least learn how many bits match, for normal mask finesse.
     *
     * It doesn't hurt us to limit how many bytes to check
     * to the length of the mask, since if it matches we had a genuine
     * match and the leaf we have is the most specific one anyway;
     * if it didn't match with a shorter length it would fail
     * with a long one.  This wins big for class B&C netmasks which
     * are probably the most common case...
     */
    if (t->rn_mask)
        vlen = *(u_char *)t->rn_mask;
    cp += off; cp2 = t->rn_key + off; cplim = v + vlen;
    for (; cp < cplim; cp++, cp2++)
        if (*cp != *cp2) /* bytes dont match? go on(1) to locate the offending bit */
            goto on1;
    /*
     * This extra grot is in case we are explicitly asked
     * to look up the default.  Ugh!
     *
     * Never return the root node itself, it seems to cause a
     * lot of confusion.
     */
    if (t->rn_flags & RNF_ROOT)
        t = t->rn_dupedkey;

    if(skipentries)
      skipentries--;    /* skip exact match */
    else
      return t;         /* return exact match */
on1:
    saved_skipentries = skipentries;
    if (cp == cplim)
    {
      /* node key matches value we are searching for. Must have gotten here
       * because skipentries != 0. So set b to indicate complete match. */
      matched_off = cp - v;
      b = matched_off << 3;    /* bytes to bits */
      rn_bit = -1 - b; /* the offset as ones complement is how the leaf stores it */
      saved_t = t;
    }
    else
    {
      /* The search tested the bit specified by rn_bmask (rn_bit)
       * so some other  bit may also vary before it!
       * So find first bit that differs!
       * also test is an int so & with 0xff to ensure all leading bits are zero */
      test = (*cp ^ *cp2) & 0xff;
      for (b = 7; (test >>= 1) > 0;)
          b--;
      matched_off = cp - v;
      b += matched_off << 3; /* add bits for the bytes skipped */
      rn_bit = -1 - b; /* the offset as ones complement is how the leaf stores it */
      /*
       * If there is a host route in a duped-key chain, it will be first.
       */
      if ((saved_t = t)->rn_mask == 0)
          t = t->rn_dupedkey;
      for (; t; t = t->rn_dupedkey)
          /*
           * Even if we don't match exactly as a host,
           * we may match if the leaf we wound up at is
           * a route to a net.
           */
          if (t->rn_flags & RNF_NORMAL) {
              /* differing bit is at rn_bit or later! */
              if (rn_bit <= t->rn_bit) {
                  if(!skipentries)
                    return t;
                  else
                    skipentries--;
              }
          } /* differing bit before rn_bit masked out? */
          else if (rn_satisfies_leaf(head, v, t, matched_off)) {
              if(!skipentries)
                return t;
              else
                skipentries--;
          }
    }
    t = saved_t;
    skipentries = saved_skipentries;

    /* Most specific leaf did not match so
     * start searching up the tree */
    do {
        register struct radix_mask *m;
        t = t->rn_parent;
        m = t->rn_mklist;
        /*
         * If non-contiguous masks ever become important
         * we can restore the masking and open coding of
         * the search and satisfaction test and put the
         * calculation of "off" back before the "do".
         */

        /* Iterate masks associated with the key (rn_mklist)
         * e.g. ff.ff.ff.0 and ff.ff.0.0 for 1.1.0.0
         */
        while (m) {
            if (m->rm_flags & RNF_NORMAL) {
                /* for normal masks a bit pos check is enough */
                if (rn_bit <= m->rm_bit) {
                  if (!skipentries)
                    return (m->rm_leaf);
                  else
                    skipentries--;
                }
            }
            else {
                off = min(t->rn_offset, matched_off);
                /* force search to end at current mask
                 * rather than longest mask */
                x = rn_search_m(v, t, m->rm_mask);
                while (x && x->rn_mask != m->rm_mask)
                    x = x->rn_dupedkey;
                if (x && rn_satisfies_leaf(head, v, x, off)) {
                    if(!skipentries)
                      return x;
                    else
                      skipentries--;
                }
            }
            m = m->rm_mklist; /* next mask */
        }
    } while (t != top);
    return 0;
}

#ifdef RN_DEBUG
int rn_nodenum;
struct  radix_node *rn_clist;
int rn_saveinfo;
int rn_debug =  1;
#endif

/*********************************************************************
* @purpose  initialize a node+leaf pair of entries
*
* @param    v                 pointer to a key with 1st byte=len
* @param    b                 bit offset of the bit to be tested
* @param    nodes             pointer to memory for 2 nodes
*
* @returns  radix_node *      pointer to the initialized nodes
*
* @notes    Every item is added to the radix tree as a leaf, node pair.
*           The node entry (t) has the test bit offset and pointers to
*           left and right children. The tree branches left if the bit
*           at the offset is 0 else right.
*
*           The leaf node has the rn_bit set to -ve (-1). It also has
*           pointers to the key, mask and other entries with a duplicate
*           key (but shorter mask).
*
* @end
*********************************************************************/
static struct radix_node *
rn_newpair(v, b, nodes)
    void *v;
    int b;
    struct radix_node nodes[2];
{
    register struct radix_node *tt = nodes, *t = tt + 1;

    /* initialize node */
    t->rn_bit = b;
    t->rn_bmask = 0x80 >> (b & 7); /* right shift restricted to 8 (7 is 111) */
    t->rn_left = tt;
    t->rn_offset = b >> 3; /* divide by 8 to get byte offset */

    /* initialize leaf */
    tt->rn_bit = -1;
    tt->rn_key = (caddr_t)v;
    tt->rn_parent = t;
    tt->rn_flags = t->rn_flags = RNF_ACTIVE;
    tt->rn_mklist = t->rn_mklist = 0;
#ifdef RN_DEBUG
    tt->rn_info = rn_nodenum++; t->rn_info = rn_nodenum++;
    tt->rn_twin = t;
    tt->rn_ybro = rn_clist;
    rn_clist = tt;
#endif
    return t;
}

/*********************************************************************
* @purpose  Insert a node+leaf pair into the radix tree
*
* @param    v_arg             pointer to a key with 1st byte=len
* @param    head              pointer to the tree head
* @param    dupentry          output parameter set to 1 if duplicate
* @param    nodes             pointer to nodes
*
* @returns  radix_node *      pointer to the leaf inserted
* @returns  radix_node *      pointer to the duplicate node (dupentry set)
*
* @notes    This function does a "patricia" insert!
*
* @end
*********************************************************************/
static struct radix_node *
rn_insert(v_arg, head, dupentry, nodes)
    void *v_arg;
    struct radix_node_head *head;
    int *dupentry;
    struct radix_node nodes[2];
{
    caddr_t v = v_arg;
    struct radix_node *top = head->rnh_treetop;
    int head_off = top->rn_offset, vlen = (int)*((u_char *)v);
    register struct radix_node *t = rn_search(v_arg, top); /* first potentially  matching leaf */
    register caddr_t cp = v + head_off;
    register int b;
    struct radix_node *tt;
        /*
     * Find first bit at which v and t->rn_key differ
     */
    {
    register caddr_t cp2 = t->rn_key + head_off;
    register int cmp_res;
    caddr_t cplim = v + vlen;

    /* compare key (cp) and leaf key (cp2) bytes */
    while (cp < cplim)
        if (*cp2++ != *cp++)
            goto on1;
    *dupentry = 1; /* if all are same it a duplicate, no need to insert */
    return t;
on1:
    /* calc bit offset 'b' at which the 2 bytes differ */
    *dupentry = 0;
    cmp_res = (cp[-1] ^ cp2[-1]) & 0xff;
    for (b = (cp - v) << 3; cmp_res; b--)
        cmp_res >>= 1;
    }
    {
    /* Go through the tree again with the additional constraint that the
     * bit being tested (rn_bit) is < (before) b (calculated offset at which
     * the new key and the "potentially matching" leaf key differ).
     *
     * Thats where this entry will go in!
     */
    register struct radix_node *p, *x = top;
    cp = v;
    do {
        p = x;
        if (cp[x->rn_offset] & x->rn_bmask)
            x = x->rn_right;
        else
            x = x->rn_left;
    } while (b > (unsigned) x->rn_bit);
                /* x->rn_bit < b && x->rn_bit >= 0 (not a leaf) */
#ifdef RN_DEBUG
    if (rn_debug)
        log(LOG_DEBUG, "rn_insert: Going In:\n"), traverse(p);
#endif
    /* Insert into the tree a entry 'x' */
    t = rn_newpair(v_arg, b, nodes);
    tt = t->rn_left;
    if ((cp[p->rn_offset] & p->rn_bmask) == 0)
        p->rn_left = t;
    else
        p->rn_right = t;
    x->rn_parent = t;
    t->rn_parent = p; /* frees x, p as temp vars below */
    if ((cp[t->rn_offset] & t->rn_bmask) == 0) {
        t->rn_right = x;
    } else {
        t->rn_right = tt;
        t->rn_left = x;
    }
#ifdef RN_DEBUG
    if (rn_debug)
        log(LOG_DEBUG, "rn_insert: Coming Out:\n"), traverse(p);
#endif
    }
    return (tt);
}

/*********************************************************************
* @purpose  search/add an annotating mask node
*
* @param    head              pointer to the tree head
* @param    n_arg             pointer to the mask (1st byte=length)
* @param    search            1 = search only, 0 = search and insert
* @param    skip              number of bytes to skip (offset)
*
* @returns  radix_node *      pointer to the node added
*
* @notes    This function adds a node+leaf pair to the tree for
*           every unique mask. These serve as annotations in the tree.
*           The mask is left padded with 1's till skip bytes and right
*           padded with 0's. All zero trailing bytes are deducted from
*           the length of the mask.
*
* @end
*********************************************************************/
struct radix_node *
rn_addmask(head, n_arg, search, skip)
  struct radix_node_head * head;
    int search, skip;
    void *n_arg;
{
    caddr_t netmask = (caddr_t)n_arg;
    register struct radix_node *x;
    register caddr_t cp, cplim;
    register int b = 0, mlen, j;
    int maskduplicated, m0, isnormal;
    struct radix_node *saved_x;
  int max_keylen = head->max_keylen;
  char *addmask_key = head->addmask_key;
  char *rn_ones = head->rn_ones;

    if ((mlen = *(u_char *)netmask) > max_keylen)
        mlen = max_keylen;
    if (skip == 0)
        skip = 1;
    if (mlen <= skip)
        return (head->rnh_nodes);
    /* left pad with 1's for skip bytes. But leave length (1st byte)=0 */
    if (skip > 1)
        Bcopy(rn_ones + 1, addmask_key + 1, skip - 1);
    /* copy non-skip part of netmask */
    if ((m0 = mlen) > skip)
        Bcopy(netmask + skip, addmask_key + skip, mlen - skip);
    /*
     * Trim trailing zeroes. (detect first 1 from the right)
     */
    for (cp = addmask_key + mlen; (cp > addmask_key) && cp[-1] == 0;)
        cp--;
    /* set mlen to no. of significant (non 0) bytes + offset */
    mlen = cp - addmask_key;
    if (mlen <= skip) {
        if (m0 >= head->mask_last_zeroed)
            head->mask_last_zeroed = mlen;
        return (head->rnh_nodes);
    }
    if (m0 < head->mask_last_zeroed)
        Bzero(addmask_key + m0, head->mask_last_zeroed - m0);
    /* set length in first byte */
    *addmask_key = head->mask_last_zeroed = mlen;

    /* search for the mask in the tree */
    x = rn_search(addmask_key, rn_masktop);

    /* exact match? */
    if (Bcmp(addmask_key, x->rn_key, mlen) != 0)
        x = 0;

    /* if found or asked only to search... */
    if (x || search)
        return (x);

    /* not found, need to insert! */

    /* changed alloc method to use user specified heap */
        /* R_Malloc(x, struct radix_node *, max_keylen + 2 * sizeof (*x)); */
        x = (struct radix_node *)radixTreeNewMaskNode(head);

    if ((saved_x = x) == 0)
        return (0);

    Bzero(x, max_keylen + 2 * sizeof (*x));
    netmask = cp = (caddr_t)(x + 2);
    Bcopy(addmask_key, cp, mlen);
    x = rn_insert(cp, head, &maskduplicated, x);
    if (maskduplicated) {
        log(LOG_ERR, "rn_addmask: mask impossibly already in tree");
                /* changed alloc method to use heap */
            /* Free(saved_x); */
                radixTreeFreeMaskNode(head, saved_x);
        return (x);
    }
    /*
     * Calculate index of mask, and check for normalcy.
     */
    cplim = netmask + mlen; isnormal = 1;
    for (cp = netmask + skip; (cp < cplim) && *(u_char *)cp == 0xff;)
        cp++;
    if (cp != cplim) {
        for (j = 0x80; (j & *cp) != 0; j >>= 1)
            b++;
        if (*cp != normal_chars[b] || cp != (cplim - 1))
            isnormal = 0;
    }
    b += (cp - netmask) << 3;
    x->rn_bit = -1 - b;
    if (isnormal)
        x->rn_flags |= RNF_NORMAL;
    return (x);
}

/*********************************************************************
* @purpose  arbitrary ordering for non-contiguous masks
*
* @param    m_arg              pointer to a mask
* @param    n_arg              pointer to a mask
*
* @returns  1 if m_arg is lexographically better
*
* @notes
*
* @end
*********************************************************************/
static int
rn_lexobetter(m_arg, n_arg)
    void *m_arg, *n_arg;
{
    register u_char *mp = m_arg, *np = n_arg, *lim;

    if (*mp > *np)
        return 1;  /* not really, but need to check longer one first */
    if (*mp == *np)
        for (lim = mp + *mp; mp < lim;)
            if (*mp++ > *np++)
                return 1;
    return 0;
}

/*********************************************************************
* @purpose  allocate and initialize a new radix_mask
*
* @param    head              pointer to the tree head
* @param    tt                pointer to the nodes to be managed
*                             by this mask
* @param    next              pointer to the next radix_mask
*
* @returns  radix_mask *      pointer to the radix_mask
*
* @notes    The mask is allocated from the tree heap.
*
* @end
*********************************************************************/
static struct radix_mask *
rn_new_radix_mask(head, tt, next)
    register struct radix_node_head *head;
    register struct radix_node *tt;
    register struct radix_mask *next;
{
    register struct radix_mask *m;

  /* pop one off the free list */
  if (head->rn_mkfreelist)
  {
        m = head->rn_mkfreelist;
        head->rn_mkfreelist = (m)->rm_mklist;
    (m)->rm_mklist = 0;
    }
  else
  {
    return(0);
  }

    if (m == 0) {
        log(LOG_ERR, "Mask for route not entered\n");
        return (0);
    }
    Bzero(m, sizeof *m);
    m->rm_bit = tt->rn_bit;
    m->rm_flags = tt->rn_flags;
    if (tt->rn_flags & RNF_NORMAL)
        m->rm_leaf = tt;
    else
        m->rm_mask = tt->rn_mask;
    m->rm_mklist = next;
    tt->rn_mklist = m;
    return m;
}

/*********************************************************************
* @purpose  add a route to the tree
*
* @param    v_arg             pointer to the key
* @param    m_arg             pointer to the mask
* @param    head              pointer to the tree head
* @param    treenodes         pointer to the nodes
*
* @returns  radix_node *      added node
*
* @notes   This function adds a node+leaf for the route to the
*          radix tree. It also handles duplicate keys if masks differ.
*
*
* @end
*********************************************************************/
struct radix_node *
rn_addroute(v_arg, n_arg, head, treenodes)
    void *v_arg, *n_arg;
    struct radix_node_head *head;
    struct radix_node treenodes[2];
{
    caddr_t v = (caddr_t)v_arg, netmask = (caddr_t)n_arg;
    register struct radix_node *t, *x = 0, *tt;
    struct radix_node *saved_tt, *top = head->rnh_treetop;
    short b = 0, b_leaf = 0;
    int keyduplicated;
    caddr_t mmask;
    struct radix_mask *m, **mp;

    /*
     * In dealing with non-contiguous masks, there may be
     * many different routes which have the same mask.
     * We will find it useful to have a unique pointer to
     * the mask to speed avoiding duplicate references at
     * nodes and possibly save time in calculating indices.
     */
    if (netmask)  {
        /* search for/add an annotation for our mask */
        if ((x = rn_addmask(head, netmask, 0, top->rn_offset)) == 0)
            return (0);
        b_leaf = x->rn_bit;
        b = -1 - x->rn_bit;
        netmask = x->rn_key;
    }

  /* If we're adding a default route (b=0) then set the key length
   * to zero to ensure that the node is added as a duplicate of rn_zeros
   */
  if(b == 0)
  {
    ((unsigned char *)v)[0] = 0;
  }

    /*
     * Deal with duplicated keys: attach node to previous instance
     */
    saved_tt = tt = rn_insert(v, head, &keyduplicated, treenodes);
    if (keyduplicated) {
        for (t = tt; tt; t = tt, tt = tt->rn_dupedkey) {
            if (tt->rn_mask == netmask)
                return (0);
            if (netmask == 0 ||
                (tt->rn_mask &&
                 ((b_leaf < tt->rn_bit) /* index(netmask) > node */
                  || rn_refines(netmask, tt->rn_mask)
                  || rn_lexobetter(netmask, tt->rn_mask))))
                break;
        }
        /*
         * If the mask is not duplicated, we wouldn't
         * find it among possible duplicate key entries
         * anyway, so the above test doesn't hurt.
         *
         * We sort the masks for a duplicated key the same way as
         * in a masklist -- most specific to least specific.
         * This may require the unfortunate nuisance of relocating
         * the head of the list.
         *
         * We also reverse, or doubly link the list through the
         * parent pointer.
         */
        if (tt == saved_tt) {
            struct  radix_node *xx = x;
            /* link in at head of list */
            (tt = treenodes)->rn_dupedkey = t;
            tt->rn_flags = t->rn_flags;
            tt->rn_parent = x = t->rn_parent;
            t->rn_parent = tt;          /* parent */
            if (x->rn_left == t)
                x->rn_left = tt;
            else
                x->rn_right = tt;
            saved_tt = tt; x = xx;
        } else {
            (tt = treenodes)->rn_dupedkey = t->rn_dupedkey;
            t->rn_dupedkey = tt;
            tt->rn_parent = t;          /* parent */
            if (tt->rn_dupedkey)            /* parent */
                tt->rn_dupedkey->rn_parent = tt; /* parent */
        }
#ifdef RN_DEBUG
        t=tt+1; tt->rn_info = rn_nodenum++; t->rn_info = rn_nodenum++;
        tt->rn_twin = t; tt->rn_ybro = rn_clist; rn_clist = tt;
#endif
        tt->rn_key = (caddr_t) v;
        tt->rn_bit = -1;
        tt->rn_flags = RNF_ACTIVE;
    }
    /*
     * Put mask in tree.
     */
    if (netmask) {
        tt->rn_mask = netmask;
        tt->rn_bit = x->rn_bit;
        tt->rn_flags |= x->rn_flags & RNF_NORMAL;
    }
    t = saved_tt->rn_parent;
    if (keyduplicated)
        goto on2;
    b_leaf = -1 - t->rn_bit;
    if (t->rn_right == saved_tt)
        x = t->rn_left;
    else
        x = t->rn_right;
    /* Promote general routes from below */
    if (x->rn_bit < 0) {
        for (mp = &t->rn_mklist; x; x = x->rn_dupedkey)
        if (x->rn_mask && (x->rn_bit >= b_leaf) && x->rn_mklist == 0) {
            *mp = m = rn_new_radix_mask(head, x, 0);
            if (m)
                mp = &m->rm_mklist;
        }
    } else if (x->rn_mklist) {
        /*
         * Skip over masks whose index is > that of new node
         */
        for (mp = &x->rn_mklist; (m = *mp); mp = &m->rm_mklist)
            if (m->rm_bit >= b_leaf)
                break;
        t->rn_mklist = m; *mp = 0;
    }
on2:
    /* Add new route to highest possible ancestor's list */
    if ((netmask == 0) || (b > t->rn_bit ))
        return tt; /* can't lift at all */
    b_leaf = tt->rn_bit;
    do {
        x = t;
        t = t->rn_parent;
    } while (b <= t->rn_bit && x != top);
    /*
     * Search through routes associated with node to
     * insert new route according to index.
     * Need same criteria as when sorting dupedkeys to avoid
     * double loop on deletion.
     */
    for (mp = &x->rn_mklist; (m = *mp); mp = &m->rm_mklist) {
        if (m->rm_bit < b_leaf)
            continue;
        if (m->rm_bit > b_leaf)
            break;
        if (m->rm_flags & RNF_NORMAL) {
            mmask = m->rm_leaf->rn_mask;
            if (tt->rn_flags & RNF_NORMAL) {
                log(LOG_ERR,
                    "Non-unique normal route, mask not entered\n");
                return tt;
            }
        } else
            mmask = m->rm_mask;
        if (mmask == netmask) {
            m->rm_refs++;
            tt->rn_mklist = m;
            return tt;
        }
        if (rn_refines(netmask, mmask)
            || rn_lexobetter(netmask, mmask))
            break;
    }
    *mp = rn_new_radix_mask(head, tt, *mp);
    return tt;
}

/*********************************************************************
* @purpose  delete a route from the radix tree
*
* @param    head              pointer to the tree head
* @param    v_arg             pointer to the key
* @param    netmask_arg       pointer to the mask
*
* @returns  radix_node *      pointer to the deleted node
*
* @notes    This function does not delete mask annotations that are
*           no longer ref'd from the tree!
*
* @end
*********************************************************************/
struct radix_node *
rn_delete(v_arg, netmask_arg, head)
    void *v_arg, *netmask_arg;
    struct radix_node_head *head;
{
    register struct radix_node *t, *p, *x, *tt;
    struct radix_mask *m, *saved_m, **mp;
    struct radix_node *dupedkey, *saved_tt, *top;
    caddr_t v, netmask;
    int b, head_off, vlen;

    v = v_arg;
    netmask = netmask_arg;
    x = head->rnh_treetop;
    tt = rn_search(v, x);
    head_off = x->rn_offset;
    vlen =  *(u_char *)v;
    saved_tt = tt;
    top = x;
    if (tt == 0 ||
        Bcmp(v + head_off, tt->rn_key + head_off, vlen - head_off))
        return (0);
    /*
     * Delete our route from mask lists.
     */
    if (netmask) {
        if ((x = rn_addmask(head, netmask, 1, head_off)) == 0)
            return (0);
        netmask = x->rn_key;
        while (tt->rn_mask != netmask)
            if ((tt = tt->rn_dupedkey) == 0)
                return (0);
    }
    if (tt->rn_mask == 0 || (saved_m = m = tt->rn_mklist) == 0)
        goto on1;
    if (tt->rn_flags & RNF_NORMAL) {
        if (m->rm_leaf != tt || m->rm_refs > 0) {
            log(LOG_ERR, "rn_delete: inconsistent annotation\n");
            return 0;  /* dangling ref could cause disaster */
        }
    } else {
        if (m->rm_mask != tt->rn_mask) {
            log(LOG_ERR, "rn_delete: inconsistent annotation\n");
            goto on1;
        }
        if (--m->rm_refs >= 0)
            goto on1;
    }
    b = -1 - tt->rn_bit;
    t = saved_tt->rn_parent;
    if (b > t->rn_bit)
        goto on1; /* Wasn't lifted at all */
    do {
        x = t;
        t = t->rn_parent;
    } while (b <= t->rn_bit && x != top);
    for (mp = &x->rn_mklist; (m = *mp); mp = &m->rm_mklist)
        if (m == saved_m) {
            *mp = m->rm_mklist;

      /* return to free pool */
      (m)->rm_mklist = head->rn_mkfreelist;
      head->rn_mkfreelist = (m);

            break;
        }
    if (m == 0) {
        log(LOG_ERR, "rn_delete: couldn't find our annotation\n");
        if (tt->rn_flags & RNF_NORMAL)
            return (0); /* Dangling ref to us */
    }
on1:
    /*
     * Eliminate us from tree
     */
    if (tt->rn_flags & RNF_ROOT)
        return (0);
#ifdef RN_DEBUG
    /* Get us out of the creation list */
    for (t = rn_clist; t && t->rn_ybro != tt; t = t->rn_ybro) {}
    if (t) t->rn_ybro = tt->rn_ybro;
#endif
    t = tt->rn_parent;
    dupedkey = saved_tt->rn_dupedkey;
    if (dupedkey) {
        /*
         * Here, tt is the deletion target and
         * saved_tt is the head of the dupekey chain.
         */
        if (tt == saved_tt) {
            /* remove from head of chain */
            x = dupedkey; x->rn_parent = t;
            if (t->rn_left == tt)
                t->rn_left = x;
            else
                t->rn_right = x;
        } else {
            /* find node in front of tt on the chain */
            for (x = p = saved_tt; p && p->rn_dupedkey != tt;)
                p = p->rn_dupedkey;
            if (p) {
                p->rn_dupedkey = tt->rn_dupedkey;
                if (tt->rn_dupedkey)        /* parent */
                    tt->rn_dupedkey->rn_parent = p;
                                /* parent */
            } else log(LOG_ERR, "rn_delete: couldn't find us\n");
        }
        t = tt + 1;
        if  (t->rn_flags & RNF_ACTIVE) {
#ifndef RN_DEBUG
            *++x = *t;
            p = t->rn_parent;
#else
            b = t->rn_info;
            *++x = *t;
            t->rn_info = b;
            p = t->rn_parent;
#endif
            if (p->rn_left == t)
                p->rn_left = x;
            else
                p->rn_right = x;
            x->rn_left->rn_parent = x;
            x->rn_right->rn_parent = x;
        }
        goto out;
    }
    if (t->rn_left == tt)
        x = t->rn_right;
    else
        x = t->rn_left;
    p = t->rn_parent;
    if (p->rn_right == t)
        p->rn_right = x;
    else
        p->rn_left = x;
    x->rn_parent = p;
    /*
     * Demote routes attached to us.
     */
    if (t->rn_mklist) {
        if (x->rn_bit >= 0) {
            for (mp = &x->rn_mklist; (m = *mp);)
                mp = &m->rm_mklist;
            *mp = t->rn_mklist;
        } else {
            /* If there are any key,mask pairs in a sibling
               duped-key chain, some subset will appear sorted
               in the same order attached to our mklist */
            for (m = t->rn_mklist; m && x; x = x->rn_dupedkey)
                if (m == x->rn_mklist) {
                    struct radix_mask *mm = m->rm_mklist;
                    x->rn_mklist = 0;
                    if (--(m->rm_refs) < 0)
          {
            /* return to free pool */
            (m)->rm_mklist = head->rn_mkfreelist;
            head->rn_mkfreelist = (m);
          }

                    m = mm;
                }
            if (m)
                log(LOG_ERR,
                    "rn_delete: Orphaned Mask %p at %p\n",
                    (void *)m, (void *)x);
        }
    }
    /*
     * We may be holding an active internal node in the tree.
     */
    x = tt + 1;
    if (t != x) {
#ifndef RN_DEBUG
        *t = *x;
#else
        b = t->rn_info;
        *t = *x;
        t->rn_info = b;
#endif
        t->rn_left->rn_parent = t;
        t->rn_right->rn_parent = t;
        p = x->rn_parent;
        if (p->rn_left == x)
            p->rn_left = t;
        else
            p->rn_right = t;
    }
out:
    tt->rn_flags &= ~RNF_ACTIVE;
    tt[1].rn_flags &= ~RNF_ACTIVE;
    return (tt);
}

#if 0  /* LVL7 NOT USED */
/*
 * This is the same as rn_walktree() except for the parameters and the
 * exit.
 */
static int
rn_walktree_from(h, a, m, f, w)
    struct radix_node_head *h;
    void *a, *m;
    walktree_f_t *f;
    void *w;
{
    int error;
    struct radix_node *base, *next;
    u_char *xa = (u_char *)a;
    u_char *xm = (u_char *)m;
    register struct radix_node *rn, *last = 0 /* shut up gcc */;
    int stopping = 0;
    int lastb;

    /*
     * rn_search_m is sort-of-open-coded here.
     */
    /* printf("about to search\n"); */
    for (rn = h->rnh_treetop; rn->rn_bit >= 0; ) {
        last = rn;
        /* printf("rn_bit %d, rn_bmask %x, xm[rn_offset] %x\n",
               rn->rn_bit, rn->rn_bmask, xm[rn->rn_offset]); */
        if (!(rn->rn_bmask & xm[rn->rn_offset])) {
            break;
        }
        if (rn->rn_bmask & xa[rn->rn_offset]) {
            rn = rn->rn_right;
        } else {
            rn = rn->rn_left;
        }
    }
    /* printf("done searching\n"); */

    /*
     * Two cases: either we stepped off the end of our mask,
     * in which case last == rn, or we reached a leaf, in which
     * case we want to start from the last node we looked at.
     * Either way, last is the node we want to start from.
     */
    rn = last;
    lastb = rn->rn_bit;

    /* printf("rn %p, lastb %d\n", rn, lastb);*/

    /*
     * This gets complicated because we may delete the node
     * while applying the function f to it, so we need to calculate
     * the successor node in advance.
     */
    while (rn->rn_bit >= 0)
        rn = rn->rn_left;

    while (!stopping) {
        /* printf("node %p (%d)\n", rn, rn->rn_bit); */
        base = rn;
        /* If at right child go back up, otherwise, go right */
        while (rn->rn_parent->rn_right == rn
               && !(rn->rn_flags & RNF_ROOT)) {
            rn = rn->rn_parent;

            /* if went up beyond last, stop */
            if (rn->rn_bit < lastb) {
                stopping = 1;
                /* printf("up too far\n"); */
            }
        }

        /* Find the next *leaf* since next node might vanish, too */
        for (rn = rn->rn_parent->rn_right; rn->rn_bit >= 0;)
            rn = rn->rn_left;
        next = rn;
        /* Process leaves */
        while ((rn = base) != 0) {
            base = rn->rn_dupedkey;
            /* printf("leaf %p\n", rn); */
            if (!(rn->rn_flags & RNF_ROOT)
                && (error = (*f)(rn, w)))
                return (error);
        }
        rn = next;

        if (rn->rn_flags & RNF_ROOT) {
            /* printf("root, stopping"); */
            stopping = 1;
        }

    }
    return 0;
}
#endif /* LVL7 NOT USED */

/*********************************************************************
* @purpose  walk the radix tree
*
* @param    head              pointer to the tree head
* @param    f                 fnptr called for each leaf
* @param    w                 data passed to the callback
*
* @returns
*
* @notes
*
* @end
*********************************************************************/
int
rn_walktree(h, f, w)
    struct radix_node_head *h;
    walktree_f_t *f;
    void *w;
{
    int error;
    struct radix_node *base, *next;
    register struct radix_node *rn = h->rnh_treetop;
    /*
     * This gets complicated because we may delete the node
     * while applying the function f to it, so we need to calculate
     * the successor node in advance.
     */
    /* First time through node, go left */
    while (rn->rn_bit >= 0)
        rn = rn->rn_left;
    for (;;) {
        base = rn;
        /* If at right child go back up, otherwise, go right */
        while (rn->rn_parent->rn_right == rn
               && (rn->rn_flags & RNF_ROOT) == 0)
            rn = rn->rn_parent;
        /* Find the next *leaf* since next node might vanish, too */
        for (rn = rn->rn_parent->rn_right; rn->rn_bit >= 0;)
            rn = rn->rn_left;
        next = rn;
        /* Process leaves */
        while ((rn = base)) {
            base = rn->rn_dupedkey;
            if (!(rn->rn_flags & RNF_ROOT)
                && (error = (*f)(rn, w)))
                return (error);
        }
        rn = next;
        if (rn->rn_flags & RNF_ROOT)
            return (0);
    }
    /* NOTREACHED */
}

/*********************************************************************
* @purpose  get the next leaf in the tree
*
* @param    rn                pointer to the current node
*
* @returns  radix_node *      next node, or null
*                             (see notes: this could be a mask node)
*
* @notes
*           - Pass the head top to start iterating
*
*           - Get next node returns mask nodes as well, so check
*             check that rn_mask and rn_key are valid pointers
*
* @end
*********************************************************************/
void *
rn_getnext(rn)
        struct radix_node *rn;
{
        /* First time through node, go left */
        while (rn->rn_bit >= 0)
                rn = rn->rn_left;

        /* Process leaves */
        if (rn->rn_dupedkey) {
                rn = rn->rn_dupedkey;
                if (!(rn->rn_flags & RNF_ROOT))
                        return(rn);
                else
                        return ((struct radix_node *)0);
                }
        else {
                /* If at right child go back up, otherwise, go right */
                while (rn &&
                       (rn->rn_parent->rn_right == rn) &&
                       ((rn->rn_flags & RNF_ROOT) == 0))
                        rn = rn->rn_parent;
                /* Find the next *leaf* since next node might vanish, too */
                if(rn)
                {
                  for (rn = rn->rn_parent->rn_right; rn && rn->rn_bit >= 0;)
                    rn = rn->rn_left;
                }

                if (!rn || rn->rn_flags & RNF_ROOT)
                        return (0);
                else
                        return(rn);
        }

        return(0);
}


/*********************************************************************
* @purpose  Initialize the radix tree head
*
* @param    head              pointer to the tree head
* @param    off               initial offset
*
* @returns
*
* @notes    The current implementation of the radix tree does not
*           support an "offset". The offset is hardcoded to 0. This
*           means that the key length and pad bytes (if any) will
*           be used while constructing the tree and for lookups.
*
*           Stevens TCP/IP vol 2. describes the BSD radix implementation
*           and the offset. However the code doesnt seem to use the offset
*           consistently. Some functions compare the entire key, others
*           use the offset before comparing. This particularly affects
*           mask nodes as rn_addmask changes the length byte by trimming
*           trailing zeros. rn_addroute detects the difference however
*           rn_insert misses it (if the offset is set). Not entirely sure
*           why this is broken, but a non zero offset didnt work...
*           ...while a zero offset works perfectly!
*
* @end
*********************************************************************/
int
rn_inithead(rnh)
    struct radix_node_head *rnh;
{
  register struct radix_node *t, *tt, *ttt;
  char * rn_ones = rnh->rn_ones;
  int off = 0;

    t = rn_newpair(rnh->rn_zeros, off, rnh->rnh_nodes);

    ttt = rnh->rnh_nodes + 2;
    t->rn_right = ttt;
    t->rn_parent = t;
    tt = t->rn_left;
    tt->rn_flags = t->rn_flags = RNF_ROOT | RNF_ACTIVE;
    tt->rn_bit = -1 - off;
    *ttt = *tt;
    ttt->rn_key = rn_ones;
    rnh->rnh_treetop = t;
    return (1);
}

void dumpKey(L7_char8 *s, L7_uchar8 * ptr)
{
  L7_uint32 maxkeylen = 8;  /* hardcoded!! */

  printf(",(%s: ", s);

  if(!ptr)
  {
    printf("NULL)");
    return;
  }

  while(maxkeylen)
  {
    printf("%x ", *ptr);

    maxkeylen--;
    ptr++;
  }
  printf(") ");
}

/*********************************************************************
* @purpose  Dump the tree to the console
*
* @param    head              pointer to the tree head
*
* @returns
*
* @notes    The output is formatted as follows:
*
* (for internal nodes: type N)
* type  self  mklist  parent  bit  bmask  flag  N:offset  N:left  N:right
*
* (for leaf nodes: type L)
* type  self  mklist  parent  bit  bmask  flag  L:key     L:mask  L:dupdkey  (key value)  (mask value)
*
* (for mask list items: type m)
* type  self  m:next   -      bit  unused flag  m:mask    -       m:refs     (mask value)  -
*
* @end
*********************************************************************/
void dumptree(struct l7_radix_node *x)
{
  if(!x)
    return;

  /* internal node? */
  if(x->rn_bit >= 0)
  {
    /* recurse for left node */
    dumptree(x->rn_u.rn_node.rn_L);

    /* recurse for right node */
    dumptree(x->rn_u.rn_node.rn_R);
  }
  else
  {
    struct l7_radix_node *d = x->rn_u.rn_leaf.rn_Dupedkey;
    /* walk duped keys */
    while(d)
    {
      dumptree(d);
      d = d->rn_u.rn_leaf.rn_Dupedkey;
    }
  }

  if(x->rn_bit >= 0)
  {
    /* print intenal node */
    printf("\nN,0x%x,0x%x,0x%x,%d,0x%x,%d,%d,0x%x,0x%x",
           (L7_uint32)x,
           (L7_uint32)x->rn_mklist, (L7_uint32)x->rn_parent,
           x->rn_bit, x->rn_bmask, x->rn_flags,
           x->rn_u.rn_node.rn_Off,
           (L7_uint32)x->rn_u.rn_node.rn_L,
           (L7_uint32)x->rn_u.rn_node.rn_R);
  }
  else
  {
    /* print leaf */
    /* print intenal node */
    printf("\nL,0x%x,0x%x,0x%x,%d,0x%x,%d,0x%x,0x%x,0x%x",
           (L7_uint32)x,
           (L7_uint32)x->rn_mklist, (L7_uint32)x->rn_parent,
           x->rn_bit, x->rn_bmask, x->rn_flags,
           (L7_uint32)x->rn_u.rn_leaf.rn_Key,
           (L7_uint32)x->rn_u.rn_leaf.rn_Mask,
           (L7_uint32)x->rn_u.rn_leaf.rn_Dupedkey);

    dumpKey("K", x->rn_u.rn_leaf.rn_Key);
    dumpKey("M", x->rn_u.rn_leaf.rn_Mask);
  }

  if(x->rn_mklist)
  {
    struct  l7_radix_mask * mk_node = x->rn_mklist;

    while(mk_node)
    {
      printf("\nm,0x%x,0x%x,,%d,%d,%d,0x%x,,%d",
             (L7_uint32)mk_node,
             (L7_uint32)mk_node->rm_mklist,
             mk_node->rm_bit,
             mk_node->rm_unused,
             mk_node->rm_flags,
             (L7_uint32)mk_node->rm_rmu.rmu_mask,
             mk_node->rm_refs);
      if ((mk_node->rm_flags & RNF_NORMAL) == 0)
        dumpKey("m", mk_node->rm_rmu.rmu_mask);
      else
        printf("  rmu_leaf:  %#x", (L7_uint32)mk_node->rm_rmu.rmu_leaf);

      mk_node = mk_node->rm_mklist;
    }
  }

  return;
}


/*********************************************************************
* @purpose  walk the radix tree: internal nodes & leafs
*
* @param    head              pointer to the tree head
* @param    f                 fnptr called for each leaf
* @param    w                 data passed to the callback
*
* @returns
*
* @notes
*
* @end
*********************************************************************/
int
rn_walktree_all(h,do_pr)
    struct radix_node_head *h;
        int do_pr;
{
    struct radix_node *next,*save;
    register struct radix_node *rn = h->rnh_treetop;
        char buf[64];
        int i,m_bit;
    register struct radix_mask *m;

        if(do_pr){
          printf("Nodes:\n");
          printf("type   bit    \n");
        }
    /*
     * This gets complicated because we may delete the node
     * while applying the function f to it, so we need to calculate
     * the successor node in advance.
     */
    /* First time through node, go left */
    while (rn->rn_bit >= 0)
        rn = rn->rn_left;
    for (;;) {
            if(rn->rn_bit < 0 )
            {
        /* Process leaves */
                save = rn;
                next = rn;
        while ((rn = next) != 0) {
            next = rn->rn_dupedkey;
            if (rn->rn_flags & RNF_ROOT){
                             if(do_pr)printf("root in leaf???\n");
                        }
                        for(i = 0; i < h->max_keylen;i++)
                        {
                            sprintf(&buf[2*i],"%02x",*((char *)rn->rn_key+i));
                        }
                        buf[2*i] = 0;
                        if(do_pr)printf("L%x      %s    \n",(int)rn,buf);
        }
        rn = save;

                /* this node is leaf, grab next int node for which we are on left */
        /* If at right child go back up, otherwise, go right */
        while (rn->rn_parent->rn_right == rn
               && (((rn->rn_flags & RNF_ROOT) == 0) || (rn->rn_bit < 0)))
                {
            rn = rn->rn_parent;
                }
        if ((rn->rn_bit >= 0) && (rn->rn_flags & RNF_ROOT))
            return (0);
                rn = rn->rn_parent;

            }
            else{
                /* process internal node */
        if (rn->rn_flags & RNF_ROOT)
                {
                    if(do_pr)printf("R%x      %3d    l= %x r= %x\n",(int)rn,rn->rn_bit,(int)rn->rn_left, (int)rn->rn_right);
                }
                else
                {
                    if(do_pr)printf("I%x      %3d    l= %x r= %x\n",(int)rn,rn->rn_bit,(int)rn->rn_left, (int)rn->rn_right);
                }
                /* masks */
        m = rn->rn_mklist;
                while(m)
                {
                    m_bit = -1 - m->rm_bit;
                    if((m_bit < 0) || (m_bit > (h->max_keylen*8)))
                    {
                        printf("invalid mask len\n");
                    }
            if ((m_bit != 0) && !(m->rm_flags & RNF_NORMAL)) {
                        printf("non-normal mask \n");
                    }
            m = m->rm_mklist; /* next mask */
                }

                /* next leaf node */
                /* find furthest left leaf off the right side */
        for (rn = rn->rn_right; rn->rn_bit >= 0;)
            rn = rn->rn_left;

            }
    }
    /* NOTREACHED */
}



/*********************************************************************
* @purpose  perform rn_search and return the radix node and count of
*           matching bits.
*
* @param    head              pointer to the tree head
* @param    v_arg             pointer to a key with 1st byte=len
* @param    rn_bits           required number of matching bits
*
* @returns  radix_node *      pointer to the node if found
* @returns  null              if not found
*
* @notes    The entry returned is the one with the longest prefix match,
*           but masks are not considered.
*
*
* @end
*********************************************************************/
struct radix_node *
rn_match_count(v_arg, head, rn_bits)
    void *v_arg;
    struct radix_node_head *head;
    int rn_bits;
{
    caddr_t v = v_arg;
    register struct radix_node *t = head->rnh_treetop;
    register caddr_t cp = v, cp2;
    caddr_t cplim;
    int off = t->rn_offset, vlen = *(u_char *)cp, matched_off;
    register int test, b;

    /*
     * Open code rn_search(v, top) to avoid overhead of extra
     * subroutine call.
     */
    for (; t->rn_bit >= 0; ) {
        if (t->rn_bmask & cp[t->rn_offset])
            t = t->rn_right;
        else
            t = t->rn_left;
    }
    cp += off; cp2 = t->rn_key + off; cplim = v + vlen;
    for (; cp < cplim; cp++, cp2++)
        if (*cp != *cp2) /* bytes dont match? go on(1) to locate the offending bit */
            goto on1;

        /* exact match */
    /*
     * This extra grot is in case we are explicitly asked
     * to look up the default.  Ugh!
     *
     * Never return the root node itself, it seems to cause a
     * lot of confusion.
     */
    if (t->rn_flags & RNF_ROOT)
        t = t->rn_dupedkey;
        return t;

on1:
    /* The search tested the bit specified by rn_bmask (rn_bit)
     * so some other  bit may also vary before it!
     * So find first bit that differs!
     * also test is an int so & with 0xff to ensure all leading bits are zero */
    test = (*cp ^ *cp2) & 0xff;
    for (b = 7; (test >>= 1) > 0;)
        b--;
    matched_off = cp - v;
    b += matched_off << 3; /* add bits for the bytes skipped */

        if(b >= rn_bits)
            return t;
        else
            return L7_NULL;

}
