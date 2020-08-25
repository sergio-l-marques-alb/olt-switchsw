/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename nim_config.c
*
* @purpose NIM config file management
*
* @component nim
*
* @comments none
*
* @create 08/30/2000
*
* @author mbaucom
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#ifndef NIM_CONFIG_H
#define NIM_CONFIG_H

#include "l7_common.h"
#include "nim_data.h"

#define L7_NIM_FILE_NAME_LENGTH       20

#define NIM_CFG_VER_1  0x1
#define NIM_CFG_VER_2  0x2
#define NIM_CFG_VER_3  0x3
#define NIM_CFG_VER_4  0x4
#define NIM_CFG_VER_5  0x5
#define NIM_CFG_VER_CURRENT  NIM_CFG_VER_5
#define NIM_CFG_FILE_NAME  "nimStack.cfg"

/*********************************************************************
* @purpose  initialize the config structures during phase 3 init
*
* @param    void
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void nimConfigInit(void);

/*********************************************************************
* @purpose  Setup the list of config file names  
*
* @param    void
*
* @returns  L7_SUCCESS  Success or sysapiRegistrGet error code
*
* @notes   
*
*       
* @end
*
*********************************************************************/
L7_RC_t nimFileSetup(void);


/*********************************************************************
* @purpose  Saves all nim user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error from osapiFsWrite
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t nimSave(void);

/*********************************************************************
* @purpose  Checks if nim user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL nimHasDataChanged(void);
void nimResetDataChanged(void);

/*********************************************************************
* @purpose  Creates an unique interface id
*
* @param    usp         @b{(input)} Pointer to nimUSP_t structure
* @param    configId    @b{(output)} Pointer to the new config ID
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t nimConfigIdCreate(nimUSP_t *usp, nimConfigID_t *configId);

/*********************************************************************
* @purpose  Allocate the memory for the memory copy of the config file
*
* @param    void
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t nimConfigPhase2Init(void);

/*********************************************************************
* @purpose  Migrate the config data
*
* @param    savedVersion  @b{(input)} version of the present config file
* @param    newVersion    @b{(input)} version to convert to
* @param    buffer        @b{(input)} the buffer of the cfg file
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Since this is the first release of Stacking, no migration to do
*       
* @end
*********************************************************************/
void nimConfigConvert(L7_uint32 savedVersion,L7_uint32 newVersion,L7_char8 *buffer);

/*********************************************************************
* @purpose  Set either the saved config or the default config in the interface
*
* @param    port        @b{(input)}   The interface being manipulated
* @param    cfgVersion  @b{(input)}   version of the config
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t nimConfigSet(nimIntf_t *port, L7_uint32 cfgVersion);

/*********************************************************************
* @purpose  apply the config to the system
*
* @param    intIfNum    @b{(input)}   The intenal interface to apply
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t nimIntfConfigApply(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the default config for the interface
*
* @param    intfDescr   @b{(input)}   A description of the interface being created
* @param    defaultCfg  @b{(output)}  The created config
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Since this is the first release of Stacking, no migration to do
*       
* @end
*********************************************************************/
L7_RC_t nimConfigDefaultGet(nimIntfDescr_t *intfDescr, nimIntfConfig_t *defaultCfg);

/*********************************************************************
* @purpose  Get the default config for the interface
*
* @param    version   @b{(input)} The version to create a default for
*
* @returns  void
*
* @notes    Since this is the first release of Stacking, no migration to do
*       
* @end
*********************************************************************/
void nimConfigFileDefaultCreate(L7_uint32 version);

/*********************************************************************
* @purpose  Sets the config file header
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Since this is the first release of Stacking, no migration to do
*       
* @end
*********************************************************************/
L7_RC_t nimConfigFileHdrCreate();

/*********************************************************************
* @purpose  Sets the offsets for ports and crc in the config struct
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    Since this is the first release of Stacking, no migration to do
*       
* @end
*********************************************************************/
L7_RC_t nimConfigFileOffsetsSet();

/* nim_migrate.c */
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
* @end
*********************************************************************/
void nimMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

#endif /* NIM_CONFIG_H */
