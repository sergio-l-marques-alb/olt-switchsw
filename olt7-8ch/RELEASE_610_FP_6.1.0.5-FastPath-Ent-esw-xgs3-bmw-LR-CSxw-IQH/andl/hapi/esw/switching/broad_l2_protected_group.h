/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_l2_protected_group.h
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

#ifndef BROAD_L2_PROTECTED_GROUP_H
#define BROAD_L2_PROTECTED_GROUP_H

#include "broad_common.h"
#include <bcm/types.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx_int.h>

/* Could not find a definition for max mod possible in the system */
#define L7_PROTECTED_GROUP_MOD_MAX 32

typedef struct
{
	bcm_pbmp_t mod_pbmp[L7_PROTECTED_GROUP_MOD_MAX];
} protectedGroup_t;

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
int l7_bcm_protected_group_set(int unit, protectedGroup_t *group_list);


#endif
