/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename dot1q_cfg.c
*
* @purpose config data for dot1q
*
* @component dot1q
*
* @comments none
*
* @create 11/1/2005
*
* @author akulkarni
*
* @end
*             
**********************************************************************/
#ifndef __DOT1Q_CFG_H__
#define __DOT1Q_CFG_H__

/* Config */
L7_RC_t dot1qSave(void);
L7_RC_t dot1qRestore(void);
L7_BOOL dot1qHasDataChanged(void);
void dot1qResetDataChanged(void);
void    dot1qBuildDefaultConfigData(L7_uint32 ver);


L7_RC_t dot1qBuildDefaultIntfConfig(L7_uint32 intf);
void dot1qIntfBuildDefaultConfigData(dot1qIntfCfgData_t *pCfg);
void dot1qIntfBuildDefaultVlanConfigData(L7_uint32 intIfNum, dot1qVlanCfgData_t *vCfg);
L7_RC_t dot1qApplyIntfConfig(L7_uint32 intf);
void    dot1qBuildDefaultQvlanData(L7_uint32 ver);
L7_RC_t dot1qApplyQvlanConfigData(void);

L7_RC_t dot1qApplyConfigData(void);
L7_RC_t dot1qApplyGarpConfigData(void);
L7_RC_t dot1qQportNumTrafficClassesUpdate(L7_uint32 intf, L7_uint32 numTrafficClasses);
L7_RC_t dot1qReset(void);

L7_RC_t dot1qIntfCreate(L7_uint32 intIfNum);
L7_RC_t dot1qIntfDelete(L7_uint32 intIfNum);
L7_RC_t dot1qApplyIntfConfigData(L7_uint32 intIfNum);
L7_RC_t dot1qApplyIntfConfigToHW(L7_uint32 intIfNum);
L7_BOOL dot1qMapIntfIsConfigurable(L7_uint32 intIfNum, dot1qIntfCfgData_t **pCfg);
void    dot1qMigrateBuild(L7_uint32 parm1, L7_uint32 parm2, L7_char8 *p);
L7_RC_t dot1qIntfUnapplyConfigData(L7_uint32 intIfNum);

L7_BOOL dot1qVlanConfigEntryGet(L7_uint32 vlanId, dot1qVlanCfgData_t **pVCfg);
L7_RC_t dot1qVlanConfigEntryReset(L7_uint32 vlanId);
L7_BOOL dot1qVlanIsConfigurable(L7_uint32 vid, dot1qVlanCfgData_t **pVCfg);
L7_RC_t dot1qVlanMemberSetCfgUpdate(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 mode);
L7_RC_t dot1qVlanTaggedMemberSetCfgUpdate(L7_uint32 vid, L7_uint32 intIfNum, L7_uint32 tagMode);


#endif /* __DOT1Q_CFG_H__*/

