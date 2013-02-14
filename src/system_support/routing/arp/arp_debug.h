/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename arp_debug.h
*
* @purpose defines data structures and prototypes for CFG
*
* @component ARP
*
* @comments none
*
* @create 12/12/2006
*
* @author sowjanya
* @end
*
**********************************************************************/
#ifndef ARP_DEBUG_H
#define ARP_DEBUG_H

#include "l7_common.h"
#include "log.h"
#include "arp.h"

extern L7_BOOL arpPacketDebugFlag;

#define ARP_USER_TRACE(__fmt__, __args__... )                              \
  if (arpDebugPacketTraceFlag == L7_TRUE)                          \
{                                                                 \
  LOG_USER_TRACE(L7_IP_MAP_COMPONENT_ID, __fmt__,##__args__);        \
}

/*********************************************************************
 *
 * @purpose Trace ARP packets received
 *
 * @param   intIfNum     @b{(input)} Internal Interface Number
 * @param   *buff        @b{(input)} pointer to packet
 *
 * @returns void
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
void arpDebugPacketRxTrace(t_ARPFrame *pdu);

/*********************************************************************
 *
 * @purpose Trace ARP packets transmitted
 *
 * @param   intIfNum     @b{(input)} Internal Interface Number
 * @param   *buff        @b{(input)} pointer to packet
 *
 * @returns void
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
void arpDebugPacketTxTrace(L7_uint32 intIfNum,t_ARPFrame *pdu);

/*********************************************************************
 * @purpose  Restores arp debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    arpDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t arpDebugRestore(void);

  /*********************************************************************
   * @purpose  Checks if arp debug config data has changed
   *
   * @param    void
   *
   * @returns  L7_TRUE or L7_FALSE
   *
   * @notes    none
   *
   * @end
   *********************************************************************/
L7_BOOL arpDebugHasDataChanged(void);
  /*********************************************************************
   * @purpose  Build default arp config data
   *
   * @param    ver   Software version of Config Data
   *
   * @returns  void
   *
   * @notes
   *
   * @end
   *********************************************************************/
void arpDebugBuildDefaultConfigData(L7_uint32 ver);

  /*********************************************************************
   * @purpose  Apply arp debug config data
   *
   * @param    void
   *
   * @returns  L7_SUCCESS/L7_FAILURE
   *
   * @notes    Called after a default config is built
   *
   * @end
   *********************************************************************/
L7_RC_t arpApplyDebugConfigData(void);
/*********************************************************************
 * @purpose  Register to general debug infrastructure
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void arpDebugRegister(void);
/*********************************************************************
  * @purpose  Save configuration settings for dot3ad trace data
  *
  * @param    void
  *
  * @returns  L7_SUCCESS
  * @returns  L7_FAILURE
  *
  * @notes
  *
  * @end
  *********************************************************************/
L7_RC_t arpDebugPacketTraceFlagSave();

/*********************************************************************
 * @purpose  Copy the trace settings to the debug config file
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void arpDebugCfgUpdate(void);

/*********************************************************************
 * @purpose  Read and apply the debug config
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void arpDebugCfgRead(void);
/*********************************************************************
 * @purpose  Saves arp configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    arpDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t arpDebugSave(void);

  
#endif /* ARP_DEBUG_H */


