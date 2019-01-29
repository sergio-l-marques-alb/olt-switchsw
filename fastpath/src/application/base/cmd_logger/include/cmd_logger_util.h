
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename   cmd_logger_util.h
*
* @purpose   Command Logger file
*
* @component  cmdLogger component
*
* @comments   none
*
* @create     25-Oct-2004
*
* @author     gaunce
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef CMD_LOGGER_UTIL_H
#define CMD_LOGGER_UTIL_H

#include "l7_common.h"
#include "defaultconfig.h"

#define CMD_LOGGER_CFG_VER_1      0x1
#define CMD_LOGGER_CFG_VER_CURRENT    CMD_LOGGER_CFG_VER_1

#define CMD_LOGGER_CFG_FILENAME  "cmdLoggerCfgData.cfg"

/****************************************
*
*  Cli Web Configuration Data                    
*
*****************************************/

typedef enum 
{
  CMD_LOGGER_PHASE_INIT_0 = 0,
  CMD_LOGGER_PHASE_INIT_1,
  CMD_LOGGER_PHASE_INIT_2,
  CMD_LOGGER_PHASE_WMU,
  CMD_LOGGER_PHASE_INIT_3,
  CMD_LOGGER_PHASE_EXECUTE,
  CMD_LOGGER_PHASE_UNCONFIG_1,
  CMD_LOGGER_PHASE_UNCONFIG_2,
} cmdLoggerCnfgrState_t;                           
                           
typedef struct
{
  L7_fileHdr_t       cfgHdr;
  L7_uint32          adminMode;
  L7_LOG_SEVERITY_t  severity;
  L7_uint32          webAdminMode;
  L7_LOG_SEVERITY_t  webSeverity;
  L7_uint32          snmpAdminMode;
  L7_LOG_SEVERITY_t  snmpSeverity;
  L7_uint32          auditAdminMode;
  L7_uint32          checkSum;      /* keep this as last 4 bytes */

} cmdLoggerCfgData_t;

/*********************************************************************
* @purpose  Saves cmdLogger user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
cmdLoggerSave(void);

/*********************************************************************
* @purpose  Restores cmdLogger user config file to factore defaults
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
cmdLoggerRestore(void);

/*********************************************************************
* @purpose  Checks if cmdLogger user config data is changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL 
cmdLoggerHasDataChanged(void);
void cmdLoggerResetDataChanged(void);
/*********************************************************************
* @purpose  Build default cmdLogger config data  
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void 
cmdLoggerBuildDefaultConfigData(L7_uint32 ver);


/*********************************************************************
* @purpose  Migrate old config to new 
*
* @param    ver   Expected software version of Config Data
* @param    buffer Outdated config buffer
*
* @returns  void
*
* @notes    Current implementation resets to factory default.  Future
*           version could move fields of prior version into new structure.
*
* @end
*********************************************************************/
void 
cmdLoggerMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver, L7_char8 * buffer);

/*********************************************************************
* @purpose  cmdLogger set to factory defaults  
*
* @param    none
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void 
cmdLoggerInitData();

/*********************************************************************
* @purpose Initialize the cmdLogger for Phase 1
*
* @param   void
*
* @returns L7_SUCCESS  Phase 1 completed
* @returns L7_FAILURE  Phase 1 incomplete
*
* @notes  If phase 1 is incomplete, it is up to the caller to call the fini
*         function if desired.  If this fails, it is due to an inability to 
*         to acquire resources.
*
* @end
*********************************************************************/
L7_RC_t cmdLoggerPhaseOneInit(void);

/*********************************************************************
* @purpose  Release all resources collected during phase 1
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void cmdLoggerPhaseOneFini(void);

/*********************************************************************
* @purpose Initialize the cmdLogger for Phase 2
*
* @param   void
*
* @returns L7_SUCCESS  Phase 2 completed
* @returns L7_FAILURE  Phase 2 incomplete
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t cmdLoggerPhaseTwoInit(void);

/*********************************************************************
* @purpose Free the resource for phase 2
*
* @param   void
*
* @returns void
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
void cmdLoggerPhaseTwoFini(void);

/*********************************************************************
* @purpose Initialize the cmdLogger for Phase 3
*
* @param   void
*
* @returns L7_SUCCESS  Phase 3 completed
* @returns L7_FAILURE  Phase 3 incomplete
*
* @notes  If phase 3 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t cmdLoggerPhaseThreeInit(void);

/*********************************************************************
* @purpose Reset the cmdLogger to prior to phase 3
*
* @param   void
*
* @returns void
*
* @notes  If phase 3 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
void cmdLoggerPhaseThreeFini(void);



#endif /* CMD_LOGGER_UTIL_H */
