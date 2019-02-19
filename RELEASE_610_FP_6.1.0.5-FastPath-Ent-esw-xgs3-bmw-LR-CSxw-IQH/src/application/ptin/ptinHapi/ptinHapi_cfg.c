/*
 * ptinHapi_cfg.c
 *
 *  Created on: 2010/04/08
 *      Author: Andre Brizido
 *
 *      Com este modulo seria possivel a partida integrar as aplicacoes ptin
 *      no Menu Debug do CLI Fast Path
 */
#include "ptinHapi_include.h"
#include "support_api.h"

void *ptinHapi_queue = L7_NULLPTR;  /* reference to the ptinHapi message queue */
void *ptinHapi_timer_queue = L7_NULLPTR; /* reference to the ptinHapi timer message queue */
static L7_uint32 ptinHapiCnfgrState = 0;
static L7_uint32 ptinHapi_timer_task_id = 0;
extern ptinHapiCnfgrState_t ptinHapiCnfgrState;
extern ptinHapiDeregister_t ptinHapiDeregister;

ptinHapiDebugCfg_t ptinHapiDebugCfg;    /* Debug Configuration File Overlay */
extern ptinHapiCfg_t                  *ptinHapiCfg;

/*********************************************************************
* @purpose  Saves ptinHapi configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ptinHapiCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t ptinHapiSave(void)
{
    L7_RC_t rc;

    rc = L7_SUCCESS;
    return(rc);
}

/*********************************************************************
* @purpose  Checks if ptinHapi user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL ptinHapiHasDataChanged(void)
{
  if (ptinHapiDeregister.ptinHapiHasDataChanged == L7_TRUE)
  {
   return L7_FALSE;
  }
  return ptinHapiCfg->hdr.dataChanged;
}
void ptinHapiResetDataChanged(void)
{
  ptinHapiCfg->hdr.dataChanged = L7_FALSE;
  return;
}


/*********************************************************************
*
* @purpose  System Initialization for IGMP Snooping component
*
* @param    none
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ptinHapiInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  System Init Undo for IGMP Snooping component
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
void ptinHapiInitUndo()
{
//  if (ptinHapi_queue != L7_NULLPTR)
//    osapiMsgQueueDelete(ptinHapi_queue);

  if (ptinHapiCnfgrState != L7_ERROR)
    osapiTaskDelete(ptinHapiCnfgrState);

  if (ptinHapi_timer_queue != L7_NULLPTR)
    osapiMsgQueueDelete(ptinHapi_timer_queue);

  if (ptinHapi_timer_task_id != L7_ERROR)
    osapiTaskDelete(ptinHapi_timer_task_id);

  ptinHapiCnfgrState = PTINHAPI_PHASE_INIT_0;
}


/*********************************************************************
* @purpose  Copies ptinHapi config data for the bridge and instance alone
*
* @param    void
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ptinHapiCopyConfigData(void)
{
   return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Saves ptinHapi configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ptinHapiDebugCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t ptinHapiDebugSave(void)
{
  L7_RC_t rc;

  rc = L7_SUCCESS;

  return(rc);
}

/*********************************************************************
* @purpose  Checks if ptinHapi debug config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL ptinHapiDebugHasDataChanged(void)
{
  return ptinHapiDebugCfg.hdr.dataChanged;
}


/*********************************************************************
* @purpose  Restores ptinHapi debug configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ptinHapiDebugCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t ptinHapiDebugRestore(void)
{
  L7_RC_t rc;

  ptinHapiDebugBuildDefaultConfigData(PTINHAPI_DEBUG_CFG_VER_CURRENT);

  ptinHapiDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = ptinHapiApplyDebugConfigData();
  rc = L7_SUCCESS;
  return rc;
}

/*********************************************************************
* @purpose  Build default ptinHapi config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ptinHapiDebugBuildDefaultConfigData(L7_uint32 ver)
{

  /* setup file header */
  ptinHapiDebugCfg.hdr.version = ver;
  ptinHapiDebugCfg.hdr.componentID = L7_PTINHAPI_COMPONENT_ID;
  ptinHapiDebugCfg.hdr.type = L7_CFG_DATA;
  ptinHapiDebugCfg.hdr.length = (L7_uint32)sizeof(ptinHapiDebugCfg);
  strcpy((L7_char8 *)ptinHapiDebugCfg.hdr.filename, PTINHAPI_DEBUG_CFG_FILENAME);
  ptinHapiDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&ptinHapiDebugCfg.cfg, 0, sizeof(ptinHapiDebugCfg.cfg));
}

/*********************************************************************
* @purpose  Apply ptinHapi debug config data
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t ptinHapiApplyDebugConfigData(void)
{
  L7_RC_t rc;

//  rc = ptinHapiDebugPacketTraceFlagSet(ptinHapiDebugCfg.cfg.ptinHapiDebugPacketTraceTxFlag,ptinHapiDebugCfg.cfg.ptinHapiDebugPacketTraceRxFlag);
  rc = L7_SUCCESS;
  return rc;
}

int ptinHapiconfigDebugTrace (st_DebugParams *input, int profileID)
{
   st_DebugParams data;

   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_APP, TRACE_SEVERITY_INFORMATIONAL,  "Configuracao do modo de debug %04X, %d, %d, %08X, %08X, %s.",
         input->mask, input->debugOutput, input->debugFormat, input->debugLayers, input->debugModules, input->debugDevice);
   GetDebugSettings (&data);

   if (input->mask & 0x01)
      SetDebugOutput (input->debugOutput);
   if (input->mask & 0x02)
      SetDebugFormat (input->debugFormat);
   if (input->mask & 0x04)
      SetDebugFilters (input->debugLayers, input->debugModules);

   if (input->mask & 0x08)
      SetDebugDevice (input->debugDevice);
   return S_OK;
} // configDebug (V2.9.2.060222)
