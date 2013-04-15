#include "k_private_base.h"
#include "usmdb_cmd_logger_api.h"
#include "strlib_base_common.h"
#include "usmdb_sim_api.h"
#include "inst_lib.h"
#include "osapi_support.h"

void snmpFree(void * memory)
{
  if (memory != NULL) 
  { 
    osapiFree(L7_SNMP_COMPONENT_ID, memory);
  }
}

void *snmpMalloc(L7_uint32 nbytes)
{
  return osapiMalloc(L7_SNMP_COMPONENT_ID, nbytes);
}

L7_BOOL
SafeMakeOctetString_track(OctetString **os_ptr, L7_uchar8 *string, L7_uint32 length, char * file, unsigned int line)
{
  OctetString *temp_os_ptr;

  temp_os_ptr = MakeOctetString_track(string, length, file, line);

  if (temp_os_ptr != NULL)
  {
    FreeOctetString(*os_ptr);
    *os_ptr = temp_os_ptr;
    return L7_TRUE;
  }

  return L7_FALSE;
}

L7_BOOL
SafeMakeOctetStringFromText_track(OctetString **os_ptr, L7_uchar8 *string, char * file, unsigned int line)
{
  OctetString *temp_os_ptr;

  temp_os_ptr = MakeOctetStringFromText_track(string, file, line);

  if (temp_os_ptr != NULL)
  {
    FreeOctetString(*os_ptr);
    *os_ptr = temp_os_ptr;
    return L7_TRUE;
  }

  return L7_FALSE;
}

L7_BOOL
SafeMakeOctetStringFromTextExact_track(OctetString **os_ptr, L7_uchar8 *string, char * file, unsigned int line)
{
  return SafeMakeOctetString_track(os_ptr, string, strlen(string), file, line);
}

L7_BOOL
SafeMakeOIDFromDot_track(OID **oid_ptr, L7_uchar8 *string, char * file, unsigned int line)
{
  OID *temp_oid_ptr;

  temp_oid_ptr = MakeOIDFromDot_track(string, file, line);

  if (temp_oid_ptr != NULL)
  {
    FreeOID(*oid_ptr);
    *oid_ptr = temp_oid_ptr;
    return L7_TRUE;
  }
  return L7_FALSE;
}

L7_BOOL
SafeMakeTimeTicksFromSeconds(L7_uint32 seconds, L7_uint32 *time_ticks)
{
    *time_ticks = seconds * 100;
    return L7_TRUE;
}

L7_BOOL
SafeMakeDateAndTime_track(OctetString **os_ptr, L7_uint32 time, char * file, unsigned int line)
{
  OctetString *temp_os_ptr;

  struct tm *tm;

  tm = localtime((void *)&time);

  temp_os_ptr = MakeDateAndTime_track(tm, file, line);

  if (temp_os_ptr != NULL)
  {
    FreeOctetString(*os_ptr);
    *os_ptr = temp_os_ptr;
    return L7_TRUE;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Add an entry for auditing
*
* @param    L7_char8 *strInput  - input string
* @param    L7_char8 *userName  - user name for SNMP v3 , community otherwise
* @param    L7_char8 *ipAddr    - string that contain IP address
*
* @returns 
*
* @notes
*
* @end
*********************************************************************/
void SnmpCmdLoggerAuditEntryAdd(ContextInfo *contextInfo, const L7_char8 *strInput)
{
  L7_char8 auditMsg[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_char8 ipAddr[SNMP_IP4_STR_LEN];
  L7_char8 *name = L7_NULLPTR;
  ObjectSyntax *os = L7_NULLPTR;

  memset(ipAddr, L7_NULL, sizeof(ipAddr));
  memset(auditMsg, L7_NULL, sizeof(auditMsg));

  if (contextInfo != L7_NULLPTR)
  {
    os = SearchContextInfo(contextInfo, NCI_MESSAGE_PROCESSING_MODEL);
    switch (os->sl_value)
    {
#ifdef SR_SNMPv1_WRAPPER
#ifdef SR_SNMPv1_PACKET
      case SR_SNMPv1_MESSAGE_PROCESSING_MODEL:
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
      case SR_SNMPv2c_MESSAGE_PROCESSING_MODEL:
#endif /* SR_SNMPv2c_PACKET */
        os = SearchContextInfo(contextInfo, NCI_COMMUNITY);
        if (os != L7_NULLPTR)
        {
          name = OctetStringToTextString(os->os_value);
        }
      break;
#endif /* SR_SNMPv1_WRAPPER */
#ifdef SR_SNMPv3_PACKET
      case SR_SNMPv3_MESSAGE_PROCESSING_MODEL:
        os = SearchContextInfo(contextInfo, NCI_SECURITY_NAME);
        if (os != L7_NULLPTR)
        {
          name = OctetStringToTextString(os->os_value);
        }
      break;
#endif /* SR_SNMPv3_PACKET */
    }

    os = SearchContextInfo(contextInfo, NCI_SENDER_TRANSPORT);
    if (os != L7_NULLPTR)
    {
      if (os->os_value != L7_NULLPTR)
      {
        osapiInetNtoa(OctetStringToIP(os->os_value), ipAddr);
        if ((name == L7_NULLPTR) || (strlen(name) == L7_NULL))
        {
          snprintf(auditMsg, sizeof(auditMsg) - 1, 
                   "[SNMP:<<UNKNOWN>>:%s] %s", ipAddr, strInput);
        }
        else
        { 
          snprintf(auditMsg, sizeof(auditMsg) - 1, 
                   "[SNMP:%s:%s] %s", name, ipAddr, strInput);
        }
        usmDbCmdLoggerAuditEntryAdd(L7_CLI_WEB_COMPONENT_ID, auditMsg);
      }
    }
    if (name != L7_NULLPTR)
    {
      free(name);
    }
  }
}

/*********************************************************************
 * @purpose  Helper function for retrieving transfer info during 
 *           upload/download file to/from the switch
 *
 * @param    isUpload   determine if upload/download
 *                      transfer info should be retrieved
 * @param    outputStr  pointer to string where transfer info 
 *                      will be stored
 * @param    size       maximum size of outputStr
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t snmpTransferInfoGet(L7_BOOL isUpload, L7_char8 *outputStr, L7_uint32 size)
{
  L7_uint32 family;
  L7_char8 remoteFilePath[L7_MAX_FILEPATH + 1];
  L7_char8 remoteFileName[L7_MAX_FILENAME + 1];
  L7_char8 localFilePath[L7_MAX_FILEPATH + 1];
  L7_char8 localFileName[L7_MAX_FILEPATH + 1];
  L7_inet_addr_t inetAddr;
  L7_char8 serverAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 auditMsg[L7_CLI_MAX_STRING_LENGTH];

  memset(remoteFilePath, L7_NULL, sizeof(remoteFilePath));
  memset(remoteFileName, L7_NULL, sizeof(remoteFileName));
  memset(localFilePath, L7_NULL, sizeof(localFilePath));
  memset(localFileName, L7_NULL, sizeof(localFileName));
  memset(serverAddr, L7_NULL, sizeof(serverAddr));
  memset(auditMsg, L7_NULL, sizeof(auditMsg));
  memset(&inetAddr, 0, sizeof(inetAddr));

  if (usmDbTransferFileNameRemoteGet(USMDB_UNIT_CURRENT, remoteFileName) != L7_SUCCESS ||
      usmDbTransferFilePathRemoteGet(USMDB_UNIT_CURRENT, remoteFilePath) != L7_SUCCESS ||
      usmDbTransferFileNameLocalGet(USMDB_UNIT_CURRENT, localFileName) != L7_SUCCESS ||
      usmDbTransferFilePathLocalGet(USMDB_UNIT_CURRENT, localFilePath) != L7_SUCCESS ||
      usmDbTransferServerAddressTypeGet(USMDB_UNIT_CURRENT, &family) != L7_SUCCESS ||
      usmDbTransferServerAddressGet(USMDB_UNIT_CURRENT, (L7_uchar8 *)&inetAddr.addr) != L7_SUCCESS ||
      osapiInetNtop(family, (L7_uchar8 *)&inetAddr.addr, serverAddr, sizeof(serverAddr)) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (isUpload == L7_TRUE) 
  {
    osapiSnprintf(outputStr, size, pStrInfo_base_Auditing_FileUpload,
                  localFilePath, localFileName, 
                  serverAddr, remoteFilePath, remoteFileName);
  }
  else
  {
    osapiSnprintf(auditMsg, sizeof(auditMsg), pStrInfo_base_Auditing_FileDownLoad,
                  localFilePath, localFileName, 
                  serverAddr, remoteFilePath, remoteFileName);
  }
  return L7_SUCCESS;
}

