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

/*
 * This file contains definitions used for agent code generation.
 */

#ifndef SR_AGENT_H
#define SR_AGENT_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "sr_proto.h"

/* external declarations */

#ifndef SR_AGENT_MAIN
extern char     dashes[];
extern char     rowStatusString[];
extern char     testAndIncrString[];
extern char     storageTypeString[];
extern char     memoryTypeString[];
extern int      writing_get_method;
#endif				/* SR_AGENT_MAIN */

/* prototypes for agent specific files */

extern struct OID_INFO *storage_type_in_family
    SR_PROTOTYPE((struct OID_INFO *ptr));

extern int settable_object_in_family
    SR_PROTOTYPE((struct OID_INFO *ptr));

extern struct OID_INFO *row_status_in_family
    SR_PROTOTYPE((struct OID_INFO *ptr));

extern struct OID_INFO *row_status_in_family2
    SR_PROTOTYPE((struct OID_INFO *ptr));

extern struct OID_INFO *next_valid_family
    SR_PROTOTYPE((struct OID_INFO *ptr));

extern int row_status_in_file
    SR_PROTOTYPE((void));

extern void write_k_clone_userpart_routine
    SR_PROTOTYPE((FILE *fp,
		  struct OID_INFO *ptr));

extern void write_k_free_userpart_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_v_free_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_v_cleanup_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_v_cleanup_prototype
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern int write_k_set_defaults_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern int write_k_undo_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_k_set_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr,
                  char *base));

extern void write_v_set_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_k_test_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_v_clone_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern int print_test_switch
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr,
                  struct OID_INFO *temp_ptr,
                  struct _index_array * temp_index,
                  struct _index_array * index_array,
                  struct OID_INFO *storage_type_ptr));

extern void write_v_test_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_v_ready_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern int write_k_ready_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_k_get_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr,
                  int new));

extern void write_k_get_wrapper
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_v_get_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));


extern void write_no_v_get_comment
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_scalar_family_TypeTable
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_tabular_family_TypeTable
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_any_family_TypeTable
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_tabular_family_TypeTable_comment
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr,
                  char *base));

extern void write_SrIndexInfo
    SR_PROTOTYPE((FILE *fp,
                  struct OID_INFO *ptr));

extern void write_rs_delete_callback
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr,
                  struct OID_INFO *row_status_ptr));

extern void write_rs_delete_entry_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_rs_ready_to_activate_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_row_status_routines
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_k_context_routine
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_k_init_routines
    SR_PROTOTYPE((FILE * fp,
                  char *base));

extern void print_type_record
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *temp_ptr,
                  struct _index_array * index_array,
                  char *parent,
		  int local));

extern void print_family_descriptor
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern void write_k_stb_headers
    SR_PROTOTYPE((FILE * fp,
                  char *base));

extern void write_v_stb_headers
    SR_PROTOTYPE((FILE * fp));

extern int write_k_agent_files
    SR_PROTOTYPE((char *base));

extern int write_v_agent_files
    SR_PROTOTYPE((char *base));

extern char    *get_string
    SR_PROTOTYPE((char *type,
                  char *name,
                  int index));

extern int print_x_to_inst
    SR_PROTOTYPE((FILE * fp,
                  struct _index_array * index,
                  int *check));

extern int trap_print_x_to_inst
    SR_PROTOTYPE((FILE * fp,
                  struct _index_array * index,
                  int number));

extern int print_string
    SR_PROTOTYPE((FILE * fp,
                  struct _index_array * index,
                  int which));

extern int WhosBuriedInGrantsTombCheck
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *temp_ptr));

extern int print_assume_var_warning
    SR_PROTOTYPE((char *name));

extern int CheckRanges
    SR_PROTOTYPE((FILE * fp,
                  struct OID_INFO *ptr));

extern int print_index_size_check
    SR_PROTOTYPE((FILE * fp,
                  char *name));

extern int NumberForm
    SR_PROTOTYPE((char *name,
                  char *buff));

extern int write_traps
    SR_PROTOTYPE((char *base));

extern int write_agent_oidlist_file
    SR_PROTOTYPE((char *base));

extern int write_localconnect_file
    SR_PROTOTYPE((char *base));

extern int write_configio_file
    SR_PROTOTYPE((char *base));

extern int output_agent_files
    SR_PROTOTYPE((char *base));

extern int isWritable
    SR_PROTOTYPE((struct OID_INFO *ptr));

extern int write_minv_file
    SR_PROTOTYPE((char *base));

extern void
emit_SrGetInfoEntry_standalone
    SR_PROTOTYPE((FILE *fp,
                  struct OID_INFO *ptr));

extern void
emit_SrGetInfoEntry_augments_standalone
    SR_PROTOTYPE((FILE *fp,
                  struct OID_INFO *orig,
                  struct OID_INFO *augment));

extern void
emit_SrGetInfoEntry_element
    SR_PROTOTYPE((FILE *fp,
                  struct OID_INFO *ptr));

extern void
emit_SrTestInfoEntry_auxiliary_array(FILE *, struct OID_INFO *);

extern void
emit_SrTestInfoEntry_standalone
    SR_PROTOTYPE((FILE *fp,
                  struct OID_INFO *ptr));

extern void
emit_SrTestInfoEntry_augments_standalone(FILE *fp, struct OID_INFO *orig, 
                                         struct OID_INFO *augment);

extern void
emit_SrTestInfoEntry_element
    SR_PROTOTYPE((FILE *fp,
                  struct OID_INFO *ptr));

extern void
emit_OID_initializer(FILE *fp, struct OID_INFO *ptr);

extern void
emit_SrTestInfoEntry_element_in_minv_file
    SR_PROTOTYPE((FILE *fp,
                  struct OID_INFO *ptr));

extern void
emit_last_sid_array(FILE *fp, struct OID_INFO *ptr);

extern int
OpenIndexIfdefs
    SR_PROTOTYPE((FILE *fp,
                  struct OID_INFO *ptr));

extern int
MatchIndexIfdefs
    SR_PROTOTYPE((FILE *fp,
                  struct OID_INFO *ptr,
                  int match_type));

/* values for match_type */
#define MII_ELSE   1
#define MII_ENDIF  2

int
IsValidFamily
    SR_PROTOTYPE((struct OID_INFO *ptr));

int
IsReadableFamily
    SR_PROTOTYPE((struct OID_INFO *ptr));

int
HasNetworkAddressInFamily
    SR_PROTOTYPE((struct OID_INFO *ptr));

int
HasNetworkAddressInIndex
    SR_PROTOTYPE((struct OID_INFO *ptr));

int
map_variable_type_for_family
    SR_PROTOTYPE((char *type,
                  char *buf));

void output_c_make_file
    SR_PROTOTYPE((char *base));

char *hexify_string(char *str);

char *substitute_rsval(struct OID_INFO *p);

/*
 * prototypes for minimal v_ routines
 */
extern void emit_minv_tables(FILE *fp, OID_INFO_t *sorted_root);
extern void emit_minv_test(FILE *fp);
extern void emit_minv_get(FILE *fp);

/*
 * prototype for functions defined in shared.c
 */
struct OID_INFO *next_augmenting_table(struct OID_INFO *base_table_ptr,
                                       struct OID_INFO *aug_table_ptr);
struct OID_INFO *next_augmenting_object(struct OID_INFO *augments_list);
struct _index_array *next_unused_index(struct _index_array *index_array, 
                                       struct _index_array *temp_index,
                                       struct OID_INFO *ptr);
int IsGoodFamily(struct OID_INFO *ptr);

#ifdef  __cplusplus
}
#endif

#endif		/* SR_AGENT_H */
