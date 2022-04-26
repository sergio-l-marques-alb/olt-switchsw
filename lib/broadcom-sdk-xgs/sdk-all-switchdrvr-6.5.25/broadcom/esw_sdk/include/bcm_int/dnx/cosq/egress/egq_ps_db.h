/**
 * \file bcm_int/dnx/cosq/egress/egq_ps_db.h
 *
 *
 * General Description:
 *
 * Purpose:.
 *
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef EGQ_PS_DB_INCLUDED
/* { */
#define EGQ_PS_DB_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (Jr2) family only!"
#endif

#include <bcm/types.h>

int dnx_ps_db_base_qpair_alloc(
    int unit,
    bcm_port_t port,
    int port_interface,
    int is_channelized,
    bcm_core_t core,
    int out_port_priority,
    int priority_propagation_enable,
    int *base_q_pair);
int dnx_ps_db_base_qpair_alloc_with_id(
    int unit,
    bcm_port_t port,
    int port_interface,
    int is_channelized,
    bcm_core_t core,
    int out_port_priority,
    int base_q_pair);
int dnx_ps_db_base_qpair_free(
    int unit,
    bcm_port_t port,
    int base_q_pair,
    int *free_ps);

/* } */
#endif /* EGQ_PS_DB_INCLUDED */
