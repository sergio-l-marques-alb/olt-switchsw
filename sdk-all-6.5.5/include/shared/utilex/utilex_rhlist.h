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
#ifndef UTILEX_RHLIST_H_INCLUDED
#define UTILEX_RHLIST_H_INCLUDED

#include <shared/utilex/utilex_str.h>
#include <shared/shrextend/shrextend_error.h>

/* Use this define if you dont want to bother with ID management */
#define    RHID_TO_BE_GENERATED    -1

typedef void *rhhandle_t;
typedef int rhid_t;

typedef struct _rhentry
{
  struct _rhentry *next;
  struct _rhentry *prev;
  rhid_t id;
  char name[RHNAME_MAX_SIZE];
  int users;
  int mode;
} rhentry_t;

typedef struct _rhlist
{
  rhentry_t entry;
  rhentry_t *top;
  rhentry_t *tail;
  int num;
  int max_id;
  int entry_size;
  int sanity;
} rhlist_t;

#define RHLNUM(rhlist)          rhlist->num

#define RHID(entry_h)           ((rhentry_t *)entry_h)->id
#define RHNAME(entry_h)         ((rhentry_t *)entry_h)->name
#define RHUSERS(entry_h)        ((rhentry_t *)entry_h)->users
#define RHUSER_ADD(entry_h)     (((rhentry_t *)entry_h)->users++)
#define RHUSER_DEL(entry_h)     (((rhentry_t *)entry_h)->users--)
#define RHMODE(entry_h)         ((rhentry_t *)entry_h)->mode

/*!
 * \brief Creates double link list and return its handle
 * \par DIRECT INPUT
 *   \param [in] name list may be assigned a name to facilitate debugging. Optional - may be NULL
 *   \param [in] entry_size size of list members - entries, serves for memory allocation, should be at least sizeof(rhentry_t)
 *   \param [in] sanity flag requesting sanity check on entry names and ID
 * \par DIRECT OUTPUT:
 *   \retval list handle, to be used in entry management
 */
rhlist_t *utilex_rhlist_create(
  char *name,
  int entry_size,
  int sanity);

/*!
 * \brief Free memory for all entries in the list and free the list
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 */
void utilex_rhlist_free_all(
  rhlist_t * rhlist);

/*!
 * \brief Free memory for all entries and reset the list to the new-born state
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 */
void utilex_rhlist_clean(
  rhlist_t * rhlist);

/*!
 * \brief Go through all entries and update max id of the list to the maximum existing entry one
 * Serves to refresh ID allocation mechanism
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 */
void utilex_rhlist_refresh_max_id(
  rhlist_t * rhlist);

/*!
 * \brief Allocate entry that will fit the list including sanity check, but not actually add it ti the list
 * Useful when there is a need to first allocate the memory and then set some fields in the entry that will
 * influence the place where entry should be added
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name entry name, optional, may be NULL
 *   \param [in] id entry id, may be either specific number, or if auto allocation requested use RHID_TO_BE_GENERATED (-1)
 *   \param [in,out] rhentry_p pointer to the entry handle
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 *   \retval _SHR_E_MEMORY memory allocation failure
 * \par INDIRECT OUTPUT
 *   *rhentry_p handle to new added top entry, undefined for failure
 */
shr_error_e utilex_rhlist_entry_alloc(
  rhlist_t * rhlist,
  char *name,
  int id,
  rhhandle_t * rhentry_p);

/*!
 * \brief Allocate entry that will fit the list including sanity check and add it on top of the list, so it will be first
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name entry name, optional, may be NULL
 *   \param [in] id entry id, may be either specific number, or if auto allocation requested use RHID_TO_BE_GENERATED (-1)
 *   \param [in,out] rhentry_p pointer to the place where entry handle will be put in case of success
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 *   \retval _SHR_E_MEMORY memory allocation failure
 * \par INDIRECT OUTPUT
 *   *rhentry_p handle to new added top entry, undefined for failure
 */
shr_error_e utilex_rhlist_entry_add_top(
  rhlist_t * rhlist,
  char *name,
  int id,
  rhhandle_t * rhentry_p);

/*!
 * \brief Allocate entry that will fit the list including sanity check and add it to the tail of the list, so it will be last
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name entry name, optional, may be NULL
 *   \param [in] id entry id, may be either specific number, or if auto allocation requested use RHID_TO_BE_GENERATED (-1)
 *   \param [out] rhentry_p pointer to the place where entry handle will be put in case of success
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 *   \retval _SHR_E_MEMORY memory allocation failure
 * \par INDIRECT OUTPUT
 *   *rhentry_p handle to new added top entry, undefined for failure
 */
shr_error_e utilex_rhlist_entry_add_tail(
  rhlist_t * rhlist,
  char *name,
  int id,
  rhhandle_t * rhentry_p);

/*!
 * \brief Insert previously allocated entry before another entry in list
 * The major use - keep list sorted by some field or criteria
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] rhentry entry to be inserted, must be valud entry handle
 *   \param [in] rhentry_next entry will be inserted before this one, if it is NULL, entry will be added to the tail
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 */
shr_error_e utilex_rhlist_entry_insert_before(
  rhlist_t * rhlist,
  rhhandle_t rhentry,
  rhhandle_t rhentry_next);

/*!
 * \brief Add previously allocated entry to the tail, making it last one
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] rhentry entry to be inserted
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 */
shr_error_e utilex_rhlist_entry_append(
  rhlist_t * rhlist,
  rhhandle_t rhentry);

/*!
 * \brief Delete entry from the list and free the memory
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] rhentry entry to be deleted
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 */
shr_error_e utilex_rhlist_entry_del_free(
  rhlist_t * rhlist,
  rhhandle_t rhentry);

/*!
 * \brief Verify that entry exists in the list
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] rhentry entry, which presence in list should be verified
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 */
shr_error_e utilex_rhlist_entry_exists(
  rhlist_t * rhlist,
  rhhandle_t rhentry);

/*!
 * \brief Get entry handle by its id
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] id entry id that serves as search criteria
 * \par DIRECT OUTPUT
 *   \retval handle found
 *   \retval NULL entry not found or error
 */
rhhandle_t utilex_rhlist_entry_get_by_id(
  rhlist_t * rhlist,
  int id);

/*!
 * \brief Get entry handle by its name
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name entry name that serves as search criteria
 * \par DIRECT OUTPUT
 *   \retval handle found
 *   \retval NULL entry not found or error
 */
rhhandle_t utilex_rhlist_entry_get_by_name(
  rhlist_t * rhlist,
  char *name);

/*!
 * \brief Get first entry(top) in the list
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 * \par DIRECT OUTPUT
 *   \retval entry handle for success
 *   \retval NULL is there are no entries or error
 */
rhhandle_t utilex_rhlist_entry_get_first(
  rhlist_t * rhlist);

/*!
 * \brief Get last entry(tail) in the list
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 * \par DIRECT OUTPUT
 *   \retval entry handle for success
 *   \retval NULL is there are no entries or error
 */
rhhandle_t utilex_rhlist_entry_get_last(
  rhlist_t * rhlist);

/*!
 * \brief Get next entry in the list, following the input one
 * \par DIRECT INPUT
 *   \param [in] rhentry entry that immediately preceding the one requested
 * \par DIRECT OUTPUT
 *   \retval entry handle for success
 *   \retval NULL is there are no entries or error
 */
rhhandle_t utilex_rhlist_entry_get_next(
  rhhandle_t rhentry);

/*!
 * \brief Get previous entry in the list, preceding the input one
 * \par DIRECT INPUT
 *   \param [in] rhentry entry that immediately follows the one requested
 * \par DIRECT OUTPUT
 *   \retval entry handle for success
 *   \retval NULL is there are no entries or error
 */
rhhandle_t utilex_rhlist_entry_get_prev(
  rhhandle_t rhentry);

/*!
 * \brief Get entry id given its name
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name entry name that serves as search criteria
 * \par DIRECT OUTPUT
 *   \retval entry id for success
 *   \retval negative for failure
 */
int utilex_rhlist_id_get_by_name(
  rhlist_t * rhlist,
  char *name);

/*!
 * \brief Verify existence of entry given its name
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] id entry id that serves as search criteria
 * \par DIRECT OUTPUT
 *   \retval valid name for success
 *   \retval NULL for failure
 */
char *utilex_rhlist_name_get_by_id(
  rhlist_t * rhlist,
  int id);

/*!
 * \brief Verify existence of entry with certain id
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] id entry id that serves as search criteria
 *   \param [in] rhentry_p - pointer to entry handle
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 *   \retval _SHR_E_NOT_FOUND id the entry was not found
 * \par INDIRECT OUTPUT
 *   *rhentry_p - entry handle, that will be assigned in the case of success
 */
shr_error_e utilex_rhlist_id_exists(
  rhlist_t * rhlist,
  int id,
  rhhandle_t * rhentry_p);

/*!
 * \brief Verify existence of entry with certain name
 * \par DIRECT INPUT
 *   \param [in] rhlist handle to list
 *   \param [in] name entry name that serves as search criteria
 *   \param [in] rhentry_p - pointer to entry handle, that will be assigned in the case of success, may be NULL is handle is not requested
 * \par DIRECT OUTPUT
 *   \retval _SHR_E_NONE for success
 *   \retval _SHR_E_PARAM problem with input parameters
 *   \retval _SHR_E_NOT_FOUND id the entry was not found
 * \par INDIRECT OUTPUT
 *   \param [out] rhentry_p - pointer to entry handle, that will be assigned in the case of success
 */
shr_error_e utilex_rhlist_name_exists(
  rhlist_t * rhlist,
  char *name,
  rhhandle_t * rhentry_p);

#define RHITERATOR(rhentry_h, rhlist)                \
        for(rhentry_h = utilex_rhlist_entry_get_first(rhlist); rhentry_h; rhentry_h = utilex_rhlist_entry_get_next(rhentry_h))

#define RHBACK_ITERATOR(rhentry_h, rhlist)                \
        for(rhentry_h = rhlist_get_last(rhlist); rhentry_h; rhentry_h = utilex_rhlist_entry_get_prev(rhentry_h))

#define RHSAFE_ITERATOR(rhentry_h, rhlist)                                                                               \
        rhhandle_t rhentry_next_h;                                                                                       \
        for(rhentry_h = utilex_rhlist_entry_get_first(rhlist), rhentry_next_h = utilex_rhlist_entry_get_next(rhentry_h); \
                       rhentry_h; rhentry_h = rhentry_next_h, rhentry_next_h = utilex_rhlist_entry_get_next(rhentry_h))

#define RHCOND_ITERATOR(rhentry_h, rhlist)                                                            \
        rhhandle_t rhentry_next_h = NULL;                                                             \
        if(rhentry_h == NULL) {                                                                       \
            rhentry_h = utilex_rhlist_entry_get_first(rhlist);                                        \
            rhentry_next_h = utilex_rhlist_entry_get_next(rhentry_h);                                 \
        }                                                                                             \
        for(;rhentry_h; rhentry_h = rhentry_next_h, rhentry_next_h = utilex_rhlist_entry_get_next(rhentry_h))

#endif /* UTILEX_RHLIST_H_INCLUDED */
