/*
 * $Id: ramon_drv.c,v 1.1.2.1 Broadcom SDK $
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
 * SOC RAMON DRV
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#include <soc/mcm/memregs.h>
#include <soc/dcmn/error.h>

    /* 
     * SBUS ring map:
     * Ring 0: OTPC(84), AVS(2)
     * Ring 2: DCH(96-103), CCH(104-111), QRH(88-95), MCT(87), RTP(86), MESH_TOPOLOGY(85), BRDC_DCH(122), BRDC_CCH(123), BRDC_QRH(126)
     * Ring 3: DCML(3-6, 8-11), BRDC_DCML(124) 
     * Ring 4: FMAC(12-59), FSRD(60-83), BRDC_MAC(120), BRDC_FSRD(121)
     * Ring 5: LCM(112-119), BRDC_LCM(125)
     * Ring 7: ECI(0), CMIC(127)
     */
int
soc_ramon_drv_rings_map_set(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x73333027));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x44443333));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x44444444));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x44444444));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x44444444));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x44444444));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0x44444444));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x44444444));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_64_71r(unit, 0x44444444));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_72_79r(unit, 0x44444444));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_80_87r(unit, 0x22204444));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_88_95r(unit, 0x22222222));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_96_103r(unit, 0x22222222));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_104_111r(unit, 0x22222222));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_112_119r(unit, 0x55555555));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_120_127r(unit, 0x72532244));
    
exit:
    SOCDNX_FUNC_RETURN;
}
