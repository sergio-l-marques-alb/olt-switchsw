/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename sslt_cfg.h
*
* @purpose SSL Tunnel configuration header
*
* @component sslt
*
* @comments none
*
* @create 07/15/2003
*
* @author spetriccione
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_SSLT_CFG_H
#define INCLUDE_SSLT_CFG_H

#include "sslt_include.h"
#include "sslt_exports.h"

/* LVL7 Configuration file */
#define SSLT_CFG_FILENAME    "sslt.cfg"
#define SSLT_CFG_VER_1       0x1
#define SSLT_CFG_VER_2       0x2
#define SSLT_CFG_VER_3       0x3
#define SSLT_CFG_VER_CURRENT SSLT_CFG_VER_3

typedef enum {
  SSLT_PHASE_INIT_0 = 0,
  SSLT_PHASE_INIT_1,
  SSLT_PHASE_INIT_2,
  SSLT_PHASE_WMU,
  SSLT_PHASE_INIT_3,
  SSLT_PHASE_EXECUTE,
  SSLT_PHASE_UNCONFIG_1,
  SSLT_PHASE_UNCONFIG_2,
} ssltCnfgrState_t;


#define SSLT_IS_READY (((ssltCnfgrState == SSLT_PHASE_INIT_3) || \
					   (ssltCnfgrState == SSLT_PHASE_EXECUTE) || \
					   (ssltCnfgrState == SSLT_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))


typedef struct {
  L7_BOOL ssltSave;
  L7_BOOL ssltRestore;
  L7_BOOL ssltHasDataChanged;
} ssltDeregister_t;

/* SSLT Operational configuration data */
typedef struct sslt_global_s
{
  /* switch admin port */
  L7_uint32 ssltAdminMode;
  L7_uint32 ssltOperMode;
  L7_uint32 ssltListenTaskId;
  L7_uint32 ssltSecurePort;
  /* auxiliary ports */
  L7_uint32 ssltAuxOperMode;
  L7_uint32 ssltAuxListenTaskId1;
  L7_uint32 ssltAuxListenTaskId2;
  L7_uint32 ssltAuxSecurePort1;
  L7_uint32 ssltAuxSecurePort2;
  /* common server & protocol */
  L7_uint32 ssltUnSecurePort;
  L7_uint32 ssltUnSecureServerAddr;
  L7_uchar8 ssltPemPassPhrase[L7_SSLT_PEM_PASS_PHRASE_MAX];
  L7_uint32 ssltProtocolLevel;
  L7_uint32 ssltDebugLevel;
  L7_uint32 ssltCertGenerateFlag;
  L7_uint32 ssltCertificateNumber;
} ssltGlobal_t;

/* SSLT configuration data */
typedef struct
{
  L7_uint32 ssltAdminMode;
  L7_uint32 ssltSecurePort;
  L7_uint32 ssltUnSecurePort;
  L7_uint32 ssltUnSecureServerAddr;
  L7_uchar8 ssltPemPassPhrase[L7_SSLT_PEM_PASS_PHRASE_MAX];
  L7_uint32 ssltProtocolSSL30;
  L7_uint32 ssltProtocolTLS10;
  L7_uint32 ssltSessionHardTimeOut;
  L7_uint32 ssltSessionSoftTimeOut;
  L7_uint32 ssltNumSessions;
  L7_uint32 ssltCertificateNumber;
} ssltCfgData_t;

typedef struct
{
  L7_fileHdr_t  hdr;
  ssltCfgData_t cfg;
  L7_uint32     checkSum;

} ssltCfg_t;

/* Debug Levels for printf's */
typedef enum
{
  SSLT_DEBUG_LVL0 = L7_NULL, /* off */
  SSLT_DEBUG_LVL1,           /* function Entry */
  SSLT_DEBUG_LVL2,           /* Misc. */
  SSLT_DEBUG_LVL3,           /* General error messages */
  SSLT_DEBUG_LVL4            /* SSL Semaphores */

} SSLT_DEBUG_LVL_t;

/*
** Internal function prototypes
*/

L7_RC_t ssltSave(void);

L7_BOOL ssltHasDataChanged(void);
void ssltResetDataChanged(void);
/*********************************************************************
*
* @purpose Build the SSLT configuration from defaults.
*
* @param ver @b{(input)} version of configuration file for SSLT
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltBuildDefaultConfigData(L7_uint32 ver);

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
void ssltMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);


void ssltApplyConfigData(void);

void ssltMigrateBuild(L7_uint32 oldVer, 
                      L7_uint32 ver, 
                      L7_char8 *buffer);

#endif /* INCLUDE_SSLT_CFG_H */

