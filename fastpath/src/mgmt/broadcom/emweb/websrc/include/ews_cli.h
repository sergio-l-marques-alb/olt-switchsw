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
 * EmWeb/CLI application interfaces
 *
 */

#ifndef _EWS_CLI_H_
#define _EWS_CLI_H_ 1 /* version */

#include "ew_types.h"
#include "ew_config.h"

#include "emweb_config.h"

#ifdef EW_CONFIG_OPTION_CLI

/*
 * Maximum number of arguments.  Rather than rely on variable argument
 * lists (not supported in many cross development environments), or
 * the overhead of linked lists, we'll simply fix the number of arguments
 * allowed on a command to EWS_CLI_MAX_ARGS.
 */
#ifndef EWS_CLI_MAX_ARGS
#define EWS_CLI_MAX_ARGS 32
#endif

/*
 * Maximum length of command line in characters.
 */
#ifndef EWS_CLI_MAX_LINE
#define EWS_CLI_MAX_LINE 1536  /* LVL7_P0006 */
#endif

/*
 * Maximum lines in history buffer
 */
#ifndef EWS_CLI_MAX_HISTORY
#define EWS_CLI_MAX_HISTORY 16  /* LVL7  */
#endif


/*
 * Application CLI handler function:
 *
 * EwaCli_f
 *
 *   context - context of request
 *   argc    - number of arguments (including name of command)
 *   argv    - array of text arguments
 *   index   - Always zero for EWS_CLI_SIMPLE style command lines
 *             For EWS_CLI_TREE, argv[0] is the first matched token in the
 *             parse tree, argv[index] is the token corresponding to the
 *             call to this action routine.  Thus, the same action may be
 *             invoked from different parts of the tree.
 *
 * Returns text to be returned from ewaTelnetData().
 */
typedef const char * EwaCli_f (
    EwsContext context
    ,uintf      argc
    ,const char **argv
    ,uintf      indx
    );



/*
 * ALTERNATIVE #2: A Cisco-like CLI with a parse tree of commands
 *
 * Use ewsCliAddNode() to build a parse tree of commands.
 */
typedef struct EwsCliCommand_s *EwsCliCommandP;
typedef struct EwsCliCommandAllocator_s *EwsCliCommandAllocatorP;

/*
 * ewsCliAddNode
 * Create a new parse tree, or add a node to an existing parse tree
 *
 * parent  - pointer to parent node, or NULL to create a new root node
 * command - pointer to string containing command word in parse node
 * description - pointer to string containing additional description, or NULL
 * action  - application function to call, or NULL.  The action of the root
 *           node will be called if no other action is found for a command
 *           line (i.e. if the command line is not parsable according to the
 *           rules).
 *
 * Returns new node.
 *
 * This function adds a new command word to the parse tree.  The EmWeb/CLI will
 * traverse the tree according to user input on the command line, handling
 * command-line completion and context-sensitive help accordingly.  When a
 * command is executed, the deepest action will be invoked with argv[0] and argc
 * adjusted to pass the command word and any additional arguments to the
 * application.
 *
 * The root node for a given tree is passed to ewsCliData for processing.
 * This function allows variable number of arguments to be supplied. The
 * number of variable argument is passed in the function through 'optCount'.
 * The optional parameters that may be passed in are defined in
 * enum 'EwsCliCommandOptions_e'. These are special parameters to be set for
 * node so that parser can treat them differently based on runtime conditions.
 */
EwsCliCommandP
ewsCliAddNode( EwsCliCommandP parent
    ,const char     *command
    ,const char     *description
    ,EwaCli_f       *action
    ,unsigned int   optCount
    ,...
    );


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
    ,unsigned int   optCount
    ,...
    );

/*
 * EwsCliNodeNoFormStatus_e
 *
 * Enum to declare possible states of a node pertaining
 * to 'No' / 'Normal' form.
 *
 */
typedef enum EwsCliNodeNoFormStatus_e
{
  L7_STATUS_BOTH
    ,L7_STATUS_NORMAL_ONLY
    ,L7_STATUS_NO_ONLY
} EwsCliNodeNoFormStatus;

/*
 * EwsCliNodeDataType_e
 *
 * Enum to declare possible data types of a node.
 *
 */
typedef enum EwsCliNodeDataType_e
{
  L7_NO_DATA_TYPE  /*default */ /*For range checking*/
    ,L7_MAC_ADDRESS_SVL_DATA_TYPE
    ,L7_IP_ADDRESS_DATA_TYPE
    ,L7_USP_DATA_TYPE
    ,L7_USP_RANGE_DATA_TYPE
    ,L7_SIGNED_INTEGER    /* for integer ranges */
    ,L7_UNSIGNED_INTEGER  /* for integer ranges */
    ,L7_STRING_DATA_TYPE  /* for string length ranges */
    ,L7_IPV6_ADDRESS_DATA_TYPE
    ,L7_IPV6_PREFIX_DATA_TYPE
    ,L7_LAST_DATA_TYPE  /*For range checking*/
} EwsCliNodeDataType;

/*
 * ewsCliAddTree
 * This function links a tree to the bottom of a node by reference.  This is
 * helpful if a subtree can describe a common construct used by many commands
 * wihtout replicating parts of the parse tree.  Note that this can only be
 * done to a node that currently has no children.
 */
void
ewsCliAddTree( EwsCliCommandP parent
    ,EwsCliCommandP tree
    );
/*
 * ewsCliDeleteTree
 * This function deletes a tree.
 */
void
ewsCliDeleteTree( EwsCliCommandP parent);

/*
 * ewsCliDeleteNode
 * Remove and delete a node (and its offspring, if any).
 */
void
ewsCliDeleteNode (EwsCliCommandP node);

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
    ,const char     *command
    ,const char     *description
    ,EwaCli_f       *action
    );

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
EwsCliCommandP
ewsCliFindNode ( EwsCliCommandP parent
    ,const char     *command
    );

/*
 * ewsCliGetCommand
 * Get a command from history buffer
 *
 *   context  - context of request
 *   index    - index to pass in
 *
 * Return text associated with passed in index (example: index = 1 returns
 * previous command) from history buffer, or empty string if no command
 * exists, or NULL if index is out of bounds.
 */
char*
ewsCliGetCommand ( EwsContext context, uintf indx );

/*
 * ewsCliNumCommand
 * Return number of command in history buffer.
 *
 *   context  - context of request
 *
 */
uintf
ewsCliNumCommand ( EwsContext context );

/*
 * ewsCliTrapNotificationMsgGet
 * Get the trap notification message to be displayed.
 *
 *   context  - context of request
 *
 */
void
ewsCliTrapNotificationMsgGet(EwsContext context);

/*
 * ewsCliAllocator
 *
 * Returns an allocator (EwsCliCommandAllocator) that may be passed
 * in to ewsCliAddNodeWithAlloc.
 *
 * This allocator assumes that the descriptions and command strings
 * passed in do not need to be copied, and allocates nodes out of a
 * linked-list of arrays of EwsCliCommand's, in order to reduce the
 * impact of node trees on memory consumption (much less per-node
 * overhead). Deallocation is slow.
 *
 * Note that this is *not* the default allocator.
 */
EwsCliCommandAllocatorP ewsCliAllocator(void);


/*
 * ewsCliData
 * Called from ewaNetTelnetData().  Returned data passed through and returned
 * by ewaNetTelnetData().
 *
 *   context - request context
 *   datap   - pointer to received data
 *   bytesp  - (input/output) number of bytes available (updated)
 *   menu    - pointer to array of valid commands.
 */
const char * ewsCliData ( EwsContext           context
    ,char                 *datap
    ,uintf                *bytesp
    ,EwsCliCommandP       menu
    );

/*
 * ewsCliDepth
 * Set the command depth.  argc/argv specify a prefix command and arguments
 * to which the arguments specified in the command line are appended.  If
 * argc is zero, then no initial command and arguments are prepended to the
 * command line.
 */
void ewsCliDepth ( EwsContext context
    ,uintf      argc
    ,const char **argv
    );

/*
 * Formatting of context-sensitive help
 */
#ifndef EWS_CLI_POSSIBLE_WIDTH
#define EWS_CLI_POSSIBLE_WIDTH 25 /* width of each choice */
#endif

#ifndef EWS_CLI_COLUMN_WIDTH
#define EWS_CLI_COLUMN_WIDTH 79 /* width of each choice */
#endif

#ifndef EWS_CLI_POSSIBLE_COLS
#define EWS_CLI_POSSIBLE_COLS   3 /* max columns across */
#endif

/*
 * Special characters for CLI editing
 */
#ifndef EWS_CLI_CHAR_BEGIN_LINE
#define EWS_CLI_CHAR_BEGIN_LINE (1+'A'-'A') /* ^A:  go to beginning of line */
#endif
#ifndef EWS_CLI_CHAR_END_LINE
#define EWS_CLI_CHAR_END_LINE   (1+'E'-'A') /* ^E:  go to end of line */
#endif
#ifndef EWS_CLI_CHAR_FORWARD
#define EWS_CLI_CHAR_FORWARD    (1+'F'-'A') /* ^F:  go forward character */
#endif
#ifndef EWS_CLI_CHAR_BACKWARD
#define EWS_CLI_CHAR_BACKWARD   (1+'B'-'A') /* ^B:  go backward character */
#endif
#ifndef EWS_CLI_CHAR_DELETE
#define EWS_CLI_CHAR_DELETE     (1+'D'-'A') /* ^D:  delete character */
#endif
#ifndef EWS_CLI_CHAR_KILL0
#define EWS_CLI_CHAR_KILL0      (1+'U'-'A') /* ^U:  kill line backward */
#endif
#ifndef EWS_CLI_CHAR_KILL1
#define EWS_CLI_CHAR_KILL1      (1+'X'-'A') /* ^X:  kill line backward */
#endif
#ifndef EWS_CLI_CHAR_KILL_END
#define EWS_CLI_CHAR_KILL_END   (1+'K'-'A') /* ^K:  kill line forward */
#endif
#ifndef EWS_CLI_CHAR_KILL_WORD
#define EWS_CLI_CHAR_KILL_WORD  (1+'W'-'A') /* ^W:  kill word backward */
#endif
#ifndef EWS_CLI_CHAR_TRANSPOSE
#define EWS_CLI_CHAR_TRANSPOSE  (1+'T'-'A') /* ^T:  transpose characters */
#endif

/* These ctrl key sequences are captured at a lower level within fastpath
 * but are documented for user
 *                                             ^S:  disable serial flow
 *
 *                                             ^Q:  enable serial flow
 */

#ifndef EWS_CLI_CHAR_UP
#define EWS_CLI_CHAR_UP         (1+'P'-'A') /* ^P:  history backward */
#endif
#ifndef EWS_CLI_CHAR_DOWN
#define EWS_CLI_CHAR_DOWN       (1+'N'-'A') /* ^N:  history forward */
#endif
#ifndef EWS_CLI_CHAR_PASTE
#define EWS_CLI_CHAR_PASTE       (1+'R'-'A') /* ^R:  Rewrite line */
#endif
#ifndef EWS_CLI_CHAR_PASTE_RECENTLY_DELETED_CHAR
#define EWS_CLI_CHAR_PASTE_RECENTLY_DELETED_CHAR       (1+'Y'-'A') /* ^Y:  print recently delt edchars */
#endif
#ifndef EWS_CLI_CHAR_DEL_BACK0
#define EWS_CLI_CHAR_DEL_BACK0  '\b'         /* BS:  delete backward */
#endif
#ifndef EWS_CLI_CHAR_DEL_BACK1
#define EWS_CLI_CHAR_DEL_BACK1  '\177'       /* DEL: delete backward */
#endif
#ifndef EWS_CLI_CHAR_COMPLETE
#define EWS_CLI_CHAR_COMPLETE   '\t'         /* TAB: command completion */
#endif
#ifndef EWS_CLI_CHAR_SPACE   /* LVL7_P0006 */
#define EWS_CLI_CHAR_SPACE      ' '          /* SPACE: command completion */
#endif
#ifndef EWS_CLI_COMMENT_CHAR /* LVL7 */
#define EWS_CLI_COMMENT_CHAR '!' /* COMMENT: begin of comment on a line */
#endif
#ifndef EWS_CLI_CHAR_POSSIBLE
#define EWS_CLI_CHAR_POSSIBLE   '?'          /* ?:   possible completions */
#endif
#ifndef EWS_CLI_CHAR_CR
#define EWS_CLI_CHAR_CR         '\r'         /* CR:  parse line */
#endif
#ifndef EWS_CLI_CHAR_ENTER   /* LVL7_P0006 */                                                     /*lvl7_P0006 start*/
#define EWS_CLI_CHAR_ENTER          '\n'         /* CR:  parse line */
#endif
#ifndef EWS_CLI_CHAR_ROOT    /* LVL7_P0006 */
#define EWS_CLI_CHAR_ROOT           (1+'Z'-'A')  /* ^Z:  go back to root */
#endif
/*lvl7_P0006 end*/


/*
 * characters for processing extended keycodes
 */
#ifndef EWS_CLI_CHAR_ESC
#define EWS_CLI_CHAR_ESC        '\033'    /* ESC:  escape */
#endif
#ifndef EWS_CLI_CHAR_LBRACKET
#define EWS_CLI_CHAR_LBRACKET   '['       /* [:  left bracket */
#endif
#ifndef EWS_CLI_CHAR_A_UP
#define EWS_CLI_CHAR_A_UP       'A'       /* A:  history backward completion */
#endif
#ifndef EWS_CLI_CHAR_B_DOWN
#define EWS_CLI_CHAR_B_DOWN     'B'       /* B:  history forward completion */
#endif
#ifndef EWS_CLI_CHAR_C_RIGHT
#define EWS_CLI_CHAR_C_RIGHT    'C'       /* C:  go forward completion */
#endif
#ifndef EWS_CLI_CHAR_D_LEFT
#define EWS_CLI_CHAR_D_LEFT     'D'       /* D:  go backward completion */
#endif


/* Parsing error messages*/
#ifndef EWS_CLI_UNRECOGNIZED_CMD
#define EWS_CLI_UNRECOGNIZED_CMD "\r\n% Unrecognized command"
#endif

#ifndef EWS_CLI_AMBIGUOUS_CMD
#define EWS_CLI_AMBIGUOUS_CMD "Ambiguous command:"
#endif

#ifndef EWS_CLI_INVALID_INPUT_DETECTED
#define EWS_CLI_INVALID_INPUT_DETECTED "% Invalid input detected at '^' marker."
#endif

/*
#define LOGIN_BANNER "\r\n\r\nNOTE: Enter '?' for Command Help. Command help displays all options\r\n      that are valid for the 'normal' and 'no' command forms.  For\r\n      the syntax of a particular command form, please consult the\r\n      documentation.\r\n\r\n"
 */

#endif /* EW_CONFIG_OPTION_CLI */

#endif /* _EWS_CLI_H_ */
