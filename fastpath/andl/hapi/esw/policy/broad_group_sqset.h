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

typedef struct
{
  bcm_field_qualify_t    *standardQualifiers;
  L7_uint32               standardQualifiersCount;
  custom_field_qualify_t *customQualifiers;
  L7_uint32               customQualifiersCount;
} super_qset_definition_t;


/* User Defined Fields (UDF) */
/* One UDF is currently allocated to System Policies to allow a combination of
 * InPorts, PacketFormat and a combination of L2/3/4 packet header fields that
 * are not supported by standard FPF settings.
 */
#define BROAD_SYSTEM_UDF customFieldQualifyUdf0

/* One UDF is currently allocated to iSCSI Control Packet Policies to allow a combination of
 * InPorts, PacketFormat and a combination of L2/3/4 packet header fields that
 * are not supported by standard FPF settings.
 */
#define BROAD_ISCSI_UDF customFieldQualifyUdf1

extern super_qset_definition_t l2SvtQsetDef;
extern super_qset_definition_t l2SvtLookupStatusQsetDef;
extern super_qset_definition_t l3l4QsetDef;
extern super_qset_definition_t l2l3SrcQsetDef;
extern super_qset_definition_t l2l3DstQsetDef;
extern super_qset_definition_t l2l3l4QsetDef;
extern super_qset_definition_t l2l3l4SrcMacGroupQsetDef;
extern super_qset_definition_t l2l3l4ClassIdQsetDef;
extern super_qset_definition_t l2l3l4Xgs4ClassIdQsetDef;
extern super_qset_definition_t ipv6SrcL4ClassIdQsetDef;
extern super_qset_definition_t ipv6DstL4ClassIdQsetDef;
extern super_qset_definition_t ipv6SrcL4QsetDef;
extern super_qset_definition_t ipv6DstL4QsetDef;
extern super_qset_definition_t vlanl3QsetDef;
extern super_qset_definition_t systemQsetDoubleDef;
extern super_qset_definition_t systemQsetDef;
extern super_qset_definition_t systemQsetTriumph2Def;
extern super_qset_definition_t iscsiQsetDef;
extern super_qset_definition_t ipv6NdQsetScorpionDef;
extern super_qset_definition_t ipv6NdQsetDef;
extern super_qset_definition_t ipv6L3L4QsetDef;
extern super_qset_definition_t ipv6L3L4ClassIdQsetDef;
extern super_qset_definition_t l2QsetEgressDef;
extern super_qset_definition_t l3l4QsetEgressDef;
extern super_qset_definition_t ipv6L3L4QsetEgressDef;
extern super_qset_definition_t l2l3l4QsetLookupDef;
extern super_qset_definition_t dot1adQsetLookupDef;
extern super_qset_definition_t llpfQsetLookupDef;
extern super_qset_definition_t ipv6L3L4QsetLookupDef;

#endif /* BROAD_GROUP_SQSET_H */
