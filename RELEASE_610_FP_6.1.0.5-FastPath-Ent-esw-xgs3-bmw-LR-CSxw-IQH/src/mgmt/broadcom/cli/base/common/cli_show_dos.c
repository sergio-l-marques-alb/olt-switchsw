/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2005-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_dos.c
 *
 * @purpose show commands for the Denial of Service cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  09/01/2005
 *
 * @author  esmiley
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "usmdb_util_api.h"
#include "comm_mask.h"
#include "cliapi.h"
#include "cliutil.h"

#include "datatypes.h"
#include "ctype.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "ews.h"


#include "usmdb_dos_api.h"
#include "doscontrol_exports.h"

/* Denial of Service CLI Commands */

/*********************************************************************
*
* @purpose     Display Denial of Service Configuration
*
*
* @param       EwsContext ewsContext
* @param       L7_uint32 argc
* @param       const L7_char8 **argv
* @param       L7_uint32 index
*
* @returntype  const L7_char8  *
* @returns     cliPrompt(ewsContext)
*
* @notes       Add argument checking
*
* @cmdsyntax   show dos-control
*
* @cmdhelp     Display Denial of Service configuration information.
*
* @cmddescript
*   Show Denial of Service Configuration
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowDenialOfService(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 val;
  L7_int32 size;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowDos_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_FIRSTFRAG_FEATURE_ID) == L7_TRUE)
  {
    cliFormat(ewsContext,pStrInfo_base_DosFirstFragMode);
    rc=usmdbDoSFirstFragModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }

    cliFormatAddBlanks (0, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_base_DosTcpSize);
    rc=usmdbDoSFirstFragSizeGet(&size);
    if (rc == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "%d",size);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
  {
    cliFormat(ewsContext,pStrInfo_base_DosIcmpMode);
    rc=usmdbDoSICMPModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMPV4_FEATURE_ID) != L7_TRUE &&
        usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMPV6_FEATURE_ID) != L7_TRUE) 
    {
       cliFormatAddBlanks (0, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_base_DosIcmpSize);
       rc=usmdbDoSICMPSizeGet(&size);
       if (rc == L7_SUCCESS)
       {
         ewsTelnetPrintf (ewsContext, "%d",size);
       }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMPV4_FEATURE_ID) == L7_TRUE)
  {
    cliFormatAddBlanks (0, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_base_DosIcmpv4Size);
    rc=usmdbDoSICMPv4SizeGet(&size);
    if (rc == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "%d",size);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMPV6_FEATURE_ID) == L7_TRUE)
  {
    cliFormatAddBlanks (0, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_base_DosIcmpv6Size);
    rc=usmdbDoSICMPv6SizeGet(&size);
    if (rc == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "%d",size);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMPFRAG_FEATURE_ID) == L7_TRUE)
  {
    cliFormat(ewsContext,pStrInfo_base_DosIcmpFragMode);
    rc=usmdbDoSICMPFragModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }
  }

  if ((usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_L4PORT_FEATURE_ID) == L7_TRUE)
      && (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPPORT_FEATURE_ID) == L7_FALSE))
  {
    cliFormat(ewsContext,pStrInfo_base_DosL4PortMode);
    rc=usmdbDoSL4PortModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPPORT_FEATURE_ID) == L7_TRUE)
  {
    cliFormatAddBlanks (0, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_base_DosTCPPortMode);
    rc=usmdbDoSTCPPortModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_UDPPORT_FEATURE_ID) == L7_TRUE)
  {
    cliFormatAddBlanks (0, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_base_DosUDPPortMode);
    rc=usmdbDoSUDPPortModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_SIPDIP_FEATURE_ID) == L7_TRUE)
  {
    cliFormat(ewsContext,pStrInfo_base_DosSipDipMode);
    rc=usmdbDoSSIPDIPModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_SMACDMAC_FEATURE_ID) == L7_TRUE)
  {
    cliFormat(ewsContext,pStrInfo_base_DosSmacDmacMode);
    rc=usmdbDoSSMACDMACModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }
  }

  if ((usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFLAG_FEATURE_ID) == L7_TRUE)
       && (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFLAGSEQ_FEATURE_ID) == L7_FALSE))
  {
    cliFormat(ewsContext,pStrInfo_base_DosTcpFlagMode);
    rc=usmdbDoSTCPFlagModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFINURGPSH_FEATURE_ID) == L7_TRUE)
  {
    cliFormatAddBlanks (0, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_base_DosTCPFinUrgPshMode);
    rc=usmdbDoSTCPFinUrgPshModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFLAGSEQ_FEATURE_ID) == L7_TRUE)
  {
    cliFormatAddBlanks (0, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_base_DosTcpFlagSeqMode);
    rc=usmdbDoSTCPFlagSeqModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPSYN_FEATURE_ID) == L7_TRUE)
  {
    cliFormatAddBlanks (0, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_base_DosTCPSynMode);
    rc=usmdbDoSTCPSynModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPSYNFIN_FEATURE_ID) == L7_TRUE)
  {
    cliFormatAddBlanks (0, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_base_DosTCPSynFinMode);
    rc=usmdbDoSTCPSynFinModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }
  }


  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFRAG_FEATURE_ID) == L7_TRUE)
  {
    cliFormat(ewsContext,pStrInfo_base_DosTcpFragMode);
    rc=usmdbDoSTCPFragModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPOFFSET_FEATURE_ID) == L7_TRUE)
  {
    cliFormatAddBlanks (0, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_base_DosTCPOffsetMode);
    rc=usmdbDoSTCPOffsetModeGet(&val);
    if (rc == L7_SUCCESS)
    {
      if (val == L7_ENABLE)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Enbl_1);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_common_Dsbl_1);
      }
    }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/* End - Denial of Service CLI Commands */
