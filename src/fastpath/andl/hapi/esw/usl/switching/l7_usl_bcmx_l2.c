/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_bcmx_l2.c
*
* @purpose    USL_BCMX L2 API implementation 
*
* @component  USL
*
* @comments   none
*
* @create     11/21/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "broad_common.h"
#include "l7_usl_bcmx_l2.h"
#include "l7_usl_l2_db.h"
#include "l7_rpc_l2.h"
#include "l7_rpc_l2_mcast.h"
#include "l7_usl_sm.h"
#include "l7_usl_trace.h"

#ifdef L7_MCAST_PACKAGE
#include "l7_rpc_ipmcast.h"
#endif

#ifdef L7_STACKING_PACKAGE
#include "l7_usl_macsync.h"
#endif

#include "bcm/types.h"
#include "bcmx/bcmx_int.h"
#include "bcmx/vlan.h"

extern L7_BOOL uslDatabaseActive;

static void *pUslVlanBcmxSema = L7_NULLPTR;
static void *pUslTrunkBcmxSema   = L7_NULLPTR;
static void *pUslStgBcmxSema     = L7_NULLPTR;
static void *pUslL2McastBcmxSema = L7_NULLPTR;
static void *pUslVlanIpSubnetBcmxSema = L7_NULLPTR;
static void *pUslVlanMacBcmxSema = L7_NULLPTR;
static void *pUslProtectedGroupBcmxSema = L7_NULLPTR;
static void *pUslSystemBcmxSema = L7_NULLPTR;

#define USL_VLAN_BCMX_LOCK_TAKE() \
  { \
     usl_trace_sema(USL_L2_VLAN_DB_ID,"VID",osapiTaskIdSelf(),__LINE__,L7_TRUE);\
     if (osapiSemaTake(pUslVlanBcmxSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
     { \
       USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", pUslVlanBcmxSema); \
     } \
  }

#define USL_VLAN_BCMX_LOCK_GIVE() \
  { \
     usl_trace_sema(USL_L2_VLAN_DB_ID,"VID",osapiTaskIdSelf(),__LINE__,L7_FALSE);\
     if (osapiSemaGive(pUslVlanBcmxSema) != L7_SUCCESS) \
     { \
       USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", pUslVlanBcmxSema); \
     } \
  }

#define USL_TRUNK_BCMX_LOCK_TAKE() \
  { \
    usl_trace_sema(USL_L2_TRUNK_DB_ID,"LAG",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
    if (osapiSemaTake(pUslTrunkBcmxSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
    { \
      USL_LOG_MSG(USL_BCM_E_LOG,"Sema take failed, semId %x\n", pUslTrunkBcmxSema); \
    } \
  }

#define USL_TRUNK_BCMX_LOCK_GIVE() \
  { \
    usl_trace_sema(USL_L2_TRUNK_DB_ID,"LAG",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
    if (osapiSemaGive(pUslTrunkBcmxSema) != L7_SUCCESS) \
    { \
      USL_LOG_MSG(USL_BCM_E_LOG,"Sema give failed, semId %x\n", pUslTrunkBcmxSema); \
    } \
  }

#define USL_STG_BCMX_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L2_STG_DB_ID,"STG",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslStgBcmxSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema take failed, semId %x\n", pUslStgBcmxSema); \
  } \
}

#define USL_STG_BCMX_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L2_STG_DB_ID,"STG",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslStgBcmxSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema give failed, semId %x\n", pUslStgBcmxSema); \
  } \
}

#define USL_L2MC_BCMX_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L2_MCAST_DB_ID,"L2MC",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslL2McastBcmxSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema take failed, semId %x\n", pUslL2McastBcmxSema); \
  } \
}

#define USL_L2MC_BCMX_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L2_MCAST_DB_ID,"L2MC",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslL2McastBcmxSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema give failed, semId %x\n", pUslL2McastBcmxSema); \
  } \
}

#define USL_VLAN_IPSUBNET_BCMX_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L2_VLAN_IPSUBNET_DB_ID,"IPVLAN",osapiTaskIdSelf(), \
                 __LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslVlanIpSubnetBcmxSema, L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema take failed, semId %x\n", pUslVlanIpSubnetBcmxSema); \
  } \
}

#define USL_VLAN_IPSUBNET_BCMX_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L2_VLAN_IPSUBNET_DB_ID,"IPVLAN",osapiTaskIdSelf(), \
                 __LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslVlanIpSubnetBcmxSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema give failed, semId %x\n", pUslVlanIpSubnetBcmxSema); \
  } \
}

#define USL_VLAN_MAC_BCMX_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L2_VLAN_MAC_DB_ID,"MACVLAN",osapiTaskIdSelf(), \
                 __LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslVlanMacBcmxSema, L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema take failed, semId %x\n", pUslVlanMacBcmxSema); \
  } \
}

#define USL_VLAN_MAC_BCMX_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L2_VLAN_MAC_DB_ID,"MACVLAN",osapiTaskIdSelf(), \
                 __LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslVlanMacBcmxSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema give failed, semId %x\n", pUslVlanMacBcmxSema); \
  } \
}

#define USL_PROTECTED_GROUP_BCMX_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_L2_PROTECTED_GRP_DB_ID,"PG",osapiTaskIdSelf(), \
                 __LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslProtectedGroupBcmxSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema take failed, semId %x\n", pUslProtectedGroupBcmxSema); \
  } \
}

#define USL_PROTECTED_GROUP_BCMX_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_L2_PROTECTED_GRP_DB_ID,"PG",osapiTaskIdSelf(), \
                __LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslProtectedGroupBcmxSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema give failed, semId %x\n", pUslProtectedGroupBcmxSema); \
  } \
}


#define USL_SYSTEM_BCMX_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_SYSTEM_DB_ID,"SDB",osapiTaskIdSelf(), \
                __LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslSystemBcmxSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG, \
                "Sema take failed, semId %x\n", \
                pUslSystemBcmxSema); \
  } \
}

#define USL_SYSTEM_BCMX_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_SYSTEM_DB_ID,"SDB",osapiTaskIdSelf(), \
                __LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslSystemBcmxSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG, \
                "Sema give failed, semId %x\n", \
                pUslSystemBcmxSema); \
  } \
}

/*********************************************************************
* @purpose  Initialize usl vlan bcmx
*
* @param    none
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_bcmx_init(void)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    /* Create the vlan bcmx semaphore */
    pUslVlanBcmxSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
    if (pUslVlanBcmxSema == L7_NULLPTR)  
    {
      break;
    }

    /* Initialize vlan rpc */
    if (l7_vlan_rpc_init() != L7_SUCCESS)
    {
      break;
    }

    rc = L7_SUCCESS;

  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Initialize usl trunk bcmx
*
* @param    none
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_trunk_bcmx_init(void)
{
  L7_RC_t rc = L7_FAILURE;
  
  do
  {
    pUslTrunkBcmxSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
    if ( pUslTrunkBcmxSema == L7_NULLPTR ) break;


    if (l7_trunk_rpc_init() != L7_SUCCESS)
    {
      break;    
    }

    rc = L7_SUCCESS;
  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Initialize usl stg bcmx
*
* @param    none
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_stg_bcmx_init(void)
{
  L7_RC_t rc = L7_FAILURE;
  
  do
  {
    pUslStgBcmxSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
    if ( pUslStgBcmxSema == L7_NULLPTR ) break;


    if (l7_stg_rpc_init() != L7_SUCCESS)
    {
      break;    
    }

    rc = L7_SUCCESS;
  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Initialize usl L2mc bcmx
*
* @param    none
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_l2mc_bcmx_init(void)
{
  L7_RC_t rc = L7_FAILURE;
  
  do
  {

    pUslL2McastBcmxSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
    if (pUslL2McastBcmxSema == L7_NULLPTR)  
    {
      break;
    }

    if (l7_l2_mcast_rpc_init() != L7_SUCCESS)
    {
      break;
    }

    rc = L7_SUCCESS;
  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Initialize usl IpSubnet Vlan bcmx
*
* @param    none
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_ipsubnet_bcmx_init(void)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    pUslVlanIpSubnetBcmxSema = osapiSemaMCreate(OSAPI_SEM_Q_PRIORITY);
    if (pUslVlanIpSubnetBcmxSema == L7_NULLPTR)
      break;

    if (l7_vlan_ipsubnet_rpc_init() != L7_SUCCESS)
      break;

    rc = L7_SUCCESS;
  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Initialize usl mac Vlan bcmx
*
* @param    none
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_mac_bcmx_init(void)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    pUslVlanMacBcmxSema = osapiSemaMCreate(OSAPI_SEM_Q_PRIORITY);
    if (pUslVlanMacBcmxSema == L7_NULLPTR)
      break;

    if (l7_vlan_mac_rpc_init() != L7_SUCCESS)
      break;

    rc = L7_SUCCESS;
  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Initialize usl protected group bcmx
*
* @param    none
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_protected_group_bcmx_init(void)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    pUslProtectedGroupBcmxSema = osapiSemaMCreate(OSAPI_SEM_Q_PRIORITY);
    if (pUslProtectedGroupBcmxSema == L7_NULLPTR)
      break;

    if (l7_protected_group_rpc_init() != L7_SUCCESS)
      break;

    rc = L7_SUCCESS;

  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Initialize usl system bcmx
*
* @param    none
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_system_bcmx_init(void)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    pUslSystemBcmxSema = osapiSemaMCreate(OSAPI_SEM_Q_PRIORITY);
    if (pUslSystemBcmxSema == L7_NULLPTR)
      break;

    l7_dvlan_rpc_init();
    l7_system_rpc_init();

    rc = L7_SUCCESS;

  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Initialize L2 bcmx
*
* @param    none
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_l2_bcmx_init(void)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    if (usl_vlan_bcmx_init() != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Failed to initialize vlan bcmx\n");
      break;    
    }

    if (usl_trunk_bcmx_init() != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Failed to initialize trunk bcmx\n");
      break;    
    }

    if (usl_stg_bcmx_init() != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Failed to initialize stg bcmx\n");
      break;    
    }

    if (usl_l2mc_bcmx_init() != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Failed to initialize l2mc bcmx\n");
      break;    
    }

    if (usl_vlan_ipsubnet_bcmx_init() != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Failed to initialize vlan ipsubnet bcmx\n");
      break;    
    }

    if (usl_vlan_mac_bcmx_init() != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Failed to initialize vlan mac bcmx\n");
      break;    
    }

    if (usl_protected_group_bcmx_init() != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Failed to initialize protected group bcmx\n");
      break;    
    }

    if (usl_system_bcmx_init() != L7_SUCCESS)
    {
      USL_LOG_MSG(USL_BCM_E_LOG,
                  "Failed to initialize system bcmx\n");
      break;    
    }

    
    rc = L7_SUCCESS;
  }
  while (0);
  
  return rc;
}

/*********************************************************************
* @purpose  Suspend L2 Bcmx calls
*
* @param    void
*
* @returns  void
*
* @end
*********************************************************************/
void usl_l2_bcmx_suspend()
{
  USL_VLAN_BCMX_LOCK_TAKE();
  USL_TRUNK_BCMX_LOCK_TAKE();
  USL_STG_BCMX_LOCK_TAKE();
  USL_L2MC_BCMX_LOCK_TAKE();
  USL_VLAN_IPSUBNET_BCMX_LOCK_TAKE();
  USL_VLAN_MAC_BCMX_LOCK_TAKE();
  USL_PROTECTED_GROUP_BCMX_LOCK_TAKE();
  USL_SYSTEM_BCMX_LOCK_TAKE();
}

/*********************************************************************
* @purpose  Suspend L2 Bcmx calls
*
* @param    void
*
* @returns  void
*
* @end
*********************************************************************/
void usl_l2_bcmx_resume()
{
  USL_PROTECTED_GROUP_BCMX_LOCK_GIVE();
  USL_VLAN_MAC_BCMX_LOCK_GIVE();    
  USL_VLAN_IPSUBNET_BCMX_LOCK_GIVE();
  USL_L2MC_BCMX_LOCK_GIVE();
  USL_STG_BCMX_LOCK_GIVE();
  USL_TRUNK_BCMX_LOCK_GIVE();
  USL_VLAN_BCMX_LOCK_GIVE();
  USL_SYSTEM_BCMX_LOCK_GIVE();
}

/* Start of vlan */
/*********************************************************************
* @purpose  Create a VLAN in the HW's table
*
* @param    vid          @{(input)} the VLAN ID to be created
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_vlan_create(bcm_vlan_t vid)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  
  USL_VLAN_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_VLAN_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_vlan_create(vid, 0, L7_NULL);
    }

    /* Do not update the USL Db if hw add failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_vlan_update(USL_CURRENT_DB, vid, USL_CMD_ADD);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }


  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error */
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_EXISTS))
  {
    USL_LOG_MSG(USL_BCM_E_LOG, 
                "USL: Failed to create vlan %d, hwRv %d dbRv %d\n",
                vid, hwRv, dbRv);
  }


  USL_VLAN_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Remove the VLAN from the HW's table
*
* @param    vid          @{(input)} the VLAN ID to be removed
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_bcmx_vlan_destroy(bcm_vlan_t vid)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* protect the tables while manipulating the data */
  USL_VLAN_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_VLAN_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_vlan_destroy(vid, 0, L7_NULL);
    }

    /* Update the USL Db even if hw deletion failed*/
    dbRv = usl_db_vlan_update(USL_CURRENT_DB, vid, USL_CMD_REMOVE);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }

  } while (0) ;

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_BCM_E_LOG, 
                "USL: Failed to delete vlan %d, hwRv %d dbRv %d\n",
                vid, hwRv, dbRv);
  }


  USL_VLAN_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set the mcast flood mode for a vlan
*
* @param    vid          @{(input)} 
* @param    floodMode    @{(input)}
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_bcmx_vlan_mcast_flood_set(bcm_vlan_t vid, 
                                  bcm_vlan_mcast_flood_t mcastFloodMode)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* protect the tables while manipulating the data */
  USL_VLAN_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_VLAN_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_vlan_mcast_flood_set(vid, mcastFloodMode, 0, L7_NULL);
    }

    /* Update the USL Db even if hw deletion failed*/
    dbRv = usl_db_vlan_mcast_flood_set(USL_CURRENT_DB, vid, mcastFloodMode);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }

  } while (0) ;

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_BCM_E_LOG, 
                "USL: Failed to set mcast flood mode: vlan %d mode %d hwRv %d dbRv %d\n",
                vid, mcastFloodMode, hwRv, dbRv);
  }


  USL_VLAN_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Update the control flags for a vlan
*
* @param    vid          @{(input)} 
* @param    flags        @{(input)} 
* @param    cmd          @{(input)} 
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_bcmx_vlan_control_flag_update(bcm_vlan_t vid, L7_uint32 flags, 
                                      USL_CMD_t cmd)
                                      
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* protect the tables while manipulating the data */
  USL_VLAN_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_VLAN_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_vlan_control_flag_update(vid, flags, cmd, 0, L7_NULL);
    }

    /* Update the USL Db even if hw deletion failed*/
    dbRv = usl_db_vlan_control_flag_update(USL_CURRENT_DB, vid, flags, cmd);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }

  } while (0) ;

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_BCM_E_LOG, 
                "USL: Failed to update control flags: vlan %d flags %d hwRv %d dbRv %d\n",
                vid, flags, hwRv, dbRv);
  }


  USL_VLAN_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Update the forwarding mode for a vlan
*
* @param    vid               @{(input)} 
* @param    forwarding_mode   @{(input)} 
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_bcmx_vlan_control_fwd_mode_set(bcm_vlan_t vid, bcm_vlan_forward_t forwarding_mode)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* protect the tables while manipulating the data */
  USL_VLAN_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_VLAN_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_vlan_control_fwd_mode_set(vid, forwarding_mode);
      if (L7_BCMX_OK(hwRv) != L7_TRUE)
      {
        break;
      }
    }

    dbRv = usl_db_vlan_control_fwd_mode_set(USL_CURRENT_DB, vid, forwarding_mode);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }
  } while (0) ;

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_BCM_E_LOG, 
                "USL: Failed to set VLAN forwarding mode: vlan %d mode %d hwRv %d dbRv %d\n",
                vid, forwarding_mode, hwRv, dbRv);
  }


  USL_VLAN_BCMX_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Create/Delete a list of vlans in the HW
*
* @param    bulkCmd          @{(input)} L7_TRUE: Create the Vlans
*                                       L7_FALSE: Delete the Vlans
* @param    vlanMask         @{(input)} Pointer to mask of vlan-ids to be created
* @param    numVlans         @{(input)} number of vlans to be created
* @param    vlanMaskFailure  @{(output)} Pointer to mask populated by usl with 
*                                       failed vlan-ids
* @param    vlanFailureCount @{(output)} Pointer to integer to pass the number 
*                                        of vlans that could not be created
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
 int usl_bcmx_vlan_bulk_configure(L7_BOOL bulkCmd,
                                  L7_VLAN_MASK_t *vlanMask, 
                                  L7_ushort16 numVlans,
                                  L7_VLAN_MASK_t *vlanMaskFailure,
                                  L7_uint32 *vlanFailureCount)
{
  int              rv, tmpDbRv;
  int              hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  USL_CMD_t vlanDbUpdateCmd;
  L7_uint32        vidx, pos, countVlans, vid;
  L7_uchar8        tmpVlanMask, tmpVlanFailureMask;

   /* protect the tables while manipulating the data */
   USL_VLAN_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_VLAN_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_vlan_bulk_configure(bulkCmd, vlanMask, numVlans, 
                                               vlanMaskFailure, vlanFailureCount);
    }
    else /* Populate the out parameters */
    {
      memset(vlanMaskFailure, 0, sizeof(*vlanMaskFailure));
      *vlanFailureCount = 0;
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      /* Update the USL Db even if hw deletion failed */
      if (bulkCmd == L7_TRUE)
      {
        break;          
      }
    }
 
    vlanDbUpdateCmd = (bulkCmd == L7_TRUE) ? USL_CMD_ADD : USL_CMD_REMOVE;
    countVlans = numVlans;

    /* Walk through the vlan masks and update the USL db for 
       vlans successfully created in the hardware. */
    for (vidx = 0; vidx < L7_VLAN_INDICES; vidx++) 
    {
	  pos = 0;
	  tmpVlanMask = vlanMask->value[vidx];
	  tmpVlanFailureMask = vlanMaskFailure->value[vidx];

	  while (tmpVlanMask) 
	  {
	    pos++;
        
	    if ((tmpVlanMask & 0x1) &&
            (!(tmpVlanFailureMask & 0x1)) /* If vlan was created in the hardware */
           )
	    {
	      vid = (vidx*8) + pos;

          tmpDbRv = usl_db_vlan_update(USL_CURRENT_DB, vid, vlanDbUpdateCmd);
          if (dbRv < tmpDbRv)
            dbRv = tmpDbRv;

          countVlans--;
        }

        tmpVlanMask >>= 1;
        tmpVlanFailureMask >>= 1;
      } /* End while */

	  if (countVlans == 0) 
	  {
	    break;
      }
    } /* End for */

  } while ( 0 );

  rv = min(hwRv, dbRv);

  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_BCM_E_LOG, 
                "Failed to perform vlan bulk operation %d, hwRv %d dbRv %d\n",
                bulkCmd, hwRv, dbRv);
  }

  USL_VLAN_BCMX_LOCK_GIVE();

  return rv;
}

/* End of Vlan */

/* Start of trunk */

/*********************************************************************
* @purpose  Set hashing mechanism for existing trunk.
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    psc          @{(input)} The hashing mechanism.
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_psc_set (L7_uint32 appId, bcm_trunk_t tid, int psc)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  
  /* protect the tables while manipulating the data */
  USL_TRUNK_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_TRUNK_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_trunk_psc_set(appId, tid, psc, 0, L7_NULL);
    }

    /* Do not update the USL Db if hw operation failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_trunk_hash_set(USL_CURRENT_DB, appId, tid, psc);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }
    
  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_BCM_E_LOG,
                "USL: Failed to update hash mode for trunk-id %d, hwRv %d dbRv %d\n", 
                tid, hwRv, dbRv);
  }

  /* release the table before calling the broadcom driver */
  USL_TRUNK_BCMX_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Create a TRUNK in the driver
*
* @param    appId        @{(input)}  Unique application identifier for
*                                    trunk
* @param    tid          @{(output)} The Trunk ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_create(L7_uint32 appId, bcm_trunk_t * tid)
{
  int       rv;
  int       hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  L7_uint32 flags  = 0;

  /* protect the tables while manipulating the data */
  USL_TRUNK_BCMX_LOCK_TAKE();

  do
  {
    hwRv = usl_trunk_hw_id_allocate(appId, tid);
    if (hwRv != BCM_E_NONE)
    {
      break;    
    }
    flags = USL_BCM_TRUNK_CREATE_WITH_ID;

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_TRUNK_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_trunk_create(appId, tid, flags, 0, L7_NULL);
    }

    /* Do not update the USL Db if hw operation failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      /* Free the allocated-id */
      if (usl_trunk_hw_id_free(*tid) != BCM_E_NONE)
      {
        USL_LOG_MSG(USL_BCM_E_LOG, "Failed to free tid %d\n", tid);
      }

      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_create_trunk(USL_CURRENT_DB, appId, *tid);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_BCM_E_LOG,
                "USL: Failed to add trunk (%x/%x), hwRv %d dbRv %d\n", 
                appId, *tid, hwRv, dbRv);
  }

  USL_TRUNK_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a trunk
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_destroy(L7_uint32 appId, bcm_trunk_t tid)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* protect the tables while manipulating the data */
  USL_TRUNK_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_TRUNK_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_trunk_destroy(appId, tid, 0, L7_NULL);
    }

    /* Update the USL Db even if hw delete failed */
    dbRv = usl_db_delete_trunk(USL_CURRENT_DB, appId, tid);

    if (usl_trunk_hw_id_free(tid) != BCM_E_NONE)
    {
      USL_LOG_MSG(USL_BCM_E_LOG, "Failed to free tid %d\n", tid);
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log a message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_BCM_E_LOG,  
                "USL: error removing trunk %x, hwRv %d dbRv %d\n", 
                tid, hwRv, dbRv);
  }


  /* release the table before calling the broadcom driver */
  USL_TRUNK_BCMX_LOCK_GIVE();

  return rv;
}


/*********************************************************************
* @purpose  Set the characteristics of a TRUNK in the HW's trunk table
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
* @param    add_info     @{(input)} The ports, algorithm, and flags for the trunk
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_set(L7_uint32 appId, bcm_trunk_t tid, 
                       bcm_trunk_add_info_t * add_info)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* protect the tables while manipulating the data */
  USL_TRUNK_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_TRUNK_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_trunk_set(appId, tid, add_info, 0, L7_NULL);
    }

    /* Do not update the USL db is hw operation failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_update_trunk_info(USL_CURRENT_DB, appId, tid, add_info);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_BCM_E_LOG,
                "USL: error setting trunk %x info, hwRv %d dbRv %d\n",
                tid, hwRv, dbRv);

  }


  USL_TRUNK_BCMX_LOCK_GIVE();

  return(rv);
}

/*********************************************************************
* @purpose  Associate a Spanning Tree group with a VLAN
*
* @param    appId        @{(input)} Unique application identifier for 
*                                   this stg
* @param    stg          @{(input)} Pointer the Spanning Tree Group
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_stg_create(L7_uint32 appId, bcm_stg_t *pStg)
{
  int         rv;
  int         hwRv = BCM_E_NONE, dbRv = BCM_E_NONE; 
  L7_int32    stgFlags = 0;

  /* protect the tables while manipulating the data */
  USL_STG_BCMX_LOCK_TAKE();

  do
  {
    hwRv = usl_stg_hw_id_allocate(appId, pStg);
    if (hwRv != BCM_E_NONE)
    {
      break;    
    }
    stgFlags = USL_BCM_STG_CREATE_WITH_ID;
    
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_STG_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_stg_create(appId, pStg, stgFlags, 0,L7_NULL);
    }

    /* Do not update the USL db if hw operation failed */ 
	if (L7_BCMX_OK(hwRv) != L7_TRUE )
    {
      /* Free the allocated-id */
      if (usl_stg_hw_id_free(*pStg) != BCM_E_NONE)
      {
        USL_LOG_MSG(USL_BCM_E_LOG, "Failed to free stgId %d\n", *pStg);
      }
      break;
    }

    /* Update the USL Db */    
    dbRv = usl_db_create_stg(USL_CURRENT_DB, appId, *pStg);
	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }
    
  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE )
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: Failed to create stg %d/%d, hwRv %d  dbRv %d\n", 
            appId, *pStg, hwRv, dbRv);
  }

  USL_STG_BCMX_LOCK_GIVE();

  /* if we failed before the bcmx call, return code */
  return rv;
}

/*********************************************************************
* @purpose  Associate a Spanning Tree group with a VLAN
*
* @param    appInstId    @{(input)} Application instance-id
* @param    stg          @{(input)} the Spanning Tree Group
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_stg_destroy(L7_uint32 appInstId, bcm_stg_t stg)
{
  int  rv;
  int  hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* protect the tables while manipulating the data */
  USL_STG_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_STG_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_stg_destroy(appInstId, stg,0,L7_NULL);
    }

    
    /* Update the USL Db even if hw delete failed */
    dbRv = usl_db_destroy_stg(USL_CURRENT_DB, appInstId, stg);
 
    if (usl_stg_hw_id_free(stg) != BCM_E_NONE)
    {
      USL_LOG_MSG(USL_BCM_E_LOG, "Failed to free stgId %d\n", stg);
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE )
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: Failed to delete stg %d, hwRv %d dbRv %d\n", 
            stg, hwRv, dbRv);
  }

  USL_STG_BCMX_LOCK_GIVE();

  /* if we failed before the bcmx call, return code */
  return rv;
}


/*********************************************************************
* @purpose  Associate a Spanning Tree group with a VLAN
*
* @param    appInstId    @{(input)} Application instance-id
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vid          @{(input)} the VLAN ID 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_stg_vlan_add(L7_uint32 appInstId, bcm_stg_t stg, bcm_vlan_t vid)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* protect the tables while manipulating the data */
  USL_STG_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_STG_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_stg_vlan_update(appInstId, stg, vid, 
                                           USL_CMD_ADD, 0, L7_NULL);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_stg_vlan_update(USL_CURRENT_DB, appInstId, stg, vid, USL_CMD_ADD);
  	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }
    
  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE )
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: Failed to add vlan %d to stg %d, hwRv %d dbRv %d\n", 
            vid, stg, hwRv, dbRv);
  }

  USL_STG_BCMX_LOCK_GIVE();

  /* if we failed before the bcmx call, return code */
  return rv;
}

/*********************************************************************
* @purpose  Remove an association between a Spanning Tree group and a VLAN
*
* @param    appInstId    @{(input)} Application instance-id
* @param    stg          @{(input)} the Spanning Tree Group
* @param    vid          @{(input)} the VLAN ID 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_stg_vlan_remove(L7_uint32 appInstId, bcm_stg_t stg, 
                             bcm_vlan_t vid)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* protect the tables while manipulating the data */
  USL_STG_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_STG_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_stg_vlan_update(appInstId, stg, vid, 
                                           USL_CMD_REMOVE, 0, L7_NULL);
    }

    /* Update the USL Db even if hw delete failed */
    dbRv = usl_db_stg_vlan_update(USL_CURRENT_DB, appInstId, stg,
                                  vid, USL_CMD_REMOVE);
  	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE )
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: Failed to remove vlan %d from stg %d, hwRv %d dbRv %d\n", 
             vid, stg, hwRv, dbRv);
  }

  USL_STG_BCMX_LOCK_GIVE();

  /* if we failed before the bcmx call, return code */
  return rv;
}

/*********************************************************************
* @purpose  Add a L2 Multicast MAC entry to the HW's Database
*
* @param   mcAddr @{(input)} Multicast address to be added
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_mcast_addr_add(usl_bcm_mcast_addr_t *mcAddr)
{
  int       rv;
  int       hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  L7_uint32 index;

  /* protect the tables while manipulating the data */
  USL_L2MC_BCMX_LOCK_TAKE();

  do
  {
    hwRv = usl_l2mc_hw_id_allocate(mcAddr, &index);
    if (hwRv != BCM_E_NONE)
    {
      break;    
    }

    mcAddr->l2mc_index = index;

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_MCAST_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_mcast_addr_update(mcAddr, USL_CMD_ADD, 0, L7_NULL);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      if (usl_l2mc_hw_id_free(mcAddr->l2mc_index) != BCM_E_NONE)
      {
        USL_LOG_MSG(USL_BCM_E_LOG, "Failed to free l2mc_index %d\n",
                    mcAddr->l2mc_index);    
      }
      break;
    }

    dbRv = usl_db_l2mc_addr_update(USL_CURRENT_DB, mcAddr, USL_CMD_ADD);
   	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }


  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE )
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: Failed to add multicast address, hwRv %d dbRv %d\n", hwRv, dbRv);
  }
    

  USL_L2MC_BCMX_LOCK_GIVE();

  /* if we failed before the bcmx call, return code */
  return rv;
}

/*********************************************************************
* @purpose  Remove a L2 Multicast address from the HW's database
*
* @param    mcAddr  @{(input)} Multicast Address to be deleted
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_mcast_addr_remove(usl_bcm_mcast_addr_t *mcAddr)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* protect the tables while manipulating the data */
  USL_L2MC_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_MCAST_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_mcast_addr_update(mcAddr, USL_CMD_REMOVE, 0, L7_NULL);
    }

    /* Update the USL Db even if hw delete failed */
    dbRv = usl_db_l2mc_addr_update(USL_CURRENT_DB, mcAddr, USL_CMD_REMOVE);

    if (usl_l2mc_hw_id_free(mcAddr->l2mc_index) != BCM_E_NONE)
    {
      USL_LOG_MSG(USL_BCM_E_LOG, "Failed to free l2mc_index %d\n",
                  mcAddr->l2mc_index);    
    }


  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE )
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: Failed to remove multicast address, hwRv %d dbRv %d\n", hwRv, dbRv);
  }

  USL_L2MC_BCMX_LOCK_GIVE();

  /* if we failed before the bcmx call, return code */
  return rv;
}

/*********************************************************************
* @purpose  Add a list of ports to a L2 Multicast address 
*
* @param    mcaddr       @{(input)} the mcast address data structure
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_mcast_join_ports(usl_bcm_mcast_addr_t *mcaddr)
{
  int     rv ;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* protect the tables while manipulating the data */
  USL_L2MC_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_MCAST_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_mcast_update_ports(mcaddr, USL_CMD_ADD, 0, L7_NULL);
    }

    if ((L7_BCMX_OK(hwRv) != L7_TRUE) || (hwRv == BCM_E_NOT_FOUND))
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_l2mc_update_ports(USL_CURRENT_DB, mcaddr, USL_CMD_ADD);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if ((L7_BCMX_OK(rv) != L7_TRUE) || (rv == BCM_E_NOT_FOUND))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: Failed to add ports to mac (%x:%x:%x:%x:%x:%x) vid %d, hwRv %d dbRv %d\n", 
            mcaddr->mac[0], mcaddr->mac[1], mcaddr->mac[2], 
            mcaddr->mac[3], mcaddr->mac[4], mcaddr->mac[5], 
            mcaddr->vid,  hwRv, dbRv);

  }

  USL_L2MC_BCMX_LOCK_GIVE();

  /* if we failed before the bcmx call, return code */
  return(rv);


}

/*********************************************************************
* @purpose  Remove a list of ports from a L2 Multicast address 
*
* @param    mcaddr       @{(input)} the mcast address data structure
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_mcast_leave_ports (usl_bcm_mcast_addr_t *mcaddr)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* protect the tables while manipulating the data */
  USL_L2MC_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_MCAST_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_mcast_update_ports(mcaddr, USL_CMD_REMOVE, 0, L7_NULL);
    }

    /* Do not update the USL Db if hw add failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_db_l2mc_update_ports(USL_CURRENT_DB, mcaddr, USL_CMD_REMOVE);
   	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }
    
  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: error leaving ports to l2 mcast address, hwRv %d dbRv %d\n", hwRv, dbRv);
  }

  USL_L2MC_BCMX_LOCK_GIVE();

  /* if we failed before the bcmx call, return code */
  return(rv);
}

/*********************************************************************
* @purpose  Add a port to a list of L2 Multicast addresses
*
* @param    port              @{(input)} port being operated on
* @param    l2mc_index        @{(input)} array of L2MC indices
* @param    l2mc_index_count  @{(input)} count of L2MC indices
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_mcast_port_join_groups(bcm_gport_t port, int *l2mc_index, int l2mc_index_count)
{
  int     rv ;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* protect the tables while manipulating the data */
  USL_L2MC_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_MCAST_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_mcast_port_update_groups(port, l2mc_index, l2mc_index_count, USL_CMD_ADD);
    }

    if ((L7_BCMX_OK(hwRv) != L7_TRUE) || (hwRv == BCM_E_NOT_FOUND))
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_l2mc_port_update_groups(USL_CURRENT_DB, port, l2mc_index, l2mc_index_count, USL_CMD_ADD);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if ((L7_BCMX_OK(rv) != L7_TRUE) || (rv == BCM_E_NOT_FOUND))
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: Failed to add port to L2MC entries, hwRv %d dbRv %d\n", 
            hwRv, dbRv);

  }

  USL_L2MC_BCMX_LOCK_GIVE();

  /* if we failed before the bcmx call, return code */
  return(rv);


}

/*********************************************************************
* @purpose  Remove a port from a list of L2 Multicast addresses
*
* @param    port              @{(input)} port being operated on
* @param    l2mc_index        @{(input)} array of L2MC indices
* @param    l2mc_index_count  @{(input)} count of L2MC indices
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_mcast_port_leave_groups(bcm_gport_t port, int *l2mc_index, int l2mc_index_count)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* protect the tables while manipulating the data */
  USL_L2MC_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_MCAST_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_mcast_port_update_groups(port, l2mc_index, l2mc_index_count, USL_CMD_REMOVE);
    }

    /* Do not update the USL Db if hw add failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_db_l2mc_port_update_groups(USL_CURRENT_DB, port, l2mc_index, l2mc_index_count, USL_CMD_REMOVE);
   	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }
    
  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: error leaving port to l2 mcast addresses, hwRv %d dbRv %d\n", hwRv, dbRv);
  }

  USL_L2MC_BCMX_LOCK_GIVE();

  /* if we failed before the bcmx call, return code */
  return(rv);
}

/*********************************************************************
* @purpose  sets the port to the uslipsubnetvlan classfiertable
*
* @param   port    @b{(input)} Ethernet Port
* @param   type    @b{(input)} type field
* @param   arg     @b{(input)} arg field
*
* @returns  bcm_error_t
*
* @end
*
*********************************************************************/
int usl_ip_bcmx_vlan_control_port_set(bcmx_lport_t port,
                                      bcm_vlan_control_port_t type, int arg)
{
   return(bcmx_vlan_control_port_set(port,type,arg));
}

/*********************************************************************
* @purpose  Adds the entry to the uslipsubnetvlan table
*
* @param   ipaddr  @b{(input)} IP Address
* @param   netmask @b{(input)} net mask
* @param   vid     @b{(input)} vlan Id
* @param   prio    @b{(input)} priority field
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
int usl_bcmx_vlan_ip4_add(bcm_ip_t ipaddr, bcm_ip_t netmask, bcm_vlan_t vid, int prio)
{
  int     rv ;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_VLAN_IPSUBNET_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_VLAN_IPSUBNET_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_vlan_ip4_add(ipaddr, netmask, vid, prio);
    }

    /* Do not update the USL Db if hw add failed */
    if (L7_BCMX_OK(hwRv) == L7_FALSE)
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_vlan_ip4_add(USL_CURRENT_DB, ipaddr, netmask, vid, prio);
    if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }


  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) == L7_FALSE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: error configuring ip vlan entry, hwRv %d dbRv %d\n", hwRv, dbRv);

  }

  USL_VLAN_IPSUBNET_BCMX_LOCK_GIVE();

  return (rv);
}

/*********************************************************************
* @purpose  Deletes the entry from the uslipsubnetvlan table
*
* @param   ipaddr     @b{(input)} IP Address
* @param   netmask @b{(input)} net mask
*
* @returns  bcm_error_t
*
* @end
*
*********************************************************************/
int usl_bcmx_vlan_ip4_delete(bcm_ip_t ipaddr, bcm_ip_t netmask)
{
  int     rv ;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_VLAN_IPSUBNET_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_VLAN_IPSUBNET_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_vlan_ip4_delete(ipaddr, netmask);
    }

    /* Delete the entry from USL db even if hw delete failed */
    dbRv = usl_db_vlan_ip4_delete(USL_CURRENT_DB, ipaddr, netmask);
   	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }


  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) == L7_FALSE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: error removing ip vlan entry, hwRv %d dbRv %d\n", hwRv, dbRv);

  }


  USL_VLAN_IPSUBNET_BCMX_LOCK_GIVE();

  return (rv);

}

/*********************************************************************
* @purpose  sets the port to the uslvlanmac classfiertable 
*
* @param   port    @b{(input)} Ethernet Port 
* @param   type    @b{(input)} type field 
* @param   arg     @b{(input)} arg field 
*
* @returns  bcm_error_t 
*
* @end
*
*********************************************************************/
int usl_mac_bcmx_vlan_control_port_set(bcmx_lport_t port,
                                       bcm_vlan_control_port_t type, int arg)
{
   return(bcmx_vlan_control_port_set(port, type, arg));
}

/*********************************************************************
* @purpose  Adds the entry to the uslvlanmac table 
*
* @param   mac     @b{(input)} Mac Address
* @param   vid     @b{(input)} vlan Id 
* @param   prio    @b{(input)} priority field 
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
int usl_bcmx_vlan_mac_add(bcm_mac_t mac, bcm_vlan_t vid, int prio)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_VLAN_MAC_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_VLAN_MAC_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_vlan_mac_add(mac, vid, prio);
    }

    if (L7_BCMX_OK(hwRv) == L7_FALSE)
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_vlan_mac_add(USL_CURRENT_DB, mac, vid, prio);
   	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }


  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) == L7_FALSE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: error configuring mac vlan entry, hwRv %d dbRv %d\n", hwRv, dbRv);

  }

  USL_VLAN_MAC_BCMX_LOCK_GIVE();

  return (rv);

}

/*********************************************************************
* @purpose  Deletes the entry to the uslvlanmac table 
*
* @param   mac     @b{(input)} Mac Address
*
* @returns BCM error code
*
* @end
*
*********************************************************************/
int usl_bcmx_vlan_mac_delete(bcm_mac_t mac)
{
  int     rv ;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_VLAN_MAC_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_VLAN_MAC_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_vlan_mac_delete(mac);
    }


    /* Delete the entry from USL Db even if hw delete failed */
    dbRv = usl_db_vlan_mac_delete(USL_CURRENT_DB, mac);
   	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }


  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) == L7_FALSE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: error deleting mac vlan entry, hwRv %d dbRv %d\n", hwRv, dbRv);

  }


  USL_VLAN_MAC_BCMX_LOCK_GIVE();

  return (rv);

}

/*********************************************************************
* @purpose  Delete a port from protected port group 
*
* @param    lport                 @{(input)}  port to be removed
* @param    groupId               @{(input)}  GroupId of the protected port
*
*
* @returns  Defined by Broadcom driver
*
* @notes   
*
* @end
*********************************************************************/
int usl_bcmx_protected_group_port_remove(bcmx_lport_t lport, L7_uint32 groupId)
{
  L7_int32                        rv;
  int                             hwRv= BCM_E_NONE, dbRv = BCM_E_NONE;
  int                             bcmPort;
  L7_int32                        modid = 0;
  usl_bcm_protected_group_delete_t  deleteInfo;
  
  memset(&deleteInfo, 0, sizeof (deleteInfo));

  bcmPort = BCM_GPORT_MODPORT_PORT_GET(lport);
  modid = BCM_GPORT_MODPORT_MODID_GET(lport);
  if (modid < 0 || bcmPort < 0)
  {
    rv = BCM_E_FAIL;
    return rv; 
  }

  USL_PROTECTED_GROUP_BCMX_LOCK_TAKE();

  do
  {

    deleteInfo.groupId = groupId;
    deleteInfo.modid   = modid;
    deleteInfo.bcmPort = bcmPort;

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_PROTECTED_GRP_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_protected_group_port_remove(&deleteInfo);
    }

    /* Update the USL Db even if hw delete failed */
    dbRv = usl_db_protected_group_port_remove(USL_CURRENT_DB, &deleteInfo);
   	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }
   

  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_E_LOG, "Failed to remove port %d from protected group %d, hwRv %d dbRv %d \n", 
                lport, groupId, hwRv, dbRv);
  }

  USL_PROTECTED_GROUP_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Create a protected port group 
*
* @param    groupInfo  @{(input)}  Protected group info
*
*
* @returns  Defined by Broadcom driver
*
* @notes   
*
* @end
*********************************************************************/
int usl_bcmx_protected_group_set(usl_bcm_protected_group_t *groupInfo)
{
  int                        rv; 
  int                        hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  

  USL_PROTECTED_GROUP_BCMX_LOCK_TAKE();

  do
  {
    
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_L2_PROTECTED_GRP_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_protected_group_set(groupInfo);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_db_protected_group_set(USL_CURRENT_DB, groupInfo);
    if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }

  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_E_LOG, "Protected port group %d set failed, hwRv %d dbRv %d \n", 
                groupInfo->groupId, hwRv, dbRv);
  }

  USL_PROTECTED_GROUP_BCMX_LOCK_GIVE();

  return rv;
}

#ifdef L7_STACKING_PACKAGE

/*********************************************************************
* @purpose  Flush dynamic MAC addresses for specified trunk.
*
* @param    tgid - BCMX trunk identifier.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the 
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_trunk (bcm_trunk_t tgid)
{
  usl_mac_table_trunk_flush ((L7_uint32) tgid);

  return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Flush dynamic MAC addresses for specified wlan port.
*
* @param    port - BCMX wlan vp identifier.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the 
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_wlan_port (bcmx_lport_t port)
{
  usl_mac_table_wlan_port_flush ((L7_uint32) port);

  return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Flush dynamic MAC addresses for specified lport.
*
* @param    tgid - BCMX trunk identifier.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the 
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_port (bcmx_lport_t lport)
{
  usl_mac_table_port_flush (lport);

  return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Flush specific dynamic MAC addresses.
*
* @param    mac - BCMX mac identifier.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the 
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_mac(bcm_mac_t mac)
{
  usl_mac_table_mac_flush (mac);

  return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Flush dynamic MAC addresses for all lport.
*
* @param    none.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_all ()
{
  usl_mac_table_flush_all ();

  return BCM_E_NONE;
}

/*********************************************************************
* @purpose Synchronize all the dynamic L2 entries w/ the FDB application
*
* @param    none.
*
* @returns  BCM_E_xxx
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_sync()
{
  usl_mac_table_sync_application();

  return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Flush dynamic MAC addresses matching specified vlan.
*
* @param    vid - BCM VLAN identifier.
*
* @returns  BCM_E_NONE
*
* @note  This function has somewhat different parameters from the 
*        corresponding BCMX call.
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_remove_by_vlan (bcm_vlan_t vid)
{
  usl_mac_table_vlan_flush (vid);

  return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Add a L2 MAC address to the HW's forwarding database
*
* @param    l2addr     @{(input)} the MAC address, settings, and flags
* @param    port_block @{(input)} List of ports to filter for this addr
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_add(bcmx_l2_addr_t *l2addr ,bcmx_lplist_t *port_block)
{
  int rc = BCM_E_NONE;

  if (!uslDatabaseActive)
    return l7_bcmx_l2_addr_add(l2addr,port_block,0,L7_NULLPTR);

  rc = usl_mac_mgmt_add(l2addr, port_block);

  return rc;
}

/*********************************************************************
* @purpose  Remove a L2 MAC address from the HW's forwarding database
*
* @param    mac_addr     @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_l2_addr_delete(bcm_mac_t mac_addr, bcm_vlan_t vid)
{
  int rc = BCM_E_NONE;

  if (!uslDatabaseActive)
    return l7_bcmx_l2_addr_delete(mac_addr, vid,0,L7_NULL);

  usl_mac_mgmt_delete(mac_addr,vid);

  return rc;
}

/*********************************************************************
* @purpose  Set the aging time
*
* @param    ageTime     @{(input)} The aging time in seconds
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_l2_age_timer_set(L7_int32 ageTime)
{
  int rv = BCM_E_NONE;

  USL_SYSTEM_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
    {
    usl_mac_age_time_set(ageTime);
    }

    rv = usl_db_l2_age_time_set(USL_CURRENT_DB, ageTime);

  } while (0);


  /* Log message on error */
  if (L7_BCMX_OK(rv) == L7_FALSE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "USL: error configuring l2 age time to %d, rv %d\n", 
            ageTime, rv);

  }

  USL_SYSTEM_BCMX_LOCK_GIVE();

  return rv;
}
#endif

/*********************************************************************
* @purpose  Sets the DVLAN MODE for all the ports 
*
* @param    mode 
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_dvlan_mode_set(L7_uint32 mode)
{
  int     rv ;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_SYSTEM_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_dvlan_mode_set(mode); 
    }

  	if (L7_BCMX_OK(hwRv) != L7_TRUE )
    {
      break;
    }


    /* Update the USL Db */
    dbRv = usl_db_dvlan_mode_set(USL_CURRENT_DB, mode);
   	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }

  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE )
  {
    USL_LOG_MSG(USL_BCM_E_LOG,
                "Failed to configure dvlan mode %d, hwRv %d dbRv %d\n",
                mode, hwRv, dbRv);
  }

  USL_SYSTEM_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
* @purpose  Sets the DVLAN Translation for all the ports
*
* @param    mac          @{(input)} the mode
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_dvlan_translation_set(L7_BOOL direction, L7_BOOL enable)
{
  int     rv ;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_SYSTEM_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_dvlan_translation_set(direction, enable); 
    }

  	if (L7_BCMX_OK(hwRv) != L7_TRUE )
    {
      break;
    }


    /* Update the USL Db */
    dbRv = usl_db_dvlan_translation_set(USL_CURRENT_DB, direction, enable);
   	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }

  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE )
  {
    USL_LOG_MSG(USL_BCM_E_LOG,
                "Failed to configure dvlan translation direction %d enable %d, hwRv %d dbRv %d\n",
                direction, enable, hwRv, dbRv);
  }

  USL_SYSTEM_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Sets the Default TPID for all the ports
*
* @param    mac          @{(input)} the mode
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_dvlan_default_tpid_set(L7_ushort16 etherType)
{
  int     rv ;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_SYSTEM_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_dvlan_tpid_set(etherType); 
    }

  	if (L7_BCMX_OK(hwRv) != L7_TRUE )
    {
      break;
    }


    /* Update the USL Db */
    dbRv = usl_db_dvlan_tpid_set(USL_CURRENT_DB, etherType);
   	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }

  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE )
  {
    USL_LOG_MSG(USL_BCM_E_LOG,
                "Failed to configure dvlan tpid ethertype %d, hwRv %d dbRv %d\n",
                etherType, hwRv, dbRv);
  }

  USL_SYSTEM_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Used to indicate to all units whether learns are allowed
*           on the specified trunk.
*
* @param    appId        @{(input)} Application-id for the trunk
* @param    tid          @{(input)} The Trunk ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_trunk_learn_mode_set(L7_uint32 appId, bcm_trunk_t tgid, 
                                  L7_BOOL learningLocked)
{
  int                        rv = BCM_E_NONE;
  L7_RC_t                    rc;

 
  USL_TRUNK_BCMX_LOCK_TAKE();

  do
  {

    rc = l7_rpc_client_trunk_learn_mode_set(appId, tgid, learningLocked); 
    if (L7_SUCCESS != rc)
    {
      rv = BCM_E_TIMEOUT;
    }

    rv = usl_db_trunk_learn_mode_set(USL_CURRENT_DB, appId, tgid, learningLocked);

  } while (0);

  USL_TRUNK_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Enable/Disable Flow control in the system
*
* @param    unit    @{{input}}  L7_ALL_UNITS - to enable on all units
*                               or unit number of the specific unit
* @param    mode    @{{input}}  enable=1,disable=0
* @param    mac     @{{input}}  mac address of switch
*
* @returns  defined by Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_flow_control_set(L7_uint32 unit, L7_uint32 mode, L7_uchar8 *mac)
{
  int     rv = BCM_E_NONE;
  L7_RC_t rc = L7_SUCCESS;
  
  USL_SYSTEM_BCMX_LOCK_TAKE();

  do
  {

    if (USL_BCMX_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
    {
      rc = l7_rpc_client_flow_control_set(unit, mode, mac);
    }

    if (rc != L7_SUCCESS)
    {
      rv = BCM_E_TIMEOUT;
      break;
    }

    rv = usl_db_flow_control_set(USL_CURRENT_DB, mode, mac);
   
  } while (0);
  
  USL_SYSTEM_BCMX_LOCK_GIVE();

  return rv; 
}

/*********************************************************************
* @purpose  Enable/Disable DOS control in the system
*
* @param    type    @{{input}}  type of DOS control
* @param    mode    @{{input}}  enable=1,disable=0
* @param    arg     @{{input}}  argument for certain types of DOS control
*
* @returns  defined by Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_doscontrol_set(L7_DOSCONTROL_TYPE type, L7_uint32 mode, L7_uint32 arg)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  
  USL_SYSTEM_BCMX_LOCK_TAKE();

  do
  {
    if (USL_BCMX_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_doscontrol_set(type, mode, arg);
    }

    /* Do not update the USL Db if hw add failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_db_doscontrol_set(USL_CURRENT_DB, type, mode, arg);
   
  } while (0);
  
  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_INFO_LOG, 
                "USL: Failed to set DOS control for type %d to mode %d, hwRv %d dbRv %d\n",
                type, mode, hwRv, dbRv);
  }

  USL_SYSTEM_BCMX_LOCK_GIVE();

  return rv; 
}

#ifdef L7_MCAST_PACKAGE
/*********************************************************************
* @purpose  Enable IPMC in the chip
*
* @param    enabled     @{(input)} enable(1) or disable(0)
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_ipmc_enable(int enabled)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  USL_SYSTEM_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_ipmc_enable(enabled, 0, L7_NULL);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_ipmc_mode_set(USL_CURRENT_DB, enabled);
   	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }

  } while ( 0 );

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE )
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Failed to set IPMC mode, hwRv %d dbRv %d\n", hwRv, dbRv);
  }

  USL_SYSTEM_BCMX_LOCK_GIVE();

  return rv;
}
#endif

/*********************************************************************
* @purpose  Enable/Disable Flow control in the system
*
* @param    mode    @{{input}}  enable=1,disable=0
*
* @returns  defined by Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_mmu_dropmode_set(L7_uint32 mode)
{
  int     rv = BCM_E_NONE;
  L7_RC_t rc = L7_SUCCESS;

  USL_SYSTEM_BCMX_LOCK_TAKE();

  do
  {

    if (USL_BCMX_CONFIGURE_HW(USL_SYSTEM_DB_ID) == L7_TRUE)
    {
      rc = l7_rpc_client_mmu_dropmode_set(mode);
    }

    if (rc != L7_SUCCESS)
    {
      rv = BCM_E_TIMEOUT;
      break;
    }

    rv = usl_db_mmu_dropmode_set(USL_CURRENT_DB, mode);
   
  } while (0);
  
  USL_SYSTEM_BCMX_LOCK_GIVE();

  return rv; 
}
