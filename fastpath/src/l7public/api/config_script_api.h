/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename config_script_api.h
 *
 * @purpose header for the config_script api
 *
 * @component user interface
 *
 * @comments
 *
 * @create  01/15/2004 
 *
 * @author  Sarvesh Kumar
 * @end
 *
 **********************************************************************/

#ifndef CONFIG_SCRIPT_API_H
#define CONFIG_SCRIPT_API_H

#include "l7_common.h"
#include "sysapi.h"

/******************************************
 *
 *Defines for Configuration Script
 *
 *******************************************/

#define CONFIG_SCRIPT_FILE_EXTENSION     ".scr"
#define TEMP_CONFIG_SCRIPT_FILE_NAME     "TempConfigScript.scr"
#define TEMP_RUNNINGCONFIG_SCRIPT_FILE_NAME  "TempRunningCfgScript.scr"
#ifdef L7_AUTO_INSTALL_PACKAGE
#define AUTO_INSTALL_TEMP_SCRIPT_FILENAME    "AutoInstallTmpScript.scr"
#endif
#define CONFIG_SCRIPT_MAX_COMMAND_SIZE   1024
#define CONFIG_SCRIPT_MAX_COUNT_LIMIT    10
#define CONFIG_SCRIPTS_MAX_SIZE_LIMIT    (2*1024*1024) /* 2 Mbytes*/
#define SHOW_RUNNING_CONFIG_ALL_MAX_SIZE_LIMIT    (5*1024*1024) /* 5 Mbytes*/
#define CONFIG_SCRIPT_ERROR_MSG_MAX_SIZE 256

#define CONFIG_SCRIPT_DELIMITER      "\n"
#define CONFIG_SCRIPT_DELIMITER_CHAR '\n'

#define CONFIG_SCRIPT_DELIMITER_LEN strlen(CONFIG_SCRIPT_DELIMITER)

/*************************************
 *
 * Command Structure
 *
 *************************************/

typedef struct cfgCommand_s
{
  L7_char8              cmdString[CONFIG_SCRIPT_MAX_COMMAND_SIZE]; 
  L7_uint32             nextWritePos;
  L7_uint32             noOfCmds;
  struct cfgCommand_s   *next;
} cfgCommand_t;


/***********************************************
 *
 * Configuration Script Global Data
 *
 **********************************************/

typedef struct L7_ConfigScript_s 
{
  L7_fileHdr_t        cfgHdr;    
  L7_uint32           listCount;
  cfgCommand_t        *cfgData;
  cfgCommand_t        *last;
  L7_uint32           size;
  int                 lastIndex; /* Used for paginating the output of "show running-config" */
} L7_ConfigScript_t;


/*******Functions for Configuration scripting*************************/
L7_RC_t readConfigScriptData(const L7_char8 *fileName, L7_ConfigScript_t *configScriptData);
L7_RC_t writeConfigScriptData(L7_ConfigScript_t *configScriptData); 
L7_uint32 getConfigScriptList(SYSAPI_DIRECTORY_t *tree);  
L7_RC_t initialiseScriptBuffer(L7_char8 *filename, L7_ConfigScript_t *configScriptData);
L7_RC_t releaseScriptBuffer(L7_ConfigScript_t *configScriptData); 
L7_RC_t checkConfigScriptFiletype(const L7_char8 *filename) ;
L7_RC_t checkConfigScriptFilename(const L7_char8 *filename) ;
L7_RC_t checkConfigScriptPresent(const L7_char8 *filename); 
void setLastError(L7_char8* errMsg);
const L7_char8* getLastError();
void resetLastError();
L7_RC_t getConfigScriptSourceFileName(L7_char8 *filename, L7_uint32 filenameSize);
L7_RC_t setConfigScriptSourceFileName(const L7_char8 *filename);
L7_char8* trimLeft(L7_char8 * str);
void trimRight(L7_char8* str);
L7_char8 * trimLeftBlanks(L7_char8 * str);
void trimRightBlanks(L7_char8 * str);
L7_char8* allTrim(L7_char8* str);
L7_RC_t setNextConfigScriptCommand(const L7_char8 *command, L7_ConfigScript_t *configScriptData) ;
L7_RC_t getConfigScriptCommand(L7_uint32 lineNumber, L7_ConfigScript_t *configScriptData, L7_uchar8* *command) ;
L7_RC_t checkConfigScriptSizeLimit(L7_uint32 newFileSize, L7_char8* filename);
L7_RC_t checkConfigScriptCountLimit(L7_char8* filename);  
L7_RC_t getConfigScriptsSize(L7_char8* filename, L7_uint32 *filesSize); 
L7_RC_t cliCfgScriptCopy (L7_char8 * outputFileName);

#endif
