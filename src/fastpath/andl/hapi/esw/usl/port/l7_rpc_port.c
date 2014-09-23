/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_rpc_port.c
*
* @purpose    RPC BCMX/BCM API's for port 
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
#include "log.h"
#include "sysapi_hpc.h"
#include "l7_rpc_port.h"

#include "sal/core/libc.h"
#include "bcmx/custom.h"
#include "bcm/custom.h"
#include "bcm/l2.h"
#include "bcm/link.h"

extern int l7_rpc_server_mcast_port_update_groups(int unit, bcm_port_t port, 
                                           int setget, uint32 *args);

/*********************************************************************
*
* @purpose RPC Client API to set the broadcast rate threshold for a port
*
*
* @param    port           @{(input)} Lport 
* @param    bcast_limit    @{(input)} Bcast rate threshold limits
*
* @returns  BCMX Error Code
*
* @notes    Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_rate_bcast_set(bcmx_lport_t port, 
                                 usl_bcm_port_rate_limit_t bcast_limit)
{
  int     rv = BCM_E_NONE;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(bcast_limit) > sizeof(args))
  {
    LOG_ERROR(sizeof(bcast_limit));
  }

  memcpy(args, &bcast_limit, sizeof(bcast_limit));

  n_args = sizeof(bcast_limit)/sizeof(uint32);
  if (sizeof(bcast_limit)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_BCAST_RATE_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_BCAST_RATE_SET, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC server API to set the broadcast rate threshold for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} Bcast rate threshold data
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_rate_bcast_set (int unit, bcm_port_t port, 
                                  int setget, uint32 *args)
{
  usl_bcm_port_rate_limit_t *bcast_limit;
  int                        rv;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  bcast_limit = (usl_bcm_port_rate_limit_t *)args;
  
  rv = usl_bcm_port_rate_bcast_set(unit, port, bcast_limit);

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the Multicast rate threshold for a port
*
*
* @param    port           @{(input)} Lport 
* @param    mcast_limit    @{(input)} Mcast rate threshold limits
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_rate_mcast_set(bcmx_lport_t port, 
                                 usl_bcm_port_rate_limit_t mcast_limit)
{
  int     rv = BCM_E_NONE;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(mcast_limit) > sizeof(args))
  {
    LOG_ERROR(sizeof(mcast_limit));
  }

  memcpy(args, &mcast_limit, sizeof(mcast_limit));

  n_args = sizeof(mcast_limit)/sizeof(uint32);
  if (sizeof(mcast_limit)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_MCAST_RATE_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_MCAST_RATE_SET, args);
  #endif

  return rv;
}


/*********************************************************************
*
* @purpose RPC Server API to set the multicast rate threshold for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} Mcast rate threshold data
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_rate_mcast_set (int unit, bcm_port_t port, 
                                  int setget, uint32 *args)
{
  usl_bcm_port_rate_limit_t *mcast_limit;
  int                        rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  mcast_limit = (usl_bcm_port_rate_limit_t *)args;
  
  rv = usl_bcm_port_rate_mcast_set(unit, port, mcast_limit);

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the DLF rate threshold for a port
*
*
* @param    port           @{(input)} Lport 
* @param    dlf_limit      @{(input)} Dlf rate threshold limits
*
* @returns BCMX Error Code
*
* @notes    Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_rate_dlfbc_set(bcmx_lport_t port, 
                                 usl_bcm_port_rate_limit_t dlf_limit)
{
  int     rv = BCM_E_NONE;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(dlf_limit) > sizeof(args))
  {
    LOG_ERROR(sizeof(dlf_limit));
  }

  memcpy(args, &dlf_limit, sizeof(dlf_limit));

  n_args = sizeof(dlf_limit)/sizeof(uint32);
  if (sizeof(dlf_limit)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_DLF_RATE_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_DLF_RATE_SET, args);
  #endif

  return rv;
}


/*********************************************************************
*
* @purpose RPC Server API to set the dlf rate threshold for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} Mcast rate threshold data
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_rate_dlfbc_set (int unit, bcm_port_t port, 
                                  int setget, uint32 *args)
{
  usl_bcm_port_rate_limit_t *dlf_limit;
  int                        rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  dlf_limit = (usl_bcm_port_rate_limit_t *)args;

  rv = usl_bcm_port_rate_dlfbc_set(unit, port, dlf_limit);

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the Ingress filtering mode for a port
*
* @param   port    -  The LPORT 
* @param   mode    -  Filtering mode
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_member_set(bcmx_lport_t port, 
                                       usl_bcm_port_filter_mode_t mode)
{
  int     rv = BCM_E_NONE;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(mode) > sizeof(args))
  {
    LOG_ERROR(sizeof(mode));
  }

  memcpy(args, &mode, sizeof(mode));

  n_args = sizeof(mode)/sizeof(uint32);
  if (sizeof(mode)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_FILTER_MODE_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_FILTER_MODE_SET, args);
  #endif

  return rv;

}

/*********************************************************************
*
* @purpose RPC Server API to set the filtering mode for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} Filter mode data
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_vlan_member_set (int unit, bcm_port_t port, 
                                        int setget, uint32 *args)
{
  usl_bcm_port_filter_mode_t   *mode;
  int                           rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  mode = (usl_bcm_port_filter_mode_t *)args;

  rv = usl_bcm_port_vlan_member_set(unit, port, mode);

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the default priority for a port
*
* @param   port        -  The LPORT 
* @param   priority    -  Deafult priority for the port
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_untagged_priority_set(bcmx_lport_t port, 
                                             usl_bcm_port_priority_t priority)
{
  int     rv = BCM_E_NONE;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(priority) > sizeof(args))
  {
    LOG_ERROR(sizeof(priority));
  }

  memcpy(args, &priority, sizeof(priority));

  n_args = sizeof(priority)/sizeof(uint32);
  if (sizeof(priority)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_PRIORITY_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_PRIORITY_SET, args);
  #endif

  return rv;

}

/*********************************************************************
*
* @purpose RPC Server API to set the priority for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} priority data
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_untagged_priority_set (int unit, bcm_port_t port, 
                                              int setget, uint32 *args)
{
  usl_bcm_port_priority_t      *priority;
  int                            rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  priority = (usl_bcm_port_priority_t *)args;

  rv = usl_bcm_port_untagged_priority_set(unit, port, priority);

  return rv;

}

/*********************************************************************
*
* @purpose RPC Client API to set the max frame size allowed on a port
*
* @param   port           -  The LPORT 
* @param   maxFrameSize   -  Max frame size data
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_frame_max_set(bcmx_lport_t port, 
                                     usl_bcm_port_frame_size_t max_frame_size)
{
  int     rv = BCM_E_NONE;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(max_frame_size) > sizeof(args))
  {
    LOG_ERROR(sizeof(max_frame_size));
  }

  memcpy(args, &max_frame_size, sizeof(max_frame_size));

  n_args = sizeof(max_frame_size)/sizeof(uint32);
  if (sizeof(max_frame_size)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_MAX_FRAME_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_MAX_FRAME_SET, args);
  #endif

  return rv;

}

/*********************************************************************
*
* @purpose RPC Server API to set the max frame size for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} priority data
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_frame_max_set(int unit, bcm_port_t port, 
                                     int setget, uint32 *args)
{
  usl_bcm_port_frame_size_t    *max_frame_size;
  int                            rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  max_frame_size = (usl_bcm_port_frame_size_t *)args;

  rv = usl_bcm_port_frame_max_set(unit, port, max_frame_size);

  return rv;

}

/*********************************************************************
*
* @purpose RPC Client API to set the learn mode for a port
*
* @param   port           -  The LPORT 
* @param   learnMode      -  Learn mode of the port
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_learn_set(bcmx_lport_t port, 
                                 usl_bcm_port_learn_mode_t learn_mode)
{
  int     rv = BCM_E_NONE;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(learn_mode) > sizeof(args))
  {
    LOG_ERROR(sizeof(learn_mode));
  }

  memcpy(args, &learn_mode, sizeof(learn_mode));

  n_args = sizeof(learn_mode)/sizeof(uint32);
  if (sizeof(learn_mode)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_LEARN_MODE_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_LEARN_MODE_SET, args);
  #endif

  return rv;

}

/*********************************************************************
*
* @purpose RPC Server API to set the learn mode for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} priority data
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_learn_set(int unit, bcm_port_t port, 
                                 int setget, uint32 *args)
{
  usl_bcm_port_learn_mode_t    *learn_mode;
  int                            rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  learn_mode = (usl_bcm_port_learn_mode_t *)args;

  rv = usl_bcm_port_learn_set(unit, port, learn_mode);

  return rv;

}

/*********************************************************************
*
* @purpose RPC Client API to set the dtag mode for a port
*
* @param   port           -  The LPORT 
* @param   dtagMode       -  Learn mode
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_dtag_mode_set(bcmx_lport_t port, 
                                usl_bcm_port_dtag_mode_t dtag_mode)
{
  int     rv = BCM_E_NONE;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(dtag_mode) > sizeof(args))
  {
    LOG_ERROR(sizeof(dtag_mode));
  }

  memcpy(args, &dtag_mode, sizeof(dtag_mode));

  n_args = sizeof(dtag_mode)/sizeof(uint32);
  if (sizeof(dtag_mode)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_DTAG_MODE_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_DTAG_MODE_SET, args);
  #endif

  return rv;

}


/*********************************************************************
*
* @purpose RPC Server API to set the dtag mode for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} priority data
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_dtag_mode_set(int unit, bcm_port_t port, 
                                     int setget, uint32 *args)
{
  usl_bcm_port_dtag_mode_t     *dtag_mode;
  int                            rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  dtag_mode = (usl_bcm_port_dtag_mode_t *)args;

  rv = usl_bcm_port_dtag_mode_set(unit, port, dtag_mode);

  return rv;

}


/*********************************************************************
*
* @purpose RPC Client API to set the dtag tpid for a port
*
* @param   port           -  The LPORT 
* @param   tpid           -  tpid for the port
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_tpid_set(bcmx_lport_t port, usl_bcm_port_tpid_t tpid)
{
  int     rv = BCM_E_NONE;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(tpid) > sizeof(args))
  {
    LOG_ERROR(sizeof(tpid));
  }

  memcpy(args, &tpid, sizeof(tpid));

  n_args = sizeof(tpid)/sizeof(uint32);
  if (sizeof(tpid)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_TPID_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_TPID_SET, args);
  #endif

  return rv;

}

/*********************************************************************
*
* @purpose RPC Client API to add a dtag tpid for a port
*
* @param   port           -  The LPORT 
* @param   tpid           -  tpid for the port
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_tpid_add(bcmx_lport_t port, usl_bcm_port_tpid_t tpid)
{
  int     rv = BCM_E_NONE;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(tpid) > sizeof(args))
  {
    LOG_ERROR(sizeof(tpid));
  }

  memcpy(args, &tpid, sizeof(tpid));

  n_args = sizeof(tpid)/sizeof(uint32);
  if (sizeof(tpid)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_TPID_ADD, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_TPID_ADD, args);
  #endif

  return rv;

}

/*********************************************************************
*
* @purpose RPC Client API to delete a dtag tpid from a port
*
* @param   port           -  The LPORT 
* @param   tpid           -  tpid for the port
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_tpid_delete(bcmx_lport_t port, usl_bcm_port_tpid_t tpid)
{
  int     rv = BCM_E_NONE;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(tpid) > sizeof(args))
  {
    LOG_ERROR(sizeof(tpid));
  }

  memcpy(args, &tpid, sizeof(tpid));

  n_args = sizeof(tpid)/sizeof(uint32);
  if (sizeof(tpid)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_TPID_DELETE, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_TPID_DELETE, args);
  #endif

  return rv;

}

/*********************************************************************
*
* @purpose RPC Server API to set the tpid for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} priority data
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_tpid_set(int unit, bcm_port_t port, 
                                int setget, uint32 *args)
{
  usl_bcm_port_tpid_t          *tpid;
  int                            rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  tpid = (usl_bcm_port_tpid_t *)args;

  rv = usl_bcm_port_tpid_set(unit, port, tpid);

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to add a tpid for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} priority data
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_tpid_add(int unit, bcm_port_t port, 
                                int setget, uint32 *args)
{
  usl_bcm_port_tpid_t          *tpid;
  int                            rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  tpid = (usl_bcm_port_tpid_t *)args;

  rv = usl_bcm_port_tpid_add(unit, port, tpid);

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to delete a tpid from a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} priority data
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_tpid_delete(int unit, bcm_port_t port, 
                                  int setget, uint32 *args)
{
  usl_bcm_port_tpid_t          *tpid;
  int                            rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  tpid = (usl_bcm_port_tpid_t *)args;

  rv = usl_bcm_port_tpid_delete(unit, port, tpid);

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the default vid for a port
*
*
* @param    port           @{(input)} Lport 
* @param    vid            @{(input)} Default vid
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_untagged_vlan_set(bcmx_lport_t port, bcm_vlan_t vid)
{
  int     rv = BCM_E_NONE;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(vid) > sizeof(args))
  {
    LOG_ERROR(sizeof(vid));
  }

  memcpy(args, &vid, sizeof(vid));

  n_args = sizeof(vid)/sizeof(uint32);
  if (sizeof(vid)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_PVID_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_PVID_SET, args);
  #endif

  return rv;

}

/*********************************************************************
*
* @purpose RPC Server API to set the default vid for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} pvid data
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_untagged_vlan_set(int unit, bcm_port_t port, 
                                         int setget, uint32 *args)
{
  bcm_vlan_t                    *pvid;
  int                            rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  pvid = (bcm_vlan_t *)args;

  rv = usl_bcm_port_untagged_vlan_set(unit, port, pvid);

  return rv;
}


/*********************************************************************
*
* @purpose RPC Client API to set the discard mode for a port
*
*
* @param    port           @{(input)} Lport 
* @param    mode           @{(input)} Discard mode
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_discard_set(bcmx_lport_t port, bcm_port_discard_t mode)
{
  int     rv = BCM_E_NONE;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(mode) > sizeof(args))
  {
    LOG_ERROR(sizeof(mode));
  }

  memcpy(args, &mode, sizeof(mode));

  n_args = sizeof(mode)/sizeof(uint32);
  if (sizeof(mode)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_DISCARD_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_DISCARD_SET, args);
  #endif

  return rv;

}

/*********************************************************************
*
* @purpose RPC Server API to set the discard mode for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} discard mode data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_discard_set(int unit, bcm_port_t port, 
                                   int setget, uint32 *args)
{
  bcm_port_discard_t            *mode;
  int                            rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  mode = (bcm_port_discard_t *)args;

  rv = usl_bcm_port_discard_set(unit, port, mode);

  return rv;

}

/*********************************************************************
*
* @purpose RPC Client API to set the phy medium config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    medium         @{(input)} Medium of the phy
* @param    config         @{(input)} Medium configuration
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_medium_config_set(bcmx_lport_t port,
                                         bcm_port_medium_t medium,
                                         bcm_phy_config_t  *config)
{
  int        rv = BCM_E_NONE;
  uint32     args[BCM_CUSTOM_ARGS_MAX];
  L7_uchar8 *argPtr;
  uint32     n_args;

  if ((sizeof(medium) + sizeof(*config)) > sizeof(args))
  {
    LOG_ERROR(sizeof(medium) + sizeof(*config));
  }

  argPtr = (L7_uchar8 *)args;

  memcpy(argPtr, &medium, sizeof(medium));
  argPtr += sizeof(medium);

  memcpy(argPtr, config, sizeof(*config));
  argPtr += sizeof(*config);


  n_args = (sizeof(medium)+sizeof(*config))/sizeof(uint32);
  if ((sizeof(medium)+sizeof(*config))%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_PHY_CONFIG_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_PHY_CONFIG_SET, args);
  #endif

  return rv;

}


/*********************************************************************
*
* @purpose RPC Server API to set the phy medium config for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} phy config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_medium_config_set(int unit, bcm_port_t port, 
                                         int setget, uint32 *args)
{
  bcm_port_medium_t            medium;
  bcm_phy_config_t             config;
  int                          rv = BCM_E_NONE;
  L7_uchar8                   *argsPtr;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  argsPtr = (L7_uchar8 *)args;

  memcpy(&medium, argsPtr, sizeof(medium));
  argsPtr += sizeof(medium);

  memcpy(&config, argsPtr, sizeof(config));
  argsPtr += sizeof(config);

  rv = usl_bcm_port_medium_config_set(unit, port, medium, &config);

  return rv;
}


/*********************************************************************
*
* @purpose RPC Client API to set the flow-control config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    pauseConfig    @{(input)} Pause configuration for the port
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_flow_control_set(bcmx_lport_t port, 
                                        usl_bcm_port_pause_config_t pauseConfig)
{
  int        rv = BCM_E_NONE;
  uint32     args[BCM_CUSTOM_ARGS_MAX];
  uint32     n_args;

  if (sizeof(pauseConfig) > sizeof(args))
  {
    LOG_ERROR(sizeof(pauseConfig));
  }

  memcpy(args, &pauseConfig, sizeof(pauseConfig));

  n_args = sizeof(pauseConfig)/sizeof(uint32);
  if (sizeof(pauseConfig)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_FLOW_CONTROL_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_FLOW_CONTROL_SET, args);
  #endif

  return rv;

}

/*********************************************************************
*
* @purpose RPC Server API to set the flow control settings for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} flow-control data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_flow_control_set(int unit, bcm_port_t port, 
                                        int setget, uint32 *args)
{
  usl_bcm_port_pause_config_t  *pauseConfig;
  int                           rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  pauseConfig = (usl_bcm_port_pause_config_t *)args;

  rv = usl_bcm_port_flow_control_set(unit, port, pauseConfig);

  return rv;

}

/*********************************************************************
*
* @purpose RPC Client API to set the Cos queue sched config for a port
*
*
* @param    port               @{(input)} Lport 
* @param    cosqSchedConfig    @{(input)} Cosq Sched configuration for the port
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_cosq_sched_set(bcmx_lport_t port, 
                                      usl_bcm_port_cosq_sched_config_t cosqSchedConfig)
{
  int        rv = BCM_E_NONE;
  uint32     args[BCM_CUSTOM_ARGS_MAX];
  uint32     n_args;

  if (sizeof(cosqSchedConfig) > sizeof(args))
  {
    LOG_ERROR(sizeof(cosqSchedConfig));
  }

  memcpy(args, &cosqSchedConfig, sizeof(cosqSchedConfig));

  n_args = sizeof(cosqSchedConfig)/sizeof(uint32);
  if (sizeof(cosqSchedConfig)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_COSQ_SCHED_CONFIG_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_COSQ_SCHED_CONFIG_SET, args);
  #endif

  return rv;

}

/*********************************************************************
*
* @purpose RPC Server API to set the cosq sched config for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_cosq_sched_set(int unit, bcm_port_t port, 
                                      int setget, uint32 *args)
{
  usl_bcm_port_cosq_sched_config_t  *cosqSchedConfig;
  int                                rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  cosqSchedConfig = (usl_bcm_port_cosq_sched_config_t *)args;

  rv = usl_bcm_port_cosq_sched_set(unit, port, cosqSchedConfig);

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the rate shaper config for a port
*
*
* @param    port               @{(input)} Lport 
* @param    shaperConfig       @{(input)} Rate shaper configuration for the port
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_rate_egress_set(bcmx_lport_t port, 
                                       usl_bcm_port_shaper_config_t shaperConfig)
{
  int        rv = BCM_E_NONE;
  uint32     args[BCM_CUSTOM_ARGS_MAX];
  uint32     n_args;

  if (sizeof(shaperConfig) > sizeof(args))
  {
    LOG_ERROR(sizeof(shaperConfig));
  }

  memcpy(args, &shaperConfig, sizeof(shaperConfig));

  n_args = sizeof(shaperConfig)/sizeof(uint32);
  if (sizeof(shaperConfig)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_SHAPER_CONFIG_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_SHAPER_CONFIG_SET, args);
  #endif

  return rv;

}

/*********************************************************************
*
* @purpose RPC Server API to set the rate shaper config for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_rate_egress_set(int unit, bcm_port_t port, 
                                       int setget, uint32 *args)
{
  usl_bcm_port_shaper_config_t      *shaperConfig;
  int                                rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  shaperConfig = (usl_bcm_port_shaper_config_t *)args;

  rv = usl_bcm_port_rate_egress_set(unit, port, shaperConfig);

  return rv;

}

/*********************************************************************
*
* @purpose RPC Client API to add/remove port to/from vlans 
*
*
* @param    port               @{(input)} Lport 
* @param    vlanConfig         @{(input)} Vlan configuration
* @param    cmd                @{(input)} L7_TRUE: Add ports to vlan
*                                         L7_FALSE: Remove ports from vlan
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX 
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_config(bcmx_lport_t port, 
                                   usl_bcm_port_vlan_t *vlanConfig, 
                                   L7_BOOL cmd)
{
  int        rv;
  uint32     args[BCM_CUSTOM_ARGS_MAX];
  L7_uchar8 *argPtr;
  uint32     n_args;

  if ((sizeof (*vlanConfig) + sizeof(cmd)) > sizeof (args))
  {
    LOG_ERROR(sizeof (*vlanConfig) + sizeof(cmd));
  }

  argPtr = (L7_char8 *) args;

  memcpy(argPtr, &cmd, sizeof(cmd));
  argPtr += sizeof(cmd);

  memcpy (argPtr, vlanConfig, sizeof (*vlanConfig));
  argPtr += sizeof(*vlanConfig);


  n_args = (sizeof(cmd)+sizeof(*vlanConfig))/sizeof(uint32);
  if ((sizeof(cmd)+sizeof(*vlanConfig))%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_VLAN_CONFIG_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_VLAN_CONFIG_SET, args);
  #endif

  return rv;

}


/*********************************************************************
*
* @purpose RPC Server API to add/remove port to/from vlans
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} vlan config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_vlan_config(int unit, bcm_port_t port, 
                                   int setget, uint32 *args)
{
  L7_uchar8                  *argPtr;
  L7_BOOL                     cmd;
  usl_bcm_port_vlan_t        *vlanConfig;
  int                         rv;

  if (setget != BCM_CUSTOM_SET) 
  {
    return BCM_E_PARAM;
  }

  argPtr = (L7_uchar8 *)args;

  memcpy(&cmd, argPtr, sizeof(cmd));
  argPtr += sizeof(cmd);

  vlanConfig = (usl_bcm_port_vlan_t *)argPtr;

  rv = usl_bcm_port_vlan_config(unit, port, vlanConfig, cmd);

  return BCM_E_NONE;
}

/*********************************************************************
*
* @purpose RPC Client API to Set the spanning-tree state for a port/stg
*
*
* @param    stg                @{(input)}  Stg id
* @param    port               @{(input)} Lport 
* @param    stpState           @{(input)}  State configuration
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_stg_stp_set(bcm_stg_t stg, bcmx_lport_t port,
                              bcm_stg_stp_t stpState)
{
  int        rv;
  uint32     args[BCM_CUSTOM_ARGS_MAX];
  L7_uchar8 *argPtr;
  uint32     n_args;

  if ((sizeof (stg) + sizeof(stpState)) > sizeof (args))
  {
    LOG_ERROR(sizeof (stg) + sizeof(stpState));
  }

  argPtr = (L7_char8 *) args;

  memcpy(argPtr, &stg, sizeof(stg));
  argPtr += sizeof(stg);

  memcpy (argPtr, &stpState, sizeof (stpState));
  argPtr += sizeof(stpState);


  n_args = (sizeof(stg)+sizeof(stpState))/sizeof(uint32);
  if ((sizeof(stg)+sizeof(stpState))%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_STG_STATE_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_STG_STATE_SET, args);
  #endif

  return rv;

}


/*********************************************************************
*
* @purpose RPC Server API to set the rate shaper config for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_stg_stp_set(int unit, bcm_port_t port, 
                              int setget, uint32 *args)
{
  bcm_stg_t                          stg;
  bcm_stg_stp_t                      stpState;
  int                                rv = BCM_E_NONE;
  L7_uchar8                         *argPtr;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  argPtr = (L7_uchar8 *)args;

  memcpy(&stg, argPtr, sizeof(stg));
  argPtr += sizeof(stg);

  memcpy(&stpState, argPtr, sizeof(stpState));
  argPtr += sizeof(stpState);

  rv = usl_bcm_stg_stp_set(unit, port, stg, stpState);

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to configure protocol based vlans on a port 
*
*
* @param    port               @{(input)} Lport 
* @param    pbvlanConfig       @{(input)} protocol Vlan configuration
* @param    cmd                @{(input)} L7_TRUE: Add pbvlan config
*                                         L7_FALSE: Remove pbvlan config
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_protocol_vlan_config(bcmx_lport_t port, 
                                            usl_bcm_port_pbvlan_config_t pbvlanConfig, 
                                            L7_BOOL cmd)
{
  int              rv;
  uint32           args[BCM_CUSTOM_ARGS_MAX];
  L7_uchar8       *argPtr;
  uint32           n_args;

  if ((sizeof (pbvlanConfig) + sizeof(cmd)) > sizeof (args))
  {
    LOG_ERROR(sizeof (pbvlanConfig) + sizeof(cmd));
  }

  argPtr = (L7_char8 *) args;

  memcpy(argPtr, &cmd, sizeof(cmd));
  argPtr += sizeof(cmd);

  memcpy (argPtr, &pbvlanConfig, sizeof (pbvlanConfig));
  argPtr += sizeof(pbvlanConfig);


  n_args = (sizeof(cmd)+sizeof(pbvlanConfig))/sizeof(uint32);
  if ((sizeof(cmd)+sizeof(pbvlanConfig))%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_PBVLAN_CONFIG_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_PBVLAN_CONFIG_SET, args);
  #endif

  return rv;

}

/*********************************************************************
*
* @purpose RPC Server API to protocol based vlan config for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_protocol_vlan_config(int unit, bcm_port_t port, 
                                            int setget, uint32 *args)
{
  L7_BOOL                            cmd;
  L7_uchar8                         *argPtr;
  int                                rv = BCM_E_NONE;
  usl_bcm_port_pbvlan_config_t      *pbvlanConfig;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  argPtr = (L7_uchar8 *)args;

  memcpy(&cmd, argPtr, sizeof(cmd));
  argPtr += sizeof(cmd);

  pbvlanConfig = (usl_bcm_port_pbvlan_config_t *)argPtr;

  rv = usl_bcm_port_protocol_vlan_config(unit, port, pbvlanConfig, cmd);

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the dot1x state for the port
*
*
* @param    port               @{(input)} Lport 
* @param    dot1xStatus        @{(input)} Dot1x state for the port
*
* @returns BCMX Error Code
*
* @notes    Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_dot1x_config(bcmx_lport_t port,
                                    L7_DOT1X_PORT_STATUS_t dot1xStatus)
{
  int              rv;
  uint32           args[BCM_CUSTOM_ARGS_MAX];
  uint32           n_args;

  if (sizeof(dot1xStatus) > sizeof (args))
  {
    LOG_ERROR(sizeof(dot1xStatus));
  }

  memcpy (args, &dot1xStatus, sizeof (dot1xStatus));

  n_args = sizeof(dot1xStatus)/sizeof(uint32);
  if (sizeof(dot1xStatus)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_DOT1X_CONFIG_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_DOT1X_CONFIG_SET, args);
  #endif

  return rv;

}

/*********************************************************************
*
* @purpose RPC Server API to to set the dot1x state for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_dot1x_config(int unit, bcm_port_t port,
                                    int setget, uint32 *args)
{
  int                                rv = BCM_E_NONE;
  L7_DOT1X_PORT_STATUS_t            *dot1xState;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  dot1xState = (L7_DOT1X_PORT_STATUS_t *) args;

  rv = usl_bcm_port_dot1x_config(unit, port, *dot1xState); 

  return rv;
}

/*********************************************************************
 *
 * @purpose RPC Client API to get SFP diagnostics for the specified port.
 *
 * @param   port - BCMX Lport
 * @param
 *
 * @returns BCMX Error Code
 *
 * @notes   Called from USL BCMX API
 *
 * @end
 *
 *********************************************************************/
int
l7_rpc_client_port_sfp_diag_get(bcmx_lport_t port,
                                int32 *temperature,
                                uint32 *voltage,
                                uint32 *current,
                                uint32 *txPower,
                                uint32 *rxPower,
                                uint32 *txFault,
                                uint32 *los)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  L7_int len;
  rv = bcmx_custom_port_get(port, USL_BCMX_PORT_SFP_DIAG_GET, BCM_CUSTOM_ARGS_MAX, args, &len);
  #else
  rv = bcmx_custom_port_get(port, USL_BCMX_PORT_SFP_DIAG_GET, args);
  #endif

  if (rv >= 0) {
    *temperature = args[0];
    *voltage     = args[1];
    *current     = args[2];
    *txPower     = args[3];
    *rxPower     = args[4];
    *txFault     = args[5];
    *los         = args[6];
  }
  return rv;
}

/*********************************************************************
 *
 * @purpose RPC Client API to get copper diagnostics for the specified port.
 *
 * @param   port - BCMX Lport
 * @param
 *
 * @returns BCMX Error Code
 *
 * @notes   Called from USL BCMX API
 *
 * @end
 *
 *********************************************************************/
int
l7_rpc_client_port_copper_diag_get(bcmx_lport_t port, bcm_port_cable_diag_t *cd)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  L7_int len;
  rv = bcmx_custom_port_get(port, USL_BCMX_PORT_COPPER_DIAG_GET, BCM_CUSTOM_ARGS_MAX, args, &len);
  #else
  rv = bcmx_custom_port_get(port, USL_BCMX_PORT_COPPER_DIAG_GET, args);
  #endif

  if (rv >= 0) 
  {
    memcpy(cd, args, sizeof(bcm_port_cable_diag_t));
  }
  return rv;
}

/*********************************************************************
 *
 * @purpose RPC Server API to SFP diag Handler.
 *
 * @param
 *
 * @returns BCM Error Code
 *
 * @notes   Remote handler
 *
 * @end
 *
 *********************************************************************/
int l7_rpc_server_port_sfp_diag_get(int unit, bcm_port_t port, int setget,
                                    uint32 *args)
{
  int     rv = 0;
  int32   temperature;
  uint32  voltage;
  uint32  current;
  uint32  txPower;
  uint32  rxPower;
  uint32  txFault;
  uint32  los;

  if (sysapiHpcDiagTempRead(unit, port, &temperature) != L7_SUCCESS)
  {
    rv = BCM_E_FAIL;
  }
  else
  {
    args[0] = temperature;
  }

  if (sysapiHpcDiagVoltageRead(unit, port, &voltage) != L7_SUCCESS)
  {
    rv = BCM_E_FAIL;
  }
  else
  {
    args[1] = voltage;
  }

  if (sysapiHpcDiagCurrentRead(unit, port, &current) != L7_SUCCESS)
  {
    rv = BCM_E_FAIL;
  }
  else
  {
    args[2] = current;
  }

  if (sysapiHpcDiagTxPwrRead(unit, port, &txPower) != L7_SUCCESS)
  {
    rv = BCM_E_FAIL;
  }
  else
  {
    args[3] = txPower;
  }

  if (sysapiHpcDiagRxPwrRead(unit, port, &rxPower) != L7_SUCCESS)
  {
    rv = BCM_E_FAIL;
  }
  else
  {
    args[4] = rxPower;
  }

  if (sysapiHpcDiagTxFaultRead(unit, port, &txFault) != L7_SUCCESS)
  {
    rv = BCM_E_FAIL;
  }
  else
  {
    args[5] = txFault;
  }

  if (sysapiHpcDiagLosRead(unit, port, &los) != L7_SUCCESS)
  {
    rv = BCM_E_FAIL;
  }
  else
  {
    args[6] = los;
  }

  return rv;
}

/*********************************************************************
 *
 * @purpose RPC Server API to copper diag Handler.
 *
 * @param
 *
 * @returns BCM Error Code
 *
 * @notes   Remote handler
 *
 * @end
 *
 *********************************************************************/
int l7_rpc_server_port_copper_diag_get(int unit, bcm_port_t port, int setget, uint32 *args)
{
  int                   rv = BCM_E_NONE;
  L7_RC_t               result;
  bcm_pbmp_t            pbm;
  bcm_port_cable_diag_t cd;
  bcm_port_medium_t     portMedium = BCM_PORT_MEDIUM_COPPER;

  BCM_PBMP_PORT_SET(pbm, port);

  do
  {
    /* disable link scan during diag so applications don't get confused */
    (void)bcm_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_NONE);

    (void)bcm_port_medium_get(unit, port, &portMedium);

    if(portMedium != BCM_PORT_MEDIUM_COPPER)
    {
      portMedium = BCM_PORT_MEDIUM_FIBER;
    }

    /* SFP... possibly copper SFP. */
    if(portMedium == BCM_PORT_MEDIUM_FIBER)
    {
      result = sysapiHpcDiagCopperSfpRead(unit, port, &cd);
      if (result == L7_SUCCESS)
      {
        rv = BCM_E_NONE;
      }
      else if (result == L7_NOT_SUPPORTED)
      {
        rv = BCM_E_UNAVAIL;
      }
      else
      {
        rv = BCM_E_FAIL;
      }
    }
    else if(portMedium == BCM_PORT_MEDIUM_COPPER)
    {
      /* run diag */
      /* cable diags only operate with the ports in GigE mode */
      rv = bcm_port_cable_diag(unit, port, &cd);
    }


    /* re-enable link scan */
    (void)bcm_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_SW);

    memcpy(args, &cd, sizeof(bcm_port_cable_diag_t));

  } while (0);

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to block an unauthorized dot1x client for the specified port.
*
* @param   port - BCMX Lport
* @param   client_cmd - Mac address, vlan Id 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int
l7_rpc_client_port_dot1x_client_block(bcmx_lport_t port,
                                      usl_bcm_port_dot1x_client_t *client_cmd)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(usl_bcm_port_dot1x_client_t) > sizeof(args)) 
  {
      LOG_ERROR(sizeof(usl_bcm_port_dot1x_client_t));
  }

  memcpy (args, client_cmd, sizeof (usl_bcm_port_dot1x_client_t));

  n_args = sizeof(usl_bcm_port_dot1x_client_t)/sizeof(uint32);
  if (sizeof(usl_bcm_port_dot1x_client_t)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_DOT1X_CLIENT_BLOCK, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_DOT1X_CLIENT_BLOCK, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to unblock an unauthorized dot1x client for the specified port.
*
* @param   port - BCMX Lport
* @param   client_cmd - Mac address, vlan Id 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int
l7_rpc_client_port_dot1x_client_unblock(bcmx_lport_t port,
                                        usl_bcm_port_dot1x_client_t *client_cmd)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(usl_bcm_port_dot1x_client_t) > sizeof(args)) 
  {
      LOG_ERROR(sizeof(usl_bcm_port_dot1x_client_t));
  }

  memcpy (args, client_cmd, sizeof (usl_bcm_port_dot1x_client_t));

  n_args = sizeof(usl_bcm_port_dot1x_client_t)/sizeof(uint32);
  if (sizeof(usl_bcm_port_dot1x_client_t)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_DOT1X_CLIENT_UNBLOCK, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_DOT1X_CLIENT_UNBLOCK, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to block dot1x client
*
* @param
*
* @returns BCM Error Code
*
* @notes   Remote handler
*
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_dot1x_client_block(int unit, bcm_port_t port,
                                          int setget, uint32 *args)
{
  int                          rv;
  usl_bcm_port_dot1x_client_t *pClientCmd;

  if (setget != BCM_CUSTOM_SET) 
  {
    return BCM_E_PARAM;
  }

  pClientCmd = (usl_bcm_port_dot1x_client_t *) args;

  rv = usl_bcm_port_dot1x_client_block(unit, port, pClientCmd);

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to unblock dot1x client
*
* @param
*
* @returns BCM Error Code
*
* @notes   Remote handler
*
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_dot1x_client_unblock(int unit, bcm_port_t port,
                                            int setget, uint32 *args)
{
  int                          rv;
  usl_bcm_port_dot1x_client_t *pClientCmd;

  if (setget != BCM_CUSTOM_SET) 
  {
    return BCM_E_PARAM;
  }

  pClientCmd = (usl_bcm_port_dot1x_client_t *) args;

  rv = usl_bcm_port_dot1x_client_unblock(unit, port, pClientCmd);

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to get dot1x client timeout for the specified port.
*
* @param   port - BCMX Lport
* @param   client_cmd - Mac address, vlan Id 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int
l7_rpc_client_port_dot1x_client_timeout_get(bcmx_lport_t port,
                                            usl_bcm_port_dot1x_client_t *client_cmd)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  usl_bcm_port_dot1x_client_t *pReturnClient;

  if (sizeof(usl_bcm_port_dot1x_client_t) > sizeof(args)) 
  {
    LOG_ERROR(sizeof(usl_bcm_port_dot1x_client_t));
  }

  memcpy (args, client_cmd, sizeof (usl_bcm_port_dot1x_client_t));

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  L7_int len;
  rv = bcmx_custom_port_get(port, USL_BCMX_DOT1X_CLIENT_TIMEOUT_GET, BCM_CUSTOM_ARGS_MAX, args, &len);
  #else
  rv = bcmx_custom_port_get(port, USL_BCMX_DOT1X_CLIENT_TIMEOUT_GET, args);
  #endif

  if (rv >= 0) 
  {
    pReturnClient = (usl_bcm_port_dot1x_client_t *) args;
    client_cmd->timedout_flg = pReturnClient->timedout_flg;
    
  }

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to get dot1x client timeout
*
* @param
*
* @returns BCM Error Code
*
* @notes   Remote handler
*
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_dot1x_client_timeout_get(int unit, bcm_port_t port, 
                                                int setget, uint32 *args)
{
  int     rv;
  usl_bcm_port_dot1x_client_t *pClientCmd;
  bcm_l2_addr_t  l2addr;

  if (setget != BCM_CUSTOM_GET) 
  {
    return BCM_E_PARAM;
  }

  pClientCmd = (usl_bcm_port_dot1x_client_t *) args;

  rv = bcm_l2_addr_get(unit, pClientCmd->mac_addr, pClientCmd->vlan_id , &l2addr);
  if (rv == BCM_E_NONE) 
  {
        
    if (l2addr.flags & BCM_L2_SRC_HIT) 
    {
      /* Clear the HIT bit and add the entry on this unit */
      l2addr.flags &= ~(BCM_L2_SRC_HIT);
      (void)bcm_l2_addr_add(unit, &l2addr);
      pClientCmd->timedout_flg = L7_FALSE;
    }
    else
    {
      pClientCmd->timedout_flg = L7_TRUE;
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to get all statistics for the specified port.
*
* @param   port - BCMX Lport
* @param   stats - 64-bit stats for the port.
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int
l7_rpc_client_stat_get(bcmx_lport_t port, uint64 stats[snmpValCount])
{
  int     rv, argi, s;
  uint32  args[BCM_CUSTOM_ARGS_MAX];

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  L7_int len;
  rv = bcmx_custom_port_get(port, USL_BCMX_STATS_GET, BCM_CUSTOM_ARGS_MAX, args, &len);
  #else
  rv = bcmx_custom_port_get(port, USL_BCMX_STATS_GET, args);
  #endif

  if (rv >= 0) 
  {
    argi = 0;
    for (s = 0; s < snmpValCount; s++) 
    {
      COMPILER_64_SET(stats[s], args[argi], args[argi+1]);
      argi += 2;
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to get all statistics for the specified port.
*
* @param   port - BCMX Lport
* @param   stats - 64-bit stats for the port.
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int
l7_rpc_client_port_stat_get(bcmx_lport_t port, uint64 stats[snmpValCount])
{
  int     rv, argi, s;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
 
  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  L7_int len;
  rv = bcmx_custom_port_get(port, USL_BCMX_PORT_STATS_GET, BCM_CUSTOM_ARGS_MAX, args, &len);
  #else
  rv = bcmx_custom_port_get(port, USL_BCMX_PORT_STATS_GET, args);
  #endif

  if (rv >= 0) 
  {
    argi = 0;
    for (s = 0; s < snmpValCount; s++) 
    {
      COMPILER_64_SET(stats[s], args[argi], args[argi+1]);
      argi += 2;
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to get Stats
*
* @param
*
* @returns BCM Error Code
*
* @notes   Remote handler
*
*
* @end
*
*********************************************************************/
int l7_rpc_server_stat_get(int unit, bcm_port_t port, 
                           int setget, uint32 *args)
{
  int     rv = 0, argi, s;
  uint64  val;

  if (setget != BCM_CUSTOM_GET) 
  {
    return BCM_E_PARAM;
  }

  argi = 0;
  for (s = 0; s < snmpValCount; s++) 
  {
    rv = bcm_stat_get(unit, port, s, &val);
    if (rv < 0) 
    {
      /* Not all stats may be supported on the port. Return zeroes for unsupported
      ** stats and set return code to OK.
      */
      rv = BCM_E_NONE;
      memset(&val, 0, sizeof (val));

    }

    if (argi >= BCM_CUSTOM_ARGS_MAX-2) 
    {
      break;
    }

    args[argi++] = COMPILER_64_HI(val);
    args[argi++] = COMPILER_64_LO(val);
  }

  /* clear remaining arguments */
  memset(&args[argi], 0, sizeof(uint32) * (BCM_CUSTOM_ARGS_MAX-argi));

  return rv;
}
/*********************************************************************
*
* @purpose Stats Handler. 
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*   Get all of a port's stats in one call.
*   This is the custom handler that runs on the slave devices.
*   See companion calling routine custom_stat_get() that runs on the master.
*
*
* @end
*
*********************************************************************/
static int l7_rpc_server_port_stat_get(int unit, bcm_port_t port, int setget,
            uint32 *args)
{
    int     rv = 0, argi, s;
    uint64  val;

    if (setget != BCM_CUSTOM_GET) {
    return BCM_E_PARAM;
    }

    argi = 0;
    for (s = 0; s < snmpValCount; s++) {
        rv = bcm_port_stat_get(unit, port, s, &val);
        if (rv < 0) {
        /* Not all stats may be supported on the port. Return zeroes for unsupported
        ** stats and set return code to OK.
        */
        rv = BCM_E_NONE;
        memset (&val, 0, sizeof (val));

        }
        if (argi >= BCM_CUSTOM_ARGS_MAX-2) {
        break;
        }
        args[argi++] = COMPILER_64_HI(val);
        args[argi++] = COMPILER_64_LO(val);
    }
    /* clear remaining arguments */
    sal_memset(&args[argi], 0,
           sizeof(uint32) * (BCM_CUSTOM_ARGS_MAX-argi));

    return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the mirroring configuration for the port
*
*
* @param    port               @{(input)} Lport 
* @param    mirrorConfig       @{(input)} Mirroring configuration
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_mirror_set(bcmx_lport_t port, 
                                  usl_bcm_port_mirror_config_t mirrorConfig)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(mirrorConfig) > sizeof(args)) 
  {
      LOG_ERROR(sizeof(mirrorConfig));
  }

  memcpy (args, &mirrorConfig, sizeof (mirrorConfig));

  n_args = sizeof(mirrorConfig)/sizeof(uint32);
  if (sizeof(mirrorConfig)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_MIRROR_CONFIG_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_MIRROR_CONFIG_SET, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to to set the mirror configuration for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_mirror_set(int unit, bcm_port_t port, 
                                  int setget, uint32 *args)
{
  int                                rv = BCM_E_NONE;
  usl_bcm_port_mirror_config_t     *mirrorConfig;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }


  mirrorConfig = (usl_bcm_port_mirror_config_t *) args;

  rv = usl_bcm_port_mirror_set(unit, port, mirrorConfig); 

  return rv;
}

/*********************************************************************
*
* @purpose RPC server API to set the admin mode for a list of ports
*
* @param   
*
* @returns BCMX Error Code
*
* @notes   Remote handler
* @end
*
*********************************************************************/
int l7_rpc_server_ports_admin_mode_set(int unit, bcm_port_t port, 
                                       int setget, uint32 *args)
{
  L7_uchar8                  *msg, *msgPtr;
  usl_bcm_port_admin_mode_t  *tmpElem;
  int                         rv = BCM_E_NONE, tmpRv;
  L7_uint32                   numElems;


  msg = (L7_uchar8 *) args;
  msgPtr = msg + sizeof(L7_uint32);
  numElems = *(L7_uint32 *)&msg[0];

  while (numElems-- > 0) 
  {
    tmpElem = (usl_bcm_port_admin_mode_t *) msgPtr;

    tmpRv = bcm_port_enable_set(tmpElem->bcmUnit, tmpElem->bcmPort, tmpElem->adminMode);
    if (tmpRv != BCM_E_NONE) 
    {
      rv = tmpRv;
    }

    msgPtr += sizeof(usl_bcm_port_admin_mode_t);
  }

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the admin mode for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_enable_set(bcmx_lport_t port, 
                                  int enable)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(enable) > sizeof(args)) 
  {
      LOG_ERROR(sizeof(enable));
  }

  memcpy (args, &enable, sizeof (enable));

  n_args = sizeof(enable)/sizeof(uint32);
  if (sizeof(enable)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_ADMIN_MODE_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_ADMIN_MODE_SET, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to to set the admin mode for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_enable_set(int unit, bcm_port_t port, 
                                  int setget, uint32 *args)
{
  int       rv = BCM_E_NONE;
  int      *enable;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }


  enable = (int *) args;

  rv = usl_bcm_port_enable_set(unit, port, *enable); 

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the wred params for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_wred_set(bcmx_lport_t port, 
                                usl_bcm_port_wred_config_t *wredParams)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(*wredParams) > sizeof(args)) 
  {
      LOG_ERROR(sizeof(*wredParams));
  }

  memcpy (args, wredParams, sizeof (*wredParams));

  n_args = sizeof(*wredParams)/sizeof(uint32);
  if (sizeof(*wredParams)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_WRED_CONFIG_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_WRED_CONFIG_SET, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to to set the wred params for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_wred_set(int unit, bcm_port_t port, 
                                int setget, uint32 *args)
{
  int                          rv = BCM_E_NONE;
  usl_bcm_port_wred_config_t  *wredParams;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  wredParams = (usl_bcm_port_wred_config_t *) args;

  rv = usl_bcm_port_wred_set(unit, port, wredParams); 

  return rv;
}


/*********************************************************************
*
* @purpose RPC Client API to set the sflow config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_sflow_config_set(bcmx_lport_t port, 
                                        usl_bcm_port_sflow_config_t *sflowConfig)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(*sflowConfig) > sizeof(args)) 
  {
      LOG_ERROR(sizeof(*sflowConfig));
  }

  memcpy (args, sflowConfig, sizeof (*sflowConfig));

  n_args = sizeof(*sflowConfig)/sizeof(uint32);
  if (sizeof(*sflowConfig)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_SFLOW_CONFIG_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_SFLOW_CONFIG_SET, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to to set the sflow params for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_sflow_config_set(int unit, bcm_port_t port, 
                                        int setget, uint32 *args)
{
  int                            rv = BCM_E_NONE;
  usl_bcm_port_sflow_config_t   *sflowConfig;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  sflowConfig = (usl_bcm_port_sflow_config_t *) args;

  rv = usl_bcm_port_sflow_config_set(unit, port, sflowConfig); 

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the VLAN translation config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_translate_ingress_enable_set(bcmx_lport_t port, L7_BOOL enable)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  memcpy (args, &enable, sizeof (enable));

  n_args = sizeof(enable)/sizeof(uint32);
  if (sizeof(enable)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_VLAN_XLATE_INGRESS_ENABLE_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_VLAN_XLATE_INGRESS_ENABLE_SET, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to to set the VLAN translation for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_vlan_translate_ingress_enable_set(int unit, bcm_port_t port, 
                                                         int setget, uint32 *args)
{
  int       rv = BCM_E_NONE;
  L7_BOOL   enable;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  memcpy(&enable, args, sizeof(enable));

  rv = usl_bcm_port_vlan_translate_ingress_enable_set(unit, port, enable); 

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the VLAN translation config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_translate_ingress_miss_drop_set(bcmx_lport_t port, L7_BOOL drop)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  memcpy (args, &drop, sizeof (drop));

  n_args = sizeof(drop)/sizeof(uint32);
  if (sizeof(drop)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_VLAN_XLATE_INGRESS_MISS_DROP_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_VLAN_XLATE_INGRESS_MISS_DROP_SET, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to to set the VLAN translation for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_vlan_translate_ingress_miss_drop_set(int unit, bcm_port_t port, 
                                                            int setget, uint32 *args)
{
  int       rv = BCM_E_NONE;
  L7_BOOL   drop;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  memcpy(&drop, args, sizeof(drop));

  rv = usl_bcm_port_vlan_translate_ingress_miss_drop_set(unit, port, drop); 

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the VLAN translation config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_translate_egress_enable_set(bcmx_lport_t port, L7_BOOL enable)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  memcpy (args, &enable, sizeof (enable));

  n_args = sizeof(enable)/sizeof(uint32);
  if (sizeof(enable)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_VLAN_XLATE_EGRESS_ENABLE_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_VLAN_XLATE_EGRESS_ENABLE_SET, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to to set the VLAN translation for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_vlan_translate_egress_enable_set(int unit, bcm_port_t port, 
                                                        int setget, uint32 *args)
{
  int       rv = BCM_E_NONE;
  L7_BOOL   enable;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  memcpy(&enable, args, sizeof(enable));

  rv = usl_bcm_port_vlan_translate_egress_enable_set(unit, port, enable); 

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the VLAN translation config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_translate_egress_miss_drop_set(bcmx_lport_t port, L7_BOOL drop)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  memcpy (args, &drop, sizeof (drop));

  n_args = sizeof(drop)/sizeof(uint32);
  if (sizeof(drop)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_VLAN_XLATE_EGRESS_MISS_DROP_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_VLAN_XLATE_EGRESS_MISS_DROP_SET, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to to set the VLAN translation for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_vlan_translate_egress_miss_drop_set(int unit, bcm_port_t port, 
                                                           int setget, uint32 *args)
{
  int       rv = BCM_E_NONE;
  L7_BOOL   drop;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  memcpy(&drop, args, sizeof(drop));

  rv = usl_bcm_port_vlan_translate_egress_miss_drop_set(unit, port, drop); 

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the VLAN translation config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_translate_key_first_set(bcmx_lport_t port, bcm_vlan_translate_key_t key)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  memcpy (args, &key, sizeof (key));

  n_args = sizeof(key)/sizeof(uint32);
  if (sizeof(key)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_VLAN_XLATE_KEY_FIRST_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_VLAN_XLATE_KEY_FIRST_SET, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to to set the VLAN translation for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_vlan_translate_key_first_set(int unit, bcm_port_t port, 
                                                    int setget, uint32 *args)
{
  int                      rv = BCM_E_NONE;
  bcm_vlan_translate_key_t key;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  memcpy(&key, args, sizeof(key));

  rv = usl_bcm_port_vlan_translate_key_first_set(unit, port, key); 

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to set the VLAN translation config for a port
*
*
* @param    port           @{(input)} Lport 
* @param    enable         @{(input)} 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_vlan_translate_key_second_set(bcmx_lport_t port, bcm_vlan_translate_key_t key)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  memcpy (args, &key, sizeof (key));

  n_args = sizeof(key)/sizeof(uint32);
  if (sizeof(key)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_VLAN_XLATE_KEY_SECOND_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_VLAN_XLATE_KEY_SECOND_SET, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to to set the VLAN translation for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_vlan_translate_key_second_set(int unit, bcm_port_t port, 
                                                     int setget, uint32 *args)
{
  int                      rv = BCM_E_NONE;
  bcm_vlan_translate_key_t key;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  memcpy(&key, args, sizeof(key));

  rv = usl_bcm_port_vlan_translate_key_second_set(unit, port, key); 

  return rv;
}
/*********************************************************************
*
* @purpose RPC Client API to set the PFC configuration for the port
*
*
* @param    port               @{(input)} Lport 
* @param    pfcConfig       @{(input)} PFC configuration
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_pfc_config_set(bcmx_lport_t port, 
                                      usl_bcm_port_pfc_config_t pfcConfig)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  if (sizeof(pfcConfig) > sizeof(args)) 
  {
      LOG_ERROR(sizeof(pfcConfig));
  }

  memcpy (args, &pfcConfig, sizeof (pfcConfig));

  n_args = sizeof(pfcConfig)/sizeof(uint32);
  if (sizeof(pfcConfig)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_PFC_CONFIG_SET, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_PFC_CONFIG_SET, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to to set the PFC configuration for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} PFC config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_pfc_config_set(int unit, bcm_port_t port, 
                                      int setget, uint32 *args)
{
  int                                rv = BCM_E_NONE;
  usl_bcm_port_pfc_config_t     *pfcConfig;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  pfcConfig = (usl_bcm_port_pfc_config_t *) args;

  rv = usl_bcm_port_pfc_config_set(unit, port, pfcConfig); 

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to get the PFC stat for the port
*
*
* @param    port       @{(input)} Lport 
* @param    stat       @{(input)} PFC configuration
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_pfc_stat_get(bcmx_lport_t port, 
                                    usl_bcm_port_pfc_stat_t *stat)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  usl_bcm_port_pfc_stat_t *ret_stat;

  if (sizeof(*stat) > sizeof(args)) 
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR,L7_PFC_COMPONENT_ID,
          "Stat size too big for buffer %d > %d\n",
          sizeof(*stat),sizeof(args));
    return BCM_E_INTERNAL;
  }

  memcpy (args, stat, sizeof (*stat));
  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  L7_int len;
  rv = bcmx_custom_port_get(port, USL_BCMX_PORT_PFC_STAT_GET, BCM_CUSTOM_ARGS_MAX, args, &len);
  #else
  rv = bcmx_custom_port_get(port, USL_BCMX_PORT_PFC_STAT_GET, args);
  #endif

  if (BCM_E_NONE == rv)
  {
      ret_stat = (usl_bcm_port_pfc_stat_t *)args;
      stat->ctr = ret_stat->ctr;
  }

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to to get the PFC stat for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} PFC config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_pfc_stat_get(int unit, bcm_port_t port, 
                                    int setget, uint32 *args)
{
  int                                rv = BCM_E_NONE;
  usl_bcm_port_pfc_stat_t     *stat;

  stat = (usl_bcm_port_pfc_stat_t *) args;

  rv = usl_bcm_port_pfc_stat_get(unit, port, stat); 

  return rv;
}

/*********************************************************************
*
* @purpose RPC Client API to clear the PFC stats for the port
*
*
* @param    port               @{(input)} Lport 
*
* @returns BCMX Error Code
*
* @notes   Called from USL BCMX API
*
* @end
*
*********************************************************************/
int l7_rpc_client_port_pfc_stats_clear(bcmx_lport_t port)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  uint32  n_args;

  /* PTin added: bug? */
  #if 1
  if (sizeof(port) > sizeof(args)) 
  {
      LOG_ERROR(sizeof(port));
  }

  memcpy (args, &port, sizeof (port));
  #endif

  n_args = sizeof(port)/sizeof(uint32);
  if (sizeof(port)%sizeof(uint32) != 0)  n_args++;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_PFC_STATS_CLEAR, n_args, args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_PORT_PFC_STATS_CLEAR, args);
  #endif

  return rv;
}

/*********************************************************************
*
* @purpose RPC Server API to clear the PFC stats for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} PFC config data
*
*
* @returns BCMX Error Code
*
* @notes   Remote handler
*
* @end
*
*********************************************************************/
int l7_rpc_server_port_pfc_stats_clear(int unit, bcm_port_t port, 
                                       int setget, uint32 *args)
{
  int                                rv = BCM_E_NONE;

  if (setget != BCM_CUSTOM_SET) 
  {
    rv = BCM_E_PARAM;
    return rv;
  }

  rv = usl_bcm_port_pfc_stats_clear(unit, port); 

  return rv;
}

/*********************************************************************
*
* @purpose Handle custom bcmx commands for port configuration 
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} config data
* @param    rv            @{(output)} return value if the command
*                                     was executed
*
*
* @returns L7_TRUE: If the command was handled
*          L7_FALSE: If the command was not handled
*
* @notes 
*
* @end
*
*********************************************************************/

L7_BOOL l7_rpc_server_port_handler(int unit, bcm_port_t port, int setget, 
                                   int type, uint32 *args, int *rv)
{
  L7_BOOL handled = L7_TRUE;

  switch (type)
  {
    case USL_BCMX_PORT_ADMIN_MODE_SET:
      *rv = l7_rpc_server_port_enable_set (unit, port , setget , args );
      break;

    case USL_BCMX_STATS_GET:
      *rv = l7_rpc_server_stat_get (unit, port , setget , args);
      break;

    case USL_BCMX_PORT_STATS_GET:
      *rv = l7_rpc_server_port_stat_get (unit, port , setget , args);
      break;

    case USL_BCMX_PORT_FLOW_CONTROL_SET:
      *rv = l7_rpc_server_port_flow_control_set (unit, port ,setget ,args );
      break;

    case USL_BCMX_DOT1X_CLIENT_TIMEOUT_GET:
      *rv=  l7_rpc_server_port_dot1x_client_timeout_get(unit, port, setget, args);
      break;

    case USL_BCMX_PORT_SFP_DIAG_GET:
      *rv = l7_rpc_server_port_sfp_diag_get(unit, port, setget, args);
      break;

    case USL_BCMX_PORT_COPPER_DIAG_GET:
      *rv = l7_rpc_server_port_copper_diag_get(unit, port, setget, args);
      break;

    case USL_BCMX_DOT1X_CLIENT_BLOCK:
      *rv = l7_rpc_server_port_dot1x_client_block(unit, port, setget, args);
      break;
  
    case USL_BCMX_DOT1X_CLIENT_UNBLOCK:
      *rv = l7_rpc_server_port_dot1x_client_unblock(unit, port, setget, args);
      break;

     case USL_BCMX_PORT_BCAST_RATE_SET:
       *rv =  l7_rpc_server_rate_bcast_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_MCAST_RATE_SET:
       *rv =  l7_rpc_server_rate_mcast_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_DLF_RATE_SET:
       *rv =  l7_rpc_server_rate_dlfbc_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_FILTER_MODE_SET:
       *rv =  l7_rpc_server_port_vlan_member_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_PRIORITY_SET:
       *rv =  l7_rpc_server_port_untagged_priority_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_MAX_FRAME_SET:
       *rv =  l7_rpc_server_port_frame_max_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_LEARN_MODE_SET:
       *rv =  l7_rpc_server_port_learn_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_DTAG_MODE_SET:
       *rv =  l7_rpc_server_port_dtag_mode_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_TPID_SET:
       *rv =  l7_rpc_server_port_tpid_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_TPID_ADD:
       *rv =  l7_rpc_server_port_tpid_add(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_TPID_DELETE:
       *rv =  l7_rpc_server_port_tpid_delete(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_PVID_SET:
       *rv =  l7_rpc_server_port_untagged_vlan_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_DISCARD_SET:
       *rv =  l7_rpc_server_port_discard_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_PHY_CONFIG_SET:
       *rv =  l7_rpc_server_port_medium_config_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_COSQ_SCHED_CONFIG_SET:
       *rv =  l7_rpc_server_port_cosq_sched_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_SHAPER_CONFIG_SET:
       *rv =  l7_rpc_server_port_rate_egress_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_VLAN_CONFIG_SET:
       *rv =  l7_rpc_server_port_vlan_config(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_STG_STATE_SET:
       *rv =  l7_rpc_server_stg_stp_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_PBVLAN_CONFIG_SET:
       *rv =  l7_rpc_server_port_protocol_vlan_config(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_DOT1X_CONFIG_SET:
       *rv =  l7_rpc_server_port_dot1x_config(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_MIRROR_CONFIG_SET:
       *rv =  l7_rpc_server_port_mirror_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_WRED_CONFIG_SET:
       *rv = l7_rpc_server_port_wred_set(unit, port, setget, args);
       break;

      case USL_BCMX_PORT_SFLOW_CONFIG_SET:
       *rv = l7_rpc_server_port_sflow_config_set(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_MCAST_GROUPS_UPDATE:
       *rv = l7_rpc_server_mcast_port_update_groups(unit, port, setget, args);
       break;

     case USL_BCMX_PORT_VLAN_XLATE_INGRESS_ENABLE_SET:
       *rv = l7_rpc_server_port_vlan_translate_ingress_enable_set(unit, port, setget, args);
       break;
   
     case USL_BCMX_PORT_VLAN_XLATE_INGRESS_MISS_DROP_SET:
       *rv = l7_rpc_server_port_vlan_translate_ingress_miss_drop_set(unit, port, setget, args);
       break;
   
     case USL_BCMX_PORT_VLAN_XLATE_EGRESS_ENABLE_SET:
       *rv = l7_rpc_server_port_vlan_translate_egress_enable_set(unit, port, setget, args);
       break;
   
     case USL_BCMX_PORT_VLAN_XLATE_EGRESS_MISS_DROP_SET:
       *rv = l7_rpc_server_port_vlan_translate_egress_miss_drop_set(unit, port, setget, args);
       break;
   
     case USL_BCMX_PORT_VLAN_XLATE_KEY_FIRST_SET:
       *rv = l7_rpc_server_port_vlan_translate_key_first_set(unit, port, setget, args);
       break;
   
     case USL_BCMX_PORT_VLAN_XLATE_KEY_SECOND_SET:
       *rv = l7_rpc_server_port_vlan_translate_key_second_set(unit, port, setget, args);
       break;

      case USL_BCMX_PORT_PFC_CONFIG_SET:
        *rv = l7_rpc_server_port_pfc_config_set(unit,port,setget,args);
        break;

      case USL_BCMX_PORT_PFC_STAT_GET:
        *rv = l7_rpc_server_port_pfc_stat_get(unit,port,setget,args);
        break;

      case USL_BCMX_PORT_PFC_STATS_CLEAR:
        *rv = l7_rpc_server_port_pfc_stats_clear(unit,port,setget,args);
       break;

     default: 
       handled = L7_FALSE;
       break;
    }

  return handled;
}


