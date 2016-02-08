/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   nim_cnfgr.c
*
* @purpose    The functions that interface to the CNFGR component
*
* @component  NIM
*
* @comments   none
*
* @create     05/07/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "nim_data.h"
#include "nim_cnfgr.h"
#include "nim_util.h"
#include "nim_events.h"
#include "nimapi.h"
#include "nim.h"
#include "sysapi.h"
#include "platform_config.h"
#include "nim_config.h"
#include "dtlapi.h"
#include "cardmgr_api.h"
#include <string.h>
#include "nim_debug.h"
#include "nim_sid.h"
#include "nim_ifindex.h"
#include "avl_api.h"
#include "nim_trace.h"
#if L7_FEAT_SF10GBT
#include "nim_port_fw.h"
#include "hpc_sf_fwupdate.h"
#endif

/* Prototypes for this module only */
L7_RC_t nimCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );

void    nimCnfgrFiniPhase1Process(void);

L7_RC_t nimCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );

void    nimCnfgrFiniPhase2Process(void);

L7_RC_t nimCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );

void    nimCnfgrFiniPhase3Process(void);

L7_RC_t nimCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason );

L7_RC_t nimCnfgrUnconfigureProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason );

void nimDataClear();


/*********************************************************************
*
* @purpose  CNFGR System Initialization for NIM component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the NIM.  This function is re-entrant.
*           The CNFGR request should be transfered to the NIM 
*           task as quickly as possible to avoid processing in the 
*           Configurator's task. 
*
* @end
*********************************************************************/
void nimApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  nimPdu_t nmpdu;
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             nimRC    = L7_ERROR;
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
        if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
            (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
        {
          if ((nimRC = nimCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
          {
            nimCnfgrFiniPhase1Process();
          }
          else
          {
            nimCtlBlk_g->nimPhaseStatus = L7_CNFGR_STATE_P1;
          }

          /* return value to caller - 
           * <prepare completion response>
           * <callback the configurator>
           */
          cbData.correlator       = correlator;
          cbData.asyncResponse.rc = nimRC;

          if (nimRC == L7_SUCCESS)
          {
            cbData.asyncResponse.u.response = response;

            /* only send the response we have success, error case handled later */
            cnfgrApiCallback(&cbData);
          }
          else
          {
            cbData.asyncResponse.u.reason   = reason;
          }

        } /* end Phase 1 processing */
        else
        {
          /* all other requests go to the nimTask to be processed */
          nmpdu.msgType = CNFGR_MSG;
          memcpy(&(nmpdu.data.pCmdData), pCmdData, sizeof (L7_CNFGR_CMD_DATA_t));

          if (nimCtlBlk_g->nimMsgQueue == L7_NULLPTR)
          {
            nimRC =  L7_ERROR;
            reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
          }
          else
          {
            nimRC = osapiMessageSend(nimCtlBlk_g->nimMsgQueue, &nmpdu, sizeof(nimPdu_t), 0, L7_MSG_PRIORITY_NORM  );
            nimRC = L7_SUCCESS;
          }

        }
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
  } /* pCmdData != L7_NULL */
  else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;

  } /* check for command valid pointer */

  /* return value to caller - 
   * <prepare completion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = nimRC;

  /* 
   * If it was successful, we either called it back already or have sent a msg 
   * to the nimTask for further processing
   */
  if (nimRC != L7_SUCCESS)
  {
    cbData.asyncResponse.u.reason   = reason;
    cnfgrApiCallback(&cbData);
  }

  return;
}

/*********************************************************************
*
* @purpose  Compare two USP AVL keys and determine Greater, Less, or Equal
*
* @param    a    a void pointer to an AVL key comprised of a USP structure
* @param    b    a void pointer to an AVL key comprised of a USP structure
* @param    len  the length of the key in bytes (unused)
*                            
* @returns  1 if a > b
* @returns  -1 if a < b
* @returns  0 if a == b
*
* @notes    This function is used as a custom comparator in the
*           nimUspTreeData AVL tree.  It is also used by the
*           custom comparator used by the custom comparator
*           used in the nimConfigIdTreeData AVL tree.  The 'len'
*           parameter is unused because the length of a USP is
*           known, however, it is required for compatibility with
*           the comparator type definition.
*
* @end
*********************************************************************/
static int nimUspCompare(const void *a, const void *b, size_t len)
{
  nimUSP_t *ua = (nimUSP_t *)a;
  nimUSP_t *ub = (nimUSP_t *)b;

  if (ua->unit < ub->unit)
  {
    return -1;
  }
  if (ua->unit > ub->unit)
  {
    return 1;
  }
  if (ua->slot < ub->slot)
  {
    return -1;
  }
  if (ua->slot > ub->slot)
  {
    return 1;
  }
  if (ua->port < ub->port)
  {
    return -1;
  }
  if (ua->port > ub->port)
  {
    return 1;
  }
  return 0;
}

/*********************************************************************
*
* @purpose  Compare two nimConfigId_t AVL keys and determine Greater,
*           Less, or Equal
*
* @param    a    a void pointer to an AVL key comprised of a nimConfigId_t
* @param    b    a void pointer to an AVL key comprised of a nimConfigId_t
* @param    len  the length of the key in bytes (unused)
*                            
* @returns  1 if a > b
* @returns  -1 if a < b
* @returns  0 if a == b
*
* @notes    This function is used as a custom comparator in the
*           in the nimConfigIdTreeData AVL tree.  The 'len'
*           parameter is unused because the length of a nimConfigId_t
*           is known, however, it is required for compatibility with
*           the comparator type definition.
*
* @end
*********************************************************************/
static int nimConfigIdCompare(const void *a, const void *b, size_t len)
{
  nimConfigID_t *id_a = (nimConfigID_t *)a;
  nimConfigID_t *id_b = (nimConfigID_t *)b;

  if (id_a->type < id_b->type)
  {
    return -1;
  }
  if (id_a->type > id_b->type)
  {
    return 1;
  }
  /*
   * The types are the same, so compare them based on type...
   */
  switch (id_a->type)
  {
    case L7_PHYSICAL_INTF:
    case L7_STACK_INTF:
    case L7_CPU_INTF:
      return nimUspCompare(&id_a->configSpecifier.usp,
                        &id_b->configSpecifier.usp,
                        sizeof(nimUSP_t));
    case L7_LAG_INTF:
      return avlCompareULong32(&id_a->configSpecifier.dot3adIntf,
                             &id_b->configSpecifier.dot3adIntf,
                             sizeof(L7_uint32));
    case L7_LOGICAL_VLAN_INTF:
      return avlCompareULong32(&id_a->configSpecifier.vlanId,
                             &id_b->configSpecifier.vlanId,
                             sizeof(L7_uint32));
    case L7_LOOPBACK_INTF:
      return avlCompareULong32(&id_a->configSpecifier.loopbackId,
                             &id_b->configSpecifier.loopbackId,
                             sizeof(L7_uint32));
    case L7_TUNNEL_INTF:
      return avlCompareULong32(&id_a->configSpecifier.tunnelId,
                             &id_b->configSpecifier.tunnelId,
                             sizeof(L7_uint32));

    case L7_WIRELESS_INTF:
      return avlCompareULong32(&id_a->configSpecifier.wirelessNetId,
                           &id_b->configSpecifier.wirelessNetId,
                           sizeof(L7_uint32));

    case L7_CAPWAP_TUNNEL_INTF:
      return avlCompareULong32(&id_a->configSpecifier.l2tunnelId,
                           &id_b->configSpecifier.l2tunnelId,
                           sizeof(L7_uint32));

    default:
      /*
       * A node with an invalid type is a critical error.
       */
      NIM_LOG_ERROR("Bad config id type");
      return 0;
  }
}

/*********************************************************************
* @purpose  phase 1 to Initialize Network Interface Manager component   
*
* @param    void
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t   nimPhaseOneInit()
{
  L7_RC_t rc = L7_SUCCESS;

  do
  {

    /* Not to be freed during operation */
    nimCtlBlk_g = osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(nimSystemData_t));

    if (nimCtlBlk_g == L7_NULL)
    {
      NIM_LOG_ERROR("NIM Control block not created\n");
      rc = L7_FAILURE;
      break;
    }
    else
    {
      /* all pointers and data with nimCtlBlk_g are to be set to 0 */
      memset((void *) nimCtlBlk_g, 0, sizeof (nimSystemData_t));
      nimLoggingSet(0);
    }

    if (nimStartTask() != L7_SUCCESS)
    {
      NIM_LOG_ERROR("NIM: Tasks and queues not created\n");
      rc = L7_FAILURE;
      break;
    }

    /* Allocate and initialize the necessary data elements of nimCtlBlk_g for 
       for phase 1 */
    nimCtlBlk_g->maxNumOfUnits                  = platUnitTotalMaxPerStackGet();
    nimCtlBlk_g->maxNumOfSlotsPerUnit           = platSlotTotalMaxPerUnitGet();
    nimCtlBlk_g->maxNumOfPhysicalPortsPerSlot   = platSlotMaxPhysicalPortsPerSlotGet();

    nimCtlBlk_g->nimNotifyList = osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(nimNotifyList_t) * L7_LAST_COMPONENT_ID);

    /* Not to be freed during operation */
    if (nimCtlBlk_g->nimNotifyList == L7_NULL)
    {
      NIM_LOG_ERROR("NIM registrant notification list not created\n");
      rc = L7_FAILURE;
      break;
    }
    else
    {
      memset(( void * )nimCtlBlk_g->nimNotifyList, 0, sizeof( nimNotifyList_t ) * L7_LAST_COMPONENT_ID ); 
    }

    /* Create NIM syncronization semaphore for the entire control block, should be used
       during all accesses to the control block in order to prevent contention.  NIM can
       be called at any time by multiple tasks */
    if (osapiRWLockCreate(&nimCtlBlk_g->rwLock,  OSAPI_RWLOCK_Q_PRIORITY) != L7_SUCCESS)
    {
      NIM_LOG_ERROR("NIM syncronization read write lock not created\n");
      rc = L7_FAILURE;
      break;
    }

    (void)nimIfIndexPhaseOneInit();

    /* Create the nimUSP AVL Tree */

    nimCtlBlk_g->nimUspTreeHeap = osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(avlTreeTables_t)*platIntfTotalMaxCountGet());
    if (nimCtlBlk_g->nimUspTreeHeap == L7_NULLPTR)
    {
      NIM_LOG_ERROR("NIM: Unable to allocate resources\n");
      break; /* goto while */
    }
    else
    {
      (void)memset(nimCtlBlk_g->nimUspTreeHeap ,0x00,sizeof(avlTreeTables_t)*platIntfTotalMaxCountGet());
    }


    nimCtlBlk_g->nimUspDataHeap = osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(nimUspIntIfNumTreeData_t)*platIntfTotalMaxCountGet());
    if (nimCtlBlk_g->nimUspDataHeap == L7_NULLPTR)
    {
      NIM_LOG_ERROR("NIM: Unable to allocate resources\n");
      break; /* goto while */
    }
    else
    {
      (void)memset(nimCtlBlk_g->nimUspDataHeap ,0x00,sizeof(nimUspIntIfNumTreeData_t)*platIntfTotalMaxCountGet());
    }

    avlCreateAvlTree(&nimCtlBlk_g->nimUspTreeData, nimCtlBlk_g->nimUspTreeHeap , nimCtlBlk_g->nimUspDataHeap ,
                     platIntfTotalMaxCountGet(), sizeof(nimUspIntIfNumTreeData_t), 0x10, sizeof(nimUSP_t));
    /* WPJ_TBD: Why is this using  nimIfIndexTreeData_t ? */
    (void)avlSetAvlTreeComparator(&nimCtlBlk_g->nimUspTreeData, nimUspCompare);

    /* Create the nimConfigId AVL Tree */

    nimCtlBlk_g->nimConfigIdTreeHeap = osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(avlTreeTables_t)*platIntfTotalMaxCountGet());
    if (nimCtlBlk_g->nimConfigIdTreeHeap == L7_NULLPTR)
    {
      NIM_LOG_ERROR("NIM: Unable to allocate resources for configId AVL tree\n");
      break; /* goto while */
    }
    else
    {
      (void)memset(nimCtlBlk_g->nimConfigIdTreeHeap ,0x00,sizeof(avlTreeTables_t)*platIntfTotalMaxCountGet());
    }


    nimCtlBlk_g->nimConfigIdDataHeap = osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(nimConfigIdTreeData_t)*platIntfTotalMaxCountGet());
    if (nimCtlBlk_g->nimConfigIdDataHeap == L7_NULLPTR)
    {
      NIM_LOG_ERROR("NIM: Unable to allocate resources\n");
      break; /* goto while */
    }
    else
    {
      (void)memset(nimCtlBlk_g->nimConfigIdDataHeap ,0x00,sizeof(nimConfigIdTreeData_t)*platIntfTotalMaxCountGet());
    }

    avlCreateAvlTree(&nimCtlBlk_g->nimConfigIdTreeData, nimCtlBlk_g->nimConfigIdTreeHeap , nimCtlBlk_g->nimConfigIdDataHeap ,
                     platIntfTotalMaxCountGet(), sizeof(nimConfigIdTreeData_t), 0x10, sizeof(nimConfigID_t));
    (void)avlSetAvlTreeComparator(&nimCtlBlk_g->nimConfigIdTreeData,
                                  nimConfigIdCompare);


    /* enable the trace utility for NIM */
    nimTraceInit(NIM_TRACE_ENTRY_MAX, NIM_TRACE_ENTRY_SIZE_MAX);
    nimProfileInit();

    /* Set the phase */
    nimCtlBlk_g->nimPhaseStatus = L7_CNFGR_STATE_P1;


  } while ( 0 );

  return(rc) ;
}

/*********************************************************************
* @purpose  Remove all resources acquired during Phase 1
*
* @param    void
*
* @returns  void
*
* @notes    nimPhaseTwoFini and nimPhaseThreeFini should be called before
*           this function is called.  If the order is not kept, then memory
*           Leaks may occur.
*       
* @end
*********************************************************************/
void nimPhaseOneFini()
{

  if (nimCtlBlk_g->nimPhaseStatus != L7_CNFGR_STATE_P1)
  {
    NIM_LOG_MSG("NIM: nimPhaseOneFini called in strange sequence\n");
  }

  if (nimCtlBlk_g->rwLock.handle != L7_NULLPTR)
  {
	(void)osapiRWLockDelete(nimCtlBlk_g->rwLock);
  }

  if (nimCtlBlk_g->nimNotifyList != L7_NULLPTR)
  {
    osapiFree(L7_NIM_COMPONENT_ID, nimCtlBlk_g->nimNotifyList);
    nimCtlBlk_g->nimNotifyList = L7_NULLPTR;
  }

  if (nimCtlBlk_g->taskId != L7_ERROR)
  {
    osapiTaskDelete(nimCtlBlk_g->taskId);
  }

  if (nimCtlBlk_g->nimMsgQueue != L7_NULLPTR)
  {
    osapiMsgQueueDelete(nimCtlBlk_g->nimMsgQueue);
    nimCtlBlk_g->nimMsgQueue = L7_NULLPTR;
  }

  /* 
   * need to deallocate the avl tree for USP lookup
   * requires a tree delete api function in AVL 
   */

  if (nimCtlBlk_g != L7_NULLPTR)
  {
    osapiFree(L7_NIM_COMPONENT_ID, nimCtlBlk_g);
    nimCtlBlk_g = L7_NULLPTR;
  }

  nimTraceFini();

  return;
}

L7_RC_t   nimCmgrNewIntfChangeCallback(L7_uint32 unit, L7_uint32 slot, L7_uint32 port,
                                       L7_uint32 cardType,L7_PORT_EVENTS_t event,
                                       SYSAPI_HPC_PORT_DESCRIPTOR_t *portData);

/*********************************************************************
* @purpose  phase 2 to Initialize Network Interface Manager component   
*
* @param    void
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t   nimPhaseTwoInit()
{

  L7_RC_t   rc = L7_SUCCESS;

  memset((void *) &nimCtlBlk_g->notifyFunctionList, 0, sizeof(nvStoreFunctionList_t));
  nimCtlBlk_g->notifyFunctionList.registrar_ID   = L7_NIM_COMPONENT_ID;
  nimCtlBlk_g->notifyFunctionList.notifySave     = nimSave;
  nimCtlBlk_g->notifyFunctionList.hasDataChanged = nimHasDataChanged;
  nimCtlBlk_g->notifyFunctionList.resetDataChanged = nimResetDataChanged;

#if L7_FEAT_SF10GBT
  /**************************************************************
  * Create message queue for HPC 
  ***************************************************************/
  nimFwRspQ = (void*)osapiMsgQueueCreate(NIM_HPC_FW_RSP_Q_NAME, NIM_HPC_FWMSG_COUNT, NIM_HPC_FWMSG_SIZE);
  
  if (nimFwRspQ == L7_NULLPTR)
    L7_LOGFNC(L7_LOG_SEVERITY_INFO, "Failed to Create NIM HPC Firmware Operations Queue.\n");

  /**************************************************************
  * Register HPC callback 
  ***************************************************************/
  rc = sysapiHpcReceiveCallbackRegister((void*)nimHPCReceiveCallback, L7_NIM_COMPONENT_ID);

#endif


  do
  {

    if ((rc = nimIntfDataAlloc()) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

    /* allocate/initialize memory to store the names of the config files 
       not freed during operation */
    nimCtlBlk_g->nimFileName = (L7_char8*)osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(L7_char8)*(L7_NIM_FILE_NAME_LENGTH+1));

    if (nimCtlBlk_g->nimFileName == L7_NULL)
    {
      NIM_LOG_ERROR("NIM:Couldn't create memory for the nimFileName\n");
      rc = L7_FAILURE;
      break;
    }
    else
    {
      memset((void*)nimCtlBlk_g->nimFileName, 0,sizeof(L7_char8)* (L7_NIM_FILE_NAME_LENGTH+1));
    }

    if (nimEventHdlrInit() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

    /* allocate the memory for the memory copy of the config file */
    if (nimConfigPhase2Init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

    /* Register NIMs NVRAM routines with the system */
    if (nvStoreRegister(nimCtlBlk_g->notifyFunctionList) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

    /* register callbacks to device driver */
    dtlRegister(nimDtlIntfChangeCallback);
    dtlGenericRegistration(DTL_FAMILY_ADDR_MGMT,DTL_EVENT_ADDR_INTF_MAC_QUERY,
        (void *)nimMacAddrQueryCallback);

    /* register callback to Card Manager */
    cmgrRegisterPortChange(L7_NIM_COMPONENT_ID,nimCmgrNewIntfChangeCallback);

    /* create the filenames of the config files */
    nimFileSetup();

#if L7_FEAT_SF10GBT
    /* Giving port some time to bring up after configuration */
    osapiSleep(1);
    hpcSF_ModuleFwUpdate();
#endif
    nimCtlBlk_g->nimPhaseStatus = L7_CNFGR_STATE_P2;

  } while (0);

  return(rc);
}

/*********************************************************************
* @purpose  Remove all resources acquired during Phase 2
*
* @param    void
*
* @returns  void
*
* @notes    nimPhaseThreeFini must be called before this is called.  
*           Failure to keep the sequence will result in memory leaks. 
*       
* @end
*********************************************************************/
void nimPhaseTwoFini()
{

  NIM_LOG_MSG("NIM: nimPhaseTwoFini called \n");

  memset((void *) &nimCtlBlk_g->notifyFunctionList, 0, sizeof(nvStoreFunctionList_t));

  if (nimCtlBlk_g->nimFileName != L7_NULLPTR)
  {
    osapiFree(L7_NIM_COMPONENT_ID, nimCtlBlk_g->nimFileName);
    nimCtlBlk_g->nimFileName = L7_NULLPTR;
  }

  if (nimCtlBlk_g->nimConfigData != L7_NULLPTR)
  {
    osapiFree(L7_NIM_COMPONENT_ID, nimCtlBlk_g->nimConfigData);
    nimCtlBlk_g->nimConfigData = L7_NULLPTR;
  }

  nimEventHdlrFini();

  nimIntfDataDeAlloc();

  nimCtlBlk_g->nimPhaseStatus = L7_CNFGR_STATE_P1;

  return;
}

/*********************************************************************
* @purpose  phase 3 to Initialize Network Interface Manager component   
*
* @param    void
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t   nimPhaseThreeInit()
{
  L7_RC_t           rc = L7_SUCCESS;  

  nimCtlBlk_g->ifNumber = 0;
  nimCtlBlk_g->ifTableLastChange = 0;
  nimCtlBlk_g->nimHighestIntfNumber = 0;

  /* read in the config file and set the datachanged flag appropriately */
  nimConfigInit();

  nimConfigIdTreePopulate();
  nimIntIfNumRangePopulate();

  nimCtlBlk_g->nimPhaseStatus = L7_CNFGR_STATE_P3;

  return(rc);
}

/*********************************************************************
* @purpose  Remove all resources acquired during Phase 3
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
void nimPhaseThreeFini()
{
  nimDataClear();

}

/*********************************************************************
* @purpose  Allocate all of the interface related data
*
* @param    none
*
* @returns  L7_RC_t  Returns L7_SUCCESS or L7_ERROR
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t nimIntfDataAlloc()
{
  L7_RC_t               rc = L7_SUCCESS;  

  do
  {
    /* Not freed during operation */
    nimCtlBlk_g->nimPorts = osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(nimIntf_t) * (platIntfTotalMaxCountGet() + 1));

    if (nimCtlBlk_g->nimPorts == L7_NULLPTR)
    {
      NIM_LOG_ERROR("NIM: unable to alloc memory for nimPorts\n");
      rc = L7_FAILURE;
      break;
    }
    else
    {
      /* reset the global data to known state */
      memset(( void * )nimCtlBlk_g->nimPorts, 
             0, 
             sizeof( nimIntf_t ) * (platIntfTotalMaxCountGet() + 1)); 
    }

    /* allocate/initialize the memory to store the number of ports per unit that have been created 
        Not freed during operation */
    nimCtlBlk_g->nimNumberOfPortsPerUnit = (L7_uint32*)osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(L7_uint32)*(nimCtlBlk_g->maxNumOfUnits+1));

    if (nimCtlBlk_g->nimNumberOfPortsPerUnit == L7_NULL)
    {
      NIM_LOG_ERROR("NIM:Couldn't allocate memory for nimNumberOfPortPerUnit\n");
      rc = L7_FAILURE;
      break;
    }
    else
    {
      memset((void*)nimCtlBlk_g->nimNumberOfPortsPerUnit, 0 , (sizeof(L7_uint32)*(nimCtlBlk_g->maxNumOfUnits+1)));
    }

    /* Not freed during operation */
    nimCtlBlk_g->nimVlanSlotsInfo = osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(nimUSP_t)*platIntfVlanIntfMaxCountGet());

    if (nimCtlBlk_g->nimVlanSlotsInfo == L7_NULLPTR)
    {
      NIM_LOG_ERROR("NIM: unable to alloc nimVlanSlotsInfo\n");
      rc = L7_FAILURE;
      break;
    }
    else
    {
      memset((void *)nimCtlBlk_g->nimVlanSlotsInfo, 0, sizeof(nimUSP_t)*platIntfVlanIntfMaxCountGet());
    }

    /* Not freed during operation */
    nimCtlBlk_g->numberOfInterfacesByType = osapiMalloc(L7_NIM_COMPONENT_ID, sizeof(L7_uint32)*L7_MAX_INTF_TYPE_VALUE);

    if (nimCtlBlk_g->numberOfInterfacesByType == L7_NULLPTR)
    {
      NIM_LOG_ERROR("NIM: unable to alloc numberOfInterfacesByType\n");
      rc = L7_FAILURE;
      break;
    }
    else
    {
      memset((void *)nimCtlBlk_g->numberOfInterfacesByType, 0, sizeof(L7_uint32)*L7_MAX_INTF_TYPE_VALUE);
    }

    nimCtlBlk_g->intfTypeData = osapiMalloc (L7_NIM_COMPONENT_ID, sizeof(nimIntfTypeData_t) * L7_MAX_INTF_TYPE_VALUE);
    if (nimCtlBlk_g->intfTypeData == L7_NULLPTR)
    {
      LOG_ERROR (0);
    }

  }while (0);

  return(rc);
}

/*********************************************************************
* @purpose  DeAllocate all of the interface related data
*
* @param    none
*
* @returns  L7_RC_t  Returns L7_SUCCESS or L7_ERROR
*
* @notes    none
*       
* @end
*********************************************************************/
void nimDataClear()
{
  if ((nimCtlBlk_g->nimPhaseStatus != L7_CNFGR_STATE_U1) && 
      (nimCtlBlk_g->nimPhaseStatus != L7_CNFGR_STATE_U2))
  {
    NIM_LOG_MSG("NIM: attempting to clear the data in an incorrect phase!");
  }
  else
  {
    NIM_CRIT_SEC_WRITE_ENTER();
    memset((void *)&nimCtlBlk_g->cpuIntfMask,0,sizeof(NIM_INTF_MASK_t));;
    memset((void *)&nimCtlBlk_g->createdMask,0,sizeof(NIM_INTF_MASK_t));;
    memset((void *)&nimCtlBlk_g->forwardStateMask,0,sizeof(NIM_INTF_MASK_t));;
    memset((void *)&nimCtlBlk_g->lagIntfMask,0,sizeof(NIM_INTF_MASK_t));;
    memset((void *)&nimCtlBlk_g->linkStateMask,0,sizeof(NIM_INTF_MASK_t));;
    memset((void *)&nimCtlBlk_g->physicalIntfMask,0,sizeof(NIM_INTF_MASK_t));;
    memset((void *)&nimCtlBlk_g->presentMask,0,sizeof(NIM_INTF_MASK_t));;
    memset((void *)&nimCtlBlk_g->vlanIntfMask,0,sizeof(NIM_INTF_MASK_t));;
    memset((void *)&nimCtlBlk_g->l2tnnlIntfMask,0,sizeof(NIM_INTF_MASK_t));;

    /* reset the global data to known state */
    memset(( void * )nimCtlBlk_g->nimPorts, 0, sizeof( nimIntf_t ) * (platIntfTotalMaxCountGet() + 1)); 
    memset((void*)nimCtlBlk_g->nimNumberOfPortsPerUnit, 0 , (sizeof(L7_uint32)*(nimCtlBlk_g->maxNumOfUnits+1)));
    memset((void *)nimCtlBlk_g->nimVlanSlotsInfo, 0, sizeof(nimUSP_t)*platIntfVlanIntfMaxCountGet());
    memset((void *)nimCtlBlk_g->numberOfInterfacesByType, 0, sizeof(L7_uint32)*L7_MAX_INTF_TYPE_VALUE);

    (void)nimEventHdlrDataClear();

    avlPurgeAvlTree(&nimCtlBlk_g->nimUspTreeData, platIntfTotalMaxCountGet());

    avlPurgeAvlTree(&nimCtlBlk_g->nimConfigIdTreeData, platIntfTotalMaxCountGet());

    nimIfIndexDataClear();

    NIM_CRIT_SEC_WRITE_EXIT();
  }

}

/*********************************************************************
* @purpose  DeAllocate all of the interface related data
*
* @param    none
*
* @returns  L7_RC_t  Returns L7_SUCCESS or L7_ERROR
*
* @notes    none
*       
* @end
*********************************************************************/
void nimIntfDataDeAlloc()
{
  if (nimCtlBlk_g->nimPorts != L7_NULLPTR)
  {
    osapiFree(L7_NIM_COMPONENT_ID, nimCtlBlk_g->nimPorts);
    nimCtlBlk_g->nimPorts = L7_NULL;
  }

  if (nimCtlBlk_g->nimNumberOfPortsPerUnit != L7_NULL)
  {
    osapiFree(L7_NIM_COMPONENT_ID, nimCtlBlk_g->nimNumberOfPortsPerUnit);
    nimCtlBlk_g->nimNumberOfPortsPerUnit = L7_NULLPTR;
  }

  if (nimCtlBlk_g->nimVlanSlotsInfo != L7_NULLPTR)
  {
    osapiFree(L7_NIM_COMPONENT_ID, nimCtlBlk_g->nimVlanSlotsInfo);
    nimCtlBlk_g->nimVlanSlotsInfo = L7_NULLPTR;
  }

  if (nimCtlBlk_g->numberOfInterfacesByType == L7_NULLPTR)
  {
    osapiFree(L7_NIM_COMPONENT_ID, nimCtlBlk_g->numberOfInterfacesByType);
    nimCtlBlk_g->numberOfInterfacesByType = L7_NULLPTR;
  }

}

/*********************************************************************
* @purpose  Initialize and start Card Manager Task function.
*
* @param    none
*
* @returns  L7_RC_t  Returns L7_SUCCESS or L7_ERROR
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t nimStartTask()
{

  L7_RC_t rc = L7_SUCCESS;

  do
  {

    nimCtlBlk_g->nimMsgQueue = (void *)osapiMsgQueueCreate("NIM-Q",nimSidMsgCountGet(),
                                                           (L7_uint32)sizeof(nimPdu_t));

    if (nimCtlBlk_g->nimMsgQueue == L7_NULLPTR)
    {
      NIM_LOG_ERROR("NIM queue not created\n");
      rc = L7_FAILURE;
      break;
    }

    nimCtlBlk_g->taskId = osapiTaskCreate("nim_t",nimTask,0, 0, 
                                          nimSidTaskStackSizeGet(), 
                                          nimSidTaskPriorityGet(), 
                                          nimSidTaskSliceGet());    

    if (nimCtlBlk_g->taskId == L7_ERROR)
    {
      NIM_LOG_ERROR("NIM task not created.\n");
      rc = L7_FAILURE;
      break;
    }

    if (osapiWaitForTaskInit (L7_NIM_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      NIM_LOG_ERROR("NIM: Could not sync the TASK\n");
      rc = L7_FAILURE;
      break;
    }

  } while ( 0 );
  return(rc);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair.
*
* @param    pCmdData  - @b{(input)}  command to be processed.       
*
* @returns  None
*
* @notes    This function completes synchronously. The return value 
*           is presented to the configurator by calling the cnfgrApiCallback().
*           The following are the possible return codes:   
*           L7_SUCCESS - There were no errors. Response is available.
*           L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_CB_ERR_RC_INVALID_RQST
*           L7_CNFGR_CB_ERR_RC_INVALID_CMD
*           L7_CNFGR_ERR_RC_INVALID_CMD_TYPE
*           L7_CNFGR_ERR_RC_INVALID_PAIR
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
void nimRecvCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             nimRC    = L7_ERROR;
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
                if ((nimRC = nimCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  nimCnfgrFiniPhase1Process();
                }
                else
                {
                  nimCtlBlk_g->nimPhaseStatus = L7_CNFGR_STATE_P1;
                }

                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((nimRC = nimCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  nimCnfgrFiniPhase2Process();
                }
                else
                {
                  nimCtlBlk_g->nimPhaseStatus = L7_CNFGR_STATE_P2;
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:  /* no configuration data to be read */
                if ((nimRC = nimCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  nimCnfgrFiniPhase3Process();
                }
                else
                {
                  nimCtlBlk_g->nimPhaseStatus = L7_CNFGR_STATE_P3;
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                if ((nimRC = nimCnfgrNoopProccess( &response, &reason)) == L7_SUCCESS)
                  nimCtlBlk_g->nimPhaseStatus = L7_CNFGR_STATE_WMU;
                break;

              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            if ((nimRC = nimCnfgrNoopProccess( &response, &reason)) == L7_SUCCESS)
              nimCtlBlk_g->nimPhaseStatus = L7_CNFGR_STATE_E;
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:

            switch (request)
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                if ((nimRC = nimCnfgrNoopProccess( &response, &reason)) == L7_SUCCESS)
                  nimCtlBlk_g->nimPhaseStatus = L7_CNFGR_STATE_U1;
                break;
              case L7_CNFGR_RQST_U_PHASE2_START:
                if ((nimRC = nimCnfgrUnconfigureProcess( &response, &reason)) == L7_SUCCESS)
                  nimCtlBlk_g->nimPhaseStatus = L7_CNFGR_STATE_U2;
                break;
              default:
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            nimRC = nimCnfgrNoopProccess( &response, &reason);
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
   * <prepare completion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = nimRC;

  if (nimRC == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
  }
  else
  {
    cbData.asyncResponse.u.reason   = reason;
  }

  cnfgrApiCallback(&cbData);

  return;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
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
L7_RC_t nimCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t nimRC = L7_SUCCESS;

  nimRC = nimPhaseOneInit();

  if (nimRC != L7_SUCCESS)
  {
    nimRC = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(nimRC);

}

/*********************************************************************
* @purpose  Release all resources collected during phase 1
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void nimCnfgrFiniPhase1Process(void)
{
  nimPhaseOneFini();

  return;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
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
L7_RC_t nimCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t nimRC = L7_SUCCESS;

  nimRC = nimPhaseTwoInit();

  if (nimRC != L7_SUCCESS)
  {
    nimRC = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(nimRC);
}

/*********************************************************************
* @purpose  Release all resources collected during phase 2
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void nimCnfgrFiniPhase2Process(void)
{
  nimPhaseTwoFini();

  return;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
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
L7_RC_t nimCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t nimRC = L7_SUCCESS;

  nimRC = nimPhaseThreeInit();

  if (nimRC != L7_SUCCESS)
  {
    nimRC = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(nimRC);
}

/*********************************************************************
* @purpose  Release all resources collected during phase 3
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void nimCnfgrFiniPhase3Process(void)
{
  nimPhaseThreeFini();

  return;
}
/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
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
L7_RC_t nimCnfgrUnconfigureProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t nimRC = L7_SUCCESS;

  nimPhaseThreeFini();

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  return(nimRC);
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
L7_RC_t nimCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t nimRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(nimRC);
}
