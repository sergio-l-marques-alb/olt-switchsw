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
 ** @brief Implementation of AACS Server functionality.
 **/

#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include "aapl.h"

#if AAPL_ENABLE_AACS_SERVER
static struct sockaddr_in client_IPaddr, client_IPaddr_ts;
static struct sockaddr_in client_IPaddr_lock[2];
static char aapl_client_username[128];
BOOL aapl_sbus_rings_override = FALSE;
BOOL aapl_num_chips_override = FALSE;

#if !defined __MINGW32__ && !defined _MSC_VER
#include <arpa/inet.h>
#endif

#if AAPL_ENABLE_PS2 && AAPL_ENABLE_FILE_IO && AAPL_ENABLE_AACS_SERVER
#include <sys/stat.h>
#endif

#if AAPL_ENABLE_CONSOLE

    pthread_mutex_t console_logging_mutex;
    #define AAPL_CONSOLE_LOGGING_LOCK \
        {if (aapl->debug & AAPL_DEBUG_LOCK) aapl_log_printf(aapl, AVAGO_INFO, __func__, __LINE__, "Attempting to lock.\n"); \
        pthread_mutex_lock(&console_logging_mutex); \
        if (aapl->debug & AAPL_DEBUG_LOCK) aapl_log_printf(aapl, AVAGO_INFO, __func__, __LINE__, "Lock complete.\n");}
    #define AAPL_CONSOLE_LOGGING_UNLOCK \
        {pthread_mutex_unlock(&console_logging_mutex); \
        if (aapl->debug & AAPL_DEBUG_LOCK) aapl_log_printf(aapl, AVAGO_INFO, __func__, __LINE__, "Unlock.\n");}

#include <setjmp.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <curses.h>
static BOOL enable_console_mode = FALSE;
static WINDOW *stdout_title;
static WINDOW *stdout_window;
static WINDOW *stderr_title;
static WINDOW *stderr_window;
static WINDOW *conn_title;
static WINDOW *conn_window;
static FILE *aacs_server_log;

static void *aapl_mouse_thread(void *x);

static void update_windows(void)
{
    char buf[512];
    int parent_y, parent_x;
    (void) parent_y;

    getmaxyx(stdscr, parent_y, parent_x);

    wmove(stderr_title, 0, 0);
    wmove(stdout_title, 0, 0);
    wmove(conn_title, 0, 0);

    snprintf(buf, parent_x, "---------- %s %s", "Messages", "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
    wprintw(stdout_title, "%s", buf);
    snprintf(buf, parent_x, "---------- %s %s", "Connection info", "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
    wprintw(conn_title, "%s", buf);
    snprintf(buf, parent_x, "---------- %s %s", "Errors", "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
    wprintw(stderr_title, "%s", buf);

    wrefresh(stdout_title);
    wrefresh(stdout_window);
    wrefresh(stderr_title);
    wrefresh(stderr_window);
    wrefresh(conn_title);
    wrefresh(conn_window);
}

bool starts_with(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

void console_logging_fn(Aapl_t *aapl, Aapl_log_type_t log_sel, const char *buf, size_t len)
{
    AAPL_CONSOLE_LOGGING_LOCK;
    if (starts_with("update AAPL console log", buf) || strstr(buf, "opened new connection") || strstr(buf, "closed connection"))
    {
        char buf2[512], lock0[64], lock1[64];
        uint parent_y, parent_x;
        (void) parent_y;

        sprintf(lock0, "%s",  inet_ntoa(client_IPaddr_lock[0].sin_addr));
        sprintf(lock1, "%s",  inet_ntoa(client_IPaddr_lock[1].sin_addr));

        getmaxyx(stdscr, parent_y, parent_x);
        if (parent_x >= sizeof(buf2)) parent_x = sizeof(buf2);

        wmove(conn_window, 0, 0);
        if (starts_with("update AAPL console log", buf))
            wmove(conn_window, 1, 0);
        else
        {
            snprintf(buf2, parent_x, "\n%s", buf);
            if (strlen(buf2) > 1 && buf2[strlen(buf2)-1] == '\n') buf2[strlen(buf2)-1] = 0;
            wprintw(conn_window,     "%s\n", buf2);
        }

        wprintw(conn_window,     "commands:  %10d  warnings/errors: %5d/%5d\n", aapl->commands, aapl->warnings, aapl->verbose);
        wprintw(conn_window,     "tck_delay: 0x%8x  debug/verbose:   %5d/%5d\n", aapl->tck_delay, aapl->debug, aapl->verbose);
        if (*aapl->chip_name[0])
            wprintw(conn_window, "chips:     %10d  chip[0]:         %11s  tap_gen %d\n", aapl->chips, aapl->chip_name[0], aapl->tap_gen);
        else
            wprintw(conn_window, "chips:     %10d  chip[0]:          0x%08x  tap_gen %d\n", aapl->chips, aapl->jtag_idcode[0], aapl->tap_gen);
        snprintf(buf2, parent_x, "Last user: %10s  Locked by: %s %s", aapl_client_username, lock0, lock1);
        wprintw(conn_window,     "%s", buf2);
        if (starts_with("update AAPL console log", buf))
        {
            update_windows();
            AAPL_CONSOLE_LOGGING_UNLOCK;
            return;
        }
    }

    if (log_sel == AVAGO_ERR || log_sel == AVAGO_WARNING)
    {
        aapl->last_err = buf;
        wprintw(stderr_window, "%s", buf);
    }
    else wprintw(stdout_window, "%s", buf);


    if (aacs_server_log) fprintf(aacs_server_log, "%s", buf);

    update_windows();
    AAPL_CONSOLE_LOGGING_UNLOCK;
}

static void console_teardown(Aapl_t *aapl)
{
    delwin(stdout_title);
    delwin(stdout_window);
    delwin(stderr_title);
    delwin(stderr_window);
    delwin(conn_title);
    delwin(conn_window);
    endwin();
    aapl->enable_stream_logging = 1;
    aapl->enable_stream_err_logging = 1;
    aapl_register_logging_fn(aapl, 0, 0, 0);
    if (aacs_server_log) fclose(aacs_server_log);
}

static void console_setup(Aapl_t *aapl)
{
    int parent_x, parent_y;
    int stdout_size, stderr_size, conn_size;

    aapl_client_username[0] = 0;

    initscr();
    noecho();
    start_color();

    curs_set(FALSE);
    getmaxyx(stdscr, parent_y, parent_x);

    stderr_size = 5;
    conn_size   = 5;
    stdout_size = parent_y - stderr_size - conn_size - 3;

    conn_title =    newwin(1,           parent_x, 0, 0);
    conn_window =   newwin(conn_size,   parent_x, 1, 0);
    stdout_title  = newwin(1,           parent_x, 1 + conn_size, 0);
    stdout_window = newwin(stdout_size, parent_x, 1 + conn_size + 1, 0);
    stderr_title  = newwin(1,           parent_x, 1 + conn_size + 1 + stdout_size,          0);
    stderr_window = newwin(stderr_size, parent_x, 1 + conn_size + 1 + stdout_size + 1,      0);

    scrollok(stderr_window,TRUE);
    scrollok(stdout_window,TRUE);
    scrollok(conn_window,TRUE);

    wmove(stderr_window, 0, 0);
    wmove(stdout_window, 0, 0);
    wmove(conn_window, 0, 0);

    init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);

    mouseinterval(0);

    update_windows();

    mousemask(ALL_MOUSE_EVENTS, NULL);
    keypad(stdscr, TRUE);

    aacs_server_log = fopen("/home/pi/log/server.log-0", "w");
    aapl->enable_stream_logging = 0; /* don't send anything to stdout */
    aapl->enable_stream_err_logging = 0;
    aapl_register_logging_fn(aapl, console_logging_fn, 0, 0);
    ms_sleep(200);
    timeout(1);
    getch();
    timeout(-1);
    console_logging_fn(aapl, AVAGO_INFO, "update AAPL console log", 0);
}
#endif

#endif

#if AAPL_ENABLE_MAIN || AAPL_ENABLE_AACS_SERVER
static time_t last_client_time = 0;

#define EQCN(str1,str2,len) (! aapl_strncasecmp(str1, str2, len))
#define EOS               '\0'
#define ISNUL(cp)         (*(cp) == EOS)
#define ISEOL(cp)         ((*(cp) == '#') || ISNUL(cp))
#define ISTERM(cp)        (isspace(*(cp)) || ISEOL(cp))
#define SKIPSPACE(cp)     while (isspace(*(cp))) ++(cp)
#define ISCMD(cp,cmd,len) (EQCN(cp, cmd, len) && ISTERM((cp) + len))
#define CKHELP(cp) (((cp)[0] == '-') && (((cp)[1] == '?') || ((cp)[1] == 'h') || ((cp)[1] == 'H')) && ISTERM((cp) + 2))
#define CKSKIPSPACE(aapl, result, cmd, cp1,cp2,errfunc) (cp1) = (cp2); if (! ISSPACE(cp1)) {errfunc(aapl, result, cmd); return;} ++(cp1); SKIPSPACE(cp1)
#define ISSPACE(cp)            CURRIS(cp, ' ', '\t')
#define CURRIS(cp,ch1,ch2)  ((*   (cp)  == (ch1)) || (*(cp) == (ch2)))
#define EQI(str1,str2,len) (! aapl_strncasecmp(str1, str2, len))
#define JTAG_INST_BITS 10
#define CHAR_TO_NUM(char)  ((char)  - '0')


#define RES_CLOSE "(close)"
#define RES_EXIT  "(exit)"


const char *HELP_STR = "Valid commands are: sbus, jtag, i2c"
#if AAPL_ALLOW_MDIO || AAPL_ALLOW_GPIO_MDIO
    ", mdio"
#endif
    ", set_debug, chips, chipnum, version, status, send, help, close, exit, spico_int, commands, sleep, tck_delay"
#if AAPL_ENABLE_ATE_VEC
    ", ate_vec"
#endif
#if AAPL_ENABLE_DIAG
    ", diag"
#endif
#if AAPL_ENABLE_PS2 && AAPL_ALLOW_SYSTEM_I2C && AAPL_ENABLE_FLOAT_USAGE && defined I2C_SMBUS_READ
    ", devices, clken, clkread, clkset, clkdrv, iread, iset, ven, vread, vset, wread"
    ", smbus_wb, smbus_wbd, smbus_wwd, smbus_rbd, smbus_rwd, smbus_sniff, reset_supplies"
#endif
#if AAPL_ENABLE_BLACKHAWK
    ", pmi"
#endif
#if AAPL_ENABLE_PS2 && AAPL_ENABLE_FILE_IO && AAPL_ENABLE_AACS_SERVER
    ", ps2_update"
#endif
    ", reset, sbus_reset, cli, lock, unlock, allow, log_dump";

#if AAPL_ENABLE_PS2_KEYSTONE
const char *KEYSTONE_HELP = ", kclken, kclkread";
#endif

const char *HELP_END = ". Command batching supported.";


/** @details     Parse a hex number and return the equivalent binary number,
 **              with *endp modified to the first char after the token.
 ** @param cp    String that should be all hex digits.
 ** @param endp  Pointer to string to return.
 ** @param min   Minimum token length in chars (must be >= 1).
 ** @param max   Maximum token length in chars.
 ** @return uint Binary equivalent, or 0 for any error, with *endp set to the
 **              original cp (as a failure flag).
 **/

static uint aapl_num_from_hex(const char *cp, const char **endp, int min, int max)
{
    uint result = 0;
    const char * start = *endp = cp;

    while (isxdigit(*cp))
    {
        if (cp - start >= max) return(0);
        result = (result << 4) | (uint)(isdigit(*cp) ? (*cp - '0') : (10 + tolower(*cp) - 'a'));
        ++cp;
    }
    if ((cp - start < min) || (! ISTERM(cp))) return(0);
    *endp = cp;
    return(result);
}

/** @details     Parse an ASCII binary number possibly containing 'x'/'X'
 **              digits, and return the equivalent binary number, with *endp
 **              modified to the first char after the token.
 ** @param cp    String that should be all '0'/'1'/'x'/'X' digits.
 ** @param endp  Pointer to string to return.
 ** @param min   Minimum token length in chars (must be >= 1).
 ** @param max   Maximum token length in chars.
 ** @param maskp Pointer to a 32-bit mask value to set, default = all 1s, but
 **              returns 0s for 'x'/'X' meaning bits not to be changed
 **              (read/modify/write).
 ** @return uint Binary equivalent, or 0 for any error, with *endp set to the
 **              original cp (as a failure flag); and with *maskp (always)
 **              modified to reflect the data value.  In case of an empty
 **              string (following a data value consisting of just "z"),
 **              assuming min >= 1, returns 0 with an apparent error, but with
 **              mask = 0, and the caller can proceed from there.
 **/

static uint aapl_num_from_bin(const char *cp, const char **endp, int min, int max, uint *maskp)
{
    uint result = 0;
    uint mask   = 0;
    const char * start = *endp = cp;

    while ((*cp == '0') || (*cp == '1') || (*cp == 'x') || (*cp == 'X'))
    {
        if (cp - start >= max) return(0);

        if ((*cp == '0') || (*cp == '1'))
        {
            result = (result << 1) | (uint)(*cp - '0');
            mask = (mask << 1) | 1;
        }
        else
        {
            result <<= 1;
            mask <<= 1;
        }
        ++cp;
    }
    *maskp = mask;

    if ((cp - start < min) || (! ISTERM(cp))) return(0);
    *endp = cp;
    return(result);
}



#define FAILRET(rc,fd1,fd2) {if ((fd1) >= 0) close(fd1); if ((fd2) >= 0) close(fd2); return rc;}




static void cmd_error(Aapl_t *aapl, char *result, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(result, fmt, ap);

    aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "%s\n", result);

    memmove(result+19, result, strlen(result)+1);
    memcpy(result, "AACS server ERROR: ", 19);
    va_end(ap);
}




static BOOL sbus_parse_da_cmd(Aapl_t *aapl, const char * cmd, const char ** cpp, uint * valp, const int argnum, const char * valname, char * result)
{
    const char * cp2;
    uint val = aapl_num_from_hex(*cpp, &cp2, 2, 2);

    if (cp2 == *cpp)
    {
        cmd_error(aapl, result, "Invalid \"sbus\" command ignored: Arg %d = <%s> must be 2 hex digits. Got: \"%s\".", argnum, valname, cmd);
        return(FALSE);
    }
    SKIPSPACE(cp2);
    *cpp  = cp2;
    *valp = val;
    return(TRUE);
}


static int sbus_parse_data(Aapl_t *aapl, const char * cmd, const char ** cpp, uint * datap, uint * maskp, char * result)
{
    const char * cp = *cpp;
    const char * cp2;
    int  reslen;
    uint data = 0;




    if ((*cp == 'z') || (*cp == 'Z'))
    {
        data = aapl_num_from_bin(++cp, &cp2, 1, 31, maskp);

        if (cp == cp2)
        {
            if (! ISTERM(cp + 1))
            {
                cmd_error(aapl, result, "Invalid \"sbus\" command ignored. Arg 4 = <data> 'z' prefix must be followed by 0-31 '0'/'1'/'x'/'X' chars, but got: \"%s\".", cmd);
                return(0);
            }
        }

        reslen = 32;
    }


    else
    {
        int len;
        *maskp = 0xffffffff;

        for (cp2 = cp; isxdigit(*cp2) || (*cp2 == 'x') || (*cp2 == 'X'); ++cp2)
        /* null */;

        len = cp2 - cp;

        if      ( len ==  2) data = aapl_num_from_hex(cp, &cp2,  2,  2);
        else if ( len ==  8) data = aapl_num_from_bin(cp, &cp2,  8,  8, maskp);
        else if ((len == 10) && EQCN(cp, "0x", 2))
                   {cp += 2; data = aapl_num_from_hex(cp, &cp2,  8,  8);}
        else if ( len == 32) data = aapl_num_from_bin(cp, &cp2, 32, 32, maskp);
        else cp2 = cp;

        if (cp == cp2)
        {
            cmd_error(aapl, result, "Invalid \"sbus\" command ignored. Arg 4 = <data> must be 2-hex, 8-bin, 0x-8-hex, 32-bin, or \"z\" prefix variable-length, but got: \"%s\".", cmd);
            return(0);
        }
        reslen = ((len <= 8) ? 8 : 32);
    }
    *cpp   = cp2;
    *datap = data;
    return(reslen);

}



static BOOL check_term(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (!cp || ISTERM(cp)) return(TRUE);

    cmd_error(aapl, result, "Unexpected extra word(s) on command line, ignored. Got: \"%s\".", cmd);
    return(FALSE);
}


static void sbus_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result, int * chip_nump, int * ring_nump)
{
    const char * cp2;
    uint new_chip = 0x10;
    uint new_ring = 0x10;
    uint sa;
    uint data_out;
    uint da, scmd, data, mask = 0xffffffff;

    int reslen;

    SKIPSPACE(cp);

    if (ISEOL(cp))
    {
        cmd_error(aapl, result, "Invalid \"sbus\" command ignored. Must be: \"sbus <sa> <da> <cmd> <data> [# <comment>]\".");
        return;
    }


    sa = aapl_num_from_hex(cp, &cp2, 2, 4);

    if (cp2 == cp)
    {
        cmd_error(aapl, result, "Invalid \"sbus\" command ignored: Arg 1 = <sa> must be 2-4 hex digits. Got: \"%s\".", cmd);
        return;
    }
    if (cp2 - cp == 4) new_chip = (sa & 0xf000) >> 12;
    if (cp2 - cp >= 3) new_ring = (sa & 0x0f00) >>  8;
    cp = cp2; SKIPSPACE(cp);


    if (! (sbus_parse_da_cmd(aapl, cmd, &cp, &da,   2, "da",  result)
        && sbus_parse_da_cmd(aapl, cmd, &cp, &scmd, 3, "cmd", result)
        && ((reslen = sbus_parse_data(aapl, cmd, &cp, &data, &mask, result)))))
    {return;}


    if (! check_term(aapl, cmd, cp, result)) return;
    if (new_chip < 0x10) *chip_nump = new_chip;
    if (new_ring < 0x10) *ring_nump = new_ring;


    sa |= (*chip_nump << 12) | (*ring_nump << 8);

    data_out = ((scmd == 1 && mask != 0xffffffff) ? avago_sbus_rmw(aapl, sa, da, data, mask) :
                (scmd == 1)                       ? avago_sbus_wr( aapl, sa, da, data) :
                                                    avago_sbus(    aapl, sa, da, scmd, data, /* recv_data_back = */ 1));

    aapl_hex_2_bin(result, data_out, /* underscore_en = */ 0, /* bits */ 32);
    if (reslen == 8) strcpy(result, result + 24);

}


#if AAPL_ALLOW_I2C || AAPL_ALLOW_SYSTEM_I2C
static void i2c_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    typedef enum { cmd_read = 0, cmd_write, cmd_writeread} i2c_cmd_t;
    unsigned int i2c_address;
    const char *cp2;
    unsigned char buffer[256];
    i2c_cmd_t command;
    int num_bytes_r = 0, num_bytes = 0;

    SKIPSPACE(cp);
    if( *cp == '\0' )
    {
BadI2cCommand:
        sprintf(result, "ERROR: Invalid \"i2c\" command ignored. Must be: "
        "\"i2c r <addr> <num_bytes> [# <comments>]\", "
        "\"i2c w <addr> <byte>[ <byte>...] [# <comments>]\", or "
        "\"i2c wr <addr> <num_bytes> <byte>[ <byte>...] [# <comments>]\". "
        "Got: \"%s\".", cmd);
        return;
    }

    if(      (cp[0] == 'r' || cp[0] == 'R') && isspace(cp[1]) )
        command = cmd_read;
    else if( (cp[1] == 'r' || cp[1] == 'R') &&
             (cp[0] == 'w' || cp[0] == 'W') && isspace(cp[2]) )
    {
        cp++;
        command = cmd_writeread;
    }
    else if( (cp[0] == 'w' || cp[0] == 'W') && (isspace(cp[1]) || cp[1]=='\0') )
        command = cmd_write;
    else
    {
        sprintf(result, "ERROR: Invalid \"i2c\" command ignored. Arg 1 must be \"r\", \"w\", or \"wr\".  Got \"%s\".", cmd);
        return;
    }
    cp++; SKIPSPACE(cp);

    i2c_address = aapl_num_from_hex(cp, &cp2, 1, 2);
    if( cp2 == cp || i2c_address > 0x7f )
    {
        sprintf(result, "ERROR: Invalid \"i2c\" command ignored. Arg 2 must be a hex I2C address in the range 00..7f. Got: \"%s\".", cmd);
        return;
    }
    cp = cp2; SKIPSPACE(cp);

    if( *cp == '\0' )
        goto BadI2cCommand;

    if( command == cmd_read || command == cmd_writeread )
    {
        num_bytes_r = (int) aapl_strtol(cp, (char **)&cp2, 10);
        if( cp2 == cp || !(isspace(*cp2) || *cp2 == '\0') || num_bytes_r < 0 || num_bytes_r > (int)sizeof(buffer) )
        {
            sprintf(result, "ERROR: Invalid \"i2c %s\" command ignored. Arg 3 must be a number of bytes in the range 1..%d. Got: \"%s\".",
                            command==cmd_read ? "r" : "wr", (int)sizeof(buffer), cmd);
            return;
        }
        cp = cp2; SKIPSPACE(cp);
    }

    if( command == cmd_write || command == cmd_writeread )
    {
        while( *cp )
        {
            int value;
            if( num_bytes >= (int)sizeof(buffer) )
            {
                sprintf(result, "ERROR: Invalid \"i2c %s\" command ignored, too many bytes specified, limit is %d. Got: \"%s\".",
                                command==cmd_write ? "w" : "wr", (int)sizeof(buffer), cmd);
                return;
            }
            value = aapl_num_from_hex(cp, &cp2, 1, 2);
            if( cp2 == cp || value > 0xff )
            {
                sprintf(result, "ERROR: Invalid \"i2c %s\" command ignored. Arg %d must be a hex byte value in the range 00..ff. Got: \"%s\".",
                                command==cmd_write ? "w" : "wr", 3 + num_bytes, cmd);
                return;
            }
            cp = cp2; SKIPSPACE(cp);
            buffer[num_bytes++] = value;
        }
        if( num_bytes == 0 )
        {
            sprintf(result, "ERROR: Invalid \"i2c %s\" command ignored. "
                            "Must specify one or more bytes to send. "
                            "Invoke with no arguments for a usage summary.",
                                command==cmd_write ? "w" : "wr");
            return;
        }
    }

#if AAPL_ENABLE_PS2 && AAPL_ALLOW_SYSTEM_I2C
    if( aapl->i2c_fd < 0 ) avago_system_i2c_open_fn(aapl);
    if( aapl->i2c_fd < 0 ) aapl_fail(aapl, __func__, __LINE__, "Failed to register i2c file descriptor.\n");
#endif

    if( command == cmd_write || command == cmd_writeread )
    {
        if( avago_i2c_write(aapl, i2c_address, num_bytes, buffer) < 0 )
        {
            sprintf(result, "ERROR: i2c command \"%s\" failed.", cmd);
            return;
        }
        if( command == cmd_write )
            sprintf(result, "Address 0x%02x, bytes written: %d.", i2c_address, num_bytes);
    }
    if( command == cmd_read || command == cmd_writeread )
    {
        int i;
        char *ptr = result;
        if( avago_i2c_read(aapl, i2c_address, num_bytes_r, buffer) < 0 )
        {
            sprintf(result, "ERROR: i2c command \"%s\" failed.", cmd);
            return;
        }
        for( i = 0; i < num_bytes_r; i++ )
            ptr += sprintf(ptr, "%02x ", buffer[i]);
        ptr[-1] = '\0';
    }
}
#endif

#if AAPL_ALLOW_MDIO || AAPL_ALLOW_GPIO_MDIO
static void mdio_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    Avago_mdio_cmd_t mdio_cmd;
    uint port_addr, dev_addr, reg_addr = 0, data = 0;
    const char *cp2;
    char cmd_name[5] = "wait";

    if( !aapl->mdio_fn )
    {
        sprintf(result, "ERROR: no mdio implementation is available.");
        return;
    }

    SKIPSPACE(cp);
    if( *cp == '\0' )
    {
        sprintf(result, "ERROR: Invalid \"mdio\" command ignored. Must be: "
        "\"mdio a <port_addr> <dev_addr> <reg_addr> [# <comments>]\", "
        "\"mdio w <port_addr> <dev_addr> <reg_data> [# <comments>]\", "
        "\"mdio r <port_addr> <dev_addr> [# <comments>]\", "
        "\"mdio wait <port_addr> <dev_addr> [# <comments>]\", "
        "Got: \"%s\".", cmd);
        return;
    }

    strncpy(cmd_name,cp,4);
    cmd_name[4] = '\0';
    if(      (cp[0] == 'a' || cp[0] == 'A') && isspace(cp[1]) ) mdio_cmd = AVAGO_MDIO_ADDR;
    else if( (cp[0] == 'w' || cp[0] == 'W') && isspace(cp[1]) ) mdio_cmd = AVAGO_MDIO_WRITE;
    else if( (cp[0] == 'r' || cp[0] == 'R') && isspace(cp[1]) ) mdio_cmd = AVAGO_MDIO_READ;
    else if( aapl_strncasecmp(cp,"wait ",5) == 0 ) { cp += 4;   mdio_cmd = AVAGO_MDIO_WAIT; }
    else
    {
        sprintf(result, "ERROR: Invalid \"mdio\" command ignored. Arg 1 must be \"a\", \"w\", \"r\" or \"wait\".  Got \"%s\".", cmd);
        return;
    }
    cp++; SKIPSPACE(cp);
    if( mdio_cmd != AVAGO_MDIO_WAIT )
        cmd_name[1] = '\0';

    port_addr = aapl_strtoul(cp, (char **)&cp2, 0);
    if( cp2 == cp || port_addr >= 256 )
    {
        sprintf(result, "ERROR: Invalid \"mdio %s\" command ignored. Arg 2 must be a numerical port value < 256. Got: \"%s\".", cmd_name, cmd);
        return;
    }
    cp = cp2; SKIPSPACE(cp);
    dev_addr = aapl_strtoul(cp, (char **)&cp2, 0);
    if( cp2 == cp || dev_addr >= 256 )
    {
        sprintf(result, "ERROR: Invalid \"mdio %s\" command ignored. Arg 3 must be a numerical device value < 256. Got: \"%s\".", cmd_name, cmd);
        return;
    }
    cp = cp2; SKIPSPACE(cp);
    if( mdio_cmd == AVAGO_MDIO_ADDR )
    {
        reg_addr = aapl_strtoul(cp, (char **)&cp2, 0);
        if( cp2 == cp )
        {
            sprintf(result, "ERROR: Invalid \"mdio a\" command ignored. Arg 4 must be a numerical address value. Got: \"%s\".", cmd);
            return;
        }
    }
    else if( mdio_cmd == AVAGO_MDIO_WRITE )
    {
        data = aapl_strtoul(cp, (char **)&cp2, 0);
        if( cp2 == cp || data > 0xffff )
        {
            sprintf(result, "ERROR: Invalid \"mdio w\" command ignored. Arg 4 must be a numerical data value < 0xffff. Got: \"%s\".", cmd);
            return;
        }
    }

    sprintf(result, "%x", aapl->mdio_fn(aapl, mdio_cmd, port_addr, dev_addr, reg_addr, data));
}
#endif

#if AAPL_ENABLE_BLACKHAWK
static void pmi_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result, int * chip_nump, int * ring_nump)
{
    err_code_t err_code;
    uint sbus_addr = 0, mask = 0xffff, reg_addr = 0, data = 0;
    int command;
    const char *cp2;
    char cmd_name[] = "reset";

    if( !aapl->pmi_fn )
    {
        sprintf(result, "ERROR: no pmi implementation is available.");
        return;
    }

    SKIPSPACE(cp);
    if( *cp == '\0' )
    {
        sprintf(result, "ERROR: Invalid \"pmi\" command ignored. Must be: "
        "\"pmi r <sbus_addr> <reg_addr> [# <comments>]\", "
        "\"pmi w <sbus_addr> <reg_addr> <write_data> [# <comments>]\", "
        "\"pmi m <sbus_addr> <reg_addr> <write_data> <write_mask> [# <comments>]\", "
        "\"pmi reset [# <comments>]\", "
        "Got: \"%s\".", cmd);
        return;
    }

    strncpy(cmd_name,cp,5);
    cmd_name[5] = '\0';
    if(      (cp[0] == 'r' || cp[0] == 'R') && isspace(cp[1]) ) command = 2;
    else if( (cp[0] == 'w' || cp[0] == 'W') && isspace(cp[1]) ) command = 1;
    else if( (cp[0] == 'm' || cp[0] == 'M') && isspace(cp[1]) ) command = 3;
    else if( aapl_strncasecmp(cp,"reset ",6) == 0 ) { cp += 6;   command = 0; }
    else
    {
        sprintf(result, "ERROR: Invalid \"pmi\" command ignored. Arg 1 must be \"r\", \"w\", \"m\" or \"reset\".  Got \"%s\".", cmd);
        return;
    }
    cp++; SKIPSPACE(cp);
    if( cmd != 0 )
        cmd_name[1] = '\0';

    if( !aapl_str_to_addr(cp, (char **)&cp2, &sbus_addr) )
        sbus_addr = aapl_strtoul(cp, (char **)&cp2, 16);
    if( cp2 == cp )
    {
        sprintf(result, "ERROR: Invalid \"pmi %s\" command ignored. Arg 2 must be an sbus address value. Got: \"%s\".", cmd_name, cmd);
        return;
    }
    *chip_nump = (sbus_addr & 0xf000) >> 12;
    *ring_nump = (sbus_addr & 0x0f00) >> 8;
    cp = cp2; SKIPSPACE(cp);

    reg_addr = aapl_strtoul(cp, (char **)&cp2, 16);
    if( cp2 == cp || reg_addr > 0xffff )
    {
        sprintf(result, "ERROR: Invalid \"pmi %s\" command ignored. Arg 3 must be a hex register address < 0xffff. Got: \"%s\".", cmd_name, cmd);
        return;
    }
    cp = cp2; SKIPSPACE(cp);

    if( command == 1 || command == 3 )
    {
        data = aapl_strtoul(cp, (char **)&cp2, 16);
        if( cp2 == cp )
        {
            sprintf(result, "ERROR: Invalid \"pmi %s\" command ignored. Arg 4 must be a hex data value. Got: \"%s\".", cmd_name, cmd);
            return;
        }
        cp = cp2; SKIPSPACE(cp);
        if( command == 3 )
        {
            mask = aapl_strtoul(cp, (char **)&cp2, 16);
            if( cp2 == cp )
            {
                sprintf(result, "ERROR: Invalid \"pmi m\" command ignored. Arg 5 must be a hex mask value. Got: \"%s\".", cmd);
                return;
            }
        }
    }

    err_code = aapl->pmi_fn(aapl, sbus_addr, command, reg_addr, &data, mask);
    if( ERR_CODE_NONE == err_code )
        sprintf(result, "%x", data);
    else
    {
        srds_access_t sa_space, *sa__ = &sa_space;
        if( 0 == avago_addr_to_srds_access_struct_BE(aapl, sa__, sbus_addr) )
            sprintf(result, "ERROR: %s", bcm_serdes_INTERNAL_e2s_err_code(sa__,err_code));
    }
}
#endif


static void jtag_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result, int * chip_nump)
{
    char *tdo = 0, *cp2 = 0;
    int opcode, bits;

    (void)chip_nump;

    AAPL_WAIT_FOR_SBUS_THREAD(aapl);
    aapl_clear_recv_data_valid(aapl);

    SKIPSPACE(cp);

    bits = aapl_strtol(cp, &cp2, 10);
    if (!bits || ISNUL(cp2)) goto BadJtagCmd;
    opcode = aapl_strtol(cp2, &cp2, 16);
    if (ISNUL(cp2)) goto BadJtagCmd;

    {
        char *buf;
        int length;
        SKIPSPACE(cp2);
        length = strcspn(cp2," ");
        buf = (char *) aapl_malloc(aapl, bits+1, __func__);
        if( buf )
        {
#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
            if (strstr(cp2, "read"))
            {
                memset(buf, '0', bits+1);
                tdo = avago_jtag_rd(aapl, opcode, bits);
            }
            else
#endif
            {
                memset(buf+length, '0', bits+1-length);
                memcpy(buf, cp2, length);
                tdo = avago_jtag(aapl, opcode, bits, buf);
            }
            if (tdo) snprintf(result, AACS_SERVER_BUFFERS, "%s", tdo);
            else result[0] = 0;
            aapl_free(aapl, buf, __func__);
        }
    }
    return;

    BadJtagCmd:
    cmd_error(aapl, result, "Unsupported \"jtag\" command ignored. Format should be \"jtag <bits> <opcode> <ascii binary data>\". Got: \"%s\".", cmd);
}

#if AAPL_ENABLE_PS2 && AAPL_ENABLE_FILE_IO && AAPL_ENABLE_AACS_SERVER
static void ps2_update(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    FILE *file = 0;
    long size;
    const char * cp1 = cp;
    char *data, *filename;
    int bytes_recv = 0, data_socket = -1, data_socket_fd = -1, rc = 0;
    struct sockaddr_in sai;
    struct sockaddr_in file_IPaddr;
    socklen_t addr_len = sizeof(file_IPaddr);
    int tcp_port = 9001;
    (void) cmd;

    SKIPSPACE(cp);
    size = strtol(cp1, &filename, 10);
    SKIPSPACE(filename);

    data = (char *) aapl_malloc(aapl, sizeof(char) * size, __func__);

    if((data_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot create file socket: %s.\n", aapl_tcp_strerr());
        goto cleanup_and_exit;
    }

    sai.sin_family      = AF_INET;
    sai.sin_addr.s_addr = INADDR_ANY;
    sai.sin_port        = htons(tcp_port);

    rc = setsockopt(data_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &rc, sizeof(rc));
    if ((rc = bind(data_socket, (struct sockaddr *) &sai, sizeof (sai))) < 0)
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot bind() AF_INET socket for clients on port %d: %s.\n", tcp_port, aapl_tcp_strerr());
        goto cleanup_and_exit;
    }

    if ((rc = listen(data_socket, /* backlog = */ 1)) < 0)
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot listen() for command client on port %d: %s.\n", tcp_port, aapl_tcp_strerr());
        goto cleanup_and_exit;
    }

    if ((data_socket_fd = accept(data_socket, (struct sockaddr *) &file_IPaddr, &addr_len)) < 0)
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot accept() for command client on port %d: %s.\n", tcp_port, aapl_tcp_strerr());
        goto cleanup_and_exit;
    }

    while (bytes_recv < size)
    {
        int last_bytes_recv = recv(data_socket_fd, data+bytes_recv, size-bytes_recv, 0);

        if (last_bytes_recv > 0) bytes_recv += last_bytes_recv;
        aapl_log_printf(aapl, AVAGO_DEBUG9, __func__, __LINE__, "Size: %ld Bytes recv: %d Last bytes recv: %d\n", size, bytes_recv, last_bytes_recv);
        if (last_bytes_recv < 0)
        {
            aapl_fail(aapl, __func__, __LINE__, "recv() returned %s (%s (%d))\n", last_bytes_recv, strerror(errno), errno);
            break;
        }
    }

    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Size: %ld Bytes recv: %d\n", size, bytes_recv);
    aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Saving %1d bytes of data to file %s.\n", size, filename);
    file = fopen(filename, "w");
    if (file && data) fwrite(data, size, 1, file);
    if (data) aapl_free(aapl, data, __func__);

cleanup_and_exit:
    if (file) fclose(file);
    if (data_socket_fd) close(data_socket_fd);
    if (data_socket) close(data_socket);

    if (size == bytes_recv)
    {
        #define PS2_UPDATE_TEMP "/tmp/aapl_update"
        #define PS2_UPDATE_DEST "/home/pi"
        #define PS2_UPDATE_LOG "/home/pi/log/update_log.txt"
        struct stat sb;
        if (stat(PS2_UPDATE_DEST, &sb) == 0 && S_ISDIR(sb.st_mode))
        {
            char buffer[128];
#if AAPL_ENABLE_CONSOLE
            if (enable_console_mode) console_teardown(aapl);
#endif

            system("rm -f " PS2_UPDATE_LOG);
            system("rm -rf " PS2_UPDATE_TEMP " > " PS2_UPDATE_LOG);
            system("mkdir -p " PS2_UPDATE_TEMP " >> " PS2_UPDATE_LOG);
            snprintf(buffer, 128, "tar -xvzf %s --directory " PS2_UPDATE_TEMP " >> " PS2_UPDATE_LOG, filename);
            system(buffer);

            if (stat(PS2_UPDATE_TEMP "/bin", &sb) == 0 && S_ISDIR(sb.st_mode))
            {
                system("rm -rf " PS2_UPDATE_DEST "/bin_old >> " PS2_UPDATE_LOG);
                system("mv " PS2_UPDATE_DEST "/bin " PS2_UPDATE_DEST "/bin_old >> " PS2_UPDATE_LOG);
                system("mkdir -p " PS2_UPDATE_DEST "/bin >> " PS2_UPDATE_LOG);
                system("cp -f -r " PS2_UPDATE_TEMP "/bin " PS2_UPDATE_DEST " >> " PS2_UPDATE_LOG);
                system("rm -rf " PS2_UPDATE_TEMP "/bin  >> " PS2_UPDATE_LOG);
                system("chgrp -R pi " PS2_UPDATE_DEST "/bin >> " PS2_UPDATE_LOG);
                system("chown -R pi " PS2_UPDATE_DEST "/bin >> " PS2_UPDATE_LOG);
                system("rsync -av --prune-empty-dirs --include='updated' --include='*/' --exclude='*' " PS2_UPDATE_DEST "/bin_old/ " PS2_UPDATE_DEST "/bin/ >> " PS2_UPDATE_LOG);
                system("cp -f -r " PS2_UPDATE_DEST "/bin_old/.git/config " PS2_UPDATE_DEST "/bin/.git/ >> " PS2_UPDATE_LOG);

                system(PS2_UPDATE_DEST "/bin/update_ps2 skip_pull &");

                sprintf(result, "Update to %s complete.\n", PS2_UPDATE_DEST);
                return;
            }
            else
            {
                system("rm -rf " PS2_UPDATE_TEMP "/bin  >> " PS2_UPDATE_LOG);
                sprintf(result, "Update file %s did not have correct format. Update aborted.\n", filename);
            }
        }
        else sprintf(result, "Destination location %s does not exist.\n", PS2_UPDATE_DEST);
    }
    else sprintf(result, "ERROR: Bytes received did not match bytes sent.\n");

#if AAPL_ENABLE_CONSOLE
    if (enable_console_mode) console_setup(aapl);
#endif
}
#endif

#if AAPL_ENABLE_MAIN
static void cli_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    char *argv[100];
    char argv_buf[100][100];
    int argc = 0, length = 0;
    (void) cmd;

    SKIPSPACE(cp);

    strcpy(argv_buf[argc], "aapl");
    argv[argc] = argv_buf[argc];
    argc++;

    while (1)
    {
        if (!cp[0]) length = 0;
        else if (strchr(cp, ' ')) length = strchr(cp, ' ') - cp;
        else length = strlen(cp);

        if (length < 0) length = strlen(cp);
        if (!length) break;

        strncpy(argv_buf[argc], cp, length);
        argv_buf[argc][length] = 0;
        argv[argc] = argv_buf[argc];
        argc++;
        cp += length;
        SKIPSPACE(cp);
    }

    aapl->enable_debug_logging = 1;
    aapl_log_clear(aapl);
    aapl_close_connection(aapl);
    aapl_main(argc, argv, aapl);
    snprintf(result, AACS_SERVER_BUFFERS, "%s\n", aapl->log);
    return;
}
#endif

#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
static void jtag_cycle_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    char *cp2;
    uint cycles, tms, tdi, trst_l;

    cycles = aapl_strtol(cp, &cp2, 10);
    if (ISNUL(cp2)) goto jtag_cycle_error;
    tms = aapl_strtol(cp2, &cp2, 10);
    if (ISNUL(cp2)) goto jtag_cycle_error;
    tdi = aapl_strtol(cp2, &cp2, 10);
    if (ISNUL(cp2)) goto jtag_cycle_error;
    trst_l = aapl_strtol(cp2, &cp2, 10);

    result[0] = 0;
    result[1] = 0;
    if (!check_term(aapl, cmd, cp2, result))
    {
        jtag_cycle_error:
        cmd_error(aapl, result, "Invalid command ignored. Command must be of format: jtag_cycle <cycles> <tms> <tdi> <trst_l>. Got: \"%s\".", cmd);
    }
    else
    {
        aapl->prev_opcode = 0;
        result[0] = avago_bit_banged_jtag(aapl, cycles, tms, tdi, trst_l, 1);
    }
}

static void jtag_scan_bad_cmd(Aapl_t *aapl, char * result, const char *cmd)
{
    cmd_error(aapl, result, "Invalid \"jtag_cycle\" command ignored. Must be: \"jtag_cycle <cycles> <tms> <tdi> <trst_l> [# <comments...>]\". Got: \"%s\".", cmd);
}

static void jtag_scan_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{

    const char * cp3;
    char * cp2;
    int    num_bits;
    int    len;
    const char * data;
    int    TMS_cycles;
    int    finish;
    #define BUFSIZE 4096
    char   aux_buffer[BUFSIZE];

    const char * cp1 = cp;

    SKIPSPACE(cp1);
    if (ISNUL(cp1) || CKHELP(cp1))
    {
        cmd_error(aapl, result, "Invalid \"jtag_scan\" command ignored. Must be: \"jtag_scan <num_data_bits> <data> <tms_cycles = 0, 1, \"DR\", 2, or \"IR\"> <finish = \"stop\", \"exit\", or \"RTI\"> [# <comments...>]\". Got: \"%s\".", cmd);
        return;
    }


    if ((num_bits = (int) strtol(cp1, &cp2, 10)) <= 0)
    {
        cmd_error(aapl, result, "Invalid \"jtag_scan\" command ignored. Arg 1 must be (positive) number of data bits to scan. Got: \"%s\".", cmd);
        return;
    }
    if (num_bits > BUFSIZE)
    {
        cmd_error(aapl, result, "Invalid \"jtag_scan\" command ignored. Arg 2 cannot be more than %d. Got %d.", BUFSIZE, num_bits);
        return;
    }
    CKSKIPSPACE(aapl, result, cmd, cp1, cp2, jtag_scan_bad_cmd);


    cp3 = cp1; while (CURRIS(cp3, '0', '1')) ++cp3;

    if ((! ISTERM(cp3)) || (cp1 == cp3) || ((len = cp3 - cp1) > num_bits))
    {
        cmd_error(aapl, result, "Invalid \"jtag_scan\" command ignored. Arg 2 must be ASCII-encoded binary <data> no longer than <num_data_bits> (%d). Got (%ld): \"%.32s%s\"", num_bits, cp3 - cp1, cp, (strlen(cp) <= 32) ? "" : "...");
        return;
    }
    data = cp1;
    CKSKIPSPACE(aapl, result, cmd, cp1, cp3, jtag_scan_bad_cmd);


    if      EQI(cp1, "dr", 2) {TMS_cycles = 1; cp3 = cp1 + 2;}
    else if EQI(cp1, "ir", 2) {TMS_cycles = 2; cp3 = cp1 + 2;}
    else                      {TMS_cycles = (int) strtol(cp1, &cp2, 10); cp3 = cp1 + 1;}
    if ((TMS_cycles < 0) || (cp1 == cp2) || (TMS_cycles > 2))
    {
        cmd_error(aapl, result, "Invalid \"jtag_scan\" command ignored. Arg 3 must be number of cycles to clock with TMS high (0, 1, \"DR\", 2, or \"IR\"). Got: \"%s\".", cmd);
        return;
    }
    CKSKIPSPACE(aapl, result, cmd, cp1, cp3, jtag_scan_bad_cmd);


    if      (EQI(cp1, "stop", 4) && ISTERM(cp1 + 4)) {finish = 0; cp3 = cp1 + 4;}
    else if (EQI(cp1, "exit", 4) && ISTERM(cp1 + 4)) {finish = 1; cp3 = cp1 + 4;}
    else if (EQI(cp1, "rti",  3) && ISTERM(cp1 + 3)) {finish = 2; cp3 = cp1 + 3;}
    else
    {
        cmd_error(aapl, result, "Invalid \"jtag_scan\" command ignored. Arg 4 must be how to finish (\"stop\", \"exit\", or \"RTI\"). Got: \"%s\".", cmd);
        return;
    }
    CKSKIPSPACE(aapl, result, cmd, cp1, cp2, jtag_scan_bad_cmd);


    if ((TMS_cycles == 2) && ((num_bits != JTAG_INST_BITS) || (finish != 2)))
    {
        cmd_error(aapl, result, "Invalid \"jtag_scan\" command ignored. An arg 3 of 2 or \"IR\" requires <num_data_bits> = %d and <finish> = \"RTI\". Got: \"%s\".", JTAG_INST_BITS, cmd);
        return;
    }



    if (num_bits > len)
    {
        int fill = num_bits - len;
        memset(aux_buffer, '0', fill);
        memcpy(aux_buffer + fill, data, len);
        data = aux_buffer;
    }

    if (finish == 2)      avago_jtag_scan(aapl, num_bits, TMS_cycles, data, result);
    else if (finish == 0) avago_jtag_scan_options(aapl, num_bits, TMS_cycles, data, result, AVAGO_SHIFT_DR);
    else if (finish == 1) avago_jtag_scan_options(aapl, num_bits, TMS_cycles, data, result, AVAGO_JTAG_UPDATE_DR);


}
#endif


static void chips_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result))
        sprintf(result, "%d", aapl->chips);
}


static void chip_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result, int * chip_nump)
{
    if (check_term(aapl, cmd, cp, result))
    {
#if AAPL_ENABLE_AACS_SERVER
        if (aapl_sbus_rings_override == FALSE)
        {
            int chip;
            for (chip = 0; chip < AAPL_MAX_CHIPS; chip++)
                aapl->sbus_rings[chip] = 0;
        }
        if (aapl_num_chips_override == FALSE) aapl->chips = 0;
#endif
        aapl_get_ip_info(aapl, 0);
        jtag_command(       aapl, cmd, "32 02b6 0", aapl->aacs_server_buffer, chip_nump);
    }

}


static void chipnum_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result, int * chip_nump)
{
    const char *cp2;
    uint num;

    SKIPSPACE(cp);
    if (ISEOL(cp)) {sprintf(result, "%d", *chip_nump); return;}

    cp2 = cp;
    num = aapl_num_from_hex(cp, &cp2, 1, 1);

    if (cp2 == cp)
        cmd_error(aapl, result, "Invalid \"chipnum\" command ignored: Parameter must be a single hex digit. Got: \"%s\".", cmd);
    else if (num >= aapl->chips)
        cmd_error(aapl, result, "Invalid \"chipnum\" command ignored: Max chip number is 0x%x. Got: \"%s\".", aapl->chips - 1, cmd);
    else {*chip_nump = num; result[0] = *cp; result[1] = '\0';}
    aapl_set_chip(aapl, *chip_nump);
}

static void set_uint(Aapl_t * aapl, const char * cmd, const char * cp, uint * ptr, char * result)
{
    char *cp2;
    uint num;

    SKIPSPACE(cp);
    if (ISEOL(cp)) {snprintf(result, AACS_SERVER_BUFFERS, "%d", *ptr); return;}

    num = aapl_strtol(cp, &cp2, 0);

    if (!cp2)
        cmd_error(aapl, result, "Invalid command ignored: Parameter must be a single integer. Got: \"%s\".", cmd);
    else
    {
        *ptr = num;
        snprintf(result, AACS_SERVER_BUFFERS, "%d", *ptr);
    }
}

static void sbus_reset_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    uint addr;

    SKIPSPACE(cp);
    if (ISEOL(cp)) goto sbus_reset_error;

    if (!aapl_str_to_addr(cp, 0, &addr)) goto sbus_reset_error;
    cp = strchr(cp, ' ');

    if (!check_term(aapl, cmd, cp, result))
    {
        sbus_reset_error:
        cmd_error(aapl, result, "Invalid \"sbus_reset\" command ignored: Parameter must be a valid address. Got: \"%s\".", cmd);
    }
    else
    {
        avago_sbus_reset(aapl, addr, 1);
        snprintf(result, AACS_SERVER_BUFFERS, "Hard SBus reset executed to address %s.", aapl_addr_to_str(addr));
    }
}

#if AAPL_ALLOW_AACS

static void my_send_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
      (void)cmd;
      SKIPSPACE(cp);


      if (ISNUL(cp)) {result[0] = '\0'; return;}

      strncpy(result, avago_aacs_send_command(aapl, cp), AACS_SERVER_BUFFERS);
      result[AACS_SERVER_BUFFERS - 1] = '\0';
}

static void set_sbus_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    int bit = -1, val = -1, x;
    char *cp2 = 0;
    char *pointer = result;
    (void) cmd;

    if (aapl_is_aacs_communication_method(aapl))
    {
        cp = cmd;
        my_send_command(aapl, cmd, cp, aapl->aacs_server_buffer);
        return;
    }

    SKIPSPACE(cp);
    if (!ISEOL(cp))
    {
        bit = aapl_strtol(cp, &cp2, 10);
        cp = cp2;
    }
    SKIPSPACE(cp);
    if (!ISEOL(cp))
    {
        val = aapl_strtol(cp, &cp2, 10);
        cp = cp2;
    }

    aapl_clear_recv_data_valid(aapl);


    if (bit != -1) bit = 125 - bit;

    if (bit != -1 && val == -1)
    {
        snprintf(pointer++, AACS_SERVER_BUFFERS, "%s", aapl->sbus_reg[bit] == 0x30 ? "0" : "1");
        return;
    }
    else if (bit != -1) aapl->sbus_reg[bit] = val + 0x30;

    for (x=0; x<=125; x++)
    {
        if (x==125-31 || x==125-36 || x==125-43 || x==125-48 || x==125-80 || x==125-88 || x==125-96 || x==125-104 || x==125-108)
            snprintf(pointer++, AACS_SERVER_BUFFERS, " ");
        snprintf(pointer++, AACS_SERVER_BUFFERS, "%s", aapl->sbus_reg[x] == 0x30 ? "0" : "1");
    }
}


static void reset_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    uint chip;
    (void) cmd;

    SKIPSPACE(cp);
    aapl_str_to_addr(cp, 0, &chip);

    avago_jtag_reset(aapl, chip);
    snprintf(result, AACS_SERVER_BUFFERS, "JTAG reset executed to chip %u.", chip);
}
#endif

#if AAPL_ENABLE_ATE_VEC
static void ate_vec_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    if( !aapl->ate_vec_fn )
        cmd_error(aapl, result, "An ate_vec function is not registered.");
    else
    {
        SKIPSPACE(cp);
        if( ISEOL(cp) )
            cmd_error(aapl, result, "Invalid \"ate_vec\" command ignored: Parameter required. Got: \"%s\".", cmd);
        else
        {
            int ret = aapl->ate_vec_fn(aapl, cp);
            snprintf(result, AACS_SERVER_BUFFERS, ret == 0 ? "PASS" : "FAIL");
        }
    }
}
#endif
static void sleep_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    char *cp2 = 0;
    int num;

    SKIPSPACE(cp);

    num = aapl_strtol(cp, &cp2, 10);
    num *= 1000;
    if( *cp2 == '.' )
    {
        char *cp3 = ++cp2;
        int num2 = aapl_strtol(cp3, &cp2, 10);
        int len = cp2-cp3;
        for( ; len > 3; len-- ) num2 = (num2 + 5) / 10;
        for( ; len < 3; len++ ) num2 *= 10;
        num += num2;
    }
    if( *cp2 != '\0' || num < 0 || *cp == '-' )
        cmd_error(aapl, result, "Invalid \"sleep\" command ignored: Parameter must be a (non-negative) number of seconds (int or decimal). Got: \"%s\".", cmd);
    else
    {
#if AAPL_ALLOW_AACS
        avago_aacs_flush(aapl);
#endif
        ms_sleep(num);
        snprintf(result, AACS_SERVER_BUFFERS, "Slept %d.%03d seconds.",num/1000,num%1000);
    }
}

static void sbus_mode_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    const char *cp2;
    Aapl_comm_method_t comm_method;
    (void)cmd;

    SKIPSPACE(cp);
    if (ISEOL(cp)) {snprintf(result, AACS_SERVER_BUFFERS, "%s", aapl_comm_method_to_str(aapl->communication_method)); return;}

    cp2 = cp;

    if( aapl_str_to_comm_method(cp2,&comm_method) )
    {
        aapl->communication_method = comm_method;
        snprintf(result, AACS_SERVER_BUFFERS, "sbus_mode set to %s.", cp2);
    }
    else snprintf(result, AACS_SERVER_BUFFERS, "sbus_mode option must be {AACS}_{I2C|MDIO|SBUS|JTAG|BB_JTAG}|SYSTEM_{I2C|MDIO}|GPIO_MDIO|OFFLINE\n");
}

#if AAPL_ENABLE_DIAG
static void diag_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    char *cp2;
    uint addr;

    SKIPSPACE(cp);
    if (ISEOL(cp)) goto diag_cmd_error;

    if (EQCN(cp, "size", 4))
    {
        cp += 4;
        if (!check_term(aapl, cmd, cp, result)) goto diag_cmd_error;
        snprintf(result, AACS_SERVER_BUFFERS, "%u", (uint)sizeof(Avago_diag_config_t));
        return;
    }

    aapl_str_to_addr(cp, 0, &addr);
    cp = strchr(cp, ' ');

    if (!check_term(aapl, cmd, cp, result))
    {
        diag_cmd_error:
        cmd_error(aapl, result, "Invalid \"diag\" command ignored. Should be: diag [size | <addr>]. Got: \"%s\".", cmd);
        return;
    }

    aapl_log_clear(aapl);

    {
        Avago_diag_config_t *config  = avago_diag_config_construct(aapl);
        if( config )
        {
            size_t index;
            for (index = 0; index < (sizeof(Avago_diag_config_t) - 2*sizeof(int))/sizeof(BOOL); index++)
            {
                int num;
                if (!cp) break;
                num = aapl_strtoul(cp, &cp2, 0);
                if (cp != cp2) *((char *)config + index) = num;
                cp = cp2;
            }

            avago_diag(aapl, addr, config);
            avago_diag_config_destruct(aapl, config);
        }
    }

    aapl_str_rep(aapl->log, 0xa, 0x2);
    aapl->aacs_server_buffer = (char *) aapl_realloc(aapl, aapl->aacs_server_buffer, strlen(aapl->log) + AACS_SERVER_BUFFERS, __func__);
    if( aapl->aacs_server_buffer )
        snprintf(aapl->aacs_server_buffer, strlen(aapl->log) + AACS_SERVER_BUFFERS, "%c%c%s", 0x1, 0x1, aapl->log);
}
#endif

static void spico_int_command(Aapl_t * aapl, const char * cmd, const char * cp, char * result)
{
    char *cp2;
    uint interrupt, param, addr;

    SKIPSPACE(cp);
    if (ISEOL(cp)) goto spico_int_error;

    aapl_str_to_addr(cp, 0, &addr);
    cp = strchr(cp, ' ');

    if (!cp) goto spico_int_error;
    SKIPSPACE(cp);
    if (ISEOL(cp)) goto spico_int_error;
    interrupt = aapl_strtoul(cp, &cp2, 0);

    cp = cp2;
    SKIPSPACE(cp);
    if (ISEOL(cp)) goto spico_int_error;
    param  = aapl_strtoul(cp, &cp2, 0);

    cp = cp2;
    if (!check_term(aapl, cmd, cp, result))
    {
        spico_int_error:
        cmd_error(aapl, result, "Invalid \"spico_int\" command ignored. Should be: spico_int <addr> <int> <param>. Got: \"%s\".", cmd);
        return;
    }
    snprintf(result, AACS_SERVER_BUFFERS, "%x", avago_spico_int(aapl, addr, interrupt, param));
}


static void version_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
#if AAPL_ENABLE_PS2 && AAPL_ALLOW_SYSTEM_I2C && AAPL_ENABLE_FLOAT_USAGE && defined I2C_SMBUS_READ
    if (check_term(aapl, cmd, cp, result))
        snprintf(result, AACS_SERVER_BUFFERS, "AAPL AACS PS2 server %s, rev %d serial# %d (%3.2fV %3.2fA)", build_info, avago_ps2_get_rev(aapl), avago_ps2_get_serial(aapl), avago_ps2_read_input_voltage(aapl), avago_ps2_read_input_current(aapl));
#else
    if (check_term(aapl, cmd, cp, result))
        snprintf(result, AACS_SERVER_BUFFERS, "AAPL AACS server %s", AAPL_VERSION);
#endif
#if AAPL_ENABLE_AACS_SERVER
    if (strstr(cp, "from user: "))
    {
        cp += 14;
        strncpy(aapl_client_username, cp, 127);
    }
#endif
}


#if AAPL_ENABLE_PS2 && AAPL_ENABLE_FILE_IO
static void run_system_cmd(const char *cmd, char *data, uint data_len)
{
    FILE *fp = popen(cmd, "r");

    if( fp != NULL )
    {
        char *data_ptr = data;
        uint remain = data_len;

        while( remain > 1 && fgets(data_ptr, remain = (data_len - (data_ptr-data)), fp) != NULL )
            data_ptr = data + strlen(data);
        if( *--data_ptr == '\n' )
            *data_ptr = '\0';

        pclose(fp);
    }
}
#endif


static void status_command(Aapl_t *aapl, const char * cmd, const char * cp, char ** result)
{
    if (check_term(aapl, cmd, cp, *result))
    {
        char *result_end = *result;
        int size = 0;
        time_t time_diff;

#if AAPL_ENABLE_PS2 && AAPL_ENABLE_FILE_IO
        char data[10240];
        data[0] = 0;

        run_system_cmd("/bin/date", data, AAPL_ARRAY_LENGTH(data));
        aapl_buf_add(aapl, result, &result_end, &size, "Updated:              %s;", data);
        run_system_cmd("/usr/bin/uptime", data, AAPL_ARRAY_LENGTH(data));
        aapl_buf_add(aapl, result, &result_end, &size, "Uptime:               %s;", data);
#endif

        if (!last_client_time) last_client_time = time(0);
        time_diff = time(0) - last_client_time;

#if AAPL_ENABLE_AACS_SERVER
        aapl_buf_add(aapl, result, &result_end, &size, "Locked by client:     %s;", inet_ntoa(client_IPaddr_lock[1].sin_addr));
        aapl_buf_add(aapl, result, &result_end, &size, "Locked by client:     %s;", inet_ntoa(client_IPaddr_lock[0].sin_addr));
        aapl_buf_add(aapl, result, &result_end, &size, "Cmd client IP addr:   %s;", inet_ntoa(client_IPaddr.sin_addr));
        aapl_buf_add(aapl, result, &result_end, &size, "Last user:            %s;", aapl_client_username);
#endif

#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
        aapl_buf_add(aapl, result, &result_end, &size, "Current chip:         %d (of 0..%d);", aapl->curr_chip, aapl->chips-1);
#endif
        if (aapl->chips)
        {
            int chip;
            for (chip = 0; chip <= (int)aapl->chips - 1; chip++)
            {
                aapl_buf_add(aapl, result, &result_end, &size, "Chip %d:               0x%08x ", chip, aapl->jtag_idcode[chip]);
                if (aapl->chip_name[chip]) aapl_buf_add(aapl, result, &result_end, &size, "(%s) ", aapl->chip_name[chip]);
                aapl_buf_add(aapl, result, &result_end, &size, "0x%x rings;", aapl->sbus_rings[chip]);
            }
        }
        aapl_buf_add(aapl, result, &result_end, &size, "TAP generation:       %d;", avago_get_tap_gen(aapl));
        aapl_buf_add(aapl, result, &result_end, &size, "Communication method: %s;", aapl_comm_method_to_str(aapl->communication_method));
        aapl_buf_add(aapl, result, &result_end, &size, "Debug level:          %d;", aapl->debug);
        aapl_buf_add(aapl, result, &result_end, &size, "Verbose level:        %d;", aapl->verbose);
        aapl_buf_add(aapl, result, &result_end, &size, "Suppress errors:      %d;", aapl->suppress_errors);
        aapl_buf_add(aapl, result, &result_end, &size, "Upgrade warnings:     %d;", aapl->upgrade_warnings);
        aapl_buf_add(aapl, result, &result_end, &size, "Serdes int timeout    %d;", aapl->serdes_int_timeout);
        aapl_buf_add(aapl, result, &result_end, &size, "TCK delay:            %d;", aapl->tck_delay);
        aapl_buf_add(aapl, result, &result_end, &size, "Error count:          %d;", aapl->errors);
        aapl_buf_add(aapl, result, &result_end, &size, "Warning count:        %d;", aapl->warnings);
        aapl_buf_add(aapl, result, &result_end, &size, "Max commands buffered:%d;", aapl->max_cmds_buffered);
        aapl_buf_add(aapl, result, &result_end, &size, "Capabilities:         %d;", aapl->capabilities);
        aapl_buf_add(aapl, result, &result_end, &size, "Client cmds received: %s (sbus %d) (sec ago: %d) (time: %d);", aapl_bigint_to_str2(aapl->commands,TRUE), aapl->sbus_commands, time_diff, last_client_time);
        aapl_buf_add(aapl, result, &result_end, &size, "AACS Server buf size: %d;", AACS_SERVER_BUFFERS);
        aapl_buf_add(aapl, result, &result_end, &size, "Data_char size:       %d;", aapl->data_char_size);
        aapl_buf_add(aapl, result, &result_end, &size, "Log size:             %d;", aapl->log_size);
        if (aapl->aacs_server) aapl_buf_add(aapl, result, &result_end, &size,
                                                       "Remote AACS server:   %s:%d;", aapl->aacs_server, aapl->tcp_port);
#if AAPL_ENABLE_PS2 && AAPL_ENABLE_FILE_IO && AAPL_ENABLE_AACS_SERVER
        {
            struct stat sb;
            if (stat("/home/pi/", &sb) == 0 && S_ISDIR(sb.st_mode))
            {
                run_system_cmd("/bin/cat /home/pi/[hp]s2-*.txt | grep -iv -e \"[tr]x packets\" -e \"[tr]x errors\"", data, AAPL_ARRAY_LENGTH(data));
                aapl_str_rep(data, 0xa, ';');
                aapl_buf_add(aapl, result, &result_end, &size, "%s;", data);
            }
        }
#endif
    }
}

static void null_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    (void)aapl;
    (void)cmd;
    (void)cp;
    if (cp[0] == '#') result[0] = 0;
    else              snprintf(result, AACS_SERVER_BUFFERS, "Unimplemented command.");
}



static void help_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result))
    {
        strcpy(result, HELP_STR);
#if AAPL_ENABLE_PS2_KEYSTONE
        if( avago_is_keystone_dut(aapl) ) strcat(result, KEYSTONE_HELP);
#endif
        strcat(result, HELP_END);
    }
}

static void commands_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result))
        snprintf(result, AACS_SERVER_BUFFERS, "%d (sbus %d)", aapl->commands, aapl->sbus_commands);
}


static void close_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result)) strcpy(result, RES_CLOSE);
}


static void exit_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    if (check_term(aapl, cmd, cp, result))
        strcpy(result, RES_EXIT);
}

#if AAPL_ENABLE_FILE_IO
static void log_dump_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    FILE *file;
    (void) aapl;
    (void) cmd;

    SKIPSPACE(cp);
    file = fopen(cp, "w+");
    if (file)
    {
        snprintf(result, AACS_SERVER_BUFFERS, "%d bytes AAPL log written to %s and then cleared.", (int) strlen(aapl->log), cp);
        fprintf(file, "%s\n", aapl_log_get(aapl));
        fclose(file);
        aapl_log_clear(aapl);
    }
    else
        cmd_error(aapl, aapl->aacs_server_buffer, "Not able to open file %s to dump log to.", cp);
}
#endif

#if AAPL_ALLOW_AACS && AAPL_ENABLE_AACS_SERVER
static void lock_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    (void) aapl;
    (void) cmd;
    (void) cp;
    client_IPaddr_lock[1].sin_addr.s_addr = client_IPaddr_lock[0].sin_addr.s_addr;
    client_IPaddr_lock[0].sin_addr.s_addr = client_IPaddr.sin_addr.s_addr;
    snprintf(result, AACS_SERVER_BUFFERS, "Locked to %s.", inet_ntoa(client_IPaddr_lock[0].sin_addr));
}

static void allow_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    (void) aapl;
    (void) cmd;
    (void) cp;

    SKIPSPACE(cp);
    if (*cp)
    {
        struct hostent *server = NULL;
        server = gethostbyname(cp);
        if( server == NULL )
        {
            snprintf(result, AACS_SERVER_BUFFERS, "Unable to determine IP address to lock to.");
            return;
        }
        client_IPaddr_lock[1].sin_addr.s_addr = client_IPaddr_lock[0].sin_addr.s_addr;
        memmove((char *)&client_IPaddr_lock[0].sin_addr.s_addr, (char *)server->h_addr_list[0], server->h_length);
    }
    else
    {
        client_IPaddr_lock[1].sin_addr.s_addr = client_IPaddr_lock[0].sin_addr.s_addr;
        client_IPaddr_lock[0].sin_addr.s_addr = client_IPaddr.sin_addr.s_addr;
    }
    snprintf(result, AACS_SERVER_BUFFERS, "Locked to %s.", inet_ntoa(client_IPaddr_lock[0].sin_addr));
}

static void unlock_command(Aapl_t *aapl, const char * cmd, const char * cp, char * result)
{
    (void) aapl;
    (void) cmd;
    (void) cp;
    client_IPaddr_lock[0].sin_addr.s_addr = 0;
    client_IPaddr_lock[1].sin_addr.s_addr = 0;
    snprintf(result, AACS_SERVER_BUFFERS, "Unlocked.");
}

#if AAPL_ENABLE_PS2 && AAPL_ALLOW_SYSTEM_I2C && AAPL_ENABLE_FLOAT_USAGE && defined I2C_SMBUS_READ

/** @brief Returns number of tokens found and pointer to beginning of each.  Input string is not modified!
 ** @details argv[0] is set to full string, but return value doesn't count argv[0].
 **          In otherwords, the returned array is from [0..N] rather than the traditional [0..N-1]!
 **/
static int tokenize(const char *cp, int argv_len, const char *argv[])
{
    int i;
    argv[0] = cp;
    for( i = 1; i < argv_len; i++ )
    {
        SKIPSPACE(cp);
        if( ISEOL(cp) )
            break;
        argv[i] = cp++;
        while( !ISTERM(cp) )
            cp++;
    }
    return i-1;
}

static void devices_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    snprintf(result, AACS_SERVER_BUFFERS, "%d %d 0x%x", avago_ps2_supply_get_count(aapl), avago_ps2_clock_get_count(aapl), avago_ps2_get_config_flags(aapl));
    return;
}

static void ven_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    uint supply_count = avago_ps2_supply_get_count(aapl);
    uint which = supply_count, enable = 2;
    const char *argv[4];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);

    if( argc == 0 )
    {
        char *ptr = result;
        char *end = ptr + AACS_SERVER_BUFFERS;
        for( which = 0; which < supply_count; which++ )
            ptr += snprintf(ptr, end-ptr, "%d ", avago_ps2_supply_status(aapl, which));
        if( ptr > result ) *--ptr = '\0';
        return;
    }

    sscanf(argv[1], "%3u", &which);
    if( argc == 1 && which < supply_count )
    {
        snprintf(result, AACS_SERVER_BUFFERS, "%d", avago_ps2_supply_status(aapl, which));
        return;
    }

    sscanf(argv[2], "%3u", &enable);
    if( argc == 2 && which < supply_count && enable < 2 )
    {
        avago_ps2_supply_enable(aapl, which, enable);
        snprintf(result, AACS_SERVER_BUFFERS, "%d", avago_ps2_supply_status(aapl, which));
        return;
    }

    cmd_error(aapl, result, "Invalid \"ven\" command ignored. Must be: ven [<0..%u> [<0|1>]]. Got: \"%s\".", supply_count-1, cmd);
}

static void vread_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    uint supply_count = avago_ps2_supply_get_count(aapl);
    uint which = supply_count;
    const char *argv[4];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);

    if( argc == 0 )
    {
        char *ptr = result;
        char *end = ptr + AACS_SERVER_BUFFERS;
        for( which = 0; which < supply_count; which++ )
            ptr += snprintf(ptr, end-ptr, "%.3g ", avago_ps2_supply_read_voltage(aapl, which));
        if( ptr > result ) *--ptr = '\0';
        return;
    }

    sscanf(argv[1], "%3u", &which);
    if( argc == 1 && which < supply_count )
    {
        snprintf(result, AACS_SERVER_BUFFERS, "%.3g", avago_ps2_supply_read_voltage(aapl, which));
        return;
    }

    cmd_error(aapl, result, "Invalid \"vread\" command ignored. Must be: vread [<0..%d>]. Got: \"%s\".", supply_count-1, cmd);
}

static void vset_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    uint supply_count = avago_ps2_supply_get_count(aapl);
    uint which = supply_count;
    float voltage;
    const char *argv[4];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);

    if( argc == 0 )
    {
        char *ptr = result;
        char *end = ptr + AACS_SERVER_BUFFERS;
        for( which = 0; which < supply_count; which++ )
            ptr += snprintf(ptr, end-ptr, "%.3g ", avago_ps2_supply_read_set_voltage(aapl, which));
        if( ptr > result ) *--ptr = '\0';
        return;
    }

    sscanf(argv[1], "%3u", &which);
    if( argc == 1 && which < supply_count )
    {
        snprintf(result, AACS_SERVER_BUFFERS, "%.3g", avago_ps2_supply_read_set_voltage(aapl, which));
        return;
    }

    if( argc == 2 && which < supply_count && 1 == sscanf(argv[2], "%8f", &voltage) )
    {
        avago_ps2_supply_set_voltage(aapl, which, voltage);
        snprintf(result, AACS_SERVER_BUFFERS, "%g", avago_ps2_supply_read_set_voltage(aapl, which));
        return;
    }

    cmd_error(aapl, result, "Invalid \"vset\" command ignored. Must be: vset [<0..%d> [<volts>]]. Got: \"%s\".", supply_count-1, cmd);
}

static void iread_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    uint supply_count = avago_ps2_supply_get_count(aapl);
    uint which = supply_count;
    const char *argv[4];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);

    if( argc == 0 )
    {
        char *ptr = result;
        char *end = ptr + AACS_SERVER_BUFFERS;
        for( which = 0; which < supply_count; which++ )
            ptr += snprintf(ptr, end-ptr, "%.3g ", avago_ps2_supply_read_current(aapl, which));
        if( ptr > result ) *--ptr = '\0';
        return;
    }

    sscanf(argv[1], "%3u", &which);
    if( argc == 1 && which < supply_count )
    {
        snprintf(result, AACS_SERVER_BUFFERS, "%.3g", avago_ps2_supply_read_current(aapl, which));
        return;
    }

    cmd_error(aapl, result, "Invalid \"iread\" command ignored. Must be: iread [<0..%d>]. Got: \"%s\".", supply_count-1, cmd);
}

static void iset_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    uint supply_count = avago_ps2_supply_get_count(aapl);
    uint which = supply_count;
    float amps;
    const char *argv[4];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);

    if( argc == 0 )
    {
        char *ptr = result;
        char *end = ptr + AACS_SERVER_BUFFERS;
        for( which = 0; which < supply_count; which++ )
            ptr += snprintf(ptr, end-ptr, "%.3g ", avago_ps2_supply_read_current_limit(aapl, which));
        if( ptr > result ) *--ptr = '\0';
        return;
    }

    sscanf(argv[1], "%3u", &which);
    if( argc == 1 && which < supply_count )
    {
        snprintf(result, AACS_SERVER_BUFFERS, "%.3g", avago_ps2_supply_read_current_limit(aapl, which));
        return;
    }

    if( argc == 2 && which < supply_count && 1 == sscanf(argv[2], "%8f", &amps) )
    {
        avago_ps2_supply_set_current_limit(aapl, which, amps);
        snprintf(result, AACS_SERVER_BUFFERS, "%g", avago_ps2_supply_read_current_limit(aapl, which));
        return;
    }

    cmd_error(aapl, result, "Invalid \"iset\" command ignored. Must be: iset [<0..%d> [<amps>]]. Got: \"%s\".", supply_count-1, cmd);
}

static void wread_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    uint supply_count = avago_ps2_supply_get_count(aapl);
    uint which = supply_count;
    const char *argv[4];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);

    if( argc == 0 )
    {
        char *ptr = result;
        char *end = ptr + AACS_SERVER_BUFFERS;
        for( which = 0; which < supply_count; which++ )
            ptr += snprintf(ptr, end-ptr, "%.3g ", avago_ps2_supply_read_power(aapl, which));
        if( ptr > result ) *--ptr = '\0';
        return;
    }

    sscanf(argv[1], "%3u", &which);
    if( argc == 1 && which < supply_count )
    {
        snprintf(result, AACS_SERVER_BUFFERS, "%.3g", avago_ps2_supply_read_power(aapl, which));
        return;
    }

    cmd_error(aapl, result, "Invalid \"wread\" command ignored. Must be: wread [<0..%d>]. Got: \"%s\".", supply_count-1, cmd);
}

#if AAPL_ENABLE_PS2_KEYSTONE
static void kclken_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    const int clock_count = 12;
    int which = clock_count, enable;
    const char *argv[20];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);

    if( argc == 0 )
    {
        int i;
        char *ptr = result;
        for( i = 0; i < clock_count; i++ )
            ptr += snprintf(ptr, AACS_SERVER_BUFFERS, "%d ", avago_ps2_kclock_status(aapl, i));
        if( ptr > result ) ptr[-1] = '\0';
        return;
    }

    sscanf(argv[1], "%3d", &which);
    if( argc == 1 && which < clock_count && which >= 0 )
    {
        snprintf(result, AACS_SERVER_BUFFERS, "%d", avago_ps2_kclock_status(aapl, which));
        return;
    }

    enable = aapl_strtoul(argv[2],NULL,0);
    if( argc == 2 && which < clock_count && enable < 2 && ISTERM(&argv[2][1]) )
    {
        avago_ps2_kclock_enable(aapl, which, enable);
        snprintf(result, AACS_SERVER_BUFFERS, "%d", avago_ps2_kclock_status(aapl, which));
        return;
    }
    cmd_error(aapl, result, "Invalid \"kclken\" command ignored. Must be: kclken [0..11 [0-1]]; Got: \"%s\".", cmd);
}

static void kclkread_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    const uint clock_count = 12;
    uint which = clock_count;
    const char *argv[20];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);

    if( argc == 0 )
    {
        uint i;
        char *ptr = result;
        for( i = 0; i < clock_count; i++ )
        {
            uint f_n = avago_ps2_kclock_read(aapl, i);
            ptr += snprintf(ptr, AACS_SERVER_BUFFERS, "%.11g ", f_n/1000000.0);
        }
        if( ptr > result ) ptr[-1] = '\0';
        return;
    }

    sscanf(argv[1], "%3u", &which);
    if( argc == 1 && which < clock_count )
    {
        uint f_n = avago_ps2_kclock_read(aapl, which);
        snprintf(result, AACS_SERVER_BUFFERS, "%.11g", f_n/1000000.0);
        return;
    }

    cmd_error(aapl, result, "Invalid \"kclkread\" command ignored. Must be: kclkread [0-11]; Got: \"%s\".", cmd);
}
#endif

static void clken_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    uint clock_count = avago_ps2_clock_get_count(aapl);
    uint which = clock_count, enable = 2;
    const char *argv[4];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);

    if( argc == 0 )
    {
        char *ptr = result;
        char *end = ptr + AACS_SERVER_BUFFERS;
        for( which = 0; which < clock_count; which++ )
            ptr += snprintf(ptr, end-ptr, "%d ", avago_ps2_clock_status(aapl, which));
        if( ptr > result ) *--ptr = '\0';
        return;
    }

    sscanf(argv[1], "%3u", &which);
    if( argc == 1 && which < clock_count )
    {
        snprintf(result, AACS_SERVER_BUFFERS, "%d", avago_ps2_clock_status(aapl, which));
        return;
    }

    enable = aapl_strtoul(argv[2],NULL,0);
    if( argc == 2 && which < clock_count && enable < 2 && ISTERM(&argv[2][1]) )
    {
        avago_ps2_clock_enable(aapl, which, enable);
        snprintf(result, AACS_SERVER_BUFFERS, "%d", avago_ps2_clock_status(aapl, which));
        return;
    }
    cmd_error(aapl, result, "Invalid \"clken\" command ignored. Must be: clken [0..%d [0|1]]; Got: \"%s\".", clock_count-1, cmd);
}

static void clkread_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    uint clock_count = avago_ps2_clock_get_count(aapl);
    uint which = clock_count;
    const char *argv[AAPL_PS2_MAX_CLOCKS + 3];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);

    if( argc == 0 )
    {
        uint fin, fout[AAPL_PS2_MAX_CLOCKS];
        uint i;
        char fmode, *ptr = result, *end = result + AACS_SERVER_BUFFERS;

        avago_ps2_clock_readv(aapl, AAPL_PS2_MAX_CLOCKS, fout, &fmode, &fin);
        for( i = 0; i < clock_count; i++ )
            ptr += snprintf(ptr, end - ptr, "%.11g ", fout[i]/1000000.0);
        *--ptr = '\0';
        return;
    }

    sscanf(argv[1], "%3u", &which);
    if( argc == 1 && which < clock_count )
    {
        uint f_n = avago_ps2_clock_read(aapl, which, 0, 0);
        snprintf(result, AACS_SERVER_BUFFERS, "%.11g", f_n/1000000.0);
        return;
    }

    cmd_error(aapl, result, "Invalid \"clkread\" command ignored. Must be: clkread [0..%d]; Got: \"%s\".", clock_count-1, cmd);
}

static void clkset_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{

    char fmode = 'x';
    uint fin = 0;
    uint clock_count = avago_ps2_clock_get_count(aapl);
    uint which = clock_count;
    const char *argv[AAPL_PS2_MAX_CLOCKS + 3];
    uint argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);

    if( argc == 0 )
    {
        uint fout[AAPL_PS2_MAX_CLOCKS];
        uint i;
        char *ptr = result, *end = result + AACS_SERVER_BUFFERS;

        avago_ps2_clock_readv(aapl, AAPL_PS2_MAX_CLOCKS, fout, &fmode, &fin);
        ptr += snprintf(ptr, end - ptr, "%c %.11g", fmode, fin/1000000.0);
        for( i = 0; i < clock_count; i++ )
            ptr += snprintf(ptr, end - ptr, " %.11g", fout[i]/1000000.0);
        return;
    }

    sscanf(argv[1], "%3u", &which);
    if( argc == 1 && which < clock_count )
    {
        uint f_n = avago_ps2_clock_read(aapl, which, 0, 0);
        snprintf(result, AACS_SERVER_BUFFERS, "%.11g", f_n/1000000.0);
        return;
    }

    if( argc == 2 && which < clock_count )
    {
        double f_out;
        if( 1 == sscanf(argv[2], "%12lf", &f_out) && f_out >= 0.2 && f_out <= 1500.0 )
        {
            uint f_n, f_set = f_out * 1000000;
            avago_ps2_clock_set(aapl, which, f_set);
            f_n = avago_ps2_clock_read(aapl, which, 0, 0);
            snprintf(result, AACS_SERVER_BUFFERS, "%.11g", f_n/1000000.0);
            return;
        }
        cmd_error(aapl, result, "Valid frequency range is [0.2 .. 1500] MHz. Got: \"%s\".", cmd);
        return;
    }
    which = argv[1][0];

    if( argc == (2+clock_count) && (which == 't' || which == 'e' || which == 'c') )
    {
        double ffin;
        if( 1 == sscanf(argv[2], "%12lf", &ffin) )
        {
            uint i;
            for( i = 1; i <= clock_count; i++ )
                argv[i] = argv[i+2];
            argc -= 2;
            fmode = which;
            fin = ffin * 1000000;
        }
    }

    if( argc == clock_count )
    {
        uint fout[AAPL_PS2_MAX_CLOCKS];
        uint i;
        for( i = 1; i <= clock_count; i++ )
        {
            double f_set;
            if( 1 != sscanf(argv[i], "%12lf", &f_set) )
                break;
            fout[i-1] = f_set * 1000000;
        }
        if( i > clock_count )
        {
            char *ptr = result, *end = result + AACS_SERVER_BUFFERS;

            avago_ps2_clock_setv(aapl, clock_count, fout, fmode, fin);
            avago_ps2_clock_readv(aapl, AAPL_PS2_MAX_CLOCKS, fout, &fmode, &fin);
            ptr += snprintf(ptr, end - ptr, "%c %.11g", fmode, fin/1000000.0);
            for( i = 0; i < clock_count; i++ )
                ptr += snprintf(ptr, end - ptr, " %.11g", fout[i]/1000000.0);
            return;
        }
        cmd_error(aapl, result, "Valid frequency range is [0.2 .. 1500] MHz. Got: \"%s\".", cmd);
    }
    cmd_error(aapl, result, "Invalid \"clkset\" command ignored. Must be: clkset [ 0..%d [f_MHz] | [{t|e} <f_XX>] <freq0> .. <freqN-1> ]; Got: \"%s\".", clock_count-1, cmd);
}

static void clkdrv_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{

    uint clock_count = avago_ps2_clock_get_count(aapl);
    uint which = clock_count;
    const char *argv[AAPL_PS2_MAX_CLOCKS + 3];
    uint argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);


    if( argc == 0 )
    {
        uint i;
        char *ptr = result, *end = result + AACS_SERVER_BUFFERS;

        for( i = 0; i < clock_count; i++ )
            ptr += snprintf(ptr, end - ptr, "%s ", avago_ps2_clock_driver_read(aapl, i));
        *--end = '\0';
        return;
    }

    sscanf(argv[1], "%3u", &which);
    if( argc == 1 && which < clock_count )
    {
        snprintf(result, AACS_SERVER_BUFFERS, "%s", avago_ps2_clock_driver_read(aapl, which));
        return;
    }

    if( argc == 2 && which < clock_count )
    {
        char value[12];
        if( 1 == sscanf(argv[2], "%11s", value) )
        {
            uint status = avago_ps2_clock_driver_set(aapl, which, value);
            if( status )
            {
                snprintf(result, AACS_SERVER_BUFFERS, "%s%s", avago_ps2_clock_driver_read(aapl, which), status == 2 ? " # not configurable" : "");
                return;
            }
        }
        cmd_error(aapl, result, "Arg 2 must be <logic-family>@<voltage>, one of: LVPECL@2.5, LVPECL@3.3, HCSL@1.8, HCSL@2.5, HCSL@3.3. Got: \"%s\".", cmd);
        return;
    }
    cmd_error(aapl, result, "Invalid \"clkdrv\" command ignored. Must be: clkdrv [ 0..%d [<logic_family>@<voltage>] ]; Got: \"%s\".", clock_count-1, cmd);
}

static void smbus_wb_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    const char *argv[5];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);
    if( argc == 2 )
    {
        char *cp1, *cp2;
        uint dev_addr = (uint) aapl_strtol(argv[1], &cp1, 0);
        uint reg_addr = (uint) aapl_strtol(argv[2], &cp2, 0);
        if( ISTERM(cp1) && ISTERM(cp2) )
        {
            avago_smbus_write_byte(aapl, dev_addr, reg_addr);
            snprintf(result, AACS_SERVER_BUFFERS, "0");
            return;
        }
    }
    cmd_error(aapl, result, "Invalid \"smbus_wb\" command ignored. Must be: smbus_wb <dev_addr> <reg_addr>. Got: \"%s\".", cmd);
}
static void smbus_wbd_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    const char *argv[5];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);
    if( argc == 3 )
    {
        char *cp1, *cp2, *cp3;
        uint dev_addr = (uint) aapl_strtol(argv[1], &cp1, 0);
        uint reg_addr = (uint) aapl_strtol(argv[2], &cp2, 0);
        uint data     = (uint) aapl_strtol(argv[3], &cp3, 0);
        if( ISTERM(cp1) && ISTERM(cp2) && ISTERM(cp3) )
        {
            avago_smbus_write_byte_data(aapl, dev_addr, reg_addr, data);
            snprintf(result, AACS_SERVER_BUFFERS, "0");
            return;
        }
    }
    cmd_error(aapl, result, "Invalid \"smbus_wbd\" command ignored. Must be: smbus_wbd <dev_addr> <reg_addr> <data>. Got: \"%s\".", cmd);
}
static void smbus_wwd_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    const char *argv[5];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);
    if( argc == 3 )
    {
        char *cp1, *cp2, *cp3;
        uint dev_addr = (uint) aapl_strtol(argv[1], &cp1, 0);
        uint reg_addr = (uint) aapl_strtol(argv[2], &cp2, 0);
        uint data     = (uint) aapl_strtol(argv[3], &cp3, 0);
        if( ISTERM(cp1) && ISTERM(cp2) && ISTERM(cp3) )
        {
            avago_smbus_write_word_data(aapl, dev_addr, reg_addr, data);
            snprintf(result, AACS_SERVER_BUFFERS, "0");
            return;
        }
    }
    cmd_error(aapl, result, "Invalid \"smbus_wwd\" command ignored. Must be: smbus_wwd <dev_addr> <reg_addr> <data>. Got: \"%s\".", cmd);
}

static void smbus_rbd_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    const char *argv[5];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);
    if( argc == 2 )
    {
        char *cp1, *cp2;
        uint dev_addr = (uint) aapl_strtol(argv[1], &cp1, 0);
        uint reg_addr = (uint) aapl_strtol(argv[2], &cp2, 0);
        if( ISTERM(cp1) && ISTERM(cp2) )
        {
            uint data = avago_smbus_read_byte_data(aapl, dev_addr, reg_addr);
            snprintf(result, AACS_SERVER_BUFFERS, "0x%x", data);
            return;
        }
    }
    cmd_error(aapl, result, "Invalid \"smbus_rbd\" command ignored. Must be: smbus_rbd <dev_addr> <reg_addr>. Got: \"%s\".", cmd);
}

static void smbus_rwd_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    const char *argv[5];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);
    if( argc == 2 )
    {
        char *cp1, *cp2;
        uint dev_addr = (uint) aapl_strtol(argv[1], &cp1, 0);
        uint reg_addr = (uint) aapl_strtol(argv[2], &cp2, 0);
        if( ISTERM(cp1) && ISTERM(cp2) )
        {
            uint data = avago_smbus_read_word_data(aapl, dev_addr, reg_addr);
            snprintf(result, AACS_SERVER_BUFFERS, "0x%x", data);
            return;
        }
    }
    cmd_error(aapl, result, "Invalid \"smbus_rwd\" command ignored. Must be: smbus_rwd <dev_addr> <reg_addr>. Got: \"%s\".", cmd);
}

static void smbus_sniff_command(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    const char *argv[5];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);
    if( argc == 1 )
    {
        char *cp1;
        uint dev_addr = (uint) aapl_strtol(argv[1], &cp1, 0);
        if( ISTERM(cp1) )
        {
            int data = avago_smbus_sniff_target(aapl, dev_addr);
            snprintf(result, AACS_SERVER_BUFFERS, "0x%x", data);
            return;
        }
    }
    cmd_error(aapl, result, "Invalid \"smbus_sniff\" command ignored. Must be: smbus_sniff <dev_addr>. Got: \"%s\".", cmd);
}

static void reset_supplies_cmd(Aapl_t *aapl, const char *cmd, const char *cp, char *result)
{
    const char *argv[5];
    int argc = tokenize(cp, AAPL_ARRAY_LENGTH(argv), argv);
    if( argc == 0 )
    {
        avago_ps2_reset_supplies(aapl);
        snprintf(result, AACS_SERVER_BUFFERS, "0");
        return;
    }
    cmd_error(aapl, result, "Invalid \"reset_supplies\" command ignored. Command takes no options. Got: \"%s\".", cmd);
}

#endif

#endif



char *avago_aacs_process_cmd(
    Aapl_t *aapl,       /**< [in] */
    const char *cmd,    /**< [in] */
    int *chip_nump,     /**< [in/out] */
    int *ring_nump)     /**< [in/out] */
{
    int errors, warnings, rc;
    const char *cp = cmd;
    int x = 0;
    int y = 0;
    if (!chip_nump) chip_nump = &x;
    if (!ring_nump) ring_nump = &y;

    if (!aapl->aacs_server_buffer ) aapl->aacs_server_buffer = (char *) aapl_malloc(aapl, AACS_SERVER_BUFFERS, __func__);
    if( !aapl->aacs_server_buffer ) return NULL;
    SKIPSPACE(cp);

    if (strchr(cmd, ';'))
    {
        char next_cmd[AACS_SERVER_BUFFERS], result_buf[AACS_SERVER_BUFFERS];
        const char * index;
        const char * remaining_cmd;
        int result_index = 0;
        int first_cmd = 0;

        result_buf[0] = 0;
        next_cmd[0] = 0;

        remaining_cmd = cmd;
        do
        {
            const char *result;
            index = strchr(remaining_cmd, ';');
            if (index) strncpy(next_cmd, remaining_cmd, index - remaining_cmd);
            else       strncpy(next_cmd, remaining_cmd, AAPL_ARRAY_LENGTH(next_cmd)-1);
            if (index) next_cmd[index-remaining_cmd] = 0;
            remaining_cmd = index + 1;

            result = avago_aacs_process_cmd(aapl, next_cmd, chip_nump, ring_nump);
            if( !result ) continue;
            if (strlen(result) + strlen(result_buf) > AACS_SERVER_BUFFERS)
            {
                aapl_fail(aapl, __func__, __LINE__, "Result buffer overflow while trying to generate response. Output truncated.\n");
                break;
            }
            if (!first_cmd++) result_index += snprintf(result_buf + result_index, AACS_SERVER_BUFFERS, "%s", result);
            else              result_index += snprintf(result_buf + result_index, AACS_SERVER_BUFFERS, ";%s", result);
        } while (index);

        strncpy(aapl->aacs_server_buffer, result_buf, AACS_SERVER_BUFFERS);
        return aapl->aacs_server_buffer;
    }

    aapl->commands++;
#if AAPL_ENABLE_CONSOLE
    if (enable_console_mode)
    {
        aapl_printf_lock(aapl);
        if (aapl->commands % 100 == 0) console_logging_fn(aapl, AVAGO_INFO, "update AAPL console log", 0);
        aapl_printf_unlock(aapl);
    }
#endif
    rc = aapl->return_code;
    errors = aapl->errors;
    warnings = aapl->warnings;

    if (cp[0] == '@') cp++;

#if AAPL_ALLOW_AACS && AAPL_ENABLE_AACS_SERVER
    if ((client_IPaddr_lock[0].sin_addr.s_addr || client_IPaddr_lock[1].sin_addr.s_addr)
        && !(   (client_IPaddr_lock[0].sin_addr.s_addr && client_IPaddr_lock[0].sin_addr.s_addr == client_IPaddr.sin_addr.s_addr)
             || (client_IPaddr_lock[1].sin_addr.s_addr && client_IPaddr_lock[1].sin_addr.s_addr == client_IPaddr.sin_addr.s_addr))
        && (   strstr(cmd, "version") == 0
            && strstr(cmd, "unlock") == 0
            && strstr(cmd, "close") == 0
            && strstr(cmd, "status") == 0
            && strstr(cmd, "allow") == 0))
    {
        char lock0[64], lock1[64];
        lock0[0] = 0;
        lock1[0] = 0;

        if ((client_IPaddr_lock[0].sin_addr.s_addr)) snprintf(lock0, 64, "%s", inet_ntoa(client_IPaddr_lock[0].sin_addr));
        if ((client_IPaddr_lock[1].sin_addr.s_addr)) snprintf(lock1, 64, "%s", inet_ntoa(client_IPaddr_lock[1].sin_addr));

        cmd_error(aapl, aapl->aacs_server_buffer, "AACS server is currently locked by %s%s%s. Command(s) ignored.", lock0, lock1[0] ? " and " : "", lock1);
        goto end;
    }
#endif

    if (!ISCMD(cp, "status", 6)) time(&last_client_time);

    if      (ISCMD(cp, "sbus",        4)) sbus_command(       aapl, cmd, cp + 4, aapl->aacs_server_buffer, chip_nump, ring_nump);
#if AAPL_ENABLE_BLACKHAWK
    else if (ISCMD(cp, "pmi",         3)) pmi_command(        aapl, cmd, cp + 3, aapl->aacs_server_buffer, chip_nump, ring_nump);
#endif
    else if (cp[0] == '#'               ) null_command(       aapl, cmd, cp    , aapl->aacs_server_buffer);
    else if (ISCMD(cp, "version",     7)) version_command(    aapl, cmd, cp + 7, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "spico_int",   9)) spico_int_command(  aapl, cmd, cp + 9, aapl->aacs_server_buffer);
    else goto next_command;
    goto end;


    next_command:
    AAPL_WAIT_FOR_SBUS_THREAD(aapl);
    if      (ISCMD(cp, "jtag",        4)) jtag_command(       aapl, cmd, cp + 4, aapl->aacs_server_buffer, chip_nump);
    else if (ISCMD(cp, "chip",        4)) chip_command(       aapl, cmd, cp + 4, aapl->aacs_server_buffer, chip_nump);
    else if (ISCMD(cp, "chips",       5)) chips_command(      aapl, cmd, cp + 5, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "chipnum",     7)) chipnum_command(    aapl, cmd, cp + 7, aapl->aacs_server_buffer, chip_nump);
    else if (ISCMD(cp, "status",      6)) status_command (    aapl, cmd, cp + 6, &aapl->aacs_server_buffer);
#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
    else if (ISCMD(cp, "jtag_cycle", 10)) jtag_cycle_command( aapl, cmd, cp +10, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "jtag_scan",   9)) jtag_scan_command(  aapl, cmd, cp + 9, aapl->aacs_server_buffer);
#endif
#if AAPL_ALLOW_SYSTEM_I2C && defined I2C_SMBUS_READ
    else if (ISCMD(cp, "clken",       5)) clken_command (     aapl, cmd, cp + 5, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "clkread",     7)) clkread_command (   aapl, cmd, cp + 7, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "clkset",      6)) clkset_command (    aapl, cmd, cp + 6, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "clkdrv",      6)) clkdrv_command (    aapl, cmd, cp + 6, aapl->aacs_server_buffer);
#if AAPL_ENABLE_PS2_KEYSTONE
    else if (ISCMD(cp, "kclken",      6)) kclken_command (    aapl, cmd, cp + 6, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "kclkread",    8)) kclkread_command (  aapl, cmd, cp + 8, aapl->aacs_server_buffer);
#endif
    else if (ISCMD(cp, "devices",     7)) devices_command (   aapl, cmd, cp + 7, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "iread",       5)) iread_command (     aapl, cmd, cp + 5, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "iset",        4)) iset_command (      aapl, cmd, cp + 4, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "ven",         3)) ven_command (       aapl, cmd, cp + 3, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "vread",       5)) vread_command (     aapl, cmd, cp + 5, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "vset",        4)) vset_command (      aapl, cmd, cp + 4, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "wread",       5)) wread_command (     aapl, cmd, cp + 5, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "smbus_wwd",   9)) smbus_wwd_command(  aapl, cmd, cp + 9, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "smbus_wbd",   9)) smbus_wbd_command(  aapl, cmd, cp + 9, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "smbus_wb",    8)) smbus_wb_command(   aapl, cmd, cp + 8, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "smbus_rbd",   9)) smbus_rbd_command(  aapl, cmd, cp + 9, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "smbus_rwd",   9)) smbus_rwd_command(  aapl, cmd, cp + 9, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "smbus_sniff", 11)) smbus_sniff_command(  aapl, cmd, cp + 11, aapl->aacs_server_buffer);
    else if (ISCMD(cp,"reset_supplies",14))reset_supplies_cmd(aapl, cmd, cp +14, aapl->aacs_server_buffer);
#endif
#if AAPL_ALLOW_I2C || AAPL_ALLOW_SYSTEM_I2C
    else if (ISCMD(cp, "i2c",         3)) i2c_command(        aapl, cmd, cp + 3, aapl->aacs_server_buffer);
#endif
#if AAPL_ALLOW_MDIO || AAPL_ALLOW_GPIO_MDIO
    else if (ISCMD(cp, "mdio",        4)) mdio_command(       aapl, cmd, cp + 4, aapl->aacs_server_buffer);
#endif
    else if (ISCMD(cp, "sbus_mode",   9)) sbus_mode_command(  aapl, cmd, cp + 9, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "sleep",       5)) sleep_command(      aapl, cmd, cp + 5, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "sbus_reset", 10)) sbus_reset_command( aapl, cmd, cp +10, aapl->aacs_server_buffer);
#if AAPL_ENABLE_ATE_VEC
    else if (ISCMD(cp, "ate_vec",     7)) ate_vec_command(    aapl, cmd, cp + 7, aapl->aacs_server_buffer);
#endif
#if AAPL_ENABLE_DIAG
    else if (ISCMD(cp, "diag",        4)) diag_command(       aapl, cmd, cp + 4, aapl->aacs_server_buffer);
#endif
#if AAPL_ALLOW_AACS
    else if (ISCMD(cp, "send",        4)) my_send_command(    aapl, cmd, cp + 4, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "reset",       5)) reset_command(      aapl, cmd, cp + 5, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "set_sbus",    8)) set_sbus_command(   aapl, cmd, cp + 8, aapl->aacs_server_buffer);
#endif
#if AAPL_ALLOW_AACS && AAPL_ENABLE_AACS_SERVER
    else if (ISCMD(cp, "allow",       5)) allow_command(      aapl, cmd, cp + 5, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "lock",        4)) lock_command(       aapl, cmd, cp + 4, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "unlock",      6)) unlock_command(     aapl, cmd, cp + 6, aapl->aacs_server_buffer);
#else
    else if (ISCMD(cp, "send",        4)) null_command(       aapl, cmd, cp + 4, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "reset",       5)) null_command(       aapl, cmd, cp    , aapl->aacs_server_buffer);
    else if (ISCMD(cp, "set_sbus",    8)) null_command(       aapl, cmd, cp    , aapl->aacs_server_buffer);
#endif
    else if (ISCMD(cp, "null",        4)) null_command(       aapl, cmd, cp + 4, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "help",        4)) help_command(       aapl, cmd, cp + 4, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "set_debug",   9)) set_uint(           aapl, cmd, cp + 9, &aapl->debug,      aapl->aacs_server_buffer);
    else if (ISCMD(cp, "tck_delay",   9)) set_uint(           aapl, cmd, cp + 9, &aapl->tck_delay,  aapl->aacs_server_buffer);
    else if (ISCMD(cp, "close",       5)) close_command(      aapl, cmd, cp + 5, aapl->aacs_server_buffer);
    else if (ISCMD(cp, "exit",        4)) exit_command(       aapl, cmd, cp + 4, aapl->aacs_server_buffer);
#if AAPL_ENABLE_FILE_IO
    else if (ISCMD(cp, "log_dump",    8)) log_dump_command(   aapl, cmd, cp + 8, aapl->aacs_server_buffer);
#endif
    else if (ISCMD(cp, "commands",    8)) commands_command(   aapl, cmd, cp + 8, aapl->aacs_server_buffer);
#if AAPL_ENABLE_MAIN
    else if (ISCMD(cp, "cli",         3)) cli_command(        aapl, cmd, cp + 3, aapl->aacs_server_buffer);
#endif
#if AAPL_ENABLE_PS2 && AAPL_ENABLE_FILE_IO && AAPL_ENABLE_AACS_SERVER
    else if (ISCMD(cp, "ps2_update", 10)) ps2_update(         aapl, cmd, cp +10, aapl->aacs_server_buffer);
#endif
    else if (ISNUL(cp)) aapl->aacs_server_buffer[0] = '\0';
    else if ((cp[0] >= '0' && cp[0] <= '9') || (cp[0] >= 'a' && cp[0] <= 'f') || (cp[0] >= 'A' && cp[0] <= 'F'))
    {
        const char *cp2;
        aapl_num_from_hex(cp, &cp2, 2, 4);
        if (cp2 != cp)                    sbus_command(       aapl, cmd, cp + 0, aapl->aacs_server_buffer, chip_nump, ring_nump);
        else cmd_error(aapl, aapl->aacs_server_buffer, "Unrecognized command ignored: \"%s\".", cmd);
    }
    else cmd_error(aapl, aapl->aacs_server_buffer, "Unrecognized command ignored: \"%s\".", cmd);

    end:
#if AAPL_ALLOW_JTAG || AAPL_ALLOW_AACS
    if ((int) aapl->curr_chip != *chip_nump)
    {
        AAPL_WAIT_FOR_SBUS_THREAD(aapl);
        aapl_set_chip(aapl, *chip_nump);
        aapl->curr_chip = *chip_nump;
    }
#endif

    if (cmd[0] == '@') aapl->aacs_server_buffer[0] = 0;


    if (rc != aapl->return_code || errors != aapl->errors || warnings != aapl->warnings)
    {
        char result_buf[256];
        strncpy(result_buf, aapl->aacs_server_buffer, 255);
        cmd_error(aapl, aapl->aacs_server_buffer, "Command failed: \"%s\". Error: \"%s\". Result => %s %s", cmd, aapl->last_err, "\x1\x1\x1", result_buf);
    }

    return aapl->aacs_server_buffer;
}
#endif


#if AAPL_ENABLE_AACS_SERVER
static void update_server_status(Aapl_t *aapl, int *chip_num, int *ring_num)
{
    #if AAPL_ENABLE_PS2
    FILE *file = fopen("/var/www/html/server_status.htm.new", "w+");
    if (file)
    {
        char *result = avago_aacs_process_cmd(aapl, "status", chip_num, ring_num);
        if( result )
        {
            aapl_str_rep(result, ';', 0xa);
            fprintf(file, "<pre>\n%s\n%s\n", "<!--#include virtual=\"/cgi-bin/update.cgi\"-->", result);
        }
        fclose(file);
        rename("/var/www/html/server_status.htm.new","/var/www/html/server_status.htm");
    }
    #else
    (void) aapl;
    (void) chip_num;
    (void) ring_num;
    #endif
}


/** @brief   Starts an AACS Server and listens for requests.
 ** @details Starts (and restarts after closure) an AACS server that accepts one TCP/IP
 ** connection on tcp_port, receives zero or more AACS commands, forwards them
 ** to avago_sbus_wr(), etc, and returns response text to the command client.
 **
 ** The caller must first:
 **
 ** - Open a lower-level (back end) connection or whatever else is required for
 **   avago_sbus_wr(), etc to function, such as by using aapl_connect() (but not
 **   necessarily this function in the end user context).
 **
 ** - Start a process or thread that's taken over by this function, which only
 **   returns upon "exit" or internal error.
 **
 ** Note:  Unlike the HW server (HS1, etc), this process does not support
 ** command batching.  It expects just one command => response at a time.
 **
 ** @return Returns FALSE for internal error logged, or TRUE for client "exit" command.
 ** Does not return for client command anomalies or "close" commands.
 **/

int avago_aacs_server_options(
    Aapl_t *aapl,                       /**< [in] Pointer to Aapl_t structure. */
    Avago_aacs_server_config_t *config) /** <[in] Struct containing configuration settings */
{
    int chip_num = 0;
    int ring_num = 0;
    int error_or_exit = 0;
    int return_status = 0;
    int rc = 1, rc_ts = 1;
    int fd_socket, fd_socket_ts = -1;
    int return_code = 0;
    Aapl_t * aapl_ats_server_host = NULL;
    struct sockaddr_in sai, sai_ts;
    Aapl_t *aapl_server = aapl_construct();
    socklen_t addr_len = sizeof(client_IPaddr);
    socklen_t addr_len_ts = sizeof(client_IPaddr_ts);
#if AAPL_ALLOW_THREAD_SUPPORT && AAPL_ENABLE_PS2
    Aapl_server_cntl_t server_cntl = {0,0,0};
#endif
    aapl_client_username[0] = 0;
    if( !aapl_server ) return 0;

#if AAPL_ENABLE_CONSOLE
    if( config->enable_console_mode )
    {
        console_setup(aapl);
        server_cntl.enable_console_mode = TRUE;
        enable_console_mode = TRUE;
    }
#endif

#if AAPL_ENABLE_PS2
#   if AAPL_ALLOW_THREAD_SUPPORT
    server_cntl.aapl_upstream = aapl_server;
    server_cntl.aapl_downstream = aapl;
    if (config->enable_telnet_server && !aapl->telnet_thread) pthread_create(&aapl->telnet_thread, NULL, &aapl_telnet_thread, (void *)&server_cntl);

#   if AAPL_ENABLE_CONSOLE
    {
        pthread_t x;
        if (config->enable_console_mode) pthread_create(&x, NULL, &aapl_mouse_thread, (void *)&server_cntl);
    }
#   endif
#   endif

#if AAPL_ALLOW_SYSTEM_I2C && defined I2C_SMBUS_READ
        ps2_aacs_server_startup_init(aapl);
#endif
#endif

    client_IPaddr_lock[0].sin_addr.s_addr = 0;
    client_IPaddr_lock[1].sin_addr.s_addr = 0;

    # ifdef WIN32
    {
        WSADATA wsaData;
        WORD vers_req = MAKEWORD(2,2);
        WSAStartup(vers_req,&wsaData);
    }
    # endif


    fd_socket = socket(PF_INET, SOCK_STREAM, 0);
    fd_socket_ts = socket(PF_INET, SOCK_STREAM, 0);
    if( fd_socket < 0 || fd_socket_ts < 0 )
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot create PF_INET socket: %s.\n", aapl_tcp_strerr());
        goto cleanup_and_exit;
    }

    rc = setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &rc, sizeof(rc));
    rc_ts = setsockopt(fd_socket_ts, SOL_SOCKET, SO_REUSEADDR, (char *) &rc_ts, sizeof(rc_ts));

    rc = rc_ts = 1;
    rc = setsockopt(fd_socket, SOL_SOCKET, SO_KEEPALIVE, (char *) &rc, sizeof(rc));
    rc_ts = setsockopt(fd_socket_ts, SOL_SOCKET, SO_KEEPALIVE, (char *) &rc_ts, sizeof(rc_ts));

    if( rc < 0 || rc_ts < 0 )
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot setsockopt(SO_REUSEADDR) on socket %d: %s.\n", fd_socket, aapl_tcp_strerr());
        goto cleanup_and_exit;
    }

    if (config->ats_server_host)
    {
        aapl_log_printf(aapl, AVAGO_INFO, __func__, 0, "Attempting to connect to remote AACS ATS server: %s:%d.\n", config->ats_server_host, config->ats_server_host_port);
        aapl_ats_server_host = aapl_construct();
        aapl_ats_server_host->debug = aapl->debug;
        aapl_ats_server_host->disable_reconnect = 1;
    }
    else
    {
        sai.sin_family      = AF_INET;
        sai.sin_addr.s_addr = INADDR_ANY;
        sai.sin_port        = htons(config->tcp_port);

        if ((rc = bind(fd_socket, (struct sockaddr *) &sai, sizeof (sai))) < 0)
        {
            aapl_fail(aapl, __func__, __LINE__, "Cannot bind() AF_INET socket for clients on port %d: %s.\n", config->tcp_port, aapl_tcp_strerr());
            goto cleanup_and_exit;
        }

        if ((rc = listen(fd_socket, /* backlog = */ 1)) < 0)
        {
            aapl_fail(aapl, __func__, __LINE__, "Cannot listen() for command client on port %d: %s.\n", config->tcp_port, aapl_tcp_strerr());
            goto cleanup_and_exit;
        }

        if (!config->ats_server_host && config->ats_server_host_port)
        {
            int queries;
            char buffer[128];
            memset(&sai_ts, 0, sizeof(sai_ts));
            sai_ts.sin_family      = AF_INET;
            sai_ts.sin_addr.s_addr = INADDR_ANY;
            sai_ts.sin_port        = htons(config->ats_server_host_port);

            if ((rc = bind(fd_socket_ts, (struct sockaddr *) &sai_ts, sizeof (sai_ts))) < 0)
            {
                aapl_fail(aapl, __func__, __LINE__, "Cannot bind() AF_INET socket for clients on port %d: %s.\n", config->ats_server_host_port, aapl_tcp_strerr());
                goto cleanup_and_exit;
            }

            if ((rc = listen(fd_socket_ts, /* backlog = */ 1)) < 0)
            {
                aapl_fail(aapl, __func__, __LINE__, "Cannot listen() for command client on port %d: %s.\n", config->ats_server_host_port, aapl_tcp_strerr());
                goto cleanup_and_exit;
            }
            aapl_log_printf(aapl, AVAGO_INFO, __func__, __LINE__, "AAPL ATS server version %s is now listening for TCP connections on port %d...\n", AAPL_VERSION, config->ats_server_host_port);

            aapl->socket = accept(fd_socket_ts, (struct sockaddr *) &client_IPaddr_ts, &addr_len_ts);
            aapl->disable_reconnect = 1;

            aapl->aacs_server = (char *) aapl_realloc(aapl, aapl->aacs_server, 32, __func__);
            if( aapl->aacs_server )
                strncpy(aapl->aacs_server, "(remote AAPL ATS server)", 32);
            aapl->tcp_port = 0;
            strncpy(aapl->data_char, "", 3);

            aapl_log_printf(aapl, AVAGO_INFO, __func__, __LINE__, "AAPL ATS server is now connected on port %d.\n", config->ats_server_host_port);

            snprintf(buffer, 128, "AAPL AACS server %s in ATS mode", AAPL_VERSION);
            avago_aacs_send_command(aapl, buffer);
            avago_aacs_send_command_options(aapl, "ok", 1, 10);
            queries = aapl->data;
            strncpy(buffer, HELP_STR, 127);
            if (queries >= 2) avago_aacs_send_command(aapl, buffer);
            if (queries >= 3) avago_aacs_send_command(aapl, "0");

            aapl_check_capabilities(aapl);
            aapl_get_ip_info(aapl, 0);
            aapl_close_connection(aapl);
        }
        aapl_log_printf(aapl, AVAGO_INFO, __func__, __LINE__, "AAPL AACS server version %s is now listening for TCP connections on port %d...\n", AAPL_VERSION, config->tcp_port);
    }

    update_server_status(aapl, &chip_num, &ring_num);


    while( !error_or_exit )
    {
        if (config->ats_server_host)
        {
            aapl_connect(aapl_ats_server_host, config->ats_server_host, config->ats_server_host_port);
            aapl_server->socket = aapl_ats_server_host->socket;
            if( aapl_server->socket < 0 ) aapl_fail(aapl, __func__, __LINE__, "Cannot connect to remote ATS server %s:%d.\n", config->ats_server_host, config->ats_server_host_port, aapl_tcp_strerr());
        }
        else
        {
            aapl_server->socket = accept(fd_socket, (struct sockaddr *) &client_IPaddr, &addr_len);
            if( aapl_server->socket < 0 ) aapl_fail(aapl, __func__, __LINE__, "Cannot accept() for command client on port %d: %s.\n", config->tcp_port, aapl_tcp_strerr());
        }

        if( aapl_server->socket < 0 )
        {
            error_or_exit = 1;
            break;
        }

        if (config->ats_server_host) aapl_log_printf(aapl, AVAGO_INFO, __func__, __LINE__, "Connection to remote AACS ATS server %s:%d (socket %d) successful.\n", config->ats_server_host, config->ats_server_host_port, aapl_server->socket);
        else
        {
            char buf[64];
            buf[0] = 0;
# ifdef AAPL_LOG_TIME_STAMPS
            if( aapl->log_time_stamps ) aapl_local_strftime(buf, 10, "%H:%M:%S");
# endif
            aapl_log_printf(aapl, AVAGO_INFO, 0, 0, "%10s %s:%d (sk %d) @ %s opened new connection\n", "", inet_ntoa(client_IPaddr.sin_addr), config->tcp_port, aapl_server->socket, buf);
        }

        if (!(config->ats_server_host || config->ats_server_host_port))
        {
            return_code = aapl->return_code;
            aapl_connect(aapl, 0, 0);
            if( return_code != aapl->return_code )
            {
                aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "aapl_connect failed.\n");
                break;
            }
        }

#       if 0
        return_code = aapl->return_code;
        aapl_get_ip_info(aapl, /* chip_reset */ 0);
        if( return_code != aapl_get_return_code(aapl) )
        {
            aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "aapl_get_ip_info returned a negative value.\n");
            break;
        }
#       endif


        for(;;)
        {
            char *cmd;
            int read_len = 0;
            int send_len = 0;
            char *result;
            size_t res_len;

            read_len = aapl_recv(aapl_server);
            cmd = aapl_server->data_char;

            if (read_len < 0)
            {
                #ifdef WIN32
                if( WSAGetLastError() == WSAECONNRESET)
                #else
                if( errno == ECONNRESET )
                #endif
                     aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "could not read from socket %d on port %d (%d: %s)\n", aapl->socket, config->tcp_port, read_len, aapl_tcp_strerr());
                else
                {
                    aapl_fail(aapl, __func__, __LINE__, "could not read from socket %d on port %d (%d: %s).\n", aapl->socket, config->tcp_port, read_len, aapl_tcp_strerr());
                    error_or_exit = 1;
                }
                break;
            }

            if( 0 == read_len )
            {
                aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Server got EOF on port %d.\n", config->tcp_port);
                break;
            }

            if ((read_len >= 1) && (cmd[read_len - 1] == '\n')) --read_len;
            if ((read_len >= 1) && (cmd[read_len - 1] == '\r')) --read_len;
            cmd[read_len] = '\0';

            if (!config->ats_server_host && config->ats_server_host_port && aapl->socket < 0)
            {
                aapl_close_connection(aapl);
                aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "AAPL ATS server lost connection with client. Waiting for new connection on port %d...\n", config->ats_server_host_port);
                aapl->socket = accept(fd_socket_ts, (struct sockaddr *) &client_IPaddr_ts, &addr_len_ts); if (aapl_connection_status(aapl) < 0)
                {
                    aapl_close_connection(aapl);
                    aapl->socket = accept(fd_socket_ts, (struct sockaddr *) &client_IPaddr_ts, &addr_len_ts);
                }
                aapl_log_printf(aapl, AVAGO_INFO, __func__, __LINE__, "AAPL ATS server is now connected on port %d (socket %d).\n", config->ats_server_host_port, aapl->socket);
                aapl_get_ip_info(aapl, 0);
            }

            result = avago_aacs_process_cmd(aapl, cmd, &chip_num, &ring_num);
            if( !result ) continue;

            aapl_log_printf(aapl, AVAGO_DEBUG2, __func__, __LINE__, "%s => %s\n", cmd, result);

            res_len = strlen(result);
            result[res_len++] = '\n';

            send_len = aapl_send(aapl_server, result, res_len, config->tcp_port);
            if (send_len < 0) {error_or_exit = 1; break;}

            if( 0 == strncmp(result, RES_CLOSE, 5) )
                break;
            if( 0 == strncmp(result, RES_EXIT, 4) )
            {
                error_or_exit = 1;
                return_status = 1;
                break;
            }
        }

        if (!config->keep_connection_open) aapl_close_connection(aapl);
        if (config->ats_server_host) aapl_close_connection(aapl_ats_server_host);
        else
        {
            char buf[64];
            buf[0] = 0;
# ifdef AAPL_LOG_TIME_STAMPS
            if( aapl->log_time_stamps ) aapl_local_strftime(buf, 10, "%H:%M:%S");
# endif
            aapl_log_printf(aapl, AVAGO_INFO, 0, 0, "%10s@%s:%d (sk %d) @ %s closed connection\n", aapl_client_username, inet_ntoa(client_IPaddr.sin_addr), config->tcp_port, aapl_server->socket, buf);
#if AAPL_ENABLE_PS2 && AAPL_ALLOW_SYSTEM_I2C && AAPL_ENABLE_FLOAT_USAGE && defined I2C_SMBUS_READ
            avago_system_i2c_close_fn(aapl);
#endif
            client_IPaddr.sin_addr.s_addr = 0;
            if (aapl_server->socket >= 0)
            {
                closesocket(aapl_server->socket);
                aapl_server->socket = -1;
            }
        }
        update_server_status(aapl, &chip_num, &ring_num);
    }

cleanup_and_exit:
#if AAPL_ENABLE_CONSOLE
    if (config->enable_console_mode) console_teardown(aapl);
#endif

    if (fd_socket >= 0) closesocket(fd_socket);
    aapl_destruct(aapl_server);
    return return_status;

}

/** @brief   Starts an AACS Server and listens for requests.
 ** @details Starts (and restarts after closure) an AACS server that accepts one TCP/IP
 ** connection on tcp_port, receives zero or more AACS commands, forwards them
 ** to avago_sbus_wr(), etc, and returns response text to the command client.
 **
 ** The caller must first:
 **
 ** - Open a lower-level (back end) connection or whatever else is required for
 **   avago_sbus_wr(), etc to function, such as by using aapl_connect() (but not
 **   necessarily this function in the end user context).
 **
 ** - Start a process or thread that's taken over by this function, which only
 **   returns upon "exit" or internal error.
 **
 ** Note:  Unlike the HW server (HS1, etc), this process does not support
 ** command batching.  It expects just one command => response at a time.
 **
 ** @return Returns FALSE for internal error logged, or TRUE for client "exit" command.
 ** Does not return for client command anomalies or "close" commands.
 **/
int avago_aacs_server(
    Aapl_t *aapl,       /**< [in] Pointer to Aapl_t structure. */
    int tcp_port)       /**< [in] Port of remote to connect to. */
{
    Avago_aacs_server_config_t config;
    memset(&config, 0, sizeof(config));
    config.tcp_port = tcp_port;
    return avago_aacs_server_options(aapl, &config);
}

#if AAPL_ALLOW_THREAD_SUPPORT && AAPL_ENABLE_PS2
#if AAPL_ENABLE_CONSOLE
static void *aapl_mouse_thread(void *server_cntl_in)
{
    Aapl_server_cntl_t *server_cntl = (Aapl_server_cntl_t *) server_cntl_in;
    Aapl_t *aapl = server_cntl->aapl_downstream;
    sleep(1);
    while(1)
    {
        int c = getch();
        if (aapl->debug) wprintw(stdout_window, " #1 %x\n", c);
        if (c == KEY_MOUSE)
        {
            MEVENT event;
            if (getmouse(&event) == OK)
            {
                if (aapl->debug) wprintw(stdout_window, " #3 %x\n", event.bstate);
                if (event.bstate == 4)
                {
                    system("sudo -u pi xterm /home/pi/bin/update_ps2 > /dev/null 2>&1 ");
                }
            }
        }
        update_windows();
        ms_sleep(400);
    }
    return NULL;
}
#endif

static void aapl_copy(Aapl_t *aapl1, Aapl_t *aapl2)
{
    #define AAPL_COPY_MEMBER(x) aapl2->x = aapl1->x;
    #define AAPL_COPY_MEMBERS(x) memcpy(aapl2->x, aapl1->x, sizeof(*(aapl2->x)));
    AAPL_COPY_MEMBER(chip_name[0])
    AAPL_COPY_MEMBER(chip_rev[0])
    AAPL_COPY_MEMBER(tck_delay)
    AAPL_COPY_MEMBER(debug)
    AAPL_COPY_MEMBER(verbose)
    AAPL_COPY_MEMBER(suppress_errors)
    AAPL_COPY_MEMBER(upgrade_warnings)
    AAPL_COPY_MEMBER(ansi_colors)
    AAPL_COPY_MEMBER(spico_int_only)
    AAPL_COPY_MEMBER(serdes_int_timeout)
    AAPL_COPY_MEMBER(sbus_mdio_timeout)
    AAPL_COPY_MEMBER(tck_delay)
    AAPL_COPY_MEMBER(communication_method)
    AAPL_COPY_MEMBER(chips)
    AAPL_COPY_MEMBERS(sbus_rings)
    AAPL_COPY_MEMBER(max_cmds_buffered)
    AAPL_COPY_MEMBER(capabilities)
    AAPL_COPY_MEMBER(tap_gen)
    AAPL_COPY_MEMBER(sbus_commands)
    AAPL_COPY_MEMBER(commands)
    AAPL_COPY_MEMBER(errors)
    AAPL_COPY_MEMBER(warnings)
    AAPL_COPY_MEMBER(curr_chip)
}

void *aapl_telnet_thread(void *server_cntl_in)
{
    int rc = 1;
    int fd_socket;
    struct sockaddr_in sai;
    int tcp_port = 23;

    Aapl_server_cntl_t *server_cntl = (Aapl_server_cntl_t *) server_cntl_in;
    Aapl_t *aapl = aapl_construct();


    struct sockaddr_in telnet_IPaddr;
    socklen_t addr_len = sizeof(telnet_IPaddr);

#if AAPL_ENABLE_CONSOLE
    if (server_cntl->enable_console_mode)
    {
        ms_sleep(500);
        aapl->enable_stream_logging = 0; /* don't send anything to stdout */
        aapl->enable_stream_err_logging = 0;
        aapl_register_logging_fn(aapl, console_logging_fn, 0, 0);
    }
#endif

    # ifdef WIN32
    {
        WSADATA wsaData;
        WORD vers_req = MAKEWORD(2,2);
        WSAStartup(vers_req,&wsaData);
    }
    # endif

    fd_socket = socket(PF_INET, SOCK_STREAM, 0);
    if( fd_socket < 0 )
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot create PF_INET socket: %s.\n", aapl_tcp_strerr());
        return 0;
    }

    rc = setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &rc, sizeof(rc));

    rc = setsockopt(fd_socket, SOL_SOCKET, SO_KEEPALIVE, (char *) &rc, sizeof(rc));

    if( rc < 0 )
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot setsockopt(SO_REUSEADDR) on socket %d: %s.\n", fd_socket, aapl_tcp_strerr());
        goto cleanup_and_exit;
    }

    sai.sin_family      = AF_INET;
    sai.sin_addr.s_addr = INADDR_ANY;
    sai.sin_port        = htons(tcp_port);

    if ((rc = bind(fd_socket, (struct sockaddr *) &sai, sizeof (sai))) < 0)
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot bind() AF_INET socket for clients on port %d: %s.\n", tcp_port, aapl_tcp_strerr());
        goto cleanup_and_exit;
    }

    if ((rc = listen(fd_socket, /* backlog = */ 1)) < 0)
    {
        aapl_fail(aapl, __func__, __LINE__, "Cannot listen() for command client on port %d: %s.\n", tcp_port, aapl_tcp_strerr());
        goto cleanup_and_exit;
    }
    aapl_log_printf(aapl, AVAGO_INFO, __func__, __LINE__, "AAPL telnet server version %s is now listening for TCP connections on port %d...\n", AAPL_VERSION, tcp_port);
    fflush(stderr);


    while (1)
    {
        int send_len;
        int size = 1024;
        char *data = (char *) aapl_malloc(aapl, size, __func__);
        char *data_end = data;
        char *build_info = aapl_build_info(2);

        aapl->socket = accept(fd_socket, (struct sockaddr *) &telnet_IPaddr, &addr_len);
        if( aapl->socket < 0 )
        {
            aapl_fail(aapl, __func__, __LINE__, "Cannot accept() for command client on port %d: %s.\n", tcp_port, aapl_tcp_strerr());
            break;
        }
        aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Connection from %s:%d (socket %d).\n", inet_ntoa(telnet_IPaddr.sin_addr), tcp_port, aapl->socket);

        aapl_buf_add(aapl, &data, &data_end, &size, "===============================================================================\n");
        aapl_buf_add(aapl, &data, &data_end, &size, "AAPL Telnet Server. AAPL version %s\n\n", build_info);
        AAPL_FREE(build_info);
        aapl_buf_add(aapl, &data, &data_end, &size, "close, c, q            close this telnet connection\n");
        aapl_buf_add(aapl, &data, &data_end, &size, "k                      Kill any current AACS server connections\n");
        aapl_buf_add(aapl, &data, &data_end, &size, "lock, y                Kill any connections, then lock.\n");
        aapl_buf_add(aapl, &data, &data_end, &size, "unlock, u, t           Unlock PS2\n");
        aapl_buf_add(aapl, &data, &data_end, &size, "reboot, r              Reboot PS2\n");
        aapl_buf_add(aapl, &data, &data_end, &size, "status, s              Display PS2 status information.\n");
        aapl_buf_add(aapl, &data, &data_end, &size, "web                    Update PS2's server_status.html file.\n");
#if AAPL_ENABLE_PS2
        aapl_buf_add(aapl, &data, &data_end, &size, "update                 Update PS2's bin directory via 'git pull'\n");
#endif
        aapl_buf_add(aapl, &data, &data_end, &size, "exit                   Terminate AACS server (will normally restart)\n");
        aapl_buf_add(aapl, &data, &data_end, &size, "===============================================================================\n");
        send_len = aapl_send(aapl, data, strlen(data), tcp_port);
        if (send_len < 0) break;

        for(;;)
        {
            char *cmd;
            int read_len;
            BOOL update_status = FALSE;

            data_end = data;
            aapl_buf_add(aapl, &data, &data_end, &size, "\nPS2] ");
            send_len = aapl_send(aapl, data, strlen(data), tcp_port);
            if (send_len < 0) break;

            read_len = aapl_recv(aapl);
            cmd = aapl->data_char;

            if (read_len < 0)
            {
                #ifdef WIN32
                if( WSAGetLastError() == WSAECONNRESET)
                #else
                if( errno == ECONNRESET )
                #endif
                     aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "could not read from socket %d on port %d (%d: %s)\n", aapl->socket, tcp_port, read_len, aapl_tcp_strerr());
                else
                {
                    aapl_fail(aapl, __func__, __LINE__, "could not read from socket %d on port %d (%d: %s).\n", aapl->socket, tcp_port, read_len, aapl_tcp_strerr());
                }
                break;
            }

            if( 0 == read_len )
            {
                aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, "Server got EOF on port %d.\n", tcp_port);
                break;
            }

            if ((read_len >= 1) && (cmd[read_len - 1] == '\n')) --read_len;
            if ((read_len >= 1) && (cmd[read_len - 1] == '\r')) --read_len;
            cmd[read_len] = '\0';

            if (cmd[0] == (char)0xff && strstr(cmd, "#")) cmd = strstr(cmd, "#") + 1;
            aapl_log_printf(aapl, AVAGO_DEBUG1, __func__, __LINE__, " %d on port %d (%d: %s)\n", aapl->socket, tcp_port, read_len, aapl_tcp_strerr());

            data[0] = 0;
            if      (strstr(cmd, "close") ||
                    (strlen(cmd) == 1 && cmd[0] == 'c') ||
                    (strlen(cmd) == 1 && cmd[0] == 'q') ) break;
            else if (strstr(cmd, "status")  || (strlen(cmd) == 1 && cmd[0] == 's'))
            {
                aapl_copy(server_cntl->aapl_downstream, aapl);

                status_command(aapl, "status", "", &data);
                aapl_str_rep(data, ';', 0x0a);
            }
            else if (strstr(cmd, "web"))
            {
                update_status = TRUE;
            }
            else if (strstr(cmd, "unlock") ||
                    (strlen(cmd) == 1 && cmd[0] == 'u') ||
                    (strlen(cmd) == 1 && cmd[0] == 't') )
            {
                client_IPaddr_lock[0].sin_addr.s_addr = 0;
                client_IPaddr_lock[1].sin_addr.s_addr = 0;
                snprintf(data, 128, "Unlocked.\n");
                update_status = TRUE;
            }
            else if (strstr(cmd, "lock") ||
                    (cmd[0] == 'y'))
            {
                snprintf(data, 128, "Closing AAPL AACS server socket %d.\n", server_cntl->aapl_upstream->socket);
                shutdown(server_cntl->aapl_upstream->socket, SHUT_RDWR);
                closesocket(server_cntl->aapl_upstream->socket);
                server_cntl->aapl_upstream->socket = -1;
                send_len = aapl_send(aapl, data, strlen(data), tcp_port);
                if (send_len < 0) break;

                client_IPaddr_lock[1].sin_addr.s_addr = client_IPaddr_lock[0].sin_addr.s_addr;
                if (strlen(cmd) == 1 || strlen(cmd) == 4)
                    client_IPaddr_lock[0].sin_addr.s_addr = telnet_IPaddr.sin_addr.s_addr;
                else
                {
                    struct hostent *server = NULL;
                    if (strstr(cmd, " ")) server = gethostbyname(strstr(cmd, " ")+1);
                    if (server) memmove((char *)&client_IPaddr_lock[0].sin_addr.s_addr, (char *)server->h_addr_list[0], server->h_length);
                    else
                    {
                        snprintf(data, 128, "Not able to determine IP address from \"%s\".\n", strstr(cmd, " ") ? strstr(cmd, " ")+1 : "");
                        send_len = aapl_send(aapl, data, strlen(data), tcp_port);
                        if (send_len < 0) break;
                    }
                }
                snprintf(data, 128, "Locked to %s.\n", inet_ntoa(client_IPaddr_lock[0].sin_addr));
                update_status = TRUE;
            }
            else if (strstr(cmd, "reboot") ||
                    (strlen(cmd) == 1 && cmd[0] == 'r'))
            {
                snprintf(data, 128, "Rebooting now.\n");
                send_len = aapl_send(aapl, data, strlen(data), tcp_port);
                if (send_len < 0) break;

                aapl->destroy_thread = 1;
                aapl_destruct(server_cntl->aapl_upstream);
                aapl_destruct(server_cntl->aapl_downstream);
                aapl_destruct(aapl);
                system("sudo shutdown -r now");
                exit(0);
            }
            else if (strlen(cmd) == 1 && cmd[0] == 'k')
            {
                snprintf(data, 128, "Closing AAPL AACS server socket %d.\n", server_cntl->aapl_upstream->socket);
                shutdown(server_cntl->aapl_upstream->socket, SHUT_RDWR);
                closesocket(server_cntl->aapl_upstream->socket);
                server_cntl->aapl_upstream->socket = -1;
            }
#if AAPL_ENABLE_PS2 && AAPL_ENABLE_FILE_IO
            else if (strstr(cmd, "update"))
            {
                char buf[126];
                run_system_cmd("sudo -u pi git -C /home/pi/bin pull ", buf, AAPL_ARRAY_LENGTH(buf));
                snprintf(data, 128, "%s\n", buf);
            }
#endif
            else if (strstr(cmd, "exit"))
            {
                snprintf(data, 128, "Exiting.\n");
                send_len = aapl_send(aapl, data, strlen(data), tcp_port);
                if (send_len < 0) break;

                aapl->destroy_thread = 1;
                aapl_destruct(server_cntl->aapl_upstream);
                aapl_destruct(server_cntl->aapl_downstream);
                aapl_destruct(aapl);
                exit(0);
            }
            else snprintf(data, 128, "Unknown command received: %s (%d)\n", cmd, (int)strlen(cmd));

            if( update_status )
            {
                int x = 0;
                aapl_copy(server_cntl->aapl_downstream, aapl);

                update_server_status(aapl, &x, &x);
                snprintf(data, 128, "server_status.htm updated.\n");
            }

#if AAPL_ENABLE_CONSOLE
            if (server_cntl->enable_console_mode)
                console_logging_fn(server_cntl->aapl_downstream, AVAGO_INFO, "update AAPL console log", 0);
#endif
            send_len = aapl_send(aapl, data, strlen(data), tcp_port);
            if (send_len < 0) break;
        }
        if (aapl->socket >= 0) closesocket(aapl->socket);
    }
cleanup_and_exit:
    if (fd_socket >= 0) closesocket(fd_socket);
    aapl_destruct(aapl);
    return NULL;
}
#endif

#endif
