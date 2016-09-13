
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
#include "osapi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "ptin_utils.h"
#include "ptin_opensaf.h"
#include "ptin_opensaf_checkpoint.h"
#include "fdb_api.h"

#if (PTIN_BOARD != PTIN_BOARD_OLT1T0)
#include <saEvt.h>


/******************************* 
 * FEATURES
 *******************************/


/*********************************************************** 
 * Typedefs
 ***********************************************************/

void *readData;
int readDataLen=0;
pthread_mutex_t readDataMux;
char readDataPublisher[32];
unsigned long long readEventID;
L7_BOOL readDataInitFlag = 0;

#define PTIN_MAX_OPENSAF_EVENT 10

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

ptin_opensaf_event_t ptin_event[PTIN_MAX_OPENSAF_EVENT];

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
	  
	  PT_LOG_DEBUG(LOG_CTX_OPENSAF, "Waiting for events on channel %s", ptin_event[id].channelNameStr);

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
	  	sleep(2);
	  	continue;
	  }

	  if (fds[0].revents & POLLIN)
	  {
	  	PT_LOG_DEBUG(LOG_CTX_OPENSAF, "Got event on channel %s", ptin_event[id].channelNameStr);

	  	pthread_mutex_lock(&readDataMux);
	  	readData = data;
	  	readDataLen = len;
	  	bzero(readDataPublisher, sizeof(readDataPublisher));
	  	saRet = saEvtDispatch(ptin_event[id].evtHandle, SA_DISPATCH_ONE);
	  	data = readData;
	  	len = readDataLen;
	  	PT_LOG_DEBUG(LOG_CTX_OPENSAF, "From %s (I am %s), ID %llu", readDataPublisher, ptin_event[id].publisherNameStr, readEventID);

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
	 	PT_LOG_ERR(LOG_CTX_OPENSAF, "Error loading event from channel %s", ptin_event[id].channelNameStr);

	 	if (saRet != SA_AIS_OK)
        {
	 	  ptin_opensaf_eventhandle_deinit(id);
        }
	   return L7_FAILURE;
	 }
    break;
  }

  PT_LOG_DEBUG(LOG_CTX_OPENSAF, "Dispatched event from channel %s", ptin_event[id].channelNameStr);
  return L7_SUCCESS;
}


void ptin_opensaf_task_OnuMac( void )
{
  ptin_opensaf_ngpon2_onustate event_data;
  unsigned char data[100]      = "";
  int           size           = 0;
  unsigned char *chName,*p,*pubName; 
  L7_enetMacAddr_t mac;
  L7_RC_t rc = L7_FAILURE;      

  memset(&mac, 0, sizeof(mac));

  chName        = "PPAUCTL";
  pubName       = "teste_pub ";

  if (osapiTaskInitDone(L7_PTIN_OPENSAF_TASK) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_OPENSAF, "Error syncing task");
    PTIN_CRASH();
  }

  ptin_opensaf_eventhandle_init(1, chName, pubName);

  /* Loop */
  while (1) 
  {
    PT_LOG_INFO(LOG_CTX_OPENSAF, "ptin_opensaf_task_OnuMac running...");
    ptin_opensaf_read_event(&event_data, sizeof(event_data), 1, chName, pubName);

    //if(flush mac)
   
    event_data.onuId = 1;
       
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "eventId     %u", event_data.eventId);
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "memberIndex %u", event_data.memberIndex);
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "parentId    %u", event_data.parentId);
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "OnuID       %u", event_data.onuId);

    ptin_checkpoint_getSection(1, 1/*event_data.onuId*/, &data, &size);

    PT_LOG_TRACE(LOG_CTX_OPENSAF, "size %u", size);

    int i = 0;
    p = data;

    while(size > i)
    {     
      memcpy(&mac.addr, p, sizeof(mac.addr));

      PT_LOG_TRACE(LOG_CTX_OPENSAF,"Search Data : %c , %c ,%c ,%c , %c , %c, ",    mac.addr[0], mac.addr[1], mac.addr[2], 
                                                                          mac.addr[3], mac.addr[4], mac.addr[5]);
      rc = fdbFlushByMac(mac);

      PT_LOG_TRACE(LOG_CTX_OPENSAF, "rc %u", rc);

      i = i + MAC_SIZE_BYTES;
      p = p + MAC_SIZE_BYTES;
    }  
    ptin_opensaf_checkpoint_deleteSection(1, 1);
  }
                                            
  osapiSleepMSec(500);
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
  	saRet = ptin_opensaf_eventhandle_init(id ,chName, pubName );
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

  channelOpenFlags = SA_EVT_CHANNEL_CREATE|SA_EVT_CHANNEL_SUBSCRIBER|SA_EVT_CHANNEL_PUBLISHER;
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
  char *chName  = "PPAUCTL";
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
  L7_uint32 ptin_opensaf_TaskId = 0;
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

#endif

#endif
/********************************End PTin Opensaf Event****************************************************************/


