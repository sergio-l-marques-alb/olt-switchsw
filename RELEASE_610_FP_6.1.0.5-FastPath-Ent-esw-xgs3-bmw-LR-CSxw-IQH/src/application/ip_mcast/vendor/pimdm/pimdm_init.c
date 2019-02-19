/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_init.c
*
* @purpose    
*
* @component  PIM-DM
*
* @comments   none
*
* @create     
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l7_socket.h"
#include "buff_api.h"
#include "l3_mcast_commdefs.h"
#include "l7_mcast_api.h"
#include "l7_pimdm_api.h"
#include "heap_api.h"
#include "mcast_map.h"
#include "pimdm_main.h"
#include "pimdm_debug.h"
#include "pimdm_mrt.h"
#include "pimdm_mgmd.h"
#include "pimdm_intf.h"
#include "pimdm_util.h"
#include "pimdm_defs.h"
#include "pimdm_init.h"
#include "pimdm_admin_scope.h"
#include "pimdm_timer.h"
#include "pimdm_rto.h"

/*******************************************************************************
**                        Function Declarations                               **
*******************************************************************************/

static L7_RC_t
pimdmCtrlBlockMembersDeInit (pimdmCB_t* pimdmCB);

static L7_RC_t
pimdmCtrlBlockMembersInit (pimdmCB_t* pimdmCB);

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/
/* None */

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  To De-initialize the members of the PIM-DM Control Block
*
* @param    pimdmCB  @b{ (input) } Pointer to the PIM-DM Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmCtrlBlockMembersDeInit (pimdmCB_t* pimdmCB)
{
  L7_RC_t retVal = L7_SUCCESS;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* De-Initialize the Socket */
  if (pimdmSocketDeInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Socket De-Init Failed");
    retVal = L7_FAILURE;
  }

  /* Destroy the Timers */
  if (pimdmTimersDeInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Timers Destroy Failed");
    retVal = L7_FAILURE;
  }

  /* Destroy the local membership database */
  if (pimdmMgmdDeInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Local Mem'ship database De-Init Failed");
    retVal = L7_FAILURE;
  }

  /* Destroy the PIM-DM Multicast Routing entry base */
  if (pimdmMrtSGTreeDeInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "MRT (S,G) Tree De-Init Failed");
    retVal = L7_FAILURE;
  }

  /* Destroy the Interface and Neighbor database */
  if (pimdmIntfDeInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Interface/Nbr database Destroy Failure");
    retVal = L7_FAILURE;
  }

  /* De-initialize the Admin Scope Boundary database */
  if (pimdmAdminScopeBoundaryDeInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Admin Scope database Destroy Failure");
    retVal = L7_FAILURE;
  }
  
  /* De-initialize the RTO Callback related resources */
  if (pimdmRTODeInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "RTO Callback Resources De-Init Failed");
    pimdmCtrlBlockMembersDeInit (pimdmCB);
    return L7_FAILURE;
  }

  /* De-Register with MFC */
  if (mfcCacheEventDeregister (L7_MRP_PIMDM) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "MFC De-Registration Failed");
    retVal = L7_FAILURE;
  }

  /* De-Register with MGMD */
  if (mgmdMapDeRegisterMembershipChangeEvent(pimdmCB->addrFamily,
                                     L7_MRP_PIMDM) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "MGMD De-Registration Failed");
    retVal = L7_FAILURE;
  }

  /* De-Register for Static MRoutes */
  if (mcastMapStaticMRouteEventDeRegister (pimdmCB->addrFamily, L7_MRP_PIMDM)
                                        != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Static MRoutes De-Registration Failed");
    retVal = L7_FAILURE;
  }

  pimdmUtilAppTimerCancel (pimdmCB, &(pimdmCB->pimdmJPBundleTimer));

  /* DeInitialize the All Routers Address */
  inetAddressZeroSet (pimdmCB->addrFamily, &(pimdmCB->allRoutersAddr));

  PIMDM_TRACE (PIMDM_DEBUG_INIT, "Ctrl Block Members De-Init Successful");

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}

/*********************************************************************
*
* @purpose  To Initialize the Memory for PIM-DM Protocol
*
* @param    addrFamily @b{ (input) } Address Family Identifier
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmMemoryInit (L7_uchar8 addrFamily)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (pimdmUtilMemoryInit (addrFamily) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_INIT, "PIM-DM Memory Initialization Failed for "
                 "addrFamily - %d", addrFamily);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_INIT, "PIM-DM Memory Initialization Successful for "
               "addrFamily - %d", addrFamily);

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To De-Initialize the Memory for PIM-DM Protocol
*
* @param    addrFamily @b{ (input) } Address Family Identifier
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmMemoryDeInit (L7_uchar8 addrFamily)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Bad Address Family");
    return L7_FAILURE;
  }

  /* De-Initialize the Heap memory */
  if (heapDeInit (mcastMapHeapIdGet (addrFamily), L7_FALSE) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Heap De-Init Failed for Family - %d",
                 addrFamily);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_INIT, "PIM-DM Memory De-Initialization Successful");

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To Initialize the members of the PIM-DM Control Block
*
* @param    pimdmCB  @b{ (input) } Pointer to the PIM-DM Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
pimdmCtrlBlockMembersInit (pimdmCB_t* pimdmCB)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Create and Initialize the Socket */
  if (pimdmSocketInit (pimdmCB) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING,L7_FLEX_PIMDM_MAP_COMPONENT_ID, "PIMDM Socket Initialization Failed for "
            "addrFamily - %d. PIMDM Socket Creation/options Set Failed. As a result of this, the PIM"
            " Control packets cannot be sent out on an interface.", pimdmCB->addrFamily);
    return L7_FAILURE;
  }

  /* Create and Initialize the Timers */
  if (pimdmTimersInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Timers Creation Failed");
    pimdmCtrlBlockMembersDeInit (pimdmCB);
    return L7_FAILURE;
  }

  /* Initialize the Multicast Routing entry base */
  if (pimdmMrtSGTreeInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "MRT (S,G) Tree Creation Failure");
    pimdmCtrlBlockMembersDeInit (pimdmCB);
    return L7_FAILURE;
  }

  /* Initialize the MGMD Local Membership database */
  if (pimdmMgmdInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Local Mem'Ship Tree Creation Failure");
    pimdmCtrlBlockMembersDeInit (pimdmCB);
    return L7_FAILURE;
  }

  /* Initialize the Interface and Neighbor database */
  if (pimdmIntfInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Interface/Nbr database Creation Failure");
    pimdmCtrlBlockMembersDeInit (pimdmCB);
    return L7_FAILURE;
  }

  /* Initialize the Admin Scope Boundary database */
  if (pimdmAdminScopeBoundaryInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Admin Scope database Creation Failure");
    pimdmCtrlBlockMembersDeInit (pimdmCB);
    return L7_FAILURE;
  }

  /* Initialize the RTO Callback related resources */
  if (pimdmRTOInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "RTO Callback Resources Init Failed");
    pimdmCtrlBlockMembersDeInit (pimdmCB);
    return L7_FAILURE;
  }

  /* Register with MFC */
  if (mfcCacheEventRegister (L7_MRP_PIMDM, pimdmMapEventChangeCallback, L7_TRUE,
                             PIMDM_MFC_WRONG_IF_RATE_LIMIT_THRESHOLD)
                          != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "MFC Registration Failed");
    pimdmCtrlBlockMembersDeInit (pimdmCB);
    return L7_FAILURE;
  }

  /* Register with MGMD */
  if (mgmdMapRegisterMembershipChangeEvent (pimdmCB->addrFamily, 
                                            L7_MRP_PIMDM,
                                            pimdmMapEventChangeCallback)
                                         != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "MGMD Registration Failed");
    pimdmCtrlBlockMembersDeInit (pimdmCB);
    return L7_FAILURE;
  }

  /* Register for Static MRoutes */
  if (mcastMapStaticMRouteEventRegister (pimdmCB->addrFamily, 
                                         L7_MRP_PIMDM,
                                         pimdmMapEventChangeCallback)
                                      != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Static MRoutes Registration Failed");
    pimdmCtrlBlockMembersDeInit (pimdmCB);
    return L7_FAILURE;
  }

  /* Initialize the All Routers Address */
  inetAllPimRouterAddressInit (pimdmCB->addrFamily, &(pimdmCB->allRoutersAddr));

  pimdmCB->pimdmJPBundleTimer = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_INIT, "Ctrl Block Members Init Successful");

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To De-initialize the PIM-DM Control Block
*
* @param    cbHandle @b{ (input) } Handle to the PIM-DM Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmCtrlBlockDeInit (MCAST_CB_HNDL_t cbHandle)
{
  pimdmCB_t* pimdmCB = (pimdmCB_t*) cbHandle;
  L7_RC_t retVal = L7_SUCCESS;
  L7_uint32 addrFamily;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if (pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_INIT, "Invalid PIM-DM Control Block");
    return L7_FAILURE;
  }

  /* De-Initialize the members of the Control Block */
  if (pimdmCtrlBlockMembersDeInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_INIT, "PIM-DM Ctrl Block Members De-Init Failed");
    retVal = L7_FAILURE;
  }

  /* Free the Control Block Memory */
  addrFamily = pimdmCB->addrFamily;
  memset(pimdmCB, 0, sizeof(pimdmCB_t));
  PIMDM_FREE (addrFamily, (void*) pimdmCB);
  pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}


/*********************************************************************
*
* @purpose  To Initialize the PIM-DM Control Block
*
* @param    addrFamily @b{ (input) } Address Family Identifier
*
* @returns  Handle to the Control Block, if success
* @returns  L7_NULLPTR, if failure
*
* @comments
*
* @end
*********************************************************************/
MCAST_CB_HNDL_t
pimdmCtrlBlockInit (L7_uchar8 addrFamily)
{
  pimdmCB_t* pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Bad Address Family");
    return L7_NULLPTR;
  }

  /* Allocate memory for the PIM-DM Control Block instance */
  if ((pimdmCB = PIMDM_ALLOC (addrFamily, sizeof (pimdmCB_t))) == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Allocation of Ctrl Block Failed");
    return L7_NULLPTR;
  }

  /* Initialize the Control Block Members */
  memset (pimdmCB, 0, sizeof (pimdmCB_t));
  pimdmCB->addrFamily = addrFamily;

  if (pimdmCtrlBlockMembersInit (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Ctrl Block Members Init Failed");
    pimdmCtrlBlockDeInit ((MCAST_CB_HNDL_t*) pimdmCB);
    return L7_NULLPTR;
  }

  PIMDM_TRACE (PIMDM_DEBUG_INIT, "Ctrl Block Init Successful");

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return ((MCAST_CB_HNDL_t) pimdmCB);
}

/*********************************************************************
*
* @purpose  To display the members of the PIM-DM Control Block
*
* @param    pimdmCB  @b{ (input) } Pointer to the PIM-DM Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
void
pimdmCtrlBlockMembersShow (pimdmCB_t* pimdmCB)
{
  L7_uchar8 addr[PIMDM_MAX_DBG_ADDR_SIZE];
  L7_uint32 index = 0;

  if (pimdmCB == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }

  PIMDM_DEBUG_PRINTF ("Address of PIM-DM Control Block : 0x%x.\n", pimdmCB);
  PIMDM_DEBUG_PRINTF ("General Information ....\n");
  PIMDM_DEBUG_PRINTF ("    addrFamily         - %s.\n", ((pimdmCB->addrFamily == L7_AF_INET) ? ("IPv4") : ("IPv6")));
  PIMDM_DEBUG_PRINTF ("    operMode           - %s.\n", ((pimdmCB->operMode == L7_TRUE) ? ("Operational") : ("Not Operational")));
  PIMDM_DEBUG_PRINTF ("    sockFD             - %d.\n", pimdmCB->sockFD);
  PIMDM_DEBUG_PRINTF ("    allRoutersAddr     - %s.\n", inetAddrPrint (&(pimdmCB->allRoutersAddr), addr));
  PIMDM_DEBUG_PRINTF ("    heapID             - 0x%x.\n", mcastMapHeapIdGet (pimdmCB->addrFamily));
  PIMDM_DEBUG_PRINTF ("Interface/Neighbor related Information ....\n");
  PIMDM_DEBUG_PRINTF ("    intfInfo           - 0x%x.\n", pimdmCB->intfInfo);
  PIMDM_DEBUG_PRINTF ("    nbrBitmap          - ");
  for (index = 1; index <= PIMDM_INTF_BIT_SIZE; index++)
    if (BITX_TEST (&pimdmCB->nbrBitmap, index) != 0)
      PIMDM_DEBUG_PRINTF (" %02d ", index);
  PIMDM_DEBUG_PRINTF ("\n");
  PIMDM_DEBUG_PRINTF ("MRT (S,G) Tree related Information ....\n");
  PIMDM_DEBUG_PRINTF ("    mrtTreeHeap        - 0x%x.\n", pimdmCB->mrtTreeHeap);
  PIMDM_DEBUG_PRINTF ("    mrtDataHeap        - 0x%x.\n", pimdmCB->mrtDataHeap);
  PIMDM_DEBUG_PRINTF ("    No of Entries      - %d.\n", avlTreeCount (&pimdmCB->mrtSGTree));
  PIMDM_DEBUG_PRINTF ("MGMD Local Membership related Information ....\n");
  PIMDM_DEBUG_PRINTF ("    pLocalMembTreeHeap - 0x%x.\n", pimdmCB->pLocalMembTreeHeap);
  PIMDM_DEBUG_PRINTF ("    pLocalMembDataHeap - 0x%x.\n", pimdmCB->pLocalMembDataHeap);
  PIMDM_DEBUG_PRINTF ("    No of Entries      - %d.\n", avlTreeCount (&pimdmCB->localMembTree));
  PIMDM_DEBUG_PRINTF ("RTO related Information ....\n");
  PIMDM_DEBUG_PRINTF ("    rtoCallback        - 0x%x.\n", pimdmCB->rtoCallback);
  PIMDM_DEBUG_PRINTF ("    rtoTimer           - 0x%x.\n", pimdmCB->rtoTimer);
  PIMDM_DEBUG_PRINTF ("    routeChangeBuffer  - 0x%x.\n", pimdmCB->routeChangeBuffer);
  PIMDM_DEBUG_PRINTF ("Admin Scope Boundary related Information ....\n");
  PIMDM_DEBUG_PRINTF ("    asbList            - 0x%x.\n", pimdmCB->asbList);
  PIMDM_DEBUG_PRINTF ("    No of Entries      - %d.\n", SLLNumMembersGet (&pimdmCB->asbList));
  PIMDM_DEBUG_PRINTF ("Timers related Information ....\n");
  PIMDM_DEBUG_PRINTF ("    appTimer           - 0x%x.\n", pimdmCB->appTimer);
  PIMDM_DEBUG_PRINTF ("    handleList         - 0x%x.\n", pimdmCB->handleList);
  PIMDM_DEBUG_PRINTF ("    maxPimdmTimers     - %d.\n", pimdmCB->maxPimdmTimers);
}

