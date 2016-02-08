/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_l2_protected_group.c
*
* @purpose   This file contains the custom bcm layer implementation for protected port
*
* @component protected port
*
* @comments
*
* @create    2/5/2006
*
* @author    nshrivastav
*
* @end
*
**********************************************************************/

#include "broad_l2_protected_group.h"

/*********************************************************************
*
* @purpose  Custom bcm function to configure a group of protected ports
*
* @param   unit        @b{(input)}   unit number
* @param   group_list  @b{(input)}   Pointer to list of protected ports
*
* @returns Defined by Broadcom driver
* @notes   none
*
* @end
*
*********************************************************************/
int l7_bcm_protected_group_set(int unit, protectedGroup_t *group_list)
{
  int        my_modid = 0, mod, port, rv;
  bcm_pbmp_t my_pbmp, tmp_pbmp;

  rv = bcm_stk_my_modid_get(unit, &my_modid);
  if (rv != BCM_E_NONE) {
	  return rv;
  }

  BCM_PBMP_CLEAR(tmp_pbmp);
  BCM_PBMP_CLEAR(my_pbmp);

  BCM_PBMP_ASSIGN(tmp_pbmp, group_list->mod_pbmp[my_modid]);
  BCM_PBMP_NEGATE(my_pbmp, tmp_pbmp);

  for (mod = 0; mod < L7_PROTECTED_GROUP_MOD_MAX; mod++) 
  {
    BCM_PBMP_CLEAR(tmp_pbmp);
	BCM_PBMP_ASSIGN(tmp_pbmp, group_list->mod_pbmp[mod]);
	if (BCM_PBMP_IS_NULL(tmp_pbmp))
	{
	  continue;
	}

	BCM_PBMP_ITER(tmp_pbmp, port)
	{
      if (SOC_IS_TUCANA(unit)) 
	  {
	      rv = bcm_port_egress_set(unit, SOC_PORT_MOD_OFFSET(unit, port), mod, my_pbmp);
    }
    else 
	  {
	    rv = bcm_port_egress_set(unit, port, mod, my_pbmp);
      }

	  if (rv < 0) 
	  {
	    return rv;
	  }
	}
  }

  return BCM_E_NONE;
}

