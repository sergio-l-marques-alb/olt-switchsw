
/**
 * ptin_opensaf.c
 *
 * Implements the opensaf interface module
 *
 * Created on: 2016/07/04 Author: Rui Fernandes(rui-f-fernandes@alticealbs.com)
 * Notes:
 *
 */
#ifndef _PTIN_OPENSAF_H
#define _PTIN_OPENSAF_H

#include "ptin_include.h"
#include "ptin_igmp.h"
#include "osapi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "ptin_utils.h"
#include "ptin_intf.h"
#include "usmdb_dhcp_snooping.h"
#include "fdb_api.h"


#ifdef OPENSAF_SUPPORTED
#include "ptin_opensaf_checkpoint.h"
#include "ptin_opensaf.h"
#include <saEvt.h>


/*******************************
 * FEATURES
 *******************************/


/***********************************************************
 * Typedefs
 ***********************************************************/
#define PTIN_MAX_OPENSAF_EVENT    20
#define STANDALONE_FLAG           1
#define ENCRYPTION_KEY_FIELD_SIZE 16
#define MAX_NGPON2_PORTS          32

#define ONU_STATE_INDEX(group, onu, slot, link) ( group != 0xFF    \
                                                    ? (  ( (group&0xFF) << 21 ) | ( 0 << 20 ) | (onu&0xFF) )  \
                                                    : (  ( (slot&0x7F) << 25 ) | ( (link&0x0F) << 21 ) | ( 1 << 20 ) | (onu&0xFF) )   \
                                                )


void *readData;
int readDataLen=0;
pthread_mutex_t readDataMux;
char readDataPublisher[32];
unsigned long long readEventID;
L7_BOOL readDataInitFlag = 0;

typedef struct
{

  SaEvtHandleT evtHandle;
  SaSelectionObjectT evtSelectionObject;
  SaNameT channelName;
  SaEvtChannelHandleT channelHandle;
  SaEvtSubscriptionIdT subscriptionId;
  SaNameT publisherName;
  L7_BOOL initialized;
  L7_BOOL processOwnEvent;
  L7_char8 channelNameStr[32];
  L7_char8 publisherNameStr[32];

} ptin_opensaf_event_t;

typedef enum {
	ONU_STATE_NOT_VALID              = 0,
	ONU_STATE_INACTIVE                  ,
	ONU_STATE_PROCESSING                ,
	ONU_STATE_DISABLED                  ,
	ONU_STATE_ACTIVE
}ptin_onu_state;

typedef struct {
    struct {
        L7_uint8     slot;       //slot for the port belonging to the NGPON2 group
        L7_uint8     link;       //link for a port from this slot belonging to the NGPON2 group
    } member[MAX_NGPON2_PORTS];
} __attribute__ ((packed)) ptin_OLTCTList;


static ptin_opensaf_event_t ptin_event[PTIN_MAX_OPENSAF_EVENT];

/*******************************
 * Debug procedures
 *******************************/

L7_BOOL   ptin_debug_opensaf_var = 0;

void ptin_debug_opensaf_enable(L7_BOOL enable)
{
  ptin_debug_opensaf_var = enable;
}

static L7_RC_t ptin_opensaf_check_event_initialization(int id, char *chName, char *pubName);
static L7_RC_t ptin_opensaf_eventhandle_init(int id, char *chName, char *pubName);
static L7_RC_t ptin_opensaf_eventhandle_deinit(int id);


/**
 * Read a event
 *
 * @param void data
 * @param int len
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_opensaf_read_event(void *data, int len, int id, char *chName, char *pubName)
{

  L7_uint32 saRet;
  L7_BOOL   own;
  struct pollfd fds[1];

  while (1)
  {
    while (1)
	{
	  ptin_opensaf_check_event_initialization(id, chName, pubName);

      fds[0].fd = ptin_event[id].evtSelectionObject;
      fds[0].events = POLLIN;

	  PT_LOG_TRACE(LOG_CTX_OPENSAF, "Waiting for events on channel %s", ptin_event[id].channelNameStr);

	  int res = poll(fds, 1, 60000);
	  if (res == 0)
      {
        //We use a timeout to avoid that poll gets blocked indefinitely when some problem with opensaf occurs.
        PT_LOG_TRACE(LOG_CTX_OPENSAF, "Refresh in poll");
        continue;
      }
	  if (res == -1)
	  {
	  	PT_LOG_ERR(LOG_CTX_OPENSAF, "Error in poll: %s", strerror(errno));
	  	osapiSleepMSec(500); 
	  	continue;
	  }

	  if (fds[0].revents & POLLIN)
	  {
	  	PT_LOG_TRACE(LOG_CTX_OPENSAF, "Got event on channel %s", ptin_event[id].channelNameStr);

	  	pthread_mutex_lock(&readDataMux);   
	  	readData = data;
	  	readDataLen = len;
	  	bzero(readDataPublisher, sizeof(readDataPublisher));
	  	saRet = saEvtDispatch(ptin_event[id].evtHandle, SA_DISPATCH_ONE);
	  	data = readData;
	  	len = readDataLen;
        PT_LOG_TRACE(LOG_CTX_OPENSAF, "From %s (I am %s), ID %llu, len %d ", readDataPublisher, ptin_event[id].publisherNameStr, readEventID, len);

        PT_LOG_TRACE(LOG_CTX_OPENSAF, "len %d", len);

	  	if (strcmp(readDataPublisher, ptin_event[id].publisherNameStr) != 0)
        {
	      own = 0;
        }
	  	else
        {
	      own = 1;
        }

	    pthread_mutex_unlock(&readDataMux);
	    break;
	  }
	}

	 if (ptin_event[id].processOwnEvent == L7_FALSE && own == 1)
	 {
	 	PT_LOG_INFO(LOG_CTX_OPENSAF, "... this is my own event, ignore");
	 	continue;
	 }

	 if (saRet != SA_AIS_OK || len < 0)
	 {
	 	PT_LOG_ERR(LOG_CTX_OPENSAF, "Error loading event from channel %s  len %d saRet %d", ptin_event[id].channelNameStr, len , saRet);

	 	if (saRet != SA_AIS_OK)
    {
	 	  ptin_opensaf_eventhandle_deinit(id);
    }
	   return L7_FAILURE;
	 }
    break;
  }

  PT_LOG_TRACE(LOG_CTX_OPENSAF, "Dispatched event from channel %s", ptin_event[id].channelNameStr);
  return L7_SUCCESS;
}


void ptin_opensaf_task_OnuMac( void )
{
  static ptin_opensaf_ngpon2_onustate event_data;
  //char data[100]      = "";
  int  size           = 0;
  char *chName,*pubName;// *p;
  L7_enetMacAddr_t mac;;
  L7_uint8 slot;
  //L7_RC_t rc = L7_FAILURE;
  static ptin_onuStatus onuStatus;
  //ptin_OLTCTList ngpon2_members;

  memset(&mac, 0, sizeof(mac));

  chName        = "ONU_STATE";
  pubName       = "Fastpath";

  if (osapiTaskInitDone(L7_PTIN_OPENSAF_TASK) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_OPENSAF, "Error syncing task");
    PTIN_CRASH();
  }
 
  ptin_opensaf_eventhandle_init(1, chName, pubName);

  /* Loop */
  while (1)
  {
    osapiSleepMSec(50); 

    PT_LOG_TRACE(LOG_CTX_OPENSAF, "ptin_opensaf_task_OnuMac running...");

    int len = sizeof(event_data);

    /* wait for a event in the ONUSTATE*/
    ptin_opensaf_read_event(&event_data, len, 1, chName, pubName);
  
    /* Read the event data */
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "eventId      %d",  event_data.eventId);
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "memberIndex  %d",  event_data.memberIndex);
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "parentId     %d",  event_data.parentId);
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "OnuID        %d",  event_data.onuId);
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "linkId       %d",  event_data.linkId);
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "slotId       %d",  event_data.slotId);

    /* Key to get the slot of the ONU from the NGPON2GROUPS checkpoint */
    ptin_intf_slot_get(&slot);

    if(slot != event_data.slotId)
    {
      PT_LOG_TRACE(LOG_CTX_OPENSAF, "slot %u", slot);
      continue;
    }

    L7_uint32 section = ONU_STATE_INDEX(event_data.parentId , event_data.onuId, event_data.slotId, event_data.linkId);

    /* Get ONU State */
    ptin_opensaf_read_checkpoint(&onuStatus,sizeof(onuStatus),ENDIAN_SWAP32(section),0,ONU_STATE);

#if 0
    ptin_opensaf_read_checkpoint(&data,sizeof(data),event_data.onuId,0,SWITCHDRVR_ONU);

    /*Retrieve MAC form a particular ONU*/
    p = data;
#endif


    PT_LOG_TRACE(LOG_CTX_OPENSAF, "section %d", ONU_STATE_INDEX(event_data.parentId , event_data.onuId, event_data.slotId, event_data.linkId));
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "section %d", section);
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "onuStatus.state %d", onuStatus.state);

    if(onuStatus.state == ONU_STATE_INACTIVE) /* If the ONU is disable remove MAC, DHCP binding and IGMP channels*/
    {    
      L7_uint32 servicesId[PTIN_SYSTEM_MAX_SERVICES_PER_ONU];
      L7_uint32 nOfServices = -1;

      /* reset querier flag*/
      ptin_igmp_multicast_get_all_serviceId_per_onu(event_data.memberIndex, event_data.onuId, servicesId, &nOfServices);

      PT_LOG_TRACE(LOG_CTX_OPENSAF, "size %u", size);

#if 0
      int i = 0;   
      while(size > i) /* Flush the each MAC from the L2 table and DHCP binding table */
      {
        memcpy(&mac.addr, p, sizeof(mac.addr));

        PT_LOG_TRACE(LOG_CTX_OPENSAF,"Search Data : %d , %d ,%d ,%d , %d , %d, ",    mac.addr[0], mac.addr[1], mac.addr[2],
                                                                            mac.addr[3], mac.addr[4], mac.addr[5]);
        /* MAC */
        rc = fdbFlushByMac(mac);

        /* IPv6 */
        //usmDbDsBindingRemove((L7_enetMacAddr_t*)mac.addr, L7_AF_INET6);

        /* IPv4 */
        //usmDbDsBindingRemove((L7_enetMacAddr_t*)mac.addr, L7_AF_INET);

        PT_LOG_TRACE(LOG_CTX_OPENSAF, "rc %u", rc);

        i = i + MAC_SIZE_BYTES;
        p = p + MAC_SIZE_BYTES;
        /* get next MAC */
      }
      /* Delete ONT MAC section from the checkpoint */
      ptin_opensaf_checkpoint_deleteSection(SWITCHDRVR_ONU, event_data.onuId);
#endif
    }
    
    if (onuStatus.state == ONU_STATE_ACTIVE)
    {
      L7_uint32 servicesId[PTIN_SYSTEM_MAX_SERVICES_PER_ONU];
      L7_uint32 nOfServices = (L7_uint32) 0;      

#if 0
      dhcpSnoopBinding_t  dsBindingIpv4,dsBindingIpv6;

      memset(&servicesId, (L7_uint32) -1, sizeof(servicesId));
      memcpy(&mac.addr, p, sizeof(mac.addr));

      PT_LOG_TRACE(LOG_CTX_OPENSAF,"Search Data : %d , %d ,%d ,%d , %d , %d, ",    mac.addr[0], mac.addr[1], mac.addr[2],
                                                                            mac.addr[3], mac.addr[4], mac.addr[5]);
      memset(&dsBindingIpv4,0x00,sizeof(dhcpSnoopBinding_t));
      memcpy(dsBindingIpv4.key.macAddr, mac.addr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
      dsBindingIpv4.key.ipType = L7_AF_INET;  //(table[i].bind_entry.ipAddr.family==0) ;//? (L7_AF_INET) : (L7_AF_INET6);

      memset(&dsBindingIpv6,0x00,sizeof(dhcpSnoopBinding_t));
      memcpy(dsBindingIpv6.key.macAddr, mac.addr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
      dsBindingIpv6.key.ipType = L7_AF_INET6;  //(table[i].bind_entry.ipAddr.family==0) ;//? (L7_AF_INET) : (L7_AF_INET6);

      PT_LOG_TRACE(LOG_CTX_OPENSAF, " event_data.memberIndex = %u, event_data.onuId = %u", event_data.memberIndex, event_data.onuId);

      PT_LOG_TRACE(LOG_CTX_OPENSAF, "Search Data : %c , %c ,%c ,%c , %c , %c, ",    dsBindingIpv4.key.macAddr[0], dsBindingIpv4.key.macAddr[1], dsBindingIpv4.key.macAddr[2],
                                                                                    dsBindingIpv4.key.macAddr[3], dsBindingIpv4.key.macAddr[4], dsBindingIpv4.key.macAddr[5]);

#endif
      memset(servicesId, 0, PTIN_SYSTEM_MAX_SERVICES_PER_ONU*sizeof(L7_uint32));
      ptin_igmp_multicast_get_all_serviceId_per_onu(event_data.memberIndex, event_data.onuId, servicesId, &nOfServices);

#if 0
      PT_LOG_TRACE(LOG_CTX_OPENSAF, " event_data.memberIndex = %u, event_data.onuId = %u", event_data.memberIndex, event_data.onuId);
      usmDbDsBindingGet(&dsBindingIpv4);

      //PT_LOG_TRACE(LOG_CTX_OPENSAF, " event_data.memberIndex = %u, event_data.onuId = %u", event_data.memberIndex, event_data.onuId);
      usmDbDsLeaseStatusUpdateIntf(&dsBindingIpv4.key, L7_AF_INET6, event_data.memberIndex);
#endif

      L7_uint32 i = 0;
      PT_LOG_TRACE(LOG_CTX_OPENSAF, " nOfServices = %d,", nOfServices);
      while (i < nOfServices)
      {
        PT_LOG_TRACE(LOG_CTX_OPENSAF, " servicesId[%d] = %u,", i, servicesId[i]);
        if (servicesId[i] <= PTIN_SYSTEM_N_EVCS)
        {
          ptin_igmp_multicast_querierReset_on_specific_serviceID(event_data.memberIndex, event_data.onuId, servicesId[i]);
        }
        i++;
      }
    }  
        
  }
  
}

//static void (*SaEvtEventDeliverCallbackT ) (SaEvtSubscriptionIdT, SaEvtHandleT, const SaSizeT);

/********************************Start PTin Opensaf Event*********************************************************/

/**
 * Process a event
 *
 * @param SaEvtSubscriptionIdT subId
 * @param SaEvtEventHandleT eventHdl
 * @param SaSizeT eventDataSize
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
void processEvent( SaEvtSubscriptionIdT subId ,SaEvtEventHandleT eventHdl, SaSizeT eventDataSize)
{
  int saRet;
  SaSizeT len;
  SaUint8T priority;
  SaTimeT retentionTime;
  SaNameT publisherName;
  SaTimeT publishTime;
  SaEvtEventIdT eventId;
  SaEvtEventPatternArrayT patternArray;

  patternArray.patterns = NULL;
  bzero(&publisherName, sizeof(publisherName));

  saRet = saEvtEventAttributesGet(eventHdl, &patternArray,
  								&priority, &retentionTime,
  								&publisherName, &publishTime,
  								&eventId);
  if (saRet != SA_AIS_OK)
  {
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "Error creating a mutex for IGMP module");
  }

  saEvtEventPatternFree(eventHdl, patternArray.patterns);

  readEventID = (unsigned long long)eventId;
  snprintf(readDataPublisher, sizeof(readDataPublisher), (char *)publisherName.value);
  len = readDataLen;

  if (len > 0)
  {
    saRet = saEvtEventDataGet(eventHdl, readData, &len);

    if (saRet != SA_AIS_OK)
    {
      PT_LOG_TRACE(LOG_CTX_OPENSAF, " Error %d getting data from event", saRet);
      readData = NULL;
      readDataLen = -1;
    }
  }
  else
  {
    saRet = SA_AIS_OK;
  }

  saEvtEventFree(eventHdl);
}

/**
 * Check if the event handle is initialize
 *
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_opensaf_check_event_initialization(int id, char *chName, char *pubName)
{
  int saRet = 0;

  if (ptin_event[id].initialized == 0)
  {
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "Checkpoint is not initialized: %s", ptin_event[id].channelNameStr);
  	saRet = ptin_opensaf_eventhandle_init(id ,chName, pubName);
  }

  return saRet;
}

/***********************************************************
 * FUNCTIONS
 ***********************************************************/

/**
 * Initialize a event handle
 *
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_opensaf_eventhandle_init(int id, char *chName, char *pubName)
{

  L7_uint8                saRet;
  SaVersionT              version;
  SaEvtChannelOpenFlagsT  channelOpenFlags;
  SaTimeT timeout =       100000000000ll;
  SaEvtEventFilterArrayT  filterArray;
  SaEvtCallbacksT         regCallbackSet;

  char namedn[128];
  int namelen;
  version.releaseCode = 'B';
  version.majorVersion = 0x01;
  version.minorVersion  = 0x01;


  snprintf(ptin_event[id].channelNameStr, sizeof(ptin_event[id].channelNameStr), "%s", chName);
  namelen = snprintf(namedn, sizeof(namedn), "safChnl=%s,safApp=safEvtService", chName);

  bzero(&ptin_event[id].channelName, sizeof(ptin_event[id].channelName));
  memcpy(ptin_event[id].channelName.value, namedn, namelen);
  ptin_event[id].channelName.length = namelen;

  snprintf(ptin_event[id].publisherNameStr, sizeof(ptin_event[id].publisherNameStr), "%s", pubName);

  bzero(&ptin_event[id].publisherName, sizeof(ptin_event[id].publisherName));
  memcpy(ptin_event[id].publisherName.value, ptin_event[id].publisherNameStr, strlen(ptin_event[id].publisherNameStr));
  ptin_event[id].publisherName.length = strlen(ptin_event[id].publisherNameStr);

  memset(&regCallbackSet.saEvtEventDeliverCallback,0,sizeof(regCallbackSet.saEvtEventDeliverCallback));

  regCallbackSet.saEvtEventDeliverCallback = processEvent;

  saRet = saEvtInitialize(&ptin_event[id].evtHandle, &regCallbackSet, &version);

  if (saRet != SA_AIS_OK)
  {
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "Error %d initializing opensaf for event %s", saRet, ptin_event[id].channelNameStr);
    return L7_FAILURE;
  }

  saRet = saEvtSelectionObjectGet(ptin_event[id].evtHandle, &ptin_event[id].evtSelectionObject);

  if (saRet != SA_AIS_OK)
  {
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "Error %d getting selection object for event %s", saRet, ptin_event[id].channelNameStr);
    return L7_FAILURE;
  }

  channelOpenFlags = SA_EVT_CHANNEL_SUBSCRIBER;
  saRet = saEvtChannelOpen(ptin_event[id].evtHandle, &ptin_event[id].channelName, channelOpenFlags, timeout, &ptin_event[id].channelHandle);

  if (saRet != SA_AIS_OK)
  {
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "Error %d oppening channel %s", saRet, ptin_event[id].channelNameStr);
    return L7_FAILURE;
  }

  filterArray.filtersNumber = 0;
  filterArray.filters = NULL;
  ptin_event[id].subscriptionId = 0;

  saRet = saEvtEventSubscribe(ptin_event[id].channelHandle, &filterArray, ptin_event[id].subscriptionId);

  if (saRet != SA_AIS_OK)
  {
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "Error %d subscribing channel %s", saRet, ptin_event[id].channelNameStr);
    return L7_FAILURE;
  }

  PT_LOG_INFO(LOG_CTX_OPENSAF, "Opensaf initialized and Event channel opened: %s", ptin_event[id].channelNameStr);
  PT_LOG_INFO(LOG_CTX_OPENSAF, "I am publisher %s", ptin_event[id].publisherNameStr);
  ptin_event[id].initialized = 1;

  return L7_SUCCESS;
}

/**
 * Denitialize a event handle
 *
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_opensaf_eventhandle_deinit(int id)
{
  saEvtEventUnsubscribe(ptin_event[id].channelHandle, ptin_event[id].subscriptionId);
  saEvtChannelClose(ptin_event[id].channelHandle);
  saEvtFinalize(ptin_event[id].evtHandle);

  ptin_event[id].initialized = 0;

  return L7_SUCCESS;
}


/**
 * Teste Initialize a event handle
 *
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_opensaf_eventhandle_init_teste(int id)
{
  char *chName  = "ONU_STATE";
  char *pubName = " teste_pub ";

  ptin_opensaf_eventhandle_init(id, chName, pubName);

  return L7_SUCCESS;
}

/**
 * Teste a event
 *
 * @param void data
 * @param int len
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_opensaf_read_event_teste(int id)
{

  char teste[256];
  char *chName  = "PPAUCTL";
  char *pubName = " teste_pub ";
  int  i;

  ptin_opensaf_read_event(teste, 256, id, chName, pubName);

  for( i = 0;i<255;i++)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "%c", teste[i]);
  }

  return L7_SUCCESS;
}

/**
 *  Init routine of opensaf event task
 *
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_opensaf_event_task_init()
{
  L7_uint64 ptin_opensaf_TaskId = 0;
  char *chName,*pubName;

  chName        = "ONU_STATE";
  pubName       = "Fastpath"; 
  ptin_opensaf_eventhandle_init(1, chName, pubName);

  // Create task for ptin opensaf processing
  ptin_opensaf_TaskId = osapiTaskCreate("ptin_opensaf_task_OnuMac", ptin_opensaf_task_OnuMac, 0, 0,
                                                L7_DEFAULT_STACK_SIZE,
                                                L7_DEFAULT_TASK_PRIORITY,
                                                L7_DEFAULT_TASK_SLICE);

  if (ptin_opensaf_TaskId == L7_ERROR)
  {
    PT_LOG_FATAL(LOG_CTX_OPENSAF, "Could not create task ptin_opensaf_task_OnuMac");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_OPENSAF,"Task ptin_opensaf_task_OnuMac created");

  if (osapiWaitForTaskInit(L7_PTIN_OPENSAF_TASK, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_OPENSAF,"Unable to initialize ptin_opensaf_task_OnuMac()");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_OPENSAF,"Task ptin_opensaf_task_OnuMac initialized");

  return L7_SUCCESS;
}

#endif /*OPENSAF_SUPPORTED*/

#endif
/********************************End PTin Opensaf Event****************************************************************/


