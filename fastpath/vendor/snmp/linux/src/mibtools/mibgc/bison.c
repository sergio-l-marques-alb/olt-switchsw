/* A Bison parser, made by GNU Bison 1.875c.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     BAR = 258,
     AUGMENTS = 259,
     CCE = 260,
     APPLICATION = 261,
     ANY = 262,
     AGENT_CAPABILITIES = 263,
     accessible_for_notify = 264,
     ACCESS = 265,
     Counter64 = 266,
     Counter32 = 267,
     Counter = 268,
     CONTACT_INFO = 269,
     COMMA = 270,
     CHOICE = 271,
     BY = 272,
     BITS = 273,
     BIT = 274,
     BINARY_NUMBER = 275,
     BEGIN_ = 276,
     DISPLAY_HINT = 277,
     DESCRIPTION = 278,
     deprecated = 279,
     DEFVAL = 280,
     DEFINITIONS = 281,
     DEFINED = 282,
     currrent = 283,
     CREATION_REQUIRES = 284,
     IDENTIFIER = 285,
     HEX_NUMBER = 286,
     GROUP = 287,
     Gauge32 = 288,
     Gauge = 289,
     FROM = 290,
     EXPORTS = 291,
     EXPLICIT = 292,
     ENTERPRISE = 293,
     END = 294,
     DOT_DOT = 295,
     DOT = 296,
     LBRACE = 297,
     LAST_UPDATED = 298,
     IpAddress = 299,
     Integer32 = 300,
     INTEGER = 301,
     INDEX = 302,
     INCLUDES = 303,
     IMPORTS = 304,
     IMPLIED = 305,
     IMPLICIT = 306,
     MIN = 307,
     MAX = 308,
     MAX_ACCESS = 309,
     MANDATORY_GROUPS = 310,
     mandatory = 311,
     LPAREN = 312,
     LOWERCASE_IDENTIFIER = 313,
     LBRACKET = 314,
     NEG_NUMBER = 315,
     MODULE_IDENTITY = 316,
     MODULE_CONFORMANCE = 317,
     MODULE_COMPLIANCE = 318,
     MODULE = 319,
     MIN_ACCESS = 320,
     NULL_ = 321,
     not_implemented = 322,
     NOTIFICATION_TYPE = 323,
     NOTIFICATIONS = 324,
     NOTIFICATION_GROUP = 325,
     not_accessible = 326,
     OCTET = 327,
     obsolete = 328,
     OBJECT_TYPE = 329,
     OBJECTS = 330,
     ObjectSyntax = 331,
     OBJECT_IDENTITY = 332,
     OBJECT_GROUP = 333,
     OBJECT = 334,
     RBRACE = 335,
     QUOTED_STRING = 336,
     PRODUCT_RELEASE = 337,
     PRIVATE = 338,
     POS_NUMBER = 339,
     ORGANIZATION = 340,
     optional = 341,
     Opaque = 342,
     OID = 343,
     OF = 344,
     SEQUENCE = 345,
     SEMICOLON = 346,
     RPAREN = 347,
     REVISION = 348,
     REFERENCE = 349,
     read_write = 350,
     read_only = 351,
     read_create = 352,
     RBRACKET = 353,
     UNION = 354,
     UInteger32 = 355,
     TRAP_TYPE = 356,
     TimeTicks = 357,
     TEXTUAL_CONVENTION = 358,
     SYNTAX = 359,
     SUPPORTS = 360,
     STRUCT = 361,
     STRING = 362,
     STATUS = 363,
     SIZE = 364,
     write_only = 365,
     VARIATION = 366,
     VARIABLES = 367,
     UPPERCASE_IDENTIFIER = 368,
     Unsigned32 = 369,
     UNIVERSAL = 370,
     UNITS = 371,
     WRITE_SYNTAX = 372
   };
#endif
#define BAR 258
#define AUGMENTS 259
#define CCE 260
#define APPLICATION 261
#define ANY 262
#define AGENT_CAPABILITIES 263
#define accessible_for_notify 264
#define ACCESS 265
#define Counter64 266
#define Counter32 267
#define Counter 268
#define CONTACT_INFO 269
#define COMMA 270
#define CHOICE 271
#define BY 272
#define BITS 273
#define BIT 274
#define BINARY_NUMBER 275
#define BEGIN_ 276
#define DISPLAY_HINT 277
#define DESCRIPTION 278
#define deprecated 279
#define DEFVAL 280
#define DEFINITIONS 281
#define DEFINED 282
#define currrent 283
#define CREATION_REQUIRES 284
#define IDENTIFIER 285
#define HEX_NUMBER 286
#define GROUP 287
#define Gauge32 288
#define Gauge 289
#define FROM 290
#define EXPORTS 291
#define EXPLICIT 292
#define ENTERPRISE 293
#define END 294
#define DOT_DOT 295
#define DOT 296
#define LBRACE 297
#define LAST_UPDATED 298
#define IpAddress 299
#define Integer32 300
#define INTEGER 301
#define INDEX 302
#define INCLUDES 303
#define IMPORTS 304
#define IMPLIED 305
#define IMPLICIT 306
#define MIN 307
#define MAX 308
#define MAX_ACCESS 309
#define MANDATORY_GROUPS 310
#define mandatory 311
#define LPAREN 312
#define LOWERCASE_IDENTIFIER 313
#define LBRACKET 314
#define NEG_NUMBER 315
#define MODULE_IDENTITY 316
#define MODULE_CONFORMANCE 317
#define MODULE_COMPLIANCE 318
#define MODULE 319
#define MIN_ACCESS 320
#define NULL_ 321
#define not_implemented 322
#define NOTIFICATION_TYPE 323
#define NOTIFICATIONS 324
#define NOTIFICATION_GROUP 325
#define not_accessible 326
#define OCTET 327
#define obsolete 328
#define OBJECT_TYPE 329
#define OBJECTS 330
#define ObjectSyntax 331
#define OBJECT_IDENTITY 332
#define OBJECT_GROUP 333
#define OBJECT 334
#define RBRACE 335
#define QUOTED_STRING 336
#define PRODUCT_RELEASE 337
#define PRIVATE 338
#define POS_NUMBER 339
#define ORGANIZATION 340
#define optional 341
#define Opaque 342
#define OID 343
#define OF 344
#define SEQUENCE 345
#define SEMICOLON 346
#define RPAREN 347
#define REVISION 348
#define REFERENCE 349
#define read_write 350
#define read_only 351
#define read_create 352
#define RBRACKET 353
#define UNION 354
#define UInteger32 355
#define TRAP_TYPE 356
#define TimeTicks 357
#define TEXTUAL_CONVENTION 358
#define SYNTAX 359
#define SUPPORTS 360
#define STRUCT 361
#define STRING 362
#define STATUS 363
#define SIZE 364
#define write_only 365
#define VARIATION 366
#define VARIABLES 367
#define UPPERCASE_IDENTIFIER 368
#define Unsigned32 369
#define UNIVERSAL 370
#define UNITS 371
#define WRITE_SYNTAX 372




/* Copy the first part of user declarations.  */
#line 1 "parser.y"

/*
 *
 * Copyright (C) 1999-2006 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================== NOTES =========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* 

Productions beginning with a lowercase letter may be empty.

Productions beginning with an uppercase letter may NOT be empty.

A file may contain multiple MIB modules or no MIB at all.

_ is used in tokens instead of -, yacc/bison doesn't like -.

_ is appended to some tokens to avoid confusion with
ASN.1 and C keywords.

*/

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================== INCLUDES ========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

#include "sr_type.h"
#include "sr_copyright.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif  /* HAVE_STDIO_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#ifdef HAVE_SYS_TIME_H                                                        
#include <sys/time.h>                                                         
#endif /* HAVE_SYS_TIME_H */                                                   

#ifdef HAVE_TIME_H                                                            
#include <time.h>                                                             
#endif /* HAVE_TIME_H */                                                      

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif /* HAVE_CTYPE_H */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================== DEFINES =========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

#define NUM_MIB_REPLACEMENTS    44

#define SAVE_FOR_ALL_PARSES     0
#define SINGLE_PARSE            1

#define NORMAL_PARSE            0 
#define IMPORT_PARSE            1 

/* standard child locations */
#define child_identity_0        0
#define child_syntax___1        1
#define child_status___2        2 
#define child_descript_3        3
#define child_referenc_4        4
#define child_object___5        5
#define child_range____6        6
#define child_index____7        7
#define child_display__8        8
#define child_value____9        9
#define child_default_10        10
#define child_access__11        11
#define chld_strt_rnge12        12
#define chld_end_range13        13

#define child_misc1___14        14
#define child_misc2___15        15

#define MAX_CHILDREN            16
#define MYBUFSIZE               8192

/* table size = use a prime number for the table size to minimize
 * the number of collisions, and thus maximize speed.
 */
#define TABLE_SIZE              499

#define YYERROR_VERBOSE 1 /* allow verbose bison error msgs */
/* #define YYDEBUG 1 */    /* only define during debug phase */

#define MAX_OID_SIZE		128
#define HASHNEXT(x) x

#ifdef SUNOS
#define strtoul(x, y, z) ((unsigned long) strtol(x, y, z))
#endif /* SUNOS */

#define MAX_RANGE_NUM		20

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ===================== STORAGE STRUCTS  ===================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* node in tree or symbol table */
typedef struct tree_tag {

    unsigned long number;       /* type tells neg from pos          */

    int type;                   /* use token values e.g. currrent   */

    char * string;              /* real data storage                */
    int freeMe;

    /* PARENT */
    struct tree_tag * parent;

    /* TREE CHILDREN */
    struct tree_tag * child[MAX_CHILDREN];
 
    /* TREE SIBLING */
    struct tree_tag * next_tree_node;
    struct tree_tag * last_tree_node_in_list;

    /* SYMBOL TABLE SIBLING */
    struct tree_tag * next_symbol_node;

    /* table of enumerated items for object */
    struct tree_tag * * enumeratedItems;

    /* free lists */
    struct tree_tag * freePtr;

} tree;

typedef tree * symbol_table; 

typedef struct _RangeInfo {
     int lower;
     int upper;
} RangeInfo;

typedef struct _IndexInfo {
     char *name;
     short int implied;
     struct _IndexInfo *next;
} IndexInfo; 

typedef struct _EnumInfo {
     char *name;
     char *value;
     struct _EnumInfo *next;
} EnumInfo;

typedef struct _RevisionInfo {
     char *date;
     char *description;
     struct _RevisionInfo *next;
} RevisionInfo;

typedef struct _ObjListInfo {
     char *name;
     char *value;
     struct _ObjListInfo *next;
} ObjListInfo; 

typedef struct _IntOIDInfo {
     int oidlen;
     int *oid;
} IntOIDInfo; 

typedef struct _CondGroup {
     char *name;
     char *descr;
     struct _CondGroup *next;
} CondGroup;

typedef struct _CondException {
     char *name;
     char *min_access;
     char *syntax;
     char *primtype;
     char *write_syntax;
     char *primtype_write;
     char *descr;
     RangeInfo *range[MAX_RANGE_NUM];
     RangeInfo *range_write[MAX_RANGE_NUM];
     EnumInfo *enum_list;
     EnumInfo *enum_list_write;
     struct _CondException *next;
} CondException;

typedef struct _ModuleComp {
     char *name;
     char *mandatory_groups;
     CondGroup *cond_group;
     CondException *cond_exception;
     struct _ModuleComp *next;
} ModuleComp;

typedef struct _ObjData {
     char *name;            /* child_identity  */
     char *type;            /* child_syntax    */
     char *primtype;        /* child_syntax    */
     char *oid;             /* child_object in dotted number form */ 
     char *id;		    /* child_object in parent name dot form */
     char *access;          /* child_access    */
     char *descr;           /* child_descr     */
     char *reference;       /* child_referece  */
     char *status;          /* child_status    */
     char *moduleid;        /* module identity */
     char *component_type;   /* oid, table, scalar, etc.*/
     char *misc;	    /* miscellaneous stuff*/
     char *defval;	    /* child_deval     */
     int augments;
     int doneTag;
     IntOIDInfo *intoid;
     RangeInfo *range[MAX_RANGE_NUM];
     IndexInfo *index_list;
     EnumInfo *enum_list;
     RevisionInfo *revision_list;
     ObjListInfo *obj_list;
     ModuleComp *module_comp;    /* module compliance info */
     struct _ObjData *next_obj;
     struct _ObjData *prev_obj;
     struct _ObjData *next_sorted;
} ObjData;

typedef struct _TC_ObjData {
     char *name;            /* child_identity  */
     char *type;            /* child_syntax    */
     char *status;          /* child_status    */
     char *descr;           /* child_descr     */
     char *reference;       /* child_referece  */
     char *component_type;   /* oid, table, scalar, etc.*/
     char *misc;	    /* miscellaneous stuff*/
     int tc_ta;	    /* 0=Textual Convention, 1=Type Assignment */
     RangeInfo *range[MAX_RANGE_NUM];
     EnumInfo *enum_list;
     struct _TC_ObjData *next_obj;
     struct _TC_ObjData *prev_obj;
} TC_ObjData;

typedef struct _IM_ObjData {
     char *name;	  /* import module */
     char *type;	  /* import FROM   */
     char *component_type; 
     struct _IM_ObjData *next_obj;
     struct _IM_ObjData *prev_obj;
} IM_ObjData;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================= GLOBALS   ======================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* use ; as path separator on windows, : elsewhere */
#ifndef SR_WINDOWS
char * SEPARATOR = ":";
#else
char * SEPARATOR = ";";
#endif /* SR_WINDOWS */

char *xml_mib;

/* ************ */
/* GLOBAL FLAGS */
/* ************ */

int globalSuppressOutputFlag        = 0; /* suppress warnings               */
int globalVerboseOutputFlag         = 0; /* add more verbose output	    */
int globalIgnoreStrictComplianceFlag= 0; /* allows more liberal compliance  */
int globalFatalErrorFlag            = 0; /* when fatal errors occur         */
int globalExitErrorFlag             = 0; /* exit with -1 status        	    */
int postParseErrorFlag	 	    = 0; /* when post parse errors occur    */
int globalParseOnlyFlag             = 0; /* only parses - no output         */
int globalCppCommentFlag            = 0; /* use C++ style comments          */
int globalProduceDescrFlag          = 0; /* produce description output file */
int globalMultipleOutputFilesFlag   = 1; /* more than one output file       */
int globalRenameFlag                = 0; /* rename input file if filename doesn't match module name */
int globalBackupOutputFlag	    = 0; /* by default should not rename the output - just stomp over */
int helpFlag                        = 0; /* print help info */
int formatFlag                      = 0; /* print help info */
int updateImportsFlag               = 0; /* replace MIB import with replacement MIB */

int openOutputFlag = 1;                  /* open the output file */
int explicitOutputFile = 0;

int importParseFlag                 = 0; /* import parse or normal parse    */

/* save type nodes for all parses */
/* used for AUX-MIB.dat           */
int saveForAllParsesFlag            = 1;

/* OUTPUT FORMATS */

#define IDAT      1
#define XML       2
#define MIBGUIDE  3

/* GLOBAL LISTS */
tree * globalInputFilenameList   = NULL;
tree * importFromList            = NULL;
tree * globalImportDirectoryList = NULL;

ObjData *llroot;
ObjData *sorted_root = NULL;
TC_ObjData *tc_root = NULL;
IM_ObjData *im_root = NULL;
int multiple_mibs = 0;
int first_parse = 1;
int last_parse = 0;

/* ********** */
/* FILE ITEMS */
/* ********** */

extern FILE * yyin;                      /* lexer */

FILE * outputFilePtr        = NULL; 
FILE * inputFilePtr         = NULL; 
FILE * descrFilePtr         = NULL;

char * lastFileParsed       = NULL;
char * lastModuleName       = NULL;
char * outputModuleName     = NULL;
char * descrFileName        = NULL;
char * outputFileName       = NULL;
char * outputDirectoryName  = NULL;
char * moduleNameBuffer     = NULL;

/* ************** */
/* SEMANTIC ITEMS */
/* ************** */

/* once one of the SMIv1 or SMIv2 flags is set by a unique identifier
   that becomes the standard and any attempt to reset is an error */
int SMIv1Flag                = 0;
int SMIv2Flag                = 0;
int SMIv3Flag                = 0;  /* SMIv3 is an augmentation of SMIv2             */




/* **** NOT USED YET **** - future semantic check */
int sawObjectIdentifier      = 0;  /* saw at least one object identifier in MIB     */




/* ************** */
/* OUTPUT FORMAT  */
/* ************** */

#define MAX_COLUMN_WIDTH 15
int maxColumnWidth           = MAX_COLUMN_WIDTH; /* max column width for printing to def file */

/* ************** */
/* WORK BUFFERS   */
/* ************** */

char * lineBuf               = NULL;
char * string                = NULL;
char * workBuf               = NULL;
char * stringBuf             = NULL;
char * fileNameBuffer        = NULL;
char * firstLineBuf          = NULL;

/* ************** */
/* *** LISTS **** */
/* ************** */

tree * singleParseNodeList          = NULL; /* free at end of each parse */
tree * allParsesNodeList            = NULL; /* free at end of all parses */

/* ************** */
/* *** TABLES *** */
/* ************** */

/* for each parse */
symbol_table * virtualTables        = NULL; /* listing of virtual tables, for this parse only */
symbol_table * emittedNodes         = NULL; /* types emitted, for this parse only */
symbol_table * singleParseTypeTable = NULL; /* local and imported type nodes, for this parses */ 
symbol_table * importedMIBs         = NULL; /* table of MIBs that have been imported for this parse */

/* for all parses */
symbol_table * allParsesTypeTable   = NULL; /* types from AUX-MIB.dat for all parses */ 

symbol_table * mibReplacementTable  = NULL; /* MIB supercession table */ 

/* ************** */
/* * PARSE ITEMS **/
/* ************** */

/* normal error tracking */
int line_cntr                = 1;
int col_cntr                 = 0;
int no_code_flag             = 0;

/* unterminated string error tracking */
int stringBufSize            = 0;
int stringPos                = 0;  /* for copying strings */
int string_line_cntr         = 1;
int string_col_cntr          = 0;
int stringWarnOn             = 0;

/* parse tree */
tree * root = NULL;                /* pointer to root of AST */

time_t myclock;
struct tm * currentTime;

/* old mib first, then update or replacment mib */
/* updates before replacements, multiple links on old mib */
char * mibReplacements[NUM_MIB_REPLACEMENTS][2] = {
{"RFC1065-SMI",     "RFC1155-SMI"},
{"RFC1066-MIB",     "RFC1156-MIB"}, 
{"RFC1067-SNMP",    "RFC1157-SNMP"},
{"RFC1098-SNMP",    "RFC1157-SNMP"},

{"RFC1158-MIB",     "IP-MIB"},
{"RFC1158-MIB",     "TCP-MIB"},
{"RFC1158-MIB",     "UDP-MIB"},
{"RFC1158-MIB",     "RFC1213-MIB"},

{"RFC1229-MIB",     "IF-MIB"},
{"RFC1232-MIB",     "IF-MIB"},
{"RFC1233-MIB",     "DLSW-MIB"},
{"RFC1243-MIB",     "APPLETALK-MIB"}, 
{"RFC1271-MIB",     "RMON-MIB"},
{"RFC1284-MIB",     "EtherLike-MIB"},
{"DSA-MIB",         "DIRECTORY-SERVER-MIB"},
{"RFC1289-phivMIB", "DECNET-PHIV-MIB"},
{"RFC1304-MIB",     "SIP-MIB"},
{"RFC1315-MIB",     "FRAME-RELAY-DTE-MIB"},
{"RFC1316-MIB",     "CHARACTER-MIB"},
{"RFC1317-MIB",     "RS-232-MIB"},
{"RFC1318-MIB",     "PARALLEL-MIB"},
{"RFC1354-MIB",     "IP-FORWARD-MIB"},
{"RFC1389-MIB",     "RIPv2-MIB"},
{"RFC1398-MIB",     "EtherLike-MIB"},
{"RFC1406-MIB",     "DS1-MIB"},
{"RFC1407-MIB",     "DS3-MIB"},

{"RFC1248-MIB",     "OSPF-MIB"},
{"RFC1248-MIB",     "OSPF-TRAP-MIB"},

{"RFC1252-MIB",     "OSPF-MIB"},
{"RFC1252-MIB",     "OSPF-TRAP-MIB"},

{"RFC1253-MIB",     "OSPF-MIB"},
{"RFC1253-MIB",     "OSPF-TRAP-MIB"},

{"RFC1286-MIB",     "BRIDGE-MIB"},
{"RFC1286-MIB",     "SOURCE-ROUTING-MIB"},

{"RFC1213-MIB",     "IP-MIB"},
{"RFC1213-MIB",     "TCP-MIB"},
{"RFC1213-MIB",     "UDP-MIB"},
{"RFC1213-MIB",     "RFC1213-MIB"}, 

{"RFC1285-MIB",     "FDDI-SMI73"},
{"RFC1285-MIB",     "RFC1285-MIB"},

{"RFC1231-MIB",     "TOKENRING-STATION-SR-MIB)"},
{"RFC1231-MIB",     "TOKENRING-MIB"},

{"TOKENRING-MIB",   "TOKENRING-STATION-SR-MIB"},
{"TOKENRING-MIB",   "TOKENRING-MIB"}
};

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================== FUNCTION PROTOTYPES ===================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

int yylex(); /* just to get rid of warnings */

void check_for_range_overlap(tree * rangelist);
void check_for_range_restriction_on_type(tree * myPtr, FILE * myOutputFilePtr);
void check_for_type_definition(tree * myPtr, FILE * myOutputFilePtr, FILE * myDescrFilePtr);
void check_range_limits(tree * rangelist, int flag);
void convert_int_to_string(int n, char s[]);
void convert_ul_to_string(unsigned long  n, char s[]);
void do_imports(tree * local_importFromList, char * fileDoingImport);
void dump_table(symbol_table * table);
void dump_table1(tree * table[]);
void fill_in_max_min_values(tree * rangelist, int flag);
void free_all_nodes(tree * my_ParseNodeList);
void insert_mib_replacement_node(char * oldName, char * newName);
void insert_name(tree * node, tree * symboltable[]);
void insert_name_into_emitted_nodes(char * name, int type);
void insert_type_node(tree * node, tree * symboltable[] );
void load_mib_replacement_table();
void mosy_code_emit(tree * ptr, FILE * outPtr, FILE * descrFilePtr);
void mosy_emit_all_nodes(tree * ptr, FILE * outPtr, FILE * descrFilePtr);
void mosy_emit_node(tree * ptr, FILE * outPtr, FILE * descrFilePtr);
void mosy_print_enumerated_range(tree * ptr, tree * tmpPtr, FILE * outPtr);
void open_output_file(char * desiredOutputName);
void print_string_warning();
void print_warning(char * my_msg, char * buf);
void print_note(char * my_msg, char * buf);
void write_header(FILE * outPtr);
void yyerror (char * errmsg);

/* new linked list functions */
void ll_emit_all_nodes(tree * ptr, FILE * outPtr);
void ll_emit_node(tree * ptr, FILE * outPtr);
void ll_add_node(ObjData *llnode); 
void ll_emit_modcomp(ModuleComp * modPtr, tree * ptr); 
void print_ll(FILE * outPtr);

/* new xml related functions */
int sort_data();
ObjData *insert_sort_data(ObjData * exist_ptr, ObjData * add_ptr);
int cmp_oid(char * oid1, char * oid2, int length1, int length2);
void print_ll_xml(FILE * outPtr);
void print_modcomp_xml(int indent_num, FILE * outPtr, ObjData * tmpptr);
void print_modrange(CondException *condException, FILE * outFilePtr, int num);
void pushtag(char * name);
char *poptag(void);
void indent(FILE * outPtr, int indent);
int oidlength(char * oid);
void ll_add_tcnode(TC_ObjData *tcnode);
void ll_add_imnode(IM_ObjData *imnode);
char *xml_correct_string(char * xml_string, int len);


char *build_oid(tree * ptr);
char *build_id(tree * ptr);
IntOIDInfo *build_int_oid(char * ptr);
char *correctoid(char *oid);
EnumInfo *build_enumeration_info(tree * ptr);
EnumInfo * build_enumeration_modcomp_info(tree *ptr);
IndexInfo *build_index_info(tree * ptr);
RangeInfo *add_range_info(tree * ptr);
ObjListInfo *build_objlist_info(tree * ptr);
void print_ll_enum(EnumInfo *list, FILE * outPtr);
void print_ll_index(IndexInfo *list, FILE * outPtr);
void print_ll_objlist(ObjListInfo *list, FILE * outPtr);
void print_ll_range(ObjData *tmpptr, FILE * outPtr);
void cleanup_ll();
void cleanup_index(IndexInfo *list);
void cleanup_enum(EnumInfo *list);
void cleanup_objlist(ObjListInfo *list);
void cleanup_revision_list(RevisionInfo *rev);
void cleanup_module_comp(ModuleComp *module_comp);
void cleanup_cond_group(CondGroup *cond_group);
void cleanup_cond_exception(CondException *cond_exception);
char *find_prim_type(char *ptr);
int  application_type_test(char *type);
void walk_tree(tree *ptr);

/*BAB*/
int garbageCheck(char *name);
int bin_pow(int exponent);

/* parse file returns 0 on success, 1 on failure */
int hashpjw(char *, int);
int parse_file(int importFlag, char * localInputFileName, char * partialName);
int print_value_list(tree * firstNodeInList, FILE * outPtr);

symbol_table * malloc_table(symbol_table * new_table);

tree * clone_node(tree * origNode);
tree * find_type(char * lookupName);
tree * lookup_name_in_emitted_nodes(char * name, int type);
tree * lookup_type(char * string, tree * symboltable[] );
tree * malloc_node(tree * node, int addToFreeList);
tree * lookup_name(char * variable_name, tree * symboltable[]);
tree * shallow_clone_node(tree * origNode);

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ====================== PARSER ============================== */
/*  %right favors shifting 
    %left favors reduction 
    precedence is from lowest to highest ( with lowest first ) 
 */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 622 "parser.y"
typedef union YYSTYPE {
    int val;
    char * char_ptr;
    struct tree_tag * tree_ptr;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 937 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 949 "y.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  34
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3298

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  118
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  155
/* YYNRULES -- Number of rules. */
#define YYNRULES  422
/* YYNRULES -- Number of states. */
#define YYNSTATES  735

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   372

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    12,    18,    22,    26,
      28,    30,    34,    36,    38,    40,    42,    45,    49,    52,
      56,    58,    60,    63,    65,    67,    70,    72,    75,    77,
      79,    83,    86,    90,    94,    96,    98,   100,   102,   104,
     106,   108,   110,   112,   114,   116,   118,   120,   122,   124,
     126,   128,   130,   132,   134,   138,   142,   146,   148,   150,
     152,   154,   156,   158,   160,   162,   164,   166,   168,   170,
     172,   174,   176,   182,   188,   190,   192,   196,   198,   201,
     203,   205,   207,   209,   211,   213,   215,   217,   220,   223,
     226,   229,   231,   233,   238,   240,   243,   246,   249,   255,
     257,   259,   263,   265,   267,   269,   271,   273,   275,   277,
     279,   281,   283,   286,   288,   291,   296,   298,   301,   306,
     308,   311,   313,   315,   320,   325,   327,   329,   334,   337,
     340,   347,   352,   357,   359,   362,   365,   367,   369,   371,
     373,   375,   377,   380,   384,   388,   392,   396,   402,   407,
     409,   413,   416,   419,   423,   426,   429,   432,   436,   439,
     442,   444,   447,   449,   451,   453,   458,   460,   463,   466,
     471,   473,   476,   479,   484,   486,   488,   491,   494,   499,
     501,   503,   506,   509,   512,   517,   522,   527,   529,   531,
     533,   535,   537,   547,   557,   562,   564,   575,   584,   593,
     595,   598,   603,   608,   610,   621,   633,   645,   654,   659,
     661,   674,   687,   690,   693,   695,   697,   699,   701,   703,
     705,   708,   710,   715,   721,   726,   728,   730,   734,   737,
     740,   742,   753,   756,   759,   771,   773,   776,   778,   780,
     782,   784,   786,   795,   802,   806,   809,   811,   823,   826,
     829,   831,   833,   835,   838,   842,   850,   852,   856,   858,
     860,   862,   863,   867,   871,   873,   875,   878,   880,   882,
     884,   887,   889,   891,   895,   902,   905,   907,   909,   911,
     913,   915,   917,   922,   924,   926,   928,   937,   940,   942,
     944,   946,   948,   950,   952,   954,   956,   958,   960,   963,
     965,   968,   970,   972,   974,   976,   978,   980,   983,   991,
     993,   995,   997,  1000,  1002,  1005,  1007,  1011,  1014,  1016,
    1018,  1021,  1026,  1028,  1033,  1038,  1043,  1045,  1048,  1050,
    1054,  1057,  1059,  1061,  1063,  1065,  1067,  1069,  1072,  1074,
    1076,  1078,  1080,  1082,  1085,  1087,  1091,  1096,  1098,  1101,
    1103,  1107,  1109,  1111,  1113,  1115,  1117,  1119,  1121,  1123,
    1125,  1127,  1129,  1131,  1133,  1135,  1137,  1139,  1141,  1143,
    1145,  1147,  1149,  1151,  1153,  1155,  1157,  1159,  1161,  1163,
    1165,  1167,  1169,  1171,  1173,  1175,  1177,  1179,  1182,  1184,
    1186,  1188,  1190,  1192,  1194,  1196,  1198,  1200,  1202,  1204,
    1206,  1209,  1212,  1214,  1216,  1219,  1223,  1227,  1232,  1238,
    1240,  1242,  1246,  1249,  1251,  1256,  1258,  1260,  1262,  1264,
    1266,  1268,  1270
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     119,     0,    -1,   120,    -1,   189,    -1,   121,    -1,   120,
     121,    -1,   122,     5,    21,   127,    39,    -1,   233,   123,
      26,    -1,   232,   123,    26,    -1,   139,    -1,   189,    -1,
      49,   125,    91,    -1,   189,    -1,   126,    -1,   189,    -1,
     133,    -1,   126,   133,    -1,   128,   124,   130,    -1,   128,
     124,    -1,    36,   129,    91,    -1,   189,    -1,   132,    -1,
     132,    15,    -1,   189,    -1,   137,    -1,   130,   137,    -1,
     132,    -1,   132,    15,    -1,   189,    -1,   134,    -1,   132,
      15,   134,    -1,   132,   134,    -1,   131,    35,   233,    -1,
     131,    35,   232,    -1,   233,    -1,   232,    -1,   135,    -1,
     145,    -1,   255,    -1,   256,    -1,   261,    -1,   243,    -1,
       8,    -1,    70,    -1,    68,    -1,   101,    -1,    55,    -1,
      63,    -1,    62,    -1,    61,    -1,    78,    -1,    77,    -1,
      74,    -1,   103,    -1,   233,     5,   143,    -1,   232,     5,
     143,    -1,   135,     5,   143,    -1,   159,    -1,   155,    -1,
     154,    -1,   181,    -1,   160,    -1,   162,    -1,   161,    -1,
     186,    -1,   164,    -1,   178,    -1,   152,    -1,   136,    -1,
     138,    -1,   171,    -1,   174,    -1,   232,    79,    30,     5,
     139,    -1,   233,    79,    30,     5,   139,    -1,   232,    -1,
     233,    -1,    42,   140,    80,    -1,   151,    -1,   140,   151,
      -1,   232,    -1,   233,    -1,   145,    -1,   261,    -1,   250,
      -1,   263,    -1,   143,    -1,   145,    -1,   251,   227,    -1,
     254,   227,    -1,   257,   227,    -1,   259,   228,    -1,   146,
      -1,   190,    -1,   244,    42,   148,    80,    -1,   177,    -1,
     258,   227,    -1,   260,   227,    -1,   253,   227,    -1,   244,
      42,   148,    15,    80,    -1,   233,    -1,   232,    -1,   233,
      41,   233,    -1,   251,    -1,   254,    -1,   257,    -1,   258,
      -1,   265,    -1,   252,    -1,   253,    -1,   259,    -1,   260,
      -1,   263,    -1,   263,   228,    -1,   261,    -1,   261,   227,
      -1,   261,    42,   220,    80,    -1,   250,    -1,   250,   228,
      -1,   250,    42,   220,    80,    -1,   255,    -1,   255,   227,
      -1,   256,    -1,   262,    -1,   262,    42,   220,    80,    -1,
     243,    42,   220,    80,    -1,   233,    -1,   232,    -1,   233,
      42,   220,    80,    -1,   233,   227,    -1,   233,   228,    -1,
     233,    41,   233,    42,   220,    80,    -1,   233,    41,   233,
     227,    -1,   233,    41,   233,   228,    -1,    66,    -1,   261,
     228,    -1,   250,   227,    -1,   266,    -1,   235,    -1,   232,
      -1,   233,    -1,   231,    -1,   234,    -1,    42,    80,    -1,
      42,   235,    80,    -1,    42,   219,    80,    -1,    42,   232,
      80,    -1,    42,   234,    80,    -1,    42,   232,    15,   214,
      80,    -1,    42,   151,   140,    80,    -1,   149,    -1,   148,
      15,   149,    -1,   148,   149,    -1,   232,   150,    -1,   232,
     269,   150,    -1,   232,   190,    -1,   232,   177,    -1,   233,
     150,    -1,   233,   269,   150,    -1,   233,   190,    -1,   233,
     177,    -1,   263,    -1,   263,   228,    -1,   255,    -1,   256,
      -1,   262,    -1,   262,    42,   220,    80,    -1,   261,    -1,
     261,   227,    -1,   261,   228,    -1,   261,    42,   220,    80,
      -1,   250,    -1,   250,   228,    -1,   250,   227,    -1,   250,
      42,   220,    80,    -1,   233,    -1,   232,    -1,   233,   227,
      -1,   233,   228,    -1,   233,    42,   220,    80,    -1,   243,
      -1,   145,    -1,   251,   227,    -1,   254,   227,    -1,   257,
     227,    -1,   251,    42,   220,    80,    -1,   254,    42,   220,
      80,    -1,   257,    42,   220,    80,    -1,   272,    -1,   235,
      -1,   232,    -1,   233,    -1,   219,    -1,   232,   101,    38,
     139,   153,   191,   226,     5,   235,    -1,   233,   101,    38,
     139,   153,   191,   226,     5,   235,    -1,   112,    42,   214,
      80,    -1,   189,    -1,   232,    62,    43,   223,    82,   231,
     192,   209,     5,   139,    -1,   232,    63,   224,   192,   226,
     156,     5,   139,    -1,   233,    63,   224,   192,   226,   156,
       5,   139,    -1,   157,    -1,   156,   157,    -1,    64,   207,
     158,   193,    -1,    55,    42,   214,    80,    -1,   189,    -1,
     232,     8,    82,   231,   224,   192,   226,   209,     5,   139,
      -1,   232,    70,    69,    42,   214,    80,   224,   192,   226,
       5,   139,    -1,   232,    78,    75,    42,   214,    80,   224,
     192,   226,     5,   139,    -1,   232,    68,   163,   224,   192,
     226,     5,   139,    -1,    75,    42,   214,    80,    -1,   189,
      -1,   232,    74,   217,   167,   165,   224,   191,   226,   168,
     218,     5,   139,    -1,   233,    74,   217,   167,   165,   224,
     191,   226,   168,   218,     5,   139,    -1,    54,   222,    -1,
      10,   166,    -1,   239,    -1,   236,    -1,   237,    -1,   238,
      -1,   241,    -1,   242,    -1,   116,   231,    -1,   189,    -1,
      47,    42,   169,    80,    -1,    47,    42,   169,    15,    80,
      -1,     4,    42,   232,    80,    -1,   189,    -1,   170,    -1,
     169,    15,   170,    -1,   169,   170,    -1,    50,   141,    -1,
     141,    -1,   172,    54,   222,   224,   191,    42,   175,    80,
       5,   139,    -1,   232,   106,    -1,   232,    99,    -1,   173,
      54,   222,   224,   191,    42,   176,   175,    80,     5,   139,
      -1,   176,    -1,   175,   176,    -1,   164,    -1,   171,    -1,
     174,    -1,   106,    -1,    99,    -1,   233,     5,   103,   180,
     224,   192,   226,   217,    -1,   179,   180,   224,   192,   226,
     217,    -1,   135,     5,   103,    -1,    22,   231,    -1,   189,
      -1,   182,    43,   223,    85,   231,    14,   231,   192,   183,
       5,   139,    -1,   232,    61,    -1,   233,    61,    -1,   184,
      -1,   189,    -1,   185,    -1,   184,   185,    -1,    93,   223,
     192,    -1,   232,    77,   224,   192,   226,     5,   139,    -1,
     188,    -1,   188,    40,   188,    -1,   235,    -1,   232,    -1,
     233,    -1,    -1,    90,    89,   233,    -1,    90,    89,   232,
      -1,   192,    -1,   189,    -1,    23,   231,    -1,   194,    -1,
     189,    -1,   195,    -1,   194,   195,    -1,   196,    -1,   197,
      -1,    32,   232,   192,    -1,    79,   232,   205,   206,   198,
     192,    -1,    65,   199,    -1,   189,    -1,   239,    -1,   241,
      -1,   236,    -1,   237,    -1,   242,    -1,    29,    42,   201,
      80,    -1,   189,    -1,   214,    -1,   189,    -1,   111,   232,
     205,   206,   203,   200,   218,   192,    -1,    10,   204,    -1,
     189,    -1,   240,    -1,   241,    -1,   236,    -1,   237,    -1,
     242,    -1,   238,    -1,   239,    -1,   217,    -1,   189,    -1,
     117,   142,    -1,   189,    -1,   233,   208,    -1,   189,    -1,
     139,    -1,   189,    -1,   210,    -1,   189,    -1,   211,    -1,
     210,   211,    -1,   105,   207,    48,    42,   214,    80,   212,
      -1,   213,    -1,   189,    -1,   202,    -1,   213,   202,    -1,
     215,    -1,   215,    15,    -1,   216,    -1,   215,    15,   216,
      -1,   215,   216,    -1,   232,    -1,   233,    -1,   104,   142,
      -1,    25,    42,   147,    80,    -1,   189,    -1,   232,    57,
     235,    92,    -1,   233,    57,   235,    92,    -1,   235,    57,
     235,    92,    -1,   221,    -1,   221,    15,    -1,   219,    -1,
     221,    15,   219,    -1,   221,   219,    -1,   239,    -1,   241,
      -1,   236,    -1,   237,    -1,   242,    -1,   231,    -1,   108,
     225,    -1,   245,    -1,   246,    -1,   247,    -1,   248,    -1,
     249,    -1,    94,   231,    -1,   189,    -1,    57,   229,    92,
      -1,    57,   264,   227,    92,    -1,   230,    -1,   230,     3,
      -1,   187,    -1,   230,     3,   187,    -1,    81,    -1,    58,
      -1,   249,    -1,   236,    -1,   237,    -1,   238,    -1,   239,
      -1,   240,    -1,   241,    -1,   242,    -1,   246,    -1,   245,
      -1,   248,    -1,   247,    -1,   113,    -1,    88,    -1,    84,
      -1,    53,    -1,    52,    -1,    60,    -1,    20,    -1,    31,
      -1,    96,    -1,    95,    -1,   110,    -1,    71,    -1,    67,
      -1,     9,    -1,    97,    -1,    18,    -1,    90,    -1,    28,
      -1,    24,    -1,    73,    -1,    56,    -1,    86,    -1,    72,
     107,    -1,    45,    -1,    44,    -1,    12,    -1,    33,    -1,
      34,    -1,    13,    -1,   114,    -1,   100,    -1,   102,    -1,
      87,    -1,    11,    -1,    46,    -1,    19,   107,    -1,    79,
      30,    -1,   109,    -1,    76,    -1,   269,   143,    -1,   269,
      51,   143,    -1,   269,    37,   143,    -1,    16,    42,   267,
      80,    -1,    16,    42,   267,    15,    80,    -1,   272,    -1,
     268,    -1,   267,    15,   268,    -1,   232,   143,    -1,   143,
      -1,    59,   270,   271,    98,    -1,   115,    -1,     6,    -1,
      83,    -1,   189,    -1,   235,    -1,   144,    -1,     7,    -1,
       7,    27,    17,   232,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   702,   702,   703,   710,   713,   720,   734,   747,   763,
     764,   768,   774,   778,   784,   788,   791,   799,   806,   816,
     819,   823,   824,   827,   831,   834,   841,   842,   846,   850,
     853,   857,   865,   936,   993,   994,   995,  1000,  1004,  1007,
    1010,  1013,  1016,  1022,  1028,  1034,  1040,  1046,  1052,  1058,
    1064,  1070,  1076,  1082,  1091,  1120,  1151,  1162,  1163,  1164,
    1165,  1166,  1167,  1168,  1169,  1170,  1171,  1172,  1173,  1174,
    1177,  1178,  1188,  1210,  1237,  1238,  1241,  1247,  1250,  1260,
    1261,  1262,  1263,  1264,  1265,  1270,  1274,  1276,  1282,  1288,
    1294,  1295,  1296,  1297,  1301,  1305,  1308,  1311,  1314,  1321,
    1322,  1323,  1327,  1328,  1329,  1330,  1331,  1332,  1333,  1334,
    1335,  1339,  1340,  1344,  1345,  1351,  1358,  1361,  1364,  1370,
    1371,  1378,  1381,  1382,  1388,  1394,  1395,  1398,  1405,  1406,
    1407,  1419,  1425,  1432,  1440,  1445,  1449,  1454,  1455,  1456,
    1457,  1458,  1459,  1466,  1467,  1468,  1469,  1471,  1477,  1485,
    1488,  1492,  1500,  1502,  1504,  1510,  1515,  1518,  1522,  1530,
    1539,  1540,  1544,  1545,  1547,  1548,  1554,  1555,  1561,  1566,
    1573,  1574,  1577,  1581,  1587,  1588,  1591,  1594,  1598,  1605,
    1607,  1608,  1614,  1620,  1627,  1633,  1639,  1647,  1652,  1653,
    1654,  1657,  1665,  1699,  1738,  1741,  1749,  1777,  1808,  1844,
    1847,  1854,  1869,  1872,  1880,  1910,  1950,  1990,  2026,  2029,
    2037,  2117,  2180,  2194,  2211,  2212,  2213,  2214,  2216,  2219,
    2225,  2238,  2243,  2252,  2264,  2278,  2282,  2285,  2289,  2297,
    2312,  2321,  2363,  2372,  2382,  2426,  2429,  2436,  2437,  2438,
    2442,  2447,  2459,  2487,  2522,  2528,  2531,  2539,  2588,  2601,
    2619,  2620,  2624,  2627,  2634,  2653,  2688,  2695,  2706,  2707,
    2708,  2712,  2720,  2734,  2754,  2755,  2770,  2777,  2778,  2782,
    2785,  2792,  2793,  2797,  2809,  2831,  2832,  2836,  2837,  2838,
    2839,  2840,  2844,  2847,  2852,  2853,  2857,  2891,  2892,  2896,
    2897,  2898,  2899,  2900,  2901,  2902,  2907,  2908,  2913,  2914,
    2918,  2927,  2931,  2932,  2936,  2937,  2941,  2944,  2951,  2964,
    2965,  2969,  2972,  2979,  2980,  2986,  2989,  2993,  3001,  3002,
    3008,  3021,  3030,  3035,  3038,  3042,  3049,  3050,  3056,  3070,
    3078,  3089,  3090,  3091,  3092,  3093,  3118,  3122,  3128,  3129,
    3130,  3131,  3132,  3136,  3139,  3144,  3150,  3156,  3157,  3163,
    3166,  3208,  3243,  3256,  3259,  3262,  3265,  3268,  3271,  3274,
    3277,  3280,  3283,  3286,  3289,  3295,  3310,  3326,  3342,  3348,
    3354,  3370,  3386,  3405,  3414,  3423,  3440,  3449,  3466,  3483,
    3500,  3526,  3534,  3553,  3562,  3571,  3590,  3609,  3619,  3644,
    3660,  3688,  3708,  3729,  3749,  3765,  3795,  3817,  3826,  3847,
    3857,  3869,  3879,  3890,  3907,  3908,  3911,  3912,  3913,  3914,
    3918,  3919,  3923,  3924,  3928,  3932,  3937,  3942,  3947,  3951,
    3952,  3956,  3961
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "BAR", "AUGMENTS", "CCE", "APPLICATION",
  "ANY", "AGENT_CAPABILITIES", "accessible_for_notify", "ACCESS",
  "Counter64", "Counter32", "Counter", "CONTACT_INFO", "COMMA", "CHOICE",
  "BY", "BITS", "BIT", "BINARY_NUMBER", "BEGIN_", "DISPLAY_HINT",
  "DESCRIPTION", "deprecated", "DEFVAL", "DEFINITIONS", "DEFINED",
  "currrent", "CREATION_REQUIRES", "IDENTIFIER", "HEX_NUMBER", "GROUP",
  "Gauge32", "Gauge", "FROM", "EXPORTS", "EXPLICIT", "ENTERPRISE", "END",
  "DOT_DOT", "DOT", "LBRACE", "LAST_UPDATED", "IpAddress", "Integer32",
  "INTEGER", "INDEX", "INCLUDES", "IMPORTS", "IMPLIED", "IMPLICIT", "MIN",
  "MAX", "MAX_ACCESS", "MANDATORY_GROUPS", "mandatory", "LPAREN",
  "LOWERCASE_IDENTIFIER", "LBRACKET", "NEG_NUMBER", "MODULE_IDENTITY",
  "MODULE_CONFORMANCE", "MODULE_COMPLIANCE", "MODULE", "MIN_ACCESS",
  "NULL_", "not_implemented", "NOTIFICATION_TYPE", "NOTIFICATIONS",
  "NOTIFICATION_GROUP", "not_accessible", "OCTET", "obsolete",
  "OBJECT_TYPE", "OBJECTS", "ObjectSyntax", "OBJECT_IDENTITY",
  "OBJECT_GROUP", "OBJECT", "RBRACE", "QUOTED_STRING", "PRODUCT_RELEASE",
  "PRIVATE", "POS_NUMBER", "ORGANIZATION", "optional", "Opaque", "OID",
  "OF", "SEQUENCE", "SEMICOLON", "RPAREN", "REVISION", "REFERENCE",
  "read_write", "read_only", "read_create", "RBRACKET", "UNION",
  "UInteger32", "TRAP_TYPE", "TimeTicks", "TEXTUAL_CONVENTION", "SYNTAX",
  "SUPPORTS", "STRUCT", "STRING", "STATUS", "SIZE", "write_only",
  "VARIATION", "VARIABLES", "UPPERCASE_IDENTIFIER", "Unsigned32",
  "UNIVERSAL", "UNITS", "WRITE_SYNTAX", "$accept", "mibFile",
  "ModuleDefinitionList", "ModuleDefinition", "StartModuleDefinition",
  "assignedIdentifier", "imports", "symbolsImported",
  "SymbolsFromModuleList", "ModuleBody", "exports", "symbolsExported",
  "AssignmentList", "importList", "SymbolList", "SymbolFromModule",
  "Symbol", "ImportKeyword", "TypeAssignment", "Assignment",
  "ValueAssignment", "ObjectIdentifierValue", "ObjectIdentifierList",
  "Index", "Syntax", "Type", "DefinedValue", "ApplicationSyntax",
  "SimpleSyntax", "DefaultValue", "SequenceItemsList", "SequenceItem",
  "SequenceSyntax", "ObjectIdentifier", "TrapTypeDefinition",
  "variablePart", "ModuleConformanceDefinition",
  "ModuleComplianceDefinition", "MIBList", "MIB", "mandatoryPart",
  "AgentCapabilitiesDefinition", "NotificationGroupDefinition",
  "ObjectGroupDefinition", "NotificationTypeDefinition", "objectPart",
  "ObjectTypeDefinition", "ObjectAccessPart", "SMIv1Access", "unitsPart",
  "indexPart", "IndexTypeList", "IndexType", "Struct", "StartStruct",
  "StartUnion", "Union", "StructItemList", "StructItem", "SMIv3Syntax",
  "TextualConventionDefinition", "StartBogusTC", "displayPart",
  "ModuleIdentityDefinition", "StartModuleIdentity", "revisionPart",
  "RevisionList", "Revision", "ObjectIdentityDefinition", "ValueRange",
  "EndPoint", "empty", "VirtualTableConstructor", "descriptionPart",
  "DescriptionPart", "compliancePart", "ComplianceList", "Compliance",
  "ComplianceGroup", "ComplianceObject", "complianceAccessPart",
  "ComplianceAccess", "creationPart", "creation", "Variation",
  "variationAccessPart", "VariationAccess", "complianceSyntaxPart",
  "writeSyntaxPart", "moduleName", "moduleOID", "modulePart", "ModuleList",
  "Module", "variationPart", "VariationList", "VarTypeList", "VarTypes",
  "VarType", "SyntaxPart", "defaultValuePart", "NamedBit",
  "NamedBitListPlugin", "NamedBitList", "SMIv2Access", "ExtUTCTime",
  "StatusPart", "Status", "referencePart", "RangeRestriction",
  "SizeRestriction", "RangeListPlugin", "RangeList", "QuotedString",
  "LowercaseIdentifier", "UppercaseIdentifier", "Oid", "Number",
  "ReadOnly", "ReadWrite", "WriteOnly", "NotAccessible", "NotImplemented",
  "AccessibleForNotify", "ReadCreate", "BitsKeyword", "SequenceKeyword",
  "CurrentKeyword", "DeprecatedKeyword", "ObsoleteKeyword",
  "MandatoryKeyword", "OptionalKeyword", "OctetStringKeyword",
  "Integer32Keyword", "IpAddressKeyword", "Counter32Keyword",
  "Gauge32Keyword", "GaugeKeyword", "CounterKeyword", "Unsigned32Keyword",
  "TimeTicksKeyword", "OpaqueKeyword", "Counter64Keyword",
  "IntegerKeyword", "BitStringKeyword", "ObjectIdentifierKeyword",
  "SizeKeyword", "ObjectSyntaxKeyword", "TypeGarbage",
  "AlternativeTypeList", "NamedType", "TypeTag", "class", "NumericValue",
  "GarbageType", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short yyr1[] =
{
       0,   118,   119,   119,   120,   120,   121,   122,   122,   123,
     123,   124,   124,   125,   125,   126,   126,   127,   127,   128,
     128,   129,   129,   129,   130,   130,   131,   131,   131,   132,
     132,   132,   133,   133,   134,   134,   134,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   136,   136,   136,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   138,   138,   139,   139,   139,   140,   140,   141,
     141,   141,   141,   141,   141,   142,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   144,
     144,   144,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   148,
     148,   148,   149,   149,   149,   149,   149,   149,   149,   149,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
     150,   150,   150,   150,   150,   150,   150,   150,   151,   151,
     151,   151,   152,   152,   153,   153,   154,   155,   155,   156,
     156,   157,   158,   158,   159,   160,   161,   162,   163,   163,
     164,   164,   165,   165,   166,   166,   166,   166,   166,   166,
     167,   167,   168,   168,   168,   168,   169,   169,   169,   170,
     170,   171,   172,   173,   174,   175,   175,   176,   176,   176,
     177,   177,   178,   178,   179,   180,   180,   181,   182,   182,
     183,   183,   184,   184,   185,   186,   187,   187,   188,   188,
     188,   189,   190,   190,   191,   191,   192,   193,   193,   194,
     194,   195,   195,   196,   197,   198,   198,   199,   199,   199,
     199,   199,   200,   200,   201,   201,   202,   203,   203,   204,
     204,   204,   204,   204,   204,   204,   205,   205,   206,   206,
     207,   207,   208,   208,   209,   209,   210,   210,   211,   212,
     212,   213,   213,   214,   214,   215,   215,   215,   216,   216,
     217,   218,   218,   219,   219,   219,   220,   220,   221,   221,
     221,   222,   222,   222,   222,   222,   223,   224,   225,   225,
     225,   225,   225,   226,   226,   227,   228,   229,   229,   230,
     230,   231,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   233,   234,   235,   235,   235,
     235,   235,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   266,   266,   266,   266,   266,
     267,   267,   268,   268,   269,   270,   270,   270,   270,   271,
     271,   272,   272
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     1,     1,     2,     5,     3,     3,     1,
       1,     3,     1,     1,     1,     1,     2,     3,     2,     3,
       1,     1,     2,     1,     1,     2,     1,     2,     1,     1,
       3,     2,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     5,     5,     1,     1,     3,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     2,
       2,     1,     1,     4,     1,     2,     2,     2,     5,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     2,     4,     1,     2,     4,     1,
       2,     1,     1,     4,     4,     1,     1,     4,     2,     2,
       6,     4,     4,     1,     2,     2,     1,     1,     1,     1,
       1,     1,     2,     3,     3,     3,     3,     5,     4,     1,
       3,     2,     2,     3,     2,     2,     2,     3,     2,     2,
       1,     2,     1,     1,     1,     4,     1,     2,     2,     4,
       1,     2,     2,     4,     1,     1,     2,     2,     4,     1,
       1,     2,     2,     2,     4,     4,     4,     1,     1,     1,
       1,     1,     9,     9,     4,     1,    10,     8,     8,     1,
       2,     4,     4,     1,    10,    11,    11,     8,     4,     1,
      12,    12,     2,     2,     1,     1,     1,     1,     1,     1,
       2,     1,     4,     5,     4,     1,     1,     3,     2,     2,
       1,    10,     2,     2,    11,     1,     2,     1,     1,     1,
       1,     1,     8,     6,     3,     2,     1,    11,     2,     2,
       1,     1,     1,     2,     3,     7,     1,     3,     1,     1,
       1,     0,     3,     3,     1,     1,     2,     1,     1,     1,
       2,     1,     1,     3,     6,     2,     1,     1,     1,     1,
       1,     1,     4,     1,     1,     1,     8,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     1,     1,     1,     1,     1,     1,     2,     7,     1,
       1,     1,     2,     1,     2,     1,     3,     2,     1,     1,
       2,     4,     1,     4,     4,     4,     1,     2,     1,     3,
       2,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     2,     1,     3,     4,     1,     2,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     1,     1,     2,     3,     3,     4,     5,     1,
       1,     3,     2,     1,     4,     1,     1,     1,     1,     1,
       1,     1,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
     261,   378,   383,   382,   385,   352,   377,   376,   384,   386,
     374,   373,   379,   375,   365,     0,     2,     4,     0,     3,
     261,   261,   354,   355,   356,   357,   358,   359,   360,   362,
     361,   364,   363,   353,     1,     5,     0,     0,     0,     9,
      10,    74,    75,     0,   261,   371,   372,   369,   368,   370,
     367,     0,    77,   191,   189,   190,   188,     8,     7,   261,
       0,   261,    20,    76,    78,     0,     0,     0,    42,   398,
     390,   393,   380,   391,   392,   389,   388,   399,    46,    49,
      48,    47,    44,    43,    52,   403,    51,    50,   397,   395,
      45,   396,    53,   394,     0,    21,    29,    36,    37,    23,
      35,    34,    41,   102,   107,   108,   103,    38,    39,   104,
     105,   109,   110,    40,   106,     6,   261,    18,    12,     0,
       0,     0,    19,    22,    31,     0,    13,     0,    26,    15,
      14,    17,     0,    68,    24,    69,    67,    59,    58,    57,
      61,    63,    62,    65,    70,     0,     0,    71,    66,   261,
      60,     0,    64,     0,     0,   323,   324,   325,    30,    11,
      16,    28,     0,    27,    25,     0,     0,     0,     0,     0,
     246,     0,     0,     0,   248,     0,     0,   261,     0,     0,
       0,     0,     0,   233,     0,   232,     0,   249,     0,     0,
       0,     0,    33,    32,   421,     0,     0,   261,   133,     0,
       0,   381,   241,   244,   240,    56,    86,    91,    94,    92,
     126,   125,     0,     0,   116,   102,   108,   103,   119,   121,
     104,   105,   109,   110,   113,   122,   111,   136,     0,   409,
       0,   333,   334,   331,   332,   335,     0,   351,   245,     0,
       0,     0,   336,    55,     0,     0,     0,     0,     0,   209,
       0,     0,   261,     0,     0,     0,     0,   261,    54,     0,
     261,     0,     0,     0,     0,   400,   416,   417,   415,   418,
       0,   387,   401,     0,     0,     0,     0,   128,   129,     0,
       0,     0,   135,   117,     0,    87,    97,    88,   120,    89,
      95,     0,    90,    96,     0,   114,   134,     0,   112,     0,
       0,   404,   261,   261,   337,   338,   339,   340,   341,   342,
       0,   261,     0,     0,     0,   261,     0,     0,     0,   320,
      85,     0,     0,   221,   261,     0,     0,   261,     0,   261,
       0,     0,   261,     0,   413,   126,     0,   410,   420,   100,
      99,   419,     0,   263,   262,     0,   328,     0,   326,     0,
       0,     0,   402,   349,   256,     0,   347,   259,   260,   258,
       0,     0,     0,   149,     0,     0,     0,     0,     0,   406,
     405,   265,     0,   264,     0,   266,     0,   344,     0,     0,
       0,     0,     0,     0,   313,   315,   318,   319,   261,     0,
     220,     0,     0,     0,     0,     0,    72,     0,   261,   195,
       0,     0,     0,    73,   261,   422,   412,     0,   407,     0,
     414,     0,   131,   132,   127,   327,   330,     0,   345,   348,
       0,   124,     0,    93,   151,     0,   180,   152,   155,   154,
     175,   174,   179,   170,   102,   103,   162,   163,   104,   166,
     164,   160,     0,   187,   156,   159,   158,     0,   118,   115,
     123,     0,     0,   343,   243,     0,   261,     0,   261,     0,
     199,   208,   314,   317,     0,     0,   213,   215,   216,   217,
     214,   218,   219,   212,   261,     0,     0,     0,   261,   261,
       0,   261,   261,   408,   411,   101,     0,   329,   257,   350,
     346,    98,   150,     0,   176,   177,     0,   172,   171,     0,
     181,     0,   182,     0,   183,     0,   167,   168,     0,   161,
     153,   157,   237,   238,   239,     0,   235,     0,     0,     0,
       0,   261,   261,   301,   261,   261,     0,   200,   316,     0,
       0,   261,   255,     0,     0,     0,     0,     0,   261,     0,
     130,     0,     0,     0,     0,     0,     0,     0,     0,   236,
       0,   261,   261,   305,     0,   304,   306,     0,     0,   261,
     203,   302,   303,   300,   197,   207,   261,   261,   261,   194,
       0,   242,   198,   261,     0,   178,   173,   184,   185,   186,
     169,   165,     0,     0,     0,     0,   250,   252,   251,     0,
       0,   307,     0,     0,     0,     0,   268,   201,   267,   269,
     271,   272,     0,     0,     0,   261,   225,     0,   192,   261,
     193,   231,     0,     0,     0,   253,     0,   204,   196,     0,
       0,   261,   270,     0,     0,     0,     0,   322,     0,     0,
       0,   234,   254,   247,     0,   202,   273,   297,   261,   296,
     205,     0,     0,   230,    81,     0,   226,    79,    80,    83,
      82,    84,     0,     0,   206,     0,     0,     0,   299,   261,
     224,   229,     0,   222,   228,     0,   366,     0,   140,   138,
     139,   141,   137,   210,   211,   261,   298,     0,   276,     0,
     223,   227,   142,     0,   191,   189,     0,   188,   321,     0,
     310,   311,   308,   309,   275,   279,   280,   277,   278,   281,
     274,     0,   144,     0,   145,   146,   143,   261,   312,   148,
       0,   261,   147,   261,     0,   288,   261,   287,   291,   292,
     294,   295,   289,   290,   293,     0,   283,   261,   261,     0,
     285,     0,   284,   286,   282
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    15,    16,    17,    18,    38,   117,   125,   126,    60,
      61,    94,   131,   127,   128,   129,    96,    97,   133,   134,
     135,    39,    51,   643,   319,   320,   338,   206,   207,   667,
     362,   363,   427,    52,   136,   398,   137,   138,   459,   460,
     559,   139,   140,   141,   142,   248,   512,   393,   466,   322,
     605,   645,   646,   513,   145,   146,   514,   515,   516,   208,
     148,   149,   169,   150,   151,   585,   586,   587,   152,   353,
     354,   377,   209,   372,   373,   597,   598,   599,   600,   601,
     679,   694,   727,   731,   691,   716,   717,   638,   659,   524,
     563,   554,   555,   556,   692,   693,   383,   384,   385,   639,
     628,   346,   347,   348,   230,   241,   240,   304,   378,   277,
     278,   355,   356,   242,    41,    42,   671,   351,    22,    23,
      24,    25,    26,    27,    28,   212,   213,    29,    30,    31,
      32,    33,   214,   103,   104,   105,   106,   218,   219,   109,
     110,   111,   112,   224,   225,   226,   360,   114,   227,   336,
     337,   228,   270,   342,   229
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -608
static const short yypact[] =
{
    3185,  -608,  -608,  -608,  -608,  -608,  -608,  -608,  -608,  -608,
    -608,  -608,  -608,  -608,  -608,    39,  3185,  -608,    42,  -608,
    1090,  1090,  -608,  -608,  -608,  -608,  -608,  -608,  -608,  -608,
    -608,  -608,  -608,  -608,  -608,  -608,    46,  3023,    44,  -608,
    -608,  -608,  -608,   129,    38,  -608,  -608,  -608,  -608,  -608,
    -608,  2831,  -608,  -608,   100,   111,   117,  -608,  -608,  2321,
     148,    32,  -608,  -608,  -608,    62,    62,    62,  -608,  -608,
    -608,  -608,  -608,  -608,  -608,  -608,  -608,  -608,  -608,  -608,
    -608,  -608,  -608,  -608,  -608,  -608,  -608,  -608,  -608,  -608,
    -608,  -608,  -608,  -608,   107,  2000,  -608,  -608,  -608,  -608,
    -608,  -608,  -608,  -608,  -608,  -608,  -608,  -608,  -608,  -608,
    -608,  -608,  -608,  -608,  -608,  -608,  2321,  2321,  -608,   126,
     151,   157,  -608,  2321,  -608,   131,  2107,   186,  2214,  -608,
     233,  2321,   245,  -608,  -608,  -608,  -608,  -608,  -608,  -608,
    -608,  -608,  -608,  -608,  -608,   231,   242,  -608,  -608,   249,
    -608,   244,  -608,   808,    79,  -608,  -608,  -608,  -608,  -608,
    -608,  -608,  3185,  2321,  -608,  1353,    95,    95,   220,   190,
    -608,   220,  1677,   221,  -608,   261,   190,   236,   237,   204,
     190,   247,   289,  -608,   282,  -608,  1461,  -608,   190,   204,
     296,   290,  -608,  -608,   302,   292,   225,    20,  -608,   229,
     307,   262,  -608,  -608,  -608,  -608,  -608,  -608,  -608,  -608,
    -608,   136,   300,   308,    -4,   295,   295,   295,   295,  -608,
     295,   295,   298,   295,   110,   314,   298,  -608,  1245,  -608,
     190,  -608,  -608,  -608,  -608,  -608,   190,  -608,  -608,   258,
     334,   273,  -608,  -608,   220,   220,   334,   317,   190,  -608,
     318,  1677,   251,   334,   319,   363,  1090,   249,  -608,   334,
     251,   365,  1090,   355,  1677,  -608,  -608,  -608,  -608,  -608,
    3023,  -608,  -608,  3185,   260,  3023,  2879,  -608,  -608,  3023,
    3185,  3023,  -608,  -608,  3023,  -608,  -608,  -608,  -608,  -608,
    -608,   265,  -608,  -608,  3023,  -608,  -608,  3023,  -608,  1677,
    1677,  -608,   334,   334,  -608,  -608,  -608,  -608,  -608,  -608,
     220,   281,   220,   190,   297,   281,  3185,   334,  3185,  -608,
    -608,   220,    89,  -608,   281,  3185,  1090,   268,   190,   281,
      89,  1090,   268,  1943,  -608,  1677,    16,  -608,  -608,  -608,
     342,  -608,   287,  -608,  -608,   137,  -608,   306,  2927,   100,
     111,   117,  -608,  -608,   347,   299,   385,  -608,  -608,  -608,
     295,   309,   992,  -608,  1785,  1785,   310,   312,   321,  -608,
    -608,  -608,   352,  -608,   356,  -608,   220,  -608,   204,   388,
     334,   220,   335,   323,  3054,  -608,  -608,  -608,   281,   324,
    -608,    54,    95,   190,   401,   328,  -608,   367,   334,  -608,
     334,   335,   190,  -608,   334,  -608,  -608,  1569,  -608,   260,
    -608,  3023,  -608,  -608,  -608,  3023,  -608,  3023,  -608,  3023,
     322,  -608,  3089,  -608,  -608,   262,  -608,  -608,  -608,  -608,
    -608,   155,  -608,   184,   187,   215,  -608,  -608,   217,   253,
     376,   298,  1893,  -608,  -608,  -608,  -608,  1893,  -608,  -608,
    -608,  3185,  3185,  -608,  -608,   220,   281,   334,   260,    22,
    -608,  -608,  3185,  -608,   415,   190,  -608,  -608,  -608,  -608,
    -608,  -608,  -608,  -608,   334,  1090,   190,  3185,   281,   281,
      55,   334,   281,  -608,  -608,  -608,   343,  -608,  -608,  -608,
    -608,  -608,  -608,  3023,  -608,  -608,  3023,  -608,  -608,  3023,
    -608,  3023,  -608,  3023,  -608,  3023,  -608,  -608,  3023,  -608,
    -608,  -608,  -608,  -608,  -608,  3120,  -608,   -38,   351,  3185,
     334,   327,   327,  -608,   372,  1090,  1090,  -608,  -608,  1090,
     334,   281,  -608,   334,   348,   428,   204,  1090,   281,   430,
    -608,   354,   357,   358,   359,   364,   369,   370,   438,  -608,
    3164,   360,   260,  -608,   440,   327,  -608,   449,   414,    48,
    -608,  -608,  -608,  -608,  -608,  -608,   281,    24,   281,  -608,
      62,  -608,  -608,    24,    62,  -608,  -608,  -608,  -608,  -608,
    -608,  -608,  1090,   453,   220,   454,   360,  -608,  -608,   416,
    1090,  -608,  1090,  3185,  1943,  1943,  -608,  -608,    48,  -608,
    -608,  -608,   461,   425,   427,   445,  -608,   466,  -608,   445,
    -608,  -608,  1090,   334,  1090,  -608,   432,  -608,  -608,   392,
     334,   204,  -608,  1090,  1943,  2575,   433,  -608,   474,  1090,
     476,  -608,  -608,  -608,  3185,  -608,  -608,  -608,   366,  -608,
    -608,   404,  2654,  -608,  -608,  2417,  -608,  -608,  -608,  -608,
    -608,  -608,  2735,  1090,  -608,  1090,   405,  1677,  -608,   421,
    -608,  -608,  2496,  -608,  -608,  2783,  -608,   408,  -608,  -608,
    -608,  -608,  -608,  -608,  -608,   378,  -608,    95,  -608,   334,
    -608,  -608,  -608,  3023,   410,     9,   411,   -16,  -608,  1943,
    -608,  -608,  -608,   378,  -608,  -608,  -608,  -608,  -608,  -608,
    -608,  2975,  -608,  3185,  -608,  -608,  -608,   204,  -608,  -608,
     417,   366,  -608,   484,   902,  -608,   473,  -608,  -608,  -608,
    -608,  -608,  -608,  -608,  -608,   464,  -608,   445,  3185,   334,
    -608,   431,  -608,  -608,  -608
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -608,  -608,  -608,   491,  -608,   488,  -608,  -608,  -608,  -608,
    -608,  -608,  -608,  -608,   451,   386,   -45,    68,  -608,   382,
    -608,   429,  -169,  -122,  -136,   240,  -608,   -26,  -608,  -608,
    -608,  -328,  -335,   -50,  -608,   191,  -608,  -608,   124,  -407,
    -608,  -608,  -608,  -608,  -608,  -608,    92,   198,  -608,   269,
     -43,  -608,  -597,   130,  -608,  -608,   135,    14,  -377,  -307,
    -608,  -608,   275,  -608,  -608,  -608,  -608,   -52,  -608,   118,
     121,   570,  -288,  -216,   -57,  -608,  -608,   -56,  -608,  -608,
    -608,  -608,  -608,  -608,  -152,  -608,  -608,  -162,  -160,    -6,
    -608,    25,  -608,    -1,  -608,  -608,  -263,  -608,  -361,  -170,
    -607,   -33,   201,  -608,  -150,  -235,   133,  -608,  -275,   763,
     520,  -608,  -608,  -165,     0,   441,  -116,   -22,  -161,  -159,
    -369,  -155,  -154,  -153,  -146,   668,  -608,   326,   332,   337,
     339,   340,  -232,  -140,  -608,   -80,  -130,   692,   712,   -11,
      41,   498,   501,    13,  -164,   179,  -608,  -608,  -608,  -608,
     159,  -255,  -608,  -608,   -18
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -262
static const short yytable[] =
{
      20,    64,   630,   238,    53,   231,   231,   232,   232,   252,
     314,   233,   233,   234,   234,    56,    20,   236,    53,   260,
     235,   235,   469,   463,   703,   215,   266,   526,   603,    56,
     444,   407,   215,    98,   424,   217,   179,    54,   281,    34,
     382,    67,   217,   119,   120,   121,   215,    36,   664,   394,
     124,    54,   527,   276,   401,   389,   217,   428,   445,   100,
     537,   183,   395,     1,   706,   681,    65,    44,   185,    98,
      57,   604,   113,   527,    59,   519,   429,   446,   158,   313,
     594,   116,    45,   124,   186,   216,   458,   374,   215,   704,
      98,    98,   216,    46,   492,   100,   408,    98,   217,   391,
      98,   528,    98,   267,     1,    98,   216,   510,   113,   442,
     447,   215,   511,   464,    47,    48,   100,   153,   158,   458,
     729,   217,    49,   100,   215,     7,   100,   595,   100,   113,
     113,   153,   433,   433,   217,   268,   113,    98,   549,   113,
     187,   113,   188,   392,   113,   375,    50,   379,   216,    10,
      11,    12,   294,   189,   220,    58,   390,    65,   190,   215,
     215,   220,   192,   100,    13,   210,     7,   276,    66,   217,
     217,   216,   210,   549,    67,   220,   113,   274,   275,   411,
     191,   521,   478,   311,   216,   132,   210,   115,   482,   315,
      10,    11,    12,   276,   276,   215,   324,   493,   122,   132,
     440,   440,   329,   535,   536,   217,   221,   539,   454,   143,
     433,   453,   276,   221,   534,   433,   457,   220,   155,   216,
     216,   162,   159,   143,   434,   434,   496,   221,   210,   499,
     467,   231,   468,   232,   435,   435,   470,   233,   471,   234,
     220,   276,   473,   156,   284,   472,   235,   144,   341,   157,
     165,   210,   147,   220,   359,   216,   567,   501,   531,   503,
     388,   144,   359,   573,   335,   538,   147,   215,   -28,   221,
     339,   168,   284,   343,   284,   349,   357,   217,   440,   349,
     364,   349,     2,   440,   357,   166,     3,   171,   220,   220,
     520,   602,   221,   607,   349,   505,   167,   349,   239,   210,
     210,   237,   434,   244,   245,   221,   250,   434,   251,   246,
     276,   247,   435,   253,     4,   416,   386,   435,   386,   255,
     256,   259,   254,   456,   220,   386,   261,   216,   262,   263,
     619,     8,   265,   405,   264,   210,   271,   272,   426,   426,
     221,   221,   279,   479,     9,   720,   443,   443,   349,   613,
     280,   273,   284,   438,   438,   291,   297,   310,   312,   316,
     318,   325,   364,   302,   430,   430,   571,   321,   326,   303,
     331,   656,   333,    14,   352,   376,   221,   439,   439,   381,
     397,   317,   487,   409,   386,   410,   414,   417,   419,   421,
     448,   418,   449,   649,   451,   359,   220,   359,   452,   458,
     522,   450,   455,   461,   465,   205,   475,   335,   476,   477,
     649,   349,   243,   649,   490,   349,   426,   357,   508,   357,
     529,   426,   364,   540,   443,   189,   258,   558,   569,   443,
     649,   438,   552,   570,   575,   574,   438,   576,   577,   578,
     710,    21,   430,   582,   579,   590,   380,   430,   221,   580,
     581,   517,   517,   584,   592,   439,   593,    21,   612,   614,
     439,   400,   386,   551,   616,   732,   623,   624,   301,   625,
     626,   629,   635,   566,   634,   652,   568,   386,    55,   653,
     361,   655,   366,   657,   660,   675,   677,   668,   688,   689,
     702,   705,    55,   349,   714,   367,   349,   712,   368,   349,
     101,   349,   725,   349,   334,   349,   728,    35,   349,    43,
      95,   734,   160,   164,   701,   517,   695,   215,   696,   517,
     661,   676,   697,   404,   698,   480,   474,   217,   402,   330,
     609,   699,   328,   550,   615,   481,   101,   489,   488,   369,
     370,   708,   622,   441,   441,   711,   589,   557,   608,   686,
     517,   713,   610,   718,   591,   719,   632,   101,   154,   721,
     722,   723,     0,   636,   101,   305,   484,   101,   724,   101,
      19,   306,   154,     0,     0,   406,   307,   216,   308,   309,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      40,    40,     0,   386,   620,   621,     0,     0,   530,   644,
       0,     0,     0,   193,   101,     0,   211,     0,     0,   533,
       0,     0,   486,   211,    62,   683,   644,     0,     0,   644,
       0,   441,   700,     0,   641,   647,   441,   211,     0,    99,
     672,   118,   684,     0,   386,     0,   644,     0,   650,     0,
       0,     0,   647,   687,     0,   647,   220,   334,     0,     0,
      53,    64,   669,     0,     0,   650,     0,   210,   650,     0,
       0,    56,   647,   222,     0,   685,   223,     0,    53,   211,
     222,     0,   733,   223,     0,   650,     0,     0,     0,    56,
       0,     0,     0,    54,   222,   327,   130,   223,     0,   707,
       0,   332,   211,     0,   541,     0,   161,   542,   221,     0,
     543,    54,   544,   386,   545,   211,   546,     0,     0,   547,
       0,   340,     0,     0,   344,   345,   350,   358,     0,   170,
     350,   365,   350,     0,     0,   358,   222,   102,   386,   223,
       0,     0,     0,     0,   283,   350,     0,     0,   350,     0,
     211,   211,   292,     0,   296,     0,   298,   249,     0,   222,
       0,   107,   223,     0,     0,   396,     0,   387,     0,   387,
     403,     0,   222,   102,     0,   223,   387,   269,     0,     0,
       0,   108,     0,     0,     0,     0,   211,     0,     0,     0,
       0,     0,     0,     0,   102,   102,     0,   107,     0,   350,
       0,   102,     0,     0,   102,     0,   102,   222,   222,   102,
     223,   223,     0,   365,   651,   431,   431,   108,   107,   107,
       0,     0,     0,   172,     0,   107,   173,     0,   107,     0,
     107,   651,   323,   107,   651,   387,     0,   170,   108,   108,
     323,   102,     0,   222,     0,   108,   223,     0,   108,     0,
     108,   651,     0,   108,     0,     0,     0,     0,   211,     0,
     485,     0,   350,     0,     0,   107,   350,     0,   358,     0,
     358,     0,     0,   365,     0,   413,     0,     0,     0,   174,
     175,   176,   371,   371,     0,   108,   177,     0,   178,     0,
       0,     0,   179,   431,     0,   180,   181,   182,   431,     0,
       0,     0,   518,   518,     0,     0,     0,   399,     0,   525,
       0,     0,   399,   387,   532,   222,     0,   183,   223,   184,
       0,     1,     0,     0,   185,     0,     0,     0,   387,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   350,     0,     0,   350,     0,     0,
     350,     0,   350,     0,   350,     0,   350,     0,     0,   350,
       0,   495,     0,   498,   561,   564,   518,     0,   565,   507,
     518,   509,     0,     0,     0,     0,   572,     0,   371,     6,
       0,     0,     0,     7,   371,     0,     0,   282,   285,   286,
     287,   288,     0,   289,   290,     0,   293,   295,     0,     0,
       0,   518,     0,   525,     0,     0,     0,    10,    11,    12,
       0,     1,     0,     0,     0,     0,     0,   422,     0,     0,
       0,   611,    13,     0,     0,     0,     2,     0,     0,   617,
       3,   618,     0,     0,     0,     0,     0,     0,   523,     0,
       0,     0,   432,   432,   387,     0,     0,     0,     0,     0,
       0,   631,     0,   633,   371,     0,     0,     0,     4,     0,
       5,   371,   640,     0,     0,     0,   436,   436,   654,     6,
       0,     0,     0,     7,     0,     8,   648,     0,     0,     0,
       0,     0,   423,     0,     0,   387,   437,   437,     9,     0,
       0,     0,   673,   648,   674,     0,   648,    10,    11,    12,
       0,   553,   553,   670,   560,   562,     0,     0,   211,     1,
       0,     0,    13,   648,     0,    14,    55,     0,   412,     0,
     432,     0,     0,     0,     2,   432,     0,     0,     3,     0,
       0,   588,   523,   420,    55,     0,     0,     0,     0,   596,
       0,     0,    37,     0,   436,     0,     0,   606,     0,   436,
       0,     0,    55,   606,   387,     0,     4,     0,     5,     0,
       0,     0,     0,     0,   437,   222,     0,     6,   223,   437,
       0,     7,     0,     8,     0,     0,     0,     0,     0,   387,
       0,     0,     0,     0,     0,   627,     9,     0,     0,   627,
       0,     0,     0,     0,     0,    10,    11,    12,     0,     0,
       0,   637,     0,     0,   494,     0,   497,   500,   502,     0,
      13,   504,   506,    14,     0,     0,     0,     0,   658,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   678,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   690,     0,     0,     0,     0,
       0,     0,   194,     0,     1,     0,    69,    70,    71,     0,
       0,   195,     0,    72,   196,     0,     0,     0,     0,     2,
       0,     0,     0,     3,     0,     0,     0,   637,    73,    74,
       0,   658,   299,   715,     0,     0,   726,     0,     0,    75,
      76,    77,     0,     0,     0,     0,   300,   627,   730,     0,
       0,     4,     0,     5,   197,     0,     0,     0,     0,     0,
       0,   198,     6,     0,     0,     0,     7,   199,     8,     0,
       0,    85,     0,     0,   200,     0,     0,     0,     0,     0,
       0,     9,    88,     0,     0,   201,     0,     0,     0,     0,
      10,    11,    12,     0,   202,    89,     0,    91,     0,     0,
       0,   204,     0,     0,     0,    13,     0,     0,    14,    93,
     194,     0,     1,     0,    69,    70,    71,     0,     0,   195,
       0,    72,   196,     0,     0,     0,     0,     2,     0,     0,
       0,     3,     0,     0,     0,     0,    73,    74,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    75,    76,    77,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     4,
       0,     5,   197,     0,     0,     0,     0,     0,     0,   198,
       6,     0,     0,     0,     7,   199,     8,     0,     0,    85,
       0,     0,   200,     0,     0,     0,     0,     0,     0,     9,
      88,     0,     0,   201,     0,     0,     0,     0,    10,    11,
      12,     0,   202,    89,     0,    91,   203,     0,     0,   204,
       0,     0,     0,    13,     0,     0,    14,    93,   194,     0,
       1,     0,    69,    70,    71,     0,     0,   195,     0,    72,
     196,     0,     0,     0,     0,     2,     0,     0,     0,     3,
       0,     0,     0,     0,    73,    74,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    75,    76,    77,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     0,     5,
     197,     0,     0,     0,     0,     0,     0,   198,     6,     0,
       0,     0,     7,   199,     8,     0,     0,    85,     0,     0,
     200,     0,     0,     0,     0,     0,     0,     9,    88,     0,
       0,   201,     0,     0,     0,     0,    10,    11,    12,     0,
     202,    89,     0,    91,   257,     0,     0,   204,     0,     0,
       0,    13,     0,     0,    14,    93,   194,     0,     1,     0,
      69,    70,    71,     0,     0,   195,     0,    72,   196,     0,
       0,     0,     0,     2,     0,     0,     0,     3,     0,     0,
       0,     0,    73,    74,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    75,    76,    77,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     4,     0,     5,   197,     0,
       0,     0,     0,     0,     0,   198,     6,     0,     0,     0,
       7,   199,     8,     0,     0,    85,     0,     0,   200,   483,
       0,     0,     0,     0,     0,     9,    88,     0,     0,   201,
       0,     0,     0,     0,    10,    11,    12,     0,   202,    89,
       0,    91,     0,     0,     0,   204,     0,     0,     0,    13,
       0,     0,    14,    93,   194,     0,     1,     0,    69,    70,
      71,     0,     0,   195,     0,    72,   196,     0,     0,     0,
       0,     2,     0,     0,     0,     3,     0,     0,     0,     0,
      73,    74,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    75,    76,    77,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     4,     0,     5,   197,     0,     0,     0,
       0,     0,     0,   198,     6,     0,     0,     0,     7,   199,
       8,     0,     0,    85,     0,     0,   200,     0,     0,     0,
       0,     0,     0,     9,    88,     0,     0,   201,     0,     0,
       0,     0,    10,    11,    12,     0,   202,    89,     0,    91,
       0,     0,     0,   204,     0,     0,     0,    13,     0,     0,
      14,    93,   194,     0,     1,     0,    69,    70,    71,     0,
       0,     0,     0,    72,   196,     0,     0,     0,     0,     2,
       0,     0,     0,     3,     0,     0,     0,     0,    73,    74,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    75,
      76,    77,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     4,     0,     5,   197,     0,     0,     0,     0,     0,
       0,     0,     6,     0,     0,     0,     7,   199,     8,     0,
       0,    85,     0,     0,   200,     0,     0,     0,     0,     0,
       0,     9,    88,     0,     0,   425,     0,     0,     0,     0,
      10,    11,    12,     0,   202,    89,     0,    91,     0,     0,
       0,   204,     0,     0,     0,    13,     0,     0,    14,    93,
     194,     0,     1,     0,    69,    70,    71,     0,     0,     0,
       0,    72,   196,     0,     0,     0,     0,     2,     0,     0,
       0,     3,     0,     0,     0,     0,    73,    74,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    75,    76,    77,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     4,
       0,     5,     1,     0,     0,     0,     0,     0,     0,     0,
       6,     0,     0,     0,     7,   199,     8,     2,     0,    85,
       0,     3,   200,     0,     0,     0,     0,     0,     0,     9,
      88,     0,     0,     0,     0,     0,     0,     0,    10,    11,
      12,     0,     0,    89,     0,    91,     0,     0,     0,     4,
       0,     5,     0,    13,     0,     0,    14,    93,    68,     1,
       6,    69,    70,    71,     7,   123,     8,     0,    72,     0,
       0,     0,     0,     0,     2,     0,     0,     0,     3,     9,
       0,     0,     0,    73,    74,     0,     0,     0,    10,    11,
      12,     0,     0,     0,    75,    76,    77,     0,     0,     0,
       0,     0,     0,    13,     0,    78,     4,     0,     5,     0,
       0,    79,    80,    81,     0,     0,     0,     6,    82,     0,
      83,     7,     0,     8,    84,     0,    85,    86,    87,     0,
       0,     0,     0,     0,     0,     0,     9,    88,     0,     0,
       0,     0,     0,     0,     0,    10,    11,    12,     0,     0,
      89,    90,    91,    92,     0,     0,     0,     0,     0,     0,
      13,     0,     0,    14,    93,    68,     1,     0,    69,    70,
      71,     0,     0,     0,     0,    72,     0,     0,     0,     0,
       0,     2,     0,     0,     0,     3,     0,     0,     0,     0,
      73,    74,  -261,     0,     0,     0,     0,     0,     0,     0,
       0,    75,    76,    77,     0,     0,     0,     0,     0,     0,
       0,     0,    78,     4,     0,     5,     0,     0,    79,    80,
      81,     0,     0,     0,     6,    82,     0,    83,     7,     0,
       8,    84,     0,    85,    86,    87,     0,     0,     0,     0,
       0,     0,     0,     9,    88,     0,     0,     0,     0,     0,
       0,     0,    10,    11,    12,     0,     0,    89,    90,    91,
      92,     0,     0,     0,     0,     0,     0,    13,     0,     0,
      14,    93,    68,     1,     0,    69,    70,    71,     0,   163,
       0,     0,    72,     0,     0,     0,     0,     0,     2,     0,
       0,     0,     3,     0,     0,     0,     0,    73,    74,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    75,    76,
      77,     0,     0,     0,     0,     0,     0,     0,     0,    78,
       4,     0,     5,     0,     0,    79,    80,    81,     0,     0,
       0,     6,    82,     0,    83,     7,     0,     8,    84,     0,
      85,    86,    87,     0,     0,     0,     0,     0,     0,     0,
       9,    88,     0,     0,     0,     0,     0,     0,     0,    10,
      11,    12,     0,     0,    89,    90,    91,    92,     0,     0,
       0,     0,     0,     0,    13,     0,     0,    14,    93,    68,
       1,     0,    69,    70,    71,     0,     0,     0,     0,    72,
       0,     0,     0,     0,     0,     2,     0,     0,     0,     3,
       0,     0,     0,     0,    73,    74,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    75,    76,    77,     0,     0,
       0,     0,     0,     0,     0,     0,    78,     4,     0,     5,
       0,     0,    79,    80,    81,     0,     0,     0,     6,    82,
       0,    83,     7,     0,     8,    84,     0,    85,    86,    87,
       0,     0,     0,     0,     0,     0,     0,     9,    88,     0,
       0,     0,     0,     0,     0,     0,    10,    11,    12,     0,
       0,    89,    90,    91,    92,     0,     1,     0,    69,    70,
       0,    13,   662,     0,    14,    93,     0,     0,     0,     0,
       0,     2,     0,     0,     0,     3,     0,     0,     0,     0,
      73,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    75,    76,    77,     0,     0,     0,   642,     0,     0,
       0,     0,     0,     4,     0,     5,     0,     0,     0,     0,
       0,     0,     0,     0,     6,     0,     0,     0,     7,   199,
       8,     0,     0,    85,     0,     0,   200,   663,     0,     0,
       0,     0,     0,     9,    88,     1,     0,    69,    70,     0,
       0,     0,    10,    11,    12,     0,     0,    89,     0,    91,
       2,     0,     0,     0,     3,     0,     0,    13,     0,    73,
      14,    93,     0,     0,     0,     0,     0,     0,     0,     0,
      75,    76,    77,     0,     0,     0,   642,     0,     0,     0,
       0,     0,     4,     0,     5,     0,     0,     0,     0,     0,
       0,     0,     0,     6,     0,     0,     0,     7,   199,     8,
       0,     0,    85,     0,     0,   200,   680,     0,     0,     0,
       0,     0,     9,    88,     1,     0,    69,    70,     0,     0,
       0,    10,    11,    12,     0,     0,    89,     0,    91,     2,
       0,     0,     0,     3,     0,     0,    13,     0,    73,    14,
      93,     0,     0,     0,     0,     0,     0,     0,     0,    75,
      76,    77,     0,     0,     0,   642,     0,     0,     0,     0,
       0,     4,     0,     5,     0,     0,     0,     0,     0,     0,
       0,     0,     6,     0,     0,     0,     7,   199,     8,     0,
       0,    85,     0,     0,   200,     0,     0,     0,     0,     0,
       0,     9,    88,     1,     0,    69,    70,     0,     0,     0,
      10,    11,    12,     0,     0,    89,     0,    91,     2,     0,
       0,     0,     3,     0,     0,    13,     0,    73,    14,    93,
       0,     0,     0,     0,     0,     0,     0,     0,    75,    76,
      77,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       4,     0,     5,     0,     0,     0,     0,     0,     0,     0,
       0,     6,     0,     0,     0,     7,   199,     8,     0,     0,
      85,     0,     0,   200,     0,     0,     0,     0,     0,     0,
       9,    88,     0,     0,     1,     0,     0,     0,     0,    10,
      11,    12,     0,     0,    89,    45,    91,     0,     0,     2,
       0,     0,     0,     3,    13,     0,    46,    14,    93,     0,
       0,     0,     0,     0,     0,     0,     0,   665,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    47,    48,     0,
       0,     4,     1,     5,     0,    49,     0,     0,     0,     0,
       0,     0,     6,    45,     0,     0,     7,     2,     8,     0,
       0,     3,     0,     0,    46,     0,   237,     0,     0,    50,
       0,     9,     0,   666,     0,     0,     0,     0,     0,     0,
      10,    11,    12,     0,     0,    47,    48,     0,     0,     4,
       1,     5,     0,    49,     0,    13,     0,     0,    14,     0,
       6,    45,     0,     0,     7,     2,     8,     0,     0,     3,
       0,     0,    46,   682,     0,     0,     0,    50,     0,     9,
       0,   666,     0,     0,     0,     0,     0,     0,    10,    11,
      12,     0,     0,    47,    48,     0,     0,     4,     1,     5,
       0,    49,     0,    13,     0,     0,    14,     0,     6,    45,
       0,     0,     7,     2,     8,     0,     0,     3,     0,     0,
      46,    63,     0,     0,     0,    50,     0,     9,     0,     0,
       0,     0,     0,     0,     0,     0,    10,    11,    12,     0,
       0,    47,    48,     0,     0,     4,     1,     5,     0,    49,
       0,    13,   415,     0,    14,     0,     6,    45,     0,     0,
       7,     2,     8,     0,     0,     3,     0,     0,    46,     0,
       0,     0,     0,    50,     0,     9,     0,     0,     0,     0,
       0,     0,     0,     0,    10,    11,    12,     0,     0,    47,
      48,     0,     0,     4,     1,     5,     0,    49,   352,    13,
       0,     0,    14,     0,     6,    45,     0,     0,     7,     2,
       8,     0,     0,     3,     0,     0,    46,     0,     0,     0,
       0,    50,     0,     9,     0,     0,     0,     0,     0,     0,
       0,     0,    10,    11,    12,     0,     0,    47,    48,     0,
       0,     4,     1,     5,     0,    49,     0,    13,     0,     0,
      14,     0,     6,    45,     0,     0,     7,     2,     8,     0,
       0,     3,     0,     0,    46,   709,     0,     0,     0,    50,
       0,     9,     0,     1,     0,     0,     0,     0,     0,   462,
      10,    11,    12,     0,     0,    47,    48,     0,     2,     4,
       0,     5,     3,    49,     0,    13,     0,     0,    14,     0,
       6,     0,     0,     0,     7,     0,     8,     0,     1,     0,
       0,     0,     0,     0,     0,     0,     0,    50,     0,     9,
       4,     0,     5,     2,     0,     0,     0,     3,    10,    11,
      12,     6,     0,     0,     0,     7,     0,     8,     0,     1,
       0,     0,     0,    13,     0,     0,    14,     0,     0,     0,
       9,     0,     0,     0,     2,     4,     0,     5,     3,    10,
      11,    12,     0,     0,     0,     0,     6,     0,     0,     0,
       7,     0,     8,     0,    13,     0,     0,    14,     0,   491,
       0,     0,     0,     1,     0,     9,     4,     0,     5,     0,
       0,     0,     0,     0,    10,    11,    12,     6,     2,     0,
       0,     7,     3,     8,     1,     0,     0,     0,     0,    13,
     548,     0,    14,     0,     0,     0,     9,     0,     0,     2,
       0,     0,     0,     3,     0,    10,    11,    12,     0,     0,
       4,     0,     5,     0,     0,     0,     0,     0,     0,     0,
      13,     6,     0,    14,     0,     7,     0,     8,     0,     0,
       0,     4,     0,     5,   583,     0,     0,     0,     0,     0,
       9,     0,     6,     0,     0,     0,     7,     0,     8,    10,
      11,    12,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     9,     0,     0,    13,     0,     0,    14,     0,     0,
      10,    11,    12,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    13,     0,     0,    14
};

static const short yycheck[] =
{
       0,    51,   609,   168,    37,   166,   167,   166,   167,   179,
     245,   166,   167,   166,   167,    37,    16,   167,    51,   189,
     166,   167,   391,   384,    15,   165,     6,     5,     4,    51,
     365,    15,   172,    59,   362,   165,    74,    37,    42,     0,
     315,    57,   172,    65,    66,    67,   186,     5,   645,   324,
      95,    51,   459,    57,   329,   318,   186,   364,   365,    59,
       5,    99,   325,     9,    80,   662,    57,    21,   106,    95,
      26,    47,    59,   480,    36,   452,   364,   365,   123,   244,
      32,    49,    20,   128,     5,   165,    64,   303,   228,    80,
     116,   117,   172,    31,   422,    95,    80,   123,   228,    10,
     126,   462,   128,    83,     9,   131,   186,   442,    95,   364,
     365,   251,   447,   388,    52,    53,   116,   117,   163,    64,
     727,   251,    60,   123,   264,    71,   126,    79,   128,   116,
     117,   131,   364,   365,   264,   115,   123,   163,   515,   126,
      61,   128,    63,    54,   131,   310,    84,   312,   228,    95,
      96,    97,    42,    74,   165,    26,   321,    57,    79,   299,
     300,   172,   162,   163,   110,   165,    71,    57,    57,   299,
     300,   251,   172,   550,    57,   186,   163,    41,    42,    42,
     101,   456,   398,   240,   264,   117,   186,    39,   404,   246,
      95,    96,    97,    57,    57,   335,   253,    42,    91,   131,
     364,   365,   259,   478,   479,   335,   165,   482,   378,   117,
     442,   376,    57,   172,   477,   447,   381,   228,    92,   299,
     300,    35,    91,   131,   364,   365,    42,   186,   228,    42,
     391,   392,   391,   392,   364,   365,   391,   392,   391,   392,
     251,    57,   392,    92,    57,   391,   392,   117,   270,    92,
       5,   251,   117,   264,   276,   335,   531,    42,   474,    42,
     317,   131,   284,   538,   264,   481,   131,   407,    35,   228,
     270,    22,    57,   273,    57,   275,   276,   407,   442,   279,
     280,   281,    24,   447,   284,    54,    28,    43,   299,   300,
     455,   566,   251,   568,   294,    42,    54,   297,   108,   299,
     300,    81,   442,    82,    43,   264,    69,   447,   104,   176,
      57,    75,   442,   180,    56,   348,   316,   447,   318,    30,
      38,   188,    75,   380,   335,   325,    30,   407,    38,    27,
     593,    73,   107,   333,    42,   335,   107,    30,   364,   365,
     299,   300,    42,   400,    86,   714,   364,   365,   348,   584,
      42,    89,    57,   364,   365,    57,    42,    23,    85,    42,
      42,    42,   362,   230,   364,   365,   536,   116,     5,   236,
       5,   634,    17,   113,   109,    94,   335,   364,   365,    82,
     112,   248,   415,    41,   384,    98,    80,    40,     3,    80,
      80,    92,    80,   625,    42,   417,   407,   419,    42,    64,
     457,    80,    14,    80,    80,   165,     5,   407,    80,    42,
     642,   411,   172,   645,    92,   415,   442,   417,    42,   419,
       5,   447,   422,    80,   442,    74,   186,    55,    80,   447,
     662,   442,   105,     5,    80,     5,   447,    80,    80,    80,
     703,     0,   442,     5,    80,     5,   313,   447,   407,    80,
      80,   451,   452,    93,     5,   442,    42,    16,     5,     5,
     447,   328,   462,   520,    48,   728,     5,    42,   228,    42,
      25,     5,    80,   530,    42,    42,   533,   477,    37,     5,
     279,     5,   281,   117,    80,    80,    65,   652,    80,   111,
      80,    80,    51,   493,    10,   294,   496,    80,   297,   499,
      59,   501,    29,   503,   264,   505,    42,    16,   508,    21,
      59,    80,   126,   131,   683,   515,   677,   657,   677,   519,
     642,   657,   677,   332,   677,   401,   393,   657,   330,   260,
     573,   677,   257,   519,   586,   402,    95,   419,   417,   299,
     300,   693,   598,   364,   365,   707,   552,   522,   570,   665,
     550,   711,   574,   714,   555,   714,   613,   116,   117,   714,
     714,   714,    -1,   620,   123,   239,   407,   126,   714,   128,
       0,   239,   131,    -1,    -1,   335,   239,   657,   239,   239,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      20,    21,    -1,   593,   594,   595,    -1,    -1,   465,   625,
      -1,    -1,    -1,   162,   163,    -1,   165,    -1,    -1,   476,
      -1,    -1,   411,   172,    44,   665,   642,    -1,    -1,   645,
      -1,   442,   679,    -1,   624,   625,   447,   186,    -1,    59,
     652,    61,   665,    -1,   634,    -1,   662,    -1,   625,    -1,
      -1,    -1,   642,   665,    -1,   645,   657,   407,    -1,    -1,
     683,   701,   652,    -1,    -1,   642,    -1,   657,   645,    -1,
      -1,   683,   662,   165,    -1,   665,   165,    -1,   701,   228,
     172,    -1,   729,   172,    -1,   662,    -1,    -1,    -1,   701,
      -1,    -1,    -1,   683,   186,   256,   116,   186,    -1,   689,
      -1,   262,   251,    -1,   493,    -1,   126,   496,   657,    -1,
     499,   701,   501,   703,   503,   264,   505,    -1,    -1,   508,
      -1,   270,    -1,    -1,   273,   274,   275,   276,    -1,   149,
     279,   280,   281,    -1,    -1,   284,   228,    59,   728,   228,
      -1,    -1,    -1,    -1,   214,   294,    -1,    -1,   297,    -1,
     299,   300,   222,    -1,   224,    -1,   226,   177,    -1,   251,
      -1,    59,   251,    -1,    -1,   326,    -1,   316,    -1,   318,
     331,    -1,   264,    95,    -1,   264,   325,   197,    -1,    -1,
      -1,    59,    -1,    -1,    -1,    -1,   335,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   116,   117,    -1,    95,    -1,   348,
      -1,   123,    -1,    -1,   126,    -1,   128,   299,   300,   131,
     299,   300,    -1,   362,   625,   364,   365,    95,   116,   117,
      -1,    -1,    -1,     5,    -1,   123,     8,    -1,   126,    -1,
     128,   642,   252,   131,   645,   384,    -1,   257,   116,   117,
     260,   163,    -1,   335,    -1,   123,   335,    -1,   126,    -1,
     128,   662,    -1,   131,    -1,    -1,    -1,    -1,   407,    -1,
     409,    -1,   411,    -1,    -1,   163,   415,    -1,   417,    -1,
     419,    -1,    -1,   422,    -1,   345,    -1,    -1,    -1,    61,
      62,    63,   302,   303,    -1,   163,    68,    -1,    70,    -1,
      -1,    -1,    74,   442,    -1,    77,    78,    79,   447,    -1,
      -1,    -1,   451,   452,    -1,    -1,    -1,   327,    -1,   458,
      -1,    -1,   332,   462,   475,   407,    -1,    99,   407,   101,
      -1,     9,    -1,    -1,   106,    -1,    -1,    -1,   477,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   493,    -1,    -1,   496,    -1,    -1,
     499,    -1,   501,    -1,   503,    -1,   505,    -1,    -1,   508,
      -1,   431,    -1,   433,   525,   526,   515,    -1,   529,   439,
     519,   441,    -1,    -1,    -1,    -1,   537,    -1,   398,    67,
      -1,    -1,    -1,    71,   404,    -1,    -1,   214,   215,   216,
     217,   218,    -1,   220,   221,    -1,   223,   224,    -1,    -1,
      -1,   550,    -1,   552,    -1,    -1,    -1,    95,    96,    97,
      -1,     9,    -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,
      -1,   582,   110,    -1,    -1,    -1,    24,    -1,    -1,   590,
      28,   592,    -1,    -1,    -1,    -1,    -1,    -1,   458,    -1,
      -1,    -1,   364,   365,   593,    -1,    -1,    -1,    -1,    -1,
      -1,   612,    -1,   614,   474,    -1,    -1,    -1,    56,    -1,
      58,   481,   623,    -1,    -1,    -1,   364,   365,   629,    67,
      -1,    -1,    -1,    71,    -1,    73,   625,    -1,    -1,    -1,
      -1,    -1,    80,    -1,    -1,   634,   364,   365,    86,    -1,
      -1,    -1,   653,   642,   655,    -1,   645,    95,    96,    97,
      -1,   521,   522,   652,   524,   525,    -1,    -1,   657,     9,
      -1,    -1,   110,   662,    -1,   113,   665,    -1,   345,    -1,
     442,    -1,    -1,    -1,    24,   447,    -1,    -1,    28,    -1,
      -1,   551,   552,   360,   683,    -1,    -1,    -1,    -1,   559,
      -1,    -1,    42,    -1,   442,    -1,    -1,   567,    -1,   447,
      -1,    -1,   701,   573,   703,    -1,    56,    -1,    58,    -1,
      -1,    -1,    -1,    -1,   442,   657,    -1,    67,   657,   447,
      -1,    71,    -1,    73,    -1,    -1,    -1,    -1,    -1,   728,
      -1,    -1,    -1,    -1,    -1,   605,    86,    -1,    -1,   609,
      -1,    -1,    -1,    -1,    -1,    95,    96,    97,    -1,    -1,
      -1,   621,    -1,    -1,   431,    -1,   433,   434,   435,    -1,
     110,   438,   439,   113,    -1,    -1,    -1,    -1,   638,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   659,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   675,    -1,    -1,    -1,    -1,
      -1,    -1,     7,    -1,     9,    -1,    11,    12,    13,    -1,
      -1,    16,    -1,    18,    19,    -1,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    28,    -1,    -1,    -1,   707,    33,    34,
      -1,   711,    37,   713,    -1,    -1,   716,    -1,    -1,    44,
      45,    46,    -1,    -1,    -1,    -1,    51,   727,   728,    -1,
      -1,    56,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    66,    67,    -1,    -1,    -1,    71,    72,    73,    -1,
      -1,    76,    -1,    -1,    79,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    87,    -1,    -1,    90,    -1,    -1,    -1,    -1,
      95,    96,    97,    -1,    99,   100,    -1,   102,    -1,    -1,
      -1,   106,    -1,    -1,    -1,   110,    -1,    -1,   113,   114,
       7,    -1,     9,    -1,    11,    12,    13,    -1,    -1,    16,
      -1,    18,    19,    -1,    -1,    -1,    -1,    24,    -1,    -1,
      -1,    28,    -1,    -1,    -1,    -1,    33,    34,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,
      -1,    58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      67,    -1,    -1,    -1,    71,    72,    73,    -1,    -1,    76,
      -1,    -1,    79,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      87,    -1,    -1,    90,    -1,    -1,    -1,    -1,    95,    96,
      97,    -1,    99,   100,    -1,   102,   103,    -1,    -1,   106,
      -1,    -1,    -1,   110,    -1,    -1,   113,   114,     7,    -1,
       9,    -1,    11,    12,    13,    -1,    -1,    16,    -1,    18,
      19,    -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    28,
      -1,    -1,    -1,    -1,    33,    34,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    44,    45,    46,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,    -1,    58,
      59,    -1,    -1,    -1,    -1,    -1,    -1,    66,    67,    -1,
      -1,    -1,    71,    72,    73,    -1,    -1,    76,    -1,    -1,
      79,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    -1,
      -1,    90,    -1,    -1,    -1,    -1,    95,    96,    97,    -1,
      99,   100,    -1,   102,   103,    -1,    -1,   106,    -1,    -1,
      -1,   110,    -1,    -1,   113,   114,     7,    -1,     9,    -1,
      11,    12,    13,    -1,    -1,    16,    -1,    18,    19,    -1,
      -1,    -1,    -1,    24,    -1,    -1,    -1,    28,    -1,    -1,
      -1,    -1,    33,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    44,    45,    46,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    56,    -1,    58,    59,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    67,    -1,    -1,    -1,
      71,    72,    73,    -1,    -1,    76,    -1,    -1,    79,    80,
      -1,    -1,    -1,    -1,    -1,    86,    87,    -1,    -1,    90,
      -1,    -1,    -1,    -1,    95,    96,    97,    -1,    99,   100,
      -1,   102,    -1,    -1,    -1,   106,    -1,    -1,    -1,   110,
      -1,    -1,   113,   114,     7,    -1,     9,    -1,    11,    12,
      13,    -1,    -1,    16,    -1,    18,    19,    -1,    -1,    -1,
      -1,    24,    -1,    -1,    -1,    28,    -1,    -1,    -1,    -1,
      33,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    56,    -1,    58,    59,    -1,    -1,    -1,
      -1,    -1,    -1,    66,    67,    -1,    -1,    -1,    71,    72,
      73,    -1,    -1,    76,    -1,    -1,    79,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    87,    -1,    -1,    90,    -1,    -1,
      -1,    -1,    95,    96,    97,    -1,    99,   100,    -1,   102,
      -1,    -1,    -1,   106,    -1,    -1,    -1,   110,    -1,    -1,
     113,   114,     7,    -1,     9,    -1,    11,    12,    13,    -1,
      -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    28,    -1,    -1,    -1,    -1,    33,    34,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    56,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    67,    -1,    -1,    -1,    71,    72,    73,    -1,
      -1,    76,    -1,    -1,    79,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    87,    -1,    -1,    90,    -1,    -1,    -1,    -1,
      95,    96,    97,    -1,    99,   100,    -1,   102,    -1,    -1,
      -1,   106,    -1,    -1,    -1,   110,    -1,    -1,   113,   114,
       7,    -1,     9,    -1,    11,    12,    13,    -1,    -1,    -1,
      -1,    18,    19,    -1,    -1,    -1,    -1,    24,    -1,    -1,
      -1,    28,    -1,    -1,    -1,    -1,    33,    34,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,
      -1,    58,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    -1,    -1,    -1,    71,    72,    73,    24,    -1,    76,
      -1,    28,    79,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    96,
      97,    -1,    -1,   100,    -1,   102,    -1,    -1,    -1,    56,
      -1,    58,    -1,   110,    -1,    -1,   113,   114,     8,     9,
      67,    11,    12,    13,    71,    15,    73,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    28,    86,
      -1,    -1,    -1,    33,    34,    -1,    -1,    -1,    95,    96,
      97,    -1,    -1,    -1,    44,    45,    46,    -1,    -1,    -1,
      -1,    -1,    -1,   110,    -1,    55,    56,    -1,    58,    -1,
      -1,    61,    62,    63,    -1,    -1,    -1,    67,    68,    -1,
      70,    71,    -1,    73,    74,    -1,    76,    77,    78,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    95,    96,    97,    -1,    -1,
     100,   101,   102,   103,    -1,    -1,    -1,    -1,    -1,    -1,
     110,    -1,    -1,   113,   114,     8,     9,    -1,    11,    12,
      13,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,
      -1,    24,    -1,    -1,    -1,    28,    -1,    -1,    -1,    -1,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    56,    -1,    58,    -1,    -1,    61,    62,
      63,    -1,    -1,    -1,    67,    68,    -1,    70,    71,    -1,
      73,    74,    -1,    76,    77,    78,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    87,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    95,    96,    97,    -1,    -1,   100,   101,   102,
     103,    -1,    -1,    -1,    -1,    -1,    -1,   110,    -1,    -1,
     113,   114,     8,     9,    -1,    11,    12,    13,    -1,    15,
      -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,    24,    -1,
      -1,    -1,    28,    -1,    -1,    -1,    -1,    33,    34,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      56,    -1,    58,    -1,    -1,    61,    62,    63,    -1,    -1,
      -1,    67,    68,    -1,    70,    71,    -1,    73,    74,    -1,
      76,    77,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      96,    97,    -1,    -1,   100,   101,   102,   103,    -1,    -1,
      -1,    -1,    -1,    -1,   110,    -1,    -1,   113,   114,     8,
       9,    -1,    11,    12,    13,    -1,    -1,    -1,    -1,    18,
      -1,    -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    28,
      -1,    -1,    -1,    -1,    33,    34,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    44,    45,    46,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    58,
      -1,    -1,    61,    62,    63,    -1,    -1,    -1,    67,    68,
      -1,    70,    71,    -1,    73,    74,    -1,    76,    77,    78,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,    -1,
      -1,   100,   101,   102,   103,    -1,     9,    -1,    11,    12,
      -1,   110,    15,    -1,   113,   114,    -1,    -1,    -1,    -1,
      -1,    24,    -1,    -1,    -1,    28,    -1,    -1,    -1,    -1,
      33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    44,    45,    46,    -1,    -1,    -1,    50,    -1,    -1,
      -1,    -1,    -1,    56,    -1,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    -1,    -1,    -1,    71,    72,
      73,    -1,    -1,    76,    -1,    -1,    79,    80,    -1,    -1,
      -1,    -1,    -1,    86,    87,     9,    -1,    11,    12,    -1,
      -1,    -1,    95,    96,    97,    -1,    -1,   100,    -1,   102,
      24,    -1,    -1,    -1,    28,    -1,    -1,   110,    -1,    33,
     113,   114,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      44,    45,    46,    -1,    -1,    -1,    50,    -1,    -1,    -1,
      -1,    -1,    56,    -1,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    -1,    -1,    -1,    71,    72,    73,
      -1,    -1,    76,    -1,    -1,    79,    80,    -1,    -1,    -1,
      -1,    -1,    86,    87,     9,    -1,    11,    12,    -1,    -1,
      -1,    95,    96,    97,    -1,    -1,   100,    -1,   102,    24,
      -1,    -1,    -1,    28,    -1,    -1,   110,    -1,    33,   113,
     114,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    -1,    -1,    -1,    50,    -1,    -1,    -1,    -1,
      -1,    56,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    67,    -1,    -1,    -1,    71,    72,    73,    -1,
      -1,    76,    -1,    -1,    79,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    87,     9,    -1,    11,    12,    -1,    -1,    -1,
      95,    96,    97,    -1,    -1,   100,    -1,   102,    24,    -1,
      -1,    -1,    28,    -1,    -1,   110,    -1,    33,   113,   114,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      56,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    67,    -1,    -1,    -1,    71,    72,    73,    -1,    -1,
      76,    -1,    -1,    79,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    -1,    -1,     9,    -1,    -1,    -1,    -1,    95,
      96,    97,    -1,    -1,   100,    20,   102,    -1,    -1,    24,
      -1,    -1,    -1,    28,   110,    -1,    31,   113,   114,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    53,    -1,
      -1,    56,     9,    58,    -1,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    67,    20,    -1,    -1,    71,    24,    73,    -1,
      -1,    28,    -1,    -1,    31,    -1,    81,    -1,    -1,    84,
      -1,    86,    -1,    88,    -1,    -1,    -1,    -1,    -1,    -1,
      95,    96,    97,    -1,    -1,    52,    53,    -1,    -1,    56,
       9,    58,    -1,    60,    -1,   110,    -1,    -1,   113,    -1,
      67,    20,    -1,    -1,    71,    24,    73,    -1,    -1,    28,
      -1,    -1,    31,    80,    -1,    -1,    -1,    84,    -1,    86,
      -1,    88,    -1,    -1,    -1,    -1,    -1,    -1,    95,    96,
      97,    -1,    -1,    52,    53,    -1,    -1,    56,     9,    58,
      -1,    60,    -1,   110,    -1,    -1,   113,    -1,    67,    20,
      -1,    -1,    71,    24,    73,    -1,    -1,    28,    -1,    -1,
      31,    80,    -1,    -1,    -1,    84,    -1,    86,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,    -1,
      -1,    52,    53,    -1,    -1,    56,     9,    58,    -1,    60,
      -1,   110,    15,    -1,   113,    -1,    67,    20,    -1,    -1,
      71,    24,    73,    -1,    -1,    28,    -1,    -1,    31,    -1,
      -1,    -1,    -1,    84,    -1,    86,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    96,    97,    -1,    -1,    52,
      53,    -1,    -1,    56,     9,    58,    -1,    60,   109,   110,
      -1,    -1,   113,    -1,    67,    20,    -1,    -1,    71,    24,
      73,    -1,    -1,    28,    -1,    -1,    31,    -1,    -1,    -1,
      -1,    84,    -1,    86,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    95,    96,    97,    -1,    -1,    52,    53,    -1,
      -1,    56,     9,    58,    -1,    60,    -1,   110,    -1,    -1,
     113,    -1,    67,    20,    -1,    -1,    71,    24,    73,    -1,
      -1,    28,    -1,    -1,    31,    80,    -1,    -1,    -1,    84,
      -1,    86,    -1,     9,    -1,    -1,    -1,    -1,    -1,    15,
      95,    96,    97,    -1,    -1,    52,    53,    -1,    24,    56,
      -1,    58,    28,    60,    -1,   110,    -1,    -1,   113,    -1,
      67,    -1,    -1,    -1,    71,    -1,    73,    -1,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    86,
      56,    -1,    58,    24,    -1,    -1,    -1,    28,    95,    96,
      97,    67,    -1,    -1,    -1,    71,    -1,    73,    -1,     9,
      -1,    -1,    -1,   110,    -1,    -1,   113,    -1,    -1,    -1,
      86,    -1,    -1,    -1,    24,    56,    -1,    58,    28,    95,
      96,    97,    -1,    -1,    -1,    -1,    67,    -1,    -1,    -1,
      71,    -1,    73,    -1,   110,    -1,    -1,   113,    -1,    80,
      -1,    -1,    -1,     9,    -1,    86,    56,    -1,    58,    -1,
      -1,    -1,    -1,    -1,    95,    96,    97,    67,    24,    -1,
      -1,    71,    28,    73,     9,    -1,    -1,    -1,    -1,   110,
      80,    -1,   113,    -1,    -1,    -1,    86,    -1,    -1,    24,
      -1,    -1,    -1,    28,    -1,    95,    96,    97,    -1,    -1,
      56,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     110,    67,    -1,   113,    -1,    71,    -1,    73,    -1,    -1,
      -1,    56,    -1,    58,    80,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    67,    -1,    -1,    -1,    71,    -1,    73,    95,
      96,    97,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,   110,    -1,    -1,   113,    -1,    -1,
      95,    96,    97,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   110,    -1,    -1,   113
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short yystos[] =
{
       0,     9,    24,    28,    56,    58,    67,    71,    73,    86,
      95,    96,    97,   110,   113,   119,   120,   121,   122,   189,
     232,   233,   236,   237,   238,   239,   240,   241,   242,   245,
     246,   247,   248,   249,     0,   121,     5,    42,   123,   139,
     189,   232,   233,   123,    21,    20,    31,    52,    53,    60,
      84,   140,   151,   219,   232,   233,   235,    26,    26,    36,
     127,   128,   189,    80,   151,    57,    57,    57,     8,    11,
      12,    13,    18,    33,    34,    44,    45,    46,    55,    61,
      62,    63,    68,    70,    74,    76,    77,    78,    87,   100,
     101,   102,   103,   114,   129,   132,   134,   135,   145,   189,
     232,   233,   243,   251,   252,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   265,    39,    49,   124,   189,   235,
     235,   235,    91,    15,   134,   125,   126,   131,   132,   133,
     189,   130,   135,   136,   137,   138,   152,   154,   155,   159,
     160,   161,   162,   164,   171,   172,   173,   174,   178,   179,
     181,   182,   186,   232,   233,    92,    92,    92,   134,    91,
     133,   189,    35,    15,   137,     5,    54,    54,    22,   180,
     189,    43,     5,     8,    61,    62,    63,    68,    70,    74,
      77,    78,    79,    99,   101,   106,     5,    61,    63,    74,
      79,   101,   232,   233,     7,    16,    19,    59,    66,    72,
      79,    90,    99,   103,   106,   143,   145,   146,   177,   190,
     232,   233,   243,   244,   250,   251,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   266,   269,   272,
     222,   236,   237,   239,   241,   242,   222,    81,   231,   108,
     224,   223,   231,   143,    82,    43,   224,    75,   163,   189,
      69,   104,   217,   224,    75,    30,    38,   103,   143,   224,
     217,    30,    38,    27,    42,   107,     6,    83,   115,   189,
     270,   107,    30,    89,    41,    42,    57,   227,   228,    42,
      42,    42,   227,   228,    57,   227,   227,   227,   227,   227,
     227,    57,   228,   227,    42,   227,   228,    42,   228,    37,
      51,   143,   224,   224,   225,   245,   246,   247,   248,   249,
      23,   192,    85,   231,   223,   192,    42,   224,    42,   142,
     143,   116,   167,   189,   192,    42,     5,   139,   180,   192,
     167,     5,   139,    17,   143,   232,   267,   268,   144,   232,
     233,   235,   271,   232,   233,   233,   219,   220,   221,   232,
     233,   235,   109,   187,   188,   229,   230,   232,   233,   235,
     264,   220,   148,   149,   232,   233,   220,   220,   220,   143,
     143,   189,   191,   192,   191,   231,    94,   189,   226,   231,
     224,    82,   226,   214,   215,   216,   232,   233,   192,   214,
     231,    10,    54,   165,   226,   214,   139,   112,   153,   189,
     224,   226,   165,   139,   153,   232,   143,    15,    80,    41,
      98,    42,   227,   228,    80,    15,   219,    40,    92,     3,
     227,    80,    15,    80,   149,    90,   145,   150,   177,   190,
     232,   233,   243,   250,   251,   254,   255,   256,   257,   261,
     262,   263,   269,   272,   150,   177,   190,   269,    80,    80,
      80,    42,    42,   231,   217,    14,   192,   231,    64,   156,
     157,    80,    15,   216,   226,    80,   166,   236,   237,   238,
     239,   241,   242,   222,   224,     5,    80,    42,   191,   192,
     156,   224,   191,    80,   268,   233,   220,   219,   188,   187,
      92,    80,   149,    42,   227,   228,    42,   227,   228,    42,
     227,    42,   227,    42,   227,    42,   227,   228,    42,   228,
     150,   150,   164,   171,   174,   175,   176,   232,   233,   176,
     231,   226,   192,   189,   207,   233,     5,   157,   216,     5,
     224,   191,   139,   224,   214,   226,   226,     5,   191,   226,
      80,   220,   220,   220,   220,   220,   220,   220,    80,   176,
     175,   192,   105,   189,   209,   210,   211,   209,    55,   158,
     189,   139,   189,   208,   139,   139,   192,   226,   192,    80,
       5,   217,   139,   226,     5,    80,    80,    80,    80,    80,
      80,    80,     5,    80,    93,   183,   184,   185,   189,   207,
       5,   211,     5,    42,    32,    79,   189,   193,   194,   195,
     196,   197,   226,     4,    47,   168,   189,   226,   235,   168,
     235,   139,     5,   223,     5,   185,    48,   139,   139,   214,
     232,   232,   195,     5,    42,    42,    25,   189,   218,     5,
     218,   139,   192,   139,    42,    80,   192,   189,   205,   217,
     139,   232,    50,   141,   145,   169,   170,   232,   233,   250,
     261,   263,    42,     5,   139,     5,   214,   117,   189,   206,
      80,   141,    15,    80,   170,    42,    88,   147,   231,   232,
     233,   234,   235,   139,   139,    80,   142,    65,   189,   198,
      80,   170,    80,   151,   219,   232,   234,   235,    80,   111,
     189,   202,   212,   213,   199,   236,   237,   239,   241,   242,
     192,   140,    80,    15,    80,    80,    80,   232,   202,    80,
     214,   205,    80,   206,    10,   189,   203,   204,   236,   237,
     238,   239,   240,   241,   242,    29,   189,   200,    42,   218,
     189,   201,   214,   192,    80
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 702 "parser.y"
    { root = yyvsp[0].tree_ptr; }
    break;

  case 4:
#line 710 "parser.y"
    {
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 5:
#line 713 "parser.y"
    {
            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 6:
#line 722 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = DEFINITIONS;
            tree_node->child[child_identity_0] = yyvsp[-4].tree_ptr;
            tree_node->child[child_misc1___14] = yyvsp[-1].tree_ptr;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 7:
#line 736 "parser.y"
    {

            /* reset flags for each module/mib */
            SMIv1Flag   = 0;
            SMIv2Flag   = 0;
            SMIv3Flag   = 0;
            sawObjectIdentifier = 0;

            /* set module name */
            lastModuleName = yyvsp[-2].tree_ptr->string;
    }
    break;

  case 8:
#line 749 "parser.y"
    {

            /* reset flags for each module/mib */
            SMIv1Flag   = 0;
            SMIv2Flag   = 0;
            SMIv3Flag   = 0;

            /* set module name */
            lastModuleName = yyvsp[-2].tree_ptr->string;
            print_warning("*** ERROR ***  MIB names must begin with an uppercase letter.", lineBuf);
    }
    break;

  case 11:
#line 768 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = yyvsp[-1].tree_ptr;
            yyval.tree_ptr = tree_node; 
    }
    break;

  case 13:
#line 778 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = yyvsp[0].tree_ptr;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 15:
#line 788 "parser.y"
    {
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 16:
#line 791 "parser.y"
    {
            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 17:
#line 799 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = yyvsp[-1].tree_ptr;
            tree_node->child[child_misc1___14] = yyvsp[0].tree_ptr;
            yyval.tree_ptr = tree_node; 
    }
    break;

  case 18:
#line 806 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = yyvsp[0].tree_ptr;
            yyval.tree_ptr = tree_node; 
    }
    break;

  case 19:
#line 816 "parser.y"
    {
            yyval.tree_ptr = yyvsp[-1].tree_ptr;
    }
    break;

  case 22:
#line 824 "parser.y"
    {
            print_warning("*** ERROR ***  There is an extra COMMA at the end of the Symbol List.", lineBuf);
    }
    break;

  case 24:
#line 831 "parser.y"
    {
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 25:
#line 834 "parser.y"
    {
            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 27:
#line 842 "parser.y"
    {
            print_warning("*** ERROR ***  There is an extra COMMA at the end of the Symbol List.", lineBuf);

    }
    break;

  case 29:
#line 850 "parser.y"
    {
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 30:
#line 853 "parser.y"
    {
            yyvsp[-2].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-2].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 31:
#line 857 "parser.y"
    {
            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
            print_warning("*** ERROR ***  There is a missing COMMA in the Symbol List.", lineBuf);
    }
    break;

  case 32:
#line 865 "parser.y"
    {
            tree * importFromNode = NULL;
            tree * importItem     = NULL;
            tree * treePtr        = NULL;
            tree * tree_node      = NULL;

            tree_node = malloc_node(tree_node, SINGLE_PARSE);

	    /*BAB*/
	    tree_node->type = IMPORTS;
	    tree_node->string = "imports";
	    /*BAB*/

            tree_node->child[child_identity_0] = yyvsp[-2].tree_ptr;
            tree_node->child[child_misc1___14] = yyvsp[0].tree_ptr;

#ifdef NOT_YET
	    /*BAB*/
            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }
	    /*BAB*/
#endif /* NOT_YET */

            yyval.tree_ptr = tree_node; 

            /* create and insert FROM node at beginning of global import list */
            importFromNode = malloc_node(importFromNode, 1);
            importFromNode->string = strdup(yyvsp[0].tree_ptr->string);
            importFromNode->freeMe = 1;

            treePtr = yyvsp[-2].tree_ptr;
            while(treePtr) {

                /* -1 is ignore keyword type */
                if((treePtr->string) && (treePtr->type != -1)){

                    /* import FROM nodes are linked using the next_tree_node              */
                    /* items are hung from the FROM nodes using the next_symbol_node link */

                    /* create and insert item into FROM node's list of importItems */
                    importItem = malloc_node(importItem, 1);
                    importItem->string = strdup(treePtr->string); 
                    importItem->freeMe = 1;

                    importItem->next_symbol_node = importFromNode->next_symbol_node;
                    importFromNode->next_symbol_node = importItem;

                } 

                treePtr = treePtr->next_tree_node;
            }

            /* don't import the file unless there is something to import */
            if(importFromNode->next_symbol_node) {

                /* insert node at end of list */
                if(importFromList == NULL) {
                    importFromList = importFromNode;
                    importFromList->last_tree_node_in_list = importFromNode;
                } else {
                    importFromList->last_tree_node_in_list->next_tree_node = importFromNode;
                    importFromList->last_tree_node_in_list = importFromNode;
                }

            }

    }
    break;

  case 33:
#line 936 "parser.y"
    {
            tree * importFromNode = NULL;
            tree * importItem     = NULL;
            tree * treePtr        = NULL;
            tree * tree_node      = NULL;

            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = yyvsp[-2].tree_ptr;
            tree_node->child[child_misc1___14] = yyvsp[0].tree_ptr;
            yyval.tree_ptr = tree_node; 

            /* create and insert FROM node at beginning of global import list */
            importFromNode = malloc_node(importFromNode, 1);
            importFromNode->string = strdup(yyvsp[0].tree_ptr->string);
            importFromNode->freeMe = 1;

            print_warning("*** ERROR ***  The names of imported MIBs must be UppercaseIdentifiers.", lineBuf);

            treePtr = yyvsp[-2].tree_ptr;
            while(treePtr) {

                /* -1 is ignore keyword type */
                if((treePtr->string) && (treePtr->type != -1)){

                    /* import FROM nodes are linked using the next_tree_node              */
                    /* items are hung from the FROM nodes using the next_symbol_node link */

                    /* create and insert item into FROM node's list of importItems */
                    importItem = malloc_node(importItem, 1);
                    importItem->string = strdup(treePtr->string); 
                    importItem->freeMe = 1;

                    importItem->next_symbol_node = importFromNode->next_symbol_node;
                    importFromNode->next_symbol_node = importItem;

                } 

                treePtr = treePtr->next_tree_node;
            }

            /* don't import the file unless there is something to import */
            if(importFromNode->next_symbol_node) {

                /* insert node at end of list */
                if(importFromList == NULL) {
                    importFromList = importFromNode;
                    importFromList->last_tree_node_in_list = importFromNode;
                } else {
                    importFromList->last_tree_node_in_list->next_tree_node = importFromNode;
                    importFromList->last_tree_node_in_list = importFromNode;
                }

            }
    }
    break;

  case 37:
#line 1000 "parser.y"
    {
            yyvsp[0].tree_ptr->type = -1;
    }
    break;

  case 38:
#line 1004 "parser.y"
    {
            yyvsp[0].tree_ptr->type = -1;
    }
    break;

  case 39:
#line 1007 "parser.y"
    {
            yyvsp[0].tree_ptr->type = -1;
    }
    break;

  case 40:
#line 1010 "parser.y"
    {
            yyvsp[0].tree_ptr->type = -1;
    }
    break;

  case 41:
#line 1013 "parser.y"
    {
            yyvsp[0].tree_ptr->type = -1;
    }
    break;

  case 42:
#line 1016 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 43:
#line 1022 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 44:
#line 1028 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 45:
#line 1034 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 46:
#line 1040 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 47:
#line 1046 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 48:
#line 1052 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            yyval.tree_ptr = tree_node; 
    }
    break;

  case 49:
#line 1058 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 50:
#line 1064 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 51:
#line 1070 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 52:
#line 1076 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 53:
#line 1082 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 54:
#line 1091 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = yyvsp[-2].tree_ptr;
            tree_node->child[child_syntax___1] = yyvsp[0].tree_ptr;
	    if(tree_node->child[child_identity_0]) {
  	      if(garbageCheck(tree_node->child[child_identity_0]->
				string)) {
		/*Garbage. Ignore*/
		tree_node->type = -1;
	      }
   	      else {
                tree_node->type                    = TEXTUAL_CONVENTION;
                tree_node->string = strdup("TYPE_ASSIGNMENT");  
	      }
	    }

            /* if saving for all parses, you must clone, otherwise
               the children would go bye-bye at the end of the parse */

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            yyval.tree_ptr = tree_node;
    }
    break;

  case 55:
#line 1120 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = yyvsp[-2].tree_ptr;
            tree_node->child[child_syntax___1] = yyvsp[0].tree_ptr;

            if(tree_node->child[child_identity_0]) {
              if(garbageCheck(tree_node->child[child_identity_0]->
                                string)) {
                /*Garbage. Ignore*/
                tree_node->type = -1;
              }
              else {
                tree_node->type                    = TEXTUAL_CONVENTION;
                tree_node->string = strdup("TYPE_ASSIGNMENT");  
              }
            }

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            yyval.tree_ptr = tree_node;

            print_warning("*** ERROR ***  You cannot use LowercaseIdentifiers in type assignments.", lineBuf);

    }
    break;

  case 56:
#line 1151 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            yyval.tree_ptr = tree_node;

            print_note("*** NOTE ***  You cannot redefine basic types.", lineBuf);

    }
    break;

  case 72:
#line 1190 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = IDENTIFIER;
            tree_node->string                  = "object-id";
            tree_node->child[child_identity_0] = yyvsp[-4].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[0].tree_ptr;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            yyval.tree_ptr = tree_node;

            sawObjectIdentifier = 1;

    }
    break;

  case 73:
#line 1212 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = IDENTIFIER;
            tree_node->string                  = "object-id";
            tree_node->child[child_identity_0] = yyvsp[-4].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[0].tree_ptr;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            yyval.tree_ptr = tree_node;

            sawObjectIdentifier = 1;

            print_warning("*** ERROR ***  Object identifiers must begin with a lowercase letter.", lineBuf);

    }
    break;

  case 75:
#line 1238 "parser.y"
    {
        print_warning("*** ERROR ***  Object identifiers must begin with a lowercase letter.", lineBuf);
    }
    break;

  case 76:
#line 1241 "parser.y"
    {
            yyval.tree_ptr = yyvsp[-1].tree_ptr;
    }
    break;

  case 77:
#line 1247 "parser.y"
    {
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 78:
#line 1250 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
	    tree_node->child[0] = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 87:
#line 1276 "parser.y"
    { 
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr; 
            fill_in_max_min_values(yyvsp[0].tree_ptr, INTEGER);
            check_range_limits(yyvsp[0].tree_ptr, INTEGER);
            check_for_range_overlap(yyvsp[0].tree_ptr);
    }
    break;

  case 88:
#line 1282 "parser.y"
    { 
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
            fill_in_max_min_values(yyvsp[0].tree_ptr, Unsigned32);
            check_range_limits(yyvsp[0].tree_ptr, Unsigned32);
            check_for_range_overlap(yyvsp[0].tree_ptr);
    }
    break;

  case 89:
#line 1288 "parser.y"
    { 
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr; 
            fill_in_max_min_values(yyvsp[0].tree_ptr, Unsigned32);
            check_range_limits(yyvsp[0].tree_ptr, Unsigned32);
            check_for_range_overlap(yyvsp[0].tree_ptr);
    }
    break;

  case 90:
#line 1294 "parser.y"
    { yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr; }
    break;

  case 93:
#line 1297 "parser.y"
    {
            yyvsp[-3].tree_ptr->child[child_identity_0] = yyvsp[-1].tree_ptr;
    }
    break;

  case 95:
#line 1305 "parser.y"
    { 
            print_warning("*** ERROR ***  Per rfc2578, TimeTicks must not have range restriction.", lineBuf);
    }
    break;

  case 96:
#line 1308 "parser.y"
    { 
            print_warning("*** ERROR ***  Per rfc2578, Counter64 must not have range restriction.", lineBuf);
    }
    break;

  case 97:
#line 1311 "parser.y"
    { 
            print_warning("*** ERROR ***  Per rfc2578, Counter32 must not have range restriction.", lineBuf);
    }
    break;

  case 98:
#line 1314 "parser.y"
    {
            yyvsp[-4].tree_ptr->child[child_identity_0] = yyvsp[-2].tree_ptr;
            print_warning("*** ERROR ***  There is an extra COMMA at the end of the Sequence Item List.", lineBuf);
    }
    break;

  case 112:
#line 1340 "parser.y"
    {
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
    }
    break;

  case 114:
#line 1345 "parser.y"
    {
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
            fill_in_max_min_values(yyvsp[0].tree_ptr, INTEGER);
            check_range_limits(yyvsp[0].tree_ptr, INTEGER);
            check_for_range_overlap(yyvsp[0].tree_ptr);
    }
    break;

  case 115:
#line 1351 "parser.y"
    {
            yyvsp[-3].tree_ptr->type                    = BITS;
            yyvsp[-3].tree_ptr->enumeratedItems         = yyvsp[-1].tree_ptr->enumeratedItems;
            yyvsp[-1].tree_ptr->enumeratedItems         = NULL;
            yyvsp[-3].tree_ptr->child[child_status___2] = yyvsp[-1].tree_ptr;
    }
    break;

  case 116:
#line 1358 "parser.y"
    {
            print_note("*** NOTE ***  OCTET STRING on previous line should have SIZE specification.", lineBuf);
    }
    break;

  case 117:
#line 1361 "parser.y"
    {
		yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
    }
    break;

  case 118:
#line 1364 "parser.y"
    {
            yyvsp[-3].tree_ptr->enumeratedItems         = yyvsp[-1].tree_ptr->enumeratedItems;
            yyvsp[-1].tree_ptr->enumeratedItems         = NULL;
            yyvsp[-3].tree_ptr->child[child_status___2] = yyvsp[-1].tree_ptr;
    }
    break;

  case 120:
#line 1371 "parser.y"
    {
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
            fill_in_max_min_values(yyvsp[0].tree_ptr, Unsigned32);
            check_range_limits(yyvsp[0].tree_ptr, Unsigned32);
            check_for_range_overlap(yyvsp[0].tree_ptr);
    }
    break;

  case 123:
#line 1382 "parser.y"
    {
            yyvsp[-3].tree_ptr->enumeratedItems         = yyvsp[-1].tree_ptr->enumeratedItems;
            yyvsp[-1].tree_ptr->enumeratedItems         = NULL;
            yyvsp[-3].tree_ptr->child[child_status___2] = yyvsp[-1].tree_ptr;
    }
    break;

  case 124:
#line 1388 "parser.y"
    { 
            yyvsp[-3].tree_ptr->enumeratedItems         = yyvsp[-1].tree_ptr->enumeratedItems;
            yyvsp[-1].tree_ptr->enumeratedItems         = NULL;
            yyvsp[-3].tree_ptr->child[child_status___2] = yyvsp[-1].tree_ptr;
    }
    break;

  case 126:
#line 1395 "parser.y"
    {
            print_warning("*** ERROR ***  UppercaseIdentifiers must be used for types, row names, or imported types, NOT LowercaseIdentifiers.", lineBuf);
    }
    break;

  case 127:
#line 1398 "parser.y"
    {
            yyvsp[-3].tree_ptr->type                    = BITS;
            yyvsp[-3].tree_ptr->enumeratedItems         = yyvsp[-1].tree_ptr->enumeratedItems;
            yyvsp[-1].tree_ptr->enumeratedItems         = NULL;
            yyvsp[-3].tree_ptr->child[child_status___2] = yyvsp[-1].tree_ptr;
    }
    break;

  case 128:
#line 1405 "parser.y"
    { yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr; }
    break;

  case 129:
#line 1406 "parser.y"
    { yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr; }
    break;

  case 130:
#line 1407 "parser.y"
    {

            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->enumeratedItems         = yyvsp[-1].tree_ptr->enumeratedItems;
            yyvsp[-1].tree_ptr->enumeratedItems                = NULL;
            tree_node->type                    = BITS;
            tree_node->child[child_identity_0] = yyvsp[-5].tree_ptr;
            tree_node->child[child_misc1___14] = yyvsp[-3].tree_ptr;
            tree_node->child[child_status___2] = yyvsp[-1].tree_ptr;
            yyval.tree_ptr = tree_node; 
    }
    break;

  case 131:
#line 1419 "parser.y"
    {

            yyvsp[-3].tree_ptr->child[child_identity_0] = yyvsp[-1].tree_ptr;
            yyvsp[-3].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
            yyvsp[-3].tree_ptr->type = DOT;
    }
    break;

  case 132:
#line 1425 "parser.y"
    {

            yyvsp[-3].tree_ptr->child[child_identity_0] = yyvsp[-1].tree_ptr;
            yyvsp[-3].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
            yyvsp[-3].tree_ptr->type = DOT;
    }
    break;

  case 133:
#line 1432 "parser.y"
    { /* rfc1065, rfc1155 */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            yyval.tree_ptr = tree_node; 
    }
    break;

  case 134:
#line 1440 "parser.y"
    { 
            /* this isn't really legal, should be range restriction */
            print_warning("*** ERROR ***  This should be a range restriction, NOT a size restriction.", lineBuf);
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
    }
    break;

  case 135:
#line 1445 "parser.y"
    { 
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr; 
            print_note("*** NOTE ***  OCTET STRING should have SIZE specification.", lineBuf);
    }
    break;

  case 142:
#line 1459 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "";
            yyval.tree_ptr = tree_node;
    }
    break;

  case 143:
#line 1466 "parser.y"
    { yyval.tree_ptr = yyvsp[-1].tree_ptr; }
    break;

  case 144:
#line 1467 "parser.y"
    { yyval.tree_ptr = yyvsp[-1].tree_ptr; }
    break;

  case 145:
#line 1468 "parser.y"
    { yyval.tree_ptr = yyvsp[-1].tree_ptr; }
    break;

  case 146:
#line 1469 "parser.y"
    { yyval.tree_ptr = yyvsp[-1].tree_ptr; }
    break;

  case 147:
#line 1471 "parser.y"
    {
            yyvsp[-3].tree_ptr->last_tree_node_in_list = yyvsp[-1].tree_ptr->last_tree_node_in_list;
            yyvsp[-3].tree_ptr->next_tree_node = yyvsp[-1].tree_ptr;
            yyval.tree_ptr = yyvsp[-3].tree_ptr;
    }
    break;

  case 148:
#line 1477 "parser.y"
    {
            yyvsp[-2].tree_ptr->last_tree_node_in_list = yyvsp[-1].tree_ptr->last_tree_node_in_list;
            yyvsp[-2].tree_ptr->next_tree_node = yyvsp[-1].tree_ptr;
            yyval.tree_ptr = yyvsp[-2].tree_ptr;
    }
    break;

  case 149:
#line 1485 "parser.y"
    {
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr; 
    }
    break;

  case 150:
#line 1488 "parser.y"
    {
            yyvsp[-2].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-2].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 151:
#line 1492 "parser.y"
    {
            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
            print_warning("*** ERROR ***  Missing COMMA in Sequence Item List.", lineBuf);
    }
    break;

  case 154:
#line 1504 "parser.y"
    {
            if(!SMIv3Flag) {
                SMIv3Flag = 1;
                print_note("*** NOTE ***  VirtualTableConstructors inside a sequence (SEQUENCE OF) are not used in SMIv2 or SMIv2, setting the SMIv3 flag.", lineBuf);
            }
    }
    break;

  case 155:
#line 1510 "parser.y"
    {
            yyvsp[0].tree_ptr->child[child_identity_0] = yyvsp[-1].tree_ptr;
            yyval.tree_ptr = yyvsp[0].tree_ptr;
    }
    break;

  case 156:
#line 1515 "parser.y"
    {
            print_warning("*** ERROR ***  Object identifiers must begin with a lowercase letter.", lineBuf);
    }
    break;

  case 157:
#line 1518 "parser.y"
    {
            print_warning("*** ERROR ***  Object identifiers must begin with a lowercase letter.", lineBuf);
    }
    break;

  case 158:
#line 1522 "parser.y"
    {
            if(!SMIv3Flag) {
                SMIv3Flag = 1;
                print_note("*** NOTE ***  VirtualTableConstructors inside a sequence (SEQUENCE OF) are not used in SMIv2 or SMIv2, setting the SMIv3 flag.", lineBuf);
            }
            print_warning("*** ERROR ***  Object identifiers must begin with a lowercase letter.", lineBuf);

    }
    break;

  case 159:
#line 1530 "parser.y"
    {
            yyvsp[0].tree_ptr->child[child_identity_0] = yyvsp[-1].tree_ptr;
            yyval.tree_ptr = yyvsp[0].tree_ptr;
            print_warning("*** ERROR ***  Object identifiers must begin with a lowercase letter.", lineBuf);
    }
    break;

  case 161:
#line 1540 "parser.y"
    {
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
    }
    break;

  case 165:
#line 1548 "parser.y"
    {
            yyvsp[-3].tree_ptr->enumeratedItems         = yyvsp[-1].tree_ptr->enumeratedItems;
            yyvsp[-1].tree_ptr->enumeratedItems         = NULL;
            yyvsp[-3].tree_ptr->child[child_status___2] = yyvsp[-1].tree_ptr;
    }
    break;

  case 167:
#line 1555 "parser.y"
    { 
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
            fill_in_max_min_values(yyvsp[0].tree_ptr, INTEGER);
            check_range_limits(yyvsp[0].tree_ptr, INTEGER);
            check_for_range_overlap(yyvsp[0].tree_ptr);
    }
    break;

  case 168:
#line 1561 "parser.y"
    { 
            /* this isn't really legal, should be range restriction */
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
            print_warning("*** ERROR ***  This should be a range restriction, NOT a size restriction.", lineBuf);
    }
    break;

  case 169:
#line 1566 "parser.y"
    {
            yyvsp[-3].tree_ptr->type                    = BITS;
            yyvsp[-3].tree_ptr->enumeratedItems         = yyvsp[-1].tree_ptr->enumeratedItems;
            yyvsp[-1].tree_ptr->enumeratedItems         = NULL;
            yyvsp[-3].tree_ptr->child[child_status___2] = yyvsp[-1].tree_ptr;
    }
    break;

  case 171:
#line 1574 "parser.y"
    {
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
    }
    break;

  case 172:
#line 1577 "parser.y"
    {
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
            print_note("*** NOTE ***  OCTET STRING should have SIZE specification.", lineBuf);
    }
    break;

  case 173:
#line 1581 "parser.y"
    {
            yyvsp[-3].tree_ptr->enumeratedItems         = yyvsp[-1].tree_ptr->enumeratedItems;
            yyvsp[-1].tree_ptr->enumeratedItems         = NULL;
            yyvsp[-3].tree_ptr->child[child_status___2] = yyvsp[-1].tree_ptr;
    }
    break;

  case 175:
#line 1588 "parser.y"
    {
            print_warning("*** ERROR ***  LowercaseIdentifiers cannot be used for SequenceSyntax.", lineBuf);
    }
    break;

  case 176:
#line 1591 "parser.y"
    {
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
    }
    break;

  case 177:
#line 1594 "parser.y"
    {
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->type = SIZE;
    }
    break;

  case 178:
#line 1598 "parser.y"
    {
            yyvsp[-3].tree_ptr->type                    = BITS;
            yyvsp[-3].tree_ptr->enumeratedItems         = yyvsp[-1].tree_ptr->enumeratedItems;
            yyvsp[-1].tree_ptr->enumeratedItems         = NULL;
            yyvsp[-3].tree_ptr->child[child_status___2] = yyvsp[-1].tree_ptr;
    }
    break;

  case 181:
#line 1608 "parser.y"
    { 
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
            fill_in_max_min_values(yyvsp[0].tree_ptr, INTEGER);
            check_range_limits(yyvsp[0].tree_ptr, INTEGER);
            check_for_range_overlap(yyvsp[0].tree_ptr);
    }
    break;

  case 182:
#line 1614 "parser.y"
    { 
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr;
            fill_in_max_min_values(yyvsp[0].tree_ptr, Unsigned32);
            check_range_limits(yyvsp[0].tree_ptr, Unsigned32);
            check_for_range_overlap(yyvsp[0].tree_ptr);
    }
    break;

  case 183:
#line 1620 "parser.y"
    { 
            yyvsp[-1].tree_ptr->child[child_range____6] = yyvsp[0].tree_ptr; 
            fill_in_max_min_values(yyvsp[0].tree_ptr, Unsigned32);
            check_range_limits(yyvsp[0].tree_ptr, Unsigned32);
            check_for_range_overlap(yyvsp[0].tree_ptr);
    }
    break;

  case 184:
#line 1627 "parser.y"
    {
            yyvsp[-3].tree_ptr->type                    = BITS;
            yyvsp[-3].tree_ptr->enumeratedItems         = yyvsp[-1].tree_ptr->enumeratedItems;
            yyvsp[-1].tree_ptr->enumeratedItems         = NULL;
            yyvsp[-3].tree_ptr->child[child_status___2] = yyvsp[-1].tree_ptr;
    }
    break;

  case 185:
#line 1633 "parser.y"
    {                /* 0 - 4294967295         */
            yyvsp[-3].tree_ptr->type                    = BITS;
            yyvsp[-3].tree_ptr->enumeratedItems         = yyvsp[-1].tree_ptr->enumeratedItems;
            yyvsp[-1].tree_ptr->enumeratedItems         = NULL;
            yyvsp[-3].tree_ptr->child[child_status___2] = yyvsp[-1].tree_ptr;
    }
    break;

  case 186:
#line 1639 "parser.y"
    {          /* 0 - 4294967295         */
            yyvsp[-3].tree_ptr->type                    = BITS;
            yyvsp[-3].tree_ptr->enumeratedItems         = yyvsp[-1].tree_ptr->enumeratedItems;
            yyvsp[-1].tree_ptr->enumeratedItems         = NULL;
            yyvsp[-3].tree_ptr->child[child_status___2] = yyvsp[-1].tree_ptr;
    }
    break;

  case 190:
#line 1654 "parser.y"
    {
            print_warning("*** ERROR ***  Object identifiers must begin with a lowercase letter.", lineBuf);
    }
    break;

  case 192:
#line 1669 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = TRAP_TYPE;
            tree_node->child[child_identity_0] = yyvsp[-8].tree_ptr;

            tree_node->child[child_object___5] = yyvsp[0].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[-5].tree_ptr;

            tree_node->child[child_descript_3] = yyvsp[-3].tree_ptr;
            tree_node->child[child_misc1___14] = yyvsp[-4].tree_ptr;

            yyvsp[-3].tree_ptr->parent = tree_node;

            yyval.tree_ptr = tree_node;

            if(!yyvsp[-3].tree_ptr->type) {
                /*
                if we are using SMIv2, this is an error, otherwise
                set the flag for SMIv1
                */
                if(SMIv2Flag == 1) { 
                    print_warning("*** ERROR *** \"DESCRIPTION\" is required in SMIv2.", lineBuf);
                } else if(SMIv1Flag == 0) {
                    SMIv1Flag = 1;
                    print_note("*** NOTE *** \"DESCRIPTION\" can only be skipped in SMIv1, setting SMIv1 flag.", lineBuf);
                }
            }
 
    }
    break;

  case 193:
#line 1703 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = TRAP_TYPE;
            tree_node->child[child_identity_0] = yyvsp[-8].tree_ptr;

            tree_node->child[child_object___5] = yyvsp[0].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[-5].tree_ptr;

            tree_node->child[child_descript_3] = yyvsp[-3].tree_ptr;
            tree_node->child[child_misc1___14] = yyvsp[-4].tree_ptr;

            yyvsp[-3].tree_ptr->parent = tree_node;

            yyval.tree_ptr = tree_node;
 
            print_warning("*** ERROR ***  You must use a LowercaseIdentifier to name a trap type.", lineBuf);

            if(!yyvsp[-3].tree_ptr->type) {
                /*
                if we are using SMIv2, this is an error, otherwise
                set the flag for SMIv1
                */
                if(SMIv2Flag == 1) { 
                    print_warning("*** ERROR *** \"DESCRIPTION\" is required in SMIv2.", lineBuf);
                } else if(SMIv1Flag == 0) {
                    SMIv1Flag = 1;
                    print_note("*** NOTE *** \"DESCRIPTION\" can only be skipped in SMIv1, setting SMIv1 flag.", lineBuf);
                }
            }

    }
    break;

  case 194:
#line 1738 "parser.y"
    { 
            yyval.tree_ptr = yyvsp[-1].tree_ptr;
    }
    break;

  case 196:
#line 1754 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = MODULE_CONFORMANCE;
            tree_node->string                  = "module-conformance";
            tree_node->child[child_identity_0] = yyvsp[-9].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[-3].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[0].tree_ptr;

            tree_node->child[child_misc1___14] = yyvsp[-2].tree_ptr;

            yyvsp[-3].tree_ptr->parent = tree_node;

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 197:
#line 1782 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = MODULE_COMPLIANCE;
            tree_node->string                  = "module-compliance";
            tree_node->child[child_identity_0] = yyvsp[-7].tree_ptr;
            tree_node->child[child_status___2] = yyvsp[-5].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[-4].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[0].tree_ptr;

            tree_node->child[child_misc1___14] = yyvsp[-2].tree_ptr;

            yyvsp[-4].tree_ptr->parent = tree_node;

            yyval.tree_ptr = tree_node; 

	    /*BAB*/
	    /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

    }
    break;

  case 198:
#line 1813 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = MODULE_COMPLIANCE;
            tree_node->string                  = "module-compliance";
            tree_node->child[child_identity_0] = yyvsp[-7].tree_ptr;
            tree_node->child[child_status___2] = yyvsp[-5].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[-4].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[0].tree_ptr;

            tree_node->child[child_misc1___14] = yyvsp[-2].tree_ptr;

            yyvsp[-4].tree_ptr->parent = tree_node;

            yyval.tree_ptr = tree_node;

	    /*BAB*/
	    /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            print_warning("*** ERROR ***  \"MODULE-COMPLIANCE\" names must begin with a lowercase letter.", lineBuf);
 
    }
    break;

  case 199:
#line 1844 "parser.y"
    { 
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr; 
    }
    break;

  case 200:
#line 1847 "parser.y"
    {
            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 201:
#line 1856 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = yyvsp[-2].tree_ptr;
            tree_node->child[child_misc1___14] = yyvsp[-1].tree_ptr;
            tree_node->child[child_misc2___15] = yyvsp[0].tree_ptr;

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 202:
#line 1869 "parser.y"
    {
            yyval.tree_ptr = yyvsp[-1].tree_ptr;
    }
    break;

  case 204:
#line 1886 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = AGENT_CAPABILITIES;
            tree_node->string                  = "agent-capabilities";
            tree_node->child[child_identity_0] = yyvsp[-9].tree_ptr;
            tree_node->child[child_status___2] = yyvsp[-5].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[-4].tree_ptr;
            tree_node->child[child_misc1___14] = yyvsp[-2].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[0].tree_ptr;

            yyvsp[-4].tree_ptr->parent = tree_node;

            yyval.tree_ptr = tree_node; 

    }
    break;

  case 205:
#line 1915 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = NOTIFICATION_GROUP;
            tree_node->string                  = "notification-group";
            tree_node->child[child_identity_0] = yyvsp[-10].tree_ptr;
            tree_node->child[child_status___2] = yyvsp[-4].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[-3].tree_ptr;
	    tree_node->child[child_referenc_4] = yyvsp[-2].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[0].tree_ptr;

            tree_node->child[child_misc1___14] = yyvsp[-6].tree_ptr;

            yyvsp[-3].tree_ptr->parent = tree_node;

            yyval.tree_ptr = tree_node; 

            /*BAB*/
            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }
            /*BAB*/

    }
    break;

  case 206:
#line 1955 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = OBJECT_GROUP;
            tree_node->string                  = "object-group";
            tree_node->child[child_identity_0] = yyvsp[-10].tree_ptr;
            tree_node->child[child_status___2] = yyvsp[-4].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[-3].tree_ptr;
	    tree_node->child[child_referenc_4] = yyvsp[-2].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[0].tree_ptr;

            tree_node->child[child_misc1___14] = yyvsp[-6].tree_ptr;

            yyvsp[-3].tree_ptr->parent = tree_node;

            yyval.tree_ptr = tree_node; 

            /*BAB*/
            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }
            /*BAB*/

    }
    break;

  case 207:
#line 1995 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = NOTIFICATION_TYPE;
            tree_node->string                  = "notification";

            tree_node->child[child_identity_0] = yyvsp[-7].tree_ptr;
            tree_node->child[child_object___5] = yyvsp[-5].tree_ptr;
            tree_node->child[child_status___2] = yyvsp[-4].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[-3].tree_ptr;
            tree_node->child[child_referenc_4] = yyvsp[-2].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[0].tree_ptr;

            yyvsp[-3].tree_ptr->parent = tree_node;

            yyval.tree_ptr = tree_node; 

	    /*BAB*/
            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }
	    /*BAB*/

    }
    break;

  case 208:
#line 2026 "parser.y"
    {
            yyval.tree_ptr = yyvsp[-1].tree_ptr;
    }
    break;

  case 210:
#line 2046 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = OBJECT_TYPE;

            tree_node->child[child_identity_0] = yyvsp[-11].tree_ptr;
            tree_node->child[child_status___2] = yyvsp[-6].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[-5].tree_ptr;
	    /*BAB add reference*/
	    tree_node->child[child_referenc_4] = yyvsp[-4].tree_ptr;
            tree_node->child[child_index____7] = yyvsp[-3].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[0].tree_ptr;
            tree_node->child[child_default_10] = yyvsp[-2].tree_ptr;
            tree_node->child[child_access__11] = yyvsp[-7].tree_ptr;
            tree_node->child[child_syntax___1] = yyvsp[-9].tree_ptr;

#ifdef NOT_YET
            /*BAB - test to see what is available*/
            tree_node->child[child_identity_0] = yyvsp[-11].tree_ptr;
            /*tree_node->child[child_syntax___1] = $2;*/
            tree_node->child[child_status___2] = yyvsp[-9].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[-8].tree_ptr;
            tree_node->child[child_referenc_4] = yyvsp[-7].tree_ptr;
            tree_node->child[child_object___5] = yyvsp[-6].tree_ptr;
            tree_node->child[child_range____6] = yyvsp[-5].tree_ptr;
            tree_node->child[child_index____7] = yyvsp[-4].tree_ptr;
            tree_node->child[child_display__8] = yyvsp[-3].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[-2].tree_ptr;
            /*tree_node->child[child_default_10] = $11;*/
            tree_node->child[child_access__11] = yyvsp[0].tree_ptr;
            /*tree_node->child[chld_strt_rnge12] = $13;*/
            /*tree_node->child[chld_end_range13] = $14;*/
            /*tree_node->child[child_misc1___14] = $15;*/
            /*tree_node->child[child_misc2___15] = $16;*/
            /*BAB*/
#endif /* NOT_YET */
	

            yyvsp[-5].tree_ptr->parent  = tree_node;
            yyvsp[-2].tree_ptr->parent = tree_node;

            yyval.tree_ptr = tree_node; 

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            if(yyvsp[-2].tree_ptr->child[child_identity_0]) {
                if( (yyvsp[-9].tree_ptr->type == Counter32) || (yyvsp[-9].tree_ptr->type == Counter64) ) {
                    print_warning("*** ERROR ***  DEFVAL is not allowed with Counter32 or Counter64.", lineBuf);
                    tree_node->child[child_default_10] = NULL;
                }
            }

            if(!yyvsp[-5].tree_ptr->type) {
                /*
                if we are using SMIv2, this is an error, otherwise
                set the flag for SMIv1
                */
                if(SMIv2Flag == 1) { 
                    print_warning("*** ERROR *** \"DESCRIPTION\" is required in SMIv2.", lineBuf);
                } else if(SMIv1Flag == 0) {
                    SMIv1Flag = 1;
                    print_note("*** NOTE *** \"DESCRIPTION\" can only be skipped in SMIv1, setting SMIv1 flag.", lineBuf);
                }
            }

    }
    break;

  case 211:
#line 2126 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = OBJECT_TYPE;
            tree_node->child[child_identity_0] = yyvsp[-11].tree_ptr;
            tree_node->child[child_status___2] = yyvsp[-6].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[-5].tree_ptr;
            tree_node->child[child_index____7] = yyvsp[-3].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[0].tree_ptr;
            tree_node->child[child_default_10] = yyvsp[-2].tree_ptr;
            tree_node->child[child_access__11] = yyvsp[-7].tree_ptr;
            tree_node->child[child_syntax___1] = yyvsp[-9].tree_ptr;

            yyvsp[-5].tree_ptr->parent  = tree_node;
            yyvsp[-2].tree_ptr->parent = tree_node;

            yyval.tree_ptr = tree_node; 

            /* *** SAVE TYPE *** */

            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            /* *** SEMANTIC CHECKS *** */

            print_warning("*** ERROR ***  Object types must begin with a lowercase letter.", lineBuf);
	
            if(yyvsp[-2].tree_ptr->child[child_identity_0]) {
                if( (yyvsp[-9].tree_ptr->type == Counter32) || (yyvsp[-9].tree_ptr->type == Counter64) ) {
                    print_warning("*** ERROR ***  DEFVAL is not allowed with Counter32 or Counter64.", lineBuf);
                    tree_node->child[child_default_10] = NULL;
                }
            }

            if(!yyvsp[-5].tree_ptr->type) {
                /*
                if we are using SMIv2, this is an error, otherwise
                set the flag for SMIv1
                */
                if(SMIv2Flag == 1) { 
                    print_warning("*** ERROR *** \"DESCRIPTION\" is required in SMIv2.", lineBuf);
                } else if(SMIv1Flag == 0) {
                    SMIv1Flag = 1;
                    print_note("*** NOTE *** \"DESCRIPTION\" can only be skipped in SMIv1, setting SMIv1 flag.", lineBuf);
                }
            }

    }
    break;

  case 212:
#line 2180 "parser.y"
    {
            yyval.tree_ptr = yyvsp[0].tree_ptr;

            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** ERROR *** \"MAX-ACCESS\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"MAX-ACCESS\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    break;

  case 213:
#line 2194 "parser.y"
    {
            yyval.tree_ptr = yyvsp[0].tree_ptr;

            /*
            if we are using SMIv2, this is an error, otherwise
            set the flag for SMIv1
            */
            if(SMIv2Flag == 1) {
                print_warning("*** ERROR *** \"ACCESS\" in ObjectAccessPart is only used in SMIv1.", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_note("*** NOTE *** \"ACCESS\" in ObjectAccessPart is only used in SMIv1, setting SMIv1 flag.", lineBuf);
            }
    }
    break;

  case 218:
#line 2216 "parser.y"
    {
             print_warning("*** ERROR *** \"accessible-for-notify\" should only be used in an SMIv2 MIB.", lineBuf);
    }
    break;

  case 219:
#line 2219 "parser.y"
    {
             print_warning("*** ERROR *** \"read-create\" should only be used in an SMIv2 MIB.", lineBuf);
    }
    break;

  case 220:
#line 2225 "parser.y"
    { 
            yyval.tree_ptr = yyvsp[0].tree_ptr;
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** ERROR *** \"UNITS\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"UNITS\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    break;

  case 221:
#line 2238 "parser.y"
    {
    }
    break;

  case 222:
#line 2243 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = INDEX;

            tree_node->child[child_identity_0] = yyvsp[-1].tree_ptr;

            yyval.tree_ptr = tree_node;
    }
    break;

  case 223:
#line 2252 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = INDEX;

            tree_node->child[child_identity_0] = yyvsp[-2].tree_ptr;

            yyval.tree_ptr = tree_node;

            print_warning("*** ERROR ***  There is an extra comma at the end of the index list.", lineBuf);
    }
    break;

  case 224:
#line 2264 "parser.y"
    {
            yyvsp[-1].tree_ptr->type = AUGMENTS;
            yyval.tree_ptr = yyvsp[-1].tree_ptr;
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** ERROR *** \"AUGMENTS\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"AUGMENTS\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    break;

  case 226:
#line 2282 "parser.y"
    {
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr; 
    }
    break;

  case 227:
#line 2285 "parser.y"
    {
            yyvsp[-2].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-2].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 228:
#line 2289 "parser.y"
    {
            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
            print_warning("*** ERROR ***  There is a missing COMMA in the Index List.", lineBuf);
    }
    break;

  case 229:
#line 2297 "parser.y"
    { 
            yyvsp[0].tree_ptr->type = IMPLIED;
            yyval.tree_ptr = yyvsp[0].tree_ptr;
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** ERROR *** \"IMPLIED\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"IMPLIED\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
 
    }
    break;

  case 231:
#line 2328 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = STRUCT;
            tree_node->string                  = "Struct";
            tree_node->child[child_identity_0] = yyvsp[-9].tree_ptr;
            tree_node->child[child_status___2] = yyvsp[-6].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[-5].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[0].tree_ptr;
            tree_node->child[child_access__11] = yyvsp[-7].tree_ptr;

            tree_node->child[child_misc1___14] = yyvsp[-3].tree_ptr;

            yyvsp[-5].tree_ptr->parent = tree_node;

            yyval.tree_ptr = tree_node;

            if(!yyvsp[-5].tree_ptr->type) {
                /*
                if we are using SMIv2, this is an error, otherwise
                set the flag for SMIv1
                */
                if(SMIv2Flag == 1) { 
                    print_warning("*** ERROR *** \"DESCRIPTION\" is required in SMIv2.", lineBuf);
                } else if(SMIv1Flag == 0) {
                    SMIv1Flag = 1;
                    print_note("*** NOTE *** \"DESCRIPTION\" can only be skipped in SMIv1, setting SMIv1 flag.", lineBuf);
                }
            }

    }
    break;

  case 232:
#line 2363 "parser.y"
    {
            if(!SMIv3Flag) {
                SMIv3Flag = 1;
                print_note("*** NOTE ***  STRUCT is not used in SMIv2 or SMIv2, setting the SMIv3 flag.", lineBuf);
            }   
    }
    break;

  case 233:
#line 2372 "parser.y"
    {
            if(!SMIv3Flag) {
                SMIv3Flag = 1;
                print_note("*** NOTE ***  UNION is not used in SMIv2 or SMIv2, setting the SMIv3 flag.", lineBuf);
            }
    }
    break;

  case 234:
#line 2390 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = UNION;
            tree_node->string                  = "Union";
            tree_node->child[child_identity_0] = yyvsp[-10].tree_ptr;
            tree_node->child[child_status___2] = yyvsp[-7].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[-6].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[0].tree_ptr;
            tree_node->child[child_access__11] = yyvsp[-8].tree_ptr;

            tree_node->child[child_misc1___14] = yyvsp[-4].tree_ptr;
            tree_node->child[child_misc2___15] = yyvsp[-3].tree_ptr;

            yyvsp[-6].tree_ptr->parent = tree_node;

            yyval.tree_ptr = tree_node;

            if(!yyvsp[-6].tree_ptr->type) {
                /*
                if we are using SMIv2, this is an error, otherwise
                set the flag for SMIv1
                */
                if(SMIv2Flag == 1) { 
                    print_warning("*** ERROR *** \"DESCRIPTION\" is required in SMIv2.", lineBuf);
                } else if(SMIv1Flag == 0) {
                    SMIv1Flag = 1;
                    print_note("*** NOTE *** \"DESCRIPTION\" can only be skipped in SMIv1, setting SMIv1 flag.", lineBuf);
                }
            }

    }
    break;

  case 235:
#line 2426 "parser.y"
    {
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 236:
#line 2429 "parser.y"
    {
            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 240:
#line 2442 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            yyval.tree_ptr = tree_node;
    }
    break;

  case 241:
#line 2447 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            yyval.tree_ptr = tree_node;
    }
    break;

  case 242:
#line 2465 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = TEXTUAL_CONVENTION;
	    tree_node->string = strdup("TEXTUAL_CONVENTION");
            tree_node->child[child_identity_0] = yyvsp[-7].tree_ptr;
            tree_node->child[child_display__8] = yyvsp[-4].tree_ptr;
            tree_node->child[child_status___2] = yyvsp[-3].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[-2].tree_ptr;
	    tree_node->child[child_referenc_4] = yyvsp[-1].tree_ptr;
            tree_node->child[child_syntax___1] = yyvsp[0].tree_ptr;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 243:
#line 2492 "parser.y"
    {

            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            yyval.tree_ptr = tree_node;

            /*
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = TEXTUAL_CONVENTION;
	    tree_node->string = strdup("TEXTUAL_CONVENTION");
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_display__8] = $4;
            tree_node->child[child_status___2] = $5;
            tree_node->child[child_descript_3] = $6;
            tree_node->child[child_syntax___1] = $8;

            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            $$ = tree_node;
            */ 
    }
    break;

  case 244:
#line 2522 "parser.y"
    { 
            print_note("*** NOTE ***  Ignoring re-definition of pre-defined type.", lineBuf);
    }
    break;

  case 245:
#line 2528 "parser.y"
    { 
            yyval.tree_ptr = yyvsp[0].tree_ptr; 
    }
    break;

  case 247:
#line 2545 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = MODULE_IDENTITY;
            tree_node->string                  = "module-identity";

            /*BAB - this was all ray was including 
	    tree_node->child[child_identity_0] = $1;
            tree_node->child[child_descript_3] = $8;
            tree_node->child[child_referenc_4] = $9;
            tree_node->child[child_value____9] = $11;
	    */
            /* revision contains descriptions */
            /*tree_node->child[child_misc1___14] = $9;*/

	    /*BAB Adding more info to include lastUpdate, organization
  	      and contactInfo
   	     */
            tree_node->child[child_identity_0] = yyvsp[-10].tree_ptr;
	    tree_node->child[child_misc1___14] = yyvsp[-8].tree_ptr;
	    tree_node->child[child_status___2] = yyvsp[-6].tree_ptr;
	    tree_node->child[child_access__11] = yyvsp[-4].tree_ptr;
	    tree_node->child[child_descript_3] = yyvsp[-3].tree_ptr;
	    tree_node->child[child_referenc_4] = yyvsp[-2].tree_ptr;
	    tree_node->child[child_value____9] = yyvsp[0].tree_ptr;
	    /*BAB*/

            yyvsp[-3].tree_ptr->parent = tree_node;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            yyval.tree_ptr = tree_node; 

    }
    break;

  case 248:
#line 2588 "parser.y"
    {

            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** ERROR *** \"MODULE-IDENTITY\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"MODULE-IDENTITY\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    break;

  case 249:
#line 2601 "parser.y"
    {

            print_warning("*** ERROR ***  \"MODULE-IDENTITY\" names must begin with an uppercase letter.", lineBuf);

            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** ERROR *** \"MODULE-IDENTITY\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"MODULE-IDENTITY\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    break;

  case 252:
#line 2624 "parser.y"
    { 
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr; 
    }
    break;

  case 253:
#line 2627 "parser.y"
    {
            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 254:
#line 2634 "parser.y"
    {
            /*$$ = $3;*/

	    /* BAB - Ray did not save the Revision date. Modified to 
	     * add both date and description
	     */
	    tree * tree_node = NULL;
	    tree_node = malloc_node(tree_node, SINGLE_PARSE);
	    tree_node->child[child_descript_3] = yyvsp[0].tree_ptr;	/*description*/
	    tree_node->child[child_referenc_4] = yyvsp[-1].tree_ptr; 	/*revision date*/
  	    yyval.tree_ptr = tree_node;
    }
    break;

  case 255:
#line 2657 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = OBJECT_IDENTITY;
            tree_node->string                  = "object-id";
            tree_node->child[child_identity_0] = yyvsp[-6].tree_ptr;
            tree_node->child[child_status___2] = yyvsp[-4].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[-3].tree_ptr;
            tree_node->child[child_referenc_4] = yyvsp[-2].tree_ptr;
            tree_node->child[child_value____9] = yyvsp[0].tree_ptr;

            yyvsp[-3].tree_ptr->parent = tree_node;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            yyval.tree_ptr = tree_node;

    }
    break;

  case 256:
#line 2688 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = DOT_DOT;
            tree_node->child[chld_end_range13] = yyvsp[0].tree_ptr;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 257:
#line 2695 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = DOT_DOT;
            tree_node->child[chld_strt_rnge12] = yyvsp[-2].tree_ptr;
            tree_node->child[chld_end_range13] = yyvsp[0].tree_ptr;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 261:
#line 2712 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            yyval.tree_ptr = tree_node;
    }
    break;

  case 262:
#line 2720 "parser.y"
    {
        /* uppercase name of row */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = OF;

            tree_node->child[child_identity_0] = yyvsp[0].tree_ptr;

            yyval.tree_ptr = tree_node;

            /* insert name into symbol table */
            insert_name(yyvsp[0].tree_ptr, virtualTables);

    }
    break;

  case 263:
#line 2734 "parser.y"
    {
        /* uppercase name of row */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = OF;

            tree_node->child[child_identity_0] = yyvsp[0].tree_ptr;

            yyval.tree_ptr = tree_node;

            /* insert name into symbol table */
            insert_name(yyvsp[0].tree_ptr, virtualTables);

            print_warning("*** ERROR *** \"SEQUENCE OF\" must be followed by an UppercaseIdentifier.", lineBuf);

    }
    break;

  case 265:
#line 2755 "parser.y"
    {
            /*
            if we are using SMIv2, this is an error, otherwise
            set the flag for SMIv1
            */
            if(SMIv2Flag == 1) { 
                print_warning("*** ERROR *** \"DESCRIPTION\" is required in SMIv2.", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_note("*** NOTE *** \"DESCRIPTION\" can only be skipped in SMIv1, setting SMIv1 flag.", lineBuf);
            }
    }
    break;

  case 266:
#line 2770 "parser.y"
    {
            yyvsp[0].tree_ptr->type = DESCRIPTION;
            yyval.tree_ptr = yyvsp[0].tree_ptr;
    }
    break;

  case 269:
#line 2782 "parser.y"
    { 
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 270:
#line 2785 "parser.y"
    { 
            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 273:
#line 2797 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = yyvsp[-1].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[0].tree_ptr;

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 274:
#line 2813 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = yyvsp[-4].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[0].tree_ptr;

            tree_node->child[child_misc1___14] = yyvsp[-2].tree_ptr;

            tree_node->child[child_access__11] = yyvsp[-1].tree_ptr;

            tree_node->child[child_misc2___15]  = yyvsp[-3].tree_ptr;

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 275:
#line 2831 "parser.y"
    { yyval.tree_ptr = yyvsp[0].tree_ptr; }
    break;

  case 282:
#line 2844 "parser.y"
    {
            yyval.tree_ptr = yyvsp[-1].tree_ptr;
    }
    break;

  case 286:
#line 2863 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = yyvsp[-6].tree_ptr;
            tree_node->child[child_descript_3] = yyvsp[0].tree_ptr;
            tree_node->child[child_referenc_4] = yyvsp[-2].tree_ptr;

            tree_node->child[child_misc1___14] = yyvsp[-4].tree_ptr;

            tree_node->child[child_default_10] = yyvsp[-1].tree_ptr;
            tree_node->child[child_access__11] = yyvsp[-3].tree_ptr;

            tree_node->child[child_misc2___15] = yyvsp[-5].tree_ptr;

            if(yyvsp[-1].tree_ptr->child[child_identity_0]) {

                if( (yyvsp[-5].tree_ptr->type == Counter32) || (yyvsp[-5].tree_ptr->type == Counter64) ) {
                    print_warning("*** ERROR ***  DEFVAL is not allowed with Counter32 or Counter64.", lineBuf);
                    tree_node->child[child_default_10] = NULL;
                }
            }

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 287:
#line 2891 "parser.y"
    { yyval.tree_ptr = yyvsp[0].tree_ptr; }
    break;

  case 298:
#line 2913 "parser.y"
    { yyval.tree_ptr = yyvsp[0].tree_ptr; }
    break;

  case 300:
#line 2918 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = yyvsp[-1].tree_ptr;

            tree_node->child[1] = yyvsp[0].tree_ptr;

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 301:
#line 2927 "parser.y"
    { /* only if contained in MIB module */ }
    break;

  case 306:
#line 2941 "parser.y"
    { 
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 307:
#line 2944 "parser.y"
    { 
            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 308:
#line 2951 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = yyvsp[-5].tree_ptr;
            tree_node->child[1]          = yyvsp[-2].tree_ptr;
            tree_node->child[5]          = yyvsp[0].tree_ptr;

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 311:
#line 2969 "parser.y"
    { 
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 312:
#line 2972 "parser.y"
    {  
            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 314:
#line 2980 "parser.y"
    {
        print_warning("*** ERROR ***  There is an extra comma at the end of the variable type list.", lineBuf);
    }
    break;

  case 315:
#line 2986 "parser.y"
    { 
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr; 
    }
    break;

  case 316:
#line 2989 "parser.y"
    {
            yyvsp[-2].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-2].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 317:
#line 2993 "parser.y"
    {
            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
            print_warning("*** ERROR ***  There is a missing COMMA in the Variable Type List.", lineBuf);
    }
    break;

  case 319:
#line 3002 "parser.y"
    {
            print_warning("*** ERROR ***  Variable types must be LowercaseIdentifiers.", lineBuf);
    }
    break;

  case 320:
#line 3008 "parser.y"
    {
            yyval.tree_ptr = yyvsp[0].tree_ptr;
    }
    break;

  case 321:
#line 3021 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = DEFVAL;

            tree_node->child[child_identity_0] = yyvsp[-1].tree_ptr;

            yyval.tree_ptr = tree_node;
    }
    break;

  case 323:
#line 3035 "parser.y"
    {
            yyvsp[-3].tree_ptr->child[child_identity_0] = yyvsp[-1].tree_ptr;
    }
    break;

  case 324:
#line 3038 "parser.y"
    {
            yyvsp[-3].tree_ptr->child[child_identity_0] = yyvsp[-1].tree_ptr;
             print_warning("*** ERROR ***  Named items must begin with a lowercase letter.", lineBuf);
    }
    break;

  case 325:
#line 3042 "parser.y"
    {
            yyvsp[-3].tree_ptr->child[child_identity_0] = yyvsp[-1].tree_ptr;
             print_warning("*** ERROR ***  Numbers must not be used for named items.", lineBuf);
    }
    break;

  case 327:
#line 3050 "parser.y"
    {
            print_warning("*** ERROR ***  There is an extra COMMA at then end of the Named Item List.", lineBuf);
    }
    break;

  case 328:
#line 3056 "parser.y"
    {
            symbol_table * enumerationTable = NULL;

            /* create enumeration table */
            enumerationTable = malloc_table(enumerationTable);
 
            /* insert name into symbol table */
            insert_name(yyvsp[0].tree_ptr, enumerationTable);

            yyvsp[0].tree_ptr->enumeratedItems = enumerationTable;

            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;

    }
    break;

  case 329:
#line 3070 "parser.y"
    {

            /* insert name into symbol table */
            insert_name(yyvsp[0].tree_ptr, yyvsp[-2].tree_ptr->enumeratedItems);

            yyvsp[-2].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-2].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 330:
#line 3078 "parser.y"
    {

            insert_name(yyvsp[0].tree_ptr, yyvsp[-1].tree_ptr->enumeratedItems);

            yyvsp[-1].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-1].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
            print_warning("*** ERROR ***  There is a missing COMMA in the Named Item List.", lineBuf);
    }
    break;

  case 337:
#line 3122 "parser.y"
    {
            yyval.tree_ptr = yyvsp[0].tree_ptr;
    }
    break;

  case 343:
#line 3136 "parser.y"
    { 
            yyval.tree_ptr = yyvsp[0].tree_ptr;
    }
    break;

  case 344:
#line 3139 "parser.y"
    {
    }
    break;

  case 345:
#line 3144 "parser.y"
    {
        yyval.tree_ptr = yyvsp[-1].tree_ptr;
    }
    break;

  case 346:
#line 3150 "parser.y"
    {
        yyval.tree_ptr = yyvsp[-1].tree_ptr;
    }
    break;

  case 348:
#line 3157 "parser.y"
    {
            print_warning("*** ERROR ***  There is an extra BAR at the end of the range list.", lineBuf);
    }
    break;

  case 349:
#line 3163 "parser.y"
    {
            yyvsp[0].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 350:
#line 3166 "parser.y"
    {
            yyvsp[-2].tree_ptr->last_tree_node_in_list->next_tree_node = yyvsp[0].tree_ptr;
            yyvsp[-2].tree_ptr->last_tree_node_in_list = yyvsp[0].tree_ptr;
    }
    break;

  case 351:
#line 3208 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            /* stringBuf was zeroed by the lexer before being filled */
            /* so there is a null after the string */ 
            tree_node->string = (char *)malloc( (stringPos + 1)*sizeof(char) ); 
            memcpy(tree_node->string, stringBuf, stringPos);
            tree_node->string[stringPos] = '\0';

            tree_node->freeMe = 1;

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 352:
#line 3243 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = strdup(yylval.char_ptr);

            tree_node->freeMe = 1;

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 353:
#line 3256 "parser.y"
    { 
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"optional\" keyword as a lowercase identifier.", lineBuf);
    }
    break;

  case 354:
#line 3259 "parser.y"
    { 
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"read-only\" keyword as a lowercase identifier.", lineBuf);
    }
    break;

  case 355:
#line 3262 "parser.y"
    { 
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"read-write\" keyword as a lowercase identifier.", lineBuf);
    }
    break;

  case 356:
#line 3265 "parser.y"
    {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"write-only\" keyword as a lowercase identifier.", lineBuf);
    }
    break;

  case 357:
#line 3268 "parser.y"
    {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"not-accessible\" keyword as a lowercase identifier.", lineBuf);
    }
    break;

  case 358:
#line 3271 "parser.y"
    {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"not-implemented\" keyword as a lowercase identifier.", lineBuf);
    }
    break;

  case 359:
#line 3274 "parser.y"
    {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"acccessible-for-notifify\" keyword as a lowercase identifier.", lineBuf);
    }
    break;

  case 360:
#line 3277 "parser.y"
    {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"read-create\" keyword as a lowercase identifier.", lineBuf);
    }
    break;

  case 361:
#line 3280 "parser.y"
    {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"deprecated\" keyword as a lowercase identifier.", lineBuf);
    }
    break;

  case 362:
#line 3283 "parser.y"
    {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"current\" keyword as a lowercase identifier.", lineBuf);
    }
    break;

  case 363:
#line 3286 "parser.y"
    {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"mandatory\" keyword as a lowercase identifier.", lineBuf);
    }
    break;

  case 364:
#line 3289 "parser.y"
    {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"obsolete\" keyword as a lowercase identifier.", lineBuf);
    }
    break;

  case 365:
#line 3295 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = strdup(yylval.char_ptr);
            tree_node->freeMe = 1;

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            yyval.tree_ptr = tree_node;
    }
    break;

  case 366:
#line 3310 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = strdup(yylval.char_ptr);

            tree_node->freeMe = 1;

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 367:
#line 3326 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = POS_NUMBER;
            tree_node->string = strdup(yylval.char_ptr);
            tree_node->freeMe = 1;

            /* convert string to decimal number */
            tree_node->number = strtoul(tree_node->string, (char **)NULL, 10);

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 368:
#line 3342 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = MAX;
            yyval.tree_ptr = tree_node; 
    }
    break;

  case 369:
#line 3348 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = MIN;
            yyval.tree_ptr = tree_node; 
    }
    break;

  case 370:
#line 3354 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = NEG_NUMBER;
            tree_node->string = strdup(yylval.char_ptr);
            tree_node->freeMe = 1;

            /* convert string to decimal number */
            tree_node->number = strtoul(tree_node->string, (char **)NULL, 10);

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 371:
#line 3370 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = POS_NUMBER;
            tree_node->string = strdup(yylval.char_ptr);
            tree_node->freeMe = 1;

            /* convert string to decimal number */
            tree_node->number = strtoul(tree_node->string+1, (char **)NULL, 2);

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 372:
#line 3386 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = POS_NUMBER;
            tree_node->string = strdup(yylval.char_ptr);
            tree_node->freeMe = 1;

            /* convert string to decimal number */
            tree_node->number = strtoul(tree_node->string+1, (char **)NULL, 16);

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            yyval.tree_ptr = tree_node; 
    }
    break;

  case 373:
#line 3405 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "read-only"; 
            yyval.tree_ptr = tree_node;
    }
    break;

  case 374:
#line 3414 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "read-write"; 
            yyval.tree_ptr = tree_node;
    }
    break;

  case 375:
#line 3423 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "write-only"; 
            yyval.tree_ptr = tree_node;

            if(SMIv2Flag == 1) { 
                print_warning("*** ERROR *** \"write-only\" is not used in SMIv2.", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_note("*** NOTE *** \"write-only\" is not used in SMIv2, setting SMIv1 flag.", lineBuf);
            }

    }
    break;

  case 376:
#line 3440 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "not-accessible"; 
            yyval.tree_ptr = tree_node;
    }
    break;

  case 377:
#line 3449 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "not-implemented"; 
            yyval.tree_ptr = tree_node;

            if(SMIv1Flag == 1) { 
                print_warning("*** ERROR *** \"not-implemented\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"not-implemented\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    break;

  case 378:
#line 3466 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "accessible-for-notify"; 
            yyval.tree_ptr = tree_node;

            if(SMIv1Flag == 1) { 
                print_warning("*** ERROR *** \"accessible-for-notify\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"accessible-for-notify\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    break;

  case 379:
#line 3483 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "read-create"; 
            yyval.tree_ptr = tree_node;

            if(SMIv1Flag == 1) { 
                print_warning("*** ERROR *** \"read-create\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"read-create\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    break;

  case 380:
#line 3500 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = BITS;
	    if (formatFlag == XML) {
		tree_node->string = "BITS";
            }
	    else {
                tree_node->string = "Bits";
	    }
            yyval.tree_ptr = tree_node;

            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** ERROR *** \"BITS\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"BITS\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    break;

  case 381:
#line 3526 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            yyval.tree_ptr = tree_node;
    }
    break;

  case 382:
#line 3534 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "current";
            yyval.tree_ptr = tree_node;
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) { 
                print_warning("*** ERROR *** \"current\" should not be used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"current\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    break;

  case 383:
#line 3553 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "deprecated";
            yyval.tree_ptr = tree_node;
    }
    break;

  case 384:
#line 3562 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "obsolete"; 
            yyval.tree_ptr = tree_node;
    }
    break;

  case 385:
#line 3571 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "mandatory";
            yyval.tree_ptr = tree_node;
            /*
            if we are using SMIv2, this is an error, otherwise
            set the flag for SMIv1
            */
            if(SMIv2Flag == 1) { 
                print_warning("*** ERROR *** \"mandatory\" should only be used in SMIv1 (perhaps you want to use \"current\").", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_note("*** NOTE *** \"mandatory\" is only used in SMIv1, setting SMIv1 flag.", lineBuf);
            }
    }
    break;

  case 386:
#line 3590 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "optional"; 
            yyval.tree_ptr = tree_node;
            /*
            if we are using SMIv2, this is an error, otherwise
            set the flag for SMIv1
            */
            if(SMIv2Flag == 1) { 
                print_warning("*** ERROR *** \"optional\" should only be used in SMIv1.", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_note("*** NOTE *** \"optional\" is only used in SMIv1, setting SMIv1 flag.", lineBuf);
            }
    }
    break;

  case 387:
#line 3609 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "OctetString";
            tree_node->type = OCTET;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 388:
#line 3619 "parser.y"
    {        /* (-2147483648..2147483647) */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Integer32";
            yyval.tree_ptr = tree_node; 

            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** ERROR *** \"Integer32\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"Integer32\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    break;

  case 389:
#line 3644 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "IpAddress";
            yyval.tree_ptr = tree_node; 
    }
    break;

  case 390:
#line 3660 "parser.y"
    { /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Counter32";
            tree_node->type = Counter32;

            yyval.tree_ptr = tree_node; 
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** ERROR *** \"Counter32\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"Counter32\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    break;

  case 391:
#line 3688 "parser.y"
    { /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Gauge32";
            yyval.tree_ptr = tree_node; 
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** ERROR *** \"Gauge32\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"Gauge32\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    break;

  case 392:
#line 3708 "parser.y"
    { /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Gauge";
            yyval.tree_ptr = tree_node; 

            /*
            if we are using SMIv2, this is an error, otherwise
            set the flag for SMIv1
            */
            if(SMIv2Flag == 1) {
                print_warning("*** ERROR *** \"Gauge\" is only used in SMIv1.", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_note("*** NOTE *** \"Gauge\" is only used in SMIv1, setting SMIv1 flag.", lineBuf);
            }

    }
    break;

  case 393:
#line 3729 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Counter";
            yyval.tree_ptr = tree_node; 

            /*
            if we are using SMIv2, this is an error, otherwise
            set the flag for SMIv1
            */
            if(SMIv2Flag == 1) {
                print_warning("*** ERROR *** \"Counter\" is only used in SMIv1.", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_note("*** NOTE *** \"Counter\" is only used in SMIv1, setting SMIv1 flag.", lineBuf);
            }
    }
    break;

  case 394:
#line 3749 "parser.y"
    {        /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Unsigned32";
            yyval.tree_ptr = tree_node;
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** ERROR *** \"Unsigned32\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"Unsigned32\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    break;

  case 395:
#line 3765 "parser.y"
    {        /* (0..4294967295)         */
            /* this is an outdated version not in the language anymore */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Unsigned32";
            yyval.tree_ptr = tree_node;

            print_warning("*** ERROR *** \"UInteger32\" is not used anymore, use \"Unsigned32\".", lineBuf);

            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** ERROR *** \"Unsigned32\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"Unsigned32\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    break;

  case 396:
#line 3795 "parser.y"
    {        /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "TimeTicks";
            yyval.tree_ptr = tree_node; 
    }
    break;

  case 397:
#line 3817 "parser.y"
    {            /* IMPLICIT OCTET STRING     */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Opaque";
            yyval.tree_ptr = tree_node; 
    }
    break;

  case 398:
#line 3826 "parser.y"
    {            /* (0..18446744073709551615) */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Counter64";
            tree_node->type = Counter64;

            yyval.tree_ptr = tree_node; 
            /*
            if we are using SMIv1, this is an error, otherwise
            set the flag for SMIv2
            */
            if(SMIv1Flag == 1) {
                print_warning("*** ERROR *** \"Counter64\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"Counter64\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }
    }
    break;

  case 399:
#line 3847 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
	    tree_node->type = INTEGER;
            tree_node->string = "INTEGER";
            yyval.tree_ptr = tree_node;
    }
    break;

  case 400:
#line 3857 "parser.y"
    {        /* (SIZE (0..65535))         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = BIT;
            tree_node->string = "BIT STRING";
            yyval.tree_ptr = tree_node;

            print_warning("*** ERROR *** \"BIT STRING\" is no longer a legal type.", lineBuf);
    }
    break;

  case 401:
#line 3869 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "ObjectID";
            tree_node->type   = IDENTIFIER;
            yyval.tree_ptr = tree_node;
    }
    break;

  case 402:
#line 3879 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = SIZE;
            tree_node->string = "SIZE";
            yyval.tree_ptr = tree_node; 

    }
    break;

  case 403:
#line 3890 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "ObjectSyntax";
            yyval.tree_ptr = tree_node; 

    }
    break;

  case 404:
#line 3907 "parser.y"
    { yyval.tree_ptr = yyvsp[0].tree_ptr; }
    break;

  case 405:
#line 3908 "parser.y"
    { 
		yyval.tree_ptr = yyvsp[0].tree_ptr; 
    }
    break;

  case 406:
#line 3911 "parser.y"
    { yyval.tree_ptr = yyvsp[0].tree_ptr; }
    break;

  case 407:
#line 3912 "parser.y"
    { yyval.tree_ptr = yyvsp[-1].tree_ptr; }
    break;

  case 408:
#line 3913 "parser.y"
    { yyval.tree_ptr = yyvsp[-2].tree_ptr; }
    break;

  case 414:
#line 3928 "parser.y"
    { yyval.tree_ptr = yyvsp[-1].tree_ptr; }
    break;

  case 415:
#line 3932 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            yyval.tree_ptr = tree_node;
    }
    break;

  case 416:
#line 3937 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            yyval.tree_ptr = tree_node;
    }
    break;

  case 417:
#line 3942 "parser.y"
    { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            yyval.tree_ptr = tree_node;
    }
    break;

  case 421:
#line 3956 "parser.y"
    {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            yyval.tree_ptr = tree_node;
    }
    break;

  case 422:
#line 3961 "parser.y"
    { yyval.tree_ptr = yyvsp[0].tree_ptr; }
    break;


    }

/* Line 993 of yacc.c.  */
#line 6463 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 3968 "parser.y"


int main(int argc, char * argv[])
{
    int i;
    int parseReturnValue;

    tree * tmpTreePtr  = NULL;
    tree * defLoc      = NULL, * thisLoc = NULL;

    char * inputFileName             = NULL;
    char * tmpCharPtr                = NULL;
    char * aux_dat_filename          = NULL;
    char * mibLocEnvironmentVariable = NULL;

    /* *********************** */
    /* MALLOC BUFFERS & TABLES */
    /* *********************** */

    lineBuf = (char *)malloc(MYBUFSIZE*sizeof(char));
    if(lineBuf) {
        memset((void *)lineBuf,        0, MYBUFSIZE );
    } else {
        fprintf(stderr, "*** ERROR ***  Compiler abort - no memory for lineBuf!\n");
        exit(1);
    }

    stringBuf = (char *)malloc(2*MYBUFSIZE*sizeof(char));
    if(stringBuf) {
        memset((void *)stringBuf,      0, stringBufSize );
        stringBufSize = 2*MYBUFSIZE;
    } else {
        fprintf(stderr, "*** ERROR ***  Compiler abort - no memory for stringBuf!\n");
        exit(1);
    }

    workBuf = (char *)malloc(MYBUFSIZE*sizeof(char));
    if(workBuf) {
        memset((void *)workBuf,        0, MYBUFSIZE );
    } else {
        fprintf(stderr, "*** ERROR ***  Compiler abort - no memory for workBuf!\n");
        exit(1);
    }

    fileNameBuffer = (char *)malloc(MYBUFSIZE*sizeof(char));
    if(fileNameBuffer) {
        memset((void *)fileNameBuffer, 0, MYBUFSIZE );
    } else {
        fprintf(stderr, "*** ERROR ***  Compiler abort - no memory for fileNameBuffer!\n");
        exit(1);
    }

    firstLineBuf   = (char *)malloc(MYBUFSIZE*sizeof(char));
    if(firstLineBuf) {
        memset((void *)firstLineBuf, 0, MYBUFSIZE );
    } else {
        fprintf(stderr, "*** ERROR ***  Compiler abort - no memory for firstLineBuf!\n");
        exit(1);
    }

    /* for all parses */
    allParsesTypeTable  = malloc_table(allParsesTypeTable);

    mibReplacementTable = malloc_table(mibReplacementTable);

    /* single parse only */
    virtualTables = malloc_table(virtualTables);
    emittedNodes  = malloc_table(emittedNodes);
    singleParseTypeTable     = malloc_table(singleParseTypeTable);
    importedMIBs  = malloc_table(importedMIBs);

    /* *************************** */
    /* INITIALIZE LINKED LIST HEAD */
    /* *************************** */

     llroot = NULL; 
     xml_mib = NULL;

    /* ************************** */
    /* GET COMMAND LINE ARGUMENTS */
    /* ************************** */

    /*
    for(i = 0 ; i < argc ; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    */

    /* get command-line arguments */
    if(argc > 1) {
        
        for(i = 1 ; i < argc ; i++) {

            /* scan for option */
            if(argv[i][0] == '-') {

                /* check for output filename option*/
                if( (argv[i][1] =='o' || argv[i][1] =='O' ) && (argc > (i+1))
			&& (argv[i][2] =='\0')  ) {

                    outputFileName = strdup(argv[i+1]);
                    explicitOutputFile = 1;
                    globalMultipleOutputFilesFlag = 0;
                    i++;

                /* check for c++ comment flag */
                } else if( (argv[i][1] =='c'  || argv[i][1] =='C' ) 
			&& (argv[i][2] =='\0')) {
                    globalCppCommentFlag = 1;

                /* check for description flag */
                } else if( (argv[i][1] =='d' || argv[i][1] =='D')
			&& (argv[i][2] =='\0')) {
                    globalProduceDescrFlag = 1;

                } else if( (argv[i][1] =='f'  || argv[i][1] =='F') && 
			(argc > (i+1)) && (argv[i][2] =='\0') ) {

                    outputDirectoryName = strdup(argv[i+1]);
                    i++;

                /* check for help flag */
                } else if( (argv[i][1] =='h' || argv[i][1] =='H')
			&& (argv[i][2] =='\0')) {
                    helpFlag = 1;

                /* check for help flag */
                } else if( (argv[i][1] =='-')  && (argv[i][2] =='h' || argv[i][2] =='H') && (argv[i][3] =='\0')) {

                    helpFlag = 1;
                /* check for alternate format */
                } else if( (argv[i][1] =='t' || argv[i][1] =='T') &&
                            (argc > (i+1)) && (argv[i][2] =='\0')) {

                    if( !strcmp("xml", argv[i+1])) {
                         formatFlag = XML;
                         if(!explicitOutputFile){  
                            outputFileName = strdup("mibgcinfo.xml");
                         }
                         globalMultipleOutputFilesFlag = 0;
                    } else if( !strcmp("mibguide", argv[i+1])) {
                         formatFlag = XML;
                         if(!explicitOutputFile){  
                            outputFileName = strdup("mibgcinfo.xml");
                         }
                         globalMultipleOutputFilesFlag = 0;
                    } else if( !strcmp("idat", argv[i+1])) {
                         formatFlag = IDAT;
                         outputFileName = strdup("mibgcinfo.dat");
                         globalMultipleOutputFilesFlag = 0;
                         
                    } else {
                        fprintf(stderr, "invalid option: %s. Use -h or -H "
					"for help.\n", argv[i+1]);
                        exit(1);
                    }
                    i++;

                /* check for help flag */
               } else if( (argv[i][1] =='-') && 
                          (argv[i][2] =='t' || argv[i][2] =='T') && 
                          (argc > (i+1)) && (argv[i][3] =='\0')) {

                    if( !strcmp("xml", argv[i+1])) {
                         formatFlag = XML; 
                         outputFileName = strdup("info.xml");
                         globalMultipleOutputFilesFlag = 0;
                    } else if( !strcmp("mibguide", argv[i+1])) {
                         formatFlag = XML; 
                         outputFileName = strdup("info.xml");
                         globalMultipleOutputFilesFlag = 0;
                    } else if( !strcmp("idat", argv[i+1])) {
                         formatFlag = IDAT;
                         outputFileName = strdup("info.dat");
                         globalMultipleOutputFilesFlag = 0;
                    } else {
                        fprintf(stderr, "invalid option: %s. Use -h or -H "
				"for help.\n", argv[i+1]);
                        exit(1);
                    }
                    i++;

                /* check for rename input file option */
                } else if( (argv[i][1] =='r' || argv[i][1] =='R' )
			&& (argv[i][2] =='\0')) {
                    globalRenameFlag = 1;

                /* check for backup output file option */
                } else if( (argv[i][1] =='b' || argv[i][1] =='B' )
			&& (argv[i][2] =='\0')) {
                    globalBackupOutputFlag = 1;

                /* check for suppress output option */
                } else if( (argv[i][1] =='s' || argv[i][1] =='S' )
			&& (argv[i][2] =='\0')) {
                    globalSuppressOutputFlag = 1;

		/* check for verbose output option */
                } else if( (argv[i][1] =='v' || argv[i][1] =='V' )
			&& (argv[i][2] =='\0')) {
                    globalVerboseOutputFlag = 1;
		
		/* check for parse only option */
                } else if( (argv[i][1] =='x' || argv[i][1] =='X' )
			&& (argv[i][2] =='\0')) {
                    globalParseOnlyFlag = 1;
		
		/* check for ignore strict compliance option */
                } else if( (argv[i][1] =='i' || argv[i][1] =='I' )
			&& (argv[i][2] =='\0')) {
                    globalIgnoreStrictComplianceFlag = 1;
		

                /* check for automatic update option */
                } else if( (argv[i][1] =='u' || argv[i][1] =='U' )
			&& (argv[i][2] =='\0')) {
                    updateImportsFlag = 1;

                /* check for mib import search directories */
                } else if( (argv[i][1] =='m' || argv[i][1] =='M')   
			&& (argc > (i+1)) && (argv[i][2] =='\0') ) {

                    memset(lineBuf, 0, MYBUFSIZE );
                    strcpy(lineBuf, argv[i+1]);

                    tmpCharPtr = strtok(lineBuf, SEPARATOR);

                    while(tmpCharPtr) {

                        tree * tree_node = NULL;
                        tree_node = malloc_node(tree_node, SAVE_FOR_ALL_PARSES);
                        tree_node->string = strdup(tmpCharPtr);
                        tree_node->freeMe = 1;

                        /* insert node at end of list */
                        if(globalImportDirectoryList == NULL) {
                            globalImportDirectoryList = tree_node;
                            globalImportDirectoryList->last_tree_node_in_list = tree_node;
                        } else {
                            globalImportDirectoryList->last_tree_node_in_list->next_tree_node = tree_node;
                            globalImportDirectoryList->last_tree_node_in_list = tree_node;
                        }

                        tmpCharPtr = strtok(NULL, SEPARATOR);
                    }    

                    i++;

                /* check for alternate format */
                } else if( (argv[i][1] =='f' && argv[i][2] =='t')  
			&& (argc > (i+1)) && (argv[i][2] =='\0') ) {
                 memset(lineBuf, 0, MYBUFSIZE );
                 strcpy(lineBuf, argv[i+1]);

                 fprintf(stderr, "recognized alternate format: %s\n", argv[i]);
                 if( !strcmp("xml", argv[i+1])) {
                      formatFlag = XML; 
                      outputFileName = strdup("info.xml");
                      globalMultipleOutputFilesFlag = 0;
                 } else if( !strcmp("mibguide", argv[i+1])) {
                      formatFlag = XML; 
                      outputFileName = strdup("info.xml");
                      globalMultipleOutputFilesFlag = 0;
                 } else if( !strcmp("idat", argv[i+1])) {
                      formatFlag = IDAT;
                      outputFileName = strdup("info.dat");
                      globalMultipleOutputFilesFlag = 0;
                  } else {
                        fprintf(stderr, "invalid option: %s. Use -h or -H "
				"for help.\n", argv[i+1]);
                        exit(1);

                  }
                    i++;
                } else {
                    fprintf(stderr, "invalid option: %s. Use -h or -H "
				"for help.\n", argv[i]);
		    exit(1);
                }

            /* any name after argv[0] that is not an option must be an input filename */
            } else {

                tree * tree_node = NULL;
                tree_node = malloc_node(tree_node, SAVE_FOR_ALL_PARSES);
                tree_node->string = strdup(argv[i]);
                tree_node->freeMe = 1;
              
/* FAM Added to compare file name to generate error if file does not exist */
                /*if (formatFlag == XML) {*/
                    xml_mib = strdup(argv[i]);
                /*}*/

                /* insert node at end of list */
                if(globalInputFilenameList == NULL) {
                    globalInputFilenameList = tree_node;
                    globalInputFilenameList->last_tree_node_in_list = tree_node;
                } else {
                    globalInputFilenameList->last_tree_node_in_list->next_tree_node = tree_node;
                    globalInputFilenameList->last_tree_node_in_list = tree_node;
                }

            }

        } /* end for loop */

    } /* end if argc > 1*/

    /* print usage if no arguments or helpFlag set */
    if((argc == 1) || helpFlag) {

        fprintf(stderr, "\nmibgc compiles SNMP SMIv1 and SMIv2 MIBs to produce\n");
	fprintf(stderr, "mosy-compatible .def or .dat files.\n\n");
#ifndef SR_WINDOWS
        fprintf(stderr, "USAGE: mibgc [-b|B] [-c|C] [-d|D] [-f|F outputdirectory] [-h|H] [-i|I]\n");
	fprintf(stderr, "           [-m|M mibFileDir:mibFileDir:...] [-o|O outputfilename] [-r|R]\n");
	fprintf(stderr, "           [-s|S] [-t|T idat] [-u|U] [-v|V] [-x|X] inputFileNames\n\n");
/*******fprintf(stderr, "       (e.g. mibgc *.my -o outfile)\n\n");*/
#else
        fprintf(stderr, "USAGE: mibgc [-b|B] [-c|C] [-d|D] [-f|F outputdirectory] [-h|H] [-i|I]\n");
	fprintf(stderr, "           [-m|M mibFileDir;mibFileDir;...] [-o|O outputfilename] [-r|R]\n");
	fprintf(stderr, "           [-s|S] [-t|T idat] [-u|U] [-v|V] [-x|X] inputFileNames\n\n");
/*******fprintf(stderr, "       (e.g. mibgc *.my -o outfile)\n\n");*/
#endif /* SR_WINDOWS */
        fprintf(stderr, "   FLAGS (use either upper or lower case:\n\n");
        fprintf(stderr, "   -b -B   - Backup any existing output files.\n");
        fprintf(stderr, "   -c -C   - Treat comments like C++ comments.\n");
        fprintf(stderr, "   -d -D   - Produce description file.\n");
        fprintf(stderr, "   -f -F   - Directory to dump output files\n");
#ifndef SR_WINDOWS
	fprintf(stderr, "             (e.g. ../mibgc *.my -s -f ~/dump)\n");
#else
        fprintf(stderr, "             (e.g. ../mibgc *.my -s -f C:\\dump)\n");
#endif
        fprintf(stderr, "   -h -H   - Show help information (also --h or --H).\n");
        fprintf(stderr, "   -i -I   - Ignore strict compliance.\n");
#ifndef SR_WINDOWS
        fprintf(stderr, "   -m -M   - Directories (separated by colons) to search for.\n");
#else
        fprintf(stderr, "   -m -M   - Directories (separated by semicolons) to search for\n");
#endif /* SR_WINDOWS */
	fprintf(stderr, "             AUX-MIB.dat and imported MIBs.\n");
        fprintf(stderr, "   -o -O   - Produce all compiled MIBs in the single file specified.\n");
        fprintf(stderr, "             If no file is specified, each MIB is put in a separate file\n");
        fprintf(stderr, "             named with the base input file plus .def.\n");
        fprintf(stderr, "   -r -R   - Rename root of input filename to match module name.\n");
        fprintf(stderr, "   -s -S   - Suppress warning output.\n");
        fprintf(stderr, "   -t -T   - Generate info.dat output. Specify \"-t idat\".\n");
        fprintf(stderr, "   -u -U   - Automatically replace/update obsolete imports.\n");
	fprintf(stderr, "             e.g. RFC1065-SMI --> RFC1155-SMI\n");
        fprintf(stderr, "   -v -V   - Prints additional information.\n");
        fprintf(stderr, "   -x -X   - Parses MIB without any conversion output to stdout or file.\n\n");

/************* Comment out for now ******************
        fprintf(stderr, "   COMMAND LINE ARGUMENTS:\n");
        fprintf(stderr, "   ~~~~~~~~~~~~~~~~~~~~~~\n");
        fprintf(stderr, "      order      - order of command-line arguments is irrelevant.\n");
#ifndef SR_WINDOWS
        fprintf(stderr, "      spaces     - enclose argument with quotes if using spaces\n");
	fprintf(stderr, "                   e.g. -m \"test dir/mibs\".\n");
#else
        fprintf(stderr, "      spaces     - enclose path with quotes if using spaces\n");
	fprintf(stderr, "                   e.g. -m \"C:\\Program Files\\mibs\"\n");
#endif 
        fprintf(stderr, "      wildcards  - (*,?) are expanded e.g. \"*.my\" becomes all files that end\n");
	fprintf(stderr, "                   in \"my\"\n\n");
******************************************************/

        fprintf(stderr, "   SEARCH ORDER for AUX-MIB.dat and imported MIBs:\n\n");
        fprintf(stderr, "       (1) command-line paths         (first to last - set with -m or -M)\n");
        fprintf(stderr, "       (2) environment-variable paths (first to last - set with SR_MIBS_BBC)\n");
#ifndef SR_WINDOWS
        fprintf(stderr, "       (3) /usr/local/mibs (default mib location)\n");
        fprintf(stderr, "       (4) ./\n\n");
#else
        fprintf(stderr, "       (3) C:\\mibs (default mib location)\n");
        fprintf(stderr, "       (4) current directory\n\n");
#endif /* SR_WINDOWS */
        fprintf(stderr, "   PRECEDENCE of definitions:\n\n");
        fprintf(stderr, "       (1) AUX-MIB.dat,\n"); 
        fprintf(stderr, "       (2) current MIB,\n");
        fprintf(stderr, "       (3) imported MIBs. (The precedence within imported MIBs is first-come,\n");
	fprintf(stderr, "           first-served.)\n\n"); 

/********* Comment out for now ***********************
        fprintf(stderr, "   OVERWRITE PROTECTION:\n");
        fprintf(stderr, "   ~~~~~~~~~~~~~~~~~~~~\n");
        fprintf(stderr, "   If the intended output file already exists then an incrementing three-digit\n");
	fprintf(stderr, "   number is used for file history if the -b or -B flag is set.\n");
	fprintf(stderr, "   i.e. .def always contains the newest compile, .def_000 contains the oldest\n\n");
******************************************************/

    }

    /* do parsing if needed*/
    if((argc > 1) && !helpFlag) {

        /* is there an environment variable set for import mib directories */
        if( (mibLocEnvironmentVariable = (char *)getenv("SR_BBC_MIBS")) != NULL) {

            memset(lineBuf, 0, MYBUFSIZE );
            strcpy(lineBuf, mibLocEnvironmentVariable);

            tmpCharPtr = strtok(lineBuf, SEPARATOR);

            while(tmpCharPtr) {

                tree * tree_node = NULL;
                tree_node = malloc_node(tree_node, SAVE_FOR_ALL_PARSES);
                tree_node->string = strdup(tmpCharPtr);
                tree_node->freeMe = 1;

                /* insert node at end of list */
                if(globalImportDirectoryList == NULL) {
                    globalImportDirectoryList = tree_node;
                    globalImportDirectoryList->last_tree_node_in_list = tree_node;
                } else {
                    globalImportDirectoryList->last_tree_node_in_list->next_tree_node = tree_node;
                    globalImportDirectoryList->last_tree_node_in_list = tree_node;
                }

                tmpCharPtr = strtok(NULL, SEPARATOR);
            }    

        }


        /* load replacement MIBs for imports */
        load_mib_replacement_table();

        /*
        dump_table1(mibReplacementTable);
        */

        /* add default import mib location, then ./ to import path */
        defLoc = malloc_node(defLoc, 0);

#ifndef SR_WINDOWS
        defLoc->string = strdup("/usr/local/mibs");
#else
        defLoc->string = strdup("C:\\mibs");
#endif /* SR_WINDOWS */

        defLoc->freeMe = 1;

        /* insert node at end of list */
        if(globalImportDirectoryList == NULL) {
            globalImportDirectoryList = defLoc;
            globalImportDirectoryList->last_tree_node_in_list = defLoc;
        } else {
            globalImportDirectoryList->last_tree_node_in_list->next_tree_node = defLoc;
            globalImportDirectoryList->last_tree_node_in_list = defLoc;
        }

        thisLoc = malloc_node(thisLoc, 0);
        thisLoc->string = strdup(".");
        thisLoc->freeMe = 1;

        /* insert node at end of list */
        globalImportDirectoryList->last_tree_node_in_list->next_tree_node = thisLoc;
        globalImportDirectoryList->last_tree_node_in_list = thisLoc;


        /* print out list of import directories */
        if((!globalSuppressOutputFlag) && (globalVerboseOutputFlag)) {
            fprintf(stderr, "import directories to search:\n");

            tmpTreePtr = globalImportDirectoryList;
            while(tmpTreePtr && tmpTreePtr->string) {
                printf("%s\n", tmpTreePtr->string);
                tmpTreePtr = tmpTreePtr->next_tree_node;
            }
        }

        /* do import parse of aux definitions */
        /* an import parse doesn't produce any output code,
           just puts the data into the type table */
        /* try each of the import paths in turn */

        tmpTreePtr = globalImportDirectoryList;
        while(tmpTreePtr && tmpTreePtr->string) {

            /* import the file */
            memset(lineBuf, 0, MYBUFSIZE );
            strcpy(lineBuf, tmpTreePtr->string);

#ifndef SR_WINDOWS
            strcat(lineBuf, "/");
#else
            strcat(lineBuf, "\\");
#endif /* SR_WINDOWS */
            strcat(lineBuf, "AUX-MIB.dat");

            /* ************************** */
            aux_dat_filename = strdup(lineBuf);

            /* do import parse, but save type nodes for all parses */
            saveForAllParsesFlag = 1;
            openOutputFlag = 1;
            parseReturnValue = parse_file(IMPORT_PARSE, aux_dat_filename, "AUX-MIB.dat");
            saveForAllParsesFlag = 0;

            if(aux_dat_filename) free(aux_dat_filename);
            aux_dat_filename = NULL;
            /* ************************** */

            /* if we have successfully parsed the file, then quit looking */
            if(!parseReturnValue) {
                break;
            }

            tmpTreePtr = tmpTreePtr->next_tree_node;
        }


        /* ******************************* */
        /* PROCESS EACH INPUT FILE IN TURN */
        /* ******************************* */

        tmpTreePtr = globalInputFilenameList;
        while(tmpTreePtr && tmpTreePtr->string) {

            tree * name_node = NULL, * tree_node = NULL;

            /* ************************** */
            inputFileName = tmpTreePtr->string;

	    if (formatFlag == XML) { 
	    /* check to see if there are other MIBs in the list. 
 	     * if so, set the global flag so that we can handle the XML 
	     * output accordingly
	     */
		if(tmpTreePtr->next_tree_node && 
			tmpTreePtr->next_tree_node->string) { 
		  /* More than one MIB in the list. Set the global */
		  multiple_mibs = 1;
		}
		else {
		  /* set flag indicating this is the last parse */
		  last_parse = 1;
		}
	    }

            /* do full parse, not import parse, of next file in list to parse */
            parseReturnValue = parse_file(NORMAL_PARSE, inputFileName, inputFileName);

	    /* set global to let other methods know that the first parse is 
             * done
      	     */
	    first_parse = 0;

            /* ************************** */

            /* add self to list of imported MIBs, so
               this file is not imported during this parse */
            name_node = malloc_node(name_node, SINGLE_PARSE);
            name_node->string = strdup(inputFileName);
            name_node->freeMe = 1;

            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[0] = name_node;

            insert_type_node( tree_node, importedMIBs);

            /* **************************************** */
            /* CLOSE ONE GROUP OF MULTIPLE OUTPUT FILES */
            /* **************************************** */

            if(globalMultipleOutputFilesFlag) {

                /* ********************** */
                /* close description file */
                /* ********************** */

                if(globalProduceDescrFlag) {
                    if(descrFilePtr) fclose(descrFilePtr);
                    if(descrFileName) free(descrFileName);
                    descrFileName = NULL;
                }

                /* ********************************* */
                /* close mosy compatible output file */ 
                /* ********************************* */

                if(outputFilePtr) { 	
	   	   fclose(outputFilePtr);
		   outputFilePtr = NULL;
	 	}
                if(outputFileName) {
		   free(outputFileName);
		   outputFileName = NULL;
		}

            }


            /* RESET FOR EACH INPUT FILE PROCESSED */

            maxColumnWidth = MAX_COLUMN_WIDTH;

            /* reset imports for next parse to NULL */
            importFromList = NULL;

            /* free old tree for new parse */
            if(singleParseNodeList) {
                free_all_nodes(singleParseNodeList);
                singleParseNodeList = NULL;
            }

            /* reset tables and emitted nodes for each new parse */
            memset((void *)virtualTables,  0, TABLE_SIZE*sizeof(tree *));
            memset((void *)emittedNodes, 0, TABLE_SIZE*sizeof(tree *));
            memset((void *)singleParseTypeTable, 0, TABLE_SIZE*sizeof(tree *));
            memset((void *)importedMIBs, 0, TABLE_SIZE*sizeof(tree *));

            tmpTreePtr = tmpTreePtr->next_tree_node;

            /* clean up linked list */

            if (llroot) {
               cleanup_ll();
            }
        }


        /* ************************* */
        /* CLOSE SINGLE OUTPUT FILE  */
        /* ************************* */
        /* if there is a single output file, then close it */

        if(!globalMultipleOutputFilesFlag) {

            /* ********************** */
            /* close description file */
            /* ********************** */

            if(globalProduceDescrFlag) {
                if(descrFilePtr) {
		  fclose(descrFilePtr);
		  descrFilePtr = NULL;
		}
                if(descrFileName) {
		  free(descrFileName);
                  descrFileName = NULL;
		}
            }

            /* ********************************* */
            /* close mosy compatible output file */ 
            /* ********************************* */

            if(outputFilePtr) {
	      fclose(outputFilePtr);
	      outputFilePtr = NULL;
	    }
            if(outputFileName) {
	      free(outputFileName);
              outputFileName = NULL;
	    }

            /* **************************** */
            /* FREE TREE NODES IN FREE LIST */
            /* **************************** */
            if(singleParseNodeList) {
                free_all_nodes(singleParseNodeList);
                singleParseNodeList = NULL;
            }

        }

        /* ************************ */
        /* FREE BUFFERS AND TABLES */
        /* ************************ */

        free(lineBuf);
        free(stringBuf);
        free(workBuf);
        free(fileNameBuffer);
        free(firstLineBuf);

        /* the nodes in these tables are freed at end of each parse */
        free(virtualTables);
        free(emittedNodes);
        free(singleParseTypeTable);
        free(importedMIBs);

        /* free all the nodes in the allParsesTypeTable plus lists */
        if(allParsesNodeList) {
            free_all_nodes(allParsesNodeList);
            allParsesNodeList = NULL;
        }

        free(allParsesTypeTable);

        if(lastFileParsed) {
            free(lastFileParsed);
        }

    }

    if(!globalExitErrorFlag) {
      return(0);
    }
    else {
      return(1);
    }

}

/* ============================================================ */

int 
parse_file(int importFlag, char * myInputFileName, char * partialFileName)
{
    FILE * tmpFilePtr;
    char errstring[80];
    int i;
    int parserReturnValue = 0;

    int fileCounter = 0;

    char * ptr;
    char * localInputFileName  = NULL;
    char * inputPrefix         = NULL;
    char * tmpOutputFileName = NULL;

    tree * mibAlreadyImportedPtr;

    /* SAVE STATE for things needed when output produced */
    tree * temp_root                  = root;
    int temp_SMIv1Flag                = SMIv1Flag;
    int temp_SMIv2Flag                = SMIv2Flag;
    int temp_SMIv3Flag                = SMIv3Flag;
    int temp_sawObjectIdentifier      = sawObjectIdentifier;

    tree * temp_importFromList;



    /* reset for error reporting */
    lastModuleName   = NULL;

    /* tell the parsing routines if this is an import,
       so don't display messages */
    importParseFlag = importFlag;

    /* set defaults for each new parse */
    line_cntr                = 1;
    col_cntr                 = 0;
    no_code_flag             = 0;
    string_line_cntr         = 0;
    stringWarnOn             = 0;
    root                     = NULL;

    if(lastFileParsed) {
        free(lastFileParsed);
    }

    /* for tracking errors */
    lastFileParsed = strdup(myInputFileName); 

    /* clear all the buffers */
    memset((void *)lineBuf,        0, MYBUFSIZE );
    memset((void *)stringBuf,      0, stringBufSize );
    memset((void *)workBuf,        0, MYBUFSIZE );
    memset((void *)fileNameBuffer, 0, MYBUFSIZE );
    memset((void *)firstLineBuf,   0, MYBUFSIZE );


    /* have we already parsed this file */
    mibAlreadyImportedPtr = lookup_type(myInputFileName, importedMIBs);

    if(mibAlreadyImportedPtr) {
        parserReturnValue = 1;
        goto restore_point;
    }


    /* OPEN INPUT FILE */
    if(  !(yyin = fopen(myInputFileName, "r"))  ) {

        if(!globalSuppressOutputFlag && globalVerboseOutputFlag) {
            fprintf(stderr, "*** NOTE ***  Could not open file for parsing: \"%s\".\n\n", myInputFileName);
        }

/* FAM - code added to print error if file doesn't exist for XML processing */
/* xml_mib used so that we do not exit for import mibs */

        /*if (formatFlag == XML) {*/
           if (myInputFileName && xml_mib) {
               if(!strcmp(myInputFileName, xml_mib)){
                     sprintf(errstring, "mibgc: %s: ", xml_mib);
                     perror(errstring);
                     exit(1);
               }
           }
        /*}*/

        parserReturnValue = 1;
        goto restore_point;
    } 

    if((!globalSuppressOutputFlag) && (globalVerboseOutputFlag)) {
        fprintf(stderr, "*** NOTE ***  Opening file for parsing: \"%s\".\n", myInputFileName);
    }

    /* do the actual parse */
    parserReturnValue = yyparse();

    /* *** EXIT*** */
    if(parserReturnValue) {
        exit(parserReturnValue);

        /* printf("parserreturnValue: %d\n", parserReturnValue); */

    }

    /* CLOSE INPUT FILE */
    fclose(yyin);

    /* GENERATE OUTPUT CODE */
    if(!root || no_code_flag || globalFatalErrorFlag) {
        fprintf(stderr, "\n*** Warning - errors ***\n");
        fprintf(stderr, "\nNo output code generated\n\n");
	parserReturnValue = 1;
	/* set the globalFatalErrorflag back to 0 in case more files */
        globalFatalErrorFlag = 0;
	goto restore_point;
    } 
    else if(globalParseOnlyFlag) {
	/* parse, but do not output file */
	goto restore_point;
    } 
    else {

        /* ************* */
        /* WRITE OUTPUT  */
        /* ************* */

        /* only open output file if not import */
        if(!importFlag) {

            /* 
             * If we don't have an output file name, but we do
             * have an input filename, then create the output filename
             * by adding .def to the root of the input filename.
             */

            if( myInputFileName && !outputFileName) {

                memset((void *)fileNameBuffer, 0, MYBUFSIZE);

                /* preface with outputDirectoryName if it exists */
                if(outputDirectoryName) {
                    strncpy(fileNameBuffer, outputDirectoryName, MYBUFSIZE/2);
#ifndef SR_WINDOWS
                    strcat(fileNameBuffer, "/");
#else
                    strcat(fileNameBuffer, "\\");
#endif
                    strcat(fileNameBuffer, myInputFileName);
                } else {
                    strcpy(fileNameBuffer, myInputFileName);
                }

                /* start at end of string to get root of input filename */
                i = strlen(fileNameBuffer) - 1;
                ptr = (char *)(fileNameBuffer+i);
                while( (*ptr != '.') && (i > 0) ) {
                  i--;
                  ptr--;
                }

                if(i > 0) {
                    *ptr = '\0';
                }

                strcat(fileNameBuffer, ".def");

	        /* now search for and strip off any proceeding directories */

		if(!(tmpOutputFileName = strrchr(fileNameBuffer, '/'))) {
		  /* Try with double backslashes */
		  if(!(tmpOutputFileName = strrchr(fileNameBuffer, '\\'))) {
		    /* Still nothing. Just set to fileNameBuffer */
		    tmpOutputFileName = fileNameBuffer;
		  } else {
                    /* strip off last '\' */
                    tmpOutputFileName = tmpOutputFileName + 1;
		  }
 	        }
		else {
 		  /* strip off last '/' */
		  tmpOutputFileName = tmpOutputFileName + 1;
		}

                outputFileName = strdup(tmpOutputFileName);

                /* check for description flag */
                if( globalProduceDescrFlag) {
                    fileNameBuffer[strlen(outputFileName)-1] = 's';
                    descrFileName  = strdup(fileNameBuffer); 
                }

            }

            if(globalMultipleOutputFilesFlag) {

                open_output_file(outputFileName);

                /* write header if not import */
                write_header(outputFilePtr);

            } else if(openOutputFlag){

                open_output_file(outputFileName);
                /* write header if not import */
                if ( (formatFlag != IDAT) || (formatFlag != XML) ) {
                   write_header(outputFilePtr);
                }
                openOutputFlag = 0;

            }

        }        

        /* OPEN DESCRIPTION FILE */
        if(!importFlag && globalProduceDescrFlag) {
            /* open the description output file */
            if((descrFilePtr = fopen(descrFileName, "a")) == NULL) {
                fprintf(stderr, "\n*** ERROR ***  Can't open \"%s\" for writing.\n", descrFileName);
                exit(1);
            }

        }

        /* ***************************** */
        /* RENAME INPUT FILES IF NEEDED  */
        /* ***************************** */

        /* only rename if not import */
        if(!importFlag && globalRenameFlag) {

            memset(lineBuf, 0, MYBUFSIZE );
            strcpy(lineBuf, myInputFileName);

            /* start at end of string to get prefix of input filename */
            i  = strlen(lineBuf) - 1;
            ptr = (char *)(lineBuf+i);
            while( (*ptr != '/') && (i >= 0) ) {
                i--;
                *ptr = '\0';
                ptr--;
            }

            inputPrefix = strdup(lineBuf);

            strcat(lineBuf, lastModuleName);
            strcat(lineBuf, ".my");

            /* module name doesn't match inputfilename, so rename */
            if( strcmp(lineBuf, myInputFileName) ) {

                localInputFileName = strdup(lineBuf);

                memset(fileNameBuffer, 0, MYBUFSIZE);
                sprintf(fileNameBuffer, "%s%s", inputPrefix, localInputFileName);

                /* does the output file already exist? */
	        while( (tmpFilePtr = fopen(fileNameBuffer, "r")) != NULL) {

                    if((!globalSuppressOutputFlag) && (globalVerboseOutputFlag) 
 			  && (!globalBackupOutputFlag)) {
                        fprintf(stderr, "\n*** NOTE ***  Output file \"%s\" already exists. Overwriting...\n", fileNameBuffer);
                    }

                    memset(fileNameBuffer, 0, MYBUFSIZE);
                    sprintf(fileNameBuffer, "%s%s_%03d", inputPrefix, localInputFileName, fileCounter++);

                    fclose(tmpFilePtr);

	        }

                /* rename to next free _??? counter */

                memset(lineBuf, 0, MYBUFSIZE );
#ifndef SR_WINDOWS
                strcpy(lineBuf, "mv ");
#else
                strcpy(lineBuf, "rename ");
#endif /* SR_WINDOWS */
                strcat(lineBuf, myInputFileName);
                strcat(lineBuf, " ");
                strcat(lineBuf, fileNameBuffer);

                system(lineBuf);


                myInputFileName = localInputFileName;

            }

        }

        /* DEAL WITH IMPORTS FOR THIS FILE */
        temp_importFromList = importFromList;
        importFromList = NULL;

        if((!globalSuppressOutputFlag) && (globalVerboseOutputFlag)) {
            fprintf(stderr,"\n*** NOTE ***  Resolving imports for %s.\n\n", partialFileName);
        }

        do_imports(temp_importFromList, partialFileName );

        /* print mosy code for tree */
        if(!importFlag) {

/* FAM ***** this is where the bulk of the .def file is emitted ******/

        if (formatFlag == IDAT) {

	  if(!globalFatalErrorFlag) {
              ll_emit_all_nodes(root, outputFilePtr);

	      sort_data();

              print_ll(outputFilePtr);

	      sorted_root = NULL;
	  }
	  else {
	    /* set the flag back to 0 in case more files are to be parsed */
	    if(outputModuleName) {
	      fprintf(stderr, "Failed to parse %s\n\n", outputModuleName);
 	      free(outputModuleName);
	      outputModuleName = NULL;
            }
	    globalFatalErrorFlag = 0;
 	  }
        } 

/* BAB ***** this is where the .xml file is emitted *****/
        else if(formatFlag == XML) {
	  if(!globalFatalErrorFlag) {
            ll_emit_all_nodes(root, outputFilePtr);

	    /* check to see if any errors during the emit node phase */
	    if(postParseErrorFlag) {

#ifdef NOT_YET
/************************************************************
 * Eventually we need to have mibgc fail when INDEX is undefined
 * but for now we just ignore and move on
 ***********************************************************/
              if(outputModuleName) {
                fprintf(stderr, "Failed to parse %s\n\n", outputModuleName);
                free(outputModuleName);
                outputModuleName = NULL;
              }
#endif /* NOT_YET */

              postParseErrorFlag = 0;
            }

	    else {
              /* now sort the linked list */
              sort_data();

	      /* print xml to file*/
              print_ll_xml(outputFilePtr);
	    }

	    /* set these back to NULL in case we have more passes */
	    sorted_root = NULL;
	    im_root = NULL;
	    tc_root = NULL;
	  }
	  else {
	    /* set the flag back to 0 in case more files are to be parsed */
            if(outputModuleName) {
              fprintf(stderr, "Failed to parse %s\n\n", outputModuleName);
              free(outputModuleName);
              outputModuleName = NULL;
            }
	    globalFatalErrorFlag = 0;
 	  }
        } 

        else {
	  if(!globalFatalErrorFlag) {
              mosy_emit_all_nodes(root, outputFilePtr, descrFilePtr);

	      if(postParseErrorFlag) {
#ifdef NOT_YET
/************************************************************
 * Eventually we need to have mibgc fail when INDEX is undefined
 * but for now we just ignore and move on
 ***********************************************************/
	         if(outputModuleName) {
              	   fprintf(stderr, "Failed to parse %s\n\n", outputModuleName);
              	   free(outputModuleName);
              	   outputModuleName = NULL;
            	 }
#endif /* NOT_YET */

            	 postParseErrorFlag = 0;
              }
	  }
	  else {
            /* set the flag back to 0 in case more files are to be parsed */
            if(outputModuleName) {
              fprintf(stderr, "Failed to parse %s\n\n", outputModuleName);
              free(outputModuleName);
              outputModuleName = NULL;
            }
            globalFatalErrorFlag = 0;
          }
        }
     }
    }

restore_point:

    /* RESTORE STATE */
    root                     = temp_root;
    SMIv1Flag                = temp_SMIv1Flag;
    SMIv2Flag                = temp_SMIv2Flag;
    SMIv3Flag                = temp_SMIv3Flag;
    sawObjectIdentifier      = temp_sawObjectIdentifier;

    /* free local mallocs */
    if(localInputFileName) free(localInputFileName);
    localInputFileName = NULL;

    if(inputPrefix) free(inputPrefix);
    inputPrefix = NULL;

    if(outputModuleName) free(outputModuleName);
    outputModuleName = NULL;

    return parserReturnValue;
}

/* ============================================================ */

void 
do_imports(tree * local_importFromList, char * fileDoingImport)
{
    int parseReturnValue         = 0;

    tree * mibToImport           = NULL;
    tree * mibAlreadyImportedPtr = NULL;
    tree * directoryListPtr      = NULL;
    char * fileToImportPtr       = NULL;
    tree * nextSymbolToImportPtr = NULL;
    tree * nextImportListPtr     = NULL;
    tree * typeExistsPtr         = NULL;
    tree * alreadyEmittedPtr     = NULL;
    tree * tmpTreePtr            = NULL;
    char * importString          = NULL;


    mibToImport = local_importFromList;

    while(mibToImport) {

        importString = mibToImport->string;

        /* if automatic update/replace feature turned on */
        if(updateImportsFlag) {

            /* does an entry already exist */
            tmpTreePtr = lookup_name(importString, mibReplacementTable);
            if(tmpTreePtr) {

                if((!globalSuppressOutputFlag) && (globalVerboseOutputFlag)) {
                    fprintf(stderr, "\n*** NOTE ***  Automatically replacing import of %s with import of %s.\n\n", importString, tmpTreePtr->next_tree_node->string);
                }

                importString = tmpTreePtr->next_tree_node->string;

            }

        }

        if(tmpTreePtr) {
            tmpTreePtr = tmpTreePtr->next_tree_node;
        }

        while(importString) {

            /* check whether already imported */
            mibAlreadyImportedPtr = lookup_type(importString, importedMIBs);

            if(!mibAlreadyImportedPtr) {

                tree * name_node = NULL, * tree_node = NULL;

                name_node = malloc_node(name_node, SINGLE_PARSE);
                name_node->string = strdup(importString);
                name_node->freeMe = 1;

                tree_node = malloc_node(tree_node, SINGLE_PARSE);
                tree_node->child[0] = name_node;

                insert_type_node( tree_node, importedMIBs);


                /* try each of the import paths in turn to find the import file */
                directoryListPtr = globalImportDirectoryList;
                while(directoryListPtr && directoryListPtr->string) {

                    /* import the file */
                    memset(lineBuf, 0, MYBUFSIZE );
                    strcpy(lineBuf, directoryListPtr->string);
#ifndef SR_WINDOWS
                    strcat(lineBuf, "/");
#else
                    strcat(lineBuf, "\\");
#endif /* SR_WINDOWS */
                    strcat(lineBuf, importString);
                    strcat(lineBuf, ".my");


                    /* ************************** */
                    fileToImportPtr = strdup(lineBuf);

                    /* do import parse */
                    parseReturnValue = parse_file(IMPORT_PARSE, fileToImportPtr, importString);


/* IMPORTS EVERY DEF IN IMPORTED MIB */


                    if(fileToImportPtr) free(fileToImportPtr);
                    fileToImportPtr = NULL;
                    /* ************************** */


                    /* if we have successfully parsed the file, then quit looking */
                    if(!parseReturnValue) {
                        break;
                    }

                    directoryListPtr = directoryListPtr->next_tree_node;
                }

            } 


            /* look at the symbols we need to import from this MIB */
            /* point to symbol import list */
            nextSymbolToImportPtr = mibToImport->next_symbol_node;
        
            /* don't bother if no symbols to import */
            if(nextSymbolToImportPtr) {

            if(formatFlag == IDAT){
                fprintf(stderr, " -- %s importing FROM %s\n", fileDoingImport, importString);
            }

            if(formatFlag == 0) {
                fprintf(outputFilePtr, "\n-- %s importing FROM %s\n", fileDoingImport, importString);
                fprintf(outputFilePtr, "-- looking for: ");
            
                /* for each of the imported symbols */
                nextImportListPtr = mibToImport->next_symbol_node;
                while(nextImportListPtr) {
                    fprintf(outputFilePtr, "%s, ", nextImportListPtr->string);
                    nextImportListPtr = nextImportListPtr->next_symbol_node;
                }
                fprintf(outputFilePtr, "\n");
            } /* formatFlag */

                /* for each of the imported symbols */
                while(nextSymbolToImportPtr) {

                    typeExistsPtr = find_type(nextSymbolToImportPtr->string);

                    /* type exists in type table */
                    if(typeExistsPtr) {

                        /* check whether already emitted for this parse */
                        alreadyEmittedPtr = lookup_name_in_emitted_nodes(nextSymbolToImportPtr->string, typeExistsPtr->type);

                        /* printf("4150 checking %s\n", nextSymbolToImportPtr->string); */

                        if(alreadyEmittedPtr) {

                            fprintf(outputFilePtr, "-- %s already emitted\n", nextSymbolToImportPtr->string);
                        /*   printf("4154 %s already emitted\n", nextSymbolToImportPtr->string); */ 

                        /* emit */
                        } else {

                            /* emit type and all parents back to root to output file */
                            while(typeExistsPtr) {

                                /* trace def back to root */
                                if(typeExistsPtr && typeExistsPtr->child[child_value____9] && typeExistsPtr->child[child_value____9]->string) {

                                    typeExistsPtr = find_type(typeExistsPtr->child[child_value____9]->string);
                                } else {
                                    typeExistsPtr = NULL;
                                }

                                alreadyEmittedPtr = NULL;
                                /* check whether already emitted for this parse */
                                if(typeExistsPtr && typeExistsPtr->child[0] && typeExistsPtr->child[0]->string) {
                                    alreadyEmittedPtr = lookup_name_in_emitted_nodes(typeExistsPtr->child[0]->string, typeExistsPtr->type);
                                }

                                if(alreadyEmittedPtr) {
                                    typeExistsPtr = NULL;
                                } 

                            }

                        }

                    } else {
                        if(formatFlag == IDAT){
                              fprintf(stderr, "*** ERROR *** Unable to find imported symbol: %s\n", nextSymbolToImportPtr->string);
                              fprintf(stderr, "Add %s to MIB directory before continuing\n", importString);
                              exit(1);
                        }
                        if(formatFlag != XML){
                          fprintf(outputFilePtr, "-- %s not found\n", nextSymbolToImportPtr->string);
                        }
                    }

                    nextSymbolToImportPtr = nextSymbolToImportPtr->next_symbol_node;
                }

        
            } /* end next symbol to import */

            /* if automatic update/replace feature turned on */
            if(updateImportsFlag) {
                if(tmpTreePtr) {
                    tmpTreePtr = tmpTreePtr->next_tree_node;
                    if(tmpTreePtr) {
                        importString = tmpTreePtr->string;
                    } else {
                        importString = NULL;
                    }

                } else {
                    importString = NULL;
                }
            } else {
                importString = NULL;
            }

        } /* end while(importString) */

        mibToImport = mibToImport->next_tree_node;


    } /* end while(mibToImport) */

}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================= FREE =============================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* frees list of tree nodes that we have allocated */

void free_all_nodes(tree * my_singleParseNodeList)
{
    tree * nextNode, * ptr;

    /* safety catch */
    if(!my_singleParseNodeList) return;

    ptr = my_singleParseNodeList;

    while(ptr) {

        nextNode = ptr->freePtr;

        if( ptr->string && ptr->freeMe) {
            free(ptr->string);
            ptr->string = NULL;
        }

        /* free enumerationTable if there is one */
        if(ptr->enumeratedItems) {
            free(ptr->enumeratedItems);
            ptr->enumeratedItems = NULL;
        }

        if(ptr) {
            ptr->freePtr = NULL;
            free(ptr);
            ptr = NULL;
        }

        ptr = nextNode;

    }

}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* =================== ERROR HANDLING ========================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* YYERROR:  yyerror is required by bison/yacc for reporting errors */
void 
yyerror (char * errmsg)
{
    int temp_importParseFlag;

    no_code_flag = 1;

    /* temporarily turn off importParseFlag if on */
    temp_importParseFlag = importParseFlag;
    importParseFlag = 0;

    print_warning(errmsg, lineBuf);

    if(stringWarnOn) {
        print_string_warning();
        stringWarnOn = 0;
    }

    importParseFlag = temp_importParseFlag;

}

/* ============================================================ */

/* 
 * This prints the error message, the line number and 
 * contents, then the ^ pointer to the error.
 */

void 
print_warning(char * my_msg, char * mybuf)
{
    int i, skipDis = 0;
    char * ptr;


    /* if import parse don't show msgs */
    if(importParseFlag) { 
	return;
    }

    if(lastModuleName && !outputModuleName) {
      outputModuleName = strdup(lastModuleName);
    }

    /* if suppressOutput turned on don't print */
    if(globalIgnoreStrictComplianceFlag) {
	if(globalSuppressOutputFlag) {
	  /* unset error flag and don't print */
	  globalFatalErrorFlag = 0;
	  return;
	}
	else {
	  /* unset error flag and do print */
	  globalFatalErrorFlag = 0;
	}
    }
    else {
      /*fatal error - print output*/
      globalFatalErrorFlag = 1;
      /* make sure mibgc exits with error */
      globalExitErrorFlag = 1;
    }

    if(line_cntr < 10) {
        skipDis = 1;
    } else if(line_cntr < 100) {
        skipDis = 2;
    } else if(line_cntr < 1000) {
        skipDis = 3;
    } else if(line_cntr < 10000) {
        skipDis = 4;
    } else if(line_cntr < 100000) {
        skipDis = 5;
    }

    /* print message */
    if(lastModuleName && lastFileParsed){
          fprintf(stderr, "\nReference module \"%s\" in file \"%s\"\n\n", lastModuleName, lastFileParsed);
    }
    fprintf(stderr, "%s\n", my_msg);

    /* print line number */
    fprintf(stderr, "line: %d ", line_cntr);

    /* skip newlines at beginning of text */

    ptr = mybuf;
    while(*ptr == '\n' || *ptr == '\r') {
        ptr++;
    }

    /* print contents of line */
    fprintf(stderr, "%s\n", ptr);

    /* point to error with ^ */
    for(i = 0 ; i < (col_cntr + 6 + skipDis) ; i++){
        fprintf(stderr, " ");
    }
    fprintf(stderr, "^\n");

    /* if ignore strict compliance, print warning but continue processing
       otherwise set the flag not to print out any more of this parse */
    if(globalIgnoreStrictComplianceFlag) {
	globalFatalErrorFlag = 0;
	if(outputModuleName) {
	  fprintf(stderr, "Ignoring warning. Continuing to parse %s\n\n",
		  	outputModuleName);
	}
    }
    else {
	/*fatal error*/
	globalFatalErrorFlag = 1;
    }
 
}

/* ============================================================ */

/* 
 * This prints the note message, the line number and 
 * contents, then the ^ pointer to the error.
 */

void 
print_note(char * my_msg, char * mybuf)
{
    int i, skipDis = 0;
    char * ptr;


    /* if import parse don't show msgs unless...*/
    if(importParseFlag) {

      /* we do want to print if this is an INDEX undefined error */
      if(postParseErrorFlag) {
	postParseErrorFlag = 0;
      }
      else {
	return;
      }
    }

    if(lastModuleName && !outputModuleName) {
      outputModuleName = strdup(lastModuleName);
    }

    /* if suppressOutput turned on or verbose turned off, don't print */
    if((globalSuppressOutputFlag || !globalVerboseOutputFlag) && 
		(!no_code_flag)) {
        return;
    }

    if(line_cntr < 10) {
        skipDis = 1;
    } else if(line_cntr < 100) {
        skipDis = 2;
    } else if(line_cntr < 1000) {
        skipDis = 3;
    } else if(line_cntr < 10000) {
        skipDis = 4;
    } else if(line_cntr < 100000) {
        skipDis = 5;
    }

    /* print message */
    if(lastModuleName && lastFileParsed){
          fprintf(stderr, "\nReference module \"%s\" in file \"%s\"\n\n", lastModuleName, lastFileParsed);
    }
    fprintf(stderr, "%s\n", my_msg);

    /* print line number */
    fprintf(stderr, "line: %d ", line_cntr);

    /* skip newlines at beginning of text */

    ptr = mybuf;
    while(*ptr == '\n' || *ptr == '\r') {
        ptr++;
    }

    /* print contents of line */
    fprintf(stderr, "%s\n", ptr);

    /* point to error with ^ */
    for(i = 0 ; i < (col_cntr + 6 + skipDis) ; i++){
        fprintf(stderr, " ");
    }
    fprintf(stderr, "^\n");
}

/* ============================================================ */

/* 
 * This prints the error message, the line number and 
 * contents, then the ^ pointer to the error.
 */

void print_string_warning()
{
    int i, skipDis = 0;
    char * ptr;

    if(lastModuleName && !outputModuleName) {
      outputModuleName = strdup(lastModuleName);
    }

    /* if suppressOutput turned on don't print */
    if(globalSuppressOutputFlag) return;

    if(string_line_cntr < 10) {
        skipDis = 1;
    } else if(string_line_cntr < 100) {
        skipDis = 2;
    } else if(string_line_cntr < 1000) {
        skipDis = 3;
    } else if(string_line_cntr < 10000) {
        skipDis = 4;
    } else if(string_line_cntr < 100000) {
        skipDis = 5;
    }

    /* print message */
    /* fprintf(stderr, "\nReference module \"%s\" in file \"%s\"\n", lastModuleName, inputFileName); */
    fprintf(stderr, "Possible beginning of unterminated string.\n");

    /* print line number */
    fprintf(stderr, "line: %d ", string_line_cntr);

    /* skip newlines at beginning of text */
    ptr = firstLineBuf;
    while(*ptr == '\n' || *ptr == '\r') {
        ptr++;
    }

    /* print contents of line */
    fprintf(stderr, "%s\n", ptr);

    /* point to error with ^ */
    for(i = 0 ; i < (col_cntr + 6 + skipDis) ; i++){
        fprintf(stderr, " ");
    }
    fprintf(stderr, "^\n");

}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ==================== MOSY EMIT ============================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

void 
write_header(FILE * outputFilePtr)
{
    int i;

    char buf[28];

    if (formatFlag |= 0){
       /* Don't write header for info.dat or xml */
       return;
    }

    /* get current time */
    myclock = time(0);

    fprintf(outputFilePtr, "-- ");
    for(i = 0 ; i < ((4*maxColumnWidth)+8) ; i++) {
        fprintf(outputFilePtr, "=");
    }   
    fprintf(outputFilePtr, "\n");

    fprintf(outputFilePtr, "-- automatically generated by mibgc\n");
    fprintf(outputFilePtr, "-- ");
    fprintf(outputFilePtr, "%s", ctime(&myclock));
    fprintf(outputFilePtr, "-- do not edit!\n");

    fprintf(outputFilePtr, "-- ");
    for(i = 0 ; i < ((4*maxColumnWidth)+8) ; i++) {
        fprintf(outputFilePtr, "=");
    }   
    fprintf(outputFilePtr, "\n");
    fprintf(outputFilePtr, "--\n");
    fprintf(outputFilePtr, "-- DIRECTIVES\n");
    fprintf(outputFilePtr, "-- %%defval  - default value, multiple BITS values are ORed together\n");
    fprintf(outputFilePtr, "-- %%ea      - augmentation\n");
    fprintf(outputFilePtr, "-- %%ei      - index clause, IMPLIED values are prefixed with *\n");
    fprintf(outputFilePtr, "-- %%er      - range for INTEGER or OCTET STRING\n");
    fprintf(outputFilePtr, "-- %%es      - syntax - multiple types in SYNTAX clause\n");
    fprintf(outputFilePtr, "-- %%ev      - enumerated values\n");
    fprintf(outputFilePtr, "-- %%n0      - OBJECT IDENTIFIER registration\n");
    fprintf(outputFilePtr, "-- %%tc      - Textual Convention definition\n");
    fprintf(outputFilePtr, "-- %%trap    - traps and notifications\n");
    fprintf(outputFilePtr, "--\n");

    fprintf(outputFilePtr, "-- FIELDS\n");
    fprintf(outputFilePtr, "-- Object Descriptor - name of the object\n");
    fprintf(outputFilePtr, "-- Group Entry       - MIB object group and the location of the MIB variable in the group\n");
    fprintf(outputFilePtr, "-- Syntax            - type of the object - \n");
    fprintf(outputFilePtr, "--                     INTEGER, OctetString, DisplayString, object-id, Aggregate\n");
    fprintf(outputFilePtr, "--                     IpAddress, Counter, Gauge, TimeTicks, Opaque\n");
    fprintf(outputFilePtr, "-- Access            - access permissions of the object \n");
    fprintf(outputFilePtr, "--                     read-only, read-write, write-only, not-accessible\n");
    fprintf(outputFilePtr, "-- Status            - RFC1213 compliance status of the object\n");
    fprintf(outputFilePtr, "--                     mandatory, optional, deprecated, obsolete\n");
    fprintf(outputFilePtr, "--\n");

    /*
    fprintf(outputFilePtr, "-- Note that mosy (unlike mibgc) doesn't handle BITS.\n");
    fprintf(outputFilePtr, "-- Mosy also fails to include all SIZE restrictions.\n\n");
    */

    fprintf(outputFilePtr, "-- ");
    for(i = 0 ; i < ((4*maxColumnWidth)+8) ; i++) {
        fprintf(outputFilePtr, "=");
    }   

    fprintf(outputFilePtr, "\n-- ");
    fprintf(outputFilePtr, "Object Descriptor ");
    for(i = 0 ; i < (maxColumnWidth - 20) ; i++) {
        fprintf(outputFilePtr, " ");
    }   

    fprintf(outputFilePtr, "Group Entry           ");
    for(i = 0 ; i < (maxColumnWidth - 21) ; i++) {
        fprintf(outputFilePtr, " ");
    }   

    fprintf(outputFilePtr, "Syntax ");
    for(i = 0 ; i < (maxColumnWidth - 6) ; i++) {
        fprintf(outputFilePtr, " ");
    }   

    fprintf(outputFilePtr, "Access ");
    for(i = 0 ; i < (maxColumnWidth - 6) ; i++) {
        fprintf(outputFilePtr, " ");
    }   

    fprintf(outputFilePtr, " Status\n");

    fprintf(outputFilePtr, "-- ");
    for(i = 0 ; i < ((4*maxColumnWidth)+8) ; i++) {
        fprintf(outputFilePtr, "=");
    }   

    fprintf(outputFilePtr, "\n");

}

/* ============================================================ */

void 
mosy_print_enumerated_range(tree * ptr, tree * rangelistPtr, FILE * outPtr) {

    int i;

    /* safety catch */
    if(!ptr || !ptr->child[child_identity_0] || !ptr->child[child_identity_0]->string) return; 

    while(rangelistPtr) {
        if(rangelistPtr->type == DOT_DOT) {

            /* column 1 */
            fprintf(outPtr, "%%er ");

            for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                fprintf(outPtr, " ");
            }   

            if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string){

                /* column 2 */
                fprintf(outPtr, "%s ", ptr->child[child_identity_0]->string);

                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                    fprintf(outPtr, " ");
                }   

            }

            /* is there a start range child */
            if(rangelistPtr->child[chld_strt_rnge12] && rangelistPtr->child[chld_strt_rnge12]->string){

                memset((void *)fileNameBuffer, 0, MYBUFSIZE );
                convert_ul_to_string(rangelistPtr->child[chld_strt_rnge12]->number, fileNameBuffer);

                if(rangelistPtr->child[chld_strt_rnge12]->type == NEG_NUMBER) {

                    /* column 3 */
                    fprintf(outPtr, "-%lu ", rangelistPtr->child[chld_strt_rnge12]->number);

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(fileNameBuffer) - 1) ; i++) {
                        fprintf(outPtr, " ");
                    }

                } else {

                    /* column 3 */
                    fprintf(outPtr, "%lu ", rangelistPtr->child[chld_strt_rnge12]->number);

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(fileNameBuffer)) ; i++) {
                        fprintf(outPtr, " ");
                    }

                }

   
            /* if there is only one number for the range it is printed twice */
            } else {

                if(rangelistPtr->child[chld_end_range13] && rangelistPtr->child[chld_end_range13]->string){

                    memset((void *)fileNameBuffer, 0, MYBUFSIZE );
                    convert_ul_to_string(rangelistPtr->child[chld_end_range13]->number, fileNameBuffer);

                    if(rangelistPtr->child[chld_end_range13]->type == NEG_NUMBER) {

                        /* column 3 */
                        fprintf(outPtr, "-%lu ", rangelistPtr->child[chld_end_range13]->number);

                        for(i = 0 ; i < (maxColumnWidth - (int)strlen(fileNameBuffer) - 1) ; i++) {
                            fprintf(outPtr, " ");
                        }

                    } else {
                        
                        /* column 3 */
                        fprintf(outPtr, "%lu ", rangelistPtr->child[chld_end_range13]->number);

                        for(i = 0 ; i < (maxColumnWidth - (int)strlen(fileNameBuffer)) ; i++) {
                            fprintf(outPtr, " ");
                        }

                    }


                }


            }

            /* is there an end range child */
            if(rangelistPtr->child[chld_end_range13] && rangelistPtr->child[chld_end_range13]->string){

                /* column 4 */
                if(rangelistPtr->child[chld_end_range13]->type == NEG_NUMBER) {
                    fprintf(outPtr, " -%lu\n", rangelistPtr->child[chld_end_range13]->number);
                } else {
                    fprintf(outPtr, " %lu\n", rangelistPtr->child[chld_end_range13]->number);
                }

            }

        }

        /* get next rangelist */
        rangelistPtr = rangelistPtr->next_tree_node;
    }

}

/* ============================================================ */

void 
mosy_emit_all_nodes(tree * ptr, FILE * outputFilePtr, FILE * descrFilePtr)
{
    int i;

    /* safety catch */
    if(!ptr)         return;

    if(!postParseErrorFlag) {
      mosy_emit_node(ptr, outputFilePtr,descrFilePtr);
    }
    else {
      return;
    }

    for(i = 0 ; i < MAX_CHILDREN ; i++)
        if(ptr->child[i])
            mosy_emit_all_nodes(ptr->child[i], outputFilePtr, descrFilePtr);

    if(ptr->next_tree_node) 
        mosy_emit_all_nodes(ptr->next_tree_node, outputFilePtr, descrFilePtr);
}

/* ============================================================ */

void 
mosy_emit_node(tree * ptr, FILE * outputFilePtr, FILE * descrFilePtr)
{
    int i = 0, j, jmpDis = 0, x;
    int tmpValue = 0;
    int isindex = 0;
    int exponent = 0;
    int pow_product = 0;

    char * tmpCharPtr = NULL;
    unsigned char * tmpEightBitValue = NULL;

    tree * tmpPtr = NULL;
    tree * lookupResult = NULL;
    tree * lookupIndexResult = NULL;
    tree * alreadyEmittedPtr;


    /* safety catch */
    if(!ptr) return;

    /* check whether already emitted for this parse */
    if(ptr->child[0] && ptr->child[0]->string) {

        alreadyEmittedPtr = lookup_name_in_emitted_nodes(ptr->child[0]->string, ptr->type);

	/* Don't want this to fail on multiple DEFVALS */
	if(ptr->type == DEFVAL) {
	   insert_name_into_emitted_nodes(ptr->child[0]->string, ptr->type);
        }
        else if(!alreadyEmittedPtr) {
            insert_name_into_emitted_nodes(ptr->child[0]->string, ptr->type);
        } 
	else {
            return;
        }

    }


    /* trace type definitions back to root */
    /* check_for_type_definition(ptr , outputFilePtr, descrFilePtr); */

    if(ptr->child[child_syntax___1]) {
        check_for_type_definition(ptr->child[child_syntax___1] , outputFilePtr, descrFilePtr);
    }

    if(ptr->parent && ptr->parent->child[child_syntax___1]) {
        check_for_type_definition(ptr->child[child_syntax___1] , outputFilePtr, descrFilePtr);
    }


    switch(ptr->type) {

        case DEFVAL:

            /* DEFVAL*/
            if( ptr->parent && ptr->parent->child[child_syntax___1] && ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                /* BITS values must be ORed, lookup values in enumeratedItems table and OR */
                if(ptr->parent->child[child_syntax___1]->type == BITS) {


		  /* Check and see if it is actually an enumerated INTEGER */
		  if(!strcmp(ptr->parent->child[child_syntax___1]->string,
			     "INTEGER")) {

                    tmpPtr = ptr->child[child_identity_0];
                    tmpValue=0;

		    while(tmpPtr) {
                      if(tmpPtr->string) {

                        if(ptr->parent->child[child_syntax___1]->enumeratedItems){

                          lookupResult = lookup_name(tmpPtr->string, ptr->parent->child[child_syntax___1]->enumeratedItems);

			  if(lookupResult && lookupResult->
                                child[child_identity_0]->string) {
                            tmpValue = atoi(lookupResult->
				child[child_identity_0]->string);
			  }
		        }
 		      }
		      tmpPtr = tmpPtr->next_tree_node;
	            }	


                    /* column 1 */
                    if(formatFlag == 0){
                     fprintf(outputFilePtr, "%%defval ");

                     for(i = 0 ; i < (maxColumnWidth - 7) ; i++) {
                        fprintf(outputFilePtr, " ");
                     }   

                     if(ptr->parent->child[child_identity_0] && ptr->parent->child[child_identity_0]->string){

                        /* column 2 */
                        fprintf(outputFilePtr, "%s ", ptr->parent->child[child_identity_0]->string);

                        for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->parent->child[child_identity_0]->string)) ; i++) {
                            fprintf(outputFilePtr, " ");
                        }

                     }   

                     /* column 4 */
		     fprintf(outputFilePtr, "%d\n", tmpValue);
                    } /* formatFlag */
	          }	    


		  else {  
		    /* BAB - The algorithm is this: loop the entire
		     * list and see what the largest bit is set. 
		     * Determine how many chars need to be 
		     * malloced. Loop again, and take the number
		     * which represents which bit is set and make
 		     * that the exponent for 2^x. Bit-wise OR that
		     * number with each subsequent number for each
		     * 8 bit block. Reverse each 8 bit block and merge
		     * each 8 bit block together starting with last 
		     * block first in Hex. Eg: 0, 2 and 8 bits would be: 
		     * 00000001 | 00000010 = 00000011 (first block)
		     * 00000001 (second block)
		     * reverse: 10000000:11000000 and convert to hex:
		     * 0x8:C0
		     */


                    tmpPtr = ptr->child[child_identity_0];
  	            tmpValue=0; 

                    while(tmpPtr) {
            
                      if(tmpPtr->string) {

                        if(ptr->parent->child[child_syntax___1]->enumeratedItems) {

                          lookupResult = lookup_name(tmpPtr->string, ptr->parent->child[child_syntax___1]->enumeratedItems);

			  /*get largest exponent to see how much to malloc*/
			  if(lookupResult && ((atoi(lookupResult->
				child[child_identity_0]->string)) > tmpValue)) {
			     tmpValue = atoi(lookupResult->child[child_identity_0]->string);	
                          }

                        }
                      }

                      tmpPtr = tmpPtr->next_tree_node;
                    }
		
		    /*divide by 8 and add 1 to get malloc size*/
		    tmpEightBitValue = (char *)malloc((sizeof(char)) * 
						(tmpValue/8 + 1));

		    /* Loop again and do Bit-wise arithemetic*/

                    tmpPtr = ptr->child[child_identity_0];
		    x = 0;

                    while(tmpPtr) {
            
                      if(tmpPtr->string) {

                        if(ptr->parent->child[child_syntax___1]->enumeratedItems) {

                          lookupResult = lookup_name(tmpPtr->string, ptr->parent->child[child_syntax___1]->enumeratedItems);

			  if(lookupResult && lookupResult->
				child[child_identity_0]->string) {
			    /* if we have reached the end of the 8 bit block 
			     * move to the next 8 bits
			     */
			    x = atoi(lookupResult->
					child[child_identity_0]->string) / 8; 
				
			    /* since we only want the exponent to be 0 - 7, 
			     * we modulo by 8
			     */
			    exponent = (atoi(lookupResult->
					child[child_identity_0]->string) % 8);

			    /* pow() function is not available with all 
			     * math libraries. Writing a simple function
			     */
  			    pow_product = bin_pow(exponent);
		 	    tmpEightBitValue[x] = tmpEightBitValue[x] | 
					(char)pow_product;
                          }

                        }
                      }

                      tmpPtr = tmpPtr->next_tree_node;
                    }

                    /* column 1 */
                    if(formatFlag == 0){
                     fprintf(outputFilePtr, "%%defval ");

                     for(i = 0 ; i < (maxColumnWidth - 7) ; i++) {
                        fprintf(outputFilePtr, " ");
                     }   

                     if(ptr->parent->child[child_identity_0] && ptr->parent->child[child_identity_0]->string){

                        /* column 2 */
                        fprintf(outputFilePtr, "%s ", ptr->parent->child[child_identity_0]->string);

                        for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->parent->child[child_identity_0]->string)) ; i++) {
                            fprintf(outputFilePtr, " ");
                        }

                     }   

                     /* column 4 */
                     /*fprintf(outputFilePtr, "%d", tmpValue);*/
	             fprintf(outputFilePtr, "0x");
		     for(i=0; i< tmpValue/8 + 1; i++) {
                       fprintf(outputFilePtr, "%x", tmpEightBitValue[i]);
		       /*if there will be more hex output*/
		       if((tmpValue/8 + 1) > i+1) {
			 fprintf(outputFilePtr, ":");
		       }
		     }

                     fprintf(outputFilePtr, "\n");
                    } /* formatFlag */
		  } /* if INTEGER */
 	        } /* if BITS */

                /* OCTET STRING type */
                else if(ptr->parent->child[child_syntax___1]->type == OCTET) {

                    if(formatFlag == 0){
                     /* column 1 */
                     fprintf(outputFilePtr, "%%defval ");

                     for(i = 0 ; i < (maxColumnWidth - 7) ; i++) {
                        fprintf(outputFilePtr, " ");
                     }   

                     if(ptr->parent->child[child_identity_0] && ptr->parent->child[child_identity_0]->string){
            
                        /* column 2 */
                        fprintf(outputFilePtr, "%s", ptr->parent->child[child_identity_0]->string);
                        for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->parent->child[child_identity_0]->string)) ; i++) {
                            fprintf(outputFilePtr, " ");
                        }   

                     }

                     /* column 3 */
                     fprintf(outputFilePtr, " %s", ptr->child[child_identity_0]->string);
                     fprintf(outputFilePtr, "\n");
                     jmpDis = strlen(ptr->child[child_identity_0]->string);
                    } /* formatFlag */

                } /* if OCTET STRING type */

		else {

                    if(formatFlag == 0){
                     /* column 1 */
                     fprintf(outputFilePtr, "%%defval ");

                     for(i = 0 ; i < (maxColumnWidth - 7) ; i++) {
                        fprintf(outputFilePtr, " ");
                     }   

                     if(ptr->parent->child[child_identity_0] && ptr->parent->child[child_identity_0]->string){

                        /* column 2 */
                        if(ptr->parent->child[child_identity_0]->type == NEG_NUMBER) {
                            fprintf(outputFilePtr, "-%s ", ptr->parent->child[child_identity_0]->string);
                            for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->parent->child[child_identity_0]->string)-1) ; i++) {
                                fprintf(outputFilePtr, " ");
                            }   
                        } else {
                            fprintf(outputFilePtr, "%s ", ptr->parent->child[child_identity_0]->string);
                            for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->parent->child[child_identity_0]->string)) ; i++) {
                                fprintf(outputFilePtr, " ");
                            }   
                        }

                     }

                     /* column 3 */
                     jmpDis = print_value_list(ptr->child[child_identity_0], outputFilePtr);

                     fprintf(outputFilePtr, "\n");
                    } /* formatFlag */
                } /* end else */

                check_for_type_definition(ptr->child[child_identity_0] , outputFilePtr, descrFilePtr);
            } 

            break;

        case DESCRIPTION:

            /* do we need to produce a description file */
            if(globalProduceDescrFlag && ptr->parent && ptr->parent->child[child_value____9] && ptr->parent->child[child_value____9]->string && ptr->string) {

                jmpDis = print_value_list(ptr->parent->child[child_value____9], descrFilePtr);

                memset(lineBuf, 0, MYBUFSIZE );
                strcpy(lineBuf, ptr->string);

                fprintf(descrFilePtr, "   \"");
                    
                tmpCharPtr = strtok(lineBuf, " \t\n\f\r");

                while(tmpCharPtr) {

                    /* print it, find next token */ 
                    fprintf(descrFilePtr, "%s ", tmpCharPtr);

                    tmpCharPtr = strtok(NULL, " \t\n\f\r");

                }    

                fprintf(descrFilePtr, "\"\n");

            }

            break;

        /* module definition */
        case DEFINITIONS:

            /* child 0 */
            if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {
              if(formatFlag == 0) {
                fprintf(outputFilePtr, "-- object definitions compiled from %s\n\n", ptr->child[child_identity_0]->string);
              }
            }

            break;

        /* this should be SMIv1 */
        case TRAP_TYPE:

            if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

             if(formatFlag == 0){
                fprintf(outputFilePtr, "%%trap ");

                /* print name */
                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);

                /* print object part */
                if(ptr->child[child_object___5] && ptr->child[child_object___5]->string) {

                    if(ptr->child[child_object___5]->type == NEG_NUMBER) {
                        fprintf(outputFilePtr, "-%s", ptr->child[child_object___5]->string);
                    } else {
                        fprintf(outputFilePtr, "%s", ptr->child[child_object___5]->string);
                    }

                    tmpPtr = ptr->child[child_object___5]->next_tree_node;
                    while(tmpPtr) {
                        if(tmpPtr->string) fprintf(outputFilePtr, ".%s", tmpPtr->string);
                        tmpPtr = tmpPtr->next_tree_node;
                    }

                }

                /* print value */
                if(ptr->child[child_value____9] && ptr->child[child_value____9]->string) {

                    /* child 1 and siblings */
                    tmpPtr = ptr->child[child_value____9];
                    while(tmpPtr->next_tree_node) {
                        if(tmpPtr->string) fprintf(outputFilePtr, " %s", tmpPtr->string);
                        tmpPtr = tmpPtr->next_tree_node;
                        fprintf(outputFilePtr, ",");
                    }

                    if(tmpPtr->string) fprintf(outputFilePtr, " %s", tmpPtr->string);

                }

                fprintf(outputFilePtr, "\n");
              } /* formatFlag */

            }

            break;

        case STRUCT:
        case UNION:

            /* print name and value */
            if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                /* column 1 */
                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);

                /* print value */
                if(ptr->child[child_value____9]) {

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    jmpDis = print_value_list(ptr->child[child_value____9], outputFilePtr);

                }

                /* column 2 */
                /* print Struct or Union */
                if(ptr->string) {

                    for(i = 0 ; i < (maxColumnWidth - jmpDis + 1) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    fprintf(outputFilePtr, "%s ", ptr->string);

                }

                /* column 3 */
                /* print access */
                if(ptr->child[child_access__11] && ptr->child[child_access__11]->string){

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_access__11]->string)) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    fprintf(outputFilePtr, " %s ", ptr->child[child_access__11]->string);

                }

                /* column 4 */
                /* print status */
                if(ptr->child[child_status___2] && ptr->child[child_status___2]->string){

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_status___2]->string)) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    fprintf(outputFilePtr, "%s\n", ptr->child[child_status___2]->string);
                }

                check_for_type_definition(ptr->child[child_value____9] , outputFilePtr, descrFilePtr);

            }

            break;

        /* this should be SMIv2 */
        case NOTIFICATION_TYPE:

            if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                fprintf(outputFilePtr, "%%trap ");

                /* print name */
                fprintf(outputFilePtr, "%s -1 ", ptr->child[child_identity_0]->string);

                /* print value */
                if(ptr->child[child_value____9] && ptr->child[child_value____9]->string) {

                    if(ptr->child[child_value____9]->type == NEG_NUMBER) {
                        fprintf(outputFilePtr, "-%s", ptr->child[child_value____9]->string);
                    } else {
                        fprintf(outputFilePtr, "%s", ptr->child[child_value____9]->string);
                    }

                    tmpPtr = ptr->child[child_value____9]->next_tree_node;
                    while(tmpPtr) {
                        if(tmpPtr->string) fprintf(outputFilePtr, ".%s", tmpPtr->string);
                        tmpPtr = tmpPtr->next_tree_node;
                    }

                }

                /* is there an objectPart */
                if(ptr->child[child_object___5] && ptr->child[child_object___5]->string) {

                    /* child 1 and siblings */
                    fprintf(outputFilePtr, " { ");

                    tmpPtr = ptr->child[child_object___5];
                    while(tmpPtr->next_tree_node) {
                        if(tmpPtr->string) fprintf(outputFilePtr, " %s", tmpPtr->string);
                        tmpPtr = tmpPtr->next_tree_node;
                        fprintf(outputFilePtr, ",");
                    }

                    if(tmpPtr->string) fprintf(outputFilePtr, " %s", tmpPtr->string);

                    fprintf(outputFilePtr, " }");

                }

                fprintf(outputFilePtr, "\n");

            }
           
        case AGENT_CAPABILITIES:
        case MODULE_IDENTITY:       /* module identification definition */
        case IDENTIFIER:            /* OBJECT IDENTIFIER */
        case MODULE_COMPLIANCE:
        case MODULE_CONFORMANCE:
        case NOTIFICATION_GROUP:
        case OBJECT_GROUP:
        case OBJECT_IDENTITY:

            /* print name and value */
            if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string && ptr->child[child_value____9] && ptr->child[child_value____9]->string) {

                /* column 1 */
                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);

                /* column 2 */
                /* print value */
                if(ptr->child[child_value____9]) {

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    jmpDis = print_value_list(ptr->child[child_value____9], outputFilePtr);

                }

                if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                    /* column 1 */
                    fprintf(outputFilePtr, "\n%%n0 ");

                    for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    /* column 2 */
                    fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    /* column 3 */
                    if(ptr->string) {
                        fprintf(outputFilePtr, "%s\n", ptr->string);
                    }

                }

                check_for_type_definition(ptr->child[child_value____9] , outputFilePtr, descrFilePtr);

            }

                  fprintf(outputFilePtr, "\n");

            break;

        case OBJECT_TYPE:

            check_for_range_restriction_on_type(ptr, outputFilePtr);

            if(ptr->child[child_value____9] && ptr->child[child_value____9]->string) {
                check_for_type_definition(ptr->child[child_value____9] , outputFilePtr, descrFilePtr);
            }

            lookupResult = NULL;

            /* print name */
            if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);

                /* print value */
                if(ptr->child[child_value____9] && ptr->child[child_value____9]->string) {

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    jmpDis = print_value_list(ptr->child[child_value____9], outputFilePtr);

                }

                for(i = 0 ; i < (maxColumnWidth - jmpDis) ; i++) {
                    fprintf(outputFilePtr, " ");
                }   



            }

            /* SEQUENCE OF, virtual table */
            if(ptr->child[child_syntax___1] && ptr->child[child_syntax___1]->type == OF) {

                fprintf(outputFilePtr, " Aggregate ");

                jmpDis = 9;

                for(i = 0 ; i < (maxColumnWidth - jmpDis) ; i++) {
                    fprintf(outputFilePtr, " ");
                }   

            } else {

                /* is this a new named aggregate type */
                if(ptr->child[child_syntax___1] && ptr->child[child_syntax___1]->string) {

                    lookupResult = lookup_type(ptr->child[child_syntax___1]->string, virtualTables);

                    if(lookupResult) {

                        fprintf(outputFilePtr, "%s", " Aggregate ");
                        jmpDis = 9;

                        /* reset */
                        lookupResult = NULL;

                    } else {

                        /* if it is an imported type, type is 0, so lookup in table */
                        if(!ptr->child[child_syntax___1]->type) {

                                lookupResult = lookup_type(ptr->child[child_syntax___1]->string, singleParseTypeTable);

                                if(lookupResult) {

                                    if(lookupResult->child[child_identity_0] && lookupResult->child[child_identity_0]->string) {

                                        if(lookupResult->child[child_identity_0]->type == NEG_NUMBER) {
                                            fprintf(outputFilePtr, " -%s ", lookupResult->child[child_identity_0]->string);
                                            jmpDis = (int)strlen(lookupResult->child[child_identity_0]->string)+1;
                                        } else {

/* *********     FAM Entry type printed here */

       if( ptr->child[child_access__11] && ptr->child[child_access__11]->string){
            if (!strcmp(ptr->child[child_access__11]->string, "not-accessible")) {
                 /* INDEX */
                 if( ptr->child[child_index____7] ) {
                    if( (ptr->child[child_index____7]->type == INDEX) ||
                        (ptr->child[child_index____7]->type == AUGMENTS) ) {
                    /* this is used for Entry */
                         isindex = 1;
                     }
                 }
                 if(isindex){

		     if(ptr->child[child_index____7]->child[child_identity_0]) {
			
			  tmpPtr = ptr->child[child_index____7]->
					child[child_identity_0];

			  /*Check to see if INDEX is valid*/

			  while(tmpPtr) {
			    if(!tmpPtr->type) {
                              /* if it is an imported type, type is 0, so 
			       *  lookup in table
			       */
			      lookupIndexResult = NULL;
                  	      /* look in AUX-MIB table first */
                 	      lookupIndexResult = lookup_type(tmpPtr->string,
                                        allParsesTypeTable);

			      if(!lookupIndexResult) {
                                 /* look in regular table */
                                 lookupIndexResult = lookup_type(tmpPtr->string,
                                        singleParseTypeTable);
                              }

                              if(!lookupIndexResult) {

				/*Index is not defined anywhere*/
        			if(!globalSuppressOutputFlag && 
					globalVerboseOutputFlag) {
            			  fprintf(stderr, "*** NOTE *** element in INDEX clause is undefined: %s\n\n", tmpPtr->string);
        			}

				/*****************************
				fprintf(stderr, "object %s: \n",
				  ptr->child[child_syntax___1]->string);
				fprintf(stderr, "    element in INDEX clause is undefined: %s\n", tmpPtr->string);
				postParseErrorFlag = 1;
				*******************************/
			      }
			    }

			    tmpPtr = tmpPtr->next_tree_node;
			  }

#ifdef NOT_YET
			  if(postParseErrorFlag) {


/******************************************************************
 *Eventually we will need to have mibgc fail when INDEX clause is
 * undefined, but for now, we just ignore and move on
 ******************************************************************/
			    if(outputModuleName) {
              			fprintf(stderr, 
				   "Failed to parse %s\n\n", outputModuleName);
              			free(outputModuleName);
              			outputModuleName = NULL;
            		    }

 			    memset(lineBuf, 0, MYBUFSIZE );
#ifndef SR_WINDOWS
        		    strcpy(lineBuf, "rm ");
#else
        		    strcpy(lineBuf, "del ");
#endif /* SR_WINDOWS */
        		    strcat(lineBuf, outputFileName);
        		    system(lineBuf);
		
			    break;
			 }
#endif /*NOT_YET*/

		     }
                     fprintf(outputFilePtr, " Aggregate ");
                     isindex=0;
                 } else {
                     fprintf(outputFilePtr, " %s ", lookupResult->child[child_identity_0]->string);
                     isindex=0;
                 }
            } else {
                 fprintf(outputFilePtr, " %s ", lookupResult->child[child_identity_0]->string);
            }
        }
/* *** FAM ***/
                                            jmpDis = (int)strlen(lookupResult->child[child_identity_0]->string);
                                      }

                                  }

                                } else {
                if(ptr->child[child_access__11] && ptr->child[child_access__11]->string){
}
                                    if(ptr->child[child_syntax___1]->string) {
                                        fprintf(outputFilePtr, " %s ", ptr->child[child_syntax___1]->string);
                                        jmpDis = (int)strlen(ptr->child[child_syntax___1]->string);
                                    }

                                } 

                        } else {
                            fprintf(outputFilePtr, " %s ", ptr->child[child_syntax___1]->string);
                            jmpDis = (int)strlen(ptr->child[child_syntax___1]->string);


                        }

                    }

                    for(i = 0 ; i < (maxColumnWidth - jmpDis) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                }

            }

            /* access */
            if(ptr->child[child_access__11] && ptr->child[child_access__11]->string){

                fprintf(outputFilePtr, " %s", ptr->child[child_access__11]->string);

                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_access__11]->string)) ; i++) {
                    fprintf(outputFilePtr, " ");
                }   
            
            }

            if(ptr->child[child_status___2] && ptr->child[child_status___2]->string){
                fprintf(outputFilePtr, " %s\n", ptr->child[child_status___2]->string);
            }




/* ======================================= */

/* enumerated range, RangeList is always child[child_range____6]  */
if( lookupResult && lookupResult->child[child_identity_0] && lookupResult->child[child_identity_0]->child[child_range____6] ) {
    mosy_print_enumerated_range(ptr, lookupResult->child[child_identity_0]->child[child_range____6], outputFilePtr);
}

/* ======================================= */

            /* INDEX or AUGMENTS*/
            if( ptr->child[child_index____7] ) {

                /* INDEX */
                if( ptr->child[child_index____7]->type == INDEX) {
                    fprintf(outputFilePtr, "%%ei ");

                    for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    /* print name */
                    if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                        fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);

                        tmpPtr = ptr->child[child_index____7]->child[child_identity_0];

                        /* do we have at least one item? */
                        if(tmpPtr) {

                            for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                                fprintf(outputFilePtr, " ");
                            }   

                            /* print opening quote */
                            fprintf(outputFilePtr, "\"");

                            while(tmpPtr->next_tree_node && tmpPtr->string) {
                                /* print each item with a space after it */
                
                                if(tmpPtr->type == IMPLIED){
                                    fprintf(outputFilePtr, "*%s ", tmpPtr->string);
                                } else {
                                    fprintf(outputFilePtr, "%s ", tmpPtr->string);
                                }

                                tmpPtr = tmpPtr->next_tree_node;
                            }

                            /* print last item without a space after it */
                            if(tmpPtr->string){
                                if(tmpPtr->type == IMPLIED){
                                    fprintf(outputFilePtr, "*%s", tmpPtr->string);
                                } else {
                                    fprintf(outputFilePtr, "%s", tmpPtr->string);
                                }
                            }

                            /* print closing quote */
                            fprintf(outputFilePtr, "\"");

                        }

                    }

                    fprintf(outputFilePtr, "\n");

                /* AUGMENTS */
                } else if( ptr->child[child_index____7]->type == AUGMENTS) {

                    fprintf(outputFilePtr, "%%ea ");

                    for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                        fprintf(outputFilePtr, " ");
                    }   

                    /* print name */
                    if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {
                        fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);
            
                        for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                            fprintf(outputFilePtr, " ");
                        }   

                    }

                    /* print name */
                    if(ptr->child[child_index____7]->string) {
                        fprintf(outputFilePtr, "%s ", ptr->child[child_index____7]->string);

                    }

                    fprintf(outputFilePtr, "\n");

                }

            }

/* ======================================= */


            if( ptr->child[child_identity_0] && ptr->child[child_identity_0]->string && ptr->child[child_syntax___1] &&  ptr->child[child_syntax___1]->child[child_status___2] ) {

                /* enumerated values , NameItemList is always child[child_status___2] */

                if( (ptr->child[child_syntax___1]->type == BITS) || (ptr->child[child_syntax___1]->type == BIT) ) {

                    /* named item list */
                    tmpPtr = ptr->child[child_syntax___1]->child[child_status___2];
                    while(tmpPtr) {
                        if(tmpPtr->child[child_identity_0]) {

                            fprintf(outputFilePtr, "%%ev ");

                            for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                                fprintf(outputFilePtr, " ");
                            }   

                            if(ptr->child[child_identity_0]->type == NEG_NUMBER) {
                                fprintf(outputFilePtr, "-%s ", ptr->child[child_identity_0]->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)-1) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            } else {
                                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            }

                            if(tmpPtr->string) {

                                if(tmpPtr->type == NEG_NUMBER) {
                                    fprintf(outputFilePtr, "-%s ", tmpPtr->string);
                                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(tmpPtr->string)-1) ; i++) {
                                        fprintf(outputFilePtr, " ");
                                    }   
                                } else {
                                    fprintf(outputFilePtr, "%s ", tmpPtr->string);
                                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(tmpPtr->string)) ; i++) {
                                        fprintf(outputFilePtr, " ");
                                    }   
                                }

                            }

                            if(tmpPtr->child[child_identity_0]->string) {
                                fprintf(outputFilePtr, " %s\n", tmpPtr->child[child_identity_0]->string);
                            }


                        }
                        tmpPtr = tmpPtr->next_tree_node;
                    }


                /* enumerated syntax, NamedBitListPlugin is always child[child_status___2] */
                } else {

                    tmpPtr = ptr->child[child_syntax___1]->child[child_status___2];
                    while(tmpPtr) {
                        if(tmpPtr->child[child_identity_0] && tmpPtr->child[child_identity_0]->string && tmpPtr->string)

                            fprintf(outputFilePtr, "%%es ");

                            for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                                fprintf(outputFilePtr, " ");
                            }
   
                            if(ptr->child[child_identity_0]->type == NEG_NUMBER) {
                                fprintf(outputFilePtr, "-%s ", ptr->child[child_identity_0]->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)-1) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            } else {
                                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            }

                            if(tmpPtr->type == NEG_NUMBER) {
                                fprintf(outputFilePtr, "-%s ", tmpPtr->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(tmpPtr->string)-1) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            } else {
                                fprintf(outputFilePtr, "%s ", tmpPtr->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(tmpPtr->string)) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            }


                            if(tmpPtr->child[child_identity_0]->type == NEG_NUMBER) {
                                fprintf(outputFilePtr, " -%s\n", tmpPtr->child[child_identity_0]->string);
                            } else {
                                fprintf(outputFilePtr, " %s\n", tmpPtr->child[child_identity_0]->string);
                            }

                        tmpPtr = tmpPtr->next_tree_node;
                    }

                }

            }

/* ======================================= */

/* enumerated range, RangeList is always child[child_range____6]  */
if( ptr->child[child_syntax___1] &&  ptr->child[child_syntax___1]->child[child_range____6] ) {
    mosy_print_enumerated_range(ptr, ptr->child[child_syntax___1]->child[child_range____6], outputFilePtr);
}

/* ======================================= */


/* ======================================= */

            break;


        case TEXTUAL_CONVENTION:

            /* child 0 */
            if(ptr->child[child_identity_0] && ptr->child[child_syntax___1] && ptr->child[child_syntax___1]->string && ptr->child[child_identity_0]->string) {

	      	/* See if TEXTUAL CONVENTION or TYPE ASSIGNMENT */
	      	if(ptr->string && !(strcmp(ptr->string, "TYPE_ASSIGNMENT"))) {
                  fprintf(outputFilePtr, "%%ta ");
	      	}
	      	else {
		  /* TEXTUAL CONVENTION */
                  fprintf(outputFilePtr, "%%tc ");
		}

                for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                    fprintf(outputFilePtr, " ");
                }   

                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);

                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                    fprintf(outputFilePtr, " ");
                }   

                if(ptr->child[child_syntax___1] && ptr->child[child_syntax___1]->string) {

                    fprintf(outputFilePtr, "%s ", ptr->child[child_syntax___1]->string);

                    for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_syntax___1]->string)) ; i++) {
                        fprintf(outputFilePtr, " ");
                    } 

                }

                /* display hint */
                fprintf(outputFilePtr, "\"");
                if(ptr->child[child_display__8] && ptr->child[child_display__8]->string ){
                    fprintf(outputFilePtr, "%s", ptr->child[child_display__8]->string);
                }
                fprintf(outputFilePtr, "\"");

                /* end the line */
                fprintf(outputFilePtr, " \n");


            /* enumerated range , RangeList is always child[child_range____6] */
            if( ptr->child[child_syntax___1] &&  ptr->child[child_syntax___1]->child[child_range____6] ) {
                mosy_print_enumerated_range(ptr, ptr->child[child_syntax___1]->child[child_range____6], outputFilePtr);
            }

            /* enumerated syntax, NamedBitListPlugin is always child[child_status___2] */
            if( ptr->child[child_identity_0] && ptr->child[child_identity_0]->string && ptr->child[child_syntax___1] &&  ptr->child[child_syntax___1]->child[child_status___2] ) {

                tmpPtr = ptr->child[child_syntax___1]->child[child_status___2];
                while(tmpPtr) {

                    if(tmpPtr && tmpPtr->child[child_identity_0] && tmpPtr->child[child_identity_0]->string && tmpPtr->string) {

                        fprintf(outputFilePtr, "%%es ");
                        for(i = 0 ; i < (maxColumnWidth - 3) ; i++) {
                            fprintf(outputFilePtr, " ");
                        }   

                        if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                            if(ptr->child[child_identity_0]->type == NEG_NUMBER) {
                                fprintf(outputFilePtr, "-%s ", ptr->child[child_identity_0]->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)-1) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }
                            } else {
                                fprintf(outputFilePtr, "%s ", ptr->child[child_identity_0]->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->child[child_identity_0]->string)) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }
                            }

                        }   

                        if(tmpPtr->string) {

                            if(tmpPtr->type == NEG_NUMBER) {
                                fprintf(outputFilePtr, "-%s ", tmpPtr->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(tmpPtr->string)-1) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            } else {
                                fprintf(outputFilePtr, "%s ", tmpPtr->string);
                                for(i = 0 ; i < (maxColumnWidth - (int)strlen(tmpPtr->string)) ; i++) {
                                    fprintf(outputFilePtr, " ");
                                }   
                            }

                        }

                        if(tmpPtr->child[child_identity_0] && tmpPtr->child[child_identity_0]->string) {

                            if(tmpPtr->child[child_identity_0]->type == NEG_NUMBER) {
                                fprintf(outputFilePtr, " -%s\n", tmpPtr->child[child_identity_0]->string);
                            } else {
                                fprintf(outputFilePtr, " %s\n", tmpPtr->child[child_identity_0]->string);
                            }

                        }

                    }

                    tmpPtr = tmpPtr->next_tree_node;
                }
             }

            }

            break;

        default:
            break;

    }

}

/* ============================================================ */

void 
check_for_type_definition(tree * myPtr, FILE * myOutputFilePtr, FILE * myDescrFilePtr)
{
    tree * typeExistsPtr     = NULL;
    tree * alreadyEmittedPtr = NULL;

    /* safety catch */
    if(!myPtr || !myPtr->string) return;

    /* check whether already emitted for this parse */
    if(myPtr->child[0] && myPtr->child[0]->string) {
        alreadyEmittedPtr = lookup_name_in_emitted_nodes(myPtr->child[0]->string, myPtr->type);
    }

    if(!alreadyEmittedPtr) {

        /* find type */
        typeExistsPtr = find_type(myPtr->string);

        /* trace type back to root */
        while(typeExistsPtr && !alreadyEmittedPtr) {

            /* insert node into list of emitted nodes */
            if(formatFlag == 0){
                mosy_emit_node(typeExistsPtr, myOutputFilePtr, myDescrFilePtr);
            } else {
                ll_emit_node(typeExistsPtr, myOutputFilePtr);
            }

            /* trace def back to root */
            if(typeExistsPtr && typeExistsPtr->child[child_syntax___1] && typeExistsPtr->child[child_syntax___1]->string) {

                typeExistsPtr = find_type(typeExistsPtr->child[child_syntax___1]->string);

            } else {
                typeExistsPtr = NULL;
            }

            if(typeExistsPtr) {

               if(typeExistsPtr->child[child_syntax___1] &&
                   typeExistsPtr->child[child_syntax___1]->string &&
                   typeExistsPtr->type) {

                      alreadyEmittedPtr = lookup_name_in_emitted_nodes(typeExistsPtr->child[child_syntax___1]->string, typeExistsPtr->type);

                      if(alreadyEmittedPtr) {
                         typeExistsPtr = NULL;  
                      }
               }

            }

        }

    }    

}

/* ============================================================ */

void 
check_for_range_restriction_on_type(tree * myPtr, FILE * myOutputFilePtr)
{
    tree * tmpPtr;

    /* is there a range restriction associated with the type of the object */
    if(myPtr && myPtr->child[child_syntax___1] && myPtr->child[child_syntax___1]->string) {

        /* check for type definition in type table */
        tmpPtr = lookup_type(myPtr->child[child_syntax___1]->string, allParsesTypeTable);

        if(!tmpPtr) {
            tmpPtr = lookup_type(myPtr->child[child_syntax___1]->string, singleParseTypeTable);
        }

        if( tmpPtr && tmpPtr->child[0] && tmpPtr->child[0]->child[0] && tmpPtr->child[0]->child[0]->child[child_range____6] ) {
            /* enumerated range, RangeList is always child[child_range____6]  */
            mosy_print_enumerated_range(myPtr, tmpPtr->child[0]->child[0]->child[child_range____6], myOutputFilePtr);

        }

    }

}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* =================== HASH TABLE ============================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* INSERT: insert node into symbol table */
/* lookup has already been used to make sure not already in table */
void 
insert_name(tree * node, tree * symboltable[] )
{
    register int location;

    /* safety catch */
    if(!node || !symboltable) return;

    /* find location to insert into symbol table */
    location  = hashpjw(node->string, TABLE_SIZE);

    /* insert at head of linked list chained at that location */
    node->next_symbol_node       = symboltable[location];
    symboltable[location] = node;

}

/* ============================================================ */

/* LOOKUP: lookup string name in symbol table */
/* will always find latest first */

tree * 
lookup_name(char * string, tree * symboltable[] )
{
    register int location, compare = 1;
    tree * entry_ptr = NULL;

    /* safety catch */
    if(!string || !symboltable) return NULL;

    location = hashpjw(string, TABLE_SIZE);
    entry_ptr = symboltable[location];

    /* traverse list to find */
    while( entry_ptr &&  entry_ptr->string && (compare = strcmp(entry_ptr->string,string)) )
        entry_ptr = entry_ptr->next_symbol_node;

    /* if not found return NULL */
    if(compare) {
        return(NULL);
    /* otherwise, return location */
    } else {
        return(entry_ptr);
    }  
}

/* ============================================================ */

tree * 
lookup_type(char * string, tree * symboltable[] )
{
    register int location, compare = 1;
    tree * entry_ptr = NULL;

    /* safety catch */
    if(!string || !symboltable || !strlen(string)) {
        return NULL;
    }

    location = hashpjw(string, TABLE_SIZE);
    entry_ptr = symboltable[location];

    /* traverse list to find */
    while( entry_ptr &&  entry_ptr->child[0] && entry_ptr->child[0]->string && (compare = strcmp(entry_ptr->child[0]->string,string)) )
        entry_ptr = entry_ptr->next_symbol_node;

    /* if not found return NULL */
    if(compare) {
        return(NULL);
    /* otherwise, return location */
    } else {
        return(entry_ptr);
    }  
}

/* ============================================================ */

void 
insert_type_node(tree * node, tree * symboltable[] )
{
    register int location;

    /* safety catch */
    if(!node || !symboltable || !node->child[0] || !node->child[0]->string ) return;

    /* if already in table, do nothing, don't overwrite */
    /* we don't want to overwrite because we load our own
       aux defs first, we don't want them overwritten by
       someone else's bogus definitions */

    if( lookup_type(node->child[0]->string, symboltable) ) {

        if(!globalSuppressOutputFlag) {
            print_warning("*** ERROR ***  Re-definition of type.", lineBuf);
        }

        return;
    }

    /* find location to insert into symbol table */
    location  = hashpjw(node->child[0]->string, TABLE_SIZE);

    /* insert at head of linked list chained at that location */
    node->next_symbol_node       = symboltable[location];
    symboltable[location] = node;

}

/* ============================================================ */

/*
 * HASHPJW FUNCTION
 *   Adds the characters in the key "s" then mods
 *   with the number of entries in 
 *   "num_entries" to produce a number that is used
 *   to index into a hash table
 *
 *   taken from page 436 of "Compilers, Principles, Techniques, and Tools",
 *   by Aho, Sethi, Ullman
 */
int 
hashpjw(char * s, int num_entries)
{
    register char * p;
    register unsigned h = 0;
    register unsigned g;

    for(p = s ; *p != '\0' ; p++) {

        h = (h << 4) + (*p);

        if((g = h&0xf0000000)) {

            h = h ^ (g >> 24);
            h = h ^ g;

        }

    }

  return h % num_entries;

}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ===================== UTILITY ============================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

tree *
shallow_clone_node(tree * origNode)
{
    tree * copyNode = NULL;
    tree * copyNodeChild = NULL;

    if(!origNode) return NULL;

    copyNode = malloc_node(copyNode, SINGLE_PARSE);
    copyNode->number = origNode->number;
    copyNode->type   = origNode->type;
    if(origNode->string) {
        copyNode->string = strdup(origNode->string);
        copyNode->freeMe = 1;
    }

    copyNodeChild = malloc_node(copyNodeChild, 1);
    copyNodeChild->number = origNode->child[0]->number;
    copyNodeChild->type   = origNode->child[0]->type;
    if(origNode->child[0]->string) {
        copyNodeChild->string = strdup(origNode->child[0]->string);
        copyNodeChild->freeMe = 1;
    }

    copyNode->child[0] = copyNodeChild;

    return copyNode;

}

/* ============================================================ */

tree *
clone_node(tree * origNode)
{
    int i;
    tree * copyNode = NULL;

    if(!origNode) return NULL;

    copyNode = malloc_node(copyNode, SAVE_FOR_ALL_PARSES);
    copyNode->number = origNode->number;
    copyNode->type   = origNode->type;
    if(origNode->string) {
        copyNode->string = strdup(origNode->string);
        copyNode->freeMe = 1;
    }

    for(i = 0 ; i < MAX_CHILDREN ; i++)
        if(origNode->child[i])
            copyNode->child[i] = clone_node(origNode->child[i]);

    if(origNode->next_tree_node) 
        copyNode->next_tree_node = clone_node(origNode->next_tree_node);

    /* table of enumerated items for object */
    /* struct tree_tag * * enumeratedItems; */

    return copyNode;

}

/* ============================================================ */

/* 
 * INT_TO_STRING: Converts a number into a string.
 * The size of s limits the size of the number to be converted.
 *
 * Adapted from page 64 of Kernighan and Ritchie
 * "The C Programming Language"
 */
void 
convert_int_to_string(int n, char s[])
{
    int i, sign, j, counter, c;

    if((sign = n) < 0) {
        n = -n;
    }

    i = 0;
    counter = 0;

    /* generate the digits in reverse order
         and place in the string s */
    do {     
        s[i++] = n % 10 + '0';
        counter++;
    } while((n/=10) > 0);

    if(sign < 0) {

        s[i++] = '-';
        counter++;
    }

    /* null terminate */
    s[i] = '\0';

    /* now reverse the string s in place */
    for(i = 0, j = counter-1; i< j;i++, j--) {

        c = s[i];
        s[i] = s[j];
        s[j] = c;

    }

}

/* ============================================================ */

void 
convert_ul_to_string(unsigned long  n, char s[])
{
    int i, j, counter, c;

    i = 0;
    counter = 0;

    /* generate the digits in reverse order
         and place in the string s */
    do {     
        s[i++] = n % 10 + '0';
        counter++;
    } while((n/=10) > 0);


    /* null terminate */
    s[i] = '\0';

    /* now reverse the string s in place */
    for(i = 0, j = counter-1; i< j;i++, j--) {

        c = s[i];
        s[i] = s[j];
        s[j] = c;

    }

}

/* ============================================================ */

int
print_value_list(tree * tmpPtr, FILE * outPtr) {

    int jmpDis = 0;

    /* first item in list */
    /* dotted identifier */
    if(tmpPtr->type == DOT) {

        if(tmpPtr->child[1] && tmpPtr->child[1]->string) {

            fprintf(outPtr, "%s", tmpPtr->child[1]->string);
            jmpDis += (int)strlen(tmpPtr->child[1]->string);

        }

    } else if(tmpPtr->type == DEFVAL) {

        if(tmpPtr->child[child_identity_0] && tmpPtr->child[child_identity_0]->string) {

            fprintf(outPtr, " %s", tmpPtr->child[child_identity_0]->string);
            jmpDis += (int)strlen(tmpPtr->child[child_identity_0]->string);

        }

    /* named item ? */
    } else if(tmpPtr->child[child_identity_0]) {

        if(tmpPtr->child[child_identity_0]->string) {

            fprintf(outPtr, "%s", tmpPtr->child[child_identity_0]->string);
            jmpDis += (int)strlen(tmpPtr->child[child_identity_0]->string);

        }
    /* normal string */
    } else if(tmpPtr->string) {

        if(tmpPtr->type == NEG_NUMBER) {

            fprintf(outPtr, "-%s", tmpPtr->string);
            jmpDis += (int)strlen(tmpPtr->string)+1;
 
        } else {

            fprintf(outPtr, "%s", tmpPtr->string);
            jmpDis += (int)strlen(tmpPtr->string);

        }

    }

    /* all other items in list */
    tmpPtr = tmpPtr->next_tree_node;

    while(tmpPtr) {

        if(tmpPtr->type == DOT) {

            if(tmpPtr->child[1] && tmpPtr->child[1]->string) {

                fprintf(outPtr, ".%s", tmpPtr->child[1]->string);
                jmpDis += (int)strlen(tmpPtr->child[1]->string) + 1;

            }

        /* named item ? */
        } else if(tmpPtr->child[child_identity_0]) {

            if(tmpPtr->child[child_identity_0]->string) {

                fprintf(outPtr, ".%s", tmpPtr->child[child_identity_0]->string);
                jmpDis += (int)strlen(tmpPtr->child[child_identity_0]->string);

            }
        } else if(tmpPtr->string) {

                fprintf(outPtr, ".%s", tmpPtr->string);
                jmpDis += (int)strlen(tmpPtr->string) + 1;

        }

        tmpPtr = tmpPtr->next_tree_node;

    }


    return jmpDis;

}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ====================== MALLOC ============================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* mallocs space, checks return, clears */
tree * 
malloc_node(tree * node, int singleParseOnly)
{
    node = (tree *)malloc(sizeof(tree));
    if(node) {
        memset((void *)node, 0, sizeof(tree));
    } else {
        fprintf(stderr, "*** ERROR ***  Compiler abort - no memory for new tree node!\n");
        exit(1);
    }

    /* trees are freed at end of parse, aux-types at end of all processing */
    if(singleParseOnly) {
        node->freePtr = singleParseNodeList;
        singleParseNodeList = node;
    } else {
        node->freePtr = allParsesNodeList;
        allParsesNodeList = node;
    }

    return(node);

}

/* ============================================================ */
void
dump_table(tree * table[]) {

    int i, haveOne;
    tree * entry_ptr;

    for(i = 0 ; i < TABLE_SIZE ; i++) {

        entry_ptr = table[i];
    
        if(entry_ptr) {
            haveOne = 1;
        } else {
            haveOne = 0;
        }

        while( entry_ptr ) {

            if(entry_ptr->child[0] && entry_ptr->child[0]->string) {

                printf("%s ", entry_ptr->child[0]->string);

            }

            entry_ptr = entry_ptr->next_symbol_node;
        }

        if(haveOne) printf("\n");

    }

}

/* ============================================================ */

symbol_table * 
malloc_table(symbol_table * new_table)
{
    new_table = (symbol_table *)malloc(TABLE_SIZE*sizeof(tree *));

    if(!new_table) {
        fprintf(stderr, "*** ERROR ***  Compiler abort - no memory for new symbol table!\n");
        exit(1);
    }

    /* clear table */
    memset((void *)new_table, 0, TABLE_SIZE*sizeof(tree *));

    return(new_table);
    
}

/* ============================================================ */

/* reuses fileNameBuffer,
   clears old outputFileName, sets new
   sets outputFilePtr
*/

void
open_output_file(char * desiredOutputName)
{

    FILE * tmpFilePtr;
    int fileCounter = 0, rename = 0;


    memset(fileNameBuffer, 0, MYBUFSIZE);
    sprintf(fileNameBuffer, "%s", desiredOutputName);

    /* does the output file already exist? */
    while( (tmpFilePtr = fopen(fileNameBuffer, "r")) != NULL) {

        if((!globalSuppressOutputFlag) && (globalVerboseOutputFlag) && 
		 (!globalBackupOutputFlag)) {
            printf("\n*** NOTE ***  Output file \"%s\" already exists. Overwriting...\n", fileNameBuffer);
        }

        memset(fileNameBuffer, 0, MYBUFSIZE);
        sprintf(fileNameBuffer, "%s_%03d", desiredOutputName, fileCounter++);

        fclose(tmpFilePtr);
	if(globalBackupOutputFlag) {
          rename = 1;
	}

    }

    if(rename) {

        /* rename .def file to next free _??? counter */

        memset(lineBuf, 0, MYBUFSIZE );
#ifndef SR_WINDOWS
        strcpy(lineBuf, "mv ");
#else
        strcpy(lineBuf, "rename ");
#endif /* SR_WINDOWS */
        strcat(lineBuf, desiredOutputName);
        strcat(lineBuf, " ");
        strcat(lineBuf, fileNameBuffer);

        system(lineBuf);

    }


    /* now you can use the output filename you wanted */
    /* OPEN OUTPUT FILE */

    if((outputFilePtr = fopen(outputFileName, "w")) == NULL) {
        fprintf(stderr, "\n*** ERROR ***  Can't open \"%s\" for writing.\n", outputFileName);
        exit(1);
    } 

    if((!globalSuppressOutputFlag) && (globalVerboseOutputFlag)) {
        printf("\n*** NOTE ***  Opening output file: \"%s\".\n", outputFileName);
    }

}


/* ============================================================ */

/* first try to find type node in AUX-MIB data from allParsesTypeTable,
   if not found, look in singleParseTypeTable - where the current
   MIB has precedence over any imported MIB
*/

tree * 
find_type(char * lookupName)
{
    tree * typeExistsPtr = NULL;

    if(!lookupName) return NULL;

    /* check if aux mib has type */
    typeExistsPtr = lookup_type(lookupName, allParsesTypeTable);

    /* if not, check regular types */
    if(!typeExistsPtr) {
        typeExistsPtr = lookup_type(lookupName, singleParseTypeTable);
    }
  
    return typeExistsPtr;
}

/* ============================================================ */

void
insert_name_into_emitted_nodes(char * name, int type)
{
    tree * name_node = NULL, * tree_node = NULL;

    if(!name) return;

    memset(fileNameBuffer, 0, MYBUFSIZE);
    sprintf(fileNameBuffer, "%s%d", name, type);

    name_node = malloc_node(name_node, SINGLE_PARSE);
    name_node->string = strdup(fileNameBuffer);
    name_node->freeMe = 1;

    tree_node = malloc_node(tree_node, SINGLE_PARSE);
    tree_node->child[0] = name_node;

    insert_type_node(tree_node, emittedNodes);

}

/* ============================================================ */

tree *
lookup_name_in_emitted_nodes(char * name, int type)
{
    if(!name) return NULL;

    memset(fileNameBuffer, 0, MYBUFSIZE);
    sprintf(fileNameBuffer, "%s%d", name, type);
    return(lookup_type(fileNameBuffer, emittedNodes));
}


/* ============================================================ */

void
load_mib_replacement_table()
{
    int i;

    for(i = 0 ; i < NUM_MIB_REPLACEMENTS ; i++) {
        insert_mib_replacement_node(mibReplacements[i][0], mibReplacements[i][1]);
    }

}

/* ============================================================ */

void
insert_mib_replacement_node(char * oldName, char * newName)
{
    tree * tree_node  = NULL;
    tree * data_node  = NULL;
    tree * tmpTreePtr = NULL;

    /* does an entry already exist */
    tmpTreePtr = lookup_name(oldName, mibReplacementTable);

    /* node exists, add data at end */
    if(tmpTreePtr) {

        data_node = malloc_node(data_node, SAVE_FOR_ALL_PARSES);
        data_node->string = strdup(newName);
        data_node->freeMe = 1;

        tmpTreePtr->next_tree_node->last_tree_node_in_list->next_tree_node = data_node;
        tmpTreePtr->next_tree_node->last_tree_node_in_list = data_node;

    /* no node, new insert */
    } else {

        tree_node = malloc_node(tree_node, SAVE_FOR_ALL_PARSES);
        tree_node->string = strdup(oldName);
        tree_node->freeMe = 1;

        data_node = malloc_node(data_node, SAVE_FOR_ALL_PARSES);
        data_node->string = strdup(newName);
        data_node->freeMe = 1;

        tree_node->next_tree_node = data_node;

        data_node->last_tree_node_in_list = data_node;

        insert_name(tree_node, mibReplacementTable);

    }

}

/* ============================================================ */
void
dump_table1(tree * table[]) {

    int i, haveOne;
    tree * entry_ptr, * tmpPtr;

    for(i = 0 ; i < TABLE_SIZE ; i++) {

        entry_ptr = table[i];
    
        if(entry_ptr) {
            haveOne = 1;
        } else {
            haveOne = 0;
        }

        while( entry_ptr ) {

            if(entry_ptr && entry_ptr->string) {

                printf("%s: ", entry_ptr->string);

            }

            tmpPtr = entry_ptr->next_tree_node;
            while(tmpPtr ) {

                if(tmpPtr && tmpPtr->string) {

                    printf("%s: ", tmpPtr->string);

                }
                tmpPtr = tmpPtr->next_tree_node;
            }


            entry_ptr = entry_ptr->next_symbol_node;
        }

        if(haveOne) printf("\n");

    }

}

/* ============================================================ */
void
check_for_range_overlap(tree * rangelist) {

    tree * rangeNode1,  * rangeNode2;
    tree * startRange1, * startRange2;
    tree * endRange1,   * endRange2;
    tree * prevPtr;
    char tmp_string[255];
    char print_note_string[255];

    int retValue = 0;


    rangeNode1     = rangelist;

    while(rangeNode1) {

        startRange1 = rangeNode1->child[chld_strt_rnge12];
        endRange1   = rangeNode1->child[chld_end_range13];

        if(!startRange1) {
            startRange1 = endRange1;
        }

        rangeNode2 = rangeNode1->next_tree_node;
        while(rangeNode2) {

            startRange2 = rangeNode2->child[chld_strt_rnge12];
            endRange2   = rangeNode2->child[chld_end_range13];

            if(!startRange2) {
                startRange2 = endRange2;
            }

            /* startRange1 is positive */
            if(startRange1->type == POS_NUMBER) {

                /* endRange1 is positive */
                if(endRange1->type == POS_NUMBER) {

                    /* startRange2 is positive */
                    if(startRange2->type == POS_NUMBER) {

                        /* ++ ++ endRange2 is positive */
                        if(endRange2->type == POS_NUMBER) {

                            if((startRange1->number >= startRange2->number) && (startRange1->number <= (endRange2->number+1))) {

                                /* is 1 completely contained in 2 */
                                if(endRange1->number <= endRange2->number) {

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                } else {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                }

                                retValue = 1;

                            } else if((startRange2->number >= startRange1->number) && (startRange2->number <= (endRange1->number+1))) {

                                /* is 2 completely contained in 1 */
                                if(endRange2->number <= endRange1->number) {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                } else {

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                }

                                retValue = 1;

                            } 

                        } 
                        /* ++ +- endRange2 is negative - CAN'T HAPPEN*/

                    /* startRange2 is negative */
                    } else {

                        /* ++ -+ endRange2 is positive */
                        if(endRange2->type == POS_NUMBER) {

                            if( startRange1->number <= (endRange2->number+1)) {

                                /* is 1 completely contained in 2 */
                                if(endRange1->number <= endRange2->number) {

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                } else {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                }

                                retValue = 1;

                            } 

                        }
                        /* ++ -- endRange2 is negative - no overlap possible*/


                    }

                }
                /* endRange1 is negative - CAN'T HAPPEN*/


            /* startRange1 is negative */
            } else {

                /* endRange1 is positive */
                if(endRange1->type == POS_NUMBER) {

                    /* startRange2 is positive */
                    if(startRange2->type == POS_NUMBER) {

                        /* -+ ++ endRange2 is positive */
                        if(endRange2->type == POS_NUMBER) {

                            if( startRange2->number <= (endRange1->number+1) ) {

                                /* is 2 completely contained in 1 */
                                if(endRange2->number <= endRange1->number) {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                } else {

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;


                                }

                                retValue = 1;

                            } 

                        }
                        /* -+ +- endRange2 is negative  - CAN'T HAPPEN */


                    /* startRange2 is negative */
                    } else {

                        /* -+ -+ endRange2 is positive */
                        if(endRange2->type == POS_NUMBER) {

                            if( startRange1->number <= startRange2->number) {

                                /* is 1 completely contained in 2 */
                                if(endRange1->number <= endRange2->number) {

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                } else {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                }

                                retValue = 1;

                            } else if( startRange2->number <= startRange1->number ) {

                                /* is 2 completely contained in 1 */
                                if(endRange2->number <= endRange1->number) {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                } else {

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                }

                                retValue = 1;

                            } 


                        /* -+ -- endRange2 is negative */
                        } else {

                            if((startRange1->number <= startRange2->number) && (startRange1->number >= (endRange2->number-1))) {

                                endRange2->number = endRange1->number;
                                if(endRange2->string) {
                                    free(endRange2->string);
                                }
                                endRange2->string = strdup(endRange1->string);
                                endRange2->type = endRange1->type;

                                /* set type to null so not printed */
                                rangeNode1->type = 0;

                                retValue = 1;

                            } else if(startRange2->number <= startRange1->number ) {

                                /* 2 completely contained in 1 */

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;



                                retValue = 1;

                            } 

                        }

                    }

                /* endRange1 is negative */
                } else {

                    /* startRange2 is positive - no cases to consider */
                        /* -- ++ endRange2 is positive - no overlap possible*/
                        /* -- +- endRange2 is negative - not possible */

                    /* startRange2 is negative */
                    if(startRange2->type == NEG_NUMBER) {

                        /* -- -+ endRange2 is positive */
                        if(endRange2->type == POS_NUMBER) {

                            if( startRange1->number <= startRange2->number) {

                                /* 1 completely contained in 2 */

                                /* set type to null so not printed */
                                rangeNode1->type = 0;

                                retValue = 1;

                            } else if((startRange2->number <= startRange1->number) && (startRange2->number >= (endRange1->number-1))) {

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;


                                retValue = 1;

                            } 

                        /* -- -- endRange2 is negative */
                        } else {

                            if((startRange1->number <= startRange2->number) && (startRange1->number >= (endRange2->number-1))) {

                                /* is 1 completely contained in 2 */
                                if(endRange1->number >= endRange2->number) {

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                } else {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;

                                }

                                retValue = 1;

                            } else if((startRange2->number <= startRange1->number) && (startRange2->number >= (endRange1->number-1))) {

                                /* is 2 completely contained in 1 */
                                if(endRange2->number >= endRange1->number) {

                                    endRange2->number = endRange1->number;
                                    if(endRange2->string) {
                                        free(endRange2->string);
                                    }
                                    endRange2->string = strdup(endRange1->string);
                                    endRange2->type = endRange1->type;

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;



                                } else {

                                    startRange2->number = startRange1->number;
                                    if(startRange2->string) {
                                        free(startRange2->string);
                                    }
                                    startRange2->string = strdup(startRange1->string);
                                    startRange2->type = startRange1->type;

                                    /* set type to null so not printed */
                                    rangeNode1->type = 0;


                                }

                                retValue = 1;

                            } 


                        }

                    }

                }

            }

            rangeNode2 = rangeNode2->next_tree_node;
        }


        rangeNode1 = rangeNode1->next_tree_node;
    }

    if(retValue && !globalSuppressOutputFlag && !importParseFlag) {

        rangeNode1 = rangelist;

	memset(tmp_string, '\0', 255);
	memset(print_note_string, '\0', 255);
    
	sprintf(print_note_string,
		"*** NOTE ***  Range overlaps automatically adjusted to: ");
        while(rangeNode1) {

            if(rangeNode1->type) {

                startRange1 = rangeNode1->child[chld_strt_rnge12];
                endRange1   = rangeNode1->child[chld_end_range13];

                if(!startRange1) {
                    startRange1 = endRange1;
                }

                if(startRange1->type == NEG_NUMBER) {
                    /*fprintf(stderr, "-%lu..", startRange1->number);*/
		    sprintf(tmp_string, "-%lu..", startRange1->number);
		    strcat(print_note_string, tmp_string);
		    memset(tmp_string, '\0', 255);
                } else {
                    /*fprintf(stderr, "%lu..", startRange1->number);*/
		    sprintf(tmp_string, "%lu..", startRange1->number);
		    strcat(print_note_string, tmp_string);
		    memset(tmp_string, '\0', 255);
                }

                if(endRange1->type == NEG_NUMBER) {
                    /*fprintf(stderr, "-%lu", endRange1->number);*/
                    sprintf(tmp_string, "-%lu", endRange1->number);
		    strcat(print_note_string, tmp_string);
		    memset(tmp_string, '\0', 255);
                } else {
                    /*fprintf(stderr, "%lu", endRange1->number);*/
                    sprintf(tmp_string, "%lu", endRange1->number);
		    strcat(print_note_string, tmp_string);
		    memset(tmp_string, '\0', 255);
                }

            }

            prevPtr = rangeNode1;

            rangeNode1 = rangeNode1->next_tree_node;

            if(prevPtr->type && rangeNode1 && rangeNode1->type) {
		 /*fprintf(stderr, "|");*/
		 sprintf(tmp_string, "|");
		 strcat(print_note_string, tmp_string);
		 memset(tmp_string, '\0', 255);
	    }

        }

        print_note(print_note_string, lineBuf);

    }

}

/* ============================================================ */
void
check_range_limits(tree * rangelist, int flag) {

    tree * rangeNode1 = rangelist;
    tree * startRange1 = NULL;
    tree * endRange1   = NULL;
    tree * prevPtr;
    char tmp_string[255];
    char print_note_string[255];

    int retValue = 0;

    unsigned long minValue = 0;
    unsigned long maxValue = 0;

    char * minValueString = NULL;
    char * maxValueString = NULL;


    /* INTEGER and Integer32 */
    if(flag == INTEGER) {
        minValue       = 2147483648U;
        maxValue       = 2147483647U;
        minValueString = "2147483648";
        maxValueString = "2147483647";

    /* Unsigned32, Gauge32, Gauge,  UInteger32, TimeTicks  */
    } else {
        minValue       = 0U;
        maxValue       = 4294967295U;
        minValueString = "0";
        maxValueString = "4294967295";
    }
    
    while(rangeNode1) {

        startRange1 = rangeNode1->child[chld_strt_rnge12];
        endRange1   = rangeNode1->child[chld_end_range13];

        if(!startRange1) {
            startRange1 = endRange1;
        }


        if(startRange1->type == NEG_NUMBER) {

            if(endRange1->type == NEG_NUMBER) {

                if(endRange1->number > minValue) {

                    endRange1->number = minValue;
                    if(endRange1->string) {
                        free(endRange1->string);
                    }
                    endRange1->string = strdup(minValueString);

                    retValue = 1;
                }


                /* -- start is actually greater than end */
                if(startRange1->number < endRange1->number) {

                    startRange1->number = endRange1->number;
                    if(startRange1->string) {
                        free(startRange1->string);
                    }
                    startRange1->string = strdup(endRange1->string);

                    retValue = 1;
                }                    


            } else {

                if(endRange1->number > maxValue) {

                    endRange1->number = maxValue;
                    if(endRange1->string) {
                        free(endRange1->string);
                    }
                    endRange1->string = strdup(maxValueString);

                    retValue = 1;

                }

                /* -+ start is always less than end */

            }


            if(startRange1->number > minValue) {

                startRange1->number = minValue;
                if(startRange1->string) {
                    free(startRange1->string);
                }
                startRange1->string = strdup(minValueString);

                retValue = 1;
            }


        } else {

            if(endRange1->type == NEG_NUMBER) {

                /* +- start is always greater than end */
                retValue = 1;

                startRange1->number = endRange1->number;
                if(startRange1->string) {
                    free(startRange1->string);
                }
                startRange1->string = strdup(endRange1->string);
                startRange1->type = endRange1->type;

            } else {

                if(endRange1->number > maxValue) {

                    endRange1->number = maxValue;
                    if(endRange1->string) {
                        free(endRange1->string);
                    }
                    endRange1->string = strdup(maxValueString);

                    retValue = 1;

                }

                /* ++ if start is greater than end */
                if(startRange1->number > endRange1->number) {
                    retValue = 1;

                    startRange1->number = endRange1->number;
                    if(startRange1->string) {
                        free(startRange1->string);
                    }
                    startRange1->string = strdup(endRange1->string);

                }                    

            }

            if(startRange1->number > maxValue) {

                startRange1->number = maxValue;
                if(startRange1->string) {
                    free(startRange1->string);
                }
                startRange1->string = strdup(maxValueString);

                retValue = 1;

            }


        }


        if(startRange1 && (startRange1->number == 0)) {
            startRange1->type = POS_NUMBER;
        }

        if(endRange1 && (endRange1->number == 0)) {
            endRange1->type = POS_NUMBER;
        }

        rangeNode1 = rangeNode1->next_tree_node;
    }

    if(retValue && !globalSuppressOutputFlag && !importParseFlag) {

        rangeNode1 = rangelist;

        memset(tmp_string, '\0', 255);
        memset(print_note_string, '\0', 255);
   
        sprintf(print_note_string,
                "*** NOTE ***  Range limits automatically adjusted to: ");
    
        while(rangeNode1) {

            if(rangeNode1->type) {

                startRange1 = rangeNode1->child[chld_strt_rnge12];
                endRange1   = rangeNode1->child[chld_end_range13];

                if(!startRange1) {
                    startRange1 = endRange1;
                }

                if(startRange1->type == NEG_NUMBER) {
                    /*fprintf(stderr, "-%lu..", startRange1->number);*/
                    sprintf(tmp_string, "-%lu..", startRange1->number);
                    strcat(print_note_string, tmp_string);
                    memset(tmp_string, '\0', 255);
                } else {
                    /*fprintf(stderr, "%lu..", startRange1->number);*/
                    sprintf(tmp_string, "%lu..", startRange1->number);
                    strcat(print_note_string, tmp_string);
                    memset(tmp_string, '\0', 255);
                }

                if(endRange1->type == NEG_NUMBER) {
                    /*fprintf(stderr, "-%lu", endRange1->number);*/
                    sprintf(tmp_string, "-%lu", endRange1->number);
                    strcat(print_note_string, tmp_string);
                    memset(tmp_string, '\0', 255);
                } else {
                    /*fprintf(stderr, "%lu", endRange1->number);*/
                    sprintf(tmp_string, "%lu", endRange1->number);
                    strcat(print_note_string, tmp_string);
                    memset(tmp_string, '\0', 255);
                }

            }

            prevPtr = rangeNode1;

            rangeNode1 = rangeNode1->next_tree_node;

            if(prevPtr->type && rangeNode1 && rangeNode1->type) {
		/* fprintf(stderr, "|");*/
                 sprintf(tmp_string, "|");
                 strcat(print_note_string, tmp_string);
                 memset(tmp_string, '\0', 255);
	    }
        }

        print_note(print_note_string, lineBuf);
    }

}

/* ============================================================ */
void
fill_in_max_min_values(tree * rangelist, int flag) {

    tree * rangeNode1 = rangelist;
    tree * startRange1, * endRange1;
    tree * prevPtr;

    int retValue = 0;

    int minType;

    unsigned long minValue;
    unsigned long maxValue;
    char * minValueString;
    char * maxValueString;

    char tmp_string[255];
    char print_note_string[255];

    /* INTEGER and Integer32 */
    if(flag == INTEGER) {
        minValue       = 2147483648U;
        maxValue       = 2147483647U;
        minValueString = "2147483648";
        maxValueString = "2147483647";
        minType        = NEG_NUMBER;

    /* Unsigned32, Gauge32, Gauge,  UInteger32, TimeTicks  */
    } else {
        minValue       = 0U;
        maxValue       = 4294967295U;
        minValueString = "0";
        maxValueString = "4294967295";
        minType        = POS_NUMBER;
    }
    
    while(rangeNode1) {

        startRange1 = rangeNode1->child[chld_strt_rnge12];
        endRange1   = rangeNode1->child[chld_end_range13];

        if(startRange1) {
            if(startRange1->type == MAX) {
                startRange1->number = maxValue;
                startRange1->string = maxValueString;
                startRange1->type   = POS_NUMBER;
                retValue = 1;
            } else if(startRange1->type == MIN) {
                startRange1->number = minValue;
                startRange1->string = minValueString;
                startRange1->type   = minType;
                retValue = 1;
            }
        }

        if(endRange1->type == MAX) {
            endRange1->number = maxValue;
            endRange1->string = maxValueString;
            endRange1->type   = POS_NUMBER;
            retValue = 1;
        } else if(endRange1->type == MIN) {
            endRange1->number = minValue;
            endRange1->string = minValueString;
            endRange1->type   = minType;
            retValue = 1;
        }

        rangeNode1 = rangeNode1->next_tree_node;
    }

    if(retValue && !globalSuppressOutputFlag && !importParseFlag) {

        rangeNode1 = rangelist;
    
        memset(tmp_string, '\0', 255);
        memset(print_note_string, '\0', 255);

        sprintf(print_note_string,
                "*** NOTE ***  MAX and MIN substitutions made to: ");

        while(rangeNode1) {

            if(rangeNode1->type) {

                startRange1 = rangeNode1->child[chld_strt_rnge12];
                endRange1   = rangeNode1->child[chld_end_range13];

                if(!startRange1) {
                    startRange1 = endRange1;
                }

                if(startRange1->type == NEG_NUMBER) {
                    /*fprintf(stderr, "-%lu..", startRange1->number);*/
                    sprintf(tmp_string, "-%lu..", startRange1->number);
                    strcat(print_note_string, tmp_string);
                    memset(tmp_string, '\0', 255);
                } else {
                    /*fprintf(stderr, "%lu..", startRange1->number);*/
                    sprintf(tmp_string, "%lu..", startRange1->number);
                    strcat(print_note_string, tmp_string);
                    memset(tmp_string, '\0', 255);
                }

                if(endRange1->type == NEG_NUMBER) {
                    /*fprintf(stderr, "-%lu", endRange1->number);*/
                    sprintf(tmp_string, "-%lu", endRange1->number);
                    strcat(print_note_string, tmp_string);
                    memset(tmp_string, '\0', 255);
                } else {
                    /*fprintf(stderr, "%lu", endRange1->number);*/
                    sprintf(tmp_string, "%lu", endRange1->number);
                    strcat(print_note_string, tmp_string);
                    memset(tmp_string, '\0', 255);
                }

            }

            prevPtr = rangeNode1;

            rangeNode1 = rangeNode1->next_tree_node;

            if(prevPtr->type && rangeNode1 && rangeNode1->type)  {
		/*fprintf(stderr, "|");*/
                 sprintf(tmp_string, "|");
                 strcat(print_note_string, tmp_string);
                 memset(tmp_string, '\0', 255);
            }

        }
        print_note(print_note_string, lineBuf);

    }

}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ============================================================ */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* FAM ***** Newly added code to support linked list creation *****/

void
ll_emit_all_nodes(tree * ptr, FILE * outputFilePtr)
{
    int i;
 
    /* safety catch */
    if(!ptr)         return;
 
    ll_emit_node(ptr, outputFilePtr);
 
    for(i = 0 ; i < MAX_CHILDREN ; i++)
        if(ptr->child[i])
            ll_emit_all_nodes(ptr->child[i], outputFilePtr);
 
    if(ptr->next_tree_node){
        ll_emit_all_nodes(ptr->next_tree_node, outputFilePtr);
    }


   return;

}

void
ll_emit_node(tree * ptr, FILE * outputFilePtr)
{

    tree * alreadyEmittedPtr = NULL;
    tree * tmpPtr = NULL;
    tree * tmpPtr2 = NULL;
    char * value = NULL;
    ObjData *llnode = NULL;
    RevisionInfo *tmpRevInfo = NULL;
    RevisionInfo *newRevInfo = NULL;
    ModuleComp *newModule = NULL; 
    ModuleComp *tmpModule = NULL;
    TC_ObjData *tcnode = NULL;
    IM_ObjData *imnode = NULL;
 
    int istable = 0;
    int i = 0;
    int strsize;
    int tmpValue = 0;
    tree * lookupResult = NULL;
    int jmpDis = 0;

    /* safety catch */
    if(!ptr) return;

    if(ptr->type == DOT){
          printf("DOT type\n");
    }

    /* check whether already emitted for this parse */
    if(ptr->child[0] && ptr->child[0]->string) {
 
        alreadyEmittedPtr = lookup_name_in_emitted_nodes(ptr->child[0]->string, ptr->type);
 
        if(!alreadyEmittedPtr) {
            insert_name_into_emitted_nodes(ptr->child[0]->string, ptr->type);
        } else {
            return;
        }
 
    }

    /* trace type definitions back to root */
    /* check_for_type_definition(ptr , outputFilePtr, descrFilePtr); */

    if(ptr->child[child_syntax___1]) {
        check_for_type_definition(ptr->child[child_syntax___1] , outputFilePtr, descrFilePtr);
    }

    if(ptr->parent && ptr->parent->child[child_syntax___1]) {
        check_for_type_definition(ptr->child[child_syntax___1] , outputFilePtr, descrFilePtr);
     }


    switch(ptr->type) {

	case IMPORTS:
	   /* Since there can be several IMPORTS items from a single FROM
            * item, may create several linked imnodes in this step
            */

	   /*if only one IMPORTS member, process*/
	   if(ptr->child[child_identity_0] && 
		ptr->child[child_identity_0]->string) {
	     /* create linked list node for imports information */
             if ( (imnode = (IM_ObjData *)malloc(sizeof(IM_ObjData))) == NULL) {
              fprintf(stderr, "Unable to malloc memory for imnode\n");
              return;
             }
             memset((IM_ObjData *)imnode, '\0', sizeof(IM_ObjData));

             /* copy component_type */
             imnode->component_type = strdup("Imports");

 	     /* copy name */
	     imnode->name = strdup(ptr->child[child_identity_0]->string);

	     /* copy FROM value */
             if(ptr->child[child_misc1___14] &&
              ptr->child[child_misc1___14]->string) {
                imnode->type = strdup(ptr->child[child_misc1___14]->string);
             }

             if(formatFlag == XML) {
                ll_add_imnode(imnode);
             }
	   }
		
	   /* see if more IMPORTS members */
           if(formatFlag == XML) {
  	      if(ptr->child[child_identity_0] && 
 	   	 ptr->child[child_identity_0]->next_tree_node) {
                   tmpPtr = ptr->child[child_identity_0]->next_tree_node;
	           do {

	             /* create linked list node for imports information */
                     if ((imnode =(IM_ObjData *)malloc(sizeof(IM_ObjData))) == NULL) {
                        fprintf(stderr, "Unable to malloc memory for imnode\n");
                        return;
                     }
                     memset((IM_ObjData *)imnode, '\0', sizeof(IM_ObjData));
                     /* copy component_type */
                     imnode->component_type = strdup("Imports");

	             /*if string is present go ahead and process*/
                     if(tmpPtr->string) {
	 	        imnode->name = strdup(tmpPtr->string);
	             }

 	             /* copy FROM value */
	             if(ptr->child[child_misc1___14] && 
	                ptr->child[child_misc1___14]->string) {
 		            imnode->type = strdup(ptr->child[child_misc1___14]->string);
	             }

                     ll_add_imnode(imnode);

	             tmpPtr = tmpPtr->next_tree_node;

	           } while(tmpPtr);
	      }
           }

	   break;

        case MODULE_COMPLIANCE:
           /* create linked list node for information */
           if ( (llnode = (ObjData *)malloc(sizeof(ObjData))) == NULL) {
              fprintf(stderr, "Unable to malloc memory for llnode\n");
              return;
           }
           memset((ObjData *)llnode, '\0', sizeof(ObjData));

            /* copy component_type */
            llnode->component_type = strdup("ModuleCompliance");

            /* module compliance name */
           if(ptr->child[child_identity_0] &&
            ptr->child[child_identity_0]->string &&
            ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string ) {
              llnode->name = strdup(ptr->child[child_identity_0]->string);
           }

           /* copy status */
           if(ptr->child[child_status___2] &&
              ptr->child[child_status___2]->string  &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->status = strdup(ptr->child[child_status___2]->string);
           }

           /* copy description */
           if(ptr->child[child_descript_3] &&
              ptr->child[child_descript_3]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->descr = strdup(ptr->child[child_descript_3]->string);
           }

           /* create oid */
           if(ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              value = build_oid(ptr->child[child_identity_0]);
              llnode->oid = value;
	      value = build_id(ptr->child[child_identity_0]);
              llnode->id = value;
           }

	   /*** add modules ***/
	   if(formatFlag == XML) {
  	    if(ptr->child[child_misc1___14] &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {


              if(llnode->module_comp == NULL) {
                llnode->module_comp = (ModuleComp *)malloc(sizeof(ModuleComp));
              }
	      memset((ModuleComp *)llnode->module_comp, '\0', 
			sizeof(ModuleComp));
 	      tmpPtr = ptr->child[child_misc1___14];

              /* module name */
              if((tmpPtr->child[child_identity_0]->child[child_identity_0]) &&
                (tmpPtr->child[child_identity_0]->child[child_identity_0]->
                string)) {
                  llnode->module_comp->name = strdup(tmpPtr->
                     child[child_identity_0]->child[child_identity_0]->string);
              }
              /* mandatory groups */
              if((tmpPtr->child[child_misc1___14]) &&
                 (tmpPtr->child[child_misc1___14]->string)) {
                  llnode->module_comp->mandatory_groups = strdup(tmpPtr->
                        child[child_misc1___14]->string);
              }
              if(tmpPtr->child[child_misc1___14]){
                tmpPtr2 = tmpPtr->child[child_misc1___14]->next_tree_node;
              }

              /* loop through all mandatory groups */
              while(tmpPtr2) {
                if(tmpPtr2->string) {
                   strsize = (strlen(llnode->module_comp->mandatory_groups) +
                        strlen(tmpPtr2->string) + 2);
                   /*reallocate more memory*/
 		   llnode->module_comp->mandatory_groups = (char *)realloc
                        (llnode->module_comp->mandatory_groups, strsize);
                   strcat(llnode->module_comp->mandatory_groups, " ");
                   strcat(llnode->module_comp->mandatory_groups,
			tmpPtr2->string);
                }
                tmpPtr2 = tmpPtr2->next_tree_node;
              }


	      /* conditional group or exception */
	      if(tmpPtr->child[child_misc2___15]->child[child_identity_0]){
		 ll_emit_modcomp(llnode->module_comp, 
			tmpPtr->child[child_misc2___15]);
	      }

	      newModule = llnode->module_comp->next;

	      /* move to the next module */
	      tmpPtr = tmpPtr->next_tree_node;

	    }
	    else {
	      tmpPtr = NULL;
	    }

	    /* loop the total number of modules */
	    while(tmpPtr) {

              if(newModule == NULL) {
                newModule = (ModuleComp *)
                                malloc(sizeof(ModuleComp));
              }
	      memset((ModuleComp *)newModule, '\0', sizeof(ModuleComp));

	      /* module name */
 	      if((tmpPtr->child[child_identity_0]->child[child_identity_0]) &&
		(tmpPtr->child[child_identity_0]->child[child_identity_0]->
		string)) {
                  newModule->name = strdup(tmpPtr->
		     child[child_identity_0]->child[child_identity_0]->string);
	      }
	      /* mandatory groups */
	      if((tmpPtr->child[child_misc1___14]) &&
		 (tmpPtr->child[child_misc1___14]->string)) {
                  newModule->mandatory_groups = strdup(tmpPtr->
                	child[child_misc1___14]->string);
	      }
  	      if(tmpPtr->child[child_misc1___14]){
                tmpPtr2 = tmpPtr->child[child_misc1___14]->next_tree_node;
	      }

	      /* loop through all mandatory groups */
	      while(tmpPtr2) {
		if(tmpPtr2->string) {
		   strsize = (strlen(newModule->mandatory_groups) + 
			strlen(tmpPtr2->string) + 2);
		   /*reallocate more memory*/
  		   newModule->mandatory_groups = (char *)realloc
			(newModule->mandatory_groups, strsize);
		   strcat(newModule->mandatory_groups, " ");
		   strcat(newModule->mandatory_groups, tmpPtr2->string);

		}
		tmpPtr2 = tmpPtr2->next_tree_node;
	      }
	
              /* conditional group or exception */
              if(tmpPtr->child[child_misc2___15]->child[child_identity_0]){
                 ll_emit_modcomp(newModule, tmpPtr->child[child_misc2___15]);
              }

	      /* link the list */
              if(llnode->module_comp->next == NULL) {
                llnode->module_comp->next = newModule;
                newModule->next = NULL;
                tmpModule = llnode->module_comp->next;
              }
              else {
                tmpModule->next = newModule;
                tmpModule = newModule;
                newModule->next = NULL;
              }

	      newModule = NULL;

	      /* move on to next module */
	      tmpPtr = tmpPtr->next_tree_node;
	    } /* end while */
	   } /* end if XML */

	
           ll_add_node(llnode);
           break;

        case MODULE_CONFORMANCE:
           break;

        case NOTIFICATION_GROUP:

           /* create linked list node for information */
           if ( (llnode = (ObjData *)malloc(sizeof(ObjData))) == NULL) {
              fprintf(stderr, "Unable to malloc memory for llnode\n");
              return;
           }
           memset((ObjData *)llnode, '\0', sizeof(ObjData));

            /* copy component_type */
            llnode->component_type = strdup("NotificationGroup");

            /* copy name */
           if(ptr->child[child_identity_0] &&
            ptr->child[child_identity_0]->string &&
            ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string ) {
              llnode->name = strdup(ptr->child[child_identity_0]->string);
           }

           /* copy object type */
           if(ptr->child[child_syntax___1] &&
              ptr->child[child_syntax___1]->string  &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->type = strdup(ptr->child[child_syntax___1]->string);
           }
	   else {
              llnode->type = strdup("nonLeaf");
           }

           /* create oid */
           if(ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
	      value = build_oid(ptr->child[child_value____9]);
	      llnode->oid = value;
	      value = build_id(ptr->child[child_identity_0]);
              llnode->id = value;
           }

           if(formatFlag == XML) {
	      /* copy notifications (saved in misc1___14 member) */
	      if(ptr->child[child_misc1___14] &&
	         ptr->child[child_misc1___14]->string &&
	         ptr->child[child_value____9] &&
                 ptr->child[child_value____9]->string) {
	         llnode->obj_list = build_objlist_info(ptr);
              }
           }

            /* copy status */
            if(ptr->child[child_status___2] &&
              ptr->child[child_status___2]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->status = strdup(ptr->child[child_status___2]->string);
            }

           /* description */
           if(ptr->child[child_descript_3] &&
              ptr->child[child_descript_3]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->descr = strdup(ptr->child[child_descript_3]->string);
           }

           /* revision*/
           if(ptr->child[child_referenc_4] &&
              ptr->child[child_referenc_4]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->reference = strdup(ptr->child[child_referenc_4]->string);
           }

           ll_add_node(llnode);
           break;


        case OBJECT_GROUP:
           /* create linked list node for information */
           if ( (llnode = (ObjData *)malloc(sizeof(ObjData))) == NULL) {
              fprintf(stderr, "Unable to malloc memory for llnode\n");
              return;
           }
           memset((ObjData *)llnode, '\0', sizeof(ObjData));

           /* copy component_type */
           llnode->component_type = strdup("ObjectGroup");

           /* copy name */
           if(ptr->child[child_identity_0] &&
              ptr->child[child_identity_0]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string ) {
              llnode->name = strdup(ptr->child[child_identity_0]->string);
           }

           /* copy object type */
           if(ptr->child[child_syntax___1] &&
              ptr->child[child_syntax___1]->string  &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->type = strdup(ptr->child[child_syntax___1]->string);
           }
	   else {
              llnode->type = strdup("nonLeaf");
           }

           /* create oid */
           if(ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
	      value = build_oid(ptr->child[child_value____9]);
	      llnode->oid = value;
	      value = build_id(ptr->child[child_identity_0]);
              llnode->id = value;
           }

           if(formatFlag == XML){
	   /* copy objects (saved in misc1___14 member) */
	      if(ptr->child[child_misc1___14] &&
	         ptr->child[child_misc1___14]->string &&
	         ptr->child[child_value____9] &&
                 ptr->child[child_value____9]->string) {
	         llnode->obj_list = build_objlist_info(ptr);
              }
           }

            /* copy status */
            if(ptr->child[child_status___2] &&
              ptr->child[child_status___2]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {

              llnode->status = strdup(ptr->child[child_status___2]->string);
            }

           /* description */
           if(ptr->child[child_descript_3] &&
              ptr->child[child_descript_3]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->descr = strdup(ptr->child[child_descript_3]->string);
           }

           /* revision*/
           if(ptr->child[child_referenc_4] &&
              ptr->child[child_referenc_4]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->reference = strdup(ptr->child[child_referenc_4]->string);
           }

           ll_add_node(llnode);
           break;

        case TRAP_TYPE:

#ifdef NOT_YET
           /* create linked list node for information */
           if ( (llnode = (ObjData *)malloc(sizeof(ObjData))) == NULL) {
              fprintf(stderr, "Unable to malloc memory for llnode\n");
              return;
           }
           memset((ObjData *)llnode, '\0', sizeof(ObjData));

           llnode->component_type = strdup("TrapType");

            /* copy name */
           if(ptr->child[child_identity_0] && 
              ptr->child[child_identity_0]->string) {
              llnode->name = strdup(ptr->child[child_identity_0]->string);
           }

           /* copy object part - for a trap */
           if(ptr->child[child_identity_0] && 
              ptr->child[child_identity_0]->string){
              value = build_oid(ptr->child[child_identity_0]);
              llnode->oid = value;
	      value = build_id(ptr->child[child_identity_0]);
              llnode->id = value;
	   }

           ll_add_node(llnode);
          
#endif /* NOT_YET */
           break;

        /* this should be SMIv2 */
        case NOTIFICATION_TYPE:

           /* create linked list node for information */
           if ( (llnode = (ObjData *)malloc(sizeof(ObjData))) == NULL) {
              fprintf(stderr, "Unable to malloc memory for llnode\n");
              return;
           }
           memset((ObjData *)llnode, '\0', sizeof(ObjData));

           /* copy component_type */
           llnode->component_type = strdup("Notification");
 
            /* copy name */
           if(ptr->child[child_identity_0] && 
	      ptr->child[child_identity_0]->string) { 
                llnode->name = strdup(ptr->child[child_identity_0]->string);
           }

           /* create oid */
	   if(ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string ) {
              value = build_oid(ptr->child[child_value____9]);
	      /*BAB adding this in the build_oid function so cleanup_ll 
	       * remains happy
	       */
              /*if(value != NULL){
                   strcat(value, ".");
                   strcat(value, ptr->child[child_value____9]->next_tree_node->string);
                   llnode->oid = value;
               }*/
	       llnode->oid = value;
	       value = build_id(ptr->child[child_identity_0]);
               llnode->id = value;

           }
           if(formatFlag == XML) {
              /* copy objects (saved in child_object___5 member) */
              if(ptr->child[child_object___5] &&
                 ptr->child[child_object___5]->string &&
                 ptr->child[child_value____9] &&
                 ptr->child[child_value____9]->string) {
                 llnode->obj_list = build_objlist_info(ptr);
              }
           }


	   /*BAB - Add description, etc. here ...*/
           /* copy status */
           if(ptr->child[child_status___2] &&
              ptr->child[child_status___2]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {

              llnode->status = strdup(ptr->child[child_status___2]->string);
           }

           /* copy description */
           if(ptr->child[child_descript_3] &&
              ptr->child[child_descript_3]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {

              llnode->descr = strdup(ptr->child[child_descript_3]->string);
           }


           /* copy reference */
           if(ptr->child[child_referenc_4] &&
              ptr->child[child_referenc_4]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {

              llnode->reference = strdup(ptr->child[child_referenc_4]->string);
           }

           ll_add_node(llnode);
           break;

        case AGENT_CAPABILITIES:
           break;

        case MODULE_IDENTITY:       /* module identification definition */

           /* create linked list node for information */
           if ( (llnode = (ObjData *)malloc(sizeof(ObjData))) == NULL) {
              fprintf(stderr, "Unable to malloc memory for llnode\n");
              return;
           }
           memset((ObjData *)llnode, '\0', sizeof(ObjData));

            /* copy component_type */
           llnode->component_type = strdup("ModuleIdentity");

            /* copy name */
           if(ptr->child[child_identity_0] &&
            ptr->child[child_identity_0]->string &&
            ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string ) {
              llnode->name = strdup(ptr->child[child_identity_0]->string);
           }

	   /* Module Identity should not have a syntax. Default to nonLeaf*/
	   llnode->type = strdup("nonLeaf");

           /* create oid */
           if(ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              value = build_oid(ptr->child[child_identity_0]);
              llnode->oid = value;
	      value = build_id(ptr->child[child_identity_0]);
              llnode->id = value;
           }

 	   /*BAB - print the other data ray is getting*/
	   /* stored lastUpdated in the misc1 field */
	   if(ptr->child[child_misc1___14] &&
              ptr->child[child_misc1___14]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->misc = strdup(ptr->child[child_misc1___14]->string);
           }

	   /* stored organization in the status field */
	   if(ptr->child[child_status___2] &&
              ptr->child[child_status___2]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->status = strdup(ptr->child[child_status___2]->string);
           }

	   /* stored contactInfo in the access field */
	   if(ptr->child[child_access__11] &&
              ptr->child[child_access__11]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->access = strdup(ptr->child[child_access__11]->string);
           }

	   /* description */
	   if(ptr->child[child_descript_3] &&
              ptr->child[child_descript_3]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->descr = strdup(ptr->child[child_descript_3]->string);
           }

	   /* revision*/
	   if(ptr->child[child_referenc_4] &&
	      ptr->child[child_referenc_4]->child[child_descript_3] &&
	      ptr->child[child_referenc_4]->child[child_referenc_4] &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              /*llnode->reference = strdup(ptr->child[child_referenc_4]->string);*/
	      if(llnode->revision_list == NULL) {
		llnode->revision_list = (RevisionInfo *) 
				malloc(sizeof(RevisionInfo));
	      }
	      memset((RevisionInfo *)llnode->revision_list, '\0', sizeof(RevisionInfo));
	      
	      llnode->revision_list->description = strdup(ptr->
		child[child_referenc_4]->child[child_descript_3]->string);
	      llnode->revision_list->date = strdup(ptr->
		child[child_referenc_4]->child[child_referenc_4]->string);
	      llnode->revision_list->next = NULL;

	      tmpPtr = ptr->child[child_referenc_4]->next_tree_node;
	      newRevInfo = llnode->revision_list->next;

	      /*BAB -- add a linked list to include all revisions if there
	       * is a ptr->child[child_referenc_4]->next_tree_node
	       */
	      while(tmpPtr) {
		if(tmpPtr->child[child_descript_3] && 
			tmpPtr->child[child_referenc_4]) {
		   newRevInfo = (RevisionInfo *)malloc(sizeof(RevisionInfo));
		   newRevInfo->description = 
			strdup(tmpPtr->child[child_descript_3]->string);
		   newRevInfo->date = 
			strdup(tmpPtr->child[child_referenc_4]->string);

   		   if(llnode->revision_list->next == NULL) {
		  	llnode->revision_list->next = newRevInfo;
		  	newRevInfo->next = NULL;
		  	tmpRevInfo = llnode->revision_list->next;
	           }
		   else {
		 	tmpRevInfo->next = newRevInfo;
		  	tmpRevInfo = newRevInfo;
		  	newRevInfo->next = NULL;
	           }
		}	

		tmpPtr = tmpPtr->next_tree_node;
	      }	
	
           }

           ll_add_node(llnode);
           break;

        case IDENTIFIER:            /* OBJECT IDENTIFIER */
           /* create linked list node for information */
          if ( (llnode = (ObjData *)malloc(sizeof(ObjData))) == NULL) {
              fprintf(stderr, "Unable to malloc memory for llnode\n");
              return;
           }
           memset((ObjData *)llnode, '\0', sizeof(ObjData));

            /* copy component_type */
           llnode->component_type = strdup("ObjIdentifier");

            /* copy name */
           if(ptr->child[child_identity_0] &&
            ptr->child[child_identity_0]->string &&
            ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string ) {
              llnode->name = strdup(ptr->child[child_identity_0]->string);
           }

           /* copy object type */
           if(ptr->child[child_syntax___1] &&
              ptr->child[child_syntax___1]->string  &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->type = strdup(ptr->child[child_syntax___1]->string);
           }
	   else {
	      llnode->type = strdup("nonLeaf");
	   }

           /* copy access */
           if(ptr->child[child_access__11] &&
              ptr->child[child_access__11]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->access = strdup(ptr->child[child_access__11]->string);
           }

           /* create oid */
           if(ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              value = build_oid(ptr->child[child_identity_0]);
              llnode->oid = value;
              llnode->intoid = build_int_oid(value);
	      value = build_id(ptr->child[child_identity_0]);
              llnode->id = value;
           }

           ll_add_node(llnode);
           break;

        case OBJECT_IDENTITY:

           /* create linked list node for information */
           if ( (llnode = (ObjData *)malloc(sizeof(ObjData))) == NULL) {
              fprintf(stderr, "Unable to malloc memory for llnode\n");
              return;
           }
           memset((ObjData *)llnode, '\0', sizeof(ObjData));

           /* copy component_type */
             llnode->component_type = strdup("ObjIdentity");

            /* copy name */
           if(ptr->child[child_identity_0] && 
              ptr->child[child_identity_0]->string && 
              ptr->child[child_value____9] && 
              ptr->child[child_value____9]->string ) {
              llnode->name = strdup(ptr->child[child_identity_0]->string);
           }

           /* copy object type */
           if(ptr->child[child_syntax___1] && 
              ptr->child[child_syntax___1]->string && 
              ptr->child[child_value____9] && 
              ptr->child[child_value____9]->string) {
              llnode->type = strdup(ptr->child[child_syntax___1]->string);

              check_for_type_definition(ptr->child[child_value____9] , outputFilePtr, NULL);
           }
	   else {
              llnode->type = strdup("nonLeaf");
           }

           /* create oid */
           if(ptr->child[child_value____9] && 
              ptr->child[child_value____9]->string) {
              value = build_oid(ptr->child[child_identity_0]);
              llnode->oid = value;
	      value = build_id(ptr->child[child_identity_0]);
              llnode->id = value;
           }

            /* copy status */
            if(ptr->child[child_status___2] &&
              ptr->child[child_status___2]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {

              llnode->status = strdup(ptr->child[child_status___2]->string);
            }

	   /* description */
           if(ptr->child[child_descript_3] &&
              ptr->child[child_descript_3]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->descr = strdup(ptr->child[child_descript_3]->string);
           }

           /* revision*/
           if(ptr->child[child_referenc_4] &&
              ptr->child[child_referenc_4]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {
              llnode->reference = strdup(ptr->child[child_referenc_4]->string);
           }

           ll_add_node(llnode);
           break;

        case DEFINITIONS:

            /* child 0 */

            if(ptr->child[child_identity_0] && 
               ptr->child[child_identity_0]->string) {
                fprintf(stdout, "\n-- object definitions compiled from %s\n\n", ptr->child[child_identity_0]->string);
                moduleNameBuffer = strdup(ptr->child[child_identity_0]->string);
            }
 
            break;

        case OBJECT_TYPE:
           if ( (llnode = (ObjData *)malloc(sizeof(ObjData))) == NULL) {
              fprintf(stderr, "Unable to malloc memory for llnode\n");
              return;
           }
           memset((ObjData *)llnode, '\0', sizeof(ObjData));
           llnode->augments=0;

	   /* SEQUENCE OF, virtual table */
	   if(ptr->child[child_syntax___1] &&                                  
               ptr->child[child_syntax___1]->type == OF) {                      
               istable=1;                                                       
           }                            

           if(ptr->child[child_value____9] && ptr->child[child_value____9]->string) {
                check_for_type_definition(ptr->child[child_value____9] , outputFilePtr, NULL);
            }

           /* copy name */
           if(ptr->child[child_identity_0] && 
              ptr->child[child_identity_0]->string && 
              ptr->child[child_value____9] && 
              ptr->child[child_value____9]->string) {
              llnode->name = strdup(ptr->child[child_identity_0]->string);
           }

           /* copy object type */
           if(ptr->child[child_syntax___1] && 
              ptr->child[child_syntax___1]->string && 
              ptr->child[child_value____9] && 
              ptr->child[child_value____9]->string) {

              llnode->type = strdup(ptr->child[child_syntax___1]->string);

	      /* check for type to retain primitive type for 
	       * textual convention. Do not do this if application type.  
	       */
              
              if(!application_type_test(ptr->child[child_syntax___1]->string)){
                   tmpPtr = find_type(ptr->child[child_syntax___1]->string);
              }

              /* use the primitive type for textual conventions */
              /* which are not enumerations */

              if(tmpPtr && tmpPtr->child &&
                     tmpPtr->child[child_syntax___1]->string){


                 /* Capture Textual Convention * Enumeration */
                 if( tmpPtr->child[child_identity_0] &&
                   tmpPtr->child[child_identity_0]->string &&
                   tmpPtr->child[child_syntax___1] &&
                   tmpPtr->child[child_syntax___1]->child[child_status___2] ) {
                     llnode->enum_list = build_enumeration_info(tmpPtr);
                 }

                 /* Capture textual convention range */
                 if(llnode->range == NULL){
                   if( tmpPtr->child[child_syntax___1] &&
                    tmpPtr->child[child_syntax___1]->child[child_range____6] ) {
                      llnode->range[0] = 
			add_range_info(tmpPtr->child[child_syntax___1]->
				child[child_range____6]);

		      /*BAB - see if there is another range 
		       *in the list 
		       */
                      if(ptr->child[child_syntax___1]->
			      child[child_range____6]->next_tree_node && 
			ptr->child[child_syntax___1]->child[child_range____6]->
				next_tree_node->child[chld_end_range13]) {
                        i = 1;
                        tmpPtr = ptr->child[child_syntax___1]->
                                	child[child_range____6]->next_tree_node;

                        /** loop until no more in the list **/
                        while(i) {
                          llnode->range[i] = add_range_info(tmpPtr);
                          if (tmpPtr->next_tree_node && tmpPtr->
                                next_tree_node->child[chld_end_range13]) {
                            tmpPtr = tmpPtr->next_tree_node;
                            i++;
                          }
                          else {
                            i = 0;
                          }
                        } /* end while */
                      }
		      /*BAB*/

                    }
                  } /* end if range == NULL */

                  if(application_type_test(tmpPtr->
			child[child_syntax___1]->string)){
                    llnode->primtype = strdup(tmpPtr->
			child[child_syntax___1]->string);
                  } 
		  else {
                    llnode->primtype = find_prim_type(tmpPtr->
			child[child_syntax___1]->string);
                  }
               }   /* if tmpPtr */
            } 
	    else {
              llnode->type = strdup("nonLeaf");
            }

           /* copy range */
           if(llnode->range[0] == NULL){
              if( ptr->child[child_syntax___1] && 
                  ptr->child[child_syntax___1]->child[child_range____6] ) {
                    llnode->range[0] = add_range_info(ptr->child[child_syntax___1]->child[child_range____6]);

	    	  /*BAB - see if there is another range
                  *in the list
                  */
                  if(ptr->child[child_syntax___1]->child[child_range____6]->
			next_tree_node && ptr->child[child_syntax___1]->
			child[child_range____6]->next_tree_node->
			child[chld_end_range13]) {
                    i = 1;
                    tmpPtr = ptr->child[child_syntax___1]->
                                 child[child_range____6]->next_tree_node;

                    /** loop until no more in the list **/
                    while(i) {
                       llnode->range[i] = add_range_info(tmpPtr);
                       if (tmpPtr->next_tree_node && tmpPtr->
                            next_tree_node->child[chld_end_range13]) {
                          tmpPtr = tmpPtr->next_tree_node;
                          i++;
                       }
                       else {
                         i = 0;
                       }
                    } /* end while */
                 }
                 /*BAB*/

              }
           }

           /* copy access */
           if(ptr->child[child_access__11] && 
              ptr->child[child_access__11]->string && 
              ptr->child[child_value____9] && 
              ptr->child[child_value____9]->string) {
              llnode->access = strdup(ptr->child[child_access__11]->string);
           }
           /* copy value */
           if(ptr->child[child_value____9] && 
              ptr->child[child_value____9]->string) {
              value = build_oid(ptr->child[child_identity_0]);
              llnode->oid = value;
	      value = build_id(ptr->child[child_identity_0]);
              llnode->id = value;
           }

           /* check if object is an index or augments object */
           if ( ptr->child[child_index____7] &&
                (ptr->child[child_index____7]->type == INDEX ||
		 ptr->child[child_index____7]->type == AUGMENTS) ) {
                llnode->index_list = build_index_info(ptr);
		istable = 1;
              if ( ptr->child[child_index____7] &&
                  ptr->child[child_index____7]->type == AUGMENTS ) {
                  llnode->augments = 1;
              }
           }

           /* check for enumeration */
            if( ptr->child[child_identity_0] &&
                ptr->child[child_identity_0]->string &&
                ptr->child[child_syntax___1] &&
                ptr->child[child_syntax___1]->child[child_status___2] ) {
             
                llnode->enum_list = build_enumeration_info(ptr);
            }

            /* copy component_type */
            if(istable) {
              llnode->component_type = strdup("Table");
	      /* if a table, replace nonLeaf type with Aggregate */
              if(llnode->type) free(llnode->type);
              llnode->type = strdup("Aggregate");
              istable = 0; 
            }
            else {
              llnode->component_type = strdup("Scalar");
            }

	    /* copy status */
	    if(ptr->child[child_status___2] &&
              ptr->child[child_status___2]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {

	      llnode->status = strdup(ptr->child[child_status___2]->string);
	    }

 	    /* copy description */
	    if(ptr->child[child_descript_3] &&
              ptr->child[child_descript_3]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {

              llnode->descr = strdup(ptr->child[child_descript_3]->string);
            }   


 	    /* copy reference */
	    if(ptr->child[child_referenc_4] &&
              ptr->child[child_referenc_4]->string &&
              ptr->child[child_value____9] &&
              ptr->child[child_value____9]->string) {

              llnode->reference = strdup(ptr->child[child_referenc_4]->string);
            }   
           
	    /* copy defval */
	    if(ptr->child[10] &&
	       ptr->child[10]->child[0] &&
	       ptr->child[10]->child[0]->string) {

	       llnode->defval = strdup(ptr->child[10]->child[0]->string);
	
	       /*see if any more in the list (primarily for BITS)*/
	       if(ptr->child[10]->child[0]->next_tree_node &&
		  ptr->child[10]->child[0]->next_tree_node->string) {
		  tmpPtr2 = ptr->child[10]->child[0]->next_tree_node;

		  while(tmpPtr2) {
		    if(tmpPtr2->string) {
                      strsize = (strlen(llnode->defval) + 
			strlen(tmpPtr2->string) + 3);
                      llnode->defval = (char *)realloc(llnode->defval, strsize);
                      strcat(llnode->defval, ", ");
                      strcat(llnode->defval, tmpPtr2->string);
                      tmpPtr2 = tmpPtr2->next_tree_node;
		    }
		    else {
		      tmpPtr2 = NULL;
		    }
		  }
		}

	    }
	
           ll_add_node(llnode);
           break;

        case DOT:
           printf("DOT  ");
           if(ptr->child[1] && ptr->child[1]->string) {
              printf(".%s\n", ptr->child[1]->string);
              /*llnode->oid = strdup(ptr->child[child_value____9]->string); */
           }
           break;

        case DEFVAL:

            /* DEFVAL*/
            if( ptr->parent && ptr->parent->child[child_syntax___1] && ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {

                /* BITS values must be ORed, lookup values in enumeratedItems table and OR */
                if(ptr->parent->child[child_syntax___1]->type == BITS) {

                    tmpValue = 0;

                    tmpPtr = ptr->child[child_identity_0];
                    while(tmpPtr) {
            
                        if(tmpPtr->string) {

                            if(ptr->parent->child[child_syntax___1]->enumeratedItems) {

                                lookupResult = lookup_name(tmpPtr->string, ptr->parent->child[child_syntax___1]->enumeratedItems);

                                /* bitwise OR converted number with previous tmpValue */
                                if(lookupResult) {
                                    tmpValue = tmpValue | atoi(lookupResult->child[child_identity_0]->string);
                                }

                            }
                        }

                        tmpPtr = tmpPtr->next_tree_node;
                    }


                    /* column 1 */
                    if(formatFlag == 0){
                     fprintf(outputFilePtr, "%%defval ");

                     for(i = 0 ; i < (maxColumnWidth - 7) ; i++) {
                        fprintf(outputFilePtr, " ");
                     }   

                     if(ptr->parent->child[child_identity_0] && ptr->parent->child[child_identity_0]->string){

                        /* column 2 */
                        fprintf(outputFilePtr, "%s ", ptr->parent->child[child_identity_0]->string);

                        for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->parent->child[child_identity_0]->string)) ; i++) {
                            fprintf(outputFilePtr, " ");
                        }

                     }   

                     /* column 4 */
                     fprintf(outputFilePtr, "%d", tmpValue);

                     fprintf(outputFilePtr, "\n");
                    } /* formatFlag */

 	        } /* if BITS */

                /* OCTET STRING type */
                else if(ptr->parent->child[child_syntax___1]->type == OCTET) {

                    if(formatFlag == 0){
                     /* column 1 */
                     fprintf(outputFilePtr, "%%defval ");

                     for(i = 0 ; i < (maxColumnWidth - 7) ; i++) {
                        fprintf(outputFilePtr, " ");
                     }   

                     if(ptr->parent->child[child_identity_0] && ptr->parent->child[child_identity_0]->string){
            
                        /* column 2 */
                        fprintf(outputFilePtr, "%s", ptr->parent->child[child_identity_0]->string);
                        for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->parent->child[child_identity_0]->string)) ; i++) {
                            fprintf(outputFilePtr, " ");
                        }   

                     }

                     /* column 3 */
                     fprintf(outputFilePtr, " %s", ptr->child[child_identity_0]->string);
                     fprintf(outputFilePtr, "\n");

                     jmpDis = strlen(ptr->child[child_identity_0]->string);
                    } /* formatFlag */

                } /* if OCTET STRING type */

		else {

		    if(formatFlag==0){
                     /* column 1 */
                     fprintf(outputFilePtr, "%%defval ");

                     for(i = 0 ; i < (maxColumnWidth - 7) ; i++) {
                        fprintf(outputFilePtr, " ");
                     }   

                     if(ptr->parent->child[child_identity_0] && ptr->parent->child[child_identity_0]->string){

                        /* column 2 */
                        if(ptr->parent->child[child_identity_0]->type == NEG_NUMBER) {
                            fprintf(outputFilePtr, "-%s ", ptr->parent->child[child_identity_0]->string);
                            for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->parent->child[child_identity_0]->string)-1) ; i++) {
                                fprintf(outputFilePtr, " ");
                            }   
                        } else {
                            fprintf(outputFilePtr, "%s ", ptr->parent->child[child_identity_0]->string);
                            for(i = 0 ; i < (maxColumnWidth - (int)strlen(ptr->parent->child[child_identity_0]->string)) ; i++) {
                                fprintf(outputFilePtr, " ");
                            }   
                        }

                     }

                     /* column 3 */
                     jmpDis = print_value_list(ptr->child[child_identity_0], outputFilePtr);

                     fprintf(outputFilePtr, "\n");
                    } /* formatFlag */
                } /* end else */

                check_for_type_definition(ptr->child[child_identity_0] , outputFilePtr, descrFilePtr);
            } 

            break;

        case TEXTUAL_CONVENTION:

           if ( (llnode = (ObjData *)malloc(sizeof(ObjData))) == NULL) {
              fprintf(stderr, "Unable to malloc memory for llnode\n");
              return;
           }
           memset((ObjData *)llnode, '\0', sizeof(ObjData));
           llnode->augments=0;

           if ( (tcnode = (TC_ObjData *)malloc(sizeof(TC_ObjData))) == NULL) {
              fprintf(stderr, "Unable to malloc memory for tcnode\n");
              return;
           }
           memset((TC_ObjData *)tcnode, '\0', sizeof(TC_ObjData));

           /* copy name */
           if(ptr->child[child_identity_0] && 
              ptr->child[child_syntax___1] && 
              ptr->child[child_syntax___1]->string && 
              ptr->child[child_identity_0]->string) {
              llnode->name = strdup(ptr->child[child_identity_0]->string);
              tcnode->name = strdup(ptr->child[child_identity_0]->string);
            }

           /* copy type */
           if(ptr->child[child_syntax___1] && 
              ptr->child[child_syntax___1]->string) {
              llnode->type = strdup(ptr->child[child_syntax___1]->string);
              tcnode->type = strdup(ptr->child[child_syntax___1]->string);
           }

          /* check for enumeration */
          if( ptr->child[child_identity_0] &&
              ptr->child[child_identity_0]->string &&
              ptr->child[child_syntax___1] &&
              ptr->child[child_syntax___1]->child[child_status___2] ) {

              llnode->enum_list = build_enumeration_info(ptr);
              tcnode->enum_list = build_enumeration_info(ptr);
          }

          /* copy range */
          if(llnode->range[0] == NULL){
              if( ptr->child[child_syntax___1] &&
                  ptr->child[child_syntax___1]->child[child_range____6] ) {
                    llnode->range[0] = add_range_info(ptr->child[child_syntax___1]->child[child_range____6]);
		    /*for Textual Convention structure*/
                    tcnode->range[0] = add_range_info(ptr->child[child_syntax___1]->child[child_range____6]);
		
		    /* see if there is another range in the list 
		     * Note - currently only doing this step for Textual 
 		     * Convention struct used by xml output since info.dat
		     * format does not support multiple ranges
	    	     */
		    if(ptr->child[child_syntax___1]->child[child_range____6]->
			next_tree_node && ptr->child[child_syntax___1]->
			  child[child_range____6]->next_tree_node->
				child[chld_end_range13]) {
		        i = 1;
			tmpPtr = ptr->child[child_syntax___1]->
                                child[child_range____6]->next_tree_node;

			/** loop until no more in the list **/
	 		while(i) {
			  tcnode->range[i] = add_range_info(tmpPtr);
			  if (tmpPtr->next_tree_node && tmpPtr->
				next_tree_node->child[chld_end_range13]) {
			    tmpPtr = tmpPtr->next_tree_node;
			    i++;
			  }
			  else {
			    i = 0;
			  }
			} /* end while */
	            }	     
              }
          }

	  /*BAB - for xml output*/
	  /* copy status */
	  if(ptr->child[child_status___2] &&
	     ptr->child[child_status___2]->string) {
	     tcnode->status = strdup(ptr->child[child_status___2]->string);
	  }

	  /* copy description */
	  if(ptr->child[child_descript_3] &&
	     ptr->child[child_descript_3]->string) {
	     tcnode->descr = strdup(ptr->child[child_descript_3]->string);
	  }	

	  /* copy reference */
	  if(ptr->child[child_referenc_4] &&
	     ptr->child[child_referenc_4]->string) {
	     tcnode->reference = strdup(ptr->child[child_referenc_4]->string);
	  }

	  /* copy display hint */
	  if(ptr->child[child_display__8] &&
	     ptr->child[child_display__8]->string) {
	     tcnode->misc = strdup(ptr->child[child_display__8]->string);
	  }

	  /*determine if this is a TypeAssignment*/
 	  if(ptr->string) {
	    if(!strcmp(ptr->string, "TEXTUAL_CONVENTION")) {
	        /* set tc_ta to 0, otherwise set to 1 */
		tcnode->tc_ta = 0;
	    }
	    else {
		tcnode->tc_ta = 1;
	    }
	  }
	  /*BAB*/

          llnode->component_type = strdup("TextualConvention");

          ll_add_node(llnode);
          if(formatFlag == XML){
             ll_add_tcnode(tcnode);
          }
          break;

        default:
           break;
     }

     return;

}

void
ll_emit_modcomp(ModuleComp * modPtr, tree * ptr)
{
  
  CondGroup *newGroupInfo = NULL;
  CondGroup *tmpGroupInfo = NULL;
  CondException *newExceptionInfo = NULL;
  CondException *tmpExceptionInfo = NULL;
  tree * tmpPtr = NULL;
  int i=0;

  short expt_loop_flag=0; 	
  short group_loop_flag=0; 	

  /* algorithm is to loop through all possible conditional groups or
   * exceptions, fill in and link the appropriate list
   */

  if(modPtr->cond_exception == NULL) {
	modPtr->cond_exception = 
		(CondException *)malloc(sizeof(CondException));
     	memset((CondException *)modPtr->cond_exception, '\0', 
		sizeof(CondException));
  }
  modPtr->cond_exception->next = NULL;
  if(modPtr->cond_group == NULL) {
	modPtr->cond_group = 
		(CondGroup *)malloc(sizeof(CondGroup));
     	memset((CondGroup *)modPtr->cond_group, '\0', sizeof(CondGroup));
  }
  modPtr->cond_group->next = NULL;

  while(ptr) {
    if((ptr->child[child_access__11] || ptr->child[child_misc1___14] || 
   	ptr->child[child_misc2___15]) && ptr->child[child_identity_0]) {

	/****conditional exception****/

	if(!expt_loop_flag){
	  /* first time through the loop for conditional exceptions*/
	  expt_loop_flag = 1;

	  /* name */
  	  if(ptr->child[child_identity_0] && 
		ptr->child[child_identity_0]->string) {
            modPtr->cond_exception->name = 
	    	strdup(ptr->child[child_identity_0]->string);
  	  }

  	  /* description */
  	  if(ptr->child[child_descript_3] && 
	  	ptr->child[child_descript_3]->string) {
            modPtr->cond_exception->descr = 
		strdup(ptr->child[child_descript_3]->string);
  	  }

  	  /* min-access */
  	  if(ptr->child[child_access__11] && 
		ptr->child[child_access__11]->string) {
            modPtr->cond_exception->min_access = 
		strdup(ptr->child[child_access__11]->string);
  	  }

  	  /* write-syntax */
  	  if(ptr->child[child_misc1___14] && 
		ptr->child[child_misc1___14]->string) {
            modPtr->cond_exception->write_syntax = 
		strdup(ptr->child[child_misc1___14]->string);
            /* copy range */
            if(modPtr->cond_exception->range_write[0] == NULL){
              if(ptr->child[child_misc1___14]->child[child_range____6]) {
                  modPtr->cond_exception->range_write[0] = 
                      add_range_info(ptr->child[child_misc1___14]->
			  child[child_range____6]);

	    	  /*BAB - see if there is another range
                  *in the list
                  */
                  if(ptr->child[child_misc1___14]->child[child_range____6]->
			next_tree_node && ptr->child[child_misc1___14]->
			child[child_range____6]->next_tree_node->
			child[chld_end_range13]) {
                    i = 1;
                    tmpPtr = ptr->child[child_misc1___14]->
                                 child[child_range____6]->next_tree_node;

                    /** loop until no more in the list **/
                    while(i) {
                       modPtr->cond_exception->range_write[i] = 
						add_range_info(tmpPtr);
                       if (tmpPtr->next_tree_node && tmpPtr->
                            next_tree_node->child[chld_end_range13]) {
                          tmpPtr = tmpPtr->next_tree_node;
                          i++;
                       }
                       else {
                         i = 0;
                       }
                    } /* end while */
                 }
	      }
            } /* end if range */
	    /* copy enumeration */
            if(ptr->child[child_misc1___14]->child[child_status___2]) {
                modPtr->cond_exception->enum_list_write = 
				build_enumeration_modcomp_info(ptr);
            }
  	  }

  	  /* syntax */
  	  if(ptr->child[child_misc2___15] && 
		ptr->child[child_misc2___15]->string) {
            modPtr->cond_exception->syntax = 
		strdup(ptr->child[child_misc2___15]->string);

            /* copy range */
            if(modPtr->cond_exception->range[0] == NULL){
              if(ptr->child[child_misc2___15]->child[child_range____6]) {
                  modPtr->cond_exception->range[0] = 
                      add_range_info(ptr->child[child_misc2___15]->
			  child[child_range____6]);

	    	  /*BAB - see if there is another range
                  *in the list
                  */
                  if(ptr->child[child_misc2___15]->child[child_range____6]->
			next_tree_node && ptr->child[child_misc2___15]->
			child[child_range____6]->next_tree_node->
			child[chld_end_range13]) {
                    i = 1;
                    tmpPtr = ptr->child[child_misc2___15]->
                                 child[child_range____6]->next_tree_node;

                    /** loop until no more in the list **/
                    while(i) {
                       modPtr->cond_exception->range[i] = 
						add_range_info(tmpPtr);
                       if (tmpPtr->next_tree_node && tmpPtr->
                            next_tree_node->child[chld_end_range13]) {
                          tmpPtr = tmpPtr->next_tree_node;
                          i++;
                       }
                       else {
                         i = 0;
                       }
                    } /* end while */
                 }
	      }
            } /* end if range */
	    /* copy enumeration */
	               /* check for enumeration */
            if(ptr->child[child_misc2___15]->child[child_status___2]) {
                modPtr->cond_exception->enum_list = 
			build_enumeration_modcomp_info(ptr);
            }

          } /* end if syntax */

  	  ptr = ptr->next_tree_node;
  	  newExceptionInfo = modPtr->cond_exception->next;
	}
	else {
	   if(newExceptionInfo == NULL) {
		newExceptionInfo = (CondException *)
				malloc(sizeof(CondException));
     	        /*memset((CondException *)modPtr->cond_exception, '\0', 
			sizeof(CondException));*/
     	        memset((CondException *)newExceptionInfo, '\0', 
			sizeof(CondException));
	   }

	   /* name */
           if(ptr->child[child_identity_0] &&
                ptr->child[child_identity_0]->string) {
                newExceptionInfo->name =
                        strdup(ptr->child[child_identity_0]->string);
           }

	   /* description */
           if(ptr->child[child_descript_3] &&
                ptr->child[child_descript_3]->string) {
                newExceptionInfo->descr =
                        strdup(ptr->child[child_descript_3]->string);
           }

           /* min-access */
           if(ptr->child[child_access__11] &&
                ptr->child[child_access__11]->string) {
                newExceptionInfo->min_access =
                        strdup(ptr->child[child_access__11]->string);
           }

           /* write-syntax */
           if(ptr->child[child_misc1___14] &&
                ptr->child[child_misc1___14]->string) {
                newExceptionInfo->write_syntax =
                        strdup(ptr->child[child_misc1___14]->string);
            /* copy range */
            if(newExceptionInfo->range_write[0] == NULL){
              if(ptr->child[child_misc1___14]->child[child_range____6]) {
                  newExceptionInfo->range_write[0] = 
                      add_range_info(ptr->child[child_misc1___14]->
			  child[child_range____6]);

	    	  /*BAB - see if there is another range
                  *in the list
                  */
                  if(ptr->child[child_misc1___14]->child[child_range____6]->
			next_tree_node && ptr->child[child_misc1___14]->
			child[child_range____6]->next_tree_node->
			child[chld_end_range13]) {
                    i = 1;
                    tmpPtr = ptr->child[child_misc1___14]->
                                 child[child_range____6]->next_tree_node;

                    /** loop until no more in the list **/
                    while(i) {
                       newExceptionInfo->range_write[i] = 
						add_range_info(tmpPtr);
                       if (tmpPtr->next_tree_node && tmpPtr->
                            next_tree_node->child[chld_end_range13]) {
                          tmpPtr = tmpPtr->next_tree_node;
                          i++;
                       }
                       else {
                         i = 0;
                       }
                    } /* end while */
                 }
	      }
            } /* end if range */
	    /* copy enumeration */
            if(ptr->child[child_misc1___14]->child[child_status___2]) {
                newExceptionInfo->enum_list_write = 
			build_enumeration_modcomp_info(ptr);
            }
           }

           /* syntax */
           if(ptr->child[child_misc2___15] &&
                ptr->child[child_misc2___15]->string) {
                newExceptionInfo->syntax =
                        strdup(ptr->child[child_misc2___15]->string);
            /* copy range */
            if(newExceptionInfo->range[0] == NULL){
              if(ptr->child[child_misc2___15]->child[child_range____6]) {
                  newExceptionInfo->range[0] = 
                      add_range_info(ptr->child[child_misc2___15]->
			  child[child_range____6]);

	    	  /*BAB - see if there is another range
                  *in the list
                  */
                  if(ptr->child[child_misc2___15]->child[child_range____6]->
			next_tree_node && ptr->child[child_misc2___15]->
			child[child_range____6]->next_tree_node->
			child[chld_end_range13]) {
                    i = 1;
                    tmpPtr = ptr->child[child_misc2___15]->
                                 child[child_range____6]->next_tree_node;

                    /** loop until no more in the list **/
                    while(i) {
                       newExceptionInfo->range[i] = 
						add_range_info(tmpPtr);
                       if (tmpPtr->next_tree_node && tmpPtr->
                            next_tree_node->child[chld_end_range13]) {
                          tmpPtr = tmpPtr->next_tree_node;
                          i++;
                       }
                       else {
                         i = 0;
                       }
                    } /* end while */
                 }
	      }
            } /* end if range */
	    /* copy enumeration */
	               /* check for enumeration */
            if(ptr->child[child_misc2___15]->child[child_status___2]) {
                newExceptionInfo->enum_list = 
			build_enumeration_modcomp_info(ptr);
            }

           } /* end if syntax */

 	   /* link the list */

           if(modPtr->cond_exception->next == NULL) {
                modPtr->cond_exception->next = newExceptionInfo;
                newExceptionInfo->next = NULL;
                tmpExceptionInfo = modPtr->cond_exception->next;
           }
           else {
                tmpExceptionInfo->next = newExceptionInfo;
                tmpExceptionInfo = newExceptionInfo;
                newExceptionInfo->next = NULL;
           }

 	   newExceptionInfo = NULL;

	   ptr = ptr->next_tree_node;
	}

    } /* end conditional exception */

    else if(ptr->child[child_identity_0]) {
    	/*conditional group*/

	if(!group_loop_flag){
	  /* first time through the loop for conditional groups*/
	  group_loop_flag = 1;

	  /* name */
  	  if(ptr->child[child_identity_0] && 
		ptr->child[child_identity_0]->string) {
            modPtr->cond_group->name = 
	    	strdup(ptr->child[child_identity_0]->string);
  	  }

  	  /* description */
  	  if(ptr->child[child_descript_3] && 
	  	ptr->child[child_descript_3]->string) {
            modPtr->cond_group->descr = 
		strdup(ptr->child[child_descript_3]->string);
  	  }

  	  ptr = ptr->next_tree_node;
  	  newGroupInfo = modPtr->cond_group->next;
	}
	else {
	   if(newGroupInfo == NULL) {
		newGroupInfo = (CondGroup *)
				malloc(sizeof(CondGroup));
     	        /*memset((CondGroup *)modPtr->cond_group, '\0', 
				sizeof(CondGroup));*/
     	        memset((CondGroup *)newGroupInfo, '\0', 
				sizeof(CondGroup));
	   }

	   /* name */
           if(ptr->child[child_identity_0] &&
                ptr->child[child_identity_0]->string) {
                newGroupInfo->name =
                        strdup(ptr->child[child_identity_0]->string);
           }

	   /* description */
           if(ptr->child[child_descript_3] &&
                ptr->child[child_descript_3]->string) {
                newGroupInfo->descr =
                        strdup(ptr->child[child_descript_3]->string);
           }

 	   /* link the list */
           if(modPtr->cond_group->next == NULL) {
                modPtr->cond_group->next = newGroupInfo;
                newGroupInfo->next = NULL;
                tmpGroupInfo = modPtr->cond_group->next;
           }
           else {
                tmpGroupInfo->next = newGroupInfo;
                tmpGroupInfo = newGroupInfo;
                newGroupInfo->next = NULL;
           }

 	   newGroupInfo = NULL;

	   ptr = ptr->next_tree_node;
	}

    } /* end conditional group */

    else {
   	/*no more conditionals*/
   	return;
    }

  } /* end while */

  return;
}

void
ll_add_node(ObjData *llnode)
{
    ObjData *prevptr;
    ObjData *tmpptr;

    /* Set root if this is first node */
    if(llroot == NULL) {
       llroot = llnode;
       llroot->next_obj = NULL;
       llroot->prev_obj = NULL;
       return;
    }

    tmpptr = llroot;

    if(tmpptr && (tmpptr->next_obj == NULL)){
         tmpptr->next_obj = llnode;
         llnode->next_obj = NULL;
         llnode->prev_obj = tmpptr;
         return;
    }

    while(tmpptr->next_obj != NULL){ 
         prevptr = tmpptr;     
         tmpptr = tmpptr->next_obj;
    }

    /* add node */

    tmpptr->next_obj = llnode;
    llnode->prev_obj = tmpptr; 

    return;

}

void
ll_add_tcnode(TC_ObjData *tcnode)
{
    TC_ObjData *prevptr;
    TC_ObjData *tmpptr;

    /* Set root if this is first node */
    if(tc_root == NULL) {
       tc_root = tcnode;
       tc_root->next_obj = NULL;
       tc_root->prev_obj = NULL;
       return;
    }

    tmpptr = tc_root;

    if(tmpptr && (tmpptr->next_obj == NULL)){
         tmpptr->next_obj = tcnode;
         tcnode->next_obj = NULL;
         tcnode->prev_obj = tmpptr;
         return;
    }

    while(tmpptr->next_obj != NULL){
         prevptr = tmpptr;
         tmpptr = tmpptr->next_obj;
    }

    /* add node */

    tmpptr->next_obj = tcnode;
    tcnode->prev_obj = tmpptr;

    return;

}

void
ll_add_imnode(IM_ObjData *imnode)
{
    IM_ObjData *prevptr;
    IM_ObjData *tmpptr;

    /* Set root if this is first node */
    if(im_root == NULL) {
       im_root = imnode;
       im_root->next_obj = NULL;
       im_root->prev_obj = NULL;
       return;
    }

    tmpptr = im_root;

    if(tmpptr && (tmpptr->next_obj == NULL)){
         tmpptr->next_obj = imnode;
         imnode->next_obj = NULL;
         imnode->prev_obj = tmpptr;
         return;
    }

    while(tmpptr->next_obj != NULL){
         prevptr = tmpptr;
         tmpptr = tmpptr->next_obj;
    }

    /* add node */

    tmpptr->next_obj = imnode;
    imnode->prev_obj = tmpptr;

    return;

}



/** BAB routines used to sort the linked list **/

int
sort_data()
{

  ObjData *tmpptr;

  for (tmpptr = llroot; tmpptr != NULL; tmpptr = tmpptr->next_obj) {
	sorted_root = insert_sort_data(sorted_root, tmpptr);
  }
  
  return 1;

}

ObjData *
insert_sort_data(exist_ptr, add_ptr)
   ObjData *exist_ptr, *add_ptr;
{
   int cc;
   
   if (exist_ptr == NULL) {
     return (add_ptr);
   }

   if(add_ptr->oid == NULL) {
     return (exist_ptr);
   }
   if(exist_ptr->oid == NULL) {
     return (add_ptr);
   }
   
   /* printf("calling cmp_oid on %s and %s\n", exist_ptr->oid, add_ptr->oid); */
   cc = cmp_oid(exist_ptr->oid, add_ptr->oid, oidlength(exist_ptr->oid),
		oidlength(add_ptr->oid));

   if (cc == 0) {              /* Duplicate! */
     /* if the name is the same, just lose the add_ptr */
     if (strcmp(exist_ptr->name, add_ptr->name) == 0) {
	return (exist_ptr);
     }
     else {
	add_ptr->next_sorted = exist_ptr->next_sorted;
        exist_ptr->next_sorted = add_ptr;

	return(exist_ptr);
     }
   
   }
   else if (cc < 0) {
     exist_ptr->next_sorted = insert_sort_data(exist_ptr->next_sorted, add_ptr);
     return (exist_ptr);
   }
   /* (cc > 0) */
   else {
     add_ptr->next_sorted = exist_ptr;
     return (add_ptr);
   }
}

int
cmp_oid(oid1, oid2, length1, length2)
    char *oid1, *oid2;
    int length1, length2;
{

   int i, j;
   int min;
   /*char *cmp_oid1[MAX_OID_SIZE];
   char *cmp_oid2[MAX_OID_SIZE];*/
   char cmp_oid1[MAX_OID_SIZE][10];
   char cmp_oid2[MAX_OID_SIZE][10];

   int dot_ctr = 0;
   int begin_sid = 0;
   int int_sid1, int_sid2;

   for(i = 0; i < MAX_OID_SIZE; i++) {
       /*cmp_oid1[i] = (char *)malloc(sizeof(char *));*/
       /*cmp_oid2[i] = (char *)malloc(sizeof(char *));*/
     for(j=0; j < 10; j++) {
       cmp_oid1[i][j] = '\0';
       cmp_oid1[i][j] = '\0';
     }
   }

   if(length1 < length2) 
	min = length1;
   else
	min = length2;

   /*put each subidentifier into array of separate strings for comparison*/
   for(i = 0; i < strlen(oid1); i++) {
	  
	/*If come accross a dot process*/
	if(oid1[i] == '.') {
	   cmp_oid1[dot_ctr][i - (begin_sid + 1)] = '\0';
	   dot_ctr++;
	   begin_sid = i;
	}
	else {
	   if(i == 0) {  /*if at the beginning of the string*/
	     cmp_oid1[dot_ctr][i] = oid1[i];
	   }
	   else { /* have to take the difference and add one for the dot*/
	     cmp_oid1[dot_ctr][i - (begin_sid + 1)] = oid1[i];
	     /* check to see if at the end of the string */
	     if(oid1[i + 1] == '\0') {
		cmp_oid1[dot_ctr][i - begin_sid] = '\0';
		dot_ctr++;
	     }
	   }
	}
    }
    /*cmp_oid1[dot_ctr] = NULL;*/
    cmp_oid1[dot_ctr][0] = '\0'; 

    dot_ctr = 0;
    begin_sid = 0;
    for(i = 0; i < strlen(oid2); i++) {

	/*If come accross a dot or end of string process*/
	if(oid2[i] == '.') {
	   cmp_oid2[dot_ctr][i - (begin_sid + 1)] = '\0';
	   dot_ctr++;
	   begin_sid = i;
	}
	else {
	   if(i == 0) {  /*if at the beginning of the string*/
             cmp_oid2[dot_ctr][i] = oid2[i];
           }
           else { /* have to take the difference and add one for the dot*/
             cmp_oid2[dot_ctr][i - (begin_sid + 1)] = oid2[i];
	      /* check to see if at the end of the string */
             if(oid2[i + 1] == '\0') {
                cmp_oid2[dot_ctr][i - begin_sid] = '\0';
                dot_ctr++;
             } 
           }
	}
    }
    /*cmp_oid2[dot_ctr] = NULL;*/
    cmp_oid2[dot_ctr][0] = '\0'; 

    /*compare each set of subidentifier strings*/
    for(i = 0; i < min; i++) {
       int_sid1 = atoi(cmp_oid1[i]);
       int_sid2 = atoi(cmp_oid2[i]);
           
       if(int_sid1 < int_sid2)
	  return (-1);
	
	/*if sid1 > sid2, we know entire oid1 > oid2*/
	else if(int_sid1 > int_sid2)
	   return (1);
	
        /* else continue, sids are equal*/
    }

    /*min sizes are the same so far, check for overall oid length*/
    if(length1 == length2) {

 	  /*compare the last sid*/
	  int_sid1 = atoi(cmp_oid1[length1 - 1]);
	  int_sid2 = atoi(cmp_oid2[length1 - 1]);
	 
 	  if(int_sid1 < int_sid2) 
              return (-1);
	  
	  else if(int_sid1 > int_sid2) 
             return (1);
          
	  else    /* cmp_size == 0 */
             return (0);
	  
    }
    else if(length1 < length2) 
	return (-1);
    
    else   /* length1 > length2 */
	 return (1);
    
}


/**BAB routine to print the linked list in idat format **/

void
print_ll(FILE *outputFilePtr)
{
   ObjData *tmpptr = NULL;
   int i=0;
   int objColumnWidth = 20;
   int oidColumnWidth = 25;
   int typeColumnWidth = 15;
   int accColumnWidth = 10;
   int isentry = 0;

   /*BAB - Debugging - See if sorted list is sorted correctly*/
   /*for(tmpptr = sorted_root; tmpptr != NULL; tmpptr = tmpptr->next_sorted) {
        if(tmpptr->oid != NULL) {
                printf("oid = %s\n", tmpptr->oid);
        }
        else {
                printf("oid is NULL\n");
        }
   }
   */


   /*BAB*/
   /*tmpptr = llroot;*/
   tmpptr = sorted_root;

   if(tmpptr == NULL){
       fprintf(stderr, "-- No MIB information to write to %s --\n\n", 
			outputFileName);
       return;
   }

   printf("-- Writing MIB information to %s --\n\n", outputFileName);
   fprintf(outputFilePtr, "\n\n");

   do {
         if( (!strcmp(tmpptr->component_type, "TextualConvention")) ||
             (!strcmp(tmpptr->component_type, "ModuleCompliance")) ||
             (!strcmp(tmpptr->component_type, "ObjectGroup")) ||
             (!strcmp(tmpptr->component_type, "NotificationGroup")) ){
              /* info.dat does not need to output a tc line */
                tmpptr=tmpptr->next_sorted;
                continue;
         }

         if( !strcmp(tmpptr->component_type, "TrapType") ){
              /* Disable output for TRAP-TYPE for now */
              tmpptr=tmpptr->next_sorted;
              continue;
         }

         if(tmpptr->name != NULL){
           fprintf(outputFilePtr, "%s", tmpptr->name);
           for (i = 0 ; i< (objColumnWidth - (int)strlen(tmpptr->name)); i++) {
                                fprintf(outputFilePtr, " ");
           }  
         }
         if(tmpptr->name == NULL){
            /* used to jump over nonLeaf only output */
              tmpptr = tmpptr->next_sorted;
              continue;
         }
         
         if(tmpptr->oid != NULL){
            fprintf(outputFilePtr, "  %s", tmpptr->oid);
            for(i = 0 ; i < (oidColumnWidth - (int)strlen(tmpptr->oid)); i++) {
                                fprintf(outputFilePtr, " ");
            }             
         }

         if(tmpptr->access != NULL){
             if(!strcmp(tmpptr->access, "not-accessible")) {
                     if( (tmpptr->index_list != NULL) || 
                         ((tmpptr->next_sorted != NULL) &&
                         (tmpptr->next_sorted->index_list !=NULL)) ||
                         (tmpptr->augments == 1) ||
                         (tmpptr->type == NULL )) {
                           fprintf(outputFilePtr, "  Aggregate");
                           isentry = 1;
                           for(i = 0 ; i < (typeColumnWidth - 9); i++) {
                                fprintf(outputFilePtr, " ");
                           }            
                     }
              }
         }

         if(tmpptr->type != NULL){
             if(!isentry){

                if(tmpptr->primtype != NULL){
                  fprintf(outputFilePtr, "  %s", tmpptr->primtype);
                } else {
                  fprintf(outputFilePtr, "  %s", tmpptr->type);
                }

                if(tmpptr->range[0]){
                    fprintf(outputFilePtr, "(%u..%u) ", tmpptr->range[0]->lower, tmpptr->range[0]->upper);
                }
             } else {
                isentry=0;
             }
             for(i = 0 ; i < (typeColumnWidth - (int)strlen(tmpptr->type)); i++) {
                                fprintf(outputFilePtr, " ");
            }
         } else {
               fprintf(outputFilePtr, "  nonLeaf\n");
         }

         /* Module Identity overloads access with contact information.
          * If component is ModuleIdentity don't print the access
          */
         if(strcmp(tmpptr->component_type, "ModuleIdentity")) {

            if(tmpptr->access != NULL){
               fprintf(outputFilePtr, "  %s", tmpptr->access);
               for(i = 0 ; i < (accColumnWidth - (int)strlen(tmpptr->access)); i++) {
                                fprintf(outputFilePtr, " ");
               }             
            }
        }

         if(tmpptr->enum_list !=NULL){
                fprintf(outputFilePtr, "\n");
                print_ll_enum(tmpptr->enum_list, outputFilePtr);
         }
         if(tmpptr->index_list !=NULL){
                fprintf(outputFilePtr, "\n");
                print_ll_index(tmpptr->index_list, outputFilePtr);
         }
         
         fprintf(outputFilePtr, "\n");

	 /*BAB*/
         /*tmpptr = tmpptr->next_obj;*/
         tmpptr = tmpptr->next_sorted;

   } while(tmpptr != NULL);

   fprintf(outputFilePtr, "\n\n");

   return;
}


/**** BAB Routines to print to properly formatted xml output ****/

static char *tag_stack[MAX_OID_SIZE];
static int stack_pos = 0;

int
oidlength(char *oid)
{
   int length=1;
   int i=0;
  
   while(oid[i] != '\0') {
	if(oid[i] == '.') {
	  length++;
	}
	i++;
   }
  
   return length;
}  

void
pushtag(char *name)
{   

    tag_stack[stack_pos++] = name;

    if (stack_pos > MAX_OID_SIZE) {
        fprintf(stderr, "mibgc internal error: tag_stack size exceeded\n");
        exit(1);
    }
}

char *poptag(void)
{   

    if (stack_pos < 1) {
        fprintf(stderr, "mibgc internal error: tag_stack underflow\n");
        exit(1);
    }
   
    return tag_stack[--stack_pos];
}

void
indent(FILE *fp, int indent)
{   
    int cols_to_indent = 2;
    int i;

    for (i = 0; i < indent * cols_to_indent; i++) {
        fprintf(fp, " ");
    }
}


void
print_ll_xml(FILE *outputFilePtr)
{

   ObjData *tmpptr = NULL;
   ObjData *skipptr = NULL;
   TC_ObjData *tmptcptr = NULL;
   IM_ObjData *tmpimptr = NULL;
   RevisionInfo *tmpRevInfo = NULL;
   int isentry;
   int diff, tmpdiff, i, j, next_len;
   int baseOID=0;
   int thisOID=0;
   int thisFixedOID=0;
   int nextOID=0;
   int prevOID=0;
   int prevFixedOID=0;
   int paragraph_flag=0;
   int breakflag=0;
   int tcvalid_flag=0;
   FILE *tmpFilePtr = NULL;

   char *end_string=NULL;

   if (!explicitOutputFile) {
        tmpFilePtr = outputFilePtr;
        outputFilePtr = stdout;
   }

   /*end_string = (char *)malloc(sizeof(char *));*/

   /*BAB - Debugging - See if sorted list is sorted correctly*/
   /*for(tmpptr = sorted_root; tmpptr != NULL; tmpptr = tmpptr->next_sorted) {
 	if(tmpptr->oid != NULL) {
		printf("oid = %s\n", tmpptr->oid);
	}
	else {
		printf("oid is NULL\n");
	}
   }*/

   tmpptr = sorted_root;
   tmptcptr = tc_root;
   tmpimptr = im_root;

   /*print out Module Name*/
   if(first_parse) {
     fprintf(outputFilePtr, "<\?xml version=\"1.0\" \?>\n");
   }
   if(multiple_mibs && first_parse) {
     fprintf(outputFilePtr, "<MIBModules>\n");
   }
   fprintf(outputFilePtr, "<MIBModule name=\"%s\">\n", moduleNameBuffer);

   pushtag("MIBModule");


   /*BAB - Add IMPORTS*/
   i=0;
   if(tmpimptr != NULL) {
     indent(outputFilePtr, 1);
     fprintf(outputFilePtr, "<IMPORTS imports=\"");

     if(tmpimptr->name && tmpimptr->type){
        /* FROM */
        fprintf(outputFilePtr, "%s.%s", tmpimptr->type, tmpimptr->name);
  	i=1;
     }

     tmpimptr = tmpimptr->next_obj;

     while(tmpimptr != NULL) {

  	if(tmpimptr->name && tmpimptr->type){
	  /* FROM */
	  if(i) {
            fprintf(outputFilePtr, " %s.%s", tmpimptr->type, tmpimptr->name);
	  }
	  else {
	    fprintf(outputFilePtr, "%s.%s", tmpimptr->type, tmpimptr->name);
	    i = 1;
	  }
        }

	tmpimptr = tmpimptr->next_obj;
     }

     fprintf(outputFilePtr, "\"/>\n");
   }
	
   
   /*BAB - Add Textual Convention*/
   if(tmptcptr != NULL) {
    /*check to make sure these are valid TCs before printing out*/
    do {
      if(tmptcptr->type != NULL) {
	tcvalid_flag = 1; 
      }
      tmptcptr = tmptcptr->next_obj;
 
    } while(tmptcptr != NULL);

    /*now only print out if at least one valid TC*/
    if(tcvalid_flag) {
     tmptcptr = tc_root;
     indent(outputFilePtr, 1);
     
     fprintf(outputFilePtr, "<TextualConventions>\n");

     do {

	/* name */
	if(tmptcptr->name && tcvalid_flag){
	  indent(outputFilePtr, 2);
          /* if a TC and not a TypeAssignment */
          if(!(tmptcptr->tc_ta)) {
             fprintf(outputFilePtr, "<TextualConvention name=\"%s\" ",
                        tmptcptr->name);
          }
          else {
             fprintf(outputFilePtr, "<TypeAssignment name=\"%s\" ",
                        tmptcptr->name);
          }

	  /* display-hint */
	  if(tmptcptr->misc) {
		tmptcptr->misc = xml_correct_string(tmptcptr->misc, 
			strlen(tmptcptr->misc));
		fprintf(outputFilePtr, "display-hint=\"%s\" ", 
			tmptcptr->misc);
	  }

	  /* status */
	  if(tmptcptr->status) {
		fprintf(outputFilePtr, "status=\"%s\" ", tmptcptr->status);
	  }

	  /* syntax */
	  if(tmptcptr->type) {
	    if(strcmp(tmptcptr->type, "OctetString") == 0) {
	  	fprintf(outputFilePtr, "type=\"OCTET STRING");
	    }
	    else if(strcmp(tmptcptr->type, "ObjectID") == 0) {
	  	fprintf(outputFilePtr, "type=\"OBJECT IDENTIFIER");
	    }
	    else {
	    	fprintf(outputFilePtr, "type=\"%s", tmptcptr->type);
	    }
	    /* if range exists handle properly */
	    i = 0;

	    /*range*/
	    if(tmptcptr->range[i]) {
	       if(strcmp(tmptcptr->type, "OctetString") == 0) {
                  if(tmptcptr->range[i]->lower == 
                    tmptcptr->range[i]->upper) {
                    /*just use one*/
                    fprintf(outputFilePtr, "(SIZE (%d",
                    tmptcptr->range[i]->lower);
                  }
                  else {
                    fprintf(outputFilePtr, "(SIZE (%d..%d",
                        tmptcptr->range[i]->lower, tmptcptr->range[i]->upper);
                  }
               }
	       /*only positive numbers*/
	       else if((strcmp(tmptcptr->type, "Unsigned32") == 0) ||
		       (strcmp(tmptcptr->type, "UInteger32") == 0) ||
		       (strcmp(tmptcptr->type, "Counter") == 0) ||
		       (strcmp(tmptcptr->type, "Counter32") == 0) ||
		       (strcmp(tmptcptr->type, "Gauge") == 0) || 
		       (strcmp(tmptcptr->type, "Gauge32") == 0)) {
		  fprintf(outputFilePtr, "(%u..%u",
			   tmptcptr->range[i]->lower, tmptcptr->range[i]->upper)
;
               }
	       /*only positive large numbers*/
	       else if(strcmp(tmptcptr->type, "Counter64") == 0) {
		  fprintf(outputFilePtr, "(%lu..%lu",
			   (unsigned long)tmptcptr->range[i]->lower, 
			   (unsigned long)tmptcptr->range[i]->upper);
	       } 
	       /*can have positive or negatives*/
               else {
                  fprintf(outputFilePtr, "(%d..%d",
                           tmptcptr->range[i]->lower, tmptcptr->range[i]->upper);
               }

 	      i++;

	      while(i) {
	        if(tmptcptr->range[i]) {
	          if(strcmp(tmptcptr->type, "OctetString") == 0) {
			if(tmptcptr->range[i]->lower == 
				tmptcptr->range[i]->upper) {
			  /*just use one*/
			  fprintf(outputFilePtr, " | %d", 
				tmptcptr->range[i]->lower);
			}
		        else {
			   fprintf(outputFilePtr, " | %d..%d",
                             tmptcptr->range[i]->lower, 
				tmptcptr->range[i]->upper);
		        }
		   }
                   /*only positive numbers*/
                   else if((strcmp(tmptcptr->type, "Unsigned32") == 0) ||
                       (strcmp(tmptcptr->type, "UInteger32") == 0) ||
                       (strcmp(tmptcptr->type, "Counter") == 0) ||
                       (strcmp(tmptcptr->type, "Counter32") == 0) ||
                       (strcmp(tmptcptr->type, "Gauge") == 0) || 
                       (strcmp(tmptcptr->type, "Gauge32") == 0)) {
                          fprintf(outputFilePtr, "(%u..%u",
                           tmptcptr->range[i]->lower, tmptcptr->range[i]->upper);
                   }
                   /*only positive large numbers*/
                   else if(strcmp(tmptcptr->type, "Counter64") == 0) {
                     fprintf(outputFilePtr, "(%lu..%lu",
                           (unsigned long)tmptcptr->range[i]->lower, 
                           (unsigned long)tmptcptr->range[i]->upper);
                   }
                   /*can have positive or negatives*/
		   else {
                   	fprintf(outputFilePtr, " | %d..%d", 
			   tmptcptr->range[i]->lower, tmptcptr->range[i]->upper);
	   	   }
		   i++;
		 }
		 else {
		   i=0;
		 }
	      }
	      if(strcmp(tmptcptr->type, "OctetString") == 0) {
		fprintf(outputFilePtr, "))");
	      }
	      else {
		fprintf(outputFilePtr, ")");
	      }
	    } /*end if range*/
	    fprintf(outputFilePtr, "\">\n");

 	  } /*end syntax*/
	   
	  /* description */
	  if(tmptcptr->descr) {
	    	indent(outputFilePtr, 3);
	  	fprintf(outputFilePtr, "<description>\"%s\"\n", 
			tmptcptr->descr);
	  	indent(outputFilePtr, 3);
	  	fprintf(outputFilePtr, "</description>\n");
	  }

	  /* reference */
	  if(tmptcptr->reference) {
		indent(outputFilePtr, 3);
                fprintf(outputFilePtr, "<reference>\"%s\"\n", 
			tmptcptr->reference);
		indent(outputFilePtr, 3);
                fprintf(outputFilePtr, "</reference>\n");
	  }

	  indent(outputFilePtr, 2);
	  if(!(tmptcptr->tc_ta)) {
	    fprintf(outputFilePtr, "</TextualConvention>\n");
          }
	  else {
	    fprintf(outputFilePtr, "</TypeAssignment>\n");
     	  }
	
	}
	tmptcptr = tmptcptr->next_obj;

     } while(tmptcptr != NULL);

     indent(outputFilePtr, 1);
     fprintf(outputFilePtr, "</TextualConventions>\n\n");
    } /*end if tmptcptr*/
   }


  /* Add the remaining MIB */
  /*only process if we have at least one OID in this list*/
  if(tmpptr && tmpptr->oid){

    /**** walk through the list and find the Module-Identity 
     *    print this and anything under its branch out first
     */
    do {

     if(tmpptr->component_type && 
	strcmp(tmpptr->component_type, "ModuleIdentity") == 0) {
	/*found the Module Identity - process*/

        /* subtract 1 from the baseOID to allow for the MIBModule on the stack*/
        baseOID = (oidlength(tmpptr->oid) - 1);

	/*push onto stack*/
        pushtag(tmpptr->component_type);

        /*get correct indentions*/
        if(tmpptr->oid){
           thisOID = oidlength(tmpptr->oid);
 
 	   /* for proper comparison later */
	   thisFixedOID = thisOID;
	   prevOID = thisOID;
	   prevFixedOID = thisOID;

           indent(outputFilePtr, thisOID - baseOID);
        }

        fprintf(outputFilePtr, "<%s ", tmpptr->component_type);

	/* add name */
        if(tmpptr->name) {
	   fprintf(outputFilePtr, "name=\"%s\" ", tmpptr->name);
  	}

	/* add lastUpdate */
        if(tmpptr->misc) {
           fprintf(outputFilePtr, "lastUpdate=\"%s\" ", tmpptr->misc);
        }
        /* add organization */
        if(tmpptr->status) {
           fprintf(outputFilePtr, "organization=\"%s\" ", tmpptr->status);
        }

	/* add oid */
       	if(tmpptr->id){
           fprintf(outputFilePtr, "oid=\"%s\" fqoid=\"%s\" ", tmpptr->id, 
			tmpptr->oid);
       	}

        fprintf(outputFilePtr, ">\n");

        /* add contactInfo */
        if(tmpptr->access){
           fprintf(outputFilePtr, "\n");
           indent(outputFilePtr, thisOID - baseOID + 1);
           fprintf(outputFilePtr, "<contactInfo>\"%s\"\n", tmpptr->access);
           indent(outputFilePtr, thisOID - baseOID + 1);
           fprintf(outputFilePtr, "</contactInfo>");
        }

        /* add description */
        if(tmpptr->descr){
             fprintf(outputFilePtr, "\n");
             indent(outputFilePtr, thisOID - baseOID + 1);
	     tmpptr->descr = xml_correct_string(tmpptr->descr, 
		strlen(tmpptr->descr));
             fprintf(outputFilePtr, "<description>\"%s\"\n",
                        tmpptr->descr);
             indent(outputFilePtr, thisOID - baseOID + 1);
             fprintf(outputFilePtr, "</description>");
        }

        /* add reference */
        if(tmpptr->reference){
             fprintf(outputFilePtr, "\n");
             indent(outputFilePtr, thisOID - baseOID + 1);

             fprintf(outputFilePtr, "<reference>\"%s\"\n",tmpptr->reference);
             indent(outputFilePtr, thisOID - baseOID + 1);
             fprintf(outputFilePtr, "</reference>");
        }
          
        fprintf(outputFilePtr, "\n");

        /*Loop through Revision List*/
        if(tmpptr->revision_list != NULL) {
             tmpRevInfo = tmpptr->revision_list;

             fprintf(outputFilePtr, "\n");

             while(tmpRevInfo) {
               indent(outputFilePtr, thisOID - baseOID + 1);
               fprintf(outputFilePtr, "<revision date=\"%s\">\"%s\"\n",
                      tmpRevInfo->date, tmpRevInfo->description);
	       indent(outputFilePtr, thisOID - baseOID + 1);
	       fprintf(outputFilePtr, "</revision>\n");

               tmpRevInfo = tmpRevInfo->next;
             }
        }

	/* mark this one as looked at*/
        tmpptr->doneTag = 1;

	/* look to next OID and compare */
        if ((tmpptr->next_sorted) &&
            (tmpptr->next_sorted->oid)) {
              nextOID = oidlength(tmpptr->next_sorted->oid);

	      /*check to make sure next oid is not more than just one sid
	       * in difference (i.e. thisOID = 1.2 nextOID = 1.2.1.2
 	       */
	      if(nextOID - thisOID > 1) {
		 nextOID = thisOID + 1;
	      }
	      prevOID = thisOID;
 	      prevFixedOID = thisFixedOID;
              next_len = nextOID;
	      diff = (next_len - thisOID);
        } else {
              next_len = baseOID;
	      /* Since at the end, use thisFixedOID to calculate diff */
	      diff = (next_len - thisFixedOID);
        }

        /* if no more under the Module Identity, close brace */
        if (diff <= 0) {
            indent(outputFilePtr, thisOID - baseOID);
            end_string = poptag();

            fprintf(outputFilePtr, "</%s>\n", end_string);
            end_string = NULL;

	    breakflag = 1;
	    tmpptr = tmpptr->next_sorted;
        }
        else {
	    tmpptr = tmpptr->next_sorted;
  	}

        /* now process the remaining components under that branch */
	if(!breakflag) {
	 do {

	  isentry = 0;

       	  /* add component and name */
          if(tmpptr->name){

            /* add opening bracket and component type*/
            if(tmpptr->component_type){

              /*get correct indentions*/
              if(tmpptr->oid){
                thisOID = oidlength(tmpptr->oid);

                /*check to make sure this oid is not more than just one sid
                 * in difference from the previous
		 * (i.e. prevOID = 1.2 thisOID = 1.2.1.2
                 */
                if(thisOID - prevOID > 0) {
		   /* check for skipping over sids and push empty data
		    * onto the stack
		    */
		   for(i = thisOID - prevOID; i > 1; i--) {
			pushtag("");
		   }
	
                   /* thisFixedOID will just be one greater */
		   thisFixedOID = prevFixedOID + 1;
                }
		else if(thisOID == prevOID) {
		   thisFixedOID = prevFixedOID;
		}
		else {
		   thisFixedOID = thisOID;
		}

                indent(outputFilePtr, thisFixedOID - baseOID);
              }
              /*push component name onto stack*/
              pushtag(tmpptr->component_type);

              fprintf(outputFilePtr, "<%s ", tmpptr->component_type);
            }
            else {
 	      tmpptr->doneTag = 1;
              tmpptr = tmpptr->next_sorted;
              continue;
            }

            fprintf(outputFilePtr, "name=\"%s\" ", tmpptr->name);
          }
          else {
            /* used to jump over nonLeaf only output */
	    tmpptr->doneTag = 1;
            tmpptr = tmpptr->next_sorted;
            continue;
          }

	  /* add status */
	  if(tmpptr->status) {
            fprintf(outputFilePtr, "status=\"%s\" ", tmpptr->status);
          }

	  /* add oid */
          if(tmpptr->id){
             fprintf(outputFilePtr, "oid=\"%s\" fqoid=\"%s\" ", tmpptr->id,
                        tmpptr->oid); 
          }

          if(!isentry){
           if(tmpptr->type){
            if(strcmp(tmpptr->type, "OctetString") == 0) {
                fprintf(outputFilePtr, "type=\"OCTET STRING");
            }
            else if(strcmp(tmpptr->type, "ObjectID") == 0) {
                fprintf(outputFilePtr, "type=\"OBJECT IDENTIFIER");
            }
            else {
                fprintf(outputFilePtr, "type=\"%s", tmpptr->type);
            }

            /* if range exists handle properly */
            i = 0;
	    
	    /*range*/
            if(tmpptr->range[i]) {
	       print_ll_range(tmpptr, outputFilePtr);
#ifdef NOT_YET
               if(strcmp(tmpptr->type, "OctetString") == 0) {
                  if(tmpptr->range[i]->lower ==
                    tmpptr->range[i]->upper) {
                    /*just use one*/
                    fprintf(outputFilePtr, "(SIZE (%d",tmpptr->range[i]->lower);
                  }
                  else {
                    fprintf(outputFilePtr, "(SIZE (%d..%d",tmpptr->
                        range[i]->lower, tmpptr->range[i]->upper);
                  }
               }
               /*only positive numbers*/
               else if((strcmp(tmpptr->type, "Unsigned32") == 0) ||
                       (strcmp(tmpptr->type, "UInteger32") == 0) ||
                       (strcmp(tmpptr->type, "Counter") == 0) ||
                       (strcmp(tmpptr->type, "Counter32") == 0) ||
                       (strcmp(tmpptr->type, "Gauge") == 0) || 
                       (strcmp(tmpptr->type, "Gauge32") == 0)) {
                          fprintf(outputFilePtr, "(%u..%u",
                           tmpptr->range[i]->lower, tmpptr->range[i]->upper);
               }
               /*only positive large numbers*/
               else if(strcmp(tmpptr->type, "Counter64") == 0) {
                     fprintf(outputFilePtr, "(%lu..%lu",
                           (unsigned long)tmpptr->range[i]->lower, 
                           (unsigned long)tmpptr->range[i]->upper);
               }
               /*can have positive or negatives*/
               else {
                  fprintf(outputFilePtr, "(%d..%d",tmpptr->range[i]->lower,
                        tmpptr->range[i]->upper);
               }

               i++;

               while(i) {
                if(tmpptr->range[i]) {
                  if(strcmp(tmpptr->type, "OctetString") == 0) {
                        if(tmpptr->range[i]->lower ==
                                tmpptr->range[i]->upper) {
                          /*just use one*/
                          fprintf(outputFilePtr, " | %d",
                                tmpptr->range[i]->lower);
                        }
                        else {
                           fprintf(outputFilePtr, " | %d..%d",
                             tmpptr->range[i]->lower,tmpptr->range[i]->upper);
                        }
                   }
                   /*only positive numbers*/
                   else if((strcmp(tmpptr->type, "Unsigned32") == 0) ||
                       (strcmp(tmpptr->type, "UInteger32") == 0) ||
                       (strcmp(tmpptr->type, "Counter") == 0) ||
                       (strcmp(tmpptr->type, "Counter32") == 0) ||
                       (strcmp(tmpptr->type, "Gauge") == 0) || 
                       (strcmp(tmpptr->type, "Gauge32") == 0)) {
                          fprintf(outputFilePtr, "(%u..%u",
                           tmpptr->range[i]->lower, tmpptr->range[i]->upper);
                   }
                   /*only positive large numbers*/
                   else if(strcmp(tmpptr->type, "Counter64") == 0) {
                     fprintf(outputFilePtr, "(%lu..%lu",
                           (unsigned long)tmpptr->range[i]->lower, 
                           (unsigned long)tmpptr->range[i]->upper);
                   }
                   /*can have positive or negatives*/
                   else {
                     fprintf(outputFilePtr, "(%d..%d",tmpptr->range[i]->lower,
                        tmpptr->range[i]->upper);
                   }

                   i++;
                 }
                 else {
                   i=0;
                 }
              } /* end while */
              if(strcmp(tmpptr->type, "OctetString") == 0) {
                fprintf(outputFilePtr, "))");
              }
              else {
                fprintf(outputFilePtr, ")");
              }

#endif /* NOT_YET */
            } /*end if range*/

            /* add enumerations */
            if(tmpptr->enum_list){
              print_ll_enum(tmpptr->enum_list, outputFilePtr);
            }

            fprintf(outputFilePtr, "\" ");

	    /* if type is a textual convention, add primitive type */
	    if(tmpptr->primtype != NULL) {
              if(strcmp(tmpptr->primtype, "OctetString") == 0) {
                fprintf(outputFilePtr, "primtype=\"OCTET STRING\" ");
              }
              else if(strcmp(tmpptr->primtype, "ObjectID") == 0) {
                fprintf(outputFilePtr, "primtype=\"OBJECT IDENTIFIER\" ");
              }
              else {
		  fprintf(outputFilePtr, "primtype=\"%s\" ", tmpptr->primtype);
	      }
	    }
           }

           /* table */
           } else if(strcmp(tmpptr->component_type, "ModuleConformance") !=0) {
            fprintf(outputFilePtr, "type=\"Aggregate\" ");
           }

	   if(tmpptr->access){
             fprintf(outputFilePtr, "access=\"%s\" ", tmpptr->access);
           }
	
	   /* add defval */
	   if(tmpptr->defval){
	     fprintf(outputFilePtr, "defval=\"%s\" ", tmpptr->defval);
	   }

           /* If an ObjectGroup or a Notification, add list of objects */
           if((strcmp(tmpptr->component_type, "ObjectGroup") == 0) ||
	      (strcmp(tmpptr->component_type, "Notification") ==0) ) {
            if(tmpptr->obj_list){
              fprintf(outputFilePtr, "objectList=");
              print_ll_objlist(tmpptr->obj_list, outputFilePtr);
            }
           }

           /* If a NotificationGroup, add list of notifications */
           if(strcmp(tmpptr->component_type, "NotificationGroup") == 0) {
            if(tmpptr->obj_list){
              fprintf(outputFilePtr, "notificationList=");
              print_ll_objlist(tmpptr->obj_list, outputFilePtr);
            }
           }

           /* add index */
           if(tmpptr->index_list){
		 /* check to see if it is an augments instead of index */
		 if(tmpptr->augments) {
                   fprintf(outputFilePtr, "augments=");
                   print_ll_index(tmpptr->index_list, outputFilePtr);
 		 }
		 else {
                   fprintf(outputFilePtr, "index=");
                   print_ll_index(tmpptr->index_list, outputFilePtr);
		}
           }

           /* check to see if we need to include paragraph info */
           /* check for contactInfo, description, reference, or 
	    * module compliance */
           if((tmpptr->descr) || (tmpptr->reference) || (tmpptr->module_comp)){
              paragraph_flag = 1;
           }

           /* look to next OID and compare */
           if ((tmpptr->next_sorted) &&
            (tmpptr->next_sorted->oid)) {
              nextOID = oidlength(tmpptr->next_sorted->oid);

              /*check to make sure next oid is not more than just one sid
               * in difference (i.e. thisOID = 1.2 nextOID = 1.2.1.2
               
              if(nextOID - thisOID > 1) {
                 nextOID = thisOID + 1;
              }*/
	      prevOID = thisOID;
	      prevFixedOID = thisFixedOID;
              next_len = nextOID;
              diff = (next_len - thisOID);
           } else {
              next_len = baseOID;
	      /* Since at the end, use thisFixedOID to calculate diff */
	      /*diff = (next_len - thisFixedOID);*/
	      diff = (next_len - thisOID);
           }

           /* if at the same level, and no paragraph, close xml tag */
           if ((diff <= 0) && (paragraph_flag == 0)) {
            end_string = poptag();
            end_string = NULL;

            fprintf(outputFilePtr, "/>\n");
           } else /* close brace, but leave tag open to be closed later */ {
            fprintf(outputFilePtr, ">\n");
           }

           /* Add paragraph components */
           if(paragraph_flag) {

            /* add list of objects */
            /*if(strcmp(tmpptr->component_type, "ObjectGroup") == 0) {
             if(tmpptr->obj_list){
              fprintf(outputFilePtr, "\n");
              indent(outputFilePtr, thisFixedOID - baseOID + 1);
              fprintf(outputFilePtr, "<objectList>\n");
              print_ll_objlist(tmpptr->obj_list, outputFilePtr);
              fprintf(outputFilePtr, "</objectList>");
             }
            }*/

            /* add description */
            if(tmpptr->descr){
             fprintf(outputFilePtr, "\n");
             indent(outputFilePtr, thisFixedOID - baseOID + 1);
	     tmpptr->descr = xml_correct_string(tmpptr->descr, 
			strlen(tmpptr->descr));
             fprintf(outputFilePtr, "<description>\"%s\"\n",
                        tmpptr->descr);
             indent(outputFilePtr, thisFixedOID - baseOID + 1);
             fprintf(outputFilePtr, "</description>\n");
            }

            /* add reference */
            if(tmpptr->reference){
             fprintf(outputFilePtr, "\n");
             indent(outputFilePtr, thisFixedOID - baseOID + 1);

             fprintf(outputFilePtr, "<reference>\"%s\"\n",tmpptr->reference);
             indent(outputFilePtr, thisFixedOID - baseOID + 1);
             fprintf(outputFilePtr, "</reference>\n");
            }

	    /* add module compliance info */
	    if(tmpptr->module_comp){
		print_modcomp_xml((thisFixedOID - baseOID + 1), outputFilePtr, 
					tmpptr);
	    }

            /* if at the end of a level, close brace */
	    tmpdiff = diff;
            if (tmpdiff <= 0) {
              indent(outputFilePtr, thisFixedOID - baseOID);
	      while(tmpdiff <=0) {
                end_string = poptag();

		/* only close "real" items popped from the stack */
		if(strcmp(end_string, "") != 0){
                  fprintf(outputFilePtr, "</%s>\n", end_string);
		  break;
		}
		tmpdiff++;
	      }
	      if(strcmp(end_string, "ModuleIdentity") == 0) {
		/*Just popped off the Module Identity*/
		breakflag = 1;
	      }
              end_string = NULL;
            }
          } /* end if paragraph */

          /* if at the end of a level, go up and close brace */
          for (i = -1, j=0; i >= diff; i--) {
	    /* pop off extraneous empty strings off the stack */
	    while(i >= diff) {
              end_string = poptag();
	      if(strcmp(end_string, "") != 0){
		break;
	      }
	      i--;
	      j++;
	    }

            if(strcmp(end_string, "") != 0){
              indent(outputFilePtr, thisFixedOID + (i+j) - baseOID);
              fprintf(outputFilePtr, "</%s>\n", end_string);
	    }
	    if(strcmp(end_string, "ModuleIdentity") == 0) {
                /*Just popped off the Module Identity*/
                breakflag = 1;
		break;
            } 
            end_string = NULL;
          }

          tmpptr->doneTag = 1;

          tmpptr = tmpptr->next_sorted;
          paragraph_flag = 0;

	} while(tmpptr != NULL && breakflag ==0);
      } /* if !breakflag */

     } /*end if ModuleIdentity*/
     else {
 	tmpptr = tmpptr->next_sorted;
     }

   }while(tmpptr != NULL && breakflag==0);
   
   
   /* Now loop through again and print out any remaining components not
    * located under the Module-Identity
    */
   /* subtract 1 from the baseOID to allow for the MIBModule on the stack*/
   
   tmpptr = sorted_root;
   baseOID = (oidlength(tmpptr->oid) - 1);

   /* for use later on */
   prevOID = baseOID + 1;
   prevFixedOID = baseOID + 1;

   breakflag = 0;	/* to be used later to identify if we need to 
			 * physically close the MIBModule tag */

   do {

    if(!(tmpptr->doneTag)) {
       breakflag = 1; 
       isentry = 0;

       /* add component and name */
       if(tmpptr->name){

         /* add opening bracket and component type*/
         if(tmpptr->component_type){

	   /*get correct indentions*/
	   if(tmpptr->oid){
		thisOID = oidlength(tmpptr->oid);

                /*check to make sure this oid is not more than just one sid
                 * in difference from the previous
                 * (i.e. prevOID = 1.2 thisOID = 1.2.1.2
                 */
                if(thisOID - prevOID > 0) {
                   /* check for skipping over sids and push empty data
                    * onto the stack
                    */
                   for(i = thisOID - prevOID; i > 1; i--) {
                        pushtag("");
                   }

                   thisFixedOID = prevFixedOID + 1;
                }
		else if(thisOID == prevOID) {
		  thisFixedOID = prevFixedOID;
		}
		else {
		  thisFixedOID = thisOID;
		}

	   	indent(outputFilePtr, thisFixedOID - baseOID);
	   }
	   /*push onto stack*/
	   pushtag(tmpptr->component_type);

           fprintf(outputFilePtr, "<%s ", tmpptr->component_type);
         }
         else {
          tmpptr = tmpptr->next_sorted;
          continue;
         }

         fprintf(outputFilePtr, "name=\"%s\" ", tmpptr->name);
       }
       else {
           /* used to jump over nonLeaf only output */
           tmpptr = tmpptr->next_sorted;
           continue;
       }

       /* add status*/
       if(tmpptr->status) {
            fprintf(outputFilePtr, "status=\"%s\" ", tmpptr->status);
       }

       /* add oid */
       if(tmpptr->id){
           fprintf(outputFilePtr, "oid=\"%s\" fqoid=\"%s\" ", tmpptr->id,
                        tmpptr->oid);
       }

       /* add type */
       if(!isentry){
          if(tmpptr->type){
            if(strcmp(tmpptr->type, "OctetString") == 0) {
                fprintf(outputFilePtr, "type=\"OCTET STRING");
            }
            else if(strcmp(tmpptr->type, "ObjectID") == 0) {
                fprintf(outputFilePtr, "type=\"OBJECT IDENTIFIER");
            }
            else {
                fprintf(outputFilePtr, "type=\"%s", tmpptr->type);
		
            }

	    /* if range exists handle properly */
            i = 0;

            /*range*/
            if(tmpptr->range[i]) {
               print_ll_range(tmpptr, outputFilePtr);

#ifdef NOT_YET
               if(strcmp(tmpptr->type, "OctetString") == 0) {
                  if(tmpptr->range[i]->lower == 
                    tmpptr->range[i]->upper) {
                    /*just use one*/
                    fprintf(outputFilePtr, "(SIZE (%d",tmpptr->range[i]->lower);
                  }
                  else {
                    fprintf(outputFilePtr, "(SIZE (%d..%d",tmpptr->
			range[i]->lower, tmpptr->range[i]->upper);
                  }
               }
               /*only positive numbers*/
               else if((strcmp(tmpptr->type, "Unsigned32") == 0) ||
                       (strcmp(tmpptr->type, "UInteger32") == 0) ||
                       (strcmp(tmpptr->type, "Counter") == 0) ||
                       (strcmp(tmpptr->type, "Counter32") == 0) ||
                       (strcmp(tmpptr->type, "Gauge") == 0) || 
                       (strcmp(tmpptr->type, "Gauge32") == 0)) {
                          fprintf(outputFilePtr, "(%u..%u",
                           tmpptr->range[i]->lower, tmpptr->range[i]->upper);
               }
               /*only positive large numbers*/
               else if(strcmp(tmpptr->type, "Counter64") == 0) {
                     fprintf(outputFilePtr, "(%lu..%lu",
                           (unsigned long)tmpptr->range[i]->lower, 
                           (unsigned long)tmpptr->range[i]->upper);
               }
               /*can have positive or negatives*/
               else {
                  fprintf(outputFilePtr, "(%d..%d",tmpptr->range[i]->lower, 
			tmpptr->range[i]->upper);
               }

               i++;

               while(i) {
                if(tmpptr->range[i]) {
                  if(strcmp(tmpptr->type, "OctetString") == 0) {
                        if(tmpptr->range[i]->lower ==
                                tmpptr->range[i]->upper) {
                          /*just use one*/
                          fprintf(outputFilePtr, " | %d",
                                tmpptr->range[i]->lower);
                        }
                        else {
                           fprintf(outputFilePtr, " | %d..%d",
                             tmpptr->range[i]->lower,tmpptr->range[i]->upper);
                        }
                   }
                   /*only positive numbers*/
                   else if((strcmp(tmpptr->type, "Unsigned32") == 0) ||
                       (strcmp(tmpptr->type, "UInteger32") == 0) ||
                       (strcmp(tmpptr->type, "Counter") == 0) ||
                       (strcmp(tmpptr->type, "Counter32") == 0) ||
                       (strcmp(tmpptr->type, "Gauge") == 0) ||
                       (strcmp(tmpptr->type, "Gauge32") == 0)) {
                          fprintf(outputFilePtr, "(%u..%u",
                           tmpptr->range[i]->lower, tmpptr->range[i]->upper);
                   }
                   /*only positive large numbers*/
                   else if(strcmp(tmpptr->type, "Counter64") == 0) {
                     fprintf(outputFilePtr, "(%lu..%lu",
                           (unsigned long)tmpptr->range[i]->lower,
                           (unsigned long)tmpptr->range[i]->upper);
                   }
                   /*can have positive or negatives*/
                   else {
                        fprintf(outputFilePtr, " | %d..%d",
                           tmpptr->range[i]->lower, tmpptr->range[i]->upper);
                   }
                   i++;
                 }
                 else {
                   i=0;
                 }
              } /* end while */
              if(strcmp(tmpptr->type, "OctetString") == 0) {
                fprintf(outputFilePtr, "))");
              }
              else {
                fprintf(outputFilePtr, ")");
              }
#endif /* NOT_YET */
            } /*end if range*/

	    /* add enumerations */
            if(tmpptr->enum_list){
              print_ll_enum(tmpptr->enum_list, outputFilePtr);
            }

            fprintf(outputFilePtr, "\" ");

	    /* if type is a textual convention, add primitive type */
	    if(tmpptr->primtype != NULL) {
              if(strcmp(tmpptr->primtype, "OctetString") == 0) {
                fprintf(outputFilePtr, "primtype=\"OCTET STRING\" ");
              }
              else if(strcmp(tmpptr->primtype, "ObjectID") == 0) {
                fprintf(outputFilePtr, "primtype=\"OBJECT IDENTIFIER\" ");
              }
              else {
		  fprintf(outputFilePtr, "primtype=\"%s\" ", tmpptr->primtype);
	      }
	    }
          }

	/* table */
        } else if(strcmp(tmpptr->component_type, "ModuleConformance") !=0) {
	    fprintf(outputFilePtr, "type=\"Aggregate\" ");
        }

	/* add access */
        if(tmpptr->access){
             fprintf(outputFilePtr, "access=\"%s\" ", tmpptr->access);
	}

  	/* add defval */
        if(tmpptr->defval){
          fprintf(outputFilePtr, "defval=\"%s\" ", tmpptr->defval);
        }

	/* If an ObjectGroup, add list of objects */
	if(strcmp(tmpptr->component_type, "ObjectGroup") == 0) {
            if(tmpptr->obj_list){
              fprintf(outputFilePtr, "objectList=");
              print_ll_objlist(tmpptr->obj_list, outputFilePtr);
            }
        }

	/* If a NotificationGroup, add list of notifications */
	if(strcmp(tmpptr->component_type, "NotificationGroup") == 0) {
            if(tmpptr->obj_list){
              fprintf(outputFilePtr, "notificationList=");
              print_ll_objlist(tmpptr->obj_list, outputFilePtr);
            }
        }

	/* add index */
        if(tmpptr->index_list){
                 /* check to see if it is an augments instead of index */
                 if(tmpptr->augments) {
                   fprintf(outputFilePtr, "augments=");
                   print_ll_index(tmpptr->index_list, outputFilePtr);
                 }
                 else {
                   fprintf(outputFilePtr, "index=");
                   print_ll_index(tmpptr->index_list, outputFilePtr);
                }
        }
        
	/* add augments */
	if(tmpptr->augments){
	
	}

	/* check to see if we need to include paragraph info */
	/* check for contactInfo, description, reference */
	/* or module compliance	*/
	if((tmpptr->descr) || (tmpptr->reference) || (tmpptr->module_comp)){
	      paragraph_flag = 1;
	}

 	/* look to next OID and compare */
        if((tmpptr->next_sorted) && (tmpptr->next_sorted->doneTag)) {
	      /* have to skip over this one and look to the next one */
	      skipptr = tmpptr->next_sorted;
	      while(skipptr) {
		if((skipptr->next_sorted) && (skipptr->next_sorted->doneTag)) {
		   /*keep skipping*/
		   skipptr = skipptr->next_sorted;
		}
		else if ((skipptr->next_sorted) && (skipptr->next_sorted->oid)){
		   nextOID = oidlength(skipptr->next_sorted->oid);

                   /*check to make sure next oid is not more than just one sid
                    * in difference (i.e. thisOID = 1.2 nextOID = 1.2.1.2
                    *
                   if(nextOID - thisOID > 1) {
                     nextOID = thisOID + 1;
                   }*/
		   prevOID = thisOID;
		   prevFixedOID = thisFixedOID;
		   next_len = nextOID;
                   diff = (next_len - thisOID);
		   skipptr = NULL;
		}
		else {
		  next_len = baseOID;
	          /* Since at the end, use thisFixedOID to calculate diff */
                  /*diff = (next_len - thisFixedOID);*/
                  diff = (next_len - thisOID);
	 	  skipptr = NULL;
        	}
	      }
	}
        else if ((tmpptr->next_sorted) &&
	    (tmpptr->next_sorted->oid)) {
	      nextOID = oidlength(tmpptr->next_sorted->oid);

              /*check to make sure next oid is not more than just one sid
               * in difference (i.e. thisOID = 1.2 nextOID = 1.2.1.2
               *
              if(nextOID - thisOID > 1) {
                 nextOID = thisOID + 1;
              }*/
	      prevOID = thisOID;
	      prevFixedOID = thisFixedOID;
              next_len = nextOID;
              diff = (next_len - thisOID);
        } else {
	      /* Since at the end, use thisFixedOID to calculate diff */
              next_len = baseOID;
              /*diff = (next_len - thisFixedOID);*/
              diff = (next_len - thisOID);
        }

	  
  	/* if at the same level, and no paragraph, close xml tag */
        if ((diff <= 0) && (paragraph_flag == 0)) {
            end_string = poptag();
	    end_string = NULL;

            fprintf(outputFilePtr, "/>\n");
        } else /* close brace, but leave tag open to be closed later */ {
            fprintf(outputFilePtr, ">\n");
        }

	/* Add paragraph components */
	if(paragraph_flag) {
	  /* add list of objects */
	  /*if(strcmp(tmpptr->component_type, "ObjectGroup") == 0) {
            if(tmpptr->obj_list){
	      fprintf(outputFilePtr, "\n");
              indent(outputFilePtr, thisFixedOID - baseOID + 1);
              fprintf(outputFilePtr, "<objectList>\n");
              print_ll_objlist(tmpptr->obj_list, outputFilePtr);
              fprintf(outputFilePtr, "</objectList>");
            }
	  }*/
	
	  /* add description */
	  if(tmpptr->descr){
	     fprintf(outputFilePtr, "\n");
	     indent(outputFilePtr, thisFixedOID - baseOID + 1);
	     fprintf(outputFilePtr, "<description>\"%s\"\n",
			tmpptr->descr);
	     indent(outputFilePtr, thisFixedOID - baseOID + 1);
             fprintf(outputFilePtr, "</description>\n");
	  }

 	  /* add reference */
	  if(tmpptr->reference){
	     fprintf(outputFilePtr, "\n");
	     indent(outputFilePtr, thisFixedOID - baseOID + 1);

	     fprintf(outputFilePtr, "<reference>\"%s\"\n",tmpptr->reference);
	     indent(outputFilePtr, thisFixedOID - baseOID + 1);
             fprintf(outputFilePtr, "</reference>\n");
	   }
 
           /* add module compliance info */
           if(tmpptr->module_comp){
              print_modcomp_xml((thisFixedOID - baseOID + 1), outputFilePtr, 
				tmpptr);
           }

	   
	  fprintf(outputFilePtr, "\n");
	
	  /* if at the end of a level, close brace */
          tmpdiff = diff;
          if (tmpdiff <= 0) {
              indent(outputFilePtr, thisFixedOID - baseOID);
              while(tmpdiff <=0) {
                end_string = poptag();

                /* only close "real" items popped from the stack */
                if(strcmp(end_string, "") != 0){
                  fprintf(outputFilePtr, "</%s>\n", end_string);
                  break;
                }
                tmpdiff++;
              } 
	      end_string = NULL;
	  }
	} /* end if paragraph */

	/* if at the end of a level, go up and close brace */
        for (i = -1, j=0; i >= diff; i--) {
            /* pop off extraneous empty strings off the stack */
            while(i >= diff) {
              end_string = poptag();
              if(strcmp(end_string, "") != 0){
                break;
              }
              i--;
              j++;
            }

	    if(strcmp(end_string, "") != 0){
              indent(outputFilePtr, thisFixedOID + (i+j) - baseOID);
              fprintf(outputFilePtr, "</%s>\n", end_string);
	    }
	    end_string = NULL;
        }

        tmpptr = tmpptr->next_sorted;
	paragraph_flag = 0;
     }
     /*skip over if already processed*/
     else {
 	tmpptr = tmpptr->next_sorted;
     }

   } while(tmpptr != NULL);

   if(!breakflag){  /* everything was listed under the Module Identity
		     * need to close with MIBModule tag
		     */ 
     indent(outputFilePtr, 0);
     end_string = poptag();

     fprintf(outputFilePtr, "</%s>\n", end_string);
     end_string = NULL;
   }

  }
  else {  /*no OIDs, close the tag*/
   fprintf(outputFilePtr, "</MIBModule>\n");
  }

  if(multiple_mibs && last_parse) {
     fprintf(outputFilePtr, "</MIBModules>\n");
  }

  fprintf(outputFilePtr, "\n\n");
  if(!explicitOutputFile){
       outputFilePtr = tmpFilePtr;
       tmpFilePtr = 0;
  }

  if(end_string) {
    free(end_string);
  }

  return;
}

void
print_modcomp_xml(int indent_num, FILE *outputFilePtr, ObjData *tmpptr) {

  CondGroup * condGroup;
  CondException * condException;
  ModuleComp *modptr;
  tree * tmpPtr = NULL;

  modptr = tmpptr->module_comp;

  while(modptr) {

    /* print the module info */
    indent(outputFilePtr, indent_num);
    if(modptr->mandatory_groups) {
      if(!modptr->name) {
        fprintf(outputFilePtr, "<module mandatoryGroups=\"%s\">\n",
		modptr->mandatory_groups);
      }
      else {
	fprintf(outputFilePtr, "<module name=\"%s\" mandatoryGroups=\"%s\">\n",
                modptr->name, modptr->mandatory_groups);
      }
    }   
    else {
      if(!modptr->name) {
        fprintf(outputFilePtr, "<module>\n");
      }
      else {
	fprintf(outputFilePtr, "<module name=\"%s\">\n", modptr->name);
      }
    }


    /* print any conditional groups */
    if(modptr->cond_group) {
      condGroup = modptr->cond_group;

      while(condGroup) {
 	if(condGroup->name) {
    	  indent(outputFilePtr, indent_num + 1);
    	  fprintf(outputFilePtr, "<conditionalGroup name=\"%s\">\n", 
		condGroup->name);
	  if(condGroup->descr) {
		indent(outputFilePtr, indent_num + 2);
		fprintf(outputFilePtr, "<description>\"%s\"\n", 
			condGroup->descr);
		indent(outputFilePtr, indent_num + 2);
        	fprintf(outputFilePtr, "</description>\n");
	  }
    	  indent(outputFilePtr, indent_num + 1);
    	  fprintf(outputFilePtr, "</conditionalGroup>\n");
	}
	condGroup = condGroup->next;
      }
    }

    /* print any conditional exceptions */
    if(modptr->cond_exception) {
      condException = modptr->cond_exception;

      while(condException) {
	if(condException->name) {
          indent(outputFilePtr, indent_num + 1);
          fprintf(outputFilePtr, "<exception name=\"%s\" ", 
                condException->name);
	  if(condException->min_access) {
        	fprintf(outputFilePtr, "minAccess=\"%s\" ",
			condException->min_access);
	  }
	  if(condException->syntax) {
		fprintf(outputFilePtr, "syntax=\"%s",
                        condException->syntax);
	    /* check for type to retain primitive type for 
	     * textual convention. Do not do this if application type.  
	     */
              
            if(!application_type_test(condException->syntax)){
                   tmpPtr = find_type(condException->syntax);
            }

            /* use the primitive type for textual conventions */
            /* which are not enumerations */

            if(tmpPtr && tmpPtr->child && 
			tmpPtr->child[child_syntax___1]->string){

             if(application_type_test(tmpPtr->
			child[child_syntax___1]->string)){
                condException->primtype = strdup(tmpPtr->
			child[child_syntax___1]->string);
             } 
	     else {
                condException->primtype = find_prim_type(tmpPtr->
			child[child_syntax___1]->string);
             }
            }   /* if tmpPtr */
	    else {
		condException->primtype = strdup(condException->syntax);
	    }

	    /*range*/
            if(condException->range[0]) {
              fprintf(outputFilePtr, " ");
	      /*call print_modrange with num==1 indicating regular syntax*/
	      print_modrange(condException, outputFilePtr, 1);
	    }
            /* add enumerations */
            if(condException->enum_list){
              fprintf(outputFilePtr, " ");
              print_ll_enum(condException->enum_list, outputFilePtr);
            }
            fprintf(outputFilePtr, "\"");
	  }

	  if(condException->write_syntax) {
	    if(condException->syntax) { /*add an extra space*/
		fprintf(outputFilePtr, " writeSyntax=\"%s",
                        condException->write_syntax);
            }
  	    else {
		fprintf(outputFilePtr, "writeSyntax=\"%s",
                        condException->write_syntax);
	    }
	    /* check for type to retain primitive type for 
	     * textual convention. Do not do this if application type.  
	     */
              
            if(!application_type_test(condException->write_syntax)){
                   tmpPtr = find_type(condException->write_syntax);
            }

            /* use the primitive type for textual conventions */
            /* which are not enumerations */

            if(tmpPtr && tmpPtr->child && 
			tmpPtr->child[child_syntax___1]->string){

             if(application_type_test(tmpPtr->
			child[child_syntax___1]->string)){
                condException->primtype_write = strdup(tmpPtr->
			child[child_syntax___1]->string);
             } 
	     else {
                condException->primtype_write = find_prim_type(tmpPtr->
			child[child_syntax___1]->string);
             }
            }   /* if tmpPtr */
            else {
                condException->primtype_write = strdup(condException->
			write_syntax);
            }

            /*range*/
            if(condException->range_write[0]) {
              fprintf(outputFilePtr, " ");
              /*call print_modrange with num==2 indicating write syntax*/
              print_modrange(condException, outputFilePtr, 2);
            }
            /* add enumerations */
            if(condException->enum_list_write){
              fprintf(outputFilePtr, " ");
              print_ll_enum(condException->enum_list_write, outputFilePtr);
            }
            fprintf(outputFilePtr, "\"");
	  }

	  fprintf(outputFilePtr, ">\n");

	  if(condException->descr) {
        	indent(outputFilePtr, indent_num + 2);
        	fprintf(outputFilePtr, "<description>\"%s\"\n", 
			condException->descr);
		indent(outputFilePtr, indent_num + 2);
        	fprintf(outputFilePtr, "</description>\n");
	  }
	  indent(outputFilePtr, indent_num + 1);
          fprintf(outputFilePtr, "</exception>\n");

	}
        condException = condException->next;
      } 
    }

    indent(outputFilePtr, indent_num);
    fprintf(outputFilePtr, "</module>\n");
  
    modptr = modptr->next;
  }

  return;

}

char *
build_oid(tree * tmpPtr) 
{

    tree * tmpExistsPtr		 = NULL;
    tree * typeExistsPtr         = NULL;
    char *retval = NULL;
    char *new_retval=NULL;
    char tmpoidptr[256];
    int len=0, i=0, j=0, c=0;
    int strsize=0;
    char oid[256];
    char *oidptr=NULL;
    int unresolved_imports_flag = 0;

   /* emit type and all parents back to root */ 

   memset(oid, 0, 256);

   if(tmpPtr->string){
       typeExistsPtr = find_type(tmpPtr->string);
   }

   if (!typeExistsPtr) {
       return NULL;
   }
   
   while(typeExistsPtr) {
             /* trace def back to root */
        memset(tmpoidptr, 0, 256);
        
        if(typeExistsPtr->child[child_value____9] && 
           typeExistsPtr->child[child_value____9]->string) {
           if(typeExistsPtr->child[child_value____9]->next_tree_node) {
	     if(typeExistsPtr->child[child_value____9]->last_tree_node_in_list 
		&& typeExistsPtr->child[child_value____9]->
 		   last_tree_node_in_list != typeExistsPtr->
		   child[child_value____9]->next_tree_node){
  	        /* Have more suboids to process*/
	        tmpExistsPtr = typeExistsPtr->child[child_value____9]->
				next_tree_node;
        	while(tmpExistsPtr) {
          	  strcat(tmpoidptr, ".");
		  /*BAB - for oids like org(3), child[9]->string is "org" and
		   *      child[9]->child[0] is "3". Use the 3 instead of "org"
		   */
		  if(tmpExistsPtr->child[child_identity_0]) {
	            /* Use the numeric format */
		    strcat(tmpoidptr, 
			tmpExistsPtr->child[child_identity_0]->string);
		  }
	 	  else {  
          	    strcat(tmpoidptr, tmpExistsPtr->string);
 		  }
		  /*BAB*/

          	  tmpExistsPtr = tmpExistsPtr->next_tree_node;
        	}
	
                /* now reverse them to align correctly with last sid first*/

	        len = strlen(tmpoidptr);

    	     	/* now reverse the string s in place */
    		for(i = 0, j = len-1; i< j;i++, j--) {
        	  c = tmpoidptr[i];
        	  tmpoidptr[i] = tmpoidptr[j];
        	  tmpoidptr[j] = c;
    		}
	 
	        /* Correct order of reversed subidentifiers */

    		oidptr = tmpoidptr;
   		retval = correctoid(oidptr);
		strcat(oid, retval);
                strcat(oid, ".");
		free(retval);
	     }
	     else {
		if(typeExistsPtr->child[child_value____9]->next_tree_node->
			child[child_identity_0]) {
                  strcat(oid, typeExistsPtr->child[child_value____9]->
			 next_tree_node->child[child_identity_0]->string);
                  }
                  else { 
		   strcat(oid, typeExistsPtr->child[child_value____9]->
				next_tree_node->string);
		  }
                strcat(oid, ".");
	     }
		
           }

	   /*BAB - check to see if starting OID is just a number*/
 	   if(typeExistsPtr->child[child_value____9]->number) {
		strcat(oid, typeExistsPtr->child[child_value____9]->string);
		typeExistsPtr = find_type(typeExistsPtr->
			child[child_value____9]->string);
	   }
	   /*BAB - check to see if starting OID is iso*/
	   else if(!strcmp("iso", typeExistsPtr->child[child_value____9]->
				string)) {
	     if((typeExistsPtr = find_type(typeExistsPtr->
			child[child_value____9]->string)) == NULL) {
		/* can't resolve iso, but we know it's 1 */
	        strcat(oid, "1");
	     }
	   }
#ifdef NOT_YET
	   else if(typeExistsPtr->child[child_value____9]->next_tree_node &&
	       typeExistsPtr->child[child_value____9]->next_tree_node->number) {
		/*BAB - copy the numeric format of the next tree node*/
	       	strcat(oid, typeExistsPtr->child[child_value____9]->
				next_tree_node->string);
#endif /* NOT_YET */
		
	   else {
		/*starting oid is not a number or iso. copy the string*/
		memset(tmpoidptr, 0, 255);
		strcpy(tmpoidptr, typeExistsPtr->child[child_value____9]->string);
		strcat(tmpoidptr, ".");
		/* if can't resolve, just copy this name by setting the flag */
		if((typeExistsPtr = find_type(typeExistsPtr->
                        child[child_value____9]->string)) == NULL) {
		  unresolved_imports_flag = 1;
		}
	   }
		
	    
           /*typeExistsPtr = find_type(typeExistsPtr->child[child_value____9]->string);*/

        } else {
           typeExistsPtr = NULL;
        }
    }

    /*strcat(oid, "1");*/

    len = strlen(oid);

    /* now reverse the string s in place */
    for(i = 0, j = len-1; i< j;i++, j--) {
 
        c = oid[i];
        oid[i] = oid[j];
        oid[j] = c;
 
    }

    /* all other items in list */
    tmpPtr = tmpPtr->next_tree_node;

    /* Correct order of reversed subidentifiers */
    oidptr = oid;
    retval = correctoid(oidptr);
 
   /* add any trailing subidentifiers */
    while(tmpPtr) {
        strsize = (strlen(retval) + strlen(tmpPtr->string) + 2);
        retval = (char *)realloc(retval, strsize);
	strcat(retval, ".");
	strcat(retval, tmpPtr->string);
        tmpPtr = tmpPtr->next_tree_node;
 
    }

    /* Check to see if we need to check for unresolved imports flag */
    if(unresolved_imports_flag) {
	strcat(tmpoidptr, retval);
	new_retval = (char *) malloc((sizeof(char) * strlen(tmpoidptr) +1));
	strcpy(new_retval, tmpoidptr);
	new_retval[strlen(tmpoidptr)] = '\0';

	return(new_retval);
    }

    return(retval);
}


char *
build_id(tree * tmpPtr)
{

   tree * typeExistsPtr = NULL;
   tree * tmpExistsPtr = NULL;
   char oid[256];
   char * retval = NULL;

   /* emit type and all parents back to root */

   memset(oid, 0, 256);

   if(tmpPtr->string){
       typeExistsPtr = find_type(tmpPtr->string);
   }

   if (!typeExistsPtr) {
       return NULL;
   }

   if(typeExistsPtr->child[child_value____9] &&
      typeExistsPtr->child[child_value____9]->string) {
      if(typeExistsPtr->child[child_value____9]->next_tree_node) {

	 strcat(oid, typeExistsPtr->child[child_value____9]->string);
	 tmpExistsPtr = typeExistsPtr->child[child_value____9]->next_tree_node;

	 while(tmpExistsPtr) {
           strcat(oid, ".");
           strcat(oid, tmpExistsPtr->string);

	   tmpExistsPtr = tmpExistsPtr->next_tree_node;

         }
	 /*strcat(oid, typeExistsPtr->child[child_value____9]->string);
           strcat(oid, ".");
           strcat(oid, typeExistsPtr->child[child_value____9]->
		next_tree_node->string);*/
	
      }
   }

   retval = strdup(oid);

   return(retval);
}


EnumInfo *
build_enumeration_info(tree *ptr)
{
    tree *tmpPtr = NULL;
    EnumInfo *newnode = NULL;
    EnumInfo *enumroot = NULL;
    EnumInfo *tmpEnumPtr = NULL;

    maxColumnWidth = 15;

    if( ptr->child[child_identity_0] && 
        ptr->child[child_identity_0]->string && 
        ptr->child[child_syntax___1] &&  
        ptr->child[child_syntax___1]->child[child_status___2] ) {
 
     /* enumerated values , NameItemList is always child[child_status___2] */
 
                if( (ptr->child[child_syntax___1]->type == BITS) || (ptr->child[child_syntax___1]->type == BIT) ) {

                    /* named item list */
                    tmpPtr = ptr->child[child_syntax___1]->child[child_status___2];
                    while(tmpPtr) {
                        if(tmpPtr->child[child_identity_0]) {

                            newnode = (EnumInfo *) malloc(sizeof(EnumInfo));
 
                    /* print enumerated object */
/*
                            if(ptr->child[child_identity_0]->type == NEG_NUMBER) {
                                newnode->name = strdup(ptr->child[child_identity_0]->string);
                            } else {
                                newnode->name = strdup(ptr->child[child_identity_0]->string);
                            }
*/

                    /*   prints out enumeration name. */

                       if(tmpPtr->string) {
                            if(tmpPtr->type == NEG_NUMBER) {
                                 newnode->name = strdup(tmpPtr->string);
                           } else {
                                 newnode->name = strdup(tmpPtr->string);
                            }
                        }

                   /* print enumeration integer value */
                       if(tmpPtr->child[child_identity_0] && tmpPtr->child[child_identity_0]->string) {
                            newnode->value = strdup(tmpPtr->child[child_identity_0]->string);
                       }
 
                        /* Add enumeration node to linked list */ 
                        if ( enumroot == NULL ) {
                            enumroot = newnode; 
                            newnode->next = NULL;
                            tmpEnumPtr = enumroot;
                        }
                        else {
                            tmpEnumPtr->next = newnode;
                            tmpEnumPtr = newnode;
                            newnode->next = NULL;
                        }
                   }

                        tmpPtr = tmpPtr->next_tree_node;
              } /* while tmpPtr */
           }
      }

      return(enumroot);
}

EnumInfo *
build_enumeration_modcomp_info(tree *ptr)
{
    tree *tmpPtr = NULL;
    EnumInfo *newnode = NULL;
    EnumInfo *enumroot = NULL;
    EnumInfo *tmpEnumPtr = NULL;

    maxColumnWidth = 15;

    if( ptr->child[child_identity_0] && 
        ptr->child[child_identity_0]->string && 
        ptr->child[child_misc1___14] &&  
        ptr->child[child_misc1___14]->child[child_status___2] ) {
 
     /* enumerated values , NameItemList is always child[child_status___2] */
 
     if( (ptr->child[child_misc1___14]->type == BITS) || 
	 (ptr->child[child_misc1___14]->type == BIT) ) {

        /* named item list */
        tmpPtr = ptr->child[child_misc1___14]->child[child_status___2];

        while(tmpPtr) {
             if(tmpPtr->child[child_identity_0]) {

               newnode = (EnumInfo *) malloc(sizeof(EnumInfo));
 
               /*   prints out enumeration name. */

               if(tmpPtr->string) {
                  if(tmpPtr->type == NEG_NUMBER) {
                     newnode->name = strdup(tmpPtr->string);
                  } 
		  else {
                     newnode->name = strdup(tmpPtr->string);
                  }
               }

               /* print enumeration integer value */
               if(tmpPtr->child[child_identity_0] && 
		  tmpPtr->child[child_identity_0]->string) {
                  newnode->value = strdup(tmpPtr->
				child[child_identity_0]->string);
               }
 
               /* Add enumeration node to linked list */ 
               if ( enumroot == NULL ) {
                  enumroot = newnode; 
                  newnode->next = NULL;
                  tmpEnumPtr = enumroot;
               }
               else {
                  tmpEnumPtr->next = newnode;
                  tmpEnumPtr = newnode;
                  newnode->next = NULL;
               }
             }

             tmpPtr = tmpPtr->next_tree_node;
         } /* while tmpPtr */
       }
    }

    return(enumroot);
}

void
print_ll_enum(EnumInfo *list, FILE *outputFilePtr)
{
   EnumInfo *tmpPtr = NULL;

     if (list == NULL) {
         return;
     }

     tmpPtr = list;

     if(formatFlag == XML) {
       if(tmpPtr !=NULL) {
         fprintf(outputFilePtr, "{");
	 fprintf(outputFilePtr, "%s", tmpPtr->name);
         fprintf(outputFilePtr, "(%s)", tmpPtr->value);
         tmpPtr = tmpPtr->next;

         while ( tmpPtr !=NULL ) {
         	fprintf(outputFilePtr, ", %s", tmpPtr->name);
         	fprintf(outputFilePtr, "(%s)", tmpPtr->value);
         	tmpPtr = tmpPtr->next;
       	 }
       	 fprintf(outputFilePtr, "}");
       }
     }
     else {  /* IDAT */
       fprintf(outputFilePtr, "(\n");
       while ( tmpPtr !=NULL ) {
         fprintf(outputFilePtr, "   %s   ", tmpPtr->value);
         fprintf(outputFilePtr, "%s\n", tmpPtr->name);
         tmpPtr = tmpPtr->next;
       }
       fprintf(outputFilePtr, ")");
     }

   return;

}

void
print_ll_range(ObjData *tmpptr, FILE * outPtr)
{

  int i=0;

  if(strcmp(tmpptr->type, "OctetString") == 0) {
    if(tmpptr->range[i]->lower == tmpptr->range[i]->upper) {
       /*just use one*/
       fprintf(outputFilePtr, "(SIZE (%d",tmpptr->range[i]->lower);
    }
    else {
       fprintf(outputFilePtr, "(SIZE (%d..%d",tmpptr->
                        range[i]->lower, tmpptr->range[i]->upper);
    }
  }
  /*only positive numbers*/
  else if((strcmp(tmpptr->type, "Unsigned32") == 0) ||
          (strcmp(tmpptr->type, "UInteger32") == 0) ||
          (strcmp(tmpptr->type, "Counter") == 0) ||
          (strcmp(tmpptr->type, "Counter32") == 0) ||
          (strcmp(tmpptr->type, "Gauge") == 0) || 
          (strcmp(tmpptr->type, "Gauge32") == 0)) {
     fprintf(outputFilePtr, "(%u..%u", tmpptr->range[i]->lower, 
		tmpptr->range[i]->upper);
  }
  /*only positive large numbers*/
  else if(strcmp(tmpptr->type, "Counter64") == 0) {
     fprintf(outputFilePtr, "(%lu..%lu", 
		(unsigned long)tmpptr->range[i]->lower, 
                (unsigned long)tmpptr->range[i]->upper);
  }
  /*can have positive or negatives*/
  else {
     fprintf(outputFilePtr, "(%d..%d",tmpptr->range[i]->lower,
                        tmpptr->range[i]->upper);
  }

  i++;

  while(i) {
     if(tmpptr->range[i]) {
       if(strcmp(tmpptr->type, "OctetString") == 0) {
         if(tmpptr->range[i]->lower == tmpptr->range[i]->upper) {
           /*just use one*/
           fprintf(outputFilePtr, " | %d", tmpptr->range[i]->lower);
         }
         else {
           fprintf(outputFilePtr, " | %d..%d",
                   tmpptr->range[i]->lower,tmpptr->range[i]->upper);
         }
       }
       /*only positive numbers*/
       else if((strcmp(tmpptr->type, "Unsigned32") == 0) ||
               (strcmp(tmpptr->type, "UInteger32") == 0) ||
               (strcmp(tmpptr->type, "Counter") == 0) ||
               (strcmp(tmpptr->type, "Counter32") == 0) ||
               (strcmp(tmpptr->type, "Gauge") == 0) || 
               (strcmp(tmpptr->type, "Gauge32") == 0)) {
          fprintf(outputFilePtr, "(%u..%u",
                           tmpptr->range[i]->lower, tmpptr->range[i]->upper);
       }
       /*only positive large numbers*/
       else if(strcmp(tmpptr->type, "Counter64") == 0) {
          fprintf(outputFilePtr, "(%lu..%lu",
                           (unsigned long)tmpptr->range[i]->lower, 
                           (unsigned long)tmpptr->range[i]->upper);
       }
       /*can have positive or negatives*/
       else {
          fprintf(outputFilePtr, "(%d..%d",tmpptr->range[i]->lower,
                        tmpptr->range[i]->upper);
       }

       i++;
     }
     else {
       i=0;
     }
   } /* end while */

   if(strcmp(tmpptr->type, "OctetString") == 0) {
     fprintf(outputFilePtr, "))");
   }
   else {
     fprintf(outputFilePtr, ")");
   }

  return;

}

void
print_modrange(CondException *condException, FILE * outFilePtr, int num)
{

  int i=0;
  int deduce_data_type=0;

  /* num indicates whether this is a syntax(1) or write syntax(2) */
  if(num==1) {
   /*syntax - use primtype*/
   if(strcmp(condException->primtype, "OctetString") == 0) {
    if(condException->range[i]->lower == condException->range[i]->upper) {
      /*just use one*/
      fprintf(outputFilePtr, "(SIZE (%d",condException->range[i]->lower);
    }
    else {
      fprintf(outputFilePtr, "(SIZE (%d..%d",condException->
                        range[i]->lower, condException->range[i]->upper);
    }
   }
   /*only positive numbers*/
   else if((strcmp(condException->primtype, "Unsigned32") == 0) ||
          	(strcmp(condException->primtype, "UInteger32") == 0) ||
          	(strcmp(condException->primtype, "Counter") == 0) ||
          	(strcmp(condException->primtype, "Counter32") == 0) ||
          	(strcmp(condException->primtype, "Gauge") == 0) ||
          	(strcmp(condException->primtype, "Gauge32") == 0)) {
    fprintf(outputFilePtr, "(%u..%u", condException->range[i]->lower, 
			condException->range[i]->upper);
   }
   /*only positive large numbers*/
   else if(strcmp(condException->primtype, "Counter64") == 0) {
    fprintf(outputFilePtr, "(%lu..%lu", 
	        (unsigned long)condException->range[i]->lower,
                (unsigned long)condException->range[i]->upper);
   }
   /*can have positive or negatives*/
   else {
    /* try to deduce whether this is OctetString type or not by seeing
     * if upper and lower ranges are the same
     */
    if(condException->range[i]->lower == condException->range[i]->upper) {
      /*just use one*/
      fprintf(outputFilePtr, "(SIZE (%d",condException->range[i]->lower);
      deduce_data_type = 1;
    }
    else {
      fprintf(outputFilePtr, "(%d..%d", condException->range[i]->lower, 
			condException->range[i]->upper);
    }
   }

   i++;

   while(i) {
    if(condException->range[i]) {
      if(strcmp(condException->primtype, "OctetString") == 0) {
        if(condException->range[i]->lower == condException->range[i]->upper) {
          /*just use one*/
          fprintf(outputFilePtr, " | %d", condException->range[i]->lower);
        }
        else {
          fprintf(outputFilePtr, " | %d..%d",
              condException->range[i]->lower, condException->range[i]->upper);
        }
      }
      /*only positive numbers*/
      else if((strcmp(condException->primtype, "Unsigned32") == 0) ||
               (strcmp(condException->primtype, "UInteger32") == 0) ||
               (strcmp(condException->primtype, "Counter") == 0) ||
               (strcmp(condException->primtype, "Counter32") == 0) ||
               (strcmp(condException->primtype, "Gauge") == 0) || 
               (strcmp(condException->primtype, "Gauge32") == 0)) {
        fprintf(outputFilePtr, "(%u..%u",
           condException->range[i]->lower, condException->range[i]->upper);
      }
      /*only positive large numbers*/
      else if(strcmp(condException->primtype, "Counter64") == 0) {
        fprintf(outputFilePtr, "(%lu..%lu",
           (unsigned long)condException->range[i]->lower, 
           (unsigned long)condException->range[i]->upper);
      }
      /*can have positive or negatives*/
      else {
    	if (deduce_data_type) {
      	  /*just use one*/
          fprintf(outputFilePtr, " | %d", condException->range[i]->lower);
    	}
    	else {
      	  fprintf(outputFilePtr, "(%d..%d", condException->range[i]->lower,
                        condException->range[i]->upper);
    	}
      }

      i++;
     }
     else {
      i=0;
     }
    } /* end while */

    if((strcmp(condException->primtype, "OctetString") == 0) ||
      (deduce_data_type)) {
     fprintf(outputFilePtr, "))");
    }
    else {
     fprintf(outputFilePtr, ")");
    }

  } /* end if syntax */

  else if(num==2) {
   /*write syntax - use primtype_write*/
   if(strcmp(condException->primtype_write, "OctetString") == 0) {
    if(condException->range_write[i]->lower == 
	condException->range_write[i]->upper) {
      /*just use one*/
      fprintf(outputFilePtr, "(SIZE (%d",condException->range_write[i]->lower);
    }
    else {
      fprintf(outputFilePtr, "(SIZE (%d..%d",condException->
		range_write[i]->lower, 
		condException->range_write[i]->upper);
    }
   }
   /*only positive numbers*/
   else if((strcmp(condException->primtype_write, "Unsigned32") == 0) ||
          	(strcmp(condException->primtype_write, "UInteger32") == 0) ||
          	(strcmp(condException->primtype_write, "Counter") == 0) ||
          	(strcmp(condException->primtype_write, "Counter32") == 0) ||
          	(strcmp(condException->primtype_write, "Gauge") == 0) ||
          	(strcmp(condException->primtype_write, "Gauge32") == 0)) {
    fprintf(outputFilePtr, "(%u..%u", condException->range_write[i]->lower, 
			condException->range_write[i]->upper);
   }
   /*only positive large numbers*/
   else if(strcmp(condException->primtype_write, "Counter64") == 0) {
    fprintf(outputFilePtr, "(%lu..%lu", 
	        (unsigned long)condException->range_write[i]->lower,
                (unsigned long)condException->range_write[i]->upper);
   }
   /*can have positive or negatives*/
   else {
    /* try to deduce whether this is OctetString type or not by seeing
     * if upper and lower ranges are the same
     */
    if(condException->range_write[i]->lower == 
		condException->range_write[i]->upper) {
      /*just use one*/
      fprintf(outputFilePtr, "(SIZE (%d",condException->range_write[i]->lower);
      deduce_data_type = 1;
    }
    else {
      fprintf(outputFilePtr, "(%d..%d", condException->range_write[i]->lower, 
			condException->range_write[i]->upper);
    }
   }

   i++;

   while(i) {
    if(condException->range_write[i]) {
      if(strcmp(condException->primtype_write, "OctetString") == 0) {
        if(condException->range_write[i]->lower == 
		condException->range_write[i]->upper) {
          /*just use one*/
          fprintf(outputFilePtr, " | %d", condException->range_write[i]->lower);
        }
        else {
          fprintf(outputFilePtr, " | %d..%d",
              condException->range_write[i]->lower, 
	      condException->range_write[i]->upper);
        }
      }
      /*only positive numbers*/
      else if((strcmp(condException->primtype_write, "Unsigned32") == 0) ||
               (strcmp(condException->primtype_write, "UInteger32") == 0) ||
               (strcmp(condException->primtype_write, "Counter") == 0) ||
               (strcmp(condException->primtype_write, "Counter32") == 0) ||
               (strcmp(condException->primtype_write, "Gauge") == 0) || 
               (strcmp(condException->primtype_write, "Gauge32") == 0)) {
        fprintf(outputFilePtr, "(%u..%u",
           condException->range_write[i]->lower, 
	   condException->range_write[i]->upper);
      }
      /*only positive large numbers*/
      else if(strcmp(condException->primtype_write, "Counter64") == 0) {
        fprintf(outputFilePtr, "(%lu..%lu",
           (unsigned long)condException->range_write[i]->lower, 
           (unsigned long)condException->range_write[i]->upper);
      }
      /*can have positive or negatives*/
      else {
    	if (deduce_data_type) {
      	  /*just use one*/
          fprintf(outputFilePtr, " | %d", condException->range_write[i]->lower);
    	}
    	else {
      	  fprintf(outputFilePtr, "(%d..%d", condException->
		range_write[i]->lower, condException->range_write[i]->upper);
    	}
      }

      i++;
     }
     else {
      i=0;
     }
    } /* end while */

    if((strcmp(condException->primtype_write, "OctetString") == 0) ||
      (deduce_data_type)) {
     fprintf(outputFilePtr, "))");
    }
    else {
     fprintf(outputFilePtr, ")");
    }
	
   } /* end if write syntax */

  return;

}
IndexInfo *
build_index_info(tree *ptr)
{

    tree *tmpPtr = NULL;
    tree * lookupIndexResult = NULL;
    IndexInfo *tmp_list = NULL;
    IndexInfo *tmp_ptr = NULL;
    IndexInfo *tmp_node = NULL;

     /* INDEX or AUGMENTS*/
    if( ptr->child[child_index____7] &&
        ptr->child[child_index____7]->type == INDEX &&
        ptr->child[child_index____7]->child[child_identity_0]) {
             
             tmpPtr = ptr->child[child_index____7]->child[child_identity_0];
 
             /* do we have at least one item? */
             while(tmpPtr && tmpPtr->string &&
                   tmpPtr->next_tree_node ) {
 
                if ( (tmp_node = (IndexInfo *) malloc (sizeof(IndexInfo))) == NULL) {
                      fprintf(stderr, "Unable to malloc memory for index\n");
                      return NULL;
                 }

                 memset((void *)tmp_node, '\0', sizeof(IndexInfo));

                 /*Check to see if INDEX is valid*/
                 if(!tmpPtr->type) {
                   /* if it is an imported type, type is 0*/
                   lookupIndexResult = NULL;
		   
		   /* look in AUX-MIB table first */
		   lookupIndexResult = lookup_type(tmpPtr->string,
					allParsesTypeTable);

		   if(!lookupIndexResult) {
		     /* try regular import table */
                     lookupIndexResult = lookup_type(tmpPtr->string,
                                        singleParseTypeTable);
 		   }

                   if(!lookupIndexResult) {

                     /*Index is not defined anywhere*/
                     if(!globalSuppressOutputFlag && 
                             globalVerboseOutputFlag) {
                       fprintf(stderr, "*** NOTE *** element in INDEX clause is undefined: %s\n\n", tmpPtr->string);
                     }

                     /*****************************
                     fprintf(stderr, "object %s: \n",
                                  ptr->child[child_syntax___1]->string);
                     fprintf(stderr, "    element in INDEX clause is undefined: %s\n", tmpPtr->string);
                     postParseErrorFlag = 1;
                     *******************************/

                   }
                 }
 
		 /*name*/
                 tmp_node->name = strdup(tmpPtr->string);

		 /*implied*/
		 if(tmpPtr->type == IMPLIED) {
			tmp_node->implied = 1;
		 }

                 if(tmp_list == NULL){
                      tmp_list = tmp_node;
                      tmp_ptr = tmp_list;
                      tmp_list->next = NULL;
                 } else {
                      tmp_ptr->next = tmp_node;
                      tmp_node->next = NULL;
                      tmp_ptr = tmp_node;
                 }

                 tmpPtr = tmpPtr->next_tree_node;

             }  /* while */

             /* Create index node for last index */
             if ( (tmp_node = (IndexInfo *) malloc (sizeof(IndexInfo))) == NULL) {
                   fprintf(stderr, "Unable to malloc memory for index\n");
                   return NULL;
             }
             memset((void *)tmp_node, '\0', sizeof(IndexInfo));
	
             if(tmpPtr->string){
	       /*Check to see if INDEX is valid*/
 	       if(!tmpPtr->type) {
                 /* if it is an imported type, type is 0, so
                  *  lookup in table
                  */
                 lookupIndexResult = NULL;
	
                 /* look in AUX-MIB table first */
                 lookupIndexResult = lookup_type(tmpPtr->string,
                                        allParsesTypeTable);

		 if(!lookupIndexResult) {
		   /* look in regular table */
                   lookupIndexResult = lookup_type(tmpPtr->string,
                                        singleParseTypeTable);
		 }

                 if(!lookupIndexResult) {

                   /*Index is not defined anywhere*/
                   if(!globalSuppressOutputFlag && 
                           globalVerboseOutputFlag) {
                     fprintf(stderr, "*** NOTE *** element in INDEX clause is undefined: %s\n\n", tmpPtr->string);
                   }

                   /*****************************
                   fprintf(stderr, "object %s: \n",
                                  ptr->child[child_syntax___1]->string);
                   fprintf(stderr, "    element in INDEX clause is undefined: %s\n", tmpPtr->string);
                   postParseErrorFlag = 1;
                   *******************************/

                 }
	       }
               tmp_node->name = strdup(tmpPtr->string);
	       /*implied*/
	       if(tmpPtr->type == IMPLIED) {
		 tmp_node->implied = 1;
	       }

               if(tmp_list == NULL){
                 tmp_list = tmp_node;
                 tmp_ptr = tmp_node;
                 tmp_ptr->next = NULL;
               } else {
                 tmp_ptr->next = tmp_node;
                 tmp_node->next = NULL;
                 tmp_ptr = tmp_node; 
               }
             } else {
                  if(tmp_node) free(tmp_node);
             }
      }
      else if ( ptr->child[child_index____7] &&
        ptr->child[child_index____7]->type == AUGMENTS) {
 
             tmpPtr = ptr->child[child_index____7];
	     if ( (tmp_node = (IndexInfo *) 
			malloc (sizeof(IndexInfo))) == NULL) {
                  fprintf(stderr, "Unable to malloc memory for index\n");
                  return NULL;
             }
             memset((void *)tmp_node, '\0', sizeof(IndexInfo));

             tmp_node->name = strdup(tmpPtr->string);

	     /* there will always be only one AUGMENTS, so no need to 
 	      *	link the list
	      */
             tmp_list = tmp_node;
             tmp_list->next = NULL;
      }

      return(tmp_list);

}

void
print_ll_index(IndexInfo *list, FILE *outputFilePtr)
{
   IndexInfo *tmpPtr = NULL;
 
    if (list == NULL) {
         return;
    }
 
    tmpPtr = list;
 
    if(formatFlag == XML) {
      /*check for IMPLIED*/
      if(tmpPtr->implied){
	fprintf(outputFilePtr, "\"IMPLIED %s", tmpPtr->name);
      } else {
        fprintf(outputFilePtr, "\"%s", tmpPtr->name);
      }
      tmpPtr = tmpPtr->next;

      /* see if there are more indicies to process */
      while (tmpPtr) {
	 if(tmpPtr->implied){
        	fprintf(outputFilePtr, ", IMPLIED %s", tmpPtr->name);
      	 } else {
           fprintf(outputFilePtr, ", %s", tmpPtr->name);
	 }
         tmpPtr = tmpPtr->next;
      }
      fprintf(outputFilePtr, "\"");
    }
    else {  /* IDAT */
      fprintf(outputFilePtr, "{\n");
      while (tmpPtr) {
         fprintf(outputFilePtr, "      %s\n", tmpPtr->name);
         tmpPtr = tmpPtr->next;
      }
      fprintf(outputFilePtr, "}");
    }
 
   return;
 
}

ObjListInfo *
build_objlist_info(tree *ptr)
{

    tree *tmpPtr = NULL;
    ObjListInfo *tmp_list = NULL;
    ObjListInfo *tmp_ptr = NULL;
    ObjListInfo *tmp_node = NULL;

    /* NOTIFICATION-TYPES store object list in [child_object___5] */
    if( ptr->child[child_misc1___14] || ptr->child[child_object___5]) {
 
        /* print name */
        if(ptr->child[child_identity_0] && ptr->child[child_identity_0]->string) {
             
	     if(ptr->type == NOTIFICATION_TYPE) {
		tmpPtr = ptr->child[child_object___5];
	     }
 	     else {
               tmpPtr = ptr->child[child_misc1___14];
 	     }
 
             /* do we have at least one item? */
             if(tmpPtr) {
 
                 /* print opening quote */
                 while(tmpPtr->next_tree_node && tmpPtr->string) {

                   if ((tmp_node = (ObjListInfo *) malloc (sizeof(ObjListInfo))) == NULL) {
                      fprintf(stderr, "Unable to malloc memory for index\n");
                      return NULL;
                   }

                   memset((void *)tmp_node, '\0', sizeof(ObjListInfo));
 
                   tmp_node->name = strdup(tmpPtr->string);

                   if(tmp_list == NULL){
                           tmp_list = tmp_node;
                           tmp_ptr = tmp_list;
                           tmp_list->next = NULL;
                   } else {
                           tmp_ptr->next = tmp_node;
                           tmp_node->next = NULL;
                           tmp_ptr = tmp_node;
                   }

                   tmpPtr = tmpPtr->next_tree_node;

                 }  /* while */

                 if ((tmp_node = (ObjListInfo *) malloc (sizeof(ObjListInfo))) == NULL) {
                    fprintf(stderr, "Unable to malloc memory for index\n");
                    return NULL;
                 }

                 if(tmpPtr->string){
                       tmp_node->name = strdup(tmpPtr->string);

                       if(tmp_list == NULL){
                           tmp_list = tmp_node;
                           tmp_ptr = tmp_node;
                           tmp_ptr->next = NULL;
                       } else {
                           tmp_ptr->next = tmp_node;
                           tmp_node->next = NULL;
                           tmp_ptr = tmp_node; 
                       }
                 }
 
             }   /* if tmpPtr - at least one item */

        }

    }

    return(tmp_list);

}

void
print_ll_objlist(ObjListInfo *list, FILE *outputFilePtr)
{
    ObjListInfo *tmpPtr = NULL;
 
    if (list == NULL) {
         return;
    }
 
    tmpPtr = list;
 
    if(formatFlag == XML) {
      fprintf(outputFilePtr, "\"%s", tmpPtr->name);
      tmpPtr = tmpPtr->next;

      /* see if there are more indicies to process */
      while (tmpPtr) {
         fprintf(outputFilePtr, ", %s", tmpPtr->name);
         tmpPtr = tmpPtr->next;
      }
      fprintf(outputFilePtr, "\"");
    }
    else {  /* IDAT */
      fprintf(outputFilePtr, "{\n");
      while (tmpPtr) {
         fprintf(outputFilePtr, "      %s\n", tmpPtr->name);
         tmpPtr = tmpPtr->next;
      }
      fprintf(outputFilePtr, "}");
    }
 
   return;
 
}

void
cleanup_ll()
{
   ObjData *tmpPtr = NULL;
   ObjData *h = NULL;
   TC_ObjData *tcPtr = NULL;
   TC_ObjData *t = NULL;
   IM_ObjData *imPtr = NULL;
   IM_ObjData *im = NULL;

   int i=0;

   tmpPtr = llroot;

   while (tmpPtr) {

       if(tmpPtr->name){
            free(tmpPtr->name);
       }
       if(tmpPtr->type){
            free(tmpPtr->type);
       }
       if(tmpPtr->oid){
            free(tmpPtr->oid);
       }
       if(tmpPtr->id){
            free(tmpPtr->id);
       }
       if(tmpPtr->access){
            free(tmpPtr->access);
       }
       if(tmpPtr->descr){
            free(tmpPtr->descr);
       }
       if(tmpPtr->moduleid){
            free(tmpPtr->moduleid);
       }
       if(tmpPtr->defval){
	    free(tmpPtr->defval);
       }
       if(tmpPtr->module_comp){
	    cleanup_module_comp(tmpPtr->module_comp);
       }
       if(tmpPtr->index_list){
            cleanup_index(tmpPtr->index_list); 
       }
       if(tmpPtr->enum_list){
            cleanup_enum(tmpPtr->enum_list);
       }
       if(tmpPtr->range[i]){
            free(tmpPtr->range[i]);
	    i++;
	    while(i) {
		if(tmpPtr->range[i]){
		  free(tmpPtr->range[i]);
            	  i++;
		}
		else {
		  i=0;
		}
	    }
       }
       /*BAB*/
       if(tmpPtr->intoid){
	    if(tmpPtr->intoid->oid){
		free(tmpPtr->intoid->oid);
	    }
	    free(tmpPtr->intoid);
       }
       if(tmpPtr->reference){
	    free(tmpPtr->reference);
       }
       if(tmpPtr->status){
	    free(tmpPtr->status);
       }
       if(tmpPtr->component_type){
	    free(tmpPtr->component_type);
       }
       if(tmpPtr->misc){
	    free(tmpPtr->misc);
       }
       if(tmpPtr->primtype){
	    free(tmpPtr->primtype);
       }
       if(tmpPtr->obj_list){
	    cleanup_objlist(tmpPtr->obj_list);
       }
       if(tmpPtr->revision_list){  
	    cleanup_revision_list(tmpPtr->revision_list);
       }

       h = tmpPtr;
       tmpPtr = tmpPtr->next_obj; 
       free(h);

   }

   if(moduleNameBuffer) free(moduleNameBuffer);
   
   llroot = NULL;
   /*sorted_root = NULL;*/

   tcPtr = tc_root;

   while(tcPtr) {
       if(tcPtr->name){
            free(tcPtr->name);
       }
       if(tcPtr->type){
            free(tcPtr->type);
       }
       if(tcPtr->status){
            free(tcPtr->status);
       }
       if(tcPtr->descr){
            free(tcPtr->descr);
       }
       if(tcPtr->reference){
            free(tcPtr->reference);
       }
       if(tcPtr->component_type){
            free(tcPtr->component_type);
       }
       if(tcPtr->misc){
            free(tcPtr->misc);
       }
       if(tcPtr->enum_list){
            cleanup_enum(tcPtr->enum_list);
       }
       if(tcPtr->range[i]){
            free(tcPtr->range[i]);
            i++;
            while(i) {
                if(tcPtr->range[i]){
		  free(tcPtr->range[i]);
                  i++;
                }
                else {
                  i=0;
                }
            }
       }

       t = tcPtr;
       tcPtr = tcPtr->next_obj;
       free(t);

   }

   tc_root = NULL;

   imPtr = im_root;

   while(imPtr) {
     if(imPtr->name){
          free(imPtr->name);
     }
     if(imPtr->type){
          free(imPtr->type);
     }
     if(imPtr->component_type){
	  free(imPtr->component_type);
     }

     im = imPtr;
     imPtr = imPtr->next_obj;
     free(im);
   }

   return;

}

void
cleanup_index(IndexInfo *list)
{
   IndexInfo *indexPtr = NULL;
   IndexInfo *h = NULL;

   if ( list == NULL ) {
      return;
   }

   indexPtr = list;

   while (indexPtr) {

       if(indexPtr->name){
            free(indexPtr->name);
       }

       h = indexPtr;
       indexPtr = indexPtr->next; 
       free(h);

   }
 
   return;
}

void
cleanup_enum(EnumInfo *list)
{
   EnumInfo *enumPtr = NULL;
   EnumInfo *h = NULL;

   if ( list == NULL ) {
      return;
   }

   enumPtr = list;
 
   while (enumPtr) {
 
       if(enumPtr->name){
            free(enumPtr->name);
       }
       if(enumPtr->value){
            free(enumPtr->value);
       }
 
       h = enumPtr;
       enumPtr = enumPtr->next;
       free(h);
 
   }

   return;
}

void
cleanup_objlist(ObjListInfo *list)
{
   ObjListInfo *enumPtr = NULL;
   ObjListInfo *h = NULL;

   if ( list == NULL ) {
      return;
   }

   enumPtr = list;
 
   while (enumPtr) {
 
       if(enumPtr->name){
            free(enumPtr->name);
       }

       h = enumPtr;
       enumPtr = enumPtr->next;
       free(h);
 
   }

   return;
}

void
cleanup_revision_list(RevisionInfo *rev)
{
   RevisionInfo *revPtr = NULL;
   RevisionInfo *r = NULL;

   if ( rev == NULL ) {
      return;
   }

   revPtr = rev;

   while (revPtr) {

       if(revPtr->description){
            free(revPtr->description);
       }
       if(revPtr->date){
            free(revPtr->date);
       }

       r = revPtr;
       revPtr = revPtr->next;
       free(r);

   }

   return;
}

void
cleanup_module_comp(ModuleComp *module_comp)
{
   ModuleComp *modPtr = NULL;
   ModuleComp *h = NULL;

   if ( module_comp == NULL ) {
      return;
   }

   modPtr = module_comp;

   while (modPtr) {

       if(modPtr->name){
            free(modPtr->name);
       }
       if(modPtr->mandatory_groups){
            free(modPtr->mandatory_groups);
       }

       /* cleanup the Conditional Groups */
       cleanup_cond_group(modPtr->cond_group);

       /*cleanup the Conditional Exceptions */
       cleanup_cond_exception(modPtr->cond_exception);

       h = modPtr;
       modPtr = modPtr->next;
       free(h);

   }

   return;
}

void
cleanup_cond_group(CondGroup *cond_group)
{
   CondGroup *groupPtr = NULL;
   CondGroup *h = NULL;

   if ( cond_group == NULL ) {
      return;
   }

   groupPtr = cond_group;

   while (groupPtr) {

       if(groupPtr->name){
            free(groupPtr->name);
       }
       if(groupPtr->descr){
            free(groupPtr->descr);
       }

       h = groupPtr;
       groupPtr = groupPtr->next;
       free(h);

   }

   return;
}

void
cleanup_cond_exception(CondException *cond_exception)
{
   CondException *exptPtr = NULL;
   CondException *h = NULL;
   int i=0;

   if ( cond_exception == NULL ) {
      return;
   }

   exptPtr = cond_exception;

   while (exptPtr) {

       if(exptPtr->name){
            free(exptPtr->name);
       }
       if(exptPtr->min_access){
	    free(exptPtr->min_access);
       }
       if(exptPtr->syntax){
	    free(exptPtr->syntax);
       }
       if(exptPtr->primtype){
	    free(exptPtr->primtype);
       }
       if(exptPtr->write_syntax){
	    free(exptPtr->write_syntax);
       }
       if(exptPtr->primtype_write){
	    free(exptPtr->primtype_write);
       }
       if(exptPtr->descr){
            free(exptPtr->descr);
       }
       if(exptPtr->enum_list){
            cleanup_enum(exptPtr->enum_list);
       }
       if(exptPtr->enum_list_write){
            cleanup_enum(exptPtr->enum_list_write);
       }
       if(exptPtr->range[i]){
            free(exptPtr->range[i]);
            i++;
            while(i) {
                if(exptPtr->range[i]){
                  free(exptPtr->range[i]);
                  i++;
                }
                else {
                  i=0;
                }
            }
       }
       if(exptPtr->range_write[i]){
            free(exptPtr->range_write[i]);
            i++;
            while(i) {
                if(exptPtr->range_write[i]){
                  free(exptPtr->range_write[i]);
                  i++;
                }
                else {
                  i=0;
                }
            }
       }

       h = exptPtr;
       exptPtr = exptPtr->next;
       free(h);

   }

   return;
}


char *
correctoid(char *oid)
{
   char *newoid=NULL;
   int i=0, y=0, len=0;
   char *tok = NULL;
   char *buf = NULL;
 
   len = strlen(oid);

   newoid = (char *) malloc(len+1);

   if(newoid == NULL){
       printf("Malloc failure\n");
       return(NULL);
   }

   buf = strdup(oid);

   if((tok = (char *)strtok(buf, "."))==NULL){
        if(buf) free(buf);
        return(NULL);
   }

   if((strlen(tok)) == 1){
       newoid[i] = tok[0];
       i++;
   } else {
       for(y=(strlen(tok)-1); y>=0; y--){
          newoid[i]=tok[y];
          i++;
       }
   }
   newoid[i]='.';
   i++;
 
   while((tok = (char *)strtok(NULL, "."))!=NULL){
      if((strlen(tok)) == 1){
          newoid[i] = tok[0];
          i++;
 
      } else {
        for(y=(strlen(tok)-1); y>=0; y--){
           newoid[i]=tok[y];
           i++;
        }
      }
      newoid[i] = '.';
      i++;
   }

   newoid[i-1]='\0';

   if(buf) free(buf);
   return(newoid);

}

void
walk_tree(tree *tmpPtr)
{

    tree * typeExistsPtr         = NULL;
 
   /* emit type and all parents back to root */

   if(tmpPtr->string){
       typeExistsPtr = find_type(tmpPtr->string);
   }
 
   if (!typeExistsPtr) {
       return;
   }
 
   while(typeExistsPtr) {
             /* trace def back to root */
 
        if(typeExistsPtr->child[child_identity_0] &&
           typeExistsPtr->child[child_identity_0]->string) {

            if(typeExistsPtr->child[child_identity_0]->next_tree_node &&
               typeExistsPtr->child[child_syntax___1] &&
               typeExistsPtr->child[child_syntax___1]->string ){
 
                fprintf(outputFilePtr, "\n\nnext tree syntax: %s\n", typeExistsPtr->child[child_syntax___1]->string);
                fprintf(outputFilePtr, "\n\n");
 
            }
 
           typeExistsPtr = find_type(typeExistsPtr->child[child_syntax___1]->string);
 
        } else {
           typeExistsPtr = NULL;
        }
    }

  return;

}

RangeInfo *
add_range_info(tree * rangelistPtr )
{
    RangeInfo *new_range = NULL;

    new_range = (RangeInfo *)malloc(sizeof(RangeInfo));

    if(new_range == NULL){
         return NULL;
    }

    if(rangelistPtr->type == DOT_DOT) {
        /* Single range value if end range exisits but start does not */
        if(  rangelistPtr->child[chld_end_range13] &&
             rangelistPtr->child[chld_end_range13]->string &&
           !(rangelistPtr->child[chld_strt_rnge12]) ) {
              new_range->lower = rangelistPtr->child[chld_end_range13]->number;
              new_range->upper = rangelistPtr->child[chld_end_range13]->number;
              return(new_range);
        }

        if(rangelistPtr->child[chld_strt_rnge12] && 
	   rangelistPtr->child[chld_strt_rnge12]->string){
	    if(rangelistPtr->child[chld_strt_rnge12]->type == NEG_NUMBER) {
		/*negative number*/
		new_range->lower = 0 - (rangelistPtr->child[chld_strt_rnge12]->number);
	    }
	    else /*positive*/
              new_range->lower = rangelistPtr->child[chld_strt_rnge12]->number;
         } 
         else {
            if(new_range != NULL){ 
	       free(new_range); 
   	    }
            return NULL;
         }

        if(rangelistPtr->child[chld_end_range13] && rangelistPtr->child[chld_end_range13]->string){
	   if(rangelistPtr->child[chld_end_range13]->type == NEG_NUMBER) {
                /*negative number*/
                new_range->upper = 0 - (rangelistPtr->child[chld_end_range13]->number);
            }   
           else /*positive*/
              new_range->upper = rangelistPtr->child[chld_end_range13]->number;
        }
        else {
           new_range->upper = new_range->lower;
        }
    } 
    else {
        if(new_range != NULL){ 
          free(new_range); 
        }
        return NULL;
    } /* if DOT_DOT */

    return(new_range);
}

IntOIDInfo *
build_int_oid(char * oid)
{
    IntOIDInfo *oidinfo = NULL;
    int i=0;
    int oidlength = 0;
    int *completeoid = NULL;
    char *tok = NULL;
    char *buf = NULL;
    int tmpoid[256];
 
   /* emit type and all parents back to root */
   memset(tmpoid, 0, 256);

   if ( (oidinfo = (IntOIDInfo *) malloc (sizeof(IntOIDInfo))) == NULL) {
      return(NULL);
   }

   memset(oidinfo, 0, sizeof(IntOIDInfo));
   buf = strdup(oid);
 
   if((tok = (char *)strtok(buf, "."))==NULL){
        if(buf) free(buf);
        return(NULL);
   }

   tmpoid[i++] = atoi(tok);
   oidlength++;

   while((tok = (char *)strtok(NULL, "."))!=NULL){
       tmpoid[i++] = atoi(tok);
       oidlength++;
   }

    completeoid = (int *) malloc (oidlength * sizeof(int));

    for (i=0; i< oidlength; i++) {
        completeoid[i] = tmpoid[i];
    }

    oidinfo->oidlen = oidlength;
    oidinfo->oid = completeoid;
 
#ifdef NOT_YET
    printf("\n");
    printf("oid length: %d\n", oidlength);
    printf("OID: ");
    for(i=0; i< oidlength; i++){
       printf("%d ", oidinfo->oid[i]);
    }
    printf("\n\n");
#endif /* NOT_YET */
 
    if(buf) free(buf);
    return(oidinfo);

}

char *
find_prim_type(char *type)
{
   tree *tmpPtr = NULL;
   tree *tmpPtr2 = NULL;
 
   tmpPtr = find_type(type);
   
   if(tmpPtr){
       if(application_type_test(tmpPtr->child[child_syntax___1]->string)){
          return(strdup(tmpPtr->child[child_syntax___1]->string));
       } else {
           tmpPtr2 = find_type(tmpPtr->child[child_syntax___1]->string);
           if(tmpPtr2 &&
              tmpPtr2->child &&
              tmpPtr2->child[child_syntax___1]->string){
              if(application_type_test(tmpPtr2->child[child_syntax___1]->string)){
                return(strdup(tmpPtr->child[child_syntax___1]->string));
              } else {
                return(strdup(tmpPtr2->child[child_syntax___1]->string));
              }
            } else {
               return(strdup(tmpPtr->child[child_syntax___1]->string));
            }
       }
   } else {
       return(strdup(type));
   }
}

int
application_type_test(char *type)
{
    if( (!strcmp(type, "Gauge")) ||
        (!strcmp(type, "Counter")) ||
        (!strcmp(type, "TimeTicks")) ||
        (!strcmp(type, "IpAddress")) ||
        (!strcmp(type, "NetworkAddress")) ||
        (!strcmp(type, "Opaque")) ) {
           return(1);
    }
    return(0);
}

char *
xml_correct_string(char * xml_string, int len)
{
    int pad=0;
    int i, j, matchflag=0; 
    int placeholder=0;
    int lastgood=0;
    char *tmpstring;

    tmpstring = (char *) malloc((len + 100) * sizeof(char));
    if(!tmpstring) {
       fprintf(stderr, "Unable to malloc memory for %s\n", xml_string);
       return NULL;
    }
	

    for(i=0; i< len; i++){
	if(xml_string[i] == '<' || xml_string[i] == '>') {
	  /*found a match. need to substitute*/
	  matchflag = 1;
	  for(j=placeholder; j<(i+pad); j++) {
		tmpstring[j] = xml_string[lastgood];
		lastgood++;
	  }
          if(xml_string[i] == '<') {
		tmpstring[j] = '&';
	        tmpstring[++j] = 'l';
		tmpstring[++j] = 't';
		tmpstring[++j] = ';';
	        placeholder = ++j;
	        lastgood = i + 1;
	        pad = pad + 3;
          }
	  else if(xml_string[i] == '>') {
	        tmpstring[j] = '&';
                tmpstring[++j] = 'g';
                tmpstring[++j] = 't';
                tmpstring[++j] = ';';
                placeholder = ++j;
                lastgood = i + 1;
                pad = pad + 3;
          }
	}
     }
     if(matchflag) { 
	/*finish copying any remaining chars*/
	for(j=placeholder; j<(i+pad); j++) {
          tmpstring[j] = xml_string[lastgood];
          lastgood++;
        }
	tmpstring[j] = '\0';

	/*realloc*/
	xml_string = realloc(xml_string, strlen(tmpstring) + 1);

 	strcpy(xml_string, tmpstring);
	
     }
     if(tmpstring) {
	free(tmpstring);
     }
     return xml_string;
}

int
garbageCheck(char *name)
{
    if(name) {
      if( (!strcmp(name, "SimpleSyntax")) ||
        (!strcmp(name, "ApplicationSyntax")) ||
        (!strcmp(name, "NetworkAddress")) ) { 
           return(1);
      }
    }
    return(0);
}

int
bin_pow(int exponent)
{

    /* This function not only raises 2 to the x power defined by exponent,
     * but it switches the bits so that the least significant becomes the
     * most significant so that it conforms with the SNMP BIT format
     * Eg. 00000010 becomes 01000000
     */

    int i;
    int product=1;

    /* reverses the bit significance */
    exponent = 7 - exponent;

    /*if exponent = 0, just leave product=1*/
    for(i=0; i<exponent; i++) {
	product = product * 2;
    }
    
    return product;
}
