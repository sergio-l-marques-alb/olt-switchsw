/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename nim_config.c
*
* @purpose NIM config file management
*
* @component nim
*
* @comments none
*
* @create 08/30/2000
*
* @author bmutz
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include <nvstoreapi.h>
#include "osapi.h"
#include "defaultconfig.h"
#include "nim.h"
#include "l7_product.h"
#include "sysapi.h"
#include "nim_data.h"
#include "nim_config.h"
#include "nim_util.h"
#include "platform_config.h"

/*********************************************************************
* @purpose  Allocate the memory for the memory copy of the config file
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimConfigPhase2Init(void)
{
  L7_uint32 fileSize = 0;
  L7_RC_t rc  = L7_SUCCESS;

  fileSize = sizeof(nimConfigData_t) +
             (sizeof(nimCfgPort_t)*(platIntfTotalMaxCountGet()+1)) + sizeof(L7_uint32);

  /* allocate/initialize the memory to store the configuration data
      Not freed during operation */

  nimCtlBlk_g->nimConfigData = (nimConfigData_t *) osapiMalloc(L7_NIM_COMPONENT_ID, fileSize);

  if (nimCtlBlk_g->nimConfigData == L7_NULL)
  {
    NIM_LOG_ERROR("NIM:Couldn't allocate memory for the nimConfigData\n");
    return(L7_FAILURE);
  }
  else
  {
    memset((void*)nimCtlBlk_g->nimConfigData, 0,fileSize);
  }

  if ((rc = nimConfigFileOffsetsSet()) == L7_SUCCESS)
    rc = nimConfigFileHdrCreate();


  return(rc);
}

/*********************************************************************
* @purpose  initialize the config structures during phase 3 init
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void nimConfigInit(void)
{

  L7_uint32             headerSize, totalPortsSize,bufferSize;
  L7_RC_t               rc = L7_FAILURE;

  headerSize = sizeof(nimConfigData_t);
  totalPortsSize = sizeof(nimCfgPort_t) * (platIntfTotalMaxCountGet() + 1);
  bufferSize = headerSize + totalPortsSize + sizeof(L7_uint32);
  /*
   *  allocate memory for the first part of the file to find how
   *  interfaces were last created on this unit
   */

  if (nimCtlBlk_g->nimConfigData != L7_NULLPTR)
  {
    rc = sysapiCfgFileGet(L7_NIM_COMPONENT_ID, nimCtlBlk_g->nimFileName,
                          (L7_char8 *)nimCtlBlk_g->nimConfigData, bufferSize,
                          nimCtlBlk_g->nimConfigData->checkSum, NIM_CFG_VER_CURRENT,
                          nimConfigFileDefaultCreate,
                          nimMigrateConfigData);

    if (rc != L7_SUCCESS)
    {
      NIM_LOG_ERROR("NIM: Failed to read the config data\n");
    }
    else if ((rc = nimConfigFileOffsetsSet()) != L7_SUCCESS)
    {
      NIM_LOG_ERROR("NIM: Failed to set the offsets in the config file\n");
    }
    else
    {
      /* nothing to do */
      if (nimCtlBlk_g->nimConfigData->numOfInterfaces != platIntfTotalMaxCountGet())
      {
        nimConfigFileDefaultCreate(NIM_CFG_VER_CURRENT);
      }
    }
  }
  else
  {
    NIM_LOG_ERROR("NIM: Memory not created for config file\n");
    /* need an error handler here */
  }

  return;
}

/*********************************************************************
* @purpose  Setup the config file name
*
* @param    void
*
* @returns  L7_SUCCESS  Success or sysapiRegistrGet error code
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_RC_t nimFileSetup(void)
{
  sprintf ((L7_char8 *)nimCtlBlk_g->nimFileName, "%s",
           NIM_CFG_FILE_NAME);

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Saves all nim user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error from osapiFsWrite
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimSave(void)
{
  L7_uint32     j;
  L7_uint32     numOfPorts;
  L7_uint32     size,checksum;
  nimCfgPort_t  *nimCfgPort;
  L7_uint32 rc=L7_SUCCESS;

  if (nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged == L7_TRUE)
  {
    /* reset number of ports */
    numOfPorts = platIntfTotalMaxCountGet();

    size =  sizeof(nimConfigData_t) +                 /* base structure */
            sizeof(nimCfgPort_t) * (numOfPorts + 1) + /* cfgPorts[] */
            sizeof(L7_uint32);                        /* checksum */

    nimConfigFileOffsetsSet();
    nimConfigFileHdrCreate();

    /* reset the affected memory */
    memset(nimCtlBlk_g->nimConfigData->cfgPort,0x00,sizeof(nimCfgPort_t) * (numOfPorts+1)+ sizeof(L7_uint32));

    /* read the port configuration */
    nimCfgPort = nimCtlBlk_g->nimConfigData->cfgPort;

    /* copy the current interface configuration to to the unit it belongs to */
    for (j=0; j <= numOfPorts ; j++)
    {
      if (nimCtlBlk_g->nimPorts[j].present == L7_TRUE)
      {

        memcpy(&nimCfgPort[j],&nimCtlBlk_g->nimPorts[j].configPort,
               sizeof(nimCfgPort_t));
      }
    }

    /* create and copy the checksum to the newly created file */
    checksum = nvStoreCrc32((L7_char8 *)nimCtlBlk_g->nimConfigData, size - sizeof(L7_uint32));


    *(nimCtlBlk_g->nimConfigData->checkSum) = checksum;

    nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_FALSE;

    /* call save NVStore routine */
    rc = sysapiCfgFileWrite(L7_NIM_COMPONENT_ID, nimCtlBlk_g->nimFileName,
                           (L7_char8 *)nimCtlBlk_g->nimConfigData, size);

  }

  return(rc);
}

/*********************************************************************
* @purpose  Checks if nim user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL nimHasDataChanged(void)
{
  if (nimPhaseStatusCheck() != L7_TRUE) return(L7_ERROR);

  return(nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged);
}
void nimResetDataChanged(void)
{
  nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
* @purpose  Creates an unique interface id
*
* @param    usp         @b{(input)}  Pointer to nimUSP_t structure
* @param    configId    @b{(output)} Pointer to the new config ID
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimConfigIdCreate(nimUSP_t *usp, nimConfigID_t *configId)
{
  if (nimPhaseStatusCheck() != L7_TRUE) return(L7_ERROR);

  memset((void*)configId,0,sizeof(nimConfigID_t));

  configId->configSpecifier.usp.unit = usp->unit;
  configId->configSpecifier.usp.slot = usp->slot;
  configId->configSpecifier.usp.port = usp->port;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Migrate the config data
*
* @param    savedVersion  @b{(input)} version of the present config file
* @param    newVersion    @b{(input)} version to convert to
* @param    buffer        @b{(input)} the buffer of the cfg file
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Since this is the first release of Stacking, no migration to do
*
* @end
*********************************************************************/
void nimConfigConvert(L7_uint32 savedVersion,L7_uint32 newVersion,L7_char8 *buffer)
{

  /*
   *  allocate memory for the first part of the file to find how
   *  interfaces were last created on this unit
   */

  switch (savedVersion)
  {
    case NIM_CFG_VER_1:
    default:
      switch (newVersion)
      {
        case NIM_CFG_VER_2:
        default:

          nimConfigFileDefaultCreate(NIM_CFG_VER_CURRENT);

          break;
      }
      break;
  }

  nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_TRUE;


}

/*********************************************************************
* @purpose  Set either the saved config or the default config in the interface
*
* @param    port        @b{(input)}   The interface being manipulated
* @param    cfgVersion  @b{(input)}   version of the config
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t nimConfigSet(nimIntf_t *port, L7_uint32 cfgVersion)
{
  nimCfgPort_t    *localConfigData;
  nimConfigID_t   configInterfaceId;
  L7_uint32        numOfInterfaces;
  L7_BOOL          configDataFound = L7_FALSE;
  L7_uint32        counter;
  L7_RC_t   rc = L7_SUCCESS;

  if ((nimCtlBlk_g == L7_NULLPTR) || (nimCtlBlk_g->nimConfigData == L7_NULLPTR))
  {
    NIM_LOG_ERROR("NIM: Control block or config data not valid\n");
    rc = (L7_ERROR);
  }
  else
  {

    localConfigData = nimCtlBlk_g->nimConfigData->cfgPort;

    numOfInterfaces = nimCtlBlk_g->nimConfigData->numOfInterfaces;

    NIM_CONFIG_ID_COPY(&configInterfaceId ,&port->configInterfaceId);

    /* see if the config for the interface exists in the file */
    for (counter = 0; counter <= numOfInterfaces; counter++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&localConfigData[counter].configInterfaceId,&configInterfaceId))
      {
        memcpy(&port->configPort,&localConfigData[counter],sizeof(nimCfgPort_t));
        configDataFound = L7_TRUE;
        break;
      }
    }

    if (configDataFound == L7_FALSE)
    {
      /* use the default config as determined before */
      memcpy(&port->configPort.cfgInfo,&port->defaultCfg,sizeof(nimIntfConfig_t));
      NIM_CONFIG_ID_COPY(&port->configPort.configInterfaceId,&port->configInterfaceId);
      port->configPort.configIdMaskOffset = port->intfNo;
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  apply the config to the system
*
* @param    intIfNum    @b{(input)}   The intenal interface to apply
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t nimIntfConfigApply(L7_uint32 intIfNum)
{
  L7_uint32 rc = L7_SUCCESS;
  L7_BOOL bintfDataChanged;

  /* save the original value of the data changed flag to restore it at the end, as nim uses its
     own set apis internally */
  bintfDataChanged =  nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged;

  if (nimIntfParmCanSet(intIfNum, L7_INTF_PARM_SPEED) == L7_TRUE &&
      (rc = nimSetIntfSpeed(intIfNum, nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifSpeed)) == L7_FAILURE)
  {
    NIM_LOG_MSG("NIM: Error on call to nimSetIntfSpeed with internal interface number %d\n",intIfNum);
  }
  else if (nimIntfParmCanSet(intIfNum, L7_INTF_PARM_AUTONEG) == L7_TRUE &&
           (rc = nimSetIntfAutoNegoStatusCapabilities(intIfNum, nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.negoCapabilities)) == L7_FAILURE)
  {
    NIM_LOG_MSG("NIM: Error on call to nimSetIntfNegotiationCapabilities with internal interface number %d\n",intIfNum);
  }
  else if (nimIntfParmCanSet(intIfNum, L7_INTF_PARM_FRAMESIZE) == L7_TRUE &&
      (rc = nimSetIntfConfigMaxFrameSize(intIfNum, nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.cfgMaxFrameSize)) == L7_FAILURE)
  {
    NIM_LOG_MSG("NIM: Error on call to nimSetIntfConfigMaxFrameSize with internal interface number %d\n",intIfNum);
  }
  else if (nimIntfParmCanSet(intIfNum, L7_INTF_PARM_MACADDR) == L7_TRUE &&
      (rc = nimSetIntfAddrType(intIfNum, nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.addrType)) == L7_FAILURE)
  {
    NIM_LOG_MSG("NIM: Error on call to nimSetIntfAddrType with internal interface number %d\n",intIfNum);
  }
  else if (nimIntfParmCanSet(intIfNum, L7_INTF_PARM_LINKTRAP) == L7_TRUE &&
      (rc = nimSetIntfLinkTrap(intIfNum, nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.trapState)) == L7_FAILURE)
  {
    NIM_LOG_MSG("NIM: Error on call to nimSetIntfLinkTrap with internal interface number %d\n",intIfNum);
  }
  else if (nimIntfParmCanSet(intIfNum, L7_INTF_PARM_ADMINSTATE) == L7_TRUE &&
      (rc = nimSetIntfAdminState(intIfNum, nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.adminState)) == L7_FAILURE)
  {
    NIM_LOG_MSG("NIM: Error on call to nimSetIntfAdminState with internal intf %d\n",intIfNum);
  }
  else if (nimIntfParmCanSet(intIfNum, L7_INTF_PARM_ENCAPTYPE) == L7_TRUE &&
           (rc = nimEncapsulationTypeSet(intIfNum, nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.encapsType)) == L7_FAILURE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    /* The preconfigured interfaces return failure for this call. I think this is a glitch in
    ** ipmap, however no harm done.
    */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
            "NIM: Error on call to nimEncapsulationTypeSet with internal intf %d, %s\n",intIfNum, ifName);
    rc = L7_SUCCESS;
  }

  if (rc == L7_SUCCESS)
  {
    NIM_CRIT_SEC_WRITE_ENTER();

    /* restore previous value of the data changed flag as nim uses its own set apis internally */
    /* ideally this should not be the case */
    nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = bintfDataChanged;

    NIM_CRIT_SEC_WRITE_EXIT();
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the default config for the interface
*
* @param    intfDescr   @b{(input)}   A description of the interface being created
* @param    defaultCfg  @b{(output)}  The created config
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Since this is the first release of Stacking, no migration to do
*
* @end
*********************************************************************/
L7_RC_t nimConfigDefaultGet(nimIntfDescr_t *intfDescr, nimIntfConfig_t *defaultCfg)
{
  L7_RC_t rc = L7_SUCCESS;

  memset(defaultCfg->LAAMacAddr.addr, 0, 6);

  defaultCfg->addrType    = FD_NIM_MACTYPE;

  strcpy(defaultCfg->ifAlias, FD_NIM_IF_ALIAS);

  defaultCfg->trapState   = FD_NIM_TRAP_STATE;

  defaultCfg->encapsType  = FD_NIM_ENCAPSULATION_TYPE;

  defaultCfg->mgmtAdminState  = FD_NIM_ADMIN_STATE;
  defaultCfg->adminState  = FD_NIM_ADMIN_STATE;

  defaultCfg->nameType    = FD_NIM_NAME_TYPE;

  defaultCfg->ipMtu = FD_NIM_DEFAULT_MTU_SIZE;

  defaultCfg->cfgMaxFrameSize = FD_NIM_DEFAULT_MAX_FRAME_SIZE;

  if (intfDescr->phyCapability & L7_PHY_CAP_PORTSPEED_AUTO_NEG)
    defaultCfg->negoCapabilities =  L7_PORT_NEGO_CAPABILITY_ALL;
  else
    defaultCfg->negoCapabilities =  0;

  switch (intfDescr->ianaType)
  {
    case L7_IANA_FAST_ETHERNET:
      /* PTin modified: Auto-neg */
      defaultCfg->ifSpeed = (intfDescr->defaultSpeed == L7_PORTCTRL_PORTSPEED_AUTO_NEG) ? L7_PORTCTRL_PORTSPEED_AUTO_NEG : FD_NIM_FAST_ENET_SPEED;
      break;

    case L7_IANA_FAST_ETHERNET_FX:
      /* PTin modified: Auto-neg */
      defaultCfg->ifSpeed = (intfDescr->defaultSpeed == L7_PORTCTRL_PORTSPEED_AUTO_NEG) ? L7_PORTCTRL_PORTSPEED_AUTO_NEG : FD_NIM_FAST_ENET_FX_SPEED;
      break;

    case L7_IANA_GIGABIT_ETHERNET:
      /* PTin modified: Auto-neg */
      defaultCfg->ifSpeed = (intfDescr->defaultSpeed == L7_PORTCTRL_PORTSPEED_AUTO_NEG) ? L7_PORTCTRL_PORTSPEED_AUTO_NEG : FD_NIM_GIG_ENET_SPEED;
      break;

    /* PTin added: Speed 2.5G */
    case L7_IANA_2G5_ETHERNET:
      defaultCfg->ifSpeed = FD_NIM_2G5_ENET_SPEED;
      break;
    /* PTin end */

    case L7_IANA_10G_ETHERNET:
      /* PTin modified: Auto-neg */
      defaultCfg->ifSpeed = (intfDescr->defaultSpeed == L7_PORTCTRL_PORTSPEED_AUTO_NEG) ? L7_PORTCTRL_PORTSPEED_AUTO_NEG : FD_NIM_10G_ENET_SPEED;
      break;

    /* PTin added: Speed 40G */
    case L7_IANA_40G_ETHERNET:
      defaultCfg->ifSpeed = FD_NIM_40G_ENET_SPEED;
      break;

    /* PTin added: Speed 100G */
    case L7_IANA_100G_ETHERNET:
      defaultCfg->ifSpeed = FD_NIM_100G_ENET_SPEED;
      break;
    /* PTin end */

    case L7_IANA_LAG_DESC:
      defaultCfg->trapState           = L7_DISABLE;
      defaultCfg->adminState          = FD_DOT3AD_ADMIN_MODE;
      defaultCfg->ifSpeed             = L7_PORTCTRL_PORTSPEED_LAG;
      /* Negotiation capabilities not applicable to LAGs. The following also disables auto-negotiation. */
      defaultCfg->negoCapabilities    = 0;
      break;

    case L7_IANA_L2_VLAN:
      defaultCfg->adminState          = L7_ENABLE;
      defaultCfg->trapState           = L7_DISABLE;
      defaultCfg->ifSpeed             = L7_PORTCTRL_PORTSPEED_HALF_10T;
      /* Negotiation capabilities not applicable to VLANs. The following also disables auto-negotiation. */
      defaultCfg->negoCapabilities    = 0;
      break;

    case L7_IANA_OTHER_CPU:
      defaultCfg->ifSpeed = FD_NIM_OTHER_PORT_TYPE_SPEED;
      break;

    default:
      defaultCfg->ifSpeed = FD_NIM_OTHER_PORT_TYPE_SPEED;
      break;
  }

  return rc;
}
/*********************************************************************
* @purpose  Sets the offsets for ports and crc in the config struct
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    Since this is the first release of Stacking, no migration to do
*
* @end
*********************************************************************/
L7_RC_t nimConfigFileOffsetsSet()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 fileSize = 0;
  L7_uchar8   *nimFile;
  nimConfigData_t *tmpCfg;

  /* sizeof the config structure + the malloc'd data + CRC */
  fileSize = sizeof(nimConfigData_t) +
             (sizeof(nimCfgPort_t)*(platIntfTotalMaxCountGet()+1)) + sizeof(L7_uint32);

  NIM_CRIT_SEC_WRITE_ENTER();

  do
  {
    if (nimCtlBlk_g->nimConfigData == L7_NULL)
    {
      NIM_LOG_ERROR("NIM: Config file not allocated during init\n");
      rc = L7_FAILURE;
      break;
    }

    nimFile = (L7_uchar8 *)nimCtlBlk_g->nimConfigData;

    tmpCfg = nimCtlBlk_g->nimConfigData;

    /* set the pointer to the cfgPort in the flat array */
    tmpCfg->cfgPort = (nimCfgPort_t *) (nimFile + sizeof(nimConfigData_t));

    /* set the pointer to the checksum in the flat array */
    tmpCfg->checkSum = (L7_uint32 *) (nimFile + fileSize - sizeof(L7_uint32));


  } while ( 0 );

  NIM_CRIT_SEC_WRITE_EXIT();

  return rc;


}


/*********************************************************************
* @purpose  Sets the config file header
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Since this is the first release of Stacking, no migration to do
*
* @end
*********************************************************************/
L7_RC_t nimConfigFileHdrCreate()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_fileHdr_t  *myHdr;

  NIM_CRIT_SEC_WRITE_ENTER();

  do
  {
    if (nimCtlBlk_g == L7_NULLPTR)
    {
      rc = L7_FAILURE;
      NIM_LOG_ERROR("NIM: nimCtlBlk_g not allocated\n");
      break;
    }
    else if (nimCtlBlk_g->nimConfigData == L7_NULLPTR)
    {
      rc = L7_FAILURE;
      NIM_LOG_ERROR("NIM: Config buffer not allocated\n");
      break;
    }

    myHdr = &nimCtlBlk_g->nimConfigData->cfgHdr;

    memset((void *)myHdr,0,sizeof(L7_fileHdr_t));

    myHdr->version = NIM_CFG_VER_CURRENT;

    strncpy(myHdr->filename,NIM_CFG_FILE_NAME,L7_MAX_FILENAME - 1);

    myHdr->componentID = L7_NIM_COMPONENT_ID;

    /* sizeof the config structure + the malloc'd data + CRC - File Hdr*/
    myHdr->length = sizeof(nimConfigData_t) +
                    (sizeof(nimCfgPort_t)*(platIntfTotalMaxCountGet()+1)) +
                    sizeof(L7_uint32) ;

    nimCtlBlk_g->nimConfigData->numOfInterfaces = platIntfTotalMaxCountGet();


  } while ( 0 );

  NIM_CRIT_SEC_WRITE_EXIT();

  return rc;


}

/*********************************************************************
* @purpose  Get the default config for the interface
*
* @param    version   @b{(input)} The version to create a default for
*
* @returns  void
*
* @notes    Since this is the first release of Stacking, no migration to do
*
* @end
*********************************************************************/
void nimConfigFileDefaultCreate(L7_uint32 version)
{
  L7_uint32 nimHeaderSize,totalPortsSize,crcSize;

  do
  {
    nimHeaderSize = sizeof(nimConfigData_t);
    totalPortsSize = sizeof(nimCfgPort_t) * (platIntfTotalMaxCountGet() + 1);
    crcSize = sizeof(L7_uint32);
    NIM_CRIT_SEC_WRITE_ENTER();
    memset((void*)nimCtlBlk_g->nimConfigData,0,nimHeaderSize + totalPortsSize + crcSize);
    NIM_CRIT_SEC_WRITE_EXIT();
    nimConfigFileOffsetsSet();

    nimConfigFileHdrCreate();
    /* No need to protect the dataChanged as we have set it to 0
     * and the above 2 function do not modify it */
    nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_FALSE;

  } while ( 0 );

  return;
}

