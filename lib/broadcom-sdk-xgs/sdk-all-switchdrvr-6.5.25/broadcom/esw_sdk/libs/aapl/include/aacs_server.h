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

/* AAPL (ASIC and ASSP Programming Layer) aacs_server exported functions */

/** Doxygen File Header
 ** @file
 ** @brief Declarations for aacs_server function.
 **/

#ifndef AVAGO_AACS_SERVER_H_
#define AVAGO_AACS_SERVER_H_

#if AAPL_ENABLE_AACS_SERVER
typedef struct
{
    int tcp_port;
    const char * ats_server_host;
    uint ats_server_host_port;
    BOOL keep_connection_open;
    BOOL enable_console_mode;
    BOOL enable_telnet_server;
} Avago_aacs_server_config_t;

/* Create a new AACS server to handle incoming connection at TCP port tcp_port: */
EXT int avago_aacs_server(Aapl_t *aapl, int tcp_port);
EXT int avago_aacs_server_options(Aapl_t *aapl, Avago_aacs_server_config_t *config);
#endif

EXT char *avago_aacs_process_cmd(Aapl_t *aapl, const char *cmd, int *chip_nump, int *ring_nump);

#ifndef SWIG
#if AAPL_ALLOW_THREAD_SUPPORT && AAPL_ENABLE_PS2
typedef struct
{
    Aapl_t *aapl_upstream;
    Aapl_t *aapl_downstream;
    BOOL enable_console_mode;
} Aapl_server_cntl_t;

EXT void *aapl_telnet_thread(void *aapl_server_cntl);
#endif
#endif

#endif
