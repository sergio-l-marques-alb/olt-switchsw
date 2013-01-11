/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008
*
**********************************************************************
*
* @filename   ipstk_ckpt_api.h
*
* @purpose    ipstack Mapping layer external function prototypes
*             for non-stop forwarding checkpointing APIs
*
* @component  ipstack Mapping Layer 
*
* @comments   none
*
* @create     
*
* @author     w. jacobs
* @end
*
**********************************************************************/

#ifndef INCLUDE_IPSTK_CKPT_API
#define INCLUDE_IPSTK_CKPT_API

#include <l7_common.h>

/*********************************************************************
* @purpose  Init ipstk checkpoint code.
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ipstkCheckpointInit(void);


/*********************************************************************
* @purpose  Callback invoked by the checkpoint service on the management unit.
*
* @param  checkpointData  @b{(input)}  Checkpoint message buffer. 
* @param  maxDataLen      @b{(input)}  Number of bytes that IP MAP can write to
*                                        checkpoint message
* @param  msgLen          @b{(output)} Number of bytes written to checkpoint msg
* @param  moreData        @b{(output)} Set to L7_TRUE if there is more data to
*                                        checkpoint
*
* @returns  void
*
* @notes  runs on the checkpoint service thread
*
*
* @end
*********************************************************************/
L7_RC_t ipstkCheckpointIntlVlansWrite(L7_uchar8 *checkpointData,
                                      L7_uint32 maxDataLen,
                                      L7_uint32 *msgLen,
                                      L7_BOOL  *moreData);

/*********************************************************************
* @purpose  Callback invoked by the checkpoint service on the management unit.
*
* @param  checkpointData  @b{(input)}  Checkpoint message buffer. 
* @param  maxDataLen      @b{(input)}  Number of bytes that IP MAP can write to
*                                        checkpoint message
* @param  msgLen          @b{(output)} Number of bytes written to checkpoint msg
* @param  moreData        @b{(output)} Set to L7_TRUE if there is more data to
*                                        checkpoint
*
* @returns  void
*
* @notes  runs on the checkpoint service thread
*
* @returns  void
*
* @notes  runs on the checkpoint service thread
*
* @end
*********************************************************************/
void ipstkpCheckpointRtrIntfMsgWrite(L7_uchar8 *checkpointData,
                                    L7_uint32 maxDataLen,
                                    L7_uint32 *msgLen,
                                    L7_BOOL  *moreData);


/*********************************************************************
* @purpose  Mark the information for router interfaces 
*           that may need to be checkpointed to Standby unit
*
* @param  void  
*
* @returns  void
*
* @notes    Can be called when a new backup unit is reported
*
* @end
*********************************************************************/
void ipstkCheckpointRtrIntfDataPopulate(void);


/*********************************************************************
* @purpose  Flush any unapplied interface data on the management unit
*
* @param  void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ipstkCheckpointRtrIntfDataFlush(void);


/*********************************************************************
* @purpose  Flush any unapplied interface data on the backup unit
*
* @param  void
*
* @returns  void
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t ipstkCheckpointBackupUnitRtrIntfDataFlush(void);

/*********************************************************************
* @purpose  Process an internal VLAN TLV from a checkpoint message
*           received on the backup unit.
*
* @param    intlVlanTlv   @b{(input)}  Start of internal VLAN TLV 
*
* @returns  Number of bytes processed from the checkpoint message
*
* @notes    This function executes while IP MAP is in WMU state. It runs
*           on the checkpoint service thread.
*
* @end
*********************************************************************/
L7_uint32 ipstkCheckpointIntlVlanProcess(L7_uchar8 *ckptData);

#endif /*INCLUDE_IPSTK_CKPT_API*/


