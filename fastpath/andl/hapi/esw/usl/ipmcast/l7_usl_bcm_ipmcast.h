
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_ipmcast.h
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

#ifndef L7_USL_BCM_IPMCAST_H
#define L7_USL_BCM_IPMCAST_H

#include "l7_common.h"

#ifdef L7_MCAST_PACKAGE

#include "l7_usl_bcm.h"
#include "broad_common.h"

typedef struct 
{
  bcm_ip6_t   s_ip_addr;              /* This will hold both ipv4 and ipv6 addresses*/
  bcm_ip6_t   mc_ip_addr;             /* This will hold both ipv4 and ipv6 addresses*/
  bcm_vlan_t  vid;
  bcm_vrf_t vrf;                      /* Virtual Router Instance. */
  bcm_cos_t cos;                      /* COS based on dst IP multicast addr. */
  bcm_pbmp_t l2_pbmp[L7_MOD_MAX];     /* L2 Port bitmap. */
  bcm_pbmp_t l2_ubmp[L7_MOD_MAX];     /* L2 Untag port bitmap. */
  bcm_pbmp_t l3_pbmp[L7_MOD_MAX];     /* L3 Port bitmap. */
  int ts;                             /* Source port or TGID bit. */
  int port_tgid;                      /* Source port or TGID. */
  int v;                              /* Valid bit. */
  int mod_id;                         /* Module ID. */
  int ipmc_index;                     /* Use this index to program IPMC table
                                         for XGS chips based on flags value.
                                           For SBX chips it is the Multicast
                                           Group index */
  uint32 flags;                       /* See BCM_IPMC_XXX flag definitions. */
  int lookup_class;                   /* Classification lookup class ID. */
  bcm_fabric_distribution_t distribution_class; /* Fabric Distribution Class. */
} usl_bcm_ipmc_addr_t;


#define USL_IPMC_INDEX_MASK_WORD_SIZE 32

/* This macro sets a bit corresponding to the IPMC index in the membership
** mask. 
*/
#define USL_BCMX_PORT_IPMC_INDEX_MEMBER_SET(ipmc_index,mask) \
            (mask[(ipmc_index) / USL_IPMC_INDEX_MASK_WORD_SIZE] |=  \
              (1 << ((ipmc_index) % USL_IPMC_INDEX_MASK_WORD_SIZE) ))

/* This macro clears a bit corresponding to the IPMC index in the membership
** mask. 
*/
#define USL_BCMX_PORT_IPMC_INDEX_MEMBER_CLEAR(ipmc_index,mask) \
            (mask[(ipmc_index) / USL_IPMC_INDEX_MASK_WORD_SIZE] &=  \
              (~(1 << ((ipmc_index) % USL_IPMC_INDEX_MASK_WORD_SIZE) )))

/* This macro returns a non-zero value if the IPMC index is a member of the specified mask.
*/
#define USL_BCMX_PORT_IS_IPMC_INDEX_MEMBER(ipmc_index,mask) \
            (mask[(ipmc_index) / USL_IPMC_INDEX_MASK_WORD_SIZE] & \
              (1 << ((ipmc_index) % USL_IPMC_INDEX_MASK_WORD_SIZE) ))



typedef enum
{
  USL_BCMX_IPMC_L2_PORT_ADD = 100,   /* Add L2 ports to a multicast group */
  USL_BCMX_IPMC_L2_PORT_DELETE,      /* Delete L2 ports from a multicast group */
  USL_BCMX_IPMC_L3_PORT_ADD,         /* Add L3 ports to a multicast group */
  USL_BCMX_IPMC_L3_PORT_DELETE,      /* Delete L3 ports from a multicast group */
  USL_BCMX_IPMC_EGRESS_PORT_ADD,     /* Add L3 ports to a multicast group */
  USL_BCMX_IPMC_EGRESS_PORT_DELETE  /* Delete L3 ports from a multicast group */
} USL_BCMX_IPMC_CMD_t;

typedef struct
{
  L7_uint32 ipmc_index_mask [(L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL / USL_IPMC_INDEX_MASK_WORD_SIZE) + 1];
  L7_uchar8 ipmc_index;
  L7_uint32 vlan_id;
  L7_BOOL   tagged;
  L7_uchar8 mac[6];
  L7_uint32 ttl;
  bcm_pbmp_t pbmp; /* L2 port bitmap */
  bcm_pbmp_t ubmp; /* L2 port bitmap */
  L7_uint32 encap_id;
} usl_bcmx_port_ipmc_cmd_t;

#define USL_BCM_IPMC_INVALID_INDEX (-1)

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
int usl_bcm_ipmc_add (usl_bcm_ipmc_addr_t  *ipmc, L7_BOOL replace_entry);

/*********************************************************************
* @purpose  Custom RPC function to handle multicast route deletes.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
int usl_bcm_ipmc_delete (usl_bcm_ipmc_addr_t *ipmc, L7_uint32 keep);

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
int usl_bcm_ipmc_rpf_set (usl_bcm_ipmc_addr_t  *ipmc);

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
L7_RC_t usl_bcm_ipmc_set_l2_ports (usl_bcm_ipmc_addr_t *ipmc_addr);

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
                                     usl_bcmx_port_ipmc_cmd_t *ipmc_cmd);

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
                                        usl_bcmx_port_ipmc_cmd_t *ipmc_cmd);

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
                                     usl_bcmx_port_ipmc_cmd_t *ipmc_cmd);

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
                                  usl_bcmx_port_ipmc_cmd_t *ipmc_cmd);

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
                                     usl_bcmx_port_ipmc_cmd_t *ipmc_cmd);

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
int usl_bcm_ipmc_egress_port_set(L7_uint32 modid, bcm_port_t port, L7_ushort16 vlanId, L7_ushort16 untagFlag);

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
                                        usl_bcmx_port_ipmc_cmd_t  *ipmc_cmd);

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
int usl_bcm_ipmc_inuse_get(usl_bcm_ipmc_addr_t *usl_ipmc, L7_uint32 *flags);

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
int usl_bcm_ipmc_l3_port_repl_set(L7_uint32 modid, L7_uint32 bcmPort, usl_bcm_ipmc_addr_t *ipmcEntry, bcm_vlan_vector_t vlanVector);

#endif /* L7_MCAST_PACKAGE */

#endif
