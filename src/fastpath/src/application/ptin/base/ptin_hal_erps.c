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

#include "ptin_hal_erps.h"
#include "ptin_prot_erps.h"
#include "ptin_oam_packet.h"
#include "ptin_evc.h"
#include "ptin_xlate_api.h"
#include "ptin_intf.h"

#include <unistd.h>

/*** TO BE DONE ***/
const L7_uchar8 srcMacAddr[L7_MAC_ADDR_LEN] = {0x00,0x06,0x91,0x06,0x7D,0xE0};


/// SW Data Base containing ERPS HAL information
ptinHalErps_t tbl_halErps[MAX_PROT_PROT_ERPS];


/**
 * Initialize ERPS hw abstraction layer
 * 
 * @author joaom (6/12/2013)
 * 
 * @param erps_idx 
 */
void ptin_hal_erps_halinit(L7_uint32 erps_idx)
{
  LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d", erps_idx);
  
  ptin_intf_port2intIfNum(tbl_erps[erps_idx].protParam.port0.idx, &tbl_halErps[erps_idx].port0intfNum);
  ptin_intf_port2intIfNum(tbl_erps[erps_idx].protParam.port1.idx, &tbl_halErps[erps_idx].port1intfNum);

  ptin_xlate_ingress_get( tbl_halErps[erps_idx].port0intfNum, tbl_erps[erps_idx].protParam.controlVid, PTIN_XLATE_NOT_DEFINED, &tbl_halErps[erps_idx].controlVidInternal );

  LOG_TRACE(LOG_CTX_ERPS,"port0.idx %d --> %d", tbl_erps[erps_idx].protParam.port0.idx,   tbl_halErps[erps_idx].port0intfNum);
  LOG_TRACE(LOG_CTX_ERPS,"port1.idx %d --> %d", tbl_erps[erps_idx].protParam.port1.idx,   tbl_halErps[erps_idx].port1intfNum);
  LOG_TRACE(LOG_CTX_ERPS,"vid       %d --> %d", tbl_erps[erps_idx].protParam.controlVid,  tbl_halErps[erps_idx].controlVidInternal);

  ptin_aps_packet_init();
  ptin_aps_packet_vlan_trap(tbl_halErps[erps_idx].controlVidInternal, 1);

  return;
}


/**
 * Send an APS packet on ring interfaces
 * 
 * @author joaom (6/11/2013)
 * 
 * @param slot 
 * @param index 
 * @param apsvid 
 * @param req_state 
 * @param status 
 */
void ptin_hal_erps_sendaps(L7_uint32 erps_idx, L7_uint8 req_state, L7_uint8 status)
{
  aps_frame_t aps_frame;

  memcpy(aps_frame.dmac,              apsMacAddr, L7_ENET_MAC_ADDR_LEN);
  memcpy(aps_frame.smac,              srcMacAddr, L7_ENET_MAC_ADDR_LEN);

  aps_frame.vlan_tag[0]               = 0x81;
  aps_frame.vlan_tag[1]               = 0x00;
  aps_frame.vlan_tag[2]               = 0xE0 | ((tbl_erps[erps_idx].protParam.controlVid>>8) & 0xF);
  aps_frame.vlan_tag[3]               = tbl_erps[erps_idx].protParam.controlVid & 0x0FF;
  aps_frame.etherType                 = L7_ETYPE_CFM;

  aps_frame.aspmsg.mel_version        = 0x21;  // MEG Level 1; Version 1
  aps_frame.aspmsg.opCode             = 40;
  aps_frame.aspmsg.flags              = 0x00;
  aps_frame.aspmsg.tlvOffset          = 0x20;
  aps_frame.aspmsg.req_state_subcode  = req_state;
  aps_frame.aspmsg.status             = status;
  memcpy(aps_frame.aspmsg.nodeid,     srcMacAddr, L7_ENET_MAC_ADDR_LEN);
  memset(aps_frame.aspmsg.reseved2,   0, 24);
  aps_frame.aspmsg.endTlv             = 0x00;

  ptin_aps_packet_send(L7_ALL_INTERFACES,
                       tbl_halErps[erps_idx].controlVidInternal,
                       (L7_uchar8 *) &aps_frame,
                       sizeof(aps_frame_t));

  return;
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
void ptin_hal_erps_sendapsX3(L7_uint32 erps_idx, L7_uint8 req_state, L7_uint8 status)
{
  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    return;
  }

  ptin_hal_erps_sendaps(erps_idx, req_state, status);
  usleep(3000);
  ptin_hal_erps_sendaps(erps_idx, req_state, status);
  usleep(3000);
  ptin_hal_erps_sendaps(erps_idx, req_state, status);

  return;
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
void ptin_hal_erps_rcvaps(L7_uint32 erps_idx, L7_uint8 *req_status, L7_uint8 *nodeid, L7_uint32 *rxport)
{
  LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d", erps_idx);

  L7_uint32 status = L7_SUCCESS;

  if (status == L7_SUCCESS) {
    if (ptin_aps_packetRx_process()!=L7_SUCCESS) {
      LOG_ERR(LOG_CTX_ERPS,"Error processing message");
    }
  } else {
    LOG_ERR(LOG_CTX_ERPS,"Failed packet reception from ptin_aps_packet queue (status = %d)",status);
  }

  return;
}

