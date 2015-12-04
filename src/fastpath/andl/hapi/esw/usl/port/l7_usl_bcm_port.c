/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_bcm_port.c
*
* @purpose    USL BCM API's for port 
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
#include "l7_usl_bcm_port.h"
#include "l7_usl_port_db.h"
#include "l7_usl_sm.h"

#include "bcm/rate.h"
#include "bcm/stack.h"
#include "bcm/mirror.h"
#include "dapi.h"
#include "broad_mmu.h"
#include "l7_usl_l2_db.h"

/*********************************************************************
*
* @purpose BCM API to set the broadcast rate threshold for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    bcast_limit   @{(input)} Bcast rate threshold data
*
* @returns BCMX Error Code
*
* @notes   
*
* @end
*
*********************************************************************/
int usl_bcm_port_rate_bcast_set (int unit, 
                                 bcm_port_t port, 
                                 usl_bcm_port_rate_limit_t *bcast_limit)
{
  int          rv = BCM_E_NONE;
  bcm_gport_t  gport = BCM_GPORT_INVALID;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Inside usl_bcm_port_rate_bcast_set with limit=%u bucket_size=%u flags=0x%x for bcm_port=0x%x",
            bcast_limit->limit, bcast_limit->bucket_size, bcast_limit->flags, port);

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    /* PTin modified: storm control */
    if (bcast_limit->units == L7_RATE_UNIT_PPS)
    {
      rv = bcm_rate_bcast_set(unit, bcast_limit->limit, bcast_limit->flags, port);
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_rate_bcast_set with limit=%u flags=0x%x for bcm_port=0x%x: rv=%d",
                bcast_limit->limit, bcast_limit->flags, port, rv);
    }
    else if (bcast_limit->units == L7_RATE_UNIT_KBPS)
    {
      bcast_limit->flags |= BCM_RATE_BCAST;
      rv = bcm_rate_bandwidth_set(unit, port, bcast_limit->flags, bcast_limit->limit, bcast_limit->bucket_size);
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_rate_bandwidth_set with limit=%u bucket_size=%u flags=0x%x for bcm_port=0x%x: rv=%d",
                bcast_limit->limit, bcast_limit->bucket_size, bcast_limit->flags, port, rv);
    }
    else
    {
      rv = BCM_E_PARAM;
    }
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);

    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_bcast_rate_set(USL_CURRENT_DB, gport, bcast_limit);
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose BCM API to set the multicast rate threshold for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    mcast_limit   @{(input)} Mcast rate threshold data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_rate_mcast_set (int unit, 
                                 bcm_port_t port, 
                                 usl_bcm_port_rate_limit_t *mcast_limit)
{
  int          rv = BCM_E_NONE;
  bcm_gport_t  gport = BCM_GPORT_INVALID;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Inside usl_bcm_port_rate_mcast_set with limit=%u bucket_size=%u flags=0x%x for bcm_port=0x%x",
            mcast_limit->limit, mcast_limit->bucket_size, mcast_limit->flags, port);

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    /* PTin modified: storm control */
    if (mcast_limit->units == L7_RATE_UNIT_PPS)
    {
      rv = bcm_rate_mcast_set(unit, mcast_limit->limit, mcast_limit->flags, port);
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_rate_mcast_set with limit=%u flags=0x%x for bcm_port=0x%x: rv=%d",
                mcast_limit->limit, mcast_limit->flags, port, rv);
    }
    else if (mcast_limit->units == L7_RATE_UNIT_KBPS)
    {
      mcast_limit->flags |= BCM_RATE_MCAST;
      rv = bcm_rate_bandwidth_set(unit, port, mcast_limit->flags, mcast_limit->limit, mcast_limit->bucket_size);
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_rate_bandwidth_set with limit=%u bucket_size=%u flags=0x%x for bcm_port=0x%x: rv=%d",
                mcast_limit->limit, mcast_limit->bucket_size, mcast_limit->flags, port, rv);
    }
    else
    {
      rv = BCM_E_PARAM;
    }
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_mcast_rate_set(USL_CURRENT_DB, gport, mcast_limit); 
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose BCM API to set the dlf rate threshold for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    dlf_limit     @{(input)} Dlf rate threshold data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_rate_dlfbc_set (int unit, 
                                 bcm_port_t port, 
                                 usl_bcm_port_rate_limit_t *dlf_limit)
{
  int                         rv = BCM_E_NONE;
  bcm_gport_t                 gport = BCM_GPORT_INVALID;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Inside usl_bcm_port_rate_dlfbc_set with limit=%u bucket_size=%u flags=0x%x for bcm_port=0x%x",
            dlf_limit->limit, dlf_limit->bucket_size, dlf_limit->flags, port);

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    /* PTin modified: storm control */
    if (dlf_limit->units == L7_RATE_UNIT_PPS)
    {
      rv = bcm_rate_dlfbc_set(unit, dlf_limit->limit, dlf_limit->flags, port);
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_rate_dlfbc_set with limit=%u flags=0x%x for bcm_port=0x%x: rv=%d",
                dlf_limit->limit, dlf_limit->flags, port, rv);
    }
    else if (dlf_limit->units == L7_RATE_UNIT_KBPS)
    {
      dlf_limit->flags |= BCM_RATE_DLF;
      rv = bcm_rate_bandwidth_set(unit, port, dlf_limit->flags, dlf_limit->limit, dlf_limit->bucket_size);
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_rate_bandwidth_set with limit=%u bucket_size=%u flags=0x%x for bcm_port=0x%x: rv=%d",
                dlf_limit->limit, dlf_limit->bucket_size, dlf_limit->flags, port, rv);
    }
    else
    {
      rv = BCM_E_PARAM;
    }
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_dlfbc_rate_set(USL_CURRENT_DB, gport, dlf_limit); 
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose BCM API to set the filtering mode for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    mode          @{(input)} Filter mode data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_member_set (int unit, 
                                  bcm_port_t port, 
                                  usl_bcm_port_filter_mode_t   *mode)
{
  int                            rv = BCM_E_NONE, flags;
  bcm_gport_t                    gport = BCM_GPORT_INVALID;


  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_port_vlan_member_get(unit, port, &flags);
    if (rv == BCM_E_NONE)
    {
      if (mode->setFlags == L7_TRUE)
      {
        flags |= mode->flags;          
      }
      else
      {
        flags &= (~(mode->flags));
      }

      rv = bcm_port_vlan_member_set(unit, port, flags);
    }
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_filter_mode_set(USL_CURRENT_DB, gport, mode); 
    }
  }

  return rv;

}

/*********************************************************************
*
* @purpose BCM API to set the priority for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} priority data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_untagged_priority_set (int unit, 
                                        bcm_port_t port, 
                                        usl_bcm_port_priority_t  *priority)
{
  int                            rv = BCM_E_NONE;
  bcm_gport_t                    gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_port_untagged_priority_set(unit, port, *priority);
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  { 
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_priority_set(USL_CURRENT_DB, gport, priority); 
    }
  }

  return rv;

}

/*********************************************************************
*
* @purpose BCM API to set the max frame size for a port
*
*
* @param    unit            @{(input)} Local bcm unit number
* @param    port            @{(input)} Local bcm port number 
* @param    max_frame_size  @{(input)} priority data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_frame_max_set(int unit, 
                               bcm_port_t port, 
                               usl_bcm_port_frame_size_t    *max_frame_size)
{
  int                            rv = BCM_E_NONE;
  bcm_gport_t                    gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_port_frame_max_set(unit, port, *max_frame_size);

    if (rv == BCM_E_NONE)
    {
      /* PTin added: Update threshold for ovsersized packets */
      //bcm_port_control_set(unit, port, bcmPortControlStatOversize, *max_frame_size - 4);

      rv = hapiBroadMmuPortMtuSet(unit,port,*max_frame_size);
    }
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_max_frame_set(USL_CURRENT_DB, gport, max_frame_size); 
    }
  }

  return rv;

}

/*********************************************************************
*
* @purpose BCM API to set the learn mode for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    learn_mode    @{(input)} Learn mode
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_learn_set(int unit, 
                           bcm_port_t port, 
                           usl_bcm_port_learn_mode_t    *learn_mode)
{
  int             rv = BCM_E_NONE;
  bcm_gport_t     gport = BCM_GPORT_INVALID;


  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_port_learn_set(unit, port, *learn_mode);
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_learn_set(USL_CURRENT_DB, gport, learn_mode);  
    }
  }

  return rv;

}

/*********************************************************************
*
* @purpose BCM API to set the dtag mode for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    dtag_mode     @{(input)} priority data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_dtag_mode_set(int unit, 
                               bcm_port_t port, 
                               usl_bcm_port_dtag_mode_t *dtag_mode)
{
  int                            rv = BCM_E_NONE;
  bcm_gport_t                    gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_port_dtag_mode_set(unit, port, *dtag_mode);
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv  == BCM_E_NONE)
    {
      rv = usl_portdb_dtag_mode_set(USL_CURRENT_DB, gport, dtag_mode); 
    }
  }

  return rv;

}


/*********************************************************************
*
* @purpose BCM API to set the tpid for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    tpid          @{(input)} Tpid data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_tpid_set(int unit, 
                          bcm_port_t port, 
                          usl_bcm_port_tpid_t *tpid)
{
  int                            rv = BCM_E_NONE;
  bcm_gport_t                    gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_port_tpid_set(unit, port, *tpid);
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_tpid_set(USL_CURRENT_DB, gport, tpid); 
    }
  }

  return rv;

}

/*********************************************************************
*
* @purpose BCM API to add a tpid for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    tpid          @{(input)} Tpid data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_tpid_add(int unit, 
                          bcm_port_t port, 
                          usl_bcm_port_tpid_t *tpid)
{
  int                            rv = BCM_E_NONE;
  bcm_gport_t                    gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_port_tpid_add(unit, port, *tpid, 0);
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_tpid_add(USL_CURRENT_DB, gport, tpid); 
    }
  }

  return rv;

}

/*********************************************************************
*
* @purpose BCM API to delete a tpid from a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    tpid          @{(input)} Tpid data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_tpid_delete(int unit, 
                             bcm_port_t port, 
                             usl_bcm_port_tpid_t *tpid)
{
  int                            rv = BCM_E_NONE;
  bcm_gport_t                    gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_port_tpid_delete(unit, port, *tpid);
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_tpid_delete(USL_CURRENT_DB, gport, tpid); 
    }
  }

  return rv;

}

/*********************************************************************
*
* @purpose BCM API to set the default vid for a port
*
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    pvid          @{(input)} pvid data
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_untagged_vlan_set(int unit, 
                                   bcm_port_t port, 
                                   bcm_vlan_t *pvid)
{
  int                            rv = BCM_E_NONE;
  bcm_gport_t                    gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_port_untagged_vlan_set(unit, port, *pvid);
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_pvid_set(USL_CURRENT_DB, gport, pvid); 
    }
  }

  return rv;

}


/*********************************************************************
*
* @purpose BCM API to set the discard mode for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    mode          @{(input)} discard mode data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_discard_set(int unit, 
                             bcm_port_t port, 
                             bcm_port_discard_t *mode)
{
  int                            rv = BCM_E_NONE;
  bcm_gport_t                    gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_port_discard_set(unit, port, *mode);
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_discard_set(USL_CURRENT_DB, gport, mode); 
    }
  }

  return rv;

}

/*********************************************************************
*
* @purpose BCM API to set the phy medium config for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    medium        @{(input)} phy medium data
* @param    config        @{(input)} phy config data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_medium_config_set(int unit, 
                                   bcm_port_t port, 
                                   bcm_port_medium_t medium,
                                   bcm_phy_config_t *config)
{
  int                          rv = BCM_E_NONE;
  bcm_gport_t                  gport = BCM_GPORT_INVALID;


  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_port_medium_config_set(unit, port, medium, config);
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_medium_config_set(USL_CURRENT_DB, gport, medium, config); 
    }
  }

  return rv;

}


/*********************************************************************
*
* @purpose BCM API to set the flow control settings for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} flow-control data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_flow_control_set(int unit, 
                                  bcm_port_t port, 
                                  usl_bcm_port_pause_config_t  *pauseConfig)
{
  int                            rv = BCM_E_NONE, tmpRv;
  bcm_gport_t                    gport = BCM_GPORT_INVALID;
  bcm_port_abil_t                ability_mask;
  L7_BOOL                        jam = L7_FALSE, abilityMaskChanged = L7_FALSE;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {

    tmpRv = bcm_port_advert_get(unit, port, &ability_mask);
    if (tmpRv != BCM_E_NONE)
    {
    rv = tmpRv;
    }

    if ((pauseConfig->pauseRx) &&
        (!(ability_mask & BCM_PORT_ABIL_PAUSE_RX)))
    {
      ability_mask |= BCM_PORT_ABIL_PAUSE_RX;
      abilityMaskChanged = L7_TRUE;
    }
    else
    {
      if (ability_mask & BCM_PORT_ABIL_PAUSE_RX)
      {
        ability_mask &= ~BCM_PORT_ABIL_PAUSE_RX;
        abilityMaskChanged = L7_TRUE;
      }
    }
  
    if ((pauseConfig->pauseTx) &&
        (!(ability_mask & BCM_PORT_ABIL_PAUSE_TX)))
    {
      ability_mask |= BCM_PORT_ABIL_PAUSE_TX;
      abilityMaskChanged = L7_TRUE;
    }
    else
    {
      if (ability_mask & BCM_PORT_ABIL_PAUSE_RX)
      {
        ability_mask &= ~BCM_PORT_ABIL_PAUSE_TX;
        abilityMaskChanged = L7_TRUE;
      }
    }


    if (abilityMaskChanged)
    {
      tmpRv = bcm_port_advert_set(unit, port, ability_mask);
      if (tmpRv != BCM_E_NONE) 
      {
        rv = tmpRv;
      }
    }


    tmpRv = bcm_port_pause_set(unit, port, pauseConfig->pauseTx, pauseConfig->pauseRx);
    if (tmpRv != BCM_E_NONE) 
    {
      rv = tmpRv;
    }

    if(hapiBroadRoboCheck() != L7_TRUE)
    {
      tmpRv = bcm_port_pause_addr_set(unit, port, pauseConfig->pauseMacAddr);
      if (tmpRv != BCM_E_NONE) 
      {
        rv = tmpRv;
      }
    }

    if (pauseConfig->pauseTx || pauseConfig->pauseRx) 
    {
      jam = L7_TRUE;
    }
    else
    {
      jam = L7_FALSE;
    }

    /* Jam is only supported for certain port types */
    tmpRv = bcm_port_jam_set(unit, port, jam);
    if ((tmpRv != BCM_E_NONE) && (tmpRv != BCM_E_UNAVAIL))
    {
      rv = tmpRv;
    }

  } /* End If (USL_BCM_CONFIGURE_HW */

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_flow_control_set(USL_CURRENT_DB, gport, pauseConfig); 
    }
  }

  return rv;

}

/*********************************************************************
*
* @purpose BCM API to set the cosq sched config for a port
*
* @param    unit              @{(input)} Local bcm unit number
* @param    port              @{(input)} Local bcm port number 
* @param    cosqSchedConfig   @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_cosq_sched_set(int unit, 
                                bcm_port_t port, 
                                usl_bcm_port_cosq_sched_config_t  *cosqSchedConfig)
{
  int                                 rv = BCM_E_NONE, cosq;
  bcm_gport_t                         gport = BCM_GPORT_INVALID;
  bcm_pbmp_t                          pbmp;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID))
  {
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_cosq_port_sched_set(unit, pbmp, 
                                 cosqSchedConfig->mode, 
                                 cosqSchedConfig->weights, 
                                 cosqSchedConfig->delay);

    if (rv == BCM_E_NONE) 
    {
      for (cosq = 0; cosq < BCM_COS_COUNT; cosq++)
      {
        rv = bcm_cosq_port_bandwidth_set(unit, 
                                         port, 
                                         cosq, 
                                         cosqSchedConfig->minKbps[cosq], 
                                         cosqSchedConfig->maxKbps[cosq], 
                                         0);
        if (rv == BCM_E_UNAVAIL)
        {
          /* Silently ignore errors if this feature is not supported. */
          rv = BCM_E_NONE;
          break;
        }

        if (rv != BCM_E_NONE)
          break;
      }
    }
  }

 
  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_cosq_sched_set(USL_CURRENT_DB, gport, cosqSchedConfig); 
    }
  }


  return rv;

}

/*********************************************************************
*
* @purpose BCM API to set the rate shaper config for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_rate_egress_set(int unit, 
                                 bcm_port_t port, 
                                 usl_bcm_port_shaper_config_t *shaperConfig)
{
  int                                 rv = BCM_E_NONE;
  bcm_gport_t                         gport = BCM_GPORT_INVALID;

                                                                                              
  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_port_rate_egress_set(unit, port, 
                                  shaperConfig->rate, 
                                  shaperConfig->burst);
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_shaper_config_set(USL_CURRENT_DB, gport, shaperConfig); 
    }
  }

  return rv;

}


/*********************************************************************
*
* @purpose BCM API to set a port in a vlan
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    cmd           @{(input)} Add or remove the port from vlan
* @param    vlan-id       @{(input)} Vlan id 
* @param    tagged        @{(input)} Port is tagged or not
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_vlan_port_update(int unit, 
                             bcm_port_t port, 
                             USL_CMD_t cmd,
                             bcm_vlan_t vid,
                             L7_BOOL tagged)
{

  int             rv = BCM_E_NONE;
  bcm_gport_t     gport = BCM_GPORT_INVALID;
  bcm_pbmp_t      pbmp, old_pbmp, ubmp, old_ubmp;

  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(old_pbmp);
  BCM_PBMP_CLEAR(ubmp);
  BCM_PBMP_CLEAR(old_ubmp);
                                                                                              
  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    BCM_PBMP_PORT_SET (pbmp, port);
    if (tagged == L7_TRUE) 
    {
      BCM_PBMP_CLEAR (ubmp);
    } 
    else
    {
      BCM_PBMP_PORT_SET (ubmp, port);
    }

    if (cmd == USL_CMD_ADD)
    {
      /* If port is already in the VLAN then remove it. 
      */
      rv = bcm_vlan_port_get (unit, vid, &old_pbmp, &old_ubmp);
      if (rv != BCM_E_NONE)
      {
        if (BCM_PBMP_MEMBER(old_pbmp, port))
        {
          (void) bcm_vlan_port_remove (unit, vid, pbmp);
        }
      }

      /* Add the port to the VLAN with specified tagging mode.
      */
      rv = bcm_vlan_port_add (unit, vid, pbmp, ubmp);

    }
    else if (cmd == USL_CMD_REMOVE)
    {
      rv = bcm_vlan_port_remove(unit, vid, pbmp);    
    }
  }

  /* Update the USL port database only on non-management units */
  if (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_db_vlan_port_update(USL_CURRENT_DB, gport, cmd, vid, tagged); 
    }
  }

  return rv;

}

/*********************************************************************
*
* @purpose BCM API to add/remove port to/from vlans 
*
* @param    unit               @{(input)} unit 
* @param    port               @{(input)} port 
* @param    vlanConfig         @{(input)} Vlan configuration
* @param    cmd                @{(input)} L7_TRUE: Add ports to vlan
*                                         L7_FALSE: Remove ports from vlan
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_config(int unit, 
                             bcm_port_t port,
                             usl_bcm_port_vlan_t *vlanConfig,
                             L7_BOOL cmd) 
{
  int                         rv = BCM_E_NONE;
  bcm_vlan_t                  vid;
  bcm_pbmp_t                  pbmp, ubmp;
  bcm_pbmp_t                  old_pbmp, old_ubmp;
  bcm_gport_t                 gport;
  int                         isuntagged = 1;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    
      for (vid = 1; vid <= L7_PLATFORM_MAX_VLAN_ID; vid++)
      {
        if (cmd == L7_TRUE) /* Add */
        {
          if (L7_VLAN_ISMASKBITSET(vlanConfig->vlan_membership_mask, vid) != 0)
          {
            if (!BCM_GPORT_IS_WLAN_PORT(port))
            {
              BCM_PBMP_PORT_SET (pbmp, port);
            }
            if (L7_VLAN_ISMASKBITSET(vlanConfig->vlan_tag_mask, vid) != 0) 
            {
              if (!BCM_GPORT_IS_WLAN_PORT(port))
              {
                BCM_PBMP_CLEAR (ubmp);
              }
              isuntagged = 0;
            } else
            {
              if (!BCM_GPORT_IS_WLAN_PORT(port))
              {
                BCM_PBMP_PORT_SET (ubmp, port);
              }
              isuntagged = 1;
            }
    
            /* If port is already in the VLAN then remove it. 
            */
            if (BCM_GPORT_IS_WLAN_PORT(port))
            {
              int temp;
              rv = bcm_vlan_gport_get(unit, vid, port, &temp);
              if (rv == BCM_E_NONE)
              {
                rv = bcm_vlan_gport_delete (unit, vid, port);
                if (rv != BCM_E_NONE)
                {
                  /*LOG_ERROR (rv);*/
                  printf(" bcm_vlan_gport_delete failed for port %d; vid %d; rv = %d \n", port, vid, rv);
                }
              }

            }
            else
            {
              rv = bcm_vlan_port_get (unit, vid, &old_pbmp, &old_ubmp);
              if (rv != BCM_E_NONE)
              {
                LOG_ERROR (rv);
              }
              if (BCM_PBMP_MEMBER(old_pbmp, port))
              {
                rv = bcm_vlan_port_remove (unit, vid, pbmp);
                if (rv != BCM_E_NONE)
                {
                  LOG_ERROR (rv);
                }
              }
            }
    
            /* Add the port to the VLAN with specified tagging mode.
            */
            if (BCM_GPORT_IS_WLAN_PORT(port))
            {
              rv = bcm_vlan_gport_add (unit, vid, port, isuntagged);
              if (rv == BCM_E_PORT)
              {
                /* ignore this error for wlan ports since the vp may not be terminated on this unit */
                rv = BCM_E_NONE;
              }
            }
            else
            {
              rv = bcm_vlan_port_add (unit, vid, pbmp, ubmp);
            }
            if (rv != BCM_E_NONE)
            {
              LOG_ERROR (rv);
            }

            
            if ((L7_BCMX_OK(rv) == L7_TRUE) && 
                (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
            {
              /* Get the gport for the local bcm unit/port */
              rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
              if (rv == BCM_E_NONE)
              {
                rv = usl_db_vlan_port_update(USL_CURRENT_DB, gport, 
                                             cmd, vid, (isuntagged ? L7_FALSE : L7_TRUE)); 
              } 
            }
          }
        } /* End if (cmd == L7_TRUE */
        else /* Delete */
        {
          if (L7_VLAN_ISMASKBITSET(vlanConfig->vlan_membership_mask, vid) != 0)
          {
            if (BCM_GPORT_IS_WLAN_PORT(port))
            {
              rv = bcm_vlan_gport_delete (unit, vid, port);
              if (rv == BCM_E_PORT)
              {
                /* ignore this error for wlan ports since the vp may not be terminated on this unit */
                rv = BCM_E_NONE;
              }
            }
            else
            {
              BCM_PBMP_PORT_SET (pbmp, port);
  
              rv = bcm_vlan_port_remove (unit, vid, pbmp);
            }
  
            if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND))
            {
              LOG_ERROR (rv);
            }


            if ((L7_BCMX_OK(rv) == L7_TRUE) && 
                (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
            {
              /* Get the gport for the local bcm unit/port */
              rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
              if (rv == BCM_E_NONE)
              {
                rv = usl_db_vlan_port_update(USL_CURRENT_DB, gport, 
                                             cmd, vid, L7_FALSE); 
              } 
            }
          }
        }
      } /* End for */
  }

  return BCM_E_NONE;

}


/*********************************************************************
*
* @purpose BCM API to set the Stg stp state config for a port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    stg           @{(input)} stg-id
* @param    stpState      @{(input)} stpState
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_stg_stp_set(int unit, 
                        bcm_port_t port, 
                        bcm_stg_t stg,
                        bcm_stg_stp_t stpState)
     
{
  int                                rv = BCM_E_NONE;
  bcm_gport_t                        gport = BCM_GPORT_INVALID;


  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_stg_stp_set(unit, stg, port, stpState);
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_stg_stp_update(USL_CURRENT_DB, gport, stg, stpState); 
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose BCM API to protocol based vlan config for a port
*
* @param    unit               @{(input)} Local bcm unit number
* @param    port               @{(input)} Local bcm port number 
* @param    pbvlanConfig       @{(input)} protocol Vlan configuration
* @param    cmd                @{(input)} L7_TRUE: Add pbvlan config
*                                         L7_FALSE: Remove pbvlan config
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_protocol_vlan_config(int unit, 
                                      bcm_port_t port,
                                      usl_bcm_port_pbvlan_config_t *pbvlanConfig,
                                      L7_BOOL cmd)
{
  int                                rv = BCM_E_NONE;
  bcm_gport_t                        gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    if (cmd == L7_TRUE)
    {
      rv = bcm_port_protocol_vlan_add(unit,  port, pbvlanConfig->frameType, 
                                      pbvlanConfig->etherType,
                                      pbvlanConfig->vid);
    }
    else
    {
      rv = bcm_port_protocol_vlan_delete(unit,  port, pbvlanConfig->frameType, 
                                         pbvlanConfig->etherType);
    }
  }

  /* Update the USL port database only on non-management units */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)) 
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_protocol_vlan_update(USL_CURRENT_DB, gport, 
                                           pbvlanConfig, cmd); 
    }
  }

  return rv;
}


/*********************************************************************
*
* @purpose BCM API to to set the dot1x state for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    dot1xConfig   @{(input)} dot1x config data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_dot1x_config(int unit, 
                              bcm_port_t port, 
                              L7_DOT1X_PORT_STATUS_t dot1xState)
{
  int                                rv = BCM_E_NONE;
  bcm_gport_t                        gport = BCM_GPORT_INVALID;

  /* Update the USL port database only on non-management units */
  if (USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE)
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_dot1x_config(USL_CURRENT_DB, gport, dot1xState); 
    }
  }

  return rv;
}


/*********************************************************************
*
* @purpose dot1x client block Handler. 
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*   This is the custom handler that runs on the slave devices.
*
*
* @end
*
*********************************************************************/
int usl_bcm_port_dot1x_client_block(int unit, 
                                    bcm_port_t port, 
                                    usl_bcm_port_dot1x_client_t *pClientCmd)
{
  int     rv;
  bcm_l2_addr_t  l2addr;


  memset(&l2addr, 0, sizeof(l2addr));
  memcpy(l2addr.mac, pClientCmd->mac_addr, sizeof(bcm_mac_t));
  l2addr.vid = pClientCmd->vlan_id;
  l2addr.port = port;
  BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &l2addr.modid));
  l2addr.flags = BCM_L2_DISCARD_SRC | BCM_L2_DISCARD_DST | BCM_L2_HIT | BCM_L2_SRC_HIT;

  rv = bcm_l2_addr_add(unit, &l2addr);

  return rv;
}

/*********************************************************************
*
* @purpose dot1x client unblock Handler. 
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*   This is the custom handler that runs on the slave devices.
*
*
* @end
*
*********************************************************************/
int usl_bcm_port_dot1x_client_unblock(int unit, 
                                      bcm_port_t port, 
                                      usl_bcm_port_dot1x_client_t *pClientCmd)
{
  int     rv;
  bcm_l2_addr_t  l2addr;

  memset(&l2addr, 0, sizeof(l2addr));
  memcpy(l2addr.mac, pClientCmd->mac_addr, sizeof(bcm_mac_t));
  l2addr.vid = pClientCmd->vlan_id;

  rv = bcm_l2_addr_delete(unit, l2addr.mac, l2addr.vid);

  return rv;
}


/*********************************************************************
*
* @purpose BCM API to to set the mirror configuration for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    setget        @{(input)} Set or Get command
* @param    args          @{(input)} cosq sched config data
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_mirror_set(int unit, 
                            bcm_port_t port,
                            usl_bcm_port_mirror_config_t *mirrorConfig )
{
  int                                rv = BCM_E_NONE;
  int                                destModid, destModport;
  bcm_gport_t                        gport = BCM_GPORT_INVALID;

  destModid = BCM_GPORT_MODPORT_MODID_GET(mirrorConfig->probePort);
  destModport = BCM_GPORT_MODPORT_PORT_GET(mirrorConfig->probePort);

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_mirror_port_set(unit, port, destModid, destModport, mirrorConfig->flags); 
  }

  /* Update the USL port database only on non-management units */
  if ((USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE) && 
      (L7_BCMX_OK(rv) == L7_TRUE))
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_mirror_set(USL_CURRENT_DB, gport, mirrorConfig); 
    }
  }

  return rv;
}


/*********************************************************************
*
* @purpose BCM API to to set the admin mode configuration for the port
*
* @param    unit     @{(input)} Local bcm unit number
* @param    port     @{(input)} Local bcm port number 
* @param    enable   @{(input)} enable config
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_enable_set(int unit, bcm_port_t port, int enable)
{
  int             rv = BCM_E_NONE;
  bcm_gport_t     gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_port_enable_set(unit, port, enable); 
  }

  /* Update the USL port database only on non-management units */
  if ((USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE) && 
      (L7_BCMX_OK(rv) == L7_TRUE))
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_db_port_enable_set(USL_CURRENT_DB, gport, enable); 
    }
  }

  return rv;

}

/* Helper function to convert queue-depth percentages to bytes. */
static uint32 l7_port_wred_percent_to_bytes(int unit, uint8 percent)
{
  static int conversion_factor = 0;

  if (conversion_factor == 0) 
  {
    /* The buffer usage allowed to a queue is: alpha*(B - omega)/(1+alpha*S)
       where alpha is the dynamic multiplier, omega is the total amount of buffer 
       used by uncontrolled queues, B is the total amount of buffer memory, 
       and S is the number of active queues. */
    /* Fastpath assumptions: omega=0, alpha=8. */
    /* We will set our conversion factor to: 100% = one queue active on 1/4 of the ports */
    unsigned int cellsize = 128;
    unsigned int totalmem = 32767; /* max cells that can be specified */
    
    /* CXO640G board */
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TRIDENT2X(unit))
    {
      cellsize = 208;
      totalmem = 46080;
    }
    /* OLT1T0 board */
    else if (SOC_IS_HELIX4(unit))
    {
      cellsize = 208;
      totalmem = 20070;
    }
    /* CXO160G and TA48GE boards */
    else if (SOC_IS_TRIUMPH3(unit))
    {
      cellsize = 208;
      totalmem = 24576;
    }
    /* TG16G board */
    else if (SOC_IS_VALKYRIE2(unit))
    {
      cellsize = 128;
      totalmem = 24576;
    }
    else if (SOC_IS_SC_CQ(unit)) 
    {
      cellsize = 128;
      totalmem = 16383;
    }

    /* subtract 1 from NUM_PORT to correct for CPU port, which never has WRED or taildrop */
    conversion_factor = cellsize * 8 * totalmem / (1 + 2 * (NUM_PORT(unit)-1));
  }

  return ((percent * conversion_factor) / 100);
}

/*********************************************************************
*
* @purpose Set WRED parameters on a port
*
* @param   unit - the local unit
* @param   port - BCMX Lport
* @param   setget - unused
* @param   args - Pointer to WRED params for all queues/colors
*
* @returns BCM Error Code
*
* @comments Companion to customx_port_wred_set(), this function runs 
*           on each unit.
* @end
*
*********************************************************************/
int usl_bcm_port_wred_set(int unit, bcm_port_t port, 
                          usl_bcm_port_wred_config_t *wredParams)                          
{
  int                         queueIndex, precIndex, rv = BCM_E_NONE;
  bcm_cosq_gport_discard_t    discardParams;
  bcm_gport_t                 gport;

        
  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    for (queueIndex=0; queueIndex<L7_MAX_CFG_QUEUES_PER_PORT; queueIndex++) 
    {
      discardParams.gain = wredParams->gain[queueIndex];
      /* PTin modified: Allow 6 DP levels */
      for (precIndex=0; precIndex<(L7_MAX_CFG_DROP_PREC_LEVELS*2); precIndex++) 
      {
        discardParams.flags = BCM_COSQ_DISCARD_BYTES;
        if (wredParams->flags[queueIndex] & BCM_COSQ_DISCARD_CAP_AVERAGE) 
        {
          discardParams.flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
        }
        if (wredParams->flags[queueIndex] & BCM_COSQ_DISCARD_ENABLE)
	    {
  	        discardParams.flags |= BCM_COSQ_DISCARD_ENABLE;
	    }
        discardParams.drop_probability = wredParams->dropProb[queueIndex][precIndex];
        discardParams.min_thresh = 
         l7_port_wred_percent_to_bytes(unit, wredParams->minThreshold[queueIndex][precIndex]);
        discardParams.max_thresh = 
         l7_port_wred_percent_to_bytes(unit, wredParams->maxThreshold[queueIndex][precIndex]);

        /* PTin modified: Allow 6 DP levels for non Valkyrie2 devices */
        if (SOC_IS_VALKYRIE2(unit)) 
        {
          /* For Valkyrie 2, only 4 DP levels are defined */
          if (precIndex >= (L7_MAX_CFG_DROP_PREC_LEVELS+1))
            break;

          /* Flags */
          switch (precIndex) 
          {
          case 0:
              discardParams.flags |= BCM_COSQ_DISCARD_COLOR_GREEN;
              break;
            case 1:
              discardParams.flags |= BCM_COSQ_DISCARD_COLOR_YELLOW;
              break;
            case 2:
              discardParams.flags |= BCM_COSQ_DISCARD_COLOR_RED;
               break;
            case 3:
              discardParams.flags |= BCM_COSQ_DISCARD_NONTCP;
              break;
            default:
              return(BCM_E_PARAM);
          }
        }
        else
        {
          /* Flags */
          switch (precIndex) 
          {
            case 0:
              discardParams.flags |= (BCM_COSQ_DISCARD_COLOR_GREEN | BCM_COSQ_DISCARD_TCP);
              break;
            case 1:
              discardParams.flags |= (BCM_COSQ_DISCARD_COLOR_YELLOW | BCM_COSQ_DISCARD_TCP);
              break;
            case 2:
              discardParams.flags |= (BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_TCP);
               break;
            case 3:
              discardParams.flags |= (BCM_COSQ_DISCARD_COLOR_GREEN | BCM_COSQ_DISCARD_NONTCP);
              break;
            case 4:
              discardParams.flags |= (BCM_COSQ_DISCARD_COLOR_YELLOW | BCM_COSQ_DISCARD_NONTCP);
              break;
            case 5:
              discardParams.flags |= (BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_NONTCP);
               break;
            default:
              return(BCM_E_PARAM);
          }
        }

        rv = bcm_cosq_gport_discard_set(unit, wredParams->bcm_gport, 
                                        queueIndex, &discardParams);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;    
        }
      } /* End for each color */
    } /* End for each queue */
  }

  /* Update the USL port database only on non-management units */
  if ((USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE) && 
      (L7_BCMX_OK(rv) == L7_TRUE))
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_db_port_wred_set(USL_CURRENT_DB, gport, wredParams); 
    }
  }

  return rv;
}


/*********************************************************************
*
* @purpose BCM API to to set the sflow configuration for the port
*
* @param    unit         @{(input)} Local bcm unit number
* @param    port         @{(input)} Local bcm port number 
* @param    sflowConfig  @{(input)} sflow config
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_sflow_config_set(int unit, bcm_port_t port, 
                                  usl_bcm_port_sflow_config_t *sflowConfig)
{
  int             rv = BCM_E_NONE;
  bcm_gport_t     gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_port_sample_rate_set(unit, port, 
                                  sflowConfig->ingressSamplingRate,
                                  sflowConfig->egressSamplingRate); 
  }

  /* Update the USL port database only on non-management units */
  if ((USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE) && 
      (L7_BCMX_OK(rv) == L7_TRUE))
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_db_port_sflow_config_set(USL_CURRENT_DB, gport, sflowConfig); 
    }
  }

  return rv;

}

/*********************************************************************
*
* @purpose Configure ingress VLAN translation for a port
*
* @param   unit
* @param   port
* @param   enable
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_translate_ingress_enable_set(int unit, bcm_port_t port, L7_BOOL enable)
{
  int             rv    = BCM_E_NONE;
  bcm_gport_t     gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_vlan_control_port_set(unit, port, bcmVlanTranslateIngressEnable, enable);
  }

  /* Update the USL port database only on non-management units */
  if ((USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE) && 
      (L7_BCMX_OK(rv) == L7_TRUE))
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_db_port_vlan_translate_ingress_enable_set(USL_CURRENT_DB, gport, enable); 
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose Configure ingress VLAN translation for a port
*
* @param   unit
* @param   port
* @param   drop
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_translate_ingress_miss_drop_set(int unit, bcm_port_t port, L7_BOOL drop)
{
  int             rv    = BCM_E_NONE;
  bcm_gport_t     gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_vlan_control_port_set(unit, port, bcmVlanTranslateIngressMissDrop, drop);
  }

  /* Update the USL port database only on non-management units */
  if ((USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE) && 
      (L7_BCMX_OK(rv) == L7_TRUE))
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_db_port_vlan_translate_ingress_miss_drop_set(USL_CURRENT_DB, gport, drop);
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose Configure egress VLAN translation for a port
*
* @param   unit
* @param   port
* @param   enable
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_translate_egress_enable_set(int unit, bcm_port_t port, L7_BOOL enable)
{
  int             rv    = BCM_E_NONE;
  bcm_gport_t     gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_vlan_control_port_set(unit, port, bcmVlanTranslateEgressEnable, enable);
  }

  /* Update the USL port database only on non-management units */
  if ((USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE) && 
      (L7_BCMX_OK(rv) == L7_TRUE))
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_db_port_vlan_translate_egress_enable_set(USL_CURRENT_DB, gport, enable); 
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose Configure egress VLAN translation for a port
*
* @param   unit
* @param   port
* @param   drop
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_translate_egress_miss_drop_set(int unit, bcm_port_t port, L7_BOOL drop)
{
  int             rv    = BCM_E_NONE;
  bcm_gport_t     gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_vlan_control_port_set(unit, port, bcmVlanTranslateEgressMissDrop, drop);
  }

  /* Update the USL port database only on non-management units */
  if ((USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE) && 
      (L7_BCMX_OK(rv) == L7_TRUE))
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_db_port_vlan_translate_egress_miss_drop_set(USL_CURRENT_DB, gport, drop); 
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose Configure VLAN translation key for a port
*
* @param   unit
* @param   port
* @param   key
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_translate_key_first_set(int unit, bcm_port_t port, bcm_vlan_translate_key_t key)
{
  int             rv    = BCM_E_NONE;
  bcm_gport_t     gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_vlan_control_port_set(unit, port, bcmVlanPortTranslateKeyFirst, key);
  }

  /* Update the USL port database only on non-management units */
  if ((USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE) && 
      (L7_BCMX_OK(rv) == L7_TRUE))
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_db_port_vlan_translate_key_first_set(USL_CURRENT_DB, gport, key); 
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose Configure VLAN translation key for a port
*
* @param   unit
* @param   port
* @param   key
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int usl_bcm_port_vlan_translate_key_second_set(int unit, bcm_port_t port, bcm_vlan_translate_key_t key)
{
  int             rv    = BCM_E_NONE;
  bcm_gport_t     gport = BCM_GPORT_INVALID;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
    rv = bcm_vlan_control_port_set(unit, port, bcmVlanPortTranslateKeySecond, key);
  }

  /* Update the USL port database only on non-management units */
  if ((USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE) && 
      (L7_BCMX_OK(rv) == L7_TRUE))
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_db_port_vlan_translate_key_second_set(USL_CURRENT_DB, gport, key); 
    }
  }

  return rv;
}
/*********************************************************************
*
* @purpose BCM API to to set the PFC configuration for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    pfcConfig     @{(input)} pfc parameters
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_pfc_config_set(int unit, 
                                bcm_port_t port,
                                usl_bcm_port_pfc_config_t *pfcConfig )
{
  int                                rv = BCM_E_NONE;
  bcm_gport_t                        gport = BCM_GPORT_INVALID;
  int ena,speed,tx_pause,rx_pause;
  bcm_port_abil_t ability_mask;

  if (!soc_feature(unit, soc_feature_priority_flow_control)) 
  {
    return BCM_E_UNAVAIL;
  }

  /* This feature is only available on GX ports */
  if (!IS_GX_PORT(unit, port)) 
  {
    return BCM_E_UNAVAIL;
  }

  ena=pfcConfig->mode;

  /* Check if the hardware should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_PORT_DB_ID) == L7_TRUE)
  {
     do {
        /* enable the MAC */
        rv = bcm_port_control_set(unit, port,
                                  bcmPortControlPFCReceive,
                                  (ena==L7_TRUE)?1:0);
        if (rv != BCM_E_NONE) break;

        rv = bcm_port_control_set(unit, port,
                                  bcmPortControlPFCTransmit,
                                  (ena==L7_TRUE)? 1:0);
        if (rv != BCM_E_NONE) break;

        rv = bcm_port_control_set(unit, port,
                                  bcmPortControlPFCPassFrames,
                                  (ena==L7_TRUE)? 1:0);
        if (rv != BCM_E_NONE) break;

        rv = bcm_port_control_set(unit, port, bcmPortControlPFCClasses,8);
        if (rv != BCM_E_NONE) break;

        /* handle flow control vs pfc priority */
        rv= bcm_port_advert_get(unit, port, &ability_mask);
        if (rv != BCM_E_NONE) break;
        rv = bcm_port_speed_get(unit, port, &speed);
        if (rv != BCM_E_NONE) break;
        rv = bcm_port_pause_get(unit,port,&tx_pause,&rx_pause);
        if (rv != BCM_E_NONE) break;
        
        if (ena)
        {
          /* PFC is being enabled, need to make sure flow control
           * parms are correctly handled with the chip
           */
          if ((ability_mask & BCM_PORT_ABIL_PAUSE_RX) ||
              (ability_mask & BCM_PORT_ABIL_PAUSE_TX))
          {
            /* always off if pfc is being enabled */
            ability_mask &= ~BCM_PORT_ABIL_PAUSE_RX;
            ability_mask &= ~BCM_PORT_ABIL_PAUSE_TX;
        
            rv = bcm_port_advert_set(unit, port, ability_mask);
          }          

          if (speed < 10000) {
            /* speed is not 10G, need to enable pause for PFC 
             * see the errata
             */
            if ((tx_pause == 0) || (rx_pause == 0))
            {
              rv = bcm_port_pause_set(unit, port, 1,1);
            }
          }
          else
          {
            if ((tx_pause != 0) || (rx_pause != 0))
            {
              /* speed is 10G, need to disable pause */
              rv = bcm_port_pause_set(unit, port, 0,0);
            }
          }
        } /* ends enabled */
        else 
        {
          /* pfc is disabled, see if flow control should be re-enabled */
          L7_uint32 mode; 
          L7_uchar8 mac[6];

          usl_db_flow_control_get(USL_OPERATIONAL_DB , &mode, mac);

          if (mode)
          {
            if (!(ability_mask & BCM_PORT_ABIL_PAUSE_RX) ||
                !(ability_mask & BCM_PORT_ABIL_PAUSE_TX))
            {
              /* we are disabling pfc, so now we must set the pause if enabled */
              ability_mask |= BCM_PORT_ABIL_PAUSE_RX;
              ability_mask |= BCM_PORT_ABIL_PAUSE_TX;

              rv = bcm_port_advert_set(unit, port, ability_mask);
            }

            if ((tx_pause == 0) || (rx_pause == 0))
            {
              rv = bcm_port_pause_set(unit, port, 1,1);
            }
          }
        }
      } while (0);
  
    if (BCM_E_NONE == rv)
    {
      /* update the mmu with the changes */ 
      rv = hapiBroadMmuPortPfcSet(unit,port,ena,pfcConfig->no_drop_priority_bmp);
    }
  }

  /* Update the USL port database only on non-management units */
  if ((USL_BCM_CONFIGURE_DB(USL_PORT_DB_ID) == L7_TRUE) && 
      (L7_BCMX_OK(rv) == L7_TRUE))
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_portdb_pfc_config_set(USL_CURRENT_DB, gport, pfcConfig); 
    }
  }

  return rv;
}


/* use the DAPI_PFC_STATS_t values as the key */
int pfc_stat_map[DAPI_STATS_PFC_ENTRIES][2] =
      {{BMAC_PFC_COS0_XOFF_CNTr,MAC_PFC_COS0_XOFF_CNTr},
       {BMAC_PFC_COS1_XOFF_CNTr,MAC_PFC_COS1_XOFF_CNTr},
       {BMAC_PFC_COS2_XOFF_CNTr,MAC_PFC_COS2_XOFF_CNTr},
       {BMAC_PFC_COS3_XOFF_CNTr,MAC_PFC_COS3_XOFF_CNTr},
       {BMAC_PFC_COS4_XOFF_CNTr,MAC_PFC_COS4_XOFF_CNTr},
       {BMAC_PFC_COS5_XOFF_CNTr,MAC_PFC_COS5_XOFF_CNTr},
       {BMAC_PFC_COS6_XOFF_CNTr,MAC_PFC_COS6_XOFF_CNTr},
       {BMAC_PFC_COS7_XOFF_CNTr,MAC_PFC_COS7_XOFF_CNTr},
       {IRXPPr,GRXPPr},
       {ITXPPr,GTXPPr}};

/*********************************************************************
*
* @purpose BCM API to to get the PFC stat for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
* @param    stat          @{(inout)} The id to get and ctr to return
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_pfc_stat_get(int unit, 
                              bcm_port_t port,
                              usl_bcm_port_pfc_stat_t *stat )
{
  int row,col;    
  int rv = BCM_E_FAIL,tmp_rv;
  unsigned int ctr = 0,tmp=0;

  if (!soc_feature(unit, soc_feature_priority_flow_control)) return BCM_E_UNAVAIL;

  row = stat->stat_id;

  if (row >= DAPI_STATS_PFC_ENTRIES) return BCM_E_FAIL;

  for (col = 0; col < 2; col++)
  {
    tmp_rv = soc_counter_get32(unit,port,pfc_stat_map[row][col],0,&tmp);
    if (tmp_rv != SOC_E_NONE)
    {
      /* if either read passes, then both pass */
      if (rv == BCM_E_FAIL)
      {
        rv = tmp_rv;
      }
    }
    else
    {
      rv = tmp_rv;
      ctr+=tmp;
    }
  }
  
  stat->ctr = ctr;

  return rv;
}

/*********************************************************************
*
* @purpose BCM API to to clear the PFC stats for the port
*
* @param    unit          @{(input)} Local bcm unit number
* @param    port          @{(input)} Local bcm port number 
*
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_port_pfc_stats_clear(int unit, bcm_port_t port)
{
  int rv = BCM_E_FAIL,tmp_rv=BCM_E_FAIL;
  int row,col;

  if (!soc_feature(unit, soc_feature_priority_flow_control)) return BCM_E_UNAVAIL;

  for (row=0; row < DAPI_STATS_PFC_ENTRIES;row++)
  {
    for (col=0;col < 2; col++)
    {
      tmp_rv = soc_counter_set32(unit,port,pfc_stat_map[row][col],0,0);
      if (tmp_rv != SOC_E_NONE)
      {
        /* if either read passes, then both pass */
        if (rv == BCM_E_FAIL)
        {
          rv = tmp_rv;
        }
      }
      else
      {
        rv = tmp_rv;
      }
    }
  }

  return rv;
}
