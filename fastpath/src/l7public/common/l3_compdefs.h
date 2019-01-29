/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  l3_compdefs.h
*
* @purpose   Common defines, enums and data structures for routing
*
* @component 
*
* @comments  none
*
* @create    09/12/2005
*
* @author    eberge
*
* @end
*             
**********************************************************************/

#ifndef L3_COMPDEFS_H
#define L3_COMPDEFS_H


/**************************************
*
*     RLIM DEFINES
*
***************************************/
#define L7_LOOPBACKID_MIN     0
#define L7_LOOPBACKID_MAX     (L7_MAX_NUM_LOOPBACK_INTF-1)
#define L7_TUNNELID_MIN       0
#define L7_TUNNELID_MAX       (L7_MAX_NUM_TUNNEL_INTF-1)

/* TLV type values in IP MAP checkpoint TLVs */
typedef enum
{
  IPMAP_CKPT_TLV_ROUTE_ADD = 1,
  IPMAP_CKPT_TLV_ROUTE_DEL = 2,
  IPMAP_CKPT_TLV_ROUTE_MOD = 3,
  IPMAP_CKPT_TLV_INTL_VLAN = 4,
  IPMAP_CKPT_TLV_DHCP_ADD = 5,
  IPMAP_CKPT_TLV_DHCP_DEL = 6

} ipMapCkptTlv_t;


#endif /* L3_COMPDEFS_H */
