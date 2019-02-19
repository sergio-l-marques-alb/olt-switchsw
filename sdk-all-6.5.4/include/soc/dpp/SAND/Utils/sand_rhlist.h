/* $Id: sand_rhlist.h,v 1.0 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/
#ifndef __SOC_SAND_RHLIST_H_INCLUDED__
#define __SOC_SAND_RHLIST_H_INCLUDED__

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_dataio_utils.h>

/* Use this define if you dont want to bother with ID management */
#define    RHID_TO_BE_GENERATED    -1

typedef void *rhhandle_t;
typedef int rhstatus_t;
typedef int rhid_t;
typedef int rhlength_t;

typedef struct _rhentry {
    struct _rhentry *next;
    struct _rhentry *prev;
    rhid_t           id;
    char             name[RHNAME_MAX_SIZE];
    int              users;
    int              mode;
} rhentry_t;

typedef struct _rhlist {
    rhentry_t     entry;

    rhentry_t    *top;
    rhentry_t    *tail;
    int            num;
    int            max_id;
    int            entry_size;
    int            sanity;
} rhlist_t;

#define RHID(entry_h)           ((rhentry_t *)entry_h)->id
#define RHNAME(entry_h)         ((rhentry_t *)entry_h)->name
#define RHUSERS(entry_h)        ((rhentry_t *)entry_h)->users
#define RHMODE(entry_h)         ((rhentry_t *)entry_h)->mode
#define RHLNUM(rhlist)          rhlist->num
#define RHUSER_ADD(entry_h)     (((rhentry_t *)entry_h)->users++)
#define RHUSER_DEL(entry_h)     (((rhentry_t *)entry_h)->users--)

rhlist_t*     rhlist_create(char *name, int entry_size, int sanity);
rhstatus_t    rhlist_alloc_entry(rhlist_t *rhlist, char *name, int id, rhhandle_t *rhentry_p);
rhstatus_t    rhlist_add_top(rhlist_t *rhlist, char *name, int id, rhhandle_t *rhentry_p);
rhstatus_t    rhlist_add_tail(rhlist_t *rhlist, char *name, int id, rhhandle_t *rhentry_p);
rhstatus_t    rhlist_insert_before(rhlist_t *rhlist, rhhandle_t rhentry, rhhandle_t rhentry_next);
rhstatus_t    rhlist_del_free(rhlist_t *rhlist, rhhandle_t entry);
rhhandle_t    rhlist_get_entry_by_id(rhlist_t *rhlist, int id);
rhstatus_t    rhlist_entry_exists(rhlist_t *rhlist, rhhandle_t entry_h);
rhhandle_t    rhlist_get_entry_by_name(rhlist_t *rhlist, char *name);
int           rhlist_get_id_by_name(rhlist_t *rhlist, char *name);
char*         rhlist_get_name_by_id(rhlist_t *rhlist, int id);
rhhandle_t    rhlist_get_first(rhlist_t *rhlist);
rhhandle_t    rhlist_get_next(rhhandle_t entry);
rhhandle_t    rhlist_get_last(rhlist_t *rhlist);
rhhandle_t    rhlist_get_prev(rhhandle_t entry);
void          rhlist_free_all(rhlist_t *rhlist);
void          rhlist_clean(rhlist_t *rhlist);
int           rhlist_generate_id(rhlist_t *rhlist);
void          rhlist_refresh_max_id(rhlist_t *rhlist);
rhstatus_t    rhlist_name_exists(rhlist_t *rhlist, char *name, rhhandle_t *rhentry_p);
rhstatus_t    rhlist_id_exists(rhlist_t *rhlist, int id);
rhhandle_t    rhlist_fetch_first(rhlist_t *rhlist);
rhstatus_t    rhlist_append(rhlist_t *rhlist, rhhandle_t entry_h);

#define RHITERATOR(rhentry_h, rhlist)                \
        for(rhentry_h = rhlist_get_first(rhlist); rhentry_h; rhentry_h = rhlist_get_next(rhentry_h))

#define RHBACK_ITERATOR(rhentry_h, rhlist)                \
        for(rhentry_h = rhlist_get_last(rhlist); rhentry_h; rhentry_h = rhlist_get_prev(rhentry_h))

#define RHSAFE_ITERATOR(rhentry_h, rhlist)                                                \
        rhhandle_t rhentry_next_h;                                                            \
        for(rhentry_h = rhlist_get_first(rhlist), rhentry_next_h = rhlist_get_next(rhentry_h);    \
                                rhentry_h; rhentry_h = rhentry_next_h, rhentry_next_h = rhlist_get_next(rhentry_h))

#define RHCOND_ITERATOR(rhentry_h, rhlist)                                                            \
        rhhandle_t rhentry_next_h = NULL;                                                             \
        if(rhentry_h == NULL) {                                                                       \
            rhentry_h = rhlist_get_first(rhlist);                                                     \
            rhentry_next_h = rhlist_get_next(rhentry_h);                                              \
        }                                                                                             \
        for(;rhentry_h; rhentry_h = rhentry_next_h, rhentry_next_h = rhlist_get_next(rhentry_h))

#endif /* __SOC_SAND_RHLIST_H_INCLUDED__ */
