/*
 * $Id: qax_ipsec.c,v 1 Broadcom SDK $
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
 * File:    qax_ipsec.c
 * Purpose: Manages IPSec block
 */

/* INCLUDE_IPSEC */
#if !defined(INCLUDE_IPSEC)

#include <shared/bsl.h>
#include <sal/compiler.h>
#include <bcm/types.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/ipsec.h>
#include <soc/portmode.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dpp/drv.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/QAX/qax_ipsec.h>


int
soc_qax_ipsec_init(int unit)
{
    return SOC_E_NONE;
}

int
soc_qax_ipsec_config_set( int unit, bcm_ipsec_config_t *config)
{

    return SOC_E_UNAVAIL;
}



int
soc_qax_ipsec_tunnel_add( int unit, uint32  flags, uint32 *tunnel_id, const bcm_ipsec_tunnel_info_t *tunnel_info)
{
    return SOC_E_UNAVAIL;

}


int
soc_qax_ipsec_sa_create( int unit, uint32 flags, uint32 *spi_id, bcm_ipsec_sa_info_t *sa)
{
    return SOC_E_UNAVAIL;
}


int
soc_qax_ipsec_sa_key_update(
        int              unit,
        uint32           flags,
        uint32           spi_id,
    bcm_ipsec_sa_keys_t *sa_keys)
{
    return SOC_E_UNAVAIL;
}

#endif/* INCLUDE_IPSEC */

