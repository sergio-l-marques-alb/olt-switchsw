/* 
 * $Id: cint_dfe_cpu_packets.c,v 1.6 Broadcom SDK $
 *
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
 * DFE configure FIFOs example:
 *  
 * This cint demonstrates how to use the CPU2CPU packets mechanism available in the fe3200:
 * start function will register a callback function , and start the main thread for rx reception
 * stop function will stop the thread, and unregister the callback function
 *  
 */

/* Callback function - prints packet information */
bcm_rx_t cint_cpu_pkts_callback_function(int unit, bcm_pkt_t *pkt, void *cookie)
{
    printf("Packet details:\n");
    printf("Source device: %d\n", pkt->src_mod);
    printf("Destination device: %d\n", pkt->dest_mod);
    printf("Packet unit: %d\n", pkt->unit);
    printf("Packet pipe index : %d\n", packet->pipe_index);
    printf("Packet priority : %d\n", packet->prio_int);
    printf("Packet length : %d\n", packet->pkt_data.len);
    printf("Packet payload : ");
    for (i = 0; i < packet->pkt_data.len; i++)
    {
        printf("%x", packet->pkt_data.data[i]);
    }
    return BCM_RX_HANDLED;
}

int cint_cpu_pkts_config_all_reachable_value(int unit, int max_all_reachable_value)
{
    int rv;

    /* Configuring AlrcMaxBaseIndex */
    rv = bcm_stk_module_max_set(unit, BCM_STK_MODULE_MAX_ALL_REACHABLE, max_all_reachable_value);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - bcm_stk_module_max_set\n");
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

int cint_cpu_pkts_reception_start(int unit)
{
    int rv;
    int cpu_address[] = {550, 650, 750, 850} ;
    int  update_base_index;
    bcm_rx_cfg_t rx_configuration;
    bcm_rx_cb_f callback_function;

    /* Config max all reachable id */
    rv = cint_cpu_pkts_config_all_reachable_value(unit, 512);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - cint_cpu_pkts_config_all_reachable_value");
        return BCM_E_FAIL;
    }

    callback_function = &cint_cpu_pkts_callback_function;
    /* Register a callback function */
    rv = bcm_rx_register(unit, "cint_dfe_cpu_packets_callback_function", callback_function, 100, NULL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - bcm_rx_register \n");
        return BCM_E_FAIL;
    }

    /* Number of mod ids to be configured, should be between 0 and 4 */
    rx_configuration.num_of_cpu_addresses = 4;
    /* Mod ids to be assigned to the cpu buffers , should be in the interval [AlrcMaxBaseIndex, UpdateBaseIndex]*/ 
    rx_configuration.cpu_address = cpu_address; 

    /* Start the packets reception thread */
    rv = bcm_rx_start(unit, &rx_configuration);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - bcm_rx_start \n");
        return BCM_E_FAIL;
    }

    printf("cint_dfe_cpu_packets - cint_cpu_pkts_reception_start finished successfully \n");
    return BCM_E_NONE;
}

int cint_cpu_pkts_reception_stop(int unit)
{
    int rv;
    bcm_rx_cfg_t rx_configuration;

    /* Configuring the rx configuration structure */

    /* Configuring num of cpu addresses = 0 , in order to un-assign the mod ids assigned to the cpu buffers */
    rx_configuration.num_of_cpu_addresses = 0; 

    /* Stop the packets reception thread, and un-assign the cpu buffer ids */
    rv = bcm_rx_stop(unit, &rx_configuration);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - bcm_rx_stop \n");
        return BCM_E_FAIL;
    }

    /* Unregister the callback function */
    rv = bcm_rx_unregister(unit, NULL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - bcm_rx_unregister \n");
        return BCM_E_FAIL;
    }

    printf("cint_dfe_cpu_packets - cint_cpu_pkts_reception_stop finished successfully \n");
    return BCM_E_NONE;
}

int main(int unit){
    cint_cpu_pkts_reception_start(unit);
    cint_cpu_pkts_reception_stop(unit);
    return BCM_E_NONE;
}



