#ifndef _DTL_MONITOR_H_
#define _DTL_MONITOR_H_
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dtl_tap_monitor.h
*
* @purpose Linux tap driver monitor 
*
* @component Device Transformation Layer
*
* @comments This header file is for use _only_ with 
*           a tap driver in place.  This api
*           allows for the registration of a file 
*           descriptor which the monitor task
*           will watch for traffic.  If any traffic is 
*           found it will send it via the dtlPduTransmit command
*
* @author  Neil Horman  
* @end
*
**********************************************************************/
#include "dtlapi.h"

struct tapDtlInfo_s
{
   L7_BOOL discard;
   DTL_CMD_t dtlCmd;
   DTL_CMD_TX_INFO_t dtlCmdInfo;
};

typedef struct tapDtlInfo_s tapDtlInfo;

typedef void (*dtlCmdSend)(int fd,  L7_uint32 intIfNum, L7_netBufHandle handle, tapDtlInfo *info);


L7_RC_t tap_monitor_init(L7_uint32 num_interfaces);

L7_RC_t tap_monitor_cleanup();

L7_RC_t tap_monitor_register(int fd, L7_uint32 intIfNum, dtlCmdSend func);

L7_RC_t tap_monitor_unregister(int fd);

#endif /* _DTL_MONITOR_H_ */
