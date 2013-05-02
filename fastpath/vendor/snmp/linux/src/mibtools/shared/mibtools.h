/*
 *
 * Copyright (C) 1992-2006 by SNMP Research, Incorporated.
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

#ifndef SR_MIBTOOLS_H
#define SR_MIBTOOLS_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "sr_conf.h"

#ifndef SNMPINFO
#define SNMPINFO
#endif /* SNMPINFO */

#ifndef NEW_OID_LIB
#define NEW_OID_LIB
#endif /* NEW_OID_LIB */

#include <stdio.h>

#include <stdlib.h>

#include <ctype.h>

#include <string.h>


#include <malloc.h>

#include "sr_snmp.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "prnt_lib.h"
#include "frmt_lib.h"
#include "oid_lib.h"

#ifdef  MIBTOOLS_MAIN
#define EXTDECL
#else
#define EXTDECL extern
#endif

/* globals */
EXTDECL int	merge_warnings;
EXTDECL int	merge_errors;
EXTDECL int	agent_errors;

/* Command line options */
EXTDECL char	*prog_name;
EXTDECL char	*postmosy_config_file;  /* for use with the -f option */
EXTDECL char	*postmosy_config_line;  /* for use with the -c option */
EXTDECL char	*light_config_file;     /* for use with the -light option */
EXTDECL char	*include_file;          /* for use with the -include option */
EXTDECL FILE	*fpmerge;               /* for use with the -merge_report */
EXTDECL char	*override_config_file;  /* if -override_v is given on the
                                         * command-line, read a config
                                         * file in which the user
                                         * can override the system
                                         * independent method routines
                                         * that will be called by directly
                                         * assigning the names of v_ get
                                         * and test functions directly
                                         * into <base>oid.c; useful if
                                         * you want most of the families
                                         * to use minimal/no v_get but
                                         * you have to use the classic
                                         * v_ functions in some special
                                         * cases; also useful if you are
                                         * splitting a single family into
                                         * two sets of method routines,
                                         * so some of the objects can
                                         * call one v_ get and other
                                         * objects in the same family
                                         * can call a different v_ get */
EXTDECL int	individual;	/* if -individual is given on the command
				 * line, individual prototypes are placed in
				 * snmptype.h, otherwise only group
				 * prototypes are used  */
EXTDECL int	short_names;	/* if -short is given on the command line,
				 * names are limited to 31 characters for
				 * microsoft C  */
EXTDECL int	manager_config_files;		/* if -nm4w is given on
						 * the command line, several
						 * config files for netmon
						 * for windows are generated */
EXTDECL int	agent;		/* generate method routine stubs */
EXTDECL int	traps;		/* generate trap routines */
EXTDECL int	oidl;		/* generate standalone OidList file */
EXTDECL int	localconnect;	/* generate LocalConnect() block file */
EXTDECL int	undo;       	/* if -no_undo is given on the command line,
                                 * undo methods will not be generated in the
                                 * v_ routines
                                 */
EXTDECL int	var_param_list;	/* if -vpl is given on the command
					 * line then k_ function prototypes
					 * may have varaible parameter lists.
					 * This is here for compatability
					 * with the old way. It will most
					 * likely disappear in a future
					 * releases */
EXTDECL int	silent;     	/* supress verbose printing */
EXTDECL int	userpart;	/* if -userpart is given on the command line,
				 * include U_family in the type file. */
EXTDECL int	D_long;		/* use longs in snmpdefs.h */
EXTDECL int	xxx_read_create;	/* add #define xxx_READ_CREATE in
					 * v_snmp.stb so that the default is
					 * read-create rather than than
					 * read-write for tables (since v1
					 * has no read-create). */
EXTDECL int 	cache;		/* add simple caching to v_snmp.stb ( 1 =
				 * cache, 0 = no cache) */
EXTDECL int	xxx_type;	/* add info about types, this is experimental */
EXTDECL int	one_based;  	/* experimental, v_routines set index to a 
 				 * minimum of the index and 1 */
EXTDECL int	priority;   	/* value for the priority field of the
                                 * ObjectInfo structure define in snmpoid.c.
                                 * The default is 32, it can be changed with
                                 * The -priority option
                                 */
EXTDECL int	ignore_nonfatal_errors;
EXTDECL int	export_st;     	/* This option will change the definition
				 * of STdeclare in <base>part.h so that
				 * the arrays are not static.  Also, if a
				 * special macro is defined, an extern
				 * declaration to the array will be used.
				 */
EXTDECL int	intel;      	/* This option will move the strings in
				 * snmp-mib.h and snmppart.h out of DGROUP 
				 * on intel 808x processors. This option
				 * cannot be used with the -const option.
				 */
EXTDECL int	use_const;      /* If -const is given on the command line,
				 * storage space in snmppart.h and snmpoid.c
				 * is declared const. This option cannot
				 * be used with the -intel option.
				 * NOTE:  this is now the default.  Kept
				 * for backwards compatibility.
				 * -no_const sets this to 0;
				 */
EXTDECL int	offset;     	/* This option causes the method routines
				 * to use the SR_OFFSET macro defined in
				 * snmpd.h. It reduces the code size, but
				 * may not be portable to all platforms
				 */
EXTDECL int     os;         	/* This option turns all all flags which
				 * will optimize code size.
				 */
EXTDECL int     cplusplus;  	/* This option modifies the generation of
				 * the minv code so that it will compile
				 * under C++ compilers.
				 */
EXTDECL int     v_elimination_data;
                                /* Generate SnmpType and SrIndexInfo
                                 * structures, the minv_getinfo array of
                                 * SrGetInfoEntry structures, and the
                                 * minv_testinfo array of SrTestInfoEntry
                                 * structures 
                                 */
EXTDECL int     small_v_uses_global;
                                /* The "small" system independent method
                                 * routines use the SrGetInfoEntry and
                                 * SrTestInfoEntry data in the global
                                 * minv_getinfo and minv_testinfo arrays.
                                 * Otherwise, generate standalone
                                 * declarations of these structs.
                                 */
EXTDECL int     small_v_get;	/* Generate system independent Get
                                 * method routines that do nothing except
                                 * invoke a library function, passing in
                                 * a pointer to the element of the
                                 * SrGetInfoEntry array that corresponds
                                 * to the family.
                                 */
EXTDECL int     small_v_test;	/* Generate system independent Test
                                 * method routines that do nothing except
                                 * invoke a library function, passing in
                                 * a pointer to the element of the
                                 * SrTestInfoEntry array that corresponds
                                 * to the family.
                                 */
EXTDECL int     medium_v;	/* Generate small system independent Get
                                 * and Test method routines.
                                 */
EXTDECL int     no_v_get;    	/* Generate an OID list in which the
                                 * GetMethod pointer does not point to a
                                 * function, but instead points to the
                                 * element of the SrGetInfoEntry array
                                 * that corresponds to the family.
                                 *
                                 * Special support is required in the
                                 * agent dispatch table code to correctly
                                 * handle this overloading--it should call
                                 * the v_get() library function and pass
                                 * to it the pointer to the data structure.
                                 */
EXTDECL int     no_v_test;    	/* Generate an OID list in which the
                                 * TestMethod pointer does not point to a
                                 * function, but instead points to the
                                 * element of the SrTestInfoEntry array
                                 * that corresponds to the family.
                                 *
                                 * Special support is required in the
                                 * agent dispatch table code to correctly
                                 * handle this overloading.
                                 */
EXTDECL int     new_k_stubs;	/* Generate system dependent Get method
                                 * routines having exactly five parameters.
                                 * In the massive system independent Test
                                 * routines, the invocations of these
                                 * functions are also modified.
                                 */
EXTDECL int     new_k_wrappers;	/* Generate pass-through system dependent
                                 * Get method routines having exactly five
                                 * parameters.  These do nothing except to
                                 * invoke the old-style routines with the
                                 * corrent number and types of parameters.
                                 */
EXTDECL int	compute_nominator; /* generate an array with the last
                                    * subidentifier as the value and nominator
                                    * as the index. use this array to get
                                    * nominator in the method routines 
                                    * instead of getting it from the 
                                    * dispatch table */	
EXTDECL int	type_table; 	/* The <family>TypeTable is used both by
                                 * by the table driven parser code and
                                 * by the min_v/no_v code, and the data
                                 * structure is instantiated in different
                                 * places for different cases.  This
                                 * variables controls whether or not the
                                 * data structure is instantiated at all,
                                 * so the user has greater control for the
                                 * case that the MIB will be compiled
                                 * piecemeal with different sets of output
                                 * files.
                                 */
EXTDECL int	table_lookup; 	/* Use table manipulation routines in
				 * lookup.c
				 */
EXTDECL int	search_table; 	/* Use wrappers around table manipulation 
                                 * routines in lookup.c
                                 */
EXTDECL int	defval_c_string;/* interpret "\0", "\n", "\r", "\a", "\b",
                                 * "\t", "\v", and "\f" in the DEFVAL
                                 * clause in the MIB as escape sequences
				 */
EXTDECL int	per_file_init;  /* print k_ initialize and k_ terminate 
				 * routines for each k_*.stb file
				 */
EXTDECL int	no_cvs_keywords;/* suppress printing of CVS keywords */
EXTDECL int	k_init; 	/* print the k_initialize and 
				 * k_terminate functions
				 */
EXTDECL int	k_term; 	/* print the k_terminate function */
EXTDECL int	stubs_only; 	/* only print .stb files */
EXTDECL int	v_stubs_only; 	/* only print v_ .stb files */
EXTDECL int	mergeinfo; 	/* read files in the format of snmpinfo.dat
				 * and write out the merged snmpindo.dat
				 * also read in snmpinfo.dat from the directory
				 * pointed to by SR_MGR_CONF_DIR 
				 */
EXTDECL int	inputinfo; 	/* read files in the format of snmpinfo.dat
				 * and write out the merged snmpindo.dat
				 */
EXTDECL int	remove_subtree;	/* remove subtree from the snmpinfo.dat file */
EXTDECL int     ansi;           /* use ansi function declarations */

EXTDECL int     no_defval;      /* Do not print the value from the defval clause
				 * in the k_ set_defaults routine
				 */
EXTDECL int	fix_rowstatus_defval;   /* interpret RowStatus DEFVAL for
                                 * k_<family>_set_defaults() as follows:
                                 *   VALUE IN MIB    INIT VALUE IN CODE
                                 *   active       -> createAndGo
                                 *   notInService -> createAndWait
                                 *   notReady     -> createAndWait
                                 */
EXTDECL int     row_status;     /* use RowStatus textual convention support
                                 * routines
                                 */
EXTDECL int     old_row_timer;  /* used with -row_status option. generate old 
                                 * style timer code on row creation for 
                                 * backward compatibility.
                                 */
EXTDECL int     comments;       /* used with -parser option, print comments 
                                 * in the config files.
                                 */
EXTDECL int     storage_type;   /* use StorageType textual convention support
                                 * routines
                                 */
EXTDECL int     test_and_incr;  /* use TestAndIncr textual convention support
                                 * routines
                                 */
EXTDECL int     argsused;	/* print ARGSUSED before mibcompiler generated
                                 * functions 
				 */
EXTDECL int     check_context;	/* check the context before calling the
				 * k_get routine 
				 */
EXTDECL int     parser;         /* use table driven parser code for
                                 * initializing tables
                                 */
EXTDECL int     late;           /* instance registration for EMANATE */
EXTDECL int     instance;       /* instance registration for EMANATE */
EXTDECL int     mibtree;        /* generate MIB Tree data structure */
EXTDECL int     separate_type_file; /* generate separate snmptype.h and
                                     * snmpprot.h files */
EXTDECL int     snmpoid_only;   /* only generate snmpoid.c file */
EXTDECL int     make;           /* generate Makefile fragment */
EXTDECL int     mgroidl;        /* generate oid list for use by managers */
EXTDECL int     augments_separate; /* treat tables with AUGMENTS clauses as
                                 * separate tables from the base table as
                                 * if they were defined with INDEX instead
                                 * of AUGMENTS */
#ifdef CIAGENT_MONITOR
EXTDECL int     ciagentmonitor; /* generate config file for CIAgent Monitor */
EXTDECL int     rtview;         /* generate config file for rtview */
#endif /* CIAGENT_MONITOR */
#ifdef SR_AGENT_TOOLS
EXTDECL int     mgrtool;        /* allow postmosy to generate mgrtools files */
#endif /* SR_AGENT_TOOLS */
#if (defined(SR_AGENT_TOOLS) || defined(SR_EP_TOOLS))
EXTDECL int     snmpmibh;        /* generate snmp-mib.h */
#endif	/* (defined(SR_AGENT_TOOLS) || defined(SR_EP_TOOLS)) */
#ifdef SR_EP_TOOLS
EXTDECL int     snmpinfo;        /* generate snmpinfo.dat */
EXTDECL int     build_varbinds;  /* create functions that build varbinds */
#endif /* SR_EP_TOOLS */
#ifdef SR_NAA_TOOLS
EXTDECL int     naa_static;      /* generate config file for -override_v */
EXTDECL int     naa_dynamic;     /* generate config file for naaagt */
#endif /* SR_NAA_TOOLS */

#define BUFF_SIZE               2048
#define NAME_SIZE               128
#define MAX_ENTRIES 		128
#define SNMP_MIB_NAME           "snmp-mib.h"
#define NOT_ACCESSIBLE_STR      "not-accessible"
#define ACCESSIBLE_FOR_NOTIFY_STR "accessible-for-notify"
#define READ_ONLY_STR           "read-only"
#define READ_WRITE_STR          "read-write"
#define READ_CREATE_STR         "read-create"
#define WRITE_ONLY_STR          "write-only"	/* for v1 compatability only */
#define DEFAULT_BASE_STR        "snmp"
#define TRUE                    1
#define FALSE                   0
#define MAX_COLUMNS             254

/*
 * The macros FREE, TEST, INST, INST_TO_X, X_TO_INST, DP, CLONE,
 * and VALUE are potential values for the 'index' parameter to
 * the get_string() function (mibtools/agttools/agtutil.c).
 * They are indexes into a buffer array with original dimensions
 * of [8][1024].
 * 
 * All of these macros are potential values for the 'which' parameter
 * to the print_string() function (mibtools/agttools/agtutil.c).
 * 
 * 1/15/2003 - DSS
 * Added FINDEX_CAT for print_string() with value 16 (binary 10000)
 * so it can be OR'd with VINDEX_CAT to make a unique value.
 *
 */
#define FREE                    0
#define TEST                    1
#define INST                    2
#define INST_TO_X               2
#define X_TO_INST               2
#define DP                      3
#define CLONE                   4
#define VALUE                   5
#define GET_INDEX               5
#define INDEX_CAT               7
#define INDEX_TYPE              8
#define VINDEX_CAT              9
#define FINDEX_CAT             16
#define ZERO_ASSIGNMENT        17

/*
 * These values for IndexType.
 * Values greater than zero indicate the SIZE of fixed-length indices,
 * such as OctetString.
 * 
 * %%% DSS -- e.g., NetworkAddress has IndexType 5...see get_index_type()
 */
#define SR_FIXED		0
#define SR_VARIABLE		-2
#define SR_IMPLIED		-3
#define SR_DONT_CARE		-4
/*
 * SR_UNKNOWN is removed because it conflicts with a different
 * definition of SR_UNKNOWN and doesn't appear to be used in 
 * the MIB tools in any case.
 */
/* #define SR_UNKNOWN		-5 */
#define SR_ASSUME_VARIABLE	-6

/*
 * defines for whether it's a .c or .h file to control which
 * sort of cvs keywords to print out.
 */
#define C_FILE 0
#define H_FILE 1

typedef struct enumerations {
    char           *name;
    char           *cname; /* change - to _ to be legal C syntax */
    int             val;
    struct enumerations *next;
}               enumeration_t;
EXTDECL enumeration_t *temp_enum;

typedef struct size_range_s {
    long min;
    long max;
    struct size_range_s *next;
} size_range_t;

typedef struct TC {		/* textual conventions */
    char           *name;
    char           *type;
    char           *hint;
    size_range_t   *size;	/* The size range (from in the SYNTAX clause) */
    enumeration_t  *enumer;	/* enumerated values */
    struct TC      *next;
}               TC;
EXTDECL TC     *tc;

typedef struct OID_INFO {
    char           *name;	/* element's name */
    char           *oid_pre_str;/* name string that should map to parent
				 * OID_INFO's *name */
    char           *oid_type;	/* string that indicates the parameter type,
				 * or NULL */
    char           *oid_prim_type;	/* string that indicates the
					 * primitive type, or NULL */
    char           *oid_access;	/* string that indicates the access type, or
				 * NULL */
    char           *oid_post_str;	/* finishing part (should be numbers
					 * or numbers.numbers...) */
    char           *oid_fleshed_str;	/* the full textual oid in
					 * number.number... format */
    size_range_t   *size;	/* The size range (from in the SYNTAX clause) */
    enumeration_t  *enumer;	/* enumerated integers, or NULL */
    OID            *oid_ptr;	/* the actual OID for the element */
    char           *augments;	/* name of the table which this augments, or
				 * NULL */
    int             augmented;	/* boolean value to indicate if this entry is
				 * augmented */
    char	   *defval;	/* value from the DEFVAL clause */
    int             valid;	/* boolean used to invalidate entries in
				 * groups not to be printed */
    int             read_write;	/* boolean to indicate if this is writable */
    int             group_read_write;	/* as above, but for the family */
    int             group_read_create;	/* as above, but for createable */
    int             family_root;/* boolean used to see if this is the root
				 * node of a family */
    int             num_entries;/* if family_root = 1, num_entries will tell
                                 * how many entries are in this family */

    /* indicates whether minimal v_ funcs support this family.  Should be 
       initialized to MINV_NOT_DETERMINED.  Only relevant for family roots. */
    int             minv_supported;  
    int             family_nr;  /* Swede way of indexing into minv_getinfo */
    int             getinfo;    /* position in minv_getinfo */
    int             testinfo;   /* position in minv_testinfo */
    char           *override_v_get;     /* explicit method routine to use */
    char           *override_v_test;    /* explicit method routine to use */

    struct OID_INFO *next_family;	/* points to the 'root' of the next
					 * family */
    struct OID_INFO *next_family_entry;	/* points to the next entry in the
					 * family */
    struct OID_INFO *parent_ptr;/* will point to parent OID_INFO structure */
    struct OID_INFO *next;	/* points to next structure on input list */
    struct OID_INFO *next_sorted;	/* points to next OID after insertion
					 * sort */
}               OID_INFO_t;
EXTDECL OID_INFO_t *root, *oid_root, *sorted_root;

typedef struct TRAP_INFO {
	char *name;
	char *enterprise;
	long number;
        char *objects;
        struct TRAP_INFO *next;
}		TRAP_INFO_t;
EXTDECL TRAP_INFO_t *trap_info;

typedef struct _index_array {
    char           *name;
    char           *c_type;
    char           *asn1_type;
    int             IndexType;
    int             length;
    int             InThisTable;
}               index_array_t;

typedef struct index_table {
    char           *name;
    char           *index;
    char           *buff;
    char           *types;
    char           *name_type;
    struct OID_INFO *ptr;
    index_array_t   index_array[32];
    struct index_table *next;
}               index_table_t;
EXTDECL index_table_t *index_table;

typedef struct index_list {
    char           *name;
    char           *type;
    struct index_list *next;
}               index_list_t;
EXTDECL index_list_t *index_list;

typedef struct augments_table {
    char           *name;
    char           *augments;
    struct augments_table *next;
}               augments_table_t;
EXTDECL augments_table_t *augments_table;

/* Enums indicating minv support for a specific family 
   in OID_INFO struct */
#define MINV_YES 1
#define MINV_NO 0
#define MINV_NOT_DETERMINED -1

/* Signatures encode parameter sizes for the indexes passed to k_get, as well
   as the number of parameters to pass. They are trinary numbers where the 
   least significant digit is the size code for the leftmost argument. */
#define MINV_END_OF_PARAMS 0
#define MINV_PTR_PARAM 1
#define MINV_INT_PARAM 2
#define MINV_ADD_PARAM(minv_sign, minv_param) \
            (minv_sign = (minv_sign)*3 + (minv_param))
typedef SR_INT32 signature_t;
typedef struct signature_node {
    signature_t           signature;
    struct signature_node *next;
} signature_node;

/* function prototypes for functions in input.c */

extern int main
    SR_PROTOTYPE((int argc,
		  char **argv));

extern void usage
    SR_PROTOTYPE((void));

extern int input_file
    SR_PROTOTYPE((FILE *fp));

extern int input_snmpinfo_file
    SR_PROTOTYPE((FILE *fp));

extern int add_trap
    SR_PROTOTYPE((char *name,
		  char *enterprise,
		  long number,
		  char *objects));

extern int get_enum
    SR_PROTOTYPE((char *buffer));

extern int get_snmpinfo_enum
    SR_PROTOTYPE((FILE *fp,
		  char *objectname));

extern int get_snmpinfo_index
    SR_PROTOTYPE((FILE *fp,
		  char *objectname));

extern int old_get_enum
    SR_PROTOTYPE((char *buffer,
		  FILE *fp));

extern int add_size_range
    SR_PROTOTYPE((char *name,
		  char *range_min,
		  char *range_max));

extern int add_init_oi
    SR_PROTOTYPE((void));

extern int process_data
    SR_PROTOTYPE((void));

extern int flesh_out
    SR_PROTOTYPE((char *buff,
		  struct OID_INFO *ptr,
		  int x));

extern int output_multiple_files
    SR_PROTOTYPE((char *base));

#ifdef SR_NAA_TOOLS
extern int output_dynamic_naa_file
    SR_PROTOTYPE((char *base));

extern int output_static_naa_file
    SR_PROTOTYPE((char *base));
#endif /* SR_NAA_TOOLS */




extern int print_copyright
    SR_PROTOTYPE((FILE *fp));

extern int print_copyright_2
    SR_PROTOTYPE((FILE *fp));

extern void print_cvs_keywords
    SR_PROTOTYPE((FILE *fp,
		  int filetype));

extern int find_type
    SR_PROTOTYPE((char *type,
		  char *name,
		  char *oid_type,
		  struct size_range_s *size));

extern int map_variable_type_for_ber
    SR_PROTOTYPE((char *type,
		  char *buf));

extern int add_oi
    SR_PROTOTYPE((char *input_name,
		  char *input_oid_str,
		  char *input_type,
		  char *input_access));

extern int check_names
    SR_PROTOTYPE((void));

extern int sort_data
    SR_PROTOTYPE((void));

extern struct OID_INFO *insert_sort_data
    SR_PROTOTYPE((struct OID_INFO *exist_ptr,
		  struct OID_INFO *add_ptr));

extern int group_nodes_by_family
    SR_PROTOTYPE((void));

extern void make_families_with_valid_augments_valid
    SR_PROTOTYPE((void));

extern int count_family_entries
    SR_PROTOTYPE((void));

extern int cmp_oid
    SR_PROTOTYPE((struct _OID *ptr1,
		  struct _OID *ptr2));

extern void check_defs_size
    SR_PROTOTYPE((char *word,
		  struct enumerations *enumeration_list,
		  char *name));

extern void shorten_names
    SR_PROTOTYPE((void));

extern void remove_hyphens
    SR_PROTOTYPE((char *word));

extern int build_index_table
    SR_PROTOTYPE((char *name,
		  char *index));

extern int process_index_table
    SR_PROTOTYPE((void));

extern char *get_table_asn1_types
    SR_PROTOTYPE((char *index));

extern int get_index_type
    SR_PROTOTYPE((char *index));

extern char *get_table_types
    SR_PROTOTYPE((char *index));

extern char *get_index_type_from_table
    SR_PROTOTYPE((char *name));

extern int ctype_is_pointer(char *s);

extern char *find_type2
    SR_PROTOTYPE((char *oid_type));

extern int get_index_length
    SR_PROTOTYPE((char *name));

extern int build_augments_table
    SR_PROTOTYPE((char *name,
		  char *augments));

extern int process_augments_table
    SR_PROTOTYPE((void));

extern int print_support_instructions
    SR_PROTOTYPE((FILE *fp_support));

extern int print_start_protection
    SR_PROTOTYPE((FILE *fp,
		  char *base,
		  char *name));

extern int print_end_protection
    SR_PROTOTYPE((FILE *fp,
		  char *base,
		  char *name));

extern int build_mib_table
    SR_PROTOTYPE((void));

extern FILE *open_file
    SR_PROTOTYPE((char *pre,
		  char *post));

extern FILE *open_file_read_only
    SR_PROTOTYPE((char *pre,
		  char *post));

extern void get_prim_type
    SR_PROTOTYPE((void));

extern int get_version_info
    SR_PROTOTYPE((char *type));

extern int is_oid
    SR_PROTOTYPE((char *type));

extern int is_octetstring
    SR_PROTOTYPE((char *type));

extern int is_counter
    SR_PROTOTYPE((char *type));

extern int is_counter64
    SR_PROTOTYPE((char *type));

extern int is_bits
    SR_PROTOTYPE((char *type));

extern int add_defval
    SR_PROTOTYPE((char *name,
		  char *defval));

extern int isAggregateType(char *);



/* function prototypes for functions defined in output.c */
extern int output_file
    SR_PROTOTYPE((char *base));

extern int output_manager_files
    SR_PROTOTYPE((char *base));

extern int output_mib_file
    SR_PROTOTYPE((char *base));

extern int output_xmib_file
    SR_PROTOTYPE((char *base));

extern int get_index_array
    SR_PROTOTYPE((char *name,
		  struct _index_array **index));

extern int get_index_array2
    SR_PROTOTYPE((struct OID_INFO *ptr,
		  struct _index_array **index));


/* function prototypes for functions defined in stbhdr.c */
void print_converter_entry
    SR_PROTOTYPE((FILE *fp, struct OID_INFO *temp_ptr));
void print_comments_entry
    SR_PROTOTYPE((FILE *fp, struct OID_INFO *temp_ptr));


/* function prototypes for functions defined in stbutil.c */
char *last_object_in_family 
    SR_PROTOTYPE((struct OID_INFO *ptr));

struct OID_INFO *next_valid_family(struct OID_INFO *ptr);

/* function prototypes for functions defined in minvutil.c */
int minimizable_vfunc_p(struct OID_INFO *family);

#ifdef  __cplusplus
}
#endif

#endif				/* _MIBTOOLS_H_ */
