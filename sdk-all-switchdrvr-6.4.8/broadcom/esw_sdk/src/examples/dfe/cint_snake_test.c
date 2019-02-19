/*
 * $Id: cint_snake_test.c,v 1.7 Broadcom SDK $
 *
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * Running snake test applicataion
 */

/*
 * flags: set snake test flags: 
 * SOC_DFE_ENABLE_PHY_LOOPBACK      - PHY loopback 
 * SOC_DFE_ENABLE_MAC_LOOPBACK      - MAC loopback 
 * SOC_DFE_ENABLE_EXTERNAL_LOOPBACK - assume External loopback 
 */
int run_snake(int unit, uint32 prepare_flags)
{
    int rv;
    soc_fabric_cell_snake_test_results_t res;
    
    /*first disabe rx_los application - no need to check errors for case rx los application is already disabled*/
    rx_los_unit_detach(unit);    

    rv =  soc_dfe_cell_snake_test_prepare(unit,prepare_flags); 
    if(rv != 0) {
        printf("Error, in soc_dfe_cell_snake_test_prepare, rv=%d, \n", rv);
        return rv;
    }
    
    rv = soc_dfe_cell_snake_test_run(unit, 0, &res);
    if(rv != 0) {
        printf("Error, in soc_dfe_cell_snake_test_run, rv=%d, \n", rv);
        return rv;
    }

    print res;
    if (res.test_failed == 0) {
        printf("run_snake: PASS\n");
    }

    return 0;
};

int is_external_loopback(int unit) {
    int rv;

    int nof_links = SOC_DFE_DEFS_GET_NOF_LINKS(&unit);
    bcm_fabric_link_connectivity_t connectivity_links[nof_links];
    int nof_connectivity_links;
    int i, external_loopback_links;

    rv = bcm_fabric_link_connectivity_status_get(unit, nof_links, connectivity_links, &nof_connectivity_links);

    for (i = 0; i < nof_links; ++i) {
        if (connectivity_links[i].device_type != 0) {
            ++external_loopback_links;
        }
    }

    if (external_loopback_links != nof_links) {
        return BCM_E_FAIL;
    }

    printf("is_external_loopback: PASS\n\n");
    return rv;
}
