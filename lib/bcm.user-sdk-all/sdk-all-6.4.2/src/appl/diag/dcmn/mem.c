/*
 * $Id: mem.c,v 1.39 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * File:       mem.c
 * Purpose:    Diag shell memory commands for DPP
 */

#include <shared/bsl.h>
#include <ibde.h>

#include <sal/appl/pci.h>

#include <soc/mem.h>
#include <soc/cm.h>

#ifdef BCM_PETRAB_SUPPORT
#include <soc/dpp/drv.h>
#endif
#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/arad_sw_db.h>
#endif

#ifdef BCM_DPP_SUPPORT
#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/stack.h>
#include <bcm_int/dpp/trunk.h>
#include <bcm_int/dpp/counters.h>
#endif

#include <appl/diag/system.h>
#include <appl/diag/bslcons.h>
#include <appl/diag/bslfile.h>


#define DUMP_TABLE_RAW          0x01
#define DUMP_TABLE_HEX          0x02
#define DUMP_TABLE_ALL          0x04
#define DUMP_TABLE_CHANGED      0x08
#define DUMP_DISABLE_CACHE      0x10

extern int dpp_all_reg_get(int unit, int is_debug);
extern  int     mem_test_default_init(int, soc_mem_t mem, void **);
extern  int     mem_test_rw_init(int, soc_mem_t mem, void **);
extern  int     mem_test(int, args_t *, void *);
extern  int     mem_test_done(int, void *);
extern void tr8_reset_bits_counter(void);
extern void tr8_increment_bits_counter(uint32 bits_num);
extern uint32 tr8_bits_counter_get(void);
extern uint32 tr8_get_bits_num(uint32 number);
extern void tr8_write_dump(const char * _Format);

void memtest_mask_get(int unit, soc_mem_t mem, uint32 *mask);
void memtest_mask_get_tr8(int unit, soc_mem_t mem, uint32 *mask);
int dpp_mem_dump_iter_callback(int unit, soc_mem_t mem, void *data);
STATIC cmd_result_t do_dump_table_single(int unit, soc_mem_t mem, int copyno, int index, int count, int flags);
STATIC cmd_result_t dpp_do_dump_table_field(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index, int count, int flags, char *field);
void soc_mem_entry_dump_field(int unit, soc_mem_t mem, void *buf, char* field_name);

/*
 * Utility routine to concatenate the first argument ("first"), with
 * the remaining arguments, with commas separating them.
 */

STATIC void
collect_comma_args(args_t *a, char *valstr, char *first)
{
    char *s;

    strcpy(valstr, first);

    while ((s = ARG_GET(a)) != 0) {
        strcat(valstr, ",");
        strcat(valstr, s);
    }
}

STATIC void
check_global(int unit, soc_mem_t mem, char *s, int *is_global)
{
    soc_field_info_t    *fld;
    soc_mem_info_t      *m = &SOC_MEM_INFO(unit, mem);
    char                *eqpos;

    eqpos = strchr(s, '=');
    if (eqpos != NULL) {
        *eqpos++ = 0;
    }
    for (fld = &m->fields[0]; fld < &m->fields[m->nFields]; fld++) {
        if (!sal_strcasecmp(s, SOC_FIELD_NAME(unit, fld->field)) &&
            (fld->flags & SOCF_GLOBAL)) {
            break;
        }
    }
    if (fld == &m->fields[m->nFields]) {
        *is_global = 0;
    } else {
        *is_global = 1;
    }
}

STATIC int
collect_comma_args_with_view(args_t *a, char *valstr, char *first,
                             char *view, int unit, soc_mem_t mem)
{
    char *s, *s_copy = NULL, *f_copy = NULL;
    int is_global, rv = 0;

    if ((f_copy = sal_alloc(strlen(first) + 1, "first")) == NULL) {
        cli_out("cmd_dpp_mem_write : Out of memory\n");
        rv = -1;
        goto done;
    }
    memset(f_copy, 0, strlen(first) + 1);
    sal_strcpy(f_copy, first);

    /* Check if field is global before applying view prefix */
    check_global(unit, mem, f_copy, &is_global);
    if (!is_global) {
        sal_strcpy(valstr, view);
        strcat(valstr, first);
    } else {
        sal_strcpy(valstr, first);
    }

    while ((s = ARG_GET(a)) != 0) {
        if ((s_copy = sal_alloc(strlen(s) + 1, "s_copy")) == NULL) {
            cli_out("cmd_dpp_mem_write : Out of memory\n");
            rv = -1;
            goto done;
        }
        memset(s_copy, 0, strlen(s) + 1);
        sal_strcpy(s_copy, s);
        check_global(unit, mem, s_copy, &is_global);
        sal_free(s_copy);
        strcat(valstr, ",");
        if (!is_global) {
            strcat(valstr, view);
            strcat(valstr, s);
        } else {
            strcat(valstr, s);
        }
    }
done:
    if (f_copy != NULL) {
        sal_free(f_copy);
    }
    return rv;
}

/*
 * modify_mem_fields
 *
 *   Verify similar to modify_reg_fields (see reg.c) but works on
 *   memory table entries instead of register values.  Handles fields
 *   of any length.
 *
 *   If mask is non-NULL, it receives an entry which is a mask of all
 *   fields modified.
 *
 *   Values may be specified with optional increment or decrement
 *   amounts; for example, a MAC address could be 0x1234+2 or 0x1234-1
 *   to specify an increment of +2 or -1, respectively.
 *
 *   If incr is FALSE, the increment is ignored and the plain value is
 *   stored in the field (e.g. 0x1234).
 *
 *   If incr is TRUE, the value portion is ignored.  Instead, the
 *   increment value is added to the existing value of the field.  The
 *   field value wraps around on overflow.
 *
 *   Returns -1 on failure, 0 on success.
 */

STATIC int
modify_mem_fields(int unit, soc_mem_t mem, uint32 *entry,
                  uint32 *mask, char *mod, int incr)
{
    soc_field_info_t    *fld;
    char                *fmod, *fval, *s;
    char                *modstr = NULL;
    uint32              fvalue[SOC_MAX_MEM_FIELD_WORDS];
    uint32              fincr[SOC_MAX_MEM_FIELD_WORDS];
    int                 i, entry_dw;
    soc_mem_info_t      *m = &SOC_MEM_INFO(unit, mem);
    char *tokstr;

    entry_dw = BYTES2WORDS(m->bytes);
    if ((modstr = sal_alloc(ARGS_BUFFER, "modify_mem")) == NULL) {
        cli_out("modify_mem_fields: Out of memory\n");
        return CMD_FAIL;
    }

    strncpy(modstr, mod, ARGS_BUFFER);/* Don't destroy input string */
    modstr[ARGS_BUFFER - 1] = 0;
    mod = modstr;

    if (mask) {
        memset(mask, 0, entry_dw * 4);
    }

    while ((fmod = sal_strtok_r(mod, ",", &tokstr)) != 0) {
        mod = NULL;             /* Pass strtok NULL next time */
        fval = strchr(fmod, '=');
        if (fval != NULL) {     /* Point fval to arg, NULL if none */
            *fval++ = 0;        /* Now fmod holds only field name. */
        }
        if (fmod[0] == 0) {
            cli_out("Null field name\n");
            sal_free(modstr);
            return -1;
        }
        if (!sal_strcasecmp(fmod, "clear")) {
            memset(entry, 0, entry_dw * sizeof (*entry));
            if (mask) {
                memset(mask, 0xff, entry_dw * sizeof (*entry));
            }
            continue;
        }
        for (fld = &m->fields[0]; fld < &m->fields[m->nFields]; fld++) {
            if (!sal_strcasecmp(fmod, SOC_FIELD_NAME(unit, fld->field))) {
                break;
            }
        }
        if (fld == &m->fields[m->nFields]) {
            cli_out("No such field \"%s\" in memory \"%s\".\n", fmod,
                    SOC_MEM_UFNAME(unit, mem));
            sal_free(modstr);
            return -1;
        }
        if (!fval) {
            cli_out("Missing %d-bit value to assign to \"%s\" field \"%s\".\n",
                    fld->len, SOC_MEM_UFNAME(unit, mem),
                    SOC_FIELD_NAME(unit, fld->field));
            sal_free(modstr);
            return -1;
        }
        s = strchr(fval, '+');
        if (s == NULL) {
            s = strchr(fval, '-');
        }
        if (s == fval) {
            s = NULL;
        }
        if (incr) {
            if (s != NULL) {
                parse_long_integer(fincr, SOC_MAX_MEM_FIELD_WORDS,
                                   s[1] ? &s[1] : "1");
                if (*s == '-') {
                    neg_long_integer(fincr, SOC_MAX_MEM_FIELD_WORDS);
                }
                if (fld->len & 31) {
                    /* Proper treatment of sign extension */
                    fincr[fld->len / 32] &= ~(0xffffffff << (fld->len & 31));
                }
                soc_mem_field_get(unit, mem, entry, fld->field, fvalue);
                add_long_integer(fvalue, fincr, SOC_MAX_MEM_FIELD_WORDS);
                if (fld->len & 31) {
                    /* Proper treatment of sign extension */
                    fvalue[fld->len / 32] &= ~(0xffffffff << (fld->len & 31));
                }
                soc_mem_field_set(unit, mem, entry, fld->field, fvalue);
            }
        } else {
            if (s != NULL) {
                *s = 0;
            }
            parse_long_integer(fvalue, SOC_MAX_MEM_FIELD_WORDS, fval);
            for (i = fld->len; i < SOC_MAX_MEM_FIELD_BITS; i++) {
                if (fvalue[i / 32] & 1 << (i & 31)) {
                    cli_out("Value \"%s\" too large for %d-bit field \"%s\".\n",
                            fval, fld->len, SOC_FIELD_NAME(unit, fld->field));
                    sal_free(modstr);
                    return -1;
                }
            }
            soc_mem_field_set(unit, mem, entry, fld->field, fvalue);
        }
        if (mask) {
            memset(fvalue, 0, sizeof (fvalue));
            for (i = 0; i < fld->len; i++) {
                fvalue[i / 32] |= 1 << (i & 31);
            }
            soc_mem_field_set(unit, mem, mask, fld->field, fvalue);
        }
    }

    sal_free(modstr);
    return 0;
}

STATIC int
parse_dwords(int count, uint32 *dw, args_t *a)
{
    char        *s;
    int         i;

    for (i = 0; i < count; i++) {
        if ((s = ARG_GET(a)) == NULL) {
            cli_out("Not enough data values (have %d, need %d)\n", i, count);
            return -1;
        }
        dw[i] = parse_integer(s);
    }

    if (ARG_CNT(a) > 0) {
        cli_out("Ignoring extra data on command line (only %d words needed)\n",
                count);
    }

    return 0;
}

/*
 * Print a one line summary for matching memories
 * If substr_match is NULL, match all memories.
 * If substr_match is non-NULL, match any memories whose name
 * or user-friendly name contains that substring.
 */
STATIC void
dpp_mem_list_summary(int unit, char *substr_match)
{
    soc_mem_t   mem;
    int         i, copies, dlen;
    int         found = 0;
    char        *dstr;

    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!soc_mem_is_valid(unit, mem)) {
            continue;
        }

        if (substr_match != NULL &&
            strcaseindex(SOC_MEM_NAME(unit, mem), substr_match) == NULL &&
            strcaseindex(SOC_MEM_UFNAME(unit, mem), substr_match) == NULL) {
            continue;
        }

        copies = 0;
        SOC_MEM_BLOCK_ITER(unit, mem, i) {
            copies += 1;
        }

        dlen = strlen(SOC_MEM_DESC(unit, mem));
        if (dlen > 38) {
            dlen = 34;
            dstr = "...";
        } else {
            dstr = "";
        }
        if (!found) {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                   " %-6s  %-22s%5s/%-4s %s\n",
                   "Flags", "Name", "Entry",
                   "Copy", "Description");
            found = 1;
        }

        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
               " %c%c%c%c%c%c  %-22s %5d",
               soc_mem_is_readonly(unit, mem) ? 'r' : '-',
               soc_mem_is_debug(unit, mem) ? 'd' : '-',
               soc_mem_is_sorted(unit, mem) ? 's' :
               soc_mem_is_hashed(unit, mem) ? 'h' :
               soc_mem_is_cam(unit, mem) ? 'A' : '-',
               soc_mem_is_cbp(unit, mem) ? 'c' : '-',
               (soc_mem_is_bistepic(unit, mem) ||
                soc_mem_is_bistffp(unit, mem) ||
                soc_mem_is_bistcbp(unit, mem)) ? 'b' : '-',
               soc_mem_is_cachable(unit, mem) ? 'C' : '-',
               SOC_MEM_UFNAME(unit, mem),
               soc_mem_index_count(unit, mem));
        if (copies == 1) {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%5s %*.*s%s\n",
                   "",
                   dlen, dlen, SOC_MEM_DESC(unit, mem), dstr);
        } else {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "/%-4d %*.*s%s\n",
                   copies,
                   dlen, dlen, SOC_MEM_DESC(unit, mem), dstr);
        }
    }

    if (found) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Flags: (r)eadonly, (d)ebug, (s)orted, (h)ashed\n"
               "       C(A)M, (c)bp, (b)ist-able, (C)achable\n");
    } else if (substr_match != NULL) {
        cli_out("No memory found with the substring '%s' in its name.\n",
                substr_match);
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");

}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
#ifdef BCM_PETRA_SUPPORT

STATIC cmd_result_t
do_dump_dpp_sw(int unit, args_t *a)
{
    char *c;
    int  dump_all = FALSE;


    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(c, "all")) {
        dump_all = TRUE;
    }

    if (dump_all || !sal_strcasecmp(c, "cosq")) {
        _bcm_dpp_cosq_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "port")) {
        _bcm_dpp_port_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "stack")) {
        _bcm_dpp_stk_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "trunk")) {
        _bcm_dpp_trunk_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "counters")) {
        _bcm_dpp_counters_sw_dump(unit);
    }

#ifdef BCM_ARAD_SUPPORT
    if (dump_all || !sal_strcasecmp(c, "arad-soc")) {
        arad_sw_db_sw_dump(unit);
    }
#endif

    return(CMD_OK);
}
#endif  /* BCM_PETRA_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/*
 * List the tables, or fields of a table entry
 */
char cmd_dpp_mem_list_usage[] =
    "Parameters: [<TABLE> [<DATA> ...]]\n\t"
    "If no parameters are given, displays a reference list of all\n\t"
    "memories and their attributes.\n\t"
    "If TABLE is given, displays the entry fields for that table.\n\t"
    "If DATA is given, decodes the data into entry fields.\n";

cmd_result_t
cmd_dpp_mem_list(int unit, args_t *a)
{
    soc_mem_info_t      *m;
    soc_field_info_t    *fld;
    char                *tab, *s;
    soc_mem_t           mem;
    uint32              entry[SOC_MAX_MEM_WORDS];
    uint32              mask[SOC_MAX_MEM_WORDS];
    int                 have_entry, i, dw, copyno;
    int                 copies, disabled, dmaable;
    char                *dmastr;
    uint32              flags;
    int                 minidx, maxidx;
    uint8               acc_type;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (!soc_property_get(unit, spn_MEMLIST_ENABLE, 1)) {
        return CMD_OK;
    }


    tab = ARG_GET(a);

    if (!tab) {
        dpp_mem_list_summary(unit, NULL);
        return CMD_OK;
    }

    if (parse_memory_name(unit, &mem, tab, &copyno, 0) < 0) {
        if ((s = strchr(tab, '.')) != NULL) {
            *s = 0;
        }
        dpp_mem_list_summary(unit, tab);
        return CMD_OK;
    }

    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");
    if (!SOC_MEM_IS_VALID(unit, mem)) {
        cli_out("ERROR: Memory \"%s\" not valid for this unit\n", tab);
        return CMD_FAIL;
    }

    if (copyno < 0) {
        copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    } else if (!SOC_MEM_BLOCK_VALID(unit, mem, copyno)) {
        cli_out("ERROR: Invalid copy number %d for memory %s\n", copyno, tab);
        return CMD_FAIL;
    }

    m = &SOC_MEM_INFO(unit, mem);
    flags = m->flags;

    dw = BYTES2WORDS(m->bytes);

    if ((s = ARG_GET(a)) == 0) {
        have_entry = 0;
    } else {
        for (i = 0; i < dw; i++) {
            if (s == 0) {
                cli_out("Not enough data specified (%d words needed)\n", dw);
                return CMD_FAIL;
            }
            entry[i] = parse_integer(s);
            s = ARG_GET(a);
        }
        if (s) {
            cli_out("Extra data specified (ignored)\n");
        }
        have_entry = 1;
    }

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Memory: %s.%s", SOC_MEM_UFNAME(unit, mem),
           SOC_BLOCK_NAME(unit, copyno));
    s = SOC_MEM_UFALIAS(unit, mem);
    if (s && *s && strcmp(SOC_MEM_UFNAME(unit, mem), s) != 0) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " alias %s \n", s);
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " address 0x%08x\n", soc_mem_addr_get(unit, mem, 0, copyno,
                                                 0, &acc_type));

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Flags:");
    if (flags & SOC_MEM_FLAG_READONLY) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " read-only");
    }

    if (flags & SOC_MEM_FLAG_WRITEONLY) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," write-only");
    }
    if (flags & SOC_MEM_FLAG_SIGNAL) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," signal/dynamic");
    }

    if (flags & SOC_MEM_FLAG_VALID) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," valid");
    }
    if (flags & SOC_MEM_FLAG_DEBUG) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," debug");
    }
    if (flags & SOC_MEM_FLAG_SORTED) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," sorted");
    }
    if (flags & SOC_MEM_FLAG_CBP) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," cbp");
    }
    if (flags & SOC_MEM_FLAG_CACHABLE) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," cachable");
    }
    if (flags & SOC_MEM_FLAG_BISTCBP) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," bist-cbp");
    }
    if (flags & SOC_MEM_FLAG_BISTEPIC) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," bist-epic");
    }
    if (flags & SOC_MEM_FLAG_BISTFFP) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," bist-ffp");
    }
    if (flags & SOC_MEM_FLAG_UNIFIED) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," unified");
    }
    if (flags & SOC_MEM_FLAG_HASHED) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," hashed");
    }
    if (flags & SOC_MEM_FLAG_WORDADR) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," word-addressed");
    }
    if (flags & SOC_MEM_FLAG_BE) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," big-endian");
    }
    if (flags & SOC_MEM_FLAG_IS_ARRAY) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"  array[0-%u]", SOC_MEM_NUMELS(unit, mem)-1);

    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"\n");

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"Blocks: ");
    copies = disabled = dmaable = 0;
    SOC_MEM_BLOCK_ITER(unit, mem, i) {
        if (SOC_INFO(unit).block_valid[i]) {
            dmastr = "";
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," %s%s", SOC_BLOCK_NAME(unit, i), dmastr);
        } else {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," [%s]", SOC_BLOCK_NAME(unit, i));
            disabled += 1;
        }
        copies += 1;
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," (%d cop%s", copies, copies == 1 ? "y" : "ies");
    if (disabled) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,", %d disabled", disabled);
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,")\n");

    minidx = soc_mem_index_min(unit, mem);
    maxidx = soc_mem_index_max(unit, mem);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"Entries: %d with indices %d-%d (0x%x-0x%x)", maxidx - minidx + 1,
           minidx, maxidx, minidx, maxidx);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,", each %d bytes %d words\n", m->bytes, dw);

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"Entry mask:");
    soc_mem_datamask_get(unit, mem, mask);
    for (i = 0; i < dw; i++) {
        if (mask[i] == 0xffffffff) {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," -1");
        } else if (mask[i] == 0) {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," 0");
        } else {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," 0x%08x", mask[i]);
        }
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"\n");

    s = SOC_MEM_DESC(unit, mem);
    if (s && *s) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"Description: %s\n", s);
    }

    for (fld = &m->fields[m->nFields - 1]; fld >= &m->fields[0]; fld--) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"  %s<%d",
               SOC_FIELD_NAME(unit, fld->field), fld->bp + fld->len - 1);
        if (fld->len > 1) {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,":%d", fld->bp);
        }
        if (have_entry) {
            uint32 fval[SOC_MAX_MEM_FIELD_WORDS];
            char tmp[132];

            memset(fval, 0, sizeof (fval));
            soc_mem_field_get(unit, mem, entry, fld->field, fval);
            format_long_integer(tmp, fval, SOC_MAX_MEM_FIELD_WORDS);
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"> = %s\n", tmp);
        } else {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,">\n");
        }
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");
    return CMD_OK;
}

/* read or write wide memory, currently supports ARAD only */
STATIC cmd_result_t
_soc_mem_array_wide_access(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data,
                           unsigned operation) /* operation should be 1 for read and 0 for write */
{
    int     rv = CMD_FAIL;
    uint32  data32, address;
    uint8   acc_type;
    int blk;
    soc_timeout_t to;

    assert(operation <= 1);
    if (index < 0) {
        index = -index; /* get rid of cache marking, do not support cache */
    }

    /* Write to one or all copies of the memory */

    MEM_LOCK(unit, mem);

    /* loop over the blocks */
    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
        if (copyno != COPYNO_ALL && copyno != blk) {
            continue;
        }

        data32 = 0;
        soc_reg_field_set(unit, OCB_OCB_CONFIGURATION_REGISTERr, &data32, IND_ENf, 1);
        if (WRITE_OCB_OCB_CONFIGURATION_REGISTERr(unit, data32) != SOC_E_NONE) {
            cli_out("Failed OCB_OCB_CONFIGURATION_REGISTERr(0x%x, 1)\n", unit);
            goto done;
        }
        if (!operation) { /* write operation */
            if (WRITE_OCB_INDIRECT_WRITE_DATA_0r(unit, entry_data) != SOC_E_NONE) {
                cli_out("Failed WRITE_OCB_INDIRECT_WRITE_DATA_0r(0x%x, %p)\n", unit, entry_data);
                goto done;
            }
            if (WRITE_OCB_INDIRECT_WRITE_DATA_1r(unit, ((uint32*)entry_data) + (512/32)) != SOC_E_NONE) {
                cli_out("Failed WRITE_OCB_INDIRECT_WRITE_DATA_1r(0x%x, %p)\n", unit, ((uint32*)entry_data) + (512/32));
                goto done;
            }
        }
        data32 = 0; /* not really needed as all bets are later set */
        address = soc_mem_addr_get(unit, mem, array_index, blk, index, &acc_type);
        /* cli_out(" address: 0x%x\n", address); */
        soc_reg_field_set(unit, OCB_INDIRECT_COMMAND_ADDRESSr, &data32, INDIRECT_COMMAND_ADDRf, address);
        soc_reg_field_set(unit, OCB_INDIRECT_COMMAND_ADDRESSr, &data32, INDIRECT_COMMAND_TYPEf, operation);
        if (WRITE_OCB_INDIRECT_COMMAND_ADDRESSr(unit, data32) != SOC_E_NONE) {
            cli_out("Failed WRITE_OCB_INDIRECT_COMMAND_ADDRESSr(0x%x, 0x%x)\n", unit, data32);
            goto done;
        }
        if (READ_OCB_INDIRECT_COMMANDr(unit, &data32) != SOC_E_NONE) {
            cli_out("Failed READ_OCB_INDIRECT_COMMANDr(0x%x, 0x%x)\n", unit, data32);
            goto done;
        }
        soc_reg_field_set(unit, OCB_INDIRECT_COMMANDr, &data32, INDIRECT_COMMAND_TRIGGERf, 1);
        if (WRITE_OCB_INDIRECT_COMMANDr(unit, data32) != SOC_E_NONE) {
            cli_out("Failed WRITE_OCB_INDIRECT_COMMANDr(0x%x, 0x%x)\n", unit, data32);
            goto done;
        }

        /* wait for trigger to become 0 */
        soc_timeout_init(&to, 5000, 10);
        while(1) {
            if (READ_OCB_INDIRECT_COMMANDr(unit, &data32) != SOC_E_NONE) {
                cli_out("Failed READ_OCB_INDIRECT_COMMANDr(0x%x, 0x%x)\n", unit, data32);
                goto done;
            }
            if (soc_reg_field_get(unit, OCB_INDIRECT_COMMANDr, data32, INDIRECT_COMMAND_TRIGGERf)) {
                if (soc_timeout_check(&to)) {
                    cli_out("indirect wide memory operation timed out\n");
                    goto done;
                }
            } else {
                break;
            }
        }
        if (operation) { /* read operation */
            soc_reg_above_64_val_t above_64_val;
            if (READ_OCB_INDIRECT_READ_DATA_0r(unit, above_64_val) != SOC_E_NONE) {
                cli_out("Failed READ_OCB_INDIRECT_READ_DATA_0r(0x%x, %p)\n", unit, entry_data);
                goto done;
            }
            memcpy(entry_data, above_64_val, (512/8));
            if (READ_OCB_INDIRECT_READ_DATA_1r(unit, above_64_val) != SOC_E_NONE) {
                cli_out("Failed READ_OCB_INDIRECT_READ_DATA_1r(0x%x, %p)\n", unit, entry_data);
                goto done;
            }
            memcpy(((uint32*)entry_data) + (512/32), above_64_val, (512/8));
        }
        soc_reg_field_set(unit, OCB_OCB_CONFIGURATION_REGISTERr, &data32, IND_ENf, 0);
        if (WRITE_OCB_OCB_CONFIGURATION_REGISTERr(unit, data32) != SOC_E_NONE) {
            cli_out("Failed OCB_OCB_CONFIGURATION_REGISTERr(0x%x, 0)\n", unit);
            goto done;
        }

    } /* finished looping over blocks */

    rv = CMD_OK;

 done:

    MEM_UNLOCK(unit, mem);
    return rv;
}

STATIC cmd_result_t
dpp_do_dump_table(int unit, soc_mem_t mem, unsigned array_index,
                  int copyno, int index, int count, int flags)
{
    int       k, i;
    uint32    entry[SOC_MAX_MEM_WORDS];
    char      lineprefix[256];
    int       entry_dw;
    int       rv = CMD_FAIL;
    int       wide = 0;

    assert(copyno >= 0);

    entry_dw = soc_mem_entry_words(unit, mem);

    if ( entry_dw >= CMIC_SCHAN_WORDS(unit)
#ifdef BCM_PETRAB_SUPPORT
         && !SOC_IS_PETRAB(unit)
#endif
       ) {
        if (mem != OCB_OCBM_EVENm && mem != OCB_OCBM_ODDm) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "Error: can't read unknown wide memory %s\n"),SOC_MEM_UFNAME(unit, mem)));
            rv = CMD_FAIL;
            goto done;
        }
        wide = 1;
    }

    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");

    for (k = index; k < index + count; k++) {

        if(wide) {
           i =  _soc_mem_array_wide_access(unit, mem, array_index, copyno, k, entry, 1);
        } else if(!(flags & DUMP_DISABLE_CACHE)) {
           i = soc_mem_array_read(unit, mem, array_index, copyno, k, entry);
        } else {
           i =soc_mem_array_read_flags(unit, mem, array_index, copyno, k, entry, SOC_MEM_DONT_USE_CACHE);
        }
        if (i < 0) {
            cli_out("Read ERROR: table %s.%s[%d]: %s\n",
                    SOC_MEM_UFNAME(unit, mem),
                    SOC_BLOCK_NAME(unit, copyno), k, soc_errmsg(i));
            goto done;
        }

        if (!(flags & DUMP_TABLE_ALL)) {
            
        }

        if (flags & DUMP_TABLE_HEX) {
            for (i = 0; i < entry_dw; i++) {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%08x\n", entry[i]);
            }
        } else if (flags & DUMP_TABLE_CHANGED) {
            if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_IS_ARRAY) {
               sal_sprintf(lineprefix, "%s[%d].%s[%d]: ", SOC_MEM_UFNAME(unit, mem), array_index,
                            SOC_BLOCK_NAME(unit, copyno), k);
            }
            else {
                sal_sprintf(lineprefix, "%s.%s[%d]: ", SOC_MEM_UFNAME(unit, mem),
                            SOC_BLOCK_NAME(unit, copyno), k);
            }
            soc_mem_entry_dump_if_changed(unit, mem, entry, lineprefix);
        } else {
            if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_IS_ARRAY) {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%s[%d].%s[%d]: ", SOC_MEM_UFNAME(unit, mem), array_index,
                       SOC_BLOCK_NAME(unit, copyno), k);
            }
            else {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%s.%s[%d]: ", SOC_MEM_UFNAME(unit, mem),
                       SOC_BLOCK_NAME(unit, copyno), k);
            }
            if (flags & DUMP_TABLE_RAW) {
                for (i = 0; i < entry_dw; i++) {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"0x%08x ", entry[i]);
                }
            } else {
                soc_mem_entry_dump(unit, mem, entry);
            }
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"\n");
        }
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");

    rv = CMD_OK;

 done:
    return rv;
}
STATIC cmd_result_t
dpp_do_dump_table_field(int unit, soc_mem_t mem, unsigned array_index,
                  int copyno, int index, int count, int flags, char *field)
{
    int       k, i;
    uint32    entry[SOC_MAX_MEM_WORDS];
    char      lineprefix[256];
    int       entry_dw;
    int       rv = CMD_FAIL;
    int       wide = 0;
    assert(copyno >= 0);
    entry_dw = soc_mem_entry_words(unit, mem);
    if ( entry_dw >= CMIC_SCHAN_WORDS(unit)
#ifdef BCM_PETRAB_SUPPORT
         && !SOC_IS_PETRAB(unit)
#endif
       ) {
        if (mem != OCB_OCBM_EVENm && mem != OCB_OCBM_ODDm) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "Error: can't read unknown wide memory %s\n"),SOC_MEM_UFNAME(unit, mem)));
            rv = CMD_FAIL;
            goto done;
        }
        wide = 1;
    }
    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");
    for (k = index; k < index + count; k++) {
        if(wide) {
           i =  _soc_mem_array_wide_access(unit, mem, array_index, copyno, k, entry, 1);
        } else if(!(flags & DUMP_DISABLE_CACHE)) {
           i = soc_mem_array_read(unit, mem, array_index, copyno, k, entry);
        } else {
           i =soc_mem_array_read_flags(unit, mem, array_index, copyno, k, entry, SOC_MEM_DONT_USE_CACHE);
        }
        if (i < 0) {
            cli_out("Read ERROR: table %s.%s[%d]: %s\n",
                    SOC_MEM_UFNAME(unit, mem),
                    SOC_BLOCK_NAME(unit, copyno), k, soc_errmsg(i));
            goto done;
        }
        if (!(flags & DUMP_TABLE_ALL)) {
        }
        if (flags & DUMP_TABLE_HEX) {
            for (i = 0; i < entry_dw; i++) {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%08x\n", entry[i]);
            }
        } else if (flags & DUMP_TABLE_CHANGED) {
            if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_IS_ARRAY) {
               sal_sprintf(lineprefix, "%s[%d].%s[%d]: ", SOC_MEM_UFNAME(unit, mem), array_index,
                            SOC_BLOCK_NAME(unit, copyno), k);
            }
            else {
                sal_sprintf(lineprefix, "%s.%s[%d]: ", SOC_MEM_UFNAME(unit, mem),
                            SOC_BLOCK_NAME(unit, copyno), k);
            }
            soc_mem_entry_dump_if_changed(unit, mem, entry, lineprefix);
        } else {
            if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_IS_ARRAY) {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%s[%d].%s[%d]: ", SOC_MEM_UFNAME(unit, mem), array_index,
                       SOC_BLOCK_NAME(unit, copyno), k);
            }
            else {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%s.%s[%d]: ", SOC_MEM_UFNAME(unit, mem),
                       SOC_BLOCK_NAME(unit, copyno), k);
            }
            if (flags & DUMP_TABLE_RAW) {
                for (i = 0; i < entry_dw; i++) {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"0x%08x ", entry[i]);
                }
            } else {
                soc_mem_entry_dump_field(unit, mem, entry, field);
            }
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"\n");
        }
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");
    rv = CMD_OK;

 done:
    return rv;
}


/*
 * Function:
 *    soc_mem_entry_dump
 * Purpose:
 *    Debug routine to dump a formatted table entry.
 *
 *    Note:  Prefix != NULL : Dump chg command
 *           Prefix == NULL : Dump     command
 *             (Actually should pass dump_chg flag but keeping for simplicity)
 */
void
soc_mem_entry_dump_field(int unit, soc_mem_t mem, void *buf, char* field_name)
{
    soc_field_info_t *fieldp;
    soc_mem_info_t *memp;
    int f;
    int field_found=0;
#if !defined(SOC_NO_NAMES)
    uint32 key_type = 0, default_type = 0;
    uint32 fval[SOC_MAX_MEM_FIELD_WORDS];
    char tmp[(SOC_MAX_MEM_FIELD_WORDS * 8) + 3];
#endif
             /* Max nybbles + "0x" + null terminator */

    memp = &SOC_MEM_INFO(unit, mem);

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        cli_out(" Memory not valid for unit ");
    } else {
#if !defined(SOC_NO_NAMES)
    if (memp->flags & SOC_MEM_FLAG_MULTIVIEW) {
#ifdef KEY_TYPEf
        if (soc_mem_field_valid(unit, mem, KEY_TYPEf)) {
            soc_mem_field_get(unit, mem, buf, KEY_TYPEf, &key_type);
        } else if (soc_mem_field_valid(unit, mem, KEY_TYPE_0f)) {
            soc_mem_field_get(unit, mem, buf, KEY_TYPE_0f, &key_type);
        } else if (soc_mem_field_valid(unit, mem, VP_TYPEf)) {
            soc_mem_field_get(unit, mem, buf, VP_TYPEf, &key_type);
        } else {
            soc_mem_field_get(unit, mem, buf, ENTRY_TYPEf, &key_type);
        }
#endif
        default_type = 0;
#if defined(BCM_TRIUMPH2_SUPPORT)
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
            SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit) ||
            SOC_IS_HURRICANE(unit) || SOC_IS_TD_TT(unit) ||
            SOC_IS_KATANA(unit)) {
            if((mem == VLAN_MACm) ||
                (mem == L3_ENTRY_IPV4_MULTICASTm) ||
                (mem == L3_ENTRY_IPV6_UNICASTm) ||
                (mem == L3_ENTRY_IPV6_MULTICASTm)){
                default_type = key_type;
            }
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
            if (sal_strlen(memp->views[key_type]) == 0) {
                cli_out(" Wrong Key_type %d",key_type);
                return;
            }
        }
#endif
    for (f = memp->nFields - 1; f >= 0; f--) {
        fieldp = &memp->fields[f];
        if (sal_strcasecmp(SOC_FIELD_NAME(unit, fieldp->field),field_name)) {
            continue;
        } else if (field_found++ == 0) {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"<");
        }

#if !defined(SOC_NO_NAMES)
            if (memp->flags & SOC_MEM_FLAG_MULTIVIEW) {
                if (strstr(soc_fieldnames[fieldp->field], memp->views[key_type]) ||
                    (strcmp(memp->views[key_type], memp->views[default_type]) == 0 &&
                     !strstr(soc_fieldnames[fieldp->field], ":"))
                    || (fieldp->flags & SOCF_GLOBAL)) {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%s=", soc_fieldnames[fieldp->field]);
                        sal_memset(fval, 0, sizeof (fval));
                        soc_mem_field_get(unit, mem, buf, fieldp->field, fval);
                        _shr_format_long_integer(tmp, fval, BITS2BYTES(fieldp->len));
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%s%s", tmp, f > 0 ? "," : "");
                }
            } else {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%s=", soc_fieldnames[fieldp->field]);
                sal_memset(fval, 0, sizeof (fval));
                soc_mem_field_get(unit, mem, buf, fieldp->field, fval);
                _shr_format_long_integer(tmp, fval, BITS2BYTES(fieldp->len));
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%s%s", tmp, f > 0 ? "," : "");
            }

#endif
         }
    }

    if (field_found) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,">");
    } else {
        cli_out(" field: \"%s\" didn't found  ", field_name);
    }

}


STATIC int
dpp_all_mem_get(int unit, int flags)
{
    soc_mem_t mem;
    int i, rv;

    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!soc_mem_is_valid(unit, mem)) {
            continue;
        }
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"unit %d, mem %d\n",unit,mem);
        SOC_MEM_BLOCK_ITER(unit, mem, i) {
            int index = SOC_MEM_INFO(unit,mem).index_min;
            int count = SOC_MEM_INFO(unit,mem).index_max - SOC_MEM_INFO(unit,mem).index_min + 1;
            if(soc_mem_entry_words(unit, mem)+1 > CMIC_SCHAN_WORDS(unit)
#ifdef BCM_PETRAB_SUPPORT
               && !SOC_IS_PETRAB(unit)
#endif
               ) {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META_U(unit,
                                      "Error: can't read memory %s\n"),SOC_MEM_UFNAME(unit, mem)));
                rv = CMD_FAIL;
            } else {
                rv = dpp_do_dump_table(unit, mem, 0, i, index, count, flags);
            }
            if(rv != CMD_OK) {
                cli_out("failed to dump table %d copy %d indexd %d unit %d\n",mem,i,index,unit);
            }
        }
    }

    return CMD_OK;

}


static void
_pci_print_config(int unit)
{
    uint32 data;
    int
        cap_len,
        cap_base,
        next_cap_base,
        i;

    if ((soc_cm_get_dev_type(unit) & BDE_PCI_DEV_TYPE) == 0) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "Error in %s(): Device does not support PCI interface."), FUNCTION_NAME()));
        return;
    }


    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_VENDOR_ID);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  DeviceID=%04x  VendorID=%04x\n",
         PCI_CONF_VENDOR_ID, data,
         (data & 0xffff0000) >> 16,
         (data & 0x0000ffff) >>  0);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_COMMAND);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  Status=%04x  Command=%04x\n",
         PCI_CONF_COMMAND, data,
         (data & 0xffff0000) >> 16,
         (data & 0x0000ffff) >>  0);
    cap_len = (data >> 16) & 0x10 ? 4 : 0;

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_REVISION_ID);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  ClassCode=%06x  RevisionID=%02x\n",
         PCI_CONF_REVISION_ID, data,
         (data & 0xffffff00) >> 8,
         (data & 0x000000ff) >> 0);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev,  PCI_CONF_CACHE_LINE_SIZE);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  BIST=%02x  HeaderType=%02x  "
         "LatencyTimer=%02x  CacheLineSize=%02x\n",
         PCI_CONF_CACHE_LINE_SIZE, data,
         (data & 0xff000000) >> 24,
         (data & 0x00ff0000) >> 16,
         (data & 0x0000ff00) >>  8,
         (data & 0x000000ff) >>  0);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev,  PCI_CONF_BAR0);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  BaseAddress0=%08x\n",
         PCI_CONF_BAR0, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev,  PCI_CONF_BAR1);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  BaseAddress1=%08x\n",
         PCI_CONF_BAR1, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev,  PCI_CONF_BAR2);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  BaseAddress2=%08x\n",
         PCI_CONF_BAR2, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev,  PCI_CONF_BAR3);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  BaseAddress3=%08x\n",
         PCI_CONF_BAR3, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_BAR4);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  BaseAddress4=%08x\n",
         PCI_CONF_BAR4, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev,  PCI_CONF_BAR5);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  BaseAddress5=%08x\n",
         PCI_CONF_BAR5, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev,  PCI_CONF_CB_CIS_PTR);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  CardbusCISPointer=%08x\n",
         PCI_CONF_CB_CIS_PTR, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_SUBSYS_VENDOR_ID);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  SubsystemID=%02x  SubsystemVendorID=%02x\n",
         PCI_CONF_SUBSYS_VENDOR_ID, data,
         (data & 0xffff0000) >> 16,
         (data & 0x0000ffff) >>  0);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev,  PCI_CONF_EXP_ROM);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  ExpansionROMBaseAddress=%08x\n",
         PCI_CONF_EXP_ROM, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, 0x34);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  Reserved=%06x  CapabilitiesPointer=%02x\n",
         0x34, data,
         (data & 0xffffff00) >> 8,
         (data & 0x000000ff) >> 0);
    cap_base = cap_len ? data & 0xff : 0;

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, 0x38);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  Reserved=%08x\n",
         0x38, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_INTERRUPT_LINE);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  Max_Lat=%02x  Min_Gnt=%02x  "
         "InterruptPin=%02x  InterruptLine=%02x\n",
         PCI_CONF_INTERRUPT_LINE, data,
         (data & 0xff000000) >> 24,
         (data & 0x00ff0000) >> 16,
         (data & 0x0000ff00) >>  8,
         (data & 0x000000ff) >>  0);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev,  0x40);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  Reserved=%02x  "
         "RetryTimeoutValue=%02x  TRDYTimeoutValue=%02x\n",
         0x40, data,
         (data & 0xffff0000) >> 16,
         (data & 0x0000ff00) >>  8,
     (data & 0x000000ff) >>  0);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, 0x44);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  PLLConf=%01x\n",
         0x44, data,
         (data & 0x000000ff) >>  0);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev,0x48);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  -\n",
         0x48, data);

    while (cap_base) {
        data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, cap_base);
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  CapabilityID=%02x "
                     "CapabilitiesPointer=%02x ",
                     cap_base, data, data & 0xff, (data >> 8) & 0xff);
        next_cap_base = (data >> 8) & 0xff;
        switch (data & 0xff) {
        case 0x01:
            cap_len = 2 * 4;
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"PWR-MGMT\n");
            break;
        case 0x03:
            cap_len = 2 * 4;
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"VPD\n");
            break;
        case 0x05:
            cap_len = 6 * 4; /* 3 to 6 DWORDS */
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"MSI\n");
            break;
        case 0x10:
            cap_len = 3 * 4;
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"PCIE\n");
            break;
        case 0x11:
            cap_len = 3 * 4;
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"MSI-X\n");
            break;
        default:
            break;
        }
        for (i = 4; i < cap_len; i += 4) {
        data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev,cap_base + i);
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%04x: %08x  -\n", cap_base + i, data);
        }
        cap_base = next_cap_base;
    }
}

char cmd_dpp_dump_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "DUMP [options]\n"
#else
    "DUMP [File=<name>] [Append=true|false] [raw] [hex] [all] [chg] [disable_cache]\n\t"
    "        <TABLE[[ARRAYINDEX]]>[.<COPYNO>][<INDEX>] [<COUNT>]\n\t"
    "        [-filter <FIELD>=<VALUE>[,...]]\n\t"
    "      If raw is specified, show raw memory words instead of fields.\n\t"
    "      If hex is specified, show hex data only (for Expect parsing).\n\t"
    "      If all is specified, show even empty or invalid entries\n\t"
    "      If chg is specified, show only fields changed from defaults\n\t"
    "      If sidable_cache is specified - dosn't read from cached memory, if exist\n\t "
    "      (Use \"listmem\" command to show a list of valid tables)\n"
#endif
    ;

cmd_result_t
cmd_dpp_dump(int unit, args_t *a)
{
    soc_mem_t mem;
    char *arg1, *arg2, *arg3;
    volatile int flags = 0;
    int copyno;
    volatile int rv = CMD_FAIL;
    parse_table_t pt;
    volatile char *fname = "";
    int append = FALSE;
    unsigned array_index;
    char *next_name;
    volatile int console_was_on = 0, console_disabled = 0, pushed_ctrl_c = 0;
    jmp_buf ctrl_c;
    uint32 is_debug = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "File", PQ_STRING, 0, &fname, 0);
    parse_table_add(&pt, "Append", PQ_BOOL, 0, &append, FALSE);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        goto done;
    }
    next_name = NULL;
    if (parse_arg_eq(a, &pt) < 0) {
        rv = CMD_USAGE;
        goto done;
    }

    console_was_on = bslcons_is_enabled();

    if (fname[0] != 0) {
        /* Catch control-C in case if using file output option. */
#ifndef NO_CTRL_C
        if (setjmp(ctrl_c)) {
            rv = CMD_INTR;
            goto done;
        }
#endif
        sh_push_ctrl_c(&ctrl_c);
        pushed_ctrl_c = TRUE;

        if (bslfile_is_enabled()) {
            cli_out("%s: Can't dump to file while logging is enabled\n",
                    ARG_CMD(a));
            rv = CMD_FAIL;
            goto done;
        }

        if (bslfile_open((char *)fname, append) < 0) {
            cli_out("%s: Could not start log file\n", ARG_CMD(a));
            rv = CMD_FAIL;
            goto done;
        }

        bslcons_enable(FALSE);
        console_disabled = 1;
    }

    arg1 = ARG_GET(a);
    for (;;) {
        if (arg1 != NULL && !sal_strcasecmp(arg1, "raw")) {
            flags |= DUMP_TABLE_RAW;
            arg1 = ARG_GET(a);
        } else if (arg1 != NULL && !sal_strcasecmp(arg1, "hex")) {
            flags |= DUMP_TABLE_HEX;
            arg1 = ARG_GET(a);
        } else if (arg1 != NULL && !sal_strcasecmp(arg1, "all")) {
            flags |= DUMP_TABLE_ALL;
            arg1 = ARG_GET(a);
        } else if (arg1 != NULL && !sal_strcasecmp(arg1, "chg")) {
            flags |= DUMP_TABLE_CHANGED;
            arg1 = ARG_GET(a);
        }else if (arg1 != NULL && !sal_strcasecmp(arg1, "debug")) {
            is_debug = 1;
            flags |= REG_PRINT_ADDR;
            arg1 = ARG_GET(a);
        } else if (arg1 != NULL && !sal_strcasecmp(arg1, "disable_cache")){
            flags |= DUMP_DISABLE_CACHE;
            arg1 = ARG_GET(a);
        } else {
            break;
        }
    }

    if (arg1 == NULL) {
        rv = CMD_USAGE;
        goto done;
    }

    if(!sal_strcasecmp(arg1, "soc")) {
        rv = dpp_all_reg_get(unit,is_debug);
        goto done;
    }

    if(!sal_strcasecmp(arg1, "socmem")) {
        rv = dpp_all_mem_get(unit, flags);
        goto done;
    }

    if(!sal_strcasecmp(arg1, "pcic")) {
        _pci_print_config(unit);
        goto done;
    }

    /* See if dumping a memory table */
    if (parse_memory_name(unit, &mem, arg1, &copyno, &array_index) >= 0) {
        int index, count;

        arg2 = ARG_GET(a);
        arg3 = ARG_GET(a);
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            cli_out("Error: Memory %s not valid for chip %s.\n",
                    SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
            goto done;
        }
#ifdef BCM_ARAD_SUPPORT
        if (SOC_IS_ARAD(unit)){
            switch (mem) {
            /* IQM_MEM_8000000 causes HW error in ARAD and ARADPLUS */
            case IQM_MEM_8000000m:
                cli_out("Error: Memory %s not valid for chip %s.\n",
                        SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
                goto done;
            }
        }
#endif /* BCM_ARAD_SUPPORT */
        if (copyno == COPYNO_ALL) {
            copyno = SOC_MEM_BLOCK_ANY(unit, mem);
        }
        if (arg2) {
            index = parse_memory_index(unit, mem, arg2);
            count = (arg3 ? parse_integer(arg3) : 1);
        } else {
            index = soc_mem_index_min(unit, mem);
            if (soc_mem_is_sorted(unit, mem) &&
            !(flags & DUMP_TABLE_ALL)) {
            count = soc_mem_entries(unit, mem, copyno);
            } else {
            count = soc_mem_index_max(unit, mem) - index + 1;
            }
        }


        next_name = ARG_GET(a);
        if (next_name != NULL) {
            rv = dpp_do_dump_table_field(unit, mem, array_index, copyno, index, count, flags,next_name);
        } else {
        rv = dpp_do_dump_table(unit, mem, array_index, copyno, index, count, flags);
        }
        goto done;
    }

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
#ifdef BCM_PETRA_SUPPORT
    /*
     * SW data structures dump
     */
    if (!sal_strcasecmp(arg1, "sw")) {
        rv = do_dump_dpp_sw(unit, a);
        goto done;
    }
#endif /* BCM_PETRA_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

    cli_out("Unknown option or memory to dump "
            "(use 'help dump' for more info)\n");

    rv = CMD_FAIL;

 done:

    if (fname[0] != 0) {
        bslfile_close();
    }

    if (console_disabled && console_was_on) {
        bslcons_enable(TRUE);
    }

    if (pushed_ctrl_c) {
        sh_pop_ctrl_c();
    }

    parse_arg_eq_done(&pt);
    return rv;
}

STATIC cmd_result_t
_dpp_mem_write(int unit, args_t *a, int mod)
{
    int         i, index, start, count, copyno;
    unsigned    array_index;
    char        *tab, *idx, *cnt, *s, *memname;
    soc_mem_t   mem;
    uint32      entry[SOC_MAX_MEM_WORDS];
    int         entry_dw, view_len;
    char        copyno_str[8];
    int         r, update;
    int         rv = CMD_FAIL;
    char        *valstr = NULL, *view = NULL;
    int         no_cache = 0;
    int         wide = 0;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        goto done;
    }

    tab = ARG_GET(a);
    if (tab != NULL && sal_strcasecmp(tab, "nocache") == 0) {
        no_cache = 1;
        tab = ARG_GET(a);
    }
    idx = ARG_GET(a);
    cnt = ARG_GET(a);
    s = ARG_GET(a);

    /* you will need at least one value and all the args .. */
    if (!tab || !idx || !cnt || !s || !isint(cnt)) {
        return CMD_USAGE;
    }

    /* Deal with VIEW:MEMORY if applicable */
    memname = strstr(tab, ":");
    view_len = 0;
    if (memname != NULL) {
        memname++;
        view_len = memname - tab;
    } else {
        memname = tab;
    }

    if (parse_memory_name(unit, &mem, memname, &copyno, &array_index) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n",tab);
        goto done;
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "Error: Memory %s not valid for chip %s.\n"),
                              SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit)));
        goto done;
    }

    if (soc_mem_is_readonly(unit, mem)) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "ERROR: Table %s is read-only\n"),
                              SOC_MEM_UFNAME(unit, mem)));
        goto done;
    }

    start = parse_memory_index(unit, mem, idx);
    count = parse_integer(cnt);

    if (copyno == COPYNO_ALL) {
        copyno_str[0] = 0;
    } else {
        sal_sprintf(copyno_str, ".%d", copyno);
    }

    entry_dw = soc_mem_entry_words(unit, mem);

    if ( entry_dw >= CMIC_SCHAN_WORDS(unit)
#ifdef BCM_PETRAB_SUPPORT
         && !SOC_IS_PETRAB(unit)
#endif
       ) {
        if (mem != OCB_OCBM_EVENm && mem != OCB_OCBM_ODDm) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "Error: can't write unknown wide memory %s\n"),SOC_MEM_UFNAME(unit, mem)));
            rv = CMD_FAIL;
            goto done;
        }
        wide = 1;
    }

    if ((valstr = sal_alloc(ARGS_BUFFER, "reg_set")) == NULL) {
        cli_out("cmd_esw_mem_write : Out of memory\n");
        goto done;
    }

    /*
     * If a list of fields were specified, generate the entry from them.
     * Otherwise, generate it by reading raw dwords from command line.
     */
    if (!isint(s)) {
        /* List of fields */
        if (view_len == 0) {
            collect_comma_args(a, valstr, s);
        } else {
            if ((view = sal_alloc(view_len + 1, "view_name")) == NULL) {
                cli_out("cmd_esw_mem_write : Out of memory\n");
                goto done;
            }
            memset(view, 0, view_len + 1);
            memcpy(view, tab, view_len);
            if (collect_comma_args_with_view(a, valstr, s, view, unit, mem)
                < 0) {
                cli_out("Out of memory: aborted\n");
                goto done;
            }
        }

        memset(entry, 0, sizeof (entry));

        if(0 == mod)
        {
            if (modify_mem_fields(unit, mem, entry, NULL, valstr, FALSE) < 0) {
                cli_out("Syntax error: aborted\n");
                goto done;
            }
        }

        update = TRUE;
    } else {
        /* List of numeric values */
        ARG_PREV(a);
        if (parse_dwords(entry_dw, entry, a) < 0) {
            goto done;
        }
        update = FALSE;
    }

    if (bsl_check(bslLayerAppl, bslSourceSocmem, bslSeverityNormal, unit)) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"WRITE[%s%s], DATA:", SOC_MEM_UFNAME(unit, mem), copyno_str);
        for (i = 0; i < entry_dw; i++) {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," 0x%x", entry[i]);
        }
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"\n");
    }

    /* Created entry, now write it */
    for (index = start; index < start + count; index++) {

        if(1 == mod)
        {
            r =
                wide ? _soc_mem_array_wide_access(unit, mem, array_index, copyno, index, entry, 1) :
                       soc_mem_array_read(unit, mem, array_index, copyno, index, entry);
            if (r < 0) {
                  cli_out("READ ERROR: table %s.%s[%d]: %s\n",
                          SOC_MEM_UFNAME(unit, mem), copyno_str, index,
                          soc_errmsg(r));
            }

            if (modify_mem_fields(unit, mem, entry, NULL, valstr, FALSE) < 0) {
                cli_out("Syntax error: aborted\n");
                goto done;
            }
        }

        r =
            wide ? _soc_mem_array_wide_access(unit, mem, array_index, copyno, no_cache ? -index : index, entry, 0) :
                   soc_mem_array_write(unit, mem, array_index, copyno, no_cache ? -index : index, entry);
        if (r < 0) {
            cli_out("Write ERROR: table %s.%s[%d]: %s\n",
                    SOC_MEM_UFNAME(unit, mem), copyno_str, index,
                    soc_errmsg(r));
            goto done;
        }

        if (mod && update) {
            modify_mem_fields(unit, mem, entry, NULL, valstr, TRUE);
        }
    }
    rv = CMD_OK;

done:
    if (valstr != NULL) {
       sal_free(valstr);
    }
    if (view != NULL) {
       sal_free(view);
    }
    return rv;
}


#if defined(BCM_DNX_P3_SUPPORT)

#define DPP_ACCESS_OP_INSERT_EM		0
#define DPP_ACCESS_OP_DELETE_EM		1
#define DPP_ACCESS_OP_LOOKUP_EM		2


STATIC cmd_result_t
_dpp_mem_ins_del_lkup(int unit, args_t *a, int oper)
{
    int         i, index, start, count, copyno;
    unsigned    array_index;
    char        *tab, *s, *memname;
    soc_mem_t   mem;
    uint32      entry[SOC_MAX_MEM_WORDS];
    int         entry_dw, view_len;
    char        copyno_str[8];
    int         update;
    int         rv = CMD_FAIL;
    char        *valstr = NULL, *view = NULL;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        goto done;
    }

    tab = ARG_GET(a);
    if (tab != NULL && sal_strcasecmp(tab, "nocache") == 0) {
        tab = ARG_GET(a);
    }

	/* you will need at least one value and all the args .. */
    if (!tab) {
        return CMD_USAGE;
    }

    /* Deal with VIEW:MEMORY if applicable */
	s = ARG_GET(a);
    memname = strstr(tab, ":");
    view_len = 0;
    if (memname != NULL) {
        memname++;
        view_len = memname - tab;
    } else {
        memname = tab;
    }

    if (parse_memory_name(unit, &mem, memname, &copyno, &array_index) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n",tab);
        goto done;
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        goto done;
    }

    if (soc_mem_is_readonly(unit, mem)) {
        cli_out("ERROR: Table %s is read-only\n",
                SOC_MEM_UFNAME(unit, mem));
        goto done;
    }

	

    /* check parameters */
    /* you will need at least one value and all the args .. */
    

    start = 0; /* no index in EM */
    count = 1;

    if (copyno == COPYNO_ALL) {
        copyno_str[0] = 0;
    } else {
        sal_sprintf(copyno_str, ".%d", copyno);
    }

    entry_dw = soc_mem_entry_words(unit, mem);

    if ( entry_dw >= CMIC_SCHAN_WORDS(unit)
#ifdef BCM_PETRAB_SUPPORT
         && !SOC_IS_PETRAB(unit)
#endif
       ) {
        if (mem != OCB_OCBM_EVENm && mem != OCB_OCBM_ODDm) {
            cli_out("Error: can't write unknown wide memory %s\n",SOC_MEM_UFNAME(unit, mem));
            rv = CMD_FAIL;
            goto done;
        }
    }

    if ((valstr = sal_alloc(ARGS_BUFFER, "reg_set")) == NULL) {
        cli_out("cmd_esw_mem_write : Out of memory\n");
        goto done;
    }

    /*
     * If a list of fields were specified, generate the entry from them.
     * Otherwise, generate it by reading raw dwords from command line.
     */
    if (!isint(s)) {
        /* List of fields */
        if (view_len == 0) {
            collect_comma_args(a, valstr, s);
        } else {
            if ((view = sal_alloc(view_len + 1, "view_name")) == NULL) {
                cli_out("cmd_esw_mem_write : Out of memory\n");
                goto done;
            }
            memset(view, 0, view_len + 1);
            memcpy(view, tab, view_len);
            if (collect_comma_args_with_view(a, valstr, s, view, unit, mem)
                < 0) {
                cli_out("Out of memory: aborted\n");
                goto done;
            }
        }

        memset(entry, 0, sizeof (entry));

		if (modify_mem_fields(unit, mem, entry, NULL, valstr, FALSE) < 0) {
			cli_out("Syntax error: aborted\n");
			goto done;
		}

        update = TRUE;
    } else {
        /* List of numeric values */
        ARG_PREV(a);
        if (parse_dwords(entry_dw, entry, a) < 0) {
            goto done;
        }
        update = FALSE;
    }

    if (LOG_CHECK(BSL_LS_APPL_SOCMEM | BSL_INFO)) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"WRITE[%s%s], DATA:", SOC_MEM_UFNAME(unit, mem), copyno_str);
        for (i = 0; i < entry_dw; i++) {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," 0x%x", entry[i]);
        }
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"\n");
    }

    /* Created entry, now write it */
    for (index = start; index < start + count; index++) {

        /* if(1 == oper)
        {*/
		if (oper == DPP_ACCESS_OP_INSERT_EM) {
				rv = soc_dpp_p3_mem_insert(unit, mem,copyno,entry);
		}
		else if (oper == DPP_ACCESS_OP_LOOKUP_EM) {
			soc_dpp_p3_mem_lkup(unit,mem,copyno,entry);
			soc_mem_entry_dump(unit, mem, entry);
		}
		else if (oper == DPP_ACCESS_OP_DELETE_EM) {
			soc_dpp_p3_mem_delete(unit,mem,copyno,entry);
		}

			if (oper && update) {
				modify_mem_fields(unit, mem, entry, NULL, valstr, TRUE);
			}
		/*}*/
    }
    rv = CMD_OK;

done:
    if (valstr != NULL) {
       sal_free(valstr);
    }
    if (view != NULL) {
       sal_free(view);
    }
    return rv;
}

char cmd_dpp_mem_insert_usage[] =
    "Parameters: <TABLE>[.<COPY>] <ENTRY> <ENTRYCOUNT>\n\t"
    "        { <DW0> .. <DWN> | <FIELD>=<VALUE>[,...] }\n\t"
    "Number of <DW> must be a multiple of table entry size.\n\t"
    "Writes entry(s) into table index(es).\n\t"
    "Key fields has to be set.\n";

cmd_result_t
cmd_dpp_mem_insert(int unit, args_t *a)
{
    return _dpp_mem_ins_del_lkup(unit,a,DPP_ACCESS_OP_INSERT_EM);
}

char cmd_dpp_mem_lkup_usage[] =
    "Parameters: <TABLE>[.<COPY>] <ENTRY> <ENTRYCOUNT>\n\t"
    "        { <DW0> .. <DWN> | <FIELD>=<VALUE>[,...] }\n\t"
    "Number of <DW> must be a multiple of table entry size.\n\t"
    "Writes entry(s) into table index(es).\n\t"
    "Key fields has to be set.\n";

cmd_result_t
cmd_dpp_mem_lkup(int unit, args_t *a)
{
    return _dpp_mem_ins_del_lkup(unit,a,DPP_ACCESS_OP_LOOKUP_EM);
}



char cmd_dpp_mem_delete_usage[] =
    "Parameters: <TABLE>[.<COPY>] <ENTRY> <ENTRYCOUNT>\n\t"
    "        { <DW0> .. <DWN> | <FIELD>=<VALUE>[,...] }\n\t"
    "Number of <DW> must be a multiple of table entry size.\n\t"
    "Writes entry(s) into table index(es).\n\t"
    "Key fields has to be set.\n";

cmd_result_t
cmd_dpp_mem_delete(int unit, args_t *a)
{
    return _dpp_mem_ins_del_lkup(unit,a,DPP_ACCESS_OP_DELETE_EM);
}

#endif /*BCM_DNX_P3_SUPPORT*/



char cmd_dpp_mem_write_usage[] =
    "Parameters: <TABLE>[.<COPY>] <ENTRY> <ENTRYCOUNT>\n\t"
    "        { <DW0> .. <DWN> | <FIELD>=<VALUE>[,...] }\n\t"
    "Number of <DW> must be a multiple of table entry size.\n\t"
    "Writes entry(s) into table index(es).\n";

cmd_result_t
cmd_dpp_mem_write(int unit, args_t *a)
{
    return _dpp_mem_write(unit, a, 0);
}


char cmd_dpp_mem_modify_usage[] =
    "Parameters: <TABLE>[.<COPY>] <ENTRY> <ENTRYCOUNT>\n\t"
    "        <FIELD>=<VALUE>[,...]\n\t"
    "Read/modify/write field(s) of a table entry(s).\n";

cmd_result_t
cmd_dpp_mem_modify(int unit, args_t *a)
{
    return _dpp_mem_write(unit, a, 1);
}

int
dpp_mem_test_iter_callback(int unit, soc_mem_t mem, void* data)
{
    int        rv = SOC_E_NONE;
    void*      p;

    /* Memory is valid for this test if it is valid, and not ro / wo. */
    if (!SOC_MEM_IS_VALID(unit, mem) ||
        (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_DEBUG) ||
        (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) ||
        (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY)) {
        return rv;
    }

#if defined (BCM_DFE_SUPPORT)
    if (SOC_IS_DFE(unit)) {
        /*this memory is auto-updated when device is connected and not in repeater mode.*/
        if(RTP_RMHMTm == mem) {
            return rv;
        }
    }
#endif

    if (mem_test_default_init(unit,mem,&p) < 0)
    {
        return CMD_FAIL;
    }

    if (mem_test(unit,data,p) < 0)
    {
        rv = CMD_FAIL;
    }

    if (mem_test_done(unit,p) < 0)
    {
        return CMD_FAIL;
    }

    return rv;
}

cmd_result_t
do_mem_test(int unit, args_t *a)
{
    int rv = SOC_E_NONE;
    if ((soc_mem_iterate(unit,
                         dpp_mem_test_iter_callback, a)) < 0) {
        rv = CMD_FAIL;
    }

    return rv;
}

int
rval_test_skip_mem(int unit, soc_mem_t mem)
{

#ifdef BCM_ARAD_SUPPORT
    if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        switch(mem) {
            /* static memories */
            case EPNI_MEM_760000m:
            case FSRD_FSRD_WL_EXT_MEMm:
            case IDR_MEM_1B0000m:
            case IHB_MEM_16E0000m:
            case IHB_TCAM_BANK_COMMANDm:
            case IHB_TCAM_BANK_REPLYm:
            case IHP_MEM_9E0000m:
            case IHP_MEM_C50000m:
            case OAMP_MEM_340000m:
            case PORT_WC_UCMEM_DATAm:
            case IHP_MACT_FLUSH_DBm: /* flush db is in ignore list temporary - for automatic tests, but it's hw bug should be resolved - can not read actual info from it*/
            /* dynamic memories */
            case EPNI_ESEM_MANAGEMENT_REQUESTm:
            case IHB_OEMA_MANAGEMENT_REQUESTm:
            case IHB_OEMB_MANAGEMENT_REQUESTm:
            case IHP_ISA_MANAGEMENT_REQUESTm:
            case IHP_ISB_MANAGEMENT_REQUESTm:
            case IDR_GLOBAL_METER_STATUSm:
            case IHB_MEM_10E0000m:
            case IHB_TCAM_BANKm:
            case MMU_DRAM_ADDRESS_SPACEm:
            case MMU_RAFA_RADDR_STATUSm:
            case MMU_RAFB_RADDR_STATUSm:
            case MMU_RAFC_RADDR_STATUSm:
            case MMU_RAFD_RADDR_STATUSm:
            case MMU_RAFE_RADDR_STATUSm:
            case MMU_RAFF_RADDR_STATUSm:
            case MMU_RAFG_RADDR_STATUSm:
            case MMU_RAFH_RADDR_STATUSm:
            case MMU_WAFA_HALFA_RADDR_STATUSm:
            case MMU_WAFA_HALFB_RADDR_STATUSm:
            case MMU_WAFB_HALFA_RADDR_STATUSm:
            case MMU_WAFB_HALFB_RADDR_STATUSm:
            case MMU_WAFC_HALFA_RADDR_STATUSm:
            case MMU_WAFC_HALFB_RADDR_STATUSm:
            case MMU_WAFD_HALFA_RADDR_STATUSm:
            case MMU_WAFD_HALFB_RADDR_STATUSm:
            case MMU_WAFE_HALFA_RADDR_STATUSm:
            case MMU_WAFE_HALFB_RADDR_STATUSm:
            case MMU_WAFF_HALFA_RADDR_STATUSm:
            case MMU_WAFF_HALFB_RADDR_STATUSm:
            case MMU_WAFG_HALFA_RADDR_STATUSm:
            case MMU_WAFG_HALFB_RADDR_STATUSm:
            case MMU_WAFH_HALFA_RADDR_STATUSm:
            case MMU_WAFH_HALFB_RADDR_STATUSm:
            case OAMP_RMAPEM_MANAGEMENT_REQUESTm:
            case SCH_PORT_QUEUE_SIZE__PQSm:
            case SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm:
            /* aliases */
            case PPDB_A_TCAM_BANKm:
            case PPDB_A_TCAM_BANK_COMMANDm:
            case PPDB_A_TCAM_BANK_REPLYm:
            case EDB_ESEM_MANAGEMENT_REQUESTm:
            case PPDB_A_OEMA_MANAGEMENT_REQUESTm:
            case PPDB_A_OEMB_MANAGEMENT_REQUESTm:
            case PPDB_B_LARGE_EM_FLUSH_DBm:
                return 1; /* Skip these tables */
            default:
                break;
        }
    }
#endif

    return 0;
}

int
rval_test_skip_mem_flipflop(int unit, soc_mem_t mem)
{

#ifdef BCM_88650_A0
    if(SOC_IS_ARAD(unit)) {
        switch(mem) {
            case            BRDC_FSRD_FSRD_WL_EXT_MEMm:
            case            PORT_WC_UCMEM_DATAm:
            case            EGQ_CBMm:
            case            IHB_MEM_230000m:
            case               RTP_RRMAm:
            case               IHB_DBG_LAST_FEMm:
            case               IHB_DBG_LAST_FESm:
            case               IPS_MEM_260000m:
            case               IPS_MEM_280000m:
            case               IPT_EGQ_TXQ_RD_ADDRm:
            case               IPT_EGQ_TXQ_WR_ADDRm:
            case               IPT_FDT_TXQ_RD_ADDRm:
            case               IPT_FDT_TXQ_WR_ADDRm:
            case               IQM_CNG_QUE_SETm:
            case               IQM_MEM_1400000m:
            case               IQM_MEM_1600000m:
            case               NBI_RBINS_MEMm:
            case               NBI_RLENG_MEMm:
            case               NBI_RPKTS_MEMm:
            case               NBI_RTYPE_MEMm:
            case               NBI_TBINS_MEMm:
            case               NBI_TLENG_MEMm:
            case               NBI_TPKTS_MEMm:
            case               NBI_TTYPE_MEMm:
            case               SCH_BUCKET_DEFICIT__BDFm:
            case               SCH_FLOW_INSTALLED_MEMORY__FIMm:
            case               SCH_FLOW_STATUS_MEMORY__FSMm:
            case               SCH_MEM_01300000m:
            case               SCH_MEM_01400000m:
            case               SCH_MEM_01500000m:
            case               SCH_MEM_01600000m:
            case               SCH_MEM_01700000m:
            case               SCH_MEM_01800000m:
            case               SCH_MEM_01900000m:
            case               SCH_MEM_01A00000m:
            case               SCH_MEM_01B00000m:
            case               SCH_MEM_01C00000m:
            case               SCH_MEM_03000000m:
            case               SCH_MEM_03100000m:
            case               SCH_MEM_03200000m:
            case               SCH_MEM_03300000m:
            case               SCH_MEM_03400000m:
            case               SCH_MEM_03500000m:
            case               SCH_MEM_07100000m:
            case               CRPS_CRPS_0_CNTS_MEMm:
            case               CRPS_CRPS_1_CNTS_MEMm:
            case               CRPS_CRPS_2_CNTS_MEMm:
            case               CRPS_CRPS_3_CNTS_MEMm:
            case               ECI_MEM_00010000m:
            case               EGQ_RDMMCm:
            case               EGQ_RDMUCm:
            case               FCR_FCR_CRM_Am:
            case               FCR_FCR_CRM_Bm:
            case               IDR_COMPLETE_PCm:
            case               IDR_MEM_100000m:
            case               IDR_MEM_110000m:
            case               IDR_MEM_120000m:
            case               IDR_MEM_140000m:
            case               IDR_MEM_1F0000m:
            case               IHB_OEMA_MANAGEMENT_REQUESTm:
            case               IHB_OEMB_MANAGEMENT_REQUESTm:
            case               IHB_TCAM_BANKm:
            case               IHP_MACT_FLUSH_DBm:
            case               IHP_MEM_430000m:
            case               IPS_MAXQSZm:
            case               IQM_VSQ_A_MX_OCm:
            case               IQM_VSQ_B_MX_OCm:
            case               IQM_VSQ_C_MX_OCm:
            case               IQM_VSQ_D_MX_OCm:
            case               IQM_VSQ_E_MX_OCm:
            case               IQM_VSQ_F_MX_OCm:
            case               NBI_MLF_RX_MEM_A_CTRLm:
            case               NBI_MLF_RX_MEM_B_CTRLm:
            case               OAMP_RMAPEM_MANAGEMENT_REQUESTm:
            case               FSRD_FSRD_WL_EXT_MEMm:
            case               IDR_CONTEXT_COLORm:
            case               IDR_CONTEXT_SIZEm:
            case               IDR_MEM_1B0000m:
            case               IHB_TCAM_ACTIONm:
            case               IHB_TCAM_ACTION_24m:
            case               IHB_TCAM_ACTION_25m:
            case               IHB_TCAM_ACTION_26m:
            case               IHB_TCAM_ACTION_27m:
            case               IHB_TCAM_ACTION_HIT_INDICATIONm:
            case               IHB_TCAM_ENTRY_PARITYm:
            case               IHB_TCAM_ENTRY_PARITY_12m:
            case               IHB_TCAM_ENTRY_PARITY_13m:
            case               IHP_MEM_420000m:
            case               IPS_MEM_200000m:
            case               IQM_MEM_8000000m:
            case               MMU_DRAM_ADDRESS_SPACEm:
            case               MMU_RAFA_RADDR_STATUSm:
            case               MMU_RAFB_RADDR_STATUSm:
            case               MMU_RAFC_RADDR_STATUSm:
            case               MMU_RAFD_RADDR_STATUSm:
            case               MMU_RAFE_RADDR_STATUSm:
            case               MMU_RAFF_RADDR_STATUSm:
            case               MMU_RAFG_RADDR_STATUSm:
            case               MMU_RAFH_RADDR_STATUSm:
            case               MMU_WAFA_HALFA_RADDR_STATUSm:
            case               MMU_WAFA_HALFB_RADDR_STATUSm:
            case               MMU_WAFB_HALFA_RADDR_STATUSm:
            case               MMU_WAFB_HALFB_RADDR_STATUSm:
            case               MMU_WAFC_HALFA_RADDR_STATUSm:
            case               MMU_WAFC_HALFB_RADDR_STATUSm:
            case               MMU_WAFD_HALFA_RADDR_STATUSm:
            case               MMU_WAFD_HALFB_RADDR_STATUSm:
            case               MMU_WAFE_HALFA_RADDR_STATUSm:
            case               MMU_WAFE_HALFB_RADDR_STATUSm:
            case               MMU_WAFF_HALFA_RADDR_STATUSm:
            case               MMU_WAFF_HALFB_RADDR_STATUSm:
            case               MMU_WAFG_HALFA_RADDR_STATUSm:
            case               MMU_WAFG_HALFB_RADDR_STATUSm:
            case               MMU_WAFH_HALFA_RADDR_STATUSm:
            case               MMU_WAFH_HALFB_RADDR_STATUSm:
            case               MMU_RAF_WADDRm:
            case               MMU_RDF_RADDRm:
            case               MMU_WAF_HALFA_WADDRm:
            case               MMU_WAF_HALFB_WADDRm:
            case               SCH_PORT_QUEUE_SIZE__PQSm:
            case               SCH_MEM_04A00000m:
            case               SCH_MEM_04700000m:
            case               SCH_PORT_ENABLE__PORTENm:
            case               SCH_PORT_GROUP__PFGMm:
            case               SCH_CH_NIF_CALENDAR_CONFIGURATION__CNCCm:
            case               SCH_CH_NIF_RATES_CONFIGURATION__CNRCm:
            case               SCH_ONE_PORT_NIF_CONFIGURATION__OPNCm:
            case               SCH_CIR_SHAPER_CALENDAR__CSCm:
            case               SCH_PIR_SHAPER_CALENDAR__PSCm:
            case               SCH_PORT_SCHEDULER_MAP__PSMm:
            case               SCH_SCHEDULER_INITm:
            case               SCH_FORCE_STATUS_MESSAGEm:
            case               EPNI_AC_FORMATm:
            case               EPNI_DATA_FORMATm:
            case               EPNI_IPV4_TUNNEL_FORMATm:
            case               EPNI_LINK_LAYER_OR_ARP_FORMATm:
            case               EPNI_MPLS_POP_FORMATm:
            case               EPNI_MPLS_PUSH_FORMATm:
            case               EPNI_MPLS_SWAP_FORMATm:
            case               EPNI_OUT_RIF_FORMATm:
            case               EPNI_TRILL_FORMATm:
            case               IDR_MCDA_PRFCFG_0m:
            case               IDR_MCDA_PRFCFG_FORMAT_1m:
            case               IHB_FEC_ENTRY_FORMAT_Am:
            case               IHB_FEC_ENTRY_FORMAT_Bm:
            case               IHB_FEC_ENTRY_FORMAT_Cm:
            case               IHB_FEC_ENTRY_FORMAT_NULLm:
            case               IHB_FEC_ENTRY_GENERALm:
            case               IHP_MACT_FORMAT_0_TYPE_0m:
            case               IHP_MACT_FORMAT_0_TYPE_1m:
            case               IHP_MACT_FORMAT_0_TYPE_2m:
            case               IHP_MACT_FORMAT_1m:
            case               IHP_MACT_FORMAT_2m:
            case               IHP_LIF_TABLE_AC_MPm:
            case               IHP_LIF_TABLE_AC_P2P_TO_ACm:
            case               IHP_LIF_TABLE_AC_P2P_TO_PBBm:
            case               IHP_LIF_TABLE_AC_P2P_TO_PWEm:
            case               IHP_LIF_TABLE_IP_TTm:
            case               IHP_LIF_TABLE_ISID_MPm:
            case               IHP_LIF_TABLE_ISID_P2Pm:
            case               IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm:
            case               IHP_LIF_TABLE_LABEL_PWE_MPm:
            case               IHP_LIF_TABLE_LABEL_PWE_P2Pm:
            case               IHP_LIF_TABLE_TRILLm:
            case               IPS_QPM_1_NO_SYS_REDm:
            case               IPS_QPM_2_SYS_REDm:
            case               IPS_CRBALm:
            case               IPS_QDESCm:
            case               IPT_MEM_80000m:
            case               IRR_MCDB_EGRESS_FORMAT_0m:
            case               IRR_MCDB_EGRESS_FORMAT_1m:
            case               IRR_MCDB_EGRESS_FORMAT_2m:
            case               IRR_MCDB_EGRESS_FORMAT_4m:
            case               IRR_MCDB_EGRESS_FORMAT_5m:
            case               IRR_MCDB_EGRESS_FORMAT_6m:
            case               IRR_MCDB_EGRESS_FORMAT_7m:
            case               IRR_MCDB_EGRESS_SPECIAL_FORMATm:
            case               IRR_MCDB_EGRESS_TDM_FORMATm:
            case               OAMP_MEP_DBm:
            case               OAMP_MEP_DB_BFD_CC_ON_MPLSTPm:
            case               OAMP_MEP_DB_BFD_CV_ON_MPLSTPm:
            case               OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm:
            case               OAMP_MEP_DB_BFD_ON_MPLSm:
            case               OAMP_MEP_DB_BFD_ON_PWEm:
            case               OAMP_MEP_DB_Y_1731_ON_MPLSTPm:
            case               OAMP_MEP_DB_Y_1731_ON_PWEm:

               return 1; /* Skip these registers */
            default:
               break;
                }
        }
#endif
    return 0;
}

#ifdef BCM_DPP_SUPPORT
STATIC int
dpp_mem_test_rw_iter_callback(int unit, soc_mem_t mem, void* data)
{
    int        rv = SOC_E_NONE;
    void*      p;
    tr7_dbase_t *tr7_ptr = (tr7_dbase_t *)data;

    /* Validity check for the memory for this test - as follows */
    if (tr7_ptr->enable_skip == 1) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            return rv;
        }

        if ( (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_DEBUG) != 0) {
            LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping debug memory %s\n"), SOC_MEM_NAME(unit, mem)));
            return rv;
        }

        if ( (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) != 0) {
            LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping read only memory %s\n"), SOC_MEM_NAME(unit, mem)));
            return rv;
        }

        if ( (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY) != 0) {
            LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping write only memory %s\n"), SOC_MEM_NAME(unit, mem)));
            return rv;
        }

        if ( (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_SIGNAL) != 0) {
            LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping signal memory (Dynamic mems if on jericho) %s\n"), SOC_MEM_NAME(unit, mem)));
            return rv;
        }

        if (rval_test_skip_mem(unit,mem) == 1) {
            LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping excluded memory %s\n"), SOC_MEM_NAME(unit, mem)));
            return rv;
        }
    }

    /* skipping memories larger than 640b */
    if (2+soc_mem_entry_words(unit, mem) > CMIC_SCHAN_WORDS(unit)) {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping large size memory %s\n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "testing memory %s\n"), SOC_MEM_NAME(unit, mem)));
    if (mem_test_rw_init(unit,mem,&p) < 0)
    {
        return CMD_FAIL;
    }
    if (mem_test(unit,data,p) < 0)
    {
        rv = CMD_FAIL;
    }
    if (mem_test_done(unit,p) < 0)
    {
        return CMD_FAIL;
    }
    return rv;
}


int
dpp_mem_test_flipflop_iter_callback(int unit, soc_mem_t mem, void* data)
{
    int        rv = SOC_E_NONE;
#ifdef BCM_ARAD_SUPPORT
    int        i_array,i_index;
    char       print_str[250];
    unsigned int start_array_index,end_array_index,start_index, end_index;
    uint32      *data_write = NULL;
    tr8_dbase_t *tr8_ptr = (tr8_dbase_t *)data;
    int fault_inject = tr8_ptr->fault_inject;
    uint32  *data_write_inject = NULL;

    if ((data_write = sal_alloc(CMIC_SCHAN_WORDS(unit)*sizeof(uint32), "data_write")) == NULL) {
        cli_out("dpp_mem_test_flipflop_iter_callback : Out of memory\n");
        rv = -1;
        goto done;
    }

    soc_sand_os_memset(data_write, tr8_ptr->write_value, CMIC_SCHAN_WORDS(unit)*sizeof(uint32));
    start_index = parse_memory_index(unit, mem, "min");
    end_index = parse_memory_index(unit, mem, "max");

    /* Memory is valid for this test if it is valid, and not ro / wo. */
    if (!SOC_MEM_IS_VALID(unit,mem)) {
        goto done;
    }

    if ( (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) != 0) {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping read only memory %s\n"), SOC_MEM_NAME(unit, mem)));
        goto done;
    }

    if ( (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY) != 0) {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping write only memory %s\n"), SOC_MEM_NAME(unit, mem)));
        goto done;
    }

    /* skipping excluded memories */
    if (rval_test_skip_mem_flipflop(unit,mem) == 1) {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping excluded memory %s\n"), SOC_MEM_NAME(unit, mem)));
        goto done;
    }

    /* skipping memories larger than 640b */
    if (2+soc_mem_entry_words(unit, mem) > CMIC_SCHAN_WORDS(unit)) {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping large size memory %s\n"), SOC_MEM_NAME(unit, mem)));
        goto done;
    }

    /* skip memories with less or equal to 16 entries (hw request, most of it is not memories but management blocks */
    if (end_index - start_index <= 16) {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping memory < 16 entries %s, end index=%d, start index=%d\n"), SOC_MEM_NAME(unit, mem), end_index, start_index));
        goto done;
    }

    if (SOC_MEM_IS_ARRAY(unit,mem)) {
        start_array_index = parse_memory_array_index(unit, mem, "min");
        end_array_index = parse_memory_array_index(unit, mem, "max");
    } else {
        start_array_index = 0;
        end_array_index = 0;
    }

    LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Writing memory: table %s\n"),SOC_MEM_UFNAME(unit, mem)));
    sal_sprintf(print_str,"Writing memory: table %s\n",SOC_MEM_UFNAME(unit, mem));
    tr8_write_dump(print_str);

    if ((data_write_inject = sal_alloc(CMIC_SCHAN_WORDS(unit)*sizeof(uint32), "data_write_inject")) == NULL) {
        cli_out("dpp_mem_test_flipflop_iter_callback , data_write_inject: Out of memory\n");
        rv = -1;
        goto done;
    }

    for (i_array = start_array_index; i_array <= end_array_index; i_array++) {
        for(i_index = start_index; i_index <= end_index ; i_index++) {
            if ((fault_inject==1) && (i_index == start_index)) {
                int i;
                for (i=0; i< CMIC_SCHAN_WORDS(unit); i++) {
                   data_write_inject[i] =data_write[i]+1;
                }
                rv = soc_mem_array_write(unit, mem, i_array, COPYNO_ALL, i_index, data_write_inject);
            } else {
                rv = soc_mem_array_write(unit, mem, i_array, COPYNO_ALL, i_index, data_write);
            }
            if (rv < 0) {
                cli_out("Write ERROR: table %s.%s[%d]: %s\n",
                        SOC_MEM_UFNAME(unit, mem),
                        SOC_BLOCK_NAME(unit, COPYNO_ALL), i_index, soc_errmsg(rv));
                goto done;
            }
        }
    }
done:
    if (data_write != NULL) {
        sal_free(data_write);
    }
    if (data_write_inject != NULL) {
        sal_free(data_write_inject);
    }

#endif
    return rv;
}



int
dpp_mem_test_flipflop_iter_read_compare_callback(int unit, soc_mem_t mem, void* data)
{
    int        rv = SOC_E_NONE;
#ifdef BCM_ARAD_SUPPORT
    char       print_str[250];
    uint32     *mask = NULL;
    int        i_array,i_index, i_bytes;
    unsigned int start_array_index,end_array_index,start_index, end_index;
    uint32      *data_read= NULL;
    tr8_dbase_t *tr8_ptr = (tr8_dbase_t *)data;
    uint32      *data_write_compare= NULL;
    uint32 incr_run_count = tr8_ptr->incr_run_count;

    start_index = parse_memory_index(unit, mem, "min");
    end_index = parse_memory_index(unit, mem, "max");

    /* Memory is valid for this test if it is valid, and not ro / wo. */
    if (!SOC_MEM_IS_VALID(unit,mem)) {
        return rv;
    }

    if ( (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) != 0) {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping read only memory %s\n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    if ( (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY) != 0) {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping write only memory %s\n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    /* skipping excluded memories */
    if (rval_test_skip_mem_flipflop(unit,mem) == 1) {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping excluded memory %s\n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    /* skipping memories larger than 640b */
    if (2+soc_mem_entry_words(unit, mem) > CMIC_SCHAN_WORDS(unit)) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping large size memory %s\n"), SOC_MEM_NAME(unit, mem)));
        sal_sprintf(print_str,"skipping large size memory %s\n", SOC_MEM_NAME(unit, mem));
        tr8_write_dump(print_str);
        return rv;
    }

    /* skipping memories with less or equal to 16 entries (hw request, most of it is not memories but management blocks*/
    if (end_index - start_index <= 16) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping memory < 16 entries %s, end index=%d, start index=%d\n"), SOC_MEM_NAME(unit, mem), end_index, start_index));
        return rv;
    }
    

    if (SOC_MEM_IS_ARRAY(unit, mem)) {
        start_array_index = parse_memory_array_index(unit, mem, "min");
        end_array_index = parse_memory_array_index(unit, mem, "max");
    } else {
        start_array_index = 0;
        end_array_index = 0;
    }

    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Reading memory: table %s\n"),SOC_MEM_UFNAME(unit, mem)));
    sal_sprintf(print_str,"Reading memory: table %s\n",SOC_MEM_UFNAME(unit, mem));
    tr8_write_dump(print_str);

    if ((mask = sal_alloc(CMIC_SCHAN_WORDS(unit)*sizeof(uint32), "mask")) == NULL) {
        cli_out("dpp_mem_test_flipflop_iter_read_compare_callback: mask : Out of memory\n");
        rv = -1;
        goto done;
    }
    memtest_mask_get_tr8(unit, mem, mask);

    if ((data_read = sal_alloc(CMIC_SCHAN_WORDS(unit)*sizeof(uint32), "data_read")) == NULL) {
        cli_out("dpp_mem_test_flipflop_iter_read_compare_callback: data_read : Out of memory\n");
        rv = -1;
        goto done;
    }
    if ((data_write_compare = sal_alloc(CMIC_SCHAN_WORDS(unit)*sizeof(uint32), "data_write_compare")) == NULL) {
        cli_out("dpp_mem_test_flipflop_iter_read_compare_callback: data_write_compare : Out of memory\n");
        rv = -1;
        goto done;
    }

    soc_sand_os_memset(data_read, 0x0, CMIC_SCHAN_WORDS(unit)*sizeof(uint32));
    soc_sand_os_memset(data_write_compare, tr8_ptr->write_value, CMIC_SCHAN_WORDS(unit)*sizeof(uint32));

    for (i_array = start_array_index; i_array <= end_array_index; i_array++) {
        for(i_index = start_index; i_index <= end_index ; i_index++) {
            rv = soc_mem_array_read(unit, mem, i_array, COPYNO_ALL, i_index, data_read);
            if (rv < 0) {
                cli_out("Read ERROR: table %s.%s[%d]: %s\n",
                        SOC_MEM_UFNAME(unit, mem),
                        SOC_BLOCK_NAME(unit, COPYNO_ALL), i_index, soc_errmsg(rv));
                goto done;
            }
            for (i_bytes=0; i_bytes<SOC_MEM_WORDS(unit, mem); i_bytes++) {
                tr8_increment_bits_counter(tr8_get_bits_num(mask[i_bytes]));
                if ((data_read[i_bytes] & mask[i_bytes]) != (data_write_compare[i_bytes] & mask[i_bytes])) {
                    sal_sprintf(print_str,"Compare error iteration number:%d in %s, array index: %d, entry index:%d, current word:%d total words:%d\n",
                           incr_run_count,
                           SOC_MEM_UFNAME(unit, mem),
                           i_array,
                           i_index,
                           i_bytes,
                           SOC_MEM_WORDS(unit, mem));
                    tr8_write_dump(print_str);
                    cli_out(print_str);
                    sal_sprintf(print_str,"Data write: 0x%x\nData read: 0x%x\nData mask: 0x%x\n",
                           data_write_compare[i_bytes],
                           data_read[i_bytes],
                           mask[i_bytes]);
                    tr8_write_dump(print_str);
                    cli_out(print_str);
                }
            }
        }
    }
done:
    if (mask != NULL) {
        sal_free(mask);
    }
    if (data_read != NULL) {
        sal_free(data_read);
    }
    if (data_write_compare != NULL) {
        sal_free(data_write_compare);
    }


#endif
    return rv;
}
#endif /* BCM_DPP_SUPPORT */

void memtest_mask_get(int unit, soc_mem_t mem, uint32 *mask)
{
    uint32		*tcammask = NULL;
    uint32		*eccmask = NULL;
    uint32              accum_tcammask;
    int			dw, i;

    if ((tcammask = sal_alloc(CMIC_SCHAN_WORDS(unit)*sizeof(uint32), "tcammask")) == NULL) {
        cli_out("dpp_mem_test_flipflop_iter_read_compare_callback: tcammask : Out of memory\n");
        goto done;
    }
    if ((eccmask = sal_alloc(CMIC_SCHAN_WORDS(unit)*sizeof(uint32), "eccmask")) == NULL) {
        cli_out("dpp_mem_test_flipflop_iter_read_compare_callback: eccmask : Out of memory\n");
        goto done;
    }

    for (i=0;i<CMIC_SCHAN_WORDS(unit);i++) {
        mask[i] = 0;
        tcammask[i] = 0;
        eccmask[i] = 0;
    }
    dw = soc_mem_entry_words(unit, mem);
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
    if(SOC_IS_DPP(unit) || SOC_IS_DFE(unit)) {
        soc_mem_datamask_rw_get(unit, mem, mask);
    } else
#endif
    {
        soc_mem_datamask_get(unit, mem, mask);
    }
    soc_mem_tcammask_get(unit, mem, tcammask);
    soc_mem_eccmask_get(unit, mem, eccmask);
    accum_tcammask = 0;
    for (i = 0; i < dw; i++) {
        accum_tcammask |= tcammask[i];
    }

    for (i = 0; i < dw; i++) {
      mask[i] &= ~eccmask[i];
    }
done:
    if (tcammask != NULL) {
        sal_free(tcammask);
    }
    if (eccmask != NULL) {
        sal_free(eccmask);
    }

}

void memtest_mask_get_tr8(int unit, soc_mem_t mem, uint32 *mask)
{
    uint32		*tcammask = NULL;
    uint32		*eccmask = NULL;
    uint32              accum_tcammask;
    int			dw, i;

    if ((tcammask = sal_alloc(CMIC_SCHAN_WORDS(unit)*sizeof(uint32), "tcammask")) == NULL) {
        cli_out("dpp_mem_test_flipflop_iter_read_compare_callback: tcammask : Out of memory\n");
        goto done;
    }
    if ((eccmask = sal_alloc(CMIC_SCHAN_WORDS(unit)*sizeof(uint32), "eccmask")) == NULL) {
        cli_out("dpp_mem_test_flipflop_iter_read_compare_callback: eccmask : Out of memory\n");
        goto done;
    }

    for (i=0;i<CMIC_SCHAN_WORDS(unit);i++) {
        mask[i] = 0;
        tcammask[i] = 0;
        eccmask[i] = 0;
    }
    dw = soc_mem_entry_words(unit, mem);

    soc_mem_datamask_rw_get(unit, mem, mask);
    soc_mem_tcammask_get(unit, mem, tcammask);
    soc_mem_eccmask_get(unit, mem, eccmask);
    accum_tcammask = 0;
    for (i = 0; i < dw; i++) {
        accum_tcammask |= tcammask[i];
    }

    for (i = 0; i < dw; i++) {
      mask[i] &= ~eccmask[i];
    }
    /* exceptions:*/
    switch (mem) {
        case IQM_PDMm:
            mask[0]= 0xffffffff;
            mask[1]= 0x1fc001;
            break;
        case SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm:
            mask[0]= 0xffffffff;
            mask[1]= 0xff;
            break;
        default:
            break;

    }
done:
    if (tcammask != NULL) {
        sal_free(tcammask);
    }
    if (eccmask != NULL) {
        sal_free(eccmask);
    }

}


#ifdef BCM_DPP_SUPPORT
cmd_result_t
do_mem_test_rw(int unit, tr7_dbase_t tr7_data, args_t *a)
{
    int rv = SOC_E_NONE;
    if ((soc_mem_iterate(unit,
                         dpp_mem_test_rw_iter_callback, &tr7_data)) < 0) {
        rv = CMD_FAIL;
    }

    return rv;
}

cmd_result_t
do_mem_test_flipflop(int unit, tr8_dbase_t tr8_data, args_t *a)
{
  int rv = SOC_E_NONE;
#ifdef BCM_ARAD_SUPPORT
  char print_str[250];
    if (tr8_data.skip_reset_and_write  != 1) {
        sal_sprintf(print_str,"****************Write pattern to all writable memories...\n");
        cli_out(print_str);
        tr8_write_dump(print_str);

        if ((soc_mem_iterate(unit,
                             dpp_mem_test_flipflop_iter_callback, &tr8_data)) < 0) {
            rv = CMD_FAIL;
        }
    } else {
        sal_sprintf(print_str,"****************Skip Write pattern...\n");
        cli_out(print_str);
        tr8_write_dump(print_str);
    }

    tr8_data.incr_run_count = 1;
    while (tr8_data.run_count > 0) {
        sal_sprintf(print_str,"****************Sleep...\n");
        cli_out(print_str);
        tr8_write_dump(print_str);

        sal_sleep(tr8_data.period);

        sal_sprintf(print_str,"****************Read & compare pattern to all writable memories, Iteration #%d...\n", tr8_data.incr_run_count);
        cli_out(print_str);
        tr8_write_dump(print_str);

        tr8_reset_bits_counter();
        if ((soc_mem_iterate(unit,
                             dpp_mem_test_flipflop_iter_read_compare_callback, &tr8_data)) < 0) {
            rv = CMD_FAIL;
        }
        {
            uint32 bit_count_num = tr8_bits_counter_get();
            sal_sprintf(print_str,"Iteration #%d : Bits processed : %u\n",tr8_data.incr_run_count, bit_count_num);
            cli_out(print_str);
            tr8_write_dump(print_str);

        }
        if (tr8_data.run_count != 0xffffffff) {
            tr8_data.run_count--;
        }
        tr8_data.incr_run_count++;
    }
#endif
    return rv;
}
#endif /* BCM_DPP_SUPPORT */

cmd_result_t
do_dump_memories(int unit, args_t *a)
{
    char    *an_arg;
    int     flags = DUMP_TABLE_ALL, rv = CMD_OK;

    an_arg = ARG_GET(a);

    while (an_arg) {
        if (sal_strcasecmp(an_arg, "diff") == 0) {
            flags = DUMP_TABLE_CHANGED;
        } else {
            cli_out("ERROR: unrecognized argument to DUMP SOC: %s\n", an_arg);
            return CMD_FAIL;
        }
        an_arg = ARG_GET(a);
    }

    if ((soc_mem_iterate(unit, dpp_mem_dump_iter_callback, INT_TO_PTR(flags))) < 0) {
        rv = CMD_FAIL;
    }

    return rv;
}
int
dpp_mem_dump_iter_callback(int unit, soc_mem_t mem, void *data)
{
    int        copyno, index_min, count;
    int        rv = SOC_E_NONE;
    int        flags = PTR_TO_INT(data);

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        return rv;
    }

    if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_DEBUG) {
        LOG_VERBOSE(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "skipping debug memory %s\n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY) {
        LOG_VERBOSE(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "skipping Write only memory %s\n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    if (2+soc_mem_entry_words(unit, mem) > CMIC_SCHAN_WORDS(unit)) {
        LOG_VERBOSE(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "skipping large size memory %s\n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }
#ifdef BCM_ARAD_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        switch(mem) {
            case IHB_TCAM_BANK_COMMANDm:
            case IHB_TCAM_BANK_REPLYm:
            case BRDC_FSRD_FSRD_WL_EXT_MEMm:
            case IQM_MEM_8000000m:
            case NBI_TBINS_MEMm:
            case NBI_RBINS_MEMm:
            case PORT_WC_UCMEM_DATAm:
                LOG_VERBOSE(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "skipping excluded memory %s\n"), SOC_MEM_NAME(unit, mem)));
                return rv;
            default:
                break;
        }
    }
#endif
    index_min = soc_mem_index_min(unit, mem);
    count = soc_mem_index_count(unit, mem);

    SOC_MEM_BLOCK_ITER(unit, mem, copyno) {
        /* Bypass dumping MMU memories. */
        if (SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_MMU) {
            LOG_VERBOSE(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "skipping MMU block memory %s\n"), SOC_MEM_NAME(unit, mem)));
            continue;
        }

      /* If changed to 0, see above '#if 0's as well. */
        LOG_VERBOSE(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "testing memory %s\n"), SOC_MEM_NAME(unit, mem)));
        if ((do_dump_table_single(unit, mem, copyno, index_min, count, flags)) != CMD_OK) {
            rv = SOC_E_INTERNAL;
            /* break; */
        }
    }

    return rv;
}


STATIC cmd_result_t
do_dump_table_single(int unit, soc_mem_t mem,
                     int copyno, int index, int count, int flags)
{
    int         k, i;
    uint32      entry[SOC_MAX_MEM_WORDS];
    char        lineprefix[256];
    int         entry_dw;
    int         rv = CMD_FAIL;

    assert(copyno >= 0);

    entry_dw = soc_mem_entry_words(unit, mem);

    for (k = index; k < index + count; k++) {
        i = soc_mem_read(unit, mem, copyno, k, entry);
        if (i < 0) {
            cli_out("Read ERROR: table %s.%s[%d]: %s\n", SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), k, soc_errmsg(i));
            goto done;
        }

        if (flags & DUMP_TABLE_HEX) {
            for (i = 0; i < entry_dw; i++) {
            /*        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%08x\n", entry[i]); */
                LOG_VERBOSE(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "%08x\n"), entry[i]));
            }
        } else if (flags & DUMP_TABLE_CHANGED) {
            sal_sprintf(lineprefix, "%s.%s[%d]: ", SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), k);
            soc_mem_entry_dump_if_changed(unit, mem, entry, lineprefix);
        } else {
            /* bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"%s.%s[%d]: ", */
            LOG_VERBOSE(BSL_LS_APPL_COMMON, (BSL_META_U(unit,"%s.%s[%d]: "), SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), k));

            if (flags & DUMP_TABLE_RAW) {
                for (i = 0; i < entry_dw; i++) {
                    /* bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"0x%08x ", entry[i]); */
                    LOG_VERBOSE(BSL_LS_APPL_COMMON, (BSL_META_U(unit,"0x%08x "), entry[i]));
                }
            } else {
                if(bsl_check(bslLayerAppl, bslSourceCommon, bslSeverityVerbose, unit)) {
                    soc_mem_entry_dump(unit, mem, entry);
                }
            }

            /*  bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"\n"); */
            LOG_VERBOSE(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "\n")));
        }
    }

    rv = CMD_OK;

 done:
    return rv;
}

