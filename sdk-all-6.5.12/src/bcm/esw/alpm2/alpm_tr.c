/*
 * $Id: alpm_tr.c $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * File:    alpm_tr.c
 * Purpose: ALPM trace & log
 */

#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/format.h>
#include <soc/lpm.h>
#include <soc/trident2.h>
#include <soc/tomahawk.h>
#include <shared/bsl.h>

#include <shared/util.h>
#include <shared/l3.h>

#if defined(ALPM_ENABLE)

#include <soc/esw/trie.h>
#include <soc/esw/alpm_trie_v6.h>
#include <bcm/l3.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/alpm.h>
#include <bcm_int/esw/alpm_util.h>

typedef struct _alpm_log_s {
    uint16  op;
    uint16  sublen;
    int     vrf;
    int     nh_idx;
    uint32  flags;
    union u {
        bcm_ip_t    ip;
        bcm_ip6_t   ip6;
    } u;
} _alpm_log_t;

typedef struct _alpm_trace_s {
    int inited;
    int trace_en;
    int sanity_en;  /* trace sanity freq_cnt */
    int wrap_en;
    int wrap;       /* log wrapped TRUE/FALSE */
    int cnt;
    _alpm_log_t *buf;
    _alpm_log_t *curr;
    _alpm_log_t *start;
} _alpm_trace_t;

#define ALPM_TRACE_SIZE   0x400000
static _alpm_trace_t *alpm_trace[SOC_MAX_NUM_DEVICES];

#define ALPMTR(u)           (alpm_trace[u])
#define ALPMTR_BUF(u)       (ALPMTR(u)->buf)
#define ALPMTR_START(u)     (ALPMTR(u)->start)
#define ALPMTR_CURR(u)      (ALPMTR(u)->curr)
#define ALPMTR_CNT(u)       (ALPMTR(u)->cnt)
#define ALPMTR_WRAP(u)      (ALPMTR(u)->wrap)
#define ALPMTR_TRACE_EN(u)  (ALPMTR(u)->trace_en)
#define ALPMTR_SANITY_EN(u) (ALPMTR(u)->sanity_en)
#define ALPMTR_WRAP_EN(u)   (ALPMTR(u)->wrap_en)
#define ALPMTR_INITED(u)    (ALPMTR(u)->inited)

/* ALPM trace log enable */
int
alpm_trace_set(int u, int val)
{
    if (ALPMTR(u) == NULL) {
        cli_out("alpm trace not initialized yet\n");
        return BCM_E_INIT;
    }

    ALPMTR_TRACE_EN(u) = val;
    return BCM_E_NONE;
}

/* ALPM trace wrap enable */
int
alpm_trace_wrap_set(int u, int val)
{
    if (ALPMTR(u) == NULL) {
        cli_out("alpm trace not initialized yet\n");
        return BCM_E_INIT;
    }

    ALPMTR_WRAP_EN(u) = val;
    return BCM_E_NONE;
}

/* ALPM trace log clear */
int
alpm_trace_clear(int u)
{
    if (ALPMTR(u) == NULL) {
        cli_out("alpm trace not initialized yet\n");
        return BCM_E_INIT;
    }

    if (ALPMTR_INITED(u)) {
        ALPMTR_CNT(u) = 0;
        ALPMTR_CURR(u) = ALPMTR_START(u) = ALPMTR_BUF(u);
        cli_out("alpm trace log deleted\n");
    }

    return BCM_E_NONE;
}

/* ALPM trace periodic sanity check with freq_cnt (route add & delete)
   freq_cnt = -1 for show trace sanity,
   freq_cnt = 0 means trace sanity check disabled */
int
alpm_trace_sanity(int u, int freq_cnt)
{
    if (ALPMTR(u) == NULL) {
        cli_out("alpm trace not initialized yet\n");
        return BCM_E_INIT;
    }

    if (freq_cnt >= 0) {
        ALPMTR_SANITY_EN(u) = freq_cnt;
    }

    if (ALPMTR_SANITY_EN(u) == 0) {
        cli_out("alpm trace sanity disabled\n");
    } else {
        cli_out("alpm trace sanity enabled (freq_cnt %d)\n", ALPMTR_SANITY_EN(u));
    }

    return BCM_E_NONE;
}

/* ALPM trace log for defip route op (ALPM_TRACE_ADD or ALPM_TRACE_DELETE) */
int
alpm_trace_log(int u, int op, _bcm_defip_cfg_t *cfg, int nh_idx)
{
    int i, alloc_sz;
    int rv = BCM_E_NONE;

    if (ALPMTR(u) == NULL) {
        return BCM_E_INIT;
    }
    if (!ALPMTR_TRACE_EN(u)) {
        return rv;
    }
    if ((!ALPMTR_WRAP_EN(u)) && ALPMTR_WRAP(u)) {
        return rv; /* if trace wrap is disabled and next log will be wrapped */
    }

    if (!ALPMTR_INITED(u)) {
        ALPMTR_CNT(u) = 0;
        /* Use ALPM_TRACE_SIZE+1 just for reaching buf limit check */
        alloc_sz = sizeof(_alpm_log_t) * (ALPM_TRACE_SIZE + 1);
        ALPM_ALLOC_EG(ALPMTR_BUF(u), alloc_sz, "alpm trace log");
        ALPMTR_CURR(u) = ALPMTR_START(u) = ALPMTR_BUF(u);
        ALPMTR_INITED(u) = TRUE;
    }

    if (ALPMTR_BUF(u) == NULL) {
        return rv;
    }

    ALPMTR_CURR(u)->op      = op;
    ALPMTR_CURR(u)->flags   = cfg->defip_flags;
    ALPMTR_CURR(u)->vrf     = cfg->defip_vrf;
    ALPMTR_CURR(u)->nh_idx  = nh_idx;
    ALPMTR_CURR(u)->sublen  = (uint16)(cfg->defip_sub_len & 0xffff);
    if (cfg->defip_flags & BCM_L3_IP6) {
        for (i = 0; i < 16; i++) {
            ALPMTR_CURR(u)->u.ip6[i] = cfg->defip_ip6_addr[i];
        }
    } else {
        ALPMTR_CURR(u)->u.ip = cfg->defip_ip_addr;
    }

    /* When current log reaches start log (rollover), except
       the initial setting curr=start (without log cnt) */
    if ((ALPMTR_CURR(u) == ALPMTR_START(u)) && ALPMTR_CNT(u)) {
        ALPMTR_START(u)++; /* advancing start log */
        /* when start log reaches buf limit, it goes to buf[0] */
        if (ALPMTR_START(u) == &ALPMTR(u)->buf[ALPM_TRACE_SIZE]) {
            ALPMTR_START(u) = ALPMTR_BUF(u);
        }
    }

    ALPMTR_CURR(u)++;
    /* when current log reaches buf limit, it goes to buf[0] */
    if (ALPMTR_CURR(u) == &ALPMTR(u)->buf[ALPM_TRACE_SIZE]) {
        ALPMTR_CURR(u) = ALPMTR_BUF(u);
        ALPMTR_WRAP(u) = TRUE; /* next log will be wrapped */
    }

    ALPMTR_CNT(u)++;

    /* Periodic sanity check */
    if (ALPMTR_SANITY_EN(u) && ((ALPMTR_CNT(u) % ALPMTR_SANITY_EN(u)) == 0)) {
        rv = bcm_esw_alpm_sanity_check(u, 0, 0, 0);
        if (BCM_FAILURE(rv)) {
            ALPM_ERR(("ALPM trace sanity found problem at RTE_CNT %d!\n", ALPMTR_CNT(u)));
        }
    }

    return rv;

bad:
    if (ALPMTR_BUF(u) != NULL) {
        alpm_util_free(ALPMTR_BUF(u));
        ALPMTR_BUF(u) = NULL;
    }
    return rv;
}

/* Dump ALPM trace log with showflags = 0 or 1 */
int
alpm_trace_dump(int u, int showflags)
{
    int rv = BCM_E_NONE;
    _alpm_log_t *p;
    int egress_idx_min;
    char flags_st[14] = {0};
    char ip_st[SAL_IPADDR_STR_LEN]; /* only for IPv4 */
    char mask_st[SAL_IPADDR_STR_LEN];

    if (ALPMTR(u) == NULL) {
        cli_out("alpm trace not initialized yet\n");
        return BCM_E_INIT;
    }

    cli_out("### ALPM trace enable: %d wrap enable: %d wrap: %d count: %d ###\n\n",
            ALPMTR_TRACE_EN(u), ALPMTR_WRAP_EN(u),
            ALPMTR_WRAP(u), ALPMTR_CNT(u));

    if (ALPMTR_CNT(u) == 0) {
        return rv;
    }

    p = ALPMTR_START(u);

    do {
        egress_idx_min = (p->flags & BCM_L3_MULTIPATH) ? /* 200000 : 100000 */
            BCM_XGS3_MPATH_EGRESS_IDX_MIN : BCM_XGS3_EGRESS_IDX_MIN;

        if (showflags) {
            sal_sprintf(flags_st, " f=0x%08x", p->flags);
        }

        if (p->flags & BCM_L3_IP6) { /* IPv6 */
            if (p->op == ALPM_TRACE_OP_ADD) { /* l3 ip6route add */
                cli_out("l3 ip6route add vrf=%d ip=%02x%02x:%02x%02x:%02x%02x"
                        ":%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x "
                        "masklen=%d intf=%d replace=%d ecmp=%d%s\n",
                        p->vrf,
                        p->u.ip6[0],
                        p->u.ip6[1],
                        p->u.ip6[2],
                        p->u.ip6[3],
                        p->u.ip6[4],
                        p->u.ip6[5],
                        p->u.ip6[6],
                        p->u.ip6[7],
                        p->u.ip6[8],
                        p->u.ip6[9],
                        p->u.ip6[10],
                        p->u.ip6[11],
                        p->u.ip6[12],
                        p->u.ip6[13],
                        p->u.ip6[14],
                        p->u.ip6[15],
                        p->sublen,
                        p->nh_idx + egress_idx_min,
                        p->flags & BCM_L3_REPLACE ? 1 : 0,
                        p->flags & BCM_L3_MULTIPATH ? 1 : 0,
                        flags_st);
            } else { /* l3 ip6route delete */
                cli_out("l3 ip6route delete vrf=%d ip=%02x%02x:%02x%02x:"
                        "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                        "%02x%02x masklen=%d%s\n",
                        p->vrf,
                        p->u.ip6[0],
                        p->u.ip6[1],
                        p->u.ip6[2],
                        p->u.ip6[3],
                        p->u.ip6[4],
                        p->u.ip6[5],
                        p->u.ip6[6],
                        p->u.ip6[7],
                        p->u.ip6[8],
                        p->u.ip6[9],
                        p->u.ip6[10],
                        p->u.ip6[11],
                        p->u.ip6[12],
                        p->u.ip6[13],
                        p->u.ip6[14],
                        p->u.ip6[15],
                        p->sublen,
                        flags_st);
            }
        } else { /* IPv4 */
            alpm_util_fmt_ipaddr(ip_st, p->u.ip);
            alpm_util_fmt_ipaddr(mask_st, bcm_ip_mask_create(p->sublen));
            if (p->op == ALPM_TRACE_OP_ADD) { /* l3 defip add */
                cli_out("l3 defip add vrf=%d ip=%s mask=%s intf=%d replace=%d ecmp=%d%s\n",
                        p->vrf,
                        ip_st,
                        mask_st,
                        p->nh_idx + egress_idx_min,
                        p->flags & BCM_L3_REPLACE ? 1 : 0,
                        p->flags & BCM_L3_MULTIPATH ? 1 : 0,
                        flags_st);
           } else { /* l3 defip delete */
                cli_out("l3 defip delete vrf=%d ip=%s mask=%s%s\n",
                        p->vrf,
                        ip_st,
                        mask_st,
                        flags_st);
           }
        }

        p++;
        if (p == &ALPMTR(u)->buf[ALPM_TRACE_SIZE]) {
            p = ALPMTR_BUF(u);
        }

    } while (p != ALPMTR_CURR(u));

    cli_out("\n### ALPM trace enable: %d wrap enable: %d wrap: %d count: %d ###\n",
            ALPMTR_TRACE_EN(u), ALPMTR_WRAP_EN(u),
            ALPMTR_WRAP(u), ALPMTR_CNT(u));

    return rv;
}

int
bcm_esw_alpm_trace_init(int u)
{
    int rv = BCM_E_NONE;
    int alloc_sz;

    alloc_sz = sizeof(_alpm_trace_t);
    ALPM_ALLOC_EG(ALPMTR(u), alloc_sz, "ALPMTR");

    /* Init ALPMTR variables */
    ALPMTR_TRACE_EN(u) = soc_property_get(u, "alpm_trace_enable", 0);
    ALPMTR_SANITY_EN(u) = soc_property_get(u, "alpm_trace_sanity", 0);
    ALPMTR_WRAP_EN(u) = soc_property_get(u, "alpm_trace_wrap", 0);

    ALPM_INFO(("bcm_esw_alpm_trace_init: trace %d wrap %d\n",
               ALPMTR_TRACE_EN(u), ALPMTR_WRAP_EN(u)));
    return rv;
bad:
    bcm_esw_alpm_trace_deinit(u);
    return rv;
}

int
bcm_esw_alpm_trace_deinit(int u)
{
    int rv = BCM_E_NONE;

    if (ALPMTR(u) != NULL) {

        if (ALPMTR_BUF(u) != NULL) {
            alpm_util_free(ALPMTR_BUF(u));
            ALPMTR_BUF(u) = NULL;
        }

        alpm_util_free(ALPMTR(u));
        ALPMTR(u) = NULL;
    }

    ALPM_INFO(("bcm_esw_alpm_trace_deinit!\n"));
    return rv;
}

#endif /* ALPM_ENABLE */
