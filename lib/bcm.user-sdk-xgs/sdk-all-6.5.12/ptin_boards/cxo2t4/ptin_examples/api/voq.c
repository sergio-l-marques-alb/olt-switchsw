/*
* Retrieve all the possible HW counters of a single VOQ.
*/
int voq_stat_get(/*in*/ int unit,
                        int base_queue,     /* Base-VOQ */
                        int cos_trf_cls     /* Local offset in the VOQ bundle */
                        )
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_stats_t stat[12];
    uint64 value[12];
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

