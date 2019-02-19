/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: VOQ counter processor~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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
 * File:        cint_voq_count.c
 * Purpose:     Example of reading VOQ counters when the counter processor has a VOQ source
 * 
 * The example counts all the possible HW counters of a single VOQ when the counter processor has a VOQ source
 *
 * The settings include:
 *  o    Set the Counter Processor to count VOQ traffic with highest definition (SOC properties)
 *  o    Get all the possible HW counters of a single VOQ. 
 *
 */
 
/* 
 * Set in the SOC properties file the Counter Processor such that: 
 * - the highest counter resolution is set (10 counters per VOQ) 
 * - a maximum number of VOQs is covered 
 *  
 * To do so, set the SOC properties: 
      counter_engine_source_0=INGRESS_VOQ
      counter_engine_source_1=INGRESS_VOQ
      counter_engine_source_2=INGRESS_VOQ
      counter_engine_source_3=INGRESS_VOQ
      counter_engine_voq_min_queue_0=0
      counter_engine_voq_min_queue_1=6553
      counter_engine_voq_min_queue_2=13107
      counter_engine_voq_min_queue_3=19660
      counter_engine_voq_queue_set_size=1
      counter_engine_statistics_0=FULL_COLOR
      counter_engine_statistics_1=FULL_COLOR
      counter_engine_statistics_2=FULL_COLOR
      counter_engine_statistics_3=FULL_COLOR
 */

/*
 * Retrieve all the possible HW counters of a single VOQ. 
 */
int voq_stat_get(/*in*/ int unit, 
                        int base_queue,     /* Base-VOQ */
                        int cos_trf_cls,    /* Local offset in the VOQ bundle */
                /*out*/ uint64* value       /* Counter value array */
                        ) {
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_stats_t stat[12];
    bcm_gport_t g;
    int x;
    int i;

    stat[0 ] = bcmCosqGportGreenAcceptedPkts  ;
    stat[1 ] = bcmCosqGportGreenAcceptedBytes ;
    stat[2 ] = bcmCosqGportGreenDroppedPkts   ;
    stat[3 ] = bcmCosqGportGreenDroppedBytes  ;
    stat[4 ] = bcmCosqGportNotGreenAcceptedPkts ;
    stat[5 ] = bcmCosqGportNotGreenAcceptedBytes;
    stat[6 ] = bcmCosqGportYellowDroppedPkts  ;
    stat[7 ] = bcmCosqGportYellowDroppedBytes ;
    stat[8 ] = bcmCosqGportRedDroppedPkts     ;
    stat[9 ] = bcmCosqGportRedDroppedBytes    ;
    stat[10] = bcmCosqGportDroppedBytes       ;
    stat[11] = bcmCosqGportDroppedPkts        ;

    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(g, base_queue);
    rv = bcm_cosq_gport_stat_enable_get(unit, g, &x);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_stat_enable_get() failed $rv\n");
        return rv;
    }
    
    /* 
     * Go over all the 12 possible statistics and retrieve them
     */
    for (i = 0; i< 12; i++) {
        rv = bcm_cosq_gport_stat_get(unit, g, cos_trf_cls, stat[i], &value[i]);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_stat_get() failed $rv\n");
            printf ("error in stat %d in queue %d\n", i, base_queue + cos_trf_cls);
            return rv;
        } else {
            /* Print the Counter result */
            if (COMPILER_64_IS_ZERO(value[i]) == 0) {
                switch (i) {
                  case 0  : printf ("stat_id=%d bcmCosqGportGreenAcceptedPkts     = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 1  : printf ("stat_id=%d bcmCosqGportGreenAcceptedBytes    = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 2  : printf ("stat_id=%d bcmCosqGportGreenDroppedPkts      = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 3  : printf ("stat_id=%d bcmCosqGportGreenDroppedBytes     = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 4  : printf ("stat_id=%d bcmCosqGportNotGreenAcceptedPkts  = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 5  : printf ("stat_id=%d bcmCosqGportNotGreenAcceptedBytes = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 6  : printf ("stat_id=%d bcmCosqGportYellowDroppedPkts     = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 7  : printf ("stat_id=%d bcmCosqGportYellowDroppedBytes    = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 8  : printf ("stat_id=%d bcmCosqGportRedDroppedPkts        = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 9  : printf ("stat_id=%d bcmCosqGportRedDroppedBytes       = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 10 : printf ("stat_id=%d bcmCosqGportDroppedBytes          = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                  case 11 : printf ("stat_id=%d bcmCosqGportDroppedPkts           = 0x%x 0x%x\n", i, COMPILER_64_HI(value[i]), COMPILER_64_LO(value[i]) ); break;
                }
            }
        }
    }

    return rv;
}

int main(void)
{
    int unit = 0;
    bcm_error_t rv = BCM_E_NONE;
    uint64 value[12];

    /* Example: get the counter of VOQ 32 */
    rv = voq_stat_get(unit, 32, 0, value);
    if (rv != BCM_E_NONE) {
        printf("voq_stat_get() failed $rv\n");
        return rv;
    }

    printf("cint_voq_count.c completed with status (%s)\n", bcm_errmsg(rv));
    return rv;
}

main();
