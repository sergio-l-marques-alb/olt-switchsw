/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmDebugUT.c
*
* @purpose  debug and statistics related structs and APIs.
*
* @component pimsm
*
* @comments
*
* @create 06/05/2006
*
* @author dsatyanarayana
* @end
*
**********************************************************************/
#include "l7_ip_api.h"
#include "pimsmdefs.h"
#include "osapi_support.h"
#include "rto_api.h"
#include "rtmbuf.h"
#include "rtiprecv.h"
#include "l3_mcast_commdefs.h"
#include "pimsmsgrpttree.h"
#include "pimsmsgtree.h"
#include "pimsmstargtree.h"
#include "pimsmstarstarrptree.h"
#include "pimsmtimer.h"
#include "pimsmbsr.h"
#include "pimsmcontrol.h"
#include "pimsmintf.h"
#include "l7apptimer_api.h"
#include "pimsmv6util.h"
#include "l3_mcast_commdefs.h"
#include "pimsmmacros.h"
#include "pimsmwrap.h"
#include "mcast_wrap.h"
#include "pimsmcandbsrfsm.h"
#include "pimsmnoncandbsrfsm.h"
#include "pimsm_vend_ctrl.h"

#define PIMSM_PRINT_ADDR(xaddr)           \
   {                                                     \
      L7_char8  __buf1__[PIMSM_MAX_MSG_SIZE];      \
           (void)inetAddrHtop((xaddr),__buf1__);              \
        PIMSM_DEBUG_PRINTF("%s \n",__buf1__);  \
   }  


#define PIMSM_IPV6_INADDR_ALL_PIM_ROUTERS  "fffe::05"      
#define PIMSM_IPV6_INADDR_PIM_ROUTER1  "fffe::01"      
#define PIMSM_IPV6_INADDR_PIM_ROUTER2  "fffe::02"      
#define PIMSM_IPV6_INADDR_PIM_ROUTER3  "fffe::03"     
#define PIMSM_IPV6_INADDR_PIM_ROUTER4  "fffe::04"     

#define PIMSM_IPV6_SRC_ADDR "2001::7"      
#define PIMSM_IPV6_SRC_ADDR1  "2001::8"      
#define PIMSM_IPV6_SRC_ADDR2  "2002::7"      
#define PIMSM_IPV6_SRC_ADDR3  "2005::7"     
#define PIMSM_IPV6_SRC_ADDR4  "2005::8"   

#define PIMSM_IPV6_RP_ADDR  "FE80::210:18FF:FE82:86C" 
#define PIMSM_IPV6_RP_ADDR1 "FE80::210:18FF:FE82:4FF"      
#define PIMSM_IPV6_RP_ADDR2  "2002::7"     
#define PIMSM_IPV6_RP_ADDR3  "2005::7"      
#define PIMSM_IPV6_RP_ADDR4  " 2001::8"      

/* some debug code to test Trees */
L7_inet_addr_t      pimsmDebugSrcAddr[5] =
{
  {L7_AF_INET,
    {{0x14010101}}},
  {L7_AF_INET,
    {{0x0A010101}}},
  {L7_AF_INET,
    {{0x05010101}}},
  {L7_AF_INET,
    {{0x05010101}}},
  {L7_AF_INET,
    {{0x04020305}}},
};

L7_inet_addr_t      pimsmDebugGrpAddr[5] =
{
  {L7_AF_INET,
    {{0xE1020301}}},
  {L7_AF_INET,
    {{0xE1010101}}},
  {L7_AF_INET,
    {{0xE1000001}}},
  {L7_AF_INET,
    {{0xE1020304}}},
  {L7_AF_INET,
    {{0xE1020305}}},
};

L7_inet_addr_t      pimsmDebugRpAddr[5] =
{
  {L7_AF_INET,
    {{0x01010101}}},
  {L7_AF_INET,
    {{0x0A010101}}},
  {L7_AF_INET,
    {{0x04020303}}},
  {L7_AF_INET,
    {{0x04020304}}},
  {L7_AF_INET,
    {{0x04020305}}},
};                              
/*
Steps for testing SG Tree
  1. Ensure that application has initialize SG Tree by
        calling pimsmSGInit().
  2. Do "devshell pimsmDebugSGEntryAdd(index)" with index = 0..4
  3. Do "devshell pimsmDebugSGTableShow()" to see that the entry is added.
  4. Do "devshell pimsmDebugSGEntryDelete(index)" with index = 0..4
  5. Do "devshell pimsmDebugSGTableShow()" to see that the entry is delete.  
  6. Repeat above steps in various combinations.
  */



void pimsmDebugCtrlBlockShow ( pimsmCB_t * pimsmCb );

static void pimsmDebugV6StringToAddr ( L7_uint32 index,
                                       L7_in6_addr_t * Ipv6Addr );
static void pimsmDebugV6SrcStringToAddr ( L7_uint32 index,
                                          L7_in6_addr_t * Ipv6Addr );
void pimsmDebugStarGEntryAdd ( L7_uchar8 family, L7_uchar8 index );

/******************************************************************************
* @purpose  Dummy function, used to link this file in the image.
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugUTDummy ()
{
}
/******************************************************************************
* @purpose  Dummy function, used to link this file in the image.
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugCbDeInit ( L7_uchar8 family )
{
  pimsmCB_t *           pimsmCb;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = (pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }

  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );

  pimsmCtrlBlockDeInit( ( void * )pimsmCb );
}

/******************************************************************************
* @purpose  Dummy function, used to link this file in the image.
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugUpdateDr ( L7_uchar8 family, L7_uint32 rtrIfNum )
{
  pimsmCB_t *   pimsmCb;
  pimsmInterfaceEntry_t * intfEntry;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }

  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  pimsmIntfEntryGet(pimsmCb, rtrIfNum, &intfEntry);
  pimsmIntfDRUpdate( pimsmCb, intfEntry);
}

/******************************************************************************
* @purpose  stub routine to add a (*,*,RP) entry (Testing only)
*
* @param        family          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugStarStarRpEntryAdd ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *               pimsmCb;
  pimsmStarStarRpNode_t     starStarRpNode, * rpNodePtr = L7_NULLPTR;
  pimsmStarStarRpIEntry_t *starStarRpIEntry = L7_NULLPTR;
  L7_uint32 ii =0;

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB Handle is %p\n", pimsmCb );
  pimsmDebugCtrlBlockShow( pimsmCb );
  memset( &starStarRpNode, 0, sizeof( pimsmStarStarRpNode_t ) );

  inetCopy( &( starStarRpNode.pimsmStarStarRpEntry.pimsmStarStarRpRPAddress ),
            &pimsmDebugSrcAddr[index] );
  /*pimsmStarStarRpAdd(pimsmCb, &starStarRpNode);*/
  pimsmStarStarRpNodeCreate( pimsmCb, &pimsmDebugSrcAddr[index], &rpNodePtr );
  if(rpNodePtr != ( pimsmStarStarRpNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("B4 Calling pimsmStarStarRpIEntryCreate\r\n");
    for(ii =0;ii<3;ii++)
    {
      if(pimsmStarStarRpIEntryCreate(pimsmCb,rpNodePtr,index +ii) == 
         L7_SUCCESS)
      {
        PIMSM_DEBUG_PRINTF("pimsmStarStarRpIEntryCreate successful\r\n");
        starStarRpIEntry = rpNodePtr->pimsmStarStarRpIEntry[index +ii];
        starStarRpIEntry->pimsmStarStarRpIJoinPruneState =
        PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_JOIN; 
      }
    }

    PIMSM_DEBUG_PRINTF( "New Node is %p\n", rpNodePtr );
  }
}
/******************************************************************************
* @purpose  stub routine to delete a (*,*,RP) entry (Testing only)
*
* @param        family          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugStarStarRpEntryDelete ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *               pimsmCb;
  pimsmStarStarRpNode_t     starStarRpNode;

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB Handle is %p\n", pimsmCb );
  pimsmDebugCtrlBlockShow( pimsmCb );
  memset( &starStarRpNode, 0, sizeof( pimsmStarStarRpNode_t ) );

  inetCopy( &( starStarRpNode.pimsmStarStarRpEntry.pimsmStarStarRpRPAddress ),
            &pimsmDebugSrcAddr[index] );
  if(pimsmStarStarRpDelete( pimsmCb, &starStarRpNode ) == L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "Deleted SUCCESSfully \n" );
  }
}


/******************************************************************************
* @purpose  stub routine to add a (*,*,RP) entry (Testing only)
*
* @param        family          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugKernelCacheEntryAdd ( L7_uchar8 family,
                                     L7_uint32 grpIndex,
                                     L7_uint32 srcIndex )
{
  pimsmCB_t *               pimsmCb;
  pimsmStarGNode_t *        starGNodePtr = L7_NULLPTR;

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB Handle is %p\n", pimsmCb );

  if(pimsmStarGFind( pimsmCb,
                     &pimsmDebugGrpAddr[grpIndex],
                     &starGNodePtr ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "(*,G) Node not found \n" );
  }

  if(starGNodePtr != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    if(pimsmStarGCacheAdd( pimsmCb,
                            starGNodePtr,
                            &pimsmDebugSrcAddr[srcIndex] ) != L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF( "Kernel Cache ADd failed \n" );
      return;
    }
    else
    {
      PIMSM_DEBUG_PRINTF( "Kern cache entry added \n" );
    }
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "(*,G) Node not found \n" );
  }
  PIMSM_DEBUG_PRINTF( "Added kernel cache successfully \n" );
}

/******************************************************************************
* @purpose  stub routine to delete a (*,*,RP) entry (Testing only)
*
* @param        family          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugKernelCacheEntryDelete ( L7_uchar8 family,
                                        L7_uint32 grpIndex,
                                        L7_uint32 srcIndex )
{
  pimsmCB_t *               pimsmCb;
  pimsmStarGNode_t *        starGNodePtr;
  pimsmCache_t *      kernelCachPtr;

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB Handle is %p\n", pimsmCb );

  pimsmStarGFind( pimsmCb, &pimsmDebugGrpAddr[grpIndex], &starGNodePtr ); 
  pimsmStarGCacheFind( pimsmCb,
                        starGNodePtr,
                        &pimsmDebugSrcAddr[srcIndex],
                        &pimsmDebugGrpAddr[grpIndex],
                        &kernelCachPtr );
  if(kernelCachPtr != ( pimsmCache_t * )L7_NULLPTR)
  {
    if(pimsmStarGCacheDelete( pimsmCb,
                               starGNodePtr,
                               kernelCachPtr ) == L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF( "KernCahe Deleted SUCCESSfully \n" );
    }
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "Delete - KernCahe Not Found \n" );
  }
}


/******************************************************************************
* @purpose  Find and return the kernel cache entry from the Kernel Cache List
*
* @param    kernCacheList @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugKernelCacheShow ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *               pimsmCb = L7_NULLPTR;
  pimsmStarGNode_t *        starGNodePtr;
  L7_RC_t                   rc;
  pimsmCache_t *      kernelCacheEntry;
  L7_uint32                 kerCachCount = 0;

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_DEBUG_PRINTF( "Invalid input parameter\r\n" );
    return;
  }

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );   
  pimsmStarGFind( pimsmCb, &pimsmDebugGrpAddr[index], &starGNodePtr );
  if(starGNodePtr != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    rc = pimsmStarGCacheGetFirst( pimsmCb, starGNodePtr, &kernelCacheEntry );
    PIMSM_DEBUG_PRINTF( "First Entry is %p \n", kernelCacheEntry );                             
    while(rc == L7_SUCCESS)
    {
      /* print info */  
      PIMSM_DEBUG_PRINTF(
                        "-----------------(Kernel Cache Entries)-------------------\n" );  
      ++kerCachCount;           
      PIMSM_DEBUG_PRINTF( "Source Addr is" ); 
      PIMSM_PRINT_ADDR( &kernelCacheEntry->pimsmSrcAddr );
      PIMSM_DEBUG_PRINTF( "Group Addr is" );
      PIMSM_PRINT_ADDR( &kernelCacheEntry->pimsmGrpAddr );
      PIMSM_DEBUG_PRINTF( "ByteCount is %d \n",
                          kernelCacheEntry->pimsmSGRealtimeByteCount );
      PIMSM_DEBUG_PRINTF( "Packet Count is %d \n",
                          kernelCacheEntry->pimsmSGRealtimePktcnt );
      PIMSM_DEBUG_PRINTF( "(S,G) Count is %d \n",
                          kernelCacheEntry->pimsmSGByteCount );
      PIMSM_DEBUG_PRINTF( "Curr Entry is %p \n", kernelCacheEntry );                              
          rc = pimsmStarGCacheNextGet( pimsmCb,
                                    starGNodePtr,
                                    kernelCacheEntry,
                                    &kernelCacheEntry );
      PIMSM_DEBUG_PRINTF( "Next Entry is %p \n", kernelCacheEntry );
    }
    PIMSM_DEBUG_PRINTF( " Number of Entries KerCache Entries are  = %d\n",
                        kerCachCount );
  }
}

/******************************************************************************
* @purpose  stub routine to add a (*,*,RP) entry (Testing only)
*
* @param        family          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugNeighborEntryUpdate ( L7_uchar8 family,
                                     L7_uint32 rtrIfNum,
                                     L7_uint32 index,
                                     L7_ushort16 holdtime,
                                     L7_uint32 genId,
                                     L7_uint32 drPriority)
{
  pimsmCB_t *       pimsmCb;
  pimsmInterfaceEntry_t *       intfEntry = L7_NULLPTR;
  pimsmHelloParams_t helloParams;
  L7_RC_t rc;
  pimsmNeighborEntry_t  *nbr;

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB Handle is %p\n", pimsmCb );

  if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &intfEntry)!=L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "IntfEntry is NULLPTR\n" );
    return;
  }
  helloParams.drPriority = drPriority;
  helloParams.genId = genId;
  helloParams.holdtime = holdtime;

  rc = pimsmNeighborFind(pimsmCb, intfEntry, 
                         &pimsmDebugSrcAddr[index], &nbr);
  if(rc != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "neighbor does not exists\n" );
    return;
  }
  if(pimsmNeighborUpdate( pimsmCb,
                          intfEntry,
                          nbr,
                          &helloParams ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "NBR node create failed \n" );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "Family: %d RtrIfNum: %d SrcIndex: %d\n",
                        family,
                        rtrIfNum,
                        index );
    PIMSM_DEBUG_PRINTF( "NBR added successfully\n" );
  }

}

/******************************************************************************
* @purpose  stub routine to add a (*,*,RP) entry (Testing only)
*
* @param        family          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugNeighborEntryAdd ( L7_uchar8 family,
                                  L7_uint32 rtrIfNum,
                                  L7_uint32 index,
                                  L7_ushort16 holdtime,
                                  L7_uint32 genId,
                                  L7_uint32 drPriority)
{
  pimsmCB_t *       pimsmCb;
  pimsmInterfaceEntry_t *       intfEntry = L7_NULLPTR;
  pimsmHelloParams_t helloParams;

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB Handle is %p\n", pimsmCb );


  if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &intfEntry )!=L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "IntfEntry is NULLPTR\n" );
    return;
  }

  memset (&helloParams, 0, sizeof(pimsmHelloParams_t));

  helloParams.drPriority = drPriority;
  helloParams.genId = genId;
  helloParams.holdtime = holdtime;

  if(pimsmNeighborCreate( pimsmCb,
                          &pimsmDebugSrcAddr[index],
                          intfEntry,
                          &helloParams ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "NBR node create failed \n" );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "Family: %d RtrIfNum: %d SrcIndex: %d\n",
                        family,
                        rtrIfNum,
                        index );
    PIMSM_DEBUG_PRINTF( "NBR added successfully\n" );
  }
}
/******************************************************************************
* @purpose  stub routine to delete a (*,*,RP) entry (Testing only)
*
* @param        family          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugNeighborEntryDelete ( L7_uchar8 family,
                                     L7_uint32 rtrIfNum,
                                     L7_uint32 index )
{
  pimsmCB_t *                   pimsmCb;
  pimsmInterfaceEntry_t *       intfEntry = L7_NULLPTR;
  pimsmNeighborEntry_t          nbrEntry;

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB Handle is %p\n", pimsmCb );

  if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &intfEntry )!=L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "IntfEntry is NULLPTR\n" );
    return;
  }
  inetCopy( &nbrEntry.pimsmNeighborAddrList.pimsmPrimaryAddress, &pimsmDebugSrcAddr[index] );
  if(pimsmNeighborDelete( pimsmCb, intfEntry, &nbrEntry ) == L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "NBR Entry Deleted SUCCESSfully \n" );
  }
}
#ifdef PIMSM_NOTNEEDED
/******************************************************************************
* @purpose  stub routine to add an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugSGEntryAdd ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmSGNode_t     routeEntry;
  pimsmCB_t *       pimsmCb;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n", pimsmCb->pimsmSGTreeHeap );

  memset( &routeEntry, 0, sizeof( pimsmSGNode_t ) );
  inetCopy( &( routeEntry.pimsmSGEntry.pimsmSGSrcAddress ),
            &pimsmDebugSrcAddr[index] );
  inetCopy( &( routeEntry.pimsmSGEntry.pimsmSGGrpAddress ),
            &pimsmDebugGrpAddr[index] );
  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGAdd\r\n" );
  pimsmSGAdd( pimsmCb, &routeEntry );
}                    

/******************************************************************************
* @purpose  stub routine to add an (S,G,I) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugSGIEntryAdd ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmSGNode_t     routeEntry, * pRouteEntry;
  pimsmCB_t *       pimsmCb;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n", pimsmCb->pimsmSGTreeHeap );

  memset( &routeEntry, 0, sizeof( pimsmSGNode_t ) );
  inetCopy( &( routeEntry.pimsmSGEntry.pimsmSGSrcAddress ),
            &pimsmDebugSrcAddr[index] );
  inetCopy( &( routeEntry.pimsmSGEntry.pimsmSGGrpAddress ),
            &pimsmDebugGrpAddr[index] );
  inetCopy( &( routeEntry.pimsmSGEntry.pimsmSGUpstreamNeighbor ),
            &pimsmDebugSrcAddr[index] );
  routeEntry.pimsmSGEntry.pimsmSGRPFIfIndex = index;
  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGAdd\r\n" );
  pimsmSGAdd( pimsmCb, &routeEntry );
  pimsmSGFirstGet( pimsmCb, &pRouteEntry );
  pimsmSGIEntryCreate( pimsmCb, pRouteEntry, index );
}                    
#endif

/******************************************************************************
* @purpose  stub routine to find a (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugSGEntryFind ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmSGNode_t *       routeEntry = L7_NULLPTR;
  pimsmCB_t *           pimsmCb;
  pimsmSGEntry_t *      pimsmSGEntry = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n", pimsmCb->pimsmSGTreeHeap );

  if(pimsmSGFind( pimsmCb,
                  &pimsmDebugGrpAddr[index],
                  &pimsmDebugSrcAddr[index],
                  &routeEntry ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmSGFind() failed\r\n" );
    return;
  }

  PIMSM_DEBUG_PRINTF( "Address of Node is %p", routeEntry );
  pimsmSGEntry = &routeEntry->pimsmSGEntry; 
  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmSGEntry->pimsmSGGrpAddress );
  PIMSM_DEBUG_PRINTF( "Source Address : " );
  PIMSM_PRINT_ADDR( &pimsmSGEntry->pimsmSGSrcAddress );
}                    
/******************************************************************************
* @purpose  stub routine to test pimsmSGTreeIntfDownUpdate()function (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGTreeIntfDownUpdate ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *   pimsmCb;
  PIMSM_DEBUG_PRINTF( "Entry" );

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  pimsmSGTreeIntfDownUpdate( pimsmCb, index );
}
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGEntryDelete ( L7_uchar8 family, L7_uchar8 index )
{
  L7_inet_addr_t *      grpAddress;
  L7_inet_addr_t *      srcAddress;
  pimsmCB_t *           pimsmCb;
  PIMSM_DEBUG_PRINTF( "Entry" );

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );

  grpAddress = &pimsmDebugGrpAddr[index];
  srcAddress = &pimsmDebugSrcAddr[index];
  PIMSM_DEBUG_PRINTF( "B4 calling pimsmSGDelete\r\n" );
  pimsmSGDelete( pimsmCb, grpAddress, srcAddress );
}

#ifdef PIMSM_NOTNEEDED
/******************************************************************************
* @purpose  stub routine to add an (S,G) Rpt entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugSGRptEntryAdd ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmSGRptNode_t      routeEntry;
  pimsmCB_t *           pimsmCb;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Rpt Tree address is %p\r\n",
                      pimsmCb->pimsmSGRptTreeHeap );

  memset( &routeEntry, 0, sizeof( pimsmSGRptNode_t ) );
  inetCopy( &( routeEntry.pimsmSGRptEntry.pimsmSGRptSrcAddress ),
            &pimsmDebugSrcAddr[index] );
  inetCopy( &( routeEntry.pimsmSGRptEntry.pimsmSGRptGrpAddress ),
            &pimsmDebugGrpAddr[index] );
  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGAdd\r\n" );
  pimsmSGRptAdd( pimsmCb, &routeEntry );
}                    

/******************************************************************************
* @purpose  stub routine to add an (S,G,I) Rpt entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugSGRptIEntryAdd ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmSGRptNode_t      routeEntry, * pRouteEntry;
  pimsmCB_t *           pimsmCb;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Rpt Tree address is %p\r\n",
                      pimsmCb->pimsmSGRptTreeHeap );

  memset( &routeEntry, 0, sizeof( pimsmSGRptNode_t ) );
  inetCopy( &( routeEntry.pimsmSGRptEntry.pimsmSGRptSrcAddress ),
            &pimsmDebugSrcAddr[index] );
  inetCopy( &( routeEntry.pimsmSGRptEntry.pimsmSGRptGrpAddress ),
            &pimsmDebugGrpAddr[index] );
  /* inetCopy(&(routeEntry.pimsmSGRptEntry.pimsmSGUpstreamNeighbor), 
            &pimsmDebugSrcAddr[index]);*/
  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGAdd\r\n" );
  pimsmSGRptAdd( pimsmCb, &routeEntry );
  pimsmSGRptFirstGet( pimsmCb, &pRouteEntry );
  pimsmSGRptIEntryCreate( pimsmCb, pRouteEntry, index );
}                    
#endif
/******************************************************************************
* @purpose  stub routine to find a (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugSGRptEntryFind ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmSGRptNode_t *    routeEntry = L7_NULLPTR;
  pimsmCB_t *           pimsmCb;
  pimsmSGRptEntry_t *   pimsmSGRptEntry = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n",
                      pimsmCb->pimsmSGRptTreeHeap );

  if(pimsmSGRptFind( pimsmCb,
                     &pimsmDebugGrpAddr[index],
                     &pimsmDebugSrcAddr[index],
                     &routeEntry ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmSGRpt Find() failed\r\n" );
    return;
  }
  pimsmSGRptEntry = &routeEntry->pimsmSGRptEntry; 
  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmSGRptEntry->pimsmSGRptGrpAddress );
  PIMSM_DEBUG_PRINTF( "Source Address : " );
  PIMSM_PRINT_ADDR( &pimsmSGRptEntry->pimsmSGRptSrcAddress );
}                    
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGRptEntryDelete ( L7_uchar8 family, L7_uchar8 index )
{
  L7_inet_addr_t *      grpAddress;
  L7_inet_addr_t *      srcAddress;
  pimsmCB_t *           pimsmCb;
  PIMSM_DEBUG_PRINTF( "Entry" );

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );

  grpAddress = &pimsmDebugGrpAddr[index];
  srcAddress = &pimsmDebugSrcAddr[index];
  PIMSM_DEBUG_PRINTF( "B4 calling pimsmSGDelete\r\n" );
  pimsmSGRptDelete( pimsmCb, grpAddress, srcAddress );
}
/******************************************************************************
* @purpose  stub routine to test pimsmSGRptTreeIntfDownUpdate()function (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGRptTreeIntfDownUpdate ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *   pimsmCb;
  PIMSM_DEBUG_PRINTF( "Entry" );

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  pimsmSGRptTreeIntfDownUpdate( pimsmCb, index );
}

#ifdef PIMSM_NOTNEEDED
/******************************************************************************
* @purpose  stub routine to add an (*,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugStarGEntryAdd ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmStarGNode_t      routeEntry;
  pimsmCB_t *           pimsmCb;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n",
                      pimsmCb->pimsmStarGTreeHeap );

  memset( &routeEntry, 0, sizeof( pimsmStarGNode_t ) );
  inetCopy( &( routeEntry.pimsmStarGEntry.pimsmStarGGrpAddress ),
            &pimsmDebugGrpAddr[index] );
  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGAdd\r\n" );
  pimsmStarGAdd( pimsmCb, &routeEntry, &pimsmDebugSrcAddr[index] );
}                    

/******************************************************************************
* @purpose  stub routine to add an (*,G,I) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugStarGIEntryAdd ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmStarGNode_t      routeEntry, * pRouteEntry = L7_NULLPTR;
  pimsmCB_t *           pimsmCb;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n",
                      pimsmCb->pimsmStarGTreeHeap );

  memset( &routeEntry, 0, sizeof( pimsmStarGNode_t ) );
  inetCopy( &( routeEntry.pimsmStarGEntry.pimsmStarGGrpAddress ),
            &pimsmDebugGrpAddr[index] );
  inetCopy( &( routeEntry.pimsmStarGEntry.pimsmStarGRPAddress ),
            &pimsmDebugSrcAddr[index] );
  inetCopy( &( routeEntry.pimsmStarGEntry.pimsmStarGUpstreamNeighbor ),
            &pimsmDebugSrcAddr[index] );
  routeEntry.pimsmStarGEntry.pimsmStarGRPFIfIndex = index;
  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGAdd\r\n" );
  pimsmStarGAdd( pimsmCb, &routeEntry, L7_NULLPTR );
  pimsmStarGFirstGet( pimsmCb, &pRouteEntry );
  pimsmStarGIEntryCreate( pimsmCb, pRouteEntry, index );
}                    
#endif
/******************************************************************************
* @purpose  stub routine to find a (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugStarGEntryFind ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmStarGNode_t *    routeEntry = L7_NULLPTR;
  pimsmCB_t *           pimsmCb;
  pimsmStarGEntry_t *   pimsmStarGEntry = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "StarG Tree address is %p\r\n",
                      pimsmCb->pimsmStarGTreeHeap );

  if(pimsmStarGFind( pimsmCb,
                     &pimsmDebugGrpAddr[index],
                     &routeEntry ) != L7_SUCCESS)
  {
      PIMSM_DEBUG_PRINTF( "pimsmStarGFind() failed\r\n" );
    return;
  }
  pimsmStarGEntry = &routeEntry->pimsmStarGEntry; 
  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmStarGEntry->pimsmStarGGrpAddress );
  PIMSM_DEBUG_PRINTF( "RP Address : " );
  PIMSM_PRINT_ADDR( &pimsmStarGEntry->pimsmStarGRPAddress );
}                    
/******************************************************************************
* @purpose  stub routine to test pimsmSGTreeIntfDownUpdate()function (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugStarGTreeIntfDownUpdate ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *   pimsmCb;
  PIMSM_DEBUG_PRINTF( "Entry" );

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  pimsmStarGTreeIntfDownUpdate( pimsmCb, index );
}
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugStarGEntryDelete ( L7_uchar8 family, L7_uchar8 index )
{
  L7_inet_addr_t *      grpAddress;
  pimsmCB_t *           pimsmCb;
  PIMSM_DEBUG_PRINTF( "Entry" );

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );

  grpAddress = &pimsmDebugGrpAddr[index];
  PIMSM_DEBUG_PRINTF( "B4 calling pimsmSGDelete\r\n" );
  pimsmStarGDelete( pimsmCb, grpAddress );
}

/******************************************************************************
* @purpose  Dummy function, used to link this file in the image.
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugCBDeInit ( L7_uchar8 family )
{
  pimsmCB_t *           pimsmCb;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = (pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }

  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );

  pimsmCtrlBlockDeInit( ( void * )pimsmCb );
}

#ifdef PIMSM_NOTNEEDED
/******************************************************************************
* @purpose  Dummy function, used to link this file in the image.
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugIntfUp ( L7_uchar8 family, L7_uint32 rtrIfNum)
{
  pimsmCB_t *   pimsmCb;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }

  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "Family is %d\n", pimsmCb->family );
  pimsmIntfUp( pimsmCb, rtrIfNum );
}
#endif
/******************************************************************************
* @purpose  Dummy function, used to link this file in the image.
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugImDR ( L7_uchar8 family, L7_uint32 rtrIfNum )
{
  pimsmCB_t *   pimsmCb;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }

  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  pimsmIAmDR( pimsmCb, rtrIfNum );
}

#if PIMSM_NOTNEEDED
/******************************************************************************
* @purpose  Dummy function, used to link this file in the image.
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugIntfDown ( L7_uchar8 family, L7_uint32 rtrIfNum )
{
  pimsmCB_t *   pimsmCb;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }

  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  pimsmIntfDown( pimsmCb, rtrIfNum );
}
#endif
void pimsmDebugIntfDRPrioritySet (L7_uchar8 family, L7_uint32 rtrIfNum,
                                  L7_uint32 drPriority )
{
  pimsmInterfaceEntry_t *   intfEntry = L7_NULLPTR;
  pimsmCB_t *               pimsmCb;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );

  if(L7_NULLPTR == pimsmCb)
  {
    return;
  }

  if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &intfEntry )!=L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "interface entry is NULL\r\n" );
    return;
  }
  intfEntry->pimsmInterfaceDRPriority = drPriority;
}
/******************************************************************************
* @purpose  Dummy function, used to link this file in the image.
*
* @param
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugIntfStatInc ( L7_uchar8 family,
                             L7_uint32 rtrIfNum,
                             L7_uchar8 pktType,
                             pimsmPktRxTxMode_t RxOrTx )
{
  pimsmCB_t *   pimsmCb;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }

  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  pimsmStatsIncrement( pimsmCb, rtrIfNum, pktType, RxOrTx );
}

/******************************************************************************
* @purpose  Execute the FSM events for different states and Events
*
* @param    family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugDnStrmSGFSM ( L7_uchar8 family,
                             L7_uint32 index,
                             L7_uint32 state,
                             L7_uint32 event )
{
  pimsmCB_t *                       pimsmCb;
  pimsmSGNode_t *                   sgNode;
  pimsmSGIEntry_t *                 sgiEntry;
  pimsmDnStrmPerIntfSGEventInfo_t   sgEvntData;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );

  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );
  pimsmSGFind( pimsmCb,
               &pimsmDebugGrpAddr[index],
               &pimsmDebugSrcAddr[index],
               &sgNode );
  if(sgNode != ( pimsmSGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGNode created: %p\n", sgNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGNode create failed\n" );
    return;
  }
  if(pimsmSGIEntryCreate( pimsmCb, sgNode, index ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "SGI Entry create failed\n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "SGI Entry  %p\n", sgNode->pimsmSGIEntry[index] );
  memset( &sgEvntData, 0, sizeof( pimsmDnStrmPerIntfSGEventInfo_t ) );
  sgEvntData.rtrIfNum = index;

  /*pimsmDebugSGEntryShow(pimsmCb, &sgNode->pimsmSGEntry);
  pimsmDebugSGIEntryShow(pimsmCb, sgNode->pimsmSGIEntry[index]);*/
  PIMSM_DEBUG_PRINTF( "Max States:%d Events %d \n",
                      PIMSM_DNSTRM_S_G_SM_STATE_MAX,
                      PIMSM_DNSTRM_S_G_SM_EVENT_MAX );
  PIMSM_DEBUG_PRINTF( "State: %d and Event %d\n", state, event );                                                       
  sgiEntry = sgNode->pimsmSGIEntry[index];
  sgiEntry->pimsmSGIJoinPruneState = state; 
  memset( &sgEvntData, 0, sizeof( pimsmDnStrmPerIntfSGEventInfo_t ) );
  sgEvntData.rtrIfNum = index;
  sgEvntData.eventType = event;
  sgEvntData.holdtime = 60; 
  if(pimsmDnStrmPerIntfSGExecute( pimsmCb,
                                  sgNode,
                                  &sgEvntData ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "\nDebug:SGExecute Failed in %d state & %d event\n",
                        state,
                        event );
  }
}

/******************************************************************************
* @purpose  Execute the FSM events for different states and Events
*
* @param    family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugDnStrmSGRptFSM ( L7_uchar8 family,
                                L7_uint32 index,
                                L7_uint32 state,
                                L7_uint32 event )
{
  pimsmCB_t *                           pimsmCb;
  pimsmSGRptNode_t *                    sgRptNode;
  pimsmSGRptIEntry_t *                  sgRptiEntry;
  pimsmDnStrmPerIntfSGRptEventInfo_t    sgRptEvntData;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if(( state >= PIMSM_DNSTRM_STAR_G_SM_STATE_MAX ) ||
     ( event >= PIMSM_DNSTRM_STAR_G_SM_EVENT_MAX ))
  {
    PIMSM_DEBUG_PRINTF( "State or Event is wrong: limits are %d & %d\n",
                        PIMSM_DNSTRM_STAR_G_SM_STATE_MAX,
                        PIMSM_DNSTRM_STAR_G_SM_EVENT_MAX );    
    return;
  }

  pimsmSGRptNodeCreate( pimsmCb,
                        &pimsmDebugSrcAddr[index],
                        &pimsmDebugGrpAddr[index],
                        &sgRptNode );
  if(sgRptNode != ( pimsmSGRptNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGRptNode created: %p\n", sgRptNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGRptNode create failed\n" );
    return;
  }
  if(pimsmSGRptIEntryCreate( pimsmCb, sgRptNode, index ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "SGRptI Entry create failed\n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "SGRptI Entry  %p\n",
                      sgRptNode->pimsmSGRptIEntry[index] );

  /*pimsmDebugSGRptEntryShow(pimsmCb, &sgRptNode->pimsmSGRptEntry);
  pimsmDebugSGRptIEntryShow(pimsmCb, sgRptNode->pimsmSGRptIEntry[index]);*/
  PIMSM_DEBUG_PRINTF( "Max States:%d Events %d \n",
                      PIMSM_DNSTRM_S_G_RPT_SM_STATE_MAX,
                      PIMSM_DNSTRM_S_G_RPT_SM_EVENT_MAX );
  PIMSM_DEBUG_PRINTF( "State: %d and Event %d\n", state, event );                                                       
  sgRptiEntry = sgRptNode->pimsmSGRptIEntry[index];
  sgRptiEntry->pimsmSGRptIJoinPruneState = state;   
  memset( &sgRptEvntData, 0, sizeof( pimsmDnStrmPerIntfSGRptEventInfo_t ) );
  sgRptEvntData.rtrIfNum = index;
  sgRptEvntData.eventType = event;
  sgRptEvntData.holdtime = 60;  
  if(pimsmDnStrmPerIntfSGRptExecute( pimsmCb,
                                     sgRptNode,
                                     &sgRptEvntData ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF(
                      "\nDebug:SGRptExecute Failed in %d state & %d event\n",
                      state,
                      event );
  }
}


/******************************************************************************
* @purpose  Execute the FSM events for different states and Events
*
* @param    family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugDnStrmStarGFSM ( L7_uchar8 family,
                                L7_uint32 index,
                                L7_uint32 state,
                                L7_uint32 event )
{
  pimsmCB_t *                           pimsmCb;
  pimsmStarGNode_t *                    stargNode;
  pimsmStarGIEntry_t *                  stargiEntry;
  /*L7_uint32 statIndex = 0, evntIndex = 0;*/
  pimsmDnStrmPerIntfStarGEventInfo_t    stargEvntData;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if(( state >= PIMSM_DNSTRM_STAR_G_SM_STATE_MAX ) ||
     ( event >= PIMSM_DNSTRM_STAR_G_SM_EVENT_MAX ))
  {
    PIMSM_DEBUG_PRINTF( "State or Event is wrong: limits are %d & %d\n",
                        PIMSM_DNSTRM_STAR_G_SM_STATE_MAX,
                        PIMSM_DNSTRM_STAR_G_SM_EVENT_MAX );    
    return;
  }
  pimsmStarGNodeCreate( pimsmCb, &pimsmDebugGrpAddr[index], &stargNode );

  if(stargNode != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "StarGNode created: %p\n", stargNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "StarGNode create failed\n" );
    return;
  }
  if(pimsmStarGIEntryCreate( pimsmCb, stargNode, index ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "StarGI Entry create failed\n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "StarGI Entry  %p\n",
                      stargNode->pimsmStarGIEntry[index] );

  /*pimsmDebugStarGEntryShow(pimsmCb, &stargNode->pimsmStarGEntry);
  pimsmDebugStarGIEntryShow(pimsmCb, stargNode->pimsmStarGIEntry[index]);*/
  PIMSM_DEBUG_PRINTF( "Max States:%d Events %d \n",
                      PIMSM_DNSTRM_STAR_G_SM_STATE_MAX,
                      PIMSM_DNSTRM_STAR_G_SM_EVENT_MAX );
  PIMSM_DEBUG_PRINTF( "State: %d and Event %d\n", state, event );                           
  stargiEntry = stargNode->pimsmStarGIEntry[index];
  stargiEntry->pimsmStarGIJoinPruneState = state;
  memset( &stargEvntData, 0, sizeof( pimsmDnStrmPerIntfStarGEventInfo_t ) );
  stargEvntData.rtrIfNum = index;
  stargEvntData.eventType = event;
  stargEvntData.holdtime = 60;

  if(pimsmDnStrmPerIntfStarGExecute( pimsmCb,
                                     stargNode,
                                     &stargEvntData ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF(
                      "\nDebug:StarGExecute Failed in %d state & %d event\n",
                      state,
                      event );
  }
}


/******************************************************************************
* @purpose  Execute the FSM events for different states and Events
*
* @param    family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugDnStrmStarStarRpFSM ( L7_uchar8 family,
                                     L7_uint32 index,
                                     L7_uint32 state,
                                     L7_uint32 event )
{
  pimsmCB_t *                                   pimsmCb;
  pimsmStarStarRpNode_t *                       starStarRpNode;
  pimsmStarStarRpIEntry_t *                     starStarRpiEntry;
  pimsmDnStrmPerIntfStarStarRPEventInfo_t     starStarRpEvntData;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );

  if(( state >= PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_MAX ) ||
     ( event >= PIMSM_DNSTRM_STAR_STAR_RP_SM_EVENT_MAX ))
  {
    PIMSM_DEBUG_PRINTF( "State or Event is wrong: limits are %d & %d\n",
                        PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_MAX,
                        PIMSM_DNSTRM_STAR_STAR_RP_SM_EVENT_MAX );    
    return;
  }

  pimsmStarStarRpNodeCreate( pimsmCb,
                             &pimsmDebugSrcAddr[index],
                             &starStarRpNode );
  pimsmStarStarRpFind( pimsmCb, &pimsmDebugSrcAddr[index], &starStarRpNode );

  if(starStarRpNode != ( pimsmStarStarRpNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "(*,*,RP) created: %p\n", starStarRpNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "(*,*,RP) create failed\n" );
    return;
  }
  if(pimsmStarStarRpIEntryCreate( pimsmCb,
                                  starStarRpNode,
                                  index ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "(*,*,RP,I) Entry create failed\n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "(*,*,RP,I) Entry  %p\n",
                      starStarRpNode->pimsmStarStarRpIEntry[index] );

  /*pimsmDebugStarGEntryShow(pimsmCb, &stargNode->pimsmStarGEntry);
  pimsmDebugStarGIEntryShow(pimsmCb, stargNode->pimsmStarGIEntry[index]);*/
  PIMSM_DEBUG_PRINTF( "Max States:%d Events %d \n",
                      PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_MAX,
                      PIMSM_DNSTRM_STAR_STAR_RP_SM_EVENT_MAX );
  starStarRpiEntry = starStarRpNode->pimsmStarStarRpIEntry[index];
  PIMSM_DEBUG_PRINTF( "S(*,*,RP,I) Entry  %p\n", starStarRpiEntry );    
  PIMSM_DEBUG_PRINTF( "State: %d and Event %d\n", state, event );
  memset( &starStarRpEvntData,
          0,
          sizeof( pimsmDnStrmPerIntfStarStarRPEventInfo_t ) );
  starStarRpEvntData.rtrIfNum = index;
  starStarRpEvntData.eventType = event;
  starStarRpEvntData.holdtime = 05;
  starStarRpiEntry->pimsmStarStarRpIJoinPruneState = state;
  if(pimsmDnStrmPerIntfStarStarRPExecute( pimsmCb,
                                            starStarRpNode,
                                            &starStarRpEvntData ) !=
     L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF(
                      "\nDebug:StarStarRpExecute Failed in %d state & %d event\n",
                      state,
                      event );
  }
}
#ifdef PIMSM_NOTNEEDED
/******************************************************************************
* @purpose  stub routine to add an (*,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmV6DebugStarGEntryAdd ( L7_uchar8 family, L7_uint32 index )
{
  pimsmStarGNode_t      routeEntry;
  pimsmCB_t *           pimsmCb;
  L7_in6_addr_t         Ipv6Addr;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n",
                      pimsmCb->pimsmStarGTreeHeap );

  memset( &routeEntry, 0, sizeof( pimsmStarGNode_t ) );
  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

  pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugGrpAddr[index] );


  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
  pimsmDebugV6SrcStringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugSrcAddr[index] );


  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );
  inetCopy( &( routeEntry.pimsmStarGEntry.pimsmStarGGrpAddress ),
            &pimsmDebugGrpAddr[index] );
  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGAdd\r\n" );
  pimsmStarGAdd( pimsmCb, &routeEntry, &pimsmDebugSrcAddr[index] );
}                    

/******************************************************************************
* @purpose  stub routine to add an (*,G,I) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmV6DebugStarGIEntryAdd ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmStarGNode_t      routeEntry, * pRouteEntry;
  pimsmCB_t *           pimsmCb;
  L7_in6_addr_t         Ipv6Addr; 

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n",
                      pimsmCb->pimsmStarGTreeHeap );

  memset( &routeEntry, 0, sizeof( pimsmStarGNode_t ) );
  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

  pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugGrpAddr[index] );
  inetCopy( &( routeEntry.pimsmStarGEntry.pimsmStarGGrpAddress ),
            &pimsmDebugGrpAddr[index] );

  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
  pimsmDebugV6SrcStringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugSrcAddr[index] );

  inetCopy( &( routeEntry.pimsmStarGEntry.pimsmStarGRPAddress ),
            &pimsmDebugSrcAddr[index] );
  inetCopy( &( routeEntry.pimsmStarGEntry.pimsmStarGUpstreamNeighbor ),
            &pimsmDebugSrcAddr[index] );
  routeEntry.pimsmStarGEntry.pimsmStarGRPFIfIndex = index;
  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGAdd\r\n" );
  pimsmStarGAdd( pimsmCb, &routeEntry, L7_NULLPTR );
  pimsmStarGFirstGet( pimsmCb, &pRouteEntry );
  pimsmStarGIEntryCreate( pimsmCb, pRouteEntry, index );
}                    
#endif
/******************************************************************************
* @purpose  stub routine to find a (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmV6DebugStarGEntryFind ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmStarGNode_t *    routeEntry = L7_NULLPTR;
  pimsmCB_t *           pimsmCb;
  pimsmStarGEntry_t *   pimsmStarGEntry = L7_NULLPTR;
  L7_in6_addr_t         Ipv6Addr;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n",
                      pimsmCb->pimsmStarGTreeHeap );

  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

  pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugGrpAddr[index] );
  if(pimsmStarGFind( pimsmCb,
                     &pimsmDebugGrpAddr[index],
                     &routeEntry ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmSGFind() failed\r\n" );
    return;
  }
  pimsmStarGEntry = &routeEntry->pimsmStarGEntry; 
  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmStarGEntry->pimsmStarGGrpAddress );
  PIMSM_DEBUG_PRINTF( "RP Address : " );
  PIMSM_PRINT_ADDR( &pimsmStarGEntry->pimsmStarGRPAddress );
}                    
/******************************************************************************
* @purpose  stub routine to test pimsmSGTreeIntfDownUpdate()function (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugV6StarGTreeIntfDownUpdate ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *   pimsmCb;
  PIMSM_DEBUG_PRINTF( "Entry" );

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  pimsmStarGTreeIntfDownUpdate( pimsmCb, index );
}
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugV6StarGEntryDelete ( L7_uchar8 family, L7_uchar8 index )
{
  L7_inet_addr_t *      grpAddress;
  pimsmCB_t *           pimsmCb;
  L7_in6_addr_t         Ipv6Addr;

  PIMSM_DEBUG_PRINTF( "Entry" );

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );

  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
  pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugGrpAddr[index] );
  grpAddress = &pimsmDebugGrpAddr[index];
  PIMSM_DEBUG_PRINTF( "B4 calling pimsmSGDelete\r\n" );
  pimsmStarGDelete( pimsmCb, grpAddress );
}

/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

static void pimsmDebugV6StringToAddr ( L7_uint32 index,
                                       L7_in6_addr_t * Ipv6Addr )
{
  switch(index)
  {
    case 0:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_INADDR_ALL_PIM_ROUTERS,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 1:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_INADDR_PIM_ROUTER1,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 2:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_INADDR_PIM_ROUTER2,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 3:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_INADDR_PIM_ROUTER3,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 4:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_INADDR_PIM_ROUTER4,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    default:
      return;
  }
  return;
}
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
static void pimsmDebugV6SrcStringToAddr ( L7_uint32 index,
                                          L7_in6_addr_t * Ipv6Addr )
{
  switch(index)
  {
    case 0:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_SRC_ADDR,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 1:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_SRC_ADDR1,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 2:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_SRC_ADDR2,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 3:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_SRC_ADDR3,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 4:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_SRC_ADDR4,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    default:
      return;
  }
  return;
}
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugV6RpStringToAddr ( L7_uint32 index,
                                  L7_in6_addr_t * Ipv6Addr )
{
  switch(index)
  {
    case 0:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_RP_ADDR,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 1:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_RP_ADDR1,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 2:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_RP_ADDR2,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 3:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_RP_ADDR3,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    case 4:
      osapiInetPton( L7_AF_INET6,
                     PIMSM_IPV6_RP_ADDR4,
                     ( L7_uchar8 * )Ipv6Addr );
      break;
    default:
      return;
  }
  return;
}
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

void pimsmDebugSGEntryCreate ( L7_uchar8 family, 
                               L7_uchar8 grpIndex, L7_uchar8 srcIndex )
{
  pimsmCB_t *       pimsmCb;
  pimsmSGNode_t *   sgNode = L7_NULLPTR;
  L7_in6_addr_t         Ipv6Addr;
  pimsmSGIEntry_t   *sgIEntry = L7_NULLPTR;
  L7_uint32 ii=0;
  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n", pimsmCb->pimsmSGTreeHeap );

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGCreate\r\n" );
  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimsmDebugV6StringToAddr( grpIndex, &Ipv6Addr );
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugGrpAddr[grpIndex] );

    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimsmDebugV6StringToAddr( srcIndex, &Ipv6Addr );
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugSrcAddr[srcIndex]);
  }

  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[srcIndex],
                     &pimsmDebugGrpAddr[grpIndex],
                     &sgNode,L7_TRUE );

  if(sgNode != L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "pimsmSGNode Successful,sgNode is %p\r\n", sgNode );
    /* sgNode->pimsmSGEntry.pimsmSGRPFIfIndex = index;*/

    for(ii =0;ii<3;ii++)
    {
      if(pimsmSGIEntryCreate(pimsmCb,sgNode,srcIndex +ii) == L7_SUCCESS)
      {
        PIMSM_DEBUG_PRINTF("pimsmSGIEntryCreate Successful\r\n");
        PIMSM_DEBUG_PRINTF("ii is %d\r\n",ii);
        sgIEntry = sgNode->pimsmSGIEntry[srcIndex +ii];
        sgIEntry->pimsmSGIJoinPruneState =
        PIMSM_DNSTRM_S_G_SM_STATE_JOIN;
  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimsmDebugV6StringToAddr( srcIndex + ii, &Ipv6Addr );
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugSrcAddr[srcIndex + ii]);
  }
        inetCopy(&sgIEntry->pimsmSGIAssertWinnerAddress,
                 &pimsmDebugSrcAddr[srcIndex +ii]);
        sgIEntry->pimsmSGILocalMembership = L7_TRUE;
      }
    }
  }
}
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

void pimsmDebugStarGEntryCreate ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmCB_t *           pimsmCb;
  pimsmStarGNode_t *    stargNode = L7_NULLPTR;
  L7_in6_addr_t         Ipv6Addr;
  pimsmStarGIEntry_t *starGIEntry = L7_NULLPTR;
  L7_uint32 ii =0;
  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n", pimsmCb->pimsmSGTreeHeap );

  PIMSM_DEBUG_PRINTF("B4 Calling pimsmSGCreate,family is %d\r\n",pimsmCb->family);
  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimsmDebugV6StringToAddr( index, &Ipv6Addr );
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugGrpAddr[index] );

  }

  pimsmStarGNodeCreate(pimsmCb,
                       &pimsmDebugGrpAddr[index],
                       &stargNode);

  if(stargNode != L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "pimsmStarGNodeCreate Successful\r\n" );
    stargNode->pimsmStarGEntry.pimsmStarGRPFIfIndex = index;
    for(ii =0;ii<3;ii++)
    {
      if(pimsmStarGIEntryCreate(pimsmCb,stargNode,index +ii) == L7_SUCCESS)
      {
        PIMSM_DEBUG_PRINTF("pimsmStarGIEntryCreate Successful\r\n");
        PIMSM_DEBUG_PRINTF("ii is %d\r\n",ii);
        starGIEntry = stargNode->pimsmStarGIEntry[index +ii];
        starGIEntry->pimsmStarGIJoinPruneState =
        PIMSM_DNSTRM_STAR_G_SM_STATE_JOIN;

  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimsmDebugV6SrcStringToAddr( index+ii, &Ipv6Addr );
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugSrcAddr[index +ii] );
  }
        inetCopy(&starGIEntry->pimsmStarGIAssertWinnerAddress,
                 &pimsmDebugSrcAddr[index +ii]);
        starGIEntry->pimsmStarGILocalMembership = L7_TRUE;
      }
    }
  }
}
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

void pimsmDebugSGRptEntryCreate ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmCB_t *           pimsmCb;
  pimsmSGRptNode_t *    sgRptNode = L7_NULLPTR;
  pimsmSGRptIEntry_t *sgRptIEntry = L7_NULLPTR;
  L7_uint32 ii=0;
  L7_in6_addr_t         Ipv6Addr;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n",
                      pimsmCb->pimsmSGRptTreeHeap );

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGCreate\r\n" );
  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimsmDebugV6StringToAddr( index, &Ipv6Addr );
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugGrpAddr[index] );

    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimsmDebugV6StringToAddr( index, &Ipv6Addr );
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugSrcAddr[index]);
  }
  pimsmSGRptNodeCreate( pimsmCb,
                        &pimsmDebugSrcAddr[index],
                        &pimsmDebugGrpAddr[index],
                        &sgRptNode );
  if(sgRptNode != L7_NULLPTR)
  {
    for(ii =0;ii <3;ii++)
    {
      if(pimsmSGRptIEntryCreate(pimsmCb,sgRptNode,index +ii) == L7_SUCCESS)
      {
        PIMSM_DEBUG_PRINTF("pimsmSGRptIEntryCreate Successful\r\n");
        sgRptIEntry = sgRptNode->pimsmSGRptIEntry[index +ii];
        sgRptIEntry->pimsmSGRptIJoinPruneState =
        PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE;
      }
      PIMSM_DEBUG_PRINTF( "pimsmStarGNodeCreate Successful\r\n" );
    }
  }
}
#ifdef PIMSM_NOTNEEDED
/******************************************************************************
* @purpose  stub routine to add an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugV6SGEntryAdd ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmSGNode_t     routeEntry;
  pimsmCB_t *       pimsmCb;
  L7_in6_addr_t     Ipv6Addr;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n", pimsmCb->pimsmSGTreeHeap );

  memset( &routeEntry, 0, sizeof( pimsmSGNode_t ) );
  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

  pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugGrpAddr[index] );


  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
  pimsmDebugV6SrcStringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugSrcAddr[index] );


  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );

  inetCopy( &( routeEntry.pimsmSGEntry.pimsmSGSrcAddress ),
            &pimsmDebugSrcAddr[index] );
  inetCopy( &( routeEntry.pimsmSGEntry.pimsmSGGrpAddress ),
            &pimsmDebugGrpAddr[index] );
  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGAdd\r\n" );
  pimsmSGAdd( pimsmCb, &routeEntry );
}                    

/******************************************************************************
* @purpose  stub routine to add an (S,G,I) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugV6SGIEntryAdd ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmSGNode_t     routeEntry, * pRouteEntry;
  pimsmCB_t *       pimsmCb;
  L7_in6_addr_t     Ipv6Addr;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n", pimsmCb->pimsmSGTreeHeap );

  memset( &routeEntry, 0, sizeof( pimsmSGNode_t ) );
  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

  pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugGrpAddr[index] );


  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
  pimsmDebugV6SrcStringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugSrcAddr[index] );


  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );

  inetCopy( &( routeEntry.pimsmSGEntry.pimsmSGSrcAddress ),
            &pimsmDebugSrcAddr[index] );
  inetCopy( &( routeEntry.pimsmSGEntry.pimsmSGGrpAddress ),
            &pimsmDebugGrpAddr[index] );
  inetCopy( &( routeEntry.pimsmSGEntry.pimsmSGUpstreamNeighbor ),
            &pimsmDebugSrcAddr[index] );
  routeEntry.pimsmSGEntry.pimsmSGRPFIfIndex = index;
  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGAdd\r\n" );
  pimsmSGAdd( pimsmCb, &routeEntry );
  pimsmSGFirstGet( pimsmCb, &pRouteEntry );
  pimsmSGIEntryCreate( pimsmCb, pRouteEntry, index );
}                    
#endif
/******************************************************************************
* @purpose  stub routine to find a (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugV6SGEntryFind ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmSGNode_t *       routeEntry = L7_NULLPTR;
  pimsmCB_t *           pimsmCb;
  pimsmSGEntry_t *      pimsmSGEntry = L7_NULLPTR;
  L7_in6_addr_t         Ipv6Addr;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n", pimsmCb->pimsmSGTreeHeap );
  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

  pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugGrpAddr[index] );


  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
  pimsmDebugV6SrcStringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugSrcAddr[index] );


  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );
  PIMSM_DEBUG_PRINTF( "Source Address : " );
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index] );

  if(pimsmSGFind( pimsmCb,
                  &pimsmDebugGrpAddr[index],
                  &pimsmDebugSrcAddr[index],
                  &routeEntry ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmSGFind() failed\r\n" );
    return;
  }

  PIMSM_DEBUG_PRINTF( "Addrss of the node is %p ", routeEntry );
  pimsmSGEntry = &routeEntry->pimsmSGEntry; 
  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmSGEntry->pimsmSGGrpAddress );
  PIMSM_DEBUG_PRINTF( "Source Address : " );
  PIMSM_PRINT_ADDR( &pimsmSGEntry->pimsmSGSrcAddress );
  PIMSM_DEBUG_PRINTF( "Exit\r\n" );
}                    
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugV6SGEntryDelete ( L7_uchar8 family, L7_uchar8 index )
{
  L7_inet_addr_t *      grpAddress;
  L7_inet_addr_t *      srcAddress;
  pimsmCB_t *           pimsmCb;
  L7_in6_addr_t         Ipv6Addr;
  PIMSM_DEBUG_PRINTF( "Entry" );

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );

  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

  pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugGrpAddr[index] );


  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
  pimsmDebugV6SrcStringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugSrcAddr[index] );


  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );
  grpAddress = &pimsmDebugGrpAddr[index];
  srcAddress = &pimsmDebugSrcAddr[index];
  PIMSM_DEBUG_PRINTF( "B4 calling pimsmSGDelete\r\n" );
  pimsmSGDelete( pimsmCb, grpAddress, srcAddress );
}
#ifdef PIMSM_NOTNEEDED
/******************************************************************************
* @purpose  stub routine to add an (S,G) Rpt entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugV6SGRptEntryAdd ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmSGRptNode_t      routeEntry;
  pimsmCB_t *           pimsmCb;
  L7_in6_addr_t         Ipv6Addr;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Rpt Tree address is %p\r\n",
                      pimsmCb->pimsmSGRptTreeHeap );
  memset( &routeEntry, 0, sizeof( pimsmSGNode_t ) );
  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

  pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugGrpAddr[index] );


  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
  pimsmDebugV6SrcStringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugSrcAddr[index] );


  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );

  inetCopy( &( routeEntry.pimsmSGRptEntry.pimsmSGRptSrcAddress ),
            &pimsmDebugSrcAddr[index] );
  inetCopy( &( routeEntry.pimsmSGRptEntry.pimsmSGRptGrpAddress ),
            &pimsmDebugGrpAddr[index] );
  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGAdd\r\n" );
  pimsmSGRptAdd( pimsmCb, &routeEntry );
}                    

/******************************************************************************
* @purpose  stub routine to add an (S,G,I) Rpt entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugV6SGRptIEntryAdd ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmSGRptNode_t      routeEntry, * pRouteEntry;
  pimsmCB_t *           pimsmCb;
  L7_in6_addr_t         Ipv6Addr;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Rpt Tree address is %p\r\n",
                      pimsmCb->pimsmSGRptTreeHeap );

  memset( &routeEntry, 0, sizeof( pimsmSGRptNode_t ) );
  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

  pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugGrpAddr[index] );


  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
  pimsmDebugV6SrcStringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugSrcAddr[index] );


  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );
  inetCopy( &( routeEntry.pimsmSGRptEntry.pimsmSGRptSrcAddress ),
            &pimsmDebugSrcAddr[index] );
  inetCopy( &( routeEntry.pimsmSGRptEntry.pimsmSGRptGrpAddress ),
            &pimsmDebugGrpAddr[index] );
  /* inetCopy(&(routeEntry.pimsmSGRptEntry.pimsmSGUpstreamNeighbor), 
            &pimsmDebugSrcAddr[index]);*/
  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGAdd\r\n" );
  pimsmSGRptAdd( pimsmCb, &routeEntry );
  pimsmSGRptFirstGet( pimsmCb, &pRouteEntry );
  pimsmSGRptIEntryCreate( pimsmCb, pRouteEntry, index );
}                    
#endif
/******************************************************************************
* @purpose  stub routine to find a (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/  
void pimsmDebugV6SGRptEntryFind ( L7_uchar8 family, L7_uchar8 index )
{
  pimsmSGRptNode_t *    routeEntry = L7_NULLPTR;
  pimsmCB_t *           pimsmCb;
  pimsmSGRptEntry_t *   pimsmSGRptEntry = L7_NULLPTR;
  L7_in6_addr_t         Ipv6Addr;

  PIMSM_DEBUG_PRINTF( "Entry\r\n" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );
  PIMSM_DEBUG_PRINTF( "SG Tree address is %p\r\n",
                      pimsmCb->pimsmSGRptTreeHeap );

  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

  pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugGrpAddr[index] );


  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
  pimsmDebugV6SrcStringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugSrcAddr[index] );


  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );
  if(pimsmSGRptFind( pimsmCb,
                     &pimsmDebugGrpAddr[index],
                     &pimsmDebugSrcAddr[index],
                     &routeEntry ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmSGFind() failed\r\n" );
    return;
  }
  pimsmSGRptEntry = &routeEntry->pimsmSGRptEntry; 
  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmSGRptEntry->pimsmSGRptGrpAddress );
  PIMSM_DEBUG_PRINTF( "Source Address : " );
  PIMSM_PRINT_ADDR( &pimsmSGRptEntry->pimsmSGRptSrcAddress );
}                    
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugV6SGRptEntryDelete ( L7_uchar8 family, L7_uchar8 index )
{
  L7_inet_addr_t *      grpAddress;
  L7_inet_addr_t *      srcAddress;
  pimsmCB_t *           pimsmCb;
  L7_in6_addr_t         Ipv6Addr;


  PIMSM_DEBUG_PRINTF( "Entry" );

  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "CB address is %p\r\n", pimsmCb );

  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

  pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugGrpAddr[index] );


  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
  pimsmDebugV6SrcStringToAddr( index, &Ipv6Addr );        
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugSrcAddr[index] );


  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );
  grpAddress = &pimsmDebugGrpAddr[index];
  srcAddress = &pimsmDebugSrcAddr[index];
  PIMSM_DEBUG_PRINTF( "B4 calling pimsmSGDelete\r\n" );
  pimsmSGRptDelete( pimsmCb, grpAddress, srcAddress );
}
#ifdef PIMSM_NOTNEEDED
/**********Routines to Test pimsmapi.c ******************************/
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

void pimsmDebugStarGEntryGet ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  L7_inet_addr_t *      grpAddr = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  grpAddr = &pimsmDebugGrpAddr[index];

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmStarGEntryGet" );
  if(pimsmStarGEntryGet( ( void * )pimsmCb, grpAddr ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmStarGEntryGet() failed" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Success" );
} 

/******************************************************************************
* @purpose  
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

void pimsmDebugStarGEntryNext ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *       pimsmCb = L7_NULLPTR;
  L7_inet_addr_t    grpAddr;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  inetCopy( &grpAddr, &pimsmDebugGrpAddr[index] );

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmStarGEntryNextGet" );
  if(pimsmStarGEntryNextGet( ( void * )pimsmCb, &grpAddr ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmStarGEntryGet() failed" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &grpAddr );

  PIMSM_DEBUG_PRINTF( "Success " );
} 

/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

void pimsmDebugStarGIEntryGet ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  L7_inet_addr_t *      grpAddr = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  grpAddr = &pimsmDebugGrpAddr[index];

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmStarGIEntryGet" );
  if(pimsmStarGIEntryGet( ( void * )pimsmCb,
                          grpAddr,
                          index ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmStarGIEntryGet() failed" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Success" );
} 
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

void pimsmDebugStarGIEntryNextGet ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  L7_inet_addr_t *      grpAddr = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  grpAddr = &pimsmDebugGrpAddr[index];

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmStarGIEntryGet" );
  if(pimsmStarGIEntryNextGet( ( void * )pimsmCb,
                              grpAddr,
                              &index ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmStarGIEntryGet() failed" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Success....rtrIfNum is %d\r\n", index );
} 


/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

/****Routines to test SG Tree Objects */
void pimsmDebugSGEntryGet ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  L7_inet_addr_t *      grpAddr = L7_NULLPTR;
  L7_inet_addr_t *      srcAddr = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  grpAddr = &pimsmDebugGrpAddr[index];
  srcAddr = &pimsmDebugSrcAddr[index];

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGEntryGet" );
  if(pimsmSGEntryGet( ( void * )pimsmCb, grpAddr, srcAddr ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmSGEntryGet() failed" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Success " );
} 
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

void pimsmDebugSGEntryNext ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *       pimsmCb = L7_NULLPTR;
  L7_inet_addr_t    grpAddr;
  L7_inet_addr_t    srcAddr;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  inetCopy( &grpAddr, &pimsmDebugGrpAddr[index] );
  inetCopy( &srcAddr, &pimsmDebugSrcAddr[index] );

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmStarGEntryNextGet" );
  if(pimsmSGEntryNextGet( ( void * )pimsmCb,
                          &grpAddr,
                          &srcAddr ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmSGEntryNextGet() failed" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &grpAddr );
  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( &srcAddr );

  PIMSM_DEBUG_PRINTF( "Success " );
} 
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

void pimsmDebugSGIEntryGet ( L7_uchar8 family,
                             L7_uint32 index,
                             L7_uint32 rtrIfNum )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  L7_inet_addr_t *      grpAddr = L7_NULLPTR;
  L7_inet_addr_t *      srcAddr = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  grpAddr = &pimsmDebugGrpAddr[index];
  srcAddr = &pimsmDebugSrcAddr[index];

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGEntryGet" );
  if(pimsmSGIEntryGet( ( void * )pimsmCb,
                       grpAddr,
                       srcAddr,
                       rtrIfNum ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmStarGEntryGet() failed" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Success " );
} 
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGIEntrySetState ( L7_uchar8 family,
                                  L7_uint32 index,
                                  L7_uint32 rtrIfNum,
                                  L7_uint32 state )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  L7_inet_addr_t *      grpAddr = L7_NULLPTR;
  L7_inet_addr_t *      srcAddr = L7_NULLPTR;
  pimsmSGNode_t *       sgNode;
  pimsmSGIEntry_t *     sgiEntry;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  if(state >= PIMSM_DNSTRM_S_G_SM_STATE_MAX)
  {
    PIMSM_DEBUG_PRINTF( "Failed!! Invalid JoinPruneState!!\n" );
  }
  grpAddr = &pimsmDebugGrpAddr[index];
  srcAddr = &pimsmDebugSrcAddr[index];

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGEntryGet" );
  if(pimsmSGFind( ( void * )pimsmCb,
                  grpAddr,
                  srcAddr,
                  &sgNode ) == L7_SUCCESS)
  {
    sgiEntry = sgNode->pimsmSGIEntry[rtrIfNum];
    if(sgiEntry != L7_NULLPTR)
    {
      sgiEntry->pimsmSGIJoinPruneState = state;
      PIMSM_DEBUG_PRINTF( "Successfully set!!\n" );
      return;
    }
    else
    {
      PIMSM_DEBUG_PRINTF( "pimsmSGIEntryGet() failed\n" );
      return;
    }
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "pimsmSGEntryGet() failed\n" );
  }
  return;
}

/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

void pimsmDebugSGIEntryNextGet ( L7_uchar8 family,
                                 L7_uint32 index,
                                 L7_uint32 rtrIfNum )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  L7_inet_addr_t *      grpAddr = L7_NULLPTR;
  L7_inet_addr_t *      srcAddr = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  grpAddr = &pimsmDebugGrpAddr[index];
  srcAddr = &pimsmDebugSrcAddr[index];

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGEntryGet" );
  if(pimsmSGIEntryNextGet( ( void * )pimsmCb,
                           grpAddr,
                           srcAddr,
                           &rtrIfNum ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmStarGEntryGet() failed" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Success ...index is %d\r\n", rtrIfNum );
} 
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

void pimsmDebugSGObjectGet ( L7_uchar8 family,
                             L7_uint32 index,
                             L7_uint32 objId )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  L7_inet_addr_t *      grpAddr = L7_NULLPTR;
  L7_inet_addr_t *      srcAddr = L7_NULLPTR;
  L7_uint32             output = 0;
  L7_inet_addr_t        outAddr;
  L7_uint32 *           pOut;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  grpAddr = &pimsmDebugGrpAddr[index];
  srcAddr = &pimsmDebugSrcAddr[index];

  if(( objId == I_pimsmSGUpstreamNeighbor ) ||
     ( objId == I_pimsmSGRPFNextHop ) || ( objId ==
                                           I_pimsmSGRPFRouteAddress ) || (
                                                                         objId ==
                                                                         I_pimsmSGRPFRouteAddress ) ||
     ( objId == I_pimsmSGRPRegisterPMBRAddress ))
  {
    PIMSM_DEBUG_PRINTF( "Out put address must be of inet type" );
    pOut = ( L7_uint32 * )&outAddr;
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "Out put address must be of integer type" );
    pOut = &output;
  }

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmStarGEntryNextGet" );
  if(pimsmSGObjectGet( ( void * )pimsmCb,
                       grpAddr,
                       srcAddr,
                       objId,
                       pOut ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmStarGEntryGet() failed" );
    return;
  }
  if(( objId == I_pimsmSGUpstreamNeighbor ) ||
     ( objId == I_pimsmSGRPFNextHop ) || ( objId ==
                                           I_pimsmSGRPFRouteAddress ) || (
                                                                         objId ==
                                                                         I_pimsmSGRPFRouteAddress ) ||
     ( objId == I_pimsmSGRPRegisterPMBRAddress ))
  {
    PIMSM_DEBUG_PRINTF( "OutPut Address : " );
    PIMSM_PRINT_ADDR( &outAddr );
    PIMSM_PRINT_ADDR( ( L7_inet_addr_t * )pOut );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "Success ....output is %d\r\n", *pOut );
  }
} 
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

/*Stub routines to test SGRpt objects */
void pimsmDebugSGRptEntryGet ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  L7_inet_addr_t *      grpAddr = L7_NULLPTR;
  L7_inet_addr_t *      srcAddr = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  grpAddr = &pimsmDebugGrpAddr[index];
  srcAddr = &pimsmDebugSrcAddr[index];

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGEntryGet" );
  if(pimsmSGRptEntryGet( ( void * )pimsmCb,
                         grpAddr,
                         srcAddr ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmStarGEntryGet() failed" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Success " );
} 
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

void pimsmDebugSGRptEntryNext ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  L7_inet_addr_t *      grpAddr = L7_NULLPTR;
  L7_inet_addr_t *      srcAddr = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  grpAddr = &pimsmDebugGrpAddr[index];
  srcAddr = &pimsmDebugSrcAddr[index];

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmStarGEntryNextGet" );
  if(pimsmSGRptEntryNextGet( ( void * )pimsmCb,
                             grpAddr,
                             srcAddr ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmStarGEntryGet() failed" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( grpAddr );
  PIMSM_DEBUG_PRINTF( "Group Address : " );
  PIMSM_PRINT_ADDR( srcAddr );

  PIMSM_DEBUG_PRINTF( "Success " );
} 
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

void pimsmDebugSGRptIEntryGet ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  L7_inet_addr_t *      grpAddr = L7_NULLPTR;
  L7_inet_addr_t *      srcAddr = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  grpAddr = &pimsmDebugGrpAddr[index];
  srcAddr = &pimsmDebugSrcAddr[index];

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGEntryGet" );
  if(pimsmSGRptIEntryGet( ( void * )pimsmCb,
                          grpAddr,
                          srcAddr,
                          index ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmStarGEntryGet() failed" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Success " );
} 
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGRptIEntryNextGet ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  L7_inet_addr_t *      grpAddr = L7_NULLPTR;
  L7_inet_addr_t *      srcAddr = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  grpAddr = &pimsmDebugGrpAddr[index];
  srcAddr = &pimsmDebugSrcAddr[index];

  PIMSM_DEBUG_PRINTF( "B4 Calling pimsmSGEntryGet" );
  if(pimsmSGRptIEntryNextGet( ( void * )pimsmCb,
                              grpAddr,
                              srcAddr,
                              &index ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmStarGEntryGet() failed" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Success...RtrIfNum is %d\r\n ", index );
} 


/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param                                                                                                                                                                                                                                                                                                               pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugMyAssertMetric ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  pimsmStarGNode_t *    stargNode;
  pimsmSGNode_t *       sgNode;
  L7_RC_t               retVal;
  pimsmAssertMetric_t   metric;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Creating (S,G) for source " );
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index] );    
  PIMSM_DEBUG_PRINTF( "and Group" );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );    
  PIMSM_DEBUG_PRINTF( "\n" );
  pimsmStarGNodeCreate( pimsmCb, &pimsmDebugGrpAddr[index], &stargNode );
  if(stargNode != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "StarGNode created: %p\n", stargNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "StarGNode create failed\n" );
    return;
  }
  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE);
  if(sgNode != ( pimsmSGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGNode created: %p\n", sgNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGNode create failed\n" );
    return;
  }                 
  retVal = pimsmMyAssertMetric( pimsmCb, sgNode, stargNode, index, &metric );
  if(retVal == L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "Metric - RPTbit %d\n", metric.rpt_bit_flag );
    PIMSM_DEBUG_PRINTF( "Metric - Preference %d\n",
                        metric.metric_preference );
    PIMSM_DEBUG_PRINTF( "Metric - route metric %d\n", metric.route_metric );      
    PIMSM_DEBUG_PRINTF( "Metric - IPAddr " );
    PIMSM_PRINT_ADDR( &metric.ip_address );
  }
}


void pimsmDebugJoinStartStarRp(L7_uchar8 family,L7_uint32 index)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  pimsmStarStarRpNode_t *starStarRpNode = L7_NULLPTR;
  interface_bitset_t  intfBitMask;

  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }
  pimsmStarStarRpFind(pimsmCb,&pimsmDebugSrcAddr[index],&starStarRpNode);

  memset(&intfBitMask,0,sizeof(interface_bitset_t));
  if(starStarRpNode != L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("pimsmStarStarRpFind succesful\r\n");
    if(pimsmJoinsStarStarRp(pimsmCb,starStarRpNode,&intfBitMask) != L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF("pimsmJoinsStarStarRp failed\r\n");
    }
    else
    {
      if(BITX_TEST(&intfBitMask,index) != L7_NULL)
      {
        PIMSM_DEBUG_PRINTF("Bit set for index %d\r\n",index);
      }
    }
  }

  PIMSM_DEBUG_PRINTF("Exit");
}

void pimsmDebugJoinStarG(L7_uchar8 family,L7_uint32 index)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  pimsmStarGNode_t *starGNode = L7_NULLPTR;
  interface_bitset_t  intfBitMask;

  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  pimsmStarGFind(pimsmCb,&pimsmDebugGrpAddr[index],&starGNode);
  memset(&intfBitMask,0,sizeof(interface_bitset_t));
  if(starGNode != L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("pimsmStarGFind succesful\r\n");
    if(pimsmJoinsStarG(pimsmCb,starGNode,&intfBitMask) != L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF("pimsmJoinsStarG failed\r\n");
    }
    else
    {
      if(BITX_TEST(&intfBitMask,index) != L7_NULL)
      {
        PIMSM_DEBUG_PRINTF("Bit set for index %d\r\n",index);
      }
    }
  }

  PIMSM_DEBUG_PRINTF("Exit");

}
void pimsmDebugJoinSG(L7_uchar8 family,L7_uint32 index)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  pimsmSGNode_t *sGNode = L7_NULLPTR;
  interface_bitset_t  intfBitMask;

  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  pimsmSGFind(pimsmCb,&pimsmDebugGrpAddr[index],&pimsmDebugSrcAddr[index],
              &sGNode);
  memset(&intfBitMask,0,sizeof(interface_bitset_t));
  if(sGNode != L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("pimsmSGFind succesful\r\n");
    if(pimsmJoinsSG(pimsmCb,sGNode,&intfBitMask) != L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF("pimsmJoinsSG failed\r\n");
    }
    else
    {
      if(BITX_TEST(&intfBitMask,index) != L7_NULL)
      {
        PIMSM_DEBUG_PRINTF("Bit set for index %d\r\n",index);
      }
    }
  }

  PIMSM_DEBUG_PRINTF("Exit");

}
void pimsmDebugPrunesSGRpt(L7_uchar8 family,L7_uint32 index)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  pimsmSGRptNode_t *sgRptNode = L7_NULLPTR;
  interface_bitset_t  intfBitMask;

  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  pimsmSGRptFind(pimsmCb,&pimsmDebugGrpAddr[index],&pimsmDebugSrcAddr[index],
                 &sgRptNode);

  memset(&intfBitMask,0,sizeof(interface_bitset_t));
  if(sgRptNode != L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("pimsmSGRptFind succesful\r\n");
    if(pimsmPrunesSGRpt(pimsmCb,sgRptNode,&intfBitMask) != L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF("pimsmPrunesSGRpt failed\r\n");
    }
    else
    {
      if(BITX_TEST(&intfBitMask,index) != L7_NULL)
      {
        PIMSM_DEBUG_PRINTF("Bit set for index %d\r\n",index);
      }
    }
  }

  PIMSM_DEBUG_PRINTF("Exit");

}


void pimsmDebugStartStarRpJoinDesired(L7_uchar8 family,L7_uint32 index)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  pimsmStarStarRpNode_t *starStarRpNode = L7_NULLPTR;
  interface_bitset_t  intfBitMask;

  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }
  pimsmStarStarRpFind(pimsmCb,&pimsmDebugSrcAddr[index],&starStarRpNode);

  memset(&intfBitMask,0,sizeof(interface_bitset_t));
  if(starStarRpNode != L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("pimsmStarStarRpFind succesful\r\n");
    if(pimsmStarStarRpJoinDesired(pimsmCb,starStarRpNode) != L7_TRUE)
    {
      PIMSM_DEBUG_PRINTF("pimsmStarStarRpJoinDesired failed\r\n");
    }
    else
    {
      PIMSM_DEBUG_PRINTF("pimsmStarStarRpJoinDesired passed\r\n");
    }
  }

  PIMSM_DEBUG_PRINTF("Exit");
}


void pimsmDebugSGImdtOlist(L7_uchar8 family,L7_uint32 index)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  pimsmSGNode_t *sGNode = L7_NULLPTR;
  interface_bitset_t  intfBitMask;
  L7_BOOL oiListFlag = L7_FALSE;

  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  pimsmSGFind(pimsmCb,&pimsmDebugGrpAddr[index],&pimsmDebugSrcAddr[index],
              &sGNode);
  memset(&intfBitMask,0,sizeof(interface_bitset_t));
  if(sGNode != L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("pimsmSGFind succesful\r\n");
    if(pimsmSGImdtOlist(pimsmCb,sGNode,&intfBitMask) != L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF("pimsmJoinsSG failed\r\n");
    }
    else
    {
      BITX_IS_EMPTY(&intfBitMask,oiListFlag);
      if(oiListFlag == L7_FALSE)
      {
        PIMSM_DEBUG_PRINTF("OiList is Not Null\r\n");
      }
      else
      {
        PIMSM_DEBUG_PRINTF("OiList is  Null\r\n");
      }
      BITX_DEBUG_PRINT(&intfBitMask);
    }
  }

  PIMSM_DEBUG_PRINTF("Exit");

}

void pimsmDebugStarGImdtOlist(L7_uchar8 family,L7_uint32 index)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  pimsmStarGNode_t *starGNode = L7_NULLPTR;
  interface_bitset_t  intfBitMask;
  L7_BOOL oiListFlag = L7_FALSE;

  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  pimsmStarGFind(pimsmCb,&pimsmDebugGrpAddr[index],
                 &starGNode);
  memset(&intfBitMask,0,sizeof(interface_bitset_t));
  PIMSM_DEBUG_PRINTF("sizeof of interface_bitset_t is %d\r\n",sizeof(intfBitMask));
  PIMSM_DEBUG_PRINTF("Sizeof bits in interface_bitset_t is %d\r\n",sizeof(intfBitMask.bits));
  if(starGNode != L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("pimsmStarGFind succesful\r\n");
    if(pimsmStarGImdtOlist(pimsmCb,starGNode,&intfBitMask) != L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF("pimsmJoinsStarG failed\r\n");
    }
    else
    {
      BITX_IS_EMPTY(&intfBitMask,oiListFlag);
      if(oiListFlag == L7_FALSE)
      {
        PIMSM_DEBUG_PRINTF("OiList is Not Null\r\n");
      }
      else
      {
        PIMSM_DEBUG_PRINTF("OiList is  Null\r\n");
      }
      BITX_DEBUG_PRINT(&intfBitMask);
    }
  }

  PIMSM_DEBUG_PRINTF("Exit");

}

void pimsmDebugSGRptInhrtdOlist(L7_uchar8 family,L7_uint32 index)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  pimsmSGRptNode_t *sgRptNode = L7_NULLPTR;
  interface_bitset_t  intfBitMask;
  L7_BOOL oiListFlag = L7_FALSE;

  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  PIMSM_DEBUG_PRINTF("Family is %d\r\n",pimsmCb->family);
  pimsmSGRptFind(pimsmCb,&pimsmDebugGrpAddr[index],&pimsmDebugSrcAddr[index],
                 &sgRptNode);
  memset(&intfBitMask,0,sizeof(interface_bitset_t));
  if(sgRptNode != L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("pimsmSGRptFind succesful...Address of Node is %p\r\n",sgRptNode);
    PIMSM_DEBUG_PRINTF("Grp & Src Addresses are:");
    PIMSM_PRINT_ADDR(&pimsmDebugGrpAddr[index]);
    PIMSM_PRINT_ADDR(&pimsmDebugSrcAddr[index]);
    if(pimsmSGRptInhrtdOlist(pimsmCb,sgRptNode,&intfBitMask) != L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF("pimsmSGRptInhrtdOlist failed\r\n");
    }
    else
    {
      BITX_IS_EMPTY(&intfBitMask,oiListFlag);
      if(oiListFlag == L7_FALSE)
      {
        PIMSM_DEBUG_PRINTF("OiList is Not Null\r\n");
      }
      else
      {
        PIMSM_DEBUG_PRINTF("OiList is  Null\r\n");
      }
      BITX_DEBUG_PRINT(&intfBitMask);
    }
  }

  PIMSM_DEBUG_PRINTF("Exit");

}

void pimsmDebugStarGJoinDesired(L7_uchar8 family,L7_uint32 index)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  pimsmStarGNode_t *starGNode = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  pimsmStarGFind(pimsmCb,&pimsmDebugGrpAddr[index],
                 &starGNode);
  if(starGNode != L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("pimsmStarGFind succesful\r\n");
    if(pimsmStarGJoinDesired(pimsmCb,starGNode) != L7_TRUE)
    {
      PIMSM_DEBUG_PRINTF("pimsmStarGJoinDesired failed\r\n");
    }
    else
    {
      PIMSM_DEBUG_PRINTF("pimsmStarGJoinDesired passed\r\n");

    }
  }

  PIMSM_DEBUG_PRINTF("Exit");

}
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugNeighborRpfDashStarG ( L7_uchar8 family,
                                      L7_uint32 index,
                                      L7_BOOL     path )
{
  pimsmCB_t *               pimsmCb = L7_NULLPTR;
  pimsmStarGNode_t *        stargNode;
  pimsmStarGIEntry_t *      starGIEntry;
  L7_inet_addr_t            nbrAddr;
  L7_uint32                 rtrIfNum;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  pimsmStarGNodeCreate( pimsmCb, &pimsmDebugGrpAddr[index], &stargNode );

  if(stargNode != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "StarGNode created: %p\n", stargNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "StarGNode create failed\n" );
    return;
  }
  rtrIfNum = stargNode->pimsmStarGEntry.pimsmStarGRPFIfIndex;
  if(pimsmStarGIEntryCreate( pimsmCb, stargNode, rtrIfNum ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "StarGI Entry create failed\n" );
    return;
  }
  if(path == L7_TRUE)
  {
    starGIEntry = stargNode->pimsmStarGIEntry[rtrIfNum];
    starGIEntry->pimsmStarGIAssertState = 
    PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_LOSER;
    inetCopy( &starGIEntry->pimsmStarGIAssertWinnerAddress,
              &pimsmDebugSrcAddr[4] );
  }
  PIMSM_DEBUG_PRINTF( "StarGI Entry  %p\n",
                      stargNode->pimsmStarGIEntry[rtrIfNum] );
  if(pimsmStarGNeighborRpfDash( pimsmCb,
                                stargNode,
                                &nbrAddr ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmStarGNeighborRpfDash failed\n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "NBR addr retd is" ); 
  PIMSM_PRINT_ADDR( &nbrAddr );
}

/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugNeighborRpfDashSGRpt ( L7_uchar8 family,
                                      L7_uint32 index,
                                      L7_BOOL path )
{
  pimsmCB_t *               pimsmCb = L7_NULLPTR;
  pimsmSGRptNode_t *        sgRptNode;
  pimsmSGNode_t *           sgNode;
  pimsmSGIEntry_t *         sgiEntry;
  pimsmStarGNode_t *        starGNode;
  L7_inet_addr_t            nbrAddr;
  L7_uint32                 rtrIfNum;
  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Creating (S,G,Rpt) for source " );
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index] );    
  PIMSM_DEBUG_PRINTF( "and Group" );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );    
  PIMSM_DEBUG_PRINTF( "\n" );
  pimsmSGRptNodeCreate( pimsmCb,
                        &pimsmDebugSrcAddr[index],
                        &pimsmDebugGrpAddr[index],
                        &sgRptNode );

  if(sgRptNode != ( pimsmSGRptNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGRptNode created: %p\n", sgRptNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGRptNode create failed\n" );
    return;
  }
  rtrIfNum = sgRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex;
  if(pimsmSGRptIEntryCreate( pimsmCb, sgRptNode, rtrIfNum ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "SGRptI Entry create failed\n" );
    return;
  }
  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );
  if(sgNode != ( pimsmSGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "sgNode created: %p\n", sgNode );
  }
  if(pimsmSGIEntryCreate( pimsmCb, sgNode, rtrIfNum ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "SGI Entry create failed\n" );
    return;
  }
  if(path == L7_TRUE)
  {
    sgiEntry = sgNode->pimsmSGIEntry[rtrIfNum];
    sgiEntry->pimsmSGIAssertState = 
    PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER;
    inetCopy( &sgiEntry->pimsmSGIAssertWinnerAddress,
              &pimsmDebugSrcAddr[4] );
  }
  pimsmStarGNodeCreate( pimsmCb, &pimsmDebugGrpAddr[index], &starGNode );
  if(starGNode != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "stargNode created: %p\n", starGNode );
  }
  if(pimsmStarGIEntryCreate( pimsmCb, starGNode, rtrIfNum ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "StarGI Entry create failed\n" );
    return;
  }
  memset( &nbrAddr, 0, sizeof( L7_inet_addr_t ) );
  if(pimsmSGRptNeighborRpfDash( pimsmCb,
                                sgRptNode,
                                &nbrAddr ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmSGRptNeighborRpfDash failed\n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "NBR addr retd is" ); 
  PIMSM_PRINT_ADDR( &nbrAddr );
}



/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugNeighborRpfDashSG ( L7_uchar8 family,
                                   L7_uint32 index,
                                   L7_BOOL path )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  pimsmSGNode_t *       sgNode;
  pimsmSGIEntry_t *     sgiEntry;
  L7_inet_addr_t        nbrAddr;
  L7_uint32             rtrIfNum;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Creating (S,G) for source " );
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index] );    
  PIMSM_DEBUG_PRINTF( "and Group" );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );    
  PIMSM_DEBUG_PRINTF( "\n" );
  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );

  if(sgNode != ( pimsmSGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGNode created: %p\n", sgNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGNode create failed\n" );
    return;
  }
  rtrIfNum = sgNode->pimsmSGEntry.pimsmSGRPFIfIndex;
  if(pimsmSGIEntryCreate( pimsmCb, sgNode, rtrIfNum ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "SGI Entry create failed\n" );
    return;
  }
  if(path == L7_TRUE)
  {
    sgiEntry = sgNode->pimsmSGIEntry[rtrIfNum];
    sgiEntry->pimsmSGIAssertState = 
    PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER;
    inetCopy( &sgiEntry->pimsmSGIAssertWinnerAddress,
              &pimsmDebugSrcAddr[4] );
  }
  PIMSM_DEBUG_PRINTF( "SGI Entry  %p\n", sgNode->pimsmSGIEntry[rtrIfNum] );
  if(pimsmSGNeighborRpfDash( pimsmCb, sgNode, &nbrAddr ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "pimsmSGNeighborRpfDash failed\n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "NBR addr retd is" ); 
  PIMSM_PRINT_ADDR( &nbrAddr );
}
#endif
/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGJoinDesired ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  pimsmSGNode_t *       sgNode;
  L7_BOOL               retVal;
  pimsmSGIEntry_t *     sgiEntry;
  L7_uint32             i;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Creating (S,G) for source " );
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index] );    
  PIMSM_DEBUG_PRINTF( "and Group" );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );    
  PIMSM_DEBUG_PRINTF( "\n" );
  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );

  if(sgNode != ( pimsmSGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGNode created: %p\n", sgNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGNode create failed\n" );
    return;
  }
  for(i = 0; i < 5; i++)
  {
    if(pimsmSGIEntryCreate( pimsmCb, sgNode, i ) != L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF( "SGIentry create failed\n" );
      return;
    }
    sgiEntry = sgNode->pimsmSGIEntry[i];
    /*sgiEntry->pimsmSGIJoinPruneState = 
                  PIMSM_DNSTRM_PER_INTF_S_G_SM_STATE_JOIN;*/
  }
  retVal = pimsmSGJoinDesired( pimsmCb, sgNode );
  PIMSM_DEBUG_PRINTF( "Ret Value is: %d \n", retVal );
}


/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugRPTJoinDesired ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *                   pimsmCb = L7_NULLPTR;
  pimsmStarGNode_t *            stargNode;
  pimsmStarStarRpNode_t *       starStarRpNode;
  pimsmStarStarRpIEntry_t *   starStarRpIEntry;
  L7_BOOL                       retVal;
  L7_uint32           i;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Creating (S,G) for source " );
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index] );    
  PIMSM_DEBUG_PRINTF( "and Group" );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );    
  PIMSM_DEBUG_PRINTF( "\n" );
  pimsmStarGNodeCreate( pimsmCb, &pimsmDebugGrpAddr[index], &stargNode );
  if(stargNode != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "StarGNode created: %p\n", stargNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "StarGNode create failed\n" );
    return;
  }
  pimsmStarStarRpNodeCreate( pimsmCb, &pimsmDebugRpAddr[0], &starStarRpNode );
  if(starStarRpNode != ( pimsmStarStarRpNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "(*,*,RP) Node created: %p\n", starStarRpNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "(*,*,RP) Node create failed\n" );
    return;
  }
  for(i = 0; i < 5; i++)
  {
    if(pimsmStarStarRpIEntryCreate( pimsmCb, starStarRpNode, i ) != L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF( "SGIentry create failed\n" );
      return;
    }
    starStarRpIEntry = starStarRpNode->pimsmStarStarRpIEntry[i];
    starStarRpIEntry->pimsmStarStarRpIJoinPruneState= 
    PIMSM_DNSTRM_STAR_STAR_RP_SM_STATE_JOIN;
  }
  retVal = pimsmRPTJoinDesired( pimsmCb, &pimsmDebugGrpAddr[index] );
  PIMSM_DEBUG_PRINTF( "Debug:Ret Value is: %d \n", retVal );
}



/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGRptPruneDesired ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *               pimsmCb = L7_NULLPTR;
  pimsmStarGNode_t *        stargNode;
  pimsmSGNode_t *           sgNode;
  pimsmSGRptNode_t *        sgRptNode;
  L7_BOOL                   retVal;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Creating (S,G) for source " );
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index] );    
  PIMSM_DEBUG_PRINTF( "and Group" );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );    
  PIMSM_DEBUG_PRINTF( "\n" );
  pimsmStarGNodeCreate( pimsmCb, &pimsmDebugGrpAddr[index], &stargNode );
  if(stargNode != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "StarGNode created: %p\n", stargNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "StarGNode create failed\n" );
    return;
  }
  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );
  pimsmSGRptNodeCreate( pimsmCb,
                        &pimsmDebugSrcAddr[index],
                        &pimsmDebugGrpAddr[index],
                        &sgRptNode );
  retVal = pimsmSGRptPruneDesired( pimsmCb, sgRptNode );
  PIMSM_DEBUG_PRINTF( "Debug:Ret Value is: %d \n", retVal );
}




/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugCheckSwitchToSpt ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  pimsmStarGNode_t *    stargNode;
  pimsmSGNode_t *       sgNode;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Creating (S,G) for source " );
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index] );    
  PIMSM_DEBUG_PRINTF( "and Group" );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );    
  PIMSM_DEBUG_PRINTF( "\n" );
  pimsmStarGNodeCreate( pimsmCb, &pimsmDebugGrpAddr[index], &stargNode );
  if(stargNode != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "StarGNode created: %p\n", stargNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "StarGNode create failed\n" );
    return;
  }
  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );
  pimsmCheckSwitchToSpt( pimsmCb,
                         &pimsmDebugSrcAddr[index],
                         &pimsmDebugGrpAddr[index] );
}


/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugCouldRegister ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *       pimsmCb = L7_NULLPTR;
  pimsmSGNode_t *   sgNode;
  L7_BOOL           retVal;
  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Creating (S,G) for source " );
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index] );    
  PIMSM_DEBUG_PRINTF( "and Group" );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );    
  PIMSM_DEBUG_PRINTF( "\n" );
  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );

  if(sgNode != ( pimsmSGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGNode created: %p\n", sgNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGNode create failed\n" );
    return;
  }
  retVal = pimsmCouldRegister( pimsmCb, sgNode );
  PIMSM_DEBUG_PRINTF( "Ret Value is: %d \n", retVal );
}




/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGIAssertTrackingDesired ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *                   pimsmCb = L7_NULLPTR;
  pimsmStarGNode_t *            stargNode;
  pimsmSGNode_t *               sgNode;
  pimsmSGRptNode_t *            sgRptNode;
  pimsmStarStarRpNode_t *       starStarRpNode;
  L7_inet_addr_t                rpAddr;
  L7_BOOL                       retVal;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Creating (S,G) for source " );
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index] );    
  PIMSM_DEBUG_PRINTF( "and Group" );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );    
  PIMSM_DEBUG_PRINTF( "\n" );
  pimsmStarGNodeCreate( pimsmCb, &pimsmDebugGrpAddr[index], &stargNode );
  if(stargNode != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "StarGNode created: %p\n", stargNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "StarGNode create failed\n" );
    return;
  }
  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );
  pimsmSGRptNodeCreate( pimsmCb,
                        &pimsmDebugSrcAddr[index],
                        &pimsmDebugGrpAddr[index],
                        &sgRptNode );
  pimsmRpAddressGet( pimsmCb, &pimsmDebugGrpAddr[index], &rpAddr );
  pimsmStarStarRpNodeCreate( pimsmCb, &rpAddr, &starStarRpNode );
  retVal = pimsmSGIAssertTrackingDesired( pimsmCb, sgNode, index );
  PIMSM_DEBUG_PRINTF( "Debug:Ret Value is: %d \n", retVal );
}


/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugStarGIAssertTrackingDesired ( L7_uchar8 family,
                                             L7_uint32 index )
{
  pimsmCB_t *           pimsmCb = L7_NULLPTR;
  pimsmStarGNode_t *    stargNode;
  L7_BOOL               retVal;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Creating (*,G) for GROUP " );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );    
  PIMSM_DEBUG_PRINTF( "\n" );
  pimsmStarGNodeCreate( pimsmCb, &pimsmDebugGrpAddr[index], &stargNode );
  if(stargNode != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "StarGNode created: %p\n", stargNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "StarGNode create failed\n" );
    return;
  }
  retVal = pimsmStarGIAssertTrackingDesired( pimsmCb, stargNode, index );
  PIMSM_DEBUG_PRINTF( "Return Value is %d\n", retVal );
}


/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugStarGICouldAssert ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *                   pimsmCb = L7_NULLPTR;
  pimsmStarGNode_t *            stargNode;
  pimsmStarStarRpNode_t *       starStarRpNode;
  L7_inet_addr_t                rpAddr;
  L7_BOOL                       retVal;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Creating (S,G) for source " );
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index] );    
  PIMSM_DEBUG_PRINTF( "and Group" );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );    
  PIMSM_DEBUG_PRINTF( "\n" );
  pimsmStarGNodeCreate( pimsmCb, &pimsmDebugGrpAddr[index], &stargNode );
  if(stargNode != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "StarGNode created: %p\n", stargNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "StarGNode create failed\n" );
    return;
  }
  pimsmRpAddressGet( pimsmCb, &pimsmDebugGrpAddr[index], &rpAddr );
  pimsmStarStarRpNodeCreate( pimsmCb, &rpAddr, &starStarRpNode );

  if(starStarRpNode != ( pimsmStarStarRpNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "(*,*,RP) Node created: %p\n", starStarRpNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "(*,*,RP) Node create failed\n" );
    return;
  }
  retVal = pimsmStarGICouldAssert( pimsmCb, stargNode, index );
  PIMSM_DEBUG_PRINTF( "Debug:Ret Value is: %d \n", retVal );
}





/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGICouldAssert ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *                   pimsmCb = L7_NULLPTR;
  pimsmStarGNode_t *            stargNode;
  pimsmSGNode_t *               sgNode;
  pimsmSGRptNode_t *            sgRptNode;
  pimsmStarStarRpNode_t *       starStarRpNode;
  L7_inet_addr_t                rpAddr;
  L7_BOOL                       retVal;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Creating (S,G) for source " );
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index] );    
  PIMSM_DEBUG_PRINTF( "and Group" );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );    
  PIMSM_DEBUG_PRINTF( "\n" );
  pimsmStarGNodeCreate( pimsmCb, &pimsmDebugGrpAddr[index], &stargNode );
  if(stargNode != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "StarGNode created: %p\n", stargNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "StarGNode create failed\n" );
    return;
  }
  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );
  if(sgNode != ( pimsmSGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGNode created: %p\n", sgNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGNode create failed\n" );
    return;
  }                 
  pimsmSGRptNodeCreate( pimsmCb,
                        &pimsmDebugSrcAddr[index],
                        &pimsmDebugGrpAddr[index],
                        &sgRptNode );
  if(sgRptNode != ( pimsmSGRptNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGRptNode created: %p\n", sgRptNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGRptNode create failed\n" );
    return;
  }                 
  pimsmRpAddressGet( pimsmCb, &pimsmDebugGrpAddr[index], &rpAddr );
  pimsmStarStarRpNodeCreate( pimsmCb, &rpAddr, &starStarRpNode );
  if(starStarRpNode != ( pimsmStarStarRpNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "StarGNode created: %p\n", starStarRpNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "StarGNode create failed\n" );
    return;
  }
  retVal = pimsmSGICouldAssert( pimsmCb, sgNode, index );
  PIMSM_DEBUG_PRINTF( "Debug:Ret Value is: %d \n", retVal );
}

/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSGInhrtdOlist ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *               pimsmCb = L7_NULLPTR;
  pimsmSGNode_t *           sgNode;
  pimsmSGRptNode_t *        sgRptNode;
  pimsmSGRptIEntry_t *    sgRptIEntry;
  L7_RC_t                   retVal;
  interface_bitset_t        oifList;
  L7_uint32                 i;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Creating (S,G) for source " );
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index] );    
  PIMSM_DEBUG_PRINTF( "and Group" );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );    
  PIMSM_DEBUG_PRINTF( "\n" );

  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );
  if(sgNode != ( pimsmSGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGNode created: %p\n", sgNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGNode create failed\n" );
    return;
  }                 
  pimsmSGRptNodeCreate( pimsmCb,
                        &pimsmDebugSrcAddr[index],
                        &pimsmDebugGrpAddr[index],
                        &sgRptNode );
  if(sgRptNode != ( pimsmSGRptNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGRptNode created: %p\n", sgRptNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGRptNode create failed\n" );
    return;
  }                 
  for(i = 0; i < 5; i++)
  {
    if(pimsmSGRptIEntryCreate( pimsmCb, sgRptNode, i ) != L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF( "SGRptIentry create failed\n" );
      return;
    }
    sgRptIEntry = sgRptNode->pimsmSGRptIEntry[i];
    sgRptIEntry->pimsmSGRptIJoinPruneState = 
    PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE;
  }

  retVal = pimsmSGInhrtdOlist( pimsmCb, sgNode, &oifList );
  PIMSM_DEBUG_PRINTF( "Debug:Ret Value is: %d \n", retVal );
}


/******************************************************************************
* @purpose  stub routine to delete an (S,G) entry (Testing only)
*
* @param        pimsmCb          @b{(input)}
* @param        index           @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugSPTbitUpdate ( L7_uchar8 family, L7_uint32 index )
{
  pimsmCB_t *               pimsmCb = L7_NULLPTR;
  pimsmStarGNode_t *        stargNode;
  pimsmSGNode_t *           sgNode;
  pimsmSGRptNode_t *        sgRptNode;

  PIMSM_DEBUG_PRINTF( "Entry" );
  pimsmCb = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet( family );

  if(pimsmCb == ( pimsmCB_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "Invalid CB (NULLPTR) \n" );
    return;
  }
  PIMSM_DEBUG_PRINTF( "Creating (S,G) for source " );
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index] );    
  PIMSM_DEBUG_PRINTF( "and Group" );
  PIMSM_PRINT_ADDR( &pimsmDebugGrpAddr[index] );    
  PIMSM_DEBUG_PRINTF( "\n" );
  pimsmStarGNodeCreate( pimsmCb, &pimsmDebugGrpAddr[index], &stargNode );
  if(stargNode != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "StarGNode created: %p\n", stargNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "StarGNode create failed\n" );
    return;
  }
  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );
  if(sgNode != ( pimsmSGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGNode created: %p\n", sgNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGNode create failed\n" );
    return;
  }                 
  pimsmSGRptNodeCreate( pimsmCb,
                        &pimsmDebugSrcAddr[index],
                        &pimsmDebugGrpAddr[index],
                        &sgRptNode );
  if(sgRptNode != ( pimsmSGRptNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGRptNode created: %p\n", sgRptNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGRptNode create failed\n" );
    return;
  }                 
  pimsmSPTbitUpdate( pimsmCb, sgNode, index );
}

/******************************************************************************
* @purpose  Execute the FSM events for different states and Events
*
* @param    family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugUpStrmSGFSM ( L7_uchar8 family,
                             L7_uint32 index,
                             L7_uint32 state,
                             L7_uint32 event )
{
  pimsmCB_t *                       pimsmCb;
  pimsmSGNode_t *                   sgNode;
  pimsmSGEntry_t *                  sgEntry;
  pimsmUpStrmSGEventInfo_t          sgEvntData;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if((state >= PIMSM_UPSTRM_S_G_SM_STATE_MAX) ||
     (event >= PIMSM_UPSTRM_S_G_SM_EVENT_MAX))
  {
    PIMSM_DEBUG_PRINTF(
                      "Invalid Input!!Max State %d and and MAx Event %d\n", 
                      PIMSM_UPSTRM_S_G_SM_STATE_MAX,
                      PIMSM_UPSTRM_S_G_SM_EVENT_MAX);
    return;
  }
  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );
  if(sgNode != ( pimsmSGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGNode created: %p\n", sgNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGNode create failed\n" );
    return;
  }
  memset( &sgEvntData, 0, sizeof( pimsmDnStrmPerIntfSGEventInfo_t ) );
  sgEvntData.rtrIfNum = index;

  /*pimsmDebugSGEntryShow(pimsmCb, &sgNode->pimsmSGEntry);*/
  PIMSM_DEBUG_PRINTF( "Max States:%d Events %d \n",
                      PIMSM_UPSTRM_S_G_SM_STATE_MAX,
                      PIMSM_UPSTRM_S_G_SM_EVENT_MAX );
  PIMSM_DEBUG_PRINTF( "State: %d and Event %d\n", state, event );                                                       
  sgEntry = &sgNode->pimsmSGEntry;
  sgEntry->pimsmSGUpstreamJoinState = state; 
  memset( &sgEvntData, 0, sizeof( pimsmDnStrmPerIntfSGEventInfo_t ) );
  sgEvntData.rtrIfNum = index;
  sgEvntData.eventType = event;
  sgEvntData.holdtime = 10; 
  if(pimsmUpStrmSGExecute( pimsmCb, sgNode, &sgEvntData ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "\nDebug:SGExecute Failed in %d state & %d event\n",
                        state,
                        event );
  }
}

/******************************************************************************
* @purpose  Execute the FSM events for different states and Events
*
* @param    family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugUpStrmStarGFSM ( L7_uchar8 family,
                                L7_uint32 index,
                                L7_uint32 state,
                                L7_uint32 event )
{
  pimsmCB_t *                          pimsmCb;
  pimsmStarGNode_t *                   stargNode;
  pimsmStarGEntry_t *                  stargEntry;
  pimsmUpStrmStarGEventInfo_t          stargEvntData;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if((state >= PIMSM_UPSTRM_STAR_G_SM_STATE_MAX) ||
     (event >= PIMSM_UPSTRM_STAR_G_SM_EVENT_MAX))
  {
    PIMSM_DEBUG_PRINTF(
                      "Invalid Input!!Max State %d and and MAx Event %d\n", 
                      PIMSM_UPSTRM_STAR_G_SM_STATE_MAX,
                      PIMSM_UPSTRM_STAR_G_SM_EVENT_MAX);
    return;
  }
  pimsmStarGNodeCreate( pimsmCb,
                        &pimsmDebugGrpAddr[index],
                        &stargNode );
  if(stargNode != ( pimsmStarGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "STAR G Node created: %p\n", stargNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "STAR G Node create failed\n" );
    return;
  }
  memset( &stargEvntData, 0, sizeof( pimsmUpStrmStarGEventInfo_t ) );
  stargEvntData.rtrIfNum = index;

  /*pimsmDebugStarGEntryShow(pimsmCb, &stargNode->pimsmStarGEntry);*/
  PIMSM_DEBUG_PRINTF( "Max States:%d Events %d \n",
                      PIMSM_UPSTRM_STAR_G_SM_STATE_MAX,
                      PIMSM_UPSTRM_STAR_G_SM_EVENT_MAX );

  PIMSM_DEBUG_PRINTF( "State: %d and Event %d\n", state, event );                                                       
  stargEntry = &stargNode->pimsmStarGEntry;
  stargEntry->pimsmStarGUpstreamJoinState = state; 
  memset( &stargEvntData, 0, sizeof( pimsmDnStrmPerIntfStarGEventInfo_t ) );
  stargEvntData.rtrIfNum = index;
  stargEvntData.eventType = event;
  stargEvntData.holdtime = 10; 
  if(pimsmUpStrmStarGExecute( pimsmCb, stargNode, &stargEvntData ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "\nDebug:(*,G) Execute Failed in %d state & %d event\n",
                        state,
                        event );
  }
}


/******************************************************************************
* @purpose  Execute the FSM events for different states and Events
*
* @param    family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugUpStrmStarStarRPFSM ( L7_uchar8 family,
                                     L7_uint32 index,
                                     L7_uint32 state,
                                     L7_uint32 event )
{
  pimsmCB_t *                             pimsmCb;
  pimsmStarStarRpNode_t *                   starStarRpNode;
  pimsmStarStarRpEntry_t *                  starStarRpEntry;
  pimsmUpStrmStarStarRPEventInfo_t          starStarRpEvntData;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if((state >= PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_MAX) ||
     (event >= PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_MAX))
  {
    PIMSM_DEBUG_PRINTF(
                      "Invalid Input!!Max State %d and and MAx Event %d\n", 
                      PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_MAX,
                      PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_MAX);
    return;
  }
  pimsmStarStarRpNodeCreate( pimsmCb,
                             &pimsmDebugRpAddr[index],
                             &starStarRpNode );
  if(starStarRpNode != ( pimsmStarStarRpNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "(*,*,RP) Node created: %p\n", starStarRpNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "(*,*,RP) Node create failed\n" );
    return;
  }
  memset( &starStarRpEvntData, 0, sizeof( pimsmUpStrmStarStarRPEventInfo_t ) );
  starStarRpEvntData.rtrIfNum = index;

  /*pimsmDebugStarStarRpEntryShow(pimsmCb, &starStarRpNode->pimsmStarStarRpEntry);*/
  PIMSM_DEBUG_PRINTF( "Max States:%d Events %d \n",
                      PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_MAX,
                      PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_MAX );

  PIMSM_DEBUG_PRINTF( "State: %d and Event %d\n", state, event );                                                       
  starStarRpEntry = &starStarRpNode->pimsmStarStarRpEntry;
  starStarRpEntry->pimsmStarStarRpUpstreamJoinState = state; 
  memset( &starStarRpEvntData, 0, sizeof( pimsmUpStrmStarStarRPEventInfo_t ) );
  starStarRpEvntData.rtrIfNum = index;
  starStarRpEvntData.eventType = event;
  starStarRpEvntData.holdtime = 10; 
  if(pimsmUpStrmStarStarRpExecute( pimsmCb, starStarRpNode, 
                                   &starStarRpEvntData ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "\nDebug:(*,*,RP) Execute Failed in %d state & %d event\n",
                        state,
                        event );
  }
}


/******************************************************************************
* @purpose  Execute the FSM events for different states and Events
*
* @param    family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugUpStrmSGRptFSM ( L7_uchar8 family,
                                L7_uint32 index,
                                L7_uint32 state,
                                L7_uint32 event )
{
  pimsmCB_t *                       pimsmCb;
  pimsmSGRptNode_t *                   sgRptNode;
  pimsmSGRptEntry_t *                  sgRptEntry;
  pimsmUpStrmSGRptEventInfo_t          sgRptEvntData;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if((state >= PIMSM_UPSTRM_S_G_RPT_SM_STATE_MAX) ||
     (event >= PIMSM_UPSTRM_S_G_RPT_SM_EVENT_MAX))
  {
    PIMSM_DEBUG_PRINTF(
                      "Invalid Input!!Max State %d and and MAx Event %d\n", 
                      PIMSM_UPSTRM_S_G_RPT_SM_STATE_MAX,
                      PIMSM_UPSTRM_S_G_RPT_SM_EVENT_MAX);
    return;
  }
  pimsmSGRptNodeCreate( pimsmCb,
                        &pimsmDebugSrcAddr[index],
                        &pimsmDebugGrpAddr[index],
                        &sgRptNode );
  if(sgRptNode != ( pimsmSGRptNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGRptNode created: %p\n", sgRptNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGRptNode create failed\n" );
    return;
  }
  memset( &sgRptEvntData, 0, sizeof( pimsmDnStrmPerIntfSGRptEventInfo_t ) );
  sgRptEvntData.rtrIfNum = index;

  /*pimsmDebugSGRptEntryShow(pimsmCb, &sgRptNode->pimsmSGRptEntry);*/
  PIMSM_DEBUG_PRINTF( "Max States:%d Events %d \n",
                      PIMSM_UPSTRM_S_G_RPT_SM_STATE_MAX,
                      PIMSM_UPSTRM_S_G_RPT_SM_EVENT_MAX );
  PIMSM_DEBUG_PRINTF( "State: %d and Event %d\n", state, event );                                                       
  sgRptEntry = &sgRptNode->pimsmSGRptEntry;
  sgRptEntry->pimsmSGRptUpstreamPruneState = state; 
  memset( &sgRptEvntData, 0, sizeof( pimsmDnStrmPerIntfSGEventInfo_t ) );
  sgRptEvntData.rtrIfNum = index;
  sgRptEvntData.eventType = event;
  sgRptEvntData.holdtime = 10; 
  if(pimsmUpStrmSGRptExecute( pimsmCb, sgRptNode, &sgRptEvntData ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "\nDebug:SGRptExecute Failed in %d state & %d event\n",
                        state,
                        event );
  }
}



/******************************************************************************
* @purpose  Execute the FSM events for different states and Events
*
* @param    family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugPerSGRegisterFSM ( L7_uchar8 family,
                                  L7_uint32 index,
                                  L7_uint32 state,
                                  L7_uint32 event )
{
  pimsmCB_t *                       pimsmCb;
  pimsmSGNode_t *                   sgNode;
  pimsmSGEntry_t *                  sgEntry;
  pimsmPerSGRegisterEventInfo_t          sgRegEvntData;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if((state >= PIMSM_REG_PER_S_G_SM_STATE_MAX) ||
     (event >= PIMSM_REG_PER_S_G_SM_EVENT_MAX))
  {
    PIMSM_DEBUG_PRINTF(
                      "Invalid Input!!Max State %d and and MAx Event %d\n", 
                      PIMSM_REG_PER_S_G_SM_STATE_MAX,
                      PIMSM_REG_PER_S_G_SM_EVENT_MAX);
    return;
  }
  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );
  if(sgNode != ( pimsmSGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGNode created: %p\n", sgNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGNode create failed\n" );
    return;
  }
  memset( &sgRegEvntData, 0, sizeof( pimsmPerSGRegisterEventInfo_t ) );
  /*pimsmDebugSGEntryShow(pimsmCb, &sgNode->pimsmSGEntry);*/
  PIMSM_DEBUG_PRINTF( "Max States:%d Events %d \n",
                      PIMSM_REG_PER_S_G_SM_STATE_MAX,
                      PIMSM_REG_PER_S_G_SM_EVENT_MAX );
  PIMSM_DEBUG_PRINTF( "State: %d and Event %d\n", state, event );                                                       
  sgEntry = &sgNode->pimsmSGEntry;
  sgEntry->pimsmSGDRRegisterState = state; 
  sgRegEvntData.eventType = event;
  if(pimsmPerSGRegisterExecute( pimsmCb, sgNode, &sgRegEvntData ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "\nDebug:SGRegisterExecute Failed in %d state & %d event\n",
                        state,
                        event );
  }
}




/******************************************************************************
* @purpose  Execute the FSM events for different states and Events
*
* @param    family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugPerSGAssertFSM ( L7_uchar8 family,
                                L7_uint32 index,
                                L7_uint32 state,
                                L7_uint32 event )
{
  pimsmCB_t *                       pimsmCb;
  pimsmSGNode_t *                   sgNode;
  pimsmSGIEntry_t *                  sgiEntry;
  pimsmPerIntfSGAssertEventInfo_t     sgAsrtEvntData;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if((state >= PIMSM_ASSERT_S_G_SM_STATE_MAX) ||
     (event >= PIMSM_ASSERT_S_G_SM_EVENT_MAX))
  {
    PIMSM_DEBUG_PRINTF(
                      "Invalid Input!!Max State %d and and Max Event %d\n", 
                      PIMSM_ASSERT_S_G_SM_STATE_MAX,
                      PIMSM_ASSERT_S_G_SM_EVENT_MAX);
    return;
  }
  pimsmSGNodeCreate( pimsmCb,
                     &pimsmDebugSrcAddr[index],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );
  if(sgNode != ( pimsmSGNode_t * )L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF( "SGNode created: %p\n", sgNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGNode create failed\n" );
    return;
  }
  if(pimsmSGIEntryCreate(pimsmCb, sgNode, index) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "SGI Entry create failed\n" );
    return;
  }
  PIMSM_DEBUG_PRINTF("Source Addr is \n");
  PIMSM_PRINT_ADDR( &pimsmDebugSrcAddr[index]);
  PIMSM_DEBUG_PRINTF("Source Addr is \n");
  PIMSM_PRINT_ADDR(&pimsmDebugGrpAddr[index]);
  memset( &sgAsrtEvntData, 0, sizeof( pimsmPerIntfSGAssertEventInfo_t ) );
  /*pimsmDebugSGEntryShow(pimsmCb, &sgNode->pimsmSGEntry);*/
  PIMSM_DEBUG_PRINTF( "Max States:%d Events %d \n",
                      PIMSM_ASSERT_S_G_SM_STATE_MAX,
                      PIMSM_ASSERT_S_G_SM_EVENT_MAX );
  PIMSM_DEBUG_PRINTF( "State: %d and Event %d\n", state, event );
  sgiEntry = sgNode->pimsmSGIEntry[index];
  sgiEntry->pimsmSGIAssertState = state; 
  sgAsrtEvntData.eventType = event;
  sgAsrtEvntData.rtrIfNum = index;
  inetCopy(&sgAsrtEvntData.winnerAddress, &pimsmDebugSrcAddr[index+1]);
  sgAsrtEvntData.winnerMetric = 60; /*Random Data , for testing*/
  sgAsrtEvntData.winnerMetricPref = 90; /*Random Data , for testing*/
  if(pimsmPerIntfSGAssertExecute( pimsmCb, sgNode, &sgAsrtEvntData ) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "\nDebug:SGAssertExecute Failed in %d state & %d event\n",
                        state, event );
  }
}

/******************************************************************************
* @purpose  Execute the FSM events for different states and Events
*
* @param    family            @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugPerStarGAssertFSM ( L7_uchar8 family,
                                   L7_uint32 index,
                                   L7_uint32 state,
                                   L7_uint32 event )
{
  pimsmCB_t *                       pimsmCb;
  pimsmStarGNode_t *                stargNode;
  pimsmStarGIEntry_t *               stargiEntry;
  pimsmPerIntfStarGAssertEventInfo_t     stargAsrtEvntData;

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if((state >= PIMSM_ASSERT_STAR_G_SM_STATE_MAX) ||
     (event >= PIMSM_ASSERT_STAR_G_SM_EVENT_MAX))
  {
    PIMSM_DEBUG_PRINTF(
                      "Invalid Input!!Max State %d and and MAx Event %d\n", 
                      PIMSM_ASSERT_STAR_G_SM_STATE_MAX,
                      PIMSM_ASSERT_STAR_G_SM_EVENT_MAX);
    return;
  }
  if(pimsmStarGNodeCreate( pimsmCb,
                           &pimsmDebugGrpAddr[index],
                           &stargNode ) == L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "StarGNode created: %p\n", stargNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "StarGNode create failed\n" );
    return;
  }
  PIMSM_DEBUG_PRINTF("StarGNode is  %p\n", stargNode);
  if(pimsmStarGIEntryCreate(pimsmCb, stargNode, index) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "(*,G,I) entry create failed\n");    
    return;
  }
  else
    PIMSM_DEBUG_PRINTF( "Created (*,G,I) entry successfully\n");    
  memset( &stargAsrtEvntData, 0, sizeof( pimsmPerIntfStarGAssertEventInfo_t ) );
  PIMSM_DEBUG_PRINTF( "Max States:%d Events %d \n",
                      PIMSM_ASSERT_STAR_G_SM_STATE_MAX,
                      PIMSM_ASSERT_STAR_G_SM_EVENT_MAX );
  PIMSM_DEBUG_PRINTF( "State: %d and Event %d\n", state, event );
  stargiEntry = stargNode->pimsmStarGIEntry[index];
  stargiEntry->pimsmStarGIAssertState = state; 
  stargAsrtEvntData.eventType = event;
  stargAsrtEvntData.rtrIfNum = index;
  inetCopy(&stargAsrtEvntData.winnerAddress, &pimsmDebugSrcAddr[index+1]);
  stargAsrtEvntData.winnerMetric = 60; /*Random Data , for testing*/
  stargAsrtEvntData.winnerMetricPref = 90; /*Random Data , for testing*/
  inetAddressZeroSet(pimsmCb->family,&stargAsrtEvntData.srcAddr);
  if(pimsmPerIntfStarGAssertExecute( pimsmCb, stargNode, &stargAsrtEvntData ) 
     != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "\nDebug:StarGRegisterExecute Failed in %d state & %d event\n",
                        state, event );
  }
}

/******************************************************************************
* @purpose  Execute the FSM events for different states and Events
*
* @param    family            @b{(input)}
*
* @returns  void
*
* @comments      
* The data rate is set to the threshold parameter
*       
* @end
******************************************************************************/
void pimsmDebugSPTThresholdSwitchOver(L7_uchar8 family, L7_uint32 index, 
                                      L7_uint32 threshold)
{
  pimsmCB_t *                       pimsmCb;
  pimsmStarGNode_t *                stargNode;
  pimsmSGNode_t    *                sgNode;
  pimsmStarGIEntry_t      *         stargiEntry;

  /*10.1.1.1 as Ixia Port / Source IP for Register Packets */
  L7_inet_addr_t      pimsmIxiaAddr[1] = { {L7_AF_INET,{{0x0A010101}}}};    

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  if(pimsmStarGNodeCreate( pimsmCb,
                           &pimsmDebugGrpAddr[index],
                           &stargNode ) == L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "StarGNode created: %p\n", stargNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "StarGNode create failed\n" );
    return;
  }
  if(pimsmStarGIEntryCreate(pimsmCb, stargNode, index) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "(*,G,I) entry create failed\n");    
    return;
  }
  else
    PIMSM_DEBUG_PRINTF( "Created (*,G,I) entry successfully\n");    

  stargiEntry = stargNode->pimsmStarGIEntry[index];

  stargiEntry->pimsmStarGILocalMembership = L7_TRUE;

  if(pimsmSGNodeCreate(pimsmCb, &pimsmIxiaAddr[0], &pimsmDebugGrpAddr[index],
                       &sgNode,L7_TRUE) == L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "SGNode created: %p\n", sgNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGNode create failed\n" );
    return;
  }

  if(pimsmStarGCacheAdd(pimsmCb, stargNode, &pimsmIxiaAddr[0]) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "Kernel cache create failed\n" );
    return;
  }
  pimsmCb->pimsmDataRateBytes = threshold /*(* 1000)*/;    
}


/******************************************************************************
* @purpose  Execute the FSM events for different states and Events
*
* @param    family            @b{(input)}
*
* @returns  void
*
* @comments      
* The data rate is set to the threshold parameter
*       
* @end
******************************************************************************/
void pimsmDebugRegisterThresholdSwitchOver(L7_uchar8 family, L7_uint32 index, 
                                           L7_uint32 threshold)
{
  pimsmCB_t *                       pimsmCb;
  pimsmSGNode_t    *                sgNode;

  /*10.1.1.1 as Ixia Port / Source IP for Register Packets */
  L7_inet_addr_t      pimsmIxiaAddr[1] = { {L7_AF_INET,{{0x0A010164}}}};    

  pimsmCb = pimsmMapProtocolCtrlBlockGet( family );
  pimsmCb->isPimsmEnabled = L7_TRUE;

  if(pimsmSGNodeCreate(pimsmCb, &pimsmIxiaAddr[0], &pimsmDebugGrpAddr[index],
                       &sgNode,L7_TRUE) == L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "SGNode created: %p\n", sgNode );
  }
  else
  {
    PIMSM_DEBUG_PRINTF( "SGNode create failed\n" );
    return;
  }

  pimsmCb->pimsmRegRateBytes = threshold /*(* 1000)*/;    
}

#ifdef PIMSM_NOTNEEDED
void pimsmDebugHelloSend( L7_uchar8 family, L7_uint32 rtrIfNum, L7_short16 holdtime)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;

  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }
  if(pimsmHelloSend(pimsmCb,rtrIfNum,holdtime) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF("pimsmHelloSend() failed");
    return;
  }
  PIMSM_DEBUG_PRINTF("pimsmHelloSend() success\r\n");
}
#endif
void pimsmDebugRegSend( L7_uchar8 family)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  L7_inet_addr_t * srcAddr;
  L7_inet_addr_t * grpAddr;
  L7_uint32 index = 0;
  L7_in6_addr_t     Ipv6Addr;

  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugGrpAddr[index] );


    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimsmDebugV6SrcStringToAddr( index, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugSrcAddr[index] );
  }
  srcAddr =&pimsmDebugSrcAddr[index];
  grpAddr = &pimsmDebugGrpAddr[index];

  if(pimsmNullRegisterSend(pimsmCb,srcAddr,grpAddr) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF("pimsmNullRegisterSend() failed");
    return;
  }
  PIMSM_DEBUG_PRINTF("pimsmNullRegisterSend() success\r\n");
}

void pimsmDebugAssertSend( L7_uchar8 family, L7_uint32 rtrIfNum)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  L7_inet_addr_t * srcAddr;
  L7_inet_addr_t * grpAddr;
  L7_uint32 index = 0;
  L7_in6_addr_t     Ipv6Addr;

  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }
  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugGrpAddr[index] );


    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimsmDebugV6SrcStringToAddr( index, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugSrcAddr[index] );
  }
  srcAddr =&pimsmDebugSrcAddr[index];
  grpAddr = &pimsmDebugGrpAddr[index];

  if(pimsmAssertCancelSend(pimsmCb,srcAddr,grpAddr, rtrIfNum) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF("pimsmAssertCancelSend() failed");
    return;
  }
  PIMSM_DEBUG_PRINTF("pimsmAssertCancelSend() success\r\n");
}
void pimsmDebugJPSend( L7_uchar8 family, L7_uint32 rtrIfNum, 
                       L7_uint32 jpFlag, L7_short16 addrFlag,
                       L7_uint32 isStarStarFlag)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  pimsmSendJoinPruneData_t jpData;
  L7_uint32 index = 0;
  L7_in6_addr_t     Ipv6Addr;
  L7_uchar8 maskLen =0;
  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimsmDebugV6StringToAddr( index, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugGrpAddr[index] );


    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimsmDebugV6SrcStringToAddr( index, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugSrcAddr[index] );


    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimsmDebugV6RpStringToAddr( index, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugRpAddr[index] );
  }
  memset(&jpData, 0 , sizeof(pimsmSendJoinPruneData_t));
  jpData.rtrIfNum = rtrIfNum;

  pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, rtrIfNum,
                                     &jpData.holdtime);
  jpData.isStarStarRpFlag = isStarStarFlag;
  jpData.pNbrAddr = &pimsmDebugRpAddr[index];
  jpData.addrFlags = addrFlag;
  jpData.pSrcAddr =&pimsmDebugSrcAddr[index];
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen = maskLen;
  jpData.pGrpAddr = &pimsmDebugGrpAddr[index];
  pimGrpMaskLenGet(pimsmCb->family,&maskLen);
  jpData.grpAddrMaskLen = maskLen;
  jpData.joinOrPruneFlag = jpFlag;

  if(pimsmJoinPruneSend(pimsmCb,&jpData) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF("pimsmJoinPruneSend() failed");
    return;
  }
  PIMSM_DEBUG_PRINTF("pimsmJoinPruneSend() success\r\n");
}

void pimsmDebugRegStopSend( L7_uchar8 family, L7_uint32 rtrIfNum)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  L7_inet_addr_t * reg_dst;
  L7_inet_addr_t * inner_grp;
  L7_inet_addr_t * inner_src;
  L7_in6_addr_t     Ipv6Addr;

  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimsmDebugV6RpStringToAddr( 1, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugRpAddr[1] );
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimsmDebugV6SrcStringToAddr( 1, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugSrcAddr[1] );

    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimsmDebugV6SrcStringToAddr( 2, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugSrcAddr[2] );

    pimsmDebugV6StringToAddr( 2, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugGrpAddr[2] );

  }
  reg_dst = &pimsmDebugSrcAddr[1];

  inner_src =&pimsmDebugSrcAddr[2];
  inner_grp = &pimsmDebugGrpAddr[2];

  if(pimsmRegisterStopSend(pimsmCb,rtrIfNum,reg_dst,inner_grp,inner_src)
     != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF("pimsmRegisterStopSend() failed");
    return;
  }
  PIMSM_DEBUG_PRINTF("pimsmRegisterStopSend() success\r\n");
}

#ifdef PIMSM_NOTNEEDED
void pimsmDebugJPBundleSend( L7_uchar8 family, L7_uint32 rtrIfNum, 
                             L7_uint32 jpFlag, L7_short16 addrFlag,
                             L7_uint32 addStarStarFlag, L7_uint32 changeGrp)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  pimsmSendJoinPruneData_t jpData;
  L7_uint32 index = 0, newIndex = 3;
  pimsmNeighborEntry_t *pimNbr;  
  pimsmInterfaceEntry_t  * intfEntry = L7_NULLPTR;
  L7_uchar8 maskLen =0;  
  PIMSM_DEBUG_PRINTF("Entry");
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }
  memset(&jpData, 0 , sizeof(pimsmSendJoinPruneData_t));
  jpData.rtrIfNum = rtrIfNum;

  pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, rtrIfNum,
                                     &jpData.holdtime);
  jpData.isStarStarRpFlag = L7_FALSE;
  jpData.pNbrAddr = &pimsmDebugRpAddr[index];
  jpData.addrFlags = addrFlag;
  jpData.pSrcAddr =&pimsmDebugSrcAddr[index];
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen = maskLen;
  jpData.pGrpAddr = &pimsmDebugGrpAddr[index];
  pimGrpMaskLenGet(pimsmCb->family,&maskLen);
  jpData.grpAddrMaskLen = maskLen;
  jpData.joinOrPruneFlag = jpFlag;

  if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &intfEntry )!=L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_TRACE_ERROR,"IntfEntry is NULLPTR\n");  
    return ;
  }

  pimsmNeighborGetFirst(pimsmCb, intfEntry, &pimNbr);
  if(pimNbr == (pimsmNeighborEntry_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_TRACE_ERROR,"pimNbr is NULLPTR\n");  
    return ;
  }
  /*pimsmDebugNeighborShow(pimsmCb,pimNbr);*/

  if(pimsmJoinPruneBundleSend(pimsmCb,pimNbr,&jpData) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF("pimsmJoinPruneBundleSend() failed");
    return;
  }

  if(changeGrp)
  {
    index = newIndex;
  }
  else
  {
    index +=1;
  }
  memset(&jpData, 0 , sizeof(pimsmSendJoinPruneData_t));
  jpData.rtrIfNum = rtrIfNum;

  pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, rtrIfNum,
                                     &jpData.holdtime);
  jpData.isStarStarRpFlag = L7_FALSE;
  jpData.pNbrAddr = &pimsmDebugRpAddr[index];
  jpData.addrFlags = addrFlag;
  jpData.pSrcAddr =&pimsmDebugSrcAddr[index];
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen = maskLen;
  jpData.pGrpAddr = &pimsmDebugGrpAddr[index];
  pimGrpMaskLenGet(pimsmCb->family,&maskLen);
  jpData.grpAddrMaskLen = maskLen;
  jpData.joinOrPruneFlag = jpFlag;

  if(pimsmJoinPruneBundleSend(pimsmCb,pimNbr, &jpData) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF("pimsmJoinPruneBundleSend() failed");
    return;
  }

  if(addStarStarFlag)
  {
    index +=1;
    memset(&jpData, 0 , sizeof(pimsmSendJoinPruneData_t));
    jpData.rtrIfNum = rtrIfNum;

    pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, rtrIfNum,
                                       &jpData.holdtime);
    jpData.isStarStarRpFlag = L7_TRUE;
    jpData.pNbrAddr = &pimsmDebugRpAddr[index];
    jpData.addrFlags = addrFlag;
    jpData.pSrcAddr =&pimsmDebugSrcAddr[index];
    pimSrcMaskLenGet(pimsmCb->family,&maskLen);
    jpData.srcAddrMaskLen = maskLen;
    jpData.pGrpAddr = &pimsmDebugGrpAddr[index];
    pimGrpMaskLenGet(pimsmCb->family,&maskLen);
    jpData.grpAddrMaskLen = maskLen;
    jpData.joinOrPruneFlag = jpFlag;

    if(pimsmJoinPruneBundleSend(pimsmCb,pimNbr, &jpData) != L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF("pimsmJoinPruneBundleSend() failed");
      return;
    }
  }
  PIMSM_DEBUG_PRINTF("pimsmJoinPruneBundleSend() success\r\n");
}

void pimsmDebugMRouteEntryNextGet( L7_uchar8 family)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  L7_inet_addr_t grpAddr;
  L7_inet_addr_t srcAddr;
  L7_inet_addr_t srcAddrMask;
  L7_RC_t rc;
  L7_uint32 rtrIfNum = 0;

  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  inetAddressZeroSet(pimsmCb ->family, &grpAddr);
  inetAddressZeroSet(pimsmCb ->family, &srcAddr);
  inetAddressZeroSet(pimsmCb ->family, &srcAddrMask);

  rc = pimsmMRouteEntryNextGet((void*)pimsmCb,&grpAddr,&srcAddr,&srcAddrMask);
  while(rc == L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF("\n\ngrpAddr = ");
    PIMSM_PRINT_ADDR(&grpAddr);
    PIMSM_DEBUG_PRINTF("srcAddr = ");
    PIMSM_PRINT_ADDR(&srcAddr);
    PIMSM_DEBUG_PRINTF("srcAddrMask = ");
    PIMSM_PRINT_ADDR(&srcAddrMask);
    PIMSM_DEBUG_PRINTF("OIF List = ");
    rtrIfNum = 0;
    rc = pimsmMRouteEntryOIFNextGet((void*)pimsmCb,&grpAddr,&srcAddr,&rtrIfNum);
    while(rc == L7_SUCCESS)
    {
      PIMSM_DEBUG_PRINTF(" %d ", rtrIfNum);
      rc = pimsmMRouteEntryOIFNextGet((void*)pimsmCb,&grpAddr,&srcAddr,&rtrIfNum);
    }
    rc = pimsmMRouteEntryNextGet((void*)pimsmCb,&grpAddr,&srcAddr,&srcAddrMask);
  }
}
#endif
void pimsmDebugStarGEntryNextGet( L7_uchar8 family)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  L7_inet_addr_t grpAddr;
  L7_RC_t rc;
   
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

 inetAddressZeroSet(pimsmCb ->family, &grpAddr);
 rc = pimsmStarGEntryNextGet((void*)pimsmCb,&grpAddr);
  while (rc == L7_SUCCESS) {
     PIMSM_DEBUG_PRINTF("\ngrpAddr = ");
     PIMSM_PRINT_ADDR(&grpAddr);
     rc = pimsmStarGEntryNextGet((void*)pimsmCb,&grpAddr);
  }
}
void pimsmDebugSGEntryNextGet( L7_uchar8 family)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  L7_inet_addr_t grpAddr, srcAddr;
  L7_RC_t rc;
   
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

 inetAddressZeroSet(pimsmCb ->family, &grpAddr);
 inetAddressZeroSet(pimsmCb ->family, &srcAddr);
 rc = pimsmSGEntryNextGet((void*)pimsmCb,&grpAddr, &srcAddr);
  while (rc == L7_SUCCESS) {
     PIMSM_DEBUG_PRINTF("\ngrpAddr = ");
     PIMSM_PRINT_ADDR(&grpAddr);
     PIMSM_DEBUG_PRINTF("srcAddr = ");
     PIMSM_PRINT_ADDR(&srcAddr);
     rc = pimsmSGEntryNextGet((void*)pimsmCb,&grpAddr, &srcAddr);
  }
}
void pimsmDebugSGRptEntryNextGet( L7_uchar8 family)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  L7_inet_addr_t grpAddr, srcAddr;
  L7_RC_t rc;
   
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

 inetAddressZeroSet(pimsmCb ->family, &grpAddr);
 inetAddressZeroSet(pimsmCb ->family, &srcAddr);
 rc = pimsmSGRptEntryNextGet((void*)pimsmCb,&grpAddr, &srcAddr);
  while (rc == L7_SUCCESS) {
     PIMSM_DEBUG_PRINTF("\ngrpAddr = ");
     PIMSM_PRINT_ADDR(&grpAddr);
     PIMSM_DEBUG_PRINTF("srcAddr = ");
     PIMSM_PRINT_ADDR(&srcAddr);
     rc = pimsmSGRptEntryNextGet((void*)pimsmCb,&grpAddr, &srcAddr);
  }
}

void pimsmDebugStarGIEntryNextGet( L7_uchar8 family)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  L7_inet_addr_t grpAddr;
  L7_RC_t rc;
  L7_uint32 rtrIfNum;

  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  inetAddressZeroSet(pimsmCb ->family, &grpAddr);
  rtrIfNum = 1;
  rc = pimsmStarGIEntryNextGet((void*)pimsmCb,&grpAddr,&rtrIfNum);
  while(rc == L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF("\ngrpAddr = ");
    PIMSM_PRINT_ADDR(&grpAddr);
    PIMSM_DEBUG_PRINTF("oif = %d ", rtrIfNum);
    rc = pimsmStarGIEntryNextGet((void*)pimsmCb,&grpAddr,&rtrIfNum);
  }
}

void pimsmDebugSGRptIEntryNextGet( L7_uchar8 family)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  L7_inet_addr_t grpAddr, srcAddr;
  L7_RC_t rc;
  L7_uint32 rtrIfNum;

  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  inetAddressZeroSet(pimsmCb ->family, &grpAddr);
  inetAddressZeroSet(pimsmCb ->family, &srcAddr);
  rtrIfNum = 1;
  rc = pimsmSGRptIEntryNextGet((void*)pimsmCb,&grpAddr, &srcAddr, &rtrIfNum);
  while(rc == L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF("\ngrpAddr = ");
    PIMSM_PRINT_ADDR(&grpAddr);
    PIMSM_DEBUG_PRINTF("oif = %d ", rtrIfNum);
    rc = pimsmSGRptIEntryNextGet((void*)pimsmCb,&grpAddr, &srcAddr, &rtrIfNum);
  }
}


void pimsmDebugSGIEntryNextGet( L7_uchar8 family)
{
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  L7_inet_addr_t grpAddr, srcAddr;
  L7_RC_t rc;
  L7_uint32 rtrIfNum;

  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCb == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }

  inetAddressZeroSet(pimsmCb ->family, &grpAddr);
  inetAddressZeroSet(pimsmCb ->family, &srcAddr); 
  rtrIfNum = 1;
  rc = pimsmSGIEntryNextGet((void*)pimsmCb,&grpAddr, &srcAddr, &rtrIfNum);
  while(rc == L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF("\ngrpAddr = ");
    PIMSM_PRINT_ADDR(&grpAddr);
    PIMSM_DEBUG_PRINTF("oif = %d ", rtrIfNum);
    rc = pimsmSGIEntryNextGet((void*)pimsmCb,&grpAddr, &srcAddr, &rtrIfNum);
  }
}
/******************************************************************************
* @purpose  To add RP-configuration
*
* @param        family            @b{(input)} family
* @param        intIfNum          @b{(input)} interface on which RP is configured
* @param        grpIndex          @b{(input)} index of debug group list
* @param        grpMask           @b{(input)} grp Mask
* @param        rpIndex           @b{(input)} index of debug rp list (shd be 0 if intIfNum address                                              is to be taken
* @param        origin            @b{(input)}  = 1 for static and = 7 for static overide 
* @param        priority          @b{(input)} priority of RP 
* @param        holdtime          @b{(input)} holdtime of RP 
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

L7_RC_t pimsmDebugRpConfigAdd(L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 grpIndex, L7_uint32 grpMask,
                              L7_uint32 rpIndex, L7_uint32 origin, L7_uint32 rpPriority,L7_uint32 rpHoldtime)
{
  pimsmCandRpConfigInfo_t pimsmRpInfo;
  L7_in6_addr_t         Ipv6Addr;
  pimsmCB_t *   pimsmCB;
  pimsmCB = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet(family);
#if PIMSM_NOTNEEDED
  L7_uint32 rtrIfNum = 0;
  L7_inet_addr_t intfAddr;
  if(ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
    PIMSM_DEBUG_PRINTF("\n unable to get rtrIFNum for given interface \n");

  if(mcastIpMapIpAddressGet(L7_AF_INET, rtrIfNum, &intfAddr) != L7_SUCCESS)
    PIMSM_DEBUG_PRINTF("\n unable to get ip address for given interface \n");
#endif
  memset(&pimsmRpInfo, 0,sizeof(pimsmRpInfo));

/*  inetCopy(&(pimsmRpInfo.rpAddr), &intfAddr);*/

  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimsmDebugV6StringToAddr( grpIndex, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugGrpAddr[grpIndex] );

    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimsmDebugV6RpStringToAddr(rpIndex, &Ipv6Addr);
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugRpAddr[rpIndex] );


  }
  inetCopy (&(pimsmRpInfo.pimsmRpAddr), &pimsmDebugRpAddr[rpIndex]);
  inetCopy(&(pimsmRpInfo.pimsmGrpAddr),&pimsmDebugGrpAddr[grpIndex]);   
  pimsmRpInfo.pimsmGrpMaskLen = grpMask;
  pimsmRpInfo.pimsmRpPriority = rpPriority;
  pimsmRpInfo.pimsmRpHashMaskLen = 10;
  if(grpIndex == 1 || grpIndex == 0)
    pimsmRpInfo.pimsmRpHoldTime = 65;

  if(rpHoldtime)
    pimsmRpInfo.pimsmRpHoldTime = rpHoldtime;
  pimsmRpInfo.pimsmOrigin= origin;

  if(pimsmRpGrpMappingAdd(pimsmCB, &pimsmRpInfo) != L7_SUCCESS)
    PIMSM_DEBUG_PRINTF("\n failed adding rp set into RP Dbase \n");
  else
    PIMSM_DEBUG_PRINTF("\n added RP set \n");

  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  to delete the RP configuration
*
* @param        family            @b{(input)} family
* @param        intIfNum          @b{(input)} inetrface on which RP is configured
* @param        grpIndex          @b{(input)} index of debug group list
* @param        grpMask           @b{(input)} grp Mask
* @param        rpIndex           @b{(input)} index of debug rp list (shd be 0 if intIfNum address                                              is to be taken
* @param        origin            @b{(input)}  = 1 for static and = 7 for static overide 
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/

L7_RC_t pimsmDebugRpConfigDelete(L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 grpIndex,
                                 L7_uint32 grpMask, L7_uint32 rpIndex, L7_uint32 origin)
{
  L7_uint32 rtrIfNum = 0;
  L7_inet_addr_t intfAddr;
  pimsmCandRpConfigInfo_t pimsmRpInfo;
  L7_in6_addr_t         Ipv6Addr;
  pimsmCB_t *   pimsmCB;
  pimsmCB = ( pimsmCB_t * )pimsmMapProtocolCtrlBlockGet(family);

  if(ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
    PIMSM_DEBUG_PRINTF("\n unable to get rtrIFNum for given interface \n");

  if(mcastIpMapIpAddressGet(L7_AF_INET, rtrIfNum, &intfAddr) != L7_SUCCESS)
    PIMSM_DEBUG_PRINTF("\n unable to get ip address for given interface \n");

  memset(&pimsmRpInfo, 0,sizeof(pimsmRpInfo));

/*  inetCopy(&(pimsmRpInfo.rpAddr), &intfAddr); */
  if(family == L7_AF_INET6)
  {
    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );

    pimsmDebugV6StringToAddr( grpIndex, &Ipv6Addr );        
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugGrpAddr[grpIndex] );

    memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
    pimsmDebugV6RpStringToAddr(rpIndex, &Ipv6Addr);
    inetAddressSet( L7_AF_INET6,
                    ( void * )&Ipv6Addr,
                    &pimsmDebugRpAddr[rpIndex] );


  }
  inetCopy (&(pimsmRpInfo.pimsmRpAddr), &pimsmDebugRpAddr[rpIndex]);
  inetCopy(&(pimsmRpInfo.pimsmGrpAddr),&pimsmDebugGrpAddr[grpIndex]);   
  pimsmRpInfo.pimsmOrigin= origin;
  pimsmRpInfo.pimsmGrpMaskLen = grpMask;

  if(pimsmRpGrpMappingDelete(pimsmCB, &pimsmRpInfo) != L7_SUCCESS)
    PIMSM_DEBUG_PRINTF("\n failed deleting rp set from RP Dbase \n");
  else
    PIMSM_DEBUG_PRINTF("\n deleted RP set \n");

  return L7_SUCCESS;
}
/******************************************************************************
* @purpose  Display the RP-addr for given group-addr (rp-hashing)
*
* @param        grpAddr      @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugGroupToRpMappingShow (L7_uint32 family, L7_uint32 grpIndex)
{
  L7_inet_addr_t rpAddr;
  L7_uchar8 strDump[40];
  pimsmCB_t * pimsmCb;
  pimsmCb = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmRpAddressGet(pimsmCb, &pimsmDebugGrpAddr[grpIndex], &rpAddr) == L7_SUCCESS)
  {
    inetAddrHtop(&(rpAddr),strDump);       
    PIMSM_DEBUG_PRINTF("\n RP -Address : %s",strDump);      
  }
  else
    PIMSM_DEBUG_PRINTF ("\n unable to get RP address \n");

}
#ifdef PIMSM_NOTNEEDED
/******************************************************************************
* @purpose  Display the RP-addr for given group-addr (rp-hashing)
*
* @param        grpAddr      @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimsmDebugIntfEvnt(L7_uchar8 family,L7_uint32 index, L7_uint32 mode)
{
  pimsmCB_t *pimsmCB;
  pimsmIntfData_t intfData;
  pimsmSGNode_t   * sgNode;
  pimsmSGIEntry_t * sgiEntry;

  /*10.1.1.1 as Ixia Port / Source IP for Register Packets */
  L7_inet_addr_t      pimsmIxiaAddr[1] = { {L7_AF_INET,{{0x0A010101}}}};    

  pimsmCB = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCB == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }
  pimsmSGNodeCreate( pimsmCB,
                     &pimsmIxiaAddr[0],
                     &pimsmDebugGrpAddr[index],
                     &sgNode,L7_TRUE );

  if(pimsmSGIEntryCreate(pimsmCB, sgNode, index) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF( "SGI Entry create failed\n" );
    return;
  }
  sgiEntry = sgNode->pimsmSGIEntry[index];
  sgiEntry->pimsmSGIJoinPruneState =
  PIMSM_DNSTRM_S_G_SM_STATE_JOIN;

  memset(&intfData, 0, sizeof(pimsmIntfData_t));
  intfData.rtrIfNum = index;
  if(mode)
    intfData.isPimsmEnabled = L7_TRUE;
  else
    intfData.isPimsmEnabled = L7_FALSE;

  if(pimsmIntfStatusProcess(pimsmCB, &intfData) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF("pimsmMapCompEventChangeNotify failed");
    return;
  }
  PIMSM_DEBUG_PRINTF("pimsmMapCompEventChangeNotify success");
}


#endif
void pimsmDebugIgmpV2JoinLeaveProcess(L7_uchar8 family,L7_uint32 index,L7_uint32 rtrIfNum,L7_uint32 mode)
{
  mgmdMrpEventInfo_t  mgmdGrpInfo;
  pimsmCB_t *pimsmCB;

  pimsmCB = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCB == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }
  memset(&mgmdGrpInfo,0,sizeof(mgmdMrpEventInfo_t));
  mgmdGrpInfo.rtrIfNum = rtrIfNum;
  if(mode == 1)
    mgmdGrpInfo.mode= MGMD_FILTER_MODE_EXCLUDE;
  else if(mode ==2)
    mgmdGrpInfo.mode= MGMD_FILTER_MODE_INCLUDE;


  inetCopy(&mgmdGrpInfo.group,&pimsmDebugGrpAddr[index]);
  mgmdGrpInfo.numSrcs = 0;

  if(pimsmEventProcess(family, MCAST_EVENT_MGMD_GROUP_UPDATE,
                       &mgmdGrpInfo) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF("pimsmEventProcess failed");
    return;
  }

  PIMSM_DEBUG_PRINTF("pimsmEventProcess success");

}


void pimsmDebugIgmpV3JoinLeaveProcess(L7_uchar8 family,L7_uint32 index,L7_uint32 rtrIfNum,L7_uint32 action)
{
  mgmdMrpEventInfo_t  mgmdGrpInfo;
  pimsmCB_t *pimsmCB;

  pimsmCB = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCB == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }
  memset(&mgmdGrpInfo,0,sizeof(mgmdMrpEventInfo_t));
  mgmdGrpInfo.rtrIfNum = rtrIfNum;
  mgmdGrpInfo.mode= MGMD_FILTER_MODE_INCLUDE;

  inetCopy(&mgmdGrpInfo.group,&pimsmDebugGrpAddr[index]);
  inetCopy(&mgmdGrpInfo.sourceList[0]->sourceAddr,&pimsmDebugSrcAddr[index]);
  if(action == 1)
  {
    mgmdGrpInfo.sourceList[0]->action = MGMD_SOURCE_ADD;
  }
  else if(action ==2)
  {
    mgmdGrpInfo.sourceList[0]->action = MGMD_SOURCE_DELETE;
  }
  mgmdGrpInfo.sourceList[0]->filterMode=  MGMD_FILTER_MODE_INCLUDE;

  mgmdGrpInfo.numSrcs = 1;

  if(pimsmEventProcess(family, MCAST_EVENT_MGMD_GROUP_UPDATE,
                       &mgmdGrpInfo) != L7_SUCCESS)
  {

    PIMSM_DEBUG_PRINTF("pimsmEventProcess failed");
    return;
  }

  PIMSM_DEBUG_PRINTF("pimsmEventProcess success");

}


void pimsmDebugV6IgmpV2JoinLeaveProcess(L7_uchar8 family,L7_uint32 index,L7_uint32 rtrIfNum,L7_uint32 mode)
{
  mgmdMrpEventInfo_t  mgmdGrpInfo;
  pimsmCB_t *pimsmCB;
  L7_in6_addr_t Ipv6Addr;

  pimsmCB = (pimsmCB_t *)pimsmMapProtocolCtrlBlockGet(family);

  if(pimsmCB == (pimsmCB_t *)L7_NULLPTR)
  {
    PIMSM_DEBUG_PRINTF("Invalid CB (NULLPTR) \n");
    return;
  }
  memset(&mgmdGrpInfo,0,sizeof(mgmdMrpEventInfo_t));
  mgmdGrpInfo.rtrIfNum = rtrIfNum;
  if(mode == 1)
    mgmdGrpInfo.mode= MGMD_FILTER_MODE_EXCLUDE;
  else if(mode ==2)
    mgmdGrpInfo.mode= MGMD_FILTER_MODE_INCLUDE;


  memset( &Ipv6Addr, 0, sizeof( L7_in6_addr_t ) );
  pimsmDebugV6StringToAddr( index, &Ipv6Addr );
  inetAddressSet( L7_AF_INET6,
                  ( void * )&Ipv6Addr,
                  &pimsmDebugGrpAddr[index] );
  inetCopy(&mgmdGrpInfo.group,&pimsmDebugGrpAddr[index]);
  mgmdGrpInfo.numSrcs = 0;

  if(pimsmEventProcess(family, MCAST_EVENT_MGMD_GROUP_UPDATE,
                            &mgmdGrpInfo) != L7_SUCCESS)
  {
    PIMSM_DEBUG_PRINTF("pimsmEventProcess failed");
    return;
  }

  PIMSM_DEBUG_PRINTF("pimsmEventProcess success");
}

/*
 * EOF 
 */


