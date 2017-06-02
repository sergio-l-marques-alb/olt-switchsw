/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   l7_radix_defs.h
*
* @purpose    File contains includes for defs required for radix 
*
* @component  system_support
*
* @comments
*
* @create     4/26/2004
*
* @author     rkelkar
* @end
*
**********************************************************************/

#ifndef _L7_RADIX_DEFS_
#define _L7_RADIX_DEFS_

#include "log.h"	

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

/* global data name remapping */
#define radix_node      l7_radix_node
#define radix_mask      l7_radix_mask
#define radix_node_head l7_radix_node_head
#define rn_mkfreelist   l7_rn_mkfreelist

/* global function name remapping */
#define rn_search       l7_rn_search
#define rn_search_m     l7_rn_search_m
#define rn_refines      l7_rn_refines
#define rn_lookup       l7_rn_lookup
#define rn_satisfies_leaf  l7_rn_satisfies_leaf
#define rn_match        l7_rn_match
#define rn_newpair      l7_rn_newpair
#define rn_insert       l7_rn_insert
#define rn_addmask      l7_rn_addmask
#define rn_lexobetter   l7_rn_lexobetter
#define rn_new_radix_mask  l7_rn_new_radix_mask
#define rn_addroute     l7_rn_addroute
#define rn_delete       l7_rn_delete
#define rn_walktree     l7_rn_walktree
#define rn_walktree_from     l7_rn_walktree_from
#define rn_inithead     l7_rn_inithead
#define rn_init         l7_rn_init

#endif /* _L7_RADIX_DEFS_ */
