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
 * Broadcom System Log FIle Sink
 */

#include <sal/core/libc.h> 
#include <sal/core/spl.h> 
#include <sal/core/alloc.h> 
#include <sal/core/dpc.h> 
#include <sal/appl/io.h> 
#include <appl/diag/bslsink.h>
#include <appl/diag/bslfile.h>

#ifndef NO_FILEIO

#include <stdio.h>

static FILE *file_fp = NULL;
static char *file_nm = NULL;

static bslsink_sink_t logfile_sink;

STATIC void
bslfile_dpc(void *a1, void *a2, void *a3, void *a4, void *a5)
{
    char *fmt = (char *)a1;
    
    if (file_fp) {
	sal_fprintf(file_fp, fmt, a2, a3, a4, a5);
	sal_fflush(file_fp);
    }
}

STATIC int
bslfile_vfprintf(void *file, const char *format, va_list args)
{
    int	retv = 0;

    if (file_fp) {
	if (sal_int_context()) {
	    void *a1, *a2, *a3, *a4;
	    a1 = va_arg(args, void *);
	    a2 = va_arg(args, void *);
	    a3 = va_arg(args, void *);
	    a4 = va_arg(args, void *);
	    /* Passing format in lieu of fake owner to avoid wasting an arg */
	    retv = sal_dpc(bslfile_dpc, (void *)format, a1, a2, a3, a4);
	} else {
	    retv = sal_vfprintf(file_fp, format, args);
	    sal_fflush(file_fp);
	}
    }
    return retv;
}

STATIC int
bslfile_check(bsl_meta_t *meta)
{
    return (file_fp != NULL);
}

STATIC int
bslfile_cleanup(struct bslsink_sink_s *sink)
{
    if (file_nm) {
	bslfile_close();
    }
    return 0;
}

#endif /* NO_FILEIO */

int
bslfile_init(void)
{
#ifndef NO_FILEIO
    bslsink_sink_t *sink;

    /* Create logfile sink */
    sink = &logfile_sink;
    bslsink_sink_t_init(sink);
    sal_strncpy(sink->name, "logfile", sizeof(sink->name));
    sink->vfprintf = bslfile_vfprintf;
    sink->check = bslfile_check;
    sink->cleanup = bslfile_cleanup;
    sink->enable_range.min = bslSeverityOff+1;
    sink->enable_range.max = bslSeverityCount-1;

    /* Configure log prefix */
    sal_strncpy(sink->prefix_format, "%u:%F: ",
                sizeof(sink->prefix_format));
    sink->prefix_range.min = bslSeverityOff+1;
    sink->prefix_range.max = bslSeverityWarn;

    bslsink_sink_add(sink);
#endif

    return 0;
}

char *
bslfile_name(void)
{
#ifdef NO_FILEIO
    return "<no filesystem>";
#else
    return(file_nm);
#endif
}

int
bslfile_close(void)
{
#ifndef NO_FILEIO
    if (file_nm) {
        sal_free(file_nm);
        file_nm = NULL;
    }
    if (file_fp) {
	sal_fclose(file_fp);
	file_fp = 0;
    }
#endif
    return 0;
}

int
bslfile_open(char *filename, int append)
{
#ifndef NO_FILEIO
    if (file_nm) {
	bslfile_close();
    }
    if ((file_fp = sal_fopen(filename, append ? "a" : "w")) == 0) {
        sal_printf("bslfile: File open error\n");
	return -1;
    }
    file_nm = sal_strdup(filename);
    if (file_nm == NULL) {
	sal_fclose(file_fp);
	file_fp = 0;
        sal_printf("bslfile: strdup failed\n");
        return -1;
    }
#endif /* NO_FILEIO */
    return 0;
}

int
bslfile_is_enabled(void)
{
#ifndef NO_FILEIO
    if (file_fp != NULL) {
        return 1;
    }
#endif
    return 0;
}

int
bslfile_enable(int enable)
{
    int cur_enable = bslfile_is_enabled();
#ifndef NO_FILEIO
    if (file_fp == NULL && enable) {
	if (file_nm == NULL) {
	    sal_printf("bslfile: No log file\n");
	    return -1;
	}
	if ((file_fp = sal_fopen(file_nm, "a")) == 0) {
	    sal_printf("bslfile: File open error\n");
	    return -1;
	}
    }

    if (file_fp != NULL && !enable) {
        sal_fclose(file_fp);
        file_fp = NULL;
    }
#endif
    return cur_enable;
}
