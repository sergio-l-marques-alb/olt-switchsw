//**************************************************************************************
//
//ABSTRACT:
//Command Line Interface for UNICOM board. Enforces command syntax. Actual get/set functions in...
//...T_CLI_SET_GET_FUNCTIONS
//Mapping   unicom/fw eth ports <> management eth ports   through...
//...unicom_eth_TO_man_eth_MAP_S14_BKE and man_eth_TO_unicom_eth_MAP_S14_BKE
//
//HISTORY:
//
//
//COPYRIGHT:
//PT Inova��o (a.k.a. PTIn, a.k.a. CET)
//
//
//CONTACT:
//Rui Costa
//
//**************************************************************************************

#include <stdio.h>
#include <cli_olt.h>
#include <string.h>
#include <stdarg.h>

extern int phy_port_config(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE]);
extern int phy_port_config_get(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE]);
extern int phy_port_state_get(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE]);
extern int phy_port_stat_get(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE]);
extern int phy_port_stat_clear(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE]);

extern int evc_create(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE]);
extern int evc_remove(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE]);
extern int evc_intf_add(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE]);
extern int evc_intf_remove(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE]);

extern int fp_ping(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE]);
extern int redirect_stdout(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE]);
extern int redirect_logger(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE]);
extern int board_show(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE]);

const command_struct command[] = {
  {
    "dbg",
    "Redirect stdout to the given console",
    "[OUTput=<console>]",
    1,
    {"out"},
    redirect_stdout,
    "Application Tools"
  },
  {
    "logger",
    "Redirect logger output to a file",
    "Index=<0:SWlogger/1:SDKlogger> Name=<filename>",
    2,
    {"i","n"},
    redirect_logger,
  },
  {
    "ping",
    "Verify switchdrvr running state",
    "Period=(seconds) Tries=(#retries)",
    2,
    {"p","t"},
    fp_ping,
  },
  {
    "boardshow",
    "Show board information",
    "(none)",
    0,
    {},
    board_show,
  },
  {
    "phyconfig",
    "Configure physical port settings",
    "Port=<portId> [ENable=(0/1)] [SPeed=<0:10Mbps/1:100Mbps/2:Autoneg/3:1Gbps/4:2.5Gbps/5:10Gbps>] [FDuplex=(0/1)] [FMax=<frameMax(bytes)>]",
    5,
    {"p","en","sp","fd","fm"},
    phy_port_config,
    "Phy Port Commands"
  },
  {
    "phyconfig_get",
    "Get physical port settings",
    "Port=<portId>",
    1,
    {"p"},
    phy_port_config_get,
  },
  {
    "phystate",
    "Get physical port state",
    "Port=<portId>",
    1,
    {"p"},
    phy_port_state_get,
  },
  {
    "phystats",
    "Read physical port statistics",
    "Port=<portId>",
    1,
    {"p"},
    phy_port_stat_get,
  },
  {
    "phystats_clear",
    "Clear physical port statistics",
    "Port=<portId>",
    1,
    {"p"},
    phy_port_stat_clear,
  },
  {
    "evc_create",
    "Create EVC",
    "EVC=<EVCid> [TYPe=<0-P2MP;1-P2P;2-Q>] [STacked=<0/1>] [MAClearn=<0/1>] [MCFlood=<0-All;1-Unkn;2-None>] [FLags=<mask>]",
    6,
    {"evc","typ","st","mac","mcf","fl"},
    evc_create,
    "EVCs Management"
  },
  {
    "evc_remove",
    "Remove EVC",
    "EVC=<EvcId>",
    1,
    {"evc"},
    evc_remove,
  },
  {
    "evc_port_add",
    "Add port to EVC",
    "EVC=<EvcId> Port=<type>/<id> [MEFtype=<0-Root/1-Leaf>] [OVid=<OuterVLAN>] [IVid=<InnerVlan>] [PCP=<prio>] [ETHtype=<etherType>]",
    7,
    {"evc","p","mef","ov","iv","pcp","eth"},
    evc_intf_add,
  },
  {
    "evc_port_remove",
    "Remove port from EVC",
    "EVC=<EvcId> Port=<type>/<id> [OVid=<OuterVLAN>] [IVid=<InnerVlan>] [PCP=<prio>] [ETHtype=<etherType>]",
    6,
    {"evc","p","ov","iv","pcp","eth"},
    evc_intf_remove,
  },
};



#define IS_EOL(ch)  ((ch) == '\0' || (ch) == ' ' || (ch) == ';' || (ch)=='\r' || (ch)=='\n' || (ch) == '\t')
static int param_parse(int argc, const char **argv, char assign_char, char (*search_token)[MAX_PARAM_NAME_SIZE], char (*str_result)[MAX_PARAM_VALUE_SIZE], int number_of_params)
{
  int processed_params=0;
  int i, arg, param, length, length_to_compare;
  const char *ptr;

  /* Run all expected parameters */
  for (param=0; param<number_of_params; param++)
  {
    /* Reset result */
    str_result[param][0] = '\0';

    /* Run all given parameters */
    for (arg=0; arg<argc; arg++)
    {
      ptr = argv[arg];

      /* If parameter is given, search for it... */
      if (search_token[param][0] != '\0')
      {
        /* Search for assign char (or end of line) */
        for (i=0; i<MAX_PARAM_NAME_SIZE-1; i++)
        {
          if (ptr[i] == assign_char || IS_EOL(ptr[i]))
            break;
        }
        length = i;
        length_to_compare = strlen(search_token[param]);

        /* Check end of line, or if no command is given */
        if (length == 0 || IS_EOL(ptr[i]))  continue;

        /* Token is different: skip to next argument (but still within same parameter) */
        if (length_to_compare > length || strncmp(ptr, search_token[param], length_to_compare) != 0)
        {
          continue;
        }

        /* Token is found! */

        /* Goto assign char */
        ptr += length;
        while (*ptr != assign_char && !IS_EOL(*ptr))  ++ptr;

        if (*ptr != assign_char)  continue;
        /* Goto char after assign */
        ptr++;
      }

      /* Analysing value... */

      /* Search for space character or end of line */
      for (i=0; i<MAX_PARAM_VALUE_SIZE-1; i++)
      {
        if (IS_EOL(ptr[i]))
          break;
      }
      length = i;

      strncpy(str_result[param], ptr, length);
      str_result[param][length] = '\0';

      /* One more param processed */
      processed_params++;

      /* Skip to the next parameter */
      break;
    }
  }

  /* Print results */
  printf("Obtaining up to %u params: ", number_of_params);
  for (i=0; i<number_of_params; i++)
  {
    if (search_token[i][0]!='\0')  printf("%s:",search_token[i]);
    if (str_result[i][0]!='\0')
      printf("%s ", str_result[i]);
    else
      printf("X ");
  }
  if (i == 0)
    printf("none");
  printf("\r\n");

  return processed_params;
}



static int help_cli_unicom(int argc, const char **argv) {

  int i;

  printf("Available commands:\r\n");

  for (i=0; i<sizeof(command)/sizeof(command_struct); i++)
  {
    if (command[i].separator[0] != '\0') {
      printf("---> %s\r\n", command[i].separator);
    }
    printf("\t%-16s - %s\r\n",command[i].name, command[i].description);
  }
  printf("**** Type '%s <command> help' to get more help ****\r\n", argv[0]);

  return 1;
}//help_cli_unicom


//Receives and analyses a command line with its specific parameters, ...
//...calls the appropriate help, in case of syntax errors, ...
//...calls the appropriate function in struct "T_CLI_SET_GET_FUNCTIONS" with the appropriate parameters,
//to execute the command.
//RETURN CODE: "0 means OK"
int cli_unicom(int argc, const char **argv) {
 u16 i;
 int  nargs;
 char values[MAX_PARAMS][MAX_PARAM_VALUE_SIZE];

 if (argc < 2) return help_cli_unicom(argc, argv);

 for (i = 0; i < sizeof(command)/sizeof(command_struct); i++)
 {
   if (0 == strcmp(argv[1], command[i].name))
   {
     if (argc >= 3 && strcmp(argv[2],"help") == 0)
     {
       printf("%s: %s\r\n", command[i].name, command[i].description);
       printf("Syntax => %s %s %s\r\n", argv[0], command[i].name, command[i].syntax);
     }
     else if ((nargs=param_parse(argc-2, (const char **) &argv[2], '=', (char (*)[MAX_PARAM_NAME_SIZE]) command[i].params, values, command[i].params_max)) < 0)
     {
       printf("Error parsing arguments\r\n");
       printf("Syntax -> %s %s %s\r\n", argv[0], command[i].name, command[i].syntax);
     }
     else if (command[i].function(nargs, values) != 0)
     {
       printf("Verify syntax => %s %s %s\r\n", argv[0], command[i].name, command[i].syntax);
     }
     return 0;
   }
 }

 return help_cli_unicom(argc, argv);
}//cli_unicom

