/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_config_dvlan.c
 *
 * @purpose config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  03/05/2007
 *
 * @author  nshrivastav
 *
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_switching_cli.h"
#include "cliapi.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"
#include "l7_relay_api.h"

#include "ews.h"
#include "clicommands_card.h"

#include "usmdb_dvlantag_api.h"

/*********************************************************************
*
* @purpose  Configures DVLAN tagging EtherType
*
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
* @cmdsyntax  dvlan-tunnel ethertype {<802-1q|vman> | <custom><%d-%d>} [default-tpid]
*
* @cmdhelp Configure DVLAN EtherType for a specific interface
*
* @cmddescript
*   Determines the ethertype for a double tagged interface in a VLAN
*
* @end
*
*********************************************************************/
const L7_char8 *commandDVlanIntfEtherType(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t     rc = L7_FAILURE;
  L7_char8    strType[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32   argTypeVal=1;
  L7_uint32   argCustVal=2;
  L7_uint32   ether_val = 0;
  L7_uint32   unit;
  L7_uint32   slot, port;
  L7_uint32   numArg;
  L7_uint32   interface = 0;
  L7_BOOL     setEtherType = L7_FALSE;
  
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }
  
  /* Fetch Slot and Port information from ewsContext */
  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);
  

  if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (usmDbDvlantagIntfValidCheck(interface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_switching_DvlanIntfError);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg > 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_DvlanIntfEtherType, L7_DVLANTAG_MIN_ETHERTYPE, L7_DVLANTAG_MAX_ETHERTYPE );
  }

  OSAPI_STRNCPY_SAFE(strType, argv[index+ argTypeVal]);

  if ((strcmp(strType, pStrInfo_common_DvlanEtherType802) == 0))
  {
    ether_val = L7_DVLANTAG_802_1Q_ETHERTYPE;
  }
  
  else if ((strcmp(strType, pStrInfo_switching_DvlanEtherTypeVman) == 0) ||
           (strcmp(strType, pStrInfo_common_DvlanEtherTypeVman_1) == 0))
  {
    ether_val = L7_DVLANTAG_VMAN_ETHERTYPE;
  }

  else if ((strcmp(strType, pStrInfo_switching_DvlanEtherTypeCustom) == 0) ||
      (strcmp(strType, pStrInfo_common_DvlanEtherTypeCustom_1) == 0))
  {
    if (cliCheckIfInteger((L7_char8 *)argv[index+argCustVal]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    ether_val = atoi((L7_char8 *)argv[index+argCustVal]);
    if ((ether_val < L7_DVLANTAG_MIN_ETHERTYPE) || (ether_val > L7_DVLANTAG_MAX_ETHERTYPE))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_DvlanEtherType, L7_DVLANTAG_MIN_ETHERTYPE, L7_DVLANTAG_MAX_ETHERTYPE );
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_DvlanEtherType, L7_DVLANTAG_MIN_ETHERTYPE, L7_DVLANTAG_MAX_ETHERTYPE );
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    setEtherType = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    setEtherType = L7_FALSE;
  }
  
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_ETHERTYPE_FEATURE_ID) == L7_TRUE)
    {
      rc = usmDbDvlantagIntfEthertypeSet(unit, interface, ether_val, setEtherType);
      switch (rc)
      {
        case L7_NOT_EXIST:
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, "TPID no configured yet");
          break;
        case L7_NOT_SUPPORTED:
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, "DVLAN not enabled on this interface");
          break;
        case L7_FAILURE:
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_DvlanEtherType, L7_DVLANTAG_MIN_ETHERTYPE, L7_DVLANTAG_MAX_ETHERTYPE );
          break;
        default:
          break;
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures DVLAN tagging EtherType
*
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
* @cmdsyntax  dvlan-tunnel ethertype {<802-1q|vman> | <custom><%d-%d>} [default-tpid]
*
* @cmdhelp Configure DVLAN EtherType for a specific interface
*
* @cmddescript
*   Determines the ethertype for a double tagged interface in a VLAN
*
* @end
*
*********************************************************************/
const L7_char8 *commandDVlanEtherType(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t     rc = L7_FAILURE;
  L7_char8    strType[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32   argTypeVal=1;
  L7_uint32   argCustVal=2;
  L7_uint32   argDefaultTpidVal = 0;
  L7_uint32   ether_val = 0;
  L7_uint32   unit;
  L7_uint32   numArg;
  L7_BOOL     defaultTpid = L7_FALSE, setEtherType = L7_FALSE;
  
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg > 3)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_DvlanEtherType, L7_DVLANTAG_MIN_ETHERTYPE, L7_DVLANTAG_MAX_ETHERTYPE );
  }

  OSAPI_STRNCPY_SAFE(strType, argv[index+ argTypeVal]);

  if ((strcmp(strType, pStrInfo_common_DvlanEtherType802) == 0))
  {
    if(numArg == 2)
    {
      argDefaultTpidVal = argTypeVal+1;
    }
    ether_val = L7_DVLANTAG_802_1Q_ETHERTYPE;
  }
  
  else if ((strcmp(strType, pStrInfo_switching_DvlanEtherTypeVman) == 0) ||
           (strcmp(strType, pStrInfo_common_DvlanEtherTypeVman_1) == 0))
  {
    if(numArg == 2)
    {
      argDefaultTpidVal = argTypeVal+1;
    }
    ether_val = L7_DVLANTAG_VMAN_ETHERTYPE;
  }

  else if ((strcmp(strType, pStrInfo_switching_DvlanEtherTypeCustom) == 0) ||
      (strcmp(strType, pStrInfo_common_DvlanEtherTypeCustom_1) == 0))
  {
    if (cliCheckIfInteger((L7_char8 *)argv[index+argCustVal]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    ether_val = atoi((L7_char8 *)argv[index+argCustVal]);
    if ((ether_val < L7_DVLANTAG_MIN_ETHERTYPE) || (ether_val > L7_DVLANTAG_MAX_ETHERTYPE))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_DvlanEtherType, L7_DVLANTAG_MIN_ETHERTYPE, L7_DVLANTAG_MAX_ETHERTYPE );
    }
    if(numArg == 3)
    {
      argDefaultTpidVal = argCustVal+1;
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_DvlanEtherType, L7_DVLANTAG_MIN_ETHERTYPE, L7_DVLANTAG_MAX_ETHERTYPE );
  }

  if(strcmp(argv[index+argDefaultTpidVal], pStrInfo_common_DefTpid) == 0 || 
    usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_MULTI_TPIDS_FEATURE_ID) != L7_TRUE)
  {
    defaultTpid = L7_TRUE;
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    setEtherType = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    setEtherType = L7_FALSE;
  }
  
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_ETHERTYPE_FEATURE_ID) == L7_TRUE)
    {
      rc = usmDbDvlantagEthertypeSet(unit, ether_val, defaultTpid, setEtherType);
      switch (rc)
      {
        case L7_NOT_EXIST:
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, "No free TPID Index");
          break;
        case L7_ALREADY_CONFIGURED:
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, "TPID already configured");
          break;
        case L7_FAILURE:
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_DvlanEtherType, L7_DVLANTAG_MIN_ETHERTYPE, L7_DVLANTAG_MAX_ETHERTYPE );
          break;
        default:
          break;
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures DVLAN tunneling/tagging
*
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
* @cmdsyntax  mode dvlan-tunnel  or
* @cmdsyntax  mode dot1q-tunnel
*
* @cmdhelp Configure DVLAN tagging for a specific interface
*
* @cmddescript
*   Determines the tagging mode for an interface in a VLAN
*
* @end
*
*********************************************************************/
const L7_char8 *commandDVlanTagging(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = 1;
  L7_uint32 interface = 0;
  L7_uint32 slot,port;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* Fetching Port  information from ewsContext */
  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (usmDbDvlantagIntfValidCheck(interface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_switching_DvlanIntfError);
  }

  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_DvlanTagging);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbDvlantagIntfModeSet(unit, interface, L7_ENABLE);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrInfo_switching_DvlanModeSetFail);
      }
    }
  }
  else if (ewsContext->commType ==  CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_DvlanTaggingNo);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbDvlantagIntfModeSet(unit, interface, L7_DISABLE);
      if (rc != L7_SUCCESS)
      {
        cliSyntaxBottom(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrInfo_switching_DvlanModeSetFail);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures DVLAN tagging Customer Id
*
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
* @cmdsyntax  dvlan-tunnel customer-id
*
* @cmdhelp Configure DVLAN Customer Id for a specific interface
*
* @cmddescript
*   Determines the tagging mode for an interface in a VLAN
*
* @end
*
*********************************************************************/
const L7_char8 *commandDVlanCustomerId(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = 1;
  L7_uint32 custID = 0;
  L7_uint32 interface = 0;
  L7_uint32 slot,port;
  L7_uint32 argCustID=1;
  L7_char8 strCustId[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* Fetching Port  information from ewsContext */
  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (usmDbDvlantagIntfValidCheck(interface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_switching_DvlanIntfError);
  }

  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_DvlanTaggingCustid, L7_DVLANTAG_MIN_CUSTID, L7_DVLANTAG_MAX_CUSTID );
    }
    OSAPI_STRNCPY_SAFE(strCustId, argv[index+ argCustID]);
    if (cliCheckIfInteger(strCustId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidInteger);
    }

    custID = atoi(strCustId);
    if ((custID < L7_DVLANTAG_MIN_CUSTID) || (custID > L7_DVLANTAG_MAX_CUSTID))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_DvlanTaggingCustid, L7_DVLANTAG_MIN_CUSTID, L7_DVLANTAG_MAX_CUSTID );
    }
  }
  else if (ewsContext->commType ==  CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_DvlanTaggingCustidNo);
    }

    custID = L7_DVLANTAG_DEFAULT_CUSTID;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbDvlantagIntfCustIdSet(unit, interface, custID);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  ewsContext, pStrInfo_switching_DvlanCustidSetFail);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}
