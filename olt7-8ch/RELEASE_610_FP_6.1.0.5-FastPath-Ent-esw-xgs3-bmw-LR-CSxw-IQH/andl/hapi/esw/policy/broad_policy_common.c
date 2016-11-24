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
  BROAD_FIELD_CLASS_ID_SIZE,
  BROAD_FIELD_L2_CLASS_ID_SIZE,
  BROAD_FIELD_ISCSI_OPCODE_SIZE,
  BROAD_FIELD_ISCSI_OPCODE_TCP_OPTIONS_SIZE,
  BROAD_FIELD_TCP_CONTROL_SIZE, 
  BROAD_FIELD_VLAN_FORMAT_SIZE,
  BROAD_FIELD_IP_TYPE_SIZE,
  BROAD_FIELD_INPORTS_SIZE,         /* PTin added */
  BROAD_FIELD_DROP_SIZE,            /* PTin added */
  BROAD_FIELD_L2_STATION_MOVE_SIZE, /* PTin added */
  BROAD_FIELD_L2_SRC_HIT_SIZE       /* PTin added */
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
    "CLASS_ID",
    "L2_CLASS_ID",
    "ISCSI_OPCODE",
    "ISCSI_OPCODE_TCP_OPTIONS",
    "TCP_CONTROL",
    "VLAN_FORMAT",
    "IP_TYPE",
    "INPORTS",
    "DROP_PACKET",
    "L2_STATION_MOVE",
    "L2_SRC_HIT"
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
    "SETCLASS",
    "SETREASON",
    "COPYUSERPFROMINNERTAG",
    "SET_COSQ_AS_PKTPRIO",  /* PTin added */
    "REDIRECT_TRUNK"        /* PTin added */
};

static char* broadTypeNameTable[BROAD_POLICY_TYPE_LAST] =
{
    "PORT       ",
    "IPSG       ",
    "VLAN       ",
    "ISCSI      ",
    "SYSTEM_PORT",
    "SYSTEM     ",
    "COSQ       ",
    "DVLAN      ",
    "DOT1AD     ",
    "SYS_EXT    "
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
  case BROAD_FIELD_CLASS_ID:
    ptr = fieldInfo->fieldClassId.value;
    break;
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
  case BROAD_FIELD_IP_TYPE:
    ptr = fieldInfo->fieldIpType.value;
    break;
    // PTin added
  case BROAD_FIELD_INPORTS:
    ptr = fieldInfo->fieldInports.value;
    break;
  case BROAD_FIELD_DROP:
    ptr = fieldInfo->fieldDrop.value;
    break;
  case BROAD_FIELD_L2_STATION_MOVE:
    ptr = fieldInfo->fieldL2StationMove.value;
    break;
  case BROAD_FIELD_L2_SRC_HIT:
    ptr = fieldInfo->fieldL2SrcHit.value;
    break;
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
  // PTin added
  case BROAD_FIELD_INPORTS:
    ptr = fieldInfo->fieldInports.mask;
    break;
  // PTin end
  /* The following fields don't support masks */
  case BROAD_FIELD_VLAN_FORMAT:
  case BROAD_FIELD_IP_TYPE:
  case BROAD_FIELD_CLASS_ID:
  case BROAD_FIELD_L2_CLASS_ID:
  case BROAD_FIELD_DROP:            /* PTin added */
  case BROAD_FIELD_L2_STATION_MOVE: /* PTin added */
  case BROAD_FIELD_L2_SRC_HIT:      /* PTin added */
    break;
  default:
    LOG_ERROR(field);
    break;
  }

  return ptr;
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

static BROAD_POLICY_DEBUG_LEVEL_t debugOutput = POLICY_DEBUG_NONE;

void hapiBroadPolicyDebugEnable(BROAD_POLICY_DEBUG_LEVEL_t val)
{
    if (val > POLICY_DEBUG_HIGH) 
        val = POLICY_DEBUG_HIGH;

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
