
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_port_db.h
*
* @purpose    USL port table
*
* @component  USL
*
* @comments   none
*
* @create     11/3/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#ifndef L7_USL_PORT_DB_H
#define L7_USL_PORT_DB_H

#include "l7_common.h"
#include "dapi.h"
#include "broad_common.h"
#include "l7_usl_common.h"
#include "l7_usl_bcm_port.h"

/* Timeout for the port-db update message */
#define USL_PORT_DB_UPDATE_TIMEOUT (1000) /* msec */

/* USL port database stores physical port configurations */
#define USL_PORT_DB_TREE_SIZE              L7_MAX_PORT_COUNT 
#define USL_MAX_TPIDS_PER_PORT             L7_DVLANTAG_MAX_TPIDS

/* Macros to set/get port valid mask */
#define USL_PORTDB_VALID_MASK_INDEX_SET(mask, index)                \
        ((mask)[((index)/(8*sizeof(L7_uchar8)))]                   \
                         |= ( 1 << (((index) % (8*sizeof(L7_char8))))))

#define USL_PORTDB_VALID_MASK_IS_INDEX_SET(mask, index)                \
        ((mask)[((index)/(8*sizeof(L7_uchar8)))]                   \
                         & ( 1 << (((index) % (8*sizeof(L7_char8))))))

/* Macros to set/get port flag mask */
#define USL_PORTDB_FLAG_INDEX_SET(flag, index)                \
        ((flag)[((index)/(8*sizeof(L7_uchar8)))]                   \
                         |= ( 1 << (((index) % (8*sizeof(L7_char8))))))

#define USL_PORTDB_FLAG_INDEX_CLR(flag, index)                \
        ((flag)[((index)/(8*sizeof(L7_uchar8)))]                   \
                         &= ~( 1 << (((index) % (8*sizeof(L7_char8))))))

#define USL_PORTDB_FLAG_IS_INDEX_SET(flag, index)                \
        ((flag)[((index)/(8*sizeof(L7_uchar8)))]                   \
                         & ( 1 << (((index) % (8*sizeof(L7_char8))))))


#define USL_PORT_VALID_MASK_SIZE ((USL_BCMX_PORT_SET_CMD_LAST / (8 * sizeof(L7_uchar8))) + 1)


/* Configuration stored as bits in portFlag */
typedef enum
{
  USL_PORT_FLAG_ADMIN_MODE = 1,
  USL_PORT_FLAG_PAUSE_TX,
  USL_PORT_FLAG_PAUSE_RX,
  USL_PORT_FLAG_MAC_LOCK,
  USL_PORT_FLAG_DOT1X_STATE,
  USL_PORT_FLAG_CP_STATE,


  /* Always last */
  USL_PORT_FLAG_LAST

} USL_PORT_FLAGS_t;

#define USL_PORT_FLAGS_SIZE ((USL_PORT_FLAG_LAST / (8 * sizeof(L7_uchar8))) + 1)

/* typedef for key of the USL port database */
typedef struct
{
  bcm_gport_t                              gport; 
} uslPortDbKey_t;

typedef struct
{
  L7_BOOL                               valid;
  bcm_phy_config_t                      mediumConfig;
} usl_port_db_phy_config_t;

typedef struct
{
  L7_BOOL           valid;
  bcm_stg_stp_t     dot1sState;
} usl_port_db_stg_config_t;
 
typedef struct
{
  L7_BOOL                  bcmVlanTranslateIngressEnable;
  L7_BOOL                  bcmVlanTranslateIngressMissDrop;
  L7_BOOL                  bcmVlanTranslateEgressEnable;
  L7_BOOL                  bcmVlanTranslateEgressMissDrop;
  bcm_vlan_translate_key_t bcmVlanPortTranslateKeyFirst;
  bcm_vlan_translate_key_t bcmVlanPortTranslateKeySecond;

} usl_port_db_vlan_control_config_t;
 

/* typepdef for Port database element. Only Add elements that have a 
** fixed size. Do not add pointer elements. Add those in usl_port_db_elem_t */
typedef struct
{
  uslPortDbKey_t                          key;

  usl_bcm_port_rate_limit_t               bcastRateLimit;

  usl_bcm_port_rate_limit_t               mcastRateLimit;

  usl_bcm_port_rate_limit_t               dlfRateLimit;

  usl_bcm_port_filter_mode_t              filterMode;

  usl_bcm_port_priority_t                 priority;

  usl_bcm_port_frame_size_t               maxFrameSize;

  usl_bcm_port_learn_mode_t               learnMode;

  usl_bcm_port_dtag_mode_t                dtagMode; 

  L7_uchar8                               tpidValid[USL_MAX_TPIDS_PER_PORT];
  usl_bcm_port_tpid_t                     tpidValue[USL_MAX_TPIDS_PER_PORT];

  bcm_vlan_t                              pvid;

  bcm_port_discard_t                      discardMode;

  usl_bcm_port_cosq_sched_config_t        cosqSchedConfig;

  usl_bcm_port_shaper_config_t            shaperConfig;

  usl_bcm_port_sflow_config_t             sflowConfig;

  usl_bcm_port_mirror_config_t            mirrorConfig;

  usl_bcm_port_wred_config_t              wredConfig;

  L7_uchar8                               pauseMacAddr[6];

  usl_port_db_phy_config_t                phyConfig[BCM_PORT_MEDIUM_COUNT];

  usl_bcm_port_vlan_t                     vlanConfig;
  
  usl_port_db_stg_config_t                dot1sConfig[HAPI_MAX_MULTIPLE_STP_INSTANCES];

    
  usl_bcm_port_pbvlan_config_t            pbvlanTable[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];

  /* Port Flags */
  L7_uchar8                               portFlags[USL_PORT_FLAGS_SIZE];
  
  usl_port_db_vlan_control_config_t       vlanControlConfig;
  


  /* Bit mask of valid port attributes
  */
  L7_uchar8                               portValidMask[USL_PORT_VALID_MASK_SIZE];

  usl_bcm_port_pfc_config_t                    pfcConfig;

} uslPortDbStaticElem_t;

typedef struct
{
  uslPortDbStaticElem_t                   data;

  /* Policies applied to port. This mask needs to be allocated dynamically based on 
     BROAD_MAX_POLICIES. */
  L7_uchar8                               *portPoliciesMask;
  L7_uchar8                               *portPoliciesChangeMask;

  void *reserved; /* AVL TREE requires this as last */

} usl_port_db_elem_t;


#define USL_PORT_DB_POLICY_MASK_SIZE      ((BROAD_MAX_POLICIES / 8) + 1)

/*********************************************************************
* @purpose  Insert a new port entry in the local USL Port database
*
* @param    dbType      @{(input)} dbType to be inserted
* @param    gport       @{(input)}  Key of the port to be inserted
* @param    numEntries  @{(output)} Final number of entries in Db
*
* @returns  L7_SUCCESS - if the port was inserted
* @returns  L7_ERROR   - if the port was not inserted
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t usl_portdb_insert_port_record(USL_DB_TYPE_t dbType, 
                                      bcm_gport_t gport,
                                      L7_uint32   *numEntries);

/*********************************************************************
* @purpose  Delete a port entry from the local USL Port database
*
* @param    dbType      @{(input)} dbType to be updated
* @param    gport       @{(input)}  Key of the port to be removed
* @param    numEntries  @{(output)} Final number of entries in Db
*
*
* @returns  L7_SUCCESS - if the port was deleted
* @returns  L7_ERROR   - if the port was not deleted
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t usl_portdb_delete_port_record(USL_DB_TYPE_t dbType,
                                      bcm_gport_t   gport, 
                                      L7_uint32    *numEntries);


/*********************************************************************
* @purpose  Add/Reomve the ports in lpList from local and targetFpUnit 
*           USL Port database  
*
* @param    updateCmd     @{(input)}  L7_TRUE: Adds the ports to db
*                                     L7_FALSE: Remove the ports from db
* @param    targetFpUnit  @{(input)}  Unit to send the message
* @param    lpList        @{(input)}  List of ports to be updated in the
*                                     USL port database
*
* @returns  L7_SUCCESS - if the ports were inserted
* @returns  L7_FAILURE - if the ports were not inserted
*
* @notes    This function should be called on Management unit only.
*       
* @end
*********************************************************************/
L7_RC_t usl_portdb_update(L7_BOOL updateCmd, L7_uint32 targetFpUnit, bcmx_lplist_t *lpList);

/*********************************************************************
* @purpose  Process the portdb update message on remote units 
*
* @param    msg  @{(input)}  The msg to be processed
*
* @returns  L7_SUCCESS - if the message was successfully processed
* @returns  L7_FAILURE - if errors were encountered in message processing
*
* @notes    Should be only called on remote units.
*       
* @end
*********************************************************************/
L7_RC_t usl_portdb_update_msg_process(L7_uchar8 *msg);

/*********************************************************************
* @purpose  Return the Gport mapping for a local bcmUnit/bcmPort
*
* @param    localBcmUnit   @{(input)}  Local Bcm Unit number
* @param    bcmPort        @{(input)}  Local Bcm port number
* @param    gport          @{(output)} Gport
*
* @returns  BCM_E_NOT_FOUND: If gport mapping could not be found
*           BCM_E_NONE: Gport mapping found
*
* @notes    Returns BCM_GPORT_INVALID on standalone package as 
*           port db is not active on standalone. 
*       
* @end
*********************************************************************/
int usl_bcm_unit_port_to_gport(int localBcmUnit, bcm_port_t bcmPort, 
                               bcm_gport_t *gport);

/*********************************************************************
* @purpose  Return the Gport mapping for a local bcmUnit/bcmPort
*
* @param    gport          @{(input)}   Gport
* @param    localBcmUnit   @{(output)}  Local Bcm Unit number
* @param    bcmPort        @{(output)}  Local Bcm port number
*
* @returns  BCM_E_NOT_FOUND: If gport mapping could not be found
*           BCM_E_NONE: Gport mapping found
*
* @notes
*       
* @end
*********************************************************************/
int usl_gport_to_bcm_unit_port(bcm_gport_t gport, int *localBcmUnit, bcm_port_t *bcmPort);

/*********************************************************************
* @purpose  Get a port's record in portdb
*
* @param    dbType  @{(input)} dbType to get the record    
* @param    gport   @{(input)}  Port number
*
* @returns  L7_NULL - If port could not be found in port-database
* @returns  Pointer to port's record in portdb
*
* @notes    Port Db lock should be taken before calling this
*           API.
*       
* @end
*********************************************************************/
usl_port_db_elem_t* usl_portdb_port_record_find(USL_DB_TYPE_t dbType, 
                                                bcm_gport_t gport);

/*********************************************************************
* @purpose  Set the broadcast rate threshold parameter in USL database 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    bcast_limit      @{(input)}  Broadcast rate threshold params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_bcast_rate_set(USL_DB_TYPE_t dbType,
                              bcm_gport_t gport, 
                              usl_bcm_port_rate_limit_t *bcast_limit);

/*********************************************************************
* @purpose  Set the multicast rate threshold parameter in USL database 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    mcast_limit      @{(input)}  Multicast rate threshold params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_mcast_rate_set(USL_DB_TYPE_t dbType, 
                              bcm_gport_t gport, 
                              usl_bcm_port_rate_limit_t *mcast_limit);

/*********************************************************************
* @purpose  Set the dlf rate threshold parameter in USL database 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    dlfbc_limit      @{(input)}  Dlf rate threshold params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_dlfbc_rate_set(USL_DB_TYPE_t dbType, 
                              bcm_gport_t gport, 
                              usl_bcm_port_rate_limit_t *dlfbc_limit);


/*********************************************************************
* @purpose  Set the filtering mode for the port in USL database 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    mode              @{(input)}   Filtering mode data
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_filter_mode_set(USL_DB_TYPE_t dbType,
                               bcm_gport_t gport, 
                               usl_bcm_port_filter_mode_t *mode);

/*********************************************************************
* @purpose  Set the priority for the port in USL database 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    priority           @{(input)}  Priority data
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_priority_set(USL_DB_TYPE_t dbType,
                            bcm_gport_t gport, 
                            usl_bcm_port_priority_t *priority);

/*********************************************************************
* @purpose  Set the max frame size for a port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    priority           @{(input)}  Max frame size
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_max_frame_set(USL_DB_TYPE_t dbType,
                             bcm_gport_t gport, 
                             usl_bcm_port_frame_size_t *max_frame_size);

/*********************************************************************
* @purpose  Set the learn mode for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    learnMode          @{(input)}  Learn mode 
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_learn_set(USL_DB_TYPE_t dbType,
                         bcm_gport_t gport, 
                         usl_bcm_port_learn_mode_t *learn_mode);

/*********************************************************************
* @purpose  Set the dtag mode for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    dtagMode           @{(input)}  dtag mode 
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_dtag_mode_set(USL_DB_TYPE_t dbType,
                             bcm_gport_t gport, 
                             usl_bcm_port_dtag_mode_t *dtag_mode);

/*********************************************************************
* @purpose  Set the dtag mode for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    dtagMode           @{(input)}  dtag mode 
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_tpid_set(USL_DB_TYPE_t dbType,
                        bcm_gport_t gport, 
                        usl_bcm_port_tpid_t *tpid);

/*********************************************************************
* @purpose  Add a tpid for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    dtagMode         @{(input)}  dtag mode 
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_tpid_add(USL_DB_TYPE_t        dbType,
                        bcm_gport_t          gport, 
                        usl_bcm_port_tpid_t *tpid);

/*********************************************************************
* @purpose  Delete a tpid from the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    dtagMode         @{(input)}  dtag mode 
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_tpid_delete(USL_DB_TYPE_t        dbType,
                           bcm_gport_t          gport, 
                           usl_bcm_port_tpid_t *tpid);

/*********************************************************************
* @purpose  Set the pvid for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    pvid               @{(input)}  Pvid
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_pvid_set(USL_DB_TYPE_t dbType,
                        bcm_gport_t gport, 
                        bcm_vlan_t *pvid);

/*********************************************************************
* @purpose  Set the discard mode for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    mode               @{(input)}  Discard mode
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_discard_set(USL_DB_TYPE_t dbType,
                           bcm_gport_t gport, 
                           bcm_port_discard_t *mode);

/*********************************************************************
* @purpose  Set the medium config for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    medium             @{(input)}  Phy medium
* @param    config             @{(input)}  Phy config data
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_medium_config_set(USL_DB_TYPE_t dbType,
                                 bcm_gport_t gport, 
                                 bcm_port_medium_t medium,
                                 bcm_phy_config_t  *config);

/*********************************************************************
* @purpose  Set the flow control config for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    pauseConfig        @{(input)}  Flow-control config data
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_flow_control_set(USL_DB_TYPE_t dbType,
                                bcm_gport_t gport, 
                                usl_bcm_port_pause_config_t *pauseConfig);

/*********************************************************************
* @purpose  Set the Cosq sched config for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    cosqSchedConfig    @{(input)}  Cosq Sched config data
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_cosq_sched_set(USL_DB_TYPE_t dbType,
                              bcm_gport_t gport, 
                              usl_bcm_port_cosq_sched_config_t *cosqSchedConfig);

/*********************************************************************
* @purpose  Set the shaper config for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    cosqSchedConfig    @{(input)}  Cosq Sched config data
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_shaper_config_set(USL_DB_TYPE_t dbType,
                                 bcm_gport_t gport, 
                                 usl_bcm_port_shaper_config_t *shaperConfig);


/*********************************************************************
* @purpose  Update the port vlan membership in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    vlanConfig         @{(input)}  Vlan config data
* @param    cmd                @{(input)}  L7_TRUE: Add vlans to the port
*                                          L7_FALSE: Remove vlans from the port
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_vlan_port_update(USL_DB_TYPE_t dbType,
                            bcm_gport_t gport, 
                            USL_CMD_t   cmd,
                            bcm_vlan_t  vid,
                            L7_BOOL     tagged);


/*********************************************************************
* @purpose  Update the port vlan membership in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    vlanConfig         @{(input)}  Vlan config data
* @param    cmd                @{(input)}  L7_TRUE: Add vlans to the port
*                                          L7_FALSE: Remove vlans from the port
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_vlan_membership_update(USL_DB_TYPE_t dbType,
                                      bcm_gport_t gport, 
                                      usl_bcm_port_vlan_t *vlanConfig, 
                                      L7_BOOL cmd);

/*********************************************************************
* @purpose  Set the spanning-tree stg state for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    stg                @{(input)}  stg-id
* @param    stpState           @{(input)}  state
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_stg_stp_update(USL_DB_TYPE_t dbType,
                              bcm_gport_t gport, 
                              bcm_stg_t stg , 
                              bcm_stg_stp_t stpState);

/*********************************************************************
* @purpose  Update the protocol-based vlan configuration in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    pbvlanConfig       @{(input)}  Vlan config data
* @param    cmd                @{(input)}  L7_TRUE: Add pbvlan config to the port
*                                          L7_FALSE: Remove pbvlan config from the port
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_protocol_vlan_update(USL_DB_TYPE_t dbType,
                                    bcm_gport_t gport, 
                                    usl_bcm_port_pbvlan_config_t *pbvlanConfig, 
                                    L7_BOOL cmd);

/*********************************************************************
* @purpose  Set the dot1x state for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    dot1xState         @{(input)}  state
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_dot1x_config(USL_DB_TYPE_t dbType,
                            bcm_gport_t gport, 
                            L7_DOT1X_PORT_STATUS_t state);

/*********************************************************************
* @purpose  Get the dot1x state for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    dot1xState         @{(output)} state
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_dot1x_config_get(USL_DB_TYPE_t dbType,
                                bcm_gport_t gport, 
                                L7_DOT1X_PORT_STATUS_t *state);

/*********************************************************************
* @purpose  Set the mirror config for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    mirrorConfig       @{(input)}  
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_mirror_set(USL_DB_TYPE_t dbType,
                          bcm_gport_t gport, 
                          usl_bcm_port_mirror_config_t *mirrorConfig);

/*********************************************************************
* @purpose  Get the mirror config for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    mirrorConfig       @{(input)}  
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_mirror_get(USL_DB_TYPE_t dbType,
                          bcm_gport_t gport, 
                          usl_bcm_port_mirror_config_t *mirrorConfig);

/*********************************************************************
* @purpose  Add a policy to a port. 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    policy             @{(input)}  Policy Id
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_policy_add(USL_DB_TYPE_t dbType,
                          bcm_gport_t gport, 
                          L7_uint32 policyId);

/*********************************************************************
* @purpose  Remove a policy from a port. 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport              @{(input)}  Port number
* @param    policy             @{(input)}  Policy Id
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_policy_remove(USL_DB_TYPE_t dbType,
                             bcm_gport_t gport, 
                             L7_uint32 policyId);

/*********************************************************************
* @purpose  Set the admin mode for a port 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    enable      @{(input)}  Admin mode
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_enable_set(USL_DB_TYPE_t dbType,
                           bcm_gport_t gport, 
                           L7_int32 enable);

/*********************************************************************
* @purpose  Set the wred parameters for a port 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    wredParams  @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_wred_set(USL_DB_TYPE_t dbType,
                         bcm_gport_t gport, 
                         usl_bcm_port_wred_config_t *wredParams);


/*********************************************************************
* @purpose  Set the sflow config for a port 
*
* @param    dbType      @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    sflowParams @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_sflow_config_set(USL_DB_TYPE_t dbType,
                                 bcm_gport_t gport, 
                                 usl_bcm_port_sflow_config_t *sflowConfig);

/*********************************************************************
* @purpose  Get the sflow config for a port 
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    wredParams  @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_sflow_config_get(USL_DB_TYPE_t dbType,
                                 bcm_gport_t gport, 
                                 usl_bcm_port_sflow_config_t *sflowConfig);

/*********************************************************************
* @purpose  Set the VLAN translation config for all ports
*
* @param    dbType      @{(input)}  dbType to modify
* @param    direction   @{(input)}  TRUE = ingress, FALSE = egress
* @param    enable      @{(input)}  enable mode
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_all_vlan_translate_set(USL_DB_TYPE_t dbType, L7_BOOL direction, L7_BOOL enable);

/*********************************************************************
* @purpose  Set the VLAN translation config for a port
*
* @param    dbType      @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    enable      @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_vlan_translate_ingress_enable_set(USL_DB_TYPE_t dbType,
                                                  bcm_gport_t   gport, 
                                                  L7_BOOL       enable);

/*********************************************************************
* @purpose  Set the VLAN translation config for a port
*
* @param    dbType      @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    enable      @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_vlan_translate_ingress_miss_drop_set(USL_DB_TYPE_t dbType,
                                                     bcm_gport_t   gport, 
                                                     L7_BOOL       drop);

/*********************************************************************
* @purpose  Set the VLAN translation config for a port
*
* @param    dbType      @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    enable      @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_vlan_translate_egress_enable_set(USL_DB_TYPE_t dbType,
                                                 bcm_gport_t   gport, 
                                                 L7_BOOL       enable);

/*********************************************************************
* @purpose  Set the VLAN translation config for a port
*
* @param    dbType      @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    enable      @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_vlan_translate_egress_miss_drop_set(USL_DB_TYPE_t dbType,
                                                    bcm_gport_t   gport, 
                                                    L7_BOOL       drop);

/*********************************************************************
* @purpose  Set the VLAN translation config for a port
*
* @param    dbType      @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    enable      @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_vlan_translate_key_first_set(USL_DB_TYPE_t            dbType,
                                             bcm_gport_t              gport, 
                                             bcm_vlan_translate_key_t key);

/*********************************************************************
* @purpose  Set the VLAN translation config for a port
*
* @param    dbType      @{(input)}  dbType to modify
* @param    gport       @{(input)}  Port number
* @param    enable      @{(input)}  Wred params
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_db_port_vlan_translate_key_second_set(USL_DB_TYPE_t            dbType,
                                              bcm_gport_t              gport, 
                                              bcm_vlan_translate_key_t key);

/*********************************************************************
* @purpose  Compare the shadow & oper value. If they are
*           different then update the hw with Shadow value.
*
* @param    unit        @{(input)}  local bcm unit number
* @param    port        @{(input)}  local bcm port number
* @param    shadowElem  @{(input)}  Pointer to shadow db elem
* @param    operElem    @{(input)}  Pointer to oper db elem
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/

L7_int32 usl_port_pfc_config_update_bcm(L7_int32 unit, L7_int32 port,
                                           usl_port_db_elem_t *shadowElem,
                                           usl_port_db_elem_t *operElem);

/*********************************************************************
* @purpose  Set the pfc config for the port in the USL database
*
* @param    dbType          @{(input)}  dbType to modify
* @param    gport           @{(input)}  Port number
* @param    pfcConfig       @{(input)}  
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_pfc_config_set(USL_DB_TYPE_t dbType,
                              bcm_gport_t gport, 
                              usl_bcm_port_pfc_config_t *pfcConfig);

/*********************************************************************
* @purpose  Get the pfc config for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    pfcConfig        @{(output)}  
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_pfc_config_get(USL_DB_TYPE_t dbType,
                              bcm_gport_t gport, 
                              usl_bcm_port_pfc_config_t *pfcConfig);

/*********************************************************************
* @purpose  Get the pfc parms for the port in the USL database
*
* @param    dbType           @{(input)}  dbType to modify
* @param    gport            @{(input)}  Port number
* @param    mmuParms         @{(output)}  
*
* @returns  BCM error code
*
* @notes    
*       
* @end
*********************************************************************/
int usl_portdb_mmu_parms_get(USL_DB_TYPE_t dbType,
                             bcm_gport_t gport, 
                             usl_bcm_port_mmu_parms_t *mmuParms);

#endif /* L7_USL_PORT_DB_H */
