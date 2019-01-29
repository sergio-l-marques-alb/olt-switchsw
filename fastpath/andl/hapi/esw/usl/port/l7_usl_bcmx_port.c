/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_bcmx_port.c
*
* @purpose    USL_BCMX Port API implementation 
*
* @component  USL
*
* @comments   none
*
* @create     11/07/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "l7_usl_bcmx_port.h"
#include "l7_usl_port_db.h"
#include "l7_rpc_port.h"
#include "l7_usl_trace.h"
#include "l7_usl_sm.h"
#include "bcmx/port.h"

/* Port Db Bcmx Lock */
static osapiRWLock_t      uslPortDbBcmxRWLock;

#define USL_PORT_BCMX_LOCK_TAKE() \
{ \
  usl_trace_sema(USL_PORT_DB_ID ,"PDB", \
                 osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiReadLockTake(uslPortDbBcmxRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG, \
                "Failed to take USL Port Db Read lock, semId %x\n", \
                uslPortDbBcmxRWLock); \
  } \
}

#define USL_PORT_BCMX_LOCK_GIVE() \
{ \
  usl_trace_sema(USL_PORT_DB_ID ,"PDB",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiReadLockGive(uslPortDbBcmxRWLock) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG, \
                "Failed to give USL Port Db Read lock, semId %x\n", \
                uslPortDbBcmxRWLock); \
  } \
}

/*********************************************************************
*
* @purpose Initialize port Bcmx
*
* @params  none
*
* @returns L7_RC_t
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t usl_port_bcmx_init()
{
  L7_RC_t rc = L7_SUCCESS;

  if (osapiRWLockCreate(&uslPortDbBcmxRWLock, 
                        OSAPI_RWLOCK_Q_PRIORITY) != L7_SUCCESS)
  {
    USL_L7_LOG_ERROR("USL: unable to allocate RW Lock for the Port database\n");
  }

  return rc;  
}

/*********************************************************************
* @purpose  Suspend all the usl_bcmx_port API calls
*
* @param    none
*
* @returns  none
*
* @notes   This is invoked at the begining of synchronization/reconciliation. 
*          
*       
* @end
*********************************************************************/
void usl_port_bcmx_suspend() 
{ 
  usl_trace_sema(USL_PORT_DB_ID ,"PDB",osapiTaskIdSelf(),__LINE__,L7_TRUE);

  if (osapiWriteLockTake(uslPortDbBcmxRWLock,L7_WAIT_FOREVER) != L7_SUCCESS) 
  {
    USL_L7_LOG_ERROR("Failed to take USL Port Write Read lock\n"); 
  }

  return;
}

/*********************************************************************
* @purpose  Resume all the usl_bcmx_port API calls
*
* @param    none
*
* @returns  none
*
* @notes   This is invoked at the end of synchronization/reconciliation. 
*          
*       
* @end
*********************************************************************/
void usl_port_bcmx_resume()
{
  usl_trace_sema(USL_PORT_DB_ID ,"PDB",osapiTaskIdSelf(),__LINE__,L7_TRUE);

  if (osapiWriteLockGive(uslPortDbBcmxRWLock) != L7_SUCCESS) 
  {
    USL_L7_LOG_ERROR("Failed to give USL Port Write Read lock\n"); 
  }
  
  return;
}

/*********************************************************************
*
* @purpose Set the broadcast rate threshold for a port
*
*
* @param    port           @{(input)} Lport 
* @param    bcast_limit    @{(input)} Bcast rate threshold limits
*
* @returns BCMX Error Code
*
* @notes   Invoked by HAPI
*
* @end
*
*********************************************************************/
int usl_bcmx_rate_bcast_set(bcmx_lport_t port, 
                            usl_bcm_port_rate_limit_t bcast_limit)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_rate_bcast_set(port, bcast_limit);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }
    
    /* Update the USL Db */
    dbRv = usl_portdb_bcast_rate_set(USL_CURRENT_DB, port, &bcast_limit);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to update bcast rate for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);      
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Set the multicast rate threshold for a port
*
*
* @param    port           @{(input)} Lport 
* @param    mcast_limit    @{(input)} Mcast rate threshold limits
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_rate_mcast_set(bcmx_lport_t port, 
                            usl_bcm_port_rate_limit_t mcast_limit)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_rate_mcast_set(port, mcast_limit);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }
    
    dbRv = usl_portdb_mcast_rate_set(USL_CURRENT_DB, port, &mcast_limit);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to update mcast rate for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;
}


/*********************************************************************
*
* @purpose Set the dlf rate threshold for a port
*
*
* @param    port           @{(input)} Lport 
* @param    dlf_limit      @{(input)} Dlf rate threshold limits
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_rate_dlfbc_set(bcmx_lport_t port, 
                            usl_bcm_port_rate_limit_t dlfbc_limit)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_rate_dlfbc_set(port, dlfbc_limit);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_portdb_dlfbc_rate_set(USL_CURRENT_DB, port, &dlfbc_limit);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set dlf rate for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Set the Ingress filtering mode for a port
*
* @param   port    -  The LPORT 
* @param   flags   -  Filtering mode
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_member_set(bcmx_lport_t port, 
                                  usl_bcm_port_filter_mode_t mode)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_vlan_member_set(port, mode);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_portdb_filter_mode_set(USL_CURRENT_DB, port, &mode);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set the filtering mode for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);

  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Set the priority for a port
*
* @param   port       -  The LPORT 
* @param   priority   -  Default priority for the port
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_untagged_priority_set(bcmx_lport_t port, 
                                        usl_bcm_port_priority_t priority)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_untagged_priority_set(port, priority);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_portdb_priority_set(USL_CURRENT_DB, port, &priority);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set priority for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);

  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Set the max frame size allowed on a port
*
* @param   port           -  The LPORT 
* @param   maxFrameSize   -  Max frame size data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_frame_max_set(bcmx_lport_t port, 
                                usl_bcm_port_frame_size_t max_frame_size)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_frame_max_set(port, max_frame_size);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_portdb_max_frame_set(USL_CURRENT_DB, port, &max_frame_size);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set max frame size for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Set the learn mode for a port
*
* @param   port           -  The LPORT 
* @param   learnMode      -  Learn mode
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_learn_set(bcmx_lport_t port, 
                            usl_bcm_port_learn_mode_t learn_mode)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;


  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_learn_set(port, learn_mode);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }
 
    dbRv = usl_portdb_learn_set(USL_CURRENT_DB, port, &learn_mode);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set learn mode for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}


/*********************************************************************
*
* @purpose Set the dtag mode for a port
*
* @param   port           -  The LPORT 
* @param   dtagMode       -  Learn mode
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_dtag_mode_set(bcmx_lport_t port, 
                                usl_bcm_port_dtag_mode_t dtag_mode)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;


  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_dtag_mode_set(port, dtag_mode);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_portdb_dtag_mode_set(USL_CURRENT_DB, port, &dtag_mode);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set dtag mode for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Set the tpid for a port
*
* @param   port           -  The LPORT 
* @param   tpid           -  tpid
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_tpid_set(bcmx_lport_t port, usl_bcm_port_tpid_t tpid)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;


  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_tpid_set(port, tpid);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_portdb_tpid_set(USL_CURRENT_DB, port, &tpid);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set tpid for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Add a tpid for a port
*
* @param   port           -  The LPORT 
* @param   tpid           -  tpid
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_tpid_add(bcmx_lport_t port, usl_bcm_port_tpid_t tpid)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;


  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_tpid_add(port, tpid);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_portdb_tpid_add(USL_CURRENT_DB, port, &tpid);
   	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }
  
  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set tpid for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Delete a tpid from a port
*
* @param   port           -  The LPORT 
* @param   tpid           -  tpid
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_tpid_delete(bcmx_lport_t port, usl_bcm_port_tpid_t tpid)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;


  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_tpid_delete(port, tpid);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_portdb_tpid_delete(USL_CURRENT_DB, port, &tpid);
   	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }
  
  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set tpid for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Set the default vid for a port
*
*
* @param    port           @{(input)} Lport 
* @param    vid            @{(input)} Default vid
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_untagged_vlan_set(bcmx_lport_t port, bcm_vlan_t vid)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;


  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_untagged_vlan_set(port, vid);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_portdb_pvid_set(USL_CURRENT_DB, port, &vid);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set pvid for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Set the discard mode for a port
*
*
* @param    port           @{(input)} Lport 
* @param    mode           @{(input)} Discard mode
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_discard_set(bcmx_lport_t port, bcm_port_discard_t mode)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_discard_set(port, mode);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_portdb_discard_set(USL_CURRENT_DB, port, &mode);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set the discard mode for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Set the phy medium config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    medium         @{(input)} Medium of the phy
* @param    config         @{(input)} Medium configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_medium_config_set(bcmx_lport_t port,
                                    bcm_port_medium_t medium,
                                    bcm_phy_config_t  *config)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_medium_config_set(port, medium, config);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    } 

    dbRv = usl_portdb_medium_config_set(USL_CURRENT_DB, port, medium, config);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to configure phy settings for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  } 

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Set the flow-control config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    pauseConfig    @{(input)} Pause configuration for the port
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_flow_control_set(bcmx_lport_t port, 
                                   usl_bcm_port_pause_config_t pauseConfig)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_flow_control_set(port, pauseConfig);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    } 

    dbRv = usl_portdb_flow_control_set(USL_CURRENT_DB, port, &pauseConfig);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set the flow-control settings for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);  
  } 

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Set the Cos queue Sched config for a port
*
*
* @param    port               @{(input)} Lport 
* @param    cosqSchedConfig    @{(input)} Cosq Sched configuration for the port
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_cosq_sched_set(bcmx_lport_t port, 
                                 usl_bcm_port_cosq_sched_config_t cosqSchedConfig)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;


  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_cosq_sched_set(port, cosqSchedConfig);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_portdb_cosq_sched_set(USL_CURRENT_DB, port, &cosqSchedConfig);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set cosq sched settings for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}


/*********************************************************************
*
* @purpose Set the rate shaper config for a port
*
*
* @param    port               @{(input)} Lport 
* @param    shaperConfig       @{(input)} Rate shaper configuration for the port
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_rate_egress_set(bcmx_lport_t port, 
                                  usl_bcm_port_shaper_config_t shaperConfig)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_rate_egress_set(port, shaperConfig);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }
    
    dbRv = usl_portdb_shaper_config_set(USL_CURRENT_DB, port, &shaperConfig);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set egress rate for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Add port to vlans 
*
*
* @param    port               @{(input)} Lport 
* @param    vlanConfig         @{(input)}  Vlan configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_member_add(bcmx_lport_t port, 
                                  usl_bcm_port_vlan_t *vlanConfig)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_vlan_config(port, vlanConfig, L7_TRUE);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }  

    dbRv = usl_portdb_vlan_membership_update(USL_CURRENT_DB, port,
                                             vlanConfig, L7_TRUE);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to add gport %d to vlan, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }  

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Remove port from vlans 
*
*
* @param    port               @{(input)} Lport 
* @param    vlanConfig         @{(input)}  Vlan configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_member_delete(bcmx_lport_t port, 
                                     usl_bcm_port_vlan_t *vlanConfig)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_vlan_config(port, vlanConfig, L7_FALSE);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }  

    dbRv = usl_portdb_vlan_membership_update(USL_CURRENT_DB, port, 
                                             vlanConfig, L7_FALSE);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to remove gport %d from vlan, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }  

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Set the spanning-tree state for a port/stg
*
*
* @param    stg                @{(input)}  Stg id
* @param    port               @{(input)} Lport 
* @param    stpState          @{(input)}  State configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_stg_stp_set(bcm_stg_t stg, bcmx_lport_t port, bcm_stg_stp_t stpState)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;


  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_stg_stp_set(stg, port, stpState);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_portdb_stg_stp_update(USL_CURRENT_DB, port, 
                                     stg, stpState);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set stp state for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Associate a vlan with a protocol on a port
*
*
* @param    port               @{(input)} Lport 
* @param    pbvlanConfig       @{(input)} Protocol-based vlan configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_protocol_vlan_add(bcmx_lport_t port, 
                                    usl_bcm_port_pbvlan_config_t pbvlanConfig)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_protocol_vlan_config(port, pbvlanConfig, L7_TRUE);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }
 
    dbRv = usl_portdb_protocol_vlan_update(USL_CURRENT_DB, port, 
                                           &pbvlanConfig, L7_TRUE);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to configure protocol vlan for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Dis-associate a vlan with a protocol on a port
*
*
* @param    port               @{(input)} Lport 
* @param    pbvlanConfig       @{(input)} Protocol-based vlan configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_protocol_vlan_delete(bcmx_lport_t port, 
                                       usl_bcm_port_pbvlan_config_t pbvlanConfig)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_protocol_vlan_config(port, pbvlanConfig, L7_FALSE);
    }
  
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_portdb_protocol_vlan_update(USL_CURRENT_DB, port, 
                                           &pbvlanConfig, L7_FALSE);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set pbvlan config for gport %d, hwRv %d dbRv 5d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Set the dot1x state for the port
*
*
* @param    port               @{(input)} Lport 
* @param    dot1xStatus        @{(input)} Dot1x state for the port
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_dot1x_config(bcmx_lport_t port, 
                               L7_DOT1X_PORT_STATUS_t dot1xStatus)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_dot1x_config(port, dot1xStatus);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    { 
      break;
    }

    dbRv = usl_portdb_dot1x_config(USL_CURRENT_DB, port, dot1xStatus);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set dot1xStatus for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv); 
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
 *
 * @purpose Get SFP diagnostics for the specified port.
 *
 * @param   port - BCMX Lport
 * @param
 *
 * @returns BCMX Error Code
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
int
usl_bcmx_port_sfp_diag_get(bcmx_lport_t port,
                           int32 *temperature,
                           uint32 *voltage,
                           uint32 *current,
                           uint32 *txPower,
                           uint32 *rxPower,
                           uint32 *txFault,
                           uint32 *los)
{
  int     rv;

  rv = l7_rpc_client_port_sfp_diag_get(port, temperature, voltage, 
                                       current, txPower, rxPower, txFault, los);

  return rv;
}

/*********************************************************************
 *
 * @purpose Get copper diagnostics for the specified port.
 *
 * @param   port - BCMX Lport
 * @param
 *
 * @returns BCMX Error Code
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
int
usl_bcmx_port_copper_diag_get(bcmx_lport_t port, bcm_port_cable_diag_t *cd)
{
  int rv;

  rv = l7_rpc_client_port_copper_diag_get(port, cd);

  return rv;
}

/*********************************************************************
*
* @purpose Block an unauthorized dot1x client for the specified port.
*
* @param   port - BCMX Lport
* @param   client_cmd - Mac address, vlan Id 
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
usl_bcmx_port_dot1x_client_block(bcmx_lport_t port,
                                 usl_bcm_port_dot1x_client_t *client_cmd)
{
  int rv;

  rv = l7_rpc_client_port_dot1x_client_block(port, client_cmd);

  return rv;

}

/*********************************************************************
*
* @purpose Unblock an unauthorized dot1x client for the specified port.
*
* @param   port - BCMX Lport
* @param   client_cmd - Mac address, vlan Id 
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
usl_bcmx_port_dot1x_client_unblock(bcmx_lport_t port,
                                   usl_bcm_port_dot1x_client_t *client_cmd)
{
  int rv;

  rv = l7_rpc_client_port_dot1x_client_unblock(port, client_cmd);

  return rv;

}

/*********************************************************************
*
* @purpose Get dot1x client timeout for the specified port.
*
* @param   port - BCMX Lport
* @param   client_cmd - Mac address, vlan Id 
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
usl_bcmx_port_dot1x_client_timeout_get(bcmx_lport_t port, 
                                       usl_bcm_port_dot1x_client_t *client_cmd)
{
  int rv;

  rv = l7_rpc_client_port_dot1x_client_timeout_get(port, client_cmd);

  return rv;

}

/*********************************************************************
*
* @purpose Get all statistics for the specified port.
*
* @param   port - BCMX Lport
* @param   stats - 64-bit stats for the port.
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
usl_bcmx_stat_get(bcmx_lport_t port, uint64 stats[snmpValCount])
{
  int rv;

  rv = l7_rpc_client_stat_get(port, stats);

  return rv;
}

/*********************************************************************
*
* @purpose Get all statistics for the specified port.
*
* @param   port - BCMX Lport
* @param   stats - 64-bit stats for the port.
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
usl_bcmx_port_stat_get(bcmx_lport_t port, uint64 stats[snmpValCount])
{
  int rv;

  rv = l7_rpc_client_port_stat_get(port, stats);

  return rv;
}

/*********************************************************************
*
* @purpose Set the mirroring configuration for the port
*
*
* @param    port               @{(input)} Lport 
* @param    mirrorConfig       @{(input)} Mirroring configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_mirror_set(bcmx_lport_t port, 
                             usl_bcm_port_mirror_config_t mirrorConfig)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_mirror_set(port, mirrorConfig);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    } 

    dbRv = usl_portdb_mirror_set(USL_CURRENT_DB, port, &mirrorConfig);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to do mirrorConfig for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  } 

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Set the admin mode for a port
*
* @param   port    {(input)} 
* @param   enable  {(input)}
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_enable_set(bcmx_lport_t port, int enable)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_enable_set(port, enable);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    dbRv = usl_db_port_enable_set(USL_CURRENT_DB, port, enable);
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set enable for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Set WRED parameters on a port
*
* @param   port - BCMX Lport
* @param   wredParams - Pointer to WRED params for all queues/colors
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_wred_set(bcmx_lport_t port,
                           usl_bcm_port_wred_config_t *wredParams)
{

  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_wred_set(port, wredParams);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    
    dbRv = usl_db_port_wred_set(USL_CURRENT_DB, port, wredParams); 
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set wred params for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}


/*********************************************************************
*
* @purpose Set sflow parameters on a port
*
* @param   port - BCMX Lport
* @param   sflowConfig - Pointer to sflow config
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_sample_rate_set(bcmx_lport_t port,
                                  usl_bcm_port_sflow_config_t *sflowConfig)
{

  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_sflow_config_set(port, sflowConfig);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    
    dbRv = usl_db_port_sflow_config_set(USL_CURRENT_DB, port, sflowConfig); 
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set sflow config for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Get sflow parameters on a port
*
* @param   port - BCMX Lport
* @param   sflowConfig - Pointer to sflow config
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_sample_rate_get(bcmx_lport_t port,
                                  usl_bcm_port_sflow_config_t *sflowConfig)
{

  int     rv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* If USL caching is on then read from db */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      rv = bcmx_port_sample_rate_get(port, 
                                     &(sflowConfig->ingressSamplingRate),
                                     &(sflowConfig->egressSamplingRate));
    }
    else
    {
      rv = usl_db_port_sflow_config_get(USL_CURRENT_DB, port, sflowConfig); 
    }


  } while (0);

  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to get sflow config for gport %d, rv %d\n", 
                port, rv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;

}

/*********************************************************************
*
* @purpose Configure ingress VLAN translation for a port
*
* @param   port
* @param   enable
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_translate_ingress_enable_set(bcmx_lport_t port, L7_BOOL enable)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_vlan_translate_ingress_enable_set(port, enable);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }
    
    dbRv = usl_db_port_vlan_translate_ingress_enable_set(USL_CURRENT_DB, port, enable); 
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set ingress VLAN translation for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Configure ingress VLAN translation for a port
*
* @param   port
* @param   drop
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_translate_ingress_miss_drop_set(bcmx_lport_t port, L7_BOOL drop)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_vlan_translate_ingress_miss_drop_set(port, drop);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }
    
    dbRv = usl_db_port_vlan_translate_ingress_miss_drop_set(USL_CURRENT_DB, port, drop); 
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set ingress VLAN translation miss drop for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Configure egress VLAN translation for a port
*
* @param   port
* @param   enable
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_translate_egress_enable_set(bcmx_lport_t port, L7_BOOL enable)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_vlan_translate_egress_enable_set(port, enable);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }
    
    dbRv = usl_db_port_vlan_translate_egress_enable_set(USL_CURRENT_DB, port, enable); 
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set egress VLAN translation for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Configure egress VLAN translation for a port
*
* @param   port
* @param   drop
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_translate_egress_miss_drop_set(bcmx_lport_t port, L7_BOOL drop)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_vlan_translate_egress_miss_drop_set(port, drop);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }
    
    dbRv = usl_db_port_vlan_translate_egress_miss_drop_set(USL_CURRENT_DB, port, drop); 
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set egress VLAN translation miss drop for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Configure VLAN translation key for a port
*
* @param   port
* @param   key
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_translate_key_first_set(bcmx_lport_t port, bcm_vlan_translate_key_t key)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_vlan_translate_key_first_set(port, key);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }
    
    dbRv = usl_db_port_vlan_translate_key_first_set(USL_CURRENT_DB, port, key); 
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set VLAN translation first key for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Configure VLAN translation key for a port
*
* @param   port
* @param   key
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcmx_port_vlan_translate_key_second_set(bcmx_lport_t port, bcm_vlan_translate_key_t key)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_vlan_translate_key_second_set(port, key);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }
    
    dbRv = usl_db_port_vlan_translate_key_second_set(USL_CURRENT_DB, port, key); 
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
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to set VLAN translation second key for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  }

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;
}


/*********************************************************************
*
* @purpose Set the pfc configuration for the port
*
*
* @param    port            @{(input)} Lport 
* @param    pfcConfig       @{(input)} PFC configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_pfc_config_set(bcmx_lport_t port, 
                                usl_bcm_port_pfc_config_t pfcConfig)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Take the portdb bcmx lock */
  USL_PORT_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
    {
      hwRv = l7_rpc_client_port_pfc_config_set(port, pfcConfig);
    }

    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    } 

    dbRv = usl_portdb_pfc_config_set(USL_CURRENT_DB, port, &pfcConfig);
   	if (L7_BCMX_OK(dbRv) != L7_TRUE )
    {
      break;
    }

  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);
 
  /* Log message on error */
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (BCM_E_UNAVAIL != rv))
  {
    USL_LOG_MSG(USL_E_LOG, 
                "Failed to do pfcConfig for gport %d, hwRv %d dbRv %d\n", 
                port, hwRv, dbRv);
  } 

  USL_PORT_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
*
* @purpose Get the pfc stat for the port
*
*
* @param    port      @{(input)} Lport 
* @param    stat      @{(input)} PFC configuration
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_pfc_stat_get(bcmx_lport_t port, usl_bcm_port_pfc_stat_t *stat)
{
  return l7_rpc_client_port_pfc_stat_get(port, stat);
}

/*********************************************************************
*
* @purpose Clear the pfc stats for the port
*
*
* @param    port      @{(input)} Lport 
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcmx_port_pfc_stats_clear(bcmx_lport_t port)
{
  return l7_rpc_client_port_pfc_stats_clear(port);
}
