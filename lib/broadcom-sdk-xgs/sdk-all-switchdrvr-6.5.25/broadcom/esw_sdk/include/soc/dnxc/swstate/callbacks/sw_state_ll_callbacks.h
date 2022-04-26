/*
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef _SW_STATE_LL_CALLBACKS_H
#define _SW_STATE_LL_CALLBACKS_H

#include <sal/types.h>

typedef struct
{
    /*
     *  First entry-id in the priority group
     */
    uint32 entry_id_first;
    /*
     *  Last entry-id in the priority group
     */
    uint32 entry_id_last;

} DNX_TCAM_PRIO_LOCATION;

/**
 * \brief
 * Compare two input buffers using custom metric:        \n
 * If the first (buffer1) is equal to the second (buffer2)       \n
 *                                        then return zero.      \n
 * If the first (buffer1) is larger than the second (buffer2)            \n
 *                                        then return a positive number. \n
 * If the first (buffer1) is smaller than the second (buffer2)           \n
 *                                        then return a negative number. \n
 * \par DIRECT INPUT:
 *   \param [in] buffer1 -
 *     Buffer of uint8s to compare contents of with 'buffer2'.
 *   \param [in] buffer2 -
 *     Buffer of uint8s to compare contents of with 'buffer1'
 *   \param [in] size - TBD
 * \par INDIRECT INPUT:
 *   SWSTATE system
 * \par DIRECT OUTPUT:
 *   * None
 * \par INDIRECT OUTPUT:
 *   In case of error condition, goto exit with _SHR_E_MEMORY
 * \remark
 *   * None
 */
int32 tcam_sorted_list_cmp_priority(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size);

int32 sw_state_sorted_list_cmp(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size);

int32 sw_state_sorted_list_cmp32(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size);

#endif /* _SW_STATE_LL_CALLBACKS_H */
