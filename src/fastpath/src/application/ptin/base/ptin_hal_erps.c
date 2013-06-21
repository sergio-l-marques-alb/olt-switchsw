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

#include <unistd.h>


/// Mac Addr used as APS Src Mac and as ERP Node ID
L7_uchar8 srcMacAddr[L7_MAC_ADDR_LEN] = {0};

/// SW Data Base containing ERPS HAL information
ptinHalErps_t tbl_halErps[MAX_PROT_PROT_ERPS];

/// Reference of erps_idx using internal vlan as reference
L7_uint8 erpsIdx_from_internalVlan[4096];

// Task id
L7_uint32 ptin_hal_apsPacketTx_TaskId = L7_ERROR;


// Task to process Tx messages
void ptin_hal_apsPacketTx_task(void);


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


  // Create task for Tx packets
  ptin_hal_apsPacketTx_TaskId = osapiTaskCreate("ptin_hal_apsPacketTx_task", ptin_hal_apsPacketTx_task, 0, 0,
                                           L7_DEFAULT_STACK_SIZE,
                                           L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                           L7_DEFAULT_TASK_SLICE);

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

  // Get base MAC address and use it as src MAC and Node Id
  if (bspapiMacAddrGet(srcMacAddr) != L7_SUCCESS) {
    PTIN_CRASH();
  }

  return L7_SUCCESS;
}


/**
 * Initialize ERPS# hw abstraction layer
 * 
 * @author joaom (6/12/2013)
 * 
 * @param erps_idx 
 */
L7_RC_t ptin_hal_erps_entry_init(L7_uint32 erps_idx)
{

  LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d", erps_idx);
  
  // Convert to internal port
  ptin_intf_port2intIfNum(tbl_erps[erps_idx].protParam.port0.idx, &tbl_halErps[erps_idx].port0intfNum);
  ptin_intf_port2intIfNum(tbl_erps[erps_idx].protParam.port1.idx, &tbl_halErps[erps_idx].port1intfNum);

  // Convert to internal VLAN ID
  ptin_xlate_ingress_get( tbl_halErps[erps_idx].port0intfNum, tbl_erps[erps_idx].protParam.controlVid, PTIN_XLATE_NOT_DEFINED, &tbl_halErps[erps_idx].controlVidInternal );
  erpsIdx_from_internalVlan[tbl_halErps[erps_idx].controlVidInternal] = erps_idx;

  // Create HW Rule
  ptin_aps_packet_vlan_trap(tbl_halErps[erps_idx].controlVidInternal, tbl_erps[erps_idx].protParam.ringId, 1);

  // Init APS packets trap and queues
  ptin_aps_packet_init();

  // Print some Debug
  LOG_TRACE(LOG_CTX_ERPS,"port0.idx %d --> %d", tbl_erps[erps_idx].protParam.port0.idx,   tbl_halErps[erps_idx].port0intfNum);
  LOG_TRACE(LOG_CTX_ERPS,"port1.idx %d --> %d", tbl_erps[erps_idx].protParam.port1.idx,   tbl_halErps[erps_idx].port1intfNum);
  LOG_TRACE(LOG_CTX_ERPS,"vid       %d --> %d", tbl_erps[erps_idx].protParam.controlVid,  tbl_halErps[erps_idx].controlVidInternal);

  tbl_halErps[erps_idx].used = L7_TRUE;

  return L7_SUCCESS;
}


/**
 * Initialize ERPS hw abstraction layer
 * 
 * @author joaom (6/17/2013)
 * 
 */
L7_RC_t ptin_hal_erps_deinit(void)
{
  L7_uint32 erps_idx;

  // Delete APS packets trap and queues
  ptin_aps_packet_deinit();

  // Delete task
  if ( ptin_hal_apsPacketTx_TaskId != L7_ERROR ) {
    osapiTaskDelete(ptin_hal_apsPacketTx_TaskId);
    ptin_hal_apsPacketTx_TaskId = L7_ERROR;
  }

  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++) {
    tbl_halErps[erps_idx].used = L7_FALSE;
  }

  return L7_SUCCESS;
}


/**
 * Send 3 consecutives APS packets on ring interfaces
 * 
 * @author joaom (6/11/2013)
 * 
 * @param slot 
 * @param index 
 * @param apsvid 
 * @param req_state 
 * @param status 
 */
L7_RC_t ptin_hal_erps_sendapsX3(L7_uint32 erps_idx, L7_uint8 req_state, L7_uint8 status)
{
  ptin_aps_packet_send(erps_idx, ((req_state<<4) & 0xF0), status);
  usleep(3000);
  ptin_aps_packet_send(erps_idx, ((req_state<<4) & 0xF0), status);
  usleep(3000);
  ptin_aps_packet_send(erps_idx, ((req_state<<4) & 0xF0), status);

  return L7_SUCCESS;
}


/**
 * Send an APS packet on ring interfaces
 * 
 * @author joaom (6/11/2013)
 * 
 * @param erps_idx 
 * @param req_state 
 * @param status 
 */
L7_RC_t ptin_hal_erps_sendaps(L7_uint32 erps_idx, L7_uint8 req_state, L7_uint8 status)
{
  L7_uint16 apsTx;

  //LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d", erps_idx);

  apsTx = ((req_state << 12) & 0xF000) | (status & 0x00FF);

  if ((tbl_halErps[erps_idx].apsReqStatusTx != apsTx) && (req_state != RReq_NONE)) {
    LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d: Tx R-APS Request 0x%x(0x%x)",  erps_idx, req_state, status);
    ptin_hal_erps_sendapsX3(erps_idx, req_state, status);
  }

  tbl_halErps[erps_idx].apsReqStatusTx = apsTx;

  return L7_SUCCESS;
}


/**
 * Task for APS constant transmission
 * 
 * @author joaom (6/17/2013)
 */
void ptin_hal_apsPacketTx_task(void)
{
  L7_uint32 erps_idx;
  
  LOG_INFO(LOG_CTX_ERPS,"PTin APS packet process task started");

  if (osapiTaskInitDone(L7_PTIN_APS_PACKET_TASK_SYNC)!=L7_SUCCESS) {
    LOG_FATAL(LOG_CTX_PTIN_SSM, "Error syncing task");
    PTIN_CRASH();
  }

  LOG_INFO(LOG_CTX_ERPS,"PTin APS packet task ready to process events");

  /* Loop */
  while (1) {
    sleep(5);

    for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++) {
      if ( (tbl_halErps[erps_idx].used) && (((tbl_halErps[erps_idx].apsReqStatusTx >> 12) & 0xF) != RReq_NONE) ) {
        ptin_aps_packet_send(erps_idx, ((tbl_halErps[erps_idx].apsReqStatusTx >> 8) & 0xFF), (tbl_halErps[erps_idx].apsReqStatusTx & 0xFF) );
      }
    }
  }
}


/**
 * Receives an APS packet on a specified interface and vlan 
 * 
 * @author joaom (6/14/2013)
 * 
 * @param erps_idx 
 * @param req_status 
 * @param nodeid 
 * @param rxport 
 */
L7_RC_t ptin_hal_erps_rcvaps(L7_uint32 erps_idx, L7_uint8 *req_state, L7_uint8 *status, L7_uint8 *nodeid, L7_uint32 *rxport)
{
  //LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d", erps_idx);
  L7_uint32 rxintport;

  if (ptin_aps_packetRx_process(erps_idx, req_state, status, nodeid, &rxintport)==L7_SUCCESS) {

    if ( rxintport == tbl_halErps[erps_idx].port0intfNum ) {
      *rxport = 0;
    } else {
      *rxport = 1;
    }

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

#endif  // PTIN_ENABLE_ERPS

