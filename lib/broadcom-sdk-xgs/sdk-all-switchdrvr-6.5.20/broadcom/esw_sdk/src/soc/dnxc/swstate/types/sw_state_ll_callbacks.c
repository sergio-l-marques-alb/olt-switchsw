/** \file sw_state_ll_callbacks.c
 *
 * This file is used for storing sw state linked list callbacks.
 * callbacks are used in the ll when it's a sorted linked list.
 * the callback will implement a compare function that
 * implements the sorting condition.
 * 
 * After implementing the callbacks here, you'll need to add
 * this callback in the link list callbacks xml
 * "dnx_sw_state_sorted_list.xml" in order to be able to give
 * the callback's name as a string to the .create() function as
 * part of the init_info input.
 * 
 * This design is not optimal in the sense that this file may
 * include multiple modules that are not related to each other.
 * 
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX

#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <shared/utilex/utilex_framework.h>
#include <sal/core/libc.h>
#include <assert.h>

#define DNX_TCAM_DB_LIST_KEY_SIZE                               (sizeof(uint32))
#define DNX_TCAM_DB_LIST_DATA_SIZE                              (sizeof(DNX_TCAM_PRIO_LOCATION))

/**
 * \brief
 *  Clear a structure which describes a 'priority group' (set of entries
 *  with the same priority). See remarks.
 * \par DIRECT INPUT
 *   \param [in] info -
 *     Pointer to structure of type DNX_TCAM_PRIO_LOCATION.
 *     \b As \b output -\n
 *     This procedure clears this structure (which is the 'priority group').
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See 'info' in DIRECT INPUT
 * \remarks:
 *   * 'Priority group' is loaded into 'data' of entries on Sorted List' so
 *     that even if two entries have the same priority, they may still differ
 *     by the 'priority group'.
 * \see
 *   * utilex_sorted_list_tcam_cmp_priority()
 */
static shr_error_e
dnx_tcam_prio_location_clear(
    DNX_TCAM_PRIO_LOCATION * info)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);

    SHR_NULL_CHECK(info, _SHR_E_INTERNAL, "info");
    sal_memset(info, 0x0, sizeof(*info));
exit:
    SHR_FUNC_EXIT;
}

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
 *     Comparison is not necessarily straightforward. See 'size'
 *     below.
 *   \param [in] buffer2 -
 *     Buffer of uint8s to compare contents of with 'buffer1'
 *     Comparison is not necessarily straightforward. See 'size'
 *     below.
 *   \param [in] size -
 *     Number of bytes in each of the 'buffer's specified above.
 *     This number is also used for selection of the 'compare'
 *     algorithm: \n
 *     If 'size' is \ref DNX_TCAM_DB_LIST_KEY_SIZE then the
 *     bytes in each buffer are converted to uint32 and
 *     the results are compared. \n
 *     If 'size' is \ref DNX_TCAM_DB_LIST_DATA_SIZE then the
 *     bytes in each buffer are converted to structure of
 *     type DNX_TCAM_PRIO_LOCATION. This structure stands
 *     for a TCAM priority group. Two peiority groups ar
 *     consideref equal only if the entry ID of the first and
 *     the entry ID of the last are the same.
 * \par INDIRECT INPUT:
 *   SWSTATE system
 * \par DIRECT OUTPUT:
 *   * None
 * \par INDIRECT OUTPUT:
 *   In case of error condition, goto exit with _SHR_E_MEMORY
 * \remark
 *   Originally, this procedure was custom tailored to TCAM of ARAD.
 *   Its porting requires device-specific definitions (e.g., arad_tcam.h)
 *   and device-specific code (e.g., arad_tcam.c)  \n
 *   For now, TEMPORARILY, we add local definitions and tailor it
 *   to DNX.
 */
int32
tcam_sorted_list_cmp_priority(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size)
{
    uint32 prio_1, prio_2;
    DNX_TCAM_PRIO_LOCATION prio_location_1, prio_location_2;

    if ((buffer1 == NULL) || (buffer2 == NULL))
    {
        /*
         * Not supposed to compare NULL pointers 
         */
        assert(0);
    }
    /*
     * Key comparison 
     */
    if (size == DNX_TCAM_DB_LIST_KEY_SIZE)
    {
        prio_1 = *((uint32*)buffer1);
        prio_2 = *((uint32*)buffer2);
        return (((int32) prio_1) - ((int32) prio_2));
    }
    else if (size == DNX_TCAM_DB_LIST_DATA_SIZE)
    {   /* Data comparison */
        dnx_tcam_prio_location_clear(&prio_location_1);
        dnx_tcam_prio_location_clear(&prio_location_2);
        /*
         * buffer1 and buffer2 can't be null. It has been checked above 
         */
        /* coverity[var_deref_model:FALSE] */
        sal_memcpy(&prio_location_1, buffer1, sizeof(DNX_TCAM_PRIO_LOCATION));
        /* coverity[var_deref_model:FALSE] */
        sal_memcpy(&prio_location_2, buffer2, sizeof(DNX_TCAM_PRIO_LOCATION));
        return ((prio_location_1.entry_id_first !=
                 prio_location_2.entry_id_first)
                || (prio_location_1.entry_id_last != prio_location_2.entry_id_last)) ? 1 : 0;
    }
    else
    {
        /*
         * Unknown input formats 
         */
        assert(0);
        return 0;
    }
}

int32
sw_state_sorted_list_cmp(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size)
{
    /* the assumption is that the key's type is uint8 */
    int32 ret_val = 0; 
    uint8 value1 = *buffer1;
    uint8 value2 = *buffer2;
    ret_val += value1;
    ret_val -= value2;
    return ret_val;
}

int32
sw_state_sorted_list_cmp32(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size)
{
    /* the assumption is that the key's type is uint8 */
    int32 ret_val = 0;
    uint32 value1 = *((uint32 *)buffer1);
    uint32 value2 = *((uint32 *)buffer2);
    ret_val += value1;
    ret_val -= value2;
    return ret_val;
}

#undef _ERR_MSG_MODULE_NAME


