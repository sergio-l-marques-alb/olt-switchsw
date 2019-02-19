/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~1588~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * $Id$
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
 * File: cint_1588.c
 * Purpose: 1588 protocol use examples
 *
 * BEFORE USING EXAMPLES IN THIS CINT:
 * ------------------------------------:
 * 1) 1588 is NOT SUPPORTED FOR ARAD A0, supported for ARAD B0 and above.
 * 2) Make sure the Makefile used includes PTP feature.
 * 3) Enable TS_PLL clock by enabling/un-commenting following soc property:
 *      phy_1588_dpll_frequency_lock.BCM88650=1
 *    (otherwise the TS clock would not run, as a result the CF won`t be updated) 
 * 4) In case the system contain more than one device, need to run Broadsync application (see 'Broadsync' section below).
 *
 * Provides the following functionalities:
 *
 * - 1588 one/two step TC (Transparent clock). 
 * - trap/drop/snoop/fwd control over 1588 messages. 
 *
 *    In transport clock mode, every fabric based system is observed by the network as a single transparent
 *    clock entity, with the residence time calculated from the moment the PTP packet enters the system through
 *    one port, to the time it exits through another. 
 *    when 1 step TC is enabled:
 *       The system updates the correction field (CF) of Event messages
 *    when 2 step TC is enabled:
 *       The system records the TX time of Event messages in a FIFO (the application can later read the TX time from the FIFO,
 *       calculate residence time and update the relevant Follow up message)
 *
 *      Event Messages:
 *        1. Sync
 *        2. Delay_Req
 *        3. Pdelay_Req
 *        4. Pdelay_Resp
 *      General Messages:
 *        1. Announce
 *        2. System 
 *        3. Follow_Up
 *        4. Delay_Resp
 *        5. Pdelay_Resp_Follow_Up
 *        6. Management
 *        7. Signaling 
 * 
 *    Supported 1588 encapsulations:
 *      follwing 1588 encapsulations are supported:
 *        1. 1588oE                    switched
 *        2. 1588oUDPoIPoE             switched/IP routed
 *        3. 1588oUDPoIPoIPoE          switched/IP routed/IP terminated
 *        4. 1588oUDPoIPoMPLSoE        switched/MPLS routed/MPLS terminated
 *        5. 1588oEoMPLSoE             switched/MPLS routed/MPLS terminated
 *    CF update:
 *        the CF (8 bytes) update is done in bytes number 2,3 (zero based).
 *        additionally, least significant bit can be changed for internal use.
 *
 *    Broadsync:
 *     1588 TC (Transparent clock) relay on clock synchronization between devices in the system i.e. Broadsync,
 *     for Broadsync configuration see BCM shell "bs config" command (see example in cint_time.c).
 *    
 *      
 *
 *
 *
 *
 */
/* 
 * how to run:
 *
 * for example: enable one step TC for unit 0 port 13, 14 (both In-port and Out-port need to be configured.
 *              the packet timestamp is saved at the ingress, the packet CF is updated at the egress):
 *
cint ../../../../src/examples/dpp/cint_1588.c
cint
int rv; 
rv = ieee_1588_port_set(0,13,1); 
print rv;
rv = ieee_1588_port_set(0,14,1); 
print rv;
 *
 * for example: disable one step TC for unit 0 port 13:
 *
cint ../../../../src/examples/dpp/cint_1588.c
cint
int rv; 
rv = ieee_1588_port_set(0,13,0); 
print rv;
 *
 */ 



/* 
 * enable = 1, enable  one step TC 
 * enable = 0, disable one step TC 
 */
int ieee_1588_port_set(int unit, bcm_gport_t port, int enable)
{
    int                           rv = BCM_E_NONE;
    bcm_port_timesync_config_t timesync_config;
    bcm_port_timesync_config_t *timesync_config_ptr;
    int config_count;

    if(0 == enable) {
        timesync_config_ptr = NULL;
        config_count        = 0;
    } else {

        /* flags to enable 1 step TC                                                                   */
        timesync_config.flags = (BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP);
        /* to enable 2 step TC use folowing flags instead                                              */
        /* timesync_config.flags = (BCM_PORT_TIMESYNC_DEFAULT | BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP); */
        timesync_config.pkt_drop  = 0;
        timesync_config.pkt_tocpu = 0;

        /* for example to trap PDELAY_REQ message and drop PDELAY_RESP message use following            */
        /* timesync_config.pkt_tocpu |= BCM_PORT_TIMESYNC_PKT_PDELAY_REQ;                               */
        /* timesync_config.pkt_drop  |= BCM_PORT_TIMESYNC_PKT_PDELAY_RESP;                              */
        /*                                                                                              */
        /* 2 notes:                                                                                     */
        /* 1) each 1588 message can be added to pkt_tocpu bitmask or pkt_drop, not both.                */
        /*    in case no bit is turned on the packet will be forwarded.                                 */
        /* 2) prior to trap 1588 message, the 1588 trap need to be cofigures with a port,               */
        /*    for example, following will trap 1588 messages raised in pkt_tocpu bitmask to port 200:   */        
        /*      cint ../../../../src/examples/dpp/cint_rx_trap_fap.c                                    */
        /*      cint                                                                                    */
        /*      int rv;                                                                                 */
        /*      int trap_id_handle;                                                                     */
        /*      rv = set_rx_trap(0, bcmRxTrap1588, 0, &trap_id_handle, 200);                            */
        /*      print rv;                                                                               */
        /*      print trap_id_handle;                                                                   */
        
        
        timesync_config_ptr = &timesync_config;
        config_count        = 1;       
    }

    rv =  bcm_port_timesync_config_set(unit, port, config_count, timesync_config_ptr);
    if (rv != BCM_E_NONE) {
        if(rv == BCM_E_UNAVAIL) {
            printf("Error, bcm_port_timesync_config_set 1588 is not supported for ARAD A0\n");
        } else {
            printf("Error, bcm_port_timesync_config_set rv = %d\n", rv);
        }
        return rv;
    }

    return rv;
}


/* 
 * enable = 1, one step TC is enabled
 * enable = 0, one step TC is disabled
 */
int ieee_1588_port_get(int unit, bcm_gport_t port, int *enabled)
{
    int                           rv = BCM_E_NONE;
    bcm_port_timesync_config_t timesync_config;
    int array_count;

    rv =  bcm_port_timesync_config_get(unit, port, 1, timesync_config, &array_count);
    if (rv != BCM_E_NONE) {
        if(rv == BCM_E_UNAVAIL) {
            printf("Error, bcm_port_timesync_config_set 1588 is not supported for ARAD A0\n");
        } else {
            printf("Error, bcm_port_timesync_config_get rv = %d\n", rv);
        }
        return rv;
    }

    if(0 == array_count) {
        *enabled = 0;
        printf("1588 is DISABLED for port = %d \n", port);
    } else {
        *enabled = 1;
        printf("1588 is ENABLED  for port = %d \n", port);
    }

    return rv;
}
