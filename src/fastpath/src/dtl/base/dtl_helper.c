/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
**********************************************************************
*
* @filename  dtl_helper.c
*
* @purpose   Technology specific functions and data for the hardware
*            platform control component
*
* @component
*
* @create    02/04/2009
*
* @author    colinw
* @end
*
*********************************************************************/

#include "l7_common.h"
#include "sysapi_hpc.h"
#include "dtl_helper.h"
#include "osapi.h"

/*********************************************************************
* @purpose  Routine for application helpers to send packets on the local
*           unit.
*          
* @param    frame              @b{(input)} 
* @param    frameSize          @b{(input)} 
* @param    priority           @b{(input)} 
* @param    slot               @b{(input)} 
* @param    port               @b{(input)} 
* @param    ignoreEgressRules  @b{(input)} 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t dtlHelperSend(L7_uchar8   *frame,
			          L7_uint32    frameSize,
			          L7_uint8     priority,
			          L7_uint8     slot,
			          L7_ushort16  port,
			          L7_BOOL      ignoreEgressRules)
{
  return sysapiHpcHelperSend(frame, frameSize, priority, slot, port, ignoreEgressRules);
}


/* Debug */
L7_RC_t dtlDebugHelperSend(L7_uint32 slot, L7_uint32 port, L7_uint32 frameCount, L7_uint32 frameSize, L7_uint32 priority)
{
  L7_RC_t    rc = L7_SUCCESS;
  L7_uchar8 *frame;
  L7_uint32  i;

  if (frameSize == 0)
  {
    frameSize = 64;
  }
  if (frameCount == 0)
  {
    frameCount = 1;
  }

  frame = (L7_uchar8 *)osapiMalloc(L7_DTL_COMPONENT_ID, frameSize);
  if (frame == L7_NULL)
  {
    return L7_FAILURE;
  }
  for (i = 0; i < frameSize; i++)
  {
    frame[i] = i;
  }

  for (i = 0; i < frameCount; i++)
  {
    rc = dtlHelperSend(frame, frameSize, priority, slot, port, L7_TRUE);
    if (rc != L7_SUCCESS)
      break;
  }

  osapiFree(L7_DTL_COMPONENT_ID, frame);

  return rc;
}
