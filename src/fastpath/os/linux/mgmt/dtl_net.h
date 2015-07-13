#ifndef _DTL_NET_H_
#define _DTL_NET_H_
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dtl_net.h
*
* @purpose Linux IP Stack Interface
*
* @component Device Transformation Layer
*
* @comments none
*
* @author   John Linville   11/21/2001
* @end
*
**********************************************************************/

#include "l7_common.h"

#define	AF_DTL	(AF_MAX-1)

#define	PF_DTL	AF_DTL

struct sockaddr_dtl {

   L7_uchar8 ifname[IFNAMSIZ];
   L7_uchar8 mac[6];

};

/* PTin added */

/* Generic Defines */
#define DTL0INBANDVID   PTIN_VLAN_INBAND


/* Print some debug */
typedef enum  {
  DTLNET_PTINDEBUG_NONE      = 0x00,
  DTLNET_PTINDEBUG_TX_LEVEL1 = 0x01,
  DTLNET_PTINDEBUG_TX_LEVEL2 = 0x02,
  DTLNET_PTINDEBUG_TX_LEVEL3 = 0x04,
  DTLNET_PTINDEBUG_TX_LEVEL4 = 0x08,
  DTLNET_PTINDEBUG_RX_LEVEL1 = 0x10,
  DTLNET_PTINDEBUG_RX_LEVEL2 = 0x20,
  DTLNET_PTINDEBUG_RX_LEVEL3 = 0x40,
  DTLNET_PTINDEBUG_RX_LEVEL4 = 0x80,
} dtlNetPtinDebug_enum_t;


/* keep info about timestamp */
#define DTLTOD_MAX_INTF 16

typedef enum  {
  DTLTOD_TX_SUCCESS = 1,
  DTLTOD_TX_ERROR   = 2,
} dtlToDStats_enum_t;

typedef struct  {
    L7_uint64  tsSeconds;       /* Seconds field of timestamp     */
    L7_long32  tsNanoseconds;   /* nanoseconds field of timestamp */
    L7_long32  tsHwCounter;     /* HW timestamp counter */
    void      *tsSemaphore;

    struct {
        L7_uint64  ts_counter[DTLTOD_MAX_INTF];
        L7_uint64  ts_error[DTLTOD_MAX_INTF];
    } stats;
} dtlToD_t;


/**
 * Add a vlan on to the dtl0 interface. 
 *  
 * @param vlanId : Vlan ID to which the routing interface will be associated
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note This creates a routing interface named dtl0.VLANID, where VLANID is the value passed through 'vlanId' 
 */
L7_int dtlVlanIfAdd(L7_uint16 vlanId);

/**
 * Update the MAC address for a given interface
 * 
 * @author joaom (7/9/2015)
 * 
 * @param newMac pointer to a mac address
 * @param ifName interface name
 * @param vlanId VLAN ID
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlMacAddrChange(L7_uchar8 *newMac, L7_uchar8 *ifName, L7_uint16 vlanId);


#endif /* _DTL_NET_H_ */
