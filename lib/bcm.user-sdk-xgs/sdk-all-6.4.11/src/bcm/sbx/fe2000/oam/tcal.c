/*
 * $Id: tcal.c,v 1.9 Broadcom SDK $
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
 *
 *  FE2000 timer calendar managment 
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/sbx/g2p3/g2p3.h>

#include <bcm_int/sbx/fe2000/oam/oam.h>
#include <bcm_int/sbx/fe2000/oam/tcal.h>

#include <shared/idxres_fl.h>

#include <bcm/error.h>


#define TCAL_MULTI_ENTRY_FLAG    0x80000000
#define TCAL_INVALID_REC         _BCM_TCAL_INVALID_ID
#define TCAL_IS_MULTI_ENTRY(x)   (!!((x) & TCAL_MULTI_ENTRY_FLAG))
#define TCAL_SET_MULTI_ENTRY(x)   (x) |= TCAL_MULTI_ENTRY_FLAG
#define TCAL_RECORD(x)           ((x) & _BCM_TCAL_ID_MASK)



#define TCAL_MAX_RETRY           16
#define TCAL_NUM_INTERVAL        8


/*********************************************************************
 *       _BCM_TCAL_WINDOW_SIZE and _BCM_TCAL_ANY_ENTRY are
 *            relevent for multi-entry sessions only.
 *********************************************************************/

/* window size defines how many entries from the ideal selection,
 * the algorithm can go within the same slot, to find a free
 * entry.  the default is 250 entries.  with surrent settings,
 * the total tcal is 6000 entries.  each slot has 2000 entries.
 * finding a free entry within +/- 250 of the ideal entry, defines
 * a maximum of 500 entries to choose from.  this is 25% of the
 * slot size, and thus 25% of the session's interval.
 * setting the window size to 0 will by-pass the algorithm that
 * tries to evenly space multi-entry sessions (3.3 ms).
 */
#define _BCM_TCAL_WINDOW_SIZE    250

/* the any-entry flag allows the algorithm to pick any entry
 * within the slot, inside or outside of the window defined
 * above.
 */
#define _BCM_TCAL_ANY_ENTRY      1
/* the window-size and any-entry flags must be used together
 * in a meaningful way.  The code is structured such that it
 * will try to place multi-entry sessions within the window
 * if window-size is non-zero.  then if that fails, it will
 * try to just allocate any free entry in the slot, if any-entry
 * is non-zero.  if both window-size and any-entry are zero
 * no allocation of multi-entry sessions will occurr and
 * NO_RESOURCES will always be returned.
 *
 * - for fast allocation, set window-size to zero and any-entry to one.
 * - for only evenly spaced allocation, set window-size to non-zero,
 *     and any-entry to zero.
 * - default setting is for both.  window-size is non-zero for
 *     evenly spaced allocation.  but if that fails to find entries,
 *     it will then try to find three free entries anywhere in their
 *     slots.  this is the slowest running combination possible!
 */


typedef struct {
    uint32 reference_timeout;
    uint32 slot_size;
    uint32 num_slots;
    uint8    slot;              /* used to attempt to evenly distribute slot 
                                 * usage
                                 */
    uint32 interval_encode[TCAL_NUM_INTERVAL];
    shr_idxres_list_handle_t  pool;   /* Timer Calendar rsource management */
} tcal_state_t;

tcal_state_t _tcal_state[SOC_MAX_NUM_DEVICES];

#define TS(u)  _tcal_state[u]


extern int global_oam_sdk;    /* number of times the ucode table writes timed out. */


/*
 *   Function
 *      _bcm_tcal_init
 *   Purpose
 *      initialize the timer calendar manager
 *   Parameters
 *      (in)  unit          - BCM device number
 *   Returns
 *       BCM_E_*
 */
int
_bcm_tcal_init(int unit) 
{
    uint32 bubble_timeout, list_mgr_entries, max_calendar, recirc, interval=0;
    int rv;

    /* sanity chec the multi-entry allocation defines.  they cant both
     * be zero.
     */
    if (!_BCM_TCAL_WINDOW_SIZE && !_BCM_TCAL_ANY_ENTRY) {
      LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "Invalid configration for multi-entry sessions.\n")));
    }


    rv = soc_sbx_g2p3_oam_bubble_timeout_get(unit, &bubble_timeout);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to read bubble timeout: %s\n"),
                   bcm_errmsg(rv)));
        return rv;
    }

    /* max number of entries in the timer calendar.  this is the size of
     * the ring buffer in the list manager.
     */
    if (SOC_IS_SBX_FE2KXT(unit)) {
        rv = soc_sbx_g2p3_oam_list_mgr_size_get(unit, &list_mgr_entries);
    } else {
        rv = soc_sbx_g2p3_oam_list_mgr_size_ca_get(unit, &list_mgr_entries);
    }
    if (BCM_FAILURE(rv)) {        
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to read oam list mgr size: %s\n"),
                   bcm_errmsg(rv)));
        return rv;
    }

    /* this recirc value is the max number of packets that can be
     * transmitted for each bubble event.  Each bubble can generate
     * at most 'recirc' number of packets.  Recirculation is used
     * to walk list manager to read 'recirc' tcal entries for each bubble.
     */
    if (SOC_IS_SBX_FE2KXT(unit)) {
        rv = soc_sbx_g2p3_oam_tx_packets_per_bubble_get(unit, &recirc);
    } else {
        rv = soc_sbx_g2p3_oam_tx_packets_per_bubble_ca_get(unit, &recirc);
    }
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to read oam recirc count: %s\n"),
                   bcm_errmsg(rv)));
        return rv;
    }

    /* bubble-timeout   * list-mgr-size = timer-period
     *  (5.7.0) 20 usec * 500 entries   = 10 msec
     *  (5.8.0) 10 usec * 1000 entries  = 10 msec
     *  (5.9.0 phase 1)  10 usec * (6000 entries / 6 Recirc per bubble)    = 10 msec
     *  (5.9.0 phase 2) 3.3 msec * (6000 entries / 6000 Recirc per bubble) = 3.3 msec
     *
     *  num-slots is valid up until 5.9.0 phase 2 at which point num-slots = 1.
     *  timer-period / min-interval    = num-slots
     *  10 msec      / 3.3 msec        = 3
     *
     *  list-mgr-size   / num-slots      = tcal-slot-size
     *  (5.7.0) 500     / 3              = 166
     *  (5.8.0) 1000    / 3              = 333
     *  (5.9.0 p1) 6000 / 3              = 2000
     */
    TS(unit).reference_timeout = (bubble_timeout * (list_mgr_entries / recirc)) / 1000;
    TS(unit).num_slots         = TS(unit).reference_timeout / 3;
    TS(unit).slot_size         = list_mgr_entries / TS(unit).num_slots;

    LOG_INFO(BSL_LS_BCM_OAM,
             (BSL_META_U(unit,
                         "reference_timeout %d, "
                         "num_slots %d, slot_size %d\n"),
              TS(unit).reference_timeout, TS(unit).num_slots, 
              TS(unit).slot_size));

    /* interval[0] is always zero.  there is no push-down. */
     TS(unit).interval_encode[0] = 0;
   
    /* read the supported interval values from push-downs */
    rv = soc_sbx_g2p3_oam_interval1_get(unit, &interval);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to read interval1: %s\n"),bcm_errmsg(rv)));
        return rv;
    }
    TS(unit).interval_encode[1] = interval;

    rv = soc_sbx_g2p3_oam_interval2_get(unit, &interval);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to read interval2: %s\n"),bcm_errmsg(rv)));
        return rv;
    }
    TS(unit).interval_encode[2] = interval;

    rv = soc_sbx_g2p3_oam_interval3_get(unit, &interval);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to read interval3: %s\n"),bcm_errmsg(rv)));
        return rv;
    }
    TS(unit).interval_encode[3] = interval;

    rv = soc_sbx_g2p3_oam_interval4_get(unit, &interval);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to read interval4: %s\n"),bcm_errmsg(rv)));
        return rv;
    }
    TS(unit).interval_encode[4] = interval;

    rv = soc_sbx_g2p3_oam_interval5_get(unit, &interval);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to read interval5: %s\n"),bcm_errmsg(rv)));
        return rv;
    }
    TS(unit).interval_encode[5] = interval;

    rv = soc_sbx_g2p3_oam_interval6_get(unit, &interval);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to read interval6: %s\n"),bcm_errmsg(rv)));
        return rv;
    }
    TS(unit).interval_encode[6] = interval;

    rv = soc_sbx_g2p3_oam_interval7_get(unit, &interval);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to read interval7: %s\n"),bcm_errmsg(rv)));
        return rv;
    }
    TS(unit).interval_encode[7] = interval;


    rv = soc_sbx_g2p3_oam_max_calendar_get(unit, &max_calendar);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to read max calendar: %s\n"),
                   bcm_errmsg(rv)));
        return rv;
    }

    rv = shr_idxres_list_create(&TS(unit).pool,
                                0, max_calendar - 1,
                                0, max_calendar - 1,
                                "timer calendar pool");
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to create timer calendar pool: %s\n"),
                   bcm_errmsg(rv)));
        return rv;
    }

    TS(unit).slot = 0;

    return rv;
}

/*
 *   Function
 *      _bcm_tcal_cleanup
 *   Purpose
 *      free any timer calendar resources
 *   Parameters
 *      (in)  unit          - BCM device number
 *   Returns
 *       BCM_E_*
 */
int
_bcm_tcal_cleanup(int unit)
{
    if (TS(unit).pool) {
        shr_idxres_list_destroy(TS(unit).pool);
        TS(unit).pool = 0;
    }
    return BCM_E_NONE;
}


/*
 *   Function
 *      _bcm_tcal_interval_decode
 *   Purpose
 *      Convert from interval enumeration to interval in ms.
 *   Parameters
 *      (in)  unit          - BCM device number
 *      (in)  encoded       - Interval enumeration
 *      (out) interval_ms   - Interval in ms
 *   Returns
 *       BCM_E_*
 */
int
_bcm_tcal_interval_decode(int unit, uint32 encoded, int *interval_ms)
{
    if (encoded < TCAL_NUM_INTERVAL) {
        *interval_ms = TS(unit).interval_encode[encoded];
        return BCM_E_NONE;
    }
    return BCM_E_PARAM;
}

/*
 *   Function
 *      _bcm_tcal_interval_encode
 *   Purpose
 *      Convert from interval in ms to an interval enumeration.
 *   Parameters
 *      (in)  unit          - BCM device number
 *      (in)  interval_ms   - Interval in ms
 *      (out) encoded       - Interval enumeration
 *   Returns
 *       BCM_E_*
 */
int
_bcm_tcal_interval_encode(int unit, int interval_ms, uint32 *encoded)
{
    int num = TCAL_NUM_INTERVAL;
    int i;

    for (i=1; i<num; i++) {
        if (interval_ms <= TS(unit).interval_encode[i]) {
            *encoded = i;
            return BCM_E_NONE;
        }
    }
    return BCM_E_PARAM;
}


/*
 *   Function
 *      _tcal_interval_get
 *   Purpose
 *      Convert a time period to BCM8802x timer calendar entry settings
 *   Parameters
 *      (in)  unit          - BCM device number
 *      (in)  period        - period in ms, for which a timer should expire
 *      (out) tcal_interval - BCM8802x timer calendar interval value
 *      (out) is_multi      - true if BCM8802x requires 3 tcal entries to 
 *                           implement the given period
 *   Returns
 *       BCM_E_*
 */
STATIC int
_tcal_interval_get(int unit, int period, int *tcal_interval, int *is_multi)
{
    if (period < TS(unit).reference_timeout) {
        *tcal_interval = 1;
        *is_multi = 1;
    } else {
        *tcal_interval = period / TS(unit).reference_timeout;
        *is_multi = 0;
    }
    return BCM_E_NONE;
}


/*
 *   Function
 *      _tcal_check_entry
 *   Purpose
 *      Checks if a timer calendar entry is available, or already reserved.
 *   Parameters
 *      (in)  unit          - BCM device number
 *      (in)  rec           - tcal entry number to check
 *   Returns
 *      If ok (entry available) returns the entry num (rec).
 *      Otherwise, if not ok (entry already reserved) returns _BCM_TCAL_INVALID_ID.
 */
STATIC int
_tcal_check_entry(int unit, int rec)
{
    return ((shr_idxres_list_elem_state(TS(unit).pool, rec) == BCM_E_NOT_FOUND) ? rec : _BCM_TCAL_INVALID_ID);
}


/*
 *   Function
 *      _tcal_get_next_in_window
 *   Purpose
 *      Tries to find an available entry, closest to the ideal location,
 *      within a defined window and within the current slot.
 *   Parameters
 *      (in)  unit          - BCM device number
 *      (in)  slot          - slot number to search within
 *      (in)  ideal         - the entry which results in the most evenly spaced transmit.
 *   Returns
 *      If ok (entry available within the window) returns the entry number.
 *      Otherwise, if not ok (no free entries in window) returns _BCM_TCAL_INVALID_ID.
 */
STATIC int
_tcal_get_next_in_window(int unit, int slot, int ideal)
{
    int min, max, i;
    int lo, hi, rec=_BCM_TCAL_INVALID_ID;

    min = (slot) * TS(unit).slot_size;
    max = (slot+1) * TS(unit).slot_size - 1;

    /* adjust the ideal entry location based on slot. */
    ideal = ideal + (slot) * TS(unit).slot_size;

    /* find an entry in this slot, within the window,
     * starting with the ideal location, and moving
     * out from there.
     */
    for (i=0; i<_BCM_TCAL_WINDOW_SIZE; i++) {
        lo = ideal - i;
        hi = ideal + i;
        rec = _BCM_TCAL_INVALID_ID;
        if (lo >= min) {
            rec = _tcal_check_entry(unit, lo);
        }
        if ((rec == _BCM_TCAL_INVALID_ID) && (hi <= max)) {
            rec = _tcal_check_entry(unit, hi);
        }
        if (rec != _BCM_TCAL_INVALID_ID) {
            break;
        }
    }

    return rec;
}


/*
 *   Function
 *      _tcal_evenly_space_multi_entries
 *   Purpose
 *      Tries to allocate a session's transmit tcal entries, evenly across
 *      all slots in the tcal.
 *   Parameters
 *      (in)  unit          - BCM device number
 *      (out) rec_list      - array of transmit points (entries) within the tcal.
 *   Returns
 *      1: ok, all the session's entries can be allocated within the defined window.
 *      0: not ok, there are no free entries across all slots such they all fall
 *         within the window.
 */
STATIC int
_tcal_evenly_space_multi_entries(int unit, uint16 rec_list[3])
{
    int found=0;
    int slot, i, rec, rec_max = TS(unit).slot_size - 1;

    /* allocate multi-entry sessions from the end of the table, to
     * avoid having the single-entry sessions leaving holes in the
     * table.  Eventually they will mix anyways, but this might
     * help occupancy rates, and setup times.
     */
    for (i=rec_max; i >= 0; i--) {
        /* iterate over slot 0, to find a potential entry for rec_list[0]. */
        if (_tcal_check_entry(unit, i) != _BCM_TCAL_INVALID_ID) {
            /* this entry is available, try placing the remaining
             * entries for this multi-entry session, within the
             * configured window size.
             */
            rec_list[0] = i;

            /* now try to place the other slots' entries. */
            found = 1;
            for (slot=1; slot<TS(unit).num_slots; slot++) {
                /* check all the entries within the window, for each remaining slot. */
                rec = _tcal_get_next_in_window(unit, slot, i);
                /* see if there was a free entry within the window. */
                if (rec == _BCM_TCAL_INVALID_ID) {
                    /* there were no free entries in this slot, must back up and
                     * pick another rec_list[0] entry.
                     */
                    found = 0;
                    break;
                } else {
                    rec_list[slot] = rec;
                }
            }

            /* did we place the remaining slots' entries?  if so, done. */
            if (found) {
                LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "Found in window: [%d,%d,%d]\n"),
                             rec_list[0], rec_list[1], rec_list[2]));
                break;
            }
        }
        /* if we're still in the outer loop, trying to find a rec_list[0] entry,
         * then we've not found the remaining slots' entries either.
         */
        found = 0;
    }

    return found;
}


/*
 *   Function
 *      _tcal_get_next_free_entry
 *   Purpose
 *      Tries to allocate one entry from the specified slot.
 *   Parameters
 *      (in)  unit          - BCM device number
 *      (out) rec_list      - array of transmit points (entries) within the tcal.
 *   Returns
 *      If ok, the entry number is returned.
 *      If not ok (couldnt find a free entry in this slot) _BCM_TCAL_INVALID_ID is returned.
 */
STATIC int
_tcal_get_next_free_entry(int unit, int slot)
{
    int i;
    int rec = _BCM_TCAL_INVALID_ID;
    int rec_min = (slot) * TS(unit).slot_size;
    int rec_max = (slot+1) * TS(unit).slot_size - 1;

    /* allocate single-entry sessions from the start of the slot.
     * mutli-entry sessions are allocated from the end, to try
     * to avoid fragmenting the slots' available entries.
     */
    for (i=rec_min; i < rec_max; i++) {
        rec =_tcal_check_entry(unit, i);
        if (rec != _BCM_TCAL_INVALID_ID) {
            break;
        }
    }

    return rec;
}


/*
 *   Function
 *      _tcal_records_alloc
 *   Purpose
 *      Allocate and initialize BCM8802x timer calendar entries
 *   Parameters
 *      (in)  unit          - BCM8802x BCM device number
 *      (in)  period        - period in ms, for which a timer should expire
 *      (out) tcal_interval - BCM8802x timer calendar interval value
 *      (out) is_multi      - true if BCM8802x requires 3 tcal entries to 
 *                           implement the given period
 *      (out) num_tcal_entries - number entries allocated, count of valid
 *                               rec_list record
 *      (out) rec_list      - records indexes allocated
 *   Returns
 *       BCM_E_*
 */
STATIC int
_tcal_records_alloc(int unit, int period, int *tcal_interval, 
                    int *num_tcal_entries, uint16 rec_list[3])
{
    int slot, rec, i;
    int is_multi;
    int rv = BCM_E_NONE;
    int found=0;
    int rec_min;
    int rec_max;

    _tcal_interval_get(unit, period, tcal_interval, &is_multi);

    /* tx interval of 3.3 ms is a special case where 3 equally spaced entries
     * must be allocated; one per 'slot'.  When allocating the special case,
     * always start at slot 0; when allocating a normal timer calendar, round-
     * robin accross the 3 slots to try to balance out the slot usage.
     */
    if (is_multi) {
        /* multi-entry sessions. */

        *num_tcal_entries = TS(unit).num_slots;

        /* first, try to evenly space the entries in each slot
         * such that they fall within a certain transmit window.
         * if the window size is zero, that means dont try to
         * evenly space.  just get any entry in the slot.
         */
        if (_BCM_TCAL_WINDOW_SIZE) {
            found = _tcal_evenly_space_multi_entries(unit, rec_list);
        }

        /* if that didnt work, and the get-any flag is set, then
         * just try to allocate from anywhere in each of the slots.
         */
        if (!found && _BCM_TCAL_ANY_ENTRY) {
            for (slot=0; slot<TS(unit).num_slots; slot++) {
                rec_min = (slot) * TS(unit).slot_size;
                rec_max = (slot+1) * TS(unit).slot_size - 1;
                found = 0;
                /* try to find an entry within this slot. */
                for (rec=rec_max; rec >= rec_min; rec--) {
                    if (_tcal_check_entry(unit, rec) != _BCM_TCAL_INVALID_ID) {
                        found = 1;
                        rec_list[slot] = rec;
                        break;
                    }
                }
                if (!found) {
                    /* couldnt find 1 free entry in this slot.
                     * cant continue, break out of loop and return fail (no-resources).
                     */
                    break;
                }
            }
            LOG_VERBOSE(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "Found any entry: [%d,%d,%d]\n"),
                         rec_list[0], rec_list[1], rec_list[2]));
        }

    } else {
        /* Single-entry sessions. */

        *num_tcal_entries = 1;
        slot = TS(unit).slot;
        /* start with the last slot used, and try to allocate 
         * an entry within that slot.  if no free entries,
         * then check the remaining two slots.
         */
        for (i=0; i<TS(unit).num_slots; i++) {
            slot = (slot+i) % TS(unit).num_slots;
            rec = _tcal_get_next_free_entry(unit, slot);
            if (rec != _BCM_TCAL_INVALID_ID) {
                rec_list[0] = rec;
                found = 1;
                /* update the state info */
                TS(unit).slot = (slot + 1) % TS(unit).num_slots;
                LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "Found single entry: [%d]\n"), rec_list[0]));
                break;
            }
        }
    }

    /* if entries have been found, reserve them. */

    if (!found) {
        /* couldnt find entry(ies) to satisfy request. */
        rv = BCM_E_RESOURCE;
    } else {
        /* found entries to satisfy request, and they are in the rec_list,
         * but have not yet been reserved.  do that now for each entry.
         */
        for (i=0; i<*num_tcal_entries; i++) {
            rv = shr_idxres_list_reserve(TS(unit).pool, rec_list[i], rec_list[i]);
            if (BCM_FAILURE(rv)) {
                /* this "i" entry failed to reserve, which means we do
                 * not need to free it.  just mark the entry as invalid.
                 */
                rec_list[i] = _BCM_TCAL_INVALID_ID;
                /* failed to reserve an entry, if there are mutliple entries,
                 * free any that might have already been reserved.
                 */
                if (_BCM_TCAL_ID_VALID(rec_list[0])) {
                    shr_idxres_list_free(TS(unit).pool, rec_list[0]);
                    rec_list[0] = _BCM_TCAL_INVALID_ID;
                }
                if (_BCM_TCAL_ID_VALID(rec_list[1])) {
                    shr_idxres_list_free(TS(unit).pool, rec_list[1]);
                    rec_list[1] = _BCM_TCAL_INVALID_ID;
                }
                rec_list[2] = _BCM_TCAL_INVALID_ID;
                rv = BCM_E_RESOURCE;
                /* do not continue trying to reserve entries if one fails! */
                break;
            }
        }
    }

    return rv;
}


/*
 *   Function
 *      _tcal_records_find
 *   Purpose
 *      Retrieve hardware IDs associtated with the given timer calendar id
 *   Parameters
 *      (in)  unit          - BCM device number
 *      (in)  tcal_id       - tcal_id used to find records
 *      (out) num_tcal_entries - number of entries associated with tcal id
 *      (out) rec_list      - hardware record ids associated with the 
 *                            given tcal_id
 *   Returns
 *       BCM_E_*
 */
STATIC int
_tcal_records_find(int unit, tcal_id_t tcal_id, int *num_tcal_entries,
                   uint16 rec_list[3])
{
    soc_sbx_g2p3_oamtimercalendar_t tcal;
    uint32 ep_rec_idx;
    int i, rv;
    int rec, start, end;


    if (TCAL_IS_MULTI_ENTRY(tcal_id)) {

        *num_tcal_entries = TS(unit).num_slots;

        /* Find the other entries: given that tcal_id contains the base
         * timer calendar record index, they are roughly TCAL_SLOT_SIZE
         * entries apart, and all the same Endpoint Record Pointer.
         */
        soc_sbx_g2p3_oamtimercalendar_t_init(&tcal);

        rec_list[0] = TCAL_RECORD(tcal_id);
        rv = soc_sbx_g2p3_oamtimercalendar_get(unit, rec_list[0], &tcal);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to get timer calendar record %d\n"),
                       rec_list[0]));
            return rv;
        }

        ep_rec_idx = tcal.endpointPtr;

        for (i=1; i<TS(unit).num_slots; i++) {
            /* Start searching for the companion entries at the
             * beginning of each slot.  it is possible that the
             * first slot's entry is a dozen or so entries in, and
             * either of the remaining two slots' entries could
             * be located right at the beginning of their slots. */
            start =  i    * TS(unit).slot_size;
            end   = (i+1) * TS(unit).slot_size;

            for (rec = start; rec < end; rec++) {

                /* first see if it's even allocated */
                rv = shr_idxres_list_elem_state(TS(unit).pool, rec);
                if (rv == BCM_E_EXISTS) {
                    /* now, go to the hw table */
                    rv = soc_sbx_g2p3_oamtimercalendar_get(unit, rec, &tcal);
                    if (rv != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                                  (BSL_META_U(unit,
                                              "Failed to get timer calendar "
                                              "record %d\n"), rec));
                        return rv;
                    }
                    if (tcal.endpointPtr == ep_rec_idx) {
                        /* got it!*/
                        rec_list[i] = rec;
                        break;
                    }
                }
            }
        }

    } else {
        /* one entry, nothing to search find, this is the entry */
        *num_tcal_entries = 1;
        rec_list[0] = TCAL_RECORD(tcal_id);
    }

    return BCM_E_NONE;
}



/*
 *   Function
 *      _tcal_records_free
 *   Purpose
 *      Free hardware records associated with the given ID, or free
 *      the records in rec_list, if known.
 *   Parameters
 *      (in)  unit          - BCM device number
 *      (in/out) num_tcal_entries - number of entries int rec_list
 *                                  0 if not known
 *      (in)  tcal_id       - tcal_id used to find records to delete,
 *                            if *num_tcal_entries == 0
 *      (in/out) rec_list   - hardware record ids to delete/deleted
 *   Returns
 *       BCM_E_PARAM        - entry doesnt exist or is invalid
 */
STATIC int
_tcal_records_free(int unit, int *num_tcal_entries,
                   tcal_id_t tcal_id, uint16 rec_list[3])
{
    int i;
    
    if (!_BCM_TCAL_ID_VALID(tcal_id)) {
        return BCM_E_PARAM;
    }

    if (*num_tcal_entries == 0) {
        _tcal_records_find(unit, tcal_id, num_tcal_entries, rec_list);
    }
    /* else, caller supplied the records to free */

    for (i=0; i<*num_tcal_entries; i++) {
        shr_idxres_list_free(TS(unit).pool, rec_list[i]);
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "free TCal idx=0x%x\n"),
         rec_list[i]));
    }

    return BCM_E_NONE;
}



/*
 *   Function
 *      _tcal_records_update
 *   Purpose
 *      Update hardware IDs associated with the given timer calendar id
 *   Parameters
 *      (in)  unit          - BCM device number
 *      (in)  tx_enable     - enable calendar for transmit
 *      (in)  ep_rec_index  - endpoint to transmit
 *      (in)  num_tcal_entries - number of entries to update
 *      (in)  rec_list      - list of records to update
 *      (in/out)  tcal_id   - tcal_id to update
 *   Returns
 *       BCM_E_*
 */

STATIC int
_tcal_records_update(int unit, int tx_enable, int period, int tcal_interval,
                     uint32 ep_rec_index, int num_tcal_entries,
                     uint16 rec_list[3], tcal_id_t *tcal_id)
{
    int rv = BCM_E_NONE;
    int i;
    soc_sbx_g2p3_oamtimercalendar_t tcal, tcal_copy;
    uint32 tcal_period=0;
    int tcal_status, timeout, error;

    /* convert the period in ms, into an enumeration representing
     * the period.  this enum must match the expected enum in
     * the CCM.flags field.
     */
    rv = _bcm_tcal_interval_encode(unit, period, &tcal_period);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to encode interval=%d\n"),period));
        return rv;
    }

    soc_sbx_g2p3_oamtimercalendar_t_init(&tcal);
    for (i=0; i<num_tcal_entries; i++) {
        tcal.xmitPeriod  = (tx_enable ? tcal_period : 0);
        tcal.xmitCount   = tcal_interval;
        tcal.endpointPtr = ep_rec_index;
        rv = soc_sbx_g2p3_oamtimercalendar_set(unit, rec_list[i], &tcal);
        
        tcal_status = 1;
        timeout = 5;
        error = 0;

        /* loop through timeout times or until values match */
        while (tcal_status && (timeout > 0) && (error == 0)) {
            timeout--;
            rv = soc_sbx_g2p3_oamtimercalendar_set(unit, rec_list[i], &tcal);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Failed to write tcal 0x%x: %d %s\n"),
                           rec_list[i], rv, bcm_errmsg(rv)));
                error = 1;
            } else {
                rv = soc_sbx_g2p3_oamtimercalendar_get(unit, rec_list[i], &tcal_copy);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "Failed to get tcal 0x%x: %d %s\n"),
                               rec_list[i], rv, bcm_errmsg(rv)));
                    error = 1;
                }
            }
            if (tcal.xmitPeriod == tcal_copy.xmitPeriod) {
                tcal_status = 0;
                LOG_VERBOSE(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "tcal_status successful %x for %x\n"),
                             rec_list[i], tcal_copy.xmitPeriod));
            } else {
                LOG_WARN(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit,
                                     "Retry to write tcal 0x%x: timeout %d. %x for %x\n"),
                          rec_list[i], timeout, tcal.xmitPeriod, tcal_copy.xmitPeriod));
            }
        }
        if (timeout == 0) {
            rv = BCM_E_INTERNAL;
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to write tcal TIMEOUT 0x%x: %d %s\n"),
                       rec_list[i], rv, bcm_errmsg(rv)));
            global_oam_sdk = global_oam_sdk + 1;
        }

        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "Wrote TCal idx=%d  period=%d  enabled=%d rv=%d\n"),
         
                  rec_list[i], tcal.xmitPeriod, tx_enable, rv));


        if (rv != BCM_E_NONE) {
            break;
        }
    }

    /* free allocated entries on failure */
    if (rv != BCM_E_NONE) {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "Failed to write TCal entry, "
                             "freeing\n")));
        for (; i>=0; i--) {
            tcal.xmitPeriod  = 0;  /* this sets an entry to unused/invalid */
            tcal.xmitCount   = 0;
            tcal.endpointPtr = 0;
            soc_sbx_g2p3_oamtimercalendar_set(unit, rec_list[i], &tcal);
        }

        _tcal_records_free(unit, &num_tcal_entries, *tcal_id, rec_list);

        *tcal_id = _BCM_TCAL_INVALID_ID;

        return rv;
    }

    return BCM_E_NONE;
}



/*
 *   Function
 *      _bcm_tcal_update
 *   Purpose
 *      Update an existing BCM8802x timer calendar entry, if existing entry
 *      is implemented as multiple entries, they will be freed as required.
 *   Parameters
 *      (in)  unit          - BCM8802x BCM device number
 *      (in)  period        - period in ms, for which a timer should expire
 *      (in)  tx_enable     - enable the timer to intiate transmit
 *      (in)  ep_rec_index  - endpoint record index to transmit
 *      (inout) tcal_id     - timer calenedar entry to update
 *   Returns
 *       BCM_E_*
 */
int
_bcm_tcal_update(int unit, int period, int tx_enable, uint32 ep_rec_index,
                 tcal_id_t *tcal_id)
{
    int rv = BCM_E_NONE;
    int num_tcal_entries = 0;
    int is_multi, was_multi;
    int tcal_interval;
    uint16 rec_list[3] = { TCAL_INVALID_REC,
                             TCAL_INVALID_REC,
                             TCAL_INVALID_REC };
    soc_sbx_g2p3_oamtimercalendar_t tcal;
    uint32 tcal_period=0;

    rv = _tcal_records_find(unit, *tcal_id, &num_tcal_entries, rec_list);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to find tcal entries at id=0x%x ep_idx=%d\n"),
                   *tcal_id, ep_rec_index));
        return rv;
    }

    _tcal_interval_get(unit, period, &tcal_interval, &is_multi);
    was_multi = TCAL_IS_MULTI_ENTRY(*tcal_id);

    if (was_multi != is_multi) {
        LOG_INFO(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "old interval is%smulti, "
                             "new interval is%smulti, "
                             "delete & realloc\n"),
                  (was_multi ? " " : " not "), (is_multi ? " " : " not ")));

        rv = _bcm_tcal_free(unit, tcal_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to free tcal id=0x%x on update\n"), *tcal_id));
            return rv;
        }

        return _bcm_tcal_alloc(unit, period, tx_enable, ep_rec_index, tcal_id);
    } else {
        /* get the old tcal entry to compare with the new.  if the periods match,
         * do not update the tcal entry.  updating will reset the tcal count down
         * value, such that period updates will prevent the tcal from ever triggering
         * message transmission.
         */
        rv = soc_sbx_g2p3_oamtimercalendar_get(unit, rec_list[0], &tcal);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "Failed to get timer calendar record %d\n"),
                       rec_list[0]));
            return rv;
        }

        /* convert the period in ms, into an enumeration representing
         * the period.  this enum must match the expected enum in
         * the CCM.flags field.
         */
        rv = _bcm_tcal_interval_encode(unit, period, &tcal_period);
        if (BCM_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "Failed to encode interval=0x%x\n"),period));
            return rv;
        }

        tcal_period = (tx_enable ? tcal_period : 0);

        /* check if we need to update */
        if (tcal.xmitPeriod != tcal_period) {
            return _tcal_records_update(unit, tx_enable, period, tcal_interval, ep_rec_index,
                                    num_tcal_entries, rec_list, tcal_id);
        } else {
            LOG_INFO(BSL_LS_BCM_OAM,
                     (BSL_META_U(unit,
                                 "No changes, "
                                 "tcal entry (id=0x%x) not updated.\n"), *tcal_id));
        }
    }

    /* no update took place */
    return BCM_E_NONE;
}


/*
 *   Function
 *      _bcm_tcal_alloc
 *   Purpose
 *      Allocate a BCM8802x timer calendar entry
 *   Parameters
 *      (in)  unit          - BCM8802x BCM device number
 *      (in)  period        - period in ms, for which a timer should expire
 *      (in)  tx_enable     - enable the timer to intiate transmit
 *      (in)  ep_rec_index  - endpoint record index to transmit
 *      (out) tcal_id       - timer calenedar entry to update
 *   Returns
 *       BCM_E_*
 */
int
_bcm_tcal_alloc(int unit, int period, int tx_enable,
                uint32 ep_rec_index, tcal_id_t *tcal_id)
{
    int tcal_interval;
    uint16 rec_list[3] = { TCAL_INVALID_REC,
                             TCAL_INVALID_REC,
                             TCAL_INVALID_REC };
    int num_tcal_entries;
    int rv;

    rv = _tcal_records_alloc(unit, period, &tcal_interval, 
                             &num_tcal_entries, rec_list);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "Failed to allocated timer calendar resources\n")));
        *tcal_id = _BCM_TCAL_INVALID_ID;
        return rv;
    }

    /* tcal_id is the first record, always.  Set a flag bit to signify this
     * id is a special case and will be handled differently for all
     * operations
     */
    *tcal_id = rec_list[0];
    if (num_tcal_entries > 1) {
        TCAL_SET_MULTI_ENTRY(*tcal_id);
    }

    return _tcal_records_update(unit, tx_enable, period, tcal_interval, ep_rec_index,
                                num_tcal_entries, rec_list, tcal_id);
}



/*
 *   Function
 *      _bcm_tcal_free
 *   Purpose
 *      Release the resources associated with the given tcalid
 *   Parameters
 *      (in)  unit          - BCM device number
 *      (in/out)  tcal_id   - tcal_id to free
 *   Returns
 *       BCM_E_*
 */
int
_bcm_tcal_free(int unit, tcal_id_t *tcal_id)
{
    int num_tcal_entries = 0;
    uint16 rec_list[3] = 
        { TCAL_INVALID_REC, TCAL_INVALID_REC, TCAL_INVALID_REC };
    soc_sbx_g2p3_oamtimercalendar_t tcal;
    int i;
    int rv, status = BCM_E_NONE;

    LOG_VERBOSE(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "Freeing tcal id 0x%x\n"), *tcal_id));

    status = _tcal_records_free(unit, &num_tcal_entries, *tcal_id, rec_list);

    *tcal_id = TCAL_INVALID_REC;

    soc_sbx_g2p3_oamtimercalendar_t_init(&tcal);

    for (i=0; i<num_tcal_entries; i++) {
        tcal.xmitPeriod  = 0; /* this set the entry to unused/invalid */
        tcal.xmitCount   = 0;
        tcal.endpointPtr = 0;
        rv = soc_sbx_g2p3_oamtimercalendar_set(unit, rec_list[i], &tcal);
        if (rv != BCM_E_NONE) {
            status = BCM_E_INTERNAL;
        }
    }

    return status;
}


/*
 *   Function
 *      _bcm_tcal_period_get
 *   Purpose
 *      obtains period associated with Tcal
 *   Parameters
 *      (in)  unit          - BCM device number
 *      (in)  tcal_id       - tcal_id
 *      (in/out) period     - timer period
 *      (in/out) tx_enable  - >0 if timer enabled
 *   Returns
 *       BCM_E_*
 */
int
_bcm_tcal_period_get(int unit, tcal_id_t tcal_id, int *period, int *tx_enable)
{
    soc_sbx_g2p3_oamtimercalendar_t tcal;
    int status = BCM_E_NONE;
    int num_tcal_entries = 0, i;
    int total_period = 0, transmit_enable = 0;

    uint16 rec_list[3] = 
        { TCAL_INVALID_REC, TCAL_INVALID_REC, TCAL_INVALID_REC };

    if (!period) {
        status = BCM_E_PARAM;
    } else {
        status = _tcal_records_find(unit, tcal_id, &num_tcal_entries, rec_list);
        if (BCM_SUCCESS(status)) {
            for (i=0; i<num_tcal_entries; i++) {
                status = soc_sbx_g2p3_oamtimercalendar_get(unit, rec_list[i], &tcal);
                if (BCM_FAILURE(status)) {
                    status = BCM_E_INTERNAL;
                } else {
                    total_period += tcal.xmitPeriod;
                    if(tcal.xmitCount) {
                        transmit_enable = 1;
                    }
                }
            }
        } else {
            status = BCM_E_INTERNAL;
        }
    }

    if(BCM_SUCCESS(status)) {
        *period    = total_period;
        *tx_enable = transmit_enable;
    }
    return status;
}
