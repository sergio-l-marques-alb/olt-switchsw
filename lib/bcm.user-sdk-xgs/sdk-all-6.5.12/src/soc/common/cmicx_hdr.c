/*
 * $Id: cmicx_hdr.c,v 1.00 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Purpose:     CMICX driver for EP_2_CPU_HDR
 *
 */

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#include <soc/debug.h>
#include <soc/dma.h>
#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/dcbformats.h>

#ifdef BCM_CMICX_SUPPORT

#if defined(BCM_TRIDENT3_SUPPORT)
static void
soc_dma_ep_2_cpu_hdr36_decoded_dump(int unit, char *prefix, void *addr, int skip_mhdr, int hdr_size)
{
    uint8       *c;
    ep_to_cpu_hdr36_t *h = (ep_to_cpu_hdr36_t *)addr;

#if defined(LE_HOST)
    int         word;
    uint32 *hdr_data = (uint32 *)addr;
    for (word = 0; word < BYTES2WORDS(hdr_size); word++) {
        hdr_data[word] = _shr_swap32(hdr_data[word]);
    }
#endif
    c = (uint8 *)addr;

    if (!skip_mhdr) {
        soc_dma_higig_dump(unit, prefix, c, 0, 0, NULL);
    }

    LOG_CLI((BSL_META_U(unit,
                        "%s\t%schg_tos %schg_ecn\n"),
             prefix,
             h->chg_tos ? "" : "!",
             h->word8.overlay1.chg_ecn ? "" : "!"));

    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sservice_tag %sswitch_pkt %suc_switch_drop %ssrc_hg\n"),
             prefix,
             h->service_tag ? "" : "!",
             h->switch_pkt ? "" : "!",
             h->uc_switch_drop ? "" : "!",
             h->src_hg ? "" : "!"));

    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sl3routed %sl3only %sreplicated %sdo_not_change_ttl "
             "%sbpdu\n"),
             prefix,
             h->l3routed ? "" : "!",
             h->l3only ? "" : "!",
             h->replicated ? "" : "!",
             h->do_not_change_ttl ? "" : "!",
             h->bpdu ? "" : "!"));

    switch (h->word16.overlay1.eh_type) {
        case 0:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 0: queue_tag=0x%04x "
                     "tag_type=%d seg_sel=%d\n"),
                     prefix,
                     h->word16.overlay1.eh_queue_tag,
                     h->word16.overlay1.eh_tag_type,
                     h->word16.overlay1.eh_seg_sel));
            break;
        case 1:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 1: classid=%d l3_iif=%d "
                     "classid_type=%d\n"),
                     prefix,
                     h->word16.overlay2.classid,
                     h->word16.overlay2.l3_iif,
                     h->word16.overlay2.classid_type));
            break;
        case 2:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 2: queue_tag=0x%04x classid=%d "
                     "classid_type=%d\n"),
                     prefix,
                     h->word16.overlay3.eh_queue_tag,
                     h->word16.overlay3.classid,
                     h->word16.overlay3.classid_type));
            break;
        default:
            break;
    }

    LOG_CLI((BSL_META_U(unit,
                        "%s\treason=%08x_%08x\n"),
             prefix,
             h->reason_hi,
             h->reason));

    LOG_CLI((BSL_META_U(unit,
                        "%s\tts_type=%d timestamp=%08x\n"),
             prefix,
             h->timestamp_type,
             h->timestamp));

    LOG_CLI((BSL_META_U(unit,
                        "%s\tsrcport=%d cpu_cos=%d pkt_len=%d forwarding_type=%d "
             "ucq=%05x\n"),
             prefix,
             h->word8.overlay1.srcport,
             h->word8.overlay1.cpu_cos,
             h->word8.overlay1.pkt_len,
             h->forwarding_type,
             h->ucq));

    LOG_CLI((BSL_META_U(unit,
                        "%s\touter_vid=%d outer_cfi=%d outer_pri=%d otag_action=%d "
             "vntag_action=%d\n"),
             prefix,
             h->outer_vid,
             h->outer_cfi,
             h->word6.overlay1.outer_pri,
             h->otag_action,
             h->vntag_action));

    LOG_CLI((BSL_META_U(unit,
                        "%s\tinner_vid=%d inner_cfi=%d inner_pri=%d "
             "decap = %d itag_action=%d\n"),
             prefix,
             h->inner_vid,
             h->word6.overlay1.inner_cfi,
             h->word6.overlay1.inner_pri,
             h->decap_tunnel_type,
             h->itag_action));

    LOG_CLI((BSL_META_U(unit,
                        "%s\tdscp=%d ecn=%d decap_tunnel_type=%d match_rule=%d "
             "mtp_ind=%d\n"),
             prefix,
             h->dscp,
             h->word8.overlay1.ecn,
             h->decap_tunnel_type,
             h->match_rule,
             h->mtp_index));
}
#endif

void soc_dma_ep_to_cpu_hdr_decoded_dump(int unit, char *pfx, void *addr,
                                int skip_mhdr, int hdr_size)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_dma_ep_2_cpu_hdr36_decoded_dump(unit, pfx, addr, skip_mhdr, hdr_size);
    }
#endif
}

#endif /* BCM_CMICX_SUPPORT */
