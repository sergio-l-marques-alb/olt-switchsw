/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_policy_bcmx.h
*
* @component hapi
*
* @create    3/18/2005
*
* @author    robp
*
* @end
*
**********************************************************************/

#ifndef BROAD_POLICY_BCMX_H
#define BROAD_POLICY_BCMX_H

#include "broad_policy_common.h"

/* Policy create/destroy doesn't really apply to a port but to a unit.
 * The port number is only used to extract specific unit numbers;
 * otherwise, bcmx would send it to all the units which is undesirable.
 */

int customx_port_policy_create(BROAD_POLICY_t        policyId,
                               BROAD_POLICY_ENTRY_t *policyEntry,
                               bcmx_lport_t          port);

int customx_port_policy_destroy(BROAD_POLICY_t policyId,
                                bcmx_lport_t   port);

int customx_port_policy_apply(BROAD_POLICY_t policyId,
                              bcmx_lport_t   port);

int customx_port_policy_check(BROAD_POLICY_t policyId,
                              bcmx_lport_t   port);

int customx_port_policy_remove(BROAD_POLICY_t policyId,
                               bcmx_lport_t   port);

int customx_policy_stats_get(BROAD_POLICY_t        policyId,
                             L7_uint32             numStats,
                             BROAD_POLICY_STATS_t *stats);

#endif /* BROAD_POLICY_BCMX_H */

