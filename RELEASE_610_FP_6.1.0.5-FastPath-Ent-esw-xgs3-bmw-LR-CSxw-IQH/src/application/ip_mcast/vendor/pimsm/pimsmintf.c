/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmIntf.c
*
* @purpose  All interface related APIs/operations.
*     
* @component pimsm
*
* @comments     
*     
* @create 01/01/2006
*     
* @author vmurali/dsatyanarayana
* @end
*
**********************************************************************/
#include "pimsmdefs.h"
#include "pimsmmacros.h"
#include "l7_pimsm_api.h"
#include "l7apptimer_api.h"
#include "buff_api.h"
#include "pimsmintf.h"
#include "pimsmtimer.h"
#include "pimsmcontrol.h"
#include "pimsmneighbor.h"
#include "pimsmmain.h"
#include "pimsmbsr.h"
#include "pimsmwrap.h"
#include "mcast_wrap.h"

/******************************************************************************
* @purpose  Return the interface block for the given interface index
*
* @param    pimsmCb       @b{(input)}  control block  
* @param    rtrIfNum      @b{(input)} router interface number
* @param    ppIntfEntry   @b{(output)}  pointer to interface entry
*
* @returns  void
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmIntfEntryGet(pimsmCB_t *pimsmCb, L7_uint32 rtrIfNum,
                          pimsmInterfaceEntry_t **ppIntfEntry)
{
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"CB recvd is NULLPTR"); 
    return L7_FAILURE;
  }
  
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
       "wrong  rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;   
  }
  pIntfEntry = &pimsmCb->intfList[rtrIfNum]; 
  if(pIntfEntry->pimsmInterfaceIfIndex != rtrIfNum)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_MAX,"intf %d is not Up",rtrIfNum);
    return L7_FAILURE;
  }
  if (ppIntfEntry != L7_NULLPTR) 
  {
  *ppIntfEntry = pIntfEntry;
  }
  return L7_SUCCESS;   
}

/******************************************************************************
* @purpose  interface down processing for PIMSM MRT and BSR/RP table
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    rtrIfNum    @b{(input)} router interface number
*
* @returns  L7_SUCCESS
*
* @comments     
*     
* @end
******************************************************************************/
static L7_RC_t pimsmUpdateMRTTreeIntfDown(pimsmCB_t *pimsmCb, L7_uint32 rtrIfNum)
{
  
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Enter"); 

  pimsmStarGTreeIntfDownUpdate(pimsmCb, rtrIfNum);
  pimsmSGRptTreeIntfDownUpdate(pimsmCb, rtrIfNum);
  pimsmSGTreeIntfDownUpdate(pimsmCb, rtrIfNum);
  pimsmStarStarRpTreeIntfDownUpdate(pimsmCb, rtrIfNum);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Exit"); 
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Verify if the given interface is the DR
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    rtrIfNum    @b{(input)} router interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments     
*     
  bool
  I_am_DR(I) {
     return DR(I) == me
  }
* @end
******************************************************************************/
L7_BOOL pimsmIAmDR(pimsmCB_t *pimsmCb, L7_uint32 rtrIfNum)
{
  pimsmInterfaceEntry_t * pIntfEntry = L7_NULLPTR;
  L7_RC_t rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Enter ");
  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"CB recvd is NULLPTR"); 
    return L7_FALSE;
  }
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"IntfEntry is NULLPTR");  
    return L7_FALSE;
  }

  return pIntfEntry->pimsmIamDR; 
}

/******************************************************************************
* @purpose  Compare DR metrics (priority and addr)
*
* @param    pimsmCb      @b{(input)}  control block  
* @param    pNbrEntry1   @b{(input)} neighbor entry
* @param    pNbrEntry2   @b{(input)} neighbor entry
* @param    pIntfEntry   @b{(input)} interface entry
*
* @returns  L7_TRUE if a is better than b otherwise 
* @returns  L7_FALSE
*
* @comments     
*     
  bool
  dr_is_better(a,b,I) {
      if( there is a neighbor n on I for which n.dr_priority_present
              is false ) {
          return a.primary_ip_address > b.primary_ip_address
      } else {
          return ( a.dr_priority > b.dr_priority ) OR
              ( a.dr_priority == b.dr_priority AND
                   a.primary_ip_address > b.primary_ip_address )
      }
  }
* @end
******************************************************************************/
static L7_BOOL pimsmDRIsBetter(pimsmCB_t *pimsmCb, pimsmNeighborEntry_t * pNbrEntry1, 
                 pimsmNeighborEntry_t * pNbrEntry2, pimsmInterfaceEntry_t * pIntfEntry)
{
  pimsmNeighborEntry_t  * pNbrEntry;
  L7_RC_t           rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Enter ");

  rc = pimsmNeighborGetFirst(pimsmCb, pIntfEntry, &pNbrEntry);
  while(rc == L7_SUCCESS)
  {
    /* pimsmNeighborDRPriority == 0 means 
       pimsmNeighborDRPriorityPresent is FALSE */
    if(pNbrEntry->pimsmNeighborDRPriority == 0)
    {
      if(L7_INET_ADDR_COMPARE(&pNbrEntry1->pimsmNeighborAddrList.pimsmPrimaryAddress,
                              &pNbrEntry2->pimsmNeighborAddrList.pimsmPrimaryAddress) > 0)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Exit:TRUE ");
        return L7_TRUE;
      }
      else
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Exit:FALSE ");
        return L7_FALSE;
      }
    }
    rc = pimsmNeighborNextGet(pimsmCb, pIntfEntry, pNbrEntry, &pNbrEntry);
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,"Dr Priority is %d\r\n",
      pNbrEntry2->pimsmNeighborDRPriority);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,"Neighbor Address is",
      &pNbrEntry2->pimsmNeighborAddrList.pimsmPrimaryAddress);
  if((pNbrEntry1->pimsmNeighborDRPriority > pNbrEntry2->pimsmNeighborDRPriority) ||
     ((pNbrEntry1->pimsmNeighborDRPriority == pNbrEntry2->pimsmNeighborDRPriority) &&
      ((L7_INET_ADDR_COMPARE(&pNbrEntry1->pimsmNeighborAddrList.pimsmPrimaryAddress,
                             &pNbrEntry2->pimsmNeighborAddrList.pimsmPrimaryAddress)> 0))))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Exit:TRUE ");
    return L7_TRUE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"EXIT: False "); 
  return L7_FALSE;
}


/******************************************************************************
* @purpose  Update the DR information in the Interface structure
*
* @param    pimsmCb      @b{(input)}  control block  
* @param    pIntfEntry   @b{(input)} interface entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
  host
  DR(I) {
      dr = me
      for each neighbor on interface I {
          if ( dr_is_better( neighbor, dr, I ) == TRUE ) {
              dr = neighbor
          }
      }
      return dr
  }
  
* NOTES: When a neighbor is deleted, it could be the DR on that interface. So we need
* to update this information in the information structure.  
* @end
******************************************************************************/
L7_RC_t pimsmIntfDRUpdate(pimsmCB_t *pimsmCb, pimsmInterfaceEntry_t *pIntfEntry)
{
  pimsmNeighborEntry_t    *pNbrEntry, drNbrEntry;
  L7_RC_t           rc;
  L7_uint32 rtrIfNum;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Enter ");

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"NULLPTR rcvd as input ");  
    return L7_FAILURE;    
  }

  if(pIntfEntry == (pimsmInterfaceEntry_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"IntfEntry is NULLPTR");  
    return L7_FAILURE;
  }
  rtrIfNum = pIntfEntry->pimsmInterfaceIfIndex ;
  memset(&drNbrEntry, 0 , sizeof(pimsmNeighborEntry_t));
  /*Copy my inet addr */
  inetCopy(&drNbrEntry.pimsmNeighborAddrList.pimsmPrimaryAddress, 
         &pIntfEntry->pimsmInterfaceAddr);
  /*Copy my priority */
  rc = pimsmMapProtocolInterfaceDRPriorityGet(pimsmCb->family,rtrIfNum,
                 &drNbrEntry.pimsmNeighborDRPriority);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, 
                "pimsmMapProtocolInterfaceDRPriorityGet() failed for rtrIfNum = %d",
                rtrIfNum);
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,"Dr priority is %d\r\n",
              drNbrEntry.pimsmNeighborDRPriority);
  /*Check with all my neighbors */
  rc = pimsmNeighborGetFirst(pimsmCb, pIntfEntry, &pNbrEntry);
  while(rc == L7_SUCCESS)
  {
    if(pimsmDRIsBetter(pimsmCb, pNbrEntry, &drNbrEntry, pIntfEntry) == L7_TRUE)
    {
      inetCopy(&drNbrEntry.pimsmNeighborAddrList.pimsmPrimaryAddress, 
               &pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress);
      drNbrEntry.pimsmNeighborDRPriority = pNbrEntry->pimsmNeighborDRPriority;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,
         "Neighbor DR priority is pNbrEntry->pimsmNeighborDRPriority");
    }
    rc = pimsmNeighborNextGet(pimsmCb, pIntfEntry, pNbrEntry, &pNbrEntry);
  }

  if(L7_INET_IS_ADDR_EQUAL(&pIntfEntry->pimsmInterfaceDR, 
                            &drNbrEntry.pimsmNeighborAddrList.pimsmPrimaryAddress) != L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,"DR has Changed");
    inetCopy(&pIntfEntry->pimsmInterfaceDR, &drNbrEntry.pimsmNeighborAddrList.pimsmPrimaryAddress); 
    /* If the DR changed, Check with (S,G) Register FSMs (Could Register) */
    pimsmProcessDRChange(pimsmCb, pIntfEntry->pimsmInterfaceIfIndex);
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,"Dr priority is %d",
              drNbrEntry.pimsmNeighborDRPriority);
  pIntfEntry->pimsmInterfaceDRPriority = drNbrEntry.pimsmNeighborDRPriority;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"EXIT ");
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  initialize all interfaces
*
* @param    pimsmCb     @b{(input)}  control block  
*
* @returns  L7_SUCCESS
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmIntfInit(pimsmCB_t *pimsmCb)
{
  L7_uint32 rtrIfNum;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Enter ");
  
  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return L7_FAILURE;
  }

  for(rtrIfNum = 0; rtrIfNum < MCAST_MAX_INTERFACES; ++rtrIfNum)
  {
    pIntfEntry = &pimsmCb->intfList[rtrIfNum];
    memset(pIntfEntry, 0 , sizeof(pimsmInterfaceEntry_t));
    pIntfEntry->pimsmInterfaceIfIndex = MCAST_MAX_INTERFACES;
  }
  /*Enable the Register Interface as default*/
  pIntfEntry = &pimsmCb->intfList[PIMSM_REGISTER_INTF_NUM];
  pIntfEntry->pimsmInterfaceIfIndex = PIMSM_REGISTER_INTF_NUM;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG, "Intf Init successful");
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  de-initialized all interfaces
*
* @param    pimsmCb     @b{(input)}  control block  
*
* @returns  L7_SUCCESS
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmIntfDeInit(pimsmCB_t *pimsmCb)
{
  L7_uint32 rtrIfNum;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,"Enter \n");

  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return L7_FAILURE;
  }

  for(rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; ++rtrIfNum)
  {
    pimsmIntfDown(pimsmCb, rtrIfNum);
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,"Exit ");
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  interface hello-timer expiry handler
*
* @param    pParam     @b{(input)}  timer-data handle  
*
* @returns  void
*
* @comments     
*     
* @end
******************************************************************************/
static void pimsmIntfHelloTimerExpiresHandler(void *pParam)
{
  L7_uint32 rtrIfNum;
  L7_int32      handle = (L7_int32)pParam;  
  pimsmTimerData_t *pTimerData;
  pimsmCB_t * pimsmCb;
  pimsmInterfaceEntry_t  * pIntfEntry = L7_NULLPTR;
  L7_RC_t rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Enter ");  

  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  } 
  rtrIfNum = pTimerData->rtrIfNum;
  pimsmCb = pTimerData->pimsmCb;

  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return;
  }


  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"IntfEntry is NULLPTR");  
    return ;
  }
  if (pIntfEntry->pimsmHelloTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMSM pimsmHelloTimer is NULL, But Still Expired");
    return;
  }
  pIntfEntry->pimsmHelloTimer = L7_NULLPTR;
 
  if (pIntfEntry->pimsmInterfaceHelloInterval == 0)
  {
    /* A Hello Interval value of zero represents an 'infinite' interval,
     * and indicates that periodic PIM Hello messages should not be
     * sent on this interface.
     */
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO, "Hello Timer Value is 0.  Hello Pkts will"
                "never be sent out of the interface");  
    return;
  }

  pimsmHelloSend(pimsmCb, rtrIfNum,  &pIntfEntry->pimsmInterfaceAddr,
         pIntfEntry->pimsmInterfaceHelloHoldtime); 

  pTimerData = &pIntfEntry->pimsmHelloTimerParams;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  pTimerData->pimsmCb = pimsmCb;
  pTimerData->rtrIfNum = rtrIfNum;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmIntfHelloTimerExpiresHandler,
                            (void*)pIntfEntry->pimsmHelloTimerHandle,
                            pIntfEntry->pimsmInterfaceHelloInterval,
                            &(pIntfEntry->pimsmHelloTimer),
                            "SM-HT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, "Inteface pimsmHelloTimer TimerAdd Failed");
    return ;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Exit");  
}
/******************************************************************************
* @purpose  Interface UP-event action-routine
*
* @param    pimsmCb         @b{(input)}  control block  
* @param    rtrIfNum        @b{(input)} router interface number
* @param    intIfNum        @b{(input)} internal interface number
* @param    pIntfAddr       @b{(input)}  interface address  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     this function should be called in following cases
      1) interface mode is up
      2) interface pimsm mode is enabled
      3) ip address is changed.
* @end
******************************************************************************/ 
L7_RC_t pimsmIntfUp(pimsmCB_t *pimsmCb, L7_uint32 rtrIfNum, 
         L7_uint32 intIfNum, L7_inet_addr_t *pIntfAddr)
{
  pimsmTimerData_t * pTimerData;
  L7_uint32 helloIntvl;
  pimsmInterfaceEntry_t *pIntfEntry  = L7_NULLPTR;
  L7_RC_t rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Enter "); 

  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return L7_FAILURE;
  }
  
  if(pimsmCb->isPimsmEnabled != L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"Got Intf Up (rtrIfNum %d) when PIM-SM not initialized ",
      rtrIfNum);
    return L7_FAILURE;
  }

  if(rtrIfNum >= MCAST_MAX_INTERFACES)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"Wrong rtrIfNum = %d", rtrIfNum);  
    return L7_FAILURE;
  }
  
  if(inetIsAddressZero(pIntfAddr) == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
      "Zero Interface Addr on rtrIfNum %d", rtrIfNum);  
    return L7_FAILURE;
  }
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if(rc == L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "Interface %d is already up", rtrIfNum);  
    return L7_SUCCESS;
  }
  
  pIntfEntry = &pimsmCb->intfList[rtrIfNum];
  pIntfEntry->pimsmHelloTimerHandle = 
  handleListNodeStore(pimsmCb->handleList,
                        (void*)&pIntfEntry->pimsmHelloTimerParams);  

  pIntfEntry->pimsmInterfaceIfIndex = rtrIfNum;
  pIntfEntry->intIfNum = intIfNum;
  pIntfEntry->pimsmLANPruneDelayPresent = L7_TRUE;
  pIntfEntry->pimsmPropagationDelay = PIMSM_DEFAULT_PROPAGATION_DELAY_MSECS;
  pIntfEntry->pimsmOverrideInterval = PIMSM_DEFAULT_OVERRIDE_INTERVAL_MSECS;

  if(rtrIfNum == PIMSM_REGISTER_INTF_NUM)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"rtrIfNum is Register-Interface");  
    /* Nothing much to do for this interface*/
    return L7_SUCCESS;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Family is %d ", pimsmCb->family);                        
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,
                   "Intf IP Addr is ", pIntfAddr);

  inetCopy(&pIntfEntry->pimsmInterfaceAddr, pIntfAddr);
  inetCopy(&pIntfEntry->pimsmInterfaceDR, pIntfAddr);
  pIntfEntry->pimsmIamDR = L7_TRUE;
  
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,
                   "DR Addr is ", &pIntfEntry->pimsmInterfaceDR);

  
  /* Enable PIMSM Control packet on this interface for IPv6 family. */
  if(L7_AF_INET6 == pimsmCb->family)
  {
    pimsmMfcIntfEventQueue(pimsmCb, rtrIfNum, L7_ENABLE);
    if (pimV6SockChangeMembership(rtrIfNum, intIfNum, L7_ENABLE, pimsmCb->sockFd)
                                  != L7_SUCCESS)
      {
		L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_FLEX_PIMSM_MAP_COMPONENT_ID, "PIMSMv6 Socket Memb'ship Enable "
				"Failed for rtrIfNum %d. Socket options Set to enable the reception of PIMv6 packets Failed."
				"As a result of this, the PIMv6 packets will not be received by the application. ", rtrIfNum);
      }      
  }
  /*Copy my priority */

  rc = pimsmMapProtocolInterfaceDRPriorityGet(pimsmCb->family,rtrIfNum,
             &pIntfEntry->pimsmInterfaceDRPriority);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, 
                "pimsmMapProtocolInterfaceDRPriorityGet() failed for rtrIfNum = %d",
                rtrIfNum);
    return L7_FAILURE;
  }

  if(pimsmNeighborInit(pimsmCb, pIntfEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_FATAL, 
                "Error: unable to create Nbr Linked List ");
    return L7_FAILURE;
  }
  helloIntvl = (1 + PIMSM_RANDOM() % PIMSM_DEFAULT_TRIGGERED_HELLO_DELAY);

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"rtrIfNum =%d",rtrIfNum);
  pTimerData = &pIntfEntry->pimsmHelloTimerParams;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  pTimerData->rtrIfNum = rtrIfNum;
  pTimerData->pimsmCb = pimsmCb;      
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmIntfHelloTimerExpiresHandler,
                            (void*)pIntfEntry->pimsmHelloTimerHandle,
                            helloIntvl,
                            &(pIntfEntry->pimsmHelloTimer),
                            "SM-HT2")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, 
                "Inteface pimsmHelloTimer TimerAdd Failed");
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"HelloTimerExpiresValue is %d ", helloIntvl);                       

  pIntfEntry->pimsmInterfaceGenerationIDValue = PIMSM_RANDOM();


  /* Post an event to MGMD to get the Group membership details for
   * this interface.
   */
  if (mgmdMapMRPGroupInfoGet (pimsmCb->family, rtrIfNum) != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION,  PIMSM_TRACE_ERROR,
                 "Failure in Posting the MGMD Info Get Event for rtrIfNum - %d", rtrIfNum);
    return L7_FAILURE;
  }

  if (pimsmCb->family == L7_AF_INET)
  {
    mcastMapMRPAdminScopeInfoGet(pimsmCb->family, rtrIfNum);
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Exit ");
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Interface DOWN-event action-routine
*
* @param    pimsmCb         @b{(input)}  control block  
* @param    rtrIfNum        @b{(input)} router interface number
* @param    intIfNum        @b{(input)} internal interface number
* @param    pIntfAddr       @b{(input)}  interface address  
*
* @returns  void
*
* @comments     
*     
*     this function should be called in following cases
      1) interface mode is down
      2) interface pimsm mode is disabled
* @end
******************************************************************************/
static L7_RC_t pimsmIntfCleanup(pimsmCB_t *pimsmCb, L7_uint32 rtrIfNum)
{
  pimsmInterfaceEntry_t *pIntfEntry  = L7_NULLPTR;
  L7_RC_t  rc;
  L7_uint32 timerHandle;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Enter ");

  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return L7_FAILURE;
  }  

  if(pimsmCb->isPimsmEnabled != L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"Got Intf Down (rtrIfNum %d) when PIM-SM not initialized ",
      rtrIfNum);
    return L7_FAILURE;
  }
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,
                "Interface %d is already down", rtrIfNum);  
    return L7_SUCCESS;
  }
  
  /* inform (eventually) the neighbors "I am going down" by sending
   * PIMSM_HELLO with holdtime=0 so someone else should become a DR.
   */
  pimsmHelloSend(pimsmCb, rtrIfNum,  
         &pIntfEntry->pimsmInterfaceAddr, 0);

  if(rtrIfNum != PIMSM_REGISTER_INTF_NUM)
  {
    pimsmUtilAppTimerCancel (pimsmCb, &(pIntfEntry->pimsmHelloTimer));
    pimsmNeighborDeInit(pimsmCb, pIntfEntry);
  }

  /* Disable PIMSM Control packet on this interface for IPv6 family. */
  if(L7_AF_INET6 == pimsmCb->family)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, 
                    "rtrIfNum is %d,intIfNum is %d",rtrIfNum,pIntfEntry->intIfNum);
     pimsmMfcIntfEventQueue(pimsmCb, rtrIfNum, L7_DISABLE);
    if(pimV6SockChangeMembership(rtrIfNum, pIntfEntry->intIfNum, 
      L7_DISABLE, pimsmCb->sockFd)!= L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_FLEX_PIMSM_MAP_COMPONENT_ID, "PIMSMv6 Socket Memb'ship Disable "
									                                  "Failed for rtrIfNum %d", rtrIfNum);
      }
   
  }
  /*interface should not be valid now */
  timerHandle = pIntfEntry->pimsmHelloTimerHandle; 
  memset(pIntfEntry, 0 , sizeof(pimsmInterfaceEntry_t)); 
  pIntfEntry->pimsmHelloTimerHandle = timerHandle; 
  pIntfEntry->pimsmInterfaceIfIndex = MCAST_MAX_INTERFACES; 

  handleListNodeDelete(pimsmCb->handleList,
                         &pIntfEntry->pimsmHelloTimerHandle);

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Exit ");
  return L7_SUCCESS;  
}

/******************************************************************************
* @purpose  Interface DOWN-event action-routine
*
* @param    pimsmCb         @b{(input)}  control block  
* @param    rtrIfNum        @b{(input)} router interface number
* @param    intIfNum        @b{(input)} internal interface number
* @param    pIntfAddr       @b{(input)}  interface address  
*
* @returns  void
*
* @comments     
*     
*     this function should be called in following cases
      1) interface mode is down
      2) interface pimsm mode is disabled
* @end
******************************************************************************/
L7_RC_t pimsmIntfDown(pimsmCB_t *pimsmCb, L7_uint32 rtrIfNum)
{
  L7_RC_t rc;   
  
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, L7_NULLPTR);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "Interface %d is already down", rtrIfNum);  
    return L7_SUCCESS;
  }
     
  pimsmRpIntfDown(pimsmCb, rtrIfNum);
  pimsmBsrIntfDown(pimsmCb, rtrIfNum);
  pimsmUpdateMRTTreeIntfDown(pimsmCb, rtrIfNum);

  pimsmIntfCleanup(pimsmCb, rtrIfNum);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Exit ");
  return L7_SUCCESS;  
}

/******************************************************************************
* @purpose  Get the neighbor entry struct
*
* @param    pimsmCb         @b{(input)}  control block  
* @param    rtrIfNum        @b{(input)} router interface number
* @param    pNbrAddr        @b{(input)}  neighbor address  
* @param    ppNbrEntry      @b{(output)} pointer to neighbor entry  
* @param    
*
* @returns  
*
* @comments      
*       
* @end
******************************************************************************/
L7_RC_t pimsmIntfNbrEntryGet(pimsmCB_t *pimsmCb,
                             L7_uint32 rtrIfNum,
                             L7_inet_addr_t *pNbrAddr,
                             pimsmNeighborEntry_t **ppNbrEntry)
{
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;
  L7_RC_t rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Enter ");     

  if(pimsmCb == L7_NULLPTR || pNbrAddr == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return L7_FAILURE;
  }  
  
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"IntfEntry is NULLPTR");  
    return L7_FAILURE;
  }

  rc = pimsmNeighborFind(pimsmCb, pIntfEntry, pNbrAddr, ppNbrEntry);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"pimsmNeighborFind() failed");
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Exit ");     
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  Increment the stats counter
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    rtrIfNum    @b{(input)}  router interface number
* @param    pktType     @b{(input)}  packet type
* @param    RxOrTx      @b{(input)}  received or transmitted or invalid
*
* @returns  void
*
* @comments     
*     
* @end
******************************************************************************/
void pimsmStatsIncrement(pimsmCB_t *pimsmCb, L7_uint32 rtrIfNum, 
                         L7_uchar8 pktType, pimsmPktRxTxMode_t RxOrTx)
{
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;  
  L7_RC_t rc;

  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return ;
  }
  
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Enter ");  
  rc = pimsmIntfEntryGet(pimsmCb,rtrIfNum,&pIntfEntry);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"Interface Entry is NULL");
    return;
  }

  switch(pktType)
  {
    case PIMSM_HELLO: 
      if(RxOrTx == PIMSM_RX)
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmHelloPktRx;
      }
      else if(RxOrTx == PIMSM_TX)
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmHelloPktTx;
      }
      else
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmInvalidPktRx;
      }
      break;
    case PIMSM_REGISTER: 
      if(RxOrTx == PIMSM_RX)
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmRegisterPktRx;
      }
      else if(RxOrTx == PIMSM_TX)
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmRegisterPktTx;
      }
      else
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmInvalidPktRx;
      }
      break;
    case PIMSM_REGISTER_STOP: 
      if(RxOrTx == PIMSM_RX)
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmRegisterStopPktRx;
      }
      else if(RxOrTx == PIMSM_TX)
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmRegisterStopPktTx;
      }
      else
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmInvalidPktRx;
      }
      break;
    case PIMSM_JOIN_PRUNE: 
      if(RxOrTx == PIMSM_RX)
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmJoinPrunePktRx;
      }
      else if(RxOrTx == PIMSM_TX)
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmJoinPrunePktTx;
      }
      else
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmInvalidPktRx;
      }
      break;          
    case PIMSM_BOOTSTRAP: 
      if(RxOrTx == PIMSM_RX)
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmBsrPktRx;
      }
      else if(RxOrTx == PIMSM_TX)
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmBsrPktTx;
      }
      else
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmInvalidPktRx;
      }
      break;          
    case PIMSM_ASSERT: 
      if(RxOrTx == PIMSM_RX)
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmAssertPktRx;
      }
      else if(RxOrTx == PIMSM_TX)
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmAssertPktTx;
      }
      else
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmInvalidPktRx;
      }
      break;
    case PIMSM_CAND_RP_ADV: 
      if(RxOrTx == PIMSM_RX)
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmCandRpPktRx;
      }
      else if(RxOrTx == PIMSM_TX)
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmCandRpPktTx;
      }
      else
      {
        ++pIntfEntry->pimsmPerIntfStats.pimsmInvalidPktRx;
      }
      break;
    default:
      ++pIntfEntry->pimsmPerIntfStats.pimsmInvalidPktRx;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Exit ");    
}

/******************************************************************************
* @purpose  Return the stats counter
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    rtrIfNum    @b{(input)}  router interface number
* @param    pktType     @b{(input)}  packet type
* @param    RxOrTx      @b{(input)}  received or transmitted or invalid
* @param    value       @b{(output)} value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
void pimsmGetStats(pimsmCB_t *pimsmCb, L7_uint32 rtrIfNum, L7_uchar8 pktType,
                   L7_uchar8 RxOrTx, L7_uint32 *value)    
{
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;  
  L7_RC_t rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Enter "); 
  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return;
  }
  
  rc = pimsmIntfEntryGet(pimsmCb,rtrIfNum,&pIntfEntry);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,"Interface Entry is NULL");
    return;
  }

  *value = 0;
  switch(pktType)
  {
    case PIMSM_HELLO: 
      if(RxOrTx == PIMSM_RX)
      {
        *value = pIntfEntry->pimsmPerIntfStats.pimsmHelloPktRx;
      }
      else if(RxOrTx == PIMSM_TX)
      {
        *value = pIntfEntry->pimsmPerIntfStats.pimsmHelloPktTx;
      }
      break;
    case PIMSM_REGISTER: 
      if(RxOrTx == PIMSM_RX)
      {
        *value = pIntfEntry->pimsmPerIntfStats.pimsmRegisterPktRx;
      }
      else if(RxOrTx == PIMSM_TX)
      {
        *value = pIntfEntry->pimsmPerIntfStats.pimsmRegisterPktTx;
      }
      break;
    case PIMSM_REGISTER_STOP: 
      if(RxOrTx == PIMSM_RX)
      {
        *value = pIntfEntry->pimsmPerIntfStats.pimsmRegisterStopPktRx;
      }
      else if(RxOrTx == PIMSM_TX)
      {
        *value = pIntfEntry->pimsmPerIntfStats.pimsmRegisterStopPktTx;
      }
      break;
    case PIMSM_JOIN_PRUNE: 
      if(RxOrTx == PIMSM_RX)
      {
        *value = pIntfEntry->pimsmPerIntfStats.pimsmJoinPrunePktRx;
      }
      else if(RxOrTx == PIMSM_TX)
      {
        *value = pIntfEntry->pimsmPerIntfStats.pimsmJoinPrunePktTx;
      }
      break;          
    case PIMSM_BOOTSTRAP: 
      if(RxOrTx == PIMSM_RX)
      {
        *value = pIntfEntry->pimsmPerIntfStats.pimsmBsrPktRx;
      }
      else if(RxOrTx == PIMSM_TX)
      {
        *value = pIntfEntry->pimsmPerIntfStats.pimsmBsrPktTx;
      }
      break;          
    case PIMSM_ASSERT: 
      if(RxOrTx == PIMSM_RX)
      {
        *value = pIntfEntry->pimsmPerIntfStats.pimsmAssertPktRx;
      }
      else if(RxOrTx == PIMSM_TX)
      {
        *value = pIntfEntry->pimsmPerIntfStats.pimsmAssertPktTx;
      }
      break;
    case PIMSM_CAND_RP_ADV: 
      if(RxOrTx == PIMSM_RX)
      {
        *value = pIntfEntry->pimsmPerIntfStats.pimsmCandRpPktRx;
      }
      else if(RxOrTx == PIMSM_TX)
      {
        *value = pIntfEntry->pimsmPerIntfStats.pimsmCandRpPktTx;
      }
      break;
    default:
      break;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG,"Exit ");    
}

/******************************************************************************
* @purpose  Post DR change event to FSMs
*
* @param    pimsmCb         @b{(input)}  control block  
* @param    rtrIfNum        @b{(input)} router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmProcessDRChange(pimsmCB_t * pimsmCb, L7_uint32 rtrIfNum)
{
  pimsmSGNode_t   *pSGNode, * pSGNodePrev;
  pimsmSGEntry_t  * pSGEntry;
  pimsmSGIEntry_t *     pSGIEntry;
  pimsmInterfaceEntry_t * pIntfEntry;
  L7_RC_t            rc, rc2 ;
  pimsmPerSGRegisterEventInfo_t registerEventInfo;
  pimsmPerIntfSGAssertEventInfo_t  sgAssertEventInfo;
  pimsmPerIntfStarGAssertEventInfo_t starGAssertEventInfo;
  pimsmStarGNode_t          *pStarGNode;
  pimsmStarGIEntry_t *      pStarGIEntry;
  interface_bitset_t oif;  
  pimsmStarStarRpNode_t* pStarStarRpNode = L7_NULLPTR;
  pimsmUpStrmStarGEventInfo_t UpStrmStarGEventInfo;
  pimsmUpStrmSGEventInfo_t UpStrmSGEventInfo;
  L7_BOOL  joinDesired =L7_FALSE; 

  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return L7_FAILURE;
  }                                   
  if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR, "IntfEntry is not available");
    return L7_FAILURE;
  }
  if(L7_INET_IS_ADDR_EQUAL(&pIntfEntry->pimsmInterfaceAddr,
       &pIntfEntry->pimsmInterfaceDR) == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_DEBUG, "I'm the DR :TRUE ");
    if (pIntfEntry->pimsmIamDR != L7_TRUE) 
    {
        pIntfEntry->pimsmIamDR = L7_TRUE;
        if (mgmdMapMRPGroupInfoGet (pimsmCb->family, rtrIfNum) != L7_SUCCESS)
        {
          PIMSM_TRACE (PIMSM_DEBUG_BIT_MGMD_INTERACTION,  PIMSM_TRACE_ERROR,
                          "Failure in Posting the MGMD Info Get Event for rtrIfNum - %d",
                          rtrIfNum);
          return L7_FAILURE;
        }
    }
  } 
  else 
  {
    pIntfEntry->pimsmIamDR = L7_FALSE;
  } 

  memset(&registerEventInfo, 0, sizeof(pimsmPerSGRegisterEventInfo_t));
  rc = pimsmSGFirstGet( pimsmCb, &pSGNode );
  memset(&oif, 0 , sizeof(interface_bitset_t));
  while(rc == L7_SUCCESS)
  {
    pSGEntry = &pSGNode->pimsmSGEntry;
    if ((pSGNode->flags & PIMSM_NULL_OIF) !=L7_NULL )
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                   "deleting MFC entry" );
      pimsmSGMFCUpdate(pimsmCb, pSGNode, MFC_DELETE_ENTRY, L7_FALSE);
    }
    if (pIntfEntry->pimsmIamDR != L7_TRUE) 
    {
      if(pSGNode->pimsmSGIEntry[rtrIfNum] != L7_NULLPTR)
      {
        pSGNode->pimsmSGIEntry[rtrIfNum]->pimsmSGILocalMembership = L7_FALSE;
      }
    }
    if(pSGEntry->pimsmSGRPFIfIndex == rtrIfNum)
    {
      rc2 = pimsmCouldRegister(pimsmCb, pSGNode);
      if(rc2 == L7_TRUE)
      {
        registerEventInfo.eventType = 
        PIMSM_REG_PER_S_G_SM_EVENT_COULD_REG_TRUE;
      }
      else if(rc2 == L7_FALSE)
      {
        registerEventInfo.eventType = 
        PIMSM_REG_PER_S_G_SM_EVENT_COULD_REG_FALSE;
      }
      pimsmPerSGRegisterExecute(pimsmCb, pSGNode, &registerEventInfo);
    }
    pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
    if(pSGIEntry != L7_NULLPTR)
    {
      #if PIMSM_TBD
      rc2 = pimsmSGIAssertTrackingDesired(pimsmCb, pSGNode, rtrIfNum);
      if(rc2 == L7_FALSE)
      {
        memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
        
        sgAssertEventInfo.eventType =
        PIMSM_ASSERT_S_G_SM_EVENT_ASSERT_TRACKING;
        sgAssertEventInfo.rtrIfNum = rtrIfNum;
        pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode, &sgAssertEventInfo);
      }
      #endif
      rc2 = pimsmSGICouldAssert(pimsmCb,pSGNode,rtrIfNum);
      if(rc2 == L7_FALSE)
      {
        memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
        
        sgAssertEventInfo.eventType =
        PIMSM_ASSERT_S_G_SM_EVENT_COULD_ASSERT_FALSE;
        sgAssertEventInfo.rtrIfNum = rtrIfNum;
        pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode, &sgAssertEventInfo);
      }
    }

    memset(&UpStrmSGEventInfo,0,sizeof(pimsmUpStrmSGEventInfo_t));
    joinDesired = pimsmSGJoinDesired(pimsmCb, pSGNode);
    if (joinDesired == L7_TRUE)
    {
      UpStrmSGEventInfo.eventType 
      = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_TRUE;
    }
    else
    {
      UpStrmSGEventInfo.eventType 
      = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_FALSE;
    }
    pimsmUpStrmSGExecute(pimsmCb, pSGNode, &UpStrmSGEventInfo);
    if(pimsmSGMFCUpdate(pimsmCb, pSGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "MFC Update failed");
    }
    pSGNodePrev = pSGNode;
        rc = pimsmSGNextGet(pimsmCb, pSGNodePrev, &pSGNode);
  }

  rc = pimsmStarGFirstGet( pimsmCb, &pStarGNode );
  while(rc == L7_SUCCESS)
  {
    pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
    if(pStarGIEntry != L7_NULLPTR)
    {
      #if PIMSM_TBD
      rc2 = pimsmStarGIAssertTrackingDesired(pimsmCb, pStarGNode, rtrIfNum);
      if(rc2 == L7_FALSE)
      {
        starGAssertEventInfo.eventType =
        PIMSM_ASSERT_STAR_G_SM_EVENT_ASSERT_TRACKING;

        starGAssertEventInfo.rtrIfNum = rtrIfNum;
        inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
        pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode, &starGAssertEventInfo);
      }
      #endif
      rc2 = pimsmStarGICouldAssert(pimsmCb,pStarGNode,rtrIfNum);
      if(rc2 == L7_FALSE)
      {
        starGAssertEventInfo.eventType =
        PIMSM_ASSERT_STAR_G_SM_EVENT_COULD_ASSERT_FALSE;

        starGAssertEventInfo.rtrIfNum = rtrIfNum;
        inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
        pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode, &starGAssertEventInfo);
        
      }
    }
    if (pIntfEntry->pimsmIamDR != L7_TRUE) 
    {
      if(pStarGNode->pimsmStarGIEntry[rtrIfNum] != L7_NULLPTR)
      {
        pStarGNode->pimsmStarGIEntry[rtrIfNum]->pimsmStarGILocalMembership = L7_FALSE;
      }
    }
    memset(&UpStrmStarGEventInfo,0,sizeof(pimsmUpStrmStarGEventInfo_t));
    joinDesired = pimsmStarGJoinDesired(pimsmCb, pStarGNode);
    if (joinDesired == L7_TRUE)
    {
      UpStrmStarGEventInfo.eventType 
      = PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_TRUE;
    }
    else
    {
      UpStrmStarGEventInfo.eventType 
      = PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_FALSE;
    }
    pimsmUpStrmStarGExecute(pimsmCb, pStarGNode, &UpStrmStarGEventInfo);
    
    if(pimsmStarGMFCUpdate(pimsmCb, pStarGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "MFC Update failed");
    }
    rc = pimsmStarGNextGet( pimsmCb, pStarGNode, &pStarGNode );
  }

  rc = pimsmStarStarRpFirstGet (pimsmCb, &pStarStarRpNode);
  while (rc == L7_SUCCESS)
  {
    if ((pStarStarRpNode->flags & PIMSM_NULL_OIF) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                   "deleting MFC entry" );
      if (pimsmStarStarRpMFCUpdate (pimsmCb, pStarStarRpNode, MFC_DELETE_ENTRY, L7_FALSE)
                                 != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO, "Failed to delete StarStarRP Node from MFC"); 
      }
    }
    /* Delete all NEGATIVE (*,G) and (S,G) entires  recursively too */
    pimsmStarGNegativeDelete(pimsmCb, 
      &pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPAddress);
#ifdef PIMSM_TBD
    if(pimsmStarStarRpMFCUpdate(pimsmCb, pStarStarRpNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "MFC Update failed");
    }
#endif
    rc = pimsmStarStarRpNextGet (pimsmCb, pStarStarRpNode, &pStarStarRpNode);
  }

  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Purge all the interface entries
*
* @param    pimsmCb     @b{(input)}  control block  
*
* @returns  L7_SUCCESS
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmIntfPurge(pimsmCB_t *pimsmCb)
{
  L7_uint32 rtrIfNum;
  L7_RC_t  rc;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;
 
  PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,"Enter ");

  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_ERROR,
                "CB is NULLPTR");
    return L7_FAILURE;
  }  
  
  for(rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; ++rtrIfNum)
  {

    rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_MAX,"IntfEntry is NULLPTR");  
      continue;
    }   
    pimsmIntfCleanup(pimsmCb, rtrIfNum);
  }
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  Get routine for join/prune-interval
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    rtrIfNum    @b{(input)}  router interface number
* @param    pInterval   @b{(output)} join prune Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmInterfaceJoinPruneIntervalGet(pimsmCB_t *pimsmCb,  
                                           L7_uint32 rtrIfNum,
                                           L7_uint32 *pInterval)
{
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;
  L7_RC_t rc;
  
  if (pimsmCb == L7_NULLPTR || pInterval == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  
      PIMSM_TRACE_ERROR,"pimsmCb or pInterval is NULL");
    return L7_FAILURE;
  }
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if (rc  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,
                "pIntfEntry is NULL for rtrIfNum = %d", rtrIfNum);
    
    /*return L7_FAILURE;*/
    /*upstream neighbor just now gone down:
      In this case the join will fail any way,
      but returning 'failure' will stop the timer, hence need to 
      return 'success' */
      *pInterval =  PIMSM_DEFAULT_JOIN_PRUNE_PERIOD;
      return L7_SUCCESS;  
  }
  *pInterval =  pIntfEntry->pimsmInterfaceJoinPruneInterval;
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Get routine for join/prune-holdtime interval
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    rtrIfNum    @b{(input)}  router interface number
* @param    pHoldtime   @b{(output)} join prune holdtime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmInterfaceJoinPruneHoldtimeGet(pimsmCB_t *pimsmCb,  
                                           L7_uint32 rtrIfNum,
                                           L7_ushort16 *pHoldtime)
{
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;
  L7_RC_t rc;
  
  if (pimsmCb == L7_NULLPTR || pHoldtime == L7_NULLPTR )
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR, 
      PIMSM_TRACE_ERROR,"pimsmCb or pHoldtime is NULL");
    return L7_FAILURE;
  }
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if (rc  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,
                "pIntfEntry is NULL for rtrIfNum = %d", rtrIfNum);
    /*return L7_FAILURE;*/
    /*upstream neighbor just now gone down:
      In this case the join will fail any way,
      but returning 'failure' will stop the timer, hence need to 
      return 'success' */
    /* set holdtime = 3.5 * jp-interval */
    *pHoldtime =  (35 * PIMSM_DEFAULT_JOIN_PRUNE_PERIOD)/10;    
    return L7_SUCCESS;  
  }
  *pHoldtime =  pIntfEntry->pimsmInterfaceJoinPruneHoldtime;
  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Get routine for hello interval
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    rtrIfNum    @b{(input)}  router interface number
* @param    pInterval   @b{(output)} hello Interval   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmInterfaceHelloIntervalGet( pimsmCB_t *pimsmCb,  
                                       L7_uint32 rtrIfNum,
                                       L7_uint32 *pInterval)
{

  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;
  L7_RC_t rc;

  if (pimsmCb == L7_NULLPTR || pInterval == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  
      PIMSM_TRACE_ERROR,"pimsmCb or pInterval is NULL");
    return L7_FAILURE;
  }
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if (rc  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,
                "pIntfEntry is NULL for rtrIfNum = %d", rtrIfNum);
    /*return L7_FAILURE;*/
    /*upstream neighbor just now gone down:
      In this case the join will fail any way,
      but returning 'failure' will stop the timer, hence need to 
      return 'success' */
      *pInterval =  PIMSM_DEFAULT_HELLO_PERIOD;
      return L7_SUCCESS; 
  }
 
  *pInterval = pIntfEntry->pimsmInterfaceHelloInterval;
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Get routine for hello hold time -interval
*
* @param    pimsmCb     @b{(input)}  control block  
* @param    rtrIfNum    @b{(input)}  router interface number
* @param    pHoldtime   @b{(output)} hello holdtime   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
L7_RC_t pimsmInterfaceHelloHoldtimeGet( pimsmCB_t *pimsmCb,  
                                       L7_uint32 rtrIfNum,
                                       L7_ushort16 *pHoldtime)
{
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;
  L7_RC_t rc;

  if (pimsmCb == L7_NULLPTR || pHoldtime == L7_NULLPTR )
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  
      PIMSM_TRACE_ERROR,"pimsmCb or pHoldtime is NULL");
    return L7_FAILURE;
  }
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if (rc  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_INTF_NEIGHBOR,  PIMSM_TRACE_INFO,
                "pIntfEntry is NULL for rtrIfNum = %d", rtrIfNum);
    /*return L7_FAILURE;*/
    /*upstream neighbor just now gone down:
      In this case the join will fail any way,
      but returning 'failure' will stop the timer, hence need to 
      return 'success' */
    /* set holdtime = 3.5 * hello-interval */
    *pHoldtime =  (35 * PIMSM_DEFAULT_HELLO_PERIOD)/10;    
    return L7_SUCCESS;      
  }
  *pHoldtime =  pIntfEntry->pimsmInterfaceHelloHoldtime;
  return L7_SUCCESS;
}

