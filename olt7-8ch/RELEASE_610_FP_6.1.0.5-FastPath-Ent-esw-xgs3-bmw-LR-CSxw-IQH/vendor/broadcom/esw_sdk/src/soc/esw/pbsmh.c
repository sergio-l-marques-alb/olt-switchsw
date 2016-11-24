/*
 * $Id: pbsmh.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * Utility routines for managing pbsmh headers
 * These are mainly used for debug purposes
 */

#include <assert.h>
#include <sal/core/libc.h>

#include <soc/types.h>
#include <soc/drv.h>
#include <soc/pbsmh.h>
#include <soc/cm.h>
#include <soc/dcb.h>
#include <soc/debug.h>

#ifdef BCM_XGS3_SWITCH_SUPPORT
static char *soc_pbsmh_field_names[] = {
    /* NOTE: strings must match soc_pbsmh_field_t */
    "start",
    "src_mod",
    "dst_port",
    "cos",
    "pri",
    "l3pbm_sel",
    NULL
};

soc_pbsmh_field_t
soc_pbsmh_name_to_field(int unit, char *name)
{
    soc_pbsmh_field_t           f;

    assert(COUNTOF(soc_pbsmh_field_names) - 1 == PBSMH_COUNT);

    COMPILER_REFERENCE(unit);

    for (f = 0; soc_pbsmh_field_names[f] != NULL; f++) {
        if (sal_strcmp(name, soc_pbsmh_field_names[f]) == 0) {
            return f;
        }
    }

    return PBSMH_invalid;
}

char *
soc_pbsmh_field_to_name(int unit, soc_pbsmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    assert(COUNTOF(soc_pbsmh_field_names) == PBSMH_COUNT);

    if (field < 0 || field >= PBSMH_COUNT) {
        return "??";
    } else {
        return soc_pbsmh_field_names[field];
    }
}

static void
soc_pbsmh_v1_field_set(int unit, soc_pbsmh_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:   mh->start = val;
                        mh->_rsvd0 = 0;
                        mh->_rsvd1 = 0;
                        mh->_rsvd2 = 0;
                        break;
    case PBSMH_src_mod: mh->src_mod = val; break;
    case PBSMH_dst_port:mh->dst_port = val;break;
    case PBSMH_cos:     mh->cos = val;break;
    default:
        soc_cm_debug(DK_WARN,
                     "pbsmh_set: unit %d: Unknown pbsmh field=%s val=0x%x\n",
                     unit, soc_pbsmh_field_to_name(unit, field), val);
        break;
    }
}

static uint32
soc_pbsmh_v1_field_get(int unit, soc_pbsmh_hdr_t *mh, soc_pbsmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:       return mh->start;
    case PBSMH_src_mod:     return mh->src_mod; 
    case PBSMH_dst_port:    return mh->dst_port;
    case PBSMH_cos:         return mh->cos;
    default:
        soc_cm_debug(DK_WARN,
                     "pbsmh_get: unit %d: Unknown pbsmh field=%d\n",
                     unit, field);
        return 0;
    }
}

static void
soc_pbsmh_v2_field_set(int unit, soc_pbsmh_v2_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:           mh->start = val;
                                mh->_rsvd0 = 0;
                                mh->_rsvd1 = 0;
                                mh->_rsvd2 = 0;
                                break;
    case PBSMH_src_mod:         mh->src_mod = val; break;
    case PBSMH_dst_port:        mh->dst_port = val; break;
    case PBSMH_cos:             mh->cos = val; break;
    case PBSMH_pri:             mh->pri = val; break;
    case PBSMH_l3pbm_sel:       mh->l3pbm_sel = val; break;
    default:
        soc_cm_debug(DK_WARN,
                     "pbsmh_set: unit %d: Unknown pbsmh field=%d val=0x%x\n",
                     unit, field, val);
        break;
    }
}

static uint32
soc_pbsmh_v2_field_get(int unit,
                       soc_pbsmh_v2_hdr_t *mh,
                       soc_pbsmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:       return mh->start;
    case PBSMH_src_mod:     return mh->src_mod; 
    case PBSMH_dst_port:    return mh->dst_port;
    case PBSMH_cos:         return mh->cos;
    case PBSMH_pri:         return mh->pri;
    case PBSMH_l3pbm_sel:   return mh->l3pbm_sel;
    default:
        soc_cm_debug(DK_WARN,
                     "pbsmh_get: unit %d: Unknown pbsmh field=%d\n",
                     unit, field);
        return 0;
    }
}

static void
soc_pbsmh_v3_field_set(int unit, soc_pbsmh_v3_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:           mh->start = val;
                                mh->_rsvd0 = 0;
                                mh->_rsvd1 = 0;
                                mh->_rsvd2 = 0;
                                break;
    case PBSMH_src_mod:         mh->src_mod_hi = (val >> 4); 
                                mh->src_mod_lo = (val & 0xf); 
                                break;
    case PBSMH_dst_port:        mh->dst_port = val; break;
    case PBSMH_cos:             mh->cos = val; break;
    case PBSMH_pri:             mh->pri = val; break;
    case PBSMH_l3pbm_sel:       mh->l3pbm_sel = val; break;
    default:
        soc_cm_debug(DK_WARN,
                     "pbsmh_set: unit %d: Unknown pbsmh field=%d val=0x%x\n",
                     unit, field, val);
        break;
    }
}

static uint32
soc_pbsmh_v3_field_get(int unit,
                       soc_pbsmh_v3_hdr_t *mh,
                       soc_pbsmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:       return mh->start;
    case PBSMH_src_mod:     return ((mh->src_mod_hi << 4) | (mh->src_mod_lo)); 
    case PBSMH_dst_port:    return mh->dst_port;
    case PBSMH_cos:         return mh->cos;
    case PBSMH_pri:         return mh->pri;
    case PBSMH_l3pbm_sel:   return mh->l3pbm_sel;
    default:
        soc_cm_debug(DK_WARN,
                     "pbsmh_get: unit %d: Unknown pbsmh field=%d\n",
                     unit, field);
        return 0;
    }
}

static void
soc_pbsmh_v4_field_set(int unit, soc_pbsmh_v4_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:           mh->start = val;
                                mh->_rsvd0 = 0;
                                mh->_rsvd1 = 0;
                                mh->_rsvd2 = 0;
                                break;
    case PBSMH_src_mod:         mh->src_mod_hi = (val >> 7); 
                                mh->src_mod_lo = (val & 0x7f); 
                                break;
    case PBSMH_dst_port:        mh->dst_port = val; break;
    case PBSMH_cos:             mh->cos = val; break;
    case PBSMH_pri:             mh->pri_hi = (val >> 3); 
                                mh->pri_lo = (val & 0x7); 
                                break;
    case PBSMH_l3pbm_sel:       mh->l3pbm_sel = val; break;
    default:
        soc_cm_debug(DK_WARN,
                     "pbsmh_set: unit %d: Unknown pbsmh field=%d val=0x%x\n",
                     unit, field, val);
        break;
    }
}

static uint32
soc_pbsmh_v4_field_get(int unit,
                       soc_pbsmh_v4_hdr_t *mh,
                       soc_pbsmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:       return mh->start;
    case PBSMH_src_mod:     return ((mh->src_mod_hi << 7) | (mh->src_mod_lo)); 
    case PBSMH_dst_port:    return mh->dst_port;
    case PBSMH_cos:         return mh->cos;
    case PBSMH_pri:         return ((mh->pri_hi << 3) | (mh->pri_lo));
    case PBSMH_l3pbm_sel:   return mh->l3pbm_sel;
    default:
        soc_cm_debug(DK_WARN,
                     "pbsmh_get: unit %d: Unknown pbsmh field=%d\n",
                     unit, field);
        return 0;
    }
}

void
soc_pbsmh_field_set(int unit, soc_pbsmh_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    switch(SOC_DCB_TYPE(unit)) {
    case 9:
    case 10:
    case 13:
        soc_pbsmh_v1_field_set(unit, mh, field, val);
        break;
    case 11:
    case 12:
    case 15:
    case 17:
    case 18:
        {
            soc_pbsmh_v2_hdr_t *mhv2 = (soc_pbsmh_v2_hdr_t *)mh;
            soc_pbsmh_v2_field_set(unit, mhv2, field, val);
            break;
        }
    case 14:
    case 19:
    case 20:
        {
            soc_pbsmh_v3_hdr_t *mhv3 = (soc_pbsmh_v3_hdr_t *)mh;
            soc_pbsmh_v3_field_set(unit, mhv3, field, val);
            break;
        }
    case 16:
        {
            soc_pbsmh_v4_hdr_t *mhv4 = (soc_pbsmh_v4_hdr_t *)mh;
            soc_pbsmh_v4_field_set(unit, mhv4, field, val);
            break;
        }
    default:
        break;
    }
}

uint32
soc_pbsmh_field_get(int unit, soc_pbsmh_hdr_t *mh, soc_pbsmh_field_t field)
{
    switch(SOC_DCB_TYPE(unit)) {
    case 9:
    case 10:
    case 13:
        return soc_pbsmh_v1_field_get(unit, mh, field);
        break;
    case 11:
    case 12:
    case 15:
    case 17:
    case 18:
        {
            soc_pbsmh_v2_hdr_t *mhv2 = (soc_pbsmh_v2_hdr_t *)mh;
            return soc_pbsmh_v2_field_get(unit, mhv2, field);
            break;
        }
    case 14:
    case 19:
    case 20:
        {
            soc_pbsmh_v3_hdr_t *mhv3 = (soc_pbsmh_v3_hdr_t *)mh;
            return soc_pbsmh_v3_field_get(unit, mhv3, field);
            break;
        }
    case 16:
        {
            soc_pbsmh_v4_hdr_t *mhv4 = (soc_pbsmh_v4_hdr_t *)mh;
            return soc_pbsmh_v4_field_get(unit, mhv4, field);
            break;
        }
    default:
        return 0;
    }
}

void
soc_pbsmh_dump(int unit, char *pfx, soc_pbsmh_hdr_t *mh)
{
    soc_cm_print("%s<START=0x%02x COS=%d PRI=%d L3PBM_SEL=%d "
                 "SRC_MODID=%d DST_PORT=%d>\n",
                 pfx,
                 soc_pbsmh_field_get(unit, mh, PBSMH_start),
                 soc_pbsmh_field_get(unit, mh, PBSMH_cos),
                 soc_pbsmh_field_get(unit, mh, PBSMH_pri),
                 soc_pbsmh_field_get(unit, mh, PBSMH_l3pbm_sel),
                 soc_pbsmh_field_get(unit, mh, PBSMH_src_mod),
                 soc_pbsmh_field_get(unit, mh, PBSMH_dst_port));
}

#endif /* BCM_XGS3_SWITCH_SUPPORT */
