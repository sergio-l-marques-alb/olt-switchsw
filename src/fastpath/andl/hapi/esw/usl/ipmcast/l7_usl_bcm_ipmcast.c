
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_ipmcast.c
*
* @purpose    USL BCM APIs for IPMCAST
*
* @component  HAPI
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#include "l7_common.h"

#ifdef L7_MCAST_PACKAGE

#include "osapi.h"
#include "log.h"
#include "soc/drv.h"
#include "bcmx/ipmc.h"
#include "bcmx/custom.h"
#include "bcmx/bcmx_int.h"
#include "bcm_int/common/multicast.h" /* PTin modified: new SDK  (esw->common) */

#include "l7_usl_bcm_ipmcast.h"
#include "l7_usl_ipmcast_db.h"
#include "l7_usl_sm.h"

#include "logger.h"

#define RV_REPLACE(_trv, _rv)                   \
        BCMX_RV_REPLACE_OK(_trv, _rv, BCM_E_UNAVAIL)

/*********************************************************************
* @purpose  Convert from USL BCM to SDK BCM format
*
* @param    ipmc    @{(input)} USL BCM address
* @param    bcmUnit @{(input)} Bcm unit number
* @param    bcmAddr @{(output)} SDK BCM ipmc address
*
* @returns  BCM Return code
*
* @end
*********************************************************************/
int l7_ipmc_to_bcm(usl_bcm_ipmc_addr_t  *ipmc,
                   L7_int32 bcm_unit,
                   bcm_ipmc_addr_t *bcm_ipmc)
{
  int rv = BCM_E_NONE, my_modid;

  bcm_ipmc_addr_t_init(bcm_ipmc);

  rv = bcm_stk_my_modid_get(bcm_unit, &my_modid);
  if (rv != BCM_E_NONE)
  {
    return rv;
  }
   
  bcm_ipmc->vid = ipmc->vid;
    
  /* This addr are initialized explicitly */
  if (ipmc->flags & BCM_IPMC_IP6)
  {
    memcpy(bcm_ipmc->s_ip6_addr,  ipmc->s_ip_addr,  L7_IPV6_ADDRESS_SIZE);
    memcpy(bcm_ipmc->mc_ip6_addr, ipmc->mc_ip_addr, L7_IPV6_ADDRESS_SIZE);
    bcm_ipmc->flags |= BCM_IPMC_IP6; /* Set the flag for ipv6 */
  }
  else
  {
    memcpy(&(bcm_ipmc->s_ip_addr),  ipmc->s_ip_addr,  L7_IPV4_ADDRESS_SIZE);
    memcpy(&(bcm_ipmc->mc_ip_addr), ipmc->mc_ip_addr, L7_IPV4_ADDRESS_SIZE);
  }
    
  bcm_ipmc->cos = ipmc->cos;
  bcm_ipmc->ts = ipmc->ts;
  bcm_ipmc->port_tgid = ipmc->port_tgid;
  bcm_ipmc->mod_id = ipmc->mod_id; 
  bcm_ipmc->v = 1;   /* VALID */
  bcm_ipmc->flags = ipmc->flags;

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcm_ipmc->group = ipmc->ipmc_index;

  bcm_ipmc->vrf = ipmc->vrf;
  bcm_ipmc->lookup_class = ipmc->lookup_class;
  bcm_ipmc->distribution_class = ipmc->distribution_class;

  /* Invalid id */
  if (ipmc->ipmc_index <= 0)
  {
    rv = BCM_E_BADID;
  }
  #else
  bcm_ipmc->ipmc_index = ipmc->ipmc_index; 
  if (ipmc->ipmc_index >= 0)
  {
    bcm_ipmc->flags |= BCM_IPMC_USE_IPMC_INDEX;
  }
  else
  {
    bcm_ipmc->flags &= ~BCM_IPMC_USE_IPMC_INDEX;
  }

  BCM_PBMP_ASSIGN (bcm_ipmc->l2_pbmp, ipmc->l2_pbmp[my_modid]);
  BCM_PBMP_OR(bcm_ipmc->l2_pbmp, PBMP_HG_ALL(bcm_unit));
  BCM_PBMP_OR(bcm_ipmc->l2_pbmp, PBMP_HL_ALL(bcm_unit));
  BCM_PBMP_ASSIGN (bcm_ipmc->l2_ubmp, ipmc->l2_ubmp[my_modid]);
  BCM_PBMP_ASSIGN (bcm_ipmc->l3_pbmp, ipmc->l3_pbmp[my_modid]);
  #endif

  return rv;
}

/*********************************************************************
* @purpose  USL BCM API to handle multicast route adds.
*
* @param    ipmc            {(input)} IPMC Group Info
*           replace_entry   {(input)} Indicates if this replaces an existing entry.
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
int usl_bcm_ipmc_add (usl_bcm_ipmc_addr_t  *ipmc, L7_BOOL replace_entry)
{
  L7_uint32 bcm_unit;
  int rv = BCM_E_NONE;
  bcm_pbmp_t    pbmp;
  L7_uint32     port;
  bcm_ipmc_addr_t bcm_ipmc;
  /* PTin added: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcm_multicast_t group;
  L7_uint32 flags;
  L7_BOOL   create_group;
  #endif

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
  {
    /* send the request to all of the local units */
    for (bcm_unit=0;bcm_unit < soc_ndev;bcm_unit++)
    {
      if (!BCM_IS_FABRIC(bcm_unit))
      {
        /* PTin added: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))

        /* Input group id */
        group = ipmc->ipmc_index;

        /* Default flags */
        flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_TYPE_L3;
        
        /* Create group, by default */
        create_group = L7_TRUE;

        /* Check if provided group is valid */
        if ( group > 0)
        {
          rv = bcm_multicast_group_is_free(bcm_unit, group);
          if (rv != BCM_E_NONE || rv != BCM_E_EXISTS)
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Invalid group index %d", group);
            break;
          }

          /* If group id exists, no need to create it */
          if (rv == BCM_E_EXISTS)
          {
            create_group = L7_FALSE;
          }
          /* Otherwise, create group with specific group id */
          else
          {
            flags |= BCM_MULTICAST_WITH_ID;
          }
        }

        LOG_TRACE(LOG_CTX_PTIN_HAPI,"Group id %d will %s created (flags=0x%08x)",group,((create_group) ? "BE" : "NOT be"),flags);

        /* Create group, if necessary */
        if (create_group)
        {
          rv = bcm_multicast_create(bcm_unit, flags, &group);
          if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
          {
            break;
          }
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"Group id %d reated (with flags=0x%08x)",group,flags);
        }

        /* Save group id */
        ipmc->ipmc_index = group;
        bcm_ipmc.group   = group;
        #endif

        /* populate the SDK bcm structure from USL bcm structure for this unit */
        rv = l7_ipmc_to_bcm(ipmc, bcm_unit, &bcm_ipmc);  
        if (rv != BCM_E_NONE && rv != BCM_E_BADID)  /* PTin modified: SDK 6.3.0 */
        {
          break;    
        }

        if (usl_db_ipmc_rpf_check_mode_get() == L7_FALSE)
        {
          bcm_ipmc.flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
        }
        if (replace_entry)
        {
          bcm_ipmc.flags |= BCM_IPMC_REPLACE;
        }

        rv = bcm_ipmc_add(bcm_unit, &bcm_ipmc);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }
        LOG_TRACE(LOG_CTX_PTIN_HAPI,"IPMC added");
      }
      else
      {
        /* must be a fabric chip, add the entry here also */
        BCM_PBMP_CLEAR(pbmp);

        BCM_PBMP_OR(pbmp,PBMP_HG_ALL(bcm_unit));

        BCM_PBMP_ITER(pbmp, port)
        {
          /* PTin modified: SDK 6.3.0 */
          #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
          rv = bcm_ipmc_bitmap_set(bcm_unit, ipmc->ipmc_index, port, pbmp);
          #else
          rv = bcm_ipmc_bitmap_set(bcm_unit, bcm_ipmc.ipmc_index, port, pbmp);
          #endif
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            break;
          }
        }
      }
    }
  }

  /* Update the USL db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_IPMC_ROUTE_DB_ID) == L7_TRUE))
  {
    rv = usl_db_ipmc_add(ipmc, replace_entry);
  }

  return  rv;  
}


/*********************************************************************
* @purpose  Custom RPC function to handle multicast route deletes.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
int usl_bcm_ipmc_delete (usl_bcm_ipmc_addr_t *ipmc, L7_uint32 keep)
{
  L7_uint32            bcm_unit;
  int                  rv = BCM_E_NONE;
  bcm_pbmp_t           pbmp, empty;
  L7_uint32            port;
  bcm_ipmc_addr_t      bcm_ipmc;
  bcm_vlan_vector_t    vlan_vector;
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcm_vlan_vector_t    vlan_vector_empty;
  #endif
  
  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
  {
    for (bcm_unit=0;bcm_unit < soc_ndev;bcm_unit++)
    {
      if (BCM_IS_FABRIC(bcm_unit))
      {
        continue;
      }

      /* Set the replication config for all L3 ports to an empty VLAN vector. */
      memset (&bcm_ipmc, 0, sizeof (bcm_ipmc));

      /* Ptin TODO: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      /* Remove replication vectors */
      if (ipmc->ipmc_index > 0)
      {
        /* Run all ports, and check if there are replicators to be removed */
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_OR(pbmp, PBMP_FE_ALL(bcm_unit));
        BCM_PBMP_OR(pbmp, PBMP_GE_ALL(bcm_unit));
        BCM_PBMP_OR(pbmp, PBMP_XE_ALL(bcm_unit));
        BCM_PBMP_OR(pbmp, PBMP_HG_ALL(bcm_unit));
        BCM_PBMP_OR(pbmp, PBMP_HL_ALL(bcm_unit));

        BCM_VLAN_VEC_ZERO(vlan_vector_empty);

        BCM_PBMP_ITER(pbmp, port)
        {
          /* Only consider ports, where there is replication vectors */
          if (bcm_multicast_repl_get(bcm_unit, ipmc->ipmc_index, port, vlan_vector) == BCM_E_NONE)
          {
            rv = bcm_multicast_repl_set(bcm_unit, ipmc->ipmc_index, port, vlan_vector_empty);
            if (rv != BCM_E_NONE)
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't clear VLAN vector for IPMC index %d, unit %d, port %d, rv %d", 
                      ipmc->ipmc_index, bcm_unit, port, rv);
            }
          }
        }
      }
      #else
      rv = bcm_ipmc_get_by_index(bcm_unit, ipmc->ipmc_index, &bcm_ipmc);
      if (rv == BCM_E_NONE)
      {
        BCM_VLAN_VEC_ZERO(vlan_vector);

        BCM_PBMP_ITER(bcm_ipmc.l3_pbmp, port)
        {
          rv = bcm_ipmc_repl_set(bcm_unit, ipmc->ipmc_index, port, vlan_vector);
          if (rv != BCM_E_NONE)
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't clear VLAN vector for IPMC index %d, unit %d, port %d, rv %d", 
                    ipmc->ipmc_index, bcm_unit, port, rv);
          }
        }
      }
      #endif

      rv = l7_ipmc_to_bcm(ipmc, bcm_unit, &bcm_ipmc);

      if (rv != BCM_E_NONE)
      {
        break;
      }

      if (keep)
      {   
        bcm_ipmc.flags |= BCM_IPMC_KEEP_ENTRY;
      }
      else 
      {
        bcm_ipmc.flags &= ~BCM_IPMC_KEEP_ENTRY;  
      }
    
      rv = bcm_ipmc_remove(bcm_unit, &bcm_ipmc);
    
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        break;
      }

      LOG_TRACE(LOG_CTX_PTIN_HAPI,"IPMC removed");

      /* PTin added: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      rv = bcm_multicast_destroy(bcm_unit, ipmc->ipmc_index);

      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        break;
      }

      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Group id %d destroyed",ipmc->ipmc_index);

      ipmc->ipmc_index = -1;
      bcm_ipmc.group   = -1;
      #endif
    }

    /* Delete the entry to the local fabric chips if we have an mc index */
    if (L7_BCMX_OK(rv) == L7_TRUE)
    {
      /* delete the local fabric */
      for (bcm_unit=0;bcm_unit < soc_ndev;bcm_unit++)
      {
        if (BCM_IS_FABRIC(bcm_unit))
        {
          BCM_PBMP_CLEAR(pbmp);
          BCM_PBMP_CLEAR(empty);

          BCM_PBMP_OR(pbmp,PBMP_HG_ALL(bcm_unit));

          BCM_PBMP_ITER(pbmp, port)
          {
            rv = bcm_ipmc_bitmap_set(bcm_unit, ipmc->ipmc_index, port, empty);
            if (L7_BCMX_OK(rv) != L7_TRUE)
            {
              break;
            }
          }
        }
      } /* end the fabric writes */
    }
  }

  /* Update the USL Db  */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_IPMC_ROUTE_DB_ID) == L7_TRUE))
  {
    rv = usl_db_ipmc_remove(ipmc);
  }

  return rv;  
}


/*********************************************************************
* @purpose  USL BCM API to handle modifications to the
*           source interface when processing RPF failures.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
int usl_bcm_ipmc_rpf_set (usl_bcm_ipmc_addr_t  *ipmc)
{
  L7_uint32            bcm_unit;
  int                  rv = BCM_E_NONE;
  bcm_ipmc_addr_t      bcm_ipmc;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
  {
    for (bcm_unit=0;bcm_unit < soc_ndev;bcm_unit++)
    {
      if (BCM_IS_FABRIC(bcm_unit))
      {
        continue;
      }

      memset (&bcm_ipmc, 0, sizeof (bcm_ipmc));
      /* PTin modified: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      /* Get IPMC data */
      rv = l7_ipmc_to_bcm(ipmc, bcm_unit, &bcm_ipmc);
      if (rv != BCM_E_NONE)
      {
        break;
      }
      rv = bcm_ipmc_find(bcm_unit, &bcm_ipmc);
      if (rv != BCM_E_NONE)
      {
        break;
      }
      #else
      rv = bcm_ipmc_get_by_index(bcm_unit, ipmc->ipmc_index, &bcm_ipmc);
      #endif
      if (rv != BCM_E_NONE)
      {
        break;
      }

      if (!bcm_ipmc.v) 
      {
        rv = BCM_E_FAIL;
        break;
      }

      bcm_ipmc.mod_id = ipmc->mod_id;
      bcm_ipmc.port_tgid = ipmc->port_tgid;
      bcm_ipmc.ts = ipmc->ts;
      bcm_ipmc.flags |= BCM_IPMC_REPLACE;

      if (usl_db_ipmc_rpf_check_mode_get() == L7_FALSE)
      {
        bcm_ipmc.flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
      }
      rv = bcm_ipmc_add(bcm_unit, &bcm_ipmc);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        break;
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_IPMC_ROUTE_DB_ID) == L7_TRUE))
  {
    rv = usl_db_ipmc_port_tgid_set(ipmc);
  }


  return rv;  
}


/*********************************************************************
*
* @purpose Sets the L2 ports for the multicast group.
*
* @param   port - 
*
* @returns BCM Error Code
*
* @notes This function sets the L2 ports for the specified group
*
* @end
*
*********************************************************************/
L7_RC_t usl_bcm_ipmc_set_l2_ports (usl_bcm_ipmc_addr_t *ipmc_addr)
{
  L7_int32            bcm_unit, modid;
  L7_int32            index;
  int                 rv = BCM_E_NONE;
  bcm_ipmc_addr_t     bcm_ipmc;
  /* PTin added: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcm_port_t port;
  bcm_gport_t gport;
  bcm_if_t encap_id;
  #endif

  memset (&bcm_ipmc, 0, sizeof (bcm_ipmc));

  index = ipmc_addr->ipmc_index;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
  {
    for (bcm_unit=0; bcm_unit < soc_ndev; bcm_unit++)
    {
      if (BCM_IS_FABRIC(bcm_unit))
      {
        continue;
      }
   
      /* TODO: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      rv = l7_ipmc_to_bcm(ipmc_addr, bcm_unit, &bcm_ipmc);
      #else
      rv = bcm_ipmc_get_by_index(bcm_unit, index, &bcm_ipmc);
      #endif
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        break;
      }

      if (!bcm_ipmc.v) 
      {
        rv = BCM_E_FAIL;
        break;
      }

      rv = bcm_stk_my_modid_get(bcm_unit, &modid);
      if (rv != BCM_E_NONE)
      {
        break;
      }

      /* PTin removed: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Adding L2 ports");
      /* Add L2 ports */
      BCM_PBMP_ITER(ipmc_addr->l2_pbmp[modid], port)
      {
        /* Get gport */
        rv = bcm_port_gport_get(bcm_unit, port, &gport);
        if (rv != BCM_E_NONE)
          break;

        /* Get encap id */
        rv = bcm_multicast_l2_encap_get(bcm_unit, ipmc_addr->ipmc_index, gport, ipmc_addr->vid, &encap_id);
        if (rv != BCM_E_NONE)
          break;

        /* Add egress port */
        rv = bcm_multicast_egress_add(bcm_unit, ipmc_addr->ipmc_index, gport, encap_id);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
          break;

        LOG_TRACE(LOG_CTX_PTIN_HAPI,"Added bcm_port %d (gport=0x%08x)",port,gport);
      }
      /* If error, undo procedure */
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        (void) bcm_multicast_egress_delete_all(bcm_unit, ipmc_addr->ipmc_index);
        break;
      }

      #if 0
      /* Add L3 ports */
      BCM_PBMP_ITER(ipmc_addr->l3_pbmp[modid], port)
      {
        /* Get gport */
        rv = bcm_port_gport_get(bcm_unit, port, &gport);
        if (rv != BCM_E_NONE)
          break;

        /* Get encap id */
        rv = bcm_multicast_l3_encap_get(bcm_unit, ipmc_addr->ipmc_index, gport, ipmc_addr->vid, &encap_id);
        if (rv != BCM_E_NONE)
          break;

        /* Add egress port */
        rv = bcm_multicast_egress_add(bcm_unit, ipmc_addr->ipmc_index, gport, encap_id);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
          break;
      }
      /* If error, undo procedure */
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        (void) bcm_multicast_egress_delete_all(bcm_unit, ipmc_addr->ipmc_index);
        break;
      }
      #endif
      #else
      BCM_PBMP_ASSIGN (bcm_ipmc.l2_pbmp, ipmc_addr->l2_pbmp[modid]);
      BCM_PBMP_OR(bcm_ipmc.l2_pbmp, PBMP_HG_ALL(bcm_unit));
      BCM_PBMP_OR(bcm_ipmc.l2_pbmp, PBMP_HL_ALL(bcm_unit));
      BCM_PBMP_ASSIGN (bcm_ipmc.l2_ubmp, ipmc_addr->l2_ubmp[modid]);

      bcm_ipmc.flags |= BCM_IPMC_REPLACE;
      if (usl_db_ipmc_rpf_check_mode_get() == L7_FALSE)
      {
        bcm_ipmc.flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
      }

      rv = bcm_ipmc_add(bcm_unit, &bcm_ipmc);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        break;
      }
      #endif
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_IPMC_ROUTE_DB_ID) == L7_TRUE))
  {
    rv = usl_db_ipmc_l2_ports_update(ipmc_addr, USL_CMD_SET);
  }

  return rv;
}

/*********************************************************************
*
* @purpose Add L2 ports to the multicast group.
*
* @param   port - 
*
* @returns BCM Error Code
*
* @notes This function adds L2 ports to the specified groups 
*
* @end
*
*********************************************************************/
int usl_bcm_ipmc_add_l2_port_groups (int unit, bcm_port_t port, 
                                     usl_bcmx_port_ipmc_cmd_t *ipmc_cmd)
{
  int                            rv = BCM_E_NONE;
  usl_bcm_ipmc_addr_t            l7_ipmc, l7_ipmc_tmp;
  bcm_ipmc_addr_t                ipmc;
  int                            index;
  /* PTin added: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcm_gport_t gport;
  bcm_if_t encap_id;
  #else
  bcm_pbmp_t                     tagged_pbmp, untagged_pbmp;
  #endif
  L7_uint32                      i;
  L7_int32                       myModid;

  rv = bcm_stk_my_modid_get(unit, &myModid);
  if (rv != BCM_E_NONE)
  {
    return rv; 
  }

  for (i = 0; i < (sizeof(ipmc_cmd->ipmc_index_mask) / sizeof(ipmc_cmd->ipmc_index_mask[0])); i++)
  {
    /* Check a word at a time for better efficiency. */
    if (ipmc_cmd->ipmc_index_mask[i] == 0)
    {
      continue;
    }

    /* Check IPMC groups w/in this word. */
    for (index = (i * 32); index < ((i + 1) * 32); index++)
    {
      /* Check to see if we're done w/ IPMC groups in this word. */
      if (ipmc_cmd->ipmc_index_mask[i] == 0)
      {
        break;
      }
      if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(index, ipmc_cmd->ipmc_index_mask) == 0)
      {
        continue;
      }

      /* Mark that we've processed this IPMC group. */
      USL_BCMX_PORT_IPMC_INDEX_MEMBER_CLEAR(index, ipmc_cmd->ipmc_index_mask);

      memset(&l7_ipmc, 0, sizeof(l7_ipmc));
      memset(&ipmc,    0, sizeof(ipmc));

      /* Check if the hw should be configured */
      if (USL_BCM_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
      {
        /* PTin modified: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        /* Valdiate group id */
        if (ipmc_cmd->ipmc_index <= 0)
        {
          rv = BCM_E_BADID;
          continue;
        }
        /* Get gport */
        rv = bcm_port_gport_get(unit, port, &gport);
        if (rv != BCM_E_NONE)
          continue;

        /* Get encap id */
        rv = bcm_multicast_l2_encap_get(unit, ipmc_cmd->ipmc_index, gport, ipmc_cmd->vlan_id, &encap_id);
        if (rv != BCM_E_NONE)
          continue;

        /* Add egress port */
        rv = bcm_multicast_egress_add(unit, ipmc_cmd->ipmc_index, gport, encap_id);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
          continue;

        LOG_TRACE(LOG_CTX_PTIN_HAPI,"Added bcm_port %d (gport=0x%08x)",port,gport);
        #else
        rv = bcm_ipmc_get_by_index(unit, index, &ipmc);
        if (rv != BCM_E_NONE)
        {
          continue;
        }

        if (!ipmc.v) {
          continue;
        }

        BCM_PBMP_PORT_SET (tagged_pbmp, port);
        if (ipmc_cmd->tagged)
        {
          BCM_PBMP_CLEAR (untagged_pbmp);
        } else
        {
          BCM_PBMP_PORT_SET (untagged_pbmp, port);
        }

        BCM_PBMP_OR(ipmc.l2_pbmp, tagged_pbmp);
        BCM_PBMP_REMOVE(ipmc.l2_ubmp, tagged_pbmp);
        BCM_PBMP_OR(ipmc.l2_ubmp, untagged_pbmp);

        ipmc.flags |= BCM_IPMC_REPLACE;
        if (usl_db_ipmc_rpf_check_mode_get() == L7_FALSE)
        {
          ipmc.flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
        }
        rv = bcm_ipmc_add(unit, &ipmc);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          continue;
        }
        #endif
      }

      if (USL_BCM_CONFIGURE_DB(USL_IPMC_ROUTE_DB_ID) == L7_FALSE)
      {
        continue; 
      }

      /* Update the USL db */
      rv = usl_db_ipmc_entry_get_by_index(index, &l7_ipmc_tmp);
      if (rv != BCM_E_NONE)
      {
        continue;
      }

      memcpy(&(l7_ipmc.s_ip_addr),  &(l7_ipmc_tmp.s_ip_addr),  sizeof(l7_ipmc.s_ip_addr));
      memcpy(&(l7_ipmc.mc_ip_addr), &(l7_ipmc_tmp.mc_ip_addr), sizeof(l7_ipmc.s_ip_addr));
      l7_ipmc.vid = l7_ipmc_tmp.vid;

      BCM_PBMP_PORT_ADD(l7_ipmc.l2_pbmp[myModid], port);

      if (ipmc_cmd->tagged == L7_FALSE)
      {
        BCM_PBMP_PORT_ADD(l7_ipmc.l2_ubmp[myModid], port);
      }

      rv = usl_db_ipmc_l2_ports_update(&l7_ipmc, USL_CMD_ADD);
    }
  }

  return BCM_E_NONE;
}


/*********************************************************************
*
* @purpose Delete L2 ports from the multicast group.
*
* @param   
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups 
*
* @end
*
*********************************************************************/
int usl_bcm_ipmc_delete_l2_port_groups (int unit, bcm_port_t port, 
                                        usl_bcmx_port_ipmc_cmd_t *ipmc_cmd)
{
  int                           rv = BCM_E_NONE;
  usl_bcm_ipmc_addr_t           l7_ipmc, l7_ipmc_tmp;
  bcm_ipmc_addr_t               ipmc;
  int                           index;
  L7_uint32                     i;
  L7_int32                      myModid;
  /* PTin added: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcm_gport_t gport;
  bcm_if_t encap_id;
  #endif

  rv = bcm_stk_my_modid_get(unit, &myModid);
  if (rv != BCM_E_NONE)
  {
    return rv; 
  }

  for (i = 0; i < (sizeof(ipmc_cmd->ipmc_index_mask) / sizeof(ipmc_cmd->ipmc_index_mask[0])); i++)
  {
    /* Check a word at a time for better efficiency. */
    if (ipmc_cmd->ipmc_index_mask[i] == 0)
    {
      continue;
    }

    /* Check IPMC groups w/in this word. */
    for (index = (i * 32); index < ((i + 1) * 32); index++)
    {
      /* Check to see if we're done w/ IPMC groups in this word. */
      if (ipmc_cmd->ipmc_index_mask[i] == 0)
      {
        break;
      }
      if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(index, ipmc_cmd->ipmc_index_mask) == 0)
      {
        continue;
      }

      /* Mark that we've processed this IPMC group. */
      USL_BCMX_PORT_IPMC_INDEX_MEMBER_CLEAR(index, ipmc_cmd->ipmc_index_mask);

      memset(&l7_ipmc, 0, sizeof(l7_ipmc));
      memset(&ipmc,    0, sizeof(ipmc));

      /* Check if the hw should be configured */
      if (USL_BCM_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
      {
        /* TODO: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        /* Valdiate group id */
        if (ipmc_cmd->ipmc_index <= 0)
        {
          rv = BCM_E_BADID;
          continue;
        }
        /* Get gport */
        rv = bcm_port_gport_get(unit, port, &gport);
        if (rv != BCM_E_NONE)
          continue;

        /* Get encap id */
        rv = bcm_multicast_l2_encap_get(unit, ipmc_cmd->ipmc_index, gport, ipmc_cmd->vlan_id, &encap_id);
        if (rv != BCM_E_NONE)
          continue;

        /* Add egress port */
        rv = bcm_multicast_egress_delete(unit, ipmc_cmd->ipmc_index, gport, encap_id);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
          continue;

        LOG_TRACE(LOG_CTX_PTIN_HAPI,"Removed bcm_port %d (gport=0x%08x)",port,gport);
        #else
        rv = bcm_ipmc_get_by_index(unit, index, &ipmc);
        if (rv != BCM_E_NONE)
        {
          continue;
        }

        if (!ipmc.v) 
        {
          continue;
        }

        BCM_PBMP_PORT_REMOVE(ipmc.l2_pbmp, port);
        BCM_PBMP_PORT_REMOVE(ipmc.l2_ubmp, port);

        ipmc.flags |= BCM_IPMC_REPLACE;
        if (usl_db_ipmc_rpf_check_mode_get() == L7_FALSE)
        {
          ipmc.flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
        }
        rv = bcm_ipmc_add(unit, &ipmc);

        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          continue;
        }
        #endif
      }

      if (USL_BCM_CONFIGURE_DB(USL_IPMC_ROUTE_DB_ID) == L7_FALSE)
      {
        continue; 
      }

      /* Update the USL Db */
      rv = usl_db_ipmc_entry_get_by_index(index, &l7_ipmc_tmp);
      if (rv != BCM_E_NONE)
      {
        continue;
      }

      memcpy(&(l7_ipmc.s_ip_addr),  &(l7_ipmc_tmp.s_ip_addr),  sizeof(l7_ipmc.s_ip_addr));
      memcpy(&(l7_ipmc.mc_ip_addr), &(l7_ipmc_tmp.mc_ip_addr), sizeof(l7_ipmc.s_ip_addr));
      l7_ipmc.vid = l7_ipmc_tmp.vid;

      BCM_PBMP_PORT_ADD(l7_ipmc.l2_pbmp[myModid], port);
      BCM_PBMP_PORT_ADD(l7_ipmc.l2_ubmp[myModid], port);


      rv = usl_db_ipmc_l2_ports_update(&l7_ipmc, USL_CMD_REMOVE);
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose Add L3 ports to the multicast group.
*
* @param   port - 
*
* @returns BCM Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int usl_bcm_ipmc_add_l3_port_groups (int unit, bcm_port_t port, 
                                     usl_bcmx_port_ipmc_cmd_t *ipmc_cmd)
{
  int                       rv = BCM_E_NONE;
  bcm_ipmc_addr_t           ipmc;
  int                       index;
  /* PTin added: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcm_gport_t gport;
  bcm_if_t encap_id;
  #else
  bcm_pbmp_t                l3_pbmp;
  #endif
  L7_uint32                 i;
  int                       ipmc_untag_flag;
  L7_uint32                 myModid;
  L7_uint32                 ipmc_group_index[L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL];
  L7_uint32                 num_ipmc_groups = 0;
  bcm_vlan_vector_t         vlan_vector;

  rv = bcm_stk_my_modid_get(unit, &myModid);
  if (rv != BCM_E_NONE)
  {
    return rv; 
  }

  for (i = 0; i < (sizeof(ipmc_cmd->ipmc_index_mask) / sizeof(ipmc_cmd->ipmc_index_mask[0])); i++)
  {
    /* Check a word at a time for better efficiency. */
    if (ipmc_cmd->ipmc_index_mask[i] != 0)
    {
      /* Check IPMC groups w/in this word. */
      for (index = (i * 32); index < ((i + 1) * 32); index++)
      {
        if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(index, ipmc_cmd->ipmc_index_mask))
        {
          ipmc_group_index[num_ipmc_groups] = index;
          num_ipmc_groups++;

          /* Mark that we've processed this IPMC group. */
          USL_BCMX_PORT_IPMC_INDEX_MEMBER_CLEAR(index, ipmc_cmd->ipmc_index_mask);
          /* Check to see if we're done w/ IPMC groups in this word. */
          if (ipmc_cmd->ipmc_index_mask[i] == 0)
          {
            break;
          }
        }
      }
    }
  }

  ipmc_untag_flag = !ipmc_cmd->tagged;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < num_ipmc_groups; i++)
    {
      index = ipmc_group_index[i];

      memset(&ipmc, 0, sizeof(ipmc));

      /* TODO: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      /* Validate group id */
      if (ipmc_cmd->ipmc_index <= 0)
      {
        rv = BCM_E_BADID;
        continue;
      }
      /* Get gport */
      rv = bcm_port_gport_get(unit, port, &gport);
      if (rv != BCM_E_NONE)
        continue;

      /* Get encap id */
      rv = bcm_multicast_l3_encap_get(unit, ipmc_cmd->ipmc_index, gport, ipmc_cmd->vlan_id, &encap_id);
      if (rv != BCM_E_NONE)
        continue;

      /* Add egress port */
      rv = bcm_multicast_egress_add(unit, ipmc_cmd->ipmc_index, gport, encap_id);
      if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        continue;

      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Added bcm_port %d (gport=0x%08x)",port,gport);

      #else
      rv = bcm_ipmc_get_by_index(unit, index, &ipmc);
      if (rv != BCM_E_NONE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't get IPMC entry for index %d, rv %d", index, rv);
        continue;
      }

      if (!ipmc.v) 
      {  
        continue;
      }

      BCM_PBMP_PORT_SET (l3_pbmp, port);
      BCM_PBMP_OR(ipmc.l3_pbmp, l3_pbmp);

      ipmc.flags |= BCM_IPMC_REPLACE;
      if (usl_db_ipmc_rpf_check_mode_get() == L7_FALSE)
      {
        ipmc.flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
      }
      rv = bcm_ipmc_add(unit, &ipmc); 

      if (rv != BCM_E_NONE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't add IPMC entry for index %d, rv %d", index, rv);
        continue;
      }
      #endif

      /* Set up VLAN replication.
      */
      /* TODO: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      rv = bcm_multicast_repl_get(unit, index, port, vlan_vector);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't get replication info for IPMC entry index %d, rv %d", index, rv);
        continue;
      }

      BCM_VLAN_VEC_SET(vlan_vector, ipmc_cmd->vlan_id);
      rv = bcm_multicast_repl_set(unit, index, port, vlan_vector);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set replication info for IPMC entry index %d, rv %d", index, rv);
        continue;
      }
      #else
      rv = bcm_ipmc_repl_get(unit, index, port, vlan_vector);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't get replication info for IPMC entry index %d, rv %d", index, rv);
        continue;
      }

      BCM_VLAN_VEC_SET(vlan_vector, ipmc_cmd->vlan_id);
      rv = bcm_ipmc_repl_set(unit, index, port, vlan_vector);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set replication info for IPMC entry index %d, rv %d", index, rv);
        continue;
      }
      #endif

      (void)bcm_ipmc_egress_port_set(unit, port, 
                                     ipmc_cmd->mac,
                                     ipmc_untag_flag, 
                                     ipmc_cmd->vlan_id,
                                     ipmc_cmd->ttl);
    }
  }

  if ((USL_BCM_CONFIGURE_DB(USL_IPMC_ROUTE_DB_ID) == L7_TRUE) &&
      (L7_BCMX_OK(rv) == L7_TRUE))
  {
    /* Update the USL Db */
    rv = usl_db_ipmc_egress_port_set(myModid, port, ipmc_cmd->vlan_id, ipmc_untag_flag);
    if (rv != BCM_E_NONE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "usl_db_ipmc_egress_port_set() returned %d", rv);
    }

    rv = usl_db_ipmc_update_l3_port_groups(myModid, port, 
                                           USL_CMD_ADD, ipmc_group_index, 
                                           num_ipmc_groups, ipmc_cmd->vlan_id);
  }

  return rv;
}

/*********************************************************************
*
* @purpose Add L3 ports to the multicast group.
*
* @param   port - 
*
* @returns BCM Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int usl_bcm_ipmc_egress_port_add (int unit, bcm_port_t port, 
                                  usl_bcmx_port_ipmc_cmd_t *ipmc_cmd)
{
  int                       rv = BCM_E_NONE;
  bcm_ipmc_addr_t           ipmc;
  int                       index;
  L7_uint32                 i;
  int                       ipmc_untag_flag;
  L7_uint32                 myModid;
  L7_uint32                 ipmc_group_index[L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL];
  L7_uint32                 num_ipmc_groups = 0;
  /* PTin added: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcm_gport_t gport;
  bcm_if_t encap_id;
  #else
  bcm_multicast_t group;
  #endif

  rv = bcm_stk_my_modid_get(unit, &myModid);
  if (rv != BCM_E_NONE)
  {
    return rv; 
  }

  for (i = 0; i < (sizeof(ipmc_cmd->ipmc_index_mask) / sizeof(ipmc_cmd->ipmc_index_mask[0])); i++)
  {
    /* Check a word at a time for better efficiency. */
    if (ipmc_cmd->ipmc_index_mask[i] != 0)
    {
      /* Check IPMC groups w/in this word. */
      for (index = (i * 32); index < ((i + 1) * 32); index++)
      {
        if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(index, ipmc_cmd->ipmc_index_mask))
        {
          ipmc_group_index[num_ipmc_groups] = index;
          num_ipmc_groups++;

          /* Mark that we've processed this IPMC group. */
          USL_BCMX_PORT_IPMC_INDEX_MEMBER_CLEAR(index, ipmc_cmd->ipmc_index_mask);
          /* Check to see if we're done w/ IPMC groups in this word. */
          if (ipmc_cmd->ipmc_index_mask[i] == 0)
          {
            break;
          }
        }
      }
    }
  }

  ipmc_untag_flag = !ipmc_cmd->tagged;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < num_ipmc_groups; i++)
    {
      index = ipmc_group_index[i];

      memset(&ipmc, 0, sizeof(ipmc));

      /* TODO: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      if ( ipmc_cmd->ipmc_index <= 0)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't get IPMC entry for index %d, rv %d", index, rv);
        rv = BCM_E_BADID;
        continue;
      }

      /* Get gport */
      rv = bcm_port_gport_get(unit, port, &gport);
      if (rv != BCM_E_NONE)
        continue;

      /* Get encap id */
      rv = bcm_multicast_l3_encap_get(unit, ipmc_cmd->ipmc_index, gport, ipmc_cmd->vlan_id, &encap_id);
      if (rv != BCM_E_NONE)
        continue;

      /* Add egress port */
      rv = bcm_multicast_egress_add(unit, ipmc_cmd->ipmc_index, gport, encap_id);
      if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't add IPMC entry for index %d, rv %d", index, rv);
        continue;
      }

      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Added egress port %d (gport=0x%08x)",port,gport);

      #else
      rv = bcm_ipmc_get_by_index(unit, index, &ipmc);
      if (rv != BCM_E_NONE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't get IPMC entry for index %d, rv %d", index, rv);
        continue;
      }

      if (!ipmc.v) 
      {  
        continue;
      }

      _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_L3, ipmc_cmd->ipmc_index);
      rv = bcm_multicast_egress_add(unit, group, port, ipmc_cmd->encap_id); 
      if (rv != BCM_E_NONE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't add IPMC entry for index %d, rv %d", index, rv);
        continue;
      }
      #endif
    }
  }

  if ((USL_BCM_CONFIGURE_DB(USL_IPMC_ROUTE_DB_ID) == L7_TRUE) &&
      (L7_BCMX_OK(rv) == L7_TRUE))
  {
    /* Update the USL Db */
    rv = usl_db_ipmc_egress_port_set(myModid, port, ipmc_cmd->vlan_id, ipmc_untag_flag);
    if (rv != BCM_E_NONE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "usl_db_ipmc_egress_port_set() returned %d", rv);
    }

    rv = usl_db_ipmc_update_l3_port_groups(myModid, port, 
                                           USL_CMD_ADD, ipmc_group_index, 
                                           num_ipmc_groups, ipmc_cmd->vlan_id);
  }

  return rv;
}

/*********************************************************************
*
* @purpose Delete L3 ports from the multicast group.
*
* @param   port - 
*
* @returns BCM Error Code
*
* @notes This function deletes L3 ports from the specified groups
*
* @end
*
*********************************************************************/
int usl_bcm_ipmc_egress_port_delete (int unit, bcm_port_t port, 
                                     usl_bcmx_port_ipmc_cmd_t *ipmc_cmd)
{
  int                       rv = BCM_E_NONE;
  bcm_ipmc_addr_t           ipmc;
  int                       index;
  L7_uint32                 i;
  int                       ipmc_untag_flag;
  L7_uint32                 myModid;
  L7_uint32                 ipmc_group_index[L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL];
  L7_uint32                 num_ipmc_groups = 0;
  /* PTin added: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcm_gport_t gport;
  bcm_if_t encap_id;
  #else
  bcm_multicast_t group;
  #endif

  rv = bcm_stk_my_modid_get(unit, &myModid);
  if (rv != BCM_E_NONE)
  {
    return rv; 
  }

  for (i = 0; i < (sizeof(ipmc_cmd->ipmc_index_mask) / sizeof(ipmc_cmd->ipmc_index_mask[0])); i++)
  {
    /* Check a word at a time for better efficiency. */
    if (ipmc_cmd->ipmc_index_mask[i] != 0)
    {
      /* Check IPMC groups w/in this word. */
      for (index = (i * 32); index < ((i + 1) * 32); index++)
      {
        if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(index, ipmc_cmd->ipmc_index_mask))
        {
          ipmc_group_index[num_ipmc_groups] = index;
          num_ipmc_groups++;

          /* Mark that we've processed this IPMC group. */
          USL_BCMX_PORT_IPMC_INDEX_MEMBER_CLEAR(index, ipmc_cmd->ipmc_index_mask);
          /* Check to see if we're done w/ IPMC groups in this word. */
          if (ipmc_cmd->ipmc_index_mask[i] == 0)
          {
            break;
          }
        }
      }
    }
  }

  ipmc_untag_flag = !ipmc_cmd->tagged;

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < num_ipmc_groups; i++)
    {
      index = ipmc_group_index[i];

      memset(&ipmc, 0, sizeof(ipmc));

      /* TODO: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      if ( ipmc_cmd->ipmc_index <= 0)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't get IPMC entry for index %d, rv %d", index, rv);
        rv = BCM_E_BADID;
        continue;
      }

      /* Get gport */
      rv = bcm_port_gport_get(unit, port, &gport);
      if (rv != BCM_E_NONE)
        continue;

      /* Get encap id */
      rv = bcm_multicast_l3_encap_get(unit, ipmc_cmd->ipmc_index, gport, ipmc_cmd->vlan_id, &encap_id);
      if (rv != BCM_E_NONE)
        continue;

      /* Add egress port */
      rv = bcm_multicast_egress_delete(unit, ipmc_cmd->ipmc_index, gport, encap_id);
      if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't delete IPMC entry for index %d, rv %d", index, rv);
        continue;
      }

      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Removed egress port %d (gport=0x%08x)",port,gport);
      #else
      rv = bcm_ipmc_get_by_index(unit, index, &ipmc);
      if (rv != BCM_E_NONE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't get IPMC entry for index %d, rv %d", index, rv);
        continue;
      }

      if (!ipmc.v) 
      {  
        continue;
      }

      _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_L3, ipmc_cmd->ipmc_index);
      rv = bcm_multicast_egress_delete(unit, group, port, ipmc_cmd->encap_id); 
      if (rv != BCM_E_NONE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't add IPMC entry for index %d, rv %d", index, rv);
        continue;
      }
      #endif
    }
  }

  if ((USL_BCM_CONFIGURE_DB(USL_IPMC_ROUTE_DB_ID) == L7_TRUE) &&
      (L7_BCMX_OK(rv) == L7_TRUE))
  {
    /* Update the USL Db */
    rv = usl_db_ipmc_egress_port_set(myModid, port, ipmc_cmd->vlan_id, ipmc_untag_flag);
    if (rv != BCM_E_NONE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "usl_db_ipmc_egress_port_set() returned %d", rv);
    }

    rv = usl_db_ipmc_update_l3_port_groups(myModid, port, 
                                           USL_CMD_ADD, ipmc_group_index, 
                                           num_ipmc_groups, ipmc_cmd->vlan_id);
  }

  return rv;
}

/*********************************************************************
*
* @purpose Add L3 ports to the multicast group.
*
* @param   port - 
*
* @returns BCM Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int usl_bcm_ipmc_egress_port_set(L7_uint32 modid, bcm_port_t port, L7_ushort16 vlanId, L7_ushort16 untagFlag)
{
  int                       rv = BCM_E_NONE;
  L7_uint32                 bcm_unit, myModid;
  bcm_mac_t                 mac = {0};

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
  {
    for (bcm_unit=0; bcm_unit < soc_ndev; bcm_unit++)
    {
      rv = bcm_stk_my_modid_get(bcm_unit, &myModid);
      if (rv != BCM_E_NONE)
      {
        break;
      }
      if (myModid == modid)
      {
        (void)bcm_ipmc_egress_port_set(bcm_unit, port, 
                                       mac,
                                       untagFlag, 
                                       vlanId,
                                       0);
      }
    }
  }

  if (USL_BCM_CONFIGURE_DB(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
  {
    rv = usl_db_ipmc_egress_port_set(modid, port, vlanId, untagFlag);
  }

  return rv;
}

/*********************************************************************
*
* @purpose Delete L3 ports from the multicast group.
*
* @param   
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int usl_bcm_ipmc_delete_l3_port_groups (int unit, bcm_port_t port, 
                                        usl_bcmx_port_ipmc_cmd_t  *ipmc_cmd)
{
  int                            rv = BCM_E_NONE;
  bcm_ipmc_addr_t                ipmc;
  int                            index;
  L7_uint32                      i;
  bcm_vlan_vector_t              vlan_vector;
  L7_uint32                      myModid;
  L7_uint32                      ipmc_group_index[L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL];
  L7_uint32                      num_ipmc_groups = 0;
  /* PTin added: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcm_gport_t gport;
  bcm_if_t encap_id;
  #endif

  rv = bcm_stk_my_modid_get(unit, &myModid);
  if (rv != BCM_E_NONE)
  {
    return rv; 
  }

  for (i = 0; i < (sizeof(ipmc_cmd->ipmc_index_mask) / sizeof(ipmc_cmd->ipmc_index_mask[0])); i++)
  {
    /* Check a word at a time for better efficiency. */
    if (ipmc_cmd->ipmc_index_mask[i] != 0)
    {
      /* Check IPMC groups w/in this word. */
      for (index = (i * 32); index < ((i + 1) * 32); index++)
      {
        if (USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(index, ipmc_cmd->ipmc_index_mask))
        {
          ipmc_group_index[num_ipmc_groups] = index;
          num_ipmc_groups++;

          /* Mark that we've processed this IPMC group. */
          USL_BCMX_PORT_IPMC_INDEX_MEMBER_CLEAR(index, ipmc_cmd->ipmc_index_mask);
          /* Check to see if we're done w/ IPMC groups in this word. */
          if (ipmc_cmd->ipmc_index_mask[i] == 0)
          {
            break;
          }
        }
      }
    }
  }

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < num_ipmc_groups; i++)
    {
      index = ipmc_group_index[i];

      memset(&ipmc, 0, sizeof(ipmc));

      /* TODO: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      /* Validate group id */
      if (ipmc_cmd->ipmc_index <= 0)
      {
        rv = BCM_E_BADID;
        continue;
      }
      #else
      rv = bcm_ipmc_get_by_index(unit, index, &ipmc);
      if (rv != BCM_E_NONE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't get IPMC entry for index %d, rv %d", index, rv);
        continue;
      }

      if (!ipmc.v) 
      {
        continue;
      }
      #endif

      /* Set up VLAN replication.
      */
      /* TODO: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      rv = bcm_multicast_repl_get(unit, index, port, vlan_vector);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't get replication info for IPMC entry index %d, rv %d", index, rv);
        continue;
      }

      BCM_VLAN_VEC_CLR(vlan_vector, ipmc_cmd->vlan_id);
      rv = bcm_multicast_repl_set(unit, index, port, vlan_vector);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set replication info for IPMC entry index %d, rv %d", index, rv);
        continue;
      }
      #else
      rv = bcm_ipmc_repl_get(unit, index, port, vlan_vector);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't get replication info for IPMC entry index %d, rv %d", index, rv);
        continue;
      }

      BCM_VLAN_VEC_CLR(vlan_vector, ipmc_cmd->vlan_id);
      rv = bcm_ipmc_repl_set(unit, index, port, vlan_vector);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set replication info for IPMC entry index %d, rv %d", index, rv);
        continue;
      }
      #endif

      /* If the egress port doesn't have any more replicated VLANs then remove the
      ** port from the L3 group.
      */
      if (usl_db_ipmc_vlan_vector_is_empty(vlan_vector))
      {
        /* TODO: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        /* Get gport */
        rv = bcm_port_gport_get(unit, port, &gport);
        if (rv != BCM_E_NONE)
          continue;

        /* Get encap id */
        rv = bcm_multicast_l3_encap_get(unit, ipmc_cmd->ipmc_index, gport, ipmc_cmd->vlan_id, &encap_id);
        if (rv != BCM_E_NONE)
          continue;

        /* Add egress port */
        rv = bcm_multicast_egress_delete(unit, ipmc_cmd->ipmc_index, gport, encap_id);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
          continue;

        #else
        BCM_PBMP_PORT_REMOVE(ipmc.l3_pbmp, port);
        ipmc.flags |= BCM_IPMC_REPLACE;
        if (usl_db_ipmc_rpf_check_mode_get() == L7_FALSE)
        {
          ipmc.flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
        }
        rv = bcm_ipmc_add(unit, &ipmc);
        #endif
      }
    }
  }

  if (USL_BCM_CONFIGURE_DB(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
  {
    /* Update the USL Db even if HW operation fails. */
    rv = usl_db_ipmc_update_l3_port_groups(myModid, port, 
                                           USL_CMD_REMOVE, 
                                           ipmc_group_index, num_ipmc_groups, 
                                           ipmc_cmd->vlan_id);
  }

  return rv;
}


/*********************************************************************
*
* @purpose Finds the Hit status of an IPMC entry in the hardware
*
* @param   bcm_ipmc {(input)} Source-IP/Group-IP/Vid/Index 
* @param   flags    {(output)} Hit Flag                            
*                     
*
* @returns BCM Error Code
*
* @notes Used in IPMC Route aging on Standalone platforms
*
* @end
*
*********************************************************************/
int usl_bcm_ipmc_inuse_get(usl_bcm_ipmc_addr_t *usl_ipmc, L7_uint32 *flags)
{
  bcm_ipmc_addr_t bcm_ipmc;
  int             rv = BCM_E_NONE, bcm_unit;

  *flags = 0;

  for (bcm_unit=0;bcm_unit < soc_ndev;bcm_unit++)
  {
    if (!BCM_IS_FABRIC(bcm_unit))
    {
      rv = l7_ipmc_to_bcm(usl_ipmc, bcm_unit, &bcm_ipmc);
      if (rv != BCM_E_NONE)
      {
        break;    
      }

      /* TODO: SDK 6.3.0 */
      #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
      rv = bcm_ipmc_find(bcm_unit, &bcm_ipmc);
      if (rv != BCM_E_NONE)
      {
        continue;
      }
      #else
      rv = bcm_ipmc_get_by_index(bcm_unit, bcm_ipmc.ipmc_index, &bcm_ipmc);
      if (rv != BCM_E_NONE)
      {
        continue;
      }
      #endif

      if (!bcm_ipmc.v) 
      {
        continue;
      }

      if (bcm_ipmc.flags & BCM_IPMC_HIT)
      {
        *flags = BCM_IPMC_HIT;
        break;    
      }
    }
  }

  return rv;
}

/*********************************************************************
*
* @purpose Sets the VLAN replication configuration for a port in an
*          IPMC entry. If there are one or more VLANs replicated, 
*          the port is added to the L3 pbmp of the IPMC entry.
*          If there are zero VLANs replicated, the port is 
*          removed from the IPMC entry.
*
* @param   modid      {(input)} modid of L3 port
* @param   bcmPort    {(input)} BCM port
* @param   ipmcEntry  {(input)} IPMC entry
* @param   vlanVector {(input)} VLAN replication info for L3 port.
*                     
*
* @returns BCM Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_bcm_ipmc_l3_port_repl_set(L7_uint32 modid, L7_uint32 bcmPort, usl_bcm_ipmc_addr_t *ipmcEntry, bcm_vlan_vector_t vlanVector)
{
  L7_int32            bcm_unit, myModid;
  int                 rv = BCM_E_NONE;
  bcm_ipmc_addr_t     bcm_ipmc;
  L7_BOOL             vlanVectorIsEmpty;
  L7_BOOL             ipmcEntryChanged = L7_FALSE;
  /* PTin added: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcm_gport_t gport;
  bcm_if_t encap_id;
  #endif

  memset (&bcm_ipmc, 0, sizeof (bcm_ipmc));

  /* Check if the hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_IPMC_ROUTE_DB_ID) == L7_TRUE)
  {
    for (bcm_unit=0; bcm_unit < soc_ndev; bcm_unit++)
    {
      rv = bcm_stk_my_modid_get(bcm_unit, &myModid);
      if (rv != BCM_E_NONE)
      {
        break;
      }
      if (myModid == modid)
      {
        /* TODO: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        rv = l7_ipmc_to_bcm(ipmcEntry, bcm_unit, &bcm_ipmc);
        #else
        rv = bcm_ipmc_get_by_index(bcm_unit, ipmcEntry->ipmc_index, &bcm_ipmc);
        #endif
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }

        if (!bcm_ipmc.v) 
        {
          rv = BCM_E_FAIL;
          break;
        }

        vlanVectorIsEmpty = usl_db_ipmc_vlan_vector_is_empty(vlanVector);
        ipmcEntryChanged = L7_FALSE;

        /* TODO: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        /* Get gport */
        rv = bcm_port_gport_get(bcm_unit, bcmPort, &gport);
        if (rv != BCM_E_NONE)
          break;
        /* Get encap id */
        rv = bcm_multicast_l3_encap_get(bcm_unit, ipmcEntry->ipmc_index, gport, ipmcEntry->vid, &encap_id);
        if (rv != BCM_E_NONE)
          break;

        if ((vlanVectorIsEmpty == L7_TRUE) && (BCM_PBMP_MEMBER(ipmcEntry->l3_pbmp[modid], bcmPort)))
        {
          /* Remove egress port */
          rv = bcm_multicast_egress_delete(bcm_unit, ipmcEntry->ipmc_index, gport, encap_id);
          if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
            break;
        }
        else if ((vlanVectorIsEmpty == L7_FALSE) && (!BCM_PBMP_MEMBER(ipmcEntry->l3_pbmp[modid], bcmPort)))
        {
          /* Add egress port */
          rv = bcm_multicast_egress_add(bcm_unit, ipmcEntry->ipmc_index, gport, encap_id);
          if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
            break;
        }
        #else
        if ((vlanVectorIsEmpty == L7_TRUE) && (BCM_PBMP_MEMBER(bcm_ipmc.l3_pbmp, bcmPort)))
        {
          /* Remove the port. */
          BCM_PBMP_PORT_REMOVE(bcm_ipmc.l3_pbmp, bcmPort);
          ipmcEntryChanged = L7_TRUE;
        }
        else if ((vlanVectorIsEmpty == L7_FALSE) && (!BCM_PBMP_MEMBER(bcm_ipmc.l3_pbmp, bcmPort)))
        {
          /* Add the port. */
          BCM_PBMP_PORT_ADD(bcm_ipmc.l3_pbmp, bcmPort);
          ipmcEntryChanged = L7_TRUE;
        }

        if (ipmcEntryChanged == L7_TRUE)
        {
          bcm_ipmc.flags |= BCM_IPMC_REPLACE;
          if (usl_db_ipmc_rpf_check_mode_get() == L7_FALSE)
          {
            bcm_ipmc.flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
          }
          rv = bcm_ipmc_add(bcm_unit, &bcm_ipmc);
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            break;
          }
        }
        #endif

        /* TODO: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        rv = bcm_multicast_repl_set(bcm_unit, ipmcEntry->ipmc_index, bcmPort, vlanVector);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }
        #else
        rv = bcm_ipmc_repl_set(bcm_unit, ipmcEntry->ipmc_index, bcmPort, vlanVector);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }
        #endif
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_IPMC_ROUTE_DB_ID) == L7_TRUE))
  {
    rv = usl_db_ipmc_l3_port_repl_set(modid, bcmPort, ipmcEntry, vlanVector);
  }

  return rv;
}
#endif /* L7_MCAST_PACKAGE */
