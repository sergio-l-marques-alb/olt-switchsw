/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_pkt.h
*
* @purpose    
*
* @component  PIM-DM
*
* @comments   none
*
* @create     
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

#ifndef _PIMDM_PKT_H
#define _PIMDM_PKT_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l3_addrdefs.h"
#include "l3_mcast_commdefs.h"

/*******************************************************************************
**                             General Definitions                            **
*******************************************************************************/

/*********************************************************************
*                    PIM-DM Protocol PDU Header
*********************************************************************/
typedef struct _pimdmHdr_s
{
  L7_uchar8   ver_type;
  L7_uchar8   reserved;
  L7_ushort16 chkSum;
}pimdmHdr_t;

/*********************************************************************
*                  PIM-DM State Refresh Message
*********************************************************************/
typedef struct pimdmStRfrMsg_s
{
  L7_inet_addr_t grpAddr;
  L7_inet_addr_t srcAddr;
  L7_inet_addr_t origAddr;
  L7_uint32 metricPref;
  L7_uint32 metric;
  L7_uchar8 maskLen;
  L7_uchar8 stRfrTTL;
  L7_uchar8 reserved;
  L7_uchar8 interval;

} pimdmStRfrMsg_t;

/*******************************************************************************
**                         Extern Declarations                                **
*******************************************************************************/

/* None */

/*******************************************************************************
**                  Function Prototypes Declarations                          **
*******************************************************************************/

/* None */

#endif  /* _PIMDM_PKT_H */
