/* 
 * $Id: cint_thresholds_fe3200.c,v 1.6 Broadcom SDK $
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
 * DFE configure FIFOs example:
 *  
 * This cint configures default fe1600 thresholds to fe3200 
 *  
 */


/* First element in the array is the default threshold for single pipe, second is for dual pipe*/

struct cint_thresholds_type_and_value_s {
    int value;
    bcm_fabric_link_threshold_type_t type;
};


struct cint_threshold_default_s {

    /* Same thresholds per pipe */
    int rx_llfc_threshold; /* bcmFabricLinkRxFifoLLFC */
    int rx_full_threshold; /* bcmFabricLinkRxFull */
    int rx_multicast_low_prio_drop_threshold; /* bcmFabricLinkRxMcLowPrioDrop */
    int rx_gci_lvl_1_threshold; /* bcmFabricLinkRxGciLvl1FC */
    int rx_gci_lvl_2_threshold; /* bcmFabricLinkRxGciLvl2FC */
    int rx_gci_lvl_3_threshold; /* bcmFabricLinkRxGciLvl3FC */
    int mid_almost_full_threshold; /* bcmFabricLinkMidAlmostFull */
    int mid_gci_lvl_1_threshold; /* bcmFabricLinkMidGciLvl1FC */
    int mid_gci_lvl_2_threshold; /* bcmFabricLinkMidGciLvl2FC */
    int mid_gci_lvl_3_threshold; /* bcmFabricLinkMidGciLvl3FC */
    int tx_llfc_bypass_threshold; /* bcmFabricLinkFE1TxBypassLLFC */
    int tx_almost_full_threshold; /* bcmFabricLinkTxAlmostFull */
    int tx_rci_threshold; /* bcmFabricLinkRciFC */

    

    cint_thresholds_type_and_value_s thresholds_both_pipes[13];
    cint_thresholds_type_and_value_s thresholds_pipe_0[7];
    cint_thresholds_type_and_value_s thresholds_pipe_1[7];


    /* Different thresholds per pipe*/
    int tx_gci_lvl_1_pipe_0_threshold; /* bcmFabricLinkTxGciLvl1FC */
    int tx_gci_lvl_2_pipe_0_threshold; /* bcmFabricLinkTxGciLvl2FC */
    int tx_gci_lvl_3_pipe_0_threshold; /* bcmFabricLinkTxGciLvl3FC */
    int tx_gci_lvl_1_pipe_1_threshold; /* bcmFabricLinkTxGciLvl1FC */
    int tx_gci_lvl_2_pipe_1_threshold; /* bcmFabricLinkTxGciLvl2FC */
    int tx_gci_lvl_3_pipe_1_threshold; /* bcmFabricLinkTxGciLvl3FC */
    int tx_drop_pipe_0_threshold; /* bcmFabricLinkTxPrio[0-3]Drop */
    int tx_drop_pipe_1_threshold; /* bcmFabricLinkTxPrio[0-3]Drop */

};


void initalize_thresholds_to_fe1600_default_thresholds(cint_threshold_default_s* default_thresholds, int nof_pipes)
{
    /* Default thresholds for both pipes (fe1600 defaults) */

    default_thresholds->thresholds_both_pipes[0].type = bcmFabricLinkRxFifoLLFC;
    default_thresholds->thresholds_both_pipes[0].value = (nof_pipes == 1) ? 100:40;

    default_thresholds->thresholds_both_pipes[1].type = bcmFabricLinkRxFull;
    default_thresholds->thresholds_both_pipes[1].value = (nof_pipes == 1) ? 120:60;

    default_thresholds->thresholds_both_pipes[2].type = bcmFabricLinkRxMcLowPrioDrop;
    default_thresholds->thresholds_both_pipes[2].value = (nof_pipes == 1) ? 64:32;

    default_thresholds->thresholds_both_pipes[3].type = bcmFabricLinkRxGciLvl1FC;
    default_thresholds->thresholds_both_pipes[3].value = (nof_pipes == 1) ? 129:129;

    default_thresholds->thresholds_both_pipes[4].type = bcmFabricLinkRxGciLvl2FC;
    default_thresholds->thresholds_both_pipes[4].value = (nof_pipes == 1) ? 129:129;

    default_thresholds->thresholds_both_pipes[5].type = bcmFabricLinkRxGciLvl3FC;
    default_thresholds->thresholds_both_pipes[5].value = (nof_pipes == 1) ? 129:129;

    default_thresholds->thresholds_both_pipes[6].type = bcmFabricLinkMidAlmostFull;
    default_thresholds->thresholds_both_pipes[6].value = (nof_pipes == 1) ? 320:120;

    default_thresholds->thresholds_both_pipes[7].type = bcmFabricLinkMidGciLvl1FC;
    default_thresholds->thresholds_both_pipes[7].value = (nof_pipes == 1) ? 256:128;

    default_thresholds->thresholds_both_pipes[8].type = bcmFabricLinkMidGciLvl2FC;
    default_thresholds->thresholds_both_pipes[8].value = (nof_pipes == 1) ? 256:128;

    default_thresholds->thresholds_both_pipes[9].type = bcmFabricLinkMidGciLvl3FC;
    default_thresholds->thresholds_both_pipes[9].value = (nof_pipes == 1) ? 256:128;

    default_thresholds->thresholds_both_pipes[10].type = bcmFabricLinkFE1TxBypassLLFC;
    default_thresholds->thresholds_both_pipes[10].value = (nof_pipes == 1) ? 322:194;

    default_thresholds->thresholds_both_pipes[11].type = bcmFabricLinkTxAlmostFull;
    default_thresholds->thresholds_both_pipes[11].value = (nof_pipes == 1) ? 322:194;

    default_thresholds->thresholds_both_pipes[12].type = bcmFabricLinkRciFC;
    default_thresholds->thresholds_both_pipes[12].value = (nof_pipes == 1) ? 200:110;

    /* Default thresholds for pipe 0 (fe1600 defaults) */

    default_thresholds->thresholds_pipe_0[0].type = bcmFabricLinkTxGciLvl1FC;
    default_thresholds->thresholds_pipe_0[0].value = (nof_pipes == 1) ? 180:80;

    default_thresholds->thresholds_pipe_0[1].type = bcmFabricLinkTxGciLvl2FC;
    default_thresholds->thresholds_pipe_0[1].value = (nof_pipes == 1) ? 220:100;

    default_thresholds->thresholds_pipe_0[2].type = bcmFabricLinkTxGciLvl3FC;
    default_thresholds->thresholds_pipe_0[2].value = (nof_pipes == 1) ? 260:120;

    default_thresholds->thresholds_pipe_0[3].type = bcmFabricLinkTxPrio0Drop;
    default_thresholds->thresholds_pipe_0[3].value = (nof_pipes == 1) ? 320:128;

    default_thresholds->thresholds_pipe_0[4].type = bcmFabricLinkTxPrio1Drop;
    default_thresholds->thresholds_pipe_0[4].value = (nof_pipes == 1) ? 320:128;

    default_thresholds->thresholds_pipe_0[5].type = bcmFabricLinkTxPrio2Drop;
    default_thresholds->thresholds_pipe_0[5].value = (nof_pipes == 1) ? 320:128;

    default_thresholds->thresholds_pipe_0[6].type = bcmFabricLinkTxPrio3Drop;
    default_thresholds->thresholds_pipe_0[6].value = (nof_pipes == 1) ? 320:128;

    /* Default thresholds for pipe 1 (fe1600 defaults) */

    default_thresholds->thresholds_pipe_1[0].type = bcmFabricLinkTxGciLvl1FC;
    default_thresholds->thresholds_pipe_1[0].value = 100;

    default_thresholds->thresholds_pipe_1[1].type = bcmFabricLinkTxGciLvl2FC;
    default_thresholds->thresholds_pipe_1[1].value = 120;

    default_thresholds->thresholds_pipe_1[2].type = bcmFabricLinkTxGciLvl3FC;
    default_thresholds->thresholds_pipe_1[2].value = 140;

    default_thresholds->thresholds_pipe_1[3].type = bcmFabricLinkTxPrio0Drop;
    default_thresholds->thresholds_pipe_1[3].value = (nof_pipes == 1) ? 320:192;

    default_thresholds->thresholds_pipe_1[4].type = bcmFabricLinkTxPrio1Drop;
    default_thresholds->thresholds_pipe_1[4].value = (nof_pipes == 1) ? 320:192;

    default_thresholds->thresholds_pipe_1[5].type = bcmFabricLinkTxPrio2Drop;
    default_thresholds->thresholds_pipe_1[5].value = (nof_pipes == 1) ? 320:192;

    default_thresholds->thresholds_pipe_1[6].type = bcmFabricLinkTxPrio3Drop;
    default_thresholds->thresholds_pipe_1[6].value = (nof_pipes == 1) ? 320:192;

}

int config_default_thresaholds(int unit, cint_threshold_default_s default_thresholds, int nof_pipes)
{
    int rv, threshold_values[13], i;
    bcm_fabric_link_threshold_type_t threshold_types[13];
    /* Thresholds for all pipes */
    for (i = 0 ; i < 13 ; i++)
    {
        threshold_types[i] = default_thresholds.thresholds_both_pipes[i].type;
        threshold_values[i] = default_thresholds.thresholds_both_pipes[i].value;
    }

    rv = bcm_fabric_link_thresholds_pipe_set(unit, 0, bcmFabricPipeAll, 0, 13, threshold_types, threshold_values); /* Assigning thresholds to type 0 */
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_thresholds_fe3200 - bcm_fabric_link_thresholds_pipe_set\n");
        return BCM_E_FAIL;
    }

    rv = bcm_fabric_link_thresholds_pipe_set(unit, 1, bcmFabricPipeAll, 0, 13, threshold_types, threshold_values); /* Assigning thresholds to type 1 */
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_thresholds_fe3200 - bcm_fabric_link_thresholds_pipe_set\n");
        return BCM_E_FAIL;
    }
    /* Thresholds for pipe 0 */
    for (i = 0 ; i < 7; i++)
    {
        threshold_types[i] = default_thresholds.thresholds_pipe_0[i].type;
        threshold_values[i] = default_thresholds.thresholds_pipe_0[i].value;
    }

    rv = bcm_fabric_link_thresholds_pipe_set(unit, 0, bcmFabricPipe0, 0, 7, threshold_types, threshold_values); /* Assigning thresholds to type 0 */
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_thresholds_fe3200 - bcm_fabric_link_thresholds_pipe_set\n");
        return BCM_E_FAIL;
    }

    rv = bcm_fabric_link_thresholds_pipe_set(unit, 1, bcmFabricPipe0, 0, 7, threshold_types, threshold_values); /* Assigning thresholds to type 1 */
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_thresholds_fe3200 - bcm_fabric_link_thresholds_pipe_set\n");
        return BCM_E_FAIL;
    }
    if (nof_pipes >= 2)
    {
        /* Thresholds for pipe 1 */
        for (i = 0 ; i < 7; i++)
        {
            threshold_types[i] = default_thresholds.thresholds_pipe_1[i].type;
            threshold_values[i] = default_thresholds.thresholds_pipe_1[i].value;
        }

        rv = bcm_fabric_link_thresholds_pipe_set(unit, 0, bcmFabricPipe1, 0, 7, threshold_types, threshold_values); /* Assigning thresholds to type 0 */
        if (rv != BCM_E_NONE)
        {
            printf("Error cint_thresholds_fe3200 - bcm_fabric_link_thresholds_pipe_set\n");
            return BCM_E_FAIL;
        }

        rv = bcm_fabric_link_thresholds_pipe_set(unit, 1, bcmFabricPipe1, 0, 7, threshold_types, threshold_values); /* Assigning thresholds to type 1 */
        if (rv != BCM_E_NONE)
        {
            printf("Error cint_thresholds_fe3200 - bcm_fabric_link_thresholds_pipe_set\n");
            return BCM_E_FAIL;
        }

    }

    return BCM_E_NONE;
}

int main(int unit, int nof_pipes)
{
    int rv;
    cint_threshold_default_s default_thresholds;

    initalize_thresholds_to_fe1600_default_thresholds(&default_thresholds, nof_pipes);

    rv = config_default_thresaholds(unit, default_thresholds, nof_pipes);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_thresholds_fe3200 - config_default_thresholds failed");
        return BCM_E_FAIL;
    }

    printf("cint_thresholds_fe3200: Thresholds configured successfully \n");
    return BCM_E_NONE;

}

