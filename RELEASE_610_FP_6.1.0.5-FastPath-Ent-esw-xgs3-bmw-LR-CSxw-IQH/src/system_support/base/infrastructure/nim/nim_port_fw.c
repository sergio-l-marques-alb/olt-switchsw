/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2002-2008
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename   nim_port_fw.c
*
* @purpose    Operations with physical port firmware
*
* @component  NIM
*
* @comments   none
*
* @create     14/04/2008
*
* @author     vkozlov
* @end
*
**********************************************************************/


#include "l7_common.h"
#include "nim_data.h"
#include "nim_util.h"
#include "usmdb_sim_api.h"
#include "nim_port_fw.h"
#include "dtlapi.h"
#include "dapi_struct.h"
#include "osapi.h"

#if L7_FEAT_SF10GBT
#include "hpc_sf_fwupdate.h"

L7_uint32   nimPortFwTaskID          = L7_NULL;

extern L7_uint32 cliGetUnitId();
void* nimFwRspQ = L7_NULL;


#define NIM_FW_DOWNLOAD_PROGRESS_WAIT_TIME 1

/*********************************************************************
* @purpose  Obtain firmware revision of SFX7101 module 
*           on slave unit
*           
* @param    msg @b{(input)} Containing HPC message
*
* @returns  void
*
* @notes    Called from HPC callback 
*
* @end
*********************************************************************/
void nimHpcFirmwareGetRevision(nimHpcMsgFwGetRevision_t* msg)
{
  
  DAPI_USP_t usp;
  nimHpcMessageHeader_t* hdr = &msg->hdr;
  usp.unit = msg->hdr.targetUnit;
  usp.slot = 0;
  usp.port = hdr->port - 1;

  hdr->result = hpcSF_GetFirmwareRevision(
                                &usp, 
                                msg->ifFWRevision.ifFwPartNumber,
                                msg->ifFWRevision.ifFwRevision);

}
  
/*********************************************************************
* @purpose  Callback for HPC messaging
*           
* @param    buffer @b{(input)} buffer containing the message
* @param    msg_len @b{(input)} Size of buffer(message length)
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void nimHPCReceiveCallback(L7_uchar8* buffer, L7_uint32 msg_len)
{
  L7_RC_t lrc;
  nimHpcMessageHeader_t* hdr;
  if (msg_len >= sizeof(nimHpcMessageHeader_t) && (buffer != L7_NULL))
  {
    hdr = (nimHpcMessageHeader_t*)buffer;
    /* responce - we are in stack manager  */
    if(hdr->bResponse == L7_TRUE )
    {
      /* need to transfer response to appropriate queue */
      if ( (lrc = osapiMessageSend(nimFwRspQ, buffer, msg_len, NIM_HPC_FWMSG_WAIT, L7_MSG_PRIORITY_NORM)) != L7_SUCCESS)
      {
                /* message send failed */
      }
    }
    else 
    {
      L7_BOOL processed = L7_TRUE;
      /* request - we are NOT in stack manager.
          we just need to retransmit message to receiver */
      if(hdr->bResponse == L7_FALSE)
      {
        hdr->result = L7_FAILURE;
        switch (hdr->msgFamily)
        {
        case NIM_HPC_FIRMWARE:
          {
            switch (hdr->msgID)
            {
            case NIM_HPC_FW_GET_REVISION:
              {
                nimHpcFirmwareGetRevision((nimHpcMsgFwGetRevision_t*)buffer);
              } /* case NIM_HPC_FW_GET_REVISION: */
              break;
            default:
              processed = L7_FALSE;
            break;
            } /* switch (hdr->msgID) */
          } /* case NIM_HPC_FIRMWARE: */
          break;
        default:
          processed = L7_FALSE;
        break;
        } /* switch (hdr->msgFamily) */
      } /* if */
      /* sending reply back */
      if (processed == L7_TRUE)
      {
        lrc = L7_FAILURE;
        hdr->bResponse = L7_TRUE;
        if ( (lrc = sysapiHpcMessageSend(L7_NIM_COMPONENT_ID, hdr->srcUnit,  msg_len ,buffer )) != L7_SUCCESS )
        {
                    /* message send failed */
        } 
      } /* if */
      else
      {
                /* message not processed*/
      }
    } /* else */
  } /* if */
}
#endif
