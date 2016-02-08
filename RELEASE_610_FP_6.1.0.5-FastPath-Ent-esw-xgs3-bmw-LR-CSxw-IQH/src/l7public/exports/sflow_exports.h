/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename sflow_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 03/26/2008
*
* @author akulkarn    
* @end
*
**********************************************************************/

#ifndef __SFLOW_EXPORTS_H_
#define __SFLOW_EXPORTS_H_

/* sFlow defines */
#define L7_SFLOW_MAX_HEADER_SIZE        256
#define L7_SFLOW_MIN_HEADER_SIZE        20
#define L7_SFLOW_DEFAULT_COLLECTOR_PORT FD_SFLOW_RCVR_PORT
#define L7_SFLOW_DEFAULT_SAMPLING_RATE  400
#define L7_SFLOW_MAX_DATAGRAM_SIZE      (L7_MAX_FRAME_SIZE - 100)
#define L7_SFLOW_MIN_DATAGRAM_SIZE      200
#define L7_SFLOW_MAX_RECEIVERS          8 
#define L7_SFLOW_MAX_POLLERS            L7_MAX_PORT_COUNT
#define L7_SFLOW_MAX_SAMPLERS           32 
#define L7_SFLOW_IFINDEX                "ifIndex"
#define L7_SFLOW_MAX_PORT               65535
#define L7_SFLOW_MIN_PORT               1
#define L7_SFLOW_MIN_TIMEOUT            0
#define L7_SFLOW_MAX_TIMEOUT            4294967295U
#define L7_SFLOW_MIN_VERSION            4
#define L7_SFLOW_MAX_VERSION            5
#define L7_SFLOW_MIN_SAMPLING_RATE      1024
#define L7_SFLOW_MAX_SAMPLING_RATE      0x10000
#define L7_SFLOW_MIN_POLLING_INTERVAL   0
#define L7_SFLOW_MAX_POLLING_INTERVAL   86400


/* End of sFlow Defines*/



/******************** conditional Override *****************************/

#ifdef INCLUDE_SFLOW_EXPORTS_OVERRIDES
#include "sflow_exports_overrides.h"
#endif

#endif /* __SFLOW_EXPORTS_H_*/

