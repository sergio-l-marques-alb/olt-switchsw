/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\application\layer2\pbvlan\pbVlan.c
*
* @purpose Protocol-Based Vlan Utilities File
*
* @component Protocol-based VLANs
*
* @comments None
*
* @create 01/14/2002
*
* @author rjindal
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#include <string.h>

#include "l7_common.h"
#include "commdefs_pkg.h"
#include "defaultconfig.h"
#include "comm_mask.h"
#include "nvstoreapi.h"
#include "osapi.h"
#include "nimapi.h"
#include "log.h"
#include "dtlapi.h"
#include "dot1q_api.h"
#include "usmdb_util_api.h"
#include "pbvlan.h"

pbVlanCfgData_t    pbVlanCfgData;
PORTEVENT_MASK_t   pbVlanEventMask_g;
L7_uint32         *pbVlanMaskOffsetToIntIfNum = L7_NULLPTR;
void              *pbVlan_Queue               = L7_NULLPTR;

/*********************************************************************
*
* @purpose  Build default pbVlan config data.
*
* @param    L7_uint32   ver   Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void pbVlanBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 i, j;

  bzero((char*)&pbVlanCfgData, sizeof(pbVlanCfgData_t));

  for (i = 0; i <= L7_PBVLAN_MAX_NUM_GROUPS; i++)
  {
    bzero(pbVlanCfgData.group[i].name, sizeof(pbVlanCfgData.group[i].name));
    pbVlanCfgData.group[i].inUse = L7_FALSE;
    pbVlanCfgData.group[i].groupID = L7_PBVLAN_NULL_GROUP;
    pbVlanCfgData.group[i].vlanid = L7_PBVLAN_NULL_VLANID;

    for (j = 0; j < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; j++)
    {
      pbVlanCfgData.group[i].protocol[j] = L7_PBVLAN_INVALID_PROTOCOL;
    }

    for (j = 1; j <= (L7_uint32)L7_MAX_INTERFACE_COUNT; j++)
    {
      /* turn off all bits in 'ports' mask */
      NIM_INTF_CLRMASKBIT(pbVlanCfgData.group[i].ports, j);
    }
  }

  for (j = 0; j < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; j++)
  {
     pbVlanCfgData.etherProtocol[j] = L7_PBVLAN_INVALID_PROTOCOL;
  }

  /* Build header */
  strcpy((char *)&pbVlanCfgData.cfgHdr.filename, PBVLAN_CFG_FILENAME);
  pbVlanCfgData.cfgHdr.version = ver;
  pbVlanCfgData.cfgHdr.componentID = L7_PBVLAN_COMPONENT_ID;
  pbVlanCfgData.cfgHdr.type = L7_CFG_DATA;
  pbVlanCfgData.cfgHdr.length = (L7_uint32)sizeof(pbVlanCfgData);
  pbVlanCfgData.cfgHdr.dataChanged = L7_FALSE;
}

/*********************************************************************
*
* @purpose  Save pbVlan user config file to NVStore.
*
* @param    void
*
* @returns  L7_SUCCESS or
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_uint32 pbVlanSave(void)
{
  L7_uint32 rc = L7_SUCCESS;

  if (pbVlanCfgData.cfgHdr.dataChanged == L7_TRUE)
  {
    pbVlanCfgData.cfgHdr.dataChanged = L7_FALSE;
    pbVlanCfgData.checkSum = nvStoreCrc32((L7_uchar8 *)&pbVlanCfgData,
                                          (L7_uint32)(sizeof(pbVlanCfgData)
                                                      - sizeof(pbVlanCfgData.checkSum)));
    /* call save NVStore routine */
    if (sysapiCfgFileWrite(L7_PBVLAN_COMPONENT_ID, PBVLAN_CFG_FILENAME, (L7_char8 *)&pbVlanCfgData,
                     (L7_int32)sizeof(pbVlanCfgData)) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PBVLAN_COMPONENT_ID,
              "Error on call to osapiFsWrite routine on config file %s\n",
              PBVLAN_CFG_FILENAME);
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Check if pbVlan user config data has changed.
*
* @param    void
*
* @returns  L7_TRUE or
* @returns  L7_FALSE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_BOOL pbVlanHasDataChanged(void)
{
  return pbVlanCfgData.cfgHdr.dataChanged;
}
void pbVlanResetDataChanged(void)
{
  pbVlanCfgData.cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Print the current PBVLAN config values to
*           serial port
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pbVlanConfigDump(void)
{
  L7_RC_t rc;
  L7_char8 buf[32];
  L7_uint32 groupID;
  L7_uint32 i;
  L7_uint32 count;
  L7_uint32 list[L7_MAX_INTERFACE_COUNT + 1];
  L7_uint32 numPorts;

  rc = pbVlanGroupGetNext(0, &groupID);
  while (rc != L7_FAILURE)
  {
    printf("\nGroup Name - %s\n", pbVlanCfgData.group[groupID].name);
    if (pbVlanCfgData.group[groupID].inUse == L7_TRUE)
      sprintf(buf,"%s","L7_TRUE");
    else
      sprintf(buf,"%s","L7_FALSE");
    printf("Group in Use - %s\n", buf);
    printf("Group ID - %d\n", (L7_int32)pbVlanCfgData.group[groupID].groupID);
    for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
    {
      switch (i)
      {
      case PBVLAN_PROTOCOL_INDEX_IP:
        if (pbVlanCfgData.group[groupID].protocol[i] == L7_USMDB_PROTOCOL_HEX_IP)
          printf("Protocol[%s] - %s\n","PBVLAN_PROTOCOL_INDEX_IP","L7_USMDB_PROTOCOL_HEX_IP");
        else
          printf("Protocol[%s] - %s\n","PBVLAN_PROTOCOL_INDEX_IP","Not in use.");
        break;
      case PBVLAN_PROTOCOL_INDEX_ARP:
        if (pbVlanCfgData.group[groupID].protocol[i] == L7_USMDB_PROTOCOL_HEX_ARP)
          printf("Protocol[%s] - %s\n","PBVLAN_PROTOCOL_INDEX_ARP","L7_USMDB_PROTOCOL_HEX_ARP");
        else
          printf("Protocol[%s] - %s\n","PBVLAN_PROTOCOL_INDEX_ARP","Not in use.");
        break;
      case PBVLAN_PROTOCOL_INDEX_IPX:
        if (pbVlanCfgData.group[groupID].protocol[i] == L7_USMDB_PROTOCOL_HEX_IPX)
          printf("Protocol[%s] - %s\n","PBVLAN_PROTOCOL_INDEX_IPX","L7_USMDB_PROTOCOL_HEX_IPX");
        else
          printf("Protocol[%s] - %s\n","PBVLAN_PROTOCOL_INDEX_IPX","Not in use.");
        break;
      default:
        printf("Protocol[%d] - %s\n", (L7_int32)i,"L7_PBVLAN_INVALID_PROTOCOL");
        break;
      }
    }
    printf("Vlan ID - %d\n", (L7_int32)pbVlanCfgData.group[groupID].vlanid);
    printf("Included Ports - ");
    memset(list, 0x00, sizeof(L7_uint32)*(L7_MAX_INTERFACE_COUNT + 1));
    (void) nimMaskToList(&pbVlanCfgData.group[groupID].ports, list, &numPorts);
    count = 0;
    while (numPorts > L7_NULL)
    {
      nimUSP_t usp;
      (void) nimGetUnitSlotPort(pbVlanMaskOffsetToIntIfNum[list[count++]], &usp);
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%d.%d.%d ", (L7_uint32)usp.unit, (L7_int32)usp.slot, (L7_int32)usp.port);
      numPorts--;
    }

    rc = pbVlanGroupGetNext(groupID,&groupID);
  }


  printf("Scaling Constants\n");
  printf("-----------------\n");


  printf("L7_PBVLAN_MAX_NUM_GROUPS - %d\n", L7_PBVLAN_MAX_NUM_GROUPS);
  printf("L7_PBVLAN_MAX_GROUP_NAME - %d\n", L7_PBVLAN_MAX_GROUP_NAME);
  printf("L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS - %d\n", L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS);
  printf("Sizeof(NIM_INTF_MASK_t) - %lu\n", (L7_ulong32)sizeof(NIM_INTF_MASK_t));

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To create all pbVlan groups that exist in the current
*           configuration.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanApplyConfigData(void)
{
  L7_uint32 i;
  L7_uint32 j;
  L7_uint32 count;
  L7_uint32 list[L7_MAX_INTERFACE_COUNT + 1];
  L7_uint32 numPorts;
  L7_uint32 tmp;
  /*L7_RC_t rc;*/

  for (i = 1; i <= L7_PBVLAN_MAX_NUM_GROUPS; i++)
  {
    if (pbVlanCfgData.group[i].inUse == L7_TRUE)
    {
      memset(list, 0x00, sizeof(L7_uint32)*(L7_MAX_INTERFACE_COUNT + 1));
      (void) nimMaskToList(&pbVlanCfgData.group[i].ports, list, &numPorts);
      tmp = numPorts;

      for (j = 0; j < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; j++)
      {
        if (pbVlanCfgData.group[i].protocol[j] != L7_NULL)
        {
          count = 0;
          numPorts = tmp;
          while (numPorts > L7_NULL)
          {
            (void) dtlPbVlanPortConfig(pbVlanMaskOffsetToIntIfNum[list[count++]],
                                     pbVlanCfgData.group[i].protocol[j],
                                     pbVlanCfgData.group[i].vlanid,
                                     L7_TRUE);
            numPorts--;
          }
        }
      }
    }
  }

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Search the PB Vlan configuration cache for occurences of
*           the interface.  If found, update the operational interface
*           table and apply changes to dtl.  This is the processing
*           needed for a interface create event from NIM.
*
* @param    intIfNum  the internal interface number for the interface
*                     being created
*
* @returns  L7_SUCCESS  interface data apply finished with no errors
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanPortConfigDataRead(L7_uint32 intIfNum)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 maskOffset;

  /* Store away the mask offset of the configured interfaces */
  nimConfigIdMaskOffsetGet(intIfNum, &maskOffset);

  pbVlanMaskOffsetToIntIfNum[maskOffset] = intIfNum;

  return rc;
}


/*********************************************************************
*
* @purpose  To Apply Port Configuration Data configuration.
*
* @param    intIfNum
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanPortDataApply(L7_uint32 intIfNum)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 maskOffset,i,j;

  nimConfigIdMaskOffsetGet(intIfNum, &maskOffset);

  for (i = 1; i <= L7_PBVLAN_MAX_NUM_GROUPS; i++)
  {
    if (pbVlanCfgData.group[i].inUse == L7_TRUE)
    {
      for (j = 0; j < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; j++)
      {
        if (pbVlanCfgData.group[i].protocol[j] != L7_NULL)
        {
          if (NIM_INTF_ISMASKBITSET(pbVlanCfgData.group[i].ports,maskOffset))
          {

            rc = dtlPbVlanPortConfig(intIfNum,
                                     pbVlanCfgData.group[i].protocol[j],
                                     pbVlanCfgData.group[i].vlanid,
                                     L7_TRUE);
          }
        }
      }
    }
  }
  return rc;
}


/*********************************************************************
*
* @purpose  To Remove Port Data configuration from hardware
*
* @param    intIfNum
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanPortDataRemove(L7_uint32 intIfNum)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 maskOffset,i,j;

  nimConfigIdMaskOffsetGet(intIfNum, &maskOffset);

  for (i = 1; i <= L7_PBVLAN_MAX_NUM_GROUPS; i++)
  {
    if (pbVlanCfgData.group[i].inUse == L7_TRUE)
    {
      for (j = 0; j < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; j++)
      {
        if (pbVlanCfgData.group[i].protocol[j] != L7_NULL)
        {
          if (NIM_INTF_ISMASKBITSET(pbVlanCfgData.group[i].ports,maskOffset))
          {

            rc = dtlPbVlanPortConfig(intIfNum,
                                     pbVlanCfgData.group[i].protocol[j],
                                     pbVlanCfgData.group[i].vlanid,
                                     L7_FALSE);
          }
        }
      }
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To Remove Port Configuration Data.
*
* @param    intIfNum
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanPortConfigRemove(L7_uint32 intIfNum)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 maskOffset,i,j;

  nimConfigIdMaskOffsetGet(intIfNum, &maskOffset);

  for (i = 1; i <= L7_PBVLAN_MAX_NUM_GROUPS; i++)
  {
    if (pbVlanCfgData.group[i].inUse == L7_TRUE)
    {
      for (j = 0; j < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; j++)
      {
        if (pbVlanCfgData.group[i].protocol[j] != L7_NULL)
        {
          if (NIM_INTF_ISMASKBITSET(pbVlanCfgData.group[i].ports,maskOffset))
          {
            NIM_INTF_CLRMASKBIT(pbVlanCfgData.group[i].ports,maskOffset);
          }
        }
      }
    }
  }
  /* clear the interface */
  pbVlanMaskOffsetToIntIfNum[maskOffset] = 0;

  return rc;
}


/*********************************************************************
*
* @purpose  To process NIM events.
*
* @param    L7_uint32  intIfNum  internal interface number
* @param    L7_uint32  event     event, defined by L7_PORT_EVENTS_t
* @param    L7_uint32  correlator correlator for the event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanIntfChangeCallback (L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;

  status.intIfNum     = intIfNum;
  status.component    = L7_PBVLAN_COMPONENT_ID;
  status.response.rc  = rc;
  status.event        = event;
  status.correlator   = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED;

  nimEventStatusCallback(status);

  if (pbVlanIsValidIntf(intIfNum) == L7_FALSE)
  {
    return rc;
  }

  switch (event)
  {
  case L7_CREATE:
    /*
    ** see if we have any configuration saved for this interface; if so
    ** update the operational tables
    */
    rc = pbVlanPortConfigDataRead(intIfNum);
    break;
  case L7_ATTACH:
    /*
    ** re-apply any operational data for this interface to dtl
    */
    rc = pbVlanPortDataApply(intIfNum);
    break;

  case L7_DETACH:
    /*
    ** remove any operational data for this interface and apply to dtl
    */
    rc = pbVlanPortDataRemove(intIfNum);
    break;

  case L7_DELETE:
    rc = pbVlanPortConfigRemove(intIfNum);
    break;

  default:
    break;
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Determine whether an interface is in the attached state.
*
* @param    intIfNum  @b{(input)}  internal interface number
*
* @returns  L7_TRUE if the interface is attached.
*           L7_FALSE otherwise.
*
* @notes    The interface is considered to be attached if the state is
*           either L7_INTF_ATTACHING or L7_INTF_ATTACHED.
*
* @end
*********************************************************************/
L7_BOOL pbVlanIntfIsAttached(L7_uint32 intIfNum)
{
    L7_NIM_QUERY_DATA_t queryData;

    /* get the interface state */
    queryData.intIfNum = intIfNum;
    queryData.request = L7_NIM_QRY_RQST_STATE;
    if (nimIntfQuery(&queryData) != L7_SUCCESS)
    {
        return L7_FALSE;
    }

    if ((queryData.data.state == L7_INTF_ATTACHING) ||
        (queryData.data.state == L7_INTF_ATTACHED))
        return L7_TRUE;
    else
        return L7_FALSE;
}

/*********************************************************************
* @purpose  Propogate Startup notifications
*
* @param    NIM_STARTUP_PHASE_t  startupPhase
*
* @notes
*
* @end
*********************************************************************/
void pbVlanStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    if (pbVlanIsValidIntf(intIfNum) == L7_TRUE)
    {
      switch (startupPhase)
      {
      case NIM_INTERFACE_CREATE_STARTUP:
        (void)pbVlanPortConfigDataRead(intIfNum);
        break;

      case NIM_INTERFACE_ACTIVATE_STARTUP:
        if (pbVlanIntfIsAttached(intIfNum) == L7_TRUE)
        {
          (void)pbVlanPortDataApply(intIfNum);
        }
        break;

      default:
        break;
      }
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  switch (startupPhase)
  {
    case NIM_INTERFACE_CREATE_STARTUP:
      /* Register with NIM to receive port CREATE/DELETE events */
      PORTEVENT_SETMASKBIT(pbVlanEventMask_g, L7_CREATE);
      PORTEVENT_SETMASKBIT(pbVlanEventMask_g, L7_DELETE);
      nimRegisterIntfEvents(L7_PBVLAN_COMPONENT_ID, pbVlanEventMask_g);
      break;

    case NIM_INTERFACE_ACTIVATE_STARTUP:
      PORTEVENT_SETMASKBIT(pbVlanEventMask_g, L7_ATTACH);
      PORTEVENT_SETMASKBIT(pbVlanEventMask_g, L7_DETACH);
      nimRegisterIntfEvents(L7_PBVLAN_COMPONENT_ID, pbVlanEventMask_g);
      break;

    default:
      break;
  }

  nimStartupEventDone(L7_PBVLAN_COMPONENT_ID);

  /* Inform cnfgr that pbVlan has completed it HW updates */
  if (startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
  {
    cnfgrApiComponentHwUpdateDone(L7_PBVLAN_COMPONENT_ID,
                                  L7_CNFGR_HW_APPLY_CONFIG);
  }
}

/*********************************************************************
*
* @purpose  To process vlan changes.
*
* @param    L7_uint32  vlanid    id of the vlan
* @param    L7_uint32  intIfNum  internal interface number
* @param    L7_uint32  event     event, defined by vlanNotifyEvent_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If a platform needs Vlans to be created, then this
*           function will have to be updated.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanVLANChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum,
                                 L7_uint32 event)
{
   L7_uint32      i = 0, vlanId = 0, numVlans = 0;

   for (i = 1; i<=L7_VLAN_MAX_MASK_BIT; i++)
    {
      if (vlanData->numVlans == 1)
      {
          vlanId = vlanData->data.vlanId;
          /* For any continue, we will break out */
          i = L7_VLAN_MAX_MASK_BIT + 1;
      }
      else
      {
          if (L7_VLAN_ISMASKBITSET(vlanData->data.vlanMask,i))
          {
              vlanId = i;
          }
          else
          {
              if (numVlans == vlanData->numVlans)
              {
                  /* Already taken care of all the bits in the mask so break out of for loop */
                  break;
              }
              else
              {
                  /* Vlan is not set check for the next bit since there are more bits that are set*/
                  continue;
              }
          }
      }
      switch (event)
      {
      case VLAN_ADD_NOTIFY:
        break;

      case VLAN_DELETE_NOTIFY:
        break;

      case VLAN_ADD_PORT_NOTIFY:
        break;

      case VLAN_DELETE_PORT_NOTIFY:
        break;

      default:
        break;
      }
      numVlans++;
   }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the array index in the protocol array.
*
* @param    L7_uint32  prtl   @b((input)) specified protocol
* @param    L7_uint32  index  @b((output)) array index
*
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupPrtlIndexGet(L7_uint32 prtl, L7_uint32 *index)
{
  L7_uint32 j;

  for (j = 0; j < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; j++)
  {
     if (pbVlanCfgData.etherProtocol[j] == prtl)
     {
        *index = j;
        return L7_SUCCESS;
     }
  }
  return L7_FAILURE;
}




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
void pbVlanBuildTestConfigData(void)
{
    L7_uint32 i, j, k;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/
    for (i = 0; i <= L7_PBVLAN_MAX_NUM_GROUPS; i++)
    {
      bzero(pbVlanCfgData.group[i].name, sizeof(pbVlanCfgData.group[i].name));

      strncpy(pbVlanCfgData.group[i].name, "TEST_NAME", L7_PBVLAN_MAX_GROUP_NAME);
      pbVlanCfgData.group[i].inUse = L7_TRUE;
      pbVlanCfgData.group[i].groupID = i;
      pbVlanCfgData.group[i].vlanid = i+1;

      for (j = 0; j < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; j++)
      {
        pbVlanCfgData.group[i].protocol[j] = i+2;
      }

      /* For test purposes, only turn on a subset of physical ports */
      k = 2 + i%L7_MAX_PORT_COUNT;  /* arbitrary seed */

      for (j = 1; j <= (L7_uint32)L7_MAX_INTERFACE_COUNT; j++)
      {
        if (j > L7_MAX_PORT_COUNT)
        {
            break;
        }

        /*  Force each group to have a different port list range */
        if (j < (2*k) )
        {
            continue;
        }


        /*  Force each group to have a different port list range */
        if (j > (4*k) )
        {
            continue;
        }
        NIM_INTF_SETMASKBIT(pbVlanCfgData.group[i].ports, j);

        k++;
      }
    }


 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   pbVlanCfgData.cfgHdr.dataChanged = L7_TRUE;
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
void pbVlanConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;
    L7_uint32     i;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &pbVlanCfgData.cfgHdr;
    sysapiPrintf("pFileHdr->filename    : %s\n", (char *)pFileHdr->filename);
    sysapiPrintf("pFileHdr->version     : %u\n", pFileHdr->version);
    sysapiPrintf("pFileHdr->componentID : %u\n", pFileHdr->componentID);
    sysapiPrintf("pFileHdr->type        : %u\n", pFileHdr->type);
    sysapiPrintf("pFileHdr->length      : %u\n", pFileHdr->length);
    sysapiPrintf("pFileHdr->dataChanged : %u\n", pFileHdr->dataChanged);

    /* Start of release I file header changes */

    sysapiPrintf("pFileHdr->savePointId : %u\n", pFileHdr->savePointId);
    sysapiPrintf("pFileHdr->targetDevice: %u\n", pFileHdr->targetDevice);
    for (i = 0; i < L7_FILE_HDR_PAD_LEN; i++)
    {
        sysapiPrintf("pFileHdr->pad[i]      : %u\n", pFileHdr->pad[i]);
    }

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    pbVlanConfigDump();


    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("pbVlanCfgData.checkSum : %u\n", pbVlanCfgData.checkSum);


}



/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/


