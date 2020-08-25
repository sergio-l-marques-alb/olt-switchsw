/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename    dot1s_nsf.c
* @purpose     802.1s Multiple Spanning Tree, Non stop forwarding support
* @component   dot1s
* @comments    none
* @create      3/2/09
* @author      akulkarn
* @end
*
**********************************************************************/

#include <stdio.h>
#include <string.h>
#include "l7_common.h"
#include "default_cnfgr.h"
#include "comm_mask.h"
#include "ckpt_api.h"
#include "dot1s_nsf.h"
#include "dot1s_db.h"
#include "dot1s_debug.h"
#include "log.h"

extern L7_uint32 dot1sPortIndexFindFirst();
extern L7_uint32 dot1sPortIndexFindNext(L7_uint32 prevIndex);
extern L7_uint32 dot1sPortIndexFromIntfNumGet(L7_uint32 intIfNum);
extern L7_uint32 dot1sIntIfNumFromPortIndexGet(L7_uint32 portIndex);
void dot1sNsfDebugPktDump(L7_uchar8 *pkt, L7_uint32 length);

dot1s_nsf_port_checkpoint_data_t *dot1sPortCkpt = L7_NULLPTR;
void *dot1sCkptMaskSema = L7_NULLPTR;
L7_BOOL dot1sBackupMgrElected = L7_FALSE;
dot1s_inst_ckpt_states_t instState[L7_MAX_MULTIPLE_STP_INSTANCES+1];
L7_INTF_MASK_t portCommonCkptBitMask;
L7_INTF_MASK_t *portInstCkptBitMask;
extern dot1s_intf_recon_data_t *port_recon_data;

dot1s_nsf_port_checkpoint_data_t *dot1sIntfCkptDataFind(L7_uint32 intIfNum)
{
  L7_uint32 portIndex = dot1sPortIndexFromIntfNumGet(intIfNum);

  if (portIndex != L7_NULL )
  {
    return &dot1sPortCkpt[portIndex];
  }

  return L7_NULLPTR;
}
dot1s_nsf_port_checkpoint_data_t *dot1sCkptDataFindFirst()
{
  L7_uint32 index =  dot1sPortIndexFindFirst();

  if (index != L7_NULL)
  {
    return &dot1sPortCkpt[index];
  }

  return L7_NULLPTR;
}

dot1s_nsf_port_checkpoint_data_t *dot1sCkptDataFindNext(L7_uint32 prevIndex)
{
  L7_uint32 index =  dot1sPortIndexFindNext(prevIndex);

  if (index != L7_NULL)
  {
    return &dot1sPortCkpt[index];
  }

  return L7_NULLPTR;
}
/*********************************************************************
* @purpose  Reset the check point data to prepare for further checkpoints
*
* @param
*
* @returns  L7_SUCCESS
*
* @notes  called during init and after the switchover is complete
*
* @end
*********************************************************************/
L7_RC_t dot1sCheckPtDataReset()
{
  dot1s_nsf_port_checkpoint_data_t *ckptPort;
  DOT1S_PORT_COMMON_t *pPort;

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,"%s: ", __FUNCTION__);
  pPort = dot1sIntfFindFirst();
  while (pPort)
  {
    ckptPort = dot1sIntfCkptDataFind(pPort->portNum);
    /* clear the ckpt data*/
    memset(ckptPort, 0, sizeof(*ckptPort));

    pPort = dot1sIntfFindNext(pPort->portNum);
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose Initialize the Non stop forwarding sub system for dot1s
*
* @param    None
*
* @returns  L7_SUCCESS   on success
*           L7_NO_MEM    if memory not sufficient
*           L7_FAILURE   otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1sNsfInit()
{
  L7_uint32 ckptDatasize, ckptBitmaskSize, reconDataSize;

  ckptDatasize = sizeof(dot1s_nsf_port_checkpoint_data_t)*(L7_DOT1S_MAX_INTERFACE_COUNT + 1);
  ckptBitmaskSize = sizeof(L7_INTF_MASK_t)*(L7_MAX_MULTIPLE_STP_INSTANCES+1);
  reconDataSize = sizeof(dot1s_intf_recon_data_t)*(L7_DOT1S_MAX_INTERFACE_COUNT + 1);

  dot1sPortCkpt =  osapiMalloc(L7_DOT1S_COMPONENT_ID, ckptDatasize);

  if (dot1sPortCkpt == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
            "Cannot allocate memory for dot1s NSF" );
    return L7_NO_MEMORY;
  }

  dot1sCkptMaskSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  if (dot1sCkptMaskSema == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
            "Unable to create dot1s checkpoint semaphore");
    return(L7_FAILURE);
  }

  portInstCkptBitMask = osapiMalloc(L7_DOT1S_COMPONENT_ID,ckptBitmaskSize);
  if (portInstCkptBitMask == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
            "Cannot allocate memory for dot1s NSF" );
    return L7_NO_MEMORY;
  }

  port_recon_data = osapiMalloc(L7_DOT1S_COMPONENT_ID,reconDataSize);

  memset(dot1sPortCkpt, 0, ckptDatasize);
  memset(portInstCkptBitMask, 0, ckptBitmaskSize);
  memset(port_recon_data, 0, reconDataSize);

  dot1sNsfFuncTable.dot1sIsIntfReconciled = dot1sIsIntfReconciled;
  dot1sNsfFuncTable.dot1sUpdateIntfReconciled = dot1sUpdateIntfReconciled;
  dot1sNsfFuncTable.dot1sNsfReconEventFunc = dot1sNsfReconEventFunc;
  dot1sNsfFuncTable.dot1sCallCheckpointService = dot1sCallCheckpointService;
  dot1sNsfFuncTable.dot1sCheckGlobalReconciled = dot1sCheckGlobalReconciled;
  dot1sNsfFuncTable.dot1sIntfReconcile = dot1sIntfReconcile;
  dot1sNsfFuncTable.dot1sNsfActivateStartupBegin = dot1sNsfActivateStartupBegin;
  dot1sNsfFuncTable.dot1sReconcileTimerExpiredProcess = dot1sReconcileTimerExpiredProcess;
  dot1sNsfFuncTable.dot1sCkptStateGet = dot1sCkptStateGet;

  dot1sNsfTraceInit(L7_NULL, L7_NULL);

  dot1sDebugNsf(DOT1S_DEBUG_NSF_RECONCILE |
                DOT1S_DEBUG_NSF_RECONCILE_EVENTS |
                DOT1S_DEBUG_NSF_RECONCILE_STATUS);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose Return resources allocated by dot1s NSF subsytem
*
* @param    None
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void dot1sNsfFini()
{
  dot1sNsfTraceFini();

  if (dot1sPortCkpt != L7_NULLPTR)
  {
    osapiFree(L7_DOT1S_COMPONENT_ID, dot1sPortCkpt);
    dot1sPortCkpt = L7_NULLPTR;
  }

  if (portInstCkptBitMask != L7_NULLPTR)
  {
    osapiFree(L7_DOT1S_COMPONENT_ID, portInstCkptBitMask);
    portInstCkptBitMask = L7_NULLPTR;
  }

  if (port_recon_data != L7_NULLPTR)
  {
    osapiFree(L7_DOT1S_COMPONENT_ID, port_recon_data);
    port_recon_data = L7_NULLPTR;
  }

  if (dot1sCkptMaskSema != L7_NULLPTR)
  {
    osapiSemaDelete(dot1sCkptMaskSema);
    dot1sCkptMaskSema = L7_NULLPTR;
  }

}

/*********************************************************************
* @purpose Initialize the Non stop forwarding sub system for dot1s
*
* @param    None
*
* @returns  L7_SUCCESS   on success
*           L7_NO_MEM    if memory not sufficient
*           L7_FAILURE   otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1sGetPortInstData(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex,
                             dot1s_nsf_port_checkpoint_data_t *ckptPort)
{

  ckptPort->instInfo[instIndex].disputed = p->portInstInfo[instIndex].disputed;
  ckptPort->instInfo[instIndex].portRole = p->portInstInfo[instIndex].role;
  ckptPort->instInfo[instIndex].portState = p->portInstInfo[instIndex].portState;

  return L7_SUCCESS;

}
/*********************************************************************
* @purpose Initialize the Non stop forwarding sub system for dot1s
*
* @param    None
*
* @returns  L7_SUCCESS   on success
*           L7_NO_MEM    if memory not sufficient
*           L7_FAILURE   otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1sGetPortCommonData(DOT1S_PORT_COMMON_t *p,
                               dot1s_nsf_port_checkpoint_data_t *ckptPort)
{

  ckptPort->common.operEdge = p->operEdge;
  ckptPort->common.rcvdBpdu = p->rcvdBpdu;
  ckptPort->common.rcvdRSTP = p->rcvdRSTP;
  ckptPort->common.rcvdSTP = p->rcvdSTP;
  ckptPort->common.rcvdMSTP = p->rcvdMSTP;
  ckptPort->common.sendRSTP = p->sendRSTP;
  ckptPort->common.bpduGuardEffect = p->bpduGuardEffect;
  ckptPort->common.loopInconsistent = p->loopInconsistent;
  ckptPort->common.diagnosticDisable = p->diagnosticDisable;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Prepare the data to be checkpointed
*
* @param
*
* @returns  L7_SUCCESS
*
* @notes   This function is called when new backup mgr notification is
*          received. This function gets the latest data for all the
*          ports from the application and sets all ports' bitmask.
*
* @end
*********************************************************************/
void dot1sNsfCheckpointPrepare()
{
  L7_uint32 instIndex;
  dot1s_nsf_port_checkpoint_data_t *ckptPort;
  DOT1S_PORT_COMMON_t *pPort;

  pPort = dot1sIntfFindFirst();
  while (pPort)
  {
    ckptPort = dot1sIntfCkptDataFind(pPort->portNum);
    /* copy the app data to ckpt data*/
    dot1sGetPortCommonData(pPort,ckptPort);
    L7_INTF_SETMASKBIT(portCommonCkptBitMask,pPort->portNum);

    for (instIndex = 0; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES ; instIndex++)
    {
      if (dot1sInstanceMap[instIndex].inUse == L7_TRUE)
      {
        dot1sGetPortInstData(pPort, instIndex, ckptPort);
        L7_INTF_SETMASKBIT(portInstCkptBitMask[instIndex], pPort->portNum);
        instState[instIndex] = DOT1S_INST_CKPT_BEGIN;

      }

    }

    /* Set the bit for common port*/
    pPort = dot1sIntfFindNext(pPort->portNum);
  }

}

/*********************************************************************
* @purpose Registers callbacks with the check point service
*
* @param    None
*
* @returns  L7_SUCCESS   on success
*           L7_FAILURE   otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1sNsfCallbacksRegister()
{
  L7_RC_t rc;
  if ((rc =ckptMgrCheckpointCallbackRegister(L7_DOT1S_COMPONENT_ID,
                                      dot1sManagerCheckpoint)) != L7_SUCCESS )
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT1S_COMPONENT_ID,
           "Failed to register to check point service");
  }
  else if ((rc =ckptBackupMgrCheckpointCallbackRegister(L7_DOT1S_COMPONENT_ID,
                                   dot1sBackupUnitCheckpoint)) != L7_SUCCESS )
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT1S_COMPONENT_ID,
           "Failed to register with check point service");
  }

  return rc;
}


/*********************************************************************
* @purpose  Build the message for each instance.
*
* @param    currBuff  @b{(input)}   buffer in which the message is being
*                                   populated
* @param    instIndex @b{(inout)}   Instance index
* @param    origBuf  @b{(intput)}   The original message ptr rx from the ckpt
*                                   service
* @param    max_data_len @b{(input)}  The max len which is allowed for this
*                                      msg
*
* @returns  L7_BOOL more_data if the data is available for this inst.
*
* @notes    caller is responsible for allocating checkpoint_data
*
* @end
*********************************************************************/
L7_BOOL dot1sBuildInstCkptMsg(L7_uchar8 **currBuff, L7_uint32 instIndex,
                              L7_uchar8 **origBuf,
                              L7_uint32 max_data_len )
{
  L7_BOOL more_data = L7_FALSE;
  L7_uint32 index, tempCurrLen, recordSize,tlvLen;
  L7_uchar8 *dot1s_ckpt_buff, *checkpoint_data;
  L7_INTF_MASK_t tempMask;
  L7_BOOL instAdded = L7_FALSE;
  dot1s_checkpoint_tlvs_t tlvType;

  dot1s_nsf_tlv_header_t *tlvHeader;
  dot1s_nsf_port_checkpoint_data_t *ckptPort;
  dot1s_nsf_port_inst_ckpt_t *ckptInstPort;

  dot1s_ckpt_buff = *currBuff;
  checkpoint_data = *origBuf;


  memcpy(&tempMask, &portInstCkptBitMask[instIndex], sizeof (tempMask));
  L7_INTF_FHMASKBIT(tempMask, index);
  while (index != 0)
  {
    instAdded = L7_TRUE;

    if (instState[instIndex] == DOT1S_INST_CKPT_BEGIN)
    {
      tlvLen = 0;
      recordSize = sizeof(dot1s_nsf_tlv_header_t) + tlvLen;
      tempCurrLen = dot1s_ckpt_buff - checkpoint_data;

      if ((tempCurrLen + recordSize)  < max_data_len )
      {
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                         "Adding DOT1S_TLV_PORT_INST_CKPT_BEGIN inst %d",
                          instIndex);
        instState[instIndex] = DOT1S_INST_CKPT_INPROGRESS;
        tlvHeader = (dot1s_nsf_tlv_header_t *)dot1s_ckpt_buff;
        tlvHeader->tlvType = osapiHtons(DOT1S_TLV_PORT_INST_CKPT_BEGIN);
        tlvHeader->tlvLen = osapiHtons(recordSize);
        tlvHeader->index = osapiHtonl(instIndex);

        dot1s_ckpt_buff += recordSize;
      }
      else
      {
        /* Message is full break out of the for*/
        more_data = L7_TRUE;
        break;
      }

    }
    /* This port has data that has been marked to for check pointing */

    tlvHeader = (dot1s_nsf_tlv_header_t *)dot1s_ckpt_buff;
    tlvLen = sizeof(dot1s_nsf_port_inst_ckpt_t) ;
    recordSize = tlvLen + sizeof(*tlvHeader);
    tlvType = DOT1S_TLV_PORT_INST_INFO;

    tempCurrLen = dot1s_ckpt_buff - checkpoint_data;
    if ((tempCurrLen + recordSize)  < max_data_len )
    {

      ckptPort = dot1sIntfCkptDataFind(index);
      if (ckptPort == L7_NULLPTR)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                "Error: Cannot find check pointed data for port %d ", index);
        L7_INTF_CLRMASKBIT(tempMask,index);
        L7_INTF_FHMASKBIT(tempMask, index);
        continue;

      }

      DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                      "Adding Port(%d) inst(%d) TLV ", index, instIndex);
      /*  object opcode*/
      tlvHeader->tlvType = osapiHtons(tlvType);
      tlvHeader->tlvLen = osapiHtons(recordSize);
      tlvHeader->index = osapiHtonl(index);

      dot1s_ckpt_buff += sizeof(*tlvHeader);
      ckptInstPort = (dot1s_nsf_port_inst_ckpt_t *)dot1s_ckpt_buff;

      /* No need to use osapiHtoN on these as these are all byte fields*/
      ckptInstPort->disputed = ckptPort->instInfo[instIndex].disputed;
      ckptInstPort->portRole = ckptPort->instInfo[instIndex].portRole;
      ckptInstPort->portState = ckptPort->instInfo[instIndex].portState;

      dot1s_ckpt_buff += tlvLen;
      L7_INTF_CLRMASKBIT(portInstCkptBitMask[instIndex], index);

    }
    else
    {
      /* Message is full. */
      more_data = L7_TRUE;
      break;
    }


    L7_INTF_CLRMASKBIT(tempMask,index);
    L7_INTF_FHMASKBIT(tempMask, index);
  }

  /* Do not add inst end tlv if we broke out of the above loop
     because the msg could not fit
  */
  if ((instAdded == L7_TRUE) &&
      (more_data == L7_FALSE))
  {
    if (instState[instIndex] == DOT1S_INST_CKPT_INPROGRESS)
    {
      tlvLen = 0;
      recordSize = sizeof(dot1s_nsf_tlv_header_t) + tlvLen;
      tempCurrLen = dot1s_ckpt_buff - checkpoint_data;
      if ((tempCurrLen + recordSize)  < max_data_len )
      {
        DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                         "Adding DOT1S_TLV_PORT_INST_CKPT_END for inst(%d)",
                          instIndex);
        instAdded = L7_FALSE;
        instState[instIndex] = DOT1S_INST_CKPT_READY;
        tlvHeader = (dot1s_nsf_tlv_header_t *)dot1s_ckpt_buff;
        tlvHeader->tlvType = osapiHtons(DOT1S_TLV_PORT_INST_CKPT_END);
        tlvHeader->tlvLen = osapiHtons(recordSize);
        tlvHeader->index = osapiHtonl(instIndex);

        dot1s_ckpt_buff += recordSize;
      }
      else
      {
        /* Message is full break out of the for*/
        more_data = L7_TRUE;
      }

    }
  }

  *currBuff = dot1s_ckpt_buff;

  return more_data;
}
/*********************************************************************
* @purpose  Build the message to transmit to the backup side
*
* @param    checkpoint_data  @b{(input)}   buffer in which the message is to be
*                                          populated
* @param    max_data_len    @b{(inout)}   Max len allocated for the data
* @param    ret_data_len  @b{(output)}  Actual length copied
* @param    more_data  @b{(output)}  Boolean to indicate if there is data
*                                    available to checkpoint
*
* @returns  L7_SUCCESS
*
* @notes    caller is responsible for allocating checkpoint_data
*
* @end
*********************************************************************/
L7_RC_t dot1sBuildCheckpointMessage(L7_uchar8   *checkpoint_data,
                                    L7_uint32               max_data_len,
                                    L7_uint32              *ret_data_len,
                                    L7_BOOL                *more_data)
{

  L7_uint32 index;
  L7_ushort16 tempCurrLen, tlvLen, recordSize;
  L7_uchar8 *dot1s_ckpt_buff; /* temp data pointer*/
  dot1s_checkpoint_tlvs_t tlvType;
  L7_INTF_MASK_t tempMask;
  dot1s_nsf_tlv_header_t *tlvHeader;
  dot1s_nsf_port_checkpoint_data_t *ckptPort;
  L7_uint32 instIndex;
  dot1s_ckpt_header_t *msgHeader;
  static L7_uint32 incompleteInst = L7_DOT1S_MSTID_ALL;

  if (checkpoint_data == L7_NULLPTR)
  {
    /* log message */
    return L7_FAILURE;
  }

  *more_data = L7_FALSE; /* Initialize */
  *ret_data_len = 0;
  dot1s_ckpt_buff = checkpoint_data;

  msgHeader = (dot1s_ckpt_header_t *)dot1s_ckpt_buff;
  msgHeader->version = osapiHtons(DOT1S_CHECKPOINT_MESSAGE_VER);
  msgHeader->messageLen = 0;

  dot1s_ckpt_buff += sizeof(dot1s_ckpt_header_t);


  osapiSemaTake(dot1sCkptMaskSema, L7_WAIT_FOREVER);

  /* Now go through the physical ports and build any check point data*/
  memcpy(&tempMask, &portCommonCkptBitMask, sizeof (tempMask));
  L7_INTF_FHMASKBIT(tempMask, index);
  while (index != 0)
  {
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                    "%s: Adding Port common TLV for port %d ",
                    __FUNCTION__, index);
    /* This port has data that has been marked to for check pointing */
    tlvHeader = (dot1s_nsf_tlv_header_t *)dot1s_ckpt_buff;
    tlvLen = DOT1S_PORT_COMMON_CKPT_SIZE;
    recordSize = tlvLen + sizeof(*tlvHeader);
    tlvType = DOT1S_TLV_PORT_COMMON;

    /* If tracking is on, log it */
    tempCurrLen = dot1s_ckpt_buff - checkpoint_data;
    if ((tempCurrLen + recordSize)  < max_data_len )
    {
      ckptPort = dot1sIntfCkptDataFind(index);
      if (ckptPort == L7_NULLPTR)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(index, L7_SYSNAME, ifName);

        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                "Error: Cannot find check pointed data for port %s ", ifName);
        osapiSemaGive(dot1sCkptMaskSema);
        return L7_FAILURE;
      }

      /*  object opcode*/
      tlvHeader->tlvType = osapiHtons(tlvType);
      tlvHeader->tlvLen = osapiHtons(recordSize);
      tlvHeader->index = osapiHtonl(index);

      dot1s_ckpt_buff +=  sizeof(*tlvHeader);
      /* Assuming here that the common peice is less than 2 bytes.
         This code needs to be changed if that assumption is no longer true
      */
      /* The dot1sPortCommon TLV is used as a flag so no need to convert
         it to network order
      */
      memcpy(dot1s_ckpt_buff, &ckptPort->common, tlvLen);
     /* *(L7_ushort16 *)dot1s_ckpt_buff = osapiHtons(tempPortCommonData);*/
      dot1s_ckpt_buff += tlvLen;

      /* Clear the bit */
      L7_INTF_CLRMASKBIT(portCommonCkptBitMask, index);

    }
    else
    {
      /* Message is full break out of the for*/
      *more_data = L7_TRUE;
      break;
    }

    L7_INTF_CLRMASKBIT(tempMask,index);
    L7_INTF_FHMASKBIT(tempMask, index);

  }

  /* If we had to break a message before and have unfinished inst message
     take care of that first.
  */
  if (incompleteInst != L7_DOT1S_MSTID_ALL)
  {
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                    "%s: Finishing incomplete inst %d first ",
                    __FUNCTION__, incompleteInst);
    *more_data = dot1sBuildInstCkptMsg(&dot1s_ckpt_buff, incompleteInst,
                           &checkpoint_data, max_data_len);
  }

  if (*more_data == L7_FALSE)
  {
    incompleteInst = L7_DOT1S_MSTID_ALL;
    for (instIndex = 0; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES ; instIndex++)
    {
       if (instIndex == incompleteInst)
       {
           continue;
       }
       *more_data = dot1sBuildInstCkptMsg(&dot1s_ckpt_buff, instIndex,
                             &checkpoint_data, max_data_len);
       if (*more_data == L7_TRUE)
       {
         incompleteInst = instIndex;
         break;
       }
    }
  }

  osapiSemaGive(dot1sCkptMaskSema);

  tempCurrLen = dot1s_ckpt_buff - checkpoint_data;
  if (tempCurrLen > sizeof(dot1s_ckpt_header_t))
  {
    *ret_data_len = tempCurrLen;
    msgHeader->messageLen = osapiHtons(tempCurrLen);
    /*dot1sNsfDebugPktDump(checkpoint_data, tempCurrLen);*/
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,"Ckpting msg of len %d ",
                    tempCurrLen);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Callback from the checkpoint service
*
* @param    callback_type  @b{(input)}   callback type CKPT_NEW_BACKUP_MANAGER,
*                                 CKPT_NEXT_ELEMENT or CKPT_BACKUP_MANAGER_GONE
* @param    checkpoint_data  @b{(input)}   buffer in which the message is to be
*                                           populated
* @param    max_data_len    @b{(inout)}   Max len allocated for the data
* @param    ret_data_len  @b{(output)}  Actual length copied
* @param    more_data  @b{(output)}  Boolean to indicate if there is data
*                                    vailable to checkpoint
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
void dot1sManagerCheckpoint(L7_CKPT_MGR_CALLBACK_t  callback_type,
                            L7_uchar8              *checkpoint_data,
                            L7_uint32               max_data_len,
                            L7_uint32              *ret_data_len,
                            L7_BOOL                *more_data)
{

  L7_RC_t rc = L7_SUCCESS;

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,"%s: Callback type %d ", __FUNCTION__,
                  callback_type);


  switch (callback_type)
  {
    case CKPT_NEW_BACKUP_MANAGER:
      osapiSemaTake(dot1sCkptMaskSema, L7_WAIT_FOREVER);
      dot1sBackupMgrElected = L7_TRUE;
      if (dot1sCnfgrAppIsReady() != L7_TRUE)
      {
        osapiSemaGive(dot1sCkptMaskSema);
        return; 
      }

      dot1sNsfCheckpointPrepare();
      osapiSemaGive(dot1sCkptMaskSema);
      rc = ckptMgrCheckpointStart(L7_DOT1S_COMPONENT_ID);
      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                "Call to checkpoint service to start failed(%d)",rc);
      }
      break;
    case CKPT_NEXT_ELEMENT:
      /* build the message*/
      dot1sBuildCheckpointMessage(checkpoint_data, max_data_len,
                                  ret_data_len, more_data);

      break;
    case CKPT_BACKUP_MANAGER_GONE:
      osapiSemaTake(dot1sCkptMaskSema, L7_WAIT_FOREVER);
      dot1sBackupMgrElected = L7_FALSE;

      osapiSemaGive(dot1sCkptMaskSema);
      break;
    default:
      break;
  }


  return;
}

/*********************************************************************
* @purpose  Populate the check point datat structures on the backup unit with
*           checkpointed data.
*
* @param    checkpoint_data  @b{(input)}  checkpointed data
* @param    data_len    @b{(inout)}   size of the data
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void dot1sPopulateCheckpointData(L7_uchar8  *checkpoint_data,
                                 L7_uint32  data_len)
{

  L7_uchar8 *currPtr, *tlvData;
  L7_int32 currLen = 0;
  /* store inst index value for port inst tlv.
     Need to make this variable static as the message could be
     broken into multiple messages
  */
  static L7_uint32 instIndex = L7_DOT1S_MSTID_ALL;
  dot1s_nsf_tlv_header_t *currTlvHeader, tlvHeader;
  dot1s_nsf_port_checkpoint_data_t *ckptPort;
  dot1s_nsf_port_inst_ckpt_t *rxInstPortData;
  L7_uint32 tempCurrLen;
  L7_uint64 populateStartTime, populateEndTime;
  dot1s_ckpt_header_t msgHeader, *rxHeader;

  if (data_len == 0)
  {
    return;
  }

  populateStartTime = osapiTimeMillisecondsGet64();

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,"Received Msg of len %d", data_len);
  currPtr = checkpoint_data;
  if (currPtr == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "Data not present");
    return;
  }
  rxHeader = (dot1s_ckpt_header_t *)currPtr;

  msgHeader.version = osapiNtohs(rxHeader->version) ;
  msgHeader.messageLen = osapiNtohs(rxHeader->messageLen) ;


  currLen = msgHeader.messageLen - sizeof(msgHeader);

  currPtr += sizeof(msgHeader);


  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                  "%s: (%d) version %d len %d ",
                  __FUNCTION__, __LINE__, msgHeader.version, currLen);

  if (msgHeader.version != DOT1S_CHECKPOINT_MESSAGE_VER)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "Version %d not supported ", msgHeader.version);
    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT, "Error version does not match : ");
    return;
  }

  osapiSemaTake(dot1sCkptMaskSema, L7_WAIT_FOREVER);
  while (currLen > 0)
  {
    currTlvHeader = (dot1s_nsf_tlv_header_t *)currPtr;

    tlvHeader.tlvType = osapiNtohs(currTlvHeader->tlvType);
    tlvHeader.tlvLen = osapiNtohs(currTlvHeader->tlvLen);
    tlvHeader.index = osapiNtohl(currTlvHeader->index);
    if (tlvHeader.tlvLen == 0)
    {
      DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                    "Error received TLV of len 0 :type %d len %d index %d",
                    tlvHeader.tlvType, tlvHeader.tlvLen, tlvHeader.index);
      break;
    }


    DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT_DETAIL,
                    "Dot1s received TLV: type %d len %d index %d",
                    tlvHeader.tlvType, tlvHeader.tlvLen, tlvHeader.index);


    switch (tlvHeader.tlvType)
    {
      case DOT1S_TLV_PORT_INST_CKPT_BEGIN:
        {
          if (instIndex != L7_DOT1S_MSTID_ALL)
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                    "Error: instIndex(%d) not the value expected. "
                    "Msgs received out of order",instIndex);
            DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                          "Unexpectedly received Begin TLV for instIndex(%d) "
                          "Msgs received out of order",instIndex);
            break;
          }
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                          "Rx DOT1S_TLV_PORT_INST_CKPT_BEGIN for inst %d ",
                          tlvHeader.index);
          instIndex = tlvHeader.index;

          if (tlvHeader.index <= L7_MAX_MULTIPLE_STP_INSTANCES)
          {
            instState[tlvHeader.index] = DOT1S_INST_CKPT_INPROGRESS;
          }

        }
        break;

      case DOT1S_TLV_PORT_INST_CKPT_END:
        if (instIndex != tlvHeader.index)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                  "Error: instIndex(%d) not the value expected. "
                  "Msgs received out of order",instIndex);
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                  "Unexpectedly received End TLV for instIndex(%d) "
                  "Msgs received out of order",instIndex);
          break;
        }
        DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                        "Rx DOT1S_TLV_PORT_INST_CKPT_END for inst %d",
                        tlvHeader.index);
        instIndex = L7_DOT1S_MSTID_ALL;
        if (tlvHeader.index <= L7_MAX_MULTIPLE_STP_INSTANCES)
        {
          instState[tlvHeader.index] = DOT1S_INST_CKPT_READY;
        }
        break;
      case DOT1S_TLV_PORT_COMMON:
        {
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                          "Rx Port common TLV for port %d", tlvHeader.index);
          ckptPort = dot1sIntfCkptDataFind(tlvHeader.index);
          if (ckptPort == L7_NULLPTR)
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                    "Error: Cannot find check pointed data for port %d ",
                    tlvHeader.index);
            break;
          }
          /* Assuming here that the common peice is less than 2 bytes.
            This code needs to be changed if that assumption is no longer true
          */
          tlvData = currPtr + sizeof(tlvHeader);
          /*tempPortCommonData = osapiNtohs(*(L7_ushort16 *)tlvData);*/
          memcpy(&ckptPort->common, tlvData, DOT1S_PORT_COMMON_CKPT_SIZE);

        }

        break;
      case DOT1S_TLV_PORT_INST_INFO:
        {
          if (instIndex == L7_DOT1S_MSTID_ALL)
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                   "Error: instIndex ALL value not valid for PORT(%d)inst TLV "
                    , tlvHeader.index);
            DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                   "Error: instIndex ALL value not valid for PORT(%d)inst TLV "
                    , tlvHeader.index);
            break;
          }
          DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                          "Rx Port(%d) inst(%d) TLV ",
                          tlvHeader.index, instIndex);
          tlvData = currPtr + sizeof(tlvHeader);

          rxInstPortData = (dot1s_nsf_port_inst_ckpt_t *)tlvData;
          ckptPort = dot1sIntfCkptDataFind(tlvHeader.index);
          if (ckptPort == L7_NULLPTR)
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                    "Error: Cannot find check pointed data for port %d ",
                    tlvHeader.index);
            break;
          }

          ckptPort->instInfo[instIndex].disputed = rxInstPortData->disputed;
          ckptPort->instInfo[instIndex].portRole = rxInstPortData->portRole;
          ckptPort->instInfo[instIndex].portState = rxInstPortData->portState;

        }
        break;

      default:
        tempCurrLen = currPtr - checkpoint_data;
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
                "Incorrect objectType(%d) received from ckpt at %d",
                tlvHeader.tlvType, tempCurrLen);
        DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,
                    "Error received unsupported TLV:type %d len %d index %d",
                    tlvHeader.tlvType, tlvHeader.tlvLen, tlvHeader.index);
        dot1sNsfDebugPktDump(currPtr, L7_NULL);
        break;
    }/* switch based on tlv type*/

    currPtr += tlvHeader.tlvLen;
    currLen -= tlvHeader.tlvLen;


  } /*currLen > 0*/
  osapiSemaGive(dot1sCkptMaskSema);

  populateEndTime = osapiTimeMillisecondsGet64();

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT, "Time to process message %d ms",
               (populateEndTime - populateStartTime));


}

/*********************************************************************
* @purpose  Callback for the backup to handle checkpointed data
*
* @param    checkpoint_data  @b{(input)}  checkpointed data
* @param    data_len    @b{(inout)}   size of the data
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
void dot1sBackupUnitCheckpoint(L7_CKPT_BACKUP_MGR_CALLBACK_t callback_type,
                               L7_uchar8                  *checkpoint_data,
                               L7_uint32                   data_len)
{

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT,"%s: ", __FUNCTION__);

  switch (callback_type)
  {
    case CKPT_BACKUP_SYNC_DATA:
      /* build the message*/
      dot1sPopulateCheckpointData(checkpoint_data, data_len);
      break;
    case CKPT_BACKUP_NEW_MANAGER:
      osapiSemaTake(dot1sCkptMaskSema, L7_WAIT_FOREVER);
      dot1sCheckPtDataReset();
      osapiSemaGive(dot1sCkptMaskSema);
      break;

    default:
      break;
  }

  return;
}

/*********************************************************************
* @purpose  Notify the checkpoint service about availability of data
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
*
* @returns  L7_SUCCESS
*
* @notes    a. sets the bitmask for the appropriate interface
*           b. copies the data from operational structure to
*              checkpoint strucutres
*
* @end
*********************************************************************/
L7_RC_t dot1sCallCheckpointService(L7_INTF_MASK_t *intfMask,
                                   L7_uint32 instIndex)
{

  dot1s_nsf_port_checkpoint_data_t *ckptPort = L7_NULLPTR;
  L7_RC_t rc = L7_SUCCESS;
  DOT1S_PORT_COMMON_t *p;
  L7_INTF_MASK_t tempMask;
  L7_uint32 intIfNum = 0;

  /* Do not propogate any changes if we are in the process of switching over*/
  if ((dot1sBackupMgrElected != L7_TRUE) ||
      (dot1sCnfgrAppIsReady() != L7_TRUE))
  {
    return rc;
  }

  memcpy(&tempMask, intfMask, sizeof(tempMask));

  L7_INTF_FHMASKBIT(tempMask, intIfNum);
  while (intIfNum != 0)
  {
    p = dot1sIntfFind(intIfNum);
    if (p== L7_NULLPTR)
    {
      return L7_FAILURE;
    }
    /* Copy the application data to check point data*/

    ckptPort = dot1sIntfCkptDataFind(intIfNum);
    if (ckptPort == L7_NULLPTR)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
              "Error: Cannot find check pointed data for port %s ", ifName);
      return L7_FAILURE;
    }

    osapiSemaTake(dot1sCkptMaskSema, L7_WAIT_FOREVER);
    if (instIndex != L7_DOT1S_MSTID_ALL)
    {
      /* update the state for inst port ckpt data*/
      if (instState[instIndex] == DOT1S_INST_CKPT_READY)
      {
        instState[instIndex] = DOT1S_INST_CKPT_BEGIN;
      }

      rc = dot1sGetPortInstData(p, instIndex, ckptPort);

      L7_INTF_SETMASKBIT(portInstCkptBitMask[instIndex],intIfNum);

    }
    else
    {

      rc = dot1sGetPortCommonData(p,ckptPort);
      L7_INTF_SETMASKBIT(portCommonCkptBitMask,intIfNum);
    }


    osapiSemaGive(dot1sCkptMaskSema);

    L7_INTF_CLRMASKBIT(tempMask,intIfNum);
    L7_INTF_FHMASKBIT(tempMask, intIfNum);
  }

  if (rc == L7_SUCCESS)
  {
    /* call the check point service */
    rc = ckptMgrCheckpointStart(L7_DOT1S_COMPONENT_ID);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DOT1S_COMPONENT_ID,
              "instIndex(%d) call to checkpoint"
              " service to start failed(%d) ", instIndex, rc);
    }
  }

  return rc;

}

/*********************************************************************
* @purpose  Get the checkpointed state for a port / instance 
*
* @param    intIfNum  @b{(input)}   internal intf of the port
* @param    instIndex  @b{(input)}  inst index
* @param    *state  @b{(ouput)}  Check pointed state of the port
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    a. The caller must make assumption of state if call fails 
*           b. Should only be called during reconciliation phase.
*
* @end
*********************************************************************/
L7_RC_t dot1sCkptStateGet(L7_uint32 intIfNum, L7_uint32 instIndex, 
                             L7_uint32 *state)
{
  dot1s_nsf_port_checkpoint_data_t *ckptPort;

  ckptPort = dot1sIntfCkptDataFind(intIfNum);
  if (ckptPort == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *state = (L7_uint32 )ckptPort->instInfo[instIndex].portState;
  return L7_SUCCESS;
}

/**************************Debug routines****************************/
void dot1sDebugCkptMaskDump()
{
  L7_uint32 instIndex, port;
  L7_uint32 firstTime = L7_FALSE;
  for (instIndex = 0; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES ; instIndex++)
  {
    for (port = 1; port < L7_MAX_INTERFACE_COUNT; port++ )
    {
      if (L7_INTF_ISMASKBITSET(portInstCkptBitMask[instIndex], port) != L7_FALSE)
      {
        if (firstTime == L7_FALSE)
        {
          firstTime = L7_TRUE;
          printf("\n Following port bits set for Instance %d ", instIndex);

        }
        printf(" %d", port);
      }
    }
    firstTime = L7_FALSE;
  }

  printf("\nFollowing ports set for port common ");
  for (port = 1; port < L7_MAX_INTERFACE_COUNT; port++ )
  {
    if (L7_INTF_ISMASKBITSET(portCommonCkptBitMask, port) != L7_FALSE)
    {
      printf(" %d", port);
    }
  }
}

void dot1sDebugCkptDataDump(L7_uint32 port, L7_BOOL instDisplayFlag)
{
  L7_uint32 i;
  dot1s_nsf_port_checkpoint_data_t *ckptPort = L7_NULLPTR;

  ckptPort = dot1sIntfCkptDataFind(port);
  if (ckptPort == L7_NULLPTR)
  {
    printf("Error: Cannot find check pointed data for port %d\n ", port);
    return;
  }

  printf("Port(%d) Common Data \n", port);
  printf("operEdge %s\n", (ckptPort->common.operEdge == L7_TRUE) ?
         "TRUE" :"FALSE");
  printf("rcvdBpdu %s\n", (ckptPort->common.rcvdBpdu == L7_TRUE) ?
         "TRUE" :"FALSE");
  printf("rcvdRSTP %s\n", (ckptPort->common.rcvdRSTP == L7_TRUE) ?
         "TRUE" :"FALSE");
  printf("rcvdSTP %s\n", (ckptPort->common.rcvdSTP == L7_TRUE) ?
         "TRUE" :"FALSE");
  printf("rcvdMSTP %s\n", (ckptPort->common.rcvdMSTP == L7_TRUE) ?
         "TRUE" :"FALSE");
  printf("sendRSTP %s\n", (ckptPort->common.sendRSTP == L7_TRUE) ?
         "TRUE" :"FALSE");
  printf("bpduGuardEffect %s\n",(ckptPort->common.bpduGuardEffect == L7_TRUE) ?
         "TRUE" :"FALSE");
  printf("loopInconsistent %s\n",(ckptPort->common.loopInconsistent == L7_TRUE) ?
         "TRUE" :"FALSE");
  printf("diagnosticDisable %s\n",(ckptPort->common.diagnosticDisable == L7_TRUE) ?
         "TRUE" :"FALSE");

  printf("\nPort inst values \n");

  if (instDisplayFlag == L7_TRUE)
  {

    for (i = 0; i <= L7_MAX_MULTIPLE_STP_INSTANCES ; i++)
    {
      printf("Inst %d disputed %s Role %s State %s\n",
             i,(ckptPort->instInfo[i].disputed == L7_TRUE) ? "TRUE": "FALSE",
             dot1sRoleStringGet(ckptPort->instInfo[i].portRole),
             dot1sStateStringGet(ckptPort->instInfo[i].portState));
    }
  }

}

void dot1sNsfDebugPktDump(L7_uchar8 *pkt, L7_uint32 length)
{
  int row, column;
  L7_uint32 numRows = 4, numCols = 16;

  if (length != 0)
  {
      numRows = length/numCols;
  }

  printf("===================\n");
  for (row = 0; row <= numRows; row++)
  {
    printf("%04x ", row * numCols);
    for (column = 0; column < numCols; column++)
    {
      printf("%2.2x ",pkt[row*numCols + column]);
    }
    printf("\n");
  }
  printf("===================\n");
}
