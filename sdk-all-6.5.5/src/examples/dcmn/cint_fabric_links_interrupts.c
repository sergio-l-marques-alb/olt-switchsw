
/*
 * $Id: cint_fabric_links_interrupts.c Exp $
 *
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
 *
 *  mask FMAC/FSRD interrupts examples
 */

int mask_fmac_interrupts(int unit, int index)
{
    int rv, i;
    bcm_switch_event_t switch_event;
    bcm_switch_event_control_t type;
    uint32 value = 0;
    switch_event = BCM_SWITCH_EVENT_DEVICE_INTERRUPT;
    type.index = index;
    type.action = bcmSwitchEventMask;

    /*
     * JER_INT_FMAC_ERROR_ECC = 501
     * JER_INT_FMAC_ERROR_ECC = 501,
     * JER_INT_FMAC_INT_REG_1 = 502,
     * JER_INT_FMAC_INT_REG_2 = 503,
     * JER_INT_FMAC_INT_REG_3 = 504,
     * JER_INT_FMAC_INT_REG_4 = 505,
     * JER_INT_FMAC_INT_REG_5 = 506,
     * JER_INT_FMAC_INT_REG_6 = 507,
     * JER_INT_FMAC_INT_REG_7 = 508,
     * JER_INT_FMAC_INT_REG_8 = 509,
     * JER_INT_FMAC_ECC_PARITY_ERR_INT = 510,
     * JER_INT_FMAC_ECC_ECC_1B_ERR_INT = 511,
     * JER_INT_FMAC_ECC_ECC_2B_ERR_INT = 512,
     */
    int event_ids[12] = {501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512};
    int event_num = 12;

    for (i=0 ; i < event_num ; i++)
    {
        type.event_id = event_ids[i];
        rv = bcm_switch_event_control_set(unit, switch_event, type, value);
        if(rv != 0) {
            printf("Error, in init_fmac_interrupts, FMAC %d\n", index);
            return rv;
        }
    }

    return 0;
}

int mask_fsrd_interrupts(int unit, int index)
{
    int rv, i;
    bcm_switch_event_t switch_event;
    bcm_switch_event_control_t type;
    uint32 value = 0;
    switch_event = BCM_SWITCH_EVENT_DEVICE_INTERRUPT;
    type.index = index;
    type.action = bcmSwitchEventMask;

    /*
     * JER_INT_FSRD_ERROR_ECC = 1344
     * JER_INT_FSRD_ECC_PARITY_ERR_INT = 1348,
     * JER_INT_FSRD_ECC_ECC_1B_ERR_INT = 1349,
     * JER_INT_FSRD_ECC_ECC_2B_ERR_INT = 1350,
     *
     */
    int event_ids[4] = {1344, 1348, 1349, 1350};
    int event_num = 4;

    for (i=0 ; i < event_num ; i++)
    {
        type.event_id = event_ids[i];
        rv = bcm_switch_event_control_set(unit, switch_event, type, value);
        if(rv != 0) {
            printf("Error, in init_fmac_interrupts, FMAC %d\n", index);
            return rv;
        }
    }

    return 0;
}

