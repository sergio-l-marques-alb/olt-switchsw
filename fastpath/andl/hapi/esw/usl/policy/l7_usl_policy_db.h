/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
*
* @filename  l7_usl_policy_db.h
*
* @purpose   This file implements the Policy Manager USL DB routines.
*
* @component hapi
*
* @create    12/12/08
*
* @author    colinw
*
* @end
*
**********************************************************************/
#ifndef BROAD_L7_USL_POLICY_DB_H
#define BROAD_L7_USL_POLICY_DB_H

#include "l7_usl_bcm.h"
#include "broad_policy_types.h"

typedef unsigned char   BROAD_PORT_MODE_t;
#define PORT_MODE_DEFAULT 0   /* default mode    */
#define PORT_MODE_ALL     1   /* applies to all  */
#define PORT_MODE_NONE    2   /* applies to none */
#define PORT_MODE_LAST    3

/* 
 * Type used to store the Policy info. 
 */
typedef struct
{
  BROAD_POLICY_t                policyId;
  L7_uchar8                     used;
  BROAD_PORT_MODE_t             policyMode;
  BROAD_POLICY_ENTRY_t          policyInfo;
  L7_uint32                     hashVal;
} BROAD_USL_POLICY_t;

/*********************************************************************
* @purpose  Initialize the policy tables, semaphores, ...
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*
* @notes    On error, all resources will be released
*       
* @end
*********************************************************************/
L7_RC_t usl_policy_init();

/*********************************************************************
* @purpose  Release all resources allocated during usl_policy_init()
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were released
* @returns  L7_ERROR   - if any resourses were not released 
*
* @notes    Should not call this unless the an error occured during init or the
*           configurator is taking the us back to pre Phase 1   
*
* @notes    
* @end
*********************************************************************/
L7_RC_t usl_policy_fini();

/*********************************************************************
* @purpose  Clear all entries from the Policy databases
*
* @param    void
*
* @returns  L7_SUCCESS or L7_ERROR 
*
* @notes    Use this function to reset all the tables to the default, no 
*           resources will be deallocated
*
* @end
*********************************************************************/
L7_RC_t usl_policy_database_invalidate(USL_DB_TYPE_t flag);

/*********************************************************************
* @purpose  Acquire the USL Policy Bcmx lock
*
* @params   none
*
* @returns  none
*
* @end
*********************************************************************/
void usl_policy_bcmx_lock_take(void);

/*********************************************************************
* @purpose  Give the USL Policy Bcmx lock
*
* @params   none
*
* @returns  none
*
* @end
*********************************************************************/
void usl_policy_bcmx_lock_give(void);

/*********************************************************************
* @purpose  Get the policy db handle based on db type
*
* @param    dbType           {(input)}   Type of db to get
*           *dbHandle        {{output}}  DB handle
*
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void usl_policy_db_handle_get(USL_DB_TYPE_t dbType, BROAD_USL_POLICY_t **dbHandle);

/*********************************************************************
* @purpose  Get policy info from the USL policy DB
*
* @param     dbType      @{(input)}  Type of db
* @param     policy      @{(input)}  the policy ID that is created
* @param    *policyInfo  @{(output)} the policy info
*
* @notes    It is up to the caller to free all nodes allocated for the
*           rule info. hapiBroadPolicyRulesPurge() can be used for this.
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_info_get(USL_DB_TYPE_t dbType, BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyInfo);

/*********************************************************************
* @purpose  Create a Policy in the DB using the specified policyId
*
* @param     dbType      @{(input)}  Type of db
* @param     policy      @{(input)} the policy ID that is created
* @param    *policyInfo  @{(input)} the policy info
* @param     hashVal      @{(input)} crc32 value from policyInfo
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_create_with_id(USL_DB_TYPE_t dbType, BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyInfo, L7_uint32 hashVal);

/*********************************************************************
* @purpose  Create a Policy in the DB
*
* @param     dbType      @{(input)}  Type of db
* @param    *policy      @{(output)} the policy ID that is created
* @param    *policyInfo  @{(input)} the policy info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_create(USL_DB_TYPE_t dbType, BROAD_POLICY_t *policy, BROAD_POLICY_ENTRY_t *policyInfo);

/*********************************************************************
* @purpose  Calculate the hash signature of a policy.
*
* @param    *policyInfo  @{(input)} the policy info
*
* @returns  32 bit CRC
*
* @end
*********************************************************************/
L7_uint32 usl_db_policy_hash_calc(BROAD_POLICY_ENTRY_t *policyInfo);

/*********************************************************************
* @purpose  Destroy a Policy in the DB
*
* @param    dbType      @{(input)} Type of db
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_destroy(USL_DB_TYPE_t dbType, BROAD_POLICY_t policy);

/*********************************************************************
* @purpose  Apply a policy to all ports
*
* @param    dbType      @{(input)} Type of db
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_apply_all(USL_DB_TYPE_t dbType, BROAD_POLICY_t policy);

/*********************************************************************
* @purpose  Remove a policy from all ports
*
* @param    dbType      @{(input)} Type of db
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_remove_all(USL_DB_TYPE_t dbType, BROAD_POLICY_t policy);

/*********************************************************************
* @purpose  Apply a policy to a port
*
* @param    dbType      @{(input)} Type of db
* @param    policy      @{(input)} policy ID
* @param    port        @{(input)} port
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_port_apply(USL_DB_TYPE_t  dbType, 
                             BROAD_POLICY_t policyId,
                             bcmx_lport_t   port);

/*********************************************************************
* @purpose  Remove a policy from a port
*
* @param    dbType      @{(input)} Type of db
* @param    policy      @{(input)} policy ID
* @param    port        @{(input)} port
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_port_remove(USL_DB_TYPE_t  dbType, 
                              BROAD_POLICY_t policyId,
                              bcmx_lport_t   port);

#ifdef L7_STACKING_PACKAGE
/*********************************************************************
* @purpose  Used to update all policies that have a 'trap to CPU' action
*           with the modid and modport of the new manager's CPU.
*           This function is called by each stack unit after
*           a failover occurs.
*
* @params   cpu_modid   {(input)} modid of new CPU port.
*           cpu_modport {(input)} modport of new CPU. 
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
int usl_policy_db_dataplane_cleanup(L7_uint32 cpu_modid, L7_uint32 cpu_modport);
#endif

L7_RC_t usl_bcmx_policy_init();

void usl_bcmx_policy_invalidate();

void usl_policy_debug(L7_uint32 setting);
/*********************************************************************
* @purpose  Apply a policy to all ports
*
* @param    dbType      @{(input)} Type of db
* @param    policy      @{(input)} policy ID
* @param    rule        @{(input)} rule ID
* @param    status      @{(input)} status information of the rule
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_rule_status_set(USL_DB_TYPE_t dbType, 
                                  BROAD_POLICY_t policy, 
                                  BROAD_POLICY_RULE_t rule, 
                                  BROAD_RULE_STATUS_t status);
#endif /* BROAD_L7_USL_POLICY_DB_H */
