#ifndef _PEM_MEMINFO_ACCESS_H_
#define _PEM_MEMINFO_ACCESS_H_

#include <stdio.h>
/*#include "pem_physical_access.h"*/
/*#include "pem_meminfo_init.h"*/
/*
 * The following defines the structure of the PEM DBs information
 * and function to access this information.
 *
 *
 * The Logical (Virtual) DBs to PEM-memory mapping data structure is made of a matrix
 * where the indexes of an element in this matrix are DB-ID and Field-ID.
 * Each element in this matrix is a matrix too. The inner matrix indexes are the row (key) ranges and
 * the columns (bits of field in the memory-chunk) range.
 * Each element in the inner matrix is a structure contains all the mapping information needed. 
 *
 * Notes:
 *   1. For registers matrix the inner matrix is actually one dimension elements (there is no row index)).
 *   2. For TCAM there are two matrices: one for the key-mask-valid mapping info and another one for the result data mapping info.
 *
 * In addition to the above, an array of DB info keep a global DB information per DB. This include:
 * DB name, ID
 * total key width and list of sub-key-field id and width
 * total result width and list of sub-result-field id and width
 * 
 */



#define UCODE_MAX_NOF_MEM_LINE  128000    /* 50 PEs x 5 Memories-per-PE x 32 lines-per-memory + 50 PEs X 2048 lines_per_sram (when dumping srams) */
#define UCODE_MAX_NOF_REG_LINE  512      /* 50 PEs x 10 regs-per-PE */

#define PEM_RANGE_LOW_LIMIT 0
#define PEM_RANGE_HIGH_LIMIT 1

#define DB_ROWS 0
#define DB_COLUMNS 1

#define MAX_NOF_FIELDS   16

#define IN 
#define OUT 

/* The following are the DB mapping info access functions */

void get_nof_entries(int db_id);
void qax_db_mapping_info_init(int nof_reg);
void qax_reg_mapping_info_init(int nof_dbs);
void qax_tcam_mapping_info_init(int nof_dbs);
void qax_em_mapping_info_init(int nof_dbs);
void qax_lpm_mapping_info_init(int nof_dbs);


/*
 * The following functions handle the micro-code upload.
 * The micro-code is first loaded (parsed) from a file and stored as a list
 * of uploading instructions. 
 * After complete parsing/loading the micro-code it than upload to the memory.
 */
void qax_ucode_init();
void qax_ucode_add(int unit, unsigned int mem_block_id, unsigned int mem_addr, unsigned int index, unsigned int *data, unsigned int data_length);
int  qax_ucode_nof_instruction_get();
int  qax_ucode_instruction_get(unsigned int instruction_no, unsigned int *unit, unsigned int *mem_block_id, unsigned int *mem_addr, 
                           unsigned int *row_offset, unsigned int **data, unsigned int *data_length);


#endif /* _PEM_MEMINFO_ACCESS_H_ */

