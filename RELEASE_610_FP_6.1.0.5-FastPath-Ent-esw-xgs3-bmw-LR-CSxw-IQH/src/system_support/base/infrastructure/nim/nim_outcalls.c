/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename nim_outcalls.c
*
* @purpose Network Interface Manager Call Out
*
* @component nim
*
* @comments none
*
* @create 12/20/2002
*
* @author wjacobs
*
* @end
**********************************************************************/

#include "l7_common.h"
#include "registry.h"
#include "nimapi.h"
#include "nim.h"
#include "nim_data.h"
#include "nim_util.h"
#include "nim_outcalls.h"
#include "dot3ad_api.h"
#include "dot1s_api.h"
#include "simapi.h"
#include "dtlapi.h"
#include "usmdb_util_api.h"
#include "dot1s_api.h"
#include "dot1x_api.h"
#include "dot1q_api.h"



/*********************************************************************
* @purpose Callout from NIM interface notifications
*
* @param   intIfNum    internal interface number
* @param   event       one of L7_PORT_EVENTS_t
*
* @return  none
*
* @notes   Hooks for interactions with other components or for platform-specific
*          extensions to the standard dot1q offering.  This outcall occurs for
*          NIM notifications of interface changes. The NIM callout may cause the
*          event to be propagated.
*
*          The LVL7 hooks to interact are specific hooks for forwarding/not
*          forwarding and link active/link inactive  notifications to be issued
*          when spanning tree is not enabled.
*
*          These hooks also ensure that the underlying platform is appropriately
*          configured with respect to spanning tree configuration.
*
*          These hooks also ensure that the filtering database is appropriately
*          configured with respect to a port's MAC address.
*
* @end
*********************************************************************/
void nimNotifyUserOfIntfChangeOutcall(L7_uint32 intIfNum, L7_uint32 event)
{
  L7_uint32 stpMode;
  L7_uint32 result;

  switch (event)
  {
    case L7_UP:
      if (!dot1sIsValidIntf(intIfNum))
      {
        /* 
         * always set to forwarding if the intf is not dot1s managed
         */
        nimNotifyIntfChange(intIfNum, L7_FORWARDING);
      } else
      {
          /* IEEE 802.1s Support for Multiple Spanning Tree */
          stpMode = dot1sModeGet();
          if (stpMode == L7_DISABLE)
          {
            /* MSTP disabled, set the Port State to manual Fowarding
             * for the CIST.
             */
            dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, intIfNum, L7_DOT1S_MANUAL_FWD);
        }
      }
      break;

    case L7_DOWN:
      if (!dot1sIsValidIntf(intIfNum))
      {
        /* 
         * always set to not forwarding if the intf is not dot1s managed
         */
        nimNotifyIntfChange(intIfNum, L7_NOT_FORWARDING);
      } else
      {
          /* IEEE 802.1s Support for Multiple Spanning Tree */
          stpMode = dot1sModeGet();

          if (stpMode == L7_DISABLE)
          {
            /* MSTP disabled, set the Port State to Disabled
             * for the CIST.
             */
            dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, intIfNum, L7_DOT1S_DISABLED);

            (void) dtlDot1sFlush (intIfNum);
          }
      }
      break;

    case L7_FORWARDING:
      /*
      ** Port is now forwarding but it also needs to be 802.1x authorized
      ** before it can become active.
      */
      if (dot1xIsValidIntf(intIfNum) == L7_TRUE )
      {
          if (dot1xPortIsAuthorized(intIfNum) == L7_SUCCESS)
          {
              nimNotifyIntfChange(intIfNum, L7_ACTIVE);
          }
      } else
      {
        /* Non dot1x port */
        nimNotifyIntfChange(intIfNum, L7_ACTIVE);
      }
      break;


    case L7_NOT_FORWARDING:
      /*
      ** Port is no longer forwarding and is now inactive
      */
      nimNotifyIntfChange(intIfNum, L7_INACTIVE);
      break;

    case L7_PORT_DISABLE:
	
	  if (!dot1sIsValidIntf(intIfNum))
      {
        /* 
         * always set to not forwarding if the intf is not dot1s managed
         */
        nimNotifyIntfChange(intIfNum, L7_NOT_FORWARDING);
      } else
      {
        /* IEEE 802.1s Support for Multiple Spanning Tree */

        stpMode = dot1sModeGet();
        if (stpMode == L7_DISABLE)
        {
          dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, intIfNum, L7_DOT1S_DISABLED);

          if (NIM_INTF_ISMASKBITSET(nimCtlBlk_g->forwardStateMask, intIfNum))
          {
            NIM_INTF_CLRMASKBIT(nimCtlBlk_g->forwardStateMask, intIfNum);
          }

          if (dot1sIsValidIntf(intIfNum) == L7_TRUE)
          {
            (void) dtlDot1sFlush (intIfNum);
          }
        }
	  }
      break;


    case L7_PORT_ENABLE:
   
	  if (!dot1sIsValidIntf(intIfNum))
      {
        /* 
         * always set to forwarding if the intf is not dot1s managed
         */
        nimNotifyIntfChange(intIfNum, L7_FORWARDING);
      } else
      {
      /* IEEE 802.1s Support for Multiple Spanning Tree */

      stpMode = dot1sModeGet();
      if (stpMode == L7_DISABLE)
      {
        /* MSTP disabled, set the Port State to manual Fowarding
         * for the CIST.
         */
        dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, intIfNum, L7_DOT1S_MANUAL_FWD);
      }
      }
      break;


    case L7_LAG_ACQUIRE:
      
        /* IEEE 802.1s Support for Multiple Spanning Tree */

        stpMode = dot1sModeGet();
        if (stpMode == L7_DISABLE)
        {
          result = NIM_INTF_ISMASKBITSET(nimCtlBlk_g->forwardStateMask, intIfNum);
          if (result)
          {
            NIM_INTF_CLRMASKBIT(nimCtlBlk_g->forwardStateMask, intIfNum);

            /* notify users thate link is no longer FWD and inactive */
            nimNotifyIntfChange(intIfNum, L7_NOT_FORWARDING);
          }

          /* MSTP disabled, set the lag member State to manual Fowarding
           * for the CIST.
           */
          dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, intIfNum, L7_DOT1S_MANUAL_FWD);
        } 
      break;

    case L7_LAG_RELEASE:

	  /* IEEE 802.1s Support for Multiple Spanning Tree */

	  stpMode = dot1sModeGet();
	  if (stpMode == L7_DISABLE)
	  {
		/* MSTP disabled, set the released lag member State to
		 * manual Fowarding for the CIST.
		 */
		dot1sIhSetPortState(L7_DOT1S_MSTID_ALL, intIfNum, L7_DOT1S_MANUAL_FWD);
	  }
      break;

    case L7_DOT1X_PORT_AUTHORIZED:
      /*
      ** Port is now 802.1x authorized but it also needs to be STP forwarding
      ** before it can become active.
      */
      result = NIM_INTF_ISMASKBITSET(nimCtlBlk_g->forwardStateMask, intIfNum);
      if (result)
      {
        nimNotifyIntfChange(intIfNum, L7_ACTIVE);
      }

      break;


    case L7_DOT1X_PORT_UNAUTHORIZED:
      /*
      ** Port is now 802.1x un-authorized and is now inactive
      */
      nimNotifyIntfChange(intIfNum, L7_INACTIVE);

      break;

    case L7_PROBE_SETUP:
      
        /* IEEE 802.1s Support for Multiple Spanning Tree */

        stpMode = dot1sModeGet();
        if (stpMode == L7_DISABLE || dot1sPortAdminModeGet(intIfNum) == L7_FALSE)
        {
          (void) dtlDot1sFlush(intIfNum);
      }
      break;

    default:
      break;
  }

  return;
}

/*********************************************************************
*
* @purpose Callout from NIM active state check for the specified interface.
*
* @param intIfNum @b((input)) Internal Interface Number
* @param activeResult @b((output)) pointer to result of outcalls,
*          L7_YES or L7_NO
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t nimGetIntfActiveStateOutcall(L7_uint32 intIfNum,
                                     L7_uint32 *activeResult)
{

  *activeResult = L7_NO;

  /*
  ** Check if port is 802.1x authorized
  */
  if (dot1xIsValidIntf(intIfNum) == L7_TRUE)
  {
    /* It cares both Supplicant & Authenticator ports */
    if (dot1xPortIsAuthorized(intIfNum) == L7_SUCCESS)
    {
       *activeResult = L7_YES;
    }
    else 
    {
       *activeResult = L7_NO;
    }
   
  } else
  {
    /* Non dot1x port */
    *activeResult = L7_YES;
  }

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Get the instance number associated with an interface
*
* @param    configId  @b{(input)} an instance of a nimConfigID_t structure
* @param    instance  @b{(output)} instance number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    Only supported for VLAN interfaces at this point
*
* @notes   Returns a relative instance number for the interface,
*          from 1 to the maximum number of ports for the interface type
*
*
* @end
*********************************************************************/
L7_RC_t nimPortInstanceNumGet(nimConfigID_t configId, L7_uint32 *instance)
{
  L7_uint32 index;
  L7_RC_t rc;

  rc = L7_SUCCESS;
  *instance = 0;

  switch (configId.type)
  {
    case L7_LOGICAL_VLAN_INTF:
      if (dot1qVlanIntfPortInstanceNumGet(configId.configSpecifier.vlanId, &index) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
        break;
      }
      *instance = index;
      break;
    default:
      NIM_LOG_MSG(" Only vlan interfaces supported at this time ");
      rc = L7_FAILURE;
      break;
  }

  return rc;
}
