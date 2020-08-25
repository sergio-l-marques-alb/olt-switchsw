/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_l2_ipsg.h
*
* @purpose   
*
* @component hapi
*
* @comments
*
* @create    4/02/07
*
* @author    colinw
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_BROAD_L2_IPSG_H
#define INCLUDE_BROAD_L2_IPSG_H

#include "l7_common.h"
#include "dapi.h"
#include "avl_api.h"

/* The initial release will not support IPv6 addresses for IPSG.
   Setting this to 1 will use more memory because the search keys
   will grow in size. */
#define HAPI_BROAD_IPSG_IPV6_SUPPORTED 1

#if L7_FEAT_IPSG_ON_IFP
/* In theory this value would be the sum of all
   the reserved IFP entries for each chip in the stack. Assume that there can 
   be 256 entries per Fastpath unit. */
#define HAPI_BROAD_IPSG_TBL_SIZE (L7_MAX_UNITS_PER_STACK * 256)
#else
/* In theory this value would be the sum of all
   the VFP entries for each chip in the stack. Assume that there can 
   be 1024 entries per Fastpath unit. */
#define HAPI_BROAD_IPSG_TBL_SIZE (L7_MAX_UNITS_PER_STACK * 1024)
#endif

typedef struct
{
  DAPI_USP_t         usp;

  union 
  {
    L7_uint32        ip4Addr;
#if HAPI_BROAD_IPSG_IPV6_SUPPORTED
    L7_in6_addr_t    ip6Addr;
#endif
  } u;
} ipsgSearchKey_t;

typedef struct
{
  ipsgSearchKey_t    key;
  L7_uint32          policyId;

  void *reserved; /* Reserved for use by the AVL library */

} ipsgEntry_t;

#define HAPI_BROAD_IPSG_TREE_HEAP_SIZE (sizeof (avlTreeTables_t) * \
                                        HAPI_BROAD_IPSG_TBL_SIZE)
#define HAPI_BROAD_IPSG_DATA_HEAP_SIZE (sizeof (ipsgEntry_t) * \
                                        HAPI_BROAD_IPSG_TBL_SIZE)
L7_BOOL hapiBroadIpsgSupported();                                        
/*********************************************************************
 *
 * @purpose Initializes IPSG code
 *
 * @param *dapi_g          system information
 *
 * @returns L7_RC_t result
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadIpsgInit(DAPI_t *dapi_g);

/*********************************************************************
 *
 * @purpose Installs the default policy for IPSG (allows DHCP to pass
 *          and blocks all other IP traffic).
 *
 * @param *dapi_g
 *
 * @returns 
 *
 * @notes L7_RC_t
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadIpsgDefaultPolicyInstall(DAPI_t *dapi_g);

/*********************************************************************
 *
 * @purpose Removes the default policy for IPSG.
 *
 * @param *dapi_g
 *
 * @returns 
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
void hapiBroadIpsgDefaultPolicyRemove(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Enable/disable a port for IP Source Guard
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_IPSG_CONFIG
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfIpsgConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Get the IP Source Guard statistics for a port
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_IPSG_STATS_GET
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfIpsgStatsGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Add an allowable IP Source Guard client for a port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_IPSG_CLIENT_ADD
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfIpsgClientAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Delete an allowable IP Source Guard client for a port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_IPSG_CLIENT_DELETE
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfIpsgClientDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Notifies the IPSG component that a physical port has been
*          added to a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIpsgLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Notifies the IPSG component that a physical port has been
*          removed from a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadIpsgLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Take semaphore to protect IPSG resources
*
* @param
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIpsgSemTake();

/*********************************************************************
*
* @purpose Give semaphore to protect IPSG resources
*
* @param
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIpsgSemGive();


#endif
