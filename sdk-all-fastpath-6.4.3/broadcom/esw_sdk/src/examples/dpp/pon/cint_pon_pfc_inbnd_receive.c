/* $Id: cint_pon_pfc_inbnd_receive.c,v 1.0 Broadcom SDK $
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
* File: cint_pon_pfc_inbnd_receive.c  
* 
* Purpose: examples of enable PFC receive on NNI port 132.
*
* To Activate Above Settings Run:
*     BCM> cint examples/dpp/pon/cint_pon_pfc_inbnd_receive.c
*     BCM> cint
*     cint> cint_arad_test_pfc_inband_rec_set(0, 132);
* 
*/
int cint_arad_test_pfc_inband_rec_set(int unit, int dest_port)
{
    int                           rc = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t        source, target;
    int                           my_modid;

    bcm_stk_modid_get(unit, &my_modid);
	
    /* flow control configuration */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    BCM_GPORT_MODPORT_SET(source.port, 0, dest_port);
    source.cosq = 0;   /* Cosq = -1 indicates LL FC */
    target.cosq = 0;   /* can be any value between 0 and 7 */
    BCM_GPORT_MODPORT_SET(target.port, 0, dest_port);

    rc = bcm_cosq_fc_path_add(unit, bcmCosqFlowControlReception, &source, &target);
    if (BCM_FAILURE(rc)) {
        printf("bcm_cosq_fc_path_add failed, PFC, port_gport(0x%x) err=%d %s\n", dest_port, rc, bcm_errmsg(rc));
        return(rc);
    }

    return(rc);
}
