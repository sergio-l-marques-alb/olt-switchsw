
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
#include "frmt_lib.h"

#include "l7_common.h"
#include "snmp_exports.h"
#include "usmdb_util_api.h"
#include "sysapi.h"

#include "snmp_confapi.h"

extern SnmpV2Table agt_snmpTargetAddrTable;          /* snmpTargetAddrEntry_t */
extern SnmpV2Table agt_snmpTargetParamsTable;        /* snmpTargetParamsEntry_t */

extern int writeConfigFileFlag;


/*******************************************************************************************/

/*********************************************************************
*
* @purpose Retrieves the row associated with the index for the given request
*
* @returns Pointer to snmpTargetAddrEntry_t object
*           
* @notes A table of transport addresses to be used in the generation
*        of SNMP messages.
*
* @end
*
*********************************************************************/
static snmpTargetAddrEntry_t* snmp_snmpTargetAddrTable_get(int searchType, L7_char8 *snmpTargetAddrName)
{
  L7_int32 index = -1;
  snmpTargetAddrEntry_t *row = NULL;

  /* check arguments */
  if ((searchType != EXACT && searchType != NEXT) || snmpTargetAddrName == NULL)
    return NULL;

  /* build the search structure */
  agt_snmpTargetAddrTable.tip[0].value.octet_val = MakeOctetStringFromText(snmpTargetAddrName);
  if (agt_snmpTargetAddrTable.tip[0].value.octet_val == NULL)
  {
    return NULL;
  }

  index = SearchTable(&agt_snmpTargetAddrTable, searchType);

  /* check to see if we need to search again */
  if (index >= 0)
  {
    row = (snmpTargetAddrEntry_t*)agt_snmpTargetAddrTable.tp[index];

    if (searchType == NEXT &&
        strncmp(snmpTargetAddrName, 
                row->snmpTargetAddrName->octet_ptr,
                row->snmpTargetAddrName->length) == 0)
    index = SearchTable(&agt_snmpTargetAddrTable, NEXT_SKIP);
  }


  FreeOctetString(agt_snmpTargetAddrTable.tip[0].value.octet_val);

  if (index >= 0)
  {
    return(snmpTargetAddrEntry_t*)agt_snmpTargetAddrTable.tp[index];
  }

  return NULL;
}

extern OctetString *agt_local_snmpID; 

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
static int l7_snmpTargetAddrEntry_set_defaults(snmpTargetAddrEntry_t *data)
{
  if ((data->snmpTargetAddrTDomain = MakeOIDFromDot("0.0")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->snmpTargetAddrTAddress = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->snmpTargetAddrTimeout = 1500;    /* TimeInterval / 1/100 seconds */
  data->snmpTargetAddrRetryCount = 3;
  if ((data->snmpTargetAddrTagList = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->snmpTargetAddrParams = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->snmpTargetAddrStorageType = D_snmpTargetAddrStorageType_nonVolatile;
  if ((data->snmpTargetAddrTMask = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->snmpTargetAddrMMS = 2048;

  data->RowStatusTimerId = -1;

  SET_ALL_VALID(data->valid);
  /*
   * clear valid bits for non-index objects that do not have
   * a DEFVAL clause
   */
  CLR_VALID(I_snmpTargetAddrTDomain, data->valid);
  CLR_VALID(I_snmpTargetAddrTAddress, data->valid);
  CLR_VALID(I_snmpTargetAddrParams, data->valid);
  CLR_VALID(I_snmpTargetAddrRowStatus, data->valid);

  return NO_ERROR;
}

/*********************************************************************
*
* @purpose Sets the row associated with the index for the given request
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*           
* @notes A table of transport addresses to be used in the generation
*        of SNMP messages.
*
* @end
*
*********************************************************************/
static L7_RC_t snmp_snmpTargetAddrTable_set(snmpTargetAddrEntry_t *data)
{
  L7_int32 index = -1;
  snmpTargetAddrEntry_t * snmpTargetAddrEntryData = NULL;
  L7_RC_t rc = L7_SUCCESS;

  /* check arguments */
  if (data == NULL)
    return L7_ERROR;

  if (!VALID(I_snmpTargetAddrName, data->valid))
    return L7_ERROR;

  /* build the search structure */
  agt_snmpTargetAddrTable.tip[0].value.octet_val = CloneOctetString(data->snmpTargetAddrName);
  if (agt_snmpTargetAddrTable.tip[0].value.octet_val == NULL)
  {
    return L7_ERROR;
  }

  index = SearchTable(&agt_snmpTargetAddrTable, EXACT);

  if (index < 0)
  {
    if (VALID(I_snmpTargetAddrRowStatus, data->valid) && 
        (data->snmpTargetAddrRowStatus == D_snmpTargetAddrRowStatus_createAndGo ||
         data->snmpTargetAddrRowStatus == D_snmpTargetAddrRowStatus_createAndWait))
    {
      index = NewTableEntry(&agt_snmpTargetAddrTable);

      writeConfigFileFlag = TRUE;

      if (index < 0 ||
          l7_snmpTargetAddrEntry_set_defaults((snmpTargetAddrEntry_t*)agt_snmpTargetAddrTable.tp[index]) != NO_ERROR)
        return L7_ERROR;

      if (data->snmpTargetAddrRowStatus == D_snmpTargetAddrRowStatus_createAndGo)
      {
        ((snmpTargetAddrEntry_t*)agt_snmpTargetAddrTable.tp[index])->snmpTargetAddrRowStatus = D_snmpTargetAddrRowStatus_active;
      }
      else
      {
        ((snmpTargetAddrEntry_t*)agt_snmpTargetAddrTable.tp[index])->snmpTargetAddrRowStatus = D_snmpTargetAddrRowStatus_notInService;
      }
      SET_VALID(I_snmpTargetAddrRowStatus, ((snmpTargetAddrEntry_t*)agt_snmpTargetAddrTable.tp[index])->valid);

      CLR_VALID(I_snmpTargetAddrRowStatus, data->valid);
    }
  }

  FreeOctetString(agt_snmpTargetAddrTable.tip[0].value.octet_val);

  if (index >= 0)
  {
    if (VALID(I_snmpTargetAddrRowStatus, data->valid) && 
        data->snmpTargetAddrRowStatus == D_snmpTargetAddrRowStatus_destroy)
    {
      RemoveTableEntry(&agt_snmpTargetAddrTable, index);
      return L7_SUCCESS;
    }

    snmpTargetAddrEntryData = (snmpTargetAddrEntry_t*)agt_snmpTargetAddrTable.tp[index];

    if (VALID(I_snmpTargetAddrTDomain, data->valid))
    {
      if (snmpTargetAddrEntryData->snmpTargetAddrTDomain != NULL)
      {
        FreeOID(snmpTargetAddrEntryData->snmpTargetAddrTDomain);
        CLR_VALID(I_snmpTargetAddrTDomain, snmpTargetAddrEntryData->valid);
      }

      snmpTargetAddrEntryData->snmpTargetAddrTDomain = CloneOID(data->snmpTargetAddrTDomain);

      if (snmpTargetAddrEntryData->snmpTargetAddrTDomain != NULL)
      {
        SET_VALID(I_snmpTargetAddrTDomain, snmpTargetAddrEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_snmpTargetAddrTAddress, data->valid))
    {
      if (snmpTargetAddrEntryData->snmpTargetAddrTAddress != NULL)
      {
        FreeOctetString(snmpTargetAddrEntryData->snmpTargetAddrTAddress);
        CLR_VALID(I_snmpTargetAddrTAddress, snmpTargetAddrEntryData->valid);
      }

      snmpTargetAddrEntryData->snmpTargetAddrTAddress = CloneOctetString(data->snmpTargetAddrTAddress);

      if (snmpTargetAddrEntryData->snmpTargetAddrTAddress != NULL)
      {
        SET_VALID(I_snmpTargetAddrTAddress, snmpTargetAddrEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_snmpTargetAddrTimeout, data->valid))
    {
      if (data->snmpTargetAddrTimeout > 2147483647)
        return L7_ERROR;

      snmpTargetAddrEntryData->snmpTargetAddrTimeout = data->snmpTargetAddrTimeout;
      SET_VALID(I_snmpTargetAddrTimeout, snmpTargetAddrEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_snmpTargetAddrRetryCount, data->valid))
    {
      if (data->snmpTargetAddrRetryCount > 255)
        return L7_ERROR;

      snmpTargetAddrEntryData->snmpTargetAddrRetryCount = data->snmpTargetAddrRetryCount;
      SET_VALID(I_snmpTargetAddrRetryCount, snmpTargetAddrEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_snmpTargetAddrTagList, data->valid))
    {
      if (snmpTargetAddrEntryData->snmpTargetAddrTagList != NULL)
      {
        FreeOctetString(snmpTargetAddrEntryData->snmpTargetAddrTagList);
        CLR_VALID(I_snmpTargetAddrTagList, snmpTargetAddrEntryData->valid);
      }

      snmpTargetAddrEntryData->snmpTargetAddrTagList = CloneOctetString(data->snmpTargetAddrTagList);

      if (snmpTargetAddrEntryData->snmpTargetAddrTagList != NULL)
      {
        SET_VALID(I_snmpTargetAddrTagList, snmpTargetAddrEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_snmpTargetAddrParams, data->valid))
    {
      if (snmpTargetAddrEntryData->snmpTargetAddrParams != NULL)
      {
        FreeOctetString(snmpTargetAddrEntryData->snmpTargetAddrParams);
        CLR_VALID(I_snmpTargetAddrParams, snmpTargetAddrEntryData->valid);
      }

      snmpTargetAddrEntryData->snmpTargetAddrParams = CloneOctetString(data->snmpTargetAddrParams);

      if (snmpTargetAddrEntryData->snmpTargetAddrParams != NULL)
      {
        SET_VALID(I_snmpTargetAddrParams, snmpTargetAddrEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_snmpTargetAddrStorageType, data->valid))
    {
      if (l7_snmpStorageTypeSetValidate(snmpTargetAddrEntryData->snmpTargetAddrStorageType,
                                        data->snmpTargetAddrStorageType) != L7_SUCCESS)
        return L7_ERROR;

      snmpTargetAddrEntryData->snmpTargetAddrStorageType = data->snmpTargetAddrStorageType;
      SET_VALID(I_snmpTargetAddrStorageType, snmpTargetAddrEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_snmpTargetAddrRowStatus, data->valid))
    {
      if (l7_snmpRowStatusSetValidate(snmpTargetAddrEntryData->snmpTargetAddrRowStatus,
                                      data->snmpTargetAddrRowStatus) != L7_SUCCESS)
        return L7_ERROR;

      snmpTargetAddrEntryData->snmpTargetAddrRowStatus = data->snmpTargetAddrRowStatus;
      SET_VALID(I_snmpTargetAddrRowStatus, snmpTargetAddrEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_snmpTargetAddrTMask, data->valid))
    {
      if (snmpTargetAddrEntryData->snmpTargetAddrTMask != NULL)
      {
        FreeOctetString(snmpTargetAddrEntryData->snmpTargetAddrTMask);
        CLR_VALID(I_snmpTargetAddrTMask, snmpTargetAddrEntryData->valid);
      }

      snmpTargetAddrEntryData->snmpTargetAddrTMask = CloneOctetString(data->snmpTargetAddrTMask);

      if (snmpTargetAddrEntryData->snmpTargetAddrTMask != NULL)
      {
        SET_VALID(I_snmpTargetAddrTMask, snmpTargetAddrEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
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
L7_RC_t l7_snmpTargetAddrTableCheckValid(L7_char8 *snmpTargetAddrName)
{
  snmpTargetAddrEntry_t * snmpTargetAddrEntryData = snmp_snmpTargetAddrTable_get(EXACT, snmpTargetAddrName);

  if (snmpTargetAddrEntryData != NULL)
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
L7_RC_t l7_snmpTargetAddrTableNextGet(L7_char8 *snmpTargetAddrName)
{
  snmpTargetAddrEntry_t * snmpTargetAddrEntryData = snmp_snmpTargetAddrTable_get(NEXT, snmpTargetAddrName);

  if (snmpTargetAddrEntryData != NULL)
  {
    strncpy(snmpTargetAddrName, snmpTargetAddrEntryData->snmpTargetAddrName->octet_ptr,
            snmpTargetAddrEntryData->snmpTargetAddrName->length);
    snmpTargetAddrName[snmpTargetAddrEntryData->snmpTargetAddrName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTDomain
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object indicates the transport type of the address
*        contained in the snmpTargetAddrTAddress object.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrTDomainGet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
  snmpTargetAddrEntry_t * snmpTargetAddrEntryData = snmp_snmpTargetAddrTable_get(EXACT, snmpTargetAddrName);

  if (snmpTargetAddrEntryData != NULL && 
      VALID(I_snmpTargetAddrTDomain, snmpTargetAddrEntryData->valid))
  {
    SPrintOID(snmpTargetAddrEntryData->snmpTargetAddrTDomain, val);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTDomain
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object indicates the transport type of the address
*        contained in the snmpTargetAddrTAddress object.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrTDomainSet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
  snmpTargetAddrEntry_t snmpTargetAddrEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpTargetAddrEntryData, 0, sizeof(snmpTargetAddrEntryData));

  snmpTargetAddrEntryData.snmpTargetAddrName = MakeOctetStringFromText(snmpTargetAddrName);
  SET_VALID(I_snmpTargetAddrName, snmpTargetAddrEntryData.valid);

  snmpTargetAddrEntryData.snmpTargetAddrTDomain = MakeOIDFromDot(val);
  SET_VALID(I_snmpTargetAddrTDomain, snmpTargetAddrEntryData.valid);

  if (snmpTargetAddrEntryData.snmpTargetAddrTDomain == NULL)
    return rc;

  rc = snmp_snmpTargetAddrTable_set(&snmpTargetAddrEntryData);

  FreeOctetString(snmpTargetAddrEntryData.snmpTargetAddrName);
  FreeOID(snmpTargetAddrEntryData.snmpTargetAddrTDomain);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTAddress
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object contains a transport address.  The format of
*        this address depends on the value of the
*        snmpTargetAddrTDomain object.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrTAddressGet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
  snmpTargetAddrEntry_t * snmpTargetAddrEntryData = snmp_snmpTargetAddrTable_get(EXACT, snmpTargetAddrName);
  L7_ushort16 temp_val;

  if (snmpTargetAddrEntryData != NULL && 
      VALID(I_snmpTargetAddrTAddress, snmpTargetAddrEntryData->valid) &&
      snmpTargetAddrEntryData->snmpTargetAddrTAddress->length == 6)
  {
    temp_val = ((snmpTargetAddrEntryData->snmpTargetAddrTAddress->octet_ptr[4] << 8) +
                snmpTargetAddrEntryData->snmpTargetAddrTAddress->octet_ptr[5]);
    sprintf(val, "%d.%d.%d.%d:%d",
            snmpTargetAddrEntryData->snmpTargetAddrTAddress->octet_ptr[0],
            snmpTargetAddrEntryData->snmpTargetAddrTAddress->octet_ptr[1],
            snmpTargetAddrEntryData->snmpTargetAddrTAddress->octet_ptr[2],
            snmpTargetAddrEntryData->snmpTargetAddrTAddress->octet_ptr[3],
            temp_val);

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTAddress
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object contains a transport address.  The format of
*        this address depends on the value of the
*        snmpTargetAddrTDomain object.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrTAddressSet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
  snmpTargetAddrEntry_t snmpTargetAddrEntryData;
  L7_uint32 ipAddress;
  L7_ushort16 port = 0;
  L7_uint32 i;
  L7_char8 buf[6];
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 1, 255) != L7_SUCCESS)
    return rc;
  memset((void*)&snmpTargetAddrEntryData, 0, sizeof(snmpTargetAddrEntryData));

  snmpTargetAddrEntryData.snmpTargetAddrName = MakeOctetStringFromText(snmpTargetAddrName);
  SET_VALID(I_snmpTargetAddrName, snmpTargetAddrEntryData.valid);

  for (i=0; i < (strlen(val) - 1); i++)
  {
    if (val[i] == ':')
    {
      val[i] = '\0';
      usmDbInetAton(val, &ipAddress);
      
      port = atoi(val + (i+1));
      break;
    }
  }

  memcpy(buf, &ipAddress, sizeof(ipAddress));
  memcpy(buf+sizeof(ipAddress), &port, sizeof(port));

  snmpTargetAddrEntryData.snmpTargetAddrTAddress = MakeOctetString(buf, sizeof(buf));

  SET_VALID(I_snmpTargetAddrTAddress, snmpTargetAddrEntryData.valid);
  rc = snmp_snmpTargetAddrTable_set(&snmpTargetAddrEntryData);
  FreeOctetString(snmpTargetAddrEntryData.snmpTargetAddrTAddress);
  FreeOctetString(snmpTargetAddrEntryData.snmpTargetAddrName);
  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTimeout
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object should reflect the expected maximum round
*        trip time for communicating with the transport address
*        defined by this row.  When a message is sent to this
*        address, and a response (if one is expected) is not
*        received within this time period, an implementation
*        may assume that the response will not be delivered.
*        
*        Note that the time interval that an application waits
*        for a response may actually be derived from the value
*        of this object.  The method for deriving the actual time
*        interval is implementation dependent.  One such method
*        is to derive the expected round trip time based on a
*        particular retransmission algorithm and on the number
*        of timeouts which have occurred.  The type of message may
*        also be considered when deriving expected round trip
*        times for retransmissions.  For example, if a message is
*        being sent with a securityLevel that indicates both
*        authentication and privacy, the derived value may be
*        increased to compensate for extra processing time spent
*        during authentication and encryption processing.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrTimeoutGet(L7_char8* snmpTargetAddrName, L7_uint32 *val)
{
  snmpTargetAddrEntry_t * snmpTargetAddrEntryData = snmp_snmpTargetAddrTable_get(EXACT, snmpTargetAddrName);

  if (snmpTargetAddrEntryData != NULL && 
      VALID(I_snmpTargetAddrTimeout, snmpTargetAddrEntryData->valid))
  {
    *val = snmpTargetAddrEntryData->snmpTargetAddrTimeout;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTimeout
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object should reflect the expected maximum round
*        trip time for communicating with the transport address
*        defined by this row.  When a message is sent to this
*        address, and a response (if one is expected) is not
*        received within this time period, an implementation
*        may assume that the response will not be delivered.
*        
*        Note that the time interval that an application waits
*        for a response may actually be derived from the value
*        of this object.  The method for deriving the actual time
*        interval is implementation dependent.  One such method
*        is to derive the expected round trip time based on a
*        particular retransmission algorithm and on the number
*        of timeouts which have occurred.  The type of message may
*        also be considered when deriving expected round trip
*        times for retransmissions.  For example, if a message is
*        being sent with a securityLevel that indicates both
*        authentication and privacy, the derived value may be
*        increased to compensate for extra processing time spent
*        during authentication and encryption processing.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrTimeoutSet(L7_char8* snmpTargetAddrName, L7_uint32 val)
{
  snmpTargetAddrEntry_t snmpTargetAddrEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpTargetAddrEntryData, 0, sizeof(snmpTargetAddrEntryData));

  snmpTargetAddrEntryData.snmpTargetAddrName = MakeOctetStringFromText(snmpTargetAddrName);
  SET_VALID(I_snmpTargetAddrName, snmpTargetAddrEntryData.valid);

  snmpTargetAddrEntryData.snmpTargetAddrTimeout = val;
  SET_VALID(I_snmpTargetAddrTimeout, snmpTargetAddrEntryData.valid);

  rc = snmp_snmpTargetAddrTable_set(&snmpTargetAddrEntryData);

  FreeOctetString(snmpTargetAddrEntryData.snmpTargetAddrName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrRetryCount
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object specifies a default number of retries to be
*        attempted when a response is not received for a generated
*        message.  An application may provide its own retry count,
*        in which case the value of this object is ignored.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrRetryCountGet(L7_char8* snmpTargetAddrName, L7_uint32 *val)
{
  snmpTargetAddrEntry_t * snmpTargetAddrEntryData = snmp_snmpTargetAddrTable_get(EXACT, snmpTargetAddrName);

  if (snmpTargetAddrEntryData != NULL && 
      VALID(I_snmpTargetAddrRetryCount, snmpTargetAddrEntryData->valid))
  {
    *val = snmpTargetAddrEntryData->snmpTargetAddrRetryCount;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrRetryCount
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object specifies a default number of retries to be
*        attempted when a response is not received for a generated
*        message.  An application may provide its own retry count,
*        in which case the value of this object is ignored.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrRetryCountSet(L7_char8* snmpTargetAddrName, L7_uint32 val)
{
  snmpTargetAddrEntry_t snmpTargetAddrEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpTargetAddrEntryData, 0, sizeof(snmpTargetAddrEntryData));

  snmpTargetAddrEntryData.snmpTargetAddrName = MakeOctetStringFromText(snmpTargetAddrName);
  SET_VALID(I_snmpTargetAddrName, snmpTargetAddrEntryData.valid);

  snmpTargetAddrEntryData.snmpTargetAddrRetryCount = val;
  SET_VALID(I_snmpTargetAddrRetryCount, snmpTargetAddrEntryData.valid);

  rc = snmp_snmpTargetAddrTable_set(&snmpTargetAddrEntryData);

  FreeOctetString(snmpTargetAddrEntryData.snmpTargetAddrName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTagList
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object contains a list of tag values which are
*        used to select target addresses for a particular
*        operation.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrTagListGet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
  snmpTargetAddrEntry_t * snmpTargetAddrEntryData = snmp_snmpTargetAddrTable_get(EXACT, snmpTargetAddrName);

  if (snmpTargetAddrEntryData != NULL && 
      VALID(I_snmpTargetAddrTagList, snmpTargetAddrEntryData->valid))
  {
    strncpy(val, snmpTargetAddrEntryData->snmpTargetAddrTagList->octet_ptr,
            snmpTargetAddrEntryData->snmpTargetAddrTagList->length);
    val[snmpTargetAddrEntryData->snmpTargetAddrTagList->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTagList
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object contains a list of tag values which are
*        used to select target addresses for a particular
*        operation.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrTagListSet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
  snmpTargetAddrEntry_t snmpTargetAddrEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 0, 255) != L7_SUCCESS)
    return rc;

  memset((void*)&snmpTargetAddrEntryData, 0, sizeof(snmpTargetAddrEntryData));

  snmpTargetAddrEntryData.snmpTargetAddrName = MakeOctetStringFromText(snmpTargetAddrName);
  SET_VALID(I_snmpTargetAddrName, snmpTargetAddrEntryData.valid);

  snmpTargetAddrEntryData.snmpTargetAddrTagList = MakeOctetStringFromText(val);
  SET_VALID(I_snmpTargetAddrTagList, snmpTargetAddrEntryData.valid);

  rc = snmp_snmpTargetAddrTable_set(&snmpTargetAddrEntryData);

  FreeOctetString(snmpTargetAddrEntryData.snmpTargetAddrName);
  FreeOctetString(snmpTargetAddrEntryData.snmpTargetAddrTagList);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrParams
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The value of this object identifies an entry in the
*        snmpTargetParamsTable.  The identified entry
*        contains SNMP parameters to be used when generating
*        messages to be sent to this transport address.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrParamsGet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
  snmpTargetAddrEntry_t * snmpTargetAddrEntryData = snmp_snmpTargetAddrTable_get(EXACT, snmpTargetAddrName);

  if (snmpTargetAddrEntryData != NULL && 
      VALID(I_snmpTargetAddrParams, snmpTargetAddrEntryData->valid))
  {
    strncpy(val, snmpTargetAddrEntryData->snmpTargetAddrParams->octet_ptr,
            snmpTargetAddrEntryData->snmpTargetAddrParams->length);
    val[snmpTargetAddrEntryData->snmpTargetAddrParams->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrParams
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The value of this object identifies an entry in the
*        snmpTargetParamsTable.  The identified entry
*        contains SNMP parameters to be used when generating
*        messages to be sent to this transport address.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrParamsSet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
  snmpTargetAddrEntry_t snmpTargetAddrEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 0, 255) != L7_SUCCESS)
    return rc;

  memset((void*)&snmpTargetAddrEntryData, 0, sizeof(snmpTargetAddrEntryData));

  snmpTargetAddrEntryData.snmpTargetAddrName = MakeOctetStringFromText(snmpTargetAddrName);
  SET_VALID(I_snmpTargetAddrName, snmpTargetAddrEntryData.valid);

  snmpTargetAddrEntryData.snmpTargetAddrParams = MakeOctetStringFromText(val);
  SET_VALID(I_snmpTargetAddrParams, snmpTargetAddrEntryData.valid);

  rc = snmp_snmpTargetAddrTable_set(&snmpTargetAddrEntryData);

  FreeOctetString(snmpTargetAddrEntryData.snmpTargetAddrName);
  FreeOctetString(snmpTargetAddrEntryData.snmpTargetAddrParams);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrStorageTypeGet(L7_char8* snmpTargetAddrName, l7_snmpStorageType_t *val)
{
  snmpTargetAddrEntry_t * snmpTargetAddrEntryData = snmp_snmpTargetAddrTable_get(EXACT, snmpTargetAddrName);

  if (snmpTargetAddrEntryData != NULL && 
      VALID(I_snmpTargetAddrStorageType, snmpTargetAddrEntryData->valid))
  {
    *val = snmpTargetAddrEntryData->snmpTargetAddrStorageType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrStorageTypeSet(L7_char8* snmpTargetAddrName, l7_snmpStorageType_t val)
{
  snmpTargetAddrEntry_t snmpTargetAddrEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpTargetAddrEntryData, 0, sizeof(snmpTargetAddrEntryData));

  snmpTargetAddrEntryData.snmpTargetAddrName = MakeOctetStringFromText(snmpTargetAddrName);
  SET_VALID(I_snmpTargetAddrName, snmpTargetAddrEntryData.valid);

  snmpTargetAddrEntryData.snmpTargetAddrStorageType = val;
  SET_VALID(I_snmpTargetAddrStorageType, snmpTargetAddrEntryData.valid);

  rc = snmp_snmpTargetAddrTable_set(&snmpTargetAddrEntryData);

  FreeOctetString(snmpTargetAddrEntryData.snmpTargetAddrName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrRowStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the snmpTargetAddrRowStatus
*        column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until the corresponding instances of
*        snmpTargetAddrTDomain, snmpTargetAddrTAddress, and
*        snmpTargetAddrParams have all been set.
*        
*        The following objects may not be modified while the
*        value of this object is active(1):
*        An attempt to set these objects while the value of
*        snmpTargetAddrRowStatus is active(1) will result in
*        an inconsistentValue error.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrRowStatusGet(L7_char8* snmpTargetAddrName, l7_snmpRowStatus_t *val)
{
  snmpTargetAddrEntry_t * snmpTargetAddrEntryData = snmp_snmpTargetAddrTable_get(EXACT, snmpTargetAddrName);

  if (snmpTargetAddrEntryData != NULL && 
      VALID(I_snmpTargetAddrRowStatus, snmpTargetAddrEntryData->valid))
  {
    *val = snmpTargetAddrEntryData->snmpTargetAddrRowStatus;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrRowStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the snmpTargetAddrRowStatus
*        column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until the corresponding instances of
*        snmpTargetAddrTDomain, snmpTargetAddrTAddress, and
*        snmpTargetAddrParams have all been set.
*        
*        The following objects may not be modified while the
*        value of this object is active(1):
*        An attempt to set these objects while the value of
*        snmpTargetAddrRowStatus is active(1) will result in
*        an inconsistentValue error.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrRowStatusSet(L7_char8* snmpTargetAddrName, l7_snmpRowStatus_t val)
{
  snmpTargetAddrEntry_t snmpTargetAddrEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpTargetAddrEntryData, 0, sizeof(snmpTargetAddrEntryData));

  if (l7_snmpStringSetValidate(snmpTargetAddrName, 1, 32) != L7_SUCCESS)
    return rc;

  snmpTargetAddrEntryData.snmpTargetAddrName = MakeOctetStringFromText(snmpTargetAddrName);
  SET_VALID(I_snmpTargetAddrName, snmpTargetAddrEntryData.valid);

  snmpTargetAddrEntryData.snmpTargetAddrRowStatus = val;
  SET_VALID(I_snmpTargetAddrRowStatus, snmpTargetAddrEntryData.valid);

  rc = snmp_snmpTargetAddrTable_set(&snmpTargetAddrEntryData);

  FreeOctetString(snmpTargetAddrEntryData.snmpTargetAddrName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTMask
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object contains a transport address.  The format of
*        this address depends on the value of the
*        snmpTargetAddrTDomain object.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrTMaskGet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
  snmpTargetAddrEntry_t * snmpTargetAddrEntryData = snmp_snmpTargetAddrTable_get(EXACT, snmpTargetAddrName);
  L7_ushort16 temp_val;

  if (snmpTargetAddrEntryData != NULL && 
      VALID(I_snmpTargetAddrTMask, snmpTargetAddrEntryData->valid) &&
      snmpTargetAddrEntryData->snmpTargetAddrTMask->length == 6)
  {
    temp_val = ((snmpTargetAddrEntryData->snmpTargetAddrTMask->octet_ptr[4] << 8) +
                snmpTargetAddrEntryData->snmpTargetAddrTMask->octet_ptr[5]);
    sprintf(val, "%d.%d.%d.%d:%d",
            snmpTargetAddrEntryData->snmpTargetAddrTMask->octet_ptr[0],
            snmpTargetAddrEntryData->snmpTargetAddrTMask->octet_ptr[1],
            snmpTargetAddrEntryData->snmpTargetAddrTMask->octet_ptr[2],
            snmpTargetAddrEntryData->snmpTargetAddrTMask->octet_ptr[3],
            temp_val);

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTMask
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object contains a transport address.  The format of
*        this address depends on the value of the
*        snmpTargetAddrTDomain object.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetAddrTMaskSet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
  snmpTargetAddrEntry_t snmpTargetAddrEntryData;
  L7_uint32 ipAddress;
  L7_ushort16 port = 0;
  L7_uint32 i;
  L7_char8 buf[6];
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 1, 255) != L7_SUCCESS)
    return rc;
  memset((void*)&snmpTargetAddrEntryData, 0, sizeof(snmpTargetAddrEntryData));

  snmpTargetAddrEntryData.snmpTargetAddrName = MakeOctetStringFromText(snmpTargetAddrName);
  SET_VALID(I_snmpTargetAddrName, snmpTargetAddrEntryData.valid);

  for (i=0; i < strlen(val); i++)
  {
    if (val[i] == ':')
    {
      val[i] = '\0';
      usmDbInetAton(val, &ipAddress);
      if (i < (strlen(val) - 1))
      {
        port = atoi((val + i + 1));
      }
      break;
    }
  }

  memcpy(buf, &ipAddress, sizeof(ipAddress));
  memcpy(buf+sizeof(ipAddress), &port, sizeof(port));

  snmpTargetAddrEntryData.snmpTargetAddrTMask = MakeOctetString(buf, sizeof(buf));

  SET_VALID(I_snmpTargetAddrTMask, snmpTargetAddrEntryData.valid);
  rc = snmp_snmpTargetAddrTable_set(&snmpTargetAddrEntryData);
  FreeOctetString(snmpTargetAddrEntryData.snmpTargetAddrTMask);
  FreeOctetString(snmpTargetAddrEntryData.snmpTargetAddrName);
  return rc;
}

void
debug_snmpTargetAddrTable_walk()
{
  L7_char8 snmpTargetAddrName[256];
  L7_char8 temp_string[256];
  L7_uint32 temp_val;

  memset((void*)snmpTargetAddrName, 0, sizeof(snmpTargetAddrName));

  sysapiPrintf("[snmpTargetAddrTable start]\n");

  while(l7_snmpTargetAddrTableNextGet(snmpTargetAddrName) == L7_SUCCESS)
  {
    sysapiPrintf("snmpTargetAddrName.\"%s\" = \"%s\"\n", snmpTargetAddrName, snmpTargetAddrName);

    if (l7_snmpTargetAddrTDomainGet(snmpTargetAddrName, temp_string) == L7_SUCCESS)
      sysapiPrintf("snmpTargetAddrTDomain.\"%s\" = \"%s\"\n", snmpTargetAddrName, temp_string);
    else
      sysapiPrintf("snmpTargetAddrTDomain.\"%s\" = L7_FAILURE\n", snmpTargetAddrName);

    if (l7_snmpTargetAddrTAddressGet(snmpTargetAddrName, temp_string) == L7_SUCCESS)
      sysapiPrintf("snmpTargetAddrTAddress.\"%s\" = \"%s\"\n", snmpTargetAddrName, temp_string);
    else
      sysapiPrintf("snmpTargetAddrTAddress.\"%s\" = L7_FAILURE\n", snmpTargetAddrName);

    if (l7_snmpTargetAddrTimeoutGet(snmpTargetAddrName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpTargetAddrTimeout.\"%s\" = %d\n", snmpTargetAddrName, temp_val);
    else
      sysapiPrintf("snmpTargetAddrTimeout.\"%s\" = L7_FAILURE\n", snmpTargetAddrName);

    if (l7_snmpTargetAddrRetryCountGet(snmpTargetAddrName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpTargetAddrRetryCount.\"%s\" = %d\n", snmpTargetAddrName, temp_val);
    else
      sysapiPrintf("snmpTargetAddrRetryCount.\"%s\" = L7_FAILURE\n", snmpTargetAddrName);

    if (l7_snmpTargetAddrTagListGet(snmpTargetAddrName, temp_string) == L7_SUCCESS)
      sysapiPrintf("snmpTargetAddrTagList.\"%s\" = \"%s\"\n", snmpTargetAddrName, temp_string);
    else
      sysapiPrintf("snmpTargetAddrTagList.\"%s\" = L7_FAILURE\n", snmpTargetAddrName);

    if (l7_snmpTargetAddrParamsGet(snmpTargetAddrName, temp_string) == L7_SUCCESS)
      sysapiPrintf("snmpTargetAddrParams.\"%s\" = \"%s\"\n", snmpTargetAddrName, temp_string);
    else
      sysapiPrintf("snmpTargetAddrParams.\"%s\" = L7_FAILURE\n", snmpTargetAddrName);

    if (l7_snmpTargetAddrStorageTypeGet(snmpTargetAddrName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpTargetAddrStorageType.\"%s\" = %d\n", snmpTargetAddrName, temp_val);
    else
      sysapiPrintf("snmpTargetAddrStorageType.\"%s\" = L7_FAILURE\n", snmpTargetAddrName);

    if (l7_snmpTargetAddrRowStatusGet(snmpTargetAddrName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpTargetAddrRowStatus.\"%s\" = %d\n", snmpTargetAddrName, temp_val);
    else
      sysapiPrintf("snmpTargetAddrRowStatus.\"%s\" = L7_FAILURE\n", snmpTargetAddrName);

    if (l7_snmpTargetAddrTMaskGet(snmpTargetAddrName, temp_string) == L7_SUCCESS)
      sysapiPrintf("snmpTargetAddrTMask.\"%s\" = \"%s\"\n", snmpTargetAddrName, temp_string);
    else
      sysapiPrintf("snmpTargetAddrTMask.\"%s\" = L7_FAILURE\n", snmpTargetAddrName);

    sysapiPrintf("\n");
  }
  sysapiPrintf("[end]\n");
}

void
debug_snmpTargetAddrTable_create(L7_char8* snmpTargetAddrName)
{
  L7_RC_t rc;

  rc = l7_snmpTargetAddrRowStatusSet(snmpTargetAddrName, snmpRowStatus_createAndWait);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpTargetAddrRowStatusSet 1 failed!\n");
    return;
  }

  rc = l7_snmpTargetAddrTDomainSet(snmpTargetAddrName, "snmpUDPDomain");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpTargetAddrTDomainSet 1 failed!\n");
    return;
  }

  rc = l7_snmpTargetAddrTAddressSet(snmpTargetAddrName, "1.2.3.4");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpTargetAddrTAddressSet 1 failed!\n");
    return;
  }

  rc = l7_snmpTargetAddrTimeoutSet(snmpTargetAddrName, 1700);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpTargetAddrTimeoutSet 1 failed!\n");
    return;
  }

  rc = l7_snmpTargetAddrRetryCountSet(snmpTargetAddrName, 17);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpTargetAddrRetryCountSet 1 failed!\n");
    return;
  }

  rc = l7_snmpTargetAddrTagListSet(snmpTargetAddrName, "tag list");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpTargetAddrTagListSet 1 failed!\n");
    return;
  }

  rc = l7_snmpTargetAddrParamsSet(snmpTargetAddrName, "params");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpTargetAddrParamsSet 1 failed!\n");
    return;
  }

  rc = l7_snmpTargetAddrRowStatusSet(snmpTargetAddrName, snmpRowStatus_active);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpTargetAddrRowStatusSet 2 failed!\n");
    return;
  }

  sysapiPrintf("debug_snmpTargetAddrTable_walk finished!");
}

/*******************************************************************************************/

/*********************************************************************
*
* @purpose Retrieves the row associated with the index for the given request
*
* @returns Pointer to snmpTargetParamsEntry_t object
*           
* @notes A table of SNMP target information to be used
*        in the generation of SNMP messages.
*
* @end
*
*********************************************************************/
static snmpTargetParamsEntry_t* snmp_snmpTargetParamsTable_get(int searchType, L7_char8 *snmpTargetParamsName)
{
  L7_int32 index = -1;
  snmpTargetParamsEntry_t *row = NULL;

  /* check arguments */
  if ((searchType != EXACT && searchType != NEXT) || snmpTargetParamsName == NULL)
    return NULL;

  /* build the search structure */
  agt_snmpTargetParamsTable.tip[0].value.octet_val = MakeOctetStringFromText(snmpTargetParamsName);
  if (agt_snmpTargetParamsTable.tip[0].value.octet_val == NULL)
  {
    return NULL;
  }

  index = SearchTable(&agt_snmpTargetParamsTable, searchType);

  /* check to see if we need to search again */
  if (index >= 0)
  {
    row = (snmpTargetParamsEntry_t*)agt_snmpTargetParamsTable.tp[index];

    if (searchType == NEXT &&
        strncmp(snmpTargetParamsName, 
                row->snmpTargetParamsName->octet_ptr,
                row->snmpTargetParamsName->length) == 0)
    index = SearchTable(&agt_snmpTargetParamsTable, NEXT_SKIP);
  }


  FreeOctetString(agt_snmpTargetParamsTable.tip[0].value.octet_val);

  if (index >= 0)
  {
    return(snmpTargetParamsEntry_t*)agt_snmpTargetParamsTable.tp[index];
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
static int l7_snmpTargetParamsEntry_set_defaults(snmpTargetParamsEntry_t *data)
{
  if ((data->snmpTargetParamsSecurityName = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->snmpTargetParamsStorageType = 
      D_snmpTargetParamsStorageType_nonVolatile;
  data->RowStatusTimerId = -1;

  SET_ALL_VALID(data->valid);
  /*
   * clear valid bits for non-index objects that do not have
   * a DEFVAL clause
   */
  CLR_VALID(I_snmpTargetParamsMPModel, data->valid);
  CLR_VALID(I_snmpTargetParamsSecurityModel, data->valid);
  CLR_VALID(I_snmpTargetParamsSecurityName, data->valid);
  CLR_VALID(I_snmpTargetParamsSecurityLevel, data->valid);
  CLR_VALID(I_snmpTargetParamsRowStatus, data->valid);
  return NO_ERROR;
}

/*
        OctetString     *snmpTargetParamsName;
        SR_INT32        snmpTargetParamsMPModel;
        SR_INT32        snmpTargetParamsSecurityModel;
        OctetString     *snmpTargetParamsSecurityName;
        SR_INT32        snmpTargetParamsSecurityLevel;
        SR_INT32        snmpTargetParamsStorageType;
        SR_INT32        snmpTargetParamsRowStatus;
*/

/*********************************************************************
*
* @purpose Sets the row associated with the index for the given request
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*           
* @notes A table of SNMP target information to be used
*        in the generation of SNMP messages.
*
* @end
*
*********************************************************************/
static L7_RC_t snmp_snmpTargetParamsTable_set(snmpTargetParamsEntry_t *data)
{
  L7_int32 index = -1;
  snmpTargetParamsEntry_t * snmpTargetParamsEntryData = NULL;
  L7_RC_t rc = L7_SUCCESS;

  /* check arguments */
  if (data == NULL)
    return L7_ERROR;

  if (!VALID(I_snmpTargetParamsName, data->valid))
    return L7_ERROR;

  /* build the search structure */
  agt_snmpTargetParamsTable.tip[0].value.octet_val = CloneOctetString(data->snmpTargetParamsName);
  if (agt_snmpTargetParamsTable.tip[0].value.octet_val == NULL)
  {
    return L7_ERROR;
  }

  index = SearchTable(&agt_snmpTargetParamsTable, EXACT);

  if (index < 0)
  {
    if (VALID(I_snmpTargetParamsRowStatus, data->valid) && 
        (data->snmpTargetParamsRowStatus == D_snmpTargetParamsRowStatus_createAndGo ||
         data->snmpTargetParamsRowStatus == D_snmpTargetParamsRowStatus_createAndWait))
    {
      index = NewTableEntry(&agt_snmpTargetParamsTable);

      writeConfigFileFlag = TRUE;

      if (index < 0 ||
          l7_snmpTargetParamsEntry_set_defaults((snmpTargetParamsEntry_t*)agt_snmpTargetParamsTable.tp[index]) != NO_ERROR)
        return L7_ERROR;

      if (data->snmpTargetParamsRowStatus == D_snmpTargetParamsRowStatus_createAndGo)
      {
        ((snmpTargetParamsEntry_t*)agt_snmpTargetParamsTable.tp[index])->snmpTargetParamsRowStatus = D_snmpTargetParamsRowStatus_active;
      }
      else
      {
        ((snmpTargetParamsEntry_t*)agt_snmpTargetParamsTable.tp[index])->snmpTargetParamsRowStatus = D_snmpTargetParamsRowStatus_notInService;
      }
      SET_VALID(I_snmpTargetParamsRowStatus, ((snmpTargetParamsEntry_t*)agt_snmpTargetParamsTable.tp[index])->valid);

      CLR_VALID(I_snmpTargetParamsRowStatus, data->valid);
    }
  }

  FreeOctetString(agt_snmpTargetParamsTable.tip[0].value.octet_val);

  if (index >= 0)
  {
    if (VALID(I_snmpTargetParamsRowStatus, data->valid) && 
        data->snmpTargetParamsRowStatus == D_snmpTargetParamsRowStatus_destroy)
    {
      RemoveTableEntry(&agt_snmpTargetParamsTable, index);
      return L7_SUCCESS;
    }

    snmpTargetParamsEntryData = (snmpTargetParamsEntry_t*)agt_snmpTargetParamsTable.tp[index];

    if (VALID(I_snmpTargetParamsMPModel, data->valid))
    {
      snmpTargetParamsEntryData->snmpTargetParamsMPModel = data->snmpTargetParamsMPModel;
      SET_VALID(I_snmpTargetParamsMPModel, snmpTargetParamsEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_snmpTargetParamsSecurityModel, data->valid))
    {
      snmpTargetParamsEntryData->snmpTargetParamsSecurityModel = data->snmpTargetParamsSecurityModel;
      SET_VALID(I_snmpTargetParamsSecurityModel, snmpTargetParamsEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_snmpTargetParamsSecurityName, data->valid))
    {
      if (snmpTargetParamsEntryData->snmpTargetParamsSecurityName != NULL)
      {
        FreeOctetString(snmpTargetParamsEntryData->snmpTargetParamsSecurityName);
        CLR_VALID(I_snmpTargetParamsSecurityName, snmpTargetParamsEntryData->valid);
      }

      snmpTargetParamsEntryData->snmpTargetParamsSecurityName = CloneOctetString(data->snmpTargetParamsSecurityName);

      if (snmpTargetParamsEntryData->snmpTargetParamsSecurityName != NULL)
      {
        SET_VALID(I_snmpTargetParamsSecurityName, snmpTargetParamsEntryData->valid);
        writeConfigFileFlag = TRUE;
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    if (VALID(I_snmpTargetParamsSecurityLevel, data->valid))
    {
      snmpTargetParamsEntryData->snmpTargetParamsSecurityLevel = data->snmpTargetParamsSecurityLevel;
      SET_VALID(I_snmpTargetParamsSecurityLevel, snmpTargetParamsEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_snmpTargetParamsStorageType, data->valid))
    {
      if (l7_snmpStorageTypeSetValidate(snmpTargetParamsEntryData->snmpTargetParamsStorageType,
                                        data->snmpTargetParamsStorageType) != L7_SUCCESS)
        return L7_ERROR;

      snmpTargetParamsEntryData->snmpTargetParamsStorageType = data->snmpTargetParamsStorageType;
      SET_VALID(I_snmpTargetParamsStorageType, snmpTargetParamsEntryData->valid);
      writeConfigFileFlag = TRUE;
    }

    if (VALID(I_snmpTargetParamsRowStatus, data->valid))
    {
      if (l7_snmpRowStatusSetValidate(snmpTargetParamsEntryData->snmpTargetParamsRowStatus,
                                      data->snmpTargetParamsRowStatus) != L7_SUCCESS)
        return L7_ERROR;

      snmpTargetParamsEntryData->snmpTargetParamsRowStatus = data->snmpTargetParamsRowStatus;
      SET_VALID(I_snmpTargetParamsRowStatus, snmpTargetParamsEntryData->valid);
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
L7_RC_t l7_snmpTargetParamsTableCheckValid(L7_char8 *snmpTargetParamsName)
{
  snmpTargetParamsEntry_t * snmpTargetParamsEntryData = snmp_snmpTargetParamsTable_get(EXACT, snmpTargetParamsName);

  if (snmpTargetParamsEntryData != NULL)
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
L7_RC_t l7_snmpTargetParamsTableNextGet(L7_char8 *snmpTargetParamsName)
{
  snmpTargetParamsEntry_t * snmpTargetParamsEntryData = snmp_snmpTargetParamsTable_get(NEXT, snmpTargetParamsName);

  if (snmpTargetParamsEntryData != NULL)
  {
    strncpy(snmpTargetParamsName, snmpTargetParamsEntryData->snmpTargetParamsName->octet_ptr,
            snmpTargetParamsEntryData->snmpTargetParamsName->length);
    snmpTargetParamsName[snmpTargetParamsEntryData->snmpTargetParamsName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsMPModel
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The Message Processing Model to be used when generating
*        SNMP messages using this entry.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetParamsMPModelGet(L7_char8* snmpTargetParamsName, L7_uint32 *val)
{
  snmpTargetParamsEntry_t * snmpTargetParamsEntryData = snmp_snmpTargetParamsTable_get(EXACT, snmpTargetParamsName);

  if (snmpTargetParamsEntryData != NULL && 
      VALID(I_snmpTargetParamsMPModel, snmpTargetParamsEntryData->valid))
  {
    *val = snmpTargetParamsEntryData->snmpTargetParamsMPModel;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsMPModel
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The Message Processing Model to be used when generating
*        SNMP messages using this entry.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetParamsMPModelSet(L7_char8* snmpTargetParamsName, L7_uint32 val)
{
  snmpTargetParamsEntry_t snmpTargetParamsEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpTargetParamsEntryData, 0, sizeof(snmpTargetParamsEntryData));

  snmpTargetParamsEntryData.snmpTargetParamsName = MakeOctetStringFromText(snmpTargetParamsName);
  SET_VALID(I_snmpTargetParamsName, snmpTargetParamsEntryData.valid);

  snmpTargetParamsEntryData.snmpTargetParamsMPModel = val;
  SET_VALID(I_snmpTargetParamsMPModel, snmpTargetParamsEntryData.valid);

  rc = snmp_snmpTargetParamsTable_set(&snmpTargetParamsEntryData);

  FreeOctetString(snmpTargetParamsEntryData.snmpTargetParamsName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsSecurityModel
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The Security Model to be used when generating SNMP
*         messages using this entry.  An implementation may
*         choose to return an inconsistentValue error if an
*         attempt is made to set this variable to a value
*         for a security model which the implementation does
*         not support.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetParamsSecurityModelGet(L7_char8* snmpTargetParamsName, L7_uint32 *val)
{
  snmpTargetParamsEntry_t * snmpTargetParamsEntryData = snmp_snmpTargetParamsTable_get(EXACT, snmpTargetParamsName);

  if (snmpTargetParamsEntryData != NULL && 
      VALID(I_snmpTargetParamsSecurityModel, snmpTargetParamsEntryData->valid))
  {
    *val = snmpTargetParamsEntryData->snmpTargetParamsSecurityModel;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsSecurityModel
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The Security Model to be used when generating SNMP
*         messages using this entry.  An implementation may
*         choose to return an inconsistentValue error if an
*         attempt is made to set this variable to a value
*         for a security model which the implementation does
*         not support.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetParamsSecurityModelSet(L7_char8* snmpTargetParamsName, L7_uint32 val)
{
  snmpTargetParamsEntry_t snmpTargetParamsEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpTargetParamsEntryData, 0, sizeof(snmpTargetParamsEntryData));

  snmpTargetParamsEntryData.snmpTargetParamsName = MakeOctetStringFromText(snmpTargetParamsName);
  SET_VALID(I_snmpTargetParamsName, snmpTargetParamsEntryData.valid);

  snmpTargetParamsEntryData.snmpTargetParamsSecurityModel = val;
  SET_VALID(I_snmpTargetParamsSecurityModel, snmpTargetParamsEntryData.valid);

  rc = snmp_snmpTargetParamsTable_set(&snmpTargetParamsEntryData);

  FreeOctetString(snmpTargetParamsEntryData.snmpTargetParamsName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsSecurityName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The securityName which identifies the Principal on
*        whose behalf SNMP messages will be generated using
*        this entry.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetParamsSecurityNameGet(L7_char8* snmpTargetParamsName, L7_char8 *val)
{
  snmpTargetParamsEntry_t * snmpTargetParamsEntryData = snmp_snmpTargetParamsTable_get(EXACT, snmpTargetParamsName);

  if (snmpTargetParamsEntryData != NULL && 
      VALID(I_snmpTargetParamsSecurityName, snmpTargetParamsEntryData->valid))
  {
    strncpy(val, snmpTargetParamsEntryData->snmpTargetParamsSecurityName->octet_ptr,
            snmpTargetParamsEntryData->snmpTargetParamsSecurityName->length);
    val[snmpTargetParamsEntryData->snmpTargetParamsSecurityName->length] = '\0';
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsSecurityName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The securityName which identifies the Principal on
*        whose behalf SNMP messages will be generated using
*        this entry.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetParamsSecurityNameSet(L7_char8* snmpTargetParamsName, L7_char8 *val)
{
  snmpTargetParamsEntry_t snmpTargetParamsEntryData;
  L7_RC_t rc = L7_FAILURE;

  if (l7_snmpStringSetValidate(val, 1, 32) != L7_SUCCESS)
    return rc;

  memset((void*)&snmpTargetParamsEntryData, 0, sizeof(snmpTargetParamsEntryData));

  snmpTargetParamsEntryData.snmpTargetParamsName = MakeOctetStringFromText(snmpTargetParamsName);
  SET_VALID(I_snmpTargetParamsName, snmpTargetParamsEntryData.valid);

  snmpTargetParamsEntryData.snmpTargetParamsSecurityName = MakeOctetStringFromText(val);
  SET_VALID(I_snmpTargetParamsSecurityName, snmpTargetParamsEntryData.valid);

  rc = snmp_snmpTargetParamsTable_set(&snmpTargetParamsEntryData);

  FreeOctetString(snmpTargetParamsEntryData.snmpTargetParamsName);
  FreeOctetString(snmpTargetParamsEntryData.snmpTargetParamsSecurityName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsSecurityLevel
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The Level of Security to be used when generating
*        SNMP messages using this entry.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetParamsSecurityLevelGet(L7_char8* snmpTargetParamsName, L7_uint32 *val)
{
  snmpTargetParamsEntry_t * snmpTargetParamsEntryData = snmp_snmpTargetParamsTable_get(EXACT, snmpTargetParamsName);

  if (snmpTargetParamsEntryData != NULL && 
      VALID(I_snmpTargetParamsSecurityLevel, snmpTargetParamsEntryData->valid))
  {
    *val = snmpTargetParamsEntryData->snmpTargetParamsSecurityLevel;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsSecurityLevel
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The Level of Security to be used when generating
*        SNMP messages using this entry.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetParamsSecurityLevelSet(L7_char8* snmpTargetParamsName, L7_uint32 val)
{
  snmpTargetParamsEntry_t snmpTargetParamsEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpTargetParamsEntryData, 0, sizeof(snmpTargetParamsEntryData));

  snmpTargetParamsEntryData.snmpTargetParamsName = MakeOctetStringFromText(snmpTargetParamsName);
  SET_VALID(I_snmpTargetParamsName, snmpTargetParamsEntryData.valid);

  snmpTargetParamsEntryData.snmpTargetParamsSecurityLevel = val;
  SET_VALID(I_snmpTargetParamsSecurityLevel, snmpTargetParamsEntryData.valid);

  rc = snmp_snmpTargetParamsTable_set(&snmpTargetParamsEntryData);

  FreeOctetString(snmpTargetParamsEntryData.snmpTargetParamsName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetParamsStorageTypeGet(L7_char8* snmpTargetParamsName, l7_snmpStorageType_t *val)
{
  snmpTargetParamsEntry_t * snmpTargetParamsEntryData = snmp_snmpTargetParamsTable_get(EXACT, snmpTargetParamsName);

  if (snmpTargetParamsEntryData != NULL && 
      VALID(I_snmpTargetParamsStorageType, snmpTargetParamsEntryData->valid))
  {
    *val = snmpTargetParamsEntryData->snmpTargetParamsStorageType;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetParamsStorageTypeSet(L7_char8* snmpTargetParamsName, l7_snmpStorageType_t val)
{
  snmpTargetParamsEntry_t snmpTargetParamsEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpTargetParamsEntryData, 0, sizeof(snmpTargetParamsEntryData));

  snmpTargetParamsEntryData.snmpTargetParamsName = MakeOctetStringFromText(snmpTargetParamsName);
  SET_VALID(I_snmpTargetParamsName, snmpTargetParamsEntryData.valid);

  snmpTargetParamsEntryData.snmpTargetParamsStorageType = val;
  SET_VALID(I_snmpTargetParamsStorageType, snmpTargetParamsEntryData.valid);

  rc = snmp_snmpTargetParamsTable_set(&snmpTargetParamsEntryData);

  FreeOctetString(snmpTargetParamsEntryData.snmpTargetParamsName);

  return rc;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsRowStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the snmpTargetParamsRowStatus
*        column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until the corresponding
*        snmpTargetParamsMPModel,
*        snmpTargetParamsSecurityModel,
*        snmpTargetParamsSecurityName,
*        and snmpTargetParamsSecurityLevel have all been set.
*        The following objects may not be modified while the
*        value of this object is active(1):
*        An attempt to set these objects while the value of
*        snmpTargetParamsRowStatus is active(1) will result in
*        an inconsistentValue error.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetParamsRowStatusGet(L7_char8* snmpTargetParamsName, l7_snmpRowStatus_t *val)
{
  snmpTargetParamsEntry_t * snmpTargetParamsEntryData = snmp_snmpTargetParamsTable_get(EXACT, snmpTargetParamsName);

  if (snmpTargetParamsEntryData != NULL && 
      VALID(I_snmpTargetParamsRowStatus, snmpTargetParamsEntryData->valid))
  {
    *val = snmpTargetParamsEntryData->snmpTargetParamsRowStatus;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsRowStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the snmpTargetParamsRowStatus
*        column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until the corresponding
*        snmpTargetParamsMPModel,
*        snmpTargetParamsSecurityModel,
*        snmpTargetParamsSecurityName,
*        and snmpTargetParamsSecurityLevel have all been set.
*        The following objects may not be modified while the
*        value of this object is active(1):
*        An attempt to set these objects while the value of
*        snmpTargetParamsRowStatus is active(1) will result in
*        an inconsistentValue error.
*
* @end
*
*********************************************************************/
L7_RC_t l7_snmpTargetParamsRowStatusSet(L7_char8* snmpTargetParamsName, l7_snmpRowStatus_t val)
{
  snmpTargetParamsEntry_t snmpTargetParamsEntryData;
  L7_RC_t rc = L7_FAILURE;

  memset((void*)&snmpTargetParamsEntryData, 0, sizeof(snmpTargetParamsEntryData));

  if (l7_snmpStringSetValidate(snmpTargetParamsName, 1, 32) != L7_SUCCESS)
    return rc;

  snmpTargetParamsEntryData.snmpTargetParamsName = MakeOctetStringFromText(snmpTargetParamsName);
  SET_VALID(I_snmpTargetParamsName, snmpTargetParamsEntryData.valid);

  snmpTargetParamsEntryData.snmpTargetParamsRowStatus = val;
  SET_VALID(I_snmpTargetParamsRowStatus, snmpTargetParamsEntryData.valid);

  rc = snmp_snmpTargetParamsTable_set(&snmpTargetParamsEntryData);

  FreeOctetString(snmpTargetParamsEntryData.snmpTargetParamsName);

  return rc;
}


void
debug_snmpTargetParamsTable_walk()
{
  L7_char8 snmpTargetParamsName[256];
  L7_char8 temp_string[256];
  L7_uint32 temp_val;

  memset((void*)snmpTargetParamsName, 0, sizeof(snmpTargetParamsName));

  sysapiPrintf("[snmpTargetParamsTable start]\n");

  while(l7_snmpTargetParamsTableNextGet(snmpTargetParamsName) == L7_SUCCESS)
  {
    sysapiPrintf("snmpTargetParamsName.\"%s\" = \"%s\"\n", snmpTargetParamsName, snmpTargetParamsName);

    if (l7_snmpTargetParamsMPModelGet(snmpTargetParamsName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpTargetParamsMPModel.\"%s\" = %d\n", snmpTargetParamsName, temp_val);
    else
      sysapiPrintf("snmpTargetParamsMPModel.\"%s\" = L7_FAILURE\n", snmpTargetParamsName);

    if (l7_snmpTargetParamsSecurityModelGet(snmpTargetParamsName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpTargetParamsSecurityModel.\"%s\" = %d\n", snmpTargetParamsName, temp_val);
    else
      sysapiPrintf("snmpTargetParamsSecurityModel.\"%s\" = L7_FAILURE\n", snmpTargetParamsName);

    if (l7_snmpTargetParamsSecurityNameGet(snmpTargetParamsName, temp_string) == L7_SUCCESS)
      sysapiPrintf("snmpTargetParamsSecurityName.\"%s\" = \"%s\"\n", snmpTargetParamsName, temp_string);
    else
      sysapiPrintf("snmpTargetParamsSecurityName.\"%s\" = L7_FAILURE\n", snmpTargetParamsName);

    if (l7_snmpTargetParamsSecurityLevelGet(snmpTargetParamsName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpTargetParamsSecurityLevel.\"%s\" = %d\n", snmpTargetParamsName, temp_val);
    else
      sysapiPrintf("snmpTargetParamsSecurityLevel.\"%s\" = L7_FAILURE\n", snmpTargetParamsName);

    if (l7_snmpTargetParamsStorageTypeGet(snmpTargetParamsName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpTargetParamsStorageType.\"%s\" = %d\n", snmpTargetParamsName, temp_val);
    else
      sysapiPrintf("snmpTargetParamsStorageType.\"%s\" = L7_FAILURE\n", snmpTargetParamsName);

    if (l7_snmpTargetParamsRowStatusGet(snmpTargetParamsName, &temp_val) == L7_SUCCESS)
      sysapiPrintf("snmpTargetParamsRowStatus.\"%s\" = %d\n", snmpTargetParamsName, temp_val);
    else
      sysapiPrintf("snmpTargetParamsRowStatus.\"%s\" = L7_FAILURE\n", snmpTargetParamsName);

    sysapiPrintf("\n");
  }
  sysapiPrintf("[end]\n");
}

void
debug_snmpTargetParamsTable_create(L7_char8* snmpTargetParamsName)
{
  L7_RC_t rc;

  rc = l7_snmpTargetParamsRowStatusSet(snmpTargetParamsName, snmpRowStatus_createAndWait);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpTargetParamsRowStatusSet 1 failed!\n");
    return;
  }

  rc = l7_snmpTargetParamsMPModelSet(snmpTargetParamsName, 1);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpTargetParamsMPModelSet 1 failed!\n");
    return;
  }

  rc = l7_snmpTargetParamsSecurityModelSet(snmpTargetParamsName, 1);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpTargetParamsSecurityModelSet 1 failed!\n");
    return;
  }

  rc = l7_snmpTargetParamsSecurityNameSet(snmpTargetParamsName, "test");
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpTargetParamsSecurityNameSet 1 failed!\n");
    return;
  }

  rc = l7_snmpTargetParamsSecurityLevelSet(snmpTargetParamsName, 1);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpTargetParamsSecurityLevelSet 1 failed!\n");
    return;
  }

  rc = l7_snmpTargetParamsRowStatusSet(snmpTargetParamsName, snmpRowStatus_active);
  if (rc != L7_SUCCESS)
  {
    sysapiPrintf("l7_snmpTargetParamsRowStatusSet 2 failed!\n");
    return;
  }

  sysapiPrintf("debug_snmpTargetParamsTable_create finished!\n");
}
