/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_policy_bcm.h
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

#ifndef BROAD_POLICY_BCM_H
#define BROAD_POLICY_BCM_H

#include "broad_policy_common.h"

/* common init function for all bcm policy functions */
int custom_policy_init();

int custom_policy_set_handler(int unit, bcm_port_t port, int setget, uint32 *args);

int custom_policy_get_handler(int unit, bcm_port_t port, int setget, uint32 *args);

#endif /* BROAD_POLICY_BCM_H */
