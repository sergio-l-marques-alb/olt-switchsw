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

#include <unistd.h>


/// Mac Addr used as APS Src MAC and as ERP Node ID
L7_uchar8 srcMacAddr[L7_MAC_ADDR_LEN] = {0};

/// SW Data Base containing ERPS HAL information
ptinHalErps_t tbl_halErps[MAX_PROT_PROT_ERPS];

/// Reference of erps_idx using internal VLAN ID as reference
L7_uint8 erpsIdx_from_controlVidInternal[4096]  = {PROT_ERPS_UNUSEDIDX};
/// Reference of erps_idx using Service VLAN ID as reference
L7_uint8 erpsIdx_from_serviceVid[4096]          = {PROT_ERPS_UNUSEDIDX};

// Task id
L7_uint32 ptin_hal_apsPacketTx_TaskId = L7_ERROR;

/// Global inits
L7_uint8 ptin_hal_initdone = 0x00;

// Task to process Tx messages
void ptin_hal_apsPacketTx_task(void);


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

  // Get base MAC address and use it as Src MAC and Node ID
  if (bspapiMacAddrGet(srcMacAddr) != L7_SUCCESS) {
    PTIN_CRASH();
  }

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

  LOG_INFO(LOG_CTX_ERPS,"ERPS#%d", erps_idx);
  
  LOG_INFO(LOG_CTX_ERPS,"APS Packets Tx         [P0] %d", tbl_halErps[erps_idx].apsPacketsTx[0]);
  LOG_INFO(LOG_CTX_ERPS,"APS Packets Tx         [P1] %d", tbl_halErps[erps_idx].apsPacketsTx[1]);
  LOG_INFO(LOG_CTX_ERPS,"APS Packets Rx Good    [P0] %d", tbl_halErps[erps_idx].apsPacketsRxGood[0]);
  LOG_INFO(LOG_CTX_ERPS,"APS Packets Rx Good    [P1] %d", tbl_halErps[erps_idx].apsPacketsRxGood[1]);
  LOG_INFO(LOG_CTX_ERPS,"APS Packets Rx Dropped [P0] %d", tbl_halErps[erps_idx].apsPacketsRxDropped[0]);
  LOG_INFO(LOG_CTX_ERPS,"APS Packets Rx Dropped [P1] %d", tbl_halErps[erps_idx].apsPacketsRxDropped[1]);

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

  LOG_INFO(LOG_CTX_ERPS,"ERPS#%d", erps_idx);
  
  tbl_halErps[erps_idx].apsPacketsTx[0]         = 0;
  tbl_halErps[erps_idx].apsPacketsTx[1]         = 0;
  tbl_halErps[erps_idx].apsPacketsRxGood[0]     = 0;
  tbl_halErps[erps_idx].apsPacketsRxGood[1]     = 0;
  tbl_halErps[erps_idx].apsPacketsRxDropped[0]  = 0;
  tbl_halErps[erps_idx].apsPacketsRxDropped[1]  = 0;

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

  LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d", erps_idx);
  
  // Print some Debug
  LOG_TRACE(LOG_CTX_ERPS,"used             %d", tbl_halErps[erps_idx].used);
  LOG_TRACE(LOG_CTX_ERPS,"port0.idx %d --> %d", tbl_erps[erps_idx].protParam.port0.idx,   tbl_halErps[erps_idx].port0intfNum);
  LOG_TRACE(LOG_CTX_ERPS,"port1.idx %d --> %d", tbl_erps[erps_idx].protParam.port1.idx,   tbl_halErps[erps_idx].port1intfNum);
  LOG_TRACE(LOG_CTX_ERPS,"vid       %d --> %d", tbl_erps[erps_idx].protParam.controlVid,  tbl_halErps[erps_idx].controlVidInternal);
  LOG_TRACE(LOG_CTX_ERPS,"apsReqStatusRx   0x%x", tbl_halErps[erps_idx].apsReqStatusRx);
  LOG_TRACE(LOG_CTX_ERPS,"apsReqStatusTx   0x%x", tbl_halErps[erps_idx].apsReqStatusTx);
  LOG_TRACE(LOG_CTX_ERPS,"hwSync           %d", tbl_halErps[erps_idx].hwSync);
  LOG_TRACE(LOG_CTX_ERPS,"hwFdbFlush       %d", tbl_halErps[erps_idx].hwFdbFlush);
  
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
        LOG_DEBUG(LOG_CTX_ERPS, "ERPS#%d: VLAN ID %d", erps_idx, vid);
        erpsIdx_from_serviceVid[vid] = erps_idx;
      }

      else {
        erpsIdx_from_serviceVid[vid] = PROT_ERPS_UNUSEDIDX;
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
  LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d", erps_idx);

  if (tbl_halErps[erps_idx].used == L7_TRUE) {
    // Print some Debug
    ptin_hal_erps_entry_print(erps_idx);
    return L7_SUCCESS;
  }
  
  // Convert to internal port
  ptin_intf_port2intIfNum(tbl_erps[erps_idx].protParam.port0.idx, &tbl_halErps[erps_idx].port0intfNum);
  ptin_intf_port2intIfNum(tbl_erps[erps_idx].protParam.port1.idx, &tbl_halErps[erps_idx].port1intfNum);

  // Convert Control VID to internal VLAN ID
  ptin_xlate_ingress_get( tbl_halErps[erps_idx].port0intfNum, tbl_erps[erps_idx].protParam.controlVid, PTIN_XLATE_NOT_DEFINED, &tbl_halErps[erps_idx].controlVidInternal );
  erpsIdx_from_controlVidInternal[tbl_halErps[erps_idx].controlVidInternal] = erps_idx;
  ptin_xlate_ingress_get( tbl_halErps[erps_idx].port1intfNum, tbl_erps[erps_idx].protParam.controlVid, PTIN_XLATE_NOT_DEFINED, &tbl_halErps[erps_idx].controlVidInternal );
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

  // HW Sync init
  tbl_halErps[erps_idx].hwSync     = 0;
  tbl_halErps[erps_idx].hwFdbFlush = 0;  

  tbl_halErps[erps_idx].used = L7_TRUE;

  // Print some Debug
  ptin_hal_erps_entry_print(erps_idx);

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
  LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d", erps_idx);

  if (tbl_halErps[erps_idx].used == L7_FALSE) {
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
  int erps_idx, ret=L7_SUCCESS;

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

  // struct init
  memset(tbl_halErps, 0, MAX_PROT_PROT_ERPS*sizeof(ptinHalErps_t));

  return L7_SUCCESS;
}


/********************************************************************************** 
 *                               APS Packet Processing
 **********************************************************************************/

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
L7_RC_t ptin_hal_erps_sendapsX3(L7_uint8 erps_idx, L7_uint8 req_state, L7_uint8 status)
{
  ptin_aps_packet_send(erps_idx, ((req_state<<4) & 0xF0), status);
  usleep(3000);
  ptin_aps_packet_send(erps_idx, ((req_state<<4) & 0xF0), status);
  usleep(3000);
  ptin_aps_packet_send(erps_idx, ((req_state<<4) & 0xF0), status);

  return L7_SUCCESS;
}


/**
 * Send an APS packet on both ring interfaces
 * 
 * @author joaom (6/11/2013)
 * 
 * @param erps_idx 
 * @param req_state 
 * @param status 
 */
L7_RC_t ptin_hal_erps_sendaps(L7_uint8 erps_idx, L7_uint8 req_state, L7_uint8 status)
{
  L7_uint16 apsTx;

  //LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d", erps_idx);

  apsTx = ((req_state << 12) & 0xF000) | (status & 0x00FF);

  if ((tbl_halErps[erps_idx].apsReqStatusTx != apsTx) && (req_state != RReq_NONE)) {
    //LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: Tx R-APS Request 0x%x(0x%x)",  erps_idx, req_state, status);
    ptin_hal_erps_sendapsX3(erps_idx, req_state, status);
  }

  tbl_halErps[erps_idx].apsReqStatusTx = apsTx;

  return L7_SUCCESS;
}


/**
 * Task for APS continual transmission
 * 
 * @author joaom (6/17/2013)
 */
void ptin_hal_apsPacketTx_task(void)
{
  L7_uint8 erps_idx;
  
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
 * Receives an APS packet on a specified interface and VLAN ID
 * 
 * @author joaom (6/14/2013)
 * 
 * @param erps_idx 
 * @param req_status 
 * @param nodeid 
 * @param rxport 
 */
L7_RC_t ptin_hal_erps_rcvaps(L7_uint8 erps_idx, L7_uint8 *req_state, L7_uint8 *status, L7_uint8 *nodeid, L7_uint32 *rxport)
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



/********************************************************************************** 
 *                               EVC Reconfiguration
 **********************************************************************************/

/**
 * Block or unblock ERP Port and/or Flush FDB
 * 
 * @author joaom (6/25/2013)
 * 
 * @param erps_idx
 * 
 * @return int 
 */
int ptin_hal_erps_hwreconfig(L7_uint8 erps_idx)
{
  L7_uint16 byte, bit;
  L7_uint16 vid, internalVlan;

  if ( (tbl_halErps[erps_idx].hwSync == 1) || (tbl_halErps[erps_idx].hwFdbFlush == 1) ) {
    LOG_DEBUG(LOG_CTX_ERPS,"ERPS#%d: HW Sync in progress...", erps_idx);      

    for (byte=0; byte<(sizeof(tbl_erps[erps_idx].protParam.vid_bmp)); byte++) {
      for (bit=0; bit<8; bit++) {
        if ((tbl_erps[erps_idx].protParam.vid_bmp[byte] >> bit) & 1) {
          vid = (byte*8)+bit;

          // Convert to internal VLAN ID
          if ( L7_SUCCESS == ptin_xlate_ingress_get( tbl_halErps[erps_idx].port0intfNum, vid, PTIN_XLATE_NOT_DEFINED, &internalVlan) ) {

            LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: VLAN %d, intVlan %d", erps_idx, vid, internalVlan);

            if (internalVlan != 0) {
              if (tbl_halErps[erps_idx].hwSync) {
                if (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERPS_PORT_FLUSHING) {
                  switching_root_unblock(tbl_erps[erps_idx].protParam.port0.idx, internalVlan);
                } else {            
                  switching_root_block(tbl_erps[erps_idx].protParam.port0.idx, internalVlan);
                }

                if (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERPS_PORT_FLUSHING) {
                  switching_root_unblock(tbl_erps[erps_idx].protParam.port1.idx, internalVlan);
                } else {            
                  switching_root_block(tbl_erps[erps_idx].protParam.port1.idx, internalVlan);
                }
              }

              if ( (tbl_halErps[erps_idx].hwFdbFlush) || (tbl_halErps[erps_idx].hwSync) ) {
                switching_fdbFlushByVlan(internalVlan);
              }

            } else {
              LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: EVC with VLAN %d does not exist", erps_idx, vid);
            }
          }

        } //if(vid_bmp...)
      } // for(bit...)
    } // for(byte...)

    tbl_halErps[erps_idx].hwSync = 0;
    tbl_halErps[erps_idx].hwFdbFlush = 0;
  }

  return L7_SUCCESS;
}

/**
 * Block or unblock ERP Port and/or Flush FDB
 * 
 * @author joaom (6/25/2013)
 * 
 * @param erps_idx
 * 
 * @return int 
 */
int ptin_hal_erps_forceHwreconfig(L7_uint8 erps_idx)
{
  tbl_halErps[erps_idx].hwSync = 1;
  tbl_halErps[erps_idx].hwFdbFlush = 1;

  return L7_SUCCESS;
}


/**
 * If the VLAN is protected force HE reconfiguration
 * 
 * @author joaom (07/04/2013)
 * 
 * @param erps_idx
 * 
 * @return int 
 */
int ptin_hal_erps_evcIsProtected(L7_uint root_intf, L7_uint16 vlan)
{
  L7_uint8 erps_idx = erpsIdx_from_serviceVid[vlan];

  // Reference of erps_idx using internal vlan as reference
  if (erps_idx != PROT_ERPS_UNUSEDIDX) {

    if ( (root_intf == tbl_erps[erps_idx].protParam.port0.idx) || (root_intf == tbl_erps[erps_idx].protParam.port1.idx) ) {

      LOG_TRACE(LOG_CTX_ERPS, "EVC with root intf %u and Int.VLAN %u is protected by ERPS#%d", root_intf, vlan, erps_idx);

      tbl_halErps[erps_idx].hwSync = 1;
      tbl_halErps[erps_idx].hwFdbFlush = 1;
      return L7_SUCCESS;
    }
  }

  LOG_TRACE(LOG_CTX_ERPS, "EVC with root intf %u and Int.VLAN %u is NOT protected by any ERPS", root_intf, vlan);

  return L7_FAILURE;
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
  return MEP_is_in_LOC(index, 0xffff, &oam);
}

#endif  // PTIN_ENABLE_ERPS

