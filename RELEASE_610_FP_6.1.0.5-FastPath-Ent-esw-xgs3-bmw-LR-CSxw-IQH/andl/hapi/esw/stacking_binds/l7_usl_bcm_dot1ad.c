/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  l7_usl_bcm_dot1ad.c
*
* @component hapi
*
* @create    5/28/2009
*
* @author   Sudheer.M 
*
* @end
*
**********************************************************************/



#include  "l7_usl_bcm_dot1ad.h"

extern L7_int32 l7_bcm_dot1ad_vfp_rule_delete(L7_int32 portGroupId);
extern L7_int32 l7_bcm_dot1ad_vfp_rule_add(bcm_dot1ad_rule_entry_t *dot1adRule);

L7_RC_t usl_dot1ad_init()
{
  return L7_SUCCESS;
}

int usl_dot1ad_policy_create(bcm_dot1ad_rule_entry_t *dot1adRule)
{

  int                        rv = BCM_E_NONE;

  rv = l7_bcm_dot1ad_vfp_rule_add (dot1adRule);
  if (rv != BCM_E_NONE)
  {
    rv = BCM_E_TIMEOUT;
  }
  return rv;
}

int usl_dot1ad_policy_delete(L7_int32 unit, L7_int32 portGroupId)
{

  int                        rv = BCM_E_NONE;

  rv = l7_bcm_dot1ad_vfp_rule_delete (portGroupId);

  if (BCM_E_NONE!= rv)
  {
    rv = BCM_E_TIMEOUT;
  }
  return rv;
}



