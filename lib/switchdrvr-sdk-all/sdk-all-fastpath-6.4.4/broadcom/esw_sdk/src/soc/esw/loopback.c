/*
 * $Id: loopback.c,v 1.25 Broadcom SDK $
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
 * Utility routines for managing loopback headers
 * These are mainly used for debug purposes
 */
#include <shared/bsl.h>
#include <assert.h>
#include <sal/core/libc.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <soc/pbsmh.h>
#include <soc/cm.h>
#include <soc/dcb.h>
#include <soc/loopback.h>

#ifdef BCM_XGS3_SWITCH_SUPPORT
static char *soc_loopback_hdr_field_names[] = {
        /* NOTE: strings must match soc_loopback_hdr_field_t */
        "start",
        "input_pri",
        "visibility",
        "pkt_profile",
        "src",
        "src_type",
        "common_hdr",
        "cng",
        "tc",
        "zero",
        NULL
};

soc_loopback_hdr_field_t
soc_loopback_hdr_name_to_field(int unit, char *name)
{
    soc_loopback_hdr_field_t           f;

    assert(COUNTOF(soc_loopback_hdr_field_names) - 1 == LBMH_COUNT);

    COMPILER_REFERENCE(unit);

    for (f = 0; soc_loopback_hdr_field_names[f] != NULL; f++) {
        if (sal_strcmp(name, soc_loopback_hdr_field_names[f]) == 0) {
            return f;
        }
    }

    return LBMH_invalid;
}

char *
soc_loopback_hdr_field_to_name(int unit, soc_loopback_hdr_field_t field)
{
    COMPILER_REFERENCE(unit);

    assert(COUNTOF(soc_loopback_hdr_field_names) == LBMH_COUNT);

    if (field < 0 || field >= LBMH_COUNT) {
        return "??";
    } else {
        return soc_loopback_hdr_field_names[field];
    }
}


void
soc_loopback_hdr_field_set(int unit, soc_loopback_hdr_t *lh,
                                  soc_loopback_hdr_field_t field, uint32 val)
{
    switch(field) {
        case LBMH_start:      lh->start      =  val; break;
        case LBMH_input_pri:  lh->input_pri  =  val; break;
        case LBMH_common_hdr: 
             lh->common_hdr0 = (val >> 1) & 0xf;
             lh->common_hdr1 = val & 0x1;
             break;
        case LBMH_src_type:   lh->source_type=  val; break;
        case LBMH_src:        
             lh->source0        = ((val >> 10  ) & 0x3f);
             lh->source1        = ((val >> 2   ) & 0xff);
             lh->source2        =  (val & 0x3);
             break;
        case LBMH_visibility: lh->visibility =  val; break;
        case LBMH_pkt_profile:lh->pkt_profile=  val; break;
        case LBMH_pp_port:    lh->pp_port    =  val; break;                    
        default:
            break;
    }
    return;

}

uint32
soc_loopback_hdr_field_get(int unit,soc_loopback_hdr_t * lh,soc_loopback_hdr_field_t field)

{
    switch(field) {
        case LBMH_start:      return lh->start;
        case LBMH_input_pri:  return lh->input_pri;
        case LBMH_visibility: return lh->visibility;
        case LBMH_pkt_profile:return lh->pkt_profile;
        case LBMH_pp_port:    return lh->pp_port;
        case LBMH_src:
            return (((lh->source2) |
                    (lh->source1 << 2) | 
                    (lh->source0 <<10))) ;
        case LBMH_src_type:   return lh->source_type;
        case LBMH_common_hdr: 
            return ((lh->common_hdr0 << 1) | (lh->common_hdr1 & 0x1));
                    
        default:
                return 0;
    }
}


void 
soc_loopback_hdr_dump(int unit, char *pfx, soc_loopback_hdr_t *lh)
{
    /* stub function for now */

    return;
}


/*
 * Function:
 *     soc_loopback_lbport_num_get
 * Description:
 *      return FIRST loopback port of the chip
 * Parameters:
 *      unit         device number
 * Return:
 *      first loopbackport on the given device
 */
int 
soc_loopback_lbport_num_get(int unit, uint32 pipe) 
{
    int port, lb_index=0,loopback_port_num=-1;
    PBMP_LB_ITER(unit, port) {
        loopback_port_num = port;
        if (pipe == lb_index) {
            break;
        }
        lb_index++;
    }
    return loopback_port_num;

}
#endif /* BCM_XGS3_SWITCH_SUPPORT*/

