/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dvlantag_ih.h
*
* @purpose   Double Vlan Tagging Interface file
*
* @component dvlantag
*
* @component switching
*
* @create    08/20/2003
*
* @author    skalyanam
*
* @end
**********************************************************************/   

#ifndef DVLANTAG_IH_H
#define DVLANTAG_IH_H


L7_RC_t dvlantagIntfModeApply(L7_uint32 intIfNum, L7_uint32 mode);
L7_RC_t dvlantagEthertypeApply(L7_uint32 etherType, L7_uint32 tpidIdx);
L7_RC_t dvlantagIntfEthertypeApply(L7_uint32 intIfNum, L7_uint32 etherType, L7_BOOL enable);
L7_RC_t dvlantagIntfCustIdApply(L7_uint32 intIfNum, L7_uint32 custId);
L7_BOOL dvlantagIntfIsAcquired(L7_uint32 intIfNum);
L7_RC_t dvlantagIntfChangeCallBack(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);
L7_RC_t dvlantagConfigPortApply(L7_uint32 intIfNum, 
								L7_uint32 mode,
								L7_uint32 etherValMask,
								L7_uint32 custId);
L7_BOOL dvlantagIntfIsConfigurable(L7_uint32 intIfNum, DVLANTAG_INTF_CFG_t **pCfg);
L7_BOOL dvlantagIntfConfigEntryGet(L7_uint32 intIfNum, DVLANTAG_INTF_CFG_t **pCfg);
L7_RC_t dvlantagIntfCreate(L7_uint32 intIfNum);
L7_RC_t dvlantagIntfDetach(L7_uint32 intIfNum);
L7_RC_t dvlantagIntfDelete(L7_uint32 intIfNum);
L7_RC_t dvlantagDefaultTpidApply(L7_uint32 etherType);
#endif/*DVLANTAG_IH_H*/
