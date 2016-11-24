/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dot3adcfg.h
*
* @purpose defines data structures and prototypes for CFG
*
* @component Link aggregation Group dot3ad (IEEE 802.3ad)
*
* @comments none
*
* @create 6/5/2001
*
* @author skalyanam
* @end
*
**********************************************************************/

#ifndef INCLUDE_DOT3AD_CFG_H
#define INCLUDE_DOT3AD_CFG_H

#include "l7_common.h"
#include "nimapi.h"

#define DOT3AD_CFG_FILENAME       "dot3ad.cfg"
#define DOT3AD_CFG_VER_1          0x1
#define DOT3AD_CFG_VER_2          0x2
#define DOT3AD_CFG_VER_3          0x3
#define DOT3AD_CFG_VER_4          0x4
#define DOT3AD_CFG_VER_5          0x5
#define DOT3AD_CFG_VER_CURRENT    DOT3AD_CFG_VER_5
#define DOT3AD_DEBUG_CFG_FILENAME "dot3ad_debug.cfg"
#define DOT3AD_DEBUG_CFG_VER_1          0x1
#define DOT3AD_DEBUG_CFG_VER_CURRENT    DOT3AD_DEBUG_CFG_VER_1

#define DOT3AD_BUFFER_SIZE		  sizeof(dot3ad_pdu_t)

typedef struct
{
  dot3ad_port_t dot3adPort[L7_MAX_PORT_COUNT + 1];      /* holds per port into       */
  dot3ad_agg_t  dot3adAgg[L7_MAX_NUM_LAG_INTF];     /* holds per aggregator info */
  dot3ad_system_t dot3adSystem;                     /* holds system info         */
  L7_uint32 dot3adAggIdx[L7_MAX_INTERFACE_COUNT];   /* maps an aggregator interface
                                                       number to a dot3adAgg index */
  L7_uint32 dot3adPortIdx[L7_MAX_PORT_COUNT+1];       /* maps a port interface number
                                                       to a dot3adPort index       */ 
} dot3adCfgData_t;

typedef struct
{
  L7_fileHdr_t     hdr;
  dot3adCfgData_t  cfg;
  L7_uint32        checkSum;
} dot3adCfg_t;


typedef struct dot3adDebugCfgData_s 
{
  L7_BOOL dot3adDebugPacketTraceFlag;
} dot3adDebugCfgData_t;

typedef struct dot3adDebugCfg_s
{
  L7_fileHdr_t          hdr;
  dot3adDebugCfgData_t  cfg;
  L7_uint32             checkSum;
} dot3adDebugCfg_t;


/*********************************************************************
* @purpose  Saves dot3ad configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot3adCfg is the overlay
*       
* @end
*********************************************************************/
L7_RC_t dot3adSave(void);

/*********************************************************************
* @purpose  Restores dot3ad configuration
*
* @param    void     
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot3adCfg is the overlay
*       
* @end
*********************************************************************/
L7_RC_t dot3adRestore(void);

/*********************************************************************
* @purpose  Checks if dot1q user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL dot3adHasDataChanged(void);
void dot3adResetDataChanged(void);
/*********************************************************************
* @purpose  Build default dot3ad config data  
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void dot3adBuildConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Apply dot3ad config data  
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dot3adApplyConfigData(void);

/*********************************************************************
* @purpose  Update dot3ad information in the Cfg file
*
* @param    vlanID      VLAN ID 
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*            
*       
* @end
*********************************************************************/
L7_RC_t   dot3adCfgUpdate(void);
/*********************************************************************
* @purpose  
*
* @param    intIfNum internal interface number  
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*            
*       
* @end
*********************************************************************/
L7_RC_t dot3adLagIntfInit(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Receive the completion notification from NIM event notifications
*
* @param    intIfNum internal interface number  
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*            
*       
* @end
*********************************************************************/
void dot3adNimEventCompletionCallback(NIM_NOTIFY_CB_INFO_t retVal);

/*********************************************************************
* @purpose  Receive the completion notification for create from NIM event notifications
*
* @param    intIfNum internal interface number  
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    This function is a wrapper for diff action on the completion callback
*           for the intf create event when port channel is created through usmdb.
*            
*       
* @end
*********************************************************************/
void dot3adNimEventCreateCompletionCallback(NIM_NOTIFY_CB_INFO_t retVal);

/*********************************************************************
* @purpose  Build default dot3ad data for a physical interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the phys interface
* @param    ver       @b{(input)}   Software version of Config Data
* @param    intf      @b{(output)}  the pointer to the interface
* @param    idx       @b{(output)}  the pointer to the appropriate idx array
*
* @returns  void
*
* @notes    Since the runtime and config structures are the same, this routine
*           can be used to set either structure to the defaults   
*
* @end
*********************************************************************/
L7_RC_t dot3adPhysIntfDefaultBuild( L7_uint32 intIfNum, L7_uint32 ver, dot3ad_port_t *intf, L7_uint32 *idx);

/*********************************************************************
* @purpose  Build default dot3ad data for a lag interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    lagId     @b{(input)}   lagId of the lag
* @param    ver       @b{(input)}   Software version of Config Data
* @param    agg       @b{(output)}  The agg port pointer to be modified
* @param    idx       @b{(output)}  The agg idx pointer to be modified
*
* @returns  L7_SUCCESS
*
* @notes    Since the runtime and config structures are the same, this routine
*           can be used to set either structure to the defaults   
*
* @end
*********************************************************************/
L7_RC_t dot3adLagIntfDefaultBuild(L7_uint32 intIfNum, L7_uint32 lagId, L7_uint32 ver, 
                                  dot3ad_agg_t *agg, L7_uint32 *idx);

/*********************************************************************
* @purpose  Interface creation from NIM
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    status    @b{(inout)}   The status for returning to NIM
* @param    callback  @b{(output)}  flag to determine if the caller should do NIM callback
*
* @returns  L7_SUCCESS
*
* @notes    The LAG create is handled by the generator of the event.  This is done to insure 
*           lag has done the necessary creation before other components are made aware of
*           of the interface
*
* @end
*********************************************************************/
L7_RC_t dot3adIntfCreate(L7_uint32 intIfNum,NIM_EVENT_COMPLETE_INFO_t *status,L7_BOOL *callback);


/*********************************************************************
* @purpose  Build default dot3ad data for a lag interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    status    @b{(inout)}   The status for returning to NIM
* @param    callback  @b{(output)}  flag to determine if the caller should do NIM callback
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dot3adIntfAttach(L7_uint32 intIfNum,NIM_EVENT_COMPLETE_INFO_t *status,L7_BOOL *callback);

/*********************************************************************
* @purpose  Build default dot3ad data for a lag interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    status    @b{(inout)}   The status for returning to NIM
* @param    callback  @b{(output)}  flag to determine if the caller should do NIM callback
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dot3adIntfDetach(L7_uint32 intIfNum,NIM_EVENT_COMPLETE_INFO_t *status,L7_BOOL *callback);

/*********************************************************************
* @purpose  Build default dot3ad data for a lag interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    status    @b{(inout)}   The status for returning to NIM
* @param    callback  @b{(output)}  flag to determine if the caller should do NIM callback
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dot3adIntfDelete(L7_uint32 intIfNum,NIM_EVENT_COMPLETE_INFO_t *status,L7_BOOL *callback);

/* dot3ad_migrate.c */

/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
void dot3adMigrateConfigData(L7_uint32 oldVer , L7_uint32 ver, L7_char8 *pCfgBuffer);

/*********************************************************************
* @purpose  Saves dot3ad configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot3adDebugCfg is the overlay
*       
* @end
*********************************************************************/
L7_RC_t dot3adDebugSave(void);

/*********************************************************************
* @purpose  Restores dot3ad debug configuration
*
* @param    void     
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot3adDebugCfg is the overlay
*       
* @end
*********************************************************************/
L7_RC_t dot3adDebugRestore(void);

/*********************************************************************
* @purpose  Checks if dot3ad debug config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL dot3adDebugHasDataChanged(void);

/*********************************************************************
* @purpose  Build default dot3ad config data  
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void dot3adDebugBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Apply dot3ad debug config data  
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t dot3adApplyDebugConfigData(void);

#endif /* INCLUDE_DOT3AD_CFG_H*/
