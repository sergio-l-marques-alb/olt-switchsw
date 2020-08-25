/* $Id: sand_cpu2tap.c,v  $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
#include <shared/bsl.h>
#define BSL_LOG_MODULE BSL_LS_SOC_MBIST
#include <shared/shrextend/shrextend_debug.h>

#include <shared/bsl.h>
#include <soc/sand/sand_mbist.h>
#include <soc/drv.h>
#include <soc/memory.h>
#include <soc/mem.h>



#define MBIST_toPauseIR 0
#define MBIST_toPauseDR 0x40000000
#define MBIST_toPauseRTI     0x80000000

#define IR  0x1
#define DR  0x2
#define RTI 0x4


#define COMMAND_OFFSET ((unsigned)(commands - first_command))

#ifdef BCM_DNX_SUPPORT
static sal_mutex_t cpu2tap_mutexes[SOC_MAX_NUM_DEVICES] = {0};

/* Take the cpu2tap mutex at the start of a cpu2tap init function, creating the mutex if needed */
#define CPU2TAP_TAKE_MUTEX_AT_INIT(unit) \
if (SOC_IS_DNX(unit)) { \
    if (cpu2tap_mutexes[unit] == NULL) { /* If a cpu2tap mutex was not created for the device */ \
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "cpu2tap not initialized%s%s%s\n", EMPTY, EMPTY, EMPTY); \
    } else { /* take the mutex */ \
        SHR_IF_ERR_EXIT(sal_mutex_take(cpu2tap_mutexes[unit], sal_mutex_FOREVER)); \
    } \
}

/* Error handling end of a cpu2tap init function after exit:, giving the mutex if needed */
#define CPU2TAP_ERROR_HANDLING_AT_INIT_EXIT(unit) \
if (_func_rv != _SHR_E_NONE && SOC_IS_DNX(unit) && cpu2tap_mutexes[unit]) { \
    sal_mutex_give(cpu2tap_mutexes[unit]); \
}

/* free cpu2tap mutex of the unit if needed */
#define CPU2TAP_GIVE_MUTEX_AT_DEINIT_EXIT(unit) \
if (SOC_IS_DNX(unit)) { \
    if (cpu2tap_mutexes[unit] == NULL || sal_mutex_give(cpu2tap_mutexes[unit]) != SOC_E_NONE) { \
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Mutex give failed\n"))); \
        _func_rv = SOC_E_FAIL; \
    } \
}

/* Init the CPU2TAP mechanism for a given unit */
shr_error_e sand_init_cpu2tap(
    const  int unit
)
{
    SHR_FUNC_INIT_VARS(unit);
    if (cpu2tap_mutexes[unit] == NULL) {
        if ((cpu2tap_mutexes[unit] = sal_mutex_create("cpu2tap")) == NULL) {
            SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "Failed to create cpu2tap mutex%s%s%s\n", EMPTY, EMPTY, EMPTY);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/* De-init the CPU2TAP mechanism for a given unit */
shr_error_e sand_deinit_cpu2tap(
    const  int unit
)
{
    SHR_FUNC_INIT_VARS(unit);
    if (cpu2tap_mutexes[unit] != NULL) {
        SHR_IF_ERR_EXIT(sal_mutex_take(cpu2tap_mutexes[unit], sal_mutex_FOREVER));
        sal_mutex_destroy(cpu2tap_mutexes[unit]);
        cpu2tap_mutexes[unit] = NULL;
    }
exit:
    SHR_FUNC_EXIT;
}

#else /* BCM_DNX_SUPPORT */

#define CPU2TAP_TAKE_MUTEX_AT_INIT(unit)
#define CPU2TAP_ERROR_HANDLING_AT_INIT_EXIT(unit)
#define CPU2TAP_GIVE_MUTEX_AT_DEINIT_EXIT(unit)

#endif /* BCM_DNX_SUPPORT */

static uint32 cpu2tap_get_uint32(int unit, uint32 *out_val, uint8  **commands,int32 *nof_commands)
{
    SHR_FUNC_INIT_VARS(unit);
    if (*nof_commands < 4) {
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "not enough data left for uint32 %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
    *out_val = (*(*commands)++) << 24;
    *out_val |= (*(*commands)++) << 16;
    *out_val |= (*(*commands)++) << 8;
    *out_val |= *((*commands)++);
    *nof_commands -= 4;
exit:
    SHR_FUNC_EXIT; 
}

/* run an MBIST test script and do not stop on errors */
uint32
soc_sand_run_cpu2tap_script_no_stop(
    const int unit, 
    const sand_cpu2tap_script_t *script,  
    const sand_cpu2tap_device_t *cpu2tap_device, 
    sand_cpu2tap_dynamic_t *dynamic,
    uint32 *output_values /* values returned by the script */
    )
{
    uint32 rv;
    uint8 stored_skip_errors = dynamic->skip_errors;
    dynamic->skip_errors = 1;
    rv = soc_sand_run_cpu2tap_script(unit, script, cpu2tap_device, dynamic, NULL);
    dynamic->skip_errors = stored_skip_errors;
    return rv;
}

/* 
 * test using a given MBIST script
 */
uint32
soc_sand_run_mbist_script(
    const int unit, 
    const sand_cpu2tap_script_t *script,  
    const sand_cpu2tap_device_t *cpu2tap_device, 
    sand_cpu2tap_dynamic_t *dynamic
    )
{
    return soc_sand_run_cpu2tap_script(unit, script, cpu2tap_device, dynamic, NULL);
}

/* 
 * test using a given MBIST/CPU2TAP script, possibly returning output from CPU2TAP
 */
uint32
soc_sand_run_cpu2tap_script(
    const int unit, 
    const sand_cpu2tap_script_t *script,  
    const sand_cpu2tap_device_t *cpu2tap_device, 
    sand_cpu2tap_dynamic_t *dynamic,
    uint32 *output_values /* values returned by the script */
    )
{
    uint8 *first_command  = (uint8 *)script->commands;
    const char **comment_ptr = script->comments;
    const char *name, *last_comment = "";
#ifdef _MBIST_PRINT_TWO_COMMENTS
    const char *prev_comment = "";
#endif
    uint8 *commands = (uint8 *)script->commands;
    uint32 script_line = 0, val32, expected_value, mask, err_val;
    int32 nof_commands = script->nof_commands;
    uint32 nof_comments = script->nof_comments;
    unsigned bit;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(script, _SHR_E_PARAM, "script");
    name = script->script_name;

    if (!first_command || !comment_ptr || !name) {
        SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "invalid MBIST script %s%s%s\n", EMPTY, EMPTY, EMPTY);
    } else if (dynamic->skip_errors && !dynamic->ser_test_delay) {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "MBIST script %s starting after %u us\n"), name, sal_time_usecs() - dynamic->measured_time));
    } else {
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "MBIST script %s starting after %u us\n"), name, sal_time_usecs() - dynamic->measured_time));
    }

    while (nof_commands > 0) {
        --nof_commands;
        switch (SAND_MBIST_COMMAND_MASK & *commands) {
        case SAND_MBIST_COMMAND_READ:
            val32 = (SAND_MBIST_COMMAND_INVMASK & *(commands++)) << 8; /* assumes SAND_MBIST_COMMAND_READ == 0 */
            val32 += *(commands++);
            if (nof_commands-- < 9) {
                SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "Read command exceeds script %s size %s%s\n", name, EMPTY, EMPTY);
            }
            script_line += val32;
            SHR_IF_ERR_EXIT(cpu2tap_get_uint32(unit, &mask, &commands, &nof_commands));
            SHR_IF_ERR_EXIT(cpu2tap_get_uint32(unit, &expected_value, &commands, &nof_commands));
            /* if mask ==0 then read command is actually dummy read command  intend to save the original line number
             * which was too big to save inside 14 bits 
             * and save it in the expected_value field whic is 32 bits long
            */
            if ((!mask)) {
                script_line = expected_value;
                break;
            }
            SHR_IF_ERR_EXIT(soc_reg32_get(unit, cpu2tap_device->reg_tap_data_out, REG_PORT_ANY, 0, &val32)); /* read MBIST output */

            /* check if a returned value is requested */
            if ((expected_value & ~mask) == ~mask) {
                expected_value &= mask;
                if (output_values == NULL) {
                    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                      "Can not return CPU2tap value when output pointer was not provided at %s:%u\n"),
                      name, script_line));
                    SHR_EXIT();
                }
                *(output_values++) = val32 & ~mask; /* return the non constant part of the read value */
            }

            if ((err_val = (val32 & mask) ^ expected_value) != 0) { /* check the success of the given test */
                for (bit = 0; (err_val & 1) == 0; ++bit, err_val >>= 1) ; /* calculate error bit */
#ifdef _MBIST_PRINT_TWO_COMMENTS
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                          "MBIST failure at %s:%u expected:0x%x read:0x%x mask:0x%x error bit:%u script comments:\n%s\n%s\n"),
                          name, script_line, expected_value, val32, mask, bit, prev_comment, last_comment));
#else
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                          "MBIST failure at %s:%u expected:0x%x read:0x%x mask:0x%x error bit:%u script comment:\n%s\n"),
                          name, script_line, expected_value, val32, mask, bit, last_comment));
#endif
                ++dynamic->nof_errors;
                if (!dynamic->skip_errors) {
                    _func_rv = _SHR_E_FAIL;
                    SHR_EXIT();
                }
            }
            break;
        case SAND_MBIST_COMMAND_WRITE:
            if (*(commands++) != SAND_MBIST_COMMAND_WRITE) {
                SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "invalid write command in script %s offset %u %s\n", name, COMMAND_OFFSET, EMPTY);
            }
            SHR_IF_ERR_EXIT(cpu2tap_get_uint32(unit, &val32, &commands, &nof_commands));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, cpu2tap_device->reg_tap_data_in, REG_PORT_ANY, 0, val32)); /* write MBIST input */
            sal_usleep(script->sleep_after_write);
            break;
        case SAND_MBIST_COMMAND_COMMENT:
            if (*(commands++) != SAND_MBIST_COMMAND_COMMENT) {
                SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "invalid comment command in script %s offset %u %s\n", name, COMMAND_OFFSET, EMPTY);
            }
            if (!nof_comments--) {
                SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "comments number mismatch in script %s %s%s\n", name, EMPTY, EMPTY);
            }
#ifdef _MBIST_PRINT_TWO_COMMENTS
            prev_comment = last_comment;
#endif
            last_comment = *(comment_ptr++);
            break;
        case SAND_MBIST_COMMAND_WAIT:
            ++nof_commands;
            SHR_IF_ERR_EXIT(cpu2tap_get_uint32(unit, &val32, &commands, &nof_commands)); /* assumes SAND_MBIST_COMMAND_WAIT == 0 */
            if (val32 == SAND_MBIST_TEST_LONG_WAIT_VALUE) { /* Is this a special long sleep used for SER testing? */
                if (dynamic->ser_test_delay & SAND_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC) { /* a long sleep of the test specified in seconds or in useconds */
                    sal_sleep(dynamic->ser_test_delay & ~SAND_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC);
                } else {
                    sal_usleep(dynamic->ser_test_delay);
                }
            } else {
                sal_usleep((val32) / cpu2tap_device->sleep_divisor); /* a regular MBIST sleep */
            }
            break;
        default:
            SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "internal case error in MBIST %s%s%s\n", EMPTY, EMPTY, EMPTY);
        }

    }

    if (nof_commands != 0) {
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "Exceeded script %s size %s%s\n", name, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/* Legacy function to initialize the MBIST mechanism */
uint32 soc_sand_mbist_init_legacy(
    const int unit, 
    const sand_cpu2tap_device_t *cpu2tap_device, 
    sand_cpu2tap_dynamic_t *dynamic
    )
{
    SHR_FUNC_INIT_VARS(unit);
    CPU2TAP_TAKE_MUTEX_AT_INIT(unit) /* take unit's cpu2tap mutex */
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Memory BIST Started\n")));
    dynamic->measured_time = sal_time_usecs();
    SHR_IF_ERR_EXIT(soc_reg32_set(unit, cpu2tap_device->reg_tap_command, REG_PORT_ANY, 0, 3));
    SHR_IF_ERR_EXIT(soc_reg32_set(unit, cpu2tap_device->reg_tap_command, REG_PORT_ANY, 0, 2));
    SHR_IF_ERR_EXIT(soc_reg32_set(unit, cpu2tap_device->reg_tap_command, REG_PORT_ANY, 0, 3));
    sal_usleep(10000);
exit:
    CPU2TAP_ERROR_HANDLING_AT_INIT_EXIT(unit)
    SHR_FUNC_EXIT;
}

/* function to initialize the MBIST mechanism */
uint32 soc_sand_mbist_init_new(
    const int unit, 
    const sand_cpu2tap_device_t *cpu2tap_device, 
    sand_cpu2tap_dynamic_t *dynamic
    )
{
    uint32 val;
    SHR_FUNC_INIT_VARS(unit);
    CPU2TAP_TAKE_MUTEX_AT_INIT(unit) /* take unit's cpu2tap mutex */
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Memory BIST Started\n")));
    dynamic->measured_time = sal_time_usecs();
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, cpu2tap_device->reg_tap_command, REG_PORT_ANY, 0, &val));
    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "MBIST status read val: 0x%x execute init: %d\n"), val, (val & 1) == 0));
    if ((val & 1) == 0) { /* configure MBIST CPU2TAP interface if not previously configured */
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, cpu2tap_device->reg_tap_command, REG_PORT_ANY, 0, 2));
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, cpu2tap_device->reg_tap_command, REG_PORT_ANY, 0, 3));
        sal_usleep(10000);
    }
exit:
    CPU2TAP_ERROR_HANDLING_AT_INIT_EXIT(unit)
    SHR_FUNC_EXIT;
}

/* function to de-initialize the MBIST mechanism */
uint32 soc_sand_mbist_deinit(
    const int unit, 
    const sand_cpu2tap_device_t *cpu2tap_device,  
    const sand_cpu2tap_dynamic_t *dynamic
    )
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_reg32_set(unit, cpu2tap_device->reg_tap_command, REG_PORT_ANY, 0, 2));
    if (!dynamic->ser_test_delay) { /* Do not report this this in SER testing iteration end */
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Memory BIST done in %u Microseconds\n"), sal_time_usecs() - dynamic->measured_time));
    }
exit:
    CPU2TAP_GIVE_MUTEX_AT_DEINIT_EXIT(unit) /* give unit's cpu2tap mutex */
    SHR_FUNC_EXIT;
}

/*
 * De-initialize the MBIST mechanism, not resetting the CPU2TAP interface.
 * We assume the MBIST scripts leave regular memory access working.
 */
uint32 soc_sand_mbist_deinit_new(
    const int unit,
    const uint32 flags,
    const sand_cpu2tap_device_t *cpu2tap_device,  
    const sand_cpu2tap_dynamic_t *dynamic
    )
{
    SHR_FUNC_INIT_VARS(unit);

    if (dynamic->skip_errors && dynamic->nof_errors) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "MBIST failed, with %u errors.\n"), dynamic->nof_errors));
        _func_rv = _SHR_E_FAIL;
    }

    if ((flags & SAND_MBIST_FLAG_SUPPRESS_MESSAGE) == 0)
    {
        if (!dynamic->ser_test_delay) { /* Do not report this this in SER testing iteration end */
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Memory BIST done in %u Microseconds\n"), sal_time_usecs() - dynamic->measured_time));
        }
    }

    CPU2TAP_GIVE_MUTEX_AT_DEINIT_EXIT(unit) /* give unit's cpu2tap mutex */
    SHR_FUNC_EXIT;
}

/* Legacy function to de-initialize the MBIST mechanism, writing a zero value */
uint32 soc_sand_mbist_deinit_legacy(
    const int unit, 
    const sand_cpu2tap_device_t *cpu2tap_device,  
    const sand_cpu2tap_dynamic_t *dynamic
    )
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_reg32_set(unit, cpu2tap_device->reg_tap_command, REG_PORT_ANY, 0, 0));
    if (!dynamic->ser_test_delay) { /* Do not report this this in SER testing iteration end */
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Memory BIST done in %u Microseconds\n"), sal_time_usecs() - dynamic->measured_time));
    }
exit:
    CPU2TAP_GIVE_MUTEX_AT_DEINIT_EXIT(unit) /* give unit's cpu2tap mutex */
    SHR_FUNC_EXIT;
}
#undef _ERR_MSG_MODULE_NAME


