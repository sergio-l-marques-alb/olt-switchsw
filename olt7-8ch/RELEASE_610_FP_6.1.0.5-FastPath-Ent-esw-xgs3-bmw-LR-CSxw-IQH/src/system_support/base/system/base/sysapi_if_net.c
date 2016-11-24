/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  sysapi_if_net.c
*
* @purpose   Provide system-wide support routines specific to network and mbuf routines
*
* @component sysapi
*
* @create    12/11/2000
*
* @author    paulq 
*
* @end
*
*********************************************************************/

#include <string.h>                /* for memcpy() etc... */
#include "l7_common.h"
#include "registry.h"
#include "l3_addrdefs.h"
#include "osapi.h"
#include "log.h"
#include "sysapi.h"
#include "simapi.h"
#include "dtlapi.h"
#include "rto_api.h"
#include "ipv6_commdefs.h"
#include "l7_rto6_api.h"
#include "bspapi.h"
#include "l7utils_api.h"
#include "l7_ip_api.h"
#include "rlim_api.h"

/*************************************
* Mbuf Queue declarations
*************************************/
extern L7_uint32 *pMbufQTop;     /* top of queue */
extern L7_uint32 *pMbufQBot;     /* bottom of queue */
extern L7_uint32 *MbufQHead;
extern L7_uint32 *MbufQTail;
extern L7_uint32 MbufsFree;
extern L7_uint32 MbufsRxUsed;
extern L7_uint32 MbufsMaxFree;
extern void      *MbufSema;
extern void      *pMbufPool;

#ifdef MBUF_HISTORY
extern  mbuf_history_t *mbufHistory;
extern  L7_uint32 historyIndex;
extern void mbufHistoryIndexInc(void);
#endif

extern int inet_addr(char *);

extern simRouteStorage_t  * servPortRoutingProtRoute;
extern simRouteStorage_t  * netPortRoutingProtRoute;

SYSAPI_IP_STATS_t sysapiIPStats;

typedef struct {
  L7_uint32  alloc_rx_norm_alloc_attempts;
  L7_uint32  alloc_rx_high_alloc_attempts;
  L7_uint32  alloc_tx_alloc_attempts;
  L7_uint32  alloc_rx_mid0_alloc_attempts;
  L7_uint32  alloc_rx_mid1_alloc_attempts;
  L7_uint32  alloc_rx_mid2_alloc_attempts;

  L7_uint32  alloc_rx_norm_failures;
  L7_uint32  alloc_rx_high_failures;
  L7_uint32  alloc_tx_failures;
  L7_uint32  alloc_rx_mid0_failures;
  L7_uint32  alloc_rx_mid1_failures;
  L7_uint32  alloc_rx_mid2_failures;
} mbuf_stats_t;

static mbuf_stats_t mbuf_stats;
static L7_uchar8 partialNetMask[8] = {0x00,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe};

L7_uchar8 *mbufLocNames[] = {"Free", "Alloc", "IPv4 FWD Q", "IPv4 FWD", "ARP RX", "ARP PEND Q", 
  "ARP RES CB", "PDU TX", "PDU RX", "VRRP RX", "DHCP Relay RX", "UDP Relay TX", "CPWIO RX", "CPWIO ARP RX", "CPWIO TX", "DAI"};

#if defined (_L7_OS_LINUX_) || defined(_L7_OS_ECOS_)
#include <pthread.h>
static pthread_mutex_t sysapiMbufMutex = PTHREAD_MUTEX_INITIALIZER;
#define SYSAPI_MBUF_LOCK() pthread_mutex_lock(&sysapiMbufMutex)
#define SYSAPI_MBUF_UNLOCK() pthread_mutex_unlock(&sysapiMbufMutex)
#else
static L7_uint32 lockKey = 0;
#define SYSAPI_MBUF_LOCK() { lockKey = osapiDisableInts(); }
#define SYSAPI_MBUF_UNLOCK() osapiEnableInts(lockKey);
#endif

#ifdef MBUF_HISTORY

extern L7_uint32 mbuf_history_size;

/**************************************************************************
* @purpose  Allocate memory to track mbuf history
*
* @param    historySize - Number of alloc and free actions to track (e.g., 
*                         size of circular buffer)
*
* @returns  void
*
* @comments
*
* @end
*************************************************************************/
void mbufHistoryInit(L7_uint32 historySize)
{
  osapiSemaTake(MbufSema, L7_WAIT_FOREVER);
  if (mbufHistory)
  {
    printf("\nMBUF history already initialized with size of %u entries.",
           mbuf_history_size);

    osapiSemaGive(MbufSema);
    return;
  }

  if (historySize)
  {
    mbuf_history_size = historySize;
  }
  else
  {
    mbuf_history_size = 1000;
  }
  historyIndex = 0;
  mbufHistory = osapiMalloc(L7_SIM_COMPONENT_ID, mbuf_history_size * sizeof(mbuf_history_t));
  if (mbufHistory)
  {
    memset(mbufHistory, 0, mbuf_history_size * sizeof(mbuf_history_t));
  }
  else
    printf("\nFailed to allocate mbuf history");

  osapiSemaGive(MbufSema);
}

/**************************************************************************
* @purpose  Deallocate memory to track mbuf history
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*************************************************************************/
void mbufHistoryDelete(void)
{
  osapiSemaTake(MbufSema, L7_WAIT_FOREVER);

  if (mbufHistory)
  {
    osapiFree(L7_SIM_COMPONENT_ID, mbufHistory);
    mbufHistory = NULL;
    mbuf_history_size = 0;
    historyIndex = 0;
  }
  osapiSemaGive(MbufSema);
}

/**************************************************************************
* @purpose  Clear mbuf history
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*************************************************************************/
void mbufHistoryClear(void)
{
  osapiSemaTake(MbufSema, L7_WAIT_FOREVER);

  if (mbufHistory)
  {
    memset(mbufHistory, 0, mbuf_history_size * sizeof(mbuf_history_t));
  }
  osapiSemaGive(MbufSema);
}

/**************************************************************************
* @purpose  Dumps mbuf history
*
* @param    p - If p is non-zero, only dump records for mbuf at this location.
*
* @returns  void
*
* @comments
*
* @end
*************************************************************************/
void mbufHistoryDump(void *p)
{
  L7_uint32 index;
  L7_uint32 i;    /* number of records already displayed */

  osapiSemaTake(MbufSema, L7_WAIT_FOREVER);

  if (!mbufHistory)
  {
    printf("\nNo mbuf history. First call mbufHistoryInit(size).");
    osapiSemaGive(MbufSema);
    return; 
  }

  /* Start at oldest record */
  index = historyIndex;

  printf("\nTime (msec)              mbuf      Action             File          Line");
  for (i = 0; i < mbuf_history_size; i++)
  {
    if (mbufHistory[index].mbuf_ptr)
    {
      if (!p || (p == mbufHistory[index].mbuf_ptr))
      {
        printf("\n%10u         %#10x     %6s       %16s      %u",        
               mbufHistory[index].timestamp, 
               (L7_uint32) mbufHistory[index].mbuf_ptr,
               (mbufHistory[index].mbufAction == MBUF_ALLOC) ? "Alloc" : "Free",
               mbufHistory[index].alloc_file,
               mbufHistory[index].alloc_line);
      }
    }
    index++;
    if (index == mbuf_history_size)
      index = 0;
  }

  osapiSemaGive(MbufSema);
}
#endif

/**************************************************************************
* @purpose  Dumps debug info for allocated buffers
*
* @param    none.
*
* @returns  void
*
* @comments
*
* @end
*************************************************************************/
void sysapiMbufDump(L7_int32 show_bufs)
{
  SYSAPI_NET_MBUF_HEADER_t *netMbufHandle;
  L7_uint32                 i;
  L7_uint32                 mbufSize;

  if ( sysapiRegistryGet( L7_MBUF_SIZE,  U32_ENTRY, (void *) &mbufSize) != L7_SUCCESS )
  {
    LOG_MSG("sysapiRegistryGet failed");
  }

  printf("mbufSize           %d (0x%x)\n",mbufSize, mbufSize);
  printf("Current Time       0x%x\n",osapiUpTimeRaw());
  printf("MbufsFree          %d\n",MbufsFree);
  printf("MbufsRxUsed        %d\n",MbufsRxUsed);
  printf("Total Rx Norm Alloc Attempts   %d\n", mbuf_stats.alloc_rx_norm_alloc_attempts);
  printf("Total Rx Mid2 Alloc Attempts   %d\n", mbuf_stats.alloc_rx_mid2_alloc_attempts);
  printf("Total Rx Mid1 Alloc Attempts   %d\n", mbuf_stats.alloc_rx_mid1_alloc_attempts);
  printf("Total Rx Mid0 Alloc Attempts   %d\n", mbuf_stats.alloc_rx_mid0_alloc_attempts);
  printf("Total Rx High Alloc Attempts   %d\n", mbuf_stats.alloc_rx_high_alloc_attempts);
  printf("Total Tx Alloc Attempts        %d\n", mbuf_stats.alloc_tx_alloc_attempts);

  printf("Total Rx Norm Alloc Failures   %d\n", mbuf_stats.alloc_rx_norm_failures);
  printf("Total Rx Mid2 Alloc Failures   %d\n", mbuf_stats.alloc_rx_mid2_failures);
  printf("Total Rx Mid1 Alloc Failures   %d\n", mbuf_stats.alloc_rx_mid1_failures);
  printf("Total Rx Mid0 Alloc Failures   %d\n", mbuf_stats.alloc_rx_mid0_failures);
  printf("Total Rx High Alloc Failures   %d\n", mbuf_stats.alloc_rx_high_failures);
  printf("Total Tx Alloc Failures        %d\n", mbuf_stats.alloc_tx_failures);

  if (show_bufs != 0)
  {
    for (i = 0; i < MbufsMaxFree; i++)
    {
      netMbufHandle = (SYSAPI_NET_MBUF_HEADER_t *)((L7_char8 *)pMbufPool + (i * mbufSize));

      if (netMbufHandle->bufStart != L7_NULL)
      {
        printf("Mbuf 0x%x at %s alloc by task 0x%x in %s:%u at %u secs\n",
               (L7_uint32)netMbufHandle,
               mbufLocNames[netMbufHandle->mbufLoc],
               netMbufHandle->taskId,
               netMbufHandle->last_file,
               netMbufHandle->last_line,
               netMbufHandle->timeStamp);

        /* Use option 2 with caution as buffers could be getting freed by other tasks */
        if (show_bufs == 2)
        {
          L7_uint32 row, column;
          printf("===================\n");
          for (row = 0; row < 4; row++)
          {
            printf("%04x ", row * 16);
            for (column = 0; column < 16; column++)
            {
              printf("%2.2x ", netMbufHandle->bufStart[row*16 + column]);
            }
            printf("\n");
          }
          printf("===================\n");
        }
      }
    }
  }
}

/**************************************************************************
* @purpose  Retrieve an mbuf to the caller
*
* @param    none.
*
* @returns  A ptr to an mbuf
* @returns  L7_NULL if none are available
*
* @comments    Use the Mutex semaphore to inhibit global variable corruption
*
* @end
*************************************************************************/
L7_uint32 *sysapiMbufGet( void )
{
  L7_uint32 buffer;

  SYSAPI_MBUF_LOCK();

  if ( MbufsFree != 0 )
  {
    buffer = *MbufQHead;
    if ( MbufQHead >= pMbufQBot )
      MbufQHead = pMbufQTop;    /* wrap the Q head ptr */
    else
      MbufQHead++;        /* move the Q head ptr */
    MbufsFree--;         /* keep track...       */

    ((SYSAPI_NET_MBUF_HEADER_t *)buffer)->in_use = L7_TRUE;
  }
  else
  {
    buffer = L7_NULL;
  }

  SYSAPI_MBUF_UNLOCK();

  return( ( L7_uint32 * )buffer );
}

/**************************************************************************
* @purpose  Return an mbuf to the mbuf pool
*
* @param    *mbuf ptr to the mbuf to return 
*
* @returns  none.
*
* @comments    Use the Mutex semaphore to inhibit global variable corruption
*
* @end
*************************************************************************/
void sysapiMbufFree(  L7_uint32 *mbuf )
{
  SYSAPI_MBUF_LOCK();

  *MbufQTail = ( L7_uint32 )mbuf;
  if ( MbufQTail >= pMbufQBot )
    MbufQTail = pMbufQTop;    /* wrap the Q tail ptr */
  else
    MbufQTail++;        /* move the Q tail ptr */

  MbufsFree++;         /* keep track...       */

  /* If we have extra buffers then somebody must have freed a buffer twice.
  ** This is a fatal error.
  */
  if (MbufsFree > MbufsMaxFree)
  {
    LOG_ERROR ((unsigned long) mbuf);
  }

  SYSAPI_MBUF_UNLOCK();

  return;
}


/**************************************************************************
*
* @purpose  Retrieve a network mbuf to the caller (and track the caller)
*
* @param    none.
*
* @returns  A ptr to a network mbuf handle
* @returns  0 if none are available
*
* @notes    Delegates to sysapiNetMbufGet
*
* @end
*
*************************************************************************/
L7_netBufHandle sysapiNetMbufGetTrack(L7_uchar8 *file, L7_uint32 line)
{
  SYSAPI_NET_MBUF_HEADER_t *netMbufHandle = 0;

  /* get the MBUF */
  netMbufHandle = (SYSAPI_NET_MBUF_HEADER_t *)sysapiNetMbufGet();

  /* store tracking information */
  if(netMbufHandle)
  {
    l7utilsFilenameStrip((L7_char8 **)&file);
    osapiStrncpySafe(netMbufHandle->last_file, file, sizeof(netMbufHandle->last_file));
    netMbufHandle->last_line = line;
    netMbufHandle->mbufLoc = MBUF_LOC_ALLOC;

#ifdef MBUF_HISTORY
    osapiSemaTake(MbufSema, L7_WAIT_FOREVER);
    if (mbufHistory)
    {
      mbufHistory[historyIndex].mbuf_ptr = (void*) netMbufHandle;
      mbufHistory[historyIndex].mbufAction = MBUF_ALLOC;
      mbufHistory[historyIndex].timestamp = osapiTimeMillisecondsGet();
      osapiStrncpySafe(mbufHistory[historyIndex].alloc_file, file, MBUF_HISTORY_FILENAME_LEN);
      mbufHistory[historyIndex].alloc_line = line;
      mbufHistoryIndexInc();    /* increment with wrap */
    }
    osapiSemaGive(MbufSema);
#endif
  }

  return((L7_uint32)netMbufHandle);
}

/**************************************************************************
* @purpose  Retrieve a network mbuf to the caller
*
* @param    none.
*
* @returns  A ptr to a network mbuf handle
* @returns  0 if none are available
*
* @comments    
*
* @end
*************************************************************************/
L7_netBufHandle sysapiNetMbufGet( void )
{
  SYSAPI_NET_MBUF_HEADER_t *netMbufHandle;

  mbuf_stats.alloc_tx_alloc_attempts++;

  netMbufHandle = (SYSAPI_NET_MBUF_HEADER_t *)sysapiMbufGet();
  if (netMbufHandle != L7_NULL)
  {
    netMbufHandle->bufStart  = (L7_uchar8 *)netMbufHandle + sizeof(SYSAPI_NET_MBUF_HEADER_t) + 
		                        NET_MBUF_START_OFFSET;

    netMbufHandle->bufStart = (L7_char8 *)SYSAPI_BUF_ALIGN(netMbufHandle->bufStart,L7_MBUF_ALIGN_BOUND);

    netMbufHandle->bufLength = 0;
    netMbufHandle->osBuffer  = L7_NULL;
    netMbufHandle->taskId    = osapiTaskIdSelf();
    netMbufHandle->timeStamp = osapiUpTimeRaw();
    netMbufHandle->rxBuffer  = L7_FALSE;

    /* wipe out tracking information */
    netMbufHandle->last_file[0] = 0;
    netMbufHandle->last_line = 0;
    netMbufHandle->mbufLoc = MBUF_LOC_ALLOC;
  } 
  else
  {
    mbuf_stats.alloc_tx_failures++;
  }

  return((L7_uint32)netMbufHandle);
}

/**************************************************************************
* @purpose  Retrieve a network mbuf to the caller, for use on receiving packets
*
* @param    L7_MBUF_RX_PRIORITY priority
*
* @returns  A ptr to a network mbuf handle
* @returns  0 if none are available
*
* @comments We limit the number of receive buffers such that we guarantee 
*           there will be L7_MBUF_RESERVED_TX_BUFFERS buffers available for
*           transmit purposes at all times. Also, we guarantee that there 
*           will be L7_MBUF_RESERVED_RX_HI_PRIO_BUFFERS buffers available
*           for receiving high priority traffic.
*
* @end
*************************************************************************/
L7_netBufHandle sysapiRxNetMbufGet( L7_MBUF_RX_PRIORITY priority, 
	                            	L7_MBUF_ALIGNMENT  alignType)
{
  SYSAPI_NET_MBUF_HEADER_t *mbufPtr;
  L7_uint32  rx_high_level;
  L7_uint32  rx_mid0_level;
  L7_uint32  rx_mid1_level;
  L7_uint32  rx_mid2_level;
  L7_uint32  rx_norm_level;
  L7_BOOL    rx_failed;

  rx_high_level = L7_MBUF_RESERVED_TX_BUFFERS + MbufsRxUsed;
  rx_mid0_level = L7_MBUF_RESERVED_RX_HI_PRIO_BUFFERS + rx_high_level; 
  rx_mid1_level = L7_MBUF_RESERVED_RX_MID0_PRIO_BUFFERS + rx_mid0_level; 
  rx_mid2_level = L7_MBUF_RESERVED_RX_MID1_PRIO_BUFFERS + rx_mid1_level;
  rx_norm_level = L7_MBUF_RESERVED_RX_MID2_PRIO_BUFFERS + rx_mid2_level;

  if (MbufSema == L7_NULL)
  {
    return L7_NULL;
  }


  osapiSemaTake(MbufSema, L7_WAIT_FOREVER);

  rx_failed = L7_FALSE;
  switch (priority)
  {
  case L7_MBUF_RX_PRIORITY_HIGH:
    mbuf_stats.alloc_rx_high_alloc_attempts++;
    if (rx_high_level >= MbufsMaxFree)
    {
      rx_failed = L7_TRUE;
      mbuf_stats.alloc_rx_high_failures++;
    }
    break;

  case L7_MBUF_RX_PRIORITY_MID0:
    mbuf_stats.alloc_rx_mid0_alloc_attempts++;
    if (rx_mid0_level >= MbufsMaxFree)
    {
      rx_failed = L7_TRUE;
      mbuf_stats.alloc_rx_mid0_failures++;
    }
    break;

  case L7_MBUF_RX_PRIORITY_MID1:
    mbuf_stats.alloc_rx_mid1_alloc_attempts++;
    if (rx_mid1_level >= MbufsMaxFree)
    {
      rx_failed = L7_TRUE;
      mbuf_stats.alloc_rx_mid1_failures++;
    }
    break;

  case L7_MBUF_RX_PRIORITY_MID2:
    mbuf_stats.alloc_rx_mid2_alloc_attempts++;
    if (rx_mid2_level >= MbufsMaxFree)
    {
      rx_failed = L7_TRUE;
      mbuf_stats.alloc_rx_mid2_failures++;
    }
    break;

  case L7_MBUF_RX_PRIORITY_NORMAL:
    mbuf_stats.alloc_rx_norm_alloc_attempts++;
    if (rx_norm_level >= MbufsMaxFree)
    {
      rx_failed = L7_TRUE;
      mbuf_stats.alloc_rx_norm_failures++;
    }
    break;

  default:
    LOG_ERROR (priority);
    break;
  }

  if (rx_failed == L7_TRUE)
  {
      osapiSemaGive(MbufSema);
      return L7_NULL;
  }

  mbufPtr = (SYSAPI_NET_MBUF_HEADER_t *)sysapiMbufGet();
  if ( mbufPtr != L7_NULL)
  {
     mbufPtr->bufStart  = (L7_uchar8 *)mbufPtr + sizeof(SYSAPI_NET_MBUF_HEADER_t) + NET_MBUF_START_OFFSET;
	 mbufPtr->bufStart = (L7_uchar8 *)SYSAPI_BUF_ALIGN(mbufPtr->bufStart,L7_MBUF_ALIGN_BOUND);

	 if(alignType ==  L7_MBUF_IP_CORRECTION)
	 {
  	   mbufPtr->bufStart  += L7_MBUF_IP_CORRECTION;    /* Compensate for ipheader offset */
	 }

     mbufPtr->bufLength = 0;
     mbufPtr->osBuffer  = L7_NULL;
     mbufPtr->taskId    = osapiTaskIdSelf();
     mbufPtr->timeStamp = osapiUpTimeRaw();
     mbufPtr->mbufLoc = MBUF_LOC_ALLOC;
     mbufPtr->rxBuffer = L7_TRUE;
     MbufsRxUsed++;

#ifdef MBUF_HISTORY
     if (mbufHistory)
     {
       mbufHistory[historyIndex].mbuf_ptr = (void*) mbufPtr;
       mbufHistory[historyIndex].mbufAction = MBUF_ALLOC;
       mbufHistory[historyIndex].timestamp = osapiTimeMillisecondsGet();
       osapiStrncpySafe(mbufHistory[historyIndex].alloc_file, "none", MBUF_HISTORY_FILENAME_LEN);
       mbufHistory[historyIndex].alloc_line = 0;
       mbufHistoryIndexInc();    /* increment with wrap */
     }
#endif
  }

  osapiSemaGive(MbufSema);

  return (L7_netBufHandle)mbufPtr;
}

/**************************************************************************
* @purpose  Retrieve an aligned network mbuf to the caller
*
* @param    align   @b{(input)}  Alignment indicator, for IP or frame
*
* @returns  A ptr to a network mbuf handle
* @returns  0 if none are available
*
* @note     All mbufs are 4 byte aligned 
*
* @end
*************************************************************************/
L7_netBufHandle sysapiNetMbufAlignGet(L7_uchar8 *file, L7_uint32 line,
   	                                  L7_MBUF_ALIGNMENT alignType)
{    
  SYSAPI_NET_MBUF_HEADER_t *netMbufHandle = 0;

  /* get the MBUF */
  netMbufHandle = (SYSAPI_NET_MBUF_HEADER_t *)sysapiNetMbufGet();

  /* store tracking information */
  if(netMbufHandle)
  {
    if (L7_MBUF_IP_ALIGNED == alignType)
    {
      /* Compensate for ipheader offset */
      netMbufHandle->bufStart += L7_MBUF_IP_CORRECTION; 
    }
    l7utilsFilenameStrip((L7_char8 **)&file);
    osapiStrncpySafe(netMbufHandle->last_file, file, sizeof(netMbufHandle->last_file));
    netMbufHandle->last_line = line;
    netMbufHandle->mbufLoc = MBUF_LOC_ALLOC;

#ifdef MBUF_HISTORY
    osapiSemaTake(MbufSema, L7_WAIT_FOREVER);
    if (mbufHistory)
    {
      mbufHistory[historyIndex].mbuf_ptr = (void*) netMbufHandle;
      mbufHistory[historyIndex].mbufAction = MBUF_ALLOC;
      mbufHistory[historyIndex].timestamp = osapiTimeMillisecondsGet();
      osapiStrncpySafe(mbufHistory[historyIndex].alloc_file, file, MBUF_HISTORY_FILENAME_LEN);
      mbufHistory[historyIndex].alloc_line = line;
      mbufHistoryIndexInc();    /* increment with wrap */
    }
    osapiSemaGive(MbufSema);
#endif
  }

  return((L7_uint32)netMbufHandle);
}

/**************************************************************************
* @purpose  Free a network mbuf
*
* @param    Ptr to network mbuf handle
*
* @returns  void
*
* @comments    
*
* @end
*************************************************************************/
void sysapiNetMbufFree( L7_netBufHandle netMbufHandle )
{
  if (netMbufHandle != L7_NULL)
  {
    if (((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->osBuffer != L7_NULL)
    {
      osapiNetMbufFree ((void *)((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->osBuffer);
    }
    ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufStart  = L7_NULL;
    ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufLength = 0;
    ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->osBuffer  = L7_NULL;
    ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->taskId    = 0;
    ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->timeStamp = 0;
    ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->in_use    = L7_FALSE;
    ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->mbufLoc   = MBUF_LOC_FREE;

    if (((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->rxBuffer == L7_TRUE)
    {
      ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->rxBuffer = L7_FALSE;

      osapiSemaTake(MbufSema, L7_WAIT_FOREVER);
      MbufsRxUsed--;
      osapiSemaGive(MbufSema);
    }

    sysapiMbufFree ((L7_uint32 *)netMbufHandle);
  }
}

/**************************************************************************
* @purpose  Free a network mbuf with debug information.
*
* @param    Ptr to network mbuf handle
*
* @returns  void
*
* @comments    
*
* @end
*************************************************************************/
void sysapiNetMbufFreeTrack( L7_netBufHandle netMbufHandle, L7_uchar8 *file, L7_uint32 line)
{
  SYSAPI_NET_MBUF_HEADER_t  *header;

  header = (SYSAPI_NET_MBUF_HEADER_t *) netMbufHandle;

  if (header->in_use == L7_FALSE)
  {
    LOG_ERROR (netMbufHandle);
  }

  l7utilsFilenameStrip((L7_char8 **)&file);
  osapiStrncpySafe(header->last_file, file, sizeof(header->last_file));
  header->last_line = line;
  header->mbufLoc = MBUF_LOC_FREE;

#ifdef MBUF_HISTORY
  osapiSemaTake(MbufSema, L7_WAIT_FOREVER);
  if (mbufHistory)
  {
    mbufHistory[historyIndex].mbuf_ptr = (void*) netMbufHandle;
    mbufHistory[historyIndex].mbufAction = MBUF_FREE;
    mbufHistory[historyIndex].timestamp = osapiTimeMillisecondsGet();
    osapiStrncpySafe(mbufHistory[historyIndex].alloc_file, file, MBUF_HISTORY_FILENAME_LEN);
    mbufHistory[historyIndex].alloc_line = line;
    mbufHistoryIndexInc();    /* increment with wrap */
  }
  osapiSemaGive(MbufSema);
#endif

  sysapiNetMbufFree (netMbufHandle);

}

/**************************************************************************
* @purpose  Retrieve start of data portion of a network mbuf
*
* @param    A ptr to a network mbuf handle
*
* @returns  A ptr to start of data portion of a network mbuf
* @returns  0 if none are available
*
* @comments    
*
* @end
*************************************************************************/
L7_uchar8 *sysapiNetMbufGetDataStart( L7_netBufHandle netMbufHandle )
{
  if (netMbufHandle != L7_NULL)
  {
    return(((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufStart);
  } else
  {
    LOG_ERROR (0);
  }

  return((L7_uchar8 *)0xFFFFFFFF);
}

/**************************************************************************
* @purpose  Set start of data portion of a network mbuf
*
* @param    A ptr to a network mbuf handle
* @param    A ptr to start of data portion
*
* @returns  void
*
* @comments    
*
* @end
*************************************************************************/
void sysapiNetMbufSetDataStart( L7_netBufHandle netMbufHandle, L7_uchar8 *dataStart )
{
  if (netMbufHandle != L7_NULL)
  {
    ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufStart = dataStart;
  }
}

/**************************************************************************
* @purpose  Retrieve Application Specific Variable of network mbuf
*
* @param    A ptr to a network mbuf handle
*
* @returns  Application Specific Variable
*
* @comments    
*
* @end
*************************************************************************/
L7_uint32 sysapiNetMbufGetApplSpecVar( L7_netBufHandle netMbufHandle )
{
  if (netMbufHandle != L7_NULL)
  {
    return(((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->applSpecVar);
  }

  return((L7_uint32)netMbufHandle);
}

/**************************************************************************
* @purpose  Set Application Specific Variable of network mbuf
*
* @param    A ptr to a network mbuf handle
* @param    Application Specific Variable
*
* @returns  void
*
* @comments    
*
* @end
*************************************************************************/
void sysapiNetMbufSetApplSpecVar( L7_netBufHandle netMbufHandle, L7_uint32 value )
{
  if (netMbufHandle != L7_NULL)
  {
    ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->applSpecVar = value;
  }
}
/**************************************************************************
* @purpose  Retrieve length of network mbuf
*
* @param    A ptr to a network mbuf handle
*
* @returns  Length of data in network mbuf
*
* @comments    
*
* @end
*************************************************************************/
L7_uint32 sysapiNetMbufGetDataLength( L7_netBufHandle netMbufHandle )
{
  if (netMbufHandle != L7_NULL)
  {
    return(((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufLength);
  }

  return((L7_uint32)netMbufHandle);
}

/**************************************************************************
* @purpose  Set length of network mbuf
*
* @param    A ptr to a network mbuf handle
* @param    Size of data in network mbuf
*
* @returns  void
*
* @comments    
*
* @end
*************************************************************************/
void sysapiNetMbufSetDataLength( L7_netBufHandle netMbufHandle, L7_uint32 size )
{
  if (netMbufHandle != L7_NULL)
  {
    ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufLength = size;
  }
}

/**************************************************************************
* @purpose  Retrieve the receive reason code(s) of network mbuf
*
* @param    A ptr to a network mbuf handle
*
* @returns  Reason code(s)
*
* @comments Valid only for RX mbufs and not TX mbufs.
*
* @end
*************************************************************************/
L7_uint32 sysapiNetMbufGetRxReasonCode(L7_netBufHandle netMbufHandle)
{
  if (netMbufHandle != L7_NULL)
  {
    return(((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->rxCode);
  }

  return((L7_uint32)netMbufHandle);
}

/**************************************************************************
* @purpose  Set the reason code(s) of RX network mbuf
*
* @param    A ptr to a network mbuf handle
* @param    Reason code(s) 
*
* @returns  void
*
* @comments Valid only for RX mbufs and not TX mbufs. Invoked by Driver when
*           a packet is received (before passing the packet to application).
*
* @end
*************************************************************************/
void sysapiNetMbufSetRxReasonCode( L7_netBufHandle netMbufHandle, 
                                   L7_uint32 rxCode )
{
  if (netMbufHandle != L7_NULL)
  {
    ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->rxCode = rxCode;
  }
}

/**************************************************************************
* @purpose  Get next buffer from network mbuf
*
* @param    A ptr to a network mbuf handle
* @param    A ptr to a buffer handle
* @param    Size of data in network mbuf
*
* @returns  void
*
* @comments    For Device Driver use Only
*
* @end
*************************************************************************/
void sysapiNetMbufGetNextBuffer( L7_netBufHandle netMbufHandle, L7_netBlockHandle *blockHandle, L7_uchar8 **bufData, L7_uint32 *bufSize )
{
  if (netMbufHandle != L7_NULL)
  {
    if (*blockHandle == L7_NULL)
    {
      if (((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufLength != 0)
      {
        *bufSize = ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufLength;
        *bufData = ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufStart;
        *blockHandle = (L7_netBlockHandle)((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufStart;
      }
      else if (((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->osBuffer != L7_NULL)
      {
        osapiNetMbufGetNextBuffer((void *)((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->osBuffer, bufData, bufSize); 
        *blockHandle = (L7_netBlockHandle)((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->osBuffer;
      }
    }
    else
    {
      if (*blockHandle == (L7_netBlockHandle)((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufStart)
      {
        *blockHandle = (L7_netBlockHandle)((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->osBuffer;
      }
      else
        *blockHandle = (L7_netBlockHandle)osapiNetMbufGetNextMbuf((void *)*blockHandle);

      if (*blockHandle != L7_NULL)
      {
        osapiNetMbufGetNextBuffer((void *)*blockHandle, bufData, bufSize); 
      }
    }
  }
  else
  {
    bufData = L7_NULL;
  }
}
/**************************************************************************
* @purpose  Get the totals frame size of a network mbuf
*
* @param    A ptr to an network mbuf handle
*
* @returns  void
*
* @comments    For Device Driver use Only
*
* @end
*************************************************************************/
L7_uint32 sysapiNetMbufGetFrameLength( L7_netBufHandle netMbufHandle )
{
  L7_uint32 frameLength = 0;

  if (netMbufHandle != L7_NULL)
  {
    frameLength = ((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->bufLength;

    if (((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->osBuffer != L7_NULL)
    {
      frameLength += osapiNetMbufGetFrameLength((void *)((SYSAPI_NET_MBUF_HEADER_t *)netMbufHandle)->osBuffer); 
    }
  }

  return( frameLength );
}

/**************************************************************************
* @purpose  Configure service port ip
*
* @param    ipAddr      @b{(input)}   32bit ip address
* @param    ipNetMask   @b{(input)}   32bit ip net mask
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t sysapiConfigServicePortIp (L7_uint32 ipAddr, L7_uint32 ipNetMask)
{
  char buf[L7_SERVICE_PORT_MAX_LENGTH+1];
  L7_RC_t rc;
  L7_uint32 gateway, oldIpAddr, oldNetMask, oldGateway;

  oldIpAddr = 0;
  oldNetMask = 0;
  oldGateway = 0;
  
  if ((ipAddr != L7_NULL) && (ipNetMask != L7_NULL))
	{
    /* Call rto function to check if a exact route exists for the
     * new service port IP address. 
     */
	  oldIpAddr = servPortRoutingProtRoute->ipAddr;   /* save route previously masked */
	  oldNetMask = servPortRoutingProtRoute->netMask;
	  oldGateway = servPortRoutingProtRoute->gateway;
	  rc = rtoBestRouteLookupExact(ipAddr, ipNetMask, &gateway);
	  if (rc == L7_SUCCESS)
	  {
      /* remember RTO route being masked by service port address */
      servPortRoutingProtRoute->ipAddr = ipAddr;     
      servPortRoutingProtRoute->netMask = ipNetMask;
      servPortRoutingProtRoute->gateway = gateway;
		/* Delete masked route from OS */
		(void)osapiDeleteMRoute(servPortRoutingProtRoute->ipAddr, 
                            servPortRoutingProtRoute->netMask, gateway,
                            L7_INVALID_INTF);
	  }
	}

  sprintf(buf,"%s%d", bspapiServicePortNameGet(), 0);
  osapiArpFlush ((L7_uchar8 *)buf);
  rc = osapiNetIfConfig((L7_uchar8 *)buf,ipAddr,ipNetMask);

  if ((ipAddr == L7_NULL) && (ipNetMask == L7_NULL))
	{
	  if ((servPortRoutingProtRoute->gateway != 0) &&
        (servPortRoutingProtRoute->ipAddr != 0) &&
        (servPortRoutingProtRoute->netMask != 0))
	  {
      /* re-add RTO route previously masked by deleted service port address */
      (void)osapiAddMRoute(servPortRoutingProtRoute->ipAddr, servPortRoutingProtRoute->netMask,
                           servPortRoutingProtRoute->gateway,L7_INVALID_INTF,0);
      memset((L7_char8 *) servPortRoutingProtRoute, 0, sizeof (simRouteStorage_t));
	  }
	}
	else
	{
	  if ((oldIpAddr != 0) && (((oldIpAddr & oldNetMask) != (ipAddr & ipNetMask)) ||
		  (ipNetMask != oldNetMask)))
	  {
      /* re-add RTO route masked by previous service port address */
      (void)osapiAddMRoute(oldIpAddr, oldNetMask, oldGateway,L7_INVALID_INTF,0);
	  }
	}
  
  return rc;
} 

/**************************************************************************
* @purpose  Configure switch ip
*
* @param    ipAddr      @b{(input)}   32bit ip address
* @param    ipNetMask   @b{(input)}   32bit ip net mask
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t sysapiConfigSwitchIp (L7_uint32 ipAddr, L7_uint32 ipNetMask)
{
  char buf[L7_DTL_PORT_IF_LEN+1];
  L7_RC_t rc;
  L7_uint32 gateway, oldIpAddr, oldNetMask, oldGateway;

  oldIpAddr = 0;
  oldNetMask = 0;
  oldGateway = 0;
  
  if (ipAddr != L7_NULL)
	{
    /* Call rto function to check if a exact route exists for the
     * new network port IP address. 
     */
	  oldIpAddr = netPortRoutingProtRoute->ipAddr;
	  oldNetMask = netPortRoutingProtRoute->netMask;
	  oldGateway = netPortRoutingProtRoute->gateway;
	  rc = rtoBestRouteLookupExact(ipAddr, ipNetMask, &gateway);
	  if (rc == L7_SUCCESS)
	  {
      /* remember RTO route being masked by network port address */
      netPortRoutingProtRoute->ipAddr = ipAddr;
      netPortRoutingProtRoute->netMask = ipNetMask;
      netPortRoutingProtRoute->gateway = gateway;
      /* Delete this route from OS */
      (void)osapiDeleteMRoute(netPortRoutingProtRoute->ipAddr, 
                              netPortRoutingProtRoute->netMask, gateway,
                              L7_INVALID_INTF);
	  }
	}

  sprintf(buf,"%s%d", L7_DTL_PORT_IF, 0);
  osapiArpFlush ((L7_uchar8 *)buf);
  rc = osapiNetIfConfig((L7_uchar8 *)buf,ipAddr,ipNetMask);

  if (ipAddr == L7_NULL)
	{
	  if (netPortRoutingProtRoute->gateway != 0)
	  {
      /* Restore RTO route previously masked by network port address */
      (void)osapiAddMRoute(netPortRoutingProtRoute->ipAddr, netPortRoutingProtRoute->netMask,
                           netPortRoutingProtRoute->gateway,L7_INVALID_INTF,0);
      memset((L7_char8 *) netPortRoutingProtRoute, 0, sizeof (simRouteStorage_t));
	  }
	}
	else
	{
	  if ((oldIpAddr != 0) && (((oldIpAddr & oldNetMask) != (ipAddr & ipNetMask)) ||
		  (ipNetMask != oldNetMask)))
	  {
      /* Restore RTO route masked by previous network port address */
      (void)osapiAddMRoute(oldIpAddr, oldNetMask, oldGateway,L7_INVALID_INTF,0);
	  }
	}
  
  return rc;
}

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)

/**************************************************************************
* @purpose  Add service port IPv6 Prefix
*
* @param    ip6Addr        @b{(input)}   IPv6 prefix address
* @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t sysapiConfigServicePortIPV6PrefixAdd (L7_in6_addr_t *ip6Addr, 
                                              L7_uint32 ip6PrefixLen)
{
  char buf[32];
  L7_RC_t rc;
  
  sprintf(buf,"%s%d", bspapiServicePortNameGet(), 0);

  rc = osapiNetAddrV6Add(buf, ip6Addr, ip6PrefixLen);

  return rc;
} 

/**************************************************************************
* @purpose  Remove service port IPv6 Prefix
*
* @param    ip6Addr        @b{(input)}   IPv6 prefix address
* @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t sysapiConfigServicePortIPV6PrefixRemove (L7_in6_addr_t *ip6Addr, 
                                                 L7_uint32 ip6PrefixLen)
{
  char buf[32];
  L7_RC_t rc;
  
  sprintf(buf,"%s%d", bspapiServicePortNameGet(), 0);

  rc = osapiNetAddrV6Del(buf, ip6Addr, ip6PrefixLen);

  return rc;
} 

/**************************************************************************
* @purpose  Add network port IPv6 Prefix
*
* @param    ip6Addr        @b{(input)}   IPv6 prefix address
* @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t sysapiConfigSystemIPV6PrefixAdd (L7_in6_addr_t *ip6Addr, 
                                         L7_uint32 ip6PrefixLen)
{
  char buf[L7_DTL_PORT_IF_LEN+1];
  L7_RC_t rc;
  
  sprintf(buf,"%s%d", L7_DTL_PORT_IF, 0);

  rc = osapiNetAddrV6Add(buf, ip6Addr, ip6PrefixLen);

  return rc;
} 

/**************************************************************************
* @purpose  Remove network port IPv6 Prefix
*
* @param    ip6Addr        @b{(input)}   IPv6 prefix address
* @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t sysapiConfigSystemIPV6PrefixRemove (L7_in6_addr_t *ip6Addr,
                                            L7_uint32 ip6PrefixLen)
{
  char buf[L7_DTL_PORT_IF_LEN+1];
  L7_RC_t rc;
  
  sprintf(buf,"%s%d", L7_DTL_PORT_IF, 0);

  rc = osapiNetAddrV6Del(buf, ip6Addr, ip6PrefixLen);

  return rc;
} 

/**************************************************************************
* @purpose  Configure service/network port IPv6 gateway
*
* @param    ifName      @b{(input)}   corresponding gateway interface name
* @param    oldGateway  @b{(input)}   old IPv6 gateway (L7_NULL if didn't exist)
* @param    newGateway  @b{(input)}   new IPv6 gateway (L7_NULL if deleting)
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t sysapiConfigIPV6Gateway (L7_char8 *ifName, L7_in6_addr_t *oldGateway,
                                 L7_in6_addr_t *newGateway)
{
  char nameBuf[L7_DTL_PORT_IF_LEN+10];
  L7_in6_addr_t ip6Gateways[L7_RTR6_MAX_INTF_ADDRS];
  L7_uint32 acount, i;
  L7_in6_addr_t v6DefaultNetwork;
  L7_in6_addr_t tempGateway;
  L7_BOOL old_gateway_removed = L7_FALSE;
  L7_BOOL new_gateway_restored = L7_FALSE;
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  L7_route6Entry_t routeEntry;
#endif
#endif

  if (ifName == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset(&v6DefaultNetwork, 0, sizeof(v6DefaultNetwork));

  if (newGateway != L7_NULLPTR)
  {
    /* We are trying to set a new gateway */

    if (strcmp(ifName, L7_DTL_PORT_IF) == 0)
    {
      /* Trying to add gateway via network port.  If gateway is
       * defined for service port, then it takes precedence.
       */
      acount = L7_RTR6_MAX_INTF_ADDRS;
      if ((osapiIfIpv6DefaultRoutersGet(bspapiServicePortNameGet(),
                          ip6Gateways, &acount) == L7_SUCCESS) &&
          (acount != 0))
      {
        /* Gateway already defined for service port, which takes precedence */
        return L7_SUCCESS;
      }
    }
    else if (strcmp(ifName, bspapiServicePortNameGet()) == 0)
    {
      /* Trying to add gateway via service port.  If gateway is
       * defined for network port, then remove it.
       */
      memset(nameBuf, 0, sizeof(nameBuf));
      sprintf(nameBuf,"%s%d", L7_DTL_PORT_IF, 0);
      acount = L7_RTR6_MAX_INTF_ADDRS;
      if ((osapiIfIpv6DefaultRoutersGet(L7_DTL_PORT_IF,
                          ip6Gateways, &acount) == L7_SUCCESS) &&
          (acount != 0))
      {
        /* Remove the existing gateway(s) */
        for (i=0; i<acount; i++)
        {
          osapiDelMRoute6(&v6DefaultNetwork, 0, &ip6Gateways[i], 0, 0, nameBuf);
          old_gateway_removed = L7_TRUE;
        }
      }
    }
    
    memset(nameBuf, 0, sizeof(nameBuf));
    sprintf(nameBuf,"%s%d", ifName, 0);

    if (!old_gateway_removed)
    {
      /* Remove existing default route via given interface, if any */
      acount = L7_RTR6_MAX_INTF_ADDRS;
      if ((osapiIfIpv6DefaultRoutersGet(ifName,
                            ip6Gateways, &acount) == L7_SUCCESS) &&
          (acount != 0))
      {
        /* Remove the existing gateway(s) */
        for (i=0; i<acount; i++)
        {
          osapiDelMRoute6(&v6DefaultNetwork, 0, &ip6Gateways[i], 0, 0, nameBuf);
        }
      }
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
      else
      {
        /* Check RTO to see if there is a default route defined.  If so, remove it */
        if (rto6BestRouteLookup(&v6DefaultNetwork, &routeEntry, L7_FALSE) == L7_SUCCESS)
        {
          osapiDelMRoute6(&routeEntry.ip6Addr, routeEntry.ip6PrefixLen,
                          &routeEntry.ecmpRoutes.equalCostPath[0].ip6Addr,
                          routeEntry.ecmpRoutes.equalCostPath[0].intIfNum,
                          routeEntry.flags, L7_NULL);
        }
      } 
#endif
#endif
    }

    /* Add the new gateway */
    if (osapiAddMRoute6(&v6DefaultNetwork, 0, newGateway, 0, 0, nameBuf)
        == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }
  else
  {
    /* We are trying to remove an existing gateway */

    /* First, attempt to remove the old gateway definition */
    memset(nameBuf, 0, sizeof(nameBuf));
    sprintf(nameBuf,"%s%d", ifName, 0);
    osapiDelMRoute6(&v6DefaultNetwork, 0, oldGateway, 0, 0, nameBuf);

    if (strcmp(ifName, L7_DTL_PORT_IF) == 0)
    {
      /* We are removing gateway via network port.  If gateway is
       * defined for service port, then we assume it is by precedence
       * in the stack already, thus we are done.
       */
      if ((simGetServPortIPV6Gateway(&tempGateway) == L7_SUCCESS) &&
          (simGetServPortIPV6AdminMode() == L7_ENABLE))
      {
        return L7_SUCCESS;
      }
    }
    else if (strcmp(ifName, bspapiServicePortNameGet()) == 0)
    {
      /* We are removing gateway via service port.  If gateway is
       * defined for network port, then restore it.
       */
      if ((simGetSystemIPV6Gateway(&tempGateway) == L7_SUCCESS) &&
          (simGetSystemIPV6AdminMode() == L7_ENABLE))
      {
        memset(nameBuf, 0, sizeof(nameBuf));
        sprintf(nameBuf,"%s%d", L7_DTL_PORT_IF, 0);
        if (osapiAddMRoute6(&v6DefaultNetwork, 0, &tempGateway, 0, 0, nameBuf)
            == L7_SUCCESS)
        {
          new_gateway_restored = L7_TRUE;
        }
        else
        {
          return L7_FAILURE;
        }
      }
    }

    if (!new_gateway_restored)
    {
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
      /* Check RTO to see if there is a default route defined.  If so, restore it */
      if (rto6BestRouteLookup(&v6DefaultNetwork, &routeEntry, L7_FALSE) == L7_SUCCESS)
      {
        if (osapiAddMRoute6(&routeEntry.ip6Addr, routeEntry.ip6PrefixLen,
                            &routeEntry.ecmpRoutes.equalCostPath[0].ip6Addr,
                            routeEntry.ecmpRoutes.equalCostPath[0].intIfNum,
                            routeEntry.flags, L7_NULL) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
      } 
#endif
#endif
    }

    return L7_SUCCESS;
  }

  return L7_FAILURE;
} 


#endif /* L7_IPV6_PACKAGE || L7_IPV6_MGMT_PACKAGE */

/**************************************************************************
*
* @purpose  Ping an IP Address 
*
* @param    *ipHost  @b{(input)} An IP Address to ping
* @param    count    @b{(input)} Number of times to issue a ping request
*
* @returns  Number of packets echoed
*
* @notes    none
*
* @end
*
*************************************************************************/
L7_uint32 sysapiPing (L7_char8 *ipHost, L7_int32 count)
{

  if ( sysapiIPConfigValid() == L7_TRUE )
  {
    return( osapiPing ( ipHost, count ) );
  }
  return 0;
}



/**************************************************************************
*
* @purpose  Initialize the sysapi network support layer
*
* @param    void
*
* @returns  L7_SUCCESS, if successful
* @returns  L7_FAILURE, otherwise
*
* @notes    none
*
* @end
*
*************************************************************************/
L7_RC_t sysapiIfNetInit(void)
{

  bzero((L7_uchar8 *)&sysapiIPStats,sizeof(SYSAPI_IP_STATS_t));

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Use prefix length to isolate network number of a network address
*
* @param    prefixLen   @b{(input)}  prefix length (in bits)
* @param    bufSize     @b{(input)}  number of bytes in output buffer
* @param    *mask       @b{(inout)}  network address buffer location
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments Caller must supply a mask buffer at least as large as the 
*           bufSize parm indicates.
*
* @comments The nwtwork address is always assumed to be in network byte order 
*           for bufSize number of bytes.  A typical bufSize of 4 is used for 
*           IPv4 and 16 for IPv6 addresses.
*
* @end
*********************************************************************/
L7_RC_t sysapiPrefixNetMaskApply(L7_uint32 prefixLen, L7_uint32 bufSize, L7_uchar8 *mask)
{
  L7_uint32     i;

  /* check for a prefix that is too large */
  if (prefixLen > (bufSize * 8))
    return L7_FAILURE;
 
  /* build mask by octet to ensure endian-neutrality (i.e. network byte order) */
  for (i = 0; i < bufSize; i++)
  {
    if (prefixLen < 8)
    {
      /* use partial mask */
      mask[i] &= partialNetMask[prefixLen];
      prefixLen = 0;
    }
    else
    {
      /* leave mask[i] byte as is */
      prefixLen -= 8;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a network address mask from an arbitrary prefix length
*
* @param    prefixLen   @b{(input)}  prefix length (in bits)
* @param    bufSize     @b{(input)}  number of bytes in output buffer
* @param    *mask       @b{(output)} address mask output buffer location
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments Caller must supply a mask buffer at least as large as the 
*           bufSize parm indicates.
*
* @comments The mask is always built in network byte order for bufSize
*           number of bytes.  A typical bufSize of 4 is used for IPv4
*           and 16 for IPv6 mask creation.
*
* @end
*********************************************************************/
L7_RC_t sysapiPrefixLenToNetMask(L7_uint32 prefixLen, L7_uint32 bufSize, L7_uchar8 *mask)
{
  L7_uint32         i;

  /* check for a prefix that is too large */
  if (prefixLen > (bufSize * 8))
    return L7_FAILURE;
 
  /* build mask by octet to ensure endian-neutrality (i.e. network byte order) */
  for (i = 0; i < bufSize; i++)
  {
    if (prefixLen < 8)
    {
      /* use partial mask */
      mask[i] = partialNetMask[prefixLen];
      prefixLen = 0;
    }
    else
    {
      mask[i] = 0xff;
      prefixLen -= 8;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Convert Prefix Length to IPv4 Subnet Mask
*
* @param    prefixLen   @b{(input)}  prefix length (in bits)
* @param    mask        @b{(output)} Mask in the format of L7_IP_MASK_t
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments The mask is always built in host byte order
*
* @end
*
*********************************************************************/
L7_RC_t sysapiIPv4PrefixLenToAddrMask (L7_uchar8 prefixLen, L7_IP_MASK_t *mask)
{
  L7_IP_MASK_t ipMask = 0;
  L7_uint32 count = 0;

  if ((prefixLen > 32) || (mask == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  for (count = 0; count < prefixLen; count++)
  {
    ipMask = (ipMask << 0x01) | 0x01;
  }
  for (; count < 32; count++)
  {
    ipMask = ipMask << 0x01;
  }

  *mask = ipMask;
  return L7_SUCCESS;
} /* sysapiIPv4PrefixLenToAddrMask */

/*********************************************************************
*
* @purpose  Convert IPv4 Subnet Mask to Prefix Length
*
* @param    mask        @b{(input)} Mask in the format of L7_IP_MASK_t
* @param    prefixLen   @b{(output)} prefix length (in bits)
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments The mask is always built in host byte order
*
* @end
*
*********************************************************************/
L7_RC_t sysapiIPv4AddrMaskToPrefixLen (L7_IP_MASK_t mask, L7_uchar8 *prefixLen)
{
  L7_uint32 count = 0;
  L7_uchar8 prefix = 0;

  if (prefixLen == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  for (count = 0; count < 32; count++)
  {
    if (mask & 0x01)
      prefix++;

    mask = (mask >> 0x01);
  }

  *prefixLen = prefix;
  return L7_SUCCESS;
} /* sysapiIPv4AddrMaskToPrefixLen */

/*********************************************************************
*
* @purpose Select an IP address for an application component,
*          intended for components that need a common method to 
*          select an IP address to communicate with external devices.
*
* @param    pIpAddr        @b{(input/output)} current/new IP address
* @param    pIpMask        @b{{input/output}} current/new IP subnet mask
* @param    pRtrMode       @b{(input/output)} current/new routing mode
* @param    pIntIfNum      @b{(input/output)} current/new internal interface number
* @param    pLoopbackInUse @b{(input/output)} indicates if application current/new using loopback
* @param    pReason        @b{(output)} reason code to indicate why no IP address selected
*
* @returns L7_SUCCESS, IP address selected after NULL IP.
*          L7_FAILURE, IP address not selected, or IP address change after previous IP.
*
* @notes  this function needs to be simplified, there is a lot of repeated code, and 
*         currently it returns failure and a failure reason code on an IP address change,
*         its intended to be called again after the failure to get the new IP address.
*         it would be easier to understand if there was a ip change reason code.
*         additionally, this could be changed to have a routing bind instead of checking
*         for package selection.
*
* @end
*
*********************************************************************/
extern L7_RC_t sysapiApplicationIPAddressSelect(L7_uint32               *pIpAddr,
                                                L7_uint32               *pIpMask,
                                                L7_uint32               *pRtrMode,
                                                L7_uint32               *pIntIfNum,
                                                L7_BOOL                 *pLoopbackInUse,
                                                SYSAPI_APP_IP_REASON_t  *pReason)
{
  L7_IP_ADDR_t  ipAddr;
  L7_IP_MASK_t  mask;

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RLIM_PACKAGE
  L7_uint32     loopbackId;
#endif
  L7_uint32     intIfNum;
  L7_uint32     ifRtrMode;
  L7_uint32     rtrMode;
  L7_uint32     i;
  L7_uint32     ifState;
  L7_RC_t       rc;

  if ((pRtrMode == L7_NULLPTR) || (pIntIfNum == L7_NULLPTR) ||
      (pLoopbackInUse == L7_NULLPTR))
  {
    return L7_FAILURE;
  }
#endif

  if ((pReason == L7_NULLPTR) || (pIpAddr == L7_NULLPTR) || (pIpMask == L7_NULLPTR))
  {
    return L7_FAILURE;
  }
  *pReason = SYSAPI_APP_IP_REASON_NONE;


#ifdef L7_ROUTING_PACKAGE

  rtrMode = ipMapRtrAdminModeGet();

  if (rtrMode != *pRtrMode) 
  {
    *pRtrMode = rtrMode;
    *pIntIfNum = L7_NULL;
    *pIpAddr   = L7_NULL_IP_ADDR;
    *pIpMask   = L7_NULL_IP_MASK;
    *pReason = SYSAPI_APP_IP_REASON_GLOBAL_ROUTING_DISABLED;
    return L7_FAILURE;
  }

  *pRtrMode = rtrMode;

  if (rtrMode == L7_ENABLE)
  {
    if (*pIntIfNum != L7_NULL)
    {
      /* check if loop back interface is not in use */
  
      /* rlimLoopbackIdNextGet() starts fetching the next loopback from
       * given index, it is unlike wdm API NextGet if given a "0"
       * gets values from starting of the table. Due to this issue
       * the fix is to start from first loopback interface present
       * and running through the list.
       */
#ifdef L7_RLIM_PACKAGE
      if (*pLoopbackInUse == L7_FALSE)
      {
        if (rlimLoopbackIdFirstGet(&loopbackId) == L7_SUCCESS)
        {
          if ((rlimLoopbackIntIfNumGet(loopbackId, &intIfNum)== L7_SUCCESS) &&
              (ipMapIpIntfExists(intIfNum) == L7_TRUE)&& 
              (ipMapRtrIntfOperModeGet(intIfNum, &ifState) == L7_SUCCESS) &&
              (ifState == L7_ENABLE))
          {
            if ((ipMapRtrIntfIpAddressGet(intIfNum,&ipAddr,&mask)== L7_SUCCESS) && 
                (ipAddr != L7_NULL_IP_ADDR))
            {
              *pIntIfNum = L7_NULL;
              *pIpAddr = L7_NULL_IP_ADDR;
              *pIpMask = L7_NULL_IP_MASK;
              *pReason = SYSAPI_APP_IP_REASON_NO_LOOPBACK_INTERFACE;
              return L7_FAILURE;
            }
          }
          while (rlimLoopbackIdNextGet(loopbackId,&loopbackId) == L7_SUCCESS)
          {
            if ((rlimLoopbackIntIfNumGet(loopbackId,&intIfNum)== L7_SUCCESS) &&
                (ipMapIpIntfExists(intIfNum) == L7_TRUE)&&
                (ipMapRtrIntfOperModeGet(intIfNum, &ifState) == L7_SUCCESS) &&
                (ifState == L7_ENABLE))
            {
              if ((ipMapRtrIntfIpAddressGet(intIfNum,&ipAddr,&mask)== L7_SUCCESS) && 
                  (ipAddr != L7_NULL_IP_ADDR))
              {
                *pIntIfNum = L7_NULL;
                *pIpAddr = L7_NULL_IP_ADDR;
                *pIpMask = L7_NULL_IP_MASK;
                *pReason = SYSAPI_APP_IP_REASON_NO_LOOPBACK_INTERFACE;
                return L7_FAILURE;
              }
            }
          }/* end while loop */
        }/* if(rlimLoopbackIdFirstGet(&loopbackId) == L7_SUCCESS) */
      }/* if(*pLoopbackInUse == L7_FALSE) */
#endif
  
      if (ipMapIpIntfExists(*pIntIfNum) == L7_TRUE)
      {
        if ((ipMapRtrIntfIpAddressGet(*pIntIfNum, &ipAddr, &mask)== L7_SUCCESS) &&
            (ipAddr != L7_NULL_IP_ADDR) &&
            (ipMapRtrIntfOperModeGet(*pIntIfNum, &ifState) == L7_SUCCESS) &&
            (ifState == L7_ENABLE))
        {
          if ((*pIpAddr != L7_NULL_IP_ADDR) &&
              (*pIpAddr != ipAddr))
          {
            *pIpAddr = ipAddr;
            *pIpMask = mask;
            *pReason = SYSAPI_APP_IP_REASON_NO_IP_ADDRESS;  /* signal IP address change */
            return L7_FAILURE;
          }
          /* IP address changed after being NULL */
          *pIpAddr = ipAddr;
          *pIpMask = mask;
        }
        else
        {
          /* Check if routing got disabled due to port shutdown or port 
           * removal from routing interface. 
           */
          if (ipMapRtrIntfOperModeGet(*pIntIfNum, &ifRtrMode) != L7_SUCCESS)
          {
            *pIntIfNum = L7_NULL;
            *pIpAddr   = L7_NULL_IP_ADDR;
            *pIpMask   = L7_NULL_IP_MASK;
            *pReason   = SYSAPI_APP_IP_REASON_NO_LOOPBACK_INTERFACE;
            return L7_FAILURE;
          }
          if (ifRtrMode != L7_ENABLE)
          {
            rtrMode = ipMapRtrAdminModeGet();
            rc = ipMapRtrIntfModeGet(*pIntIfNum, &ifRtrMode);
  
            if((rtrMode != L7_ENABLE) && (ifRtrMode == L7_ENABLE))
            {
              *pReason = SYSAPI_APP_IP_REASON_GLOBAL_ROUTING_DISABLED;
            }
            else
            {
              *pReason = SYSAPI_APP_IP_REASON_NO_LOOPBACK_INTERFACE;
              *pIntIfNum = L7_NULL;
              *pIpAddr   = L7_NULL_IP_ADDR;
              *pIpMask   = L7_NULL_IP_MASK;
            }
            return L7_FAILURE;
          }
          else
          {
            *pIpAddr = L7_NULL_IP_ADDR;
            *pIpMask = L7_NULL_IP_MASK;
            *pReason = SYSAPI_APP_IP_REASON_NO_IP_ADDRESS;
            return L7_FAILURE;
          }
        }
      }
      else
      {
        /* Interface has been removed */
        *pIntIfNum = L7_NULL;
        *pIpAddr   = L7_NULL_IP_ADDR;
        *pIpMask   = L7_NULL_IP_MASK;
        *pReason = SYSAPI_APP_IP_REASON_NO_LOOPBACK_INTERFACE;
        return L7_FAILURE;
      }
    }
    else
    {
      /* rlimLoopbackIdNextGet() starts fetching the next loopback from
       * given index, it is unlike wdm API NextGet if given a "0"
       * gets values from starting of the table. Due to this issue
       * the fix is to start from first loopback interface present
       * and running through the list.
       */
#ifdef L7_RLIM_PACKAGE
      if(rlimLoopbackIdFirstGet(&loopbackId) == L7_SUCCESS)
      {
        if((rlimLoopbackIntIfNumGet(loopbackId,&intIfNum)== L7_SUCCESS) &&
           (ipMapIpIntfExists(intIfNum) == L7_TRUE) &&
           (ipMapRtrIntfOperModeGet(intIfNum, &ifState) == L7_SUCCESS) &&
           (ifState == L7_ENABLE))

        {
          if ((ipMapRtrIntfIpAddressGet(intIfNum,&ipAddr,&mask)== L7_SUCCESS) && 
              (ipAddr != L7_NULL_IP_ADDR))
          {
            *pIntIfNum = intIfNum;
            *pIpAddr = ipAddr;
            *pIpMask = mask;
            *pLoopbackInUse = L7_TRUE;
          }
        }
        /* if first loopback interface is not valid to use */
        if(*pIntIfNum == L7_NULL)
        {
          while (rlimLoopbackIdNextGet(loopbackId, &loopbackId) == L7_SUCCESS)
          {
            if ((rlimLoopbackIntIfNumGet(loopbackId, &intIfNum)== L7_SUCCESS) &&
                (ipMapIpIntfExists(intIfNum) == L7_TRUE) &&
                (ipMapRtrIntfOperModeGet(intIfNum, &ifState) == L7_SUCCESS) &&
                (ifState == L7_ENABLE))
            {
              if ((ipMapRtrIntfIpAddressGet(intIfNum,&ipAddr,&mask)== L7_SUCCESS) && 
                (ipAddr != L7_NULL_IP_ADDR))
              {
                *pIntIfNum = intIfNum;
                *pIpAddr = ipAddr; 
                *pIpMask = mask;
                *pLoopbackInUse = L7_TRUE;
                break;
              }
            }
          }/* end while loop */
        }/* if(*pIntIfNum == L7_NULL) */
      }/* if(rlimLoopbackIdFirstGet(&loopbackId) == L7_SUCCESS) */
#endif  
      if (*pIntIfNum == L7_NULL)
      {
        /* Get the first routing interface as no loopback interface */
        for (i = 1; i <= L7_RTR_MAX_RTR_INTERFACES; i++)
        {
          if ((ipMapRtrIntfToIntIfNum(i,&intIfNum) == L7_SUCCESS) &&
              (ipMapIpIntfExists(intIfNum) == L7_TRUE) &&
              (ipMapRtrIntfOperModeGet(intIfNum, &ifState) == L7_SUCCESS) &&
              (ifState == L7_ENABLE))
          {
            if ((ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &mask) == L7_SUCCESS) &&
                (ipAddr != L7_NULL_IP_ADDR))
            {
              *pIntIfNum = intIfNum;
              *pIpAddr = ipAddr; 
              *pIpMask = mask;
              *pLoopbackInUse = L7_FALSE;
              break;
            }
          }/*end if */
        }/* end for loop */
      }/* end if (*pIntIfNum == L7_NULL) */
    }
  
    if (*pIntIfNum == L7_NULL)
    {
      *pReason = SYSAPI_APP_IP_REASON_NO_LOOPBACK_INTERFACE;
      return L7_FAILURE;
    }
  
    if (*pIpAddr == L7_NULL_IP_ADDR)
    {
      *pReason = SYSAPI_APP_IP_REASON_NO_IP_ADDRESS;
      return L7_FAILURE;
    }
    return L7_SUCCESS;
  } /* if (*pRtrMode == L7_ENABLE) */

#endif /* L7_ROUTING_PACKAGE */

  ipAddr = simGetSystemIPAddr();
  mask   = simGetSystemIPNetMask();

  if (ipAddr == L7_NULL_IP_ADDR)
  {
    *pReason = SYSAPI_APP_IP_REASON_NO_IP_ADDRESS;
    *pIpAddr = L7_NULL_IP_ADDR;
    *pIpMask = L7_NULL_IP_MASK;
    return L7_FAILURE;
  }

  if ((*pIpAddr != L7_NULL_IP_ADDR) &&
      (*pIpAddr != ipAddr))
  {
    *pIpAddr = ipAddr;
    *pIpMask = mask;
    *pReason = SYSAPI_APP_IP_REASON_NO_IP_ADDRESS; /*signal IP change*/

    return L7_FAILURE;
  }

  *pIpAddr = ipAddr;
  *pIpMask = mask;
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose Verify if configured static IP address for wireless application 
*          component is same as network interface for switching package or 
*          a routing interface on a routing package.
*
* @param    staticIPAddr        @b{(input)} Configured static IP address 
*
* @returns L7_SUCCESS, Configured IP address match found.
*          L7_FAILURE, Configured IP address match not found.
*
* @notes  
*
* @end
*
*********************************************************************/
extern L7_RC_t sysapiApplicationIPAddressVerify(L7_uint32  staticIPAddr)
{

  L7_IP_ADDR_t  ipAddr;

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RLIM_PACKAGE
  L7_uint32     loopbackId;
#endif
  L7_IP_MASK_t  mask;
  L7_uint32     intIfNum;
  L7_uint32     rtrMode;
  L7_uint32     i;
  L7_uint32     ifState;
#endif

#ifdef L7_ROUTING_PACKAGE
  rtrMode = ipMapRtrAdminModeGet();

  if (rtrMode == L7_ENABLE)
  {
#ifdef L7_RLIM_PACKAGE
    /* First check if address belongs to loopback interface */
    if(rlimLoopbackIdFirstGet(&loopbackId) == L7_SUCCESS)
    {
      if((rlimLoopbackIntIfNumGet(loopbackId,&intIfNum)== L7_SUCCESS) &&
         (ipMapIpIntfExists(intIfNum) == L7_TRUE) &&
         (ipMapRtrIntfOperModeGet(intIfNum, &ifState) == L7_SUCCESS) &&
         (ifState == L7_ENABLE))
      {
        if ((ipMapRtrIntfIpAddressGet(intIfNum,&ipAddr,&mask)== L7_SUCCESS) && 
            (ipAddr != L7_NULL_IP_ADDR) && 
            (ipAddr == staticIPAddr))
        {
           /* static IP address matches with loopback interface IP address */
           return L7_SUCCESS;
        }
      }

        while (rlimLoopbackIdNextGet(loopbackId, &loopbackId) == L7_SUCCESS)
          {
            if ((rlimLoopbackIntIfNumGet(loopbackId, &intIfNum)== L7_SUCCESS) &&
                (ipMapIpIntfExists(intIfNum) == L7_TRUE) &&
                (ipMapRtrIntfOperModeGet(intIfNum, &ifState) == L7_SUCCESS) &&
                (ifState == L7_ENABLE))
            {
              if ((ipMapRtrIntfIpAddressGet(intIfNum,&ipAddr,&mask)== L7_SUCCESS) && 
                  (ipAddr != L7_NULL_IP_ADDR) &&
                  (ipAddr == staticIPAddr))
              {
                 /* static IP address matches with loopback interface IP address */       
                 return L7_SUCCESS;
              }
            }
          }/* end while loop */ 

      }/* if(rlimLoopbackIdFirstGet(&loopbackId) == L7_SUCCESS) */
#endif

        /* Get the first routing interface as no loopback interface */
        for (i = 1; i <= L7_RTR_MAX_RTR_INTERFACES; i++)
        {
          if ((ipMapRtrIntfToIntIfNum(i,&intIfNum) == L7_SUCCESS) &&
              (ipMapIpIntfExists(intIfNum) == L7_TRUE) &&
              (ipMapRtrIntfOperModeGet(intIfNum, &ifState) == L7_SUCCESS) &&
              (ifState == L7_ENABLE))
          {
            if ((ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &mask) == L7_SUCCESS) &&
                (ipAddr != L7_NULL_IP_ADDR) &&
                (ipAddr == staticIPAddr))
            {
                /* static IP address matches with routing interface IP address */
                return L7_SUCCESS;
            }
          }/*end if */
        }/* end for loop */

        /* No matching routing interface IP adress for configured static IP address */
        return L7_FAILURE;
  }
#endif

   ipAddr = simGetSystemIPAddr();
   if ((ipAddr != L7_NULL_IP_ADDR) &&
       (ipAddr == staticIPAddr))
   {
     return L7_SUCCESS;
   }

   /* Network interface IP adress not matching static IP address */
   return L7_FAILURE;
}
