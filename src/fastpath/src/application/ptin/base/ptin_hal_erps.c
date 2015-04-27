/**
 * ptin_hal_erps.c
 *  
 * Implements ITU-T G.8032/Y.1344 Ethernet Ring Protection 
 * Switching HAL
 *
 * @author joaom (6/12/2013)
 * (joao-v-mateiro@ptinovacao.pt) 
 *  
 */


#include "ptin_globaldefs.h"
#ifdef PTIN_ENABLE_ERPS

#include "ptin_hal_erps.h"
#include "ptin_prot_erps.h"
#include "ptin_oam_packet.h"
#include "ptin_evc.h"
#include "ptin_xlate_api.h"
#include "ptin_intf.h"

#include "ptin_prot_oam_eth.h"
#include "fdb_api.h"
#include "ptin_utils.h"

#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <linux/sched.h>


/// Mac Addr used as APS Src MAC and as ERP Node ID
L7_uchar8 srcMacAddr[L7_MAC_ADDR_LEN] = {0};

/// SW Data Base containing ERPS HAL information
ptinHalErps_t tbl_halErps[MAX_PROT_PROT_ERPS];

/// Reference of erps_idx using internal VLAN ID as reference
L7_uint8 erpsIdx_from_controlVidInternal[4096]  = {PROT_ERPS_UNUSEDIDX};
/// Reference of erps_idx using Service VLAN ID as reference
L7_uint8 erpsIdx_from_serviceVid[4096]          = {PROT_ERPS_UNUSEDIDX};

/* Semaphore to control concurrent accesses */
void *ptin_hal_erps_sem = L7_NULLPTR;


/* Client indexes pool */
typedef struct vlan_entry_s
{
  struct vlan_entry_s *next;
  struct vlan_entry_s *prev;

  L7_BOOL   used;
  L7_uint16 vid;
} vlan_entry_t;

/* VLANs pool */
struct vlan_entry_s vlan_entry_pool[4096];

static dl_queue_t queue_vlans_used[MAX_PROT_PROT_ERPS];

// Task id
L7_uint32 ptin_hal_apsPacketTx_TaskId = L7_ERROR;

/// Global inits
L7_uint8 ptin_hal_initdone = 0x00;

// Task to process Tx messages
void ptin_hal_apsPacketTx_task(void);

#define PTIN_ERPS_WAKE_UP_SIGNAL 41

/* Static Functions */
static void __ptin_hal_erps_signal_handler(int sig);
static L7_RC_t ptin_hal_erps_queue_vlans_used_init(void);

/********************************************************************************** 
 *                                Initialize
 **********************************************************************************/

/**
 * Initialize ERPS hw abstraction layer
 * 
 * @author joaom (6/17/2013)
 * 
 */
L7_RC_t ptin_hal_erps_init(void)
{
  // struct init
  memset(tbl_halErps, 0, MAX_PROT_PROT_ERPS*sizeof(ptinHalErps_t));

  /* Create semaphore to control concurrent accesses */
  ptin_hal_erps_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_hal_erps_sem == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to create ptin_hal_erps_sem semaphore!");
    return L7_FAILURE;
  }

  // Create task for Tx packets
  ptin_hal_apsPacketTx_TaskId = osapiTaskCreate("ptin_hal_apsPacketTx_task", ptin_hal_apsPacketTx_task, 0, 0,
                                           L7_DEFAULT_STACK_SIZE,
                                           10,
                                           0);

  if (ptin_hal_apsPacketTx_TaskId == L7_ERROR) {
    LOG_FATAL(LOG_CTX_ERPS, "Could not create task ptin_hal_apsPacketTx_task");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_ERPS,"Task ptin_hal_apsPacketTx_task created");

  if (osapiWaitForTaskInit (L7_PTIN_APS_PACKET_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS) {
    LOG_FATAL(LOG_CTX_ERPS,"Unable to initialize ptin_hal_apsPacketTx_task()\n");
    return(L7_FAILURE);
  }
  LOG_TRACE(LOG_CTX_ERPS,"Task ptin_hal_apsPacketTx_task initialized");

  // Get base MAC address and use it as Src MAC and Node ID
  if (bspapiMacAddrGet(srcMacAddr) != L7_SUCCESS) {
    PTIN_CRASH();
  }

  ptin_hal_erps_queue_vlans_used_init();

  return L7_SUCCESS;
}


/**
 * Print APS Counters
 * 
 * @author joaom (7/09/2013)
 * 
 * @param erps_idx 
 */
L7_RC_t ptin_hal_erps_counters(L7_uint8 erps_idx)
{
  L7_uint8 port;

  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  printf("\nERPS#%d APS Statistics\n", erps_idx);
  
  osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);

  for (port=0; port<2; port++) {
    printf("Tx         [P%d] %d\n", port, tbl_halErps[erps_idx].statistics.apsPacketsTx[port]); 
    printf("Fw         [P%d] %d\n", port, tbl_halErps[erps_idx].statistics.apsPacketsFw[port]);
    printf("Tx Event   [P%d] %d\n", port, tbl_halErps[erps_idx].statistics.apsPacketsTxReqEvent[port]);
    printf("Tx FS      [P%d] %d\n", port, tbl_halErps[erps_idx].statistics.apsPacketsTxReqFS[port]);
    printf("Tx SF      [P%d] %d\n", port, tbl_halErps[erps_idx].statistics.apsPacketsTxReqSF[port]);
    printf("Tx MS      [P%d] %d\n", port, tbl_halErps[erps_idx].statistics.apsPacketsTxReqMS[port]);
    printf("Tx NR      [P%d] %d\n", port, tbl_halErps[erps_idx].statistics.apsPacketsTxReqNR[port]);
    printf("Rx         [P%d] %d\n", port, tbl_halErps[erps_idx].statistics.apsPacketsRx[port]);
    printf("Rx Event   [P%d] %d\n", port, tbl_halErps[erps_idx].statistics.apsPacketsRxReqEvent[port]);
    printf("Rx FS      [P%d] %d\n", port, tbl_halErps[erps_idx].statistics.apsPacketsRxReqFS[port]);
    printf("Rx SF      [P%d] %d\n", port, tbl_halErps[erps_idx].statistics.apsPacketsRxReqSF[port]);
    printf("Rx MS      [P%d] %d\n", port, tbl_halErps[erps_idx].statistics.apsPacketsRxReqMS[port]);
    printf("Rx NR      [P%d] %d\n", port, tbl_halErps[erps_idx].statistics.apsPacketsRxReqNR[port]);
    printf("Dropped    [P%d] %d\n", port, tbl_halErps[erps_idx].statistics.apsPacketsRxDropped[port]);
  }

  osapiSemaGive(ptin_hal_erps_sem);

  return L7_SUCCESS;
}


/**
 * Clear APS Counters
 * 
 * @author joaom (7/09/2013)
 * 
 * @param erps_idx 
 */
L7_RC_t ptin_hal_erps_countersClear(L7_uint8 erps_idx)
{
  L7_uint8 port;

  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);

  for (port=0; port<2; port++) {
    tbl_halErps[erps_idx].statistics.apsPacketsTx[port] = 0;
    tbl_halErps[erps_idx].statistics.apsPacketsFw[port] = 0;
    tbl_halErps[erps_idx].statistics.apsPacketsTxReqEvent[port] = 0;
    tbl_halErps[erps_idx].statistics.apsPacketsTxReqFS[port] = 0;
    tbl_halErps[erps_idx].statistics.apsPacketsTxReqSF[port] = 0;
    tbl_halErps[erps_idx].statistics.apsPacketsTxReqMS[port] = 0;
    tbl_halErps[erps_idx].statistics.apsPacketsTxReqNR[port] = 0;
    tbl_halErps[erps_idx].statistics.apsPacketsRx[port] = 0;
    tbl_halErps[erps_idx].statistics.apsPacketsRxReqEvent[port] = 0;
    tbl_halErps[erps_idx].statistics.apsPacketsRxReqFS[port] = 0;
    tbl_halErps[erps_idx].statistics.apsPacketsRxReqSF[port] = 0;
    tbl_halErps[erps_idx].statistics.apsPacketsRxReqMS[port] = 0;
    tbl_halErps[erps_idx].statistics.apsPacketsRxReqNR[port] = 0;
    tbl_halErps[erps_idx].statistics.apsPacketsRxDropped[port] = 0;
  }

  osapiSemaGive(ptin_hal_erps_sem);

  return L7_SUCCESS;
}


/**
 * APS Counters
 * 
 * @author joaom (7/09/2013)
 * 
 * @param erps_idx 
 */
L7_RC_t ptin_hal_erps_counters_tx(L7_uint8 erps_idx, L7_uint8 port, L7_uint8 req, L7_uint8 units)
{
  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }
  if (port > PROT_ERPS_PORT1)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d: port %d not valid", erps_idx, port);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  tbl_halErps[erps_idx].statistics.apsPacketsTx[port] += units;
  
  if (req == RReq_EVENT)
  {
    tbl_halErps[erps_idx].statistics.apsPacketsTxReqEvent[port] += units;
  }
  else if (req == RReq_FS)
  {
    tbl_halErps[erps_idx].statistics.apsPacketsTxReqFS[port] += units;
  }
  else if (req == RReq_SF)
  {
    tbl_halErps[erps_idx].statistics.apsPacketsTxReqSF[port] += units;
  }
  else if (req == RReq_MS)
  {
    tbl_halErps[erps_idx].statistics.apsPacketsTxReqMS[port] += units;
  }
  else if (req == RReq_NR)
  {
    tbl_halErps[erps_idx].statistics.apsPacketsTxReqNR[port] += units;
  }
  else
  {
    LOG_TRACE(LOG_CTX_ERPS,"Unknown Req: 0x%02X, erps_idx %d, port %d", req, erps_idx, port);
  }

  return L7_SUCCESS;
}


/**
 * APS Counters
 * 
 * @author joaom (7/09/2013)
 * 
 * @param erps_idx 
 */
L7_RC_t ptin_hal_erps_counters_fw(L7_uint8 erps_idx, L7_uint8 port, L7_uint8 req)
{
  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }
  if (port > PROT_ERPS_PORT1)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d: port %d not valid", erps_idx, port);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);

  tbl_halErps[erps_idx].statistics.apsPacketsFw[port] += 1;

  osapiSemaGive(ptin_hal_erps_sem);

  return L7_SUCCESS;
}


/**
 * APS Counters
 * 
 * @author joaom (7/09/2013)
 * 
 * @param erps_idx 
 */
L7_RC_t ptin_hal_erps_counters_rx(L7_uint8 erps_idx, L7_uint8 port, L7_uint8 req)
{
  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }
  if (port > PROT_ERPS_PORT1)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d: port %d not valid", erps_idx, port);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  tbl_halErps[erps_idx].statistics.apsPacketsRx[port] += 1;
  
  if (req == RReq_EVENT)
  {
    tbl_halErps[erps_idx].statistics.apsPacketsRxReqEvent[port] += 1; 
  }
  else if (req == RReq_FS)
  {
    tbl_halErps[erps_idx].statistics.apsPacketsRxReqFS[port] += 1; 
  }
  else if (req == RReq_SF)
  {
    tbl_halErps[erps_idx].statistics.apsPacketsRxReqSF[port] += 1; 
  }
  else if (req == RReq_MS)
  {
    tbl_halErps[erps_idx].statistics.apsPacketsRxReqMS[port] += 1;
  }
  else if (req == RReq_NR)
  {
    tbl_halErps[erps_idx].statistics.apsPacketsRxReqNR[port] += 1;
  }
  else
  {
    LOG_TRACE(LOG_CTX_ERPS,"Unknown Req: 0x%02X, erps_idx %d, port %d", req, erps_idx, port);
  }

  return L7_SUCCESS;
}


/**
 * APS Counters
 * 
 * @author joaom (7/09/2013)
 * 
 * @param erps_idx 
 */
L7_RC_t ptin_hal_erps_counters_rxdrop(L7_uint8 erps_idx, L7_uint8 port)
{
  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }
  if (port > PROT_ERPS_PORT1)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d: port %d not valid", erps_idx, port);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  tbl_halErps[erps_idx].statistics.apsPacketsRxDropped[port] += 1;

  return L7_SUCCESS;
}


/**
 * Print ERPS# entry
 * 
 * @author joaom (6/12/2013)
 * 
 * @param erps_idx 
 */
L7_RC_t ptin_hal_erps_entry_print(L7_uint8 erps_idx)
{
  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  printf("ERPS#%d\n", erps_idx);
  
  osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);

  // Print some Debug
  printf("used             %d\n", tbl_halErps[erps_idx].used);
  printf("port0.idx %d --> %d\n", tbl_erps[erps_idx].protParam.port0.idx,   tbl_halErps[erps_idx].port0intfNum);
  printf("port1.idx %d --> %d\n", tbl_erps[erps_idx].protParam.port1.idx,   tbl_halErps[erps_idx].port1intfNum);
  printf("vid       %d --> %d\n", tbl_erps[erps_idx].protParam.controlVid,  tbl_halErps[erps_idx].controlVidInternal);
  printf("apsReqStatusRx   0x%02X\n", tbl_halErps[erps_idx].apsReqStatusRx);
  printf("apsReqStatusTx   0x%02X\n", tbl_halErps[erps_idx].apsReqStatusTx);
  printf("hwSync           %d\n", tbl_halErps[erps_idx].hwSync);
  printf("hwFdbFlush       %d\n", tbl_halErps[erps_idx].hwFdbFlush);

  osapiSemaGive(ptin_hal_erps_sem);

  return L7_SUCCESS;
}


/**
 * Initialize ERPS# hw abstraction layer
 * 
 * @author joaom (7/05/2013)
 * 
 * @param erps_idx 
 */
L7_RC_t ptin_hal_erps_convert_vid_init(L7_uint8 erps_idx)
{
  L7_uint16 byte, bit;
  L7_uint16 vid=0;

  // Convert Services VID to internal VID
  for (byte=0; byte<(sizeof(tbl_erps[erps_idx].protParam.vid_bmp)); byte++) {
    for (bit=0; bit<8; bit++) {

      vid = (byte*8)+bit;

      if ((tbl_erps[erps_idx].protParam.vid_bmp[byte] >> bit) & 1) {       
        LOG_DEBUG(LOG_CTX_ERPS, "ERPS#%d: VLAN ID %d Added", erps_idx, vid);
        erpsIdx_from_serviceVid[vid] = erps_idx;
      }
      else if (erpsIdx_from_serviceVid[vid] == erps_idx)
      {
        erpsIdx_from_serviceVid[vid] = PROT_ERPS_UNUSEDIDX;
        LOG_DEBUG(LOG_CTX_ERPS, "ERPS#%d: VLAN ID %d Removed", erps_idx, vid);
      }
    } // for(bit...)
  } // for(byte...)

  return L7_SUCCESS;
}


/**
 * Initialize ERPS# hw abstraction layer
 * 
 * @author joaom (6/12/2013)
 * 
 * @param erps_idx 
 */
L7_RC_t ptin_hal_erps_entry_init(L7_uint8 erps_idx)
{
  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d", erps_idx);

  osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);

  if (tbl_halErps[erps_idx].used == L7_TRUE) {
    osapiSemaGive(ptin_hal_erps_sem);
    return L7_SUCCESS;
  }
  
  // Convert to internal port
  ptin_intf_port2intIfNum(tbl_erps[erps_idx].protParam.port0.idx, &tbl_halErps[erps_idx].port0intfNum);
  ptin_intf_port2intIfNum(tbl_erps[erps_idx].protParam.port1.idx, &tbl_halErps[erps_idx].port1intfNum);

  // Convert Control VID to internal VLAN ID
  if (ptin_xlate_ingress_get( tbl_halErps[erps_idx].port0intfNum, tbl_erps[erps_idx].protParam.controlVid, PTIN_XLATE_NOT_DEFINED,
                              &tbl_halErps[erps_idx].controlVidInternal, L7_NULLPTR ) != L7_SUCCESS)
  {
    tbl_halErps[erps_idx].controlVidInternal = 0;
  }
  erpsIdx_from_controlVidInternal[tbl_halErps[erps_idx].controlVidInternal] = erps_idx;
  if (ptin_xlate_ingress_get( tbl_halErps[erps_idx].port1intfNum, tbl_erps[erps_idx].protParam.controlVid, PTIN_XLATE_NOT_DEFINED,
                              &tbl_halErps[erps_idx].controlVidInternal, L7_NULLPTR ) != L7_SUCCESS)
  {
    tbl_halErps[erps_idx].controlVidInternal = 0;
  }
  erpsIdx_from_controlVidInternal[tbl_halErps[erps_idx].controlVidInternal] = erps_idx;

  // Convert Services VIDs to internal VLANs ID
  ptin_hal_erps_convert_vid_init(erps_idx);

  // Create HW Rule
#ifdef __APS_AND_CCM_COMMON_FILTER__
  ptin_aps_packet_vlan_trap(tbl_halErps[erps_idx].controlVidInternal, tbl_erps[erps_idx].protParam.megLevel, 1 /* enable */);
#else
  ptin_aps_packet_vlan_trap(tbl_halErps[erps_idx].controlVidInternal, tbl_erps[erps_idx].protParam.ringId, 1 /* enable */);
#endif

  if (!(ptin_hal_initdone & 0x01)) {
    ptin_aps_packet_global_trap(1 /* enable */);
    ptin_hal_initdone |= 0x01;
  }

#ifndef COMMON_APS_CCM_CALLBACKS__ETYPE_REG
  // Init APS packets trap and queues
  ptin_aps_packet_init(erps_idx);
#endif

  tbl_halErps[erps_idx].apsReqTxRemainingCounter = 0;
  tbl_halErps[erps_idx].apsReqStatusTx = 0;
  tbl_halErps[erps_idx].apsReqStatusRx = 0;

  // HW Sync init
  tbl_halErps[erps_idx].hwSync     = 0;
  tbl_halErps[erps_idx].hwFdbFlush = 0;  

  tbl_halErps[erps_idx].used = L7_TRUE;

  osapiSemaGive(ptin_hal_erps_sem);

  ptin_hal_erps_countersClear(erps_idx);

  return L7_SUCCESS;
}


/**
 * DEInitialize ERPS# hw abstraction layer
 * 
 * @author joaom (6/12/2013)
 * 
 * @param erps_idx 
 */
L7_RC_t ptin_hal_erps_entry_deinit(L7_uint8 erps_idx)
{
  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d", erps_idx);

  osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);

  if (tbl_halErps[erps_idx].used == L7_FALSE) {
    osapiSemaGive(ptin_hal_erps_sem);
    return L7_SUCCESS;
  }

  // Delete HW Rule
#ifdef __APS_AND_CCM_COMMON_FILTER__
  ptin_aps_packet_vlan_trap(tbl_halErps[erps_idx].controlVidInternal, tbl_erps[erps_idx].protParam.megLevel, 0 /* disable */);
#else
  ptin_aps_packet_vlan_trap(tbl_halErps[erps_idx].controlVidInternal, tbl_erps[erps_idx].protParam.ringId, 0 /* disable */);
#endif

  // Delete APS packets trap and queues
  ptin_aps_packet_deinit(erps_idx);

  // Delete association
  erpsIdx_from_controlVidInternal[tbl_halErps[erps_idx].controlVidInternal] = PROT_ERPS_UNUSEDIDX;

  // struct init
  memset(&tbl_halErps[erps_idx], 0, sizeof(ptinHalErps_t));

  tbl_halErps[erps_idx].used = L7_FALSE;

  osapiSemaGive(ptin_hal_erps_sem);

  return L7_SUCCESS;
}


/**
 * Clear ERPS hw abstraction layer
 * 
 * @author joaom (7/22/2013)
 * 
 */
L7_RC_t ptin_hal_erps_clear(void)
{
  L7_uint8 erps_idx;
  L7_RC_t  ret = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_ERPS, "");

  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++) {
    if ((ret=ptin_hal_erps_entry_deinit(erps_idx))!=L7_SUCCESS) {
      LOG_ERR(LOG_CTX_ERPS, "ERROR: (%d) while removing ERPS#%d\n\r", ret, erps_idx);
      ret = L7_FAILURE;
    }
  }
  
  return ret;
}


/**
 * DEInitialize ERPS hw abstraction layer
 * 
 * @author joaom (6/17/2013)
 * 
 */
L7_RC_t ptin_hal_erps_deinit(void)
{
  // Delete task
  if ( ptin_hal_apsPacketTx_TaskId != L7_ERROR ) {
    osapiTaskDelete(ptin_hal_apsPacketTx_TaskId);
    ptin_hal_apsPacketTx_TaskId = L7_ERROR;
  }

  /* Create semaphore to control concurrent accesses */
  if (osapiSemaDelete(ptin_hal_erps_sem) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Failed to delete ptin_hal_erps_sem semaphore!");
    return L7_FAILURE;
  }

  // struct init
  memset(tbl_halErps, 0, MAX_PROT_PROT_ERPS*sizeof(ptinHalErps_t));

  return L7_SUCCESS;
}


/********************************************************************************** 
 *                               APS Packet Processing
 **********************************************************************************/

/**
 * Signal handler. This method is registered in __controlblock_handler. 
 */
void __ptin_hal_erps_signal_handler (int sig) 
{
  switch (sig) 
    {
      case PTIN_ERPS_WAKE_UP_SIGNAL:
        //LOG_ERR(LOG_CTX_ERPS, "ERPS: Tx R-APS WAKE UP SIGNAL Handler!");
        break;
      default:
        break;
    }
}


/**
 * Send an APS packet on both ring interfaces
 * 
 * @author joaom (6/11/2013)
 * 
 * @param erps_idx 
 * @param req 
 * @param status 
 */
L7_RC_t ptin_hal_erps_sendaps(L7_uint8 erps_idx, L7_uint8 req, L7_uint8 status)
{
  L7_uint16 apsTx;

    /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  //LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d", erps_idx);

  apsTx = ((req << 12) & 0xF000) | (status & 0x00FF);

  osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);

  tbl_halErps[erps_idx].apsReqStatusTx = apsTx;
  tbl_halErps[erps_idx].apsReqTxRemainingCounter = 3; // Send 3 consecutive R-APS

  osapiSemaGive(ptin_hal_erps_sem);

  osapiTaskSignal(ptin_hal_apsPacketTx_TaskId, PTIN_ERPS_WAKE_UP_SIGNAL);
  //LOG_ERR(LOG_CTX_ERPS, "ERPS#%d: Tx R-APS WAKE UP SIGNAL Sent!", erps_idx);

  return L7_SUCCESS;
}


/**
 * Task for APS continual transmission
 * 
 * @author joaom (6/17/2013)
 */
void ptin_hal_apsPacketTx_task(void)
{
  L7_uint8  erps_idx, req;
  L7_uint16 counter, reqStatus, vid;
  L7_uint8  megLevel;

  struct timespec requiredSleepTime;
  struct timespec remainingSleepTime;

  signal(PTIN_ERPS_WAKE_UP_SIGNAL, __ptin_hal_erps_signal_handler);

  /* Give more priority to this thread */
  nice(-20);

  /* Sleep Thread for 5s */
  requiredSleepTime.tv_sec  = 5;
  requiredSleepTime.tv_nsec = 0;
  
  LOG_INFO(LOG_CTX_ERPS,"PTin APS packet process task started");

  if (osapiTaskInitDone(L7_PTIN_APS_PACKET_TASK_SYNC)!=L7_SUCCESS) {
    LOG_FATAL(LOG_CTX_PTIN_SSM, "Error syncing task");
    PTIN_CRASH();
  }

  LOG_INFO(LOG_CTX_ERPS,"PTin APS packet task ready to process events");

  /* Loop */
  while (1) {

    /* Sleep Thread for 5s */
    if(nanosleep(&requiredSleepTime, &remainingSleepTime) == 0)
    {
      /* Next time Sleep Thread for 5s */
      requiredSleepTime.tv_sec  = 5;
      requiredSleepTime.tv_nsec = 0;
    }
    else
    {
      /* We were interrupted. Next time Sleep Thread for Remaining Sleep Time + 5s */
      memcpy(&requiredSleepTime, &remainingSleepTime, sizeof(requiredSleepTime));
      requiredSleepTime.tv_sec  += 5;
    }

    for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++)
    {
      osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);

      if (!tbl_halErps[erps_idx].used)
      {
        osapiSemaGive(ptin_hal_erps_sem);
        continue;
      }

      counter = tbl_halErps[erps_idx].apsReqTxRemainingCounter;

      req = (tbl_halErps[erps_idx].apsReqStatusTx >> 12) & 0xF;
      reqStatus = tbl_halErps[erps_idx].apsReqStatusTx;
      vid = tbl_erps[erps_idx].protParam.controlVid;
      megLevel = tbl_erps[erps_idx].protParam.megLevel;

      osapiSemaGive(ptin_hal_erps_sem);

      if (req != RReq_NONE)
      {
        ptin_aps_packet_send(erps_idx, vid, megLevel, ((reqStatus >> 8) & 0xFF), (reqStatus & 0xFF));

        osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);
        ptin_hal_erps_counters_tx(erps_idx, PROT_ERPS_PORT0, req, 1);
        ptin_hal_erps_counters_tx(erps_idx, PROT_ERPS_PORT1, req, 1);
        osapiSemaGive(ptin_hal_erps_sem);

        if (counter > 0)
        {
          LOG_NOTICE(LOG_CTX_ERPS, "ERPS#%d: Tx R-APS Req %d (reqStatus 0x%02X)", erps_idx, req, reqStatus);
          osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);
          tbl_halErps[erps_idx].apsReqTxRemainingCounter--;
          osapiSemaGive(ptin_hal_erps_sem);

          /* 3 R-APS should be sent with a period of 3.33ms */
          memcpy(&requiredSleepTime, &remainingSleepTime, sizeof(requiredSleepTime));
          requiredSleepTime.tv_sec  = 0;
          requiredSleepTime.tv_nsec = 3;
        }
      }

      else if (counter > 0)
      {
        osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);
        tbl_halErps[erps_idx].apsReqTxRemainingCounter = 0;
        osapiSemaGive(ptin_hal_erps_sem);
      }
    }
  }
}


/**
 * Receives an APS packet on a specified interface and VLAN ID
 * 
 * @author joaom (6/14/2013)
 * 
 * @param erps_idx 
 * @param req_status 
 * @param nodeid 
 * @param rxport 
 */
L7_RC_t ptin_hal_erps_rcvaps(L7_uint8 erps_idx, L7_uint8 *req, L7_uint8 *status, L7_uint8 *nodeid, L7_uint32 *rxport)
{
  //LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d", erps_idx);
  L7_uint32 rxintport;

  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  if (ptin_aps_packetRx_process(erps_idx, req, status, nodeid, &rxintport)==L7_SUCCESS)
  {
    osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);
    if ( rxintport == tbl_halErps[erps_idx].port0intfNum ) {
      *rxport = PROT_ERPS_PORT0;
    } else {
      *rxport = PROT_ERPS_PORT1;
    }

    ptin_hal_erps_counters_rx(erps_idx, *rxport, *req);

    osapiSemaGive(ptin_hal_erps_sem);

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}



/********************************************************************************** 
 *                               EVC Reconfiguration
 **********************************************************************************/

/**
 * Get a free VLAN entry
 * 
 * @author joaom (11/9/2014)
 *  
 * @return L7_uint16 VID
 */
static L7_uint16 ptin_hal_erps_free_vlan_entry_get(void)
{
  L7_uint16 vid;

  for (vid = 0; vid < 4096; vid++)
  {
    if (vlan_entry_pool[vid].used == L7_FALSE)
    {
      return vid;
    }
  }

  return 4096;
}


/**
 * Initialise VLANs entries and VLAN queues
 * 
 * @author joaom (11/9/2014)
 *  
 * @return L7_RC_t
 */
static L7_RC_t ptin_hal_erps_queue_vlans_used_init(void)
{
  L7_uint16 vid;
  int erps_idx;

  for (vid = 0; vid < 4096; vid++)
  {
    vlan_entry_pool[vid].used = L7_FALSE;
    vlan_entry_pool[vid].vid = 0;
  }

  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++)
  {
    /* Initialize VLANs queue */
    if(NOERR != dl_queue_init(&queue_vlans_used[erps_idx]))
    {
      LOG_ERR(LOG_CTX_ERPS, "Unable to create new dl_queue %d for VLANs in use", erps_idx);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}


/**
 * Debug Function to print VLANs pool
 * 
 * @author joaom (11/9/2014)
 * 
 * @param erps_idx
 * 
 * @return vlan_entry_t* 
 */
struct vlan_entry_s *ptin_hal_erps_queue_vlan_used_find(int erps_idx, L7_uint16 internalVlan)
{
  vlan_entry_t *vlan_entry = NULL;

  /* Get pointer to the first element */
  if(NOERR != dl_queue_get_head(&queue_vlans_used[erps_idx], (dl_queue_elem_t**)&vlan_entry))
  {
    LOG_DEBUG(LOG_CTX_ERPS, "VLANs queue %d is empty", erps_idx);
    return NULL;
  }

  while(vlan_entry != NULL)
  {
    if ( (vlan_entry->used) && (vlan_entry->vid == internalVlan) )
      break;
    
    /* Get next entry */
    vlan_entry = (vlan_entry_t*) dl_queue_get_next(&queue_vlans_used[erps_idx], (dl_queue_elem_t*)vlan_entry);
  }
  return vlan_entry;
}


/**
 * Add VLAN entry to the VLAN queue
 * 
 * @author joaom (11/10/2014)
 *  
 * @return L7_RC_t
 */
static L7_RC_t ptin_hal_erps_queue_vlan_used_add(int erps_idx, L7_uint16 internalVlan)
{
  vlan_entry_t *vlan_entry_find;
  vlan_entry_t *vlan_entry = vlan_entry_pool;
  L7_uint16 free_entry;

  /* Check if the internal vlan exists on the queue */
  vlan_entry_find = ptin_hal_erps_queue_vlan_used_find(erps_idx, internalVlan);
  if (vlan_entry_find != NULL)
  {
    LOG_TRACE(LOG_CTX_ERPS, "VLAN %d already exists on queue %d", internalVlan, erps_idx);
    return L7_SUCCESS;
  }

  /* Add the internal vlan to the queue */
  free_entry = ptin_hal_erps_free_vlan_entry_get();
  vlan_entry[free_entry].used = L7_TRUE;
  vlan_entry[free_entry].vid = internalVlan;
  dl_queue_add(&queue_vlans_used[erps_idx], (dl_queue_elem_t*) &vlan_entry[free_entry]);

  LOG_TRACE(LOG_CTX_ERPS, "VLAN %d added to queue %d", internalVlan, erps_idx);

  return L7_SUCCESS;
}


/**
 * Remove VLAN entry from the VLAN queue
 * 
 * @author joaom (11/10/2014)
 *  
 * @return L7_RC_t
 */
static L7_RC_t ptin_hal_erps_queue_vlan_used_remove(int erps_idx, L7_uint16 internalVlan)
{
  vlan_entry_t *vlan_entry;

  /* Remove the internal vlan to the queue */
  vlan_entry = ptin_hal_erps_queue_vlan_used_find(erps_idx, internalVlan);

  if (vlan_entry == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_ERPS, "VLANs %d not found on queue %d", internalVlan, erps_idx);
    return L7_SUCCESS;
  }

  LOG_TRACE(LOG_CTX_ERPS, "VLANs %d added to queue %d", internalVlan, erps_idx);

  vlan_entry->used = L7_FALSE;
  vlan_entry->vid = 0;
  dl_queue_remove(&queue_vlans_used[erps_idx], (dl_queue_elem_t*) vlan_entry);

  return L7_SUCCESS;
}


/**
 * Debug Function to print VLANs pool
 * 
 * @author joaom (11/9/2014)
 * 
 * @param erps_idx
 * 
 * @return L7_RC_t
 */
L7_RC_t ptin_hal_erps_queue_vlans_used_print(int erps_idx)
{
  vlan_entry_t *vlan_entry;
  L7_uint16 n=0; 

  /* Get pointer to the first element */
  if(NOERR != dl_queue_get_head(&queue_vlans_used[erps_idx], (dl_queue_elem_t**)&vlan_entry))
  {
    LOG_DEBUG(LOG_CTX_ERPS, "VLANs queue %d is empty", erps_idx);
    return L7_FAILURE;
  }

  /* print contents */
  printf("\nERPS#%d: %d Internal VLANs in used:\n", erps_idx, queue_vlans_used[erps_idx].n_elems);
  while(vlan_entry != NULL)
  {
    printf("%4d; " , vlan_entry->vid);
    n++;
    if (n % 16 == 0)
    {
      printf("\n");
    }

    /* Get next entry */
    vlan_entry = (vlan_entry_t*) dl_queue_get_next(&queue_vlans_used[erps_idx], (dl_queue_elem_t*)vlan_entry);
  }

  printf("\n");

  return L7_SUCCESS;
}


/**
 * Debug Function to print Service VLANs pool
 * 
 * @author joaom (11/9/2014)
 * 
 * @param erps_idx
 * 
 * @return L7_RC_t
 */
L7_RC_t ptin_hal_erpsIdx_from_serviceVid_print(void)
{
  L7_uint16 i;

  /* print contents */
  printf("\nServices in used: ERPS#, VLAN\n");
  for (i=0; i<4096; i++)
  {
    if (erpsIdx_from_serviceVid[i] == PROT_ERPS_UNUSEDIDX)
      continue;

    printf("                   %02d, %03d\n", erpsIdx_from_serviceVid[i], i); 
  }

  printf("\n");

  return L7_SUCCESS;
}


/**
 * Set internal VLANs
 * 
 * @author joaom (11/9/2014)
 * 
 * @param erps_idx
 * 
 * @return L7_RC_t
 */
L7_RC_t ptin_hal_erps_internal_vlans_used_sync(L7_uint8 erps_idx)
{
  L7_int initial_evc_id = 0;
  L7_uint16 internalVlan = 0;
  
  LOG_TRACE(LOG_CTX_ERPS, "Adding VLANs to queue %d", erps_idx);

  while (initial_evc_id < PTIN_SYSTEM_N_EVCS)
  {
    initial_evc_id = switching_erps_internalVlan_get(initial_evc_id, 
                                                     tbl_erps[erps_idx].protParam.port0.idx, tbl_erps[erps_idx].protParam.port1.idx, tbl_erps[erps_idx].protParam.vid_bmp, 
                                                     &internalVlan);
    if (internalVlan != 0)
    {
      ptin_hal_erps_queue_vlan_used_add(erps_idx, internalVlan);
    }

    initial_evc_id++;
  }

  return L7_SUCCESS;
}


/**
 * Initialise VLANs entries and VLAN queues
 * 
 * @author joaom (11/9/2014)
 *  
 * @return L7_RC_t
 */
L7_RC_t ptin_hal_erps_queue_vlans_used_clear(L7_uint8 erps_idx)
{
  vlan_entry_t *vlan_entry;

  /* Clear this queue */
  while(dl_queue_remove_tail(&queue_vlans_used[erps_idx], (dl_queue_elem_t**) &vlan_entry) == NOERR);

  return L7_SUCCESS;
}


/**
 * Block or unblock ERP Port
 * 
 * @author joaom (6/25/2013) / Last edit (11/06/2014)
 * 
 * @param erps_idx
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hal_erps_hwSync(L7_uint8 erps_idx)
{
  L7_uint16 internalVlan;
  vlan_entry_t *vlan_entry;

  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);

  if (tbl_halErps[erps_idx].hwSync == 1)
  {
    LOG_DEBUG(LOG_CTX_ERPS,"ERPS#%d: HW Sync in progress...", erps_idx);

    /* Get pointer to the first element */
    if(NOERR != dl_queue_get_head(&queue_vlans_used[erps_idx], (dl_queue_elem_t**)&vlan_entry))
    {
      tbl_halErps[erps_idx].hwSync = 0;
      osapiSemaGive(ptin_hal_erps_sem);
      LOG_DEBUG(LOG_CTX_ERPS, "VLANs queue is empty");
      return L7_FAILURE;
    }

    while (vlan_entry != NULL)
    {
      internalVlan = vlan_entry->vid;

      if (internalVlan != 0)
      {
        if (tbl_halErps[erps_idx].hwSync)
        {
          if (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERPS_PORT_FLUSHING)
          {
            switching_root_unblock(tbl_erps[erps_idx].protParam.port0.idx, internalVlan);
          }
          else
          {
            switching_root_block(tbl_erps[erps_idx].protParam.port0.idx, internalVlan);
          }

          if (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERPS_PORT_FLUSHING)
          {
            switching_root_unblock(tbl_erps[erps_idx].protParam.port1.idx, internalVlan);
          }
          else
          {
            switching_root_block(tbl_erps[erps_idx].protParam.port1.idx, internalVlan);
          }
        }

        /* Get next entry */
        vlan_entry = (vlan_entry_t*) dl_queue_get_next(&queue_vlans_used[erps_idx], (dl_queue_elem_t*)vlan_entry);
      }
    }

    LOG_DEBUG(LOG_CTX_ERPS,"ERPS#%d: HW Sync done!", erps_idx);

    tbl_halErps[erps_idx].hwSync = 0;
  }

  osapiSemaGive(ptin_hal_erps_sem);

  return L7_SUCCESS;
}


/**
 * Perform a FDB Flush
 * 
 * @author joaom (6/25/2013) / Last edit (11/06/2014)
 * 
 * @param erps_idx
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hal_erps_hwFdbFlush(L7_uint8 erps_idx)
{
  L7_uint16 internalVlan;
  vlan_entry_t *vlan_entry;

  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);

  if (tbl_halErps[erps_idx].hwFdbFlush == 1)
  {
    LOG_DEBUG(LOG_CTX_ERPS,"ERPS#%d: HW FdbFlush in progress...", erps_idx);

    /* Get pointer to the first element */
    if(NOERR != dl_queue_get_head(&queue_vlans_used[erps_idx], (dl_queue_elem_t**)&vlan_entry))
    {
      tbl_halErps[erps_idx].hwFdbFlush = 0;
      osapiSemaGive(ptin_hal_erps_sem);
      LOG_DEBUG(LOG_CTX_ERPS, "VLANs queue is empty");
      return L7_FAILURE;
    }

    while (vlan_entry != NULL)
    {
      internalVlan = vlan_entry->vid;

      if (internalVlan != 0)
      {
        if (tbl_halErps[erps_idx].hwFdbFlush)
        {
          //LOG_DEBUG(LOG_CTX_ERPS,"ERPS#%d: Flushing VLAN ID %d", erps_idx, internalVlan);
          fdbFlushByVlan(internalVlan);
        }

        /* Get next entry */
        vlan_entry = (vlan_entry_t*) dl_queue_get_next(&queue_vlans_used[erps_idx], (dl_queue_elem_t*)vlan_entry);
      }
    }

    LOG_DEBUG(LOG_CTX_ERPS,"ERPS#%d: HW Flush done!", erps_idx);

    tbl_halErps[erps_idx].hwFdbFlush = 0;
  }

  osapiSemaGive(ptin_hal_erps_sem);

  return L7_SUCCESS;
}


/**
 * Block or unblock ERP Port and/or Flush FDB
 * 
 * @author joaom (6/25/2013)
 * 
 * @param erps_idx
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hal_erps_forceHwReconfig(L7_uint8 erps_idx)
{
  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  osapiSemaTake(ptin_hal_erps_sem, L7_WAIT_FOREVER);

  tbl_halErps[erps_idx].hwSync = 1;
  tbl_halErps[erps_idx].hwFdbFlush = 1;

  osapiSemaGive(ptin_hal_erps_sem);

  return L7_SUCCESS;
}


/**
 * If the VLAN is protected force HW reconfiguration
 * 
 * @author joaom (07/04/2013)
 * 
 * @param erps_idx
 * 
 * @return L7_BOOL 
 */
L7_BOOL ptin_hal_erps_isPortBlocked(L7_uint root_intf, L7_uint16 vlan, L7_uint16 internalVlan)
{
  L7_uint8 erps_idx;

  /* Validate arguments */
  if (vlan >= 4096)
  {
    LOG_ERR(LOG_CTX_ERPS,"VLAN %u not valid", vlan);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  erps_idx = erpsIdx_from_serviceVid[vlan];

  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  // Reference of erps_idx using internal vlan as reference
  if (erps_idx != PROT_ERPS_UNUSEDIDX)
  {
    osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

    if ( (root_intf == tbl_erps[erps_idx].protParam.port0.idx) || (root_intf == tbl_erps[erps_idx].protParam.port1.idx) )
    {
      if ( (vlan < 1<<12) && (tbl_erps[erps_idx].protParam.vid_bmp[vlan/8] & 1<<(vlan%8)) ) //ERP protected VID
      {
        /* Add the internal vlan to the queue */
        ptin_hal_erps_queue_vlan_used_add(erps_idx, internalVlan);

        if ( (root_intf == tbl_erps[erps_idx].protParam.port0.idx) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERPS_PORT_BLOCKING) )
        {
          osapiSemaGive(ptin_prot_erps_sem);
          LOG_TRACE(LOG_CTX_ERPS, "root intf %u with Int.VLAN %u is blocked by ERPS#%d", root_intf, vlan, erps_idx);      
          return L7_TRUE;
        }
        else if ( (root_intf == tbl_erps[erps_idx].protParam.port1.idx) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERPS_PORT_BLOCKING) )
        {
          osapiSemaGive(ptin_prot_erps_sem);
          LOG_TRACE(LOG_CTX_ERPS, "root intf %u with Int.VLAN %u is blocked by ERPS#%d", root_intf, vlan, erps_idx);      
          return L7_TRUE;
        }

        osapiSemaGive(ptin_prot_erps_sem);

        //ptin_hal_erps_forceHwReconfig(erps_idx);

        LOG_TRACE(LOG_CTX_ERPS, "root intf %u with Int.VLAN %u is NOT blocked by ERPS#%d", root_intf, vlan, erps_idx);      
        return L7_FALSE;
      }
    }

    osapiSemaGive(ptin_prot_erps_sem);

    LOG_TRACE(LOG_CTX_ERPS, "EVC with root intf %u and Int.VLAN %u is NOT protected by ERPS#%d", root_intf, vlan, erps_idx);
    return L7_FALSE;
  }

  LOG_TRACE(LOG_CTX_ERPS, "EVC with root intf %u and Int.VLAN %u is NOT protected by any ERPS", root_intf, vlan);

  return L7_FALSE;
}


/**
 * If the VLAN is protected force HW reconfiguration
 * 
 * @author joaom (07/04/2013)
 * 
 * @param erps_idx
 * 
 * @return L7_BOOL 
 */
L7_BOOL ptin_hal_erps_evcProtectedRemove(L7_uint root_intf, L7_uint16 vlan, L7_uint16 internalVlan)
{
  L7_uint8 erps_idx;

  /* Validate arguments */
  if (vlan >= 4096)
  {
    LOG_ERR(LOG_CTX_ERPS,"VLAN %u not valid", vlan);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  erps_idx = erpsIdx_from_serviceVid[vlan];

  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  // Reference of erps_idx using internal vlan as reference
  if (erps_idx != PROT_ERPS_UNUSEDIDX)
  {
    osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

    if ( (root_intf == tbl_erps[erps_idx].protParam.port0.idx) || (root_intf == tbl_erps[erps_idx].protParam.port1.idx) )
    {
      /* Remove the internal vlan to the queue */
      ptin_hal_erps_queue_vlan_used_remove(erps_idx, internalVlan);

      osapiSemaGive(ptin_prot_erps_sem);

      LOG_TRACE(LOG_CTX_ERPS, "EVC with root intf %u and Int.VLAN %u is protected by ERPS#%d", root_intf, vlan, erps_idx);      

      ptin_hal_erps_forceHwReconfig(erps_idx);

      return L7_TRUE;
    }

    osapiSemaGive(ptin_prot_erps_sem);
  }

  LOG_TRACE(LOG_CTX_ERPS, "EVC with root intf %u and Int.VLAN %u is NOT protected by any ERPS", root_intf, vlan);

  return L7_FALSE;
}


/********************************************************************************** 
 *                               OAM Abstraction
 **********************************************************************************/

/**
 * Get MEP alarm
 * 
 * @author joaom (6/28/2013)
 * 
 * @param slot    Unused
 * @param index   MEP Idx
 * 
 * @return int    SF or No SF
 */
int ptin_hal_erps_rd_alarms(L7_uint8 slot, L7_uint32 index)
{
  return MEP_is_in_LOC(index, 0xffff, &oam) |  MEP_is_in_RDI(index, 0xffff, &oam);
}

#endif  // PTIN_ENABLE_ERPS

