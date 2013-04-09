/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  osapi_trace.h
*
* @purpose   OS independent prototype declarations
*
* @component osapi
*
* @comments  none 
*
* @create    11/02/2003
*
* @author     
*
* @end
*
*********************************************************************/
#ifndef OSAPITRACE_HEADER
#define OSAPITRACE_HEADER

#include "l7_common.h"

#ifdef L7_TRACE_ENABLED
 #define OSAPI_TRACE_EVENT(event, buff, buff_len) (osapi_trace_event(event, (L7_uchar8*)buff, buff_len))
 #define OSAPI_TRACE_TRIGGER(trigger)             (osapi_trace_trigger(trigger))
#else
 #define OSAPI_TRACE_EVENT(event, buff, buff_len) 
 #define OSAPI_TRACE_TRIGGER(trigger) 
#endif /* L7_TRACE_ENABLED */

typedef enum 
{
  L7_TRACE_EVENT_DRIVER_ENTER = 1,
  L7_TRACE_EVENT_DRIVER_EXIT = 2,
  L7_TRACE_EVENT_DRIVER_TX = 3,

  L7_TRACE_EVENT_USMDB_VLAN_ADD_ENTER = 100,
  L7_TRACE_EVENT_USMDB_VLAN_ADD_EXIT = 101,
  L7_TRACE_EVENT_UNITMGR_MASTER_FAIL = 102,
  L7_TRACE_EVENT_UNITMGR_DRIVER_RESTART = 103,

  L7_TRACE_EVENT_CNFGR_P3_START = 201,
  L7_TRACE_EVENT_CNFGR_P3_END = 202,
  L7_TRACE_EVENT_CNFGR_EXECUTE_START = 203,
  L7_TRACE_EVENT_CNFGR_EXECUTE_END = 204,
  L7_TRACE_EVENT_CNFGR_HW_UPDATE_GENERIC_DONE = 205,
  L7_TRACE_EVENT_CNFGR_HW_UPDATE_L3_DONE = 206,
  L7_TRACE_EVENT_CNFGR_HW_UPDATE_IPMCAST_DONE = 207,
  L7_TRACE_EVENT_CNFGR_COMP_ENTRY = 210,
  L7_TRACE_EVENT_CNFGR_COMP_END = 211,
  L7_TRACE_EVENT_CNFGR_COMP_EVT_ENTRY = 212,
  L7_TRACE_EVENT_CNFGR_COMP_ACT_ENTRY = 213,
  L7_TRACE_EVENT_CNFGR_COMP_ACT_END = 214,
  L7_TRACE_EVENT_CNFGR_MAIN_DELAY = 215,

  L7_TRACE_EVENT_NIM_CREATE_STARTUP_START = 301,
  L7_TRACE_EVENT_NIM_CREATE_STARTUP_END = 302,
  L7_TRACE_EVENT_NIM_ACTIVATE_STARTUP_START = 303,
  L7_TRACE_EVENT_NIM_ACTIVATE_STARTUP_END = 304,

  L7_TRACE_EVENT_CMGR_CONFIG_SLOTS_START = 401,
  L7_TRACE_EVENT_CMGR_CONFIG_SLOTS_END = 402,
  L7_TRACE_EVENT_CMGR_CARD_CREATE_START = 403,
  L7_TRACE_EVENT_CMGR_CARD_CREATE_END = 404,
  L7_TRACE_EVENT_CMGR_STARTUP_CREATE_START = 405,
  L7_TRACE_EVENT_CMGR_STARTUP_CREATE_END = 406,
  L7_TRACE_EVENT_CMGR_CONFIG_PORTS_START = 407,
  L7_TRACE_EVENT_CMGR_CONFIG_PORTS_END = 408,
  L7_TRACE_EVENT_CMGR_CARD_INSERT_START = 409,
  L7_TRACE_EVENT_CMGR_CARD_INSERT_END = 410,
  L7_TRACE_EVENT_CMGR_STARTUP_ACTIVATE_START = 411,
  L7_TRACE_EVENT_CMGR_STARTUP_ACTIVATE_END = 412,
  L7_TRACE_EVENT_CMGR_INSREM_EVT_RCV = 421,
  L7_TRACE_EVENT_CMGR_INSREM_EVT_CPT = 422,
  L7_TRACE_EVENT_CMGR_NIM_CREATE_NOTIFY = 423,
  L7_TRACE_EVENT_CMGR_NIM_EVENT_NOTIFY = 424,

  L7_TRACE_EVENT_OSPF2_GRACE_LSAS_SENT = 501,
  L7_TRACE_EVENT_OSPF2_RESTART_DONE = 502,

  L7_TRACE_EVENT_DOT1S_STARTUP_ACTIVATE_START = 601,
  L7_TRACE_EVENT_DOT1S_STARTUP_ACTIVATE_END = 602,
  L7_TRACE_EVENT_DOT1S_EXTERNAL_RECON_START = 603,
  L7_TRACE_EVENT_DOT1S_EXTERNAL_RECON_END = 604,
  L7_TRACE_EVENT_DOT1S_GLOBAL_RECON_END = 605,

  L7_TRACE_EVENT_EMWEB_CONFIG_APPLY_START = 701,
  L7_TRACE_EVENT_EMWEB_CONFIG_APPLY_END = 702,

  L7_TRACE_EVENT_LAST = 39999
} osapiTraceEvents;

typedef enum 
{
  L7_TRACE_TRIGGER_FIRST = 40000,

  L7_TRACE_TRIGGER_USMDB_VLAN_ADD_ENTER = 40100,
  L7_TRACE_TRIGGER_USMDB_VLAN_ADD_EXIT = 40101,
  L7_TRACE_TRIGGER_UNITMGR_MASTER_FAIL = 40102,
  L7_TRACE_TRIGGER_UNITMGR_DRIVER_RESTART = 40103,
  L7_TRACE_TRIGGER_DRIVER_UNIT_FAIL_START = 40104,
  L7_TRACE_TRIGGER_DRIVER_UNIT_FAIL_END = 40105,

	/* Define more triggers here....
	*/

	L7_TRACE_TRIGGER_LAST = 65535
} osapiTraceTriggers;

/**************************************************************************
*
* @purpose  Add event to the Operating System event utility.
*
* @param    event       Event Number
* @param    buff        Event Data
* @param    buff_len    Number of bytes in the event data
*
* @returns  None
*
* @comments   
*
* @end
*
*************************************************************************/
void osapi_trace_event (osapiTraceEvents event,
                        L7_uchar8 * buff,
                        L7_uint32 buff_len);

/**************************************************************************
*
* @purpose  Add trigger to the Operating System event utility.
*
* @param    trigger  Trigger Number
*
* @returns  None
*
* @comments   
*
* @end
*
*************************************************************************/
void osapi_trace_trigger (osapiTraceTriggers trigger);

#ifdef L7_TRACE_ENABLED
/**************************************************************************
*
* @purpose  Configure Windview to collect data in ring buffer for later retieval
*
* @param    class  Windview Class
*
* @returns  None
*
* @comments   
*     WV_CLASS_1      Context Switch
*     WV_CLASS_2      Task State Transition
*     WV_CLASS_3      Object and System Libraries
*
* @end
*
*************************************************************************/
STATUS osapiWvOn(int class);
#endif

#endif

