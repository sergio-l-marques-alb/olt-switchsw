/*
 * $Id: cmicx.h,v 1.40 Broadcom SDK $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        intr_cmicx.h
 * Purpose:     CMICX interrupt include file
 */
#ifndef _SOC_INTR_CMICX_H
#define _SOC_INTR_CMICX_H

#include <soc/intr.h>
#include <soc/mcm/intr_cmicx.h>


#define CMICX_INTR_REG_NUM     (8)
#define CMCx_SBUSDMA_CHy_BASE  (CMC0_SBUSDMA_CH0_DONE)
#define INTR_SBUSDMA(x, y)     (CMC0_SBUSDMA_CH0_DONE + 48*x + y)
#define INTR_SCHAN(x)          (SCHAN_CH0_OP_DONE + x)
#define INTR_SCHAN_FIFO(x)     (SCHAN_FIFO_CH0_DONE + x)
#define INTR_FIFO_DMA(x)       (FIFO_CH0_DMA_INTR + x)

typedef enum cmic_intr_num_ser_type_e {
L2_MGMT_TO_CMIC_INTR = 129,
PVTMON_INTR,
PLL_LOCK_LOSS_INTR,
MMU_TO_CMIC_MEMFAIL_INTR,
PIPE0_EP_TO_CMIC_PERR_INTR,
PIPE1_EP_TO_CMIC_PERR_INTR,
PIPE0_IDB_TO_CMIC_PERR_INTR,
PIPE1_IDB_TO_CMIC_PERR_INTR,
PIPE0_IP_TO_CMIC_PERR_INTR,
PIPE1_IP_TO_CMIC_PERR_INTR,
PIPE0_ISW4_TO_CMIC_PERR_INTR,
PIPE1_ISW4_TO_CMIC_PERR_INTR,
AVS_SW_MDONE_INTR = 145,
AVS_ROSC_THRESHOLD2_INTR,
AVS_ROSC_THRESHOLD1_INTR,
AVS_POWERWATCHDOG_INTR,
AVS_VDDC_MON_WARNING1_INTR,
AVS_VDDC_MON_WARNING0_INTR,
AVS_TEMPERATURE_RESET_INTR,
AVS_TEMPERATURE_LOW_THRESHOLD_INTR,
AVS_TEMPERATURE_HIGH_THRESHOLD_INTR,
VTMON_OVERTRMP_RESET_INTR,
PM0_INTR = 160,
PM1_INTR,
PM2_INTR,
PM3_INTR,
PM4_INTR,
PM5_INTR,
PM6_INTR,
PM7_INTR,
PM8_INTR,
PM9_INTR,
PM10_INTR,
PM11_INTR,
PM12_INTR,
PM13_INTR,
PM14_INTR,
PM15_INTR,
PM16_INTR,
PM17_INTR,
PM18_INTR,
PM19_INTR,
PM20_INTR,
PM21_INTR,
PM22_INTR,
PM23_INTR,
PM24_INTR,
PM25_INTR,
PM26_INTR,
PM27_INTR,
PM28_INTR,
PM29_INTR,
PM30_INTR,
PM31_INTR,
PM32_INTR,
CMIC_INTERRUPT_NUM_ALL_MAX = 256
} cmic_intr_num_ser_type_e;

/*******************************************
* @function soc_cmicx_intr_init
* purpose initialize CMICX interrupt framework
* @param unit [out] soc_cmic_intr_op_t pointer
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
********************************************/
extern int
soc_cmicx_intr_init(int unit, soc_cmic_intr_op_t *intr_op);

/*******************************************
* @function soc_cmicx_intr
* purpose SOC CMICX Interrupt Service Routine
*
* @param unit [in] unit
*
*
* @end
********************************************/
extern void
soc_cmicx_intr(void *_unit);

#endif  /* !_SOC_INTR_CMICX_H */
