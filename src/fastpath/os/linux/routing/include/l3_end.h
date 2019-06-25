/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l3_end.h
*
* @purpose VxWorks End driver for the router interfaces.
*
* @component VxWorks IPM
*
* @comments none
*
* @create 03/13/2001
*
* @author alt
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef L3_END_H
#define L3_END_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>   /* basic system data types */
#include <sys/socket.h>  /* basic socket definitions */
#include <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>   /* inet(3) functions */
#include <sys/ioctl.h>   
#include <net/if.h>      /* for struct ifreq */
#include <net/route.h>   /* for struct rtentry */

#include "l7_common.h"              /* lvl7 common definitions 
                                            and datatypes */
#include "simapi.h"
#include "sysapi.h"
#include "nvstoreapi.h"
#include "registry.h"
#include "cfg_change_api.h"
#include "osapi.h"
#include "nimapi.h"
#include "fdb_api.h"
#include "log.h"
#include "nimapi.h"
#include "dapi.h"                   
#include "statsapi.h"
#include "garpapi.h"
#include "sysnet_api.h"


/*
** board level defines
*/

/* size of msg header proto type version len */
#define L3END_DEVICE_NAME         "eth0"  
#define L3END_DEVICE_NAME_LEN	    5
#define L3END_MBLK_NUM			      256
#define L3END_CL_NUM				      28
#define L3END_CL_BLK_NUM		      256
#define L3END_CLUSTER_SIZE		    2048
#define L3END_MIN_PDU_SIZE        0x40
#define L3END_MAX_FRM			        1500
#define L3END_IF_DESCR_LEN        50

/* We use only 2048-byte MBLOCKs
*/
#define  L3END_CL_TBL_SIZE  1
/*
** port state
*/
#define L3END_PORT_NOT_INIT		    1
#define L3END_PORT_INIT			      2		/* PORT initializing */
#define L3END_PORT_CFG			      3		/* PORT configured */
#define L3END_PORT_DOWN			      4		/* PORT_CFG but Link Down or conf down */
#define L3END_PORT_UP			        5		/* PORT CFG and Link UP */
#define L3END_PORT_BASIC_BRIDGE	  6
#define L3END_PORT_STP			      7
#define L3END_PORT_RIP			      8
#define L3END_PORT_STP_RIP		    9
#define L3END_PORT_OSPF			      10
#define L3END_PORT_STP_OSPF		    11

/* debug message control (can be redefined in individual .c file) */
#define L3END_DEBUG_MSG           0     /* set to 1 to display messages */

/* Structure Table for Interfaces Group MIB Table */
typedef struct l3_if_mib_tbl 
{
  L7_uchar8 ifDescr[L3END_IF_DESCR_LEN];
  L7_uint32 ifAdminStatus;
  L7_uint32 ifOperStatus;
  L7_uint32 ifLastChange; 
  L7_uint32 ifInOctets;
  L7_uint32 ifInUcastPkts;
  L7_uint32 ifInNUcastPkts;
  L7_uint32 ifOutOctets;
  L7_uint32 ifOutUcastPkts;
  L7_uint32 ifOutDiscards;
}L3_IF_MIB_TBL;

typedef struct l3end_device
{
  L3_IF_MIB_TBL mib2Tbl;   /* Interfaces Group MIB Table */
  L7_uint32   port_state;
  L7_uint32   intIfNum;    /* internal interface number */

  L7_uint32   vlan_id;  /* VLAN on which we send all data */

  L7_uint32   ether_mode; /* 802.3 or DIX */

  L7_uint32   instance_num;         /* Instance of this device */

  L7_uchar8   enetAddr[8];

  L7_uint32   ip_addr; /* IP Address of this interface */
  L7_uint32   ip_netmask; /* Netmask of the interface */
  L7_uint32   ip_gateway; /* Default gateway */

  L7_FUNCPTR  addrResFunc; /* Address resolution function for this interface */

/* We support multiple instances of the routing interface END driver. 
** Each driver has its own buffer pool and cluster descriptors.
*/
#ifdef _L7_OS_VXWORKS_
  NET_POOL      NetPool;
  NET_POOL_ID   pNetPool;

  M_CL_CONFIG   MclBlkConfig;
  CL_DESC       ClDescTbl [L3END_CL_TBL_SIZE];
#endif
  void          *devLoadCookie;

/* Some routing interfaces may be for 802.3 port, other for DIX, and others for
** tagged VLAN. Since frames may be constructed differently for these interfaces
** we need to provide a private copy of the function table.
*/
#ifdef _L7_OS_VXWORKS_
  NET_FUNCS FuncTable;

  /* taken from template file */
  CL_POOL_ID  ClPoolId;
#endif
  L7_uint32   flags;
  L7_uint32   offset;

  AcquiredMask    acquiredList; /* Mask of components "acquiring" an interface */

} L3END_END_DEVICE;

/* globals */
extern L3END_END_DEVICE * l3endGlobalDrv;
/* Maximum number of router END drivers.
*/
extern L7_uint32 MaxRouterEndIf;


/* Prototypes */
L7_uint32 ipmIfCheck (L7_uint32 ifIndex);


/* L3_END_H */
#endif 
