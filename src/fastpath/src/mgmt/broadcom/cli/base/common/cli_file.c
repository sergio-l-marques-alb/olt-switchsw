/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_file.c
 *
 * @purpose      Image/File manipulation commands for the cli
 *
 * @component    user interface
 *
 * @comments     none
 *
 * @create       03/06/2005
 *
 * @author       bviswanath
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "datatypes.h"

#include "usmdb_dim_api.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "transfer_exports.h"
#include "usmdb_slotmapper.h"
#include "usmdb_file_api.h"
#include "usmdb_util_api.h"
#include "usmdb_sim_api.h"
#include "clicommands_card.h"

#ifdef L7_STACKING_PACKAGE

#include "clicommands_stacking.h"
#include "usmdb_cda_api.h"

#endif

#include "usmdb_unitmgr_api.h"

#ifdef L7_AUTO_INSTALL_PACKAGE
  #include "usmdb_auto_install_api.h"
#endif /* L7_AUTO_INSTALL_PACKAGE */

/*********************************************************************
*
* @purpose  Command to activate an image
*
*
* @param EwsContext     ewsContext
* @param L7_uint32      argc
* @param const L7_char8 **argv
* @param L7_uint32      index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  boot system <image-file-name>
*
* @end
*
*********************************************************************/
const L7_char8 *commandBoot(EwsContext ewsContext, L7_uint32 argc,
                            const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 rc, unit;
  L7_char8 fileName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[100];
  L7_uint32 imageId;

  memset(fileName, 0, sizeof(fileName));
  /* validate the command */

  if (strcmp( argv[index+1], pStrInfo_base_BootSys ) == 0)
  {
    /* boot system < image-file-name > command */

#ifdef L7_STACKING_PACKAGE

    if(argc == 3)
    {
      /* all units */
      unit = L7_USMDB_CDA_CDA_ALL_UNITS;
    }
    else if(argc == 4)
    {
      /* unit number in argv[index+2] */
      OSAPI_STRNCPY_SAFE(buf, argv[index+2]);
      cliConvertTo32BitUnsignedInteger(buf, &unit);
      if (cliIsUnitPresent(unit) != L7_TRUE)
      {
        return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_BOOT);
      }

      index++;
    }
    else
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_BOOT);
    }
#else
    if (argc != 3)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_BOOT);
    }

    usmDbUnitMgrNumberGet(&unit);
#endif

    /* validate the local file name */

    if(strlen(argv[index+2]) > L7_MAX_FILENAME)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_base_FileNameLen, L7_MAX_FILENAME);

    }
    else if (strlen(argv[index+2]) == 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_BOOT);
    }
    if((usmDbImageFileNameValid((char *)argv[index+2], &imageId) != L7_TRUE) &&
      (usmDbImageNameValid((char *)argv[index+2]) != L7_TRUE))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_NoImage);
    }
    /* extract the file name
     */

    OSAPI_STRNCPY_SAFE(fileName, argv[index+2]);

    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_ActivatingImage, fileName);

    cliWrite(buf);
    

    /* handover the request to usmdb */

    rc = usmDbImageActivate(unit, fileName, L7_FALSE);

    if (rc == L7_IMAGE_IN_USE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_base_ImageInUse);
    }
    else if(rc == L7_NOT_EXIST)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_NoImage);
    }
    else if( rc == L7_REQUEST_DENIED)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidImage);
    }
    else if( rc == L7_ALREADY_CONFIGURED)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_ActivateDbAckUp);
    }
    else if (rc != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_FileNotFound);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");

  }

  return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_BOOT);

}

/*********************************************************************
*
* @purpose  Command to show the boot images
*
*
* @param EwsContext     ewsContext
* @param L7_uint32      argc
* @param const L7_char8 **argv
* @param L7_uint32      index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show boot
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowBoot(EwsContext ewsContext, L7_uint32 argc,
                                const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit,count;
  L7_char8 buf[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_char8 activeImage[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 activatedImage[L7_CLI_MAX_STRING_LENGTH];

#if L7_FEAT_DIM_USE_FILENAME
  L7_char8 image1Version[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 image2Version[L7_CLI_MAX_STRING_LENGTH];

  L7_char8 image1Descr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 image2Descr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 image1FileName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 image2FileName[L7_CLI_MAX_STRING_LENGTH];
#else
  L7_char8 activeName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 backupName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 activeVersion[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 backupVersion[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 activatedVersion[L7_CLI_MAX_STRING_LENGTH];

  L7_char8 activeDescr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 backupDescr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 activeFileName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 backupFileName[L7_CLI_MAX_STRING_LENGTH];
#endif

  static L7_uint32 unitAry[20];
  static L7_int32 numUnits = 1, j = 0;
  static L7_BOOL firstTime = L7_TRUE;

  L7_RC_t rc = L7_SUCCESS;

  cliCmdScrollSet( L7_FALSE);

  memset(activeImage, 0, sizeof(activeImage));
  memset(activatedImage, 0, sizeof(activatedImage));
  /* get switch ID based on presence/absence of STACKING package */
  usmDbUnitMgrNumberGet(&unit);

  /* validate the command */

  if (strcmp( argv[index], pStrInfo_base_BootVar ) != 0)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SHOWBOOT);
  }

  rc = L7_SUCCESS;

#ifdef L7_STACKING_PACKAGE

  if(argc == 2)
  {
    /* Get all units */

    rc = usmDbUnitMgrStackMemberGetFirst(&unit);
    numUnits = 0;

    while(rc == L7_SUCCESS)
    {
      unitAry[numUnits] = unit;
      numUnits++;
      rc = usmDbUnitMgrStackMemberGetNext(unit, &unit);
    }
  }
  else if(argc == 3)
  {
    /* unit number in argv[index+1] */
    OSAPI_STRNCPY_SAFE(buf, argv[index+1]);
    cliConvertTo32BitUnsignedInteger(buf, &unit);

    if (cliIsUnitPresent(unit) != L7_TRUE)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SHOWBOOT);
    }

    numUnits = 1;
    unitAry[0] = unit;

  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SHOWBOOT);
  }
#else
  if (argc != 2)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_BOOT);
  }

  usmDbUnitMgrNumberGet(&unit);

  numUnits = 1;
  unitAry[0] = unit;

#endif
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {   /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      j=0;
      numUnits=1;
      firstTime = L7_TRUE;
      memset(unitAry, 0,sizeof (unitAry[20]));
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }

#if L7_FEAT_DIM_USE_FILENAME
  usmDbImageFileNameGet(0, &image1FileName[0]);
  usmDbImageFileNameGet(1, &image2FileName[0]);
#else
  usmDbActiveNameGet(activeName);
  usmDbBackupNameGet(backupName);
#endif

  if(firstTime == L7_TRUE)
  {
    /* Show the file descriptions */

#if L7_FEAT_DIM_USE_FILENAME
    usmDbImageDescrGet(image1FileName, &image1Descr[0]);
    usmDbImageDescrGet(image2FileName, &image2Descr[0]);
#else
    usmDbImageDescrGet(activeName, activeDescr);
    usmDbImageDescrGet(backupName, backupDescr);
#endif

    ewsTelnetWrite(ewsContext, "\r\n\r\n");
    ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_ImageDescs);
#if L7_FEAT_DIM_USE_FILENAME
    ewsTelnetPrintf (ewsContext, "\r\n %6s : %s \r\n", image1FileName, image1Descr);
    ewsTelnetPrintf (ewsContext, " %6s : %s \r\n", image2FileName, image2Descr);
#else
    ewsTelnetPrintf (ewsContext, "\r\n %6s : %s \r\n", activeName, activeDescr);
    ewsTelnetPrintf (ewsContext, " %6s : %s \r\n", backupName, backupDescr);
#endif
    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);

    ewsTelnetWriteAddBlanks (1, 1, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_ImagesCurrentlyAvailableOnFlash);

    ewsTelnetWrite(ewsContext,
                   "\r\n--------------------------------------------------------------------");
    osapiSnprintf(buf, sizeof(buf), "\r\n %4s  %10s  %10s  %17s  %17s \r\n", pStrInfo_base_Unit_3,
#if L7_FEAT_DIM_USE_FILENAME
                  image1FileName, image2FileName,
#else
                  activeName, backupName,
#endif
                 pStrInfo_base_BootCurrentActive, pStrInfo_base_BootNextActive);
    ewsTelnetWrite(ewsContext, buf);
    ewsTelnetWrite(ewsContext,
                   "-------------------------------------------------------------------- \r\n");
    count = 13;   /* Already used lines */
    firstTime = L7_FALSE;
  }
  else
  {
    ewsTelnetWrite(ewsContext,
                   "\r\n--------------------------------------------------------------------");
    osapiSnprintf(buf, sizeof(buf), "\r\n %4s  %10s  %10s  %17s  %17s \r\n", pStrInfo_base_Unit_3,
#if L7_FEAT_DIM_USE_FILENAME
                  image1FileName, image2FileName,
#else
                  activeName, backupName,
#endif
                 pStrInfo_base_BootCurrentActive, pStrInfo_base_BootNextActive);
    ewsTelnetWrite(ewsContext, buf);
    ewsTelnetWrite(ewsContext,
                   "-------------------------------------------------------------------- \r\n");
    count=4;   /*Already used lines */
  }
/* j what about j ??*/
  for(; ((count < CLI_MAX_SCROLL_LINES-6)); count++)
  {
#if L7_FEAT_DIM_USE_FILENAME
    if (usmDbImageVersionGet(unitAry[j], image1FileName, &image1Version[0])
        != L7_SUCCESS)
    {
      OSAPI_STRNCPY_SAFE(image1Version, pStrInfo_base_None);
    }

    if (usmDbImageVersionGet(unitAry[j], image2FileName, &image2Version[0])
        != L7_SUCCESS)
    {
      OSAPI_STRNCPY_SAFE(image2Version, pStrInfo_base_None);
    }

    (void)usmDbActiveImageNameGet(unitAry[j], &activeImage[0]);
    (void)usmDbActivatedImageNameGet(unitAry[j], &activatedImage[0]);

    /* formulate the string and display */

    osapiSnprintf(buf, sizeof(buf), "\r\n %4d  %10s  %10s  %17s  %17s ", unitAry[j], image1Version,
                  image2Version, activeImage, activatedImage);
#else
    OSAPI_STRNCPY_SAFE(activeVersion, pStrInfo_base_None);
    if ((usmDbActiveImageNameGet(unitAry[j], activeFileName) == L7_SUCCESS) &&
        (osapiStrnlen(activeFileName, sizeof(activeFileName)) != 0))
    {
      (void)usmDbImageVersionGet(unitAry[j], activeFileName, activeVersion);
    }

    OSAPI_STRNCPY_SAFE(backupVersion, pStrInfo_base_None);
    if (usmDbBackupImageNameGet(unitAry[j], backupFileName) == L7_SUCCESS)
    {
      (void)usmDbImageVersionGet(unitAry[j], backupFileName, backupVersion);
    }

    OSAPI_STRNCPY_SAFE(activatedVersion, pStrInfo_base_None);
    if ((usmDbActivatedImageNameGet(unitAry[j], activatedImage) == L7_SUCCESS) &&
        (osapiStrnlen(activatedImage, sizeof(activatedImage)) != 0))
    {
      (void)usmDbImageVersionGet(unitAry[j], activatedImage, activatedVersion);
    }

    /* formulate the string and display */
    osapiSnprintf(buf, sizeof(buf), "\r\n %4d  %10s  %10s  %17s  %17s ", unitAry[j], activeVersion,
                  backupVersion, activeVersion, activatedVersion);
#endif

    ewsTelnetWrite(ewsContext, buf);
    j++;
    if( j >= numUnits)
    {
      j=0;
      numUnits=1;
      firstTime = L7_TRUE;
      memset(unitAry, 0,sizeof (unitAry[20]));
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  cliSyntaxBottom(ewsContext);
  cliAlternateCommandSet(pStrInfo_base_ShowBootVar);

  return pStrInfo_common_Name_2;     /* --More-- or (q)uit */

}

/*********************************************************************
*
* @purpose  Command to delete a file
*
*
* @param EwsContext     ewsContext
* @param L7_uint32      argc
* @param const L7_char8 **argv
* @param L7_uint32      index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  delete <file-name>
*
* @end
*
*********************************************************************/
const L7_char8 *commandDelete(EwsContext ewsContext, L7_uint32 argc,
                              const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 rc, unit, imgIndex;
  L7_char8 fileName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[100];

  memset(fileName, 0, sizeof(fileName));
  /* get switch ID based on presence/absence of STACKING package */

  usmDbUnitMgrNumberGet(&unit);

#ifdef L7_STACKING_PACKAGE
  if ((argc != 3) && (argc != 2))
#else
  if( argc != 2)
#endif
  {
    /* error in command */

    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_DELETE);
  }

  imgIndex = index+1;

#ifdef L7_STACKING_PACKAGE
  if (argc == 2)
  {
    /* all units */
    unit = L7_USMDB_CDA_CDA_ALL_UNITS;
  }
  else
  {
    OSAPI_STRNCPY_SAFE(buf, argv[index+1]);
    imgIndex = index+2;
    cliConvertTo32BitUnsignedInteger(buf, &unit);
    if (cliIsUnitPresent(unit) != L7_TRUE)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_DELETE);
    }
  }
#endif

  /* validate the command */

  /* validate the local file name */

  if(strlen(argv[imgIndex]) > L7_MAX_FILENAME)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_base_FileNameLen, L7_MAX_FILENAME);

  }
  else if (strlen(argv[index+1]) == 0)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_DELETE);
  }

  /* extract the file name */

  OSAPI_STRNCPY_SAFE(fileName, argv[imgIndex]);

  osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_DeletingImage, fileName);

  ewsTelnetWrite(ewsContext, buf);

  /* handover the request to usmdb */

  rc = usmDbFileDelete(unit, fileName);
  
  if (rc == L7_IMAGE_IN_USE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_base_ImageInUse);
  }
  else if(rc == L7_NOT_EXIST)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_FileNotFound);
  }
  else if( rc == L7_REQUEST_DENIED)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_ReqDenied);
  }
  else if (rc != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_FileNotFound);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

#if L7_FEAT_BOOTCODE_UPDATE
/*********************************************************************
*
* @purpose  Command to update the bootloader
*
*
* @param EwsContext     ewsContext
* @param L7_uint32      argc
* @param const L7_char8 **argv
* @param L7_uint32      index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  update bootcode
*
* @end
*
*********************************************************************/
const L7_char8 *commandUpdate(EwsContext ewsContext, L7_uint32 argc,
                              const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_RC_t rc;
  L7_char8 buf[100];

  /* get switch ID based on presence/absence of STACKING package */
  usmDbUnitMgrNumberGet(&unit);

#ifdef L7_STACKING_PACKAGE
  if((argc != 3) && (argc != 2) && (strcmp(argv[index+1], pStrInfo_base_BootCode) != 0))
#else
  if( (argc != 2) || (strcmp(argv[index+1], pStrInfo_base_BootCode) != 0))
#endif
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_UPDATEBOOTCODE);
  }

#ifdef L7_STACKING_PACKAGE
  if (argc == 2)
  {
    /* all units */
    unit = L7_USMDB_CDA_CDA_ALL_UNITS;
  }
  else
  {
    /* unit number in argv[index+2] */
    OSAPI_STRNCPY_SAFE(buf, argv[index+2]);
    cliConvertTo32BitUnsignedInteger(buf, &unit);
    if (cliIsUnitPresent(unit) != L7_TRUE)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_UPDATEBOOTCODE);
    }
    index++;
  }
#endif

  osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrErr_base_UpdatingBootCode);

  ewsContext->unbufferedWrite = L7_TRUE;
  ewsTelnetWrite( ewsContext, buf);

  rc = usmDbBootCodeUpdate(unit);
  if (rc == L7_IMAGE_IN_USE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, 
                             pStrInfo_base_ImageInUse);
  }
  else if (rc != L7_SUCCESS)
  {
#ifdef _L7_OS_VXWORKS_
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_UpdateBootFail);
#else
    L7_uint32 val;
    usmDbTransferResultGet(unit, &val, buf);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, buf, ewsContext, pStrErr_base_UpdateBootFail);
#endif
  }
  else
  {
#ifdef _L7_OS_VXWORKS_
    ewsTelnetWriteAddBlanks (0, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_BootCodeUpdated);
#else
    ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrInfo_common_Success, ewsContext, "!");
#endif
  }
  ewsContext->unbufferedWrite = L7_FALSE;

  return cliSyntaxReturnPrompt (ewsContext, "");
}
#endif

/*********************************************************************
*
* @purpose  Command to associate a text description with an image
*
*
* @param EwsContext     ewsContext
* @param L7_uint32      argc
* @param const L7_char8 **argv
* @param L7_uint32      index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  filedescr <image1 | image2> <descrition>
*
* @end
*
*********************************************************************/
const L7_char8 *commandFileDescr(EwsContext ewsContext, L7_uint32 argc,
                                 const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 rc, unit, len;
  L7_char8 buffer[L7_CLI_MAX_STRING_LENGTH+1];
  L7_uint32 imageId;

  /* get switch ID based on presence/absence of STACKING package */
  usmDbUnitMgrNumberGet(&unit);

  /* validate the image name */

  if ((usmDbImageFileNameValid((char *)argv[index+1], &imageId) != L7_TRUE) &&
      (usmDbImageNameValid((char *)argv[index+1]) != L7_TRUE))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_FileDescr_1);
  }

  /* check the length of the description */

  len = strlen( argv[index+2]);
  if (len > L7_CLI_MAX_STRING_LENGTH)
  {
    return cliSyntaxReturnPromptAddBlanks (1,2, 0, 1, L7_NULLPTR,ewsContext, pStrInfo_base_FileDescrTooLong, L7_CLI_MAX_STRING_LENGTH);
  }

  if (ewsContext->commType == CLI_NO_CMD)
  {
    memset(buffer, 0, sizeof(buffer));
  }
  else
  {
    osapiStrncpySafe(buffer, (L7_char8 *)argv[index+2], sizeof(buffer));
  }

  /* handover the request to usmdb */

  rc = usmDbImageDescrSet((L7_char8 *)argv[index+1], buffer);
  if (rc == L7_IMAGE_IN_USE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_base_ImageInUse);
  }
  else if(rc == L7_NOT_EXIST)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_NoImage);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");

}

#ifdef L7_AUTO_INSTALL_PACKAGE
/******************************************************************************
*
* @purpose  Enabled/Disabled autoinstall on device.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @end
*
******************************************************************************/
const L7_char8 *commandBootAutoinstall(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  
  if(numArg == 1)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if(strcmp(argv[index + numArg], pStrInfo_base_AutoInstallStart) == 0)
      {
        if(usmdbAutoInstallStartStopSet(L7_TRUE) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                                                  L7_NULLPTR, ewsContext,
                                                  pStrErr_base_AutoInstallMode);
      }
        }
        else if(strcmp(argv[index + numArg], pStrInfo_base_AutoInstallStop) == 0)
        {
          if(usmdbAutoInstallStartStopSet(L7_FALSE) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                                                  L7_NULLPTR, ewsContext,
                                                  pStrErr_base_AutoInstallMode);
          }
        }
    }
  }


  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/******************************************************************************
*
* @purpose  Enables/Disables autosave of downloaded configuration on device.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @end
*
******************************************************************************/
const L7_char8 *commandBootAutoinstallAutoSave(EwsContext ewsContext,
                                               L7_uint32 argc,
                                               const L7_char8 * * argv,
                                               L7_uint32 index)
{
  L7_uint32 autoSave = L7_FALSE;
  L7_uchar8 errStr[L7_CLI_MAX_ERROR_MSG_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      autoSave = L7_TRUE;
      osapiSnprintf(errStr, L7_CLI_MAX_ERROR_MSG_LENGTH, pStrErr_base_AutoInstallAutoSaveEnable);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      autoSave = L7_FALSE;
      osapiSnprintf(errStr, L7_CLI_MAX_ERROR_MSG_LENGTH, pStrErr_base_AutoInstallAutoSaveDisable);
    }
    if (usmdbAutoInstallAutoSaveSet(autoSave) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                                             L7_NULLPTR, ewsContext,
                                             errStr);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/******************************************************************************
*
* @purpose  Sets/Resets retry count for TFTP attempts attempts for specific configuration 
*           file download.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @end
*
******************************************************************************/
const L7_char8 *commandBootAutoinstallRetrycount(EwsContext ewsContext,
                                               L7_uint32 argc,
                                               const L7_char8 * * argv,
                                               L7_uint32 index)
{
  L7_uint32 retryCnt = L7_NULL;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (numArg != 1)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                                                L7_NULLPTR, ewsContext,
                                                pStrErr_base_AutoInstallRetryCnt);
      }
      if (cliConvertTo32BitUnsignedInteger((L7_char8 *)argv[index+1],
                                           &retryCnt) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
      }
      if ((retryCnt < FD_AUTO_INSTALL_UNICAST_RETRY_COUNT_MIN) ||
          (retryCnt > FD_AUTO_INSTALL_UNICAST_RETRY_COUNT_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                                                L7_NULLPTR, ewsContext,
                                                pStrErr_base_AutoInstallRetryCntRange);
      }
      if (usmdbAutoInstallFileDownLoadRetryCountSet(retryCnt) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                                               L7_NULLPTR, ewsContext,
                                               pStrErr_base_AutoInstallRetryCountEnable);
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (numArg != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                                                L7_NULLPTR, ewsContext,
                                                pStrErr_base_AutoInstallRetryCntNo);
      }
      retryCnt = FD_AUTO_INSTALL_UNICAST_RETRY_COUNT_DEFAULT;
      if (usmdbAutoInstallFileDownLoadRetryCountSet(retryCnt) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                                               L7_NULLPTR, ewsContext,
                                               pStrErr_base_AutoInstallRetryCountDisable);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

#endif /*  L7_AUTO_INSTALL_PACKAGE */

/******************************************************************************
*
* @purpose  Erase startup-config file.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @end
*
******************************************************************************/
const L7_char8 *commandEraseStartupConfig(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliGetCharInputID() == CLI_INPUT_EMPTY &&
              ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
  {
    cliSetCharInputID(U_IDX, ewsContext, argv);

    cliAlternateCommandSet(pStrInfo_base_EraseCmd);
    return pStrErr_base_StartupConfigEraseQuestionYesNo;
  }

  else if ( cliGetCharInputID() == 1 ||
                  ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
  {
    if ( tolower(cliGetCharInput()) == 'y' ||
                  ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
    {
        usmDbUnitMgrEraseStartupConfig();	
        if (usmdbEraseStartupConfig() != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks(1,1, 0, 0,
                        L7_NULLPTR, ewsContext, pStrErr_base_EraseStartupConfigFile);
        }
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                                                pStrErr_base_EraseStartupConfigFile);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt(ewsContext, "");
}

