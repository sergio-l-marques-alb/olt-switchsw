/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    garp_debug.c
* @purpose     Prints useful info about GARP components
* @component   GARP
* @comments    none
* @create      02/21/2005
* @author      vrynkov
* @author
* @end
*
**********************************************************************/

#include "gidapi.h"
#include "garp_leaveall_timer.h"
#include "garpcfg.h"
#include "garp_debug.h"
#include "dot1dgarp.h"
#include "garp_timer.h"
#include "dot1q_api.h"
#include "dot1s_api.h"
#include "garpctlblk.h"
#include "gipapi.h"
#include "l7_product.h"

extern garpCfg_t    *garpCfg;
extern void         *garpQueue;
extern void         *garpPduQueue;
extern GarpInfo_t garpInfo;
extern gvrpInfo_t *gvrpInfo;
extern gmrpInfo_t *gmrpInfo;
extern GARPCBptr     GARPCB;
extern multipleGIP_t *multipleGIP;


static L7_BOOL garpQueueFiftyPercentLogged = L7_FALSE;
static L7_BOOL garpQueueEightyPercentLogged = L7_FALSE;
static L7_BOOL garpQueueNinetyPercentLogged = L7_FALSE;

/* The elements in  GetApplicantState are character
 * equivalents of elements from gidtt.c/Applicant_states */
char *GetApplicantState[] =
{
    "Va",  /* Very anxious, active  */
    "Aa",  /* Anxious,      active  */
    "Qa",  /* Quiet,        active  */
    "La",  /* Leaving,      active  */
    "Vp",  /* Very anxious, passive */
    "Ap",  /* Anxious,      passive */
    "Qp",  /* Quiet,        passive */
    "Vo",  /* Very anxious observer */
    "Ao",  /* Anxious observer      */
    "Qo",  /* Quiet observer        */
    "Lo",  /* Leaving observer      */
    "Von", /* Very anxious observer, non-participant */
    "Aon", /* Anxious observer,      non-participant */
    "Qon"  /* Quiet_observer,        non-participant */
};


/* The elements in  GetRegisrarState are character
 * equivalents of elements from gidtt.c/Registrar_states */
char *GetRegisrarState[] =
{
    "Inn",
    "Lv",
    "Mt",

    "Inr", /* In, registration fixed */
    "Lvr",
    "Mtr",

    "Inf", /* In, registration forbidden */
    "Lvf",
    "Mtf"
};

static char *GetTimerType[] =
{
    "GARP Undf",
    "GVRP Leav",
    "GMRP Leav",
    "GVRP Join",
    "GMRP Join"
};

static char *GetTimerStatus[] =
{
    "Undf",
    "Pend",
    "Popd"
};

typedef enum
{
    GARP_ALL  = 0,
    GARP_GVRP,
    GARP_GMRP

} garpTimersToPrint;

/* for tracking a single Vlan transitions through the state machine */
static char* Gid_Dbg_Events[] = {

  "Gid_null", "Gid_rcv_leaveempty","Gid_rcv_leavein", "Gid_rcv_empty",

  "Gid_rcv_joinempty", "Gid_rcv_joinin",

  "Gid_join", "Gid_leave",

  "Gid_normal_operation", "Gid_no_protocol",

  "Gid_normal_registration", "Gid_fix_registration", "Gid_forbid_registration",

  "Gid_rcv_leaveall", "Gid_rcv_leaveall_range",

  "Gid_tx_leaveempty", "Gid_tx_leavein", "Gid_tx_empty", "Gid_tx_joinempty",

  "Gid_tx_joinin", "Gid_tx_leaveall", "Gid_tx_leaveall_range"

};

 garpVlanTransitions_t garpDebugVlanRec[L7_MAX_VLAN_TRACK];

L7_uint32 Track_Vlan[L7_MAX_VLAN_TRACK];
L7_BOOL   vlanFnd;
L7_BOOL   bTrackVlan;
L7_uint32 vlanCnt=0;
L7_uint32 VlanIndx;


extern Gvr     *gvrp_app;
extern L7_BOOL gvd_get_key(L7_uint32, Vlan_id *);
extern Gmr     *gmrpInstanceGet(L7_uint32);
extern void    garpPrintTimerInfo(L7_uint32);
extern void    gmrpPrintTimerInfo(L7_uint32 port_no, L7_uint32 vlan_Id);


extern gmrpInstance_t *gmrpInstance;

extern L7_BOOL gmd_get_key(void *my_gmd, L7_uint32 index, L7_uchar8 *key);

extern L7_uint32 getNumberOfGVDEntries();

/* DON'T DELETE: this function is needed so that linker linked in other
 * functions located in this file. */
void garpDumbFunction() { return; }


static void garpTraceMsgs(L7_uchar8 *traceMsg);
static void garpTraceQueueMsgFormat(GarpPacket *msg, L7_uint32 direction);

/* functions added for tracking single Vlan transistions through the state machine */
L7_uint32  garpDebugTrackVlan(L7_short16 vlanID);
void garpDebugTrackVlanInit();
void garpDebugTrackVlanTransitionsPrint();
void garpDebugTrackVlanSet(Vlan_id vlanId);

/*====================================================================*/
/*====================================================================*/
/*====================================================================*/
/*====================================================================*/
/*
            Debug Routines designed for support and maintenance:

*/
/*====================================================================*/
/*====================================================================*/
/*====================================================================*/



/*********************************************************************
*
* @purpose Display help menu for the GARP component
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
void garpDebugHelp(void)
{
  sysapiPrintf ("\n");
  sysapiPrintf ("\nList of garpDebug... functions\n");
  sysapiPrintf ("------------------------------\n");

  sysapiPrintf ("\n");
  sysapiPrintf ("GARP Internals:\n");
  sysapiPrintf ("\n");
  sysapiPrintf ("garpDebugGarpStatusShow()\n");
  sysapiPrintf ("       - Prints GARP Info structures for both GVRP and GMRP\n");
  sysapiPrintf ("garpDebugIntfCountersShow(intIfNum)\n");
  sysapiPrintf ("       - Prints GARP interface counters\n");
  sysapiPrintf ("garpDebugGmrpGidShow(vlanId,intIfNum)\n");
  sysapiPrintf ("       - Prints Gid structure associated with a vlan and interface\n");
  sysapiPrintf ("garpDebugGvrpGidShow(intIfNum)\n");
  sysapiPrintf ("       - Prints Gid structure associated with a interface\n");
  sysapiPrintf ("garpDebugDot1sInstanceShow(dot1sInstanceId)\n");
  sysapiPrintf ("       - Prints GARP dot1s instance info\n");
  sysapiPrintf ("garpDebugGmrpDot1sInstanceShow()\n");
  sysapiPrintf ("       - Prints VLANs in use for GMRP and dot1s\n");
   sysapiPrintf ("garpDebugGvrpStateInfo(no_of_attr_to_print, intIfNum)\n");
  sysapiPrintf ("       - Prints the states of GVRP attributes on the port\n");
  sysapiPrintf ("garpDebugGmrpStateInfo(no_of_attr_to_print, intIfNum, vlanId)\n");
  sysapiPrintf ("       - Prints the states of GMRP attributes on the port\n");
  sysapiPrintf ("garpDebugGvrpLeaveAllTimerInfo(intIfNum)\n");
  sysapiPrintf ("       - Prints the state of GVRP Leave All Timer running on the port\n");
  sysapiPrintf ("garpDebugGmrpLeaveAllTimerInfo(intIfNum)\n");
  sysapiPrintf ("       - Prints the state of GMRP Leave All Timer running on the port\n");
  sysapiPrintf ("garpDebugGvdRecordsInfo()\n");
  sysapiPrintf ("       - Prints the number of records in GVD database\n");
  sysapiPrintf ("garp_pending_info(timers_to_print, no_of_timers_to_print)\n");
  sysapiPrintf ("       - Prints no_of_timers_to_print of pending timers\n");
  sysapiPrintf ("garp_popped_info(timers_to_print, no_of_timers_to_print)\n");
  sysapiPrintf ("       - Prints no_of_timers_to_print of popped timers\n");

  sysapiPrintf ("\n");
  sysapiPrintf ("GARP Traces:\n");
  sysapiPrintf ("\n");

  sysapiPrintf ("garpDebugTraceModePrint()\n");
  sysapiPrintf ("       - Display the configured tracing modes\n");
  sysapiPrintf ("garpDebugTraceModeSet(mode)\n");
  sysapiPrintf ("       - Set a debug trace mode:  0 will display help\n");
  sysapiPrintf ("garpDebugTraceModeClear(mode)\n");
  sysapiPrintf ("       - Clear a debug trace mode:  0 will display help\n");


  sysapiPrintf ("\n");
  sysapiPrintf ("GARP Message Queue:\n");
  sysapiPrintf ("\n");

  sysapiPrintf ("garpDebugMsgQueueShow()\n");
  sysapiPrintf ("       - Display message queue info\n");
  sysapiPrintf ("garpDebugMsgQueueCountsClear()\n");
  sysapiPrintf ("       - Clear message queue counts\n");

  sysapiPrintf("\n");
  sysapiPrintf("Garp Track Transitions per Vlan:\n");
  sysapiPrintf("\n");

  sysapiPrintf("garpDebugTrackVlan(<vlanId>)\n");
  sysapiPrintf("         - Set Vlans to be tracked .Can track Max of %d vlans.\n",L7_MAX_VLAN_TRACK);
  sysapiPrintf("garpDebugTrackVlanPrint()\n");
  sysapiPrintf("        -Print the transitions for vlans tracked.");
  sysapiPrintf("garpDebugTrackVlanResetAll() \n");
  sysapiPrintf("         - Clear all Vlans being tracked.");

}



/*********************************************************************
* @purpose  Prints the states of GVRP attributes on the port.
*
* @param    L7_ushort16  no_of_attr_to_print
* @param    L7_ushort16  intIfNum
*
* @returns  void
*
* @notes    Prints the states of both Registrars and Applicants of
*           no_of_attr_to_print GVRP attributes registered on port
*           port_no.
*
*           Called by user (developer) like
*
*               devshel garpDebugGvrpStateInfoDump(5, 3)
*
*
*           The output might look like:
*
*   > Port 3:
*   >    Attrib  1: A= Qa, R=Inr, VID=    1, incoming_port=0
*   >    Attrib  2: A= Ao, R=Inn, VID=    4, incoming_port=1
*   >    Attrib  3: A= Ao, R=Inn, VID=   16, incoming_port=1
*   >    Attrib  4: A= Ao, R=Inn, VID=   64, incoming_port=1
*   >    Attrib  5: A= Vo, R= Mt, VID=65535, incoming_port=0
*
* @end
*********************************************************************/
void garpDebugGvrpStateInfo(L7_ushort16 no_of_attr_to_print, L7_ushort16 intIfNum)
{
    gvrp_state_info(no_of_attr_to_print, intIfNum);
}



/*********************************************************************
* @purpose  Prints the states of GMRP attributes on the port.
*
* @param    L7_ushort16  no_of_attr_to_print
* @param    L7_ushort16  intIfNum
*           Vlan_id      vlan_id
*
* @returns  void
*
* @notes    Prints the states of both Registrars and Applicants of
*           no_of_attr_to_print GMRP attributes registered on port
*           port_no.
*
*           Called by user (developer) like
*
*               dev garpDebugGmrpStateInfoDump(3,11,3)
*
*
*           The output might look like:
*
*
*   > Port 11:
*   >    Attrib  1: A= Lo, R= Mt, MAC=00-00-00-00-00-00, VID=    1, incoming_port=0, rcv_join=0
*   >    Attrib  2: A= Lo, R= Mt, MAC=00-00-00-00-00-00, VID=    1, incoming_port=0, rcv_join=0
*   >    Attrib  3: A= Ao, R=Inn, MAC=01-02-03-04-05-01, VID=    1, incoming_port=1, rcv_join=1
*
* @end
*********************************************************************/
void garpDebugGmrpStateInfo(L7_ushort16 no_of_attr_to_print, L7_ushort16 intIfNum,
                            Vlan_id     vlan_id)
{
    gmrp_state_info(no_of_attr_to_print, intIfNum, vlan_id);
}



/*********************************************************************
* @purpose  Prints the state of GVRP Leave All Timer running
*           on the port.
*
* @param    L7_ushort32  port_no
*
* @returns  void
*
* @notes    Prints the fields of the GARP Leave All timer structure
*           running on the port;  also, prints the number of
*           interfaces used and the number of GVRP and GMRP Leave All
*           timers running on all the ports of the device.
*
*           Called by user (developer) like
*
*               devshel garpDebugLeaveAllTimerInfoDump(24)
*
*
*           The output might look like:
*
*           > Fields of garpLeaveAllTimer[24]:
*           >             port_no = 24
*           >    leaveall_timeout = 60000 msec
*           >  gvrp_leaveall_left = 82250 msec
*           >        gvrp_enabled = 1
*           >        gmrp_enabled = 1
*           >      gmrp_instances = 5
*           >
*           >  Interfaces/Applications:
*           >     interfaces_used = 24
*           >           gvrp_used = 24
*           >           gmrp_used = 3
*
*           It means that on port 24, Leave All timeout value specified
*           by user is 60000msec (6000csec), GVRP Leave All timer
*           (gvrp_leaveall_left) will expire in 82250msec,
*           both GVRP (gvrp_enabled) and GMRP (gmrp_enabled)
*           are enabled.  There are 5 GMRP instances running on the port
*           (gmrp_instances = 5).
*           The number of ports used (interfaces_used) is 24, and GVRP
*           Leave All timers (gvrp_used) are running on all 24 ports,
*           whereas GMRP Leave All timers (gmrp_used) are only running
*           on 3 ports.
*
* @end
*********************************************************************/
void garpDebugGvrpLeaveAllTimerInfo(L7_uint32 intIfNum)
{
   garp_timer_info(intIfNum);
}



/*********************************************************************
* @purpose  Prints the state of GMRP Leave All Timers running
*           for all instances of GMRP on the given port.  Similar to
*           to garp_timer_info().
*
* @param    L7_ushort32  intIfNum
*
* @returns  void
*
* @end
*********************************************************************/
void garpDebugGmrpLeaveAllTimerInfo(L7_uint32 intIfNum)
{
    gmrp_timer_info( intIfNum);
}


/*********************************************************************
* @purpose  Prints the number of records in GVD database.
*
* @returns  void
*
* @notes    Shows how many records are currently contained in
*           GVD database.
*
*           Called by user (developer) like
*
*               devshel gvd_records_info
*
*           The output might look like:
*
*   >
*   >    GVD numberOfEntries=15
*   >
*
* @end
*********************************************************************/
void garpDebugGvdRecordsInfo()
{
  gvd_records_info();
}

#if 0
/*********************************************************************
* @purpose  Print all of the GMRP entries in the GMRP database
*
* @returns  void
*
* @notes
*
*
*
* @end
*********************************************************************/
void garpDebugGmrpEntriesShow(void)
{

    L7_uint32 index;
    Gmr       *my_gmr;
    l7_dll_t  *my_gmd;
    L7_dll_member_t *my_entry;

    my_entry = L7_NULL;

    for (index = 0; index < L7_MAX_VLAN_ID; index++)
    {
        my_gmr = gmrpInstance[index].gmr;

        if (gmrpInstance[index].inuse != L7_TRUE)
            continue;


        my_gmr = gmrpInstance[index].gmr;
        my_gmd = gmrpInstance[index].gmr->gmd;

        sysapiPrintf("\n");
        sysapiPrintf("VLAN %d\n", index);
        sysapiPrintf("-------------\n", index);

        my_entry = (L7_dll_member_t *)my_gmd;
        if (my_entry != L7_NULL)
        {
          do
          {
            intIfNum = ((dot1dPortData_t*)(portData->data))->intIfNum;          /* @lvl7 - MSTP */
            printf("%u\r\n", intIfNum);

            memcpy((void *)&vlanId,(void *)key, 2);
            memcpy((void *)memInfo.macAddr,(void *)&key[2],L7_MAC_ADDR_LEN);


          }while (DLLNextGet(&my_entry) == L7_SUCCESS);
        }
        else
        {
          sysapiPrintf("No members!\r\n");
        }


        sysapiPrintf("\n");

    } /* index < L7_MAX_VLAN_ID */
}

#endif




/*********************************************************************
* @purpose  Display the number of messages in the GARP message queue.
*
* @param    @b{(input)}   void
*
* @returns  void
*
* @comments devshell command
*
* @end
*********************************************************************/
void garpDebugMsgQueueShow(void)
{
  L7_int32 num;


  sysapiPrintf ("\n");

  if (osapiMsgQueueGetNumMsgs(garpQueue, &num) == L7_SUCCESS)
  {
      sysapiPrintf("Number of messages  currently in in the garpQueue : %d\n", num);
  }
  sysapiPrintf("Number of messages  received on the garpQueue : %d\n", garpInfo.msgsRecvdOnQueue);
  sysapiPrintf("Number of times garpQueue usage has exceeded fifty percent : %d\n", garpInfo.queueUsageOverFiftyPercent);
  sysapiPrintf("Number of times garpQueue usage has exceeded eighty percent : %d\n", garpInfo.queueUsageOverEightyPercent);
  sysapiPrintf("Number of times garpQueue usage has exceeded ninety percent : %d\n", garpInfo.queueUsageOverNinetyPercent);


}

/*********************************************************************
* @purpose  Display the number of messages in the GARP message queue.
*
* @param    @b{(input)}   void
*
* @returns  void
*
* @comments devshell command
*
* @end
*********************************************************************/
void garpDebugMsgQueueCountsClear(void)
{
  if (osapiSemaTake(garpTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    garpInfo.msgsRecvdOnQueue = 0;
    (void)osapiSemaGive(garpTaskSyncSema);
  }
}




/*********************************************************************
* @purpose  Print GARP tracing modes
*
* @param      void
*
* @returns    void
*
* @notes
*
* @end
*********************************************************************/
void garpDebugTraceModePrint(void)
{
  if (garpCfg == L7_NULL)
  {
    return;
  }

  sysapiPrintf ("\n");

  if (garpCfg->traceFlags & GARP_TRACE_TO_LOG)
      sysapiPrintf("GARP:  tracing to log enabled\n");
  else
      sysapiPrintf("GARP:  tracing to log disabled\n");


  if (garpCfg->traceFlags & GARP_TRACE_TO_CONSOLE)
      sysapiPrintf("GARP:  tracing to console enabled\n");
  else
      sysapiPrintf("GARP:  tracing to console disabled\n");


  if (garpCfg->traceFlags & GARP_TRACE_INTERNAL)
      sysapiPrintf("GARP:  tracing internal paths enabled\n");
  else
      sysapiPrintf("GARP:  tracing internal paths disabled\n");


  if (garpCfg->traceFlags & GARP_TRACE_TIMERS)
      sysapiPrintf("GARP:  tracing timer path enabled\n");
  else
      sysapiPrintf("GARP:  tracing timer path disabled\n");


  if (garpCfg->traceFlags & GARP_TRACE_QUEUE)
      sysapiPrintf("GARP:  tracing messages to/from queue enabled\n");
  else
      sysapiPrintf("GARP:  tracing messages to/from queue disabled\n");


  if (garpCfg->traceFlags & GARP_TRACE_PDU)
      sysapiPrintf("GARP:  tracing PDUs enabled\n");
  else
      sysapiPrintf("GARP:  tracing PDUs disabled\n");


  if (garpCfg->traceFlags & GARP_TRACE_QUEUE_THRESH)
      sysapiPrintf("GARP:  message for queue threshold exceeded enabled\n");
  else
      sysapiPrintf("GARP:  message for queue threshold exceeded disabled\n");


  if (garpCfg->traceFlags & GARP_TRACE_PROTOCOL)
      sysapiPrintf("GARP:  message for Protocol events enabled\n");
  else
      sysapiPrintf("GARP:  message for Protocol events disabled\n");

  return;
}



/*********************************************************************
* @purpose  Set a debug trace mode.
*
* @param    mode        @b{(input)} debug flag setting
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t garpDebugTraceModeSet(L7_uint32 mode)
{

    if (mode == 0)
    {
        garpDebugTraceModeHelp();
        return L7_SUCCESS;
    }

    if (mode == 0xFFFFFFFF)
    {
        garpCfg->traceFlags = 0xFFFFFFFF;
        return L7_SUCCESS;
    }

    switch (mode)
    {
    case 1:
        garpCfg->traceFlags |= GARP_TRACE_TO_LOG;
        break;


    case 2:
        garpCfg->traceFlags |= GARP_TRACE_TO_CONSOLE;
        break;

    case 3:
        garpCfg->traceFlags |= GARP_TRACE_INTERNAL;
        break;

    case 4:
        garpCfg->traceFlags |= GARP_TRACE_TIMERS;
        break;

    case 5:
        garpCfg->traceFlags |= GARP_TRACE_QUEUE;
        break;

    case 6:
        garpCfg->traceFlags |= GARP_TRACE_PDU;
        break;

    case 7:
        garpCfg->traceFlags |= GARP_TRACE_QUEUE_THRESH;
        break;

    case 8:
        garpCfg->traceFlags |= GARP_TRACE_PROTOCOL;
        break;

    default:
        sysapiPrintf(" Unrecognized input\n");
        return L7_FAILURE;
        break;
    }

    return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Clear a debug trace mode.
*
* @param    mode        @b{(input)} debug flag setting
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t garpDebugTraceModeClear(L7_uint32 mode)
{

    if (mode == 0)
    {
        garpDebugTraceModeHelp();
        return L7_SUCCESS;
    }

    if (mode == 0xFFFFFFFF)
    {
        garpCfg->traceFlags = 0;
        return L7_SUCCESS;
    }

    switch (mode)
    {
    case 1:
        garpCfg->traceFlags &= (~GARP_TRACE_TO_LOG);
        break;


    case 2:
        garpCfg->traceFlags &= (~GARP_TRACE_TO_CONSOLE);
        break;

    case 3:
        garpCfg->traceFlags &= (~GARP_TRACE_INTERNAL);
        break;

    case 4:
        garpCfg->traceFlags &= (~GARP_TRACE_TIMERS);
        break;

    case 5:
        garpCfg->traceFlags &= (~GARP_TRACE_QUEUE);
        break;

    case 6:
        garpCfg->traceFlags &= (~GARP_TRACE_PDU);
        break;

    case 7:
        garpCfg->traceFlags &= (~GARP_TRACE_QUEUE_THRESH);
        break;

    case 8:
        garpCfg->traceFlags &= (~GARP_TRACE_PROTOCOL);
        break;

    default:
        sysapiPrintf(" Unrecognized input\n");
        return L7_FAILURE;
        break;
    }

    return L7_SUCCESS;
}



/*====================================================================*/
/*====================================================================*/
/*====================================================================*/
/*====================================================================*/
/*
            Internal Debug  Routines:
                either static or invoked internally

*/
/*====================================================================*/
/*====================================================================*/
/*====================================================================*/


/*********************************************************************
* @purpose  Prints the states of GVRP attributes on the port.
*
* @param    L7_ushort16  no_of_attr_to_print
* @param    L7_ushort16  port_no
*
* @returns  void
*
* @notes    Prints the states of both Registrars and Applicants of
*           no_of_attr_to_print GVRP attributes registered on port
*           port_no.
*
*           Called by user (developer) like
*
*               devshel gvrp_state_info(5, 3)
*
*
*           The output might look like:
*
*   > Port 3:
*   >    Attrib  1: A= Qa, R=Inr, VID=    1, incoming_port=0
*   >    Attrib  2: A= Ao, R=Inn, VID=    4, incoming_port=1
*   >    Attrib  3: A= Ao, R=Inn, VID=   16, incoming_port=1
*   >    Attrib  4: A= Ao, R=Inn, VID=   64, incoming_port=1
*   >    Attrib  5: A= Vo, R= Mt, VID=65535, incoming_port=0
*
* @end
*********************************************************************/
void gvrp_state_info(L7_ushort16 no_of_attr_to_print, L7_ushort16 port_no)
{
    Gid         *my_port;
    Garp        *application;
    L7_ushort16 i;
    Vlan_id     vlan_id;


   /* previously took task synchronization semaphore at this point . removed */
   application = &gvrp_app->g;

    if (gid_find_port(application->gid, port_no, (void*)&my_port))
    {
        if ( no_of_attr_to_print > (application->max_gid_index + 2) )
        {
            sysapiPrintf("\nYou can only print up to %u attributes.\n",
                         (application->max_gid_index + 2));
        }
        else
        {
            sysapiPrintf("\nPort %d:\n", my_port->port_no);

            for (i=0; i<no_of_attr_to_print; i++)
            {
                /* get the value of attribute i */
                gvd_get_key(i, &vlan_id);

                sysapiPrintf(
                    "   Attrib %4d: A=%3s, R=%3s, VID=%5u, incoming_port=%d\n",
                    (i+1),
                    GetApplicantState[my_port->machines[i].applicant],
                    GetRegisrarState[my_port->machines[i].registrar],
                    vlan_id,
                    my_port->machines[i].incoming_port);
            }
            /* for (i=0; i<no_of_attr_to_print; i++) */
        }
    }
    else
    {
        sysapiPrintf("\n\nPort %d not enabled", port_no);
    }

   return;
}

/*********************************************************************
* @purpose  Prints the states of GMRP attributes on the port.
*
* @param    L7_ushort16  no_of_attr_to_print
* @param    L7_ushort16  port_no
*           Vlan_id      vlan_id
*
* @returns  void
*
* @notes    Prints the states of both Registrars and Applicants of
*           no_of_attr_to_print GMRP attributes registered on port
*           port_no.
*
*           Called by user (developer) like
*
*               dev gmrp_state_info(3,11
*
*
*           The output might look like:
*
*
*   > Port 11:
*   >    Attrib  1: A= Lo, R= Mt, MAC=00-00-00-00-00-00, VID=    1, incoming_port=0, rcv_join=0
*   >    Attrib  2: A= Lo, R= Mt, MAC=00-00-00-00-00-00, VID=    1, incoming_port=0, rcv_join=0
*   >    Attrib  3: A= Ao, R=Inn, MAC=01-02-03-04-05-01, VID=    1, incoming_port=1, rcv_join=1
*
* @end
*********************************************************************/
void gmrp_state_info(L7_ushort16 no_of_attr_to_print,
                     L7_ushort16 port_no,
                     Vlan_id     vlan_id)
{
    Gid         *my_port, *this_port;
    L7_ushort16 i, j;
    L7_uint32   vlanId, nextvlanId, index;
    Gmr         *my_gmr;
    L7_uchar8   macAddr[L7_MAC_ADDR_LEN];
    L7_char8    buf[18];


    nextvlanId = 0;
    index      = 0;
    my_gmr     = (Gmr *)L7_NULL;
    vlanId     = vlan_id;

    if (L7_FALSE == garpIsValidIntf(port_no))
    {
        sysapiPrintf("\n\nPort %d not valid", port_no);
        return;
    }


    while (L7_SUCCESS == dot1qNextVlanGet(vlanId, &nextvlanId))
    {
        if (L7_SUCCESS == gmrpInstanceIndexFind(nextvlanId, &index))
        {
            my_gmr    = gmrpInstance[index].gmr;
            my_port   = my_gmr->g.gid;
            this_port = my_port;

            if ( no_of_attr_to_print > (my_gmr->g.max_gid_index + 2) )
            {
                sysapiPrintf("\nYou can only print up to %u attributes.\n",
                             (my_gmr->g.max_gid_index + 2));
                break;
            }

            if (L7_NULL != (Gid *)my_port)
            {
                do
                {
                    if (my_port->port_no == port_no)
                    {
                        sysapiPrintf("\nPort %d, VID %u:\n",
                                     my_port->port_no,
                                     my_port->application->vlan_id);

                        for (i=0; i<no_of_attr_to_print; i++)
                        {
                            bzero(macAddr, sizeof(macAddr));
                            bzero(buf,     sizeof(buf));

                            if (i >= Number_of_legacy_controls)
                            {
                                j = i - Number_of_legacy_controls;
                                gmd_get_key((void *)my_gmr->gmd, j, macAddr);
                            }

                            sprintf(buf,"%02x-%02x-%02x-%02x-%02x-%02x",
                                    macAddr[0], macAddr[1], macAddr[2],
                                    macAddr[3], macAddr[4], macAddr[5]);

                            sysapiPrintf(
                                "  Attrib %4d: A=%3s, R=%3s, MAC=%s, incoming_port=%d\n",
                                (i+1),
                                GetApplicantState[my_port->machines[i].applicant],
                                GetRegisrarState[my_port->machines[i].registrar],
                                &buf,
                                my_port->machines[i].incoming_port);
                        }
                        /* for (i=0; i<no_of_attr_to_print; i++) */
                    }

                    my_port = my_port->next_in_port_ring;

                } while (this_port != my_port);
            }
        }

        vlanId = nextvlanId;
    }

   return;
}

/*********************************************************************
* @purpose  Prints the state of GVRP Leave All Timer running
*           on the port.
*
* @param    L7_ushort32  port_no
*
* @returns  void
*
* @notes    Prints the fields of the GARP Leave All timer structure
*           running on the port;  also, prints the number of
*           interfaces used and the number of GVRP and GMRP Leave All
*           timers running on all the ports of the device.
*
*           Called by user (developer) like
*
*               devshel garp_timer_info(24)
*
*
*           The output might look like:
*
*           > Fields of garpLeaveAllTimer[24]:
*           >             port_no = 24
*           >    leaveall_timeout = 60000 msec
*           >  gvrp_leaveall_left = 82250 msec
*           >        gvrp_enabled = 1
*           >        gmrp_enabled = 1
*           >      gmrp_instances = 5
*           >
*           >  Interfaces/Applications:
*           >     interfaces_used = 24
*           >           gvrp_used = 24
*           >           gmrp_used = 3
*
*           It means that on port 24, Leave All timeout value specified
*           by user is 60000msec (6000csec), GVRP Leave All timer
*           (gvrp_leaveall_left) will expire in 82250msec,
*           both GVRP (gvrp_enabled) and GMRP (gmrp_enabled)
*           are enabled.  There are 5 GMRP instances running on the port
*           (gmrp_instances = 5).
*           The number of ports used (interfaces_used) is 24, and GVRP
*           Leave All timers (gvrp_used) are running on all 24 ports,
*           whereas GMRP Leave All timers (gmrp_used) are only running
*           on 3 ports.
*
* @end
*********************************************************************/
void garp_timer_info(L7_uint32 port_no)
{

    if (L7_TRUE == garpIsValidIntf(port_no))
    {
        garpPrintTimerInfo(port_no);
    }
    else
    {
        sysapiPrintf("\n   Port %u doesn't exist. The existing ports are <1-%u>.\n",
                     port_no, GARP_INTF_MAX_COUNT);
    }

   return;
}

/*********************************************************************
* @purpose  Prints the state of GMRP Leave All Timers running
*           for all instances of GMRP on the given port.  Similar to
*           to garp_timer_info().
*
* @param    L7_ushort32  port_no
*
* @returns  void
*
* @end
*********************************************************************/
void gmrp_timer_info(L7_uint32 port_no)
{
    L7_uint32 vlanId, nextvlanId;

    if (L7_TRUE == garpIsValidIntf(port_no))
    {
        vlanId = 0;
        while (L7_SUCCESS== dot1qNextVlanGet(vlanId, &nextvlanId))
        {
            gmrpPrintTimerInfo(port_no, nextvlanId);

            vlanId = nextvlanId;
        } /* dot1qNextVlanGet */
    }
    else
    {
        sysapiPrintf("\n   Port %u doesn't exist. The existing ports are <1-%u>.\n",
                     port_no, GARP_INTF_MAX_COUNT);
    }

    return;
}


/*********************************************************************
* @purpose  Prints the number of records in GVD database.
*
* @returns  void
*
* @notes    Shows how many records are currently contained in
*           GVD database.
*
*           Called by user (developer) like
*
*               devshel gvd_records_info
*
*           The output might look like:
*
*   >
*   >    GVD numberOfEntries=15
*   >
*
* @end
*********************************************************************/
void gvd_records_info()
{
    sysapiPrintf("\n    GVD numberOfEntries=%u\n", getNumberOfGVDEntries());
}




/*********************************************************************
* @purpose  Print help for trace mdoes
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void garpDebugTraceModeHelp(void)
{


    sysapiPrintf ("\n");

    sysapiPrintf("Tracing occurs only if tracing to log or console is enabled\n");
    sysapiPrintf("      \n");
    sysapiPrintf("Tracing Modes:    \n");
    sysapiPrintf("      1:    Enables or disables tracing to log\n");
    sysapiPrintf("      2:    Enables or disables tracing to console\n");
    sysapiPrintf("      \n");
    sysapiPrintf("      3:    Enables or disables tracing misc. internal paths\n");
    sysapiPrintf("      4:    Enables or disables tracing timer path\n");
    sysapiPrintf("      5:    Enables or disables tracing messages to/from queue\n");
    sysapiPrintf("      6:    Enables or disables tracing PDUs\n");
    sysapiPrintf("      7:    Enables or disables tracing Queue Threshold exceeded\n");
    sysapiPrintf("      8:    Enables or disables tracing Protocol events\n");
    sysapiPrintf("      0xFFFFFFFF: Enables all tracing modes\n");
    sysapiPrintf("      \n");

    sysapiPrintf("Example usages:\n");
    sysapiPrintf("      \n");
    sysapiPrintf("    To enable logging to the console, input\n");
    sysapiPrintf("       devshell garpDebugTraceModeSet(1)");


    sysapiPrintf("      \n");
    sysapiPrintf("    To disable logging timer events, input\n");
    sysapiPrintf("       devshell garpDebugTraceModeClear(4)");

}

/*********************************************************************
* @purpose  Check if a particular GARP tracing mode is set
*
* @param      void
*
* @returns    void
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL garpDebugTraceModeGet(L7_uint32 traceFlag)
{
  L7_BOOL mode;

  if (garpCfg == L7_NULL)
  {
    return(L7_FAILURE);
  }

  mode = L7_FALSE;

   switch (traceFlag)
    {
       case GARP_TRACE_TO_LOG:
          if (garpCfg->traceFlags & GARP_TRACE_TO_LOG)
              mode = L7_TRUE;
          break;
       case GARP_TRACE_TO_CONSOLE:
           if (garpCfg->traceFlags & GARP_TRACE_TO_CONSOLE)
               mode = L7_TRUE;
           break;
       case GARP_TRACE_QUEUE:
           if (garpCfg->traceFlags & GARP_TRACE_QUEUE)
               mode = L7_TRUE;
           break;
       case GARP_TRACE_TIMERS:
           if (garpCfg->traceFlags & GARP_TRACE_TIMERS)
               mode = L7_TRUE;
           break;
       case GARP_TRACE_INTERNAL:
           if (garpCfg->traceFlags & GARP_TRACE_INTERNAL)
               mode = L7_TRUE;
           break;
       case GARP_TRACE_PDU:
           if (garpCfg->traceFlags & GARP_TRACE_PDU)
               mode = L7_TRUE;
           break;

       case GARP_TRACE_QUEUE_THRESH:
           if (garpCfg->traceFlags & GARP_TRACE_QUEUE_THRESH)
               mode = L7_TRUE;
           break;

       default:
           break;

   }

   return mode;


}





/*====================================================================*/
/*====================================================================*/
/*====================================================================*/
/*====================================================================*/
/*
            Internal Debug Routines

*/
/*====================================================================*/
/*====================================================================*/
/*====================================================================*/

/*********************************************************************
* @purpose  Write a debug trace message.
*
* @param    traceMsg    @b{(input)} A string to be displayed as a trace message.
* @param    format      @b{(input)} format string
* @param    ... @b{(input)} additional arguments (per format string)
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void garpTraceWrite(L7_uchar8 *traceMsg)
{
    L7_uint32 msecSinceBoot;
    L7_uint32 secSinceBoot;
    L7_uint32 msecs;
    L7_uchar8 debugMsg[300 + 1];

    msecSinceBoot = osapiTimeMillisecondsGet();
    secSinceBoot = msecSinceBoot / 1000;
    msecs = msecSinceBoot % 1000;


    sysapiPrintf("\n");
    /* For now, just print the message with a timestamp. */
    sprintf(debugMsg, "GARP: %d.%03d sec:  ", secSinceBoot, msecs);
    strncat(debugMsg, traceMsg, min(L7_LOG_FORMAT_BUF_SIZE, 300) );
    sysapiPrintf(debugMsg);
}


/*********************************************************************
* @purpose  Record a GARP event trace if trace configuration permits
*
* @param    event_flag  @b{(input)} one of GARP_TRACE_* flags allowed in config
* @param    format      @b{(input)} format string
* @param    ...         @b{(input)} additional arguments (per format string)
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void garpTraceEvents(L7_uint32 event_flag, L7_char8 * format, ...)
{
    L7_char8 buf[L7_LOG_FORMAT_BUF_SIZE];
    va_list ap;

    va_start(ap, format);
    (void)vsprintf(buf, format, ap);
    va_end(ap);


    if ( garpCfg->traceFlags & event_flag)
    {
        garpTraceMsgs(buf);

    }

}


/*********************************************************************
* @purpose  Trace GARP events
*
* @param    format      @b{(input)} format string
* @param    ... @b{(input)} additional arguments (per format string)
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void garpTraceMsgs(L7_uchar8 *traceMsg)
{

    if ( garpDebugTraceModeGet(GARP_TRACE_TO_LOG) == L7_TRUE )
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_GARP_COMPONENT_ID,traceMsg);
    }

    if (garpDebugTraceModeGet(GARP_TRACE_TO_CONSOLE)  == L7_TRUE )
    {
        garpTraceWrite( traceMsg);

    }


}


/*********************************************************************
*
* @purpose  Trace message sent on garp queue
*
* @param    *msg    @b{(input)} pointer to GarpPacket
*
*
* @returns  void
*
* @notes
*
*
* @end
*********************************************************************/
void garpTraceQueueMsgSend(GarpPacket *msg)
{
    garpTraceQueueMsgFormat(msg, 0);
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
void garpTraceQueueMsgRecv(GarpPacket *msg)
{
    garpTraceQueueMsgFormat(msg, 1);
}


/*********************************************************************
*
* @purpose  Format and trace message sent or received on GARP queue
*
* @param    *msg        @b{(input)} pointer to GarpPacket message
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
static void garpTraceQueueMsgFormat(GarpPacket *msg, L7_uint32 direction)
{
    L7_uchar8   traceDirection[22];
    L7_uchar8   traceCommandName[50];
    L7_uchar8   traceGarpApp[15];
    GARPCommand *cmd;
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

    /* Determine the direction */

    if (direction == L7_NULL)
    {
        sprintf(traceDirection, "GARP Task Msg Sent");
    }
    else
    {
        sprintf(traceDirection, "GARP Task Msg Recv");
    }

    /* Format and log the event */


    switch (msg->msgId)
    {
      case G_COMMAND:

          cmd = &(msg->msgData.command);
          garpTraceProcessCommandNameFormat(cmd, traceCommandName);
          garpTraceProcessCommandAppNameFormat(cmd,traceGarpApp);

          GARP_TRACE_QUEUE_EVENTS("%s: %s: %s: data = %d, port = %d\n",
                                     traceDirection, traceCommandName, traceGarpApp,
                                      *(L7_ushort16 *)&cmd->data, cmd->port);

        break;

      case G_PORT_CONN:
        /* GARP connect port (span)*/
          nimGetIntfName(msg->port, L7_SYSNAME, ifName);
          GARP_TRACE_QUEUE_EVENTS("%s: G_PORT_CONN: port = %d, %s\n",
                           traceDirection, msg->port, ifName);
        break;

      case G_PORT_DISCONN:
          nimGetIntfName(msg->port, L7_SYSNAME, ifName);
          GARP_TRACE_QUEUE_EVENTS("%s: G_PORT_DISCONN: port = %d, %s\n",
                           traceDirection, msg->port, ifName);
        break;


      case G_TIMER:
        GARP_TRACE_QUEUE_EVENTS("%s: G_TIMER: timer interval expiry\n",traceDirection);
        break;


      case G_LEAVEALL_TIMER:
          GARP_TRACE_QUEUE_EVENTS("%s: G_LEAVEALL_TIMER: timer interval expiry\n",traceDirection);
          break;


      case G_INTF_STATE_CHANGE:
        nimGetIntfName(msg->port, L7_SYSNAME, ifName);
        GARP_TRACE_QUEUE_EVENTS("%s: G_INTF_STATE_CHANGE: port = %d, %s, event= %d, correlator =%x\n",
                                traceDirection, msg->port, ifName, msg->msgData.nim.nim_event,
                                msg->msgData.nim.nim_correlator);
        break;

      case G_NIM_STARTUP:
        GARP_TRACE_QUEUE_EVENTS("%s: G_NIM_STARTUP: startupPhase =%x\n",
                                  traceDirection, msg->msgData.startup.startupPhase);
        break;

      case G_CNFGR:
        GARP_TRACE_QUEUE_EVENTS("%s: G_CNFGR: command = %d\n",
                         traceDirection, msg->msgData.CmdData.command);
        break;

      case G_VLAN_NOTIFY:
        nimGetIntfName(msg->port, L7_SYSNAME, ifName);
        GARP_TRACE_QUEUE_EVENTS("%s: G_VLAN_NOTIFY: intf = %d, %s, event = %d , \n",
                            traceDirection, msg->port, ifName,
                            msg->msgData.dot1q.event);
        break;


      case G_DOT1S_NOTIFY:
        nimGetIntfName(msg->port, L7_SYSNAME, ifName);
        GARP_TRACE_QUEUE_EVENTS("%s: G_DOT1S_NOTIFY: mstId = %d, intf = %d, %s, event = %d\n",
                                traceDirection, msg->msgData.dot1s.mstID, msg->port, ifName,
                                msg->msgData.dot1s.event);
        break;


     default:
        GARP_TRACE_QUEUE_EVENTS("%s: Unknown Event %d received \n", traceDirection,msg->msgId);
        break;

    }

    return;
}



/*********************************************************************
*
* @purpose  Format and trace commands sent to process_command
*
* @param    *msg        @b{(input)} pointer to GARPCommand message
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
void garpTraceProcessCommand(GARPCommand *cmd)
{
    L7_uchar8   traceCommandName[50];
    L7_uchar8   traceGarpApp[15];
    L7_uchar8   traceGmrpMacAddr[18];

    garpTraceProcessCommandNameFormat(cmd, traceCommandName);


    garpTraceProcessCommandAppNameFormat(cmd,traceGarpApp);


    /* Format and log the event */

    /* begin processing of a GARPCommandNameTable type command */
    switch (cmd->command)
    {
      case GARP_JOIN:
      case GARP_LEAVE:
      case GARP_DELETE_ATTRIBUTE:
      case GARP_NORMAL_PARTICIPANT:
      case GARP_NON_PARTICIPANT:
      case GARP_NORMAL_REGISTRATION:
      case GARP_REGISTRATION_FIXED:
      case GARP_REGISTRATION_FORBIDDEN:

        if (cmd->app == GARP_GVRP_APP)
        {
            GARP_TRACE_INTERNAL_EVENTS("process_command: %s: %s: data = %d, port = %d\n",
                                       traceCommandName, traceGarpApp,
                                        *(L7_ushort16 *)&cmd->data, cmd->port);
        }
        else if (cmd->app == GARP_GMRP_APP)
        {
            sprintf(traceGmrpMacAddr, "%02x%02x%02x%02x%02x%02x",
                    cmd->data[0],
                    cmd->data[1],
                    cmd->data[2],
                    cmd->data[3],
                    cmd->data[4],
                    cmd->data[5]);


          if(cmd->vlan_id != L7_NULL)
          {
              GARP_TRACE_INTERNAL_EVENTS("process_command: %s: %s: vlan = %d, port = %d, macAddr = %s, flags = %d\n",
                                     traceCommandName, traceGarpApp, cmd->vlan_id, cmd->port,
                                     traceGmrpMacAddr, cmd->flags);
          }
          else
          {
            /* this command needs to run on every instance of gmrp */
              GARP_TRACE_INTERNAL_EVENTS("process_command: %s: %s: vlan = ALL, port = %d, macAddr = %s, flags = %d\n",
                                         traceCommandName, traceGarpApp, cmd->port,
                                         traceGmrpMacAddr, cmd->flags);
            }
          }
        break;


     case GARP_ENABLE_PORT:
     case GARP_DISABLE_PORT:

        GARP_TRACE_INTERNAL_EVENTS("process_command: %s: %s: vlan = %d, port = %d\n",
                                   traceCommandName, traceGarpApp,
                                   *(L7_ushort16 *)&cmd->data, cmd->port);
        break;

      case GARP_ENABLE:

        GARP_TRACE_INTERNAL_EVENTS("process_command: %s: %s: \n", traceCommandName, traceGarpApp);
        break;

      case GARP_DISABLE:
        GARP_TRACE_INTERNAL_EVENTS("process_command: %s: %s: \n", traceCommandName, traceGarpApp);

        break;

      case GARP_JOIN_TIME:
        GARP_TRACE_INTERNAL_EVENTS("process_command: %s: %s: port = %d, join_time =%d\n",
                                   traceCommandName, traceGarpApp, cmd->port,
                                   *(L7_uint32 *)&cmd->data);

        break;

      case GARP_LEAVE_TIME:
        GARP_TRACE_INTERNAL_EVENTS("process_command: %s: %s: port = %d, leave_time =%d\n",
                                   traceCommandName, traceGarpApp, cmd->port,
                                   *(L7_uint32 *)&cmd->data);
        break;

      case GARP_LEAVEALL_TIME:
        GARP_TRACE_INTERNAL_EVENTS("process_command: %s: %s: port = %d, leaveall_time =%d\n",
                                   traceCommandName, traceGarpApp, cmd->port,
                                   *(L7_uint32 *)&cmd->data);
        break;

      default:
        GARP_TRACE_INTERNAL_EVENTS("process_command: %s: Unknown Event %d received \n", traceCommandName,
                                   cmd->command);
        break;

    } /* switch(cmd) */


    return;
}




/*********************************************************************
*
* @purpose  Format and trace commands sent to gvr_proc_command
*
* @param    *msg        @b{(input)} pointer to GARPCommand message
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
void garpTraceProcessGvrCommand(GARPCommand *cmd)
{
    L7_uchar8   traceCommandName[50];

    garpTraceProcessCommandNameFormat(cmd, traceCommandName);


    GARP_TRACE_INTERNAL_EVENTS("gvr_proc_command: %s: vlan = %d, port = %d\n",
                        traceCommandName, (Vlan_id)*(Vlan_id *)&cmd->data, cmd->port);

    return;
}


/*********************************************************************
*
* @purpose  Format and trace commands sent to gmr_proc_command
*
* @param    *msg        @b{(input)} pointer to GARPCommand message
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
void garpTraceProcessGmrCommand(GARPCommand *cmd)
{
    L7_uchar8   traceCommandName[30];
    L7_uchar8   traceGmrpMacAddr[18];

    garpTraceProcessCommandNameFormat(cmd, traceCommandName);

    sprintf(traceGmrpMacAddr, "%02x%02x%02x%02x%02x%02x",
            cmd->data[0],
            cmd->data[1],
            cmd->data[2],
            cmd->data[3],
            cmd->data[4],
            cmd->data[5]);

    if ( (cmd->command == GARP_ENABLE_PORT) || (cmd->command == GARP_DISABLE_PORT) )
    {
        GARP_TRACE_INTERNAL_EVENTS("gmr_proc_command: %s: vlan = %d, port = %d, flags = %d\n",
                                   traceCommandName, cmd->vlan_id, cmd->port, cmd->flags);
    }
    else
    {
        GARP_TRACE_INTERNAL_EVENTS("gmr_proc_command: %s: vlan = %d, port = %d, macAddr = %sn, flags = %d\n",
                                   traceCommandName, cmd->vlan_id, cmd->port, traceGmrpMacAddr, cmd->flags);
    }



    return;
}

/*********************************************************************
*
* @purpose  Format name of GARP command
*
* @param    *cmd                @b{(input)} pointer to GARPCommand
*
* @param    *traceCommandName   @b{(input)} pointer to string to contain name
*
* @returns  void
*
*
* @notes
*
*
* @end
*********************************************************************/
void garpTraceProcessCommandNameFormat(GARPCommand *cmd, L7_uchar8 *traceCommandName)
{

    /* Get command name */
    switch (cmd->command)
    {
      case GARP_JOIN:
          sprintf(traceCommandName, "GARP_JOIN");
          break;
      case GARP_LEAVE:
          sprintf(traceCommandName, "GARP_LEAVE");
          break;
      case GARP_DELETE_ATTRIBUTE:
          sprintf(traceCommandName, "GARP_DELETE_ATTRIBUTE");
          break;
      case GARP_NORMAL_PARTICIPANT:
          sprintf(traceCommandName, "GARP_NORMAL_PARTICIPANT");
          break;
      case GARP_NON_PARTICIPANT:
          sprintf(traceCommandName, "GARP_NON_PARTICIPANT");
          break;
      case GARP_NORMAL_REGISTRATION:
          sprintf(traceCommandName, "GARP_NORMAL_REGISTRATION");
          break;
      case GARP_REGISTRATION_FIXED:
          sprintf(traceCommandName, "GARP_REGISTRATION_FIXED");
          break;
      case GARP_REGISTRATION_FORBIDDEN:
          sprintf(traceCommandName, "GARP_REGISTRATION_FORBIDDEN");
          break;
      case GARP_ENABLE_PORT:
          sprintf(traceCommandName, "GARP_ENABLE_PORT");
          break;
      case GARP_DISABLE_PORT:
          sprintf(traceCommandName, "GARP_DISABLE_PORT");
          break;
      case GARP_ENABLE:
          sprintf(traceCommandName, "GARP_ENABLE");
          break;
      case GARP_DISABLE:
          sprintf(traceCommandName, "GARP_DISABLE");
          break;
      case GARP_JOIN_TIME:
          sprintf(traceCommandName, "GARP_JOIN_TIME");
          break;
      case GARP_LEAVE_TIME:
          sprintf(traceCommandName, "GARP_LEAVE_TIME");
          break;
      case GARP_LEAVEALL_TIME:
          sprintf(traceCommandName, "GARP_LEAVEALL_TIME");
          break;
      default:
          sprintf(traceCommandName, "GARP_UNDEFINED_COMMAND");
          break;

    } /* switch(cmd) */


    return;
}



/*********************************************************************
*
* @purpose  Format name of GARP Application
*
* @param    *cmd                @b{(input)} pointer to GARPCommand
*
* @param    *traceGarpApp   @b{(input)} pointer to string to contain name
*
* @returns  void
*
*
* @notes
*
*
* @end
*********************************************************************/
void garpTraceProcessCommandAppNameFormat(GARPCommand *cmd, L7_uchar8 *traceGarpApp)
{

    /* Get command name */
    switch (cmd->app)
    {
      case GARP_GVRP_APP:
          sprintf(traceGarpApp, "GARP_GVRP_APP");
          break;
      case GARP_GMRP_APP:
          sprintf(traceGarpApp, "GARP_GMRP_APP");
          break;
      default:
          sprintf(traceGarpApp, "UNDEFINED_GARP_APP");
          break;

    } /* switch(cmd) */


    return;
}

/*********************************************************************
* @purpose  Prints the list of pending or popped timers.
*
* @param    timers           pointer to the head of timer queue to be printed
*           timers_to_print  what timers to print:
*                                0 - all timers (default)
*                                1 - GVRP timers
*                                2 - GMRP timers
*           no_of_timers_to_print how many timers to print
*                                0 - all timers (default)
*                                n - some positive number
* @returns  void
*
* @notes    Prints all (by default) or certain number (as specified by
*           argument 3) of either both GVRP and GMRP pending timers, or
*           GVRP timers only, or GMRP timers only (as specified by
*           argument 2).
*
* @end
*********************************************************************/
void garpDebugPrintTimersInfo(void  **timers,
                              garpTimersToPrint timers_to_print,
                              L7_uint32 no_of_timers_to_print)
{
    L7_uint32 printedTimers = 0, currentTime = 0;
    garpTimerDescr_t *timersList;

    if (no_of_timers_to_print <=0 )
    {
      sysapiPrintf("\n Must specify number of timers to print.");
      return;
    }
    timersList = (garpTimerDescr_t *)*timers;

    /*
     *                         0 - both GVRP and GMRP timers
     *       timers_to_print:  1 - GVRP timers only
     *                         2 - GMRP timers only
     *
     * no_of_timers_to_print:  0 - all timers
     *                         n - positive number
     *
     */

    if ((garpTimerDescr_t *)L7_NULL == timersList)
    {
        sysapiPrintf("\n  Queue is empty.\n");
        return;
    }


    while (L7_NULL != timersList)
    {
        if (((GARP_GVRP == timers_to_print) &&
            ((GARP_TIMER_GVRP_JOIN  != timersList->timerType) &&
            (GARP_TIMER_GVRP_LEAVE != timersList->timerType)))
            ||
            ((GARP_GMRP == timers_to_print) &&
            (GARP_TIMER_GMRP_JOIN  != timersList->timerType) &&
            (GARP_TIMER_GMRP_LEAVE != timersList->timerType)))
        {
            /* we are printing either GVRP or GMRP timers, but
             * the current timer is not what we want;
             * move to the next timer
             */
            timersList = timersList->next;
            continue;
        }

        printedTimers++;

        sysapiPrintf("\n%4d %10s %5s Port=%3d VID=%4d ",
                     printedTimers,
                     GetTimerType[timersList->timerType],
                     GetTimerStatus[timersList->timer_status],
                     timersList->port_no,
                     timersList->vid);

        currentTime = osapiTimeMillisecondsGet();

        if (currentTime >= timersList->expiryTime)
            sysapiPrintf("Expired %i msecs ago", currentTime - timersList->expiryTime);
        else
            sysapiPrintf("Will Expire %i msecs", timersList->expiryTime - currentTime);

        if ((no_of_timers_to_print > 0) &&
            (no_of_timers_to_print == printedTimers))
        {
            break;
        }

        /* move to the next timer */
        timersList = timersList->next;
    }

    return;
}


/*********************************************************************
* @purpose  Prints the list of pending timers.
*
* @param    garpTimersToPrint timers_to_print
*                       0 - all timers (default)
*                       1 - GVRP timers
*                       2 - GMRP timers
*
* @param    L7_ushort32 no_of_timers_to_print
*                       0 - all timers (default)
*                       n - some positive number
* @returns  void
*
* @notes    Prints all (by default) or certain number (as specified by
*           argument 2) of either both GVRP and GMRP pending timers, or
*           GVRP timers only, or GMRP timers only (as specified by
*           argument 1).
*
* @end
*********************************************************************/
void garp_pending_info(garpTimersToPrint timers_to_print,
                       L7_uint32 no_of_timers_to_print)
{
    sysapiPrintf("Pending Timers Queue:\n");
    garpDebugPrintTimersInfo((void **)&(garpTimers.garpTimersPendingList),
                             timers_to_print,
                             no_of_timers_to_print);
    return;
}


/*********************************************************************
* @purpose  Prints the list of popped timers.
*
* @param    garpTimersToPrint timers_to_print
*                       0 - all timers (default)
*                       1 - GVRP timers
*                       2 - GMRP timers
*
* @param    L7_ushort32 no_of_timers_to_print
*                       0 - all timers (default)
*                       n - some positive number
* @returns  void
*
* @notes    Prints all (by default) or certain number (as specified by
*           argument 2) of either both GVRP and GMRP popped timers, or
*           GVRP timers only, or GMRP timers only (as specified by
*           argument 1).
*
* @end
*********************************************************************/
void garp_popped_info(garpTimersToPrint timers_to_print,
                      L7_uint32 no_of_timers_to_print)
{
    sysapiPrintf("Popped Timers Queue:\n");
    garpDebugPrintTimersInfo((void **)&(garpTimers.garpTimersPoppedList),
                             timers_to_print,
                             no_of_timers_to_print);
    return;
}


/*********************************************************************
* @purpose  Trace the percentage usage of GARP message queue.
*
* @param    @b{(input)}   void
*
* @returns  void
*
* @comments devshell command
*
* @end
*********************************************************************/
void garpTraceMsgQueueUsage(void)
{
  L7_int32 num;

  /* Count the number of times the number of messages in the garpQueue
     exceed certain thresholds.

    Allow, per configuration, a message to be displayed *once* if a
    threshold is exceeded */


  if (osapiMsgQueueGetNumMsgs(garpQueue, &num) == L7_SUCCESS)
  {
      if (num > GARP_QUEUE_USAGE_FIFTY_PERCENT)
      {
          garpInfo.queueUsageOverFiftyPercent++;

          if (garpQueueFiftyPercentLogged == L7_FALSE)
          {
              L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_GARP_COMPONENT_ID,
                  "garpQueue usage has exceeded fifty percent."
                  " Traces the build up of message queue. Helpful"
                  " in determining the load on GARP.");
              if (garpDebugTraceModeGet(GARP_TRACE_QUEUE_THRESH) == L7_TRUE )
              {
                  garpTraceWrite("garpQueue usage has exceeded fifty percent\n");
                  garpQueueFiftyPercentLogged = L7_TRUE;
              }
          }
      }

      if (num > GARP_QUEUE_USAGE_EIGHTY_PERCENT)
      {
          garpInfo.queueUsageOverEightyPercent++;

          if (garpQueueEightyPercentLogged == L7_FALSE)
          {

              L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_GARP_COMPONENT_ID,
                  "garpQueue usage has exceeded eighty percent."
                  " Traces the build up of message queue. Helpful"
                  " in determining the load on GARP.");
              if (garpDebugTraceModeGet(GARP_TRACE_QUEUE_THRESH) == L7_TRUE )
              {
                  garpTraceWrite("garpQueue usage has exceeded eighty percent\n");
                  garpQueueEightyPercentLogged = L7_TRUE;
              }
          }
      }

      if (num > GARP_QUEUE_USAGE_NINETY_PERCENT)
      {
          garpInfo.queueUsageOverNinetyPercent++;

          if (garpQueueNinetyPercentLogged == L7_FALSE)
          {

              L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_GARP_COMPONENT_ID,
                  "garpQueue usage has exceeded ninety percent."
                  " Traces the build up of message queue. Helpful"
                  " in determining the load on GARP.");
              if (garpDebugTraceModeGet(GARP_TRACE_QUEUE_THRESH) == L7_TRUE )
              {
                  garpTraceWrite("garpQueue usage has exceeded ninety percent\n");
                  garpQueueNinetyPercentLogged = L7_TRUE;
              }
          }
      }
  }

}

/*********************************************************************
* @purpose  Prints the states transitions for a all Vlans .
*
* @param    void
*
* @returns  void
*
* @notes    Prints the states transtiontions  recorded for a specific Vlan
*           both  for Registrars and Applicants of that machine
*
*           Called by user (developer) like
*
*               devshel garpDebugTrackVlanPrint()
*
*
*           The output might look like:
*
*   > Vlan 370:
*   >   Applicatnt State Transitions :
*   >         Event=Gid_leaveall : A= Qa,
*
*   >   Resistrar State Taransitions:
*   >         Event=Gid_leave   : R = Lv
*
* @end
*********************************************************************/
void garpDebugTrackVlanPrint()
{
  L7_uint32 i,j ;
  L7_uint32 app_index, reg_index;

  if (bTrackVlan)
  {
    sysapiPrintf("\n Vlan count : %d",vlanCnt);
    for (i=0;i<vlanCnt;i++)
    {
      sysapiPrintf("\n Vlan ID: %d",Track_Vlan[i]);

      /* Applicant info*/
      app_index = garpDebugVlanRec[i].nAppEvtCnt;
      if (app_index > 0)
      {
            sysapiPrintf("\n Number of times storage exceeded: %u",garpDebugVlanRec[i].nAppEvtEx);
            sysapiPrintf("\n\n Applicant State Transitions:");
            for(j=0;j<app_index;j++)
            {
              sysapiPrintf("\nEvent = %s  : A = %s",
                       Gid_Dbg_Events[garpDebugVlanRec[i].gvrpApplicantTrans[j].event],
                       GetApplicantState[garpDebugVlanRec[i].gvrpApplicantTrans[j].val]);
            }
        }/* if nAppEvtCnt =0 */
      else
      {
        sysapiPrintf("\n No Applicant State Transtions recorded.");
      }

      /* Registrars Info*/
      reg_index = garpDebugVlanRec[i].nRegEvtCnt;
      if (reg_index > 0)
      {
         sysapiPrintf("\n\n Registrar State Transitions:");
         sysapiPrintf("\n Number of times storage exceeded: %u",garpDebugVlanRec[i].nRegEvtEx);
         for(j=0;j<reg_index;j++)
         {
           sysapiPrintf("\nEvent = %s  : R = %s",
                       Gid_Dbg_Events[garpDebugVlanRec[i].gvrpRegistrarTrans[j].event],
                       GetRegisrarState[garpDebugVlanRec[i].gvrpRegistrarTrans[j].val]);
         }
       }/* if nRegEvtCnt =0 */
      else
      {
        sysapiPrintf("\n No Registrar State Transitions recorded.");
      }
   }/* for vlans*/
 }
 else
 {
   sysapiPrintf("\n  Vlan Tracking not enabled.");
 }

}

/*********************************************************************
* @purpose  Initialize garpDebugVlanRec array for tracking vlan
*
* @param    none
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
 void garpDebugVlanRecInit(L7_ushort16 indx)
 {
  /* L7_uint32 i;

   for (i=0;i<L7_MAX_VLANS;i++)
   { */
     memset(&(garpDebugVlanRec[indx].gvrpApplicantTrans),0,sizeof(garpTransitions_t));
     memset(&(garpDebugVlanRec[indx].gvrpRegistrarTrans),0,sizeof(garpTransitions_t));
     garpDebugVlanRec[indx].nAppEvtCnt =0;
     garpDebugVlanRec[indx].nRegEvtCnt =0;
     garpDebugVlanRec[indx].bVlanCreated=L7_FALSE;
     garpDebugVlanRec[indx].nAppEvtEx =0;
     garpDebugVlanRec[indx].nRegEvtEx =0;

  /* }

   bTrackVlan = L7_TRUE;   */
   return;
 }

 /*********************************************************************
 * @purpose  Initialize VlanId for tracking vlan
 *
 * @param   Vlan_id VlanId  , vlan Id of the vlan whoose transitions are
 *                            to be recorded
 *
 * @returns  void
 *
 * @comments
 *
 * @end
 *********************************************************************/
  void garpDebugTrackVlanSet(Vlan_id vlanId)
  {
    L7_uint32 i=0;
    if ( vlanId > L7_MAX_VLANS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
              "\n%s: Vlan Id out of range. Vlan Id : %u",vlanId);
      return;
    }

    /* check if vlanId needs to be tracked */
    for(i=0;i<vlanCnt;i++)
    {
       if (Track_Vlan[i] == vlanId)
       {
          vlanFnd = L7_TRUE;
          VlanIndx = i;
          break;
       }
    }

  }

  /*********************************************************************
  * @purpose  Reset vlanFnd flag and VlanIndx
  *
  * @param   void
  *
  * @returns  void
  *
  * @comments
  *
  * @end
  *********************************************************************/
  void garpDebugTrackVlanReset()
  {
    if(vlanFnd)
    {
      vlanFnd = L7_FALSE;
      VlanIndx = 0;
    }
  }

/*********************************************************************
* @purpose  Record Applicant or Registrar  state according to given vlan Id
*
* @param    none
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void garpDebugTrackVlanRecord(L7_uchar8 app_state,Gid_event event,gvrpType type)
 {
   L7_uint32 app_index;
   L7_uint32 reg_index=0;

   if ((type!= GVRP_APP) && (type!= GVRP_REG))
   {
     L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
             "\n%s: Invalid Type.Type: %u",__FUNCTION__,type);
     return;
   }

   if ((VlanIndx <0 ) || ( VlanIndx > L7_MAX_VLAN_TRACK))
   {
     L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
             "\n%s: Vlan Id out of range. Vlan Index : %u",VlanIndx);
     return;
   }

   /* set vlan cnt */
   if (garpDebugVlanRec[VlanIndx].bVlanCreated == L7_FALSE)
   {
      garpDebugVlanRec[VlanIndx].bVlanCreated = L7_TRUE;
   }


   /* set the appropriate value */
   if (type == GVRP_APP)
   {
     app_index = garpDebugVlanRec[VlanIndx].nAppEvtCnt;
     garpDebugVlanRec[VlanIndx].gvrpApplicantTrans[app_index].event = event;
     garpDebugVlanRec[VlanIndx].gvrpApplicantTrans[app_index].val = app_state;
     garpDebugVlanRec[VlanIndx].nAppEvtCnt++;
     if( garpDebugVlanRec[VlanIndx].nAppEvtCnt > L7_GARP_DEBUG_MAX_EVNT_CNT)
     {
        garpDebugVlanRec[VlanIndx].nAppEvtEx++;
        garpDebugVlanRec[VlanIndx].nAppEvtCnt=0;
     }
   }
   else if (type == GVRP_REG)
   {
     reg_index = garpDebugVlanRec[VlanIndx].nRegEvtCnt;
     garpDebugVlanRec[VlanIndx].gvrpRegistrarTrans[reg_index].event = event;
     garpDebugVlanRec[VlanIndx].gvrpRegistrarTrans[reg_index].val = app_state;
     garpDebugVlanRec[VlanIndx].nRegEvtCnt++;
     if( garpDebugVlanRec[VlanIndx].nRegEvtCnt > L7_GARP_DEBUG_MAX_EVNT_CNT)
     {
        garpDebugVlanRec[VlanIndx].nRegEvtEx++;
        garpDebugVlanRec[VlanIndx].nRegEvtCnt=0;
     }


   }

   return;

 }

 /*********************************************************************
* @purpose  Get Input on the vlans to be tracked
*
* @param    L7_uint32 VlanId  vlan Id to be tracked
*
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
 L7_RC_t garpDebugTrackVlan(L7_short16 vlanId)
 {
   if (vlanCnt >= L7_MAX_VLAN_TRACK)
   {
     sysapiPrintf("\n Maximum Vlans (%d) that can be tracked already set.",vlanCnt);
     return  L7_FAILURE;
   }

   if (vlanId >L7_MAX_VLANS)
   {
     sysapiPrintf("\n Vlan Id (%u) out of range.",vlanId);
   }


   Track_Vlan[vlanCnt]=vlanId;
   /* initialize structures*/
   garpDebugVlanRecInit(vlanCnt);
   vlanCnt++;

   bTrackVlan = L7_TRUE;

   return L7_SUCCESS;

 }

 /*********************************************************************
* @purpose  Reset all vlans tracking information
*
* @param    void
*
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
 void garpDebugTrackVlanResetAll()
 {
   L7_uint32 i;
   for(i=0;i<vlanCnt;i++)
   {
     garpDebugVlanRecInit(i);
     Track_Vlan[i]=0;

   }
   vlanCnt =0;
   if(vlanFnd)
   {
     vlanFnd = L7_FALSE;
     VlanIndx=0;
   }
   bTrackVlan = L7_FALSE;

 }


 /*********************************************************************
 * @purpose  Print Gid structure associated with an interface
 *
 * @param    intIfNum       @b{(input)} internal Interface Number
 *
 * @returns  void
 *
 * @comments
 *
 * @end
 *********************************************************************/
 void garpDebugGvrpGidShow(L7_uint32 intIfNum)
 {
     Gid *my_port;
     Gid *first_gid;
     L7_BOOL print_usage;

     my_port    = L7_NULL;
     first_gid  = L7_NULL;
     print_usage = L7_FALSE;


     if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
     {
         sysapiPrintf("interface %d is not valid\n", intIfNum );
         print_usage = L7_TRUE;
     }

     if (print_usage == L7_TRUE)
     {
         sysapiPrintf("\n");
         sysapiPrintf("Usage: garpDebugIntfGidShow(app,intIfNum)\n");
         sysapiPrintf("     where \n");
         sysapiPrintf("         app = {GVRP = 0, GMRP = 1} \n");

         return;
     }

     if (gid_find_port(first_gid, intIfNum, (void*)&my_port) != L7_TRUE)
     {
         sysapiPrintf("\n");
         sysapiPrintf("Gid structure for interface %d not found\n", intIfNum);
         return;
     }

     garpDebugGidShow(my_port);
 }


 /*********************************************************************
 * @purpose  Print Gid structure associated with a vlan and interface
 *
 * @param    vlanId       @b{(input)} vlanId
 * @param    intIfNum     @b{(input)} internal interface Number
 *
 * @returns  void
 *
 * @comments
 *
 * @end
 *********************************************************************/
 void garpDebugGmrpGidShow(L7_uint32 vlanId,L7_uint32 intIfNum)
 {
     Gid *my_port;
     L7_BOOL print_usage;
     L7_uint32 index;
     Gmr *application;

     my_port    = L7_NULL;
     print_usage = L7_FALSE;

     if (dot1qVlanCheckValid(vlanId) != L7_SUCCESS)
     {
         /* Note: Process command even if there is a mismatch.
            There may be leftover data in the tables */
         sysapiPrintf("VLAN %d does not exist in dot1q\n", vlanId );

     }

     if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
     {
         sysapiPrintf("interface %d is not valid\n", intIfNum );
         print_usage = L7_TRUE;
     }

     if (print_usage == L7_TRUE)
     {
         sysapiPrintf("\n");
         sysapiPrintf("Usage: garpDebugGmrpGidShow(vlanId,intIfNum)\n");
         return;
     }

     if (gmrpInstanceIndexFind(vlanId, &index) != L7_SUCCESS)
     {
          sysapiPrintf("gmrpInstance does not exist for vlan %d\n", vlanId);
          return;
     }


     application = gmrpInstance[index].gmr;
     if(application->g.gid != L7_NULL)
     {
         if (gid_find_port(application->g.gid, intIfNum, (void *) &my_port)== L7_TRUE)
         {
             garpDebugGidShow(my_port);
         }
         else
         {
             sysapiPrintf("Gid not found for vlan %d and intIfNum 5d\n", vlanId, intIfNum);
         }
     }
     else
     {
         sysapiPrintf("Gid pointer is null vlan %d\n", vlanId);
     }


 }



 /*********************************************************************
 * @purpose  Print Gid structure
 *
 * @param    *my_port       @b{(input)} pointer to a Gid structure
 *
 * @returns  void
 *
 * @comments
 *
 * @end
 *********************************************************************/
 void garpDebugGidShow(Gid *my_port)
 {
      sysapiPrintf("\n");
      sysapiPrintf("/*-----------------------------*/\n");
      sysapiPrintf("/* Printing Gid Structures    */\n");
      sysapiPrintf("/*-----------------------------*/\n");
      sysapiPrintf("\n");

      sysapiPrintf("my_port->application->app:           %d\n",my_port->application->app);
      sysapiPrintf("my_port->application->app:           %d\n",my_port->application->app);
      sysapiPrintf("my_port->port_no:                    %d\n",my_port->port_no);
      sysapiPrintf("my_port->is_enabled:                 %d\n",my_port->is_enabled);
      sysapiPrintf("my_port->is_connected:               %d\n",my_port->is_connected);
      sysapiPrintf("my_port->cschedule_tx_now:           %d\n",my_port->cschedule_tx_now);
      sysapiPrintf("my_port->cstart_join_timer:          %d\n",my_port->cstart_join_timer);
      sysapiPrintf("my_port->cstart_leave_timer:         %d\n",my_port->cstart_leave_timer);
      sysapiPrintf("my_port->join_timer_running:         %d\n",my_port->join_timer_running);
      sysapiPrintf("my_port->leave_timer_running:        %d\n",my_port->leave_timer_running);
      sysapiPrintf("my_port->tx_pending:                 %d\n",my_port->tx_pending);
      sysapiPrintf("my_port->transmit_leaveall:          %d\n",my_port->transmit_leaveall);
      sysapiPrintf("my_port->join_timeout:               %d\n",my_port->join_timeout);
      sysapiPrintf("my_port->leave_timeout:              %d\n",my_port->leave_timeout);
      sysapiPrintf("my_port->leaveall_timeout:           %d\n",my_port->leaveall_timeout);
      sysapiPrintf("my_port->gmrp_leaveall_left:         %d\n",my_port->gmrp_leaveall_left);
      sysapiPrintf("&my_port->machines:                  %x\n",my_port->machines);
      sysapiPrintf("my_port->last_transmitted:           %d\n",my_port->last_transmitted);
      sysapiPrintf("my_port->untransmit_machine:         %d\n",my_port->untransmit_machine);
      sysapiPrintf("\n");
      sysapiPrintf("&my_port->join_timer_instance                %x\n", &my_port->join_timer_instance);
      sysapiPrintf("my_port->join_timer_instance.timerType      %d\n", my_port->join_timer_instance.timerType);
      sysapiPrintf("my_port->join_timer_instance.vid            %d\n", my_port->join_timer_instance.vid);
      sysapiPrintf("my_port->join_timer_instance.port_no        %d\n", my_port->join_timer_instance.port_no);
      sysapiPrintf("my_port->join_timer_instance.expiryTime     %d\n", my_port->join_timer_instance.expiryTime);
      sysapiPrintf("my_port->join_timer_instance.timer_status   %d\n", my_port->join_timer_instance.timer_status);
      sysapiPrintf("\n");
      sysapiPrintf("&my_port->leave_timer_instance                %x\n", &my_port->leave_timer_instance);
      sysapiPrintf("my_port->leave_timer_instance.timerType      %d\n", my_port->leave_timer_instance.timerType);
      sysapiPrintf("my_port->leave_timer_instance.vid            %d\n", my_port->leave_timer_instance.vid);
      sysapiPrintf("my_port->leave_timer_instance.port_no        %d\n", my_port->leave_timer_instance.port_no);
      sysapiPrintf("my_port->leave_timer_instance.expiryTime     %d\n", my_port->leave_timer_instance.expiryTime);
      sysapiPrintf("my_port->leave_timer_instance.timer_status   %d\n", my_port->leave_timer_instance.timer_status);
      sysapiPrintf("\n");


      sysapiPrintf("\n");
 }



/*********************************************************************
 * @purpose  Dump GARP Info structures for both GVRP and GMRP
 *
 * @param    void
 *
 * @returns  void
 *
 * @comments
 *
 * @end
 *********************************************************************/
 void garpDebugGarpStatusShow(void)
 {
     L7_uint32 i;
     Garp *pGarp;
     extern Gvr  *gvrp_app;


     sysapiPrintf("\n");
     sysapiPrintf("/*-----------------------------*/\n");
     sysapiPrintf("/* Printing GARP Control Block */\n");
     sysapiPrintf("/*-----------------------------*/\n");
     sysapiPrintf("\n");
     sysapiPrintf("GarpCB_gvrpEnabled:      %d\n", GARPCB->GarpCB_gvrpEnabled);
     sysapiPrintf("GarpCB_gmrpEnabled:      %d\n", GARPCB->GarpCB_gmrpEnabled);
     sysapiPrintf("\n");

     sysapiPrintf("\n");
     sysapiPrintf("/*-----------------------------*/\n");
     sysapiPrintf("/* Printing garpInfo Structure */\n");
     sysapiPrintf("/*-----------------------------*/\n");
     sysapiPrintf("\n");

     sysapiPrintf("garpInfo.msgsRecvdOnQueue:            %d\n",garpInfo.msgsRecvdOnQueue);
     sysapiPrintf("garpInfo.queueUsageOverFiftyPercent:  %d\n",garpInfo.queueUsageOverFiftyPercent);
     sysapiPrintf("garpInfo.queueUsageOverEightyPercent: %d\n",garpInfo.queueUsageOverEightyPercent);
     sysapiPrintf("garpInfo.queueUsageOverNinetyPercent: %d\n",garpInfo.queueUsageOverNinetyPercent);
     sysapiPrintf("\n");
     sysapiPrintf("garpInfo.infoCounters.gmrpRegistationCount:   %d\n",garpInfo.infoCounters.gmrpRegistationCount);
     sysapiPrintf("garpInfo.infoCounters.gmrpRxPdusNotProcessed: %d\n",garpInfo.infoCounters.gmrpRxPdusNotProcessed);
     sysapiPrintf("garpInfo.infoCounters.gmrpInstanceForcedUpdates: %d\n",garpInfo.infoCounters.gmrpInstanceForcedUpdates);

     sysapiPrintf("\n");
     sysapiPrintf("garpInfo.infoCounters.timerExpiriesAfterGARPDisable:   %d\n",garpInfo.infoCounters.timerExpiriesAfterGARPDisable);
     sysapiPrintf("garpInfo.infoCounters.leaveAllTimerExpiriesAfterGARPDisable: %d\n",garpInfo.infoCounters.leaveAllTimerExpiriesAfterGARPDisable);
     sysapiPrintf("\n");
     sysapiPrintf("garpInfo.infoCounters.garpPDUsReceived:   %d\n",garpInfo.infoCounters.garpPDUsReceived);
     sysapiPrintf("garpInfo.infoCounters.gvrpPDUsReceived:   %d\n",garpInfo.infoCounters.gvrpPDUsReceived);
     sysapiPrintf("garpInfo.infoCounters.gmrpPDUsReceived:   %d\n",garpInfo.infoCounters.gmrpPDUsReceived);
     sysapiPrintf("\n");
     sysapiPrintf("/*-------------------------*/\n");
     sysapiPrintf("/* Printing Garp structures  */\n");
     sysapiPrintf("/*-------------------------*/\n");
     sysapiPrintf("\n");
     sysapiPrintf("\n");


     for (i=0; i < UNDEFINED_GARP_APP; i++)
     {
         if (i == GARP_GVRP_APP )
             pGarp = &gvrp_app->g;
         else
             break;
         sysapiPrintf("Printing %s structure:\n",  (pGarp->app == GARP_GVRP_APP) ? "GARP_GVRP_APP":"GARP_GMRP_APP");
         sysapiPrintf("\n");
         sysapiPrintf("app:               %s \n",(pGarp->app ?GARP_GVRP_APP:GARP_GMRP_APP));
         sysapiPrintf("process_id:        %x \n",pGarp->process_id);
         sysapiPrintf("gid:               %x \n",pGarp->gid);
         sysapiPrintf("gip:               %x \n",pGarp->gip);
         sysapiPrintf("max_gid_index:     %d \n",pGarp->max_gid_index);
         sysapiPrintf("last_gid_used:     %d \n",pGarp->last_gid_used);
         sysapiPrintf("vlan_id:           %d \n",pGarp->vlan_id);
         sysapiPrintf("\n");

     } /* i < UNDEFINED_GARP_APP */


 }

/*********************************************************************
* @purpose  Print GARP interface counters
*
* @param    intIfNum       @b{(input)} internal Interface Number
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
 void garpDebugIntfCountersShow(L7_uint32 intIfNum)
 {

     if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
     {
         sysapiPrintf("interface %d is not valid\n", intIfNum );
         return;
     }

      sysapiPrintf("/*-----------------------------*/\n");
      sysapiPrintf("/* Printing GARP PDU Counters  */\n");
      sysapiPrintf("/*-----------------------------*/\n");
      sysapiPrintf("\n");

      sysapiPrintf("GVRP Packets Received               %d\n",  gvrpInfo->received[intIfNum]);
      sysapiPrintf("GVRP Packets Sent                   %d\n",  gvrpInfo->send[intIfNum]);
      sysapiPrintf("GVRP Packets Failed Registrations   %d\n",  gvrpInfo->failedReg[intIfNum]);
      sysapiPrintf("\n");
      sysapiPrintf("GMRP Packets Received               %d\n",  gmrpInfo->received[intIfNum]);
      sysapiPrintf("GMRP Packets Sent                   %d\n",  gmrpInfo->send[intIfNum]);
      sysapiPrintf("GMRP Packets Failed Registrations   %d\n",  gmrpInfo->failedReg[intIfNum]);
      sysapiPrintf("\n");

 }


/*********************************************************************
* @purpose  Print VLANs in use for GMRP
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void garpDebugGmrpDot1sInstanceShow(void)
{
    L7_uint32 i;
    L7_uint32 j;

    sysapiPrintf("\n");
    sysapiPrintf("/*----------------------------------*/\n");
    sysapiPrintf("/* Printing gmrpInstance Structures  */\n");
    sysapiPrintf("/*----------------------------------*/\n");
    sysapiPrintf("\n");
    sysapiPrintf("VLANs in use:\n");
    sysapiPrintf("\n");

    j = 0;
    sysapiPrintf("List of vlans:                      \n");
    for (i=0; i < L7_MAX_VLAN_ID; i++)
    {
        if (gmrpInstance[i].inuse == L7_TRUE)
        {
            /* Print both values to determine if there is a discrepancy */
            sysapiPrintf("  %05d (%d) - ",i, gmrpInstance[i].gmr->vlan_id);
            if (j%5)
            {
                sysapiPrintf("\n");
            }
            j++;
        }
    }

    sysapiPrintf("\n");
    sysapiPrintf("\n");
}


/*********************************************************************
* @purpose  Print GARP dot1s instance info
*
* @param    dot1sInstanceId   @b{(input)} dot1s instance identifier
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
 void garpDebugDot1sInstanceShow(L7_uint32 dot1sInstanceId)
 {
     portInRing_t *pPortInRing;
     L7_uint32      i;

     if (dot1sInstanceGet(dot1sInstanceId) != L7_SUCCESS)
     {
         sysapiPrintf("MST instance %d does not exist\n", dot1sInstanceId);
         return;
     }

     sysapiPrintf("\n");
     sysapiPrintf("/*----------------------------------*/\n");
     sysapiPrintf("/* Printing multipleGip Structures  */\n");
     sysapiPrintf("/*----------------------------------*/\n");
     sysapiPrintf("\n");
     sysapiPrintf("/* For Dot1s Instance %d:  */\n",  dot1sInstanceId);
     sysapiPrintf("\n");

     sysapiPrintf("inUse:                              %d\n",multipleGIP[dot1sInstanceId].inUse);
     sysapiPrintf("intstanceID:                        %d\n",multipleGIP[dot1sInstanceId].intstanceID);
     sysapiPrintf("\n");
     sysapiPrintf("List of ports:                      \n");


     pPortInRing   = multipleGIP[dot1sInstanceId].ringOfPorts;
     i = 0;

     while (pPortInRing != L7_NULLPTR)
     {
         sysapiPrintf("                                    %05d-",multipleGIP[dot1sInstanceId].ringOfPorts->portNumber);
         if (i%5)
         {
             sysapiPrintf("\n");
         }
         pPortInRing = pPortInRing->next;
         i++;

     }

     sysapiPrintf("\n");
     sysapiPrintf("\n");
 }

/*********************************************************************
 * @purpose  Dump All GARP related structures
 *
 * @param    void
 *
 * @returns  void
 *
 * @comments This is a general dump.  This could take a long time.
 *
 * @end
 *********************************************************************/
  void garpDebugShowAll(void)
  {
      L7_RC_t   rc;
      L7_uint32 intIfNum;
      L7_uint32 vlanId, nextvlanId;
      L7_uint32 i;


      sysapiPrintf("\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/* Invoking garpDebugGarpStatusShow            */\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("\n");

      garpDebugGarpStatusShow();

      sysapiPrintf("\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/* Invoking garpDebugIntfCountersShow          */\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("\n");

      rc = nimFirstValidIntfNumber(&intIfNum);
      while (rc == L7_SUCCESS)
      {
          if (garpIsValidIntf(intIfNum) != L7_TRUE)
          {
              rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
              continue;
          }

          sysapiPrintf("\n");
          sysapiPrintf("INTERFACE     %d    \n", intIfNum);
          sysapiPrintf("=====================\n");

          garpDebugIntfCountersShow(intIfNum);
          rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
      }


      sysapiPrintf("\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/* Invoking garpDebugGvdRecordsInfo            */\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("\n");

      garpDebugGvdRecordsInfo();


      sysapiPrintf("\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/* Invoking garpDebugMsgQueueShow            */\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("\n");

      garpDebugMsgQueueShow();

      sysapiPrintf("\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/* Invoking garpDebugTraceModePrint            */\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("\n");

      garpDebugTraceModePrint();


      sysapiPrintf("\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/* Invoking garpDebugGvrpGidShow               */\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("\n");


      rc = nimFirstValidIntfNumber(&intIfNum);
      while (rc == L7_SUCCESS)
      {
          if (garpIsValidIntf(intIfNum) != L7_TRUE)
          {
              rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
              continue;
          }

          sysapiPrintf("\n");
          sysapiPrintf("INTERFACE     %d    \n", intIfNum);
          sysapiPrintf("=====================\n");

          garpDebugGvrpGidShow(intIfNum);
          rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
      }





      sysapiPrintf("\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/* Invoking garpDebugGmrpGidShow               */\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("\n");


      vlanId = 0;
      while (L7_SUCCESS== dot1qNextVlanGet(vlanId, &nextvlanId))
      {

          sysapiPrintf("\n");
          sysapiPrintf("VLAN %d, INTERFACE %d    \n", vlanId, intIfNum);
          sysapiPrintf("============================\n");

          rc = nimFirstValidIntfNumber(&intIfNum);
          while (rc == L7_SUCCESS)
          {
              if (garpIsValidIntf(intIfNum) != L7_TRUE)
              {
                  rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
                  continue;
              }

              sysapiPrintf("\n");
              sysapiPrintf("INTERFACE     %d    \n", intIfNum);
              sysapiPrintf("=====================\n");

              garpDebugGmrpGidShow(vlanId, intIfNum);
              rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
          }

          vlanId = nextvlanId;
      } /* dot1qNextVlanGet */


      sysapiPrintf("\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/* Invoking garpDebugDot1sInstanceShow         */\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("\n");


      for (i=0; i < L7_MAX_MULTIPLE_STP_INSTANCES; i++)
      {

          sysapiPrintf("\n");
          sysapiPrintf("INSTANCE     %d    \n", i);
          sysapiPrintf("=====================\n");

          garpDebugDot1sInstanceShow(i);

      } /* i < L7_MAX_MULTIPLE_STP_INSTANCES */

      sysapiPrintf("\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/* Invoking garpDebugGmrpDot1sInstanceShow     */\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("\n");

      garpDebugGmrpDot1sInstanceShow();


      sysapiPrintf("\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/* Invoking garpDebugGvrpStateInfo             */\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("\n");


      rc = nimFirstValidIntfNumber(&intIfNum);
      while (rc == L7_SUCCESS)
      {
          if (garpIsValidIntf(intIfNum) != L7_TRUE)
          {
              rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
              continue;
          }

          sysapiPrintf("\n");
          sysapiPrintf("INTERFACE     %d    \n", intIfNum);
          sysapiPrintf("=====================\n");

          garpDebugGvrpStateInfo(L7_MAX_VLANS+1,intIfNum);
          rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
      }



      sysapiPrintf("\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/* Invoking garpDebugGmrpStateInfo             */\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("\n");



      vlanId = 0;
      while (L7_SUCCESS== dot1qNextVlanGet(vlanId, &nextvlanId))
      {

          sysapiPrintf("\n");
          sysapiPrintf("VLAN %d    \n", vlanId, intIfNum);
          sysapiPrintf("==============\n");
          sysapiPrintf("==============\n");

          rc = nimFirstValidIntfNumber(&intIfNum);
          while (rc == L7_SUCCESS)
          {
              if (garpIsValidIntf(intIfNum) != L7_TRUE)
              {
                  rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
                  continue;
              }

              sysapiPrintf("\n");
              sysapiPrintf("VLAN %d, INTERFACE %d    \n", vlanId, intIfNum);
              sysapiPrintf("=====================\n");

              garpDebugGmrpStateInfo(L7_MAX_GROUP_REGISTRATION_ENTRIES+1,intIfNum, vlanId);
              rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
          }

          vlanId = nextvlanId;
      } /* dot1qNextVlanGet */

      sysapiPrintf("\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/* Invoking garpDebugGvrpLeaveAllTimerInfo     */\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("\n");

      rc = nimFirstValidIntfNumber(&intIfNum);
      while (rc == L7_SUCCESS)
      {
          if (garpIsValidIntf(intIfNum) != L7_TRUE)
          {
              rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
              continue;
          }

          sysapiPrintf("\n");
          sysapiPrintf("INTERFACE     %d    \n", intIfNum);
          sysapiPrintf("=====================\n");

          garpDebugGvrpLeaveAllTimerInfo(intIfNum);

          rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
      }



      sysapiPrintf("\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/* Invoking garpDebugGmrpLeaveAllTimerInfo     */\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("/*---------------------------------------------*/\n");
      sysapiPrintf("\n");

      rc = nimFirstValidIntfNumber(&intIfNum);
      while (rc == L7_SUCCESS)
      {
          if (garpIsValidIntf(intIfNum) != L7_TRUE)
          {
              rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
              continue;
          }

          sysapiPrintf("\n");
          sysapiPrintf("INTERFACE     %d    \n", intIfNum);
          sysapiPrintf("=====================\n");

          garpDebugGmrpLeaveAllTimerInfo(intIfNum);

          rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
      }
  }
