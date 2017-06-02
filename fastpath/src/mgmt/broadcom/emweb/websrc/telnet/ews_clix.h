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
 * EmWeb/CLI internal interfaces
 *
 */

#ifndef _EWS_CLIX_H_
#define _EWS_CLIX_H_ 1 /* version */

#include "strlib_common_common.h"
#include "strlib_common_common.h"
#include "strlib_base_common.h"
#include "strlib_base_common.h"
#include "ews_cli.h"

#ifdef EW_CONFIG_OPTION_CLI

#define MAX_OPTIONS_PER_NODE 1
#define MAX_IGNORES_PER_NODE 1
#define MAX_SAME_AS_PER_NODE 1
#define NODE_MAX_DISPLAY_COUNT_DEFAULT 1    /* LVL7_P51286 */

#define L7_MAX_DATA_TYPE_ERROR_MESSAGE   1023

/* Using the flags will reduce the memory for nodes */
#define L7_NODE_FLAGS_PHYS_IF_VALID 0x01
#define L7_NODE_FLAGS_LAG_IF_VALID  0x02
#define L7_NODE_FLAGS_RTR_IF_VALID  0x04
#define L7_NODE_FLAGS_LOG_VLAN_IF_VALID  0x08

#define L7_NODE_FLAGS_SUPPORT       0x10
#define L7_NODE_FLAGS_EXEC_NORMAL   0x20
#define L7_NODE_FLAGS_EXEC_NO       0x40
#define L7_NODE_FLAGS_TREE          0x80

#define L7_NODE_FLAGS_IS_SUPPORT(np)      ((np)->flags & L7_NODE_FLAGS_SUPPORT)
#define L7_NODE_FLAGS_IS_EXEC_NO(np)      ((np)->flags & L7_NODE_FLAGS_EXEC_NO)
#define L7_NODE_FLAGS_IS_EXEC_NORMAL(np)  ((np)->flags & L7_NODE_FLAGS_EXEC_NORMAL)
#define L7_NODE_FLAGS_IS_TREE(np)         ((np)->flags & L7_NODE_FLAGS_TREE)

#define L7_NODE_FLAGS_IS_PHYS_IF_VALID(np)      ((np)->flags & L7_NODE_FLAGS_PHYS_IF_VALID)
#define L7_NODE_FLAGS_IS_LAG_IF_VALID(np)      ((np)->flags & L7_NODE_FLAGS_LAG_IF_VALID)
#define L7_NODE_FLAGS_IS_RTR_IF_VALID(np)      ((np)->flags & L7_NODE_FLAGS_RTR_IF_VALID)
#define L7_NODE_FLAGS_IS_LOG_VLAN_IF_VALID(np)      ((np)->flags & L7_NODE_FLAGS_LOG_VLAN_IF_VALID)

#define L7_NODE_FLAGS_SET_PHYS_IF_VALID(np) ((np)->flags |= L7_NODE_FLAGS_PHYS_IF_VALID)
#define L7_NODE_FLAGS_SET_LAG_IF_VALID(np) ((np)->flags |= L7_NODE_FLAGS_LAG_IF_VALID)
#define L7_NODE_FLAGS_SET_RTR_IF_VALID(np) ((np)->flags |= L7_NODE_FLAGS_RTR_IF_VALID)
#define L7_NODE_FLAGS_SET_LOG_VLAN_IF_VALID(np) ((np)->flags |= L7_NODE_FLAGS_LOG_VLAN_IF_VALID)

#define L7_NODE_FLAGS_SET_SUPPORT(np) ((np)->flags |= L7_NODE_FLAGS_SUPPORT)
#define L7_NODE_FLAGS_SET_EXEC_NO(np) ((np)->flags |= L7_NODE_FLAGS_EXEC_NO)
#define L7_NODE_FLAGS_SET_EXEC_NORMAL(np) ((np)->flags |= L7_NODE_FLAGS_EXEC_NORMAL)
#define L7_NODE_FLAGS_SET_TREE(np) ((np)->flags |= L7_NODE_FLAGS_TREE)

#define L7_NODE_FLAGS_CLR_SUPPORT(np) ((np)->flags &= ~L7_NODE_FLAGS_SUPPORT)
#define L7_NODE_FLAGS_CLR_EXEC_NO(np) ((np)->flags &= ~L7_NODE_FLAGS_EXEC_NO)
#define L7_NODE_FLAGS_CLR_EXEC_NORMAL(np) ((np)->flags &= ~L7_NODE_FLAGS_EXEC_NORMAL)
#define L7_NODE_FLAGS_CLR_TREE(np) ((np)->flags &= ~L7_NODE_FLAGS_TREE)

typedef struct EwsCliCommand_s
{
  EwsLink siblings;
  const char * command;
  const char * description;
  const char * syntax_normal;
  const char * syntax_no;
  struct EwsCliCommand_s * parent;
  struct EwsCliCommand_s * first_child;
  EwaCli_f * action;
  struct EwsCliCommandAllocator_s * allocator;
  int16 refcount;
  int16 maxDisplayCount;/*LVL7_P51286 maximum number of times to allow node to be used in a command*/
  int16 componentId; /*to store advance keying enabled component id*/
  int16 featureId;   /*to store advance keying enabled feature id (only valid when componentId is also specified)*/
  EwsCliNodeNoFormStatus noFormStatus;/*to store the state as pertaining to no / normal form*/
  struct EwsCliCommand_s * optionNodes[MAX_OPTIONS_PER_NODE];/*array to store list of options nodes*/
  struct EwsCliCommand_s * ignoreNodes[MAX_IGNORES_PER_NODE];/*array to store list of ignore nodes*/
  struct EwsCliCommand_s * sameAsNodes[MAX_SAME_AS_PER_NODE];/*array to store list of same as nodes*/
  L7_uchar8 dataType;  /*to store the data type of the node*/
  union
  {
    L7_uint32 minu32;  /*to store the minimum length of the node*/
    L7_int32 min32;   /*to store the minimum length of the node*/
  } minLength;
  union
  {
    L7_uint32 maxu32;  /*to store the maximum length of the node*/
    L7_int32 max32;   /*to store the maximum length of the node*/
  } maxLength;
  L7_uchar8 mode;    /* the mode the command can trigger change to */
  L7_uchar8 flags;   /* various flags */
  L7_uchar8 cmdPad;   /* command width padding */
} EwsCliCommand;

/*
 * EwsCliCommandAllocator_s defines the routines to be
 * used for allocating and deallocating command entries.
 *
 * The deallocator routine may be null, in which case
 * entries will not be deleted (and some other deallocation
 * scheme outside of EmWeb should be used by the caller).
 *
 */

typedef struct EwsCliCommandAllocatorExtendExtra *
EwsCliCommandAllocatorExtendP;

typedef struct EwsCliCommandAllocator_s
{
  EwsCliCommand * (* allocate)(const char * command, const char * descr);
  void (*deallocate)(EwsCliCommand *ptr);
  EwsCliCommandAllocatorExtendP extra;
} EwsCliCommandAllocator;

typedef enum EwsCliCommandOptions_e
{
  L7_NO_OPTION = 0,       /* range checking */
  L7_KEY_RESTRICTED_NODE, /* for a node to be restricted by advance keying */
  L7_FEATURE_DEP_NODE,    /* for a node to be restricted by advance keying (only valid when L7_KEY_RESTRICTED_NODE is also specified)*/
  L7_OPTIONS_NODE,        /* for a node pointing to other node to use their child nodes as own child nodes*/
  L7_IGNORE_NODE,         /* for a node to ignore an existing node to be considered as child*/
  L7_SAME_AS_NODE,        /* for a node to treat some other node to be same as itself*/
  L7_NO_COMMAND_SUPPORTED,/* for a node to have special values pertaining to no / mormal form*/
  L7_NODE_DATA_TYPE,      /* for a node to have its data type specified*/
  L7_NODE_MAX_DISPLAY,    /* LVL7_P51286 for a node to be displayed for up to n times (primarily for options treatment) */
  L7_NODE_HIDDEN_SUPPORT_NODE, /* for a node on a single session to be restricted by entering an l7support key */
  L7_NODE_STRING_RANGE,   /* for a node to have a maximum length */
  L7_NODE_INT_RANGE,      /* for a node to have a signed integer range */
  L7_NODE_UINT_RANGE,     /* for a node to have an unsigned integer range */
  L7_SYNTAX_NORMAL,       /* command syntax for 'normal' command */
  L7_SYNTAX_NO,           /* command syntax for 'no' command */
  L7_NODE_TYPE_MODE,      /* to indicate (at <cr>) if the command is mode type */
  L7_NODE_IF_TYPES,       /* to indicate interfaces valid for this command */
  L7_LAST_OPTION          /* this must be last */
} EwsCliCommandOptions;

typedef struct EwsCliState_s
{
  uintf historyTop;    /* top of history */
  uintf historyBottom; /* bottom of history */
  uintf historyIndex;  /* current line in history */
  uintf historySearch; /* current history search */
  sintf lineIndex;     /* current character in line */
  char line[EWS_CLI_MAX_HISTORY][EWS_CLI_MAX_LINE]; /* history buffer */
  char arg_buffer[EWS_CLI_MAX_LINE];                /* parse buffer */
  uintf argc;                    /* number of arguments */
  char * argv[EWS_CLI_MAX_ARGS]; /* argument list */

  EwsCliCommandP parseList[EWS_CLI_MAX_ARGS]; /* parsed nodes */
  uintf parseCount;                  /* number of nodes matched */

  uintf depth;                   /* depth of pre-defined arguments */
  char depth_buffer[EWS_CLI_MAX_LINE]; /* storage for depth arguments */
  char deletedBuffer[EWS_CLI_MAX_LINE]; /* storage for deleted chars */
  boolean arrow_esc;
  boolean arrow_lb;
  boolean dataCheckDisable;
} EwsCliState_t;

typedef struct EwsCliState_s * EwsCliState;
typedef struct EwsCliCommandLine_s
{
  char * command_list[EWS_CLI_MAX_ARGS];
  uintf args;
} EwsCliCommandLine;

typedef struct EwsCliCommandLine_s * EwsCliCommandLineP;

/*
 * Internal functions
 */
void ewsCliStart(EwsContext context);
void ewsCliCleanup(EwsContext context);
void ewsCliInit(void);
void ewsCliShutdown(void);
EwsCliCommandP ewsCliCheckSibling (EwsCliCommandP node);
EwsCliCommandP ewsCliCheckOffspring (EwsCliCommandP node);
void ewsCliParseCommandLine(const char * real_command
                            ,EwsCliCommandLineP command_p);
void ewsCliKillLine ( EwsContext context, EwsCliState cli );
void ewsCliParseLine( EwsContext context, EwsCliState cli );
void ewsCliPossible ( EwsContext context
                     ,boolean full_help
                     ,const char * command
                     ,const char * description
                     ,uintf * column
                     ,unsigned char pad
);
boolean ewsCliParseTree (EwsContext context, EwsCliState cli, EwsCliCommandP root, sintf * marker, uintf * token, char * dataTypeErrorMessage);
EwsCliCommandP ewsCliParseSimple( EwsCliState cli, EwsCliCommandP mp );

#endif /* EW_CONFIG_OPTION_CLI */

#endif /* _EWS_CLIX_H_ */
