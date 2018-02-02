/**
 * \file appl/diag/dnxc/legacy/port.h
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DIAG_DNXC_LEGACY_PORT_H_INCLUDED
#define DIAG_DNXC_LEGACY_PORT_H_INCLUDED

#include <bcm/port.h>

int diag_dnxc_phy_measure_port(
    int unit,
    bcm_port_t port,
    int is_rx,
    uint32 *rate_int,
    uint32 *rate_remainder);

#endif /* DIAG_DNXC_LEGACY_PORT_H_INCLUDED */
