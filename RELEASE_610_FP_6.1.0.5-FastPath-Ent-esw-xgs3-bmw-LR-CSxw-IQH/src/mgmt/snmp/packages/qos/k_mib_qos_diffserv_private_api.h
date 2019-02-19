/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_qos_diffserv_private_api.h
*
* @purpose  Provide interface to DiffServ Private MIB
*                  
* @component SNMP
*
* @comments 
*
* @create 07/12/2002
*
* @author cpverne
* @end
*
**********************************************************************/
#include <k_private_base.h>
#include "usmdb_mib_diffserv_private_api.h"
#include "acl_exports.h"
#include "diffserv_exports.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "osapi_support.h"

/************************** General Status *****************************/  
L7_RC_t          
snmpDiffServGenAdminModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbDiffServGenAdminModeGet (UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentDiffServGenStatusAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentDiffServGenStatusAdminMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t          
snmpDiffServGenAdminModeSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServGenStatusAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentDiffServGenStatusAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServGenAdminModeSet(UnitIndex, temp_val);
  }

  return rc;
}

/************************************Class Table *****************************/

L7_RC_t          
snmpDiffServClassTypeGet ( L7_uint32 UnitIndex, L7_uint32 classIndex, L7_int32 *val )
{
  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t temp_val;
  L7_RC_t rc;

  rc = usmDbDiffServClassTypeGet (UnitIndex, classIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL:
      *val = D_agentDiffServClassType_all;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ANY:
      *val = D_agentDiffServClassType_any;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL:
      *val = D_agentDiffServClassType_acl;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t          
snmpDiffServClassTypeSet ( L7_uint32 UnitIndex, L7_uint32 classIndex, L7_int32 val )
{
  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServClassType_all:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_TYPE_ALL_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL;
    }
    else
    {
      /* not supported */
      rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServClassType_any:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_TYPE_ANY_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ANY;
    }
    else
    {
      /* not supported */
      rc = L7_FAILURE;
    }
    break;


  case D_agentDiffServClassType_acl:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_TYPE_ACL_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL;
    }
    else
    {
      /* not supported */
      rc = L7_FAILURE;
    }
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServClassTypeSet(UnitIndex, classIndex, temp_val);
  }

  return rc;
}

L7_RC_t          
snmpDiffServClassAclTypeGet ( L7_uint32 UnitIndex, L7_uint32 classIndex, L7_int32 *val )
{
  L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_t temp_val;
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t l3Proto;

  rc = usmDbDiffServClassAclTypeGet (UnitIndex, classIndex, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_IP:
       if((rc = usmDbDiffServClassL3ProtoGet(UnitIndex, classIndex, &l3Proto)) == L7_SUCCESS)
       {
          if(l3Proto == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4)
          {
             *val = D_agentDiffServClassAclType_ip;
          }
          else if(l3Proto == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6)
          {
             *val = D_agentDiffServClassAclType_ipv6;
          }
          else
          {
             rc = L7_FAILURE;
          }
       }
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_MAC:
      *val = D_agentDiffServClassAclType_mac;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t          
snmpDiffServClassProtoTypeGet(L7_uint32 UnitIndex, L7_uint32 classIndex, L7_int32 *val)
{
  L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t temp_val;
  L7_RC_t rc;

  rc = usmDbDiffServClassL3ProtoGet(UnitIndex, classIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4:
      *val = D_agentDiffServClassProtoType_ipv4;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6:
      *val = D_agentDiffServClassProtoType_ipv6;
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_NONE:
    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t          
snmpDiffServClassAclTypeSet ( L7_uint32 UnitIndex, L7_uint32 classIndex, L7_int32 val )
{
  L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_t temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;
  L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t l3Proto=L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_NONE;

  switch (val)
  {
  case D_agentDiffServClassAclType_ip:
  case D_agentDiffServClassAclType_ipv6:
    if(usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID, 
                                L7_ACL_FEATURE_SUPPORTED) == L7_TRUE)
    {
      l3Proto = L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4;   
      temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_IP;
    }
    else if(usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID, 
                                     L7_ACL_TYPE_IPV6_FEATURE_ID) == L7_TRUE)
    {
       l3Proto = L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6;   
       temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_IP;
    }
    else
    {
      /* not supported */
      rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServClassAclType_mac:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID, 
                                 L7_ACL_TYPE_MAC_FEATURE_ID) == L7_TRUE)
    {
       l3Proto = L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4;   
       temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_MAC;
    }
    else
    {  /* not supported */
       rc = L7_FAILURE;
    }
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    if((rc = usmDbDiffServClassL3ProtoSet(USMDB_UNIT_CURRENT, classIndex, l3Proto)) == L7_SUCCESS)
    {
       rc = usmDbDiffServClassAclTypeSet(UnitIndex, classIndex, temp_val);
    } 
  }

  return rc;
}

L7_RC_t          
snmpDiffServClassProtoTypeSet(L7_uint32 UnitIndex, L7_uint32 classIndex, L7_int32 val)
{
  L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServClassProtoType_ipv4:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4;
    }
    else
    {
      /* not supported */
      rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServClassProtoType_ipv6:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6;
    }
    else
    {
      /* not supported */
      rc = L7_FAILURE;
    }
    break;


  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServClassL3ProtoSet(UnitIndex, classIndex, temp_val);
  }

  return rc;
}

L7_RC_t          
snmpDiffServClassStorageTypeGet ( L7_uint32 UnitIndex, L7_uint32 classIndex, L7_int32 *val )
{
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbDiffServClassStorageTypeGet (UnitIndex, classIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_agentDiffServClassStorageType_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_agentDiffServClassStorageType_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_agentDiffServClassStorageType_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_agentDiffServClassStorageType_permanent;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_agentDiffServClassStorageType_readOnly;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t          
snmpDiffServClassStorageTypeSet ( L7_uint32 UnitIndex, L7_uint32 classIndex, L7_int32 val )
{
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServClassStorageType_other:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER;
    break;

  case D_agentDiffServClassStorageType_volatile:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE;
    break;

  case D_agentDiffServClassStorageType_nonVolatile:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE;
    break;

  case D_agentDiffServClassStorageType_permanent:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT;
    break;

  case D_agentDiffServClassStorageType_readOnly:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServClassStorageTypeSet(UnitIndex, classIndex, temp_val);
  }

  return rc;
}

L7_RC_t          
snmpDiffServClassRowStatusGet ( L7_uint32 UnitIndex, L7_uint32 classIndex, L7_int32 *val )
{
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbDiffServClassRowStatusGet (UnitIndex, classIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_agentDiffServClassRowStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_agentDiffServClassRowStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_agentDiffServClassRowStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_agentDiffServClassRowStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_agentDiffServClassRowStatus_createAndWait;
      break;


    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_agentDiffServClassRowStatus_destroy;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t          
snmpDiffServClassRowStatusSet ( L7_uint32 UnitIndex, L7_uint32 classIndex, L7_int32 val )
{
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServClassRowStatus_active:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
    break;

  case D_agentDiffServClassRowStatus_notInService:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;
    break;

  case D_agentDiffServClassRowStatus_notReady:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY;
    break;

  case D_agentDiffServClassRowStatus_createAndGo:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO;
    break;

  case D_agentDiffServClassRowStatus_createAndWait:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT;
    break; 

  case D_agentDiffServClassRowStatus_destroy:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServClassRowStatusSet(UnitIndex, classIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDiffServClassGet ( agentDiffServClassEntry_t *agentDiffServClassEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len;

  if (usmDbDiffServClassGet(USMDB_UNIT_CURRENT, agentDiffServClassEntryData->agentDiffServClassIndex) == L7_SUCCESS)
  {
    CLR_VALID(nominator, agentDiffServClassEntryData->valid);

/*
 * if ( nominator != -1 ) condition is added to all the case statements 
 * for storing all the values to support the undo functionality. 
 */ 
    switch (nominator)
    {
    case -1:
    case I_agentDiffServClassIndex:
      rc = L7_SUCCESS;
      SET_VALID(I_agentDiffServClassIndex, agentDiffServClassEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassName:
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      snmp_buffer_len = SNMP_BUFFER_LEN;
      if (usmDbDiffServClassNameGet(USMDB_UNIT_CURRENT, 
                                    agentDiffServClassEntryData->agentDiffServClassIndex,
                                    snmp_buffer, &snmp_buffer_len) == L7_SUCCESS &&
          (SafeMakeOctetString(&agentDiffServClassEntryData->agentDiffServClassName, 
                               snmp_buffer, snmp_buffer_len-1) == L7_TRUE))
      {
        rc = L7_SUCCESS;
        SET_VALID(I_agentDiffServClassName, agentDiffServClassEntryData->valid);
      }

      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassType:
      rc = snmpDiffServClassTypeGet(USMDB_UNIT_CURRENT, 
                                    agentDiffServClassEntryData->agentDiffServClassIndex,
                                    &agentDiffServClassEntryData->agentDiffServClassType);
      if ( rc == L7_SUCCESS )
         SET_VALID(I_agentDiffServClassType, agentDiffServClassEntryData->valid);

      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassAclNum:
      rc = usmDbDiffServClassAclNumGet(USMDB_UNIT_CURRENT, 
                                       agentDiffServClassEntryData->agentDiffServClassIndex,
                                       &agentDiffServClassEntryData->agentDiffServClassAclNum);
      if ( rc == L7_SUCCESS )
         SET_VALID(I_agentDiffServClassAclNum, agentDiffServClassEntryData->valid);

      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleIndexNextFree:
      rc = usmDbDiffServClassRuleIndexNext(USMDB_UNIT_CURRENT, 
                                           agentDiffServClassEntryData->agentDiffServClassIndex,
                                           &agentDiffServClassEntryData->agentDiffServClassRuleIndexNextFree);
      if ( rc == L7_SUCCESS )
         SET_VALID(I_agentDiffServClassRuleIndexNextFree, agentDiffServClassEntryData->valid);

      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassStorageType:
      rc = snmpDiffServClassStorageTypeGet(USMDB_UNIT_CURRENT, 
                                           agentDiffServClassEntryData->agentDiffServClassIndex,
                                           &agentDiffServClassEntryData->agentDiffServClassStorageType);
      if ( rc == L7_SUCCESS )
         SET_VALID(I_agentDiffServClassStorageType, agentDiffServClassEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRowStatus:
      rc = snmpDiffServClassRowStatusGet(USMDB_UNIT_CURRENT, 
                                         agentDiffServClassEntryData->agentDiffServClassIndex,
                                         &agentDiffServClassEntryData->agentDiffServClassRowStatus);
      if ( rc == L7_SUCCESS )
         SET_VALID(I_agentDiffServClassRowStatus, agentDiffServClassEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassAclType:
      rc = snmpDiffServClassAclTypeGet(USMDB_UNIT_CURRENT, 
                                       agentDiffServClassEntryData->agentDiffServClassIndex,
                                       &agentDiffServClassEntryData->agentDiffServClassAclType);
      if ( rc == L7_SUCCESS )
         SET_VALID(I_agentDiffServClassAclType, agentDiffServClassEntryData->valid);
      break;

    case I_agentDiffServClassProtoType:
      rc = snmpDiffServClassProtoTypeGet(USMDB_UNIT_CURRENT, 
                                         agentDiffServClassEntryData->agentDiffServClassIndex,
                                         &agentDiffServClassEntryData->agentDiffServClassProtoType);
      if ( rc == L7_SUCCESS )
         SET_VALID(I_agentDiffServClassProtoType, agentDiffServClassEntryData->valid);
      break;

    default:
      rc = L7_FAILURE;
      break;
    }

    if (nominator >= 0 && rc == L7_SUCCESS)
      SET_VALID(nominator, agentDiffServClassEntryData->valid);
    else if (nominator == -1)
      rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpDiffServClassGetNext ( agentDiffServClassEntry_t *agentDiffServClassEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while (usmDbDiffServClassGetNext(USMDB_UNIT_CURRENT, 
                                   agentDiffServClassEntryData->agentDiffServClassIndex, 
                                   &agentDiffServClassEntryData->agentDiffServClassIndex) == L7_SUCCESS)
  {
    if (snmpDiffServClassGet(agentDiffServClassEntryData, nominator) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }
  }

  return rc;
}


/*********************************** ClassRule Table ***********************************/

L7_RC_t          
snmpDiffServClassRuleMatchEntryTypeGet ( L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                         L7_uint32 classRuleIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;
  rc = usmDbDiffServClassRuleMatchEntryTypeGet(UnitIndex, classIndex, classRuleIndex, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS:
      *val = D_agentDiffServClassRuleMatchEntryType_cos;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2:
      *val = D_agentDiffServClassRuleMatchEntryType_cos2;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
      *val = D_agentDiffServClassRuleMatchEntryType_dstip;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
      *val = D_agentDiffServClassRuleMatchEntryType_dstl4port;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
      *val = D_agentDiffServClassRuleMatchEntryType_dstmac;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:
      *val = D_agentDiffServClassRuleMatchEntryType_etype;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY:
      *val = D_agentDiffServClassRuleMatchEntryType_every;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
      *val = D_agentDiffServClassRuleMatchEntryType_ipdscp;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
      *val = D_agentDiffServClassRuleMatchEntryType_ipprecedence;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
      *val = D_agentDiffServClassRuleMatchEntryType_iptos;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
      *val = D_agentDiffServClassRuleMatchEntryType_protocol;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS:
      *val = D_agentDiffServClassRuleMatchEntryType_refclass;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
      *val = D_agentDiffServClassRuleMatchEntryType_srcip;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
      *val = D_agentDiffServClassRuleMatchEntryType_srcl4port;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
      *val = D_agentDiffServClassRuleMatchEntryType_srcmac;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID:
      *val = D_agentDiffServClassRuleMatchEntryType_vlanid;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2:
      *val = D_agentDiffServClassRuleMatchEntryType_vlanid2;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
      *val = D_agentDiffServClassRuleMatchEntryType_flowLabel;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
       *val = D_agentDiffServClassRuleMatchEntryType_dstPrefix;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
       *val = D_agentDiffServClassRuleMatchEntryType_srcPrefix;
      break;   

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }

  }
  else if(rc == L7_ERROR )
  {
     rc = L7_FAILURE; 
  }

  return rc;
}


L7_RC_t          
snmpDiffServClassRuleMatchEveryFlagGet ( L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                         L7_uint32 classRuleIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbDiffServClassRuleMatchEveryFlagGet (UnitIndex, classIndex, classRuleIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE:
      *val = D_agentDiffServClassRuleMatchEvery_true;
      break;

    case L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE:
      *val = D_agentDiffServClassRuleMatchEvery_false;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServClassRuleMatchIpTosBitsGet ( L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                           L7_uint32 classRuleIndex, L7_char8 *buf, L7_uint32 *buf_len)
{
  L7_RC_t rc;
  L7_uchar8 temp_char;

  rc = usmDbDiffServClassRuleMatchIpTosBitsGet(UnitIndex,classIndex,classRuleIndex, &temp_char);

  if (rc == L7_SUCCESS)
  {
    buf[0] = temp_char;
    *buf_len = 1;
  }

  return rc;
}

L7_RC_t
snmpDiffServClassRuleMatchIpTosMaskGet ( L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                           L7_uint32 classRuleIndex, L7_char8 *buf, L7_uint32 *buf_len)
{
  L7_RC_t rc;
  L7_uchar8 temp_char;

  rc = usmDbDiffServClassRuleMatchIpTosMaskGet(UnitIndex,classIndex,classRuleIndex, &temp_char);

  if (rc == L7_SUCCESS)
  {
    buf[0] = temp_char;
    *buf_len = 1;
  }

  return rc;
}

L7_RC_t          
snmpDiffServClassRuleMatchExcludeFlagGet ( L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                           L7_uint32 classRuleIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbDiffServClassRuleMatchExcludeFlagGet (UnitIndex, classIndex, classRuleIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE:
      if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                   L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) == L7_TRUE)
      {
        *val = D_agentDiffServClassRuleMatchExcludeFlag_true;
      }
      else
      {
        rc = L7_FAILURE;
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE:
      *val = D_agentDiffServClassRuleMatchExcludeFlag_false;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}




L7_RC_t          
snmpDiffServClassRuleStorageTypeGet ( L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                      L7_uint32 classRuleIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbDiffServClassRuleStorageTypeGet (UnitIndex, classIndex, classRuleIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_agentDiffServClassRuleStorageType_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_agentDiffServClassRuleStorageType_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_agentDiffServClassRuleStorageType_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_agentDiffServClassRuleStorageType_permanent;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_agentDiffServClassRuleStorageType_readOnly;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t        
snmpDiffServClassRuleRowStatusGet ( L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                    L7_uint32 classRuleIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbDiffServClassRuleRowStatusGet (UnitIndex, classIndex, classRuleIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_agentDiffServClassRuleRowStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_agentDiffServClassRuleRowStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_agentDiffServClassRuleRowStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_agentDiffServClassRuleRowStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_agentDiffServClassRuleRowStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_agentDiffServClassRuleRowStatus_destroy;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t          
snmpDiffServClassRuleMatchExcludeFlagSet ( L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                           L7_uint32 classRuleIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServClassRuleMatchExcludeFlag_true:
    temp_val = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE;
    break;

  case D_agentDiffServClassRuleMatchExcludeFlag_false:
    temp_val = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServClassRuleMatchExcludeFlagSet(UnitIndex, classIndex, classRuleIndex, temp_val);
  }

  return rc;
}


L7_RC_t          
snmpDiffServClassRuleMatchEntryTypeSet ( L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                         L7_uint32 classRuleIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;
  L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t l3Proto;

  if(usmDbDiffServClassL3ProtoGet(UnitIndex, classIndex, &l3Proto) == L7_SUCCESS)
  {
  switch (val)
  {
  case D_agentDiffServClassRuleMatchEntryType_cos:
    temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS;
    break;
    
  case D_agentDiffServClassRuleMatchEntryType_dstip:
    if(l3Proto == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4)
    {
       temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP;
    }
    else
    {
       rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServClassRuleMatchEntryType_dstl4port:
    temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT;
    break;

  case D_agentDiffServClassRuleMatchEntryType_dstmac:
    temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC;
    break;

  case D_agentDiffServClassRuleMatchEntryType_every:
    temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY;
    break;

  case D_agentDiffServClassRuleMatchEntryType_ipdscp:
    temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP;
    break;

  case D_agentDiffServClassRuleMatchEntryType_ipprecedence:
    if(l3Proto == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4)
    {
       temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE;
    }
    else
    {
       rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServClassRuleMatchEntryType_iptos:
     if(l3Proto == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4)
     {
        temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS;
     }
     else
     {
        rc = L7_FAILURE;
     }
    break;

  case D_agentDiffServClassRuleMatchEntryType_protocol:
    temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL;
    break;

  case D_agentDiffServClassRuleMatchEntryType_refclass:
    temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS;
    break;

  case D_agentDiffServClassRuleMatchEntryType_srcip:
    if(l3Proto == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4)
    {
       temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP;
    }
    else
    {
       rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServClassRuleMatchEntryType_srcl4port:
    temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT;
    break;

  case D_agentDiffServClassRuleMatchEntryType_srcmac:
    temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC;
    break;

  case D_agentDiffServClassRuleMatchEntryType_cos2:
    temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2;
    break;

  case D_agentDiffServClassRuleMatchEntryType_etype:
    temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE;
    break;

  case D_agentDiffServClassRuleMatchEntryType_vlanid:
    temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID;
    break;

  case D_agentDiffServClassRuleMatchEntryType_vlanid2:
    temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2;
    break;

  case D_agentDiffServClassRuleMatchEntryType_flowLabel:
     if(l3Proto == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6)
     {
        temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6;
     }
     else
     {
        rc = L7_FAILURE;
     }
    break;

  case D_agentDiffServClassRuleMatchEntryType_srcPrefix:
     if(l3Proto == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6)
     {
        temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6;
     }
     else
     {
        rc = L7_FAILURE;
     }
    break; 

  case D_agentDiffServClassRuleMatchEntryType_dstPrefix:
     if(l3Proto == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6)
     {
        temp_val = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6;
     }
     else
     {
        rc = L7_FAILURE;
     }
    break; 

  default:
    rc = L7_FAILURE;
    break;
  }
  } /* Get type L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_t *pAclType */
  else
  {
     rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServClassRuleMatchEntryTypeSet(UnitIndex, classIndex, classRuleIndex, temp_val);
  }

  return rc;
}


L7_RC_t
snmpDiffServClassRuleMatchIpTosBitsSet(L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                       L7_uint32 classRuleIndex, L7_char8 *buf, L7_uint32 buf_len)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 temp_char = 0;

  if (buf_len == 1)
  {
    rc = L7_SUCCESS;
    temp_char = buf[0];
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServClassRuleMatchIpTosBitsSet(UnitIndex, classIndex, classRuleIndex, temp_char);
  }

  return rc;
}


L7_RC_t
snmpDiffServClassRuleMatchIpTosMaskSet ( L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                           L7_uint32 classRuleIndex, L7_char8 *buf, L7_uint32 buf_len)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 temp_char = 0;

  if (buf_len == 1)
  {
    rc = L7_SUCCESS;
    temp_char = buf[0];
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServClassRuleMatchIpTosMaskSet(UnitIndex, classIndex, classRuleIndex, temp_char);
  }

  return rc;
}


L7_RC_t          
snmpDiffServClassRuleStorageTypeSet ( L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                      L7_uint32 classRuleIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServClassRuleStorageType_other:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER;
    break;

  case D_agentDiffServClassRuleStorageType_volatile:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE;
    break;

  case D_agentDiffServClassRuleStorageType_nonVolatile:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE;
    break;

  case D_agentDiffServClassRuleStorageType_permanent:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT;
    break;

  case D_agentDiffServClassRuleStorageType_readOnly:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServClassRuleStorageTypeSet(UnitIndex, classIndex, classRuleIndex, temp_val);
  }

  return rc;
}

L7_RC_t          
snmpDiffServClassRuleRowStatusSet ( L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                    L7_uint32 classRuleIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServClassRuleRowStatus_active:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
    break;

  case D_agentDiffServClassRuleRowStatus_notInService:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;
    break;

  case D_agentDiffServClassRuleRowStatus_notReady:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY;
    break;

  case D_agentDiffServClassRuleRowStatus_createAndGo:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO;
    break;

  case D_agentDiffServClassRuleRowStatus_createAndWait:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT;
    break; 

  case D_agentDiffServClassRuleRowStatus_destroy:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }
  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServClassRuleRowStatusSet(UnitIndex, classIndex, classRuleIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentDiffServClassRuleMatchEtypeKeyGet ( L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                             L7_uint32 classRuleIndex, L7_int32 *val )
{
  L7_RC_t rc;
  L7_QOS_ETYPE_KEYID_t temp_val;

  rc = usmDbDiffServClassRuleMatchEtypeKeyGet(UnitIndex, classIndex, classRuleIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_QOS_ETYPE_KEYID_CUSTOM:
      *val = D_agentDiffServClassRuleMatchEtypeKey_custom;
      break;

    case L7_QOS_ETYPE_KEYID_APPLETALK:
      *val = D_agentDiffServClassRuleMatchEtypeKey_appletalk;
      break;

    case L7_QOS_ETYPE_KEYID_ARP:
      *val = D_agentDiffServClassRuleMatchEtypeKey_arp;
      break;

    case L7_QOS_ETYPE_KEYID_IBMSNA:
      *val = D_agentDiffServClassRuleMatchEtypeKey_ibmsna;
      break;

    case L7_QOS_ETYPE_KEYID_IPV4:
      *val = D_agentDiffServClassRuleMatchEtypeKey_ipv4;
      break;

    case L7_QOS_ETYPE_KEYID_IPV6:
      *val = D_agentDiffServClassRuleMatchEtypeKey_ipv6;
      break;

    case L7_QOS_ETYPE_KEYID_IPX:
      *val = D_agentDiffServClassRuleMatchEtypeKey_ipx;
      break;

    case L7_QOS_ETYPE_KEYID_MPLSMCAST:
      *val = D_agentDiffServClassRuleMatchEtypeKey_mplsmcast;
      break;

    case L7_QOS_ETYPE_KEYID_MPLSUCAST:
      *val = D_agentDiffServClassRuleMatchEtypeKey_mplsucast;
      break;

    case L7_QOS_ETYPE_KEYID_NETBIOS:
      *val = D_agentDiffServClassRuleMatchEtypeKey_netbios;
      break;

    case L7_QOS_ETYPE_KEYID_NOVELL:
      *val = D_agentDiffServClassRuleMatchEtypeKey_novell;
      break;

    case L7_QOS_ETYPE_KEYID_PPPOE:
      *val = D_agentDiffServClassRuleMatchEtypeKey_pppoe;
      break;

    case L7_QOS_ETYPE_KEYID_RARP:
      *val = D_agentDiffServClassRuleMatchEtypeKey_rarp;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentDiffServClassRuleMatchEtypeKeySet ( L7_uint32 UnitIndex, L7_uint32 classIndex, 
                                             L7_uint32 classRuleIndex, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_QOS_ETYPE_KEYID_t temp_val = 0;

  switch (val)
  {
  case D_agentDiffServClassRuleMatchEtypeKey_custom:
    temp_val = L7_QOS_ETYPE_KEYID_CUSTOM;
    break;

  case D_agentDiffServClassRuleMatchEtypeKey_appletalk:
    temp_val = L7_QOS_ETYPE_KEYID_APPLETALK;
    break;

  case D_agentDiffServClassRuleMatchEtypeKey_arp:
    temp_val = L7_QOS_ETYPE_KEYID_ARP;
    break;

  case D_agentDiffServClassRuleMatchEtypeKey_ibmsna:
    temp_val = L7_QOS_ETYPE_KEYID_IBMSNA;
    break;

  case D_agentDiffServClassRuleMatchEtypeKey_ipv4:
    temp_val = L7_QOS_ETYPE_KEYID_IPV4;
    break;

  case D_agentDiffServClassRuleMatchEtypeKey_ipv6:
    temp_val = L7_QOS_ETYPE_KEYID_IPV6;
    break;

  case D_agentDiffServClassRuleMatchEtypeKey_ipx:
    temp_val = L7_QOS_ETYPE_KEYID_IPX;
    break;

  case D_agentDiffServClassRuleMatchEtypeKey_mplsmcast:
    temp_val = L7_QOS_ETYPE_KEYID_MPLSMCAST;
    break;

  case D_agentDiffServClassRuleMatchEtypeKey_mplsucast:
    temp_val = L7_QOS_ETYPE_KEYID_MPLSUCAST;
    break;

  case D_agentDiffServClassRuleMatchEtypeKey_netbios:
    temp_val = L7_QOS_ETYPE_KEYID_NETBIOS;
    break;

  case D_agentDiffServClassRuleMatchEtypeKey_novell:
    temp_val = L7_QOS_ETYPE_KEYID_NOVELL;
    break;

  case D_agentDiffServClassRuleMatchEtypeKey_pppoe:
    temp_val = L7_QOS_ETYPE_KEYID_PPPOE;
    break;

  case D_agentDiffServClassRuleMatchEtypeKey_rarp:
    temp_val = L7_QOS_ETYPE_KEYID_RARP;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServClassRuleMatchEtypeKeySet(UnitIndex, classIndex, classRuleIndex, temp_val);
  }

  return rc;
}

L7_RC_t snmpDiffServClassRuleMatchDstIpv6PrefixGet(L7_uint32 UnitIndex,
                                                   L7_uint32 classIndex,
                                                   L7_uint32 classRuleIndex,
                                                   L7_char8  *snmp_buffer, 
                                                   L7_uint32 *prefix_len, 
                                                   L7_uint32 *prefixLen)
{
   L7_in6_addr_t prefix;
   
   memset(&prefix, 0x00, sizeof(prefix));
   memset(snmp_buffer, 0x00, sizeof(snmp_buffer));
   if((usmDbDiffServClassRuleMatchDstIpv6AddrGet(UnitIndex, classIndex, classRuleIndex, &prefix) == L7_SUCCESS) &&
      (usmDbDiffServClassRuleMatchDstIpv6PrefLenGet(UnitIndex, classIndex, classRuleIndex, prefixLen) == L7_SUCCESS))
   {
      memcpy(snmp_buffer, prefix.in6.addr8, sizeof(prefix.in6.addr8)); 
      *prefix_len = sizeof(prefix.in6.addr8);
      return L7_SUCCESS;
   }
   return L7_FAILURE;
}

L7_RC_t snmpDiffServClassRuleMatchSrcIpv6PrefixGet(L7_uint32 UnitIndex,
                                                   L7_uint32 classIndex,
                                                   L7_uint32 classRuleIndex,
                                                   L7_char8  *snmp_buffer, 
                                                   L7_uint32 *prefix_len, 
                                                   L7_uint32 *prefixLen)
{
   L7_in6_addr_t prefix6;

   memset(&prefix6, 0x00, sizeof(prefix6));
   memset(snmp_buffer, 0x00, sizeof(snmp_buffer));
   if((usmDbDiffServClassRuleMatchSrcIpv6AddrGet(UnitIndex, classIndex, classRuleIndex, &prefix6) == L7_SUCCESS) &&
      (usmDbDiffServClassRuleMatchSrcIpv6PrefLenGet(UnitIndex, classIndex, classRuleIndex, prefixLen) == L7_SUCCESS))
   {
      memcpy(snmp_buffer, prefix6.in6.addr8, sizeof(prefix6.in6.addr8)); 
      *prefix_len = sizeof(prefix6.in6.addr8);
      return L7_SUCCESS;
   }
   return L7_FAILURE;
}

L7_RC_t
snmpDiffServClassRuleGet(  agentDiffServClassRuleEntry_t *agentDiffServClassRuleEntryData, L7_int32 nominator )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len, prefixLen;

  if (usmDbDiffServClassRuleGet(USMDB_UNIT_CURRENT, 
                                agentDiffServClassRuleEntryData->agentDiffServClassIndex, 
                                agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex) == L7_SUCCESS)
  {
    CLR_VALID(nominator, agentDiffServClassRuleEntryData->valid);

/*
 * if ( nominator != -1 ) condition is added to all the case statements 
 * for storing all the values to support the undo functionality. 
 */ 
    switch (nominator)
    {
    
    case -1:
    case I_agentDiffServClassRuleEntryIndex_agentDiffServClassIndex:
    case I_agentDiffServClassRuleIndex:
      rc = L7_SUCCESS;
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchEntryType :
      rc = L7_FAILURE;
      rc = snmpDiffServClassRuleMatchEntryTypeGet(USMDB_UNIT_CURRENT, 
                                                  agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                  agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                  &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchEntryType);
      if(rc == L7_SUCCESS)
      {
         SET_VALID(I_agentDiffServClassRuleMatchEntryType, agentDiffServClassRuleEntryData->valid);
      }
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchCos :
      rc = usmDbDiffServClassRuleMatchCosGet(USMDB_UNIT_CURRENT, 
                                             agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                             agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                             &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchCos);
      if ( rc == L7_SUCCESS )
      {
         SET_VALID(I_agentDiffServClassRuleMatchCos, agentDiffServClassRuleEntryData->valid);
      }
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchDstIpAddr :
      rc = usmDbDiffServClassRuleMatchDstIpAddrGet(USMDB_UNIT_CURRENT, 
                                                   agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                   agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                   &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchDstIpAddr);
      if ( rc == L7_SUCCESS )
      {
         SET_VALID(I_agentDiffServClassRuleMatchDstIpAddr, agentDiffServClassRuleEntryData->valid);
      }
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchDstIpMask :
      rc = usmDbDiffServClassRuleMatchDstIpMaskGet(USMDB_UNIT_CURRENT, 
                                                   agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                   agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                   &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchDstIpMask);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServClassRuleMatchDstIpMask, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchDstL4PortStart :
      rc = usmDbDiffServClassRuleMatchDstL4PortStartGet(USMDB_UNIT_CURRENT, 
                                                        agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                        agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                        &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchDstL4PortStart);
      if ( rc == L7_SUCCESS )
      {
         SET_VALID(I_agentDiffServClassRuleMatchDstL4PortStart, agentDiffServClassRuleEntryData->valid);
      }
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchDstL4PortEnd :
      rc = usmDbDiffServClassRuleMatchDstL4PortEndGet(USMDB_UNIT_CURRENT, 
                                                      agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                      agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                      &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchDstL4PortEnd);
      if ( rc == L7_SUCCESS )
      {
         SET_VALID(I_agentDiffServClassRuleMatchDstL4PortEnd, agentDiffServClassRuleEntryData->valid);
      }
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchDstMacAddr :
      memset(snmp_buffer, 0x00, sizeof(snmp_buffer));
      snmp_buffer_len = sizeof(snmp_buffer);
      if ((usmDbDiffServClassRuleMatchDstMacAddrGet(USMDB_UNIT_CURRENT, 
                                                    agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                    agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                    snmp_buffer, &snmp_buffer_len) == L7_SUCCESS) &&
          (SafeMakeOctetString(&agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchDstMacAddr, 
                               snmp_buffer, snmp_buffer_len) == L7_TRUE))
      {
        rc = L7_SUCCESS;
        SET_VALID(I_agentDiffServClassRuleMatchDstMacAddr, agentDiffServClassRuleEntryData->valid);
      }
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchDstMacMask :
      memset(snmp_buffer, 0x00, sizeof(snmp_buffer));
      snmp_buffer_len = sizeof(snmp_buffer);
      if ((usmDbDiffServClassRuleMatchDstMacMaskGet(USMDB_UNIT_CURRENT, 
                                                    agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                    agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                    snmp_buffer, &snmp_buffer_len) == L7_SUCCESS) &&
          (SafeMakeOctetString(&agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchDstMacMask, 
                               snmp_buffer, snmp_buffer_len) == L7_TRUE))
      {
        rc = L7_SUCCESS;
        SET_VALID(I_agentDiffServClassRuleMatchDstMacMask, agentDiffServClassRuleEntryData->valid);
      }
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchEvery :
      rc = snmpDiffServClassRuleMatchEveryFlagGet(USMDB_UNIT_CURRENT, 
                                                  agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                  agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                  &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchEvery);
      if ( rc == L7_SUCCESS )
      {
         SET_VALID(I_agentDiffServClassRuleMatchEvery, agentDiffServClassRuleEntryData->valid);
      }
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchIpDscp :
      rc = usmDbDiffServClassRuleMatchIpDscpGet(USMDB_UNIT_CURRENT, 
                                                agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchIpDscp);
      if ( rc == L7_SUCCESS )
      {
         SET_VALID(I_agentDiffServClassRuleMatchIpDscp, agentDiffServClassRuleEntryData->valid);
      }
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchIpPrecedence :
      rc = usmDbDiffServClassRuleMatchIpPrecedenceGet(USMDB_UNIT_CURRENT, 
                                                      agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                      agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                      &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchIpPrecedence);
      if ( rc == L7_SUCCESS )
      {
         SET_VALID(I_agentDiffServClassRuleMatchIpPrecedence, agentDiffServClassRuleEntryData->valid);
      }
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchIpTosBits:
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      snmp_buffer_len = SNMP_BUFFER_LEN;

      if ((snmpDiffServClassRuleMatchIpTosBitsGet(USMDB_UNIT_CURRENT, 
                                                  agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                  agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                  snmp_buffer, &snmp_buffer_len) == L7_SUCCESS) &&
          (SafeMakeOctetString(&agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchIpTosBits, 
                               snmp_buffer, snmp_buffer_len) == L7_TRUE))
      {
        rc = L7_SUCCESS;
        SET_VALID(I_agentDiffServClassRuleMatchIpTosBits, agentDiffServClassRuleEntryData->valid);
      }
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchIpTosMask :
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      snmp_buffer_len = SNMP_BUFFER_LEN;
      if ((snmpDiffServClassRuleMatchIpTosMaskGet(USMDB_UNIT_CURRENT, 
                                                  agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                  agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                  snmp_buffer, &snmp_buffer_len) == L7_SUCCESS) &&
          (SafeMakeOctetString(&agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchIpTosMask, 
                               snmp_buffer, snmp_buffer_len) == L7_TRUE))
      {
        rc = L7_SUCCESS;
        SET_VALID(I_agentDiffServClassRuleMatchIpTosMask, agentDiffServClassRuleEntryData->valid);
      }
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchProtocolNum :
      rc = usmDbDiffServClassRuleMatchProtocolNumGet(USMDB_UNIT_CURRENT, 
                                                     agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                     agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                     &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchProtocolNum);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServClassRuleMatchProtocolNum, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchRefClassIndex :
      rc = usmDbDiffServClassRuleMatchRefClassIndexGet(USMDB_UNIT_CURRENT, 
                                                       agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                       agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                       &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchRefClassIndex);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServClassRuleMatchRefClassIndex, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchSrcIpAddr :
      rc = usmDbDiffServClassRuleMatchSrcIpAddrGet(USMDB_UNIT_CURRENT, 
                                                   agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                   agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                   &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchSrcIpAddr);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServClassRuleMatchSrcIpAddr, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchSrcIpMask :
      rc = usmDbDiffServClassRuleMatchSrcIpMaskGet(USMDB_UNIT_CURRENT, 
                                                   agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                   agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                   &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchSrcIpMask);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServClassRuleMatchSrcIpMask, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchSrcL4PortStart :
      rc = usmDbDiffServClassRuleMatchSrcL4PortStartGet(USMDB_UNIT_CURRENT, 
                                                        agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                        agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                        &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchSrcL4PortStart);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServClassRuleMatchSrcL4PortStart, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchSrcL4PortEnd :
      rc = usmDbDiffServClassRuleMatchSrcL4PortEndGet(USMDB_UNIT_CURRENT, 
                                                      agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                      agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                      &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchSrcL4PortEnd);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServClassRuleMatchSrcL4PortEnd, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchSrcMacAddr :
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      snmp_buffer_len = SNMP_BUFFER_LEN;
      if ((usmDbDiffServClassRuleMatchSrcMacAddrGet(USMDB_UNIT_CURRENT, 
                                                    agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                    agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                    snmp_buffer, &snmp_buffer_len) == L7_SUCCESS) &&
          (SafeMakeOctetString(&agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchSrcMacAddr, 
                               snmp_buffer, snmp_buffer_len) == L7_TRUE))
      {
        rc = L7_SUCCESS;
        SET_VALID(I_agentDiffServClassRuleMatchSrcMacAddr, agentDiffServClassRuleEntryData->valid);
      }
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchSrcMacMask :
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      snmp_buffer_len = SNMP_BUFFER_LEN;
      if ((usmDbDiffServClassRuleMatchSrcMacMaskGet(USMDB_UNIT_CURRENT, 
                                                    agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                    agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                    snmp_buffer, &snmp_buffer_len) == L7_SUCCESS) &&
          (SafeMakeOctetString(&agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchSrcMacMask, 
                               snmp_buffer, snmp_buffer_len) == L7_TRUE))
      {
        rc = L7_SUCCESS;
        SET_VALID(I_agentDiffServClassRuleMatchSrcMacMask, agentDiffServClassRuleEntryData->valid);
      }
      if ( nominator != -1 ) break;
      /* else pass through */

#ifdef OBSOLETE
    case I_agentDiffServClassRuleMatchVlanId :
      rc = usmDbDiffServClassRuleMatchVlanIdGet(USMDB_UNIT_CURRENT, 
                                                agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchVlanId);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServClassRuleMatchVlanId, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */
#endif

    case I_agentDiffServClassRuleMatchExcludeFlag :
      rc = snmpDiffServClassRuleMatchExcludeFlagGet(USMDB_UNIT_CURRENT, 
                                                    agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                    agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                    &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchExcludeFlag);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServClassRuleMatchExcludeFlag, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleStorageType :
      rc = snmpDiffServClassRuleStorageTypeGet(USMDB_UNIT_CURRENT, 
                                               agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                               agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                               &agentDiffServClassRuleEntryData->agentDiffServClassRuleStorageType);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServClassRuleStorageType, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleRowStatus :
      rc = snmpDiffServClassRuleRowStatusGet(USMDB_UNIT_CURRENT, 
                                             agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                             agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                             &agentDiffServClassRuleEntryData->agentDiffServClassRuleRowStatus);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServClassRuleRowStatus, agentDiffServClassRuleEntryData->valid);

      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchCos2 :
      rc = usmDbDiffServClassRuleMatchCos2Get(USMDB_UNIT_CURRENT,
                                              agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                              agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                              &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchCos2);
      if (rc == L7_SUCCESS)
        SET_VALID(I_agentDiffServClassRuleMatchCos2, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchEtypeKey :
      rc = snmpAgentDiffServClassRuleMatchEtypeKeyGet(USMDB_UNIT_CURRENT,
                                                      agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                      agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                      &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchEtypeKey);
      if (rc == L7_SUCCESS)
        SET_VALID(I_agentDiffServClassRuleMatchEtypeKey, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchEtypeValue :
      rc = usmDbDiffServClassRuleMatchEtypeValueGet(USMDB_UNIT_CURRENT,
                                                    agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                    agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                    &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchEtypeValue);
      if (rc == L7_SUCCESS)
        SET_VALID(I_agentDiffServClassRuleMatchEtypeValue, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchVlanIdStart :
      rc = usmDbDiffServClassRuleMatchVlanIdStartGet(USMDB_UNIT_CURRENT,
                                                     agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                     agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                     &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchVlanIdStart);
      if (rc == L7_SUCCESS)
        SET_VALID(I_agentDiffServClassRuleMatchVlanIdStart, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchVlanIdEnd :
      rc = usmDbDiffServClassRuleMatchVlanIdEndGet(USMDB_UNIT_CURRENT,
                                                   agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                   agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                   &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchVlanIdEnd);
      if (rc == L7_SUCCESS)
        SET_VALID(I_agentDiffServClassRuleMatchVlanIdEnd, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchVlanId2Start :
      rc = usmDbDiffServClassRuleMatchVlanId2StartGet(USMDB_UNIT_CURRENT,
                                                      agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                      agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                      &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchVlanId2Start);
      if (rc == L7_SUCCESS)
        SET_VALID(I_agentDiffServClassRuleMatchVlanId2Start, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchVlanId2End :
      rc = usmDbDiffServClassRuleMatchVlanId2EndGet(USMDB_UNIT_CURRENT,
                                                    agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                    agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                    &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchVlanId2End);
      if (rc == L7_SUCCESS)
        SET_VALID(I_agentDiffServClassRuleMatchVlanId2End, agentDiffServClassRuleEntryData->valid);
      break;

    case I_agentDiffServClassRuleMatchFlowLabel:
      rc = usmDbDiffServClassRuleMatchIp6FlowLabelGet(USMDB_UNIT_CURRENT, 
                                                      agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                      agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                      &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchFlowLabel);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServClassRuleMatchFlowLabel, agentDiffServClassRuleEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServClassRuleMatchDstIpv6Prefix:
       memset(snmp_buffer, 0x00, sizeof(snmp_buffer));
        if((rc = snmpDiffServClassRuleMatchDstIpv6PrefixGet(USMDB_UNIT_CURRENT, 
                                                            agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                            agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                            snmp_buffer, &snmp_buffer_len, &prefixLen) == L7_SUCCESS)
             && (SafeMakeOctetString(&agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchDstIpv6Prefix, 
                                     snmp_buffer, snmp_buffer_len) == L7_TRUE))
        {
           rc = L7_SUCCESS;
           SET_VALID(I_agentDiffServClassRuleMatchDstIpv6Prefix, agentDiffServClassRuleEntryData->valid);
        }
        else
        {
           rc = L7_FAILURE;
        }
        if (nominator != -1 ) break;
        /* else pass through */

    case I_agentDiffServClassRuleMatchDstIpv6PrefixLength:
       memset(snmp_buffer, 0x00, sizeof(snmp_buffer));
          if(((rc = snmpDiffServClassRuleMatchDstIpv6PrefixGet(USMDB_UNIT_CURRENT, 
                                                               agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                               agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                               snmp_buffer, &snmp_buffer_len, 
                                                               &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchDstIpv6PrefixLength)) == L7_SUCCESS)
             &&(agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchDstIpv6PrefixLength != 0))
          {
             rc = L7_SUCCESS;
             SET_VALID(I_agentDiffServClassRuleMatchDstIpv6PrefixLength, agentDiffServClassRuleEntryData->valid);
          }
          else
          {
             rc = L7_FAILURE;
          }
          if (nominator != -1 ) break;
          /* else pass through */

    case I_agentDiffServClassRuleMatchSrcIpv6Prefix:
       memset(snmp_buffer, 0x00, sizeof(snmp_buffer));
        if((rc = snmpDiffServClassRuleMatchSrcIpv6PrefixGet(USMDB_UNIT_CURRENT, 
                                                            agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                            agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                            snmp_buffer, &snmp_buffer_len, &prefixLen) == L7_SUCCESS)
             && (SafeMakeOctetString(&agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchSrcIpv6Prefix, 
                                     snmp_buffer, snmp_buffer_len) == L7_TRUE))
        {
           rc = L7_SUCCESS;
           SET_VALID(I_agentDiffServClassRuleMatchSrcIpv6Prefix, agentDiffServClassRuleEntryData->valid);
        }
        else
        {
           rc = L7_FAILURE;
        }
        if (nominator != -1 ) break;

    case I_agentDiffServClassRuleMatchSrcIpv6PrefixLength:
       memset(snmp_buffer, 0x00, sizeof(snmp_buffer));
          if(((rc = snmpDiffServClassRuleMatchSrcIpv6PrefixGet(USMDB_UNIT_CURRENT, 
                                                              agentDiffServClassRuleEntryData->agentDiffServClassIndex,
                                                              agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex,
                                                              snmp_buffer, &snmp_buffer_len, 
                                                              &agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchSrcIpv6PrefixLength)) == L7_SUCCESS)
             &&(agentDiffServClassRuleEntryData->agentDiffServClassRuleMatchSrcIpv6PrefixLength != 0))
          {
             rc = L7_SUCCESS;
             SET_VALID(I_agentDiffServClassRuleMatchSrcIpv6PrefixLength, agentDiffServClassRuleEntryData->valid);
          }
          else
          {
             rc = L7_FAILURE;
          }
          if (nominator != -1 ) break;
 
    default:
      rc = L7_FAILURE;
      break;
    }

    if (nominator > 0 && rc == L7_SUCCESS)
      SET_VALID(nominator, agentDiffServClassRuleEntryData->valid);
    else if (nominator == -1)
      rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpDiffServClassRuleGetNext( agentDiffServClassRuleEntry_t *agentDiffServClassRuleEntryData, L7_int32 nominator )
{
  L7_RC_t rc = L7_FAILURE;

  while (usmDbDiffServClassRuleGetNext(USMDB_UNIT_CURRENT, 
                                       agentDiffServClassRuleEntryData->agentDiffServClassIndex, 
                                       agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex, 
                                       &agentDiffServClassRuleEntryData->agentDiffServClassIndex, 
                                       &agentDiffServClassRuleEntryData->agentDiffServClassRuleIndex) == L7_SUCCESS)
  {
    if (snmpDiffServClassRuleGet(agentDiffServClassRuleEntryData, nominator) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpDiffServPolicyTypeGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *val)
{
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t temp_val;
  L7_RC_t rc;

  rc = usmDbDiffServPolicyTypeGet(UnitIndex, policyIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
      *val = D_agentDiffServPolicyType_in;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
      *val = D_agentDiffServPolicyType_out;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyStorageTypeGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_int32 *val)
{
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;
  L7_RC_t rc;

  rc = usmDbDiffServPolicyStorageTypeGet(UnitIndex, policyIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_agentDiffServPolicyStorageType_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_agentDiffServPolicyStorageType_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_agentDiffServPolicyStorageType_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_agentDiffServPolicyStorageType_readOnly;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_agentDiffServPolicyStorageType_permanent;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyRowStatusGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_int32 *val)
{
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;
  L7_RC_t rc;

  rc = usmDbDiffServPolicyRowStatusGet (UnitIndex, policyIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_agentDiffServPolicyRowStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_agentDiffServPolicyRowStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_agentDiffServPolicyRowStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_agentDiffServPolicyRowStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_agentDiffServPolicyRowStatus_createAndWait;
      break;


    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_agentDiffServPolicyRowStatus_destroy;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyTypeSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_int32 val)
{
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServPolicyType_in:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN;
    break;

  case D_agentDiffServPolicyType_out:
     if (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
             L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID) == L7_TRUE ||
                    usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                            L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID) == L7_TRUE )
       temp_val = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT;
    
     break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServPolicyTypeSet(UnitIndex, policyIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyStorageTypeSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_int32 val)
{
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServPolicyStorageType_other:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER;
    break;

  case D_agentDiffServPolicyStorageType_volatile:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE;
    break;

  case D_agentDiffServPolicyStorageType_nonVolatile:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE;
    break;

  case D_agentDiffServPolicyStorageType_permanent:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT;
    break;

  case D_agentDiffServPolicyStorageType_readOnly:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServPolicyStorageTypeSet(UnitIndex, policyIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyRowStatusSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_int32 val)
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServPolicyRowStatus_active:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
    break;

  case D_agentDiffServPolicyRowStatus_notInService:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;
    break;

  case D_agentDiffServPolicyRowStatus_notReady:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY;
    break;

  case D_agentDiffServPolicyRowStatus_createAndGo:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO;
    break;

  case D_agentDiffServPolicyRowStatus_createAndWait:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT;
    break; 

  case D_agentDiffServPolicyRowStatus_destroy:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }
  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServPolicyRowStatusSet(UnitIndex, policyIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyGet(agentDiffServPolicyEntry_t *agentDiffServPolicyEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len;

  if (usmDbDiffServPolicyGet(USMDB_UNIT_CURRENT, agentDiffServPolicyEntryData->agentDiffServPolicyIndex) == L7_SUCCESS)
  {
    CLR_VALID(nominator, agentDiffServPolicyEntryData->valid);

/*
 * if ( nominator != -1 ) condition is added to all the case statements 
 * for storing all the values to support the undo functionality. 
 */ 
    switch (nominator)
    {
    case -1:
    case I_agentDiffServPolicyIndex:
      rc = L7_SUCCESS;
      if ( nominator != -1 ) break;
         /* else pass through */

    case I_agentDiffServPolicyName :
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      snmp_buffer_len = SNMP_BUFFER_LEN;
      if (usmDbDiffServPolicyNameGet(USMDB_UNIT_CURRENT, 
                    agentDiffServPolicyEntryData->agentDiffServPolicyIndex,
                    snmp_buffer, &snmp_buffer_len) == L7_SUCCESS &&
          (SafeMakeOctetString(&agentDiffServPolicyEntryData->agentDiffServPolicyName, 
                               snmp_buffer, snmp_buffer_len-1) == L7_TRUE))
      {
        rc = L7_SUCCESS;
        SET_VALID(I_agentDiffServPolicyName, agentDiffServPolicyEntryData->valid);
      }

      if ( nominator != -1 ) break;
         /* else pass through */

    case I_agentDiffServPolicyType :
      rc = snmpDiffServPolicyTypeGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyEntryData->agentDiffServPolicyIndex,
                   &agentDiffServPolicyEntryData->agentDiffServPolicyType);
      if( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyType, agentDiffServPolicyEntryData->valid);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServPolicyInstIndexNextFree :
      rc = usmDbDiffServPolicyInstIndexNext(USMDB_UNIT_CURRENT, 
                      agentDiffServPolicyEntryData->agentDiffServPolicyIndex,
                      &agentDiffServPolicyEntryData->agentDiffServPolicyInstIndexNextFree);
      if ( nominator != -1 ) break;
         /* else pass through */

    case I_agentDiffServPolicyStorageType :
      rc = snmpDiffServPolicyStorageTypeGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyEntryData->agentDiffServPolicyIndex,
                   &agentDiffServPolicyEntryData->agentDiffServPolicyStorageType);
      if( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyStorageType, agentDiffServPolicyEntryData->valid);
      if ( nominator != -1 ) break;
         /* else pass through */

    case I_agentDiffServPolicyRowStatus :
      rc = snmpDiffServPolicyRowStatusGet(USMDB_UNIT_CURRENT, 
                 agentDiffServPolicyEntryData->agentDiffServPolicyIndex,
                 &agentDiffServPolicyEntryData->agentDiffServPolicyRowStatus);
      if( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyRowStatus, agentDiffServPolicyEntryData->valid);
      break;

    default:
      rc = L7_FAILURE;
      break;
    }

    if (nominator >= 0 && rc == L7_SUCCESS)
      SET_VALID(nominator, agentDiffServPolicyEntryData->valid);
    else if (nominator == -1)
      rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyGetNext(agentDiffServPolicyEntry_t *agentDiffServPolicyEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while (usmDbDiffServPolicyGetNext(USMDB_UNIT_CURRENT, 
                                    agentDiffServPolicyEntryData->agentDiffServPolicyIndex, 
                                    &agentDiffServPolicyEntryData->agentDiffServPolicyIndex) == L7_SUCCESS)
  {
    if (snmpDiffServPolicyGet(agentDiffServPolicyEntryData, nominator) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpDiffServPolicyInstStorageTypeGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_int32 *val)
{
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;
  L7_RC_t rc;

  rc = usmDbDiffServPolicyInstStorageTypeGet(UnitIndex, policyIndex, policyInstIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_agentDiffServPolicyInstStorageType_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_agentDiffServPolicyInstStorageType_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_agentDiffServPolicyInstStorageType_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_agentDiffServPolicyInstStorageType_readOnly;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_agentDiffServPolicyInstStorageType_permanent;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyInstRowStatusGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_int32 *val)
{
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;
  L7_RC_t rc;

  rc = usmDbDiffServPolicyInstRowStatusGet (UnitIndex, policyIndex, policyInstIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_agentDiffServPolicyInstRowStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_agentDiffServPolicyInstRowStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_agentDiffServPolicyInstRowStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_agentDiffServPolicyInstRowStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_agentDiffServPolicyInstRowStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_agentDiffServPolicyInstRowStatus_destroy;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyInstStorageTypeSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_int32 val)
{
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServPolicyInstStorageType_other:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER;
    break;

  case D_agentDiffServPolicyInstStorageType_volatile:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE;
    break;

  case D_agentDiffServPolicyInstStorageType_nonVolatile:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE;
    break;

  case D_agentDiffServPolicyInstStorageType_permanent:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT;
    break;

  case D_agentDiffServPolicyInstStorageType_readOnly:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServPolicyInstStorageTypeSet(UnitIndex, policyIndex, policyInstIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyInstRowStatusSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_int32 val)
{
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServPolicyInstRowStatus_active:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
    break;

  case D_agentDiffServPolicyInstRowStatus_notInService:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;
    break;

  case D_agentDiffServPolicyInstRowStatus_notReady:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY;
    break;

  case D_agentDiffServPolicyInstRowStatus_createAndGo:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO;
    break;

  case D_agentDiffServPolicyInstRowStatus_createAndWait:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT;
    break; 

  case D_agentDiffServPolicyInstRowStatus_destroy:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServPolicyInstRowStatusSet(UnitIndex, policyIndex, policyInstIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyInstGet(agentDiffServPolicyInstEntry_t *agentDiffServPolicyInstEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  if (usmDbDiffServPolicyInstGet(USMDB_UNIT_CURRENT, 
                                 agentDiffServPolicyInstEntryData->agentDiffServPolicyIndex,
                                 agentDiffServPolicyInstEntryData->agentDiffServPolicyInstIndex) == L7_SUCCESS)
  {
    CLR_VALID(nominator, agentDiffServPolicyInstEntryData->valid);

/*
 * if ( nominator != -1 ) condition is added to all the case statements 
 * for storing all the values to support the undo functionality. 
 */ 
    switch (nominator)
    {
    case -1:
    case I_agentDiffServPolicyInstEntryIndex_agentDiffServPolicyIndex:
    case I_agentDiffServPolicyInstIndex:
      rc = L7_SUCCESS;
      if ( nominator != -1 ) break;
         /* else pass through */

    case I_agentDiffServPolicyInstClassIndex :
      rc = usmDbDiffServPolicyInstClassIndexGet(USMDB_UNIT_CURRENT,
                   agentDiffServPolicyInstEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyInstEntryData->agentDiffServPolicyInstIndex,
                   &agentDiffServPolicyInstEntryData->agentDiffServPolicyInstClassIndex);
      if( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyInstClassIndex, agentDiffServPolicyInstEntryData->valid);
      if ( nominator != -1 ) break;
         /* else pass through */

    case I_agentDiffServPolicyInstAttrIndexNextFree :
      rc = usmDbDiffServPolicyAttrIndexNext(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyInstEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyInstEntryData->agentDiffServPolicyInstIndex,
                   &agentDiffServPolicyInstEntryData->agentDiffServPolicyInstAttrIndexNextFree);
      if( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyInstAttrIndexNextFree, agentDiffServPolicyInstEntryData->valid);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServPolicyInstStorageType :
      rc = snmpDiffServPolicyInstStorageTypeGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyInstEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyInstEntryData->agentDiffServPolicyInstIndex,
                   &agentDiffServPolicyInstEntryData->agentDiffServPolicyInstStorageType);
      if( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyInstStorageType, agentDiffServPolicyInstEntryData->valid);
      if ( nominator != -1 ) break;
         /* else pass through */

    case I_agentDiffServPolicyInstRowStatus :
      rc = snmpDiffServPolicyInstRowStatusGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyInstEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyInstEntryData->agentDiffServPolicyInstIndex,
                   &agentDiffServPolicyInstEntryData->agentDiffServPolicyInstRowStatus);
      if( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyInstRowStatus, agentDiffServPolicyInstEntryData->valid);
      break;

    default:
      rc = L7_FAILURE;
      break;
    }

    if (nominator >= 0 && rc == L7_SUCCESS)
      SET_VALID(nominator, agentDiffServPolicyInstEntryData->valid);
    else if (nominator == -1)
      rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyInstGetNext(agentDiffServPolicyInstEntry_t *agentDiffServPolicyInstEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while (usmDbDiffServPolicyInstGetNext(USMDB_UNIT_CURRENT, 
                                        agentDiffServPolicyInstEntryData->agentDiffServPolicyIndex, 
                                        agentDiffServPolicyInstEntryData->agentDiffServPolicyInstIndex, 
                                        &agentDiffServPolicyInstEntryData->agentDiffServPolicyIndex,
                                        &agentDiffServPolicyInstEntryData->agentDiffServPolicyInstIndex) == L7_SUCCESS)
  {
    if (snmpDiffServPolicyInstGet(agentDiffServPolicyInstEntryData, nominator) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }
  }

  return rc;
}

/**************************************************************************************************************/


L7_RC_t
snmpDiffServPolicyAttrStmtEntryTypeGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t temp_val;

  rc = usmDbDiffServPolicyAttrStmtEntryTypeGet(UnitIndex, policyIndex, policyInstIndex, policyAttrIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
      *val = D_agentDiffServPolicyAttrStmtEntryType_assignQueue;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      *val = D_agentDiffServPolicyAttrStmtEntryType_drop;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      *val = D_agentDiffServPolicyAttrStmtEntryType_markCosVal;
      break;

#ifdef D_agentDiffServPolicyAttrStmtEntryType_markCosAsSecCos
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
      *val = D_agentDiffServPolicyAttrStmtEntryType_markCosAsSecCos;
      break;
#endif

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
      *val = D_agentDiffServPolicyAttrStmtEntryType_markCos2Val;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      *val = D_agentDiffServPolicyAttrStmtEntryType_markIpDscpVal;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      *val = D_agentDiffServPolicyAttrStmtEntryType_markIpPrecedenceVal;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
      *val = D_agentDiffServPolicyAttrStmtEntryType_mirror;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      *val = D_agentDiffServPolicyAttrStmtEntryType_policeSimple;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      *val = D_agentDiffServPolicyAttrStmtEntryType_policeSinglerate;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      *val = D_agentDiffServPolicyAttrStmtEntryType_policeTworate;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
      *val = D_agentDiffServPolicyAttrStmtEntryType_redirect;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyAttrStmtPoliceConformActGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t temp_val;

  rc = usmDbDiffServPolicyAttrStmtPoliceConformActGet(UnitIndex, policyIndex, policyInstIndex, policyAttrIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
      *val = D_agentDiffServPolicyAttrStmtPoliceConformAct_drop;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
      *val = D_agentDiffServPolicyAttrStmtPoliceConformAct_markcos;
      break;

#ifdef D_agentDiffServPolicyAttrStmtPoliceConformAct_markcosAsSecCos
    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
      *val = D_agentDiffServPolicyAttrStmtPoliceConformAct_markcosAsSecCos;
      break;
#endif

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
      *val = D_agentDiffServPolicyAttrStmtPoliceConformAct_markcos2;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
      *val = D_agentDiffServPolicyAttrStmtPoliceConformAct_markdscp;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
      *val = D_agentDiffServPolicyAttrStmtPoliceConformAct_markprec;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
      *val = D_agentDiffServPolicyAttrStmtPoliceConformAct_send;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyAttrStmtPoliceExceedActGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t temp_val;

  rc = usmDbDiffServPolicyAttrStmtPoliceExceedActGet(UnitIndex, policyIndex, policyInstIndex, policyAttrIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
      *val = D_agentDiffServPolicyAttrStmtPoliceExceedAct_drop;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
      *val = D_agentDiffServPolicyAttrStmtPoliceExceedAct_markcos;
      break;

#ifdef D_agentDiffServPolicyAttrStmtPoliceExceedAct_markcosAsSecCos
    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
      *val = D_agentDiffServPolicyAttrStmtPoliceExceedAct_markcosAsSecCos;
      break;
#endif

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
      *val = D_agentDiffServPolicyAttrStmtPoliceExceedAct_markcos2;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
      *val = D_agentDiffServPolicyAttrStmtPoliceExceedAct_markdscp;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
      *val = D_agentDiffServPolicyAttrStmtPoliceExceedAct_markprec;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
      *val = D_agentDiffServPolicyAttrStmtPoliceExceedAct_send;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyAttrStmtPoliceNonconformActGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t temp_val;

  rc = usmDbDiffServPolicyAttrStmtPoliceNonconformActGet(UnitIndex, policyIndex, policyInstIndex, policyAttrIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
      *val = D_agentDiffServPolicyAttrStmtPoliceNonconformAct_drop;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
      *val = D_agentDiffServPolicyAttrStmtPoliceNonconformAct_markcos;
      break;

#ifdef D_agentDiffServPolicyAttrStmtPoliceNonconformAct_markcosAsSecCos
    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
      *val = D_agentDiffServPolicyAttrStmtPoliceNonconformAct_markcosAsSecCos;
      break;
#endif

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
      *val = D_agentDiffServPolicyAttrStmtPoliceNonconformAct_markcos2;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
      *val = D_agentDiffServPolicyAttrStmtPoliceNonconformAct_markdscp;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
      *val = D_agentDiffServPolicyAttrStmtPoliceNonconformAct_markprec;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
      *val = D_agentDiffServPolicyAttrStmtPoliceNonconformAct_send;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyAttrStorageTypeGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_int32 *val)
{
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;
  L7_RC_t rc;

  rc = usmDbDiffServPolicyAttrStorageTypeGet(UnitIndex, policyIndex, policyInstIndex, policyAttrIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_agentDiffServPolicyAttrStorageType_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_agentDiffServPolicyAttrStorageType_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_agentDiffServPolicyAttrStorageType_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_agentDiffServPolicyAttrStorageType_readOnly;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_agentDiffServPolicyAttrStorageType_permanent;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyAttrRowStatusGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_int32 *val)
{
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;
  L7_RC_t rc;

  rc = usmDbDiffServPolicyAttrRowStatusGet (UnitIndex, policyIndex, policyInstIndex, policyAttrIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_agentDiffServPolicyAttrRowStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_agentDiffServPolicyAttrRowStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_agentDiffServPolicyAttrRowStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_agentDiffServPolicyAttrRowStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_agentDiffServPolicyAttrRowStatus_createAndWait;
      break;


    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_agentDiffServPolicyAttrRowStatus_destroy;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyAttrStmtEntryTypeSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t temp_val = 0;

  switch (val)
  {
  case D_agentDiffServPolicyAttrStmtEntryType_bandwidth:
    /* deprecated */
      rc = L7_FAILURE;
    break;

  case D_agentDiffServPolicyAttrStmtEntryType_expedite:
    /* deprecated */
      rc = L7_FAILURE;
    break;

  case D_agentDiffServPolicyAttrStmtEntryType_markCosVal:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

#ifdef D_agentDiffServPolicyAttrStmtEntryType_markCosAsSecCos
  case D_agentDiffServPolicyAttrStmtEntryType_markCosAsSecCos:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;
#endif

  case D_agentDiffServPolicyAttrStmtEntryType_markIpDscpVal:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServPolicyAttrStmtEntryType_markIpPrecedenceVal:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServPolicyAttrStmtEntryType_mirror:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_POLICY_ATTR_MIRROR_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServPolicyAttrStmtEntryType_policeSimple:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_POLICY_ATTR_POLICE_SIMPLE_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServPolicyAttrStmtEntryType_policeSinglerate:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_POLICY_ATTR_POLICE_SINGLERATE_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServPolicyAttrStmtEntryType_policeTworate:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_POLICY_ATTR_POLICE_TWORATE_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServPolicyAttrStmtEntryType_randomdrop:
    /* deprecated */
      rc = L7_FAILURE;
    break;

  case D_agentDiffServPolicyAttrStmtEntryType_shapeAverage:
    /* deprecated */
      rc = L7_FAILURE;
    break;

  case D_agentDiffServPolicyAttrStmtEntryType_shapePeak:
    /* deprecated */
      rc = L7_FAILURE;
    break;

  case D_agentDiffServPolicyAttrStmtEntryType_assignQueue:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServPolicyAttrStmtEntryType_drop:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_POLICY_ATTR_DROP_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServPolicyAttrStmtEntryType_markCos2Val:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  case D_agentDiffServPolicyAttrStmtEntryType_redirect:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                 L7_DIFFSERV_POLICY_ATTR_REDIRECT_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServPolicyAttrStmtEntryTypeSet(UnitIndex, policyIndex, policyInstIndex, policyAttrIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyAttrStmtPoliceConformActSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t temp_val = 0;

  switch (val)
  {
  case D_agentDiffServPolicyAttrStmtPoliceConformAct_drop:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP;
    break;

  case D_agentDiffServPolicyAttrStmtPoliceConformAct_markdscp:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP;
    break;

  case D_agentDiffServPolicyAttrStmtPoliceConformAct_markprec:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC;
    break;

  case D_agentDiffServPolicyAttrStmtPoliceConformAct_send:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND;
    break;

  case D_agentDiffServPolicyAttrStmtPoliceConformAct_markcos:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS;
    break;

#ifdef D_agentDiffServPolicyAttrStmtPoliceConformAct_markcosAsSecCos
  case D_agentDiffServPolicyAttrStmtPoliceConformAct_markcosAsSecCos:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2;
    break;
#endif

  case D_agentDiffServPolicyAttrStmtPoliceConformAct_markcos2:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServPolicyAttrStmtPoliceConformActSet(UnitIndex, policyIndex, policyInstIndex, policyAttrIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyAttrStmtPoliceExceedActSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t temp_val = 0;

  switch (val)
  {
  case D_agentDiffServPolicyAttrStmtPoliceExceedAct_drop:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP;
    break;

  case D_agentDiffServPolicyAttrStmtPoliceExceedAct_markdscp:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP;
    break;

  case D_agentDiffServPolicyAttrStmtPoliceExceedAct_markprec:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC;
    break;

  case D_agentDiffServPolicyAttrStmtPoliceExceedAct_send:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND;
    break;

  case D_agentDiffServPolicyAttrStmtPoliceExceedAct_markcos:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS;
    break;

#ifdef D_agentDiffServPolicyAttrStmtPoliceExceedAct_markcosAsSecCos
  case D_agentDiffServPolicyAttrStmtPoliceExceedAct_markcosAsSecCos:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2;
    break;
#endif

  case D_agentDiffServPolicyAttrStmtPoliceExceedAct_markcos2:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServPolicyAttrStmtPoliceExceedActSet(UnitIndex, policyIndex, policyInstIndex, policyAttrIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyAttrStmtPoliceNononformActSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t temp_val = 0;

  switch (val)
  {
  case D_agentDiffServPolicyAttrStmtPoliceNonconformAct_drop:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP;
    break;

  case D_agentDiffServPolicyAttrStmtPoliceNonconformAct_markdscp:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP;
    break;

  case D_agentDiffServPolicyAttrStmtPoliceNonconformAct_markprec:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC;
    break;

  case D_agentDiffServPolicyAttrStmtPoliceNonconformAct_send:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND;
    break;

  case D_agentDiffServPolicyAttrStmtPoliceNonconformAct_markcos:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS;
    break;

#ifdef D_agentDiffServPolicyAttrStmtPoliceNonconformAct_markcosAsSecCos
  case D_agentDiffServPolicyAttrStmtPoliceNonconformAct_markcosAsSecCos:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2;
    break;
#endif

  case D_agentDiffServPolicyAttrStmtPoliceNonconformAct_markcos2:
    temp_val = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServPolicyAttrStmtPoliceNonconformActSet(UnitIndex, policyIndex, policyInstIndex, policyAttrIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyAttrStorageTypeSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_int32 val)
{
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServPolicyAttrStorageType_other:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER;
    break;

  case D_agentDiffServPolicyAttrStorageType_volatile:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE;
    break;

  case D_agentDiffServPolicyAttrStorageType_nonVolatile:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE;
    break;

  case D_agentDiffServPolicyAttrStorageType_permanent:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT;
    break;

  case D_agentDiffServPolicyAttrStorageType_readOnly:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServPolicyAttrStorageTypeSet(UnitIndex, policyIndex, policyInstIndex, policyAttrIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyAttrRowStatusSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_int32 val)
{
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentDiffServPolicyAttrRowStatus_active:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
    break;

  case D_agentDiffServPolicyAttrRowStatus_notInService:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;
    break;

  case D_agentDiffServPolicyAttrRowStatus_notReady:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY;
    break;

  case D_agentDiffServPolicyAttrRowStatus_createAndGo:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO;
    break;

  case D_agentDiffServPolicyAttrRowStatus_createAndWait:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT;
    break; 

  case D_agentDiffServPolicyAttrRowStatus_destroy:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }
  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServPolicyAttrRowStatusSet(UnitIndex, policyIndex, policyInstIndex, policyAttrIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentDiffServPolicyAttrStmtDropGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_t temp_val;

  rc = usmDbDiffServPolicyAttrStmtDropFlagGet(UnitIndex,policyIndex,policyInstIndex,policyAttrIndex,&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE:
      *val = D_agentDiffServPolicyAttrStmtDrop_true;
      break;

    case L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE:
      *val = D_agentDiffServPolicyAttrStmtDrop_false;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentDiffServPolicyAttrStmtPoliceColorConformIndexSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val;

  if (val != 0)
  {
    rc = usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(UnitIndex,policyIndex,policyInstIndex,val);
  }

  if (rc == L7_SUCCESS)
  {
    /* make sure conform color and exceed color are not the same */
    if (usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexGet(UnitIndex,policyIndex,policyInstIndex,policyAttrIndex,&temp_val) == L7_SUCCESS)
    {
      if (usmDbDiffServPolicyAttrStmtPoliceColorAwareClassesAreEquivalent(UnitIndex, val, temp_val) == L7_TRUE)
        return L7_FAILURE;
    }
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServPolicyAttrStmtPoliceColorConformIndexSet(UnitIndex,policyIndex,policyInstIndex,policyAttrIndex,val);
  }

  return rc;
}

L7_RC_t
snmpAgentDiffServPolicyAttrStmtPoliceColorConformModeGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t temp_val;

  rc = usmDbDiffServPolicyAttrStmtPoliceColorConformModeGet(UnitIndex,policyIndex,policyInstIndex,policyAttrIndex,&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND:
      *val = D_agentDiffServPolicyAttrStmtPoliceColorConformMode_blind;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
      *val = D_agentDiffServPolicyAttrStmtPoliceColorConformMode_cos;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
      *val = D_agentDiffServPolicyAttrStmtPoliceColorConformMode_cos2;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
      *val = D_agentDiffServPolicyAttrStmtPoliceColorConformMode_ipdscp;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
      *val = D_agentDiffServPolicyAttrStmtPoliceColorConformMode_ipprec;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentDiffServPolicyAttrStmtPoliceColorExceedIndexSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  if (val != 0)
  {
    rc = usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(UnitIndex,policyIndex,policyInstIndex,val);
  }

  if (rc == L7_SUCCESS)
  {
    /* do not allow non-zero (color-aware) exceed index to be set if
     * the color conform index is zero (color-blind)
     *
     * make sure conform color and exceed color are not the same
     */
    if ((usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet(UnitIndex,policyIndex,policyInstIndex,policyAttrIndex,&temp_val) != L7_SUCCESS) ||
        ((val != 0) && (temp_val == 0)) ||
        (usmDbDiffServPolicyAttrStmtPoliceColorAwareClassesAreEquivalent(UnitIndex, val, temp_val) == L7_TRUE))
    {
      rc = L7_FAILURE;
    }
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexSet(UnitIndex,policyIndex,policyInstIndex,policyAttrIndex,val);
  }

  return rc;
}

L7_RC_t
snmpAgentDiffServPolicyAttrStmtPoliceColorExceedModeGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 policyAttrIndex, L7_int32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t temp_val;

  rc = usmDbDiffServPolicyAttrStmtPoliceColorExceedModeGet(UnitIndex,policyIndex,policyInstIndex,policyAttrIndex,&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND:
      *val = D_agentDiffServPolicyAttrStmtPoliceColorExceedMode_blind;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
      *val = D_agentDiffServPolicyAttrStmtPoliceColorExceedMode_cos;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
      *val = D_agentDiffServPolicyAttrStmtPoliceColorExceedMode_cos2;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
      *val = D_agentDiffServPolicyAttrStmtPoliceColorExceedMode_ipdscp;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
      *val = D_agentDiffServPolicyAttrStmtPoliceColorExceedMode_ipprec;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED:
      *val = D_agentDiffServPolicyAttrStmtPoliceColorExceedMode_unused;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyAttrGet(agentDiffServPolicyAttrEntry_t *agentDiffServPolicyAttrEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  if (usmDbDiffServPolicyAttrGet(USMDB_UNIT_CURRENT, 
                                 agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex, 
                                 agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex, 
                                 agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex) == L7_SUCCESS)
  {
    CLR_VALID(nominator, agentDiffServPolicyAttrEntryData->valid);

/*
 * if ( nominator != -1 ) condition is added to all the case statements 
 * for storing all the values to support the undo functionality. 
 */ 
    switch (nominator)
    {
    case -1:
    case I_agentDiffServPolicyAttrEntryIndex_agentDiffServPolicyIndex:
    case I_agentDiffServPolicyAttrEntryIndex_agentDiffServPolicyInstIndex:
    case I_agentDiffServPolicyAttrIndex:
      rc = L7_SUCCESS;
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtEntryType :
      rc = snmpDiffServPolicyAttrStmtEntryTypeGet(USMDB_UNIT_CURRENT, 
                                                  agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                  agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                  agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                  &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtEntryType);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtEntryType, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtMarkCosVal :
      rc = usmDbDiffServPolicyAttrStmtMarkCosValGet(USMDB_UNIT_CURRENT, 
                                                    agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                    agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                    agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                    &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtMarkCosVal);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtMarkCosVal, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

#ifdef I_agentDiffServPolicyAttrStmtMarkCosAsSecCos
    case I_agentDiffServPolicyAttrStmtMarkCosAsSecCos :
      rc = usmDbDiffServPolicyAttrStmtMarkCosAsCos2Get(USMDB_UNIT_CURRENT, 
                                                    agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                    agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                    agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                    &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtMarkCosAsSecCos);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtMarkCosAsSecCos, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */
#endif

    case I_agentDiffServPolicyAttrStmtMarkIpDscpVal :
      rc = usmDbDiffServPolicyAttrStmtMarkIpDscpValGet(USMDB_UNIT_CURRENT, 
                                                       agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                       agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                       agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                       &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtMarkIpDscpVal);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtMarkIpDscpVal, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtMarkIpPrecedenceVal :
      rc = usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValGet(USMDB_UNIT_CURRENT, 
                                                             agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                             agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                             agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                             &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtMarkIpPrecedenceVal);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtMarkIpPrecedenceVal, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceConformAct :
      rc = snmpDiffServPolicyAttrStmtPoliceConformActGet(USMDB_UNIT_CURRENT, 
                                                         agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                         agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                         agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                         &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceConformAct);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceConformAct, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceConformVal :
      rc = usmDbDiffServPolicyAttrStmtPoliceConformValGet(USMDB_UNIT_CURRENT, 
                                                          agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                          agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                          agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                          &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceConformVal);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceConformVal, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceExceedAct :
      rc = snmpDiffServPolicyAttrStmtPoliceExceedActGet(USMDB_UNIT_CURRENT, 
                                                        agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                        agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                        agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                        &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceExceedAct);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceExceedAct, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceExceedVal :
      rc = usmDbDiffServPolicyAttrStmtPoliceExceedValGet(USMDB_UNIT_CURRENT, 
                                                         agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                         agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                         agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                         &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceExceedVal);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceExceedVal, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceNonconformAct :
      rc = snmpDiffServPolicyAttrStmtPoliceNonconformActGet(USMDB_UNIT_CURRENT, 
                                                            agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                            agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                            agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                            &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceNonconformAct);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceNonconformAct, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceNonconformVal :
      rc = usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(USMDB_UNIT_CURRENT, 
                                                             agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                             agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                             agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                             &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceNonconformVal);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceNonconformVal, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceSimpleCrate :
      rc = usmDbDiffServPolicyAttrStmtPoliceSimpleCrateGet(USMDB_UNIT_CURRENT, 
                                                           agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                           agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                           agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                           &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceSimpleCrate);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceSimpleCrate, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceSimpleCburst :
      rc = usmDbDiffServPolicyAttrStmtPoliceSimpleCburstGet(USMDB_UNIT_CURRENT, 
                                                            agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                            agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                            agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                            &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceSimpleCburst);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceSimpleCburst, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceSinglerateCrate :
      rc = usmDbDiffServPolicyAttrStmtPoliceSinglerateCrateGet(USMDB_UNIT_CURRENT, 
                                                               agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                               agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                               agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                               &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceSinglerateCrate);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceSinglerateCrate, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceSinglerateCburst :
      rc = usmDbDiffServPolicyAttrStmtPoliceSinglerateCburstGet(USMDB_UNIT_CURRENT, 
                                                                agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                                agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                                agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                                &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceSinglerateCburst);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceSinglerateCburst, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceSinglerateEburst :
      rc = usmDbDiffServPolicyAttrStmtPoliceSinglerateEburstGet(USMDB_UNIT_CURRENT, 
                                                                agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                                agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                                agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                                &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceSinglerateEburst);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceSinglerateEburst, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceTworateCrate :
      rc = usmDbDiffServPolicyAttrStmtPoliceTworateCrateGet(USMDB_UNIT_CURRENT, 
                                                            agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                            agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                            agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                            &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceTworateCrate);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceTworateCrate, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceTworateCburst :
      rc = usmDbDiffServPolicyAttrStmtPoliceTworateCburstGet(USMDB_UNIT_CURRENT, 
                                                             agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                             agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                             agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                             &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceTworateCburst);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceTworateCburst, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceTworatePrate :
      rc = usmDbDiffServPolicyAttrStmtPoliceTworatePrateGet(USMDB_UNIT_CURRENT, 
                                                            agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                            agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                            agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                            &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceTworatePrate);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceTworatePrate, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceTworatePburst :
      rc = usmDbDiffServPolicyAttrStmtPoliceTworatePburstGet(USMDB_UNIT_CURRENT, 
                                                             agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                             agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                             agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                             &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceTworatePburst);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceTworatePburst, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStorageType :
      rc = snmpDiffServPolicyAttrStorageTypeGet(USMDB_UNIT_CURRENT, 
                                                agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStorageType);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStorageType, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrRowStatus :
      rc = snmpDiffServPolicyAttrRowStatusGet(USMDB_UNIT_CURRENT, 
                                              agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                              agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                              agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                              &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrRowStatus);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrRowStatus, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtAssignQueueId :
      rc = usmDbDiffServPolicyAttrStmtAssignQueueIdGet(USMDB_UNIT_CURRENT, 
                                                       agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                       agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                       agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                       &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtAssignQueueId);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtAssignQueueId, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtDrop :
      rc = snmpAgentDiffServPolicyAttrStmtDropGet(USMDB_UNIT_CURRENT, 
                                                  agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                  agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                  agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                  &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtDrop);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtDrop, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtMarkCos2Val :
      rc = usmDbDiffServPolicyAttrStmtMarkCos2ValGet(USMDB_UNIT_CURRENT, 
                                                     agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                     agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                     agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                     &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtMarkCos2Val);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtMarkCos2Val, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceColorConformIndex :
      rc = usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet(USMDB_UNIT_CURRENT, 
                                                                 agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                                 agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                                 agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                                 &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceColorConformIndex);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceColorConformIndex, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceColorConformMode :
      rc = snmpAgentDiffServPolicyAttrStmtPoliceColorConformModeGet(USMDB_UNIT_CURRENT, 
                                                                    agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                                    agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                                    agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                                    &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceColorConformMode);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceColorConformMode, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceColorConformVal :
      rc = usmDbDiffServPolicyAttrStmtPoliceColorConformValGet(USMDB_UNIT_CURRENT, 
                                                               agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                               agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                               agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                               &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceColorConformVal);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceColorConformVal, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceColorExceedIndex :
      rc = usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexGet(USMDB_UNIT_CURRENT, 
                                                                agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                                agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                                agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                                &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceColorExceedIndex);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceColorExceedIndex, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceColorExceedMode :
      rc = snmpAgentDiffServPolicyAttrStmtPoliceColorExceedModeGet(USMDB_UNIT_CURRENT, 
                                                                   agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                                   agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                                   agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                                   &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceColorExceedMode);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceColorExceedMode, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtPoliceColorExceedVal :
      rc = usmDbDiffServPolicyAttrStmtPoliceColorExceedValGet(USMDB_UNIT_CURRENT, 
                                                              agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                              agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                              agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                              &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtPoliceColorExceedVal);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtPoliceColorExceedVal, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtRedirectIntf :
      rc = usmDbDiffServPolicyAttrStmtRedirectIntfGet(USMDB_UNIT_CURRENT, 
                                                      agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                      agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                      agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                      &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtRedirectIntf);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtRedirectIntf, agentDiffServPolicyAttrEntryData->valid);
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDiffServPolicyAttrStmtMirrorIntf :
      rc = usmDbDiffServPolicyAttrStmtMirrorIntfGet(USMDB_UNIT_CURRENT, 
                                                    agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex,
                                                    agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex,
                                                    agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                                    &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrStmtMirrorIntf);
      if ( rc == L7_SUCCESS )
        SET_VALID(I_agentDiffServPolicyAttrStmtMirrorIntf, agentDiffServPolicyAttrEntryData->valid);
      break;

    default:
      rc = L7_FAILURE;
      break;
    }

    if (nominator >= 0 && rc == L7_SUCCESS)
      SET_VALID(nominator, agentDiffServPolicyAttrEntryData->valid);
    else if (nominator == -1)
      rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyAttrGetNext(agentDiffServPolicyAttrEntry_t *agentDiffServPolicyAttrEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;


  while (usmDbDiffServPolicyAttrGetNext(USMDB_UNIT_CURRENT, 
                                        agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex, 
                                        agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex, 
                                        agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex,
                                        &agentDiffServPolicyAttrEntryData->agentDiffServPolicyIndex, 
                                        &agentDiffServPolicyAttrEntryData->agentDiffServPolicyInstIndex, 
                                        &agentDiffServPolicyAttrEntryData->agentDiffServPolicyAttrIndex) == L7_SUCCESS)
  {
    if (snmpDiffServPolicyAttrGet(agentDiffServPolicyAttrEntryData, nominator) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpDiffServPolicyPerfInStorageTypeGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;
  L7_RC_t rc;

  rc = usmDbDiffServPolicyPerfInStorageTypeGet(UnitIndex, policyIndex, policyInstIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_agentDiffServPolicyPerfInStorageType_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_agentDiffServPolicyPerfInStorageType_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_agentDiffServPolicyPerfInStorageType_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_agentDiffServPolicyPerfInStorageType_readOnly;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_agentDiffServPolicyPerfInStorageType_permanent;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyPerfInRowStatusGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;
  L7_RC_t rc;

  rc = usmDbDiffServPolicyPerfInRowStatusGet (UnitIndex, policyIndex, policyInstIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_agentDiffServPolicyPerfInRowStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_agentDiffServPolicyPerfInRowStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_agentDiffServPolicyPerfInRowStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_agentDiffServPolicyPerfInRowStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_agentDiffServPolicyPerfInRowStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_agentDiffServPolicyPerfInRowStatus_destroy;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyPerfInGet(agentDiffServPolicyPerfInEntry_t *agentDiffServPolicyPerfInEntryData, L7_uint32 intIfNum, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_uint;

  if (usmDbDiffServPolicyPerfInGet(USMDB_UNIT_CURRENT, 
                                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyIndex, 
                                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyInstIndex, 
                                   intIfNum) == L7_SUCCESS)
  {
    switch (nominator)
    {
    case -1:
    case I_agentDiffServPolicyPerfInEntryIndex_agentDiffServPolicyIndex:
    case I_agentDiffServPolicyPerfInEntryIndex_agentDiffServPolicyInstIndex:
    case I_agentDiffServPolicyPerfInEntryIndex_ifIndex:
      rc = L7_SUCCESS;
      break;

    case I_agentDiffServPolicyPerfInOfferedOctets:

      rc = usmDbDiffServPolicyPerfInOfferedOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyPerfInOfferedOctets);
      break;


    case I_agentDiffServPolicyPerfInOfferedPackets:

      rc = usmDbDiffServPolicyPerfInOfferedPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyPerfInOfferedPackets);
      break; 


    case I_agentDiffServPolicyPerfInDiscardedOctets:

      rc = usmDbDiffServPolicyPerfInDiscardedOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyPerfInDiscardedOctets);
      break;


    case I_agentDiffServPolicyPerfInDiscardedPackets:

      rc = usmDbDiffServPolicyPerfInDiscardedPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyPerfInDiscardedPackets);
      break;


    case I_agentDiffServPolicyPerfInHCOfferedOctets:

      rc = usmDbDiffServPolicyPerfInOfferedOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyPerfInHCOfferedOctets->big_end,
                   &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyPerfInHCOfferedOctets->little_end);
      break;

    case I_agentDiffServPolicyPerfInHCOfferedPackets:

      rc = usmDbDiffServPolicyPerfInOfferedPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyPerfInHCOfferedPackets->big_end,
                   &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyPerfInHCOfferedPackets->little_end);
      break; 


    case I_agentDiffServPolicyPerfInHCDiscardedOctets:

      rc = usmDbDiffServPolicyPerfInDiscardedOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyPerfInHCDiscardedOctets->big_end,
                   &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyPerfInHCDiscardedOctets->little_end);
      break;


    case I_agentDiffServPolicyPerfInHCDiscardedPackets:

      rc = usmDbDiffServPolicyPerfInDiscardedPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyPerfInHCDiscardedPackets->big_end,
                   &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyPerfInHCDiscardedPackets->little_end);
      break;


    case I_agentDiffServPolicyPerfInStorageType:

      rc = snmpDiffServPolicyPerfInStorageTypeGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyPerfInStorageType);
      break;


    case I_agentDiffServPolicyPerfInRowStatus:

      rc = snmpDiffServPolicyPerfInRowStatusGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfInEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyPerfInRowStatus);
      break;


    default:
      rc = L7_FAILURE;
      break;
    }

    if (nominator >= 0)
    {
      if (rc == L7_SUCCESS)
      {
        SET_VALID(nominator, agentDiffServPolicyPerfInEntryData->valid);
      }
      else
      {
        CLR_VALID(nominator, agentDiffServPolicyPerfInEntryData->valid);
      }
    }

  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyPerfInGetNext(agentDiffServPolicyPerfInEntry_t *agentDiffServPolicyPerfInEntryData, L7_uint32 *intIfNum, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while ( (usmDbDiffServPolicyPerfInGetNext(USMDB_UNIT_CURRENT, 
                                            agentDiffServPolicyPerfInEntryData->agentDiffServPolicyIndex,
                                            agentDiffServPolicyPerfInEntryData->agentDiffServPolicyInstIndex,
                                            *intIfNum,
                                            &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyIndex,
                                            &agentDiffServPolicyPerfInEntryData->agentDiffServPolicyInstIndex,
                                            intIfNum) == L7_SUCCESS) &&
          (usmDbExtIfNumFromIntIfNum(*intIfNum, &agentDiffServPolicyPerfInEntryData->ifIndex) == L7_SUCCESS) )
  {
    if (snmpDiffServPolicyPerfInGet(agentDiffServPolicyPerfInEntryData, *intIfNum, nominator) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpDiffServPolicyPerfOutStorageTypeGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;
  L7_RC_t rc;

  rc = usmDbDiffServPolicyPerfOutStorageTypeGet(UnitIndex, policyIndex, policyInstIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_agentDiffServPolicyPerfOutStorageType_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_agentDiffServPolicyPerfOutStorageType_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_agentDiffServPolicyPerfOutStorageType_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_agentDiffServPolicyPerfOutStorageType_readOnly;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_agentDiffServPolicyPerfOutStorageType_permanent;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyPerfOutRowStatusGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyInstIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;
  L7_RC_t rc;

  rc = usmDbDiffServPolicyPerfOutRowStatusGet (UnitIndex, policyIndex, policyInstIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_agentDiffServPolicyPerfOutRowStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_agentDiffServPolicyPerfOutRowStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_agentDiffServPolicyPerfOutRowStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_agentDiffServPolicyPerfOutRowStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_agentDiffServPolicyPerfOutRowStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_agentDiffServPolicyPerfOutRowStatus_destroy;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyPerfOutGet(agentDiffServPolicyPerfOutEntry_t *agentDiffServPolicyPerfOutEntryData, L7_uint32 intIfNum, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_uint;

  if (usmDbDiffServPolicyPerfOutGet(USMDB_UNIT_CURRENT, 
                                    agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex, 
                                    agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex, 
                                    intIfNum) == L7_SUCCESS)
  {
    switch (nominator)
    {
    case -1:
    case I_agentDiffServPolicyPerfOutEntryIndex_agentDiffServPolicyIndex:
    case I_agentDiffServPolicyPerfOutEntryIndex_agentDiffServPolicyInstIndex:
    case I_agentDiffServPolicyPerfOutEntryIndex_ifIndex:
      rc = L7_SUCCESS;
      break;

#ifdef OBSOLETE
    case I_agentDiffServPolicyPerfOutTailDroppedOctets:

      rc = usmDbDiffServPolicyPerfOutTailDroppedOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutTailDroppedOctets);
      break;


    case I_agentDiffServPolicyPerfOutTailDroppedPackets:

      rc = usmDbDiffServPolicyPerfOutTailDroppedPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutTailDroppedPackets);
      break;


    case I_agentDiffServPolicyPerfOutRandomDroppedOctets:

      rc = usmDbDiffServPolicyPerfOutRandomDroppedOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutRandomDroppedOctets);
      break;


    case I_agentDiffServPolicyPerfOutRandomDroppedPackets:

      rc = usmDbDiffServPolicyPerfOutRandomDroppedPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutRandomDroppedPackets);
      break;


    case I_agentDiffServPolicyPerfOutShapeDelayedOctets:

      rc = usmDbDiffServPolicyPerfOutShapeDelayedOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutShapeDelayedOctets);
      break;


    case I_agentDiffServPolicyPerfOutShapeDelayedPackets:

      rc = usmDbDiffServPolicyPerfOutShapeDelayedPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutShapeDelayedPackets);
      break;


    case I_agentDiffServPolicyPerfOutSentOctets:

      rc = usmDbDiffServPolicyPerfOutSentOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutSentOctets);
      break;


    case I_agentDiffServPolicyPerfOutSentPackets:

      rc = usmDbDiffServPolicyPerfOutSentPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutSentPackets);
      break;


    case I_agentDiffServPolicyPerfOutHCTailDroppedOctets:

      rc = usmDbDiffServPolicyPerfOutTailDroppedOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCTailDroppedOctets->big_end,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCTailDroppedOctets->little_end);
      break;

    case I_agentDiffServPolicyPerfOutHCTailDroppedPackets:

      rc = usmDbDiffServPolicyPerfOutTailDroppedPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCTailDroppedPackets->big_end,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCTailDroppedPackets->little_end);
      break;

    case I_agentDiffServPolicyPerfOutHCRandomDroppedOctets:

      rc = usmDbDiffServPolicyPerfOutRandomDroppedOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCRandomDroppedOctets->big_end,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCRandomDroppedOctets->little_end);
      break;

    case I_agentDiffServPolicyPerfOutHCRandomDroppedPackets:

      rc = usmDbDiffServPolicyPerfOutRandomDroppedPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCRandomDroppedPackets->big_end,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCRandomDroppedPackets->little_end);
      break;

    case I_agentDiffServPolicyPerfOutHCShapeDelayedOctets:

      rc = usmDbDiffServPolicyPerfOutShapeDelayedOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCShapeDelayedOctets->big_end,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCShapeDelayedOctets->little_end);
      break;

    case I_agentDiffServPolicyPerfOutHCShapeDelayedPackets:

      rc = usmDbDiffServPolicyPerfOutShapeDelayedPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCShapeDelayedPackets->big_end,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCShapeDelayedPackets->little_end);
      break;

    case I_agentDiffServPolicyPerfOutHCSentOctets:

      rc = usmDbDiffServPolicyPerfOutSentOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCSentOctets->big_end,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCSentOctets->little_end);
      break;

    case I_agentDiffServPolicyPerfOutHCSentPackets:

      rc = usmDbDiffServPolicyPerfOutSentPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCSentPackets->big_end,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCSentPackets->little_end);
      break;
#endif /* #ifdef OBSOLETE */

    case I_agentDiffServPolicyPerfOutOfferedOctets:

      rc = usmDbDiffServPolicyPerfOutOfferedOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutOfferedOctets);
      break;


    case I_agentDiffServPolicyPerfOutOfferedPackets:

      rc = usmDbDiffServPolicyPerfOutOfferedPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutOfferedPackets);
      break; 


    case I_agentDiffServPolicyPerfOutDiscardedOctets:

      rc = usmDbDiffServPolicyPerfOutDiscardedOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutDiscardedOctets);
      break;


    case I_agentDiffServPolicyPerfOutDiscardedPackets:

      rc = usmDbDiffServPolicyPerfOutDiscardedPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &temp_uint,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutDiscardedPackets);
      break;


    case I_agentDiffServPolicyPerfOutHCOfferedOctets:

      rc = usmDbDiffServPolicyPerfOutOfferedOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCOfferedOctets->big_end,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCOfferedOctets->little_end);
      break;

    case I_agentDiffServPolicyPerfOutHCOfferedPackets:

      rc = usmDbDiffServPolicyPerfOutOfferedPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCOfferedPackets->big_end,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCOfferedPackets->little_end);
      break; 


    case I_agentDiffServPolicyPerfOutHCDiscardedOctets:

      rc = usmDbDiffServPolicyPerfOutDiscardedOctetsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCDiscardedOctets->big_end,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCDiscardedOctets->little_end);
      break;


    case I_agentDiffServPolicyPerfOutHCDiscardedPackets:

      rc = usmDbDiffServPolicyPerfOutDiscardedPacketsGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCDiscardedPackets->big_end,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutHCDiscardedPackets->little_end);
      break;

    case I_agentDiffServPolicyPerfOutStorageType:

      rc = snmpDiffServPolicyPerfOutStorageTypeGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutStorageType);
      break;


    case I_agentDiffServPolicyPerfOutRowStatus:

      rc = snmpDiffServPolicyPerfOutRowStatusGet(USMDB_UNIT_CURRENT, 
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                   agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                   intIfNum,
                   &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyPerfOutRowStatus);
      break;


    default:
      rc = L7_FAILURE;
      break;
    }

    if (nominator >= 0)
    {
      if (rc == L7_SUCCESS)
      {
        SET_VALID(nominator, agentDiffServPolicyPerfOutEntryData->valid);
      }
      else
      {
        CLR_VALID(nominator, agentDiffServPolicyPerfOutEntryData->valid);
      }
    }

  }

  return rc;
}

L7_RC_t
snmpDiffServPolicyPerfOutGetNext(agentDiffServPolicyPerfOutEntry_t *agentDiffServPolicyPerfOutEntryData, L7_uint32 *intIfNum, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while ( (usmDbDiffServPolicyPerfOutGetNext(USMDB_UNIT_CURRENT, 
                                             agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                                             agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                                             *intIfNum,
                                             &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyIndex,
                                             &agentDiffServPolicyPerfOutEntryData->agentDiffServPolicyInstIndex,
                                             intIfNum) == L7_SUCCESS) &&
          (usmDbExtIfNumFromIntIfNum(*intIfNum, &agentDiffServPolicyPerfOutEntryData->ifIndex) == L7_SUCCESS) )
  {
    if (snmpDiffServPolicyPerfOutGet(agentDiffServPolicyPerfOutEntryData, *intIfNum, nominator) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpDiffServServiceIfOperStatusGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t intDirection, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_t temp_val;

  rc = usmDbDiffServServiceIfOperStatusGet(UnitIndex, intIfNum, intDirection, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_UP:
      *val = D_agentDiffServServiceIfOperStatus_up;
      break;

    case L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_DOWN:
      *val = D_agentDiffServServiceIfOperStatus_down;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServServiceStorageTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t intDirection, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val;

  rc = usmDbDiffServServiceStorageTypeGet(UnitIndex, intIfNum, intDirection, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER:
      *val = D_agentDiffServServiceStorageType_other;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE:
      *val = D_agentDiffServServiceStorageType_volatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE:
      *val = D_agentDiffServServiceStorageType_nonVolatile;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY:
      *val = D_agentDiffServServiceStorageType_readOnly;
      break;

    case L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT:
      *val = D_agentDiffServServiceStorageType_permanent;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServServiceRowStatusGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t intDirection, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val;

  rc = usmDbDiffServServiceRowStatusGet (UnitIndex, intIfNum, intDirection, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
      *val = D_agentDiffServServiceRowStatus_active;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_agentDiffServServiceRowStatus_notInService;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
      *val = D_agentDiffServServiceRowStatus_notReady;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *val = D_agentDiffServServiceRowStatus_createAndGo;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_agentDiffServServiceRowStatus_createAndWait;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
      *val = D_agentDiffServServiceRowStatus_destroy;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDiffServServiceStorageTypeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t intDirection, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t temp_val = 0;

  switch (val)
  {
  case D_agentDiffServServiceStorageType_other:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER;
    break;

  case D_agentDiffServServiceStorageType_volatile:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE;
    break;

  case D_agentDiffServServiceStorageType_nonVolatile:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE;
    break;

  case D_agentDiffServServiceStorageType_readOnly:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY;
    break;

  case D_agentDiffServServiceStorageType_permanent:
    temp_val = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServServiceStorageTypeSet(UnitIndex, intIfNum, intDirection, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDiffServServiceRowStatusSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t intDirection, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t temp_val = 0;

  switch (val)
  {
  case D_agentDiffServServiceRowStatus_active:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
    break;

  case D_agentDiffServServiceRowStatus_notInService:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;
    break;

  case D_agentDiffServServiceRowStatus_notReady:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY;
    break;

  case D_agentDiffServServiceRowStatus_createAndGo:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO;
    break;

  case D_agentDiffServServiceRowStatus_createAndWait:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT;
    break;

  case D_agentDiffServServiceRowStatus_destroy:
    temp_val = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDiffServServiceRowStatusSet (UnitIndex, intIfNum, intDirection, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDiffServServiceGet(agentDiffServServiceEntry_t *agentDiffServServiceEntryData, L7_uint32 intIfNum, L7_int32 nominator, L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t *intDirection)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 UnitIndex = USMDB_UNIT_CURRENT;
  L7_uint32 temp_val;

  /* translate ifDirection to intDirection */

/*
 * if ( nominator != -1 ) condition is added to all the case statements 
 * for storing all the values to support the undo functionality. 
 */ 
  switch (agentDiffServServiceEntryData->agentDiffServServiceIfDirection)
  {
  case D_agentDiffServServiceIfDirection_in:
    *intDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
     break;

  case D_agentDiffServServiceIfDirection_out:
    *intDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
    break;

  default:
    return L7_FAILURE;
    break;
  }

  if (usmDbDiffServServiceGet(UnitIndex, intIfNum, *intDirection) == L7_SUCCESS)
  {
    CLR_VALID(nominator, agentDiffServServiceEntryData->valid);

    switch (nominator)
    {
    case -1:
    case I_agentDiffServServiceIfIndex:
    case I_agentDiffServServiceIfDirection:
      rc = L7_SUCCESS;
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServicePolicyIndex:

      rc = usmDbDiffServServicePolicyIndexGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &agentDiffServServiceEntryData->agentDiffServServicePolicyIndex);
      if ( rc == L7_SUCCESS )
         SET_VALID(I_agentDiffServServicePolicyIndex, agentDiffServServiceEntryData->valid);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServiceIfOperStatus:

      rc = snmpDiffServServiceIfOperStatusGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &agentDiffServServiceEntryData->agentDiffServServiceIfOperStatus);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServicePerfOfferedOctets:

      rc = usmDbDiffServServicePerfOfferedOctetsGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &temp_val,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfOfferedOctets);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServicePerfOfferedPackets:

      rc = usmDbDiffServServicePerfOfferedPacketsGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &temp_val,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfOfferedPackets);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServicePerfDiscardedOctets:

      rc = usmDbDiffServServicePerfDiscardedOctetsGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &temp_val,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfDiscardedOctets);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServicePerfDiscardedPackets:

      rc = usmDbDiffServServicePerfDiscardedPacketsGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &temp_val,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfDiscardedPackets);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServicePerfSentOctets:

      rc = usmDbDiffServServicePerfSentOctetsGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &temp_val,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfSentOctets);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServicePerfSentPackets:

      rc = usmDbDiffServServicePerfSentPacketsGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &temp_val,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfSentPackets);
    if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServicePerfHCOfferedOctets:

      rc = usmDbDiffServServicePerfOfferedOctetsGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfHCOfferedOctets->big_end,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfHCOfferedOctets->little_end);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServicePerfHCOfferedPackets:

      rc = usmDbDiffServServicePerfOfferedPacketsGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfHCOfferedPackets->big_end,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfHCOfferedPackets->little_end);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServicePerfHCDiscardedOctets:

      rc = usmDbDiffServServicePerfDiscardedOctetsGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfHCDiscardedOctets->big_end,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfHCDiscardedOctets->little_end);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServicePerfHCDiscardedPackets:

      rc = usmDbDiffServServicePerfDiscardedPacketsGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfHCDiscardedPackets->big_end,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfHCDiscardedPackets->little_end);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServicePerfHCSentOctets:

      rc = usmDbDiffServServicePerfSentOctetsGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfHCSentOctets->big_end,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfHCSentOctets->little_end);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServicePerfHCSentPackets:

      rc = usmDbDiffServServicePerfSentPacketsGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfHCSentPackets->big_end,
                   &agentDiffServServiceEntryData->agentDiffServServicePerfHCSentPackets->little_end);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServiceStorageType:

      rc = snmpDiffServServiceStorageTypeGet(UnitIndex,
                   intIfNum,
                   *intDirection,
                   &agentDiffServServiceEntryData->agentDiffServServiceStorageType);
      if ( rc == L7_SUCCESS )
         SET_VALID(I_agentDiffServServiceStorageType, agentDiffServServiceEntryData->valid);
      if ( nominator != -1 ) break;
         /* else pass through */


    case I_agentDiffServServiceRowStatus:

      rc = snmpDiffServServiceRowStatusGet(USMDB_UNIT_CURRENT, 
                   intIfNum,
                   *intDirection,
                   &agentDiffServServiceEntryData->agentDiffServServiceRowStatus);
      if ( rc == L7_SUCCESS )
         SET_VALID(I_agentDiffServServiceRowStatus, agentDiffServServiceEntryData->valid);
      break;


    default:
      rc = L7_FAILURE;
      break;
    }

    if (nominator >= 0 && rc == L7_SUCCESS)
      SET_VALID(nominator, agentDiffServServiceEntryData->valid);
    else if (nominator == -1)
      rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpDiffServServiceGetNext(agentDiffServServiceEntry_t *agentDiffServServiceEntryData, L7_uint32 *intIfNum, L7_int32 nominator, L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t *intDirection)
{
  L7_RC_t rc = L7_FAILURE;
  L7_int32  direction;

  /* find the right ifDirection starting value for GetNext */
  /* NOTE: assumes values for out > in */
  direction = agentDiffServServiceEntryData->agentDiffServServiceIfDirection;
  if (direction >= D_agentDiffServServiceIfDirection_out)
  {
    *intDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
  }
  else if (direction >= D_agentDiffServServiceIfDirection_in)
  {
    *intDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
  }
  else
  {
    *intDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE;
  }

  while ( (usmDbDiffServServiceGetNext(USMDB_UNIT_CURRENT, 
                                       *intIfNum,
                                       *intDirection,
                                       intIfNum,
                                       intDirection) == L7_SUCCESS) &&
          (usmDbExtIfNumFromIntIfNum(*intIfNum, &agentDiffServServiceEntryData->agentDiffServServiceIfIndex) == L7_SUCCESS) )
  {
    /* translate new direction back to external value */
    switch (*intDirection)
    {
    case L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN:
      direction = D_agentDiffServServiceIfDirection_in;
      agentDiffServServiceEntryData->agentDiffServServiceIfDirection = direction;
      break;

    case L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT:
      direction = D_agentDiffServServiceIfDirection_out;
      agentDiffServServiceEntryData->agentDiffServServiceIfDirection = direction;
      break;

    default:
      return L7_FAILURE;
      break;
    }

    if (snmpDiffServServiceGet(agentDiffServServiceEntryData, *intIfNum, nominator, intDirection) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }
  }

  return rc;
}

