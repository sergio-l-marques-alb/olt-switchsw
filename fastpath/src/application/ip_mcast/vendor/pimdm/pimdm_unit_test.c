
/**************************************** UT Stubs Start ***********************/


#include "l7_common.h"
#include "l3_mcast_commdefs.h"
#include "l3_addrdefs.h"
#include "pimdm_main.h"
#include "pimdm_debug.h"
#include "pimdm_mrt.h"
#include "pimdm_upstrm_fsm.h"
#include "pimdm_dnstrm_fsm.h"
#include "pimdm_strfr_fsm.h"
#include "pimdm_asrt_fsm.h"

pimdmCB_t *stubCbHandle[2] = {L7_NULLPTR, L7_NULLPTR};


#define PIMDM_IPV6_INADDR_ALL_PIM_ROUTERS  "fffe::05"
#define PIMDM_IPV6_INADDR_PIM_ROUTER1  "fffe::01"
#define PIMDM_IPV6_INADDR_PIM_ROUTER2  "fffe::02"
#define PIMDM_IPV6_INADDR_PIM_ROUTER3  "fffe::03"
#define PIMDM_IPV6_INADDR_PIM_ROUTER4  "fffe::04"

#define PIMDM_IPV6_SRC_ADDR "2001::7"
#define PIMDM_IPV6_SRC_ADDR1  "2001::8"
#define PIMDM_IPV6_SRC_ADDR2  "2002::7"
#define PIMDM_IPV6_SRC_ADDR3  "3005::9"
#define PIMDM_IPV6_SRC_ADDR4  "2005::8"

L7_inet_addr_t pimdmDebugSrcAddr[5] =
{
  {L7_AF_INET,{{0x64010102}}},
  {L7_AF_INET,{{0xc0a80a64}}},
  {L7_AF_INET,{{0xc0a80a0a}}},
  {L7_AF_INET,{{0x0a010105}}},
  {L7_AF_INET,{{0x14010105}}},
};
L7_inet_addr_t pimdmDebugGrpAddr[5] =
{
  {L7_AF_INET,{{0xe1010101}}},
  {L7_AF_INET,{{0xe1010102}}},
  {L7_AF_INET,{{0xe1010103}}},
  {L7_AF_INET,{{0xe1010104}}},
  {L7_AF_INET,{{0xe1010105}}},
};


static void pimdmDebugV6StringToAddr ( L7_uint32 index,
                                       L7_in6_addr_t * Ipv6Addr );

static void pimdmDebugV6SrcStringToAddr ( L7_uint32 index,
                                       L7_in6_addr_t * Ipv6Addr );

L7_RC_t
pimdmStubDeInitPimdmCtrlBlock (void)
{
  if (pimdmCtrlBlockDeInit (stubCbHandle[0]) != L7_SUCCESS)
  {
    PIMDM_DEBUG_PRINTF ("\nSTUB: Control Block De-Init Failed.\n");
    return L7_FAILURE;
  }
  else
  {
  	PIMDM_DEBUG_PRINTF("\n STUB:Control Block De-Init Success\n");
    return L7_SUCCESS;
  }

}

L7_RC_t
pimdmStubDeInitPimdmExecBlock (void)
{
  return L7_NOT_SUPPORTED;
}

L7_RC_t
pimdmStubEventSend(void)
{
  
  pimdmMapEvent_t eventMsg;
  pimdmMapIntfMode_t  intfMode;
  
  intfMode.helloHoldTime =34;
  intfMode.helloInterval= 10;
  intfMode.intIfIndex =1;
  intfMode.mode =L7_ENABLE;
  intfMode.overrideIntvl = 32;
  intfMode.propagationDelay =34;
  intfMode.rtrIfIndex =1 ;
  intfMode.triggeredHelloDelay = 2;

  eventMsg.msg.intfMode = intfMode;

  if(pimdmIntfAdminModeSet(stubCbHandle[0], &eventMsg) == L7_SUCCESS)
  	{
  	  PIMDM_DEBUG_PRINTF("\n STUB:Posted event to PimdmTaskMain\n");
  	  return L7_SUCCESS;
  	}
  else

  	{
  	 PIMDM_DEBUG_PRINTF("\n STUB:Failed to post and event to pimdmTaskMain\n");
  	 return L7_FAILURE;
  	}

}

void pimdmDebugUpStrmFSM(L7_uchar8 family,L7_uint32 index,L7_uint32 state,
                         L7_uint32 event)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t *pimdmMrtEntry = L7_NULLPTR;
  pimdmUpstrmEventInfo_t eventInfo;
  L7_uchar8 src[40],grp[40];
  L7_in6_addr_t Ipv6Addr;
  memset(&eventInfo,0,sizeof(pimdmUpstrmEventInfo_t));
  
  pimdmCB = pimdmMapProtocolCtrlBlockGet(family);

  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("pimdmMapProtocolCtrlBlockGet failed");
    return;
  }

  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimdmDebugV6StringToAddr( index, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimdmDebugGrpAddr[index] );

    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimdmDebugV6SrcStringToAddr(index, &Ipv6Addr);
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimdmDebugSrcAddr[index] );


  }
  PIMDM_DEBUG_PRINTF ("Source Address is %s,Group Address is %s",inetAddrPrint(&pimdmDebugSrcAddr[index],src),inetAddrPrint(&pimdmDebugGrpAddr[index],grp)); 
  pimdmMrtEntry = pimdmMrtEntryAdd(pimdmCB,&pimdmDebugSrcAddr[index],
                   &pimdmDebugGrpAddr[index],index);

  if(pimdmMrtEntry == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("pimdmMrtEntryAdd failed");
    return;
  } 
  eventInfo.rtrIfIndex = index;
  pimdmMrtEntry->upstrmGraftPruneStateInfo.grfPrnState = state;

  if(pimdmUpstrmFsmExecute(event,
                           pimdmMrtEntry,&eventInfo)
                          != L7_SUCCESS)
  {
    PIMDM_DEBUG_PRINTF ("pimdmUpstrmFsmExecute failed");
    return;
  }
   
  PIMDM_DEBUG_PRINTF ("Success");
  return;
}

void pimdmDebugMrtEntryRemove(L7_uchar8 family,L7_uint32 index)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t *pimdmMrtEntry = L7_NULLPTR;
  L7_uchar8 src[40],grp[40];
  L7_in6_addr_t Ipv6Addr;
 
  pimdmCB = pimdmMapProtocolCtrlBlockGet(family);

  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("pimdmMapProtocolCtrlBlockGet failed");
    return;
  }
  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimdmDebugV6StringToAddr( index, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimdmDebugGrpAddr[index] );

    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimdmDebugV6SrcStringToAddr(index, &Ipv6Addr);
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimdmDebugSrcAddr[index] );
  }
  PIMDM_DEBUG_PRINTF ("Source Address is %s,Group Address is %s",inetAddrPrint(&pimdmDebugSrcAddr[index],src),inetAddrPrint(&pimdmDebugGrpAddr[index],grp));

  pimdmMrtEntry = pimdmMrtEntryGet(pimdmCB,&pimdmDebugSrcAddr[index],
                                   &pimdmDebugGrpAddr[index]);
  if(pimdmMrtEntry == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("pimdmMrtEntryGet failed");
    return;
  }
  if(pimdmMrtEntryRemove(pimdmCB,pimdmMrtEntry) != L7_SUCCESS)
  {
    PIMDM_DEBUG_PRINTF ("pimdmMrtEntryRemove failed");
    return;
  } 
  PIMDM_DEBUG_PRINTF ("Exit");
  return;
}

void pimdmDebugDnStrmFSM(L7_uchar8 family,L7_uint32 index,L7_uint32 rtrIfNum,
                         L7_uint32 state,
                         L7_uint32 event)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t *pimdmMrtEntry = L7_NULLPTR;
  pimdmDnstrmEventInfo_t eventInfo;
  L7_uchar8 src[40],grp[40];
  L7_in6_addr_t Ipv6Addr;

  memset(&eventInfo,0,sizeof(pimdmDnstrmEventInfo_t));
  
  pimdmCB = pimdmMapProtocolCtrlBlockGet(family);

  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("pimdmMapProtocolCtrlBlockGet failed");
    return;
  }
  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimdmDebugV6StringToAddr( index, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimdmDebugGrpAddr[index] );

    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimdmDebugV6SrcStringToAddr(index, &Ipv6Addr);
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimdmDebugSrcAddr[index] );


    pimdmDebugV6SrcStringToAddr(3, &Ipv6Addr);
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimdmDebugSrcAddr[3] );
  }

  PIMDM_DEBUG_PRINTF ("Source Address is %s,Group Address is %s",inetAddrPrint(&pimdmDebugSrcAddr[index],src),inetAddrPrint(&pimdmDebugGrpAddr[index],grp)); 
  pimdmMrtEntry = pimdmMrtEntryGet(pimdmCB,&pimdmDebugSrcAddr[index],
                   &pimdmDebugGrpAddr[index]);

  if(pimdmMrtEntry == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("pimdmMrtEntryAdd failed");
    return;
  } 
  eventInfo.rtrIfIndex = rtrIfNum;
  eventInfo.holdTime = 60;
  inetCopy(&eventInfo.nbrAddr,&pimdmDebugSrcAddr[3]);
  pimdmMrtEntry->downstrmStateInfo[rtrIfNum].pruneState = state;
  if(pimdmDnstrmFsmExecute(event,
                           pimdmMrtEntry,&eventInfo)
                          != L7_SUCCESS)
  {
    PIMDM_DEBUG_PRINTF ("pimdmUpstrmFsmExecute failed");
    return;
  }
   
  PIMDM_DEBUG_PRINTF ("Success");
  return;
}

void pimdmIgmpV2JoinLeaveProcess(L7_uchar8 family,L7_uint32 index,L7_uint32 rtrIfNum,L7_uint32 mode)
{
  mgmdMrpEventInfo_t  mgmdGrpInfo;
  pimdmCB_t *pimdmCB;

  pimdmCB = (pimdmCB_t *)pimdmMapProtocolCtrlBlockGet(family);

  if(pimdmCB == (pimdmCB_t *)L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Invalid CB (NULLPTR) \n");
    return;
  }
  memset(&mgmdGrpInfo,0,sizeof(mgmdMrpEventInfo_t));
  mgmdGrpInfo.rtrIfNum = rtrIfNum;
  if(mode == 1)
    mgmdGrpInfo.mode= MGMD_FILTER_MODE_EXCLUDE;
  else if(mode ==2)
    mgmdGrpInfo.mode= MGMD_FILTER_MODE_INCLUDE;


  inetCopy(&mgmdGrpInfo.group,&pimdmDebugGrpAddr[index]);
  mgmdGrpInfo.numSrcs = 0;

  if(pimdmEventProcess((void*)pimdmCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                &mgmdGrpInfo) != L7_SUCCESS)
  {
    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"pimdmEventProcess failed");
    return;
  }

  PIMDM_TRACE(PIMDM_DEBUG_API,"pimdmEventProcess success");

}


void pimdmIgmpV3JoinLeaveProcess(L7_uchar8 family,L7_uint32 index,L7_uint32 rtrIfNum,L7_uint32 action,L7_uint32 mode)
{
  mgmdMrpEventInfo_t  mgmdGrpInfo;
  pimdmCB_t *pimdmCB;

  pimdmCB = (pimdmCB_t *)pimdmMapProtocolCtrlBlockGet(family);

  if(pimdmCB == (pimdmCB_t *)L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Invalid CB (NULLPTR) \n");
    return;
  }
  memset(&mgmdGrpInfo,0,sizeof(mgmdMrpEventInfo_t));
  mgmdGrpInfo.rtrIfNum = rtrIfNum;
  if(mode == 1)
    mgmdGrpInfo.mode= MGMD_FILTER_MODE_EXCLUDE;
  else if(mode ==2)
    mgmdGrpInfo.mode= MGMD_FILTER_MODE_INCLUDE;

  inetCopy(&mgmdGrpInfo.group,&pimdmDebugGrpAddr[index]);
  inetCopy(&mgmdGrpInfo.sourceList[0]->sourceAddr,&pimdmDebugSrcAddr[index]);
  if(action == 1)
  {
    mgmdGrpInfo.sourceList[0]->action = MGMD_SOURCE_ADD;
  }
  else if(action ==2)
  {
    mgmdGrpInfo.sourceList[0]->action = MGMD_SOURCE_DELETE;
  }
  mgmdGrpInfo.sourceList[0]->filterMode=  MGMD_FILTER_MODE_EXCLUDE;

  mgmdGrpInfo.numSrcs = 1;

  if(pimdmEventProcess((void*)pimdmCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                                &mgmdGrpInfo) != L7_SUCCESS)
  {

    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"pimdmEventProcess failed");
    return;
  }

  PIMDM_TRACE(PIMDM_DEBUG_API,"pimdmEventProcess success");

}

void pimdmAdminScopeAddDel(L7_uchar8 family,L7_uint32 index,L7_uint32 rtrIfNum,L7_uint32 mode)
{
  mcastAdminMsgInfo_t abNode;
  pimdmCB_t *pimdmCB;
  L7_uint32 mask= 0xffffff00;

  pimdmCB = (pimdmCB_t *)pimdmMapProtocolCtrlBlockGet(family);

  if(pimdmCB == (pimdmCB_t *)L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Invalid CB (NULLPTR) \n");
    return;
  }
  memset(&abNode,0,sizeof(mcastAdminMsgInfo_t));
  abNode.intIfNum = rtrIfNum;
  abNode.family = family;
  if(mode == 1)
    abNode.mode= L7_ENABLE;
  else if(mode ==2)
    abNode.mode= L7_DISABLE;

  inetCopy(&abNode.groupAddress,&pimdmDebugGrpAddr[index]);
  inetAddressSet(L7_AF_INET,(void *)&mask,&abNode.groupMask);
  if(pimdmEventProcess((void*)pimdmCB, MCAST_EVENT_ADMINSCOPE_BOUNDARY,
                                &abNode) != L7_SUCCESS)
  {

    PIMDM_TRACE(PIMDM_DEBUG_FAILURE,"pimdmEventProcess failed");
    return;
  }

  PIMDM_TRACE(PIMDM_DEBUG_API,"pimdmEventProcess success");

}

static void pimdmDebugV6StringToAddr ( L7_uint32 index,
                                       L7_in6_addr_t * Ipv6Addr )
{
  switch(index)
  {
    case 0:
      osapiInetPton( L7_AF_INET6,
                     PIMDM_IPV6_INADDR_ALL_PIM_ROUTERS,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 1:
      osapiInetPton( L7_AF_INET6,
                     PIMDM_IPV6_INADDR_PIM_ROUTER1,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 2:
      osapiInetPton( L7_AF_INET6,
                     PIMDM_IPV6_INADDR_PIM_ROUTER2,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 3:
      osapiInetPton( L7_AF_INET6,
                     PIMDM_IPV6_INADDR_PIM_ROUTER3,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 4:
      osapiInetPton( L7_AF_INET6,
                     PIMDM_IPV6_INADDR_PIM_ROUTER4,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    default:
      return;
  }
  return;
}
static void pimdmDebugV6SrcStringToAddr ( L7_uint32 index,
                                          L7_in6_addr_t * Ipv6Addr )
{
  switch(index)
  {
    case 0:
      osapiInetPton( L7_AF_INET6,
                     PIMDM_IPV6_SRC_ADDR,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 1:
      osapiInetPton( L7_AF_INET6,
                     PIMDM_IPV6_SRC_ADDR1,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 2:
      osapiInetPton( L7_AF_INET6,
                     PIMDM_IPV6_SRC_ADDR2,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 3:
      osapiInetPton( L7_AF_INET6,
                     PIMDM_IPV6_SRC_ADDR3,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 4:
      osapiInetPton( L7_AF_INET6,
                     PIMDM_IPV6_SRC_ADDR4,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    default:
      return;
  }
  return;
}

void
pimdmStubInvokeStRfrFSM (L7_uchar8 family,
                         L7_uint32 index,
                         L7_uint32 state,
                         L7_uint32 event)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t *pimdmMrtEntry = L7_NULLPTR;
  pimdmStrfrEventInfo_t eventInfo;
  L7_uchar8 src[40],grp[40];
  L7_in6_addr_t Ipv6Addr;

  memset(&eventInfo,0,sizeof(pimdmStrfrEventInfo_t));
  
  pimdmCB = pimdmMapProtocolCtrlBlockGet(family);

  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("STUB: pimdmMapProtocolCtrlBlockGet failed");
    return;
  }

  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimdmDebugV6StringToAddr( index, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimdmDebugGrpAddr[index] );

    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimdmDebugV6SrcStringToAddr(index, &Ipv6Addr);
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimdmDebugSrcAddr[index] );


  }

  PIMDM_DEBUG_PRINTF ("STUB: Src - %s, Grp - %s",inetAddrPrint(&pimdmDebugSrcAddr[index],src),inetAddrPrint(&pimdmDebugGrpAddr[index],grp)); 
  pimdmMrtEntry = pimdmMrtEntryAdd(pimdmCB,&pimdmDebugSrcAddr[index],
                   &pimdmDebugGrpAddr[index],index);

  if(pimdmMrtEntry == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("STUB: pimdmMrtEntryAdd failed");
    return;
  } 
  eventInfo.ttl = 5;
  pimdmMrtEntry->origStateInfo.origState = state;

  if (pimdmStateRfrFsmExecute (event, pimdmMrtEntry, &eventInfo) != L7_SUCCESS)
  {
    PIMDM_DEBUG_PRINTF ("STUB: pimdmStateRfrFsmExecute Failed");
    return;
  }

  PIMDM_DEBUG_PRINTF ( "STUB: pimdmStateRfrFsmExecute Success");
  return;
}

void
pimdmStubInvokeAsrtFSM (L7_uchar8 family,
                        L7_uint32 index,
                        L7_uint32 state,
                        L7_uint32 event)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  pimdmMrtEntry_t *pimdmMrtEntry = L7_NULLPTR;
  pimdmAssertEventInfo_t eventInfo;
  L7_uchar8 src[40],grp[40];
  L7_in6_addr_t Ipv6Addr;

  memset(&eventInfo,0,sizeof(pimdmStrfrEventInfo_t));

  pimdmCB = pimdmMapProtocolCtrlBlockGet(family);

  if(pimdmCB == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("STUB: pimdmMapProtocolCtrlBlockGet failed");
    return;
  }

  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimdmDebugV6StringToAddr( index, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimdmDebugGrpAddr[index] );

    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimdmDebugV6SrcStringToAddr(index, &Ipv6Addr);
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimdmDebugSrcAddr[index] );


  }

  PIMDM_DEBUG_PRINTF ("STUB: Src - %s, Grp - %s",inetAddrPrint(&pimdmDebugSrcAddr[index],src),inetAddrPrint(&pimdmDebugGrpAddr[index],grp)); 
  pimdmMrtEntry = pimdmMrtEntryAdd(pimdmCB,&pimdmDebugSrcAddr[index],
                   &pimdmDebugGrpAddr[index],index);

  if(pimdmMrtEntry == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("STUB: pimdmMrtEntryAdd failed");
    return;
  } 

  pimdmMrtEntry->downstrmStateInfo[index].downstrmAssertInfo.assertState = state;

  if (event == 0)
 	{
    eventInfo.rtrIfIndex = index;
 	}
  else if (event == 1)
 	{
    eventInfo.rtrIfIndex = index;
    eventInfo.assertMetric = 10;
    eventInfo.assertPref = 5;
    eventInfo.interval = PIMDM_DEFAULT_ASSERT_TIMEOUT;
    eventInfo.type = PIMDM_CTRL_PKT_ASSERT;
    inetCopy (&(eventInfo.assertAddr), &pimdmDebugSrcAddr[2]);
 	}
  else if (event == 2)
 	{
    eventInfo.rtrIfIndex = index;
    eventInfo.assertMetric = 10;
    eventInfo.assertPref = 5;
    eventInfo.interval = PIMDM_DEFAULT_ASSERT_TIMEOUT;
    eventInfo.type = PIMDM_CTRL_PKT_ASSERT;
    inetCopy (&(eventInfo.assertAddr), &pimdmDebugSrcAddr[2]);
 	}
  else if (event == 3)
 	{
    eventInfo.rtrIfIndex = index;
    eventInfo.assertMetric = 10;
    eventInfo.assertPref = 5;
    eventInfo.interval = PIMDM_DEFAULT_ASSERT_TIMEOUT;
    eventInfo.type = PIMDM_CTRL_PKT_ASSERT;
    inetCopy (&(eventInfo.assertAddr), &pimdmDebugSrcAddr[2]);
 	}
  else if (event == 4)
 	{
    eventInfo.rtrIfIndex = index;
    eventInfo.interval = (3 * 50);
  }
  else if (event == 5)
 	{
    eventInfo.rtrIfIndex = index;
 	}
  else if (event == 6)
 	{
    eventInfo.rtrIfIndex = index;
 	}
  else if (event == 7)
 	{
    eventInfo.rtrIfIndex = index;
 	}
  else if (event == 8)
 	{
    eventInfo.rtrIfIndex = index;
 	}
  else if (event == 9)
 	{
    eventInfo.rtrIfIndex = index;
    eventInfo.type = PIMDM_CTRL_PKT_GRAFT;
    inetCopy (&(eventInfo.assertAddr), &pimdmDebugSrcAddr[2]);
 	}

  if (pimdmAssertFsmExecute (event, pimdmMrtEntry, &eventInfo) != L7_SUCCESS)
  {
    PIMDM_DEBUG_PRINTF ("STUB: pimdmAssertFsmExecute Failed");
    return;
  }

  PIMDM_DEBUG_PRINTF ( "STUB: pimdmAssertFsmExecute Success");
  return;
}

void
pimdmStubV6MgmdV2JoinLeaveMsgSend (L7_uchar8 family,
                                   L7_uint32 index,
                                   L7_uint32 rtrIfNum,
                                   L7_uint32 mode)
{
  mgmdMrpEventInfo_t  mgmdGrpInfo;
  pimdmCB_t *pimdmCB;
  L7_in6_addr_t Ipv6Addr;

  pimdmCB = (pimdmCB_t *)pimdmMapProtocolCtrlBlockGet(family);

  if(pimdmCB == (pimdmCB_t *)L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Invalid CB (NULLPTR) \n");
    return;
  }
  memset(&mgmdGrpInfo,0,sizeof(mgmdMrpEventInfo_t));
  mgmdGrpInfo.rtrIfNum = rtrIfNum;
  if(mode == 1)
    mgmdGrpInfo.mode= MGMD_FILTER_MODE_EXCLUDE;
  else if(mode ==2)
    mgmdGrpInfo.mode= MGMD_FILTER_MODE_INCLUDE;

  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
  pimdmDebugV6StringToAddr( index, &Ipv6Addr );        
  inetAddressSet(L7_AF_INET6, (void*)&Ipv6Addr, &pimdmDebugGrpAddr[index]);

  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
  pimdmDebugV6SrcStringToAddr(index, &Ipv6Addr);
  inetAddressSet(L7_AF_INET6, (void*)&Ipv6Addr, &pimdmDebugSrcAddr[index]);

  inetCopy(&mgmdGrpInfo.group,&pimdmDebugGrpAddr[index]);
  mgmdGrpInfo.numSrcs = 0;

  if(pimdmEventProcess((void*)pimdmCB, MCAST_EVENT_MGMD_GROUP_UPDATE,
                            &mgmdGrpInfo) != L7_SUCCESS)
  {
    PIMDM_DEBUG_PRINTF ("pimdmEventProcess failed");
    return;
  }
}

void
pimdmStubV6MfcNoCacheMsgSend (L7_uchar8 family,
                              L7_uint32 index,
                              L7_uint32 rtrIfNum)
{
  pimdmCB_t *pimdmCB;
  L7_in6_addr_t Ipv6Addr;
  mfcEntry_t mfcEntry;

  pimdmCB = (pimdmCB_t *)pimdmMapProtocolCtrlBlockGet(family);

  if(pimdmCB == (pimdmCB_t *)L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Invalid CB (NULLPTR) \n");
    return;
  }
  memset(&mfcEntry, 0, sizeof (mfcEntry_t));

  memset (&Ipv6Addr, 0, sizeof (L7_in6_addr_t));
  pimdmDebugV6StringToAddr (index, &Ipv6Addr );        
  inetAddressSet (L7_AF_INET6, (void*) &Ipv6Addr, &pimdmDebugGrpAddr[index]);
  inetCopy (&(mfcEntry.group), &pimdmDebugGrpAddr[index]);

  memset (&Ipv6Addr, 0, sizeof (L7_in6_addr_t));
  pimdmDebugV6SrcStringToAddr (index, &Ipv6Addr );        
  inetAddressSet (L7_AF_INET6, (void*) &Ipv6Addr, &pimdmDebugSrcAddr[index]);
  inetCopy (&(mfcEntry.source), &pimdmDebugSrcAddr[index]);

  mfcEntry.iif = rtrIfNum;
  mfcEntry.dataTTL = 6;

  if (pimdmEventProcess ((void*)pimdmCB, MCAST_MFC_NOCACHE_EVENT,
                        &mfcEntry) != L7_SUCCESS)
  {
    PIMDM_DEBUG_PRINTF ("pimdmEventProcess failed");
    return;
  }
}

void
pimdmStubV6MfcWrongIfMsgSend (L7_uchar8 family,
                              L7_uint32 index,
                              L7_uint32 rtrIfNum)
{
  pimdmCB_t *pimdmCB;
  L7_in6_addr_t Ipv6Addr;
  mfcEntry_t mfcEntry;

  pimdmCB = (pimdmCB_t *)pimdmMapProtocolCtrlBlockGet(family);

  if(pimdmCB == (pimdmCB_t *)L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Invalid CB (NULLPTR) \n");
    return;
  }
  memset(&mfcEntry, 0, sizeof (mfcEntry_t));

  memset (&Ipv6Addr, 0, sizeof (L7_in6_addr_t));
  pimdmDebugV6StringToAddr (index, &Ipv6Addr );        
  inetAddressSet (L7_AF_INET6, (void*) &Ipv6Addr, &pimdmDebugGrpAddr[index]);
  inetCopy (&(mfcEntry.group), &pimdmDebugGrpAddr[index]);

  memset (&Ipv6Addr, 0, sizeof (L7_in6_addr_t));
  pimdmDebugV6SrcStringToAddr (index, &Ipv6Addr );        
  inetAddressSet (L7_AF_INET6, (void*) &Ipv6Addr, &pimdmDebugSrcAddr[index]);
  inetCopy (&(mfcEntry.source), &pimdmDebugSrcAddr[index]);

  mfcEntry.iif = rtrIfNum;

  if (pimdmEventProcess((void*)pimdmCB, MCAST_MFC_WRONGIF_EVENT,
                        &mfcEntry) != L7_SUCCESS)
  {
    PIMDM_DEBUG_PRINTF ("pimdmEventProcess failed");
    return;
  }
}

void
pimdmStubV6AdminScopeMsgSend (L7_uchar8 family,
                              L7_uint32 index,
                              L7_uint32 rtrIfNum,
                              L7_uint32 mode)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  mcastAdminMsgInfo_t asbNode;
  L7_in6_addr_t Ipv6Addr;

  pimdmCB = (pimdmCB_t *)pimdmMapProtocolCtrlBlockGet(family);

  if(pimdmCB == (pimdmCB_t *)L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Invalid CB (NULLPTR) \n");
    return;
  }

  memset (&asbNode, 0, sizeof (mcastAdminMsgInfo_t));
  asbNode.intIfNum = rtrIfNum;
  asbNode.family = family;

  if ((mode != 1) && (mode != 2))
  {
    PIMDM_DEBUG_PRINTF ("Invalid Mode Specified.\n");
    return;
  }

  ((mode == 1) ? (asbNode.mode = L7_ENABLE) : (asbNode.mode = L7_DISABLE));

  memset (&Ipv6Addr, 0, sizeof (L7_in6_addr_t));
  pimdmDebugV6StringToAddr (index, &Ipv6Addr );        
  inetAddressSet (L7_AF_INET6, (void*) &Ipv6Addr, &pimdmDebugGrpAddr[index]);
  inetCopy (&asbNode.groupAddress, &pimdmDebugGrpAddr[index]);

  if (pimdmEventProcess ((void*)pimdmCB, MCAST_EVENT_ADMINSCOPE_BOUNDARY,
                        &asbNode) != L7_SUCCESS)
  {

    PIMDM_DEBUG_PRINTF ("pimdmEventProcess failed");
    return;
  }

  PIMDM_DEBUG_PRINTF ("pimdmEventProcess success");

}

void
pimdmDebugDummy (void)
{
  PIMDM_DEBUG_PRINTF ("Dummy Routine.\n");
  return;
}


/**************************************** UT Stubs End ***********************/
