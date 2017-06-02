/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  rdisc_config.h
*
* @purpose   To discover router protocols
*
* @component 
*
* @comments  none
*
* @create    11/02/2001
*
* @author    ksdesai
*
* @end
*             
**********************************************************************/

#ifndef INCLUDE_RTR_DISC_CONFIG_H
#define INCLUDE_RTR_DISC_CONFIG_H

#include "l7_common.h"
#include "log.h"
#include "nimapi.h"

#define L7_RTR_DISC_FILENAME        "rtrDiscCfg.cfg"
#define L7_RTR_DISC_CFG_VER_5        0x5
#define L7_RTR_DISC_CFG_VER_CURRENT  L7_RTR_DISC_CFG_VER_5

#define MIN_ADV_INTERVAL      MAX_ADV_INTERVAL * .75
#define ADV_LIFETIME          MAX_ADV_INTERVAL * 3
          

/* packet size(Bytes) = Sizeof (Type + Code + Checksum + Num Addr + 
     Addr Entry Size + lifetime) + Max number of IP addresses 
     that can be set on an interface * sizeof(IPAddre + Preference Level)*/
#define RTR_DISC_PKT_SIZE             8 + 8 * L7_L3_NUM_IP_ADDRS

#define RTR_DISCOVERY_PKT_SIZE(num)                     ( 8 + 8 * (num))

typedef struct rtrDiscIntf_s
{
        L7_uint32 intIfNum;
        L7_uint32 ticksLeft;
        L7_uint32 ipAddr;               /* IP Address associated */
        L7_uint32 ipMask;
        L7_uchar8 numInitialAdvLeft;
        L7_BOOL   state;               /* L7_TRUE : send adv, L7_FALSE : do not send adv */
        struct rtrDiscIntf_s *prev;
        struct rtrDiscIntf_s *next;

}rtrDiscIntf_t;



/****************************************
*
*  Router Discovery Configuration Data         
*
*****************************************/
typedef struct rtrDiscIntfCfgData_s
{
  nimConfigID_t configId;
  L7_IP_ADDR_t  advAddress;
  L7_uint32     maxAdvInt;
  L7_uint32     minAdvInt;
  L7_uint32     advLifetime;
  L7_BOOL       advertise;        /* flag indicating whether address is to be advertised */
  L7_int32      preferenceLevel;  /* the prefarability of each router address */

} rtrDiscIntfCfgData_t;


typedef struct rtrDiscCfgData_s
{
  L7_fileHdr_t            cfgHdr;
  rtrDiscIntfCfgData_t    rtrDiscIntfCfgData[L7_RTR_DISC_INTF_MAX_COUNT];
  L7_uint32               checkSum;

} rtrDiscCfgData_t;



/* Function prototypes */

/*********************************************************************
* @purpose  Print the current router discovery config values to 
*           serial port
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    prints out info for all internal interfaces that are present
*       
* @end
*********************************************************************/
L7_RC_t rtrDiscConfigDump();

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to policy interface config structure
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
L7_BOOL rtrDiscMapIntfIsConfigurable(L7_uint32 intIfNum, rtrDiscIntfCfgData_t **pCfg);

/*********************************************************************
* @purpose  Applies router discovery config data
*
* @param    void 
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t rtrDiscApplyConfigData (void);

/*********************************************************************
* @purpose  Applies interface router discovery config data
*
* @param    void 
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t rtrDiscApplyIntfConfigData(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Build default router discovery data  
*
* @param    ver   @b{(input)} Software version of Discovery Data
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void rtrDiscBuildDefaultConfigData(L7_uint32 ver);


/*********************************************************************
* @purpose  Build default router discovery data for the interface 
*
* @param    pCfg                pointer to intf configuration information
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void rtrDiscIntfBuildDefaultConfigData(rtrDiscIntfCfgData_t *pCfg);

/*********************************************************************
* @purpose  Save the router discovery data  
*
* @param    ver   @b{(input)} Software version of Discovery Data
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t rtrDiscSave(void);

/*********************************************************************
* @purpose  Restore the router discovery data  
*
* @param    ver   @b{(input)} Software version of Discovery Data
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void rtrDiscRestoreProcess(void);


/*********************************************************************
* @purpose  Checks if router discovery user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL rtrDiscHasDataChanged(void);
void rtrDiscResetDataChanged(void);

/* rtrdisc_migrate.c */

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
void rtrDiscMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

#endif /* INCLUDE_RTR_DISC_CONFIG_H */
