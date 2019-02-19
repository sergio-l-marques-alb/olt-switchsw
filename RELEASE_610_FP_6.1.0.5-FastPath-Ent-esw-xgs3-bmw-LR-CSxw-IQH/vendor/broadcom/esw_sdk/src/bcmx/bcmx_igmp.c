/*
 * $Id: bcmx_igmp.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * File:	src/bcmx/igmp.c
 * Purpose:	IGMP Snooping Controls
 */

#include <bcm/types.h>

#include <bcmx/igmp.h>
#include <bcmx/bcmx.h>

#include "bcmx_int.h"

int
bcmx_igmp_snooping_init(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_igmp_snooping_init(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

int
bcmx_igmp_snooping_enable_set(int enable)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_igmp_snooping_enable_set(bcm_unit, enable);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

int
bcmx_igmp_snooping_enable_get(int *enable)
{
    int rv, i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_igmp_snooping_enable_get(bcm_unit, enable);
	if (rv >= 0) {
	    return rv;
	}
    }

    return BCM_E_UNAVAIL;
}
