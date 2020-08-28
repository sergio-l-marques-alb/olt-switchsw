/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: VOQ counter processor~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 *
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        cint_counter_processor.c
 * Purpose:     Example of configuring counter processor statistics gatheting using dynamic APIs.
 * 
 * *
 * The settings include:
 *  - Configuring engine's counting source, and counting formats.
 *  - Configuring LIF counting ranges.
 *
 */

int lif_counting_set(int unit,                                           /*in*/ 
                     bcm_stat_counter_source_type_t source,              /*in*/ 
                     int command_id,                                     /*in*/ 
                     bcm_stat_counter_lif_range_id_t lif_counting_mask,  /*in*/ 
                     bcm_stat_counter_lif_stack_id_t lif_stack_to_map,   /*in*/ 
                     bcm_stat_counter_lif_stack_id_t lif_stack_to_count  /*in*/ 
                ) {
    bcm_stat_counter_lif_range_id_t 
        lif_ranges[BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS] = {
            bcmStatCounterLifRangeIdLifInvalid, 
            bcmStatCounterLifRangeIdNotInAny, 
            bcmBcmStatCounterLifRangeId0, 
            bcmBcmStatCounterLifRangeId1};
    bcm_error_t rv = BCM_E_NONE;
    uint32 flags = 0;
    int bitmap = 0;
    int match;
    int lif_0 = 0,
        lif_1 = 0,
        lif_2 = 0,
        lif_3 = 0;
    bcm_stat_counter_source_t counter_source;
    bcm_stat_counter_lif_mask_t counting_mask;
    counter_source.engine_source = source;
    counter_source.command_id = command_id;
    for (bitmap = 0; bitmap < 0x100 ;bitmap++) {
        lif_0 = bitmap & 0x3;
        lif_1 = (bitmap >> 2) & 0x3;
        lif_2 = (bitmap >> 4) & 0x3;
        lif_3 = (bitmap >> 6) & 0x3;
        counting_mask.lif_counting_mask[0] = lif_ranges[lif_0];
        counting_mask.lif_counting_mask[1] = lif_ranges[lif_1];
        counting_mask.lif_counting_mask[2] = lif_ranges[lif_2];
        counting_mask.lif_counting_mask[3] = lif_ranges[lif_3];

        match = 0;
        if (lif_stack_to_map != bcmStatCounterLifStackIdNone) {
            if (counting_mask.lif_counting_mask[lif_stack_to_map] == lif_counting_mask) {match = 1;}
        } else {
            if (counting_mask.lif_counting_mask[0] == lif_counting_mask) {match = 1;}
            if (counting_mask.lif_counting_mask[1] == lif_counting_mask) {match = 1;}
            if (counting_mask.lif_counting_mask[2] == lif_counting_mask) {match = 1;}
            if (counting_mask.lif_counting_mask[3] == lif_counting_mask) {match = 1;}
        }
        if (match) {
            rv = bcm_stat_counter_lif_counting_set (unit, flags, &counter_source, &counting_mask, lif_stack_to_count);
            if (rv != BCM_E_NONE) {
                printf("bcm_stat_counter_lif_counting_set() failed $rv\n");
                return rv;
            }
        }
    }
    return rv;
}

int lif_counting_get(int unit,                                           /*in*/ 
                     bcm_stat_counter_source_type_t source,              /*in*/ 
                     int command_id,                                     /*in*/ 
                     bcm_stat_counter_lif_range_id_t  lif_counting_mask, /*in*/ 
                     bcm_stat_counter_lif_stack_id_t  lif_stack_to_map,  /*in*/ 
                     bcm_stat_counter_lif_stack_id_t* lif_stack_to_count /*out*/
                ) {
    bcm_stat_counter_lif_range_id_t 
        lif_ranges[BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS] = {
            bcmStatCounterLifRangeIdLifInvalid, 
            bcmStatCounterLifRangeIdNotInAny, 
            bcmBcmStatCounterLifRangeId0, 
            bcmBcmStatCounterLifRangeId1};
    bcm_error_t rv = BCM_E_NONE;
    uint32 flags = 0;
    int bitmap = 0;
    int lif_0 = 0,
        lif_1 = 0,
        lif_2 = 0,
        lif_3 = 0;
    bcm_stat_counter_source_t counter_source;
    bcm_stat_counter_lif_mask_t counting_mask;
    counter_source.engine_source = source;
    counter_source.command_id = command_id;
    bitmap = sal_rand() & 0xff;
    lif_0 = bitmap & 0x3;
    lif_1 = (bitmap >> 2) & 0x3;
    lif_2 = (bitmap >> 4) & 0x3;
    lif_3 = (bitmap >> 6) & 0x3;
    counting_mask.lif_counting_mask[0] = lif_ranges[lif_0];
    counting_mask.lif_counting_mask[1] = lif_ranges[lif_1];
    counting_mask.lif_counting_mask[2] = lif_ranges[lif_2];
    counting_mask.lif_counting_mask[3] = lif_ranges[lif_3];

    
    if (lif_stack_to_map != bcmStatCounterLifStackIdNone) {
        counting_mask.lif_counting_mask[lif_stack_to_map] = lif_counting_mask;
    } else {
        rv = BCM_E_PARAM;
        return rv;
    }
    rv = bcm_stat_counter_lif_counting_get(unit, flags, &counter_source, &counting_mask, lif_stack_to_count);
    if (rv != BCM_E_NONE) {
        printf("bcm_stat_counter_lif_counting_get() failed $rv\n");
        return rv;
    }
    
    return rv;
}

int lif_counting_test(int unit,                                           /*in*/ 
                      bcm_stat_counter_source_type_t source,              /*in*/ 
                      int command_id,                                     /*in*/ 
                      bcm_stat_counter_lif_range_id_t lif_counting_mask,  /*in*/ 
                      bcm_stat_counter_lif_stack_id_t lif_stack_to_map,   /*in*/ 
                      bcm_stat_counter_lif_stack_id_t lif_stack_to_count, /*in*/
                      int nof_iterations                                  /*in*/
                ) {
    bcm_stat_counter_lif_stack_id_t lif_stack_to_count_get;
    bcm_error_t rv = BCM_E_NONE;
    int idx_iteration;
    rv = lif_counting_set(unit, source, command_id, lif_counting_mask, lif_stack_to_map, lif_stack_to_count);
    if (rv != BCM_E_NONE) {
        printf("lif_counting_set() failed $rv\n");
        return rv;
    }

    if (lif_stack_to_map != bcmStatCounterLifStackIdNone) {
        for (idx_iteration = 0; idx_iteration < nof_iterations; idx_iteration++) {
            rv = lif_counting_get(unit, source, command_id, lif_counting_mask, lif_stack_to_map, &lif_stack_to_count_get);
            if (rv != BCM_E_NONE) {
                printf("lif_counting_get() failed $rv\n");
                return rv;
            }
            if (lif_stack_to_count != lif_stack_to_count_get) {
                printf("expected lif_stack_to_count_get to be $d but got %d.\n", lif_stack_to_count, lif_stack_to_count_get);
                return BCM_E_CONFIG;
            }
        }
    }
    return rv;
}

int lif_counting_get_test(int unit,                                                    /*in*/ 
                          bcm_stat_counter_source_type_t source,                       /*in*/ 
                          int command_id,                                              /*in*/ 
                          bcm_stat_counter_lif_stack_id_t expected_lif_stack_to_count, /*in*/
                          int nof_iterations                                           /*in*/)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 flags = 0;
    int idx_iteration; 
    int lif_mask_iter;
    bcm_stat_counter_source_t counter_source;
    bcm_stat_counter_lif_mask_t counting_mask;

    counter_source.engine_source = source;
    counter_source.command_id = command_id;
    int lif_stack_to_count;
    bcm_stat_counter_lif_range_id_t 
        lif_ranges[BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS] = {
            bcmStatCounterLifRangeIdLifInvalid, 
            bcmStatCounterLifRangeIdNotInAny, 
            bcmBcmStatCounterLifRangeId0, 
            bcmBcmStatCounterLifRangeId1};
    for (idx_iteration = 0; idx_iteration < nof_iterations; idx_iteration++) {
        for (lif_mask_iter = bcmStatCounterLifStackId0; lif_mask_iter < BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS; lif_mask_iter++) {
            if (lif_mask_iter == expected_lif_stack_to_count) {
                counting_mask.lif_counting_mask[lif_mask_iter] = (command_id == 0) ? bcmBcmStatCounterLifRangeId0 : bcmBcmStatCounterLifRangeId1; 
            } else {
                counting_mask.lif_counting_mask[lif_mask_iter] = lif_ranges[sal_rand() & 0x3];
            }
        }
        rv = _bcm_petra_stat_counter_lif_counting_get(unit, flags, &counter_source, &counting_mask, &lif_stack_to_count); 
        if (rv != BCM_E_NONE) {
            printf("bcm_stat_counter_lif_counting_get() failed %d\n", rv);
            return rv;
        }
        if (lif_stack_to_count != expected_lif_stack_to_count) {
            printf("bcm_stat_counter_lif_counting_get() expected %d, got %d\n", expected_lif_stack_to_count, lif_stack_to_count);
            return BCM_E_CONFIG;
        }
    }
    return rv;
}



int sfi_stat_bad_get(int unit)
{
    uint64 temp_val;
    int port;
    bcm_port_config_t pcfg;

    bcm_port_config_get(0, &pcfg);

    BCM_PBMP_ITER(pcfg.sfi, port) {
        bcm_stat_get(0, port, snmpBcmTxControlCells, &temp_val);
        bcm_stat_get(0, port, snmpBcmTxDataCells, &temp_val);
        bcm_stat_get(0, port, snmpBcmTxDataBytes, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxCrcErrors, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxFecCorrectable, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxControlCells, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxDataCells, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxDataBytes, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxDroppedRetransmittedControl, &temp_val);
        bcm_stat_get(0, port, snmpBcmTxAsynFifoRate, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxAsynFifoRate, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxFecUncorrectable, &temp_val);
        bcm_stat_get(0, port, snmpBcmRxCrcErrors, &temp_val);
    }
    return BCM_E_NONE;
}

int sfi_stat_counter_clear(int unit, int isFE)
{
    uint64 temp_val;
    int port;
    bcm_port_config_t pcfg;
    bcm_error_t rv = BCM_E_NONE;

    bcm_port_config_get(0, &pcfg);

    BCM_PBMP_ITER(pcfg.sfi, port) {
        rv = bcm_stat_clear(0, port);
        if(rv !=  BCM_E_NONE) {
            printf("clear port %d counter error. \n", port);
            return rv;
        }
    }
    return BCM_E_NONE;
}

int sfi_stat_rxcrc_err_check(int unit, int isFE)
{
    uint64 temp_val;
    uint32 hi,lo;
    int port;
    bcm_port_config_t pcfg;
    uint32 link_status, errored_token_count = 0;

    COMPILER_64_ZERO(temp_val);
    bcm_port_config_get(0, &pcfg);
    if( isFE == 1) { /*FE device*/
        printf("FE device. \n");
        BCM_PBMP_ITER(pcfg.sfi, port) {
            bcm_fabric_link_status_get(unit, port, &link_status, &errored_token_count);
            if ( errored_token_count ) {
                printf("Failed,port:%d Errored tokens count expected 0,but got %d\n",port,errored_token_count);
                return BCM_E_FAIL;
            }
        }
    } else {
        BCM_PBMP_ITER(pcfg.sfi, port) {
            bcm_stat_get(0, port, snmpBcmRxCrcErrors, &temp_val);
            hi = COMPILER_64_HI(temp_val);
            lo = COMPILER_64_LO(temp_val);
            if (hi || lo) {
                printf("Failed,port:%d snmpBcmRxCrcErrors expected(0,0), but got(hi=%d,lo=%d)\n", port,hi,lo);
                return BCM_E_FAIL;
            }
        }
    }
    return BCM_E_NONE;
}


int sfi_stat_rxcrc_err_test(int unit, int isFE)
{
    int i, loop = 30;
    bcm_error_t rv = BCM_E_NONE;

    printf("Clear sfi port counter. \n");
    rv = sfi_stat_counter_clear(unit,isFE);
    if(rv !=  BCM_E_NONE) {
        return rv;
    }
    printf("Check pre configuration. \n");
    rv = sfi_stat_rxcrc_err_check(unit,isFE);
    if(rv !=  BCM_E_NONE) {
        return rv;
    }

    printf("Loop %d times,please wait...\n", loop);
    for(i = 0; i < loop; i++) {
        sfi_stat_bad_get(0);
        sal_sleep(1);
    }
    printf("Check test result. \n");
    return sfi_stat_rxcrc_err_check(unit,isFE);

}
