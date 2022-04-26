/*
 * $Id: ramon_fabric_cell.h,v 1.6 Broadcom SDK $
 *
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * RAMON COSQ H
 */

#ifndef _SOC_RAMON_COSQ_H_
#define _SOC_RAMON_COSQ_H_

#ifndef BCM_DNXF1_SUPPORT
#error "This file is for use by DNXF1 (Ramon) family only!"
#endif

/**********************************************************/
/*                  Includes                              */
/**********************************************************/

#include <soc/error.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/types.h>

/**********************************************************/
/*                  Defines                               */
/**********************************************************/
#define SOC_RAMON_COSQ_WFQ_WEIGHT_MAX                127
#define SOC_RAMON_COSQ_WFQ_WEIGHT_IGNORE             1
#define SOC_RAMON_COSQ_WFQ_WEIGHT_NOF_BITS           7

#define SOC_RAMON_COSQ_RATE_DCH_PERIOD_MAX           0xf
#define SOC_RAMON_COSQ_RATE_DCL_DEC_INC_VAL_MAX      0xff
#define SOC_RAMON_COSQ_RATE_DCL_DEC_INC_NOF_BITS     8
/**********************************************************/
/*                  Functions                             */
/**********************************************************/

/* pipe wfq weight functions */
shr_error_e soc_ramon_cosq_pipe_weight_set(
    int unit,
    soc_dnxf_cosq_pipe_type_t pipe_type,
    int pipe,
    int port,
    soc_dnxf_cosq_weight_mode_t mode,
    int weight);

shr_error_e soc_ramon_cosq_pipe_weight_get(
    int unit,
    soc_dnxf_cosq_pipe_type_t pipe_type,
    int pipe,
    int port,
    soc_dnxf_cosq_weight_mode_t mode,
    int *weight);

#endif /* !_SOC_RAMON_COSQ_H_ */
