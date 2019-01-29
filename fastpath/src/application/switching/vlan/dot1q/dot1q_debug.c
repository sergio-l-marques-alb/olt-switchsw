/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename dot1q_debug.c
*
* @purpose 802.1Q Debug File
*
* @component dot1q
*
* @comments none
*
* @create 08/14/2005
*
* @author akulkarni
*
* @end
*             
**********************************************************************/

#include "osapi.h"
#include "sysapi.h"
#include "dot1q.h"
#include "dot1q_cfg.h"
#include "dot1q_control.h"
#include "dot1q_debug.h"
#include "dot1q_cnfgr.h"
#include "dot1q_sid.h"
#include "nimapi.h"
#include "l7_product.h"

/* Max number of characters in an trace message */
#define DOT1Q_TRACE_LEN_MAX 255
#define L7_INTF_MASK_STRING_LENGTH  (((L7_MAX_INTERFACE_COUNT/32)+1)*15)   /* TBD: Reuse for mask utility */

char* dot1qDebugEventStringGet(L7_uint32 event);

extern void *dot1qMsgQueue;
extern dot1qCfgData_t      *dot1qCfg; 
extern  dot1qInfo_t         *dot1qInfo;
extern L7_uint32 dot1qEventsList[DOT1Q_MAX_EVENTS];
extern  L7_VLAN_MASK_t 		dot1qIntfVidList[L7_MAX_INTERFACE_COUNT +1];
extern dot1qInternalVlanInfo_t  *dot1qInternalVlanInfo;
extern dot1qVlanPendingCreate_t   dot1qVlanPendingCreate[DOT1Q_MAX_PENDING_CREATES + 1];


L7_uint32 dot1qDebugMsgs;
L7_uint32 dot1qProbeMask = L7_NULL;
static void dot1qTraceQueueMsgFormat(DOT1Q_MSG_t *msg, L7_uint32 direction);

/*********************************************************************
*
* @purpose Display help menu for component dot1q
*
* @param   void
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
void dot1qDebugHelp(void)
{
  sysapiPrintf ("\nList of dot1qDebug... functions ->\n\n");

  sysapiPrintf ("dot1qDebugMsgQueue() - Display the number of messages in the VLAN message queue\n");
  sysapiPrintf ("dot1qDebugTraceModeGet() - Get the tracing mode\n");
  sysapiPrintf ("dot1qDebugTraceModeSet(mode) - Set the tracing mode: 1= enable, 0 = disable\n");
  sysapiPrintf ("dot1qDebugConsoleTraceModeGet() - Get the console tracing mode\n");
  sysapiPrintf ("dot1qDebugConsoleTraceModeSet() - Set the console tracing mode: 1= enable, 0 = disable\n");
}

#ifdef BROADCOM_DEBUG
static L7_uint32 num = 0;
L7_uint32 probeData[DOT1Q_PROBE_INDEXES][DOT1Q_PROBE_POINTS];
#endif

/*********************************************************************
* @purpose  Linker assist function for this file    
*          
* @param    @b{(input)}     void
*
* @returns   void 
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1qDebugInit(void)
{
#ifdef BROADCOM_DEBUG
  probedata = osapiMalloc(sizeof(L7_uint32) * (DOT1Q_PROBE_INDEXES) * (DOT1Q_PROBE_POINTS));

  memset(probeData,0x0,sizeof(probeData));
	num = 0;
#endif

	return;
}


/*********************************************************************
* @purpose  Display the number of messages in the VLAN message queue.
*          
* @param    @b{(input)}   void
*
* @returns  void  
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1qDebugMsgQueue(L7_uint32 detailed)
{
  L7_int32 num;
  L7_RC_t rc;
  L7_uint32 i;

  rc = osapiMsgQueueGetNumMsgs(dot1qMsgQueue, &num);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Dot1q Messages in queue           : %d\n", num);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Dot1q Max Messages in queue       : %d\n", DOT1Q_MSG_COUNT);

  if (detailed != L7_FALSE)
  {
	 for (i=1;i < DOT1Q_MAX_EVENTS ; i++)
	 {
		 sysapiPrintf("Event %s num messages %d \n", dot1qDebugEventStringGet(i), dot1qEventsList[i]);
	 }
  }
}

/*********************************************************************
* @purpose  Display the number of messages in the VLAN message queue.
*          
* @param    @b{(input)}   void
*
* @returns  void  
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1qDebugMsgQueueCountersReset(L7_uint32 detailed)
{

   memset(dot1qEventsList, 0x0, sizeof(dot1qEventsList));
}

/*********************************************************************
* @purpose  Get the tracing mode
*
* @param      void
*
* @returns    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dot1qDebugTraceModeGet(void)
{
  if (dot1qCfg == L7_NULL)
  {
    return(L7_DISABLE);
  }

  if (dot1qCfg->traceFlags & DOT1Q_TRACE_ENABLED)
  {
      return L7_ENABLE;
  }
  return(L7_DISABLE);

}



/*********************************************************************
* @purpose  Set the debug trace mode.
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qDebugTraceModeSet(L7_uint32 mode)
{

    if (mode == L7_ENABLE)
    {
        dot1qCfg->traceFlags |= DOT1Q_TRACE_ENABLED;
    }
    else if (mode == L7_DISABLE)
    {
        dot1qCfg->traceFlags &= (~DOT1Q_TRACE_ENABLED);
    }
    else
        return L7_FAILURE;
        
    dot1qCfg->hdr.dataChanged = L7_TRUE;

    return L7_SUCCESS;
}




/*********************************************************************
* @purpose  Get the  console tracing mode
*
* @param    void
*
* @returns   mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dot1qDebugConsoleTraceModeGet(void)
{
  if (dot1qCfg == L7_NULL)
  {
    return(L7_DISABLE);
  }

  if (dot1qCfg->traceFlags & DOT1Q_TRACE_TO_CONSOLE)
  {
      return L7_ENABLE;
  }
  return(L7_DISABLE);

}

/*********************************************************************
* @purpose  Set the console debug trace mode.
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qDebugConsoleTraceModeSet(L7_uint32 mode)
{

    if (mode == L7_ENABLE)
    {
        dot1qCfg->traceFlags |= DOT1Q_TRACE_TO_CONSOLE;
    }
    else if (mode == L7_DISABLE)
    {
        dot1qCfg->traceFlags &= (~DOT1Q_TRACE_TO_CONSOLE);
    }
    else
        return L7_FAILURE;

    dot1qCfg->hdr.dataChanged = L7_TRUE;

    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the dot1q debug message mode
*
* @param    void
*
* @returns   mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dot1qDebugMsgModeGet(void)
{
  return dot1qDebugMsgs;
}

/*********************************************************************
* @purpose  Set the dot1q debug message mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1qDebugMsgModeSet(L7_uint32 mode)
{

    if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
    {
      sysapiPrintf ("Invalid value specified (%u)\n", mode);
      return L7_FAILURE;
    }

    dot1qDebugMsgs = mode;

    return L7_SUCCESS;
}



/*====================================================================*/
/*====================================================================*/
/*====================================================================*/
/*====================================================================*/
/*
            Internal Debug  Routines
            
*/

L7_char8* dot1qDebugEventStringGet(L7_uint32 event)
{
	switch (event)
	{
	case dot1q_first_Event:return"dot1q_first_Event";
	case dot1q_vlan_create_static:return"dot1q_vlan_create_static";
	case dot1q_vlan_delete_static:return"dot1q_vlan_delete_static";
	case dot1q_vlan_create_dynamic:return"dot1q_vlan_create_dynamic";
	case dot1q_vlan_delete_dynamic:return"dot1q_vlan_delete_dynamic";
    case dot1q_vlan_tagged_set:return"dot1q_vlan_tagged_set";
	case dot1q_vlan_tagged_set_static_mask:return"dot1q_vlan_tagged_set_static_mask";
	case dot1q_vlan_member_set:return"dot1q_vlan_member_set";
	case dot1q_vlan_member_set_static_mask:return"dot1q_vlan_member_set_static_mask";
	case dot1q_vlan_tagged_set_dynamic:return"dot1q_vlan_tagged_set_dynamic";
	case dot1q_vlan_dynamic_to_static_set:return"dot1q_vlan_dynamic_to_static_set";
	case dot1q_qports_vid_set:return"dot1q_qports_vid_set";
	case dot1q_qports_accept_frame_type_set:return"dot1q_qports_vid_set";
	case dot1q_qports_default_user_prio_set:return"dot1q_qports_default_user_prio_set";
	case dot1q_qports_default_user_prio_global_set:return"dot1q_qports_default_user_prio_global_set";
	case dot1q_qports_ingress_filter_set:return"dot1q_qports_ingress_filter_set";
	case dot1q_qports_traffic_classes_enabled_set:return"dot1q_qports_traffic_classes_enabled_set";
	case dot1q_qports_traffic_class_set:return"dot1q_qports_traffic_class_set";
	case dot1q_qports_num_traffic_classes_set:return"dot1q_qports_num_traffic_classes_set";
	case dot1q_qports_num_traffic_classes_global_set:return"dot1q_qports_num_traffic_classes_global_set";
	case dot1q_qports_traffic_class_global_set:return"dot1q_qports_traffic_class_global_set";
	case dot1q_clear:return"dot1q_clear";
	case dot1q_cnfgr:return"dot1q_cnfgr";
	case dot1q_intfChange:return"dot1q_intfChange";
	case dot1q_qports_vlan_membership_delta_mask_set:return"dot1q_qports_vlan_membership_delta_mask_set";
    case dot1q_qports_vlan_tagging_delta_mask_set:return"dot1q_qports_vlan_tagging_delta_mask_set";
    case dot1q_vlan_create_static_mask:return"dot1q_vlan_create_static_mask";
    case dot1q_vlan_delete_static_mask:return"dot1q_vlan_delete_static_mask";
	case dot1q_last_event :return"dot1q_last_event";
	default:return "Unknwon event"; 
	}
}

/*====================================================================*/
/*====================================================================*/
/*====================================================================*/

/*********************************************************************
* @purpose  Write a debug trace message.
*
* @param    traceMsg @b{(input)} A string to be displayed as a trace message.
*
* @notes    The input string must be no longer than IPMAP_TRACE_LEN_MAX
*           characters. This function will prepend the message with the
*           current time and will insert a new line character at the
*           beginning of the message.
*
* @end
*********************************************************************/
void dot1qTraceWrite(L7_uchar8 *traceMsg)
{
    L7_uint64 msecSinceBoot;
    L7_uint32 secSinceBoot;
    L7_uint32 msecs;
    L7_uchar8 debugMsg[300];

    if (traceMsg == L7_NULL)
        return;

    msecSinceBoot = osapiTimeMillisecondsGet64();
    secSinceBoot = msecSinceBoot / 1000;
    msecs = msecSinceBoot % 1000;


    sysapiPrintf("\n");
    /* For now, just print the message with a timestamp. */
    sprintf(debugMsg, "DOT1Q: %d.%03d sec:  ", secSinceBoot, msecs);
    strncat(debugMsg, traceMsg, DOT1Q_TRACE_LEN_MAX);
    sysapiPrintf(debugMsg);
}


/*********************************************************************
*
* @purpose  Trace message sent on dot1q queue
*
* @param    *msg    @b{(input)} pointer to DOT1Q_MSG_t
*            
*                            
* @returns  void
*
* @notes    
*           
*
* @end
*********************************************************************/
void dot1qTraceQueueMsgSend(DOT1Q_MSG_t *msg)
{
    dot1qTraceQueueMsgFormat(msg, 0);
}

/*********************************************************************
*
* @purpose  Trace message received on dot1q queue
*
* @param    *msg        @b{(input)} pointer to DOT1Q_MSG_t
*            
* @returns  void
*
* @notes    
*           
*
* @end
*********************************************************************/
void dot1qTraceQueueMsgRecv(DOT1Q_MSG_t *msg)
{
    dot1qTraceQueueMsgFormat(msg, 1);
}


/*********************************************************************
*
* @purpose  Format and trace message sent or received on dot1q queue
*
* @param    *msg        @b{(input)} pointer to DOT1Q_MSG_t
*
* @param    *direction  @b{(input)} if 0, interprets message as being sent on queue, 
*                                   otherwise interprets message as being received
*
* @returns  void
*            
*                            
* @notes    
*           
*
* @end
*********************************************************************/
void dot1qTraceQueueMsgFormat(DOT1Q_MSG_t *msg, L7_uint32 direction)
{
    
    L7_uchar8   maskString[L7_INTF_MASK_STRING_LENGTH]; /* number of chars per mask index */
    L7_uint32   *remainingMask;
    L7_uchar8   traceDirection[22];
    L7_int32    i;
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(msg->data.intIfNum, L7_SYSNAME, ifName);

    /* Determine the direction */

    if (direction == L7_NULL) 
    {
        sprintf(traceDirection, "Dot1q Task Msg Sent");
    }
    else
    {
        sprintf(traceDirection, "Dot1q Task Msg Recv");
    }

    /* Format and log the event */
	switch (msg->event)
	{
	case dot1q_vlan_create_static:
        DOT1Q_TRACE("%s: dot1q_vlan_create_static: vlan = %d\n", 
                    traceDirection, msg->vlanId);
		break;

	case dot1q_vlan_delete_static:
        DOT1Q_TRACE("%s: dot1q_vlan_delete_static: vlan %d\n", 
                    traceDirection, msg->vlanId);
		break;

	case dot1q_vlan_create_dynamic:
        DOT1Q_TRACE("%s: dot1q_vlan_create_dynamic: vlan %d\n", 
                    traceDirection, msg->vlanId);
		break;


	case dot1q_vlan_delete_dynamic:
        DOT1Q_TRACE("%s: dot1q_vlan_delete_dynamic: vlan %d\n", 
                    traceDirection, msg->vlanId);
		break;


    case dot1q_vlan_tagged_set:
        DOT1Q_TRACE("%s: dot1q_vlan_tagged_set_static: vlan %d, intf %d, %s, mode = %d\n",
                    traceDirection, msg->vlanId, msg->data.intIfNum, ifName, msg->mode );
		break;

	case dot1q_vlan_member_set:
        DOT1Q_TRACE("%s: dot1q_vlan_member_set: vlan %d, intf %d, %s, mode %d, Req %d \n",
                    traceDirection, msg->vlanId, msg->data.intIfNum, ifName, msg->mode,msg->requestor);
		break;

#if defined(DOT1Q_FUTURE_FUNC_GROUP_FILTER) && DOT1Q_FUTURE_FUNC_GROUP_FILTER
	case dot1q_vlan_group_filter_static:
        DOT1Q_TRACE("%s: dot1q_vlan_group_filter_static: vlan %d, intf %d, %s, mode %d\n",
                    traceDirection, msg->vlanId, msg->data.intIfNum, ifName, msg->mode );
		break;
#endif

	case dot1q_vlan_tagged_set_static_mask:

        /* Point to beginning of string to be printed */
        remainingMask = (L7_uint32 *)&msg->data.intfMask;

        /* Print each element of the string */

        for (i=(L7_MAX_INTERFACE_COUNT / 32); i >= 0 ;i--)
        {
          if ((L7_INTF_MASK_STRING_LENGTH - strlen(maskString)) >= 15)
          {
            sprintf(maskString,"%s 0x%.8x ",maskString,remainingMask[i]);
          }
        }


        DOT1Q_TRACE("%s: dot1q_vlan_tagged_set_static_mask: vlan %d, mode %d\n", 
                    traceDirection, msg->vlanId, msg->mode ); 
		break;

    case dot1q_vlan_member_set_static_mask:

        /* Point to beginning of string to be printed */
        remainingMask = (L7_uint32 *)&msg->data.intfMask;

        /* Print each element of the string */

        for (i=(L7_MAX_INTERFACE_COUNT / 32); i >= 0 ;i--)
        {
          if ((L7_INTF_MASK_STRING_LENGTH - strlen(maskString)) >= 15)
          {
            sprintf(maskString,"%s 0x%.8x ",maskString,remainingMask[i]);
          }
        }


        DOT1Q_TRACE("%s: dot1q_vlan_member_set_static_mask: vlan %d, mode %d\n", 
                    traceDirection, msg->vlanId, msg->mode );   
   		break;

#if defined(DOT1Q_FUTURE_FUNC_GROUP_FILTER) && DOT1Q_FUTURE_FUNC_GROUP_FILTER
	case dot1q_vlan_group_filter_dynamic:
        DOT1Q_TRACE("%s: dot1q_vlan_group_filter_dynamic: vlan %d, intf %d, %s, mode %d\n",
                    traceDirection, msg->vlanId, msg->data.intIfNum, ifName, msg->mode );
		break;
#endif


    case dot1q_vlan_dynamic_to_static_set:
        DOT1Q_TRACE("%s: dot1q_vlan_dynamic_to_static_set: vlan %d \n",
                    traceDirection, msg->vlanId );
		break;

	case dot1q_qports_vid_set:
        DOT1Q_TRACE("%s: dot1q_qports_vid_set: vlan %d, intf %d, %s, mode %d\n",
                    traceDirection, msg->vlanId, msg->data.intIfNum, ifName, msg->mode );
		break;

	case dot1q_qports_accept_frame_type_set:
        DOT1Q_TRACE("%s: dot1q_qports_accept_frame_type_set: intf %d, %s, mode %d\n",
                    traceDirection, msg->data.intIfNum, ifName, msg->mode );
		break;

	case dot1q_qports_default_user_prio_set:
        DOT1Q_TRACE("%s: dot1q_qports_default_user_prio_set: intf %d, %s, mode %d\n",
                    traceDirection, msg->data.intIfNum, ifName, msg->mode );
		break;

	case dot1q_qports_default_user_prio_global_set:
        DOT1Q_TRACE("%s: dot1q_qports_default_user_prio_global_set: mode %d\n", 
                    traceDirection, msg->mode );
		break;
                
	case dot1q_qports_ingress_filter_set:
        DOT1Q_TRACE("%s: dot1q_qports_ingress_filter_set:  intf %d, %s, mode %d\n",
                    traceDirection, msg->data.intIfNum, ifName, msg->mode );
		break;

	case dot1q_qports_traffic_classes_enabled_set:
        DOT1Q_TRACE("%s: dot1q_qports_traffic_classes_enabled_set: mode %d\n", 
                    traceDirection, msg->mode );
		break;

	case dot1q_qports_traffic_class_set:
        DOT1Q_TRACE("%s: dot1q_qports_traffic_class_set: intf %d, %s\n",
                    traceDirection, msg->data.intIfNum, ifName);
		break;

	case dot1q_qports_num_traffic_classes_set:
        DOT1Q_TRACE("%s: dot1q_qports_num_traffic_classes_set:  intf %d, %s, mode %d\n",
                    traceDirection, msg->data.intIfNum, ifName, msg->mode );
		break;

	case dot1q_qports_num_traffic_classes_global_set:
        DOT1Q_TRACE("%s: dot1q_qports_num_traffic_classes_global_set: mode %d\n", 
                    traceDirection, msg->mode );
		break;

	case dot1q_qports_traffic_class_global_set:
        DOT1Q_TRACE("%s: dot1q_qports_traffic_class_global_set\n", 
                    traceDirection); 
        break;

   case dot1q_clear:
        DOT1Q_TRACE("%s: dot1q_clear: \n", traceDirection); 
		break;

	case dot1q_intfChange:
        DOT1Q_TRACE("%s: dot1q_intfChange: intf %d, %s, event %d\n",
                    traceDirection, msg->data.intIfNum, ifName, msg->data.status.event );
		break;

	case dot1q_cnfgr:
        DOT1Q_TRACE("%s: dot1q_cnfgr: command %d, type %d, correlator %d\n", 
                    traceDirection, msg->data.CmdData.command, msg->data.CmdData.type, 
                    msg->data.CmdData.correlator );
		break;

    case  dot1q_queueSynchronize:
        DOT1Q_TRACE("%s:dot1q_queueSynchronize: vlan %d",traceDirection,msg->vlanId);
        break;

	case dot1q_qports_vlan_membership_delta_mask_set:
        DOT1Q_TRACE("%s:dot1q_qports_vlan_membership_delta_mask_set: interface %d, %s", traceDirection, msg->data.intfVlanMask.intIfNum, ifName);
		break;

	case dot1q_qports_vlan_tagging_delta_mask_set:
        DOT1Q_TRACE("%s:dot1q_qports_vlan_tagging_delta_mask_set: interface %d, %s",  traceDirection,  msg->data.intfVlanMask.intIfNum, ifName);
		break;
    case dot1q_vlan_create_static_mask:
        DOT1Q_TRACE("%s: dot1q_vlan_create_static_mask: vlan = %d\n", 
                    traceDirection, msg->vlanId);
        break;
    case dot1q_vlan_delete_static_mask:
        DOT1Q_TRACE("%s: dot1q_vlan_delete_static_mask: vlan = %d\n", 
                    traceDirection, msg->vlanId);
		break;
	default:
		DOT1Q_TRACE("%s: Unknown Event %d received \n", 
                    traceDirection,msg->event); 
		break;
	}

	return;

}





/*********************************************************************
* @purpose  Format a list of interfaces into a string
*
* @param     *intIfList     @b{(input)}  list of interfaces
* @param     numPorts       @b{(input)}  number of ports in list
* @param     *pMaskString   @b{(input)}  pointer to charString minimally of size L7_INTF_MASK_STRING_LENGTH  
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The interfaces can be physical or logical
*
* @end
*********************************************************************/
void dot1qDebugListToMaskString(L7_ushort16 *intIfList,L7_uint32 numPorts, L7_uchar8 *pMaskString)

{

    L7_uchar8       maskString[L7_INTF_MASK_STRING_LENGTH]; /* number of chars per mask index */
    L7_INTF_MASK_t  portMask;
    L7_uint32       *remainingMask;
    L7_int32        i;

    memset(&portMask, 0, sizeof(portMask));
    

    for (i=0; i<numPorts; i++)
    {
        L7_INTF_SETMASKBIT(portMask, intIfList[i]);
    }

    /* Point to beginning of string to be printed */
    remainingMask = (L7_uint32 *)&portMask;

    /* Print each element of the string */
    for (i=(L7_MAX_INTERFACE_COUNT / 32); i >= 0 ;i--)
    {
      if ((L7_INTF_MASK_STRING_LENGTH - strlen(maskString)) >= 15)
      {
        sprintf(maskString,"%s 0x%.8x ",maskString, remainingMask[i]);
      }
    }

    osapiStrncpySafe(pMaskString,maskString, sizeof(maskString));

}

#ifdef DOT1Q_DEBUG_UNIT_TEST
/*********************************************************************
* @purpose  Test printing of interface mask data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void dot1qDebugTestIntfMaskDump (L7_INTF_MASK_t *mask)
{
    
    L7_uchar8   maskString[L7_INTF_MASK_STRING_LENGTH]; /* number of chars per mask index */
    L7_uint32   *remainingMask;
    L7_int32   i;

    /* Point to beginning of string to be printed */
    remainingMask = (L7_uint32 *)mask;

    /* Print each element of the string */

    for (i=(L7_MAX_INTERFACE_COUNT / 32); i >= 0 ;i--)
    {
      if ((L7_INTF_MASK_STRING_LENGTH - strlen(maskString)) >= 15)
      {
        sprintf(maskString,"%s 0x%.8x ",maskString,remainingMask[i]);
      }
    }


    DOT1Q_TRACE("Test Dump of Intf Mask: %s\n", maskString);


}



/*********************************************************************
* @purpose  Convert interface mask to printable string
*
* @param    *mask       interface mask to print
* @param    *pString    pointer to a string of length L7_INTF_MASK_STRING_LENGTH
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void dot1qDebugIntfMaskToString(L7_INTF_MASK_t *mask, L7_uchar8 *pString)
{
    
    L7_uchar8   maskString[L7_INTF_MASK_STRING_LENGTH]; /* number of chars per mask index */
    L7_uint32   *remainingMask;
    L7_int32   i;

    /* Point to beginning of string to be printed */
    remainingMask = (L7_uint32 *)mask;

    /* Print each element of the string */

    for (i=(L7_MAX_INTERFACE_COUNT / 32); i >= 0 ;i--)
    {
      if ((L7_INTF_MASK_STRING_LENGTH - strlen(maskString)) >= 15)
      {
        sprintf(maskString,"%s 0x%.8x ",maskString,remainingMask[i]);
      }
    }

    memcpy(pString, maskString, L7_INTF_MASK_STRING_LENGTH);



}



/*********************************************************************
* @purpose  Debug Test Driver for routines in dot1q_api.c
*
* @param     deleteFlag     @b{(input)}  L7_TRUE, if exercise the delete path
* @param     numPorts       @b{(input)}  number of ports in list
* @param     *pMaskString   @b{(input)}  pointer to charString minimally of size L7_INTF_INDICES  
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This routine causes most paths in dot1q to be exercised
*
* @end
*********************************************************************/
void dot1qDebugApiTest(L7_uint32 directive)
{

    L7_INTF_MASK_t  portMask;   
    L7_uint32 vlanId;
    L7_uint32 intIfNum;
    L7_uint32 parm1;
    L7_uchar8  name[L7_MAX_VLAN_NAME];
    L7_uchar8  maskString[L7_INTF_MASK_STRING_LENGTH]; 



    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/
    /* 
        CREATE/CONFIGURE PATH
    */
    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/

    if (directive == 0)
    {

        sysapiPrintf ("\nHelp for dot1qDebugApiTest\n\n");

        sysapiPrintf ("\nFormat: dot1qDebugApiTest(directive)\n\n");

        sysapiPrintf ("0:  Displays help\n");
        sysapiPrintf ("1:  Test Create/Modify/Configure APIs\n");
        sysapiPrintf ("2:  Test Deletion APIs\n");
        sysapiPrintf ("3:  Test Get APIs\n");

     }




    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/
    /* 
        CREATE/CONFIGURE PATH
    */
    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/

    if (directive == 1)
    {


        /*----------------------------------*/
        /* 
            dot1qVlanCreate 
        */
        /*----------------------------------*/

        vlanId = 2;
        if (dot1qVlanCreate(vlanId, DOT1Q_ADMIN) != L7_SUCCESS)
            sysapiPrintf("Failed to create VLAN %d\n", vlanId);


        vlanId = 50;
        if (dot1qVlanCreate(vlanId, DOT1Q_GVRP) != L7_SUCCESS)
            sysapiPrintf("Failed to create VLAN %d\n", vlanId);


        /*----------------------------------*/
        /* 
            dot1qVlanNameSet 
        */
        /*----------------------------------*/
        vlanId = 2;
        if (dot1qVlanNameSet(vlanId, "Daffy" ) != L7_SUCCESS)
            sysapiPrintf("Failed to set name of VLAN %d\n", vlanId);


        vlanId = 50;
        if (dot1qVlanNameSet(vlanId, "Duck" ) != L7_SUCCESS)
            sysapiPrintf("Failed to set name of VLAN %d\n", vlanId);


        /*----------------------------------*/
        /* 
            dot1qVlanMemberSet 
        */
        /*----------------------------------*/
        vlanId = 2;
        intIfNum = 5;
        if (dot1qVlanMemberSet(vlanId, intIfNum, L7_DOT1Q_FIXED, DOT1Q_ADMIN) != L7_SUCCESS)
            sysapiPrintf("Failed to set membership: VLAN %d, intifNum %d, %s, %s\n", 
                         vlanId, intIfNum,  "DOT1Q_FIXED", "DOT1Q_ADMIN");


        intIfNum = 10;
        if (dot1qVlanMemberSet(vlanId, intIfNum, L7_DOT1Q_FORBIDDEN, DOT1Q_ADMIN) != L7_SUCCESS)
            sysapiPrintf("Failed to set membership: VLAN %d, intifNum %d, %s, %s\n", 
                         vlanId, intIfNum,  "L7_DOT1Q_FORBIDDEN", "DOT1Q_ADMIN");

        intIfNum = 15;
        if (dot1qVlanMemberSet(vlanId, intIfNum, L7_DOT1Q_NORMAL_REGISTRATION, DOT1Q_ADMIN) != L7_SUCCESS)
            sysapiPrintf("Failed to set membership: VLAN %d, intifNum %d, %s, %s\n", 
                         vlanId, intIfNum,  "L7_DOT1Q_NORMAL_REGISTRATION", "DOT1Q_ADMIN");

        vlanId = 50;
        intIfNum = 5;
        if (dot1qVlanMemberSet(vlanId, intIfNum, L7_DOT1Q_FIXED, DOT1Q_GVRP) != L7_SUCCESS)
            sysapiPrintf("Failed to set membership: VLAN %d, intifNum %d, %s, %s\n", 
                         vlanId, intIfNum,  "L7_DOT1Q_FIXED", "DOT1Q_GVRP");

        intIfNum = 10;
        if (dot1qVlanMemberSet(vlanId, intIfNum, L7_DOT1Q_FORBIDDEN, DOT1Q_GVRP) != L7_SUCCESS)
            sysapiPrintf("Failed to set membership: VLAN %d, intifNum %d, %s, %s\n", 
                         vlanId, intIfNum,  "L7_DOT1Q_FORBIDDEN", "DOT1Q_GVRP");


        /*----------------------------------*/
        /* 
            dot1qVlanTaggedMemberSet 
        */
        /*----------------------------------*/
        vlanId = 2;
        intIfNum = 5;
        parm1 = L7_DOT1Q_TAGGED;
        if (dot1qVlanTaggedMemberSet(vlanId, intIfNum, parm1, DOT1Q_ADMIN) != L7_SUCCESS)
            sysapiPrintf("Failed to set tagging: VLAN %d, intifNum %d, mode %d\n", 
                         vlanId, intIfNum, parm1);


        intIfNum = 10;
        parm1 = L7_DOT1Q_UNTAGGED;
        if (dot1qVlanTaggedMemberSet(vlanId, intIfNum, parm1, DOT1Q_ADMIN) != L7_SUCCESS)
            sysapiPrintf("Failed to set tagging: VLAN %d, intifNum %d, mode %d\n", 
                         vlanId, intIfNum, parm1);

        /* GVRP Registered */
        vlanId = 50;
        intIfNum = 5;
        parm1 = L7_DOT1Q_UNTAGGED;
        if (dot1qVlanTaggedMemberSet(vlanId, intIfNum, parm1, DOT1Q_ADMIN) != L7_SUCCESS)
           sysapiPrintf("Failed to set tagging: VLAN %d, intifNum %d, mode %d\n", 
                        vlanId, intIfNum, parm1);
       else
           sysapiPrintf("Tagging should not be allowed on dynamic VLAN \n");


       /*----------------------------------*/
       /* 
           dot1qVlanStaticEgressPortsSet 
       */
       /*----------------------------------*/
       vlanId = 2;

       memset(&portMask, 0, sizeof(portMask));

       /* set port bits */
       L7_INTF_SETMASKBIT(portMask, 4);
       L7_INTF_SETMASKBIT(portMask, 8);
       L7_INTF_SETMASKBIT(portMask, 12);
       L7_INTF_SETMASKBIT(portMask, 17);

       if (dot1qVlanStaticEgressPortsSet(vlanId, (NIM_INTF_MASK_t *)&portMask) != L7_SUCCESS)
           sysapiPrintf("Failed to set static egressMask of VLAN %d\n", vlanId);


       /* GVRP Registered */
       vlanId = 50;
       memset(&portMask, 0, sizeof(portMask));

       /* set port bits */
       L7_INTF_SETMASKBIT(portMask, 4);
       L7_INTF_SETMASKBIT(portMask, 8);
       L7_INTF_SETMASKBIT(portMask, 12);
       L7_INTF_SETMASKBIT(portMask, 17);

       if (dot1qVlanStaticEgressPortsSet(vlanId, (NIM_INTF_MASK_t *)&portMask) != L7_SUCCESS)
           sysapiPrintf("Failed to set static egressMask of VLAN %d\n", vlanId);
       else
           sysapiPrintf("Setting static egress mask should not be allowed on dynamic VLAN \n");



    }   /* directive == 1 )  */



    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/
    /* 
        DELETE PATH
    */
    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/

    if (directive == 2)
    {

        /*----------------------------------*/
        /* 
            dot1qVlanDelete 
        */
        /*----------------------------------*/

        vlanId = 2;
        if (dot1qVlanDelete(vlanId, DOT1Q_ADMIN) != L7_SUCCESS)
            sysapiPrintf("Failed to create VLAN %d\n", vlanId);


        vlanId = 4;
        if (dot1qVlanDelete(vlanId, DOT1Q_GVRP) != L7_SUCCESS)
            sysapiPrintf("Failed to create VLAN %d\n", vlanId);
    }   /* directive == 2 */



    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/
    /* 
        DISPLAY PATH
    */
    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/
    /*----------------------------------*/

    if (directive == 3)
    {

        /*----------------------------------*/
        /* 
            dot1qVlanNameGet 
        */
        /*----------------------------------*/
        vlanId = 2;
        if (dot1qVlanNameGet(vlanId, name ) != L7_SUCCESS)
            sysapiPrintf("Failed to get name of VLAN %d\n", vlanId);
        else
            sysapiPrintf("dot1qVlanNameGet: Vlan %d : name %s\n", vlanId, name);


        vlanId = 50;
        if (dot1qVlanNameGet(vlanId, name ) != L7_SUCCESS)
            sysapiPrintf("Failed to get name of VLAN %d\n", vlanId);
        else
            sysapiPrintf("dot1qVlanNameGet: Vlan %d : name %s\n", vlanId, name);


        /*----------------------------------*/
        /* 
            dot1qVlanMemberGet 
        */
        /*----------------------------------*/
        vlanId = 2;
        intIfNum = 5;
        if (dot1qVlanMemberGet(vlanId, intIfNum, &parm1) != L7_SUCCESS)
            sysapiPrintf("Failed to get membership: VLAN %d, intifNum %d, mode %d\n", 
                         vlanId, intIfNum,  parm1);
        else
            sysapiPrintf("dot1qVlanMemberGet: Vlan %d : intIfNum %d mode %d\n",
                          vlanId,intIfNum, parm1);


        intIfNum = 10;
        if (dot1qVlanMemberGet(vlanId, intIfNum, &parm1) != L7_SUCCESS)
            sysapiPrintf("Failed to get membership: VLAN %d, intifNum %d, mode %d\n", 
                         vlanId, intIfNum,  parm1);
        else
            sysapiPrintf("dot1qVlanMemberGet: Vlan %d : intIfNum %d mode %d\n",
                          vlanId,intIfNum, parm1);

        intIfNum = 15;
        if (dot1qVlanMemberGet(vlanId, intIfNum, &parm1) != L7_SUCCESS)
            sysapiPrintf("Failed to get membership: VLAN %d, intifNum %d, mode %d\n", 
                         vlanId, intIfNum,  parm1);
        else
            sysapiPrintf("dot1qVlanMemberGet: Vlan %d : intIfNum %d mode %d\n",
                          vlanId,intIfNum, parm1);

        vlanId = 50;
        intIfNum = 5;
        if (dot1qVlanMemberGet(vlanId, intIfNum, &parm1) != L7_SUCCESS)
            sysapiPrintf("Failed to get membership: VLAN %d, intifNum %d, mode %d\n", 
                         vlanId, intIfNum,  parm1);
        else
            sysapiPrintf("dot1qVlanMemberGet: Vlan %d : intIfNum %d mode %d\n",
                          vlanId,intIfNum, parm1);

        intIfNum = 10;
        if (dot1qVlanMemberGet(vlanId, intIfNum, &parm1) != L7_SUCCESS)
            sysapiPrintf("Failed to get membership: VLAN %d, intifNum %d, mode %d\n", 
                         vlanId, intIfNum,  parm1);
        else
            sysapiPrintf("dot1qVlanMemberGet: Vlan %d : intIfNum %d mode %d\n",
                          vlanId,intIfNum, parm1);


        /*----------------------------------*/
        /* 
            dot1qVlanTaggedMemberGet 
        */
        /*----------------------------------*/
        vlanId = 2;
        intIfNum = 5;
        if (dot1qVlanTaggedMemberGet(vlanId, intIfNum, &parm1) != L7_SUCCESS)
            sysapiPrintf("Failed to get tagging: VLAN %d, intifNum %d, mode %d\n", 
                         vlanId, intIfNum,  parm1);
        else
            sysapiPrintf("dot1qVlanTaggedMemberGet: Vlan %d : intIfNum %d mode %d\n",
                          vlanId,intIfNum, parm1);


        intIfNum = 10;
        if (dot1qVlanTaggedMemberGet(vlanId, intIfNum, &parm1) != L7_SUCCESS)
            sysapiPrintf("Failed to get tagging: VLAN %d, intifNum %d, mode %d\n", 
                         vlanId, intIfNum,  parm1);
        else
            sysapiPrintf("dot1qVlanTaggedMemberGet: Vlan %d : intIfNum %d mode %d\n",
                          vlanId,intIfNum, parm1);

        intIfNum = 15;
        if (dot1qVlanTaggedMemberGet(vlanId, intIfNum, &parm1) != L7_SUCCESS)
            sysapiPrintf("Failed to get tagging: VLAN %d, intifNum %d, mode %d\n", 
                         vlanId, intIfNum,  parm1);
        else
            sysapiPrintf("dot1qVlanTaggedMemberGet: Vlan %d : intIfNum %d mode %d\n",
                          vlanId,intIfNum, parm1);

        /* GVRP Registered */

        vlanId = 50;
        intIfNum = 5;
        if (dot1qVlanTaggedMemberGet(vlanId, intIfNum, &parm1) != L7_SUCCESS)
            sysapiPrintf("Failed to get tagging: VLAN %d, intifNum %d, mode %d\n", 
                         vlanId, intIfNum,  parm1);
        else
            sysapiPrintf("dot1qVlanTaggedMemberGet: Vlan %d : intIfNum %d mode %d\n",
                          vlanId,intIfNum, parm1);

        intIfNum = 10;
        if (dot1qVlanTaggedMemberGet(vlanId, intIfNum, &parm1) != L7_SUCCESS)
            sysapiPrintf("Failed to get tagging: VLAN %d, intifNum %d, mode %d\n", 
                         vlanId, intIfNum,  parm1);
        else
            sysapiPrintf("dot1qVlanTaggedMemberGet: Vlan %d : intIfNum %d mode %d\n",
                          vlanId,intIfNum, parm1);


        /*----------------------------------*/
        /* 
            dot1qVlanStaticEgressPortsGet 
        */
        /*----------------------------------*/
        vlanId = 2;
        if (dot1qVlanStaticEgressPortsGet(vlanId, (NIM_INTF_MASK_t *)&portMask, &parm1) != L7_SUCCESS)
            sysapiPrintf("Failed to static egress portmask: VLAN %d,\n", 
                         vlanId);
        else
        {

            dot1qDebugIntfMaskToString((L7_INTF_MASK_t *)&portMask, maskString);
            sysapiPrintf("dot1qVlanStaticEgressPortsGet: Vlan %d : mask %s  size %d\n",
                          vlanId,maskString, parm1);
        }


        /* GVRP Registered */
        vlanId = 50;
        if (dot1qVlanStaticEgressPortsGet(vlanId, (NIM_INTF_MASK_t *)&portMask, &parm1) != L7_SUCCESS)
        {
            sysapiPrintf("Failed to static egress portmask: VLAN %d,\n", 
                         vlanId );
            sysapiPrintf("This failure is correct behavior for a dynamic VLAN\n") ;
        }
        else
        {

            dot1qDebugIntfMaskToString(&portMask, maskString);
            sysapiPrintf("dot1qVlanStaticEgressPortsGet: Vlan %d : mask %s  size %d\n",
                          vlanId,maskString, parm1);
            sysapiPrintf("Dynamic VLANs should not have static egress mask\n"); 
        }


    }  /* directive == 3 */


}


#endif

/*********************************************************************
* @purpose  Collect the time values for the index specfied.
*
* @param    probeIndex 
*
* @param    probePoint 
*
*
* @returns  void
*
* @notes    The index is the different possible values of the data
*			While the probepoint is diff probes for the same data. 
*
* @end
*********************************************************************/
void dot1qDebugProbe(L7_uint32 probeIndex, L7_uint32 probePoint, L7_uint32 mask)
{
#ifdef BROADCOM_DEBUG
	if (!DOT1Q_IS_PROBE_MASK_SET(mask))
		return;
	probeData[probeIndex][probePoint -1] = osapiTimeMillisecondsGet();
	if(probePoint == 1)
		num++;
#endif
}



void dot1qProbeReport(L7_BOOL detailed)
{
#ifdef BROADCOM_DEBUG
	int i, j;
	L7_uint32 dt[DOT1Q_PROBE_POINTS - 1];
	L7_uint32 diff = 0; 

	memset (dt, 0x0, sizeof(dt));

	for (i =0; i < DOT1Q_PROBE_INDEXES; i++)
	{
		if (detailed)
			SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"%d >    ",i);
		for (j = 1; j < DOT1Q_PROBE_POINTS ; j++)
		{
			diff = (probeData[i][j] > probeData[i][0]) ? (probeData[i][j] - probeData[i][0]) : 0 ;
			if (detailed)
				SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
						  "  %d  ", diff); 
			dt[j -1] += diff;

		}
		if (detailed)
			SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n");



	}


	SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS," Total ");
	for (j = 0; j < DOT1Q_PROBE_POINTS - 1;j++ )
	{
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"  %d  ", dt[j]);
	}
	SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n");

	SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS," Avg   ");
	for (j = 0; j < DOT1Q_PROBE_POINTS - 1;j++ )
	{
		SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"  %d  ", (L7_uint32)(dt[j]/(num-1)));
	}
	SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n");

#endif
}

void dot1qProbeDump(L7_uint32 numRows)
{
#ifdef BROADCOM_DEBUG
	int i, j;

	if ((numRows == 0) || (numRows > DOT1Q_PROBE_INDEXES))
	{
		numRows = DOT1Q_PROBE_INDEXES;
	}

	for (i = 0; i < numRows ;i++)
	{
		SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS," %d ", i);
		for (j = 0; j < DOT1Q_PROBE_POINTS; j++)
		{
			SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"  %d  ", probeData[i][j]);

		}
		SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n");
	}
#endif
}

void dot1qProbeMaskSet(L7_uint32 mask)
{
	dot1qProbeMask |= mask;
}
L7_RC_t dot1qVIDListMaskPrint(L7_uint32 intfNum)
{

	nimUSP_t usp;
	L7_uint32 vlanId;
	L7_uint32 vlansPrinted = 0;
	/*extern void dot1qVlanMaskDump(L7_VLAN_MASK_t *pMask);*/
	L7_RC_t rc = nimGetUnitSlotPort(intfNum, &usp);

	if (rc == L7_ERROR)
		return L7_NOT_EXISTS;


	/*dot1qVlanMaskDump(&dot1qIntfVidList[intfNum]);*/

    for (vlanId = 1; vlanId <= L7_PLATFORM_MAX_VLAN_ID; vlanId++)
	{
		if (L7_VLAN_ISMASKBITSET(dot1qIntfVidList[intfNum], vlanId))
		{
			printf(" %d ", vlanId);
			vlansPrinted++;

		if ((vlansPrinted % 15) == 0)
		{
			printf("\n");
		}
	}

	}                            

    if (vlansPrinted > 0)
    {
        printf("\n");
    }



	return L7_SUCCESS;
}

void dot1qCurrentPortParmsPrint(L7_uint32 intIfNum)
{
    L7_uint32 pvid = 0, acceptFrameType = 0, ingressFiltering = 0;

    /* Operational Membership*/
    printf("Intf present in \n");
    dot1qVIDListMaskPrint(intIfNum);

    /* PVID*/
    dot1qQportsCurrentVIDGet(intIfNum, L7_DOT1Q_DEFAULT_VID_INDEX, &pvid);
    printf("PVID : %d  \n", pvid);

    /* Acceptable Frame Type*/
    dot1qQportsAcceptFrameTypeGet(intIfNum, &acceptFrameType);
    printf("Acceptable Frame Type : %d  \n", acceptFrameType);
    /* Ingress filtering*/
    dot1qQportsCurrentIngressFilterGet(intIfNum, &ingressFiltering);
    printf("Ingree Filtering : %d  \n", ingressFiltering);
}



/*********************************************************************
* @purpose  Display internal VLAN usage
*
* @param    void 
*
* @param    void 
*
*
* @returns  void
*
* @notes    
*			
*
* @end
*********************************************************************/
void dot1qDebugInternalUsageShow(void)
{
  L7_uint32 i;

  sysapiPrintf("\n");
  sysapiPrintf("Printing internal VLAN information\n");
  sysapiPrintf("\n");
  for (i = 1; i <= L7_DOT1Q_MAX_INTERNAL_VLANS; i++)
  {
     if (dot1qInternalVlanInfo[i].vlanId != 0) 
     {
         sysapiPrintf("index = %d    VLAN = %d    requestor = %d  Descr: %s \n",
                      i, dot1qInternalVlanInfo[i].vlanId,
                      dot1qInternalVlanInfo[i].requestor,
                      dot1qInternalVlanInfo[i].description);
     }
  }
  sysapiPrintf("\n");
}



/*********************************************************************
* @purpose  Display list of VLANs awaiting creation
*
* @param    void 
*
* @param    void 
*
*
* @returns  void
*
* @notes    
*			
*
* @end
*********************************************************************/
void dot1qDebugPendingVlanShow(void)
{
  L7_uint32 i;

  sysapiPrintf("\n");
  sysapiPrintf("Printing list of VLAN creates\n");
  sysapiPrintf("\n");
  for (i = 1; i <= DOT1Q_MAX_PENDING_CREATES; i++)
  {
     if (dot1qVlanPendingCreate[i].vlanId != 0) 
     {
         sysapiPrintf("index = %d    VLAN = %d    requestor = %d  \n",
                      i, dot1qVlanPendingCreate[i].vlanId,
                      dot1qVlanPendingCreate[i].vlanType);
     }
  }
  sysapiPrintf("\n");
}
