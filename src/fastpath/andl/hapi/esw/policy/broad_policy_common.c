/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_policy_common.c
*
* This file implements common policy code that can be used in any unit. 
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

#include "broad_policy_common.h"
#include "l7_rpc_policy.h"
#include "l7_usl_policy_db.h"

/* Standard size of each field (in bytes). */
static int broadFieldMapTable[BROAD_FIELD_LAST] =
{
  BROAD_FIELD_MACDA_SIZE,
  BROAD_FIELD_MACSA_SIZE,
  BROAD_FIELD_ETHTYPE_SIZE,
  BROAD_FIELD_OVID_SIZE,
  BROAD_FIELD_IVID_SIZE,
  BROAD_FIELD_DSCP_SIZE,
  BROAD_FIELD_PROTO_SIZE,
  BROAD_FIELD_SIP_SIZE,
  BROAD_FIELD_DIP_SIZE,
  BROAD_FIELD_SPORT_SIZE,
  BROAD_FIELD_DPORT_SIZE,
  BROAD_FIELD_IP6_HOPLIMIT_SIZE,
  BROAD_FIELD_IP6_NEXTHEADER_SIZE,
  BROAD_FIELD_LOOKUP_STATUS_SIZE,
  BROAD_FIELD_IP6_SRC_SIZE,
  BROAD_FIELD_IP6_DST_SIZE,
  BROAD_FIELD_IP6_FLOWLABEL_SIZE,
  BROAD_FIELD_IP6_TRAFFIC_CLASS_SIZE,
  BROAD_FIELD_ICMP_MSG_TYPE_SIZE,
  //BROAD_FIELD_CLASS_ID_SIZE,
  BROAD_FIELD_L2_CLASS_ID_SIZE,
  BROAD_FIELD_ISCSI_OPCODE_SIZE,
  BROAD_FIELD_ISCSI_OPCODE_TCP_OPTIONS_SIZE,
  BROAD_FIELD_TCP_CONTROL_SIZE, 
  BROAD_FIELD_VLAN_FORMAT_SIZE,
  BROAD_FIELD_L2_FORMAT_SIZE,
  BROAD_FIELD_SNAP_SIZE,
  BROAD_FIELD_IP_TYPE_SIZE,
  BROAD_FIELD_INPORTS_SIZE,         /* PTin added: FP */
  BROAD_FIELD_OUTPORT_SIZE,         /* PTin added: FP */
  BROAD_FIELD_SRCTRUNK_SIZE,        /* PTin added: FP */
  BROAD_FIELD_PORTCLASS_SIZE,       /* PTin added: FP */
  BROAD_FIELD_DROP_SIZE,            /* PTin added: FP */
  //BROAD_FIELD_L2_STATION_MOVE_SIZE  /* PTin added: FP */
};

static char *broadFieldNameTable[BROAD_FIELD_LAST] =
{
    "MACDA  ",
    "MACSA  ",
    "ETHTYPE",
    "OVID   ",
    "IVID   ",
    "DSCP   ",
    "PROTO  ",
    "SIP    ",
    "DIP    ",
    "SPORT  ",
    "DPORT  ",
    "IP6_HOPLIMIT",
    "IP6_NEXTHEADER",
    "LOOKUP_STATUS",
    "IP6_SRC",
    "IP6_DST",
    "IP6_FLOWLABEL",
    "IP6_TRAFFIC_CLASS",
    "ICMP_MSG_TYPE",
    //"CLASS_ID",
    "L2_CLASS_ID",
    "ISCSI_OPCODE",
    "ISCSI_OPCODE_TCP_OPTIONS",
    "TCP_CONTROL",
    "VLAN_FORMAT",
    "L2_FORMAT",
    "SNAP",
    "IP_TYPE",
    "INPORTS",          /* PTin added: FP */
    "OUTPORT" ,         /* PTin added: FP */
    "SRCTRUNK",         /* PTin added: FP */
    "PORTCLASS",        /* PTin added: FP */
    "DROP_PACKET",      /* PTin added: FP */
    //"L2_STATION_MOVE"   /* PTin added: FP */
};

static char *broadActionNameTable[BROAD_ACTION_LAST] =
{
    "SOFTDROP",
    "HARDDROP",
    "PERMIT  ",
    "REDIRECT",
    "MIRROR  ",
    "TRAP2CPU",
    "COPY2CPU",
    "SETCOSQ ",
    "SETDSCP ",
    "SETTOS  ",
    "SETUSERP",
    "SETDPREC",
    "SETOVID ",
    "SETIVID ",
    "ADDOVID ",
    "ADDIVID ",
    "NOLEARN ",
    "SETCLASS",
    "SETREASON",
    "COPYUSERPFROMINNERTAG",
};

static char* broadTypeNameTable[BROAD_POLICY_TYPE_LAST] =
{
    "PORT       ",
    "DOT1AD     ",
    "IPSG       ",
    "VLAN       ",
    "ISCSI      ",
    "SYSTEM_PORT",
    "SYSTEM     ",
    "COSQ       ",
    "DVLAN      ",
    "DOT1ADSNOOP",
    "LLPF       ",
    "PTIN       ",
    "STAT_EVC   ",
    "STAT_CLIENT"
};

int hapiBroadPolicyFieldSize(BROAD_POLICY_FIELD_t field)
{
    if (field >= BROAD_FIELD_LAST)
        LOG_ERROR(field);

    return broadFieldMapTable[field];
}

char *hapiBroadPolicyFieldName(BROAD_POLICY_FIELD_t field)
{
    if (field >= BROAD_FIELD_LAST)
        return "INVALID";

    return broadFieldNameTable[field];
}

void hapiBroadPolicyFieldFlagsSet(BROAD_FIELD_ENTRY_t *fieldInfo, BROAD_POLICY_FIELD_t field, L7_uchar8 value)
{
  if (field >= BROAD_FIELD_LAST)
      LOG_ERROR(field);

  if (field >= 32)
  {
    /* Need to add another 'flags' field to BROAD_FIELD_ENTRY_t */
    LOG_ERROR(field);
  }

  if (value == BROAD_FIELD_SPECIFIED)
  {
    fieldInfo->flags |= (1 << field);
  }
  else
  {
    fieldInfo->flags &= ~(1 << field);
  }
}

L7_uchar8 hapiBroadPolicyFieldFlagsGet(BROAD_FIELD_ENTRY_t *fieldInfo, BROAD_POLICY_FIELD_t field)
{
  L7_uchar8 value;

  if (field >= BROAD_FIELD_LAST)
      LOG_ERROR(field);

  if (field >= 32)
  {
    /* Need to add another 'flags' field to BROAD_FIELD_ENTRY_t */
    LOG_ERROR(field);
  }

  if (fieldInfo->flags & (1 << field))
  {           
    value = BROAD_FIELD_SPECIFIED;
  }
  else
  {
    value = BROAD_FIELD_NONE;
  }

  return value;
}

L7_uchar8 *hapiBroadPolicyFieldValuePtr(BROAD_FIELD_ENTRY_t *fieldInfo, BROAD_POLICY_FIELD_t field)
{
  L7_uchar8 *ptr = L7_NULL;

  if (field >= BROAD_FIELD_LAST)
      LOG_ERROR(field);

  switch (field)
  {
  case BROAD_FIELD_MACDA:
    ptr = fieldInfo->u.l2Ipv4.fieldMacDa.value;
    break;
  case BROAD_FIELD_MACSA:
    ptr = fieldInfo->u.l2Ipv4.fieldMacSa.value;
    break;
  case BROAD_FIELD_ETHTYPE:
    ptr = fieldInfo->fieldEthtype.value;
    break;
  case BROAD_FIELD_OVID:
    ptr = fieldInfo->fieldOvid.value;
    break;
  case BROAD_FIELD_IVID:
    ptr = fieldInfo->fieldIvid.value;
    break;
  case BROAD_FIELD_DSCP:
    ptr = fieldInfo->u.l2Ipv4.fieldDscp.value;
    break;
  case BROAD_FIELD_PROTO:
    ptr = fieldInfo->fieldProto.value;
    break;
  case BROAD_FIELD_SIP:
    ptr = fieldInfo->u.l2Ipv4.fieldSip.value;
    break;
  case BROAD_FIELD_DIP:
    ptr = fieldInfo->u.l2Ipv4.fieldDip.value;
    break;
  case BROAD_FIELD_SPORT:
    ptr = fieldInfo->fieldSport.value;
    break;
  case BROAD_FIELD_DPORT:
    ptr = fieldInfo->fieldDport.value;
    break;
  case BROAD_FIELD_IP6_HOPLIMIT:
    ptr = fieldInfo->fieldIp6Hoplimit.value;
    break;
  case BROAD_FIELD_IP6_NEXTHEADER:
    ptr = fieldInfo->fieldIp6Nextheader.value;
    break;
  case BROAD_FIELD_LOOKUP_STATUS:
    ptr = fieldInfo->fieldLookupStatus.value;
    break;
  case BROAD_FIELD_IP6_SRC:
    ptr = fieldInfo->u.Ipv6.fieldIp6Src.value;
    break;
  case BROAD_FIELD_IP6_DST:
    ptr = fieldInfo->u.Ipv6.fieldIp6Dst.value;
    break;
  case BROAD_FIELD_IP6_FLOWLABEL:
    ptr = fieldInfo->fieldIp6Flowlabel.value;
    break;
  case BROAD_FIELD_IP6_TRAFFIC_CLASS:
    ptr = fieldInfo->fieldIp6TrafficClass.value;
    break;
  case BROAD_FIELD_ICMP_MSG_TYPE:
    ptr = fieldInfo->fieldIcmpMsgType.value;
    break;
//case BROAD_FIELD_CLASS_ID:
//  ptr = fieldInfo->fieldClassId.value;
//  break;
  case BROAD_FIELD_L2_CLASS_ID:
    ptr = fieldInfo->fieldL2ClassId.value;
    break;
  case BROAD_FIELD_ISCSI_OPCODE:
  case BROAD_FIELD_ISCSI_OPCODE_TCP_OPTIONS:
    ptr = fieldInfo->fieldIscsiOpcode.value;
    break;
  case BROAD_FIELD_TCP_CONTROL:
    ptr = fieldInfo->fieldTCPControl.value;
    break;
  case BROAD_FIELD_VLAN_FORMAT:
    ptr = fieldInfo->fieldVlanFormat.value;
    break;
  case BROAD_FIELD_L2_FORMAT:
    ptr = fieldInfo->fieldL2Format.value;
    break;
  case BROAD_FIELD_SNAP:
    ptr = fieldInfo->fieldSnap.value;
    break;
  case BROAD_FIELD_IP_TYPE:
    ptr = fieldInfo->fieldIpType.value;
    break;
  // PTin added: FP
  case BROAD_FIELD_INPORTS:
    ptr = fieldInfo->fieldInports.value;
    break;
  case BROAD_FIELD_OUTPORT:
    ptr = fieldInfo->fieldOutport.value;
    break;
  case BROAD_FIELD_SRCTRUNK:
    ptr = fieldInfo->fieldSrcTrunk.value;
    break;
  case BROAD_FIELD_PORTCLASS:
    ptr = fieldInfo->fieldPortClass.value;
    break;
  case BROAD_FIELD_DROP:
    ptr = fieldInfo->fieldDrop.value;
    break;
//case BROAD_FIELD_L2_STATION_MOVE:
//  ptr = fieldInfo->fieldL2StationMove.value;
//  break;
  // PTin end
  default:
    LOG_ERROR(field);
    break;
  }

  return ptr;
}

L7_uchar8 *hapiBroadPolicyFieldMaskPtr(BROAD_FIELD_ENTRY_t *fieldInfo, BROAD_POLICY_FIELD_t field)
{
  L7_uchar8 *ptr = L7_NULL;

  if (field >= BROAD_FIELD_LAST)
      LOG_ERROR(field);

  switch (field)
  {
  case BROAD_FIELD_MACDA:
    ptr = fieldInfo->u.l2Ipv4.fieldMacDa.mask;
    break;
  case BROAD_FIELD_MACSA:
    ptr = fieldInfo->u.l2Ipv4.fieldMacSa.mask;
    break;
  case BROAD_FIELD_ETHTYPE:
    break;
  case BROAD_FIELD_OVID:
    ptr = fieldInfo->fieldOvid.mask;
    break;
  case BROAD_FIELD_IVID:
    ptr = fieldInfo->fieldIvid.mask;
    break;
  case BROAD_FIELD_DSCP:
    ptr = fieldInfo->u.l2Ipv4.fieldDscp.mask;
    break;
  case BROAD_FIELD_PROTO:
    ptr = fieldInfo->fieldProto.mask;
    break;
  case BROAD_FIELD_SIP:
    ptr = fieldInfo->u.l2Ipv4.fieldSip.mask;
    break;
  case BROAD_FIELD_DIP:
    ptr = fieldInfo->u.l2Ipv4.fieldDip.mask;
    break;
  case BROAD_FIELD_SPORT:
    ptr = fieldInfo->fieldSport.mask;
    break;
  case BROAD_FIELD_DPORT:
    ptr = fieldInfo->fieldDport.mask;
    break;
  case BROAD_FIELD_IP6_HOPLIMIT:
    break;
  case BROAD_FIELD_IP6_NEXTHEADER:
    ptr = fieldInfo->fieldIp6Nextheader.mask;
    break;
  case BROAD_FIELD_LOOKUP_STATUS:
    ptr = fieldInfo->fieldLookupStatus.mask;
    break;
  case BROAD_FIELD_IP6_SRC:
    ptr = fieldInfo->u.Ipv6.fieldIp6Src.mask;
    break;
  case BROAD_FIELD_IP6_DST:
    ptr = fieldInfo->u.Ipv6.fieldIp6Dst.mask;
    break;
  case BROAD_FIELD_IP6_FLOWLABEL:
    ptr = fieldInfo->fieldIp6Flowlabel.mask;
    break;
  case BROAD_FIELD_IP6_TRAFFIC_CLASS:
    ptr = fieldInfo->fieldIp6TrafficClass.mask;
    break;
  case BROAD_FIELD_ICMP_MSG_TYPE:
    ptr = fieldInfo->fieldIcmpMsgType.mask;
    break;
  case BROAD_FIELD_ISCSI_OPCODE:
  case BROAD_FIELD_ISCSI_OPCODE_TCP_OPTIONS:
    ptr = fieldInfo->fieldIscsiOpcode.mask;
    break;
  case BROAD_FIELD_TCP_CONTROL:
    ptr = fieldInfo->fieldTCPControl.mask;
    break;
  case BROAD_FIELD_SNAP:
    ptr = fieldInfo->fieldSnap.mask;
    break;
  // PTin added: FP
  case BROAD_FIELD_INPORTS:
    ptr = fieldInfo->fieldInports.mask;
    break;
  case BROAD_FIELD_OUTPORT:
    ptr = fieldInfo->fieldOutport.mask;
    break;
  case BROAD_FIELD_SRCTRUNK:
    ptr = fieldInfo->fieldSrcTrunk.mask;
    break;
  case BROAD_FIELD_PORTCLASS:
    ptr = fieldInfo->fieldPortClass.mask;
    break;
  // PTin end
  /* The following fields don't support masks */
  case BROAD_FIELD_VLAN_FORMAT:
  case BROAD_FIELD_L2_FORMAT:
  case BROAD_FIELD_IP_TYPE:
  //case BROAD_FIELD_CLASS_ID:
  case BROAD_FIELD_L2_CLASS_ID:
  case BROAD_FIELD_DROP:            /* PTin added: FP */
  //case BROAD_FIELD_L2_STATION_MOVE: /* PTin added: FP */
    break;
  default:
    LOG_ERROR(field);
    break;
  }

  return ptr;
}

void hapiBroadPolicyActionParmsGet(BROAD_ACTION_ENTRY_t       *actionPtr, 
                                   BROAD_POLICY_STAGE_t        policyStage,
                                   BROAD_POLICY_ACTION_t       action, 
                                   BROAD_POLICY_ACTION_SCOPE_t action_scope,
                                   L7_uint32                  *param0, 
                                   L7_uint32                  *param1, 
                                   L7_uint32                  *param2)
{
  *param0 = 0;
  *param1 = 0;
  *param2 = 0;

  switch (action)
  {
  case BROAD_ACTION_REDIRECT:
  case BROAD_ACTION_MIRROR:
    *param0 = actionPtr->u.ifp_parms.modid;
    *param1 = actionPtr->u.ifp_parms.modport;
    break;

  case BROAD_ACTION_SET_COSQ:
    *param0 = actionPtr->u.ifp_parms.set_cosq[action_scope];
    break;

  case BROAD_ACTION_COPY_TO_CPU:
    if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
    {
      *param0 = actionPtr->u.vfp_parms.cpu_cosq;
    }
    break;

  case BROAD_ACTION_TS_TO_CPU:
    if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
    {
      *param0 = actionPtr->u.vfp_parms.cpu_cosq;
    }
    break;

  case BROAD_ACTION_SET_TOS:
  case BROAD_ACTION_SET_DSCP:
    if (policyStage == BROAD_POLICY_STAGE_EGRESS)
    {
      *param0 = actionPtr->u.efp_parms.set_dscp[action_scope];
    }
    else
    {
      *param0 = actionPtr->u.ifp_parms.set_dscp[action_scope];
    }
    break;

  case BROAD_ACTION_SET_USERPRIO:
    if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
    {
      *param0 = actionPtr->u.vfp_parms.set_userprio[action_scope];
    }
    else if (policyStage == BROAD_POLICY_STAGE_EGRESS)
    {
      *param0 = actionPtr->u.efp_parms.set_userprio[action_scope];
    }
    else
    {
      *param0 = actionPtr->u.ifp_parms.set_userprio[action_scope];
    }
    break;

  case BROAD_ACTION_SET_DROPPREC:
    *param0 = actionPtr->u.ifp_parms.set_dropprec.conforming;
    *param1 = actionPtr->u.ifp_parms.set_dropprec.exceeding;
    *param2 = actionPtr->u.ifp_parms.set_dropprec.nonconforming;
    break;

  case BROAD_ACTION_SET_OUTER_VID:
    if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
    {
      *param0 = actionPtr->u.vfp_parms.set_ovid;
    }
    else
    {
      *param0 = actionPtr->u.efp_parms.set_ovid;
    }
    break;

  case BROAD_ACTION_SET_INNER_VID:
    *param0 = actionPtr->u.efp_parms.set_ivid;
    break;

  case BROAD_ACTION_ADD_OUTER_VID:
    *param0 = actionPtr->u.vfp_parms.add_ovid;
    break;

  case BROAD_ACTION_ADD_INNER_VID:
    *param0 = actionPtr->u.vfp_parms.add_ivid;
    break;

  case BROAD_ACTION_SET_CLASS_ID:
    if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
    {
      *param0 = actionPtr->u.vfp_parms.set_class_id;
    }
    else
    {
      *param0 = actionPtr->u.ifp_parms.set_class_id;
    }
    break;

  case BROAD_ACTION_SET_REASON_CODE:
    *param0 = actionPtr->u.ifp_parms.set_reason;
    break;

  default:
    break;
  }
}


/*********************************************************************
*
* @purpose Frees all the rule entries for a policy.
*
* @end
*
*********************************************************************/
void hapiBroadPolicyRulesPurge(BROAD_POLICY_ENTRY_t *policyInfo)
{
  BROAD_POLICY_RULE_ENTRY_t *rulePtr, *nextRulePtr;

  if (policyInfo == L7_NULL)
  {
    return;
  }

  rulePtr = policyInfo->ruleInfo;

  while (rulePtr != L7_NULL)
  {
    nextRulePtr = rulePtr->next;

    osapiFree(L7_DRIVER_COMPONENT_ID, rulePtr);

    rulePtr = nextRulePtr;
  }

  policyInfo->ruleInfo = L7_NULL;
}

/*********************************************************************
*
* @purpose Copies the source policy info to the destination policy
*          structure. Note that this function will allocate 
*          memory which can be freed later using hapiBroadPolicyRulesPurge().
*
* @returns  BCM_E_XXX
*
* @end
*
*********************************************************************/
int hapiBroadPolicyCopy(BROAD_POLICY_ENTRY_t *srcPolicyInfo, BROAD_POLICY_ENTRY_t *dstPolicyInfo)
{
  int                        rv = BCM_E_NONE;
  BROAD_POLICY_RULE_ENTRY_t *rulePtr, *lastRulePtr, *newRulePtr;

  dstPolicyInfo->policyType     = srcPolicyInfo->policyType;
  dstPolicyInfo->policyStage    = srcPolicyInfo->policyStage;
  dstPolicyInfo->policyFlags    = srcPolicyInfo->policyFlags;
  dstPolicyInfo->strictEthTypes = srcPolicyInfo->strictEthTypes;
  dstPolicyInfo->ruleCount      = srcPolicyInfo->ruleCount;
  /* Check for possible memory leak here. */
  if (dstPolicyInfo->ruleInfo != L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID, "Copying policy rules to non-empty list, possible memory leak.");
  }
  dstPolicyInfo->ruleInfo    = L7_NULL;

  /* Iterate through each rule and make a copy for the caller. 
     Note that this functions expects the caller to free the
     rule nodes when it's done using the policy info. */
  rulePtr = srcPolicyInfo->ruleInfo;
  lastRulePtr = L7_NULL;
  while (rulePtr != L7_NULL)
  {
    newRulePtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(*newRulePtr));
    if (newRulePtr == L7_NULL)
    {
      rv = BCM_E_MEMORY;
      break;
    }

    if (lastRulePtr == L7_NULL)
    {
      dstPolicyInfo->ruleInfo = newRulePtr;
    }
    else
    {
      lastRulePtr->next = newRulePtr;
    }
    lastRulePtr = newRulePtr;

    memcpy(newRulePtr, rulePtr, sizeof(*newRulePtr));
    newRulePtr->next = L7_NULL;

    rulePtr = rulePtr->next;
  }

  return rv;
}

char *hapiBroadPolicyActionName(BROAD_POLICY_ACTION_t action)
{
    if (action >= BROAD_ACTION_LAST)
        return "INVALID";

    return broadActionNameTable[action];
}

char *hapiBroadPolicyTypeName(BROAD_POLICY_TYPE_t type)
{
    if (type >= BROAD_POLICY_TYPE_LAST)
        return "INVALID";

    return broadTypeNameTable[type];
}

/* Debug */

static BROAD_POLICY_DEBUG_LEVEL_t debugOutput = /*POLICY_DEBUG_HIGH*/ POLICY_DEBUG_NONE;

void hapiBroadPolicyDebugEnable(BROAD_POLICY_DEBUG_LEVEL_t val)
{
    if (val > POLICY_DEBUG_HIGH) 
        val = POLICY_DEBUG_HIGH;

    usl_policy_debug(val == POLICY_DEBUG_NONE ? 0 : 1);
    rpc_policy_debug(val == POLICY_DEBUG_NONE ? 0 : 1);

    debugOutput = val;
}

void hapiBroadDebugPolicyEnable(BROAD_POLICY_DEBUG_LEVEL_t val)
{
    /* an alias for convenience */
    hapiBroadPolicyDebugEnable(val);
}

BROAD_POLICY_DEBUG_LEVEL_t hapiBroadPolicyDebugLevel()
{
    return debugOutput;
}
