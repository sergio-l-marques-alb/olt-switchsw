/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2010
*
**********************************************************************
*
* @filename  broad_group_sqset.h
*
* @component hapi
*
* @create    1/27/2010
*
* @author    colinw
*
* @end
*
**********************************************************************/
#ifndef BROAD_GROUP_SQSET_H
#define BROAD_GROUP_SQSET_H

#include "datatypes.h"
#include "broad_group_bcm.h"
#include "bcm/field.h"

typedef enum 
{
  sqsetWidthFirst = 0,
  sqsetWidthSingle = sqsetWidthFirst,
  sqsetWidthDoubleIntraslice,
  sqsetWidthDouble,
  sqsetWidthQuad,
  sqsetWidthLast
} sqsetWidth_t;

typedef enum 
{
  sasetWidthFirst = 0,
  sasetWidthSingle = sqsetWidthFirst,
  sasetWidthDouble,
  sasetWidthLast
} sasetWidth_t;

typedef struct
{
  bcm_field_qualify_t    *standardQualifiers;
  L7_uint32               standardQualifiersCount;
  custom_field_qualify_t *customQualifiers;
  L7_uint32               customQualifiersCount;

  bcm_field_action_t     *standardActions;
  L7_uint32               standardActionsCount;
} super_xset_definition_t;


extern super_xset_definition_t systemXsetAradDef;
extern super_xset_definition_t systemQsetPTinDef;     /* PTin added: ICAP */

extern super_xset_definition_t l3l4QsetDef;
extern super_xset_definition_t l2l3SrcQsetDef;
extern super_xset_definition_t l2l3DstQsetDef;
extern super_xset_definition_t l2l3l4Xgs4ClassIdQsetDef;
extern super_xset_definition_t vlanl3QsetDef;
extern super_xset_definition_t ipv6SrcL4QsetDef;
extern super_xset_definition_t ipv6DstL4QsetDef;

extern super_xset_definition_t l2QsetEgressDef;
extern super_xset_definition_t l3l4QsetEgressDef;

#endif /* BROAD_GROUP_SQSET_H */

