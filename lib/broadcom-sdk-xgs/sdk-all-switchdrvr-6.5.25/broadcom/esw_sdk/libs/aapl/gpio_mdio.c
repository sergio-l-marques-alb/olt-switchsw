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
 ** @brief Raspberry PI GPIO example
 **/

#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include "aapl.h"

#if AAPL_ALLOW_GPIO_MDIO

/** @cond INTERNAL */

#define AAPL_LOG_PRINT13 if( aapl->debug >= 13 ) aapl_log_printf
#define AAPL_LOG_PRINT14 if( aapl->debug >= 14 ) aapl_log_printf

typedef enum {mw_data, mw_addr} mw_t;


/** @endcond */

static void aapl_gpio_mdio_send_bit(Aapl_t *aapl, const int out)
{
    (void)aapl;
    GPIO_SET_VALUE(MDIO_port, out);
    MDC_CYCLE;
}

static int aapl_gpio_mdio_get_bit(Aapl_t *aapl)
{
    int bit = 0, count;
    (void)aapl;

    for( count = 0; count < MDIO_READ_DELAY; count++ )
        bit = GPIO_GET_VALUE(MDIO_port);

    MDC_CYCLE;
    return bit;
}

static void aapl_gpio_mdio_start(Aapl_t *aapl, const int op, const int prtad, const int devad)
{

    GPIO_SET_VALUE(MDC_port, 0);


    GPIO_SET_DIR(MDIO_port, PORT_DIR_OUTPUT);
    GPIO_SET_DIR(MDC_port,  PORT_DIR_OUTPUT);

    {
        int at;
        for( at = 0; at < 32; at++ )
            aapl_gpio_mdio_send_bit(aapl,1);
    }
    aapl_gpio_mdio_send_bit(aapl, 0);
    aapl_gpio_mdio_send_bit(aapl, 0);
    aapl_gpio_mdio_send_bit(aapl, op >> 1);
    aapl_gpio_mdio_send_bit(aapl, op & 1);

    aapl_gpio_mdio_send_bit(aapl, (prtad >> 4) & 1);
    aapl_gpio_mdio_send_bit(aapl, (prtad >> 3) & 1);
    aapl_gpio_mdio_send_bit(aapl, (prtad >> 2) & 1);
    aapl_gpio_mdio_send_bit(aapl, (prtad >> 1) & 1);
    aapl_gpio_mdio_send_bit(aapl,  prtad       & 1);

    aapl_gpio_mdio_send_bit(aapl, (devad >> 4) & 1);
    aapl_gpio_mdio_send_bit(aapl, (devad >> 3) & 1);
    aapl_gpio_mdio_send_bit(aapl, (devad >> 2) & 1);
    aapl_gpio_mdio_send_bit(aapl, (devad >> 1) & 1);
    aapl_gpio_mdio_send_bit(aapl,  devad       & 1);
}

static void aapl_gpio_mdio_write(Aapl_t *aapl, const mw_t type, const int prtad, const int devad, const int data)
{
    int at;
    AAPL_LOG_PRINT14(aapl, AVAGO_DEBUG14, __func__, __LINE__, "Type = %d; Port Addr = %d; Dev Addr = %d; Data = 0x%x\n", type, prtad, devad, data);

    aapl_gpio_mdio_start(aapl, /* op = */ (type == mw_data) ? 1 : 0, prtad, devad);
    aapl_gpio_mdio_send_bit(aapl, 1); aapl_gpio_mdio_send_bit(aapl, 0);
    for (at = 15; at >= 0; --at)
    {
        aapl_gpio_mdio_send_bit(aapl, (data >> at) & 1);
    }

    GPIO_SET_DIR(MDIO_port, PORT_DIR_INPUT);
#ifdef MDIO_EXTRA_CYCLES
    MDC_CYCLE; MDC_CYCLE;
#endif
    GPIO_SET_DIR(MDC_port,  PORT_DIR_INPUT);

}

static void aapl_gpio_mdio_read_op(Aapl_t *aapl, int op, const int prtad, const int devad, unsigned int *datap)
{
    int at;

    aapl_gpio_mdio_start(aapl, op, prtad, devad);
    GPIO_SET_DIR(MDIO_port, PORT_DIR_INPUT);
    MDC_CYCLE; MDC_CYCLE;

    *datap = 0;
    for (at = 15; at >= 0; --at)
    {
        *datap = (*datap << 1) | aapl_gpio_mdio_get_bit(aapl);
    }
    AAPL_LOG_PRINT14(aapl, AVAGO_DEBUG14, __func__, __LINE__, "%s prtad %d, devad %d, data = 0x%x\n", op==3?"Read":"Read-Inc", prtad, devad, *datap);

#ifdef MDIO_EXTRA_CYCLES
    MDC_CYCLE; MDC_CYCLE;
#endif
    GPIO_SET_DIR(MDC_port, PORT_DIR_INPUT);
}

static void aapl_gpio_mdio_read(Aapl_t *aapl, const int prtad, const int devad, unsigned int * datap)
{
    aapl_gpio_mdio_read_op(aapl, 3, prtad, devad, datap);
}


static int aapl_gpio_gpio_init(void)
{
    if( !bcm2835_init() )
        return -1;

    GPIO_SET_DIR(MDC_port,  PORT_DIR_OUTPUT);
    GPIO_SET_DIR(MDIO_port, PORT_DIR_OUTPUT);

    return 0;
}

/** @cond INTERNAL */

/** @brief Initializes the GPIO device.
 ** @return On success, returns 0.
 ** @return On error, decrements aapl->return_code and returns -1.
 **/
int avago_gpio_mdio_open_fn(Aapl_t *aapl)
{
    static BOOL initialized = FALSE;
    if( !initialized && aapl_gpio_gpio_init() < 0)
    {
        aapl->socket = -1;
        aapl_fail(aapl,__func__,__LINE__,"Failed to Initialize GPIO\n");
    }
    initialized = TRUE;

    {
        const char *device = "/dev/gpiomem";
        struct flock lockinfo;

        if( (aapl->socket = open(device, O_RDWR)) < 0 )
           return aapl_fail(aapl, __func__, __LINE__, "can't open %s\n", device);

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
    }

    aapl_log_printf(aapl, AVAGO_DEBUG13, __func__, __LINE__, "open status = TRUE\n");
    return 0;
}

/** @brief Closes the GPIO device.
 ** @return On success, returns 0.
 ** @return On error, decrements aapl->return_code and returns -1.
 **/
int avago_gpio_mdio_close_fn(Aapl_t *aapl)
{
    int rc = 0;
    if( aapl->socket >= 0 )
    {
        struct flock lockinfo;
        lockinfo.l_type = F_UNLCK;
        lockinfo.l_whence = SEEK_SET;
        lockinfo.l_start  = 0;
        lockinfo.l_len    = 0;
        fcntl(aapl->socket, F_SETLKW, &lockinfo);
        close(aapl->socket);
        aapl->socket = -1;
    }
    aapl_log_printf(aapl, AVAGO_DEBUG13, __func__, __LINE__, "close status = %d\n", rc);
    return rc;
}

/** @brief   Executes the requested MDIO request.
 ** @return On success, address and write requests return the data just written, and read requests return the data just read.
 ** @return On error, decrements aapl->return_code and returns the value of data passed in.
 **         The only error detected is passing an read-increment mdio_cmd.
 **/
uint avago_gpio_mdio_fn(
    Aapl_t *aapl,
    Avago_mdio_cmd_t mdio_cmd,
    unsigned int port_addr,
    unsigned int dev_addr,
    unsigned int reg_addr,
    unsigned int data)
{
    if( aapl->debug >= 13 )
    {
        const char *cmd_str = mdio_cmd == AVAGO_MDIO_ADDR ? "ADDR"
                            : mdio_cmd == AVAGO_MDIO_WRITE ? "WRITE"
                            : mdio_cmd == AVAGO_MDIO_READ ? "READ" : "WAIT";
        AAPL_LOG_PRINT13(aapl, AVAGO_DEBUG13, __func__, __LINE__,
            "avago_gpio_mdio_fn(aapl, %s, port=0x%x, dev=0x%x, reg=0x%x, data=0x%x\n",
            cmd_str, port_addr, dev_addr, reg_addr, data);
    }

    switch (mdio_cmd)
    {
    case AVAGO_MDIO_ADDR:
        aapl_gpio_mdio_write(aapl, mw_addr, port_addr, dev_addr, reg_addr);
        break;
    case AVAGO_MDIO_WRITE:
        aapl_gpio_mdio_write(aapl, mw_data, port_addr, dev_addr, data);
        break;
    case AVAGO_MDIO_READ:
        aapl_gpio_mdio_read(aapl, port_addr, dev_addr, &data);
        break;
    default:
        aapl_fail(aapl, __func__, __LINE__, "Unrecognized mdio command %d\n", mdio_cmd);
    }

    return data;
}

/** @endcond */

#endif
