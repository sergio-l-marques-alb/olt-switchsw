/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  protected_port_cfg.h
*
* @purpose   protected port configuration file
*
* @component protectedPort
*
* @comments 
*
* @create    6/5/2005
*
* @author    ryadagiri 
*
* @end
*             
**********************************************************************/
#ifndef PROTECTED_PORT_CFG_H
#define PROTECTED_PORT_CFG_H

#define PROTECTED_PORT_CFG_FILENAME       "protectedPort.cfg"
#define PROTECTED_PORT_CFG_VER_1 0x01
#define PROTECTED_PORT_CFG_VER_CURRENT           PROTECTED_PORT_CFG_VER_1

/* Configuration data */

typedef struct protectedPortGroup_s
{

    L7_char8 protectedPortName[L7_PROTECTED_PORT_GROUP_NAME_SIZE];
    L7_INTF_MASK_t protectedPortPortMask;

} protectedPortGroup_t;

typedef struct protectedPortCfg_s
{
    L7_fileHdr_t cfgHdr;
    protectedPortGroup_t protectedPortGroup[L7_PROTECTED_PORT_MAX_GROUPS];
    L7_uint32 checkSum;
} protectedPortCfg_t;


/* Begin Function Prototypes */

void protectedPortBuildDefaultConfigData(L7_uint32 ver);  
void protectedPortBuildDefaultIntfConfigData(protectedPortGroup_t * pCfg); 
L7_BOOL protectedPortHasDataChanged();           
void protectedPortResetDataChanged(void);
L7_RC_t protectedPortRestore();                  
L7_RC_t protectedPortSave();                     
L7_RC_t protectedPortApplyConfigData();          
L7_RC_t protectedPortApplyIntfConfigData(L7_uint32 intIfNum);
void protectedPortCfgRemove();

/* End Function Prototypes */

#endif /* PROTECTED_PORT_CFG_H */
