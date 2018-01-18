/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_l2_db.h
*
* @purpose    USL L2 DB API header file
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

#ifndef L7_USL_L2_DB_H
#define L7_USL_L2_DB_H

#include "l7_common.h"
#include "l7_usl_mask.h"
#include "l7_usl_vlan_db.h"
#include "l7_usl_stg_db.h"
#include "l7_usl_trunk_db.h"
#include "l7_usl_l2mc_db.h"
#include "l7_usl_bcm.h"
#include "l7_usl_bcm_l2.h"
#include "avl_api.h"


/* Type used to store IpSubnet Vlan Entry*/
typedef struct
{
  usl_bcm_vlan_ipsubnet_t  subnetData;
  L7_BOOL                  used;
} usl_vlan_ipsubnet_db_elem_t;

/* Type used to store Mac Vlan Entry*/
typedef struct
{
  usl_bcm_vlan_mac_t macData;
  L7_BOOL            used;
} usl_vlan_mac_db_elem_t;

typedef struct
{
  usl_bcm_protected_group_t groupData;
  L7_BOOL                   valid;
} usl_protected_group_db_elem_t;

/* Element of the USL System Db. This Db stores
** general system configurations.
*/
typedef struct
{
  /* L2 address age time */
  L7_uint32 l2AgingTime;

  /* Flow-control configuration */
  L7_uint32         flowControlMode;
  L7_enetMacAddr_t  systemMac; 

  /* DOS control configuration */
  L7_uchar8         dosConfigMode[DOSCONTROL_MAX];
  L7_uint32         dosConfigArg[DOSCONTROL_MAX];

  /* Dvlan config */
  L7_uint32   dvlanMode;
  L7_ushort16 dvlanEther;

  /* Global IPMC mode */
  L7_BOOL           ipmcMode;

  /* global drop mode */
  L7_uint32         mmuDropMode;

  /* Indicates whether this element is valid or not */
  L7_BOOL           isValid;

} usl_system_db_elem_t; 





/*********************************************************************
* @purpose  Adds the entry to the uslipsubnetvlan cfg table
*
* @param   dbType  @{(input)} dbType to update
* @param   ipaddr  @b{(input)} IP Address
* @param   netmask @b{(input)} Net mask 
* @param   vid     @b{(input)} vlan Id
* @param   prio    @b{(input)} priority field
*
* @returns  BCM error code
*
* @end
*
*********************************************************************/
int usl_db_vlan_ip4_add(USL_DB_TYPE_t dbType, bcm_ip_t ipaddr, 
                        bcm_ip_t netmask,bcm_vlan_t vid, int prio);

/*********************************************************************
* @purpose  Deletes the entry from the uslipsubnetvlan cfg table
*
* @param   dbType  @{(input)} dbType to update
* @param   ipaddr  @b{(input)} IP Address
* @param   netmask @b{(input)} Net mask 
*
* @returns  BCM error code
*
* @end
*
*********************************************************************/
int usl_db_vlan_ip4_delete(USL_DB_TYPE_t dbType, bcm_ip_t ipaddr, 
                           bcm_ip_t netmask);

/*********************************************************************
* @purpose  Adds the entry to the USL db 
*
* @param   dbType  @{(input)} db type to update
* @param   mac     @b{(input)} Mac Address
* @param   vid     @b{(input)} vlan Id 
* @param   prio    @b{(input)} priority field 
*
* @returns BCM error code
*
* @end
*
*********************************************************************/
int usl_db_vlan_mac_add(USL_DB_TYPE_t dbType, bcm_mac_t mac, 
                        bcm_vlan_t vid, int prio);


/*********************************************************************
* @purpose  Deletes the entry from the usl table 
*
* @param   dbType  @{(input)} db type to update
* @param   mac     @b{(input)} Mac Address
*
* @returns  BCM error code
*
* @end
*
*********************************************************************/
int usl_db_vlan_mac_delete(USL_DB_TYPE_t dbType, bcm_mac_t mac);

/*********************************************************************
* @purpose  Synchronize the L2 unicast table
*
* @param    none
*
* @returns  
*
* @end
*********************************************************************/
extern L7_RC_t usl_l2_addr_sync();

/*********************************************************************
* @purpose  Remove a port from protected group in the USL database
*
* @param    dbType      @{(input)} dbType to modify
* @param    deleteInfo  @{(input)}  Information about the deleted port
*
*
* @returns  BCM error code
*
* @notes   
*
* @end
*********************************************************************/
int usl_db_protected_group_port_remove(USL_DB_TYPE_t dbType,
                                       usl_bcm_protected_group_delete_t *deleteInfo);

/*********************************************************************
* @purpose  Set a protected group membership in the USL database
*
* @param    dbType      @{(input)} dbType to modify
* @param    protectedGroupInfo  @{(input)}  protected port group info
*
*
* @returns  BCM error code
*
* @notes   
*
* @end
*********************************************************************/
int usl_db_protected_group_set(USL_DB_TYPE_t dbType,
                               usl_bcm_protected_group_t *groupInfo);

/*********************************************************************
* @purpose  Set the dvlan mode data in Db
*
* @params   dbType    @{(input)} db type to update
* @params   mode      @{(input)} Dvlan mode
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_dvlan_mode_set(USL_DB_TYPE_t dbType, L7_uint32 mode);

/*********************************************************************
* @purpose  Set the dvlan translation data in Db
*
* @params   dbType    @{(input)} db type to update
* @params   direction @{(input)} ingress or egress
* @params   enable    @{(input)} enable/disable vlan translation
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_dvlan_translation_set(USL_DB_TYPE_t dbType, L7_BOOL direction, L7_BOOL enable);

/*********************************************************************
* @purpose  Set the dvlan TPID data in Db
*
* @params   dbType    @{(input)} db type to update
* @params   etherType @{(input)} Dvlan ethertype
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_dvlan_tpid_set(USL_DB_TYPE_t dbType, L7_ushort16 ethertype);

/*********************************************************************
* @purpose  Set the L2 age-time in Db
*
* @params   age_time  @{(input)} age time
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l2_age_time_set(USL_DB_TYPE_t dbType, L7_uint32 age_time);

/*********************************************************************
* @purpose  Set the IPMC global mode
* 
* @param    dbType {(input)} dbType to update
* @param    enabled {(input)} IP Mcast global mode
*
* @returns  BCM error code 
*
* @end
*********************************************************************/
int usl_db_ipmc_mode_set(USL_DB_TYPE_t dbType, L7_BOOL enabled);

/*********************************************************************
* @purpose  Set the flow-control mode and mac
*
* @params   dbType {(input)} db type to update
* @params   age_time  @{(input)} age time
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_flow_control_set(USL_DB_TYPE_t dbType, L7_uint32 mode, L7_uchar8 *mac);

/*********************************************************************
* @purpose  Get the flow-control mode and mac
*
* @params   dbType {(input)} db type to update
* @params   age_time  @{(input)} age time
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_flow_control_get(USL_DB_TYPE_t dbType, L7_uint32 *mode, L7_uchar8 *mac);

/*********************************************************************
* @purpose  Set the DOS control mode
*
* @params   dbType  @{(input)}  db type to update
* @param    type    @{{input}}  type of DOS control
* @param    mode    @{{input}}  enable=1,disable=0
* @param    arg     @{{input}}  argument for certain types of DOS control
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_doscontrol_set(USL_DB_TYPE_t dbType, L7_DOSCONTROL_TYPE type, L7_uint32 mode, L7_uint32 arg);

/*********************************************************************
* @purpose  Set the mmu drop mode
*
* @params   dbType {(input)} db type to update
* @params   mode   {(input)} drop mode
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_mmu_dropmode_set(USL_DB_TYPE_t dbType, L7_uint32 mode);

/*********************************************************************
* @purpose  Gets the current DB Type for the L2 macsync table.
*
* @param    
*
* @returns  USL_DB_TYPE_t
*
*       
* @end
*********************************************************************/
USL_DB_TYPE_t usl_l2_db_macsync_dbtype_get();

#endif /* L7_USL_L2_DB_H */
