/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename radius_debug.c
*
* @purpose RADIUS Client debug utilities
*
* @component radius
*
* @comments none
*
* @create 03/28/2003
*
* @author spetriccione
*
* @end
*             
**********************************************************************/
#include "radius_include.h"
#include "commdefs.h"

#include "usmdb_radius_api.h"

extern radiusCfg_t radiusCfg;

/* Mutex for shared data. */
extern void * radiusTaskSyncSema;

static L7_uchar8 radiusDebugLevel[RD_LEVEL_LAST];
int  radiusDebugLevelShow(void);

/*********************************************************************
*
* @purpose Do any initilization required for the debug utilities.
*          
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusDebugInit(void)
{

  /* Register for debug user requests callback */
  if (radiusResponseRegister(L7_RADIUS_COMPONENT_ID, radiusDebugCallback) != L7_SUCCESS)
  {
    LOG_MSG("RADIUS: Unable to register for response callback\n");
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  List the RADIUS Client Debug commands availiable
*          
* @param @b((input))
* @param @b((output))
*
* @returns L7_SUCCESS  
*
* @comments devshell command
*
* @end
*
*********************************************************************/
L7_RC_t radiusDebugHelp()
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "Available RADIUS Client Debug commands:\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "-------------------------------\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void radiusDebugSizesShow()\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "L7_RC_t radiusDebugUserAuth(L7_uint32 correlator)\r\n");

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Display the sizes of the RADIUS Client structures
*          
* @param @b((input))
* @param @b((output))
*
* @returns void  
*
* @comments devshell command
*
* @end
*
*********************************************************************/
void radiusDebugSizesShow()
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "RADIUS Client Data Structures:\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "------------------------------\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "sizeof radiusCfg_t = %d\r\n", sizeof(radiusCfg_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "sizeof radiusValuePair_t = %d\r\n", sizeof(radiusValuePair_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "sizeof radiusRequest_t = %d\r\n", sizeof(radiusRequest_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "sizeof radiusServerEntry_t = %d\r\n", sizeof(radiusServerEntry_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "sizeof radiusDictValue_t = %d\r\n", sizeof(radiusDictValue_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "sizeof radiusRequestInfo_t = %d\r\n", sizeof(radiusRequestInfo_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "RADIUS_MAX_MESSAGE_LENGTH = %d\r\n", RADIUS_MAX_MESSAGE_LENGTH);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "RADIUS_MAX_TOKEN_SIZE = %d\r\n", RADIUS_MAX_TOKEN_SIZE);

}

/*********************************************************************
*
* @purpose Test the different RADIUS API's
*          
* @param correlator @b((input)) to correlate request with response
* @param @b((output))
*
* @returns void  
*
* @comments devshell command
*
* @end
*
*********************************************************************/
L7_RC_t radiusDebugUserAuth(L7_uint32 correlator, 
                            L7_uint32 type, 
                            L7_char8 *serverName, 
                            L7_uint32 testCase,
                            L7_uint32 ipAddrType,
                            L7_uchar8 *ipAddr)
{
  L7_uint32 value;
  radiusValuePair_t *vpList = L7_NULL;
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];
  L7_char8 tmpIpAddr[32];/*dummy*/
  L7_IP_ADDRESS_TYPE_t pType;/*dummy*/
  
  memset(name,0,sizeof(name));
  osapiStrncpySafe(name,"def",sizeof(name)); 

  if(RADIUS_SERVER_TYPE_AUTH == type)  
  {       
     if(osapiStrncmp(serverName,name,sizeof(name)) == 0 )
     {
       memset(name,0,sizeof(name));
       osapiStrncpySafe(name,L7_RADIUS_SERVER_DEFAULT_NAME_AUTH,sizeof(name));
     }
     else
     {
       osapiStrncpySafe(name,serverName,sizeof(name));
     }
     if( radiusNamedServerAddrGet(name, L7_RADIUS_SERVER_USAGE_ALL, tmpIpAddr, &pType) != L7_SUCCESS )
     {
       sysapiPrintf("\n named server:'%s' doesn't exist, \
                     Failed to run test cases..\n",name);
       return L7_FAILURE;
     }
  }
  else if(RADIUS_SERVER_TYPE_ACCT == type) 
  {
     if(osapiStrncmp(serverName,name,sizeof(name)) == 0 )
     {
       memset(name,0,sizeof(name));
       osapiStrncpySafe(name,L7_RADIUS_SERVER_DEFAULT_NAME_ACCT,sizeof(name));
     }
     else
     {
       osapiStrncpySafe(name,serverName,sizeof(name));
     }
     if( radiusNamedAcctServerGet(name) != L7_SUCCESS )
     {
       sysapiPrintf("\n named server:'%s' doesn't exist, \
                     Failed to run test cases..\n",name);
       return L7_FAILURE;
     }     
  }
  else
  {
    sysapiPrintf("\n Incorrect server Type mentioned: %d\n",type);
    return L7_FAILURE;
  }

  /* Test cases 1 through 5 uses default auth/acct. server while rest 
     communicates to the specified named servers */
  switch(testCase)
  {
    case 1:          /* Test Authentication Request - 1 */
      radiusAttrValuePairListInit(&vpList);
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_USER_NAME, L7_NULL, "testuser1", 9);
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_USER_PASSWORD, L7_NULL, "testpass1", 9);
      value = RADIUS_SERVICE_TYPE_NAS_PROMPT; 
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_SERVICE_TYPE, L7_NULL, &value, 4);
      value = 0xFFFFFFFF; 
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_NAS_IDENTIFIER, L7_NULL, &value, 4);
    
      (void)radiusAccessRequestSend(vpList, correlator++, L7_RADIUS_COMPONENT_ID);
       break;

    case 2:          /* Test Authentication Request - 2 */      
      (void)radiusUserAuthenticate("testuser2", 
                                   "testpass2", 
                                   "state string for testuser1", 
                                   correlator++, 
                                   L7_RADIUS_COMPONENT_ID);
       break;

    case 3:          /* Test Authentication Request - 3 */
      radiusAttrValuePairListInit(&vpList);
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_USER_NAME, L7_NULL, "testuser3", 9);
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_USER_PASSWORD, L7_NULL, "testpass3", 9);
      value = RADIUS_SERVICE_TYPE_ADMIN;
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_SERVICE_TYPE, L7_NULL, &value, 4);
      value = 0x00000000; 
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_NAS_IDENTIFIER, L7_NULL, &value, 4);
    
      (void)radiusAccessRequestSend(vpList, correlator++, L7_RADIUS_COMPONENT_ID);
       break;

    case 4:          /* Test Authentication Request - 4 */
      (void)radiusUserAuthenticate("testuser4", 
                                   "testpass4", 
                                   L7_NULL,  /* Null state */
                                   correlator++, 
                                   L7_RADIUS_COMPONENT_ID);
       break;

    case 5:          /* Test Accounting Request - 1 */
      radiusAttrValuePairListInit(&vpList);
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_USER_NAME, L7_NULL, "AcctUser1", 9);
    
      (void)radiusAccountingStart("session1", 1, vpList);
    
      radiusAttrValuePairListInit(&vpList);
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_USER_NAME, L7_NULL, "AcctUser1", 9);
      value = 10001, radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_INPUT_OCTETS, L7_NULL, &value, 4);
      value = 10002, radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_OUTPUT_OCTETS, L7_NULL, &value, 4);
      value = 101, radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_INPUT_PACKETS, L7_NULL, &value, 4);
      value = 102, radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_OUTPUT_PACKETS, L7_NULL, &value, 4);
      value = 1001, radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_SESSION_TIME, L7_NULL, &value, 4); 
      value = 0, radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_DELAY_TIME, L7_NULL, &value, 4);
      value = RADIUS_ACCT_TERM_CAUSE_USER_REQUEST;
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_TERMINATE_CAUSE, L7_NULL, &value, 4);
    
      (void)radiusAccountingStop("session1", 1, vpList);
      break;

    case 6:          /* Test Authentication Request - 1 */
      radiusAttrValuePairListInit(&vpList);
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_USER_NAME, L7_NULL, "Namedtestuser1", 14);
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_USER_PASSWORD, L7_NULL, "Namedtestpass1", 14);
      value = RADIUS_SERVICE_TYPE_NAS_PROMPT; 
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_SERVICE_TYPE, L7_NULL, &value, 4);
      value = 0xFFFFFFFF; 
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_NAS_IDENTIFIER, L7_NULL, &value, 4);

      (void)radiusAccessRequestNamedSend(vpList, correlator++, L7_RADIUS_COMPONENT_ID,serverName,ipAddrType,ipAddr);
      break;

    case 7:          /* Test Authentication Request - 2 */      
        (void)radiusNamedUserAuthenticate("Namedestuser2", 
                                       "Namedtestpass2", 
                                       "state string for testuser1", 
                                       correlator++, 
                                       L7_RADIUS_COMPONENT_ID,
                                       serverName,
                                       ipAddrType,
                                       ipAddr);
       break;

    case 8:          /* Test Authentication Request - 3 */
      radiusAttrValuePairListInit(&vpList);
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_USER_NAME, L7_NULL, "Namedtestuser3", 14);
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_USER_PASSWORD, L7_NULL, "Namedtestpass3", 14);
      value = RADIUS_SERVICE_TYPE_ADMIN;
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_SERVICE_TYPE, L7_NULL, &value, 4);
      value = 0x00000000; 
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_NAS_IDENTIFIER, L7_NULL, &value, 4);
        (void)radiusAccessRequestNamedSend(vpList, correlator++, L7_RADIUS_COMPONENT_ID,serverName,ipAddrType,ipAddr);
       break;

    case 9:          /* Test Authentication Request - 4 */
        (void)radiusNamedUserAuthenticate("Namedtestuser4", 
                                   "Namedtestpass4", 
                                   L7_NULL,  /* Null state */
                                   correlator++, 
                                   L7_RADIUS_COMPONENT_ID,
                                   serverName,
                                   ipAddrType,
                                   ipAddr);
       break;

    case 10:          /* Test Accounting Request - 1 */
      radiusAttrValuePairListInit(&vpList);
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_USER_NAME, L7_NULL, "NamedAcctUser1", 14);
        (void)radiusAccountingNamedStart("session1", 1, vpList,name,ipAddrType,ipAddr);

    
      radiusAttrValuePairListInit(&vpList);
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_USER_NAME, L7_NULL, "NamedAcctUser1", 14);
      value = 10001, radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_INPUT_OCTETS, L7_NULL, &value, 4);
      value = 10002, radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_OUTPUT_OCTETS, L7_NULL, &value, 4);
      value = 101, radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_INPUT_PACKETS, L7_NULL, &value, 4);
      value = 102, radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_OUTPUT_PACKETS, L7_NULL, &value, 4);
      value = 1001, radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_SESSION_TIME, L7_NULL, &value, 4); 
      value = 0, radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_DELAY_TIME, L7_NULL, &value, 4);
      value = RADIUS_ACCT_TERM_CAUSE_USER_REQUEST;
      (void)radiusAttrValuePairAdd(&vpList, RADIUS_ATTR_TYPE_ACCT_TERMINATE_CAUSE, L7_NULL, &value, 4);
      (void)radiusAccountingNamedStop("session1", 1, vpList,name,ipAddrType,ipAddr);
      break;

    default:
      sysapiPrintf("\n Undefined test case specified .. \n");
      break;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Callback routine for testing purposes
*          
* @param status @b((input)) 
* @param correlator @b((input))
* @param attributes @b((input))
* @param attributesLen @b((input))
* @param @b((output))
*
* @returns L7_SUCCESS  
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusDebugCallback(L7_uint32 status, 
                            L7_uint32 correlator,
                            L7_uchar8 *attributes,
                            L7_uint32 attributesLen) 
{
  L7_char8 statusString[63];

  if (status == RADIUS_STATUS_SUCCESS)
    strcpy(statusString, "RADIUS_STATUS_SUCCESS");
  else if (status == RADIUS_STATUS_CHALLENGED)
    strcpy(statusString, "RADIUS_STATUS_CHALLENGED");
  else if (status == RADIUS_STATUS_AUTHEN_FAILURE)
    strcpy(statusString, "RADIUS_STATUS_AUTHEN_FAILURE");
  else if (status == RADIUS_STATUS_REQUEST_TIMED_OUT)
    strcpy(statusString, "RADIUS_STATUS_REQUEST_TIMED_OUT");
  else if (status == RADIUS_STATUS_COMM_FAILURE)
    strcpy(statusString, "RADIUS_STATUS_COMM_FAILURE");
  else 
  {
    strcpy(statusString, "Unknown");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "RADIUS: radiusDebugCallback received a response of status %u - %s\n", status, statusString);
    return L7_SUCCESS;
  }


  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Known Status Response:\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "RADIUS: radiusDebugCallback received a response of status %u - %s\n", status, statusString);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "RADIUS: radiusDebugCallback received a correlator %u\n", correlator);

  if (attributes != L7_NULL)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                  "RADIUS: radiusDebugCallback received an attribute list, length=%u\n", attributesLen);
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                  "RADIUS: radiusDebugCallback received a NULL attribute list\n");
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Hex dump of radius packet 
*
* @param    level  @b((input))  If debug level enabled dump the packet
* @param    pkt    @b((input))  buffer containing the message
* @param    len    @b((input))  message length 
*
* @returns  void
*
* @notes    This dumps packets only if radiusPktDump flag is enabled.
*
* @end
*********************************************************************/
void radiusDebugPktDump(L7_uint32 level, L7_uchar8 *pkt, L7_uint32 len)
{
    int data;
    int row = 0 ;

    if (radiusDebugLevelCheck(level) == L7_FALSE)
    {
        return;
    }

    sysapiPrintf("\n=========== START RADIUS PKT DUMP ==============");

    for (data = 0; data < len; data++)
    {
        if ((data %16) == 0 )
        {
            sysapiPrintf("\n");
            sysapiPrintf("%04x ", row*16);
            row++;
        }

        sysapiPrintf("%2.2x ", pkt[data]);
    }

    sysapiPrintf("\n=========== END RADIUS PKT DUMP ==============\n");

    return;

} /* radiusDebugPktDump */

/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void radiusDebugBuildTestConfigData(void)
{

    L7_uint32 i,j,k;
    radiusClient_t *cfgClient;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/
  
    /* default values for the radiusClient structure */
    cfgClient = &radiusCfg.cfg.radiusClient;

    cfgClient->authOperMode = L7_DISABLE;
    cfgClient->acctAdminMode = L7_ENABLE;
    cfgClient->maxNumRetrans = 12;
    cfgClient->timeOutDuration = 7 * RADIUS_TIMER_GRANULARITY;
    cfgClient->radiusServerDeadTime = 10 * RADIUS_TIMER_GRANULARITY; 
    cfgClient->sourceIpAddress      = 0x0a0a0a0a;
   
   for(k=0; k<L7_RADIUS_MAX_SECRET; k++)
   {
        cfgClient->radiusServerSecret[k] = 0xb + k;
   }
    cfgClient->retryPrimaryTime = 455 * RADIUS_TIMER_GRANULARITY;

    for (i = L7_NULL; (i < L7_RADIUS_MAX_SERVERS); i++)
    {
      cfgClient->serverConfigEntries[i].radiusServerConfigIpAddress = 0x0A0A0A0C;
      cfgClient->serverConfigEntries[i].radiusServerConfigUdpPort = 2200 + i;
      cfgClient->serverConfigEntries[i].radiusServerConfigServerType = RADIUS_SERVER_TYPE_AUTH;
      cfgClient->serverConfigEntries[i].radiusServerDeadTime = (i+1)* RADIUS_TIMER_GRANULARITY ; 
      cfgClient->serverConfigEntries[i].sourceIpAddress      = 0x0a0a0a0a;
      for(j=0; j<L7_RADIUS_MAX_SECRET; j++)
      {
        cfgClient->serverConfigEntries[i].radiusServerConfigSecret[j] = 0xb + j + i;
      }

      cfgClient->serverConfigEntries[i].radiusServerIsSecretConfigured = L7_TRUE;
      cfgClient->serverConfigEntries[i].radiusServerConfigRowStatus = RADIUS_SERVER_ACTIVE;
      cfgClient->serverConfigEntries[i].radiusServerConfigServerEntryType = L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY;
      cfgClient->serverConfigEntries[i].radiusServerConfigPriority = 6;
      cfgClient->serverConfigEntries[i].incMsgAuthMode = L7_DISABLE;
    }

 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   radiusCfg.hdr.dataChanged = L7_TRUE;  
   sysapiPrintf("Built test config data\n");


}

                          
/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusDebugConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;
    L7_uint32 i;
    radiusClient_t *cfgClient;


    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(radiusCfg.hdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/
  
   /* default values for the radiusClient structure */
   cfgClient = &radiusCfg.cfg.radiusClient;

   sysapiPrintf( " cfgClient->authOperMode     = %d\n", cfgClient->authOperMode);
   sysapiPrintf( " cfgClient->acctAdminMode    = %d\n", cfgClient->acctAdminMode);
   sysapiPrintf( " cfgClient->maxNumRetrans    = %d\n", cfgClient->maxNumRetrans);
   sysapiPrintf( " cfgClient->timeOutDuration  = %d\n", cfgClient->timeOutDuration);
   sysapiPrintf( " cfgClient->retryPrimaryTime = %d\n", cfgClient->retryPrimaryTime);
   sysapiPrintf( " cfgClient->numAuthServers   = %d\n", cfgClient->numAuthServers);
   sysapiPrintf( " cfgClient->radiusServerDeadTime = %d\n", cfgClient->radiusServerDeadTime);
   sysapiPrintf( " cfgClient->sourceIpAddress      = 0x%x\n", cfgClient->sourceIpAddress);
   sysapiPrintf( " cfgClient->radiusServerSecret   = %s\n", cfgClient->radiusServerSecret);



   for (i = L7_NULL; (i < L7_RADIUS_MAX_SERVERS); i++)
   {
     L7_uint32 j;


     sysapiPrintf( "\n");
     sysapiPrintf( "ENTRY: %d\n", i);
     sysapiPrintf( "----------------\n");

     sysapiPrintf( "cfgClient->serverConfigEntries[i].radiusServerConfigIpAddress = %d\n",
                   cfgClient->serverConfigEntries[i].radiusServerConfigIpAddress);
     sysapiPrintf( "cfgClient->serverConfigEntries[i].radiusServerConfigUdpPort   = %d\n",
                   cfgClient->serverConfigEntries[i].radiusServerConfigUdpPort);
     sysapiPrintf( "cfgClient->serverConfigEntries[i].radiusServerConfigServerType = %d\n",
                      cfgClient->serverConfigEntries[i].radiusServerConfigServerType);


     sysapiPrintf( "cfgClient->serverConfigEntries[i].radiusServerConfigSecret = ", 
                   cfgClient->serverConfigEntries[i].radiusServerConfigSecret);
     for(j=0; j<L7_RADIUS_MAX_SECRET; j++)
     {
       sysapiPrintf(" %02", cfgClient->serverConfigEntries[i].radiusServerConfigSecret[j]);
     }

    sysapiPrintf( "\n");

    sysapiPrintf( "cfgClient->serverConfigEntries[i].radiusServerIsSecretConfigured     = %d\n", 
                  cfgClient->serverConfigEntries[i].radiusServerIsSecretConfigured);
    sysapiPrintf( "cfgClient->serverConfigEntries[i].radiusServerConfigRowStatus        = %d\n", 
                  cfgClient->serverConfigEntries[i].radiusServerConfigRowStatus);
    sysapiPrintf( "cfgClient->serverConfigEntries[i].radiusServerConfigServerEntryType  = %d\n", 
                  cfgClient->serverConfigEntries[i].radiusServerConfigServerEntryType );
    sysapiPrintf( "cfgClient->serverConfigEntries[i].radiusServerConfigPriority         = %d\n",
                  cfgClient->serverConfigEntries[i].radiusServerConfigPriority);
    sysapiPrintf( "cfgClient->serverConfigEntries[i].incMsgAuthMode                     = %d\n",
                  cfgClient->serverConfigEntries[i].incMsgAuthMode);
    sysapiPrintf( "cfgClient->serverConfigEntries[i].maxNumRetrans                      = %d\n",
                  cfgClient->serverConfigEntries[i].maxNumRetrans);
    sysapiPrintf( "cfgClient->serverConfigEntries[i].timeOutDuration                    = %d\n",
                  cfgClient->serverConfigEntries[i].timeOutDuration);
    sysapiPrintf( "cfgClient->serverConfigEntries[i].radiusServerDeadTime               = %d\n",
                  cfgClient->serverConfigEntries[i].radiusServerDeadTime);
    sysapiPrintf( "cfgClient->serverConfigEntries[i].sourceIpAddress                    = 0x%x\n",
                  cfgClient->serverConfigEntries[i].sourceIpAddress);
    sysapiPrintf( "cfgClient->serverConfigEntries[i].localConfigMask                    = %d\n",
                  cfgClient->serverConfigEntries[i].localConfigMask);
   }

   sysapiPrintf( "\n");
   
   /*-------------------------------*/
   /* Scaling Constants             */
   /*-------------------------------*/

    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "L7_RADIUS_MAX_SERVERS - %d\n", L7_RADIUS_MAX_SERVERS);
    sysapiPrintf( "L7_RADIUS_MAX_SECRET - %d\n", L7_RADIUS_MAX_SECRET);

                                                      

    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("radiusCfg.checkSum : %u\n", radiusCfg.checkSum);


}
/*********************************************************************
*
* @purpose  Display the list of All Accounting Servers configured or
*           next entry to the given server based on choice.
*
* @param serverAddr @b((input)) server address.
* @param Type @b((input))       server address type.
* @param choice @b((input))     Choice whether display all the entries
*                               or only next entry to the specified one.
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusDebugAcctServerNextIPHostNameGet(L7_uchar8 *serverAddr,L7_uchar8 Type, L7_uchar8 choice) 
{
 L7_IP_ADDRESS_TYPE_t pType = Type;
 L7_uint32 index = 0;
 
 if(choice == 1) 
 {
     sysapiPrintf("\n Displaying All of server entries \n",serverAddr);
     if(radiusAcctServerFirstIPHostNameGet(serverAddr,&pType) == L7_SUCCESS )
     {
         sysapiPrintf("\n %d. Server: %s",++index,serverAddr);
         while(radiusAcctServerNextIPHostNameGet(serverAddr, 
                                              serverAddr,
                                              &pType) == L7_SUCCESS ) 
         {
           sysapiPrintf("\n %d. Server: %s",++index,serverAddr);
         }
     }
     else
       sysapiPrintf("\n No extries exist...\n");
 }
 else
 {
     sysapiPrintf("\n Displaying Next server entry to %s \n",serverAddr);
     
     if(radiusAcctServerNextIPHostNameGet(serverAddr, 
                                          serverAddr,
                                          &pType) != L7_SUCCESS )
     {
       sysapiPrintf("\n Failed to get the next entry..\n");   
     }
     else
     {
       sysapiPrintf("\n Next Entry: %s",serverAddr);
     }
 }
}

void radiusDebugGlobalShow(void)
{
    dnsHost_t  hostAddr;
    L7_uint32  sourceIpAddress, deadtime, maxRetrans, timeout;
    L7_uint32  val;
    L7_char8  secret[L7_RADIUS_MAX_SECRET+1];
    L7_char8  buf[L7_DNS_HOST_NAME_LEN_MAX];

    hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4; 
    hostAddr.host.ipAddr  = ALL_RADIUS_SERVERS;
   
    /* Secret Configured */
    sysapiPrintf("\n Secret Configured:");          
    if(radiusHostNameServerSharedSecretGet(&hostAddr,secret,&val) != L7_SUCCESS)
    {
        sysapiPrintf("\n SecretGet Failed");
    }
    else
    {
        if (val == L7_RADIUSLOCAL)
        {
            sysapiPrintf("Local");
        }
        else
        {
            sysapiPrintf("Global");
        }
    }

    /* SourceIP */
    sysapiPrintf("\n SourceIP: ");          
    if ( radiusServerSourceIPGet( &hostAddr, &sourceIpAddress, &val ) != L7_SUCCESS )
    {
        sysapiPrintf("Get-Failed");
    }
    else
    {
        osapiInetNtoa(sourceIpAddress, buf);
        sysapiPrintf("%s: ", buf);
        if (val == L7_RADIUSLOCAL)
        {
            sysapiPrintf("Local");
        }
        else
        {
            sysapiPrintf("Global");
        }
    }

    /* Deadtime*/
    sysapiPrintf("\n Deadtime: ");          
    if ( radiusServerDeadtimeGet( &hostAddr, &deadtime, &val ) != L7_SUCCESS )
    {
        sysapiPrintf("Get-Failed");
    }                                    
    else
    {
        sysapiPrintf("%d: ", deadtime);
        if (val == L7_RADIUSLOCAL)
        {
            sysapiPrintf("Local");
        }
        else
        {
            sysapiPrintf("Global");
        }
    }

    /* MaxRetrans */
    sysapiPrintf("\n MaxRetrans: ");          
    if ( radiusMaxNoOfRetransGet( &hostAddr, &maxRetrans, &val ) != L7_SUCCESS )
    {
        sysapiPrintf("Get-Failed");
    }                                    
    else
    {
        sysapiPrintf("%d: ", maxRetrans);
        if (val == L7_RADIUSLOCAL)
        {
            sysapiPrintf("Local");
        }
        else
        {
            sysapiPrintf("Global");
        }
    }

    /* Timeout */
    sysapiPrintf("\n Timeout: ");          
    if ( radiusTimeOutDurationGet( &hostAddr, &timeout, &val ) != L7_SUCCESS )
    {
        sysapiPrintf("Get-Failed");
    }                                     
    else
    {
        sysapiPrintf("%d: ", timeout);
        if (val == L7_RADIUSLOCAL)
        {
            sysapiPrintf("Local");
        }
        else
        {
            sysapiPrintf("Global");
        }
    }
}
void radiusDebugAuthServerEntriesShow(void)
{
  L7_char8 host[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8 buf[L7_DNS_HOST_NAME_LEN_MAX];
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_uint32 nameIndex,val,ip;
  dnsHost_t  hostAddr;
  L7_uint32  sourceIpAddress, deadtime, maxRetrans, timeout;
  L7_RADIUS_SERVER_USAGE_TYPE_t usageType;
  L7_char8   secret[L7_RADIUS_MAX_SECRET+1];
  memset (host, 0, sizeof(host) );

  if (radiusServerFirstIPHostNameGet( host, &addrType) != L7_SUCCESS)
  {
    sysapiPrintf("\n There are no server entries \n");
    return;
  }
  sysapiPrintf("\n HOST ADDR BASED WALK THROUGH THE TABLE");
  sysapiPrintf("\n ======================================");
  do
  {
   
    /*host addr */
    sysapiPrintf("\n\n Host Addr: ");
    sysapiPrintf(" %-20s",host);

    memset (buf, 0, sizeof(buf) );
    if( radiusServerNameIndexGet(host,addrType, &nameIndex) == L7_SUCCESS &&
        radiusCurrentAuthHostNameServerGet(nameIndex, L7_RADIUS_SERVER_USAGE_ALL, buf) == L7_SUCCESS &&
        ( osapiStrncmp(host,buf,sizeof(host)) == 0 )
      )
    {
      sysapiPrintf("[  ***  ]"); /* To indicate current server*/
    }

    if (addrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      hostAddr.hostAddrType = addrType;
      hostAddr.host.ipAddr  = osapiInet_addr(host);
    }
    else
    {
      hostAddr.hostAddrType = addrType;
      memcpy(hostAddr.host.hostName, host, L7_DNS_HOST_NAME_LEN_MAX);
    }
   if( L7_IP_ADDRESS_TYPE_DNS == addrType )
   {
     if( radiusServerHostIPAddrGet(host, RADIUS_SERVER_TYPE_AUTH, &ip) 
                     == L7_SUCCESS )
     {
      sysapiPrintf("\nIP: %s",osapiInet_ntoa(ip));
     }
     else
     {
       sysapiPrintf("\n ipAddr get Failed");
     }
   }

    /* host name (alias, not dns)*/
    memset (buf, 0, sizeof(buf) );
    sysapiPrintf("\n Name: ");
    if( radiusServerHostNameGet(host, addrType, buf) == L7_SUCCESS )
    {
      sysapiPrintf("%s",buf);
    }
    else
    {
      sysapiPrintf("%s","serverName Failed");
    }
    /*index to server name entries */ 
    sysapiPrintf("\n index:  %d",nameIndex);

    /* Type (Pri/Sec)*/
    sysapiPrintf("\n Type: ");
    if( radiusHostNameServerEntryTypeGet(host, addrType, &val) == L7_SUCCESS )
    {
      if (val == L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY)
      {
        sysapiPrintf("%s",  "Primary");
      }                                            
      else
      {
        sysapiPrintf("%s",  "Secondary");
      }                                            
    }
    else
    {
      sysapiPrintf(" %s","Get-Failed");
    }   
    /* Port */
    sysapiPrintf("\n Port: ");          
    if (radiusHostNameServerPortNumGet(host, addrType, &val) != L7_SUCCESS)
    {
      sysapiPrintf("Get-Failed");
    }                                    /* "N\A" */
    else
    {
      sysapiPrintf( "%u", val);
    }
    
    /* Secret Configured */
    sysapiPrintf("\n Secret Configured: ");          
    if ( radiusHostNameServerSharedSecretConfigured( &hostAddr, &val ) != L7_SUCCESS )
    {
      sysapiPrintf("\nGet-Failed");
    }                                     /* "N\A" */
    else
    {
      if (val == L7_TRUE)
      {
        sysapiPrintf( "Yes:");
        if(radiusHostNameServerSharedSecretGet(&hostAddr,secret,&val) != L7_SUCCESS)
        {
            sysapiPrintf("\n SecretGet Failed");
        }
        else
        {
            if (val == L7_RADIUSLOCAL)
            {
              sysapiPrintf("Local");
            }
            else
            {
              sysapiPrintf("Global");
            }
        }
      }                                       /* "Yes"*/
      else
      {
        sysapiPrintf( "No");
      }                                      /* "No"*/
    }
    /* SourceIP */
    sysapiPrintf("\n SourceIP: ");          
    if ( radiusServerSourceIPGet( &hostAddr, &sourceIpAddress, &val ) != L7_SUCCESS )
    {
      sysapiPrintf("Get-Failed");
    }                                     /* "N\A" */
    else
    {
      osapiInetNtoa(sourceIpAddress, buf);
      sysapiPrintf("%s: ", buf);
      if (val == L7_RADIUSLOCAL)
      {
        sysapiPrintf("Local");
      }
      else
      {
        sysapiPrintf("Global");
      }
    }
    /* Deadtime*/
    sysapiPrintf("\n Deadtime: ");          
    if ( radiusServerDeadtimeGet( &hostAddr, &deadtime, &val ) != L7_SUCCESS )
    {
      sysapiPrintf("Get-Failed");
    }                                     /* "N\A" */
    else
    {
      sysapiPrintf("%d: ", deadtime);
      if (val == L7_RADIUSLOCAL)
      {
        sysapiPrintf("Local");
      }
      else
      {
        sysapiPrintf("Global");
      }
    }
    /* MaxRetrans */
    sysapiPrintf("\n MaxRetrans: ");          
    if ( radiusMaxNoOfRetransGet( &hostAddr, &maxRetrans, &val ) != L7_SUCCESS )
    {
      sysapiPrintf("Get-Failed");
    }                                     /* "N\A" */
    else
    {
      sysapiPrintf("%d: ", maxRetrans);
      if (val == L7_RADIUSLOCAL)
      {
        sysapiPrintf("Local");
      }
      else
      {
        sysapiPrintf("Global");
      }
    }
    /* Timeout */
    sysapiPrintf("\n Timeout: ");          
    if ( radiusTimeOutDurationGet( &hostAddr, &timeout, &val ) != L7_SUCCESS )
    {
      sysapiPrintf("Get-Failed");
    }                                     /* "N\A" */
    else
    {
      sysapiPrintf("%d: ", timeout);
      if (val == L7_RADIUSLOCAL)
      {
        sysapiPrintf("Local");
      }
      else
      {
        sysapiPrintf("Global");
      }
    }
    /* Priority */
    sysapiPrintf("\n Priority: ");          
    if ( radiusHostNameServerPriorityGet( host, addrType, &val ) != L7_SUCCESS )
    {
      sysapiPrintf("Get-Failed");
    }                                     /* "N\A" */
    else
    {
      sysapiPrintf("%d", val);
    }
    /* Usage Type*/
    sysapiPrintf("\n UsageType: ");          
    if ( radiusServerUsageTypeGet( &hostAddr, &usageType) != L7_SUCCESS )
    {
      sysapiPrintf("Get-Failed");
    }                                     /* "N\A" */
    else
    {
      if (usageType == L7_RADIUS_SERVER_USAGE_LOGIN)
      {
        sysapiPrintf("Login");
      }
      else if (usageType == L7_RADIUS_SERVER_USAGE_DOT1X)
      {
        sysapiPrintf("Dot1x");
      }
      else 
      {
        sysapiPrintf("All");
      }
    }
  }while( radiusServerNextIPHostNameGet(host, host, &addrType) == L7_SUCCESS );
  
}

void radiusDebugAcctServerEntriesShow(void)
{
  L7_char8 host[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8 buf[L7_DNS_HOST_NAME_LEN_MAX];
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_uint32 val,ip;
  dnsHost_t  hostAddr;

  memset (host, 0, sizeof(host) );

  if (radiusAcctServerFirstIPHostNameGet( host, &addrType) != L7_SUCCESS)
  {
    sysapiPrintf("\n There are no server entries \n");
    return;
  }
   
  do
  {
    if (addrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      hostAddr.hostAddrType = addrType;
      hostAddr.host.ipAddr  = osapiInet_addr(host);
    }
    else
    {
      hostAddr.hostAddrType = addrType;
      memcpy(hostAddr.host.hostName, host, L7_DNS_HOST_NAME_LEN_MAX);
    }

    /*host addr */
    sysapiPrintf("\n\n HOST: %s",host);
 
   if( L7_IP_ADDRESS_TYPE_DNS == addrType )
   {
     if( radiusServerHostIPAddrGet(host, RADIUS_SERVER_TYPE_ACCT, 
                     &ip) == L7_SUCCESS )
     {
      sysapiPrintf("\nIP: %s",osapiInet_ntoa(ip));
     }
     else
     {
       sysapiPrintf("\n ipAddr get Failed");
     }
   }
    /* host name (alias, not dns)*/
    sysapiPrintf("\n Server Name: ");
    memset (buf, 0, sizeof(buf) );
    if( radiusServerAcctHostNameGet(host, addrType, buf) == L7_SUCCESS )
    {
      sysapiPrintf(" %-32s",buf);
    }
    else
    {
      sysapiPrintf(" %-32s","serverName Failed");
    }

    /* Port */
    sysapiPrintf("\n Port: ");          
    if (radiusHostNameServerPortNumGet(host, addrType, &val) != L7_SUCCESS)
    {
      sysapiPrintf("Get-Failed");
    }                                    /* "N\A" */
    else
    {
      sysapiPrintf( "%u", val);
    }
    
    /* Secret Configured */
    sysapiPrintf("\n Secret Configured: ");          
    if ( radiusHostNameServerSharedSecretConfigured( &hostAddr, &val ) != L7_SUCCESS )
    {
      sysapiPrintf("Get-Failed");
    }                                     /* "N\A" */
    else
    {
      if (val == L7_TRUE)
      {
        sysapiPrintf( "Yes");
      }                                       /* "Yes"*/
      else
      {
        sysapiPrintf( "No");
      }                                      /* "No"*/
    }


  }while( radiusServerNextIPHostNameGet(host, host, &addrType) == L7_SUCCESS );  
}
/*configure server with desired name.*/
void radiusDebugGlobalConfig(void)
{
    L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];
    L7_uint32  sourceIpAddress, deadTime, maxRetrans, timeOut;
    dnsHost_t hostAddr;
    L7_char8   secret[L7_RADIUS_MAX_SECRET+1] = "broadcom";
    memset(name,0,L7_RADIUS_SERVER_NAME_LENGTH+1);

    hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4; 
    hostAddr.host.ipAddr  = ALL_RADIUS_SERVERS;
    maxRetrans = 10;
    if( radiusMaxRetransSet(&hostAddr,maxRetrans,L7_RADIUSGLOBAL) == L7_SUCCESS )
    {
        sysapiPrintf("\n configured max retransmit = %d \n",maxRetrans);
    }
    else
    {  
        sysapiPrintf("\n max retransmit configuration failed \n");
    }

    timeOut = 9 ;
    if( radiusTimeOutDurationSet(&hostAddr,timeOut, L7_RADIUSGLOBAL) == L7_SUCCESS )
    {
        sysapiPrintf("\n configured time out = %d \n",timeOut);
    }
    else
    {  
        sysapiPrintf("\n max timeout configuration failed \n");
    }
 
    deadTime = 6 * RADIUS_TIMER_GRANULARITY;
    if( radiusServerDeadtimeSet(&hostAddr, deadTime, L7_RADIUSGLOBAL) == L7_SUCCESS )
    {
        sysapiPrintf("\n configured dead time = %d \n",deadTime);
    }
    else
    {  
        sysapiPrintf("\n max dead time configuration failed \n");
    }

    sourceIpAddress = 0x0a0b0b0a;
    if( radiusServerSourceIPSet(&hostAddr, sourceIpAddress, L7_RADIUSGLOBAL) == L7_SUCCESS )
    {
        sysapiPrintf("\n configured source IP= 0x%x \n",sourceIpAddress);
    }
    else
    {  
        sysapiPrintf("\n source IP configuration failed \n");
    }

    if( radiusHostNameServerSharedSecretSet(&hostAddr, secret, L7_RADIUSGLOBAL) == L7_SUCCESS )
    {
        sysapiPrintf(" \n configured secret \n");
    }
    else
    {
        sysapiPrintf("\n secret configuration failed \n");
    }    
}
/*configure server with desired name.*/
void radiusDebugServerConfig(L7_uint32 type, L7_char8 *serverAddr, L7_char8 *serverName)
{
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];
  L7_uint32  serverPriority, sourceIpAddress, deadTime, maxRetrans, timeOut;
  dnsHost_t hostAddr;
  L7_char8   secret[L7_RADIUS_MAX_SECRET+1] = "broadcom";
  memset(name,0,L7_RADIUS_SERVER_NAME_LENGTH+1);

  if(RADIUS_SERVER_TYPE_AUTH == type)  
  {  
    /* If user doesn't specify any Name, use default one.*/
     if(L7_NULLPTR == serverName) 
     {
       osapiStrncpySafe(name,L7_RADIUS_SERVER_DEFAULT_NAME_AUTH,L7_RADIUS_SERVER_NAME_LENGTH+1);
     }
     else
       osapiStrncpySafe(name,serverName,L7_RADIUS_SERVER_NAME_LENGTH+1);
  
     /* To start with, hostaddr type assumed to be ipv4*/
     if( radiusServerIPHostNameAdd(serverAddr, L7_IP_ADDRESS_TYPE_IPV4, name)  == L7_SUCCESS )
     {
       sysapiPrintf("\n configured auth server entry with: \n host: %-20s name: %-32s \n",serverAddr,name);
     }
     else
       sysapiPrintf("\n configuration failed \n");
  }
 
  if(RADIUS_SERVER_TYPE_ACCT == type) 
  {
    /* If user doesn't specify any Name, use default one.*/
     if(L7_NULLPTR == serverName) 
     {
       osapiStrncpySafe(name,L7_RADIUS_SERVER_DEFAULT_NAME_ACCT,L7_RADIUS_SERVER_NAME_LENGTH+1);
     }
     else
       osapiStrncpySafe(name,serverName,L7_RADIUS_SERVER_NAME_LENGTH+1);
  
     /* To start with, hostaddr type assumed to be ipv4*/
     if( radiusAcctServerIPHostNameAdd(serverAddr, L7_IP_ADDRESS_TYPE_IPV4, name)  == L7_SUCCESS )
     {
       sysapiPrintf("\n configured acct server entry with: \n host: %-20s name: %-32s \n",serverAddr,name);
     }
     else
       sysapiPrintf("\n configuration failed \n");    
  }
  hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4; 
  hostAddr.host.ipAddr = osapiInet_addr(serverAddr); 

  if ( radiusServerUsageTypeSet(&hostAddr,L7_RADIUS_SERVER_USAGE_DOT1X) == L7_SUCCESS )
  {
       sysapiPrintf("\n configured Usage Type \n");
  }
  else
  {
       sysapiPrintf("\n usaget type configuration failed \n");
  }

  serverPriority = 1;
  if( radiusHostNameServerPrioritySet(serverAddr, L7_IP_ADDRESS_TYPE_IPV4,1) == L7_SUCCESS )
  {
       sysapiPrintf("\n configured server prirority= %d \n",serverPriority);
  }
  else
  {  
       sysapiPrintf("\n server priority configuration failed \n");
  }


  maxRetrans = 10;
  if( radiusMaxRetransSet(&hostAddr,maxRetrans,L7_RADIUSLOCAL) == L7_SUCCESS )
  {
       sysapiPrintf("\n configured max retransmit = %d \n",maxRetrans);
  }
  else
  {  
       sysapiPrintf("\n max retransmit configuration failed \n");
  }

  timeOut = 9 ;
  if( radiusTimeOutDurationSet(&hostAddr,timeOut, L7_RADIUSGLOBAL) == L7_SUCCESS )
  {
       sysapiPrintf("\n configured time out = %d \n",timeOut);
  }
  else
  {  
       sysapiPrintf("\n max timeout configuration failed \n");
  }
  deadTime = 6 * RADIUS_TIMER_GRANULARITY;
  if( radiusServerDeadtimeSet(&hostAddr, deadTime, L7_RADIUSLOCAL) == L7_SUCCESS )
  {
       sysapiPrintf("\n configured dead time = %d \n",deadTime);
  }
  else
  {  
       sysapiPrintf("\n max dead time configuration failed \n");
  }
  sourceIpAddress = 0x0a0b0b0a;
  if( radiusServerSourceIPSet(&hostAddr, sourceIpAddress, L7_RADIUSGLOBAL) == L7_SUCCESS )
  {
       sysapiPrintf("\n configured source IP= 0x%x \n",sourceIpAddress);
  }
  else
  {  
       sysapiPrintf("\n source IP configuration failed \n");
  }
  
  if( radiusHostNameServerSharedSecretSet(&hostAddr, secret, L7_RADIUSGLOBAL) == L7_SUCCESS )
  {
      sysapiPrintf(" \n configured secret \n");
  }
  else
  {
      sysapiPrintf("\n secret configuration failed \n");
  }    
}

/*delete server with desired name.*/
/* 1 - Auth / 2- Acct */
void radiusDebugServerEntryRemove(L7_uint32 type, L7_char8 *serverAddr)
{
  sysapiPrintf("\n server: %s , type: d",(L7_uchar8 *)serverAddr, type);
  if(RADIUS_SERVER_TYPE_AUTH == type)  
  {    
     /* To start with, hostaddr type assumed to be ipv4*/
     if( radiusAuthHostNameServerRemove(serverAddr, L7_IP_ADDRESS_TYPE_IPV4)  == L7_SUCCESS )
     {
       sysapiPrintf("\n successfully removed the Auth server entry \n host: %-20s \n",serverAddr);
     }
     else
     {
       sysapiPrintf("\n configuration failed \n");
     }
  }
 
  if(RADIUS_SERVER_TYPE_ACCT == type) 
  {
     /* To start with, hostaddr type assumed to be ipv4*/
     if( radiusAcctHostNameServerRemove(serverAddr, L7_IP_ADDRESS_TYPE_IPV4)  == L7_SUCCESS )
     {
       sysapiPrintf("\n successfully removed the Auth server entry \n host: %-20s \n",serverAddr);
     }
     else
     {
       sysapiPrintf("\n configuration failed \n");
     }
  }
}

/*Configure desired Primary server*/
/* entryType = 1 (primary) /2 (secondary) according to macro definitions*/
void radiusDebugServerTypeConfig(L7_char8 *serverAddr, L7_uint8 entryType)
{

 /*To start with assuming only ipadd being passed in */
  sysapiPrintf("\n Configuring server: %s type: %s",serverAddr,(entryType==1?"primary":(entryType==2?"secondary":"")));
  if( radiusHostNameServerEntryTypeSet(serverAddr, L7_IP_ADDRESS_TYPE_IPV4, entryType) == L7_SUCCESS )
  { 
    sysapiPrintf("\n SUCCESS! \n");
  }
  else
  {
    sysapiPrintf("\n FAILURE! \n");
  }

}
/* type indicates auth(1)/acct(2)*/
void radiusDebugServerNameSet(L7_uint8 type, L7_char8 *serverAddr, L7_char8 *serverName )
{
  sysapiPrintf("\n Setting the name attribute to the host: %s value: %s of type: \n",serverAddr,serverName);
  if( RADIUS_SERVER_TYPE_AUTH == type) 
  {
    sysapiPrintf("\n AUTH \n");
    /* To start with, hostaddr type assumed to be ipv4*/
    if( radiusServerAuthNameSet(serverAddr, L7_IP_ADDRESS_TYPE_IPV4, serverName)  == L7_SUCCESS )
    {
      sysapiPrintf("\n configured server entry with: \n host: %-20s name: %-32s \n",serverAddr,serverName);
    }
    else
     sysapiPrintf("\n configuration failed \n");
    return;
  }
  
  if( RADIUS_SERVER_TYPE_ACCT == type) 
  {
    sysapiPrintf("\n ACCT \n");
    /* To start with, hostaddr type assumed to be ipv4*/
    if( radiusServerAcctNameSet(serverAddr, L7_IP_ADDRESS_TYPE_IPV4, serverName)  == L7_SUCCESS )
    {
      sysapiPrintf("\n configured server entry with: \n host: %-20s name: %-32s \n",serverAddr,serverName);
    }
    else
     sysapiPrintf("\n configuration failed \n");
    
    return;
  }
}
/*Unconditional Walk through the nameArray[]*/
void radiusDebugServerNameEntriesShowAll(L7_uint32 type)
{
  L7_uint32 i,count=0;
  L7_char8 serverName[L7_RADIUS_SERVER_NAME_LENGTH+1];
  L7_char8 currentServer[L7_RADIUS_SERVER_NAME_LENGTH+1];
  radiusServerEntry_t *currentEntry = L7_NULL;
  
  if( RADIUS_SERVER_TYPE_AUTH == type) 
  {
      sysapiPrintf("\nauthServerNameArray[] Contents :");
      sysapiPrintf("\n============================");
      for(i=1;i<=L7_RADIUS_MAX_AUTH_SERVERS;i++) 
      {    
        if( radiusServerNameEntryDetailedGet(i,serverName,currentServer,&count,currentEntry) == L7_SUCCESS )
        {
          sysapiPrintf("\n %-32s %-5d %-20s %u",serverName,count,currentServer,currentEntry);
        }
        else
        {
          sysapiPrintf(" Failed to get the details");
        }    
      }
  }
  if( RADIUS_SERVER_TYPE_ACCT == type) 
  {
      sysapiPrintf("\nacctServerNameArray[] Contents :");
      sysapiPrintf("\n============================");
      for(i=1;i<=L7_RADIUS_MAX_ACCT_SERVERS;i++) 
      {    
        if( radiusServerAcctNameEntryDetailedGet(i,serverName,&count) == L7_SUCCESS )
        {
          sysapiPrintf("\n %-5d %-32s %-5d ",i, serverName,count);
        }
        else
        {
          sysapiPrintf(" Failed to get the details");
        }    
      }
  }

  sysapiPrintf("\n============================");
}
/* 1=Auth / 2=Acct */
/* Lists all the server names & corresponding current servers*/
void radiusDebugNamedServersWalk(L7_uint32 type)
{  
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];
  L7_char8 host[L7_DNS_HOST_NAME_LEN_MAX];
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;

  memset(name,0,L7_RADIUS_SERVER_NAME_LENGTH+1);
  sysapiPrintf("\n=======================");
  sysapiPrintf("\n Name based walk through the server Table, Type: ");
  if( RADIUS_SERVER_TYPE_AUTH == type) 
  {
    sysapiPrintf("AUTH");
    if( radiusNamedServerOrderlyGetNext(name, name, host, &addrType) != L7_SUCCESS )
	   {
	     sysapiPrintf("\n Failed to get first entry \n");
		    return;
	   }
	   do
	   {
		    sysapiPrintf("\n server name: %-32s ",name);
		    sysapiPrintf("\n host name: %-32s ",host);
      memset(host,0,L7_DNS_HOST_NAME_LEN_MAX);
	   }while( radiusNamedServerOrderlyGetNext(name, name, host, &addrType) == L7_SUCCESS );
  }

  if( RADIUS_SERVER_TYPE_ACCT == type) 
  {
    sysapiPrintf("ACCT");
    if( radiusNamedAcctServerOrderlyGetNext(name, name, host, &addrType) != L7_SUCCESS )
	   {
	     sysapiPrintf("\n Failed to get first entry \n");
		    return;
	   }
	   do
	   {
		    sysapiPrintf("\n server name: %-32s ",name);
		    sysapiPrintf("\n server name: %-32s ",host);
	   }
    while( radiusNamedAcctServerOrderlyGetNext(name, name, host,&addrType) == L7_SUCCESS );
  }
}

/* Left only for debug purpose*/
L7_RC_t radiusServerNameEntryDetailedGet(L7_uint32 ind,L7_char8 *serverName,L7_char8 *currentServer,L7_uint32 *count,void *currentEntry)
{
  L7_RC_t rc = L7_FAILURE;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if( radiusAuthServerNameEntryDetailedGet(ind,serverName,currentServer,count,currentEntry) == L7_SUCCESS )
    {
      rc = L7_SUCCESS;
    }
  }
  (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  return rc;
}

L7_RC_t radiusServerAcctNameEntryDetailedGet(L7_uint32 ind,L7_char8 *serverName,L7_uint32 *count)
{
  L7_RC_t rc = L7_FAILURE;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if( radiusAcctServerNameEntryDetailedGet(ind,serverName,count) == L7_SUCCESS )
    {
      rc = L7_SUCCESS;
    }
  }
  (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  return rc;
}

/*********************************************************************
*
* @purpose Get the server name index held by the specified server.
*          
* @param serverAddr @b((input)) IP address of the Auth Server
* @param L7_IP_ADDRESS_TYPE_t type @b{(input)} server address Type.
* @param L7_uint32 *index @b((output)) Pointer to the Index of the Auth Server
*
* @returns L7_SUCCESS - if there is a server corresponding to the 
*                       specified index
* @returns L7_FAILURE - if no server for the index exists
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerhostIndexGet(L7_uchar8 *serverAddr,
                     L7_IP_ADDRESS_TYPE_t type, L7_uint32 *index) 
{
  radiusServerEntry_t * serverEntry; 
  L7_RC_t rc = L7_FAILURE;

  if ((L7_NULL == serverAddr) || (L7_NULL == index)) 
  {
    LOG_MSG("RADIUS: radiusServerhostIndexGet(): Invalid parameters! \n");

    return rc;
  }

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    serverEntry = radiusServerEntries;

    if(type == L7_IP_ADDRESS_TYPE_IPV4)
    {
      serverEntry = radiusServerEntryGet(osapiInet_addr(serverAddr), RADIUS_SERVER_TYPE_AUTH);
    }
    else
    {
      serverEntry = radiusServerHostEntryGet(serverAddr, RADIUS_SERVER_TYPE_AUTH);
    }

    if (serverEntry != L7_NULL)
    {
      *index = serverEntry->radiusServerStats_t.authStats.radiusAuthServerIndex;
      
      rc = L7_SUCCESS;
    }

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }

  return rc;
}

/*********************************************************************
*
* @purpose To display constant values of parameters.
*          
* @param L7_uint32 type @b((input)) 
*
* @returns none
*
* @comments
*
* @end
*
*********************************************************************/
void radiusDebugDumpConstants(L7_uint32 type)
{
#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
 if(type == 1) 
  {
    sysapiPrintf("\n L7_RADIUS_MAX_NETWORK_INTERFACES : %d \n",L7_RADIUS_MAX_NETWORK_INTERFACES);
    sysapiPrintf("\n L7_RTR_MAX_RTR_INTERFACES : %d \n",L7_RTR_MAX_RTR_INTERFACES);
    sysapiPrintf("\n L7_MAX_NUM_LOOPBACK_INTF : %d \n",L7_MAX_NUM_LOOPBACK_INTF);
    sysapiPrintf("\n radiusGlobalData.totalNwInterfaces :%d\n",radiusGlobalData.totalNwInterfaces);
    sysapiPrintf("\n radiusGlobalData.socketInactiveTime :%u\n",radiusGlobalData.socketInactiveTime );
  }
#endif
}
#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
/*********************************************************************
*
* @purpose To display socket database dynamically formed for 
*          local IP interfaces of the switch.
*          
* @param void
*
* @returns none
*
* @comments
*
* @end
*
*********************************************************************/
void socketDbWalk (void)
{
  nwInterface_t *interface = &radiusGlobalData.nwInterfaces[0]; 
  L7_uint32 i;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    sysapiPrintf("\n List of sockets bound to local Interfaces: ");
    for(i=0;i<radiusGlobalData.totalNwInterfaces;i++) 
    {
      if(interface[i].socket) 
      {
        sysapiPrintf("\n %d. IP: %s socket: %d",i+1,osapiInet_ntoa(interface[i].ipAddr),interface[i].socket);
      }
    }
    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
}
#endif

/*********************************************************************
*
* @purpose To display next name of Auth. or Acct. RADIUS servers in 
*          that comes in alphabetical order.
*          
* @param L7_char8 *name @b((input)) Pointer to the Name of the server.
* @param L7_uint32 type @b((input)) Auth or Acct. type.
* @param L7_uint32 useAPI @b((input)) Whether to use APIs or call 
*                                   internal functions of database.
*
* @returns none
*
* @comments
*
* @end
*
*********************************************************************/
void radiusDebugNameOrderlyGetNext(L7_char8 *name, L7_uint32 type, 
                     L7_uint32 useAPI)
{
  L7_char8 nextName[L7_RADIUS_SERVER_NAME_LENGTH+1];
  L7_char8 tName[L7_RADIUS_SERVER_NAME_LENGTH+1];
  radiusServerEntry_t tmpServer, *pEntry = &tmpServer; /*dummies*/
  L7_uchar8 nextAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_IP_ADDRESS_TYPE_t addrType;

  sysapiPrintf("\n AuthOrderlyGetNext(): name: %s \n ",name);
  memset(nextName,0,sizeof(nextName));
  

     osapiStrncpySafe(tName,"null",sizeof(tName));
     if(osapiStrncmp(tName,name,sizeof(name)) == 0 )
     {
       memset(tName,0,sizeof(tName));
     }
     else
     {
       osapiStrncpySafe(tName,name,sizeof(tName));
     }

  if(useAPI == 1) 
  {

        if(type == RADIUS_SERVER_TYPE_AUTH) 
        {
          if( radiusNamedServerOrderlyGetNext(tName, nextName, nextAddr, &addrType) == L7_SUCCESS )
          {
            sysapiPrintf("\n nextName: %s  nextAddr: %s addrType: %d\n",nextName,nextAddr,addrType);
          }
          else
          {
            sysapiPrintf("\n Failed to get next order AUTH name\n");
          }
        }
        if(type == RADIUS_SERVER_TYPE_ACCT) 
        {
          if( radiusNamedAcctServerOrderlyGetNext(tName, nextName, nextAddr, &addrType) == L7_SUCCESS )
          {
            sysapiPrintf("\n nextName: %s  nextAddr: %s addrType: %d\n",nextName,nextAddr,addrType);
          }
          else
          {
            sysapiPrintf("\n Failed to get next order AUTH name\n");
          }
        }
  }
  else
  {
      if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
      {

        if(type == RADIUS_SERVER_TYPE_AUTH) 
        {
          if( radiusAuthServerNameEntryOrderlyGetNext(tName, nextName, &pEntry) == L7_SUCCESS )
          {
            sysapiPrintf("\n nextName: %s \n",nextName);
          }
          else
          {
            sysapiPrintf("\n Failed to get next order AUTH name\n");
          }
        }
        if(type == RADIUS_SERVER_TYPE_ACCT) 
        {
          if( radiusAcctServerNameEntryOrderlyGetNext(tName, nextName, &pEntry) == L7_SUCCESS )
          {
            sysapiPrintf("\n nextName: %s \n",nextName);
          }
          else
          {
            sysapiPrintf("\n Failed to get next order AUTH name\n");
          }
        }
    
        (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
      }    
  }
}

/*********************************************************************
*
* @purpose Display the IP address of the specified DNS server.
*          
* @param host @b((input)) Pointer to the dns address string.
* @param radiusServerType_t type @b((input)) Auth/Acct.
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusDebugServerHostIPAddrGet(L7_uchar8 *host, L7_uint32 type)
{
  L7_uint32 ip;

  if( radiusServerHostIPAddrGet(host, type, &ip) 
                     == L7_SUCCESS )
  {
    sysapiPrintf(" %s",osapiInet_ntoa(ip));
  }
  else
  {
    sysapiPrintf(" ipAddr get Failed");
  }
}

/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/

/*********************************************************************
* @purpose  This function clears all radius debug flags.
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t
radiusDebugReset(void)
{
    memset (radiusDebugLevel, 0, sizeof (radiusDebugLevel));
    return L7_SUCCESS;

} /* radiusDebugReset */


/*********************************************************************
* @purpose  This function sets specified debug flag.
*
* @param    level
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/

void
radiusDebugLevelSet(L7_uint32 level)
{
  if (level >= RD_LEVEL_LAST)
  {
    LOG_ERROR (level);
  }
  else
    radiusDebugLevel[level] = 1;
} /* radiusDebugLevelSet */


/*********************************************************************
* @purpose  This function clears specified debug flag.
*
* @param    level
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/

void
radiusDebugLevelClear(L7_uint32 level)
{
  if (level >= RD_LEVEL_LAST)
  {
    LOG_ERROR (level);
  }
  else
    radiusDebugLevel[level] = 0;
} /* radiusDebugLevelClear */

/*********************************************************************
* @purpose  This function checks whether specified debug flag is set.
*
* @param    level
*
* @returns  L7_TRUE - Flag is set.
*           L7_FALSE - Flag is not set.
*
* @notes    none
*
* @end
*********************************************************************/

L7_BOOL
radiusDebugLevelCheck(L7_uint32 level)
{
   if (level >= RD_LEVEL_LAST)
   {
      LOG_ERROR (level);
   }
   else if (radiusDebugLevel[level] == 1)
   {
      return L7_TRUE;
   }

   return L7_FALSE;
} /* radiusDebugLevelCheck */

int
radiusDebugLevelShow(void)
{
    L7_uint32 i;

    for (i = 0; i < RD_LEVEL_LAST; i++)
    {
        sysapiPrintf("Level %d - %d\n", i, radiusDebugLevel[i]);
    }

    return 0;

} /* radiusDebugLevelShow */

