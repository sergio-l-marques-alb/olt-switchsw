/*
 * $Id: cint_ctrp_egress_received_tm.c,v 1.2 Broadcom SDK $
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
 * File:        cint_ctrp_egress_received_tm.c
 * Purpose:     Example of reading ERPP forward and drop counters
 * 
 * The example counts all the possible HW counters related to this specific mode
 *
 * The settings include:
 *  o    Set the Counter Processor to count forward and droped counters at the ERPP (SOC properties)
 *  o    Get all the relevant counters. 
 *
 */
 
/* 
 *  
 * To do so, set the SOC properties: 
      counter_engine_source_0.BCM88650=EGRESS_RECEIVE_TM
      counter_engine_statistics_0.BCM88650=FWD_DROP
 
   Actually 2 possibilities of count: per egress queue (EGRESS_RECEIVE_TM)
   or per port (EGRESS_RECEIVE_TM_PORT). In both cases, the API gport is
   accessed per Queue. The translation queue -> port is done internally.
 */

/*
 * Retrieve all the possible HW counters of a single VOQ. 
 */
int egress_receive_tm_stat_get(/*in*/ int unit, 
                        soc_port_t port,
                        bcm_cos_t cos,
                /*out*/ uint64* value       /* Counter value array */
                        ) {
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_stats_t stat[6];
    bcm_gport_t gport;
    int i, x;

    stat[0 ] = bcmCosqGportReceivedBytes;          /* Bytes received into queue. */
    stat[1 ] = bcmCosqGportReceivedPkts;           /* Pkts received into queue. */
    stat[2 ] = bcmCosqGportDroppedBytes;           /* Bytes dropped in queue. */
    stat[3 ] = bcmCosqGportDroppedPkts;            /* Pkts dropped in queue. */   
    stat[4 ] = bcmCosqGportEnqueuedPkts;           /* enqueued packets */
    stat[5 ] = bcmCosqGportEnqueuedPkts;          /* enqueued bytes */

    BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(gport, port);
    rv = bcm_cosq_gport_stat_enable_get(unit, gport, &x);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_stat_enable_get() failed $rv\n");
        return rv;
    }
    
    /* 
     * Go over all the 6 possible statistics and retrieve them
     */
    for (i = 0; i< 6; i++) {
        rv = bcm_cosq_gport_stat_get(unit, gport , cos, stat[i], &value[i]);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_stat_get() failed $rv\n");
            printf ("error in stat %d in port %d\n", i, port);
            return rv;
        } else {
            /* Print the Counter result */
            if (COMPILER_64_IS_ZERO(value[i]) == 0) {
                switch (i) {
                  case 0  : printf ("stat_id=%d bcmCosqGportReceivedBytes       = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 1  : printf ("stat_id=%d bcmCosqGportReceivedPkts        = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 2  : printf ("stat_id=%d bcmCosqGportGreenDroppedPkts    = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 3  : printf ("stat_id=%d bcmCosqGportDroppedPkts         = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 4  : printf ("stat_id=%d bcmCosqGportEnqueuedBytes       = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 5  : printf ("stat_id=%d bcmCosqGportEnqueuedPkts        = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                }
            }
        }
    }

    return rv;
}

int main(void)
{
    int unit = 0;
    int port =  14;
    int cos = 0;
    bcm_error_t rv = BCM_E_NONE;
    uint64 value[6];


    /* Example: get the counters of port 14 cos 0 */
    rv = egress_receive_tm_stat_get(unit, port, cos, value);
    if (rv != BCM_E_NONE) {
        printf("egress_receive_tm_stat_get() failed $rv\n");
        return rv;
    }

    printf("egress_receive_tm_stat_get completed with status (%s)\n", bcm_errmsg(rv));
    return rv;
}

main();

