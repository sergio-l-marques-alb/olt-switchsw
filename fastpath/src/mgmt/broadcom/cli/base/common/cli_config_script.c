/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/cli_config_script.c
*
* @purpose Implementation functions for configuration script.
*
* @component user interface
*
* @comments
*
* @create  01/15/2004
*
* @author  Rahul Hak, Sarvesh Kumar
* @end
*
**********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_security_cli.h"
#include <string.h>


#include "cli_config_script.h"
#include "osapi.h"
#include "usmdb_sim_api.h"
#include "transfer_exports.h"

#define MAX_ERROR_COUNT 2048
extern L7_uint32 cliGetCurrentMode (void);
static L7_uint32 validationErrorLines[MAX_ERROR_COUNT];
static L7_uint32 pwRecoveryFlag = L7_FALSE;

/*********************************************************************
*
* @purpose  Function to reset the buffer containg error line numbers
*
* @param none
*
* @returntype void
* @returns
*
* @notes
*
* @end
*
*********************************************************************/
void resetValidationErrorLines()
{
  L7_uint32 i = 0;

  while (i < MAX_ERROR_COUNT)
  {
      validationErrorLines[i] = 0;
      i++;
  }
}

/*********************************************************************
*
* @purpose  Function to reset the buffer containg error line numbers
*
*
* @param none
*
* @returntype void
* @returns
*
* @notes
*
*
* @end
*
*********************************************************************/
void addValidationErrorLine(L7_uint32 lineNo)
{
  L7_uint32 i = 0;

  while (i < MAX_ERROR_COUNT && lineNo > 0)
  {
      if(validationErrorLines[i] == 0)
      {
        validationErrorLines[i] =  lineNo;
        break;
      }
      i++;
  }
}

/*********************************************************************
*
* @purpose  Applies the configuration script
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  script apply <scriptname>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigScriptApply(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 unit, numArgs;
   L7_ConfigScript_t configScriptData;
   L7_char8 tempStr[CONFIG_SCRIPT_MAX_COMMAND_SIZE];
   L7_char8 altCmdStr[CONFIG_SCRIPT_MAX_COMMAND_SIZE];
   L7_BOOL hasDataChanged;
   L7_char8 userInput;
   enum
   {
     ApplyScriptQuestion = 1,
     saveChangesQuestion = 2
   };

   cliSyntaxTop(ewsContext);
   ewsContext->unbufferedWrite = L7_TRUE;

   numArgs = cliNumFunctionArgsGet();

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
   }

   if(numArgs != 1)
   {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgScriptApply_1);
     ewsContext->unbufferedWrite = L7_FALSE;
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  osapiSnprintf(altCmdStr, sizeof(altCmdStr), "%s %s", pStrErr_base_CfgScriptApplyCmd, argv[index+1]);
   cliAlternateCommandSet(altCmdStr);            /*  config script apply */

  switch (cliGetCharInputID())
  {
   case ApplyScriptQuestion:
    if ( tolower(cliGetCharInput()) == 'y' )
    {
      usmDbUnsavedConfigCheck(unit, &hasDataChanged);
      if (hasDataChanged == L7_TRUE)
      {
        cliSetCharInputID(saveChangesQuestion, ewsContext, argv);
        return pStrInfo_base_QuestionSaveChgs;
      }
    }
    else
    {
      /*Configuration  script Not applied!*/
      ewsTelnetWrite(ewsContext, pStrInfo_base_CfgScriptApplyCanceled);
      ewsContext->unbufferedWrite = L7_FALSE;
      cliSyntaxBottom(ewsContext);
      return cliSetPriviledgeUserMode();
    }
    break;

   case saveChangesQuestion:

    userInput =  tolower(cliGetCharInput());
    if ( userInput == 'y' )
    {
      usmDbSwDevCtrlSaveConfigurationSet(unit, 1);
      /*  inside usmdb now nvStoreSave(); saveConfigPermanently(); */
      ewsTelnetWrite(ewsContext, pStrInfo_common_MsgCfgSaved);
    }
    else if ( userInput == 'n' )
    {
      ewsTelnetWrite(ewsContext, pStrErr_common_MsgCfgNotSaved);
    }
    else
    {
      cliSetCharInputID(saveChangesQuestion, ewsContext, argv);
      return pStrInfo_base_QuestionSaveChgs;
    }
    cliSyntaxBottom(ewsContext);
    break;

 default:
   if(checkConfigScriptFiletype(argv[index + 1]) != L7_SUCCESS ||
   checkConfigScriptPresent(argv[index + 1]) != L7_SUCCESS)
   {
     ewsTelnetWrite(ewsContext, getLastError());
     ewsContext->unbufferedWrite = L7_FALSE;
      return cliSyntaxReturnPrompt (ewsContext, "");
   }

    cliSetCharInputID(ApplyScriptQuestion, ewsContext, argv);

    /* Are you sure you want to apply the configuration script? (y/n) */
    return pStrInfo_base_VerifyCfgScriptApplyYesNo;
  }

 /***Get the File data structure**/
  if(readConfigScriptData(argv[index+1], &configScriptData) == L7_SUCCESS)
  {
    /***Set the Flag to Execute***/
    ewsContext->scriptActionFlag = L7_EXECUTE_SCRIPT;

    cliExecuteBuffer(ewsContext, &configScriptData);

    /*release buffer*/
    releaseScriptBuffer(&configScriptData) ;

    if(ewsContext->actionFunctionStatusFlag == L7_ACTION_FUNC_SUCCESS)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, tempStr, sizeof(tempStr), pStrInfo_base_CfgScriptApplied, argv[index+1]);
    }
    else
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, tempStr, sizeof(tempStr), pStrWarn_base_ExecutionOfCfgScriptCantBeCompletedWarningRunningCfgMayNotBeDesiredCfgYouMightWantToReloadSavedCfg, argv[index+1]);
    }

    ewsTelnetWrite(ewsContext, tempStr);
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, getLastError());

  }

   /****Setting the flags to default for proper execution of CLI command independently**/
   ewsContext->scriptActionFlag=L7_EXECUTE_SCRIPT;
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
   ewsContext->configScriptStateFlag = L7_CONFIG_SCRIPT_NOT_RUNNING;

   ewsContext->unbufferedWrite = L7_FALSE;
   cliSyntaxBottom(ewsContext);
   return cliSetPriviledgeUserMode();
}

/*********************************************************************
*
* @purpose  Command to delete configuration script(s)
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  script delete {all | <scriptname>}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigScriptDelete(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
    L7_uint32 rc, numArgs, num_files, i, ignoredFiles;
    L7_char8 strFileName[L7_MAX_FILENAME];
    SYSAPI_DIRECTORY_t  tree[SYSAPI_MAX_FILE_ENTRIES];
    L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];

    cliSyntaxTop(ewsContext);

    /*Initialise*/
    num_files = 0;
    ignoredFiles = 0;
    i=0;

    numArgs = cliNumFunctionArgsGet();
    if(numArgs != 1)
    {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgScriptDel_1);
    }

    rc = L7_FAILURE;

    if ( cliGetCharInputID() != CLI_INPUT_EMPTY )
    {
      if ( tolower(cliGetCharInput()) == 'y' )
      {
      OSAPI_STRNCPY_SAFE(strFileName, argv[index + 1]);

      if (strcmp(strFileName, pStrInfo_common_All) == 0)
        {
            if ( sysapiTreeMalloc(tree) == L7_SUCCESS )
            {
                num_files = getConfigScriptList(tree);

                rc = L7_SUCCESS;

                for(i =0; i < num_files; i++)
                {

                  if(strcmp(tree[i].filename, TEMP_CONFIG_SCRIPT_FILE_NAME) == 0 ||
                     strcmp(tree[i].filename, TEMP_RUNNINGCONFIG_SCRIPT_FILE_NAME) == 0)
                  {
                    ignoredFiles++;
                  }
                  else
                  {
                    rc = osapiFsDeleteFile (tree[i].filename);
              if(rc != L7_SUCCESS)
              {
                cliSyntaxBottom(ewsContext);
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_base_DeletingAllCfgScripts);
                        break;
                    }
                  }
                }

                sysapiTreeFree(tree);
            }

            if(rc != L7_SUCCESS)
            {
                cliSyntaxBottom(ewsContext);
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_CfgScriptDel_2);
            }
        }
        else
        {
            if(checkConfigScriptFiletype(argv[index + 1]) == L7_SUCCESS ||
               checkConfigScriptPresent(argv[index + 1]) == L7_SUCCESS)
            {
                if( osapiFsDeleteFile (strFileName) == L7_SUCCESS)
                {
                    i = 1;
                    ignoredFiles = 0;
                    rc = L7_SUCCESS;
                }
            }
            else
            {
          return cliSyntaxReturnPrompt (ewsContext, getLastError());
        }
        }

        /*print number of files deleted*/
      ewsTelnetPrintf (ewsContext, pStrInfo_base_CfgScriptSDeld, i - ignoredFiles);
        cliSyntaxBottom(ewsContext);
      }
      else
      {
        /* no */
      ewsTelnetWrite(ewsContext, pStrInfo_base_CfgScriptDeletionCanceled);      /*Configuration  script Not deleted!  " */
        cliSyntaxBottom(ewsContext);
      }

      return cliPrompt(ewsContext);
    }
    else
    {
    if (strcmp(argv[index + 1], pStrInfo_common_All) != 0)
        {
            if(checkConfigScriptFiletype(argv[index + 1]) != L7_SUCCESS ||
               checkConfigScriptPresent(argv[index + 1]) != L7_SUCCESS)
            {
        return cliSyntaxReturnPrompt (ewsContext, getLastError());
            }
        }

        cliSetCharInputID(1, ewsContext, argv);
    osapiSnprintf(stat, sizeof(stat), "%s %s",
                  pStrErr_base_CfgScriptDelCmd, argv[index+1]);
        cliAlternateCommandSet(stat);            /*  config script delete */

    return pStrInfo_base_VerifyCfgScriptDelYesNo;                 /* Are you sure you want to delete the configuration script(s)? (y/n) */
    }
}

/*********************************************************************
*
* @purpose  Command to list configuration scripts
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  script list
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigScriptList(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 numArgs, num_files, i;
  SYSAPI_DIRECTORY_t  tree[SYSAPI_MAX_FILE_ENTRIES];
  L7_char8 strFormatedMessage[128];
  L7_uint32 tempFile = 0;
  L7_uint32 totalFileSize = 0;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  if(numArgs != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgScriptList_1);
  }

  num_files = 0;
  totalFileSize = 0;

  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_CfgScriptNameSizeBytes);
  ewsTelnetWrite(ewsContext,"\r\n-------------------------------- -----------");

  if ( sysapiTreeMalloc(tree) == L7_SUCCESS )
  {
      num_files = getConfigScriptList(tree);

      for(i =0; i < num_files; i++)
      {
        if(strcmp(tree[i].filename, TEMP_CONFIG_SCRIPT_FILE_NAME) == 0 ||
           strcmp(tree[i].filename, TEMP_RUNNINGCONFIG_SCRIPT_FILE_NAME) == 0)
        {
           tempFile++ ;
           continue;
        }

      ewsTelnetPrintf (ewsContext, "\r\n%-32s %11d", tree[i].filename, tree[i].filesize);
        totalFileSize += tree[i].filesize;
      }

    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, strFormatedMessage, sizeof(strFormatedMessage), pStrInfo_base_CfgScriptSFound, (num_files - tempFile));

      cliSyntaxBottom(ewsContext);
      ewsTelnetWrite(ewsContext, strFormatedMessage);

    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, strFormatedMessage, sizeof(strFormatedMessage), pStrInfo_base_KbytesFree, (L7_uint32) ((CONFIG_SCRIPTS_MAX_SIZE_LIMIT - totalFileSize)/1024));
      ewsTelnetWrite(ewsContext, strFormatedMessage);
      cliSyntaxBottom(ewsContext);
  }
  else
  {
    cliSyntaxBottom(ewsContext);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_CfgScriptList_2);
    cliSyntaxBottom(ewsContext);
  }

  sysapiTreeFree(tree);

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Command to show configuration script
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  script show <scriptname>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandConfigScriptShow(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
    L7_uint32 numArgs;
    static L7_uint32 i;
    L7_uint32 count = 0;
    static L7_ConfigScript_t configScriptData;
  L7_uchar8 * scriptCommand;
    L7_char8 strFileName[L7_MAX_FILENAME];
    L7_char8 strFormatedCommand[CONFIG_SCRIPT_MAX_COMMAND_SIZE];
    L7_char8 altCmdStr[CONFIG_SCRIPT_MAX_COMMAND_SIZE];

    cliCmdScrollSet( L7_FALSE);

  osapiSnprintf(altCmdStr, sizeof(altCmdStr), "%s %s", pStrErr_base_CfgScriptShowCmd, argv[index+1]);
    cliAlternateCommandSet(altCmdStr);            /*  config script apply */

    numArgs = cliNumFunctionArgsGet();

    if(numArgs != 1)
    {
      cliSyntaxTop(ewsContext);
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgScriptShow_1);
    }

    if ( cliGetCharInputID() != CLI_INPUT_EMPTY )
    {                                                                     /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        releaseScriptBuffer(&configScriptData) ;
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      i = 0;
      cliSyntaxTop(ewsContext);
    OSAPI_STRNCPY_SAFE(strFileName, argv[index+ 1]);

      if(readConfigScriptData(strFileName, &configScriptData) != L7_SUCCESS)
      {
      return cliSyntaxReturnPrompt (ewsContext, getLastError());
      }
    }

    while(getConfigScriptCommand(i + 1, &configScriptData, &scriptCommand) == L7_SUCCESS)
    {
    osapiSnprintf(strFormatedCommand, sizeof(strFormatedCommand), "%d : %s", i+1, scriptCommand);
      cliSyntaxBottom(ewsContext);
      ewsTelnetWrite( ewsContext, strFormatedCommand);

      i++;
      count++;
      if (( count == CLI_MAX_SCROLL_LINES - 3)
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
           || (strstr(scriptCommand,pStrInfo_security_CP_ENCODED_HEADER4) != NULL)
#endif
         )
      {
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliSyntaxBottom(ewsContext);
      return pStrInfo_common_Name_2;     /* --More-- or (q)uit */
      }
    }

    releaseScriptBuffer(&configScriptData) ;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Validates the configuration script
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  script validate <scriptname>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandConfigScriptValidate(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 numArgs;
   L7_ConfigScript_t configScriptData;
   L7_char8 tempStr[CONFIG_SCRIPT_MAX_COMMAND_SIZE];

   cliSyntaxTop(ewsContext);

   ewsContext->unbufferedWrite = L7_TRUE;
   numArgs = cliNumFunctionArgsGet();

  if(numArgs != 1)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgScriptValidate);
    cliSyntaxBottom(ewsContext);
    ewsContext->unbufferedWrite = L7_FALSE;
    return cliPrompt(ewsContext);
  }

  /***Get the File data structure**/
  if(readConfigScriptData(argv[index+1], &configScriptData) == L7_SUCCESS)
  {
    /***Set the Flag to Execute***/
    ewsContext->scriptActionFlag = L7_VALIDATE_SCRIPT;

    cliExecuteBuffer(ewsContext, &configScriptData);

    if(ewsContext->actionFunctionStatusFlag == L7_ACTION_FUNC_SUCCESS)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, tempStr, sizeof(tempStr), pStrInfo_base_CfgScriptValidated, argv[index+1]);
      ewsTelnetWrite( ewsContext, tempStr);
      cliSyntaxBottom(ewsContext);
    }
    else
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, tempStr, sizeof(tempStr), pStrInfo_base_CfgScriptValidationFailed, argv[index+1]);
      ewsTelnetWrite( ewsContext, tempStr);
      cliSyntaxBottom(ewsContext);
    }

    /*release buffer*/
    releaseScriptBuffer(&configScriptData) ;
  }
  else
  {
    cliSyntaxBottom(ewsContext);
    ewsTelnetWrite( ewsContext, getLastError());
    cliSyntaxBottom(ewsContext);
  }

  /****Setting the flags to default for proper execution of CLI command independently**/
  ewsContext->scriptActionFlag=L7_EXECUTE_SCRIPT;
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  ewsContext->configScriptStateFlag = L7_CONFIG_SCRIPT_NOT_RUNNING;

  ewsContext->unbufferedWrite = L7_FALSE;

  return cliSetPriviledgeUserMode();
}

/*********************************************************************
*
* @purpose  Validates and creates a downloaded configuration script
*
*
* @param EwsContext ewsContext
* @param L7_char8 *scriptFileName  name of script file to validate and download
* @param L7_char8 *newFilename     name of file which will involve downloaded script
*
* @returntype L7_RC_t
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_RC_t configScriptValidateAndDownload(EwsContext ewsContext, L7_char8 * scriptFileName, L7_char8 * newFilename, L7_uint32 argc)
{

    L7_uint32 rc;
    L7_ConfigScript_t configScriptData;
    L7_uint32 i = 0;
    L7_uchar8 * commandStr;
    L7_char8 strMessage[CONFIG_SCRIPT_MAX_COMMAND_SIZE * 2];

    rc = L7_FAILURE;

    if(checkConfigScriptFiletype(newFilename) != L7_SUCCESS)
    {
        cliSyntaxBottom(ewsContext);
        ewsTelnetWrite(ewsContext, getLastError());
        cliSyntaxBottom(ewsContext);
        return L7_NOT_SUPPORTED;
    }


    if(argc!=5)
    {
      ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ValidatingCfgScript);
    }


    if(readDownloadedConfigScript(scriptFileName, newFilename, &configScriptData) == L7_SUCCESS)
    {
      OSAPI_STRNCPY_SAFE(configScriptData.cfgHdr.filename, newFilename);

   

      if(argc!=5) 
      {
        rc = configScriptValidate(ewsContext, &configScriptData);
      }
      else
      {
        rc = L7_SUCCESS;
      }

      if (rc == L7_SUCCESS) 
      {
        configScriptData.cfgHdr.dataChanged = L7_TRUE;

        if(writeConfigScriptData(&configScriptData) == L7_SUCCESS)
        {
          rc = L7_SUCCESS;
        }
        else
        {
          ewsTelnetWrite(ewsContext, getLastError());
          rc = L7_ERROR;
        }
      }
      else
      {
        cliCmdScrollSet(L7_FALSE);

        /*display all the error lines*/
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_FollowingLinesInScriptMayHaveProblem);

        /*loop through buffer to display error lines*/
        while(validationErrorLines[i] > 0)
        {
          if(getConfigScriptCommand(validationErrorLines[i], &configScriptData, &commandStr) != L7_SUCCESS)
          {
             break;
          }

          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, strMessage, sizeof(strMessage), pStrInfo_base_Line_1, validationErrorLines[i], commandStr);
          ewsTelnetWrite(ewsContext, strMessage);

          i++;
        }

        osapiSnprintfAddBlanks (2, 0, 0, 0, L7_NULLPTR, strMessage, sizeof(strMessage), pStrInfo_base_TotalErrorLines, i);
        ewsTelnetWrite(ewsContext, strMessage);
      }
      osapiFsDeleteFile (scriptFileName);
      /*release buffer*/
      releaseScriptBuffer(&configScriptData) ;
    }
    else
    {
        osapiFsDeleteFile (scriptFileName);
        cliSyntaxBottom(ewsContext);
        ewsTelnetWrite( ewsContext, getLastError());
        cliSyntaxBottom(ewsContext);
        rc = L7_ERROR;
    }

    cliSetPriviledgeUserMode();

    return rc;
}

/*********************************************************************
*
* @purpose  Reads the downloaded configuration script into buffer
*
*
* @param const L7_char8 *filename
* @param const L7_char8 *newFilename
* @param L7_ConfigScript_t *configScriptData
*
* @returntype L7_RC_t
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_RC_t readDownloadedConfigScript(const L7_char8 * filename, const L7_char8 * newFilename, L7_ConfigScript_t * configScriptData)
{
    L7_RC_t       rc = L7_FAILURE;
    L7_int32      filedesc;
    L7_int32      i, ctr=0;
  L7_uchar8 * readConfigScript;
    L7_uchar8     tempBuffer[CONFIG_SCRIPT_MAX_COMMAND_SIZE * 2];
    L7_uint32     size;
  L7_char8 * ptok, * pnexttok;
    L7_char8      strFileName[L7_MAX_FILENAME];
    L7_char8      errMsg[128];
    L7_int32      charsRead = 0;
    L7_char8      strNewFileName[L7_MAX_FILENAME];

  /*Initialise*/
  memset (tempBuffer, 0, sizeof(tempBuffer));
  OSAPI_STRNCPY_SAFE(strFileName, filename);
  OSAPI_STRNCPY_SAFE(strNewFileName, newFilename);

    if(checkConfigScriptFiletype(filename) != L7_SUCCESS ||
       checkConfigScriptPresent(filename) != L7_SUCCESS)
    {
    osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg), pStrInfo_base_DownloadedFileNotFound);
      setLastError(errMsg);
      return L7_FAILURE;
    }

    if(initialiseScriptBuffer(strNewFileName, configScriptData) == L7_SUCCESS)
    {
      if(getConfigScriptsSize(strNewFileName, &size) == L7_SUCCESS)
      {
        configScriptData->size += size;
        size = 0;
      }
      else
      {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot, errMsg, sizeof(errMsg), pStrInfo_base_GetSizeOfExistingScripts);
        setLastError(errMsg);
        releaseScriptBuffer(configScriptData);
        return L7_FAILURE;
      }

      if(osapiFsFileSizeGet(strFileName, &size) != L7_SUCCESS)
      {
         osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg), pStrInfo_base_InReadingFileSize, strFileName);
         setLastError(errMsg);
         return L7_FAILURE;
      }

      ctr = CONFIG_SCRIPT_MAX_COMMAND_SIZE;

      /*allocate memory as per counter size of file*/
      readConfigScript = osapiMalloc(L7_CLI_WEB_COMPONENT_ID, ctr+1);

      i = size/(CONFIG_SCRIPT_MAX_COMMAND_SIZE-1) + 1;

      if(readConfigScript != L7_NULL)
      {
        /* open config script file */
        if (osapiFsOpen (strFileName, &filedesc) == L7_ERROR)
        {
           osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg), pStrInfo_base_InOpeningFile, strFileName);
           setLastError(errMsg);
           return L7_ERROR;
        }
   

        while (i >= 0)
        {
          i--;

        memset (readConfigScript, 0,ctr+1);
          /*second call to read couter size file*/
          if (osapiFileRead(filedesc, (L7_char8 *)readConfigScript, ctr - 1) != L7_ERROR)
          {
            charsRead = strlen(readConfigScript);
            if(charsRead == 0)
            {
              break;
            }

            if(readConfigScript[0] == '\n')
            {
            if(strlen((char *)tempBuffer) > 0)
              {
              if(setNextConfigScriptCommand((char *)tempBuffer, configScriptData) == L7_FAILURE)
                {
                  osapiFsClose(filedesc);
                  /* free the memory */
                  osapiFree(L7_CLI_WEB_COMPONENT_ID, readConfigScript);
                  releaseScriptBuffer(configScriptData);
                  return L7_FAILURE;
                }

              memset (tempBuffer, 0, CONFIG_SCRIPT_MAX_COMMAND_SIZE);
              }
            }

            ptok = strtok ((L7_uchar8 *) readConfigScript, "\n");

            while (ptok != NULL)
            {
              /*Get next token*/
              pnexttok = strtok ((L7_char8 *)'\0', "\n");

              if( pnexttok == NULL)   /*check if current entry is last entry*/
              {
              if((readConfigScript[charsRead - 1] == '\0') ||
                 (readConfigScript[charsRead - 1] == '\n'))
                {
                  if(strlen(ptok) > 0)
                  {
                    if(setNextConfigScriptCommand(ptok, configScriptData) == L7_FAILURE)
                    {
                      osapiFsClose(filedesc);
                      /* free the memory */
                      osapiFree(L7_CLI_WEB_COMPONENT_ID, readConfigScript);
                      releaseScriptBuffer(configScriptData);

                      return L7_FAILURE;
                    }
                  }
                }
                else
                {
                memset (tempBuffer, 0, CONFIG_SCRIPT_MAX_COMMAND_SIZE);
                  memcpy(&tempBuffer,ptok,strlen(ptok));
                }
                break;
              }
              else
              {
                if(strlen(tempBuffer) > 0)
                {
                OSAPI_STRNCAT(tempBuffer, ptok);
                  if(setNextConfigScriptCommand(tempBuffer, configScriptData) == L7_FAILURE)
                  {
                    osapiFsClose(filedesc);
                    /* free the memory */
                    osapiFree(L7_CLI_WEB_COMPONENT_ID, readConfigScript);
                    releaseScriptBuffer(configScriptData);

                    return L7_FAILURE;
                  }
                memset (tempBuffer, 0, CONFIG_SCRIPT_MAX_COMMAND_SIZE);
                }
                else
                {
                  if(strlen(ptok) > 0)
                  {
                    if(setNextConfigScriptCommand(ptok, configScriptData) == L7_FAILURE)
                    {
                      osapiFsClose(filedesc);
                      /* free the memory */
                      osapiFree(L7_CLI_WEB_COMPONENT_ID, readConfigScript);
                      releaseScriptBuffer(configScriptData);

                      return L7_FAILURE;
                    }
                  }
                }
              }

              ptok = pnexttok;
            }
          }
          else
          {
          osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg), pStrInfo_base_InReadingDownloadedFile);
            setLastError(errMsg);
            osapiFsClose(filedesc);
            /* free the memory */
            osapiFree(L7_CLI_WEB_COMPONENT_ID, readConfigScript);
            releaseScriptBuffer(configScriptData);

            return L7_FAILURE;
          }
        }  /*end of while */
      }
      else
      {
      setLastErrorAddBlanks (1, 0, 0, 0, pStrErr_common_Error, pStrInfo_base_InGettingMemory);
        return L7_FAILURE;
      }

      /* free the memory */
      osapiFree(L7_CLI_WEB_COMPONENT_ID, readConfigScript);

      osapiFsClose(filedesc);

      rc = L7_SUCCESS;
    }
    else
    {
    setLastErrorAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot, pStrInfo_base_InitialiseScriptingBuf);
      rc =  L7_FAILURE;
    }

  return rc;
}

/*********************************************************************
*
* @purpose  Validates the configuration script
*
* @param EwsContext ewsContext
* @param L7_ConfigScript_t *configScriptData
*
* @returntype L7_RC_t
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t configScriptValidate(EwsContext ewsContext, L7_ConfigScript_t * configScriptData)
{
    L7_RC_t rc = L7_FAILURE;

    /***Set the Flag to Validate***/
    ewsContext->scriptActionFlag = L7_VALIDATE_SCRIPT;

    cliExecuteBuffer(ewsContext, configScriptData);

    if(ewsContext->actionFunctionStatusFlag == L7_ACTION_FUNC_SUCCESS)
    {
    ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_CfgScriptValidated_1);
        rc = L7_SUCCESS;
    }
    else
    {
    ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_CfgScriptValidationFailed_1);
    }

    /****Setting the flags to default for proper execution of CLI command independently**/
    ewsContext->scriptActionFlag=L7_EXECUTE_SCRIPT;
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    ewsContext->configScriptStateFlag = L7_CONFIG_SCRIPT_NOT_RUNNING;

    return rc;

}

/*********************************************************************
*
* @purpose  Function to loop throurh the script buffer
*
*
* @param EwsContext ewsContext
* @param L7_ConfigScript_t *configScriptData
*
* @returntype L7_RC_t
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
*
* @end
*
*********************************************************************/

L7_RC_t cliExecuteBuffer(EwsContext ewsContext, L7_ConfigScript_t * configScriptData)
{
  L7_uint32 i = 0;
  L7_char8 strMessage[CONFIG_SCRIPT_MAX_COMMAND_SIZE * 2];
  L7_uchar8 * commandStr;
  boolean validationFailed = L7_FALSE;
  L7_char8 displayStr[CONFIG_SCRIPT_MAX_COMMAND_SIZE];
  L7_uint32 j = 0, len;
   
   ewsContext->configScriptStateFlag = L7_CONFIG_SCRIPT_RUNNING;

   resetValidationErrorLines();

   /*loop through buffer to execute buffer*/
   while(getConfigScriptCommand(i+1, configScriptData, &commandStr) == L7_SUCCESS)
   {
     if (cliGetStringPassword() != L7_TRUE)
     {
       commandStr = trimLeft(commandStr);
     }

     len = strlen(commandStr);
     if(len > 0)
     {
            if (cliGetStringPassword() == L7_TRUE)
            {
                /* Remove extra space from the end. Due to this password check fails */
                if(commandStr[len-1] == ' ')
                    commandStr[len-1] = '\0';
            }

            /*reset flag*/
            ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

            /*ignore comment and show commands*/
            if(commandStr[0] != '!' && !(strncmp(commandStr,pStrInfo_base_Show_1,4)==0 && commandStr[4] == ' ' ))
            {
                L7_uint32 ct;
                L7_BOOL validStrFlg = L7_TRUE;
                
                if(strchr(commandStr, '!') != NULL)  
                {
                  L7_BOOL startQuotFlg = L7_FALSE,
                          commentCharFlg = L7_FALSE,
                          endQuotFlg = L7_FALSE,
                          errorFlg = L7_FALSE;

                  for(ct=0; ct<len; ct++)
                  {
                    if((commandStr[ct] == '"') && (startQuotFlg == L7_FALSE))
                    {
                      startQuotFlg = L7_TRUE;
                    }
                    else if((commandStr[ct] == '!') && (startQuotFlg == L7_TRUE))
                    {
                      commentCharFlg = L7_TRUE;
                    }
                    else if((commandStr[ct] == '"') && (commentCharFlg == L7_TRUE))
                    {
                      endQuotFlg = L7_TRUE; 
                      if((ct+1 < len) && (commandStr[ct+1] != ' '))
                      {
                        errorFlg = L7_TRUE;
                      }
                    }  
                  }
                  
                  if(!((startQuotFlg == L7_TRUE) && 
                       (commentCharFlg == L7_TRUE) && 
                       (endQuotFlg == L7_TRUE) && 
                       (errorFlg == L7_FALSE)))
                  {
                    validStrFlg = L7_FALSE;
                  }       
                }  
                
                if(validStrFlg == L7_TRUE)
                {
                    if(strchr(commandStr, '?') == NULL)
                    {
                        cliProcessCharInput(commandStr); /**Process the inputted chars**/

                        for ( j = 0; j < len;j++)
                        {
                          cliProcessStringInput(commandStr+j);
                        }

                        cliCmdScrollSet(L7_FALSE);
                        if ( cliGetStringPassword() == L7_TRUE)
                        {
                          for ( j = 0; j < len; j++)
                          {
                            displayStr[j] = '*';
                          }
                          ewsTelnetWrite( ewsContext, displayStr);
                        }
                        else
                        {
                          cliSyntaxBottom(ewsContext);
                          ewsTelnetWrite( ewsContext, commandStr);
                        }
                        ewsCliDataConfigScript(ewsContext, commandStr, mainMenu[cliCurrentHandleGet()]);
                    }
                    else
                    {
                      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, strMessage, sizeof(strMessage), pStrInfo_base_FoundCharWithinCmd);
                      ewsTelnetWrite( ewsContext, strMessage);
                      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
                    }
                }
                else
                {
                  osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, strMessage, sizeof(strMessage), 
                                              pStrInfo_base_FoundCommentCharWithinCmd);
                  ewsTelnetWrite( ewsContext, strMessage);
                  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
                }
            }

            /*check for previous action function failure or success to go ahead with the next function*/
            if(ewsContext->actionFunctionStatusFlag != L7_ACTION_FUNC_SUCCESS)
            {
               if(ewsContext->scriptActionFlag != L7_VALIDATE_SCRIPT)
               {
                 osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, strMessage, sizeof(strMessage), 
                                          pStrInfo_base_InCfgScriptFileAtLineNumCliCmdAbortingScript, i+1, commandStr);
                 ewsTelnetWrite( ewsContext, strMessage);
                 break;
               }
               else
               {
                 /*store the info to be used later to print error info*/
                 addValidationErrorLine(i + 1);
                 validationFailed = L7_TRUE;
               }
            }
       } 
       i++;
   }

   ewsContext->configScriptStateFlag = L7_CONFIG_SCRIPT_NOT_RUNNING;

   /* Clean up CLI input state. Its possible error in script would have messed
    * up the state ex: "config\nuser name user123\nuser pass user123\nabc\n"
    */
   cliClearStringInput();

   if(ewsContext->scriptActionFlag == L7_VALIDATE_SCRIPT && validationFailed == L7_TRUE)
   {
     ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
     return L7_FAILURE;
   }
   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Function to identify the commands to skip if previous mode
*           command is failed
*********************************************************************/
static L7_BOOL ewsCliCheckSkipCommand(EwsContext context, L7_int32 * skipExit,
                                      L7_BOOL isExit, EwsCliState csp)
{
  L7_uint32 mode;

  /* return if we don't care */
  if(skipExit == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  /* check if we ust need to update the skip exit -
     as in case of comming here after running the command */
  
   /* If we want to move to wireless config, the parent node should be global config.
      The reason for testing this condition is we are accepting wireless mode through 
      hidden commands in non wireless builds to support text based configuration.There 
      is chance it can enter from any mode just like devshell command. So we have to 
      avoid such occurences.We should allow the wireless mode through global config only.*/ 
     
  if(context != L7_NULLPTR && context->actionFunctionStatusFlag != L7_ACTION_FUNC_SUCCESS)
  {
    mode = csp->parseList[csp->parseCount-1]->mode;
    if(mode != 0)
    { 
      if(mode == L7_WIRELESS_CONFIG_MODE)
      {
        if(cliGetCurrentMode() == L7_GLOBAL_CONFIG_MODE)
        {
          *skipExit = *skipExit + 1;
          cliChangeMode(mode);
        }  
      }
      else
      {  
        *skipExit = *skipExit + 1;
        cliChangeMode(mode);
      } 
    }
    return L7_FALSE;
  }

  /* no error yet - allow commands */
  if(*skipExit <= 0)
  {
    return L7_FALSE;
  }

  /* allow the exit to get executed */
  if(isExit == L7_TRUE)
  {
    *skipExit = *skipExit - 1;
    return L7_FALSE;
  }

  mode = csp->parseList[csp->parseCount-1]->mode;
  if(mode != 0)
  {
    if(mode == L7_WIRELESS_CONFIG_MODE)
    {
      if(cliGetCurrentMode() == L7_GLOBAL_CONFIG_MODE)
      {
        *skipExit = *skipExit + 1;
         cliChangeMode(mode);
      }
    }
    else
    {
      *skipExit = *skipExit + 1;
      cliChangeMode(mode);
    } 
  }
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Function to parse the script command and
*           invoke action function in a particular mode
*********************************************************************/
static L7_RC_t ewsCliDataConfigScriptMode (EwsContext context, EwsCliState csp,
                                           EwsCliCommandP menu, L7_int32 * skipExitp)
{
  L7_BOOL isExit = L7_FALSE;
  sintf marker, i;
  uintf tokens;
  boolean ambiguous;
  EwsCliCommandP mp = NULL;
  L7_char8 lastToken[L7_CLI_MAX_COMMAND_LENGTH];
  L7_char8 dataTypeErrorMessage[L7_MAX_DATA_TYPE_ERROR_MESSAGE];

  ambiguous = ewsCliParseTree (context, csp, menu, &marker, &tokens, dataTypeErrorMessage);
  if (((ambiguous == FALSE && marker >= 0) || ambiguous == TRUE))
  {
    OSAPI_STRNCPY_SAFE (lastToken, csp->argv[csp->argc - 1]);
    if ((strcmp (lastToken, pStrInfo_common_Lvl7Clr) != 0) &&
        (strcmp (lastToken, pStrInfo_base_Lvl7Hidden) != 0) &&
        (strcmp (lastToken, pStrInfo_common_Lvl7TraceEnbl) != 0) &&
        (strcmp (lastToken, pStrInfo_common_Lvl7TraceDsbl) != 0))
    {
      return (ambiguous == TRUE) ? CLI_CONFIG_CMD_AMBIGIOUS : CLI_CONFIG_CMD_NOT_FOUND;
    }
  }

  /* get the last action in the path */
  for (i = csp->parseCount-1; i >= 0; i--)
  {
    if (csp->parseList[i]->action != NULL)
    {
      mp = csp->parseList[i];
      break;
    }
  }

  if (mp == L7_NULL && strncmp (csp->argv[0], pStrInfo_common_Exit, 4) == 0)
  {
    isExit = L7_TRUE;
    mp = menu;
  }

  if (mp == L7_NULL)
  {
    return CLI_CONFIG_CMD_INVALID;
  }

  if(L7_TRUE == ewsCliCheckSkipCommand(L7_NULLPTR, skipExitp, isExit, csp))
  {
    return CLI_CONFIG_CMD_SKIPPED;
  }

  cliExamine (context, csp->argc, (const L7_char8 * *) csp->argv, i);
  mp->action (context, csp->argc, (const L7_char8 * *) csp->argv, i);

  ewsCliCheckSkipCommand(context, skipExitp, L7_FALSE, csp);

  return CLI_CONFIG_CMD_SUCCESS;
}

/*********************************************************************
* @purpose  Function to return global cli state after initialising
*********************************************************************/
static EwsCliState ewsCliGetGlobalState (EwsContext context, L7_char8 * datap)
{
  static struct EwsCliState_s cliScriptState;

   cliScriptState.historyTop      = 0;              /* top of history */
   cliScriptState.historyBottom   = 1;              /* bottom of history */
   cliScriptState.historyIndex    = 0;              /* current line in history */
   cliScriptState.historySearch   = 1;              /* current history search */
   cliScriptState.parseCount      = 0;              /* number of nodes matched */
   cliScriptState.depth           = 0;              /* depth of pre-defined arguments */
   cliScriptState.depth_buffer[0] = '\0';           /* storage for depth arguments */
  cliScriptState.deletedBuffer[0] = ' ';        /* storage for deleted chars */
  cliScriptState.deletedBuffer[1] = '\0';       /* storage for deleted chars */
   cliScriptState.arrow_esc       = L7_FALSE;
   cliScriptState.arrow_lb        = L7_FALSE;
   cliScriptState.argc            = 0;              /* number of arguments */

  OSAPI_STRNCPY_SAFE (cliScriptState.line[0], datap);   /* history buffer */
  cliScriptState.lineIndex = strlen (datap);    /* history buffer length */
  cliScriptState.arg_buffer[0] = '\0';        /* parse buffer */
  memset(cliScriptState.parseList, 0, sizeof(cliScriptState.parseList));
  ewsCliParseLine (context, &cliScriptState);
  return &cliScriptState;
}

/*********************************************************************
*
* @purpose  Function to parse the script command and invoke action function
*           - also searches for the commands in hidden tree
*
* @param EwsContext ewsContext
* @param char *datap
* @param EwsCliCommandP menu
*
* @returntype L7_RC_t
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t ewsCliDataConfigScript (EwsContext context, L7_char8 * datap, EwsCliCommandP menu)
{
  L7_char8 buffer[256];
  EwsCliState csp;
  L7_CLI_CFG_ERRTYPE_t errType1 = CLI_CONFIG_CMD_SUCCESS;
  L7_CLI_CFG_ERRTYPE_t errType2 = CLI_CONFIG_CMD_SUCCESS;

  csp = ewsCliGetGlobalState (context, datap);
  errType1 = ewsCliDataConfigScriptMode (context, csp, menu, L7_NULLPTR);
  if (errType1 != CLI_CONFIG_CMD_SUCCESS)
  {
    errType2 = ewsCliDataConfigScriptMode (context, csp, cliGetHiddenNode (), L7_NULLPTR);
  }

  if (errType1 == CLI_CONFIG_CMD_SUCCESS || errType2 == CLI_CONFIG_CMD_SUCCESS)
  {
    context->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return L7_SUCCESS;
  }

  switch (errType1)
  {
  case CLI_CONFIG_CMD_INVALID:
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buffer, sizeof (buffer), pStrInfo_base_CmdNotFoundIncompleteCmd,
                            datap);
    break;
  case CLI_CONFIG_CMD_AMBIGIOUS:
    osapiSnprintf (buffer, sizeof (buffer), "\r\n%s %s", EWS_CLI_AMBIGUOUS_CMD, datap);
    break;
  case CLI_CONFIG_CMD_NOT_FOUND:
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buffer, sizeof (buffer), pStrInfo_base_UnrecognizedCmd, datap);
    break;
  default:
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buffer, sizeof (buffer), pStrInfo_base_UnkownErrorExecuting, datap);
    break;
  }

  cliSyntaxBottom (context);
  ewsTelnetWrite (context, buffer);
  cliSyntaxBottom (context);
  context->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  return L7_ERROR;
}

/*********************************************************************
*
* @purpose  Function to parse the script command and invoke action function
*           - Without displaying the errors
*           - smoothly handling the failed commands
*           - also searches for the commands in maintainance tree
*
* @param EwsContext ewsContext
* @param char *datap
* @param EwsCliCommandP menu
*
* @returntype L7_RC_t
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t ewsCliDataConfigScript2 (EwsContext context, L7_char8 * datap,
                                 EwsCliCommandP menu, L7_int32 * skipExitp)
{
  EwsCliState csp;
  L7_CLI_CFG_ERRTYPE_t errType1 = CLI_CONFIG_CMD_SUCCESS;
  L7_CLI_CFG_ERRTYPE_t errType2 = CLI_CONFIG_CMD_SUCCESS;
  L7_CLI_CFG_ERRTYPE_t errType3 = CLI_CONFIG_CMD_SUCCESS;
  L7_int32 envVarFlag = L7_FALSE;
  L7_char8 *tempPtr = L7_NULL;

  if(pwRecoveryFlag != L7_TRUE)
  {
    if(passwdRecoveryFlagGet(&envVarFlag) == L7_SUCCESS)
    {
      if(envVarFlag != L7_FALSE)
      {
        if(strncmp(datap,"users passwd \"admin\"",20) == L7_NULL)
        {
          tempPtr = strstr(datap,"encrypted");
          if(tempPtr != L7_NULL)
          {
            osapiSnprintf(datap, sizeof(datap),pStrInfo_base_UsrsPasswdEncrypted, FD_DEFAULT_PASSWORD);
            pwRecoveryFlag = L7_TRUE;
            printf("\r\nBox is in recovery mode;Please configure password for admin\r\n");
            if(passwdRecoveryFlagSet(L7_FALSE) != L7_SUCCESS)
            {
              printf("\r\n Failed to write the passwd flag to env file\r\n");
            }
          }
        }
        if (strncmp(datap,"enable passwd",13) == L7_NULL)
        {
          tempPtr = strstr(datap,"encrypted");
          if(tempPtr != L7_NULL)
          {
            osapiSnprintf(datap, sizeof(datap),pStrInfo_base_UsrsPasswdEncrypted, FD_DEFAULT_PASSWORD);
          }
          printf("\r\nBox is in recovery mode;please configure password for enable mode\r\n");
        }
      }
      else
      {
    pwRecoveryFlag = L7_TRUE;
        if(passwdRecoveryFlagSet(L7_FALSE) != L7_SUCCESS)
        {
          printf("\r\n Failed to write the passwd flag to env file\r\n");
        } 
      }
    }
 /* Set the pwRecoveryFlag to TRUE so that this logic wont get executed for next time */
    else
    {
      pwRecoveryFlag = L7_TRUE; 
    }
  }

  csp = ewsCliGetGlobalState (context, datap);
  csp->dataCheckDisable = L7_TRUE;
  errType1 = ewsCliDataConfigScriptMode (context, csp, menu, skipExitp);
  if (errType1 != CLI_CONFIG_CMD_SUCCESS)
  {
    errType2 = ewsCliDataConfigScriptMode (context, csp, cliGetMaintenanceNode (), skipExitp);
  }

  if (errType1 != CLI_CONFIG_CMD_SUCCESS && (errType2 != CLI_CONFIG_CMD_SUCCESS ))
  {
    errType3 = ewsCliDataConfigScriptMode (context, csp,  cliGetHiddenNode(), skipExitp);
  }

  if (errType1 == CLI_CONFIG_CMD_SUCCESS || errType2 == CLI_CONFIG_CMD_SUCCESS || errType3 == CLI_CONFIG_CMD_SUCCESS)
  {
    return L7_SUCCESS;
  }

  return L7_ERROR;
}
