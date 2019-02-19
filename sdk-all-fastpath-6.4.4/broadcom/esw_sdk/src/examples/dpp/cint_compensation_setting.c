/* $Id$
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
 * $
*/

int cint_packet_length_adjust_cb (
    int unit, 
    bcm_gport_t port, 
    int numq, 
    uint32 flags, 
    bcm_gport_t gport, 
    void *user_data)
{
     int rv;
     int* ingress_compensation_ptr = user_data;
     if(BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
         rv = bcm_cosq_control_set(unit, gport, 0, bcmCosqControlPacketLengthAdjust, *ingress_compensation_ptr); 
         if(rv<0) {
             return rv;
         }
     }
     
     return 0;
}
     

                                   

/*Example of usage: cint_compensation_set(unit, 2, -2)*/

int cint_compensation_set(int unit, int ingress_comp, int egress_comp)
{
    int rv = BCM_E_NONE;
    int port;
    bcm_gport_t gport; 
    bcm_port_config_t config;

    rv = bcm_port_config_get(unit, &config);
    if (rv != BCM_E_NONE) {
        printf("failed to get port bmps in cint_compensation_set");
        return rv;
    }
    rv = bcm_cosq_gport_traverse(unit, cint_packet_length_adjust_cb, &ingress_comp);
    if (rv != BCM_E_NONE) {
        printf("failed to traverse gports and set ingress compensation");
        return rv;
    }

    BCM_PBMP_ITER(config.all, port) {
     if(!BCM_PBMP_MEMBER(config.sfi, port)) {
        BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(gport, port);
        rv = bcm_cosq_control_set(unit, gport, 0, bcmCosqControlPacketLengthAdjust, egress_comp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_control_set - unicast\n");
            return rv;
        }
        BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(gport, port);
        rv = bcm_cosq_control_set(unit, gport, 0, bcmCosqControlPacketLengthAdjust, egress_comp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_control_set - multicast\n");
            return rv;
        }

     }
   }
    printf("cint_compensation_set: PASS.\n");
    return rv;
}
