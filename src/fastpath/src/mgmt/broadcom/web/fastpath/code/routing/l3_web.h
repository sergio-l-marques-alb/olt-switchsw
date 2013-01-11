/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/routing/l3_web.h
 *
 * @purpose Support of the EmWeb code
 *
 * @component unitmgr
 *
 * @comments tba
 *
 * @create 06/28/2001
 *
 * @author betsyt
 * @end
 *
 **********************************************************************/

#ifndef L3_WEB_H__
#define L3_WEB_H__

/* Used in determining the valid Slot.Port */
#define USMWEB_INVALID_VALUE -1

#define USMWEB_LOOPBACK_ITERATOR_STRLEN 96

L7_uint32 usmWebOspfIsAdminModeEnabled(L7_uint32 unit);
L7_BOOL usmWebIsRouterPortAvaialable(L7_uint32 unit);
extern void *webFormSelect_ipv4_intf_secaddr(EwsContext context,
                                             EwsFormSelectOptionP optionp,
                                             void * iterator,
                                             L7_uint32 intIfNum);

/* One structure array per language known. */

typedef struct
{
  L7_uint32 routerId;
  L7_uint32 areaId;
  L7_uint32 type;
  L7_uint32 Lsid;
} usmWeb_LsdbInfo_t;

typedef struct
{
  L7_uint32 areaId;
  L7_uint32 subnet;
  L7_uint32 ipaddr;
  L7_uint32 lsdb_type;
  L7_uint32 advertisement;
} usmWeb_ospfAreaRangeInfo_t;

typedef struct
{
  L7_uint32 nbr_IP;
  L7_uint32 intIfNum;
} usmWeb_ospfNeighborInfo_t;

/*********************************************************************
*
* @purpose Generate the list of configured Router Ports.
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
void *l3_select_rtr_port(EwsContext context,
                         EwsFormSelectOptionP optionp,
                         void * iterator,
                         L7_BOOL (* compIsValidIntf)(L7_uint32,
                                                                   L7_uint32));
#if 0
void *l3_select_V6rtr_port(EwsContext             context, 
                         EwsFormSelectOptionP   optionp,
                           void * iterator,
                           L7_BOOL (* compIsValidIntf)(L7_uint32,
                                                                   L7_uint32));
#endif 
#endif
