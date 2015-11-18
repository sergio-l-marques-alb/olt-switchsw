/*
 * $Id: $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * Broadcom System Log Trace Buffer Sink
 */
#include <soc/drv.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <sal/appl/io.h>
#include <appl/diag/bslsink.h>
#include <appl/diag/bslenable.h>

/*
 * BSL copies the trace string to the buffer in two steps.
 * First the prefix string is copied, then the log string.
 * So each trace string uses two trace buffer entries.
 * Using a short entry length and double the max entries.
 */
#define MAX_NUM_TRACE_ENTRIES 1000
#define MAX_TRACE_ENTRY_LEN 64

typedef struct bsl_trace_buf_s {
    char        * buffer_start;
    int           buffer_size;
    int           cur_num_entries;
    int           max_num_entries;
    char        * head;
    sal_mutex_t   lock;
} bsl_trace_buf_t;

static bsl_trace_buf_t trace;
static bslsink_sink_t trace_sink;

/*
 * Note that inserting trace messages during an interrupt is
 * only allowed in cases where the interrupt handling is done
 * at thread level.
 */
STATIC int bsltrace_add(const char *format, va_list args)
{
    int str_len = 0;

    /* Lock access to trace buf */
    sal_mutex_take(trace.lock, sal_mutex_FOREVER);

    /* Add string to head of trace buffer */
    str_len = sal_vsprintf(trace.head, format, args);

    if (str_len > MAX_TRACE_ENTRY_LEN) {
        /* Handle Error */
    } else if (str_len < 0) {
        /* Handle Error */
    } else {
        /* Increment head*/
        trace.head = (char *)(trace.head) + MAX_TRACE_ENTRY_LEN;

        /* Handle wrap */
        if (trace.head == (trace.buffer_start + trace.buffer_size) ) {
            trace.head = trace.buffer_start;
        }

        /* Count up to max number of entries until full,
           then simply overwrite the oldest */
        if (trace.cur_num_entries < trace.max_num_entries) {
            trace.cur_num_entries++;
        }
    }

    /* Unlock access to trace buf */
    sal_mutex_give(trace.lock);

    return str_len;
}

STATIC int
bsltrace_vfprintf(void *file, const char *format, va_list args)
{
    /* Need to print to circular trace buffer */
    return bsltrace_add(format, args);
}

STATIC int
bsltrace_cleanup(bslsink_sink_t * sink)
{

    /* Free allocated trace buffer */
    if (trace.buffer_start) {
        sal_free(trace.buffer_start);
    }

    return 0;
}

int bsltrace_print(void)
{
    char * entry = NULL;
    int i;

    if (!trace.lock) {
        sal_printf("Trace Buffer Not Initialized\n");
        return 0;
    }

    /* Lock access to trace buf */
    sal_mutex_take(trace.lock, sal_mutex_FOREVER);

    if (trace.cur_num_entries == 0) {
        sal_printf("Empty Trace Buffer\n");

        /* Unlock access to trace buf */
        sal_mutex_give(trace.lock);

        return 0;
    } else if (trace.cur_num_entries < trace.max_num_entries) {
        entry = trace.buffer_start;
    }
    else {
        entry = trace.head;
    }

    /* Print trace oldest to newest */
    for (i = 0; i < trace.cur_num_entries; i++) {
        sal_printf("%s", entry);
        entry += MAX_TRACE_ENTRY_LEN;
        if (entry == (trace.buffer_start + trace.buffer_size)) {
            entry = trace.buffer_start;
        }
    }

    /* Unlock access to trace buf */
    sal_mutex_give(trace.lock);

    return 0;
}

int
bsltrace_init(void)
{

    bslsink_sink_t *sink;

    if (!soc_property_get(0, "tracesink", 1)) {
        return 0;
    }

    /* Create mutex lock to protect buffer access */
    if (!trace.lock) {
        trace.lock = sal_mutex_create("trace_lock");
    }

    /* Alocate and initialize trace buffer */
    trace.buffer_start =
        sal_alloc(MAX_TRACE_ENTRY_LEN * MAX_NUM_TRACE_ENTRIES, "bsltrace");

    if (trace.buffer_start == NULL) {
        return 1;
    }
    sal_memset(trace.buffer_start,
               0,
               MAX_TRACE_ENTRY_LEN * MAX_NUM_TRACE_ENTRIES);

    /* Initialize the trace buffer management info */
    trace.head = trace.buffer_start;
    trace.buffer_size = MAX_TRACE_ENTRY_LEN * MAX_NUM_TRACE_ENTRIES;
    trace.cur_num_entries = 0;
    trace.max_num_entries = MAX_NUM_TRACE_ENTRIES;

    /* Create and configure the trace sink */
    sink = &trace_sink;
    bslsink_sink_t_init(sink);
    sal_strncpy(sink->name, "trace", sizeof(sink->name));
    sink->vfprintf = bsltrace_vfprintf;
    sink->cleanup = bsltrace_cleanup;
    sink->enable_range.min = bslSeverityDebug;
    sink->enable_range.max = bslSeverityDebug;

    /* Configure the trace prefix */
    sal_strncpy(sink->prefix_format, "%u:%L%S%s (%P:%T):",
                sizeof(sink->prefix_format));
    sink->prefix_range.min = bslSeverityDebug;
    sink->prefix_range.max = bslSeverityDebug;

    bslsink_sink_add(sink);

    /*
     * To debug a specific issue related to the RX thread, we must:
     * 1. Modify the console sink enable levels to filter out info and debug level
     *    msgs because we are going to enable debug output for a few layer/sources.
     * 2. Turn on debug level for Trace sources used in RX, LINK, and INTR sources.
     */
    sink = bslsink_sink_find("console");
    sink->enable_range.max = bslSeverityVerbose;

    sal_printf("WARNING: BSL console sink's max enable range is temporarily set to Verbose\n");
    sal_printf("    Use shell command 'deb sink console emax=debug' to allow debug to the console\n");

    /* Turn on debug so that trace sink can catch it */
    bslmgmt_set(bslLayerBcm, bslSourceTrace, bslSeverityDebug);
    bslmgmt_set(bslLayerSoc, bslSourceTrace, bslSeverityDebug);

    return 0;
}
