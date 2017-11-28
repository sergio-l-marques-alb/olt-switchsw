 /** \file ingress_congestion_tune.c
 * Tuning of Ingress Congestion parameters
 * 
 * NOTE: as this code is invoked from iside SDK, 
 * API invocation is done via bcm_dnx_XXX functions.
 * When this code is taken outside SDK, 
 * these calls should be replaced by bcm_XXX functions.
 */

/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/fabric.h>
#include <bcm/types.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnxf_dispatch.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_port.h>

#include <soc/error.h>

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/mbcm.h>

#include <soc/dnxf/ramon/ramon_cgm.h>

#include "cgm_tune.h"

/*
 * }
 * Include files.
 */

#define BCM_DNXF_CGM_TUNE_PROFILE_0              (0)
#define BCM_DNXF_CGM_TUNE_TH_DISABLE             (-1)
#define BCM_DNXF_CGM_TUNE_NUM_OF_RCI_LEVELS      (4)
#define BCM_DNXF_CGM_MAX_NOF_ROW_ELEMENTS        (4)
typedef struct threshold_descriptor_s
{
    bcm_fabric_threshold_type_t threshold_type; 
    bcm_fabric_threshold_id_t threshold_id;
    int  value;
    uint32 flags;
} threshold_descriptor_t;

/*
* { Single pipe mode
*/

/* Table with default CGM threshold values for Ramon in "single pipe" mode*/
static const threshold_descriptor_t cgm_init_thresholds_single_pipe_mode[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DCH / RX*/ {bcmFabricRxFifoSizePipeTh,               BCM_FABRIC_PIPE_ALL,                                                            768,                              0x0},
             {bcmFabricRxFragDropPipeTh,               BCM_FABRIC_PIPE_ALL,                                                            764,                              0x0},
             {bcmFabricRxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricRxLLFCFcPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  384,                              0x0},
/*DTM / MID*/{bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                1700,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                2100,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                2600,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                4000,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              4000,                             0x0},
             {bcmFabricMidFragDropClassTh,             BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     2000,                             0x0},
             {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     2200,                             0x0},
             {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     3050,                             0x0},
             {bcmFabricMidFragGciPipeTh,               BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     2420,                             0x0},
             {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     2620,                             0x0},
             {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     2830,                             0x0},
             {bcmFabricMidFragRciPipeTh,               BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFifoSizePipeTh,              0,                                                                              4608,                             0x0},
/*DTL / TX*/ {bcmFabricTxLinkLoadDropPipeTh,           BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragGuaranteedPipeTh,         BCM_FABRIC_PIPE_ALL,                                                            256,                              0x0},
             {bcmFabricTxFE1BypassLLFCFcPipeTh,        BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     120,                              0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     150,                              0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     180,                              0x0},
             {bcmFabricTxLinkLoadRciPipeTh,            BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                1700,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                2100,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                2600,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                4000,                             0x0},
             {bcmFabricTxFragDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     2000,                             0x0},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     2200,                             0x0},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     3000,                             0x0},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     2400,                             0x0},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     2600,                             0x0},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     2800,                             0x0},
             {bcmFabricTxFragRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagFcPipeTh,                  BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFifoSizePipeTh,               BCM_FABRIC_PIPE_ALL,                                                            4092,                             0x0},
/*DFL / SHR*/{bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                11389,                            0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                14225,                            0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                17800,                            0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                27120,                            0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              27120,                            0x0},
             {bcmFabricSharedMcCopiesDropPrioTh,       BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     13524,                            0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     14950,                            0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     20650,                            0x0},
             {bcmFabricSharedFragCopiesGciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     16370,                            0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     17800,                            0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     19200,                            0x0},
             {bcmFabricSharedFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedBankMcDropPrioTh,         BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0}
};
/*
* } Single pipe mode
*/

/*
* { Dual pipes mode
*/

/* Table with default CGM threshold values for Ramon in "dual pipes" mode*/


/*bcmFabricSharedGciPipeTh,bcmFabricSharedRciPipeTh have to write different values for Method 1 and Method 2 According to SDD which is not supported ! */
static const threshold_descriptor_t cgm_init_thresholds_dual_pipes_mode[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DCH / RX*/ {bcmFabricRxFifoSizePipeTh,               BCM_FABRIC_PIPE_ALL,                                                            384,                              0x0},
             {bcmFabricRxFragDropPipeTh,               BCM_FABRIC_PIPE_ALL,                                                            380,                              0x0},
             {bcmFabricRxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricRxLLFCFcPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  192,                              0x0},
/*DTM / MID*/{bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                850,                              0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                1050,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                1300,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                2000,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              2000,                             0x0},
             {bcmFabricMidFragDropClassTh,             BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                       1000,                             0x0},
             {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                       1100,                             0x0},
             {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                       1525,                             0x0},
             {bcmFabricMidFragGciPipeTh,               BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                       1210,                             0x0},
             {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                       1310,                             0x0},
             {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                       1415,                             0x0},
             {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFragRciPipeTh,               BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFifoSizePipeTh,              0,                                                                              2304,                             0x0},
             {bcmFabricMidFifoSizePipeTh,              1,                                                                              2304,                             0x0},
/*DTL / TX*/ {bcmFabricTxLinkLoadDropPipeTh,           BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFE1BypassLLFCFcPipeTh,        BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                       60,                               0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                       75,                               0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                       90,                               0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxLinkLoadRciPipeTh,            BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                850,                              0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                1050,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                1300,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                2000,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastAll, BCM_FABRIC_PRIO_ALL),             BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                       1000,                             0x0},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                       1100,                             0x0},
             {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                       1500,                             0x0},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                       1200,                             0x0},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                       1300,                             0x0},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                       1400,                             0x0},
             {bcmFabricTxFragRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagFcPipeTh,                  BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFifoSizePipeTh,               BCM_FABRIC_PIPE_ALL,                                                            2044,                             0x0},
/*DFL / SHR*/{bcmFabricSharedMcCopiesDropPrioTh,       BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedFragCopiesGciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedBankMcDropPrioTh,         BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0}
};

/* Table with CGM threshold values for Ramon in "dual pipes" mode which should overwrite or add to the default 2 pipe tuning values
   if the mode is:
   PIPE 0 - UC 
   PIPE 1 - MC
  */
static const threshold_descriptor_t cgm_init_thresholds_pipe_map_dual_uc_mc[] =
{
    /* Threshold Type                                  | Threshold ID                                                                 | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DTL / TX*/ {bcmFabricTxFragGuaranteedPipeTh,         BCM_FABRIC_PIPE_ALL,                                                            128,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                5700,                            0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                7100,                            0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                8900,                            0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                13560,                           0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              13560,                           0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     6750,                            0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     7475,                            0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     10320,                           0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     8200,                            0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     8900,                            0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     9600,                            0x0}


};

/* Table with CGM threshold values for Ramon in mode with two pipes which should overwrite or add to the default 2 pipe tuning values
   if the pipe mapping is:
   PIPE 0 - Trafic (UC+MC)
   PIPE 1 - TDM
  */
static const threshold_descriptor_t cgm_init_thresholds_pipe_map_dual_tdm_non_tdm[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DTL / TX*/ {bcmFabricTxFragGuaranteedPipeTh,         0,                                                                              128,                              0x0},
             {bcmFabricTxFragGuaranteedPipeTh,         1,                                                                              84,                               0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                6150,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                7670,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                9600,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                14600,                            0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              14600,                            0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     7300,                             0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     8050,                             0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     11125,                            0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     8820,                             0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     9600,                             0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     10350,                            0x0}
};

/*
* } Two pipes mode
*/

/*
* { Three pipes mode
*/

/* Table with default CGM threshold values for Ramon in "thriple pipes" mode*/


/*bcmFabricSharedGciPipeTh,bcmFabricSharedRciPipeTh have to write different values for Method 1 and Method 2 According to SDD which is not supported ! */
static const threshold_descriptor_t cgm_init_thresholds_thriple_pipes_mode[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DCH / RX*/ {bcmFabricRxFifoSizePipeTh,               BCM_FABRIC_PIPE_ALL,                                                            256,                              0x0},
             {bcmFabricRxFragDropPipeTh,               BCM_FABRIC_PIPE_ALL,                                                            252,                              0x0},
             {bcmFabricRxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricRxLLFCFcPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  128,                              0x0},
/*DTM / MID*/{bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                550,                              0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                700,                              0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                850,                              0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                1300,                             0x0},
             {bcmFabricMidTagDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              1300,                             0x0},
             {bcmFabricMidFragDropClassTh,             BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFragGciPipeTh,               BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFragRciPipeTh,               BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricMidFifoSizePipeTh,              0,                                                                              1536,                             0x0},
             {bcmFabricMidFifoSizePipeTh,              1,                                                                              1536,                             0x0},
/*DTL / TX*/ {bcmFabricTxLinkLoadDropPipeTh,           BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFE1BypassLLFCFcPipeTh,        BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                       45,                               0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                       50,                               0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                       60,                               0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragGuaranteedRciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, BCM_FABRIC_LEVEL_ALL),                    BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxLinkLoadRciPipeTh,            BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                550,                              0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                700,                              0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                850,                              0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                1300,                             0x0},
             {bcmFabricTxTagDropClassTh,               BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, BCM_FABRIC_PRIO_ALL),              BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxTagFcPipeTh,                  BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFragFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricTxFifoSizePipeTh,               BCM_FABRIC_PIPE_ALL,                                                            1360,                             0x0},
/*DFL / SHR*/{bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 0),                                3800,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 1),                                4750,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 2),                                5930,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastMC, 3),                                9040,                             0x0},
             {bcmFabricSharedDropClassTh,              BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(bcmCastUC, BCM_FABRIC_PRIO_ALL),              9040,                             0x0},
             {bcmFabricSharedMcCopiesDropPrioTh,       BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     4500,                             0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     5000,                             0x0},
             {bcmFabricSharedGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     6900,                             0x0},
             {bcmFabricSharedFragCopiesGciPipeTh,      BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, BCM_FABRIC_LEVEL_ALL),  BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 0),                     5450,                             0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 1),                     5931,                             0x0},
             {bcmFabricSharedRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(BCM_FABRIC_PIPE_ALL, 2),                     6400,                             0x0},
             {bcmFabricSharedFcPipeTh,                 BCM_FABRIC_PIPE_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0},
             {bcmFabricSharedBankMcDropPrioTh,         BCM_FABRIC_PRIO_ALL,                                                            BCM_DNXF_CGM_TUNE_TH_DISABLE,     0x0}

};
/* Table with CGM threshold values for Ramon in mode with three pipes which should overwrite or add to the default 3 pipe tuning values
   if the pipe mapping is:
   PIPE 0  - UC
   PIPE 1  - MC
   PIPE 2  - TDM
*/
static const threshold_descriptor_t cgm_init_thresholds_pipe_map_triple_uc_mc_tdm[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DTM/MID*/ {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                         650,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                         715,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                         990,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                         785,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                         850,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                         920,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                         BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 0),                                         785,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 1),                                         850,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 2),                                         920,                             0x0},
/*DTL / TX*/{bcmFabricTxFragGuaranteedPipeTh,         BCM_FABRIC_PIPE_ALL,                                                              84,                              0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                         650,                             0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                         700,                             0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                         1000,                            0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                         780,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                         850,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                         920,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 0),                                         780,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 1),                                         850,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 2),                                         920,                             0x0}
};

/* Table with CGM threshold values for Ramon in mode with three pipes which should overwrite or add to the default 3 pipe tuning values
   if the pipe mapping is:
   PIPE 0 - UC
   PIPE 1 - MC-High Prio
   PIPE 2 - MC-Low Prio
  */
static const threshold_descriptor_t cgm_init_thresholds_pipe_map_triple_uc_hp_mc_lp_mc[] =
{
    /* Threshold Type                                  | Threshold ID                                                                  | Value                          | Flags */
    /* ================================================================================================================================================================================== */
/*DTM/MID*/ {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                         650,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                         715,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                         990,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 0),                                         650,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 1),                                         715,                             0x0},
            {bcmFabricMidTagGciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 2),                                         990,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                         785,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                         850,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                         920,                             0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricMidTagRciPipeTh,                BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
/*DTL / TX*/{bcmFabricTxFragGuaranteedPipeTh,         0,                                                                                84,                              0x0},
            {bcmFabricTxFragGuaranteedPipeTh,         1,                                                                                42,                              0x0},
            {bcmFabricTxFragGuaranteedPipeTh,         2,                                                                                42,                              0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 0),                                         650,                             0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 1),                                         700,                             0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, 2),                                         1000,                            0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 0),                                         650,                             0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 1),                                         700,                             0x0},
            {bcmFabricTxTagGciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, 2),                                         1000,                            0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 0),                                         780,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 1),                                         850,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(0, 2),                                         920,                             0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(1, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0},
            {bcmFabricTxTagRciPipeTh,                 BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(2, BCM_FABRIC_LEVEL_ALL),                      BCM_DNXF_CGM_TUNE_TH_DISABLE,    0x0}
};

/*
* } Three pipes mode
*/

static const uint32 bcm_dnxf_rci_map_single_pipe[BCM_DNXF_CGM_TUNE_NUM_OF_RCI_LEVELS][BCM_DNXF_CGM_TUNE_NUM_OF_RCI_LEVELS] =
             {  /*             grnt0    grnt1   grnt2   grnt3     */
                /* shr0 */     {0,      0,      0,      1},
                /* shr1 */     {1,      1,      1,      1},
                /* shr2 */     {2,      2,      2,      2},
                /* shr3 */     {3,      3,      3,      3},
             };

/**
 * \brief -  Tune congestion thresholds
 * 
 */

shr_error_e
dnxf_cgm_tune_init(
    int unit)
{
    uint32 flags, nof_table_entries;
    int table_entry_idx,value, nof_pipes, links_count = 0;
    bcm_fabric_threshold_id_t threshold_id;
    bcm_fabric_threshold_type_t threshold_type;
    bcm_port_t links[DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS], link;
    uint32 profile_attach_flags = 0x0;
    uint32 shared_rci, guaranteed_rci;
    bcm_fabric_rci_resolution_key_t key;
    bcm_fabric_rci_resolution_config_t config;
    const threshold_descriptor_t * cgm_init_values_table;

    SHR_FUNC_INIT_VARS(unit);

    nof_pipes = dnxf_data_fabric.pipes.nof_pipes_get(unit);

    /* Get the correct init vlaues table in dependace of the operational mode of the  device*/
    switch(nof_pipes)
    {
        case 1: nof_table_entries = sizeof(cgm_init_thresholds_single_pipe_mode)/sizeof(threshold_descriptor_t);
                cgm_init_values_table = cgm_init_thresholds_single_pipe_mode;
                break;

        case 2: nof_table_entries = sizeof(cgm_init_thresholds_dual_pipes_mode)/sizeof(threshold_descriptor_t);
                cgm_init_values_table = cgm_init_thresholds_dual_pipes_mode;
                break;

        case 3: nof_table_entries = sizeof(cgm_init_thresholds_thriple_pipes_mode)/sizeof(threshold_descriptor_t);
                cgm_init_values_table = cgm_init_thresholds_thriple_pipes_mode;
                break;
        default: SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid number of pipes received!");
                break;
    }

    /* Configure CGM threhold for each entry in the Deafult init values table*/
    for (table_entry_idx=0; table_entry_idx < nof_table_entries; table_entry_idx++)
    {
        threshold_type = cgm_init_values_table[table_entry_idx].threshold_type;
        threshold_id = cgm_init_values_table[table_entry_idx].threshold_id;
        flags = cgm_init_values_table[table_entry_idx].flags;
        value = cgm_init_values_table[table_entry_idx].value;

        SHR_IF_ERR_EXIT(bcm_dnxf_fabric_profile_threshold_set(unit, BCM_DNXF_CGM_TUNE_PROFILE_0, threshold_id, threshold_type, flags, value));

    }

    if (nof_pipes  == 1) {
        /*No need for additional CGM tuning*/
        nof_table_entries = 0;
    } else {
        /* Get additional init values specific for each of the pipe mapping possibilities */
        switch (dnxf_data_fabric.pipes.map_get(unit)->type)
        {
            case soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm:
                nof_table_entries = sizeof(cgm_init_thresholds_pipe_map_triple_uc_mc_tdm)/sizeof(threshold_descriptor_t);
                cgm_init_values_table = cgm_init_thresholds_pipe_map_triple_uc_mc_tdm;
                break;
            case soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc:
                nof_table_entries = sizeof(cgm_init_thresholds_pipe_map_triple_uc_hp_mc_lp_mc)/sizeof(threshold_descriptor_t);
                cgm_init_values_table = cgm_init_thresholds_pipe_map_triple_uc_hp_mc_lp_mc;
                break;
            case soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm:
                nof_table_entries = sizeof(cgm_init_thresholds_pipe_map_dual_tdm_non_tdm)/sizeof(threshold_descriptor_t);
                cgm_init_values_table = cgm_init_thresholds_pipe_map_dual_tdm_non_tdm;
                break;
            case soc_dnxc_fabric_pipe_map_dual_uc_mc:
                nof_table_entries = sizeof(cgm_init_thresholds_pipe_map_dual_uc_mc)/sizeof(threshold_descriptor_t);
                cgm_init_values_table = cgm_init_thresholds_pipe_map_dual_uc_mc;
                break;
            case soc_dnxc_fabric_pipe_map_single:
                /*No need for additional CGM tuning*/
                nof_table_entries = 0;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid pipe mapping vlaue received!");
                break;
        };
    }


    for (table_entry_idx=0; table_entry_idx < nof_table_entries; table_entry_idx++)
    {
        threshold_type = cgm_init_values_table[table_entry_idx].threshold_type;
        threshold_id = cgm_init_values_table[table_entry_idx].threshold_id;
        flags = cgm_init_values_table[table_entry_idx].flags;
        value = cgm_init_values_table[table_entry_idx].value;

        SHR_IF_ERR_EXIT(bcm_dnxf_fabric_profile_threshold_set(unit, BCM_DNXF_CGM_TUNE_PROFILE_0, threshold_id, threshold_type, flags, value));
    }

    /*Configure all link to CGM profile 0*/
    SOC_PBMP_ITER(SOC_INFO(unit).sfi.bitmap, link)
    {
        links[links_count] = link;
        links_count++;
    }

    SHR_IF_ERR_EXIT(bcm_dnxf_fabric_link_profile_set(unit, BCM_DNXF_CGM_TUNE_PROFILE_0, profile_attach_flags, links_count, links));

    /* Map RCI levels */
    for (shared_rci = 0 ; shared_rci < BCM_DNXF_CGM_TUNE_NUM_OF_RCI_LEVELS ; shared_rci++)
    {
        for (guaranteed_rci = 0 ; guaranteed_rci < BCM_DNXF_CGM_TUNE_NUM_OF_RCI_LEVELS ; guaranteed_rci++)
        {
            key.pipe = BCM_FABRIC_PIPE_ALL;
            key.shared_rci = shared_rci;
            key.guaranteed_rci = guaranteed_rci;
            config.resolved_rci = bcm_dnxf_rci_map_single_pipe[shared_rci][guaranteed_rci];
            SHR_IF_ERR_EXIT(bcm_dnxf_fabric_rci_resolution_set(unit, 0x0, &key, &config));
        }
    }

exit:
    SHR_FUNC_EXIT;
}




