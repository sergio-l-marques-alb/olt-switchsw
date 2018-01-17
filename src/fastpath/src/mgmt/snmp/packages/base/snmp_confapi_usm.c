#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "v2clssc.h"
#include "lookup.h"
#include "v2table.h"            /* for SnmpV2Table */
#include "pw2key.h"             /* for SrTextPasswordToLocalizedKey */
#include "snmpid.h"             /* for SrGetSnmpID  */
#include "snmpv3/v3type.h"      /* for snmpTargetAddrEntry_t */
#include "scan.h"
#include "diag.h"
#include "sr_ip.h"		/* for ConvToken_IP_TAddr */
#include "snmpv3/i_usm.h"       /* for usmUserEntryTypeTable */
#include "snmpv3/v3defs.h"      /* for usmUserTable constant macros */
#include "snmpv3/v3supp.h"      /* for usmUserTable constant macros */
#include "snmpv3/i_tgt.h"       /* for snmpTargetParamsEntryTypeTable */
#include "snmpv3/i_notif.h"     /* for snmpNotifyEntryTypeTable */
#include "coex/i_comm.h"        /* for snmpCommunityEntryTypeTable */
#include "coex/coextype.h"      /* for snmpCommunityEntry_t definition */
#include "coex/coexsupp.h"      /* for I_snmpCommunity* definition */
#include "coex/coexdefs.h"      /* for snmpCommunityTable constant macros */
#include "context.h"
#include "method.h"
#include "oid_lib.h"
#include "sr_msg.h"
#include "v3_msg.h"
#include "pw2key.h"

#include "l7_common.h"
#include "snmp_exports.h"
#include "user_manager_exports.h"
#include "sysapi.h"
#include "snmp_confapi.h"
#include "snmp_util_api.h"

extern snmpCfgData_t snmpCfgData;

extern SnmpV2Table agt_usmUserTable;                 /* usmUserEntry_t */

extern int writeConfigFileFlag;

/*******************************************************************************************/

/*
        OctetString     *usmUserEngineID;    (index)
        OctetString     *usmUserName;        (index)
        OctetString     *usmUserSecurityName;
        OID             *usmUserCloneFrom;
        OID             *usmUserAuthProtocol;
        OctetString     *usmUserAuthKeyChange;
        OctetString     *usmUserOwnAuthKeyChange;
        OID             *usmUserPrivProtocol;
        OctetString     *usmUserPrivKeyChange;
        OctetString     *usmUserOwnPrivKeyChange;
        OctetString     *usmUserPublic;
        SR_INT32        usmUserStorageType;
        SR_INT32        usmUserStatus;
        OctetString     *usmTargetTag;
*/

/*********************************************************************
*
* @purpose Retrieves the row associated with the index for the given request
*
* @returns Pointer to usmUserEntry_t object
*           
* @notes The table of users configured in the SNMP engine's
*        Local Configuration Datastore (LCD).
*        
*        To create a new user (i.e., to instantiate a new
*        conceptual row in this table), it is recommended to
*        follow this procedure:
*        
*          1)  GET(usmUserSpinLock.0) and save in sValue.
*          2)  SET(usmUserSpinLock.0=sValue,
*                  usmUserCloneFrom=templateUser,
*                  usmUserStatus=createAndWait)
*              You should use a template user to clone from
*              which has the proper auth/priv protocol defined.
*        
*        If the new user is to use privacy:
*        
*          3)  generate the keyChange value based on the secret
*              privKey of the clone-from user and the secret key
*              to be used for the new user. Let us call this
*              pkcValue.
*          4)  GET(usmUserSpinLock.0) and save in sValue.
*          5)  SET(usmUserSpinLock.0=sValue,
*                  usmUserPrivKeyChange=pkcValue
*                  usmUserPublic=randomValue1)
*          6)  GET(usmUserPulic) and check it has randomValue1.
*              If not, repeat steps 4-6.
*        
*        If the new user will never use privacy:
*        
*          7)  SET(usmUserPrivProtocol=usmNoPrivProtocol)
*        
*        If the new user is to use authentication:
*        
*          8)  generate the keyChange value based on the secret
*              authKey of the clone-from user and the secret key
*              to be used for the new user. Let us call this
*              akcValue.
*          9)  GET(usmUserSpinLock.0) and save in sValue.
*          10) SET(usmUserSpinLock.0=sValue,
*                  usmUserAuthKeyChange=akcValue
*                  usmUserPublic=randomValue2)
*          11) GET(usmUserPulic) and check it has randomValue2.
*              If not, repeat steps 9-11.
*        
*        If the new user will never use authentication:
*        
*          12) SET(usmUserAuthProtocol=usmNoAuthProtocol)
*        
*        Finally, activate the new user:
*        
*          13) SET(usmUserStatus=active)
*        
*        The new user should now be available and ready to be
*        used for SNMPv3 communication. Note however that access
*        to MIB data must be provided via configuration of the
*        SNMP-VIEW-BASED-ACM-MIB.
*        
*        The use of usmUserSpinlock is to avoid conflicts with
*        another SNMP command responder application which may
*        also be acting on the usmUserTable.
*
* @end
*
*********************************************************************/
static usmUserEntry_t* snmp_usmUserTable_get(int searchType, L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName)
{
  L7_int32 index = -1;
  usmUserEntry_t *row = NULL;

  /* check arguments */
  if ((searchType != EXACT && searchType != NEXT) || usmUserName == NULL)
    return NULL;

  /* build the search structure */
  if ((usmUserEngineID == NULL || usmUserEngineID_len == 0) && searchType == EXACT)
  {
    agt_usmUserTable.tip[0].value.octet_val = SrGetSnmpID(SR_SNMPID_ALGORITHM_SIMPLE_DEFAULT,NULL);
  }
  else
  {
    agt_usmUserTable.tip[0].value.octet_val = MakeOctetString(usmUserEngineID, usmUserEngineID_len);
  }
  if (agt_usmUserTable.tip[0].value.octet_val == NULL)
  {
    return NULL;
  }

  agt_usmUserTable.tip[1].value.octet_val = MakeOctetStringFromText(usmUserName);
  if (agt_usmUserTable.tip[1].value.octet_val == NULL)
  {
    FreeOctetString(agt_usmUserTable.tip[0].value.octet_val);
    return NULL;
  }

  index = SearchTable(&agt_usmUserTable, searchType);

  /* check to see if we need to search again */
  if (index >= 0)
  {
    row = (usmUserEntry_t*)agt_usmUserTable.tp[index];

    if (searchType == NEXT &&
        CmpOctetStrings(agt_usmUserTable.tip[0].value.octet_val, row->usmUserEngineID) == 0 &&
        CmpOctetStrings(agt_usmUserTable.tip[1].value.octet_val, row->usmUserName) == 0)
    {
      index = SearchTable(&agt_usmUserTable, NEXT_SKIP);
    }
  }


  FreeOctetString(agt_usmUserTable.tip[0].value.octet_val);
  FreeOctetString(agt_usmUserTable.tip[1].value.octet_val);

  if (index >= 0)
  {
    return(usmUserEntry_t*)agt_usmUserTable.tp[index];
  }

  return NULL;
}

/*********************************************************************
*
* @purpose Retrieves the next valid instance for the table
*
* @returns  NO_ERROR                     if defaults were set
*           RESOURCE_UNAVAILABLE_ERROR   if memory was not sufficient
*           
* @end
*
*********************************************************************/
static int l7_usmUserEntry_set_defaults(usmUserEntry_t *data)
{
  if ((data->usmUserSecurityName = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->usmUserCloneFrom = MakeOIDFromDot("0.0")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  /* usmNoAuthProtocol = 1.3.6.1.6.3.10.1.1.1 */
  /* usmHMACMD5AuthProtocol = 1.3.6.1.6.3.10.1.1.2 */
  if ((data->usmUserAuthProtocol = MakeOIDFromDot("1.3.6.1.6.3.10.1.1.1")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->usmUserAuthKeyChange = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->usmUserOwnAuthKeyChange = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  /* usmNoPrivProtocol = 1.3.6.1.6.3.10.1.2.1 */
  if ((data->usmUserPrivProtocol = MakeOIDFromDot("1.3.6.1.6.3.10.1.2.1")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->usmUserPrivKeyChange = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->usmUserOwnPrivKeyChange = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->usmUserPublic = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->usmUserStorageType = D_usmUserStorageType_nonVolatile;
  if ((data->usmTargetTag = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->RowStatusTimerId = -1;

#ifndef SR_UNSECURABLE
  data->auth_secret = NULL;
#ifndef SR_NO_PRIVACY
  data->priv_secret = NULL;
#ifdef SR_3DES
  data->privKeyChangeFlag = FALSE;
  data->key1 = NULL;
  data->key2 = NULL;
  data->key3 = NULL;
#endif /* SR_3DES */
#endif /* SR_NO_PRIVACY */
#endif /* SR_UNSECURABLE */

  SET_ALL_VALID(data->valid);
  CLR_VALID(I_usmUserAuthKeyChange, data->valid);
  CLR_VALID(I_usmUserOwnAuthKeyChange, data->valid);
  CLR_VALID(I_usmUserPrivKeyChange, data->valid);
  CLR_VALID(I_usmUserOwnPrivKeyChange, data->valid);
  CLR_VALID(I_usmUserCloneFrom, data->valid);

  return NO_ERROR;
}

/*********************************************************************
*
* @purpose Sets the row associated with the index for the given request
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*           
* @notes The table of users configured in the SNMP engine's
*        Local Configuration Datastore (LCD).
*        
*        To create a new user (i.e., to instantiate a new
*        conceptual row in this table), it is recommended to
*        follow this procedure:
*        
*          1)  GET(usmUserSpinLock.0) and save in sValue.
*          2)  SET(usmUserSpinLock.0=sValue,
*                  usmUserCloneFrom=templateUser,
*                  usmUserStatus=createAndWait)
*              You should use a template user to clone from
*              which has the proper auth/priv protocol defined.
*        
*        If the new user is to use privacy:
*        
*          3)  generate the keyChange value based on the secret
*              privKey of the clone-from user and the secret key
*              to be used for the new user. Let us call this
*              pkcValue.
*          4)  GET(usmUserSpinLock.0) and save in sValue.
*          5)  SET(usmUserSpinLock.0=sValue,
*                  usmUserPrivKeyChange=pkcValue
*                  usmUserPublic=randomValue1)
*          6)  GET(usmUserPulic) and check it has randomValue1.
*              If not, repeat steps 4-6.
*        
*        If the new user will never use privacy:
*        
*          7)  SET(usmUserPrivProtocol=usmNoPrivProtocol)
*        
*        If the new user is to use authentication:
*        
*          8)  generate the keyChange value based on the secret
*              authKey of the clone-from user and the secret key
*              to be used for the new user. Let us call this
*              akcValue.
*          9)  GET(usmUserSpinLock.0) and save in sValue.
*          10) SET(usmUserSpinLock.0=sValue,
*                  usmUserAuthKeyChange=akcValue
*                  usmUserPublic=randomValue2)
*          11) GET(usmUserPulic) and check it has randomValue2.
*              If not, repeat steps 9-11.
*        
*        If the new user will never use authentication:
*        
*          12) SET(usmUserAuthProtocol=usmNoAuthProtocol)
*        
*        Finally, activate the new user:
*        
*          13) SET(usmUserStatus=active)
*        
*        The new user should now be available and ready to be
*        used for SNMPv3 communication. Note however that access
*        to MIB data must be provided via configuration of the
*        SNMP-VIEW-BASED-ACM-MIB.
*        
*        The use of usmUserSpinlock is to avoid conflicts with
*        another SNMP command responder application which may
*        also be acting on the usmUserTable.
*
* @end
*
*********************************************************************/
static L7_RC_t snmp_usmUserTable_set(usmUserEntry_t *data)
{
  L7_int32 index = -1;
  usmUserEntry_t * usmUserEntryData = NULL;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 authProt;
  L7_char8 temp_buf[256];

  /* check arguments */
  if (data == NULL)
    return L7_ERROR;

  if (!VALID(I_usmUserName, data->valid))
    return L7_ERROR;

  /* build the search structure */
  /* build the search structure */
  if (!VALID(I_usmUserEngineID, data->valid) ||
      data->usmUserEngineID->length == 0)
  {
    agt_usmUserTable.tip[0].value.octet_val = SrGetSnmpID(SR_SNMPID_ALGORITHM_SIMPLE_DEFAULT,NULL);
  }
  else
  {
    agt_usmUserTable.tip[0].value.octet_val = CloneOctetString(data->usmUserEngineID);
  }
  if (agt_usmUserTable.tip[0].value.octet_val == NULL)
  {
    return L7_ERROR;
  }
  agt_usmUserTable.tip[1].value.octet_val = CloneOctetString(data->usmUserName);
  if (agt_usmUserTable.tip[1].value.octet_val == NULL)
  {
    FreeOctetString(agt_usmUserTable.tip[0].value.octet_val);
    return L7_ERROR;
  }

  index = SearchTable(&agt_usmUserTable, EXACT);

  if (index < 0)
  {
    if (VALID(I_usmUserStatus, data->valid) && 
        (data->usmUserStatus == D_usmUserStatus_createAndGo ||
         data->usmUserStatus == D_usmUserStatus_createAndWait))
    {
      index = NewTableEntry(&agt_usmUserTable);

      writeConfigFileFlag = TRUE;

      if (index < 0 ||
          l7_usmUserEntry_set_defaults((usmUserEntry_t*)agt_usmUserTable.tp[index]) != NO_ERROR)
        return L7_ERROR;

      if (data->usmUserStatus == D_usmUserStatus_createAndGo)
      {
        ((usmUserEntry_t*)agt_usmUserTable.tp[index])->usmUserStatus = D_usmUserStatus_active;
      }
      else
      {
        ((usmUserEntry_t*)agt_usmUserTable.tp[index])->usmUserStatus = D_usmUserStatus_notInService;
      }
      SET_VALID(I_usmUserStatus, ((usmUserEntry_t*)agt_usmUserTable.tp[index])->valid);

      CLR_VALID(I_usmUserStatus, data->valid);
    }
  }

  FreeOctetString(agt_usmUserTable.tip[0].value.octet_val);
  FreeOctetString(agt_usmUserTable.tip[1].value.octet_val);

  if (index >= 0)
  {
    if (VALID(I_usmUserStatus, data->valid) && 
        data->usmUserStatus == D_usmUserStatus_destroy)
    {
      RemoveTableEntry(&agt_usmUserTable, index);
      return L7_SUCCESS;
    }

    usmUserEntryData = (usmUserEntry_t*)agt_usmUserTable.tp[index];

    if (VALID(I_usmUserSecurityName, data->valid))
    {
      if (usmUserEntryData->usmUserSecurityName != NULL)
      {
        FreeOctetString(usmUserEntryData->usmUserSecurityName);
        CLR_VALID(I_usmUserSecurityName, usmUserEntryData->valid);
      }

      usmUserEntryData->usmUserSecurityName = CloneOctetString(data->usmUserSecurityName);

      if (usmUserEntryData->usmUserSecurityName != NULL)
      {
        SET_VALID(I_usmUserSecurityName, usmUserEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_usmUserAuthProtocol, data->valid))
    {
      FreeOID(usmUserEntryData->usmUserAuthProtocol);
      CLR_VALID(I_usmUserAuthProtocol, usmUserEntryData->valid);

      /* check that the Privacy protocol isn't already set.  This must be either not set, 
         or set to none if the Authentication protocol is to be set to none */
      if (VALID(I_usmUserPrivProtocol, usmUserEntryData->valid) &&
          usmUserEntryData->usmUserPrivProtocol != NULL &&
          CmpOID(usmUserEntryData->usmUserPrivProtocol, &usmNoPrivProtocol) != 0 &&
          data->usmUserAuthProtocol != NULL &&
          CmpOID(data->usmUserAuthProtocol, &usmNoAuthProtocol) != 0)
      {
        return L7_ERROR;
      }

      usmUserEntryData->usmUserAuthProtocol = CloneOID(data->usmUserAuthProtocol);
      FreeOctetString(usmUserEntryData->auth_secret);
      usmUserEntryData->auth_secret = NULL;
      /* clone auth_secret if supplied */
      if (data->auth_secret != NULL)
      {
        usmUserEntryData->auth_secret = CloneOctetString(data->auth_secret);
      }

      if (usmUserEntryData->usmUserAuthProtocol != NULL)
      {
        SET_VALID(I_usmUserAuthProtocol, usmUserEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
    }

    if (VALID(I_usmUserAuthKeyChange, data->valid))
    {

      authProt = SrV3ProtocolOIDToInt(usmUserEntryData->usmUserAuthProtocol);

      if (usmUserEntryData->auth_secret != NULL)
      {
        FreeOctetString(usmUserEntryData->auth_secret);
        usmUserEntryData->auth_secret = NULL;
      }

      switch (authProt)
      {
      case SR_USM_NOAUTH_PROTOCOL:
        return L7_ERROR;
        break;
#ifndef SR_UNSECURABLE
      case SR_USM_HMAC_MD5_AUTH_PROTOCOL:
        memcpy(temp_buf, data->usmUserAuthKeyChange->octet_ptr, data->usmUserAuthKeyChange->length);
        temp_buf[data->usmUserAuthKeyChange->length] = 0;
        usmUserEntryData->auth_secret = SrOctetStringPasswordToLocalizedKey(SR_PW2KEY_ALGORITHM_MD5,
                                                                            usmUserEntryData->usmUserEngineID, data->usmUserAuthKeyChange);
/*        sysapiPrintf("MD5 AuthKey:");
        debug_snmp_printHexString(data->usmUserAuthKeyChange->octet_ptr, data->usmUserAuthKeyChange->length);
        sysapiPrintf("\n");
        debug_snmp_printHexString(usmUserEntryData->auth_secret->octet_ptr, usmUserEntryData->auth_secret->length);
        sysapiPrintf("\n");*/
        break;
#ifdef SR_SHA_HASH_ALGORITHM
      case SR_USM_HMAC_SHA_AUTH_PROTOCOL:
        memcpy(temp_buf, data->usmUserAuthKeyChange->octet_ptr, data->usmUserAuthKeyChange->length);
        temp_buf[data->usmUserAuthKeyChange->length] = 0;
        usmUserEntryData->auth_secret = SrOctetStringPasswordToLocalizedKey(SR_PW2KEY_ALGORITHM_SHA,
                                                                            usmUserEntryData->usmUserEngineID, 
                                                                            data->usmUserAuthKeyChange);
/*        sysapiPrintf("SHA AuthKey:");
        debug_snmp_printHexString(data->usmUserAuthKeyChange->octet_ptr, data->usmUserAuthKeyChange->length);
        sysapiPrintf("\n");
        debug_snmp_printHexString(usmUserEntryData->auth_secret->octet_ptr, usmUserEntryData->auth_secret->length);
        sysapiPrintf("\n");*/
        break;
#endif
#endif
      default:
        return L7_ERROR;
        break;
      }

      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_usmUserPrivProtocol, data->valid))
    {
      FreeOID(usmUserEntryData->usmUserPrivProtocol);
      CLR_VALID(I_usmUserPrivProtocol, usmUserEntryData->valid);

      /* check that the Authentication protocol is already set.  This must be set to anything
         other than none if the privacy protocol is to be set to anything other than none */
      if (VALID(I_usmUserAuthProtocol, usmUserEntryData->valid) &&
          usmUserEntryData->usmUserAuthProtocol != NULL &&
          CmpOID(usmUserEntryData->usmUserAuthProtocol, &usmNoAuthProtocol) == 0 &&
          data->usmUserPrivProtocol != NULL &&
          CmpOID(data->usmUserPrivProtocol, &usmNoPrivProtocol) != 0)
      {
        return L7_ERROR;
      }

      usmUserEntryData->usmUserPrivProtocol = CloneOID(data->usmUserPrivProtocol);
      FreeOctetString(usmUserEntryData->priv_secret);
      usmUserEntryData->priv_secret = NULL;
      /* clone priv_secret if supplied */
      if (data->priv_secret != NULL)
      {
        usmUserEntryData->priv_secret = CloneOctetString(data->priv_secret);
      }

      if (usmUserEntryData->usmUserPrivProtocol != NULL)
      {
        SET_VALID(I_usmUserPrivProtocol, usmUserEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
    }

    if (VALID(I_usmUserPrivKeyChange, data->valid))
    {
      authProt = SrV3ProtocolOIDToInt(usmUserEntryData->usmUserAuthProtocol);

      if (usmUserEntryData->priv_secret != NULL)
        FreeOctetString(usmUserEntryData->priv_secret);

      switch (authProt)
      {
      case SR_USM_NOAUTH_PROTOCOL:
        return L7_ERROR;
        break;
#ifndef SR_UNSECURABLE
      case SR_USM_HMAC_MD5_AUTH_PROTOCOL:
        memcpy(temp_buf, data->usmUserPrivKeyChange->octet_ptr, data->usmUserPrivKeyChange->length);
        temp_buf[data->usmUserPrivKeyChange->length] = 0;
        usmUserEntryData->priv_secret = SrOctetStringPasswordToLocalizedKey(SR_PW2KEY_ALGORITHM_MD5,
                                                                            usmUserEntryData->usmUserEngineID, data->usmUserPrivKeyChange);
/*        sysapiPrintf("MD5 PrivKey:");
        debug_snmp_printHexString(data->usmUserPrivKeyChange->octet_ptr, data->usmUserPrivKeyChange->length);
        sysapiPrintf("\n");
        debug_snmp_printHexString(usmUserEntryData->priv_secret->octet_ptr, usmUserEntryData->priv_secret->length);
        sysapiPrintf("\n");*/
        break;
#ifdef SR_SHA_HASH_ALGORITHM
      case SR_USM_HMAC_SHA_AUTH_PROTOCOL:
        memcpy(temp_buf, data->usmUserPrivKeyChange->octet_ptr, data->usmUserPrivKeyChange->length);
        temp_buf[data->usmUserPrivKeyChange->length] = 0;
        usmUserEntryData->priv_secret = SrOctetStringPasswordToLocalizedKey(SR_PW2KEY_ALGORITHM_SHA,
                                                                            usmUserEntryData->usmUserEngineID, data->usmUserPrivKeyChange);
/*        sysapiPrintf("SHA PrivKey:");
        debug_snmp_printHexString(data->usmUserPrivKeyChange->octet_ptr, data->usmUserPrivKeyChange->length);
        sysapiPrintf("\n");
        debug_snmp_printHexString(usmUserEntryData->priv_secret->octet_ptr, usmUserEntryData->priv_secret->length);
        sysapiPrintf("\n");*/
        break;
#endif
#endif
      default:
        return L7_ERROR;
        break;
      }

      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_usmUserStorageType, data->valid))
    {
      if (l7_snmpStorageTypeSetValidate(usmUserEntryData->usmUserStorageType,
                                        data->usmUserStorageType) != L7_SUCCESS)
        return L7_ERROR;

      usmUserEntryData->usmUserStorageType = data->usmUserStorageType;
      SET_VALID(I_usmUserStorageType, usmUserEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_usmUserStatus, data->valid))
    {
      if (l7_snmpRowStatusSetValidate(usmUserEntryData->usmUserStatus,
                                      data->usmUserStatus) != L7_SUCCESS)
        return L7_ERROR;

      usmUserEntryData->usmUserStatus = data->usmUserStatus;
      SET_VALID(I_usmUserStatus, usmUserEntryData->valid);
      writeConfigFileFlag = TRUE;
    }
  }
  else
  {
    return L7_ERROR;
  }

  return rc;
}

/*********************************************************************
*
* @purpose Checks that the row exists for the given instances
*
* @returns  L7_SUCCESS    if the row exists
*           L7_FAILURE    if no row exists
*           
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserTableCheckValid(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName)
{
  usmUserEntry_t * usmUserEntryData = snmp_usmUserTable_get(EXACT, usmUserEngineID, usmUserEngineID_len, usmUserName);

  if (usmUserEntryData != NULL)
    return L7_SUCCESS;
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Retrieves the next valid instance for the table
*
* @returns  L7_SUCCESS    if annother row exists
*           L7_FAILURE    if no more rows exist
*           
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserTableNextGet(L7_char8 *usmUserEngineID, L7_uint32 *usmUserEngineID_len, L7_char8 *usmUserName)
{
  usmUserEntry_t * usmUserEntryData = snmp_usmUserTable_get(NEXT, usmUserEngineID, *usmUserEngineID_len, usmUserName);

  if (usmUserEntryData != NULL)
  {
    memcpy(usmUserEngineID, usmUserEntryData->usmUserEngineID->octet_ptr,
            usmUserEntryData->usmUserEngineID->length);
    *usmUserEngineID_len = usmUserEntryData->usmUserEngineID->length;
    strncpy(usmUserName, usmUserEntryData->usmUserName->octet_ptr,
            usmUserEntryData->usmUserName->length);
    usmUserName[usmUserEntryData->usmUserName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserSecurityName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes A human readable string representing the user in
*        Security Model independent format.
*        
*        The default transformation of the User-based Security
*        Model dependent security ID to the securityName and
*        vice versa is the identity function so that the
*        securityName is the same as the userName.
*
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserSecurityNameGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, L7_char8 *val)
{
  usmUserEntry_t * usmUserEntryData = snmp_usmUserTable_get(EXACT, usmUserEngineID, usmUserEngineID_len, usmUserName);

  if (usmUserEntryData != NULL && 
      VALID(I_usmUserSecurityName, usmUserEntryData->valid))
  {
    strncpy(val, usmUserEntryData->usmUserSecurityName->octet_ptr,
            usmUserEntryData->usmUserSecurityName->length);
    val[usmUserEntryData->usmUserSecurityName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserSecurityName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes A human readable string representing the user in
*        Security Model independent format.
*        
*        The default transformation of the User-based Security
*        Model dependent security ID to the securityName and
*        vice versa is the identity function so that the
*        securityName is the same as the userName.
*
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserSecurityNameSet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, L7_char8 *val)
{
  usmUserEntry_t usmUserEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 1, 32) != L7_SUCCESS)
    return rc;

  memset((void*)&usmUserEntryData, 0, sizeof(usmUserEntryData));

  usmUserEntryData.usmUserEngineID = MakeOctetString(usmUserEngineID, usmUserEngineID_len);
  SET_VALID(I_usmUserEngineID, usmUserEntryData.valid);

  usmUserEntryData.usmUserName = MakeOctetStringFromText(usmUserName);
  SET_VALID(I_usmUserName, usmUserEntryData.valid);

  usmUserEntryData.usmUserSecurityName = MakeOctetStringFromText(val);
  SET_VALID(I_usmUserSecurityName, usmUserEntryData.valid);

  rc = snmp_usmUserTable_set(&usmUserEntryData);

  FreeOctetString(usmUserEntryData.usmUserEngineID);
  FreeOctetString(usmUserEntryData.usmUserName);
  FreeOctetString(usmUserEntryData.usmUserSecurityName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserAuthProtocol
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be authenticated, and if so,
*        the type of authentication protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoAuthProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoAuthProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        If a set operation tries to set the value to the
*        usmNoAuthProtocol while the usmUserPrivProtocol value
*        in the same row is not equal to usmNoPrivProtocol,
*        then an 'inconsistentValue' error must be returned.
*        That means that an SNMP command generator application
*        must first ensure that the usmUserPrivProtocol is set
*        to the usmNoPrivProtocol value before it can set
*        the usmUserAuthProtocol value to usmNoAuthProtocol.
*
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserAuthProtocolGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserAuthProtocol_t *val)
{
  L7_uint32 protocol;
  usmUserEntry_t * usmUserEntryData = snmp_usmUserTable_get(EXACT, usmUserEngineID, usmUserEngineID_len, usmUserName);

  if (usmUserEntryData != NULL && 
      VALID(I_usmUserAuthProtocol, usmUserEntryData->valid))
  {
    protocol = SrV3ProtocolOIDToInt(usmUserEntryData->usmUserAuthProtocol);

    switch (protocol)
    {
    case SR_USM_NOAUTH_PROTOCOL:
      *val = usmUserAuthProtocol_none;
      break;
#ifndef SR_UNSECURABLE
    case SR_USM_HMAC_MD5_AUTH_PROTOCOL:
      *val = usmUserAuthProtocol_md5;
      break;
#ifdef SR_SHA_HASH_ALGORITHM
    case SR_USM_HMAC_SHA_AUTH_PROTOCOL:
      *val = usmUserAuthProtocol_sha;
      break;
#endif
#endif
    default:
      return L7_FAILURE;
      break;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t l7_usmUserAuthKeyGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, L7_char8 *key, L7_uint32 *key_len)
{
  usmUserEntry_t * usmUserEntryData = snmp_usmUserTable_get(EXACT, usmUserEngineID, usmUserEngineID_len, usmUserName);

  if (usmUserEntryData != NULL &&
      usmUserEntryData->auth_secret != NULL)
  {
    memcpy(key, usmUserEntryData->auth_secret->octet_ptr, usmUserEntryData->auth_secret->length);
    *key_len = usmUserEntryData->auth_secret->length;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Sets the value of the object usmUserAuthProtocol
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be authenticated, and if so,
*        the type of authentication protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoAuthProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoAuthProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        If a set operation tries to set the value to the
*        usmNoAuthProtocol while the usmUserPrivProtocol value
*        in the same row is not equal to usmNoPrivProtocol,
*        then an 'inconsistentValue' error must be returned.
*        That means that an SNMP command generator application
*        must first ensure that the usmUserPrivProtocol is set
*        to the usmNoPrivProtocol value before it can set
*        the usmUserAuthProtocol value to usmNoAuthProtocol.
*
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserAuthProtocolSet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserAuthProtocol_t val, L7_char8 *pass)
{
  usmUserEntry_t usmUserEntryData;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 protocol;

  memset((void*)&usmUserEntryData, 0, sizeof(usmUserEntryData));

  usmUserEntryData.usmUserEngineID = MakeOctetString(usmUserEngineID, usmUserEngineID_len);
  SET_VALID(I_usmUserEngineID, usmUserEntryData.valid);

  usmUserEntryData.usmUserName = MakeOctetStringFromText(usmUserName);
  SET_VALID(I_usmUserName, usmUserEntryData.valid);

  switch (val)
  {
  case usmUserAuthProtocol_none:
    protocol = SR_USM_NOAUTH_PROTOCOL;
    break;
#ifndef SR_UNSECURABLE
  case usmUserAuthProtocol_md5:
    protocol = SR_USM_HMAC_MD5_AUTH_PROTOCOL;
    break;
#ifdef SR_SHA_HASH_ALGORITHM
  case usmUserAuthProtocol_sha:
    protocol = SR_USM_HMAC_SHA_AUTH_PROTOCOL;
    break;
#endif
#endif
  default:
    return L7_FAILURE;
    break;
  }

  usmUserEntryData.usmUserAuthProtocol = CloneOID(SrV3AuthProtocolIntToOID(protocol));
  SET_VALID(I_usmUserAuthProtocol, usmUserEntryData.valid);

  usmUserEntryData.usmUserAuthKeyChange = MakeOctetStringFromText(pass);
  SET_VALID(I_usmUserAuthKeyChange, usmUserEntryData.valid);

  rc = snmp_usmUserTable_set(&usmUserEntryData);

  FreeOctetString(usmUserEntryData.usmUserEngineID);
  FreeOctetString(usmUserEntryData.usmUserName);
  FreeOID(usmUserEntryData.usmUserAuthProtocol);
  FreeOctetString(usmUserEntryData.usmUserAuthKeyChange);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserAuthProtocol
*          plus generated key
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be authenticated, and if so,
*        the type of authentication protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoAuthProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoAuthProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        If a set operation tries to set the value to the
*        usmNoAuthProtocol while the usmUserPrivProtocol value
*        in the same row is not equal to usmNoPrivProtocol,
*        then an 'inconsistentValue' error must be returned.
*        That means that an SNMP command generator application
*        must first ensure that the usmUserPrivProtocol is set
*        to the usmNoPrivProtocol value before it can set
*        the usmUserAuthProtocol value to usmNoAuthProtocol.
*
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserAuthProtocolKeyGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserAuthProtocol_t *val, L7_uchar8 *key, L7_uint32 *key_len)
{
  L7_uint32 protocol;
  usmUserEntry_t * usmUserEntryData = snmp_usmUserTable_get(EXACT, usmUserEngineID, usmUserEngineID_len, usmUserName);

  if (usmUserEntryData != NULL)
  {
    if (VALID(I_usmUserAuthProtocol, usmUserEntryData->valid))
    {
      protocol = SrV3ProtocolOIDToInt(usmUserEntryData->usmUserAuthProtocol);
  
      switch (protocol)
      {
      case SR_USM_NOAUTH_PROTOCOL:
        *val = usmUserAuthProtocol_none;
        break;
#ifndef SR_UNSECURABLE
      case SR_USM_HMAC_MD5_AUTH_PROTOCOL:
        *val = usmUserAuthProtocol_md5;
        break;
#ifdef SR_SHA_HASH_ALGORITHM
      case SR_USM_HMAC_SHA_AUTH_PROTOCOL:
        *val = usmUserAuthProtocol_sha;
        break;
#endif
#endif
      default:
        return L7_FAILURE;
        break;
      }
    }
    else
    {
      return L7_FAILURE;
    }

    if (usmUserEntryData->auth_secret != NULL)
    {
      memcpy(key, usmUserEntryData->auth_secret->octet_ptr, usmUserEntryData->auth_secret->length);
      *key_len = usmUserEntryData->auth_secret->length;
    }
    else
    {
      key[0] = 0;
      *key_len = 0;
    }

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Sets the value of the object usmUserAuthProtocol
*          plus pregenerated key
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be authenticated, and if so,
*        the type of authentication protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoAuthProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoAuthProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        If a set operation tries to set the value to the
*        usmNoAuthProtocol while the usmUserPrivProtocol value
*        in the same row is not equal to usmNoPrivProtocol,
*        then an 'inconsistentValue' error must be returned.
*        That means that an SNMP command generator application
*        must first ensure that the usmUserPrivProtocol is set
*        to the usmNoPrivProtocol value before it can set
*        the usmUserAuthProtocol value to usmNoAuthProtocol.
*
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserAuthProtocolKeySet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserAuthProtocol_t val, L7_char8 *key, L7_uint32 key_len)
{
  usmUserEntry_t usmUserEntryData;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 protocol;

  memset((void*)&usmUserEntryData, 0, sizeof(usmUserEntryData));

  usmUserEntryData.usmUserEngineID = MakeOctetString(usmUserEngineID, usmUserEngineID_len);
  SET_VALID(I_usmUserEngineID, usmUserEntryData.valid);

  usmUserEntryData.usmUserName = MakeOctetStringFromText(usmUserName);
  SET_VALID(I_usmUserName, usmUserEntryData.valid);

  switch (val)
  {
  case usmUserAuthProtocol_none:
    protocol = SR_USM_NOAUTH_PROTOCOL;
    break;
#ifndef SR_UNSECURABLE
  case usmUserAuthProtocol_md5:
    protocol = SR_USM_HMAC_MD5_AUTH_PROTOCOL;
    break;
#ifdef SR_SHA_HASH_ALGORITHM
  case usmUserAuthProtocol_sha:
    protocol = SR_USM_HMAC_SHA_AUTH_PROTOCOL;
    break;
#endif
#endif
  default:
    return L7_FAILURE;
    break;
  }

  usmUserEntryData.usmUserAuthProtocol = CloneOID(SrV3AuthProtocolIntToOID(protocol));
  SET_VALID(I_usmUserAuthProtocol, usmUserEntryData.valid);

  usmUserEntryData.auth_secret = MakeOctetString(key, key_len);

  rc = snmp_usmUserTable_set(&usmUserEntryData);

  FreeOctetString(usmUserEntryData.usmUserEngineID);
  FreeOctetString(usmUserEntryData.usmUserName);
  FreeOID(usmUserEntryData.usmUserAuthProtocol);
  FreeOctetString(usmUserEntryData.auth_secret);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserPrivProtocol
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be protected from disclosure,
*        and if so, the type of privacy protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoPrivProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoPrivProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        Note that if any privacy protocol is used, then you
*        must also use an authentication protocol. In other
*        words, if usmUserPrivProtocol is set to anything else
*        than usmNoPrivProtocol, then the corresponding instance
*        of usmUserAuthProtocol cannot have a value of
*        usmNoAuthProtocol. If it does, then an
*
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserPrivProtocolGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserPrivProtocol_t *val)
{
  L7_uint32 protocol;
  usmUserEntry_t * usmUserEntryData = snmp_usmUserTable_get(EXACT, usmUserEngineID, usmUserEngineID_len, usmUserName);

  if (usmUserEntryData != NULL && 
      VALID(I_usmUserPrivProtocol, usmUserEntryData->valid))
  {
    protocol = SrV3ProtocolOIDToInt(usmUserEntryData->usmUserPrivProtocol);

    switch (protocol)
    {
    case SR_USM_NOPRIV_PROTOCOL:
      *val = usmUserPrivProtocol_none;
      break;
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
    case SR_USM_DES_PRIV_PROTOCOL:
      *val = usmUserPrivProtocol_des;
      break;
#ifdef SR_3DES
    case SR_USM_3DES_PRIV_PROTOCOL:
      *val = usmUserPrivProtocol_3des;
      break;
#endif
#ifdef SR_AES
    case SR_USM_AES_CFB_128_PRIV_PROTOCOL:
      *val = usmUserPrivProtocol_aes128;
      break;
    case SR_USM_AES_CFB_192_PRIV_PROTOCOL:
      *val = usmUserPrivProtocol_aes192;
      break;
    case SR_USM_AES_CFB_256_PRIV_PROTOCOL:
      *val = usmUserPrivProtocol_aes256;
      break;
#endif
#endif
#endif
    default:
      return L7_FAILURE;
      break;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t l7_usmUserPrivKeyGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, L7_char8 *key, L7_uint32 *key_len)
{
  usmUserEntry_t * usmUserEntryData = snmp_usmUserTable_get(EXACT, usmUserEngineID, usmUserEngineID_len, usmUserName);

  if (usmUserEntryData != NULL &&
      usmUserEntryData->priv_secret != NULL)
  {
    memcpy(key, usmUserEntryData->priv_secret->octet_ptr, usmUserEntryData->priv_secret->length);
    *key_len = usmUserEntryData->priv_secret->length;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Sets the value of the object usmUserPrivProtocol
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be protected from disclosure,
*        and if so, the type of privacy protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoPrivProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoPrivProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        Note that if any privacy protocol is used, then you
*        must also use an authentication protocol. In other
*        words, if usmUserPrivProtocol is set to anything else
*        than usmNoPrivProtocol, then the corresponding instance
*        of usmUserAuthProtocol cannot have a value of
*        usmNoAuthProtocol. If it does, then an
*
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserPrivProtocolSet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserPrivProtocol_t val, L7_char8 *pass)
{
  usmUserEntry_t usmUserEntryData;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 protocol;

  memset((void*)&usmUserEntryData, 0, sizeof(usmUserEntryData));

  usmUserEntryData.usmUserEngineID = MakeOctetString(usmUserEngineID, usmUserEngineID_len);
  SET_VALID(I_usmUserEngineID, usmUserEntryData.valid);

  usmUserEntryData.usmUserName = MakeOctetStringFromText(usmUserName);
  SET_VALID(I_usmUserName, usmUserEntryData.valid);

  switch (val)
  {
  case usmUserPrivProtocol_none:
    protocol = SR_USM_NOPRIV_PROTOCOL;
    break;
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
  case usmUserPrivProtocol_des:
    protocol = SR_USM_DES_PRIV_PROTOCOL;
    break;
#ifdef SR_3DES
  case usmUserPrivProtocol_3des:
    protocol = SR_USM_3DES_PRIV_PROTOCOL;
    break;
#endif
#ifdef SR_AES
  case usmUserPrivProtocol_aes128:
    protocol = SR_USM_AES_CFB_128_PRIV_PROTOCOL;
    break;
  case usmUserPrivProtocol_aes192:
    protocol = SR_USM_AES_CFB_192_PRIV_PROTOCOL;
    break;
  case usmUserPrivProtocol_aes256:
    protocol = SR_USM_AES_CFB_256_PRIV_PROTOCOL;
    break;
#endif
#endif
#endif
  default:
    return L7_FAILURE;
    break;
  }

  usmUserEntryData.usmUserPrivProtocol = CloneOID(SrV3PrivProtocolIntToOID(protocol));
  SET_VALID(I_usmUserPrivProtocol, usmUserEntryData.valid);

  usmUserEntryData.usmUserPrivKeyChange = MakeOctetStringFromText(pass);
  SET_VALID(I_usmUserPrivKeyChange, usmUserEntryData.valid);

  rc = snmp_usmUserTable_set(&usmUserEntryData);

  FreeOctetString(usmUserEntryData.usmUserEngineID);
  FreeOctetString(usmUserEntryData.usmUserName);
  FreeOID(usmUserEntryData.usmUserPrivProtocol);
  FreeOctetString(usmUserEntryData.usmUserPrivKeyChange);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserPrivProtocol
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be protected from disclosure,
*        and if so, the type of privacy protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoPrivProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoPrivProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        Note that if any privacy protocol is used, then you
*        must also use an authentication protocol. In other
*        words, if usmUserPrivProtocol is set to anything else
*        than usmNoPrivProtocol, then the corresponding instance
*        of usmUserAuthProtocol cannot have a value of
*        usmNoAuthProtocol. If it does, then an
*
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserPrivProtocolKeyGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserPrivProtocol_t *val, L7_uchar8 *key, L7_uint32 *key_len)
{
  L7_uint32 protocol;
  usmUserEntry_t * usmUserEntryData = snmp_usmUserTable_get(EXACT, usmUserEngineID, usmUserEngineID_len, usmUserName);

  if (usmUserEntryData != NULL)
  {
    if (VALID(I_usmUserPrivProtocol, usmUserEntryData->valid))
    {
      protocol = SrV3ProtocolOIDToInt(usmUserEntryData->usmUserPrivProtocol);
  
      switch (protocol)
      {
      case SR_USM_NOPRIV_PROTOCOL:
        *val = usmUserPrivProtocol_none;
        break;
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
      case SR_USM_DES_PRIV_PROTOCOL:
        *val = usmUserPrivProtocol_des;
        break;
#ifdef SR_3DES
      case SR_USM_3DES_PRIV_PROTOCOL:
        *val = usmUserPrivProtocol_3des;
        break;
#endif
#ifdef SR_AES
      case SR_USM_AES_CFB_128_PRIV_PROTOCOL:
        *val = usmUserPrivProtocol_aes128;
        break;
      case SR_USM_AES_CFB_192_PRIV_PROTOCOL:
        *val = usmUserPrivProtocol_aes192;
        break;
      case SR_USM_AES_CFB_256_PRIV_PROTOCOL:
        *val = usmUserPrivProtocol_aes256;
        break;
#endif
#endif
#endif
      default:
        return L7_FAILURE;
        break;
      }
    }
    else
    {
      return L7_FAILURE;
    }

    if (usmUserEntryData->priv_secret != NULL)
    {
      memcpy(key, usmUserEntryData->priv_secret->octet_ptr, usmUserEntryData->priv_secret->length);
      *key_len = usmUserEntryData->priv_secret->length;
    }
    else
    {
      key[0] = 0;
      *key_len = 0;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserPrivProtocol
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be protected from disclosure,
*        and if so, the type of privacy protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoPrivProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoPrivProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        Note that if any privacy protocol is used, then you
*        must also use an authentication protocol. In other
*        words, if usmUserPrivProtocol is set to anything else
*        than usmNoPrivProtocol, then the corresponding instance
*        of usmUserAuthProtocol cannot have a value of
*        usmNoAuthProtocol. If it does, then an
*
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserPrivProtocolKeySet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserPrivProtocol_t val, L7_char8 *key, L7_uint32 key_len)
{
  usmUserEntry_t usmUserEntryData;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 protocol;

  memset((void*)&usmUserEntryData, 0, sizeof(usmUserEntryData));

  usmUserEntryData.usmUserEngineID = MakeOctetString(usmUserEngineID, usmUserEngineID_len);
  SET_VALID(I_usmUserEngineID, usmUserEntryData.valid);

  usmUserEntryData.usmUserName = MakeOctetStringFromText(usmUserName);
  SET_VALID(I_usmUserName, usmUserEntryData.valid);

  switch (val)
  {
  case usmUserPrivProtocol_none:
    protocol = SR_USM_NOPRIV_PROTOCOL;
    break;
#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
  case usmUserPrivProtocol_des:
    protocol = SR_USM_DES_PRIV_PROTOCOL;
    break;
#ifdef SR_3DES
  case usmUserPrivProtocol_3des:
    protocol = SR_USM_3DES_PRIV_PROTOCOL;
    break;
#endif
#ifdef SR_AES
  case usmUserPrivProtocol_aes128:
    protocol = SR_USM_AES_CFB_128_PRIV_PROTOCOL;
    break;
  case usmUserPrivProtocol_aes192:
    protocol = SR_USM_AES_CFB_192_PRIV_PROTOCOL;
    break;
  case usmUserPrivProtocol_aes256:
    protocol = SR_USM_AES_CFB_256_PRIV_PROTOCOL;
    break;
#endif
#endif
#endif
  default:
    return L7_FAILURE;
    break;
  }

  usmUserEntryData.usmUserPrivProtocol = CloneOID(SrV3PrivProtocolIntToOID(protocol));
  SET_VALID(I_usmUserPrivProtocol, usmUserEntryData.valid);

  usmUserEntryData.priv_secret = MakeOctetString(key, key_len);

  rc = snmp_usmUserTable_set(&usmUserEntryData);

  FreeOctetString(usmUserEntryData.usmUserEngineID);
  FreeOctetString(usmUserEntryData.usmUserName);
  FreeOID(usmUserEntryData.usmUserPrivProtocol);
  FreeOctetString(usmUserEntryData.priv_secret);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row.
*        
*        Conceptual rows having the value 'permanent' must
*        allow write-access at a minimum to:
*        
*          and usmUserPublic for a user who employs
*          authentication, and
*          and usmUserPublic for a user who employs
*          privacy.
*        
*        Note that any user who employs authentication or
*        privacy must allow its secret(s) to be updated and
*        thus cannot be 'readOnly'.
*        
*        If an initial set operation tries to set the value to
*        privacy, then an 'inconsistentValue' error must be
*        returned.  Note that if the value has been previously
*        set (implicit or explicit) to any value, then the rules
*        as defined in the StorageType Textual Convention apply.
*        
*        It is an implementation issue to decide if a SET for
*        a readOnly or permanent row is accepted at all. In some
*        contexts this may make sense, in others it may not. If
*        a SET for a readOnly or permanent row is not accepted
*        at all, then a 'wrongValue' error must be returned.
*
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserStorageTypeGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_snmpStorageType_t *val)
{
  usmUserEntry_t * usmUserEntryData = snmp_usmUserTable_get(EXACT, usmUserEngineID, usmUserEngineID_len, usmUserName);

  if (usmUserEntryData != NULL && 
      VALID(I_usmUserStorageType, usmUserEntryData->valid))
  {
    *val = usmUserEntryData->usmUserStorageType;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row.
*        
*        Conceptual rows having the value 'permanent' must
*        allow write-access at a minimum to:
*        
*          and usmUserPublic for a user who employs
*          authentication, and
*          and usmUserPublic for a user who employs
*          privacy.
*        
*        Note that any user who employs authentication or
*        privacy must allow its secret(s) to be updated and
*        thus cannot be 'readOnly'.
*        
*        If an initial set operation tries to set the value to
*        privacy, then an 'inconsistentValue' error must be
*        returned.  Note that if the value has been previously
*        set (implicit or explicit) to any value, then the rules
*        as defined in the StorageType Textual Convention apply.
*        
*        It is an implementation issue to decide if a SET for
*        a readOnly or permanent row is accepted at all. In some
*        contexts this may make sense, in others it may not. If
*        a SET for a readOnly or permanent row is not accepted
*        at all, then a 'wrongValue' error must be returned.
*
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserStorageTypeSet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_snmpStorageType_t val)
{
  usmUserEntry_t usmUserEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&usmUserEntryData, 0, sizeof(usmUserEntryData));

  usmUserEntryData.usmUserEngineID = MakeOctetString(usmUserEngineID, usmUserEngineID_len);
  SET_VALID(I_usmUserEngineID, usmUserEntryData.valid);

  usmUserEntryData.usmUserName = MakeOctetStringFromText(usmUserName);
  SET_VALID(I_usmUserName, usmUserEntryData.valid);

  usmUserEntryData.usmUserStorageType = val;
  SET_VALID(I_usmUserStorageType, usmUserEntryData.valid);

  rc = snmp_usmUserTable_set(&usmUserEntryData);

  FreeOctetString(usmUserEntryData.usmUserEngineID);
  FreeOctetString(usmUserEntryData.usmUserName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the usmUserStatus column
*        is 'notReady'.
*        
*        In particular, a newly created row for a user who
*        employs authentication, cannot be made active until the
*        corresponding usmUserCloneFrom and usmUserAuthKeyChange
*        have been set.
*        
*        Further, a newly created row for a user who also
*        employs privacy, cannot be made active until the
*        usmUserPrivKeyChange has been set.
*        
*        The RowStatus TC [RFC2579] requires that this
*        DESCRIPTION clause states under which circumstances
*        other objects in this row can be modified:
*        
*        The value of this object has no effect on whether
*        other objects in this conceptual row can be modified,
*        except for usmUserOwnAuthKeyChange and
*        usmUserOwnPrivKeyChange. For these 2 objects, the
*        value of usmUserStatus MUST be active.
*
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserStatusGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_snmpRowStatus_t *val)
{
  usmUserEntry_t * usmUserEntryData = snmp_usmUserTable_get(EXACT, usmUserEngineID, usmUserEngineID_len, usmUserName);

  if (usmUserEntryData != NULL && 
      VALID(I_usmUserStatus, usmUserEntryData->valid))
  {
    *val = usmUserEntryData->usmUserStatus;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the usmUserStatus column
*        is 'notReady'.
*        
*        In particular, a newly created row for a user who
*        employs authentication, cannot be made active until the
*        corresponding usmUserCloneFrom and usmUserAuthKeyChange
*        have been set.
*        
*        Further, a newly created row for a user who also
*        employs privacy, cannot be made active until the
*        usmUserPrivKeyChange has been set.
*        
*        The RowStatus TC [RFC2579] requires that this
*        DESCRIPTION clause states under which circumstances
*        other objects in this row can be modified:
*        
*        The value of this object has no effect on whether
*        other objects in this conceptual row can be modified,
*        except for usmUserOwnAuthKeyChange and
*        usmUserOwnPrivKeyChange. For these 2 objects, the
*        value of usmUserStatus MUST be active.
*
* @end
*
*********************************************************************/
L7_RC_t l7_usmUserStatusSet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_snmpRowStatus_t val)
{
  usmUserEntry_t usmUserEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&usmUserEntryData, 0, sizeof(usmUserEntryData));

  if (l7_snmpStringSetValidate(usmUserName, 1, 32) != L7_SUCCESS)
    return rc;

  usmUserEntryData.usmUserEngineID = MakeOctetString(usmUserEngineID, usmUserEngineID_len);
  SET_VALID(I_usmUserEngineID, usmUserEntryData.valid);

  usmUserEntryData.usmUserName = MakeOctetStringFromText(usmUserName);
  SET_VALID(I_usmUserName, usmUserEntryData.valid);

  usmUserEntryData.usmUserStatus = val;
  SET_VALID(I_usmUserStatus, usmUserEntryData.valid);

  rc = snmp_usmUserTable_set(&usmUserEntryData);

  FreeOctetString(usmUserEntryData.usmUserEngineID);
  FreeOctetString(usmUserEntryData.usmUserName);

  return rc;
}

void
debug_usmUserTable_walk()
{
  L7_char8 usmUserEngineID[256];
  L7_uint32 usmUserEngineID_len = 0;
  L7_char8 usmUserEngineID_string[256];
  L7_char8 usmUserName[256];
  L7_char8 key_string[256];
  L7_char8 temp_string[256];
  L7_uint32 temp_val,temp_val2;

  memset((void*)usmUserEngineID, 0, sizeof(usmUserEngineID));

  sysapiPrintf("[usmUserTable start]\n");

  while(l7_usmUserTableNextGet(usmUserEngineID, &usmUserEngineID_len, usmUserName) == L7_SUCCESS)
  {
    debug_snmp_sprintHexString(usmUserEngineID, usmUserEngineID_len, usmUserEngineID_string);

    sysapiPrintf("usmUserEngineID.\"%s\".\"%s\" = \"%s\"\n", usmUserEngineID_string, usmUserName, usmUserEngineID_string);

    sysapiPrintf("usmUserName.\"%s\".\"%s\" = \"%s\"\n", usmUserEngineID_string, usmUserName, usmUserName);

    if (l7_usmUserSecurityNameGet(usmUserEngineID, usmUserEngineID_len, usmUserName, temp_string) == L7_SUCCESS)
    {
      sysapiPrintf("usmUserSecurityName.\"%s\".\"%s\" = \"%s\"\n", usmUserEngineID_string, usmUserName, temp_string);
      if (l7_vacmGroupNameGet(snmpSecurityModel_usm, temp_string, key_string) == L7_SUCCESS)
        sysapiPrintf("usmUserGroup.\"%s\".\"%s\" = \"%s\"\n", usmUserEngineID_string, usmUserName, key_string);
    }
    else
    {
      sysapiPrintf("usmUserSecurityName.\"%s\".\"%s\" = L7_FAILURE\n", usmUserEngineID_string, usmUserName);
      sysapiPrintf("usmUserGroup.\"%s\".\"%s\" = \"%s\" = L7_FAILURE\n", usmUserEngineID_string, usmUserName);
    }

    if (l7_usmUserAuthProtocolGet(usmUserEngineID, usmUserEngineID_len, usmUserName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("usmUserAuthProtocol.\"%s\".\"%s\" = %d\n", usmUserEngineID_string, usmUserName, temp_val);
    else
      sysapiPrintf("usmUserAuthProtocol.\"%s\".\"%s\" = L7_FAILURE\n", usmUserEngineID_string, usmUserName);

    if (l7_usmUserAuthProtocolKeyGet(usmUserEngineID, usmUserEngineID_len, usmUserName, &temp_val, temp_string, &temp_val2) 
          == L7_SUCCESS)
    {
      sysapiPrintf("usmUserAuthProtocol.\"%s\".\"%s\" = %d\n", usmUserEngineID_string, usmUserName, temp_val);

      debug_snmp_sprintHexString(temp_string, temp_val2, key_string);
      sysapiPrintf("auth_secret = %s\n", key_string);
    }
    else
      sysapiPrintf("usmUserAuthProtocol.\"%s\".\"%s\" = L7_FAILURE\n", usmUserEngineID_string, usmUserName);

    if (l7_usmUserPrivProtocolGet(usmUserEngineID, usmUserEngineID_len, usmUserName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("usmUserPrivProtocol.\"%s\".\"%s\" = %d\n", usmUserEngineID_string, usmUserName, temp_val);
    else
      sysapiPrintf("usmUserPrivProtocol.\"%s\".\"%s\" = L7_FAILURE\n", usmUserEngineID_string, usmUserName);

    if (l7_usmUserPrivProtocolKeyGet(usmUserEngineID, usmUserEngineID_len, usmUserName, &temp_val, temp_string, &temp_val2) == L7_SUCCESS)
    {
      sysapiPrintf("usmUserPrivProtocol.\"%s\".\"%s\" = %d\n", usmUserEngineID_string, usmUserName, temp_val);

      debug_snmp_sprintHexString(temp_string, temp_val2, key_string);
      sysapiPrintf("priv_secret = %s\n", key_string);
    }
    else
      sysapiPrintf("usmUserPrivProtocol.\"%s\".\"%s\" = L7_FAILURE\n", usmUserEngineID_string, usmUserName);

    if (l7_usmUserStorageTypeGet(usmUserEngineID, usmUserEngineID_len, usmUserName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("usmUserStorageType.\"%s\".\"%s\" = %d\n", usmUserEngineID_string, usmUserName, temp_val);
    else
      sysapiPrintf("usmUserStorageType.\"%s\".\"%s\" = L7_FAILURE\n", usmUserEngineID_string, usmUserName);

    if (l7_usmUserStatusGet(usmUserEngineID, usmUserEngineID_len, usmUserName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("usmUserStatus.\"%s\".\"%s\" = %d\n", usmUserEngineID_string, usmUserName, temp_val);
    else
      sysapiPrintf("usmUserStatus.\"%s\".\"%s\" = L7_FAILURE\n", usmUserEngineID_string, usmUserName);

    sysapiPrintf("\n");
  }
  sysapiPrintf("[end]\n");
}

void
debug_usmUserTable_create(L7_char8 *usmUserName)
{
  L7_RC_t rc = L7_FAILURE;

  rc = l7_usmUserStatusSet(NULL, 0, usmUserName, snmpRowStatus_createAndGo);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_usmUserStatusSet 1 failed!\n");
    return;
  }

  rc = l7_usmUserSecurityNameSet(NULL, 0, usmUserName, usmUserName);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_usmUserSecurityNameSet 1 failed!\n");
    return;
  }

  rc = l7_usmUserAuthProtocolSet(NULL, 0, usmUserName, usmUserAuthProtocol_none, "");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_usmUserAuthProtocolSet 1 failed!\n");
    return;
  }

  rc = l7_usmUserAuthProtocolSet(NULL, 0, usmUserName, usmUserAuthProtocol_md5, "12341234");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_usmUserAuthProtocolSet 2 failed!\n");
    return;
  }

  rc = l7_usmUserAuthProtocolSet(NULL, 0, usmUserName, usmUserAuthProtocol_sha, "12341234");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_usmUserAuthProtocolSet 3 failed!\n");
    return;
  }


  rc = l7_usmUserPrivProtocolSet(NULL, 0, usmUserName, usmUserPrivProtocol_none, "");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_usmUserPrivProtocolSet 1 failed!\n");
    return;
  }

  rc = l7_usmUserPrivProtocolSet(NULL, 0, usmUserName, usmUserPrivProtocol_des, "12341234");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_usmUserPrivProtocolSet 2 failed!\n");
    return;
  }

  rc = l7_usmUserPrivProtocolSet(NULL, 0, usmUserName, usmUserPrivProtocol_3des, "12341234");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_usmUserPrivProtocolSet 3 failed!  Good!\n");
  }

  rc = l7_usmUserStatusSet(NULL, 0, usmUserName, snmpRowStatus_active);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_usmUserStatusSet 2 failed!\n");
    return;
  }

  sysapiPrintf("debug_usmUserTable_create finished!\n");
}
