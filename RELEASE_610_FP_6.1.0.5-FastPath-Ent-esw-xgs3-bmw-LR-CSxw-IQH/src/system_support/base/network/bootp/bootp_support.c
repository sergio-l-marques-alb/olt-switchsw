/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename bootp_support.c
*
* @purpose bootp main file
*
* @component bootp
*
* @comments none
*
* @create 
*
* @author    Deepesh Aggarwal
*
* @end
*             
**********************************************************************/
#include "l7_bootp.h"
#include "sysapi.h"
#include "default_cnfgr.h"
#include "l7netapi.h"
#include "bspapi.h"
#include "usmdb_util_api.h"

#ifndef NO_PRODUCT_BOOTP_SUPPORT
L7_int32 L7_BootP_task_id;

L7_bootp_t service_bootp_data;
L7_bootp_t network_bootp_data;

static L7_uint32 networkConfigStatus = L7_SYSCONFIG_MODE_NOT_COMPLETE;                                    
static L7_uint32 servPortConfigStatus = L7_SYSCONFIG_MODE_NOT_COMPLETE;                                   

/* BOOTP Configuraration protection semaphore.
*/
void * bootpConfigSem = 0;

/* BOOTP Notification protection semaphore.
*/
void * bootpNotificationSem = L7_NULLPTR;

/* BOOTP notification callback functions list */
bootpNotifyList_t *bootpNotifyList = L7_NULLPTR;

/* Signals if there is predefined static IP configuration on service port */
static L7_BOOL defaultServiceIpConfigExists = L7_FALSE;

/* Signals if there is predefined static IP configuration on network port*/
static L7_BOOL defaultNetworkIpConfigExists = L7_FALSE;

/**************************************************************************
* @purpose  Returns default IP address on BOOTP enabled port
*
* @param    L7_bootp_t   boot params structure
*
* @param    L7_uint32    current BOOTP enabled port address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/
void bootpEnabledPortAddressGet(L7_bootp_t* bootpParams, L7_uint32 *portIpAddr)
{
  L7_uint32 servicePortConfigMode = L7_SYSCONFIG_MODE_NONE; /* service port mode -  BOOTP/DHCP or none */
  L7_uint32 networkPortConfigMode = L7_SYSCONFIG_MODE_NONE; /* network port mode -  BOOTP/DHCP or none */
  
  servicePortConfigMode = simGetServPortConfigMode();
  networkPortConfigMode = simGetSystemConfigMode();

  /* Try first if there does not exist factory default IP address */
  if (servicePortConfigMode == L7_SYSCONFIG_MODE_BOOTP && defaultServiceIpConfigExists == L7_TRUE)
  {
    *portIpAddr = bootpParams->bp_ciaddr;
  }
  
  else if (networkPortConfigMode == L7_SYSCONFIG_MODE_BOOTP && defaultNetworkIpConfigExists == L7_TRUE)
  {
    *portIpAddr = bootpParams->bp_ciaddr;
  }
  
  /* if no default IP config is present - use BOOTP offered IP address */  
  else
  {
    *portIpAddr = bootpParams->bp_yiaddr;
  }
}

/*************************************************************************
* @purpose  Extract parameters from bootp parameters structue.
*
* @param    bootpParams    @b{(input)} Ptr to L7_bootp_t structure
* @param    networkParams  @b{(output)} Ptr to L7_bootp_dhcp_t structure 
* 
*
* @returns L7_SUCCESS If parameter extracted successfully
*           
* @returns L7_FAILURE If an error occurs while looking for the parameter 
*
* @comments None.
*
* @end
*************************************************************************/

L7_RC_t getParamsFromBootpStruct(L7_bootp_t* bootpParams, L7_bootp_dhcp_t* networkParams)
{
  L7_uint32 netMask    = L7_NULL;
  L7_uint32 gateway    = L7_NULL;
  L7_uint32 siaddr     = L7_NULL;
  L7_uint32 portIpAddr = L7_NULL;  
  L7_uint32 dnsServerIpAddr = L7_NULL;
  
  memset(networkParams,  0, sizeof(L7_bootp_dhcp_t));
    
  /* Get subnet mask from bootp packet*/
  if(L7_FALSE== L7_bootp_option_get(bootpParams, L7_TAG_SUBNET_MASK,&netMask ))
  {
    LOG_MSG("getParamsFromBootpStruct():L7_bootp_option_get() returned failure for netmask");
    return L7_FAILURE;
  }
  /* Get gateway from bootp packet*/
  if(L7_FALSE== L7_bootp_option_get(bootpParams, L7_TAG_GATEWAY, &gateway ))
  { 
    /* if no gateway in reply default to 0 */
    gateway=0;
  }
  
  memcpy(&siaddr, &(bootpParams->bp_siaddr), sizeof(bootpParams->bp_siaddr));
  
  if(L7_FALSE== L7_bootp_option_get( bootpParams, L7_TAG_DOMAIN_SERVER, &dnsServerIpAddr ))
  {
    dnsServerIpAddr = 0;
  }

  /* Start look for TFTP server hostname */
  if ( strlen(bootpParams->bp_sname) != L7_NULL )
  {
    osapiStrncpy(networkParams->sname, bootpParams->bp_sname, strlen(bootpParams->bp_sname));
  }
  
  /* Start look for boot file name */  
  if ( strlen(bootpParams->bp_file) != L7_NULL )
  {
    osapiStrncpy(networkParams->bootFile, bootpParams->bp_file, strlen(bootpParams->bp_file));
  }
  
  bootpEnabledPortAddressGet(bootpParams, &portIpAddr);
    
  networkParams->ip      = osapiNtohl(portIpAddr);
  networkParams->netMask = osapiNtohl(netMask);
  networkParams->gateway = osapiNtohl(gateway);
  networkParams->optDnsServerIpAddr  = osapiNtohl(dnsServerIpAddr);
  networkParams->siaddr = osapiNtohl(siaddr);

  /* Set offered parameters mask */
  if (networkParams->ip != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_IP_ADDR;
  }
    
  if (networkParams->netMask != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_NETMASK;
  }

  if (networkParams->gateway != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_GATEWAY;
  }

  if (strlen(networkParams->sname) != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_SNAME;
  }
      
  if (strlen(networkParams->bootFile) != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_BOOTFILE;
  }
    
  if (networkParams->optDnsServerIpAddr != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_OPTION_DNS_SVR_ADDR;
  }

  if (networkParams->siaddr != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_SIADDR;
  }
    
  /* Validate the ip address, subnet mask & gateway */
  return usmDbIpInfoValidate(networkParams->ip, networkParams->netMask, networkParams->gateway);
}

/**************************************************************************
* @purpose  Print bootp parameters from specified bootp structure 
*
* @param    bootp      BOOTP structure
* @param    L7_uint8   port type
*
* @returns  L7_SUCCESS or L7_FAILURE
*
*
* @end
*************************************************************************/

L7_int32 bootpDebugPortParamsPrint(struct bootp* bootpParams, L7_uint8 portType)
{
  L7_bootp_dhcp_t networkParams;
  L7_uchar8 ipString[OSAPI_INET_NTOA_BUF_SIZE];

  memset(&networkParams, 0, sizeof(L7_bootp_dhcp_t));
  memset(ipString, 0, sizeof(ipString));

  getParamsFromBootpStruct(bootpParams, &networkParams);

  if( portType == L7_BP_SERVICE_PORT ) sysapiPrintf("Service port settings:\n");
  else
    if( portType == L7_BP_NETWORK_PORT ) sysapiPrintf("Network port settings:\n");
  else
    return L7_FAILURE;
   
  osapiInetNtoa(networkParams.ip, ipString);
  sysapiPrintf("BOOTP offered ip address %s \n", ipString);

  osapiInetNtoa(networkParams.netMask, ipString);
  sysapiPrintf("BOOTP offered netMask %s \n", ipString);
  
  osapiInetNtoa(networkParams.gateway, ipString);
  sysapiPrintf("BOOTP offered gateway %s \n", ipString);
  
  osapiInetNtoa(networkParams.optDnsServerIpAddr, ipString);
  sysapiPrintf("BOOTP offered DNS server address %s \n", ipString);
  
  osapiInetNtoa(networkParams.siaddr, ipString);
  sysapiPrintf("BOOTP offered TFTP server address %s \n", ipString);
  
  sysapiPrintf("BOOTP offered TFTP server name %s \n", networkParams.sname);
  sysapiPrintf("BOOTP offered boot file %s \n\n", networkParams.bootFile);

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Print bootp parameters
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
*
* @end
*************************************************************************/

L7_int32 bootpDebugParamsPrint(void)
{
  bootpDebugPortParamsPrint(&service_bootp_data, L7_BP_SERVICE_PORT);
  bootpDebugPortParamsPrint(&network_bootp_data, L7_BP_NETWORK_PORT);

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  BOOTP task entry point for configuration of Sercvice port and 
*            network port parameters 
*			
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments None.
*
* @end
*************************************************************************/

L7_RC_t L7_bootp()
{
  L7_uint32       networkConfigMode;                                      
  L7_uint32       servPortConfigMode;                                     
  L7_bootp_dhcp_t params;                                          
  L7_uint32       timeout = 10;
  L7_uint32       errorMsg = L7_NULL;
  L7_BOOL         rc = L7_FALSE;

  bootpConfigSemGet ();
  
  (simGetServPortIPAddr() == L7_NULL) ? (defaultServiceIpConfigExists = L7_FALSE) : (defaultServiceIpConfigExists = L7_TRUE);	
  (simGetSystemIPAddr() == L7_NULL) ? (defaultNetworkIpConfigExists = L7_FALSE) : (defaultNetworkIpConfigExists = L7_TRUE);	

  bootpConfigSemFree ();
		  
  while (L7_TRUE)
  {
    bootpConfigSemGet ();

    /* for the service port */  
    servPortConfigMode = simGetServPortConfigMode();
    /* for network */
    networkConfigMode = simGetSystemConfigMode();

    bootpConfigSemFree ();

    if ((servPortConfigMode == L7_SYSCONFIG_MODE_BOOTP) && 
        (servPortConfigStatus != L7_SYSCONFIG_MODE_COMPLETE))
    {
      if (defaultServiceIpConfigExists == L7_TRUE)
      {
        rc = bootpVendorSpecificValuesRequest(bspapiServicePortNameGet(), &service_bootp_data, timeout);
      }
      else
      {
        rc = L7_bootp_do(bspapiServicePortNameGet(), &service_bootp_data, timeout);
      }
	  
      if ( rc == L7_TRUE )
      {
        if (L7_SUCCESS == getParamsFromBootpStruct(&service_bootp_data,&params))
        {
          /*
          ** Check for IP Address conflicts with other interfaces
          */
		  bootpConfigSemGet ();

          if (defaultServiceIpConfigExists == L7_FALSE)
          {
            if (sysapiIPConfigConflict(FD_CNFGR_NIM_MIN_SERV_PORT_INTF_NUM,
                                       params.ip, 
                                       params.netMask, 
                                       (L7_uint32 *)&errorMsg) == L7_FALSE)
            { 
              /* No conflict */
              /*Set IP Addresss, Default Gateway, Netmask */
              simSetServPortIPAddr(params.ip,L7_FALSE);
              simSetServPortIPNetMask(params.netMask,L7_FALSE);
              simSetServPortIPGateway(params.gateway,L7_FALSE);
              servPortConfigStatus = L7_SYSCONFIG_MODE_COMPLETE;

              bootpNotifyRegisteredUsers(&params);
            }
            else /* IP Addr conflict */
            {
              LOG_MSG("L7_bootp():Conflict w/offered IP Addr, error=%u\r\n", errorMsg);
            }
          }  
          else if (defaultServiceIpConfigExists == L7_TRUE)
          {
            simSetServPortIPGateway(params.gateway,L7_FALSE);
            servPortConfigStatus = L7_SYSCONFIG_MODE_COMPLETE;		  
			
            bootpNotifyRegisteredUsers(&params);            
          }
		  
          bootpConfigSemFree ();
        }
        else
        {
          LOG_MSG("L7_bootp():getParamsFromBootpStruct()returned failure for service port");
        }
      }
    }
    
    if ((networkConfigMode == L7_SYSCONFIG_MODE_BOOTP) &&
        (networkConfigStatus != L7_SYSCONFIG_MODE_COMPLETE))
    {  
      if (defaultNetworkIpConfigExists == L7_TRUE)
      {
        rc = bootpVendorSpecificValuesRequest(L7_LOCAL_NETWORK_PORT_IF, &network_bootp_data, timeout);
      }
      else
      {
        rc = L7_bootp_do(L7_LOCAL_NETWORK_PORT_IF, &network_bootp_data, timeout);
      }

      if ( rc == L7_TRUE )
      {
        if (L7_SUCCESS == getParamsFromBootpStruct(&network_bootp_data, &params))
        {
          /*
          ** Check for IP Address conflicts with other interfaces
          */
		  bootpConfigSemGet ();

          if (defaultServiceIpConfigExists == L7_FALSE)
          {
            if (sysapiIPConfigConflict(FD_CNFGR_NIM_MIN_SERV_PORT_INTF_NUM,
                                       params.ip, 
                                       params.netMask, 
                                       (L7_uint32 *)&errorMsg) == L7_FALSE)
            { 
              /* No conflict */
              /*Set IP Addresss, Default Gateway, Netmask */
              simSetSystemIPAddr(params.ip,L7_FALSE);
              simSetSystemIPNetMask(params.netMask);
              simSetSystemIPGateway(params.gateway,L7_FALSE);
              networkConfigStatus = L7_SYSCONFIG_MODE_COMPLETE;  

              bootpNotifyRegisteredUsers(&params);
            }
            else /* IP Addr conflict */
            {
              LOG_MSG("L7_bootp():Conflict w/offered IP Addr, error=%u\r\n", errorMsg);
            }
          }  
          else if (defaultNetworkIpConfigExists == L7_TRUE)
          {
            simSetSystemIPGateway(params.gateway,L7_FALSE);
            networkConfigStatus = L7_SYSCONFIG_MODE_COMPLETE;			
			
            bootpNotifyRegisteredUsers(&params);            
          }
		  
          bootpConfigSemFree ();
        }
        else
        {
          LOG_MSG("L7_bootp():getParamsFromBootpStruct()returned failure for service port");
        }
      }
    }


    /* Recheck after some time .....*/
    osapiSleep(10);
  }

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Create configuration protection semaphore.
*
* @comments The BOOTP task is started by SIM in phase 2.
*			The configuration protection semaphore allows the task to 
*			read configuration only when the system is in phase 3.
*
* @end
*************************************************************************/
void bootpConfigSemCreate (void)
{

   bootpConfigSem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
   if (bootpConfigSem == L7_NULL) 
   {
	   LOG_ERROR (0);
   }
}

/**************************************************************************
* @purpose  Get the configuration semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void bootpConfigSemGet (void)
{
  L7_RC_t rc;

   rc = osapiSemaTake (bootpConfigSem, L7_WAIT_FOREVER);
   if (rc != L7_SUCCESS) 
   {
	   LOG_ERROR (rc);
   }
}

/**************************************************************************
* @purpose  Get the configuration semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void bootpConfigSemFree (void)
{
	L7_RC_t rc;

	rc = osapiSemaGive (bootpConfigSem);
	if (rc != L7_SUCCESS) 
	{
		LOG_ERROR (rc);
	}
}

/**************************************************************************
* @purpose  Create notification protection semaphore.
*
* @comments The BOOTP task is started by SIM in phase 2.
*			The notification protection semaphore allows the task to read
*           records in notify callback table synchronously with client tasks.
*
* @end
*************************************************************************/
void bootpNotificationSemCreate (void)
{
  bootpNotificationSem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (bootpNotificationSem == L7_NULL) 
  {
    LOG_ERROR (0);
  }
}

/**************************************************************************
* @purpose  Get the notification semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void bootpNotificationSemGet (void)
{
  L7_RC_t rc;
 
  rc = osapiSemaTake (bootpNotificationSem, L7_WAIT_FOREVER);
  if (rc != L7_SUCCESS) 
  {
    LOG_ERROR (rc);
  }
}

/**************************************************************************
* @purpose  Give the notification semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void bootpNotificationSemFree (void)
{
  L7_RC_t rc;

  rc = osapiSemaGive (bootpNotificationSem);
  if (rc != L7_SUCCESS) 
  {
    LOG_ERROR (rc);
  }
}

/*********************************************************************
* @purpose  Re-init bootp protocol.
*
* @param    none
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
void bootpTaskReInit()
{
  networkConfigStatus =  L7_SYSCONFIG_MODE_NOT_COMPLETE;                                    
  servPortConfigStatus = L7_SYSCONFIG_MODE_NOT_COMPLETE;                                   
}

/**************************************************************************
* @purpose  BOOTP task startup function.
*            
* @comments None.
*
* @end
*************************************************************************/

void bootpTaskStart()
{
  L7_BootP_task_id = osapiTaskCreate( "BootP", L7_bootp, 0, 0,
                                      L7_DEFAULT_STACK_SIZE,
                                      L7_DEFAULT_TASK_PRIORITY,
                                      L7_DEFAULT_TASK_SLICE);
  if (L7_BootP_task_id == L7_ERROR)
    LOG_MSG("Failed to Create BootP Task.");

}

/*********************************************************************
* @purpose  Notification Registration 
*
* @param    L7_uint32  component_ID   @b((input)) componant id
* @param    L7_uint32  (*notify)(L7_bootp_dhcp_t)   @b((input))  function
*           to callback
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t bootpNotificationRegister( L7_COMPONENT_IDS_t component_ID,
                                   L7_RC_t (*notifyFuncPtr)(L7_bootp_dhcp_t *networkParamsPtr))
{
  if (component_ID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("BOOTP registrar ID %u greater than Last Component ID\n", component_ID);
    return(L7_FAILURE);
  }

  if ((L7_uint32)bootpNotifyList[component_ID].notifyFunction != L7_NULL)
  {
    LOG_MSG("BOOTP registrar ID %u already registered\n", component_ID);
    return(L7_FAILURE);
  }
  
  bootpNotificationSemGet();
  bootpNotifyList[component_ID].registrar_ID = component_ID;
  bootpNotifyList[component_ID].notifyFunction = notifyFuncPtr;
  bootpNotificationSemFree();
  
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose Deregister the routine to be called when a BOOTP request completes.
*
* @param   L7_COMPONENT_IDS_t componentId  @b((input)) component ID      
*
* @returns L7_SUCCESS
* @returns L7_FAILURE, invalid argument
*
* @comments
*                                 
* @end
*
*********************************************************************/
L7_RC_t bootpNotificationDeregister(L7_COMPONENT_IDS_t componentId)
{
  if ( componentId >= L7_LAST_COMPONENT_ID )
  {
    LOG_MSG("BOOTP registrar ID %u greater than Last Component ID\n", componentId);
    return L7_FAILURE;
  }

  bootpNotificationSemGet();
  bootpNotifyList[componentId].notifyFunction = L7_NULLPTR;
  bootpNotifyList[componentId].registrar_ID = L7_NULL;
  bootpNotificationSemFree();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Call registered users with information 
*
* @param L7_bootp_dhcp_t networkParamsPtr @b((input)) pointer to
*        network parameters structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
void bootpNotifyRegisteredUsers(L7_bootp_dhcp_t *networkParamsPtr)
{
  L7_uint32 i;

  bootpNotificationSemGet();

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (bootpNotifyList[i].registrar_ID != L7_NULL)
    {
      (void)(*bootpNotifyList[i].notifyFunction)(networkParamsPtr);
    }
  }
  
  bootpNotificationSemFree();
  
  return;
}

#else /* NO_PRODUCT_BOOTP_SUPPORT */
void bootpTaskReInit()
{
}

void bootpConfigSemFree (void)
{
}

void bootpConfigSemCreate (void)
{
}
void bootpConfigSemGet (void)
{
}

L7_RC_t bootpCnfgrInitPhase1Process(void)
{
  return L7_FAILURE;
}

L7_RC_t bootpCnfgrUconfigPhase2(void)
{
  return L7_FAILURE;
}

L7_RC_t bootpNotificationRegister( L7_COMPONENT_IDS_t component_ID,
                                   L7_RC_t (*notifyFuncPtr)(L7_bootp_dhcp_t *networkParamsPtr))
{
  return L7_FAILURE;
}

L7_RC_t bootpNotificationDeregister(L7_COMPONENT_IDS_t componentId)
{
  return L7_FAILURE;
}

#endif /* NO_PRODUCT_BOOTP_SUPPORT */
/* End of File */

