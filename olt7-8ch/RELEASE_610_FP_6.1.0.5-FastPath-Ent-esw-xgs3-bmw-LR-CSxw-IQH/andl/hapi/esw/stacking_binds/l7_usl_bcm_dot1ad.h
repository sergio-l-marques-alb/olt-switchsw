/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  l7_usl_bcm_dot1ad.h
*
* @component hapi
*
* @create    3/18/2005
*
* @author   Sudheer.M 
*
* @end
*
**********************************************************************/


#ifndef BROAD_DOT1AD_USL_H
#define BROAD_DOT1AD_USL_H

#include "broad_policy_common.h"

L7_RC_t usl_dot1ad_init();

int usl_dot1ad_policy_create(bcm_dot1ad_rule_entry_t *dot1adRule);
int usl_dot1ad_policy_delete(L7_int32 unit, L7_int32 portGroupId);

#endif /* BROAD_POLICY_USL_H */




