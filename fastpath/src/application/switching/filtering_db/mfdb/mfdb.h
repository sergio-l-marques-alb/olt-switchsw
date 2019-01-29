/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\application\layer2\mfdb\mfdb.h
*
* @purpose Multicast Forwarding Database Utilities Header File
*
* @component Multicast Forwarding Database
*
* @comments None
*
* @create 03/20/2002
*
* @author rjindal
*
* @end
*
*********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef INCLUDE_MFDB_H
#define INCLUDE_MFDB_H


#include "mfdb_api.h"
#include "l7_cnfgr_api.h"


typedef struct mfdb_stats_s
{
  L7_uint32       most_entries;                               /* most entries ever in MFDB table */
} mfdbStats_t;

typedef enum {
  MFDB_PHASE_INIT_0 = 0,
  MFDB_PHASE_INIT_1,
  MFDB_PHASE_INIT_2,
  MFDB_PHASE_WMU,
  MFDB_PHASE_INIT_3,
  MFDB_PHASE_EXECUTE,
  MFDB_PHASE_UNCONFIG_1,
  MFDB_PHASE_UNCONFIG_2,
} mfdbCnfgrState_t;


/*********************************************************************
*
* @purpose  To insert a VLANID-MAC entry in the avl tree.
*
* @param    mfdbData_t  *pData  pointer to the entry data structure
*
* @returns  L7_SUCCESS, if the entry is inserted
* @returns  L7_FAILURE, if the entry is not inserted
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbTreeEntryInsert(mfdbData_t *pData);

/*********************************************************************
*
* @purpose  To delete a VLANID-MAC entry from the avl tree.
*
* @param    mfdbData_t  *pData  pointer to the entry data structure
*
* @returns  L7_SUCCESS, if the entry is deleted
* @returns  L7_NOT_EXIST, if the entry is not found
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbTreeEntryDelete(mfdbData_t *pData);

/*********************************************************************
*
* @purpose  To update a VLANID-MAC entry in the avl tree.
*
* @param    mfdbData_t  *pData  pointer to the entry data structure
*
* @returns  L7_SUCCESS, if the entry is updated
* @returns  L7_FAILURE, if the entry is not found
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbTreeEntryUpdate(mfdbData_t *pData);

/*********************************************************************
*
* @purpose  To add a VLANID-MAC entry to the NPU.
*           This function is also used to modify an existing entry.
*
* @param    mfdbData_t  *pData  pointer to the entry data structure
*
* @returns  L7_SUCCESS, if entry is added (or modified)
* @returns  L7_FAILURE, if entry is not added (or modified)
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbDtlAdd(mfdbData_t *pData);

/*********************************************************************
*
* @purpose  To remove a VLANID-MAC entry from the NPU.
*
* @param    mfdbData_t  *pData  pointer to the entry data structure
*
* @returns  L7_SUCCESS, if entry is removed
* @returns  L7_FAILURE, if entry is not removed
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t mfdbDtlRemove(mfdbData_t *pData);

/*********************************************************************
*
* @purpose  To notify all the MFDB user components of any change
*           in the MFDB table.
*
* @param    mfdbData_t  *pData  pointer to the entry data structure
* @param    L7_uint32   index   index of the componentId
* @param    L7_uint32   event   the event notified to all users:
*                                        MFDB_ENTRY_ADD_NOTIFY,
*                                        MFDB_ENTRY_DELETE_NOTIFY,
*                                        MFDB_ENTRY_PORT_ADD_NOTIFY,
*                                        MFDB_ENTRY_PORT_DELETE_NOTIFY
*
* @returns  Void.
*
* @comments None.
*       
* @end
*
*********************************************************************/
void mfdbUserComponentsNotify(mfdbData_t *pData, L7_uint32 index, L7_uint32 event);

/*********************************************************************
*
* @purpose  To map a specific protocol to its component id.
*
* @param    L7_uint32  protocol  the protocol type
*                                (see L7_MFDB_PROTOCOL_TYPE_t)   
* @param    L7_uint32  *compId   pointer to user component id
*                                (see L7_COMPONENT_IDS_t)   
*
* @returns  Void.
*
* @returns  L7_SUCCESS, if the protocol is mapped to the component id
* @returns  L7_FAILURE, if invalid protocol type
*
* @end
*
*********************************************************************/
L7_RC_t mfdbProtocolToCompID(L7_uint32 protocol, L7_COMPONENT_IDS_t *compId);


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
L7_RC_t mfdbCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

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
L7_RC_t mfdbCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

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
L7_RC_t mfdbCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function undoes mfdbCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mfdbCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function undoes mfdbCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mfdbCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function undoes mfdbCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mfdbCnfgrFiniPhase3Process();


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
L7_RC_t mfdbCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason );

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

L7_RC_t mfdbCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason );

#endif /* MFDB_H */

