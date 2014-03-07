/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @create    21/10/2013
*
* @author    Daniel Filipe Figueira
* @author    Marcio Daniel Melo
*
**********************************************************************/
#include "ptin_mgmd_cnfgr.h"
#include "ptin_mgmd_util.h"
#include "ptin_mgmd_core.h"
#include "ptin_mgmd_logger.h"
#include "ptin_mgmd_defs.h"
#include "ptin_mgmd_cfg.h"
#include "ptin_mgmd_osapi.h"
#include "ptin_mgmd_db.h"
#include "ptin_mgmd_statistics.h"
#include "ptin_mgmd_proxycmtimer.h"
#include "memory.h"
#include "ptin_fifo_api.h"

static ptin_mgmd_eb_t    mgmdEB;           /* Snoop execution block holder */
static ptin_mgmd_cb_t   *mgmdCB = PTIN_NULLPTR;    /* Mgmd Control blocks holder */

extern unsigned long     ptin_mgmd_memory_allocation;

void ptin_mgmd_cnfgr_memory_allocation(void)
{
  ptin_mgmd_memory_allocation+=sizeof(ptin_mgmd_eb_t);  
}

/*********************************************************************
* @purpose  MGMD Router Execution block initializations
*
* @param    None
*
* @returns  SUCCESS - Initialization complete
*           FAILURE - Initilaization failed because of
*                        insufficient system resources
*
* @notes
*
* @end
*********************************************************************/
RC_t ptinMgmdGroupAVLTreeInit(void)
{
  ptin_mgmd_eb_t *pSnoopEB;
  uint32          i;

  pSnoopEB = &mgmdEB;

  pSnoopEB->ptinMgmdGroupTreeHeap = (ptin_mgmd_avlTreeTables_t *)       ptin_mgmd_malloc(PTIN_MGMD_MAX_GROUPS*sizeof(ptin_mgmd_avlTreeTables_t));  
  pSnoopEB->ptinMgmdGroupDataHeap = (ptinMgmdGroupInfoData_t *) ptin_mgmd_malloc(PTIN_MGMD_MAX_GROUPS*sizeof(ptinMgmdGroupInfoData_t));  

  if ((pSnoopEB->ptinMgmdGroupTreeHeap == PTIN_NULLPTR) || (pSnoopEB->ptinMgmdGroupDataHeap == PTIN_NULLPTR))
  {
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Error allocating data for snoopPtinRouterAVLTreeInit");    
    return FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset(&pSnoopEB->ptinMgmdGroupAvlTree, 0x00, sizeof(pSnoopEB->ptinMgmdGroupAvlTree));

  /* AVL Tree creations - snoopAvlTree*/
  ptin_mgmd_avlCreateAvlTree(&(pSnoopEB->ptinMgmdGroupAvlTree), pSnoopEB->ptinMgmdGroupTreeHeap, pSnoopEB->ptinMgmdGroupDataHeap,
                   PTIN_MGMD_MAX_GROUPS, sizeof(ptinMgmdGroupInfoData_t), 0x10, sizeof(ptinMgmdGroupInfoDataKey_t));

  /* Create the FIFO queue for the sources */
  ptin_fifo_create(&pSnoopEB->sourcesQueue, (PTIN_MGMD_MAX_PORTS+1)*PTIN_MGMD_MAX_SOURCES);//Plus 1 for the root port
  for(i=0; i<((PTIN_MGMD_MAX_PORTS+1)*PTIN_MGMD_MAX_SOURCES); ++i) //Plus 1 for the root port
  {
    ptinMgmdSource_t *new_source = (ptinMgmdSource_t*) ptin_mgmd_malloc(sizeof(ptinMgmdSource_t));    
    
    ptin_fifo_push(pSnoopEB->sourcesQueue, (PTIN_FIFO_ELEMENT_t)new_source);
  }

  /* Create the Leaf Port Client Bitmap and the Leaf Source Client Bitmap*/
  ptin_fifo_create(&pSnoopEB->leafClientBitmap, PTIN_MGMD_MAX_PORTS+PTIN_MGMD_MAX_PORTS*PTIN_MGMD_MAX_SOURCES);
  for(i=0; i<(PTIN_MGMD_MAX_PORTS*PTIN_MGMD_MAX_SOURCES+PTIN_MGMD_MAX_PORTS); ++i) 
  {
    ptinMgmdLeafClient_t *new_element = (ptinMgmdLeafClient_t*) ptin_mgmd_malloc(sizeof(ptinMgmdLeafClient_t));   
    
    ptin_fifo_push(pSnoopEB->leafClientBitmap, (PTIN_FIFO_ELEMENT_t)new_element);
  }

  /* Create the Root Port Client Bitmap and the Root Source Client Bitmap*/
  ptin_fifo_create(&pSnoopEB->rootClientBitmap, 1+PTIN_MGMD_MAX_SOURCES);
  for(i=0; i<(1+PTIN_MGMD_MAX_SOURCES); ++i) 
  {
    ptinMgmdRootClient_t *new_element = (ptinMgmdRootClient_t*) ptin_mgmd_malloc(sizeof(ptinMgmdRootClient_t));
       
    ptin_fifo_push(pSnoopEB->rootClientBitmap, (PTIN_FIFO_ELEMENT_t)new_element);
  }
  return SUCCESS;
}


/*********************************************************************
* @purpose  Proxy Source Execution block initializations
*
* @param    None
*
* @returns  SUCCESS - Initialization complete
*           FAILURE - Initilaization failed because of
*                        insufficient system resources
*
* @notes
*
* @end
*********************************************************************/
RC_t ptinMgmdGroupRecordSourceAVLTreeInit(void)
{
  ptin_mgmd_eb_t *pSnoopEB;
  pSnoopEB = &mgmdEB;

  pSnoopEB->snoopPTinProxySourceTreeHeap = (ptin_mgmd_avlTreeTables_t *)        ptin_mgmd_malloc(PTIN_MGMD_MAX_GROUPS*sizeof(ptin_mgmd_avlTreeTables_t));
  pSnoopEB->snoopPTinProxySourceDataHeap = (snoopPTinSourceRecord_t *) ptin_mgmd_malloc(PTIN_MGMD_MAX_GROUPS*sizeof(snoopPTinSourceRecord_t));
  
  if ((pSnoopEB->snoopPTinProxySourceTreeHeap == PTIN_NULLPTR) || (pSnoopEB->snoopPTinProxySourceDataHeap == PTIN_NULLPTR))
  {
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Error allocating data for snoopPtinProxySourceAVLTreeInit");    
    return FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset(&pSnoopEB->snoopPTinProxySourceAvlTree, 0x00, sizeof(pSnoopEB->snoopPTinProxySourceAvlTree));

  /* AVL Tree creations - snoopAvlTree*/
  ptin_mgmd_avlCreateAvlTree(&(pSnoopEB->snoopPTinProxySourceAvlTree), pSnoopEB->snoopPTinProxySourceTreeHeap, pSnoopEB->snoopPTinProxySourceDataHeap,
                   PTIN_MGMD_MAX_GROUPS, sizeof(snoopPTinSourceRecord_t), 0x10, sizeof(snoopPTinSourceRecordKey_t));
  return SUCCESS;
}

/*********************************************************************
* @purpose  Proxy Group Execution block initializations
*
* @param    None
*
* @returns  SUCCESS - Initialization complete
*           FAILURE - Initilaization failed because of
*                        insufficient system resources
*
* @notes
*
* @end
*********************************************************************/
RC_t ptinMgmdGroupRecordGroupAVLTreeInit(void)
{
  ptin_mgmd_eb_t *pSnoopEB;
  pSnoopEB = &mgmdEB;

  pSnoopEB->snoopPTinProxyGroupTreeHeap = (ptin_mgmd_avlTreeTables_t *)       ptin_mgmd_malloc(PTIN_MGMD_MAX_GROUPS*sizeof(ptin_mgmd_avlTreeTables_t));
  pSnoopEB->snoopPTinProxyGroupDataHeap = (mgmdGroupRecord_t *) ptin_mgmd_malloc(PTIN_MGMD_MAX_GROUPS*sizeof(mgmdGroupRecord_t));

  if ((pSnoopEB->snoopPTinProxyGroupTreeHeap == PTIN_NULLPTR) || (pSnoopEB->snoopPTinProxyGroupDataHeap == PTIN_NULLPTR))
  {
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Error allocating data for snoopPtinProxyGroupAVLTreeInit");   
    return FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset(&pSnoopEB->snoopPTinProxyGroupAvlTree, 0x00, sizeof(pSnoopEB->snoopPTinProxyGroupAvlTree));

  /* AVL Tree creations - snoopAvlTree*/
  ptin_mgmd_avlCreateAvlTree(&(pSnoopEB->snoopPTinProxyGroupAvlTree), pSnoopEB->snoopPTinProxyGroupTreeHeap, pSnoopEB->snoopPTinProxyGroupDataHeap,
                   PTIN_MGMD_MAX_GROUPS, sizeof(mgmdGroupRecord_t), 0x10, sizeof(mgmdGroupRecordKey_t));
  return SUCCESS;
}


/*********************************************************************
* @purpose  Proxy Interface Execution block initializations
*
* @param    None
*
* @returns  SUCCESS - Initialization complete
*           FAILURE - Initilaization failed because of
*                        insufficient system resources
*
* @notes
*
* @end
*********************************************************************/
RC_t ptinMgmdRootInterfaceAVLTreeInit(void)
{
  ptin_mgmd_eb_t *pSnoopEB;
  pSnoopEB = &mgmdEB;

  pSnoopEB->snoopPTinProxyInterfaceTreeHeap = (ptin_mgmd_avlTreeTables_t *)           ptin_mgmd_malloc(PTIN_MGMD_MAX_SERVICES*sizeof(ptin_mgmd_avlTreeTables_t));
  pSnoopEB->snoopPTinProxyInterfaceDataHeap = (mgmdProxyInterface_t *) ptin_mgmd_malloc(PTIN_MGMD_MAX_SERVICES*sizeof(mgmdProxyInterface_t));
  
  if ((pSnoopEB->snoopPTinProxyInterfaceTreeHeap == PTIN_NULLPTR) || (pSnoopEB->snoopPTinProxyInterfaceDataHeap == PTIN_NULLPTR))
  {
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Error allocating data for snoopPtinProxyInterfaceAVLTreeInit");   
    return FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset(&pSnoopEB->snoopPTinProxyInterfaceAvlTree, 0x00, sizeof(pSnoopEB->snoopPTinProxyInterfaceAvlTree));

  /* AVL Tree creations - snoopAvlTree*/
  ptin_mgmd_avlCreateAvlTree(&(pSnoopEB->snoopPTinProxyInterfaceAvlTree), pSnoopEB->snoopPTinProxyInterfaceTreeHeap, pSnoopEB->snoopPTinProxyInterfaceDataHeap,
                   PTIN_MGMD_MAX_SERVICES, sizeof(mgmdProxyInterface_t), 0x10, sizeof(snoopPTinProxyInterfaceKey_t));
  return SUCCESS;
}


/*********************************************************************
* @purpose  Mgmd Execution block initializations
*
* @param    None
*
* @returns  SUCCESS - Initialization complete
*           FAILURE - Initilaization failed because of
*                        insufficient system resources
*
* @notes
*
* @end
*********************************************************************/
RC_t ptinMgmdEBInit(void)
{
  uchar8 family[PTIN_MGMD_MAX_CB_INSTANCES];

#if PTIN_MGMD_MAX_CB_INSTANCES==1
  family[0]=PTIN_MGMD_AF_INET;
#else
#if PTIN_MGMD_MAX_CB_INSTANCES==2
  family[0]=PTIN_MGMD_AF_INET;
  family[1]=PTIN_MGMD_AF_INET6;
#else
#error "PTIN_MGMD_MAX_CB_INSTANCES higher than the number of IP address families supported"
#endif
#endif

  uint8 cbIndex;
  mgmdEB.maxMgmdInstances=PTIN_MGMD_MAX_CB_INSTANCES;
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Setting maxSnoopInstances to :%u",mgmdEB.maxMgmdInstances);   


  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"snoopEB.maxSnoopInstances: Allocating %u Bytes",sizeof(ptin_mgmd_cb_t) *mgmdEB.maxMgmdInstances);
  if((mgmdCB = (ptin_mgmd_cb_t *)ptin_mgmd_malloc(sizeof(ptin_mgmd_cb_t) * mgmdEB.maxMgmdInstances))==PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed to allocate memory on mgmdCB");   
    return FAILURE;
  }  
  
  for (cbIndex=0;cbIndex<PTIN_MGMD_MAX_CB_INSTANCES && cbIndex<mgmdEB.maxMgmdInstances;cbIndex++)
  {    
    if(ptinMgmdCBInit(cbIndex,family[cbIndex])!=SUCCESS)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed ptinMgmdCBInit cbIndex:%u , family:%u",cbIndex,family[cbIndex]);  
      return ERROR;
    }
  }
  return SUCCESS;    
}

/*********************************************************************
*
* @purpose  Mgmd Control block initializations
*
* @param    cbIndex  - @b{(input)}  Instance index whose Cb is
*                                   to be initalized.
* @param    family      @b{(input)}  PTIN_MGMD_AF_INET  => IGMP Snooping
*                                    PTIN_MGMD_AF_INET6 => MLD Snooping
*
* @returns  SUCCESS - Initialization complete
*           FAILURE - Initilaization failed because of
*                        insufficient system resources
*
* @notes
*
* @end
*********************************************************************/
RC_t     ptinMgmdCBInit(uint32 cbIndex, uchar8 family)
{    
  ptin_mgmd_cb_t  *pMgmdCB = PTIN_NULLPTR;
  uint32         i;

  /* validate the cbIndex */
  if (cbIndex >= mgmdEB.maxMgmdInstances)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Invalid cbIndex :%u",cbIndex);  
    return ERROR;
  }

   pMgmdCB = (mgmdCB + cbIndex);
  /* Control Block initialization */
   pMgmdCB->family  = family;
   pMgmdCB->cbIndex = cbIndex;

   ptinMgmdGeneralQueryAVLTreeInit(family);

   //ProxyCM initialization
   for(i=0; i<PTIN_MGMD_MAX_SERVICE_ID; ++i)
   {
     pMgmdCB->proxyCM[i].compatibilityMode = PTIN_MGMD_COMPATIBILITY_V3;
   }

   return SUCCESS;
}

/*********************************************************************
* @purpose  Proxy Interface Execution block initializations
*
* @param    None
*
* @returns  SUCCESS - Initialization complete
*           FAILURE - Initilaization failed because of
*                        insufficient system resources
*
* @notes
*
* @end
*********************************************************************/
RC_t ptinMgmdGeneralQueryAVLTreeInit(uchar8 family)
{
  ptin_mgmd_cb_t *pMgmdCB=PTIN_NULLPTR;
 
  if((pMgmdCB=mgmdCBGet(family))==PTIN_NULLPTR)
  {   
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed to get pMgmdCB family:%u",family);   
    return FAILURE;
  }

  pMgmdCB->mgmdPTinQuerierTreeHeap = (ptin_mgmd_avlTreeTables_t *)           ptin_mgmd_malloc(PTIN_MGMD_MAX_SERVICES*sizeof(ptin_mgmd_avlTreeTables_t));  
  pMgmdCB->mgmdPTinQuerierDataHeap = (ptinMgmdQuerierInfoData_t *) ptin_mgmd_malloc(PTIN_MGMD_MAX_SERVICES*sizeof(ptinMgmdQuerierInfoData_t));  

  if ((pMgmdCB->mgmdPTinQuerierTreeHeap == PTIN_NULLPTR) || (pMgmdCB->mgmdPTinQuerierDataHeap == PTIN_NULLPTR))
  {
    PTIN_MGMD_LOG_FATAL(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Error allocating data for snoopPtinProxyInterfaceAVLTreeInit");   
    return FAILURE;
  }

  /* Initialize the storage for all the AVL trees */
  memset(&pMgmdCB->mgmdPTinQuerierAvlTree, 0x00, sizeof(pMgmdCB->mgmdPTinQuerierAvlTree));

  /* AVL Tree creations - snoopAvlTree*/
  ptin_mgmd_avlCreateAvlTree(&(pMgmdCB->mgmdPTinQuerierAvlTree), pMgmdCB->mgmdPTinQuerierTreeHeap, pMgmdCB->mgmdPTinQuerierDataHeap,
                   PTIN_MGMD_MAX_SERVICES, sizeof(ptinMgmdQuerierInfoData_t), 0x10, sizeof(ptinMgmdQuerierInfoDataKey_t));

  return SUCCESS;
}


void ptin_mgmd_memory_report(void)
{
  long  vmrss_kb;
  short found_vmrss = 0;
  FILE  *procfile;
  char  buffer[8192]; 

  /* Get the the current process' status file from the proc filesystem */
  procfile = fopen("/proc/self/status", "r");
  fread(buffer, sizeof(char), sizeof(buffer), procfile);
  fclose(procfile);

  /* Look through proc status contents line by line */
  char *line = strtok(buffer, "\n");
  while (line != NULL && found_vmrss == 0)
  {
    char  *search_result;

    search_result = strstr(line, "VmRSS:");
    if (search_result != NULL)
    {
      sscanf(line, "%*s %lu", &vmrss_kb);
      found_vmrss = 1;
    }
    line = strtok(NULL, "\n");
  }
  
  printf("MGMD Configurations: [Channels=%u Whitelist=%u Services=%u Groups=%u Sources=%u Ports=%u Clients=%u]\n", 
            PTIN_MGMD_MAX_CHANNELS, PTIN_MGMD_MAX_WHITELIST, PTIN_MGMD_MAX_SERVICES, PTIN_MGMD_MAX_GROUPS, PTIN_MGMD_MAX_SOURCES, PTIN_MGMD_MAX_PORTS, PTIN_MGMD_MAX_CLIENTS);
  printf("MGMD Memory Allocated: %lu MB\n",ptin_mgmd_memory_allocation/1024/1024);
  printf("Thread Memory Allocated: %lu MB\n", vmrss_kb/1024);
}


void ptin_mgmd_memory_log_report(void)
{
  long  vmrss_kb;
  short found_vmrss = 0;
  FILE  *procfile;
  char  buffer[8192]; 

  /* Get the the current process' status file from the proc filesystem */
  procfile = fopen("/proc/self/status", "r");
  fread(buffer, sizeof(char), sizeof(buffer), procfile);
  fclose(procfile);

  /* Look through proc status contents line by line */
  char *line = strtok(buffer, "\n");
  while (line != NULL && found_vmrss == 0)
  {
    char  *search_result;

    search_result = strstr(line, "VmRSS:");
    if (search_result != NULL)
    {
      sscanf(line, "%*s %lu", &vmrss_kb);
      found_vmrss = 1;
    }
    line = strtok(NULL, "\n");
  }

  //Memory Allocated for the Statistics Component
  ptin_mgmd_statistics_memory_allocation();
  //Memory Allocated for the Configuration Component
  ptin_mgmd_cfg_memory_allocation();
  //Memory Allocated for the Cnfgr Component
  ptin_mgmd_cnfgr_memory_allocation();
  //Memory Allocated for the Core Component
  ptin_mgmd_core_memory_allocation();

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"MGMD Configurations: [Channels=%u Whitelist=%u Services=%u Groups=%u Sources=%u Ports=%u Clients=%u]", 
                      PTIN_MGMD_MAX_CHANNELS, PTIN_MGMD_MAX_WHITELIST, PTIN_MGMD_MAX_SERVICES, PTIN_MGMD_MAX_GROUPS, PTIN_MGMD_MAX_SOURCES, PTIN_MGMD_MAX_PORTS, PTIN_MGMD_MAX_CLIENTS);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"MGMD Memory Allocated: %lu MB",ptin_mgmd_memory_allocation/1024/1024);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Thread Memory Allocated: %lu MB", vmrss_kb/1024);
}


void ptin_mgmd_process_memory_report(void)
{
  long  vmrss_kb;
  short found_vmrss = 0;
  FILE  *procfile;
  char  buffer[8192]; 

  /* Get the the current process' status file from the proc filesystem */
  procfile = fopen("/proc/self/status", "r");
  fread(buffer, sizeof(char), sizeof(buffer), procfile);
  fclose(procfile);

  /* Look through proc status contents line by line */
  char *line = strtok(buffer, "\n");
  while (line != NULL && found_vmrss == 0)
  {
    char  *search_result;

    search_result = strstr(line, "VmRSS:");
    if (search_result != NULL)
    {
      sscanf(line, "%*s %lu", &vmrss_kb);
      found_vmrss = 1;
    }
    line = strtok(NULL, "\n");
  }

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"FP Currently Allocated Memory : %lu MB", vmrss_kb/1024);
}

/*********************************************************************
* @purpose  Get the Snoop Execution block
*
* @param    none
*
* @returns  pointer to snoop execution block
*
* @notes    none
*
* @end
*********************************************************************/
ptin_mgmd_eb_t *mgmdEBGet(void)
{
  return &mgmdEB;
}


/*********************************************************************
* @purpose  Get the First Mgmd Control block
*
* @param    none
*
* @returns  pMgmdCB pointer to first supported snoop instance
*
* @notes    none
*
* @end
*********************************************************************/
ptin_mgmd_cb_t *mgmdFirsCBGet(void)
{
  return mgmdCB;
}



/*********************************************************************
* @purpose  Get the Snoop Control block if it is supported
*
* @param    family  @b{(input)}   PTIN_MGMD_AF_INET  => IGMP Snooping
*                                 PTIN_MGMD_AF_INET6 => MLD Snooping
*
* @returns  pointer to the snoop control block
* @returns  PTIN_NULLPTR  -  If invalid snoop instance
*
* @notes    none
*
* @end
*********************************************************************/
ptin_mgmd_cb_t *mgmdCBGet(uchar8 family)
{
  ptin_mgmd_cb_t *pMgmdCB = PTIN_NULLPTR;
  uint32     cbIndex; 

  for (cbIndex = 0; cbIndex < mgmdEB.maxMgmdInstances; cbIndex++)
  {
     pMgmdCB = (mgmdCB + cbIndex);
     if (family == pMgmdCB->family)
     {
       break;
     }
  }
  if (cbIndex == mgmdEB.maxMgmdInstances)
  {
    pMgmdCB = PTIN_NULLPTR;
  }
  return pMgmdCB;
}


/*********************************************************************
* @purpose  Get the number of snoop instances supported
*
* @param    none
*
* @returns  number of snoop instances
*
* @comments none
*
* @end
*********************************************************************/
uint32 maxMgmdInstancesGet(void)
{
  return mgmdEB.maxMgmdInstances;
}
