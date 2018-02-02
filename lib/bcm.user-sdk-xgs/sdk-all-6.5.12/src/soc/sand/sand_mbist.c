/* $Id: sand_mbist.c,v  $
 * $Copyright: (c) 2017 Broadcom.
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


#define COMMAND_OFFSET ((unsigned)(commands - first_command))
uint32 mbist_get_uint32(int unit, uint32 *out_val, uint8  **commands,int32 *nof_commands)
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

/* 
 * test using a given MBIST script
 */
uint32
soc_sand_run_mbist_script(
    const int unit, 
    const sand_mbist_script_t *script,  
    const sand_mbist_device_t *mbist_device, 
    sand_mbist_dynamic_t *dynamic
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
                SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "Write command exceeds script %s size %s%s\n", name, EMPTY, EMPTY);
            }
            script_line += val32;
            SHR_IF_ERR_EXIT(mbist_get_uint32(unit, &mask, &commands, &nof_commands));
            SHR_IF_ERR_EXIT(mbist_get_uint32(unit, &expected_value, &commands, &nof_commands));
            /* if mask ==0 then read command is actually dummy read command  intend to save the original line number
             * which was too big to save inside 14 bits 
             * and save it in the expected_value field whic is 32 bits long
            */
            if ((!mask)) {
                script_line = expected_value;
                break;
            }
            SHR_IF_ERR_EXIT(soc_reg32_get(unit, mbist_device->reg_tap_data_out, REG_PORT_ANY, 0, &val32)); /* read MBIST output */
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
            SHR_IF_ERR_EXIT(mbist_get_uint32(unit, &val32, &commands, &nof_commands));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, mbist_device->reg_tap_data_in, REG_PORT_ANY, 0, val32)); /* write MBIST input */
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
            SHR_IF_ERR_EXIT(mbist_get_uint32(unit, &val32, &commands, &nof_commands)); /* assumes SAND_MBIST_COMMAND_WAIT == 0 */
            if (val32 == SAND_MBIST_TEST_LONG_WAIT_VALUE) { /* Is this a special long sleep used for SER testing? */
                if (dynamic->ser_test_delay & SAND_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC) { /* a long sleep of the test specified in seconds or in useconds */
                    sal_sleep(dynamic->ser_test_delay & ~SAND_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC);
                } else {
                    sal_usleep(dynamic->ser_test_delay);
                }
            } else {
                sal_usleep((val32) / mbist_device->sleep_divisor); /* a regular MBIST sleep */
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
    const sand_mbist_device_t *mbist_device, 
    sand_mbist_dynamic_t *dynamic
    )
{
    SHR_FUNC_INIT_VARS(unit);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Memory BIST Started\n")));
    dynamic->measured_time = sal_time_usecs();
    SHR_IF_ERR_EXIT(soc_reg32_set(unit, mbist_device->reg_tap_command, REG_PORT_ANY, 0, 3));
    SHR_IF_ERR_EXIT(soc_reg32_set(unit, mbist_device->reg_tap_command, REG_PORT_ANY, 0, 2));
    SHR_IF_ERR_EXIT(soc_reg32_set(unit, mbist_device->reg_tap_command, REG_PORT_ANY, 0, 3));
    sal_usleep(10000);
exit:
    SHR_FUNC_EXIT;
}

/* function to initialize the MBIST mechanism */
uint32 soc_sand_mbist_init_new(
    const int unit, 
    const sand_mbist_device_t *mbist_device, 
    sand_mbist_dynamic_t *dynamic
    )
{
    uint32 val;
    SHR_FUNC_INIT_VARS(unit);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Memory BIST Started\n")));
    dynamic->measured_time = sal_time_usecs();
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, mbist_device->reg_tap_command, REG_PORT_ANY, 0, &val));
    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "MBIST status read val: 0x%x execute init: %d\n"), val, (val & 1) == 0));
    if ((val & 1) == 0) { /* configure MBIST CPU2TAP interface if not previously configured */
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, mbist_device->reg_tap_command, REG_PORT_ANY, 0, 2));
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, mbist_device->reg_tap_command, REG_PORT_ANY, 0, 3));
    }
    sal_usleep(10000);
exit:
    SHR_FUNC_EXIT;
}

/* function to de-initialize the MBIST mechanism */
uint32 soc_sand_mbist_deinit(
    const int unit, 
    const sand_mbist_device_t *mbist_device,  
    const sand_mbist_dynamic_t *dynamic
    )
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_reg32_set(unit, mbist_device->reg_tap_command, REG_PORT_ANY, 0, 2));
    if (!dynamic->ser_test_delay) { /* Do not report this this in SER testing iteration end */
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Memory BIST done in %u Microseconds\n"), sal_time_usecs() - dynamic->measured_time));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * De-initialize the MBIST mechanism, not resetting the CPU2TAP interface.
 * We assume the MBIST scripts leave regular memory access working.
 */
uint32 soc_sand_mbist_deinit_new(
    const int unit, 
    const sand_mbist_device_t *mbist_device,  
    const sand_mbist_dynamic_t *dynamic
    )
{
    SHR_FUNC_INIT_VARS(unit);

    if (dynamic->skip_errors && dynamic->nof_errors) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "MBIST failed, with %u errors.\n"), dynamic->nof_errors));
        _func_rv = _SHR_E_FAIL;
    }
    if (!dynamic->ser_test_delay) { /* Do not report this this in SER testing iteration end */
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Memory BIST done in %u Microseconds\n"), sal_time_usecs() - dynamic->measured_time));
    }

    SHR_FUNC_EXIT;
}

/* Legacy function to de-initialize the MBIST mechanism, writing a zero value */
uint32 soc_sand_mbist_deinit_legacy(
    const int unit, 
    const sand_mbist_device_t *mbist_device,  
    const sand_mbist_dynamic_t *dynamic
    )
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_reg32_set(unit, mbist_device->reg_tap_command, REG_PORT_ANY, 0, 0));
    if (!dynamic->ser_test_delay) { /* Do not report this this in SER testing iteration end */
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Memory BIST done in %u Microseconds\n"), sal_time_usecs() - dynamic->measured_time));
    }
exit:
    SHR_FUNC_EXIT;
}


#undef _ERR_MSG_MODULE_NAME


