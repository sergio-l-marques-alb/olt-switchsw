/* AAPL CORE Revision: master
 *
 * Copyright (c) 2014-2021 Avago Technologies. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


/** Doxygen File Header
 ** @file
 ** @brief Core AAPL functions.
 **/

#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include "aapl.h"
#include "asic_info.h"

#define AAPL_LSB_IGNORE (-1)
#define AAPL_LSB_INIT (-2)

const char *aapl_default_server = "localhost";
const uint  aapl_default_port   = 90;
#if AAPL_ENABLE_AACS_SERVER
extern BOOL aapl_sbus_rings_override;
extern BOOL aapl_num_chips_override;
#endif

#if AAPL_ALLOW_GPIO_JTAG
static BOOL avago_bit_banged_gpio_jtag_fn(Aapl_t *aapl, BOOL tms, BOOL tdi, BOOL trst_l, BOOL get_tdo);
static int avago_bit_banged_gpio_jtag_open_fn(Aapl_t *aapl);
static int avago_bit_banged_gpio_jtag_close_fn(Aapl_t *aapl);
#endif

#if AAPL_ALLOW_THREAD_SUPPORT
static AAPL_THREAD_STORAGE int ip_lock_count = 0;
#endif


volatile int aapl_sigint_detected = 0;

#if AAPL_ALLOW_SIGNALS
static void aapl_signal_handler(int dummy)
{
    (void)dummy;
    aapl_sigint_detected ++;
    if (aapl_sigint_detected >= 2)
    {
        printf("AAPL caught %d unhandled instances of SIGINT and is now exiting.\n", aapl_sigint_detected);
        exit(-2);
    }
    signal(SIGINT, aapl_signal_handler);
}
#endif

void aapl_sigint_check(Aapl_t *aapl)
{
    (void) aapl;
#if AAPL_ALLOW_SIGNALS
    if (aapl_sigint_detected > 0)
    {
        aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "%d SIGINT detected. Attemping to close connections.\n", aapl_sigint_detected);
        aapl->return_code --;
#if AAPL_ALLOW_THREAD_SUPPORT
        AAPL_SPICO_INT_UNLOCK;
        AAPL_CLOSE_UNLOCK;
        AAPL_AACS_UNLOCK;
        AAPL_IP_INFO_UNLOCK;
        AAPL_SENSOR_UNLOCK;
        AAPL_SBUS_UNLOCK;

        pthread_mutex_unlock(&aapl->serdes_mem_mutex);
        pthread_mutex_unlock(&aapl->log_printf_mutex);

#endif
        if (aapl->cmds_buffered) avago_aacs_flush(aapl);
        if (aapl->socket != -1) aapl_close_connection(aapl);
        exit(-1);
    }
#endif
}


/** @brief   Checks if JTAG communication is used.
 ** @return  Returns TRUE if JTAG communication is used, FALSE otherwise.
 **/
AAPL_INLINE BOOL aapl_is_jtag_communication_method(
    Aapl_t *aapl)  /**< [in] Pointer to AAPL structure. */
{
    return aapl->communication_method == AVAGO_BB_GPIO_JTAG ||
           aapl->communication_method == AVAGO_BB_JTAG ||
           aapl->communication_method == AVAGO_JTAG;
}

#if AAPL_ALLOW_I2C || AAPL_ALLOW_SYSTEM_I2C
/** @brief   Checks if I2C communication is used.
 ** @return  Returns TRUE if I2C communication is used, FALSE otherwise.
 **/
BOOL aapl_is_i2c_communication_method(
    Aapl_t *aapl)  /**< [in] Pointer to AAPL structure. */
{
    return aapl->communication_method == AVAGO_I2C ||
           aapl->communication_method == AVAGO_SYSTEM_I2C;
}
#endif

#if AAPL_ALLOW_MDIO || AAPL_ALLOW_GPIO_MDIO
/** @brief   Checks if MDIO communication is used.
 ** @return  Returns TRUE if MDIO communication is used, FALSE otherwise.
 **/
BOOL aapl_is_mdio_communication_method(
    Aapl_t *aapl)  /**< [in] Pointer to AAPL structure. */
{
    return aapl->communication_method == AVAGO_MDIO ||
           aapl->communication_method == AVAGO_GPIO_MDIO;
}
#endif

/** @brief   Checks if SBus communication is used.
 ** @return  Returns TRUE if SBus communication is used, FALSE otherwise.
 **/
BOOL aapl_is_sbus_communication_method(
    Aapl_t *aapl)   /**< [in] Pointer to AAPL structure. */
{
    return aapl->communication_method == AVAGO_SBUS ||
           aapl->communication_method == AVAGO_OFFLINE;
}

/** @brief   Checks if AACS communication is used.
 ** @return  Returns TRUE if AACS communication is used, FALSE otherwise.
 **/
BOOL aapl_is_aacs_communication_method(
    Aapl_t *aapl)  /**< [in] Pointer to AAPL structure. */
{
    return aapl->aacs;
}

/** @cond INTERNAL */

void aapl_set_chip(Aapl_t *aapl, uint chipnum)
{
    if (chipnum == 0xff) return;
    if( chipnum != 0 && chipnum >= aapl->chips)
        aapl_fail(aapl, __func__, __LINE__, "Attempting to set current chip number to %d, however only %d chips are present.\n", chipnum, aapl->chips);
    else if( aapl->chips > 1 )
    {
        if( aapl->aacs )
        {
            char buf[16];
            snprintf(buf, 16, "chipnum %1u", chipnum);
            AAPL_SUPPRESS_ERRORS_PUSH(aapl);
            avago_aacs_send_command(aapl, buf);
            AAPL_SUPPRESS_ERRORS_POP(aapl);
        }
#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
        if( aapl->curr_chip != chipnum )
        {
            aapl->prev_opcode = 0;
            aapl->curr_chip = chipnum;
        }
#endif
    }
}

void aapl_set_chip_by_addr(Aapl_t *aapl, uint addr)
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr, &addr_struct);
    aapl_set_chip(aapl, addr_struct.chip);
}

/** @endcond */

#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
static char *avago_aacs_jtag_fn(
    Aapl_t *aapl,       /**< [in] Pointer to AAPL structure */
    int opcode,         /**< [in] The JTAG opcode. */
    int length,         /**< [in] The number of bits to be sent. */
    const char *tdi,    /**< [in] Data to be sent to scan chain. */
    BOOL get_tdo)       /**< [in] Is data back required? */
{
    int buf_size = length+1+32;
    char *buffer = 0;
    (void) get_tdo;

    buffer = (char *)aapl_malloc(aapl, buf_size, __func__);

    if (buffer)
    {
        snprintf(buffer, buf_size, "jtag %d %04x %.*s", length, opcode, length, tdi);
        avago_aacs_send_command(aapl, buffer);
        aapl_free(aapl, buffer, __func__);
    }
    return aapl->data_char;
}

static void avago_jtag_set_opcode(Aapl_t *aapl, int opcode)
{
    if( opcode != aapl->prev_opcode )
    {
        char op[64];
        aapl_hex_2_bin(op, opcode, 0, 10);
        avago_jtag_scan(aapl, 10, 2, op, NULL);
        aapl->prev_opcode = opcode;
    }
}

static char *avago_jtag_fn(
    Aapl_t *aapl,       /**< [in] Pointer to AAPL structure */
    int opcode,         /**< [in] The JTAG opcode. */
    int length,         /**< [in] The number of bits to be sent. */
    const char *tdi,    /**< [in] Data to be sent to scan chain. */
    BOOL get_tdo)       /**< [in] Is data back required? */
{
    char *buffer;
    if (get_tdo)
        buffer = (char *)aapl_malloc(aapl, length+1+32, __func__);
    else buffer = 0;

    avago_jtag_set_opcode(aapl, opcode);
    avago_jtag_scan(aapl, length, /* tms_cycles */ 1, tdi, buffer);
    aapl->data_char[0] = 0;

    if (buffer)
    {
        aapl_log_printf(aapl, AVAGO_DATA_CHAR, __func__, __LINE__, "%s", buffer);
        aapl_free(aapl, buffer, __func__);
    }
    return aapl->data_char;
}

/** @brief  Retrieves the jtag idcode from the device.
 ** @return Returns the jtag idcode value.
 **/
static uint avago_get_jtag_idcode_fn(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint device)    /**< [in] Device number */
{
    aapl_set_chip(aapl, device);
    avago_jtag(aapl, 0x02b6, 32, "00000000000000000000000000000000");
    return aapl_strtoul(aapl->data_char, NULL, 2);
}
#endif


/** @brief   Sets the value of the async_cancel flag in Aapl_t.
 ** @details The async_cancel flag is used to terminate long AAPL
 **          operations before they would otherwise return.  This is a
 **          cooperative cancel, so only certain operations can be canceled.
 **          The caller should clear this flag after use, before calling
 **          another cancelable function.
 ** @see     aapl_get_async_cancel_flag().
 ** @return  Returns the previous value of the flag.
 **/
int aapl_set_async_cancel_flag(Aapl_t *aapl, int new_value)
{
    int old_value = aapl->async_cancel;
    aapl->async_cancel = new_value;
    ms_sleep(1);
    aapl->async_cancel = new_value;
    return old_value;
}

/** @brief   Gets the current value of the async_cancel flag from Aapl_t.
 ** @see     aapl_set_async_cancel_flag().
 **/
int aapl_get_async_cancel_flag(Aapl_t *aapl)
{
    return aapl->async_cancel;
}

#define FUNCTION_STRING(addr,a) if( addr == (fn_cast_t)a ) { aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "%-22s: %-s\n", label, #a); return; }
static void print_function(Aapl_t *aapl, const char *label, fn_cast_t addr)
{
    if( addr == 0 ) return;
#if AAPL_ALLOW_AACS
    FUNCTION_STRING(addr, avago_aacs_open_fn);
    FUNCTION_STRING(addr, avago_aacs_close_fn);
#  if AAPL_ALLOW_I2C || AAPL_ALLOW_SYSTEM_I2C
    FUNCTION_STRING(addr, avago_aacs_i2c_read_fn);
    FUNCTION_STRING(addr, avago_aacs_i2c_write_fn);
#  endif
#  if AAPL_ALLOW_MDIO || AAPL_ALLOW_GPIO_MDIO
    FUNCTION_STRING(addr, avago_aacs_mdio_fn);
    FUNCTION_STRING(addr, aapl_aacs_sbus_fn);
#  endif
#if AAPL_ENABLE_BLACKHAWK
    FUNCTION_STRING(addr, avago_pmi_sbus_fn);
    FUNCTION_STRING(addr, avago_aacs_pmi_fn);
#endif
#endif

#if AAPL_ALLOW_OFFLINE_SBUS
    FUNCTION_STRING(addr, avago_offline_sbus_fn);
    FUNCTION_STRING(addr, avago_offline_open_fn);
    FUNCTION_STRING(addr, avago_offline_close_fn);
#endif

#if AAPL_ALLOW_SYSTEM_I2C
    FUNCTION_STRING(addr, avago_system_i2c_open_fn);
    FUNCTION_STRING(addr, avago_system_i2c_close_fn);
    FUNCTION_STRING(addr, avago_system_i2c_read_fn);
    FUNCTION_STRING(addr, avago_system_i2c_write_fn);
#endif
#if AAPL_ALLOW_I2C || AAPL_ALLOW_SYSTEM_I2C
    FUNCTION_STRING(addr, avago_i2c_sbus_fn);
#endif
#if AAPL_ALLOW_MDIO || AAPL_ALLOW_GPIO_MDIO
    FUNCTION_STRING(addr, avago_mdio_sbus_fn);
#endif
#if AAPL_ALLOW_GPIO_MDIO
    FUNCTION_STRING(addr, avago_gpio_mdio_open_fn);
    FUNCTION_STRING(addr, avago_gpio_mdio_close_fn);
    FUNCTION_STRING(addr, avago_gpio_mdio_fn);
    FUNCTION_STRING(addr, avago_pmi_mdio_fn);
    FUNCTION_STRING(addr, avago_pmi_sbus_fn);
#endif
#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
    FUNCTION_STRING(addr, avago_jtag_fn);
    FUNCTION_STRING(addr, avago_aacs_jtag_fn);
    FUNCTION_STRING(addr, avago_get_jtag_idcode_fn);
    FUNCTION_STRING(addr, avago_jtag_sbus_fn);
#endif
#if AAPL_ALLOW_GPIO_JTAG
    FUNCTION_STRING(addr, avago_bit_banged_gpio_jtag_open_fn);
    FUNCTION_STRING(addr, avago_bit_banged_gpio_jtag_close_fn);
    FUNCTION_STRING(addr, avago_bit_banged_gpio_jtag_fn);
#endif
    FUNCTION_STRING(addr, avago_serdes_spico_int_sbus_fn);
    FUNCTION_STRING(addr, avago_parallel_serdes_int_sbus_fn);
    FUNCTION_STRING(addr, avago_parallel_core_serdes_int_sbus_fn);
    FUNCTION_STRING(addr, avago_hard_sbus_reset_fn);
    FUNCTION_STRING(addr, aapl_built_in_logging_fn);
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "%-20s: function@0x%lx\n", label, addr);
}

#define PRINT_FUNCTION(aapl, member) print_function(aapl, #member, (fn_cast_t)aapl->member)
static void aapl_print_registration_info(Aapl_t *aapl)
{
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__,  "Communication method:      %s (AACS: %x)\n", aapl_comm_method_to_str(aapl->communication_method), aapl->aacs);
    PRINT_FUNCTION(aapl, comm_open_fn);
    PRINT_FUNCTION(aapl, comm_close_fn);
    PRINT_FUNCTION(aapl, sbus_fn);
    PRINT_FUNCTION(aapl, sbus_fn_2);
    PRINT_FUNCTION(aapl, jtag_fn);
    PRINT_FUNCTION(aapl, bit_banged_jtag_fn);
    PRINT_FUNCTION(aapl, jtag_idcode_fn);
    PRINT_FUNCTION(aapl, i2c_read_fn);
    PRINT_FUNCTION(aapl, i2c_write_fn);
    PRINT_FUNCTION(aapl, mdio_fn);
    PRINT_FUNCTION(aapl, serdes_int_fn);
    PRINT_FUNCTION(aapl, parallel_serdes_int_fn);
    PRINT_FUNCTION(aapl, hard_sbus_reset_fn);
    PRINT_FUNCTION(aapl, pmi_fn);
#if AAPL_ENABLE_ATE_VEC
    PRINT_FUNCTION(aapl, ate_vec_fn);
#endif
    PRINT_FUNCTION(aapl, log_fn);
    PRINT_FUNCTION(aapl, log_open_fn);
    PRINT_FUNCTION(aapl, log_close_fn);
}


static void aapl_register_common_fn(Aapl_t *aapl)
{
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Called %s() registration function.\n", __func__);
#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
    if (!aapl->jtag_fn && !aapl->aacs && aapl->bit_banged_jtag_fn) aapl->jtag_fn = &avago_jtag_fn;
    if (!aapl->jtag_fn && aapl->aacs) aapl->jtag_fn = &avago_aacs_jtag_fn;
    if (!aapl->jtag_idcode_fn) aapl_register_jtag_idcode_fn(aapl, &avago_get_jtag_idcode_fn);
#endif
    if( !aapl->parallel_serdes_int_fn )
    {
        if( aapl->serdes_int_fn )
            aapl_register_parallel_serdes_int_fn(aapl, &avago_parallel_core_serdes_int_sbus_fn);
        else
            aapl_register_parallel_serdes_int_fn(aapl, &avago_parallel_serdes_int_sbus_fn);
    }
    if (!aapl->serdes_int_fn) aapl_register_spico_int_fn(aapl, &avago_serdes_spico_int_sbus_fn);
#if AAPL_ENABLE_BLACKHAWK
    if (!aapl->pmi_fn) aapl_register_pmi_fn(aapl, &avago_pmi_sbus_fn);
#endif
}

/** @defgroup register Function Registration API.
 ** @{
 ** @brief   Registers SBus functions.
 ** @details Open and close functions are optional, and can be NULL.
 **          The arguments for the registered SBus function are:
 **           - return: TRUE or FALSE to indicate if the command succeeded.
 **           - addr: SBus address to operate on. Corresponds to the *_sbus_receiver_address ports of the SBus master.
 **           - reg_addr: Data address within the given SBus address to operate on. Corresponds to the *_sbus_data_address ports on the SBus master.
 **           - command: SBus command to send. Corresponds to the *_sbus_command ports on the SBus master.
 **                 Required commands are: 1: write, 2: read, 0: reset
 **           - sbus_data: Pointer to the SBus data to write. Results of SBus read operations will be placed here.
 **/
void aapl_register_sbus_fn(Aapl_t *aapl,
    uint (* user_sbus_fn)(Aapl_t *, uint addr, unsigned char reg_addr, unsigned char command, uint *sbus_data),
    int (* comm_open_fn)(Aapl_t *),
    int (* comm_close_fn)(Aapl_t *)
)
{
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Called %s() registration function.\n", __func__);
    aapl->communication_method = AVAGO_SBUS;
    aapl->sbus_fn = user_sbus_fn;
    aapl->sbus_fn_2 = 0;

    aapl->comm_open_fn = comm_open_fn;
    aapl->comm_close_fn = comm_close_fn;

    aapl_register_common_fn(aapl);
}

#if AAPL_ENABLE_ATE_VEC
void aapl_register_ate_vec_fn(Aapl_t *aapl,
    int (* ate_vec_fn)(Aapl_t *, const char *command))
{
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Called %s() registration function.\n", __func__);
    aapl->ate_vec_fn = ate_vec_fn;

    aapl_register_common_fn(aapl);
}
#endif

#if AAPL_ALLOW_AACS
/** @brief   Registers SBus functions.
 ** @details Open and close functions are optional, and can be NULL.
 **          The arguments for the registered SBus function are:
 **             return: TRUE or FALSE to indicate if the command succeeded.
 **             addr: SBus address to operate on. Corresponds to the *_sbus_receiver_address ports of the SBus master.
 **             reg_addr: Data address within the given SBus address to operate on. Corresponds to the *_sbus_data_address ports on the SBus master.
 **             command: SBus command to send. Corresponds to the *_sbus_command ports on the SBus master.
 **                 Required commands are: 1: write, 2: read, 0: reset
 **             sbus_data: Pointer to the SBus data to write. Results of SBus read operations will be placed here.
 **             recv_data_back: When 0, the command may be queued for execution later.
 **/
static void aapl_register_sbus_fn_2(Aapl_t *aapl,
    uint (* user_sbus_fn)(Aapl_t *, uint addr, unsigned char reg_addr, unsigned char command, uint *sbus_data, uint recv_data_back),
    int (* comm_open_fn)(Aapl_t *),
    int (* comm_close_fn)(Aapl_t *)
)
{
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Called %s() registration function.\n", __func__);
    aapl->communication_method = AVAGO_SBUS;
    aapl->sbus_fn = 0;
    aapl->sbus_fn_2 = user_sbus_fn;

    aapl->comm_open_fn = comm_open_fn;
    aapl->comm_close_fn = comm_close_fn;

    aapl_register_common_fn(aapl);
}
#endif

#if AAPL_ALLOW_MDIO || AAPL_ALLOW_GPIO_MDIO
/** @brief   Registers MDIO functions.
 ** @brief   This function will be used to implement SBus commands.
 ** @details Open and close functions are optional, and can be NULL.
 **/
void aapl_register_mdio_fn(Aapl_t *aapl,
    uint (* mdio_fn)(Aapl_t *, Avago_mdio_cmd_t mdio_cmd, uint port_addr, uint dev_addr, uint reg_addr, uint data),
    int (* comm_open_fn)(Aapl_t *),
    int (* comm_close_fn)(Aapl_t *)
)
{
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Called %s() registration function.\n", __func__);
    aapl->communication_method = AVAGO_MDIO;
    aapl->sbus_fn = &avago_mdio_sbus_fn;
    aapl->sbus_fn_2 = 0;

    aapl->comm_open_fn = comm_open_fn;
    aapl->comm_close_fn = comm_close_fn;

    aapl->mdio_fn = mdio_fn;

    aapl_register_common_fn(aapl);
}
#endif

#if AAPL_ALLOW_I2C || AAPL_ALLOW_SYSTEM_I2C
/** @brief   Registers I2C functions.
 ** @brief   These functions will be used to implement SBus commands.
 ** @details Open and close functions are optional, and can be NULL.
 **          The arguments for the registered I2C functions are:
 **              return   length (>= 0) on success, -1 on failure.
 **              aapl     Pointer to AAPL structure.
 **              dev_addr I2C address to write/read.
 **              length   Number of bytes to write/read from buffer.
 **              *buffer  Pointer to the buffer containing bytes to be written/read to/from the device.
 **/
void aapl_register_i2c_fn(Aapl_t *aapl,
    int (* i2c_read_fn)(Aapl_t *, unsigned int dev_addr, unsigned int length, unsigned char * buffer),
    int (* i2c_write_fn)(Aapl_t *, unsigned int dev_addr, unsigned int length, unsigned char * buffer),
    int (* comm_open_fn)(Aapl_t *),
    int (* comm_close_fn)(Aapl_t *)
)
{
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Called %s() registration function.\n", __func__);
    aapl->communication_method = AVAGO_I2C;
    aapl->sbus_fn = &avago_i2c_sbus_fn;
    aapl->sbus_fn_2 = 0;

    aapl->comm_open_fn = comm_open_fn;
    aapl->comm_close_fn = comm_close_fn;

    aapl->i2c_write_fn = i2c_write_fn;
    aapl->i2c_read_fn = i2c_read_fn;

    aapl_register_common_fn(aapl);
}
#endif

#if AAPL_ALLOW_JTAG
/** @brief   Registers JTAG functions.
 ** @brief   This function will be used to implement SBus commands.
 ** @details Open and close functions are optional, and can be NULL.
 **/
void aapl_register_jtag_fn(Aapl_t *aapl,
    char *(* jtag_fn)(Aapl_t *, int opcode, int length, const char *tdi, BOOL get_tdo),
    int (* comm_open_fn)(Aapl_t *),
    int (* comm_close_fn)(Aapl_t *)
)
{
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Called %s() registration function.\n", __func__);
    aapl->communication_method = AVAGO_JTAG;
    aapl->sbus_fn = &avago_jtag_sbus_fn;
    aapl->sbus_fn_2 = 0;

    aapl->comm_open_fn = comm_open_fn;
    aapl->comm_close_fn = comm_close_fn;

    aapl->jtag_fn = jtag_fn;

    aapl_register_common_fn(aapl);
}


/** @brief   Registers bit-banged JTAG functions.
 ** @brief   This function will be used to implement SBus commands.
 ** @details Open and close functions are optional, and can be NULL.
 **/
void aapl_register_bit_banged_jtag_fn(Aapl_t *aapl,
    BOOL (* jtag_fn)(Aapl_t *, BOOL tms, BOOL tdi, BOOL trst_l, BOOL get_tdo),
    int (* comm_open_fn)(Aapl_t *),
    int (* comm_close_fn)(Aapl_t *)
)
{
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Called %s() registration function.\n", __func__);
    aapl->communication_method = AVAGO_BB_JTAG;
    aapl->sbus_fn = &avago_jtag_sbus_fn;
    aapl->sbus_fn_2 = 0;

    aapl->comm_open_fn = comm_open_fn;
    aapl->comm_close_fn = comm_close_fn;

    aapl->jtag_fn = &avago_jtag_fn;
    aapl->bit_banged_jtag_fn = jtag_fn;

    aapl_register_common_fn(aapl);
}
#endif

/** @brief   Registers a function to obtain the JTAG IDCODE of a device. */
void aapl_register_jtag_idcode_fn(Aapl_t *aapl,
    uint (* jtag_idcode_fn)(Aapl_t *, uint device)
)
{
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Called %s() registration function.\n", __func__);
    aapl->jtag_idcode_fn = jtag_idcode_fn;
}

/** @brief   Registers a SerDes SPICO interrupt function.
 ** @brief   If registered, SPICO interrupts will be sent via this function, otherwise interrupts
 **          will be sent via SBus commands.
 **/
void aapl_register_spico_int_fn(Aapl_t *aapl,
    uint  (*serdes_int_fn)     (Aapl_t *aapl, uint addr, int int_code, int int_data)
)
{
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Called %s() registration function.\n", __func__);
    aapl->serdes_int_fn = serdes_int_fn;
}

/** @brief   Registers an SBus reset function.
 ** @brief   SBus resets will be sent via the registered function.
 ** @brief   This function performs an SBus reset operation on the chip of the addressed SerDes.
 **/
void aapl_register_hard_sbus_reset_fn(Aapl_t *aapl,
    int  (*hard_sbus_reset_fn)(Aapl_t *aapl, uint addr)
)
{
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Called %s() registration function.\n", __func__);
    aapl->hard_sbus_reset_fn = hard_sbus_reset_fn;
}

/** @brief   Registers a PMI access function.
 ** @brief   If registered, PMI commands will be sent via this function, otherwise they
 **          will be sent via SBus commands.
 **/
void aapl_register_pmi_fn(Aapl_t *aapl,
    uint  (*pmi_fn)(Aapl_t *aapl, uint sbus_addr, int command, uint reg_addr, uint *data, uint mask))
{
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Called %s() registration function.\n", __func__);
    aapl->pmi_fn = pmi_fn;
}

/** @brief   Registers a function to execute an interrupt on multiple SerDes in parallel.
 ** @details If registered, parallel SPICO interrupts will be sent via this function;
 **          otherwise parallel interrupts will be sent via SBus commands.
 ** @details Note that addr_list members contain a results field into which
 **          should be written the return value from each SerDes.
 ** @details When called, the registered function should return 1 if all interrupts successfully return the same value,
 **          return 0 if all interrupts successfully return with non-identical values,
 **          and return -1 and decrement aapl->return_code if any failure.
 ** @details For an example implementation using SBus, see avago_parallel_serdes_int_sbus_fn().
 ** @details For an example serial implementation, see avago_parallel_core_serdes_int_sbus_fn().
 **/
void aapl_register_parallel_serdes_int_fn(Aapl_t *aapl,
    int  (*parallel_serdes_int_fn) (Aapl_t *aapl, Avago_addr_t *addr_list, int int_code, int int_data))
{
    aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Called %s() registration function.\n", __func__);
    aapl->parallel_serdes_int_fn = parallel_serdes_int_fn;
}

/** @} */

static void aapl_register_built_in_fn(Aapl_t *aapl)
{

    Aapl_comm_method_t comm_method = aapl->communication_method;

    if( aapl->communication_method == AVAGO_GPIO_MDIO || aapl->communication_method == AVAGO_SYSTEM_I2C
        || aapl->communication_method == AVAGO_OFFLINE || aapl->communication_method == AVAGO_BB_GPIO_JTAG )
         aapl->aacs = 0;
    else aapl->aacs = 1;

    aapl_print_registration_info(aapl);

    if( !aapl->hard_sbus_reset_fn) aapl_register_hard_sbus_reset_fn(aapl, &avago_hard_sbus_reset_fn);
    if( !aapl->log_fn ) aapl_register_logging_fn(aapl, &aapl_built_in_logging_fn, 0, 0);

#if AAPL_ALLOW_AACS
    if (aapl->aacs)
    {
        aapl_register_sbus_fn_2(aapl, &aapl_aacs_sbus_fn, &avago_aacs_open_fn, &avago_aacs_close_fn);
        aapl->communication_method = comm_method;
        avago_aacs_open_fn(aapl);

#if AAPL_ALLOW_I2C
        aapl->i2c_write_fn = &avago_aacs_i2c_write_fn;
        aapl->i2c_read_fn = &avago_aacs_i2c_read_fn;
#endif
#if AAPL_ALLOW_MDIO
        aapl->mdio_fn = &avago_aacs_mdio_fn;
#endif

#if AAPL_ENABLE_BLACKHAWK
#  if AAPL_ALLOW_MDIO
        if( aapl->communication_method == AVAGO_MDIO )
            aapl->pmi_fn = &avago_pmi_mdio_fn;
        else
#  endif
        if( aapl->capabilities & AACS_SERVER_PMI )
            aapl->pmi_fn = &avago_aacs_pmi_fn;
        else
            aapl->pmi_fn = &avago_pmi_sbus_fn;
#endif
    }
#endif

#if AAPL_ALLOW_SYSTEM_I2C
    if( !aapl->i2c_read_fn && (aapl_is_i2c_communication_method(aapl) || aapl->communication_method == AVAGO_BB_GPIO_JTAG) )
    {
        if (aapl->communication_method == AVAGO_BB_GPIO_JTAG)
        {
            aapl_register_i2c_fn(aapl, &avago_system_i2c_read_fn, &avago_system_i2c_write_fn, &avago_system_i2c_open_fn, &avago_system_i2c_close_fn);
            aapl->communication_method = AVAGO_BB_GPIO_JTAG;
        }
        else aapl_register_i2c_fn(aapl, &avago_system_i2c_read_fn, &avago_system_i2c_write_fn, &avago_system_i2c_open_fn, &avago_system_i2c_close_fn);
    }
#endif

#if AAPL_ALLOW_GPIO_MDIO
    if( !aapl->mdio_fn && aapl_is_mdio_communication_method(aapl) )
        aapl_register_mdio_fn(aapl, &avago_gpio_mdio_fn, &avago_gpio_mdio_open_fn, &avago_gpio_mdio_close_fn);
#endif

#if AAPL_ALLOW_OFFLINE_SBUS
    if (aapl->communication_method == AVAGO_OFFLINE)
    {
        aapl->sbus_fn       = &avago_offline_sbus_fn;
        aapl->sbus_fn_2     = 0;
        aapl->comm_open_fn  = &avago_offline_open_fn;
        aapl->comm_close_fn = &avago_offline_close_fn;
    }
#endif

#if AAPL_ALLOW_JTAG && AAPL_ALLOW_AACS
#if AAPL_ALLOW_GPIO_JTAG
    if(aapl->communication_method == AVAGO_BB_GPIO_JTAG)
    {
        aapl_register_jtag_fn(aapl, &avago_jtag_fn, 0, 0);
        aapl_register_bit_banged_jtag_fn(aapl, &avago_bit_banged_gpio_jtag_fn, &avago_bit_banged_gpio_jtag_open_fn, &avago_bit_banged_gpio_jtag_close_fn);
    }
    else
#endif
    if(aapl->communication_method == AVAGO_JTAG)
        aapl_register_jtag_fn(aapl, &avago_aacs_jtag_fn, &avago_aacs_open_fn, &avago_aacs_close_fn);
    else if(aapl->communication_method == AVAGO_BB_JTAG)
        aapl_register_jtag_fn(aapl, &avago_jtag_fn, &avago_aacs_open_fn, &avago_aacs_close_fn);
#endif

    aapl->communication_method = comm_method;
    aapl_print_registration_info(aapl);

    if (!aapl->sbus_fn && !aapl->sbus_fn_2) aapl_fail(aapl, __func__, __LINE__, "AAPL SBus function not registered.\n");
}

/** @brief   Opens a connection to an Avago device.
 ** @details If aacs_server is NULL, assumes that the aacs_server and tcp_port
 **          fields of Aapl_t have already been set.
 ** @return  void, but decrements aapl->return_code value on error.
 **/
void aapl_connect(
    Aapl_t *aapl,               /**< [in] Pointer to Aapl_t structure. */
    const char *aacs_server,    /**< [in] AACS server name string. */
    int tcp_port)               /**< [in] TCP port number for AACS server. */
{
    (void)aacs_server;
    (void)tcp_port;

#if AAPL_ALLOW_AACS
    if( aacs_server )
    {
        uint len;
        if (!aapl->aacs_server)
        {
            aapl_fail(aapl, __func__, __LINE__, "aapl_connect() called using Aapl_t struct that wasn't initialized.\n");
            return;
        }
        len = strlen(aacs_server)+1;
        aapl->aacs_server = (char *) aapl_realloc(aapl, aapl->aacs_server, len, __func__);
        if( aapl->aacs_server ) strncpy(aapl->aacs_server, aacs_server, len);
    }
    if( tcp_port ) aapl->tcp_port = tcp_port;
#endif

    if (!aapl->sbus_fn && !aapl->sbus_fn_2)
    {
        aapl_log_printf(aapl, AVAGO_DEBUG3, __func__, __LINE__, "%s() called and no SBus function has been registered. Calling aapl_register_built_in_fn() to attempt to register built-in functions.\n", __func__);
        aapl_register_built_in_fn(aapl);
    }
    else
    {
        aapl_log_printf(aapl, AVAGO_DEBUG3, __func__, __LINE__, "%s() called and SBus function has been registered.\n", __func__);
        aapl_print_registration_info(aapl);
    }

    if (aapl->socket < 0 && aapl->comm_open_fn) aapl->comm_open_fn(aapl);
    if (aapl->socket < 0 && !aapl->comm_open_fn) aapl->socket = 1;
    aapl_log_printf(aapl, AVAGO_DEBUG3, __func__, __LINE__, "Current aapl->socket: %d. return_code = %d\n", aapl->socket, aapl->return_code);
    aapl_sigint_check(aapl);
}

/** @cond INTERNAL */

/** @brief  Closes the current connection to the AAPL device.
 ** @detail Does not flush buffered commands to avoid infinite loops on error.
 ** @return void
 **/
void aapl_close_connection2(
    Aapl_t *aapl)   /**< [in] Pointer to Aapl_t structure. */
{
    if (aapl->comm_close_fn) aapl->comm_close_fn(aapl);
    else aapl->socket = -1;
}

/** @endcond */

/** @brief   Closes the current connection to the AAPL device.
 ** @details Will flush any buffered commands first.
 ** @return  void
 **/
void aapl_close_connection(
    Aapl_t *aapl)   /**< [in] Pointer to Aapl_t structure. */
{
    AAPL_CLOSE_LOCK;
    if( aapl->socket >= 0 )
    {
        AAPL_SUPPRESS_ERRORS_PUSH(aapl);
        aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Closing connection to %s:%d (socket #%d).\n", aapl->aacs_server, aapl->tcp_port, aapl->socket);
        avago_aacs_flush(aapl);
        AAPL_SUPPRESS_ERRORS_POP(aapl);
    }
    aapl_close_connection2(aapl);
    #if AAPL_ALLOW_SYSTEM_I2C
    avago_system_i2c_close_fn(aapl);
    #endif
    aapl->socket = -1;
    AAPL_CLOSE_UNLOCK;
    aapl_sigint_check(aapl);
}

/** @brief Checks the socket status of aapl->socket
 **
 ** @return 0 if the socket is still connected, -1 if it is closed
 **/
int aapl_connection_status(
    Aapl_t *aapl)   /**< [in] Pointer to AAPL structure. */
{
    if( aapl->socket < 0 )
        return -1;
    #if AAPL_ALLOW_AACS
    if (aapl->aacs)
    {
        int rc, error = 0;
        socklen_t len = sizeof (error);
        avago_aacs_send_command(aapl, "version");

        rc = getsockopt (aapl->socket, SOL_SOCKET, SO_ERROR, (void *)&error, &len );

        aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "socket: %d getsockopt: %d errno: %s\n", aapl->socket, rc, aapl_tcp_strerr());

        return rc;
    }
    #endif
    return 0;
}

/** @cond INTERNAL */

int aapl_reconnect(Aapl_t *aapl, const char * command)
{
    int rc = aapl->return_code;
    if (aapl->socket >= 0 || aapl->disable_reconnect)
        return 1;
    aapl_connect(aapl, 0, 0);
    if (aapl->return_code - rc != 0)
    {
        aapl_fail(aapl, __func__, __LINE__, "The command \"%s\" was sent to a closed socket.\n", command);
        aapl->data = 0;
        aapl_log_printf(aapl, AVAGO_DATA_CHAR, 0, 0, "");
        aapl_close_connection2(aapl);
        return -1;
    }
    return 0;
}

/** @endcond */

/** @brief Constructs the Aapl_t structure. Allocates memory and sets default values.
 ** @warning Generally the user should not read or modify any elements of this struct.
 ** Use functions provided to get and set information in this structure.
 ** @return A pointer to the Aapl_t struct
 **/
Aapl_t *aapl_construct(void)
{
    uint i;

    Aapl_t *aapl = (Aapl_t *) AAPL_MALLOC(sizeof(Aapl_t));
    if( !aapl ) return 0;

    memset(aapl, 0, sizeof(*aapl));

#if AAPL_ALLOW_SIGNALS
    signal(SIGINT, aapl_signal_handler);
#endif

#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
    memset(aapl->sbus_reg, '0', sizeof(aapl->sbus_reg));
    aapl->sbus_reg[125-47] = '1';
#endif

    for( i = 0; i < AAPL_MAX_CHIPS; i++ )
        aapl->chip_name[i] = aapl->chip_rev[i] = "";

    memset(aapl->lsb_rev,AAPL_LSB_INIT, sizeof(aapl->lsb_rev));

    aapl->socket         = -1;
    aapl->i2c_fd         = -1;

    aapl->data_char      = (char *) AAPL_MALLOC(AAPL_LOG_BUF_SIZE);
    if( !aapl->data_char ) { AAPL_FREE(aapl); return 0; }
    aapl->data_char_end  = aapl->data_char;
    aapl->data_char_size = AAPL_LOG_BUF_SIZE;
    *aapl->data_char     = '\0';
    aapl->aacs_server    = aapl_strdup(aapl_default_server);
    aapl->tcp_port       = aapl_default_port;

    aapl->debug = 0;

    aapl->enable_debug_logging      = AAPL_DEFAULT_ENABLE_DEBUG_LOGGING;
    aapl->enable_stream_logging     = AAPL_DEFAULT_ENABLE_STREAM_LOGGING;
    aapl->enable_stream_err_logging = AAPL_DEFAULT_ENABLE_STREAM_ERR_LOGGING;
    aapl->logging_file_path         = NULL;

    aapl->chips                 = AAPL_NUMBER_OF_CHIPS_OVERRIDE;
    for (i=0; i<aapl->chips; i++)
        aapl->sbus_rings[i]     = AAPL_NUMBER_OF_RINGS_OVERRIDE;


# ifdef AAPL_LOG_TIME_STAMPS
    aapl->log_time_stamps       = AAPL_LOG_TIME_STAMPS;
# endif
    aapl->serdes_int_timeout    = AAPL_SERDES_INT_TIMEOUT;
    aapl->sbus_mdio_timeout     = AAPL_SBUS_MDIO_TIMEOUT;
    aapl->max_cmds_buffered     = AAPL_MAX_CMDS_BUFFERED;
    aapl->communication_method  = AAPL_DEFAULT_COMM_METHOD;
    aapl->i2c_base_addr         = AAPL_DEFAULT_I2C_BASE_ADDR;
    aapl->mdio_base_port_addr   = AAPL_DEFAULT_MDIO_BASE_PORT_ADDR;
    aapl->ansi_colors           = AAPL_ALLOW_ANSI_COLORS;
    aapl->sbus_tdr_opcode_base  = AAPL_DEFAULT_SBUS_TDR_OPCODE_BASE;

#if AAPL_ALLOW_THREAD_SUPPORT
    pthread_mutex_init(&aapl->aacs_mutex, NULL);
    pthread_mutex_init(&aapl->spico_int_mutex, NULL);
    pthread_mutex_init(&aapl->serdes_mem_mutex, NULL);
    pthread_mutex_init(&aapl->ip_info_mutex, NULL);
    pthread_mutex_init(&aapl->sensor_mutex, NULL);
#endif

    return aapl;
}


/**
 ** @brief Frees memory of an aapl struct when it is no longer needed.
 ** @return void
 **/
void aapl_destruct(
    Aapl_t *aapl)   /**< [in] Pointer to Aapl_t structure. */
{
#if AAPL_ENABLE_FILE_IO
    int chip, addr, ring;
    for (chip = 0; chip < AAPL_MAX_CHIPS; chip++)
        for (ring = 0; ring < AAPL_MAX_RINGS; ring++)
            for (addr = 0; addr <= 0xff; addr++)
                if (aapl->firmware_file[chip][ring][addr]) aapl_free(aapl, aapl->firmware_file[chip][ring][addr], __func__);
#endif

#if AAPL_ALLOW_THREADED_SBUS || AAPL_ALLOW_THREAD_SUPPORT
    aapl->destroy_thread = 1;
#endif
#if AAPL_ALLOW_THREADED_SBUS
    if (aapl->sbus_thread) pthread_join(aapl->sbus_thread, NULL);
#endif

    aapl_close_connection(aapl);

#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
    if (aapl->buf_cmd)              AAPL_FREE(aapl->buf_cmd);
    aapl->buf_cmd = 0;
#endif

    if (aapl->data_char)            AAPL_FREE(aapl->data_char);
    if (aapl->log)                  AAPL_FREE(aapl->log);
    if (aapl->aacs_server)          AAPL_FREE(aapl->aacs_server);
    if (aapl->aacs_server_buffer)   AAPL_FREE(aapl->aacs_server_buffer);

    aapl->data_char = 0;
    aapl->log = 0;
    aapl->aacs_server = 0;
    aapl->aacs_server_buffer = 0;

#if AAPL_ALLOW_OFFLINE_SBUS
    if (aapl->offline_sbus_reg)     AAPL_FREE(aapl->offline_sbus_reg);
    aapl->offline_sbus_reg = 0;
#endif

    AAPL_FREE(aapl);
}

/**
 ** @brief Returns the return code and then sets it to zero.
 **    Values less than 0 indicate that some failure has occurred.
 ** @warning Calling this function clears the return code.
 ** @return The current value of the AAPL return code.
 **/
int aapl_get_return_code(
    Aapl_t *aapl)   /**< [in] Pointer to Aapl_t structure. */
{
    int x;
    x = aapl->return_code;
    aapl->return_code = 0;
    return x;
}



/** @brief  Gets the number of chips found on the current ring.
 ** @return The number of chips on the current ring.
 **/
uint aapl_get_chips(
    Aapl_t *aapl)   /**< [in] Pointer to Aapl_t structure. */
{
    return aapl->chips;
}

/** @brief  Gets the maximum sbus address for the specified chip and sbus ring within that chip.
 ** @return Maximum sbus address of the specifiec SBus ring and chip.
 **/
uint aapl_get_max_sbus_addr_for(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint chip,      /**< [in] Chip number. */
    uint ring)      /**< [in] Ring number. */
{
    if( chip >= AAPL_MAX_CHIPS || ring >= AAPL_MAX_RINGS )
        return 0;
    return aapl->max_sbus_addr[chip][ring];
}

/** @brief  Gets the maximum sbus address for the specified chip and sbus ring within that chip.
 ** @return Maximum sbus address of the sbus ring and chip specified by the sbus address.
 **/
uint aapl_get_max_sbus_addr(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    return aapl_get_max_sbus_addr_for(aapl, addr_struct.chip, addr_struct.ring);
}

/** @brief Get the JTAG idcode of the specified chip
 ** @return JTAG idcode of the current sbus ring (specified by the sbus address)
 **/
uint aapl_get_jtag_idcode(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS ) addr_struct.chip = 0;
    return aapl->jtag_idcode[addr_struct.chip];
}

/** @brief Get the chip name of the specified chip
 ** @return The chip name of the current chip (specified by the sbus address)
 **        Returns the aapl->data_char string pointer, which is a C-string holding the command status.
 **        aapl->data is set to length of the returned string.
 **/
const char *aapl_get_chip_name(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS )
        return "unknown";
    return aapl->chip_name[addr_struct.chip];
}

/** @brief  Gets the chip revision number of the specified chip.
 ** @return The chip revision number of the current chip (specified by the sbus address).
 **/
const char *aapl_get_chip_rev_str(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS )
        return "";
    return aapl->chip_rev[addr_struct.chip];
}



/** @brief Gets the Logical Sub Block (LSB) revision number of the device.
 ** @details
 ** Note: aapl_get_lsb_rev() stores the lowest lsb rev in [0][0][0xff]
 ** @return The Logical Sub Block (LSB) revision number of the addressed device.
 **/
uint aapl_get_lsb_rev(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);

    if( aapl_check_broadcast_address(aapl, addr, __func__, __LINE__, FALSE) )
    {
        if (aapl->lsb_rev[0][0][addr_struct.sbus] == AAPL_LSB_INIT)
        {
            BOOL st;
            Avago_addr_t start, stop, next;
            for( st = aapl_broadcast_first(aapl, &addr_struct, &start, &stop, &next, AAPL_BROADCAST_IGNORE_LANE);
                 st;
                 st = aapl_broadcast_next(aapl, &next, &start, &stop, AAPL_BROADCAST_IGNORE_LANE) )
            {
                uint addr = avago_struct_to_addr(&next);
                if (aapl_get_ip_type(aapl, addr) != AVAGO_SERDES) continue;
                aapl_get_lsb_rev(aapl, addr);
                break;
            }
        }
        return aapl->lsb_rev[0][0][addr_struct.sbus];
    }

    if( addr_struct.chip >= AAPL_MAX_CHIPS ||
        addr_struct.ring >= AAPL_MAX_CHIPS )
        return 0;

    if( aapl->lsb_rev[AAPL_3D_ARRAY_ADDR(addr_struct)] == AAPL_LSB_INIT )
    {
        signed char lsb_rev = avago_serdes_get_lsb_rev(aapl, addr);
        aapl->lsb_rev[AAPL_3D_ARRAY_ADDR(addr_struct)] = lsb_rev;
        if( lsb_rev >= 0 )
        {
            int baddr = 0;
            Avago_ip_type_t ip_type = aapl_get_ip_type(aapl, addr);
            if(      ip_type == AVAGO_SERDES ) baddr = AVAGO_SERDES_D6_BROADCAST;
            else if( ip_type == AVAGO_M4     ) baddr = AVAGO_SERDES_M4_BROADCAST;
            else if( ip_type == AVAGO_P1     ) baddr = AVAGO_SERDES_P1_BROADCAST;
            if( baddr > 0 && (aapl->lsb_rev[0][0][baddr] == AAPL_LSB_INIT || aapl->lsb_rev[0][0][baddr] > lsb_rev) )
                 aapl->lsb_rev[0][0][baddr] = lsb_rev;
        }
        aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Setting aapl->lsb_rev for %s to 0x%04x\n", aapl_addr_to_str(0xff), aapl->lsb_rev[AAPL_3D_ARRAY_ADDR(addr_struct)]);
    }
    return aapl->lsb_rev[AAPL_3D_ARRAY_ADDR(addr_struct)];
}


static int ip_rev(Aapl_t *aapl, uint sbus_addr)
{
    Avago_ip_type_t ip_type = aapl_get_ip_type(aapl, sbus_addr);
    if (!aapl_check_process(aapl, sbus_addr, __func__, __LINE__, TRUE, 4, AVAGO_PROCESS_E, AVAGO_PROCESS_B, AVAGO_PROCESS_F, AVAGO_TSMC_05)) return -1;

    switch( ip_type )
    {
#if AAPL_ENABLE_D5
    case AVAGO_D5: return avago_d5_serdes_get_rev(aapl, sbus_addr);
#endif
#if AAPL_ENABLE_D7
    case AVAGO_D7S: return avago_d7_serdes_get_rev(aapl, sbus_addr);
#endif
#if AAPL_ENABLE_BLACKHAWK
    case AVAGO_OSPREY:
    case AVAGO_BLACKHAWK:
        return blackhawk_serdes_get_ip_rev(aapl, sbus_addr);
#endif
    case AVAGO_SERDES:
    case AVAGO_P1:
    case AVAGO_M4:
        switch( aapl_get_process_id(aapl, sbus_addr) )
        {
        case AVAGO_PROCESS_F: {
            int ret_data = avago_sbus_rd(aapl, sbus_addr, 0xfe);
            if( 0 == ret_data )
            {
                ret_data = avago_serdes_mem_rd(aapl, sbus_addr, AVAGO_ESB, 0x0303);
            }
            return (ret_data >> 8) & 0xff;
            }
        case AVAGO_PROCESS_B:
            {
                int reg = avago_sbus_rd(aapl, sbus_addr, 0xfe);
                int iprev = (reg >> 8) & 0xff;
                int orig_iprev = iprev;
                if( iprev == 0x22 )
                {
                    int sbus = sbus_addr & 0xff;
                    if( (sbus == 8 || sbus == 9) && aapl_get_jtag_idcode(aapl,0) == 0x09b1657f )
                        iprev = 0x28;
                    else if( aapl_get_jtag_idcode(aapl,0) == 0x09c2557f )
                        iprev = 0x28;
                    else if( aapl_get_jtag_idcode(aapl,0) == 0x19b0257f )
                        iprev = 0x28;
                }
                if( iprev == 0x51 && (sbus_addr&0xff) <= 7 && aapl_get_jtag_idcode(aapl, 0) == 0x49c2557f )
                    iprev = 0x510;

                if( 0 != (reg & 0x80) && (iprev == 0x4e || iprev == 0x58 || iprev == 0x5e) )
                    iprev += 0xa0;

                if( orig_iprev != iprev )
                    aapl_log_printf(aapl,AVAGO_DEBUG2,__func__,__LINE__,"SBus %s: Correcting SerDes iprev from 0x%02x -> 0x%02x\n", aapl_addr_to_str(sbus_addr), orig_iprev, iprev);
                return iprev;
            }
        case AVAGO_PROCESS_E:
            return (avago_sbus_rd(aapl, sbus_addr, 0xfe) >> 8) & 0xff;
        case AVAGO_TSMC_05:
            return (avago_sbus_rd(aapl, sbus_addr, 0xfe) >> 8) & 0xff;
        default:
            return avago_sbus_rd(aapl, sbus_addr, 0xfe) & 0xff;
        }

    case AVAGO_PCIE_PCS:                return (avago_sbus_rd(aapl, sbus_addr, 0x00) >> 1) & 0xf;

    case AVAGO_AVSP_CONTROL_LOGIC:
    case AVAGO_PCS64B66B:
    case AVAGO_PCS64B66B_FEC:
    case AVAGO_SAPPH_GBX:               return (avago_sbus_rd(aapl, sbus_addr, 0x00) >> 5) & 7;

    case AVAGO_CORE_PLL:                return  avago_sbus_rd(aapl, sbus_addr, 0x00) >> 28;
    case AVAGO_AUTO_NEGOTIATION:        return  avago_sbus_rd(aapl, sbus_addr, 0x23) >> 1;
    case AVAGO_APC:                     return  avago_sbus_rd(aapl, sbus_addr,   62);

    case AVAGO_MLD:
    case AVAGO_OPAL_CONTROL:
    case AVAGO_OPAL_RSFEC528:
    case AVAGO_OPAL_RSFEC528_544:
    case AVAGO_PAD_CONTROL:
    case AVAGO_RSFEC_BRIDGE:            return (avago_sbus_rd(aapl, sbus_addr, 0xfc) >> 12) & 0xf;

    case AVAGO_RS4K_FEC:
    case AVAGO_BCH16K_FEC:              return (avago_sbus_rd(aapl, sbus_addr, 0xf5) >> 12) & 0xf;
    case AVAGO_SPICO:                   return (avago_sbus_rd(aapl, sbus_addr, 0x0f) >> 12) & 0xf;

    case AVAGO_MAX:
    case AVAGO_OPAL_HOST_ALIGNER:
    case AVAGO_OPAL_MOD_ALIGNER:
    case AVAGO_GARNET_25GE_INTERFACE:   return avago_sbus_rd(aapl, sbus_addr, 0xfc) & 0xf;

    case AVAGO_VTMON:
    case AVAGO_ANCIL:
    case AVAGO_SBUS2APB:                return avago_sbus_rd(aapl, sbus_addr, 0xfd) & 0xf;

    case AVAGO_SBUS_CONTROLLER:         return avago_sbus_rd(aapl, sbus_addr, 0xfe);
    case AVAGO_LINK_EMULATOR:           return 0;

    case AVAGO_LINK_EMULATOR_2:         return avago_sbus_rd(aapl, sbus_addr, 0x5f) >> 4;
    default: break;
    }

    return -1;
}

static BOOL verify_addr_and_ip_type(Aapl_t *aapl, uint addr, Avago_addr_t *addr_struct)
{
    avago_addr_to_struct(addr,addr_struct);
    if( addr_struct->chip >= AAPL_MAX_CHIPS ||
        addr_struct->ring >= AAPL_MAX_RINGS )
        return FALSE;
    if (aapl->ip_type[AAPL_3D_ARRAY_ADDR(*addr_struct)] == 0xff) aapl_set_ip_type(aapl, addr);
    return TRUE;
}

/** @brief Gets the revision number of the sbus device for the specified chip
 **        and sbus ring
 ** @return The revision number of the sbus device for the current chip and
 **        sbus ring (specified by the sbus address)
 **/
uint aapl_get_ip_rev(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    if( !verify_addr_and_ip_type(aapl, addr, &addr_struct) )
        return 0;

    if (aapl->ip_rev[AAPL_3D_ARRAY_ADDR(addr_struct)] == 0xfffe)
    {
        aapl->ip_rev[AAPL_3D_ARRAY_ADDR(addr_struct)] = ip_rev(aapl, addr) & 0xFFFF;
        aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Setting aapl->ip_rev for %s to 0x%04x\n", aapl_addr_to_str(addr), aapl->ip_rev[AAPL_3D_ARRAY_ADDR(addr_struct)]);
    }
    return aapl->ip_rev[AAPL_3D_ARRAY_ADDR(addr_struct)];
}


/** @brief  Returns the firmware rev from AAPL's cache
 ** @return Returns the firmware rev from AAPL's cache
 **/
int aapl_get_firmware_rev(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    if( !verify_addr_and_ip_type(aapl, addr, &addr_struct) )
        return 0;

    if (!aapl->firm_rev[AAPL_3D_ARRAY_ADDR(addr_struct)])
    {
        aapl->firm_rev[AAPL_3D_ARRAY_ADDR(addr_struct)] = avago_firmware_get_rev(aapl, addr);
        aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Setting aapl->firm_rev for %s to 0x%04x\n", aapl_addr_to_str(addr), aapl->firm_rev[AAPL_3D_ARRAY_ADDR(addr_struct)]);
    }
    return aapl->firm_rev[AAPL_3D_ARRAY_ADDR(addr_struct)];
}


/** @brief  Returns the firmware build from AAPL's cache
 ** @return Returns the firmware build from AAPL's cache
 **/
int aapl_get_firmware_build(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    if( !verify_addr_and_ip_type(aapl, addr, &addr_struct) )
        return 0;

    if (!aapl->firm_build[AAPL_3D_ARRAY_ADDR(addr_struct)])
    {
        aapl->firm_build[AAPL_3D_ARRAY_ADDR(addr_struct)] = avago_firmware_get_build_id(aapl, addr);
        aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Setting aapl->firm_build for %s to 0x%04x\n", aapl_addr_to_str(addr), aapl->firm_build[AAPL_3D_ARRAY_ADDR(addr_struct)]);
    }
    return aapl->firm_build[AAPL_3D_ARRAY_ADDR(addr_struct)];
}


/** @brief  Sets the client data pointer.
 ** @return void
 **/
void aapl_bind_set(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    void *client_data)  /**< [in] Pointer to client data. */
{
    aapl->client_data = client_data;
}

/** @brief  Retrieves the client data pointer.
 ** @return Pointer to client data.
 **/
void *aapl_bind_get(
    Aapl_t *aapl)   /**< [in] Pointer to Aapl_t structure. */
{
    return aapl->client_data;
}


/** @brief  Gets the IP type for the sbus address.
 ** @return The IP type of the sbus device for the current chip and
 **         sbus ring as specified by the sbus address.
 **/
Avago_ip_type_t aapl_get_ip_type(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_ip_type_t ip_type;
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS ) addr_struct.chip = 0;
    if( addr_struct.ring >= AAPL_MAX_RINGS ) addr_struct.ring = 0;

    AAPL_IP_INFO_LOCK;
    if (aapl->ip_type[AAPL_3D_ARRAY_ADDR(addr_struct)] == 0xff) aapl_set_ip_type(aapl, addr);
    ip_type = (Avago_ip_type_t) aapl->ip_type[AAPL_3D_ARRAY_ADDR(addr_struct)];
    AAPL_IP_INFO_UNLOCK;
    return ip_type;
}

/** @brief Internal function that returns index from avago_chip_id array (from asic_info.h) */
static int avago_find_chip_index(uint jtag, int jtag_mask)
{
    int at;
    for( at = 0; at < AAPL_ARRAY_LENGTH(avago_chip_id); ++at )
    {
        if( jtag == (avago_chip_id[at].jtag_idcode & jtag_mask) )
            return at;
    }
    return -1;
}

/** @brief Internal function that returns the process id of a device. Can be used as a check to see if SBus is working. */
static Avago_process_id_t get_process_id(Aapl_t *aapl, uint chip, uint ring)
{
    int rc = aapl->return_code;
    Avago_process_id_t ret = AVAGO_UNKNOWN_PROCESS;
    uint data = avago_sbus_rd(aapl, avago_make_addr3(chip, ring, AVAGO_SBUS_CONTROLLER_ADDRESS), 0xfe);

    aapl->ip_rev[chip][ring][AVAGO_SBUS_CONTROLLER_ADDRESS] = data & 0xFFFF;

    if( aapl->return_code == rc )
    {
        switch (data)
        {
        /* coverity[unterminated_default:FALSE] Fallthrough is intentional */
        default: aapl_log_printf(aapl,AVAGO_WARNING,__func__,__LINE__,"Unknown process id: 0x%x\n",data);
        /* coverity[fallthrough:FALSE] Fallthrough is intentional */
        case 0xbd:
            {
                int id = avago_find_chip_index(aapl->jtag_idcode[chip], 0xffffffff);
                if( id >= 0 )
                    ret = avago_chip_id[id].process_id;
                if( ret == AVAGO_UNKNOWN_PROCESS && data == 0xbd )
                    ret = AVAGO_PROCESS_F;
            }
            break;

        case 0xbe:
        case 0xbf:
        case 0xc1:
                   ret = AVAGO_PROCESS_F; break;

        case 0xc0:
        case 0xc2:
        case 0xc3:
                   ret = AVAGO_PROCESS_B; break;
        case 0xc5:
                   ret = AVAGO_PROCESS_E; break;
        case 0xc7:
                   ret = AVAGO_TSMC_05; break;
        }
    }
    return ret;
}

/** @brief Returns cached value of chip process identifier. */
Avago_process_id_t aapl_get_process_id(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);

    if( addr_struct.chip == AVAGO_BROADCAST )
    {
        Avago_process_id_t process = AVAGO_UNKNOWN_PROCESS;
        for (addr_struct.chip = 0; addr_struct.chip < aapl_get_chips(aapl); addr_struct.chip ++)
        {
            Avago_process_id_t curr_process = aapl_get_process_id(aapl, avago_struct_to_addr(&addr_struct));
            if( addr_struct.chip == 0 )
                process = curr_process;
            else if( curr_process != process )
                return AVAGO_UNKNOWN_PROCESS;
        }
        return process;
    }

    if( addr_struct.ring == AVAGO_BROADCAST )
    {
        Avago_process_id_t process = AVAGO_UNKNOWN_PROCESS;
        for( addr_struct.ring = 0; addr_struct.ring < (aapl->sbus_rings[addr_struct.chip]&0xf); addr_struct.ring++ )
        {
            Avago_process_id_t curr_process = aapl_get_process_id(aapl, avago_struct_to_addr(&addr_struct));
            if( addr_struct.ring == 0 )
                process = curr_process;
            else if( curr_process != process )
                return AVAGO_UNKNOWN_PROCESS;
        }
        return process;
    }

    if( addr_struct.chip >= AAPL_MAX_CHIPS || addr_struct.ring >= AAPL_MAX_RINGS )
        return AVAGO_UNKNOWN_PROCESS;

    if( aapl->process_id[addr_struct.chip][addr_struct.ring] == AVAGO_UNKNOWN_PROCESS )
    {
        Avago_process_id_t process_id = (Avago_process_id_t)aapl->process_id[addr_struct.chip][0];
        if( process_id != AVAGO_TSMC_05 )
        {
            uint i;
            for( i = 1; i < aapl->sbus_rings[addr_struct.chip]; i++ )
            {
                aapl->process_id[addr_struct.chip][i] = process_id;
                aapl->ip_rev[addr_struct.chip][i][AVAGO_SBUS_CONTROLLER_ADDRESS] = aapl->ip_rev[addr_struct.chip][0][AVAGO_SBUS_CONTROLLER_ADDRESS];
            }
            aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Setting aapl->process_id[%d][*] to 0x%04x\n", addr_struct.chip, process_id);
        }
        else
        {
            process_id = get_process_id(aapl, addr_struct.chip, addr_struct.ring);
            aapl->process_id[addr_struct.chip][addr_struct.ring] = process_id;
            aapl_log_printf(aapl, AVAGO_DEBUG6, __func__, __LINE__, "Setting aapl->process_id[%d][%d] to 0x%04x\n", addr_struct.chip, addr_struct.ring, process_id);
        }
    }
    return (Avago_process_id_t)aapl->process_id[addr_struct.chip][addr_struct.ring];
}

BOOL aapl_get_spico_running_flag(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr, &addr_struct);
    if (aapl->spico_int_only) return TRUE;
    if (aapl_check_broadcast_address(aapl, addr, __func__, __LINE__, FALSE))
    {
        BOOL st;
        Avago_addr_t start, stop, next;
        char running = TRUE;
        for( st = aapl_broadcast_first(aapl, &addr_struct, &start, &stop, &next, AAPL_BROADCAST_IGNORE_LANE);
             st;
             st = aapl_broadcast_next(aapl, &next, &start, &stop, AAPL_BROADCAST_IGNORE_LANE) )
        {
            uint sbus_addr = avago_struct_to_addr(&next);
            if (aapl_get_ip_type(aapl, sbus_addr) != AVAGO_SERDES) continue;
            running &= aapl->spico_running[next.chip][next.ring][next.sbus];
        }
        return (BOOL)running;
    }
    else
    {
        if( addr_struct.chip >= AAPL_MAX_CHIPS ||
            addr_struct.ring >= AAPL_MAX_RINGS )
            return FALSE;
        return aapl->spico_running[addr_struct.chip][addr_struct.ring][addr_struct.sbus];
    }
}

BOOL aapl_set_spico_running_flag(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr,      /**< [in] Address number. */
    BOOL running)   /**< [in] New running value */
{
    Avago_addr_t addr_struct;
    avago_addr_to_struct(addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS ||
        addr_struct.ring >= AAPL_MAX_RINGS )
        return FALSE;
    return (aapl->spico_running[addr_struct.chip][addr_struct.ring][addr_struct.sbus] = running);
}

/** @brief  Get a string representation of the Process ID of the specified chip
 **         and sbus ring
 ** @return The Process ID string of the sbus device for the current chip and
 **         sbus ring (specified by the sbus address)
 **/
const char *aapl_get_process_id_str(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint addr)      /**< [in] Address number. */
{
    return aapl_process_id_to_str(aapl_get_process_id(aapl, addr));
}


static int  try_tck_value(Aapl_t *aapl, uint chip, uint tck_delay_val)
{
    char buf[64];
    snprintf(buf, 64, "tck_delay %u", tck_delay_val);
    avago_aacs_send_command(aapl, buf);
    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Trying tck_delay of %d\n", tck_delay_val);

    return avago_diag_sbus_rw_test(aapl, avago_make_addr3(chip, 0, 0xfe), 1);
}


static int  hs1_try_mode(Aapl_t *aapl, uint chip, const char * mode)
{
    char hs1_mode[64];
    int rw;
    char *ptr;

    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Checking %s mode in HS1\n", mode);
    snprintf (hs1_mode, 64, "sbus_mode %s", avago_aacs_send_command(aapl, "sbus_mode"));

    ptr = strstr(hs1_mode," I2C");
    if( ptr ) *ptr = '\0';

    avago_aacs_send_command(aapl, mode);

    AAPL_SUPPRESS_ERRORS_PUSH(aapl);
    rw = avago_diag_sbus_rw_test(aapl, avago_make_addr3(chip, 0, 0xfe), 1);
    if (!rw && strstr("sbus_mode jtag", mode))
    {
        char tck_delay_orig[64];
        snprintf (tck_delay_orig, 64, "tck_delay %s", avago_aacs_send_command(aapl, "tck_delay"));

        rw = try_tck_value(aapl, chip, 10);
        if( rw )
        {
            uint tck_delay_val = 0;
            while( tck_delay_val < 10 )
            {
                rw = try_tck_value(aapl, chip, tck_delay_val);
                if( rw ) break;
                tck_delay_val ++;
            }
        }
        if( !rw )
        {
            avago_aacs_send_command(aapl, tck_delay_orig);
            aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Resetting original tck_delay of %s\n", tck_delay_orig);
        }
    }
    AAPL_SUPPRESS_ERRORS_POP(aapl);

    if( !rw ) avago_aacs_send_command(aapl, hs1_mode);
    else      aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "The original HS1 communication method \"%s\" failed, changing HS1 communication method to %s.\n", hs1_mode, avago_aacs_send_command(aapl, "sbus_mode"));

    return rw;
}


/** @brief Sends and receives JTAG information and checks the result
 ** @details OPCODE is assumed to be 10 bits. Use avago_jtag for the same functionality without output checking.
 ** @return Data received back from the JTAG command
 **        Returns the aapl->data_char string pointer, which is a C-string holding the command status.
 **/
char *avago_jtag_chk(
    Aapl_t *aapl,       /**< [in] Pointer to AAPL structure */
    int opcode,         /**< [in] The JTAG opcode. */
    int length,         /**< [in] The number of bits to be sent. */
    const char *data,   /**< [in] Data to be sent to scan chain. */
    const char *chk,    /**< [in] Data to compare with TDO. */
    int *failures)      /**< [out] Number of mismatched bits */
{
    int x, fail = 0;
    char *tdo = (char *)aapl_malloc(aapl, length+1, __func__);
    avago_jtag(aapl, opcode, length, data);
    snprintf(tdo, length+1, "%s", aapl->data_char);

    for (x=0; x<length; x++)
    {
        if (chk[x] != 'x' && chk[x] != 'X' && chk[x] != tdo[x])
        {
            if (!fail) aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Mismatched bit(s) for opcode 0x%03x:  ", opcode);
            fail++;
            aapl_log_printf(aapl, AVAGO_DEBUG1, 0, 0, "%d ", length-x-1);
        }
    }
    if (fail) aapl_log_printf(aapl, AVAGO_DEBUG1, 0, 0, "\n");

    if ((aapl->debug & 0xf) >= 2)
    {
        aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__, "TDO:  %s\n", tdo);
        aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__, "CHK:  %s\n", chk);
        aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__, "Fail: ");

        for (x=0; x<length; x++)
        {
            if (chk[x] != 'x' && chk[x] != 'X' && chk[x] != tdo[x])
            aapl_log_printf(aapl, AVAGO_DEBUG1, 0, 0, "%s", tdo[x] == '1' ? "1" : "0");
            else aapl_log_printf(aapl, AVAGO_DEBUG1, 0, 0, "-");
        }
        aapl_log_printf(aapl, AVAGO_DEBUG1, 0, 0, "\n");
    }

    if (failures) *failures = fail;
    snprintf(aapl->data_char, length, "%s", tdo);

    return tdo;
}

/** @brief Sends and receives JTAG information.
 ** @details OPCODE is assumed to be 10 bits. Use avago_jtag_scan for more generic JTAG operations.
 ** @return Data received back from the JTAG command
 **        Returns the aapl->data_char string pointer, which is a C-string holding the command status.
 **        aapl->data is set to length of the returned string.
 **/
char *avago_jtag(
    Aapl_t *aapl,       /**< [in] Pointer to AAPL structure */
    int opcode,         /**< [in] The JTAG opcode. */
    int length,         /**< [in] The number of bits to be sent. */
    const char *data)   /**< [in] Data to be sent to scan chain. */
{
    AAPL_WAIT_FOR_SBUS_THREAD(aapl);
    if (aapl_reconnect(aapl, __func__) < 0) return 0;
    if (!aapl->jtag_fn) aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "JTAG function function not registered.\n");
    else return aapl->jtag_fn(aapl, opcode, length, data, 1);
    return 0;
}

char *avago_jtag_options(
    Aapl_t *aapl,       /**< [in] Pointer to AAPL structure */
    int opcode,         /**< [in] The JTAG opcode. */
    int length,         /**< [in] The number of bits to be sent. */
    const char *data,   /**< [in] Data to be sent to scan chain. */
    BOOL get_tdo)
{
    AAPL_WAIT_FOR_SBUS_THREAD(aapl);
    if (aapl_reconnect(aapl, __func__) < 0) return 0;
    if (!aapl->jtag_fn) aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "JTAG function function not registered.\n");
    else return aapl->jtag_fn(aapl, opcode, length, data, get_tdo);
    return 0;
}



#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS

/** Private function used by avago_jtag_scan and avago_jtag_rd
 ** Exported function below for use outside this file
 **/
static AAPL_INLINE char avago_bit_banged_jtag_int(Aapl_t *aapl, int cycles, int tms, int tdi, int trst_l, BOOL get_tdo)
{
    if (aapl->aacs)
    {
        char buf[64];

        snprintf(buf, 63, "jtag_cycle %d %d %d %d", cycles, tms, tdi, trst_l);
        avago_aacs_send_command(aapl, buf);
        return aapl->data_char[0];
    }
    else
    {
        if (!aapl->bit_banged_jtag_fn)
        {
            aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "Bit banged JTAG function function not registered.\n");
            return 0x30;
        }
        while (cycles > 1)
        {
            aapl->bit_banged_jtag_fn(aapl, tms, tdi, trst_l, 0);
            cycles--;
        }
        return aapl->bit_banged_jtag_fn(aapl, tms, tdi, trst_l, get_tdo) + 0x30;
    }
}

/** Private function to shift dummy bits before the current ASIC */
static AAPL_INLINE int shift_bits_before(Aapl_t *aapl, uint tms)
{
    int devices_after = aapl->curr_chip;
    int devices_before = (aapl->chips - 1) - aapl->curr_chip;
    if (devices_before < 0) devices_before = 0;

    while (devices_after)
    {
        aapl_log_printf(aapl, AVAGO_DEBUG9, __func__, __LINE__, "## Shifting extra bits for devices after current one. tms_mode: %d curr_chip: %d before: %d after:%d\n", tms, aapl->curr_chip, devices_before, devices_after);
        if (tms == 2)
            avago_bit_banged_jtag_int(aapl, 10, /*tms*/ 0, /*tdi*/ 1, /*trst_l*/ 1, /*get_tdo*/ 0);
        else
            avago_bit_banged_jtag_int(aapl, 1, /*tms*/ 0, /*tdi*/ 0, /*trst_l*/ 1, /*get_tdo*/ 0);
        devices_after --;
    }
    return devices_before;
}

/** Private function to shift dummy bits after the current ASIC */
static AAPL_INLINE void shift_bits_after(Aapl_t *aapl, uint tms, int devices_before, int final_tms)
{
    while (devices_before)
    {
        aapl_log_printf(aapl, AVAGO_DEBUG9, __func__, __LINE__, "## Shifting extra bits for devices before current one. tms_mode: %d curr_chip: %d before: %d\n", tms, aapl->curr_chip, devices_before);
        if (tms == 2)
            avago_bit_banged_jtag_int(aapl, 9, /*tms*/ 0, /*tdi*/ 1, /*trst_l*/ 1, /*get_tdo*/ 0);
        devices_before --;
        if (devices_before) avago_bit_banged_jtag_int(aapl, 1, /*tms*/ 0, /*tdi*/ 1, /*trst_l*/ 1, /*get_tdo*/ 0);
    }
    avago_bit_banged_jtag_int(aapl, 1, /*tms*/ final_tms, /*tdi*/ 1, /*trst_l*/ 1, /*get_tdo*/ 0);
}


/** @brief  Sets individual JTAG bits and runs TCK for a specified number of cycles
 ** @return Data received back from the TDO on the last cycle, if requested
 **/
char avago_bit_banged_jtag(Aapl_t *aapl, int cycles, int tms, int tdi, int trst_l, BOOL get_tdo)
{
    return avago_bit_banged_jtag_int(aapl, cycles, tms, tdi, trst_l, get_tdo);
}


/** @brief  Reads JTAG information.
 ** @return Data received back from the JTAG command
 **        Returns the aapl->data_char string pointer, which is a C-string holding the command status.
 **        aapl->data is set to length of the returned string.
 **/
char *avago_jtag_rd(
    Aapl_t *aapl,       /**< [in] Pointer to AAPL structure */
    int opcode,         /**< [in] The JTAG opcode. */
    int length)         /**< [in] The length of the JTAG chain specified by the opcode. */
{
    AAPL_WAIT_FOR_SBUS_THREAD(aapl);
    if (aapl->aacs)
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "jtag %d %04x read",length,opcode);
        avago_aacs_send_command(aapl, buffer);
    }
    else if (aapl_is_jtag_communication_method(aapl))
    {
        char *tdo = (char *)aapl_malloc(aapl, length+1, __func__);

        if( tdo )
        {
            int current_bit = 0;
            int devices_before = 0;
            int final_tms = 1;
            memset(tdo, '0', length+1);
            length --;

            avago_jtag_set_opcode(aapl, opcode);
            avago_bit_banged_jtag_int(aapl, 2, 0, 0, 1, 0);
            avago_bit_banged_jtag_int(aapl, 1, 1, 0, 1, 0);
            avago_bit_banged_jtag_int(aapl, 2, 0, 0, 1, 0);

            if (aapl->chips > 1 && !aapl->jtag_non_daisy_chained)
            {
                devices_before = shift_bits_before(aapl, 1);
                if (devices_before) final_tms = 0;
            }

            while (current_bit <= length)
            {
                tdo[length - current_bit] = avago_bit_banged_jtag_int(aapl, 1, 0, tdo[length - current_bit + 1] - 0x30, 1, 1);
                current_bit++;
            }
            avago_bit_banged_jtag_int(aapl, 1, final_tms, tdo[length - current_bit + 1] - 0x30, 1, 1);

            tdo[length+1] = 0;

            if (devices_before)
            {
                final_tms = 1;
                shift_bits_after(aapl, 1, devices_before, final_tms);
            }

            avago_bit_banged_jtag_int(aapl, 1, 1, 0, 1, 0);
            avago_bit_banged_jtag_int(aapl, 1, 0, 0, 1, 0);

            aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "length: %d, tdo: %s\n", length+1, tdo);

            aapl_log_printf(aapl, AVAGO_DATA_CHAR, __func__, __LINE__, "%s", tdo);
            aapl_free(aapl, tdo, __func__);
        }
    }
    else
    {
        aapl_fail(aapl,__func__,__LINE__,"Implementation missing.\n");
    }
    return aapl->data_char;
}


/** @brief  Reads JTAG information and automatically determines the chain length.
 ** @return On success, returns the aapl->data_char string pointer, which is a C-string holding the JTAG information.
 **         Both *length and aapl->data are set to the length of the returned string.
 **/
char *avago_jtag_rd_len(
    Aapl_t *aapl,       /**< [in] Pointer to AAPL structure */
    int opcode,         /**< [in] The JTAG opcode. */
    int *length)        /**< [out] The length of the JTAG chain specified by the opcode. */
{
    int i;
    uint tdo_size = 100000;
    char *tdo = (char *)aapl_malloc(aapl, tdo_size, __func__);
    char pattern[] = "011001010001111011101100001100111000101001110110101100101101010001101000010010010110001010100110011010000100100101100010101001100010010000000110111011000101001";
    char *in_ptr = pattern + AAPL_ARRAY_LENGTH(pattern);
    char *out_ptr = tdo + tdo_size;
    pattern[AAPL_ARRAY_LENGTH(pattern)-1] = '0';

    AAPL_WAIT_FOR_SBUS_THREAD(aapl);

    avago_jtag_set_opcode(aapl, opcode);
    avago_bit_banged_jtag_int(aapl, 2, 0, 0, 1, 0);
    avago_bit_banged_jtag_int(aapl, 1, 1, 0, 1, 0);
    avago_bit_banged_jtag_int(aapl, 2, 0, 0, 1, 0);

    *--out_ptr = '\0';
    for( i = 0; i < AAPL_ARRAY_LENGTH(pattern); i++ )
        *--out_ptr = avago_bit_banged_jtag_int(aapl, 1, 0, *--in_ptr - '0', 1, 1);

    in_ptr = tdo + tdo_size - 1;
    while( 0 != strncmp(pattern+1, out_ptr, AAPL_ARRAY_LENGTH(pattern)-1) && out_ptr > tdo )
        *--out_ptr = avago_bit_banged_jtag_int(aapl, 1, 0, *--in_ptr - '0', 1, 1);

    *--out_ptr = avago_bit_banged_jtag_int(aapl, 1, 1, *--in_ptr - '0', 1, 1);
    *length = tdo_size - (in_ptr - tdo) - 1;

    *length -= aapl->chips ? aapl->chips - 1 : 0;
    in_ptr +=  aapl->chips ? aapl->chips - 1 - aapl->curr_chip : -aapl->curr_chip;
    in_ptr[*length] = '\0';

    avago_bit_banged_jtag_int(aapl, 1, 1, 0, 1, 0);
    avago_bit_banged_jtag_int(aapl, 1, 0, 0, 1, 0);

    aapl_log_printf(aapl, AVAGO_DEBUG8, __func__, __LINE__, "chip: %u/%u, length: %d, tdo: %s\n", aapl->curr_chip, aapl->chips, *length, in_ptr);
    aapl_log_printf(aapl, AVAGO_DATA_CHAR, __func__, __LINE__, "%s", in_ptr);

    aapl_free(aapl, tdo, __func__);
    aapl->data = *length;
    return aapl->data_char;
}

/** @brief Performs jtag chip level reset. */
void avago_jtag_reset(
    Aapl_t *aapl,       /**< [in] Pointer to AAPL structure */
    int chip)
{
    AAPL_WAIT_FOR_SBUS_THREAD(aapl);
    aapl_log_printf(aapl, AVAGO_DEBUG9, __func__, __LINE__, "Performing JTAG reset (aacs: %x, jtag: %x)\n", aapl->aacs, aapl_is_jtag_communication_method(aapl));
    if( aapl->aacs )
    {
        aapl_set_sbus_tap_reset_sel(aapl, chip, 0);
        avago_aacs_send_command(aapl, "reset");
    }
    if( aapl->aacs || aapl_is_jtag_communication_method(aapl) )
    {
        avago_bit_banged_jtag_int(aapl, 10, 1, 0, 0, 0);
        avago_bit_banged_jtag_int(aapl, 10, 1, 0, 1, 0);
        avago_bit_banged_jtag_int(aapl, 10, 0, 0, 1, 0);
    }
    aapl->prev_opcode = 0;
    aapl_clear_recv_data_valid(aapl);
}

/** @brief Sets single bit in JTAG scan chain */
void avago_jtag_set_bit(
    Aapl_t *aapl,       /**< [in] Pointer to AAPL structure */
    int opcode,         /**< [in] The JTAG opcode. */
    uint length,        /**< [in] The number of bits in the scan chain */
    uint set_bit,       /**< [in] Bit number to set */
    uint value)         /**< [in] Value to set */
{
    char * buffer = (char *)aapl_malloc(aapl, length, __func__);
    avago_jtag_rd(aapl, opcode, length);
    aapl->data_char[(length-1) - set_bit] = value;
    memcpy(buffer, aapl->data_char, length);
    avago_jtag(aapl, opcode, length, buffer);
    aapl_free(aapl, buffer, __func__);
}

/** @brief   Does a JTAG scan of scan chain of arbitary type and length
 ** @details Scan length number of bits from tdi, writing output into tdo.
 **          tms input specifies how many TMS cycles to toggle before starting scan.
 **          1 would signify DR, 2 is IR.
 **/
void avago_jtag_scan(
    Aapl_t *aapl,       /**< Pointer to AAPL structure */
    int length,         /**< Scan length number of bits from tdi. */
    int tms,            /**< How many TMS cycles to toggle before starting scan. */
    const char *tdi,    /**< [in] Bits to output, one per byte. ASCII '0' and '1' characters. */
    char *tdo)          /**< [out] Where bits are stored as ASCII '0' and '1' characters. */
{
    avago_jtag_scan_options(aapl, length, tms, tdi, tdo, AVAGO_RTI);
}


/** @brief   Does a JTAG scan of scan chain of arbitary type and length
 ** @details Scan length number of bits from tdi, writing output into tdo.
 **          tms input specifies how many TMS cycles to toggle before starting scan.
 **          1 would signify DR, 2 is IR.
 **/
void avago_jtag_scan_options(
    Aapl_t *aapl,               /**< Pointer to AAPL structure */
    int length,                 /**< Scan length number of bits from tdi. */
    int tms,                    /**< How many TMS cycles to toggle before starting scan. */
    const char *tdi,            /**< [in] Bits to output, one per byte. ASCII '0' and '1' characters. */
    char *tdo,                  /**< [out] Where bits are stored as ASCII '0' and '1' characters. */
    Aapl_jtag_state_t state)    /**< Starting state, either AVAGO_SHIFT_DR or AVAGO_RTI. */
{
    int devices_before = 0;
    int current_bit = length - 1;
    int final_tms = 1;
    if (state == AVAGO_SHIFT_DR) final_tms = 0;

    AAPL_WAIT_FOR_SBUS_THREAD(aapl);

    if (tms)
    {
        avago_bit_banged_jtag_int(aapl, tms, 1, 0, 1, 0);
        avago_bit_banged_jtag_int(aapl, 2, 0, 0, 1, 0);
    }

    if (aapl->chips > 1 && !aapl->jtag_non_daisy_chained)
    {
        devices_before = shift_bits_before(aapl, tms);
        if (devices_before) final_tms = 0;
    }

    if (tdo)
    {
        while (current_bit)
        {
            tdo[current_bit] = avago_bit_banged_jtag_int(aapl, 1, 0, tdi[current_bit] - 0x30, 1, 1);
            current_bit--;
        }
        tdo[current_bit] = avago_bit_banged_jtag_int(aapl, 1, final_tms, tdi[current_bit] - 0x30, 1, 1);
        tdo[length] = 0;
    }
    else
    {
        while (current_bit)
            avago_bit_banged_jtag_int(aapl, 1, 0, tdi[current_bit--] - 0x30, 1, 0);
        avago_bit_banged_jtag_int(aapl, 1, final_tms, tdi[current_bit] - 0x30, 1, 0);
    }

    if (devices_before)
    {
        if (state != AVAGO_SHIFT_DR) final_tms = 1;
        shift_bits_after(aapl, tms, devices_before, final_tms);
    }

    if (state != AVAGO_SHIFT_DR)
    {
        avago_bit_banged_jtag_int(aapl, 1, 1, 0, 1, 0);
        if (state == AVAGO_RTI) avago_bit_banged_jtag_int(aapl, 1, 0, 0, 1, 0);
    }

    if (aapl->debug >= 9)
    {
        aapl_log_printf(aapl, AVAGO_DEBUG9, __func__, __LINE__, "length: %d, tms: %d, tdi: %.*s\n", length, tms, length, tdi);
        if (tdo) aapl_log_printf(aapl, AVAGO_DEBUG9, __func__, __LINE__, "length: %d, tms: %d, tdo: %.*s\n", length, tms, length, tdo);
    }
}
#endif


/** @brief   Checks sbus_addr to see if it is a broadcast address (including the SerDes broadcast address)
 ** @details Checks to see if sbus_addr is a chip broadcast address (0xfXXX),
 **          a ring broadcast address (0xXfXX), or the SerDes broadcast address (0xXXff)
 ** @return  TRUE if sbus_addr belongs to any of the above broadcast addresses
 **/
BOOL aapl_check_broadcast_address(
    Aapl_t *aapl,           /**< AAPL structure pointer. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    const char *caller,     /**< Caller function, usually __func__. */
    int line,               /**< Caller line number, usually __LINE__. */
    int error_on_match)     /**< Whether to print error on match. */
{
    BOOL match;
    Avago_addr_t addr_struct;
    avago_addr_to_struct(sbus_addr,&addr_struct);

    match = (addr_struct.chip == AVAGO_BROADCAST)
         || (addr_struct.ring == AVAGO_BROADCAST)
         || (addr_struct.sbus == AVAGO_BROADCAST)
         || (addr_struct.sbus > AVAGO_MAX_RING_ADDRESS && addr_struct.sbus <= 0xfc);

    if( match && error_on_match )
        aapl_fail(aapl, caller, line, "Broadcast address (0x%02x) not supported.\n", sbus_addr);

    return match;
}


/** @brief   Checks sbus_addr against firmware revisions given in args list.
 ** @details Checks if the device firmware revision is >= what is in the list.
 **          If the revision has bit 19 set (0x80000), it must match
 **          exactly (not >=) after ignoring bit 19.
 ** <pre>
 ** Example: To check that the firmware revision == 0x1234 or >= 0x5678,
 **          and error if not, call:
 **       aapl_check_firmware_rev(aapl,sbus_addr,__func__,__LINE__,TRUE,2,0x81234,0x5678);
 ** </pre>
 ** @return  TRUE if the firmware revision matches one of those listed, FALSE if not.
 **/
BOOL aapl_check_firmware_rev(
    Aapl_t *aapl,           /**< AAPL structure pointer. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    const char *caller,     /**< Caller function, usually __func__. */
    int line,               /**< Caller line number, usually __LINE__. */
    int error_on_no_match,  /**< Whether to print error on no match. */
    int arg_count,          /**< The number of firmware revision arguments which follow. */
    ...)                    /**< List of valid firmware revisions. */
{
    BOOL match = FALSE;
    int firmware_rev;
    int i;
    Avago_addr_t addr_struct;
    va_list firm_rev_type;
    avago_addr_to_struct(sbus_addr,&addr_struct);

    if (addr_struct.chip == AVAGO_BROADCAST) addr_struct.chip = 0;
    if (addr_struct.ring == AVAGO_BROADCAST) addr_struct.ring = 0;
    if (addr_struct.sbus == AVAGO_BROADCAST) addr_struct.sbus = 0xff;
    sbus_addr = avago_struct_to_addr(&addr_struct);
    firmware_rev = aapl_get_firmware_rev(aapl, sbus_addr) & 0xFFF;
    if( firmware_rev == 0 ) return FALSE;

    va_start(firm_rev_type, arg_count);
    for( i=0; i < arg_count; i++ )
    {
        int firm_rev_check = va_arg(firm_rev_type, int);

        if( (!(firm_rev_check & 0x80000) && (firmware_rev >= (firm_rev_check & 0x0fff))) ||
                                            (firmware_rev == (firm_rev_check & 0x0fff)) )
        {
             match = 1;
             break;
        }
    }

    va_end(firm_rev_type);

    if( !match && error_on_no_match )
        aapl_fail(aapl, caller, line, "Unsupported firmware rev 0x%04X on SBus address %s.\n",
            aapl_get_firmware_rev(aapl, sbus_addr), aapl_addr_to_str(sbus_addr));

    return match;
}

/** @brief   Checks sbus_addr against firmware build given in args list.
 ** @details Checks if the device firmware build exactly matches an entry in the list.
 ** <pre>
 ** Example: To check that the firmware build == 0x1234 or == 0x5678,
 **          and error if not, call:
 **       aapl_check_firmware_build(aapl,sbus_addr,__func__,__LINE__,TRUE,2,0x1234,0x5678);
 ** </pre>
 ** @return  TRUE if the firmware build matches one of those listed, FALSE if not.
 **/
BOOL aapl_check_firmware_build(
    Aapl_t *aapl,           /**< [in] AAPL structure pointer. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    const char *caller,     /**< [in] Caller function, usually __func__. */
    int line,               /**< [in] Caller line number, usually __LINE__. */
    int error_on_no_match,  /**< [in] Whether to print error on no match. */
    int arg_count,          /**< [in] The number of firmware build arguments which follow. */
    ...)                    /**< [in] List of valid firmware builds. */
{
    BOOL match = FALSE;
    int i;
    Avago_addr_t addr_struct;
    va_list firm_build_type;
    int firmware_build;
    avago_addr_to_struct(sbus_addr,&addr_struct);

    if (addr_struct.chip == AVAGO_BROADCAST) addr_struct.chip = 0;
    if (addr_struct.ring == AVAGO_BROADCAST) addr_struct.ring = 0;
    if (addr_struct.sbus == AVAGO_BROADCAST) addr_struct.sbus = 0xff;
    sbus_addr = avago_struct_to_addr(&addr_struct);
    firmware_build = aapl_get_firmware_build(aapl, sbus_addr);
    if( firmware_build == 0 ) return FALSE;

    va_start(firm_build_type, arg_count);
    for( i=0; i < arg_count; i++ )
    {
        int firm_build_check = va_arg(firm_build_type, int);

        if( firmware_build == firm_build_check )
        {
             match = TRUE;
             break;
        }
    }

    va_end(firm_build_type);

    if( !match && error_on_no_match )
        aapl_fail(aapl, caller, line, "SBus address %s is running firmware build 0x%x, which is not supported by %s.\n",
            aapl_addr_to_str(sbus_addr), aapl_get_firmware_build(aapl, sbus_addr), caller);

    return match;
}

/** @cond INTERNAL */

/** @brief   Checks device addr against list of supported devices.
 ** @details Args is the number of arguments passed in for ip_type or process\n
 **          for example: aapl_check_ip_type(aapl, sbus_addr, __func__, __line__, TRUE, 2, AAPL_SDREV_D6_16, AAPL_SDREV_CM4_16);
 ** @return  Returns TRUE if the device is one of the listed types. returns FALSE if not.
 **/
BOOL aapl_check_sdrev(
    Aapl_t *aapl,           /**< [in] AAPL structure pointer. */
    uint addr,              /**< [in] SBus slice address. */
    const char *caller,     /**< [in] Caller function, usually __func__. */
    int line,               /**< [in] Caller line number, usually __LINE__. */
    int error_on_no_match,  /**< [in] Whether to print error on no match. */
    int arg_count,          /**< [in] The number of ip_type arguments which follow. */
    ...)                    /**< [in] List of valid IP types. */
{
    BOOL match = FALSE;
    int i, my_sdrev = aapl_get_sdrev(aapl, addr);
    va_list sdrev_list;

    va_start(sdrev_list, arg_count);
    for( i = 0; i < arg_count; i++ )
    {
        int sdrev = (int)va_arg(sdrev_list, int);
        if( sdrev == my_sdrev )
        {
            match = TRUE;
            break;
        }
    }
    va_end(sdrev_list);

    if( !match && error_on_no_match )
        aapl_fail(aapl, caller, line, "SBus %s, of IP type 0x%x, in process %s, is not supported by %s.\n",
            aapl_addr_to_str(addr),
            aapl_get_ip_type(aapl,addr),
            aapl_get_process_id_str(aapl,addr),
            caller);

    return match;
}

/** @endcond */

/** @brief   Checks addr against list of supported IP types.
 ** @details Args is the number of arguments passed in for ip_type or process\n
 **          for example: aapl_check_ip_type(aapl, addr, __FUNC__, __line__, TRUE, 2, AVAGO_SERDES, AVAGO_QPI);
 ** @details If arg_count is 0, then the list must be terminated by an AVAGO_UNKNOWN_IP entry.
 ** @return  TRUE if the device is one of the listed types, FALSE if not.
 **/
BOOL aapl_check_ip_type(
    Aapl_t *aapl,           /**< [in] AAPL structure pointer. */
    uint addr,              /**< [in] SBus slice address. */
    const char *caller,     /**< [in] Caller function, usually __func__. */
    int line,               /**< [in] Caller line number, usually __LINE__. */
    int error_on_no_match,  /**< [in] Whether to print error on no match. */
    int arg_count,          /**< [in] The number of ip_type arguments which follow. */
    ...)                    /**< [in] List of valid IP types. */
{
    BOOL match = FALSE;
    int i;
    Avago_ip_type_t my_ip_type = aapl_get_ip_type(aapl, addr);
    Avago_addr_t addr_struct;
    va_list ip_types;
    va_start(ip_types, arg_count);

    avago_addr_to_struct(addr,&addr_struct);
    for( i=0; i < arg_count || arg_count == 0; i++ )
    {
        Avago_ip_type_t ip_type = (Avago_ip_type_t)va_arg(ip_types, int);
        if( arg_count == 0 && ip_type == AVAGO_UNKNOWN_IP )
            break;
        if( (uint)ip_type == addr_struct.sbus && ip_type > AVAGO_MAX_RING_ADDRESS )
        {
            match = TRUE;
            break;
        }
        if( addr_struct.chip < AAPL_MAX_CHIPS &&
            addr_struct.ring < AAPL_MAX_RINGS &&
            addr_struct.sbus < 255 &&
            my_ip_type == ip_type )
        {
            match = TRUE;
            break;
        }
    }
    va_end(ip_types);

    if( !match && error_on_no_match )
        aapl_fail(aapl, caller, line, "SBus %s, of IP type 0x%x (%s), in process %s, is not supported by %s.\n",
            aapl_addr_to_str(addr),
            my_ip_type,
            aapl_ip_type_to_str(my_ip_type),
            aapl_get_process_id_str(aapl,addr),
            caller);

    return match;
}

/** @brief   Checks sbus_addr against list of supported IP types.
 ** @details Args is the number of arguments passed in for ip_type or process\n
 **          for example: aapl_check_ip_type(aapl, sbus_addr, __FUNC__, __line__, TRUE, 2, AVAGO_SERDES, AVAGO_QPI);
 ** @return  TRUE if the device is one of the listed types, FALSE if not.
 **/
BOOL aapl_check_ip_type_exists(
    Aapl_t *aapl,           /**< AAPL structure pointer. */
    uint sbus_addr,         /**< [in] SBus address to provide chip & ring to search. */
    const char *caller,     /**< Caller function, usually __func__. */
    int line,               /**< Caller line number, usually __LINE__. */
    int error_on_no_match,  /**< Whether to print error on no match. */
    int arg_count,          /**< The number of ip_type arguments which follow. */
    ...)                    /**< List of valid IP types. */
{
    BOOL match = FALSE;
    int i;
    Avago_addr_t addr_struct;
    va_list ip_types;
    va_start(ip_types, arg_count);

    avago_addr_to_struct(sbus_addr,&addr_struct);
    for( i=0; i < arg_count && !match; i++ )
    {
        Avago_ip_type_t ip_type = (Avago_ip_type_t)va_arg(ip_types, int);
        Avago_addr_t    start, stop, next;
        BOOL            st;

        for( st = aapl_broadcast_first(aapl, &addr_struct, &start, &stop, &next, 0);
             st;
             st = aapl_broadcast_next(aapl, &next, &start, &stop, 0) )
        {
            if (aapl_get_ip_type(aapl, avago_struct_to_addr(&next)) == ip_type )
            {
                match = TRUE;
                break;
            }
        }
    }
    va_end(ip_types);

    if( !match && error_on_no_match )
        aapl_fail(aapl, caller, line, "SBus %s, of IP type 0x%x, in process %s, is not supported by %s.\n",
            aapl_addr_to_str(sbus_addr),
            aapl_get_ip_type(aapl,sbus_addr),
            aapl_get_process_id_str(aapl,sbus_addr),
            caller);

    return match;
}


/** @brief   Check sbus_addr against list of supported process types.
 ** @details Example:  Call aapl_check_process(aapl, sbus_addr, __func__, __LINE__, TRUE, 1, AVAGO_PROCESS_F);
 **          to check that the device at sbus_addr is of type AVAGO_PROCESS_F.
 ** @return  TRUE if the device is one of the listed types, FALSE if not.
 **/
BOOL aapl_check_process(
    Aapl_t *aapl,           /**< AAPL structure pointer. */
    uint sbus_addr,         /**< [in] SBus slice address. */
    const char *caller,     /**< Caller function, usually __func__. */
    int line,               /**< Caller line number, usually __LINE__. */
    int error_on_no_match,  /**< Whether to print error on no match. */
    int arg_count,          /**< The number of process_type arguments which follow. */
    ...)                    /**< List of valid processes. */
{
    BOOL match = FALSE;
    int i;
    va_list processes;
    Avago_addr_t addr_struct;
    Avago_process_id_t process_id;

    avago_addr_to_struct(sbus_addr,&addr_struct);

    if (addr_struct.chip == AVAGO_BROADCAST) addr_struct.chip = 0;
    if (addr_struct.chip >= AAPL_MAX_CHIPS ) return 0;

    sbus_addr = avago_struct_to_addr(&addr_struct);
    process_id = aapl_get_process_id(aapl, sbus_addr);

    va_start(processes, arg_count);
    for( i=0; i < arg_count; i++ )
        if( process_id == (Avago_process_id_t)va_arg(processes, int) )
        {
            match = TRUE;
            break;
        }

    va_end(processes);

    if( !match && error_on_no_match )
        aapl_fail(aapl, caller, line, "SBus %s, IP type 0x%x, in process %s, is not supported by %s.\n",
            aapl_addr_to_str(sbus_addr),
            aapl_get_ip_type(aapl,sbus_addr),
            aapl_get_process_id_str(aapl,sbus_addr),
            caller);

    return match;
}

/** @brief Internal function that returns the tap_gen from an HS1 */
uint avago_get_tap_gen(Aapl_t *aapl)
{
    if (aapl->tap_gen) return aapl->tap_gen;

    if (aapl->aacs)
    {
        avago_aacs_send_command(aapl, "status");

        aapl_log_printf(aapl, AVAGO_DEBUG5, __func__, __LINE__, "The status command returned: \"%s\".\n", aapl->data_char);

        if( strlen(aapl->data_char) >= 1 )
        {
            char *string = strstr(aapl->data_char, "TAP generation:");
            if( string )
            {
                string += 15;
                aapl->tap_gen = aapl_strtoul(string, NULL, 10);
            }
            else aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "Can not determine tap gen. Received: %s.\n", aapl->data_char);
            if (strstr(aapl->data_char, ";;;;;;;;;;"))
            {
                uint supp = aapl->suppress_errors;
                if (supp) aapl->suppress_errors -= 1;
                aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "Invalid response received from HS1 (%s). This may be caused by an unsupported version of HS1 firmware. This may cause some issues, including the inability to reset the device.\n", aapl->data_char );
                if (supp) aapl->suppress_errors += 1;
            }
        }
    }
    else if (aapl->jtag_fn)
    {
#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
        memset(aapl->data_char, 0, 5);
        aapl->sbus_reg[1] = '1';
        aapl->sbus_reg[3] = '1';
        avago_jtag(aapl, 0x0180, 126, aapl->sbus_reg);
        aapl->sbus_reg[1] = '0';
        aapl->sbus_reg[3] = '0';
        avago_jtag(aapl, 0x0180, 126, aapl->sbus_reg);
        if (aapl->data_char[0] == '1' && aapl->data_char[1] == '0' && aapl->data_char[2] == '1' && aapl->data_char[3] == '0') aapl->tap_gen = 3;
        if (aapl->data_char[1] == '1' && aapl->data_char[2] == '0' && aapl->data_char[3] == '1' && aapl->data_char[4] == '0') aapl->tap_gen = 4;
#endif
    }

    aapl_log_printf(aapl, AVAGO_DEBUG5, __func__, __LINE__, "TAP gen: %d\n", aapl->tap_gen);

    return aapl->tap_gen;
}

/** @brief Reset the chip
 ** @details <pre>
 ** Performs a JTAG reset (if using AACS)
 ** Performs a MDIO reset (if using MDIO)
 ** Performs any necessary JTAG setup steps (if using AACS).
 ** </pre>
 ** @return void
 **/
void avago_system_chip_setup(
    Aapl_t *aapl,   /**< [in] Pointer to AAPL structure. */
    int reset,      /**< [in] If set, send the "reset" command to the chip. */
    int chip)       /**< [in] The chip number. */
{
    BOOL rw;

    if (aapl->aacs && aapl->tck_delay)
    {
        char buf[16];
        snprintf(buf, 16, "tck_delay %d", aapl->tck_delay);
        avago_aacs_send_command(aapl, buf);
        aapl_log_printf(aapl, AVAGO_INFO, __func__, __LINE__, "Setting tck_delay of remote server to %d\n", aapl->tck_delay);
    }
    aapl_set_chip(aapl, chip);

#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
    if (reset) avago_jtag_reset(aapl, chip);

    if (!(aapl->debug & AAPL_DONT_UPDATE_JTAG))
    {
        uint jtag_idcode = aapl->jtag_idcode[chip];
        if( jtag_idcode == 0x18e5657f )
        {
            avago_jtag_set_bit(aapl, 0x02b4, 830, 80, '0');
            avago_jtag_set_bit(aapl, 0x02b4, 830, 425, '1');
        }
        else if( (jtag_idcode & 0x0fffffff) == 0x0995957f ||
                 (jtag_idcode & 0x0fffffff) == 0x09c2557f ||
                 (jtag_idcode & 0x0fffffff) == 0x09b1657f )
        {
            avago_jtag_set_bit(aapl, 0x02b4, 883, 2, '0');
            avago_jtag_set_bit(aapl, 0x02b4, 883, 1, '1');
            avago_jtag_set_bit(aapl, 0x02b4, 883, 2, '1');
        }
        else if( (jtag_idcode & 0x0fffffff) == 0x0911457f ||
                 (jtag_idcode & 0x0fffffff) == 0x090b957f )
        {
            avago_jtag_set_bit(aapl, 0x02b4, 1293, 500, '1');
        }
        else if( (jtag_idcode & 0x0fffffff) == 0x0a19457f )
        {
            const int length = 989;
            char *buffer = (char *)aapl_malloc(aapl, length, __func__);

            avago_jtag_rd(aapl, 0x02b4, length);
            memcpy(buffer, aapl->data_char, length);
            buffer[length - 1 -   2] = '1';
            buffer[length - 1 -   1] = '1';
            buffer[length - 1 -  96] = '1';
            buffer[length - 1 -  86] = '1';
            buffer[length - 1 - 443] = '1';
            buffer[length - 1 - 424] = '0';
            buffer[length - 1 - 423] = '1';
            buffer[length - 1 - 426] = '0';
            buffer[length - 1 - 425] = '0';
            buffer[length - 1 - 428] = '1';
            buffer[length - 1 - 427] = '0';
            buffer[length - 1 - 430] = '1';
            buffer[length - 1 - 429] = '0';
            avago_jtag(aapl, 0x02b4, length, buffer);
            aapl_free(aapl, buffer, __func__);
        }
        else if ((jtag_idcode & 0x0fffffff) == 0x0a38857f)
        {
            avago_jtag_set_bit(aapl, 0x02b4, 989, 87, '1');
            avago_jtag_set_bit(aapl, 0x02b4, 989, 1, '1');
            avago_jtag_set_bit(aapl, 0x02b4, 989, 2, '1');
            avago_jtag_set_bit(aapl, 0x02b4, 989, 63, '1');
            avago_jtag_set_bit(aapl, 0x02b4, 989, 963, '1');
        }
    #if 1
        else if( (jtag_idcode & 0x0fffffff) == 0x0a51157f )
        {
            int length = 981;
            char *buffer = (char *)aapl_malloc(aapl, length, __func__);

            avago_jtag_rd(aapl, 0x02b4, length);
            memcpy(buffer, aapl->data_char, length);
            buffer[length - 1 - 63] = '1';
            avago_jtag(aapl, 0x02b4, length, buffer);
            aapl_free(aapl, buffer, __func__);
        }
    #endif
    }

#endif

#if AAPL_ALLOW_MDIO || AAPL_ALLOW_GPIO_MDIO
    if(aapl_is_mdio_communication_method(aapl))
    {
        uint reset_val = avago_mdio_rd(aapl, chip, AVSP_DEVAD, AVAGO_MDIO_REG_RESET);

        aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Initial MDIO reset reg: 0x%0x\n", reset_val);

        reset_val &= 0x1;
        reset_val |= (reset_val << 1);

        AAPL_SUPPRESS_ERRORS_PUSH(aapl);
        rw = avago_diag_sbus_rw_test(aapl, avago_make_addr3(chip, 0, 0xfe), 1);
        AAPL_SUPPRESS_ERRORS_POP(aapl);

        if( rw ) reset_val ^= 3;
        else
        {
            int prev_timeout = aapl->sbus_mdio_timeout;

            avago_mdio_wr(aapl, chip, AVSP_DEVAD, AVAGO_MDIO_REG_RESET, 0x0100 | (reset_val ^ 3));
            aapl->sbus_mdio_timeout = 10000;

            avago_sbus_rd(aapl, avago_make_addr3(chip, 0, 0xfe), 0xfe);
            aapl->sbus_mdio_timeout = prev_timeout;

            aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Final MDIO reset reg: 0x%0x\n", 0x0100 | (reset_val ^ 3));
        }
        avago_mdio_wr(aapl, chip, AVSP_DEVAD, 32785, 0x0000);
    }
#endif

    AAPL_SUPPRESS_ERRORS_PUSH(aapl);
    if (reset) avago_sbus_reset(aapl, avago_make_addr3(chip, 0, AVAGO_BROADCAST), 1);
    rw = avago_diag_sbus_rw_test(aapl, avago_make_addr3(chip, 0, 0xfe), 1);
    AAPL_SUPPRESS_ERRORS_POP(aapl);

    if (!rw && aapl->aacs) rw = hs1_try_mode(aapl, chip, "sbus_mode jtag");
    if (!rw && aapl->aacs) rw = hs1_try_mode(aapl, chip, "sbus_mode slow");
    if (!rw && aapl->aacs) rw = hs1_try_mode(aapl, chip, "sbus_mode mdio");

#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
    if( !rw )
    {
        aapl_set_sbus_tap_reset_sel(aapl, chip, 1);
        if (!rw && aapl->aacs) rw = hs1_try_mode(aapl, chip, "sbus_mode jtag");
        if (!rw && aapl->aacs) rw = hs1_try_mode(aapl, chip, "sbus_mode slow");
        if (!rw && aapl->aacs) rw = hs1_try_mode(aapl, chip, "sbus_mode mdio");
        if (!rw) aapl_set_sbus_tap_reset_sel(aapl, chip, 0);
    }
#endif

    if( !rw )
        avago_diag_sbus_rw_test(aapl, avago_make_addr3(chip, 0, 0xfe), 1);

    if (reset) avago_sbus_reset(aapl, avago_make_addr3(chip, 0, AVAGO_BROADCAST), 1);
}

/** @brief Sets up name and ring info for chip. */
static void aapl_set_chip_name(Aapl_t *aapl, int chip)
{
    int at = avago_find_chip_index(aapl->jtag_idcode[chip], 0xffffffff);
    if( at >= 0 )
    {
        uint ring, rings = avago_chip_id[at].sbus_rings & 0xf;
        uint non_chiplet_rings = rings - (avago_chip_id[at].sbus_rings >> 4);
        Avago_process_id_t process_id = avago_chip_id[at].process_id;

        aapl->chip_name[chip] = avago_chip_id[at].name;
        aapl->chip_rev[chip]  = avago_chip_id[at].rev;
        for( ring = 0; ring < rings; ring++ )
            aapl->process_id[chip][ring] = ring < non_chiplet_rings ? process_id : AVAGO_PROCESS_E;

        if( !aapl->sbus_rings[chip] )
        {
            aapl->sbus_rings[chip] = avago_chip_id[at].sbus_rings;
            for( ring = 0; ring < rings; ring++ )
                aapl->max_sbus_addr[chip][ring] = 1;
        }
#if AAPL_ENABLE_AACS_SERVER
        else aapl_sbus_rings_override = TRUE;
#endif

        aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__,  "IDCODE 0x%08x: %s %s\n", aapl->jtag_idcode[chip], aapl->chip_name[chip], aapl->chip_rev[chip]);
        return;
    }
    if( !aapl->sbus_rings[chip] )
    {
        aapl->sbus_rings[chip] = 1;
        aapl->max_sbus_addr[chip][0] = 1;
    }
    aapl->chip_name[chip] = "";
    aapl->chip_rev[chip] = "";
    aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__,  "IDCODE 0x%08x: %s %s\n", aapl->jtag_idcode[chip], aapl->chip_name[chip], aapl->chip_rev[chip]);
}

/** @brief Internal function that returns chip name from asic_info.h based on the JTAG idcode. */
static void get_chip_name(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    int chip)       /**< [in] Chip number. */
{
    const char *jtag_string = 0;
    uint ring;

    aapl->data_char[0] = 0;

    #ifdef AAPL_CHIP_ID_OVERRIDE0
        if (chip == 0) jtag_string = AAPL_CHIP_ID_OVERRIDE0;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE0
        if (chip == 0) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE0;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE1
        if (chip == 1) jtag_string = AAPL_CHIP_ID_OVERRIDE1;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE1
        if (chip == 1) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE1;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE2
        if (chip == 2) jtag_string = AAPL_CHIP_ID_OVERRIDE2;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE2
        if (chip == 2) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE2;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE3
        if (chip == 3) jtag_string = AAPL_CHIP_ID_OVERRIDE3;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE3
        if (chip == 3) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE3;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE4
        if (chip == 4) jtag_string = AAPL_CHIP_ID_OVERRIDE4;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE4
        if (chip == 4) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE4;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE5
        if (chip == 5) jtag_string = AAPL_CHIP_ID_OVERRIDE5;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE5
        if (chip == 5) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE5;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE6
        if (chip == 6) jtag_string = AAPL_CHIP_ID_OVERRIDE6;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE6
        if (chip == 6) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE6;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE7
        if (chip == 7) jtag_string = AAPL_CHIP_ID_OVERRIDE7;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE7
        if (chip == 7) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE7;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE8
        if (chip == 8) jtag_string = AAPL_CHIP_ID_OVERRIDE8;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE8
        if (chip == 8) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE8;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE9
        if (chip == 9) jtag_string = AAPL_CHIP_ID_OVERRIDE9;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE9
        if (chip == 9) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE9;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE10
        if (chip == 10) jtag_string = AAPL_CHIP_ID_OVERRIDE10;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE10
        if (chip == 10) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE10;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE11
        if (chip == 11) jtag_string = AAPL_CHIP_ID_OVERRIDE11;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE11
        if (chip == 11) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE11;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE12
        if (chip == 12) jtag_string = AAPL_CHIP_ID_OVERRIDE12;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE12
        if (chip == 12) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE12;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE13
        if (chip == 13) jtag_string = AAPL_CHIP_ID_OVERRIDE13;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE13
        if (chip == 13) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE13;
    #endif
    #ifdef AAPL_CHIP_ID_OVERRIDE14
        if (chip == 14) jtag_string = AAPL_CHIP_ID_OVERRIDE14;
    #endif
    #ifdef AAPL_CHIP_ID_HEX_OVERRIDE14
        if (chip == 14) aapl->jtag_idcode[chip] = AAPL_CHIP_ID_HEX_OVERRIDE14;
    #endif

    if (aapl->jtag_idcode[chip] == 0 && jtag_string)
        aapl->jtag_idcode[chip] = aapl_strtoul(jtag_string, NULL, 2);

    aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__,  "User supplied IDCODE for device %d (from aapl->jtag_idcode[chip] or aapl.h defines): 0x%08x\n", chip, aapl->jtag_idcode[chip]);

    AAPL_SUPPRESS_ERRORS_PUSH(aapl);
    if (aapl->jtag_idcode[chip] == 0)
    {
        int rc = aapl->return_code;

        aapl_set_chip(aapl, chip);

        if (aapl->jtag_idcode_fn) aapl->jtag_idcode[chip] = aapl->jtag_idcode_fn(aapl, chip);
        aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__,  "IDCODE returned from avago_get_jtag_idcode for device %d: 0x%08x\n", chip, aapl->jtag_idcode[chip]);

        if ((rc != aapl->return_code) || ( aapl->jtag_idcode[chip] == 0) || (aapl->jtag_idcode[chip] == 0xffffffff))
        {
            uint addr_80;
            uint cntl_addr = avago_make_addr3(chip, 0, 0xfe);

            addr_80 = avago_sbus_rd(aapl, cntl_addr, 0x80);

            aapl->jtag_idcode[chip] = 0;
            if ((addr_80 & 0x3) == 0x00)
            {
                aapl->jtag_idcode[chip] = (avago_sbus_rd(aapl, cntl_addr, 0x81) & 0xff) << 12;
                if ((avago_find_chip_index(aapl->jtag_idcode[chip], 0xff << 12) >= 0) && aapl->jtag_idcode[chip] != 0)
                {
                    avago_sbus_rmw(aapl, cntl_addr, 0x80, 0x1, 0x3);
                    aapl->jtag_idcode[chip] |= (avago_sbus_rd(aapl, cntl_addr, 0x81) & 0xff) << 20;
                    avago_sbus_rmw(aapl, cntl_addr, 0x80, 0x2, 0x3);
                    aapl->jtag_idcode[chip] |= (avago_sbus_rd(aapl, cntl_addr, 0x81) & 0x0f) << 28;
                    avago_sbus_rmw(aapl, cntl_addr, 0x80, 0x0, 0x3);

                    aapl->jtag_idcode[chip] |= 0x57f;
                }
                else aapl->jtag_idcode[chip] = 0;

                aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__,  "IDCODE read from SBus for device %d: 0x%08x\n", chip, aapl->jtag_idcode[chip]);
            }
        }
    }
    AAPL_SUPPRESS_ERRORS_POP(aapl);

    if (aapl->jtag_idcode[chip] == 0x0 && !aapl_is_aacs_communication_method(aapl))
        aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "AAPL has not been configured to use AACS or a JTAG mode, and an override (AAPL_CHIP_ID_OVERRIDE, aapl->jtag_idcode[], or AAPL_CHIP_ID_HEX_OVERRIDE) has not been set for chip %d. This may cause AAPL to not function properly as it has no way of determining exactly what device it is communicating with.\n", chip);

    for( ring = 0; ring < AAPL_MAX_RINGS; ring++ )
        aapl->max_sbus_addr[chip][ring] = 0;

    if (aapl->sbus_rings[chip])
    {
        for( ring = 0; ring < (aapl->sbus_rings[chip]&0xf); ring++ )
            aapl->max_sbus_addr[chip][ring] = 1;
    }

    aapl_set_chip_name(aapl, chip);
}


static int get_number_of_chips(
    Aapl_t *aapl)   /**< [in] Pointer to AAPL structure. */
{
    if (aapl->chips == 0)
    {
#if AAPL_ALLOW_AACS || AAPL_ALLOW_OFFLINE_SBUS
        if(aapl_is_aacs_communication_method(aapl) || aapl->communication_method == AVAGO_OFFLINE)
        {
            avago_aacs_send_command(aapl, "status");

            aapl_log_printf(aapl, AVAGO_DEBUG5, __func__, __LINE__, "The status command returned: \"%s\".\n", aapl->data_char);

            if( strlen(aapl->data_char) >= 1 )
            {
                const char *current_chip = "Current chip:";
                char *string = strstr(aapl->data_char,current_chip);
                if( string ) string = strstr(string,"..");
                if( string )
                {
                    string += 2;
                    aapl->chips = aapl_strtoul(string, NULL, 10);
                    aapl->chips += 1;
                }
                else
                    aapl->chips = 0;
            }
        }
        else
#endif
#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
        if (aapl_is_jtag_communication_method(aapl))
        {
            char tdo;
            avago_bit_banged_jtag_int(aapl, 2, 0, 0, 1, 0);

            avago_bit_banged_jtag_int(aapl, 2, 1, 0, 1, 0);
            avago_bit_banged_jtag_int(aapl, 2, 0, 0, 1, 0);

            avago_bit_banged_jtag_int(aapl, 10 * AAPL_MAX_CHIPS, /*tms*/ 0, /*tdi*/ 1, /*trst_l*/ 1, /*get_tdo*/ 0);

            avago_bit_banged_jtag_int(aapl, 2, 1, 1, 1, 0);
            avago_bit_banged_jtag_int(aapl, 1, 0, 0, 1, 0);

            avago_bit_banged_jtag_int(aapl, 1, 1, 0, 1, 0);
            avago_bit_banged_jtag_int(aapl, 2, 0, 0, 1, 0);

            avago_bit_banged_jtag_int(aapl, AAPL_MAX_CHIPS * 2, /*tms*/ 0, /*tdi*/ 0, /*trst_l*/ 1, /*get_tdo*/ 1);
            tdo = avago_bit_banged_jtag_int(aapl, AAPL_MAX_CHIPS * 2, /*tms*/ 0, /*tdi*/ 0, /*trst_l*/ 1, /*get_tdo*/ 1);
            avago_bit_banged_jtag_int(aapl, AAPL_MAX_CHIPS * 2, /*tms*/ 0, /*tdi*/ 1, /*trst_l*/ 1, /*get_tdo*/ 1);

            if (tdo == '0')
            {
                for (aapl->chips = 0; aapl->chips <= AAPL_MAX_CHIPS - 1; aapl->chips++)
                {
                    char x = avago_bit_banged_jtag_int(aapl, 1, /*tms*/ 0, /*tdi*/ 0, /*trst_l*/ 1, /*get_tdo*/ 1);
                    aapl_log_printf(aapl, AVAGO_DEBUG9, __func__, __LINE__, "Chip %d: %c\n", aapl->chips, x);
                    if (x == '0') break;
                }
            }

            avago_bit_banged_jtag_int(aapl, 2, 1, 0, 1, 0);
            avago_bit_banged_jtag_int(aapl, 1, 0, 0, 1, 0);

            if (aapl->chips == 0)
            {
                aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "No devices found when autodetecting via JTAG. Assuming there is 1 device.\n");
                aapl->chips = 1;
            }
            aapl->prev_opcode = 0;
        }
        else
#endif
        {
            aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "AAPL is not using AACS (TCP) to communicate with devices and AAPL_NUMBER_OF_CHIPS_OVERRIDE has not been defined in aapl.h. Since AACS is the only method to auto-detect the number of devices present, AAPL is assuming there is 1 device.\n");
            aapl->chips = 1;
        }
    }
#if AAPL_ENABLE_AACS_SERVER
    else aapl_num_chips_override = 1;
#endif

    if (aapl->chips > AAPL_MAX_CHIPS)
    {
        aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "AAPL discovered %d devices, but AAPL is compiled to support a maximum of %d devices. Setting aapl->chips to %d.\n", aapl->chips, AAPL_MAX_CHIPS, AAPL_MAX_CHIPS);
        aapl->chips = AAPL_MAX_CHIPS;
    }

    aapl_log_printf(aapl, AVAGO_DEBUG5, __func__, __LINE__, "aapl->chips set to: %d\n", aapl->chips);
    return aapl->chips;
}

/** @cond INTERNAL */

/** @return Returns an Aapl_sdrev_t for the design of the SerDes.
 ** @return If unrecognized device, logs an error message, decrements aapl->return_code and returns AAPL_SDREV_UNKNOWN.
 **/
int aapl_get_sdrev(Aapl_t *aapl, uint addr)
{
    Avago_process_id_t process_id = AVAGO_PROCESS_B;
    static AAPL_THREAD_STORAGE BOOL recursing = FALSE;
    Avago_ip_type_t ip_type = aapl_get_ip_type(aapl,addr);
    if( ip_type == AVAGO_P1 ) return AAPL_SDREV_P1;

    process_id = aapl_get_process_id(aapl,addr);
#if AAPL_ENABLE_D5
    if( ip_type == AVAGO_D5 )
        return AAPL_SDREV_D5_05;
#endif
#if AAPL_ENABLE_D7
    if( ip_type == AVAGO_D7S )
        return AAPL_SDREV_D7S_07;
#endif
#if AAPL_ENABLE_BLACKHAWK
    if( (ip_type == AVAGO_BLACKHAWK) || (ip_type == AVAGO_BLACKHAWK_BROADCAST) )
        return AAPL_SDREV_BH_07;
#endif
#if AAPL_ENABLE_OSPREY
    if( (ip_type == AVAGO_OSPREY) || (ip_type == AVAGO_OSPREY_BROADCAST) )
        return AAPL_SDREV_OS_07;
#endif

    if( ip_type == AVAGO_M4 )
    {
        uint ip_rev;
        if( process_id == AVAGO_PROCESS_B )
            return AAPL_SDREV_CM4_16;
        if( recursing )
            return AAPL_SDREV_CM4;

        recursing = TRUE;
        ip_rev = aapl_get_ip_rev(aapl,addr);
        recursing = FALSE;

        return (ip_rev == 0xe4) ? AAPL_SDREV_CM4 : AAPL_SDREV_OM4;
    }
    if( ip_type == AVAGO_SERDES )
    {
        uint lsb_rev;
        if( process_id == AVAGO_PROCESS_B )
        {
            uint ip_rev;
            if( recursing )
                return AAPL_SDREV_16;

            recursing = TRUE;
            ip_rev = aapl_get_ip_rev(aapl,addr);
            recursing = FALSE;

            return (ip_rev == 0x59 || ip_rev == 0x56 || ip_rev == 0x4b || ip_rev == 0x48 || ip_rev == 0x39) ? AAPL_SDREV_PON : AAPL_SDREV_16;
        }
        else if( process_id == AVAGO_PROCESS_E )
            return AAPL_SDREV_D6_07;

        if( recursing ) return AAPL_SDREV_D6;
        recursing = TRUE;
        lsb_rev = aapl_get_lsb_rev(aapl,addr);
        recursing = FALSE;

        return (lsb_rev == 8) ? AAPL_SDREV_HVD6 : AAPL_SDREV_D6;
    }
    aapl_fail(aapl, __func__, __LINE__, "Addr = 0x%x, Unknown SerDes design.\n", addr);
    return AAPL_SDREV_UNKNOWN;
}

int aapl_get_interrupt_rev(Aapl_t *aapl, uint addr, int int_num)
{
    switch( int_num )
    {
    case 0x25:
        {
            int sdrev = aapl_get_sdrev(aapl, addr);
            if( sdrev == AAPL_SDREV_PON ) return 2;
            if( sdrev == AAPL_SDREV_D6_07 ) return 2;
            if( aapl_get_ip_type(aapl, addr) == AVAGO_M4 && aapl_check_firmware_rev(aapl,addr,0,0,FALSE,1,0x1056) )
                return 2;
            break;
        }
    default: break;
    }
    return 1;
}
/** @endcond */

static void update_ring_info(Aapl_t *aapl, int chip, int ring)
{
    uint data;
    int rc = aapl->return_code;
    Avago_addr_t addr_struct;

    if (aapl->max_sbus_addr[chip][ring] == 0) return;

    avago_addr_init(&addr_struct);
    addr_struct.chip = chip;
    addr_struct.ring = ring;
    addr_struct.sbus = AVAGO_SBUS_CONTROLLER_ADDRESS;
    addr_struct.lane = AVAGO_BROADCAST;

    AAPL_SUPPRESS_ERRORS_PUSH(aapl);
    data = avago_sbus_rd(aapl, avago_struct_to_addr(&addr_struct), 0x02);
    AAPL_SUPPRESS_ERRORS_POP(aapl);

    aapl->max_sbus_addr[chip][ring] =
        (aapl->return_code < rc || data == 0 || data >= 0xfe)
            ? 0
            : data - 1;

    aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__,  "Chip %d (%s %s), ring %d, SBus devices: %d\n", chip, aapl->chip_name[chip], aapl->chip_rev[chip], ring, aapl->max_sbus_addr[chip][ring]);

    if( aapl->max_sbus_addr[chip][ring] == 0 )
    {
        aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "Error occurred while trying to determine the number of devices on chip %d, SBus ring %d. aapl->return_code: %d, 0xfe 0x02 0x02 0x00: %d.\n", chip, ring, rc, data);
        return;
    }

    for( addr_struct.sbus = 1; addr_struct.sbus <= aapl->max_sbus_addr[chip][ring]; addr_struct.sbus++ )
    {
        aapl->ip_type[chip][ring][addr_struct.sbus] = 0xff;
        aapl->spico_running[chip][ring][addr_struct.sbus] = 0;
    }

    for( addr_struct.sbus = 0xff; addr_struct.sbus > AVAGO_MAX_RING_ADDRESS; addr_struct.sbus-- )
    {
        aapl->firm_rev     [chip][ring][addr_struct.sbus] = 0;
        aapl->firm_build   [chip][ring][addr_struct.sbus] = 0;
        aapl->ip_rev       [chip][ring][addr_struct.sbus] = 0xfffe;
        aapl->spico_running[chip][ring][addr_struct.sbus] = 0;
        aapl->ip_type      [chip][ring][addr_struct.sbus] = addr_struct.sbus;
    }

    aapl->ip_type[chip][ring][0xfd] = 0xff;
    aapl->ip_type[chip][ring][0xe0] = 0xff;

    addr_struct.sbus = AVAGO_SBUS_CONTROLLER_ADDRESS;
    aapl_set_ip_type(aapl,avago_struct_to_addr(&addr_struct));
    avago_twi_wait_for_complete(aapl, avago_struct_to_addr(&addr_struct));
}


static void update_chip_info(Aapl_t *aapl, int chips, int chip_reset)
{
    int chip;
    for (chip=0; chip<chips; chip++)
    {
        AAPL_SUPPRESS_ERRORS_PUSH(aapl);
        get_chip_name(aapl, chip);
        AAPL_SUPPRESS_ERRORS_POP(aapl);

        aapl_clear_recv_data_valid(aapl);

        avago_system_chip_setup(aapl, chip_reset, chip);
        if (aapl->jtag_idcode[chip] == 0) get_chip_name(aapl, chip);

        if( !(aapl->capabilities & AACS_SERVER_NO_CRC) )
        {
            if( 0x0995957f == aapl->jtag_idcode[chip] )
            {
                uint addr = avago_make_addr3(chip, 0, 4);
                int iprev = (avago_sbus_rd(aapl, addr, 0xfe) >> 8) & 0xff;
                if( iprev == 0x17 )
                {
                    aapl->jtag_idcode[chip] = 0x09b1657f;
                    aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__, "Correcting IDCODE to 0x%x\n",aapl->jtag_idcode[chip]);
                    aapl_set_chip_name(aapl, chip);
                }
            }
            if( 0x0964257f == aapl->jtag_idcode[chip] )
            {
                uint addr = avago_make_addr3(chip, 0, 6);
                int iprev = (avago_sbus_rd(aapl, addr, 0xfe) >> 8) & 0xff;
                if( iprev == 0xde )
                {
                    aapl->jtag_idcode[chip] = 0x099a557f;
                    aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__, "Correcting IDCODE to 0x%x\n",aapl->jtag_idcode[chip]);
                    aapl_set_chip_name(aapl, chip);
                }
            }
        }

        #ifdef AAPL_PROCESS_ID_OVERRIDE
            aapl->process_id[chip][0] = AAPL_PROCESS_ID_OVERRIDE;
        #else
            if( aapl->process_id[chip][0] == AVAGO_UNKNOWN_PROCESS )
            {
                Avago_process_id_t process_id;
                AAPL_SUPPRESS_ERRORS_PUSH(aapl);
                process_id = get_process_id(aapl, chip, 0);
                AAPL_SUPPRESS_ERRORS_POP(aapl);
                if (aapl->process_id[chip][0] == AVAGO_UNKNOWN_PROCESS)
                    aapl->process_id[chip][0] = process_id;
                else if( process_id != aapl->process_id[chip][0] )
                {
                    int old = aapl->upgrade_warnings;
                    aapl->upgrade_warnings = 0;
                    aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__,  "Chip %d has inconsistent process id information: Ids as both %s and %s.\n",
                        chip, aapl_process_id_to_str((Avago_process_id_t)(aapl->process_id[chip][0])), aapl_process_id_to_str(process_id));
                    aapl->upgrade_warnings = old;
                }
            }
        #endif
    }


    for( chip=0; chip < chips; chip++ )
    {
        int ring;
#if AAPL_ALLOW_CHIPLET_I2C && AAPL_ENABLE_BLACKHAWK
        if( aapl_addr_is_chiplet(aapl, avago_make_addr3(chip,AVAGO_BROADCAST,0)) )
        {
            if( aapl->pmi_fn != avago_pmi_sbus_fn && (aapl->capabilities & AACS_SERVER_PMI) && !(aapl->capabilities & AACS_SERVER_CHIPLET_I2C) )
            {
                aapl_log_printf(aapl, AVAGO_DEBUG7, __func__, __LINE__, "Reverting to avago_pmi_sbus_fn\n");
                aapl->pmi_fn = &avago_pmi_sbus_fn;
            }

            aapl_unreset_chiplet(aapl, chip);
        }
#endif
        for( ring=0; ring < AAPL_MAX_RINGS; ring++ )
            update_ring_info(aapl, chip, ring);
    }
}

/** @brief Fills out the AAPL struct with information about all devices it can communicate with
 ** @return void
 **/
void aapl_get_ip_info(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    int chip_reset) /**< [in] Reset the chip if non-zero. */
{
    int chips;
    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "AAPL Version " AAPL_VERSION ", " AAPL_COPYRIGHT "\n");

    if (aapl->spico_int_only)
    {
        int sbus;
        for( sbus = 1; sbus <= 0xff; sbus++ )
        {
            aapl->ip_type[0][0][sbus] = 0xff;
            aapl->spico_running[0][0][sbus] = 0;
        }

        return;
    }

    {
        AAPL_SUPPRESS_ERRORS_PUSH(aapl);
        avago_aacs_send_command(aapl, "chip");
        AAPL_SUPPRESS_ERRORS_POP(aapl);
    }

    chips = get_number_of_chips(aapl);
    update_chip_info(aapl, chips, chip_reset);
    aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__, "End of get_ip_info.\n");
}

/** @brief Sets the ip_type, ip_rev, spico_running, firm_rev, and lsb_rev fields of aapl.
 ** @param aapl Aapl_t struct
 ** @param sbus_addr sbus address of the sbus device, sbus ring, and chip
 ** @return void
 **/
void aapl_set_ip_type(
    Aapl_t *aapl,   /**< [in] Pointer to Aapl_t structure. */
    uint sbus_addr) /**< [in] SBus slice address. */
{
    uint data;
    uint jtag_idcode;
    Avago_addr_t addr_struct;
    if (aapl_check_broadcast_address(aapl, sbus_addr, __func__, __LINE__, FALSE)) return;

    AAPL_IP_INFO_LOCK;

    avago_addr_to_struct(sbus_addr,&addr_struct);
    if( addr_struct.chip >= AAPL_MAX_CHIPS ) addr_struct.chip = 0;
    if( addr_struct.ring >= AAPL_MAX_RINGS ) addr_struct.ring = 0;

    aapl->ip_type      [AAPL_3D_ARRAY_ADDR(addr_struct)] = AVAGO_UNKNOWN_IP;
    aapl->ip_rev       [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xfffe;
    aapl->spico_running[AAPL_3D_ARRAY_ADDR(addr_struct)] = 0;
    aapl->firm_rev     [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0;
    aapl->firm_build   [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0;
    aapl->lsb_rev      [AAPL_3D_ARRAY_ADDR(addr_struct)] = AAPL_LSB_INIT;

    if(      addr_struct.sbus == 0xfe ) data = AVAGO_SBUS_CONTROLLER;
    else if( addr_struct.sbus == 0xfd ) data = AVAGO_SPICO;
    else                                data = avago_sbus_rd(aapl, sbus_addr, 0xff);
    jtag_idcode = aapl->jtag_idcode[addr_struct.chip];

    switch( data )
    {
    case 0x00:
        if(    jtag_idcode == 0x0912557f
            || jtag_idcode == 0x0954857f
            || jtag_idcode == 0x0954957f
            || jtag_idcode == 0x0972357f
            || jtag_idcode == 0x0975457f
            || jtag_idcode == 0x0901457f
            || jtag_idcode == 0x0912257f
            || jtag_idcode == 0x090b857f
            || jtag_idcode == 0x0911657f
            || jtag_idcode == 0x18e5657f
            || jtag_idcode == 0x0985357f
            || jtag_idcode == 0x099a757f
            )
            data = AVAGO_LINK_EMULATOR;
        break;

    case 0x01:
        if(    jtag_idcode == 0x099a557f &&
               addr_struct.sbus == 0x04
            )
            data = AVAGO_UNKNOWN_IP;
        break;
    case 0x08:
        if( jtag_idcode == 0x14211001 &&
            (addr_struct.sbus == 0x05 || addr_struct.sbus == 0x06)
            )
            data = AVAGO_MLD;
        break;

    case 0x0b:
        if( jtag_idcode == 0x14211001 && addr_struct.sbus == 0x1c )
            data = AVAGO_RAM_PMRO;
        break;

    case 0x11:
        if(    (jtag_idcode == 0x1911357f && addr_struct.sbus < 0x17)
            || (jtag_idcode == 0x090b957f && addr_struct.sbus < 0x16)
            || (jtag_idcode == 0x0911457f && addr_struct.sbus < 0x16)
            )
            data = AVAGO_AVSP_CONTROL_LOGIC;
        break;

#if AAPL_ENABLE_07NM_IP || AAPL_ENABLE_05NM_IP
    case 0x1b:
      {
        int subtype = avago_sbus_rd(aapl, sbus_addr, 0xfe);
        switch( subtype )
        {
        case 0x01: data = AVAGO_VTMON; break;
        case 0x00: data = AVAGO_VTMON; break;
        case 0x04: data = AVAGO_ANCIL; break;
        case 0xf1: data = AVAGO_IO_PMRO; break;
        default: break;
        }
        break;
      }
    case 0x39:
      {
        int subtype = avago_sbus_rd(aapl, sbus_addr, 0xfe);
        switch( subtype )
        {
        case 0x00:
            {
                int regfc = avago_sbus_rd(aapl, sbus_addr, 0xfc);
#             if AAPL_ENABLE_05NM_IP
                if( regfc == 1587192926 )
                    data = AVAGO_D5;
#             endif
#             if AAPL_ENABLE_07NM_IP
                if( regfc == 1600372408 ||
                    regfc == 1600368525 )
                {
                    data = AVAGO_D7S;
                }
#             endif
            }
            break;
        case 0x01:
            if( aapl_get_process_id(aapl, sbus_addr) == AVAGO_PROCESS_E )
                data = AVAGO_D7S;
            else if( aapl_get_process_id(aapl, sbus_addr) == AVAGO_TSMC_05 )
                data = AVAGO_D5;
            break;
        case 0x10: data = AVAGO_NWL; break;
        case 0x80: data = AVAGO_CDMII2MAX_INIT; break;
        case 0x81: data = AVAGO_CDMII2MAX_TARGET; break;
        case 0xc0: data = AVAGO_CUSTOMER_IP_0; break;
        case 0xc1: data = AVAGO_CUSTOMER_IP_1; break;
        case 0xc2: data = AVAGO_CUSTOMER_IP_2; break;
        case 0xc3: data = AVAGO_CUSTOMER_IP_3; break;
        case 0xc4: data = AVAGO_CUSTOMER_IP_4; break;
        case 0xc5: data = AVAGO_CUSTOMER_IP_5; break;
        case 0xc6: data = AVAGO_CUSTOMER_IP_6; break;
        case 0xc7: data = AVAGO_CUSTOMER_IP_7; break;
        case 0xc8: data = AVAGO_CUSTOMER_IP_8; break;
        case 0xc9: data = AVAGO_CUSTOMER_IP_9; break;
        case 0xca: data = AVAGO_CUSTOMER_IP_A; break;
        case 0xcb: data = AVAGO_CUSTOMER_IP_B; break;
        case 0xcc: data = AVAGO_CUSTOMER_IP_C; break;
        case 0xcd: data = AVAGO_CUSTOMER_IP_D; break;
        case 0xce: data = AVAGO_CUSTOMER_IP_E; break;
        case 0xcf: data = AVAGO_CUSTOMER_IP_F; break;
        case 0xf8: data = AVAGO_PROM; break;
        case 0xfa: data = AVAGO_PLDA_CONTROLLER; break;
        case 0xfe: data = AVAGO_MTIP_PCS_400G; break;
        case 0xff: data = AVAGO_MTIP_PCS_800G; break;
        default: if( subtype >= 0xd0 && subtype <= 0xdf ) data = AVAGO_1K_OTP;
            else if( subtype >= 0xe0 && subtype <= 0xef ) data = AVAGO_4K_OTP;
            break;
        }
        break;
      }
      break;
#endif

    case 0x83:
        if( aapl_get_process_id(aapl, sbus_addr) == AVAGO_PROCESS_B )
            data = AVAGO_SLE;
        if( aapl_get_process_id(aapl, sbus_addr) == AVAGO_PROCESS_F )
            data = AVAGO_RAM_PMRO;
        break;

    case 0xc1:
        if( aapl_get_process_id(aapl, sbus_addr) == AVAGO_PROCESS_F )
            data = AVAGO_PANDORA_LSB;
        break;
    }

    if (data <= AVAGO_IP_TYPE_MAX || (data >= AVAGO_IP_TYPE_ALT_RANGE_LO && data < AVAGO_IP_TYPE_ALT_RANGE_HI))
        aapl->ip_type[AAPL_3D_ARRAY_ADDR(addr_struct)] = (Avago_ip_type_t) data;
    else
    {
        aapl->ip_type[AAPL_3D_ARRAY_ADDR(addr_struct)] = AVAGO_UNKNOWN_IP;
        aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "IP type 0x%02x unrecognized for SBus address %s.\n", data, aapl_addr_to_str(sbus_addr));
    }

    AAPL_IP_INFO_UNLOCK;
    switch (aapl->ip_type[AAPL_3D_ARRAY_ADDR(addr_struct)])
    {
    case AVAGO_P1:
    case AVAGO_M4:
    case AVAGO_SERDES:
        switch (aapl_get_process_id(aapl, sbus_addr))
        {
        case AVAGO_PROCESS_E:
        case AVAGO_PROCESS_B:
        case AVAGO_PROCESS_F:
            avago_spico_wait_for_upload(aapl, sbus_addr);
            avago_sbus_wr(aapl, sbus_addr, 0x01, 0x20000000);
            break;
        default:
            aapl->firm_rev  [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
            aapl->firm_build[AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
            break;
        }
        break;
    case AVAGO_SPICO:
        switch (aapl_get_process_id(aapl, sbus_addr))
        {
        case AVAGO_TSMC_05:
        case AVAGO_PROCESS_E:
        case AVAGO_PROCESS_B:
        case AVAGO_PROCESS_F:
            avago_spico_wait_for_upload(aapl, sbus_addr);
            break;
        default:
            aapl->firm_rev  [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
            aapl->firm_build[AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
            aapl->ip_rev    [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
            break;
        }
        aapl->lsb_rev   [AAPL_3D_ARRAY_ADDR(addr_struct)] = AAPL_LSB_IGNORE;
        break;
    case AVAGO_SBUS_CONTROLLER:
        aapl->firm_rev  [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
        aapl->firm_build[AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
        aapl->lsb_rev   [AAPL_3D_ARRAY_ADDR(addr_struct)] = AAPL_LSB_IGNORE;
        break;
#if AAPL_ENABLE_BLACKHAWK
    case AVAGO_BLACKHAWK:
    case AVAGO_OSPREY:
        aapl->spico_running[AAPL_3D_ARRAY_ADDR(addr_struct)] = 0;
        aapl->firm_rev     [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0;
        aapl->firm_build   [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0;
        aapl->lsb_rev      [AAPL_3D_ARRAY_ADDR(addr_struct)] =  avago_sbus_rd(aapl, sbus_addr, BH_REVISION_ID_REG) & 0x0f;
       break;

#endif

    default:
        aapl->spico_running[AAPL_3D_ARRAY_ADDR(addr_struct)] = 0;
        aapl->firm_rev     [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
        aapl->firm_build   [AAPL_3D_ARRAY_ADDR(addr_struct)] = 0xffff;
        aapl->lsb_rev      [AAPL_3D_ARRAY_ADDR(addr_struct)] = AAPL_LSB_IGNORE;
    }
}


#if AAPL_ALLOW_GPIO_JTAG

#define AAPL_GPIO_SET(g) *(aapl->rpi_gpio_access+7) = g
#define AAPL_GPIO_CLR(g) *(aapl->rpi_gpio_access+10) = g
#define AVAGO_GET_GPIO(g) (*(aapl->rpi_gpio_access+13)&(1<<g))


#define AVAGO_INP_GPIO(g) *(aapl->rpi_gpio_access+((g)/10)) &= ~(7<<(((g)%10)*3))
#define AVAGO_OUT_GPIO(g) *(aapl->rpi_gpio_access+((g)/10)) |=  (1<<(((g)%10)*3))


#ifndef BCM2835_GPPUD
#define BCM2835_GPPUD 1
#endif

#ifndef BCM2835_GPPUDCLK0
#define BCM2835_GPPUDCLK0 1
#endif

#ifndef BCM2836_GPIO_PUD_OFF
#define BCM2836_GPIO_PUD_OFF 1
#endif

#define GPPUPPDN0 57
#define GPPUPPDN1 58
#define GPPUPPDN2 59
#define GPPUPPDN3 60

#define AVAGO_GPIO_PUD_OFF 0
#define AVAGO_GPIO_PUD_DOWN 1
#define AVAGO_GPIO_PUD_UP 2


#define AVAGO_GPIO_TDO 26
#define AVAGO_GPIO_TCK 19
#define AVAGO_GPIO_TMS 13
#define AVAGO_GPIO_TDI 6
#define AVAGO_GPIO_TRST_L 5


static void aapl_get_rpi_info(uint *peri_base, uint *rpi_type)
{
    FILE *fp;

    if( fp = fopen("/proc/device-tree/soc/ranges" , "rb") )
    {
        unsigned char buf[20];
        int len = fread(buf, 1, sizeof(buf), fp);

        if( len >= 8 )
        {
            uint base = buf[4]<<24 | buf[5]<<16 | buf[6]<<8 | buf[7];
            if( !base && len >= 12 )
                base = buf[8]<<24 | buf[9]<<16 | buf[10]<<8 | buf[11];

            if (peri_base) *peri_base = base;
            if (rpi_type)
            {
               if (base == 0xFE000000) *rpi_type = 4;
               else *rpi_type = 0;
            }
        }
        fclose(fp);
    }
}

static void aapl_gpio_pud(Aapl_t *aapl, uint8_t pud)
{
    volatile uint32_t* paddr = aapl->rpi_gpio_access + BCM2835_GPPUD/4;
    *paddr = pud;
}

static void aapl_gpio_pudclk(Aapl_t *aapl, uint8_t pin, uint8_t on)
{
    volatile uint32_t* paddr = aapl->rpi_gpio_access + BCM2835_GPPUDCLK0/4 + pin/32;
    uint8_t shift = pin % 32;
    *paddr = (on ? 1 : 0) << shift;
}

static void aapl_set_pud(Aapl_t *aapl, int pin, int pud)
{
    uint rpi_type;
    aapl_get_rpi_info(0, &rpi_type);
    if (rpi_type == 4)
    {
        #warning TODO please fix this
        uint32_t bits, pull;
        int shift = (pin & 0xf) << 1;
        return;
        switch (pud)
        {
            case AVAGO_GPIO_PUD_OFF:  pull = 0; break;
            case AVAGO_GPIO_PUD_UP:   pull = 1; break;
            case AVAGO_GPIO_PUD_DOWN: pull = 2; break;
        }
        bits = *(aapl->rpi_gpio_access + GPPUPPDN0 + (pin>>4));
        bits &= ~(3 << shift);
        bits |= (pull << shift);
        *(aapl->rpi_gpio_access + GPPUPPDN0 + (pin>>4)) = bits;
    }
    else
    {
        aapl_gpio_pud(aapl, pud);
        ms_sleep(1);
        aapl_gpio_pudclk(aapl, pin, 1);
        ms_sleep(1);
        aapl_gpio_pud(aapl, BCM2836_GPIO_PUD_OFF);
        aapl_gpio_pudclk(aapl, pin, 0);
    }
}


static int avago_bit_banged_gpio_jtag_open_fn(Aapl_t *aapl)
{
    void *gpio_map;
    const char *device = "/dev/gpiomem";
    struct flock lockinfo;
    uint peri_base, gpio_base, rpi_type;

    aapl->rpi_gpio_access = 0;

    if( (aapl->socket = open(device, O_RDWR|O_SYNC)) < 0 )
       return aapl_fail(aapl, __func__, __LINE__, "can't open /dev/gpiomem \n");

    lockinfo.l_type = F_WRLCK;
    lockinfo.l_whence = SEEK_SET;
    lockinfo.l_start  = 0;
    lockinfo.l_len    = 0;
    if( 0 != fcntl(aapl->socket, F_SETLKW, &lockinfo) )
    {
        aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "Lock on %s failed: %s\n", device, strerror(errno));
        close(aapl->socket);
        return aapl->socket = -1;
    }

    aapl_get_rpi_info(&peri_base, &rpi_type);
    gpio_base = peri_base + 0x200000;

    gpio_map = mmap(
       NULL,
       0xf4,
       PROT_READ|PROT_WRITE,
       MAP_SHARED,
       aapl->socket,
       gpio_base
    );

    if (gpio_map == MAP_FAILED)
    {
        aapl_fail(aapl, __func__, __LINE__, "mmap error %s (%d)\n", strerror(errno), gpio_map);
        close(aapl->socket);
        return aapl->socket = -1;
    }

    aapl->rpi_gpio_access = (volatile unsigned *)gpio_map;

    aapl_set_pud(aapl, AVAGO_GPIO_TRST_L, AVAGO_GPIO_PUD_UP);
    aapl_set_pud(aapl, AVAGO_GPIO_TCK,    AVAGO_GPIO_PUD_UP);

    AVAGO_INP_GPIO(AVAGO_GPIO_TRST_L);
    AVAGO_OUT_GPIO(AVAGO_GPIO_TRST_L);
    AVAGO_INP_GPIO(AVAGO_GPIO_TDI);
    AVAGO_OUT_GPIO(AVAGO_GPIO_TDI);
    AVAGO_INP_GPIO(AVAGO_GPIO_TMS);
    AVAGO_OUT_GPIO(AVAGO_GPIO_TMS);
    AVAGO_INP_GPIO(AVAGO_GPIO_TCK);
    AVAGO_OUT_GPIO(AVAGO_GPIO_TCK);
    return 0;
}

static int avago_bit_banged_gpio_jtag_close_fn(Aapl_t *aapl)
{
    munmap((void *)aapl->rpi_gpio_access, 4*1024);
    if (aapl->socket) close(aapl->socket);
    aapl->socket = -1;
    aapl->rpi_gpio_access = 0;
    return 0;
}

static BOOL avago_bit_banged_gpio_jtag_fn(Aapl_t *aapl, BOOL tms, BOOL tdi, BOOL trst_l, BOOL get_tdo)
{

    int data_set = 1<<AVAGO_GPIO_TRST_L;
    int data_clr = 1<<AVAGO_GPIO_TMS | 1<<AVAGO_GPIO_TCK;

    if( !aapl->rpi_gpio_access )
    {
        aapl_fail(aapl, __func__, __LINE__, "GPIO access pointer not defined.\n");
        return 0;
    }

    if (tms)
    {
        data_set |= 1<<AVAGO_GPIO_TMS;
        data_clr ^= 1<<AVAGO_GPIO_TMS;
    }

    if (!trst_l)
    {
        data_clr |= 1<<AVAGO_GPIO_TRST_L;
        data_set ^= 1<<AVAGO_GPIO_TRST_L;
    }

    if (tdi)    data_set |= 1<<AVAGO_GPIO_TDI;
    else        data_clr |= 1<<AVAGO_GPIO_TDI;

    if (!aapl->tck_delay)
    {
        AAPL_GPIO_CLR(data_clr);
        AAPL_GPIO_SET(data_set);
        AAPL_GPIO_SET(1<<AVAGO_GPIO_TCK);
        if (get_tdo && AVAGO_GET_GPIO(AVAGO_GPIO_TDO)) return 1;
        return 0;
    }
    {
        int ret = 0;
        get_tdo |= aapl->tck_delay & 0xff0000;
        aapl->tck_delay &= 0xffff;
        if (aapl->tck_delay == 1)
        {
            uint delay = 1;
            while (delay--) AAPL_GPIO_CLR(data_clr);
            AAPL_GPIO_SET(data_set);
            AAPL_GPIO_SET(1<<AVAGO_GPIO_TCK);
            if (get_tdo) ret = AVAGO_GET_GPIO(AVAGO_GPIO_TDO);
        }
        else if (!(aapl->tck_delay & 0xff00))
        {
            uint delay = aapl->tck_delay;
            while (delay--) AAPL_GPIO_CLR(data_clr);
            AAPL_GPIO_SET(data_set);
            AAPL_GPIO_SET(1<<AVAGO_GPIO_TCK);
            if (get_tdo) ret = AVAGO_GET_GPIO(AVAGO_GPIO_TDO);
        }
        else
        {
            uint delay = aapl->tck_delay & 0xff;
            while (delay--) AAPL_GPIO_CLR(data_clr);
            delay = aapl->tck_delay & 0xff;
            while (delay--) AAPL_GPIO_SET(data_set);
            delay = (aapl->tck_delay & 0x00ff00) >> 8;
            while (delay--) AAPL_GPIO_SET(1<<AVAGO_GPIO_TCK);
            if (get_tdo) ret = AVAGO_GET_GPIO(AVAGO_GPIO_TDO);
            delay = (aapl->tck_delay & 0x00ff00) >> 8;
            while (delay--) AAPL_GPIO_SET(1<<AVAGO_GPIO_TCK);
        }
        if (ret) return 1;
        else return 0;
    }
}
#endif

void aapl_clear_recv_data_valid(Aapl_t *aapl)
{
    (void)aapl;
#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
    memset(aapl->recv_data_valid, 0, sizeof(aapl->recv_data_valid));
#endif
}
