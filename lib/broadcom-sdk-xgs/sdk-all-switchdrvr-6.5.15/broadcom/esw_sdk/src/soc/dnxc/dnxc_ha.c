/** \file dnxc_ha.c
 * General HA routines.
 */
/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/types.h>
#include <sal/core/alloc.h>
#include <sal/core/sync.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <soc/dnxc/dnxc_ha.h>

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>


#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#define DNXC_ALLOW_HW_WRITE_TIME_OUT            (10000000)      /* 10 sec */

typedef struct
{
    sal_thread_t thread_id[DNXC_HA_ALLOW_NOF];
    sal_mutex_t lock[DNXC_HA_ALLOW_NOF];
    uint32 counter[DNXC_HA_ALLOW_NOF];
} dnxc_allow_write_t;

dnxc_allow_write_t *dnxc_allow_write_db[SOC_MAX_NUM_DEVICES];

char *
dnxc_ha_access_type_str(
    dnxc_ha_allow_access_e access_type)
{
    static char *access_type_str[] = { "dbal", "swstate", "schan" };
    assert(access_type < DNXC_HA_ALLOW_NOF);
    return access_type_str[access_type];
}

char *
dnxc_ha_mutex_name_str(
    dnxc_ha_allow_access_e access_type)
{
    static char *mutex_name_str[] = { "HaAllowWriteDbal", "HaAllowWriteSwState", "HaAllowWriteSchan" };
    assert(access_type < DNXC_HA_ALLOW_NOF);
    return mutex_name_str[access_type];
}

int
dnxc_ha_init(
    int unit)
{
    int i = 0;
    SHR_FUNC_INIT_VARS(unit);

    dnxc_allow_write_db[unit] = sal_alloc(sizeof(dnxc_allow_write_t), "dnxc_allow_write_db");
    SHR_NULL_CHECK(dnxc_allow_write_db[unit], SOC_E_RESOURCE, "dnxc_allow_write_db");
    sal_memset(dnxc_allow_write_db[unit], 0x0, sizeof(dnxc_allow_write_t));

    for (i = 0; i < DNXC_HA_ALLOW_NOF; i++)
    {
        dnxc_allow_write_db[unit]->lock[i] = sal_mutex_create(dnxc_ha_mutex_name_str(i));
        SHR_NULL_CHECK(dnxc_allow_write_db[unit]->lock[i], SOC_E_RESOURCE, "dnxc_allow_write_db mutex");

        dnxc_allow_write_db[unit]->thread_id[i] = SAL_THREAD_ERROR;
        dnxc_allow_write_db[unit]->counter[i] = 0;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
dnxc_ha_deinit(
    int unit)
{
    int i = 0;
    char tname;
    SHR_FUNC_INIT_VARS(unit);

    if (dnxc_allow_write_db[unit] != NULL)
    {
        for (i = 0; i < DNXC_HA_ALLOW_NOF; i++)
        {
            if (dnxc_allow_write_db[unit]->counter[i] > 0)
            {
                LOG_ERROR(BSL_LS_SWSTATEDNX_GENERAL,
                          (BSL_META_U
                           (unit, "dnxc_ha_deinit destroying mutex while locked: thread=%s access=%s counter=%d\n"),
                           sal_thread_name(dnxc_allow_write_db[unit]->thread_id[i], &tname, 1),
                           dnxc_ha_access_type_str(i), dnxc_allow_write_db[unit]->counter[i]));
                assert(dnxc_allow_write_db[unit]->counter[i] == 0);
            }

            dnxc_allow_write_db[unit]->thread_id[i] = SAL_THREAD_ERROR;
            sal_mutex_destroy(dnxc_allow_write_db[unit]->lock[i]);
        }

        SHR_FREE(dnxc_allow_write_db[unit]);
        SHR_EXIT();
    }
exit:
    SHR_FUNC_EXIT;
}

int
dnxc_ha_is_access_disabled(
    int unit,
    dnxc_ha_allow_access_e access_type)
{
    /*
     * If thread has temporary permission to write, return True 
     */
    if (dnxc_allow_write_db[unit]->thread_id[access_type] == sal_thread_self())
    {
        return FALSE;
    }

    if (SOC_WARM_BOOT(unit))
    {
        return TRUE;
    }

    if (SOC_IS_DETACHING(unit))
    {
        return TRUE;
    }

    return FALSE;
}

int
dnxc_ha_tmp_allow_access_enable(
    int unit,
    dnxc_ha_allow_access_e access_type)
{
    char tname;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * check that unit is valid 
     */
    SHR_RANGE_VERIFY(unit, 0, SOC_MAX_NUM_DEVICES, SOC_E_UNIT, "wrong unit");

    /*
     * if allow_access was not initialized, exit 
     */
    if (!dnxc_allow_write_db[unit])
    {
        SHR_EXIT();
    }

    if (dnxc_allow_write_db[unit]->thread_id[access_type] == sal_thread_self())
    {
        /*
         * since locked and I'm the thread who locked it I need to ++counter 
         */
        dnxc_allow_write_db[unit]->counter[access_type]++;
        SHR_EXIT();
    }

    /*
     * take the mutex 
     */
    if (sal_mutex_take(dnxc_allow_write_db[unit]->lock[access_type], DNXC_ALLOW_HW_WRITE_TIME_OUT))
    {
        LOG_ERROR(BSL_LS_SWSTATEDNX_GENERAL, (BSL_META_U(unit, "Failed to take dnxc_allow_write_mutex.\n")));
        SHR_IF_ERR_EXIT(SOC_E_INTERNAL);
    }


    dnxc_allow_write_db[unit]->counter[access_type] = 1;
    dnxc_allow_write_db[unit]->thread_id[access_type] = sal_thread_self();

    SHR_EXIT();
exit:
    LOG_DEBUG(BSL_LS_SWSTATEDNX_GENERAL,
              (BSL_META_U(unit, "dnxc_ha_tmp_allow_access_enable: thread=%s access=%s counter=%d\n"),
               sal_thread_name(sal_thread_self(), &tname, 1), dnxc_ha_access_type_str(access_type),
               dnxc_allow_write_db[unit]->counter[access_type]));
    SHR_FUNC_EXIT;
}

int
dnxc_ha_tmp_allow_access_disable(
    int unit,
    dnxc_ha_allow_access_e access_type)
{
    char tname;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * check that unit is valid 
     */
    SHR_RANGE_VERIFY(unit, 0, SOC_MAX_NUM_DEVICES, SOC_E_UNIT, "wrong unit");

    if (dnxc_allow_write_db[unit]->thread_id[access_type] == sal_thread_self())
    {
        SHR_RANGE_VERIFY(dnxc_allow_write_db[unit]->counter[access_type], 1, 8, SOC_E_INTERNAL,
                         "bad value for allow access counter");
        dnxc_allow_write_db[unit]->counter[access_type]--;

        if (dnxc_allow_write_db[unit]->counter[access_type] != 0)
        {
            SHR_EXIT();
        }

        /*
         * mark dnxc_allow_write_thread_id as empty 
         */
        dnxc_allow_write_db[unit]->thread_id[access_type] = SAL_THREAD_ERROR;

        /*
         * give away the mutex 
         */
        if (sal_mutex_give(dnxc_allow_write_db[unit]->lock[access_type]))
        {
            LOG_ERROR(BSL_LS_SWSTATEDNX_GENERAL, (BSL_META_U(unit, "Failed to release dnxc_allow_write_mutex.\n")));
            SHR_IF_ERR_EXIT(SOC_E_INTERNAL);
        }
    }

    SHR_EXIT();
exit:

    LOG_DEBUG(BSL_LS_SWSTATEDNX_GENERAL,
              (BSL_META_U(unit, "dnxc_ha_tmp_allow_access_disable: thread=%s access=%s counter=%d\n"),
               sal_thread_name(sal_thread_self(), &tname, 1), dnxc_ha_access_type_str(access_type),
               dnxc_allow_write_db[unit]->counter[access_type]));
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
