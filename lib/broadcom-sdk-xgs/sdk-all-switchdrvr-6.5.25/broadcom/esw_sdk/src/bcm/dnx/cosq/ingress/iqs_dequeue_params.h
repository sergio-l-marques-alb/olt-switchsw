/** \file iqs_dequeue_params.h
 * $Id$
 *
 * Module IQS - Ingress Queue Scheduling
 *
 * Configurations of dequeue command parameters
 *
 */

/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNX_IQS_DEQUEUE_PARAMS_INCLUDED__
#define _DNX_IQS_DEQUEUE_PARAMS_INCLUDED__

#ifndef BCM_DNX2_SUPPORT
#error "This file is for use by DNX family only!"
#endif

shr_error_e dnx_iqs_dequeue_params_init(
    int unit);

#endif/*_DNX_IQS_DEQUEUE_PARAMS_INCLUDED__*/
