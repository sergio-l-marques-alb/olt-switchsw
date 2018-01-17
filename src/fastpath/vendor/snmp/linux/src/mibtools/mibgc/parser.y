%{
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>                                                         

#include <time.h>                                                             

#include <ctype.h>

#include <unistd.h>

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
char * SEPARATOR = ":";

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

%}

%union {
    int val;
    char * char_ptr;
    struct tree_tag * tree_ptr;
}

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================== TOKENS ========================== */
/*  %right favors shifting                                      */ 
/*  %left favors reduction                                      */ 
/*  precedence is from lowest to highest ( with lowest first )  */ 
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* make tokens look like keywords to enhance error reporting */

%right  ACCESS accessible_for_notify AGENT_CAPABILITIES ANY APPLICATION CCE AUGMENTS BAR
%right  BEGIN_ BINARY_NUMBER BIT BITS BY CHOICE COMMA CONTACT_INFO Counter Counter32 Counter64
%right  CREATION_REQUIRES currrent DEFINED DEFINITIONS DEFVAL deprecated DESCRIPTION DISPLAY_HINT
%right  DOT DOT_DOT END ENTERPRISE EXPLICIT EXPORTS FROM Gauge Gauge32 GROUP HEX_NUMBER IDENTIFIER
%right  IMPLICIT IMPLIED IMPORTS INCLUDES INDEX INTEGER Integer32 IpAddress LAST_UPDATED LBRACE
%right  LBRACKET LOWERCASE_IDENTIFIER LPAREN mandatory MANDATORY_GROUPS MAX_ACCESS MAX MIN
%right  MIN_ACCESS MODULE MODULE_COMPLIANCE MODULE_CONFORMANCE MODULE_IDENTITY NEG_NUMBER
%right  not_accessible NOTIFICATION_GROUP NOTIFICATIONS NOTIFICATION_TYPE not_implemented NULL_
%right  OBJECT OBJECT_GROUP OBJECT_IDENTITY ObjectSyntax OBJECTS OBJECT_TYPE obsolete OCTET
%right  OF OID Opaque optional ORGANIZATION POS_NUMBER PRIVATE PRODUCT_RELEASE QUOTED_STRING RBRACE
%right  RBRACKET read_create read_only read_write REFERENCE REVISION RPAREN SEMICOLON SEQUENCE
%right  SIZE STATUS STRING STRUCT SUPPORTS SYNTAX TEXTUAL_CONVENTION TimeTicks TRAP_TYPE UInteger32 UNION
%right  UNITS UNIVERSAL Unsigned32 UPPERCASE_IDENTIFIER VARIABLES VARIATION write_only
%right  WRITE_SYNTAX

/*BAB***
%right  COMMENT 
**BAB*/

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ==================== PRODUCTION NAMES  ===================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

%type <tree_ptr>    AccessibleForNotify AgentCapabilitiesDefinition AlternativeTypeList GarbageType ApplicationSyntax
%type <tree_ptr>    assignedIdentifier Assignment AssignmentList BitsKeyword BitStringKeyword class Compliance
%type <tree_ptr>    ComplianceAccess complianceAccessPart ComplianceGroup ComplianceList ComplianceObject
%type <tree_ptr>    compliancePart complianceSyntaxPart CounterKeyword Counter32Keyword Counter64Keyword creation creationPart
%type <tree_ptr>    CurrentKeyword DefaultValue defaultValuePart DefinedValue DeprecatedKeyword descriptionPart
%type <tree_ptr>    DescriptionPart displayPart EndPoint empty exports ExtUTCTime GaugeKeyword Gauge32Keyword ImportKeyword importList
%type <tree_ptr>    imports Index indexPart IndexType IndexTypeList Integer32Keyword IntegerKeyword IpAddressKeyword
%type <tree_ptr>    LowercaseIdentifier MandatoryKeyword mandatoryPart MIB mibFile moduleName MIBList Module
%type <tree_ptr>    ModuleBody ModuleComplianceDefinition ModuleConformanceDefinition ModuleDefinition
%type <tree_ptr>    ModuleDefinitionList ModuleIdentityDefinition ModuleList moduleOID modulePart NamedBit NamedBitList
%type <tree_ptr>    NamedBitListPlugin NamedType NotAccessible NotificationGroupDefinition NotificationTypeDefinition
%type <tree_ptr>    NotImplemented Number NumericValue ObjectAccessPart ObjectGroupDefinition
%type <tree_ptr>    ObjectIdentifier ObjectIdentifierKeyword ObjectIdentifierList
%type <tree_ptr>    ObjectIdentifierValue ObjectIdentityDefinition ObjectSyntaxKeyword objectPart ObjectTypeDefinition ObsoleteKeyword
%type <tree_ptr>    OctetStringKeyword Oid OpaqueKeyword OptionalKeyword QuotedString RangeList RangeListPlugin
%type <tree_ptr>    RangeRestriction ReadCreate ReadOnly ReadWrite referencePart Revision RevisionList
%type <tree_ptr>    revisionPart SequenceItem SequenceItemsList SequenceKeyword SequenceSyntax SimpleSyntax SizeKeyword SizeRestriction
%type <tree_ptr>    SMIv1Access SMIv2Access SMIv3Syntax StartBogusTC StartModuleDefinition StartModuleIdentity StartStruct
%type <tree_ptr>    StartUnion Status StatusPart Struct StructItem StructItemList Symbol SymbolFromModule SymbolList
%type <tree_ptr>    symbolsExported SymbolsFromModuleList symbolsImported Syntax SyntaxPart TextualConventionDefinition
%type <tree_ptr>    TimeTicksKeyword TrapTypeDefinition Type TypeAssignment TypeGarbage TypeTag Union unitsPart 
%type <tree_ptr>    Unsigned32Keyword UppercaseIdentifier ValueAssignment ValueRange variablePart Variation
%type <tree_ptr>    VariationAccess variationAccessPart VariationList variationPart VarType VarTypeList VarTypes
%type <tree_ptr>    VirtualTableConstructor WriteOnly writeSyntaxPart

/*BAB***
%type <tree_ptr>    Comment 
**BAB*/

%start  mibFile

%%

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ==================== OVERALL ORGANIZATION ================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/*
 * One mibFile may contain multiple MIB modules.
 * It's also possible that there's no module in a file.
 */
mibFile
    :   ModuleDefinitionList { root = $1; }
    |   empty
    /*BAB*/
    /*|   Comment*/
    /*BAB*/
;

ModuleDefinitionList
    :   ModuleDefinition {
            $1->last_tree_node_in_list = $1;
    }
    |    ModuleDefinitionList ModuleDefinition {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

ModuleDefinition
    :   StartModuleDefinition
        CCE
        BEGIN_ ModuleBody END {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = DEFINITIONS;
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_misc1___14] = $4;
            $$ = tree_node;
    }
    ;

StartModuleDefinition
    :   UppercaseIdentifier 
        assignedIdentifier
        DEFINITIONS {

            /* reset flags for each module/mib */
            SMIv1Flag   = 0;
            SMIv2Flag   = 0;
            SMIv3Flag   = 0;
            sawObjectIdentifier = 0;

            /* set module name */
            lastModuleName = $1->string;
    }
    |   LowercaseIdentifier 
        assignedIdentifier
        DEFINITIONS {

            /* reset flags for each module/mib */
            SMIv1Flag   = 0;
            SMIv2Flag   = 0;
            SMIv3Flag   = 0;

            /* set module name */
            lastModuleName = $1->string;
            print_warning("*** ERROR ***  MIB names must begin with an uppercase letter.", lineBuf);
    }
    ;

assignedIdentifier
    :   ObjectIdentifierValue
    |   empty
    ;

imports
    :   IMPORTS symbolsImported SEMICOLON {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $2;
            $$ = tree_node; 
    }
    |   empty
    ;

symbolsImported
    :   SymbolsFromModuleList {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $1;
            $$ = tree_node;
    }
    |   empty
    ;

SymbolsFromModuleList
    :   SymbolFromModule {
            $1->last_tree_node_in_list = $1;
    }
    |   SymbolsFromModuleList SymbolFromModule {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

/* exports are not used, just parsed and tossed */
ModuleBody
    :   exports imports AssignmentList {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $2;
            tree_node->child[child_misc1___14] = $3;
            $$ = tree_node; 
    }
    |   exports imports {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $2;
            $$ = tree_node; 
    }
    ;

/* EXPORTS are not used, just parsed and tossed */
exports
    :   EXPORTS symbolsExported SEMICOLON {
            $$ = $2;
    }
    |   empty
    ;

symbolsExported
    :   SymbolList 
    |   SymbolList COMMA {
            print_warning("*** ERROR ***  There is an extra COMMA at the end of the Symbol List.", lineBuf);
    } 
    |   empty
    ;

AssignmentList
    :   Assignment {
            $1->last_tree_node_in_list = $1;
    }
    |   AssignmentList Assignment {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

importList
    :   SymbolList
    |   SymbolList COMMA {
            print_warning("*** ERROR ***  There is an extra COMMA at the end of the Symbol List.", lineBuf);

    }
    |   empty
    ;

SymbolList
    :   Symbol {
            $1->last_tree_node_in_list = $1;
    }
    |   SymbolList COMMA Symbol {
            $1->last_tree_node_in_list->next_tree_node = $3;
            $1->last_tree_node_in_list = $3;
    }
    |   SymbolList Symbol {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
            print_warning("*** ERROR ***  There is a missing COMMA in the Symbol List.", lineBuf);
    }
    ;

SymbolFromModule
    :   importList FROM UppercaseIdentifier {
            tree * importFromNode = NULL;
            tree * importItem     = NULL;
            tree * treePtr        = NULL;
            tree * tree_node      = NULL;

            tree_node = malloc_node(tree_node, SINGLE_PARSE);

	    /*BAB*/
	    tree_node->type = IMPORTS;
	    tree_node->string = "imports";
	    /*BAB*/

            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_misc1___14] = $3;

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

            $$ = tree_node; 

            /* create and insert FROM node at beginning of global import list */
            importFromNode = malloc_node(importFromNode, 1);
            importFromNode->string = strdup($3->string);
            importFromNode->freeMe = 1;

            treePtr = $1;
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
    |   importList FROM LowercaseIdentifier {
            tree * importFromNode = NULL;
            tree * importItem     = NULL;
            tree * treePtr        = NULL;
            tree * tree_node      = NULL;

            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_misc1___14] = $3;
            $$ = tree_node; 

            /* create and insert FROM node at beginning of global import list */
            importFromNode = malloc_node(importFromNode, 1);
            importFromNode->string = strdup($3->string);
            importFromNode->freeMe = 1;

            print_warning("*** ERROR ***  The names of imported MIBs must be UppercaseIdentifiers.", lineBuf);

            treePtr = $1;
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
    ;

Symbol
    :   UppercaseIdentifier
    |   LowercaseIdentifier
    |   ImportKeyword
    ;

/* -1 for type we want to ignore */
ImportKeyword
    :   ApplicationSyntax {
            $1->type = -1;
    }

    |   GaugeKeyword {
            $1->type = -1;
    }
    |   CounterKeyword {
            $1->type = -1;
    }
    |   IntegerKeyword {
            $1->type = -1;
    }
    |   BitsKeyword {
            $1->type = -1;
    }
    |   AGENT_CAPABILITIES { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   NOTIFICATION_GROUP {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   NOTIFICATION_TYPE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   TRAP_TYPE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   MANDATORY_GROUPS {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   MODULE_COMPLIANCE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   MODULE_CONFORMANCE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node; 
    }
    |   MODULE_IDENTITY {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   OBJECT_GROUP {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   OBJECT_IDENTITY {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   OBJECT_TYPE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    |   TEXTUAL_CONVENTION {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = -1;
            $$ = tree_node;
    }
    ;

TypeAssignment
    :   UppercaseIdentifier CCE Type {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_syntax___1] = $3;
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

            $$ = tree_node;
    }
    |   LowercaseIdentifier CCE Type {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_syntax___1] = $3;

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

            $$ = tree_node;

            print_warning("*** ERROR ***  You cannot use LowercaseIdentifiers in type assignments.", lineBuf);

    }
        /* garbage collector */
    |   ImportKeyword CCE Type {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;

            print_note("*** NOTE ***  You cannot redefine basic types.", lineBuf);

    }
    ;    

Assignment
    :   AgentCapabilitiesDefinition
    |   ModuleComplianceDefinition
    |   ModuleConformanceDefinition
    |   ModuleIdentityDefinition
    |   NotificationGroupDefinition
    |   NotificationTypeDefinition
    |   ObjectGroupDefinition
    |   ObjectIdentityDefinition
    |   ObjectTypeDefinition
    |   TextualConventionDefinition
    |   TrapTypeDefinition
    |   TypeAssignment
    |   ValueAssignment

        /* SMIv3 extensions */
    |   Struct
    |   Union
    /*BAB*/
    /*|   Comment*/
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ============== GRAY AREA - GRAMMAR ILL-DEFINED ============= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ValueAssignment
    :   LowercaseIdentifier 
        OBJECT IDENTIFIER 
        CCE ObjectIdentifierValue  {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = IDENTIFIER;
            tree_node->string                  = "object-id";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_value____9] = $5;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            $$ = tree_node;

            sawObjectIdentifier = 1;

    }
    |   UppercaseIdentifier 
        OBJECT IDENTIFIER 
        CCE ObjectIdentifierValue  {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = IDENTIFIER;
            tree_node->string                  = "object-id";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_value____9] = $5;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            $$ = tree_node;

            sawObjectIdentifier = 1;

            print_warning("*** ERROR ***  Object identifiers must begin with a lowercase letter.", lineBuf);

    }
    ;

ObjectIdentifierValue
    :   LowercaseIdentifier
    |   UppercaseIdentifier {
        print_warning("*** ERROR ***  Object identifiers must begin with a lowercase letter.", lineBuf);
    }
    |   LBRACE ObjectIdentifierList RBRACE {
            $$ = $2;
    }
    ;

ObjectIdentifierList
    :   ObjectIdentifier {
            $1->last_tree_node_in_list = $1;
    }
    |   ObjectIdentifierList ObjectIdentifier {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
	    tree_node->child[0] = $2;
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

Index
    :   LowercaseIdentifier
    |   UppercaseIdentifier
    |   ApplicationSyntax
    |   IntegerKeyword
    |   OctetStringKeyword
    |   ObjectIdentifierKeyword
    ;

/* may also be a textual convention (or its refinement)*/
Syntax
    :   Type
    ;

Type
    :   ApplicationSyntax

    |   Integer32Keyword  RangeRestriction { 
            $1->child[child_range____6] = $2; 
            fill_in_max_min_values($2, INTEGER);
            check_range_limits($2, INTEGER);
            check_for_range_overlap($2);
    }
    |   Gauge32Keyword    RangeRestriction { 
            $1->child[child_range____6] = $2;
            fill_in_max_min_values($2, Unsigned32);
            check_range_limits($2, Unsigned32);
            check_for_range_overlap($2);
    }
    |   Unsigned32Keyword RangeRestriction { 
            $1->child[child_range____6] = $2; 
            fill_in_max_min_values($2, Unsigned32);
            check_range_limits($2, Unsigned32);
            check_for_range_overlap($2);
    }
    |   OpaqueKeyword     SizeRestriction  { $1->child[child_range____6] = $2; }
    |   SimpleSyntax
    |   VirtualTableConstructor
    |   SequenceKeyword LBRACE SequenceItemsList RBRACE {
            $1->child[child_identity_0] = $3;
    }

    |   SMIv3Syntax

    /* *** GARBAGE COLLECTORS *** */

    |   TimeTicksKeyword  RangeRestriction { 
            print_warning("*** ERROR ***  Per rfc2578, TimeTicks must not have range restriction.", lineBuf);
    }
    |   Counter64Keyword  RangeRestriction { 
            print_warning("*** ERROR ***  Per rfc2578, Counter64 must not have range restriction.", lineBuf);
    }
    |   Counter32Keyword  RangeRestriction { 
            print_warning("*** ERROR ***  Per rfc2578, Counter32 must not have range restriction.", lineBuf);
    }
    |   SequenceKeyword LBRACE SequenceItemsList COMMA RBRACE {
            $1->child[child_identity_0] = $3;
            print_warning("*** ERROR ***  There is an extra COMMA at the end of the Sequence Item List.", lineBuf);
    }
    ;

DefinedValue
    :   UppercaseIdentifier
    |   LowercaseIdentifier
    |   UppercaseIdentifier DOT UppercaseIdentifier
    ;

ApplicationSyntax
    :   Integer32Keyword
    |   Gauge32Keyword
    |   Unsigned32Keyword
    |   TimeTicksKeyword
    |   ObjectSyntaxKeyword
    |   IpAddressKeyword
    |   Counter32Keyword
    |   OpaqueKeyword
    |   Counter64Keyword
    ;

SimpleSyntax
    :   ObjectIdentifierKeyword
    |   ObjectIdentifierKeyword SizeRestriction {
            $1->child[child_range____6] = $2;
    }

    |   IntegerKeyword
    |   IntegerKeyword RangeRestriction {
            $1->child[child_range____6] = $2;
            fill_in_max_min_values($2, INTEGER);
            check_range_limits($2, INTEGER);
            check_for_range_overlap($2);
    }
    |   IntegerKeyword LBRACE  NamedBitListPlugin RBRACE  {
            $1->type                    = BITS;
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }

    |   OctetStringKeyword {
            print_note("*** NOTE ***  OCTET STRING on previous line should have SIZE specification.", lineBuf);
    }
    |   OctetStringKeyword SizeRestriction  {
		$1->child[child_range____6] = $2;
    }
    |   OctetStringKeyword LBRACE NamedBitListPlugin RBRACE {
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }        

    |   GaugeKeyword
    |   GaugeKeyword RangeRestriction {
            $1->child[child_range____6] = $2;
            fill_in_max_min_values($2, Unsigned32);
            check_range_limits($2, Unsigned32);
            check_for_range_overlap($2);
    }

    |   CounterKeyword


    |   BitStringKeyword
    |   BitStringKeyword LBRACE NamedBitListPlugin RBRACE  {
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }

    |   BitsKeyword LBRACE NamedBitListPlugin RBRACE  { 
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }

    |   UppercaseIdentifier             /* e.g. the uppercase name of a row or an imported type */
    |   LowercaseIdentifier {
            print_warning("*** ERROR ***  UppercaseIdentifiers must be used for types, row names, or imported types, NOT LowercaseIdentifiers.", lineBuf);
    }
    |   UppercaseIdentifier LBRACE NamedBitListPlugin RBRACE  {
            $1->type                    = BITS;
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }

    |   UppercaseIdentifier RangeRestriction { $1->child[child_range____6] = $2; }
    |   UppercaseIdentifier SizeRestriction  { $1->child[child_range____6] = $2; }
    |   UppercaseIdentifier DOT UppercaseIdentifier LBRACE NamedBitListPlugin RBRACE  {

            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->enumeratedItems         = $5->enumeratedItems;
            $5->enumeratedItems                = NULL;
            tree_node->type                    = BITS;
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_misc1___14] = $3;
            tree_node->child[child_status___2] = $5;
            $$ = tree_node; 
    }
    |   UppercaseIdentifier DOT UppercaseIdentifier RangeRestriction {

            $1->child[child_identity_0] = $3;
            $1->child[child_range____6] = $4;
            $1->type = DOT;
    }
    |    UppercaseIdentifier DOT UppercaseIdentifier SizeRestriction {

            $1->child[child_identity_0] = $3;
            $1->child[child_range____6] = $4;
            $1->type = DOT;
    }

    |    NULL_ { /* rfc1065, rfc1155 */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node; 
    }

    /* *** GARBAGE COLLECTORS *** */

    |   IntegerKeyword SizeRestriction { 
            /* this isn't really legal, should be range restriction */
            print_warning("*** ERROR ***  This should be a range restriction, NOT a size restriction.", lineBuf);
            $1->child[child_range____6] = $2;
    }
    |   OctetStringKeyword RangeRestriction { 
            $1->child[child_range____6] = $2; 
            print_note("*** NOTE ***  OCTET STRING should have SIZE specification.", lineBuf);
    }
    |   TypeGarbage
    ;

/* FUTURE CHECK **** must be valid for the type specified in SYNTAX clause of same OBJECT-TYPE macro */
DefaultValue
    :   Number
    |   LowercaseIdentifier
    |   UppercaseIdentifier
    |   QuotedString
    |   Oid
    |   LBRACE RBRACE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "";
            $$ = tree_node;
    }

    |   LBRACE Number              RBRACE { $$ = $2; }
    |   LBRACE NamedBit            RBRACE { $$ = $2; }
    |   LBRACE LowercaseIdentifier RBRACE { $$ = $2; }
    |   LBRACE Oid                 RBRACE { $$ = $2; }

    |   LBRACE LowercaseIdentifier COMMA VarTypeList RBRACE {
            $2->last_tree_node_in_list = $4->last_tree_node_in_list;
            $2->next_tree_node = $4;
            $$ = $2;
    }

    |   LBRACE ObjectIdentifier ObjectIdentifierList RBRACE {
            $2->last_tree_node_in_list = $3->last_tree_node_in_list;
            $2->next_tree_node = $3;
            $$ = $2;
    }
    ;

SequenceItemsList
    :   SequenceItem {
            $1->last_tree_node_in_list = $1; 
    }
    |   SequenceItemsList COMMA SequenceItem {
            $1->last_tree_node_in_list->next_tree_node = $3;
            $1->last_tree_node_in_list = $3;
    }
    |   SequenceItemsList SequenceItem {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
            print_warning("*** ERROR ***  Missing COMMA in Sequence Item List.", lineBuf);
    }
    ;

SequenceItem
    :   LowercaseIdentifier SequenceSyntax

    |   LowercaseIdentifier TypeTag SequenceSyntax /* garbage collector */

    |   LowercaseIdentifier VirtualTableConstructor {
            if(!SMIv3Flag) {
                SMIv3Flag = 1;
                print_note("*** NOTE ***  VirtualTableConstructors inside a sequence (SEQUENCE OF) are not used in SMIv2 or SMIv2, setting the SMIv3 flag.", lineBuf);
            }
    }
    |   LowercaseIdentifier SMIv3Syntax {
            $2->child[child_identity_0] = $1;
            $$ = $2;
    }

    |   UppercaseIdentifier SequenceSyntax {
            print_warning("*** ERROR ***  Object identifiers must begin with a lowercase letter.", lineBuf);
    }
    |   UppercaseIdentifier TypeTag SequenceSyntax {
            print_warning("*** ERROR ***  Object identifiers must begin with a lowercase letter.", lineBuf);
    }

    |   UppercaseIdentifier VirtualTableConstructor {
            if(!SMIv3Flag) {
                SMIv3Flag = 1;
                print_note("*** NOTE ***  VirtualTableConstructors inside a sequence (SEQUENCE OF) are not used in SMIv2 or SMIv2, setting the SMIv3 flag.", lineBuf);
            }
            print_warning("*** ERROR ***  Object identifiers must begin with a lowercase letter.", lineBuf);

    }
    |   UppercaseIdentifier SMIv3Syntax {
            $2->child[child_identity_0] = $1;
            $$ = $2;
            print_warning("*** ERROR ***  Object identifiers must begin with a lowercase letter.", lineBuf);
    }
    ;

/* SEQUENCE is used to generate lists or tables */
SequenceSyntax
    :   ObjectIdentifierKeyword
    |   ObjectIdentifierKeyword SizeRestriction {
            $1->child[child_range____6] = $2;
    }

    |   GaugeKeyword
    |   CounterKeyword

    |   BitStringKeyword
    |   BitStringKeyword LBRACE NamedBitListPlugin RBRACE  {
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }                

    |   IntegerKeyword
    |   IntegerKeyword RangeRestriction { 
            $1->child[child_range____6] = $2;
            fill_in_max_min_values($2, INTEGER);
            check_range_limits($2, INTEGER);
            check_for_range_overlap($2);
    }      
    |   IntegerKeyword SizeRestriction  { 
            /* this isn't really legal, should be range restriction */
            $1->child[child_range____6] = $2;
            print_warning("*** ERROR ***  This should be a range restriction, NOT a size restriction.", lineBuf);
    }
    |   IntegerKeyword LBRACE NamedBitListPlugin RBRACE  {
            $1->type                    = BITS;
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }                

    |   OctetStringKeyword
    |   OctetStringKeyword SizeRestriction {
            $1->child[child_range____6] = $2;
    }        
    |   OctetStringKeyword RangeRestriction {
            $1->child[child_range____6] = $2;
            print_note("*** NOTE ***  OCTET STRING should have SIZE specification.", lineBuf);
    }        
    |   OctetStringKeyword LBRACE NamedBitListPlugin RBRACE {
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }        

    |   UppercaseIdentifier
    |   LowercaseIdentifier {
            print_warning("*** ERROR ***  LowercaseIdentifiers cannot be used for SequenceSyntax.", lineBuf);
    }
    |   UppercaseIdentifier RangeRestriction {
            $1->child[child_range____6] = $2;
    } 
    |   UppercaseIdentifier SizeRestriction {
            $1->child[child_range____6] = $2;
            $1->type = SIZE;
    } 
    |   UppercaseIdentifier LBRACE NamedBitListPlugin RBRACE {
            $1->type                    = BITS;
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    } 

    |   BitsKeyword

    |   ApplicationSyntax
    |   Integer32Keyword  RangeRestriction    { 
            $1->child[child_range____6] = $2;
            fill_in_max_min_values($2, INTEGER);
            check_range_limits($2, INTEGER);
            check_for_range_overlap($2);
    }
    |   Gauge32Keyword    RangeRestriction    { 
            $1->child[child_range____6] = $2;
            fill_in_max_min_values($2, Unsigned32);
            check_range_limits($2, Unsigned32);
            check_for_range_overlap($2);
    }
    |   Unsigned32Keyword RangeRestriction    { 
            $1->child[child_range____6] = $2; 
            fill_in_max_min_values($2, Unsigned32);
            check_range_limits($2, Unsigned32);
            check_for_range_overlap($2);
    }

    |   Integer32Keyword LBRACE NamedBitListPlugin RBRACE  {
            $1->type                    = BITS;
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }           
    |   Gauge32Keyword LBRACE  NamedBitListPlugin RBRACE  {                /* 0 - 4294967295         */
            $1->type                    = BITS;
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }
    |   Unsigned32Keyword LBRACE NamedBitListPlugin RBRACE  {          /* 0 - 4294967295         */
            $1->type                    = BITS;
            $1->enumeratedItems         = $3->enumeratedItems;
            $3->enumeratedItems         = NULL;
            $1->child[child_status___2] = $3;
    }

        /* garbage collector */
    |   GarbageType

    ;

ObjectIdentifier
    :   Number
    |   LowercaseIdentifier
    |   UppercaseIdentifier {
            print_warning("*** ERROR ***  Object identifiers must begin with a lowercase letter.", lineBuf);
    }
    |   NamedBit
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================= TRAP TYPE MACRO ==================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

TrapTypeDefinition
    :   LowercaseIdentifier TRAP_TYPE ENTERPRISE ObjectIdentifierValue 
        variablePart 
        descriptionPart
        referencePart 
        CCE Number {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = TRAP_TYPE;
            tree_node->child[child_identity_0] = $1;

            tree_node->child[child_object___5] = $9;
            tree_node->child[child_value____9] = $4;

            tree_node->child[child_descript_3] = $6;
            tree_node->child[child_misc1___14] = $5;

            $6->parent = tree_node;

            $$ = tree_node;

            if(!$6->type) {
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
    |   UppercaseIdentifier TRAP_TYPE ENTERPRISE ObjectIdentifierValue 
        variablePart 
        descriptionPart
        referencePart 
        CCE Number {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = TRAP_TYPE;
            tree_node->child[child_identity_0] = $1;

            tree_node->child[child_object___5] = $9;
            tree_node->child[child_value____9] = $4;

            tree_node->child[child_descript_3] = $6;
            tree_node->child[child_misc1___14] = $5;

            $6->parent = tree_node;

            $$ = tree_node;
 
            print_warning("*** ERROR ***  You must use a LowercaseIdentifier to name a trap type.", lineBuf);

            if(!$6->type) {
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
    ;

variablePart
    :   VARIABLES LBRACE VarTypeList RBRACE { 
            $$ = $3;
    }
    |   empty
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================= MODULE CONFORMANCE MACRO ================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ModuleConformanceDefinition
    :   LowercaseIdentifier MODULE_CONFORMANCE
        LAST_UPDATED        ExtUTCTime
        PRODUCT_RELEASE     QuotedString
        DescriptionPart
        modulePart
        CCE  ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = MODULE_CONFORMANCE;
            tree_node->string                  = "module-conformance";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_descript_3] = $7;
            tree_node->child[child_value____9] = $10;

            tree_node->child[child_misc1___14] = $8;

            $7->parent = tree_node;

            $$ = tree_node; 
    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================== MODULE COMPLIANCE MACRO ================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ModuleComplianceDefinition
    :   LowercaseIdentifier MODULE_COMPLIANCE
        StatusPart
        DescriptionPart
        referencePart 
        MIBList 
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = MODULE_COMPLIANCE;
            tree_node->string                  = "module-compliance";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $3;
            tree_node->child[child_descript_3] = $4;
            tree_node->child[child_value____9] = $8;

            tree_node->child[child_misc1___14] = $6;

            $4->parent = tree_node;

            $$ = tree_node; 

	    /*BAB*/
	    /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

    }
    |   UppercaseIdentifier MODULE_COMPLIANCE
        StatusPart
        DescriptionPart
        referencePart 
        MIBList 
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = MODULE_COMPLIANCE;
            tree_node->string                  = "module-compliance";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $3;
            tree_node->child[child_descript_3] = $4;
            tree_node->child[child_value____9] = $8;

            tree_node->child[child_misc1___14] = $6;

            $4->parent = tree_node;

            $$ = tree_node;

	    /*BAB*/
	    /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            print_warning("*** ERROR ***  \"MODULE-COMPLIANCE\" names must begin with a lowercase letter.", lineBuf);
 
    }
    ;

MIBList
    :   MIB { 
            $1->last_tree_node_in_list = $1; 
    }
    |   MIBList MIB {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

MIB
    :   MODULE moduleName
        mandatoryPart 
        compliancePart {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = $2;
            tree_node->child[child_misc1___14] = $3;
            tree_node->child[child_misc2___15] = $4;

            $$ = tree_node; 
    }
    ;

mandatoryPart
    :   MANDATORY_GROUPS LBRACE VarTypeList RBRACE {
            $$ = $3;
    }
    |   empty
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================== AGENT CAPABILITIES MACRO ================ */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

AgentCapabilitiesDefinition
    :   LowercaseIdentifier AGENT_CAPABILITIES 
        PRODUCT_RELEASE QuotedString 
        StatusPart
        DescriptionPart
        referencePart 
        modulePart 
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = AGENT_CAPABILITIES;
            tree_node->string                  = "agent-capabilities";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $5;
            tree_node->child[child_descript_3] = $6;
            tree_node->child[child_misc1___14] = $8;
            tree_node->child[child_value____9] = $10;

            $6->parent = tree_node;

            $$ = tree_node; 

    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================== NOTIFICATION GROUP MACRO ================ */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

NotificationGroupDefinition
    :   LowercaseIdentifier NOTIFICATION_GROUP 
        NOTIFICATIONS LBRACE VarTypeList RBRACE
        StatusPart
        DescriptionPart
        referencePart 
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = NOTIFICATION_GROUP;
            tree_node->string                  = "notification-group";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $7;
            tree_node->child[child_descript_3] = $8;
	    tree_node->child[child_referenc_4] = $9;
            tree_node->child[child_value____9] = $11;

            tree_node->child[child_misc1___14] = $5;

            $8->parent = tree_node;

            $$ = tree_node; 

            /*BAB*/
            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }
            /*BAB*/

    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ===================== OBJECT GROUP MACRO =================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ObjectGroupDefinition
    :   LowercaseIdentifier OBJECT_GROUP 
        OBJECTS LBRACE VarTypeList RBRACE         
        StatusPart
        DescriptionPart
        referencePart 
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = OBJECT_GROUP;
            tree_node->string                  = "object-group";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $7;
            tree_node->child[child_descript_3] = $8;
	    tree_node->child[child_referenc_4] = $9;
            tree_node->child[child_value____9] = $11;

            tree_node->child[child_misc1___14] = $5;

            $8->parent = tree_node;

            $$ = tree_node; 

            /*BAB*/
            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }
            /*BAB*/

    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================== NOTIFICATION TYPE MACRO ================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

NotificationTypeDefinition
    :   LowercaseIdentifier NOTIFICATION_TYPE 
        objectPart
        StatusPart
        DescriptionPart
        referencePart 
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = NOTIFICATION_TYPE;
            tree_node->string                  = "notification";

            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_object___5] = $3;
            tree_node->child[child_status___2] = $4;
            tree_node->child[child_descript_3] = $5;
            tree_node->child[child_referenc_4] = $6;
            tree_node->child[child_value____9] = $8;

            $5->parent = tree_node;

            $$ = tree_node; 

	    /*BAB*/
            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }
	    /*BAB*/

    }
    ;

objectPart
    :   OBJECTS LBRACE VarTypeList RBRACE {
            $$ = $3;
    }
    |   empty
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ====================== OBJECT TYPE MACRO =================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ObjectTypeDefinition
    :   LowercaseIdentifier OBJECT_TYPE 
        SyntaxPart
        unitsPart 
        ObjectAccessPart
        StatusPart
        descriptionPart
        referencePart 
        indexPart 
        defaultValuePart
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = OBJECT_TYPE;

            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $6;
            tree_node->child[child_descript_3] = $7;
	    /*BAB add reference*/
	    tree_node->child[child_referenc_4] = $8;
            tree_node->child[child_index____7] = $9;
            tree_node->child[child_value____9] = $12;
            tree_node->child[child_default_10] = $10;
            tree_node->child[child_access__11] = $5;
            tree_node->child[child_syntax___1] = $3;

#ifdef NOT_YET
            /*BAB - test to see what is available*/
            tree_node->child[child_identity_0] = $1;
            /*tree_node->child[child_syntax___1] = $2;*/
            tree_node->child[child_status___2] = $3;
            tree_node->child[child_descript_3] = $4;
            tree_node->child[child_referenc_4] = $5;
            tree_node->child[child_object___5] = $6;
            tree_node->child[child_range____6] = $7;
            tree_node->child[child_index____7] = $8;
            tree_node->child[child_display__8] = $9;
            tree_node->child[child_value____9] = $10;
            /*tree_node->child[child_default_10] = $11;*/
            tree_node->child[child_access__11] = $12;
            /*tree_node->child[chld_strt_rnge12] = $13;*/
            /*tree_node->child[chld_end_range13] = $14;*/
            /*tree_node->child[child_misc1___14] = $15;*/
            /*tree_node->child[child_misc2___15] = $16;*/
            /*BAB*/
#endif /* NOT_YET */
	

            $7->parent  = tree_node;
            $10->parent = tree_node;

            $$ = tree_node; 

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            if($10->child[child_identity_0]) {
                if( ($3->type == Counter32) || ($3->type == Counter64) ) {
                    print_warning("*** ERROR ***  DEFVAL is not allowed with Counter32 or Counter64.", lineBuf);
                    tree_node->child[child_default_10] = NULL;
                }
            }

            if(!$7->type) {
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
    |   UppercaseIdentifier OBJECT_TYPE 
        SyntaxPart
        unitsPart 
        ObjectAccessPart
        StatusPart
        descriptionPart
        referencePart 
        indexPart 
        defaultValuePart
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type                    = OBJECT_TYPE;
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $6;
            tree_node->child[child_descript_3] = $7;
            tree_node->child[child_index____7] = $9;
            tree_node->child[child_value____9] = $12;
            tree_node->child[child_default_10] = $10;
            tree_node->child[child_access__11] = $5;
            tree_node->child[child_syntax___1] = $3;

            $7->parent  = tree_node;
            $10->parent = tree_node;

            $$ = tree_node; 

            /* *** SAVE TYPE *** */

            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            /* *** SEMANTIC CHECKS *** */

            print_warning("*** ERROR ***  Object types must begin with a lowercase letter.", lineBuf);
	
            if($10->child[child_identity_0]) {
                if( ($3->type == Counter32) || ($3->type == Counter64) ) {
                    print_warning("*** ERROR ***  DEFVAL is not allowed with Counter32 or Counter64.", lineBuf);
                    tree_node->child[child_default_10] = NULL;
                }
            }

            if(!$7->type) {
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
    ;

ObjectAccessPart
    :   MAX_ACCESS  SMIv2Access {
            $$ = $2;

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
    |   ACCESS SMIv1Access{
            $$ = $2;

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
    ;

SMIv1Access
    :   NotAccessible
    |   ReadOnly
    |   ReadWrite
    |   WriteOnly

    |   AccessibleForNotify {
             print_warning("*** ERROR *** \"accessible-for-notify\" should only be used in an SMIv2 MIB.", lineBuf);
    }
    |   ReadCreate {
             print_warning("*** ERROR *** \"read-create\" should only be used in an SMIv2 MIB.", lineBuf);
    }
    ;

unitsPart
    :   UNITS QuotedString { 
            $$ = $2;
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
    |   empty {
    }
    ;

indexPart
    :   INDEX LBRACE IndexTypeList RBRACE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = INDEX;

            tree_node->child[child_identity_0] = $3;

            $$ = tree_node;
    }
    |   INDEX LBRACE IndexTypeList COMMA RBRACE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = INDEX;

            tree_node->child[child_identity_0] = $3;

            $$ = tree_node;

            print_warning("*** ERROR ***  There is an extra comma at the end of the index list.", lineBuf);
    }

    |   AUGMENTS LBRACE LowercaseIdentifier RBRACE {
            $3->type = AUGMENTS;
            $$ = $3;
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
    |   empty
    ;

IndexTypeList
    :   IndexType {
            $1->last_tree_node_in_list = $1; 
    }
    |   IndexTypeList COMMA IndexType {
            $1->last_tree_node_in_list->next_tree_node = $3;
            $1->last_tree_node_in_list = $3;
    }
    |   IndexTypeList IndexType {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
            print_warning("*** ERROR ***  There is a missing COMMA in the Index List.", lineBuf);
    }
    ;

IndexType
    :   IMPLIED Index { 
            $2->type = IMPLIED;
            $$ = $2;
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
    |   Index
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ======================= SMIv3 EXTENSIONS =================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* struct may have only one item */
Struct
    :   StartStruct
        MAX_ACCESS  SMIv2Access
        StatusPart
        descriptionPart
        LBRACE        
        StructItemList
        RBRACE
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = STRUCT;
            tree_node->string                  = "Struct";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $4;
            tree_node->child[child_descript_3] = $5;
            tree_node->child[child_value____9] = $10;
            tree_node->child[child_access__11] = $3;

            tree_node->child[child_misc1___14] = $7;

            $5->parent = tree_node;

            $$ = tree_node;

            if(!$5->type) {
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
    ;

StartStruct
    :   LowercaseIdentifier STRUCT {
            if(!SMIv3Flag) {
                SMIv3Flag = 1;
                print_note("*** NOTE ***  STRUCT is not used in SMIv2 or SMIv2, setting the SMIv3 flag.", lineBuf);
            }   
    }
    ;

StartUnion
    :   LowercaseIdentifier UNION {
            if(!SMIv3Flag) {
                SMIv3Flag = 1;
                print_note("*** NOTE ***  UNION is not used in SMIv2 or SMIv2, setting the SMIv3 flag.", lineBuf);
            }
    }
    ; 

/* union must have at least two items */
Union
    :   StartUnion 
        MAX_ACCESS  SMIv2Access
        StatusPart
        descriptionPart
        LBRACE        
        StructItem
        StructItemList
        RBRACE
        CCE ObjectIdentifierValue {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = UNION;
            tree_node->string                  = "Union";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $4;
            tree_node->child[child_descript_3] = $5;
            tree_node->child[child_value____9] = $11;
            tree_node->child[child_access__11] = $3;

            tree_node->child[child_misc1___14] = $7;
            tree_node->child[child_misc2___15] = $8;

            $5->parent = tree_node;

            $$ = tree_node;

            if(!$5->type) {
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
    ;

StructItemList
    :   StructItem {
            $1->last_tree_node_in_list = $1;
    }
    |   StructItemList StructItem {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;
    
StructItem
    :   ObjectTypeDefinition
    |   Struct
    |   Union
    ;

SMIv3Syntax
    :   STRUCT {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    |   UNION {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ================= TEXTUAL CONVENTION MACRO ================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

TextualConventionDefinition
    :   UppercaseIdentifier 
        CCE TEXTUAL_CONVENTION 
        displayPart 
        StatusPart
        DescriptionPart
        referencePart 
        SyntaxPart {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = TEXTUAL_CONVENTION;
	    tree_node->string = strdup("TEXTUAL_CONVENTION");
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_display__8] = $4;
            tree_node->child[child_status___2] = $5;
            tree_node->child[child_descript_3] = $6;
	    tree_node->child[child_referenc_4] = $7;
            tree_node->child[child_syntax___1] = $8;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            $$ = tree_node; 
    }
    |   StartBogusTC
        displayPart 
        StatusPart
        DescriptionPart
        referencePart 
        SyntaxPart {

            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;

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
    ;

StartBogusTC
    :   ImportKeyword CCE TEXTUAL_CONVENTION { 
            print_note("*** NOTE ***  Ignoring re-definition of pre-defined type.", lineBuf);
    }
    ;

displayPart
    :   DISPLAY_HINT QuotedString { 
            $$ = $2; 
    }
    |   empty
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ============== MODULE IDENTITY MACRO ======================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ModuleIdentityDefinition
    :   StartModuleIdentity 
        LAST_UPDATED ExtUTCTime
        ORGANIZATION QuotedString 
        CONTACT_INFO QuotedString 
        DescriptionPart
        revisionPart 
        CCE ObjectIdentifierValue {
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
            tree_node->child[child_identity_0] = $1;
	    tree_node->child[child_misc1___14] = $3;
	    tree_node->child[child_status___2] = $5;
	    tree_node->child[child_access__11] = $7;
	    tree_node->child[child_descript_3] = $8;
	    tree_node->child[child_referenc_4] = $9;
	    tree_node->child[child_value____9] = $11;
	    /*BAB*/

            $8->parent = tree_node;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            $$ = tree_node; 

    }
    ;

StartModuleIdentity
    :   LowercaseIdentifier MODULE_IDENTITY {

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
    |   UppercaseIdentifier MODULE_IDENTITY {

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
    ;

revisionPart
    :   RevisionList
    |   empty
    ;

RevisionList
    :   Revision { 
            $1->last_tree_node_in_list = $1; 
    }
    |   RevisionList Revision {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

Revision
    :   REVISION ExtUTCTime DescriptionPart {
            /*$$ = $3;*/

	    /* BAB - Ray did not save the Revision date. Modified to 
	     * add both date and description
	     */
	    tree * tree_node = NULL;
	    tree_node = malloc_node(tree_node, SINGLE_PARSE);
	    tree_node->child[child_descript_3] = $3;	/*description*/
	    tree_node->child[child_referenc_4] = $2; 	/*revision date*/
  	    $$ = tree_node;
    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ============== OBJECT IDENTITY MACRO ======================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ObjectIdentityDefinition
    :   LowercaseIdentifier OBJECT_IDENTITY
        StatusPart
        DescriptionPart
        referencePart 
        CCE ObjectIdentifierValue  {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->type                    = OBJECT_IDENTITY;
            tree_node->string                  = "object-id";
            tree_node->child[child_identity_0] = $1;
            tree_node->child[child_status___2] = $3;
            tree_node->child[child_descript_3] = $4;
            tree_node->child[child_referenc_4] = $5;
            tree_node->child[child_value____9] = $7;

            $4->parent = tree_node;

            /* insert node into appropriate type table */
            if(saveForAllParsesFlag) {
                insert_type_node(clone_node(tree_node), allParsesTypeTable);
            } else {
                insert_type_node(tree_node, singleParseTypeTable);
            }

            $$ = tree_node;

    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================= COMMON =========================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

ValueRange
    :   EndPoint {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = DOT_DOT;
            tree_node->child[chld_end_range13] = $1;
            $$ = tree_node;
    }
    |   EndPoint DOT_DOT EndPoint {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = DOT_DOT;
            tree_node->child[chld_strt_rnge12] = $1;
            tree_node->child[chld_end_range13] = $3;
            $$ = tree_node;
    }
    ;

EndPoint
    :   Number
    |   LowercaseIdentifier
    |   UppercaseIdentifier
    ;

empty
    :   { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    ;

VirtualTableConstructor
    :   SEQUENCE OF UppercaseIdentifier {
        /* uppercase name of row */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = OF;

            tree_node->child[child_identity_0] = $3;

            $$ = tree_node;

            /* insert name into symbol table */
            insert_name($3, virtualTables);

    }
    |   SEQUENCE OF LowercaseIdentifier {
        /* uppercase name of row */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = OF;

            tree_node->child[child_identity_0] = $3;

            $$ = tree_node;

            /* insert name into symbol table */
            insert_name($3, virtualTables);

            print_warning("*** ERROR *** \"SEQUENCE OF\" must be followed by an UppercaseIdentifier.", lineBuf);

    }
    ;

/* description is mandatory in SMIv2, but optional in SMIv1 */
descriptionPart
    :   DescriptionPart
    |   empty {
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
    ;

DescriptionPart
    :   DESCRIPTION QuotedString {
            $2->type = DESCRIPTION;
            $$ = $2;
    }  
    ; 

compliancePart
    :   ComplianceList
    |   empty
    ;

ComplianceList
    :   Compliance { 
            $1->last_tree_node_in_list = $1;
    }
    |   ComplianceList Compliance { 
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

Compliance
    :   ComplianceGroup
    |   ComplianceObject
    ;

ComplianceGroup
    :   GROUP LowercaseIdentifier DescriptionPart {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = $2;
            tree_node->child[child_descript_3] = $3;

            $$ = tree_node; 
    }
    ;

ComplianceObject
    :   OBJECT LowercaseIdentifier 
        complianceSyntaxPart 
        writeSyntaxPart
        complianceAccessPart
        DescriptionPart {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = $2;
            tree_node->child[child_descript_3] = $6;

            tree_node->child[child_misc1___14] = $4;

            tree_node->child[child_access__11] = $5;

            tree_node->child[child_misc2___15]  = $3;

            $$ = tree_node; 
    }
    ;

complianceAccessPart
    :   MIN_ACCESS ComplianceAccess { $$ = $2; }
    |   empty
    ;

ComplianceAccess
    :   NotAccessible
    |   AccessibleForNotify
    |   ReadOnly
    |   ReadWrite
    |   ReadCreate
    ;

creationPart
    :   CREATION_REQUIRES LBRACE creation RBRACE {
            $$ = $3;
    }
    |   empty 
    ;

/* can this part be empty ?? */
creation
    :   VarTypeList
    |   empty
    ;

Variation 
    :   VARIATION LowercaseIdentifier
        complianceSyntaxPart 
        writeSyntaxPart
        variationAccessPart
        creationPart 
        defaultValuePart 
        DescriptionPart {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = $2;
            tree_node->child[child_descript_3] = $8;
            tree_node->child[child_referenc_4] = $6;

            tree_node->child[child_misc1___14] = $4;

            tree_node->child[child_default_10] = $7;
            tree_node->child[child_access__11] = $5;

            tree_node->child[child_misc2___15] = $3;

            if($7->child[child_identity_0]) {

                if( ($3->type == Counter32) || ($3->type == Counter64) ) {
                    print_warning("*** ERROR ***  DEFVAL is not allowed with Counter32 or Counter64.", lineBuf);
                    tree_node->child[child_default_10] = NULL;
                }
            }

            $$ = tree_node; 
    }
    ;

variationAccessPart
    :   ACCESS VariationAccess { $$ = $2; }
    |   empty
    ;

VariationAccess 
    :   NotImplemented
    |   AccessibleForNotify
    |   ReadOnly
    |   ReadWrite
    |   ReadCreate
    |   WriteOnly
    |   NotAccessible
    ;

/* must be a refinement for object's SYNTAX clause */
complianceSyntaxPart
    :   SyntaxPart
    |   empty
    ;

/* must be a refinement for object's SYNTAX clause */
writeSyntaxPart
    :   WRITE_SYNTAX Syntax { $$ = $2; }
    |   empty
    ;

moduleName
    :   UppercaseIdentifier moduleOID {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->child[child_identity_0] = $1;

            tree_node->child[1] = $2;

            $$ = tree_node; 
    }
    |   empty { /* only if contained in MIB module */ }
    ;

moduleOID
    :   ObjectIdentifierValue
    |   empty
    ;

modulePart
    :   ModuleList
    |   empty
    ;

ModuleList
    :   Module { 
            $1->last_tree_node_in_list = $1;
    }
    |   ModuleList Module { 
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

Module
    :   SUPPORTS moduleName INCLUDES LBRACE VarTypeList RBRACE variationPart {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->child[child_identity_0] = $2;
            tree_node->child[1]          = $5;
            tree_node->child[5]          = $7;

            $$ = tree_node; 
    }
    ;

variationPart
    :   VariationList
    |   empty
    ;

VariationList
    :   Variation { 
            $1->last_tree_node_in_list = $1;
    }
    |   VariationList Variation {  
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
    }
    ;

VarTypeList
    :   VarTypes
    |   VarTypes COMMA {
        print_warning("*** ERROR ***  There is an extra comma at the end of the variable type list.", lineBuf);
    }
    ;

VarTypes
    :   VarType { 
            $1->last_tree_node_in_list = $1; 
    }
    |   VarTypes COMMA VarType {
            $1->last_tree_node_in_list->next_tree_node = $3;
            $1->last_tree_node_in_list = $3;
    }
    |   VarTypes VarType {
            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
            print_warning("*** ERROR ***  There is a missing COMMA in the Variable Type List.", lineBuf);
    }
    ;

VarType
    :   LowercaseIdentifier
    |   UppercaseIdentifier {
            print_warning("*** ERROR ***  Variable types must be LowercaseIdentifiers.", lineBuf);
    }
    ;

SyntaxPart
    :   SYNTAX Syntax {
            $$ = $2;
    }
    ;

/*
   rfc1212 - The DEFVAL clause, which need not be present in SMIv1, defines an acceptable
   default value which may be used when an object instance is created at
   the discretion of the agent acting in conformance with the third
   paradigm described in Section 4.2 above.

*/
defaultValuePart
    :   DEFVAL LBRACE DefaultValue RBRACE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = DEFVAL;

            tree_node->child[child_identity_0] = $3;

            $$ = tree_node;
    }
    |   empty 
    ;

/* number is non-negative */
NamedBit
    :   LowercaseIdentifier LPAREN Number RPAREN  {
            $1->child[child_identity_0] = $3;
    }
    |   UppercaseIdentifier LPAREN Number RPAREN  {
            $1->child[child_identity_0] = $3;
             print_warning("*** ERROR ***  Named items must begin with a lowercase letter.", lineBuf);
    }
    |   Number LPAREN Number RPAREN  {
            $1->child[child_identity_0] = $3;
             print_warning("*** ERROR ***  Numbers must not be used for named items.", lineBuf);
    }
    ;

NamedBitListPlugin
    :   NamedBitList
    |   NamedBitList COMMA {
            print_warning("*** ERROR ***  There is an extra COMMA at then end of the Named Item List.", lineBuf);
    }
    ;

NamedBitList
    :   NamedBit {
            symbol_table * enumerationTable = NULL;

            /* create enumeration table */
            enumerationTable = malloc_table(enumerationTable);
 
            /* insert name into symbol table */
            insert_name($1, enumerationTable);

            $1->enumeratedItems = enumerationTable;

            $1->last_tree_node_in_list = $1;

    }
    |   NamedBitList COMMA NamedBit {

            /* insert name into symbol table */
            insert_name($3, $1->enumeratedItems);

            $1->last_tree_node_in_list->next_tree_node = $3;
            $1->last_tree_node_in_list = $3;
    }
    |   NamedBitList NamedBit {

            insert_name($2, $1->enumeratedItems);

            $1->last_tree_node_in_list->next_tree_node = $2;
            $1->last_tree_node_in_list = $2;
            print_warning("*** ERROR ***  There is a missing COMMA in the Named Item List.", lineBuf);
    }
    ;

SMIv2Access
    :   NotAccessible
    |   AccessibleForNotify
    |   ReadOnly
    |   ReadWrite
    |   ReadCreate
    ;

/*
-- Extended UTCTime, to allow dates with four-digit years
-- (Note that this definition of ExtUTCTime is not to be IMPORTed
--  by MIB modules.)
ExtUTCTime ::= OCTET STRING(SIZE(11 | 13))
    -- format is YYMMDDHHMMZ or YYYYMMDDHHMMZ
    --   where: YY   - last two digits of year (only years
    --                 between 1900-1999)
    --          YYYY - last four digits of the year (any year)
    --          MM   - month (01 through 12)
    --          DD   - day of month (01 through 31)
    --          HH   - hours (00 through 23)
    --          MM   - minutes (00 through 59)
    --          Z    - denotes GMT (the ASCII character Z)
    --
    -- For example, "9502192015Z" and "199502192015Z" represent
    -- 8:15pm GMT on 19 February 1995. Years after 1999 must use
    -- the four digit year format. Years 1900-1999 may use the
    -- two or four digit format.

*/
ExtUTCTime
    :   QuotedString
    ;

StatusPart
    :   STATUS Status {
            $$ = $2;
    }
    ;

Status
    :   CurrentKeyword     /* SMIv2 */
    |   DeprecatedKeyword  /* both */
    |   ObsoleteKeyword    /* both */
    |   MandatoryKeyword   /* SMIv1 */
    |   OptionalKeyword    /* SMIv1 */
    ;

referencePart
    :   REFERENCE QuotedString { 
            $$ = $2;
    }
    |   empty {
    }
    ;

RangeRestriction
    :   LPAREN RangeListPlugin RPAREN {
        $$ = $2;
    }
    ; 

SizeRestriction
    :   LPAREN SizeKeyword RangeRestriction RPAREN {
        $$ = $3;
    }
    ; 

RangeListPlugin
    :   RangeList
    |   RangeList BAR {
            print_warning("*** ERROR ***  There is an extra BAR at the end of the range list.", lineBuf);
    }
    ;

RangeList
    :   ValueRange {
            $1->last_tree_node_in_list = $1;
    }
    |   RangeList BAR ValueRange {
            $1->last_tree_node_in_list->next_tree_node = $3;
            $1->last_tree_node_in_list = $3;
    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ===================== TOKEN HOLDERS ======================== */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/*
rfc2578 3.1.1.  Textual Values and Strings

   Some clauses in a macro invocation may take a character string as a
   textual value (e.g., the DESCRIPTION clause).  Other clauses take
   binary or hexadecimal strings (in any position where a non-negative
   number is allowed).

   A character string is preceded and followed by the quote character
   ("), and consists of an arbitrary number (possibly zero) of:

      - any 7-bit displayable ASCII characters except quote ("),
      - tab characters,
      - spaces, and
      - line terminator characters (\n or \r\n).

   The value of a character string is interpreted as ASCII.

   A binary string consists of a number (possibly zero) of zeros and
   ones preceded by a single (') and followed by either the pair ('B) or
   ('b), where the number is a multiple of eight.

   A hexadecimal string consists of an even number (possibly zero) of
   hexadecimal digits, preceded by a single (') and followed by either
   the pair ('H) or ('h).  Digits specified via letters can be in upper
   or lower case.

   Note that ASN.1 comments can not be enclosed inside any of these
   types of strings.
*/

QuotedString
    :   QUOTED_STRING {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            /* stringBuf was zeroed by the lexer before being filled */
            /* so there is a null after the string */ 
            tree_node->string = (char *)malloc( (stringPos + 1)*sizeof(char) ); 
            memcpy(tree_node->string, stringBuf, stringPos);
            tree_node->string[stringPos] = '\0';

            tree_node->freeMe = 1;

            $$ = tree_node; 
    }
    ;

/*BAB*****
Comment
    :   COMMENT {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);

            tree_node->string = (char *)malloc( (stringPos + 1)*sizeof(char) );
            memcpy(tree_node->string, stringBuf, stringPos);
            tree_node->string[stringPos] = '\0';
	    tree_node->type = COMMENT;

            tree_node->freeMe = 1;

            $$ = tree_node;
    }
    ;
****BAB*/

LowercaseIdentifier
    :   LOWERCASE_IDENTIFIER {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = strdup(yylval.char_ptr);

            tree_node->freeMe = 1;

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            $$ = tree_node; 
    }
    |   OptionalKeyword { 
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"optional\" keyword as a lowercase identifier.", lineBuf);
    }
    |   ReadOnly { 
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"read-only\" keyword as a lowercase identifier.", lineBuf);
    }
    |   ReadWrite { 
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"read-write\" keyword as a lowercase identifier.", lineBuf);
    }
    |   WriteOnly {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"write-only\" keyword as a lowercase identifier.", lineBuf);
    }
    |   NotAccessible {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"not-accessible\" keyword as a lowercase identifier.", lineBuf);
    }
    |   NotImplemented {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"not-implemented\" keyword as a lowercase identifier.", lineBuf);
    }
    |   AccessibleForNotify {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"acccessible-for-notifify\" keyword as a lowercase identifier.", lineBuf);
    }
    |   ReadCreate {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"read-create\" keyword as a lowercase identifier.", lineBuf);
    }
    |   DeprecatedKeyword {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"deprecated\" keyword as a lowercase identifier.", lineBuf);
    }
    |   CurrentKeyword {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"current\" keyword as a lowercase identifier.", lineBuf);
    }
    |   MandatoryKeyword {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"mandatory\" keyword as a lowercase identifier.", lineBuf);
    }
    |   ObsoleteKeyword {
            print_note("*** NOTE ***  Although allowed, may not be wise to use the \"obsolete\" keyword as a lowercase identifier.", lineBuf);
    }
    ;

UppercaseIdentifier
    :   UPPERCASE_IDENTIFIER  { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = strdup(yylval.char_ptr);
            tree_node->freeMe = 1;

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            $$ = tree_node;
    }
    ;

Oid
    :   OID {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = strdup(yylval.char_ptr);

            tree_node->freeMe = 1;

            if((int)strlen(tree_node->string) > maxColumnWidth) {
                maxColumnWidth = (int)strlen(tree_node->string); 
            }

            $$ = tree_node; 
    }
    ;

Number
    :   POS_NUMBER {
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

            $$ = tree_node; 
    }
    |   MAX {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = MAX;
            $$ = tree_node; 
    }
    |   MIN {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = MIN;
            $$ = tree_node; 
    }
    |   NEG_NUMBER {
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

            $$ = tree_node; 
    }
    |   BINARY_NUMBER {
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

            $$ = tree_node; 
    }
    |   HEX_NUMBER {
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

            $$ = tree_node; 
    }
    ;

ReadOnly
    :   read_only { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "read-only"; 
            $$ = tree_node;
    }
    ;

ReadWrite
    :   read_write { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "read-write"; 
            $$ = tree_node;
    }
    ;

WriteOnly
    :   write_only { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "write-only"; 
            $$ = tree_node;

            if(SMIv2Flag == 1) { 
                print_warning("*** ERROR *** \"write-only\" is not used in SMIv2.", lineBuf);
            } else if(SMIv1Flag == 0) {
                SMIv1Flag = 1;
                print_note("*** NOTE *** \"write-only\" is not used in SMIv2, setting SMIv1 flag.", lineBuf);
            }

    }
    ;

NotAccessible
    :   not_accessible { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "not-accessible"; 
            $$ = tree_node;
    }
    ;

NotImplemented
    :   not_implemented { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "not-implemented"; 
            $$ = tree_node;

            if(SMIv1Flag == 1) { 
                print_warning("*** ERROR *** \"not-implemented\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"not-implemented\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    ;

AccessibleForNotify
    :   accessible_for_notify { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "accessible-for-notify"; 
            $$ = tree_node;

            if(SMIv1Flag == 1) { 
                print_warning("*** ERROR *** \"accessible-for-notify\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"accessible-for-notify\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    ;

ReadCreate
    :   read_create { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "read-create"; 
            $$ = tree_node;

            if(SMIv1Flag == 1) { 
                print_warning("*** ERROR *** \"read-create\" is not used in SMIv1.", lineBuf);
            } else if(SMIv2Flag == 0) {
                SMIv2Flag = 1;
                print_note("*** NOTE *** \"read-create\" is not used in SMIv1, setting SMIv2 flag.", lineBuf);
            }

    }
    ;

BitsKeyword
    :   BITS {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = BITS;
	    if (formatFlag == XML) {
		tree_node->string = "BITS";
            }
	    else {
                tree_node->string = "Bits";
	    }
            $$ = tree_node;

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
    ;

SequenceKeyword
    :   SEQUENCE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    ;

CurrentKeyword
    :   currrent { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "current";
            $$ = tree_node;
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
    ;

DeprecatedKeyword
    :   deprecated { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "deprecated";
            $$ = tree_node;
    }
    ;

ObsoleteKeyword
    :   obsolete { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "obsolete"; 
            $$ = tree_node;
    }
    ;

MandatoryKeyword
    :   mandatory { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "mandatory";
            $$ = tree_node;
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
    ;

OptionalKeyword
    :   optional { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "optional"; 
            $$ = tree_node;
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
    ;

OctetStringKeyword
    :   OCTET STRING {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "OctetString";
            tree_node->type = OCTET;
            $$ = tree_node;
    }
    ;

Integer32Keyword
    :    Integer32 {        /* (-2147483648..2147483647) */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Integer32";
            $$ = tree_node; 

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
    ;

/*
   This application-wide type represents a 32-bit internet address.  It
   is represented as an OCTET STRING of length 4, in network byte-order.
*/
IpAddressKeyword
    :   IpAddress {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "IpAddress";
            $$ = tree_node; 
    }
    ;

/*
   This application-wide type represents a non-negative integer which
   monotonically increases until it reaches a maximum value, then it
   wraps around and starts increasing again from zero.  This memo
   specifies a maximum value of 2^32-1 (4294967295 decimal) for
   counters.
*/
Counter32Keyword
    :   Counter32 { /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Counter32";
            tree_node->type = Counter32;

            $$ = tree_node; 
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
    ;

/*
   This application-wide type represents a non-negative integer, which
   may increase or decrease, but which latches at a maximum value.  This
   memo specifies a maximum value of 2^32-1 (4294967295 decimal) for
   gauges.
*/
Gauge32Keyword
    :   Gauge32 { /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Gauge32";
            $$ = tree_node; 
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
    ;

GaugeKeyword
    :   Gauge { /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Gauge";
            $$ = tree_node; 

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
    ;

CounterKeyword
    :   Counter {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Counter";
            $$ = tree_node; 

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
    ;

Unsigned32Keyword
    :   Unsigned32 {        /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Unsigned32";
            $$ = tree_node;
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
    |   UInteger32 {        /* (0..4294967295)         */
            /* this is an outdated version not in the language anymore */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Unsigned32";
            $$ = tree_node;

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
    ;

/*
   This application-wide type represents a non-negative integer which
   counts the time in hundredths of a second since some epoch.  When
   object types are defined in the MIB which use this ASN.1 type, the
   description of the object type identifies the reference epoch.
*/
TimeTicksKeyword
    :   TimeTicks {        /* (0..4294967295)         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "TimeTicks";
            $$ = tree_node; 
    }
    ;

/*
   This application-wide type supports the capability to pass arbitrary
   ASN.1 syntax.  A value is encoded using the ASN.1 basic rules into a
   string of octets.  This, in turn, is encoded as an OCTET STRING, in
   effect "double-wrapping" the original ASN.1 value.

   Note that a conforming implementation need only be able to accept and
   recognize opaquely-encoded data.  It need not be able to unwrap the
   data and then interpret its contents.

   Further note that by use of the ASN.1 EXTERNAL type, encodings other
   than ASN.1 may be used in opaquely-encoded data.
*/
OpaqueKeyword
    :   Opaque {            /* IMPLICIT OCTET STRING     */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Opaque";
            $$ = tree_node; 
    }
    ;

Counter64Keyword    
    :   Counter64 {            /* (0..18446744073709551615) */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "Counter64";
            tree_node->type = Counter64;

            $$ = tree_node; 
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
    ;

IntegerKeyword
    :   INTEGER {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
	    tree_node->type = INTEGER;
            tree_node->string = "INTEGER";
            $$ = tree_node;
    }  
    ;

BitStringKeyword  
    :    BIT STRING {        /* (SIZE (0..65535))         */
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = BIT;
            tree_node->string = "BIT STRING";
            $$ = tree_node;

            print_warning("*** ERROR *** \"BIT STRING\" is no longer a legal type.", lineBuf);
    }
    ;

ObjectIdentifierKeyword
    :   OBJECT IDENTIFIER {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "ObjectID";
            tree_node->type   = IDENTIFIER;
            $$ = tree_node;
    }
    ;

SizeKeyword
    :   SIZE {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->type = SIZE;
            tree_node->string = "SIZE";
            $$ = tree_node; 

    }
    ;

ObjectSyntaxKeyword
    :   ObjectSyntax {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            tree_node->string = "ObjectSyntax";
            $$ = tree_node; 

    }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* =================== GARBAGE CATCHERS ======================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

/* used to catch garbage in bad mibs so they still parse */
/* this stuff is not really in the language */

TypeGarbage
    :   TypeTag Type { $$ = $2; }
    |   TypeTag IMPLICIT Type { 
		$$ = $3; 
    }
    |   TypeTag EXPLICIT Type { $$ = $3; }
    |   CHOICE LBRACE AlternativeTypeList RBRACE       { $$ = $3; }
    |   CHOICE LBRACE AlternativeTypeList COMMA RBRACE { $$ = $3; }
    |   GarbageType
    ;

AlternativeTypeList
    :   NamedType
    |   AlternativeTypeList COMMA NamedType
    ;

NamedType
    :   LowercaseIdentifier Type
    |   Type
    ;

TypeTag
    :   LBRACKET class NumericValue RBRACKET { $$ = $3; }
    ;

class
    :   UNIVERSAL { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    |   APPLICATION { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    |   PRIVATE { 
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    |   empty
    ;

NumericValue
    :   Number
    |   DefinedValue
    ;

GarbageType
    :   ANY {
            tree * tree_node = NULL;
            tree_node = malloc_node(tree_node, SINGLE_PARSE);
            $$ = tree_node;
    }
    |   ANY DEFINED BY LowercaseIdentifier { $$ = $4; }
    ;

/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */
/* ========================= MAIN ============================= */
/* ªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªªª */

%%

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
        fprintf(stderr, "USAGE: mibgc [-b|B] [-c|C] [-d|D] [-f|F outputdirectory] [-h|H] [-i|I]\n");
	fprintf(stderr, "           [-m|M mibFileDir:mibFileDir:...] [-o|O outputfilename] [-r|R]\n");
	fprintf(stderr, "           [-s|S] [-t|T idat] [-u|U] [-v|V] [-x|X] inputFileNames\n\n");
/*******fprintf(stderr, "       (e.g. mibgc *.my -o outfile)\n\n");*/
        fprintf(stderr, "   FLAGS (use either upper or lower case:\n\n");
        fprintf(stderr, "   -b -B   - Backup any existing output files.\n");
        fprintf(stderr, "   -c -C   - Treat comments like C++ comments.\n");
        fprintf(stderr, "   -d -D   - Produce description file.\n");
        fprintf(stderr, "   -f -F   - Directory to dump output files\n");
	fprintf(stderr, "             (e.g. ../mibgc *.my -s -f ~/dump)\n");
        fprintf(stderr, "   -h -H   - Show help information (also --h or --H).\n");
        fprintf(stderr, "   -i -I   - Ignore strict compliance.\n");
        fprintf(stderr, "   -m -M   - Directories (separated by colons) to search for.\n");
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
        fprintf(stderr, "       (3) /usr/local/mibs (default mib location)\n");
        fprintf(stderr, "       (4) ./\n\n");
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

        defLoc->string = strdup("/usr/local/mibs");

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

            strcat(lineBuf, "/");
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
                    strcat(fileNameBuffer, "/");
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
                strcpy(lineBuf, "mv ");
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
                    strcat(lineBuf, "/");
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
    fprintf(outputFilePtr, "%s", ctime(&myclock, buf));
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
        		    strcpy(lineBuf, "rm ");
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
        strcpy(lineBuf, "mv ");
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
