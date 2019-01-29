/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_api.c
*
* @purpose VOIP API
*
* @component VOIP
*
* @comments none
*
* @create 05/12/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_common.h"
#include "voip_api.h"
#include "voip_util.h"
#include "voip_parse.h"

extern voipCfgData_t  *voipCfgData;
extern osapiRWLock_t  voipCfgRWLock;

/*********************************************************************
* @purpose  Enables or disables the VOIP profile on interface
*
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      enabled or disabled
* @param  L7_uint32 mode      (input)  Mode value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipIntfVoipModeSet(L7_uint32 interface, L7_uint32 mode)
{
  voipIntfCfgData_t *voipCfg = L7_NULLPTR;
  
  /* check mode */
  if((mode != L7_ENABLE) && (mode != L7_DISABLE))
  {
    return L7_FAILURE;
  }
  
  (void)osapiWriteLockTake(voipCfgRWLock, L7_WAIT_FOREVER);
   
  if (interface)
  {
    if (voipMapIntfIsConfigurable(interface, &voipCfg) != L7_TRUE)
    {
      (void)osapiWriteLockGive(voipCfgRWLock);
      return L7_FAILURE;
    }
  }
  else
  {
    voipCfg = &voipCfgData->voipIntfCfgData[interface];
  }
  
  if(mode == voipCfg->voipProfile)
  {
     (void)osapiWriteLockGive(voipCfgRWLock);
     return L7_SUCCESS;
  } 
  
  if (interface)
  {
    if(voipProfileIntfModeApply(interface,mode,
                                voipCfg->voipBandwidth)!=L7_SUCCESS)
    {
       (void)osapiWriteLockGive(voipCfgRWLock);
       return L7_FAILURE;
    }	 
  }
  voipCfg->voipProfile = mode;
  voipCfgData->cfgHdr.dataChanged = L7_TRUE;
  
  (void)osapiWriteLockGive(voipCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Mode of VOIP profile on interface
*
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      be checked 
* @param  L7_uint32 *mode     (output)  Mode value
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipIntfVoipModeGet(L7_uint32 interface, L7_uint32 *mode)
{
  voipIntfCfgData_t *voipCfg = L7_NULLPTR;
  
  (void )osapiReadLockTake(voipCfgRWLock,L7_WAIT_FOREVER);

  if (interface)
  {
    if (voipMapIntfIsConfigurable(interface, &voipCfg) != L7_TRUE)
    {
      (void)osapiReadLockGive(voipCfgRWLock);
      return L7_FAILURE;
    }
  }
  else
  {
    voipCfg = &voipCfgData->voipIntfCfgData[interface];
  }

  *mode = voipCfg->voipProfile;

  (void )osapiReadLockGive(voipCfgRWLock); 
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Sets the VOIP profile Min Bandwidth on the interface
*
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      enabled or disabled
* @param  L7_uint32 bandwidth (input)  Bandwidth value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipIntfVoipMinBandwidthSet(L7_uint32 interface, L7_uint32 bandwidth)
{
  voipIntfCfgData_t *voipCfg = L7_NULLPTR;
  
  (void)osapiWriteLockTake(voipCfgRWLock, L7_WAIT_FOREVER);

  if (interface)
  {
    if (voipMapIntfIsConfigurable(interface, &voipCfg) != L7_TRUE)
    {
      (void)osapiWriteLockGive(voipCfgRWLock);
      return L7_FAILURE;
    }
  }
  else
  {
    voipCfg = &voipCfgData->voipIntfCfgData[interface];
  }

  voipCfg->voipBandwidth = bandwidth;
  voipCfgData->cfgHdr.dataChanged = L7_TRUE;
  
  (void)osapiWriteLockGive(voipCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Mode of VOIP profile Min bandwidth on the interface
*
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      be checked 
* @param  L7_uint32 *bandwidth (output)  Bandwidth value
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipIntfVoipMinBandwidthGet(L7_uint32 interface, L7_uint32 *bandwidth)
{
  voipIntfCfgData_t *voipCfg = L7_NULLPTR;
  
  (void )osapiReadLockTake(voipCfgRWLock,L7_WAIT_FOREVER);

  if (interface)
  {
    if (voipMapIntfIsConfigurable(interface, &voipCfg) != L7_TRUE)
    {
      (void)osapiReadLockGive(voipCfgRWLock);
      return L7_FAILURE;
    }
  }
  else
  {
    voipCfg = &voipCfgData->voipIntfCfgData[interface];
  }

  *bandwidth =  voipCfg->voipBandwidth;

  (void )osapiReadLockGive(voipCfgRWLock); 
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the status of VOIP profile on interface
*
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      be checked 
* @param  L7_uint32 *mode     (output)  Mode value
*
* @param  L7_uint32 *bandwidth (output)  Bandwidth value
*
* @param  L7_uint32 *CosQueue (output)  Cos Queue
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipIntfVoipProfileGet(L7_uint32 interface, L7_uint32 *mode, 
                               L7_uint32 *bandwidth, L7_uint32 *CosQueue)
{
  voipIntfCfgData_t *voipCfg = L7_NULLPTR;
  
  (void )osapiReadLockTake(voipCfgRWLock,L7_WAIT_FOREVER);

  if (interface)
  {
    if (voipMapIntfIsConfigurable(interface, &voipCfg) != L7_TRUE)
    {
      (void)osapiReadLockGive(voipCfgRWLock);
      return L7_FAILURE;
    }
  }
  else
  {
    voipCfg = &voipCfgData->voipIntfCfgData[interface];
  }

  *mode      =  voipCfg->voipProfile;
  *bandwidth =  voipCfg->voipBandwidth;
  *CosQueue = FD_VOIP_COS_QUEUE;
  
  (void )osapiReadLockGive(voipCfgRWLock); 
  return L7_SUCCESS;
}

/* DEBUG FUNCTIONS */

/*********************************************************************
* @purpose DUMP configuration structures 
*
* @param   none
*
* @returns void
*
* @notes    none
*
* @end
*********************************************************************/
void voipDebugCfgShow()
{
  L7_uint32 interface;
  nimUSP_t usp;  
 
  sysapiPrintf("Dumping CFG\n");
  for(interface = 0;interface < L7_VOIP_MAX_INTF ;interface++)
  {
    memset((void *)&usp, 0, sizeof(nimUSP_t));
    if(nimGetUnitSlotPort(interface, &usp) == L7_SUCCESS)
    {
       sysapiPrintf( "USP = %d/%d/%d  (intIfNum  = %d)\n",
       usp.unit, usp.slot, usp.port, interface);
    }
    else
    { 
      continue;
    }
    sysapiPrintf("VoIP Profile = %s\n",voipCfgData->voipIntfCfgData[interface].voipProfile?"ENABLE":"DISABLE");
    sysapiPrintf("VoIP Bandwidth = %u\n",voipCfgData->voipIntfCfgData[interface].voipBandwidth);
  }
 
}

/*********************************************************************
*
* @purpose  Return VoIP Config entry
*
* @param  L7_uint32 interface (input)  Internal Interface
*                            
* @returns  VoIP Config entry or NULL
*
* @notes    given a intIfNum return the cfg structure pointer 
*
* @end
*********************************************************************/
voipIntfCfgData_t *voipIntfCfgEntryGet(L7_uint32 intIfNum)
{
  L7_uint32 i;
  nimConfigID_t nullConfigId;
  nimConfigID_t configId;

  memset((void *)&nullConfigId, 0x00, (L7_uint32)sizeof(nimConfigID_t));
  
  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
  {
    return L7_NULLPTR;
  }

  for (i = 1; i < L7_VOIP_MAX_INTF; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(voipCfgData->voipIntfCfgData[i].configId)))
    {
      return &voipCfgData->voipIntfCfgData[i];
    }
  }
  return L7_NULLPTR;
}

