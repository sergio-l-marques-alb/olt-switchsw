/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2008
 *
 **********************************************************************
 *
 * @filename ews_cli_dump.c
 *
 * @purpose
 *
 * @component emweb
 *
 * @comments
 *
 * @create 06/05/2008
 *
 * @author Rama Sasthri, Kristipati
 * @end
 *
 **********************************************************************/

#include "ews.h"

#include "cli.h"

#define FPRINTF(x,y,z...) if(x == NULL){printf(y,## z);} else{fprintf(x,y,## z);fflush(x);}

extern void ewsCliCmdCountDump (void);

static int printCount = 0;
static int cmdCount = 0;
static int printReadMode = 1;
static int printWriteMode = 1;
static char ewsCliTempBuf[1024];

static FILE *ewsCliDumpStart (int isFile, char *name)
{
  FILE *cliXmlFp = NULL;
  if (isFile)
  {
    cliXmlFp = fopen (name, "w");
    if (cliXmlFp == NULL)
    {
      printf ("Failed to create cli.xml\n");
      return NULL;
    }
  }
  return cliXmlFp;
}

static FILE *ewsCliDumpEnd (FILE * cliXmlFp)
{
  if (cliXmlFp)
  {
    fflush (cliXmlFp);
    fclose (cliXmlFp);
  }
  return NULL;
}

static char *ewsCliModeNameGet (int value)
{
  switch (value)
  {
  case L7_ACL_MODE:
    return "L7_ACL_MODE";
    /*pass-through */
  case L7_ACL_RULE_MODE:
    return "L7_ACL_RULE_MODE";
    /*pass-through */
  case L7_ARP_ACL_CONFIG_MODE:
    return "L7_ARP_ACL_CONFIG_MODE";
    /*pass-through */
  case L7_CAPTIVEPORTAL_MODE:
    return "L7_CAPTIVEPORTAL_MODE";
    /*pass-through */
  case L7_CLASS_MAP_IPV6_MODE:
    return "L7_CLASS_MAP_IPV6_MODE";
    /*pass-through */
  case L7_CLASS_MAP_MODE:
    return "L7_CLASS_MAP_MODE";
    /*pass-through */
  case L7_CPENCODED_IMAGE_MODE:
    return "L7_CPENCODED_IMAGE_MODE";
    /*pass-through */
  case L7_CPINSTANCE_LOCALE_MODE:
    return "L7_CPINSTANCE_LOCALE_MODE";
    /*pass-through */
  case L7_CPINSTANCE_MODE:
    return "L7_CPINSTANCE_MODE";
    /*pass-through */
  case L7_DHCP6S_POOL_CONFIG_MODE:
    return "L7_DHCP6S_POOL_CONFIG_MODE";
    /*pass-through */
  case L7_DHCP_POOL_CONFIG_MODE:
    return "L7_DHCP_POOL_CONFIG_MODE";
    /*pass-through */
  case L7_FIRST_MODE:
    return "L7_FIRST_MODE";
    /*pass-through */
  case L7_GLOBAL_CONFIG_MODE:
    return "L7_GLOBAL_CONFIG_MODE";
    /*pass-through */
  case L7_GLOBAL_SUPPORT_MODE:
    return "L7_GLOBAL_SUPPORT_MODE";
    /*pass-through */
  case L7_HIDDEN_COMMAND_MODE:
    return "L7_HIDDEN_COMMAND_MODE";
    /*pass-through */
  case L7_INTERFACE_CONFIG_MODE:
    return "L7_INTERFACE_CONFIG_MODE";
    /*pass-through */
  case L7_IPV4_ACCESS_LIST_CONFIG_MODE:
    return "L7_IPV4_ACCESS_LIST_CONFIG_MODE";
    /*pass-through */
  case L7_IPV6_ACCESS_LIST_CONFIG_MODE:
    return "L7_IPV6_ACCESS_LIST_CONFIG_MODE";
    /*pass-through */
  case L7_LAST_MODE:
    return "L7_LAST_MODE";
    /*pass-through */
  case L7_LINE_CONFIG_CONSOLE_MODE:
    return "L7_LINE_CONFIG_CONSOLE_MODE";
    /*pass-through */
  case L7_LINE_CONFIG_SSH_MODE:
    return "L7_LINE_CONFIG_SSH_MODE";
    /*pass-through */
  case L7_LINE_CONFIG_TELNET_MODE:
    return "L7_LINE_CONFIG_TELNET_MODE";
    /*pass-through */
  case L7_LOOPBACK_CONFIG_MODE:
    return "L7_LOOPBACK_CONFIG_MODE";
    /*pass-through */
  case L7_MAC_ACCESS_LIST_CONFIG_MODE:
    return "L7_MAC_ACCESS_LIST_CONFIG_MODE";
    /*pass-through */
  case L7_MAINTENANCE_MODE:
    return "L7_MAINTENANCE_MODE";
    /*pass-through */
  case L7_POLICY_CLASS_MODE:
    return "L7_POLICY_CLASS_MODE";
    /*pass-through */
  case L7_POLICY_MAP_MODE:
    return "L7_POLICY_MAP_MODE";
    /*pass-through */
  case L7_PRIVILEGE_USER_MODE:
    return "L7_PRIVILEGE_USER_MODE";
    /*pass-through */
  case L7_ROUTER_CONFIG_BGP4_MODE:
    return "L7_ROUTER_CONFIG_BGP4_MODE";
    /*pass-through */
  case L7_ROUTER_CONFIG_OSPF_MODE:
    return "L7_ROUTER_CONFIG_OSPF_MODE";
    /*pass-through */
  case L7_ROUTER_CONFIG_OSPFV3_MODE:
    return "L7_ROUTER_CONFIG_OSPFV3_MODE";
    /*pass-through */
  case L7_ROUTER_CONFIG_RIP_MODE:
    return "L7_ROUTER_CONFIG_RIP_MODE";
    /*pass-through */
  case L7_STACK_MODE:
    return "L7_STACK_MODE";
    /*pass-through */
  case L7_TACACS_MODE:
    return "L7_TACACS_MODE";
    /*pass-through */
  case L7_TUNNEL_CONFIG_MODE:
    return "L7_TUNNEL_CONFIG_MODE";
    /*pass-through */
  case L7_USER_EXEC_MODE:
    return "L7_USER_EXEC_MODE";
    /*pass-through */
  case L7_VLAN_MODE:
    return "L7_VLAN_MODE";
    /*pass-through */
  case L7_WIRELESS_AP_CONFIG_MODE:
    return "L7_WIRELESS_AP_CONFIG_MODE";
    /*pass-through */
  case L7_WIRELESS_AP_PROFILE_CONFIG_MODE:
    return "L7_WIRELESS_AP_PROFILE_CONFIG_MODE";
    /*pass-through */
  case L7_WIRELESS_AP_PROFILE_RADIO_CONFIG_MODE:
    return "L7_WIRELESS_AP_PROFILE_RADIO_CONFIG_MODE";
    /*pass-through */
  case L7_WIRELESS_AP_PROFILE_VAP_CONFIG_MODE:
    return "L7_WIRELESS_AP_PROFILE_VAP_CONFIG_MODE";
    /*pass-through */
  case L7_WIRELESS_CONFIG_MODE:
    return "L7_WIRELESS_CONFIG_MODE";
    /*pass-through */
  case L7_WIRELESS_NETWORK_CONFIG_MODE:
    return "L7_WIRELESS_NETWORK_CONFIG_MODE";
    /*pass-through */
  case L7_DOT1AG_MAINTAINANCE_DOMAIN_MODE:
    return "L7_DOT1AG_MAINTAINANCE_DOMAIN_MODE";
    /*pass-through */
  case L7_TIMERANGE_CONFIG_MODE:
    return "L7_TIMERANGE_CONFIG_MODE";
    /*pass-through */
  default:
    break;
  }
  return NULL;
}

static void ewsCliPrint2Root (FILE * cliXmlFp, struct EwsCliCommand_s *parent)
{
  char buf1[512];
  char buf2[512];

  buf1[0] = buf2[0] = 0;
  while (parent->parent)
  {
    sprintf (buf1, "%s%s", parent->command, buf2);
    strcpy (buf2, buf1);
    parent = parent->parent;
  }
  FPRINTF (cliXmlFp, "%s\r\n", buf2);
}

static void ewsCliPrintNode (FILE * cliXmlFp, struct EwsCliCommand_s *parent)
{
  struct EwsCliCommand_s *np, *np_firstchild;

  if (parent == NULL)
  {
    return;
  }

  if (parent->first_child == NULL || L7_NODE_FLAGS_IS_TREE(parent))
  {
    ewsCliPrint2Root (cliXmlFp, parent);
    cmdCount++;
    return;
  }

  np = np_firstchild = parent->first_child;
  do
  {
    ewsCliPrintNode (cliXmlFp, np);
    np = (struct EwsCliCommand_s *) EWS_LINK_NEXT (np->siblings);
  }
  while (np_firstchild != np);

}

void ewsCliIntentSet (int w, int r)
{
  printReadMode = r;
  printWriteMode = w;
}

int ewsCliPrintTree (int isFile)
{
  int i;
  FILE *cliXmlFp = ewsCliDumpStart (isFile, "cli.txt");

  FPRINTF (cliXmlFp, "!===================== START ========================\r\n");
  cmdCount = 0;
  for (i = 0; i < L7_CLIMODE_COUNT; i++)
  {
    FPRINTF (cliXmlFp, "!MODE : %s\n", ewsCliModeNameGet (i + L7_FIRST_MODE));
    if (printReadMode)
    {
      ewsCliPrintNode (cliXmlFp, cliModeRO.cliModeNode[i]);
    }
    if (printWriteMode)
    {
      ewsCliPrintNode (cliXmlFp, cliModeRW.cliModeNode[i]);
    }
  }

  FPRINTF (cliXmlFp, "!===================== END ========================\r\n");
  cliXmlFp = ewsCliDumpEnd (cliXmlFp);
  printf ("Number of Commands = %d\n", cmdCount);
  printf ("INTENT: ReadMode = %d WriteMode = %d\r\n", printReadMode, printWriteMode);
  ewsCliCmdCountDump ();
  return cmdCount;
}

static char *ewsCliXmlStrCpy (const char *src)
{
  char *cp = ewsCliTempBuf;
  int i, len = strlen (src);

  for (i = 0; i < len; i++)
  {
    switch (src[i])
    {
    case '<':
      strcpy (cp, "&lt;");
      cp += 4;
      break;
    case '>':
      strcpy (cp, "&gt;");
      cp += 4;
      break;
    case '&':
      strcpy (cp, "&amp;");
      cp += 5;
      break;
    default:
      *cp++ = src[i];
      break;
    }
  }
  *cp = 0;

  return ewsCliTempBuf;
}

static void ewsCliXmlPrintNode (FILE * cliXmlFp, struct EwsCliCommand_s *parent, char *lvl)
{
  const char *help;
  struct EwsCliCommand_s *np, *np_firstchild;

  if (parent == NULL)
  {
    return;
  }

  if (parent->command != NULL)
  {
    FPRINTF (cliXmlFp, "<token ");
    if (strchr (parent->command, '<'))
    {
      FPRINTF (cliXmlFp, "argobj=\"TO_BE_FILLED\" ");
    }
    FPRINTF (cliXmlFp, "name=\"%s\" ", ewsCliXmlStrCpy (parent->command));
    help = parent->description ? parent->description : "";
    FPRINTF (cliXmlFp, "help=\"%s\" ", ewsCliXmlStrCpy (help));
    FPRINTF (cliXmlFp, "level=\"%s\" ", lvl);
    FPRINTF (cliXmlFp, ">\n");
    printCount++;
  }

  if (parent->first_child == NULL)
  {
    FPRINTF (cliXmlFp, "<action ");
    if (parent->mode != 0)
    {
      FPRINTF (cliXmlFp, "type=\"chg-mode\" ");
      FPRINTF (cliXmlFp, "target-mode=\"%s\" ", ewsCliModeNameGet (parent->mode));
    }
    else
    {
      FPRINTF (cliXmlFp, "type=\"call-func\" ");
      FPRINTF (cliXmlFp, "target-func=\"xCliCommandHandlerDump\" ");
    }
    FPRINTF (cliXmlFp, ">\n");
    FPRINTF (cliXmlFp, "</action>\n");

    if (printCount > 0)
    {
      FPRINTF (cliXmlFp, "</token>\n");
      printCount--;
    }
    return;
  }

  np = np_firstchild = parent->first_child;
  do
  {
    ewsCliXmlPrintNode (cliXmlFp, np, lvl);
    np = (struct EwsCliCommand_s *) EWS_LINK_NEXT (np->siblings);
  }
  while (np_firstchild != np);

  if (printCount > 0)
  {
    FPRINTF (cliXmlFp, "</token>\n");
    printCount--;
  }
}

void ewsCliXmlPrintTree (int isFile)
{
  int i;
  FILE *cliXmlFp = ewsCliDumpStart (isFile, "cli.xml");

  FPRINTF (cliXmlFp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  FPRINTF (cliXmlFp, "<!DOCTYPE FastPathXML SYSTEM  \"cli.dtd\" >\n");
  FPRINTF (cliXmlFp, "<FastPathXML>\n");
  FPRINTF (cliXmlFp, "<clixml name=\"cli2\" version=\"1.0\">\n");

  for (i = 0; i < L7_CLIMODE_COUNT; i++)
  {
    if (cliModeRO.cliModeNode[i] == NULL && cliModeRW.cliModeNode[i] == NULL)
    {
      continue;
    }
    FPRINTF (cliXmlFp, "<tokens mode=\"%s\">\n", ewsCliModeNameGet (i + L7_FIRST_MODE));
    if (printReadMode)
    {
      ewsCliXmlPrintNode (cliXmlFp, cliModeRO.cliModeNode[i], "ReadOnlyUser");
    }
    if (printWriteMode)
    {
      ewsCliXmlPrintNode (cliXmlFp, cliModeRW.cliModeNode[i], "ReadWriteUser");
    }
    while (printCount > 0)
    {
      FPRINTF (cliXmlFp, "</token>\n");
      printCount--;
    }
    FPRINTF (cliXmlFp, "</tokens>\n");
  }

  FPRINTF (cliXmlFp, "</clixml>\n");
  FPRINTF (cliXmlFp, "</FastPathXML>\n");

  cliXmlFp = ewsCliDumpEnd (cliXmlFp);

  printf ("INTENT: ReadMode = %d WriteMode = %d\r\n", printReadMode, printWriteMode);
  ewsCliCmdCountDump ();
}
