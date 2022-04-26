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
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
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
int32
tcam_sorted_list_cmp_priority(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size)
{
    uint32 prio_1, prio_2;
    uint8 handler_1, handler_2;

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
    prio_1 = *((uint32*)buffer1);
    /* Handler_ID is located after priority inside the key */
    handler_1 = *(buffer1+sizeof(uint32));
    prio_2 = *((uint32*)buffer2);
    handler_2 = *(buffer2+sizeof(uint32));
    return (handler_1 == handler_2)? (prio_1 - prio_2) : (handler_1 - handler_2);
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


