/*
 * Product: EmWeb/CLI
 * Release: R6_2_0
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
 * THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
 * AND OTHER US AND INTERNATIONAL PATENTS PENDING.
 * 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
 *
 * Notice to Users of this Software Product:
 *
 * This software product of Virata Corporation ("Virata"), 5 Clock Tower
 * Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in
 * source and object code format embodies valuable intellectual property
 * including trade secrets, copyrights and patents which are the exclusive
 * proprietary property of Virata. Access, use, reproduction, modification
 * disclsoure and distribution are expressly prohibited unless authorized
 * in writing by Virata.  Under no circumstances may you copy this
 * software or distribute it to anyone else by any means whatsoever except in
 * strict accordance with a license agreement between Virata and your
 * company.  This software is also protected under patent, trademark and
 * copyright laws of the United States and foreign countries, as well as
 * under the license agreement, against any unauthorized copying, modification,
 * or distribution.
 * Please see your supervisor or manager regarding any questions that you may
 * have about your right to use this software product.  Thank you for your
 * cooperation.
 *
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
 * All Rights Reserved
 *
 * Virata Corporation
 * 5 Clock Tower Place
 * Suite 400
 * Maynard, MA  01754
 *
 * Voice: +1 (978) 461-0888
 * Fax:   +1 (978) 461-2080
 *
 * http://www.emweb.com/
 *   support@agranat.com
 *     sales@agranat.com
 *
 * EmWeb/Server CLI interfaces
 */

#include "strlib_common_common.h"
#include "strlib_base_common.h"
#include <stdarg.h>
#include "l7_common.h"          /* LVL7_P0006 */
#include "usmdb_unitmgr_api.h"
#include "usmdb_util_api.h"
#include "osapi.h"              /* LVL7_P0006 */
#include "osapi_support.h"
#include "defaultconfig.h"      /* LVL7_P0006 */

#include "ews.h"
#include "ews_sys.h"
#include "ews_telnet.h"

#include "cli.h"                /* LVL7_P0006 */
#include "cliutil.h"            /* LVL7_P0006 */
#include "clicommands.h"        /* LVL7_P0006 */
#include "clicommands_card.h"
#include "cliapi.h"             /* LVL7_P0006 */
#include "cli_web_mgr_api.h"
#include "sysapi.h"
#include "login_sessions_api.h"

#ifdef L7_IPV6_PACKAGE
#define  L7_AF_INET6             2
#endif

extern char *strcpy (char * __s1, const char * __s2);    /* LVL7_P0006 */
extern int tolower(int __c);                           /* LVL7_P0006 */
extern int sprintf (char *, const char *, ...);        /* LVL7_P0006 */

static boolean ewsCliTreeHiddenDebugSupportFunctionIsEnabled(void);

static L7_uint32 currUnit = L7_MAX_UNITS_PER_STACK + 1;

void ewsCliPrintCaret( EwsCliState cli, uintf marker, sintf token, EwsContext context, boolean printMsg );
void ewsCliPrintHelp( EwsContext context, const char * str, int startPos, unsigned char cmdPad);
/*
 * Default allocator to use if none specified by the caller.
 *
 * This version is conservative in its assumptions, but
 * profligate in its memory consumption in that it copies
 * the string arguments passed in by the caller, which is
 * sensible if they are on the stack somewhere, but not so good
 * if they are in the data segment.
 *
 */

static EwsCliCommand *ewsCliDefaultAlloc(
  const char * command,
  const char * description)
{
  uintf clen = 0, dlen = 0;
  uintf string_len;
  EwsCliCommandP node;

  /*
   * Create new node
   */
  if (command != NULL)
  {
    EMWEB_STRLEN(clen, command);
    /* Need to add a trailing space. */
    if ( ' ' != command[clen - 1] )
    {
      clen++;
    }
  }
  if (description != NULL)
  {
    EMWEB_STRLEN(dlen, description);
  }

  string_len = clen + dlen + 2;

  node = (EwsCliCommandP) ewaAlloc(sizeof(EwsCliCommand) + string_len);
  if (node == NULL)
  {
    return NULL;
  }

  /*
   * Initialize node
   */

  node->command = (char *) &node[1];
  node->description = node->command + clen + 1;
  if (command != NULL)
  {
    EMWEB_STRCPY(node->command, command);
    /* Ensure command has a trailing space. */
    if ( ' ' != command[clen - 1] )
    {
      EMWEB_STRCAT((char *)node->command, (char *)" ");
    }
  }
  else
  {
    node->command = NULL;
  }
  if (description != NULL)
  {
    EMWEB_STRCPY(node->description, description);
  }
  else
  {
    node->description = NULL;
  }

  return node;
}

static void ewsCliDefaultDealloc( EwsCliCommand * node )
{
  ewaFree(node);
}

static EwsCliCommandAllocator defaultNodellocator = {
  &ewsCliDefaultAlloc,
  &ewsCliDefaultDealloc,
  0
};

/*
 * ewsCliStart
 * Reset parser state
 *
 *   context - request context
 */
void ewsCliStart ( EwsContext context)
{
  uintf i;

  context->telnet->cli.historyTop = 0;
  context->telnet->cli.historyBottom = 0;
  context->telnet->cli.historyIndex = 0;
  context->telnet->cli.historySearch = 0;
  context->telnet->cli.lineIndex = 0;
  context->telnet->cli.depth = 0;
  for (i = 0; i < EWS_CLI_MAX_HISTORY; i++)
  {
    context->telnet->cli.line[i][0] = '\0';
    context->telnet->cli.line[i][EWS_CLI_MAX_LINE - 1] = '\0';
  }
  context->telnet->cli.arrow_esc = 0;
  context->telnet->cli.arrow_lb = 0;
}

/*
 * ewsCliKillLine
 * Clear current line
 *
 *   context - request context
 *   cli     - CLI state
 */
void ewsCliKillLine ( EwsContext context, EwsCliState cli )
{
  sintf i;
  while (cli->line[cli->historyIndex][cli->lineIndex] != '\0')
  {
    cli->lineIndex++;
    ewsTelnetWrite(context, " ");
  }
  for (i = cli->lineIndex; i > 0; i--)
  {
    ewsTelnetWrite(context, "\b \b");
  }
}

/*
 * ewsCliSaveLine
 * Save Line in deleted buffer
 *
 *   cli     - CLI state
 */
void ewsCliSaveLine( EwsCliState cli, char * buff, sintf length )
{
  uintf buffLen=0;
  EMWEB_STRLEN(buffLen, cli->deletedBuffer);
  if( length >buffLen)
  {
    length = buffLen;
  }
  memset ( cli->deletedBuffer, 0,sizeof(cli->deletedBuffer));
  strncpy(cli->deletedBuffer, buff, length);
}

/*
 * ewsCliSaveChar
 * Save a char in deleted buffer
 *
 *   cli     - CLI state
 *   ch - Char to be saved
 *   initBuffer - bzero deletedBuffer if TRUE
 */
void ewsCliSaveChar(  EwsCliState cli,  char ch,  boolean initBuffer )
{
  uintf buffLen=0;

  if(initBuffer == TRUE)
  {
    memset ( cli->deletedBuffer, 0,sizeof(cli->deletedBuffer));
  }
  else
  {
    EMWEB_STRLEN(buffLen, cli->line[cli->historyIndex]);
  }
  cli->deletedBuffer[buffLen]=ch;

}
/*
 * ewsCliParseLine
 * Parse line into arguments
 *
 *   context - request context
 *   cli     - CLI state
 */
void ewsCliParseLine ( EwsContext context, EwsCliState cli )
{
  char * cp;

  /*
   * Copy current line into argument buffer.  We're going to replace
   * spaces with nuls to break line into tokens.  A copy is needed so
   * we don't lose history.
   */

  if ( cliCheckAltInput() == FALSE)    /* LVL7_P0006 start */
  {
    EMWEB_STRCPY(cli->arg_buffer, cli->line[cli->historyIndex]);
    cp = cli->arg_buffer;
    ewsStringTrimLeading (cp);
  }                                    /* LVL7_P0006 start */
  else
  {
    osapiStrncpySafe(cli->arg_buffer, cliAlternateCommandGet(), sizeof(cli->arg_buffer));
    cp = cli->arg_buffer;
    cliAlternateCommandSet("");
  }                                    /* LVL7_P0006 end */

  /*
   * If depth, argv[0..depth-1] is already initialized.  Prepare to
   * parse command line into arguments >= depth.
   */
  cli->argc = cli->depth;
  if((cp[0] == 'n' || cp[0] == 'N') && (cp[1] == 'o' || cp[1] == 'O') && cp[2] == ' ' )
  {
    context->commType = CLI_NO_CMD;
    cp+=2;
    ewsStringTrimLeading (cp);
  }
  else
  {
    context->commType = CLI_NORMAL_CMD;
  }
  cli->argv[cli->depth] = cp;

  /*
   * Scan line until end
   */
  while (*cp != '\0' && cli->argc < EWS_CLI_MAX_ARGS - 1)
  {
    /*
     * Handle quoted arguments
     */
    if (*cli->argv[cli->argc] == '"' || *cli->argv[cli->argc] == '\'')
    {
      if ( (cp[1] == '"' || cp[1] == '\'') && (cp[0] == '"' || cp[0] == '\''))
      {
        break;
      }
      if (cp == cli->argv[cli->argc] ||
          *cp != *cli->argv[cli->argc])
      {
        cp++;
        continue;
      }
      *cp = ' ';
      cli->argv[cli->argc] += 1;
    }

    /* LVL7_P5804 start */
    /*
     * If the comment character is observed anywhere on the command line,
     * ignore the rest of the line until CRLF, provided the comment charecter
     * is not encountered within a quoted argument
     */
    if ((*cp == EWS_CLI_COMMENT_CHAR) && ((*cli->argv[cli->argc] != '"') ||
                                         (*cli->argv[cli->argc] != '\'')))
    {
      if(cli->argc > 0)
      {
        cli->argc--;
      }
      break;
    }
    /* LVL7_P5804 end */

    /*
     * Space terminates current argument, prepare for next argument
     */
    if (*cp == ' ')
    {
      for (; *cp == ' '; cp++)
      {
        *cp = '\0';
      }

      /*check for null argument for cases like "" type of parameters*/
      if(strlen(cli->argv[cli->argc]) == 0)
      {
        if(cli->argc > 0)
        {
          cli->argc--;
        }

        break;
      }

      cli->argc++;
      cli->argv[cli->argc] = cp;
    }
    else
    {
      cp++;
    }
  }

  /*
   * Count last argument
   */
  /* LVL7_P5804 start */
  if ((cli->argv[cli->argc][0] != '\0') && (cli->argv[cli->argc][0] != EWS_CLI_COMMENT_CHAR))
  /* LVL7_P5804 end */
  {
    cli->argc++;
  }
}

/*
 * ewsCliPossible
 * Generate context-sensitive help
 *
 *   context     - context of request
 *   full_help   - TRUE if generating help lines with descriptions
 *                 FALSE if generating list of possible completions
 *   command     - current possible command
 *   description - description of possible command
 *   column      - (input/output) current column
 *   cmdPad      - (input) padding required for command column
 */
void
ewsCliPossible ( EwsContext context
                ,boolean full_help
                ,const char * command
                ,const char * description
                ,uintf * column
                ,unsigned char cmdPad
)
{
  uintf i;

  /*
   * If on column zero, newline
   */
  if (*column == 0)
  {
    ewsTelnetWrite(context, pStrInfo_common_CrLf);
  }

  /*
   * Write command padded to width
   */
  ewsTelnetWrite(context, command);
  EMWEB_STRLEN(i, command);
  for (; i < cmdPad; i++)
  {
    ewsTelnetWrite(context, " ");
  }

  /*
   * If full help, write description followed by newline
   */
  if (full_help)
  {
    if (description != NULL)
    {
      ewsCliPrintHelp(context, description, cmdPad, cmdPad);
    }
  }

  /*
   * Otherwise, wrap columns
   */
  else
  {
    (*column) += 1;
    if (*column == EWS_CLI_POSSIBLE_COLS)
    {
      *column = 0;
    }
  }
}

/*
 * chkIfUsed
 *
 * Checks if the same node has already been used earlier in the comand.
 * Returns L7_TRUE if node is used earlier.
 * Returns L7_FALSE if node is not used earlier.
 *
 */
boolean chkIfUsed(EwsCliState cli, EwsCliCommandP mp)
{
  int y, z;

  for(z = 0 ; z < cli->parseCount && z < (cli->argc - 1); z++)
  {
    if(mp == cli->parseList[z] && strncmp(mp->command,pStrInfo_common_Cr, 4) != 0)
    {
      return L7_TRUE;
    }

    for(y = 0 ; cli->parseList[z]->sameAsNodes[y] != NULL && y < MAX_SAME_AS_PER_NODE; y++)
    {
      if(mp == cli->parseList[z]->sameAsNodes[y])
      {
        return L7_TRUE;
      }
    }
  }

  return L7_FALSE;
}

/*
 * chkIfAlreadyParsed
 *
 * Checks if the same node has already been parsed.
 * Returns L7_TRUE if node has been parsed earlier.
 * Returns L7_FALSE if node has not been parsed earlier.
 *
 */
boolean chkIfAlreadyParsed(EwsCliState cli, EwsCliCommandP mp)
{
  int y, z;

  for(z = 0 ; z < cli->parseCount; z++)
  {
    if(mp == cli->parseList[z] && strncmp(mp->command,pStrInfo_common_Cr, 4) != 0)
    {
      return L7_TRUE;
    }

    for(y = 0 ; cli->parseList[z]->sameAsNodes[y] != NULL && y < MAX_SAME_AS_PER_NODE; y++)
    {
      if(mp == cli->parseList[z]->sameAsNodes[y])
      {
        return L7_TRUE;
      }
    }
  }

  return L7_FALSE;
}

/* LVL7_P51286 start */
/*
 * chkIfStillDisplayable
 *
 * Checks if the same node can still be displayed based on number of times allowed.
 * Returns L7_TRUE if node has not reached its display maximum for this command.
 * Returns L7_FALSE if node has reached its display maximum for this command.
 *
 */
boolean chkIfStillDisplayable(EwsCliState cli, EwsCliCommandP mp)
{
  int y, z;
  int16 count = mp->maxDisplayCount;  /* max is always 1 or more */

  for(z = 0 ; z < cli->parseCount && z < (cli->argc - 1); z++)
  {
    if(mp == cli->parseList[z] && strncmp(mp->command,pStrInfo_common_Cr, 4) != 0)
    {
      if (--count > 0)
      {
        continue;
      }

      return L7_FALSE;
    }

    for(y = 0 ; cli->parseList[z]->sameAsNodes[y] != NULL && y < MAX_SAME_AS_PER_NODE; y++)
    {
      if(mp == cli->parseList[z]->sameAsNodes[y])
      {
        if (--count > 0)
        {
          continue;
        }

        return L7_FALSE;
      }
    }
  }

  return L7_TRUE;
}
/* LVL7_P51286 end */

/*
 * chkIfIgnored
 *
 * Checks if the given node is to be ignored while parsing.
 * Returns L7_TRUE if node is is to be ignored.
 * Returns L7_FALSE if node is not to be ignored.
 *
 */
boolean chkIfIgnored(EwsCliCommandP * ignoreNodes, EwsCliCommandP mp)
{
  int z = 0;

  for(z = 0 ; ignoreNodes[z] != NULL && z < MAX_IGNORES_PER_NODE; z++)
  {
    if(mp == ignoreNodes[z])
    {
      return L7_TRUE;
    }
  }

  return L7_FALSE;
}

/*
 * chkDataType
 *
 * Checks data type of the node.
 * Returns L7_TRUE if the data matches the data type.
 * Returns L7_FALSE if the data does not matches the data type.
 *
 */
L7_BOOL chkDataType(EwsContext context, EwsCliNodeDataType dataType, char * buf, sintf * marker, char * dataTypeErrorMessage)
{
  L7_uint32 ipaddr;
  L7_enetMacAddr_t strMacAddr;
  L7_uint32 unit, slot, port, intIfNum;
  L7_INTF_MASK_t ifMask;
  L7_uint32 ifTypeMask;

#ifdef L7_IPV6_PACKAGE
  L7_in6_addr_t ip6addr;
  L7_uint32 prefixLen;
#endif

  *marker = -1;

  switch(dataType)
  {
  case L7_MAC_ADDRESS_SVL_DATA_TYPE:
    if (cliConvertMac((L7_uchar8 *)buf, strMacAddr.addr) != L7_TRUE)
    {
      osapiStrncat(dataTypeErrorMessage, pStrErr_common_UsrInputInvalidClientMacAddr, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
      osapiStrncat(dataTypeErrorMessage, pStrInfo_common_CrLf, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
      return L7_FALSE;
    }
    break;

  case L7_IP_ADDRESS_DATA_TYPE:
    if (usmDbInetAton((L7_uchar8 *)buf, &ipaddr) != L7_SUCCESS)
    {
      osapiStrncat(dataTypeErrorMessage, pStrErr_base_InvalidIpAddr_1, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
      osapiStrncat(dataTypeErrorMessage, pStrInfo_common_CrLf, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
      return L7_FALSE;
    }
    break;

#ifdef L7_IPV6_PACKAGE
  case L7_IPV6_ADDRESS_DATA_TYPE:
    if (osapiInetPton(L7_AF_INET6, (L7_uchar8 *)buf, (L7_uchar8 *)&ip6addr) != L7_SUCCESS)
    {
      osapiStrncat(dataTypeErrorMessage, pStrErr_base_InvalidIpv6Addr, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
      osapiStrncat(dataTypeErrorMessage, pStrInfo_common_CrLf, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
      return L7_FALSE;
    }
    break;
  case L7_IPV6_PREFIX_DATA_TYPE:
    if(cliValidPrefixPrefixLenCheck(buf, &ip6addr, &prefixLen) != L7_SUCCESS)
    {
      osapiStrncat(dataTypeErrorMessage, pStrErr_common_CfgIpv6InvalidPrefix, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
      osapiStrncat(dataTypeErrorMessage, pStrInfo_common_CrLf, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
      return L7_FALSE;
    }
    break;
#endif

  case L7_USP_DATA_TYPE:
    if (cliValidSpecificUSPCheck(buf, &unit, &slot, &port) != L7_SUCCESS)
    {
      osapiStrncat(dataTypeErrorMessage, pStrErr_base_InvalidSlotPort, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
      osapiStrncat(dataTypeErrorMessage, pStrInfo_common_CrLf, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
      return L7_FALSE;
    }
    else if (context->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfNum) != L7_SUCCESS)
      {
        osapiStrncat(dataTypeErrorMessage, pStrErr_base_IntfNotExisting, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
        osapiStrncat(dataTypeErrorMessage, pStrInfo_common_CrLf, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
        return L7_FALSE;
      }
    }
    break;

  case L7_USP_RANGE_DATA_TYPE:
    if (cliValidateEthernetRangeInput(buf) != L7_SUCCESS)
    {
      osapiStrncat(dataTypeErrorMessage, pStrErr_common_InvalidInterfaceRange, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
      osapiStrncat(dataTypeErrorMessage, pStrInfo_common_CrLf, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
      return L7_FALSE;
    }
    else if (context->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      memset(&ifMask, 0x00, sizeof(ifMask));
      if (cliParseEthernetRangeInput(buf, &ifMask, &ifTypeMask) != L7_SUCCESS)
      {
        osapiStrncat(dataTypeErrorMessage, pStrErr_common_InvalidInterfaceRange, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
        osapiStrncat(dataTypeErrorMessage, pStrInfo_common_CrLf, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1);
        return L7_FALSE;
      }
    } 
    break;

    /* add more data types here */

  case L7_NO_DATA_TYPE:
  default:
    return L7_TRUE;
    break;
  }

  return L7_TRUE;
}

/*
 * chkNodeLength
 *
 * Checks data type of the node.
 * Returns L7_TRUE if the data length is in the valid range.
 * Returns L7_FALSE if the data length is not in the valid range.
 * Returns L7_FALSE if the minLength or maxLength are invalid values.
 *
 */
L7_BOOL chkNodeLength(L7_ushort16 minLength, L7_ushort16 maxLength, char * buf, char * dataTypeErrorMessage)
{
  if (maxLength <= 0)
  {
    return L7_FALSE;
  }
  if (minLength > maxLength)
  {
    return L7_FALSE;
  }

  if ((strlen(buf) < minLength) ||
      (strlen(buf) > maxLength))
  {
    if (minLength == maxLength)
    {
      osapiSnprintf(dataTypeErrorMessage, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1, pStrErr_base_ParamLen, minLength);
    }
    else
    {
      osapiSnprintf(dataTypeErrorMessage, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1, pStrErr_base_ParamLen_1, minLength, maxLength);
    }
    return L7_FALSE;
  }
  return L7_TRUE;
}

/*
 * chkIntRange
 *
 * Checks data type of the node.
 * Returns L7_TRUE if the integer is in the valid range.
 * Returns L7_FALSE if the input is not an integer.
 * Returns L7_FALSE if the integer is not in the valid range.
 *
 */
L7_BOOL chkIntRange(EwsCliCommandP nodep, char * buf, char * dataTypeErrorMessage)
{
  L7_uint32 uintVal;
  L7_int32 intVal;

  if (cliCheckIfInteger(buf) != L7_SUCCESS)
  {
    if (nodep->dataType == L7_SIGNED_INTEGER)
    {
      osapiSnprintf(dataTypeErrorMessage, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1, pStrErr_base_ParamTypeInt,
                    nodep->minLength.min32, nodep->maxLength.max32);
    }
    else
    {
      osapiSnprintf(dataTypeErrorMessage, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1, pStrErr_base_ParamTypeUint32,
                    nodep->minLength.minu32, nodep->maxLength.maxu32);
    }
    return L7_FALSE;
  }

  if (nodep->dataType == L7_UNSIGNED_INTEGER)
  {
    if (cliConvertTo32BitUnsignedInteger(buf, &uintVal) != L7_SUCCESS)
    {
      osapiSnprintf(dataTypeErrorMessage, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1, pStrErr_base_ParamTypeUint32,
                    nodep->minLength.minu32, nodep->maxLength.maxu32);
      return L7_FALSE;
    }
    if (uintVal < nodep->minLength.minu32 || uintVal > nodep->maxLength.maxu32)
    {
      osapiSnprintf(dataTypeErrorMessage, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1, pStrErr_base_ParamRangeUint32,
                    nodep->minLength.minu32, nodep->maxLength.maxu32);
      return L7_FALSE;
    }
  }

  else if (nodep->dataType == L7_SIGNED_INTEGER)
  {
    if (cliConvertTo32BitSignedInteger(buf, &intVal) != L7_SUCCESS)
    {
      osapiSnprintf(dataTypeErrorMessage, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1, pStrErr_base_ParamTypeInt,
                    nodep->minLength.min32, nodep->maxLength.max32);
      return L7_FALSE;
    }
    if (intVal < nodep->minLength.min32 || intVal > nodep->maxLength.max32)
    {
      osapiSnprintf(dataTypeErrorMessage, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1, pStrErr_base_ParamRangeInt,
                    nodep->minLength.min32, nodep->maxLength.max32);
      return L7_FALSE;
    }
  }

  return L7_TRUE;
}

/*
 * chkNodeIntfSupported
 *
 * Checks to see if the current node is compatible with the
 *          interface being configured.
 * Returns L7_TRUE if the interface(s) match(es) one of the bits 
 *                 in the node's interface mask.
 * Returns L7_FALSE if no match or invalid interface.
 *
 */
L7_BOOL chkNodeIntfSupported(EwsCliCommandP np, EwsContext context)
{
  L7_uint32 valid_if_mask = 0;

  if (L7_VALIDATE_SCRIPT != context->scriptActionFlag)
  {
    if (L7_NODE_FLAGS_IS_PHYS_IF_VALID(np))
    {
      valid_if_mask |= USM_PHYSICAL_INTF;
    }
    if (L7_NODE_FLAGS_IS_LAG_IF_VALID(np))
    {
      valid_if_mask |= USM_LAG_INTF;
    }
    if (L7_NODE_FLAGS_IS_RTR_IF_VALID(np))
    {
      valid_if_mask |= USM_ROUTER_INTF;
    }
    if (L7_NODE_FLAGS_IS_LOG_VLAN_IF_VALID(np))
    {
      valid_if_mask |= USM_LOGICAL_VLAN_INTF;
    }

    if (valid_if_mask)
    {
      if (!(EWSINTFTYPEMASK(context) & valid_if_mask))
      {
        return L7_FALSE;
      }
    }
  }


  return L7_TRUE;
}

/*
 * ewsCliSearchChilds
 * Walk the parse tree, looking for unambiguous partial matches.
 *
 *
 * Follows parse tree looking for unambiguous matches to arguments and
 * saves traversed nodes in array.
 * Recursively calls itself to look for childs of option nodes.
 */
void ewsCliSearchChilds( EwsContext context,
                        EwsCliState cli,
                        EwsCliCommandP parent,
                        EwsCliCommandP * np_match,
                        EwsCliCommandP * np_optMatch,
                        char * searchCommand,
                        boolean * ambiguous,
                        sintf * marker,
                        uintf * token,
                        EwsCliCommandP * ignoreNodes,
                        boolean * dataTypeError,
                        char * dataTypeErrorMessage)
{
  sintf location = -1;
  uintf i = 0;
  EwsCliCommandP np;
  EwsCliCommandP np_firstchild = NULL;
  boolean bestMatch = FALSE;
  char * cp1 = NULL;
  char * cp2 = NULL;
  char * cpNum1, * cpNum2;
  char num1[32], num2[32];
  uintf copyCount;
  boolean next = FALSE;               /* LVL7_P0006 */
  boolean isSigned = TRUE;
  boolean f_child;
  boolean hideNode;
  signed long long minRange, maxRange, minVal;
  unsigned long long minuRange, maxuRange, minuVal;
  boolean dataError = FALSE;

  char l7_buf[L7_CLI_MAX_COMMAND_LENGTH];            /* LVL7_P0006, LVL7_P2093 */

  /*
   * Check each sibling for match
   */
  f_child = TRUE;
  bestMatch = FALSE;

  np=NULL;
  if(parent != NULL)
  {
    np=parent->first_child;
  }

  do
  {
    if(np == NULL)
    {
      break;
    }

    location =-1;
    next = FALSE;

    if (f_child == TRUE)
    {
      f_child = FALSE;
      np_firstchild = np;
    }

    /*check / change mp to point to node haveing no form*/
    if(context->commType ==  CLI_NO_CMD && np->noFormStatus == L7_STATUS_NORMAL_ONLY)
    {
      np = (EwsCliCommandP) EWS_LINK_NEXT(np->siblings);
      continue;
    }

    /*Check for special node conditions*/
    /*check np for noForm of node for normal command */
    if((context->commType ==  CLI_NORMAL_CMD && np->noFormStatus == L7_STATUS_NO_ONLY))
    {
      np = (EwsCliCommandP) EWS_LINK_NEXT(np->siblings);
      continue;
    }

    /*check if the node is already used till this point*/
    if (chkIfAlreadyParsed(cli, np) == L7_TRUE
        && chkIfStillDisplayable(cli, np) == L7_FALSE)  /* LVL7_P51286 */
    {
      np = (EwsCliCommandP) EWS_LINK_NEXT(np->siblings);
      continue;
    }

    /*check if the node is to be ignored*/
    if (chkIfIgnored(ignoreNodes, np) == L7_TRUE)
    {
      np = (EwsCliCommandP) EWS_LINK_NEXT(np->siblings);
      continue;
    }

    /*check if the node is available for processing*/
    if (np->componentId > L7_FIRST_COMPONENT_ID && usmDbComponentPresentCheck(cliGetUnitId(), np->componentId) != L7_TRUE)
    {
      np = (EwsCliCommandP) EWS_LINK_NEXT(np->siblings);
      continue;
    }
    if (np->componentId > L7_FIRST_COMPONENT_ID && np->featureId > 0 &&
        usmDbFeaturePresentCheck(cliGetUnitId(), np->componentId, np->featureId) != L7_TRUE)
    {
      np = (EwsCliCommandP) EWS_LINK_NEXT(np->siblings);
      continue;
    }

    /*check if the l7support node is available for processing*/

    /* The hiding of this tree is analogous to the hiding a keyed feature.
       However, instead of invoking usmDbComponentPresentCheck(), a different routine
       is required because the displaying of this tree is on a per-session basis.

     */
    hideNode = FALSE;
    if (L7_NODE_FLAGS_IS_SUPPORT(np))
    {
      if (ewsCliTreeHiddenDebugSupportFunctionIsEnabled() != TRUE)
      {
        hideNode = TRUE;
      }
    }

    if (chkNodeIntfSupported(np, context) == L7_FALSE)
    {
      hideNode = TRUE;
    }

    if (hideNode == TRUE)
    {
      np = (EwsCliCommandP) EWS_LINK_NEXT(np->siblings);
      if(np == np_firstchild)
      {
        break;
      }
      else
      {
        continue;
      }
    }

    /*
     * Compare argument with node token
     */
    for (strncpy(l7_buf,np->command, L7_CLI_MAX_COMMAND_LENGTH), l7_buf[strlen(l7_buf)-1] = 0, cp1 = l7_buf, cp2 = searchCommand, next = FALSE;
         *cp1 != '\0' && *cp2 != '\0';
         cp1++, cp2++)
    {
      if (cp1[0] == '<' || cp1[0] == '[')    /* LVL7_P0006 start */
      {
        if( strncmp(cp1,pStrInfo_common_Cr, 4) == 0)
        {
          break;
        }
        if( cp1[1] != '-')
        {
          isSigned = FALSE;
          cpNum1 = strstr(cp1+1,"-");
        }
        else
        {
          isSigned = TRUE;
          cpNum1 = strstr(cp1+2,"-");
        }
        memset (num1, 0, sizeof(num1));
        memset (num2, 0, sizeof(num2));
        if( cpNum1 == NULL)
        {
          bestMatch = TRUE;
          *marker = -1;
          break;
        }
        copyCount = (cpNum1 - cp1);
        if (copyCount > sizeof(num1))
        {
          copyCount = sizeof(num1);
        }
        osapiStrncpySafe(num1, cp1+1, copyCount);
        cpNum1 = cpNum1+1;
        cpNum2 = strstr(cpNum1,">");
        if( cpNum2 == NULL)
        {
          bestMatch = TRUE;
          *marker = -1;
          break;
        }

        copyCount = (cpNum2-cpNum1+1);
        if (copyCount > sizeof(num2))
        {
          copyCount = sizeof(num2);
        }
        osapiStrncpySafe(num2, cpNum1, copyCount);

        if(cliCheckIfInteger(num1) != L7_SUCCESS || cliCheckIfInteger(num2) != L7_SUCCESS)
        {
          bestMatch = TRUE;
          *marker = -1;
          break;
        }
        else
        {
          if ( ((isSigned == TRUE) && (cliConvertTo64BitSignedInteger(num1, &minRange)!= L7_SUCCESS || cliConvertTo64BitSignedInteger(num2, &maxRange) != L7_SUCCESS)) || 
               ((isSigned != TRUE ) && (cliConvertTo64BitUnsignedInteger(num1, &minuRange)!= L7_SUCCESS || cliConvertTo64BitUnsignedInteger(num2, &maxuRange) != L7_SUCCESS)))
          {
            bestMatch = TRUE;
            *marker = -1;
            break;
          }

          if ( cliCheckIfInteger(cp2) != L7_SUCCESS)
          {
            if (isSigned == L7_TRUE)
            {
              osapiSnprintf(dataTypeErrorMessage, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1, pStrErr_base_ParamTypeInt_1, minRange, maxRange);
            }
            else
            {
              osapiSnprintf(dataTypeErrorMessage, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1, pStrErr_base_ParamTypeUint, minuRange, maxuRange);
            }
            *marker = 0;
            /*This is the case when user doesn't enter the proper number*/
            break;
          }
          else
          {
            if ( isSigned == TRUE )
            {
              if ( cliConvertTo64BitSignedInteger(cp2, &minVal) != L7_SUCCESS )
              {
                osapiSnprintf(dataTypeErrorMessage, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1, pStrErr_base_ParamTypeInt_1, minRange, maxRange);
                *marker = 0;
                break;
              }
            }
            else
            {
              if ( cliConvertTo64BitUnsignedInteger(cp2, &minuVal) != L7_SUCCESS )
              {
                osapiSnprintf(dataTypeErrorMessage, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1, pStrErr_base_ParamTypeUint, minuRange, maxuRange);
                *marker = 0;
                break;
              }
            }

            if ( ( (isSigned == L7_TRUE)  && (minVal >= minRange) && (minVal <= maxRange)) || (
                                                                                              isSigned == L7_FALSE && minuVal >= minuRange && minuVal <= maxuRange ))
            {
              next = TRUE;
              *marker = -1;
              break;
            }
            else
            {
              if (isSigned == L7_TRUE)
              {
                osapiSnprintf(dataTypeErrorMessage, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1, pStrErr_base_ParamRangeInt_1, minRange, maxRange);
              }
              else
              {
                osapiSnprintf(dataTypeErrorMessage, L7_MAX_DATA_TYPE_ERROR_MESSAGE-1, pStrErr_base_ParamRangeUint, minuRange, maxuRange);
              }
              *marker = 0;
              break;
            }
          }
        }
      }
      location = location +1;

      if (tolower(*cp1) != tolower(*cp2))   /* LVL7_P0006 end */
      {
        if(*marker <location)
        {
          *marker = location;
        }
        break;
      }
    }

    /*
     * If the entire argument matched all of the token, then we have
     * a complete match.
     */
    if ((*cp2 == '\0' && *cp1 == '\0') || next)  /* LVL7_P0006 */
    {
      *np_match = np;
      *marker = -1;
      *ambiguous = TRUE;
      break;
    }
    else if(bestMatch == TRUE)
    {
      bestMatch = FALSE;
      *marker = -1;

      if(*np_optMatch == NULL)  /* if we've already matched a datatype, no need to test further datatypes */
      {
        /*Check for node length*/
        if (np->dataType == L7_STRING_DATA_TYPE && (np->minLength.minu32 > 0 || np->maxLength.maxu32 > 0))
        {
          if (chkNodeLength(np->minLength.minu32, np->maxLength.maxu32, searchCommand, dataTypeErrorMessage) == L7_FALSE)
          {
            dataError = TRUE;
          }
          else
          {
            *np_optMatch = np;
          }
        }
        else if (np->dataType == L7_SIGNED_INTEGER || np->dataType == L7_UNSIGNED_INTEGER)
        {
          if (chkIntRange(np, searchCommand, dataTypeErrorMessage) == L7_FALSE)
          {
            dataError = TRUE;
          }
          else
          {
            *np_optMatch = np;
          }
        }

        /*Check for node data type*/
        else if(np->dataType > L7_NO_DATA_TYPE && cli->dataCheckDisable == L7_FALSE)
        {
          if( chkDataType(context, np->dataType, searchCommand, marker, dataTypeErrorMessage) == L7_FALSE)
          {
            dataError = TRUE;
          }
          else
          {
            *np_optMatch = np;
          }
        }
        /* if there's a sibling node without a datatype assigned, must let the command function handle it */
        else if (np->dataType == L7_NO_DATA_TYPE)
        {
          *dataTypeError = L7_FALSE;
          dataError = FALSE;
          *np_optMatch = np;
        }
      }

      /* clear the flag if there is no datatype error */
      if (dataError == FALSE)
      {
        *dataTypeError = L7_FALSE;
      }

      if(*np_optMatch == NULL)  /*ignore best match if already matched to some node*/
      {
        if(*dataTypeError == L7_FALSE && dataError == TRUE)
        {
          *dataTypeError = L7_TRUE;
        }

        if(*dataTypeError == L7_FALSE)
        {
          *np_optMatch = np;
          *ambiguous = TRUE;
        }
      }

      else
      {
        if (dataError == FALSE)
        {
          *np_optMatch = np;
          *ambiguous = TRUE;
        }
      }
    }

    /*
     * If the entire argument matched some of the token,
     * then we have a partial match.
     */
    else if (*cp2 == '\0')
    {

      /*
       * If we have no match yet, then we are not yet ambiguous.
       * Remember the match and keep scanning to make sure the
       * match is unambiguous across the sibling nodes.
       */
      if (*ambiguous)
      {
        *ambiguous = FALSE;
        *np_match = np;
      }

      /*
       * Otherwise, we have ambiguity. Clear the ambiguous match.
       */
      else
      {
        *np_match = NULL;
      }
    }
    else if( *cp1 ==  '\0')
    {
      *marker = location +1;
      *ambiguous = TRUE;
    }
    /*
     * Next sibling
     */
    np = (EwsCliCommandP) EWS_LINK_NEXT(np->siblings);

  } while (np_firstchild != np);

  for(i = 0 ; (*np_match == NULL || *ambiguous == FALSE) && 
  	  ((parent != NULL) && (parent->optionNodes[i] != NULL)) && 
  	   i < MAX_OPTIONS_PER_NODE; i++)
  {
    ewsCliSearchChilds( context,
                       cli,
                       parent->optionNodes[i],
                       np_match,
                       np_optMatch,
                       searchCommand,
                       ambiguous,
                       marker,
                       token,
                       ignoreNodes,
                       dataTypeError,
                       dataTypeErrorMessage);
  }
}

/*
 * ewsCliParseTree
 * Write the node pointers into array.
 *
 *   cli          - CLI state
 *   root         - root of parse tree
 *
 */
boolean ewsCliParseTree (EwsContext context, EwsCliState cli, EwsCliCommandP root,
                         sintf * marker, uintf * token, char * dataTypeErrorMessage)
{
  uintf i;
  EwsCliCommandP np;
  EwsCliCommandP np_match = NULL;
  EwsCliCommandP np_optMatch = NULL;
  boolean ambiguous = TRUE;
  boolean dataTypeError = L7_FALSE;

  /*
   * Starting with root of parse tree, begin matching arguments
   */
  np = root;

  /*initialise the variables*/
  cli->parseCount = 0;
  *token =0;
  dataTypeError = L7_FALSE;
  memset (dataTypeErrorMessage, 0, L7_MAX_DATA_TYPE_ERROR_MESSAGE);
  for (i = 0; i < EWS_CLI_MAX_ARGS; i++)
  {
    cli->parseList[i] = NULL;
  }

  /*loop thorugh the given tokens to fill parseList*/
  for (i = 0; i < cli->argc && np != NULL; i++)
  {
    /*initialise the variables*/
    np_match = NULL;
    np_optMatch = NULL;

    *marker = -1;

    dataTypeError = L7_FALSE;
    memset (dataTypeErrorMessage, 0, L7_MAX_DATA_TYPE_ERROR_MESSAGE);
    /*search for best / exact match of child*/
    ewsCliSearchChilds( context,
                       cli,
                       np,
                       &np_match,
                       &np_optMatch,
                       cli->argv[i],
                       &ambiguous,
                       marker,
                       token,
                       np->ignoreNodes,
                       &dataTypeError,
                       dataTypeErrorMessage);

    /*
     * If no match was found, we can't parse anymore.
     */
    if (np_match == NULL )
    {
      if( np_optMatch != NULL)
      {
        np_match = np_optMatch;
      }
      else
      {
        if( *marker == -1)
        {
          *marker= 0;
        }
        break;
      }
    }

    *marker = -1;
    *token = *token+1;

    /*
     * Save match and continue parsing arguments
     */
    cli->parseList[i] = np_match;
    cli->parseCount++;
    np = np_match;
    ambiguous = TRUE;
  }

  if( ambiguous == FALSE && np_match == NULL)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*
 * ewsCliNodeHasCRChild
 * determines if the children of this node, or any children
 * of the options nodes, are <cr>.
 *
 */
void ewsCliNodeHasCRChild(EwsCliCommandP startSearchNodeP, EwsContext context, boolean * foundCr, boolean * hasChild)
{
  EwsCliCommandP searchNodeP = NULL;
  EwsCliCommandP firstChild = NULL;
  boolean foundChild = FALSE;
  sintf i;

  if ((startSearchNodeP == L7_NULLPTR) || (context == L7_NULLPTR) || 
      (foundCr == L7_NULLPTR) || (hasChild == L7_NULLPTR))
  {
    if (foundCr != L7_NULLPTR)
    {
      *foundCr = FALSE;
    }
    if (hasChild != L7_NULLPTR)
    {
      *hasChild = FALSE;
    }
    return;
  }

  if ((L7_NODE_FLAGS_IS_EXEC_NO(startSearchNodeP)) &&
      (startSearchNodeP->noFormStatus != L7_STATUS_NORMAL_ONLY) &&
      (context->commType == CLI_NO_CMD))
  {
    *foundCr = TRUE;
  }
  else if ((L7_NODE_FLAGS_IS_EXEC_NORMAL(startSearchNodeP)) &&
           (startSearchNodeP->noFormStatus != L7_STATUS_NO_ONLY) &&
           (context->commType == CLI_NORMAL_CMD))
  {
    *foundCr = TRUE;
  }

  searchNodeP = startSearchNodeP->first_child;
  firstChild = searchNodeP;
  do
  {
    foundChild = FALSE;
    if (searchNodeP == NULL || searchNodeP->command == NULL)
    {
      break;
    }
    if (((searchNodeP->noFormStatus != L7_STATUS_NO_ONLY) &&
         (context->commType == CLI_NORMAL_CMD))||
        ((searchNodeP->noFormStatus != L7_STATUS_NORMAL_ONLY) &&
         (context->commType == CLI_NO_CMD)))
    {
      *hasChild = TRUE;
      foundChild = TRUE;
    }
    searchNodeP = (EwsCliCommandP) EWS_LINK_NEXT(searchNodeP->siblings);
  }
  while (searchNodeP != NULL && searchNodeP != firstChild);

  /* if we still haven't found <cr> look in the optionNode child list */
  if (*foundCr == FALSE)
  {
    searchNodeP = startSearchNodeP;
    for (i = 0 ; searchNodeP->optionNodes[i] != NULL && i < MAX_OPTIONS_PER_NODE; i++)
    {
      ewsCliNodeHasCRChild(searchNodeP->optionNodes[i], context, foundCr, hasChild);
      if (*foundCr == TRUE)
      {
        return;
      }
    }
  }
}

/*
 * ewsCliSearchPossible
 * Follows the parse tree to find and print
 * possible nodes at next level
 *
 *
 * Returns text of cli prompt (if any)
 */
const char * ewsCliSearchPossible(
  EwsContext context,
  char * datap,
  EwsCliState cli,
  EwsCliCommandP parent,
  char * searchCommand,
  boolean full_help,
  const char * * token,
  const char * * token_remaining,
  const char * * ambiguous_token,
  const char * * ambiguous_remaining,
  uintf * ambiguous_overlap,
  boolean * help_printed,
  EwsCliCommandP * ignoreNodes
)
{
  char * cp = NULL;
  const char * cp2 = NULL;
  uintf i;
  EwsCliCommandP mp = NULL;
  uintf column = 0;
  boolean firstchild;
  boolean hideNode;
  EwsCliCommandP mp_firstchild = NULL;
  boolean foundCr = FALSE;
  boolean hasChild = FALSE;

  /*Initialise mp*/
  mp = NULL;

  if( parent != NULL)
  {
    mp = parent->first_child;
  }
  else
  {
    return NULL;
  }

  /*
   * Tree CLI: loop through siblings
   */
  firstchild = TRUE;

  do
  {
    /* print the <cr> node as a possible option, if necessary */
    if (firstchild == TRUE)
    {
      foundCr = FALSE;
      hasChild = FALSE;
      ewsCliNodeHasCRChild(parent, context, &foundCr, &hasChild);
      if (*datap == EWS_CLI_CHAR_POSSIBLE && full_help == TRUE &&
          foundCr == TRUE && *help_printed == FALSE)
      {
        ewsTelnetWrite(context,pStrInfo_common_CrLf);
        ewsCliPossible(context
                       ,TRUE
                       ,pStrInfo_common_Cr
                       ,pStrInfo_common_NewLine
                       ,&column
                       ,parent->cmdPad);
        *help_printed = TRUE;
      }
    }
    /*
     * If we have no siblings, then break out now
     */
    if( mp == NULL)
    {
      break;
    }

    if (firstchild)
    {
      firstchild = FALSE;
      mp_firstchild = mp;
    }

    /*check / change mp to point to node haveing no form*/
    if(context->commType ==  CLI_NO_CMD && mp->noFormStatus == L7_STATUS_NORMAL_ONLY)
    {
      mp = (EwsCliCommandP) EWS_LINK_NEXT(mp->siblings);
      continue;
    }

    /*Check for no form status of node*/
    /*check mp for noForm of node for normal command */
    if(context->commType ==  CLI_NORMAL_CMD && mp->noFormStatus == L7_STATUS_NO_ONLY)
    {
      mp = (EwsCliCommandP) EWS_LINK_NEXT(mp->siblings);
      continue;
    }

    /*check if the node is already used till this point*/
    if (chkIfUsed(cli, mp) == L7_TRUE
        && chkIfStillDisplayable(cli, mp) == L7_FALSE)  /* LVL7_P51286 */
    {
      mp = (EwsCliCommandP) EWS_LINK_NEXT(mp->siblings);
      continue;
    }

    /*check if the node is to ignored*/
    if (chkIfIgnored(ignoreNodes, mp) == L7_TRUE)
    {
      mp = (EwsCliCommandP) EWS_LINK_NEXT(mp->siblings);
      continue;
    }

    /*check for advance key restricted node*/
    if (mp->componentId > L7_FIRST_COMPONENT_ID && usmDbComponentPresentCheck(cliGetUnitId(), mp->componentId) != L7_TRUE)
    {
      mp = (EwsCliCommandP) EWS_LINK_NEXT(mp->siblings);
      continue;
    }
    if (mp->componentId > L7_FIRST_COMPONENT_ID && mp->featureId > 0 &&
        usmDbFeaturePresentCheck(cliGetUnitId(), mp->componentId, mp->featureId) != L7_TRUE)
    {
      mp = (EwsCliCommandP) EWS_LINK_NEXT(mp->siblings);
      continue;
    }

    /*check if the l7support node is available for processing*/

    /* The hiding of this tree is analogous to the hiding a keyed feature.
       However, instead of invoking usmDbComponentPresentCheck(), a different routine
       is required because the displaying of this tree is on a per-session basis.

     */
    hideNode = FALSE;
    if (L7_NODE_FLAGS_IS_SUPPORT(mp))
    {
      if (ewsCliTreeHiddenDebugSupportFunctionIsEnabled() != TRUE)
      {
        hideNode = TRUE;
      }
    }

    if (chkNodeIntfSupported(mp, context) == L7_FALSE)
    {
      hideNode = TRUE;
    }

    if (hideNode == TRUE)
    {
      mp = (EwsCliCommandP) EWS_LINK_NEXT(mp->siblings);
      if(mp == mp_firstchild) /*if(np == f_child)*/
      {
        break;
      }
      else
      {
        continue;
      }
    }

    /*
     * Find the best matching command
     */
    if (cli->argc == 0)
    {
      cp = (char *)"";
    }
    else
    {
      cp = searchCommand;
    }

    cp2 = mp->command;

    while (tolower(*cp) == tolower(*cp2) && *cp != '\0')      /*lvl7_P0006*/
    {
      cp++;
      cp2++;
    }
    if(( *cp == '\0') && (*cp2 == '\0' || *cp2 == ' ') && *datap == EWS_CLI_CHAR_SPACE)
    {
      cp = &cli->line[cli->historyIndex][cli->lineIndex];
      EMWEB_MEMCPY(cp
                   ,"  "
                   ,1);
      cli->lineIndex += 1;
      cli->line[cli->historyIndex][cli->lineIndex] = '\0';
      return cp;
    }

    /*
     * We have at least a partial match
     */
    if (*cp == '\0')
    {
      /*
       * If we are looking for possible matches, then we've found
       * one.
       */
      if (*datap == EWS_CLI_CHAR_POSSIBLE)
      {
        if(FALSE == *help_printed)
        {
          ewsTelnetWrite(context,pStrInfo_common_CrLf);
        }
        ewsCliPossible(context, full_help, mp->command, mp->description, &column, parent->cmdPad);
        mp = (EwsCliCommandP) EWS_LINK_NEXT(mp->siblings);
        *help_printed = TRUE;
        continue;
      }

      /*
       * If we already found a partial match, then there are
       * multiple (ambiguous) choices.  If the choices all have
       * unmatched characters in common, then command completion
       * can complete these characters up to the next point of
       * differentiation.
       */
      if (*token != NULL)
      {
        *ambiguous_token = *token;
        *ambiguous_remaining = *token_remaining;
      }

      /*
       * Save partial match and keep looking
       */
      *token = mp->command;
      *token_remaining = cp2;
      mp = (EwsCliCommandP) EWS_LINK_NEXT(mp->siblings);

      /*
       * If ambiguous, determine how much common overlap there
       * is, and track the smallest overlap across all matching
       * possibilities.
       */
      if (*ambiguous_token != NULL)
      {
        for (i = 0; *(*ambiguous_remaining)++ == *cp2++; i++)
        {
        }
        if (i < *ambiguous_overlap)
        {
          *ambiguous_overlap = i;
        }
      }
    }
    /*
     * Otherwise we don't have a match, so keep scanning possibles
     */
    else
    {
      mp = (EwsCliCommandP) EWS_LINK_NEXT(mp->siblings);
    }
  }
  while (mp_firstchild != mp);

  for(i = 0 ; parent->optionNodes[i] != NULL && i < MAX_OPTIONS_PER_NODE; i++)
  {
    cp = (char *) ewsCliSearchPossible( context,
                                       datap,
                                       cli,
                                       parent->optionNodes[i],
                                       searchCommand,
                                       full_help,
                                       token,
                                       token_remaining,
                                       ambiguous_token,
                                       ambiguous_remaining,
                                       ambiguous_overlap,
                                       help_printed,
                                       ignoreNodes);

    if(cp != NULL)
    {
      return cp;
    }
  }

  return NULL;
}

/*
 * ewsCliData
 * Send data to CLI parser
 *
 *   context  - context of request
 *   datap    - data from telnet
 *   bytesp   - (input/output) bytes available
 *   menu     - array of currently valid CLI Commands
 *
 * Returns text to be returned from ewsTelnetData().
 */
const char *
ewsCliData ( EwsContext context
            ,char * datap
            ,uintf * bytesp
            ,EwsCliCommandP menu
)
{
  char * ret;
  static char ccc[2];
  static L7_BOOL upArrowFlag = L7_FALSE;
  EwsCliState cli = &context->telnet->cli;
  char * cp = NULL;
  char * tmp;
  char charBuffer[2];    /* LVL7_P0006 */
  const char * cp2 = NULL;
  const char * token;
  const char * token_remaining;
  const char * ambiguous_token;
  const char * ambiguous_remaining;
  uintf ambiguous_overlap;
  sintf i;
  EwsCliCommandP mp = NULL;
  const char * syntaxMsg = NULL;
  boolean full_help = FALSE;
  uintf column = 0;
  uintf j = 0;
  boolean ambiguous = TRUE;
  uintf deletedBufferLen;
  uintf commandLen;
  uintf tokens ;
  sintf marker;
  uintf hiddTokens ;
  sintf hiddMarker;
  sintf diff;
  boolean hiddAmbiguous = TRUE;
  boolean commandHandle = FALSE;
  boolean help_printed = FALSE;
  L7_uint32 newUnit;
  char lastToken[L7_CLI_MAX_COMMAND_LENGTH];        /* last command line parameter */
  char dataTypeErrorMessage[L7_MAX_DATA_TYPE_ERROR_MESSAGE];
  boolean data_error_printed = FALSE;
  L7_uint32 entryIndex;
  L7_BOOL addCmdLoggerEntry;
  L7_uint32 strlenArg, strlenNodeName,prevArgsLen = 0,l,m;
  static char log_buf[EWS_CLI_MAX_LINE];
  sintf bk_space_counter = 0;
  boolean foundCr = FALSE;
  boolean hasChild = FALSE;

  /*initialize the string*/
  memset (dataTypeErrorMessage, 0, L7_MAX_DATA_TYPE_ERROR_MESSAGE);
  memset (log_buf, 0, EWS_CLI_MAX_LINE);
  /*
   * By default, we will read one character at a time
   */
  *bytesp = *bytesp - 1;

  if (cli->arrow_lb)
  {
    if (*datap == EWS_CLI_CHAR_A_UP)
    {
      *datap = EWS_CLI_CHAR_UP;
    }
    else if (*datap == EWS_CLI_CHAR_B_DOWN)
    {
      *datap = EWS_CLI_CHAR_DOWN;
    }
    else if (*datap == EWS_CLI_CHAR_C_RIGHT)
    {
      *datap = EWS_CLI_CHAR_FORWARD;
    }
    else if (*datap == EWS_CLI_CHAR_D_LEFT)
    {
      *datap = EWS_CLI_CHAR_BACKWARD;
    }
    cli->arrow_lb = FALSE;
    cli->arrow_esc = FALSE;
  }

  /*
   * Switch on current character
   */
  switch (*datap)
  {
    /*lvl7_P0006 start*/
    /*********************************************************************
     * Edit: go to root
     */
  case EWS_CLI_CHAR_ROOT:
    cli->depth = 0;
    cli->line[cli->historyIndex][0] = '\0';
    cli->line[cli->historyIndex][EWS_CLI_MAX_LINE - 1] = '\0';
    cli->lineIndex = 0;
    cliPrevDepthSet(0);
    ewsTelnetWrite(context, cliSetRootMode());
    ewsFlushAll(context);
    break;

    /*lvl7_P0006 end*/

    /*********************************************************************
     * Edit: get ESC keycode
     */
  case EWS_CLI_CHAR_ESC:
    cli->arrow_esc = TRUE;
    break;

    /*********************************************************************
     * Edit: go to beginning of line
     */
  case EWS_CLI_CHAR_BEGIN_LINE:
    while (cli->lineIndex > 0)
    {
      ewsTelnetWrite(context, "\b");
      cli->lineIndex--;
    }
    break;

    /*********************************************************************
     * Edit: go to end of line
     */
  case EWS_CLI_CHAR_END_LINE:
    cp = &cli->line[cli->historyIndex][cli->lineIndex];
    while (cli->line[cli->historyIndex][cli->lineIndex] != '\0')
    {
      cli->lineIndex++;
    }
    return cp;

    /*********************************************************************
     * Edit: forward character
     */
  case EWS_CLI_CHAR_FORWARD:
    ccc[0] = cli->line[cli->historyIndex][cli->lineIndex];
    if (ccc[0] != '\0')
    {
      cli->lineIndex++;
    }
    return ccc;

    /*********************************************************************
     * Edit: backward character
     */
  case EWS_CLI_CHAR_BACKWARD:
    if (cli->lineIndex != 0)
    {
      cli->lineIndex--;
      ccc[0] = '\b';
      return ccc;
    }
    break;

    /*********************************************************************
     * Edit: delete backwards
     */
  case EWS_CLI_CHAR_DEL_BACK0:
  case EWS_CLI_CHAR_DEL_BACK1:
    if (cli->lineIndex == 0)
    {
      break;
    }
    cli->lineIndex--;
    ewsTelnetWrite(context, "\b");

    /* no break -- follow through to delete */

    /*********************************************************************
     * Edit: delete character
     */
  case EWS_CLI_CHAR_DELETE:
    cli->historySearch = cli->historyBottom;

    /*
     * If end-of-line, then nothing to do
     */
    if (cli->line[cli->historyIndex][cli->lineIndex] == '\0')
    {
      break;
    }

    /*
     * Move characters down in line
     */
    for (i = cli->lineIndex;
         cli->line[cli->historyIndex][i] != '\0';
         i++)
    {
      /*
       * Save char at lineIndex to buffer
       */
      if(i == cli->lineIndex)
      {
        ewsCliSaveChar(cli,cli->line[cli->historyIndex][i], TRUE);
      }
      cli->line[cli->historyIndex][i] =
        cli->line[cli->historyIndex][i+1];
    }

    /*
     * Write remaining line (and blank) to screen
     */
    cp = &cli->line[cli->historyIndex][cli->lineIndex];
    ewsTelnetWrite(context, cp);
    ewsTelnetWrite(context, " ");

    /*
     * Backup curser
     */
    while (i >= cli->lineIndex + 1)
    {
      ewsTelnetWrite(context, "\b");
      i--;
    }
    break;

    /*********************************************************************
     * Edit: Kill Line Backward
     */
  case EWS_CLI_CHAR_KILL0:
  case EWS_CLI_CHAR_KILL1:
    cli->historySearch = cli->historyBottom;
    cp = &cli->line[cli->historyIndex][cli->lineIndex];
    ewsCliSaveLine(cli, cli->line[cli->historyIndex], cli->lineIndex);
    ewsCliKillLine(context, cli);
    EMWEB_STRLEN(i, cp);
    EMWEB_STRCPY(cli->line[cli->historyIndex], cp);
    cli->lineIndex = i;
    cli->line[cli->historyIndex][i] = '\0';
    ewsTelnetWrite(context, cli->line[cli->historyIndex]);
    for (; cli->lineIndex > 0; cli->lineIndex--)
    {
      ewsTelnetWrite(context, "\b");
    }
    break;

    /*********************************************************************
     * Edit: Kill Line Forward
     */
  case EWS_CLI_CHAR_KILL_END:
    cli->historySearch = cli->historyBottom;
    cp = &cli->line[cli->historyIndex][cli->lineIndex];
    EMWEB_STRLEN(i, cp);
    /*  Initialize deleted buffer for first char only*/
    ewsCliSaveLine(cli, cp,i);
    for (i = 0; cp[i] != '\0'; i++)
    {
      *cp = '\0';
      ewsTelnetWrite(context, " ");
    }
    for (; i > 0;  i--)
    {
      ewsTelnetWrite(context, "\b");
    }
    break;

    /*********************************************************************
     * Edit: Kill Word Backward
     */
  case EWS_CLI_CHAR_KILL_WORD:
    cli->historySearch = cli->historyBottom;
    i = cli->lineIndex;
    cp = &cli->line[cli->historyIndex][i];
    cp2 = (const char *) cp;
    ewsCliKillLine(context, cli);
    cli->lineIndex = i;
    memset ( cli->deletedBuffer, 0,sizeof(cli->deletedBuffer));
    while (*cp == ' ' && cli->lineIndex > 0)
    {
      cli->lineIndex--;
      cp--;
    }
    while (*cp != ' ' && cli->lineIndex > 0)
    {
      cli->lineIndex--;
      cp--;
    }
    ewsCliSaveLine(cli, cp, i - cli->lineIndex);
    EMWEB_STRCPY(cp, cp2);
    EMWEB_STRLEN(i, cli->line[cli->historyIndex]);
    cli->line[cli->historyIndex][i] = '\0';
    ewsTelnetWrite(context, cli->line[cli->historyIndex]);
    for (; i > cli->lineIndex; i--)
    {
      ewsTelnetWrite(context, "\b");
    }
    break;

    /*********************************************************************
     * Edit: Transpose Characters
     */
  case EWS_CLI_CHAR_TRANSPOSE:
    cli->historySearch = cli->historyBottom;
    cp = &cli->line[cli->historyIndex][cli->lineIndex];
    if (*cp != '\0' && cli->lineIndex != 0)
    {
      char ctmp[3];
      ctmp[0] = cp[0];
      ctmp[1] = cp[-1];
      ctmp[2] = '\0';
      cp[0] = ctmp[1];
      cp[-1] = ctmp[0];
      ewsTelnetWrite(context, "\b");
      ewsTelnetWrite(context, ctmp);
      ewsTelnetWrite(context, "\b");
    }
    break;

    /*********************************************************************
     * Edit: History Up
     */
  case EWS_CLI_CHAR_UP:
    if (cli->historySearch != cli->historyTop)
    {
      ewsCliKillLine(context, cli);
      cli->historySearch = (cli->historySearch+EWS_CLI_MAX_HISTORY-1)
                           % EWS_CLI_MAX_HISTORY;
      upArrowFlag = L7_TRUE;
      memset ( cli->line[cli->historyIndex], 0,EWS_CLI_MAX_LINE);
      EMWEB_STRCPY(cli->line[cli->historyIndex]
                   ,cli->line[cli->historySearch]);
      EMWEB_STRLEN(cli->lineIndex, cli->line[cli->historyIndex]);
      cli->line[cli->historyIndex][cli->lineIndex] = '\0';
      return cli->line[cli->historyIndex];
    }
    break;

    /*********************************************************************
     * Edit: History Down
     */
  case EWS_CLI_CHAR_DOWN:
    if (cli->historySearch != cli->historyBottom)
    {
      ewsCliKillLine(context, cli);
      cli->historySearch = (cli->historySearch+1) % EWS_CLI_MAX_HISTORY;

      /*
       * Bottom of history is blank line
       */
      if (cli->historySearch == cli->historyBottom)
      {
        *cli->line[cli->historyIndex] = '\0';
        cli->lineIndex = 0;
        break;
      }
      memset ( cli->line[cli->historyIndex], 0,EWS_CLI_MAX_LINE);
      EMWEB_STRCPY(cli->line[cli->historyIndex]
                   ,cli->line[cli->historySearch]);
      EMWEB_STRLEN(cli->lineIndex, cli->line[cli->historyIndex]);
      cli->line[cli->historyIndex][cli->lineIndex] = '\0';
      return cli->line[cli->historyIndex];
    }
    break;
    /*********************************************************************
     * Edit: print recently deleted Line/Word/Char
     */
  case EWS_CLI_CHAR_PASTE_RECENTLY_DELETED_CHAR:
    /*
     * Print chars stored in deleted buffer
     */

    EMWEB_STRLEN(deletedBufferLen, cli->deletedBuffer);
    EMWEB_STRLEN(commandLen, cli->line[cli->historyIndex]);

    if(cli->lineIndex + deletedBufferLen > EWS_CLI_MAX_LINE)
    {
      deletedBufferLen = EWS_CLI_MAX_LINE - cli->lineIndex -1;
    }

    /*
     * Make room for chars in deleted buffer
     * Move each char in line to +offset deletedBufferLen
     */
    if( commandLen >0)
    {
      i = commandLen -1;
    }
    else
    {
      i =0;
    }

    for( ; i < cli->lineIndex ; i++)
    {
      if(i + deletedBufferLen <= EWS_CLI_MAX_LINE )
      {
        cli->line[cli->historyIndex][i+deletedBufferLen]=
          cli->line[cli->historyIndex][i];
      }
    }

    /*
     * Start filling up chars from deletedBuffer at lineIndex onward
     */
    for(i=0; i <deletedBufferLen; i++)
    {
      cli->line[cli->historyIndex][i + cli->lineIndex ]=
        cli->deletedBuffer[i];
    }

    i= cli->lineIndex + deletedBufferLen;
    ewsCliKillLine(context, cli);
    EMWEB_STRLEN(j, cli->line[cli->historyIndex]);
    cli->line[cli->historyIndex][j] = '\0';
    ewsTelnetWrite(context, cli->line[cli->historyIndex]);
    for(; i<j && cli->lineIndex>0; i++)
    {
      ewsTelnetWrite(context, "\b");
      cli->lineIndex--;
    }
    memset ( cli->deletedBuffer, 0,sizeof(cli->deletedBuffer));
    break;

    /*********************************************************************
     * Edit: Rewrite the line
     */
  case EWS_CLI_CHAR_PASTE:

    ewsCliKillLine(context, cli);

    /*
     * Rewrite line  to screen
     */
    EMWEB_STRLEN(cli->lineIndex, cli->line[cli->historyIndex]);
    cli->line[cli->historyIndex][cli->lineIndex] = '\0';
    return cli->line[cli->historyIndex];
    break;

    /*********************************************************************
     * Command completion
     */
  case EWS_CLI_CHAR_POSSIBLE:
  case EWS_CLI_CHAR_COMPLETE:
  case EWS_CLI_CHAR_SPACE:
    if (cliLoginSessionIndexGet() == -1)
    {
        if (*datap ==  EWS_CLI_CHAR_SPACE)
        {
          cli->line[cli->historyIndex][cli->lineIndex] = ' ';
          cli->lineIndex++;
          cli->line[cli->historyIndex][cli->lineIndex] = '\0';
          return " ";
        }
      break;
    }

    /* @lvl7 - if the char is a '?' then print it out */
    if (*datap == EWS_CLI_CHAR_POSSIBLE)
    {
      memset(charBuffer, 0, 2);
      strncpy(charBuffer, datap, 1);
      cliWrite(charBuffer);
    }      /* LVL7_P0006 end */

    /*
     * If editing line, first truncate to end of line
     */
    i = cli->lineIndex;

    /*initialise the variable */
    tmp = cli->line[cli->historyIndex] + i;

    memset (log_buf, 0,sizeof(EWS_CLI_MAX_LINE));
    /*copy the remaining part of command string*/
	osapiStrncpySafe(log_buf,(const char *) tmp, sizeof(log_buf));

    while (cli->line[cli->historyIndex][i] != '\0')
    {
      cli->line[cli->historyIndex][i++] = '\0';
      ewsTelnetWrite(context, " ");
    }
    while (i > cli->lineIndex)
    {
      ewsTelnetWrite(context, "\b");
      i--;
    }

    /*
     * Parse arguments
     */
    ewsCliParseLine( context, cli );

    /* LVL7_P5804 start */
    /*
     * If the first character in the argbuffer is a COMMENT_CHARACTER, then return
     * without parsing the tree
     */
    if (cli->arg_buffer[0] == EWS_CLI_COMMENT_CHAR)
    {
      cli->line[cli->historyIndex][cli->lineIndex] = ' ';
      cli->lineIndex++;
      cli->line[cli->historyIndex][cli->lineIndex] = '\0';
      return " ";
    }
    /* LVL7_P5804 end */
    /*
     * If the previous character is a space, then we are at the
     * beginning of an argument.  We want to search keywords for
     * completion instead of commands.
     */
    if ((cli->lineIndex != 0 &&
         cli->line[cli->historyIndex][cli->lineIndex - 1] == ' '
        ) ||
        (cli->lineIndex == 0 &&
         cli->depth > 0
        ))
    {
      if (*datap == EWS_CLI_CHAR_SPACE)
      {
        goto just_a_space;
      }
      cli->argc++;
      full_help = TRUE;
    }
    if (cli->lineIndex == 0)
    {
      full_help = TRUE;
    }

    /*
     * If completing argument and argument is NULL, then we don't
     * actually want to match anything.  This is primarily intended
     * for listing possible choices when none is given, where the
     * list may be descriptive, not intended for expansion.
     */
    if (*datap == EWS_CLI_CHAR_COMPLETE &&
        cli->argc > 1 &&
        cli->argv[cli->argc - 1][0] == '\0')
    {
      break;
    }

    mp = menu;
    tmp = cli->argv[cli->argc-1];
    /*
     * Tree CLI:
     * Traverse the tree as deep as there are arguments (and branches)
     */
    if (cli->argc > 1)
    {
      ambiguous = ewsCliParseTree(context, cli, menu, &marker, &tokens, dataTypeErrorMessage);

      if (*datap == EWS_CLI_CHAR_SPACE)
      {
        if (marker >=0 && strlen(dataTypeErrorMessage) > 0 && ambiguous == FALSE)
        {
          ewsCliPrintCaret(cli, marker, tokens, context, FALSE);
          ewsTelnetWrite(context, dataTypeErrorMessage);
          ewsTelnetWrite(context, cliCurrentPromptGet());
          ewsTelnetWrite(context,cli->line[cli->historyIndex]);

          return "";
        }
      }

      if (cli->parseCount >= cli->argc - 1)
      {
        mp = cli->parseList[cli->argc - 2];
      }
      else
      {
        mp = NULL;
      }
    }
    else
    {
      mp = menu;
      cli->parseCount = 0;
    }

    /*
     * For each item in menu
     */
    token = NULL;
    token_remaining = NULL;
    ambiguous_token = NULL;
    ambiguous_remaining = NULL;
    ambiguous_overlap = EWS_CLI_MAX_LINE;

    cp = (char *) ewsCliSearchPossible( context,
                                       datap,
                                       cli,
                                       mp,
                                       tmp,
                                       full_help,
                                       &token,
                                       &token_remaining,
                                       &ambiguous_token,
                                       &ambiguous_remaining,
                                       &ambiguous_overlap,
                                       &help_printed,
                                       mp->ignoreNodes);

    if(cp != NULL)
    {
      /*check for space or tab*/
      if(strlen(log_buf) > 0 && ((*datap == EWS_CLI_CHAR_SPACE)||(*datap == EWS_CLI_CHAR_COMPLETE)))
      {
        /*append the buffer to return val and increment the index*/
        strncat(cp, log_buf, strlen(log_buf));
        cli->lineIndex += strlen(log_buf);

        ewsTelnetWrite(context, cp);

        /*clear log_buf and add as many backspaces, this
           will take curser back to where it was*/
        bk_space_counter = strlen(log_buf);
        memset (log_buf, 0, EWS_CLI_MAX_LINE);
        for(; bk_space_counter > 0 ; bk_space_counter--)
        {
          strncat(log_buf, "\b", 1);
          cli->lineIndex--;
        }

        return log_buf;
      }

      return cp;
    }

    if( EWS_CLI_CHAR_POSSIBLE == *datap )
    {
      /* we allow 'no' commands without children to execute without
         <cr> indications built into the node, so show the <cr> option here
       */
      if(mp != NULL &&
         context->commType ==  CLI_NO_CMD &&
         help_printed == FALSE &&
         full_help &&
         cli->argc > 1
      )
      {
        ewsTelnetWrite(context,pStrInfo_common_CrLf);
        ewsCliPossible(context
                       ,TRUE
                       ,pStrInfo_common_Cr
                       ,pStrInfo_common_NewLine
                       ,&column
                       ,mp->cmdPad);
        help_printed = TRUE;
      }
      ewsTelnetWrite(context,pStrInfo_common_CrLf);
    }

    if (mp == NULL && help_printed == FALSE && *datap == EWS_CLI_CHAR_POSSIBLE)
    {
      ewsTelnetWrite(context,EWS_CLI_UNRECOGNIZED_CMD);
      ewsTelnetWrite(context,pStrInfo_common_CrLf);
    }

    /*
     * If no match was found, we can not complete.  If we were
     * listing possible matches, we're done.
     */
    if (token == NULL)
    {
      if (*datap == ' ')
      {
        goto just_a_space;
      }
      if (*datap == EWS_CLI_CHAR_POSSIBLE)
      {
        /* Check if the context is null
         * If not get it from context
         * else get it from currentPromptGet
         */
        if(context->telnet->prompt != NULL)
        {
          ewsTelnetWrite(context, context->telnet->prompt);
        }
        else
        {
          ewsTelnetWrite(context,cliCurrentPromptGet());
        }
        EMWEB_STRLEN(cli->lineIndex, cli->line[cli->historyIndex]);
        return cli->line[cli->historyIndex];
      }
      else
      {
        break;
      }
    }
    /*
     * If ambiguous match, update with common overlapping portion.
     */
    else if (ambiguous_token != NULL)
    {
      cp = &cli->line[cli->historyIndex][cli->lineIndex];
      EMWEB_MEMCPY(cp
                   ,token_remaining
                   ,ambiguous_overlap);
      cli->lineIndex += ambiguous_overlap;
      cli->line[cli->historyIndex][cli->lineIndex] = '\0';

      /*check for space or tab*/
      if(strlen(log_buf) > 0 && ((*datap == EWS_CLI_CHAR_SPACE)||(*datap == EWS_CLI_CHAR_COMPLETE)))
      {
        /*this code is to append buffer to the end of command line*/

        tmp=log_buf;

        EMWEB_STRLEN(i, cp);

        /*check for token already present at the beginning of buffer*/
        if(strstr(tmp, cp) == tmp)
        {
          tmp = tmp + i ;
        }
        else
        {
          /*add an extra space to the beginning for buffer*/
          memmove(tmp + 1, tmp, strlen(tmp));
          tmp[0] = ' ';
        }

        /*concatnate buffer content to return value*/
        strncat(cp, tmp, strlen(tmp));

        /*increment lineIndex accordingly*/
        cli->lineIndex += strlen(tmp);

        ewsTelnetWrite(context, cp);

        /*clear log_buf and add as many backspaces, this
           will take curser back to where it was*/
        bk_space_counter = strlen(tmp)-1;
        memset (log_buf, 0, EWS_CLI_MAX_LINE);
        for(; bk_space_counter > 0 ; bk_space_counter--)
        {
          strncat(log_buf, "\b", 1);
          cli->lineIndex--;
        }

        return log_buf;
      }

      return cp;
    }

    /*
     * Complete command
     */
    ret = &cli->line[cli->historyIndex][cli->lineIndex];
    EMWEB_STRLEN(i, token_remaining);
    EMWEB_STRCPY(ret,token_remaining);
    /* EMWEB_STRCAT(ret," ");     LVL7_P0006 */
    cli->lineIndex += i;         /* LVL7_P0006 */

    /*check for space or tab*/
    if(strlen(log_buf) > 0 && ((*datap == EWS_CLI_CHAR_SPACE)||(*datap == EWS_CLI_CHAR_COMPLETE)))
    {
      /*this code is to append buffer to the end of command line*/

      tmp = log_buf;

      /*check for token already present at the beginning of buffer*/
      if(strstr(log_buf, token_remaining) == log_buf)
      {
        tmp = tmp + i;
      }

      /*concatnate buffer content to command line*/
      strncat(cli->line[cli->historyIndex], tmp, strlen(tmp));

      /*increment lineIndex accordingly*/
      cli->lineIndex += strlen(tmp);

      ewsTelnetWrite(context, ret);

      /*clear log_buf and add as many backspaces, this
         will take curser back to where it was*/
      bk_space_counter = strlen(tmp);
      memset (log_buf, 0, EWS_CLI_MAX_LINE);
      for(; bk_space_counter > 0 ; bk_space_counter--)
      {
        strncat(log_buf, "\b", 1);
        cli->lineIndex--;
      }

      return log_buf;
    }

    return ret;

    /*********************************************************************
     * End of line
     */
  case EWS_CLI_CHAR_CR:
  case EWS_CLI_CHAR_ENTER:              /*lvl7_P0006*/
    /*
     * Ignore blank lines
     */

    /*check if the prompt has changed for stacking members (devShell prompt)*/
    if (cliIsCurrUnitMgmtUnit() != L7_TRUE &&
        usmDbUnitMgrNumberGet(&newUnit) == L7_SUCCESS)
    {
      if (newUnit != currUnit)
      {
        sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, cliSystemPromptGet(), pStrInfo_base_Unit, newUnit);
        cliInitConnection(cliCurrentHandleGet());
        ewsSetTelnetPrompt(context, cliCurrentPromptGet());
      }
    }

    if (cliCheckAltInput() == FALSE)    /* LVL7_P0006 */
    {                                   /* LVL7_P0006 */
      if (cli->line[cli->historyIndex][0] == '\0')
      {
        /* check if the message has already been displayed */
        if (cliTrapMsgStatusGet() == L7_TRUE)
        {
          ewsTelnetWrite(context, pStrInfo_common_CrLf);
          ewsCliTrapNotificationMsgGet(context);
          cliTrapMsgStatusSet(L7_FALSE);
        }

        return context->telnet->prompt;
      }
    }         /* LVL7_P0006 start */

    if (cli->line[cli->historyIndex][0] == ' ')   /* Checking the first character */
    {
      uintf i = 1;

      while (cli->line[cli->historyIndex][i] == ' ')
      {
        i++;    /* Ignoring all spaces */
      }

      if (cli->line[cli->historyIndex][i] == '\0')
      {
        return context->telnet->prompt;    /* Returning prompt if the command consists of only spaces */
      }
    }

    /*
     * Parse command line
     */
    ewsCliParseLine(context, cli);

    /*
     * If the first character in the argbuffer is a COMMENT_CHARECTER, then return without
     * parsing the tree
     */
    if (cli->arg_buffer[0] == EWS_CLI_COMMENT_CHAR)
    {
      /*
       * Do not include the comment line into history. Nullify the history string
       */
      *cli->line[cli->historyIndex] = '\0';
      cli->lineIndex = 0;
      ewsTelnetWrite(context, pStrInfo_common_CrLf);

      /* check if the message has already been displayed */
      if (cliTrapMsgStatusGet() == L7_TRUE)
      {
        ewsTelnetWrite(context, pStrInfo_common_CrLf);
        ewsCliTrapNotificationMsgGet(context);
        cliTrapMsgStatusSet(L7_FALSE);
      }

      return context->telnet->prompt;
    }
    /*
     * Find action
     */
    if( cli->argc == 0 && context->commType == CLI_NO_CMD )
    {
      ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, context, pStrErr_common_CfgTrapFlagsOspf);
      cliClearCharInput();
      cp = cliCurrentPromptGet();

    }
    else if (cli->argc == 0)
    {
      cp = cliCurrentPromptGet();
    }
    else
    {
      ambiguous = ewsCliParseTree(context, cli, menu, &marker, &tokens, dataTypeErrorMessage);

      if (marker >=0 && strlen(dataTypeErrorMessage) > 0 && ambiguous == FALSE)
      {
        ewsCliPrintCaret(cli, marker, tokens, context, FALSE);
        ewsTelnetWrite(context, dataTypeErrorMessage);
        data_error_printed = TRUE;
        /*  Don't return here.  Need to fall through so that the line is saved in the history buffer */
      }
      /* capture syntax string, if any */
      for (i = 0; i < cli->parseCount; i++)
      {
        if ( cli->parseList[i]->syntax_normal != NULL && context->commType == CLI_NORMAL_CMD)
        {
          syntaxMsg = cli->parseList[i]->syntax_normal;
        }
        if (cli->parseList[i]->syntax_no != NULL && context->commType == CLI_NO_CMD)
        {
          syntaxMsg = cli->parseList[i]->syntax_no;
        }
      }

      /* If normal command doesn't match, try to search for hidden
         command Tree */
	  osapiStrncpySafe(lastToken,cli->argv[cli->argc - 1],sizeof(lastToken));

      /* if lvl7clear is the last parameter, do not try to find parse error, but
         allow command to execute and enter command function with that one parameter */
      if ( ((ambiguous == FALSE && marker >=0) ||ambiguous == TRUE) &&
          (strcmp(lastToken, pStrInfo_common_Lvl7Clr) != 0) &&
          (strcmp(lastToken, pStrInfo_base_Lvl7Hidden) != 0) &&
          (strcmp(lastToken, pStrInfo_common_Lvl7TraceEnbl) != 0) &&
          (strcmp(lastToken, pStrInfo_common_Lvl7TraceDsbl) != 0)&&
          !((strncmp(cli->argv[0], pStrInfo_base_Cfgure_1, strlen(pStrInfo_base_Cfgure_1)) == 0) &&
            (menu == cliGetThisMode(L7_PRIVILEGE_USER_MODE, &cliModeRW)))&&
          (marker >=0))
      {
        hiddAmbiguous = ewsCliParseTree(context, cli, cliGetHiddenNode(), &hiddMarker, &hiddTokens, dataTypeErrorMessage);

        if(hiddAmbiguous == FALSE && hiddMarker <0)
        {
          commandHandle = TRUE;
          mp = menu;
          j = 0;
          for (i = 0; i < cli->parseCount; i++)
          {
            if (cli->parseList[i]->action != NULL )
            {
              j = i;
              mp = cli->parseList[i];
            }
          }

          /*Ensure that all arguments carry their full command/ node name*/
          cp =  cli->arg_buffer;

          if (context->commType == CLI_NO_CMD)
          {
            cp+=2;
          }

          for (m=0 ; m < cli->argc && m < cli->parseCount ; m++)
          {
            if ((cli->parseList[m]->command[0]) != '<'&& (cli->parseList[m]->command[0]) != '[')
            {
              prevArgsLen = 0;
              strlenArg = strlen(cli->argv[m]);
              strlenNodeName =strlen(cli->parseList[m]->command) - 1;
              prevArgsLen = cli->argv[m] - cli->argv[0];
              memmove(cp + strlenNodeName + prevArgsLen, cp + strlenArg + prevArgsLen, EWS_CLI_MAX_LINE - strlenNodeName - prevArgsLen);
              strncpy(cp + prevArgsLen, cli->parseList[m]->command, strlenNodeName);
              for(l = m + 1; l < cli->argc; l++)
              {
                cli->argv[l] = cli->argv[l] + strlenNodeName - strlenArg;
              }
            }
          }

          cliExamine(context, cli->argc,(const char * *) cli->argv, j);
          cp = (char *)mp->action( context
                                  ,cli->argc
                                  ,(const char * *) cli->argv
                                  ,j);

          if ( cp == NULL)
          {
            cp = cliCurrentPromptGet();
          }
          else if( strcmp(cp, " ") ==0)
          {
            cp = NULL;
          }
        }
      }
      if( commandHandle == FALSE)
      {
        if(ambiguous == FALSE )
        {
          /* if parsing mechanism found error at marker position and lvl7clear
             is not last parameter, allow command to execute and enter command
             function with that one parameter */
          if( marker >=0 &&
             (strcmp(lastToken, pStrInfo_common_Lvl7Clr) != 0) &&
             (strcmp(lastToken, pStrInfo_base_Lvl7Hidden) != 0) &&
             (strcmp(lastToken, pStrInfo_common_Lvl7TraceEnbl) != 0) &&
             (strcmp(lastToken, pStrInfo_common_Lvl7TraceDsbl) != 0))
          {
            L7_char8 * tmpcp = cli->argv[0];
            if(cli->argc == 1 && tokens == 0 &&
               ((tmpcp[0] == 'n' || tmpcp[0] == 'N') &&
                (tmpcp[1] == 'o' || tmpcp[1] == 'O')))
            {
              cliClearCharInput();
              ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, context, pStrErr_common_CfgTrapFlagsOspf);
              if (syntaxMsg != NULL)
              {
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, context, pStrInfo_common_EmptyString);
                ewsTelnetWrite(context, syntaxMsg);
              }
            }
            else
            {
              /* if there is a datatype error message, no need to print a message here */
              if (data_error_printed == FALSE)
              {
                ewsCliPrintCaret(cli, marker, tokens, context, TRUE);
                if (syntaxMsg != NULL)
                {
                  ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, context, pStrInfo_common_EmptyString);
                  ewsTelnetWrite(context, syntaxMsg);
                }
              }
            }
            cp = cliCurrentPromptGet();
          }
          else
          {
            int move_buf_len;

            mp = menu;
            j = 0;
            for (i = 0; i < cli->parseCount; i++)
            {
              if (cli->parseList[i]->action != NULL )
              {
                j = i;
                mp = cli->parseList[i];
              }
            }

            /*Ensure that all arguments carry their full command/ node name*/
            cp =  cli->arg_buffer;
            move_buf_len = EWS_CLI_MAX_LINE;

            if (context->commType == CLI_NO_CMD)
            {
              cp+=2;
              move_buf_len-=2;
            }

            for (m=0 ; m < cli->argc  && m < cli->parseCount ; m++)
            {
              if ((cli->parseList[m]->command[0]) != '<'&& (cli->parseList[m]->command[0]) != '[')
              {
                prevArgsLen = 0;
                strlenArg = strlen(cli->argv[m]);
                strlenNodeName =strlen(cli->parseList[m]->command) - 1;
                prevArgsLen = cli->argv[m] - cli->argv[0];
                memmove(cp + strlenNodeName + prevArgsLen, cp + strlenArg + prevArgsLen, move_buf_len - strlenNodeName - prevArgsLen);
                strncpy(cp + prevArgsLen, cli->parseList[m]->command, strlenNodeName);
                for(l = m + 1; l < cli->argc; l++)
                {
                  cli->argv[l] = cli->argv[l] + strlenNodeName - strlenArg;
                }
              }
            }

            if (cli->parseList[cli->parseCount-1] == L7_NULLPTR)
            {
              char buffer[200];
              ewsTelnetWrite(context, pStrInfo_common_CrLf);
              sprintf(buffer,"\r\n%s%s", "INVALID COMMAND:", cli->line[cli->historyIndex]);
              ewsTelnetWrite(context, buffer);
              ewsTelnetWrite(context, pStrInfo_common_CrLf);
              cp = cliCurrentPromptGet();
            }
            else
            {
  
              /* catch commands that have not completed all the arguments */
              ewsCliNodeHasCRChild(cli->parseList[cli->parseCount-1], context, &foundCr, &hasChild);
  
              if ((hasChild == FALSE) &&
                  (cli->parseList[cli->parseCount-1]->noFormStatus != L7_STATUS_NORMAL_ONLY) &&
                  (context->commType == CLI_NO_CMD))
              {
                /* allow "no" form commands to execute if they have no <cr> and no other children */
                foundCr = TRUE;
              }
  
              if ((foundCr == FALSE) &&
                  (strcmp(lastToken, pStrInfo_common_Lvl7Clr) != 0) &&
                  (strcmp(lastToken, pStrInfo_base_Lvl7Hidden) != 0) &&
                  (strcmp(lastToken, pStrInfo_common_Lvl7TraceEnbl) != 0) &&
                  (strcmp(lastToken, pStrInfo_common_Lvl7TraceDsbl) != 0)) 
              {
                cliClearCharInput();
                ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, context, pStrErr_common_CfgTrapFlagsOspf);
  
                if (syntaxMsg != NULL)
                {
                  ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, context, pStrInfo_common_EmptyString);
                  ewsTelnetWrite(context, syntaxMsg);
                }
                cp = cliCurrentPromptGet();
              }
              else
              {
                cliExamine(context, cli->argc, (const char * *) cli->argv, j);
                cp = (char *)mp->action( context
                                        ,cli->argc
                                        ,(const char * *) cli->argv
                                        ,j);
              }
  
              if ( cp == NULL)
              {
                cp = cliCurrentPromptGet();
              }
              else if( strcmp(cp, " ") ==0)
              {
                cp = NULL;
              }
            }
          }
        }
        else
        {
          char buffer[200];
          ewsTelnetWrite(context, pStrInfo_common_CrLf);
          sprintf(buffer,"\r\n%s%s", EWS_CLI_AMBIGUOUS_CMD, cli->line[cli->historyIndex]);
          ewsTelnetWrite(context, buffer);
          ewsTelnetWrite(context, pStrInfo_common_CrLf);
          cp = cliCurrentPromptGet();
        }
      }
    }

#ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    /*
     * If command handler suspends context, then put the NL
     * character back (i.e. increment *bytesp indicating to
     * TELNET that no NL character was consumed).  When the
     * context is resumed, TELNET will send NL to CLI, and
     * CLI will return to this state reinvoking the command
     * handler.
     */
    if (context->schedulingState == ewsContextSuspended)
    {
      *bytesp = *bytesp + 1;
      return cp;
    }
#endif          /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

    if (cliCheckAltInput() == FALSE)        /* LVL7_P0006 */
    {                                       /* LVL7_P0006 */

      /*
       * Update history
       */
      EMWEB_STRCMP(diff,cli->line[(cli->historyIndex+EWS_CLI_MAX_HISTORY-1) % EWS_CLI_MAX_HISTORY],cli->line[cli->historyIndex]);
      if(diff)
      {
        cli->historyIndex = (cli->historyIndex+1) % EWS_CLI_MAX_HISTORY;
        cli->historyBottom = cli->historyIndex;
        cli->historySearch = cli->historyBottom;
        if (cli->historyIndex == cli->historyTop)
        {
          cli->historyTop = (cli->historyTop+1) % EWS_CLI_MAX_HISTORY;
        }
      }
      else if(upArrowFlag == L7_TRUE)
      {
        upArrowFlag = L7_FALSE;
        cli->historySearch = (cli->historySearch + 1) % EWS_CLI_MAX_HISTORY;
      }

      if(cli->historySearch != cli->historyIndex)
      {
        cli->historySearch = (cli->historySearch + EWS_CLI_MAX_HISTORY - 1) % EWS_CLI_MAX_HISTORY;
      }

      cli->lineIndex = 0;
      *cli->line[cli->historyIndex] = '\0';

      /* Logging of the CLI commands in the parser */
      if (cli->historyIndex != 0)
      {
        entryIndex = ((cli->historyIndex) - 1);
      }
      else
      {
        entryIndex = (EWS_CLI_MAX_HISTORY - 1);
      }

      /* clear config is dealt with in the command so we are sure it gets logged ...
         skipped here so it does not get double counted */

      addCmdLoggerEntry = L7_TRUE;
      if ((strstr(cli->line[entryIndex], pStrErr_base_ClrCfgCmd) != 0) ||
          (strstr(cli->line[entryIndex], pStrInfo_base_Logout_1) != 0)  )
      {
        addCmdLoggerEntry = L7_FALSE;
      }

      if (addCmdLoggerEntry == L7_TRUE)
      {
        cliWebCmdLoggerEntryAdd(cli->line[entryIndex], FD_WEB_DEFAULT_MAX_CONNECTIONS+1);
      }

    }                                      /* LVL7_P0006 */

    return cp;

    /*********************************************************************
     * Handle other characters
     */
  default:

    if (cli->arrow_esc && (*datap == EWS_CLI_CHAR_LBRACKET))
    {
      cli->arrow_lb = TRUE;
      break;
    }
    /*
     * If non control character and room in buffer, store and echo.
     */
just_a_space:            /* LVL7_P0006 */
    if (*datap >= ' ' && cli->lineIndex < EWS_CLI_MAX_LINE - 2)
    {
      cli->arrow_esc = FALSE;

      cp = &cli->line[cli->historyIndex][cli->lineIndex];

      /*
       * If inserting character
       */
      cli->historySearch = cli->historyBottom;
      if (*cp != '\0')
      {
        /*
         * Move characters forward in line
         */
        for (i = EWS_CLI_MAX_LINE - 2; i != cli->lineIndex; i--)
        {
          cli->line[cli->historyIndex][i] =
            cli->line[cli->historyIndex][i - 1];
        }
        *cp = *datap;

        /*
         * Print line, then backup to cursor position
         */
        ewsTelnetWrite(context, cp);
        for (i = cli->lineIndex;
             cli->line[cli->historyIndex][i+1] != '\0';
             i++)
        {
          ewsTelnetWrite(context, "\b");
        }
        cli->lineIndex++;
        break;
      }

      /*
       * Else appending character, echo to client
       */
      else
      {
        ccc[0] = *cp = *datap;
        cp[1] = '\0';

        /*check for space or tab*/
        if(strlen(log_buf) > 0 && ((*datap == EWS_CLI_CHAR_SPACE)||(*datap == EWS_CLI_CHAR_COMPLETE)))
        {
          /*concatnate buffer content to command line*/
          strncat(cli->line[cli->historyIndex], log_buf, strlen(log_buf));

          /*add a space in begining*/
          memmove(log_buf + 1, log_buf, strlen(log_buf));
          log_buf[0] = ' ';

          /*increment lineIndex accordingly*/
          cli->lineIndex += strlen(log_buf);

          ewsTelnetWrite(context, log_buf);

          /*clear log_buf and add as many backspaces, this
             will take curser back to where it was*/
          bk_space_counter = strlen(log_buf)-1;
          memset (log_buf, 0, EWS_CLI_MAX_LINE);
          for(; bk_space_counter > 0 ; bk_space_counter--)
          {
            strncat(log_buf, "\b", 1);
            cli->lineIndex--;
          }

          return log_buf;
        }

        cli->lineIndex++;
        return ccc;
      }
    }
    break;
  }

  /*
   * Ignore characters by default
   */
  return NULL;
}

/*
 * ewsCliDepth
 * Set the command depth.  argc/argv specify a prefix command and arguments
 * to which the arguments specified in the command line are appended.  If
 * argc is zero, then no initial command and arguments are prepended to the
 * command line.
 */
void ewsCliDepth ( EwsContext context
                  ,uintf argc
                  ,const char * * argv
)
{
  uintf i;
  char * cp;
  const char * argvp;

  /*
   * Copy arguments into depth buffer for storage, and save argument
   * pointers in argv[] state.
   */
  cp = context->telnet->cli.depth_buffer;
  context->telnet->cli.depth = argc;
  for (i = 0; i < argc; i++)
  {
    argvp = argv[i];
    context->telnet->cli.argv[i] = cp;
    while (*argvp != '\0')
    {
      *cp++ = *argvp++;
    }
    *cp++ = '\0';
  }
}

/*
 * ewsCliSetSameAs
 * Sets the same as list of the node with some
 * other node if not already set.
 *
 * node  - pointer to existing node
 * sameAsNode - node that is to be set as same as
 *
 * Returns void.
 *
 * This function adds another node in same as list of a node.
 */

void ewsCliSetSameAs(EwsCliCommandP node, EwsCliCommandP sameAsNode)
{
  int i;

  if(sameAsNode == NULL || node == NULL)
  {
    return;
  }

  /*loop through to find and set sameAsNode*/
  for(i = 0; i < MAX_SAME_AS_PER_NODE; i++)
  {
    /*check if node is already present*/
    if(node->sameAsNodes[i] == sameAsNode)
    {
      return;
    }

    /*find the available index and store*/
    if (node->sameAsNodes[i] == NULL)
    {
      node->sameAsNodes[i] = sameAsNode;
      return;
    }
  }
}

/*
 * ewsCliAddSameAsCondtion
 * Sets the same as condition in all related nodes.
 *
 * node  - pointer to existing node
 * sameAsNode - node that is to be set as same as
 *
 * Returns void.
 *
 * This function adds another node in same as list of all related nodes.
 */
void ewsCliAddSameAsCondtion(EwsCliCommandP node, EwsCliCommandP sameAsNode)
{
  int i,j,k;

  if(sameAsNode == NULL || node == NULL)
  {
    return;
  }

  /*loop through to find and set sameAsNode*/
  for(i = 0; i < MAX_SAME_AS_PER_NODE; i++)
  {
    /*check if node is already present*/
    if(node->sameAsNodes[i] == sameAsNode)
    {
      return;
    }

    /*find the available index and store*/
    if (node->sameAsNodes[i] == NULL)
    {
      /*loop to set source node in all nodes in target's same as list */
      for(j = 0; j < MAX_SAME_AS_PER_NODE; j++)
      {
        /*check for end of list*/
        if (node->sameAsNodes[j] != NULL)
        {
          ewsCliSetSameAs(node->sameAsNodes[j], sameAsNode);
        }
        else
        {
          break;
        }
      }

      /*loop to find existing same as nodes of source*/
      for(k = 0; k < MAX_SAME_AS_PER_NODE; k++)
      {
        /*break if end of list reached*/
        if (sameAsNode->sameAsNodes[k] != NULL)
        {
          /*loop set node in all nodes in target's same as list */
          for(j = 0; j < MAX_SAME_AS_PER_NODE; j++)
          {
            /*check for end of list*/
            if (node->sameAsNodes[j] != NULL)
            {
              ewsCliSetSameAs(node->sameAsNodes[j], sameAsNode->sameAsNodes[k]);
            }
            else
            {
              break;
            }
          }
        }
        else
        {
          break;
        }
      }

      /*set same as node in target*/
      node->sameAsNodes[i]  = sameAsNode;

      /*loop to find existing same as nodes of source*/
      for(k = 0; k < MAX_SAME_AS_PER_NODE; k++)
      {
        /*set the nodes in target, break if end of list reached*/
        if (sameAsNode->sameAsNodes[k] != NULL)
        {
          node->sameAsNodes[i + k + 1]  = sameAsNode->sameAsNodes[k];
        }
        else
        {
          break;
        }
      }

      /*loop set target node in all nodes in source's same as list */
      for(j = 0; j < MAX_SAME_AS_PER_NODE; j++)
      {
        /*check for end of list*/
        if (sameAsNode->sameAsNodes[j] != NULL)
        {
          ewsCliSetSameAs(sameAsNode->sameAsNodes[j], node);
        }
        else
        {
          break;
        }
      }

      /*loop to find existing same as nodes of target*/
      for(k = 0; k < MAX_SAME_AS_PER_NODE && k < i; k++)
      {
        /*break if end of list reached*/
        if (node->sameAsNodes[k] != NULL)
        {
          /*loop set node in all nodes in target's same as list */
          for(j = 0; j < MAX_SAME_AS_PER_NODE; j++)
          {
            /*check for end of list*/
            if (sameAsNode->sameAsNodes[j] != NULL)
            {
              ewsCliSetSameAs(sameAsNode->sameAsNodes[j], node->sameAsNodes[k]);
            }
            else
            {
              break;
            }
          }
        }
        else
        {
          break;
        }
      }

      /*set target as same as node in source*/
      ewsCliSetSameAs(sameAsNode, node);

      /*loop to find existing same as nodes of source*/
      for(k = 0; k < MAX_SAME_AS_PER_NODE && k < i; k++)
      {
        /*set the nodes in target, break if end of list reached*/
        if (node->sameAsNodes[k] != NULL)
        {
          ewsCliSetSameAs(sameAsNode, node->sameAsNodes[k]);
        }
        else
        {
          break;
        }
      }

      break;
    }
  }
}
/*
 * ewsCliSetOptionalConditions
 * Sets the optional conditions for a node.
 *
 * node  - pointer to existing node
 * optCount - count of optional paramaters
 * marker - va_list containing optional parameters
 *
 * Returns void.
 *
 * This function sets the optional conditions for a node.
 */

void ewsCliSetOptionalConditions(EwsCliCommandP node, unsigned int optCount, va_list marker)
{
  EwsCliCommandOptions optValue = 0;
  int i = 0, optComponentId, optCountTemp = optCount;
  int maxDisplayCount;        /* LVL7_P51286 */
  EwsCliCommandP optNode;
  EwsCliNodeNoFormStatus noFormStatus;
  EwsCliNodeDataType nodeDataType;
  int minLength = 0;
  int maxLength = 0;
  int tempInt;

  /*check validity of node*/
  if (node == NULL)
  {
    return;
  }

  /*loop to process all optional arguments*/
  while( optCountTemp > 0 )
  {
    optValue = va_arg(marker, EwsCliCommandOptions);
    optCountTemp--;

    switch (optValue)
    {
      /*check condition for option node
         Use this flag to indicate that the node being added to the tree
         should use the children of another node as its own.  You can also
         use this flag to create a command with order independent parameters
         without spelling out all combinations of the parameters by pointing back
         to an ancestor node repeatedly.  In this case, each node is allowed to be used
         only once unless the L7_NODE_MAX_DISPLAY is used.
       */
    case L7_OPTIONS_NODE:
      optNode = va_arg(marker, EwsCliCommandP);
      optCountTemp--;

      for(i = 0; i < MAX_OPTIONS_PER_NODE && optNode != NULL; i++)
      {
        if (node->optionNodes[i] == NULL)
        {
          node->optionNodes[i]  = optNode;
          break;
        }
      }

      if (optNode != NULL)
      {
        node->cmdPad = optNode->cmdPad;
      }
  
      break;

      /*check condition for ignore node
         Used in conjunction with L7_OPTIONS_NODE to indicate not to show a particular
         node as a child.  If you use the L7_OPTIONS_NODE to allow order independent
         children, but one or more of these children requires special treatment such
         that it must be terminal (or a leaf), then add the unique node as an L7_IGNORE_NODE
         node on each of the siblings.
       */
    case L7_IGNORE_NODE:
      optNode = va_arg(marker, EwsCliCommandP);
      optCountTemp--;

      for(i = 0; i < MAX_IGNORES_PER_NODE && optNode != NULL; i++)
      {
        if (node->ignoreNodes[i] == NULL)
        {
          node->ignoreNodes[i]  = optNode;
          break;
        }
      }
      break;

      /*check condition for same as node
         Used in conjunction with L7_OPTIONS_NODE so as to create an
         exclusive OR between 2 nodes at the same level which are both
         pointing to the same L7_OPTION_NODE ancestor.  The effect is
         that you retain the recursive properties between the node and
         its siblings, but the node and the node pointed to by the flag,
         L7_SAME_AS_NODE, become an atomic unit.  This way only one of
         the 'same as' linked nodes can be used in the command, not both,
         and any other siblings can be used once each in any order.
       */
    case L7_SAME_AS_NODE:
      optNode = va_arg(marker, EwsCliCommandP);
      optCountTemp--;
      ewsCliAddSameAsCondtion(node, optNode);
      break;

      /* LVL7_P51286 start */
      /*check for max display option and get count value
         Use this flag in conjunction with L7_OPTIONS_NODE flag.
         By default, each node is allowed to be entered only once in the command,
         but this flag can alter the default behavior.
         "match external 1 external 2"
         In the above example,
         the node "external" can be entered twice using this flag so that the two
         combinations "external 1" and "external 2" can be entered in a single command
         while "external" is a single node.
       */
    case L7_NODE_MAX_DISPLAY:
      maxDisplayCount = va_arg(marker, int);
      optCountTemp--;
      if (maxDisplayCount > 0)
      {
        node->maxDisplayCount = maxDisplayCount;
      }
      break;
      /* LVL7_P51286 end */

      /*check condition for no form
         Node is valid for 'no' or both 'no' and 'normal' forms
       */
    case L7_NO_COMMAND_SUPPORTED:
      noFormStatus = va_arg(marker, EwsCliNodeNoFormStatus);
      optCountTemp--;
      node->noFormStatus = noFormStatus;
      break;

      /*check optValue for advance keying node
         Shows the node only if a component ID is present.
       */
    case L7_KEY_RESTRICTED_NODE:
      optComponentId = va_arg(marker, int);
      optCountTemp--;
      node->componentId = optComponentId;
      break;

      /*check optValue for feature dependent node (only valid when L7_KEY_RESTRICTED_NODE is also specified)
         Shows the node only if a feature ID is present.
       */
    case L7_FEATURE_DEP_NODE:
      node->featureId = va_arg(marker, int);
      optCountTemp--;
      break;

      /* data type node
         Specify a known data type, i.e., MAC address, IP address, USP, etc.
       */
    case L7_NODE_DATA_TYPE:
      nodeDataType = va_arg(marker, EwsCliNodeDataType);
      optCountTemp--;
      node->dataType = nodeDataType;
      break;

      /* length of node
         Enforce max/min restrictions on length of node string
       */
    case L7_NODE_STRING_RANGE:
      minLength = va_arg(marker, int);
      optCountTemp--;
      node->minLength.minu32 = minLength;
      if (optCountTemp > 0)
      {
        maxLength = va_arg(marker, int);
        optCountTemp--;
        node->maxLength.maxu32 = maxLength;
      }
      node->dataType = L7_STRING_DATA_TYPE;
      break;

      /* unsigned integer range
         Node is an unsigned integer value within a min/max range
       */
    case L7_NODE_UINT_RANGE:
      optCountTemp--;
      node->minLength.minu32 = va_arg(marker, L7_uint32);
      if (optCountTemp > 0)
      {
        optCountTemp--;
        node->maxLength.maxu32 = va_arg(marker, L7_uint32);
      }
      node->dataType = L7_UNSIGNED_INTEGER;
      break;

      /* signed integer range
         Node is a signed integer value within a min/max range
       */
    case L7_NODE_INT_RANGE:
      optCountTemp--;
      node->minLength.min32 = va_arg(marker, L7_int32);
      if (optCountTemp > 0)
      {
        optCountTemp--;
        node->maxLength.max32 = va_arg(marker, L7_int32);
      }
      node->dataType = L7_SIGNED_INTEGER;
      break;

      /* command syntax message*/
    case L7_SYNTAX_NORMAL:
      node->syntax_normal = va_arg(marker, char *);
      optCountTemp--;
      break;

      /* command syntax message*/
    case L7_SYNTAX_NO:
      node->syntax_no = va_arg(marker, char *);
      optCountTemp--;
      break;

    case L7_NODE_HIDDEN_SUPPORT_NODE:
      L7_NODE_FLAGS_SET_SUPPORT(node);
      break;

    case L7_NODE_IF_TYPES:

      tempInt = va_arg(marker, L7_int32);
      optCountTemp--;
      if (tempInt & USM_PHYSICAL_INTF)
      {
        L7_NODE_FLAGS_SET_PHYS_IF_VALID(node);
      }
      if (tempInt & USM_LAG_INTF)
      {
        L7_NODE_FLAGS_SET_LAG_IF_VALID(node);
      }
      if (tempInt & USM_ROUTER_INTF)
      {
        L7_NODE_FLAGS_SET_RTR_IF_VALID(node);
      }
      if (tempInt & USM_LOGICAL_VLAN_INTF)
      {
        L7_NODE_FLAGS_SET_LOG_VLAN_IF_VALID(node);
      }
      break;

    case L7_NODE_TYPE_MODE:
      node->mode = va_arg(marker, L7_int32);
      optCountTemp--;
      break;

    default:
      EMWEB_WARN(("ewsCliSetOptionalConditions: unrecognized option %d for node %s\n", optValue, node->command));
      break;
    }
  } /*end of loop for optional parameters*/

  /* check for parameter errors */
  if (node->componentId == 0 && node->featureId > 0)
  {
    EMWEB_WARN(("ewsCliSetOptionalConditions: featureId specified (L7_FEATURE_DEP_NODE) without componentId (L7_KEY_RESTRICTED_NODE) for %s\n", node->command));
  }
  if ((node->dataType == L7_SIGNED_INTEGER) &&
      (node->minLength.min32 > node->maxLength.max32))
  {
    EMWEB_WARN(("ewsCliSetOptionalConditions: invalid integer range for %s\n", node->command));
  }
  else if ((node->dataType == L7_UNSIGNED_INTEGER) &&
           (node->minLength.minu32 > node->maxLength.maxu32))
  {
    EMWEB_WARN(("ewsCliSetOptionalConditions: invalid integer range for %s\n", node->command));
  }
  if (optCountTemp != 0)
  {
    EMWEB_WARN(("ewsCliSetOptionalConditions: invalid option count specified for node %s\n", node->command));
  }
}

/*
 * ewsCliAddConditionToNode
 * Adds a new special condition to the existing node.
 *
 * node  - pointer to existing node
 * optCount - count of number of variable paramertrs
 *
 * Returns void.
 *
 * This function adds new special condition to the existing node.
 */
void
ewsCliAddConditionToNode( EwsCliCommandP node
                         ,unsigned int optCount
                         ,...
)
{
  va_list marker;

  if ( NULL == node )
  {
    EMWEB_WARN(("ewsCliAddConditionToNode: node is NULL\n"));
    return;
  }

  /*get count of optional parameter set*/

  va_start( marker, optCount);     /* Initialize variable arguments. */

  ewsCliSetOptionalConditions(node, optCount, marker); /*set conditions*/

  va_end( marker );              /* Reset variable arguments.      */
}

/*
 * ewsCliAddChildSorted
 *
 * Add a new child node to a parent, but in such a way as to make the list of
 * children alphabetically sorted.
 *
 * parent - pointer to parent node
 * child  - pointer to child node
 *
 * No returned value.
 *
 * This function is to enhance ewsCliAddNode so that the children are
 * maintained in a sorted order.  Previously, it was necessary to add nodes in
 * the order in which they should be displayed.
 */
static void ewsCliAddChildSorted( EwsCliCommandP parent,
                                  EwsCliCommandP child )
{
  EwsCliCommandP curr_child;
  EwsCliCommandP next_child;
  int rc;

  if (parent->first_child == NULL)
  {
    parent->first_child = child;
  }
  /*
   * Otherwise, link to siblings
   */
  else
  {
    EWA_TASK_LOCK();

    if((child->command != NULL) &&(parent->first_child->command != NULL))
    {
      EMWEB_STRNCMP( rc, child->command, parent->first_child->command, EWS_CLI_MAX_LINE );

      if ( rc > 0 )               /* New child comes after first_child. */
      {
        curr_child = parent->first_child;
        next_child = (EwsCliCommandP)parent->first_child->siblings.next;

        while ( next_child != parent->first_child )
        {
          EMWEB_STRNCMP( rc, child->command, next_child->command, EWS_CLI_MAX_LINE );

          if ( rc <= 0 )          /* if child comes before next_child */
          {
            break;
          }

          curr_child = next_child;
          next_child = (EwsCliCommandP)curr_child->siblings.next;
        }

        EWS_LINK_PUSH( &curr_child->siblings, &child->siblings );
      }
      else
      {
        parent->first_child->siblings.prev->next = &child->siblings;
        child->siblings.prev                     = parent->first_child->siblings.prev;

        parent->first_child->siblings.prev = &child->siblings;
        child->siblings.next               = &parent->first_child->siblings;

        parent->first_child = child;
      }
    }
    EWA_TASK_UNLOCK();
  }
}

static int ewsCliNumTokens = 0;
static int ewsCliNumCmds = 0;

void ewsCliCmdCountDump(void)
{
  printf("Number of tokens = %d\n", ewsCliNumTokens);
  printf("Number of commands = %d\n", ewsCliNumCmds);
}

/*
 * ewsCliAddNode
 * Create a new parse tree, or add a node to an existing parse tree
 *
 * parent  - pointer to parent node, or NULL to create a new root node
 * command - pointer to string containing command in parse node
 * description - pointer to string containing more description, or NULL
 * action  - application function to call, or NULL.  The action of the root
 *           node will be called if no other action is found for a command
 *           line (i.e. if the command line is not parsable according to the
 *           rules).
 *
 * Returns new node.
 *
 * This function adds a new command to the parse tree.  The EmWeb/CLI will
 * traverse the tree according to user input on the command line, handling
 * command-line completion and context-sensitive help accordingly.  When a
 * command is executed, the deepest action will be invoked with argv[0] and argc
 * adjusted to pass the command and any additional arguments to the
 * application.
 *
 * The root node for a given tree is passed to ewsCliData for processing.
 *
 * This function allows variable number of arguments to be supplied. The
 * number of variable argument is passed in the function through 'optCount'.
 * The optional parameters that may be passed in are defined in
 * enum 'EwsCliCommandOptions_e'. These are special parameters to be set for
 * node so that parser can treat them differently based on runtime conditions.
 */
EwsCliCommandP
ewsCliAddNode( EwsCliCommandP parent
              ,const char * command
              ,const char * description
              ,EwaCli_f * action
              ,unsigned int optCount
              ,...
)
{
  EwsCliCommandP node;
  EwsCliCommandAllocator * allocator = &defaultNodellocator;
  EwsCliCommandP tempNode;
  L7_char8 commandBuf[MAX_COMMAND_LENGTH];
  const char * tempCommandP;
  const char * tempDescrP;
  uintf cmdLen;
  uintf tmpPad;

  int i = 0;
  va_list marker;

#ifdef EMWEB_SANITY
  if ( !allocator )
  {
    EMWEB_WARN(("No allocator provided"));
    return 0;
  }
  if ( !allocator->allocate )
  {
    EMWEB_WARN(("No allocation routine provided"));
    return 0;
  }
#endif

  node = (*allocator->allocate)(command,description);
  if ( NULL == node )
  {
    EMWEB_WARN(("ewsCliAddNodeWithAlloc: allocator failed\n"));
    return 0;
  }

  /*
   * Initialize node
   */
  node->allocator = allocator;
  node->parent = parent;
  node->action = action;
  node->first_child = NULL;
  node->refcount = 0;
  node->noFormStatus = L7_STATUS_NORMAL_ONLY;
  node->componentId = 0;
  node->featureId = 0;
  node->dataType = L7_NO_DATA_TYPE;
  node->minLength.minu32 = 0;
  node->maxLength.maxu32 = 0;
  node->maxDisplayCount = NODE_MAX_DISPLAY_COUNT_DEFAULT;
  L7_NODE_FLAGS_CLR_EXEC_NORMAL(node);
  L7_NODE_FLAGS_CLR_EXEC_NO(node);
  L7_NODE_FLAGS_CLR_SUPPORT(node);
  node->mode = 0;
  node->syntax_normal = NULL;
  node->syntax_no     = NULL;

  if (parent != NULL)
  {
    if (command != NULL)
    {
      EMWEB_STRLEN(cmdLen, command);

      if (cmdLen >= EWS_CLI_POSSIBLE_WIDTH)
      {
        tmpPad = cmdLen + 2;
      }
      else
      {
        tmpPad = EWS_CLI_POSSIBLE_WIDTH;
      }

      if (tmpPad > parent->cmdPad)
      {
        parent->cmdPad = tmpPad;
      }
    } 
    else {
      parent->cmdPad = EWS_CLI_POSSIBLE_WIDTH;
    }
  }  

  /*initialise array*/
  for(i = 0; i < MAX_OPTIONS_PER_NODE; i++)
  {
    node->optionNodes[i] = NULL;
  }

  /*initialise array*/
  for(i = 0; i < MAX_IGNORES_PER_NODE; i++)
  {
    node->ignoreNodes[i] = NULL;
  }

  /*initialise array*/
  for(i = 0; i < MAX_SAME_AS_PER_NODE; i++)
  {
    node->sameAsNodes[i] = NULL;
  }

  /* process optional properties of node*/

  va_start( marker, optCount);     /* Initialize variable arguments. */

  ewsCliSetOptionalConditions(node, optCount, marker); /*set conditions*/

  va_end( marker );              /* Reset variable arguments.      */

  /* create the command string from the provided integer range */
  if (command == NULL && (node->dataType == L7_UNSIGNED_INTEGER || node->dataType == L7_SIGNED_INTEGER))
  {
    if (node->dataType == L7_UNSIGNED_INTEGER)
    {
      osapiSnprintf(commandBuf, sizeof(commandBuf), "<%lu-%lu> ", node->minLength.minu32, node->maxLength.maxu32);
    }
    else
    {
      osapiSnprintf(commandBuf, sizeof(commandBuf), "<%ld-%ld> ", node->minLength.min32, node->maxLength.max32);
    }

    tempNode = (*allocator->allocate)(commandBuf, node->description);
    tempDescrP = tempNode->description;
    tempCommandP = tempNode->command;

    memcpy(tempNode, node, sizeof(EwsCliCommand));
    tempNode->description = tempDescrP;
    tempNode->command = tempCommandP;

    (*allocator->deallocate)(node);
    node = tempNode;
  }

  ewsCliNumTokens++;
  /* prevent <cr> nodes from using memory */
  if (command != NULL && strncmp(command, pStrInfo_common_Cr, 4) == 0)
  {
    ewsCliNumCmds++;
    if (parent == NULL)
    {
      EMWEB_WARN(("\newsCliAddNode: CR node added to NULL parent\n"));
    }
    else
    {
      parent->mode = node->mode;
      switch (node->noFormStatus)
      {
      case L7_STATUS_BOTH:
        L7_NODE_FLAGS_SET_EXEC_NO(parent);
        L7_NODE_FLAGS_SET_EXEC_NORMAL(parent);
        break;
      case L7_STATUS_NO_ONLY:
        L7_NODE_FLAGS_SET_EXEC_NO(parent);
        break;
      case L7_STATUS_NORMAL_ONLY:
        L7_NODE_FLAGS_SET_EXEC_NORMAL(parent);
        break;
      default:
        L7_NODE_FLAGS_SET_EXEC_NORMAL(parent);
        break;
      }
    }
    (*allocator->deallocate)(node);
    return 0;
  }
  EWS_LINK_INIT(&node->siblings);

  /*
   * If not root of tree
   */
  if (parent != NULL)
  {
    ewsCliAddChildSorted( parent, node );
  }
  else
  {
    ewsCliAddChildSorted( ews_state->super_root, node );
  }
#if 0
  if (parent != NULL)
  {
    /*
     * If first child, add it
     */
    if (parent->first_child == NULL)
    {
      parent->first_child = node;
    }
    /*
     * Otherwise, link to siblings
     */
    else
    {
      EWS_LINK_INSERT(&parent->first_child->siblings, &node->siblings);
    }
  }
  else
  {
    EWA_TASK_LOCK();
    if (ews_state->super_root->first_child == NULL)
    {
      ews_state->super_root->first_child = node;
    }
    else
    {
      EWS_LINK_INSERT(&ews_state->super_root->first_child->siblings,&node->siblings);
    }
    node->parent = ews_state->super_root;
    EWA_TASK_UNLOCK();
  }
#endif
  return node;
}

/*
 * ewsCliAddTree
 * This function links a tree to the bottom of a node by reference.  This is
 * helpful if a subtree can describe a common construct used by many commands
 * without replicating parts of the parse tree.  Note that this can only be
 * done to a node that currently has no children.
 */
void ewsCliAddTree( EwsCliCommandP parent,EwsCliCommandP tree)
{
#ifdef EMWEB_SANITY
  if (parent->first_child != NULL)
  {
    EMWEB_WARN(("ewsCliAddTree: can't add tree to non-leaf node\n"));
    return;
  }
#endif  /* EMWEB_SANITY */

  L7_NODE_FLAGS_SET_TREE(parent);

  parent->first_child = tree->first_child;
  tree->refcount++;
  parent->cmdPad = tree->cmdPad;


  /* new <cr> handling requires copying the executable flags
   * since the <cr>s no longer exist as actual nodes */

  if (L7_NODE_FLAGS_IS_EXEC_NO(tree))
  {
    L7_NODE_FLAGS_SET_EXEC_NO(parent);
  }
  if (L7_NODE_FLAGS_IS_EXEC_NORMAL(tree))
  {
    L7_NODE_FLAGS_SET_EXEC_NORMAL(parent);
  }
}

/*
 * ewsCliShutdown
 * This function frees allocated memory upon CLI shutdown
 */
void ewsCliShutdown (void)
{
  EwsCliCommandP f_child;

  f_child = ews_state->super_root->first_child;

  if (f_child != NULL)
  {
    while (TRUE)
    {
      f_child = (EwsCliCommandP) EWS_LINK_NEXT (f_child->siblings);
      if (f_child->refcount == 0)
      {
        ews_state->super_root->first_child = f_child;
        break;
      }
    }
    while (ews_state->super_root->first_child != NULL)
    {
      ewsCliDeleteTree (ews_state->super_root->first_child);
    }
  }
  ewaFree (ews_state->super_root);
}

/*
 * ewsCliDeleteTree
 * This function deletes a tree.
 *
 * tree - root of a tree to be deleted.
 */
void ewsCliDeleteTree (EwsCliCommandP tree)
{
  if (tree->parent == ews_state->super_root)
  {
    if (tree->refcount <= 0)
    {
      EWA_TASK_LOCK();
      ewsCliDeleteNode (tree);
      EWA_TASK_UNLOCK();
    }
    else
    {
      EMWEB_WARN(("ewsCliDeleteTree: can't delete tree which is in use\n"));
    }
  }
  else
  {
    EMWEB_WARN(("ewsCliDeleteTree: can't delete non-tree node\n"));
  }
}

/*
 * ewsCliDeleteNode
 * Remove and delete a node (and its offspring, if any).
 *
 * node  - pointer to a node to be removed and deleted
 */
void ewsCliDeleteNode (EwsCliCommandP node)
{
  EwsCliCommandP cur_np, next_np;
  boolean node_delete = FALSE;
  cur_np = node;

  while (TRUE)
  {
    next_np = ewsCliCheckOffspring (cur_np);
    if (next_np == node)
    {
      if (node->parent->first_child == node)
      {
        while (TRUE)
        {
          next_np = (EwsCliCommandP) EWS_LINK_NEXT (next_np->siblings);
          if (next_np->refcount == 0)
          {
            break;
          }
        }
        if (next_np == node)
        {
          node->parent->first_child = NULL;
          node_delete = TRUE;
        }
        else
        {
          next_np->parent->first_child = next_np;
          EWS_LINK_DELETE (&node->siblings);
          node_delete = TRUE;
        }
      }
      else
      {
        EWS_LINK_DELETE (&node->siblings);
        node_delete = TRUE;
      }
    }
    else if (next_np == cur_np)
    {
      next_np = ewsCliCheckSibling (cur_np);
    }
    cur_np = next_np;
    if (node_delete)
    {
      EwsCliCommandAllocatorP allocator = node->allocator;
      /* if no allocator provided, then just silently ignore;
       * maybe the application has some scheme of its own.
       */
      if (allocator)
      {
        void (*dealloc)(EwsCliCommand *p) = allocator->deallocate;
        if ( dealloc != NULL )
        {
          (*dealloc)(node);
        }
      }
      break;
    }
  }
}

/*
 * ewsCliCheckOffspring
 * Check out offspring nodes until the childless one is found
 *
 * node - pointer to the node to be checked
 * Return pointer to first_child node, or itself (if there is no children)
 * If first child belongs to tree - unlink it.
 */

EwsCliCommandP ewsCliCheckOffspring (EwsCliCommandP node)
{
  EwsCliCommandP np, real_parent;

  if (node->first_child == NULL)
  {
    np = node;
  }
  else
  {
    real_parent = node->first_child->parent;
    if (node != real_parent)
    {
      real_parent->refcount--;
      node->first_child = NULL;
      np = node;
    }
    else
    {
      np = node->first_child;
    }
  }
  return np;
}

/*
 * ewsCliCheckSibling
 * Check out siblings and delete childless ones
 *
 * node - pointer to the childless node
 * Return pointer to sibling node with child, if one found, or
 *        pointer to parent node, if all siblings are deleted.
 */

EwsCliCommandP ewsCliCheckSibling (EwsCliCommandP node)
{
  EwsCliCommandP cur_np, next_np;

  cur_np = node;

  while (TRUE)
  {
    next_np = (EwsCliCommandP) EWS_LINK_NEXT (cur_np->siblings);
    if (next_np != cur_np)
    {
      EWS_LINK_DELETE (&cur_np->siblings);
      ewaFree (cur_np);
      cur_np = next_np;
      if (cur_np->first_child != NULL)
      {
        break;
      }
    }
    else
    {
      cur_np = cur_np->parent;
      cur_np->first_child = NULL;
      ewaFree (next_np);
      break;
    }
  }
  return cur_np;
}

/*
 * ewsCliAddChain
 * Add a node or a chain of nodes (as specified in command string) to an
 * existing parse tree.
 *
 * parent  - pointer to parent node
 * command - pointer to string containing commands in parse chain
 * description - pointer to string containing a description for the last
 *               command, or NULL
 * action - application function for the last command in chain to call,
 *          or NULL.  The action of the root node will be called if no
 *          other action is found for a command line (i.e. if the command
 *          line is not parsable according to the rules).
 *
 * Returns last node in command chain.
 *
 * This function checks existance of each node representing a command
 * listed in the command string. If the command is not found it will be added
 * to the parse tree.
 */
EwsCliCommandP
ewsCliAddChain ( EwsCliCommandP parent
                ,const char * command
                ,const char * description
                ,EwaCli_f * action
)
{
  EwsCliCommandP np;
  EwsCliCommandLineP command_p = NULL;
  const char * desc = NULL;
  EwaCli_f * act = NULL;

  boolean differ = TRUE;
  boolean node_found = FALSE;
  uintf i;
  EwsCliCommandLine dummy;

  if (parent == NULL)
  {
    EMWEB_WARN(("ewsCliAddChain: can't be used to create a tree\n"));
    return NULL;
  }
  command_p = &dummy;
  ewsCliParseCommandLine (command, command_p);

  np = parent;

  for (i = 0; i < command_p->args; i++)
  {
    if (np->first_child == NULL)
    {
      node_found = FALSE;
    }
    else
    {
      np = np->first_child;
      do
      {
        EMWEB_STRCMP (differ, np->command, command_p->command_list[i]);
        if (!differ)
        {
          node_found = TRUE;
          break;
        }
        else
        {
          node_found = FALSE;
        }
        np = (EwsCliCommandP) EWS_LINK_NEXT(np->siblings);
      } while (np->parent->first_child != np);
    }
    if (!node_found)
    {
      if (i < command_p->args - 1)
      {
        desc = NULL;
        act = NULL;
      }
      else if (i == command_p->args - 1)
      {
        desc = description;
        act = action;
      }
      np = ewsCliAddNode(np, command_p->command_list[i], desc, act, 0);
    }
    else if (node_found && i == command_p->args - 1)
    {
      np->description = description;
      np->action = action;
    }
  }
  return np;
}

/*
 * ewsCliParseCommandLine
 * Parse command string into array of commands
 *
 * command - pointer to string containing commands in parse chain
 * command-p - pointer to command line structure
 */
void ewsCliParseCommandLine (const char * real_command, EwsCliCommandLineP command_p)
{
  char * cp;
  static char command[EWS_CLI_MAX_LINE]; /* dummy buffer to hold the command */

  EMWEB_STRCPY (command, real_command);
  EMWEB_MEMSET (command_p, 0, sizeof(*command_p));

  cp = (char *)command;
  command_p->command_list[0] = cp;

  while (*cp != '\0' && command_p->args < EWS_CLI_MAX_ARGS - 1)
  {
    if (*cp == ' ')
    {
      for ( ; *cp == ' '; cp++)
      {
        *cp = '\0';
      }

      command_p->args++;
      command_p->command_list[command_p->args] = cp;
      continue;
    }
    cp++;
  }
  if (command_p->command_list[command_p->args][0] != '\0')
  {
    command_p->args++;
  }
}

/*
 * ewsCliFindNode
 * Locate a node in the tree
 *
 * parent  - pointer to node to start search from
 * command - pointer to string containing commands in parse chain
 *
 * Returns last node in command chain, or NULL if not in the tree.
 *
 */
EwsCliCommandP ewsCliFindNode ( EwsCliCommandP parent
                               ,const char * command
)
{
  EwsCliCommandP np, ret_np = NULL;
  EwsCliCommandLineP command_p = NULL;
  boolean node_found = FALSE;
  boolean differ = TRUE;
  uintf i, len;

  EwsCliCommandLine dummy;
  command_p = &dummy;

  ewsCliParseCommandLine (command, command_p);

  np = parent->first_child;

  for (i = 0; i < command_p->args; i++)
  {
    if (np == NULL)
    {
      node_found = FALSE;
    }
    else
    {
      do
      {
        EMWEB_STRLEN (len, command_p->command_list[i]);
        EMWEB_MEMCMP (differ, np->command, command_p->command_list[i], len);
        if (!differ) /* the polarity of this changed with UPnP merge */
        {
          node_found = TRUE;
          if (i != command_p->args - 1)
          {
            np = np->first_child;
          }
          break;
        }
        else
        {
          node_found = FALSE;
        }
        np = (EwsCliCommandP) EWS_LINK_NEXT(np->siblings);
      } while (np->parent->first_child != np);
    }
    if (!node_found)
    {
      ret_np = NULL;
      break;
    }
    else if (node_found && i == command_p->args - 1)
    {
      ret_np = np;
    }
  }
  return ret_np;
}

/*
 * ewsCliInit
 * Create and initialize a super root
 */
void ewsCliInit ()
{
  /*
   * Create a super root
   */
  ews_state->super_root = (EwsCliCommandP) ewaAlloc(sizeof(EwsCliCommand));
  if (ews_state->super_root == NULL)
  {
    EMWEB_WARN(("ewsCliInit: ewaAlloc failed\n"));
    return;
  }

  /*
   * Initialize super root
   */
  ews_state->super_root->command = NULL;
  ews_state->super_root->description = NULL;
  ews_state->super_root->parent = NULL;
  ews_state->super_root->action = NULL;
  ews_state->super_root->first_child = NULL;
  EWS_LINK_INIT(&ews_state->super_root->siblings);
}

/*
 * ewsCliGetCommand
 * Get a command from a history buffer.
 *
 *   context  - context of request
 *   indx     - index to pass in
 *
 * Return text associated with passed in index (example: index = 1 returns
 * previous command) from history buffer, or empty string if no command
 * exists, or NULL if index is out of bounds.
 */
char * ewsCliGetCommand ( EwsContext context, uintf indx  )
{
  EwsCliState cli = &context->telnet->cli;
  char * cp;
  sintf idx;

  if (indx  <= (EWS_CLI_MAX_HISTORY - 1))
  {
    idx = (cli->historyIndex) - indx ;
    if (idx < 0)
    {
      idx += EWS_CLI_MAX_HISTORY;
    }
    cp = cli->line[idx];
    if (*cp == '\0')
    {
      cp = (char *)"";
    }
  }
  else
  {
    cp = NULL;
  }
  return cp;
}

/*
 * ewsCliNumCommand
 * Return number of commands in history buffer.
 *
 *   context  - context of request
 *
 */
uintf ewsCliNumCommand ( EwsContext context )
{
  EwsCliState cli = &context->telnet->cli;
  uintf i, n = 0;

  for (i = 0; i < EWS_CLI_MAX_HISTORY; i++)
  {
    if ( (cli->line[i][0]) != '\0' )
    {
      n++;
    }
  }
  return n;
}

/*
 * ewsCliPrintCaret
 * Print a caret sign
 *
 *   cli     -  cli structre.
 *   marker  -  Error position marker
 *   tokens  -  Number of successfully parsed tokens.
 *   context  - context of request
 *
 * Prints the invalid input detected caret symbol
 */
void ewsCliPrintCaret( EwsCliState cli, uintf marker, sintf tokens, EwsContext context, boolean printMsg )
{
  char command[EWS_CLI_MAX_LINE];
  uintf offset = cli->depth;
  uintf caretPos =0;
  uintf i =0;
  char * cp;
  char * cp1 = NULL;
  uintf length = strlen(cliCurrentPromptGet());

  EMWEB_STRCPY(command, cli->line[cli->historyIndex]);
  cp = command;
  if( tokens>0)
  {
    for ( i =offset; i<=tokens; i++)
    {
      if( i == offset)
      {
        cp1 = strstr(cp, cli->argv[i]);
      }
      else
      {
        cp1 = strstr(cp + strlen(cli->argv[i-1]), cli->argv[i]);
      }

      if( cp1 != NULL)
      {
        caretPos += cp1 - cp ;
        cp = cp1;
      }
    }
  }
  else
  {
    cp1 = strstr(cp, cli->argv[0]);
    if( cp1 != NULL)
    {
      caretPos += cp1 - cp ;
    }
  }
  ewsTelnetWrite(context, pStrInfo_common_CrLf);
  if( caretPos+marker >0)
  {
    for ( i =0; i<caretPos+marker  ; i++)
    {
      ewsTelnetWrite(context, " ");
    }
  }

  if(length > 2)
  {
    for(i=0; i<length -2; i++)
    {
      ewsTelnetWrite(context, " ");
    }
  }
  ewsTelnetWrite(context, "^\r\n");
  if (printMsg == TRUE)
  {
    ewsTelnetWrite(context,EWS_CLI_INVALID_INPUT_DETECTED);
    ewsTelnetWrite(context, pStrInfo_common_CrLf);
  }

  cliClearCharInput();
}

/*
 * ewsCliPrintHelp
 *
 *   context -  pointer to cli context
 *   str     -  Help String
 *   startPos -  current cursor position
 *   cmdPad      - (input) padding required for command column 
 * Prints the help string
 */

void ewsCliPrintHelp( EwsContext context, const char * str, int startPos, unsigned char cmdPad)
{
  int i,j,k;
  char tmpStr[2];
  int k1;
  int found;
  int done = 0;
  char *pipe_seq = " | ";
  int pipe_seq_len = strlen(pipe_seq);
  char char_sep[] = {' ', '/', '|'};            /* ordered from most to least desired */
  int num_chars = sizeof(char_sep) / sizeof(char_sep[0]);
  const char *p_str;
  int length;
  int plength;

  p_str = str;
  memset (tmpStr, 0, sizeof(tmpStr));

  do
  {
    found = 0;
    length = strlen(p_str);

    plength = cmdPad - startPos;
    startPos = 0;
    for (i = 0; i < plength; i++)
    {
      ewsTelnetWrite(context," ");
    }

    if((length + cmdPad) > EWS_CLI_COLUMN_WIDTH)
    {
      k1 = EWS_CLI_COLUMN_WIDTH - cmdPad - 1;

      /* First try splitting at a main pipe separator sequence: " | " */
      for (k = k1; k > pipe_seq_len - 1; k--)
      {
        if (memcmp(&p_str[k - pipe_seq_len + 1], pipe_seq, pipe_seq_len) == 0)
        {
          found = 1;
          break;
        }
      }

      /* Sequentially check for single character separators in the following
       * order:
       *   1. Space character
       *   2. Forward slash
       *   3. Single pipe character
       *
       * Run the inner loop to completion once for each character in the set.
       */
      for (i = 0; (i < num_chars) && (found == 0); i++)
      {
        for (k = k1; k > 0; k--)
        {
          if (p_str[k] == char_sep[i])
          {
            found = 1;
            break;
          }
        }
      }
      for (j = 0; j <= k; j++)
      {
        sprintf(tmpStr,"%c", p_str[j]);
        ewsTelnetWrite(context,tmpStr);
      }
      ewsTelnetWrite(context,pStrInfo_common_CrLf);
      p_str += (k + 1);
    }
    else
    {
      ewsTelnetWrite(context, p_str);
      done = 1;
    }

  } while (done == 0);

}

/*
 * ewsCliTrapNotificationMsgGet
 * Get the trap notification message to be displayed.
 *
 *   context  - context of request
 *
 */
void ewsCliTrapNotificationMsgGet(EwsContext context)
{
  /* get the trap notification message and display it */
  ewsSetTelnetStackingTrapMsg(context, cliTrapMsgStringGet());
  cliSyntaxNewLine(context);
  ewsTelnetWrite(context, context->telnet->trapString);
  cliSyntaxNewLine(context);
}

/*********************************************************************
*
* @purpose  Determine if the hidden support tree can be displayed for
*           this session
*
* @param   void
*
* @return  TRUE or FALSE
*
* @end
*
*********************************************************************/
static boolean ewsCliTreeHiddenDebugSupportFunctionIsEnabled(void)
{
  L7_int32 current_handle;

  current_handle = cliCurrentHandleGet ();

  if (current_handle < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    if (loginSessionSupportFeatureHiddenModeGet(cliCommon[current_handle].userLoginSessionIndex) == L7_ENABLE)
    {
      return TRUE;
    }

  }
  return FALSE;
}
