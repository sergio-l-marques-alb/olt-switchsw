/** \file sw_state_htb_callbacks.c
 *
 * This file is used for storing sw state hash table callbacks.
 * callbacks are used in the hash table when it's being printed.
 * 
 * After implementing the callbacks here, you'll need to add
 * this callback in the hash table callbacks xml
 * "dnx_sw_state_hash_table.xml" in order to be able to give
 * the callback's name as a string to the .create() function as
 * part of the init_info input.
 * 
 * This design is not optimal in the sense that this file may
 * include multiple modules that are not related to each other.
 * 
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
#include <sal/core/libc.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/multithread_analyzer.h>
#ifdef DNXC_MTA_ENABLED
#include <soc/dnxc/swstate/auto_generated/types/multithread_analyzer_types.h>
#endif

int
dnx_algo_lif_mapping_htb_entry_print(
    int unit,
    void *key,
    void *data)
{
    int *mydata;
    int *mykey;

    mydata = (int *) data;
    mykey = (int *) key;

    DNX_SW_STATE_PRINT(unit, "[0x%x,0x%x] ", *mykey, *mydata);

    return 0;
}

int
sw_state_htb_example_entry_print(
    int unit,
    void *key,
    void *data)
{
    int *mydata;
    int *mykey;

    mydata = (int *) data;
    mykey = (int *) key;

    DNX_SW_STATE_PRINT(unit, "[%d,%d] ", *mykey, *mydata);

    return 0;
}


int
sw_state_htb_multithread_print(
    int unit,
    void *key,
    void *data)
{

#ifdef DNXC_MTA_ENABLED
    thread_access_log_entry_t *mykey;

    mykey = (thread_access_log_entry_t *) key;

    DNX_SW_STATE_PRINT(unit, "[%s,%s,%s,%s,%s,%s] ",
                    dnxc_multithread_analyzer_thread_name_get(unit, mykey->thread_id),
                    dnxc_multithread_analyzer_resource_type_name_get(mykey->resource_type),
                    dnxc_multithread_analyzer_resource_id_name_get(unit, mykey->resource_type, mykey->resource_id),
                    mykey->is_write ? "write" : "read", mykey->function_name,
                    dnxc_multithread_analyzer_mutex_list_get(mykey->mutex_name));
#else
    DNX_SW_STATE_PRINT(unit, "should never be called\n");
#endif


    return 0;
}

int
kbp_ipv4_fwd_tcam_access_mapping_htb_entry_print(
    int unit,
    void *key,
    void *data)
{
    int *mydata;
    int *mykey;

    mydata = (int *) data;
    mykey = (int *) key;

    DNX_SW_STATE_PRINT(unit, "[0x%x,0x%x] ", *mykey, *mydata);

    return 0;
}

int
kbp_ipv6_fwd_tcam_access_mapping_htb_entry_print(
    int unit,
    void *key,
    void *data)
{
    int *mydata;
    int *mykey;

    mydata = (int *) data;
    mykey = (int *) key;

    DNX_SW_STATE_PRINT(unit, "[0x%x,0x%x] ", *mykey, *mydata);

    return 0;
}


#undef _ERR_MSG_MODULE_NAME


