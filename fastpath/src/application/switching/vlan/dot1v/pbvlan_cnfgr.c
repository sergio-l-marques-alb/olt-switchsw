/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename pbvlan_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component
*
* @comments
*
* @create 07/28/2003
*
* @author msmith
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "defaultconfig.h"
#include "cnfgr.h"
#include "pbvlan.h"
#include "pbvlan_cnfgr.h"
#include "nvstoreapi.h"
#include "dot1q_api.h"


pbVlanCnfgrState_t pbVlanCnfgrState = PBVLAN_PHASE_INIT_0;
extern L7_uint32 *pbVlanMaskOffsetToIntIfNum;
extern pbVlanCfgData_t    pbVlanCfgData;
extern PORTEVENT_MASK_t   pbVlanEventMask_g;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for pbVlan component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the pbVlan comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void pbVlanApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response = 0;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             pbVlanRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;

  /* validate command type */
  if ( pCmdData != L7_NULL )
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if ( request > L7_CNFGR_RQST_FIRST &&
           request < L7_CNFGR_RQST_LAST )
      {
        /* validate command/event pair */
        switch ( command )
        {
          case L7_CNFGR_CMD_INITIALIZE:
            switch ( request )
            {
              case L7_CNFGR_RQST_I_PHASE1_START:
                if ((pbVlanRC = pbVlanCnfgrInitPhase1Process( pCmdData )) != L7_SUCCESS)
                {
                  pbVlanCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((pbVlanRC = pbVlanCnfgrInitPhase2Process( pCmdData )) != L7_SUCCESS)
                {
                  pbVlanCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((pbVlanRC = pbVlanCnfgrInitPhase3Process( pCmdData )) != L7_SUCCESS)
                {
                  pbVlanCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                pbVlanRC = pbVlanCnfgrNoopProcess( pCmdData );
                pbVlanCnfgrState = PBVLAN_PHASE_WMU;
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            switch ( request )
            {
              case L7_CNFGR_RQST_E_START:
                pbVlanCnfgrState = PBVLAN_PHASE_EXECUTE;

                pbVlanRC  = L7_SUCCESS;
                response  = L7_CNFGR_CMD_COMPLETE;
                reason    = 0;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:
            switch ( request )
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                pbVlanRC = pbVlanCnfgrNoopProcess( pCmdData );
                response = L7_CNFGR_CMD_COMPLETE;
                reason   = 0;
                pbVlanCnfgrState = PBVLAN_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                pbVlanRC = pbVlanCnfgrUconfigPhase2( pCmdData );
                pbVlanCnfgrState = PBVLAN_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            pbVlanRC = pbVlanCnfgrNoopProcess( pCmdData );
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */

      }
      else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;

      } /* endif validate request */

    }
    else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;

    } /* endif validate command type */

  }
  else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;

  } /* check for command valid pointer */

  /* return value to caller -
   * <prepare complesion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = pbVlanRC;
  if (pbVlanRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);


  return;
}

/*********************************************************************
*
* @purpose  Perform all Phase 1 initialization for the pbVlan component.
*
* @param    pCmdData  pointer to a configurator request control block structure
*
* @returns  L7_SUCCESS, if pbVlan Phase 1 init was successful
* @returns  L7_FAILURE, if pbVlan Phase 1 was not successful
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanCnfgrInitPhase1Process(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_RC_t rc=L7_SUCCESS;

  pbVlanMaskOffsetToIntIfNum = osapiMalloc(L7_PBVLAN_COMPONENT_ID, sizeof(L7_uint32)*L7_MAX_INTERFACE_COUNT);

  if (pbVlanMaskOffsetToIntIfNum == L7_NULLPTR)
  {
    rc = L7_FAILURE;
    return rc;
  }

  memset(pbVlanMaskOffsetToIntIfNum,0,sizeof(L7_uint32)*L7_MAX_INTERFACE_COUNT);

  return(rc);
}

/*********************************************************************
*
* @purpose  Perform all Phase 2 initialization for the pbVlan component.
*
* @param    pCmdData  pointer to a configurator request control block structure
*
* @returns  L7_SUCCESS, if pbVlan Phase 2 init was successful
* @returns  L7_FAILURE, if pbVlan Phase 2 was not successful
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanCnfgrInitPhase2Process(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_RC_t rc;
  nvStoreFunctionList_t notifyFunctionList;
  L7_uint32 eventMask = 0;

  /* perform registrations with other components in Phase 2 */

  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_PBVLAN_COMPONENT_ID;
  notifyFunctionList.notifySave     = pbVlanSave;
  notifyFunctionList.hasDataChanged = pbVlanHasDataChanged;
  notifyFunctionList.notifyConfigDump     = pbVlanConfigDump;
  notifyFunctionList.resetDataChanged = pbVlanResetDataChanged;

  eventMask = 0;

  if ((rc = nvStoreRegister(notifyFunctionList)) == L7_SUCCESS)
  {
    /* register NIM callback to support interface changes */
    if ((rc = nimRegisterIntfChange(L7_PBVLAN_COMPONENT_ID,
                                    pbVlanIntfChangeCallback,
                                    pbVlanStartupNotifyCallback,
                                    NIM_STARTUP_PRIO_PBVLAN)) == L7_SUCCESS)
    {
      /* Register with dot1q to receive vlan changes*/
      if ((rc = vlanRegisterForChange(pbVlanVLANChangeCallback, L7_PBVLAN_COMPONENT_ID, eventMask)) != L7_SUCCESS)
      {
        L7_assert(1);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PBVLAN_COMPONENT_ID,
                "Unable to register NIM callback."
                " Appears when nimRegisterIntfChange"
                " fails to register pbVlan for link state changes.");
      }
    }
    else
    {
      L7_assert(1);
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PBVLAN_COMPONENT_ID,
              "Unable to register pbVlan callback with vlans."
              " Appears when vlanRegisterForChange fails to register"
              " pbVlan for vlan changes.");
    }
  }
  else
  {
    L7_assert(1);
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PBVLAN_COMPONENT_ID,
            "Unable to register pbVlan callback with nvStore."
            " Appears when nvStoreRegister fails to register "
            "save and restore functions for configuration save.");
  }
  return(rc);
}

/*********************************************************************
*
* @purpose  Perform all Phase 3 initialization for the pbVlan component.
*
* @param    pCmdData  pointer to a configurator request control block structure
*
* @returns  L7_SUCCESS, if pbVlan Phase 3 init was successful
* @returns  L7_FAILURE, if pbVlan Phase 3 was not successful
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanCnfgrInitPhase3Process(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_RC_t rc;

  /* Clear the NIM port event mask until ready to receive events */
  memset(&pbVlanEventMask_g, 0, sizeof(pbVlanEventMask_g));
  nimRegisterIntfEvents(L7_PBVLAN_COMPONENT_ID, pbVlanEventMask_g);

  /* read saved config */
  rc = sysapiCfgFileGet(L7_PBVLAN_COMPONENT_ID, PBVLAN_CFG_FILENAME, (L7_char8 *)&pbVlanCfgData,
                      (L7_uint32)sizeof(pbVlanCfgData), &pbVlanCfgData.checkSum,
                        PBVLAN_CFG_VER_CURRENT, pbVlanBuildDefaultConfigData, pbvlanMigrateConfigData);

  return(rc);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.
*
* @param    pResponse - @b{(output)}  Response always command complete.
*
* @param    pReason   - @b{(output)}  Always 0
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           None.
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t pbVlanCnfgrNoopProcess(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t pbVlanCnfgrUconfigPhase2(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_RC_t rc = L7_SUCCESS;

  /* reset internal state of component to point where waiting for
  ** Phase 3 Init request is expected
  */

  bzero((char*)&pbVlanCfgData, sizeof(pbVlanCfgData_t));
  return(rc);
}

/*********************************************************************
* @purpose  This function undoes pbVlanCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pbVlanCnfgrFiniPhase1Process()
{
  if (pbVlanMaskOffsetToIntIfNum != L7_NULLPTR)
    osapiFree(L7_PBVLAN_COMPONENT_ID, pbVlanMaskOffsetToIntIfNum);

  return;
}

/*********************************************************************
* @purpose  This function undoes pbVlanCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pbVlanCnfgrFiniPhase2Process()
{
  L7_RC_t rc;
  nvStoreFunctionList_t notifyFunctionList;

  /* perform registrations with other components in Phase 2 */

  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID     = L7_PBVLAN_COMPONENT_ID;
  notifyFunctionList.notifySave       = L7_NULLPTR;
  notifyFunctionList.hasDataChanged   = L7_NULLPTR;
  notifyFunctionList.notifyConfigDump = L7_NULLPTR;

  if ((rc = nvStoreRegister(notifyFunctionList)) == L7_SUCCESS)
  {
    /* register NIM callback to support interface changes */
    if ((rc = nimDeRegisterIntfChange(L7_PBVLAN_COMPONENT_ID)) == L7_SUCCESS)
    {
      /* Register with dot1q to receive vlan changes*/
      if ((rc = vlanRegisterForChange(L7_NULLPTR, L7_PBVLAN_COMPONENT_ID,0)) != L7_SUCCESS)
      {
        L7_assert(1);
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PBVLAN_COMPONENT_ID,
                "Unable to deregister NIM callback\n");
      }
    }
    else
    {
      L7_assert(1);
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PBVLAN_COMPONENT_ID,
              "Unable to deregister pbVlan callback with vlans\n");
    }
  }
  else
  {
    L7_assert(1);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PBVLAN_COMPONENT_ID,
            "Unable to deregister pbVlan callback with nvStore\n");
  }
  return;
}

/*********************************************************************
* @purpose  This function undoes pbVlanCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void pbVlanCnfgrFiniPhase3Process()
{
  return;
}
