/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/acl/cliutil_acl.c
 *
 * @purpose  ACL utility functions' implementation.
 *
 * @component  IS-CLI
 *
 * @comments  None
 *
 * @create  03/24/2004
 *
 * @author  rjindal
 *
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "cliapi.h"
#include "acl_api.h"

#ifndef _L7_OS_LINUX_
  #include <vxworks_config.h>   /* for DEFAULT_MAX_CONNECTIONS */
#endif /* _L7_OS_LINUX_ */

#include "clicommands_acl.h"
#include "cliutil_acl.h"
#include "usmdb_qos_acl_api.h"
#include "cli_web_exports.h"

/*********************************************************************
* @purpose  CLI helper routine for supported DSCP keywords string
*
* @param  Void
*
* @returns  DSCP keywords' string
*
* @notes
*
* @end
*********************************************************************/
L7_char8 *cliTreeAccessListDscpNodeHelp(void)
{
  L7_uint32 unit;
  static L7_char8 dscp_string[L7_CLI_MAX_LARGE_STRING_LENGTH];

  memset (dscp_string, 0, L7_CLI_MAX_LARGE_STRING_LENGTH);
  unit = cliGetUnitId();
  if (unit != 0)
  {
    strcpy(dscp_string, pStrInfo_common_EnterADscpValInRangeOf0To63OrADscpKeyword);

    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, dscp_string, pStrInfo_qos_Af11Af12Af13Af21Af22Af23Af31Af32Af33Af41Af42Af43);
    strcat(dscp_string, pStrInfo_qos_BeCs0Cs1Cs2Cs3Cs4Cs5Cs6Cs7Ef);

    strcat(dscp_string, ").");
  }

  return dscp_string;
}

/*********************************************************************
* @purpose  Convert a DSCP keyword string to the associated value. If
*           string holds an integer value, then integer value is returned.
*
* @param  *dscpString  @b{(input)) pointer to dscp string
* @param  dscpVal      @b{(output)) pointer to the dscp value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cliAclConvertDSCPStringToVal(L7_char8 * dscpString, L7_uint32 * dscpVal)
{
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return L7_FAILURE;
  }

  if (strcmp(dscpString, pStrInfo_common_Af11_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_AF11;
  }
  else if (strcmp(dscpString, pStrInfo_common_Af12_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_AF12;
  }
  else if (strcmp(dscpString, pStrInfo_common_Af13_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_AF13;
  }
  else if (strcmp(dscpString, pStrInfo_common_Af21_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_AF21;
  }
  else if (strcmp(dscpString, pStrInfo_common_Af22_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_AF22;
  }
  else if (strcmp(dscpString, pStrInfo_common_Af23_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_AF23;
  }
  else if (strcmp(dscpString, pStrInfo_common_Af31_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_AF31;
  }
  else if (strcmp(dscpString, pStrInfo_common_Af32_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_AF32;
  }
  else if (strcmp(dscpString, pStrInfo_common_Af33_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_AF33;
  }
  else if (strcmp(dscpString, pStrInfo_common_Af41_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_AF41;
  }
  else if (strcmp(dscpString, pStrInfo_common_Af42_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_AF42;
  }
  else if (strcmp(dscpString, pStrInfo_common_Af43_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_AF43;
  }
  else if (strcmp(dscpString, pStrInfo_common_Be_1) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_BE;
  }
  else if (strcmp(dscpString, pStrInfo_common_Cs0) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_CS0;
  }
  else if (strcmp(dscpString, pStrInfo_common_Cs1_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_CS1;
  }
  else if (strcmp(dscpString, pStrInfo_common_Cs2_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_CS2;
  }
  else if (strcmp(dscpString, pStrInfo_common_Cs3_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_CS3;
  }
  else if (strcmp(dscpString, pStrInfo_common_Cs4_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_CS4;
  }
  else if (strcmp(dscpString, pStrInfo_common_Cs5_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_CS5;
  }
  else if (strcmp(dscpString, pStrInfo_common_Cs6_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_CS6;
  }
  else if (strcmp(dscpString, pStrInfo_common_Cs7_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_CS7;
  }
  else if (strcmp(dscpString, pStrInfo_common_Ef_2) == 0)
  {
    *dscpVal = L7_USMDB_ACL_IP_DSCP_EF;
  }
  else
  {
    /* verify if the specified  argument is an integer */
    if (cliCheckIfInteger(dscpString) != L7_SUCCESS)
    {
      return L7_ERROR;
    }

    /* check for overflow of integer value */
    if (cliConvertTo32BitUnsignedInteger(dscpString, dscpVal) != L7_SUCCESS)
    {
      return L7_ERROR;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert a DSCP value to an associated keyword, if available.
*           Else, a string of the input dscpVal is returned.
*
* @param  dscpVal      @b{(input)) the dscp value to convert
* @param  *dscpString  @b{(output)) pointer to string to be returned
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cliAclConvertDSCPValToString(L7_uint32 dscpVal, L7_char8 * dscpString)
{
  switch(dscpVal)
  {
  case L7_USMDB_ACL_IP_DSCP_AF11:
    sprintf (dscpString, pStrInfo_qos_Af11, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_AF12:
    sprintf (dscpString, pStrInfo_qos_Af12, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_AF13:
    sprintf (dscpString, pStrInfo_qos_Af13, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_AF21:
    sprintf (dscpString, pStrInfo_qos_Af21, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_AF22:
    sprintf (dscpString, pStrInfo_qos_Af22, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_AF23:
    sprintf (dscpString, pStrInfo_qos_Af23, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_AF31:
    sprintf (dscpString, pStrInfo_qos_Af31, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_AF32:
    sprintf (dscpString, pStrInfo_qos_Af32, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_AF33:
    sprintf (dscpString, pStrInfo_qos_Af33, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_AF41:
    sprintf (dscpString, pStrInfo_qos_Af41, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_AF42:
    sprintf (dscpString, pStrInfo_qos_Af42, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_AF43:
    sprintf (dscpString, pStrInfo_qos_Af43, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_BE:
    sprintf (dscpString, pStrInfo_qos_BeCs0, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_CS1:
    sprintf (dscpString, pStrInfo_qos_Cs1, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_CS2:
    sprintf (dscpString, pStrInfo_qos_Cs2, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_CS3:
    sprintf (dscpString, pStrInfo_qos_Cs3, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_CS4:
    sprintf (dscpString, pStrInfo_qos_Cs4, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_CS5:
    sprintf (dscpString, pStrInfo_qos_Cs5, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_CS6:
    sprintf (dscpString, pStrInfo_qos_Cs6, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_CS7:
    sprintf (dscpString, pStrInfo_qos_Cs7, dscpVal);
    break;
  case L7_USMDB_ACL_IP_DSCP_EF:
    sprintf (dscpString, pStrInfo_qos_Ef, dscpVal);
    break;
  default:
    sprintf (dscpString, "%u", dscpVal);
    break;
  }

  return L7_SUCCESS;
}
