/*
 * $Id: circ_cmd_buffer.c,v 1.3 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * Purpose: Thread safe circular buffer 
 */

#include <shared/bsl.h>

#include <soc/types.h>
#include <soc/drv.h>

#ifdef BCM_CALADAN3_SUPPORT
#include <shared/util.h>
#include <sal/appl/sal.h>
#include <soc/sbx/caladan3/tmu/circ_cmd_buffer.h>

int circ_cmd_buffer_init(int unit, circ_cmd_buffer_t *cbuf)
{
    if (cbuf == NULL) {
        return SOC_E_PARAM;
    }

    cbuf->read_pos = cbuf->write_pos = 0;
    cbuf->length = 0;
    cbuf->buffer = NULL;

    cbuf->mutex = sal_mutex_create("CIRC_BUF_MUTEX");
    if (cbuf->mutex == NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "failed to create mutex\n")));
        return SOC_E_RESOURCE;
    }

    return SOC_E_NONE;
}

int circ_cmd_buffer_initFromBuf(int unit, circ_cmd_buffer_t *cbuf,
                                soc_sbx_caladan3_tmu_cmd_t **buffer, int length)
{
    int status;

    if (cbuf == NULL || buffer == NULL || length <= 0) {
        return SOC_E_PARAM;
    }

    status = circ_cmd_buffer_init(unit, cbuf);

    if (status == SOC_E_NONE) {
        cbuf->buffer = buffer;
        cbuf->length = length;
    }

    return status;
}

int circ_cmd_buffer_destroy(int unit, circ_cmd_buffer_t *cbuf)
{
    if (cbuf == NULL) {
        return SOC_E_PARAM;
    }

    if(cbuf->mutex) {
        sal_mutex_destroy(cbuf->mutex);
    }
    return SOC_E_NONE;
}

int circ_cmd_buffer_full(int unit, circ_cmd_buffer_t *cbuf)
{
    uint8 full=FALSE;
    sal_mutex_take(cbuf->mutex, sal_mutex_FOREVER);
    full = (((cbuf->write_pos+1)%cbuf->length) == cbuf->read_pos)? TRUE: FALSE; 
    sal_mutex_give(cbuf->mutex);
    return full;
}

int circ_cmd_buffer_empty(int unit, circ_cmd_buffer_t *cbuf)
{
    uint8 empty=FALSE;
    sal_mutex_take(cbuf->mutex, sal_mutex_FOREVER);
    empty = (cbuf->read_pos == cbuf->write_pos)? TRUE: FALSE; 
    sal_mutex_give(cbuf->mutex);
    return empty;
}

int circ_cmd_buffer_put(int unit, circ_cmd_buffer_t *cbuf, soc_sbx_caladan3_tmu_cmd_t *element, uint8 overflow)
{
    int status = SOC_E_NONE;

    if (overflow || !circ_cmd_buffer_full(unit, cbuf)) {
        sal_mutex_take(cbuf->mutex, sal_mutex_FOREVER);
        cbuf->buffer[cbuf->write_pos] = element;
        cbuf->write_pos++;
        cbuf->write_pos %= cbuf->length;
        sal_mutex_give(cbuf->mutex);
    } else {
        status = SOC_E_FULL;
    }

    return status;
}

int circ_cmd_buffer_get(int unit, circ_cmd_buffer_t *cbuf, soc_sbx_caladan3_tmu_cmd_t **element)
{
    int status = SOC_E_NONE;

    if (!circ_cmd_buffer_empty(unit, cbuf)) {
        sal_mutex_take(cbuf->mutex, sal_mutex_FOREVER);
        *element = cbuf->buffer[cbuf->read_pos];
        cbuf->read_pos++;
        cbuf->read_pos %= cbuf->length;
        sal_mutex_give(cbuf->mutex);
    } else {
        status = SOC_E_EMPTY;
    }

    return status;
}

void circ_cmd_buffer_printf(int unit, circ_cmd_buffer_t *cbuf)
{
    int index;
    sal_mutex_take(cbuf->mutex, sal_mutex_FOREVER);
    LOG_CLI((BSL_META_U(unit,
                        "-- Circ Buffer Fill[%d] Empty[%d] Capacity[%d] --\n"),
             circ_cmd_buffer_full(unit,cbuf),
             circ_cmd_buffer_empty(unit,cbuf),
             cbuf->length-1));

    if (circ_cmd_buffer_empty(unit,cbuf)) return;

    for (index=0; index < cbuf->length; index++) {
        if (cbuf->buffer[index] != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "\n Index= %d: %p"), index, (void *)cbuf->buffer[index]));
            /*tmu_cmd_printf(unit, cbuf->buffer[index]);*/
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    sal_mutex_give(cbuf->mutex);
}

#endif
