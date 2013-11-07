/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename cnfgr_sid_extern.h
*
* @purpose    Configurator component extern header file that contains
*             the function externs used by the configurator structural
*             initialization data.
*
* @component cnfgr
*
* @comments none
*
* @create 10/12/2004
*
* @author cpverne
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_CNFGR_SID_EXTERN_H
#define INCLUDE_CNFGR_SID_EXTERN_H

extern void dhcpClientApiCnfgrCommand();
extern void simApiCnfgrCommand();
extern void nimApiCnfgrCommand();
extern void dtlApiCnfgrCommand();
extern void cmgrApiCnfgrCommand();
extern void poeApiCnfgrCommand();
extern void userMgrApiCnfgrCommand();
extern void cliWebApiCnfgrCommand();
extern void cmdLoggerApiCnfgrCommand();
extern void policyApiCnfgrCommand();
extern void mirrorApiCnfgrCommand();
extern void fdbApiCnfgrCommand();
extern void snmpApiCnfgrCommand();
extern void edbApiCnfgrCommand();
extern void dot1qApiCnfgrCommand();
extern void pbVlanApiCnfgrCommand();
extern void vlanIpSubnetApiCnfgrCommand();
extern void vlanMacApiCnfgrCommand();
extern void dot1xApiCnfgrCommand();
extern void dot1sApiCnfgrCommand();
extern void garpApiCnfgrCommand();
extern void dot1adApiCnfgrCommand();
extern void mfdbApiCnfgrCommand();
extern void filterApiCnfgrCommand();
extern void radiusApiCnfgrCommand();
extern void macalApiCnfgrCommand();
extern void tacacsApiCnfgrCommand();
extern void unitMgrApiCnfgrCommand();
extern void snoopApiCnfgrCommand();
extern void dot3adApiCnfgrCommand();
extern void trapMgrApiCnfgrCommand();
extern void ssltApiCnfgrCommand();
extern void sshdApiCnfgrCommand();
extern void opensslApiCnfgrCommand();
extern void dvlantagApiCnfgrCommand();
extern void ipv6ProvApiCnfgrCommand();
extern void fftpApiCnfgrCommand();
extern void spmApiCnfgrCommand();
extern void cdaApiCnfgrCommand();
extern void ckptApiCnfgrCommand();
extern void dhcpsApiCnfgrCommand();
extern void logApiCnfgrCommand();

extern void sntpApiCnfgrCommand();
extern void telnetApiCnfgrCommand();
extern void pmlApiCnfgrCommand();
extern void protectedPortApiCnfgrCommand();
extern void dhcpFilterApiCnfgrCommand( void );
extern void dsApiCnfgrCommand(void);
extern void dot3ahApiCnfgrCommand(void);
extern void dot1agApiCnfgrCommand(void);
extern void daiApiCnfgrCommand(void);
extern void doSApiCnfgrCommand();
extern void voiceVlanApiCnfgrCommand();
extern void sFlowApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData);
extern void tr069ApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData);
extern void pfcApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData);
#ifdef L7_CHASSIS
extern void alarmApiCnfgrCommand();
extern void chassisAlarmApiCnfgrCommand();
extern void pumgrApiCnfgrCommand();
extern void fumgrApiCnfgrCommand();
#endif
/* extern that belongs to flex packages */
#ifdef L7_ROUTING_PACKAGE
extern void ipMapApiCnfgrCommand();
extern void ripMapApiCnfgrCommand();
extern void ospfMapApiCnfgrCommand();
extern void vrrpApiCnfgrCommand();
extern L7_RC_t L7_vrrpAdminModeSet(L7_BOOL adminMode);
extern void dhcpRelayApiCnfgrCommand();
extern void rtrDiscApiCnfgrCommand();
extern void ospfv3MapApiCnfgrCommand();
extern void rlimApiCnfgrCommand();
#endif

#ifdef L7_BGP_PACKAGE
extern void bgpApiCnfgrCommand();
extern L7_RC_t bgpMapBgpAdminModeSet(L7_uint32 mode);
#endif


#ifdef L7_MCAST_PACKAGE
extern void mcastApiCnfgrCommand();
extern void mgmdApiCnfgrCommand();
extern void dvmrpApiCnfgrCommand();
extern void pimdmApiCnfgrCommand();
extern void pimsmApiCnfgrCommand();
#endif

extern void mfdbApiCnfgrCommand();
extern void radiusApiCnfgrCommand();
extern void macalApiCnfgrCommand();

#ifdef L7_QOS_PACKAGE
#ifdef L7_QOS_FLEX_PACKAGE_DIFFSERV
extern void diffServApiCnfgrCommand();
#endif
#ifdef L7_QOS_FLEX_PACKAGE_ACL
extern void aclApiCnfgrCommand();
#endif
#ifdef L7_QOS_FLEX_PACKAGE_COS
extern void cosApiCnfgrCommand();
#endif
#ifdef L7_QOS_FLEX_PACKAGE_VOIP
extern void voipApiCnfgrCommand();
#endif
#ifdef L7_QOS_FLEX_PACKAGE_ISCSI
extern void iscsiApiCnfgrCommand();
#endif
#endif

#ifdef L7_SERVICES_PACKAGE
extern void servicesApiCnfgrCommand();
#endif
#ifdef L7_IPV6_PACKAGE
extern void ip6MapApiCnfgrCommand();
extern void dhcp6sApiCnfgrCommand();
#endif

extern void dnsApiCnfgrCommand();

#ifdef L7_WIRELESS_PACKAGE
extern void wirelessApiCnfgrCommand();
#endif

#ifdef L7_FLEX_CAPTIVE_PORTAL_PACKAGE
extern void captivePortalApiCnfgrCommand();
#endif

#ifdef L7_CP_WIO_PACKAGE
extern void wioApiCnfgrCommand();
#endif


extern void lldpApiCnfgrCommand();
extern void boxsApiCnfgrCommand();
extern void linkDependencyApiCnfgrCommand();
extern void autoInstallApiCnfgrCommand();
 
#ifdef L7_ISDP_PACKAGE
extern void isdpApiCnfgrCommand();
#endif /* L7_ISDP_PACKAGE */

#ifdef L7_LLPF_PACKAGE
extern void llpfApiCnfgrCommand();
#endif /* L7_LLPF_PACKAGE */

#ifdef L7_RMON_PACKAGE
extern void rmonApiCnfgrCommand();
#endif /* L7_RMON_PACKAGE */

extern void dhcp6cApiCnfgrCommand();

extern void dot1xAuthServApiCnfgrCommand();

#ifdef L7_TIMERANGES_PACKAGE
extern void timeRangeApiCnfgrCommand();
#endif /* L7_TIMERANGES_PACKAGE */

/* PTin added: ptin module */
extern void ptinApiCnfgrCommand();
#ifdef PTIN_PPPOE_PACKAGE
extern void pppoeApiCnfgrCommand();
#endif

#endif /* INCLUDE_CNFGR_SID_EXTERN_H */
