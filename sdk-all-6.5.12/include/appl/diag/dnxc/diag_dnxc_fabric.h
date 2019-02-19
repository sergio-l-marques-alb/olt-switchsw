/*
 * ! \file diag_swstate_tests.h Purpose: shell registers commands for sw state tests 
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DIAG_DNXC_FABRIC_H_INCLUDED
#define DIAG_DNXC_FABRIC_H_INCLUDED

/* 
* { Local Defines
*/

#define DIAG_DNXC_OPTION_ALL                     -1

#define DNXF_DIAG_CGM_TH_TYPE_GCI                 0
#define DNXF_DIAG_CGM_TH_TYPE_RCI                 1
#define DNXF_DIAG_CGM_TH_TYPE_FC                  2
#define DNXF_DIAG_CGM_TH_TYPE_DROP                3
#define DNXF_DIAG_CGM_TH_TYPE_FIFO_SIZE           4
#define DNXF_DIAG_CGM_TH_TYPE_DYNAMIC_WEIGHT      5

#define DNXF_DIAG_CGM_TH_STAGE_RX                 0
#define DNXF_DIAG_CGM_TH_STAGE_MIDDLE             1
#define DNXF_DIAG_CGM_TH_STAGE_TX                 2
#define DNXF_DIAG_CGM_TH_STAGE_SHARED             3
#define DNXF_DIAG_CGM_TH_STAGE_All                4
/* 
* } Local Defines
*/

/* 
* { Local Structures
*/
typedef enum diag_threshold_group_options_e
{
    diag_stage_gci_option = DNXF_DIAG_CGM_TH_TYPE_GCI,
    diag_stage_rci_option = DNXF_DIAG_CGM_TH_TYPE_RCI,
    diag_stage_fc_option = DNXF_DIAG_CGM_TH_TYPE_FC,
    diag_stage_drop_option = DNXF_DIAG_CGM_TH_TYPE_DROP,
    diag_stage_size_option = DNXF_DIAG_CGM_TH_TYPE_FIFO_SIZE
} diag_threshold_group_options_t;

typedef enum diag_threshold_stage_options_e
{
    diag_stage_tx_option = DNXF_DIAG_CGM_TH_STAGE_TX,
    diag_stage_middle_option = DNXF_DIAG_CGM_TH_STAGE_MIDDLE,
    diag_stage_rx_option = DNXF_DIAG_CGM_TH_STAGE_RX,
    diag_stage_shared_option = DNXF_DIAG_CGM_TH_STAGE_SHARED
} diag_threshold_stage_options_t;

typedef enum diag_queue_stage_options_e
{
    diag_dch_stage_option = 0,
    diag_dtm_stage_option = 1,
    diag_dtl_stage_option = 2,
    diag_dfl_stage_option = 3
} diag_queue_stage_options_t;

/* 
* } Local Structures
*/
#endif /* DIAG_DNXC_FABRIC_H_INCLUDED */
