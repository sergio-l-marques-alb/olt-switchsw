/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename    gmrp_api.h
* @purpose     GMRP API
* @component   GMRP-GARP
* @comments    none
* @create      03/7/2002
* @author      Shekhar Kalyanam
* @end
*             
**********************************************************************/

#ifndef INCLUDE_GMRP_API_H_
#define INCLUDE_GMRP_API_H_
#include "nimapi.h"

#define L7_GMRP_MAX_INTF L7_MAX_INTERFACE_COUNT
#define L7_GMRP_MAX_CFG_IDX L7_MAX_INTERFACE_COUNT

typedef enum
{
  GMRP_NORMAL = 0,
  GMRP_FIXED,
  GMRP_FORBIDDEN
} GMRP_REG_TYPE_t;

typedef struct
{
  L7_uchar8       vlanIdmacAddress[8];
  NIM_INTF_MASK_t fixedReg;
  NIM_INTF_MASK_t forbiddenReg;
  L7_BOOL                 inUse;
} GMRP_REG_ENTRY_t;

typedef struct 
{
  L7_ushort16     vlanId;
  NIM_INTF_MASK_t fixedReg;
  NIM_INTF_MASK_t forbiddenReg;
} GMRP_SERVICE_ENTRY_t;

typedef struct
{
  nimConfigID_t configId;
} gmrpIntfCfgData_t;

typedef struct
{
  GMRP_REG_ENTRY_t         staticEntry[L7_MAX_GROUP_REGISTRATION_ENTRIES];
  GMRP_SERVICE_ENTRY_t staticForwardAll[L7_MAX_VLANS];
  GMRP_SERVICE_ENTRY_t staticForwardUnreg[L7_MAX_VLANS];
  L7_BOOL                          gmrpEnabled[L7_MAX_INTERFACE_COUNT+1];
} gmrpCfgData_t;

typedef struct
{
  L7_fileHdr_t      hdr;
  gmrpCfgData_t     cfg;
  gmrpIntfCfgData_t intf[L7_GARP_MAX_CFG_IDX];
  L7_uint32         checkSum;
} gmrpCfg_t;

/* Operational configuration used by apis and the protocol */
/*  this is converted to a config-id or config-mask based config during a save config */
typedef struct
{
  GMRP_REG_ENTRY_t         staticEntry[L7_MAX_GROUP_REGISTRATION_ENTRIES];
  GMRP_SERVICE_ENTRY_t staticForwardAll[L7_MAX_VLANS];         /* config offset */
  GMRP_SERVICE_ENTRY_t staticForwardUnreg[L7_MAX_VLANS];       /* config offset */
  L7_BOOL              gmrpEnabled[L7_MAX_INTERFACE_COUNT+1];  /* config id offset */
} gmrpOperCfg_t;

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpCreateGmr(L7_uint32 vlanId);

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpDestroyGmr(L7_uint32 vlanId);

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpInstanceEmptyIndexGet(L7_uint32 vlanId, 
                                         L7_uint32 *emptyIndex);

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpInstanceIndexFind(L7_uint32 vlanId, L7_uint32 *index);

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpConnectPort(L7_uint32 intIfNum);

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpDisconnectPort(L7_uint32 intIfNum);

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpSave(void);

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern L7_BOOL gmrpHasDataChanged(void);
extern void gmrpResetDataChanged(void);

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern void gmrpBuildDefaultConfigData(L7_uint32 ver);

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
void gmrpMigrateConfigData(L7_uint32 oldVer, 
                           L7_uint32 ver, L7_char8 *pCfgBuffer);

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to snoop nterface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL gmrpMapIntfConfigEntryGet(L7_uint32 intIfNum, 
                                  gmrpIntfCfgData_t **pCfg);

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpVlanChangeCallback(L7_uint32 vlanId, 
                                      L7_uint32 intIfnum, L7_uint32 event);

/*********************************************************************
* @purpose  See if an interface type is of the right type for GARP
*
* @param    sysIntfType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if not created, not configured or wrong type
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL gmrpIsValidIntfType(L7_INTF_TYPES_t sysIntfType);

/*********************************************************************
* @purpose  See if an interface type is of the right type for GARP
*
* @param    sysIntfType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if not created, not configured or wrong type
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL gmrpIsValidIntfType(L7_INTF_TYPES_t sysIntfType);

/*********************************************************************
* @purpose  See if an interface is of the right type for GARP
*
* @param    intifNum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if not created, not configured or wrong type
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL gmrpIsValidIntf(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to dot1q interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL gmrpMapIntfIsConfigurable(L7_uint32 intIfNum, 
                                  gmrpIntfCfgData_t **pCfg);

/*********************************************************************
*
* @purpose  Process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Allocates intf config storage if a config does not exist
*
* @end
*
*********************************************************************/
L7_RC_t gmrpIntfCreate(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t gmrpIntfDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern Gmr* gmrpInstanceGet(L7_uint32 vlanId);

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpPortStatusSet(L7_uint32 intIfNum, L7_uint32 status);

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpGroupEntryRegFixed(L7_uint32 vlanId, 
                                      L7_uchar8 *macAddr, L7_uint32 intIfNum);

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpGroupEntryRegForbid(L7_uint32 vlanId, 
                                       L7_uchar8 *macAddr, L7_uint32 intIfNum);

/*********************************************************************
* @purpose   
*
* @param    
*
* @returns  
*
* @notes   
*
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpGroupEntryRegNormal(L7_uint32 vlanId, 
                                       L7_uchar8 *macAddr, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Adds a port to the forwarding list for the vlan id group mac address entry
*
* @param    L7_uint32   vlanId   Vlan Id associated
* @param        L7_uchar8       *macAddr Group MacAddr
* @param        L7_uint32       intIfNum The interface to be forwarded on
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This routine check if the entry exists if not it creates an entry with the
*           mFdb and then proceeds to add this port in the forwarding mask
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpFwdPortAdd(L7_uint32 vlanId, 
                              L7_uchar8 *macAddr, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  deletes a port from the vlan id group mac address entry   
*
* @param    L7_uint32   vlanId   Vlan Id associated
* @param        L7_uchar8       *macAddr Group MacAddr
* @param        L7_uint32       intIfNum The interface to be forwarded on
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This routine proceeds to delete this interface from the forwarding mask
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpFwdPortDelete(L7_uint32 vlanId, 
                                 L7_uchar8 *macAddr, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  This routine delete a mFdb entry     
*
* @param    L7_uint32   vlanId   Vlan Id associated
* @param        L7_uchar8       *macAddr Group MacAddr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpEntryDelete(L7_uint32 vlanId, L7_uchar8 *macAddr);

/*********************************************************************
* @purpose  This checks if a given mac address is a group address or not     
*
* @param        L7_uchar8       *macAddr MacAddr
*
* @returns  L7_SUCCESS - if it is a group mac address
* @returns  L7_FAILURE - if it is a unicast mac adddress
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpGroupAddrCheck(L7_uchar8 *macAddr);

/*********************************************************************
* @purpose  This checks if a given mac address is a restricted address or not     
*
* @param        L7_uchar8       *macAddr MacAddr
*
* @returns  L7_SUCCESS - if it is NOT a restricted mac address
* @returns  L7_FAILURE - if it is a restricted mac adddress
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpMacAddrCheck(L7_uchar8 *macAddr);

/*********************************************************************
* @purpose  connects a port to all vlans gips that the port is a member of    
*
* @param    L7_uint32 mstId
* @param        L7_uint32 intIfNum 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpDot1sConnectPort(L7_uint32 mstID, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Disconnects a port from all vlans gips that it is a member of    
*
* @param    L7_uint32 mstId
* @param        L7_uint32 intIfNum 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t gmrpDot1sDisconnectPort(L7_uint32 mstID, L7_uint32 intIfNum);
#endif
