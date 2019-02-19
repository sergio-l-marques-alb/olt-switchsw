/* $Id: sand_sorted_list.c,v 1.8 Broadcom SDK $
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

#include <sal/appl/sal.h>
#include <sal/core/alloc.h>
#include <soc/dpp/SAND/Utils/sand_rhlist.h>

rhlist_t *rhlist_create(char *name, int entry_size, int sanity)
{
    rhlist_t *rhlist;

    rhlist = sal_alloc(sizeof(rhlist_t), "sl");
    if(rhlist) {
        memset(rhlist, 0, sizeof(rhlist_t));
        if(name != NULL) {
            if(strlen(name) >= RHNAME_MAX_SIZE) {
                name[RHNAME_MAX_SIZE - 1] = 0;
            }
            strcpy(RHNAME(rhlist), name);
        }
        rhlist->entry_size = entry_size;
        rhlist->sanity = sanity;
    }

    return rhlist;
}

/************************************************************************************************
 * Only allocate the entry for specific list, but do not modify the list itself, besides max_id *
 ************************************************************************************************/
rhstatus_t rhlist_alloc_entry(rhlist_t *rhlist, char *name, int id, rhhandle_t *rhentry_p)
{
    rhstatus_t ret = SOC_SAND_OK;
    rhentry_t *entry;

    if(rhlist == NULL) {
        ret = -SOC_SAND_ERR;
        goto out;
    }

    /* Check whether the name exists */
    if(rhlist->sanity && name) {
        if(rhlist_name_exists(rhlist, name, rhentry_p) == SOC_SAND_OK) {
            ret = -SOC_SAND_ERR;
            goto out;
        }
        else if(strlen(name) >= RHNAME_MAX_SIZE) {
            ret = -SOC_SAND_ERR;
            goto out;
        }
    }

    /* Assign the next number after maximum id */
    if(id == RHID_TO_BE_GENERATED)
        id = ++rhlist->max_id;

    if(id < 0) {
        ret = -SOC_SAND_ERR;
        goto out;
    }

    /* Check whether the id exists */
    if(rhlist->sanity && rhlist_id_exists(rhlist, id) == SOC_SAND_OK) {
        ret = -SOC_SAND_ERR;
        goto out;
    }

    if(!(entry = sal_alloc(rhlist->entry_size, "sl"))) {
        ret = -SOC_SAND_MALLOC_FAIL;
        goto out;
    }

    sal_memset(entry, 0, rhlist->entry_size);

    entry->id = id;

    if(name)
        strcpy(RHNAME(entry), name);

    *rhentry_p = entry;

out:
    return ret;
}

rhstatus_t rhlist_add_top(rhlist_t *rhlist, char *name, int id, rhhandle_t *rhentry_p)
{
    rhstatus_t ret = SOC_SAND_OK;
    rhentry_t *entry;

    if((ret = rhlist_alloc_entry(rhlist, name, id, rhentry_p)) != SOC_SAND_OK)
        goto out;

    entry = *rhentry_p;

    entry->next = rhlist->top;
    if(rhlist->top)
        rhlist->top->prev = entry;
    else
        rhlist->tail = entry;

    rhlist->top = entry;

    rhlist->num++;

    if(id > rhlist->max_id)
        rhlist->max_id = id;

out:
    return ret;
}

rhstatus_t rhlist_add_tail(rhlist_t *rhlist, char *name, int id, rhhandle_t *rhentry_p)
{
    rhstatus_t ret = SOC_SAND_OK;
    rhentry_t *entry;

    if(rhentry_p == NULL) {
        ret = -SOC_SAND_ERR;
        goto out;
    }

    if((ret = rhlist_alloc_entry(rhlist, name, id, rhentry_p)) != SOC_SAND_OK) {
        goto out;
    }

    entry = *rhentry_p;

    entry->prev = rhlist->tail;
    if(rhlist->tail)
        rhlist->tail->next = entry;
    else
        rhlist->top = entry;

    rhlist->tail = entry;

    rhlist->num++;

    if(id > rhlist->max_id)
        rhlist->max_id = id;

out:
    return ret;
}

rhstatus_t rhlist_insert_before(rhlist_t *rhlist, rhhandle_t rhentry, rhhandle_t rhentry_next)
{
    rhstatus_t ret = SOC_SAND_OK;
    rhentry_t *entry = (rhentry_t *)rhentry, *entry_next = (rhentry_t *)rhentry_next;

    /* actual list management */
    if(entry_next == NULL) {
        /* NULL entry_next means we are at the end of list, so we add_tail it */
        entry->prev = rhlist->tail;
        if(rhlist->tail)
            rhlist->tail->next = entry;
        else
            rhlist->top = entry;

        rhlist->tail = entry;
    } else if(entry_next->prev == NULL) {
        /* NULL prev means we are inserting it at top, so add_top it */
        entry->next = rhlist->top;
        if(rhlist->top)
            rhlist->top->prev = entry;
        else
            rhlist->tail = entry;

        rhlist->top = entry;
    } else {
        /* Insert it in the middle */
        entry->prev = entry_next->prev;
        entry_next->prev = entry;
        entry->prev->next = entry;
        entry->next = entry_next;
    }
    /* end of list management */
    rhlist->num++;

    if(entry->id > rhlist->max_id)
        rhlist->max_id = entry->id;

    return ret;
}

rhstatus_t rhlist_append(rhlist_t *rhlist, rhhandle_t entry_h)
{
    rhstatus_t ret = SOC_SAND_OK;
    rhentry_t *entry = entry_h;

    if(rhlist == NULL) {
        ret = -SOC_SAND_ERR;
        goto out;
    }

    /* Check whether the name exists */
    if(rhlist->sanity && (rhlist_name_exists(rhlist, entry->name, NULL) == SOC_SAND_OK)) {
        ret = -SOC_SAND_ERR;
        goto out;
    }

    /* Check whether the ID exists */
    if(rhlist->sanity && (rhlist_id_exists(rhlist, entry->id) == SOC_SAND_OK)) {
        ret = -SOC_SAND_ERR;
        goto out;
    }

    entry->prev = rhlist->tail;
    if(rhlist->tail)
        rhlist->tail->next = entry;
    else
        rhlist->top = entry;

    rhlist->tail = entry;

    rhlist->num++;

out:
    return ret;
}

void rhlist_refresh_max_id(rhlist_t *rhlist)
{
    rhentry_t *entry;
    int max_id = 0;

    if(!rhlist) {
        return;
    }

    for(entry = rhlist->top; entry; entry = entry->next)
        if(max_id < entry->id)
            max_id = entry->id;

    rhlist->max_id = max_id;
    return;
}

int rhlist_generate_id(rhlist_t *rhlist)
{
    if(!rhlist) {
        return -SOC_SAND_ERR;
    }

    /* Meanwhile - just increment the id of max element */
    return ++rhlist->max_id;
}

/* delete from double linked list */
rhstatus_t rhlist_del_free(rhlist_t *rhlist, rhhandle_t entry_h)
{
    rhstatus_t ret = SOC_SAND_OK;
    rhentry_t *entry = (rhentry_t *)entry_h;

    if(!rhlist) {
        ret = -SOC_SAND_ERR;
        goto out;
    }

    if(entry == rhlist->top)
        rhlist->top = entry->next;

    if(entry == rhlist->tail)
        rhlist->tail = entry->prev;

    if(entry->next)
        entry->next->prev = entry->prev;
    if(entry->prev)
        entry->prev->next = entry->next;

    rhlist->num--;

    sal_free(entry);
out:
    return ret;
}

/* get first element from double linked list and remove it from it */
rhhandle_t rhlist_fetch_first(rhlist_t *rhlist)
{
    rhentry_t *entry;

    if(!rhlist) {
        return NULL;
    }

    entry = rhlist->top;

    rhlist->top = entry->next;

    if(entry->next)
        entry->next->prev = NULL;

    rhlist->num--;

    return entry;
}

rhhandle_t rhlist_get_entry_by_id(rhlist_t *rhlist, int id)
{
    rhentry_t *entry;

    if(!rhlist) {
        return NULL;
    }

    for(entry = rhlist->top; entry; entry = entry->next) {
        if(entry->id == id)
            return (rhhandle_t)entry;
    }

    return NULL;
}

rhstatus_t rhlist_entry_exists(rhlist_t *rhlist, rhhandle_t entry_h)
{
    rhentry_t *entry;

    if(!rhlist) {
        return -SOC_SAND_ERR;
    }

    for(entry = rhlist->top; entry; entry = entry->next) {
        if(entry == entry_h)
            return SOC_SAND_OK;
    }

    return -SOC_SAND_ERR;
}

rhhandle_t rhlist_get_entry_by_name(rhlist_t *rhlist, char *name)
{
    rhentry_t *entry;

    if(!rhlist) {
        return NULL;
    }

    for(entry = rhlist->top; entry; entry = entry->next) {
        if(!sal_strcasecmp(name, entry->name))
            return entry;
    }

    return NULL;
}

int rhlist_get_id_by_name(rhlist_t *rhlist, char *name)
{
    rhentry_t *entry;

    for(entry = rhlist->top; entry; entry = entry->next) {
        if(!strcmp(name, entry->name))
            return entry->id;
    }

    return -SOC_SAND_ERR;
}

char *rhlist_get_name_by_id(rhlist_t *rhlist, int id)
{
    rhentry_t *entry;

    for(entry = rhlist->top; entry; entry = entry->next) {
        if(id == entry->id)
            return entry->name;
    }

    return NULL;
}

rhhandle_t rhlist_get_first(rhlist_t *rhlist)
{
    if(!rhlist) {
        return NULL;
    }

    return rhlist->top;
}

rhhandle_t rhlist_get_last(rhlist_t *rhlist)
{
    if(!rhlist) {
        return NULL;
    }

    return rhlist->tail;
}

rhhandle_t rhlist_get_next(rhhandle_t entry_h)
{
    if(!entry_h) {
        return NULL;
    }

    return ((rhentry_t *)entry_h)->next;
}

rhhandle_t rhlist_get_prev(rhhandle_t entry_h)
{
    if(!entry_h) {
        return NULL;
    }

    return ((rhentry_t *)entry_h)->prev;
}

rhstatus_t rhlist_name_exists(rhlist_t *rhlist, char *name, rhhandle_t *rhentry_p)
{
    rhstatus_t ret = -SOC_SAND_ERR;
    rhentry_t *entry;

    if(!rhlist) {
        ret = -SOC_SAND_ERR;
        goto out;
    }

    for(entry = rhlist->top; entry; entry = entry->next)
        if(!strcmp(entry->name, name)) {
            ret = SOC_SAND_OK;
            if(rhentry_p != NULL)
                *rhentry_p = entry;
            goto out;
        }

out:
    return ret;
}

rhstatus_t rhlist_id_exists(rhlist_t *rhlist, int id)
{
    rhstatus_t ret = SOC_SAND_ERR;
    rhentry_t *entry;

    if(!rhlist) {
        ret = -SOC_SAND_ERR;
        goto out;
    }

    for(entry = rhlist->top; entry; entry = entry->next)
        if(entry->id == id) {
            ret = SOC_SAND_ERR;
            goto out;
        }

out:
    return ret;
}

/* free all entries and list itself */
void rhlist_free_all(rhlist_t *rhlist)
{
    rhentry_t *next_entry, *entry;

    if(!rhlist) {
        return;
    }

    entry = rhlist->top;

    while(entry) {
        next_entry = entry->next;
        sal_free(entry);
        entry = next_entry;
    }
    sal_free(rhlist);
    return;
}

/* free all entries, reset the content and leave the list */
void rhlist_clean(rhlist_t *rhlist)
{
    rhentry_t *next_entry, *entry;

    if(!rhlist) {
        return;
    }

    entry = rhlist->top;

    while(entry) {
        next_entry = entry->next;
        sal_free(entry);
        entry = next_entry;
    }

    rhlist->top = NULL;
    rhlist->tail = NULL;
    rhlist->num = 0;
    rhlist->max_id = 0;

    return;
}
